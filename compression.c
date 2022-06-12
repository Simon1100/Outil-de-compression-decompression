#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

//On définit un type permettant la représentation d'un arbre de huffman composé d'une probabilité 
//et d'une chaine de caratère
struct noeud {
    char chaine[257]; //avec \0 à la fin 
    double proba;
    struct noeud* sag;
    struct noeud* sad;
};
typedef struct noeud *Arbre;

//On définit un type permettant la représentation d'une probabilité proba d'apparition d'un caractère carac
typedef struct {
    char carac;
    double proba;
} ProbabiliteCarac;

//On définit un type permettant de chainer par l'intermédiaire d'une liste d'Arbre (les noeuds de l'Arbre de huffman)
//La liste va réduire en nombre d'arbre au cours de la création de l'abre de Huffman jusqu'à ne contenir qu'un seul arbre
//:il sera notre abre de Huffman
struct celluleArbre {
    Arbre a;
    struct celluleArbre* suivant;
};
typedef struct celluleArbre *ListeArbre;

//On définit un type permettant de contenir pour chaque caractère du fichier qui sera lu,
//sa représentation par sa taille en bits (nombre de bits), et sa valeur. Utile à la compressions et décompression
//Par exemple pour a : il pourra être représenté par 010 et sa taille sera de 3
typedef struct {
    unsigned char carac;
    unsigned char val;
    int taille;
} code;

//fonction pour transformer un char en char* : copié de https://www.developpez.net/forums/d701164/c-cpp/c/convertir-char-char/
char *char2string(char c)
{
   char *str=(char*)malloc(2*sizeof(char));
   if(str==NULL)
     return NULL;
   str[0]=c;
   str[1]='\0';
   return str;
}

//Trie par insertion par ordre croissante d'un tableau de ProbabiliteCarac par rapport à sa valeur double
void triparinsertion(ProbabiliteCarac tab[256], int Nbcarac){
    double cle;
    char carac;
    int j;
    for(int i = 1; i<Nbcarac; i++){
        cle = tab[i].proba;
        carac = tab[i].carac;
        j = i - 1;
        while(j>=0 && tab[j].proba>cle){
            tab[j+1].proba=tab[j].proba;
            tab[j+1].carac=tab[j].carac;
            j--;
        }
        tab[j+1].proba=cle;
        tab[j+1].carac=carac;
    }
}

//Fonction permettant de transformer un tableau de ProbabiliteCarac en une liste d'arbre
//Pour ce faire chaque cellule du tableau devient un arbre et chaque arbre est ajouté dans la liste
ListeArbre transformationTableauProbabiliteCaracEnListeArbre(ListeArbre lstArbre, ProbabiliteCarac tabproba[], int Nbcellule){
    ListeArbre save = lstArbre;
    for (int i = 0; i < Nbcellule; i++) //Pour toutes les cellules du tableau faire
    {
        Arbre tmp = (Arbre) calloc(1, sizeof(struct noeud));
        strcpy(tmp->chaine,char2string(tabproba[i].carac)); //On place le caractère dit en tant que chaine de caractère de l'arbre
        tmp->proba=tabproba[i].proba;
        tmp->sad = NULL;
        tmp->sag = NULL;
        if(lstArbre->a==NULL){
            lstArbre->a = tmp;
        }
        else{
            lstArbre->suivant = (ListeArbre) calloc(1, sizeof(struct celluleArbre));
            lstArbre = lstArbre->suivant;
            lstArbre->a = tmp;
        }
    }
    return save;
}

//Fonction permettant d'insérer en tête de liste un Arbre 
ListeArbre insererentete(Arbre a, ListeArbre lst) {
    ListeArbre L = (ListeArbre) calloc(1, sizeof(struct celluleArbre));
    L->a = a;
    L->suivant = lst;
    return L;
}

