/*
* Sistemas Operativos I.
* ----------------------
*
* Trabajo Práctico Nº 2.
*
* Alumno: Giles García, Arian.
* Matrícula: 36201187.
*/

/*
* Inclusión de liberrias
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h> 
#include <fcntl.h>

/*
* Definición de constantes
*/

/* Tamaños */
#define BUFFERSIZE 256
#define PATHLENGTH 32

/* Colores para diferenciar de la terminal de linux */
#define RESET   "\033[0m"					
#define BOLDGREEN   "\033[1m\033[32m"     

/* Tipos de Pipes */
#define PIPE_TYPE_1 1
#define PIPE_TYPE_2 2
#define PIPE_TYPE_3 3

/*
* Declaración de funciones
*/

void get_path_entries();

void builtin_cd(char *path);
void builtin_pwd();

void find_command();
int find_command_in_path(char *path, int size, char *command);
int find_command_absolute_path(char *path, int size, char *command);

void parse_all(char *command);
void parse_arguments(char *command, char **argv, int *argc);
void parse_command();
int parse_pipe(char *command, char *command_1, char *command_2);
void execute_pipe(char *command_1, char *command_2, int pipe_type);

void trim(char *string);

/*
* Declaración de variables globales
*/

/* Cantidad de argumentos y arreglo de argumentos */
int my_argc;
char *my_argv[BUFFERSIZE];

/* Cantidad de argumentos y arrgelo de argumentos para los casos de pipe */
int my_argc_2;
char *my_argv_2[BUFFERSIZE];

/* Array que almacena las rutas de la variable PATH */
char path_array[PATHLENGTH][BUFFERSIZE];

/* Contiene la cantidad de entradas en PATH */
int path_length = 0;

/* Almacena el directorio actual */
char cwd[1024];

/* Almacena $HOME para mostrar en prompt */
char *home_var;

/* Nombre de usuario y nombre de host para mostrar en prompt */ 
char user_name[BUFFERSIZE];
char host_name[BUFFERSIZE];

/*
* Inicio del programa
*/

int main(int argc, char **argv) {
	/* Obtengo el nombre de usuario y host */
	getlogin_r(user_name, BUFFERSIZE);
	gethostname(host_name, BUFFERSIZE);

	/* Obtengo el path y guardo sus entradas */
	get_path_entries();

	/* Obtengo el directorio actual */
	getcwd(cwd, sizeof(cwd));
	char *current_dir;

	/* Obtengo $HOME */
	home_var = getenv("HOME");

	/* Varaible que almacena el comando ingresado */
	char command[BUFFERSIZE];

	/* Loop principal del 'baash' */
	while(1) {
			/* Aloco memoria para las variables de argumentos */
			int i;
			for (i = 0; i < 256; ++i) {
		  		my_argv[i] = malloc(256*sizeof(char));
			}

			for (i = 0; i < 256; ++i) {
		  		my_argv_2[i] = malloc(256*sizeof(char));
			}

		/* Auxiliar, copio la dirección del cwd */
		current_dir = cwd;

		/* Veo si puedo cambiar '$HOME' por '~' */
		if(strncmp(cwd, home_var, strlen(home_var)) == 0) {
			current_dir = current_dir + strlen(home_var);
			/* Imprimo el prompt */
			printf(BOLDGREEN "%s@%s:~%s$ " RESET, user_name, host_name, current_dir);
		} else {
			/* Imprimo el prompt */
			printf(BOLDGREEN "%s@%s:%s$ " RESET, user_name, host_name, current_dir);
		}

		/* Leo la entrada por teclado */ 
   		fgets(command, BUFFERSIZE, stdin);  

   		/* Veo si se apreto Ctrl+D */
		if(feof(stdin)) {
			printf("\n");
			/* Salgo del programa */
			exit(1);
		}

   		/* Remuevo los espacios y parseo comandos */
   		trim(command);
   		parse_all(command);

   		/* Libero memoria de las variables de argumentos */
		for (i = 0; i < 256; ++i) {
			free(my_argv[i]);
		}
		for (i = 0; i < 256; ++i) {
			free(my_argv_2[i]);	
		}

	}

	return 0;
}

