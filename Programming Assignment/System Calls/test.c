#include<unistd.h>
#include<sys/types.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<stdio.h>

int main(){


int usrid = getuid();
int eusrid = geteuid();
int grpid = getgid();
int egrpid = getegid();
int proid = getpid();
int pproid = getppid();
printf("%d %d %d %d %d %d\n",usrid,eusrid,grpid,egrpid,proid,pproid);

/*
int link_status = link("/home/hs/trace","/home/hs/trace_1");
printf("%d\n",link_status);
*/

/*
int unlink_status = unlink("/home/hs/trace_1");
printf("%d\n",unlink_status);
*/

//uid_t tt = geteuid();

/*
int fd = open("/home/hs/trace",O_RDWR);
struct stat s;
struct stat *buf = &s;
int fstat_status = fstat(fd,buf);
printf("%d %s\n",fstat_status,buf);
*/

/*
mode_t m = O_RDWR;
int fd = open("/home/hs/trace",m);
int fd_1 = open("/home/hs/dest",O_RDWR);
char cc = 'e';
char *c = &cc;
int lseek_status = lseek(fd,3,SEEK_SET);
lseek(fd,2,SEEK_CUR);
lseek(fd,-12,SEEK_END);
int read_status = read(fd,c,10);
int write_status = write(fd_1,c,10);
//printf("%s\n",c);
int close_status = close(fd);
int close_status_1 = close(fd_1);
//ssize_t w1 = write(fd,"\nAdded text\n",12);
//gid_t g = getgid();
printf("%d %d %d %d %s\n",read_status,write_status,close_status,lseek_status,c);
*/

/*
int access_status = open("/home/hs/trace",X_OK,R_OK,W_OK);
printf("%d\n",access_status);
*/

/*
int chmod_status = chmod("/home/hs/trace",00744);
printf("%d\n",chmod_status);
*/

int fd = creat("/home/hs/new",O_RDWR);
printf("%d\n",fd);

}
