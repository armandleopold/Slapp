/* Nom : main.c

Dernier Auteur  : Armand

Creation :
24/01/2014

Dernière modification :
08/03/2014

Description :
    Fichier main

*///

#include "headers/linker.h"

// Fonction qui initialise les composants SDL
void MAIN_chargement_sdl()
{
    // Chargement de la SDL

    if ( SDL_Init(SDL_INIT_EVERYTHING) < 0 )
    {
        printf("Chargement SDL : ERREUR %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    else
    {
        atexit(SDL_Quit);
    }

     // Initialisation de l'API TTF pour afficher du texte

    if(TTF_Init() < 0)
    {
        printf("Chargement SDL_TTF : ERREUR %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }
    else
    {
        atexit(TTF_Quit);
    }

     //Initialisation de l'API Mixer pour jouer du son

    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) < 0)
    {
        printf("Chargement SDL_Mixer : ERREUR %s\n", Mix_GetError());
        exit(EXIT_FAILURE);
    }
    else
    {
        atexit(Mix_CloseAudio);
    }
}

int main(int argc, char *argv[])
{
    // On charge la SDL
    MAIN_chargement_sdl();

    /*----------------------------------------------------------------------------*/
                                         /*                                       */
        SDL_Window   *splash_screen;     /* La fenetre                            */
        SDL_Renderer *splash_renderer;   /* La surface graphique                  */
        SDL_Texture  *splash_tx;         /* La surface logique                    */
        SDL_Rect position_menu;          /* La position de rendering pour l'image */
                                         /*                                       */
        int w, h;                        /* Variables taille de l'image chargée   */
                                         /*                                       */
    /*----------------------------------------------------------------------------*/

    splash_screen = SDL_CreateWindow(   "splash screen",
                                        SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED,
                                        718,
                                        360,
                                        SDL_WINDOW_BORDERLESS);

    splash_renderer = SDL_CreateRenderer(splash_screen, -1, SDL_RENDERER_ACCELERATED);

    if (splash_screen != NULL
    &&  splash_renderer != NULL)
    {
        // Fonctions d'initialisation
        SDL_SetHint             (SDL_HINT_RENDER_SCALE_QUALITY, "linear");
        SDL_RenderSetLogicalSize(splash_renderer, 718, 360);
        SDL_SetRenderDrawColor  (splash_renderer, 0, 0, 0, 255);
        SDL_RenderClear         (splash_renderer);

        // Chargement de l'image
        splash_tx = IMG_LoadTexture(splash_renderer,"artwork/splash_screen.png");

        // Récupération de sa dimension
        SDL_QueryTexture(splash_tx, NULL, NULL, &w, &h);

        // Positionnement centré de l'image par rapport à la fenetre
        position_menu.x = fabs(718/2 - w/2);
        position_menu.y = 0;
        position_menu.w = w;
        position_menu.h = h;

        // On applique l'image à notre ecran
        SDL_RenderCopy(splash_renderer,splash_tx,NULL,&position_menu);

        // On affiche l'ecran
        SDL_RenderPresent(splash_renderer);

        // On attend une demi-seconde
        SDL_Delay(500);

        // On détruit tout
        SDL_DestroyTexture (splash_tx);
        SDL_DestroyRenderer(splash_renderer);
        SDL_DestroyWindow  (splash_screen);
    }
    else
    {
        exit(EXIT_FAILURE);
    }

    // On check le fichier de config
    check_config_file();

    /*---------------------------------------------------------------------------------*/
                                        /*                                             */
        SDL_Window   *fenetre;          /* La fenetre                                  */
        SDL_Renderer *ecran;            /* L'ecran                                     */
                                        /*                                             */
        SDL_Surface *icon;              /* L'icone de la fenetre                       */
                                        /*                                             */
        INIFile ini_fichier;            /* Le buffer du fichier de configuration       */
                                        /*                                             */
        int largeur_fenetre = 0;        /*                                             */
        int hauteur_fenetre = 0;        /*                                             */
                                        /*                                             */
    /*---------------------------------------------------------------------------------*/

    // On récupère les variables de définition de résolution de la fenetre
    ini_fichier     = ParseINI("config.ini");
    largeur_fenetre = GetINIValueInt(ini_fichier,"PARAM_FENETRE/nb_blocs_largeur",40)*32;
    hauteur_fenetre = GetINIValueInt(ini_fichier,"PARAM_FENETRE/nb_blocs_hauteur",23)*32;

    // On créer la fenetre
    if(GetINIValueInt(ini_fichier,"PARAM_FENETRE/fullscreen",-1) == 1) // Fenetre plein écran
    {
        fenetre  = SDL_CreateWindow("SLAPP Beta 1.0.1",
                                           SDL_WINDOWPOS_CENTERED,
                                           SDL_WINDOWPOS_CENTERED,
                                           0,
                                           0,
                                           SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_OPENGL);

        // On créer le renderer de la fenetre
        ecran = SDL_CreateRenderer(fenetre, -1, SDL_RENDERER_ACCELERATED);
                SDL_GetWindowSize(fenetre,&largeur_fenetre,&hauteur_fenetre);
    }
    else // Mode fenetré
    {
        fenetre  = SDL_CreateWindow("SLAPP Beta 1.0.1",
                                           SDL_WINDOWPOS_CENTERED,
                                           SDL_WINDOWPOS_CENTERED,
                                           largeur_fenetre,
                                           hauteur_fenetre,
                                           SDL_WINDOW_OPENGL);

        // On créer le renderer de la fenetre
        ecran = SDL_CreateRenderer(fenetre, -1, SDL_RENDERER_ACCELERATED);
    }

    FreeINI(ini_fichier);

    if (fenetre != NULL
    &&  ecran != NULL)
    {
        // On initialise l'écran
        SDL_RenderSetLogicalSize(ecran,largeur_fenetre,hauteur_fenetre);
        SDL_SetHint             (SDL_HINT_RENDER_SCALE_QUALITY, "linear");
        SDL_SetRenderDrawColor  (ecran, 0, 0, 0, 255);
        SDL_RenderClear         (ecran);

        // On charge l'icone de la fenetre
        icon = IMG_Load("artwork/slapp_logo_ICON2.png");

        // On affiche l'icone
        SDL_SetWindowIcon(fenetre,icon);

        // On lance le Menu Principal
        Display_Menu(fenetre,ecran);

        SDL_DestroyRenderer(ecran);
        SDL_DestroyWindow(fenetre);
        SDL_FreeSurface(icon);
    }
    else
    {
        printf("Erreur de creation de la fenetre: %s\n",SDL_GetError());
        printf("Erreur de creation du renderer: %s\n",SDL_GetError());
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}