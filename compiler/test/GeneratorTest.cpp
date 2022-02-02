// Copyright (c) 2021-2022 ChilliBits. All rights reserved.

#include <gtest/gtest.h>
#include <iostream>
#include "antlr4-runtime.h"

#include "SpiceLexer.h"
#include "SpiceParser.h"
#include "TestUtil.h"
#include <analyzer/AnalyzerVisitor.h>

const unsigned int IR_FILE_SKIP_LINES = 4;

struct GeneratorTestCase {
    const std::string testName;
    const std::string testPath;
};

typedef std::vector<GeneratorTestCase> GeneratorTestSuite;

std::vector<GeneratorTestCase> detectGeneratorTestCases(const std::string& suitePath) {
    std::vector<std::string> subDirs = TestUtil::getSubdirs(suitePath);

    std::vector<GeneratorTestCase> testCases;
    testCases.reserve(subDirs.size());
    for (std::string& dirName : subDirs) {
        // Save test suite
        testCases.push_back({dirName, suitePath + "/" + dirName});
    }

    return testCases;
}

std::vector<GeneratorTestSuite> detectGeneratorTestSuites(const std::string& testFilesPath) {
    std::vector<std::string> subDirs = TestUtil::getSubdirs(testFilesPath);

    std::vector<GeneratorTestSuite> testSuites;
    testSuites.reserve(subDirs.size());
    for (std::string& dirName : subDirs)
        testSuites.push_back(detectGeneratorTestCases(testFilesPath + "/" + dirName));

    return testSuites;
}

void executeTest(const GeneratorTestCase& testCase) {
    // Check if disabled
    std::string disabledFile = testCase.testPath + "/disabled";
    if (TestUtil::fileExists(disabledFile)) GTEST_SKIP();

    // Read source file
    std::string sourceFile = testCase.testPath + "/source.spice";
    std::ifstream sourceStream;
    sourceStream.open(sourceFile);
    if (!sourceStream) throw std::runtime_error("Test file '" + sourceFile + "' does not exist");
    antlr4::ANTLRInputStream input(sourceStream);

    // Create error handlers for lexer and parser
    AntlrThrowingErrorListener lexerErrorHandler = AntlrThrowingErrorListener(LEXER);
    AntlrThrowingErrorListener parserErrorHandler = AntlrThrowingErrorListener(PARSER);

    try {
        // Tokenize input
        SpiceLexer lexer(&input);
        lexer.removeErrorListeners();
        lexer.addErrorListener(&lexerErrorHandler);
        antlr4::CommonTokenStream tokens((antlr4::TokenSource*) &lexer);

        // Parse input to AST
        SpiceParser parser(&tokens);
        parser.removeErrorListeners();
        parser.addErrorListener(&parserErrorHandler);
        antlr4::tree::ParseTree* tree = parser.entry();

        // Execute semantic analysis
        AnalyzerVisitor analyzer = AnalyzerVisitor(
                sourceFile,
                "",
                "",
                "",
                ".",
                false,
                0,
                true,
                false
        );
        SymbolTable* symbolTable = analyzer.visit(tree).as<SymbolTable*>();

        // Fail if an error was expected
        if (TestUtil::fileExists(testCase.testPath + "/exception.out"))
            FAIL() << "Expected error, but got no error";

        // Check if the symbol table matches the expected output
        std::string symbolTableFileName = testCase.testPath + "/symbol-table.txt";
        if (TestUtil::fileExists(symbolTableFileName)) {
            std::string expectedSymbolTable = TestUtil::getFileContent(symbolTableFileName);
            EXPECT_EQ(expectedSymbolTable, symbolTable->toString());
        }

        // Determine the expected opt level
        std::string irCodeO1FileName = testCase.testPath + "/ir-code-O1.ll";
        std::string irCodeO2FileName = testCase.testPath + "/ir-code-O2.ll";
        std::string irCodeO3FileName = testCase.testPath + "/ir-code-O3.ll";
        std::string expectedOptIR;
        int expectedOptLevel = 0;
        if (TestUtil::fileExists(irCodeO1FileName)) {
            expectedOptLevel = 1;
            expectedOptIR = TestUtil::getFileContent(irCodeO1FileName);
        } else if (TestUtil::fileExists(irCodeO2FileName)) {
            expectedOptLevel = 2;
            expectedOptIR = TestUtil::getFileContent(irCodeO2FileName);
        } else if (TestUtil::fileExists(irCodeO3FileName)) {
            expectedOptLevel = 3;
            expectedOptIR = TestUtil::getFileContent(irCodeO3FileName);
        }

        // Execute generator
        GeneratorVisitor generator = GeneratorVisitor(
                symbolTable,
                sourceFile,
                "",
                "",
                "",
                "./source.spice.o",
                false,
                expectedOptLevel,
                true
        );
        generator.init(); // Initialize code generation
        generator.visit(tree); // Generate IR code

        // Check if the ir code matches the expected output
        std::string irCodeFileName = testCase.testPath + "/ir-code.ll";
        if (TestUtil::fileExists(irCodeFileName)) {
            std::string expectedIR = TestUtil::getFileContent(irCodeFileName);
            std::string actualIR = generator.getIRString();
            // Cut of first n lines to have a target independent
            for (int i = 0; i < IR_FILE_SKIP_LINES; i++) {
                expectedIR.erase(0, expectedIR.find('\n') + 1);
                actualIR.erase(0, actualIR.find('\n') + 1);
            }
            EXPECT_EQ(expectedIR, actualIR);
        }

        // Check if the optimized ir code matches the expected output
        if (expectedOptLevel > 0) {
            generator.optimize();
            std::string actualOptimizedIR = generator.getIRString();
            // Cut of first n lines to have a target independent
            for (int i = 0; i < IR_FILE_SKIP_LINES; i++) {
                expectedOptIR.erase(0, expectedOptIR.find('\n') + 1);
                actualOptimizedIR.erase(0, actualOptimizedIR.find('\n') + 1);
            }
            EXPECT_EQ(expectedOptIR, actualOptimizedIR);
        }

        // Check if the execution output matches the expected output
        std::string outputFileName = testCase.testPath + "/cout.out";
        if (TestUtil::fileExists(outputFileName)) {
            std::string expectedOutput = TestUtil::getFileContent(outputFileName);

            // Emit the object file
            generator.emit(); // Emit object file for specified platform

            // Link
            TestUtil::exec("gcc -no-pie -o source source.spice.o");

            // Execute the program and get the output
            std::string executionOutput = TestUtil::exec(TestUtil::getDefaultExecutableName());

            // Check if the outputs are matching
            EXPECT_EQ(expectedOutput, executionOutput);
        }

        SUCCEED();
    } catch (LexerParserError& error) {
        FAIL() << "Hit lexer/parser error: " << error.what();
    } catch (SemanticError& error) {
        // Check if the exception message matches the expected output
        std::string exceptionFile = testCase.testPath + "/exception.out";
        if (TestUtil::fileExists(exceptionFile)) {
            std::string expectedException = TestUtil::getFileContent(exceptionFile);
            EXPECT_EQ(std::string(error.what()), expectedException);
        } else {
            FAIL() << "Expected no error, but got '" << error.what() << "'";
        }
    }
}

