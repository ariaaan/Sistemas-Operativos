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
#include <time.h>

#define BUFFSIZE 256

void showInfo();
void showExtra();

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

int getUptimeSeconds();

time_t t;
struct tm tm;

main(int argc, char *argv[]) {
  //Posibles parámetros
  const char arg_extra_info[] = "-s";

  //Informacion que se muestra siempre
  showInfo();

  //Comparo los parametros que pasaron con los posibles parametros
  int i = 0;

  for (i = 0; i < argc; i++) {
    if(strcmp(arg_extra_info, argv[i]) == 0) {
      //Si se paso "-s" muestro información extra
      showExtra();
    }
  }

  return 0;
}

void showInfo() {
// Title
  printf("\n");
  printf("Info: \n");
  printf("----- \n\n");

// Hostname
  int size = 64;
  char *hostname = malloc(size*sizeof(char));
  getHostname(hostname, size);
  hostname[strcspn(hostname, "\n")] = 0;
  printf("Hostname: %s\n", hostname);

// Date - Time
  size = 16;
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
  getKernelVersion(kernel_version, size);
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
// Title
  printf("Extra: \n");
  printf("------ \n\n");

// CPU Usage Time
  int size = 16;
  char *cpu_user_time = malloc(size*sizeof(char));
  char *cpu_system_time = malloc(size*sizeof(char));
  char *cpu_idle_time = malloc(size*sizeof(char));
  getCpuUsageTime(cpu_user_time, cpu_system_time, cpu_idle_time, size);
  cpu_user_time[strcspn(cpu_user_time, "\n")] = 0;
  cpu_system_time[strcspn(cpu_system_time, "\n")] = 0;
  cpu_idle_time[strcspn(cpu_idle_time, "\n")] = 0;
  printf("CPU User Time: %s [USER_HZ]\n", cpu_user_time);
  printf("CPU System Time: %s [USER_HZ]\n", cpu_system_time);
  printf("CPU Idle Time: %s [USER_HZ]\n", cpu_idle_time);

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


void getDateTime(char *date_time, int size) {
/*
  //Variables
  time_t rawtime;
  struct tm *timeinfo;

  //Obtengo el tiempo actual
  time (&rawtime);
  timeinfo = localtime (&rawtime);
*/
  t = time(NULL);
  tm = *localtime(&t);

  sprintf(date_time, "%d-%d-%d %d:%d:%d\n", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);

  //Copio el string con la fecha/hora en date_time
  //strcpy(date_time, asctime(localtime(&result)));
}

void getStartupDateTime(char *startup_date_time, int size) {
  tm.tm_sec -= getUptimeSeconds();
  mktime(&tm);
  sprintf(startup_date_time, "%d-%d-%d %d:%d:%d\n", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);

  //Copio el string con la fecha/hora en date_time
  //strcpy(startup_date_time, asctime(localtime(&result)));
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
        strcpy(context_switches, token);
        break;
      }
    }
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
      strcpy(processes_created, token);
      break;
    }
  }
}

void getCpuUsageTime(char *cpu_user_time, char *cpu_system_time, char *cpu_idle_time, int size) {
  //Archivo
  FILE *file;

  //Strings
  char stats[BUFFSIZE+1] = {0};

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
  token = strtok(NULL, divider);
  strcpy(cpu_user_time, token);

  //El tercer token no me sirve
  token = strtok(NULL, divider);

  //El cuarto token es el tiempo de cpu en procesos del sistema
  token = strtok(NULL, divider);
  strcpy(cpu_system_time, token);

  //El quinto token es el tiempo de cpu en reposo
  token = strtok(NULL, divider);
  strcpy(cpu_idle_time, token);

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
      strcpy(cpu_type, token);
    } else if(strcmp(token, "model name	") == 0) {
      token = strtok(NULL, divider);
      strcpy(cpu_model, token);
    }
  }
}

void getKernelVersion(char *kernel_version, int size) {
  //Archivo
  FILE *file;

  //String auxiliar
  char aux[BUFFSIZE+1] = {0};

  //Abro el archivo "/proc/version"
  file = fopen("/proc/version","r");

  //Leo la primera linea y la guardo
  fgets(aux, BUFFSIZE+1, file);

  //Corto una parte del string obtenido
  strncpy(kernel_version, aux, 32);

  fclose(file);
}

void getUptime(char *uptime, int size) {
  //Obtengo el uptime en segundos
  int uptime_en_segundos = getUptimeSeconds();

  //Lo paso a formato hh:mm.ss
  int horas = uptime_en_segundos / 3600;
  uptime_en_segundos = uptime_en_segundos - 3600*horas;
  int minutos = uptime_en_segundos / 60;
  int segundos = uptime_en_segundos - minutos*60;

  char aux[BUFFSIZE+1] = {0};
  uptime[0] = '\0';

  sprintf(aux, "%d", horas);
  if(horas < 10) {
    strcat(uptime, "0");
    strcat(uptime, aux);
    strcat(uptime, ":");
  } else {
    strcat(uptime, aux);
    strcat(uptime, ":");
  }

  sprintf(aux, "%d", minutos);
  if(minutos < 10) {
    strcat(uptime, "0");
    strcat(uptime, aux);
    strcat(uptime, ":");
  } else {
    strcat(uptime, aux);
    strcat(uptime, ":");
  }

  sprintf(aux, "%d", segundos);
  if(segundos < 10) {
    strcat(uptime, "0");
    strcat(uptime, aux);
  } else {
    strcat(uptime, aux);
  }
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
  sprintf(filesystems, "%d", cantidad_filesystems);

  fclose(file);
}
