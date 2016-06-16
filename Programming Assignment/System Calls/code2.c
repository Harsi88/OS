// I am going to open,delete a file,read a file, write to a file in this code.		\
1.   "open" takes two arguments:		\
   1. path:	path or file_name as which file we need to open.	\
   2. flag:	mode in which we want to open file.	\
   		O_RDONLY: read-only	O_WRONLY: write-only	O_RDWR: rad-write	\
   		\
   open may fail in case we don't have permissions to open file in a particular mode.	\
   open returns -1 when fails and	\
   file descriptor(fd) on success, that is used to refer to file in rest of the program.\
   \
   \
2.   "unlink" command is used to delete a file. \
   It takes only one argument:		\
   1.path_name:	path to the file that needs to be deleted	\
   \
   \
3.   "read" for reading from a file descriptor	\
   Arguments:	\
   1.file-descriptor:	which is returned by "open" command on file	\
   2.buffer pointer:	starting pointer for buffer from which file is to be read	\
   3.size:		size of file(in bytes) you want to read	\
   "read" returns the number of bytes read from file_descriptor	on success\
   and -1 on failure	\
   \
   \
4.   "write" for writing to  file descriptor.	\
   Arguments:	\
   1.file-descriptor:	which is returned by "open" command on file in which we want to write	\
   2.buffer pointer:	starting pointer for buffer from which data is to be written to file		\
   3.size:		size of file(in bytes) you want to read	\
   "write" return number of bytes written on success	\
   and -1 on failure.	\
   \
   \
5.   "lseek" is used for reposition.	\
   Arguments:	\
   1.file-descriptor:	for file where operation needs to b applied	\
   2.offset:		how many byte you want to move	\
   3.flag:		from where you want to move		\
   			SEEK_SET:	from start	\
   			SEEK_CUR: 	from current position	\
   			SEEK_END:	from End Position.
   
   
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

int main(){
creat("new_file",00644);
open("new_file_1",O_CREAT,00644);

int fd = open("new_file",O_RDWR);	// opened a file
write(fd,"HELLO WORLD!!!\n",15);
close(fd);

fd = open("new_file",O_RDWR);
lseek(fd,6,SEEK_SET);
char c[8];  // buffer for reading and writing
read(fd,c,8);  // reading to buffer c from file with file_descriptor fd
close(fd);

int fd_1 = open("new_file_1",O_RDWR);
write(fd_1,"HELLO",5);  // writing from buffer c to file with file_descriptor fd_1
close(fd_1);

return 0;
}
