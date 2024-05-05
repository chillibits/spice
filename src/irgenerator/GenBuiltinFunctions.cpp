// Copyright (c) 2021-2024 ChilliBits. All rights reserved.

#include "IRGenerator.h"

#include <ast/ASTNodes.h>

namespace spice::compiler {

std::any IRGenerator::visitPrintfCall(const PrintfCallNode *node) {
  // Retrieve printf function
  llvm::Function *printfFct = stdFunctionManager.getPrintfFct();

  // Push the template string as first argument
  std::vector<llvm::Value *> printfArgs;
  llvm::Constant *templateString = createGlobalStringConst("printf.str.", node->templatedString, node->codeLoc);
  printfArgs.push_back(templateString);

  // Collect replacement arguments
  for (AssignExprNode *arg : node->args()) {
    // Retrieve type of argument
    const QualType argSymbolType = arg->getEvaluatedSymbolType(manIdx);

    // Re-map some values
    llvm::Value *argVal;
    if (argSymbolType.isArray()) {
      llvm::Value *argValPtr = resolveAddress(arg);
      llvm::Value *indices[2] = {builder.getInt32(0), builder.getInt32(0)};
      llvm::Type *argType = argSymbolType.getType().toLLVMType(context, currentScope);
      argVal = insertInBoundsGEP(argType, argValPtr, indices);
    } else if (argSymbolType.getBase().getType().isStringObj()) {
      llvm::Value *argValPtr = resolveAddress(arg);
      llvm::Type *argBaseType = argSymbolType.getBase().getType().toLLVMType(context, currentScope);
      argValPtr = insertStructGEP(argBaseType, argValPtr, 0);
      argVal = insertLoad(builder.getPtrTy(), argValPtr);
    } else {
      argVal = resolveValue(arg);
    }

    // Extend all integer types lower than 32 bit to 32 bit
    if (argSymbolType.removeReferenceWrapper().isOneOf({TY_SHORT, TY_BYTE, TY_CHAR, TY_BOOL})) {
      if (argSymbolType.removeReferenceWrapper().isSigned())
        argVal = builder.CreateSExt(argVal, llvm::Type::getInt32Ty(context));
      else
        argVal = builder.CreateZExt(argVal, llvm::Type::getInt32Ty(context));
    }

    printfArgs.push_back(argVal);
  }

  // Call printf function
  llvm::CallInst *returnValue = builder.CreateCall(printfFct, printfArgs);

  // Add noundef attribute to template string
  returnValue->addParamAttr(0, llvm::Attribute::NoUndef);

  return LLVMExprResult{.value = returnValue};
}

std::any IRGenerator::visitSizeofCall(const SizeofCallNode *node) {
  llvm::Type *type;
  if (node->isType) { // Size of type
    type = any_cast<llvm::Type *>(visit(node->dataType()));
  } else { // Size of value
    type = node->assignExpr()->getEvaluatedSymbolType(manIdx).getType().toLLVMType(context, currentScope);
  }
  // Calculate size at compile-time
  const llvm::TypeSize sizeInBits = module->getDataLayout().getTypeSizeInBits(type);

  // Return size value
  llvm::Value *sizeValue = builder.getInt64(sizeInBits);
  return LLVMExprResult{.value = sizeValue};
}

std::any IRGenerator::visitAlignofCall(const AlignofCallNode *node) {
  llvm::Type *type;
  if (node->isType) { // Align of type
    type = any_cast<llvm::Type *>(visit(node->dataType()));
  } else { // Align of value
    type = node->assignExpr()->getEvaluatedSymbolType(manIdx).getType().toLLVMType(context, currentScope);
  }
  // Calculate size at compile-time
  const llvm::Align align = module->getDataLayout().getABITypeAlign(type);

  // Return align value
  llvm::Value *sizeValue = builder.getInt64(align.value());
  return LLVMExprResult{.value = sizeValue};
}

std::any IRGenerator::visitLenCall(const LenCallNode *node) {
  // Check if the length is fixed and known via the symbol type
  QualType symbolType = node->assignExpr()->getEvaluatedSymbolType(manIdx);
  symbolType = symbolType.removeReferenceWrapper();

  llvm::Value *lengthValue;
  if (symbolType.is(TY_STRING)) {
    llvm::Function *getRawLengthFct = stdFunctionManager.getStringGetRawLengthStringFct();
    lengthValue = builder.CreateCall(getRawLengthFct, resolveValue(node->assignExpr()));
  } else {
    assert(symbolType.isArray() && symbolType.getType().getArraySize() != ARRAY_SIZE_UNKNOWN);
    // Return length value
    lengthValue = builder.getInt64(symbolType.getType().getArraySize());
  }
  return LLVMExprResult{.value = lengthValue};
}

std::any IRGenerator::visitPanicCall(const PanicCallNode *node) {
  // Create constant for error message
  const std::string codeLoc = node->codeLoc.toPrettyString();
  const std::string errorMsg = "Program panicked at " + codeLoc + ":\n" + node->getErrorMessage() + "\n";
  llvm::Constant *globalString = builder.CreateGlobalStringPtr(errorMsg, getUnusedGlobalName(ANON_GLOBAL_STRING_NAME));
  // Print the error message
  llvm::Function *printfFct = stdFunctionManager.getPrintfFct();
  builder.CreateCall(printfFct, globalString);
  // Generate call to exit()
  llvm::Function *exitFct = stdFunctionManager.getExitFct();
  builder.CreateCall(exitFct, builder.getInt32(EXIT_FAILURE));
  // Create unreachable instruction
  builder.CreateUnreachable();
  // Unreachable counts as terminator
  blockAlreadyTerminated = true;

  return nullptr;
}

} // namespace spice::compiler