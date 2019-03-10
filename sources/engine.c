/* Nom : engine.c

Dernier Auteur  : Armand

Creation :
05/02/2014

Derni�re modification :
22/02/2014

Description :
    Fichier qui contient toutes les fonctions relatives au moteur graphique du jeux

*///

#include "../headers/linker.h"

int handler(void* conf, const char* section, const char* name, const char* value)
{
    Configuration* pconfig = (Configuration*)conf;

    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0

    if (MATCH("PARAM_FENETRE", "largeur_tile")) {
        pconfig->largeur_tile = atoi(value);
    } else if (MATCH("PARAM_FENETRE", "hauteur_tile")) {
        pconfig->hauteur_tile = atoi(value);
    } else if (MATCH("PARAM_FENETRE", "nb_blocs_largeur")) {
        pconfig->nb_blocs_largeur = atoi(value);
    } else if (MATCH("PARAM_FENETRE", "nb_blocs_hauteur")) {
        pconfig->nb_blocs_hauteur = atoi(value);
    } else if (MATCH("PARAM_FENETRE", "largeur_fenetre")) {
        pconfig->largeur_fenetre = atoi(value);
    } else if (MATCH("PARAM_FENETRE", "hauteur_fenetre")) {
        pconfig->hauteur_fenetre = atoi(value);
    } else if (MATCH("PARAM_FENETRE", "fullscreen")) {
        pconfig->fullscreen = atoi(value);
    } else if (MATCH("PARAM_GRAPHIC_ENGINE", "image_par_secondes")) {
        pconfig->image_par_secondes = atoi(value);
    } else if (MATCH("PARAM_GRAPHIC_ENGINE", "taille_map_x")) {
        pconfig->taille_map_x = atoi(value);
    } else if (MATCH("PARAM_GRAPHIC_ENGINE", "taille_map_y")) {
        pconfig->taille_map_y = atoi(value);
    } else if (MATCH("PARAM_GRAPHIC_ENGINE", "marge_deplacement_x")) {
        pconfig->marge_deplacement_x = atoi(value);
    } else if (MATCH("PARAM_GRAPHIC_ENGINE", "marge_deplacement_y")) {
        pconfig->marge_deplacement_y = atoi(value);
    } else if (MATCH("PARAM_PHYSIC_ENGINE", "gravity")) {
        pconfig->gravity = atoi(value);
    } else if (MATCH("PARAM_PHYSIC_ENGINE", "vitesse_de_chute_max")) {
        pconfig->vitesse_de_chute_max = atoi(value);
    } else if (MATCH("PARAM_MENU", "nbr_bouton")) {
        pconfig->nbr_bouton = atoi(value);
    } else if (MATCH("PARAM_MENU", "top_menu_y")) {
        pconfig->top_menu_y = atoi(value);
    } else if (MATCH("PARAM_GUI", "separateur")) {
        pconfig->separateur = atoi(value);
    } else if (MATCH("PARAM_AUDIO", "musique")) {
        pconfig->musique = atoi(value);
    } else if (MATCH("PARAM_AUDIO", "effets")) {
        pconfig->effets = atoi(value);
    } else {
        return 0;  /* unknown section/name, error */
    }

    return 1;
}

