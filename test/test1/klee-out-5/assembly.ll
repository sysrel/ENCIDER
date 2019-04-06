; ModuleID = 'test1.bc'
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@.str = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@.str.1 = private unnamed_addr constant [12 x i8] c"klee_choose\00", align 1
@.str.1.2 = private unnamed_addr constant [86 x i8] c"/home/tuba/Documents/tools/pmguidedsymex/klee/runtime/Intrinsic/klee_div_zero_check.c\00", align 1
@.str.1.2.3 = private unnamed_addr constant [15 x i8] c"divide by zero\00", align 1
@.str.2 = private unnamed_addr constant [8 x i8] c"div.err\00", align 1
@.str.3 = private unnamed_addr constant [8 x i8] c"IGNORED\00", align 1
@.str.1.4 = private unnamed_addr constant [16 x i8] c"overshift error\00", align 1
@.str.2.5 = private unnamed_addr constant [14 x i8] c"overshift.err\00", align 1
@.str.6 = private unnamed_addr constant [77 x i8] c"/home/tuba/Documents/tools/pmguidedsymex/klee/runtime/Intrinsic/klee_range.c\00", align 1
@.str.1.7 = private unnamed_addr constant [14 x i8] c"invalid range\00", align 1
@.str.2.8 = private unnamed_addr constant [5 x i8] c"user\00", align 1

; Function Attrs: nounwind uwtable
define i32 @loop(i32 %ru, i32 %base) #0 !dbg !4 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %result = alloca i32, align 4
  %i = alloca i32, align 4
  store i32 %ru, i32* %1, align 4
  store i32 %base, i32* %2, align 4
  %3 = load i32, i32* %2, align 4, !dbg !119
  store i32 %3, i32* %result, align 4, !dbg !120
  store i32 0, i32* %i, align 4, !dbg !121
  br label %4, !dbg !123

; <label>:4                                       ; preds = %9, %0
  %5 = load i32, i32* %i, align 4, !dbg !124
  %6 = load i32, i32* %1, align 4, !dbg !126
  %7 = icmp slt i32 %5, %6, !dbg !127
  %8 = load i32, i32* %result, align 4, !dbg !128
  br i1 %7, label %9, label %14, !dbg !129

; <label>:9                                       ; preds = %4
  %10 = load i32, i32* %2, align 4, !dbg !130
  %11 = mul nsw i32 %8, %10, !dbg !131
  store i32 %11, i32* %result, align 4, !dbg !132
  %12 = load i32, i32* %i, align 4, !dbg !133
  %13 = add nsw i32 %12, 1, !dbg !133
  store i32 %13, i32* %i, align 4, !dbg !133
  br label %4, !dbg !134

; <label>:14                                      ; preds = %4
  ret i32 %8, !dbg !135
}

; Function Attrs: nounwind readnone
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: nounwind uwtable
define i32 @foo(i32 %L, i32 %H) #0 !dbg !8 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  store i32 %L, i32* %2, align 4
  store i32 %H, i32* %3, align 4
  %4 = load i32, i32* %3, align 4, !dbg !136
  %5 = icmp sgt i32 %4, 0, !dbg !138
  %6 = load i32, i32* %2, align 4, !dbg !139
  br i1 %5, label %7, label %21, !dbg !142

; <label>:7                                       ; preds = %0
  %8 = icmp slt i32 %6, 1, !dbg !143
  br i1 %8, label %9, label %12, !dbg !144

; <label>:9                                       ; preds = %7
  %10 = call i32 @loop(i32 3, i32 2), !dbg !145
  %11 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i32 0, i32 0), i32 %10), !dbg !147
  store i32 1, i32* %1, align 4, !dbg !148
  br label %45, !dbg !148

; <label>:12                                      ; preds = %7
  %13 = load i32, i32* %2, align 4, !dbg !149
  %14 = icmp slt i32 %13, 5, !dbg !151
  br i1 %14, label %15, label %18, !dbg !152

; <label>:15                                      ; preds = %12
  %16 = call i32 @loop(i32 4, i32 2), !dbg !153
  %17 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i32 0, i32 0), i32 %16), !dbg !155
  store i32 1, i32* %1, align 4, !dbg !156
  br label %45, !dbg !156

; <label>:18                                      ; preds = %12
  %19 = call i32 @loop(i32 5, i32 2), !dbg !157
  %20 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i32 0, i32 0), i32 %19), !dbg !159
  store i32 1, i32* %1, align 4, !dbg !160
  br label %45, !dbg !160

; <label>:21                                      ; preds = %0
  %22 = icmp slt i32 %6, 0, !dbg !161
  br i1 %22, label %23, label %42, !dbg !164

; <label>:23                                      ; preds = %21
  %24 = load i32, i32* %3, align 4, !dbg !165
  %25 = icmp sgt i32 %24, -10, !dbg !168
  %26 = load i32, i32* %2, align 4, !dbg !169
  %27 = icmp eq i32 %26, -1, !dbg !172
  br i1 %25, label %28, label %35, !dbg !173

; <label>:28                                      ; preds = %23
  br i1 %27, label %29, label %32, !dbg !174

; <label>:29                                      ; preds = %28
  %30 = call i32 @loop(i32 6, i32 2), !dbg !175
  %31 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i32 0, i32 0), i32 %30), !dbg !177
  store i32 1, i32* %1, align 4, !dbg !178
  br label %45, !dbg !178

; <label>:32                                      ; preds = %28
  %33 = call i32 @loop(i32 7, i32 2), !dbg !179
  %34 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i32 0, i32 0), i32 %33), !dbg !181
  store i32 1, i32* %1, align 4, !dbg !182
  br label %45, !dbg !182

; <label>:35                                      ; preds = %23
  br i1 %27, label %36, label %39, !dbg !183

; <label>:36                                      ; preds = %35
  %37 = call i32 @loop(i32 8, i32 2), !dbg !185
  %38 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i32 0, i32 0), i32 %37), !dbg !188
  store i32 1, i32* %1, align 4, !dbg !189
  br label %45, !dbg !189

; <label>:39                                      ; preds = %35
  %40 = call i32 @loop(i32 9, i32 2), !dbg !190
  %41 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i32 0, i32 0), i32 %40), !dbg !192
  store i32 1, i32* %1, align 4, !dbg !193
  br label %45, !dbg !193

; <label>:42                                      ; preds = %21
  %43 = call i32 @loop(i32 10, i32 2), !dbg !194
  %44 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i32 0, i32 0), i32 %43), !dbg !196
  store i32 1, i32* %1, align 4, !dbg !197
  br label %45, !dbg !197

; <label>:45                                      ; preds = %42, %39, %36, %32, %29, %18, %15, %9
  %46 = load i32, i32* %1, align 4, !dbg !198
  ret i32 %46, !dbg !198
}

declare i32 @printf(i8*, ...) #2

; Function Attrs: nounwind uwtable
define i64 @klee_choose(i64 %n) #3 !dbg !12 {
  %x = alloca i64, align 8
  %1 = bitcast i64* %x to i8*, !dbg !199
  call void @klee_make_symbolic(i8* %1, i64 8, i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.1, i64 0, i64 0)) #8, !dbg !200
  %2 = load i64, i64* %x, align 8, !dbg !201, !tbaa !203
  %3 = icmp ult i64 %2, %n, !dbg !207
  br i1 %3, label %5, label %4, !dbg !208

; <label>:4                                       ; preds = %0
  call void @klee_silent_exit(i32 0) #9, !dbg !209
  unreachable, !dbg !209

; <label>:5                                       ; preds = %0
  ret i64 %2, !dbg !210
}

; Function Attrs: nounwind readnone
declare void @llvm.dbg.value(metadata, i64, metadata, metadata) #1

; Function Attrs: argmemonly nounwind
declare void @llvm.lifetime.start(i64, i8* nocapture) #4

declare void @klee_make_symbolic(i8*, i64, i8*) #5

; Function Attrs: noreturn
declare void @klee_silent_exit(i32) #6

; Function Attrs: argmemonly nounwind
declare void @llvm.lifetime.end(i64, i8* nocapture) #4

; Function Attrs: nounwind uwtable
define void @klee_div_zero_check(i64 %z) #3 !dbg !24 {
  %1 = icmp eq i64 %z, 0, !dbg !211
  br i1 %1, label %2, label %3, !dbg !213

; <label>:2                                       ; preds = %0
  tail call void @klee_report_error(i8* nonnull getelementptr inbounds ([86 x i8], [86 x i8]* @.str.1.2, i64 0, i64 0), i32 14, i8* nonnull getelementptr inbounds ([15 x i8], [15 x i8]* @.str.1.2.3, i64 0, i64 0), i8* nonnull getelementptr inbounds ([8 x i8], [8 x i8]* @.str.2, i64 0, i64 0)) #9, !dbg !214
  unreachable, !dbg !214

; <label>:3                                       ; preds = %0
  ret void, !dbg !215
}

; Function Attrs: noreturn
declare void @klee_report_error(i8*, i32, i8*, i8*) #6

; Function Attrs: nounwind uwtable
define i32 @klee_int(i8* %name) #3 !dbg !33 {
  %x = alloca i32, align 4
  %1 = bitcast i32* %x to i8*, !dbg !216
  call void @klee_make_symbolic(i8* %1, i64 4, i8* %name) #8, !dbg !217
  %2 = load i32, i32* %x, align 4, !dbg !218, !tbaa !219
  ret i32 %2, !dbg !221
}

; Function Attrs: nounwind uwtable
define void @klee_overshift_check(i64 %bitWidth, i64 %shift) #3 !dbg !45 {
  %1 = icmp ult i64 %shift, %bitWidth, !dbg !222
  br i1 %1, label %3, label %2, !dbg !224

; <label>:2                                       ; preds = %0
  tail call void @klee_report_error(i8* nonnull getelementptr inbounds ([8 x i8], [8 x i8]* @.str.3, i64 0, i64 0), i32 0, i8* nonnull getelementptr inbounds ([16 x i8], [16 x i8]* @.str.1.4, i64 0, i64 0), i8* nonnull getelementptr inbounds ([14 x i8], [14 x i8]* @.str.2.5, i64 0, i64 0)) #9, !dbg !225
  unreachable, !dbg !225

; <label>:3                                       ; preds = %0
  ret void, !dbg !227
}

; Function Attrs: nounwind uwtable
define i32 @klee_range(i32 %start, i32 %end, i8* %name) #3 !dbg !57 {
  %x = alloca i32, align 4
  %1 = bitcast i32* %x to i8*, !dbg !228
  %2 = icmp slt i32 %start, %end, !dbg !229
  br i1 %2, label %4, label %3, !dbg !231

; <label>:3                                       ; preds = %0
  tail call void @klee_report_error(i8* nonnull getelementptr inbounds ([77 x i8], [77 x i8]* @.str.6, i64 0, i64 0), i32 17, i8* nonnull getelementptr inbounds ([14 x i8], [14 x i8]* @.str.1.7, i64 0, i64 0), i8* nonnull getelementptr inbounds ([5 x i8], [5 x i8]* @.str.2.8, i64 0, i64 0)) #9, !dbg !232
  unreachable, !dbg !232

; <label>:4                                       ; preds = %0
  %5 = add nsw i32 %start, 1, !dbg !233
  %6 = icmp eq i32 %5, %end, !dbg !235
  br i1 %6, label %21, label %7, !dbg !236

; <label>:7                                       ; preds = %4
  call void @klee_make_symbolic(i8* %1, i64 4, i8* %name) #8, !dbg !237
  %8 = icmp eq i32 %start, 0, !dbg !239
  %9 = load i32, i32* %x, align 4, !dbg !241, !tbaa !219
  br i1 %8, label %10, label %13, !dbg !243

; <label>:10                                      ; preds = %7
  %11 = icmp ult i32 %9, %end, !dbg !244
  %12 = zext i1 %11 to i64, !dbg !245
  call void @klee_assume(i64 %12) #8, !dbg !246
  br label %19, !dbg !247

; <label>:13                                      ; preds = %7
  %14 = icmp sge i32 %9, %start, !dbg !248
  %15 = zext i1 %14 to i64, !dbg !250
  call void @klee_assume(i64 %15) #8, !dbg !251
  %16 = load i32, i32* %x, align 4, !dbg !252, !tbaa !219
  %17 = icmp slt i32 %16, %end, !dbg !253
  %18 = zext i1 %17 to i64, !dbg !252
  call void @klee_assume(i64 %18) #8, !dbg !254
  br label %19

; <label>:19                                      ; preds = %13, %10
  %20 = load i32, i32* %x, align 4, !dbg !255, !tbaa !219
  br label %21, !dbg !256

; <label>:21                                      ; preds = %4, %19
  %.0 = phi i32 [ %20, %19 ], [ %start, %4 ]
  ret i32 %.0, !dbg !257
}

declare void @klee_assume(i64) #5

; Function Attrs: norecurse nounwind uwtable
define weak i8* @memcpy(i8* %destaddr, i8* %srcaddr, i64 %len) #7 !dbg !68 {
  %1 = icmp eq i64 %len, 0, !dbg !258
  br i1 %1, label %._crit_edge, label %.lr.ph.preheader, !dbg !260

.lr.ph.preheader:                                 ; preds = %0
  %min.iters.check = icmp ult i64 %len, 16, !dbg !261
  br i1 %min.iters.check, label %.lr.ph.preheader71, label %min.iters.checked, !dbg !261

min.iters.checked:                                ; preds = %.lr.ph.preheader
  %n.vec = and i64 %len, -16, !dbg !261
  %cmp.zero = icmp eq i64 %n.vec, 0, !dbg !261
  br i1 %cmp.zero, label %.lr.ph.preheader71, label %vector.memcheck, !dbg !261

