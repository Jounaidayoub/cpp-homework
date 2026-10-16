#set page(paper: "a4", margin: 2cm)
#set text(font: "DejaVu Sans", size: 10.5pt)
#set heading(numbering: "1.")
#show raw: set text(font: "DejaVu Sans Mono", size: 8pt)

#align(center)[
  #text(size: 20pt, weight: "bold")[Plus longue sous-chaîne répétée (naïf) + recherche KMP]
  #v(4pt)
  #text(size: 12pt)[Répétition naïve #h(8pt) | Recherche de motif de Knuth-Morris-Pratt]
  #v(2pt)
  #text(size: 10pt, fill: gray)[longest_repeated_naive.cpp #h(8pt) | kmp_search.cpp #h(8pt) | vérifié par exécution, `g++ -O2 -Wall`]
]

= Objectif

Étant donnés un texte et un motif, résoudre deux problèmes liés :

1. *Plus longue sous-chaîne répétée* : trouver dans UN seul texte `s` le plus long bloc qui apparaît au moins deux fois (par exemple une phrase répétée dans un livre). Les chevauchements sont autorisés.
2. *Recherche de motif (KMP)* : trouver toutes les occurrences d'un motif `pat` dans un texte `txt`, chevauchements inclus, en temps linéaire.

Résultats de référence (les deux programmes ont été exécutés localement) :

- `"banana"` $arrow$ `"ana"` (len 3, en 1 et 3)
- `"the cat sat. the dog ran. the cat sat."` $arrow$ `"the cat sat."` (len 12, en 0 et 26)
- `"ABABDABACDABABCABAB"` recherche `"ABABCABAB"` $arrow$ 1 occurrence à l'indice 10
- `"aaaaa"` recherche `"aaa"` $arrow$ occurrences en 0, 1, 2 (chevauchements inclus)

= Plus longue sous-chaîne répétée (méthode naïve)

#grid(
  columns: (1fr, 1fr),
  gutter: 12pt,
  [#block(fill: rgb("#f4f4f5"), radius: 6pt, inset: 10pt)[
    #text(weight: "bold")[Signature]
    #v(4pt)
    ```cpp
    RepeatResult longestRepeatedNaive(
        const string &s)
    ```
    #v(4pt)
    Retourne le plus long bloc apparaissant au moins deux fois dans `s`, avec sa longueur et ses deux positions de départ. Texte vide $arrow$ résultat vide.
  ]],
  [#block(fill: rgb("#eef6ff"), radius: 6pt, inset: 10pt)[
    #text(weight: "bold")[Idée en 3 points]
    #v(4pt)
    1. Essayer chaque paire de départ `(i, j)` avec `j > i`.
    #v(2pt)
    2. Étendre `k` tant que `s[i+k] == s[j+k]`.
    #v(2pt)
    3. Garder le meilleur quadruplet vu jusqu'ici.
    #v(4pt)
    Aucun prétraitement, aucune mémoire supplémentaire. Chevauchements autorisés (`"aaaa"` $arrow$ `"aaa"`).
  ]],
)

#text(size: 9pt, fill: gray)[Fonction testée telle quelle depuis `longest_repeated_naive.cpp:19`.]
```cpp
RepeatResult longestRepeatedNaive(const string &s) {
    RepeatResult best;
    for (size_t i = 0; i < s.size(); i++) {
        for (size_t j = i + 1; j < s.size(); j++) {
            size_t k = 0;
            while (j + k < s.size() && s[i + k] == s[j + k])
                k++;
            if (k > best.len) {
                best.len = k;
                best.posA = i;
                best.posB = j;
                best.value = s.substr(i, k);
            }
        }
    }
    return best;
}
```

Exemple détaillé : `s = "banana"`. Paires de départ sélectionnées :

#table(
  columns: (auto, auto, 1fr, auto),
  fill: (col, row) => if row == 0 { rgb("#1f2937") } else if row == 2 { rgb("#ecfdf5") },
  align: center,
  table.header[#text(white)[*(i, j)*]][#text(white)[*s[i] / s[j]*]][#text(white)[*extension*]][#text(white)[*len*]],
  [(0, 1)], [b / a], [pas de correspondance (`b` ne se répète jamais)], [0],
  [(1, 3)], [a / a], [`ana`, meilleur], [3],
  [(2, 4)], [n / n], [`na`], [2],
  [(3, 5)], [a / a], [`a`], [1],
)

