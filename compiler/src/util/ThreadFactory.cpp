// Copyright (c) 2021-2022 ChilliBits. All rights reserved.

#include "ThreadFactory.h"

// Instance of the module registry
ThreadFactory* ThreadFactory::instance = nullptr;

/**
 * Retrieve an instance of the thread factory.
 *
 * @return Instance of the thread factory
 */
ThreadFactory* ThreadFactory::getInstance() {
    if (!instance) instance = new ThreadFactory();
    return instance;
}

/**
 * Drops an instance of the thread factory
 */
void ThreadFactory::dropInstance() {
    instance = nullptr;
}

/**
 * Retrieve the next free function suffix
 *
 * @return Free function suffix
 */
int ThreadFactory::getNextFunctionSuffix() {
    return nextFctSuffix++;
}