vector.memcheck:                                  ; preds = %min.iters.checked
  %2 = add i64 %len, -1, !dbg !261
  %scevgep = getelementptr i8, i8* %destaddr, i64 %2, !dbg !261
  %scevgep4 = getelementptr i8, i8* %srcaddr, i64 %2, !dbg !261
  %bound0 = icmp uge i8* %scevgep4, %destaddr, !dbg !261
  %bound1 = icmp uge i8* %scevgep, %srcaddr, !dbg !261
  %memcheck.conflict = and i1 %bound0, %bound1, !dbg !261
  %ind.end = getelementptr i8, i8* %srcaddr, i64 %n.vec, !dbg !261
  %ind.end6 = getelementptr i8, i8* %destaddr, i64 %n.vec, !dbg !261
  %ind.end8 = sub i64 %len, %n.vec, !dbg !261
  br i1 %memcheck.conflict, label %.lr.ph.preheader71, label %vector.body.preheader, !dbg !261

vector.body.preheader:                            ; preds = %vector.memcheck
  %3 = add i64 %len, -16
  %4 = lshr i64 %3, 4
  %5 = add nuw nsw i64 %4, 1
  %xtraiter72 = and i64 %5, 7
  %lcmp.mod73 = icmp eq i64 %xtraiter72, 0
  br i1 %lcmp.mod73, label %vector.body.preheader.split, label %vector.body.prol

vector.body.prol:                                 ; preds = %vector.body.preheader, %vector.body.prol
  %index.prol = phi i64 [ %index.next.prol, %vector.body.prol ], [ 0, %vector.body.preheader ]
  %prol.iter74 = phi i64 [ %prol.iter74.sub, %vector.body.prol ], [ %xtraiter72, %vector.body.preheader ]
  %next.gep.prol = getelementptr i8, i8* %srcaddr, i64 %index.prol
  %next.gep39.prol = getelementptr i8, i8* %destaddr, i64 %index.prol
  %6 = bitcast i8* %next.gep.prol to <16 x i8>*, !dbg !262
  %wide.load.prol = load <16 x i8>, <16 x i8>* %6, align 1, !dbg !262, !tbaa !263
  %7 = bitcast i8* %next.gep39.prol to <16 x i8>*, !dbg !264
  store <16 x i8> %wide.load.prol, <16 x i8>* %7, align 1, !dbg !264, !tbaa !263
  %index.next.prol = add i64 %index.prol, 16, !dbg !261
  %prol.iter74.sub = add i64 %prol.iter74, -1, !dbg !261
  %prol.iter74.cmp = icmp eq i64 %prol.iter74.sub, 0, !dbg !261
  br i1 %prol.iter74.cmp, label %vector.body.preheader.split, label %vector.body.prol, !dbg !261, !llvm.loop !265

vector.body.preheader.split:                      ; preds = %vector.body.prol, %vector.body.preheader
  %index.unr = phi i64 [ 0, %vector.body.preheader ], [ %index.next.prol, %vector.body.prol ]
  %8 = icmp ult i64 %3, 112
  br i1 %8, label %middle.block, label %vector.body

vector.body:                                      ; preds = %vector.body.preheader.split, %vector.body
  %index = phi i64 [ %index.next.7, %vector.body ], [ %index.unr, %vector.body.preheader.split ]
  %next.gep = getelementptr i8, i8* %srcaddr, i64 %index
  %next.gep39 = getelementptr i8, i8* %destaddr, i64 %index
  %9 = bitcast i8* %next.gep to <16 x i8>*, !dbg !262
  %wide.load = load <16 x i8>, <16 x i8>* %9, align 1, !dbg !262, !tbaa !263
  %10 = bitcast i8* %next.gep39 to <16 x i8>*, !dbg !264
  store <16 x i8> %wide.load, <16 x i8>* %10, align 1, !dbg !264, !tbaa !263
  %index.next = add i64 %index, 16, !dbg !261
  %next.gep.1 = getelementptr i8, i8* %srcaddr, i64 %index.next
  %next.gep39.1 = getelementptr i8, i8* %destaddr, i64 %index.next
  %11 = bitcast i8* %next.gep.1 to <16 x i8>*, !dbg !262
  %wide.load.1 = load <16 x i8>, <16 x i8>* %11, align 1, !dbg !262, !tbaa !263
  %12 = bitcast i8* %next.gep39.1 to <16 x i8>*, !dbg !264
  store <16 x i8> %wide.load.1, <16 x i8>* %12, align 1, !dbg !264, !tbaa !263
  %index.next.1 = add i64 %index, 32, !dbg !261
  %next.gep.2 = getelementptr i8, i8* %srcaddr, i64 %index.next.1
  %next.gep39.2 = getelementptr i8, i8* %destaddr, i64 %index.next.1
  %13 = bitcast i8* %next.gep.2 to <16 x i8>*, !dbg !262
  %wide.load.2 = load <16 x i8>, <16 x i8>* %13, align 1, !dbg !262, !tbaa !263
  %14 = bitcast i8* %next.gep39.2 to <16 x i8>*, !dbg !264
  store <16 x i8> %wide.load.2, <16 x i8>* %14, align 1, !dbg !264, !tbaa !263
  %index.next.2 = add i64 %index, 48, !dbg !261
  %next.gep.3 = getelementptr i8, i8* %srcaddr, i64 %index.next.2
  %next.gep39.3 = getelementptr i8, i8* %destaddr, i64 %index.next.2
  %15 = bitcast i8* %next.gep.3 to <16 x i8>*, !dbg !262
  %wide.load.3 = load <16 x i8>, <16 x i8>* %15, align 1, !dbg !262, !tbaa !263
  %16 = bitcast i8* %next.gep39.3 to <16 x i8>*, !dbg !264
  store <16 x i8> %wide.load.3, <16 x i8>* %16, align 1, !dbg !264, !tbaa !263
  %index.next.3 = add i64 %index, 64, !dbg !261
  %next.gep.4 = getelementptr i8, i8* %srcaddr, i64 %index.next.3
  %next.gep39.4 = getelementptr i8, i8* %destaddr, i64 %index.next.3
  %17 = bitcast i8* %next.gep.4 to <16 x i8>*, !dbg !262
  %wide.load.4 = load <16 x i8>, <16 x i8>* %17, align 1, !dbg !262, !tbaa !263
  %18 = bitcast i8* %next.gep39.4 to <16 x i8>*, !dbg !264
  store <16 x i8> %wide.load.4, <16 x i8>* %18, align 1, !dbg !264, !tbaa !263
  %index.next.4 = add i64 %index, 80, !dbg !261
  %next.gep.5 = getelementptr i8, i8* %srcaddr, i64 %index.next.4
  %next.gep39.5 = getelementptr i8, i8* %destaddr, i64 %index.next.4
  %19 = bitcast i8* %next.gep.5 to <16 x i8>*, !dbg !262
  %wide.load.5 = load <16 x i8>, <16 x i8>* %19, align 1, !dbg !262, !tbaa !263
  %20 = bitcast i8* %next.gep39.5 to <16 x i8>*, !dbg !264
  store <16 x i8> %wide.load.5, <16 x i8>* %20, align 1, !dbg !264, !tbaa !263
  %index.next.5 = add i64 %index, 96, !dbg !261
  %next.gep.6 = getelementptr i8, i8* %srcaddr, i64 %index.next.5
  %next.gep39.6 = getelementptr i8, i8* %destaddr, i64 %index.next.5
  %21 = bitcast i8* %next.gep.6 to <16 x i8>*, !dbg !262
  %wide.load.6 = load <16 x i8>, <16 x i8>* %21, align 1, !dbg !262, !tbaa !263
  %22 = bitcast i8* %next.gep39.6 to <16 x i8>*, !dbg !264
  store <16 x i8> %wide.load.6, <16 x i8>* %22, align 1, !dbg !264, !tbaa !263
  %index.next.6 = add i64 %index, 112, !dbg !261
  %next.gep.7 = getelementptr i8, i8* %srcaddr, i64 %index.next.6
  %next.gep39.7 = getelementptr i8, i8* %destaddr, i64 %index.next.6
  %23 = bitcast i8* %next.gep.7 to <16 x i8>*, !dbg !262
  %wide.load.7 = load <16 x i8>, <16 x i8>* %23, align 1, !dbg !262, !tbaa !263
  %24 = bitcast i8* %next.gep39.7 to <16 x i8>*, !dbg !264
  store <16 x i8> %wide.load.7, <16 x i8>* %24, align 1, !dbg !264, !tbaa !263
  %index.next.7 = add i64 %index, 128, !dbg !261
  %25 = icmp eq i64 %index.next.7, %n.vec, !dbg !261
  br i1 %25, label %middle.block, label %vector.body, !dbg !261, !llvm.loop !267

middle.block:                                     ; preds = %vector.body, %vector.body.preheader.split
  %cmp.n = icmp eq i64 %n.vec, %len
  br i1 %cmp.n, label %._crit_edge, label %.lr.ph.preheader71, !dbg !261

.lr.ph.preheader71:                               ; preds = %middle.block, %vector.memcheck, %min.iters.checked, %.lr.ph.preheader
  %src.03.ph = phi i8* [ %srcaddr, %vector.memcheck ], [ %srcaddr, %min.iters.checked ], [ %srcaddr, %.lr.ph.preheader ], [ %ind.end, %middle.block ]
  %dest.02.ph = phi i8* [ %destaddr, %vector.memcheck ], [ %destaddr, %min.iters.checked ], [ %destaddr, %.lr.ph.preheader ], [ %ind.end6, %middle.block ]
  %.01.ph = phi i64 [ %len, %vector.memcheck ], [ %len, %min.iters.checked ], [ %len, %.lr.ph.preheader ], [ %ind.end8, %middle.block ]
  %26 = add i64 %.01.ph, -1, !dbg !261
  %xtraiter = and i64 %.01.ph, 7, !dbg !261
  %lcmp.mod = icmp eq i64 %xtraiter, 0, !dbg !261
  br i1 %lcmp.mod, label %.lr.ph.preheader71.split, label %.lr.ph.prol, !dbg !261

.lr.ph.prol:                                      ; preds = %.lr.ph.preheader71, %.lr.ph.prol
  %src.03.prol = phi i8* [ %28, %.lr.ph.prol ], [ %src.03.ph, %.lr.ph.preheader71 ]
  %dest.02.prol = phi i8* [ %30, %.lr.ph.prol ], [ %dest.02.ph, %.lr.ph.preheader71 ]
  %.01.prol = phi i64 [ %27, %.lr.ph.prol ], [ %.01.ph, %.lr.ph.preheader71 ]
  %prol.iter = phi i64 [ %prol.iter.sub, %.lr.ph.prol ], [ %xtraiter, %.lr.ph.preheader71 ]
  %27 = add i64 %.01.prol, -1, !dbg !261
  %28 = getelementptr inbounds i8, i8* %src.03.prol, i64 1, !dbg !270
  %29 = load i8, i8* %src.03.prol, align 1, !dbg !262, !tbaa !263
  %30 = getelementptr inbounds i8, i8* %dest.02.prol, i64 1, !dbg !271
  store i8 %29, i8* %dest.02.prol, align 1, !dbg !264, !tbaa !263
  %prol.iter.sub = add i64 %prol.iter, -1, !dbg !260
  %prol.iter.cmp = icmp eq i64 %prol.iter.sub, 0, !dbg !260
  br i1 %prol.iter.cmp, label %.lr.ph.preheader71.split, label %.lr.ph.prol, !dbg !260, !llvm.loop !272

.lr.ph.preheader71.split:                         ; preds = %.lr.ph.prol, %.lr.ph.preheader71
  %src.03.unr = phi i8* [ %src.03.ph, %.lr.ph.preheader71 ], [ %28, %.lr.ph.prol ]
  %dest.02.unr = phi i8* [ %dest.02.ph, %.lr.ph.preheader71 ], [ %30, %.lr.ph.prol ]
  %.01.unr = phi i64 [ %.01.ph, %.lr.ph.preheader71 ], [ %27, %.lr.ph.prol ]
  %31 = icmp ult i64 %26, 7, !dbg !261
  br i1 %31, label %._crit_edge, label %.lr.ph, !dbg !261

.lr.ph:                                           ; preds = %.lr.ph.preheader71.split, %.lr.ph
  %src.03 = phi i8* [ %54, %.lr.ph ], [ %src.03.unr, %.lr.ph.preheader71.split ]
  %dest.02 = phi i8* [ %56, %.lr.ph ], [ %dest.02.unr, %.lr.ph.preheader71.split ]
  %.01 = phi i64 [ %53, %.lr.ph ], [ %.01.unr, %.lr.ph.preheader71.split ]
  %32 = getelementptr inbounds i8, i8* %src.03, i64 1, !dbg !270
  %33 = load i8, i8* %src.03, align 1, !dbg !262, !tbaa !263
  %34 = getelementptr inbounds i8, i8* %dest.02, i64 1, !dbg !271
  store i8 %33, i8* %dest.02, align 1, !dbg !264, !tbaa !263
  %35 = getelementptr inbounds i8, i8* %src.03, i64 2, !dbg !270
  %36 = load i8, i8* %32, align 1, !dbg !262, !tbaa !263
  %37 = getelementptr inbounds i8, i8* %dest.02, i64 2, !dbg !271
  store i8 %36, i8* %34, align 1, !dbg !264, !tbaa !263
  %38 = getelementptr inbounds i8, i8* %src.03, i64 3, !dbg !270
  %39 = load i8, i8* %35, align 1, !dbg !262, !tbaa !263
  %40 = getelementptr inbounds i8, i8* %dest.02, i64 3, !dbg !271
  store i8 %39, i8* %37, align 1, !dbg !264, !tbaa !263
  %41 = getelementptr inbounds i8, i8* %src.03, i64 4, !dbg !270
  %42 = load i8, i8* %38, align 1, !dbg !262, !tbaa !263
  %43 = getelementptr inbounds i8, i8* %dest.02, i64 4, !dbg !271
  store i8 %42, i8* %40, align 1, !dbg !264, !tbaa !263
  %44 = getelementptr inbounds i8, i8* %src.03, i64 5, !dbg !270
  %45 = load i8, i8* %41, align 1, !dbg !262, !tbaa !263
  %46 = getelementptr inbounds i8, i8* %dest.02, i64 5, !dbg !271
  store i8 %45, i8* %43, align 1, !dbg !264, !tbaa !263
  %47 = getelementptr inbounds i8, i8* %src.03, i64 6, !dbg !270
  %48 = load i8, i8* %44, align 1, !dbg !262, !tbaa !263
  %49 = getelementptr inbounds i8, i8* %dest.02, i64 6, !dbg !271
  store i8 %48, i8* %46, align 1, !dbg !264, !tbaa !263
  %50 = getelementptr inbounds i8, i8* %src.03, i64 7, !dbg !270
  %51 = load i8, i8* %47, align 1, !dbg !262, !tbaa !263
  %52 = getelementptr inbounds i8, i8* %dest.02, i64 7, !dbg !271
  store i8 %51, i8* %49, align 1, !dbg !264, !tbaa !263
  %53 = add i64 %.01, -8, !dbg !261
  %54 = getelementptr inbounds i8, i8* %src.03, i64 8, !dbg !270
  %55 = load i8, i8* %50, align 1, !dbg !262, !tbaa !263
  %56 = getelementptr inbounds i8, i8* %dest.02, i64 8, !dbg !271
  store i8 %55, i8* %52, align 1, !dbg !264, !tbaa !263
  %57 = icmp eq i64 %53, 0, !dbg !258
  br i1 %57, label %._crit_edge, label %.lr.ph, !dbg !260, !llvm.loop !273

