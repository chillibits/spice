; ModuleID = 'source.spice'
source_filename = "source.spice"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-w64-windows-gnu"

@0 = private unnamed_addr constant [15 x i8] c"The age is: %d\00", align 1

; Function Attrs: nounwind
define internal i32 @_f_getAge() #0 {
entry:
  %result = alloca i32, align 4
  %i = alloca i1, align 1
  %0 = alloca i1, align 1
  %1 = alloca i32, align 4
  %2 = alloca i1, align 1
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  store i1 true, i1* %0, align 1
  %5 = load i1, i1* %0, align 1
  br i1 %5, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  store i32 20, i32* %1, align 4
  %6 = load i32, i32* %1, align 4
  store i32 %6, i32* %result, align 4
  %7 = load i32, i32* %result, align 4
  ret i32 %7

if.else:                                          ; preds = %entry
  store i1 false, i1* %2, align 1
  %8 = load i1, i1* %2, align 1
  store i1 %8, i1* %i, align 1
  %9 = load i1, i1* %i, align 1
  br i1 %9, label %if.then1, label %if.end

if.then1:                                         ; preds = %if.else
  store i32 19, i32* %3, align 4
  %10 = load i32, i32* %3, align 4
  store i32 %10, i32* %result, align 4
  br label %if.end

if.end:                                           ; preds = %if.then1, %if.else
  br label %if.end2

if.end2:                                          ; preds = %if.end
  store i32 15, i32* %4, align 4
  %11 = load i32, i32* %4, align 4
  ret i32 %11
}

define i32 @main() {
entry:
  %result = alloca i32, align 4
  %age = alloca i32, align 4
  %0 = alloca i32, align 4
  store i32 0, i32* %result, align 4
  %1 = call i32 @_f_getAge()
  store i32 %1, i32* %0, align 4
  %2 = load i32, i32* %0, align 4
  store i32 %2, i32* %age, align 4
  %3 = load i32, i32* %age, align 4
  %4 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([15 x i8], [15 x i8]* @0, i32 0, i32 0), i32 %3)
  %5 = load i32, i32* %result, align 4
  ret i32 %5
}

declare i32 @printf(i8*, ...)

attributes #0 = { nounwind }
