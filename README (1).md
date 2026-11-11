# Práctica 2. Algoritmos de ordenamiento: tiempo y memoria

ESCOM-IPN · Ingeniería en Inteligencia Artificial · Análisis y Diseño de Algoritmos · Grupo 3BV1
Alumna: Molina Zavaleta Angie Lalaine

Bubble sort, Mergesort, Tree sort, Heapsort y Counting sort en **C++** y **Python**.

## Archivos
- `sorts.cpp`: implementación en C++ (`g++ -O2 -std=c++17 -o sorts sorts.cpp`)
- `sorts.py`: implementación en Python (sin `sorted()` ni `list.sort()`)
- `run_bench.sh`: ejecuta todas las pruebas y escribe `results.csv`
- `plot.py`: genera `tiempo.png`, `memoria.png`, `lenguajes.png` y `tablas.json` desde `results.csv`
- `report.py`: genera el PDF del reporte
- `resultados/`: datos y gráficas obtenidas; `reporte/`: PDF final

## Uso
    ./sorts <bubble|merge|tree|heap|counting> <n>        # C++
    python3 sorts.py <bubble|merge|tree|heap|counting> <n>
    ./run_bench.sh && python3 plot.py && python3 report.py

Salida: `lang,alg,n,time_s,mem_total_mb,mem_aux_mb,ok`

## Datos y medición
Reales con 2 decimales en [0, 100000.00) (k/100, k uniforme en [0, 10^7)). Counting sort usa la clave entera round(100·x).
Tiempo = solo el ordenamiento. Memoria = pico RSS del proceso (un proceso por prueba).

## Alcance de las pruebas realizadas
Entorno: 1 vCPU, 3.9 GB RAM. C++ hasta n = 10^8 (Bubble sort hasta 10^5); Python hasta 10^6 (Counting sort hasta 10^7).
**n = 10^9 no se ejecutó** (el arreglo solo ocupa 8 GB); en el reporte esos valores son estimaciones. Ver sección 5 del reporte.