// Fonction qui permet d'�diter en brut la map
int MAP_EDITOR_loader(SDL_Window *window,char *nom_de_la_map2,char *musique,char *background,int creation)
{
    /*-------------------------------------------------------------------------------------------------------------------------------------------------------------*/
                                                                                 /*                                                                                */
        int exit_value = 0;                                                      /* Valeur de retour pour tester l'�tat final du moteur graphique                  */
        int leave = 0;                                                           /* variable qui met fin � la while                                                */
        int etat_curseur = 0;                                                    /* variable qui d�fini si on �dite ou pas                                         */
        int current_tiles_cursor_int = 0;                                        /* le type de la tile courante                                                    */
        int x,y;                                                                 /* les coordonn�es de l'�cran sur la map                                          */
        int tile_select_cursor = 0;                                              /* variable qui compte la position du type de tile selectionn� courant.           */
                                                                                 /*                                                                                */
        struct Map map;                                                          /* La map                                                                         */
        struct Coordonnees position_absolue;                                     /* la variable qui contient la position absolue de la fenetre par rapport � la map*/
        struct Coordonnees grille;                                               /* la variable qui contient les dimensions de la fenetre en tiles                 */
        struct Coordonnees taille_fenetre;                                       /* Variables qui contient la dimension en nombres de tiles de la taille_fenetre   */
                                                                                 /*                                                                                */
        char nom_de_la_map[100];                                                 /* le nom de la map                                                               */
                                                                                 /*                                                                                */
        SDL_Surface* ecran =  SDL_GetWindowSurface(window);                      /* l'ecran                                                                        */
        SDL_Surface* current_tiles_cursor;                                       /* l'image de la tile courante                                                    */
        SDL_Surface* curseur_tiles_png = IMG_Load("tiles/curseur.png");          /* l'image du curseur                                                             */
        SDL_Surface* curseur_tiles_png_edit = IMG_Load("tiles/curseur_edit.png");/* l'image du curseur en mode �dition                                             */
                                                                                 /*                                                                                */
        SDL_Event event;                                                         /* gestion des events                                                             */
                                                                                 /*                                                                                */
        SDL_Rect positionCurrentTilesSelected,pos_curseur;                       /* les positions du curseur et de la tile                                         */
                                                                                 /*                                                                                */
        SDL_Surface* tiles[100];                                                 /* le tableau de tiles                                                            */
                                                                                 /*                                                                                */
        Uint32 start;                                                            /* le timer                                                                       */
                                                                                 /*                                                                                */
    /*-------------------------------------------------------------------------------------------------------------------------------------------------------------*/

    ENGINE_INIT_console_engine();

    /////////////// RECUPERATION DES DONNEES STOCKEES ///////////////

    sprintf(nom_de_la_map,"map/%s",nom_de_la_map2);

    // R�cup�ration des donn�es stock�es dans les fichiers
    printf("\tChargement de la map : %s\n",nom_de_la_map);

    // on charge la map
    exit_value = ENGINE_Loader(nom_de_la_map,&map,tiles);

    // Si on cr�er la map pour la premi�re fois, alors on initialise les variables par d�fauts
    if (creation == 1)
    {
        sprintf(map.musique,"musique/%s",musique);
        sprintf(map.background,"map/%s",background);
        map.start.x = 0;
        map.start.y = 0;
    }

    // On charge les tiles sp�ciales
    tiles[98] = IMG_Load("tiles/startmarker.png");
    tiles[99] = IMG_Load("tiles/stopmarker.png");

    if(exit_value == 0 || leave == 0)
    {

        taille_fenetre = ENGINE_POCESS_nombresDeTiles(window);

        positionCurrentTilesSelected.x = 10;
        positionCurrentTilesSelected.y = 10;
        current_tiles_cursor = tiles[0];

        // On initialise la grille � 0 et la position absolue de la fenetre � 0
        grille.x = 0;
        grille.y = 0;

        position_absolue.x = map.start.x-taille_fenetre.x/2;
        position_absolue.y = map.start.y-taille_fenetre.y/2;

        //////////////////// LANCEMENT DE LA BOUCLE  //////////////

        printf("\n\tCreation de l'environnement en dimension : %d par %d tiles.\n\n",taille_fenetre.x,taille_fenetre.y);

        printf("Fonctionnement...\n");

        ENGINE_SET_tramage(window);

        taille_fenetre = ENGINE_POCESS_nombresDeTiles(window); // On calcule la dimension en nombre de tiles de la fenetre
        ecran = ENGINE_SET_map(&map,ecran,0,0,tiles,0,taille_fenetre);
        SDL_BlitSurface(current_tiles_cursor, NULL, ecran, &positionCurrentTilesSelected);

        SDL_ShowCursor(0); // On desactive le curseur

        while(leave == 0)
        {
            start = SDL_GetTicks(); // on r�cup�re le temps
            // GESTIONNAIRE D EVENEMENTS
            while (SDL_PollEvent(&event)) // R�cup�ration des actions de l'utilisateur
            {
                switch(event.type)
                {
                    case SDL_KEYDOWN: // Rel�chement d'une touche
                        if(event.key.keysym.sym == SDLK_ESCAPE
                        && etat_curseur == 0)
                        {
                            leave=1;
                        }

                            switch(event.key.keysym.sym)
                            {
                                case SDLK_1: // Tile sp�cial : point de d�part
                                    current_tiles_cursor = tiles[98];
                                    current_tiles_cursor_int = 1;
                                    break;
                                case SDLK_2: // Tile sp�cial : point d'arriv�e
                                    current_tiles_cursor = tiles[99];
                                    current_tiles_cursor_int = 2;
                                    break;
                                case SDLK_3:
                                    current_tiles_cursor = tiles[2];
                                    current_tiles_cursor_int = 2;
                                    break;
                                case SDLK_4:
                                    current_tiles_cursor = tiles[3];
                                    current_tiles_cursor_int = 3;
                                    break;
                                case SDLK_5:
                                    current_tiles_cursor = tiles[4];
                                    current_tiles_cursor_int = 4;
                                    break;
                                case SDLK_z:
                                        grille.y--;
                                        if (grille.y == -1)
                                        {
                                            grille.y = 0;
                                        }
                                    break;
                                case SDLK_q:
                                        grille.x--;
                                        if (grille.x == -1)
                                        {
                                            grille.x = 0;
                                        }
                                    break;
                                case SDLK_s:
                                        grille.y++;
                                        if(grille.y == taille_fenetre.y)
                                        {
                                            grille.y = taille_fenetre.y-1;
                                        }
                                    break;
                                case SDLK_d:
                                        grille.x++;
                                        if(grille.x == taille_fenetre.x)
                                        {
                                            grille.x = taille_fenetre.x-1;
                                        }
                                    break;
                                case SDLK_RETURN:
                                    if(etat_curseur == 0)
                                    {
                                        etat_curseur = 1;
                                    }
                                    else
                                    {
                                        etat_curseur = 0;
                                    }
                                    break;
                                case SDLK_UP:
                                        position_absolue.y--;
                                        if (position_absolue.y == -1)
                                        {
                                            position_absolue.y = 0;
                                        }
                                    break;
                                case SDLK_DOWN:
                                        position_absolue.y++;
                                        if(position_absolue.y == (TAILLE_MAP_Y - taille_fenetre.y))
                                        {
                                            position_absolue.y = TAILLE_MAP_Y - 1 - taille_fenetre.y;
                                        }
                                    break;
                                case SDLK_LEFT:
                                        position_absolue.x--;
                                        if (position_absolue.x == -1)
                                        {
                                            position_absolue.x = 0;
                                        }
                                    break;
                                case SDLK_RIGHT:
                                        position_absolue.x++;
                                        if(position_absolue.x == (TAILLE_MAP_X - taille_fenetre.x))
                                        {
                                            position_absolue.x = TAILLE_MAP_X - 1 - taille_fenetre.x;
                                        }
                                    break;
                                case SDLK_ESCAPE:
                                // On sauvegarde la map
                                    break;
                            }

                        break;

                    case SDL_MOUSEWHEEL :
                    {
                        tile_select_cursor += event.wheel.y;
                        if (tile_select_cursor < 0)
                        {
                            tile_select_cursor = 0;
                        }
                        current_tiles_cursor = tiles[tile_select_cursor];
                        current_tiles_cursor_int = tile_select_cursor;
                    }

                    case SDL_MOUSEBUTTONUP: // relachement de la souris
                    {
                        if (event.button.button == SDL_BUTTON_LEFT)
                        {
                            if(etat_curseur == 0)
                            {
                                etat_curseur = 1;
                            }
                            else
                            {
                                etat_curseur = 0;
                            }
                        }
                    }

                    case SDL_MOUSEBUTTONDOWN: // clic de la souris
                    {
                        if (event.button.button == SDL_BUTTON_LEFT)
                        {
                            if(etat_curseur == 0)
                            {
                                etat_curseur = 1;
                            }
                            else
                            {
                                etat_curseur = 0;
                            }
                        }
                    }

                    case SDL_MOUSEMOTION: // Mouvement de la souris
                    {
                        SDL_GetMouseState(&x, &y);
                        grille.y =  (y - y%HAUTEUR_TILE)/HAUTEUR_TILE;
                        grille.x =  (x - x%LARGEUR_TILE)/LARGEUR_TILE;
                    }
                }
            }

            ecran = ENGINE_SET_map(&map,ecran,position_absolue.x,position_absolue.y,tiles,0,taille_fenetre);
            pos_curseur.y = grille.y*32;
            pos_curseur.x = grille.x*32;

            SDL_BlitSurface(current_tiles_cursor, NULL, ecran, &pos_curseur);
            SDL_BlitSurface(current_tiles_cursor, NULL, ecran, &positionCurrentTilesSelected);

            if(etat_curseur == 1)
            {
                map.tiles_T[grille.x+position_absolue.x][grille.y+position_absolue.y].type = current_tiles_cursor_int;
                printf("%d x %d type : %d \n",grille.y,grille.x,current_tiles_cursor_int);
                if (current_tiles_cursor_int == 1)
                {
                    map.start.x = grille.x+position_absolue.x;
                    map.start.y = grille.y+position_absolue.y;

                    etat_curseur = 0;
                }

                SDL_BlitSurface(curseur_tiles_png_edit, NULL, ecran, &pos_curseur);
            }
            else
            {
                SDL_BlitSurface(curseur_tiles_png, NULL, ecran, &pos_curseur);
            }

            SDL_UpdateWindowSurface(window);

            if(1000/IMAGE_PAR_SECONDES > SDL_GetTicks()-start)
            {
                SDL_Delay(1000/IMAGE_PAR_SECONDES-(SDL_GetTicks()-start));
            }
        }

        SDL_ShowCursor(1);
    }
    else
    {
        printf("\n/////////////////////////////////////////////////////////////////////////////");
        printf("\n/// EDITEUR : Erreur de chargement des donnees stocker dans les fichiers  ///");
        printf("\n/////////////////////////////////////////////////////////////////////////////\n");
    }

    printf("\n\nStopping...\n\n");
    printf("\tremove SDL proprieties\n");
    // destruction des objets; sauvegarde temporaire des etats etc....
        FILE* flux3;

        flux3 = fopen(nom_de_la_map,"wb");

        if (flux3 == NULL)
        {
            printf ("// Erreur a l'ouverture du fichier de specification des tiles\n");
            exit_value = 1;
            leave = 1;
        }
        else
        {
            printf("%d:%d\n", map.start.x,map.start.y);
            strcpy(map.nom,nom_de_la_map);
            fwrite(&map,sizeof(struct Map),1,flux3);
            printf("\tSauvegarde de la map \n");
        }
        
        fclose(flux3);

    SDL_FreeSurface(ecran);
    SDL_FreeSurface(current_tiles_cursor);
    SDL_FreeSurface(curseur_tiles_png);

    printf("\n\n******************       Exit        ******************\n");

    return exit_value;
}

