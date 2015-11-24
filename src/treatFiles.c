#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include"treatFiles.h"

/**
 * Fonction fournie, manière sécurisée d'ouvrir un fichier.
 */
void fileOpen(FILE **f, char *name) {
   *f = fopen(name, "r");
	if (f==NULL) {fprintf(stderr, "!!!!! Error opening %s !!!!! \n", name); exit(EXIT_FAILURE);}
}

/**
 * Fonction qui récupère le nombre de lignes d'un fichier. 
 * Une ligne se termine par le caractère '\n'.
 *
 * @param f : pointeur d'un fichier déjà ouvert. Le fichier reste ouvert.
 * @return : nombre de lignes du fichier.
 */
int getLengthFromFile(FILE *f) {
    int n = 0;
    char buffer;

    buffer = fgetc(f);
    while(buffer!=EOF)
    {
        if(buffer=='\n')
        {
            //on a trouvé un saut de ligne
            n++;
        }
        //on avance dans le fichier
        buffer = fgetc(f);
    }
    return n;
}

/**
 * Fonction calculant le nombre de caractères par lignes et le tableau 
 * des offsets.
 *
 * @param f : pointeur d'un fichier déjà ouvert. Le fichier reste ouvert.
 * @param n : longueur des tableaux lengthLine et offLine
 * @param lengthLine : tableau des longueurs des lignes du fichier. 
 * Après l'appel à la fonction, on a lengthLine[i] = |F(i)|
 * @param offLine : tableau des offsets du fichier.
 * Après l'appel à la fonction, on a offLine[i] = F(i) commence avec le
 *  i-ème caractère.
 */
void auxStruct(FILE *f, size_t n, int lengthLine[n], int offLine[n]) {
    char buffer;
    int offset = 0;
    int line = 0;
    offLine[0] = 0;

    //initialisation des variables
    buffer = fgetc(f);
    offset++;
    //on lit jusqu'à la fin du fichier
    while(buffer!=EOF) {
        //on lit jusqu'à la fin de la ligne
        if(buffer=='\n') {
            //mise à jour des tableaux
            lengthLine[line] = (line==0 ? offset : offset - offLine[line]);
            offLine[++line] = offset;
        }
        buffer = fgetc(f);
        offset++;
    }
}

/**
 * Fonction remplissant la matrice des coûts.
 *
 * @param n, m : tailles des tableaux d'entrées et de la matrice c[n+1][m+1]
 */
void computeCostsEff(size_t n, size_t m, int **c, FILE *f1, FILE *f2, 
        int lengthLineF1[n], int lengthLineF2[m], int offLineF1[n], int offLineF2[m]) {
    for(int i=0; i<n; i++) {
        for(int j=0; j<m; j++) {
            //on vérifie s'il s'agit d'une substitution triviale
            if(lengthLineF1[i]==lengthLineF2[j]) {
                //on charge les deux lignes
                char f1_i[lengthLineF1[i]];
                fseek(f1, offLineF1[i], SEEK_SET);
                fgets(f1_i, lengthLineF1[i], f1);
                char f2_j[lengthLineF2[j]];
                fseek(f2, offLineF2[j], SEEK_SET);
                fgets(f2_j, lengthLineF2[j], f2);

                if(strcmp(f1_i, f2_j)==0) {
                    c[i][j] = 0;
                } else {
                    c[i][j] = 10 + lengthLineF2[j];
                }
            } else {
                c[i][j] = 10 + lengthLineF2[j];
            }
        }
    }
}

void treatFiles(FILE *f1, FILE *f2, char *name1, char *name2, size_t n, size_t m, 
        int **c, int offLineF2[m], int lengthLineF2[m]) {

    //il faut parcourir les deux fichiers une deuxième fois
    //pour récupérer toutes les lignes des deux fichiers
    //réinitialisation des fichiers
    fseek(f1, 0, SEEK_SET);
    fseek(f2, 0, SEEK_SET);

    int lengthLineF1[n];
    int offLineF1[n];
    auxStruct(f1, n, lengthLineF1, offLineF1);
    auxStruct(f2, m, lengthLineF2, offLineF2);

   computeCostsEff(n, m, c, f1, f2, lengthLineF1, lengthLineF2,
            offLineF1, offLineF2);
//    findPatch(n, m, c, lengthLineF2, offLineF2, f2);
}
