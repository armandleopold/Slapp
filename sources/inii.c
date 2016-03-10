/* Nom : Menu.c

Dernier Auteur  : Armand

Creation :
08/02/2014

Dernière modification :
08/03/2014

Description :
    Gestion du menu

*///

#include "../headers/linker.h"

// Fonction qui créer et initialise les valeurs par défauts du jeu dans un fichier de configuration au format ini
void constructeur_config_file()
{
    FILE* flux = fopen("config.ini","w+");
        if (flux == NULL)
        {
            printf ("// Erreur a l'ouverture du fichier de config\n");
        }
        else
        {
            // Définition des parametres de la fenetre de jeu
            SetINIValueInt("config.ini","PARAM_FENETRE/largeur_tile",32);
            SetINIValueInt("config.ini","PARAM_FENETRE/hauteur_tile",32);
            SetINIValueInt("config.ini","PARAM_FENETRE/nb_blocs_largeur",40);
            SetINIValueInt("config.ini","PARAM_FENETRE/nb_blocs_hauteur",23);
            SetINIValueInt("config.ini","PARAM_FENETRE/largeur_fenetre",1280);
            SetINIValueInt("config.ini","PARAM_FENETRE/hauteur_fenetre",736);
            SetINIValueInt("config.ini","PARAM_FENETRE/fullscreen",0);

            // Définition des parametres du moteur graphique du jeu
            SetINIValueInt("config.ini","PARAM_GRAPHIC_ENGINE/image_par_secondes",60);
            SetINIValueInt("config.ini","PARAM_GRAPHIC_ENGINE/taille_map_x",1000);
            SetINIValueInt("config.ini","PARAM_GRAPHIC_ENGINE/taille_map_y",250);
            SetINIValueInt("config.ini","PARAM_GRAPHIC_ENGINE/marge_deplacement_x",1);
            SetINIValueInt("config.ini","PARAM_GRAPHIC_ENGINE/marge_deplacement_y",1);

            // Définition des parametres du moteur physique du jeu
            SetINIValueInt("config.ini","PARAM_PHYSIC_ENGINE/gravity",3000);
            SetINIValueInt("config.ini","PARAM_PHYSIC_ENGINE/vitesse_de_chute_max",1500);

            // Définition des parametres du menu principal du jeu
            SetINIValueInt("config.ini","PARAM_MENU/nbr_bouton",5);
            SetINIValueInt("config.ini","PARAM_MENU/top_menu_y",220);

            // Définition des parametres de la GUI du jeu
            SetINIValueInt("config.ini","PARAM_GUI/separateur",0);

            // Définition des parametres du son
            SetINIValueInt("config.ini","PARAM_AUDIO/musique",100);
            SetINIValueInt("config.ini","PARAM_AUDIO/effets",50);

            //SetINIValue("config.ini","CONFIG/map","");
        }
    fclose(flux);
}


// Fonction qui check l'intégrité des variables de base du jeu, sinon elle recréer le fichier de configuration
void check_config_file()
{
    int temp_value;

    FILE* flux = fopen("config.ini","r");
        if (flux == NULL)
        {
            constructeur_config_file();
        }
    fclose(flux);

    INIFile ini_fichier = ParseINI("config.ini");

    temp_value = GetINIValueInt(ini_fichier,"PARAM_FENETRE/largeur_tile",-1);
    if (temp_value == -1)
        constructeur_config_file();

    temp_value = GetINIValueInt(ini_fichier,"PARAM_FENETRE/hauteur_tile",-1);
    if (temp_value == -1)
        constructeur_config_file();

    temp_value = GetINIValueInt(ini_fichier,"PARAM_FENETRE/nb_blocs_largeur",-1);
    if (temp_value == -1)
        constructeur_config_file();

    temp_value = GetINIValueInt(ini_fichier,"PARAM_FENETRE/nb_blocs_hauteur",-1);
    if (temp_value == -1)
        constructeur_config_file();

    temp_value = GetINIValueInt(ini_fichier,"PARAM_FENETRE/largeur_fenetre",-1);
    if (temp_value == -1)
        constructeur_config_file();

    temp_value = GetINIValueInt(ini_fichier,"PARAM_FENETRE/hauteur_fenetre",-1);
    if (temp_value == -1)
        constructeur_config_file();

    temp_value = GetINIValueInt(ini_fichier,"PARAM_FENETRE/fullscreen",-1);
    if (temp_value == -1)
        constructeur_config_file();

    temp_value = GetINIValueInt(ini_fichier,"PARAM_GRAPHIC_ENGINE/image_par_secondes",-1);
    if (temp_value == -1)
        constructeur_config_file();

    temp_value = GetINIValueInt(ini_fichier,"PARAM_GRAPHIC_ENGINE/taille_map_x",-1);
    if (temp_value == -1)
        constructeur_config_file();

    temp_value = GetINIValueInt(ini_fichier,"PARAM_GRAPHIC_ENGINE/taille_map_y",-1);
    if (temp_value == -1)
        constructeur_config_file();

    temp_value = GetINIValueInt(ini_fichier,"PARAM_GRAPHIC_ENGINE/marge_deplacement_x",-1);
    if (temp_value == -1)
        constructeur_config_file();

    temp_value = GetINIValueInt(ini_fichier,"PARAM_GRAPHIC_ENGINE/marge_deplacement_y",-1);
    if (temp_value == -1)
        constructeur_config_file();

    temp_value = GetINIValueInt(ini_fichier,"PARAM_PHYSIC_ENGINE/gravity",-1);
    if (temp_value == -1)
        constructeur_config_file();

    temp_value = GetINIValueInt(ini_fichier,"PARAM_PHYSIC_ENGINE/vitesse_de_chute_max",-1);
    if (temp_value == -1)
        constructeur_config_file();

    temp_value = GetINIValueInt(ini_fichier,"PARAM_MENU/nbr_bouton",-1);
    if (temp_value == -1)
        constructeur_config_file();

    temp_value = GetINIValueInt(ini_fichier,"PARAM_MENU/top_menu_y",-1);
    if (temp_value == -1)
        constructeur_config_file();

    temp_value = GetINIValueInt(ini_fichier,"PARAM_GUI/separateur",-1);
    if (temp_value == -1)
        constructeur_config_file();

    temp_value = GetINIValueInt(ini_fichier,"PARAM_AUDIO/musique",-1);
    if (temp_value == -1)
        constructeur_config_file();

    temp_value = GetINIValueInt(ini_fichier,"PARAM_AUDIO/effets",-1);
    if (temp_value == -1)
        constructeur_config_file();

    FreeINI(ini_fichier);
}
