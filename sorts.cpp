// ADA Práctica 2 - Ordenamientos en C++
// Compilar: g++ -O2 -std=c++17 -o sorts sorts.cpp
// Uso:      ./sorts <bubble|merge|tree|heap|counting> <n>
// Salida CSV: lang,alg,n,time_s,mem_total_mb,mem_aux_mb,ok
//
// Datos: reales con 2 decimales en [0, 100000.00), k/100.0 con k uniforme en [0, K), K = 10^7.
// (Counting sort solo es aplicable a reales si tienen precisión y rango acotados: la clave
//  entera es k = round(x*100).)
#include <sys/resource.h>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <random>
#include <string>
#include <vector>
using namespace std;

static const int64_t K = 10000000;

static double rss_mb() {  // pico de memoria residente del proceso (MB)
    rusage r;
    getrusage(RUSAGE_SELF, &r);
    return r.ru_maxrss / 1024.0;  // en Linux ru_maxrss está en KB
}

// ---------------------------------------------------------------- Bubble sort  O(n^2), in situ
void bubble_sort(vector<double>& a) {
    size_t n = a.size();
    bool swapped = true;
    while (swapped && n > 1) {
        swapped = false;
        for (size_t i = 1; i < n; ++i)
            if (a[i - 1] > a[i]) { swap(a[i - 1], a[i]); swapped = true; }
        --n;  // el mayor ya quedó al final
    }
}

// ---------------------------------------------------------------- Mergesort  O(n log n), buffer auxiliar de n
static void msort(double* a, double* t, size_t n) {
    if (n < 2) return;
    size_t m = n / 2;
    msort(a, t, m);
    msort(a + m, t, n - m);
    size_t i = 0, j = m, k = 0;
    while (i < m && j < n) t[k++] = (a[j] < a[i]) ? a[j++] : a[i++];
    while (i < m) t[k++] = a[i++];
    while (j < n) t[k++] = a[j++];
    memcpy(a, t, n * sizeof(double));
}
void merge_sort(vector<double>& a) {
    vector<double> t(a.size());
    msort(a.data(), t.data(), a.size());
}

// ---------------------------------------------------------------- Heapsort  O(n log n), in situ
static void sift_down(double* a, size_t n, size_t i) {
    double v = a[i];
    for (;;) {
        size_t c = 2 * i + 1;
        if (c >= n) break;
        if (c + 1 < n && a[c + 1] > a[c]) ++c;
        if (a[c] <= v) break;
        a[i] = a[c];
        i = c;
    }
    a[i] = v;
}
void heap_sort(vector<double>& a) {
    size_t n = a.size();
    for (size_t i = n / 2; i-- > 0;) sift_down(a.data(), n, i);
    for (size_t e = n; e-- > 1;) { swap(a[0], a[e]); sift_down(a.data(), e, 0); }
}

// ---------------------------------------------------------------- Tree sort (BST sin balancear)
// Esperado O(n log n) con datos aleatorios; O(n^2) en el peor caso (datos ya ordenados).
// Un nodo por elemento (16 B: valor + 2 índices int32). Duplicados van a la derecha.
struct Node { double v; int32_t l, r; };
void tree_sort(vector<double>& a) {
    size_t n = a.size();
    if (n == 0) return;
    vector<Node> pool;
    pool.reserve(n);
    pool.push_back({a[0], -1, -1});
    for (size_t i = 1; i < n; ++i) {
        int32_t idx = (int32_t)pool.size();
        double x = a[i];
        int32_t cur = 0;
        for (;;) {
            if (x < pool[cur].v) {
                if (pool[cur].l < 0) { pool[cur].l = idx; break; }
                cur = pool[cur].l;
            } else {
                if (pool[cur].r < 0) { pool[cur].r = idx; break; }
                cur = pool[cur].r;
            }
        }
        pool.push_back({x, -1, -1});
    }
    // recorrido inorden iterativo (sin recursión)
    vector<int32_t> st;
    int32_t cur = 0;
    size_t k = 0;
    while (cur >= 0 || !st.empty()) {
        while (cur >= 0) { st.push_back(cur); cur = pool[cur].l; }
        cur = st.back(); st.pop_back();
        a[k++] = pool[cur].v;
        cur = pool[cur].r;
    }
}

// ---------------------------------------------------------------- Counting sort  O(n + K)
// clave = round(x*100) en [0, K). Versión estable con sumas prefijas y arreglo de salida.
void counting_sort(vector<double>& a) {
    size_t n = a.size();
    vector<uint32_t> c(K + 1, 0);
    for (size_t i = 0; i < n; ++i) ++c[(size_t)llround(a[i] * 100.0) + 1];
    for (int64_t i = 1; i <= K; ++i) c[i] += c[i - 1];  // c[k] = posición inicial de la clave k
    vector<double> out(n);
    for (size_t i = 0; i < n; ++i) out[c[(size_t)llround(a[i] * 100.0)]++] = a[i];
    a.swap(out);
}

int main(int argc, char** argv) {
    if (argc != 3) { fprintf(stderr, "uso: %s <bubble|merge|tree|heap|counting> <n>\n", argv[0]); return 2; }
    string alg = argv[1];
    size_t n = strtoull(argv[2], nullptr, 10);

    vector<double> a(n);
    mt19937_64 rng(42);
    uint64_t sum0 = 0;
    for (size_t i = 0; i < n; ++i) {
        int64_t k = (int64_t)(rng() % K);
        a[i] = k / 100.0;
        sum0 += (uint64_t)k;
    }
    double base = rss_mb();  // memoria con solo el arreglo de entrada

    auto t0 = chrono::steady_clock::now();
    if (alg == "bubble") bubble_sort(a);
    else if (alg == "merge") merge_sort(a);
    else if (alg == "tree") tree_sort(a);
    else if (alg == "heap") heap_sort(a);
    else if (alg == "counting") counting_sort(a);
    else { fprintf(stderr, "algoritmo desconocido\n"); return 2; }
    double secs = chrono::duration<double>(chrono::steady_clock::now() - t0).count();
    double peak = rss_mb();

    // verificación: ordenado y es permutación (misma suma de claves)
    bool ok = true;
    uint64_t sum1 = 0;
    for (size_t i = 0; i < n; ++i) {
        sum1 += (uint64_t)llround(a[i] * 100.0);
        if (i && a[i - 1] > a[i]) ok = false;
    }
    if (sum0 != sum1) ok = false;

    printf("cpp,%s,%zu,%.4f,%.1f,%.1f,%d\n", alg.c_str(), n, secs, peak, peak - base, ok ? 1 : 0);
    return ok ? 0 : 1;
}
