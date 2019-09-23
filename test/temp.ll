; ModuleID = 'temp.bc'
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

%struct.myst = type { i32, i8, i32 }

; Function Attrs: nounwind uwtable
define i32 @foo(i32 %A, i32* %B, i32 %C, i32* %D, i32** %F, %struct.myst* %M) #0 !dbg !4 {
  %1 = alloca i32, align 4
  %2 = alloca i32*, align 8
  %3 = alloca i32, align 4
  %4 = alloca i32*, align 8
  %5 = alloca i32**, align 8
  %6 = alloca %struct.myst*, align 8
  %E = alloca i32*, align 8
  %temp = alloca i32, align 4
  store i32 %A, i32* %1, align 4
  call void @llvm.dbg.declare(metadata i32* %1, metadata !20, metadata !21), !dbg !22
  store i32* %B, i32** %2, align 8
  call void @llvm.dbg.declare(metadata i32** %2, metadata !23, metadata !21), !dbg !24
  store i32 %C, i32* %3, align 4
  call void @llvm.dbg.declare(metadata i32* %3, metadata !25, metadata !21), !dbg !26
  store i32* %D, i32** %4, align 8
  call void @llvm.dbg.declare(metadata i32** %4, metadata !27, metadata !21), !dbg !28
  store i32** %F, i32*** %5, align 8
  call void @llvm.dbg.declare(metadata i32*** %5, metadata !29, metadata !21), !dbg !30
  store %struct.myst* %M, %struct.myst** %6, align 8
  call void @llvm.dbg.declare(metadata %struct.myst** %6, metadata !31, metadata !21), !dbg !32
  call void @llvm.dbg.declare(metadata i32** %E, metadata !33, metadata !21), !dbg !34
  %7 = load i32, i32* %1, align 4, !dbg !35
  %8 = load i32, i32* %3, align 4, !dbg !37
  %9 = add nsw i32 %7, %8, !dbg !38
  %10 = icmp sgt i32 %9, 0, !dbg !39
  br i1 %10, label %11, label %13, !dbg !40

; <label>:11                                      ; preds = %0
  %12 = load i32*, i32** %2, align 8, !dbg !41
  store i32* %12, i32** %E, align 8, !dbg !42
  br label %15, !dbg !43

; <label>:13                                      ; preds = %0
  %14 = load i32*, i32** %4, align 8, !dbg !44
  store i32* %14, i32** %E, align 8, !dbg !45
  br label %15

; <label>:15                                      ; preds = %13, %11
  %16 = load i32, i32* %1, align 4, !dbg !46
  %17 = load i32*, i32** %E, align 8, !dbg !47
  store i32 %16, i32* %17, align 4, !dbg !48
  call void @llvm.dbg.declare(metadata i32* %temp, metadata !49, metadata !21), !dbg !50
  %18 = load i32**, i32*** %5, align 8, !dbg !51
  %19 = load i32*, i32** %18, align 8, !dbg !52
  %20 = load i32, i32* %19, align 4, !dbg !53
  store i32 %20, i32* %temp, align 4, !dbg !50
  %21 = load i32, i32* %temp, align 4, !dbg !54
  %22 = load %struct.myst*, %struct.myst** %6, align 8, !dbg !55
  %23 = getelementptr inbounds %struct.myst, %struct.myst* %22, i32 0, i32 2, !dbg !56
  store i32 %21, i32* %23, align 4, !dbg !57
  %24 = load i32, i32* %3, align 4, !dbg !58
  ret i32 %24, !dbg !59
}

; Function Attrs: nounwind readnone
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

