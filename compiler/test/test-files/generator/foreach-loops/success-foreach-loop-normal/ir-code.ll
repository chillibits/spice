; ModuleID = 'source.spice'
source_filename = "source.spice"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-w64-windows-gnu"

@intArray = dso_local constant [7 x i32] [i32 1, i32 5, i32 4, i32 0, i32 12, i32 12345, i32 9]
@0 = private unnamed_addr constant [22 x i8] c"Item at index %d: %d\0A\00", align 1

declare i32 @printf(i8*, ...)

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
  store i32 0, i32* %result, align 4
  store i32 1, i32* %0, align 4
  store i32 5, i32* %1, align 4
  store i32 4, i32* %2, align 4
  store i32 0, i32* %3, align 4
  store i32 12, i32* %4, align 4
  store i32 12345, i32* %5, align 4
  store i32 9, i32* %6, align 4
  %7 = load [7 x i32], [7 x i32]* @intArray, align 4
  store [7 x i32] %7, [7 x i32]* %intArray, align 4
  store i32 0, i32* %idx, align 4
  %8 = load [7 x i32], [7 x i32]* %intArray, align 4
  %9 = load i32, i32* %idx, align 4
  %10 = getelementptr inbounds [7 x i32], [7 x i32]* %intArray, i32 0, i32 %9
  %11 = load i32, i32* %10, align 4
  store i32 %11, i32* %item, align 4
  br label %foreach.loop

foreach.loop:                                     ; preds = %foreach.inc, %entry
  %12 = load i32, i32* %idx, align 4
  %13 = load i32, i32* %item, align 4
  %14 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([22 x i8], [22 x i8]* @0, i32 0, i32 0), i32 %12, i32 %13)
  br label %foreach.inc

foreach.inc:                                      ; preds = %foreach.loop
  %15 = load i32, i32* %idx, align 4
  %foreach_idx_inc = add i32 %15, 1
  store i32 %foreach_idx_inc, i32* %idx, align 4
  %16 = getelementptr inbounds [7 x i32], [7 x i32]* %intArray, i32 0, i32 %foreach_idx_inc
  %17 = load i32, i32* %16, align 4
  store i32 %17, i32* %item, align 4
  br label %foreach.loop

foreach.end:                                      ; No predecessors!
  %18 = load i32, i32* %result, align 4
  ret i32 %18
}
