; ModuleID = 'source.spice'
source_filename = "source.spice"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-w64-windows-gnu"

@anon.string.0 = private unnamed_addr constant [7 x i8] c"string\00", align 1
@anon.string.1 = private unnamed_addr constant [7 x i8] c"string\00", align 1
@printf.str.0 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@printf.str.1 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

define private i32 @_Z4testPc(ptr %0) {
  %result = alloca i32, align 4
  %input = alloca ptr, align 8
  store ptr %0, ptr %input, align 8
  ret i32 12
}

define private i32 @_Z6invokePPKPFiPcE(ptr %0) {
  %result = alloca i32, align 4
  %fctPtr = alloca ptr, align 8
  store ptr %0, ptr %fctPtr, align 8
  %2 = load ptr, ptr %fctPtr, align 8
  %3 = load ptr, ptr %2, align 8
  %4 = load ptr, ptr %3, align 8
  %5 = call i32 %4(ptr @anon.string.0)
  ret i32 %5
}

define private i32 @_Z6invokeRKPFiPcE(ptr %0) {
  %result = alloca i32, align 4
  %fctPtr = alloca ptr, align 8
  store ptr %0, ptr %fctPtr, align 8
  %2 = load ptr, ptr %fctPtr, align 8
  %3 = load ptr, ptr %2, align 8
  %4 = call i32 %3(ptr @anon.string.1)
  ret i32 %4
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
  %result = alloca i32, align 4
  %testFct = alloca ptr, align 8
  %testFctPtr = alloca ptr, align 8
  store i32 0, ptr %result, align 4
  store ptr @_Z4testPc, ptr %testFct, align 8
  store ptr %testFct, ptr %testFctPtr, align 8
  %1 = call i32 @_Z6invokePPKPFiPcE(ptr %testFctPtr)
  %2 = call i32 (ptr, ...) @printf(ptr noundef @printf.str.0, i32 %1)
  %3 = call i32 @_Z6invokeRKPFiPcE(ptr %testFct)
  %4 = call i32 (ptr, ...) @printf(ptr noundef @printf.str.1, i32 %3)
  %5 = load i32, ptr %result, align 4
  ret i32 %5
}

; Function Attrs: nofree nounwind
declare noundef i32 @printf(ptr nocapture noundef readonly, ...) #1

attributes #0 = { noinline nounwind optnone uwtable }
attributes #1 = { nofree nounwind }
