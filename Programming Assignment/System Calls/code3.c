// This is code for changing file permissions using "access" and "chmod" system call	\
1.	"access" takes following arguments:	\
	1.filepath:	path to the file which you want to check for permission		\
	2.flag:		which permission you want to check				\
			R_OK:	for Read Attributes	\
			W_OK:	for Write Attributes	\
			X_OK:	for Execute Attributes	\
			F_OK:	to Check the existence of file	\
			It returns 0 on success and -1 on failure(i.e permission is denied).						\
			you can also use above flags more than one at a time, if any one of them have permission denied then it returns -1.	\
2.	"chmod" takes following arguments:	\
	1.filepath:	path to the file for which you want to change permissions		\
	2.permissions:	here you give permissions in bitmask mode.

#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

int main(){
creat("newfile",00644);
if (access("newfile",X_OK)==0) write(1,"newfile have executable permission\n",37);
else write(1,"newfile doesn't have executable permission\n",45);

if (chmod("newfile",00744)==0) write(1,"chmod successful\n",17);
else write(1,"chmod not successful\n",21);

if (access("newfile",X_OK)==0) write(1,"newfile have executable permission\n",37);
else write(1,"newfile doesn't have executable permission\n",45);
}
