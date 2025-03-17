#include <stdio.h>
#include <stdlib.h>
#include "mergingUnit.h"
#include "phasor.h"


int main() {


    while (1) {
        // Obtém o fasor
        Phasor Va = get_phasor(0, 1); // Fasor de tensão, dados do canal 0
        Phasor Ia = get_phasor(4, 0); // Fasor de corrente, dados do canal 4

        // Imprime os resultados
        printf("Magnitude Tensao A: %.5f | Fase Tensao A: %.2f graus\n", Va.magnitude, Va.phase);
        printf("Magnitude Corrente A: %.5f | Fase Corrente A: %.2f graus\n", Ia.magnitude, Ia.phase);
        printf("Magnitude Impedancia vista: %.5f | Fase: %.2f graus\n", Va.magnitude / Ia.magnitude, Va.phase - Ia.phase);


        }

    return 0;
}
