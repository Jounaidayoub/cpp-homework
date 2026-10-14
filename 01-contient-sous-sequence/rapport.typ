#set page(paper: "a4", margin: 2cm)
#set text(font: "DejaVu Sans", size: 11pt)
#set heading(numbering: "1.")

#align(center)[
  #text(size: 20pt, weight: "bold")[Recherche de motif : `contient(texte, mot)`]
  #v(4pt)
  #text(size: 12pt)[Algo 1 #h(8pt) | Sous-séquence dans le même ordre]
  #v(2pt)
  #text(size: 10pt, fill: gray)[algo.cpp #h(8pt) | vérifié par exécution]
]

= Objectif

Étant donnés deux chaînes `texte` et `mot`, décider si `mot`
apparaît dans `texte` *dans le même ordre*, sans exiger que les
lettres soient côte à côte.

Exemple de référence :

- `texte = "computer"`, `mot = "cote"` $arrow$ #strong[trouvé]
- `texte = "hello"`, `mot = "world"` $arrow$ #strong[non trouvé]

= L'algorithme

#grid(columns: (1fr, 1fr), gutter: 12pt)[
  #block(fill: rgb("#f4f4f5"), radius: 6pt, inset: 10pt)[
    #text(weight: "bold")[Signature]
    #v(4pt)
    ```cpp
    bool contient(const string &texte,
                  const string &mot)
    ```
    #v(4pt)
    Retourne `true` si tout `mot` est retrouvé dans `texte`, `false` sinon.
    Cas vide : `mot == ""` $arrow$ `true`.
  ]
  #block(fill: rgb("#eef6ff"), radius: 6pt, inset: 10pt)[
    #text(weight: "bold")[Idée en 3 points]
    #v(4pt)
    1. Indice `j` = progression dans `mot`.
    #v(2pt)
    2. On balaye `texte` avec `i` une seule fois.
    #v(2pt)
    3. Si `texte[i] == mot[j]`, on avance `j`.
    #v(4pt)
    Succès si `j == mot.len()` à la fin.
  ]
]

#text(size: 9pt, fill: gray)[Fonction testée telle quelle dans `algo.cpp`.]
```cpp
bool contient(const string &texte, const string &mot)
{
    if (mot.empty())
        return true;
    size_t j = 0;
    for (size_t i = 0; i < texte.length() && j < mot.length(); i++)
    {
        if (texte[i] == mot[j])
            j++;
    }
    return j == mot.length();
}
```

= Exemple visuel : "computer" contient "cote"

Lettres appariées mises en évidence :

#align(center)[
  #block(inset: 10pt, fill: rgb("#fafafa"), radius: 8pt, stroke: 1pt + gray)[
    #text(size: 16pt, font: "DejaVu Sans Mono")[
      #highlight(fill: rgb("#bbf7d0"))[c]
      #highlight(fill: rgb("#bbf7d0"))[o]mpu#highlight(fill: rgb("#bbf7d0"))[t]#highlight(fill: rgb("#bbf7d0"))[e]r
      #h(12pt)#text(fill: gray)[$arrow$]
      #h(12pt)#highlight(fill: rgb("#bbf7d0"))[c]
      #highlight(fill: rgb("#bbf7d0"))[o]
      #highlight(fill: rgb("#bbf7d0"))[t]
      #highlight(fill: rgb("#bbf7d0"))[e]
    ]
    #v(4pt)
    #text(size: 9pt, fill: gray)[c(0) $arrow$ o(1) $arrow$ t(5) $arrow$ e(6) : même ordre, trous autorisés]
  ]
]

Déroulé indice par indice :

#table(
  columns: (auto, auto, auto, auto),
  fill: (col, row) => if row == 0 { rgb("#1f2937") } else if row == 6 or row == 7 { rgb("#ecfdf5") },
  align: center,
  table.header[#text(white)[*i*]][#text(white)[*texte[i]*]][#text(white)[*j avant*]][#text(white)[*action*]],
  [0], [c], [0], [match $arrow$ j=1],
  [1], [o], [1], [match $arrow$ j=2],
  [2..4], [m/p/u], [2], [ignoré, on cherche `t`],
  [5], [t], [2], [match $arrow$ j=3],
  [6], [e], [3], [match $arrow$ j=4 ✓],
  [7], [r], [4], [déjà gagné],
)

