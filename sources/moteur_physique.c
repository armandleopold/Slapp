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

// Fonction qui test si il y a collision entre des elements de la map
struct Collision ENGINE_collision(SDL_Rect *T_Rect,struct Element *T_Elements,struct Map *map,SDL_Surface *ecran,struct tiles_spec *tiles_spec)
{

    struct Collision collision;

    int tile_type1 = 0,
    tile_type2 = 0,
    tile_type3 = 0,
    tile_type4 = 0,
    tile_type3bis=0,
    tile_type4bis=0,
    tile_type3bis2=0,
    tile_type4bis2=0;

    int X1=0,
    X2=0,
    X3=0,
    X4=0;

    int Y1=0,
    Y2=0,
    Y3=0,
    Y4=0;

    int P1,P2,P3,P4,P3bis,P4bis,P3bis2,P4bis2;

    int new_pos_x = 0,cam_pos_x=0;
    int new_pos_y = 0,cam_pos_y=0;

    // On initialise le compte rendus de collisions

        // Collisions en haut
            // Collisions en haut a gauche
            collision.collision[0][0]=0;
            // Collisions en haut a droite
            collision.collision[0][1]=0;

        // Collision en bas
            // Collisions en bas a gauche
            collision.collision[1][0]=0;
            // Collisions en bas a droite
            collision.collision[1][1]=0;

    cam_pos_x = T_Elements[1].dynamique.x;
    cam_pos_y = T_Elements[1].dynamique.y;

    // Coordonnées de la tile qui contient le point en haut à gauche du perso
    X1 = (T_Elements[1].dynamique.x + T_Elements[0].dynamique.x)/32;
    Y1 = (T_Elements[1].dynamique.y + T_Elements[0].dynamique.y)/32;
    // Coordonnées de la tile qui contient le point en haut à droite du perso
    X2 = (T_Elements[1].dynamique.x + T_Elements[0].dynamique.x + T_Rect[0].w)/32;
    Y2 = (T_Elements[1].dynamique.y + T_Elements[0].dynamique.y)/32;
    // Coordonnées de la tile qui contient le point en bas à gauche du perso
    X3 = (T_Elements[1].dynamique.x + T_Elements[0].dynamique.x)/32;
    Y3 = (T_Elements[1].dynamique.y + T_Elements[0].dynamique.y + T_Rect[0].h)/32;
    // Coordonnées de la tile qui contient le point en bas à droite du perso
    X4 = (T_Elements[1].dynamique.x + T_Elements[0].dynamique.x + T_Rect[0].w)/32;
    Y4 = (T_Elements[1].dynamique.y + T_Elements[0].dynamique.y + T_Rect[0].h)/32;

    // On récupère le type des tiles
    tile_type1 = map->tiles_T[X1][Y1].type;
    tile_type2 = map->tiles_T[X2][Y2].type;
    tile_type3 = map->tiles_T[X3][Y3].type;
    tile_type4 = map->tiles_T[X4][Y4].type;

    tile_type3bis = map->tiles_T[X3][Y3-1].type;
    tile_type4bis = map->tiles_T[X4][Y4-1].type;
    tile_type3bis2 = map->tiles_T[X3+1][Y3].type;
    tile_type4bis2 = map->tiles_T[X4-1][Y4].type;

    // Les points qui sont utilisés dans les tests de collisions en fonction de leur valeur
    // 0 ou 1 ( un espèce de booléen trafiqué qui marche pas trop mal )
    P1 = tiles_spec[tile_type1].passable;
    P2 = tiles_spec[tile_type2].passable;
    P3 = tiles_spec[tile_type3].passable;
    P4 = tiles_spec[tile_type4].passable;

    P3bis = tiles_spec[tile_type3bis].passable;
    P4bis = tiles_spec[tile_type4bis].passable;
    P3bis2 = tiles_spec[tile_type3bis2].passable;
    P4bis2 = tiles_spec[tile_type4bis2].passable;

    SDL_Rect point1,
    point2,
    point3,
    point4,
    point5,
    point6,
    point7,
    point8;

    point1.x = X1*32 - cam_pos_x;
    point1.y = Y1*32- cam_pos_y;
    point1.w = 16;
    point1.h = 16;
    T_Rect[1] = point1;

