#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <unistd.h> 
#include <signal.h>

void fils(pid_t pidParent){

	for(int i = 0;i<=100;i+=2){
		printf("Fils : %d\n",i);
		kill(pidParent,SIGUSR1);
		pause();
	}
}

void pere(pid_t pidSon){

	pause();
	for(int i = 1;i<=100;i+=2){
		printf("Pere : %d\n",i);
		kill(pidSon,SIGUSR1);
		pause();
	}
	
}

void onSignal(){}

int main(){

	signal(SIGUSR1,onSignal);
	
	pid_t pidParent = getpid();
	pid_t pidSon = fork();

	if(pidSon == 0){
		fils(pidParent);
	}else{
		pere(pidSon);
	}

	return 0;
}