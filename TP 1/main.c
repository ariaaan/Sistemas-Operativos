/*
Escriba una segunda versión del programa que imprima la misma información
que la versión por defecto, pero en caso de invocarse con la opción -s,
agrega la siguiente información:
- Cantidad de tiempo de CPU utilizado para usuarios, sistema y proceso idle.
- Cantidad de cambios de contexto.
- Fecha y hora cuando el sistema fue iniciado.
- Número de procesos creados desde el inicio del sistema.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define BUFFSIZE 256

void showInfo();
void showExtra();
void showMore();

void getHostname(char *hostname, int size);
void getCpuInfo(char *cpu_type, char *cpu_model, int size);
void getDateTime(char *date_time, int size);
void getKernelVersion(char *kernel_version, int size);
void getUptime(char *uptime, int size);
void getFilesystems(char *filesystems, int size);

void getCpuUsageTime(char *cpu_user_time, char *cpu_system_time, char *cpu_idle_time, int size);
void getProcessesCreated(char *processes_created, int size);
void getContextSwitch(char *context_switches, int size);
void getStartupDateTime(char *startup_date_time, int size);

void getDiskInfo(char *disk_read, char *disk_write, int size);
void getMemoryInfo(char *total_memory, char *available_memory, int size);
void getLoadAvg(char *load_avg, int size);

void printLoop(int interval, int duration);
void secondsToTime(char *time_string, int seconds);
int getUptimeSeconds();

void printUsage() {
    printf("KSAMP Usage: ./ksamp [-s] [-l param_1 param_2]\n");
}

time_t t;
struct tm tm;

main(int argc, char *argv[]) {

  int option = 0;
  int interval = 0;
  int duration = 0;

  while ((option = getopt(argc, argv,"sl")) != -1) {
    switch (option) {
       case 's' :
           if(argc == 2) {
              showExtra();
              exit(EXIT_SUCCESS);
           } else {
              printUsage();
              exit(EXIT_FAILURE);
           }
       case 'l' :
           if((optind + 1 > argc - 1) || (argc != 4)) {
              printUsage();
              exit(EXIT_FAILURE);
           } else {
             interval = atoi(argv[optind]);
             duration = atoi(argv[optind+1]);
             printLoop(interval, duration);
             exit(EXIT_SUCCESS);
           }
       default:
           printUsage();
           exit(EXIT_FAILURE);
    }
  }

  showInfo();


  return 0;
}

void printLoop(int interval, int duration) {
    int count = (int) duration/interval;

    int i;
    for(i = 0; i < count; i++) {
      showMore();
      sleep(interval);
    }

    showMore();
}

void secondsToTime(char *time_string, int seconds) {
  //Lo paso a formato hh:mm.ss
  int horas = seconds / 3600;
  seconds = seconds - 3600*horas;
  int minutos = seconds / 60;
  int segundos = seconds - minutos*60;

  char aux[BUFFSIZE+1] = {0};
  char string[BUFFSIZE+1] = {0};

  string[0] = '\0';

  sprintf(aux, "%d", horas);
  if(horas < 10) {
    strcat(string, "0");
    strcat(string, aux);
    strcat(string, ":");
  } else {
    strcat(string, aux);
    strcat(string, ":");
  }

  sprintf(aux, "%d", minutos);
  if(minutos < 10) {
    strcat(string, "0");
    strcat(string, aux);
    strcat(string, ":");
  } else {
    strcat(string, aux);
    strcat(string, ":");
  }

  sprintf(aux, "%d", segundos);
  if(segundos < 10) {
    strcat(string, "0");
    strcat(string, aux);
  } else {
    strcat(string, aux);
  }

  strcpy(time_string, string);
}

void showInfo() {
// New line
  printf("\n");

// Hostname
  int size = 64;
  char *hostname = malloc(size*sizeof(char));
  getHostname(hostname, size);
  hostname[strcspn(hostname, "\n")] = 0;
  printf("Hostname: %s\n", hostname);

// Date - Time
  size = 32;
  char *date_time = malloc(size*sizeof(char));
  getDateTime(date_time, size);
  date_time[strcspn(date_time, "\n")] = 0;
  printf("Date and Time: %s\n", date_time);

  printf("\n");

// CPU Info
  size = 256;
  char *cpu_type = malloc(size*sizeof(char));
  char *cpu_model = malloc(size*sizeof(char));
  getCpuInfo(cpu_type, cpu_model, size);
  cpu_type[strcspn(cpu_type, "\n")] = 0;
  cpu_model[strcspn(cpu_model, "\n")] = 0;
  printf("CPU Type: %s\n", cpu_type);
  printf("CPU Model: %s\n", cpu_model);

// Kernel Version
  size = 32;
  char *kernel_version = malloc(size*sizeof(char));
  getKernelVersion(kernel_version, size - 1);
  kernel_version[strcspn(kernel_version, "\n")] = 0;
  printf("Kernel Version: %s\n", kernel_version);

// Up Time
  size = 16;
  char *uptime = malloc(size*sizeof(char));
  getUptime(uptime, size);
  uptime[strcspn(uptime, "\n")] = 0;
  printf("Uptime: %s\n", uptime);

// File Systems
  size = 8;
  char *filesystems = malloc(size*sizeof(char));
  getFilesystems(filesystems, size);
  filesystems[strcspn(filesystems, "\n")] = 0;
  printf("File Systems: %s\n", filesystems);

// New line
  printf("\n");
}

void showExtra() {
// New line
  printf("\n");

// CPU Usage Time
  int size = 16;
  char *cpu_user_time = malloc(size*sizeof(char));
  char *cpu_system_time = malloc(size*sizeof(char));
  char *cpu_idle_time = malloc(size*sizeof(char));
  getCpuUsageTime(cpu_user_time, cpu_system_time, cpu_idle_time, size);
  cpu_user_time[strcspn(cpu_user_time, "\n")] = 0;
  cpu_system_time[strcspn(cpu_system_time, "\n")] = 0;
  cpu_idle_time[strcspn(cpu_idle_time, "\n")] = 0;
  printf("CPU User Time: %s\n", cpu_user_time);
  printf("CPU System Time: %s\n", cpu_system_time);
  printf("CPU Idle Time: %s\n", cpu_idle_time);

// Processes Created
  size = 16;
  char *processes_created = malloc(size*sizeof(char));
  getProcessesCreated(processes_created, size);
  processes_created[strcspn(processes_created, "\n")] = 0;
  printf("Processes Created: %s\n", processes_created);

// Context Switches
  size = 16;
  char *context_switches = malloc(size*sizeof(char));
  getContextSwitch(context_switches, size);
  context_switches[strcspn(context_switches, "\n")] = 0;
  printf("Context Switches: %s\n", context_switches);

//Startup Date and Time
  size = 32;
  char *startup_date_time = malloc(size*sizeof(char));
  getStartupDateTime(startup_date_time, size);
  startup_date_time[strcspn(startup_date_time, "\n")] = 0;
  printf("Startup Date and Time: %s\n", startup_date_time);

//New Line
  printf("\n");
}

void showMore() {
// New line
  printf("\n");

// Disk Read-Write
  int size = 16;
  char *disk_read = malloc(size*sizeof(char));
  char *disk_write = malloc(size*sizeof(char));
  getDiskInfo(disk_read, disk_write, size);
  disk_read[strcspn(disk_read, "\n")] = 0;
  disk_write[strcspn(disk_write, "\n")] = 0;
  printf("Disk Read: %s\n", disk_read);
  printf("Disk Write: %s\n", disk_write);

// Total / Available Memory
  size = 16;
  char *total_memory = malloc(size*sizeof(char));
  char *available_memory = malloc(size*sizeof(char));
  getMemoryInfo(total_memory, available_memory, size);
  total_memory[strcspn(total_memory, "\n")] = 0;
  available_memory[strcspn(available_memory, "\n")] = 0;
  printf("Total Memory: %s\n", total_memory);
  printf("Available Memory: %s\n", available_memory);

// Load Average
  size = 16;
  char *load_avg = malloc(size*sizeof(char));
  getLoadAvg(load_avg, size);
  load_avg[strcspn(load_avg, "\n")] = 0;
  printf("Load Average: %s\n", load_avg);

  printf("\n");
}

void getLoadAvg(char *load_avg, int size) {
  //Archivo
  FILE *file;

  //Abro el archivo "/proc/loadavg"
  file = fopen("/proc/loadavg","r");

  //Leo el valor
  fscanf(file, "%s", load_avg);

  fclose(file);
}

void getMemoryInfo(char *total_memory, char *available_memory, int size) {
  //Archivo
  FILE *file;

  //Variables auxiliares
  char aux[BUFFSIZE+1] = {0};
  char buffer[BUFFSIZE+1] = {0};
  char copy[BUFFSIZE+1] = {0};
  int memTotal;
  int memAvailable;

  char *divider = ":";
  char *token;

  //Abro el archivo "/proc/meminfo"
  file = fopen("/proc/meminfo","r");

  //Busco la linea que tiene empieza con "MemTotal" y "MemAvailable"
  while(!feof(file)) {
    //Leo la linea
    fgets(aux, BUFFSIZE+1, file);
    strcpy(copy, aux);

    //Busco el tercer token, dividiendo por " "
    token = strtok(aux, divider);

    if(strcmp(token, "MemTotal") == 0) {
        //Total Memory
        sscanf(copy, "%s %d", buffer, &memTotal);
        snprintf(total_memory, size, "%d kB", memTotal);
    } else if(strcmp(token, "MemFree") == 0) {
        //Available Memmory
        sscanf(copy, "%s %d", buffer, &memAvailable);
        snprintf(available_memory, size, "%d kB", memAvailable);
    }
  }

  fclose(file);
}

void getDiskInfo(char *disk_read, char *disk_write, int size) {
  //Archivo
  FILE *file;

  //Variables auxiliares
  char aux[BUFFSIZE+1] = {0};

  char *divider = " ";
  char *token;

  //Abro el archivo "/proc/diskstats"
  file = fopen("/proc/diskstats","r");

  //Busco la linea que tiene empieza con "sda"
  while(!feof(file)) {
    //Leo la linea
    fgets(aux, BUFFSIZE+1, file);

    //Busco el tercer token, dividiendo por " "
    token = strtok(aux, divider);
    token = strtok(NULL, divider);
    token = strtok(NULL, divider);

    if(strcmp(token, "sda") == 0) {
        //Reads Completed
        token = strtok(NULL, divider);
        strncpy(disk_read, token, size);

        //Datos no necesarios
        token = strtok(NULL, divider);
        token = strtok(NULL, divider);
        token = strtok(NULL, divider);

        //Writes Completed
        token = strtok(NULL, divider);
        strncpy(disk_write, token, size);

        break;
      }
    }

    fclose(file);
}

void getDateTime(char *date_time, int size) {
  //Obtengo el tiemmpo actual y lo guardo en las variables globales
  t = time(NULL);
  tm = *localtime(&t);

  //Copio los datos obtenidos en el string "date_time"
  snprintf(date_time, size, "%d-%d-%d %d:%d:%d\n", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);
}

void getStartupDateTime(char *startup_date_time, int size) {
  //Le agrego al tiempo obtenido
  tm.tm_sec -= getUptimeSeconds();

  //Normalizo la fecha
  mktime(&tm);

  //Copio los datos en "startup_date_time"
  snprintf(startup_date_time, size, "%d-%d-%d %d:%d:%d\n", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);
}

int getUptimeSeconds() {
  //Auxiliar
  char aux[BUFFSIZE+1] = {0};

  //Archivo
  FILE *file;

  //Abro el archivo "/proc/uptime"
  file = fopen("/proc/uptime","r");

  //Leo la primera linea y la guardo
  fgets(aux, BUFFSIZE+1, file);

  //Divido en espacios y tomo el primer token
  char *divider = " ";
  char *token = strtok(aux, divider);

  //Convierto el valor en segundos
  int uptime_en_segundos;
  sscanf(aux, "%d", &uptime_en_segundos);

  fclose(file);

  //Devuelvo los segundos
  return uptime_en_segundos;
}

void getContextSwitch(char *context_switches, int size) {
  //Archivo
  FILE *file;

  //Variables auxiliares
  char aux[BUFFSIZE+1] = {0};

  char *divider = " ";
  char *token;

  //Abro el archivo "/proc/sys/kernel/hostname"
  file = fopen("/proc/stat","r");

  //Busco la linea que empieza con "processes"
  while(!feof(file)) {
    //Leo la linea
    fgets(aux, BUFFSIZE+1, file);

    //Busco la primera palabra, dividiendo por " "
    token = strtok(aux, divider);
    if(strcmp(token, "ctxt") == 0) {
        token = strtok(NULL, divider);
        strncpy(context_switches, token, size);
        break;
      }
    }

    fclose(file);
}

void getProcessesCreated(char *processes_created, int size) {
  //Archivo
  FILE *file;

  //Cantidad procesos
  int processes = 0;

  //Variables auxiliares
  char aux[BUFFSIZE+1] = {0};

  char *divider = " ";
  char *token;

  //Abro el archivo "/proc/sys/kernel/hostname"
  file = fopen("/proc/stat","r");

  //Busco la linea que empieza con "processes"
  while(!feof(file)) {
    //Leo la linea
    fgets(aux, BUFFSIZE+1, file);

    //Busco la primera palabra, dividiendo por " "
    token = strtok(aux, divider);
    if(strcmp(token, "processes") == 0) {
      token = strtok(NULL, divider);
      strncpy(processes_created, token, size);
      break;
    }
  }

  fclose(file);
}

void getCpuUsageTime(char *cpu_user_time, char *cpu_system_time, char *cpu_idle_time, int size) {
  //Archivo
  FILE *file;

  //Strings
  char stats[BUFFSIZE+1] = {0};
  char aux[BUFFSIZE+1] = {0};

  //Abro el archivo "/proc/sys/kernel/hostname"
  file = fopen("/proc/stat","r");

  //Leo la primera linea
  fgets(stats, BUFFSIZE+1, file);

  //Divido en " ""
  char *divider = " ";
  char *token;

  //El primero no me sirve
  token = strtok(stats, divider);

  //El segundo token es el tiempo de cpu en procesos de usuario
  int cpu1;
  token = strtok(NULL, divider);
  sscanf(token, "%d", &cpu1);
  cpu1 = cpu1/100;
  secondsToTime(aux, cpu1);

  strncpy(cpu_user_time, aux, size);

  //El tercer token no me sirve
  token = strtok(NULL, divider);

  //El cuarto token es el tiempo de cpu en procesos del sistema
  int cpu2;
  token = strtok(NULL, divider);
  sscanf(token, "%d", &cpu2);
  cpu2 = cpu2/100;
  secondsToTime(aux, cpu2);

  strncpy(cpu_system_time, aux, size);

  //El quinto token es el tiempo de cpu en reposo
  int cpu3;
  token = strtok(NULL, divider);
  sscanf(token, "%d", &cpu3);
  cpu3 = cpu3/100;
  secondsToTime(aux, cpu3);

  strncpy(cpu_idle_time, aux, size);

  fclose(file);
}

void getHostname(char *hostname, int size) {
  //Archivo
  FILE *file;

  //Abro el archivo "/proc/sys/kernel/hostname"
  file = fopen("/proc/sys/kernel/hostname","r");

  //Leo la primera linea
  fgets(hostname, size, file);

  fclose(file);
}

void getCpuInfo(char *cpu_type, char *cpu_model, int size) {
  //Archivo
  FILE *file;

  //String auxiliar
  char aux[BUFFSIZE + 1] = {0};
  cpu_type[0] = '\0';
  cpu_model[0] = '\0';

  //Abro el archivo "/proc/cpuinfo"
  file = fopen("/proc/cpuinfo","r");

  //Divido en " ""
  char *divider = ":";
  char *token;

  //Busco las lineas que empieza con "vendor_id" y "model name"
  while(!feof(file)) {
    //Leo la linea
    fgets(aux, BUFFSIZE+1, file);

    //Busco la primera palabra, dividiendo por " "
    token = strtok(aux, divider);

    if(strcmp(token, "vendor_id	") == 0) {
      token = strtok(NULL, divider);
      strncpy(cpu_type, token, size);
    } else if(strcmp(token, "model name	") == 0) {
      token = strtok(NULL, divider);
      strncpy(cpu_model, token, size);
    }
  }

  fclose(file);
}

void getKernelVersion(char *kernel_version, int size) {
  //Archivo
  FILE *file;

  //String auxiliar
  char aux_1[BUFFSIZE+1] = {0};
  char aux_2[BUFFSIZE+1] = {0};
  char aux_3[BUFFSIZE+1] = {0};

  //Abro el archivo "/proc/version"
  file = fopen("/proc/version","r");


  fscanf(file, "%s %s %s", aux_1, aux_2, aux_3);

  //Corto una parte del string obtenido
  strncpy(kernel_version, aux_3, size);

  fclose(file);
}

void getUptime(char *uptime, int size) {
  //Obtengo el uptime en segundos
  int uptime_en_segundos = getUptimeSeconds();
  char aux[BUFFSIZE+1] = {0};

  secondsToTime(aux, uptime_en_segundos);
  strncpy(uptime, aux, size);
}

void getFilesystems(char *filesystems, int size) {
  //Archivo
  FILE *file;

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
  snprintf(filesystems, size, "%d", cantidad_filesystems);

  fclose(file);
}
