# Outil-de-compression-decompression
L'objectif principal de ce mini-projet est de concevoir un outil de compression qui réduit de manière significative la taille d'un fichier texte. Pour ce faire, nous allons utiliser l’algorithme de Huffman.

Pour tester le programme vous pouvez essayer de compresser le fichier **hamlet.txt**

Ne pas hésiter à effectuer un chmod des fichiers en cas de problème de compilation.

Les fichiers avec accents et autres caractères spéciaux entraineront des problèmes de compression,
Hamlet ne comportant pas d'accents, seul le cas du retour chariot et du retour à la ligne a été considéré (contenu dans hamlet), de ce fait la compression est tout à fait compatible avec le fichier hamlet.txt.

Le fichier **compression.c** réalisera la compression et décompression du fichier cible
