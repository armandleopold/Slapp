/* Nom : moteur_jeux.c

Dernier Auteur  : Armand

Creation :
24/01/2014

Dernière modification :
08/03/2014

Description :
    Fichier main

*///

#include "../headers/linker.h"

// fonction qui réinitialise l'état des touches pour être sur de ne pas maintenir enfoncé des touches malgrès un changement d'écran
struct state reset_key_map()
{
	struct state btn;

	btn.etat[0] = 0;
	btn.etat[1] = 0;
	btn.etat[2] = 0;
	btn.etat[3] = 0;
	btn.etat[4] = 0;
	btn.etat[5] = 0;
	btn.etat[6] = 0;
	btn.etat[7] = 0;
	btn.etat[8] = 0;
	btn.etat[9] = 0;
	btn.etat[10] = 0;

	return btn;
}

// Fonction qui affiche les stats générales d'un fichier de sauvegarde et qui propose la selection de maps
struct Joueur selection_map(struct Joueur player,SDL_Window *window, SDL_Rect *T_Rect,struct Map *map)
{
	/*----------------------------------------------------------------*/

		int curseur = 0;
	    int i = 0;
	    int current_pos_curseur_stream = 0;
	    int borne_curseur_stream = 0;
	    int exit_while = 0;

		char temps[200];

		struct GUI_object obj[5];
		struct dirent *lecture;
	    struct GUI_object map_list[30];
	    struct Stats stats;

		SDL_Surface *ecran = SDL_GetWindowSurface(window);
		SDL_Surface *background = IMG_Load("artwork/slapp_logo_3.png");
		SDL_Surface *render;

		TTF_Font *police_super_big = TTF_OpenFont("fonts/Roboto-Medium.ttf", 35);
		TTF_Font *police_big = TTF_OpenFont("fonts/Roboto-Medium.ttf", 24);
		TTF_Font *police_small = TTF_OpenFont("fonts/Roboto-Medium.ttf", 12);

	    SDL_Rect position;
	    SDL_Rect fond_blanc = {0,0,ecran->w/2,ecran->h};
	    SDL_Rect separateur = {ecran->w/2,0,1,ecran->h};
	    SDL_Rect pseudo_joueur;

	    SDL_Color gris = {20, 20, 20};

	    DIR *rep;

    /*----------------------------------------------------------------*/

	position.x = ecran->w/2 - background->w/2;
    position.y = ecran->h/2 - background->h/2;

    // On affiche le fond + le fond blanc à gauche avec le séparateur

	SDL_FillRect(ecran,NULL,SDL_MapRGB(ecran->format, 255, 255, 255));
	SDL_BlitSurface(background, NULL, ecran, &position);
    SDL_FillRect(ecran,&fond_blanc,SDL_MapRGB(ecran->format, 255, 255, 255));
    SDL_FillRect(ecran,&separateur,SDL_MapRGB(ecran->format, 150, 150, 150));

    // On affiche le pseudo + les stats

	sprintf(temps,"%s",player.pseudo);

	temps[0] -= 32;

	render = TTF_RenderText_Blended(police_super_big, temps, gris);

	pseudo_joueur.x = ecran->w/4 - render->w/2;
	pseudo_joueur.y = 15;
	pseudo_joueur.w = render->w;
	pseudo_joueur.h = render->h;

	SDL_BlitSurface(render, NULL, ecran, &pseudo_joueur);
	SDL_FreeSurface(render);

	separateur.x = 0;
	separateur.y = 60;
	separateur.w = ecran->w/2;
	separateur.h = 1;

	SDL_FillRect(ecran,&separateur,SDL_MapRGB(ecran->format, 150, 150, 150));

	// On ouvre le fichier du joueur pour charger les stats
	sprintf(temps,"save/%s.save",player.pseudo);
	FILE *fichier = fopen(temps,"rb");

	if (fichier != NULL)
	{
		// on passe la structure joueur pour s'intéresser aux structures stats enregistré en add dans la suite du fichier (si il y en a)
		fseek(fichier,sizeof(struct Joueur),SEEK_SET);

		borne_curseur_stream = ftell(fichier);

		fseek(fichier,-1*(sizeof(struct Stats)),SEEK_END);

		if (ftell(fichier) == borne_curseur_stream)
		{
			exit_while = 1;
		}

		i = 0;
		current_pos_curseur_stream = 1;

		// On va charger et afficher dans la foulé les stats
		while(!feof(fichier) && exit_while == 0)
		{
			fseek(fichier,-1*current_pos_curseur_stream*sizeof(struct Stats),SEEK_END);

			if(ftell(fichier) == (borne_curseur_stream - sizeof(struct Stats)))
			{
				break;
			}

			if(fread(&stats,sizeof(struct Stats),1,fichier) == 1)
			{
				// On affiche les stats de la partie
				sprintf(temps,"Map : %s  |  V : %d / D : %d | BS : %d | Score  : %d",stats.nom_map,stats.nb_victoire,stats.nb_defaite,stats.best_score,stats.score);
				render = TTF_RenderText_Blended(police_small, temps, gris);

				pseudo_joueur.x = ecran->w/4 - render->w/2;
				pseudo_joueur.y = 65 + 3 + i*50;
				pseudo_joueur.w = render->w;
				pseudo_joueur.h = render->h;

				SDL_BlitSurface(render, NULL, ecran, &pseudo_joueur);
				SDL_FreeSurface(render);

				sprintf(temps,"Temps Ph 1 : %d | Temps Ph 2 : %d  | Eau debut : %d | Eau fin : %d ",stats.temps_phase_1,stats.temps_phase_2,stats.eau_debut,stats.eau_fin);
				render = TTF_RenderText_Blended(police_small, temps, gris);

				pseudo_joueur.x = ecran->w/4 - render->w/2;
				pseudo_joueur.y = 65 + 23 + i*50;
				pseudo_joueur.w = render->w;
				pseudo_joueur.h = render->h;

				SDL_BlitSurface(render, NULL, ecran, &pseudo_joueur);
				SDL_FreeSurface(render);

				separateur.y = 90 + 23 + i*50;
				SDL_FillRect(ecran,&separateur,SDL_MapRGB(ecran->format, 150, 150, 150));

				i++;
				current_pos_curseur_stream++;
			}
		}

		if (i == 0)
		{
			// Il n'y a aucune stats de partie enregistrée pour le moment
			sprintf(temps,"Pas encore de stats de parties");
			render = TTF_RenderText_Blended(police_big, temps, gris);

			pseudo_joueur.x = ecran->w/4 - render->w/2;
			pseudo_joueur.y = ecran->h/2;
			pseudo_joueur.w = render->w;
			pseudo_joueur.h = render->h;

			SDL_BlitSurface(render, NULL, ecran, &pseudo_joueur);
			SDL_FreeSurface(render);
		}
	}

	fclose(fichier);

    obj[4].largeur = 250;
    obj[4].hauteur = 100;
    obj[4].pos_x = ecran->w/2 - obj[4].largeur/2;
    obj[4].pos_y = ecran->h/2 - obj[4].hauteur/2;
    obj[4].id = 4;

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
            strcpy(map_list[curseur].text,temps);
            if(curseur == 0)
            {
                map_list[curseur].largeur = 200;
                map_list[curseur].hauteur = 24;
                map_list[curseur].pos_x = ecran->w/2 - obj[4].largeur/2;
                map_list[curseur].pos_y = ecran->h/2 - obj[4].hauteur/2;
                map_list[curseur].id = curseur;
            }
            else
            {
                map_list[curseur].largeur = map_list[curseur - 1].largeur ;
                map_list[curseur].hauteur = map_list[curseur - 1].hauteur ;
                map_list[curseur].pos_x = map_list[curseur - 1].pos_x;
                map_list[curseur].pos_y = map_list[curseur - 1].pos_y + map_list[curseur].hauteur + 3;
                map_list[curseur].id = curseur;
            }
            curseur++;
        }
    }
    closedir(rep);

    if (curseur != 0) // des maps existent
    {
        map_list[29].largeur = 250;
        map_list[29].hauteur = 30;
        map_list[29].pos_y = 50 + map_list[29].hauteur*curseur/2;
        map_list[29].pos_x = 3*ecran->w/4 - map_list[29].largeur/2 ;
        map_list[curseur].id = 999;

        int get_ui = 0;
        get_ui  = GUI_spam(window,map_list[29],4,"Selectionner la map :",NULL,map_list);

        if(get_ui >= 0)
        {
        	sprintf(player.save.nom_map,"map/%s",map_list[get_ui].text);
			srand(time(NULL));

        	// On récupère la map
        	FILE* flux = fopen(player.save.nom_map,"rb");
		        if (flux == NULL)
		        {
		            printf ("// Erreur a l'ouverture de la map\n");
		            player.load = 0;
		        }
		        else
		        {
		            fread(*&map,sizeof(struct Map),1,flux);
		        }
		    fclose(flux);

		    printf("%d:%d\n", map->start.x,map->start.y);

			// On positionne le personnage au point de départ en modifiant ses coordonnées
			player.save.T_Elements[0].dynamique.x =  ecran->w/2;
			player.save.T_Elements[0].dynamique.y =  ecran->h/2;

			player.save.T_Elements[1].dynamique.x = map->start.x*32 - ecran->w/2;
			player.save.T_Elements[1].dynamique.y = map->start.y*32 - ecran->h/2;
        }
        else
        {
        	player.load = 0;
        }
    }
    else
    {
    	GUI_message_box(window,"Il n'existe aucune map pour le moment");
    }

	SDL_FreeSurface(ecran);
	SDL_FreeSurface(background);

	return player;
}

