; ModuleID = 'source.spice'
source_filename = "source.spice"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-w64-windows-gnu"

@intArray = constant [7 x i32] [i32 1, i32 5, i32 4, i32 0, i32 12, i32 12345, i32 9]
@0 = private unnamed_addr constant [23 x i8] c"Item for index %d, %d\0A\00", align 1

define i32 @main() {
entry:
  %result = alloca i32, align 4
  %intArray = alloca [7 x i32], align 4
  %0 = alloca i32, align 4
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  %idx = alloca i32, align 4
  %item = alloca i32, align 4
  %7 = alloca i32, align 4
  store i32 0, i32* %result, align 4
  store i32 1, i32* %0, align 4
  store i32 5, i32* %1, align 4
  store i32 4, i32* %2, align 4
  store i32 0, i32* %3, align 4
  store i32 12, i32* %4, align 4
  store i32 12345, i32* %5, align 4
  store i32 9, i32* %6, align 4
  %8 = load [7 x i32], [7 x i32]* @intArray, align 4
  store [7 x i32] %8, [7 x i32]* %intArray, align 4
  store i32 2, i32* %idx, align 4
  store i32 0, i32* %item, align 4
  %9 = load [7 x i32], [7 x i32]* %intArray, align 4
  %10 = load i32, i32* %idx, align 4
  %11 = getelementptr inbounds [7 x i32], [7 x i32]* %intArray, i32 0, i32 %10
  %12 = load i32, i32* %11, align 4
  store i32 %12, i32* %item, align 4
  br label %foreach.loop

foreach.loop:                                     ; preds = %foreach.inc, %entry
  %13 = load i32, i32* %idx, align 4
  %14 = load i32, i32* %item, align 4
  %15 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([23 x i8], [23 x i8]* @0, i32 0, i32 0), i32 %13, i32 %14)
  %16 = load i32, i32* %idx, align 4
  %17 = add i32 %16, 1
  store i32 %17, i32* %idx, align 4
  store i32 %16, i32* %7, align 4
  %18 = load i32, i32* %idx, align 4
  %19 = icmp slt i32 %18, 6
  br i1 %19, label %foreach.inc, label %foreach.end

foreach.inc:                                      ; preds = %foreach.loop
  %idx1 = load i32, i32* %idx, align 4
  %idx.inc = add i32 %idx1, 1
  store i32 %idx.inc, i32* %idx, align 4
  %20 = getelementptr inbounds [7 x i32], [7 x i32]* %intArray, i32 0, i32 %idx.inc
  %21 = load i32, i32* %20, align 4
  store i32 %21, i32* %item, align 4
  br label %foreach.loop

foreach.end:                                      ; preds = %foreach.loop
  %22 = load i32, i32* %result, align 4
  ret i32 %22
}

declare i32 @printf(i8*, ...)
