#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* Para alinear punteros, todos muliplos de 32 bits */
#define align4(x) (((((x)-1)>>2)<<2)+4)

/* Tamaño de la estrcutura que representa un bloque */
#define BLOCK_SIZE sizeof(struct mem_block)

/* Definicion de tipo puntero a mem_block */
typedef struct mem_block* block;

/* Estructura que representa un bloque del heap */
struct mem_block {
	size_t size;
	block next;
	block prev;
	int is_free;
};

/* Funciones */
void *malloc(size_t size);
void print_memory_list();
block find_block(size_t size);

/* Inicio de la lista */
void *memory_list_head = NULL;
/* Fin de la lista */
void *memory_list_tail = NULL;

int main(int argc, char *argv[]){

	malloc(10);
	malloc(13);
	malloc(13);
	malloc(10);
	malloc(10);
	malloc(13);
	malloc(13);
	malloc(10);
	malloc(10);
	malloc(13);
	malloc(13);
	malloc(10);

	print_memory_list();

	// block my_block = memory_list_head;
	// printf("Size of head block: %zu\n", my_block->size);

	return 0;
}

void *malloc(size_t size){
	/* Primero encuentro el valor mas cercano multiplo de 4 bytes (32 bits) para alocar */
	size_t aligned_size = align4(size);

	/* Me fijo si es la primera vez que se ejecuta malloc() */
	if(memory_list_head == NULL) {
		/* Creo un nuevo bloque */
		block new_block;

		/* Obtengo la posicion del break y pongo el nuevo bloque ahi */
		new_block = sbrk(0);

		/* Intento mover el break en el size alineado, si falla retorno NULL */
		if (sbrk( BLOCK_SIZE + size) == (void*) -1) {
			printf("Error\n");
			return (NULL);
		}

		/* Si pude mover el break, anoto los datos en la estructura */
		new_block->size = aligned_size;
		new_block->is_free = 0;
		new_block->next = NULL;
		new_block->prev = NULL;

		/* Pongo al nuevo bloque como la cabeza de la lista */
		memory_list_head = new_block;
		memory_list_tail = new_block;
	} else {
		/* Si ya existe una lista */

		/* Veo si hay un espacio libre en la lista en la que entre */
		block my_block = find_block(size);

		/* Si hay, lo ocupo (Todo el tamaño por ahora) */
		if(my_block != NULL) {
			my_block->size = aligned_size;
			my_block->is_free = 0;
		} else {
			/* Creo un nuevo bloque al final */
			my_block = sbrk(aligned_size);
			my_block->size = aligned_size;
			my_block->is_free = 0;
			my_block->prev = memory_list_tail;
			my_block->next = NULL;

			my_block->prev->next = my_block;

			memory_list_tail = my_block;
		}
	}
}

block find_block(size_t size) {
	// block b = memory_list_head;
	// /* Recorro la lista */
	// while(b && !((b->is_free) && (b->size >= size))) {
	// 	b = b->next;
	// }
	//
	// return b;
	return NULL;
}

void print_memory_list() {
	block b = memory_list_head;

	/* Si la lista esta vacia no hago nada */
	if(b == NULL) return;

	/* Si no esta vacia, la recorro e imprimo los datos */
	int index = 0;

	printf("+----------+----------+----------+\n");
	printf("| %8s | %8s | %8s |\n", "Block Nº", "Size", "Is Free?");
	printf("+----------+----------+----------+\n");

	do {
		printf("| %8d | %8zu | %8d |\n", index, b->size,  b->is_free);
		index++;
		b = b->next;
	} while(b != NULL);

	printf("+----------+----------+----------+\n");

}
