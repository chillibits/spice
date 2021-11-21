// Copyright (c) 2021 ChilliBits. All rights reserved.

#include "ScopeIdUtil.h"

/**
 * Get scope id for the MainFunctionDef AST node
 *
 * @return Scope id
 */
std::string ScopeIdUtil::getScopeId(SpiceParser::MainFunctionDefContext*) {
    return "main()";
}

/**
 * Get scope id for the FunctionDef AST node
 *
 * @return Scope id
 */
std::string ScopeIdUtil::getScopeId(SpiceParser::FunctionDefContext* ctx) {
    auto symbol = ctx->F()->getSymbol();
    return "f:" + ctx->IDENTIFIER()->toString() + ":" + std::to_string(symbol->getLine()) + ":" +
           std::to_string(symbol->getCharPositionInLine());
}

/**
 * Get scope id for the ProcedureDef AST node
 *
 * @return Scope id
 */
std::string ScopeIdUtil::getScopeId(SpiceParser::ProcedureDefContext* ctx) {
    auto symbol = ctx->P()->getSymbol();
    return "p:" + ctx->IDENTIFIER()->toString() + ":" + std::to_string(symbol->getLine()) + ":" +
           std::to_string(symbol->getCharPositionInLine());
}

/**
 * Get scope id for the StructDef AST node
 *
 * @return Scope id
 */
std::string ScopeIdUtil::getScopeId(SpiceParser::StructDefContext* ctx) {
    auto symbol = ctx->TYPE()->getSymbol();
    return "struct:" + ctx->IDENTIFIER()->toString() + ":" + std::to_string(symbol->getLine()) + ":" +
           std::to_string(symbol->getCharPositionInLine());
}

/**
 * Get scope id for the ForLoop AST node
 *
 * @return Scope id
 */
std::string ScopeIdUtil::getScopeId(SpiceParser::ForLoopContext* ctx) {
    auto symbol = ctx->FOR()->getSymbol();
    return "for:" + std::to_string(symbol->getLine()) + ":" + std::to_string(symbol->getCharPositionInLine());
}

/**
 * Get scope id for the WhileLoop AST node
 *
 * @return Scope id
 */
std::string ScopeIdUtil::getScopeId(SpiceParser::WhileLoopContext* ctx) {
    auto symbol = ctx->WHILE()->getSymbol();
    return "while:" + std::to_string(symbol->getLine()) + ":" + std::to_string(symbol->getCharPositionInLine());
}

/**
 * Get scope id for the IfStmt AST node
 *
 * @return Scope id
 */
std::string ScopeIdUtil::getScopeId(SpiceParser::IfStmtContext* ctx) {
    auto symbol = ctx->IF()->getSymbol();
    return "if:" + std::to_string(symbol->getLine()) + ":" + std::to_string(symbol->getCharPositionInLine());
}

/**
 * Get scope id for the ElseStmt AST node
 *
 * @return Scope id
 */
std::string ScopeIdUtil::getScopeId(SpiceParser::ElseStmtContext* ctx) {
    auto symbol = ctx->ELSE()->getSymbol();
    return "else:" + std::to_string(symbol->getLine()) + ":" + std::to_string(symbol->getCharPositionInLine());
}
