#!/usr/bin/env bash
# Ejecuta los benchmarks (un proceso por combinación, para medir memoria pico de cada una).
# Uso: ./run_bench.sh            -> resultados en results.csv
# Ajusta las listas de tamaños según la RAM y el tiempo de tu equipo.
set -u
g++ -O2 -std=c++17 -o sorts sorts.cpp || exit 1
OUT=results.csv
LIMIT=${LIMIT:-1500}   # segundos máximos por ejecución
echo "lang,alg,n,time_s,mem_total_mb,mem_aux_mb,ok" > "$OUT"

run() { timeout "$LIMIT" "$@" >> "$OUT" || echo "# fallo/timeout ($?): $*" >> log.txt; }

# --- C++
for n in 10000 100000; do run ./sorts bubble $n; done
for n in 100000 1000000 10000000 100000000; do
  for alg in counting merge heap tree; do run ./sorts $alg $n; done
done
# 1,000,000,000: ~8 GB solo el arreglo (16 GB con mergesort/counting). Descomenta si tienes >=32 GB de RAM:
# for alg in heap; do run ./sorts $alg 1000000000; done

# --- Python (puro)
for n in 1000 10000; do run python3 sorts.py bubble $n; done
for n in 100000 1000000; do
  for alg in counting merge heap tree; do run python3 sorts.py $alg $n; done
done
run python3 sorts.py counting 10000000
echo "FIN" >> log.txt
