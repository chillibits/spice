; ModuleID = 'Module'
source_filename = "Module"

@0 = private unnamed_addr constant [15 x i8] c"Condition true\00", align 1
@1 = private unnamed_addr constant [16 x i8] c"Condition false\00", align 1

declare i32 @printf(i8*, ...)

define i32 @main() {
main_entry:
  %condition = alloca i1, align 1
  %result = alloca i32, align 4
  store i1 true, i1* %condition, align 1
  %0 = load i1, i1* %condition, align 1
  br i1 %0, label %then, label %end

then:                                             ; preds = %main_entry
  %1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([15 x i8], [15 x i8]* @0, i32 0, i32 0))
  ret i32 0
  br label %end

end:                                              ; preds = %then, %main_entry
  %2 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([16 x i8], [16 x i8]* @1, i32 0, i32 0))
  ret i32 0
}
