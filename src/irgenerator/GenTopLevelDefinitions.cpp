// Copyright (c) 2021-2023 ChilliBits. All rights reserved.

#include "IRGenerator.h"

#include <SourceFile.h>
#include <ast/ASTNodes.h>
#include <symboltablebuilder/SymbolTableBuilder.h>

namespace spice::compiler {

std::any IRGenerator::visitMainFctDef(const MainFctDefNode *node) {
  // Ignore main function definitions if this is not the main source file
  if (!sourceFile->mainFile)
    return nullptr;

  // Do not generate main function if it is explicitly specified
  if (resourceManager.cliOptions.noEntryFct)
    return nullptr;

  // Change scope to function scope
  currentScope = node->fctScope;
  assert(currentScope != nullptr);

  // Visit parameters
  std::vector<std::string> paramNames;
  std::vector<SymbolTableEntry *> paramSymbols;
  std::vector<SymbolType> paramSymbolTypes;
  std::vector<llvm::Type *> paramTypes;
  if (node->takesArgs) {
    const size_t numOfParams = node->paramLst()->params().size();
    paramNames.reserve(numOfParams);
    paramSymbols.reserve(numOfParams);
    paramSymbolTypes.reserve(numOfParams);
    paramTypes.reserve(numOfParams);
    for (DeclStmtNode *param : node->paramLst()->params()) {
      // Get symbol table entry of param
      SymbolTableEntry *paramSymbol = node->fctScope->lookupStrict(param->varName);
      assert(paramSymbol != nullptr);
      // Retrieve type of param
      auto paramType = any_cast<llvm::Type *>(visit(param->dataType()));
      // Add it to the lists
      paramNames.push_back(param->varName);
      paramSymbols.push_back(paramSymbol);
      paramSymbolTypes.push_back(paramSymbol->getType());
      paramTypes.push_back(paramType);
    }
  }

  // Build the function
  llvm::Type *returnType = builder.getInt32Ty();
  llvm::FunctionType *fctType = llvm::FunctionType::get(returnType, paramTypes, false);
  llvm::Function *fct = llvm::Function::Create(fctType, llvm::Function::ExternalLinkage, MAIN_FUNCTION_NAME, module);
  fct->setDSOLocal(true);

  // Add function attributes
  fct->addFnAttr(llvm::Attribute::NoInline);
  fct->addFnAttr(llvm::Attribute::NoUnwind);
  if (cliOptions.optLevel == O0)
    fct->addFnAttr(llvm::Attribute::OptimizeNone);
  else if (cliOptions.optLevel >= Os)
    fct->addFnAttr(llvm::Attribute::OptimizeForSize);
  fct->addFnAttr(llvm::Attribute::getWithUWTableKind(context, llvm::UWTableKind::Default));

  // Add debug info
  if (cliOptions.generateDebugInfo) {
    auto nonConstNode = const_cast<MainFctDefNode *>(node);
    const Function spiceFunc = FunctionManager::createMainFunction(node->entry, paramSymbolTypes, nonConstNode);
    diGenerator.generateFunctionDebugInfo(fct, &spiceFunc);
    diGenerator.setSourceLocation(node);
  }

  // Create entry block
  llvm::BasicBlock *bEntry = createBlock();
  switchToBlock(bEntry, fct);

  // Reset alloca insert markers to this block
  allocaInsertBlock = bEntry;
  allocaInsertInst = nullptr;

  // Allocate result variable
  llvm::Value *resultAddress = insertAlloca(returnType, RETURN_VARIABLE_NAME);
  // Update the symbol table entry
  SymbolTableEntry *resultEntry = currentScope->lookupStrict(RETURN_VARIABLE_NAME);
  assert(resultEntry != nullptr);
  resultEntry->updateAddress(resultAddress);
  // Generate debug info
  diGenerator.generateLocalVarDebugInfo(RETURN_VARIABLE_NAME, resultAddress, SIZE_MAX);
  // Store the default result value
  builder.CreateStore(builder.getInt32(0), resultAddress);

  // Store function argument values
  for (auto &arg : fct->args()) {
    // Get information about the parameter
    const size_t argNumber = arg.getArgNo();
    const std::string paramName = paramNames.at(argNumber);
    llvm::Type *paramType = fctType->getParamType(argNumber);
    // Allocate space for it
    llvm::Value *paramAddress = insertAlloca(paramType, paramName);
    // Update the symbol table entry
    SymbolTableEntry *paramSymbol = paramSymbols.at(argNumber);
    assert(paramSymbol != nullptr);
    paramSymbol->updateAddress(paramAddress);
    // Generate debug info
    diGenerator.generateLocalVarDebugInfo(paramName, paramAddress, argNumber + 1);
    // Store the value at the new address
    builder.CreateStore(&arg, paramAddress);
  }

  // Visit function body
  visit(node->body());

  // Create return statement if the block is not terminated yet
  if (!blockAlreadyTerminated) {
    llvm::Value *result = builder.CreateLoad(fct->getReturnType(), resultEntry->getAddress());
    builder.CreateRet(result);
  }

  // Conclude debug info for function
  diGenerator.concludeFunctionDebugInfo();

  // Verify function
  verifyFunction(fct, node->codeLoc);

  // Change back to root scope
  currentScope = rootScope;
  assert(currentScope != nullptr);

  return nullptr;
}

std::any IRGenerator::visitFctDef(const FctDefNode *node) {
  // Loop through manifestations
  manIdx = 0; // Reset the symbolTypeIndex
  for (const Function *manifestation : node->fctManifestations) {
    assert(manifestation->entry != nullptr);

    // Check if the manifestation is substantiated
    if (!manifestation->isFullySubstantiated()) {
      manIdx++; // Increment symbolTypeIndex
      continue;
    }

    // Do not generate this manifestation if it is private and used by nobody
    const bool isPublic = manifestation->entry->getType().isPublic();
    if (!isPublic && !manifestation->used) {
      manIdx++; // Increment symbolTypeIndex
      continue;
    }

    std::vector<std::string> paramNames;
    std::vector<SymbolTableEntry *> paramSymbols;
    std::vector<llvm::Type *> paramTypes;

    // Add 'this' type as first argument type
    if (manifestation->isMethod()) {
      const SymbolType &thisType = manifestation->thisType;
      paramNames.emplace_back(THIS_VARIABLE_NAME);
      paramTypes.push_back(builder.getPtrTy());
      // Change to struct scope
      const std::string signature = Struct::getSignature(thisType.getSubType(), thisType.getTemplateTypes());
      currentScope = currentScope->getChildScope(STRUCT_SCOPE_PREFIX + signature);
      assert(currentScope != nullptr);
    }

    // Change scope
    currentScope = currentScope->getChildScope(manifestation->getSignature(false));
    assert(currentScope != nullptr);

    // Get 'this' entry
    if (manifestation->isMethod()) {
      SymbolTableEntry *thisEntry = currentScope->lookupStrict(THIS_VARIABLE_NAME);
      assert(thisEntry != nullptr);
      paramSymbols.push_back(thisEntry);
    }

    // Visit parameters
    size_t argIdx = 0;
    if (node->hasParams) {
      const size_t numOfParams = manifestation->paramList.size();
      paramNames.reserve(numOfParams);
      paramSymbols.reserve(numOfParams);
      paramTypes.reserve(numOfParams);
      for (; argIdx < numOfParams; argIdx++) {
        const DeclStmtNode *param = node->paramLst()->params().at(argIdx);
        // Get symbol table entry of param
        SymbolTableEntry *paramSymbol = currentScope->lookupStrict(param->varName);
        assert(paramSymbol != nullptr);
        // Retrieve type of param
        llvm::Type *paramType = manifestation->getParamTypes().at(argIdx).toLLVMType(context, currentScope);
        // Add it to the lists
        paramNames.push_back(param->varName);
        paramSymbols.push_back(paramSymbol);
        paramTypes.push_back(paramType);
      }
    }

    // Get return type
    llvm::Type *returnType = manifestation->returnType.toLLVMType(context, currentScope);

    // Check if function is explicitly inlined
    const bool explicitlyInlined = manifestation->entry->getType().isInline();
    // Get function linkage
    llvm::GlobalValue::LinkageTypes linkage = isPublic ? llvm::Function::ExternalLinkage : llvm::Function::PrivateLinkage;

    // Create function or implement declared function
    const std::string mangledName = manifestation->getMangledName();
    llvm::FunctionType *funcType = llvm::FunctionType::get(returnType, paramTypes, false);
    module->getOrInsertFunction(mangledName, funcType);
    llvm::Function *func = module->getFunction(mangledName);
    node->entry->updateAddress(func);

    // Set attributes to function
    func->setDSOLocal(true);
    func->setLinkage(linkage);
    if (explicitlyInlined)
      func->addFnAttr(llvm::Attribute::AlwaysInline);

    // Set attributes to 'this' param
    if (manifestation->isMethod()) {
      func->addParamAttr(0, llvm::Attribute::NoUndef);
      func->addParamAttr(0, llvm::Attribute::NonNull);
    }

    // Add debug info
    diGenerator.generateFunctionDebugInfo(func, manifestation);
    diGenerator.setSourceLocation(node);

    // Create entry block
    llvm::BasicBlock *bEntry = createBlock();
    switchToBlock(bEntry, func);

    // Reset alloca insert markers to this block
    allocaInsertBlock = bEntry;
    allocaInsertInst = nullptr;

    // Declare result variable
    llvm::Value *resultAddr = insertAlloca(returnType, RETURN_VARIABLE_NAME);
    SymbolTableEntry *resultEntry = currentScope->lookupStrict(RETURN_VARIABLE_NAME);
    assert(resultEntry != nullptr);
    resultEntry->updateAddress(resultAddr);
    // Generate debug info
    diGenerator.generateLocalVarDebugInfo(RETURN_VARIABLE_NAME, resultAddr, SIZE_MAX);

    // Store function argument values
    for (auto &arg : func->args()) {
      // Get information about the parameter
      const size_t argNumber = arg.getArgNo();
      const std::string paramName = paramNames.at(argNumber);
      llvm::Type *paramType = funcType->getParamType(argNumber);
      // Allocate space for it
      llvm::Value *paramAddress = insertAlloca(paramType, paramName);
      // Update the symbol table entry
      SymbolTableEntry *paramSymbol = paramSymbols.at(argNumber);
      assert(paramSymbol != nullptr);
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

    // Visit function body
    visit(node->body());

    // Create return statement if the block is not terminated yet
    if (!blockAlreadyTerminated) {
      llvm::Value *result = builder.CreateLoad(returnType, resultEntry->getAddress());
      builder.CreateRet(result);
    }

    // Conclude debug info for function
    diGenerator.concludeFunctionDebugInfo();

    // Verify function
    verifyFunction(func, node->codeLoc);

    // Change to root scope
    currentScope = rootScope;

    manIdx++; // Increment symbolTypeIndex
  }
  manIdx = 0; // Reset the symbolTypeIndex

  // Ensure that we are at the root scope again
  assert(currentScope == rootScope);

  return nullptr;
}

std::any IRGenerator::visitProcDef(const ProcDefNode *node) {
  // Loop through manifestations
  manIdx = 0; // Reset the symbolTypeIndex
  for (const Function *manifestation : node->procManifestations) {
    assert(manifestation->entry != nullptr);

    // Check if the manifestation is substantiated
    if (!manifestation->isFullySubstantiated()) {
      manIdx++; // Increment symbolTypeIndex
      continue;
    }

    // Do not generate this manifestation if it is private and used by nobody
    const bool isPublic = manifestation->entry->getType().isPublic();
    if (!isPublic && !manifestation->used) {
      manIdx++; // Increment symbolTypeIndex
      continue;
    }

    std::vector<std::string> paramNames;
    std::vector<SymbolTableEntry *> paramSymbols;
    std::vector<llvm::Type *> paramTypes;

    // Add 'this' type as first argument type
    if (manifestation->isMethod()) {
      const SymbolType &thisType = manifestation->thisType;
      paramNames.emplace_back(THIS_VARIABLE_NAME);
      paramTypes.push_back(thisType.toLLVMType(context, currentScope)->getPointerTo());
      // Change to struct scope
      const std::string signature = Struct::getSignature(thisType.getSubType(), thisType.getTemplateTypes());
      currentScope = currentScope->getChildScope(STRUCT_SCOPE_PREFIX + signature);
      assert(currentScope != nullptr);
    }

    // Change scope
    currentScope = currentScope->getChildScope(manifestation->getSignature(false));
    assert(currentScope != nullptr);

    // Get 'this' entry
    if (manifestation->isMethod()) {
      SymbolTableEntry *thisEntry = currentScope->lookupStrict(THIS_VARIABLE_NAME);
      assert(thisEntry != nullptr);
      paramSymbols.push_back(thisEntry);
    }

    // Visit parameters
    size_t argIdx = 0;
    if (node->hasParams) {
      const size_t numOfParams = manifestation->paramList.size();
      paramNames.reserve(numOfParams);
      paramSymbols.reserve(numOfParams);
      paramTypes.reserve(numOfParams);
      for (; argIdx < numOfParams; argIdx++) {
        const DeclStmtNode *param = node->paramLst()->params().at(argIdx);
        // Get symbol table entry of param
        SymbolTableEntry *paramSymbol = currentScope->lookupStrict(param->varName);
        assert(paramSymbol != nullptr);
        // Retrieve type of param
        llvm::Type *paramType = manifestation->getParamTypes().at(argIdx).toLLVMType(context, currentScope);
        // Add it to the lists
        paramNames.push_back(param->varName);
        paramSymbols.push_back(paramSymbol);
        paramTypes.push_back(paramType);
      }
    }

    // Get return type
    llvm::Type *returnType = builder.getVoidTy();

    // Check if procedure is explicitly inlined
    const bool explicitlyInlined = manifestation->entry->getType().isInline();
    // Get procedure linkage
    llvm::GlobalValue::LinkageTypes linkage = isPublic ? llvm::Function::ExternalLinkage : llvm::Function::PrivateLinkage;

    // Create procedure or implement declared procedure
    const std::string mangledName = manifestation->getMangledName();
    llvm::FunctionType *procType = llvm::FunctionType::get(returnType, paramTypes, false);
    module->getOrInsertFunction(mangledName, procType);
    llvm::Function *proc = module->getFunction(mangledName);
    node->entry->updateAddress(proc);

    // Set attributes to procedure
    proc->setDSOLocal(true);
    proc->setLinkage(linkage);
    if (explicitlyInlined)
      proc->addFnAttr(llvm::Attribute::AlwaysInline);

    // Set attributes to 'this' param
    if (manifestation->isMethod()) {
      proc->addParamAttr(0, llvm::Attribute::NoUndef);
      proc->addParamAttr(0, llvm::Attribute::NonNull);
    }

    // Add debug info
    diGenerator.generateFunctionDebugInfo(proc, manifestation);
    diGenerator.setSourceLocation(node);

    // Create entry block
    llvm::BasicBlock *bEntry = createBlock();
    switchToBlock(bEntry, proc);

    // Reset alloca insert markers to this block
    allocaInsertBlock = bEntry;
    allocaInsertInst = nullptr;

    // Store procedure argument values
    for (auto &arg : proc->args()) {
      // Get information about the parameter
      const size_t argNumber = arg.getArgNo();
      const std::string paramName = paramNames.at(argNumber);
      llvm::Type *paramType = procType->getParamType(argNumber);
      // Allocate space for it
      llvm::Value *paramAddress = insertAlloca(paramType, paramName);
      // Update the symbol table entry
      SymbolTableEntry *paramSymbol = paramSymbols.at(argNumber);
      assert(paramSymbol != nullptr);
      paramSymbol->updateAddress(paramAddress);
      // Generate debug info
      diGenerator.generateLocalVarDebugInfo(paramName, paramAddress, argNumber + 1);
      // Store the value at the new address
      builder.CreateStore(&arg, paramAddress);
    }

    // Store the default values for optional procedure args
    if (node->paramLst()) {
      const std::vector<DeclStmtNode *> params = node->paramLst()->params();
      for (; argIdx < params.size(); argIdx++)
        visit(params.at(argIdx));
    }

    // Visit procedure body
    visit(node->body());

    // Create return statement if the block is not terminated yet
    if (!blockAlreadyTerminated)
      builder.CreateRetVoid();

    // Conclude debug info for procedure
    diGenerator.concludeFunctionDebugInfo();

    // Verify procedure
    verifyFunction(proc, node->codeLoc);

    // Change to root scope
    currentScope = rootScope;

    manIdx++; // Increment symbolTypeIndex
  }
  manIdx = 0; // Reset the symbolTypeIndex

  // Ensure that we are at the root scope again
  assert(currentScope == rootScope);

  return nullptr;
}

std::any IRGenerator::visitStructDef(const StructDefNode *node) {
  // Get all substantiated structs which result from this struct def
  std::vector<Struct *> manifestations = node->structManifestations;

  // Sort the manifestations to prevent generating the struct types in the wrong order (in case of dependencies between structs)
  auto comp = [](const Struct *lhs, Struct *rhs) { return lhs->manifestationIndex < rhs->manifestationIndex; };
  std::sort(manifestations.begin(), manifestations.end(), comp);

  for (Struct *spiceStruct : manifestations) {
    // Skip structs, that are not fully substantiated
    if (!spiceStruct->isFullySubstantiated())
      continue;

    // Do not generate this struct if it is private and used by nobody
    if (!spiceStruct->used && !spiceStruct->entry->getType().isPublic())
      continue;

    // Change scope to struct scope, specific to substantiation
    currentScope = spiceStruct->structScope;
    assert(currentScope);

    // Create struct definition
    llvm::StructType *structType = llvm::StructType::create(context, spiceStruct->getMangledName());

    // Set LLVM type to the struct entry
    assert(spiceStruct->entry != nullptr);
    spiceStruct->entry->setStructLLVMType(structType);

    // Collect concrete field types
    std::vector<llvm::Type *> fieldTypes;
    fieldTypes.reserve(node->fields().size());
    for (const FieldNode *field : node->fields()) {
      SymbolTableEntry *fieldEntry = currentScope->lookupStrict(field->fieldName);
      assert(fieldEntry && !fieldEntry->getType().hasAnyGenericParts());
      fieldTypes.push_back(fieldEntry->getType().toLLVMType(context, currentScope));
    }

    // Set field types to struct type
    structType->setBody(fieldTypes);

    // Return to root scope
    currentScope = rootScope;
    assert(currentScope);
  }

  return nullptr;
}

std::any IRGenerator::visitInterfaceDef(const InterfaceDefNode *node) {
  return nullptr; // Noop (interfaces are high-level semantic-only structures)
}

std::any IRGenerator::visitEnumDef(const EnumDefNode *node) {
  return nullptr; // Noop (enums are high-level semantic-only structures)
}

std::any IRGenerator::visitGenericTypeDef(const GenericTypeDefNode *node) {
  return nullptr; // Noop (generic types are high-level semantic-only structures)
}

std::any IRGenerator::visitAliasDef(const AliasDefNode *node) {
  return nullptr; // Noop (alias definitions are high-level semantic-only structures)
}

std::any IRGenerator::visitGlobalVarDef(const GlobalVarDefNode *node) {
  // Retrieve some information about the variable
  assert(node->entry != nullptr);
  const SymbolType &entryType = node->entry->getType();
  const bool isPublic = entryType.isPublic();
  const bool isConst = entryType.isConst();

  // Get correct type and linkage type
  auto varType = std::any_cast<llvm::Type *>(visit(node->dataType()));
  auto linkage = isPublic ? llvm::GlobalValue::ExternalLinkage : llvm::GlobalValue::PrivateLinkage;

  // Create global var
  llvm::Value *varAddress = module->getOrInsertGlobal(node->varName, varType);
  llvm::GlobalVariable *var = module->getNamedGlobal(node->varName);
  // Set some attributes, based on the given information
  var->setLinkage(linkage);
  var->setConstant(isConst);

  // Set initializer
  llvm::Constant *constantValue;
  if (node->hasValue) { // Set the constant value as variable initializer
    constantValue = std::any_cast<llvm::Constant *>(visit(node->constant()));
  } else { // Set the default value as variable initializer
    constantValue = getDefaultValueForSymbolType(node->entry->getType());
  }
  var->setInitializer(constantValue);

  node->entry->updateAddress(varAddress);

  // Add debug info
  diGenerator.generateGlobalVarDebugInfo(var, node->entry);

  return nullptr;
}

std::any IRGenerator::visitExtDecl(const ExtDeclNode *node) {
  // Get return type
  const Function *spiceFunc = node->extFunction;
  assert(spiceFunc != nullptr);
  llvm::Type *returnType = builder.getVoidTy();
  if (!spiceFunc->returnType.is(TY_DYN))
    returnType = spiceFunc->returnType.toLLVMType(context, currentScope);

  // Get arg types
  std::vector<llvm::Type *> argTypes;
  argTypes.reserve(spiceFunc->paramList.size());
  for (const SymbolType &paramType : spiceFunc->getParamTypes())
    argTypes.push_back(paramType.toLLVMType(context, currentScope));

  // Declare function
  llvm::FunctionType *functionType = llvm::FunctionType::get(returnType, argTypes, node->isVarArg);
  module->getOrInsertFunction(spiceFunc->name, functionType);

  // If the function should be imported as dll, add the dll attribute
  if (node->fctAttrs()) {
    const bool linkAsDll = node->fctAttrs()->attrLst()->getAttrByName(AttrNode::ATTR_CORE_LINKER_DLL)->getValue().boolValue;
    if (linkAsDll)
      module->getFunction(spiceFunc->name)->setDLLStorageClass(llvm::GlobalValue::DLLImportStorageClass);
  }

  return nullptr;
}

} // namespace spice::compiler