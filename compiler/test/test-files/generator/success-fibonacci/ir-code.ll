; ModuleID = 'Module'
source_filename = "Module"

@0 = private unnamed_addr constant [11 x i8] c"Result: %d\00", align 1

declare i32 @printf(i8*, ...)

define i32 @"fib(int)"(i32 %0) {
entry:
  %n = alloca i32, align 4
  store i32 %0, i32* %n, align 4
  %result = alloca i32, align 4
  %1 = load i32, i32* %n, align 4
  %le = icmp sle i32 %1, 1
  br i1 %le, label %then, label %end

then:                                             ; preds = %entry
  ret i32 1
  br label %end

end:                                              ; preds = %then, %entry
  %2 = load i32, i32* %n, align 4
  %sub = sub i32 %2, 1
  %3 = call i32 @"fib(int)"(i32 %sub)
  %4 = load i32, i32* %n, align 4
  %sub1 = sub i32 %4, 2
  %5 = call i32 @"fib(int)"(i32 %sub1)
  %add = add i32 %3, %5
  ret i32 %add
}

define i32 @main() {
main_entry:
  %result = alloca i32, align 4
  %0 = call i32 @"fib(int)"(i32 46)
  %1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([11 x i8], [11 x i8]* @0, i32 0, i32 0), i32 %0)
  ret i32 0
}