//Fonction permettant d'insérer au milieu d'une liste un Arbre 
ListeArbre insereraumilieu(Arbre a, ListeArbre lst1, ListeArbre lst2) {
    ListeArbre L = (ListeArbre) calloc(1, sizeof(struct celluleArbre));
    L->a = a;
    L->suivant = lst2;
    lst1->suivant=L;
    return lst1;
}

// //Fonction permettant d'insérer en tête d'une liste un Arbre 
// ListeArbre insererenqueue(Arbre a, ListeArbre lst) {
//     ListeArbre L = lst;
//     while(lst->suivant!=NULL){
//         lst = lst->suivant;
//     }
//     lst->suivant = (ListeArbre) calloc(1, sizeof(struct celluleArbre));
//     lst = lst->suivant;
//     lst->a = a;
//     lst->suivant = NULL;
//     return L;
// }

//Fonction permettant de trier une liste d'arbre
ListeArbre trieliste(ListeArbre a){
    //On définit la nouvelle liste
    ListeArbre b = (ListeArbre) calloc(1,sizeof(struct celluleArbre));
    b->a = a->a; //on insére le premier élément
    b->suivant=NULL;

    //Tant que nous ne somme pas à l'avant dernier arbre de la liste alors
    while(a->suivant->suivant!=NULL){
        int flag=0;
        a = a->suivant;
        ListeArbre tmp = b;
        //On parcourt à chaque itération de la boucle toute la liste b (liste que nous sommes en train de construire)
        //jusqu'à trouver un arbre avec une probabilité plus grande que l'arbre actuel, afin d'inserer cet arbre dans l'ordre croissant de la probabilté des arbres
        while(tmp->suivant!=NULL){
            if(a->a->proba>tmp->a->proba){
                if(a->a->proba<=tmp->suivant->a->proba){
                    tmp = insereraumilieu(a->a,tmp,tmp->suivant);
                    flag=1;
                    break;
                }
                tmp = tmp->suivant; //On continue tant que nous ne trouvons pas une probabilité supérieur à celle de l'arbre courant
            }
            else{
               break; 
            }
        }
        //Si on a parcouru toute la liste sans trouver d'arbre avec une probabilité plus grande que celle actuelle, ou bien si on effectue le premier placement dans la liste d'arbre
        if(!flag){
            if(tmp->a->proba>a->a->proba){
                tmp = insererentete(a->a,tmp);
            }
            else{
                tmp = insereraumilieu(a->a,tmp,tmp->suivant); //on insere en queue
            }
        }
    }
    //On traite de la même façon le dernier arbre de la liste
    a = a->suivant;
    ListeArbre tmp = b;
    int flag=0;
    while(tmp->suivant!=NULL){
        int flag=0;
        if(a->a->proba>tmp->a->proba){
            if(a->a->proba<=tmp->suivant->a->proba){
                tmp = insereraumilieu(a->a,tmp,tmp->suivant);
                flag=1;
                break;
            }
            tmp = tmp->suivant;
        }
        else {
           break; 
        }
    }
    if(!flag){
        if(tmp->a->proba>a->a->proba){
            tmp = insererentete(a->a,tmp);
            return tmp;
        }
        else{
            tmp = insereraumilieu(a->a,tmp,tmp->suivant); //on insere en queue
            return b;
        }
    }
    return b; //On retourne la liste d'arbre triés
}


// créer une fonction permettant de retourner l’arbre de Huffman qui représente le codage
// qui sera utilisé pour la compression du fichier en question.