// Fonction qui r�cup�re lesdiff�rentes variables des tiles
int get_data_tiles(struct tiles_spec *table)
{
    FILE* flux = fopen("map/tiles_config.slapp","rb");
    if(flux == NULL)
    {
        printf("Erreur lors de l'ouverture du fichier");
        FILE* flux2 = fopen("map/tiles_config.slapp","ab");
        if (flux2 == NULL)
        {
            printf("Echec de tentative de creation du fichier, probleme au niveau de l'acces en lecture ecriture sur le support materiel.");
        }
        else
        {
            printf("fichier creer a defaut d'avoir pus etre ouvert.");
        }
        fclose(flux2);
    }
    int i = 0;
    struct tiles_spec temp;
    while(fread(&temp,sizeof(struct tiles_spec),1,flux) && !feof(flux))
    {
        table[i] = temp;
        ++i;
    }

    fclose(flux);

    return i;
}

void afficher_table(SDL_Window *window,struct tiles_spec *table,int nombre,int start)
{
    int i = 0;
    char temps[200] = "";

    SDL_Color blanc = {255, 255, 255};

    SDL_Surface *render;
    SDL_Surface *fenetre;
    SDL_Surface *image_tile;

    SDL_Rect position;
    SDL_Rect step1; // fond du tableau
    SDL_Rect pos_tile;

    TTF_Font *police = TTF_OpenFont("fonts/Roboto-Medium.ttf", 12);

    fenetre = SDL_GetWindowSurface(window);

    pos_tile.w = 32;
    pos_tile.h = 32;

    step1.x = 0;
    step1.y = 73;
    step1.w = fenetre->w;
    step1.h = fenetre->h - step1.y;

    SDL_FillRect(fenetre,&step1,SDL_MapRGB(fenetre->format, 110, 110, 110));
    step1.y = 74;
    SDL_FillRect(fenetre,&step1,SDL_MapRGB(fenetre->format, 30, 30, 30));

    for (i = start; i < nombre; ++i)
    {
        sprintf(temps,"Type : %d       Passable : %d       Plan : %d       Abs : %d    Image : \"%s\"",table[i].type,table[i].passable,table[i].plan,table[i].abs,table[i].image);
        render = TTF_RenderText_Blended(police, temps, blanc);
        position.x = 75;
        position.y = 87 + 32*i + 5 - 32*start;
        SDL_BlitSurface(render, NULL, fenetre, &position);
        SDL_FreeSurface(render);

        sprintf(temps,"tiles/%s",table[i].image);
        image_tile = IMG_Load(temps);

        pos_tile.x = 20;
        pos_tile.y = 80 + 32*i + 5 - 32*start;

        SDL_BlitSurface(image_tile,NULL,fenetre,&pos_tile);
        SDL_FreeSurface(image_tile);
    }
}

