# C++ Homework — Algorithmes sur les chaînes

Travaux pratiques C++ : recherche de motif et sous-chaînes communes,
avec rapports Typst et benchmarks mesurés.

## Contenu

| Dossier | Sujet | Code | Rapport |
|---|---|---|---|
| [01-contient-sous-sequence](01-contient-sous-sequence/) | `contient(texte, mot)` : sous-séquence dans le même ordre + recherche naïve dans un fichier | `algo.cpp`, `search_file.cpp` | `rapport.typ` / `rapport.pdf` |
| [02-longest-substring-kmp](02-longest-substring-kmp/) | Plus longue sous-chaîne commune (naïf) + recherche de motif KMP | `longest_common_naive.cpp`, `kmp_search.cpp` | `report/report.typ` / `report/report.pdf` |

## Compiler et exécuter

```bash
# Assignment 1
g++ -O2 -Wall -o /tmp/algo 01-contient-sous-sequence/algo.cpp
g++ -O2 -Wall -o /tmp/search 01-contient-sous-sequence/search_file.cpp

# Assignment 2
g++ -O2 -Wall -o /tmp/lcs 02-longest-substring-kmp/longest_common_naive.cpp && /tmp/lcs
g++ -O2 -Wall -o /tmp/kmp 02-longest-substring-kmp/kmp_search.cpp && /tmp/kmp
```

## Recompiler un rapport

```bash
typst compile 02-longest-substring-kmp/report/report.typ 02-longest-substring-kmp/report/report.pdf
```

Voir le `README.md` de chaque dossier pour les détails (énoncé, complexités, benchmarks).
