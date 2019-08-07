; ModuleID = 'test11.bc'
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

%struct.mytest = type { i32, i32, i32 }

@.str = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

; Function Attrs: nounwind uwtable
define i32 @loop(i32 %ru, i32 %base) #0 !dbg !4 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %result = alloca i32, align 4
  %i = alloca i32, align 4
  store i32 %ru, i32* %1, align 4
  call void @llvm.dbg.declare(metadata i32* %1, metadata !20, metadata !21), !dbg !22
  store i32 %base, i32* %2, align 4
  call void @llvm.dbg.declare(metadata i32* %2, metadata !23, metadata !21), !dbg !24
  call void @llvm.dbg.declare(metadata i32* %result, metadata !25, metadata !21), !dbg !26
  %3 = load i32, i32* %2, align 4, !dbg !27
  store i32 %3, i32* %result, align 4, !dbg !26
  call void @llvm.dbg.declare(metadata i32* %i, metadata !28, metadata !21), !dbg !30
  store i32 0, i32* %i, align 4, !dbg !30
  br label %4, !dbg !31

; <label>:4                                       ; preds = %12, %0
  %5 = load i32, i32* %i, align 4, !dbg !32
  %6 = load i32, i32* %1, align 4, !dbg !35
  %7 = icmp slt i32 %5, %6, !dbg !36
  br i1 %7, label %8, label %15, !dbg !37

; <label>:8                                       ; preds = %4
  %9 = load i32, i32* %result, align 4, !dbg !38
  %10 = load i32, i32* %2, align 4, !dbg !39
  %11 = mul nsw i32 %9, %10, !dbg !40
  store i32 %11, i32* %result, align 4, !dbg !41
  br label %12, !dbg !42

; <label>:12                                      ; preds = %8
  %13 = load i32, i32* %i, align 4, !dbg !43
  %14 = add nsw i32 %13, 1, !dbg !43
  store i32 %14, i32* %i, align 4, !dbg !43
  br label %4, !dbg !45

; <label>:15                                      ; preds = %4
  %16 = load i32, i32* %result, align 4, !dbg !46
  ret i32 %16, !dbg !47
}

; Function Attrs: nounwind readnone
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: nounwind uwtable
define i32 @foo(%struct.mytest* %t) #0 !dbg !8 {
  %1 = alloca i32, align 4
  %2 = alloca %struct.mytest*, align 8
  store %struct.mytest* %t, %struct.mytest** %2, align 8
  call void @llvm.dbg.declare(metadata %struct.mytest** %2, metadata !48, metadata !21), !dbg !49
  %3 = load %struct.mytest*, %struct.mytest** %2, align 8, !dbg !50
  %4 = getelementptr inbounds %struct.mytest, %struct.mytest* %3, i32 0, i32 2, !dbg !52
  %5 = load i32, i32* %4, align 4, !dbg !52
  %6 = icmp sgt i32 %5, 0, !dbg !53
  br i1 %6, label %7, label %26, !dbg !54

; <label>:7                                       ; preds = %0
  %8 = load %struct.mytest*, %struct.mytest** %2, align 8, !dbg !55
  %9 = getelementptr inbounds %struct.mytest, %struct.mytest* %8, i32 0, i32 1, !dbg !58
  %10 = load i32, i32* %9, align 4, !dbg !58
  %11 = icmp slt i32 %10, 1, !dbg !59
  br i1 %11, label %12, label %15, !dbg !60

; <label>:12                                      ; preds = %7
  %13 = call i32 @loop(i32 3, i32 2), !dbg !61
  %14 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i32 0, i32 0), i32 %13), !dbg !63
  store i32 1, i32* %1, align 4, !dbg !65
  br label %61, !dbg !65

; <label>:15                                      ; preds = %7
  %16 = load %struct.mytest*, %struct.mytest** %2, align 8, !dbg !66
  %17 = getelementptr inbounds %struct.mytest, %struct.mytest* %16, i32 0, i32 1, !dbg !68
  %18 = load i32, i32* %17, align 4, !dbg !68
  %19 = icmp slt i32 %18, 5, !dbg !69
  br i1 %19, label %20, label %23, !dbg !70

; <label>:20                                      ; preds = %15
  %21 = call i32 @loop(i32 4, i32 2), !dbg !71
  %22 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i32 0, i32 0), i32 %21), !dbg !73
  store i32 1, i32* %1, align 4, !dbg !75
  br label %61, !dbg !75

; <label>:23                                      ; preds = %15
  %24 = call i32 @loop(i32 5, i32 2), !dbg !76
  %25 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i32 0, i32 0), i32 %24), !dbg !78
  store i32 1, i32* %1, align 4, !dbg !80
  br label %61, !dbg !80