attributes #0 = { nounwind uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!17, !18}
!llvm.ident = !{!19}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "clang version 3.8.0-2ubuntu4 (tags/RELEASE_380/final)", isOptimized: false, runtimeVersion: 0, emissionKind: 1, enums: !2, subprograms: !3)
!1 = !DIFile(filename: "temp.c", directory: "/home/tuba/Documents/tools/pmguidedsymex/test/test23")
!2 = !{}
!3 = !{!4}
!4 = distinct !DISubprogram(name: "foo", scope: !1, file: !1, line: 9, type: !5, isLocal: false, isDefinition: true, scopeLine: 9, flags: DIFlagPrototyped, isOptimized: false, variables: !2)
!5 = !DISubroutineType(types: !6)
!6 = !{!7, !7, !8, !7, !8, !9, !10}
!7 = !DIBasicType(name: "int", size: 32, align: 32, encoding: DW_ATE_signed)
!8 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !7, size: 64, align: 64)
!9 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !8, size: 64, align: 64)
!10 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !11, size: 64, align: 64)
!11 = !DICompositeType(tag: DW_TAG_structure_type, name: "myst", file: !1, line: 3, size: 96, align: 32, elements: !12)
!12 = !{!13, !14, !16}
!13 = !DIDerivedType(tag: DW_TAG_member, name: "index", scope: !11, file: !1, line: 4, baseType: !7, size: 32, align: 32)
!14 = !DIDerivedType(tag: DW_TAG_member, name: "c", scope: !11, file: !1, line: 5, baseType: !15, size: 8, align: 8, offset: 32)
!15 = !DIBasicType(name: "char", size: 8, align: 8, encoding: DW_ATE_signed_char)
!16 = !DIDerivedType(tag: DW_TAG_member, name: "age", scope: !11, file: !1, line: 6, baseType: !7, size: 32, align: 32, offset: 64)
!17 = !{i32 2, !"Dwarf Version", i32 4}
!18 = !{i32 2, !"Debug Info Version", i32 3}
!19 = !{!"clang version 3.8.0-2ubuntu4 (tags/RELEASE_380/final)"}
!20 = !DILocalVariable(name: "A", arg: 1, scope: !4, file: !1, line: 9, type: !7)
!21 = !DIExpression()
!22 = !DILocation(line: 9, column: 13, scope: !4)
!23 = !DILocalVariable(name: "B", arg: 2, scope: !4, file: !1, line: 9, type: !8)
!24 = !DILocation(line: 9, column: 21, scope: !4)
!25 = !DILocalVariable(name: "C", arg: 3, scope: !4, file: !1, line: 9, type: !7)
!26 = !DILocation(line: 9, column: 28, scope: !4)
!27 = !DILocalVariable(name: "D", arg: 4, scope: !4, file: !1, line: 9, type: !8)
!28 = !DILocation(line: 9, column: 36, scope: !4)
!29 = !DILocalVariable(name: "F", arg: 5, scope: !4, file: !1, line: 9, type: !9)
!30 = !DILocation(line: 9, column: 45, scope: !4)
!31 = !DILocalVariable(name: "M", arg: 6, scope: !4, file: !1, line: 9, type: !10)
!32 = !DILocation(line: 9, column: 61, scope: !4)
!33 = !DILocalVariable(name: "E", scope: !4, file: !1, line: 10, type: !8)
!34 = !DILocation(line: 10, column: 8, scope: !4)
!35 = !DILocation(line: 11, column: 7, scope: !36)
!36 = distinct !DILexicalBlock(scope: !4, file: !1, line: 11, column: 7)
!37 = !DILocation(line: 11, column: 11, scope: !36)
!38 = !DILocation(line: 11, column: 9, scope: !36)
!39 = !DILocation(line: 11, column: 13, scope: !36)
!40 = !DILocation(line: 11, column: 7, scope: !4)
!41 = !DILocation(line: 12, column: 10, scope: !36)
!42 = !DILocation(line: 12, column: 8, scope: !36)
!43 = !DILocation(line: 12, column: 6, scope: !36)
!44 = !DILocation(line: 14, column: 10, scope: !36)
!45 = !DILocation(line: 14, column: 8, scope: !36)
!46 = !DILocation(line: 15, column: 8, scope: !4)
!47 = !DILocation(line: 15, column: 4, scope: !4)
!48 = !DILocation(line: 15, column: 6, scope: !4)
!49 = !DILocalVariable(name: "temp", scope: !4, file: !1, line: 16, type: !7)
!50 = !DILocation(line: 16, column: 7, scope: !4)
!51 = !DILocation(line: 16, column: 16, scope: !4)
!52 = !DILocation(line: 16, column: 15, scope: !4)
!53 = !DILocation(line: 16, column: 14, scope: !4)
!54 = !DILocation(line: 17, column: 12, scope: !4)
!55 = !DILocation(line: 17, column: 3, scope: !4)
!56 = !DILocation(line: 17, column: 6, scope: !4)
!57 = !DILocation(line: 17, column: 10, scope: !4)
!58 = !DILocation(line: 18, column: 10, scope: !4)
!59 = !DILocation(line: 18, column: 3, scope: !4)