._crit_edge:                                      ; preds = %.lr.ph.preheader71.split, %.lr.ph, %middle.block, %0
  ret i8* %destaddr, !dbg !274
}

; Function Attrs: norecurse nounwind uwtable
define weak i8* @memmove(i8* %dst, i8* %src, i64 %count) #7 !dbg !86 {
  %1 = icmp eq i8* %src, %dst, !dbg !275
  br i1 %1, label %.loopexit, label %2, !dbg !277

; <label>:2                                       ; preds = %0
  %3 = icmp ugt i8* %src, %dst, !dbg !278
  br i1 %3, label %.preheader, label %61, !dbg !280

.preheader:                                       ; preds = %2
  %4 = icmp eq i64 %count, 0, !dbg !281
  br i1 %4, label %.loopexit, label %.lr.ph.preheader, !dbg !281

.lr.ph.preheader:                                 ; preds = %.preheader
  %min.iters.check152 = icmp ult i64 %count, 16, !dbg !284
  br i1 %min.iters.check152, label %.lr.ph.preheader244, label %min.iters.checked153, !dbg !284

min.iters.checked153:                             ; preds = %.lr.ph.preheader
  %n.vec155 = and i64 %count, -16, !dbg !284
  %cmp.zero156 = icmp eq i64 %n.vec155, 0, !dbg !284
  br i1 %cmp.zero156, label %.lr.ph.preheader244, label %vector.memcheck164, !dbg !284

vector.memcheck164:                               ; preds = %min.iters.checked153
  %5 = add i64 %count, -1, !dbg !284
  %scevgep158 = getelementptr i8, i8* %dst, i64 %5, !dbg !284
  %scevgep159 = getelementptr i8, i8* %src, i64 %5, !dbg !284
  %bound0160 = icmp uge i8* %scevgep159, %dst, !dbg !284
  %bound1161 = icmp uge i8* %scevgep158, %src, !dbg !284
  %memcheck.conflict163 = and i1 %bound0160, %bound1161, !dbg !284
  %ind.end169 = getelementptr i8, i8* %src, i64 %n.vec155, !dbg !284
  %ind.end171 = getelementptr i8, i8* %dst, i64 %n.vec155, !dbg !284
  %ind.end173 = sub i64 %count, %n.vec155, !dbg !284
  br i1 %memcheck.conflict163, label %.lr.ph.preheader244, label %vector.body149.preheader, !dbg !284

vector.body149.preheader:                         ; preds = %vector.memcheck164
  %6 = add i64 %count, -16
  %7 = lshr i64 %6, 4
  %8 = add nuw nsw i64 %7, 1
  %xtraiter247 = and i64 %8, 7
  %lcmp.mod248 = icmp eq i64 %xtraiter247, 0
  br i1 %lcmp.mod248, label %vector.body149.preheader.split, label %vector.body149.prol

vector.body149.prol:                              ; preds = %vector.body149.preheader, %vector.body149.prol
  %index166.prol = phi i64 [ %index.next167.prol, %vector.body149.prol ], [ 0, %vector.body149.preheader ]
  %prol.iter249 = phi i64 [ %prol.iter249.sub, %vector.body149.prol ], [ %xtraiter247, %vector.body149.preheader ]
  %next.gep175.prol = getelementptr i8, i8* %src, i64 %index166.prol
  %next.gep207.prol = getelementptr i8, i8* %dst, i64 %index166.prol
  %9 = bitcast i8* %next.gep175.prol to <16 x i8>*, !dbg !285
  %wide.load243.prol = load <16 x i8>, <16 x i8>* %9, align 1, !dbg !285, !tbaa !263
  %10 = bitcast i8* %next.gep207.prol to <16 x i8>*, !dbg !287
  store <16 x i8> %wide.load243.prol, <16 x i8>* %10, align 1, !dbg !287, !tbaa !263
  %index.next167.prol = add i64 %index166.prol, 16, !dbg !284
  %prol.iter249.sub = add i64 %prol.iter249, -1, !dbg !284
  %prol.iter249.cmp = icmp eq i64 %prol.iter249.sub, 0, !dbg !284
  br i1 %prol.iter249.cmp, label %vector.body149.preheader.split, label %vector.body149.prol, !dbg !284, !llvm.loop !288

vector.body149.preheader.split:                   ; preds = %vector.body149.prol, %vector.body149.preheader
  %index166.unr = phi i64 [ 0, %vector.body149.preheader ], [ %index.next167.prol, %vector.body149.prol ]
  %11 = icmp ult i64 %6, 112
  br i1 %11, label %middle.block150, label %vector.body149

vector.body149:                                   ; preds = %vector.body149.preheader.split, %vector.body149
  %index166 = phi i64 [ %index.next167.7, %vector.body149 ], [ %index166.unr, %vector.body149.preheader.split ]
  %next.gep175 = getelementptr i8, i8* %src, i64 %index166
  %next.gep207 = getelementptr i8, i8* %dst, i64 %index166
  %12 = bitcast i8* %next.gep175 to <16 x i8>*, !dbg !285
  %wide.load243 = load <16 x i8>, <16 x i8>* %12, align 1, !dbg !285, !tbaa !263
  %13 = bitcast i8* %next.gep207 to <16 x i8>*, !dbg !287
  store <16 x i8> %wide.load243, <16 x i8>* %13, align 1, !dbg !287, !tbaa !263
  %index.next167 = add i64 %index166, 16, !dbg !284
  %next.gep175.1 = getelementptr i8, i8* %src, i64 %index.next167
  %next.gep207.1 = getelementptr i8, i8* %dst, i64 %index.next167
  %14 = bitcast i8* %next.gep175.1 to <16 x i8>*, !dbg !285
  %wide.load243.1 = load <16 x i8>, <16 x i8>* %14, align 1, !dbg !285, !tbaa !263
  %15 = bitcast i8* %next.gep207.1 to <16 x i8>*, !dbg !287
  store <16 x i8> %wide.load243.1, <16 x i8>* %15, align 1, !dbg !287, !tbaa !263
  %index.next167.1 = add i64 %index166, 32, !dbg !284
  %next.gep175.2 = getelementptr i8, i8* %src, i64 %index.next167.1
  %next.gep207.2 = getelementptr i8, i8* %dst, i64 %index.next167.1
  %16 = bitcast i8* %next.gep175.2 to <16 x i8>*, !dbg !285
  %wide.load243.2 = load <16 x i8>, <16 x i8>* %16, align 1, !dbg !285, !tbaa !263
  %17 = bitcast i8* %next.gep207.2 to <16 x i8>*, !dbg !287
  store <16 x i8> %wide.load243.2, <16 x i8>* %17, align 1, !dbg !287, !tbaa !263
  %index.next167.2 = add i64 %index166, 48, !dbg !284
  %next.gep175.3 = getelementptr i8, i8* %src, i64 %index.next167.2
  %next.gep207.3 = getelementptr i8, i8* %dst, i64 %index.next167.2
  %18 = bitcast i8* %next.gep175.3 to <16 x i8>*, !dbg !285
  %wide.load243.3 = load <16 x i8>, <16 x i8>* %18, align 1, !dbg !285, !tbaa !263
  %19 = bitcast i8* %next.gep207.3 to <16 x i8>*, !dbg !287
  store <16 x i8> %wide.load243.3, <16 x i8>* %19, align 1, !dbg !287, !tbaa !263
  %index.next167.3 = add i64 %index166, 64, !dbg !284
  %next.gep175.4 = getelementptr i8, i8* %src, i64 %index.next167.3
  %next.gep207.4 = getelementptr i8, i8* %dst, i64 %index.next167.3
  %20 = bitcast i8* %next.gep175.4 to <16 x i8>*, !dbg !285
  %wide.load243.4 = load <16 x i8>, <16 x i8>* %20, align 1, !dbg !285, !tbaa !263
  %21 = bitcast i8* %next.gep207.4 to <16 x i8>*, !dbg !287
  store <16 x i8> %wide.load243.4, <16 x i8>* %21, align 1, !dbg !287, !tbaa !263
  %index.next167.4 = add i64 %index166, 80, !dbg !284
  %next.gep175.5 = getelementptr i8, i8* %src, i64 %index.next167.4
  %next.gep207.5 = getelementptr i8, i8* %dst, i64 %index.next167.4
  %22 = bitcast i8* %next.gep175.5 to <16 x i8>*, !dbg !285
  %wide.load243.5 = load <16 x i8>, <16 x i8>* %22, align 1, !dbg !285, !tbaa !263
  %23 = bitcast i8* %next.gep207.5 to <16 x i8>*, !dbg !287
  store <16 x i8> %wide.load243.5, <16 x i8>* %23, align 1, !dbg !287, !tbaa !263
  %index.next167.5 = add i64 %index166, 96, !dbg !284
  %next.gep175.6 = getelementptr i8, i8* %src, i64 %index.next167.5
  %next.gep207.6 = getelementptr i8, i8* %dst, i64 %index.next167.5
  %24 = bitcast i8* %next.gep175.6 to <16 x i8>*, !dbg !285
  %wide.load243.6 = load <16 x i8>, <16 x i8>* %24, align 1, !dbg !285, !tbaa !263
  %25 = bitcast i8* %next.gep207.6 to <16 x i8>*, !dbg !287
  store <16 x i8> %wide.load243.6, <16 x i8>* %25, align 1, !dbg !287, !tbaa !263
  %index.next167.6 = add i64 %index166, 112, !dbg !284
  %next.gep175.7 = getelementptr i8, i8* %src, i64 %index.next167.6
  %next.gep207.7 = getelementptr i8, i8* %dst, i64 %index.next167.6
  %26 = bitcast i8* %next.gep175.7 to <16 x i8>*, !dbg !285
  %wide.load243.7 = load <16 x i8>, <16 x i8>* %26, align 1, !dbg !285, !tbaa !263
  %27 = bitcast i8* %next.gep207.7 to <16 x i8>*, !dbg !287
  store <16 x i8> %wide.load243.7, <16 x i8>* %27, align 1, !dbg !287, !tbaa !263
  %index.next167.7 = add i64 %index166, 128, !dbg !284
  %28 = icmp eq i64 %index.next167.7, %n.vec155, !dbg !284
  br i1 %28, label %middle.block150, label %vector.body149, !dbg !284, !llvm.loop !289

middle.block150:                                  ; preds = %vector.body149, %vector.body149.preheader.split
  %cmp.n174 = icmp eq i64 %n.vec155, %count
  br i1 %cmp.n174, label %.loopexit, label %.lr.ph.preheader244, !dbg !284

.lr.ph.preheader244:                              ; preds = %middle.block150, %vector.memcheck164, %min.iters.checked153, %.lr.ph.preheader
  %b.05.ph = phi i8* [ %src, %vector.memcheck164 ], [ %src, %min.iters.checked153 ], [ %src, %.lr.ph.preheader ], [ %ind.end169, %middle.block150 ]
  %a.04.ph = phi i8* [ %dst, %vector.memcheck164 ], [ %dst, %min.iters.checked153 ], [ %dst, %.lr.ph.preheader ], [ %ind.end171, %middle.block150 ]
  %.03.ph = phi i64 [ %count, %vector.memcheck164 ], [ %count, %min.iters.checked153 ], [ %count, %.lr.ph.preheader ], [ %ind.end173, %middle.block150 ]
  %29 = add i64 %.03.ph, -1, !dbg !284
  %xtraiter = and i64 %.03.ph, 7, !dbg !284
  %lcmp.mod = icmp eq i64 %xtraiter, 0, !dbg !284
  br i1 %lcmp.mod, label %.lr.ph.preheader244.split, label %.lr.ph.prol, !dbg !284

.lr.ph.prol:                                      ; preds = %.lr.ph.preheader244, %.lr.ph.prol
  %b.05.prol = phi i8* [ %31, %.lr.ph.prol ], [ %b.05.ph, %.lr.ph.preheader244 ]
  %a.04.prol = phi i8* [ %33, %.lr.ph.prol ], [ %a.04.ph, %.lr.ph.preheader244 ]
  %.03.prol = phi i64 [ %30, %.lr.ph.prol ], [ %.03.ph, %.lr.ph.preheader244 ]
  %prol.iter = phi i64 [ %prol.iter.sub, %.lr.ph.prol ], [ %xtraiter, %.lr.ph.preheader244 ]
  %30 = add i64 %.03.prol, -1, !dbg !284
  %31 = getelementptr inbounds i8, i8* %b.05.prol, i64 1, !dbg !290
  %32 = load i8, i8* %b.05.prol, align 1, !dbg !285, !tbaa !263
  %33 = getelementptr inbounds i8, i8* %a.04.prol, i64 1, !dbg !291
  store i8 %32, i8* %a.04.prol, align 1, !dbg !287, !tbaa !263
  %prol.iter.sub = add i64 %prol.iter, -1, !dbg !281
  %prol.iter.cmp = icmp eq i64 %prol.iter.sub, 0, !dbg !281
  br i1 %prol.iter.cmp, label %.lr.ph.preheader244.split, label %.lr.ph.prol, !dbg !281, !llvm.loop !292

