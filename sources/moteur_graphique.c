/* Nom : engine.c

Dernier Auteur  : Armand

Creation :
05/02/2014

Dernière modification :
22/02/2014

Description :
    Fichier qui contient toutes les fonctions relatives au moteur graphique du jeux

*///

#include "../headers/linker.h"

// Fonction qui permet d'afficher une map dans une fenetre à partir de la structure map et des coordonnées en x et y relative
void ENGINE_render_map(struct Map *map,int tiles_x,int tiles_y,SDL_Surface **tiles,struct Coordonnees taille_fenetre,SDL_Renderer *sdlRenderer)
{
    /*int i=0,j=0,type=0,u=0,v=0; // Curseur de deplacement dans le tableau bi-dimensionnel + type
    SDL_Rect Tiles_dimension;
    SDL_Surface* tiles_type;
    SDL_Texture *Tx;

    i = tiles_x;
    j = tiles_y;

    while(i <= (tiles_x + taille_fenetre.x))
    {
        j = tiles_y;
        v = 0;
        while(j <= (tiles_y + taille_fenetre.y))
        {
            Tiles_dimension.x = u * LARGEUR_TILE;
            Tiles_dimension.y = v * HAUTEUR_TILE;
            type = ; // Le type de la tiles courante dans la map

            Tx = SDL_CreateTextureFromSurface(sdlRenderer,tiles[map->tiles_T[i][j].type]); // On créer une texture associé au renderer de la fenetre
            SDL_RenderCopy(Main_Renderer, BlueShapes, &SrcR, &DestR);
            SDL_BlitSurface(tiles_type, NULL, ecran1, &Tiles_dimension);
            j++;
            v++;
        }
        i++;
        u++;
    }*/
}

// Fonction qui permet d'afficher une map dans une fenetre à partir de la structure map et des coordonnées en x et y relative
SDL_Surface* ENGINE_SET_map(struct Map *map,SDL_Surface* ecran1,int tiles_x,int tiles_y,SDL_Surface **tiles,int mode,struct Coordonnees taille_fenetre)
{
    // struct Map map : La map à afficher
    // int tiles_x : la tile relative horizontale
    // int tiles_y : la tile relative verticale
    // SDL_Surface* tiles[] : le tableau de surface de tiles
    // int mode : 0 = mode éditeur ; 1 = mode rendus graphique du jeux

    int i=0,j=0,type=0,u=0,v=0; // Curseur de deplacement dans le tableau bi-dimensionnel + type
    SDL_Rect Tiles_dimension;

    Tiles_dimension.h = 32;
    Tiles_dimension.w = 32;

    i = tiles_x;
    j = tiles_y;

    if(mode == 0)
    {
        SDL_FillRect(ecran1,NULL,SDL_MapRGB(ecran1->format, 149, 202, 244));
        i = tiles_x;
        j = tiles_y;
        while(i <= (tiles_x + taille_fenetre.x))
        {
            j = tiles_y;
            v = 0;
            while(j <= (tiles_y + taille_fenetre.y))
            {
                Tiles_dimension.x = u * LARGEUR_TILE;
                Tiles_dimension.y = v * HAUTEUR_TILE;
                type = map->tiles_T[i][j].type;
                if(type != 0)
                {
                    SDL_BlitSurface(tiles[map->tiles_T[i][j].type], NULL, ecran1, &Tiles_dimension);
                }
                j++;
                v++;
            }
            i++;
            u++;
        }
        return ecran1;
    }
    else
    {
        SDL_Surface* ecran2 = SDL_CreateRGBSurface(0,taille_fenetre.x*32,taille_fenetre.y*32,32,0,0,0,0);
        SDL_FillRect(ecran2,NULL,SDL_MapRGB(ecran2->format, 149, 202, 244));
        i = tiles_x;
        j = tiles_y;
        while(i <= (tiles_x + taille_fenetre.x))
        {
            j = tiles_y;
            v = 0;
            while(j <= (tiles_y + taille_fenetre.y))
            {
                Tiles_dimension.x = u * LARGEUR_TILE;
                Tiles_dimension.y = v * HAUTEUR_TILE;
                type = map->tiles_T[i][j].type;
                if(type != 0)
                {
                    SDL_BlitSurface(tiles[map->tiles_T[i][j].type], NULL, ecran2, &Tiles_dimension);
                }
                j++;
                v++;
            }
            i++;
            u++;
        }
        return ecran2;
    }
}

