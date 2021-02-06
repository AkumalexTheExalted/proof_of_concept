#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "CUnit/Automated.h"
#include <CUnit/CUnit.h>

//Variation maximum de température
#define MAX_TEMP    25
#define MIN_TEMP    15
/*
 * Variation d'ouverture de la fenêtre en pourcentage
 * 0 = fenêtre fermée
 * 100 fenêtre entièrement ouverte
 */
#define MAX_POURCENTAGE 100
#define MIN_POURCENTAGE 0

//Minimum et maximum pour le paiement
#define MAX_AMOUNT  250
#define MIN_AMOUNT  10

//Nature de la transaction, envoyé par la banque
#define ILLICITE    0
#define REUSSITE    1
#define ECHEC       -1
#define NC          -100

//Proposition captée par la reconnaissance vocale
#define PROP1 "fenetre ferme"
#define PROP2 "chauffage augmente"
#define PROP3 "chauffage baisse"
#define PROP4 "fenetre ouvre"

/**
 * capteur vocal
 * commande : la commande entrée par l'utilisateur et normalisée par le capteur de reconnaissance vocale
 * auth : à 1 si la voix est celle du propriétaire de la voiture, à 0 sinon
 */
typedef struct capteur_vocal{
    char* commande;
    bool auth;
} capteur_vocal;

/**
 * commande de la voiture
 * temperature : température actuelle de la voiture
 * pourcentage : pourcentage d'ouverture des fenêtres
 * destinataire : destinataire du paiement
 * montant : montant du paiement
 */
typedef struct commande{
    float temperature;
    int pourcentage;
    char* destinataire;
    int montant;
} commande;

//Initialisation des tests
int init_suite(void) { return 0; }
int clean_suite(void) { return 0; }

/**
 * PROGRAMMATION DEFENSIVE : vérifie que la température est entre les bornes
 * @param temp la température à vérifier
 * @return true si la température est entre les bornes, false sinon
 */
bool is_correct_temp(float temp){
    if(temp >= MIN_TEMP && temp <= MAX_TEMP){
        return true;
    }
    return false;
}

/**
 * PROGRAMMATION DEFENSIVE : vérifie que le pourcentage d'ouverture de la fenêtre est correct
 * @param pourc le pourcentage d'ouverture que l'on veut appliquer à la fenêtre
 * @return true si le pourcentage est correct, false sinon
 */
bool is_correct_pourc(int pourc){
    if(pourc >= MIN_POURCENTAGE && pourc <= MAX_POURCENTAGE){
        return true;
    }
    return false;
}

/**
 * PROGRAMMATION DEFENSIVE : vérifie que le montant à payer est correct
 * @param pourc le montant à vérifier
 * @return true si le montant est entre les bornes, false sinon
 */
bool is_correct_amount(int pourc){
    if(pourc >= MIN_AMOUNT && pourc <= MAX_AMOUNT){
        //printf("OK MONTANT\n");
        return true;
    }
    return false;
}

/**
 * PROGRAMMATION DEFENSIVE : vérifie que la nature de l'opération bancaire est correcte
 * @param cb    le retour de la banque à vérifier
 * @return  true si le retour est correct, false sinon
 */
bool is_correct_cb(int cb){
    if(cb == ECHEC || cb == ILLICITE || cb == REUSSITE || cb == NC){
        return true;
    }
    return false;
}

/**
 * Compte le nombre de mot dans une chaîne
 * @param instruction   la chaîne de caractère à analyser
 * @return le nombre de mot dans la chaîne
 */
int count_words(char *instruction){
    int count = 1;
    for (int i = 0; instruction[i] != '\0'; i++) {
        if (instruction[i] == ' ' && instruction[i+1] != ' ')
            count++;
    }
    return count;
}

/**
 * Vérifie si une chaîne de caractère est un nomber
 * @param val   la chaîne de caractère à analyser
 * @return  true si la chaîne est un nombre, false sinon
 */
bool is_numeric_value(char* val){
    for(int i = 0 ; val[i] != '\0' ; i++){
        if(val[i] < '0' || val[i] > '9'){
            return false;
        }
    }
    return true;
}

/**
 * Transforme une chaîne de caractère en tableau
 * Chaque case contient un mot
 * @param taille    la taille du tableau
 * @param commande  la chaîne de caractère à transformer
 * @return  le tableau contenant les mots de la chaîne
 */
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
        split = strtok(NULL, " ");
        i++;
    }
    return token;
}

/**
 * PROGRAMMATION DEFENSIVE : vérifie que les informations données par le capteur sont correctes
 * @param cdv la commande vocale à analyser
 * @return true si les isntructions sont correctes, false sinon
 */
