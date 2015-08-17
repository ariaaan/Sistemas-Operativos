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

#define BUFFSIZE 256

void showInfo();
void showExtraInfo();

void getHostname();
void getCpuInfo();
void getFechaHora();
void getKernelInfo();
void getUptime();
void getFilesystems();

void getCpuUsageTime();
void getProcessesCreated();
void getContextSwitch();

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
      showExtraInfo();
    }
  }

  return 0;
}

void showInfo() {
  printf("\n");
  printf("Info: \n");
  printf("----- \n\n");

  getHostname();
  getFechaHora();
  getCpuInfo();
  getKernelInfo();
  getUptime();

  printf("\n");
}

void showExtraInfo() {
  printf("Extra Info: \n");
  printf("----------- \n\n");

  getCpuUsageTime();
  getProcessesCreated();
  getContextSwitch();

  printf("\n");
}

void getContextSwitch() {
  //Archivo
  FILE *file;

  //Cantidad procesos
  int context_switch = 0;

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
      sscanf(token, "%d", &context_switch);
      break;
    }
  }

  printf("Context Switches: %d \n", context_switch);

}

void getProcessesCreated() {
  //Archivo
  FILE *file;

  //Cantidad procesos
  int cantidad_procesos = 0;

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
      sscanf(token, "%d", &cantidad_procesos);
      break;
    }
  }

  printf("Processes: %d \n", cantidad_procesos);

}

void getCpuUsageTime() {
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

  int cpu_user_time;
  sscanf(token, "%d", &cpu_user_time);

  //El tercer token no me sirve
  token = strtok(NULL, divider);

  //El cuarto token es el tiempo de cpu en procesos del sistema
  token = strtok(NULL, divider);

  int cpu_system_time;
  sscanf(token, "%d", &cpu_system_time);

  //El quinto token es el tiempo de cpu en reposo
  token = strtok(NULL, divider);

  int cpu_idle_time;
  sscanf(token, "%d", &cpu_idle_time);

  //Imprimo lo leido
  printf("CPU User Time: %d\n", cpu_user_time);
  printf("CPU System Time: %d\n", cpu_system_time);
  printf("CPU Idle Time: %d\n", cpu_idle_time);

  fclose(file);
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

  printf("Time: %s", hora);

  //Leo la segunda linea (fecha)
  fgets(fecha, BUFFSIZE+1, file);

  //Separo la parte que necesito con strtok
  token = strtok(fecha, divider);
  token = strtok(NULL, divider);

  strcpy(fecha, token);
  printf("Date: %s", fecha);
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

  printf("Filesystems Supported: %d\n", cantidad_filesystems);

  fclose(file);
}
