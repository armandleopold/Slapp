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


// Fonction qui gère les actions du menu pour les différents boutons
int Submit_Menu(struct state bouton,SDL_Window *fenetre,SDL_Renderer *sdlRenderer)
{
    int quit = 0;

    // EDITEUR DE MAP
    if(bouton.etat[2] == 1)
    {
        ANIM_fade_out(fenetre);
        GLOBAL_MOTEUR_EDITEUR(fenetre);
    }

    // JOUER
    if(bouton.etat[0] == 1)
    {
        ANIM_fade_out(fenetre);
        SUPER_GLOBAL_MOTEUR_JEU(fenetre,sdlRenderer,0);
    }

    // CHARGER UNE PARTIE
    if(bouton.etat[1] == 1)
    {
        ANIM_fade_out(fenetre);
        SUPER_GLOBAL_MOTEUR_JEU(fenetre,sdlRenderer,1);
    }

    // OPTIONS PRINCIPALES
    if(bouton.etat[3] == 1)
    {
        OPTIONS(fenetre);
        SDL_Surface *ecran = SDL_GetWindowSurface(fenetre);
        SDL_UpdateWindowSurface(fenetre);
        SDL_FreeSurface(ecran);
    }

    // QUITTER LE JEUX
    if(bouton.etat[4] == 1)
    {
        quit=1;
    }

    bouton.etat[0] = 0;
    bouton.etat[1] = 0;
    bouton.etat[2] = 0;
    bouton.etat[3] = 0;
    bouton.etat[4] = 0;

    return quit;
}

