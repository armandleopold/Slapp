/* Nom : moteur_jeux.h

Dernier Auteur  : Armand

Creation :
24/01/2014

Dernière modification :
08/03/2014

Description :
    Fichier main

*///

#ifndef DEF_MOTEUR_JEU
#define DEF_MOTEUR_JEU

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Fonction super-global qui gère plusieurs fonctions globales                                                   //
	//                                                                                                               //
	// Cette fonction est le coeur du jeux , elle gère l'interaction utilisateur avec le jeux                        //
	// et se charge de modifier les contextes du jeux                                                                //
	// Elle prend en parametres la fenetre ainsi que le mode d'execution pour le chargement des parametres initiaux  //
	//                                                                                                               //
	// soit 0 en mode parametres de base : création d'une nouvelle partie                                            //
	// soit 1 en mode chargement des parametres initiaux : chargement d'une partie déjà existante                    //
	//                                                                                                               //
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Fonction principale du jeux
	void SUPER_GLOBAL_MOTEUR_JEU(SDL_Window *window,SDL_Renderer *sdlRenderer,int type);

	// Fonction qui associe aux touches presser, un type de déplacement
	int ENGINE_deplacement(struct Element *T_Elements,int compteur_saut,struct state *bouton);

    // Fonction qui récupère les données stockées dans les differents fichiers du jeu
    int ENGINE_Loader(char *nom_de_la_map,struct Map *map,SDL_Surface **tiles);

	// Fonction qui permet de modifier les valeurs des boutons
	struct state GET_Deplacement(SDL_Event event,struct state bouton);

	// Fonction qui permet la création d'une nouvelle partie de jeux
	void JEU_creation_partie(SDL_Window *window,SDL_Rect *T_Rect,struct Joueur *player);

	// Fonction qui gère le Sous menu dans le jeux
	int Sub_Menu(SDL_Window *window,struct state *bouton,struct Joueur player);

	// La barre d'eau
	int DISPLAY_barre(int init_eau,int now_eau, SDL_Window *window);
	
	// Le chrono
	int DISPLAY_decompte(int init_timestamp,SDL_Window *window);

	// Fonction qui enregistre la variable player dans un fichier de sauvegarde
	void sauvegarder_partie(struct Joueur player,SDL_Window *window);

	// Fonction qui charge la variable player à partir d'un fichier de sauvegarde
	void JEU_charger_partie(SDL_Window *window,SDL_Rect *T_Rect,struct Joueur *player);

	// Fonction qui affiche le menu de lancement de partie du jeu
	int DISPLAY_Start_menu(struct Joueur player,SDL_Window *window);

	// Fonction qui affiche le menu de lancement de partie du jeu
	int DISPLAY_win_menu(struct Joueur player,SDL_Window *window);

	// Fonction qui affiche le menu de lancement de partie du jeu
	int DISPLAY_game_over_menu(struct Joueur player,SDL_Window *window);

	// Fonction qui affiche les stats générales d'un fichier de sauvegarde et qui propose la séléctione de map
	struct Joueur selection_map(struct Joueur player,SDL_Window *window, SDL_Rect *T_Rect,struct Map *map);

      /*----------------------------------------------------------------------------------------------*/
												/* Fonctions phase  1 */
	
	int Phase1(SDL_Window *fenetre, SDL_Renderer *Renderer);

	void Nb_aleatoire_a_b(int a, int b, int tab[], int taille);

	SDL_Rect Def_Rect ( int x, int y, int largeur, int hauteur);
	struct Goutte Def_Goutte( SDL_Rect R_Goutte, int vitesse_x, int vitesse_y);

	int Scrolling_Nuage(int compteur_X, int X_aleatoire[], SDL_Rect R_Nuage[]);

	int Collision(int Nb_gouttes, SDL_Rect R_Slappy, SDL_Rect R_Goutte[]);
	     
      /*----------------------------------------------------------------------------------------------*/

#endif