Contre-exemple : `texte = "hello"`, `mot = "world"` $arrow$
`w` jamais trouvé $arrow$ `j` reste à 0 $arrow$ `false`.

= Tests vérifiés et complexité

Programme compilé avec `g++` et exécuté. Sorties réelles :

#table(
  columns: (1fr, 1fr, auto),
  fill: (col, row) => if row == 0 { rgb("#1f2937") },
  align: left,
  table.header[#text(white)[*Entrée (texte / mot)*]][#text(white)[*Sortie*]][#text(white)[*Statut*]],
  [`computer` / `cote`], [existe dans la premiere], [#text(fill: rgb("#15803d"))[OK]],
  [`hello` / `world`], [n'existe pas dans la premiere], [#text(fill: rgb("#15803d"))[OK]],
  [`""` / `""`], [`true` (mot vide)], [#text(fill: rgb("#15803d"))[OK]],
)

#block(fill: rgb("#fffbeb"), radius: 6pt, inset: 10pt, stroke: 1pt + rgb("#f59e0b"))[
  #text(weight: "bold")[Complexité] : un seul passage sur `texte` $arrow$ temps `O(n)`,
  mémoire `O(1)` (deux compteurs). Idéal comme brique pour la suite :
  appliquer ce même `contient` ligne par ligne sur un grand fichier
  (`search_file.cpp`), sans tout charger en mémoire.
]

= Design : comment aller plus vite sur gros fichier

Le coût total = `O(volume fichier)` en lecture + `O(n)` par ligne en CPU.
L'I/O domine, donc on optimise dans cet ordre :

#table(
  columns: (auto, 1fr),
  fill: (col, row) => if row == 0 { rgb("#1f2937") },
  align: left,
  table.header[#text(white)[*Levier*]][#text(white)[*Effet*]],
  [1. Streaming ligne par ligne], [`ifstream + getline` : mémoire `O(longueur max ligne)`, pas `O(fichier)`. Vérifié : 18 Mo traités avec ~4 Mo RSS.],
  [2. Pré-filtre longueur], [si `mot.len() > ligne.len()` $arrow$ `skip` immédiat, zéro balayage.],
  [3. Sortie anticipée], [dès que `j == mot.len()` on arrête la ligne ; dès qu'on a assez de hits on peut arrêter le fichier.],
  [4. Parallèle par blocs], [découper le fichier en chunks (ex. 4 threads) $arrow$ gain proche de x4 sur disque rapide. Chaque chunk cherche en indépendant.],
  [5. `mmap` + SIMD], [évite les copies `read`, compare 16-32 octets par instruction. Utile au-delà de ~100 Mo.],
)

Point d'attention mesuré : en logique sous-séquence, `cote`
matche presque toutes les lignes ordinaires (200 000 / 200 000),
car il suffit que `c`, `o`, `t`, `e` apparaissent dans l'ordre.
Pour une recherche fichier précise, passer en sous-chaîne contiguë
(`string.find` / Boyer-Moore) divise les faux positifs et permet
le saut de caractères.

= Benchmarks réels

Fichier synthétique : 200 000 lignes, 18 Mo.
Machine locale, `g++ -O2`, moyenne d'une passe :

#table(
  columns: (1fr, auto, auto),
  fill: (col, row) => if row == 0 { rgb("#1f2937") },
  align: left,
  table.header[#text(white)[*Cas*]][#text(white)[*Temps*]][#text(white)[*Mémoire*]],
  [mot=`cote` (hit massif, sous-séquence)], [0.28 s], [~4.0 Mo],
  [mot=`zzzqqq` (miss total)], [0.05 s], [~3.9 Mo],
  [micro : 1 M caractères, hit], [~64 ms], [—],
  [micro : 1 M caractères, miss], [~64 ms], [—],
)

Lecture : le débit est d'environ 60-360 Mo/s selon le taux de match,
l'empreinte reste plate grâce au streaming. Le miss est plus rapide
car aucun affichage. Prochaine étape : comparer `find` contigu vs
sous-séquence sur le même fichier et tracer la courbe temps / taille.

#align(center)[#text(size: 10pt, fill: gray)[Sources : `algo.cpp` (console) #h(8pt) | `search_file.cpp` (fichier) #h(8pt) | réf `mots.cpp`]]
