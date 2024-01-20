; ModuleID = 'source.spice'
source_filename = "source.spice"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-w64-windows-gnu"

%struct.Vector = type { ptr, i64, i64 }
%struct.VectorIterator = type { %interface.IIterator, ptr, i64 }
%interface.IIterator = type { ptr }
%struct.Pair = type { i64, ptr }

@anon.string.0 = private unnamed_addr constant [68 x i8] c"Assertion failed: Condition 'vi.getSize() == 3' evaluated to false.\00", align 1
@anon.string.1 = private unnamed_addr constant [63 x i8] c"Assertion failed: Condition 'it.isValid()' evaluated to false.\00", align 1
@anon.string.2 = private unnamed_addr constant [66 x i8] c"Assertion failed: Condition 'it.get() == 123' evaluated to false.\00", align 1
@anon.string.3 = private unnamed_addr constant [66 x i8] c"Assertion failed: Condition 'it.get() == 123' evaluated to false.\00", align 1
@anon.string.4 = private unnamed_addr constant [67 x i8] c"Assertion failed: Condition 'it.get() == 4321' evaluated to false.\00", align 1
@anon.string.5 = private unnamed_addr constant [63 x i8] c"Assertion failed: Condition 'it.isValid()' evaluated to false.\00", align 1
@anon.string.6 = private unnamed_addr constant [71 x i8] c"Assertion failed: Condition 'pair.getFirst() == 2' evaluated to false.\00", align 1
@anon.string.7 = private unnamed_addr constant [75 x i8] c"Assertion failed: Condition 'pair.getSecond() == 9876' evaluated to false.\00", align 1
@anon.string.8 = private unnamed_addr constant [64 x i8] c"Assertion failed: Condition '!it.isValid()' evaluated to false.\00", align 1
@anon.string.9 = private unnamed_addr constant [63 x i8] c"Assertion failed: Condition 'it.isValid()' evaluated to false.\00", align 1
@anon.string.10 = private unnamed_addr constant [66 x i8] c"Assertion failed: Condition 'it.get() == 123' evaluated to false.\00", align 1
@anon.string.11 = private unnamed_addr constant [63 x i8] c"Assertion failed: Condition 'it.isValid()' evaluated to false.\00", align 1
@anon.string.12 = private unnamed_addr constant [67 x i8] c"Assertion failed: Condition 'it.get() == 4321' evaluated to false.\00", align 1
@anon.string.13 = private unnamed_addr constant [66 x i8] c"Assertion failed: Condition 'it.get() == 123' evaluated to false.\00", align 1
@anon.string.14 = private unnamed_addr constant [66 x i8] c"Assertion failed: Condition 'it.get() == -99' evaluated to false.\00", align 1
@anon.string.15 = private unnamed_addr constant [64 x i8] c"Assertion failed: Condition '!it.isValid()' evaluated to false.\00", align 1
@anon.string.16 = private unnamed_addr constant [67 x i8] c"Assertion failed: Condition 'vi.get(0) == 123' evaluated to false.\00", align 1
@anon.string.17 = private unnamed_addr constant [68 x i8] c"Assertion failed: Condition 'vi.get(1) == 4321' evaluated to false.\00", align 1
@anon.string.18 = private unnamed_addr constant [68 x i8] c"Assertion failed: Condition 'vi.get(2) == 9876' evaluated to false.\00", align 1
@anon.string.19 = private unnamed_addr constant [67 x i8] c"Assertion failed: Condition 'vi.get(0) == 124' evaluated to false.\00", align 1
@anon.string.20 = private unnamed_addr constant [68 x i8] c"Assertion failed: Condition 'vi.get(1) == 4322' evaluated to false.\00", align 1
@anon.string.21 = private unnamed_addr constant [68 x i8] c"Assertion failed: Condition 'vi.get(2) == 9877' evaluated to false.\00", align 1
@anon.string.22 = private unnamed_addr constant [67 x i8] c"Assertion failed: Condition 'vi.get(0) == 124' evaluated to false.\00", align 1
@anon.string.23 = private unnamed_addr constant [68 x i8] c"Assertion failed: Condition 'vi.get(1) == 4323' evaluated to false.\00", align 1
@anon.string.24 = private unnamed_addr constant [68 x i8] c"Assertion failed: Condition 'vi.get(2) == 9879' evaluated to false.\00", align 1
@printf.str.0 = private unnamed_addr constant [24 x i8] c"All assertions passed!\0A\00", align 1, !dbg !0

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main(i32 %0, ptr %1) #0 !dbg !15 {
  %result = alloca i32, align 4
  %_argc = alloca i32, align 4
  %_argv = alloca ptr, align 8
  %vi = alloca %struct.Vector, align 8
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  %it = alloca %struct.VectorIterator, align 8
  %pair = alloca %struct.Pair, align 8
  %6 = alloca i1, align 1
  %7 = alloca i32, align 4
  %8 = alloca i32, align 4
  %9 = alloca i1, align 1
  %10 = alloca %struct.VectorIterator, align 8
  %item = alloca i32, align 4
  %11 = alloca %struct.VectorIterator, align 8
  %item1 = alloca ptr, align 8
  %12 = alloca ptr, align 8
  %13 = alloca %struct.VectorIterator, align 8
  %idx = alloca i64, align 8
  %item2 = alloca ptr, align 8
  %pair_addr = alloca %struct.Pair, align 8
  %14 = alloca ptr, align 8
  call void @llvm.dbg.declare(metadata ptr %result, metadata !23, metadata !DIExpression()), !dbg !24
  store i32 0, ptr %result, align 4, !dbg !24
  call void @llvm.dbg.declare(metadata ptr %_argc, metadata !25, metadata !DIExpression()), !dbg !24
  store i32 %0, ptr %_argc, align 4, !dbg !24
  call void @llvm.dbg.declare(metadata ptr %_argv, metadata !26, metadata !DIExpression()), !dbg !24
  store ptr %1, ptr %_argv, align 8, !dbg !24
  call void @llvm.dbg.declare(metadata ptr %vi, metadata !27, metadata !DIExpression()), !dbg !36
  call void @_ZN6VectorIiE4ctorEv(ptr noundef nonnull align 8 dereferenceable(24) %vi), !dbg !37
  store i32 123, ptr %3, align 4, !dbg !38
  call void @_ZN6VectorIiE8pushBackERKi(ptr noundef nonnull align 8 dereferenceable(24) %vi, ptr %3), !dbg !38
  store i32 4321, ptr %4, align 4, !dbg !39
  call void @_ZN6VectorIiE8pushBackERKi(ptr noundef nonnull align 8 dereferenceable(24) %vi, ptr %4), !dbg !39
  store i32 9876, ptr %5, align 4, !dbg !40
  call void @_ZN6VectorIiE8pushBackERKi(ptr noundef nonnull align 8 dereferenceable(24) %vi, ptr %5), !dbg !40
  %15 = call i64 @_ZN6VectorIiE7getSizeEv(ptr noundef nonnull align 8 dereferenceable(24) %vi), !dbg !41
  %16 = icmp eq i64 %15, 3, !dbg !42
  br i1 %16, label %assert.exit.L12, label %assert.then.L12, !dbg !42, !prof !43

