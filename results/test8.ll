; ModuleID = 'results/test8.bc'
source_filename = "test8.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@.str = private unnamed_addr constant [22 x i8] c"%d %d %d %d %d %f %f\0A\00", align 1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
entry:
  %retval = alloca i32, align 4
  %a = alloca i32, align 4
  %b = alloca i32, align 4
  %c = alloca i32, align 4
  %d = alloca i32, align 4
  %e = alloca i32, align 4
  %f = alloca float, align 4
  %g = alloca double, align 8
  %i = alloca i32, align 4
  store i32 0, i32* %retval, align 4
  %0 = load i32, i32* %a, align 4
  %cmp = icmp sgt i32 %0, 100
  br i1 %cmp, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  store i32 10, i32* %a, align 4
  br label %if.end

if.else:                                          ; preds = %entry
  store i32 20, i32* %a, align 4
  store i32 20, i32* %b, align 4
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %1 = load i32, i32* %e, align 4
  %mul = mul nsw i32 %1, 100
  store i32 %mul, i32* %e, align 4
  %2 = load i32, i32* %c, align 4
  %mul1 = mul nsw i32 %2, 2
  store i32 %mul1, i32* %d, align 4
  store i32 0, i32* %i, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %if.end
  %3 = load i32, i32* %i, align 4
  %4 = load i32, i32* %a, align 4
  %cmp2 = icmp slt i32 %3, %4
  br i1 %cmp2, label %for.body, label %for.end

for.body:                                         ; preds = %for.cond
  %5 = load i32, i32* %i, align 4
  %6 = load i32, i32* %a, align 4
  %cmp3 = icmp slt i32 %5, %6
  br i1 %cmp3, label %if.then4, label %if.else5

if.then4:                                         ; preds = %for.body
  %7 = load double, double* %g, align 8
  %add = fadd double %7, 1.000000e+00
  store double %add, double* %g, align 8
  br label %if.end6

if.else5:                                         ; preds = %for.body
  store double 5.000000e+02, double* %g, align 8
  br label %if.end6

if.end6:                                          ; preds = %if.else5, %if.then4
  br label %for.inc

for.inc:                                          ; preds = %if.end6
  %8 = load i32, i32* %i, align 4
  %inc = add nsw i32 %8, 1
  store i32 %inc, i32* %i, align 4
  br label %for.cond

for.end:                                          ; preds = %for.cond
  %9 = load i32, i32* %a, align 4
  %10 = load i32, i32* %b, align 4
  %11 = load i32, i32* %c, align 4
  %12 = load i32, i32* %d, align 4
  %13 = load i32, i32* %e, align 4
  %14 = load float, float* %f, align 4
  %conv = fpext float %14 to double
  %15 = load double, double* %g, align 8
  %call = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([22 x i8], [22 x i8]* @.str, i64 0, i64 0), i32 %9, i32 %10, i32 %11, i32 %12, i32 %13, double %conv, double %15)
  ret i32 0
}

declare dso_local i32 @printf(i8*, ...) #1

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0-4ubuntu1 "}