/*
* Definición de funciones
*/

/*
 * Función:  get_path_entries 
 * --------------------------
 *  Obtiene los directorios que almacena la variable de entorno $PATH.
 *
 */

void get_path_entries() {
	/* Guardo el contenido de la variable de entorno PATH. */
	char *path;
	path = getenv("PATH");

	/* Divido en ":" y encuentro todos las rutas incluidas en PATH. */
	char *token;
	char *divider = ":";

	token = strtok (path, divider);

	/* Hasta que no haya más rutas */
	while (token != NULL) {
		/* Guardo la ruta en "path_array" */
		strcpy(path_array[path_length], token);

		/* Aumento en uno el contador de rutas del path*/
		path_length++;

		/* Busco el próximo token */
		token = strtok (NULL, divider);
	}
}

/*
 * Función:  parse_all 
 * ---------------------------
 *  Parsea el commando ingresado por teclado, 
 *  según eso realiza diferentes acciones.
 *
 *  command: el comando ingresado por el usuario
 */

void parse_all(char *command) {
	/* Veo si hay que hacer Pipe */
	char command_1[BUFFERSIZE];
	char command_2[BUFFERSIZE];

	int pipe_type = parse_pipe(command, command_1, command_2);

	if(pipe_type) { 
		/* Si se encontro un pipe, lo ejecuto */
		execute_pipe(command_1, command_2, pipe_type);
	} else {
		/* Guardo los argumentos en argv y argc */
		parse_arguments(command, my_argv, &my_argc);

		/* Veo que tengo que hacer leyendo my_argc y my_argv */
		parse_command();
	}
}

/*
 * Función:  execute_pipe 
 * ----------------------
 *  Ejecuta el pipe ingresado por el usuario.
 *
 *  command_1: primer comando del pipe
 *  command_2: segundo comando del pipe
 *  pipe_type: tipo de pipe
 */

