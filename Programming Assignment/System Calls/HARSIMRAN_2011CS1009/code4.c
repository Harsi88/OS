// We implemented following process control system call in this code.		\
1.	"getpid" to get process id for calling process.		\
	It returns the process id.		\
2.	"getppid" to get process id for parent process of calling process.	\
3.	"getpriority" to get priority of a process (ranging from -20 to 19).	\
	It takes two arguments:	PRIO_PROCESS and process id	\
	PRIO_PROCESS to determine that we want to get priority for a process with process id given as second argument.	\
4.	"setpriority" to set priority of a process (ranging from -20 to 19).	\
	It takes three arguments:	PRIO_PROCESS, process id and priority	\
	PRIO_PROCESS to determine that we want to set priority for a process with process id given as second argument to the priority given in third argument.	\
5.	"fork" is used to create a child process.	\
	It clone the process into child process. Now two process will run simultaneously, one is parent an d other is child. It assign 0 process id to child process. The order in which these processes will run is random.	\
6.	"_exit()" is used to exit the current running process.		\
7.	"sbrk()" gives us the address of last memory cell allocated to process.	\
8.	"brk(ptr)" change the last memory address of calling process to ptr, hence allocating and deallocating memory to a process.		\
9.	"kill(pid,n)" kills the process having process id as pid and send the signal corresponding to n.	\
10	"wait()" is used in parent process, so it would wait for child process to complete and than start.

#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<signal.h>
#include<sys/time.h>
#include<sys/resource.h>

int main(){
int pid = getpid();
int ppid = getppid();
int priority = getpriority(PRIO_PROCESS,pid);
setpriority(PRIO_PROCESS,pid,2);
int spriority = getpriority(PRIO_PROCESS,pid);
if (spriority == 2) write(1,"Process priority is changed\n",28);
int i,j;
int f = fork();
if(f==0) for(j=0;j<10;j++){
write(1,"child\n",6);
if (j==5) _exit(0);
}
else for(i=0;i<10;i++){
wait();
write(1,"parent\n",7);
}
int *ptr = sbrk(0);	// getting last memory cell address belonging to process
ptr+=4;			// increased ptr by 4, hence allocating 4 additional bytes to this process
brk(ptr);
kill(pid,6);
return 0;
}
