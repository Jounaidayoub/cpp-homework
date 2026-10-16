# Plus longue sous-chaîne répétée (naïf) + recherche KMP

Deux petits algorithmes sur les chaînes, chacun dans un seul fichier C++
autonome avec cas de démonstration intégrés, plus un rapport Typst avec
exemples détaillés et mesures. Tout est en français.

## Fichiers

- `longest_repeated_naive.cpp` — plus longue sous-chaîne répétée naïve : dans un
  seul texte, essaie chaque paire de départ `(i, j)` avec `j > i` et étend tant
  que les caractères coïncident (chevauchements autorisés : `"aaaa"` donne
  `"aaa"`). Temps `O(n^2*L)`, mémoire supplémentaire `O(1)`.
- `kmp_search.cpp` — recherche de motif de Knuth-Morris-Pratt (`computeLPS` +
  `kmpSearch`), chevauchements inclus. Temps `O(n+m)`, espace `O(m)`.
- `report/report.typ` — rapport complet (source Typst) : idées, listages de
  code tels quels, exemples détaillés, tableaux de mesures, conclusion, sources.
- `report/report.pdf` — rapport compilé.

## Compiler et exécuter

Depuis ce dossier :

```
g++ -O2 -Wall longest_repeated_naive.cpp -o rep && ./rep
g++ -O2 -Wall kmp_search.cpp -o kmp && ./kmp
```

Sorties attendues :

```
"banana" -> "ana" (len=3, at 1 and 3) [OK]
"abcdabc" -> "abc" (len=3, at 0 and 4) [OK]
"aaaa" -> "aaa" (len=3, at 0 and 1) [OK]
"abcdef" -> "" (len=0, at 0 and 0) [OK]
"" -> "" (len=0, at 0 and 0) [OK]
"the cat sat. the dog ran. the cat sat." -> "the cat sat." (len=12, at 0 and 26) [OK]
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

Machine : `g++ (Ubuntu 15.2.0-16ubuntu1) 15.2.0`, `-O2`,
CPU Intel Core i7-8665U à 1,90 GHz. Chaque mesure répétée 3 fois, meilleur
temps gardé (livre entier : 1 seul passage). Le programme n'affiche pas de
compteur de comparaisons : seuls le temps, la longueur et la répétition
trouvée sont reportés.

Répétition naïve sur Alice au pays des merveilles (`alice_story.txt`,
texte du Projet Gutenberg, préfixes de N caractères) :

| N | Temps (meilleur de 3) | Répétition trouvée |
|---|---|---|
| 2000 | 6,7 ms | len 21 en 15 et 483 : "Down the Rabbit-Hole" |
| 4000 | 26,9 ms | len 21, idem |
| 8000 | 108 ms | len 21, idem |
| 16000 | 442 ms | len 136 en 9173 et 12116 : ornement "* * *" |
| 32000 | 1079 ms | len 136, idem |
| 162949 (livre entier) | 27,4 s | len 145 en 118654 et 119452 : refrain "Will you, won't you..." |

Commande utilisée : `g++ -O2 -Wall longest_repeated_naive.cpp -o /tmp/rep && /tmp/rep alice_story.txt N`.

| Cas | Résultat |
|---|---|
| KMP contre naïf, entrée adverse (`"aaab"` dans `"a"*50000+"b"`) | naïf 199992 cmp / 0,38 ms contre KMP 99998 cmp / 0,18 ms |
| KMP contre naïf à n=100k | naïf 247k cmp / 0,43 ms contre KMP 132k cmp / 0,31 ms (~1,9x moins de comparaisons) |
| Borne KMP | au plus 2n comparaisons ; temps `O(n+m)`, espace `O(m)` |

Règle pratique : répétition naïve pour la clarté et les petites entrées (sur un
livre, le `O(n^2)` est infaisable : tableau des suffixes ou automate, hors
sujet ici) ; KMP dès que le texte est grand ou que le motif se chevauche
lui-même.

## Sources

1. Nayuki — Recherche de motifs de Knuth-Morris-Pratt.
   https://www.nayuki.io/page/knuth-morris-pratt-string-matching
2. GeeksforGeeks — plus longue sous-chaîne commune (article sur la programmation dynamique).
3. Wikipédia — Problème de la plus longue sous-chaîne commune.