//On transforme notre tableau de probablité d'apparition par caractère en arbre de huffman
Arbre arbredehuffman(ProbabiliteCarac tabproba[],int NbcaracUtilise){

    //On définit une liste d'arbre, permettant la construction d'un arbre de huffman
    ListeArbre lstArbre = (ListeArbre) calloc(1, sizeof(struct celluleArbre));
    //On définit une liste d'arbre permettant de sauvegarder la tête de la liste lstArbre
    ListeArbre save = lstArbre;
    lstArbre->a = NULL;

    //on transforme chaque cellule du tableau de ProbabiliteCarac en abre que l'on insére dans une liste d'abre
    lstArbre = transformationTableauProbabiliteCaracEnListeArbre(lstArbre, tabproba, NbcaracUtilise);

    //On construit dans cette boucle petit à petit notre arbre de huffman
    while(lstArbre->suivant!=NULL){
        //On rassemble dans cette boucle les différents arbres entre-eux
        while(lstArbre->suivant!=NULL){
            //Si la probabilité du prochain arbre est plus grande que l'arbre actuel alors on continue d'assembler les abres
            if(lstArbre->a->proba<=lstArbre->suivant->a->proba){
                Arbre tmp = (Arbre) calloc(1, sizeof(struct noeud));
                char chaine[257]="";
                //On assemble les deux chaines des deux arbres
                strcat(chaine, lstArbre->a->chaine);
                strcpy(tmp->chaine,strcat(chaine,lstArbre->suivant->a->chaine));
                //On assemble leurs probabilités
                tmp->proba=lstArbre->a->proba+lstArbre->suivant->a->proba;

                //on assemble les sous arbres dans en tant que noeud du nouvel arbre
                tmp->sag=lstArbre->a;
                tmp->sad=lstArbre->suivant->a;

                //On avance dans la liste
                lstArbre->a = tmp;

                //Si on a fini le parcours de la liste d'abre
                if(lstArbre->suivant->suivant==NULL){
                    lstArbre->a = tmp;
                    lstArbre->suivant = NULL;
                    break;
                }
                //sinon
                else{
                    lstArbre->suivant = lstArbre->suivant->suivant;
                    lstArbre = lstArbre->suivant;
                }
            }
            //Sinon on stop
            else{
                break;
            }
        }
        lstArbre = save;

        //Tous les arbres on était rassemblé on a donc notre arbre de huffman sous la main, on peut stoper la boucle 
        if(lstArbre->suivant==NULL){
            break;
        }
        //on trie en l'etat actuel la liste d'arbre afin de recommencer le rassemblement des arbres
        if(lstArbre != NULL){
            save = trieliste(lstArbre);
        }

        lstArbre = save;
    }

    //on recupere l'arbre :
    Arbre Huffman = lstArbre->a;
    //et le retourne
    return Huffman;
}


//Fonction permettant de parcourir notre arbre de huffman dans le but de recuperer la taille et la valeur en bit
//de chaque caractère contenue dans notre fichier dans le but d'effectuer une compression et décompression de notre fichier
//a : Arbre de huffman, c : pour contenir les informations sur chaque caractère, tailletab : nombre maximum de caractere dans le fichier soit 256
//val : valeur actuel -> 0 premier sous abre à gauche et 1 premier sous abre à droite, nb : hauteur de l'abre -> soit le nombre de bits qui composeront la valeur.
void parcoursArbre(Arbre a, code c[], int tailletab, int val, int nb){
    if(a!=NULL){
        if(strlen(a->chaine)==1){ //Si on trouve un caractère isolé (c'est à dire non composé de deux caractères) alors on lui indique sa valeur, et la taille de la représentation en bit de sa valeur
            c[(int)a->chaine[0]].carac=a->chaine[0];
            c[(int)a->chaine[0]].val=val;
            c[(int)a->chaine[0]].taille=nb;
        }
        else{
            //on effectue un parcours recursive de l'arbre de huffman pour ce faire
            nb = nb + 1;
            parcoursArbre(a->sag, c, tailletab, val*2, nb);
            if(val==0){
                val=1;
            }
            else{
                val=val*2+1;
            }
            parcoursArbre(a->sad, c, tailletab, val, nb);
        }
    }
}

// en paramètre : le fichier à compresser (.txt) et le fichier où stocker le résultat de la 
// compression (par ex fichiers .huff).
//c : pour contenir les informations sur chaque caractère, tailletab : nombre maximum de caractere dans le fichier soit 256

