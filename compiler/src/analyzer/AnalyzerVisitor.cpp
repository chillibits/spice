// Copyright (c) 2021 ChilliBits. All rights reserved.

#include "AnalyzerVisitor.h"

antlrcpp::Any AnalyzerVisitor::visitEntry(SpiceParser::EntryContext* ctx) {
    // Pre-traversing actions

    // Traverse AST
    visitChildren(ctx);

    // Check if the visitor got a main function
    if (mustHaveMainFunction && !hasMainFunction)
        throw SemanticError(*ctx->start, MISSING_MAIN_FUNCTION, "No main function found");

    // Post traversing actions

    // Return the symbol table to the main program for further compile phases
    return currentScope;
}

antlrcpp::Any AnalyzerVisitor::visitMainFunctionDef(SpiceParser::MainFunctionDefContext* ctx) {
    std::string mainSignature = "main()";
    // Check if the function is already defined
    if (currentScope->lookup(mainSignature)) {
        throw SemanticError(*ctx->start, FUNCTION_DECLARED_TWICE,
                            "Main function is declared twice");
    }
    // Insert function name into the root symbol table
    currentScope->insert(mainSignature, SymbolType(TYPE_FUNCTION), INITIALIZED, true, false);
    // Create a new scope
    currentScope = currentScope->createChildBlock("main()");
    // Declare variable for the return value
    SymbolType returnType = SymbolType(TYPE_INT);
    currentScope->insert(RETURN_VARIABLE_NAME, returnType, INITIALIZED, false, false);
    // Visit parameters
    parameterMode = true;
    if (ctx->paramLstDef()) visit(ctx->paramLstDef());
    parameterMode = false;
    // Visit statements in new scope
    visit(ctx->stmtLst());
    // Return to old scope
    currentScope = currentScope->getParent();
    // Confirm main function
    hasMainFunction = true;
    return returnType;
}

antlrcpp::Any AnalyzerVisitor::visitFunctionDef(SpiceParser::FunctionDefContext* ctx) {
    std::string functionName = ctx->IDENTIFIER()->toString();
    // Create a new scope
    std::string scopeId = ScopeIdUtil::getScopeId(ctx);
    currentScope = currentScope->createChildBlock(scopeId);
    // Visit params in new scope
    parameterMode = true;
    std::vector<SymbolType> paramTypes;
    if (ctx->paramLstDef())
        paramTypes = visit(ctx->paramLstDef()).as<std::vector<SymbolType>>();
    parameterMode = false;
    // Declare variable for the return value in new scope
    SymbolType returnType = getSymbolTypeFromDataType(ctx->dataType());
    currentScope->insert(RETURN_VARIABLE_NAME, returnType, DECLARED, false, false);
    // Return to old scope
    currentScope = currentScope->getParent();
    // Insert function into the symbol table
    FunctionSignature signature = FunctionSignature(functionName, paramTypes);
    // Check if the function is already defined
    if (currentScope->lookup(signature.toString())) {
        throw SemanticError(*ctx->start, FUNCTION_DECLARED_TWICE,
                            "Function '" + signature.toString() + "' is declared twice");
    }
    currentScope->insert(signature.toString(), SymbolType(TYPE_FUNCTION), INITIALIZED, true, false);
    currentScope->pushSignature(signature);
    // Rename function scope block to support function overloading
    currentScope->renameChildBlock(scopeId, signature.toString());
    // Go down again in scope
    currentScope = currentScope->getChild(signature.toString());
    // Visit statements in new scope
    visit(ctx->stmtLst());
    // Check if return variable is now initialized
    if (currentScope->lookup(RETURN_VARIABLE_NAME)->getState() == DECLARED)
        throw SemanticError(*ctx->start, FUNCTION_WITHOUT_RETURN_STMT, "Function without return statement");
    // Return to old scope
    currentScope = currentScope->getParent();
    return returnType;
}

antlrcpp::Any AnalyzerVisitor::visitProcedureDef(SpiceParser::ProcedureDefContext* ctx) {
    std::string procedureName = ctx->IDENTIFIER()->toString();
    // Create a new scope
    std::string scopeId = ScopeIdUtil::getScopeId(ctx);
    currentScope = currentScope->createChildBlock(scopeId);
    // Visit params in new scope
    parameterMode = true;
    std::vector<SymbolType> paramTypes;
    if (ctx->paramLstDef())
        paramTypes = visit(ctx->paramLstDef()).as<std::vector<SymbolType>>();
    parameterMode = false;
    // Return to old scope
    currentScope = currentScope->getParent();
    // Insert procedure into the symbol table
    FunctionSignature signature = FunctionSignature(procedureName, paramTypes);
    // Check if the procedure is already defined
    if (currentScope->lookup(signature.toString())) {
        throw SemanticError(*ctx->start, PROCEDURE_DECLARED_TWICE,
                            "Procedure '" + signature.toString() + "' is declared twice");
    }
    currentScope->insert(signature.toString(), SymbolType(TYPE_PROCEDURE), INITIALIZED, true, false);
    currentScope->pushSignature(signature);
    // Rename function scope block to support function overloading
    currentScope->renameChildBlock(scopeId, signature.toString());
    // Go down again in scope
    currentScope = currentScope->getChild(signature.toString());
    // Visit statement list in new scope
    visit(ctx->stmtLst());
    // Return to old scope
    currentScope = currentScope->getParent();
    return SymbolType(TYPE_BOOL);
}

antlrcpp::Any AnalyzerVisitor::visitStructDef(SpiceParser::StructDefContext* ctx) {
    std::string structName = ctx->IDENTIFIER()->toString();
    // Check if struct already exists in this scope
    if (currentScope->lookup(structName))
        throw SemanticError(*ctx->start, STRUCT_DECLARED_TWICE, "Duplicate struct '" + structName + "'");
    // Create a new table entry for the struct
    currentScope->insert(structName, SymbolType(TYPE_STRUCT, structName), DECLARED, true, false);
    // Visit field list in a new scope
    std::string scopeId = ScopeIdUtil::getScopeId(ctx);
    currentScope = currentScope->createChildBlock(scopeId);
    visit(ctx->fieldLst());
    // Return to the old scope
    currentScope = currentScope->getParent();
    return SymbolType(TYPE_BOOL);
}

