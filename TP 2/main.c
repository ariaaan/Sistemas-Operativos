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

void get_path(char *command);
void parse_cd(char *command);
void parse_pwd(char *command);
void parse_exit(char *command);
void parse_command(char *command);
void parse_all(char *command);

/*
* Declaración de variables globales
*/

//Array donde voy a guardar las rutas de la variable PATH
char path_array[PATHLENGTH][BUFFERSIZE];

//Contador de cantidad de rutas en PATH
int length = 0;

//Variable almacena en directorio actual
char cwd[1024];

int main(int argc, char **argv) {

	//Busco el current working directory y lo guardo
   	getcwd(cwd, sizeof(cwd));
    printf("\nCurrent Dir: %s\n\n", cwd);

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

		//Debug option
		//printf("path_array[%d] -> %s\n", length, path_array[length]);

		//Aumento en uno el contador
		length++;

		//Busco el próximo token
		token = strtok (NULL, divider);
	}

	//Varaible auxiliar que almacena el comando ingresado
	char command[BUFFERSIZE];

	//Loop principal del 'baash'
	while(1) {
		printf("arian@baash: ");
   		fgets(command, BUFFERSIZE, stdin);  

   		//Parseo comandos
   		parse_all(command);
	}

	return 0;
}


/*
* Definición de funciones
*/

void parse_all(char *command) {
	//Veo si encuentro el real path del ejecutable
	parse_command(command);

   	//Veo si hay que salir
   	parse_exit(command);

   	//Veo si hay que cambiar de directorio
   	parse_cd(command);

   	//Veo si pidieron el 'pwd'
   	parse_pwd(command);
}

void parse_command(char *command) {
	//Copio el comando en aux
	char aux[BUFFERSIZE];
	strcpy(aux, command);

	//Divido en " " 
	char *divider = " ";
  	char *token;

  	//Leo la primera parte
    token = strtok(aux, divider);

    //Variables auxiliares
    char real_path[PATH_MAX + 1]; 
    char *control;

    //Busco el path real
    control = realpath(token, real_path);

    if(real_path != NULL) {
		printf("Real Path: %s", real_path);
    } else {
    	printf("Error parsing path\n");
    }

}

void parse_cd(char *command) {
	//Copio el comando en aux
	char aux[BUFFERSIZE];
	strcpy(aux, command);
	
	//Divido en " " 
	char *divider = " ";
  	char *token;

  	//Leo la primera parte
    token = strtok(aux, divider);

    //Si es 'cd' busco el directorio
    if(strcmp(token, "cd") == 0) {
    	//Obtengo el directorio
		token = strtok(NULL, divider);

		//Borro en '\n' del final.
		token[strlen (token) - 1] = '\0';

		if(chdir(token) != -1) {
			//Busco el current working directory y lo guardo
   			getcwd(cwd, sizeof(cwd));
			printf("Directory changed to: %s\n", cwd);
		} else {
			printf("Error %d.\n", errno);
		}
	}
}

void parse_exit(char *command) {
	//Copio el comando en aux
	char aux[BUFFERSIZE];
	strcpy(aux, command);

	//Si tiene '\n' al final, la borro
	if (aux[strlen (aux) - 1] == '\n') {
    	aux[strlen (aux) - 1] = '\0';
    }

	//Ver si se apreto Ctrl+D
	if(feof(stdin)) {
		printf("\n\n");
		exit(1);
	}

	//O si se escribio el commando exit
	if(strcmp(aux, "exit") == 0) {
		printf("\n");
		exit(1);
	}
}

void parse_pwd(char *command) {
	//Copio el comando en aux
	char aux[BUFFERSIZE];
	strcpy(aux, command);

	//Si tiene '\n' al final, la borro
	if (aux[strlen (aux) - 1] == '\n') {
    	aux[strlen (aux) - 1] = '\0';
    }

	//O si se escribio el commando exit
	if(strcmp(aux, "pwd") == 0) {
		getcwd(cwd, sizeof(cwd));
    	printf("Current Dir: %s\n", cwd);
	}
}

void get_path(char *command) {

	//Nos indica si se encontró el programa o no
	int found = 0;

	//Variable auxiliar
	char path_aux[BUFFERSIZE];
	char path[BUFFERSIZE];

	int i;
	for(i = 0; i < length; i++) {
		strcpy(path_aux, "");

		//Armo el path completo
		strcat(path_aux, path_array[i]);
		strcat(path_aux, "/");
		strcat(path_aux, command);

		//Veo si el programa existe
		if(access(path_aux, F_OK) != -1 ) {
			//Si existe copio el path en 'path'
			strcpy(path, path_aux);
			//Pongo la bandera de 'found' en 1
			found = 1;
			//Salgo del loop
			break;
		} else {
			//Si no existe no hago nada
		}
	}

	//Si no lo encontre
	if(found) {
		//Imprimo que existe y su ubicación
		printf("File exists in: %s\n", path_aux);
	} else {
		//Imprimo que no existe
		printf("Couldn't find file\n");

	}

}
