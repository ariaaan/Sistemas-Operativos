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
void free(void *p);

block find_block(size_t size);
block extend_heap(size_t size);
void split_block(size_t size);

void print_memory_list();

/* Inicio de la lista */
void *memory_list_head = NULL;
/* Fin de la lista */
void *memory_list_tail = NULL;

int main(int argc, char *argv[]){

	malloc(45);
	malloc(16);
	malloc(146);
	malloc(79);
	malloc(255);
	malloc(25);

	print_memory_list();

	return 0;
}

void *malloc(size_t size){
	/* Primero encuentro el valor mas cercano multiplo de 4 bytes (32 bits) para alocar */
	size_t aligned_size = align4(size);
	block new_block, last_block;

	/* Me fijo si es la primera vez que se ejecuta malloc() */
	if(memory_list_head == NULL) {
		/* Llamo a extend_heap */
		new_block = extend_heap(aligned_size);

		/* Si no se pudo extender el heap retorno NULL */
		if(!new_block) return NULL;

		/* Si todo salio bien, pongo al nuevo bloque como la cabeza y cola de la lista */
		memory_list_head = new_block;
		memory_list_tail = new_block;
	} else {
		/* Si ya existe una lista */

		/* Veo si hay un espacio libre en la lista en la que entre */
		block my_block = NULL;
		my_block = find_block(size);

		if(my_block != NULL) {
			/* To be coded :P */
			printf("Se encontro un espacio libre, falta agregar codigo aca.");
		} else {
			my_block = extend_heap(aligned_size);
			if(my_block == NULL) return NULL;

			memory_list_tail = my_block;
		}
	}
}

void free(void *p) {

}

block extend_heap(size_t size) {
	block new_block = NULL;
	new_block = sbrk(0);

	if(sbrk(BLOCK_SIZE + size) == (void*)-1) return NULL;

	new_block->size = size;
	new_block->next = NULL;
	new_block->prev = memory_list_tail;
	new_block->is_free = 0;

	if(memory_list_tail != NULL) {
		block last = memory_list_tail;
		last->next = new_block;
	}

	return new_block;
}

block find_block(size_t size) {
	block my_block = memory_list_head;

	while(my_block != NULL) {
		if((my_block->size > size) && (my_block->is_free)) return my_block;

		my_block = my_block->next;
	}

	return NULL;
}

void print_memory_list() {
	block b = memory_list_head;

	/* Si la lista esta vacia no hago nada */
	if(b == NULL) return;

	/* Si no esta vacia, la recorro e imprimo los datos */
	int index = 0;

	printf("+----------+----------+----------+------------+------------+\n");
	printf("| %8s | %8s | %8s | %10s | %10s |\n", "Block Nº", "Size", "Is Free?", "Previous", "Next");
	printf("+----------+----------+----------+------------+------------+\n");

	do {
		printf("| %8d | %8zu | %8s | %10p | %10p |\n", index, b->size,  (b->is_free) ? "Yes": "No", b->prev, b->next);
		index++;
		b = b->next;
	} while(b != NULL);

	printf("+----------+----------+----------+------------+------------+\n");

}
