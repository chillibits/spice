; ModuleID = 'source.spice'
source_filename = "source.spice"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-w64-windows-gnu"

%_s__int__Stack__intptr_long_long_int = type { ptr, i64, i64, i32 }

@0 = private unnamed_addr constant [16 x i8] c"Stack size: %d\0A\00", align 1
@1 = private unnamed_addr constant [20 x i8] c"Stack capacity: %d\0A\00", align 1
@2 = private unnamed_addr constant [18 x i8] c"Stack item 3: %d\0A\00", align 1
@3 = private unnamed_addr constant [18 x i8] c"Stack item 2: %d\0A\00", align 1
@4 = private unnamed_addr constant [18 x i8] c"Stack item 1: %d\0A\00", align 1

define i32 @main() local_unnamed_addr {
entry.l3:
  %0 = alloca %_s__int__Stack__intptr_long_long_int, align 8
  call void @_mp__Stack_int__void__ctor(ptr nonnull %0)
  call void @_mp__Stack_int__void__push__int(ptr nonnull %0, i32 123)
  call void @_mp__Stack_int__void__push__int(ptr nonnull %0, i32 456)
  call void @_mp__Stack_int__void__push__int(ptr nonnull %0, i32 789)
  %1 = call i64 @_mf__Stack_int__long__getSize(ptr nonnull %0)
  %2 = call i32 (ptr, ...) @printf(ptr nonnull @0, i64 %1)
  %3 = call i64 @_mf__Stack_int__long__getCapacity(ptr nonnull %0)
  %4 = call i32 (ptr, ...) @printf(ptr nonnull @1, i64 %3)
  %5 = call i32 @_mf__Stack_int__int__pop(ptr nonnull %0)
  %6 = call i32 (ptr, ...) @printf(ptr nonnull @2, i32 %5)
  %7 = call i32 @_mf__Stack_int__int__pop(ptr nonnull %0)
  %8 = call i32 (ptr, ...) @printf(ptr nonnull @3, i32 %7)
  %9 = call i32 @_mf__Stack_int__int__pop(ptr nonnull %0)
  %10 = call i32 (ptr, ...) @printf(ptr nonnull @4, i32 %9)
  call void @_mp__Stack_int__void__dtor(ptr nonnull %0)
  ret i32 0
}

declare void @_mp__Stack_int__void__ctor(ptr) local_unnamed_addr

declare void @_mp__Stack_int__void__push__int(ptr, i32) local_unnamed_addr

; Function Attrs: nofree nounwind
declare noundef i32 @printf(ptr nocapture noundef readonly, ...) local_unnamed_addr #0

declare i64 @_mf__Stack_int__long__getSize(ptr) local_unnamed_addr

declare i64 @_mf__Stack_int__long__getCapacity(ptr) local_unnamed_addr

declare i32 @_mf__Stack_int__int__pop(ptr) local_unnamed_addr

declare void @_mp__Stack_int__void__dtor(ptr) local_unnamed_addr

attributes #0 = { nofree nounwind }
