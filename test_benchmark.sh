#! /bin/bash
bench_1=benchmark/benchmark_01;
bench_2=benchmark/benchmark_02;
bench_3=benchmark/benchmark_03;
bench_4=benchmark/benchmark_04;
bench_5=benchmark/benchmark_05;
bench_6=benchmark/benchmark_06;

for j in {1..5}
do
    for i in $bench_1 $bench_2 $bench_3 $bench_4 $bench_5 $bench_6
    do
        echo $i;
        ./bin/computePatchOpt $i/source $i/target > $i/patch;
        ./bin/applyPatch $i/patch $i/source > $i/targetPatched -j8;
        diff $i/target $i/targetPatched;
    done
done