void execute_pipe(char *command_1, char *command_2, int pipe_type) {
	/* Variables auxiliares */
	char *path_1 = malloc(PATH_MAX*sizeof(char));
	char *path_2 = malloc(PATH_MAX*sizeof(char));

	/* Agrego NULL al final de cada array */
	my_argv[my_argc] = NULL;
	my_argv_2[my_argc_2] = NULL;

	/* Quito espacios antes y despues */
	trim(command_1);
    trim(command_2);

    /* Pareso los argumentos de cada uno */
    parse_arguments(command_1, my_argv, &my_argc);
    parse_arguments(command_2, my_argv_2, &my_argc_2);

    int found_1 = 0;
    int found_2 = 0;

    int status;

    /* Dependiendo el tipo de pipe, realizo diferentes acciones */
    switch(pipe_type) {
    	case PIPE_TYPE_1:
    		/* Si el pipe es tipo "|", los dos comandos deben existir */
    		
    		/* Veo si esta o no en el PATH */
		found_1 = find_command_in_path(path_1, PATH_MAX, my_argv[0]);
		/* Si no lo encontre en algun directorio de la variable PATH */
		if(!found_1) {
			/* Lo busco como ruta absoluta o realtiva */
			found_1 = find_command_absolute_path(path_1, PATH_MAX, my_argv[0]);
		} 

    		/* Veo si esta o no en el PATH */
		found_2 = find_command_in_path(path_2, PATH_MAX, my_argv_2[0]);
		/* Si no lo encontre en algun directorio de la variable PATH */
		if(!found_2) {
			/* Lo busco como ruta absoluta o realtiva */
			found_2 = find_command_absolute_path(path_2, PATH_MAX, my_argv_2[0]);
		} 

		/* Si encontre los 2 ejecuto el pipe */
		if(found_1 && found_2) {
			strcpy(my_argv[0], path_1);
			strcpy(my_argv_2[0], path_2);

			/* File Descriptor y PID para fork() */
			int fd[2];
			pid_t pid;

			/* Si hay error al crear el pipe, salgo */
			if(pipe(fd) == -1) {
				printf("Error creating pipe\n");
				exit(1);
			} 

			/* Hago un fork() */
			pid = fork();

			/* Si hay error, salgo */
			if(pid == -1) {
				printf("Error creating child process\n");
				exit(1);
			} else if (pid == 0) {
				/* Esto lo ejecuta el hijo */
				/* Redirecciono el pipe */
			
				/* Ciero el READ_END del pipe */
				close(fd[0]);
				/* Hago que 1 sea el WRITE_END del pipe */
			       	dup2(fd[1],1);  
		          	/* Ciero los fd que sobran */
		          	close(fd[1]);

		          	/* Ejecuto */
		          	execv(my_argv[0], my_argv);
		          	perror("Error Child 1.\n");
		          	_exit(1);
			} else {
			    	/* El padre vuelve a crear otro hijo */
			    	pid = fork();

				/* Si hay error, salgo */
			    	if(pid == -1) {
					printf("Error creating child process\n");
					exit(1);
				} else if (pid == 0) {
					/* Esto lo ejecuta el hijo */
					/* Redirecciono el pipe */
				
					/* Ciero el WRITE_END del pipe */
					close(fd[1]);   
					/* Hago que 0 sea el REAND_END del pipe */
				      	dup2(fd[0],0);  
				      	/* Cierro los fd que sobran */
				      	close(fd[0]);   

				      	/* Ejecuto */
					execv(my_argv_2[0], my_argv_2);
					exit(1);
					perror("Error Child 2.\n");
				} 
			}

			/* Espero que los hijos terminen */
		    	while (wait() < 0) {

			}
		}

    		break;

    		case PIPE_TYPE_2:
    			/* Si el pipe es tipo ">" debe existir el primer comando, el otro es un archivo */
    			/* Veo si esta o no en el PATH */
    			found_1 = find_command_in_path(path_1, PATH_MAX, my_argv[0]);

			/* Si no lo encontre en algun directorio de la variable PATH */
			if(!found_1) {
				/* Lo busco como ruta absoluta o realtiva */
				found_1 = find_command_absolute_path(path_1, PATH_MAX, my_argv[0]);
			} 

			/* Si lo encontre, ejecuto el pipe */
			if(found_1) {
				strcpy(my_argv[0], path_1);

				/* File Descriptor y PID para fork() */
				pid_t pid;
				pid = fork();

				/* Si hay error al hacer el fork(), salgo */
				if (pid == -1)   {
				 	perror("Error forking"); 
				 	exit(1);
				} else if (pid == 0) {        
					/* Esto lo ejecuta el hijo */
					/* Redirecciono el pipe */

					 /* Abro archivo */
					 int fd = open(my_argv_2[0], O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
					 
					/* Hago que STDOUT vaya al archivo */
					dup2(fd, 1);   
				    	close(fd);     

				      	/* Ejecuto */
			          	execv(my_argv[0], my_argv);
			          	perror("Error Child 1.\n");
			          	_exit(1);
				}
			}

			/* Espero por el hijo */
			status;
			wait (&status);

			break;

		case PIPE_TYPE_3:
			/* Si el pipe es tipo ">" debe existir el primer comando, el otro es un archivo */
    			/* Veo si esta o no en el PATH */
		   	found_1 = find_command_in_path(path_1, PATH_MAX, my_argv[0]);

			/* Si no lo encontre en algun directorio de la variable PATH */
			if(!found_1) {
				/* Lo busco como ruta absoluta o realtiva */
				found_1 = find_command_absolute_path(path_1, PATH_MAX, my_argv[0]);
			} 

			/* Si lo encontre, ejecuto el pipe */
			if(found_1) {
				strcpy(my_argv[0], path_1);
				
				/* File Descriptor y PID para fork() */	
				pid_t pid;
				pid = fork();

				/* Si hay error al crear el pipe, salgo */
				if (pid == -1)   {
				 	perror("Error forking"); 
				 	exit(1);
				} else if (pid == 0) {        
					/* Esto lo ejecuta el hijo */
					/* Redirecciono el pipe */

				 	/* Abro el archivo */
				 	int fd = open(my_argv_2[0], O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR);

				 	/* Hago que STDOUT vaya al archivo */
				 	dup2(fd, 0);   
			    	close(fd);     

			      	/* Ejecuto */
		          	execv(my_argv[0], my_argv);
		          	perror("Error Child 1.\n");
		          	_exit(1);
				 }
			}

			/* Espero por el hijo */
			status;
			wait (&status);

			break;

		default:
			break;
    }

}

/*
 * Función:  parse_pipe 
 * --------------------
 *  Parsea el commando ingresado por teclado, 
 *  se fija si hay un "|", ">" o "<" en él.
 *
 *  command: el comando ingresado por el usuario
 *  command_1: donde se almacenara el primer comando del pipe
 *  command_2: donde se almacenara el segundo comando del pipe
 */

int parse_pipe(char *command, char *command_1, char *command_2) {
	char aux[BUFFERSIZE];

	int pipe = 0;

	char *token_1;
	char *token_2;
	
	/* Veo si hay pipe tipo "|" */
	strcpy(aux, command);
	char *divider = "|";

	token_1 = strtok(aux, divider);	
	token_2 = strtok(NULL, divider);
	
	if(token_2 != NULL) {
		pipe = PIPE_TYPE_1;
		strncpy(command_1, token_1, BUFFERSIZE);
		strncpy(command_2, token_2, BUFFERSIZE);

		parse_arguments(command_1, my_argv, &my_argc);
		parse_arguments(command_2, my_argv_2, &my_argc_2);
		return pipe;
	}

	/* Veo si hay pipe tipo ">" */
	strcpy(aux, command);
	divider = ">";

	token_1 = strtok(aux, divider);	
	token_2 = strtok(NULL, divider);
	
	if(token_2 != NULL) {
		pipe = PIPE_TYPE_2;
		strncpy(command_1, token_1, BUFFERSIZE);
		strncpy(command_2, token_2, BUFFERSIZE);

		parse_arguments(command_1, my_argv, &my_argc);
		parse_arguments(command_2, my_argv_2, &my_argc_2);
		return pipe;
	}

	/* Veo si hay pipe tipo "<" */
	strcpy(aux, command);
	divider = "<";

	token_1 = strtok(aux, divider);	
	token_2 = strtok(NULL, divider);
	
	if(token_2 != NULL) {
		pipe = PIPE_TYPE_3;
		strncpy(command_1, token_1, BUFFERSIZE);
		strncpy(command_2, token_2, BUFFERSIZE);

		parse_arguments(command_1, my_argv, &my_argc);
		parse_arguments(command_2, my_argv_2, &my_argc_2);
		return pipe;
	}

	return pipe;
}

/*
 * Función:  parse_arguments 
 * -------------------------
 *  Parsea el commando ingresado por teclado, 
 *  guarda los argumentos en las variables correspondientes.
 *
 *  command: el comando ingresado por el usuario
 *  argv: valores de los argumentos
 *  argc: cantidad de argumentos
 */

void parse_arguments(char *command, char **argv, int *argc) {
	/* Copio el comando en aux */
	char aux[BUFFERSIZE];
	strcpy(aux, command);

	/* Divido en " " */
	char *divider = " ";
  	char *token;

  	/* Leo la primera parte */
    token = strtok(aux, divider);

    int count = 0;

    while(token != NULL) {
    	/* Guardo el argumento en argv */
    	strcpy(argv[count], token);

		/* Busco nuevo argumento */
    	token = strtok(NULL, divider);

    	/* Aumento el contador */
    	count++;
    }

    /* Guardo al cantidad de argumentos en argc */
    *argc = count;
 
    int i;

    /* Elimino la "\n" de los argumentos */
    for(i = 0; i < *argc; i++) {
    	//Si tiene '\n' al final, la borro
		if (argv[i][strlen (argv[i]) - 1] == '\n') {
	    	argv[i][strlen (argv[i]) - 1] = '\0';
	    }
    }
}

/*
 * Función:  parse_command 
 * -----------------------
 *  Dado el comando ingresado, que se encuentra
 *  en 'my_argv[0]', debo ver si es un comando 
 *  builtin, un programa que debo buscar en 
 *  los directorios del 'PATH' o si es una
 *  ruta, ya sea relativa o absoluta al programa.
 */

void parse_command() {
	/* Variable con el comando que se pidio */
	char *command = malloc(BUFFERSIZE*sizeof(char));

	/* Si no se escribio un comando */
	if(my_argc <= 0) {
		/* Salgo de la función */
		return;
	} 
		
	/* Lo busco y lo copio en 'command' */
	strcpy(command, my_argv[0]);

	/* Si tiene '\n' al final, la borro */
	if (command[strlen (command) - 1] == '\n') {
    	command[strlen (command) - 1] = '\0';
    }

    /* Primero veo si se llamo a alguna funcion builtin */
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
		/* Si se llama a 'pwd' sin argumentos */
		if(my_argc == 1) {
			/* Llamo a la función builtin 'pwd' */
			builtin_pwd();
		} else {
			/* Si se le pasaron argumentos extras, muestro el uso de la función */
			printf("pwd usage: pwd\n");
		}
	} else if(strcmp(command, "exit") == 0) {
		/* Si se llamo a 'exit' sin argumentos */
		if(my_argc == 1) {
			/* Salgo del programa */
			exit(1);
		} else {
			/* Si se le pasaron argumentos extras, muestro el uso de la función */
			printf("exit usage: exit\n");
		}
	} else {
		/* Si no se llamo a ninguna builtin debo obtener el path del programa que esta en my_argv[0] */
		find_command();
	}

}