assert.then.L12:                                  ; preds = %2
  %17 = call i32 (ptr, ...) @printf(ptr @anon.string.0), !dbg !42
  call void @exit(i32 1), !dbg !42
  unreachable, !dbg !42

assert.exit.L12:                                  ; preds = %2
  %18 = call %struct.VectorIterator @_Z7iterateR6VectorIiE(ptr %vi), !dbg !44
  call void @llvm.dbg.declare(metadata ptr %it, metadata !45, metadata !DIExpression()), !dbg !52
  store %struct.VectorIterator %18, ptr %it, align 8, !dbg !44
  %19 = call i1 @_ZN14VectorIteratorIiE7isValidEv(ptr noundef nonnull align 8 dereferenceable(24) %it), !dbg !53
  br i1 %19, label %assert.exit.L16, label %assert.then.L16, !dbg !53, !prof !43

assert.then.L16:                                  ; preds = %assert.exit.L12
  %20 = call i32 (ptr, ...) @printf(ptr @anon.string.1), !dbg !53
  call void @exit(i32 1), !dbg !53
  unreachable, !dbg !53

assert.exit.L16:                                  ; preds = %assert.exit.L12
  %21 = call ptr @_ZN14VectorIteratorIiE3getEv(ptr noundef nonnull align 8 dereferenceable(24) %it), !dbg !54
  %22 = load i32, ptr %21, align 4, !dbg !55
  %23 = icmp eq i32 %22, 123, !dbg !55
  br i1 %23, label %assert.exit.L17, label %assert.then.L17, !dbg !55, !prof !43

assert.then.L17:                                  ; preds = %assert.exit.L16
  %24 = call i32 (ptr, ...) @printf(ptr @anon.string.2), !dbg !55
  call void @exit(i32 1), !dbg !55
  unreachable, !dbg !55

assert.exit.L17:                                  ; preds = %assert.exit.L16
  %25 = call ptr @_ZN14VectorIteratorIiE3getEv(ptr noundef nonnull align 8 dereferenceable(24) %it), !dbg !56
  %26 = load i32, ptr %25, align 4, !dbg !57
  %27 = icmp eq i32 %26, 123, !dbg !57
  br i1 %27, label %assert.exit.L18, label %assert.then.L18, !dbg !57, !prof !43

assert.then.L18:                                  ; preds = %assert.exit.L17
  %28 = call i32 (ptr, ...) @printf(ptr @anon.string.3), !dbg !57
  call void @exit(i32 1), !dbg !57
  unreachable, !dbg !57

assert.exit.L18:                                  ; preds = %assert.exit.L17
  call void @_ZN14VectorIteratorIiE4nextEv(ptr noundef nonnull align 8 dereferenceable(24) %it), !dbg !58
  %29 = call ptr @_ZN14VectorIteratorIiE3getEv(ptr noundef nonnull align 8 dereferenceable(24) %it), !dbg !59
  %30 = load i32, ptr %29, align 4, !dbg !60
  %31 = icmp eq i32 %30, 4321, !dbg !60
  br i1 %31, label %assert.exit.L20, label %assert.then.L20, !dbg !60, !prof !43

assert.then.L20:                                  ; preds = %assert.exit.L18
  %32 = call i32 (ptr, ...) @printf(ptr @anon.string.4), !dbg !60
  call void @exit(i32 1), !dbg !60
  unreachable, !dbg !60

assert.exit.L20:                                  ; preds = %assert.exit.L18
  %33 = call i1 @_ZN14VectorIteratorIiE7isValidEv(ptr noundef nonnull align 8 dereferenceable(24) %it), !dbg !61
  br i1 %33, label %assert.exit.L21, label %assert.then.L21, !dbg !61, !prof !43

assert.then.L21:                                  ; preds = %assert.exit.L20
  %34 = call i32 (ptr, ...) @printf(ptr @anon.string.5), !dbg !61
  call void @exit(i32 1), !dbg !61
  unreachable, !dbg !61