.lr.ph.preheader244.split:                        ; preds = %.lr.ph.prol, %.lr.ph.preheader244
  %b.05.unr = phi i8* [ %b.05.ph, %.lr.ph.preheader244 ], [ %31, %.lr.ph.prol ]
  %a.04.unr = phi i8* [ %a.04.ph, %.lr.ph.preheader244 ], [ %33, %.lr.ph.prol ]
  %.03.unr = phi i64 [ %.03.ph, %.lr.ph.preheader244 ], [ %30, %.lr.ph.prol ]
  %34 = icmp ult i64 %29, 7, !dbg !284
  br i1 %34, label %.loopexit, label %.lr.ph, !dbg !284

.lr.ph:                                           ; preds = %.lr.ph.preheader244.split, %.lr.ph
  %b.05 = phi i8* [ %57, %.lr.ph ], [ %b.05.unr, %.lr.ph.preheader244.split ]
  %a.04 = phi i8* [ %59, %.lr.ph ], [ %a.04.unr, %.lr.ph.preheader244.split ]
  %.03 = phi i64 [ %56, %.lr.ph ], [ %.03.unr, %.lr.ph.preheader244.split ]
  %35 = getelementptr inbounds i8, i8* %b.05, i64 1, !dbg !290
  %36 = load i8, i8* %b.05, align 1, !dbg !285, !tbaa !263
  %37 = getelementptr inbounds i8, i8* %a.04, i64 1, !dbg !291
  store i8 %36, i8* %a.04, align 1, !dbg !287, !tbaa !263
  %38 = getelementptr inbounds i8, i8* %b.05, i64 2, !dbg !290
  %39 = load i8, i8* %35, align 1, !dbg !285, !tbaa !263
  %40 = getelementptr inbounds i8, i8* %a.04, i64 2, !dbg !291
  store i8 %39, i8* %37, align 1, !dbg !287, !tbaa !263
  %41 = getelementptr inbounds i8, i8* %b.05, i64 3, !dbg !290
  %42 = load i8, i8* %38, align 1, !dbg !285, !tbaa !263
  %43 = getelementptr inbounds i8, i8* %a.04, i64 3, !dbg !291
  store i8 %42, i8* %40, align 1, !dbg !287, !tbaa !263
  %44 = getelementptr inbounds i8, i8* %b.05, i64 4, !dbg !290
  %45 = load i8, i8* %41, align 1, !dbg !285, !tbaa !263
  %46 = getelementptr inbounds i8, i8* %a.04, i64 4, !dbg !291
  store i8 %45, i8* %43, align 1, !dbg !287, !tbaa !263
  %47 = getelementptr inbounds i8, i8* %b.05, i64 5, !dbg !290
  %48 = load i8, i8* %44, align 1, !dbg !285, !tbaa !263
  %49 = getelementptr inbounds i8, i8* %a.04, i64 5, !dbg !291
  store i8 %48, i8* %46, align 1, !dbg !287, !tbaa !263
  %50 = getelementptr inbounds i8, i8* %b.05, i64 6, !dbg !290
  %51 = load i8, i8* %47, align 1, !dbg !285, !tbaa !263
  %52 = getelementptr inbounds i8, i8* %a.04, i64 6, !dbg !291
  store i8 %51, i8* %49, align 1, !dbg !287, !tbaa !263
  %53 = getelementptr inbounds i8, i8* %b.05, i64 7, !dbg !290
  %54 = load i8, i8* %50, align 1, !dbg !285, !tbaa !263
  %55 = getelementptr inbounds i8, i8* %a.04, i64 7, !dbg !291
  store i8 %54, i8* %52, align 1, !dbg !287, !tbaa !263
  %56 = add i64 %.03, -8, !dbg !284
  %57 = getelementptr inbounds i8, i8* %b.05, i64 8, !dbg !290
  %58 = load i8, i8* %53, align 1, !dbg !285, !tbaa !263
  %59 = getelementptr inbounds i8, i8* %a.04, i64 8, !dbg !291
  store i8 %58, i8* %55, align 1, !dbg !287, !tbaa !263
  %60 = icmp eq i64 %56, 0, !dbg !281
  br i1 %60, label %.loopexit, label %.lr.ph, !dbg !281, !llvm.loop !293

; <label>:61                                      ; preds = %2
  %62 = add i64 %count, -1, !dbg !294
  %63 = icmp eq i64 %count, 0, !dbg !296
  br i1 %63, label %.loopexit, label %.lr.ph9.preheader, !dbg !296

.lr.ph9.preheader:                                ; preds = %61
  %64 = getelementptr inbounds i8, i8* %src, i64 %62, !dbg !298
  %65 = getelementptr inbounds i8, i8* %dst, i64 %62, !dbg !299
  %min.iters.check = icmp ult i64 %count, 32, !dbg !300
  br i1 %min.iters.check, label %.lr.ph9.preheader245, label %min.iters.checked, !dbg !300

min.iters.checked:                                ; preds = %.lr.ph9.preheader
  %n.vec = and i64 %count, -32, !dbg !300
  %cmp.zero = icmp eq i64 %n.vec, 0, !dbg !300
  br i1 %cmp.zero, label %.lr.ph9.preheader245, label %vector.memcheck, !dbg !300

vector.memcheck:                                  ; preds = %min.iters.checked
  %66 = add i64 %count, -1, !dbg !300
  %scevgep = getelementptr i8, i8* %dst, i64 %66, !dbg !300
  %scevgep13 = getelementptr i8, i8* %src, i64 %66, !dbg !300
  %bound0 = icmp uge i8* %scevgep13, %dst, !dbg !300
  %bound1 = icmp uge i8* %scevgep, %src, !dbg !300
  %memcheck.conflict = and i1 %bound0, %bound1, !dbg !300
  %67 = sub i64 0, %n.vec, !dbg !300
  %ind.end = getelementptr i8, i8* %64, i64 %67, !dbg !300
  %68 = sub i64 0, %n.vec, !dbg !300
  %ind.end15 = getelementptr i8, i8* %65, i64 %68, !dbg !300
  %ind.end17 = sub i64 %count, %n.vec, !dbg !300
  br i1 %memcheck.conflict, label %.lr.ph9.preheader245, label %vector.body.preheader, !dbg !300

vector.body.preheader:                            ; preds = %vector.memcheck
  %69 = add i64 %count, -32
  %70 = lshr i64 %69, 5
  %71 = and i64 %70, 1
  %lcmp.mod254 = icmp eq i64 %71, 0
  br i1 %lcmp.mod254, label %vector.body.prol, label %vector.body.preheader.split

vector.body.prol:                                 ; preds = %vector.body.preheader
  %72 = getelementptr i8, i8* %64, i64 -15, !dbg !301
  %73 = bitcast i8* %72 to <16 x i8>*, !dbg !301
  %wide.load.prol = load <16 x i8>, <16 x i8>* %73, align 1, !dbg !301, !tbaa !263
  %74 = getelementptr i8, i8* %64, i64 -16, !dbg !301
  %75 = getelementptr i8, i8* %74, i64 -15, !dbg !301
  %76 = bitcast i8* %75 to <16 x i8>*, !dbg !301
  %wide.load145.prol = load <16 x i8>, <16 x i8>* %76, align 1, !dbg !301, !tbaa !263
  %77 = getelementptr i8, i8* %65, i64 -15, !dbg !303
  %78 = bitcast i8* %77 to <16 x i8>*, !dbg !303
  store <16 x i8> %wide.load.prol, <16 x i8>* %78, align 1, !dbg !303, !tbaa !263
  %79 = getelementptr i8, i8* %65, i64 -16, !dbg !303
  %80 = getelementptr i8, i8* %79, i64 -15, !dbg !303
  %81 = bitcast i8* %80 to <16 x i8>*, !dbg !303
  store <16 x i8> %wide.load145.prol, <16 x i8>* %81, align 1, !dbg !303, !tbaa !263
  br label %vector.body.preheader.split, !dbg !300

vector.body.preheader.split:                      ; preds = %vector.body.prol, %vector.body.preheader
  %index.unr = phi i64 [ 0, %vector.body.preheader ], [ 32, %vector.body.prol ]
  %82 = icmp eq i64 %70, 0
  br i1 %82, label %middle.block, label %vector.body

vector.body:                                      ; preds = %vector.body.preheader.split, %vector.body
  %index = phi i64 [ %index.next.1, %vector.body ], [ %index.unr, %vector.body.preheader.split ]
  %83 = sub i64 0, %index
  %next.gep = getelementptr i8, i8* %64, i64 %83
  %84 = sub i64 0, %index
  %next.gep80 = getelementptr i8, i8* %65, i64 %84
  %85 = getelementptr i8, i8* %next.gep, i64 -15, !dbg !301
  %86 = bitcast i8* %85 to <16 x i8>*, !dbg !301
  %wide.load = load <16 x i8>, <16 x i8>* %86, align 1, !dbg !301, !tbaa !263
  %87 = getelementptr i8, i8* %next.gep, i64 -16, !dbg !301
  %88 = getelementptr i8, i8* %87, i64 -15, !dbg !301
  %89 = bitcast i8* %88 to <16 x i8>*, !dbg !301
  %wide.load145 = load <16 x i8>, <16 x i8>* %89, align 1, !dbg !301, !tbaa !263
  %90 = getelementptr i8, i8* %next.gep80, i64 -15, !dbg !303
  %91 = bitcast i8* %90 to <16 x i8>*, !dbg !303
  store <16 x i8> %wide.load, <16 x i8>* %91, align 1, !dbg !303, !tbaa !263
  %92 = getelementptr i8, i8* %next.gep80, i64 -16, !dbg !303
  %93 = getelementptr i8, i8* %92, i64 -15, !dbg !303
  %94 = bitcast i8* %93 to <16 x i8>*, !dbg !303
  store <16 x i8> %wide.load145, <16 x i8>* %94, align 1, !dbg !303, !tbaa !263
  %95 = sub i64 -32, %index
  %next.gep.1 = getelementptr i8, i8* %64, i64 %95
  %96 = sub i64 -32, %index
  %next.gep80.1 = getelementptr i8, i8* %65, i64 %96
  %97 = getelementptr i8, i8* %next.gep.1, i64 -15, !dbg !301
  %98 = bitcast i8* %97 to <16 x i8>*, !dbg !301
  %wide.load.1 = load <16 x i8>, <16 x i8>* %98, align 1, !dbg !301, !tbaa !263
  %99 = getelementptr i8, i8* %next.gep.1, i64 -16, !dbg !301
  %100 = getelementptr i8, i8* %99, i64 -15, !dbg !301
  %101 = bitcast i8* %100 to <16 x i8>*, !dbg !301
  %wide.load145.1 = load <16 x i8>, <16 x i8>* %101, align 1, !dbg !301, !tbaa !263
  %102 = getelementptr i8, i8* %next.gep80.1, i64 -15, !dbg !303
  %103 = bitcast i8* %102 to <16 x i8>*, !dbg !303
  store <16 x i8> %wide.load.1, <16 x i8>* %103, align 1, !dbg !303, !tbaa !263
  %104 = getelementptr i8, i8* %next.gep80.1, i64 -16, !dbg !303
  %105 = getelementptr i8, i8* %104, i64 -15, !dbg !303
  %106 = bitcast i8* %105 to <16 x i8>*, !dbg !303
  store <16 x i8> %wide.load145.1, <16 x i8>* %106, align 1, !dbg !303, !tbaa !263
  %index.next.1 = add i64 %index, 64, !dbg !300
  %107 = icmp eq i64 %index.next.1, %n.vec, !dbg !300
  br i1 %107, label %middle.block, label %vector.body, !dbg !300, !llvm.loop !304

middle.block:                                     ; preds = %vector.body, %vector.body.preheader.split
  %cmp.n = icmp eq i64 %n.vec, %count
  br i1 %cmp.n, label %.loopexit, label %.lr.ph9.preheader245, !dbg !300

.lr.ph9.preheader245:                             ; preds = %middle.block, %vector.memcheck, %min.iters.checked, %.lr.ph9.preheader
  %b.18.ph = phi i8* [ %64, %vector.memcheck ], [ %64, %min.iters.checked ], [ %64, %.lr.ph9.preheader ], [ %ind.end, %middle.block ]
  %a.17.ph = phi i8* [ %65, %vector.memcheck ], [ %65, %min.iters.checked ], [ %65, %.lr.ph9.preheader ], [ %ind.end15, %middle.block ]
  %.16.ph = phi i64 [ %count, %vector.memcheck ], [ %count, %min.iters.checked ], [ %count, %.lr.ph9.preheader ], [ %ind.end17, %middle.block ]
  %108 = add i64 %.16.ph, -1, !dbg !300
  %xtraiter250 = and i64 %.16.ph, 7, !dbg !300
  %lcmp.mod251 = icmp eq i64 %xtraiter250, 0, !dbg !300
  br i1 %lcmp.mod251, label %.lr.ph9.preheader245.split, label %.lr.ph9.prol, !dbg !300

.lr.ph9.prol:                                     ; preds = %.lr.ph9.preheader245, %.lr.ph9.prol
  %b.18.prol = phi i8* [ %110, %.lr.ph9.prol ], [ %b.18.ph, %.lr.ph9.preheader245 ]
  %a.17.prol = phi i8* [ %112, %.lr.ph9.prol ], [ %a.17.ph, %.lr.ph9.preheader245 ]
  %.16.prol = phi i64 [ %109, %.lr.ph9.prol ], [ %.16.ph, %.lr.ph9.preheader245 ]
  %prol.iter252 = phi i64 [ %prol.iter252.sub, %.lr.ph9.prol ], [ %xtraiter250, %.lr.ph9.preheader245 ]
  %109 = add i64 %.16.prol, -1, !dbg !300
  %110 = getelementptr inbounds i8, i8* %b.18.prol, i64 -1, !dbg !305
  %111 = load i8, i8* %b.18.prol, align 1, !dbg !301, !tbaa !263
  %112 = getelementptr inbounds i8, i8* %a.17.prol, i64 -1, !dbg !306
  store i8 %111, i8* %a.17.prol, align 1, !dbg !303, !tbaa !263
  %prol.iter252.sub = add i64 %prol.iter252, -1, !dbg !296
  %prol.iter252.cmp = icmp eq i64 %prol.iter252.sub, 0, !dbg !296
  br i1 %prol.iter252.cmp, label %.lr.ph9.preheader245.split, label %.lr.ph9.prol, !dbg !296, !llvm.loop !307

