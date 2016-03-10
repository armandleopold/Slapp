/* Nom : structures.h

Dernier Auteur  : Armand

Creation :
24/01/2014

Dernière modification :
08/02/2014

Description :
	Fichier qui contient toutes les structures du jeux

*///

// Définition du type de la structure tiles


// Structure qui contient le compte rendus de collisions du moteur de collisions
struct Collision{

	int collision[2][2];
};

// Structure contenant toutes les propriétés d'un objet GUI
struct GUI_object{

	int id;

	int largeur;
	int hauteur;
	int pos_x;
	int pos_y;

	char text[150]; // Zone de texte

	int etat[2]; // Bouton

};

// Structure contenant les caractéristiques associées à un type de tiles
struct tiles_spec {

	int type;
	int passable;
	int plan;
	int abs;

	char image[100];

};

// Structure etat qui permet de définir 10 types d'états différents pour un objet ou tout autre chose ...
struct state {

	int etat[30];

};

// structure qui contient les vecteurs accelerations vitesse et position d'un element
struct Mvt {

	float x;
	float y;

	float vx;
	float vy;

	float ax;
	float ay;

};

// structure d'une tile
struct tiles {

    int type;

};

// Structure coordonnees en int
struct Coordonnees {

	int x;
	int y;

};

// Structure coordonnees en float
struct fCoordonnees {

	float x;
	float y;

};

// Structure d'une map
struct Map {

	char nom[100];
	char musique[100];
	char background[100];

	struct tiles_spec tiles_spec_T[250];
	struct tiles tiles_T[TAILLE_MAP_X][TAILLE_MAP_Y];

	struct Coordonnees start;

};


// structure qui contient les données relative à un element.
struct Element {

	struct Mvt dynamique;
	int type_element;
	int etat_element;

};

// Structure permmettant d'enregistrer des statistiques sur la partie.
struct Stats {

	char nom_map[100];

	int nb_victoire;
	int nb_defaite;

	int best_score;
	int score;

	int temps_phase_1;
	int temps_phase_2;
	int eau_debut;
	int eau_fin;

};

// structure qui contient toutes les données pour sauvegarder une partie.
struct Sauvegarde_partie {

	char nom_map[100];

	struct Coordonnees position_joueur;
	struct Element T_Elements[20];

	int phase;
	int eau;

};

// structure qui associe des données à un joueur.
struct Joueur {

	char pseudo[201];
	
	struct Stats stats;
	struct Sauvegarde_partie save;

	int load;

};