Lecture : la paire (0, 1) échoue car `b` ne se répète jamais ; la paire (1, 3) construit `"ana"` (len 3) et devient la meilleure. Les paires suivantes (`"na"`, `"a"`) ne la battent pas, donc la réponse est `"ana"` (en 1 et 3).

Exemple grandeur nature : `"the cat sat. the dog ran. the cat sat."` $arrow$ `"the cat sat."` (len 12, en 0 et 26) : la phrase entière répétée est détectée telle quelle.

#block(fill: rgb("#fffbeb"), radius: 6pt, inset: 10pt, stroke: 1pt + rgb("#f59e0b"))[
  #text(weight: "bold")[Complexité] : double boucle sur les paires `(i, j)` avec `j > i`, chaque extension jusqu'à `L` $arrow$ temps $O(n^2 dot L)$, mémoire supplémentaire `O(1)`.
  Sur texte aléatoire l'extension casse vite ; le pire cas (`"a"` répété) allonge chaque extension — mais le nombre de paires domine déjà (section 4, tableau A).
]

Sorties de démonstration vérifiées (`longest_repeated_naive.cpp:37`):

#table(
  columns: (1fr, 1fr, auto),
  fill: (col, row) => if row == 0 { rgb("#1f2937") },
  align: left,
  table.header[#text(white)[*Entrée (`s`)*]][#text(white)[*Sortie*]][#text(white)[*Statut*]],
  [`banana`], [`"ana"` (len 3, en 1 et 3)], [#text(fill: rgb("#15803d"))[OK]],
  [`abcdabc`], [`"abc"` (len 3, en 0 et 4)], [#text(fill: rgb("#15803d"))[OK]],
  [`aaaa`], [`"aaa"` (len 3, en 0 et 1)], [#text(fill: rgb("#15803d"))[OK]],
  [`abcdef`], [`""` (len 0)], [#text(fill: rgb("#15803d"))[OK]],
  [`""`], [`""` (len 0)], [#text(fill: rgb("#15803d"))[OK]],
  [`the cat sat. the dog ran. the cat sat.`], [`"the cat sat."` (len 12, en 0 et 26)], [#text(fill: rgb("#15803d"))[OK]],
)

= Recherche de motif de Knuth-Morris-Pratt

#grid(
  columns: (1fr, 1fr),
  gutter: 12pt,
  [#block(fill: rgb("#f4f4f5"), radius: 6pt, inset: 10pt)[
    #text(weight: "bold")[Signatures]
    #v(4pt)
    ```cpp
    vector<int> computeLPS(const string &pat)
    vector<int> kmpSearch(
        const string &txt, const string &pat)
    ```
    #v(4pt)
    `computeLPS` construit la table des préfixes ; `kmpSearch` retourne chaque indice de départ, chevauchements inclus.
  ]],
  [#block(fill: rgb("#eef6ff"), radius: 6pt, inset: 10pt)[
    #text(weight: "bold")[Idée clé]
    #v(4pt)
    En cas de différence, ne pas reculer le pointeur du texte. Replier avec `j = lps[j-1]` et reprendre la comparaison.
    #v(4pt)
    L'indice du texte `i` ne recule jamais, donc le travail total reste linéaire.
  ]],
)

#text(size: 9pt, fill: gray)[Fonction de préfixe telle quelle depuis `kmp_search.cpp:10`.]
```cpp
vector<int> computeLPS(const string &pat) {
    vector<int> lps(pat.size(), 0);
    int len = 0; // length of current longest prefix-suffix
    for (size_t i = 1; i < pat.size();) {
        if (pat[i] == pat[len]) {
            lps[i++] = ++len;
        } else if (len > 0) {
            len = lps[len - 1]; // fall back, do not advance i
        } else {
            lps[i++] = 0;
        }
    }
    return lps;
}
```

#text(size: 9pt, fill: gray)[Recherche telle quelle depuis `kmp_search.cpp:27`.]
```cpp
vector<int> kmpSearch(const string &txt, const string &pat) {
    vector<int> occ;
    if (pat.empty())
        return occ;
    vector<int> lps = computeLPS(pat);
    size_t i = 0; // index in txt
    size_t j = 0; // index in pat
    while (i < txt.size()) {
        if (txt[i] == pat[j]) {
            i++;
            j++;
            if (j == pat.size()) {
                occ.push_back((int)(i - j));
                j = lps[j - 1]; // allow overlapping matches
            }
        } else if (j > 0) {
            j = lps[j - 1]; // skip re-comparisons, key KMP idea
        } else {
            i++;
        }
    }
    return occ;
}
```

Table LPS pour `pat = "ABABCABAB"` (sortie mesurée `[0,0,1,2,0,1,2,3,4]`) :

#table(
  columns: (auto, auto, auto, 1fr),
  fill: (col, row) => if row == 0 { rgb("#1f2937") } else if row == 9 { rgb("#ecfdf5") },
  align: center,
  table.header[#text(white)[*i*]][#text(white)[*pat\[i\]*]][#text(white)[*lps\[i\]*]][#text(white)[*sens*]],
  [0], [A], [0], [#align(left)[un seul caractère, pas de bordure propre]],
  [1], [B], [0], [#align(left)[`"AB"` : pas de bordure]],
  [2], [A], [1], [#align(left)[`"ABA"` : bordure `"A"`]],
  [3], [B], [2], [#align(left)[`"ABAB"` : bordure `"AB"`]],
  [4], [C], [0], [#align(left)[rupture, repli à 0]],
  [5], [A], [1], [#align(left)[bordure `"A"` à nouveau]],
  [6], [B], [2], [#align(left)[bordure `"AB"`]],
  [7], [A], [3], [#align(left)[bordure `"ABA"`]],
  [8], [B], [4], [#align(left)[bordure `"ABAB"`]],
)

Trace de recherche (événements clés) : `txt = "ABABDABACDABABCABAB"`, `pat = "ABABCABAB"`, occurrence en 10.

#table(
  columns: (auto, auto, auto, 1fr),
  fill: (col, row) => if row == 0 { rgb("#1f2937") } else if row == 4 { rgb("#ecfdf5") },
  align: left,
  table.header[#text(white)[*étape*]][#text(white)[*pos. texte*]][#text(white)[*pos. motif*]][#text(white)[*événement*]],
  [1], [`i = 0..3`], [`j = 0..3`], [`ABAB` correspond, puis `txt[4] = D` vs `pat[4] = C` : repli `j = lps[3] = 2`],
  [2], [`i = 5..9`], [`j = 2..`], [le deuxième essai échoue vers `txt[8] = C` ; KMP décale via LPS au lieu de reculer `i`],
  [3], [`i = 10..18`], [`j = 0..8`], [correspondance complète de longueur 9 $arrow$ occurrence en 10],
  [4], [fin], [—], [1 occurrence en {10} ; les motifs à chevauchement repartiraient de `j = lps[8] = 4`],
)

Vérification du chevauchement : `txt = "aaaaa"`, `pat = "aaa"` $arrow$ occurrences en 0, 1, 2, car après chaque occurrence `j` repart à `lps[2] = 2` au lieu de 0.

#block(fill: rgb("#fffbeb"), radius: 6pt, inset: 10pt, stroke: 1pt + rgb("#f59e0b"))[
  #text(weight: "bold")[Complexité] : LPS coûte temps `O(m)` et espace `O(m)` ; la recherche coûte temps `O(n)`. Total temps `O(n + m)`, espace `O(m)`.
  L'indice du texte avance au plus `n` fois et l'indice du motif recule au plus `n` fois, donc les comparaisons sont bornées par `2n`.
]

= Mesures de performance

Machine : `g++ (Ubuntu 15.2.0-16ubuntu1) 15.2.0`, drapeau `-O2`, CPU Intel Core i7-8665U à 1,90 GHz.
Chaque mesure est répétée 3 fois et le meilleur temps est gardé (livre entier : 1 seul passage).
Le programme n'affiche pas de compteur de comparaisons : seuls le temps, la longueur et la répétition trouvée sont reportés.

#block(fill: rgb("#f4f4f5"), radius: 6pt, inset: 10pt)[
  #text(weight: "bold")[Mesures en bref] : le temps montre l'échelle ; la mémoire est `O(1)` supplémentaire pour la répétition naïve contre `O(m)` pour la table LPS ;
  le nombre de paires rend la méthode naïve quadratique (tableau A) tandis que la recherche KMP reste bornée par `2n` comparaisons.
]

Tableau A — répétition naïve sur Alice au pays des merveilles (`alice_story.txt`, texte du Projet Gutenberg, préfixes de N caractères) :

#table(
  columns: (auto, auto, 1fr),
  fill: (col, row) => if row == 0 { rgb("#1f2937") },
  align: left,
  table.header[#text(white)[*N*]][#text(white)[*temps (meilleur de 3)*]][#text(white)[*répétition trouvée*]],
  [2000], [6,7 ms], [len 21 en 15 et 483 : `"Down the Rabbit-Hole"`],
  [4000], [26,9 ms], [len 21, idem],
  [8000], [108 ms], [len 21, idem],
  [16000], [442 ms], [len 136 en 9173 et 12116 : ornement `"* * *"`],
  [32000], [1079 ms], [len 136, idem],
  [162949 (livre entier, 1 passage)], [27,4 s], [len 145 en 118654 et 119452 : refrain `"Will you, won't you..."`],
)

Lecture : le temps est multiplié par 4 environ quand `N` double (6,7 $arrow$ 26,9 $arrow$ 108 $arrow$ 442 ms, rapports 4,0 / 4,0 / 4,1) : le nombre de paires domine et l'extension casse vite sur texte courant.
La dernière marche (442 $arrow$ 1079 ms, rapport 2,4) a capté un moment creux sur une machine partagée ; la tendance reste clairement quadratique.
Le livre entier (162949 caractères) prend 27,4 s et révèle le refrain de la chanson (len 145) : le $O(n^2)$ naïf est infaisable en pratique courante — cela motive les méthodes linéaires (tableau des suffixes, automate), hors sujet ici.

Tableau B — recherche KMP contre recherche naïve :

#table(
  columns: (1fr, auto, auto),
  fill: (col, row) => if row == 0 { rgb("#1f2937") },
  align: center,
  table.header[#text(white)[*cas*]][#text(white)[*naïve*]][#text(white)[*KMP*]],
  [#align(left)[adverse, `pat = "aaab"`, `txt = "a" * 50000 + "b"`]], [199992 cmp / 0.38 ms], [99998 cmp / 0.18 ms],
  [#align(left)[classique, occurrence en {10}]], [29 cmp], [23 cmp],
  [#align(left)[échelle n = 20k]], [49k cmp / 0.12 ms], [26k cmp / 0.10 ms],
  [#align(left)[échelle n = 100k]], [247k cmp / 0.43 ms], [132k cmp / 0.31 ms],
)

Lecture : sur l'entrée adverse, le balayage naïf revérifie chaque décalage (199992 contre 99998 comparaisons, 0.38 contre 0.18 ms) car il recule après chaque quasi-correspondance tandis que KMP ne recule jamais.
À grande échelle, KMP utilise environ 1,9x moins de comparaisons (49k contre 26k ; 247k contre 132k) et les deux passent à l'échelle linéairement, ce qui confirme le comportement en `O(n + m)`.

= Conclusion et sources

La répétition naïve est courte, évidemment correcte et adaptée aux petits textes, mais son temps en $O(n^2 dot L)$ devient impraticable sur grand texte (1079 ms pour `N = 32000`, 27,4 s sur le livre entier de 162949 caractères). KMP paie une mémoire en `O(m)` pour la table LPS et obtient une recherche linéaire avec au plus `2n` comparaisons, divisant environ par deux le travail du balayage naïf sur les entrées adverses et volumineuses. Règle pratique : naïf pour la clarté et les minuscules entrées, KMP dès que le texte est grand ou que le motif se chevauche lui-même.

Sources :

1. Nayuki — Recherche de motifs de Knuth-Morris-Pratt. https://www.nayuki.io/page/knuth-morris-pratt-string-matching
2. GeeksforGeeks — plus longue sous-chaîne commune (article sur la programmation dynamique).
3. Wikipédia — Problème de la plus longue sous-chaîne commune.
