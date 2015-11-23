#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int max_length = 100;

void fileOpen(FILE **f, char *name) {
   *f = fopen(name, "r");
	if (f==NULL) {fprintf(stderr, "!!!!! Error opening %s !!!!! \n", name); exit(EXIT_FAILURE);}
}

int getLengthFromFile(FILE *f) {
    int n = 0;
    char buffer[max_length];

    while(fgets(buffer, max_length, f)!=NULL)
    {
        n++;
    }
    return n;
}

void outputIntMatrix(size_t n, size_t m, int **c) {
    printf("Sortie de la matrice\n");
    for(int i=0; i<n; i++) {
        for(int j=0; j<m; j++) {
            printf(" %i ", c[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

//fonction calculant le nombre de caractères par lignes et le tableau des offsets
void auxStruct(FILE *f, size_t n, int lengthLine[n], int offLine[n]) {
    char buffer;
    int offset = 0;
    int line = 0;
    offLine[0] = 0;
    buffer = fgetc(f);
    offset++;
    //on lit jusqu'à la fin du fichier
    while(buffer!=EOF) {
        //on lit jusqu'à la fin de la ligne
        if(buffer=='\n') {
            lengthLine[line] = (line==0 ? offset : offset - offLine[line]);
            offLine[++line] = offset;
        }
        buffer = fgetc(f);
        offset++;
    }
}

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

void treatFiles(FILE *f1, FILE *f2, char *name1, char *name2) {

    //on récupère les longueurs des fichiers
    int n = getLengthFromFile(f1);
    int m = getLengthFromFile(f2);
    printf("longueur du premier fichier: %i\n", n);
    printf("longueur du deuxième fichier: %i\n", m);

    //il faut parcourir les deux fichiers une deuxième fois
    //pour récupérer toutes les lignes des deux fichiers
    //réinitialisation des fichiers
    fseek(f1, 0, SEEK_SET);
    fseek(f2, 0, SEEK_SET);

    int lengthLineF1[n];
    int offLineF1[n];
    auxStruct(f1, n, lengthLineF1, offLineF1);
    int lengthLineF2[m];
    int offLineF2[m];
    auxStruct(f2, m, lengthLineF2, offLineF2);
    for(int j=0; j<m; j++) {
        printf("%i\n", lengthLineF2[j]);
    }

    //calcul de la matrice des coûts
    int **c;
    if((c = malloc(n*sizeof(int*)))!=NULL) {
        for(int i=0; i<n; i++) {
            c[i] = malloc(m*sizeof(int));
        }
    }
    computeCostsEff(n, m, c, f1, f2, lengthLineF1, lengthLineF2,
            offLineF1, offLineF2);
//    outputIntMatrix(n, m, c);
    //désallocation de la mémoire
    if(c!=NULL) {
        for(int i=0; i<n; i++) {
            if(c[i]!=NULL) {
                free(c[i]);
            }
        }
    free(c);
    }
}

int main (int argc, char *argv[]) {
	FILE *f1;
	FILE *f2;
	
	if(argc<3){
		fprintf(stderr, "!!!!! Usage: ./treatFiles firstFile secondFile !!!!!\n");
	    exit(EXIT_FAILURE); /* indicate failure.*/
	}

    fileOpen(&f1, argv[1]);
    fileOpen(&f2, argv[2]);

    treatFiles(f1, f2, argv[1], argv[2]);

    if(f1!=NULL) {
        fclose(f1);
    }
    if(f2!=NULL) {
        fclose(f2);
    }

    return 0;
}
