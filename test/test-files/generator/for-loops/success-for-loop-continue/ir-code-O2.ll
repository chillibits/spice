; ModuleID = 'source.spice'
source_filename = "source.spice"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-w64-windows-gnu"

@0 = private unnamed_addr constant [13 x i8] c"Loop run %d\0A\00", align 1
@1 = private unnamed_addr constant [19 x i8] c"Inner loop run %d\0A\00", align 1
@2 = private unnamed_addr constant [5 x i8] c"End.\00", align 1

; Function Attrs: nofree nounwind
define i32 @main() local_unnamed_addr #0 {
for.l5.preheader.3:
  %0 = tail call i32 (i8*, ...) @printf(i8* nonnull dereferenceable(1) getelementptr inbounds ([13 x i8], [13 x i8]* @0, i64 0, i64 0), i32 0)
  %1 = tail call i32 (i8*, ...) @printf(i8* nonnull dereferenceable(1) getelementptr inbounds ([13 x i8], [13 x i8]* @0, i64 0, i64 0), i32 2)
  %2 = tail call i32 (i8*, ...) @printf(i8* nonnull dereferenceable(1) getelementptr inbounds ([13 x i8], [13 x i8]* @0, i64 0, i64 0), i32 4)
  %3 = tail call i32 (i8*, ...) @printf(i8* nonnull dereferenceable(1) getelementptr inbounds ([13 x i8], [13 x i8]* @0, i64 0, i64 0), i32 6)
  br label %for.l5.3

for.l5.3:                                         ; preds = %for.l5.3, %for.l5.preheader.3
  %subCounter.010.3 = phi i32 [ %5, %for.l5.3 ], [ 100, %for.l5.preheader.3 ]
  %4 = tail call i32 (i8*, ...) @printf(i8* nonnull dereferenceable(1) getelementptr inbounds ([19 x i8], [19 x i8]* @1, i64 0, i64 0), i32 %subCounter.010.3)
  %.not = icmp eq i32 %subCounter.010.3, 11
  %5 = add nsw i32 %subCounter.010.3, -1
  br i1 %.not, label %for.l5.preheader.4, label %for.l5.3

for.l5.preheader.4:                               ; preds = %for.l5.3
  %6 = tail call i32 (i8*, ...) @printf(i8* nonnull dereferenceable(1) getelementptr inbounds ([13 x i8], [13 x i8]* @0, i64 0, i64 0), i32 8)
  br label %for.l5.4

for.l5.4:                                         ; preds = %for.l5.4, %for.l5.preheader.4
  %subCounter.010.4 = phi i32 [ %8, %for.l5.4 ], [ 100, %for.l5.preheader.4 ]
  %7 = tail call i32 (i8*, ...) @printf(i8* nonnull dereferenceable(1) getelementptr inbounds ([19 x i8], [19 x i8]* @1, i64 0, i64 0), i32 %subCounter.010.4)
  %.not12 = icmp eq i32 %subCounter.010.4, 11
  %8 = add nsw i32 %subCounter.010.4, -1
  br i1 %.not12, label %for.inc.l2.4, label %for.l5.4

for.inc.l2.4:                                     ; preds = %for.l5.4
  %9 = tail call i32 (i8*, ...) @printf(i8* nonnull dereferenceable(1) getelementptr inbounds ([5 x i8], [5 x i8]* @2, i64 0, i64 0))
  ret i32 0
}

; Function Attrs: nofree nounwind
declare noundef i32 @printf(i8* nocapture noundef readonly, ...) local_unnamed_addr #0

attributes #0 = { nofree nounwind }
