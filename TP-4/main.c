#include <stdio.h>
#include <unistd.h>

/* Definición de macros */
#define align4 (x) ((((( x) -1) > >2) < <2)+4)

/* Definición de tipo */
typedef struct struct_block* block;

/* Estructura de bloque de memoria */
struct struct_block {
	size_t size;	/* Tamaño del bloque */
	int free;		/* Flag de libre/ocupado */
	block prev;		/* Puntero al bloque anterior */
	block next;		/* Puntero al bloque siguiente */
};

/* Declaración de funciones */
void* malloc(size_t size);
void free(void* ptr);

/* Main para testeo */
int main(int argc, char *argv[]){



	return 0;
}

/* malloc() */
void* malloc(size_t size) {
	return NULL;
}

/* free() */
void free(void* ptr) {

}

/* find_block() */
block find_block(block *last, size_t size){

}