antlrcpp::Any AnalyzerVisitor::visitForLoop(SpiceParser::ForLoopContext* ctx) {
    // Create a new scope
    std::string scopeId = ScopeIdUtil::getScopeId(ctx);
    currentScope = currentScope->createChildBlock(scopeId);
    // Visit assignment in new scope
    visit(ctx->assignment()[0]);
    // Visit condition in new scope
    SymbolType conditionType = visit(ctx->assignment()[1]).as<SymbolType>();
    if (conditionType.getSuperType() != TYPE_BOOL)
        throw SemanticError(*ctx->assignment()[1]->start, CONDITION_MUST_BE_BOOL,
                            "For loop condition must be of type bool");
    // Visit incrementer in new scope
    visit(ctx->assignment()[2]);
    // Visit statement list in new scope
    nestedLoopCounter++;
    visit(ctx->stmtLst());
    nestedLoopCounter--;
    // Return to old scope
    currentScope = currentScope->getParent();
    return SymbolType(TYPE_BOOL);
}

antlrcpp::Any AnalyzerVisitor::visitWhileLoop(SpiceParser::WhileLoopContext* ctx) {
    // Create a new scope
    std::string scopeId = ScopeIdUtil::getScopeId(ctx);
    currentScope = currentScope->createChildBlock(scopeId);
    // Visit condition
    SymbolType conditionType = visit(ctx->assignment()).as<SymbolType>();
    if (conditionType.getSuperType() != TYPE_BOOL)
        throw SemanticError(*ctx->assignment()->start, CONDITION_MUST_BE_BOOL,
                            "While loop condition must be of type bool");
    // Visit statement list in new scope
    nestedLoopCounter++;
    visit(ctx->stmtLst());
    nestedLoopCounter--;
    // Return to old scope
    currentScope = currentScope->getParent();
    return SymbolType(TYPE_BOOL);
}

antlrcpp::Any AnalyzerVisitor::visitIfStmt(SpiceParser::IfStmtContext* ctx) {
    // Create a new scope
    std::string scopeId = ScopeIdUtil::getScopeId(ctx);
    currentScope = currentScope->createChildBlock(scopeId);
    // Visit condition
    SymbolType conditionType = visit(ctx->assignment()).as<SymbolType>();
    if (conditionType.getSuperType() != TYPE_BOOL)
        throw SemanticError(*ctx->assignment()->start, CONDITION_MUST_BE_BOOL,
                            "If condition must be of type bool");
    // Visit statement list in new scope
    visit(ctx->stmtLst());
    // Return to old scope
    currentScope = currentScope->getParent();
    // Visit else statement if it exists
    if (ctx->elseStmt()) visit(ctx->elseStmt());
    return SymbolType(TYPE_BOOL);
}

antlrcpp::Any AnalyzerVisitor::visitElseStmt(SpiceParser::ElseStmtContext* ctx) {
    if (ctx->ifStmt()) { // Visit if statement in the case of an else if branch
        visit(ctx->ifStmt());
    } else { // Make a new scope in case of an else branch
        // Create a new scope
        std::string scopeId = ScopeIdUtil::getScopeId(ctx);
        currentScope = currentScope->createChildBlock(scopeId);
        // Visit statement list in new scope
        visit(ctx->stmtLst());
        // Return to old scope
        currentScope = currentScope->getParent();
    }
    return SymbolType(TYPE_BOOL);
}

antlrcpp::Any AnalyzerVisitor::visitParamLstDef(SpiceParser::ParamLstDefContext* ctx) {
    std::vector<SymbolType> paramTypes;
    for (auto& param : ctx->declStmt()) { // Parameters without default value
        SymbolType paramType = visit(param).as<SymbolType>();
        std::string paramName = param->IDENTIFIER()->toString();
        if (paramType.getSuperType() == TYPE_DYN)
            throw SemanticError(*param->start, FCT_PARAM_IS_TYPE_DYN,
                                "Type of parameter '" + paramName + "' is invalid");
        paramTypes.push_back(paramType);
    }
    for (auto& param : ctx->assignment()) { // Parameters with default value
        SymbolType paramType = visit(param).as<SymbolType>();
        paramTypes.push_back(paramType);
    }
    return paramTypes;
}

antlrcpp::Any AnalyzerVisitor::visitDeclStmt(SpiceParser::DeclStmtContext* ctx) {
    std::string variableName = ctx->IDENTIFIER()->toString();
    // Check if symbol already exists in the symbol table
    if (currentScope->lookup(variableName))
        throw SemanticError(*ctx->start, VARIABLE_DECLARED_TWICE,
                            "The variable '" + variableName + "' was declared more than once");
    // Insert variable name to symbol table
    SymbolType type = getSymbolTypeFromDataType(ctx->dataType());
    currentScope->insert(variableName, type, DECLARED, ctx->CONST(), parameterMode);
    return type;
}

antlrcpp::Any AnalyzerVisitor::visitFunctionCall(SpiceParser::FunctionCallContext* ctx) {
    std::vector<std::string> functionNamespace;
    for (auto& segment : ctx->IDENTIFIER()) functionNamespace.push_back(segment->toString());
    std::string functionName = functionNamespace.back();
    // Visit params
    std::vector<SymbolType> paramTypes;
    if (ctx->paramLstCall()) {
        for (auto& param : ctx->paramLstCall()->assignment())
            paramTypes.push_back(visit(param).as<SymbolType>());
    }
    // Check if function signature exists in symbol table
    FunctionSignature signature = FunctionSignature(functionName, paramTypes);
    functionNamespace.back() = signature.toString();
    SymbolTable* entryTable = currentScope->lookupTableWithSymbol(functionNamespace);
    if (!entryTable)
        throw SemanticError(*ctx->start, REFERENCED_UNDEFINED_FUNCTION_OR_PROCEDURE,
                            "Function/Procedure '" + signature.toString() + "' could not be found");
    SymbolTableEntry* entry = entryTable->lookup(signature.toString());
    if (!entry)
        throw SemanticError(*ctx->start, REFERENCED_UNDEFINED_FUNCTION_OR_PROCEDURE,
                            "Function/Procedure '" + signature.toString() + "' could not be found");
    // Add function call to the signature queue of the current scope
    currentScope->pushSignature(signature);
    // Search for symbol table of called function/procedure to read parameters
    if (entry->getType().getSuperType() == TYPE_FUNCTION) {
        entryTable = entryTable->getChild(signature.toString());
        // Get return type of called function
        return entryTable->lookup(RETURN_VARIABLE_NAME)->getType();
    }
    return SymbolType(TYPE_BOOL);
}

