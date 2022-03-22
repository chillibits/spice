// Copyright (c) 2021-2022 ChilliBits. All rights reserved.

#include "CliInterface.h"
#include "util/CompilerWarning.h"

#include <util/FileUtil.h>

void CliInterface::createInterface() {
  // Allow positional args
  app.allow_windows_style_options();
  app.allow_extras();
  app.positionals_at_end();
  app.footer("(c) Marc Auberer 2021-2022");

  // Add version flag
  std::string versionName = std::string(SPICE_VERSION);
  std::string builtBy = std::string(SPICE_BUILT_BY);
  std::string versionString = "Spice version " + versionName + "\nbuilt by: " + builtBy + "\n\n(c) Marc Auberer 2021-2022";
  app.set_version_flag("--version,-v", versionString);

  // Create sub-commands
  addBuildSubcommand();
  addRunSubcommand();
  addInstallSubcommand();
  addUninstallSubcommand();

  app.final_callback([&]() {
    if (!compile)
      return;

    // If the binary should be installed, set the output path to the Spice bin directory
    if (install) {
      std::string installPath = FileUtil::getSpiceBinDir();
      FileUtil::createDirs(installPath);
      installPath += FileUtil::getFileName(cliOptions.mainSourceFile.substr(0, cliOptions.mainSourceFile.length() - 6));
#ifdef OS_WINDOWS
      installPath += ".exe";
#endif
      cliOptions.outputPath = installPath;
    }

    // Ensure that both, the output path and the output dir have valid values
    if (cliOptions.outputPath.empty())
      cliOptions.outputPath = ".";
    if (cliOptions.outputPath == "." || cliOptions.outputPath == "..") {
      cliOptions.outputPath = FileUtil::getFileName(cliOptions.mainSourceFile.substr(0, cliOptions.mainSourceFile.length() - 6));
#ifdef OS_WINDOWS
      cliOptions.outputPath += ".exe";
#endif
    }

    // Add relative prefix to filename
    if (cliOptions.mainSourceFile.find("/\\") != std::string::npos)
      cliOptions.mainSourceFile = "./" + cliOptions.mainSourceFile;

    // Set outputDir to <system-tmp-dir>/spice-output
    char separator = FileUtil::getDirSeparator();
    uint64_t millis = duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    cliOptions.outputDir = std::filesystem::temp_directory_path().string();
    if (cliOptions.outputDir.back() != '/' && cliOptions.outputDir.back() != '\\')
      cliOptions.outputDir += separator;
    cliOptions.outputDir += "spice-output";
    cliOptions.outputDir += separator;
    cliOptions.outputDir += std::to_string(millis);

    // Set output path to output dir if running is enabled
    if (run) {
      cliOptions.outputPath = cliOptions.outputDir + FileUtil::getDirSeparator();
      cliOptions.outputPath += FileUtil::getFileName(cliOptions.mainSourceFile.substr(0, cliOptions.mainSourceFile.length() - 6));
    }

    // Create the output dir if it does not exist already
    if (!FileUtil::dirExists(cliOptions.outputDir))
      FileUtil::createDirs(cliOptions.outputDir);
  });
}

/**
 * Validates if all necessary cli options were provided.
 *
 * @throws InvalidCliOptionsException if there were an invalid combination of cli options provided
 */
void CliInterface::validate() const {
  // Check if all three of --target-arch, --target-vendor and --target-os are provided or none of them
  if (!((cliOptions.targetArch.empty() && cliOptions.targetVendor.empty() && cliOptions.targetOs.empty()) ||
        (!cliOptions.targetArch.empty() && !cliOptions.targetVendor.empty() && !cliOptions.targetOs.empty()))) {
    throw err.get(INCOMPLETE_TARGET_TRIPLE, "You need to provide all three of --target-arch, --target-vendor and --target-os");
  }
}

/**
 * Add build subcommand to cli interface
 */
