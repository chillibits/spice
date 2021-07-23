// Copyright (c) 2021 ChilliBits. All rights reserved.

#include <gtest/gtest.h>
#include <iostream>
#include "antlr4-runtime.h"

#include "SpiceLexer.h"
#include "SpiceParser.h"
#include <analyzer/AnalyzerVisitor.h>

struct AnalyzerParams {
    const std::string testCaseName;
    const std::string errorMessage; // Empty error message for testing for no error
};

const AnalyzerParams ANALYZER_TEST_PARAMETERS[] = {
        {
            "error-assignment-same-type",
            "Wrong data type for operator: Cannot apply the assign operator to different data types"
        },
        {
            "success-fibonacci",
            ""
        }
};

class AnalyzerTests : public ::testing::TestWithParam<AnalyzerParams> {};

TEST_P(AnalyzerTests, TestAnalyzerWithValidAndInvalidTestFiles) {
    AnalyzerParams param = GetParam();

    // Read source file
    std::ifstream sourceStream;
    sourceStream.open("./test-files/" + param.testCaseName + "/source.spice");
    antlr4::ANTLRInputStream input(sourceStream);

    // Parse input to AST
    SpiceLexer lexer(&input);
    antlr4::CommonTokenStream tokens((antlr4::TokenSource*) &lexer);
    SpiceParser parser(&tokens);

    // Execute syntactical analysis
    antlr4::tree::ParseTree *tree = parser.entry();
    try {
        // Execute semantic analysis
        SymbolTable* symbolTable = AnalyzerVisitor().visit(tree).as<SymbolTable*>();

        // Fail if an error was expected
        if (param.errorMessage.length() > 0)
            FAIL() << "Expected error message '" + param.errorMessage + "', but got no error";

        // Check if the symbol table matches the expected output
        std::ifstream symbolTableStream;
        symbolTableStream.open("./test-files/" + param.testCaseName + "/symbol-table.txt");
        std::ostringstream stringStream;
        stringStream << symbolTableStream.rdbuf();
        std::string expectedSymbolTable = stringStream.str();
        EXPECT_EQ(expectedSymbolTable, symbolTable->toString());

        SUCCEED();
    } catch (SemanticError &error) {
        if (param.errorMessage.length() == 0)
            FAIL() << "Expected no error, but got '" + std::string(error.what()) + "'";
        EXPECT_EQ(std::string(error.what()), param.errorMessage);
    }
}

INSTANTIATE_TEST_SUITE_P(
        AnalyzerTests,
        AnalyzerTests,
        ::testing::ValuesIn(ANALYZER_TEST_PARAMETERS)
);