antlrcpp::Any AnalyzerVisitor::visitNewStmt(SpiceParser::NewStmtContext* ctx) {
    std::string structName = ctx->IDENTIFIER()->toString();
    std::string structScope = ScopeIdUtil::getScopeId(ctx);
    // Check if the struct is defined
    SymbolTableEntry* structSymbol = currentScope->lookup(structName);
    if (!structSymbol || structSymbol->getType().getSuperType() != TYPE_STRUCT ||
        structSymbol->getType().getSubType() != structName) {
        throw SemanticError(*ctx->IDENTIFIER()->getSymbol(), REFERENCED_UNDEFINED_STRUCT,
                            "Struct '" + structName + "' was used before defined.");
    }

    // Get the symbol table where the struct is defined
    SymbolTable* structTable = currentScope->lookupTable(structScope);
    std::cout << "Struct table: " << structTable->toString() << std::endl;
    // Check if the number of fields matches
    if (structTable->getSymbolsCount() != ctx->fieldLstAssignment()->ternary().size())
        throw SemanticError(*ctx->start, NUMBER_OF_FIELDS_NOT_MATCHING, "You've passed too less/many field values");

    // Check if the field types are matching
    for (int i = 0; i < ctx->fieldLstAssignment()->ternary().size(); i++) {
        // Get actual type
        auto ternary = ctx->fieldLstAssignment()->ternary()[i];
        SymbolType actualType = visit(ternary).as<SymbolType>();
        // Get expected type
        SymbolTableEntry* expectedField = structTable->lookupByIndexInCurrentScope(i);
        SymbolType expectedType = expectedField->getType();
        // Check if type matches declaration
        if (actualType != expectedType)
            throw SemanticError(*ternary->start, FIELD_TYPE_NOT_MATCHING,
                                "The type of the field '" + expectedField->getName() + "' does not match the declaration");
    }

    return SymbolType(TYPE_STRUCT, structName);
}

antlrcpp::Any AnalyzerVisitor::visitImportStmt(SpiceParser::ImportStmtContext* ctx) {
    // Check if imported library exists
    std::string importPath = ctx->STRING()->toString();
    importPath = importPath.substr(1, importPath.size() - 2);

    // Check if source file exists
    std::string filePath;
    if (importPath.rfind("std/", 0) == 0) { // Include source file from standard library
        std::string sourceFileIden = importPath.substr(importPath.find("std/") + 4);
        // Find std library
        std::string stdPath;
        if (FileUtil::fileExists("/usr/lib/spice/std")) {
            stdPath = "/usr/lib/spice/std/";
        } else if (FileUtil::dirExists(std::string(std::getenv("SPICE_STD_DIR")))) {
            stdPath = std::string(std::getenv("SPICE_STD_DIR"));
            if (stdPath.rfind('/') != stdPath.size() - 1) {
                stdPath += "/";
            }
        } else {
            throw SemanticError(STD_NOT_FOUND,
                                "Standard library could not be found. Check if the env var SPICE_STD_DIR exists");
        }
        // Check if source file exists
        if (FileUtil::fileExists(stdPath + sourceFileIden + ".spice")) {
            filePath = stdPath + sourceFileIden + ".spice";
        } else if (FileUtil::fileExists(stdPath + sourceFileIden + "_" + targetOs + ".spice")) {
            filePath = stdPath + sourceFileIden + "_" + targetOs + ".spice";
        } else if (FileUtil::fileExists(stdPath + sourceFileIden + "_" + targetOs + "_" + targetArch + ".spice")) {
            filePath = stdPath + sourceFileIden + "_" + targetOs + "_" + targetArch + ".spice";
        } else {
            throw SemanticError(IMPORTED_FILE_NOT_EXISTING, "The source file '" + importPath +
                                                            ".spice' was not found in std library");
        }
    } else { // Include own source file
        // Check in module registry if the file can be imported
        std::string sourceFileDir = FileUtil::getFileDir(mainSourceFile);
        ModuleRegistry* registry = ModuleRegistry::getInstance();
        registry->addModule(sourceFileDir + "/" + importPath);
        // Import file
        if (FileUtil::fileExists(sourceFileDir + "/" + importPath + ".spice")) {
            filePath = sourceFileDir + "/" + importPath + ".spice";
        } else if (FileUtil::fileExists(sourceFileDir + "/" + importPath + "_" + targetOs + ".spice")) {
            filePath = sourceFileDir + "/" + importPath + "_" + targetOs + ".spice";
        } else if (FileUtil::fileExists(sourceFileDir + "/" + importPath + "_" + targetOs + "_" + targetArch + ".spice")) {
            filePath = sourceFileDir + "/" + importPath + "_" + targetOs + "_" + targetArch + ".spice";
        } else {
            throw SemanticError(IMPORTED_FILE_NOT_EXISTING, "The source file '" + importPath +
                                                            ".spice' does not exist");
        }
    }

    // Kick off the compilation of the imported source file
    SymbolTable* nestedTable = CompilerInstance::CompileSourceFile(filePath, targetArch, targetVendor, targetOs, objectDir,
                                                                   debugOutput, optLevel, false);

    // Create symbol of type TYPE_IMPORT in the current scope
    std::string importIden = ctx->IDENTIFIER()->toString();
    currentScope->insert(importIden, SymbolType(TYPE_IMPORT), INITIALIZED, true, false);

    // Mount symbol table of the imported source file into the current scope
    currentScope->mountChildBlock(importIden, nestedTable);

    return SymbolType(TYPE_STRING);
}

antlrcpp::Any AnalyzerVisitor::visitReturnStmt(SpiceParser::ReturnStmtContext* ctx) {
    SymbolType returnType = visit(ctx->assignment()).as<SymbolType>();
    // Check if return variable is in the symbol table
    SymbolTableEntry* returnVariable = currentScope->lookup(RETURN_VARIABLE_NAME);
    if (!returnVariable)
        throw SemanticError(*ctx->start, RETURN_STMT_WITHOUT_FUNCTION,
                            "Cannot assign return statement to a function");
    // Check data type of return statement
    if (returnVariable->getType().getSuperType() == TYPE_DYN) {
        // Set explicit return type to the return variable
        returnVariable->updateType(returnType);
    } else {
        // Check if return type matches with function definition
        if (returnType != returnVariable->getType())
            throw SemanticError(*ctx->assignment()->start, OPERATOR_WRONG_DATA_TYPE,
                                "Passed wrong data type to return statement");
    }
    // Set the return variable to initialized
    returnVariable->updateState(INITIALIZED);
    return returnType;
}