// Fonction GLOBAL de chargement du moteur graphique
int GLOBAL_MOTEUR_GRAPHIQUE(SDL_Window *window,SDL_Renderer *sdlRenderer,struct Map *map,struct Joueur *player,SDL_Surface **tiles,SDL_Rect *T_Rect,int init_timestamp,int init_eau)
{
    /*----------------------------------------------*/

        struct Coordonnees position_absolue; //
        struct Coordonnees taille_fenetre; //
        struct Element *T_Elements = player->save.T_Elements;
        int exit_value = 0;

        SDL_Surface *fenetre;
        SDL_Surface *background;
        SDL_Surface *hero;

    /*----------------------------------------------*/

    fenetre = SDL_GetWindowSurface(window);

    taille_fenetre = ENGINE_POCESS_nombresDeTiles(window); // On calcule la dimension en nombre de tiles de la fenetre

    taille_fenetre.x += MARGE_DEPLACEMENT_X;
    taille_fenetre.y += MARGE_DEPLACEMENT_Y;

    T_Rect[1].x = T_Elements[1].dynamique.x;
    T_Rect[1].y = T_Elements[1].dynamique.y;
    T_Rect[1].w = fenetre->w;
    T_Rect[1].h = fenetre->h;

    T_Rect[0].x = T_Elements[0].dynamique.x;
    T_Rect[0].y = T_Elements[0].dynamique.y;

    position_absolue.x = T_Rect[1].x/32;
    position_absolue.y = T_Rect[1].y/32;

    T_Rect[1].x -= position_absolue.x*32;
    T_Rect[1].y -= position_absolue.y*32;

    // On génère la map en position absolue
    background = ENGINE_SET_map(map,fenetre,position_absolue.x,position_absolue.y,tiles,1,taille_fenetre);

    // On affiche la map avec la camera
    SDL_BlitSurface(background, &T_Rect[1], fenetre, NULL);

    // On affiche le personnage
    hero =  HERO_display(T_Elements[0],T_Elements[1]);

    SDL_BlitSurface(hero, NULL, fenetre, &T_Rect[0]);
    
    if(DISPLAY_decompte(init_timestamp,window) == 1)
    {
        player->save.phase = 3;
    }

    if(DISPLAY_barre(init_eau,player->save.eau,window) == 0)
    {
        player->save.phase = 3;
    }

    SDL_FreeSurface(background);
    SDL_FreeSurface(hero);
    SDL_FreeSurface(fenetre);

    return exit_value;
}

// Fonction qui affiche une trame en guise de repère pour le débuggage des éléments graphiques du jeux
void ENGINE_SET_tramage(SDL_Window *window)
{
    struct Coordonnees tiles;
    tiles = ENGINE_POCESS_nombresDeTiles(window);

    SDL_Surface* ecran = SDL_GetWindowSurface(window);

    int i = 0;
    int j = 0;
    int k = 0,l = 0; // le switcheur de ligne pour les cases

    SDL_Rect trame1;
    SDL_Rect trame2;
    trame1.w = 32;
    trame1.h = 32;
    trame2.w = 32;
    trame2.h = 32;

    for (i = 1; i <= tiles.y; ++i)
    {
        trame1.y = (i-1)*32;
        trame2.y = (i-1)*32;
        k = i%2;
        l = (i+1)%2;
        for (j = 1; j <= tiles.x; j+=2)
        {

            trame1.x = (j+k-1)*32;
            trame2.x = (j+l-1)*32;

            SDL_FillRect(ecran, &trame1, SDL_MapRGB(ecran->format, 242, 242, 242));
            SDL_FillRect(ecran, &trame2, SDL_MapRGB(ecran->format, 199, 199, 199));
        }
    }
    SDL_UpdateWindowSurface(window);
}
