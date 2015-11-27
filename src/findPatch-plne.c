#include<stdio.h>
#include<stdlib.h>
#include<string.h>

void nextLine(FILE *data) {
    char b = fgetc(data);
    while(b != '\n') {
        b = fgetc(data);
    }
}

void outTab(int n, int *tab) {
    int i=0;
    for(i=0; i<n; i++) {
        printf("%i\n", tab[i]);
    }
}

int encode(int i) {
    if (i==48) {
        return 0;
    } else {
        return 1;
    }
}

void getData(FILE *data, int *n, int *m, int **d, int **a) {
    size_t instLength = 13;
    char buffer[instLength];
    char bufferLength[100];

    //lecture des longueurs des fichiers
    fgets(bufferLength, 100, data);
    sscanf(bufferLength, "%i %i", n, m);
//    printf("%i %i\n", *n, *m);
    *a = malloc(*m*sizeof(int));
    *d = malloc(*n*sizeof(int));
    //lecture du header du fichier
    while((buffer[0] != 'a') && (buffer[1] != '[') && (buffer[2] != '1')) {
        fgets(buffer, instLength, data);
        nextLine(data);
    }
//    printf("tableau a:\n");
    //lecture du tableau a
    int j;
    for(j=0; j< *m; j++) {
        (*a)[j] = encode(buffer[instLength-2]);
        fgets(buffer, instLength, data);
        nextLine(data);
    }
//    outTab(*m, *a);
    fgets(buffer, instLength, data);
    nextLine(data);
    //on saute le tableau s
    fseek(data, 15*(*n)*(*m)*sizeof(char), SEEK_CUR);
    nextLine(data);
    fgets(buffer, instLength, data);
    //lecture du tableau a
//    printf("tableau d:\n");
    int i;
    for(i=0; i< *n; i++) {
        (*d)[i] = encode(buffer[instLength-2]);
        fgets(buffer, instLength, data);
        nextLine(data);
    }
//    outTab(*n, *d);
    //lecture du coût 
    fgets(bufferLength, 100, data);
    char *tmp1=NULL, *tmp2=NULL;
    int cost;
    sscanf(bufferLength, "%s %s %i", tmp1, tmp2, &cost);
}

char * getLineF(int *nbLine, FILE *f, int getLine) {
    char *buffer = NULL;
    size_t length;
    while(*nbLine < getLine-1) {
        nextLine(f);
        (*nbLine)++;
    }
    if (*nbLine == getLine-1) {
        nextLine(f);
    }
    getline(&buffer, &length, f);
    (*nbLine)++;
    return buffer;
}

void findPatch(int n, int m, int *d, int *a, FILE *f1, FILE *f2) {
    int f1_c = 1;
    int f2_c = 1;
    int nbLine1 = 1;
    int nbLine2 = 1;
    char *f1_i=NULL, *f2_j=NULL;

    while((f1_c <= n) || (f2_c <= m)) {
        if (d[f1_c - 1] == 1) {
            printf("d %i\n", f1_c++);
        } else if (a[f2_c-1] == 1) {
            printf("+ %i\n", f1_c-1);
            printf("%s", getLineF(&nbLine2, f2, f2_c));
            f2_c++;
        } else {
            if (nbLine1 == n-1) {
                char * buffer;
                size_t length;
                getline(&buffer, &length, f1);
                f1_i = buffer;
            } else {
                f1_i = getLineF(&nbLine1, f1, f1_c);
            }
            
            f2_j = getLineF(&nbLine2, f2, f2_c);
 
            if (strcmp(f1_i, f2_j) != 0) {
                printf("= %i\n", f1_c);
                printf("%s", f2_j);
            }
            f1_c++;
            f2_c++;
        }
//        printf("f1_c: %i, nbLine1: %i, f2_c: %i, nbLine2: %i\n", f1_c, nbLine1, f2_c, nbLine2);
    }
}

int main(int argc, char *argv[]) {
    FILE *data;
    data = fopen("data", "r");
    int n,m;
    int *d, *a;

    //on recupère les tableaux d'ajouts et de destructions
    getData(data, &n, &m, &d, &a);
    //écriture du patch sur la sortie standard
    FILE *f1 = fopen(argv[1], "r");
    FILE *f2 = fopen(argv[2], "r");
    findPatch(n, m, d, a, f1, f2);

    fclose(f2);
    fclose(data);
    free(a);
    free(d);
	 if (data==NULL) {fprintf(stderr, "!!!!! Error opening dataFile !!!!! \n"); exit(EXIT_FAILURE);}
    return 0;
}