antlrcpp::Any AnalyzerVisitor::visitBreakStmt(SpiceParser::BreakStmtContext* ctx) {
    int breakCount = 1;
    if (ctx->INTEGER()) {
        // Check if the stated number is valid
        breakCount = std::stoi(ctx->INTEGER()->toString());
        if (breakCount < 1)
            throw SemanticError(*ctx->INTEGER()->getSymbol(), INVALID_BREAK_NUMBER,
                                "Break count must be >= 1: " + ctx->INTEGER()->toString());
    }
    // Check if we can break this often
    if (breakCount > nestedLoopCounter)
        throw SemanticError(*ctx->INTEGER()->getSymbol(), INVALID_BREAK_NUMBER,
                            "We can only break " + std::to_string(nestedLoopCounter) + " time(s) here");
    return SymbolType(TYPE_INT);
}

antlrcpp::Any AnalyzerVisitor::visitContinueStmt(SpiceParser::ContinueStmtContext* ctx) {
    int continueCount = 1;
    if (ctx->INTEGER()) {
        // Check if the stated number is valid
        continueCount = std::stoi(ctx->INTEGER()->toString());
        if (continueCount < 1)
            throw SemanticError(*ctx->INTEGER()->getSymbol(), INVALID_CONTINUE_NUMBER,
                                "Continue count must be >= 1: " + ctx->INTEGER()->toString());
    }
    // Check if we can continue this often
    if (continueCount > nestedLoopCounter)
        throw SemanticError(*ctx->INTEGER()->getSymbol(), INVALID_CONTINUE_NUMBER,
                            "We can only continue " + std::to_string(nestedLoopCounter) + " time(s) here");
    return SymbolType(TYPE_INT);
}

antlrcpp::Any AnalyzerVisitor::visitPrintfStmt(SpiceParser::PrintfStmtContext* ctx) {
    std::string templateString = ctx->STRING()->toString();
    templateString = templateString.substr(1, templateString.size() - 2);

    // Check if assignment types match placeholder types
    std::size_t index = templateString.find_first_of('%');
    int placeholderCount = 0;
    while (index != std::string::npos) {
        auto assignment = ctx->assignment()[placeholderCount];
        SymbolType assignmentType = visit(assignment).as<SymbolType>();
        switch (templateString[index + 1]) {
            case 'c':
            case 'd':
            case 'i':
            case 'o':
            case 'x':
            case 'X': {
                if (assignmentType.getSuperType() != TYPE_INT && assignmentType.getSuperType() != TYPE_BOOL)
                    throw SemanticError(*assignment->start, PRINTF_TYPE_ERROR,
                                        "Template string expects an int or a bool here");
                placeholderCount++;
                break;
            }
            case 'a':
            case 'A':
            case 'f':
            case 'F':
            case 'e':
            case 'E':
            case 'g':
            case 'G': {
                if (assignmentType.getSuperType() != TYPE_DOUBLE)
                    throw SemanticError(*assignment->start, PRINTF_TYPE_ERROR,
                                        "Template string expects a double here");
                placeholderCount++;
                break;
            }
            case 's': {
                if (assignmentType.getSuperType() != TYPE_STRING)
                    throw SemanticError(*assignment->start, PRINTF_TYPE_ERROR,
                                        "Template string expects a string here");
                placeholderCount++;
                break;
            }
            case 'p': {
                if (assignmentType.getSuperType() != TYPE_DOUBLE_PTR && assignmentType.getSuperType() != TYPE_INT_PTR &&
                    assignmentType.getSuperType() != TYPE_STRING_PTR && assignmentType.getSuperType() != TYPE_BOOL_PTR)
                    throw SemanticError(*assignment->start, PRINTF_TYPE_ERROR,
                                        "Template string expects a pointer here");
                placeholderCount++;
                break;
            }
        }
        index = templateString.find_first_of('%', index + 1);
    }

    return SpiceBaseVisitor::visitPrintfStmt(ctx);
}

antlrcpp::Any AnalyzerVisitor::visitAssignment(SpiceParser::AssignmentContext* ctx) {
    // Take a look on the right side
    SymbolType rightType;
    if (ctx->ternary()) { // Ternary
        rightType = visit(ctx->ternary()).as<SymbolType>();
    } else { // NewStmt
        rightType = visit(ctx->newStmt()).as<SymbolType>();
    }

    // Check if there is an assign operator applied
    if (ctx->declStmt() || !ctx->IDENTIFIER().empty()) {
        // Take a look on the left side
        std::string variableName;
        SymbolType leftType;
        antlr4::Token* token;
        if (ctx->declStmt()) { // Variable was declared in this line
            visit(ctx->declStmt());
            variableName = ctx->declStmt()->IDENTIFIER()->toString();
            token = ctx->declStmt()->IDENTIFIER()->getSymbol();
        } else { // Variable was declared before and is referenced here
            variableName = IdentifierUtil::getVarNameFromIdentList(ctx->IDENTIFIER());
            token = ctx->IDENTIFIER()[0]->getSymbol();
        }
        // Retrieve the left type from the symbol table
        SymbolTableEntry* symbolTableEntry = currentScope->lookup(variableName);
        if (symbolTableEntry == nullptr)
            throw SemanticError(*token, REFERENCED_UNDEFINED_VARIABLE,
                                "Variable " + variableName + " was referenced before declared.");
        leftType = symbolTableEntry->getType();

        // If it is a pointer to the value, resolve the type
        if (ctx->MUL()) leftType = getTypeFromReferenceType(leftType);

        // If left type is dyn, set left type to right type
        if (leftType.getSuperType() == TYPE_DYN) {
            leftType = rightType;
            symbolTableEntry->updateType(rightType);
        }

        // Take a look at the operator
        if (ctx->ASSIGN_OP()) {
            if (leftType != rightType)
                throw SemanticError(*ctx->ASSIGN_OP()->getSymbol(), OPERATOR_WRONG_DATA_TYPE,
                                    "Cannot apply the assign operator to different data types");
        } else if (ctx->PLUS_EQUAL()) {
            if (!(leftType.getSuperType() == TYPE_DOUBLE && rightType.getSuperType() == TYPE_DOUBLE) &&
                !(leftType.getSuperType() == TYPE_INT && rightType.getSuperType() == TYPE_INT) &&
                !(leftType.getSuperType() == TYPE_STRING && rightType.getSuperType() == TYPE_STRING)) {
                throw SemanticError(*ctx->PLUS_EQUAL()->getSymbol(), OPERATOR_WRONG_DATA_TYPE,
                                    "Cannot apply += operator on this combination of types");
            }
        } else if (ctx->MINUS_EQUAL()) {
            if (!(leftType.getSuperType() == TYPE_DOUBLE && rightType.getSuperType() == TYPE_DOUBLE) &&
                !(leftType.getSuperType() == TYPE_INT && rightType.getSuperType() == TYPE_INT)) {
                throw SemanticError(*ctx->MINUS_EQUAL()->getSymbol(), OPERATOR_WRONG_DATA_TYPE,
                                    "Cannot apply -= operator on this combination of types");
            }
        } else if (ctx->MUL_EQUAL()) {
            if (!(leftType.getSuperType() == TYPE_DOUBLE && rightType.getSuperType() == TYPE_DOUBLE) &&
                !(leftType.getSuperType() == TYPE_INT && rightType.getSuperType() == TYPE_INT)) {
                throw SemanticError(*ctx->MUL_EQUAL()->getSymbol(), OPERATOR_WRONG_DATA_TYPE,
                                    "Cannot apply *= operator on this combination of types");
            }
        } else if (ctx->DIV_EQUAL()) {
            if (!(leftType.getSuperType() == TYPE_DOUBLE && rightType.getSuperType() == TYPE_DOUBLE) &&
                !(leftType.getSuperType() == TYPE_INT && rightType.getSuperType() == TYPE_INT)) {
                throw SemanticError(*ctx->DIV_EQUAL()->getSymbol(), OPERATOR_WRONG_DATA_TYPE,
                                    "Cannot apply /= operator on this combination of types");
            }
        }
        // Update variable in symbol table
        symbolTableEntry->updateState(INITIALIZED);
    }

    // Return the rhs type
    return rightType;
}