void save_tab(struct tiles_spec *table,int nombre)
{
    FILE* flux = fopen("map/tiles_config.slapp","wb");
    if(flux == NULL)
    {
        printf("Erreur lors de l'ouverture du fichier");
        FILE* flux2 = fopen("map/tiles_config.slapp","wb");
        if (flux2 == NULL)
        {
            printf("Echec de tentative de creation du fichier, probleme au niveau de l'acces en lecture ecriture sur le support materiel.");
        }
        else
        {
            printf("fichier creer a defaut d'avoir pus etre ouvert.");
        }
        fclose(flux2);
    }

    int i = 0;
    for (i = 0; i < nombre; ++i)
    {
        fwrite(&table[i],sizeof(struct tiles_spec),1,flux);
    }

    fclose(flux);
}

int ajouter_tile(SDL_Window *window)
{
    struct tiles_spec k;
    struct GUI_object obj[1];
    struct tiles_spec table[250];

    char temp_string[60];

    int erreur = 0;
    int nombre = 0;
    int add = 0;
    int i,j;

    SDL_Surface *ecran = SDL_GetWindowSurface(window);

    obj[0].largeur = 250;
    obj[0].hauteur = 100;
    obj[0].pos_x = ecran->w/2 - obj[0].largeur/2;
    obj[0].pos_y = ecran->h/2 - obj[0].hauteur/2;
    obj[0].id = 0;

    nombre = get_data_tiles(table); // on r�cup�re les tiles

    if (nombre == 0)
    {
        table[nombre].type = 0;
    }
    else
    {
        table[nombre].type = nombre;
    }

    if(GUI_spam(window,obj[0],3,"Passable 1 , non passable 0 :",temp_string,NULL) > 0)
    {
        table[nombre].passable = atoi(temp_string);

        if(GUI_spam(window,obj[0],3,"Plan :",temp_string,NULL) == 1)
        {
            table[nombre].plan = atoi(temp_string);

            if(GUI_spam(window,obj[0],3,"Taux d'absorption :",temp_string,NULL) > 0)
            {
                table[nombre].abs = atoi(temp_string);
                if(GUI_spam(window,obj[0],3,"Nom de l'image de la tile :",table[nombre].image,NULL) > 0)
                {
                    nombre = nombre + 1;
                    // On tri par valeur de type de tile par ordre croissant
                    for (j=nombre;j>0;j--)
                    {
                        for (i=0;i<j-1;i++)
                        {
                            if (table[i].type>table[i+1].type)
                            {
                                k=table[i];
                                table[i]=table[i+1];
                                table[i+1]=k;
                            }
                        }
                    }

                    save_tab(table,nombre);
                }
                else
                {
                    printf("// Erreur de recuperation de la variable binaire \n");
                    erreur = 1;
                    add = 1;
                }
            }
            else
            {
                printf("// Erreur de recuperation de la variable binaire \n");
                erreur = 1;
                add = 1;
            }
        }
        else
        {
            printf("// Erreur de recuperation de la variable binaire \n");
            erreur = 1;
            add = 1;
        }
    }
    else
    {
        printf("// Erreur de recuperation de la variable binaire \n");
        erreur = 1;
        add = 1;
    }

    if (add == 1)
    {
        GUI_spam(window,obj[0],1,"Ajout interrompus",NULL,NULL);
    }

    SDL_FreeSurface(ecran);

    return erreur;
}

int modifier_tile(SDL_Window *window)
{
    struct tiles_spec new_tile;
    struct tiles_spec table[250];
    struct GUI_object obj[1];

    char temp_string[60];

    int erreur = 0;
    int nombre = 0;
    int i = 0;
    int edit = 0;

    SDL_Surface *ecran = SDL_GetWindowSurface(window);

    obj[0].largeur = 250;
    obj[0].hauteur = 100;
    obj[0].pos_x = ecran->w/2 - obj[0].largeur/2;
    obj[0].pos_y = ecran->h/2 - obj[0].hauteur/2;
    obj[0].id = 0;

    // On r�cup�re le type de tiles
    if(GUI_spam(window,obj[0],3,"Modifier : Type de tiles :",temp_string,NULL) > 0)
    {
        new_tile.type = atoi(temp_string);

        nombre = get_data_tiles(table); // On r�cup�re le nombre de tiles enregistr�s
        for (i = 0; i < nombre; ++i)
        {
            if (table[i].type == new_tile.type) // on atteint la tile en question
            {
                if(GUI_spam(window,obj[0],3,"Passable 1 , non passable 0 :",temp_string,NULL) > 0)
                {
                    table[i].passable = atoi(temp_string);

                    if(GUI_spam(window,obj[0],3,"Plan :",temp_string,NULL) == 1)
                    {
                        table[i].plan = atoi(temp_string);

                        if(GUI_spam(window,obj[0],3,"Taux absorption :",temp_string,NULL) > 0)
                        {
                            table[i].abs = atoi(temp_string);
                            if(GUI_spam(window,obj[0],3,"Nom de l'image de la tile :",table[i].image,NULL) > 0)
                            {
                                edit = 1;
                            }
                            else
                            {
                                printf("// Erreur de recuperation de la variable binaire \n");
                                erreur = 1;
                                edit = 2;
                            }
                        }
                        else
                        {
                            printf("// Erreur de recuperation de la variable binaire \n");
                            erreur = 1;
                            edit = 2;
                        }
                    }
                    else
                    {
                        printf("// Erreur de recuperation de la variable binaire \n");
                        erreur = 1;
                        edit = 2;
                    }
                }
                else
                {
                    printf("// Erreur de recuperation de la variable binaire \n");
                    erreur = 1;
                    edit = 2;
                }
            }
        }

        if (edit == 1)
        {
            save_tab(table,nombre);
        }
        else if(edit == 2)
        {
            GUI_spam(window,obj[0],1,"Modification interrompus",NULL,NULL);
        }
        else
        {
            GUI_spam(window,obj[0],1,"Tiles inexistante",NULL,NULL);
        }
    }
    else
    {
        printf("// Erreur de recuperation du type de tiles\n");
        erreur = 1;
    }

    SDL_FreeSurface(ecran);
    return erreur;
}

