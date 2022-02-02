// Copyright (c) 2021-2022 ChilliBits. All rights reserved.

#pragma once

#include <utility>

#include "SpiceBaseVisitor.h"
#include "SpiceLexer.h"

#include <CompilerInstance.h>
#include <analyzer/OpRuleManager.h>
#include <symbol/SymbolTable.h>
#include <symbol/ScopePath.h>
#include <symbol/SymbolType.h>
#include <symbol/SymbolSpecifiers.h>
#include <util/ModuleRegistry.h>
#include <util/ScopeIdUtil.h>
#include <util/FileUtil.h>
#include <exception/SemanticError.h>

#include <llvm/ADT/Triple.h>
#include <llvm/Support/Host.h>


const static std::string MAIN_FUNCTION_NAME = "main";
const static std::string RETURN_VARIABLE_NAME = "result";
const static std::string THIS_VARIABLE_NAME = "this";
const static std::string FOREACH_DEFAULT_IDX_VARIABLE_NAME = "idx";
const static std::string UNUSED_VARIABLE_NAME = "_";
const std::vector<std::string> RESERVED_KEYWORDS = { "new", "switch", "case" };

class AnalyzerVisitor : public SpiceBaseVisitor {
public:
    // Constructors
    explicit AnalyzerVisitor(std::string sourceFile, std::string targetArch, std::string targetVendor, std::string targetOs,
                             std::string outputPath, bool debugOutput, int optLevel, bool mustHaveMainFunction, bool stdFile) :
            mainSourceFile(std::move(sourceFile)),
            targetArch(std::move(targetArch)),
            targetVendor(std::move(targetVendor)),
            targetOs(std::move(targetOs)),
            objectDir(std::move(outputPath)),
            debugOutput(debugOutput),
            optLevel(optLevel),
            requiresMainFunction(mustHaveMainFunction),
            stdFile(stdFile) {
        if (targetArch.empty()) {
            llvm::Triple targetTriple = llvm::Triple(llvm::sys::getDefaultTargetTriple());
            targetArch = targetTriple.getArchName();
            targetVendor = targetTriple.getVendorName();
            targetOs = targetTriple.getOSName();
        }
    }

    // Public methods
    antlrcpp::Any visitEntry(SpiceParser::EntryContext* ctx) override;
    antlrcpp::Any visitMainFunctionDef(SpiceParser::MainFunctionDefContext* ctx) override;
    antlrcpp::Any visitFunctionDef(SpiceParser::FunctionDefContext* ctx) override;
    antlrcpp::Any visitProcedureDef(SpiceParser::ProcedureDefContext* ctx) override;
    antlrcpp::Any visitExtDecl(SpiceParser::ExtDeclContext* ctx) override;
    antlrcpp::Any visitStructDef(SpiceParser::StructDefContext* ctx) override;
    antlrcpp::Any visitGlobalVarDef(SpiceParser::GlobalVarDefContext* ctx) override;
    antlrcpp::Any visitForLoop(SpiceParser::ForLoopContext* ctx) override;
    antlrcpp::Any visitForeachLoop(SpiceParser::ForeachLoopContext* ctx) override;
    antlrcpp::Any visitWhileLoop(SpiceParser::WhileLoopContext* ctx) override;
    antlrcpp::Any visitIfStmt(SpiceParser::IfStmtContext* ctx) override;
    antlrcpp::Any visitElseStmt(SpiceParser::ElseStmtContext* ctx) override;
    antlrcpp::Any visitParamLstDef(SpiceParser::ParamLstDefContext* ctx) override;
    antlrcpp::Any visitDeclStmt(SpiceParser::DeclStmtContext* ctx) override;
    antlrcpp::Any visitImportStmt(SpiceParser::ImportStmtContext* ctx) override;
    antlrcpp::Any visitReturnStmt(SpiceParser::ReturnStmtContext* ctx) override;
    antlrcpp::Any visitBreakStmt(SpiceParser::BreakStmtContext* ctx) override;
    antlrcpp::Any visitContinueStmt(SpiceParser::ContinueStmtContext* ctx) override;
    antlrcpp::Any visitBuiltinCall(SpiceParser::BuiltinCallContext* ctx) override;
    antlrcpp::Any visitPrintfCall(SpiceParser::PrintfCallContext* ctx) override;
    antlrcpp::Any visitSizeOfCall(SpiceParser::SizeOfCallContext* ctx) override;
    antlrcpp::Any visitAssignExpr(SpiceParser::AssignExprContext* ctx) override;
    antlrcpp::Any visitTernaryExpr(SpiceParser::TernaryExprContext* ctx) override;
    antlrcpp::Any visitLogicalOrExpr(SpiceParser::LogicalOrExprContext* ctx) override;
    antlrcpp::Any visitLogicalAndExpr(SpiceParser::LogicalAndExprContext* ctx) override;
    antlrcpp::Any visitBitwiseOrExpr(SpiceParser::BitwiseOrExprContext* ctx) override;
    antlrcpp::Any visitBitwiseXorExpr(SpiceParser::BitwiseXorExprContext* ctx) override;
    antlrcpp::Any visitBitwiseAndExpr(SpiceParser::BitwiseAndExprContext* ctx) override;
    antlrcpp::Any visitEqualityExpr(SpiceParser::EqualityExprContext* ctx) override;
    antlrcpp::Any visitRelationalExpr(SpiceParser::RelationalExprContext* ctx) override;
    antlrcpp::Any visitShiftExpr(SpiceParser::ShiftExprContext* ctx) override;
    antlrcpp::Any visitAdditiveExpr(SpiceParser::AdditiveExprContext* ctx) override;
    antlrcpp::Any visitMultiplicativeExpr(SpiceParser::MultiplicativeExprContext* ctx) override;
    antlrcpp::Any visitCastExpr(SpiceParser::CastExprContext* ctx) override;
    antlrcpp::Any visitPrefixUnaryExpr(SpiceParser::PrefixUnaryExprContext* ctx) override;
    antlrcpp::Any visitPostfixUnaryExpr(SpiceParser::PostfixUnaryExprContext* ctx) override;
    antlrcpp::Any visitAtomicExpr(SpiceParser::AtomicExprContext* ctx) override;
    antlrcpp::Any visitValue(SpiceParser::ValueContext* ctx) override;
    antlrcpp::Any visitPrimitiveValue(SpiceParser::PrimitiveValueContext* ctx) override;
    antlrcpp::Any visitDataType(SpiceParser::DataTypeContext* ctx) override;
    antlrcpp::Any visitBaseDataType(SpiceParser::BaseDataTypeContext* ctx) override;

private:
    // Members
    std::string mainSourceFile;
    std::string targetArch;
    std::string targetVendor;
    std::string targetOs;
    std::string objectDir;
    bool debugOutput;
    int optLevel;
    bool requiresMainFunction = true;
    bool hasMainFunction = false;
    bool stdFile = false;
    SymbolTable* currentScope = new SymbolTable(nullptr, requiresMainFunction);
    ScopePath scopePath;
    std::string scopePrefix;
    bool parameterMode = false;
    int nestedLoopCounter = 0;
    std::string currentVariableName;

    // Private methods
    SymbolType initExtStruct(const antlr4::Token&, SymbolTable*, const std::string&, const std::string&);
};