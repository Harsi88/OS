#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<readline/readline.h>
#include<readline/history.h>

typedef int bool;
#define true 1
#define false 0

int main()
{ 
 char *arg[10];
 char *pt[2];
 int i=0;
 int count = 0;
 
 char* ipt,*ipt1, shell_prompt[100];
 
 // Configure readline to auto-complete paths when the tab key is hit.
 rl_bind_key('\t', rl_complete);
 
 while(1)
 { 
  // Create prompt string from user name and current working directory.
  snprintf(shell_prompt, sizeof(shell_prompt), "myshell>>");
 
  // Display prompt and read input (n.b. input must be freed after use)...
  ipt = readline(shell_prompt);

  // Add input to history.
  add_history(ipt);
 
  ipt1 = ipt;        
 
  char input[100]; //--- Input Array
  int q = 0;
  
  while((*ipt1)!='\0')
  {
   input[q] = *ipt1;
   ipt1++;
   q++;
  }
  input[q] = '\0';
  free(ipt);
  
  printf(" count>>");
  scanf("%d",&count);  
  
  char fmt_input[100];  //--- formatted input for filename
  char temp[100];	//--- Temporary variable to store formatted parts of inputs
  int j=0;		//--- Iterator for input
  bool inputEnd = false; //--- Boolean variable to check if input has end
  bool outRedirect = false; //--- Boolean variable to check if output redirection is present
  int outFile;		//--- Number which will tell where filename is in pt[] array

//---------------------------------

  while(!inputEnd)	//--- Removing spaces from the input
  {
   if(input[j]=='\0') 
   { 
    inputEnd = true;
    break;
   }
   if(input[j]!=' ') break;
   j++;
  }
  if(inputEnd)
  {
   printf("Are you sure this is the command?\n");
   continue;
  }
  
//  printf("%d\n",j);

//---------------------------------

  int k=0;		//--- Iterator for command
  while(!inputEnd)	//--- Getting Command name to execute
  {
   if(input[j]=='\0')
   { 
    inputEnd = true;
    break;
   }
   if(input[j]==' ') break;
   if(input[j]=='>')
   {
    outRedirect = true;
    outFile = 1;
    break;
   }
   temp[k] = input[j];
//   printf("%c",temp[k]);
   j++;   k++;
  }
  temp[k] = '\0';


  if(strcmp("cd",temp)==0)
  {
  	temp[0] = 'c';	temp[1] = 'h';	temp[2] = 'd'; temp[3] = 'i'; temp[4]='r'; temp[5]='\0';
//  	printf("%s\n",temp);
  }


  if(strcmp("gcc",temp)==0)
  {
   sprintf(fmt_input,"/usr/bin/%s",temp);
  }

  else sprintf(fmt_input,"/bin/%s",temp); // Storing path in fmt_input
//  printf("\n%s\n",fmt_input);
  pt[0] = fmt_input;		//--- Putting formatted input as first argument of argument array for execv command
  

  if(strcmp(pt[0],"/bin/exit")==0) break;


  if(strcmp(pt[0],"/bin/bash")==0)
  {
   printf("Are you sure this is the command???\n");
   continue;
  }
  
  j++;

//---------------------------------Getting Arguments for Commands

  int argn = 1;			//--- Counter for arguments

  while(!inputEnd)
  {
   //--------------------
   if(input[j]==' ')
   {
    while(!inputEnd)		//--- Removing Spaces
    {
     if(input[j]=='\0')
     { 
      inputEnd = true;
      break;
     }
     if(input[j]!=' ') break;
     j++;
    }
   }
//   printf("%c\n",input[j]);
   if(inputEnd) break;
   
   //---------------------
   
   k = 0;			//--- Iterator for Arguments
   while(!inputEnd)	//--- Getting Argument name to be passed
   {
    if(input[j]=='\0')
    { 
     inputEnd = true;
     break;
    }
    if(input[j]==' ') break;
    if(input[j]=='>')
    {
     outRedirect = true;
     outFile = argn;
     j++;

     if(input[j]==' ')
     {
      while(!inputEnd)		//--- Removing Spaces
      {
       if(input[j]=='\0')
       { 
        inputEnd = true;
        break;
       }
       if(input[j]!=' ') break;
       j++;
      }
     }
    }
    
    temp[k] = input[j];
    j++;   k++;
   }
   temp[k] = '\0';
//   printf("%s\n",temp);
   pt[argn] = temp;
   argn++;
   if(outRedirect)
   {
    break;
   }
  }

  pt[argn] = NULL; 
 
//----------------------------------- Special case for chdir

 if(strcmp("/bin/chdir",fmt_input)==0)
 {
  chdir(pt[1]);
  continue;
 }


//------------------------------------ Output Redirection
 int fd[2];
 if(outRedirect)
 { 
  pipe(fd);
  open(pt[outFile],O_CREAT,00644);
  fd[1] = open(pt[outFile],O_RDWR);
 }  

//-----------------------------------
  
  int ct;
  for(ct=0;ct<count;ct++)
  {
  int f = fork();  		//--- Making a child process
  if(f==0){
   if(outRedirect)
   {
    close(1);
    dup(fd[1]);
    pt[outFile] = NULL;
   }
   execv(fmt_input,pt);		//--- Executing command in a child process
   printf("Are you sure this is the command??\n");
   continue;
  }
  else
  {
   wait();			//--- Waiting for child process to complete
//   i++;
//   if(i==5) break;
   continue;
  }
  }
 }
 return 0;
}
