// Copyright (c) 2021 ChilliBits. All rights reserved.

#include "GeneratorVisitor.h"

void GeneratorVisitor::init() {
    // Initialize LLVM
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();
}

void GeneratorVisitor::optimize() {
    /*// Register optimization passes
    std::unique_ptr<llvm::legacy::FunctionPassManager> functionPassManager =
            llvm::make_unique<llvm::legacy::FunctionPassManager>(module.get());

    // Promote allocas to registers.
    functionPassManager->add(llvm::createPromoteMemoryToRegisterPass());
    // Do simple "peephole" optimizations
    functionPassManager->add(llvm::createInstructionCombiningPass());
    // Reassociate expressions.
    functionPassManager->add(llvm::createReassociatePass());
    // Eliminate Common SubExpressions.
    functionPassManager->add(llvm::createGVNPass());
    // Simplify the control flow graph (deleting unreachable blocks etc).
    functionPassManager->add(llvm::createCFGSimplificationPass());

    functionPassManager->doInitialization();

    for (auto &function : functions) {
        llvm::Function *llvmFun =
                module->getFunction(llvm::StringRef(function->functionName));
        functionPassManager->run(*llvmFun);
    }

    llvm::Function *llvmMainFun = module->getFunction(llvm::StringRef("main"));
    functionPassManager->run(*llvmMainFun);*/
}

void GeneratorVisitor::emit() {
    // Configure output target
    // ToDo: Make target customizable by setting an cli arg or similar
    auto targetTriple = llvm::sys::getDefaultTargetTriple();
    module->setTargetTriple(targetTriple);

    // Search after selected target
    std::string error;
    auto target = llvm::TargetRegistry::lookupTarget(targetTriple, error);
    if (!target) throw IRError(TARGET_NOT_AVAILABLE, "Selected target was not found: " + error);

    auto cpu = "generic";
    auto features = "";

    llvm::TargetOptions opt;
    auto rm = llvm::Optional<llvm::Reloc::Model>();
    auto targetMachine = target->createTargetMachine(targetTriple, cpu, features, opt, rm);

    module->setDataLayout(targetMachine->createDataLayout());

    // Open file output stream
    std::string filename = "output.o";
    std::error_code errorCode;
    llvm::raw_fd_ostream dest(filename, errorCode, llvm::sys::fs::OF_None);
    if (errorCode) throw IRError(CANT_OPEN_OUTPUT_FILE, "File '" + filename + "' could not be opened");

    llvm::legacy::PassManager pass;
    auto FileType = llvm::CGFT_ObjectFile;
    if (targetMachine->addPassesToEmitFile(pass, dest, nullptr, FileType))
        throw IRError(WRONG_TYPE, "Target machine can't emit a file of this type");

    // Emit object file
    pass.run(*module);
    dest.flush();
}

void GeneratorVisitor::dumpIR() {
    module->print(llvm::outs(), nullptr);
}

antlrcpp::Any GeneratorVisitor::visitEntry(SpiceParser::EntryContext *ctx) {
    // Generate code for external functions
    initializeExternalFunctions();

    return SpiceBaseVisitor::visitEntry(ctx);
}

antlrcpp::Any GeneratorVisitor::visitMainFunctionDef(SpiceParser::MainFunctionDefContext *ctx) {
    // Build function itself
    auto mainType = llvm::FunctionType::get(llvm::IntegerType::getInt32Ty(*context),
                                    std::vector<llvm::Type *>(), false);
    auto main = llvm::Function::Create(mainType, llvm::Function::ExternalLinkage, "main", module.get());
    auto mainBasicBlock = llvm::BasicBlock::Create(*context, "entry", main);
    builder->SetInsertPoint(mainBasicBlock);
    namedValues.clear();

    // Generate IR for function body
    visit(ctx->stmtLst());

    // Verify function
    llvm::verifyFunction(*main);

    // Return true as result for the function definition
    return llvm::ConstantInt::get(llvm::Type::getInt1Ty(*context), 1);
}

