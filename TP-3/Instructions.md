# Pasos para usar un Char Device Driver

### Compilar módulo
Para compilar el módulo hacemos un `make`.

### Insertar módulo
Para insertar un módulo (en este caso el módulo `chardev.ko`) tenemos que hacer:
```
sudo insmod chardev.ko
```

### Crear un device en `/dev`
Luego, para crear un char device y que aparezca en `/dev`, hacemos:

```
sudo mknod -m  666 /dev/dev_encryptor c 256 0
```

Donde vemos que se va a llamar `dev_encryptor` y va a ser responsable del Major number 256, y el minor number 0.

### Borrar un device de `/dev`
Para borrar simplemente hacemos:

```
sudo rm /dev/dev_encryptor
```

### Leer de un char device
Para leer de un char devices hacemos por ejemplo:

```
cat /dev/dev_encryptor
```

### Escribir en un char device
Para escribir en un char devices hacemos por ejemplo:

```
echo 123 > /dev/dev_encryptor
```

### Ver los mensajes de los `printk()`
Para ver los mensajes de los printk, podemos hacer:

```
sudo tail -f  /var/log/syslog
```


O simplemente:

```
 dmesg
```

	