int compression(char source[255], char destination[255], code c[], int tailletab){
    FILE *TEXTES = fopen(source, "r");
    if(TEXTES == NULL){
        return 1;
    }
    //Si la destination n'existe pas elle  sera crée dans le fichier source du code
    FILE *TEXTED = fopen(destination, "wb");

    //On effectue une première lecture du fichier source, afin de connaitre précisément le nombre de bits qui devront
    //être lu et interprété au cours de la décompression
    long long tailleEnBitDestination = 0;
    char a;
    while ((a = fgetc(TEXTES)) != EOF)
    {
       tailleEnBitDestination+=c[(int)a].taille;
    }

    //On replace le pointeur de lecture en début de fichier
    rewind(TEXTES);
    //Pour chaque caractère on prend l'information le concernant, sa taille en nombre de bits, sa valeur.
    //Ces informations ont étés obtenu au préalable grâce à l'arbre de huffman.
    //On place ensuite ces informations en signature du fichier de décompression.
    for(int i=0;i<tailletab;i++){
        if(c[i].taille!=0){
            //On prend cas à cas le cas du retour chariot et du retour à la ligne
            if(i==10){
                //On ecrit "SL" dans la signature quand il y a présence d'un ou de plusieurs \n
                char chaine[256]="";
                strcat(chaine,"SL");
                strcat(chaine,"/|/");
                char chaine2[256]="";
                //On écrit la taille que prend le bit dans la compression
                //(information utile lors de la phase de décompression)
                sprintf(chaine2,"%d",c[i].taille);
                strcat(chaine,chaine2);
                strcat(chaine,"/|/");
                //On fait de même avec sa valeur 
                sprintf(chaine2,"%d",c[i].val);
                strcat(chaine,chaine2);
                strcat(chaine,"\n");
                fputs(chaine,TEXTED);
            }
            else if(i==13){
                //On ecrit "CR" dans la signature quand il y a présence d'un ou de plusieurs \r
                char chaine[256]="";
                strcat(chaine,"CR");
                strcat(chaine,"/|/");
                char chaine2[256]="";
                //On écrit la taille que prend le bit dans la compression
                //(information utile lors de la phase de décompression)
                sprintf(chaine2,"%d",c[i].taille);
                strcat(chaine,chaine2);
                strcat(chaine,"/|/");
                //On fait de même avec sa valeur
                sprintf(chaine2,"%d",c[i].val);
                strcat(chaine,chaine2);
                strcat(chaine,"\n");
                fputs(chaine,TEXTED);
            }
            else{
                //On ecrit le caractère dit dans la signature quand il y a présence d'un ou de plusieurs de celui-ci
                char chaine[256]="";
                strcat(chaine,char2string(c[i].carac));
                strcat(chaine,"/|/");
                char chaine2[256]="";
                //On écrit la taille que prend le bit dans la compression
                //(information utile lors de la phase de décompression)
                sprintf(chaine2,"%d",c[i].taille);
                strcat(chaine,chaine2);
                strcat(chaine,"/|/");
                //On fait de même avec sa valeur
                sprintf(chaine2,"%d",c[i].val);
                strcat(chaine,chaine2);
                strcat(chaine,"\n");
                fputs(chaine,TEXTED);
            }
        }
    }
    //On précise qu'on est en fin de signature
    char chaine[256]="";char chaine2[256]="";
    fputs("finsignature\n",TEXTED);
    //Et indique le nombre de bit qui devra être lu à l'étape de la décompression, afin de rétablir le fichier d'origine
    strcpy(chaine,"finfichier/|/");
    sprintf(chaine2,"%lld",tailleEnBitDestination);
    strcat(chaine,chaine2);
    strcat(chaine,"\n");
    fputs(chaine,TEXTED);

    //On commence ici la compression
    int i = 0;
    char temp = 0;
    //Tant qu'il n'y a plus de caractère à lire dans le fichier à compresser faire
    while ((a = fgetc(TEXTES)) != EOF)
    {
        //On cherche le nombre de bits à récuper
        i=i+c[(int)a].taille;
        //Tant que nous n'avons pas 8 bits nous continuons à stoker cette suite de bits dans une variable temp
        if(i<=8){
            temp = temp | (c[(int)a].val<<(8-i));
        }
        //Quand nous avons plus(+) ou 8 bits nous pouvons écrire le premier caractère dans le fichier compressé, et stocker
        //le surplus de bits dans une variable temp
        if(i>=8){
            temp = temp | ( c[(int)a].val >> ( i - 8 ));
            fputs(char2string(temp),TEXTED);
            i = i - 8;
            temp = 0;
            temp = temp | ( c[(int)a].val << ( 8 - i ));
        }
    }
    if(i!=0){ //On complétera la fin de fichier par des 0 au dernier octet (si celui-ci n'est pas composé de 8 bits)
        temp = temp | 0;
        fputs(char2string(temp),TEXTED);
    }
    fclose(TEXTED);
    fclose(TEXTES);
    printf("\nLa compression du fichier est terminée");
    return 0;
}