antlrcpp::Any GeneratorVisitor::visitFunctionDef(SpiceParser::FunctionDefContext *ctx) {
    /*std::string functionName = ctx->IDENTIFIER()->toString();
    // Create function itself
    auto function = module->getFunction(llvm::StringRef(functionName));

    // Create entry block
    auto bEntry = llvm::BasicBlock::Create(*context, "entry");
    function->getBasicBlockList().push_back(bEntry);
    builder->SetInsertPoint(bEntry);

    // Store function params
    namedValues.clear();
    for (auto& param : function->args()) {
        auto paramNo = param.getArgNo();
        std::string paramName = ctx->paramLstDef()->assignment()[paramNo]->IDENTIFIER()->toString();
        llvm::Type *paramType = function->getFunctionType()->getParamType(paramNo);
        namedValues[paramName] = builder->CreateAlloca(paramType, nullptr, paramName);
        builder->CreateStore(&param, namedValues[paramName]);
    }

    // Generate IR for function body
    visit(ctx->stmtLst());

    // Verify function
    llvm::verifyFunction(*function);

    // Return true as result for the function definition
    return llvm::ConstantInt::get((llvm::Type::getInt1Ty(*context)), 1);*/
    return SpiceBaseVisitor::visitFunctionDef(ctx);
}

antlrcpp::Any GeneratorVisitor::visitProcedureDef(SpiceParser::ProcedureDefContext *ctx) {
    /*// Create procedure itself
    auto procedure = module->getFunction(llvm::StringRef(ctx->IDENTIFIER()->toString()));

    // Create entry block
    auto bEntry = llvm::BasicBlock::Create(*context, "entry");
    procedure->getBasicBlockList().push_back(bEntry);
    builder->SetInsertPoint(bEntry);

    // Store procedure params
    namedValues.clear();
    for (auto& param : procedure->args()) {
        auto paramNo = param.getArgNo();
        std::string paramName = ctx->paramLstDef()->assignment()[paramNo]->IDENTIFIER()->toString();
        llvm::Type *paramType = procedure->getFunctionType()->getParamType(paramNo);
        namedValues[paramName] = builder->CreateAlloca(paramType, nullptr, paramName);
        builder->CreateStore(&param, namedValues[paramName]);
    }

    // Generate IR for procedure body
    visit(ctx->stmtLst());

    // Create return
    builder->CreateRetVoid();

    // Verify procedure
    llvm::verifyFunction(*procedure);

    // Return true as result for the function definition
    return llvm::ConstantInt::get((llvm::Type::getInt1Ty(*context)), 1);*/
    return SpiceBaseVisitor::visitProcedureDef(ctx);
}

antlrcpp::Any GeneratorVisitor::visitForLoop(SpiceParser::ForLoopContext *ctx) {
    auto parentFct = builder->GetInsertBlock()->getParent();

    // Create blocks
    auto bLoop = llvm::BasicBlock::Create(*context, "loop");
    auto bLoopPre = llvm::BasicBlock::Create(*context, "loop_pre");
    auto bLoopPost = llvm::BasicBlock::Create(*context, "loop_post");
    auto bLoopEnd = llvm::BasicBlock::Create(*context, "loop_end");

    // Fill loop pre block
    parentFct->getBasicBlockList().push_back(bLoopPre);
    builder->SetInsertPoint(bLoopPre);
    visit(ctx->assignment()[0]);
    // Check if entering the loop is necessary
    auto conditionValue = visit(ctx->assignment()[1]).as<llvm::Value*>();
    builder->CreateCondBr(conditionValue, bLoop, bLoopEnd);

    // Fill loop block
    parentFct->getBasicBlockList().push_back(bLoop);
    builder->SetInsertPoint(bLoop);
    // Generate IR for nested statements
    visit(ctx->stmtLst());
    // Check if condition is now false
    conditionValue = visit(ctx->assignment()[1]).as<llvm::Value*>();
    builder->CreateCondBr(conditionValue, bLoopPost, bLoopEnd);

    // Fill loop post block
    parentFct->getBasicBlockList().push_back(bLoopPost);
    builder->SetInsertPoint(bLoopPost);
    visit(ctx->assignment()[2]);
    builder->CreateBr(bLoop);

    // Fil loop end block
    parentFct->getBasicBlockList().push_back(bLoopEnd);
    builder->SetInsertPoint(bLoopEnd);

    // Return true as result for the loop
    return llvm::ConstantInt::get(llvm::Type::getInt1Ty(*context), 1);
}