    point2.x = X2*32 - cam_pos_x;
    point2.y = Y2*32- cam_pos_y;
    point2.w = 16;
    point2.h = 16;
    T_Rect[2] = point2;

    point3.x = X3*32 - cam_pos_x;
    point3.y = Y3*32- cam_pos_y;
    point3.w = 16;
    point3.h = 16;
    T_Rect[3] = point3;

    point4.x = X4*32 - cam_pos_x;
    point4.y = Y4*32- cam_pos_y;
    point4.w = 16;
    point4.h = 16;
    T_Rect[4] = point4;

    point5.x = X3*32+32 - cam_pos_x;
    point5.y = Y3*32- cam_pos_y;
    point5.w = 16;
    point5.h = 16;
    T_Rect[5] = point5;

    point6.x = X4*32-32 - cam_pos_x;
    point6.y = Y4*32- cam_pos_y;
    point6.w = 16;
    point6.h = 16;
    T_Rect[6] = point6;

    point7.x = X3*32 - cam_pos_x;
    point7.y = Y3*32-32- cam_pos_y;
    point7.w = 16;
    point7.h = 16;
    T_Rect[7] = point7;

    point8.x = X4*32 - cam_pos_x;
    point8.y = Y4*32-32- cam_pos_y;
    point8.w = 16;
    point8.h = 16;
    T_Rect[8] = point8;