// Test classes

class GeneratorArbitraryTests : public ::testing::TestWithParam<GeneratorTestCase> {};
class GeneratorArrayTests : public ::testing::TestWithParam<GeneratorTestCase> {};
class GeneratorBuiltinTests : public ::testing::TestWithParam<GeneratorTestCase> {};
class GeneratorCliArgsTests : public ::testing::TestWithParam<GeneratorTestCase> {};
class GeneratorForLoopTests : public ::testing::TestWithParam<GeneratorTestCase> {};
class GeneratorForEachLoopTests : public ::testing::TestWithParam<GeneratorTestCase> {};
class GeneratorFunctionTests : public ::testing::TestWithParam<GeneratorTestCase> {};
class GeneratorIfStatementTests : public ::testing::TestWithParam<GeneratorTestCase> {};
class GeneratorImportTests : public ::testing::TestWithParam<GeneratorTestCase> {};
class GeneratorMethodTests : public ::testing::TestWithParam<GeneratorTestCase> {};
class GeneratorOperatorTests : public ::testing::TestWithParam<GeneratorTestCase> {};
class GeneratorPointerTests : public ::testing::TestWithParam<GeneratorTestCase> {};
class GeneratorProcedureTests : public ::testing::TestWithParam<GeneratorTestCase> {};
class GeneratorStructTests : public ::testing::TestWithParam<GeneratorTestCase> {};
class GeneratorVariableTests : public ::testing::TestWithParam<GeneratorTestCase> {};
class GeneratorWhileLoopTests : public ::testing::TestWithParam<GeneratorTestCase> {};

// Test macros

TEST_P(GeneratorArbitraryTests, ArbitraryTests) {
    executeTest(GetParam());
}

TEST_P(GeneratorArrayTests, ArrayTests) {
    executeTest(GetParam());
}

TEST_P(GeneratorBuiltinTests, BuiltinTests) {
    executeTest(GetParam());
}