/*
 * Función:  find_command 
 * ----------------------
 *  Busco el comando que esta en 'my_argv[0]'
 *  en algun directorio de la variable 'PATH',
 *  si no lo encuentro, lo transformo a un
 *  path absoluto, y veo si lo existe.
 */

void find_command() {
	/* Variable donde guardo el path que encontre */
	char *path = malloc(PATH_MAX*sizeof(char));

	/* Variable para ver si encontre o no el path */
	int found = 0;

	/* Variables auxiliares */
	char aux[BUFFERSIZE];
	strncpy(aux, my_argv[0], BUFFERSIZE);

	/* Divido en "/" para ver si lo tengo que buscar en el path */
	char *divider = "/";
  	char *token;

    token = strtok(aux, divider);
    token = strtok(NULL, divider);

    /* Si no pude dividr en barras, busco en el path */
    if(token == NULL) {
    	/* Actualizo found si lo encontre o no */
    	found = find_command_in_path(path, PATH_MAX, my_argv[0]);
    }

    /* Si no lo encontre en algun directorio de la variable PATH */
	if(!found) {
		/* Lo busco como ruta absoluta o realtiva */
		found = find_command_absolute_path(path, PATH_MAX, my_argv[0]);
	} 

	/* Si lo encontre */
	if(found) {
		int ampersand = 0;

		/* Veo si se agrego el '&' */
		if(strcmp(my_argv[my_argc-1], "&") == 0) {
			ampersand = 1;
			my_argv[my_argc-1] = NULL;
		} 

		/* Creo proceso hijo */
		pid_t f = fork();
		strcpy(my_argv[0], path);

		if(f == 0) {
			my_argv[my_argc] = NULL;

			/* Ejecuto el comando */
			execv(my_argv[0], my_argv);

			exit(1);
		}
 		
 		/* Si tenia un '&' al final, no espero */
		if(!ampersand) {
			int status;
			wait(&status);
		}	

	} else {
		/* Si no se encontro, no hago nada */
	}
}