    // Test de collision en fonction des directions de l'objet
    if ((T_Elements[0].dynamique.vx < 0 || T_Elements[1].dynamique.vx < 0)
    && (T_Elements[0].dynamique.vy < 0 || T_Elements[1].dynamique.vy < 0)) // Vers la gauche et le haut
    {
        if ((P1 == 0)
        || (P3 == 0 && P4 != 0)
        || (P2 == 0 && P4 == 0))
        {
            if ((P1 == 0 && P2 != 0 && P3 != 0)
             || (P1 == 0 && P2 == 0 && P3 == 0)
             || (P1 != 0 && P2 == 0 && P3 == 0))
            { // Obstacle coin entier ou juste carré en P1
                new_pos_x = X1;
                T_Elements[0].dynamique.x = new_pos_x*32 + 32 - cam_pos_x;
                T_Elements[0].dynamique.ax = 0;
                T_Elements[0].dynamique.vx = 0;

                new_pos_y = Y1;
                T_Elements[0].dynamique.y = new_pos_y*32 + 32 - cam_pos_y;
                T_Elements[0].dynamique.ay = 0;
                T_Elements[0].dynamique.vy = 0;

                collision.collision[0][0]=1;
            }
            else if (P1 == 0
            &&       P2 == 0
            &&       P3 != 0)
            { // Obstacle au dessus
                new_pos_y = Y1;
                T_Elements[0].dynamique.y = new_pos_y*32 + 32 - cam_pos_y;
                T_Elements[0].dynamique.ay = 0;
                T_Elements[0].dynamique.vy = 0;

                collision.collision[0][0]=1;
                collision.collision[0][1]=1;
            }
            else if (P1 == 0
            &&       P2 != 0
            &&       P3bis == 0)
            { // Obstacle a gauche
                new_pos_x = X1;
                T_Elements[0].dynamique.x = new_pos_x*32 + 32 - cam_pos_x;
                T_Elements[0].dynamique.ax = 0;
                T_Elements[0].dynamique.vx = 0;
            }
            else if (P3 == 0 && P3bis2 == 0 && P1 != 0)
            {
                // Obstacle a gauche
                new_pos_x = X1;
                T_Elements[0].dynamique.x = new_pos_x*32 + 32 - cam_pos_x;
                T_Elements[0].dynamique.ax = 0;
                T_Elements[0].dynamique.vx = 0;
            }
        }
    }
    else if ((T_Elements[0].dynamique.vx > 0 || T_Elements[1].dynamique.vx > 0)
    && (T_Elements[0].dynamique.vy < 0 || T_Elements[1].dynamique.vy < 0)) // Vers la droite et le haut
    {
        if ((P2 == 0)
        || (P3 != 0 && P4 == 0)
        || (P1 == 0 && P3 != 0))
        {
            if ((P2 == 0 && P1 != 0 && P4 != 0)
             || (P2 == 0 && P1 == 0 && P4 == 0)
             || (P2 != 0 && P1 == 0 && P4 == 0))
            { // Obstacle coin entier ou juste carré en P2
                new_pos_x = X2;
                T_Elements[0].dynamique.x = new_pos_x*32 - T_Rect[0].w - cam_pos_x - 1;
                T_Elements[0].dynamique.ax = 0;
                T_Elements[0].dynamique.vx = 0;

                new_pos_y = Y2;
                T_Elements[0].dynamique.y = new_pos_y*32 + 32 - cam_pos_y;
                T_Elements[0].dynamique.ay = 0;
                T_Elements[0].dynamique.vy = 0;
            }
            else if ((P1 == 0
            &&       P2 == 0
            &&       P4 != 0))
            { // Obstacle au dessus
                new_pos_y = Y2;
                T_Elements[0].dynamique.y = new_pos_y*32 + 32 - cam_pos_y;
                T_Elements[0].dynamique.ay = 0;
                T_Elements[0].dynamique.vy = 0;
            }
            else if (P2 == 0
            &&       P1 != 0
            &&       P4 == 0)
            { // Obstacle a droite
                new_pos_x = X2;
                T_Elements[0].dynamique.x = new_pos_x*32 - T_Rect[0].w - cam_pos_x - 1;
                T_Elements[0].dynamique.ax = 0;
                T_Elements[0].dynamique.vx = 0;
            }
        }
    }
    else if ((T_Elements[0].dynamique.vy > 0 || T_Elements[1].dynamique.vy > 0)
    && (T_Elements[0].dynamique.vx > 0 || T_Elements[1].dynamique.vx > 0)) // Vers le bas et la droite
    {
        if ((P4 == 0)
        || (P1 != 0 && P2 != 0)
        || (P1 != 0 && P3 == 0))
        {
            if ((P4 == 0 && P3 != 0 && P2 != 0)
            || (P4 == 0 && P3 == 0 && P2 == 0)
            || (P4 != 0 && P3 == 0 && P2 == 0))
            { // Obstacle coin entier ou juste carré en P1
                new_pos_x = X4;
                T_Elements[0].dynamique.x = new_pos_x*32 - T_Rect[0].w-cam_pos_x;
                T_Elements[0].dynamique.ax = 0;
                T_Elements[0].dynamique.vx = 0;

                new_pos_y = Y4;
                T_Elements[0].dynamique.y = new_pos_y*32 - T_Rect[0].h - cam_pos_y;
                T_Elements[0].dynamique.ay = 0;
                T_Elements[0].dynamique.vy = 0;
            }
            else if (P3 == 0
            &&       P4 == 0
            &&       P2 != 0)
            { // Obstacle au dessous
                new_pos_y = Y4;
                T_Elements[0].dynamique.y = new_pos_y*32 - T_Rect[0].h - cam_pos_y;
                T_Elements[0].dynamique.ay = 0;
                T_Elements[0].dynamique.vy = 0;
            }
            else if (P2 == 0
            &&       P3 != 0
            &&       P4 == 0)
            { // Obstacle a droite
                new_pos_x = X4;
                T_Elements[0].dynamique.x = new_pos_x*32 - T_Rect[0].w-cam_pos_x;
                T_Elements[0].dynamique.ax = 0;
                T_Elements[0].dynamique.vx = 0;
            }
        }
    }
    else if ((T_Elements[0].dynamique.vy > 0 || T_Elements[1].dynamique.vy > 0)
    && (T_Elements[0].dynamique.vx < 0 || T_Elements[1].dynamique.vx < 0)) // Vers le bas et la gauche
    {
        if ((P3 == 0)
        || (P1 == 0 && P2 != 0)
        || (P2 == 0 && P4 != 0))
        {
            if ((P3 == 0 && P1 != 0 && P4 != 0)
             || (P3 == 0 && P1 == 0 && P4 == 0)
             || (P3 != 0 && P1 == 0 && P4 == 0))
            { // Obstacle coin entier ou juste carré en P1
                new_pos_x = X3;
                T_Elements[0].dynamique.x = new_pos_x*32 + 32 - cam_pos_x - 1;
                T_Elements[0].dynamique.ax = 0;
                T_Elements[0].dynamique.vx = 0;

                new_pos_y = Y3;
                T_Elements[0].dynamique.y = new_pos_y*32 - T_Rect[0].h - cam_pos_y;
                T_Elements[0].dynamique.ay = 0;
                T_Elements[0].dynamique.vy = 0;
            }
            else if (P3 == 0
            &&       P4 == 0
            &&       P1 != 0)
            { // Obstacle au dessous
                new_pos_y = Y3;
                T_Elements[0].dynamique.y = new_pos_y*32 - T_Rect[0].h - cam_pos_y;
                T_Elements[0].dynamique.ay = 0;
                T_Elements[0].dynamique.vy = 0;
            }
            else if (P1 == 0
            &&       P4 != 0
            &&       P3 == 0)
            { // Obstacle a gauche
                new_pos_x = X3;
                T_Elements[0].dynamique.x = new_pos_x*32 + 32 - cam_pos_x;
                T_Elements[0].dynamique.ax = 0;
                T_Elements[0].dynamique.vx = 0;
            }
        }
    }
    else if ((T_Elements[0].dynamique.vx < 0 || T_Elements[1].dynamique.vx < 0)
    && (T_Elements[0].dynamique.vy == 0  || T_Elements[1].dynamique.vy == 0)) // Vers la gauche
    {
        if ((P3bis == 0 && P4bis != 0)
        || (P1 == 0 && P2 != 0)
        || (P1 == 0 && P3bis == 0))
        {
            new_pos_x = X1;
            T_Elements[0].dynamique.x = new_pos_x*32 + 32 - cam_pos_x;
            T_Elements[0].dynamique.ax = 0;
            T_Elements[0].dynamique.vx = 0;

            collision.collision[0][0]=1;
            collision.collision[1][0]=1;
        }
    }
    else if ((T_Elements[0].dynamique.vy < 0 || T_Elements[1].dynamique.vy < 0)
    && (T_Elements[0].dynamique.vx == 0 || T_Elements[1].dynamique.vx == 0)) // Vers le haut
    {
        if ((P1 == 0 && P2 == 0)
        ||  (P1 == 0 && P2 != 0 && P3 != 0)
        || (P1 != 0 && P2 == 0 && P4 != 0))
        {
            new_pos_y = Y1;
            T_Elements[0].dynamique.y = new_pos_y*32 + 32 - cam_pos_y;
            T_Elements[0].dynamique.ay = 0;
            T_Elements[0].dynamique.vy = 0;

            collision.collision[0][0]=1;
            collision.collision[0][1]=1;
        }
    }
    else if ((T_Elements[0].dynamique.vx > 0 || T_Elements[1].dynamique.vx > 0)
    && (T_Elements[0].dynamique.vy == 0 || T_Elements[1].dynamique.vy == 0)) // Vers la droite
    {
        if ((P3bis != 0 && P4bis == 0)
        || (P1 != 0 && P2 == 0)
        || (P2 == 0 && P4bis == 0))
        {
            new_pos_x = X2;
            T_Elements[0].dynamique.x = new_pos_x*32 - T_Rect[0].w -cam_pos_x - 1;
            T_Elements[0].dynamique.ax = 0;
            T_Elements[0].dynamique.vx = 0;

            collision.collision[0][1]=1;
            collision.collision[1][1]=1;
        }
    }
    else if ((T_Elements[0].dynamique.vy > 0 || T_Elements[1].dynamique.vy > 0)
    && (T_Elements[0].dynamique.vx == 0 || T_Elements[1].dynamique.vx == 0)) // Vers le bas
    {
        if ((P3 == 0 && P4 == 0)
        || (P3 == 0 && P1 != 0)
        || (P2 != 0 && P4 == 0)
        || (P3bis2 == 0 && P4bis2 == 0))
        {
            new_pos_y = Y3;
            T_Elements[0].dynamique.y = new_pos_y*32 - T_Rect[0].h  - cam_pos_y;
            T_Elements[0].dynamique.ay = 0;
            T_Elements[0].dynamique.vy = 0;

            collision.collision[1][0]=1;
            collision.collision[1][1]=1;
        }
    }


