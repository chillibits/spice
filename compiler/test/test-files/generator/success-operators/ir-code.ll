; ModuleID = 'source.spice'
source_filename = "source.spice"

@0 = private unnamed_addr constant [10 x i8] c"Value: %d\00", align 1

declare i32 @printf(i8*, ...)

define i32 @main() {
entry:
  %result = alloca i32, align 4
  store i32 0, i32* %result, align 4
  %0 = alloca i32, align 4
  store i32 1, i32* %0, align 4
  %1 = load i32, i32* %0, align 4
  %val = alloca i32, align 4
  store i32 %1, i32* %val, align 4
  %2 = alloca i32, align 4
  store i32 1, i32* %2, align 4
  %3 = load i32, i32* %2, align 4
  %4 = add i32 %3, 1
  store i32 %4, i32* %2, align 4
  %5 = load i32, i32* %2, align 4
  %6 = sub i32 %5, 1
  store i32 %6, i32* %2, align 4
  %7 = load i32, i32* %2, align 4
  %8 = alloca i32, align 4
  store i32 2, i32* %8, align 4
  %9 = load i32, i32* %8, align 4
  %10 = mul i32 %7, %9
  %11 = alloca i32, align 4
  store i32 %10, i32* %11, align 4
  %12 = alloca i32, align 4
  store i32 2, i32* %12, align 4
  %13 = load i32, i32* %11, align 4
  %14 = load i32, i32* %12, align 4
  %15 = shl i32 %13, %14
  %16 = alloca i32, align 4
  store i32 %15, i32* %16, align 4
  %17 = load i32, i32* %16, align 4
  %18 = getelementptr inbounds i32, i32* %val, i32 0
  %19 = load i32, i32* %18, align 4
  %add = add i32 %19, %17
  store i32 %add, i32* %18, align 4
  %20 = getelementptr inbounds i32, i32* %val, i32 0
  %21 = load i32, i32* %20, align 4
  %22 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([10 x i8], [10 x i8]* @0, i32 0, i32 0), i32 %21)
  %23 = load i32, i32* %result, align 4
  ret i32 %23
}
