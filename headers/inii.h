/* Nom : engine.c

Dernier Auteur  : Armand

Creation :
05/02/2014

Dernière modification :
22/02/2014

Description :
    Fichier qui contient toutes les fonctions relatives au moteur graphique du jeux

*///

#ifndef INII
#define INII

	// Fonction qui créer et initialise les valeurs par défauts du jeu dans un fichier de configuration au format ini
	void constructeur_config_file();

	// Fonction qui check l'intégrité des variables de base du jeu, sinon elle recréer le fichier de configuration
	void check_config_file();

#endif // MOTEUR_GRAPHIQUE_H_INCLUDED