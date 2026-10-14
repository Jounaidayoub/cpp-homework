# 01 — `contient(texte, mot)` : sous-séquence + recherche fichier

## Énoncé

Décider si `mot` apparaît dans `texte` **dans le même ordre**,
sans exiger que les lettres soient côte à côte
(ex. `"computer"` contient `"cote"`),
puis appliquer la recherche ligne par ligne sur un grand fichier.

## Fichiers

| Fichier | Rôle |
|---|---|
| `algo.cpp` | `contient()` : un seul balayage de `texte`, `O(n)` temps, `O(1)` mémoire + démo console |
| `search_file.cpp` | `motExiste()` : recherche naïve contiguë par ligne + traitement streaming (`ifstream` + `getline`) |
| `large.txt` | Petit fichier d'exemple |
| `rapport.typ` / `rapport.pdf` | Rapport complet (algo, exemple visuel, complexité, design streaming, benchmarks) |

## Compiler et exécuter

```bash
g++ -O2 -Wall -o /tmp/algo algo.cpp && /tmp/algo
g++ -O2 -Wall -o /tmp/search search_file.cpp && /tmp/search large.txt cote
typst compile rapport.typ rapport.pdf
```

## Résultats (mesurés, voir rapport)

- 200 000 lignes / 18 Mo : ~0.28 s (hit massif), ~0.05 s (miss), ~4 Mo RSS grâce au streaming.
- Complexité : `O(volume fichier)` en lecture + `O(n)` par ligne en CPU.
