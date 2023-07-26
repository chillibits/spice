; ModuleID = 'source.spice'
source_filename = "source.spice"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-w64-windows-gnu"

@anon.string.0 = private unnamed_addr constant [68 x i8] c"Assertion failed: Condition 'asDouble == 123.0' evaluated to false.\00", align 1
@anon.string.1 = private unnamed_addr constant [67 x i8] c"Assertion failed: Condition 'asShort == -345s' evaluated to false.\00", align 1
@anon.string.2 = private unnamed_addr constant [68 x i8] c"Assertion failed: Condition 'asLong == 459873l' evaluated to false.\00", align 1
@anon.string.3 = private unnamed_addr constant [70 x i8] c"Assertion failed: Condition 'asByte == (byte) 12' evaluated to false.\00", align 1
@anon.string.4 = private unnamed_addr constant [64 x i8] c"Assertion failed: Condition 'asChar == 'B'' evaluated to false.\00", align 1
@anon.string.5 = private unnamed_addr constant [66 x i8] c"Assertion failed: Condition 'asBool1 == true' evaluated to false.\00", align 1
@anon.string.6 = private unnamed_addr constant [67 x i8] c"Assertion failed: Condition 'asBool2 == false' evaluated to false.\00", align 1
@anon.string.7 = private unnamed_addr constant [67 x i8] c"Assertion failed: Condition 'isPowerOfTwo(16)' evaluated to false.\00", align 1
@anon.string.8 = private unnamed_addr constant [68 x i8] c"Assertion failed: Condition '!isPowerOfTwo(31)' evaluated to false.\00", align 1
@printf.str.0 = private unnamed_addr constant [25 x i8] c"All assertions succeeded\00", align 1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() local_unnamed_addr #0 {
  %1 = tail call double @_Z8toDoublei(i32 123) #3
  %2 = fcmp oeq double %1, 1.230000e+02
  br i1 %2, label %assert.exit.L6, label %assert.then.L6, !prof !0

assert.then.L6:                                   ; preds = %0
  %3 = tail call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) @anon.string.0)
  tail call void @llvm.trap()
  unreachable

assert.exit.L6:                                   ; preds = %0
  %4 = tail call i16 @_Z7toShorti(i32 -345) #3
  %5 = icmp eq i16 %4, -345
  br i1 %5, label %assert.exit.L10, label %assert.then.L10, !prof !0

assert.then.L10:                                  ; preds = %assert.exit.L6
  %6 = tail call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) @anon.string.1)
  tail call void @llvm.trap()
  unreachable

assert.exit.L10:                                  ; preds = %assert.exit.L6
  %7 = tail call i64 @_Z6toLongi(i32 459873) #3
  %8 = icmp eq i64 %7, 459873
  br i1 %8, label %assert.exit.L14, label %assert.then.L14, !prof !0

assert.then.L14:                                  ; preds = %assert.exit.L10
  %9 = tail call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) @anon.string.2)
  tail call void @llvm.trap()
  unreachable

assert.exit.L14:                                  ; preds = %assert.exit.L10
  %10 = tail call i8 @_Z6toBytei(i32 12) #3
  %11 = icmp eq i8 %10, 12
  br i1 %11, label %assert.exit.L18, label %assert.then.L18, !prof !0

assert.then.L18:                                  ; preds = %assert.exit.L14
  %12 = tail call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) @anon.string.3)
  tail call void @llvm.trap()
  unreachable

assert.exit.L18:                                  ; preds = %assert.exit.L14
  %13 = tail call i8 @_Z6toChari(i32 66) #3
  %14 = icmp eq i8 %13, 66
  br i1 %14, label %assert.exit.L22, label %assert.then.L22, !prof !0

assert.then.L22:                                  ; preds = %assert.exit.L18
  %15 = tail call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) @anon.string.4)
  tail call void @llvm.trap()
  unreachable

assert.exit.L22:                                  ; preds = %assert.exit.L18
  %16 = tail call i1 @_Z6toBooli(i32 1) #3
  br i1 %16, label %assert.exit.L31, label %assert.then.L31, !prof !0

assert.then.L31:                                  ; preds = %assert.exit.L22
  %17 = tail call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) @anon.string.5)
  tail call void @llvm.trap()
  unreachable

assert.exit.L31:                                  ; preds = %assert.exit.L22
  %18 = tail call i1 @_Z6toBooli(i32 0) #3
  br i1 %18, label %assert.then.L33, label %assert.exit.L33, !prof !1

assert.then.L33:                                  ; preds = %assert.exit.L31
  %19 = tail call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) @anon.string.6)
  tail call void @llvm.trap()
  unreachable

assert.exit.L33:                                  ; preds = %assert.exit.L31
  %20 = tail call i1 @_Z12isPowerOfTwoi(i32 16) #3
  br i1 %20, label %assert.exit.L36, label %assert.then.L36, !prof !0

assert.then.L36:                                  ; preds = %assert.exit.L33
  %21 = tail call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) @anon.string.7)
  tail call void @llvm.trap()
  unreachable

assert.exit.L36:                                  ; preds = %assert.exit.L33
  %22 = tail call i1 @_Z12isPowerOfTwoi(i32 31) #3
  br i1 %22, label %assert.then.L37, label %assert.exit.L37, !prof !1

assert.then.L37:                                  ; preds = %assert.exit.L36
  %23 = tail call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) @anon.string.8)
  tail call void @llvm.trap()
  unreachable

assert.exit.L37:                                  ; preds = %assert.exit.L36
  %24 = tail call i32 (ptr, ...) @printf(ptr noundef nonnull dereferenceable(1) @printf.str.0)
  ret i32 0
}

declare double @_Z8toDoublei(i32) local_unnamed_addr

; Function Attrs: nofree nounwind
declare noundef i32 @printf(ptr nocapture noundef readonly, ...) local_unnamed_addr #1

; Function Attrs: cold noreturn nounwind
declare void @llvm.trap() #2

declare i16 @_Z7toShorti(i32) local_unnamed_addr

declare i64 @_Z6toLongi(i32) local_unnamed_addr

declare i8 @_Z6toBytei(i32) local_unnamed_addr

declare i8 @_Z6toChari(i32) local_unnamed_addr

declare i1 @_Z6toBooli(i32) local_unnamed_addr

declare i1 @_Z12isPowerOfTwoi(i32) local_unnamed_addr

attributes #0 = { noinline nounwind optnone uwtable }
attributes #1 = { nofree nounwind }
attributes #2 = { cold noreturn nounwind }
attributes #3 = { nounwind }

!0 = !{!"branch_weights", i32 2000, i32 1}
!1 = !{!"branch_weights", i32 1, i32 2000}
