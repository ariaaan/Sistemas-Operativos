#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFERSIZE 256

//Cantidad de argumentos y arrgelo de argumentos
int my_argc;
char *my_argv[BUFFERSIZE];

//Cantidad de argumentos y arrgelo de argumentos para pipe
int my_argc_2;
char *my_argv_2[BUFFERSIZE];

int main(void) {
	
int pfildes[2];
pid_t pid;

 if (pipe(pfildes) == -1)    {
 	perror("demo"); exit(1);
 }
 
 if ((pid = fork()) == -1)   {
 	perror("demo"); exit(1);

 } else if (pid == 0) {        /* child:  "cat a"                      */
          close(pfildes[0]);    /* close read end of pipe               */
          dup2(pfildes[1],1);   /* make 1 same as write-to end of pipe  */
          close(pfildes[1]);    /* close excess fildes                  */
          execlp("ls","ls", NULL);
          perror("demo");       /* still around?  exec failed           */
          _exit(1);             /* no flush                             */ 
        }
    else {                      /* parent:  "/usr/bin/wc"               */
          close(pfildes[1]);    /* close write end of pipe              */
          dup2(pfildes[0],0);   /* make 0 same as read-from end of pipe */
          close(pfildes[0]);    /* close excess fildes                  */
          execlp("more","more",NULL);
          perror("demo");       /* still around?  exec failed           */
          exit(1);	        /* parent flushes                       */
        }

	return 0;
}