antlrcpp::Any GeneratorVisitor::visitWhileLoop(SpiceParser::WhileLoopContext *ctx) {
    auto conditionValue = visit(ctx->assignment()).as<llvm::Value*>();
    auto parentFct = builder->GetInsertBlock()->getParent();

    // Create blocks
    auto bLoop = llvm::BasicBlock::Create(*context, "loop");
    auto bLoopEnd = llvm::BasicBlock::Create(*context, "loop_end");

    // Check if entering the loop is necessary
    builder->CreateCondBr(conditionValue, bLoop, bLoopEnd);

    // Fill loop block
    parentFct->getBasicBlockList().push_back(bLoop);
    builder->SetInsertPoint(bLoop);
    // Generate IR for nested statements
    visit(ctx->stmtLst());
    // Visit condition again
    conditionValue = visit(ctx->assignment()).as<llvm::Value*>();
    // Check if condition is now false
    bLoop = builder->GetInsertBlock();
    builder->CreateCondBr(conditionValue, bLoop, bLoopEnd);

    // Fill loop end block
    parentFct->getBasicBlockList().push_back(bLoopEnd);
    builder->SetInsertPoint(bLoopEnd);

    // Return true as result for the loop
    return llvm::ConstantInt::get(llvm::Type::getInt1Ty(*context), 1);
}

antlrcpp::Any GeneratorVisitor::visitIfStmt(SpiceParser::IfStmtContext *ctx) {
    auto conditionValue = visit(ctx->assignment()).as<llvm::Value*>();
    auto parentFct = builder->GetInsertBlock()->getParent();

    // Create blocks
    auto bThen = llvm::BasicBlock::Create(*context, "then");
    auto bEnd = llvm::BasicBlock::Create(*context, "end");

    // Check if if condition is fulfilled
    builder->CreateCondBr(conditionValue, bThen, bEnd);

    // Fill then block
    parentFct->getBasicBlockList().push_back(bThen);
    builder->SetInsertPoint(bThen);
    // Generate IR for nested statements
    visit(ctx->stmtLst());
    builder->CreateBr(bEnd);

    // Fill end block
    parentFct->getBasicBlockList().push_back(bEnd);
    builder->SetInsertPoint(bEnd);

    // Return conditional value as result for the if stmt
    return conditionValue;
}

antlrcpp::Any GeneratorVisitor::visitDeclStmt(SpiceParser::DeclStmtContext *ctx) {
    return SpiceBaseVisitor::visitDeclStmt(ctx);
}

antlrcpp::Any GeneratorVisitor::visitFunctionCall(SpiceParser::FunctionCallContext *ctx) {
    llvm::Function* calleeFun = module->getFunction(llvm::StringRef(ctx->IDENTIFIER()->toString()));
    llvm::FunctionType* calleeFunTy = calleeFun->getFunctionType();
    std::vector<llvm::Value*> argValues;
    for (int i = 0; i < ctx->paramLstCall()->assignment().size(); i++) {
        auto argValue = visit(ctx->paramLstCall()->assignment()[i]).as<llvm::Value*>();
        auto paramTy = calleeFunTy->getParamType(i);
        auto bitCastArgValue = builder->CreateBitCast(argValue, paramTy);
        argValues.push_back(bitCastArgValue);
    }
    return builder->CreateCall(calleeFun, argValues);
}

antlrcpp::Any GeneratorVisitor::visitImportStmt(SpiceParser::ImportStmtContext *ctx) {
    return SpiceBaseVisitor::visitImportStmt(ctx);
}

antlrcpp::Any GeneratorVisitor::visitReturnStmt(SpiceParser::ReturnStmtContext *ctx) {
    auto returnValue = visit(ctx->assignment()).as<llvm::Value*>();
    // Build return value
    builder->CreateRet(returnValue);
    return returnValue;
}

antlrcpp::Any GeneratorVisitor::visitPrintfStmt(SpiceParser::PrintfStmtContext *ctx) {
    auto printf = module->getFunction("printf");
    std::vector<llvm::Value*> printfArgs;
    auto stringTemplate = ctx->STRING()->toString();
    stringTemplate = stringTemplate.erase(stringTemplate.size() -1).erase(0, 1);
    printfArgs.push_back(builder->CreateGlobalStringPtr(stringTemplate));
    for (auto &arg : ctx->assignment()) {
        auto argVal = visit(arg).as<llvm::Value*>();
        if (argVal == nullptr) throw std::runtime_error("Printf has null arg");
        printfArgs.push_back(argVal);
    }
    return builder->CreateCall(printf, printfArgs);
}

antlrcpp::Any GeneratorVisitor::visitAssignment(SpiceParser::AssignmentContext *ctx) {
    if (ctx->declStmt() || ctx->IDENTIFIER()) {
        // Get value of left and right side
        auto rhs = visit(ctx->ternary()).as<llvm::Value*>();
        auto lhs = visit(ctx->IDENTIFIER()).as<llvm::Value*>();
        // Store right side on the left one
        builder->CreateStore(rhs, lhs);
        // Return value of the right side
        return rhs;
    }
    return visit(ctx->ternary());
}