int supprimer_tile(SDL_Window *window)
{
    struct tiles_spec new_tile;
    struct tiles_spec table[250];
    struct GUI_object obj[1];

    char temp_string[60];

    int erreur = 0;
    int nombre = 0;
    int i = 0;
    int j = 0;
    int edit = 0;

    SDL_Surface *ecran = SDL_GetWindowSurface(window);

    obj[0].largeur = 250;
    obj[0].hauteur = 100;
    obj[0].pos_x = ecran->w/2 - obj[0].largeur/2;
    obj[0].pos_y = ecran->h/2 - obj[0].hauteur/2;
    obj[0].id = 0;

    // On r�cup�re le type de tiles
    if(GUI_spam(window,obj[0],3,"Supprimer : Type de tiles :",temp_string,NULL) > 0)
    {
        new_tile.type = atoi(temp_string);

        nombre = get_data_tiles(table); // On r�cup�re le nombre de tiles enregistr�s
        for (i = 0; i < nombre; ++i)
        {
            if (table[i].type == new_tile.type) // on atteint la tile en question
            {
                for (j = i; j < nombre; ++j)
                {
                    table[j].type = table[j+1].type;
                    table[j].passable = table[j+1].passable;
                    table[j].plan = table[j+1].plan;
                    table[j].abs = table[j+1].abs;
                    strcpy(table[j].image,table[j+1].image);

                    edit = 1;
                }
            }
        }

        if (edit == 1)
        {
            save_tab(table,--nombre);
        }
        else
        {
            GUI_spam(window,obj[0],1,"Tiles inexistante",NULL,NULL);
        }
    }
    else
    {
        printf("// Erreur de recuperation du type de tiles\n");
        erreur = 1;
    }

    SDL_FreeSurface(ecran);
    return erreur;
}

// Fonction qui permet de g�rer les tiles
int MAP_EDITOR_gestionnaire_de_tiles(SDL_Window *window)
{
    int return_value = 0;
    int quit = 0;
    int refresh_load_file = 0; // 0 on ne recharge pas le fichier  :  1 on recharge le fichier
    int nombre = 0;
    int tile_select_cursor = 0;

    struct GUI_object btn[20];
    struct state etat_btn[20];

    struct tiles_spec table[250];

    SDL_Surface *ecran = SDL_GetWindowSurface(window);

    SDL_Event event;
    Uint32 start;

    btn[0].largeur = 150;
    btn[0].hauteur = 24;
    btn[0].pos_x = 30;
    btn[0].pos_y = 50;
    btn[0].id = 0;

    btn[2].largeur = 150;
    btn[2].hauteur = 24;
    btn[2].pos_x = 30*2 + 150 + 5;
    btn[2].pos_y = 50;
    btn[2].id = 0;

    btn[3].largeur = 150;
    btn[3].hauteur = 24;
    btn[3].pos_x = 30*3 + 150*2 + 5;
    btn[3].pos_y = 50;
    btn[3].id = 0;

    btn[1].largeur = 250;
    btn[1].hauteur = 100;
    btn[1].pos_x = ecran->w/2 - btn[1].largeur/2;
    btn[1].pos_y = ecran->h/2 - btn[1].hauteur/2;
    btn[1].id = 1;

    nombre = get_data_tiles(table); // On r�cup�re le nombre de tiles enregistr�s
    afficher_table(window,table,nombre,0); // On affiche les donn�es brutes

    GUI_DISPLAY_button(btn[0],ecran,"Ajouter une tiles");
    GUI_DISPLAY_button(btn[2],ecran,"Modifier une tiles");
    GUI_DISPLAY_button(btn[3],ecran,"Supprimer une tiles");

    SDL_UpdateWindowSurface(window);

    while(quit == 0)
    {
        start = SDL_GetTicks();

        if (refresh_load_file == 1) // Si on a ajout� une tile, on recharge le fichier
        {
            nombre = get_data_tiles(table); // On r�cup�re le nombre de tiles enregistr�s
            afficher_table(window,table,nombre,tile_select_cursor); // On affiche les donn�es brutes
            refresh_load_file = 0;
        }

        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {

                case SDL_QUIT: // Clic sur la croix
                {
                    quit = 1;
                }

                case SDL_MOUSEWHEEL :
                {
                    tile_select_cursor += event.wheel.y;

                    if (tile_select_cursor < 0)
                    {
                        tile_select_cursor = 0;
                    }
                    refresh_load_file = 1;
                }

                case SDL_KEYDOWN: // Rel�chement d'une touche
                {
                    if(event.key.keysym.sym == SDLK_ESCAPE)
                    {
                        quit = 1;
                    }
                }

                case SDL_MOUSEMOTION: // Mouvement de la souris
                {
                    etat_btn[0] = GUI_DISPLAY_button(btn[0],ecran,"Ajouter une tiles");
                    etat_btn[2] = GUI_DISPLAY_button(btn[2],ecran,"Modifier une tiles");
                    etat_btn[3] = GUI_DISPLAY_button(btn[3],ecran,"Supprimer une tiles");
                }


                case SDL_MOUSEBUTTONDOWN: // clic de la souris
                {
                    if (event.button.button == SDL_BUTTON_LEFT)
                    {
                        if(etat_btn[0].etat[0] == 1) // Nouvelle tile
                        {
                            ajouter_tile(window);
                            refresh_load_file = 1;
                        }
                        else if(etat_btn[2].etat[0] == 1) // Modifier une tiles
                        {
                            modifier_tile(window);
                            refresh_load_file = 1;
                        }
                        else if(etat_btn[3].etat[0] == 1) // Supprimer une tiles
                        {
                            supprimer_tile(window);
                            refresh_load_file = 1;
                        }
                    }
                }
            }
        }

        SDL_UpdateWindowSurface(window);
        if(1000/IMAGE_PAR_SECONDES > SDL_GetTicks()-start)
        {
            SDL_Delay(1000/IMAGE_PAR_SECONDES-(SDL_GetTicks()-start));
        }
    }

    save_tab(table,nombre);

    return return_value;
}