TEST_P(GeneratorCliArgsTests, CliArgTests) {
    executeTest(GetParam());
}

TEST_P(GeneratorForLoopTests, ForLoopTests) {
    executeTest(GetParam());
}

TEST_P(GeneratorForEachLoopTests, ForEachLoopTests) {
    executeTest(GetParam());
}

TEST_P(GeneratorFunctionTests, FunctionTests) {
    executeTest(GetParam());
}

TEST_P(GeneratorIfStatementTests, IfStatementTests) {
    executeTest(GetParam());
}

TEST_P(GeneratorImportTests, ImportTests) {
    executeTest(GetParam());
}

TEST_P(GeneratorMethodTests, MethodTests) {
    executeTest(GetParam());
}

TEST_P(GeneratorOperatorTests, OperatorTests) {
    executeTest(GetParam());
}

TEST_P(GeneratorPointerTests, PointerTests) {
    executeTest(GetParam());
}

TEST_P(GeneratorProcedureTests, ProcedureTests) {
    executeTest(GetParam());
}

TEST_P(GeneratorStructTests, StructTests) {
    executeTest(GetParam());
}

TEST_P(GeneratorVariableTests, VariableTests) {
    executeTest(GetParam());
}

TEST_P(GeneratorWhileLoopTests, WhileLoopTests) {
    executeTest(GetParam());
}

// Name resolver

struct NameResolver {
    template <class AnalyzerTestCase>
    std::string operator()(const ::testing::TestParamInfo<AnalyzerTestCase>& info) const {
        auto testCase = static_cast<AnalyzerTestCase>(info.param);
        return TestUtil::toCamelCase(testCase.testName);
    }
};

// Instantiations

const std::vector<GeneratorTestSuite> generatorSuites = detectGeneratorTestSuites("./test-files/generator");

INSTANTIATE_TEST_SUITE_P(
        GeneratorArbitraryTests,
        GeneratorArbitraryTests,
        ::testing::ValuesIn(generatorSuites[0]),
        NameResolver());

INSTANTIATE_TEST_SUITE_P(
        GeneratorArrayTests,
        GeneratorArrayTests,
        ::testing::ValuesIn(generatorSuites[1]),
        NameResolver());

INSTANTIATE_TEST_SUITE_P(
        GeneratorBuiltinTests,
        GeneratorBuiltinTests,
        ::testing::ValuesIn(generatorSuites[2]),
        NameResolver());

INSTANTIATE_TEST_SUITE_P(
        GeneratorCliArgsTests,
        GeneratorCliArgsTests,
        ::testing::ValuesIn(generatorSuites[3]),
        NameResolver());

INSTANTIATE_TEST_SUITE_P(
        GeneratorForLoopTests,
        GeneratorForLoopTests,
        ::testing::ValuesIn(generatorSuites[4]),
        NameResolver());

INSTANTIATE_TEST_SUITE_P(
        AnalyzerForEachLoopTests,
        GeneratorForEachLoopTests,
        ::testing::ValuesIn(generatorSuites[5]),
        NameResolver());

INSTANTIATE_TEST_SUITE_P(
        GeneratorFunctionTests,
        GeneratorFunctionTests,
        ::testing::ValuesIn(generatorSuites[6]),
        NameResolver());

INSTANTIATE_TEST_SUITE_P(
        GeneratorIfStatementTests,
        GeneratorIfStatementTests,
        ::testing::ValuesIn(generatorSuites[7]),
        NameResolver());

INSTANTIATE_TEST_SUITE_P(
        GeneratorImportTests,
        GeneratorImportTests,
        ::testing::ValuesIn(generatorSuites[8]),
        NameResolver());

INSTANTIATE_TEST_SUITE_P(
        GeneratorMethodTests,
        GeneratorMethodTests,
        ::testing::ValuesIn(generatorSuites[9]),
        NameResolver());

INSTANTIATE_TEST_SUITE_P(
        GeneratorOperatorTests,
        GeneratorOperatorTests,
        ::testing::ValuesIn(generatorSuites[10]),
        NameResolver());

INSTANTIATE_TEST_SUITE_P(
        GeneratorPointerTests,
        GeneratorPointerTests,
        ::testing::ValuesIn(generatorSuites[11]),
        NameResolver());

INSTANTIATE_TEST_SUITE_P(
        GeneratorProcedureTests,
        GeneratorProcedureTests,
        ::testing::ValuesIn(generatorSuites[12]),
        NameResolver());

INSTANTIATE_TEST_SUITE_P(
        GeneratorStructTests,
        GeneratorStructTests,
        ::testing::ValuesIn(generatorSuites[13]),
        NameResolver());