# Plus longue sous-chaîne commune (naïf) + recherche KMP

Deux petits algorithmes sur les chaînes, chacun dans un seul fichier C++
autonome avec cas de démonstration intégrés, plus un rapport Typst avec
exemples détaillés et mesures. Tout est en français.

## Fichiers

- `longest_common_naive.cpp` — plus longue sous-chaîne commune naïve : essaie
  chaque paire de départ `(i, j)` et étend tant que les caractères coïncident.
  Temps `O(n*m*L)`, mémoire supplémentaire `O(1)`.
- `kmp_search.cpp` — recherche de motif de Knuth-Morris-Pratt (`computeLPS` +
  `kmpSearch`), chevauchements inclus. Temps `O(n+m)`, espace `O(m)`.
- `report/report.typ` — rapport complet (source Typst) : idées, listages de
  code tels quels, exemples détaillés, tableaux de mesures, conclusion, sources.
- `report/report.pdf` — rapport compilé.

## Compiler et exécuter

Depuis ce dossier :

```
g++ -O2 -Wall longest_common_naive.cpp -o lcs && ./lcs
g++ -O2 -Wall kmp_search.cpp -o kmp && ./kmp
```

Sorties attendues :

```
"abcdef" vs "zabcde" -> "abcde" (len=5) [OK]
"computer" vs "commuter" -> "uter" (len=4) [OK]
"hello" vs "world" -> "l" (len=1) [OK]
"abc" vs "xyz" -> "" (len=0) [OK]
"" vs "abc" -> "" (len=0) [OK]
```

```
text="ABABDABACDABABCABAB" pat="ABABCABAB" -> 1 hit(s) at 10
text="aaaaa" pat="aaa" -> 3 hit(s) at 0,1,2
text="hello world" pat="world" -> 1 hit(s) at 6
text="hello" pat="xyz" -> no match
text="abc" pat="" -> no match
LPS("ABABCABAB") = [0,0,1,2,0,1,2,3,4]
```

## Reconstruire le PDF

```
typst compile report/report.typ report/report.pdf
```

Nécessite l'outil Typst et les polices DejaVu Sans / DejaVu Sans Mono.

## Résumé des mesures (g++ -O2, machine locale)

| Cas | Résultat |
|---|---|
| LCS naïf, chaînes aléatoires, n=2000 | 22,6 ms / 4,16M comparaisons (quadratique) |
| LCS naïf, pire cas (`"a"*n`), n=1000 | 577 ms / 334M comparaisons (explosion cubique) |
| KMP contre naïf, entrée adverse (`"aaab"` dans `"a"*50000+"b"`) | naïf 199992 cmp / 0,38 ms contre KMP 99998 cmp / 0,18 ms |
| KMP contre naïf à n=100k | naïf 247k cmp / 0,43 ms contre KMP 132k cmp / 0,31 ms (~1,9x moins de comparaisons) |
| Borne KMP | au plus 2n comparaisons ; temps `O(n+m)`, espace `O(m)` |

Règle pratique : LCS naïf pour la clarté et les petites entrées ; KMP dès que
le texte est grand ou que le motif se chevauche lui-même.

## Sources

1. Nayuki — Recherche de motifs de Knuth-Morris-Pratt.
   https://www.nayuki.io/page/knuth-morris-pratt-string-matching
2. GeeksforGeeks — plus longue sous-chaîne commune (article sur la programmation dynamique).
3. Wikipédia — Problème de la plus longue sous-chaîne commune.
