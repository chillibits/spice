; ModuleID = 'source.spice'
source_filename = "source.spice"

%struct.Test = type { i32, ptr }

@0 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@printf.str.0 = private unnamed_addr constant [9 x i8] c"Int: %d\0A\00", align 1
@printf.str.1 = private unnamed_addr constant [12 x i8] c"String: %s\0A\00", align 1

define private void @_ZN4Test4ctorEv(ptr noundef nonnull align 8 dereferenceable(16) %0) {
  %this = alloca ptr, align 8
  store ptr %0, ptr %this, align 8
  %2 = load ptr, ptr %this, align 8
  %3 = getelementptr inbounds %struct.Test, ptr %2, i32 0, i32 0
  store i32 12, ptr %3, align 4
  %4 = getelementptr inbounds %struct.Test, ptr %2, i32 0, i32 1
  store ptr @0, ptr %4, align 8
  %5 = load ptr, ptr %this, align 8
  %i_addr = getelementptr inbounds %struct.Test, ptr %5, i32 0, i32 0
  store i32 14, ptr %i_addr, align 4
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
  %result = alloca i32, align 4
  %t = alloca %struct.Test, align 8
  store i32 0, ptr %result, align 4
  call void @_ZN4Test4ctorEv(ptr noundef nonnull align 8 dereferenceable(16) %t)
  %i_addr = getelementptr inbounds %struct.Test, ptr %t, i32 0, i32 0
  %1 = load i32, ptr %i_addr, align 4
  %2 = call i32 (ptr, ...) @printf(ptr noundef @printf.str.0, i32 %1)
  %s_addr = getelementptr inbounds %struct.Test, ptr %t, i32 0, i32 1
  %3 = load ptr, ptr %s_addr, align 8
  %4 = call i32 (ptr, ...) @printf(ptr noundef @printf.str.1, ptr %3)
  %5 = load i32, ptr %result, align 4
  ret i32 %5
}

; Function Attrs: nofree nounwind
declare noundef i32 @printf(ptr nocapture noundef readonly, ...) #1

attributes #0 = { noinline nounwind optnone uwtable }
attributes #1 = { nofree nounwind }