antlrcpp::Any AnalyzerVisitor::visitTernary(SpiceParser::TernaryContext* ctx) {
    // Check if there is a ternary operator applied
    if (ctx->children.size() > 1) {
        auto condition = ctx->logicalOrExpr()[0];
        SymbolType conditionType = visit(condition).as<SymbolType>();
        SymbolType trueType = visit(ctx->logicalOrExpr()[1]).as<SymbolType>();
        SymbolType falseType = visit(ctx->logicalOrExpr()[2]).as<SymbolType>();
        // Check if the condition evaluates to boolean
        if (conditionType.getSuperType() != TYPE_BOOL)
            throw SemanticError(*condition->start, OPERATOR_WRONG_DATA_TYPE,
                                "Condition operand in ternary must be a bool");
        // Check if trueType and falseType are matching
        if (trueType != falseType)
            throw SemanticError(*ctx->start, OPERATOR_WRONG_DATA_TYPE,
                                "True operand and false operand in ternary must be from same data type");
        return trueType;
    }
    return visit(ctx->logicalOrExpr()[0]);
}

antlrcpp::Any AnalyzerVisitor::visitLogicalOrExpr(SpiceParser::LogicalOrExprContext* ctx) {
    // Check if a logical or operator is applied
    if (ctx->children.size() > 1) {
        SymbolType leftType = visit(ctx->logicalAndExpr()[0]).as<SymbolType>();
        for (int i = 1; i < ctx->logicalAndExpr().size(); i++) {
            SymbolType rightType = visit(ctx->logicalAndExpr()[i]).as<SymbolType>();
            // Allow logical or operator for booleans
            if (leftType.getSuperType() == TYPE_BOOL && rightType.getSuperType() == TYPE_BOOL) {
                leftType = SymbolType(TYPE_BOOL);
                continue;
            }
            // Every other combination is invalid
            throw SemanticError(*ctx->start, OPERATOR_WRONG_DATA_TYPE,"Can only apply logical or to booleans");
        }
        return leftType;
    }
    return visit(ctx->logicalAndExpr()[0]);
}

antlrcpp::Any AnalyzerVisitor::visitLogicalAndExpr(SpiceParser::LogicalAndExprContext* ctx) {
    // Check if a logical and operator is applied
    if (ctx->children.size() > 1) {
        SymbolType leftType = visit(ctx->bitwiseOrExpr()[0]).as<SymbolType>();
        for (int i = 1; i < ctx->bitwiseOrExpr().size(); i++) {
            SymbolType rightType = visit(ctx->bitwiseOrExpr()[i]).as<SymbolType>();
            // Allow logical and operator for booleans
            if (leftType.getSuperType() == TYPE_BOOL && rightType.getSuperType() == TYPE_BOOL) {
                leftType = SymbolType(TYPE_BOOL);
                continue;
            }
            // Every other combination is invalid
            throw SemanticError(*ctx->start, OPERATOR_WRONG_DATA_TYPE,
                                "Can only apply logical and to booleans");
        }
        return leftType;
    }
    return visit(ctx->bitwiseOrExpr()[0]);
}

antlrcpp::Any AnalyzerVisitor::visitBitwiseOrExpr(SpiceParser::BitwiseOrExprContext* ctx) {
    // Check if a bitwise or operator is applied
    if (ctx->children.size() > 1) {
        SymbolType leftType = visit(ctx->bitwiseAndExpr()[0]).as<SymbolType>();
        for (int i = 1; i < ctx->bitwiseAndExpr().size(); i++) {
            SymbolType rightType = visit(ctx->bitwiseAndExpr()[i]).as<SymbolType>();
            // Allow bitwise or operator for booleans
            if (leftType.getSuperType() == TYPE_BOOL && rightType.getSuperType() == TYPE_BOOL) {
                leftType = SymbolType(TYPE_BOOL);
                continue;
            }
            // Allow bitwise or operator for integers
            if (leftType.getSuperType() == TYPE_INT && rightType.getSuperType() == TYPE_INT) {
                leftType = SymbolType(TYPE_INT);
                continue;
            }
            // Every other combination is invalid
            throw SemanticError(*ctx->start, OPERATOR_WRONG_DATA_TYPE,
                                "Can only apply bitwise or to booleans and integers");
        }
        return leftType;
    }
    return visit(ctx->bitwiseAndExpr()[0]);
}