    return collision;
}

// Fonction qui modifie les proprietes de mouvement d'un element
void ENGINE_mouvement(struct Mvt *element)
{
    float t = 1/(IMAGE_PAR_SECONDES*1.00);

    element->vx += element->ax*t;
    element->vy += element->ay*t;

    element->x += 0.5*element->ax*t*t + element->vx*t;
    element->y += 0.5*element->ay*t*t + element->vy*t;
}

int ENGINE_eau_manager(SDL_Rect *T_Rect,struct Sauvegarde_partie *save,struct Map *map,SDL_Surface *ecran,struct tiles_spec *tiles_spec)
{
    int phase = 2;
    int i = 0;

    int tile_type1 = 0,
    tile_type2 = 0,
    tile_type3 = 0,
    tile_type4 = 0;

    int X1=0,
    X2=0,
    X3=0,
    X4=0;

    int Y1=0,
    Y2=0,
    Y3=0,
    Y4=0;

    // Coordonnées de la tile qui contient le point en haut à gauche du perso
    X1 = (save->T_Elements[1].dynamique.x + save->T_Elements[0].dynamique.x)/32;
    Y1 = (save->T_Elements[1].dynamique.y + save->T_Elements[0].dynamique.y)/32;
    // Coordonnées de la tile qui contient le point en haut à droite du perso
    X2 = (save->T_Elements[1].dynamique.x + save->T_Elements[0].dynamique.x + T_Rect[0].w)/32;
    Y2 = (save->T_Elements[1].dynamique.y + save->T_Elements[0].dynamique.y)/32;
    // Coordonnées de la tile qui contient le point en bas à gauche du perso
    X3 = (save->T_Elements[1].dynamique.x + save->T_Elements[0].dynamique.x)/32;
    Y3 = (save->T_Elements[1].dynamique.y + save->T_Elements[0].dynamique.y + T_Rect[0].h)/32;
    // Coordonnées de la tile qui contient le point en bas à droite du perso
    X4 = (save->T_Elements[1].dynamique.x + save->T_Elements[0].dynamique.x + T_Rect[0].w)/32;
    Y4 = (save->T_Elements[1].dynamique.y + save->T_Elements[0].dynamique.y + T_Rect[0].h)/32;

    // On récupère le type des tiles
    tile_type1 = map->tiles_T[X1][Y1].type;
    tile_type2 = map->tiles_T[X2][Y2].type;
    tile_type3 = map->tiles_T[X3][Y3].type;
    tile_type4 = map->tiles_T[X4][Y4].type;

    for ( i = 0; i < 50; ++i)
    {
        if (map->tiles_spec_T[i].type == tile_type1
        ||  map->tiles_spec_T[i].type == tile_type2
        ||  map->tiles_spec_T[i].type == tile_type3
        ||  map->tiles_spec_T[i].type == tile_type4)
        {
            save->eau -= map->tiles_spec_T[i].abs;
        }
    }

    if (save->eau < 0)
    {
        save->eau = 0;
    }

    return phase;
}

