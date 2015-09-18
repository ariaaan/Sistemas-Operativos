/*
* Sistemas Operativos I.
* ----------------------
*
* Trabajo Práctico Nº 2.
*
* Alumno: Giles García, Arian.
* Matrícula: 36201187.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h> 

#define BUFFERSIZE 256
#define PATHLENGTH 32

/*
* Declaración de funciones
*/

void get_path_entries();

void builtin_cd(char *path);
void builtin_pwd();

void find_command();
int find_command_in_path(char *path, int size);
int find_command_absolute_path(char *path, int size);

void parse_all(char *command);
void parse_arguments(char *command, char **argv, int *argc);
void parse_command();
int parse_pipe(char *command, char *command_1, char *command_2);

/*
* Declaración de variables globales
*/

//Cantidad de argumentos y arrgelo de argumentos
int my_argc;
char *my_argv[BUFFERSIZE];

//Cantidad de argumentos y arrgelo de argumentos para pipe
int my_argc_2;
char *my_argv_2[BUFFERSIZE];


//Array donde voy a guardar las rutas de la variable PATH
char path_array[PATHLENGTH][BUFFERSIZE];

//Contador de cantidad de rutas en PATH
int length = 0;

//Variable almacena en directorio actual
char cwd[1024];

//Almacena $HOME para mostrar en prompt 
char *home_var;


#define RESET   "\033[0m"					/* Reset */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */

#define READ_END 0
#define WRITE_END 1


/*
* Inicio del programa
*/

int main(int argc, char **argv) {
	//Aloco memoria
	int i;
	for (i = 0; i < 256; ++i) {
  		my_argv[i] = malloc(256*sizeof(char));
	}

	for (i = 0; i < 256; ++i) {
  		my_argv_2[i] = malloc(256*sizeof(char));
	}

	//Obtengo el path y guardo sus entradas
	get_path_entries();

	//Obtengo el PWD
	getcwd(cwd, sizeof(cwd));
	char *current_dir;

	//Obtengo $HOME
	home_var = getenv("HOME");

	//Varaible auxiliar que almacena el comando ingresado
	char command[BUFFERSIZE];

	//Loop principal del 'baash'
	while(1) {
		//Auxiliar, copio la dirección de cwd
		current_dir = cwd;

		//Veo si puedo cambiar '$HOME' por '~' 
		if(strncmp(cwd, home_var, strlen(home_var)) == 0) {
			current_dir = current_dir + strlen(home_var);
			printf(BOLDGREEN "user@baash:~%s$ " RESET, current_dir);
		} else {
			printf(BOLDGREEN "user@baash:%s$ " RESET, current_dir);
		}

   		fgets(command, BUFFERSIZE, stdin);  

   		//Ver si se apreto Ctrl+D
		if(feof(stdin)) {
			printf("\n");
			exit(1);
		}

   		//Parseo comandos
   		parse_all(command);
	}

	return 0;
}


/*
* Definición de funciones
*/


/*
* get_path_entries
* ----------------
*
* Obtengo todos los directorios que 
* estan en la variable de entorno 'PATH'
* y los guardo para luego buscar en esos
* directorios, los comandos pasados al baash.
*/

void get_path_entries() {
	//Guardo en path el valor de la variable de entorno PATH.
	char *path;
	path = getenv("PATH");

	//Divido en ":" y encuentro todos las rutas incluidas en PATH.
	char *token;
	char *divider = ":";

	token = strtok (path, divider);

	//Hasta que no haya más rutas, las agrego a "path_array"
	while (token != NULL) {
		//Guardo la ruta en "path_array"
		strcpy(path_array[length], token);

		//Aumento en uno el contador
		length++;

		//Busco el próximo token
		token = strtok (NULL, divider);
	}
}


/*
* parse_all
* ---------
*
* parametros: command
* 
* Dado el comando ingresado, primero obtengo
* los argumentos y despues parseo el comando
*/
void parse_all(char *command) {
	//Veo si hay que hacer Pipe
	char command_1[BUFFERSIZE];
	char command_2[BUFFERSIZE];

	int pipe_aux = parse_pipe(command, command_1, command_2);

	if(pipe_aux) { 
		//Agrego NULL al final de cada array
		my_argv[my_argc] = NULL;
		my_argv_2[my_argc_2] = NULL;

		//Ejecutar el pipe
		int fd[2];
		pid_t pid;

		if(pipe(fd) == -1) {
			printf("Error creating pipe\n");
			exit(1);
		}

		if((pid = fork()) == -1) {
			printf("Error creating child process\n");
			exit(1);
		} else if (pid == 0) {
			//Ciero el READ_END del pipe
			close(fd[0]);
			//Hago que 1 sea el WRITE_END del pipe              
          	dup2(fd[1],1);  
          	//Ciero los fd que sobran
          	close(fd[1]);

          	//Ejecuto
          	execv(my_argv[0], my_argv);
          	_exit(1);
		} 

		//Ciero el WRITE_END del pipe
		close(fd[1]);   
		//Hago que 0 sea el REAND_END del pipe  
      	dup2(fd[0],0);  
      	//Cierro los fd que sobran
      	close(fd[0]);   

      	//Ejecuto
		execv(my_argv_2[0], my_argv_2);
	} else {
		//Guardo los argumentos en argv y argc
		parse_arguments(command, my_argv, &my_argc);

		//Veo que tengo que hacer leyendo my_argc y my_argv
		parse_command();
	}
}