assert.exit.L21:                                  ; preds = %assert.exit.L20
  call void @_ZN14VectorIteratorIiE4nextEv(ptr noundef nonnull align 8 dereferenceable(24) %it), !dbg !62
  %35 = call %struct.Pair @_ZN14VectorIteratorIiE6getIdxEv(ptr noundef nonnull align 8 dereferenceable(24) %it), !dbg !63
  call void @llvm.dbg.declare(metadata ptr %pair, metadata !64, metadata !DIExpression()), !dbg !71
  store %struct.Pair %35, ptr %pair, align 8, !dbg !63
  %36 = call ptr @_ZN4PairImRiE8getFirstEv(ptr noundef nonnull align 8 dereferenceable(16) %pair), !dbg !72
  %37 = load i64, ptr %36, align 8, !dbg !73
  %38 = icmp eq i64 %37, 2, !dbg !73
  br i1 %38, label %assert.exit.L24, label %assert.then.L24, !dbg !73, !prof !43

assert.then.L24:                                  ; preds = %assert.exit.L21
  %39 = call i32 (ptr, ...) @printf(ptr @anon.string.6), !dbg !73
  call void @exit(i32 1), !dbg !73
  unreachable, !dbg !73

assert.exit.L24:                                  ; preds = %assert.exit.L21
  %40 = call ptr @_ZN4PairImRiE9getSecondEv(ptr noundef nonnull align 8 dereferenceable(16) %pair), !dbg !74
  %41 = load i32, ptr %40, align 4, !dbg !75
  %42 = icmp eq i32 %41, 9876, !dbg !75
  br i1 %42, label %assert.exit.L25, label %assert.then.L25, !dbg !75, !prof !43

assert.then.L25:                                  ; preds = %assert.exit.L24
  %43 = call i32 (ptr, ...) @printf(ptr @anon.string.7), !dbg !75
  call void @exit(i32 1), !dbg !75
  unreachable, !dbg !75

assert.exit.L25:                                  ; preds = %assert.exit.L24
  call void @_ZN14VectorIteratorIiE4nextEv(ptr noundef nonnull align 8 dereferenceable(24) %it), !dbg !76
  %44 = call i1 @_ZN14VectorIteratorIiE7isValidEv(ptr noundef nonnull align 8 dereferenceable(24) %it), !dbg !77
  %45 = xor i1 %44, true, !dbg !77
  store i1 %45, ptr %6, align 1, !dbg !77
  br i1 %45, label %assert.exit.L27, label %assert.then.L27, !dbg !77, !prof !43

assert.then.L27:                                  ; preds = %assert.exit.L25
  %46 = call i32 (ptr, ...) @printf(ptr @anon.string.8), !dbg !77
  call void @exit(i32 1), !dbg !77
  unreachable, !dbg !77

assert.exit.L27:                                  ; preds = %assert.exit.L25
  store i32 321, ptr %7, align 4, !dbg !78
  call void @_ZN6VectorIiE8pushBackERKi(ptr noundef nonnull align 8 dereferenceable(24) %vi, ptr %7), !dbg !78
  store i32 -99, ptr %8, align 4, !dbg !79
  call void @_ZN6VectorIiE8pushBackERKi(ptr noundef nonnull align 8 dereferenceable(24) %vi, ptr %8), !dbg !79
  %47 = call i1 @_ZN14VectorIteratorIiE7isValidEv(ptr noundef nonnull align 8 dereferenceable(24) %it), !dbg !80
  br i1 %47, label %assert.exit.L32, label %assert.then.L32, !dbg !80, !prof !43

assert.then.L32:                                  ; preds = %assert.exit.L27
  %48 = call i32 (ptr, ...) @printf(ptr @anon.string.9), !dbg !80
  call void @exit(i32 1), !dbg !80
  unreachable, !dbg !80

assert.exit.L32:                                  ; preds = %assert.exit.L27
  call void @_Z13op.minusequalR14VectorIteratorIiEi(ptr %it, i32 3), !dbg !81
  %49 = call ptr @_ZN14VectorIteratorIiE3getEv(ptr noundef nonnull align 8 dereferenceable(24) %it), !dbg !82
  %50 = load i32, ptr %49, align 4, !dbg !83
  %51 = icmp eq i32 %50, 123, !dbg !83
  br i1 %51, label %assert.exit.L36, label %assert.then.L36, !dbg !83, !prof !43

assert.then.L36:                                  ; preds = %assert.exit.L32
  %52 = call i32 (ptr, ...) @printf(ptr @anon.string.10), !dbg !83
  call void @exit(i32 1), !dbg !83
  unreachable, !dbg !83

assert.exit.L36:                                  ; preds = %assert.exit.L32
  %53 = call i1 @_ZN14VectorIteratorIiE7isValidEv(ptr noundef nonnull align 8 dereferenceable(24) %it), !dbg !84
  br i1 %53, label %assert.exit.L37, label %assert.then.L37, !dbg !84, !prof !43

assert.then.L37:                                  ; preds = %assert.exit.L36
  %54 = call i32 (ptr, ...) @printf(ptr @anon.string.11), !dbg !84
  call void @exit(i32 1), !dbg !84
  unreachable, !dbg !84

assert.exit.L37:                                  ; preds = %assert.exit.L36
  %55 = load %struct.VectorIterator, ptr %it, align 8, !dbg !85
  call void @_Z16op.plusplus.postR14VectorIteratorIiE(ptr %it), !dbg !85
  %56 = call ptr @_ZN14VectorIteratorIiE3getEv(ptr noundef nonnull align 8 dereferenceable(24) %it), !dbg !86
  %57 = load i32, ptr %56, align 4, !dbg !87
  %58 = icmp eq i32 %57, 4321, !dbg !87
  br i1 %58, label %assert.exit.L39, label %assert.then.L39, !dbg !87, !prof !43