int ENGINE_End_Traqueur(SDL_Rect *T_Rect,struct Element *T_Elements,struct Map *map,SDL_Surface *ecran,struct tiles_spec *tiles_spec)
{
    int phase = 2;

    int tile_type1 = 0,
    tile_type2 = 0,
    tile_type3 = 0,
    tile_type4 = 0;

    int X1=0,
    X2=0,
    X3=0,
    X4=0;

    int Y1=0,
    Y2=0,
    Y3=0,
    Y4=0;

    // Coordonnées de la tile qui contient le point en haut à gauche du perso
    X1 = (T_Elements[1].dynamique.x + T_Elements[0].dynamique.x)/32;
    Y1 = (T_Elements[1].dynamique.y + T_Elements[0].dynamique.y)/32;
    // Coordonnées de la tile qui contient le point en haut à droite du perso
    X2 = (T_Elements[1].dynamique.x + T_Elements[0].dynamique.x + T_Rect[0].w)/32;
    Y2 = (T_Elements[1].dynamique.y + T_Elements[0].dynamique.y)/32;
    // Coordonnées de la tile qui contient le point en bas à gauche du perso
    X3 = (T_Elements[1].dynamique.x + T_Elements[0].dynamique.x)/32;
    Y3 = (T_Elements[1].dynamique.y + T_Elements[0].dynamique.y + T_Rect[0].h)/32;
    // Coordonnées de la tile qui contient le point en bas à droite du perso
    X4 = (T_Elements[1].dynamique.x + T_Elements[0].dynamique.x + T_Rect[0].w)/32;
    Y4 = (T_Elements[1].dynamique.y + T_Elements[0].dynamique.y + T_Rect[0].h)/32;

    // On récupère le type des tiles
    tile_type1 = map->tiles_T[X1][Y1].type;
    tile_type2 = map->tiles_T[X2][Y2].type;
    tile_type3 = map->tiles_T[X3][Y3].type;
    tile_type4 = map->tiles_T[X4][Y4].type;

    if (tile_type1 == 2 &&
        tile_type2 == 2 &&
        tile_type3 == 2 &&
        tile_type4 == 2)
    {
        phase = 4; // le joueur est entrée dans la zone de fin, il gagne.
    }

    return phase;
}