antlrcpp::Any GeneratorVisitor::visitTernary(SpiceParser::TernaryContext *ctx) {
    if (ctx->logicalOrExpr().size() > 1) {
        auto conditionValue = visit(ctx->logicalOrExpr()[0]).as<llvm::Value*>();
        auto parentFct = builder->GetInsertBlock()->getParent();

        // Create blocks
        auto bThen = llvm::BasicBlock::Create(*context, "then");
        auto bElse = llvm::BasicBlock::Create(*context, "else");
        auto bEnd = llvm::BasicBlock::Create(*context, "end");

        // Conditional jump to respective block
        builder->CreateCondBr(conditionValue, bThen, bElse);

        // Fill then block
        parentFct->getBasicBlockList().push_back(bThen);
        builder->SetInsertPoint(bThen);
        auto thenValue = visit(ctx->logicalOrExpr()[1]).as<llvm::Value*>();
        builder->CreateBr(bEnd);

        // Fill else block
        parentFct->getBasicBlockList().push_back(bElse);
        builder->SetInsertPoint(bElse);
        auto elseValue = visit(ctx->logicalOrExpr()[2]).as<llvm::Value*>();
        builder->CreateBr(bEnd);

        // Fill end block
        parentFct->getBasicBlockList().push_back(bEnd);
        builder->SetInsertPoint(bEnd);
        // if either is void or their types don't match (which indicates one of them
        // returned the null value for void, then don't construct a phi node)
        if (thenValue->getType() == llvm::Type::getVoidTy(*context) ||
            elseValue->getType() == llvm::Type::getVoidTy(*context) ||
            thenValue->getType() != elseValue->getType()) {
            return llvm::Constant::getNullValue(llvm::Type::getInt32Ty(*context));
        }
        // Setup phi value
        auto phi = builder->CreatePHI(thenValue->getType(), 2, "phi");
        phi->addIncoming(thenValue, bThen);
        phi->addIncoming(elseValue, bElse);
        return phi;
    }
    return visit(ctx->logicalOrExpr()[0]);
    //return SpiceBaseVisitor::visitTernary(ctx);
}

antlrcpp::Any GeneratorVisitor::visitLogicalOrExpr(SpiceParser::LogicalOrExprContext *ctx) {
    if (ctx->logicalAndExpr().size() > 1) {
        auto lhs = visit(ctx->logicalAndExpr()[0]).as<llvm::Value*>();
        for (int i = 1; i < ctx->logicalAndExpr().size(); i++) {
            auto rhs = visit(ctx->logicalAndExpr()[i]).as<llvm::Value*>();
            lhs = builder->CreateLogicalOr(lhs, rhs, "logical or");
        }
        return lhs;
    }
    return visit(ctx->logicalAndExpr()[0]);
}

antlrcpp::Any GeneratorVisitor::visitLogicalAndExpr(SpiceParser::LogicalAndExprContext *ctx) {
    if (ctx->bitwiseOrExpr().size() > 1) {
        auto lhs = visit(ctx->bitwiseOrExpr()[0]).as<llvm::Value*>();
        for (int i = 1; i < ctx->bitwiseOrExpr().size(); i++) {
            auto rhs = visit(ctx->bitwiseOrExpr()[i]).as<llvm::Value*>();
            lhs = builder->CreateLogicalAnd(lhs, rhs, "logical and");
        }
        return lhs;
    }
    return visit(ctx->bitwiseOrExpr()[0]);
}

antlrcpp::Any GeneratorVisitor::visitBitwiseOrExpr(SpiceParser::BitwiseOrExprContext *ctx) {
    if (ctx->bitwiseAndExpr().size() > 1) {
        auto lhs = visit(ctx->bitwiseAndExpr()[0]).as<llvm::Value*>();
        for (int i = 1; i < ctx->bitwiseAndExpr().size(); i++) {
            auto rhs = visit(ctx->bitwiseAndExpr()[i]).as<llvm::Value*>();
            lhs = builder->CreateOr(lhs, rhs, "bitwise or");
        }
        return lhs;
    }
    return visit(ctx->bitwiseAndExpr()[0]);
}

