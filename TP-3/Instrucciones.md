# Pasos para usar un Char Device Driver

### Compilar módulo
Para compilar el módulo hacemos un `make`.

### Insertar módulo
Para insertar un módulo (en este caso el módulo `encrypter.ko`) tenemos que hacer:
```
sudo insmod encrypter.ko
```

### Crear un device en `/dev`
Luego, para crear un char device y que aparezca en `/dev`, hacemos:

```
sudo mknod -m  666 /dev/encrypter c 256 0
```

Donde vemos que se va a llamar `encrypter` y va a ser responsable del Major number 256, y el minor number 0.

### Borrar un device de `/dev`
Para borrar simplemente hacemos:

```
sudo rm /dev/encrypter
```

### Leer de un char device
Para leer de un char devices hacemos por ejemplo:

```
cat /dev/encrypter
```

### Escribir en un char device
Para escribir en un char devices hacemos por ejemplo:

```
echo 123 > /dev/encrypter
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

	