// Fonction qui gère la physique du jeux
int GLOBAL_MOTEUR_PHYSIQUE(SDL_Surface* Ecran,struct Map *map,struct Sauvegarde_partie *save,SDL_Rect *T_Rect,struct tiles_spec *tiles_spec)
{
    int return_value = 0;

    // On soummet les elements de la map à la dur loi de la gravite
    ENGINE_Gravity(T_Rect,Ecran,map,save->T_Elements);

    // On ajuste la position de la camera pour la centrer sur la nouvelle position du personnage principal
    ENGINE_traqueur(Ecran,&save->T_Elements[0].dynamique,&save->T_Elements[1].dynamique,map);

    // On fait bouger la camera
    ENGINE_mouvement(&save->T_Elements[1].dynamique);

    // On fait bouger le perso principal
    ENGINE_mouvement(&save->T_Elements[0].dynamique);

    // On test les colissions et on réajuste la position du perso pour eviter qu'il traverse les murs etc...
    ENGINE_collision(T_Rect,save->T_Elements,map,Ecran,tiles_spec);

    // On regarde si le hero n'est pas dans une zone d'arrivée
    save->phase = ENGINE_End_Traqueur(T_Rect,save->T_Elements,map,Ecran,tiles_spec);

    // On réduit la barre d'eau de la valeur d'absorption de la tile sur laquelle est posé le joueur
    ENGINE_eau_manager(T_Rect,save,map,Ecran,tiles_spec);

    return return_value;
}