antlrcpp::Any GeneratorVisitor::visitBitwiseAndExpr(SpiceParser::BitwiseAndExprContext *ctx) {
    if (ctx->equalityExpr().size() > 1) {
        auto lhs = visit(ctx->equalityExpr()[0]).as<llvm::Value*>();
        for (int i = 1; i < ctx->equalityExpr().size(); i++) {
            auto rhs = visit(ctx->equalityExpr()[i]).as<llvm::Value*>();
            lhs = builder->CreateAnd(lhs, rhs, "bitwise and");
        }
        return lhs;
    }
    return visit(ctx->equalityExpr()[0]);
}

antlrcpp::Any GeneratorVisitor::visitEqualityExpr(SpiceParser::EqualityExprContext *ctx) {
    if (ctx->children.size() > 1) {
        auto lhs = visit(ctx->relationalExpr()[0]).as<llvm::Value*>();
        auto rhs = visit(ctx->relationalExpr()[1]).as<llvm::Value*>();

        // Equality expr is: relationalExpr EQUAL relationalExpr
        if (ctx->EQUAL()) return builder->CreateICmpEQ(lhs, rhs, "equal");

        // Equality expr is: relationalExpr NOT_EQUAL relationalExpr
        return builder->CreateICmpNE(lhs, rhs, "not equal");
    }
    return visit(ctx->relationalExpr()[0]);
}

antlrcpp::Any GeneratorVisitor::visitRelationalExpr(SpiceParser::RelationalExprContext *ctx) {
    if (ctx->children.size() > 1) {
        auto lhs = visit(ctx->additiveExpr()[0]).as<llvm::Value*>();
        auto rhs = visit(ctx->additiveExpr()[1]).as<llvm::Value*>();

        // Relational expr is: additiveExpr LESS additiveExpr
        if (ctx->LESS()) return builder->CreateICmpSLT(lhs, rhs, "less than");

        // Relational expr is: additiveExpr GREATER additiveExpr
        if (ctx->GREATER()) return builder->CreateICmpSGT(lhs, rhs, "greater than");

        // Relational expr is: additiveExpr LESS_EQUAL additiveExpr
        if (ctx->LESS_EQUAL()) return builder->CreateICmpSLE(lhs, rhs, "less equal");

        // Relational expr is: additiveExpr GREATER_EQUAL additiveExpr
        return builder->CreateICmpSGE(lhs, rhs, "greater equal");
    }
    return visit(ctx->additiveExpr()[0]);
}

antlrcpp::Any GeneratorVisitor::visitAdditiveExpr(SpiceParser::AdditiveExprContext *ctx) {
    if (ctx->multiplicativeExpr().size() > 1) {
        auto lhs = visit(ctx->multiplicativeExpr()[0]).as<llvm::Value*>();
        for (int i = 1; i < ctx->multiplicativeExpr().size(); i++) {
            auto rhs = visit(ctx->multiplicativeExpr()[i]).as<llvm::Value*>();
            if (ctx->PLUS()[i-1])
                lhs = builder->CreateAdd(lhs, rhs, "add");
            else
                lhs = builder->CreateSub(lhs, rhs, "sub");
        }
        return lhs;
    }
    return visit(ctx->multiplicativeExpr()[0]);
}

antlrcpp::Any GeneratorVisitor::visitMultiplicativeExpr(SpiceParser::MultiplicativeExprContext *ctx) {
    if (ctx->prefixUnary().size() > 1) {
        auto lhs = visit(ctx->prefixUnary()[0]).as<llvm::Value*>();
        for (int i = 1; i < ctx->prefixUnary().size(); i++) {
            auto rhs = visit(ctx->prefixUnary()[i]).as<llvm::Value*>();
            if (ctx->MUL()[i-1])
                lhs = builder->CreateMul(lhs, rhs, "mul");
            else
                lhs = builder->CreateSDiv(lhs, rhs, "div");
        }
        return lhs;
    }
    return visit(ctx->prefixUnary()[0]);
}

antlrcpp::Any GeneratorVisitor::visitPrefixUnary(SpiceParser::PrefixUnaryContext *ctx) {
    auto value = visit(ctx->postfixUnary());

    // Prefix unary is: PLUS_PLUS postfixUnary
    if (ctx->PLUS_PLUS())
        return builder->CreateAdd(value.as<llvm::Value*>(), builder->getInt32(1), "++ prefix");

    // Prefix unary is: MINUS_MINUS postfixUnary
    if (ctx->MINUS_MINUS())
        return builder->CreateSub(value.as<llvm::Value*>(), builder->getInt32(1), "-- prefix");

    // Prefix unary is: NOT postfixUnary
    if (ctx->NOT())
        return builder->CreateNot(value.as<llvm::Value*>(), "not");

    return value;
}

