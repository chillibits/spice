// Copyright (c) 2021-2023 ChilliBits. All rights reserved.

// GCOV_EXCL_START

#pragma once

#include <functional>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "util/FileUtil.h"

const char *const PATH_TEST_FILES = "./test-files/";
const unsigned int EXPECTED_NUMBER_OF_TESTS = 250;
const unsigned int IR_FILE_SKIP_LINES = 5; // Skip the first couple of lines, because they contain target dependent information
const char *const GDB_READING_SYMBOLS_MESSAGE = "Reading symbols from ";
const char *const GDB_INFERIOR_MESSAGE = "[Inferior";
extern bool updateRefs;
extern bool runBenchmarks;
extern bool enableLeakDetection;
extern bool skipNonGitHubTests;

const char *const INPUT_NAME_LINKER_FLAGS = "linker-flags.txt";
const char *const INPUT_NAME_CLI_FLAGS = "cli-flags.txt";

const char *const REF_NAME_SOURCE = "source.spice";
const char *const REF_NAME_PARSE_TREE = "parse-tree.dot";
const char *const REF_NAME_SYNTAX_TREE = "syntax-tree.dot";
const char *const REF_NAME_SYMBOL_TABLE = "symbol-table.json";
const char *const REF_NAME_IR = "ir-code.ll";
const char *const REF_NAME_ASM = "assembly.asm";
const char *const REF_NAME_OPT_IR[5] = {"ir-code-O1.ll", "ir-code-O2.ll", "ir-code-O3.ll", "ir-code-Os.ll", "ir-code-Oz.ll"};
const char *const REF_NAME_EXECUTION_OUTPUT = "cout.out";
const char *const REF_NAME_GDB_OUTPUT = "debug.out";
const char *const REF_NAME_ERROR_OUTPUT = "exception.out";
const char *const REF_NAME_WARNING_OUTPUT = "warning.out";
const char *const REF_NAME_EXIT_CODE = "exit-code.out";

const char *const CTL_SKIP_DISABLED = "disabled";
const char *const CTL_SKIP_GH = "skip-gh-actions";
const char *const CTL_DEBUG_INFO = "with-debug-info";
const char *const CTL_DEBUG_SCRIPT = "debug.gdb";
const char *const CTL_LTO = "with-lto";

struct TestCase {
  const std::string testSuite;
  const std::string testName;
  const std::filesystem::path testPath;
};

// Typedefs
typedef const std::function<std::string(void)> &GetOutputFct;
typedef const std::function<void(std::string &expectedOutput, std::string &actualOutput)> &ModifyOutputFct;

class TestUtil {
public:
  // Public structs
  struct NameResolver {
    template <class T> std::string operator()(const testing::TestParamInfo<T> &info) const {
      T testCase = static_cast<T>(info.param);
      return TestUtil::toCamelCase(testCase.testSuite) + "_" + TestUtil::toCamelCase(testCase.testName);
    }
  };

  // Public static methods
  static std::vector<TestCase> collectTestCases(const std::string &suiteName, bool useSubDirs);
  static bool checkRefMatch(
      const std::filesystem::path &refPath, GetOutputFct getActualOutput,
      ModifyOutputFct modifyOutput = [](std::string &, std::string &) {});
  static void handleError(const TestCase &testCase, const std::exception &error);
  static std::vector<std::string> getSubdirs(const std::filesystem::path &basePath);
  static std::vector<std::string> getFileContentLinesVector(const std::filesystem::path &filePath);
  static std::string toCamelCase(std::string input);
  static constexpr const char *getDefaultExecutableName() {
#if OS_WINDOWS
    return ".\\source.exe";
#else
    return "./source";
#endif
  }
  static bool isDisabled(const TestCase &testCase, bool isGHActions);
  static void eraseIRModuleHeader(std::string &irCode);
  static void eraseGDBHeader(std::string &gdbOutput);
  static void eraseLinesBySubstring(std::string &irCode, const char *needle);
};

// GCOV_EXCL_STOP