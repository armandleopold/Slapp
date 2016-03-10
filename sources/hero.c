/* Nom : hero.c

Dernier Auteur  : Armand

Creation :
08/03/2014

Dernière modification :
08/03/2014

Description :
	Fichier qui contient toutes les fonctions du personnage

*///

#include "../headers/linker.h"

SDL_Surface* HERO_display(struct Element hero,struct Element camera)
{
	SDL_Surface* render;
	char temp[50] = "";

	if ((hero.dynamique.vx > 0 || camera.dynamique.vx > 0)
	&& hero.dynamique.vy == 0 
	&& camera.dynamique.vy == 0) // Droite
	{
		sprintf(temp,"artwork/hero/droite.png");
	}
	else if((hero.dynamique.vx < 0 || camera.dynamique.vx < 0)
	&& hero.dynamique.vy == 0 
	&& camera.dynamique.vy == 0) // Gauche
	{
		sprintf(temp,"artwork/hero/gauche.png");
	}
	else if (hero.dynamique.vx == 0 
	&& camera.dynamique.vx == 0
	&& (hero.dynamique.vy > 0 || camera.dynamique.vy > 0)) // Bas
	{
		sprintf(temp,"artwork/hero/bas.png");
	}
	else if(hero.dynamique.vx == 0 
	&& camera.dynamique.vx == 0
	&& (hero.dynamique.vy < 0 || camera.dynamique.vy < 0)) // haut
	{
		sprintf(temp,"artwork/hero/haut.png");
	}
	else if ((hero.dynamique.vx > 0 || camera.dynamique.vx > 0)
	&& (hero.dynamique.vy > 0 || camera.dynamique.vy > 0)) // Bas droite
	{
		sprintf(temp,"artwork/hero/bas_droite.png");
	}
	else if ((hero.dynamique.vx < 0 || camera.dynamique.vx < 0)
	&& (hero.dynamique.vy > 0 || camera.dynamique.vy > 0)) // Bas gauche
	{
		sprintf(temp,"artwork/hero/bas_gauche.png");
	}
	else if ((hero.dynamique.vx > 0 || camera.dynamique.vx > 0)
	&& (hero.dynamique.vy < 0 || camera.dynamique.vy < 0)) // Haut droite
	{
		sprintf(temp,"artwork/hero/haut_droite.png");
	}
	else if ((hero.dynamique.vx < 0 || camera.dynamique.vx < 0)
	&& (hero.dynamique.vy < 0 || camera.dynamique.vy < 0)) // Haut gauche
	{
		sprintf(temp,"artwork/hero/haut_gauche.png");
	}
	else // immobile
	{
		sprintf(temp,"artwork/hero/stop.png");
	}
	
	render = IMG_Load(temp);

	return render;
}