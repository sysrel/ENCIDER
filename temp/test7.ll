; ModuleID = 'test7.bc'
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct.device = type { i32, i8*, [1 x %struct.data] }
%struct.data = type { [8 x i8] }

@.str = private unnamed_addr constant [11 x i8] c">5 data C\0A\00", align 1
@.str.1 = private unnamed_addr constant [6 x i8] c">5 A\0A\00", align 1
@.str.2 = private unnamed_addr constant [10 x i8] c">5 not A\0A\00", align 1
@.str.3 = private unnamed_addr constant [7 x i8] c"<=5 B\0A\00", align 1
@.str.4 = private unnamed_addr constant [11 x i8] c"<=5 not B\0A\00", align 1
@.str.5 = private unnamed_addr constant [7 x i8] c"locdev\00", align 1

; Function Attrs: nounwind uwtable
define void @api1(%struct.device* %dev) #0 {
  %1 = alloca %struct.device*, align 8
  store %struct.device* %dev, %struct.device** %1, align 8
  %2 = load %struct.device*, %struct.device** %1, align 8
  %3 = icmp eq %struct.device* %2, null
  br i1 %3, label %4, label %5

; <label>:4                                       ; preds = %0
  br label %48

; <label>:5                                       ; preds = %0
  %6 = load %struct.device*, %struct.device** %1, align 8
  %7 = getelementptr inbounds %struct.device, %struct.device* %6, i32 0, i32 0
  %8 = load i32, i32* %7, align 8
  %9 = icmp sgt i32 %8, 5
  br i1 %9, label %10, label %34

; <label>:10                                      ; preds = %5
  %11 = load %struct.device*, %struct.device** %1, align 8
  %12 = getelementptr inbounds %struct.device, %struct.device* %11, i32 0, i32 2
  %13 = getelementptr inbounds [1 x %struct.data], [1 x %struct.data]* %12, i32 0, i32 0
  %14 = getelementptr inbounds %struct.data, %struct.data* %13, i32 0, i32 0
  %15 = getelementptr inbounds [8 x i8], [8 x i8]* %14, i64 0, i64 0
  %16 = load i8, i8* %15, align 8
  %17 = sext i8 %16 to i32
  %18 = icmp eq i32 %17, 67
  br i1 %18, label %19, label %21

; <label>:19                                      ; preds = %10
  %20 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([11 x i8], [11 x i8]* @.str, i32 0, i32 0))
  br label %21

; <label>:21                                      ; preds = %19, %10
  %22 = load %struct.device*, %struct.device** %1, align 8
  %23 = getelementptr inbounds %struct.device, %struct.device* %22, i32 0, i32 1
  %24 = load i8*, i8** %23, align 8
  %25 = getelementptr inbounds i8, i8* %24, i64 0
  %26 = load i8, i8* %25, align 1
  %27 = sext i8 %26 to i32
  %28 = icmp eq i32 %27, 65
  br i1 %28, label %29, label %31

; <label>:29                                      ; preds = %21
  %30 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str.1, i32 0, i32 0))
  br label %33

; <label>:31                                      ; preds = %21
  %32 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([10 x i8], [10 x i8]* @.str.2, i32 0, i32 0))
  br label %33

; <label>:33                                      ; preds = %31, %29
  br label %47

; <label>:34                                      ; preds = %5
  %35 = load %struct.device*, %struct.device** %1, align 8
  %36 = getelementptr inbounds %struct.device, %struct.device* %35, i32 0, i32 1
  %37 = load i8*, i8** %36, align 8
  %38 = getelementptr inbounds i8, i8* %37, i64 1
  %39 = load i8, i8* %38, align 1
  %40 = sext i8 %39 to i32
  %41 = icmp eq i32 %40, 66
  br i1 %41, label %42, label %44

; <label>:42                                      ; preds = %34
  %43 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @.str.3, i32 0, i32 0))
  br label %46

; <label>:44                                      ; preds = %34
  %45 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([11 x i8], [11 x i8]* @.str.4, i32 0, i32 0))
  br label %46

; <label>:46                                      ; preds = %44, %42
  br label %47

; <label>:47                                      ; preds = %46, %33
  br label %48

; <label>:48                                      ; preds = %4, %47
  ret void
}

declare i32 @printf(i8*, ...) #1

; Function Attrs: nounwind uwtable
define i32 @main(i32 %argc, i8** %argv) #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i8**, align 8
  %locdev = alloca %struct.device, align 8
  store i32 0, i32* %1, align 4
  store i32 %argc, i32* %2, align 4
  store i8** %argv, i8*** %3, align 8
  %4 = call noalias i8* @malloc(i64 8) #3
  %5 = getelementptr inbounds %struct.device, %struct.device* %locdev, i32 0, i32 1
  store i8* %4, i8** %5, align 8
  %6 = bitcast %struct.device* %locdev to i8*
  call void @klee_make_symbolic(i8* %6, i64 24, i8* getelementptr inbounds ([7 x i8], [7 x i8]* @.str.5, i32 0, i32 0))
  call void @api1(%struct.device* %locdev)
  ret i32 0
}

; Function Attrs: nounwind
declare noalias i8* @malloc(i64) #2

declare void @klee_make_symbolic(i8*, i64, i8*) #1

attributes #0 = { nounwind uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { nounwind "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { nounwind }

!llvm.ident = !{!0}

!0 = !{!"clang version 3.8.0-2ubuntu3~trusty5 (tags/RELEASE_380/final)"}