int parse_pipe(char *command, char *command_1, char *command_2) {
	char aux[BUFFERSIZE];
	strcpy(aux, command);

	int pipe = 0;

	char *token_1;
	char *token_2;
	char *divider = "|";


	token_1 = strtok(aux, divider);	
	token_2 = strtok(NULL, divider);
	
	if(token_2 == NULL) {
		pipe = 0;
	} else {
		pipe = 1;
		strncpy(command_1, token_1, BUFFERSIZE);
		strncpy(command_2, token_2, BUFFERSIZE);

		parse_arguments(command_1, my_argv, &my_argc);
		parse_arguments(command_2, my_argv_2, &my_argc_2);
	}

	return pipe;
}


/*
* parse_arguments
* ---------------
* 
* Dado el comando ingresado, leo todos los
* argumentos y guardo tanto los valores
* como la cantidad en 'my_argc' y 'my_argv'
*/
void parse_arguments(char *command, char **argv, int *argc) {
	//Copio el comando en aux
	char aux[BUFFERSIZE];
	strcpy(aux, command);

	//Divido en " " 
	char *divider = " ";
  	char *token;

  	//Leo la primera parte
    token = strtok(aux, divider);

    int count = 0;

    while(token != NULL) {
    	//Guardo el argumento en argv
    	strcpy(argv[count], token);

		//Busco nuevo argumento
    	token = strtok(NULL, divider);

    	//Aumento el contador
    	count++;
    }

    *argc = count;
 
    int i;

    for(i = 0; i < *argc; i++) {
    	//Si tiene '\n' al final, la borro
		if (argv[i][strlen (argv[i]) - 1] == '\n') {
	    	argv[i][strlen (argv[i]) - 1] = '\0';
	    }
    }
}

/*
* parse_command
* -------------
* 
* Dado el comando ingresado, que se encuentra
* en 'my_argv[0]', debo ver si es un comando 
* builtin, un programa que debo buscar en 
* los directorios del 'PATH' o si es una
* ruta, ya sea relativa o absoluta al programa.
*/
void parse_command() {
	//Variable con el comando que se pidio
	char *command = malloc(BUFFERSIZE*sizeof(char));

	//Si no se escribio un comando 
	if(my_argc <= 0) {
		//Salgo de la función
		return;
	} 
		
	//Lo busco y lo copio en 'command'
	strcpy(command, my_argv[0]);

	//Si tiene '\n' al final, la borro
	if (command[strlen (command) - 1] == '\n') {
    	command[strlen (command) - 1] = '\0';
    }

    //Priemro veo si se llamo a alguna funcion builtin
	if(strcmp(command, "cd") == 0) {
		char cd_path[BUFFERSIZE] = "";
		int i;
		for(i = 1; i < my_argc; i++) {
			if(i == 1) {
				strcat(cd_path, my_argv[i]);
			} else {
				strcat(cd_path, " ");
				strcat(cd_path, my_argv[i]);
			}
		}

		builtin_cd(cd_path);
	} else if(strcmp(command, "pwd") == 0) {
		//Si se llama a 'pwd' sin argumentos
		if(my_argc == 1) {
			//Llamo a la función builtin 'pwd'
			builtin_pwd();
		} else {
			//Si se le pasaron argumentos extras, muestro el uso de la función
			printf("pwd usage: pwd\n");
		}
	} else if(strcmp(command, "exit") == 0) {
		//Si se llamo a 'exit' sin argumentos
		if(my_argc == 1) {
			//Salgo del programa
			exit(1);
		} else {
			//Si se le pasaron argumentos extras, muestro el uso de la función
			printf("exit usage: exit\n");
		}
	} else {
		//Si no se llamo a ninguna builtin debo obtener 
		//el path del programa que esta en my_argv[0]
		find_command();
	}

}

