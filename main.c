#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
//Pour exe: gcc -o test_case main.c -lcunit
#include "CUnit/Automated.h"
//#include "CUnit/Console.h"
//#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

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
#define PROP4 "fenetre ouvre"


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

/* Test suite setup and cleanup functions: */
int init_suite(void) { return 0; }
int clean_suite(void) { return 0; }

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
 * [paiement destinataire %montant].*/
bool is_correct_capteur(capteur_vocal cdv){
    int number_of_word = count_words(cdv.commande);
    //printf("number of word : %d\n", number_of_word);
    if(number_of_word >= 2 || number_of_word <= 3){
        if(number_of_word == 2){
            if (cdv.commande == PROP1 || cdv.commande == PROP2 || cdv.commande == PROP3 || cdv.commande == PROP4){
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
                printf("operation bancaire precedente ratee\n");
                break;
            case ILLICITE:
                printf("opération bancaire precedente illicite\n");
                break;
            case REUSSITE:
                printf("operation bancaire precedente reussie\n");
                break;
            case NC:
                printf("aucune operation bancaire precedemment\n");
                break;
            default:
                printf("operation precedente non reconnue\n");
                break;
        }
    }else{
        printf("retour inconnu\n");
    }
}

/*
* Rajouter la gestion de l'authentification lors de l'achat
*/
bool controleur(capteur_vocal cdv, float curr_temp, int curr_pourc, int curr_cb){
    //phase de sécurisation des entrées.
    // 15 <= curr_temp <= 25 && 0 < curr_pourc <= 100 && curr_cb = [-1, 0, 1, 2]
    bool job_is_done = false;
    int compteur = count_words(cdv.commande);
    commande com;
    //état de base
    com.temperature = 20;
    com.pourcentage = 0;
    com.destinataire = NULL;
    com.montant = 0;
    //printf("avant correct temperature : %f\n", com.temperature);
    if (is_correct_temp(curr_temp)== true && is_correct_pourc(curr_pourc) == true && is_correct_capteur(cdv) == true){
        job_is_done = true;
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
            }else if(compteur == 2) {
                com.pourcentage = 100;
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
    //affichage cosmétique
    affichage_cb(curr_cb);
    printf("valeur temperature : %f\n", ret.temperature);
    printf("valeur pourcentage : %d\n", ret.pourcentage);
    printf("valeur destinataire : %s\n", ret.destinataire);
    printf("valeur montant : %d\n", ret.montant);
    return job_is_done;
}

/*
*****************************************************
* TEST CASE pour controleur, A VERIFIER
* Ajouter les tests pour les températures et pourcentage en entrée de controleur
* Pas de tests pour le retour du compte bancaire, on ne fait qu'afficher ce que nous dit la banque
*****************************************************
*/

//test comportement logiciel commande fenetre.
void test_CC_fenetre(){
    capteur_vocal cv1 = {"fenetre ouvre 25", 1};
    capteur_vocal cv2 = {"fenetre ferme", 1};
    capteur_vocal cv3 = {"fenetre ouvre", 1};
    capteur_vocal cv4 = {"fenetre ouvrir 25", 1};
    capteur_vocal cv5 = {"fenetre fermer", 1};
    capteur_vocal cv6 = {"fenetre ouvre -25", 1};
    capteur_vocal cv7 = {"fene ferme", 1};
    /*capteur_vocal cv1;
    capteur_vocal cv2;
    capteur_vocal cv3;
    capteur_vocal cv4;
    capteur_vocal cv5;
    capteur_vocal cv6;
    capteur_vocal cv7;

    cv1.commande = "fenetre ouvre 25";
    cv1.auth = 1;
    cv2.commande = "fenetre ferme";
    cv2.auth = 1;
    cv3.commande = "fenetre ouvre";
    cv3.auth = 1;
    cv4.commande = "fenetre ouvrir 25";
    cv4.auth = 1;
    cv5.commande = "fenetre fermer";
    cv5.auth = 1;
    cv6.commande = "fenetre ouvre -25";
    cv6.auth = 1;
    cv7.commande = "fene ferme";
    cv7.auth = 1;*/

    CU_ASSERT_EQUAL( controleur(cv1, 15, 0, NC), true);
    CU_ASSERT_EQUAL( controleur(cv2, 15, 0, NC), true);
    CU_ASSERT_EQUAL( controleur(cv3, 15, 0, NC), true);
    CU_ASSERT_EQUAL( controleur(cv4, 15, 0, NC), false);
    CU_ASSERT_EQUAL( controleur(cv5, 15, 0, NC), false);
    CU_ASSERT_EQUAL( controleur(cv6, 15, 0, NC), false);
    CU_ASSERT_EQUAL( controleur(cv7, 15, 0, NC), false);
    
}


//test comportement logiciel commande chauffage.
void test_CC_chauffage(){
    capteur_vocal cv1 = {"chauffage augmente", 1};
    capteur_vocal cv2 = {"chauffage baisse", 1};
    capteur_vocal cv3 = {"chauffage augmentation", 1};
    capteur_vocal cv4 = {"chauffage diminution", 1};
    capteur_vocal cv5 = {"chauffage augmente 20", 1};
    capteur_vocal cv6 = {"chauffage baisse 30", 1};
    capteur_vocal cv7 = {"chauf baisse", 1};
/*
    cv1.commande = "chauffage augmente";
    cv1.auth = 1;
    cv2.commande = "chauffage baisse";
    cv2.auth = 1;
    cv3.commande = "chauffage augmentation";
    cv3.auth = 1;
    cv4.commande = "chauffage diminution";
    cv4.auth = 1;
    cv5.commande = "chauffage augmente 20";
    cv5.auth = 1;
    cv6.commande = "chauffage baisse 30";
    cv6.auth = 1;
    cv7.commande = "chauf baisse";
    cv7.auth = 1;*/

    CU_ASSERT_EQUAL( controleur(cv1, 15, 0, NC), true);
    CU_ASSERT_EQUAL( controleur(cv2, 15, 0, NC), true);
    CU_ASSERT_EQUAL( controleur(cv3, 15, 0, NC), false);
    CU_ASSERT_EQUAL( controleur(cv4, 15, 0, NC), false);
    CU_ASSERT_EQUAL( controleur(cv5, 15, 0, NC), false);
    CU_ASSERT_EQUAL( controleur(cv6, 15, 0, NC), false);
    CU_ASSERT_EQUAL( controleur(cv7, 15, 0, NC), false);
    
}

//test comportement logiciel commande paiement.
void test_CB(){
    capteur_vocal cv1 = {"paiement McDonalds 36", 1};
    capteur_vocal cv2 = {"paiement 36", 1};
    capteur_vocal cv3 = {"paiement McDonalds", 1};
    capteur_vocal cv4 = {"paiement EIDD 600", 1};
    capteur_vocal cv5 = {"paie EIDD 20", 1};
/*
    cv1.commande = "paiement McDonalds 36";
    cv1.auth = 1;
    cv2.commande = "paiement 36";
    cv2.auth = 1;
    cv3.commande = "paiement McDonalds";
    cv3.auth = 1;
    cv4.commande = "paiement EIDD 600";
    cv4.auth = 1;
    cv5.commande = "paie EIDD 20";
    cv5.auth = 1;*/

    CU_ASSERT_EQUAL( controleur(cv1, 15, 0, NC), true);
    CU_ASSERT_EQUAL( controleur(cv2, 15, 0, NC), false);
    CU_ASSERT_EQUAL( controleur(cv3, 15, 0, NC), false);
    CU_ASSERT_EQUAL( controleur(cv4, 15, 0, NC), false);
    CU_ASSERT_EQUAL( controleur(cv5, 15, 0, NC), false);
}

/*
*****************************************************
*****************************************************
*/

int main() {
    /*
    capteur_vocal test;
    test.commande[0]= FENETRE;
    test.commande[1] = OUVRIR;
    test.commande[2] = NC;
    //controleur(test, 0, 0, 0);
    is_correct_capteur(test);*/

    /*capteur_vocal cdv;
    cdv.commande = "paiement Diegu_enterprise 56";

    //printf("valeur : %d\n", is_correct_capteur(cdv));
    //char** test = parser(3,cdv.commande);

    commande ret = controleur(cdv, 20, 0, ECHEC);

    //printf("%s\n", test[1]);
    printf("valeur temperature : %f\n", ret.temperature);
    printf("valeur pourcentage : %d\n", ret.pourcentage);
    printf("valeur destinataire : %s\n", ret.destinataire);
    printf("valeur montant : %d\n", ret.montant);*/

    CU_pSuite pSuite = NULL;

    if ( CUE_SUCCESS != CU_initialize_registry() )
        return CU_get_error();

    pSuite = CU_add_suite( "CSC_test_suite", init_suite, clean_suite );
    if ( NULL == pSuite ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if ( (NULL == CU_add_test(pSuite, "test_CC_fenetre", test_CC_fenetre)) ||
         (NULL == CU_add_test(pSuite, "test_CC_chauffage", test_CC_chauffage)) ||
         (NULL == CU_add_test(pSuite, "test_CB", test_CB))
            )
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_automated_run_tests();
    //CU_basic_run_tests();
    printf("\n\n");

    CU_cleanup_registry();

    /*test_CC_fenetre();
    test_CC_chauffage();
    test_CB();*/

    return CU_get_error();
}