// en c : -2 : 110 et non 00000110. -> on transforme -2 en 00000110
//Si on ne le fait pas des calculs peuvent être faussé lors de décallage
//Notre but est de récupérer des bits et non pas leurs valeurs correspondantes
//On indique en paramètre l'octet sur lequel on va travailler (8 derniers bits de TMP)
//Et le nombre de bits qui nous intéresse dans cet octet
int entierSigneVersNonsigne(int TMP, int besoin){
    if(TMP<0){
        TMP = ~TMP;
        TMP = 0 | TMP;
        if(besoin == 1){
            TMP = TMP ^ 1;
        }
        if(besoin == 2){
            TMP = TMP ^ 3;
        }
        if(besoin == 3){
            TMP = TMP ^ 7;
        }
        if(besoin == 4){
            TMP = TMP ^ 15;
        }
        if(besoin == 5){
            TMP = TMP ^ 31;
        }
        if(besoin == 6){
            TMP = TMP ^ 63;
        }
        if(besoin == 7){
            TMP = TMP ^ 127;
        }
    }
    return TMP;
}


//Fonction qui permet la décompression, prend en paramètre la source du fichier compressé avec sa
// signature et la destination du fichier qui contiendra le résultat de la décompression
int decompression(char source[255], char destination[255]){
    //On ouvre les deux fichiers
    FILE *TEXTES = fopen(source, "r");
    if(TEXTES == NULL){
        return 1;
    }
    FILE *TEXTED = fopen(destination, "wb");
    //On initialise un tableau de code : permettant de contenir pour chaque caractere
    //du fichier qui sera lu, sa représentation par sa taille en bits (nombre de bits), et sa valeur.
    code c[256];
    for(int i=0;i<256;i++){
        c[i].carac=0;
        c[i].val=0;
        c[i].taille=0;
    }

    //Lecture de la signature du fichier à décompresser

    char chaine[257];
    int cpt=0;

    //Tant qu'on ne lit pas "finsignature" dans le fichier faire
    while (1)
    {
        //On lit ligne à ligne la signature
        fgets(chaine,sizeof(chaine),TEXTES);
        //On quitte la boucle si on lit "finsignature"
        if(strcmp(chaine,"finsignature\n")==0){
            break;
        }
        char tmp;
        char *token;
        int i =0;
        //On sépare les éléments de la signature grâce à leur séparateur "/|/" (contenue dans la signature)
        //(écrit à l'étape de compression)
        token = strtok(chaine,"/|/");
        while (token != NULL) {
            if (i==0){
                //On associe dans la signature les caractères à ce qu'ils sont "SL" = \n par exemple
                if(strcmp(token,"SL")==0) tmp='\n';
                else if(strcmp(token,"CR")==0) tmp='\r';
                else tmp=token[0];
                c[(int)tmp].carac = tmp;
            }
            //On associe chaque caractère à sa taille de représentatione en bit
            if (i==1) c[(int)tmp].taille=atoi(token);
            //De même pour sa valeur    
            if (i==2) c[(int)tmp].val=atoi(token);
            i++;
            token = strtok (NULL, "/|/");
        }
    }
    //On lit le nombre de bits que l'on doit prendre en considération dans le fichier de décompression
    //Utile pour ne pas ajouter de faux caractère en fin de fichier (utile dans la situation de fin de fichier)
    fgets(chaine,sizeof(chaine),TEXTES);
    char *token = strtok(chaine,"/|/");
    if(strcmp(token,"finfichier")==0)
        token = strtok (NULL, "/|/");
    long long finFichier = atoll(token);
    while(token != NULL){
        token = strtok (NULL, "/|/");
    }


    int a;
    //La variable act contiendra les 8 prochains bits à analyser dans la décompression
    char act = 0;
    //La variable tmp contiendra les prochains bits [1 à 8 bits] à insérer dans la variable act,
    char tmp = 0;
    //Cette variable permet d'obtenir l'information sur le nombre de bits utile dans tmp
    //Si 5 bits sont utiles dans tmp ils seront placés du côté du bit de poid fort
    //les 3 autres bits considéré comme inutile seront complété par des 0 du côté du bit de poid faible
    int tailletmp = 0;
    //La variable besoin, indiquera le nombre de bits à ajouter dans act.
    //C'est à dire que si on "décompresse" le caractère 'a' et qu'il est représenté par 3 bits
    //on aura de ce fait besoin de 3 bits supplémentaire dans la varible act, que l'on prendra dans la varibale tmp. (Si tmp < besoin on réallouera des bits à tmp pendant la procédure)
    int besoin=0;
    //flag permet de stopper la boucle qui permet de trouver la bonne coresspondance de bit à caractère si la boucle trouve une correspondance
    int flag=0;

    //Tant que la boucle n'est pas stoppé du fait d'un manque de caractère à lire dans le fichier compressé faire
    while(finFichier>0){
        //Si tmp est vide alors on lui réalloue 8 nouveaux bits
        if(tailletmp==0){
            a = fgetc(TEXTES);
            tmp = a;
            tailletmp = 8; //on indique que tmp est composé de 8 bits utilisable
            //Arret de la boucle remplacé à l'aide de la variable finFichier
            if(a==EOF){ //S'il n'y a plus de caractère à lire dans le fichier alors on sort de la boucle permettant la décompression
               // break;
            }
        }
        //Si on doit ajouter des bits à la variable act alors
        if(besoin!=0){
            //Si tmp contient moins de bits que ce que act à besoin alors
            if(besoin>tailletmp){
                //on récupère les derniers bits contenue dans tmp qu'on stocke dans TMP
                int TMP = tmp >> (8 - tailletmp);
                //Si TMP commence par 1 alors on le transforme
                TMP = entierSigneVersNonsigne(TMP,tailletmp);
                //On décalle du nombre de bits  nécessaire pour ajouter les bits de TMP2 (déclaré plus bas) à la suite de TMP
                TMP = TMP << (besoin - tailletmp);

                //On ajoute 8 nouveaux bits à tmp puisqu'il est vide
                a = fgetc(TEXTES);
                tmp = a;
                //Arret de la boucle remplacé à l'aide de la variable finFichier
                if(a==EOF){ //S'il n'y a plus de caractère à lire dans le fichier alors on sort de la boucle permettant la décompression
                   // break;
                }

                //on récupère dans le nouveau tmp les bits manquant
                int TMP2;
                TMP2 = tmp >> (8 - (besoin - tailletmp)); //tailletmp correspond à l'ancienne taille de tmp avant l'entree dans ce if

                //Si TMP2 commence par 1 alors on le transforme
                TMP2 = entierSigneVersNonsigne(TMP2, besoin - tailletmp);
                TMP = TMP | TMP2;
                //On obtient la nouvelle valeur de act          
                act = act | TMP;
                //On obtient la nouvelle valeur de tmp
                tmp = tmp << (besoin - tailletmp);
                tailletmp = 8 - (besoin - tailletmp);
                besoin = 0;
            }
            //Si tmp contient les bits nécessaire à ajouter à act
            else{
                //On prend dans tmp les bits à ajouter dans act
                int TMP = tmp >> (8 - besoin);
                //Si TMP commence par 1 alors on le transforme
                if(TMP<0){
                    TMP = entierSigneVersNonsigne(TMP, besoin);
                    act = act | TMP;
                }
                else{
                    act = act | TMP;
                }
                //on enlève les bits pris par act à tmp
                tmp = tmp << besoin;
                tailletmp = tailletmp - besoin;
                besoin = 0;
            }
        }
        //Si act est vide et tmp est composé de 8 bits
        else{
            act = tmp;
            tailletmp=0;
        }
        //Ici on cherche si on peut trouver dans act une correspondance avec un caractère connu
        int memoireact = act;
        for(int i=7;i>=0;i--){
            flag=0;
            act = memoireact >> i; //Exemple a = 1001 0010 avec i=7 on récupère 1
            //On transforme act si il commence par 1
            act = entierSigneVersNonsigne(act,8 - i);
            int taille = 8 - i;
            //on cherche ici une correpondance avec un carctère connu, grace aux tailles et valeur de bits connu par caractère
            for(int j=0; j<256;j++){
                if (c[j].val==act && c[j].taille==taille){
                    fputc(c[j].carac,TEXTED);
                    flag  = 1; //on a trouvé une correspondance
                    act = memoireact << taille; //On récupère la partie de l'octet non utilisé
                    besoin = taille; //j'ai besoin de i nouveau bits dans act;
                    break;
                }
            }
            if(flag){
                //On décrémente la varirable qui nous indiquera si nous sommes en fin de fichier,
                //si finFichier = 0 nous sommes en fin de fichier et la boucle se stoppera
                finFichier-=taille;
                break;
            }
        }
    }
    printf("\nLa décompression du fichier est terminée");
    return 0;
}