void CliInterface::addBuildSubcommand() {
  // Create sub-command itself
  CLI::App *subCmd = app.add_subcommand("build", "Builds your Spice program and emits an executable");
  subCmd->alias("b");
  subCmd->ignore_case();
  subCmd->configurable();
  subCmd->callback([&]() {
    compile = true; // Requires the source file to be compiled
  });

  // --debug-output
  subCmd->add_flag<bool>("--debug-output,-d", cliOptions.printDebugOutput, "Enable debug output");

  // --target-triple
  std::function<void(const std::string &)> targetTripleCallback = [&](const std::string &triple) {
    cliOptions.targetTriple = triple;
    // Get arch, vendor and os from target triple string
    size_t firstIndex = triple.find('-', 0);
    size_t secondIndex = triple.find('-', firstIndex + 1);
    size_t thirdIndex = triple.find('-', secondIndex + 1);
    cliOptions.targetArch = triple.substr(0, firstIndex);
    cliOptions.targetVendor = triple.substr(firstIndex + 1, secondIndex - firstIndex);
    cliOptions.targetOs = triple.substr(secondIndex + 1, thirdIndex - secondIndex);
  };
  subCmd->add_option_function<std::string>("--target,--target-triple,--target-triplet,-t", targetTripleCallback,
                                           "Target triple for the emitted executable (for cross-compiling)");

  // --target-arch
  subCmd->add_option<std::string>("--target-arch", cliOptions.targetArch,
                                  "Target arch for emitted executable (for cross-compiling)");

  // --target-vendor
  subCmd->add_option<std::string>("--target-vendor", cliOptions.targetVendor,
                                  "Target vendor for emitted executable (for cross-compiling)");

  // --target-os
  subCmd->add_option<std::string>("--target-os", cliOptions.targetOs, "Target os for emitted executable (for cross-compiling)");

  // --output
  subCmd->add_option<std::string>("--output,-o", cliOptions.outputPath, "Set the output file path");

  // Opt levels
  subCmd->add_flag<short>("-O0{0}", cliOptions.optLevel, "Disable optimization for the output executable.");
  subCmd->add_flag<short>("-O1{1}", cliOptions.optLevel, "Optimization level 1. Only basic optimization is executed.");
  subCmd->add_flag<short>("-O2{2}", cliOptions.optLevel, "Optimization level 2. More advanced optimization is applied.");
  subCmd->add_flag<short>("-O3{3}", cliOptions.optLevel, "Optimization level 3. Aggressive optimization for best performance.");
  subCmd->add_flag<short>("-Os{4}", cliOptions.optLevel, "Optimization level s. Size optimization for output executable.");
  subCmd->add_flag<short>("-Oz{5}", cliOptions.optLevel, "Optimization level z. Aggressive optimization for best size.");

  // Source file
  subCmd->add_option<std::string>("<main-source-file>", cliOptions.mainSourceFile, "Main source file")
      ->check(CLI::ExistingFile)
      ->required();
}

/**
 * Add run subcommand to cli interface
 */
void CliInterface::addRunSubcommand() {
  // Create sub-command itself
  CLI::App *subCmd = app.add_subcommand("run", "Builds your Spice program and runs it immediately");
  subCmd->alias("r");
  subCmd->ignore_case();
  subCmd->callback([&]() {
    compile = run = true; // Requires the source file to be compiled
  });

  // --debug-output
  subCmd->add_flag<bool>("--debug-output,-d", cliOptions.printDebugOutput, "Enable debug output");

  // --output
  subCmd->add_option<std::string>("--output,-o", cliOptions.outputPath, "Set the output file path");

  // Opt levels
  subCmd->add_flag<short>("-O0{0}", cliOptions.optLevel, "Disable optimization for the output executable.");
  subCmd->add_flag<short>("-O1{1}", cliOptions.optLevel, "Optimization level 1. Only basic optimization is executed.");
  subCmd->add_flag<short>("-O2{2}", cliOptions.optLevel, "Optimization level 2. More advanced optimization is applied.");
  subCmd->add_flag<short>("-O3{3}", cliOptions.optLevel, "Optimization level 3. Aggressive optimization for best performance.");
  subCmd->add_flag<short>("-Os{4}", cliOptions.optLevel, "Optimization level s. Size optimization for output executable.");
  subCmd->add_flag<short>("-Oz{5}", cliOptions.optLevel, "Optimization level z. Aggressive optimization for best size.");

  // Source file
  subCmd->add_option<std::string>("<main-source-file>", cliOptions.mainSourceFile, "Main source file")
      ->check(CLI::ExistingFile)
      ->required();
}

