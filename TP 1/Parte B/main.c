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
#include <string.h>
#define BUFFSIZE 256

void getHostname();
void getCpuInfo();
void getFechaHora();
void getKernelInfo();
void getUptime();
void getFilesystems();

main(int argc, char *argv[]) {

  getHostname();
  getFechaHora();
  getCpuInfo();
  getKernelInfo();
  getUptime();

  return 0;
}

void getHostname() {
  //Archivo
  FILE *file;

  //Strings
  char hostname[BUFFSIZE+1] = {0};

  //Abro el archivo "/proc/sys/kernel/hostname"
  file = fopen("/proc/sys/kernel/hostname","r");

  //Leo la primera linea
  fgets(hostname, BUFFSIZE+1, file);

  printf("Hostname: %s", hostname);

  fclose(file);
}

void getFechaHora(){
  //Archivo
  FILE *file;

  //String auxiliar
  char aux[BUFFSIZE+1] = {0};

  //Fecha y hora
  char fecha[BUFFSIZE+1] = {0};
  char hora[BUFFSIZE+1] = {0};

  //Abro el archivo "/proc/driver/rtc"
  file = fopen("/proc/driver/rtc","r");

  //Leo la primera linea (hora)
  fgets(aux, BUFFSIZE+1, file);

  //Divido en ":" y luego vuelvo a unir la hora
  char *divider = ":";

  char *token = strtok(aux, divider);
  token = strtok(NULL, divider);
  strcpy(hora, token);
  strcat(hora, ":");

  token = strtok(NULL, divider);
  strcat(hora, token);
  strcat(hora, ":");

  token = strtok(NULL, divider);
  strcat(hora, token);

  printf("Hora: %s", hora);

  //Leo la segunda linea (fecha)
  fgets(fecha, BUFFSIZE+1, file);

  //Separo la parte que necesito con strtok
  token = strtok(fecha, divider);
  token = strtok(NULL, divider);

  strcpy(fecha, token);
  printf("Fecha: %s", fecha);
}

void getCpuInfo(){
  //Archivo
  FILE *file;

  //String auxiliar
  char aux[BUFFSIZE+1] = {0};

  //Strings
  char cpu_type[BUFFSIZE+1] = {0};
  char cpu_model[BUFFSIZE+1] = {0};

  //Abro el archivo "/proc/cpuinfo"
  file = fopen("/proc/cpuinfo","r");

  //La primera linea no me sirve
  fgets(aux, BUFFSIZE+1, file);

  //Leo la linea de "Type" y lo pongo en cpu_type
  fgets(cpu_type, BUFFSIZE+1, file);

  //Tercera y cuarta linea no me sirven
  fgets(aux, BUFFSIZE+1, file);
  fgets(aux, BUFFSIZE+1, file);

  //Leo la linea de "Model" y lo pongo en cpu_model
  fgets(cpu_model, BUFFSIZE+1, file);

  //Cierro el archivo
  fclose(file);

  //Con "strtok" divido en ":" y obtengo solo lo que necesito
  char *divider = ":";

  char *token = strtok(cpu_type, divider);
  token = strtok(NULL, divider);

  strcpy(cpu_type, token);
  printf("CPU Type: %s", cpu_type);

  token = strtok(cpu_model, divider);
  token = strtok(NULL, divider);

  strcpy(cpu_model, token);
  printf("CPU Model: %s", cpu_model);
}

void getKernelInfo() {
  //Archivo
  FILE *file;

  //String auxiliar
  char aux[BUFFSIZE+1] = {0};

  //Strings
  char kernel_version[BUFFSIZE+1] = {0};

  //Abro el archivo "/proc/version"
  file = fopen("/proc/version","r");

  //Leo la primera linea y la guardo
  fgets(aux, BUFFSIZE+1, file);

  //Corto una parte del string obtenido
  strncpy(kernel_version, aux, 32);

  printf("Kernel Version: %s\n", kernel_version);

  fclose(file);
}

void getUptime() {
  //Archivo
  FILE *file;

  //Strings
  char uptime[BUFFSIZE+1] = {0};

  //Abro el archivo "/proc/uptime"
  file = fopen("/proc/uptime","r");

  //Leo la primera linea y la guardo
  fgets(uptime, BUFFSIZE+1, file);

  //Divido en espacios y tomo el primer token
  char *divider = " ";
  char *token = strtok(uptime, divider);

  //Convierto el valor en segundos
  int uptime_en_segundos;
  sscanf(uptime, "%d", &uptime_en_segundos);

  //Lo paso a formato hh:mm.ss
  int horas = uptime_en_segundos / 3600;
  uptime_en_segundos = uptime_en_segundos - 3600*horas;
  int minutos = uptime_en_segundos / 60;
  int segundos = uptime_en_segundos - minutos*60;

  printf("Uptime: %d:%d:%d\n", horas, minutos, segundos);

  fclose(file);
}

void getFilesystems() {
  //Archivo
  FILE *file;

  //Strings
  char filesystems[BUFFSIZE+1] = {0};

  //String auxiliar
  char aux[BUFFSIZE+1] = {0};

  //Abro el archivo "/proc/filesystems"
  file = fopen("/proc/filesystems","r");

  //Cuento la cantidad de lineas
  int cantidad_filesystems = 0;
  while(!feof(file)) {
    fgets(aux, BUFFSIZE+1, file);
    cantidad_filesystems++;
  }

  cantidad_filesystems = cantidad_filesystems - 1;

  printf("Candidad Filesystems: %d\n", cantidad_filesystems);

  fclose(file);
}
