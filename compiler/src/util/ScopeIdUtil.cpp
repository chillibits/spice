// Copyright (c) 2021 ChilliBits. All rights reserved.

#include "ScopeIdUtil.h"

std::string ScopeIdUtil::getScopeId(SpiceParser::MainFunctionDefContext* ctx) {
    return "f:main";
}

std::string ScopeIdUtil::getScopeId(SpiceParser::FunctionDefContext* ctx) {
    return "f:" + ctx->IDENTIFIER()->toString();
}

std::string ScopeIdUtil::getScopeId(SpiceParser::ProcedureDefContext* ctx) {
    return "p:" + ctx->IDENTIFIER()->toString();
}

std::string ScopeIdUtil::getScopeId(SpiceParser::ForLoopContext* ctx) {
    return "for:" + std::to_string(ctx->FOR()->getSymbol()->getLine()) + ":" +
        std::to_string(ctx->FOR()->getSymbol()->getCharPositionInLine());
}

std::string ScopeIdUtil::getScopeId(SpiceParser::WhileLoopContext* ctx) {
    return "while:" + std::to_string(ctx->WHILE()->getSymbol()->getLine()) + ":" +
        std::to_string(ctx->WHILE()->getSymbol()->getCharPositionInLine());
}

std::string ScopeIdUtil::getScopeId(SpiceParser::IfStmtContext* ctx) {
    return "if:" + std::to_string(ctx->IF()->getSymbol()->getLine()) + ":" +
        std::to_string(ctx->IF()->getSymbol()->getCharPositionInLine());
}