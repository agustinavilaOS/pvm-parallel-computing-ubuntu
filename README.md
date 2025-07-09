---

# 🚀 Manual de Instalación y Ejecución de PVM en Ubuntu 18.04

¿Te imaginas potenciar tu capacidad de cómputo, distribuyendo tareas complejas y acelerando procesos en tu propio clúster?
Con **PVM (Parallel Virtual Machine)** puedes aprovechar el poder del paralelismo en Ubuntu 18.04 de forma sencilla, profesional y escalable.
Sigue esta guía paso a paso y prepárate para transformar tu flujo de trabajo con computación distribuida real.

---

## 1️⃣ Instalación de PVM en Ubuntu 18.04

### 1.1. Instalación de Paquetes Necesarios

Convierte tu Ubuntu en un entorno preparado para la computación paralela instalando estos paquetes esenciales. ¡Listo para experimentar la eficiencia de PVM desde la terminal!

```bash
sudo apt install pvm pvm-dev
```

* `pvm`: Instala la biblioteca y las herramientas necesarias para usar PVM.
* `pvm-dev`: Instala las bibliotecas de desarrollo de PVM.

Completa el entorno agregando herramientas de compilación modernas para desarrollo profesional:

```bash
sudo apt install build-essential -y
```

* `build-essential`: Instala herramientas básicas para la compilación de programas en C, como gcc, make, etc.

---

### 1.2. Creación de Directorios

Organiza tu proyecto desde el inicio, separando ejecutables y código fuente como lo hacen los equipos profesionales. Así, tu flujo de trabajo será mucho más eficiente:

```bash
mkdir -p $HOME/pvm3/bin/LINUX
mkdir -p $HOME/proyecto_pvm
```

* `$HOME/pvm3/bin/LINUX`: Directorio donde se almacenarán los ejecutables de PVM.
* `$HOME/proyecto_pvm`: Directorio donde almacenarás el código fuente del proyecto.

---

### 1.3. Navegar al Directorio del Proyecto

Colócate en tu espacio de trabajo y prepárate para desarrollar y ejecutar tus aplicaciones distribuidas:

```bash
cd $HOME/proyecto_pvm
```

---

## 2️⃣ Creación de Archivos de Código

¿Listo para comenzar a programar y orquestar procesos en paralelo?
En esta sección crearás los archivos clave de tu solución basada en PVM.

---

### 2.1. Crear el Archivo `master.c`

Lidera tu clúster con el proceso maestro:
Abre un editor y comienza a construir la lógica central de tu solución distribuida.

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

---

### 2.2. Crear el Archivo `slave.c`

Ahora, configura el “motor” de cálculo que responde al maestro.
Este proceso esclavo ejecutará los trabajos en paralelo y enviará los resultados automáticamente.

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

---

### 2.3. Crear el Archivo `Makefile`

Agiliza tu desarrollo y compila todo tu proyecto con un solo comando, como lo hacen los equipos profesionales de software.

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

## 3️⃣ Compilación del Proyecto

Haz que tu proyecto cobre vida con una compilación simple, rápida y confiable.
Usa make y olvídate de compilar archivo por archivo.

---

### 3.1. ¿Por qué usar `make`?

`make` es la herramienta que todo desarrollador moderno debe dominar: automatiza la compilación, gestiona dependencias y evita errores manuales. ¡Ahorra tiempo y gana robustez!

---

### 3.2. Pasos para Compilar

#### Navega al directorio del proyecto

Asegúrate de estar en el directorio donde se encuentra el archivo Makefile:

```bash
cd $HOME/proyecto_pvm
```

#### Ejecuta make para compilar

Lanza el proceso de compilación con un solo comando:

```bash
make
```

Este comando hará que `make` lea el archivo Makefile y compile los archivos master.c y slave.c, generando los ejecutables `master` y `slave`.

#### Verifica los Archivos Generados

Confirma que tienes tus ejecutables listos para usar:

```bash
ls
```

#### Copiar archivos a entorno PVM

Ubica el ejecutable esclavo donde lo requiere el entorno de PVM para una ejecución transparente:

```bash
cp slave $HOME/pvm3/bin/LINUX/
```

#### Limpiar Archivos Generados (Opcional)

¿Quieres dejar tu proyecto limpio y profesional?
Elimina archivos temporales y ejecutables fácilmente:

```bash
make clean
```

---

## 4️⃣ Ejecución del Proyecto

¡Momento de la verdad! Ejecuta tu sistema paralelo y comprueba el poder de PVM en acción.

---

### 4.1. Iniciar PVM

#### Abrir una Nueva Terminal

Abre una nueva terminal en tu sistema (puedes buscar “Terminal” en el menú de inicio).

#### Crear el Archivo de Hosts de PVM

Prepara el entorno para reconocer tu máquina como nodo de ejecución:

```bash
echo 'localhost' > $HOME/pvm3/hostfile
```

#### Iniciar PVM

Arranca el entorno de PVM y comienza a gestionar tareas distribuidas:

```bash
pvm
```

#### Añadir localhost al Host de PVM

Haz disponible tu propia máquina para recibir trabajos en paralelo:

```bash
add localhost
```

---

### 4.2. Ejecutar el Programa Maestro

#### Navegar a la Carpeta del Proyecto

En la misma terminal donde está corriendo PVM, navega al directorio del proyecto:

```bash
cd $HOME/proyecto_pvm
```

#### Ejecutar el programa maestro con rango

Inicia tu primer procesamiento paralelo real ejecutando el maestro con los parámetros que desees:

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

## 5️⃣ Conclusión

¡Felicidades! Has montado, compilado y ejecutado tu propio sistema de computación paralela con PVM en Ubuntu.
Con esta base, puedes escalar tus soluciones, optimizar tiempos de procesamiento y enfrentarte a retos de alto rendimiento como los profesionales de la industria.

---

¿Listo para llevar tu desarrollo al siguiente nivel?
**Con PVM y Ubuntu, la computación distribuida está en tus manos.**

---
