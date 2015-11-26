#define min(a,b) \
       ({ __typeof__ (a) _a = (a); \
               __typeof__ (b) _b = (b); \
             _a < _b ? _a : _b; })
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

/**
 * La structure Coord nous sert à gérer la matrice des prédecesseurs
 * pour trouver le bon patch.
 */
struct Coord {
    int k; //la ligne
    int l; //la colonne
};

/**
 * Fonction calculant V(k, l), le noeud du graphe des dépendances. Il s'agit
 * de l'étape principale de l'équation de Bellman.
 *
 * @param tl0k_1 : la valeur V(k-1, l-1), substitution
 * @param tl0k : la valeur V(k, l-1), ajout
 * @param k, l : les coordonnées actuelles dans le graphe des dépendances, i.e.
 * l'étape courante.
 * @param n : le nombre de lignes du fichier F1
 * @param tl : le tableau contenant les valeurs V(*, l), de longueur n+1.
 * @param pred : la matrice des prédecesseurs, de taille [n+1][m+1]
 * @param *min_dest : la valeur minimale parmi les V(*, l).
 * @param *min_dest_k : le k tel que V(k, l) est minimale parmi les V(*, l)
 */
int minInst(int tl0k_1, int tl0k, int k, size_t n, int tl[n+1], int l, 
        struct Coord **pred, int *min_dest, int *min_dest_k) {
    //on compare parmi l'ajout et la substitution
    int actual_min = min(tl0k_1, tl0k);

    pred[k][l].l = l-1;
    if(actual_min == tl0k_1) {//il s'agit d'une suppression
        pred[k][l].k = k-1;
    } else { //il s'agit d'un ajout
        pred[k][l].k = k;
    }
    //ou bien il s'agit d'une destruction multiple
    if (actual_min > *min_dest + 15) {
        pred[k][l].k = *min_dest_k;
        pred[k][l].l = l;
        actual_min = *min_dest + 15;
    }
    if (actual_min < *min_dest) {
        *min_dest = actual_min;
        *min_dest_k = k;
    }
    //ou bien d'une destruction simple
    if (actual_min > tl[k-1] + 10) {
        pred[k][l].k = k-1;
        pred[k][l].l = l;
        actual_min = tl[k-1] + 10;
    }
    if (actual_min < *min_dest) {
        *min_dest = actual_min;
        *min_dest_k = k-1;
    }
    return actual_min;
}

/**
 * Fonction affichant le chemin trouvé par la méthode des prédecesseurs en
 * le transformant en un vrai patch, i.e. en utilisant les notations "+ k", etc.
 *
 * @param n, m : tailles des fichiers F1, F2.
 * @param start_succ : l'indice de départ pour trouver le chemin dans le 
 * tableau succ.
 * @param succ : le tableau contenant le chemin du patch.
 * @param c : la matrice des coûts de taille [n][m]. Nécessaire pour 
 * regarder s'il s'agit d'une substitution triviale.
 * @param lengthLineF2 : tableau de taille [m] tel que lengthLineF2[i]=|F2(i)|
 * @param offLineF2 : tableau de taille [m] tel que offLineF2[i] contient le
 * offset de F2(i)
 * @param f2 : pointeur du fichier F2, avant: ouvert, après: ouvert.
 */
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
        if (l_succ == (l+1)) {
            //il s'agit soit d'un ajout soit d'une substitution
            //on charge la ligne correspondante
            char f2_j[lengthLineF2[l_succ-1]];  
            fseek(f2, offLineF2[l_succ-1], SEEK_SET);
            fgets(f2_j, lengthLineF2[l_succ-1], f2);

            if (k_succ == k) {
                //il s'agit d'un ajout
                printf("+ %i\n", f1_count);
                printf("%s\n", f2_j);
                f2_count++;
            } else {
                //il s'agit d'une substitution
                if(c[f1_count][f2_count] != 0) {
                    //substitution non-triviale
                    printf("= %i\n", k_succ);
                    printf("%s\n", f2_j);
                }
                f1_count++;
                f2_count++;
            }
        }
        
        if(l_succ == l) {
            //il s'agit d'une destruction
            if(k_succ == (k+1)) {
                //destruction simple
                printf("d %i\n", f1_count+1);
                f1_count++;
            } else {
                printf("D %i %i\n", f1_count+1, k_succ-k);
                f1_count += k_succ -k;
            }
        }
        //mise à jour des variables de parcours
        k = k_succ;
        l = l_succ;
    }
}

