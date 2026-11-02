# 04 — Tas manuel et `std::priority_queue`

Comparaison d'un tas max implémenté avec `std::vector` et du conteneur
standard `std::priority_queue<int>`.

## Compiler et tester

```text
g++ -std=c++17 -O2 -Wall -Wextra -pedantic heap_benchmark.cpp -o heap_benchmark
./heap_benchmark --test
```

## Benchmark et graphiques

```text
./heap_benchmark --csv > benchmark.csv
uv run --with matplotlib python plot_benchmarks.py benchmark.csv report/graphs
typst compile report/report.typ report/report.pdf
```

Le benchmark utilise 100 000, 500 000 et 1 000 000 entiers distincts, la
graine `2026` et trois répétitions. Il mesure la construction par insertions,
la construction bottom-up par heapify, `top()` répété, le retrait de tous les
éléments par `pop()` et la libération du stockage après vidage.
