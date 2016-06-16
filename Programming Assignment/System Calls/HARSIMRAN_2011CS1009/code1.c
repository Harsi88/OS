// This code is creating a new file using system call "creat" and "open"	\
   "creat" takes two arguments:	\
   1. file_name:	the name you want to give to your newly created file	\
   2. mode:		mode specifies permissions to use the newly created files	\
   			mode is specified by bitmask created by ORing together		\
   			different values for different permissions		\
   			\
   "open" takes three arguments to create a file:	\
   1. file_name:	same as in "creat"		\
   2. flag:		when set to O_CREAT, it run the system call open in create mode	\
   3. mode:		same as in "creat" 
   


#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

int main(){
creat("new_file",00644);
open("new_file_1",O_CREAT,00644);
return 0;
}
