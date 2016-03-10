/* Nom : GUI.h

Dernier Auteur  : Armand

Creation :
26/02/2014

Dernière modification :
26/02/2014

Description :
	Fichier qui contient des fonctions relatives au fonctionnement de l'environnement graphique utilisateur

*///

#ifndef DEF_GUI
#define DEF_GUI

	// Fonction qui permet d'afficher un message dans une simple text_box avec un bouton ok
	int GUI_message_box(SDL_Window *window,char *message);

	// Fonction GUI qui gère des layout types de fenetres
	int GUI_spam(SDL_Window *window, struct GUI_object get_box,int type,char *texte,char *get_String,struct GUI_object *Tbox);

	// Fonction qui enregistre les frappes du clavier pour les convertir en chaine de caractères
	void GUI_GET_string(struct GUI_object box,char *T,int taille,SDL_Window *window);

	// Fonction qui affiche un bouton
	struct state GUI_DISPLAY_button(struct GUI_object box,SDL_Surface *ecran,char *nom_bouton);

	// Fonction qui contrôle la position de la souris dans la fenetre
	void GUI_LOG_souris();

	// Fonction qui contrôle l'état des touches du clavier
	void GUI_LOG_clavier(SDL_Event event_keyboard);

	// Fonction qui charge les boutons
	void GUI_chargement_boutons(const char **image,SDL_Surface **Tableau_BOUTON, int count);

	// Fonction qui supprime toutes les surfaces des boutons
	void GUI_free_tab_boutons(SDL_Surface **Tableau_BOUTON,int count);

	// Fonction qui affiche un menu en tenant compte uniquement de l'état des boutons et non pas de la position de la souris
	struct state GUI_check_key_state(SDL_Surface *ecran,SDL_Surface **Tableau_BOUTON,struct state etat_bouton);

	// Fonction qui renvoit l'état des boutons et qui les blites en tenant compte de la position de la souris
	struct state GUI_check_state(SDL_Surface *ecran,SDL_Surface **Tableau_BOUTON, int mode);

	// Fonction qui met l'etat nombre à un bouton en retournant le nouvel etat du bouton
	struct state GUI_set( int nombre);

	// Fonction ANIM qui permet de faire un fondus vers le noir
	void ANIM_fade_out(SDL_Window *window);

	// Fonction ANIM qui permet de faire un fondus entre deux écrans
	void ANIM_fade_in(SDL_Window *window,SDL_Surface *appear);

#endif