// La barre d'eau
int DISPLAY_barre(int init_eau,int now_eau, SDL_Window *window)
{
	int eau_left_ok = 1;
	int longueur_barre = 0;
	float longueur_barre_float = 500.0;
	float coeff = 0.0;
	float temp = 0.0;

	if (now_eau < 0)
	{
		now_eau = 0;
	}

	temp = (now_eau*100)/init_eau; // on a un pourcentage
	coeff = temp/100; // on a maintenant un coefficient multiplicateur

	longueur_barre_float = 598*coeff;
	longueur_barre = longueur_barre_float;

	if (longueur_barre == 0)
	{
		eau_left_ok = 0;
	}

	SDL_Rect cadre_barre;
	SDL_Rect fond_barre;
	SDL_Rect barre;

	SDL_Surface *ecran = SDL_GetWindowSurface(window);

	cadre_barre.x = ecran->w/2 - 300;
	cadre_barre.y = 5;
	cadre_barre.h = 15;
	cadre_barre.w = 600;

	fond_barre.x = cadre_barre.x + 1;
	fond_barre.y = cadre_barre.y + 1;
	fond_barre.h = cadre_barre.h - 2;
	fond_barre.w = cadre_barre.w - 2;

	barre.x = fond_barre.x;
	barre.y = fond_barre.y;
	barre.h = fond_barre.h;
	barre.w = longueur_barre;

	SDL_FillRect(ecran,&cadre_barre,SDL_MapRGB(ecran->format, 33, 33, 33));
	SDL_FillRect(ecran,&fond_barre,SDL_MapRGB(ecran->format, 83, 83, 83));
	SDL_FillRect(ecran,&barre,SDL_MapRGB(ecran->format, 0, 183, 243));

	return eau_left_ok;
}

// Fonction qui calcul le temps entre le temps passé en parametre et le temps actuel
int calcul_time(int init_timestamp)
{
    int itime = 0;

	itime = SDL_GetTicks(); // on récupère le temps

	itime = itime - init_timestamp; // on obtiens la différence de temps entre les deux temps
	// le temps qui s'est écoulé entre le init_timestamp et le lancement de la fonction decompte

	itime = (itime - itime%1000)/1000; // on simplifie à la seconde près

    return itime;
}

void DISPLAY_int(int param,struct SDL_Window *window)
{
	SDL_Surface *ecran   = SDL_GetWindowSurface(window);
    SDL_Surface *render;

    char temp[200];

    SDL_Rect position;

    TTF_Font *light = TTF_OpenFont("fonts/Roboto-Medium.ttf", 50);
    SDL_Color blanc = {50, 50, 50};

	sprintf(temp,"%d",param*100);
	render = TTF_RenderText_Blended(light, temp,blanc);

	// On position le chrono en haut au centre de l'ecran
	position.x = ecran->w/2 - render->w/2;
	position.y = 20;
	position.h = render->h;
	position.w = render->w;

    SDL_BlitSurface(render, NULL, ecran, &position);

    SDL_FreeSurface(render);
    SDL_FreeSurface(ecran);
    TTF_CloseFont(light);
}

// Le chrono
int DISPLAY_decompte(int init_timestamp,SDL_Window *window)
{
	SDL_Surface *ecran   = SDL_GetWindowSurface(window);
    SDL_Surface *render;

    SDL_Rect position;

    TTF_Font *light = TTF_OpenFont("fonts/Roboto-Medium.ttf", 50);
    SDL_Color blanc = {50, 50, 50};

    int itime = 0;

    int minute = 0;
    int seconde = 0;

    int dif_minute = 0;
    int dif_seconde = 0;

    int time_left_ok = 0;

    char temp[200];

	itime = SDL_GetTicks(); // on récupère le temps

	// on calcul la valeur en minute et en seconde à afficher

	minute = (CHRONO - CHRONO%60)/60;
	seconde = CHRONO%60;

	itime = itime - init_timestamp; // on obtiens la différence de temps entre les deux temps
	// le temps qui s'est écoulé entre le init_timestamp et le lancement de la fonction decompte

	itime = (itime - itime%1000)/1000; // on simplifie à la seconde près

	// maintenant on a notre décalage en secondes

	dif_minute =  (itime - itime%60)/60; // on a la différence en minutes
	dif_seconde = itime%60; // on a la différence en secondes

	// finalement il faut soustraire les différences au temps initial et on obitent le décompte

	minute -= dif_minute;
	seconde -= dif_seconde;


	// On obtient en minute et seconde le temps restant
	if (minute <= 0
	&&	seconde <= 0 )
	{
		time_left_ok = 1;
	}
	else if (seconde < 0)
	{
		minute--;
		seconde += 60;
	}
	else if (minute < 0)
	{
		minute = 0;
	}

	sprintf(temp,"%d : %d",minute,seconde);
	render = TTF_RenderText_Blended(light, temp,blanc);

	// On position le chrono en haut au centre de l'ecran
	position.x = ecran->w/2 - render->w/2;
	position.y = 20;
	position.h = render->h;
	position.w = render->w;

    SDL_BlitSurface(render, NULL, ecran, &position);

    SDL_FreeSurface(render);
    SDL_FreeSurface(ecran);
    TTF_CloseFont(light);

    return time_left_ok;
}