antlrcpp::Any GeneratorVisitor::visitPostfixUnary(SpiceParser::PostfixUnaryContext *ctx) {
    auto value = visit(ctx->atomicExpr());

    // Postfix unary is: PLUS_PLUS atomicExpr
    if (ctx->PLUS_PLUS())
        return builder->CreateAdd(value.as<llvm::Value*>(), builder->getInt32(1), "++ postfix");

    // Postfix unary is: MINUS_MINUS atomicExpr
    if (ctx->MINUS_MINUS())
        return builder->CreateSub(value.as<llvm::Value*>(), builder->getInt32(1), "-- postfix");

    return value;
}

antlrcpp::Any GeneratorVisitor::visitAtomicExpr(SpiceParser::AtomicExprContext *ctx) {
    // Atomic expr is: LPAREN value RPAREN
    if (ctx->LPAREN()) return visit(ctx->assignment()).as<llvm::Value*>();

    // Atomic expr is: value
    return visit(ctx->value());
}

antlrcpp::Any GeneratorVisitor::visitValue(SpiceParser::ValueContext *ctx) {
    // Value is a double constant
    if (ctx->DOUBLE()) {
        auto value = std::stod(ctx->DOUBLE()->toString());
        return (llvm::Value*) llvm::ConstantFP::get(*context, llvm::APFloat(value));
    }

    // Value is an integer constant
    if (ctx->INTEGER()) {
        auto value = std::stoi(ctx->INTEGER()->toString());
        return (llvm::Value*) llvm::ConstantInt::getSigned(llvm::Type::getInt32Ty(*context), value);
    }

    // Value is a string constant
    if (ctx->STRING()) {
        std::string value = ctx->STRING()->toString();
        auto charType = llvm::IntegerType::get(*context, 8);
        std::vector<llvm::Constant*> chars(value.size());
        for(unsigned int i = 0; i < value.size(); i++) chars[i] = llvm::ConstantInt::get(charType, value[i]);
        return (llvm::Value*) llvm::ConstantArray::get(llvm::ArrayType::get(charType, chars.size()), chars);
    }

    // Value is a boolean constant
    if (ctx->TRUE() || ctx->FALSE())
        return (llvm::Value*) llvm::ConstantInt::getSigned(llvm::Type::getInt1Ty(*context), ctx->TRUE() ? 1 : 0);

    // Value is an identifier
    if (ctx->IDENTIFIER()) {
        llvm::Value* var = namedValues[ctx->IDENTIFIER()->toString()];
        if (!var) throw std::runtime_error("Internal compiler error - Variable not found in code generation step");
        return var;
    }

    // Value is a function call
    auto calleeFun = module->getFunction(llvm::StringRef(ctx->functionCall()->IDENTIFIER()->toString()));
    auto calleeFunTy = calleeFun->getFunctionType();
    std::vector<llvm::Value*> argValues;
    auto params = ctx->functionCall()->paramLstCall()->assignment();
    for (int i = 0; i < params.size(); i++) {
        auto argVal = visit(params[i]).as<llvm::Value*>();
        llvm::Type* paramTy = calleeFunTy->getParamType(i);
        llvm::Value* bitCastArgVal = builder->CreateBitCast(argVal, paramTy);
        argValues.push_back(bitCastArgVal);
    }
    return (llvm::Value*) builder->CreateCall(calleeFun, argValues);
}

antlrcpp::Any GeneratorVisitor::visitDataType(SpiceParser::DataTypeContext *ctx) {
    if (ctx->TYPE_DOUBLE()) return llvm::Type::getDoubleTy(*context);

    if (ctx->TYPE_INT()) return llvm::Type::getInt32Ty(*context);

    if (ctx->TYPE_BOOL()) return llvm::Type::getInt1Ty(*context);
}

std::string GeneratorVisitor::getIRString() {
    std::string output;
    llvm::raw_string_ostream oss(output);
    module->print(oss, nullptr);
    return oss.str();
}

void GeneratorVisitor::initializeExternalFunctions() {
    module->getOrInsertFunction("printf", llvm::FunctionType::get(
            llvm::IntegerType::getInt32Ty(*context),
            llvm::Type::getInt8Ty(*context)->getPointerTo(), true));
}
