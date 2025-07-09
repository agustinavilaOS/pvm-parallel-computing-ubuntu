---

# Manual de Instalación y Ejecución de PVM en Ubuntu 18.04

Este manual proporciona una guía paso a paso para instalar y configurar PVM (Parallel Virtual Machine) en un sistema Ubuntu 18.04. Además, se incluye un ejemplo de código que utiliza PVM para ejecutar tareas paralelas.

---

## 1. Instalación de PVM en Ubuntu 18.04

### 1.1. Instalación de Paquetes Necesarios

Para comenzar, abre una terminal en tu sistema Ubuntu y ejecuta los siguientes comandos para instalar los paquetes necesarios:

```bash
sudo apt install pvm pvm-dev
```

* `pvm`: Instala la biblioteca y las herramientas necesarias para usar PVM.
* `pvm-dev`: Instala las bibliotecas de desarrollo de PVM.

A continuación, instala otros paquetes que pueden ser útiles:

```bash
sudo apt install build-essential -y
```

* `build-essential`: Instala herramientas básicas para la compilación de programas en C, como gcc, make, etc.

### 1.2. Creación de Directorios

Crea los directorios donde se almacenarán los archivos de PVM y el proyecto. Ejecuta los siguientes comandos:

```bash
mkdir -p $HOME/pvm3/bin/LINUX
mkdir -p $HOME/proyecto_pvm
```

* `$HOME/pvm3/bin/LINUX`: Directorio donde se almacenarán los ejecutables de PVM.
* `$HOME/proyecto_pvm`: Directorio donde almacenarás el código fuente del proyecto.

### 1.3. Navegar al Directorio del Proyecto

Accede al directorio del proyecto que acabas de crear:

```bash
cd $HOME/proyecto_pvm
```

---

## 2. Creación de Archivos de Código

En este paso, crearás los archivos de código necesarios para el programa PVM.

### 2.1. Crear el Archivo `master.c`

Abre un editor de texto para crear el archivo master.c:

```bash
nano master.c
```

**Código para master.c**
Este archivo contiene el código para el proceso maestro que lanza el proceso esclavo y recibe los resultados.

> **Importante:** Asegúrate de reemplazar `/home/myuser/pvm3/bin/LINUX` por la salida del siguiente comando:
>
> ```bash
> echo $HOME/pvm3/bin/LINUX
> ```

**Código para master.c:**

```c
#include <stdio.h>
#include <stdlib.h>
#include <pvm3.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s inicio fin\n", argv[0]);
        return 1;
    }

    int inicio = atoi(argv[1]);
    int fin = atoi(argv[2]);
    int slave_tid;
    int tids[1];
    int spawned;

    printf("Maestro: lanzando proceso esclavo...\n");

    spawned = pvm_spawn("/home/myuser/pvm3/bin/LINUX/slave", NULL, PvmTaskDefault, "", 1, tids);

    if (spawned < 1) {
        fprintf(stderr, "Error: no se pudo lanzar el esclavo.\n");
        pvm_exit();
        return 1;
    }

    slave_tid = tids[0];
    printf("Maestro: esclavo lanzado con TID %d\n", slave_tid);

    // Enviar el rango al esclavo
    pvm_initsend(PvmDataDefault);
    pvm_pkint(&inicio, 1, 1);
    pvm_pkint(&fin, 1, 1);
    pvm_send(slave_tid, 1);
    printf("Maestro: rango enviado [%d, %d]\n", inicio, fin);

    // Esperar y recibir el resultado
    int suma = 0;
    pvm_recv(slave_tid, 2);
    pvm_upkint(&suma, 1, 1);
    printf("Maestro: suma recibida = %d\n", suma);

    pvm_exit();
    return 0;
}
```

### 2.2. Crear el Archivo `slave.c`

Ahora, crea el archivo slave.c para el proceso esclavo:

```bash
nano slave.c
```

**Código para slave.c**
Este archivo contiene el código para el proceso esclavo, que recibe los parámetros del maestro, realiza un cálculo (en este caso, la suma de los números entre inicio y fin), y envía el resultado de vuelta al proceso maestro.

**Código para slave.c:**

```c
#include <stdio.h>
#include <stdlib.h>
#include <pvm3.h>

int main() {
    int bufid, bytes, msgtag, tid;
    int inicio, fin;

    // Espera un mensaje desde cualquier tarea con etiqueta 1
    bufid = pvm_recv(-1, 1);
    pvm_bufinfo(bufid, &bytes, &msgtag, &tid);

    // Desempaquetar datos
    pvm_upkint(&inicio, 1, 1);
    pvm_upkint(&fin, 1, 1);

    // Calcular la suma en el rango
    int suma = 0;
    for (int i = inicio; i <= fin; i++) {
        suma += i;
    }

    // Preparar y enviar respuesta al padre
    pvm_initsend(PvmDataDefault);
    pvm_pkint(&suma, 1, 1);
    pvm_send(pvm_parent(), 2);

    pvm_exit();
    return 0;
}
```

