/* Nom : engine.h

Dernier Auteur  : Armand

Creation :
24/01/2014

Dernière modification :
12/02/2014

Description :
	Fichier qui contient des fonctions relatives au fonctionnement de l'environnement graphique du moteur du jeux

*///

#ifndef ENGINE_H_INCLUDED
#define ENGINE_H_INCLUDED

	// Fonction qui permet d'éditer en brut la map
	int MAP_EDITOR_loader(SDL_Window *window,char *nom_de_la_map2,char *musique,char *background,int creation);

	// Fonction qui permet de gérer les tiles
	int MAP_EDITOR_gestionnaire_de_tiles(SDL_Window *window);

	// Fonction GLOBAL de chargement de l'editeur de map
	int GLOBAL_MOTEUR_EDITEUR(SDL_Window *window);

	// Fonction qui affiche les specifications des tiles dans la console
	void ENGINGE_LOG_afficher_table(struct tiles_spec *table,int nombre);

	// Fonction pouvant être appelée pour réinitialiser les parametres par default du moteur graphique
	void ENGINE_SET_defaultConfigFile();

	// Fonction qui réinitialise la console pour afficher les infos du moteur graphique
	void ENGINE_INIT_console_engine();

	// Fonction qui renvois le nombres de tiles constructibles par rapport à la résolution de la fenètre courante
	struct Coordonnees ENGINE_POCESS_nombresDeTiles(SDL_Window *window);

	// Fonction qui ajuste la taille de la fenetre de manière à garder toujours le même format en l'occurence 16/9
	void ENGINE_SET_tailleFenetre(const SDL_Event *event,SDL_Window *window,int priorite);

	// Fonction qui initialise tout les composants de la SDL
	void ENGINE_SDL_loadElement();

	// Fonction qui initialise la console
	void ENGINE_INIT_console();

	// Fonction qui contrôle l'état de la fenetre
	void ENGINE_LOG_fenetre(const SDL_Event *event);

	// Fonction qui récupère lesdifférentes variables des tiles
	int get_data_tiles(struct tiles_spec *table);

#endif // ENGINE_H_INCLUDED