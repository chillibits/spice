; ModuleID = 'source.spice'
source_filename = "source.spice"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-w64-windows-gnu"

%_s__String__charptr_long_long = type { ptr, i64, i64 }

@0 = private unnamed_addr constant [7 x i8] c"Hello \00", align 1
@1 = private unnamed_addr constant [7 x i8] c"World!\00", align 1
@2 = private unnamed_addr constant [12 x i8] c"Equals: %d\0A\00", align 1
@3 = private unnamed_addr constant [15 x i8] c"Hello World!?!\00", align 1
@4 = private unnamed_addr constant [16 x i8] c"Not Equals: %d\0A\00", align 1
@5 = private unnamed_addr constant [14 x i8] c"Hello World!!\00", align 1
@6 = private unnamed_addr constant [15 x i8] c"Capacity: %d\0A\0A\00", align 1
@7 = private unnamed_addr constant [13 x i8] c"Content: %s\0A\00", align 1
@8 = private unnamed_addr constant [12 x i8] c"Length: %d\0A\00", align 1
@9 = private unnamed_addr constant [13 x i8] c"Capacity: %d\00", align 1

define i32 @main() local_unnamed_addr {
entry.l3:
  %s = alloca %_s__String__charptr_long_long, align 8
  call void @_mp__String__ctor__string(ptr nonnull %s, ptr nonnull @0)
  %0 = call ptr @_mf__String__getRaw(ptr nonnull %s)
  %1 = call i32 (ptr, ...) @printf(ptr nonnull @7, ptr %0)
  %2 = call i64 @_mf__String__getLength(ptr nonnull %s)
  %3 = call i32 (ptr, ...) @printf(ptr nonnull @8, i64 %2)
  %4 = call i64 @_mf__String__getCapacity(ptr nonnull %s)
  %5 = call i32 (ptr, ...) @printf(ptr nonnull @6, i64 %4)
  call void @_mp__String__append__string(ptr nonnull %s, ptr nonnull @1)
  %6 = call ptr @_mf__String__getRaw(ptr nonnull %s)
  %7 = call i32 (ptr, ...) @printf(ptr nonnull @7, ptr %6)
  %8 = call i64 @_mf__String__getLength(ptr nonnull %s)
  %9 = call i32 (ptr, ...) @printf(ptr nonnull @8, i64 %8)
  %10 = call i64 @_mf__String__getCapacity(ptr nonnull %s)
  %11 = call i32 (ptr, ...) @printf(ptr nonnull @6, i64 %10)
  call void @_mp__String__append__char(ptr nonnull %s, i8 63)
  %12 = call ptr @_mf__String__getRaw(ptr nonnull %s)
  %13 = call i32 (ptr, ...) @printf(ptr nonnull @7, ptr %12)
  %14 = call i64 @_mf__String__getLength(ptr nonnull %s)
  %15 = call i32 (ptr, ...) @printf(ptr nonnull @8, i64 %14)
  %16 = call i64 @_mf__String__getCapacity(ptr nonnull %s)
  %17 = call i32 (ptr, ...) @printf(ptr nonnull @6, i64 %16)
  call void @_mp__String__append__char(ptr nonnull %s, i8 33)
  %18 = call ptr @_mf__String__getRaw(ptr nonnull %s)
  %19 = call i32 (ptr, ...) @printf(ptr nonnull @7, ptr %18)
  %20 = call i64 @_mf__String__getLength(ptr nonnull %s)
  %21 = call i32 (ptr, ...) @printf(ptr nonnull @8, i64 %20)
  %22 = call i64 @_mf__String__getCapacity(ptr nonnull %s)
  %23 = call i32 (ptr, ...) @printf(ptr nonnull @6, i64 %22)
  %24 = call i1 @_mf__String__opEquals__string(ptr nonnull %s, ptr nonnull @3)
  %25 = zext i1 %24 to i32
  %26 = call i32 (ptr, ...) @printf(ptr nonnull @2, i32 %25)
  %27 = call i1 @_mf__String__opEquals__string(ptr nonnull %s, ptr nonnull @5)
  %28 = zext i1 %27 to i32
  %29 = call i32 (ptr, ...) @printf(ptr nonnull @2, i32 %28)
  %30 = call i1 @_mf__String__opNotEquals__string(ptr nonnull %s, ptr nonnull @3)
  %31 = zext i1 %30 to i32
  %32 = call i32 (ptr, ...) @printf(ptr nonnull @4, i32 %31)
  %33 = call i1 @_mf__String__opNotEquals__string(ptr nonnull %s, ptr nonnull @5)
  %34 = zext i1 %33 to i32
  %35 = call i32 (ptr, ...) @printf(ptr nonnull @4, i32 %34)
  call void @_mp__String__clear(ptr nonnull %s)
  %36 = call ptr @_mf__String__getRaw(ptr nonnull %s)
  %37 = call i32 (ptr, ...) @printf(ptr nonnull @7, ptr %36)
  %38 = call i64 @_mf__String__getLength(ptr nonnull %s)
  %39 = call i32 (ptr, ...) @printf(ptr nonnull @8, i64 %38)
  %40 = call i64 @_mf__String__getCapacity(ptr nonnull %s)
  %41 = call i32 (ptr, ...) @printf(ptr nonnull @6, i64 %40)
  call void @_mp__String__reserve__long(ptr nonnull %s, i64 100)
  %42 = call ptr @_mf__String__getRaw(ptr nonnull %s)
  %43 = call i32 (ptr, ...) @printf(ptr nonnull @7, ptr %42)
  %44 = call i64 @_mf__String__getLength(ptr nonnull %s)
  %45 = call i32 (ptr, ...) @printf(ptr nonnull @8, i64 %44)
  %46 = call i64 @_mf__String__getCapacity(ptr nonnull %s)
  %47 = call i32 (ptr, ...) @printf(ptr nonnull @9, i64 %46)
  ret i32 0
}

declare void @_mp__String__ctor__string(ptr, ptr) local_unnamed_addr

; Function Attrs: nofree nounwind
declare noundef i32 @printf(ptr nocapture noundef readonly, ...) local_unnamed_addr #0

declare ptr @_mf__String__getRaw(ptr) local_unnamed_addr

declare i64 @_mf__String__getLength(ptr) local_unnamed_addr

declare i64 @_mf__String__getCapacity(ptr) local_unnamed_addr

declare void @_mp__String__append__string(ptr, ptr) local_unnamed_addr

declare void @_mp__String__append__char(ptr, i8) local_unnamed_addr

declare i1 @_mf__String__opEquals__string(ptr, ptr) local_unnamed_addr

declare i1 @_mf__String__opNotEquals__string(ptr, ptr) local_unnamed_addr

declare void @_mp__String__clear(ptr) local_unnamed_addr

declare void @_mp__String__reserve__long(ptr, i64) local_unnamed_addr

attributes #0 = { nofree nounwind }
