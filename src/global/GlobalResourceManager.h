// Copyright (c) 2021-2023 ChilliBits. All rights reserved.

#pragma once

#include <global/CacheManager.h>
#include <global/RuntimeModuleManager.h>
#include <global/ThreadFactory.h>
#include <linker/ExternalLinkerInterface.h>
#include <util/Timer.h>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Target/TargetMachine.h>

#include "../../lib/thread-pool/thread-pool.hpp"

namespace spice::compiler {

// Forward declarations
struct CliOptions;

/**
 * The GlobalResourceManager is instantiated at startup of the compiler and serves as distribution point for globally used assets.
 * Other components of the compiler can request the required global resources from the GlobalResourceManager.
 */
class GlobalResourceManager {
public:
  // Constructors
  explicit GlobalResourceManager(const CliOptions &cliOptions);
  GlobalResourceManager(const GlobalResourceManager &) = delete;

  // Public members
  const CliOptions &cliOptions;
  ThreadFactory threadFactory;
  ExternalLinkerInterface linker;
  CacheManager cacheManager;
  RuntimeModuleManager runtimeModuleManager;
  llvm::LLVMContext context;
  llvm::IRBuilder<> builder = llvm::IRBuilder<>(context);
  llvm::TargetMachine *targetMachine;
  Timer totalTimer;
  BS::thread_pool threadPool = BS::thread_pool(cliOptions.compileJobCount);
  BS::synced_stream tout;
  std::mutex objectEmitLock;
};

} // namespace spice::compiler