// Fonction principale du jeux
void SUPER_GLOBAL_MOTEUR_JEU(SDL_Window *window,SDL_Renderer *sdlRenderer,int type)
{
	if((type == 1)
	|| (type == 0)) // Verification du type de lancement du moteur du jeux
	{

	    /*------------------------------------------------------------------------------------------------------------*/

	        int quitter = 0;                 // variable qui gère l'arret de la boucle de fonctionnement
	        int exit_value = 0;
        	int compteur_saut = 0;           // Variable qui compte le nombre de saut successifs
        	int init_timestamp = 0; 		 // Variable qui contient la valeur unix du temps de lancement de la phase de jeux
        	int init_eau = 0;
			int i = 0;
			int state_jouer = 0;
			int leave_while = 0;
			int load_stats = 0;

			char temps[200];

			float temp_score = 0.0;

	        struct Joueur player;            // Variable qui contient toutes les données relative aux joueur
	        struct Map map;                  // la variable qui contient toutes les informations concernant la map
	        struct state btn;                // Structure qui agrège les inputs
	        struct tiles_spec tiles_spec[200];
			struct Stats stats;

	        SDL_Surface *tiles[100];         // Le tableau qui contient toutes les tiles différentes

	        Uint32 start;                    // le timer qui permet de définir une notion de temps au jeu

        	SDL_Rect T_Rect[10];             // Rect utiliser pour délimiter des zones d'affichage dans la fenetre

	        SDL_Event event;                 // Le handle d'evenements

	    /*------------------------------------------------------------------------------------------------------------*/

		if (type == 0) // On créer une nouvelle partie
		{
			JEU_creation_partie(window,T_Rect,&player);
		}
		else if(type == 1) // On charge une partie
		{
			JEU_charger_partie(window,T_Rect,&player);
			init_eau = player.save.eau;
		}

		// On charge le niveau correspondant

		if(player.load != 0) // On continue si le chargement s'est bien déroulé
		{

			player.save.phase = 0;

			// setting des variables avant la boucle infinie
			if(player.save.phase != 0)
			{
				SDL_ShowCursor(0); // On desactive le curseur
			}

			ANIM_fade_out(window);

			btn.etat[10] = 0;

			init_timestamp = SDL_GetTicks();

			while(((quitter == 0) || (btn.etat[10] == 0))
			&& (exit_value == 0)) // Boucle global du jeu
			{
				start = SDL_GetTicks(); // on récupère le temps

				if(player.save.phase == 0) // Phase 0 de jeux, Menu récapitulatif des victoires/défaites du joueur et lancement d'une nouvelle session de jeux
				{
					state_jouer = 0;
					leave_while = 0;

					if (player.load != 0)
					{
						strcpy(temps,player.save.nom_map);

						player = selection_map(player,window,T_Rect,&map);

						if (strcmp(temps,player.save.nom_map) != 0)
						{
							load_stats = 0;
						}

						if (player.load != 0)
						{
							state_jouer = 1;
							ANIM_fade_out(window);
						}
					}

					if (ENGINE_Loader(player.save.nom_map,&map,tiles) == 0
					&& get_data_tiles(tiles_spec) > 0
					&& state_jouer != 0 )
					{

						if (load_stats == 0)
						{
							player.stats.nb_victoire = 0;
							player.stats.nb_defaite = 0;
							player.stats.best_score = 0;

							sprintf(temps,"save/%s.save",player.pseudo);

							FILE *fichier = fopen(temps,"rb");

							fseek(fichier,sizeof(struct Joueur),SEEK_SET);

							// On récupère le meilleur score sur cette map
							while(!feof(fichier))
							{
							    if(fread(&stats,sizeof(struct Stats),1,fichier) == 1)
							    {
							    	if (strcmp(stats.nom_map,player.save.nom_map) == 0)
							    	{
							    		if (stats.score >= player.stats.score
							    		&& stats.score >= player.stats.best_score)
							    		{
							    			player.stats.best_score = stats.score;
							    		}

							    		if (stats.nb_victoire == 1)
							    		{
							    			player.stats.nb_victoire++;
							    		}
							    		else if (stats.nb_defaite == 1)
							    		{
							    			player.stats.nb_defaite++;
							    		}

							    		i++;
							    	}
							    }
							}
							fclose(fichier);

							load_stats = 1;
						}

					    while(leave_while == 0)
					    {
					        start = SDL_GetTicks(); // on récupère le temps
					    	state_jouer = DISPLAY_Start_menu(player,window);

					        while(SDL_PollEvent(&event))
					        {
					        	switch(event.type)
					            {
					         		case SDL_KEYDOWN: // Relâchement d'une touche
					                    if(event.key.keysym.sym == SDLK_ESCAPE)
					                    {
					                        leave_while = 1;
					                        quitter = 1;
					                        ANIM_fade_out(window);
					                    }
					                	break;

					         		case SDL_MOUSEBUTTONDOWN: // clic de  souris
					                	if (state_jouer == 1)
					                	{
					                		player.save.phase = 1;
					                		leave_while = 1;
					                		SDL_ShowCursor(0); // On desactive le curseur
					                		ANIM_fade_out(window);
					                	}
					                	break;

					                case SDL_QUIT: // Clic sur la croix
					                        leave_while = 1;
					                        quitter = 1;
					                        ANIM_fade_out(window);
					 					break;
					            }
					        }

					        SDL_UpdateWindowSurface(window);

					        if(1000/IMAGE_PAR_SECONDES > SDL_GetTicks()-start)
					        {
					            SDL_Delay(1000/IMAGE_PAR_SECONDES-(SDL_GetTicks()-start));
					        }
						}

						init_timestamp = SDL_GetTicks();
						btn = reset_key_map();
					}
					else
					{
						quitter = 1;
						exit_value = 1;
					}
				}
				else if (player.save.phase == 1) // Phase de jeux N°1 on doit agréger un maximum de gouttes d'eau
				{

					// Valeurs temp
					player.save.eau = Phase1(window,sdlRenderer);
					player.save.eau = player.save.eau*100;
					player.save.phase = 2;

					// stats partie actuelle
					strcpy(player.stats.nom_map,player.save.nom_map);
					player.stats.temps_phase_1 = calcul_time(init_timestamp);

					player.stats.eau_debut = player.save.eau;

					// valeurs pour la phase suivante
					init_eau = player.save.eau;
				}
				else if (player.save.phase == 2) // Phase de jeux N°2 on doit rejoindre la source.
				{
					// On récupère les inputs
					btn = GET_Deplacement(event,btn);

		            // En fonction des inputs on modifie les valeurs des éléments et on modifie compteur_saut ou pas
				    compteur_saut = ENGINE_deplacement(player.save.T_Elements,compteur_saut,&btn);

		            // On fait appel au moteur physique
		            exit_value = GLOBAL_MOTEUR_PHYSIQUE(SDL_GetWindowSurface(window),&map,&player.save,T_Rect,tiles_spec);

		            // On fait appel au moteur graphique
		            exit_value = GLOBAL_MOTEUR_GRAPHIQUE(window,sdlRenderer,&map,&player,tiles,T_Rect,init_timestamp,init_eau);

					if (player.save.phase == 3
						|| player.save.phase == 4)
					{
						ANIM_fade_out(window);
					}

		            // Fonction qui ajoute des informations concernant le moteur de collisions
				    //Debug_collision(map,window,T_Rect,player.save.T_Elements,collision);
				}
				else if (player.save.phase == 3) // Phase de jeux N°3 ecran de game over : retour sur la phase 0
				{
					state_jouer = 0;
					leave_while = 0;

					player.stats.temps_phase_2 = calcul_time(init_timestamp);
					player.stats.eau_fin = player.save.eau;
					player.stats.score = 0;
					player.stats.nb_victoire = 0;
					player.stats.nb_defaite = 1;

					sprintf(temps,"save/%s.save",player.pseudo);
					FILE *fichier = fopen(temps,"ab");

					if (fichier != NULL)
					{
						 fwrite(&player.stats,sizeof(struct Stats),1,fichier);
					}

					fclose(fichier);

					fichier = fopen(temps,"rb");

					fseek(fichier,sizeof(struct Joueur),SEEK_SET);

					// On récupère le meilleur score sur cette map
					while(!feof(fichier))
					{
					    if(fread(&stats,sizeof(struct Stats),1,fichier) == 1)
					    {
					    	if (strcmp(stats.nom_map,player.stats.nom_map) == 0)
					    	{
					    		if (stats.score >= player.stats.score
					    		&& stats.score >= player.stats.best_score)
					    		{
					    			player.stats.best_score = stats.score;
					    		}
					    		i++;
					    	}
					    }
					}
					fclose(fichier);

				    while(leave_while == 0)
				    {
				    	state_jouer = DISPLAY_game_over_menu(player,window);

				        start = SDL_GetTicks(); // on récupère le temps

				        SDL_WaitEvent(&event);
				        while(SDL_PollEvent(&event)); // Récupération des actions de l'utilisateur(while pour une bonne utilisation)
				        {
				        	switch(event.type)
				            {
				                case SDL_QUIT: // Clic sur la croix
				                        leave_while = 1;
				                        player.save.phase = 0;
				                        quitter = 1;
				                        ANIM_fade_out(window);
				 					break;

				                case SDL_KEYDOWN: // Relâchement d'une touche
				                    if(event.key.keysym.sym == SDLK_ESCAPE)
				                    {
				                        leave_while = 1;
				                        player.save.phase = 0;
				                        quitter = 1;
				                        ANIM_fade_out(window);
				                    }
				                	break;

				                case SDL_MOUSEBUTTONDOWN: // clic de la souris
				                	if (state_jouer == 1)
				                	{
				                		player.save.phase = 0;
				                		leave_while = 1;
				                		ANIM_fade_out(window);
				                	}
				                	break;
				            }
				        }

				        SDL_UpdateWindowSurface(window);

				        if(1000/IMAGE_PAR_SECONDES > SDL_GetTicks()-start)
				        {
				            SDL_Delay(1000/IMAGE_PAR_SECONDES-(SDL_GetTicks()-start));
				        }
					}

					btn = reset_key_map();
				}
				else if (player.save.phase == 4) // Phase de jeux N°4 ecran de win : retour sur la phase 0
				{
					state_jouer = 0;
					leave_while = 0;

					player.stats.temps_phase_2 = calcul_time(init_timestamp);
					player.stats.eau_fin = player.save.eau;

					/**/////////////////////////////////////////////////////////////// Calcul du Score //////////////////////////////////////////////////////////**/
					/**/
                    /**/
					/**/	temp_score = (player.stats.eau_debut + player.stats.eau_fin)*5/(player.stats.temps_phase_1*0.3+1)
									    + (player.stats.eau_debut + player.stats.eau_fin)*10/(player.stats.temps_phase_2*0.8+1);	/**/
				    /**/                                                                                                                                     	/**/
					/**/	if (player.stats.eau_debut == player.stats.eau_fin)                                                                              	/**/
					/**/	{                                                                                                                                	/**/
					/**/		temp_score = temp_score*2;                                                                                                   	/**/
					/**/	}                                                                                                                                	/**/
				    /**/                                                                                                                                     	/**/
					/**/	player.stats.score = temp_score;                                                                                                 	/**/
					/**/                                                                                                                                     	/**/
					/**/////////////////////////////////////////////////////////////// Calcul du Score //////////////////////////////////////////////////////////**/

					player.stats.nb_victoire = 1;
					player.stats.nb_defaite = 0;

					sprintf(temps,"save/%s.save",player.pseudo);
					FILE *fichier = fopen(temps,"ab");

					if (fichier != NULL)
					{
						 fwrite(&player.stats,sizeof(struct Stats),1,fichier);
					}

					fclose(fichier);

					fichier = fopen(temps,"rb");

					fseek(fichier,sizeof(struct Joueur),SEEK_SET);

					// On récupère le meilleur score sur cette map
					while(!feof(fichier))
					{
					    if(fread(&stats,sizeof(struct Stats),1,fichier) == 1)
					    {
					    	if (strcmp(stats.nom_map,player.stats.nom_map) == 0)
					    	{
					    		if (stats.score >= player.stats.score
					    		&& stats.score >= player.stats.best_score)
					    		{
					    			player.stats.best_score = stats.score;
					    		}
					    		i++;
					    	}
					    }
					}
					fclose(fichier);

					if (i == 0)
					{
						player.stats.best_score = player.stats.score;
					}

				    while(leave_while == 0)
				    {
				    	state_jouer = DISPLAY_win_menu(player,window);

				        start = SDL_GetTicks(); // on récupère le temps

				        SDL_WaitEvent(&event);
				        while(SDL_PollEvent(&event)); // Récupération des actions de l'utilisateur(while pour une bonne utilisation)
				        {
				        	switch(event.type)
				            {
				                case SDL_QUIT: // Clic sur la croix
				                        leave_while = 1;
				                        player.save.phase = 0;
				                        quitter = 1;
				                        ANIM_fade_out(window);
				 					break;

				                case SDL_KEYDOWN: // Relâchement d'une touche
				                    if(event.key.keysym.sym == SDLK_ESCAPE)
				                    {
				                        leave_while = 1;
				                        player.save.phase = 0;
				                        quitter = 1;
				                        ANIM_fade_out(window);
				                    }
				                	break;

				                case SDL_MOUSEBUTTONDOWN: // clic de la souris
				                	if (state_jouer == 1)
				                	{
				                		player.save.phase = 0;
				                		leave_while = 1;
				                		ANIM_fade_out(window);
				                	}
				                	break;
				            }
				        }

				        SDL_UpdateWindowSurface(window);

				        if(1000/IMAGE_PAR_SECONDES > SDL_GetTicks()-start)
				        {
				            SDL_Delay(1000/IMAGE_PAR_SECONDES-(SDL_GetTicks()-start));
				        }
					}

					btn = reset_key_map();
				}
				else // erreur de phase de jeux retour sur la phase 0
				{
					GUI_message_box(window,"Erreur de phase de jeu");
					player.save.phase = 0;
					quitter = 1;
				}

	            if (btn.etat[10] == 1)
	            {
	            	if(Sub_Menu(window,&btn,player) == 1)
	            	{
	            		// retour à la phase 0
	            		player.save.phase = 0;
	            		btn.etat[10] = 0;
	            		ANIM_fade_out(window);
	            	}
	            }

			    SDL_UpdateWindowSurface(window);

	            if(1000/IMAGE_PAR_SECONDES > SDL_GetTicks()-start)
	            {
	                SDL_Delay(1000/IMAGE_PAR_SECONDES-(SDL_GetTicks()-start));
	            }
			}

			ANIM_fade_out(window);
			SDL_ShowCursor(1); // On réaffiche le curseur de la souris
		}
		else
		{
			ANIM_fade_out(window);
			printf("// Erreur de chargement des donnees stockees\n");
		}
	}
	else // Erreur de type
	{
		printf("// Erreur de définition des parametres d'initialisation du moteur du jeu.\n");
	}
}

