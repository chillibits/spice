; ModuleID = 'source.spice'
source_filename = "source.spice"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-w64-windows-gnu"

%struct.Vector = type { i1, ptr }

@printf.str.0 = private unnamed_addr constant [19 x i8] c"Destructor called!\00", align 1
@anon.string.0 = private unnamed_addr constant [5 x i8] c"Test\00", align 1
@anon.struct.0 = private unnamed_addr constant %struct.Vector { i1 true, ptr @anon.string.0 }
@printf.str.1 = private unnamed_addr constant [16 x i8] c"Fields: %d, %s\0A\00", align 1

define private void @_ZN6Vector4dtorEv(ptr noundef nonnull %0) {
  %this = alloca ptr, align 8
  store ptr %0, ptr %this, align 8
  %2 = call i32 (ptr, ...) @printf(ptr noundef @printf.str.0)
  ret void
}

; Function Attrs: nofree nounwind
declare noundef i32 @printf(ptr nocapture noundef readonly, ...) #0

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #1 {
  %result = alloca i32, align 4
  %vec = alloca %struct.Vector, align 8
  store i32 0, ptr %result, align 4
  call void @llvm.memcpy.p0.p0.i64(ptr %vec, ptr @anon.struct.0, i64 16, i1 false)
  store %struct.Vector { i1 true, ptr @anon.string.0 }, ptr %vec, align 8
  %field1_addr = getelementptr inbounds %struct.Vector, ptr %vec, i32 0, i32 0
  %1 = load i1, ptr %field1_addr, align 1
  %2 = zext i1 %1 to i32
  %field2_addr = getelementptr inbounds %struct.Vector, ptr %vec, i32 0, i32 1
  %3 = load ptr, ptr %field2_addr, align 8
  %4 = call i32 (ptr, ...) @printf(ptr noundef @printf.str.1, i32 %2, ptr %3)
  call void @_ZN6Vector4dtorEv(ptr %vec)
  %5 = load i32, ptr %result, align 4
  ret i32 %5
}

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: readwrite)
declare void @llvm.memcpy.p0.p0.i64(ptr noalias nocapture writeonly, ptr noalias nocapture readonly, i64, i1 immarg) #2

attributes #0 = { nofree nounwind }
attributes #1 = { noinline nounwind optnone uwtable }
attributes #2 = { nocallback nofree nounwind willreturn memory(argmem: readwrite) }
