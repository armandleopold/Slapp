/* Nom : engine.c

Dernier Auteur  : Armand

Creation :
05/02/2014

Dernière modification :
22/02/2014

Description :
    Fichier qui contient toutes les fonctions relatives au moteur graphique du jeux

*///

#ifndef MOTEUR_GRAPHIQUE_H_INCLUDED
#define MOTEUR_GRAPHIQUE_H_INCLUDED

	// Fonction qui permet d'afficher une map dans une fenetre à partir de la structure map et des coordonnées en x et y relative
	SDL_Surface* ENGINE_SET_map(struct Map *map,SDL_Surface* ecran1,int tiles_x,int tiles_y,SDL_Surface **tiles,int mode,struct Coordonnees taille_fenetre);

	// Fonction qui permet d'afficher une map dans une fenetre à partir de la structure map et des coordonnées en x et y relative
	void ENGINE_render_map(struct Map *map,int tiles_x,int tiles_y,SDL_Surface **tiles,struct Coordonnees taille_fenetre,SDL_Renderer *sdlRenderer);

	// Fonction GLOBAL de chargement du moteur graphique
	int GLOBAL_MOTEUR_GRAPHIQUE(SDL_Window *window,SDL_Renderer *sdlRenderer,struct Map *map,struct Joueur *player,SDL_Surface **tiles,SDL_Rect *T_Rect,int init_timestamp,int init_eau);
	
	// Fonction qui affiche une trame en guise de repère pour le débuggage des éléments graphiques du jeux
	void ENGINE_SET_tramage(SDL_Window *window);

#endif // MOTEUR_GRAPHIQUE_H_INCLUDED