//Structure d'un jeton de Puissance 4
typedef struct TypeJeton{
	//Les couleurs possible d'un jeton, avec une équipe jaune, une équipe rouge et un jeton blanc pour une case vide
	enum { rouge=1, jaune=2, blanc=3 }couleur;
} Jeton;

//Structure du plateau
typedef struct TypePlateau{
	//Taille du plateau en longueur et largeur
	int tailleX;
	int tailleY;
	//Nombre de jeton alignés pour remporter la victoire
	int nombreAligne;
	//Génération de la zone de jeu
	Jeton zoneDeJeu[100][150]; //TODO Définir la taille à l'aide des variables tailleX et tailleY
} Plateau;

void generationZoneDeJeu();

int positionner(int, int);

void afficher();
void afficherGraphique(int,int);

int verifierVictoire();

int selectMenu(int);

int menuPrincipal(char * choix1, char * choix2);

void connexionServeur();

void creerServeur();
int game();