// Fonction GLOBAL de chargement de l'editeur de map
int GLOBAL_MOTEUR_EDITEUR(SDL_Window *window)
{

    /*-----------------------------------------------------------------------------------------*/

        int return_value = 0;
        int quit =0;
        int start;
        int i = 0;
        int curseur = 0;

        char nom_de_la_map[50];
        char musique[100];
        char background[100];

        char temps[200];

        struct dirent *lecture;
        struct GUI_object obj[30];
        struct GUI_object map[30];
        struct state etat[10];

        SDL_Surface *render;
        SDL_Surface *ecran = SDL_GetWindowSurface(window);

        SDL_Rect position;

        SDL_Color blanc = {200, 200, 200};

        SDL_Event event;

        TTF_Font *comforta = TTF_OpenFont("fonts/Roboto-Medium.ttf", 70);

        DIR *rep;

    /*-----------------------------------------------------------------------------------------*/

    sprintf(temps, "Sl�pp Map Editor v 2.1");

    render = TTF_RenderText_Blended(comforta, temps, blanc);

    position.x = ecran->w/2 - render->w/2;
    position.y = ecran->h/2 - render->h/2;

    obj[0].largeur = 150;
    obj[0].hauteur = 24;
    obj[0].pos_x = 0;
    obj[0].pos_y = 0;
    obj[0].id = 1;

    obj[1].largeur = 150;
    obj[1].hauteur = 24;
    obj[1].pos_x = 150;
    obj[1].pos_y = 0;
    obj[1].id = 2;

    obj[2].largeur = 150;
    obj[2].hauteur = 24;
    obj[2].pos_x = 300;
    obj[2].pos_y = 0;
    obj[2].id = 3;

    obj[3].largeur = 150;
    obj[3].hauteur = 24;
    obj[3].pos_x = ecran->w-150;
    obj[3].pos_y = 0;
    obj[3].id = 5;

    obj[4].largeur = 250;
    obj[4].hauteur = 100;
    obj[4].pos_x = ecran->w/2 - obj[4].largeur/2;
    obj[4].pos_y = ecran->h/2 - obj[4].hauteur/2;
    obj[4].id = 4;

    obj[5].largeur = 150;
    obj[5].hauteur = 24;
    obj[5].pos_x = 450;
    obj[5].pos_y = 0;
    obj[5].id = 3;

    SDL_FillRect(ecran,NULL,SDL_MapRGB(ecran->format, 235, 235, 235));

    SDL_BlitSurface(render, NULL, ecran, &position);

    GUI_DISPLAY_button(obj[0],ecran,"Nouvelle map");
    GUI_DISPLAY_button(obj[1],ecran,"Charger une map");
    GUI_DISPLAY_button(obj[2],ecran,"Supprimer une map");
    GUI_DISPLAY_button(obj[3],ecran,"Quitter");
    GUI_DISPLAY_button(obj[5],ecran,"Gestionnaire de tiles");

    SDL_UpdateWindowSurface(window);

    // Boucle d' evenements
    while(quit == 0)
    {
        start = SDL_GetTicks(); // on r�cup�re le temps

        SDL_WaitEvent(&event);
        while(SDL_PollEvent(&event)); // R�cup�ration des actions de l'utilisateur(while pour une bonne utilisation)
        {
            switch(event.type)
            {
                case SDL_QUIT: // Clic sur la croix
                {
                    quit=1;
                }

                case SDL_KEYDOWN: // Rel�chement d'une touche
                {
                    if(event.key.keysym.sym == SDLK_ESCAPE)
                    {
                        quit=1;
                    }
                }

                case SDL_MOUSEMOTION: // Mouvement de la souris
                {
                    SDL_FillRect(ecran,NULL,SDL_MapRGB(ecran->format, 235, 235, 235));

                    SDL_BlitSurface(render, NULL, ecran, &position);

                    etat[0] = GUI_DISPLAY_button(obj[0],ecran,"Nouvelle map");
                    etat[1] = GUI_DISPLAY_button(obj[1],ecran,"Charger une map");
                    etat[2] = GUI_DISPLAY_button(obj[2],ecran,"Supprimer une map");
                    etat[3] = GUI_DISPLAY_button(obj[3],ecran,"Quitter");
                    etat[4] = GUI_DISPLAY_button(obj[5],ecran,"Gestionnaire de tiles");
                }

                case SDL_MOUSEBUTTONDOWN: // clic de la souris
                {
                    if (event.button.button == SDL_BUTTON_LEFT)
                    {
                        if(etat[3].etat[0] == 1) // Quitter
                        {
                            quit=1;
                        }
                        else if(etat[0].etat[0] == 1) // Nouvelle map
                        {
                            if(GUI_spam(window,obj[4],3,"Entrer le nom de la map :",nom_de_la_map,NULL) == 1
                            && nom_de_la_map[0] != '\0')
                            {
                                if(GUI_spam(window,obj[4],3,"Musique associ� � la map :",musique,NULL) == 1)
                                {
                                    if(GUI_spam(window,obj[4],3,"Background de la map :",background,NULL) == 1)
                                    {
                                        MAP_EDITOR_loader(window,nom_de_la_map,musique,background,1);
                                    }
                                }
                            }
                        }
                        else if (etat[1].etat[0] == 1) // charger une map
                        {
                            rep = opendir("map/" );

                            if (!rep)
                            {
                                printf("// Erreur d'ouverture du dossier de MAP\n");
                            }

                            curseur = 0;

                            while ((lecture = readdir(rep)))
                            {
                                i = 0;
                                strcpy(temps,lecture->d_name);
                                while(temps[i] != '.')
                                {
                                    i++;
                                }

                                if (temps[i+1] == 'm'
                                && temps[i+2] == 'a'
                                && temps[i+3] == 'p')
                                {
                                    strcpy(map[curseur].text,temps);
                                    if(curseur == 0)
                                    {
                                        map[curseur].largeur = 200;
                                        map[curseur].hauteur = 24;
                                        map[curseur].pos_x = ecran->w/2 - obj[4].largeur/2;
                                        map[curseur].pos_y = ecran->h/2 - obj[4].hauteur/2;
                                        map[curseur].id = curseur;
                                    }
                                    else
                                    {
                                        map[curseur].largeur = map[curseur - 1].largeur ;
                                        map[curseur].hauteur = map[curseur - 1].hauteur ;
                                        map[curseur].pos_x = map[curseur - 1].pos_x;
                                        map[curseur].pos_y = map[curseur - 1].pos_y + map[curseur].hauteur + 3;
                                        map[curseur].id = curseur;
                                    }
                                    curseur++;
                                }
                            }
                            closedir(rep);

                            if (curseur != 0) // des maps existent
                            {
                                map[50].largeur = 250;
                                map[50].hauteur = 30;
                                map[50].pos_y = ecran->h/2 - map[50].hauteur*curseur/2;
                                map[50].pos_x = ecran->w/2 - map[50].largeur/2;
                                map[curseur].id = 999;

                                int get_ui = 0;
                                get_ui  = GUI_spam(window,map[50],4,"Selectionner la map :",NULL,map);

                                if(get_ui >= 0)
                                {
                                    MAP_EDITOR_loader(window,map[get_ui].text,NULL,NULL,0);
                                }
                            }
                            else
                            {
                                GUI_spam(window,obj[4],1,"Il n'existe aucune map pour le moment",NULL,NULL);
                            }
                        }
                        else if (etat[2].etat[0] == 1) // supprimer une map
                        {
                            if(GUI_spam(window,obj[4],3,"Entrer le nom de la map :",nom_de_la_map,NULL) == 1
                            && nom_de_la_map[0] != '\0')
                            {
                                if(GUI_spam(window,obj[4],2,"Voulez-vous vraiment supprimer la map ?",NULL,NULL) == 1)
                                {
                                    char tempdelete[100];
                                    sprintf(tempdelete,"map/%s",nom_de_la_map);
                                    remove(tempdelete);
                                }
                            }
                        }
                        else if (etat[4].etat[0] == 1) // Gestionnaire de tiles
                        {
                            MAP_EDITOR_gestionnaire_de_tiles(window);
                        }

                        SDL_FillRect(ecran,NULL,SDL_MapRGB(ecran->format, 235, 235, 235));

                        SDL_BlitSurface(render, NULL, ecran, &position);

                        GUI_DISPLAY_button(obj[0],ecran,"Nouvelle map");
                        GUI_DISPLAY_button(obj[1],ecran,"Charger une map");
                        GUI_DISPLAY_button(obj[2],ecran,"Supprimer une map");
                        GUI_DISPLAY_button(obj[3],ecran,"Quitter");
                        GUI_DISPLAY_button(obj[5],ecran,"Gestionnaire de tiles");
                    }
                }
            }
        }
        SDL_UpdateWindowSurface(window);

        if(1000/IMAGE_PAR_SECONDES > SDL_GetTicks()-start)
        {
            SDL_Delay(1000/IMAGE_PAR_SECONDES-(SDL_GetTicks()-start));
        }
    }
    ANIM_fade_out(window);
    SDL_FreeSurface(render);
    return return_value;
}