### 2.3. Crear el Archivo `Makefile`

A continuación, crea un archivo Makefile para facilitar la compilación de los programas. Abre el editor de texto y crea el archivo:

```bash
nano Makefile
```

**Código para Makefile**
El Makefile contiene las reglas necesarias para compilar los programas master.c y slave.c.

```makefile
CC = gcc
CFLAGS = -Wall -g -I/usr/include/pvm3
LDFLAGS = -L/usr/lib -lpvm3

TARGETS = master slave

all: $(TARGETS)

master: master.c
        $(CC) $(CFLAGS) master.c -o master $(LDFLAGS)

slave: slave.c
        $(CC) $(CFLAGS) slave.c -o slave $(LDFLAGS)

clean:
        rm -f $(TARGETS) *.o
```

---

## 3. Compilación del Proyecto

Para compilar el proyecto de PVM, utilizaremos el archivo Makefile que acabamos de crear. Este archivo contiene las instrucciones necesarias para compilar los archivos fuente (`master.c` y `slave.c`) y generar los ejecutables correspondientes.

### 3.1. ¿Por qué usar `make`?

`make` es una herramienta de automatización de compilación que facilita el proceso de compilar programas. En lugar de ejecutar manualmente los comandos de compilación para cada archivo, `make` lee las reglas definidas en el archivo Makefile y compila todo automáticamente. Esto simplifica la gestión de dependencias y reduce los errores.

### 3.2. Pasos para Compilar

#### Navega al directorio del proyecto

Asegúrate de estar en el directorio donde se encuentra el archivo Makefile:

```bash
cd $HOME/proyecto_pvm
```

#### Ejecuta make para compilar

Ejecuta el siguiente comando para compilar el proyecto:

```bash
make
```

Este comando hará que `make` lea el archivo Makefile y compile los archivos master.c y slave.c, generando los ejecutables `master` y `slave`.

#### Verifica los Archivos Generados

Si la compilación es exitosa, deberías ver los ejecutables `master` y `slave` en el directorio actual. Verifica que los archivos hayan sido creados con:

```bash
ls
```

#### Copiar archivos a entorno PVM

Copia el archivo `slave` al directorio adecuado dentro del entorno PVM:

```bash
cp slave $HOME/pvm3/bin/LINUX/
```

#### Limpiar Archivos Generados (Opcional)

Si deseas eliminar los archivos generados, como los ejecutables y los archivos objeto, puedes ejecutar:

```bash
make clean
```

---

## 4. Ejecución del Proyecto

Una vez compilado el proyecto, sigue estos pasos para ejecutarlo correctamente.

### 4.1. Iniciar PVM

#### Abrir una Nueva Terminal

Abre una nueva terminal en tu sistema (puedes buscar “Terminal” en el menú de inicio).

#### Crear el Archivo de Hosts de PVM

Crea el archivo de hosts para PVM con el siguiente comando:

```bash
echo 'localhost' > $HOME/pvm3/hostfile
```

#### Iniciar PVM

En la nueva terminal, inicia el entorno de PVM con el siguiente comando:

```bash
pvm
```

#### Añadir localhost al Host de PVM

Para que PVM pueda utilizar tu máquina local como nodo, agrega localhost al archivo de hosts de PVM:

```bash
add localhost
```

### 4.2. Ejecutar el Programa Maestro

#### Navegar a la Carpeta del Proyecto

En la misma terminal donde está corriendo PVM, navega al directorio del proyecto:

```bash
cd $HOME/proyecto_pvm
```

#### Ejecutar el programa maestro con rango

Ejecuta el programa maestro indicando el rango de números que quieres sumar. Por ejemplo, para sumar números del 1 al 10:

```bash
./master 1 10
```

Deberías ver una salida similar a:

```
Maestro: lanzando proceso esclavo...
Maestro: esclavo lanzado con TID 12345
Maestro: rango enviado [1, 10]
Maestro: suma recibida = 55
```

---

## 5. Conclusión

Con este manual, has instalado, compilado y ejecutado un programa simple usando PVM en Ubuntu 18.04. Puedes modificar los códigos para implementar tareas paralelas más complejas usando PVM.

---