antlrcpp::Any AnalyzerVisitor::visitBitwiseAndExpr(SpiceParser::BitwiseAndExprContext* ctx) {
    // Check if a bitwise and operator is applied
    if (ctx->children.size() > 1) {
        SymbolType leftType = visit(ctx->equalityExpr()[0]).as<SymbolType>();
        for (int i = 1; i < ctx->equalityExpr().size(); i++) {
            SymbolType rightType = visit(ctx->equalityExpr()[i]).as<SymbolType>();
            // Allow bitwise and operator for booleans
            if (leftType.getSuperType() == TYPE_BOOL && rightType.getSuperType() == TYPE_BOOL) {
                leftType = SymbolType(TYPE_BOOL);
                continue;
            }
            // Allow bitwise and operator for integers
            if (leftType.getSuperType() == TYPE_INT && rightType.getSuperType() == TYPE_INT) {
                leftType = SymbolType(TYPE_INT);
                continue;
            }
            // Every other combination is invalid
            throw SemanticError(*ctx->start, OPERATOR_WRONG_DATA_TYPE,
                                "Can only apply bitwise and to booleans and integers");
        }
        return leftType;
    }
    return visit(ctx->equalityExpr()[0]);
}

antlrcpp::Any AnalyzerVisitor::visitEqualityExpr(SpiceParser::EqualityExprContext* ctx) {
    // Check if at least one equality operator is applied
    if (ctx->children.size() > 1) {
        SymbolType leftType = visit(ctx->relationalExpr()[0]).as<SymbolType>();
        SymbolType rightType = visit(ctx->relationalExpr()[1]).as<SymbolType>();
        if (leftType.getSuperType() == TYPE_DOUBLE && rightType.getSuperType() == TYPE_DOUBLE)
            return SymbolType(TYPE_BOOL); // Can compare double with double
        if (leftType.getSuperType() == TYPE_DOUBLE && rightType.getSuperType() == TYPE_INT)
            return SymbolType(TYPE_BOOL); // Can compare double with int
        if (leftType.getSuperType() == TYPE_INT && rightType.getSuperType() == TYPE_DOUBLE)
            return SymbolType(TYPE_BOOL); // Can compare int with double
        if (leftType.getSuperType() == TYPE_INT && rightType.getSuperType() == TYPE_INT)
            return SymbolType(TYPE_BOOL); // Can compare int with int
        if (leftType.getSuperType() == TYPE_STRING && rightType.getSuperType() == TYPE_STRING)
            return SymbolType(TYPE_BOOL); // Can compare string with string
        if (leftType.getSuperType() == TYPE_BOOL && rightType.getSuperType() == TYPE_BOOL)
            return SymbolType(TYPE_BOOL); // Can compare bool with bool
        // Every other combination is invalid
        throw SemanticError(*ctx->start, OPERATOR_WRONG_DATA_TYPE,
                            "Can only compare some type combinations with an equality operator");
    }
    return visit(ctx->relationalExpr()[0]);
}

antlrcpp::Any AnalyzerVisitor::visitRelationalExpr(SpiceParser::RelationalExprContext* ctx) {
    // Check if at least one relational operator is applied
    if (ctx->children.size() > 1) {
        SymbolType leftType = visit(ctx->additiveExpr()[0]).as<SymbolType>();
        SymbolType rightType = visit(ctx->additiveExpr()[1]).as<SymbolType>();
        if (leftType.getSuperType() == TYPE_DOUBLE && rightType.getSuperType() == TYPE_DOUBLE)
            return SymbolType(TYPE_BOOL); // Can compare double with double
        if (leftType.getSuperType() == TYPE_DOUBLE && rightType.getSuperType() == TYPE_INT)
            return SymbolType(TYPE_BOOL); // Can compare double with int
        if (leftType.getSuperType() == TYPE_INT && rightType.getSuperType() == TYPE_DOUBLE)
            return SymbolType(TYPE_BOOL); // Can compare int with double
        if (leftType.getSuperType() == TYPE_INT && rightType.getSuperType() == TYPE_INT)
            return SymbolType(TYPE_BOOL); // Can compare int with int
        // Every other combination is invalid
        throw SemanticError(*ctx->start, OPERATOR_WRONG_DATA_TYPE,
                            "Can only compare doubles or ints with one another with a relational operator");
    }
    return visit(ctx->additiveExpr()[0]);
}

antlrcpp::Any AnalyzerVisitor::visitAdditiveExpr(SpiceParser::AdditiveExprContext* ctx) {
    // Check if at least one additive operator is applied
    if (ctx->children.size() > 1) {
        SymbolType currentType = visit(ctx->multiplicativeExpr()[0]).as<SymbolType>();
        // Check if data types are compatible
        for (int i = 1; i < ctx->multiplicativeExpr().size(); i++) {
            auto next = ctx->multiplicativeExpr()[i];
            SymbolType nextType = visit(next).as<SymbolType>();
            // Check all combinations
            if (currentType.getSuperType() == TYPE_DOUBLE) {
                if (nextType.getSuperType() == TYPE_DOUBLE) { // e.g.: 4.3 + 6.1
                    currentType = SymbolType(TYPE_DOUBLE);
                } else if (nextType.getSuperType() == TYPE_INT) { // e.g.: 4.3 + 4
                    currentType = SymbolType(TYPE_DOUBLE);
                } else if (nextType.getSuperType() == TYPE_STRING) { // e.g.: 4.3 + "Test"
                    currentType = SymbolType(TYPE_STRING);
                } else if (nextType.getSuperType() == TYPE_BOOL) { // e.g.: 4.3 + true
                    throw SemanticError(*next->start, OPERATOR_WRONG_DATA_TYPE,
                                        "Incompatible operands double and bool for additive operator");
                }
            } else if (currentType.getSuperType() == TYPE_INT) {
                if (nextType.getSuperType() == TYPE_DOUBLE) { // e.g.: 4 + 6.1
                    currentType = SymbolType(TYPE_DOUBLE);
                } else if (nextType.getSuperType() == TYPE_INT) { // e.g.: 4 + 5
                    currentType = SymbolType(TYPE_INT);
                } else if (nextType.getSuperType() == TYPE_STRING) { // e.g.: 4 + "Test"
                    currentType = SymbolType(TYPE_STRING);
                } else if (nextType.getSuperType() == TYPE_BOOL) { // e.g.: 4 + true
                    throw SemanticError(*next->start, OPERATOR_WRONG_DATA_TYPE,
                                        "Incompatible operands int and bool for additive operator");
                }
            } else if (currentType.getSuperType() == TYPE_STRING) {
                if (nextType.getSuperType() == TYPE_DOUBLE) { // e.g.: "Test" + 6.1
                    currentType = SymbolType(TYPE_STRING);
                } else if (nextType.getSuperType() == TYPE_INT) { // e.g.: "Test" + 5
                    currentType = SymbolType(TYPE_STRING);
                } else if (nextType.getSuperType() == TYPE_STRING) { // e.g.: "Test" + "Test"
                    currentType = SymbolType(TYPE_STRING);
                } else if (nextType.getSuperType() == TYPE_BOOL) { // e.g.: "Test" + true
                    throw SemanticError(*next->start, OPERATOR_WRONG_DATA_TYPE,
                                        "Incompatible operands string and bool for additive operator");
                }
            } else if (currentType.getSuperType() == TYPE_BOOL) {
                if (nextType.getSuperType() == TYPE_DOUBLE) { // e.g.: true + 6.1
                    throw SemanticError(*next->start, OPERATOR_WRONG_DATA_TYPE,
                                        "Incompatible operands bool and double for additive operator");
                } else if (nextType.getSuperType() == TYPE_INT) { // e.g.: true + 5
                    throw SemanticError(*next->start, OPERATOR_WRONG_DATA_TYPE,
                                        "Incompatible operands bool and int for additive operator");
                } else if (nextType.getSuperType() == TYPE_STRING) { // e.g.: true + "Test"
                    throw SemanticError(*next->start, OPERATOR_WRONG_DATA_TYPE,
                                        "Incompatible operands string and string for additive operator");
                } else if (nextType.getSuperType() == TYPE_BOOL) { // e.g.: true + false
                    throw SemanticError(*next->start, OPERATOR_WRONG_DATA_TYPE,
                                        "Incompatible operands bool and bool for additive operator");
                }
            }
        }
        return currentType;
    }
    return visit(ctx->multiplicativeExpr()[0]);
}

