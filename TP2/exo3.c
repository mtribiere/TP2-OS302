#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <unistd.h> 
#include <signal.h>
#include <sys/wait.h>
#include <time.h>

#define CHILD_COUNT 6

int needToExit = 0;

//Liste de signaux pour les fils
void onPlaySignal(){};
void onQuitSignal(){
	needToExit = 1;
}


//Liste des signaux pour le pere
void onChildAliveSignal(){}

void onChildDead(){
	needToExit = 1;
}


//Fonction
void writeRandomNum(){

	//Ouvrir le fichier barillet
	FILE *file = fopen("barillet","w+");

	//Tirer un nombre aléatoire
	srand(time(NULL));
	int num = (rand()%CHILD_COUNT)+1;

	//Ecrire le résultat
	fprintf(file,"%d",num);

	//Fermer le fichier
	fclose(file);
}


//Fonction pour jouer
void playGame(int childNum){
	
	//Attendre de recevoir le signal de jouer
	pause();

	//Si le jeu n'est fini
	if(!needToExit){

		//Lire le fichier barillet
		FILE *file = fopen("barillet","r");

		//Si erreur d'ouverture du fichier
		if(file == NULL){
			printf("Erreur lors de l'ouverture du fichier\n");
			exit(1);
		}
		
		int readNum;
		fscanf(file,"%d",&readNum);

		fclose(file);

		//Si c'est notre numéro
		if(childNum == readNum){
			printf("%d : C'est mon nu<*PAN*>.......... \n",childNum);
			exit(0);
		}else{
			printf("%d : Ce n'est pas mon numéro au suivant ! \n",childNum);
		}


		//Indiquer au père qu'on est vivant
		kill(getppid(),SIGUSR2);

		//Attendre la fin du jeu
		while(!needToExit);
	}

	//Quitter la table
	printf("%d : Quitte la table\n",childNum);
	exit(0);
}

int main(){

	//Remplir le fichier barillet
	writeRandomNum();

	//Definir la réaction au signal SIGUSR1
	signal(SIGUSR1, onPlaySignal);

	//Definir la réaction au signal SIGUSR2
	signal(SIGUSR2, onQuitSignal);

	//Creer 6 fils
	pid_t childPid[CHILD_COUNT] = {0};

	for(int i = 1;i<=CHILD_COUNT;i++){
		
		childPid[i-1] = fork();

		if(childPid[i-1] == 0) playGame(i);
	}
	
	//Changer son signal SIGCHILD
	signal(SIGCHLD,onChildDead);

	//Changer son signal SIGUSR2
	signal(SIGUSR2,onChildAliveSignal);

	printf("Pere : Bien commençons !\n");

	//Tant que un fils n'est pas mort
	int playerNum = 1;
	while(!needToExit){

		printf("Pere : Au fils %d\n",playerNum);

		//Indiquer au fils de jouer
		kill(childPid[playerNum-1],SIGUSR1);

		//Attendre le retour
		pause();

		//Passer au fils suivant
		playerNum++;

		sleep(2);

	}

	printf("\nPere : Le fils %d a perdu, vous etes libres !\n",playerNum-1);

	//Liberer les fils
	for(int i = 0;i<CHILD_COUNT;i++){
		kill(childPid[i],SIGUSR2);
	}

	//Attendre que tout les fils soient partis
	while(wait(NULL) != -1);

	printf("Pere : Quitte la table\n");

	return 0;
}