.lr.ph9.preheader245.split:                       ; preds = %.lr.ph9.prol, %.lr.ph9.preheader245
  %b.18.unr = phi i8* [ %b.18.ph, %.lr.ph9.preheader245 ], [ %110, %.lr.ph9.prol ]
  %a.17.unr = phi i8* [ %a.17.ph, %.lr.ph9.preheader245 ], [ %112, %.lr.ph9.prol ]
  %.16.unr = phi i64 [ %.16.ph, %.lr.ph9.preheader245 ], [ %109, %.lr.ph9.prol ]
  %113 = icmp ult i64 %108, 7, !dbg !300
  br i1 %113, label %.loopexit, label %.lr.ph9, !dbg !300

.lr.ph9:                                          ; preds = %.lr.ph9.preheader245.split, %.lr.ph9
  %b.18 = phi i8* [ %136, %.lr.ph9 ], [ %b.18.unr, %.lr.ph9.preheader245.split ]
  %a.17 = phi i8* [ %138, %.lr.ph9 ], [ %a.17.unr, %.lr.ph9.preheader245.split ]
  %.16 = phi i64 [ %135, %.lr.ph9 ], [ %.16.unr, %.lr.ph9.preheader245.split ]
  %114 = getelementptr inbounds i8, i8* %b.18, i64 -1, !dbg !305
  %115 = load i8, i8* %b.18, align 1, !dbg !301, !tbaa !263
  %116 = getelementptr inbounds i8, i8* %a.17, i64 -1, !dbg !306
  store i8 %115, i8* %a.17, align 1, !dbg !303, !tbaa !263
  %117 = getelementptr inbounds i8, i8* %b.18, i64 -2, !dbg !305
  %118 = load i8, i8* %114, align 1, !dbg !301, !tbaa !263
  %119 = getelementptr inbounds i8, i8* %a.17, i64 -2, !dbg !306
  store i8 %118, i8* %116, align 1, !dbg !303, !tbaa !263
  %120 = getelementptr inbounds i8, i8* %b.18, i64 -3, !dbg !305
  %121 = load i8, i8* %117, align 1, !dbg !301, !tbaa !263
  %122 = getelementptr inbounds i8, i8* %a.17, i64 -3, !dbg !306
  store i8 %121, i8* %119, align 1, !dbg !303, !tbaa !263
  %123 = getelementptr inbounds i8, i8* %b.18, i64 -4, !dbg !305
  %124 = load i8, i8* %120, align 1, !dbg !301, !tbaa !263
  %125 = getelementptr inbounds i8, i8* %a.17, i64 -4, !dbg !306
  store i8 %124, i8* %122, align 1, !dbg !303, !tbaa !263
  %126 = getelementptr inbounds i8, i8* %b.18, i64 -5, !dbg !305
  %127 = load i8, i8* %123, align 1, !dbg !301, !tbaa !263
  %128 = getelementptr inbounds i8, i8* %a.17, i64 -5, !dbg !306
  store i8 %127, i8* %125, align 1, !dbg !303, !tbaa !263
  %129 = getelementptr inbounds i8, i8* %b.18, i64 -6, !dbg !305
  %130 = load i8, i8* %126, align 1, !dbg !301, !tbaa !263
  %131 = getelementptr inbounds i8, i8* %a.17, i64 -6, !dbg !306
  store i8 %130, i8* %128, align 1, !dbg !303, !tbaa !263
  %132 = getelementptr inbounds i8, i8* %b.18, i64 -7, !dbg !305
  %133 = load i8, i8* %129, align 1, !dbg !301, !tbaa !263
  %134 = getelementptr inbounds i8, i8* %a.17, i64 -7, !dbg !306
  store i8 %133, i8* %131, align 1, !dbg !303, !tbaa !263
  %135 = add i64 %.16, -8, !dbg !300
  %136 = getelementptr inbounds i8, i8* %b.18, i64 -8, !dbg !305
  %137 = load i8, i8* %132, align 1, !dbg !301, !tbaa !263
  %138 = getelementptr inbounds i8, i8* %a.17, i64 -8, !dbg !306
  store i8 %137, i8* %134, align 1, !dbg !303, !tbaa !263
  %139 = icmp eq i64 %135, 0, !dbg !296
  br i1 %139, label %.loopexit, label %.lr.ph9, !dbg !296, !llvm.loop !308

.loopexit:                                        ; preds = %.lr.ph9.preheader245.split, %.lr.ph9, %.lr.ph.preheader244.split, %.lr.ph, %middle.block, %middle.block150, %61, %.preheader, %0
  ret i8* %dst, !dbg !309
}

; Function Attrs: norecurse nounwind uwtable
define weak i8* @mempcpy(i8* %destaddr, i8* %srcaddr, i64 %len) #7 !dbg !96 {
  %1 = icmp eq i64 %len, 0, !dbg !310
  br i1 %1, label %._crit_edge, label %.lr.ph.preheader, !dbg !312

.lr.ph.preheader:                                 ; preds = %0
  %min.iters.check = icmp ult i64 %len, 16, !dbg !313
  br i1 %min.iters.check, label %.lr.ph.preheader72, label %min.iters.checked, !dbg !313

min.iters.checked:                                ; preds = %.lr.ph.preheader
  %n.vec = and i64 %len, -16, !dbg !313
  %cmp.zero = icmp eq i64 %n.vec, 0, !dbg !313
  br i1 %cmp.zero, label %.lr.ph.preheader72, label %vector.memcheck, !dbg !313

vector.memcheck:                                  ; preds = %min.iters.checked
  %2 = add i64 %len, -1, !dbg !313
  %scevgep4 = getelementptr i8, i8* %destaddr, i64 %2, !dbg !313
  %scevgep5 = getelementptr i8, i8* %srcaddr, i64 %2, !dbg !313
  %bound0 = icmp uge i8* %scevgep5, %destaddr, !dbg !313
  %bound1 = icmp uge i8* %scevgep4, %srcaddr, !dbg !313
  %memcheck.conflict = and i1 %bound0, %bound1, !dbg !313
  %ind.end = getelementptr i8, i8* %srcaddr, i64 %n.vec, !dbg !313
  %ind.end7 = getelementptr i8, i8* %destaddr, i64 %n.vec, !dbg !313
  %ind.end9 = sub i64 %len, %n.vec, !dbg !313
  br i1 %memcheck.conflict, label %.lr.ph.preheader72, label %vector.body.preheader, !dbg !313

vector.body.preheader:                            ; preds = %vector.memcheck
  %3 = add i64 %len, -16
  %4 = lshr i64 %3, 4
  %5 = add nuw nsw i64 %4, 1
  %xtraiter73 = and i64 %5, 7
  %lcmp.mod74 = icmp eq i64 %xtraiter73, 0
  br i1 %lcmp.mod74, label %vector.body.preheader.split, label %vector.body.prol

vector.body.prol:                                 ; preds = %vector.body.preheader, %vector.body.prol
  %index.prol = phi i64 [ %index.next.prol, %vector.body.prol ], [ 0, %vector.body.preheader ]
  %prol.iter75 = phi i64 [ %prol.iter75.sub, %vector.body.prol ], [ %xtraiter73, %vector.body.preheader ]
  %next.gep.prol = getelementptr i8, i8* %srcaddr, i64 %index.prol
  %next.gep40.prol = getelementptr i8, i8* %destaddr, i64 %index.prol
  %6 = bitcast i8* %next.gep.prol to <16 x i8>*, !dbg !314
  %wide.load.prol = load <16 x i8>, <16 x i8>* %6, align 1, !dbg !314, !tbaa !263
  %7 = bitcast i8* %next.gep40.prol to <16 x i8>*, !dbg !315
  store <16 x i8> %wide.load.prol, <16 x i8>* %7, align 1, !dbg !315, !tbaa !263
  %index.next.prol = add i64 %index.prol, 16, !dbg !313
  %prol.iter75.sub = add i64 %prol.iter75, -1, !dbg !313
  %prol.iter75.cmp = icmp eq i64 %prol.iter75.sub, 0, !dbg !313
  br i1 %prol.iter75.cmp, label %vector.body.preheader.split, label %vector.body.prol, !dbg !313, !llvm.loop !316

vector.body.preheader.split:                      ; preds = %vector.body.prol, %vector.body.preheader
  %index.unr = phi i64 [ 0, %vector.body.preheader ], [ %index.next.prol, %vector.body.prol ]
  %8 = icmp ult i64 %3, 112
  br i1 %8, label %middle.block, label %vector.body

vector.body:                                      ; preds = %vector.body.preheader.split, %vector.body
  %index = phi i64 [ %index.next.7, %vector.body ], [ %index.unr, %vector.body.preheader.split ]
  %next.gep = getelementptr i8, i8* %srcaddr, i64 %index
  %next.gep40 = getelementptr i8, i8* %destaddr, i64 %index
  %9 = bitcast i8* %next.gep to <16 x i8>*, !dbg !314
  %wide.load = load <16 x i8>, <16 x i8>* %9, align 1, !dbg !314, !tbaa !263
  %10 = bitcast i8* %next.gep40 to <16 x i8>*, !dbg !315
  store <16 x i8> %wide.load, <16 x i8>* %10, align 1, !dbg !315, !tbaa !263
  %index.next = add i64 %index, 16, !dbg !313
  %next.gep.1 = getelementptr i8, i8* %srcaddr, i64 %index.next
  %next.gep40.1 = getelementptr i8, i8* %destaddr, i64 %index.next
  %11 = bitcast i8* %next.gep.1 to <16 x i8>*, !dbg !314
  %wide.load.1 = load <16 x i8>, <16 x i8>* %11, align 1, !dbg !314, !tbaa !263
  %12 = bitcast i8* %next.gep40.1 to <16 x i8>*, !dbg !315
  store <16 x i8> %wide.load.1, <16 x i8>* %12, align 1, !dbg !315, !tbaa !263
  %index.next.1 = add i64 %index, 32, !dbg !313
  %next.gep.2 = getelementptr i8, i8* %srcaddr, i64 %index.next.1
  %next.gep40.2 = getelementptr i8, i8* %destaddr, i64 %index.next.1
  %13 = bitcast i8* %next.gep.2 to <16 x i8>*, !dbg !314
  %wide.load.2 = load <16 x i8>, <16 x i8>* %13, align 1, !dbg !314, !tbaa !263
  %14 = bitcast i8* %next.gep40.2 to <16 x i8>*, !dbg !315
  store <16 x i8> %wide.load.2, <16 x i8>* %14, align 1, !dbg !315, !tbaa !263
  %index.next.2 = add i64 %index, 48, !dbg !313
  %next.gep.3 = getelementptr i8, i8* %srcaddr, i64 %index.next.2
  %next.gep40.3 = getelementptr i8, i8* %destaddr, i64 %index.next.2
  %15 = bitcast i8* %next.gep.3 to <16 x i8>*, !dbg !314
  %wide.load.3 = load <16 x i8>, <16 x i8>* %15, align 1, !dbg !314, !tbaa !263
  %16 = bitcast i8* %next.gep40.3 to <16 x i8>*, !dbg !315
  store <16 x i8> %wide.load.3, <16 x i8>* %16, align 1, !dbg !315, !tbaa !263
  %index.next.3 = add i64 %index, 64, !dbg !313
  %next.gep.4 = getelementptr i8, i8* %srcaddr, i64 %index.next.3
  %next.gep40.4 = getelementptr i8, i8* %destaddr, i64 %index.next.3
  %17 = bitcast i8* %next.gep.4 to <16 x i8>*, !dbg !314
  %wide.load.4 = load <16 x i8>, <16 x i8>* %17, align 1, !dbg !314, !tbaa !263
  %18 = bitcast i8* %next.gep40.4 to <16 x i8>*, !dbg !315
  store <16 x i8> %wide.load.4, <16 x i8>* %18, align 1, !dbg !315, !tbaa !263
  %index.next.4 = add i64 %index, 80, !dbg !313
  %next.gep.5 = getelementptr i8, i8* %srcaddr, i64 %index.next.4
  %next.gep40.5 = getelementptr i8, i8* %destaddr, i64 %index.next.4
  %19 = bitcast i8* %next.gep.5 to <16 x i8>*, !dbg !314
  %wide.load.5 = load <16 x i8>, <16 x i8>* %19, align 1, !dbg !314, !tbaa !263
  %20 = bitcast i8* %next.gep40.5 to <16 x i8>*, !dbg !315
  store <16 x i8> %wide.load.5, <16 x i8>* %20, align 1, !dbg !315, !tbaa !263
  %index.next.5 = add i64 %index, 96, !dbg !313
  %next.gep.6 = getelementptr i8, i8* %srcaddr, i64 %index.next.5
  %next.gep40.6 = getelementptr i8, i8* %destaddr, i64 %index.next.5
  %21 = bitcast i8* %next.gep.6 to <16 x i8>*, !dbg !314
  %wide.load.6 = load <16 x i8>, <16 x i8>* %21, align 1, !dbg !314, !tbaa !263
  %22 = bitcast i8* %next.gep40.6 to <16 x i8>*, !dbg !315
  store <16 x i8> %wide.load.6, <16 x i8>* %22, align 1, !dbg !315, !tbaa !263
  %index.next.6 = add i64 %index, 112, !dbg !313
  %next.gep.7 = getelementptr i8, i8* %srcaddr, i64 %index.next.6
  %next.gep40.7 = getelementptr i8, i8* %destaddr, i64 %index.next.6
  %23 = bitcast i8* %next.gep.7 to <16 x i8>*, !dbg !314
  %wide.load.7 = load <16 x i8>, <16 x i8>* %23, align 1, !dbg !314, !tbaa !263
  %24 = bitcast i8* %next.gep40.7 to <16 x i8>*, !dbg !315
  store <16 x i8> %wide.load.7, <16 x i8>* %24, align 1, !dbg !315, !tbaa !263
  %index.next.7 = add i64 %index, 128, !dbg !313
  %25 = icmp eq i64 %index.next.7, %n.vec, !dbg !313
  br i1 %25, label %middle.block, label %vector.body, !dbg !313, !llvm.loop !317

