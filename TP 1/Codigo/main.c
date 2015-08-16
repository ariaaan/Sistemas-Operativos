/*
Escriba una versión inicial del ksamp programa que inspeccione
las variables del kernel a través del /proc e informe por stdout:
 - Tipo y modelo de CPU.
 - Versión de Kernel.
- Cantidad de tiempo transcurrido desde que se inició
  el sistema operativo, con el formato ddD hh:mm:ss.
- Cantidad de sistemas de archivo soportados por el kernel
- También se pide incluir una cabecera donde se indique el
  nombre de la máquina y la fecha y hora actuales.
*/

#include <stdio.h>
#define BUFFSIZE 256

main(int argc, char *argv[]) {

  //Archivo
  FILE *file;

  //Strings
  char cpu_type[BUFFSIZE+1];
  char cpu_model[BUFFSIZE+1];

  char aux[BUFFSIZE+1];

  //Abro el archivo "cpuinfo"
  file = fopen("/proc/cpuinfo","r");

  //La primera linea no me sirve
  fgets(aux, BUFFSIZE+1, file);

  //Leo el "Type" y lo pongo en cpu_type
  fgets(cpu_type, BUFFSIZE+1, file);

  //Tercera y cuarta linea no me sirven
  fgets(aux, BUFFSIZE+1, file);
  fgets(aux, BUFFSIZE+1, file);

  //Leo el "Model" y lo pongo en cpu_model
  fgets(cpu_model, BUFFSIZE+1, file);

  //Cierro el archivo
  fclose(file);

  //Imprimo lo leido
  printf("\n");
  printf("INFO:\n");
  printf("-----\n\n");

  printf("%s \n", cpu_type);
  printf("%s \n", cpu_model);

}
