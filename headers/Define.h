/* Nom : Define.h

Dernier Auteur  : Armand

Creation :
24/01/2014

Dernière modification :
12/02/2014

Description :
     Definition de toutes les constantes du programme


Ce serais intéréssant à la fin de mettre les #DEFINES dans un fichier de config qui serais chargé par le programme
au début comme ça on pourrait modifier des paramètres du jeux sans avoir à le recompiler.
*///

/* Condition de préprocesseur pour éviter les inclusions infinies (2 lignes suivantes) */
#ifndef DEF_DEFINE // Si la constante n'a pas été définie le fichier Define n'a jamais été inclus
#define DEF_DEFINE // On définit la constante pour que la prochaine fois le fichier ne soit plus inclus

    /* Parametres Jeux */
    #define MAP_LEVEL1 "map/helloworld.map"
    #define MAP_LEVEL2 "map/mo.map"
    #define CHRONO 180

    /* Parametres Fenetre */
    #define LARGEUR_TILE         32 // Largeur d'une tiles (carré) en pixels
    #define HAUTEUR_TILE         32
    #define NB_BLOCS_LARGEUR     40 // 30
    #define NB_BLOCS_HAUTEUR     23 // 20
    #define LARGEUR_FENETRE      LARGEUR_TILE * NB_BLOCS_LARGEUR //32*40 = 1280
    #define HAUTEUR_FENETRE      HAUTEUR_TILE * NB_BLOCS_HAUTEUR //32*20 = 640

    /* Parametres Moteur graphique */
    #define IMAGE_PAR_SECONDES 60
    #define TAILLE_MAP_X     1000
    #define TAILLE_MAP_Y     250
    #define MARGE_DEPLACEMENT_X 2
    #define MARGE_DEPLACEMENT_Y 2

    /* Parametres Moteur Physique */
    #define GRAVITY 3000
    #define VITESSE_DE_CHUTE_MAX 1500

    /* Parametres Menu*/
    #define NBR_BOUTON 5
    #define TOP_MENU_Y  220

    /* Parametres GUI */
    #define SEPARATEUR 0

    /*----------------------------------------------------------------------------------------------*/
                                                /* Phase 1 */

    #define TAILLE_X_ALEATOIRE 200  // Taille du tableau de coordonnées x aleatoire
    #define TAILLE_Y_ALEATOIRE 200  // Taille du tableau de coordonnées y aleatoire
    #define TAILLE_VITESSE_X 200    // Taille du tableau de vitesses aléatoire horizontale
    #define TAILLE_VITESSE_Y 200    // Taille du tableau de vitesses aléatoire horizontale

    #define VITESSE_Y_PHASE_1 4  // Vitesse de chute de Slappy en pixels par centième de seconde
    #define VITESSE_X_PHASE_1 4   // Vitesse horizontale de Slappy en pixels par centième de seconde

    #define VITESSE_MIN_X_GOUTTE 1
    #define VITESSE_MAX_X_GOUTTE 3
    #define VITESSE_MIN_Y_GOUTTE 4
    #define VITESSE_MAX_Y_GOUTTE 8

    #define NB_ETATS_SLAPPY 4    // Nombre de positions du personage
    #define NB_GOUTTES 12 // Nombre de gouttes sur l'écran

    enum {ATTENTE, BAS, DROITE, GAUCHE}; // pour Slappy
    enum {DEPART, PETIT, MOYEN, GROS}; // pour les nuages

    /*----------------------------------------------------------------------------------------------*/


#endif // fin de la condition de preprocesseur