antlrcpp::Any AnalyzerVisitor::visitMultiplicativeExpr(SpiceParser::MultiplicativeExprContext* ctx) {
    // Check if at least one multiplicative operator is applied
    if (ctx->children.size() > 1) {
        SymbolType currentType = visit(ctx->prefixUnary()[0]).as<SymbolType>();
        // Check if data types are compatible
        for (int i = 1; i < ctx->prefixUnary().size(); i++) {
            auto next = ctx->prefixUnary()[i];
            SymbolType nextType = visit(next).as<SymbolType>();
            // Check all combinations
            if (currentType.getSuperType() == TYPE_DOUBLE) {
                if (nextType.getSuperType() == TYPE_DOUBLE) { // e.g.: 4.3 * 6.1
                    currentType = SymbolType(TYPE_DOUBLE);
                } else if (nextType.getSuperType() == TYPE_INT) { // e.g.: 4.3 * 4
                    currentType = SymbolType(TYPE_DOUBLE);
                } else if (nextType.getSuperType() == TYPE_STRING) { // e.g.: 4.3 * "Test"
                    throw SemanticError(*next->start, OPERATOR_WRONG_DATA_TYPE,
                                        "Incompatible operands double and string for multiplicative operator");
                } else if (nextType.getSuperType() == TYPE_BOOL) { // e.g.: 4.3 * true
                    throw SemanticError(*next->start, OPERATOR_WRONG_DATA_TYPE,
                                        "Incompatible operands double and bool for multiplicative operator");
                }
            } else if (currentType.getSuperType() == TYPE_INT) {
                if (nextType.getSuperType() == TYPE_DOUBLE) { // e.g.: 4 * 6.1
                    currentType = SymbolType(TYPE_DOUBLE);
                } else if (nextType.getSuperType() == TYPE_INT) { // e.g.: 4 * 5
                    currentType = SymbolType(TYPE_INT);
                } else if (nextType.getSuperType() == TYPE_STRING) { // e.g.: 4 * "Test"
                    currentType = SymbolType(TYPE_STRING);
                } else if (nextType.getSuperType() == TYPE_BOOL) { // e.g.: 4 * true
                    throw SemanticError(*next->start, OPERATOR_WRONG_DATA_TYPE,
                                        "Incompatible operands int and bool for multiplicative operator");
                }
            } else if (currentType.getSuperType() == TYPE_STRING) {
                if (nextType.getSuperType() == TYPE_DOUBLE) { // e.g.: "Test" * 6.1
                    throw SemanticError(*next->start, OPERATOR_WRONG_DATA_TYPE,
                                        "Incompatible operands string and double for multiplicative operator");
                } else if (nextType.getSuperType() ==TYPE_INT) { // e.g.: "Test" * 5
                    currentType = SymbolType(TYPE_STRING);
                } else if (nextType.getSuperType() == TYPE_STRING) { // e.g.: "Test" * "Test"
                    throw SemanticError(*next->start, OPERATOR_WRONG_DATA_TYPE,
                                        "Incompatible operands string and string for multiplicative operator");
                } else if (nextType.getSuperType() == TYPE_BOOL) { // e.g.: "Test" * true
                    throw SemanticError(*next->start, OPERATOR_WRONG_DATA_TYPE,
                                        "Incompatible operands string and bool for multiplicative operator");
                }
            } else if (currentType.getSuperType() == TYPE_BOOL) {
                if (nextType.getSuperType() == TYPE_DOUBLE) { // e.g.: true * 6.1
                    throw SemanticError(*next->start, OPERATOR_WRONG_DATA_TYPE,
                                        "Incompatible operands bool and double for multiplicative operator");
                } else if (nextType.getSuperType() == TYPE_INT) { // e.g.: true * 5
                    throw SemanticError(*next->start, OPERATOR_WRONG_DATA_TYPE,
                                        "Incompatible operands bool and int for multiplicative operator");
                } else if (nextType.getSuperType() == TYPE_STRING) { // e.g.: true * "Test"
                    throw SemanticError(*next->start, OPERATOR_WRONG_DATA_TYPE,
                                        "Incompatible operands string and string for multiplicative operator");
                } else if (nextType.getSuperType() == TYPE_BOOL) { // e.g.: true * false
                    throw SemanticError(*next->start, OPERATOR_WRONG_DATA_TYPE,
                                        "Incompatible operands bool and bool for multiplicative operator");
                }
            }
        }
        return currentType;
    }
    return visit(ctx->prefixUnary()[0]);
}

