; ModuleID = 'source.spice'
source_filename = "source.spice"

@printf.str.0 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@printf.str.1 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@printf.str.2 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@printf.str.3 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@printf.str.4 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
  %result = alloca i32, align 4
  store i32 0, ptr %result, align 4
  %1 = call i32 (ptr, ...) @printf(ptr noundef @printf.str.0, i32 12)
  %2 = call i32 (ptr, ...) @printf(ptr noundef @printf.str.1, i32 2)
  %3 = call i32 (ptr, ...) @printf(ptr noundef @printf.str.2, i32 1)
  %4 = call i32 (ptr, ...) @printf(ptr noundef @printf.str.3, i32 3)
  %5 = call i32 (ptr, ...) @printf(ptr noundef @printf.str.4, i32 0)
  %6 = load i32, ptr %result, align 4
  ret i32 %6
}

; Function Attrs: nofree nounwind
declare noundef i32 @printf(ptr nocapture noundef readonly, ...) #1

attributes #0 = { noinline nounwind optnone uwtable }
attributes #1 = { nofree nounwind }