assert.then.L39:                                  ; preds = %assert.exit.L37
  %59 = call i32 (ptr, ...) @printf(ptr @anon.string.12), !dbg !87
  call void @exit(i32 1), !dbg !87
  unreachable, !dbg !87

assert.exit.L39:                                  ; preds = %assert.exit.L37
  %60 = load %struct.VectorIterator, ptr %it, align 8, !dbg !88
  call void @_Z18op.minusminus.postR14VectorIteratorIiE(ptr %it), !dbg !88
  %61 = call ptr @_ZN14VectorIteratorIiE3getEv(ptr noundef nonnull align 8 dereferenceable(24) %it), !dbg !89
  %62 = load i32, ptr %61, align 4, !dbg !90
  %63 = icmp eq i32 %62, 123, !dbg !90
  br i1 %63, label %assert.exit.L41, label %assert.then.L41, !dbg !90, !prof !43

assert.then.L41:                                  ; preds = %assert.exit.L39
  %64 = call i32 (ptr, ...) @printf(ptr @anon.string.13), !dbg !90
  call void @exit(i32 1), !dbg !90
  unreachable, !dbg !90

assert.exit.L41:                                  ; preds = %assert.exit.L39
  call void @_Z12op.plusequalR14VectorIteratorIiEi(ptr %it, i32 4), !dbg !91
  %65 = call ptr @_ZN14VectorIteratorIiE3getEv(ptr noundef nonnull align 8 dereferenceable(24) %it), !dbg !92
  %66 = load i32, ptr %65, align 4, !dbg !93
  %67 = icmp eq i32 %66, -99, !dbg !93
  br i1 %67, label %assert.exit.L43, label %assert.then.L43, !dbg !93, !prof !43

assert.then.L43:                                  ; preds = %assert.exit.L41
  %68 = call i32 (ptr, ...) @printf(ptr @anon.string.14), !dbg !93
  call void @exit(i32 1), !dbg !93
  unreachable, !dbg !93

assert.exit.L43:                                  ; preds = %assert.exit.L41
  call void @_ZN14VectorIteratorIiE4nextEv(ptr noundef nonnull align 8 dereferenceable(24) %it), !dbg !94
  %69 = call i1 @_ZN14VectorIteratorIiE7isValidEv(ptr noundef nonnull align 8 dereferenceable(24) %it), !dbg !95
  %70 = xor i1 %69, true, !dbg !95
  store i1 %70, ptr %9, align 1, !dbg !95
  br i1 %70, label %assert.exit.L45, label %assert.then.L45, !dbg !95, !prof !43

assert.then.L45:                                  ; preds = %assert.exit.L43
  %71 = call i32 (ptr, ...) @printf(ptr @anon.string.15), !dbg !95
  call void @exit(i32 1), !dbg !95
  unreachable, !dbg !95

assert.exit.L45:                                  ; preds = %assert.exit.L43
  %72 = call %struct.VectorIterator @_Z7iterateR6VectorIiE(ptr %vi), !dbg !96
  call void @llvm.dbg.declare(metadata ptr %item, metadata !98, metadata !DIExpression()), !dbg !99
  store %struct.VectorIterator %72, ptr %10, align 8, !dbg !96
  br label %foreach.head.L48, !dbg !99

foreach.head.L48:                                 ; preds = %foreach.tail.L48, %assert.exit.L45
  %73 = call i1 @_ZN14VectorIteratorIiE7isValidEv(ptr %10), !dbg !99
  br i1 %73, label %foreach.body.L48, label %foreach.exit.L48, !dbg !99

foreach.body.L48:                                 ; preds = %foreach.head.L48
  %74 = call ptr @_ZN14VectorIteratorIiE3getEv(ptr %10), !dbg !99
  %75 = load i32, ptr %74, align 4, !dbg !99
  store i32 %75, ptr %item, align 4, !dbg !99
  %76 = load i32, ptr %item, align 4, !dbg !100
  %77 = add nsw i32 %76, 1, !dbg !100
  store i32 %77, ptr %item, align 4, !dbg !100
  br label %foreach.tail.L48, !dbg !100

foreach.tail.L48:                                 ; preds = %foreach.body.L48
  call void @_ZN14VectorIteratorIiE4nextEv(ptr %10), !dbg !100
  br label %foreach.head.L48, !dbg !100

foreach.exit.L48:                                 ; preds = %foreach.head.L48
  %78 = call ptr @_ZN6VectorIiE3getEj(ptr noundef nonnull align 8 dereferenceable(24) %vi, i32 0), !dbg !101
  %79 = load i32, ptr %78, align 4, !dbg !102
  %80 = icmp eq i32 %79, 123, !dbg !102
  br i1 %80, label %assert.exit.L51, label %assert.then.L51, !dbg !102, !prof !43

assert.then.L51:                                  ; preds = %foreach.exit.L48
  %81 = call i32 (ptr, ...) @printf(ptr @anon.string.16), !dbg !102
  call void @exit(i32 1), !dbg !102
  unreachable, !dbg !102

assert.exit.L51:                                  ; preds = %foreach.exit.L48
  %82 = call ptr @_ZN6VectorIiE3getEj(ptr noundef nonnull align 8 dereferenceable(24) %vi, i32 1), !dbg !103
  %83 = load i32, ptr %82, align 4, !dbg !104
  %84 = icmp eq i32 %83, 4321, !dbg !104
  br i1 %84, label %assert.exit.L52, label %assert.then.L52, !dbg !104, !prof !43

assert.then.L52:                                  ; preds = %assert.exit.L51
  %85 = call i32 (ptr, ...) @printf(ptr @anon.string.17), !dbg !104
  call void @exit(i32 1), !dbg !104
  unreachable, !dbg !104

