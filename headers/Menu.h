/* Nom : Menu.c

Dernier Auteur  : Charles

Creation :
08/02/2014

Dernière modification :
08/02/2014

Description :
	prototypes des fonctions du menu.

*///

#ifndef DEF_MENU
#define DEF_MENU

	// Fonction qui gère les actions du menu pour les différents boutons
	int Submit_Menu(struct state bouton,SDL_Window *fenetre,SDL_Renderer *sdlRenderer);

	// Fonction qui gère l'affichage du menu
	void Display_Menu(SDL_Window *fenetre,SDL_Renderer *sdlRenderer);

	// Fonction qui gère le menu d'options
	int OPTIONS(SDL_Window *fenetre);
	
#endif
