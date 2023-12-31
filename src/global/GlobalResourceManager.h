// Copyright (c) 2021-2023 ChilliBits. All rights reserved.

#pragma once

#include <exception/ErrorManager.h>
#include <global/CacheManager.h>
#include <global/RuntimeModuleManager.h>
#include <linker/ExternalLinkerInterface.h>
#include <util/CodeLoc.h>
#include <util/Timer.h>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Target/TargetMachine.h>

#include "../../lib/thread-pool/thread-pool.hpp"
#include "../../lib/thread-pool/thread-pool-utils.hpp"

namespace spice::compiler {

// Forward declarations
struct CliOptions;
class SourceFile;

// Constants
const char *const MAIN_FILE_NAME = "root";
const char *const LTO_FILE_NAME = "lto-module";

/**
 * The GlobalResourceManager is instantiated at startup of the compiler and serves as distribution point for globally used assets.
 * This component owns all SourceFile instances and AST nodes and therefore is the resource root of the compiler.
 * Other components of the compiler can request the required global resources from the GlobalResourceManager.
 */
class GlobalResourceManager {
public:
  // Constructors
  explicit GlobalResourceManager(const CliOptions &cliOptions);
  GlobalResourceManager(const GlobalResourceManager &) = delete; // Global resource manager can only exist exactly once
  ~GlobalResourceManager();

  // Public methods
  SourceFile *createSourceFile(SourceFile *parent, const std::string &depName, const std::filesystem::path &path, bool isStdFile);
  uint64_t getNextCustomTypeId();

  // Public members
  llvm::LLVMContext context;
  llvm::IRBuilder<> builder = llvm::IRBuilder<>(context);
  std::unique_ptr<llvm::Module> ltoModule;
  std::unique_ptr<llvm::TargetMachine> targetMachine;
  std::unordered_map<std::string, std::unique_ptr<SourceFile>> sourceFiles; // The GlobalResourceManager owns all source files
  std::vector<std::unique_ptr<ASTNode>> astNodes;                           // The GlobalResourceManager owns all AST nodes
  const CliOptions &cliOptions;
  ExternalLinkerInterface linker;
  CacheManager cacheManager;
  RuntimeModuleManager runtimeModuleManager;
  Timer totalTimer;
  BS::thread_pool threadPool = BS::thread_pool(cliOptions.compileJobCount);
  BS::synced_stream tout;
  std::mutex objectEmitLock;
  ErrorManager errorManager;

private:
  // Private members
  std::atomic<uint64_t> nextCustomTypeId = UINT8_MAX + 1; // Start at 256 because all primitive types come first
};

} // namespace spice::compiler