int main(int argc, char *argv[]){
    FILE *Livre = NULL;
    // Tableau correspondant aux occurrences de chaque caractère indicé par le caractère dit
    int Nbcaractere[256] = {0};
    // Nombre de caractères total dans le fichier
    int Nbcarac = 0;
    // Variable correspondant aux nombres de caractère utilisé (diffèrent) dans le fichier
    int NbcaracUtilise=0;
    // Tableau de ProbabiliteCarac contenant par indice : le caractère dit en char et sa probabilité en double
    ProbabiliteCarac tabproba[256];
    //On déclare notre futur arbre de huffman
    Arbre Huffman;

    // Nous récupèrons ici l'argument de la ligne de commande (soit le fichie texte à ouvrir)
    char source[255] = ""; char destination[255] = "";
    if(argc<=2){
        printf("Deux paramètres sont attendues : \n\t'-c' pour compresser le fichier passé en 2ème paramètre ou bien '-d' pour le décompresser \n\t ./compression -c simon.txt ou /compression -d simon.txt sont acceptées\n");
        return 1;
    }
    else{
        if(strcmp(argv[1],"-d")!=0 && strcmp(argv[1],"-c")!=0){
            printf("Deux paramètres sont attendues : \n\t'-c' pour compresser le fichier passé en 2ème paramètre ou bien '-d' pour le décompresser \n\t ./compression -c simon.txt ou /compression -d simon.txt sont acceptées\n");
            return 1;
        }
        else{
            stpcpy(source,argv[2]);
            Livre = fopen(source, "rb");
        }
    }
    if (Livre == NULL)
    {
        if(strcmp(argv[1],"-c")==0){
            strcpy(source,"hamlet.txt");
        }
        else{
            strcpy(source,"hamlet.txt.huff");
        }
        if(argc>2){
            printf("Votre fichier n'a pas pu être ouvert, par défaut le traitement sera donc effectué sur %s\n",source);
        }
        Livre = fopen(source, "r"); // fichier par défaut
        if(Livre == NULL){
            printf("erreur lors de l'ouverture du fichier par défaut\n");
            return 1;
        }
    }
    if(strcmp(argv[1],"-c")==0){

        //Pour chaque caractère, nous cherchons son nombre d'occurrences dans le livre, ainsi que le nombre de caractère au total dans le livre
        char a;
        while ((a = fgetc(Livre)) != EOF)
        {
            Nbcaractere[a]++;
            Nbcarac++;
        }

        //On place dans le tableau tabproba, la probabilité d'apparition de chaque caractère dit, et ce caractère dans un char
        for (int i = 0; i < 256; i++)
        {
            tabproba[i].carac= (char) i;
            tabproba[i].proba = (double) Nbcaractere[i] / Nbcarac;
        }

        //On rassemble ici tous les caractères utilisés c'est à dire :
        //Si les indices 1 2 3 4 du tableau tabproba ont zéro occurence mais qu'à l'indice 0 et 5 ils en ont
        //alors on passe l'indice 5 en 1
        //On rassemble donc les caractères utilisés côte à côte dans le tableau tabproba.
        //(C'est la raison pour laquelle nous avons placé le caractère dans une structure et sa probabilité,
        //au lieu d'indicer le tableau par la valeur entière du caractère)
        for (int i = 0; i < 256; i++)
        {
            if(tabproba[i].proba !=0){
            tabproba[NbcaracUtilise].proba=tabproba[i].proba;
            tabproba[NbcaracUtilise].carac=(char) i;
            NbcaracUtilise++; //On obtiendra grâce à cette incrémentation l'information sur le nombre de caractère différent dans le fichier
            }
        }

        //On trie le tableau de probabilité de manière croissante
        triparinsertion(tabproba,NbcaracUtilise);

        //On transforme se tableau de probabilite en arbre de huffman
        Huffman = arbredehuffman(tabproba,NbcaracUtilise);
    }

    //On initialise un tableau de code : permettant de contenir pour chaque caractere
    //du fichier qui sera lu, sa représentation par sa taille en bits (nombre de bits), et sa valeur.
    // Utile à la compression et décompression


    code c[256];
    for (int i = 0; i < 256; i++)
    {
        c[i].taille=0;
        c[i].val=0;
        c[i].carac=0;
    }
    //Compression
    if(strcmp(argv[1],"-c")==0){
        //On parcourt notre arbre de huffman dans le but de recuperer la taille et la valeur en bit
        //de chaque caractère de notre fichier.
        // c : pour contenir les informations sur chaque caractère, tailletab (256) : nombre maximum de caractere dans le fichier
        //Les deux autres paramètres seront expliqué en haut de la fonction en commentaire
        parcoursArbre(Huffman, c, 256, 0, 0);

        //On donne le nom à notre fichier de destination.
        strcpy(destination,source);
        strcat(destination,".huff");

        // en paramètre : le fichier à compresser (.txt) et le fichier où stocker le résultat de la compression (par ex fichiers .huff).
        // c : pour contenir les informations sur chaque caractère, tailletab (256) : nombre maximum de caractere dans le fichier
        int erreur = compression(source, destination, c, 256);
        if(erreur){
            printf("La compression n'a pas pu avoir lieu, une erreur est survenue");
            return 1;
        }
    }
    //Décompression
    else{
        //On enleve le ".huff" au nom du fichier qui va etre généré
        char source1[256]="";
        strcpy(source1,source); //on garde la source telquel
        char*token = strtok(source,".");
        strcat(destination,&token[0]);
        while (token != NULL) {
            token = strtok (NULL, ".");
            if(strcmp(&token[0],"huff")!=0 && token!=NULL){
                strcat(destination,".");
                strcat(destination,&token[0]);
            }
            else{
                break;
            }
        }

        printf("\nLa décompression est en cours");
        int erreur = decompression(source1,destination);
        if(erreur){
            printf("\nLa décompression n'a pas pu avoir lieu, une erreur est survenue");
            return 1;
        }
    }
    fclose(Livre);
    return 0;
}