/*
* find_command
* ------------
* 
* Busco el comando que esta en 'my_argv[0]'
* en algun directorio de la variable 'PATH',
* si no lo encuentro, lo transformo a un
* path absoluto, y veo si lo encuentro.
*/
void find_command() {
	//Variable donde guardo el path que encontre
	char *path = malloc(PATH_MAX*sizeof(char));

	//Variable para ver si encontre o no el path
	int found = 0;

	//Variables auxiliares
	char aux[BUFFERSIZE];
	strncpy(aux, my_argv[0], BUFFERSIZE);

	//Divido en "/" para ver si lo tengo que buscar en el path
	char *divider = "/";
  	char *token;

  	//Veo si pude dividir en "/"
    token = strtok(aux, divider);
    token = strtok(NULL, divider);

    //Si no pude dividr en barras, busco en el path
    if(token == NULL) {
    	//Actualizo found si lo encontre o no
    	found = find_command_in_path(path, PATH_MAX);
    }

    //Si no lo encontre en algun directorio de la variable PATH
	if(!found) {
		//Lo busco como ruta absoluta o realtiva
		found = find_command_absolute_path(path, PATH_MAX);
	} 

	//Si lo encontre
	if(found) {
		int ampersand = 0;

		//Veo si se agrego el '&'
		if(strcmp(my_argv[my_argc-1], "&") == 0) {
			ampersand = 1;
			my_argv[my_argc-1] = NULL;
		} 

		pid_t f = fork();
		strcpy(my_argv[0], path);

		if(f == 0) {
			my_argv[my_argc] = NULL;

			execv(my_argv[0], my_argv);

			exit(1);
		}

		if(!ampersand) {
			int status;
			wait(&status);
		}	

	} else {
		//Sino imprimo error
		printf("Couldn't find file\n");
	}
}

/*
* find_command_in_path
* --------------------
* 
* Busco el comando que esta en 'my_argv[0]'
* en algun directorio de la variable 'PATH',
* si lo encuentro, guardo el path donde lo encontre.
*/
int find_command_in_path(char *path, int size) {
	//Nos indica si se encontró el programa o no
	int found = 0;

	//Variable auxiliar
	char path_aux[BUFFERSIZE];

	int i;
	for(i = 0; i < length; i++) {
		strcpy(path_aux, "");

		//Armo el path completo
		strcat(path_aux, path_array[i]);
		strcat(path_aux, "/");
		strcat(path_aux, my_argv[0]);

		//Si tiene '\n' al final, la borro
		if (path_aux[strlen (path_aux) - 1] == '\n') {
    		path_aux[strlen (path_aux) - 1] = '\0';
    	}

		//Veo si el programa existe
		if(access(path_aux, F_OK) != -1 ) {
			//Si existe copio el path en 'path'
			strncpy(path, path_aux, PATH_MAX);
			//Pongo la bandera de 'found' en 1
			found = 1;
			//Salgo del loop
			break;
		} else {
			//Si no existe no hago nada
		}
	}

	return found;
}

/*
* find_command_absolute_path
* --------------------------
* 
* Busco el comando que esta en 'my_argv[0]'
* obteniendo primero su ruta absoluta, si lo
* encuentro, guardo el path donde lo encontre.
*/
int find_command_absolute_path(char *path, int size) {
    //Variables auxiliares
    char real_path[PATH_MAX + 1]; 
    char *control;

    int found = 0;

    //Busco el path real
    control = realpath(my_argv[0], real_path);

    if(real_path != NULL) {

    	//Si tiene '\n' al final, la borro
		if (real_path[strlen (real_path) - 1] == '\n') {
	    	real_path[strlen (real_path) - 1] = '\0';
	    }

    	//Veo si el programa existe
		if(access(real_path, F_OK) != -1 ) {
			strncpy(path, real_path, PATH_MAX);

			found = 1;
		}

    } else {
    	printf("Error parsing path\n");
    }

    return found;
}

/*
* builtin_cd
* ----------
* 
* Cambio de directorio según el 
* path que me pasaron (puede ser
* un path relativo, o absoluto).
*/
void builtin_cd(char *path) {
	//Copio el comando en aux
	char aux[BUFFERSIZE];
	strcpy(aux, path);

	if(strcmp(path, "") == 0) {
		//Si se mando NULL, debo cambiar al $HOME
		if(chdir(home_var) != -1) {
			//Busco el current working directory y lo guardo
	   		getcwd(cwd, sizeof(cwd));
		} else {
			printf("Error %d. Couldn't change directory\n", errno);
		}	
	} else {
		//Si tiene '\n' al final, la borro
		if (path[strlen (path) - 1] == '\n') {
	    	path[strlen (path) - 1] = '\0';
	    }

		if(chdir(path) != -1) {
			//Busco el current working directory y lo guardo
	   		getcwd(cwd, sizeof(cwd));
		} else {
			printf("Error %d. Couldn't change directory\n", errno);
		}	
	}
	
	
}

/*
* builtin_pwd
* -----------
* 
* Busco el directorio en el que me 
* encuentro actualmente y lo imprimo.
*/
void builtin_pwd() {
	getcwd(cwd, sizeof(cwd));
    printf("%s\n", cwd);
}