; ModuleID = 'source.spice'
source_filename = "source.spice"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-w64-windows-gnu"

@0 = private unnamed_addr constant [23 x i8] c"Item 0: %d, item 2: %d\00", align 1

define i32 @main() {
entry:
  %result = alloca i32, align 4
  %value0 = alloca i32, align 4
  %intArray = alloca [5 x i32], align 4
  %0 = alloca i32, align 4
  %1 = alloca i32, align 4
  %2 = alloca [5 x i32], align 4
  store i32 0, i32* %result, align 4
  store i32 2, i32* %value0, align 4
  store i32 7, i32* %0, align 4
  store i32 4, i32* %1, align 4
  %3 = load i32, i32* %value0, align 4
  %4 = getelementptr inbounds [5 x i32], [5 x i32]* %2, i32 0, i32 0
  store i32 %3, i32* %4, align 4
  %5 = load i32, i32* %0, align 4
  %6 = getelementptr inbounds [5 x i32], [5 x i32]* %2, i32 0, i32 1
  store i32 %5, i32* %6, align 4
  %7 = load i32, i32* %1, align 4
  %8 = getelementptr inbounds [5 x i32], [5 x i32]* %2, i32 0, i32 2
  store i32 %7, i32* %8, align 4
  %9 = load [5 x i32], [5 x i32]* %2, align 4
  store [5 x i32] %9, [5 x i32]* %intArray, align 4
  %10 = load [5 x i32], [5 x i32]* %intArray, align 4
  %11 = getelementptr inbounds [5 x i32], [5 x i32]* %intArray, i32 0, i32 2
  %12 = load i32, i32* %11, align 4
  %13 = mul i32 %12, 11
  store i32 %13, i32* %11, align 4
  %14 = load [5 x i32], [5 x i32]* %intArray, align 4
  %15 = getelementptr inbounds [5 x i32], [5 x i32]* %intArray, i32 0, i32 0
  store i32 3, i32* %15, align 4
  %16 = load [5 x i32], [5 x i32]* %intArray, align 4
  %17 = getelementptr inbounds [5 x i32], [5 x i32]* %intArray, i32 0, i32 0
  %18 = load i32, i32* %17, align 4
  %19 = load [5 x i32], [5 x i32]* %intArray, align 4
  %20 = getelementptr inbounds [5 x i32], [5 x i32]* %intArray, i32 0, i32 2
  %21 = load i32, i32* %20, align 4
  %22 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([23 x i8], [23 x i8]* @0, i32 0, i32 0), i32 %18, i32 %21)
  %23 = load i32, i32* %result, align 4
  ret i32 %23
}

declare i32 @printf(i8*, ...)
