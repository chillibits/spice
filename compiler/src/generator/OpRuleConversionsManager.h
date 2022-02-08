// Copyright (c) 2021-2022 ChilliBits. All rights reserved.

#pragma once

#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>

enum PrimitiveType {
    P_TY_DOUBLE,
    P_TY_INT,
    P_TY_SHORT,
    P_TY_LONG,
    P_TY_BYTE_OR_CHAR,
    P_TY_STRING,
    P_TY_BOOL
};

#define COMB(en1, en2) ((en1) | ((en2) << 16))

typedef std::function<void(llvm::Type*)> ConversionAllocator;

class OpRuleConversionsManager {
public:
    explicit OpRuleConversionsManager(llvm::IRBuilder<>* builder) : builder(builder) {}

    // Public methods
    llvm::Value* getPlusEqualInst(llvm::Value* lhs, llvm::Value* rhs, const ConversionAllocator& allocator);
    llvm::Value* getMinusEqualInst(llvm::Value* lhs, llvm::Value* rhs, const ConversionAllocator& allocator);
    llvm::Value* getMulEqualInst(llvm::Value* lhs, llvm::Value* rhs, const ConversionAllocator& allocator);
    llvm::Value* getDivEqualInst(llvm::Value* lhs, llvm::Value* rhs, const ConversionAllocator& allocator);
    llvm::Value* getRemEqualInst(llvm::Value* lhs, llvm::Value* rhs, const ConversionAllocator& allocator);
    llvm::Value* getSHLEqualInst(llvm::Value* lhs, llvm::Value* rhs, const ConversionAllocator& allocator);
    llvm::Value* getSHREqualInst(llvm::Value* lhs, llvm::Value* rhs, const ConversionAllocator& allocator);
    llvm::Value* getAndEqualInst(llvm::Value* lhs, llvm::Value* rhs, const ConversionAllocator& allocator);
    llvm::Value* getOrEqualInst(llvm::Value* lhs, llvm::Value* rhs, const ConversionAllocator& allocator);
    llvm::Value* getXorEqualInst(llvm::Value* lhs, llvm::Value* rhs, const ConversionAllocator& allocator);
    llvm::Value* getBitwiseAndInst(llvm::Value* lhs, llvm::Value* rhs);
    llvm::Value* getBitwiseOrInst(llvm::Value* lhs, llvm::Value* rhs);
    llvm::Value* getBitwiseXorInst(llvm::Value* lhs, llvm::Value* rhs);
    llvm::Value* getEqualInst(llvm::Value* lhs, llvm::Value* rhs);
    llvm::Value* getNotEqualInst(llvm::Value* lhs, llvm::Value* rhs);
    llvm::Value* getLessInst(llvm::Value* lhs, llvm::Value* rhs);
    llvm::Value* getGreaterInst(llvm::Value* lhs, llvm::Value* rhs);
    llvm::Value* getLessEqualInst(llvm::Value* lhs, llvm::Value* rhs);
    llvm::Value* getGreaterEqualInst(llvm::Value* lhs, llvm::Value* rhs);
    llvm::Value* getShiftLeftInst(llvm::Value* lhs, llvm::Value* rhs);
    llvm::Value* getShiftRightInst(llvm::Value* lhs, llvm::Value* rhs);
    llvm::Value* getPlusInst(llvm::Value* lhs, llvm::Value* rhs);
    llvm::Value* getMinusInst(llvm::Value* lhs, llvm::Value* rhs);
    llvm::Value* getMulInst(llvm::Value* lhs, llvm::Value* rhs);
    llvm::Value* getDivInst(llvm::Value* lhs, llvm::Value* rhs);
    llvm::Value* getRemInst(llvm::Value* lhs, llvm::Value* rhs);
    llvm::Value* getPrefixMinusInst(llvm::Value* lhs);
    llvm::Value* getPrefixPlusPlusInst(llvm::Value* lhs);
    llvm::Value* getPrefixMinusMinusInst(llvm::Value* lhs);
    llvm::Value* getPrefixNotInst(llvm::Value* lhs);
    llvm::Value* getPrefixBitwiseNotInst(llvm::Value* lhs);
    llvm::Value* getPostfixPlusPlusInst(llvm::Value* lhs);
    llvm::Value* getPostfixMinusMinusInst(llvm::Value* lhs);
    llvm::Value* getCastInst(llvm::Type*, llvm::Value* lhs);
private:
    // Members
    llvm::IRBuilder<>* builder;

    // Private methods
    static PrimitiveType getPrimitiveTypeFromLLVMType(llvm::Type* llvmType);
    static bool isDouble(llvm::Type* llvmType);
    static bool isInt(llvm::Type* llvmType);
    static bool isShort(llvm::Type* llvmType);
    static bool isLong(llvm::Type* llvmType);
    static bool isByteOrChar(llvm::Type* llvmType);
    static bool isString(llvm::Type* llvmType);
    static bool isBool(llvm::Type* llvmType);
};