/**
 * Fonction parcourant la matrice des prédecesseurs en prenant le chemin des
 * prédecesseurs pour trouver le bon patch.
 *
 * @param n, m : tailles des fichiers F1, F2
 * @param pred : matrice des prédecesseurs de taille [n+1][m+1]
 * @param c : la matrice des coûts de taille [n][m]
 * @param lengthLineF2 : tableau de taille [m] tel que lengthLineF2[i]=|F2(i)|
 * @param offLineF2 : tableau de taille [m] tel que offLineF2[i] contient le
 * offset de F2(i)
 * @param f2 : pointeur du fichier F2, avant: ouvert, après: ouvert.
 */
void findPath(size_t n, size_t m, struct Coord **pred, int **c, 
        int lengthLineF2[m], int offLineF2[m], FILE *f2) {
    //pred_tmp sert juste à stocker temporairement les coordonnées 
    //du parcours
    struct Coord pred_tmp;
    //on commence avec le point en bas à droite, la dernière cellule
    //de la matrice
    pred_tmp.k = pred[n][m].k;
    pred_tmp.l = pred[n][m].l;
    int k, l;
    //on va stocker les successeurs dans un tableau.
    //dans un cas très général, le chemin maximal est de longueur  
    //n+m+2, en n'effectuant jamais de trajet "diagonal" (ie substitution)
    struct Coord succ[n+m+2];
    int start_succ = n+m+1;

    k = n;
    l = m;
    //tant qu'on n'est pas arrivé
    while((pred_tmp.k!=0) || (pred_tmp.l!=0)) {
        //on met le chemin pris dans le tableau
        succ[start_succ].k = k;
        succ[start_succ--].l = l;
        //mise à jour des coordonnées du parcours
        k = pred_tmp.k;
        l = pred_tmp.l;
        pred_tmp.k = pred[k][l].k;
        pred_tmp.l = pred[k][l].l;
    }
    //on rajoute encore les deux derniers points qu'on n'a pas encore 
    //sauvegardé à cause du critère d'arrêt
    succ[start_succ].k = k;
    succ[start_succ--].l = l;
    succ[start_succ].k = pred_tmp.k;
    succ[start_succ].l = pred_tmp.l;
    //on passe à l'affichage du patch
    evaluateSucc(n, m, start_succ, succ, c, lengthLineF2, offLineF2, f2);
}

/**
 * Fonction de départ du calcul du patch optimal. Assemblage des autres 
 * fonctions de ce fichier.
 *
 * @param n, m : tailles des fichiers F1, F2
 * @param c : la matrice des coûts de taille [n][m]
 * @param lengthLineF2 : tableau de taille [m] tel que lengthLineF2[i]=|F2(i)|
 * @param offLineF2 : tableau de taille [m] tel que offLineF2[i] contient le
 * offset de F2(i)
 * @param f2 : pointeur du fichier F2. Avant: ouvert. Après: ouvert.
 */
int findPatch(size_t n, size_t m, int **c,  int lengthLineF2[m], 
        int offLineF2[m], FILE *f2) {
    int result = 0;
    //on a opté pour un parcours itératif, on a donc besoin de se rappeler de
    //deux colonnes.
    int tl0[n+1];
    int tl[n+1];
    //pour pouvoir repérer le chemin, on va utiliser une matrice de 
    //prédecesseurs
    struct Coord **pred;
    pred = malloc((n+1)*sizeof(struct Coord*));
    for(int i=0; i<(n+1); i++) {
        pred[i] = malloc((m+1)*sizeof(struct Coord));
    }

    //CI
    tl0[0] = 0;
    pred[0][0].k = 0;
    pred[0][0].l = 0;

    if (n != 0) {//il faut traiter le cas où le fichier F1 est vide
        tl0[1] = 10;
        pred[1][0].k = 0;
        pred[1][0].l = 0;
    }

    for(int k=2; k<=n; k++) {
        tl0[k] = 15;
        pred[k][0].k = 0;
        pred[k][0].l = 0;
    }
    
    for(int l=1; l<=m; l++) {
        tl[0] = tl0[0] + 10 + lengthLineF2[0]; //dans l2, on commence avec la 1ère ligne
        pred[0][l].k = 0;
        pred[0][l].l = l-1;
        //pour ne pas devoir chercher le min parmi les multidestructions,
        //on le sauvegarde pour une ligne 
        int min_dest = tl[0];
        int min_dest_k = 0;
        for(int k=1; k<=n; k++) {
            //c'est ici qu'on implémente l'équation de bellman
            tl[k] = minInst(tl0[k-1] + c[k-1][l-1], tl0[k] + 10 + lengthLineF2[l-1], 
                    k, n, tl, l, pred, &min_dest, &min_dest_k);
        }
        //recopie
        for(int p = 0; p < (n+1); p++) {
            tl0[p] = tl[p];
        }
    }
    if(m  == 0) {
        result = tl0[n];
    } else {
        result = tl[n];
    }
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
    return result;
}