// Fonction qui affiche l'écran de crédit du jeu
void credit(SDL_Window *fenetre)
{
    /*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
                                                                                                    /*                                                                         */
        Uint32 start;                                                                               /* compteur de temps pour restreindre le taux de rafraichissement à 60 IPS */
        SDL_Event event;                                                                            /* variable qui contient les events                                        */
                                                                                                    /*                                                                         */
        SDL_Surface *ecran = SDL_GetWindowSurface(fenetre);                                         /* surface qui contient l'ecran                                            */
        SDL_Surface *menu  = IMG_Load("artwork/splash_screen.png");                                 /* l'image de crédit                                                       */
                                                                                                    /*                                                                         */
        SDL_Rect position_menu = {ecran->w/2-menu->w/2,ecran->h/2-menu->h/2, menu->w, menu->h};     /* position centré de l'image par raport à la fenetre                      */
                                                                                                    /*                                                                         */
        int quit = 0;                                                                               /* variable de sortie de la boucle                                         */
                                                                                                    /*                                                                         */
    /*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

    // Affichage du fond noir + de l'image 
    SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 17, 17, 17));
    SDL_BlitSurface(menu,NULL,ecran,&position_menu);

    SDL_UpdateWindowSurface(fenetre);

    while(quit==0)
    {
        start = SDL_GetTicks();

        while(SDL_PollEvent(&event));
        {
            switch(event.type)
            {
                case SDL_QUIT:
                {
                    quit=1;
                }
                case SDL_KEYUP:
                {
                    quit=1;
                }

                case SDL_MOUSEBUTTONDOWN:
                {
                    quit=1;
                }
            }
        }
        if(1000/IMAGE_PAR_SECONDES > SDL_GetTicks()-start)
        {
            SDL_Delay(1000/IMAGE_PAR_SECONDES-(SDL_GetTicks()-start));
        }
    }

    SDL_FreeSurface(ecran);
    SDL_FreeSurface(menu);
}

// Fonction qui gère l'affichage du menu
void Display_Menu(SDL_Window *fenetre,SDL_Renderer *sdlRenderer)
{
    /*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
                                                                                                  /*                                                                                       */
        struct state etat_bouton;                                                                 /* Variable qui va contenir l'état des boutons du menu                                   */
        struct GUI_object obj[1];                                                                 /* Variable qui contient les propriétés de taille du bouton crédit                       */
        struct state btn_credit;                                                                  /* variable qui va contenir l'état du bouton crédit                                      */
                                                                                                  /*                                                                                       */
        const char *image[NBR_BOUTON*2];                                                          /* tableau d'image qui va contenir les nom des images des boutons du menu                */
                                                                                                  /*                                                                                       */
        int start;                                                                                /* variable qui va contenir le temps (permet de modifier le taux de rafraichissement)    */
        int quit    = 0 ;                                                                         /* variable qui permet de sortir d'une boucle                                            */
        int i       = 0 ;                                                                         /* variable d'itérration                                                                 */
        int curseur = -1;                                                                         /* variable qui contient la position du menu au clavier                                  */
        int render  = 0 ;                                                                         /* variable qui permet de savoir si on réaffiche le menu ou non                          */
        int play_mouse_sound_over = 0;                                                            /* variable qui définit si on joue ou non le chunk de séléction de bouton menu (souris)  */
                                                                                                  /*                                                                                       */
        // Variables globales de son                                                              /*                                                                                       */
        int volume_musique = 0;                                                                   /*                                                                                       */
        int volume_effets = 0;                                                                    /*                                                                                       */
                                                                                                  /*                                                                                       */
        SDL_Surface *ecran     = SDL_GetWindowSurface(fenetre);                                   /* on récupère l'écran de la fenetre                                                     */
        SDL_Surface *menu      = IMG_Load("artwork/Menu.png");                                    /* on charge l'image du menu                                                             */
        SDL_Surface *back      = IMG_Load("artwork/back_bouton.png");                             /* on charge l'image de fond du menu                                                     */
        SDL_Surface *Tableau_BOUTON[NBR_BOUTON*2];                                                /* Tableau qui va contenir les images des boutons du menu                                */
                                                                                                  /*                                                                                       */
        SDL_Rect position_menu = {ecran->w/2-menu->w/2,0, menu->w, menu->h};                      /* On centre l'image par rapport à la fenetre                                            */
        SDL_Rect position_back = {fabs(ecran->w/2 - back->w/2),TOP_MENU_Y-15, back->w, back->h};  /* On centre l'image par rapport à la fenetre                                            */
                                                                                                  /*                                                                                       */
        SDL_Event event;                                                                          /* Variable qui contient les events SDL                                                  */
                                                                                                  /*                                                                                       */
        Mix_Music *musique;                                                                       /* La musique du menu                                                                    */
        Mix_Chunk *menu_sound;                                                                    /* Le son de selection de bouton                                                         */
                                                                                                  /*                                                                                       */
        INIFile ini_fichier;                                                                      /* Variable qui contient les données du fichier de configuration                         */
                                                                                                  /*                                                                                       */
    /*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

    // On charge toutes les images des boutons pour le menu
    image[0] = "artwork/Jouer_basic.png          ";
    image[1] = "artwork/Jouer_cursor.png         ";
    image[2] = "artwork/Charger_partie_basic.png ";
    image[3] = "artwork/Charger_partie_cursor.png";
    image[4] = "artwork/Editeur_basic.png        ";
    image[5] = "artwork/Editeur_cursor.png       ";
    image[6] = "artwork/Options_basic.png        ";
    image[7] = "artwork/Options_cursor.png       ";
    image[8] = "artwork/Quitter_basic.png        ";
    image[9] = "artwork/Quitter_cursor.png       ";

    // On défini un objet Gui -> le bouton crédit
    obj[0].largeur = 150;
    obj[0].hauteur = 24;
    obj[0].pos_x = ecran->w-obj[0].largeur;
    obj[0].pos_y = ecran->h-obj[0].hauteur;
    obj[0].id = 1;

    GUI_chargement_boutons(image,Tableau_BOUTON,NBR_BOUTON*2);

    SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 3, 96, 130));
    SDL_BlitSurface(menu,NULL,ecran,&position_menu); // Copie de l'image du menu
    SDL_BlitSurface(back,NULL,ecran,&position_back); // Copie de l'image du menu

    GUI_check_state(ecran,Tableau_BOUTON,1); // Affichage des boutons

    GUI_DISPLAY_button(obj[0],ecran,"Crédits");

    SDL_UpdateWindowSurface(fenetre);

    Mix_AllocateChannels(10); // On créer 10 cannaux de son différents

    musique = Mix_LoadMUS("musique/life_of_sin.mp3"); //Chargement de la musique
    menu_sound = Mix_LoadWAV("musique/effects/menu_1.wav"); // chargement du son du menu

    Mix_PlayMusic(musique, -1); //Jouer infiniment la musique

    ini_fichier = ParseINI("config.ini");

    // On récupère les variables de son  dans le fichier de config
    volume_musique = GetINIValueInt(ini_fichier,"PARAM_AUDIO/musique",0);
    volume_effets = GetINIValueInt(ini_fichier,"PARAM_AUDIO/effets",0);

    FreeINI(ini_fichier);

    // On applique le volume
    Mix_VolumeMusic(volume_musique);
    Mix_VolumeChunk(menu_sound, volume_effets);

    // Boucle d' evenements
    while(quit==0)
    {
        start = SDL_GetTicks(); // On récupère le temps
        SDL_WaitEvent(&event); // On attend un event
        while(SDL_PollEvent(&event)); // Récupération des actions de l'utilisateur
        {
            switch(event.type)
            {
                case SDL_QUIT: // Clic sur la croix
                {
                    quit=1;
                }
                case SDL_KEYUP: // Appuis d'une touche
                {
                    switch(event.key.keysym.sym)
                    {
                        case SDLK_UP:
                            curseur--;
                            Mix_PlayChannel(1, menu_sound, 0);
                            if (curseur < 0)
                            {
                                curseur = 0;
                            }
                            break;
                        case SDLK_DOWN:
                            curseur++;
                            Mix_PlayChannel(1, menu_sound, 0);
                            if (curseur >= NBR_BOUTON)
                            {
                                curseur = NBR_BOUTON-1;
                            }
                            break;
                        case SDLK_z:
                            curseur--;
                            Mix_PlayChannel(1, menu_sound, 0);
                            if (curseur < 0)
                            {
                                curseur = 0;
                            }
                            break;
                        case SDLK_s:
                            curseur++;
                            Mix_PlayChannel(1, menu_sound, 0);
                            if (curseur >= NBR_BOUTON)
                            {
                                curseur = NBR_BOUTON-1;
                            }
                            break;
                        case SDLK_1:
                            curseur = 0;
                            Mix_PlayChannel(1, menu_sound, 0);
                            break;
                        case SDLK_2:
                            curseur = 1;
                            Mix_PlayChannel(1, menu_sound, 0);
                            break;
                        case SDLK_3:
                            curseur = 2;
                            Mix_PlayChannel(1, menu_sound, 0);
                            break;
                        case SDLK_4:
                            curseur = 3;
                            Mix_PlayChannel(1, menu_sound, 0);
                            break;
                        case SDLK_5:
                            curseur = 4;
                            Mix_PlayChannel(1, menu_sound, 0);
                            break;
                        case SDLK_RETURN: // appuit sur ENTRER

                            Mix_PlayChannel(1, menu_sound, 0);

                            // On lance le sous menu
                            quit = Submit_Menu(GUI_set(curseur),fenetre,sdlRenderer);

                            position_menu.x = ecran->w/2-menu->w/2;
                            position_menu.y = 0;
                            position_menu.w = menu->w;
                            position_menu.h = menu->h;

                            SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 3, 96, 130));
                            SDL_BlitSurface(menu,NULL,ecran,&position_menu); // Copie de l'image du menu
                            SDL_BlitSurface(back,NULL,ecran,&position_back); // Copie de l'image du menu

                            GUI_check_state(ecran,Tableau_BOUTON,1); // Affichage des boutons

                            break;
                    }
                }
                case SDL_MOUSEMOTION: // Mouvement de la souris
                {
                    etat_bouton = GUI_check_state(ecran,Tableau_BOUTON,0); // On vérifie si la souris ne passe pas sur un bouton

                    for ( i = 0; i < NBR_BOUTON; ++i)
                    {
                       if (etat_bouton.etat[i] == 1)
                       {
                           if (play_mouse_sound_over == 0)
                           {
                               Mix_PlayChannel(1, menu_sound, 0);
                               play_mouse_sound_over = 1;
                           }
                           render = 1;
                       }
                    }

                    position_menu.x = ecran->w/2-menu->w/2;
                    position_menu.y = 0;
                    position_menu.w = menu->w;
                    position_menu.h = menu->h;

                    SDL_BlitSurface(menu,NULL,ecran,&position_menu); // Copie de l'image du menu
                    SDL_BlitSurface(back,NULL,ecran,&position_back); // Copie de l'image du menu

                    if (render == 0) // Si la souris ne passe nul part, on affiche le menu classique.
                    {
                        GUI_check_key_state(ecran,Tableau_BOUTON,GUI_set(curseur));
                        play_mouse_sound_over = 0;
                    }
                    else
                    {
                        // Sinon on supprime la selection clavier et on réaffiche les boutons 
                        curseur = -1;
                        GUI_check_state(ecran,Tableau_BOUTON,1);
                    }

                    // on affiche finalement le bouton crédit
                    btn_credit = GUI_DISPLAY_button(obj[0],ecran,"Crédits");
                }
                case SDL_MOUSEBUTTONDOWN: // clic de la souris
                {
                    if (event.button.button == SDL_BUTTON_LEFT)
                    {

                        if(btn_credit.etat[0] == 1)
                        {
                            ANIM_fade_out(fenetre);
                            credit(fenetre);
                        }

                        // on affiche le sous menu
                        quit = Submit_Menu(GUI_check_state(ecran,Tableau_BOUTON,0),fenetre,sdlRenderer);

                        position_menu.x = ecran->w/2-menu->w/2;
                        position_menu.y = 0;
                        position_menu.w = menu->w;
                        position_menu.h = menu->h;

                        SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 3, 96, 130));
                        SDL_BlitSurface(menu,NULL,ecran,&position_menu); // Copie de l'image du menu
                        SDL_BlitSurface(back,NULL,ecran,&position_back); // Copie de l'image du menu
 
                        GUI_check_state(ecran,Tableau_BOUTON,1); // Affichage des boutons

                        btn_credit = GUI_DISPLAY_button(obj[0],ecran,"Crédits");
                    }
                }
            }
            render = 0;
        }

        SDL_UpdateWindowSurface(fenetre);

        if(1000/IMAGE_PAR_SECONDES > SDL_GetTicks()-start)
        {
            SDL_Delay(1000/IMAGE_PAR_SECONDES-(SDL_GetTicks()-start));
        }
    }

    // Libération de la ram occupée par les images etc...
    GUI_free_tab_boutons(Tableau_BOUTON,NBR_BOUTON*2);
    Mix_FreeMusic(musique); //Libération de la musique
    Mix_FreeChunk(menu_sound);
    SDL_FreeSurface(menu);
    SDL_FreeSurface(back);
    SDL_FreeSurface(ecran);
}

// Fonction qui gère le menu d'options
int OPTIONS(SDL_Window *fenetre)
{
    /*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
                                                                                                    /*                                                                              */
        int reset_values = 0;                                                                       /* var bool qui détermine si on a modifier des variables de configuration       */
        int quit = 0;                                                                               /* var de sortie de boucle                                                      */
        int fullscreen;                                                                             /* var de config mode plein écran ou pas                                        */
        int vol_musique;                                                                            /* var de config vol musique                                                    */
        int vol_effets;                                                                             /* var de config vol bruitages                                                  */
        int ecran_x;                                                                                /* var de config largeur écran                                                  */
        int ecran_y;                                                                                /* var de config hauteur écran                                                  */
        int i = 0;                                                                                  /* var d'ittération                                                             */
        int get_value = 0;                                                                          /* var temp qui permet de récupéré un int à partir d'un input de type string    */
        int b_x, b_y;                                                                               /* var de config                                                                */
        int a_x,a_y;                                                                                /* var de config                                                                */
                                                                                                    /*                                                                              */
        char temps[200] = "";                                                                       /* var temp d'input pour les textbox de la GUI                                  */
                                                                                                    /*                                                                              */
        struct GUI_object obj[11];                                                                  /* les fenetres d'input                                                         */
        struct GUI_object resolution[8];                                                            /* les boutons du menu de selection de résolution                               */
        struct state etat[20];                                                                      /* tab qui défini l'état des différents boutons 0 : inactif / 1 : actif         */
                                                                                                    /*                                                                              */
        INIFile ini_fichier;                                                                        /* le buffer du fichier de config                                               */
                                                                                                    /*                                                                              */
        Uint32 start;                                                                               /* timer                                                                        */
        SDL_Event event;                                                                            /* event                                                                        */
                                                                                                    /*                                                                              */
        SDL_Surface *ecran = SDL_GetWindowSurface(fenetre);                                         /* l'ecran                                                                      */
        SDL_Surface *menu  = IMG_Load("artwork/Menu.png");                                          /* l'image de fond                                                              */
        SDL_Surface *back  = IMG_Load("artwork/back_bouton.png");                                   /* l'image de fond noir                                                         */
        SDL_Surface *render;                                                                        /* surface qui contient le texte généré par tff                                 */
                                                                                                    /*                                                                              */
        SDL_Rect position_menu = {ecran->w/2-menu->w/2,0, menu->w, menu->h};                        /* image de fond centré                                                         */
        SDL_Rect position_back = {fabs(ecran->w/2 - back->w/2),TOP_MENU_Y-15, back->w, back->h};    /* image de fond noir centré                                                    */
        SDL_Rect position = {fabs(ecran->w/2 - back->w/2 + 30),TOP_MENU_Y + 20, back->w, back->h};  /* la position des boutons                                                      */
                                                                                                    /*                                                                              */
        SDL_Color couleur = {255, 255, 255};                                                        /* la couleur du texte (noir)                                                   */
                                                                                                    /*                                                                              */
        TTF_Font *police = TTF_OpenFont("fonts/Roboto-Medium.ttf", 15);                             /* la police de caractère du texte                                              */
                                                                                                    /*                                                                              */
    /*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

    ini_fichier = ParseINI("config.ini");

    // On récupère les variables du fichier de config
    fullscreen  = GetINIValueInt(ini_fichier,"PARAM_FENETRE/fullscreen",0);
    vol_musique = GetINIValueInt(ini_fichier,"PARAM_AUDIO/musique",0);
    vol_effets  = GetINIValueInt(ini_fichier,"PARAM_AUDIO/effets",0);
    ecran_x     = GetINIValueInt(ini_fichier,"PARAM_FENETRE/nb_blocs_largeur",30);
    ecran_y     = GetINIValueInt(ini_fichier,"PARAM_FENETRE/nb_blocs_hauteur",23);

    FreeINI(ini_fichier);

    // On affiche les images de fond
    SDL_BlitSurface(menu,NULL,ecran,&position_menu);
    SDL_BlitSurface(back,NULL,ecran,&position_back);

    // On affiche les différentes variables avec les boutons
    sprintf(temps,"Volume de la musique : %d ",vol_musique);
    render = TTF_RenderText_Blended(police, temps, couleur);
    SDL_BlitSurface(render, NULL, ecran, &position);
    SDL_FreeSurface(render);

    position.y += 50;

    sprintf(temps,"Volume des effets : %d ",vol_effets);
    render = TTF_RenderText_Blended(police, temps, couleur);
    SDL_BlitSurface(render, NULL, ecran, &position);
    SDL_FreeSurface(render);

    position.y += 50;

    sprintf(temps,"Résolution écran : %d x %d ",ecran_x*32,ecran_y*32);
    render = TTF_RenderText_Blended(police, temps, couleur);
    SDL_BlitSurface(render, NULL, ecran, &position);
    SDL_FreeSurface(render);

    position.y += 50;

    sprintf(temps,"Plein écran : %d ",fullscreen);
    render = TTF_RenderText_Blended(police, temps, couleur);
    SDL_BlitSurface(render, NULL, ecran, &position);
    SDL_FreeSurface(render);

    // On défini les boutons
    for (i = 0; i < 4; ++i)
    {
        obj[i].largeur = 150;
        obj[i].hauteur = 24;
        obj[i].pos_x = ecran->w/2 - back->w/2 +335;
        obj[i].pos_y = TOP_MENU_Y + 20 +i*50;
        obj[i].id = i+1;
    }

    // On défini les textbox
    obj[10].largeur = 250;
    obj[10].hauteur = 100;
    obj[10].pos_x = ecran->w/2 - obj[10].largeur/2;
    obj[10].pos_y = ecran->h/2 - obj[10].hauteur/2;
    obj[10].id = 10;

    // On définit le message de sortis avec modif
    obj[6].largeur = 400;
    obj[6].hauteur = 100;
    obj[6].pos_x = ecran->w/2 - obj[6].largeur/2;
    obj[6].pos_y = ecran->h/2 - obj[6].hauteur/2;
    obj[6].id = 10;

    // On initialise le tableau avec les différentes résolutions

    sprintf(resolution[0].text,"800 x 600");
    sprintf(resolution[1].text,"1024 x 768");
    sprintf(resolution[2].text,"1280 x 1024");
    sprintf(resolution[3].text,"1440 x 810");
    sprintf(resolution[4].text,"1680 x 1050");
    sprintf(resolution[5].text,"1920 x 1080");
    sprintf(resolution[6].text,"Plein Ecran Fenétré");

    for (i = 0; i < 7; ++i)
    {
        if(i == 0)
        {
            resolution[i].largeur = 200;
            resolution[i].hauteur = 24;
            resolution[i].pos_x = ecran->w/2;
            resolution[i].pos_y = ecran->h/2;
            resolution[i].id = i;
        }
        else
        {
            resolution[i].largeur = resolution[i - 1].largeur ;
            resolution[i].hauteur = resolution[i - 1].hauteur ;
            resolution[i].pos_x = resolution[i - 1].pos_x;
            resolution[i].pos_y = resolution[i - 1].pos_y + resolution[i].hauteur + 3;
            resolution[i].id = i;
        }
    }

    resolution[7].largeur = 250;
    resolution[7].hauteur = 30;
    resolution[7].pos_y = ecran->h/2 - resolution[7].hauteur*6/2;
    resolution[7].pos_x = ecran->w/2 - resolution[7].largeur/2;
    resolution[7].id = 999;

    // On affiche les boutons
    GUI_DISPLAY_button(obj[0],ecran,"Modifier volume musique");
    GUI_DISPLAY_button(obj[1],ecran,"Modifier volume effets");
    GUI_DISPLAY_button(obj[2],ecran,"Modifier la résolution");
    GUI_DISPLAY_button(obj[3],ecran,"Modifier mode");

    SDL_UpdateWindowSurface(fenetre);

    while(quit==0)
    {
        start = SDL_GetTicks();

        while(SDL_PollEvent(&event));
        {
            switch(event.type)
            {
                case SDL_QUIT:
                {
                    quit=1;
                    if (reset_values == 1)
                    {
                        GUI_spam(fenetre,obj[6],1,"Les paramemtres s'appliqueront au prochain démarrage du jeu",NULL,NULL);
                    }
                }

                case SDL_KEYUP: // Relâchement d'une touche
                {
                    if(event.key.keysym.sym == SDLK_ESCAPE)
                    {
                        quit=1;
                        if (reset_values == 1)
                        {
                            GUI_spam(fenetre,obj[6],1,"Les paramemtres s'appliqueront au prochain démarrage du jeu",NULL,NULL);
                        }
                    }
                }

                case SDL_MOUSEMOTION: // Mouvement de la souris
                {

                    etat[0] = GUI_DISPLAY_button(obj[0],ecran,"Modifier volume musique");
                    etat[1] = GUI_DISPLAY_button(obj[1],ecran,"Modifier volume effets");
                    etat[2] = GUI_DISPLAY_button(obj[2],ecran,"Modifier la résolution");
                    etat[3] = GUI_DISPLAY_button(obj[3],ecran,"Modifier mode");

                }

                case SDL_MOUSEBUTTONDOWN: // clic de la souris
                {
                    if (event.button.button == SDL_BUTTON_LEFT)
                    {
                        if(etat[0].etat[0] == 1) // Modif volume musique
                        {
                            GUI_spam(fenetre,obj[10],3,"Volume 0 - 100 :",temps,NULL);
                            get_value = atoi(temps);

                            if(get_value >= 0
                                && get_value <= 100 )
                            {
                                SetINIValueInt("config.ini","PARAM_AUDIO/musique",get_value);
                                reset_values = 1;
                            }
                            else
                            {
                                GUI_spam(fenetre,obj[10],1,"Mauvaise valeur",NULL,NULL);
                            }

                            etat[0].etat[0] = 0;
                        }
                        else if(etat[1].etat[0] == 1) // Modif volume effets
                        {
                            GUI_spam(fenetre,obj[10],3,"Volume 0 - 100 :",temps,NULL);
                            get_value = atoi(temps);

                            if(get_value >= 0
                                && get_value <= 100 )
                            {
                                SetINIValueInt("config.ini","PARAM_AUDIO/effets",get_value);
                                reset_values = 1;
                            }
                            else
                            {
                                GUI_spam(fenetre,obj[10],1,"Mauvaise valeur",NULL,NULL);
                            }

                            etat[1].etat[0] = 0;
                        }
                        else if (etat[2].etat[0] == 1) // Modif résolution
                        {
                            int get_ui = 0;
                            get_ui  = GUI_spam(fenetre,resolution[7],4,"Selectionner la résolution :",NULL,resolution);

                            if(get_ui >= 0)
                            {
                                printf("%d\n", get_ui);
                                switch(get_ui)
                                {
                                    // On apporte les modifications des valeurs en fonction de la résolution choisit
                                    case 0 :

                                        SetINIValueInt("config.ini","PARAM_FENETRE/nb_blocs_largeur",25);
                                        SetINIValueInt("config.ini","PARAM_FENETRE/nb_blocs_hauteur",19);
                                        break;
                                    case 1 :

                                        SetINIValueInt("config.ini","PARAM_FENETRE/nb_blocs_largeur",32);
                                        SetINIValueInt("config.ini","PARAM_FENETRE/nb_blocs_hauteur",24);
                                        break;
                                    case 2 :

                                        SetINIValueInt("config.ini","PARAM_FENETRE/nb_blocs_largeur",40);
                                        SetINIValueInt("config.ini","PARAM_FENETRE/nb_blocs_hauteur",32);
                                        break;
                                    case 3 :

                                        SetINIValueInt("config.ini","PARAM_FENETRE/nb_blocs_largeur",45);
                                        SetINIValueInt("config.ini","PARAM_FENETRE/nb_blocs_hauteur",25);
                                        break;
                                    case 4 :

                                        SetINIValueInt("config.ini","PARAM_FENETRE/nb_blocs_largeur",52);
                                        SetINIValueInt("config.ini","PARAM_FENETRE/nb_blocs_hauteur",33);
                                        break;
                                    case 5 :

                                        SetINIValueInt("config.ini","PARAM_FENETRE/nb_blocs_largeur",60);
                                        SetINIValueInt("config.ini","PARAM_FENETRE/nb_blocs_hauteur",34);
                                        break;
                                    case 6 :
                                        SDL_GetWindowSize(fenetre,&b_x,&b_y);

                                        SDL_SetWindowFullscreen(fenetre,SDL_WINDOW_FULLSCREEN_DESKTOP);

                                        SDL_GetWindowSize(fenetre,&a_x,&a_y);

                                        SDL_SetWindowFullscreen(fenetre,0);

                                        SDL_SetWindowSize(fenetre,b_x,b_y);

                                        SetINIValueInt("config.ini","PARAM_FENETRE/nb_blocs_largeur",a_x/32);
                                        SetINIValueInt("config.ini","PARAM_FENETRE/nb_blocs_hauteur",a_y/32-1);

                                        quit=1;

                                        break;
                                }
                                SetINIValueInt("config.ini","PARAM_FENETRE/fullscreen",0);
                                reset_values = 1;
                            }

                            etat[2].etat[0] = 0;
                        }
                        else if (etat[3].etat[0] == 1) // Modif plein ecran ou non
                        {
                            GUI_spam(fenetre,obj[10],3,"Oui : 1 / Non : 0",temps,NULL);
                            get_value = atoi(temps);

                            if(get_value >= 0
                                && get_value <= 1 )
                            {
                                SetINIValueInt("config.ini","PARAM_FENETRE/fullscreen",get_value);
                                reset_values = 1;
                            }
                            else
                            {
                                GUI_spam(fenetre,obj[10],1,"Mauvaise valeur",NULL,NULL);
                            }

                            etat[3].etat[0] = 0;
                        }

                        INIFile ini_fichier2 = ParseINI("config.ini");

                        // On récupère les variables du fichier de config
                        fullscreen  = GetINIValueInt(ini_fichier2,"PARAM_FENETRE/fullscreen",0);
                        vol_musique = GetINIValueInt(ini_fichier2,"PARAM_AUDIO/musique",0);
                        vol_effets  = GetINIValueInt(ini_fichier2,"PARAM_AUDIO/effets",0);
                        ecran_x     = GetINIValueInt(ini_fichier2,"PARAM_FENETRE/nb_blocs_largeur",30);
                        ecran_y     = GetINIValueInt(ini_fichier2,"PARAM_FENETRE/nb_blocs_hauteur",23);

                        FreeINI(ini_fichier2);

                        position.y = TOP_MENU_Y + 20;

                        position_menu.x = ecran->w/2-menu->w/2;
                        position_menu.y = 0;
                        position_menu.w = menu->w;
                        position_menu.h = menu->h;

                        // On affiche les images de fond
                        SDL_BlitSurface(menu,NULL,ecran,&position_menu);
                        SDL_BlitSurface(back,NULL,ecran,&position_back);

                        // On affiche les différentes variables avec les boutons
                        sprintf(temps,"Volume de la musique : %d ",vol_musique);
                        render = TTF_RenderText_Blended(police, temps, couleur);
                        SDL_BlitSurface(render, NULL, ecran, &position);
                        SDL_FreeSurface(render);

                        position.y += 50;

                        sprintf(temps,"Volume des effets : %d ",vol_effets);
                        render = TTF_RenderText_Blended(police, temps, couleur);
                        SDL_BlitSurface(render, NULL, ecran, &position);
                        SDL_FreeSurface(render);

                        position.y += 50;

                        sprintf(temps,"Résolution écran : %d x %d ",ecran_x*32,ecran_y*32);
                        render = TTF_RenderText_Blended(police, temps, couleur);
                        SDL_BlitSurface(render, NULL, ecran, &position);
                        SDL_FreeSurface(render);

                        position.y += 50;

                        sprintf(temps,"Plein écran : %d ",fullscreen);
                        render = TTF_RenderText_Blended(police, temps, couleur);
                        SDL_BlitSurface(render, NULL, ecran, &position);
                        SDL_FreeSurface(render);

                        GUI_DISPLAY_button(obj[0],ecran,"Modifier volume musique");
                        GUI_DISPLAY_button(obj[1],ecran,"Modifier volume effets");
                        GUI_DISPLAY_button(obj[2],ecran,"Modifier la résolution");
                        GUI_DISPLAY_button(obj[3],ecran,"Modifier mode");
                    }
                }
            }
        }

        SDL_UpdateWindowSurface(fenetre);

        if(1000/IMAGE_PAR_SECONDES > SDL_GetTicks()-start)
        {
            SDL_Delay(1000/IMAGE_PAR_SECONDES-(SDL_GetTicks()-start));
        }
    }

    SDL_FreeSurface(ecran);
    SDL_FreeSurface(menu);
    SDL_FreeSurface(back);

    return reset_values;
}
