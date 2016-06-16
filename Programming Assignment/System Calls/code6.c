// I am executing a system executable file "date" stored in /bin using "execl" system call.

#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<signal.h>
#include<sys/time.h>
#include<sys/resource.h>

int main(){
execl("/bin/date","date",(char*)0,(char*)0);
return 0;
}
