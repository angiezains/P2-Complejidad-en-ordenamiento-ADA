#!/usr/bin/env python3
"""ADA Práctica 2 - Ordenamientos en Python (implementaciones propias, sin sorted()/list.sort()).

Uso:  python3 sorts.py <bubble|merge|tree|heap|counting> <n>
Salida CSV: lang,alg,n,time_s,mem_total_mb,mem_aux_mb,ok

Datos: reales con 2 decimales en [0, 100000.00): k/100 con k uniforme en [0, K), K = 10^7.
Counting sort usa la clave entera k = round(x*100).
"""
import random
import resource
import sys
import time

K = 10_000_000


def rss_mb():
    return resource.getrusage(resource.RUSAGE_SELF).ru_maxrss / 1024.0  # KB -> MB en Linux


# ---------------------------------------------------------------- Bubble sort O(n^2)
def bubble_sort(a):
    n = len(a)
    swapped = True
    while swapped and n > 1:
        swapped = False
        for i in range(1, n):
            if a[i - 1] > a[i]:
                a[i - 1], a[i] = a[i], a[i - 1]
                swapped = True
        n -= 1


# ---------------------------------------------------------------- Mergesort O(n log n)
def _msort(a, t, lo, hi):
    if hi - lo < 2:
        return
    mid = (lo + hi) // 2
    _msort(a, t, lo, mid)
    _msort(a, t, mid, hi)
    i, j, k = lo, mid, lo
    while i < mid and j < hi:
        if a[j] < a[i]:
            t[k] = a[j]; j += 1
        else:
            t[k] = a[i]; i += 1
        k += 1
    while i < mid:
        t[k] = a[i]; i += 1; k += 1
    while j < hi:
        t[k] = a[j]; j += 1; k += 1
    a[lo:hi] = t[lo:hi]


def merge_sort(a):
    _msort(a, [0.0] * len(a), 0, len(a))


# ---------------------------------------------------------------- Heapsort O(n log n)
def _sift_down(a, n, i):
    v = a[i]
    while True:
        c = 2 * i + 1
        if c >= n:
            break
        if c + 1 < n and a[c + 1] > a[c]:
            c += 1
        if a[c] <= v:
            break
        a[i] = a[c]
        i = c
    a[i] = v


def heap_sort(a):
    n = len(a)
    for i in range(n // 2 - 1, -1, -1):
        _sift_down(a, n, i)
    for e in range(n - 1, 0, -1):
        a[0], a[e] = a[e], a[0]
        _sift_down(a, e, 0)


# ---------------------------------------------------------------- Tree sort (BST sin balancear)
# Listas paralelas (valor, hijo izq., hijo der.) en lugar de objetos para ahorrar memoria.
def tree_sort(a):
    n = len(a)
    if n == 0:
        return
    val = list(a)
    left = [-1] * n
    right = [-1] * n
    for i in range(1, n):
        x = val[i]
        cur = 0
        while True:
            if x < val[cur]:
                nxt = left[cur]
                if nxt < 0:
                    left[cur] = i
                    break
            else:
                nxt = right[cur]
                if nxt < 0:
                    right[cur] = i
                    break
            cur = nxt
    stack = []
    cur = 0
    k = 0
    while cur >= 0 or stack:
        while cur >= 0:
            stack.append(cur)
            cur = left[cur]
        cur = stack.pop()
        a[k] = val[cur]
        k += 1
        cur = right[cur]


# ---------------------------------------------------------------- Counting sort O(n + K)
def counting_sort(a):
    n = len(a)
    c = [0] * (K + 1)
    for x in a:
        c[round(x * 100) + 1] += 1
    for i in range(1, K + 1):
        c[i] += c[i - 1]
    out = [0.0] * n
    for x in a:
        key = round(x * 100)
        out[c[key]] = x
        c[key] += 1
    a[:] = out


ALGS = {"bubble": bubble_sort, "merge": merge_sort, "tree": tree_sort,
        "heap": heap_sort, "counting": counting_sort}


def main():
    if len(sys.argv) != 3 or sys.argv[1] not in ALGS:
        sys.exit("uso: sorts.py <bubble|merge|tree|heap|counting> <n>")
    alg, n = sys.argv[1], int(sys.argv[2])
    sys.setrecursionlimit(10000)
    rng = random.Random(42)
    a = []
    sum0 = 0
    for _ in range(n):  # sin lista auxiliar de claves, para que el pico base sea solo el arreglo
        k = rng.randrange(K)
        sum0 += k
        a.append(k / 100.0)
    base = rss_mb()

    t0 = time.perf_counter()
    ALGS[alg](a)
    secs = time.perf_counter() - t0
    peak = rss_mb()

    ok = all(a[i - 1] <= a[i] for i in range(1, n)) and sum(round(x * 100) for x in a) == sum0
    print(f"python,{alg},{n},{secs:.4f},{peak:.1f},{peak - base:.1f},{int(ok)}")
    sys.exit(0 if ok else 1)


if __name__ == "__main__":
    main()
