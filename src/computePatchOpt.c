#define min(a,b) \
       ({ __typeof__ (a) _a = (a); \
               __typeof__ (b) _b = (b); \
             _a < _b ? _a : _b; })
#include<stdlib.h>
#include<stdio.h>
#include<string.h>


void fileOpen(FILE **f, char *name) {
   *f = fopen(name, "r");
	if (f==NULL) {fprintf(stderr, "!!!!! Error opening %s !!!!! \n", name); exit(EXIT_FAILURE);}
}

struct Coord {
    int k; //la ligne
    int l; //la colonne
};

int minInst(int tl0k_1, int tl0k, int k, size_t n, int tl[n+1], int l, size_t m, struct Coord **pred) {
    int actual_min = min(tl0k_1, tl0k);

    pred[k][l].l = l-1;
    if(actual_min == tl0k_1) {//il s'agit d'une suppression
        pred[k][l].k = k-1;
    } else { //il s'agit d'un ajout
        pred[k][l].k = k;
    }
    for(int p=0; p<(k-1); p++) {
        int min_before = actual_min;
        actual_min = min(actual_min, 15 + tl[p]);
        if(actual_min < min_before) {
            //multi-destruction
            pred[k][l].k = p;
            pred[k][l].l = l;
        }
    }
    int min_before = actual_min;
    actual_min = min(actual_min, 10 + tl[k-1]);
    if(actual_min < min_before) {
        //multi-destruction
        pred[k][l].k = k-1;
        pred[k][l].l = l;
    }
    return actual_min;
}

void evaluateSucc(size_t n, size_t m, int start_succ, struct Coord succ[n+m+2], 
        int **c, int lengthLineF2[m], int offLineF2[m], FILE *f2) {
    //on a besoin d'un compteur pour les destructions et les substitutions
    int f1_count = 0;
    int f2_count = 0;
    int k = 0;
    int l = 0;
    int k_succ = 0;
    int l_succ = 0;
    for(int i=start_succ+1; i<(n+m+2); i++) {
        k_succ = succ[i].k;
        l_succ = succ[i].l;
        //on regarde de quelle opération il s'agit
        if(k_succ == k) {
            //il s'agit d'un ajout
//            printf("+ %i\n", k_succ);
            printf("+ %i\n", f1_count);
            //on charge la ligne correspondante
            char f2_j[lengthLineF2[l_succ-1]];  
            fseek(f2, offLineF2[l_succ-1], SEEK_SET);
            fgets(f2_j, lengthLineF2[l_succ-1], f2);
            printf("%s\n", f2_j);
            f2_count++;
        }
        if((k_succ == (k+1)) && (l_succ == (l+1))) {
            //il s'agit d'une substitution
            if(c[f1_count][f2_count] != 0) {
                printf("= %i\n", k_succ);
                //on charge la ligne correspondante
                char f2_j[lengthLineF2[l_succ-1]];  
                fseek(f2, offLineF2[l_succ-1], SEEK_SET);
                fgets(f2_j, lengthLineF2[l_succ-1], f2);
                printf("%s\n", f2_j);
                f1_count++;
                f2_count++;
            } else {
                f2_count++;
                f1_count++;
            }
        }
        if(l_succ == l) {
            //il s'agit d'une destruction
            if(k_succ == (k+1)) {
                //destruction simple
                printf("d %i\n", f1_count+1);
                f1_count++;
            } else {
//                printf("D %i %i\n", f1_count+1, k_succ-k);
                printf("D %i %i\n", f1_count+1, k_succ-k);
                f1_count += k_succ -k;
            }
        }
        k = k_succ;
        l = l_succ;
    }
}