assert.exit.L52:                                  ; preds = %assert.exit.L51
  %86 = call ptr @_ZN6VectorIiE3getEj(ptr noundef nonnull align 8 dereferenceable(24) %vi, i32 2), !dbg !105
  %87 = load i32, ptr %86, align 4, !dbg !106
  %88 = icmp eq i32 %87, 9876, !dbg !106
  br i1 %88, label %assert.exit.L53, label %assert.then.L53, !dbg !106, !prof !43

assert.then.L53:                                  ; preds = %assert.exit.L52
  %89 = call i32 (ptr, ...) @printf(ptr @anon.string.18), !dbg !106
  call void @exit(i32 1), !dbg !106
  unreachable, !dbg !106

assert.exit.L53:                                  ; preds = %assert.exit.L52
  %90 = call %struct.VectorIterator @_Z7iterateR6VectorIiE(ptr %vi), !dbg !107
  call void @llvm.dbg.declare(metadata ptr %item1, metadata !109, metadata !DIExpression()), !dbg !110
  store %struct.VectorIterator %90, ptr %11, align 8, !dbg !107
  br label %foreach.head.L56, !dbg !110

foreach.head.L56:                                 ; preds = %foreach.tail.L56, %assert.exit.L53
  %91 = call i1 @_ZN14VectorIteratorIiE7isValidEv(ptr %11), !dbg !110
  br i1 %91, label %foreach.body.L56, label %foreach.exit.L56, !dbg !110

foreach.body.L56:                                 ; preds = %foreach.head.L56
  %92 = call ptr @_ZN14VectorIteratorIiE3getEv(ptr %11), !dbg !110
  store ptr %92, ptr %12, align 8, !dbg !110
  %93 = load ptr, ptr %12, align 8, !dbg !111
  %94 = load i32, ptr %93, align 4, !dbg !111
  %95 = add nsw i32 %94, 1, !dbg !111
  store i32 %95, ptr %93, align 4, !dbg !111
  br label %foreach.tail.L56, !dbg !111

foreach.tail.L56:                                 ; preds = %foreach.body.L56
  call void @_ZN14VectorIteratorIiE4nextEv(ptr %11), !dbg !111
  br label %foreach.head.L56, !dbg !111

foreach.exit.L56:                                 ; preds = %foreach.head.L56
  %96 = call ptr @_ZN6VectorIiE3getEj(ptr noundef nonnull align 8 dereferenceable(24) %vi, i32 0), !dbg !112
  %97 = load i32, ptr %96, align 4, !dbg !113
  %98 = icmp eq i32 %97, 124, !dbg !113
  br i1 %98, label %assert.exit.L59, label %assert.then.L59, !dbg !113, !prof !43

assert.then.L59:                                  ; preds = %foreach.exit.L56
  %99 = call i32 (ptr, ...) @printf(ptr @anon.string.19), !dbg !113
  call void @exit(i32 1), !dbg !113
  unreachable, !dbg !113

assert.exit.L59:                                  ; preds = %foreach.exit.L56
  %100 = call ptr @_ZN6VectorIiE3getEj(ptr noundef nonnull align 8 dereferenceable(24) %vi, i32 1), !dbg !114
  %101 = load i32, ptr %100, align 4, !dbg !115
  %102 = icmp eq i32 %101, 4322, !dbg !115
  br i1 %102, label %assert.exit.L60, label %assert.then.L60, !dbg !115, !prof !43

assert.then.L60:                                  ; preds = %assert.exit.L59
  %103 = call i32 (ptr, ...) @printf(ptr @anon.string.20), !dbg !115
  call void @exit(i32 1), !dbg !115
  unreachable, !dbg !115

assert.exit.L60:                                  ; preds = %assert.exit.L59
  %104 = call ptr @_ZN6VectorIiE3getEj(ptr noundef nonnull align 8 dereferenceable(24) %vi, i32 2), !dbg !116
  %105 = load i32, ptr %104, align 4, !dbg !117
  %106 = icmp eq i32 %105, 9877, !dbg !117
  br i1 %106, label %assert.exit.L61, label %assert.then.L61, !dbg !117, !prof !43

assert.then.L61:                                  ; preds = %assert.exit.L60
  %107 = call i32 (ptr, ...) @printf(ptr @anon.string.21), !dbg !117
  call void @exit(i32 1), !dbg !117
  unreachable, !dbg !117

assert.exit.L61:                                  ; preds = %assert.exit.L60
  %108 = call %struct.VectorIterator @_Z7iterateR6VectorIiE(ptr %vi), !dbg !118
  store %struct.VectorIterator %108, ptr %13, align 8, !dbg !118
  call void @llvm.dbg.declare(metadata ptr %idx, metadata !120, metadata !DIExpression()), !dbg !122
  call void @llvm.dbg.declare(metadata ptr %item2, metadata !123, metadata !DIExpression()), !dbg !124
  store i64 0, ptr %idx, align 8, !dbg !122
  br label %foreach.head.L63, !dbg !124

foreach.head.L63:                                 ; preds = %foreach.tail.L63, %assert.exit.L61
  %109 = call i1 @_ZN14VectorIteratorIiE7isValidEv(ptr %13), !dbg !124
  br i1 %109, label %foreach.body.L63, label %foreach.exit.L63, !dbg !124

