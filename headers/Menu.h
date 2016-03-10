/* Nom : Menu.c

Dernier Auteur  : Charles

Creation :
08/02/2014

Derni�re modification :
08/02/2014

Description :
	prototypes des fonctions du menu.

*///

#ifndef DEF_MENU
#define DEF_MENU

	// Fonction qui g�re les actions du menu pour les diff�rents boutons
	int Submit_Menu(struct state bouton,SDL_Window *fenetre,SDL_Renderer *sdlRenderer);

	// Fonction qui g�re l'affichage du menu
	void Display_Menu(SDL_Window *fenetre,SDL_Renderer *sdlRenderer);

	// Fonction qui g�re le menu d'options
	int OPTIONS(SDL_Window *fenetre);
	
#endif
