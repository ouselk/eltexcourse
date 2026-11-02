#!/bin/sh
# сборка и запуск бенчмарка
gcc -O2 -o bench_time bench.c linkedlist.c phonebook.c
gcc -O2 -DLIST_COUNT_STEPS -o bench_steps bench.c linkedlist.c phonebook.c
gcc -O2 -o phonebook_menu main.c linkedlist.c phonebook.c
./bench_steps names100k.csv 1 test_real.csv test_uniform.csv test_indep.csv
./bench_time names100k.csv 5 test_real.csv test_uniform.csv test_indep.csv
