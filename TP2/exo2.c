#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <unistd.h> 
#include <signal.h>
#include <sys/wait.h>

int needToQuit = 0;

void printDigit(int digit){

	if(digit>=10){
		printf("%d",digit);
	}else{
		printf("0%d",digit);
	}

	fflush(stdout);
}

//Pour les heures
void h(int startH){
	
	while(!needToQuit){
	
		pause();

		startH++;
		if(startH >= 24){
			startH = 0;
		}

		printf("\b\b\b\b\b\b\b\b");
		printDigit(startH);
		printf(":00:00");
		fflush(stdout);
		
	}

	exit(0);
}

//Pour les minutes
void m(pid_t pidH,int startM){

	while(!needToQuit){
	
		pause();

		startM++;
		if(startM >= 60){
			startM = 0;
			kill(pidH,SIGUSR1);
		}else{

			printf("\b\b\b\b\b");
			printDigit(startM);
			printf(":00");
			fflush(stdout);

		}


	}

	exit(0);
}

//Pour les secondes
void s(pid_t pidM, int startS){

	while(!needToQuit){
		
		alarm(1);
		pause();

		startS++;
		
		if(startS >= 60){
			startS = 0;
			kill(pidM,SIGUSR1);
		}else{

			printf("\b\b");
			printDigit(startS);

		}

	
	}

	exit(0);
}

void onSignal1(){}
void onSignal2(){ needToQuit = 1; }



int main(int argc, char *argv[]){

	//Si pas asez d'argument
	if(argc<4){
		printf("Pas assez d'arguments : \n ./horloge H M S\n");
		exit(1);
	}

	signal(SIGUSR1,onSignal1);
	signal(SIGALRM,onSignal1);
	signal(SIGUSR2,onSignal2);
	signal(SIGINT,onSignal2);

	pid_t pidH;
	pid_t pidM;
	pid_t pidS;

	//Affichage du début
	printDigit(atoi(argv[1]));
	printf(":");
	printDigit(atoi(argv[2]));
	printf(":");
	printDigit(atoi(argv[3]));


	//Creation de H
	pidH = fork();
	if(pidH == 0){
	
		h(atoi(argv[1]));
	
	}else{
		
		//Creation de M
		pidM = fork();
		if(pidM == 0){
			
			m(pidH, atoi(argv[2]));

		}else{

			//Creation de S
			pidS = fork();
			if(pidS == 0){
				s(pidM,atoi(argv[3]));
			}

		}
	}
	
	fflush(stdout);
	while(!needToQuit);

	//Tuez tout les fils
	kill(pidH,SIGUSR2);
	kill(pidM,SIGUSR2);
	kill(pidS,SIGUSR2);

	//Attendre leur mort
	while(wait(NULL) != -1);

	//Retourner à la ligne
	printf("\n");

	return 0;
}