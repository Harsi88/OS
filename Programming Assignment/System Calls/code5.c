// In this code I am using "signal" system call to handle a signal having integer id SIGQUIT with our function SIGQUIT_handler.		\
	"signal" takes following arguments:	\
	1.signal id:	the signal you want to deliver to  your handler function when triggered			\
	2.Function:	name of function handler for the triggered signal	\
	Therefore, what "signal" call do is provide programmer with a way to change the how a particular signal triggering is handled.

#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<signal.h>
#include<sys/time.h>
#include<sys/resource.h>

#include<stdio.h>

// Routine to handle Signal
void SIGQUIT_handler(int sig){
	signal(sig,SIG_IGN);  // Ignore the normal procedure
	write(1,"\nYou just pressed Ctrl+\\",23);
	write(0,"\nAgain press Ctrl+\\ \n",21);
}

int main(){
write(0,"Press Ctrl+\\ \n",14);
while(1) signal(SIGQUIT,SIGQUIT_handler);
}