; <label>:26                                      ; preds = %0
  %27 = load %struct.mytest*, %struct.mytest** %2, align 8, !dbg !81
  %28 = getelementptr inbounds %struct.mytest, %struct.mytest* %27, i32 0, i32 1, !dbg !84
  %29 = load i32, i32* %28, align 4, !dbg !84
  %30 = icmp slt i32 %29, 0, !dbg !85
  br i1 %30, label %31, label %58, !dbg !86

; <label>:31                                      ; preds = %26
  %32 = load %struct.mytest*, %struct.mytest** %2, align 8, !dbg !87
  %33 = getelementptr inbounds %struct.mytest, %struct.mytest* %32, i32 0, i32 2, !dbg !90
  %34 = load i32, i32* %33, align 4, !dbg !90
  %35 = icmp sgt i32 %34, -10, !dbg !91
  br i1 %35, label %36, label %47, !dbg !92

; <label>:36                                      ; preds = %31
  %37 = load %struct.mytest*, %struct.mytest** %2, align 8, !dbg !93
  %38 = getelementptr inbounds %struct.mytest, %struct.mytest* %37, i32 0, i32 1, !dbg !96
  %39 = load i32, i32* %38, align 4, !dbg !96
  %40 = icmp eq i32 %39, -1, !dbg !97
  br i1 %40, label %41, label %44, !dbg !98

; <label>:41                                      ; preds = %36
  %42 = call i32 @loop(i32 6, i32 2), !dbg !99
  %43 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i32 0, i32 0), i32 %42), !dbg !101
  store i32 1, i32* %1, align 4, !dbg !103
  br label %61, !dbg !103

; <label>:44                                      ; preds = %36
  %45 = call i32 @loop(i32 7, i32 2), !dbg !104
  %46 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i32 0, i32 0), i32 %45), !dbg !106
  store i32 1, i32* %1, align 4, !dbg !108
  br label %61, !dbg !108

; <label>:47                                      ; preds = %31
  %48 = load %struct.mytest*, %struct.mytest** %2, align 8, !dbg !109
  %49 = getelementptr inbounds %struct.mytest, %struct.mytest* %48, i32 0, i32 1, !dbg !112
  %50 = load i32, i32* %49, align 4, !dbg !112
  %51 = icmp eq i32 %50, -1, !dbg !113
  br i1 %51, label %52, label %55, !dbg !114

; <label>:52                                      ; preds = %47
  %53 = call i32 @loop(i32 8, i32 2), !dbg !115
  %54 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i32 0, i32 0), i32 %53), !dbg !117
  store i32 1, i32* %1, align 4, !dbg !119
  br label %61, !dbg !119

; <label>:55                                      ; preds = %47
  %56 = call i32 @loop(i32 9, i32 2), !dbg !120
  %57 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i32 0, i32 0), i32 %56), !dbg !122
  store i32 1, i32* %1, align 4, !dbg !124
  br label %61, !dbg !124

; <label>:58                                      ; preds = %26
  %59 = call i32 @loop(i32 10, i32 2), !dbg !125
  %60 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i32 0, i32 0), i32 %59), !dbg !127
  store i32 1, i32* %1, align 4, !dbg !129
  br label %61, !dbg !129

; <label>:61                                      ; preds = %58, %55, %52, %44, %41, %23, %20, %12
  %62 = load i32, i32* %1, align 4, !dbg !130
  ret i32 %62, !dbg !130
}

declare i32 @printf(i8*, ...) #2

