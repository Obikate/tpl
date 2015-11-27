#include<stdint.h>
#include<stdio.h>
#include<stdlib.h>
#include"treatFiles.h"
#include"findPatch.h"

/**
 * Fonction écrivant les données pour la résolution du PLNE.
 */
void writeDonnes(int n, int m, int **c, int *lengthLineF2) {
    FILE *out = fopen("donnees.data", "w");
    if (out == NULL) {
          fprintf(stderr, "Can't open output file %s!\n",
                            "donnees.data");
            exit(1);
    }
    //on écrit le header
    fprintf(out, "data;\n\n");
    fprintf(out, "param n := %i;\n", n); 
    fprintf(out, "param m := %i;\n", m); 
    //on écrit le tableau des longueurs
    fprintf(out, "param L : "); 
    for(int i=1; i<=m; i++) {
        fprintf(out, "%i ", i);
    }
    fprintf(out, ":=\n");
    fprintf(out, "       1 ");
    for(int i=0; i<m; i++) {
        fprintf(out, " %i", lengthLineF2[i]);
    }
    fprintf(out, ";\n\n");

    //écriture de la matrice c
    fprintf(out, "param c : "); 
    for(int i=1; i<=m; i++) {
        fprintf(out, "%i ", i);
    }
    fprintf(out, ":=\n");
    for(int i=0; i<n; i++) {
        fprintf(out, "       %i ", i+1);
        for(int j=0; j<m; j++) {
            fprintf(out, " %i",c [i][j]);
        }
        if(i == (n-1)) {
            fprintf(out, ";\n");
        } else {
            fprintf(out, "\n");
        }
    }
    fprintf(out, "end;\n");

    fclose(out);
}

int main(int argc, char *argv[]) {
	FILE *f1;
	FILE *f2;
	
	if(argc<3){
		fprintf(stderr, "!!!!! Usage: ./treatFiles firstFile secondFile !!!!!\n");
	    exit(EXIT_FAILURE); /* indicate failure.*/
	}

    fileOpen(&f1, argv[1]);
    fileOpen(&f2, argv[2]);

    //on récupère les longueurs des fichiers
    int n = getLengthFromFile(f1);
    int m = getLengthFromFile(f2);
    //écriture des longueurs dans le fichier data
    FILE *data = fopen("data", "w");
    fprintf(data, "%i %i\n", n, m);
    fclose(data);

    if ((n == 0) && (m == 0))
        return 0;

    //structures auxiliaires nécessaires concernant le fichier F2
    int lengthLineF2[m];
    int offLineF2[m];

    //allocation de la matrice des coûts
    //l'allocation dynamique est nécessaire, car en général, les données
    //sont trop grandes pour stocker une matrice
    int **c;
    if((c = malloc(n*sizeof(int*)))!=NULL) {
        for(int i=0; i<n; i++) {
            c[i] = malloc(m*sizeof(int));
        }
    }
 
    //on calcule la matrice des coûts, le tableau des offsets et le tableau
    //des longueurs des lignes
    treatFiles(f1, f2, argv[1], argv[2], n, m, c, offLineF2, lengthLineF2);
    writeDonnes(n, m, c, lengthLineF2);

    //désallocation de la mémoire
    if(c!=NULL) {
        for(int i=0; i<n; i++) {
            if(c[i]!=NULL) {
                free(c[i]);
            }
        }
    free(c);
    } 

    if(f1!=NULL) {
        fclose(f1);
    }
    if(f2!=NULL) {
        fclose(f2);
    }
    return 0;
}
