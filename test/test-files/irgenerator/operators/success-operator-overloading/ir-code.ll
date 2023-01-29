; ModuleID = 'source.spice'
source_filename = "source.spice"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-w64-windows-gnu"

%__Counter__long = type { i64 }

@printf.str.0 = private unnamed_addr constant [20 x i8] c"Counter1 value: %d\0A\00", align 1
@printf.str.1 = private unnamed_addr constant [20 x i8] c"Counter2 value: %d\0A\00", align 1
@printf.str.2 = private unnamed_addr constant [20 x i8] c"Counter3 value: %d\0A\00", align 1

define private void @_mp__Counter__void__ctor__long(ptr noundef nonnull %0, i64 %1) {
  %this = alloca ptr, align 8
  %initialValue = alloca i64, align 8
  store ptr %0, ptr %this, align 8
  store i64 %1, ptr %initialValue, align 8
  %3 = load ptr, ptr %this, align 8
  %value = getelementptr inbounds %__Counter__long, ptr %3, i32 0, i32 0
  %4 = load i64, ptr %initialValue, align 8
  store i64 %4, ptr %value, align 8
  ret void
}

define private i64 @_mf__Counter__long__getValue(ptr noundef nonnull %0) {
  %result = alloca i64, align 8
  %this = alloca ptr, align 8
  store ptr %0, ptr %this, align 8
  %2 = load ptr, ptr %this, align 8
  %value = getelementptr inbounds %__Counter__long, ptr %2, i32 0, i32 0
  %3 = load i64, ptr %value, align 8
  ret i64 %3
}

define private %__Counter__long @_f__void__Counter__op.plus__Counter_Counter(%__Counter__long %0, %__Counter__long %1) {
  %result = alloca %__Counter__long, align 8
  %c1 = alloca %__Counter__long, align 8
  %c2 = alloca %__Counter__long, align 8
  %3 = alloca %__Counter__long, align 8
  store %__Counter__long %0, ptr %c1, align 8
  store %__Counter__long %1, ptr %c2, align 8
  %value = getelementptr inbounds %__Counter__long, ptr %c1, i32 0, i32 0
  %value1 = getelementptr inbounds %__Counter__long, ptr %c2, i32 0, i32 0
  %4 = load i64, ptr %value1, align 8
  %5 = load i64, ptr %value, align 8
  %6 = add i64 %5, %4
  call void @_mp__Counter__void__ctor__long(ptr %3, i64 %6)
  %7 = load %__Counter__long, ptr %3, align 8
  ret %__Counter__long %7
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
  %result = alloca i32, align 4
  %1 = alloca %__Counter__long, align 8
  %counter1 = alloca %__Counter__long, align 8
  %2 = alloca %__Counter__long, align 8
  %counter2 = alloca %__Counter__long, align 8
  %counter3 = alloca %__Counter__long, align 8
  store i32 0, ptr %result, align 4
  call void @_mp__Counter__void__ctor__long(ptr %1, i64 2)
  %3 = load %__Counter__long, ptr %1, align 8
  store %__Counter__long %3, ptr %counter1, align 8
  call void @_mp__Counter__void__ctor__long(ptr %2, i64 3)
  %4 = load %__Counter__long, ptr %2, align 8
  store %__Counter__long %4, ptr %counter2, align 8
  %5 = call i64 @_mf__Counter__long__getValue(ptr %counter1)
  %6 = call i32 (ptr, ...) @printf(ptr noundef @printf.str.0, i64 %5)
  %7 = call i64 @_mf__Counter__long__getValue(ptr %counter2)
  %8 = call i32 (ptr, ...) @printf(ptr noundef @printf.str.1, i64 %7)
  %9 = load %__Counter__long, ptr %counter1, align 8
  %10 = load %__Counter__long, ptr %counter2, align 8
  %11 = call %__Counter__long @_f__void__Counter__op.plus__Counter_Counter(%__Counter__long %9, %__Counter__long %10)
  store %__Counter__long %11, ptr %counter3, align 8
  %12 = call i64 @_mf__Counter__long__getValue(ptr %counter3)
  %13 = call i32 (ptr, ...) @printf(ptr noundef @printf.str.2, i64 %12)
  %14 = load i32, ptr %result, align 4
  ret i32 %14
}

declare i32 @printf(ptr noundef, ...)

attributes #0 = { noinline nounwind optnone uwtable }