middle.block:                                     ; preds = %vector.body, %vector.body.preheader.split
  %cmp.n = icmp eq i64 %n.vec, %len
  br i1 %cmp.n, label %._crit_edge.loopexit, label %.lr.ph.preheader72, !dbg !313

.lr.ph.preheader72:                               ; preds = %middle.block, %vector.memcheck, %min.iters.checked, %.lr.ph.preheader
  %src.03.ph = phi i8* [ %srcaddr, %vector.memcheck ], [ %srcaddr, %min.iters.checked ], [ %srcaddr, %.lr.ph.preheader ], [ %ind.end, %middle.block ]
  %dest.02.ph = phi i8* [ %destaddr, %vector.memcheck ], [ %destaddr, %min.iters.checked ], [ %destaddr, %.lr.ph.preheader ], [ %ind.end7, %middle.block ]
  %.01.ph = phi i64 [ %len, %vector.memcheck ], [ %len, %min.iters.checked ], [ %len, %.lr.ph.preheader ], [ %ind.end9, %middle.block ]
  %26 = add i64 %.01.ph, -1, !dbg !313
  %xtraiter = and i64 %.01.ph, 7, !dbg !313
  %lcmp.mod = icmp eq i64 %xtraiter, 0, !dbg !313
  br i1 %lcmp.mod, label %.lr.ph.preheader72.split, label %.lr.ph.prol, !dbg !313

.lr.ph.prol:                                      ; preds = %.lr.ph.preheader72, %.lr.ph.prol
  %src.03.prol = phi i8* [ %28, %.lr.ph.prol ], [ %src.03.ph, %.lr.ph.preheader72 ]
  %dest.02.prol = phi i8* [ %30, %.lr.ph.prol ], [ %dest.02.ph, %.lr.ph.preheader72 ]
  %.01.prol = phi i64 [ %27, %.lr.ph.prol ], [ %.01.ph, %.lr.ph.preheader72 ]
  %prol.iter = phi i64 [ %prol.iter.sub, %.lr.ph.prol ], [ %xtraiter, %.lr.ph.preheader72 ]
  %27 = add i64 %.01.prol, -1, !dbg !313
  %28 = getelementptr inbounds i8, i8* %src.03.prol, i64 1, !dbg !318
  %29 = load i8, i8* %src.03.prol, align 1, !dbg !314, !tbaa !263
  %30 = getelementptr inbounds i8, i8* %dest.02.prol, i64 1, !dbg !319
  store i8 %29, i8* %dest.02.prol, align 1, !dbg !315, !tbaa !263
  %prol.iter.sub = add i64 %prol.iter, -1, !dbg !312
  %prol.iter.cmp = icmp eq i64 %prol.iter.sub, 0, !dbg !312
  br i1 %prol.iter.cmp, label %.lr.ph.preheader72.split, label %.lr.ph.prol, !dbg !312, !llvm.loop !320

.lr.ph.preheader72.split:                         ; preds = %.lr.ph.prol, %.lr.ph.preheader72
  %src.03.unr = phi i8* [ %src.03.ph, %.lr.ph.preheader72 ], [ %28, %.lr.ph.prol ]
  %dest.02.unr = phi i8* [ %dest.02.ph, %.lr.ph.preheader72 ], [ %30, %.lr.ph.prol ]
  %.01.unr = phi i64 [ %.01.ph, %.lr.ph.preheader72 ], [ %27, %.lr.ph.prol ]
  %31 = icmp ult i64 %26, 7, !dbg !313
  br i1 %31, label %._crit_edge.loopexit, label %.lr.ph, !dbg !313

.lr.ph:                                           ; preds = %.lr.ph.preheader72.split, %.lr.ph
  %src.03 = phi i8* [ %54, %.lr.ph ], [ %src.03.unr, %.lr.ph.preheader72.split ]
  %dest.02 = phi i8* [ %56, %.lr.ph ], [ %dest.02.unr, %.lr.ph.preheader72.split ]
  %.01 = phi i64 [ %53, %.lr.ph ], [ %.01.unr, %.lr.ph.preheader72.split ]
  %32 = getelementptr inbounds i8, i8* %src.03, i64 1, !dbg !318
  %33 = load i8, i8* %src.03, align 1, !dbg !314, !tbaa !263
  %34 = getelementptr inbounds i8, i8* %dest.02, i64 1, !dbg !319
  store i8 %33, i8* %dest.02, align 1, !dbg !315, !tbaa !263
  %35 = getelementptr inbounds i8, i8* %src.03, i64 2, !dbg !318
  %36 = load i8, i8* %32, align 1, !dbg !314, !tbaa !263
  %37 = getelementptr inbounds i8, i8* %dest.02, i64 2, !dbg !319
  store i8 %36, i8* %34, align 1, !dbg !315, !tbaa !263
  %38 = getelementptr inbounds i8, i8* %src.03, i64 3, !dbg !318
  %39 = load i8, i8* %35, align 1, !dbg !314, !tbaa !263
  %40 = getelementptr inbounds i8, i8* %dest.02, i64 3, !dbg !319
  store i8 %39, i8* %37, align 1, !dbg !315, !tbaa !263
  %41 = getelementptr inbounds i8, i8* %src.03, i64 4, !dbg !318
  %42 = load i8, i8* %38, align 1, !dbg !314, !tbaa !263
  %43 = getelementptr inbounds i8, i8* %dest.02, i64 4, !dbg !319
  store i8 %42, i8* %40, align 1, !dbg !315, !tbaa !263
  %44 = getelementptr inbounds i8, i8* %src.03, i64 5, !dbg !318
  %45 = load i8, i8* %41, align 1, !dbg !314, !tbaa !263
  %46 = getelementptr inbounds i8, i8* %dest.02, i64 5, !dbg !319
  store i8 %45, i8* %43, align 1, !dbg !315, !tbaa !263
  %47 = getelementptr inbounds i8, i8* %src.03, i64 6, !dbg !318
  %48 = load i8, i8* %44, align 1, !dbg !314, !tbaa !263
  %49 = getelementptr inbounds i8, i8* %dest.02, i64 6, !dbg !319
  store i8 %48, i8* %46, align 1, !dbg !315, !tbaa !263
  %50 = getelementptr inbounds i8, i8* %src.03, i64 7, !dbg !318
  %51 = load i8, i8* %47, align 1, !dbg !314, !tbaa !263
  %52 = getelementptr inbounds i8, i8* %dest.02, i64 7, !dbg !319
  store i8 %51, i8* %49, align 1, !dbg !315, !tbaa !263
  %53 = add i64 %.01, -8, !dbg !313
  %54 = getelementptr inbounds i8, i8* %src.03, i64 8, !dbg !318
  %55 = load i8, i8* %50, align 1, !dbg !314, !tbaa !263
  %56 = getelementptr inbounds i8, i8* %dest.02, i64 8, !dbg !319
  store i8 %55, i8* %52, align 1, !dbg !315, !tbaa !263
  %57 = icmp eq i64 %53, 0, !dbg !310
  br i1 %57, label %._crit_edge.loopexit, label %.lr.ph, !dbg !312, !llvm.loop !321

._crit_edge.loopexit:                             ; preds = %.lr.ph.preheader72.split, %.lr.ph, %middle.block
  %scevgep = getelementptr i8, i8* %destaddr, i64 %len, !dbg !313
  br label %._crit_edge, !dbg !322

._crit_edge:                                      ; preds = %._crit_edge.loopexit, %0
  %dest.0.lcssa = phi i8* [ %destaddr, %0 ], [ %scevgep, %._crit_edge.loopexit ]
  ret i8* %dest.0.lcssa, !dbg !322
}

; Function Attrs: norecurse nounwind uwtable
define weak i8* @memset(i8* %dst, i32 %s, i64 %count) #7 !dbg !106 {
  %1 = icmp eq i64 %count, 0, !dbg !323
  br i1 %1, label %._crit_edge, label %.lr.ph, !dbg !325

.lr.ph:                                           ; preds = %0
  %2 = trunc i32 %s to i8, !dbg !326
  %3 = add i64 %count, -1, !dbg !325
  %xtraiter = and i64 %count, 7, !dbg !325
  %lcmp.mod = icmp eq i64 %xtraiter, 0, !dbg !325
  br i1 %lcmp.mod, label %.lr.ph.split, label %.preheader, !dbg !325

.preheader:                                       ; preds = %.lr.ph, %.preheader
  %a.02.prol = phi i8* [ %5, %.preheader ], [ %dst, %.lr.ph ]
  %.01.prol = phi i64 [ %4, %.preheader ], [ %count, %.lr.ph ]
  %prol.iter = phi i64 [ %prol.iter.sub, %.preheader ], [ %xtraiter, %.lr.ph ]
  %4 = add i64 %.01.prol, -1, !dbg !327
  %5 = getelementptr inbounds i8, i8* %a.02.prol, i64 1, !dbg !328
  store volatile i8 %2, i8* %a.02.prol, align 1, !dbg !329, !tbaa !263
  %prol.iter.sub = add i64 %prol.iter, -1, !dbg !325
  %prol.iter.cmp = icmp eq i64 %prol.iter.sub, 0, !dbg !325
  br i1 %prol.iter.cmp, label %.lr.ph.split, label %.preheader, !dbg !325, !llvm.loop !330

.lr.ph.split:                                     ; preds = %.preheader, %.lr.ph
  %a.02.unr = phi i8* [ %dst, %.lr.ph ], [ %5, %.preheader ]
  %.01.unr = phi i64 [ %count, %.lr.ph ], [ %4, %.preheader ]
  %6 = icmp ult i64 %3, 7, !dbg !325
  br i1 %6, label %._crit_edge, label %.lr.ph.split.split, !dbg !325

.lr.ph.split.split:                               ; preds = %.lr.ph.split, %.lr.ph.split.split
  %a.02 = phi i8* [ %15, %.lr.ph.split.split ], [ %a.02.unr, %.lr.ph.split ]
  %.01 = phi i64 [ %14, %.lr.ph.split.split ], [ %.01.unr, %.lr.ph.split ]
  %7 = getelementptr inbounds i8, i8* %a.02, i64 1, !dbg !328
  store volatile i8 %2, i8* %a.02, align 1, !dbg !329, !tbaa !263
  %8 = getelementptr inbounds i8, i8* %a.02, i64 2, !dbg !328
  store volatile i8 %2, i8* %7, align 1, !dbg !329, !tbaa !263
  %9 = getelementptr inbounds i8, i8* %a.02, i64 3, !dbg !328
  store volatile i8 %2, i8* %8, align 1, !dbg !329, !tbaa !263
  %10 = getelementptr inbounds i8, i8* %a.02, i64 4, !dbg !328
  store volatile i8 %2, i8* %9, align 1, !dbg !329, !tbaa !263
  %11 = getelementptr inbounds i8, i8* %a.02, i64 5, !dbg !328
  store volatile i8 %2, i8* %10, align 1, !dbg !329, !tbaa !263
  %12 = getelementptr inbounds i8, i8* %a.02, i64 6, !dbg !328
  store volatile i8 %2, i8* %11, align 1, !dbg !329, !tbaa !263
  %13 = getelementptr inbounds i8, i8* %a.02, i64 7, !dbg !328
  store volatile i8 %2, i8* %12, align 1, !dbg !329, !tbaa !263
  %14 = add i64 %.01, -8, !dbg !327
  %15 = getelementptr inbounds i8, i8* %a.02, i64 8, !dbg !328
  store volatile i8 %2, i8* %13, align 1, !dbg !329, !tbaa !263
  %16 = icmp eq i64 %14, 0, !dbg !323
  br i1 %16, label %._crit_edge, label %.lr.ph.split.split, !dbg !325

._crit_edge:                                      ; preds = %.lr.ph.split, %.lr.ph.split.split, %0
  ret i8* %dst, !dbg !331
}

attributes #0 = { nounwind uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone }
attributes #2 = { "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { nounwind uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { argmemonly nounwind }
attributes #5 = { "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #6 = { noreturn "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #7 = { norecurse nounwind uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #8 = { nobuiltin nounwind }
attributes #9 = { nobuiltin noreturn nounwind }

