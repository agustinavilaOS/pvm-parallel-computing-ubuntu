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