// Fonction qui ajuste la position de la caméra pour la centrer sur le personnage principal
void ENGINE_traqueur(SDL_Surface *ecran,struct Mvt *point,struct Mvt *camera,struct Map *map)
{
    const int ref_map_x = TAILLE_MAP_X*LARGEUR_TILE;
    const int ref_map_y = TAILLE_MAP_Y*HAUTEUR_TILE;

    const int ref_ecran_x = ecran->w;
    const int ref_ecran_y = ecran->h;

    float centre_camera_x = ecran->w/2;
    float centre_camera_y = ecran->h/2;

    float distance_camera_point_x;
    float distance_camera_point_y;

    float point_absolue_x = point->x + camera->x;
    float point_absolue_y = point->y + camera->y;

    if (point_absolue_x > 0
     && point_absolue_x < ref_map_x
     && point_absolue_y > 0
     && point_absolue_y < ref_map_y)
    {
        // On verifie si le point se trouve sur la map

        distance_camera_point_x = point->x - centre_camera_x ;
        distance_camera_point_y = point->y - centre_camera_y ;

        if (camera->x >= 0
         && camera->y >= 0
         && ref_ecran_x <= ref_map_x
         && ref_ecran_y <= ref_map_y
         && (camera->x + ref_ecran_x) <= ref_map_x
         && (camera->y + ref_ecran_y) <= ref_map_y)
        {
            if (distance_camera_point_x >= ref_ecran_x/5) // bordure droite
            {
                camera->vx = 400;
                point->vx -= 400;
            }
            else if (distance_camera_point_x < -ref_ecran_x/5) // bordure gauche
            {
                camera->vx = -400;
                point->vx += 400;
            }
            else
            {
                camera->vx = 0;
            }

            if (distance_camera_point_y > ref_ecran_y/5) // bordure bas
            {
                if (camera->vy >= VITESSE_DE_CHUTE_MAX)
                {
                    camera->ay = 0;
                }
                else
                {
                    camera->ay = GRAVITY;
                }

                point->vy = 0;
                point->ay = 0;
            }
            else if (distance_camera_point_y < - ref_ecran_y/7) // bordure haut
            {
                camera->vy = 1.8*distance_camera_point_y;
            }
            else
            {
                camera->vy = 0;
                camera->ay = 0;
            }
            // si la distance est nulle, alors la camera est déjà centrée sur le point on ne fait rien.
        }
        else
        {

            if (camera->x < 0)
            {
                camera->x = 0;
                camera->vx = 0;
            }

            if (camera->y < 0)
            {
                camera->y = 0;
                camera->vy = 0;
            }

            if (point->x < 0 )
            {
                point->x = 0;
                point->vx = 0;
            }


            if (point->y < 0 )
            {
                point->y = 0;
                point->vy = 0;
            }

            //printf("// Traqueur : Erreur de valeur de coordonnées de la camera , la camera n'est pas dans la map.\n");
        }
    }
    else // si la caméra sort de la map, on la remet au point de départ
    {
        printf("return\n");
        point->x = ecran->w/2;
        point->vx = 0;

        point->y = ecran->h/2;
        point->vy = 0;

        camera->x = map->start.x*32 - ecran->w/2;
        camera->vx = 0;
        camera->ax = 0;

        camera->y = map->start.y*32 - ecran->h/2;
        camera->vy = 0;
        camera->ay = 0;

        //printf("// Traqueur : Erreur de valeur de coordonnées du point , le point n'est pas dans la map.\n");
    }
}

// Fonction qui associe aux éléments la gravité
void ENGINE_Gravity(SDL_Rect *T_Rect,SDL_Surface *Ecran,struct Map *map,struct Element *T_Elements)
{
    int i = 0;
    int     tile_type3 = 0,
    tile_type4 = 0,tile_type3bis2=0,
    tile_type4bis2=0;

    int X3=0,
    X4=0;

    int Y3=0,
    Y4=0;

    int P3,P4,P3bis2,P4bis2;

    while(T_Elements[i].type_element != -1)
    {
        // Coordonnées de la tile qui contient le point en bas à gauche du perso
        X3 = (T_Elements[1].dynamique.x + T_Elements[0].dynamique.x)/32;
        Y3 = (T_Elements[1].dynamique.y + T_Elements[0].dynamique.y + T_Rect[0].h)/32;
        // Coordonnées de la tile qui contient le point en bas à droite du perso
        X4 = (T_Elements[1].dynamique.x + T_Elements[0].dynamique.x + T_Rect[0].w)/32;
        Y4 = (T_Elements[1].dynamique.y + T_Elements[0].dynamique.y + T_Rect[0].h)/32;

        // On récupère le type des tiles
        tile_type3 = map->tiles_T[X3][Y3].type;
        tile_type4 = map->tiles_T[X4][Y4].type;
        tile_type3bis2 = map->tiles_T[X3+1][Y3].type;
        tile_type4bis2 = map->tiles_T[X4-1][Y4].type;

        // Les points qui sont utilisés dans les tests de collisions en fonction de leur valeur
        // 0 ou 1 ( un espèce de booléen trafiqué qui marche pas trop mal ^_^ )
        P3 = map->tiles_spec_T[tile_type3].passable;
        P4 = map->tiles_spec_T[tile_type4].passable;
        P3bis2 = map->tiles_spec_T[tile_type3bis2].passable;
        P4bis2 = map->tiles_spec_T[tile_type4bis2].passable;

        if((P3bis2 != 0 && P4bis2 != 0 && P3 != 0 && P4 != 0))
        {
            if (T_Elements[i].dynamique.vy >= VITESSE_DE_CHUTE_MAX)
            {
                T_Elements[i].dynamique.ay = 0;
            }
            else
            {
                T_Elements[i].dynamique.ay = GRAVITY;
            }
        }

        i++;
    }
}

