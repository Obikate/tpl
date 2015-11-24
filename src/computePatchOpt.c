#include<stdint.h>
#include<stdio.h>
#include<stdlib.h>
#include"treatFiles.h"
#include"findPatch.h"

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
    //calcul du patch optimal et affichage sur la sortie standard
    findPatch(n, m, c, lengthLineF2, offLineF2, f2);

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
