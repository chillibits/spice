; ModuleID = 'source.spice'
source_filename = "source.spice"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-w64-windows-gnu"

@printf.str.1 = private unnamed_addr constant [5 x i8] c" %d \00", align 1
@printf.str.3 = private unnamed_addr constant [24 x i8] c"Solution does not exist\00", align 1
@str = private unnamed_addr constant [52 x i8] c"Solution Exists:\0AFollowing are the assigned colors:\00", align 1

; Function Attrs: nofree nounwind
declare noundef i32 @printf(ptr nocapture noundef readonly, ...) local_unnamed_addr #0

; Function Attrs: nofree nounwind
define private fastcc i1 @_f__void__bool__graphColoring__boolarrayarray_int_int_intarray(i32 %0, ptr nocapture %1) unnamed_addr #0 {
  %3 = icmp eq i32 %0, 4
  br i1 %3, label %if.then.L27, label %for.head.L36.preheader

for.head.L36.preheader:                           ; preds = %2
  %4 = sext i32 %0 to i64
  %5 = getelementptr inbounds i32, ptr %1, i64 %4
  %6 = add i32 %0, 1
  store i32 1, ptr %5, align 4
  %7 = tail call fastcc i1 @_f__void__bool__graphColoring__boolarrayarray_int_int_intarray(i32 %6, ptr %1)
  br i1 %7, label %common.ret, label %if.exit.L40

if.then.L27:                                      ; preds = %2
  %8 = getelementptr inbounds i32, ptr %1, i64 1
  %9 = load i32, ptr %1, align 4
  %10 = load i32, ptr %8, align 4
  %11 = icmp eq i32 %10, %9
  br i1 %11, label %common.ret, label %land.1.L12C16.121.i

land.1.L12C16.121.i:                              ; preds = %if.then.L27
  %12 = getelementptr inbounds i32, ptr %1, i64 2
  %13 = load i32, ptr %12, align 4
  %14 = icmp eq i32 %13, %9
  br i1 %14, label %common.ret, label %land.1.L12C16.224.i

land.1.L12C16.224.i:                              ; preds = %land.1.L12C16.121.i
  %15 = getelementptr inbounds i32, ptr %1, i64 3
  %16 = load i32, ptr %15, align 4
  %17 = icmp eq i32 %16, %9
  %18 = icmp eq i32 %13, %10
  %or.cond = select i1 %17, i1 true, i1 %18
  %19 = icmp eq i32 %16, %13
  %or.cond1 = select i1 %or.cond, i1 true, i1 %19
  br i1 %or.cond1, label %common.ret, label %if.then.L29

common.ret:                                       ; preds = %for.head.L36.preheader, %if.exit.L40, %if.exit.L40.1, %if.exit.L40.2, %land.1.L12C16.224.i, %land.1.L12C16.121.i, %if.then.L27, %if.then.L29
  %common.ret.op = phi i1 [ true, %if.then.L29 ], [ false, %if.then.L27 ], [ false, %land.1.L12C16.121.i ], [ false, %land.1.L12C16.224.i ], [ true, %for.head.L36.preheader ], [ true, %if.exit.L40 ], [ true, %if.exit.L40.1 ], [ false, %if.exit.L40.2 ]
  ret i1 %common.ret.op

if.then.L29:                                      ; preds = %land.1.L12C16.224.i
  %puts.i = tail call i32 @puts(ptr nonnull @str)
  %20 = load i32, ptr %1, align 4
  %21 = tail call i32 (ptr, ...) @printf(ptr noundef nonnull @printf.str.1, i32 %20)
  %22 = load i32, ptr %8, align 4
  %23 = tail call i32 (ptr, ...) @printf(ptr noundef nonnull @printf.str.1, i32 %22)
  %24 = load i32, ptr %12, align 4
  %25 = tail call i32 (ptr, ...) @printf(ptr noundef nonnull @printf.str.1, i32 %24)
  %26 = load i32, ptr %15, align 4
  %27 = tail call i32 (ptr, ...) @printf(ptr noundef nonnull @printf.str.1, i32 %26)
  %putchar.i = tail call i32 @putchar(i32 10)
  br label %common.ret

if.exit.L40:                                      ; preds = %for.head.L36.preheader
  store i32 2, ptr %5, align 4
  %28 = tail call fastcc i1 @_f__void__bool__graphColoring__boolarrayarray_int_int_intarray(i32 %6, ptr nonnull %1)
  br i1 %28, label %common.ret, label %if.exit.L40.1

if.exit.L40.1:                                    ; preds = %if.exit.L40
  store i32 3, ptr %5, align 4
  %29 = tail call fastcc i1 @_f__void__bool__graphColoring__boolarrayarray_int_int_intarray(i32 %6, ptr nonnull %1)
  br i1 %29, label %common.ret, label %if.exit.L40.2

if.exit.L40.2:                                    ; preds = %if.exit.L40.1
  store i32 0, ptr %5, align 4
  br label %common.ret
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() local_unnamed_addr #1 {
for.exit.L69:
  %color = alloca [4 x i32], align 4
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 4 dereferenceable(16) %color, i8 0, i64 16, i1 false)
  %0 = call fastcc i1 @_f__void__bool__graphColoring__boolarrayarray_int_int_intarray(i32 0, ptr nonnull %color) #3
  br i1 %0, label %if.exit.L73, label %if.then.L73

if.then.L73:                                      ; preds = %for.exit.L69
  %1 = tail call i32 (ptr, ...) @printf(ptr noundef nonnull @printf.str.3)
  br label %if.exit.L73

if.exit.L73:                                      ; preds = %if.then.L73, %for.exit.L69
  ret i32 0
}

; Function Attrs: nofree nounwind
declare noundef i32 @puts(ptr nocapture noundef readonly) local_unnamed_addr #0

; Function Attrs: nofree nounwind
declare noundef i32 @putchar(i32 noundef) local_unnamed_addr #0

; Function Attrs: argmemonly nocallback nofree nounwind willreturn writeonly
declare void @llvm.memset.p0.i64(ptr nocapture writeonly, i8, i64, i1 immarg) #2

attributes #0 = { nofree nounwind }
attributes #1 = { noinline nounwind optnone uwtable }
attributes #2 = { argmemonly nocallback nofree nounwind willreturn writeonly }
attributes #3 = { nounwind }
