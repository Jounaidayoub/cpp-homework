# Conception — arbre binaire de recherche

## Objectif

Créer une remise C++ consacrée à un arbre binaire de recherche (ABR)
implémenté manuellement, puis le comparer au conteneur standard `std::map` sur
les mêmes données et les mêmes opérations. Le protocole retenu est celui du
rapport de référence : 200 000 valeurs distinctes, mélange reproductible avec
`mt19937` et graine 2026, insertion, recherche et suppression, trois
répétitions et moyenne.

## Conception retenue

- `BinarySearchTree` possède ses nœuds, ses pointeurs gauche/droit, une
  insertion itérative, une recherche et une suppression itérative.
- Les doublons sont ignorés.
- `std::map<int, int>` reçoit exactement les mêmes clés et les mêmes opérations.
- Le temps mesure chaque opération séparément; génération et mélange sont hors
  chronométrage.
- Un compteur de contrôle confirme les 200 000 recherches et les 200 000
  suppressions réussies lors de chacune des trois répétitions.
- La hauteur finale de l'ABR est conservée comme indicateur de sa forme.

## Rapport

Le rapport en français présente les deux structures, les opérations
équivalentes, les tests fonctionnels, la méthode expérimentale, les moyennes
réelles, la complexité, les limites et la conclusion. Le PDF est compilé
depuis `report/report.typ` et les résultats sont conservés dans
`benchmark.csv`.

## Validation

- cas de recherche et d'insertion avec arbre vide, valeurs croissantes et
  doublons;
- suppressions d'une feuille, d'un nœud avec un fils et d'un nœud avec deux
  fils;
- compilation avec `g++ -O2 -Wall -Wextra -pedantic`;
- exécution des benchmarks ABR/`std::map` avec une graine annoncée;
- compilation du rapport avec Typst;
- vérification que les fichiers C++ ne contiennent pas de commentaires.
