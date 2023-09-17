; ModuleID = 'source.spice'
source_filename = "source.spice"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-w64-windows-gnu"

%struct.Vector = type { i1, ptr }

@anon.string.0 = private unnamed_addr constant [12 x i8] c"Test string\00", align 1
@0 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@1 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@anon.string.1 = private unnamed_addr constant [13 x i8] c"Hello World!\00", align 1
@printf.str.0 = private unnamed_addr constant [16 x i8] c"Fields: %d, %s\0A\00", align 1
@anon.string.2 = private unnamed_addr constant [16 x i8] c"Another message\00", align 1
@printf.str.1 = private unnamed_addr constant [16 x i8] c"Fields: %d, %s\0A\00", align 1
@printf.str.2 = private unnamed_addr constant [13 x i8] c"Message: %s\0A\00", align 1

define private void @_ZN6Vector4ctorEv(ptr noundef nonnull %0) {
  %this = alloca ptr, align 8
  %msg = alloca ptr, align 8
  store ptr %0, ptr %this, align 8
  store ptr @anon.string.0, ptr %msg, align 8
  store %struct.Vector { i1 false, ptr @0 }, ptr %0, align 8
  %2 = load ptr, ptr %this, align 8
  %field1_addr = getelementptr inbounds %struct.Vector, ptr %2, i32 0, i32 0
  store i1 false, ptr %field1_addr, align 1
  %3 = load ptr, ptr %this, align 8
  %field2_addr = getelementptr inbounds %struct.Vector, ptr %3, i32 0, i32 1
  %4 = load ptr, ptr %msg, align 8
  store ptr %4, ptr %field2_addr, align 8
  ret void
}

define private void @_ZN6Vector4ctorEPc(ptr noundef nonnull %0, ptr %1) {
  %this = alloca ptr, align 8
  %msg = alloca ptr, align 8
  store ptr %0, ptr %this, align 8
  store ptr %1, ptr %msg, align 8
  store %struct.Vector { i1 false, ptr @1 }, ptr %0, align 8
  %3 = load ptr, ptr %this, align 8
  %field1_addr = getelementptr inbounds %struct.Vector, ptr %3, i32 0, i32 0
  store i1 false, ptr %field1_addr, align 1
  %4 = load ptr, ptr %this, align 8
  %field2_addr = getelementptr inbounds %struct.Vector, ptr %4, i32 0, i32 1
  %5 = load ptr, ptr %msg, align 8
  store ptr %5, ptr %field2_addr, align 8
  ret void
}

define private ptr @_ZN6Vector4testEv(ptr noundef nonnull %0) {
  %result = alloca ptr, align 8
  %this = alloca ptr, align 8
  store ptr %0, ptr %this, align 8
  ret ptr @anon.string.1
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
  %result = alloca i32, align 4
  %vec = alloca %struct.Vector, align 8
  %1 = alloca %struct.Vector, align 8
  store i32 0, ptr %result, align 4
  call void @_ZN6Vector4ctorEv(ptr %vec)
  %field1_addr = getelementptr inbounds %struct.Vector, ptr %vec, i32 0, i32 0
  %2 = load i1, ptr %field1_addr, align 1
  %3 = zext i1 %2 to i32
  %field2_addr = getelementptr inbounds %struct.Vector, ptr %vec, i32 0, i32 1
  %4 = load ptr, ptr %field2_addr, align 8
  %5 = call i32 (ptr, ...) @printf(ptr noundef @printf.str.0, i32 %3, ptr %4)
  call void @_ZN6Vector4ctorEPc(ptr %1, ptr @anon.string.2)
  %6 = load %struct.Vector, ptr %1, align 8
  store %struct.Vector %6, ptr %vec, align 8
  %field1_addr1 = getelementptr inbounds %struct.Vector, ptr %vec, i32 0, i32 0
  %7 = load i1, ptr %field1_addr1, align 1
  %8 = zext i1 %7 to i32
  %field2_addr2 = getelementptr inbounds %struct.Vector, ptr %vec, i32 0, i32 1
  %9 = load ptr, ptr %field2_addr2, align 8
  %10 = call i32 (ptr, ...) @printf(ptr noundef @printf.str.1, i32 %8, ptr %9)
  %11 = call ptr @_ZN6Vector4testEv(ptr %vec)
  %12 = call i32 (ptr, ...) @printf(ptr noundef @printf.str.2, ptr %11)
  %13 = load i32, ptr %result, align 4
  ret i32 %13
}

; Function Attrs: nofree nounwind
declare noundef i32 @printf(ptr nocapture noundef readonly, ...) #1

attributes #0 = { noinline nounwind optnone uwtable }
attributes #1 = { nofree nounwind }