attributes #0 = { nounwind uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone }
attributes #2 = { "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!17, !18}
!llvm.ident = !{!19}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "clang version 3.8.0-2ubuntu4 (tags/RELEASE_380/final)", isOptimized: false, runtimeVersion: 0, emissionKind: 1, enums: !2, subprograms: !3)
!1 = !DIFile(filename: "test11.c", directory: "/home/tuba/Documents/tools/pmguidedsymex/test/test11")
!2 = !{}
!3 = !{!4, !8}
!4 = distinct !DISubprogram(name: "loop", scope: !1, file: !1, line: 12, type: !5, isLocal: false, isDefinition: true, scopeLine: 12, flags: DIFlagPrototyped, isOptimized: false, variables: !2)
!5 = !DISubroutineType(types: !6)
!6 = !{!7, !7, !7}
!7 = !DIBasicType(name: "int", size: 32, align: 32, encoding: DW_ATE_signed)
!8 = distinct !DISubprogram(name: "foo", scope: !1, file: !1, line: 25, type: !9, isLocal: false, isDefinition: true, scopeLine: 25, flags: DIFlagPrototyped, isOptimized: false, variables: !2)
!9 = !DISubroutineType(types: !10)
!10 = !{!7, !11}
!11 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !12, size: 64, align: 64)
!12 = !DICompositeType(tag: DW_TAG_structure_type, name: "mytest", file: !1, line: 19, size: 96, align: 32, elements: !13)
!13 = !{!14, !15, !16}
!14 = !DIDerivedType(tag: DW_TAG_member, name: "nonsens", scope: !12, file: !1, line: 20, baseType: !7, size: 32, align: 32)
!15 = !DIDerivedType(tag: DW_TAG_member, name: "L", scope: !12, file: !1, line: 21, baseType: !7, size: 32, align: 32, offset: 32)
!16 = !DIDerivedType(tag: DW_TAG_member, name: "H", scope: !12, file: !1, line: 22, baseType: !7, size: 32, align: 32, offset: 64)
!17 = !{i32 2, !"Dwarf Version", i32 4}
!18 = !{i32 2, !"Debug Info Version", i32 3}
!19 = !{!"clang version 3.8.0-2ubuntu4 (tags/RELEASE_380/final)"}
!20 = !DILocalVariable(name: "ru", arg: 1, scope: !4, file: !1, line: 12, type: !7)
!21 = !DIExpression()
!22 = !DILocation(line: 12, column: 14, scope: !4)
!23 = !DILocalVariable(name: "base", arg: 2, scope: !4, file: !1, line: 12, type: !7)
!24 = !DILocation(line: 12, column: 22, scope: !4)
!25 = !DILocalVariable(name: "result", scope: !4, file: !1, line: 13, type: !7)
!26 = !DILocation(line: 13, column: 9, scope: !4)
!27 = !DILocation(line: 13, column: 18, scope: !4)
!28 = !DILocalVariable(name: "i", scope: !29, file: !1, line: 14, type: !7)
!29 = distinct !DILexicalBlock(scope: !4, file: !1, line: 14, column: 5)
!30 = !DILocation(line: 14, column: 13, scope: !29)
!31 = !DILocation(line: 14, column: 9, scope: !29)
!32 = !DILocation(line: 14, column: 18, scope: !33)
!33 = !DILexicalBlockFile(scope: !34, file: !1, discriminator: 1)
!34 = distinct !DILexicalBlock(scope: !29, file: !1, line: 14, column: 5)
!35 = !DILocation(line: 14, column: 20, scope: !33)
!36 = !DILocation(line: 14, column: 19, scope: !33)
!37 = !DILocation(line: 14, column: 5, scope: !33)
!38 = !DILocation(line: 15, column: 17, scope: !34)
!39 = !DILocation(line: 15, column: 26, scope: !34)
!40 = !DILocation(line: 15, column: 24, scope: !34)
!41 = !DILocation(line: 15, column: 15, scope: !34)
!42 = !DILocation(line: 15, column: 8, scope: !34)
!43 = !DILocation(line: 14, column: 25, scope: !44)
!44 = !DILexicalBlockFile(scope: !34, file: !1, discriminator: 2)
!45 = !DILocation(line: 14, column: 5, scope: !44)
!46 = !DILocation(line: 16, column: 13, scope: !4)
!47 = !DILocation(line: 16, column: 6, scope: !4)
!48 = !DILocalVariable(name: "t", arg: 1, scope: !8, file: !1, line: 25, type: !11)
!49 = !DILocation(line: 25, column: 24, scope: !8)
!50 = !DILocation(line: 26, column: 9, scope: !51)
!51 = distinct !DILexicalBlock(scope: !8, file: !1, line: 26, column: 9)
!52 = !DILocation(line: 26, column: 12, scope: !51)
!53 = !DILocation(line: 26, column: 14, scope: !51)
!54 = !DILocation(line: 26, column: 9, scope: !8)
!55 = !DILocation(line: 27, column: 13, scope: !56)
!56 = distinct !DILexicalBlock(scope: !57, file: !1, line: 27, column: 13)
!57 = distinct !DILexicalBlock(scope: !51, file: !1, line: 26, column: 19)
!58 = !DILocation(line: 27, column: 16, scope: !56)
!59 = !DILocation(line: 27, column: 17, scope: !56)
!60 = !DILocation(line: 27, column: 13, scope: !57)
!61 = !DILocation(line: 28, column: 27, scope: !62)
!62 = distinct !DILexicalBlock(scope: !56, file: !1, line: 27, column: 21)
!63 = !DILocation(line: 28, column: 12, scope: !64)
!64 = !DILexicalBlockFile(scope: !62, file: !1, discriminator: 1)
!65 = !DILocation(line: 29, column: 12, scope: !62)
!66 = !DILocation(line: 31, column: 18, scope: !67)
!67 = distinct !DILexicalBlock(scope: !56, file: !1, line: 31, column: 18)
!68 = !DILocation(line: 31, column: 21, scope: !67)
!69 = !DILocation(line: 31, column: 22, scope: !67)
!70 = !DILocation(line: 31, column: 18, scope: !56)
!71 = !DILocation(line: 32, column: 27, scope: !72)
!72 = distinct !DILexicalBlock(scope: !67, file: !1, line: 31, column: 26)
!73 = !DILocation(line: 32, column: 12, scope: !74)
!74 = !DILexicalBlockFile(scope: !72, file: !1, discriminator: 1)
!75 = !DILocation(line: 33, column: 12, scope: !72)
!76 = !DILocation(line: 36, column: 27, scope: !77)
!77 = distinct !DILexicalBlock(scope: !67, file: !1, line: 35, column: 14)
!78 = !DILocation(line: 36, column: 12, scope: !79)
!79 = !DILexicalBlockFile(scope: !77, file: !1, discriminator: 1)
!80 = !DILocation(line: 37, column: 12, scope: !77)
!81 = !DILocation(line: 41, column: 13, scope: !82)
!82 = distinct !DILexicalBlock(scope: !83, file: !1, line: 41, column: 13)
!83 = distinct !DILexicalBlock(scope: !51, file: !1, line: 40, column: 10)
!84 = !DILocation(line: 41, column: 16, scope: !82)
!85 = !DILocation(line: 41, column: 17, scope: !82)
!86 = !DILocation(line: 41, column: 13, scope: !83)
!87 = !DILocation(line: 42, column: 16, scope: !88)
!88 = distinct !DILexicalBlock(scope: !89, file: !1, line: 42, column: 16)
!89 = distinct !DILexicalBlock(scope: !82, file: !1, line: 41, column: 21)
!90 = !DILocation(line: 42, column: 19, scope: !88)
!91 = !DILocation(line: 42, column: 21, scope: !88)
!92 = !DILocation(line: 42, column: 16, scope: !89)
!93 = !DILocation(line: 43, column: 19, scope: !94)
!94 = distinct !DILexicalBlock(scope: !95, file: !1, line: 43, column: 19)
!95 = distinct !DILexicalBlock(scope: !88, file: !1, line: 42, column: 28)
!96 = !DILocation(line: 43, column: 22, scope: !94)
!97 = !DILocation(line: 43, column: 24, scope: !94)
!98 = !DILocation(line: 43, column: 19, scope: !95)
!99 = !DILocation(line: 44, column: 33, scope: !100)
!100 = distinct !DILexicalBlock(scope: !94, file: !1, line: 43, column: 31)
!101 = !DILocation(line: 44, column: 18, scope: !102)
!102 = !DILexicalBlockFile(scope: !100, file: !1, discriminator: 1)
!103 = !DILocation(line: 45, column: 18, scope: !100)
!104 = !DILocation(line: 48, column: 33, scope: !105)
!105 = distinct !DILexicalBlock(scope: !94, file: !1, line: 47, column: 20)
!106 = !DILocation(line: 48, column: 18, scope: !107)
!107 = !DILexicalBlockFile(scope: !105, file: !1, discriminator: 1)
!108 = !DILocation(line: 49, column: 18, scope: !105)
!109 = !DILocation(line: 53, column: 19, scope: !110)
!110 = distinct !DILexicalBlock(scope: !111, file: !1, line: 53, column: 19)
!111 = distinct !DILexicalBlock(scope: !88, file: !1, line: 52, column: 17)
!112 = !DILocation(line: 53, column: 22, scope: !110)
!113 = !DILocation(line: 53, column: 24, scope: !110)
!114 = !DILocation(line: 53, column: 19, scope: !111)
!115 = !DILocation(line: 54, column: 33, scope: !116)
!116 = distinct !DILexicalBlock(scope: !110, file: !1, line: 53, column: 31)
!117 = !DILocation(line: 54, column: 18, scope: !118)
!118 = !DILexicalBlockFile(scope: !116, file: !1, discriminator: 1)
!119 = !DILocation(line: 55, column: 18, scope: !116)
!120 = !DILocation(line: 58, column: 33, scope: !121)
!121 = distinct !DILexicalBlock(scope: !110, file: !1, line: 57, column: 20)
!122 = !DILocation(line: 58, column: 18, scope: !123)
!123 = !DILexicalBlockFile(scope: !121, file: !1, discriminator: 1)
!124 = !DILocation(line: 59, column: 18, scope: !121)
!125 = !DILocation(line: 64, column: 27, scope: !126)
!126 = distinct !DILexicalBlock(scope: !82, file: !1, line: 63, column: 14)
!127 = !DILocation(line: 64, column: 12, scope: !128)
!128 = !DILexicalBlockFile(scope: !126, file: !1, discriminator: 1)
!129 = !DILocation(line: 65, column: 12, scope: !126)
!130 = !DILocation(line: 68, column: 1, scope: !8)
