# 03 — Comparaison entre un ABR et `std::map`

Implémentation manuelle d'un arbre binaire de recherche et comparaison avec
`std::map<int, int>` sur les mêmes opérations.

## Fichiers

- `bst.cpp` — ABR, insertion, recherche, suppression, tests et banc de test.
- `report/report.typ` — rapport Typst en français.
- `report/report.pdf` — rapport compilé après les mesures.
- `benchmark.csv` — moyennes de trois exécutions sur 200 000 valeurs.

## Compiler et tester

```text
g++ -O2 -Wall -Wextra -pedantic bst.cpp -o bst
./bst --test
```

## Benchmark

```text
./bst --n 200000 --seed 2026 --runs 3 --csv
```

Le programme génère les entiers distincts `1..200000`, les mélange avec
`std::mt19937` et la graine `2026`, puis mesure séparément l'insertion, la
recherche et la suppression pour l'ABR et `std::map`. Le contrôle attendu est
`1 200 000` par structure.

## Rapport

```text
typst compile report/report.typ report/report.pdf
```
