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
