// Copyright (c) 2021-2022 ChilliBits. All rights reserved.

#include "CompilerPass.h"

#include <SourceFile.h>

namespace spice::compiler {

CompilerPass::CompilerPass(GlobalResourceManager &resourceManager, SourceFile *sourceFile)
    : resourceManager(resourceManager), cliOptions(resourceManager.cliOptions), sourceFile(sourceFile),
      rootScope(sourceFile->globalScope.get()) {}

} // namespace spice::compiler