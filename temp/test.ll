; ModuleID = 'test.bc'
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct.device = type { i32, [8 x i8], [1 x %struct.data] }
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
  br label %46

; <label>:5                                       ; preds = %0
  %6 = load %struct.device*, %struct.device** %1, align 8
  %7 = getelementptr inbounds %struct.device, %struct.device* %6, i32 0, i32 0
  %8 = load i32, i32* %7, align 4
  %9 = icmp sgt i32 %8, 5
  br i1 %9, label %10, label %33

; <label>:10                                      ; preds = %5
  %11 = load %struct.device*, %struct.device** %1, align 8
  %12 = getelementptr inbounds %struct.device, %struct.device* %11, i32 0, i32 2
  %13 = getelementptr inbounds [1 x %struct.data], [1 x %struct.data]* %12, i32 0, i32 0
  %14 = getelementptr inbounds %struct.data, %struct.data* %13, i32 0, i32 0
  %15 = getelementptr inbounds [8 x i8], [8 x i8]* %14, i64 0, i64 0
  %16 = load i8, i8* %15, align 4
  %17 = sext i8 %16 to i32
  %18 = icmp eq i32 %17, 67
  br i1 %18, label %19, label %21

; <label>:19                                      ; preds = %10
  %20 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([11 x i8], [11 x i8]* @.str, i32 0, i32 0))
  br label %21

; <label>:21                                      ; preds = %19, %10
  %22 = load %struct.device*, %struct.device** %1, align 8
  %23 = getelementptr inbounds %struct.device, %struct.device* %22, i32 0, i32 1
  %24 = getelementptr inbounds [8 x i8], [8 x i8]* %23, i64 0, i64 0
  %25 = load i8, i8* %24, align 4
  %26 = sext i8 %25 to i32
  %27 = icmp eq i32 %26, 65
  br i1 %27, label %28, label %30

; <label>:28                                      ; preds = %21
  %29 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str.1, i32 0, i32 0))
  br label %32

; <label>:30                                      ; preds = %21
  %31 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([10 x i8], [10 x i8]* @.str.2, i32 0, i32 0))
  br label %32

; <label>:32                                      ; preds = %30, %28
  br label %45

; <label>:33                                      ; preds = %5
  %34 = load %struct.device*, %struct.device** %1, align 8
  %35 = getelementptr inbounds %struct.device, %struct.device* %34, i32 0, i32 1
  %36 = getelementptr inbounds [8 x i8], [8 x i8]* %35, i64 0, i64 1
  %37 = load i8, i8* %36, align 1
  %38 = sext i8 %37 to i32
  %39 = icmp eq i32 %38, 66
  br i1 %39, label %40, label %42

; <label>:40                                      ; preds = %33
  %41 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @.str.3, i32 0, i32 0))
  br label %44

; <label>:42                                      ; preds = %33
  %43 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([11 x i8], [11 x i8]* @.str.4, i32 0, i32 0))
  br label %44

; <label>:44                                      ; preds = %42, %40
  br label %45

; <label>:45                                      ; preds = %44, %32
  br label %46

; <label>:46                                      ; preds = %4, %45
  ret void
}

declare i32 @printf(i8*, ...) #1

; Function Attrs: nounwind uwtable
define i32 @main(i32 %argc, i8** %argv) #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i8**, align 8
  %locdev = alloca %struct.device, align 4
  store i32 0, i32* %1, align 4
  store i32 %argc, i32* %2, align 4
  store i8** %argv, i8*** %3, align 8
  %4 = bitcast %struct.device* %locdev to i8*
  call void @klee_make_symbolic(i8* %4, i64 20, i8* getelementptr inbounds ([7 x i8], [7 x i8]* @.str.5, i32 0, i32 0))
  call void @api1(%struct.device* %locdev)
  ret i32 0
}

declare void @klee_make_symbolic(i8*, i64, i8*) #1

attributes #0 = { nounwind uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.ident = !{!0}

!0 = !{!"clang version 3.8.0-2ubuntu3~trusty5 (tags/RELEASE_380/final)"}