bool is_correct_capteur(capteur_vocal cdv){
    int number_of_word = count_words(cdv.commande);
    if(number_of_word >= 2 || number_of_word <= 3){
        if(number_of_word == 2){    //Si l'instruction contient 2 mots, il n'y a que 4 instructions possibles
            if (cdv.commande == PROP1 || cdv.commande == PROP2 || cdv.commande == PROP3 || cdv.commande == PROP4){
                return true;
            }
        }else{
            char **token = parser(3, cdv.commande);     //cas de 3 mots dans l'instruction
            if (strncmp(token[0], "fenetre", 7) == 0 && strncmp(token[1], "ouvre", 5) == 0){
                if(is_numeric_value(token[2])){     //vérifier que le dernier mot est un nombre
                    int numeric_value = atoi(token[2]);
                    if(is_correct_pourc(numeric_value)){    //vérifier le pourcentage d'ouverture de la fenêtre
                        return true;
                    }else{
                        return false;
                    }
                }
                return false;
            }
            //on ne peut payer que si c'est le propriétaire de la voiture qui fait la demande
            if (strncmp(token[0], "paiement", 8) == 0 && cdv.auth == 1){
                if(is_numeric_value(token[2])){ //vérifier que le dernier mot est un nombre
                    int numeric_value = atoi(token[2]);
                    if(is_correct_amount(numeric_value)){   //vérifier que le montant est correct
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

/**
 * Affichage du retour de la banque (purement cosmétique)
 * @param curr_cb   l'opération bancaire à analyser
 */
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
            default:
                printf("aucune operation bancaire precedemment\n");
                break;
        }
    }else{
        printf("retour inconnu\n");
    }
}

/**
 * Contrôle les demandes de commandes vocales et effectue la tâche demandée le cas échéant
 * @param cdv   la commande vocale de l'utilisateut avec son authentification
 * @param curr_temp     la température courante du véhicule
 * @param curr_pourc    la pourcentage d'ouverture courant de la fenêtre
 * @param curr_cb       la nature de la précédente opération bancaire retournée par la banque
 * @return  true si l'instruction demandée a pu être effectuée, false sinon
 */
bool controleur(capteur_vocal cdv, float curr_temp, int curr_pourc, int curr_cb){
    bool job_is_done = false;
    int compteur = count_words(cdv.commande);   //compte le nombre de mot de l'instruction
    commande com;
    //Initialisation à l'état de base
    com.temperature = 20;
    com.pourcentage = 0;
    com.destinataire = NULL;
    com.montant = 0;
    //vérification de la véracité des instructions
    if (is_correct_temp(curr_temp)== true && is_correct_pourc(curr_pourc) == true && is_correct_capteur(cdv) == true){
        job_is_done = true; //si les instructions sont correctes, on va forcément aboutir à une action
        //Initialisation à l'état courant
        com.temperature = curr_temp;
        com.pourcentage = curr_pourc;
        com.destinataire = NULL;
        com.montant = 0;
        char **commande = parser(count_words(cdv.commande), cdv.commande);  //lecture de l'instruction
        if(strncmp(commande[0], "fenetre", 7) == 0){
            if (strncmp(commande[1], "ferme", 7) == 0) {
                com.pourcentage = 0;
            }else if(compteur == 2) {
                com.pourcentage = 100;
            }else{
                com.pourcentage = atoi(commande[2]);    //pourcentage vérifié dans is_correct_capteur
            }
        }else if(strncmp(commande[0], "chauffage", 9) == 0){
            if(strncmp(commande[1], "augmente", 8) == 0 && (curr_temp + 2 <= MAX_TEMP)){
                com.temperature = curr_temp + 2;
            }else if (strncmp(commande[1], "baisse", 8) == 0 && curr_temp - 2 >= MIN_TEMP){
                com.temperature = curr_temp - 2;
            }
        }else{
            com.destinataire = commande[1];
            com.montant = atoi(commande[2]);    //montant vérifié dans is_correct_capteur
        }
    }
    //affichage cosmétique du retour de la banque, pas de tests là dessus, ça dépend de la banque, pas de la voiture
    affichage_cb(curr_cb);
    printf("valeur temperature : %f\n", com.temperature);
    printf("valeur pourcentage : %d\n", com.pourcentage);
    printf("valeur destinataire : %s\n", com.destinataire);
    printf("valeur montant : %d\n", com.montant);
    return job_is_done;
}

/*
*****************************************************
* TEST CASE pour controleur, A VERIFIER
* Ajouter les tests pour les températures et pourcentage en entrée de controleur
* Pas de tests pour le retour du compte bancaire, on ne fait qu'afficher ce que nous dit la banque
*****************************************************
*/
/**
 * TEST CASE : comportement du contrôleur lors de la manipulation de la fenêtre
 */
void test_CC_fenetre(){
    capteur_vocal cv1 = {"fenetre ouvre 25", 1};    //ouverture correcte
    capteur_vocal cv2 = {"fenetre ferme", 1};       //fermeture correcte
    capteur_vocal cv3 = {"fenetre ouvre", 1};       //ouverture correcte
    capteur_vocal cv4 = {"fenetre ouvrir 25", 1};   //erreur d'instruction "ouvrir"
    capteur_vocal cv5 = {"fenetre fermer", 1};      //erreur d'instruction "fermer"
    capteur_vocal cv6 = {"fenetre ouvre -25", 1};   //erreur de pourcentage
    capteur_vocal cv7 = {"fene ferme", 1};          //erreur d'instruction

    CU_ASSERT_EQUAL( controleur(cv1, 15, 0, NC), true);
    CU_ASSERT_EQUAL( controleur(cv2, 15, 0, NC), true);
    CU_ASSERT_EQUAL( controleur(cv3, 15, 0, NC), true);
    CU_ASSERT_EQUAL( controleur(cv4, 15, 0, NC), false);
    CU_ASSERT_EQUAL( controleur(cv5, 15, 0, NC), false);
    CU_ASSERT_EQUAL( controleur(cv6, 15, 0, NC), false);
    CU_ASSERT_EQUAL( controleur(cv7, 15, 0, NC), false);
    
    CU_ASSERT_EQUAL( controleur(cv1, 15, 90, NC), true);
    CU_ASSERT_EQUAL( controleur(cv2, 15, 0, NC), true);
    CU_ASSERT_EQUAL( controleur(cv3, 15, 100, NC), true);
    CU_ASSERT_EQUAL( controleur(cv3, 15, 120, NC), false);
}

/**
 * TEST CASE : comportement du contrôleur lors de la manipulation du chauffage
 */
void test_CC_chauffage(){
    capteur_vocal cv1 = {"chauffage augmente", 1};  //instruction correcte
    capteur_vocal cv2 = {"chauffage baisse", 1};    //instruction correcte
    capteur_vocal cv3 = {"chauffage augmentation", 1};  //erreur d'instruction "augmentation"
    capteur_vocal cv4 = {"chauffage diminution", 1};    //erreur d'instruction "diminution"
    capteur_vocal cv5 = {"chauffage augmente 20", 1};   //erreur d'instruction
    capteur_vocal cv6 = {"chauffage baisse 30", 1};     //erreur d'instruction
    capteur_vocal cv7 = {"chauf baisse", 1};            //erreur d'instruction

    CU_ASSERT_EQUAL( controleur(cv1, 15, 0, NC), true);
    CU_ASSERT_EQUAL( controleur(cv2, 15, 0, NC), true);
    CU_ASSERT_EQUAL( controleur(cv3, 15, 0, NC), false);
    CU_ASSERT_EQUAL( controleur(cv4, 15, 0, NC), false);
    CU_ASSERT_EQUAL( controleur(cv5, 15, 0, NC), false);
    CU_ASSERT_EQUAL( controleur(cv6, 15, 0, NC), false);
    CU_ASSERT_EQUAL( controleur(cv7, 15, 0, NC), false);
    
    CU_ASSERT_EQUAL( controleur(cv1, 25, 0, 0), true);
    CU_ASSERT_EQUAL( controleur(cv2, 35, 0, NC), false);
}

/**
 * TEST CASE : comportement du contrôleur lors d'une demande de paiement
 */
void test_CB(){
    capteur_vocal cv1 = {"paiement McDonalds 36", 1};   //paiement correct
    capteur_vocal cv2 = {"paiement 36", 1};     //sans destinataire
    capteur_vocal cv3 = {"paiement McDonalds", 1};  //sans montant
    capteur_vocal cv4 = {"paiement EIDD 600", 1};   //montant trop élevé
    capteur_vocal cv5 = {"paie EIDD 20", 1};    //montant trop faible
    capteur_vocal cv6 = {"paiement McDonalds 36", 0};   //authentification incorrect

    CU_ASSERT_EQUAL( controleur(cv1, 15, 0, NC), true);
    CU_ASSERT_EQUAL( controleur(cv2, 15, 0, NC), false);
    CU_ASSERT_EQUAL( controleur(cv3, 15, 0, NC), false);
    CU_ASSERT_EQUAL( controleur(cv4, 15, 0, NC), false);
    CU_ASSERT_EQUAL( controleur(cv5, 15, 0, NC), false);
    CU_ASSERT_EQUAL( controleur(cv6, 15, 0, NC), false);

    CU_ASSERT_EQUAL( controleur(cv1, 15, 0, 0), true);
    CU_ASSERT_EQUAL( controleur(cv1, 15, 0, 1), true);
    CU_ASSERT_EQUAL( controleur(cv1, 15, 0, -1), true);
    CU_ASSERT_EQUAL( controleur(cv1, 15, 0, NC), true);
    CU_ASSERT_EQUAL( controleur(cv1, 15, 0, 250), true);
}

/*
*****************************************************
*****************************************************
*/

int main() {
    //lancement des TC du contrôleur
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
    printf("\n\n");
    CU_cleanup_registry();
    return CU_get_error();
}