antlrcpp::Any AnalyzerVisitor::visitPrefixUnary(SpiceParser::PrefixUnaryContext* ctx) {
    antlrcpp::Any prefixUnary = visit(ctx->postfixUnary());

    // Ensure integer when '++' or '--' is applied
    if (ctx->PLUS_PLUS() || ctx->MINUS_MINUS()) {
        if (prefixUnary.as<SymbolType>().getSuperType() != TYPE_INT || ctx->postfixUnary()->atomicExpr()->value()->IDENTIFIER().empty()) {
            throw SemanticError(*ctx->postfixUnary()->start, OPERATOR_WRONG_DATA_TYPE,
                                "Prefix '++' or '--' can only be applied to an identifier of type integer");
        }
    }

    // Ensure right return type if not is applied
    if (ctx->NOT()) {
        /* If not applied to double, return bool (evaluates later to variable == 0.0)
           If not applied to int, return bool (evaluates later to variable == 0)
           If not applied to string, return bool (evaluates later to variable == "")
           If not applied to bool, return bool (evaluates later to variable == false)*/
        return SymbolType(TYPE_BOOL);
    }

    return prefixUnary;
}

antlrcpp::Any AnalyzerVisitor::visitPostfixUnary(SpiceParser::PostfixUnaryContext* ctx) {
    antlrcpp::Any atomicExpr = visit(ctx->atomicExpr());

    // Ensure integer when '++' or '--' is applied
    if (ctx->PLUS_PLUS() || ctx->MINUS_MINUS()) {
        if (atomicExpr.as<SymbolType>().getSuperType() != TYPE_INT || ctx->atomicExpr()->value()->IDENTIFIER().empty()) {
            throw SemanticError(*ctx->atomicExpr()->start, OPERATOR_WRONG_DATA_TYPE,
                                "Postfix '++' or '--' can only be applied to an identifier of type integer");
        }
    }

    return atomicExpr;
}

antlrcpp::Any AnalyzerVisitor::visitAtomicExpr(SpiceParser::AtomicExprContext* ctx) {
    if (ctx->assignment()) return visit(ctx->assignment());
    return visit(ctx->value());
}

antlrcpp::Any AnalyzerVisitor::visitValue(SpiceParser::ValueContext* ctx) {
    if (ctx->DOUBLE()) return SymbolType(TYPE_DOUBLE);
    if (ctx->INTEGER()) return SymbolType(TYPE_INT);
    if (ctx->STRING()) return SymbolType(TYPE_STRING);
    if (ctx->TRUE() || ctx->FALSE()) return SymbolType(TYPE_BOOL);
    if (!ctx->IDENTIFIER().empty()) {
        std::string variableName = IdentifierUtil::getVarNameFromIdentList(ctx->IDENTIFIER());
        SymbolTableEntry* entry = currentScope->lookup(variableName);
        if (entry == nullptr)
            throw SemanticError(*ctx->IDENTIFIER()[0]->getSymbol(), REFERENCED_UNDEFINED_VARIABLE,
                                "Variable " + variableName + " was referenced before initialized");
        SymbolType valueType = entry->getType();
        // Check for referencing operator
        if (ctx->BITWISE_AND()) valueType = getReferenceTypeFromType(valueType);
        // Check for de-referencing operator
        if (ctx->MUL()) valueType = getTypeFromReferenceType(valueType);
        return valueType;
    }
    return visit(ctx->functionCall());
}

antlrcpp::Any AnalyzerVisitor::visitDataType(SpiceParser::DataTypeContext* ctx) {
    if (ctx->IDENTIFIER()) {
        std::string structName = ctx->IDENTIFIER()->toString();
        SymbolTableEntry* structSymbol = currentScope->lookup(structName);
        if (structSymbol == nullptr)
            throw SemanticError(*ctx->start, UNKNOWN_DATATYPE, "Unknown datatype '" + structName + "'");
    }
    return SymbolType(TYPE_BOOL);
}

SymbolType AnalyzerVisitor::getSymbolTypeFromDataType(SpiceParser::DataTypeContext* ctx) {
    if (ctx->TYPE_DOUBLE()) return ctx->MUL() ? SymbolType(TYPE_DOUBLE_PTR) : SymbolType(TYPE_DOUBLE);
    if (ctx->TYPE_INT()) return ctx->MUL() ? SymbolType(TYPE_INT_PTR) : SymbolType(TYPE_INT);
    if (ctx->TYPE_STRING()) return ctx->MUL() ? SymbolType(TYPE_STRING_PTR) : SymbolType(TYPE_STRING);
    if (ctx->TYPE_BOOL()) return ctx->MUL() ? SymbolType(TYPE_BOOL_PTR) : SymbolType(TYPE_BOOL);
    if (ctx->IDENTIFIER()) {
        std::string structName = ctx->IDENTIFIER()->toString();
        return ctx->MUL() ? SymbolType(TYPE_STRUCT_PTR, structName) :
            SymbolType(TYPE_STRUCT, structName);
    }
    return SymbolType(TYPE_DYN);
}

SymbolType AnalyzerVisitor::getReferenceTypeFromType(SymbolType type) {
    if (type.getSuperType() == TYPE_DOUBLE) return SymbolType(TYPE_DOUBLE_PTR);
    if (type.getSuperType() == TYPE_INT) return SymbolType(TYPE_INT_PTR);
    if (type.getSuperType() == TYPE_STRING) return SymbolType(TYPE_STRING_PTR);
    if (type.getSuperType() == TYPE_BOOL) return SymbolType(TYPE_BOOL_PTR);
    if (type.getSuperType() == TYPE_STRUCT) return SymbolType(TYPE_STRUCT_PTR, type.getSubType());
    return SymbolType(TYPE_DYN);
}

SymbolType AnalyzerVisitor::getTypeFromReferenceType(SymbolType type) {
    if (type.getSuperType() == TYPE_DOUBLE_PTR) return SymbolType(TYPE_DOUBLE);
    if (type.getSuperType() == TYPE_INT_PTR) return SymbolType(TYPE_INT);
    if (type.getSuperType() == TYPE_STRING_PTR) return SymbolType(TYPE_STRING);
    if (type.getSuperType() == TYPE_BOOL_PTR) return SymbolType(TYPE_BOOL);
    if (type.getSuperType() == TYPE_STRUCT_PTR) return SymbolType(TYPE_STRUCT, type.getSubType());
    return SymbolType(TYPE_DYN);
}