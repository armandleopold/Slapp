/* Nom : engine.c

Dernier Auteur  : Armand

Creation :
05/02/2014

Dernière modification :
22/02/2014

Description :
    Fichier qui contient toutes les fonctions relatives au moteur graphique du jeux

*///

#ifndef MOTEUR_PHYSIQUE_H_INCLUDED
#define MOTEUR_PHYSIQUE_H_INCLUDED

	// Fonction qui test si il y a collision entre des elements de la map
	struct Collision ENGINE_collision(SDL_Rect *T_Rect,struct Element *T_Elements,struct Map *map,SDL_Surface *ecran,struct tiles_spec *tiles_spec);

	// Fonction qui modifie les proprietes de mouvement d'un element
	void ENGINE_mouvement(struct Mvt *element);

	// Fonction qui ajuste la position de la caméra pour la centrer sur le personnage principal
	void ENGINE_traqueur(SDL_Surface *ecran,struct Mvt *point,struct Mvt *camera,struct Map *map);

	// Fonction qui associe aux éléments la gravité
	void ENGINE_Gravity(SDL_Rect *T_Rect,SDL_Surface *Ecran,struct Map *map,struct Element *T_Elements);

	// Fonction qui affiche les test effectués par le moteur de collisions
	void Debug_collision(struct Map map,SDL_Window *window,SDL_Rect *T_Rect,struct Element *T_Elements, struct Collision collision);

	// Fonction qui gère la physique du jeux
	int GLOBAL_MOTEUR_PHYSIQUE(SDL_Surface* Ecran,struct Map *map,struct Sauvegarde_partie *save,SDL_Rect *T_Rect,struct tiles_spec *tiles_spec);

#endif // MOTEUR_PHYSIQUE_H_INCLUDED