// Fonction qui affiche les specifications des tiles dans la console
void ENGINGE_LOG_afficher_table(struct tiles_spec *table,int nombre)
{
    int i = 0;
    for (i = 0; i < nombre; ++i)
    {
        printf("\tType : %d \t Passable : %d \t  Plan : %d \t Image %s\n",
            table[i].type,
            table[i].passable,
            table[i].plan,
            *&table[i].image);
    }
}

// Fonction pouvant �tre appel�e pour r�initialiser les parametres par default du moteur graphique
void ENGINE_SET_defaultConfigFile()
{
    //const float gravity = 9.81; // constante de gravit�
    //const int width = 1280; // largeur par defaut de la fenetre
    //const int height = 1024; // hauteur par d�faut de la fenetre

    FILE* flux = fopen("engine_config.slapp","wb");
    if (flux == NULL)
    {
        printf ("// Erreur a l'ouverture du fichier de configuration du moteur graphique\n");
    }
    fclose(flux);
}

// Fonction qui r�initialise la console pour afficher les infos du moteur graphique
void ENGINE_INIT_console_engine()
{
    // Initialisation de la console
    system("cls");
    printf("\n\n************************************************************* \n");
    printf("******************     VYDIA ENGINE 2.0    ****************** \n");
    printf("************************************************************* \n\n\n");

    printf("Starting...\n\n");
}

// Fonction qui renvois le nombres de tiles constructibles par rapport � la r�solution de la fen�tre courante
struct Coordonnees ENGINE_POCESS_nombresDeTiles(SDL_Window *window)
{
    struct Coordonnees recup_nrb_tiles;