// Fonction qui affiche les test effectués par le moteur de collisions
void Debug_collision(struct Map map,SDL_Window *window,SDL_Rect *T_Rect,struct Element *T_Elements, struct Collision collision)
{
    /*----------------------------------------------*/

        int i;
        int cam_x = T_Elements[1].dynamique.x;
        int cam_y = T_Elements[1].dynamique.y;

        char temps[200] = "";

        SDL_Rect position;

        SDL_Surface *render;
        SDL_Surface *fenetre;

        SDL_Color couleurNoire = {255, 0, 0};
        SDL_Color couleurNoire2 = {0, 0, 0};
        SDL_Color couleurNoire3 = {0, 0, 255};

        TTF_Font *police = TTF_OpenFont("comfortalight.ttf", 16);
    /*----------------------------------------------*/

    fenetre = SDL_GetWindowSurface(window);

    sprintf(temps, "Pos cam : %f : %f ", T_Elements[1].dynamique.x,T_Elements[1].dynamique.y);
    render = TTF_RenderText_Blended(police, temps, couleurNoire);
    position.x = 5;
    position.y = 10;
    SDL_BlitSurface(render, NULL, fenetre, &position);
    SDL_FreeSurface(render);

    sprintf(temps, "Vit cam : %f : %f ", T_Elements[1].dynamique.vx,T_Elements[1].dynamique.vy);
    render = TTF_RenderText_Blended(police, temps, couleurNoire);
    position.x = 5;
    position.y = 20;
    SDL_BlitSurface(render, NULL, fenetre, &position);
    SDL_FreeSurface(render);

    sprintf(temps, "Pos Perso : %f : %f ", T_Elements[0].dynamique.x,T_Elements[0].dynamique.y);
    render = TTF_RenderText_Blended(police, temps, couleurNoire2);
    position.x = 5;
    position.y = 35;
    SDL_BlitSurface(render, NULL, fenetre, &position);
    SDL_FreeSurface(render);

    sprintf(temps, "Vit Perso : %f : %f ", T_Elements[0].dynamique.vx,T_Elements[0].dynamique.vy);
    render = TTF_RenderText_Blended(police, temps, couleurNoire2);
    position.x = 5;
    position.y = 45;
    SDL_BlitSurface(render, NULL, fenetre, &position);
    SDL_FreeSurface(render);

    sprintf(temps, "Pos MAP : %f : %f ", T_Elements[1].dynamique.x/32,T_Elements[1].dynamique.y/32);
    render = TTF_RenderText_Blended(police, temps, couleurNoire3);
    position.x = 5;
    position.y = 60;
    SDL_BlitSurface(render, NULL, fenetre, &position);
    SDL_FreeSurface(render);

    /*sprintf(temps, "Collisions : %d : %d ", collision.collision[0][0],collision.collision[0][1]);
    render = TTF_RenderText_Blended(police, temps, couleurNoire3);
    position.x = 5;
    position.y = 80;
    SDL_BlitSurface(render, NULL, fenetre, &position);
    SDL_FreeSurface(render);

    sprintf(temps, " %d : %d ",collision.collision[1][0],collision.collision[1][1]);
    render = TTF_RenderText_Blended(police, temps, couleurNoire3);
    position.x = 62;
    position.y = 90;
    SDL_BlitSurface(render, NULL, fenetre, &position);
    SDL_FreeSurface(render);*/

    for ( i = 2; i < 9; ++i)
    {
        T_Rect[i].h = 16;
        T_Rect[i].w = 16;

        if (map.tiles_spec_T[map.tiles_T[(cam_x + T_Rect[i].x)/32][(cam_y  + T_Rect[i].y)/32].type].passable == 0)
        {
            SDL_FillRect(fenetre,&T_Rect[i],SDL_MapRGB(fenetre->format, 255, 0, 0));
        }
        else
        {
            SDL_FillRect(fenetre,&T_Rect[i],SDL_MapRGB(fenetre->format, 0, 255, 0));
        }
    }
}