foreach.body.L63:                                 ; preds = %foreach.head.L63
  %pair3 = call %struct.Pair @_ZN14VectorIteratorIiE6getIdxEv(ptr %13), !dbg !124
  store %struct.Pair %pair3, ptr %pair_addr, align 8, !dbg !124
  %110 = load i64, ptr %pair_addr, align 8, !dbg !124
  store i64 %110, ptr %idx, align 8, !dbg !124
  %item_addr = getelementptr inbounds %struct.Pair, ptr %pair_addr, i32 0, i32 1, !dbg !124
  %111 = load ptr, ptr %item_addr, align 8, !dbg !124
  store ptr %111, ptr %14, align 8, !dbg !124
  %112 = load i64, ptr %idx, align 8, !dbg !125
  %113 = trunc i64 %112 to i32, !dbg !125
  %114 = load ptr, ptr %14, align 8, !dbg !125
  %115 = load i32, ptr %114, align 4, !dbg !125
  %116 = add nsw i32 %115, %113, !dbg !125
  store i32 %116, ptr %114, align 4, !dbg !125
  br label %foreach.tail.L63, !dbg !125

foreach.tail.L63:                                 ; preds = %foreach.body.L63
  call void @_ZN14VectorIteratorIiE4nextEv(ptr %13), !dbg !125
  br label %foreach.head.L63, !dbg !125

foreach.exit.L63:                                 ; preds = %foreach.head.L63
  %117 = call ptr @_ZN6VectorIiE3getEj(ptr noundef nonnull align 8 dereferenceable(24) %vi, i32 0), !dbg !126
  %118 = load i32, ptr %117, align 4, !dbg !127
  %119 = icmp eq i32 %118, 124, !dbg !127
  br i1 %119, label %assert.exit.L66, label %assert.then.L66, !dbg !127, !prof !43

assert.then.L66:                                  ; preds = %foreach.exit.L63
  %120 = call i32 (ptr, ...) @printf(ptr @anon.string.22), !dbg !127
  call void @exit(i32 1), !dbg !127
  unreachable, !dbg !127

assert.exit.L66:                                  ; preds = %foreach.exit.L63
  %121 = call ptr @_ZN6VectorIiE3getEj(ptr noundef nonnull align 8 dereferenceable(24) %vi, i32 1), !dbg !128
  %122 = load i32, ptr %121, align 4, !dbg !129
  %123 = icmp eq i32 %122, 4323, !dbg !129
  br i1 %123, label %assert.exit.L67, label %assert.then.L67, !dbg !129, !prof !43

assert.then.L67:                                  ; preds = %assert.exit.L66
  %124 = call i32 (ptr, ...) @printf(ptr @anon.string.23), !dbg !129
  call void @exit(i32 1), !dbg !129
  unreachable, !dbg !129

assert.exit.L67:                                  ; preds = %assert.exit.L66
  %125 = call ptr @_ZN6VectorIiE3getEj(ptr noundef nonnull align 8 dereferenceable(24) %vi, i32 2), !dbg !130
  %126 = load i32, ptr %125, align 4, !dbg !131
  %127 = icmp eq i32 %126, 9879, !dbg !131
  br i1 %127, label %assert.exit.L68, label %assert.then.L68, !dbg !131, !prof !43

assert.then.L68:                                  ; preds = %assert.exit.L67
  %128 = call i32 (ptr, ...) @printf(ptr @anon.string.24), !dbg !131
  call void @exit(i32 1), !dbg !131
  unreachable, !dbg !131

assert.exit.L68:                                  ; preds = %assert.exit.L67
  %129 = call i32 (ptr, ...) @printf(ptr noundef @printf.str.0), !dbg !132
  call void @_ZN6VectorIiE4dtorEv(ptr %vi), !dbg !132
  %130 = load i32, ptr %result, align 4, !dbg !132
  ret i32 %130, !dbg !132
}

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

declare void @_ZN6VectorIiE4ctorEv(ptr)

declare void @_ZN6VectorIiE8pushBackERKi(ptr, ptr)

declare i64 @_ZN6VectorIiE7getSizeEv(ptr)

; Function Attrs: nofree nounwind
declare noundef i32 @printf(ptr nocapture noundef readonly, ...) #2

; Function Attrs: cold noreturn nounwind
declare void @exit(i32) #3

declare %struct.VectorIterator @_Z7iterateR6VectorIiE(ptr)

declare i1 @_ZN14VectorIteratorIiE7isValidEv(ptr)

declare ptr @_ZN14VectorIteratorIiE3getEv(ptr)

declare void @_ZN14VectorIteratorIiE4nextEv(ptr)

declare %struct.Pair @_ZN14VectorIteratorIiE6getIdxEv(ptr)

declare ptr @_ZN4PairImRiE8getFirstEv(ptr)

declare ptr @_ZN4PairImRiE9getSecondEv(ptr)

declare void @_Z13op.minusequalR14VectorIteratorIiEi(ptr, i32)

declare void @_Z16op.plusplus.postR14VectorIteratorIiE(ptr)

declare void @_Z18op.minusminus.postR14VectorIteratorIiE(ptr)

declare void @_Z12op.plusequalR14VectorIteratorIiEi(ptr, i32)

declare ptr @_ZN6VectorIiE3getEj(ptr, i32)

declare void @_ZN6VectorIiE4dtorEv(ptr)

attributes #0 = { noinline nounwind optnone uwtable }
attributes #1 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }
attributes #2 = { nofree nounwind }
attributes #3 = { cold noreturn nounwind }

!llvm.dbg.cu = !{!2}
!llvm.module.flags = !{!7, !8, !9, !10, !11, !12, !13}
!llvm.ident = !{!14}

