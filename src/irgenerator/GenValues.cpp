// Copyright (c) 2021-2023 ChilliBits. All rights reserved.

#include "IRGenerator.h"

#include <ast/ASTNodes.h>
#include <irgenerator/NameMangling.h>
#include <symboltablebuilder/ScopeHandle.h>
#include <symboltablebuilder/SymbolTableBuilder.h>

namespace spice::compiler {

std::any IRGenerator::visitValue(const ValueNode *node) {
  diGenerator.setSourceLocation(node);

  // Function call
  if (node->fctCall())
    return visit(node->fctCall());

  // Array initialization
  if (node->arrayInitialization())
    return visit(node->arrayInitialization());

  // Struct instantiation
  if (node->structInstantiation())
    return visit(node->structInstantiation());

  // Lambda function
  if (node->lambdaFunc())
    return visit(node->lambdaFunc());

  // Lambda procedure
  if (node->lambdaProc())
    return visit(node->lambdaProc());

  // Lambda expression
  if (node->lambdaExpr())
    return visit(node->lambdaExpr());

  if (node->isNil) {
    // Retrieve type of the nil constant
    auto nilType = any_cast<llvm::Type *>(visit(node->nilType()));
    // Create constant nil value
    llvm::Constant *nilValue = llvm::Constant::getNullValue(nilType);
    // Return it
    return LLVMExprResult{.constant = nilValue};
  }

  throw CompilerError(UNHANDLED_BRANCH, "Value fall-through"); // GCOV_EXCL_LINE
}

std::any IRGenerator::visitConstant(const ConstantNode *node) {
  return getConst(node->getCompileTimeValue(), node->getEvaluatedSymbolType(manIdx), node);
}

std::any IRGenerator::visitFctCall(const FctCallNode *node) {
  diGenerator.setSourceLocation(node);

  const FctCallNode::FctCallData &data = node->data.at(manIdx);

  Function *spiceFunc = data.callee;
  Scope *accessScope = data.calleeParentScope;
  std::string mangledName;
  if (!data.isFctPtrCall())
    mangledName = spiceFunc->getMangledName();
  std::vector<llvm::Value *> argValues;

  // Get entry of the first fragment
  SymbolTableEntry *firstFragEntry = currentScope->lookup(node->functionNameFragments.front());

  // Get this type
  llvm::Value *thisPtr = nullptr;
  if (data.isMethodCall()) {
    assert(!data.isCtorCall());

    // Retrieve entry of the first fragment
    assert(firstFragEntry != nullptr && firstFragEntry->getType().getBaseType().isOneOf({TY_STRUCT, TY_INTERFACE}));
    Scope *structScope = firstFragEntry->getType().getBaseType().getBodyScope();

    // Get address of the referenced variable / struct instance
    thisPtr = firstFragEntry->getAddress();

    // Auto de-reference 'this' pointer
    SymbolType firstFragmentType = firstFragEntry->getType();
    autoDeReferencePtr(thisPtr, firstFragmentType, structScope->parent);
    llvm::Type *structTy = firstFragEntry->getType().getBaseType().toLLVMType(context, structScope->parent);

    // Traverse through structs - the first fragment is already looked up and the last one is the function name
    for (size_t i = 1; i < node->functionNameFragments.size() - 1; i++) {
      const std::string identifier = node->functionNameFragments.at(i);
      // Retrieve field entry
      SymbolTableEntry *fieldEntry = structScope->lookupStrict(identifier);
      assert(fieldEntry != nullptr);
      SymbolType fieldEntryType = fieldEntry->getType();
      assert(fieldEntryType.isBaseType(TY_STRUCT));
      // Get struct type and scope
      structScope = fieldEntryType.getBaseType().getBodyScope();
      assert(structScope != nullptr);
      // Get address of field
      llvm::Value *indices[2] = {builder.getInt32(0), builder.getInt32(fieldEntry->orderIndex)};
      thisPtr = insertInBoundsGEP(structTy, thisPtr, indices);
      // Auto de-reference pointer and get new struct type
      autoDeReferencePtr(thisPtr, fieldEntryType, structScope->parent);
      structTy = fieldEntryType.getBaseType().toLLVMType(context, structScope->parent);
    }

    // Add 'this' pointer to the front of the argument list
    argValues.push_back(thisPtr);
  }

  if (data.isCtorCall()) {
    assert(!data.isMethodCall());

    llvm::Type *thisType = spiceFunc->thisType.toLLVMType(context, spiceFunc->thisType.getBodyScope());
    thisPtr = insertAlloca(thisType);

    // Add 'this' pointer to the front of the argument list
    argValues.push_back(thisPtr);
  }

  // If we have a lambda call that takes captures, add them to the argument list
  llvm::Value *fctPtr = nullptr;
  if (data.isFctPtrCall()) {
    llvm::Value *fatPtr = firstFragEntry->getAddress();
    // Load fctPtr
    llvm::StructType *fatStructType = llvm::StructType::get(context, {builder.getPtrTy(), builder.getPtrTy()});
    fctPtr = builder.CreateStructGEP(fatStructType, fatPtr, 0);
    if (firstFragEntry->getType().hasLambdaCaptures()) {
      // Load captures struct
      llvm::Value *capturesPtrPtr = builder.CreateStructGEP(fatStructType, fatPtr, 1);
      llvm::Value *capturesPtr = insertLoad(builder.getPtrTy(), capturesPtrPtr, CAPTURES_PARAM_NAME);
      // Add captures to argument list
      argValues.push_back(capturesPtr);
    }
  }

  // Get arg values
  if (node->hasArgs) {
    argValues.reserve(node->argLst()->args().size());
    const std::vector<AssignExprNode *> args = node->argLst()->args();
    const std::vector<SymbolType> paramSTypes =
        data.isFctPtrCall() ? firstFragEntry->getType().getBaseType().getFunctionParamTypes() : spiceFunc->getParamTypes();
    assert(paramSTypes.size() == args.size());
    for (size_t i = 0; i < args.size(); i++) {
      AssignExprNode *argNode = args.at(i);
      const SymbolType &expectedSTy = paramSTypes.at(i);
      const SymbolType &actualSTy = argNode->getEvaluatedSymbolType(manIdx);

      // If the arrays are both of size -1 or 0, they are both pointers and do not need to be cast implicitly
      if (expectedSTy.matches(actualSTy, false, true, true)) { // Matches the param type
        // Resolve address if actual type is reference, otherwise value
        llvm::Value *argValue = actualSTy.isRef() ? resolveAddress(argNode) : resolveValue(argNode);
        argValues.push_back(argValue);
      } else if (expectedSTy.isRef() && expectedSTy.getContainedTy().matches(actualSTy, false, true, true)) { // Matches with ref
        llvm::Value *argAddress = resolveAddress(argNode);
        argValues.push_back(argAddress);
      } else if (actualSTy.isRef() && expectedSTy.matches(actualSTy.getContainedTy(), false, true, true)) { // Matches with ref
        llvm::Value *argAddress = resolveValue(argNode);
        argValues.push_back(argAddress);
      } else { // Need implicit cast
        llvm::Value *argAddress = resolveAddress(argNode);
        argValues.push_back(doImplicitCast(argAddress, expectedSTy, actualSTy));
      }
    }
  }

  // Retrieve return and param types
  SymbolType returnSType(TY_DYN);
  std::vector<SymbolType> paramSTypes;
  if (data.isFctPtrCall()) {
    if (firstFragEntry->getType().isBaseType(TY_FUNCTION))
      returnSType = firstFragEntry->getType().getBaseType().getFunctionReturnType();
    paramSTypes = firstFragEntry->getType().getBaseType().getFunctionParamTypes();
  } else {
    returnSType = spiceFunc->returnType;
    paramSTypes = spiceFunc->getParamTypes();
  }

  // Function is not defined in the current module -> declare it
  llvm::FunctionType *fctType = nullptr;
  if (llvm::Function *fct = module->getFunction(mangledName)) {
    fctType = fct->getFunctionType();
  } else {
    // Get returnType
    llvm::Type *returnType = builder.getVoidTy();
    if (!returnSType.is(TY_DYN))
      returnType = returnSType.toLLVMType(context, accessScope);

    // Get arg types
    std::vector<llvm::Type *> argTypes;
    if (data.isMethodCall() || data.isCtorCall())
      argTypes.push_back(builder.getPtrTy());
    if (data.isFctPtrCall() && firstFragEntry->getType().hasLambdaCaptures())
      argTypes.push_back(builder.getPtrTy());
    for (const SymbolType &paramType : paramSTypes)
      argTypes.push_back(paramType.toLLVMType(context, accessScope));

    fctType = llvm::FunctionType::get(returnType, argTypes, false);
    if (!data.isFctPtrCall() && !data.isVirtualMethodCall())
      module->getOrInsertFunction(mangledName, fctType);
  }
  assert(fctType != nullptr);

  llvm::CallInst *result;
  if (data.isVirtualMethodCall()) {
    assert(data.callee->isVirtual);
    assert(thisPtr != nullptr);
    // Load VTable
    llvm::Value *vtablePtr = insertLoad(builder.getPtrTy(), thisPtr, "vtable.addr");
    const size_t vtableIndex = data.callee->vtableIndex;
    // Lookup function pointer in VTable
    fctPtr = insertInBoundsGEP(builder.getPtrTy(), vtablePtr, builder.getInt64(vtableIndex), "vfct.addr");
    llvm::Value *fct = insertLoad(builder.getPtrTy(), fctPtr, "fct");

    // Generate function call
    result = builder.CreateCall({fctType, fct}, argValues);
  } else if (data.isFctPtrCall()) {
    assert(firstFragEntry != nullptr);
    SymbolType firstFragType = firstFragEntry->getType();
    if (!fctPtr)
      fctPtr = firstFragEntry->getAddress();
    autoDeReferencePtr(fctPtr, firstFragType, currentScope);
    llvm::Value *fct = insertLoad(builder.getPtrTy(), fctPtr, "fct");

    // Generate function call
    result = builder.CreateCall({fctType, fct}, argValues);
  } else {
    // Get callee function
    llvm::Function *callee = module->getFunction(mangledName);
    assert(callee != nullptr);

    // Generate function call
    result = builder.CreateCall(callee, argValues);
  }

  if (data.isMethodCall() || data.isCtorCall() || data.isVirtualMethodCall()) {
    llvm::Type *thisType = data.thisType.toLLVMType(context, currentScope);
    result->addParamAttr(0, llvm::Attribute::NoUndef);
    result->addParamAttr(0, llvm::Attribute::NonNull);
    result->addDereferenceableParamAttr(0, module->getDataLayout().getTypeStoreSize(thisType));
    result->addParamAttr(0, llvm::Attribute::getWithAlignment(context, module->getDataLayout().getABITypeAlign(thisType)));
  }

  // Attach address to anonymous symbol to keep track of deallocation
  SymbolTableEntry *anonymousSymbol = nullptr;
  llvm::Value *resultPtr = nullptr;
  if (returnSType.is(TY_STRUCT) || data.isCtorCall()) {
    anonymousSymbol = currentScope->symbolTable.lookupAnonymous(node->codeLoc);
    if (anonymousSymbol != nullptr) {
      if (data.isCtorCall()) {
        anonymousSymbol->updateAddress(thisPtr);
      } else {
        resultPtr = insertAlloca(result->getType());
        builder.CreateStore(result, resultPtr);
        anonymousSymbol->updateAddress(resultPtr);
      }
    }
  }

  // In case this is a constructor call, return the thisPtr as pointer
  if (data.isCtorCall())
    return LLVMExprResult{.ptr = thisPtr, .refPtr = resultPtr, .entry = anonymousSymbol};

  // In case this is a callee, returning a reference, return the address
  if (returnSType.isRef())
    return LLVMExprResult{.ptr = result, .refPtr = resultPtr, .entry = anonymousSymbol};

  // Otherwise return the value
  return LLVMExprResult{.value = result, .ptr = resultPtr, .entry = anonymousSymbol};
}

std::any IRGenerator::visitArrayInitialization(const ArrayInitializationNode *node) {
  // Return immediately if the initialization is empty
  if (node->actualSize == 0)
    return LLVMExprResult{.node = node};

  // Visit array items
  bool canBeConstant = true;
  std::vector<LLVMExprResult> itemResults;
  itemResults.reserve(node->actualSize);
  for (AssignExprNode *itemNode : node->itemLst()->args()) {
    auto item = std::any_cast<LLVMExprResult>(visit(itemNode));
    canBeConstant &= item.constant != nullptr;
    item.node = itemNode;
    itemResults.push_back(item);
  }

  // Get LLVM type of item and array
  assert(!itemResults.empty());
  const SymbolType &firstItemSTy = node->itemLst()->args().front()->getEvaluatedSymbolType(manIdx);
  llvm::Type *itemType = firstItemSTy.toLLVMType(context, currentScope);
  llvm::ArrayType *arrayType = llvm::ArrayType::get(itemType, node->actualSize);

  if (canBeConstant) { // All items are constants, so we can create a global constant array
    // Collect constants
    std::vector<llvm::Constant *> constants;
    for (const LLVMExprResult &exprResult : itemResults) {
      // Delete potential constant globals, that were already created a layer below
      if (exprResult.constant->getType()->isArrayTy())
        module->getNamedGlobal(exprResult.ptr->getName())->eraseFromParent();
      constants.push_back(exprResult.constant);
    }

    // Create global array
    llvm::Constant *constantArray = llvm::ConstantArray::get(arrayType, constants);
    llvm::Value *arrayAddr = createGlobalConst(ANON_GLOBAL_ARRAY_NAME, constantArray);

    return LLVMExprResult{.constant = constantArray, .ptr = arrayAddr};
  } else { // We have non-immediate values as items, so we need to take normal arrays as fallback
    llvm::Value *arrayAddr = insertAlloca(arrayType);

    // Retrieve address of first item
    llvm::Value *firstItemAddress = insertInBoundsGEP(arrayType, arrayAddr, builder.getInt32(0));

    // Store all array items at their corresponding offsets
    llvm::Value *currentItemAddress = firstItemAddress;
    for (size_t i = 0; i < itemResults.size(); i++) {
      LLVMExprResult &exprResult = itemResults[i];
      llvm::Value *itemValue = resolveValue(exprResult.node, exprResult);
      // Retrieve current item address
      if (i >= 1)
        currentItemAddress = insertInBoundsGEP(itemType, currentItemAddress, builder.getInt32(1));
      // Store the item value
      const bool storeVolatile = exprResult.entry != nullptr && exprResult.entry->isVolatile;
      builder.CreateStore(itemValue, currentItemAddress, storeVolatile);
    }

    return LLVMExprResult{.ptr = arrayAddr};
  }
}

std::any IRGenerator::visitStructInstantiation(const StructInstantiationNode *node) {
  // Get struct object
  const Struct *spiceStruct = node->instantiatedStructs.at(manIdx);
  assert(spiceStruct != nullptr);
  const std::vector<SymbolType> &fieldTypes = spiceStruct->fieldTypes;

  // Can only be constant if none of the fields is of type reference
  bool canBeConstant = !spiceStruct->hasReferenceFields();

  // Get struct type
  assert(spiceStruct->entry != nullptr);
  auto structType = reinterpret_cast<llvm::StructType *>(spiceStruct->entry->getType().toLLVMType(context, currentScope));
  assert(structType != nullptr);

  if (!node->fieldLst()) {
    llvm::Constant *constantStruct = getDefaultValueForSymbolType(spiceStruct->entry->getType());
    return LLVMExprResult{.constant = constantStruct};
  }

  // Visit struct field values
  std::vector<LLVMExprResult> fieldValueResults;
  fieldValueResults.reserve(spiceStruct->fieldTypes.size());
  for (AssignExprNode *fieldValueNode : node->fieldLst()->args()) {
    auto fieldValue = std::any_cast<LLVMExprResult>(visit(fieldValueNode));
    fieldValue.node = fieldValueNode;
    fieldValueResults.push_back(fieldValue);
    canBeConstant &= fieldValue.constant != nullptr;
  }

  if (canBeConstant) { // All field values are constants, so we can create a global constant struct instantiation
    // Collect constants
    std::vector<llvm::Constant *> constants;
    // For each interface a nullptr
    for (const SymbolType &interfaceType : spiceStruct->interfaceTypes)
      constants.push_back(getDefaultValueForSymbolType(interfaceType));
    // Constant value for each field
    for (const LLVMExprResult &exprResult : fieldValueResults)
      constants.push_back(exprResult.constant);

    // Create global constant struct
    llvm::Constant *constantStruct = llvm::ConstantStruct::get(structType, constants);

    return LLVMExprResult{.constant = constantStruct};
  } else { // We have at least one non-immediate value, so we need to take normal struct instantiation as fallback
    llvm::Value *structAddr = insertAlloca(structType);
    const size_t interfaceCount = spiceStruct->interfaceTypes.size();
    const size_t fieldCount = spiceStruct->fieldTypes.size();
    size_t i = 0;

    // Store interface values at their corresponding offsets
    for (; i < interfaceCount; i++) {
      const SymbolType &interfaceType = spiceStruct->interfaceTypes.at(i);
      // Get field value
      llvm::Value *itemValue = getDefaultValueForSymbolType(interfaceType);
      // Get field address
      llvm::Value *currentFieldAddress = builder.CreateStructGEP(structType, structAddr, i);
      // Store the item value
      builder.CreateStore(itemValue, currentFieldAddress);
    }

    // Store all field values at their corresponding offsets
    for (; i < interfaceCount + fieldCount; i++) {
      LLVMExprResult &exprResult = fieldValueResults.at(i);
      // Get field value
      llvm::Value *itemValue = fieldTypes.at(i).isRef() ? resolveAddress(exprResult) : resolveValue(exprResult.node, exprResult);
      // Get field address
      llvm::Value *currentFieldAddress = builder.CreateStructGEP(structType, structAddr, i);
      // Store the item value
      const bool storeVolatile = exprResult.entry != nullptr && exprResult.entry->isVolatile;
      builder.CreateStore(itemValue, currentFieldAddress, storeVolatile);
    }

    // Attach address to anonymous symbol to keep track of deallocation
    SymbolTableEntry *returnSymbol = currentScope->symbolTable.lookupAnonymous(node->codeLoc);
    if (returnSymbol != nullptr)
      returnSymbol->updateAddress(structAddr);

    return LLVMExprResult{.ptr = structAddr};
  }
}

std::any IRGenerator::visitLambdaFunc(const LambdaFuncNode *node) {
  Function spiceFunc = node->manifestations.at(manIdx);
  ParamInfoList paramInfoList;
  std::vector<llvm::Type *> paramTypes;

  // Change scope
  Scope *bodyScope = currentScope = currentScope->getChildScope(node->getScopeId());

  // If there are captures, we pass them in a struct as the first function argument
  const std::unordered_map<std::string, Capture> &captures = bodyScope->symbolTable.captures;
  const bool hasCaptures = !captures.empty();
  llvm::StructType *capturesStructType = nullptr;
  if (hasCaptures) {
    const CaptureMode capturingMode = bodyScope->symbolTable.capturingMode;
    // Create captures struct type
    std::vector<llvm::Type *> captureTypes;
    for (const std::pair<const std::string, Capture> &capture : captures) {
      llvm::Type *captureType = builder.getPtrTy();
      if (capturingMode == BY_VALUE)
        captureType = capture.second.capturedEntry->getType().toLLVMType(context, currentScope);
      captureTypes.push_back(captureType);
    }
    capturesStructType = llvm::StructType::get(context, captureTypes);
    // Add the captures struct as first parameter
    paramInfoList.emplace_back(CAPTURES_PARAM_NAME, nullptr);
    paramTypes.push_back(builder.getPtrTy()); // The capture struct is always passed as pointer
  }

  // Visit parameters
  size_t argIdx = 0;
  if (node->hasParams) {
    const size_t numOfParams = spiceFunc.paramList.size();
    paramInfoList.reserve(numOfParams);
    paramTypes.reserve(numOfParams);
    for (; argIdx < numOfParams; argIdx++) {
      const DeclStmtNode *param = node->paramLst()->params().at(argIdx);
      // Get symbol table entry of param
      SymbolTableEntry *paramSymbol = currentScope->lookupStrict(param->varName);
      assert(paramSymbol != nullptr);
      // Retrieve type of param
      llvm::Type *paramType = spiceFunc.getParamTypes().at(argIdx).toLLVMType(context, currentScope);
      // Add it to the lists
      paramInfoList.emplace_back(param->varName, paramSymbol);
      paramTypes.push_back(paramType);
    }
  }

  // Get return type
  llvm::Type *returnType = spiceFunc.returnType.toLLVMType(context, currentScope);

  // Create function or implement declared function
  spiceFunc.mangleSuffix = "." + std::to_string(manIdx);
  const std::string mangledName = spiceFunc.getMangledName();
  llvm::FunctionType *funcType = llvm::FunctionType::get(returnType, paramTypes, false);
  module->getOrInsertFunction(mangledName, funcType);
  llvm::Function *lambda = module->getFunction(mangledName);

  // Set attributes to function
  lambda->setDSOLocal(true);
  lambda->setLinkage(llvm::Function::PrivateLinkage);

  // In case of captures, add attribute to captures argument
  if (hasCaptures) {
    lambda->addParamAttr(0, llvm::Attribute::NoUndef);
    lambda->addParamAttr(0, llvm::Attribute::NonNull);
    lambda->addDereferenceableParamAttr(0, module->getDataLayout().getPointerSize());
  }

  // Add debug info
  diGenerator.generateFunctionDebugInfo(lambda, &spiceFunc, true);
  diGenerator.setSourceLocation(node);

  // Save alloca insert markers
  llvm::BasicBlock *allocaInsertBlockOrig = allocaInsertBlock;
  llvm::Instruction *allocaInsertInstOrig = allocaInsertInst;
  llvm::BasicBlock *bOrig = builder.GetInsertBlock();

  // Create entry block
  llvm::BasicBlock *bEntry = createBlock();
  switchToBlock(bEntry, lambda);

  // Reset alloca insert markers to this block
  allocaInsertBlock = bEntry;
  allocaInsertInst = nullptr;

  // Declare result variable
  SymbolTableEntry *resultEntry = currentScope->lookupStrict(RETURN_VARIABLE_NAME);
  assert(resultEntry != nullptr);
  llvm::Value *resultAddr = insertAlloca(returnType, RETURN_VARIABLE_NAME);
  resultEntry->updateAddress(resultAddr);
  // Generate debug info
  diGenerator.generateLocalVarDebugInfo(RETURN_VARIABLE_NAME, resultAddr, SIZE_MAX);

  // Store function argument values
  llvm::Value *captureStructPtrPtr = nullptr;
  for (auto &arg : lambda->args()) {
    // Get parameter info
    const size_t argNumber = arg.getArgNo();
    auto [paramName, paramSymbol] = paramInfoList.at(argNumber);
    // Allocate space for it
    llvm::Type *paramType = funcType->getParamType(argNumber);
    llvm::Value *paramAddress = insertAlloca(paramType, paramName);
    // Update the symbol table entry
    if (hasCaptures && argNumber == 0) {
      captureStructPtrPtr = paramAddress;
    } else {
      paramSymbol->updateAddress(paramAddress);
      // Generate debug info
      diGenerator.generateLocalVarDebugInfo(paramName, paramAddress, argNumber + 1);
    }
    // Store the value at the new address
    builder.CreateStore(&arg, paramAddress);
  }

  // Store the default values for optional function args
  if (node->paramLst()) {
    const std::vector<DeclStmtNode *> params = node->paramLst()->params();
    for (; argIdx < params.size(); argIdx++)
      visit(params.at(argIdx));
  }

  // Extract captures from captures struct
  if (hasCaptures) {
    assert(!paramInfoList.empty());
    llvm::Value *captureStructPtr = insertLoad(builder.getPtrTy(), captureStructPtrPtr);
    size_t captureIdx = 0;
    for (const std::pair<const std::string, Capture> &capture : captures) {
      llvm::Value *captureAddress = builder.CreateStructGEP(capturesStructType, captureStructPtr, captureIdx);
      captureAddress->setName(capture.second.getName());
      capture.second.capturedEntry->pushAddress(captureAddress);
      // Generate debug info
      diGenerator.generateLocalVarDebugInfo(capture.second.getName(), captureAddress);
      captureIdx++;
    }
  }

  visit(node->body());

  // Create return statement if the block is not terminated yet
  if (!blockAlreadyTerminated) {
    llvm::Value *result = insertLoad(returnType, resultEntry->getAddress());
    builder.CreateRet(result);
  }

  // Pop capture addresses
  if (hasCaptures)
    for (const std::pair<const std::string, Capture> &capture : captures)
      capture.second.capturedEntry->popAddress();

  // Conclude debug info for function
  diGenerator.concludeFunctionDebugInfo();
  diGenerator.setSourceLocation(node);

  // Restore alloca insert markers
  builder.SetInsertPoint(bOrig);
  blockAlreadyTerminated = false;
  allocaInsertBlock = allocaInsertBlockOrig;
  allocaInsertInst = allocaInsertInstOrig;

  // Change back to original scope
  currentScope = currentScope->parent;

  // Verify function
  verifyFunction(lambda, node->codeLoc);

  // Captures, create a struct { <fct-ptr>, <capture struct ptr> }
  llvm::Value *result = buildFatFctPtr(bodyScope, capturesStructType, lambda);

  return LLVMExprResult{.ptr = result, .node = node};
}

std::any IRGenerator::visitLambdaProc(const LambdaProcNode *node) {
  Function spiceFunc = node->manifestations.at(manIdx);
  ParamInfoList paramInfoList;
  std::vector<llvm::Type *> paramTypes;

  // Change scope
  Scope *bodyScope = currentScope = currentScope->getChildScope(node->getScopeId());

  // If there are captures, we pass them in a struct as the first function argument
  const std::unordered_map<std::string, Capture> &captures = bodyScope->symbolTable.captures;
  const bool hasCaptures = !captures.empty();
  llvm::StructType *capturesStructType = nullptr;
  if (hasCaptures) {
    const CaptureMode capturingMode = bodyScope->symbolTable.capturingMode;
    // Create captures struct type
    std::vector<llvm::Type *> captureTypes;
    for (const std::pair<const std::string, Capture> &capture : captures) {
      llvm::Type *captureType = builder.getPtrTy();
      if (capturingMode == BY_VALUE)
        captureType = capture.second.capturedEntry->getType().toLLVMType(context, currentScope);
      captureTypes.push_back(captureType);
    }
    capturesStructType = llvm::StructType::get(context, captureTypes);
    // Add the captures struct as first parameter
    paramInfoList.emplace_back(CAPTURES_PARAM_NAME, nullptr);
    paramTypes.push_back(builder.getPtrTy()); // The captures struct is always passed as pointer
  }

  // Visit parameters
  size_t argIdx = 0;
  if (node->hasParams) {
    const size_t numOfParams = spiceFunc.paramList.size();
    paramInfoList.reserve(numOfParams);
    paramTypes.reserve(numOfParams);
    for (; argIdx < numOfParams; argIdx++) {
      const DeclStmtNode *param = node->paramLst()->params().at(argIdx);
      // Get symbol table entry of param
      SymbolTableEntry *paramSymbol = currentScope->lookupStrict(param->varName);
      assert(paramSymbol != nullptr);
      // Retrieve type of param
      llvm::Type *paramType = spiceFunc.getParamTypes().at(argIdx).toLLVMType(context, currentScope);
      // Add it to the lists
      paramInfoList.emplace_back(param->varName, paramSymbol);
      paramTypes.push_back(paramType);
    }
  }

  // Create function or implement declared function
  spiceFunc.mangleSuffix = "." + std::to_string(manIdx);
  const std::string mangledName = spiceFunc.getMangledName();
  llvm::FunctionType *funcType = llvm::FunctionType::get(builder.getVoidTy(), paramTypes, false);
  module->getOrInsertFunction(mangledName, funcType);
  llvm::Function *lambda = module->getFunction(mangledName);

  // Set attributes to function
  lambda->setDSOLocal(true);
  lambda->setLinkage(llvm::Function::PrivateLinkage);

  // In case of captures, add attribute to captures argument
  if (hasCaptures) {
    lambda->addParamAttr(0, llvm::Attribute::NoUndef);
    lambda->addParamAttr(0, llvm::Attribute::NonNull);
    lambda->addDereferenceableParamAttr(0, module->getDataLayout().getPointerSize());
  }

  // Add debug info
  diGenerator.generateFunctionDebugInfo(lambda, &spiceFunc, true);
  diGenerator.setSourceLocation(node);

  // Save alloca insert markers
  llvm::BasicBlock *allocaInsertBlockOrig = allocaInsertBlock;
  llvm::Instruction *allocaInsertInstOrig = allocaInsertInst;
  llvm::BasicBlock *bOrig = builder.GetInsertBlock();

  // Create entry block
  llvm::BasicBlock *bEntry = createBlock();
  switchToBlock(bEntry, lambda);

  // Reset alloca insert markers to this block
  allocaInsertBlock = bEntry;
  allocaInsertInst = nullptr;

  // Save values of parameters to locals
  llvm::Value *captureStructPtrPtr = nullptr;
  for (auto &arg : lambda->args()) {
    // Get information about the parameter
    const size_t argNumber = arg.getArgNo();
    auto [paramName, paramSymbol] = paramInfoList.at(argNumber);
    // Allocate space for it
    llvm::Type *paramType = funcType->getParamType(argNumber);
    llvm::Value *paramAddress = insertAlloca(paramType, paramName);
    // Update the symbol table entry
    if (hasCaptures && argNumber == 0) {
      captureStructPtrPtr = paramAddress;
    } else {
      paramSymbol->updateAddress(paramAddress);
      // Generate debug info
      diGenerator.generateLocalVarDebugInfo(paramName, paramAddress, argNumber + 1);
    }
    // Store the value at the new address
    builder.CreateStore(&arg, paramAddress);
  }

  // Store the default values for optional function args
  if (node->paramLst()) {
    const std::vector<DeclStmtNode *> params = node->paramLst()->params();
    for (; argIdx < params.size(); argIdx++)
      visit(params.at(argIdx));
  }

  // Extract captures from captures struct
  if (hasCaptures) {
    assert(!paramInfoList.empty());
    llvm::Value *captureStructPtr = insertLoad(builder.getPtrTy(), captureStructPtrPtr);
    size_t captureIdx = 0;
    for (const std::pair<const std::string, Capture> &capture : captures) {
      llvm::Value *captureAddress = builder.CreateStructGEP(capturesStructType, captureStructPtr, captureIdx);
      captureAddress->setName(capture.second.getName());
      capture.second.capturedEntry->pushAddress(captureAddress);
      // Generate debug info
      diGenerator.generateLocalVarDebugInfo(capture.second.getName(), captureAddress);
      captureIdx++;
    }
  }

  visit(node->body());

  // Create return statement if the block is not terminated yet
  if (!blockAlreadyTerminated)
    builder.CreateRetVoid();

  // Pop capture addresses
  if (hasCaptures)
    for (const std::pair<const std::string, Capture> &capture : captures)
      capture.second.capturedEntry->popAddress();

  // Conclude debug info for function
  diGenerator.concludeFunctionDebugInfo();
  diGenerator.setSourceLocation(node);

  // Restore alloca insert markers
  builder.SetInsertPoint(bOrig);
  blockAlreadyTerminated = false;
  allocaInsertBlock = allocaInsertBlockOrig;
  allocaInsertInst = allocaInsertInstOrig;

  // Change back to original scope
  currentScope = currentScope->parent;

  // Verify function
  verifyFunction(lambda, node->codeLoc);

  // Create a struct { <fct-ptr>, <capture struct ptr> }
  llvm::Value *result = buildFatFctPtr(bodyScope, capturesStructType, lambda);

  return LLVMExprResult{.ptr = result, .node = node};
}

std::any IRGenerator::visitLambdaExpr(const LambdaExprNode *node) {
  const Function &spiceFunc = node->manifestations.at(manIdx);
  ParamInfoList paramInfoList;
  std::vector<llvm::Type *> paramTypes;

  // Change scope
  Scope *bodyScope = currentScope = currentScope->getChildScope(node->getScopeId());

  // If there are captures, we pass them in a struct as the first function argument
  const std::unordered_map<std::string, Capture> &captures = bodyScope->symbolTable.captures;
  const bool hasCaptures = !captures.empty();
  llvm::StructType *capturesStructType = nullptr;
  if (hasCaptures) {
    const CaptureMode capturingMode = bodyScope->symbolTable.capturingMode;
    // Create captures struct type
    std::vector<llvm::Type *> captureTypes;
    for (const std::pair<const std::string, Capture> &capture : captures) {
      llvm::Type *captureType = builder.getPtrTy();
      if (capturingMode == BY_VALUE)
        captureType = capture.second.capturedEntry->getType().toLLVMType(context, currentScope);
      captureTypes.push_back(captureType);
    }
    capturesStructType = llvm::StructType::get(context, captureTypes);
    // Add the captures struct as first parameter
    paramInfoList.emplace_back(CAPTURES_PARAM_NAME, nullptr);
    paramTypes.push_back(builder.getPtrTy()); // The capture struct is always passed as pointer
  }

  // Visit parameters
  size_t argIdx = 0;
  if (node->hasParams) {
    const size_t numOfParams = spiceFunc.paramList.size();
    paramInfoList.reserve(numOfParams);
    paramTypes.reserve(numOfParams);
    for (; argIdx < numOfParams; argIdx++) {
      const DeclStmtNode *param = node->paramLst()->params().at(argIdx);
      // Get symbol table entry of param
      SymbolTableEntry *paramSymbol = currentScope->lookupStrict(param->varName);
      assert(paramSymbol != nullptr);
      // Retrieve type of param
      llvm::Type *paramType = spiceFunc.getParamTypes().at(argIdx).toLLVMType(context, currentScope);
      // Add it to the lists
      paramInfoList.emplace_back(param->varName, paramSymbol);
      paramTypes.push_back(paramType);
    }
  }

  // Get return type
  llvm::Type *returnType = builder.getVoidTy();
  if (spiceFunc.isFunction())
    returnType = spiceFunc.returnType.toLLVMType(context, currentScope);

  // Create function or implement declared function
  const std::string mangledName = spiceFunc.getMangledName();
  llvm::FunctionType *funcType = llvm::FunctionType::get(returnType, paramTypes, false);
  module->getOrInsertFunction(mangledName, funcType);
  llvm::Function *lambda = module->getFunction(mangledName);

  // Set attributes to function
  lambda->setDSOLocal(true);
  lambda->setLinkage(llvm::Function::PrivateLinkage);

  // In case of captures, add attribute to captures argument
  if (hasCaptures) {
    lambda->addParamAttr(0, llvm::Attribute::NoUndef);
    lambda->addParamAttr(0, llvm::Attribute::NonNull);
    lambda->addDereferenceableParamAttr(0, module->getDataLayout().getPointerSize());
  }

  // Add debug info
  diGenerator.generateFunctionDebugInfo(lambda, &spiceFunc, true);
  diGenerator.setSourceLocation(node);

  // Save alloca insert markers
  llvm::BasicBlock *allocaInsertBlockOrig = allocaInsertBlock;
  llvm::Instruction *allocaInsertInstOrig = allocaInsertInst;
  llvm::BasicBlock *bOrig = builder.GetInsertBlock();

  // Create entry block
  llvm::BasicBlock *bEntry = createBlock();
  switchToBlock(bEntry, lambda);

  // Reset alloca insert markers to this block
  allocaInsertBlock = bEntry;
  allocaInsertInst = nullptr;

  // Save values of parameters to locals
  llvm::Value *captureStructPtrPtr = nullptr;
  for (auto &arg : lambda->args()) {
    // Get information about the parameter
    const size_t argNumber = arg.getArgNo();
    auto [paramName, paramSymbol] = paramInfoList.at(argNumber);
    // Allocate space for it
    llvm::Type *paramType = funcType->getParamType(argNumber);
    llvm::Value *paramAddress = insertAlloca(paramType, paramName);
    // Update the symbol table entry
    if (hasCaptures && argNumber == 0)
      captureStructPtrPtr = paramAddress;
    else
      paramSymbol->updateAddress(paramAddress);
    // Generate debug info
    diGenerator.generateLocalVarDebugInfo(paramName, paramAddress, argNumber + 1);
    // Store the value at the new address
    builder.CreateStore(&arg, paramAddress);
  }

  // Store the default values for optional function args
  if (node->paramLst()) {
    const std::vector<DeclStmtNode *> params = node->paramLst()->params();
    for (; argIdx < params.size(); argIdx++)
      visit(params.at(argIdx));
  }

  // Extract captures from captures struct
  if (hasCaptures) {
    assert(!paramInfoList.empty());
    llvm::Value *captureStructPtr = insertLoad(builder.getPtrTy(), captureStructPtrPtr);
    size_t captureIdx = 0;
    for (const std::pair<const std::string, Capture> &capture : captures) {
      llvm::Value *captureAddress = builder.CreateStructGEP(capturesStructType, captureStructPtr, captureIdx);
      captureAddress->setName(capture.second.getName());
      capture.second.capturedEntry->pushAddress(captureAddress);
      // Generate debug info
      diGenerator.generateLocalVarDebugInfo(capture.second.getName(), captureAddress);
      captureIdx++;
    }
  }

  // Visit lambda expression
  llvm::Value *exprResult = resolveValue(node->lambdaExpr(), currentScope);
  builder.CreateRet(exprResult);

  // Pop capture addresses
  if (hasCaptures)
    for (const std::pair<const std::string, Capture> &capture : captures)
      capture.second.capturedEntry->popAddress();

  // Conclude debug info for function
  diGenerator.concludeFunctionDebugInfo();
  diGenerator.setSourceLocation(node);

  // Restore alloca insert markers
  builder.SetInsertPoint(bOrig);
  blockAlreadyTerminated = false;
  allocaInsertBlock = allocaInsertBlockOrig;
  allocaInsertInst = allocaInsertInstOrig;

  // Change back to original scope
  currentScope = currentScope->parent;

  // Verify function
  verifyFunction(lambda, node->codeLoc);
  // Create a struct { <fct-ptr>, <capture struct ptr> }
  llvm::Value *result = buildFatFctPtr(bodyScope, capturesStructType, lambda);

  return LLVMExprResult{.ptr = result, .node = node};
}

std::any IRGenerator::visitDataType(const DataTypeNode *node) {
  // Only set the source location if this is not the root scope
  if (currentScope != rootScope && !node->isParamType && !node->isReturnType && !node->isFieldType)
    diGenerator.setSourceLocation(node);
  // Retrieve symbol type
  SymbolType symbolType = node->getEvaluatedSymbolType(manIdx);
  assert(!symbolType.is(TY_DYN)); // Symbol type should not be dyn anymore at this point
  return symbolType.toLLVMType(context, currentScope);
}

llvm::Value *IRGenerator::buildFatFctPtr(Scope *bodyScope, llvm::StructType *capturesStructType, llvm::Value *lambda) {
  // Create capture struct if required
  llvm::Value *captureStructAddress = nullptr;
  if (capturesStructType != nullptr) {
    assert(bodyScope != nullptr);
    const CaptureMode capturingMode = bodyScope->symbolTable.capturingMode;
    captureStructAddress = insertAlloca(capturesStructType, CAPTURES_PARAM_NAME);

    size_t captureIdx = 0;
    for (const std::pair<const std::string, Capture> &capture : bodyScope->symbolTable.captures) {
      const SymbolTableEntry *capturedEntry = capture.second.capturedEntry;
      // Get address or value of captured variable, depending on the capturing mode
      llvm::Value *capturedValue = capturedEntry->getAddress();
      assert(capturedValue != nullptr);
      if (capturingMode == BY_VALUE) {
        llvm::Type *captureType = capturedEntry->getType().toLLVMType(context, currentScope);
        capturedValue = insertLoad(captureType, capturedValue);
      }
      // Store it in the capture struct
      llvm::Value *captureAddress = builder.CreateStructGEP(capturesStructType, captureStructAddress, captureIdx);
      builder.CreateStore(capturedValue, captureAddress);
      captureIdx++;
    }
  }

  // Create fat ptr struct type if not exists yet
  if (!llvmTypes.fatPtrType)
    llvmTypes.fatPtrType = llvm::StructType::get(context, {builder.getPtrTy(), builder.getPtrTy()});

  // Create fat pointer
  llvm::Value *fatFctPtr = insertAlloca(llvmTypes.fatPtrType, "fat.ptr");
  llvm::Value *fctPtr = builder.CreateStructGEP(llvmTypes.fatPtrType, fatFctPtr, 0);
  builder.CreateStore(lambda, fctPtr);
  if (capturesStructType != nullptr) {
    llvm::Value *capturePtr = builder.CreateStructGEP(llvmTypes.fatPtrType, fatFctPtr, 1);
    builder.CreateStore(captureStructAddress, capturePtr);
  }

  return fatFctPtr;
}

} // namespace spice::compiler