/*
 * Función:  find_command_in_path 
 * ------------------------------
 *  Busco el comando que esta en 'my_argv[0]'
 *  en algun directorio de la variable 'PATH',
 *  si lo encuentro, guardo el path en donde lo encontre.
 *
 *  path: donde guardo el path absoluto al comando 
 *  size: tamalo máximo del buffer
 *  command: comando ingresado por el usuario
 */

int find_command_in_path(char *path, int size, char *command) {
	/* Nos indica si se encontró el programa o no */
	int found = 0;

	/* Variable auxiliar */
	char path_aux[BUFFERSIZE];

	int i;
	for(i = 0; i < path_length; i++) {
		strcpy(path_aux, "");

		/* Armo el path completo */
		strcat(path_aux, path_array[i]);
		strcat(path_aux, "/");
		strcat(path_aux, command);

		/* Si tiene '\n' al final, la borro */
		if (path_aux[strlen (path_aux) - 1] == '\n') {
    		path_aux[strlen (path_aux) - 1] = '\0';
    	}

		/* Veo si el programa existe */
		if(access(path_aux, F_OK) != -1 ) {
			/* Si existe copio el path en 'path' */
			strncpy(path, path_aux, PATH_MAX);
			/* Pongo la bandera de 'found' en 1 */
			found = 1;
			/* Salgo del loop */
			break;
		} else {
			/* Si no existe no hago nada */
		}
	}

	return found;
}