!0 = !DIGlobalVariableExpression(var: !1, expr: !DIExpression())
!1 = distinct !DIGlobalVariable(name: "printf.str.0", linkageName: "printf.str.0", scope: !2, file: !5, line: 70, type: !6, isLocal: true, isDefinition: true)
!2 = distinct !DICompileUnit(language: DW_LANG_C_plus_plus_20, file: !3, producer: "spice version dev (https://github.com/spicelang/spice)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, globals: !4, splitDebugInlining: false, nameTableKind: None)
!3 = !DIFile(filename: "C:\\Users\\Marc\\Documents\\JustForFunGitHubClonesFast\\spice\\cmake-build-debug\\test\\test-files\\irgenerator\\debug-info\\success-dbg-info-complex\\source.spice", directory: ".\\test-files\\irgenerator\\debug-info\\success-dbg-info-complex")
!4 = !{!0}
!5 = !DIFile(filename: "source.spice", directory: ".\\test-files\\irgenerator\\debug-info\\success-dbg-info-complex")
!6 = !DIStringType(name: "printf.str.0", size: 192)
!7 = !{i32 7, !"Dwarf Version", i32 4}
!8 = !{i32 2, !"Debug Info Version", i32 3}
!9 = !{i32 1, !"wchar_size", i32 4}
!10 = !{i32 8, !"PIC Level", i32 2}
!11 = !{i32 7, !"PIE Level", i32 2}
!12 = !{i32 7, !"uwtable", i32 2}
!13 = !{i32 7, !"frame-pointer", i32 2}
!14 = !{!"spice version dev (https://github.com/spicelang/spice)"}
!15 = distinct !DISubprogram(name: "main", linkageName: "_Z4mainiPPc", scope: !5, file: !5, line: 6, type: !16, scopeLine: 6, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !22)
!16 = !DISubroutineType(types: !17)
!17 = !{!18, !18, !19}
!18 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!19 = !DICompositeType(tag: DW_TAG_array_type, baseType: !20, elements: !22)
!20 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !21, size: 64)
!21 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_unsigned_char)
!22 = !{}
!23 = !DILocalVariable(name: "result", scope: !15, file: !5, line: 6, type: !18)
!24 = !DILocation(line: 6, column: 1, scope: !15)
!25 = !DILocalVariable(name: "_argc", arg: 1, scope: !15, file: !5, line: 6, type: !18)
!26 = !DILocalVariable(name: "_argv", arg: 2, scope: !15, file: !5, line: 6, type: !19)
!27 = !DILocalVariable(name: "vi", scope: !15, file: !5, line: 8, type: !28)
!28 = !DICompositeType(tag: DW_TAG_structure_type, name: "Vector", scope: !29, file: !29, line: 23, size: 192, align: 8, flags: DIFlagTypePassByReference | DIFlagNonTrivial, elements: !30, identifier: "struct.Vector")
!29 = !DIFile(filename: "vector.spice", directory: "C:\\Users\\Marc\\Documents\\JustForFunGitHubClonesFast\\spice\\std\\data")
!30 = !{!31, !33, !35}
!31 = !DIDerivedType(tag: DW_TAG_member, name: "contents", scope: !28, file: !29, line: 24, baseType: !32, size: 64)
!32 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !18, size: 64)
!33 = !DIDerivedType(tag: DW_TAG_member, name: "capacity", scope: !28, file: !29, line: 25, baseType: !34, size: 64, offset: 64)
!34 = !DIBasicType(name: "unsigned long", size: 64, encoding: DW_ATE_unsigned)
!35 = !DIDerivedType(tag: DW_TAG_member, name: "size", scope: !28, file: !29, line: 26, baseType: !34, size: 64, offset: 128)
!36 = !DILocation(line: 8, column: 5, scope: !15)
!37 = !DILocation(line: 8, column: 22, scope: !15)
!38 = !DILocation(line: 9, column: 17, scope: !15)
!39 = !DILocation(line: 10, column: 17, scope: !15)
!40 = !DILocation(line: 11, column: 17, scope: !15)
!41 = !DILocation(line: 12, column: 12, scope: !15)
!42 = !DILocation(line: 12, column: 28, scope: !15)
!43 = !{!"branch_weights", i32 2000, i32 1}
!44 = !DILocation(line: 15, column: 22, scope: !15)
!45 = !DILocalVariable(name: "it", scope: !15, file: !5, line: 15, type: !46)
!46 = !DICompositeType(tag: DW_TAG_structure_type, name: "VectorIterator", scope: !47, file: !47, line: 13, size: 192, align: 8, flags: DIFlagTypePassByReference | DIFlagNonTrivial, elements: !48, identifier: "struct.VectorIterator")
!47 = !DIFile(filename: "vector-iterator.spice", directory: "C:\\Users\\Marc\\Documents\\JustForFunGitHubClonesFast\\spice\\std\\iterator")
!48 = !{!49, !51}
!49 = !DIDerivedType(tag: DW_TAG_member, name: "vector", scope: !46, file: !47, line: 14, baseType: !50, size: 64, offset: 64)
!50 = !DIDerivedType(tag: DW_TAG_reference_type, baseType: !28, size: 64)
!51 = !DIDerivedType(tag: DW_TAG_member, name: "cursor", scope: !46, file: !47, line: 15, baseType: !34, size: 64, offset: 128)
!52 = !DILocation(line: 15, column: 5, scope: !15)
!53 = !DILocation(line: 16, column: 12, scope: !15)
!54 = !DILocation(line: 17, column: 12, scope: !15)
!55 = !DILocation(line: 17, column: 24, scope: !15)
!56 = !DILocation(line: 18, column: 12, scope: !15)
!57 = !DILocation(line: 18, column: 24, scope: !15)
!58 = !DILocation(line: 19, column: 5, scope: !15)
!59 = !DILocation(line: 20, column: 12, scope: !15)
!60 = !DILocation(line: 20, column: 24, scope: !15)
!61 = !DILocation(line: 21, column: 12, scope: !15)
!62 = !DILocation(line: 22, column: 5, scope: !15)
!63 = !DILocation(line: 23, column: 16, scope: !15)
!64 = !DILocalVariable(name: "pair", scope: !15, file: !5, line: 23, type: !65)
!65 = !DICompositeType(tag: DW_TAG_structure_type, name: "Pair", scope: !66, file: !66, line: 8, size: 128, align: 8, flags: DIFlagTypePassByReference | DIFlagNonTrivial, elements: !67, identifier: "struct.Pair")
!66 = !DIFile(filename: "pair.spice", directory: "C:\\Users\\Marc\\Documents\\JustForFunGitHubClonesFast\\spice\\std\\data")
!67 = !{!68, !69}
!68 = !DIDerivedType(tag: DW_TAG_member, name: "first", scope: !65, file: !66, line: 9, baseType: !34, size: 64)
!69 = !DIDerivedType(tag: DW_TAG_member, name: "second", scope: !65, file: !66, line: 10, baseType: !70, size: 64, offset: 64)
!70 = !DIDerivedType(tag: DW_TAG_reference_type, baseType: !18, size: 64)
!71 = !DILocation(line: 23, column: 5, scope: !15)
!72 = !DILocation(line: 24, column: 12, scope: !15)
!73 = !DILocation(line: 24, column: 31, scope: !15)
!74 = !DILocation(line: 25, column: 12, scope: !15)
!75 = !DILocation(line: 25, column: 32, scope: !15)
!76 = !DILocation(line: 26, column: 5, scope: !15)
!77 = !DILocation(line: 27, column: 13, scope: !15)
!78 = !DILocation(line: 30, column: 17, scope: !15)
!79 = !DILocation(line: 31, column: 17, scope: !15)
!80 = !DILocation(line: 32, column: 12, scope: !15)
!81 = !DILocation(line: 35, column: 5, scope: !15)
!82 = !DILocation(line: 36, column: 12, scope: !15)
!83 = !DILocation(line: 36, column: 24, scope: !15)
!84 = !DILocation(line: 37, column: 12, scope: !15)
!85 = !DILocation(line: 38, column: 5, scope: !15)
!86 = !DILocation(line: 39, column: 12, scope: !15)
!87 = !DILocation(line: 39, column: 24, scope: !15)
!88 = !DILocation(line: 40, column: 5, scope: !15)
!89 = !DILocation(line: 41, column: 12, scope: !15)
!90 = !DILocation(line: 41, column: 24, scope: !15)
!91 = !DILocation(line: 42, column: 5, scope: !15)
!92 = !DILocation(line: 43, column: 12, scope: !15)
!93 = !DILocation(line: 43, column: 24, scope: !15)
!94 = !DILocation(line: 44, column: 5, scope: !15)
!95 = !DILocation(line: 45, column: 13, scope: !15)
!96 = !DILocation(line: 48, column: 32, scope: !97)
!97 = distinct !DILexicalBlock(scope: !15, file: !5, line: 48, column: 5)
!98 = !DILocalVariable(name: "item", scope: !97, file: !5, line: 48, type: !18)
!99 = !DILocation(line: 48, column: 13, scope: !97)
!100 = !DILocation(line: 49, column: 9, scope: !97)
!101 = !DILocation(line: 51, column: 19, scope: !15)
!102 = !DILocation(line: 51, column: 25, scope: !15)
!103 = !DILocation(line: 52, column: 19, scope: !15)
!104 = !DILocation(line: 52, column: 25, scope: !15)
!105 = !DILocation(line: 53, column: 19, scope: !15)
!106 = !DILocation(line: 53, column: 25, scope: !15)
!107 = !DILocation(line: 56, column: 33, scope: !108)
!108 = distinct !DILexicalBlock(scope: !15, file: !5, line: 56, column: 5)
!109 = !DILocalVariable(name: "item", scope: !108, file: !5, line: 56, type: !70)
!110 = !DILocation(line: 56, column: 13, scope: !108)
!111 = !DILocation(line: 57, column: 9, scope: !108)
!112 = !DILocation(line: 59, column: 19, scope: !15)
!113 = !DILocation(line: 59, column: 25, scope: !15)
!114 = !DILocation(line: 60, column: 19, scope: !15)
!115 = !DILocation(line: 60, column: 25, scope: !15)
!116 = !DILocation(line: 61, column: 19, scope: !15)
!117 = !DILocation(line: 61, column: 25, scope: !15)
!118 = !DILocation(line: 63, column: 43, scope: !119)
!119 = distinct !DILexicalBlock(scope: !15, file: !5, line: 63, column: 5)
!120 = !DILocalVariable(name: "idx", scope: !119, file: !5, line: 63, type: !121)
!121 = !DIBasicType(name: "long", size: 64, encoding: DW_ATE_signed)
!122 = !DILocation(line: 63, column: 13, scope: !119)
!123 = !DILocalVariable(name: "item", scope: !119, file: !5, line: 63, type: !70)
!124 = !DILocation(line: 63, column: 23, scope: !119)
!125 = !DILocation(line: 64, column: 9, scope: !119)
!126 = !DILocation(line: 66, column: 19, scope: !15)
!127 = !DILocation(line: 66, column: 25, scope: !15)
!128 = !DILocation(line: 67, column: 19, scope: !15)
!129 = !DILocation(line: 67, column: 25, scope: !15)
!130 = !DILocation(line: 68, column: 19, scope: !15)
!131 = !DILocation(line: 68, column: 25, scope: !15)
!132 = !DILocation(line: 70, column: 5, scope: !15)