    SDL_Point taille_fenetre;
    SDL_GetWindowSize(window,&taille_fenetre.x,&taille_fenetre.y); // on r�cup�re la taille de la fenetre

    recup_nrb_tiles.x = taille_fenetre.x/32; // on calcule le nombre de tiles en x
    recup_nrb_tiles.y = taille_fenetre.y/32; // on calcule le nombre de tiles en y

    return recup_nrb_tiles;
}

// Fonction qui ajuste la taille de la fenetre de mani�re � garder toujours le m�me format en l'occurence 16/9
void ENGINE_SET_tailleFenetre(const SDL_Event *event,SDL_Window *window,int priorite)
{
    if ((event->type == SDL_WINDOWEVENT
    && event->window.event == SDL_WINDOWEVENT_RESIZED)
    || priorite == 1)
    {
        int size_w = 0;
        int size_h = 0;

        if(priorite == 1)
        {
            SDL_Point taille_fenetre;
            SDL_GetWindowSize(window,&taille_fenetre.x,&taille_fenetre.y);
            size_w = taille_fenetre.x;
            size_h = taille_fenetre.y;
        }
        else
        {
            size_w = event->window.data1;
            size_h = event->window.data2;
        }

        int new_size_w = 0;
        int new_size_h = 0;

        if(size_w/size_h != 16/9
        || size_w%32 != 0
        || size_h%32 != 0)
        {
            // on augmente la hauteur pour garder le format 16/9
            new_size_w = size_w - size_w%32;
            new_size_h = (9*new_size_w/16) - (9*new_size_w/16)%32;
            if (new_size_h > size_h) // si l'ecran n'est pas assez grand en hauteur on r�duit la largeur
            {
                new_size_h = size_h;
                new_size_w = size_w;

                new_size_h = size_h - size_h%32;
                new_size_w = (16*new_size_h/9) - (16*new_size_h/9)%32;
            }

            printf("\n\n\t WINDOWS RESIZED  %d x %d \n",new_size_w,new_size_h);

            SDL_SetWindowSize(window,new_size_w,new_size_h);
        }
    }
}

// Fonction qui initialise tout les composants de la SDL
void ENGINE_SDL_loadElement()
{
    if ( SDL_Init( SDL_INIT_EVERYTHING ) < 0 ) // Initialisation des composants SDL principaux
    {
        printf("Chargement SDL : ERREUR %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Chargement SDL : OK \n");
    }

    if(TTF_Init() < 0) // Initialisation de l'API TTF
    {
        printf("Chargement SDL_TTF : ERREUR %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Chargement SDL_TTF : OK \n");
    }

    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) < 0) //Initialisation de l'API Mixer
    {
        printf("Chargement SDL_Mixer : ERREUR %s\n", Mix_GetError());
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Chargement SDL_Mixer : OK \n");
    }

    printf("************************************************************* \n\n");
}

// Fonction qui initialise la console
void ENGINE_INIT_console()
{
    // Initialisation de la console
    printf("\n************************************************************* \n");
    printf("***********************     SLAPP    ************************ \n");
    printf("************************************************************* \n\n\n");

    printf("\tPhase d'initialisation de la SDL \n\n");

    SDL_version compiled;
    SDL_version linked;
    SDL_VERSION(&compiled);
    printf("\t\tSDL Version : %d.%d.%d \n",
           compiled.major, compiled.minor, compiled.patch);
    printf("\t\t\t ID : %d.%d.%d\n\n",
           linked.major, linked.minor, linked.patch);
}

// Fonction qui contr�le l'�tat de la fenetre
void ENGINE_LOG_fenetre(const SDL_Event *event)
{
    if (event->type == SDL_WINDOWEVENT)
    {
        switch (event->window.event)
        {
            case SDL_WINDOWEVENT_SHOWN:
                printf("\nFenetre  %d shown", event->window.windowID);
                break;
            case SDL_WINDOWEVENT_HIDDEN:
                printf("\nFenetre  %d hidden", event->window.windowID);
                break;
            case SDL_WINDOWEVENT_EXPOSED:
                printf("\n\t Window %d exposed", event->window.windowID);
                break;
            case SDL_WINDOWEVENT_MOVED:
                printf("\nFenetre  %d moved to %d,%d",
                        event->window.windowID, event->window.data1,
                        event->window.data2);
                break;
            case SDL_WINDOWEVENT_RESIZED:
                printf("\n\t Window %d resized to %dx%d",
                        event->window.windowID, event->window.data1,
                        event->window.data2);
                break;
            case SDL_WINDOWEVENT_MINIMIZED:
                printf("\nFenetre  %d minimized", event->window.windowID);
                break;
            case SDL_WINDOWEVENT_MAXIMIZED:
                printf("\nFenetre  %d maximized", event->window.windowID);
                break;
            case SDL_WINDOWEVENT_RESTORED:
                printf("\nFenetre  %d restored", event->window.windowID);
                break;
            case SDL_WINDOWEVENT_ENTER:
                printf("\nFenetre entered window %d",
                        event->window.windowID);
                break;
            case SDL_WINDOWEVENT_LEAVE:
                printf("\nFenetre left window %d", event->window.windowID);
                break;
            case SDL_WINDOWEVENT_FOCUS_GAINED:
                printf("\nFenetre  %d gained keyboard focus",
                        event->window.windowID);
                break;
            case SDL_WINDOWEVENT_FOCUS_LOST:
                printf("\nFenetre  %d lost keyboard focus",
                        event->window.windowID);
                break;
            case SDL_WINDOWEVENT_CLOSE:
                printf("\nFenetre  %d closed", event->window.windowID);
                break;
            default:
                printf("\n\t Window %d got unknown event %d",
                        event->window.windowID, event->window.event);
                break;
        }
    }
}