void findPath(size_t n, size_t m, struct Coord **pred, int **c, 
        int lengthLineF2[m], int offLineF2[m], FILE *f2) {
    struct Coord pred_tmp;
    pred_tmp.k = pred[n][m].k;
    pred_tmp.l = pred[n][m].l;
    int k, l;
    //on va stocker les successeurs dans un tableau
    struct Coord succ[n+m+2];
    int start_succ = n+m+1;

    k = n;
    l = m;
    while((pred_tmp.k!=0) || (pred_tmp.l!=0)) {
        succ[start_succ].k = k;
        succ[start_succ--].l = l;
        k = pred_tmp.k;
        l = pred_tmp.l;
        pred_tmp.k = pred[k][l].k;
        pred_tmp.l = pred[k][l].l;
    }
    succ[start_succ].k = k;
    succ[start_succ--].l = l;
    succ[start_succ].k = pred_tmp.k;
    succ[start_succ].l = pred_tmp.l;
//    succ[start_succ--].l = pred_tmp.l;
//    succ[start_succ].k = 0;
//    succ[start_succ].l = 0;
    evaluateSucc(n, m, start_succ, succ, c, lengthLineF2, offLineF2, f2);
}

void outputCoordMatrix(size_t n, size_t m, struct Coord **pred) {
    for(int i=0; i<=n; i++) {
        for(int j=0; j<=m; j++) {
            printf("(%i, %i) ", pred[i][j].k, pred[i][j].l);
        }
        printf("\n");
    }
}


void findPatch(size_t n, size_t m, int **c,  int lengthLineF2[m], 
        int offLineF2[m], FILE *f2) {
    int tl0[n+1];
    int tl[n+1];
    //pour pouvoir repérer le chemin, on va utiliser une matriced de 
    //prédecesseurs
//    struct Coord pred[n+1][m+1];
    struct Coord **pred;
    pred = malloc((n+1)*sizeof(struct Coord*));
    for(int i=0; i<(n+1); i++) {
        pred[i] = malloc((m+1)*sizeof(struct Coord));
    }

    //CI
    tl0[0] = 0;
    pred[0][0].k = 0;
    pred[0][0].l = 0;

    tl0[1] = 10;
    pred[1][0].k = 0;
    pred[1][0].l = 0;

    for(int k=2; k<=n; k++) {
        tl0[k] = 15;
        pred[k][0].k = 0;
        pred[k][0].l = 0;
    }
    
    for(int l=1; l<=m; l++) {
        tl[0] = tl0[0] + 10 + lengthLineF2[0]; //dans l2, on commence avec la 1ère ligne
        pred[0][l].k = 0;
        pred[0][l].l = l-1;
        for(int k=1; k<=n; k++) {
            //c'est ici qu'on implémente l'équation de bellman
            tl[k] = minInst(tl0[k-1] + c[k-1][l-1], tl0[k] + 10 + lengthLineF2[l-1], k, n, tl, l, m, pred);
        }
        //recopie
        for(int q=0; q <(n+1); q++) {
            tl0[q] = tl[q];
        }
    }
//    outputCoordMatrix(n, m, pred);
    fprintf(stderr, "%i\n", tl[n]);
    findPath(n, m, pred, c, lengthLineF2, offLineF2, f2);
    //désallocation de la mémoire
    if(pred!=NULL) {
        for(int i=0; i<(n+1); i++) {
            if(pred[i]!=NULL) {
                free(pred[i]);
            }
        }
        free(pred);
    }
}

//////////////// Partie prétraitement des fichiers ///////

int max_length = 100;
int getLengthFromFile(FILE *f) {
    int n = 0;
//    char buffer[max_length];
    char buffer;

    buffer = fgetc(f);
//    while(fgets(buffer, max_length, f)!=NULL)
    while(buffer!=EOF)
    {
        if(buffer=='\n')
        {
            n++;
        }
        buffer = fgetc(f);
    }
    return n;
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

    treatFiles(f1, f2, argv[1], argv[2]);

    if(f1!=NULL) {
        fclose(f1);
    }
    if(f2!=NULL) {
        fclose(f2);
    }
    return 0;
}