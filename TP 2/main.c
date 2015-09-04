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

#define BUFFERSIZE 256
#define PATHLENGTH 32

/*
* Declaración de funciones
*/

void get_path(char *command);
void parse_exit(char *command);

/*
* Declaración de variables globales
*/

//Array donde voy a guardar las rutas de la variable PATH
char path_array[PATHLENGTH][BUFFERSIZE];

//Contador de cantidad de rutas en PATH
int length = 0;

int main(int argc, char **argv) {

	//Busco el current working directory y lo guardo
	char cwd[1024];
   	if (getcwd(cwd, sizeof(cwd)) != NULL) {
    	fprintf(stdout, "\nCurrent Dir: %s\n\n", cwd);
   	} else {
       	perror("getcwd() error");
   	}


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

	//Variable que indica si hay que terminar el programa
	int exit = 0;

	//Varaible auxiliar que almacena el comando ingresado
	char command[BUFFERSIZE];

	//Loop principal del 'baash'
	while(!exit) {
		printf("arian@baash: ");
   		scanf("%s", command);

   		parse_exit(command);

   		get_path(command);
	}

	return 0;
}


/*
* Definición de funciones
*/

void parse_exit(char *command) {
	if(strcmp(command, "exit") == 0) {
		printf("\n");
		exit(1);
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