/**
 * Add install subcommand to cli interface
 */
void CliInterface::addInstallSubcommand() {
  // Create sub-command itself
  CLI::App *subCmd =
      app.add_subcommand("install", "Builds your Spice program and installs it to a directory in the PATH variable");
  subCmd->alias("i");
  subCmd->ignore_case();
  subCmd->callback([&]() {
    compile = install = true; // Requires the source file to be compiled
  });

  // --debug-output
  subCmd->add_flag<bool>("--debug-output,-d", cliOptions.printDebugOutput, "Enable debug output");

  // --output
  subCmd->add_option<std::string>("--output,-o", cliOptions.outputPath, "Set the output file path");

  // Opt levels
  subCmd->add_flag<short>("-O0{0}", cliOptions.optLevel, "Disable optimization for the output executable.");
  subCmd->add_flag<short>("-O1{1}", cliOptions.optLevel, "Optimization level 1. Only basic optimization is executed.");
  subCmd->add_flag<short>("-O2{2}", cliOptions.optLevel, "Optimization level 2. More advanced optimization is applied.");
  subCmd->add_flag<short>("-O3{3}", cliOptions.optLevel, "Optimization level 3. Aggressive optimization for best performance.");
  subCmd->add_flag<short>("-Os{4}", cliOptions.optLevel, "Optimization level s. Size optimization for output executable.");
  subCmd->add_flag<short>("-Oz{5}", cliOptions.optLevel, "Optimization level z. Aggressive optimization for best size.");

  // Source file
  subCmd->add_option<std::string>("<main-source-file>", cliOptions.mainSourceFile, "Main source file")
      ->check(CLI::ExistingFile)
      ->required();
}

/**
 * Add uninstall subcommand to cli interface
 */
void CliInterface::addUninstallSubcommand() {
  // Create sub-command itself
  CLI::App *subCmd = app.add_subcommand("uninstall", "Builds your Spice program and runs it immediately");
  subCmd->alias("u");
  subCmd->ignore_case();
  subCmd->callback([&]() {
    std::string installPath = FileUtil::getSpiceBinDir();
    installPath += FileUtil::getFileName(cliOptions.mainSourceFile.substr(0, cliOptions.mainSourceFile.length() - 6));
#ifdef OS_WINDOWS
    installPath += ".exe";
#endif
    if (!FileUtil::fileExists(installPath)) {
      CompilerWarning(UNINSTALL_FAILED, "The executable was not found at the expected location").print();
      return;
    }
    FileUtil::deleteFile(installPath);
    std::cout << "Successfully uninstalled." << std::endl;
  });

  // Source file
  subCmd->add_option<std::string>("<main-source-file>", cliOptions.mainSourceFile, "Main source file")
      ->check(CLI::ExistingFile)
      ->required();
}

/**
 * Get cli option struct
 *
 * @return Cli options
 */
CliOptions *CliInterface::getOptions() { return &cliOptions; }

/**
 * Checks if compiling is necessary
 *
 * @return Compile or not
 */
bool CliInterface::shouldCompile() const { return compile; }

/**
 * Checks if running is necessary
 *
 * @return Run or not
 */
bool CliInterface::shouldRun() const { return run; }

/**
 * Start the parsing process
 *
 * @param argc Argument count
 * @param argv Argument vector
 * @return Return code
 */
int CliInterface::parse(int argc, char **argv) {
  try {
    app.parse(argc, argv);
  } catch (const CLI::ParseError &e) {
    return app.exit(e);
  }
  return 0;
}

/**
 * Executes the built executable
 */
void CliInterface::runBinary() const {
  // Print status message
  if (cliOptions.printDebugOutput)
    std::cout << std::endl << "Running executable ..." << std::endl;

  // Run executable
  std::system(cliOptions.outputPath.c_str());
}