/*
 * Función:  find_command_absolute_path 
 * ------------------------------------
 *  Busco el comando que esta en 'my_argv[0]'
 *  obteniendo primero su ruta absoluta, si lo
 *  encuentro, guardo el path donde lo encontre.
 *
 *  path: donde guardo el path absoluto al comando 
 *  size: tamalo máximo del buffer
 *  command: comando ingresado por el usuario
 */

int find_command_absolute_path(char *path, int size, char *command) {
    /* Variables auxiliares */
    char real_path[PATH_MAX + 1]; 
    char *control;

    int found = 0;

    /* Busco el path real */
    control = realpath(command, real_path);

    if(real_path != NULL) {

    	/* Si tiene '\n' al final, la borro */
		if (real_path[strlen (real_path) - 1] == '\n') {
	    	real_path[strlen (real_path) - 1] = '\0';
	    }

    	/* Veo si el programa existe */
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
 * Función:  builtin_cd 
 * --------------------
 *  Cambio de directorio.
 *
 *  path: directorio al que se quiere cambiar
 */

void builtin_cd(char *path) {
	/* Copio el comando en aux */
	char aux[BUFFERSIZE];
	strcpy(aux, path);

	if(strcmp(path, "") == 0) {
		/* Si se mando NULL, debo cambiar al $HOME */
		if(chdir(home_var) != -1) {
			/* Busco el current working directory y lo guardo */
	   		getcwd(cwd, sizeof(cwd));
		} else {
			printf("Error %d. Couldn't change directory\n", errno);
		}	
	} else {
		/* Si tiene '\n' al final, la borro */
		if (path[strlen (path) - 1] == '\n') {
	    	path[strlen (path) - 1] = '\0';
	    }

		if(chdir(path) != -1) {
			/* Busco el current working directory y lo guardo */
	   		getcwd(cwd, sizeof(cwd));
		} else {
			printf("Error %d. Couldn't change directory\n", errno);
		}	
	}
	
	
}

/*
 * Función:  builtin_pwd 
 * ---------------------
 *  Imprimo el directorio actual.
 */

void builtin_pwd() {
	getcwd(cwd, sizeof(cwd));
    printf("%s\n", cwd);
}

/*
 * Función:  trim
 * --------------
 *  Quita los espacios al comienzo
 *  y al final del string.
 */

void trim(char *string) {
    char *p = string;
    int l = strlen(p);

    while(isspace(p[l-1])) p[--l] = 0;
    while(*p && isspace(*p)) ++p, --l;

    memmove(string, p, l+1);
}