// Fonction qui associe aux touches presser, un type de déplacement
int ENGINE_deplacement(struct Element *T_Elements,int compteur_saut,struct state *btn)
{
    if(btn->etat[0] == 1
    || btn->etat[4] == 1
    || btn->etat[8] == 1) // Deplacement Vertical
    {
	    if(compteur_saut == 0 && T_Elements[0].dynamique.vy == 0)
	    {
	        T_Elements[0].dynamique.vy = -800;
	        compteur_saut++;
	    }
	    else if (compteur_saut == 1 && T_Elements[0].dynamique.vy != 0)
	    {
	        T_Elements[0].dynamique.vy = -800;
	        compteur_saut++;
	    }
    }

    if(btn->etat[1] == 1
    || btn->etat[6] == 1
    || btn->etat[3] == 1
    || btn->etat[7] == 1) // Deplacement Horizontal
    {
	    if(btn->etat[1] == 1
	    || btn->etat[6] == 1) // Droite
	    {
	        T_Elements[0].dynamique.vx = -400;
	    }
	    else if(btn->etat[3] == 1
	    ||	    btn->etat[7] == 1) // Droite
	    {
	        T_Elements[0].dynamique.vx = 400;
	    }
    }
    else
    {
	    T_Elements[0].dynamique.vx = 0;
    }

    if (T_Elements[0].dynamique.vy == 0)
    {
        compteur_saut = 0;
    }

    return compteur_saut;
}

// Fonction qui récupère les données stockées dans les differents fichiers du jeu
int ENGINE_Loader(char *nom_de_la_map,struct Map *map,SDL_Surface **tiles)
{
    int return_value = 0;
    int i = 0;

    char path[50]="";

    // On réupère la map

    FILE* flux = fopen(nom_de_la_map,"rb");
        if (flux == NULL)
        {
            printf ("// Erreur a l'ouverture de la map\n");
            return_value = 1;
        }
        else
        {
            fread(&*map,sizeof(struct Map),1,flux);
        }
    fclose(flux);

    // On ouvre le fichier de spécification des tiles et on charge les images des tiles

    FILE* flux2 = fopen("map/tiles_config.slapp","rb");
        if (flux2 == NULL)
        {
            printf ("// Erreur a l'ouverture du fichier de specification des tiles\n");
            return_value = 1;
        }
        else
        {
        	printf("\n\t Chargement des tiles...\n");
            while(fread(&map->tiles_spec_T[i],sizeof(struct tiles_spec),1,flux2) && !feof(flux2))
            {
            	sprintf(path,"tiles/%s",map->tiles_spec_T[i].image);
            	tiles[i] = IMG_Load(path);
                i++;
            }
        }
    fclose(flux2);

    return return_value;
}

// Fonction qui permet de modifier les valeurs des btns
struct state GET_Deplacement(SDL_Event event,struct state btn)
{
	while(SDL_PollEvent(&event)) // Récupération des actions de l'utilisateur
	{
	    switch(event.type)
	    {
	        case SDL_KEYDOWN:
	                switch(event.key.keysym.sym)
	                {
	                	case SDLK_ESCAPE :
	                		btn.etat[10] = 1;
	                		break;
	                    case SDLK_z:
	                        btn.etat[0] = 1;
	                        break;
	                    case SDLK_q:
	                        btn.etat[1] = 1;
	                        break;
	                    case SDLK_s:
	                        btn.etat[2] = 1;
	                        break;
	                    case SDLK_d:
	                        btn.etat[3] = 1;
	                        break;
	                    case SDLK_UP:
	                        btn.etat[4] = 1;
	                        break;
	                    case SDLK_DOWN:
	                        btn.etat[5] = 1;
	                        break;
	                    case SDLK_LEFT:
	                        btn.etat[6] = 1;
	                        break;
	                    case SDLK_RIGHT:
	                        btn.etat[7] = 1;
	                        break;
	                    case SDLK_SPACE:
	                        btn.etat[8] = 1;
	                        break;
	                    case SDLK_F3:
	                        if (btn.etat[9] == 0)
	                        {
	                            btn.etat[9] = 1;
	                        }
	                        else
	                        {
	                            btn.etat[9] = 0;
	                        }
	                        break;
	                }
	            break;
	        case SDL_KEYUP:
	            switch(event.key.keysym.sym)
	            {
	                	case SDLK_ESCAPE :
	                		btn.etat[10] = 0;
	                		break;
	                    case SDLK_z:
	                        btn.etat[0] = 0;
	                        break;
	                    case SDLK_q:
	                        btn.etat[1] = 0;
	                        break;
	                    case SDLK_s:
	                        btn.etat[2] = 0;
	                        break;
	                    case SDLK_d:
	                        btn.etat[3] = 0;
	                        break;
	                    case SDLK_UP:
	                        btn.etat[4] = 0;
	                        break;
	                    case SDLK_DOWN:
	                        btn.etat[5] = 0;
	                        break;
	                    case SDLK_LEFT:
	                        btn.etat[6] = 0;
	                        break;
	                    case SDLK_RIGHT:
	                        btn.etat[7] = 0;
	                        break;
	                    case SDLK_SPACE:
	                        btn.etat[8] = 0;
	                        break;
	            }
	            break;
	    }
	}

	return btn;
}

// Fonction qui permet la création d'une nouvelle partie de jeux
void JEU_creation_partie(SDL_Window *window,SDL_Rect *T_Rect,struct Joueur *player)
{

	/*-------------------------------------------------------------------------------*/

		struct GUI_object obj[5];
		SDL_Surface *fenetre = SDL_GetWindowSurface(window);
		SDL_Surface *background = IMG_Load("artwork/abstract-background-1.png");
	    DIR *rep;
		struct dirent *lecture;
		int match = 0;
		int i = 0;
		char temp[200]; // variables de constructions de chaines de caractères
		char temp2[200]; // variables de constructions de chaines de caractères

	/*-------------------------------------------------------------------------------*/

	//SDL_BlitSurface(background,&center,fenetre,NULL);

	ANIM_fade_in(window,background);
	SDL_UpdateWindowSurface(window);

    obj[0].largeur = 250;
    obj[0].hauteur = 100;
    obj[0].pos_x = fenetre->w/2 - obj[0].largeur/2;
    obj[0].pos_y = fenetre->h/2;
    obj[0].id = 0;

	if(GUI_spam(window,obj[0],3,"Entrer Votre pseudo :",player->pseudo,NULL) == 1
	&& player->pseudo[0] != '\0')
    {
    	if((rep = opendir("save/")))
	    {
		    sprintf(temp,"%s.save",player->pseudo);

		    while ((lecture = readdir(rep)))
		    {
		        printf("%s\n", temp2);
		        if(strcmp(temp,lecture->d_name) == 0)
		        {
		        	match = 1;
		        }
		    }
		    closedir(rep);

		    if (match != 1)
		    {
		    	sprintf(temp,"save/%s.save",player->pseudo);

		    	FILE* flux = fopen(temp,"ab");
		        if (flux == NULL)
		        {
		            printf ("// Erreur lors de la creation du fichier de sauvegarde du joueur, verifier les droits d'acces en lecture et ecriture du support de stockage courant.\n");
		        }
		        else
		        {

		        	// Initialisation des valeurs des variables du fichier de sauvegarde

		        	for ( i = 0; i < 20; ++i)
		        	{
		        			player->save.T_Elements[i].dynamique.x  = 0;
							player->save.T_Elements[i].dynamique.y  = 0;
							player->save.T_Elements[i].dynamique.vx = 0;
							player->save.T_Elements[i].dynamique.vy = 0;
							player->save.T_Elements[i].dynamique.ax = 0;
							player->save.T_Elements[i].dynamique.ay = 0;

							player->save.T_Elements[i].type_element = 0;
							player->save.T_Elements[i].etat_element = 0;
		        	}

					// On initialise les attribus dynamiques du héro
					player->save.T_Elements[0].dynamique.x  = fenetre->w/2;
					player->save.T_Elements[0].dynamique.y  = fenetre->h/2;
					player->save.T_Elements[0].dynamique.vx = 0;
					player->save.T_Elements[0].dynamique.vy = 0;
					player->save.T_Elements[0].dynamique.ax = 0;
					player->save.T_Elements[0].dynamique.ay = 0;

					// On initialise les attribus dynamiques de la caméra
					player->save.T_Elements[1].dynamique.x  = 0;
					player->save.T_Elements[1].dynamique.y  = 0;
					player->save.T_Elements[1].dynamique.vx = 0;
					player->save.T_Elements[1].dynamique.vy = 0;
					player->save.T_Elements[1].dynamique.ax = 0;
					player->save.T_Elements[1].dynamique.ay = 0;

					player->save.T_Elements[1].type_element = -1;

					// Perso
					T_Rect[0].x = player->save.T_Elements[0].dynamique.x;
					T_Rect[0].y = player->save.T_Elements[0].dynamique.y;

					T_Rect[0].w = 40;
					T_Rect[0].h = 34;

					// Camera
					T_Rect[1].x = player->save.T_Elements[1].dynamique.x;
					T_Rect[1].y = player->save.T_Elements[1].dynamique.y;

					player->load = 1;
		            fwrite(player,sizeof(struct Joueur),1,flux);
		        }
			    fclose(flux);
		    }
		    else
		    {
		    	GUI_message_box(window,"Ce nom d'utilisateur existe deja");
		    }
	    }
	    else
	    {
	        printf("// Erreur d'ouverture du dossier de Sauvegarde \n");
	    }
    }
    else
    {
    	player->load = 0;
    	printf("// Erreur de recuperation du nom du joueur\n");
    }
}

