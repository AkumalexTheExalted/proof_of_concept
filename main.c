#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define MAX_TEMP    25
#define MIN_TEMP    15
#define MAX_POURCENTAGE 100
#define MIN_POURCENTAGE 0

#define MAX_AMOUNT  250
#define MIN_AMOUNT  10

#define ILLICITE    0
#define REUSSITE    1
#define ECHEC       -1
#define NC          -100


#define PROP1 "fenetre ferme"
#define PROP2 "chauffage augmente"
#define PROP3 "chauffage baisse"


typedef struct capteur_vocal{
    char* commande;
    bool auth;
} capteur_vocal;

typedef struct commande{
    float temperature;
    int pourcentage;
    char* destinataire;
    int montant;
} commande;

bool is_correct_temp(float temp){
    if(temp >= MIN_TEMP && temp <= MAX_TEMP){
        //printf("OK TEMPERATURE\n");
        return true;
    }
    return false;
}

bool is_correct_pourc(int pourc){
    if(pourc >= MIN_POURCENTAGE && pourc <= MAX_POURCENTAGE){
        //printf("OK POURCENTAGE\n");
        return true;
    }
    return false;
}

bool is_correct_amount(int pourc){
    if(pourc >= MIN_AMOUNT && pourc <= MAX_AMOUNT){
        //printf("OK MONTANT\n");
        return true;
    }
    return false;
}

bool is_correct_cb(int cb){
    if(cb == ECHEC || cb == ILLICITE || cb == REUSSITE || cb == NC){
        return true;
    }
    return false;
}


int count_words(char *instruction){
    int count = 1;
    for (int i = 0; instruction[i] != '\0'; i++) {
        if (instruction[i] == ' ' && instruction[i+1] != ' ')
            count++;
    }
    return count;
}

bool is_numeric_value(char* val){
    for(int i = 0 ; val[i] != '\0' ; i++){
        if(val[i] < '0' || val[i] > '9'){
            return false;
        }
    }
    return true;
}

char** parser(int taille, char *commande){
    char **token = malloc(taille *sizeof(char*));
    int i = 0;
    char* copie = malloc(strlen(commande) + 1);
    if(copie == NULL){
        exit(-1);
    }
    strcpy(copie, commande);
    char *split = strtok(copie, " ");
    while(split){
        token[i] = split;
        //printf("token : %s\n", token[i]);
        split = strtok(NULL, " ");
        i++;
    }
    return token;
}

/*[fenetre ouvre %X%]
 * [fenetre ferme]
 * [chauffage aumgente]
 * [chauffage baisse]
 * [paiement destinataire %montant]*/
bool is_correct_capteur(capteur_vocal cdv){
    int number_of_word = count_words(cdv.commande);
    //printf("number of word : %d\n", number_of_word);
    if(number_of_word >= 2 || number_of_word <= 3){
        if(number_of_word == 2){
            if (cdv.commande == PROP1 || cdv.commande == PROP2 || cdv.commande == PROP3){
                return true;
            }
        }else{
            char **token = parser(3, cdv.commande);
            if (strncmp(token[0], "fenetre", 7) == 0 && strncmp(token[1], "ouvre", 5) == 0){
                if(is_numeric_value(token[2])){
                    int numeric_value = atoi(token[2]);
                    if(is_correct_pourc(numeric_value)){
                        return true;
                    }else{
                        return false;
                    }
                }
                return false;
            }
            if (strncmp(token[0], "paiement", 8) == 0){
                if(is_numeric_value(token[2])){
                    int numeric_value = atoi(token[2]);
                    if(is_correct_amount(numeric_value)){
                        return true;
                    }else{
                        return false;
                    }
                }
                return false;
            }
        }
    }
    return false;
}

void affichage_cb(int curr_cb){
    if(is_correct_cb(curr_cb)){
        switch (curr_cb) {
            case ECHEC:
                printf("operation bancaire précédente ratee\n");
                break;
            case ILLICITE:
                printf("opération bancaire precedente illicite\n");
                break;
            case REUSSITE:
                printf("operation bancaire precedente reussie\n");
                break;
            default:
                printf("aucune operation bancaire precedemment\n");
                break;
        }
    }else{
        printf("retour inconnu\n");
    }
}

commande controleur(capteur_vocal cdv, float curr_temp, int curr_pourc, int curr_cb){
    //phase de sécurisation des entrées
    // 15 <= curr_temp <= 25 && 0 < curr_pourc <= 100 && curr_cb = [-1, 0, 1, 2]
    affichage_cb(curr_cb);
    commande com;
    //état de base
    com.temperature = 20;
    com.pourcentage = 0;
    com.destinataire = NULL;
    com.montant = 0;
    //printf("avant correct temperature : %f\n", com.temperature);
    if (is_correct_temp(curr_temp)== true && is_correct_pourc(curr_pourc) == true && is_correct_capteur(cdv) == true){
        com.temperature = curr_temp;
        com.pourcentage = curr_pourc;
        com.destinataire = NULL;
        com.montant = 0;
        //printf("correct\n");
        char **commande = parser(count_words(cdv.commande), cdv.commande);
        if(strncmp(commande[0], "fenetre", 7) == 0){
            //printf("FENETRE\n");
            if (strncmp(commande[1], "ferme", 7) == 0) {
                com.pourcentage = 0;
            }else{
                com.pourcentage = atoi(commande[2]);
            }
        }else if(strncmp(commande[0], "chauffage", 9) == 0){
            //printf("CHAUFFAGE\n");
            if(strncmp(commande[1], "augmente", 8) == 0 && (curr_temp + 2 <= MAX_TEMP)){
                com.temperature = curr_temp + 2;
            }else if (curr_temp - 2 >= MIN_TEMP){
                com.temperature = curr_temp - 2;
            }
        }else{
            //printf("PAIEMENT\n");
            com.destinataire = commande[1];
            com.montant = atoi(commande[2]);
        }
    }
    //printf("apres traitement : %f\n", com.temperature);
    return com;
}


int main() {/*
    capteur_vocal test;
    test.commande[0]= FENETRE;
    test.commande[1] = OUVRIR;
    test.commande[2] = NC;
    //controleur(test, 0, 0, 0);


    is_correct_capteur(test);*/

    capteur_vocal cdv;
    cdv.commande = "paiement Diegu_enterprise 56";

    //printf("valeur : %d\n", is_correct_capteur(cdv));
    //char** test = parser(3,cdv.commande);

    commande ret = controleur(cdv, 20, 0, ECHEC);

    //printf("%s\n", test[1]);
    printf("valeur temperature : %f\n", ret.temperature);
    printf("valeur pourcentage : %d\n", ret.pourcentage);
    printf("valeur destinataire : %s\n", ret.destinataire);
    printf("valeur montant : %d\n", ret.montant);

    return 0;
}