!llvm.dbg.cu = !{!0, !9, !21, !30, !42, !52, !65, !83, !93, !103}
!llvm.module.flags = !{!116, !117}
!llvm.ident = !{!118, !118, !118, !118, !118, !118, !118, !118, !118, !118}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "clang version 3.8.0-2ubuntu4 (tags/RELEASE_380/final)", isOptimized: false, runtimeVersion: 0, emissionKind: 1, enums: !2, subprograms: !3)
!1 = !DIFile(filename: "test1.c", directory: "/home/tuba/Documents/tools/pmguidedsymex/test/test1")
!2 = !{}
!3 = !{!4, !8}
!4 = distinct !DISubprogram(name: "loop", scope: !1, file: !1, line: 12, type: !5, isLocal: false, isDefinition: true, scopeLine: 12, flags: DIFlagPrototyped, isOptimized: false, variables: !2)
!5 = !DISubroutineType(types: !6)
!6 = !{!7, !7, !7}
!7 = !DIBasicType(name: "int", size: 32, align: 32, encoding: DW_ATE_signed)
!8 = distinct !DISubprogram(name: "foo", scope: !1, file: !1, line: 19, type: !5, isLocal: false, isDefinition: true, scopeLine: 19, flags: DIFlagPrototyped, isOptimized: false, variables: !2)
!9 = distinct !DICompileUnit(language: DW_LANG_C89, file: !10, producer: "clang version 3.8.0-2ubuntu4 (tags/RELEASE_380/final)", isOptimized: true, runtimeVersion: 0, emissionKind: 1, enums: !2, subprograms: !11)
!10 = !DIFile(filename: "/home/tuba/Documents/tools/pmguidedsymex/klee/runtime/Intrinsic/klee_choose.c", directory: "/home/tuba/Documents/tools/pmguidedsymex/klee_build_dir/runtime/Intrinsic")
!11 = !{!12}
!12 = distinct !DISubprogram(name: "klee_choose", scope: !10, file: !10, line: 12, type: !13, isLocal: false, isDefinition: true, scopeLine: 12, flags: DIFlagPrototyped, isOptimized: true, variables: !18)
!13 = !DISubroutineType(types: !14)
!14 = !{!15, !15}
!15 = !DIDerivedType(tag: DW_TAG_typedef, name: "uintptr_t", file: !16, line: 122, baseType: !17)
!16 = !DIFile(filename: "/usr/include/stdint.h", directory: "/home/tuba/Documents/tools/pmguidedsymex/klee_build_dir/runtime/Intrinsic")
!17 = !DIBasicType(name: "long unsigned int", size: 64, align: 64, encoding: DW_ATE_unsigned)
!18 = !{!19, !20}
!19 = !DILocalVariable(name: "n", arg: 1, scope: !12, file: !10, line: 12, type: !15)
!20 = !DILocalVariable(name: "x", scope: !12, file: !10, line: 13, type: !15)
!21 = distinct !DICompileUnit(language: DW_LANG_C89, file: !22, producer: "clang version 3.8.0-2ubuntu4 (tags/RELEASE_380/final)", isOptimized: true, runtimeVersion: 0, emissionKind: 1, enums: !2, subprograms: !23)
!22 = !DIFile(filename: "/home/tuba/Documents/tools/pmguidedsymex/klee/runtime/Intrinsic/klee_div_zero_check.c", directory: "/home/tuba/Documents/tools/pmguidedsymex/klee_build_dir/runtime/Intrinsic")
!23 = !{!24}
!24 = distinct !DISubprogram(name: "klee_div_zero_check", scope: !22, file: !22, line: 12, type: !25, isLocal: false, isDefinition: true, scopeLine: 12, flags: DIFlagPrototyped, isOptimized: true, variables: !28)
!25 = !DISubroutineType(types: !26)
!26 = !{null, !27}
!27 = !DIBasicType(name: "long long int", size: 64, align: 64, encoding: DW_ATE_signed)
!28 = !{!29}
!29 = !DILocalVariable(name: "z", arg: 1, scope: !24, file: !22, line: 12, type: !27)
!30 = distinct !DICompileUnit(language: DW_LANG_C89, file: !31, producer: "clang version 3.8.0-2ubuntu4 (tags/RELEASE_380/final)", isOptimized: true, runtimeVersion: 0, emissionKind: 1, enums: !2, subprograms: !32)
!31 = !DIFile(filename: "/home/tuba/Documents/tools/pmguidedsymex/klee/runtime/Intrinsic/klee_int.c", directory: "/home/tuba/Documents/tools/pmguidedsymex/klee_build_dir/runtime/Intrinsic")
!32 = !{!33}
!33 = distinct !DISubprogram(name: "klee_int", scope: !31, file: !31, line: 13, type: !34, isLocal: false, isDefinition: true, scopeLine: 13, flags: DIFlagPrototyped, isOptimized: true, variables: !39)
!34 = !DISubroutineType(types: !35)
!35 = !{!7, !36}
!36 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !37, size: 64, align: 64)
!37 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !38)
!38 = !DIBasicType(name: "char", size: 8, align: 8, encoding: DW_ATE_signed_char)
!39 = !{!40, !41}
!40 = !DILocalVariable(name: "name", arg: 1, scope: !33, file: !31, line: 13, type: !36)
!41 = !DILocalVariable(name: "x", scope: !33, file: !31, line: 14, type: !7)
!42 = distinct !DICompileUnit(language: DW_LANG_C89, file: !43, producer: "clang version 3.8.0-2ubuntu4 (tags/RELEASE_380/final)", isOptimized: true, runtimeVersion: 0, emissionKind: 1, enums: !2, subprograms: !44)
!43 = !DIFile(filename: "/home/tuba/Documents/tools/pmguidedsymex/klee/runtime/Intrinsic/klee_overshift_check.c", directory: "/home/tuba/Documents/tools/pmguidedsymex/klee_build_dir/runtime/Intrinsic")
!44 = !{!45}
!45 = distinct !DISubprogram(name: "klee_overshift_check", scope: !43, file: !43, line: 20, type: !46, isLocal: false, isDefinition: true, scopeLine: 20, flags: DIFlagPrototyped, isOptimized: true, variables: !49)
!46 = !DISubroutineType(types: !47)
!47 = !{null, !48, !48}
!48 = !DIBasicType(name: "long long unsigned int", size: 64, align: 64, encoding: DW_ATE_unsigned)
!49 = !{!50, !51}
!50 = !DILocalVariable(name: "bitWidth", arg: 1, scope: !45, file: !43, line: 20, type: !48)
!51 = !DILocalVariable(name: "shift", arg: 2, scope: !45, file: !43, line: 20, type: !48)
!52 = distinct !DICompileUnit(language: DW_LANG_C89, file: !53, producer: "clang version 3.8.0-2ubuntu4 (tags/RELEASE_380/final)", isOptimized: true, runtimeVersion: 0, emissionKind: 1, enums: !2, retainedTypes: !54, subprograms: !56)
!53 = !DIFile(filename: "/home/tuba/Documents/tools/pmguidedsymex/klee/runtime/Intrinsic/klee_range.c", directory: "/home/tuba/Documents/tools/pmguidedsymex/klee_build_dir/runtime/Intrinsic")
!54 = !{!55}
!55 = !DIBasicType(name: "unsigned int", size: 32, align: 32, encoding: DW_ATE_unsigned)
!56 = !{!57}
!57 = distinct !DISubprogram(name: "klee_range", scope: !53, file: !53, line: 13, type: !58, isLocal: false, isDefinition: true, scopeLine: 13, flags: DIFlagPrototyped, isOptimized: true, variables: !60)
!58 = !DISubroutineType(types: !59)
!59 = !{!7, !7, !7, !36}
!60 = !{!61, !62, !63, !64}
!61 = !DILocalVariable(name: "start", arg: 1, scope: !57, file: !53, line: 13, type: !7)
!62 = !DILocalVariable(name: "end", arg: 2, scope: !57, file: !53, line: 13, type: !7)
!63 = !DILocalVariable(name: "name", arg: 3, scope: !57, file: !53, line: 13, type: !36)
!64 = !DILocalVariable(name: "x", scope: !57, file: !53, line: 14, type: !7)
!65 = distinct !DICompileUnit(language: DW_LANG_C89, file: !66, producer: "clang version 3.8.0-2ubuntu4 (tags/RELEASE_380/final)", isOptimized: true, runtimeVersion: 0, emissionKind: 1, enums: !2, subprograms: !67)
!66 = !DIFile(filename: "/home/tuba/Documents/tools/pmguidedsymex/klee/runtime/Intrinsic/memcpy.c", directory: "/home/tuba/Documents/tools/pmguidedsymex/klee_build_dir/runtime/Intrinsic")
!67 = !{!68}
!68 = distinct !DISubprogram(name: "memcpy", scope: !66, file: !66, line: 12, type: !69, isLocal: false, isDefinition: true, scopeLine: 12, flags: DIFlagPrototyped, isOptimized: true, variables: !76)
!69 = !DISubroutineType(types: !70)
!70 = !{!71, !71, !72, !74}
!71 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: null, size: 64, align: 64)
!72 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !73, size: 64, align: 64)
!73 = !DIDerivedType(tag: DW_TAG_const_type, baseType: null)
!74 = !DIDerivedType(tag: DW_TAG_typedef, name: "size_t", file: !75, line: 62, baseType: !17)
!75 = !DIFile(filename: "/usr/lib/llvm-3.8/bin/../lib/clang/3.8.0/include/stddef.h", directory: "/home/tuba/Documents/tools/pmguidedsymex/klee_build_dir/runtime/Intrinsic")
!76 = !{!77, !78, !79, !80, !82}
!77 = !DILocalVariable(name: "destaddr", arg: 1, scope: !68, file: !66, line: 12, type: !71)
!78 = !DILocalVariable(name: "srcaddr", arg: 2, scope: !68, file: !66, line: 12, type: !72)
!79 = !DILocalVariable(name: "len", arg: 3, scope: !68, file: !66, line: 12, type: !74)
!80 = !DILocalVariable(name: "dest", scope: !68, file: !66, line: 13, type: !81)
!81 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !38, size: 64, align: 64)
!82 = !DILocalVariable(name: "src", scope: !68, file: !66, line: 14, type: !36)
!83 = distinct !DICompileUnit(language: DW_LANG_C89, file: !84, producer: "clang version 3.8.0-2ubuntu4 (tags/RELEASE_380/final)", isOptimized: true, runtimeVersion: 0, emissionKind: 1, enums: !2, subprograms: !85)
!84 = !DIFile(filename: "/home/tuba/Documents/tools/pmguidedsymex/klee/runtime/Intrinsic/memmove.c", directory: "/home/tuba/Documents/tools/pmguidedsymex/klee_build_dir/runtime/Intrinsic")
!85 = !{!86}
!86 = distinct !DISubprogram(name: "memmove", scope: !84, file: !84, line: 12, type: !69, isLocal: false, isDefinition: true, scopeLine: 12, flags: DIFlagPrototyped, isOptimized: true, variables: !87)
!87 = !{!88, !89, !90, !91, !92}
!88 = !DILocalVariable(name: "dst", arg: 1, scope: !86, file: !84, line: 12, type: !71)
!89 = !DILocalVariable(name: "src", arg: 2, scope: !86, file: !84, line: 12, type: !72)
!90 = !DILocalVariable(name: "count", arg: 3, scope: !86, file: !84, line: 12, type: !74)
!91 = !DILocalVariable(name: "a", scope: !86, file: !84, line: 13, type: !81)
!92 = !DILocalVariable(name: "b", scope: !86, file: !84, line: 14, type: !36)
!93 = distinct !DICompileUnit(language: DW_LANG_C89, file: !94, producer: "clang version 3.8.0-2ubuntu4 (tags/RELEASE_380/final)", isOptimized: true, runtimeVersion: 0, emissionKind: 1, enums: !2, subprograms: !95)
!94 = !DIFile(filename: "/home/tuba/Documents/tools/pmguidedsymex/klee/runtime/Intrinsic/mempcpy.c", directory: "/home/tuba/Documents/tools/pmguidedsymex/klee_build_dir/runtime/Intrinsic")
!95 = !{!96}
!96 = distinct !DISubprogram(name: "mempcpy", scope: !94, file: !94, line: 11, type: !69, isLocal: false, isDefinition: true, scopeLine: 11, flags: DIFlagPrototyped, isOptimized: true, variables: !97)
!97 = !{!98, !99, !100, !101, !102}
!98 = !DILocalVariable(name: "destaddr", arg: 1, scope: !96, file: !94, line: 11, type: !71)
!99 = !DILocalVariable(name: "srcaddr", arg: 2, scope: !96, file: !94, line: 11, type: !72)
!100 = !DILocalVariable(name: "len", arg: 3, scope: !96, file: !94, line: 11, type: !74)
!101 = !DILocalVariable(name: "dest", scope: !96, file: !94, line: 12, type: !81)
!102 = !DILocalVariable(name: "src", scope: !96, file: !94, line: 13, type: !36)
!103 = distinct !DICompileUnit(language: DW_LANG_C89, file: !104, producer: "clang version 3.8.0-2ubuntu4 (tags/RELEASE_380/final)", isOptimized: true, runtimeVersion: 0, emissionKind: 1, enums: !2, subprograms: !105)
!104 = !DIFile(filename: "/home/tuba/Documents/tools/pmguidedsymex/klee/runtime/Intrinsic/memset.c", directory: "/home/tuba/Documents/tools/pmguidedsymex/klee_build_dir/runtime/Intrinsic")
!105 = !{!106}
!106 = distinct !DISubprogram(name: "memset", scope: !104, file: !104, line: 11, type: !107, isLocal: false, isDefinition: true, scopeLine: 11, flags: DIFlagPrototyped, isOptimized: true, variables: !109)
!107 = !DISubroutineType(types: !108)
!108 = !{!71, !71, !7, !74}
!109 = !{!110, !111, !112, !113}
!110 = !DILocalVariable(name: "dst", arg: 1, scope: !106, file: !104, line: 11, type: !71)
!111 = !DILocalVariable(name: "s", arg: 2, scope: !106, file: !104, line: 11, type: !7)
!112 = !DILocalVariable(name: "count", arg: 3, scope: !106, file: !104, line: 11, type: !74)
!113 = !DILocalVariable(name: "a", scope: !106, file: !104, line: 12, type: !114)
!114 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !115, size: 64, align: 64)
!115 = !DIDerivedType(tag: DW_TAG_volatile_type, baseType: !38)
!116 = !{i32 2, !"Dwarf Version", i32 4}
!117 = !{i32 2, !"Debug Info Version", i32 3}
!118 = !{!"clang version 3.8.0-2ubuntu4 (tags/RELEASE_380/final)"}
!119 = !DILocation(line: 13, column: 18, scope: !4)
!120 = !DILocation(line: 13, column: 9, scope: !4)
!121 = !DILocation(line: 14, column: 13, scope: !122)
!122 = distinct !DILexicalBlock(scope: !4, file: !1, line: 14, column: 5)
!123 = !DILocation(line: 14, column: 9, scope: !122)
!124 = !DILocation(line: 14, column: 18, scope: !125)
!125 = distinct !DILexicalBlock(scope: !122, file: !1, line: 14, column: 5)
!126 = !DILocation(line: 14, column: 20, scope: !125)
!127 = !DILocation(line: 14, column: 19, scope: !125)
!128 = !DILocation(line: 15, column: 17, scope: !125)
!129 = !DILocation(line: 14, column: 5, scope: !122)
!130 = !DILocation(line: 15, column: 26, scope: !125)
!131 = !DILocation(line: 15, column: 24, scope: !125)
!132 = !DILocation(line: 15, column: 15, scope: !125)
!133 = !DILocation(line: 14, column: 25, scope: !125)
!134 = !DILocation(line: 14, column: 5, scope: !125)
!135 = !DILocation(line: 16, column: 6, scope: !4)
!136 = !DILocation(line: 20, column: 9, scope: !137)
!137 = distinct !DILexicalBlock(scope: !8, file: !1, line: 20, column: 9)
!138 = !DILocation(line: 20, column: 11, scope: !137)
!139 = !DILocation(line: 21, column: 13, scope: !140)
!140 = distinct !DILexicalBlock(scope: !141, file: !1, line: 21, column: 13)
!141 = distinct !DILexicalBlock(scope: !137, file: !1, line: 20, column: 16)
!142 = !DILocation(line: 20, column: 9, scope: !8)
!143 = !DILocation(line: 21, column: 14, scope: !140)
!144 = !DILocation(line: 21, column: 13, scope: !141)
!145 = !DILocation(line: 22, column: 27, scope: !146)
!146 = distinct !DILexicalBlock(scope: !140, file: !1, line: 21, column: 18)
!147 = !DILocation(line: 22, column: 12, scope: !146)
!148 = !DILocation(line: 23, column: 12, scope: !146)
!149 = !DILocation(line: 25, column: 18, scope: !150)
!150 = distinct !DILexicalBlock(scope: !140, file: !1, line: 25, column: 18)
!151 = !DILocation(line: 25, column: 19, scope: !150)
!152 = !DILocation(line: 25, column: 18, scope: !140)
!153 = !DILocation(line: 26, column: 27, scope: !154)
!154 = distinct !DILexicalBlock(scope: !150, file: !1, line: 25, column: 23)
!155 = !DILocation(line: 26, column: 12, scope: !154)
!156 = !DILocation(line: 27, column: 12, scope: !154)
!157 = !DILocation(line: 30, column: 27, scope: !158)
!158 = distinct !DILexicalBlock(scope: !150, file: !1, line: 29, column: 14)
!159 = !DILocation(line: 30, column: 12, scope: !158)
!160 = !DILocation(line: 31, column: 12, scope: !158)
!161 = !DILocation(line: 35, column: 14, scope: !162)
!162 = distinct !DILexicalBlock(scope: !163, file: !1, line: 35, column: 13)
!163 = distinct !DILexicalBlock(scope: !137, file: !1, line: 34, column: 10)
!164 = !DILocation(line: 35, column: 13, scope: !163)
!165 = !DILocation(line: 36, column: 16, scope: !166)
!166 = distinct !DILexicalBlock(scope: !167, file: !1, line: 36, column: 16)
!167 = distinct !DILexicalBlock(scope: !162, file: !1, line: 35, column: 18)
!168 = !DILocation(line: 36, column: 18, scope: !166)
!169 = !DILocation(line: 37, column: 19, scope: !170)
!170 = distinct !DILexicalBlock(scope: !171, file: !1, line: 37, column: 19)
!171 = distinct !DILexicalBlock(scope: !166, file: !1, line: 36, column: 25)
!172 = !DILocation(line: 37, column: 21, scope: !170)
!173 = !DILocation(line: 36, column: 16, scope: !167)
!174 = !DILocation(line: 37, column: 19, scope: !171)
!175 = !DILocation(line: 38, column: 33, scope: !176)
!176 = distinct !DILexicalBlock(scope: !170, file: !1, line: 37, column: 28)
!177 = !DILocation(line: 38, column: 18, scope: !176)
!178 = !DILocation(line: 39, column: 18, scope: !176)
!179 = !DILocation(line: 42, column: 33, scope: !180)
!180 = distinct !DILexicalBlock(scope: !170, file: !1, line: 41, column: 20)
!181 = !DILocation(line: 42, column: 18, scope: !180)
!182 = !DILocation(line: 43, column: 18, scope: !180)
!183 = !DILocation(line: 47, column: 19, scope: !184)
!184 = distinct !DILexicalBlock(scope: !166, file: !1, line: 46, column: 17)
!185 = !DILocation(line: 48, column: 33, scope: !186)
!186 = distinct !DILexicalBlock(scope: !187, file: !1, line: 47, column: 28)
!187 = distinct !DILexicalBlock(scope: !184, file: !1, line: 47, column: 19)
!188 = !DILocation(line: 48, column: 18, scope: !186)
!189 = !DILocation(line: 49, column: 18, scope: !186)
!190 = !DILocation(line: 52, column: 33, scope: !191)
!191 = distinct !DILexicalBlock(scope: !187, file: !1, line: 51, column: 20)
!192 = !DILocation(line: 52, column: 18, scope: !191)
!193 = !DILocation(line: 53, column: 18, scope: !191)
!194 = !DILocation(line: 58, column: 27, scope: !195)
!195 = distinct !DILexicalBlock(scope: !162, file: !1, line: 57, column: 14)
!196 = !DILocation(line: 58, column: 12, scope: !195)
!197 = !DILocation(line: 59, column: 12, scope: !195)
!198 = !DILocation(line: 62, column: 1, scope: !8)
!199 = !DILocation(line: 13, column: 3, scope: !12)
!200 = !DILocation(line: 14, column: 3, scope: !12)
!201 = !DILocation(line: 17, column: 6, scope: !202)
!202 = distinct !DILexicalBlock(scope: !12, file: !10, line: 17, column: 6)
!203 = !{!204, !204, i64 0}
!204 = !{!"long", !205, i64 0}
!205 = !{!"omnipotent char", !206, i64 0}
!206 = !{!"Simple C/C++ TBAA"}
!207 = !DILocation(line: 17, column: 8, scope: !202)
!208 = !DILocation(line: 17, column: 6, scope: !12)
!209 = !DILocation(line: 18, column: 5, scope: !202)
!210 = !DILocation(line: 19, column: 3, scope: !12)
!211 = !DILocation(line: 13, column: 9, scope: !212)
!212 = distinct !DILexicalBlock(scope: !24, file: !22, line: 13, column: 7)
!213 = !DILocation(line: 13, column: 7, scope: !24)
!214 = !DILocation(line: 14, column: 5, scope: !212)
!215 = !DILocation(line: 15, column: 1, scope: !24)
!216 = !DILocation(line: 14, column: 3, scope: !33)
!217 = !DILocation(line: 15, column: 3, scope: !33)
!218 = !DILocation(line: 16, column: 10, scope: !33)
!219 = !{!220, !220, i64 0}
!220 = !{!"int", !205, i64 0}
!221 = !DILocation(line: 16, column: 3, scope: !33)
!222 = !DILocation(line: 21, column: 13, scope: !223)
!223 = distinct !DILexicalBlock(scope: !45, file: !43, line: 21, column: 7)
!224 = !DILocation(line: 21, column: 7, scope: !45)
!225 = !DILocation(line: 27, column: 5, scope: !226)
!226 = distinct !DILexicalBlock(scope: !223, file: !43, line: 21, column: 26)
!227 = !DILocation(line: 29, column: 1, scope: !45)
!228 = !DILocation(line: 14, column: 3, scope: !57)
!229 = !DILocation(line: 16, column: 13, scope: !230)
!230 = distinct !DILexicalBlock(scope: !57, file: !53, line: 16, column: 7)
!231 = !DILocation(line: 16, column: 7, scope: !57)
!232 = !DILocation(line: 17, column: 5, scope: !230)
!233 = !DILocation(line: 19, column: 12, scope: !234)
!234 = distinct !DILexicalBlock(scope: !57, file: !53, line: 19, column: 7)
!235 = !DILocation(line: 19, column: 14, scope: !234)
!236 = !DILocation(line: 19, column: 7, scope: !57)
!237 = !DILocation(line: 22, column: 5, scope: !238)
!238 = distinct !DILexicalBlock(scope: !234, file: !53, line: 21, column: 10)
!239 = !DILocation(line: 25, column: 14, scope: !240)
!240 = distinct !DILexicalBlock(scope: !238, file: !53, line: 25, column: 9)
!241 = !DILocation(line: 26, column: 30, scope: !242)
!242 = distinct !DILexicalBlock(scope: !240, file: !53, line: 25, column: 19)
!243 = !DILocation(line: 25, column: 9, scope: !238)
!244 = !DILocation(line: 26, column: 32, scope: !242)
!245 = !DILocation(line: 26, column: 19, scope: !242)
!246 = !DILocation(line: 26, column: 7, scope: !242)
!247 = !DILocation(line: 27, column: 5, scope: !242)
!248 = !DILocation(line: 28, column: 25, scope: !249)
!249 = distinct !DILexicalBlock(scope: !240, file: !53, line: 27, column: 12)
!250 = !DILocation(line: 28, column: 19, scope: !249)
!251 = !DILocation(line: 28, column: 7, scope: !249)
!252 = !DILocation(line: 29, column: 19, scope: !249)
!253 = !DILocation(line: 29, column: 21, scope: !249)
!254 = !DILocation(line: 29, column: 7, scope: !249)
!255 = !DILocation(line: 32, column: 12, scope: !238)
!256 = !DILocation(line: 32, column: 5, scope: !238)
!257 = !DILocation(line: 34, column: 1, scope: !57)
!258 = !DILocation(line: 16, column: 16, scope: !259)
!259 = !DILexicalBlockFile(scope: !68, file: !66, discriminator: 1)
!260 = !DILocation(line: 16, column: 3, scope: !259)
!261 = !DILocation(line: 16, column: 13, scope: !259)
!262 = !DILocation(line: 17, column: 15, scope: !68)
!263 = !{!205, !205, i64 0}
!264 = !DILocation(line: 17, column: 13, scope: !68)
!265 = distinct !{!265, !266}
!266 = !{!"llvm.loop.unroll.disable"}
!267 = distinct !{!267, !268, !269}
!268 = !{!"llvm.loop.vectorize.width", i32 1}
!269 = !{!"llvm.loop.interleave.count", i32 1}
!270 = !DILocation(line: 17, column: 19, scope: !68)
!271 = !DILocation(line: 17, column: 10, scope: !68)
!272 = distinct !{!272, !266}
!273 = distinct !{!273, !268, !269}
!274 = !DILocation(line: 18, column: 3, scope: !68)
!275 = !DILocation(line: 16, column: 11, scope: !276)
!276 = distinct !DILexicalBlock(scope: !86, file: !84, line: 16, column: 7)
!277 = !DILocation(line: 16, column: 7, scope: !86)
!278 = !DILocation(line: 19, column: 10, scope: !279)
!279 = distinct !DILexicalBlock(scope: !86, file: !84, line: 19, column: 7)
!280 = !DILocation(line: 19, column: 7, scope: !86)
!281 = !DILocation(line: 20, column: 5, scope: !282)
!282 = !DILexicalBlockFile(scope: !283, file: !84, discriminator: 1)
!283 = distinct !DILexicalBlock(scope: !279, file: !84, line: 19, column: 16)
!284 = !DILocation(line: 20, column: 17, scope: !282)
!285 = !DILocation(line: 20, column: 28, scope: !286)
!286 = !DILexicalBlockFile(scope: !283, file: !84, discriminator: 2)
!287 = !DILocation(line: 20, column: 26, scope: !286)
!288 = distinct !{!288, !266}
!289 = distinct !{!289, !268, !269}
!290 = !DILocation(line: 20, column: 30, scope: !286)
!291 = !DILocation(line: 20, column: 23, scope: !286)
!292 = distinct !{!292, !266}
!293 = distinct !{!293, !268, !269}
!294 = !DILocation(line: 22, column: 13, scope: !295)
!295 = distinct !DILexicalBlock(scope: !279, file: !84, line: 21, column: 10)
!296 = !DILocation(line: 24, column: 5, scope: !297)
!297 = !DILexicalBlockFile(scope: !295, file: !84, discriminator: 1)
!298 = !DILocation(line: 23, column: 6, scope: !295)
!299 = !DILocation(line: 22, column: 6, scope: !295)
!300 = !DILocation(line: 24, column: 17, scope: !297)
!301 = !DILocation(line: 24, column: 28, scope: !302)
!302 = !DILexicalBlockFile(scope: !295, file: !84, discriminator: 2)
!303 = !DILocation(line: 24, column: 26, scope: !302)
!304 = distinct !{!304, !268, !269}
!305 = !DILocation(line: 24, column: 30, scope: !302)
!306 = !DILocation(line: 24, column: 23, scope: !302)
!307 = distinct !{!307, !266}
!308 = distinct !{!308, !268, !269}
!309 = !DILocation(line: 28, column: 1, scope: !86)
!310 = !DILocation(line: 15, column: 16, scope: !311)
!311 = !DILexicalBlockFile(scope: !96, file: !94, discriminator: 1)
!312 = !DILocation(line: 15, column: 3, scope: !311)
!313 = !DILocation(line: 15, column: 13, scope: !311)
!314 = !DILocation(line: 16, column: 15, scope: !96)
!315 = !DILocation(line: 16, column: 13, scope: !96)
!316 = distinct !{!316, !266}
!317 = distinct !{!317, !268, !269}
!318 = !DILocation(line: 16, column: 19, scope: !96)
!319 = !DILocation(line: 16, column: 10, scope: !96)
!320 = distinct !{!320, !266}
!321 = distinct !{!321, !268, !269}
!322 = !DILocation(line: 17, column: 3, scope: !96)
!323 = !DILocation(line: 13, column: 20, scope: !324)
!324 = !DILexicalBlockFile(scope: !106, file: !104, discriminator: 1)
!325 = !DILocation(line: 13, column: 5, scope: !324)
!326 = !DILocation(line: 14, column: 14, scope: !106)
!327 = !DILocation(line: 13, column: 17, scope: !324)
!328 = !DILocation(line: 14, column: 9, scope: !106)
!329 = !DILocation(line: 14, column: 12, scope: !106)
!330 = distinct !{!330, !266}
!331 = !DILocation(line: 15, column: 5, scope: !106)