// Fonction qui gère le Sous menu dans le jeux
int Sub_Menu(SDL_Window *window,struct state *btn,struct Joueur player)
{
	/*------------------------------------------------------*/

		struct GUI_object obj[5];
		struct state etat[5];
		int return_value = 0;
		int quit = 0;
        int curseur = -1;
		Uint32 start;
		SDL_Event event;
		SDL_Surface *ecran = SDL_GetWindowSurface(window);

	    SDL_Surface *background;
	    Uint32 rmask, gmask, bmask, amask;

	/*------------------------------------------------------*/

	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;

    background = SDL_CreateRGBSurface(0, ecran->w, ecran->h, 32,
                                   rmask, gmask, bmask, amask);

	SDL_ShowCursor(1);

    obj[0].largeur = 350;
    obj[0].hauteur = 30;
    obj[0].pos_x = ecran->w/2 - obj[0].largeur/2;
    obj[0].pos_y = ecran->h/2 - obj[0].hauteur/2;
    obj[0].id = 0;

    obj[1].largeur = 350;
    obj[1].hauteur = 30;
    obj[1].pos_x = ecran->w/2 - obj[1].largeur/2;
    obj[1].pos_y = ecran->h/2 + 75;
    obj[1].id = 1;

    obj[2].largeur = 350;
    obj[2].hauteur = 30;
    obj[2].pos_x = ecran->w/2 - obj[2].largeur/2;
    obj[2].pos_y = ecran->h/2 + 30;
    obj[2].id = 2;

    GUI_DISPLAY_button(obj[0],ecran,"Sauvegarder la Partie");
    GUI_DISPLAY_button(obj[1],ecran,"Quitter (Ne sauvegarde pas automatiquement)");
    GUI_DISPLAY_button(obj[2],ecran,"Reprendre la Partie");

    while(quit==0)
    {
        start = SDL_GetTicks(); // on récupère le temps

        while(SDL_PollEvent(&event)); // Récupération des actions de l'utilisateur(while pour une bonne utilisation)
        {
            switch(event.type)
            {
            	case SDL_KEYDOWN:
	                switch(event.key.keysym.sym)
	                {
	                	case SDLK_ESCAPE : // reprendre le jeux
	                        	quit = 1;
				            	btn->etat[10] = 0;
				            	SDL_ShowCursor(0);
	                		break;
	                	case SDLK_UP:
                            curseur--;
                            if (curseur < 0)
                            {
                                curseur = 0;
                            }
                            break;
                        case SDLK_DOWN:
                            curseur++;
                            if (curseur >= NBR_BOUTON)
                            {
                                curseur = NBR_BOUTON-1;
                            }
                            break;
                        case SDLK_z:
                            curseur--;
                            if (curseur < 0)
                            {
                                curseur = 0;
                            }
                            break;
                        case SDLK_s:
                            curseur++;
                            if (curseur >= NBR_BOUTON)
                            {
                                curseur = NBR_BOUTON-1;
                            }
                            break;
                        case SDLK_1:
                            curseur = 0;
                            break;
                        case SDLK_2:
                            curseur = 1;
                            break;
                        case SDLK_3:
                            curseur = 2;
                            break;
                        case SDLK_4:
                            curseur = 3;
                            break;
                        case SDLK_5:
                            curseur = 4;
                            break;
                        case SDLK_RETURN:

                            break;
	                }
		            break;

                case SDL_QUIT: // Clic sur la croix
                {
                    quit=1;
                    return_value = 1;
                }

                case SDL_MOUSEMOTION: // Mouvement de la souris
                {
                    SDL_BlitSurface(background,NULL,ecran,NULL);

                    etat[0] = GUI_DISPLAY_button(obj[0],ecran,"Sauvegarder la Partie");
                    etat[1] = GUI_DISPLAY_button(obj[1],ecran,"Quitter (Ne sauvegarde pas automatiquement)");
					etat[2] = GUI_DISPLAY_button(obj[2],ecran,"Reprendre la Partie");

                }

                case SDL_MOUSEBUTTONDOWN: // clic de la souris
                {
                    if (event.button.button == SDL_BUTTON_LEFT)
                    {
                        if(etat[0].etat[0] == 1) // Sauvegarder
                        {
                        	sauvegarder_partie(player,window);
                        }
                        else if (etat[1].etat[0] == 1) // Quitter le jeux
                        {
                        	quit = 1;
                        	return_value = 1;
                        }
                        else if (etat[2].etat[0] == 1) // Reprendre le jeux
                        {
                        	quit = 1;
			            	btn->etat[10] = 0;
			            	SDL_ShowCursor(0);
                        }

                        SDL_BlitSurface(background,NULL,ecran,NULL);

                        GUI_DISPLAY_button(obj[0],ecran,"Sauvegarder la Partie");
                        GUI_DISPLAY_button(obj[1],ecran,"Quitter (Ne sauvegarde pas automatiquement)");
                        GUI_DISPLAY_button(obj[2],ecran,"Reprendre la Partie");

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

    return return_value;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* NEUTRALISER CAR PEUT CONDUIRE A LA SUPPRESSION DES DONNEES DE STATS DES JOUEURS DANS LES FICHIERS DE SAUVEGARDE */
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Fonction qui enregistre la variable player dans un fichier de sauvegarde
void sauvegarder_partie(struct Joueur player,SDL_Window *window)
{
	/*-------------------------------------------------------------------------------*/

	    /*DIR *rep;
		char temp[200]; // variables de constructions de chaines de caractères*/

	/*-------------------------------------------------------------------------------*/

    /*if((rep = opendir("save/")))
	{
	    sprintf(temp,"save/%s.save",player.pseudo);

	    FILE* flux = fopen(temp,"wb");
	    if (flux == NULL)
	    {
	        printf ("// Erreur lors de la creation du fichier de sauvegarde du joueur, verifier les droits d'acces en lecture et ecriture du support de stockage courant.\n");
	    }
	    else
	    {
	        // Initialisation des valeurs des variables du fichier de sauvegarde
	        fwrite(&player,sizeof(struct Joueur),1,flux);
	    }
		fclose(flux);
	}
	else
	{
	    printf("// Erreur d'ouverture du dossier de Sauvegarde \n");
	}*/
}

// Fonction qui charge la variable player à partir d'un fichier de sauvegarde
void JEU_charger_partie(SDL_Window *window,SDL_Rect *T_Rect,struct Joueur *player)
{
	/*-------------------------------------------------------------------------------*/

		struct GUI_object obj[5];
		SDL_Surface *fenetre = SDL_GetWindowSurface(window);
		SDL_Surface *background = IMG_Load("artwork/abstract-background-1.png");
	    DIR *rep;
		struct dirent *lecture;
		int i =0;
		struct GUI_object save[20];
		int curseur = 0;

		char temp[200]; // variables de constructions de chaines de caractères
		char temps[200];

	/*-------------------------------------------------------------------------------*/

	ANIM_fade_in(window,background);
	SDL_UpdateWindowSurface(window);

    obj[0].largeur = 250;
    obj[0].hauteur = 100;
    obj[0].pos_x = fenetre->w/2 - obj[0].largeur/2;
    obj[0].pos_y = fenetre->h/2 - obj[0].hauteur/2;
    obj[0].id = 0;

	if((rep = opendir("save/")))
	{
	    while ((lecture = readdir(rep)))
	    {
	        i = 0;

	        strcpy(temps,lecture->d_name);

	        while(temps[i] != '.')
	        {
	            i++;
	        }

	        if (temps[i+1] == 's'
	        && temps[i+2] == 'a'
	        && temps[i+3] == 'v')
	        {
	            strcpy(save[curseur].text,temps);
	            if(curseur == 0)
	            {
	                save[curseur].largeur = 200;
	                save[curseur].hauteur = 24;
	                save[curseur].pos_x = fenetre->w/2;
	                save[curseur].pos_y = fenetre->h/2;
	                save[curseur].id = curseur;
	            }
	            else
	            {
	                save[curseur].largeur = save[curseur - 1].largeur ;
	                save[curseur].hauteur = save[curseur - 1].hauteur ;
	                save[curseur].pos_x = save[curseur - 1].pos_x;
	                save[curseur].pos_y = save[curseur - 1].pos_y + save[curseur].hauteur + 3;
	                save[curseur].id = curseur;
	            }
	            curseur++;
	        }
	    }
	    closedir(rep);

	    if (curseur != 0) // des save existent
	    {
	        save[50].largeur = 250;
	        save[50].hauteur = 30;
	        save[50].pos_y = fenetre->h/2 - save[50].hauteur*curseur/2;
	        save[50].pos_x = fenetre->w/2 - save[50].largeur/2;
	        save[curseur].id = 999;

	        int get_ui = 0;
	        get_ui  = GUI_spam(window,save[50],4,"Selectionner la save :",NULL,save);

		    sprintf(temp,"save/%s",save[get_ui].text);

	        if(get_ui >= 0)
	        {

				FILE* flux = fopen(temp,"rb");
				if (flux == NULL)
				{
				    printf ("// Erreur lors du chargement du fichier de sauvegarde du joueur, verifier les droits d'acces en lecture et ecriture du support de stockage courant.\n");
				    obj[0].largeur = 500;
				    obj[0].pos_x = fenetre->w/2 - obj[0].largeur/2;
					GUI_spam(window,obj[0],1,"Erreur le fichier de sauegarde n'existe pas ou est innaccessible",NULL,NULL);
					player->load = 0;
				}
				else
				{
				    // Initialisation des valeurs des variables du fichier de sauvegarde
				    fread(player,sizeof(struct Joueur),1,flux);


					// Perso
					T_Rect[0].x = player->save.T_Elements[0].dynamique.x;
					T_Rect[0].y = player->save.T_Elements[0].dynamique.y;

					T_Rect[0].w = 40;
					T_Rect[0].h = 34;

					// Camera
					T_Rect[1].x = player->save.T_Elements[1].dynamique.x;
					T_Rect[1].y = player->save.T_Elements[1].dynamique.y;

				}
				fclose(flux);
	        }
	        else
	        {
	        	player->load = 0;
	        }
	    }
	    else
	    {
			GUI_spam(window,obj[0],1,"Il n'existe pas de fichier de sauvegarde",NULL,NULL);
			player->load = 0;
	    }
	}
	else
	{
	    printf("// Erreur d'ouverture du dossier de Sauvegarde \n");
	    GUI_spam(window,obj[0],1,"Erreur d'ouverture du dossier de Sauvegarde",NULL,NULL);
	    player->load = 0;
	}
}

// Fonction qui affiche le menu de lancement de partie du jeu
int DISPLAY_Start_menu(struct Joueur player,SDL_Window *window)
{
	char temps[200];
	struct GUI_object obj;
	struct state etat;
	SDL_Surface *render;
    SDL_Rect position;

	SDL_Surface *ecran = SDL_GetWindowSurface(window);
	SDL_Surface *background = IMG_Load("artwork/background_score.png");
	SDL_Color blanc = {255, 255, 255};

    obj.largeur = 175;
    obj.hauteur = 38;
    obj.pos_x = ecran->w/2 - obj.largeur/2;
    obj.pos_y = ecran->h/2;
    obj.id = 1;

	position.x = ecran->w/2 - background->w/2;
    position.y = ecran->h/2 - background->h/2;

	SDL_BlitSurface(background, NULL, ecran, &position);

	TTF_Font *comforta = TTF_OpenFont("fonts/Roboto-Medium.ttf", 40);
	sprintf(temps, "Victoires : %d / Defaites : %d",player.stats.nb_victoire,player.stats.nb_defaite);
	render = TTF_RenderText_Blended(comforta, temps, blanc);

	position.x = ecran->w/2 - render->w/2;
    position.y = 4*render->h;

    SDL_BlitSurface(render, NULL, ecran, &position);

    SDL_FreeSurface(render);

	sprintf(temps, "Meilleur Score : %d",player.stats.best_score);

	render = TTF_RenderText_Blended(comforta, temps, blanc);

	position.x = ecran->w/2 - render->w/2;
    position.y = 6*render->h;

    SDL_BlitSurface(render, NULL, ecran, &position);
    etat = GUI_DISPLAY_button(obj,ecran,"JOUER");

	SDL_FreeSurface(render);
	SDL_FreeSurface(ecran);
	SDL_FreeSurface(background);

	return etat.etat[0];
}

// Fonction qui affiche le menu de lancement de partie du jeu
int DISPLAY_win_menu(struct Joueur player,SDL_Window *window)
{
	char temps[200];
	struct GUI_object obj;
	struct state etat;
	SDL_Surface *render;
    SDL_Rect position;

	SDL_Surface *ecran = SDL_GetWindowSurface(window);
	SDL_Surface *background = IMG_Load("artwork/win.png");
	SDL_Color blanc = {0, 0, 0};

	SDL_ShowCursor(1);

    obj.largeur = 175;
    obj.hauteur = 38;
    obj.pos_x = ecran->w/2 - obj.largeur/2;
    obj.pos_y = ecran->h/2 +200;
    obj.id = 1;

	position.x = ecran->w/2 - background->w/2;
    position.y = ecran->h/2 - background->h/2;

	SDL_FillRect(ecran,NULL,SDL_MapRGB(ecran->format, 255, 255, 255));
	SDL_BlitSurface(background, NULL, ecran, &position);

	TTF_Font *comforta = TTF_OpenFont("fonts/Roboto-Medium.ttf", 40);

	sprintf(temps, "Score : %d",player.stats.score);
	render = TTF_RenderText_Blended(comforta, temps, blanc);

	position.x = ecran->w/2 - render->w/2;
    position.y = render->h;

    SDL_BlitSurface(render, NULL, ecran, &position);

	sprintf(temps, "Meilleur Score : %d",player.stats.best_score);
	render = TTF_RenderText_Blended(comforta, temps, blanc);

	position.x = ecran->w/2 - render->w/2;
    position.y = 2*render->h;

    SDL_BlitSurface(render, NULL, ecran, &position);

    etat = GUI_DISPLAY_button(obj,ecran,"Retour");

	SDL_FreeSurface(render);
	SDL_FreeSurface(ecran);
	SDL_FreeSurface(background);

	SDL_UpdateWindowSurface(window);

	return etat.etat[0];
}

// Fonction qui affiche le menu de lancement de partie du jeu
int DISPLAY_game_over_menu(struct Joueur player,SDL_Window *window)
{
	char temps[200];
	struct GUI_object obj;
	struct state etat;
	SDL_Surface *render;
    SDL_Rect position;

    SDL_ShowCursor(1);

	SDL_Surface *ecran = SDL_GetWindowSurface(window);
	SDL_Surface *background = IMG_Load("artwork/game_over.png");
	SDL_Color blanc = {0, 0, 0};

    obj.largeur = 175;
    obj.hauteur = 38;
    obj.pos_x = ecran->w/2 - obj.largeur/2;
    obj.pos_y = ecran->h/2 +200;
    obj.id = 1;

	position.x = ecran->w/2 - background->w/2;
    position.y = ecran->h/2 - background->h/2;

	SDL_FillRect(ecran,NULL,SDL_MapRGB(ecran->format, 255, 255, 255));
	SDL_BlitSurface(background, NULL, ecran, &position);

	TTF_Font *comforta = TTF_OpenFont("fonts/Roboto-Medium.ttf", 40);
	sprintf(temps, "Meilleur Score : %d",player.stats.best_score);

	render = TTF_RenderText_Blended(comforta, temps, blanc);

	position.x = ecran->w/2 - render->w/2;
    position.y = 2*render->h;

    SDL_BlitSurface(render, NULL, ecran, &position);
    etat = GUI_DISPLAY_button(obj,ecran,"Retour");

	SDL_FreeSurface(render);
	SDL_FreeSurface(ecran);
	SDL_FreeSurface(background);

	SDL_UpdateWindowSurface(window);

	return etat.etat[0];
}

/////////////////////// PHASE 1  //////////////////////////////////////////

int Phase1(SDL_Window *fenetre, SDL_Renderer *Renderer)
{
    /*-------------------------------------------------------------------------------*/
                                       /* Déclarations variables */
	int i=0, quit=0, eau=0, debut=0, tempsPrecedent=0, tempsActuel=0, compteur_X=0,compteur_Y=0, compteur_vitesse_X=0, compteur_vitesse_Y=0;
	int Vitesse_X_Gouttes[NB_GOUTTES];
	int Vitesse_Y_Gouttes[NB_GOUTTES];

	int touches[2];

	int X_aleatoire[TAILLE_X_ALEATOIRE];
	int Y_aleatoire[TAILLE_Y_ALEATOIRE];
	int Vitesse_X_aleatoire[TAILLE_VITESSE_X];
	int Vitesse_Y_aleatoire[TAILLE_VITESSE_Y];

	int largeur_fenetre;
	int hauteur_fenetre;
	int init_timestamp;
	int temps_actuel;

	Configuration configuration;

	if (ini_parse("config.ini", handler, &configuration) < 0) {
		printf("Can't load 'test.ini'\n");
		return 1;
	}

//    largeur_fenetre = configuration.nb_blocs_largeur;
	largeur_fenetre = NB_BLOCS_LARGEUR;
    largeur_fenetre = largeur_fenetre*32;

//    hauteur_fenetre = configuration.nb_blocs_hauteur;
	hauteur_fenetre = NB_BLOCS_HAUTEUR;
    hauteur_fenetre = hauteur_fenetre*32;

    Nb_aleatoire_a_b(0, largeur_fenetre-100, X_aleatoire, TAILLE_X_ALEATOIRE); // remplissage du tableau X_aleatoire
    Nb_aleatoire_a_b(hauteur_fenetre, hauteur_fenetre*2, Y_aleatoire, TAILLE_Y_ALEATOIRE); // remplissage du tableau Y_aleatoire
    Nb_aleatoire_a_b(1, VITESSE_MAX_X_GOUTTE, Vitesse_X_aleatoire, TAILLE_VITESSE_X); // remplissage du tableau Vitesse_X_aleatoire
    Nb_aleatoire_a_b(VITESSE_MIN_Y_GOUTTE, VITESSE_MAX_Y_GOUTTE, Vitesse_Y_aleatoire, TAILLE_VITESSE_Y); // remplissage du tableau Vitesse_Y_aleatoire

    SDL_Event event;

    /* Chargement surfaces */
    SDL_Surface *S_Fleche_bas = IMG_Load("artwork/Phase1/FLECHE_BAS.png");
    // la barre d'eau
    SDL_Surface *barre = IMG_Load("artwork/Phase1/barre.png");

       /* Nuages */
    SDL_Surface *S_Nuage[4];
     S_Nuage[DEPART] = IMG_Load("artwork/Phase1/NUAGE_DEBUT.png");
     S_Nuage[PETIT] = IMG_Load("artwork/Phase1/NUAGE_PETIT.png");
     S_Nuage[MOYEN] = IMG_Load("artwork/Phase1/NUAGE_MOYEN.png");
     S_Nuage[GROS] = IMG_Load("artwork/Phase1/NUAGE_GROS.png");

       /* Slappy */
    SDL_Surface *S_Slappy[NB_ETATS_SLAPPY];
    S_Slappy[ATTENTE] = IMG_Load("artwork/Phase1/ATTENTE.png");
    S_Slappy[BAS]     = IMG_Load("artwork/Phase1/BAS.png");
    S_Slappy[DROITE]  = IMG_Load("artwork/Phase1/DROITE.png");
    S_Slappy[GAUCHE]  = IMG_Load("artwork/Phase1/GAUCHE.png");

    touches[0] = 0;
    touches[1] = 0;

       /* Gouttes d'eau */
   SDL_Surface *S_Goutte[NB_GOUTTES];
   for(i=0;i<NB_GOUTTES;i++)
   {
    S_Goutte[i]=IMG_Load("artwork/Phase1/GOUTTE.png");
   }

   /* Création des Textures */
   SDL_Texture *T_Fleche_bas = SDL_CreateTextureFromSurface(Renderer,S_Fleche_bas);
   SDL_Texture *tx_barre = SDL_CreateTextureFromSurface(Renderer,barre);

      /* Nuages */
   SDL_Texture *T_Nuage[4];
    for(i=0;i<4;i++)
   {
    T_Nuage[i] = SDL_CreateTextureFromSurface(Renderer,S_Nuage[i]);
   }

      /* Slappy */
   SDL_Texture *T_SlappyActuel = NULL;
   SDL_Texture *T_Slappy[NB_ETATS_SLAPPY];
   for(i=0;i<NB_ETATS_SLAPPY;i++)
   {
    T_Slappy[i] = SDL_CreateTextureFromSurface(Renderer,S_Slappy[i]);
   }

        /* Gouttes d'eau */
   SDL_Texture *T_Goutte[NB_GOUTTES];
   for(i=0;i<NB_GOUTTES;i++)
   {
    T_Goutte[i] = SDL_CreateTextureFromSurface(Renderer,S_Goutte[i]);
   }

    /*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/


    /*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
                                         /* Déclaration Rect */
    SDL_Rect R_Slappy = { largeur_fenetre/2 - S_Slappy[ATTENTE]->w/2, hauteur_fenetre/4 - S_Slappy[ATTENTE]->h/2, S_Slappy[ATTENTE]->w, S_Slappy[ATTENTE]->h};

    SDL_Rect R_Fleche_bas = { largeur_fenetre/2 - S_Fleche_bas->w/2, hauteur_fenetre/2, S_Fleche_bas->w, S_Fleche_bas->h};
    SDL_Rect R_barre = { largeur_fenetre/2 - barre->w/2, 5 , 1, barre->h};

    SDL_Rect R_Nuage[4];
    R_Nuage[DEPART] = Def_Rect( largeur_fenetre/2 - S_Nuage[DEPART]->w/2, hauteur_fenetre/4 - S_Nuage[DEPART]->h/2, S_Nuage[DEPART]->w, S_Nuage[DEPART]->h);
    R_Nuage[PETIT]  = Def_Rect(largeur_fenetre/4, hauteur_fenetre, S_Nuage[PETIT]->w, S_Nuage[PETIT]->h);
    R_Nuage[MOYEN]  = Def_Rect(largeur_fenetre-largeur_fenetre/4, hauteur_fenetre*5/3, S_Nuage[MOYEN]->w, S_Nuage[MOYEN]->h);
    R_Nuage[GROS]   = Def_Rect(largeur_fenetre/2, hauteur_fenetre*4/3, S_Nuage[GROS]->w, S_Nuage[GROS] ->h);

    SDL_Rect R_Goutte[NB_GOUTTES];
    for(i=0;i<NB_GOUTTES;i++)
    {
      R_Goutte[i].x = X_aleatoire[compteur_X];
      compteur_X++;
      if(compteur_X==TAILLE_X_ALEATOIRE)
       {
         compteur_X=0;
       }
      R_Goutte[i].y = Y_aleatoire[compteur_Y];
      compteur_Y++;
      if(compteur_Y==TAILLE_Y_ALEATOIRE)
       {
         compteur_Y=0;
       }

      R_Goutte[i].w = S_Goutte[i]->w;
      R_Goutte[i].h = S_Goutte[i]->h;
    }
        /*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/



        /*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
                                         /* Initialisation des tableaux de vitesses */
   for(i=0;i<NB_GOUTTES;i++)
   {
    Vitesse_X_Gouttes[i] = Vitesse_X_aleatoire[compteur_vitesse_X]; // Vitesse Horizontale
    Vitesse_Y_Gouttes[i] = Vitesse_Y_aleatoire[compteur_vitesse_Y]; // Vitesse Verticale

    compteur_vitesse_X++;
    if(compteur_vitesse_X==TAILLE_VITESSE_X)
    {compteur_vitesse_X=0;
    }

    compteur_vitesse_Y++;
    if(compteur_vitesse_Y==TAILLE_VITESSE_Y)
    {compteur_vitesse_Y=0;
    }
   }
        /*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/


        /*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
                                         /* Boucle d'evenements */
    while(quit==0)
    {
      	if(debut==0)
      	{
      		T_SlappyActuel = T_Slappy[ATTENTE]; // Image de Slappy avant la chute (avant debut du jeu)
      	}
      	else
      	{
      		T_SlappyActuel = T_Slappy[BAS]; // Image de Slappy par défaut pendant la chute (pendant le jeu)
      	}

    	while(SDL_PollEvent(&event)); // Récupération des actions de l'utilisateur(while pour une bonne utilisation)
    	{

	    	switch(event.type)
	    	{
	    		case SDL_QUIT: // Clic sur la croix
	    		      quit=1;
	    		      break;

	    		case SDL_KEYDOWN: // Appuis d'une touche
	    		    switch(event.key.keysym.sym)
	    		    {
	    		        case SDLK_ESCAPE: // Echap
	    		            quit=1;
	    		        break;

	    		        case SDLK_DOWN: // Flèche bas
	    		            debut=1;
	    		              init_timestamp = SDL_GetTicks();
	    		        break;

	    		        case SDLK_RIGHT: // Flèche droite

	    		        	touches[0] = 1;
	    		        	touches[1] = 0;

	    		        break;

	    		        case SDLK_LEFT: // Flèche gauche

	    		        	touches[1] = 1;
	    		        	touches[0] = 0;

	    		        break;
	    		    }
	    		break;


	    		case SDL_KEYUP: // Appuis d'une touche
	    		    switch(event.key.keysym.sym)
	    		    {
	    		        case SDLK_RIGHT: // Flèche droite
	    		        	touches[0] = 0;
	    		        break;

	    		        case SDLK_LEFT: // Flèche gauche
	    		        	touches[1] = 0;
	    		        break;
	    		    }
	    		break;
	    	}

	    	/* gestion des inputs */
	    	if (touches[0] == 1)
	    	{
	    		if(debut==1 && R_Slappy.x<largeur_fenetre-R_Slappy.w)
	    		{
	    		  R_Slappy.x=R_Slappy.x + VITESSE_X_PHASE_1;
	    		  T_SlappyActuel = T_Slappy[DROITE];
	    		}
	    		touches[1] = 0;
	    	}
	    	else if(touches[1] == 1)
	    	{
	    		if(debut==1 && R_Slappy.x>0)
	    		{
	    		  R_Slappy.x=R_Slappy.x - VITESSE_X_PHASE_1;
	    		  T_SlappyActuel = T_Slappy[GAUCHE];
	    		}
	    		touches[0] = 0;
	    	}
	    	else
	    	{
	    		T_SlappyActuel = T_Slappy[BAS];
	    	}

    	    /* gestion temps */

    	 	temps_actuel = calcul_time(init_timestamp);
    	 	if (temps_actuel == 30)
    	 	{
    	 		quit = 1;
    	 	}

    	 /*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
    	                                    /* Gestion Collisions */
    	    eau = eau + Collision(NB_GOUTTES, R_Slappy, R_Goutte);

    	 /*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
    	                                    /* Gestion CPU */
    	   tempsActuel = SDL_GetTicks();
    	   if (tempsActuel - tempsPrecedent > 10)
    	   {
    	       tempsPrecedent = tempsActuel;

    	                                    /* Gestion scrolling (Nuages et gouttes) */
    	       if(debut==1)
    	       {
    	          compteur_X = Scrolling_Nuage(compteur_X, X_aleatoire, R_Nuage);
    	          //compteur_X = Mouvement_Gouttes(compteur_X, X_aleatoire, R_Goutte);

    	          for(i=1;i<NB_GOUTTES;i++)
    	          {
    	              if(R_Goutte[i].y>-R_Goutte[i].h) // Si la Goutte est encore visible
    	              {
    	                 if(i%2==0)
    	                 {
    	                  R_Goutte[i].x = R_Goutte[i].x - Vitesse_X_Gouttes[i]; // Deplacement horizontal
    	                 }
    	                 else
    	                 {
    	                  R_Goutte[i].x = R_Goutte[i].x + Vitesse_X_Gouttes[i]; // Deplacement horizontal
    	                 }

    	                R_Goutte[i].y = R_Goutte[i].y - Vitesse_Y_Gouttes[i]; // Deplacement vertical


    	              }
    	              else
    	              {
    	                 R_Goutte[i].y = Y_aleatoire[compteur_Y];
    	                 compteur_Y++;
    	                 if(compteur_Y==TAILLE_Y_ALEATOIRE)
    	                 {compteur_Y=0;
    	                 }

    	                 R_Goutte[i].x = X_aleatoire[compteur_X];
    	                 compteur_X++;
    	                 if(compteur_X==TAILLE_X_ALEATOIRE)
    	                 {compteur_X=0;
    	                 }

    	                 Vitesse_X_Gouttes[i] = Vitesse_X_aleatoire[compteur_vitesse_X];
    	                 if(compteur_vitesse_X==TAILLE_VITESSE_X)
    	                 {compteur_vitesse_X=0;
    	                 }

    	                 Vitesse_Y_Gouttes[i] = Vitesse_Y_aleatoire[compteur_vitesse_Y];
    	                 if(compteur_vitesse_Y==TAILLE_VITESSE_Y)
    	                 {compteur_vitesse_Y=0;
    	                 }
    	              }
    	          }
    	        }
    	   }
    	   else
    	   {
    	       SDL_Delay(10 - (tempsActuel - tempsPrecedent));
    	   }
    	 /*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/


    	 /*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
    	      /* Couleur de fond */
    	   SDL_SetRenderDrawColor(Renderer, 149, 202, 244, 255); // couleur du fond
    	   SDL_RenderClear(Renderer);


    	      /* Copie des sprites grâce au SDL_Renderer */
    	   if(debut==1)
    	   {                  // Nuages utilisés pour le scrolling (T_Nuage[])
    	     for(i=1;i<4;i++)
    	     {
    	       SDL_RenderCopy(Renderer,T_Nuage[i],NULL,&R_Nuage[i]);
    	     }
    	     for(i=1;i<NB_GOUTTES;i++)
    	     {
    	      SDL_RenderCopy(Renderer,T_Goutte[i],NULL,&R_Goutte[i]);
    	     }
    	   }
    	   else
    	   {
    	    SDL_RenderCopy(Renderer,T_Fleche_bas,NULL,&R_Fleche_bas);
    	   }

    	   R_barre.w = eau*5;
    	   R_barre.x = largeur_fenetre/2 - R_barre.w/2;

    	   SDL_RenderCopy(Renderer,tx_barre,NULL,&R_barre);
    	   SDL_RenderCopy(Renderer,T_Nuage[DEPART],NULL,&R_Nuage[DEPART]);
    	   SDL_RenderCopy(Renderer,T_SlappyActuel,NULL,&R_Slappy);


    	   SDL_RenderPresent(Renderer); // Affichage

    	}
    }
    /*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/


    /*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
                                       /* Libérations mémoire */
   // Textures
   SDL_DestroyTexture(T_SlappyActuel);
   SDL_DestroyTexture(T_Fleche_bas);
   SDL_DestroyTexture(tx_barre);

   for(i=0;i<NB_ETATS_SLAPPY;i++)
   { SDL_DestroyTexture(T_Slappy[i]);
   }

   for(i=0;i<4;i++)
   { SDL_DestroyTexture(T_Nuage[i]);
   }

   for(i=0;i<NB_GOUTTES;i++)
   { SDL_DestroyTexture(T_Goutte[i]);
   }

   // Surfaces
   SDL_FreeSurface(S_Fleche_bas);
   SDL_FreeSurface(barre);

   for(i=0;i<NB_ETATS_SLAPPY;i++)
   { SDL_FreeSurface(S_Slappy[i]);
   }

   for(i=0;i<4;i++)
   { SDL_FreeSurface(S_Nuage[i]);
   }

   for(i=0;i<NB_GOUTTES;i++)
   { SDL_FreeSurface(S_Goutte[i]);
   }

        /*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
   eau = eau + 1;

   return eau;

}



void Nb_aleatoire_a_b(int a, int b, int tab[], int taille) // Fonction pour avoir tableau de nombres aléatoire compris entre a et b (a et b inclus)
{
  int i=0;
  srand(time(NULL)); // initialisation de rand

  if(a<b)
  {
    a=a+1; // Pour que le nb aléatoire puisse etre égale à b
    for(i=0;i<taille;i++)
    {
      tab[i] = rand()%(b-a) +a;
    }
  }

  if(a>b)
  {
    b=b+1; // Pour que le nb aléatoire puisse etre égale à a
    for(i=0;i<taille;i++)
    {
      tab[i] = rand()%(a-b) +b;
    }
  }
}


SDL_Rect Def_Rect ( int x, int y, int largeur, int hauteur)
{
  SDL_Rect Rect;
  Rect.x = x;
  Rect.y = y;
  Rect.w = largeur;
  Rect.h = hauteur;

  return Rect;
}



int Scrolling_Nuage(int compteur_X, int X_aleatoire[], SDL_Rect R_Nuage[]) // Fonction modifiant le tableau de coordonnées des nuages
{
  int i;
  // Nuage de départ
  if(R_Nuage[DEPART].y > -R_Nuage[DEPART].h)
  {
    R_Nuage[DEPART].y = R_Nuage[DEPART].y - VITESSE_Y_PHASE_1; // disparition du nuage de début
  }

  // Autres Nuages
  for(i=1;i<4;i++) // Pour les nuages "petit", "moyen" et "gros"
  {
    if(R_Nuage[i].y>-R_Nuage[i].h) // Si le nuage est encore visible
    {
      R_Nuage[i].y = R_Nuage[i].y - VITESSE_Y_PHASE_1;
    }
    else
    {
     R_Nuage[i].y = HAUTEUR_FENETRE;
     R_Nuage[i].x = X_aleatoire[compteur_X];
     compteur_X++;
     if(compteur_X==TAILLE_X_ALEATOIRE)
     {
       compteur_X=0;
     }
    }
  }
  return compteur_X;
}


int Collision(int Nb_gouttes, SDL_Rect R_Slappy, SDL_Rect R_Goutte[])
{
  int i=0, eau=0;
  for(i=0;i<Nb_gouttes;i++)
  {
    if( (R_Slappy.x >= R_Goutte[i].x + R_Goutte[i].w) || (R_Slappy.x + R_Slappy.w <= R_Goutte[i].x) || (R_Slappy.y+(R_Slappy.h/2) >= R_Goutte[i].y + R_Goutte[i].h) || (R_Slappy.y + R_Slappy.h <= R_Goutte[i].y) ) // si trop à droite ou à gauche ou en bas ou en haut de la petite goutte
    {
      // Si on entre dans le if c est qu'il n y a pas collision
    }
    else
    {
      R_Goutte[i].y = -R_Goutte[i].h-1; // La goutte disparait de l'écran
      eau++;
    }
  }
  return eau;
}
