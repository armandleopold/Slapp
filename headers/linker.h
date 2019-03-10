/* Nom : linker.h

Dernier Auteur  : Armand

Creation :
24/01/2014

Dernière modification :
25/02/2014

Description :
	Fichier qui contient tous les includes de headers du jeu
*///

// AJOUT LIB de bases en c 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <dirent.h>
#include <time.h>
	
// AJOUT LIB SDL
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "SDL_mixer.h"

// Ajout LIB OpenGL
#include <GL/gl.h>
#include <GL/glu.h>

// Ajout Headers de définition
#include "Define.h"
#include "structures.h"

// Ajout Headers de déclaration des préprocésseurs de fonctions
#include "GUI.h"
#include "moteur_jeu.h"
#include "engine.h"
#include "Menu.h"
#include "moteur_physique.h"
#include "moteur_graphique.h"
#include "moteur_ia.h"
#include "hero.h"
#include "inii.h"
#include "ini.h"
