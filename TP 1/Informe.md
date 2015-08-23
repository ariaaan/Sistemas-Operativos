# Sistemas Operativos I
### Trabajo Práctico I

>**Alumno:** Giles García, Arian  
**Profesor:** Ing. Pablo Martínez

## Introducción
El trabajo consiste en realizar un pequeño programa escrito en lenguaje 'C', que permita al usuario conocer cierta información acerca de su PC, asi como datos del CPU, memoria, kernel, etc. Para ello, se utilizaran los diferentes archivos que se encuentran dentro del directorio `/proc`. Debemos buscar la información pedida, parsear los datos necesarios y imprimir por el stdout de la terminal de Linux.

## Desarrollo
#### Parte A

La primera parte de este trabajo consiste en buscar donde se encuentra la información que vamos a necesitar para nuestro programa.

##### Tipo y Modelo de CPU  
> /proc/cpuinfo

El archivo `cpuinfo`   nos ofrece datos sobre el CPU de nuestra PC, tales como el nombre, el modelo, el tipo, la cantidad de núcleos, la frecuencia a la que trabajo y más. En nuestro caso solo necesitamos el tipo y el modelo. El tipo lo podemos encontrar en la linea  `vendor_id`, mientras que el modelo lo encontramos en la linea `model name`.

##### Versión del Kernel
> /proc/version

El archivo `version` del sistema de archivos `/proc` contiene información, entre otras cosas, sobre el kernel. Por lo tanto aquí podemos encontrar la versión del kernel necesaria para mostrar en nustro programa.

##### Tiempo desde el inicio del sistema operativo
> /proc/uptime

Este archivo contiene dos valores numéricos. El primero es el que necesitamos, contiene el numero de segundos que transcurrió desde el incio del sistema operativo. Para que sea legible, lo transformamos en formato `hh:mm:ss`.

##### Uso del CPU
> /proc/stat

De este archivo, podemos conseguir los diferentes tiempos del CPU. Necesitamos el tiempo utilizado en procesos del usuario, el tiempo utilizado en procesos del sistema, y el tiempo en reposo. Estos valores se encuentran en este archivo pero representados en **USER_HZ**, que representan una centésima de segundo (1/100 segundos). Por lo tanto, necesitaremos realizar una transformación para pasarlo a formato `hh:mm:ss`.

##### Memoria total y disponible
> /proc/meminfo

El archivo `meminfo` nos ofrece información sobre la memoria RAM de nuestra PC. La información que necesitamos es la cantidad de memoria RAM libre, y total. Esto lo podemos encontrar en las lineas `MemFree` y `MemTotal`.

##### Cantidad de sistemas de archivos soportados
> /proc/filesystems

En este archivo se encuentran listados linea por linea, los diferentes sistemas de archivos soportados por el sistema operativo. Por lo tanto, contando la cantida de líneas, obtenemos la cantidad.

##### Pedidos de lectura y escritura al disco
> /proc/diskstats

En este archivo se encuentra diferente tipo de información sobre el disco, las que nos interesan a nostros son la linea 4 y la linea 8, que representan los pedidos de lectura y escritura respectivamente.

##### Cambios de contexto
> /proc/stat

La linea `ctxt` del archivo `stat` nos ofrece la cantidad de cambios de contexto que hacen sucedido.

##### Cantidad de procesos creados
> /proc/stat

En el mismo archivo `stat`, pero en la linea `processes` podemos obtener la cantidad de procesos que han sido creados hasta el momento.

#### Parte B

Se debe crear una primera versión del programa que nos muestre por pantalla:

- Tipo y modelo de CPU. 
- Versión de Kernel.
- Cantidad de tiempo transcurrido desde que se inició el sistema operativo, con el formato hh:mm:ss. 
- Cantidad de sistemas de archivo soportados por el kernel.

Además, se nos pide que incluyamos una cabecera con el nombre de la máquina y la fecha y hora actuales.

El nombre de la máquina lo sacamos del archivo `/proc/sys/kernel/hostname`, mientras que la fecha y hora acutal, la obtenemos utilizando funciones de la libreria `<time.h>`.  
El resto de los datos son obtenidos leyendo los archivos anteriormente mencionados y parseandolos para obtener solo la información necesaria.

#### Parte C

En esta parte, tenemos que aceptar un parámetro `-s`. Si se llama al programa con este parámetro, debemos mostrar la siguiente información:

- Cantidad de tiempo de CPU utilizado para usuarios, sistema y proceso idle. 
- Cantidad de cambios de contexto. 
- Fecha y hora cuando el sistema fue iniciado. 
- Número de procesos creados desde el inicio del sistema.

Toda esta información se encuentra en los archivos mencionados en la parte A, por lo tanto solo tenemos que leer dichos archivos y parsearlos para obtener la información deseada, e imprimir la información obtenida por pantalla. La fecha y hora de cuando el sistema fue iniciado, fue obtenida leyendo el tiempo que el sistema estuvo encendido y restando dicho valor a la fecha actual.

Para controlar las opciones ingresadas, se utiliza la funcion `getopt()`, que nos facilita la manera de parsear las opciones y los argumentos con los que se llama al programa.

#### Parte D
En esta parte, si se invoca al programa con la opción `-l param_1 param_2`, debemos imprimir los siguientes datos durante `param_2` segundos, cada `param_1` segundos:

- Número de peticiones a disco realizadas. 
- Cantidad de memoria configurada en el hardware. 
- Cantidad de memoria disponible. 
- Lista de los promedios de carga de 1 minuto. 

Para esto, agregamos a la función `getopt()` la opción `-l` y luego, si se llamo al programa con esa opción, obtenemos los argumentos necesarios, de intervalo (`param_1`) y duración (`param_2`).

Por lo tanto, si llamamos al programa de la siguiente manera:

```
./ksamp -l 5 40
```

Se buscará e imprimirá la información mencionada anteriormente cada 5 segundos, durante 40 segundos, y luego el programa terminará.

Además, agregamos control de errores para los casos que se ingresen opciones que no son aceptadas, le muestre al usuario la forma de uso de programa.

## Conclusión
Luego de la realización de este trabajo, pudimos empezar a comprender como funciona un sistema operativo. Los datos que obtuvimos son solo una pequeña parte de toda la información que nos proveía el sistema de archivos `/proc`.  
Resulto útil la utilización de parámetros para mostar solo la información que se quiere ver, y no mostrar toda la información, todo el tiempo, ya que se pierde mucho tiempo hasta que se encuentra la información que queremos ver.
