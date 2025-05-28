#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <stdbool.h>
#include "mergingUnit.h"
#include "phasor.h"

// Comando para rodar o código: gcc Algoritmos/Impedancias.c Algoritmos/fft.c Algoritmos/phasor.c Algoritmos/mergingUnit.c  -o Impedancias -lm
// Comando para executar: sudo chrt -f 80 taskset -c 2 ./Impedancias

#define ZL0 1.06955250923
#define ZL1 0.318744239164
#define K0 ((ZL0 - ZL1) / (3.0 * ZL1))
#define L 200
#define NUM_ITERATIONS 60 // Ajuste conforme o número de linhas do arquivo "dados_ATP.txt" em mergingUnit.c dividido pelo tamanho da janela em mergingUnit.h

typedef struct {
    double R;
    double X;
} Impedance;


// Cria um fasor complexo a partir de magnitude e fase em graus
static inline double complex fasor_to_complex(Phasor p) {
    return p.magnitude * cexp(I * p.phase);
}

int main() {

    // Cria um arquivo com a parte real e imaginária das impedâncias de cada unidade
    FILE *file = fopen("Dados/impedancias.txt", "w"); // Salva na pasta "Dados"
    if (!file) {
        perror("Erro ao abrir arquivo");
        return EXIT_FAILURE;
    }

    fprintf(file, "ZrAT_R, ZrAT_X, ZrBT_R, ZrBT_X, ZrCT_R, ZrCT_X, ZrAB_R, ZrAB_X, ZrBC_R, ZrBC_X, ZrCA_R, ZrCA_X,\n");

    for (int cont = 0; cont < NUM_ITERATIONS; cont++) {
        Phasor *ph = get_phasors(); // [VA, VB, VC, IA, IB, IC]

        // Declaração dos vetores de fasores
        Phasor Z[6];

        // Conversão dos fasores de corrente e tensão
        double complex IA = fasor_to_complex(ph[3]);
        double complex IB = fasor_to_complex(ph[4]);
        double complex IC = fasor_to_complex(ph[5]);
        double complex IN = fasor_to_complex((Phasor){cabs(IA + IB + IC), carg(IA + IB + IC)});
        double complex VA = fasor_to_complex(ph[0]);
        double complex VB = fasor_to_complex(ph[1]);
        double complex VC = fasor_to_complex(ph[2]);

        // Impedâncias fase-terra
        Z[0] = (Phasor){cabs(VA / (IA + K0 * IN)), carg(VA / (IA + K0 * IN))}; // ZAT
        Z[1] = (Phasor){cabs(VB / (IB + K0 * IN)), carg(VB / (IB + K0 * IN))}; // ZBT
        Z[2] = (Phasor){cabs(VC / (IC + K0 * IN)), carg(VC / (IC + K0 * IN))}; // ZCT

        // Impedâncias fase-fase
        Z[3] = (Phasor){cabs((VA - VB) / (IA - IB)), carg((VA - VB) / (IA - IB))}; // ZAB
        Z[4] = (Phasor){cabs((VB - VC) / (IB - IC)), carg((VB - VC) / (IB - IC))}; // ZBC
        Z[5] = (Phasor){cabs((VC - VA) / (IC - IA)), carg((VC - VA) / (IC - IA))}; // ZCA

        double complex ZAT = fasor_to_complex(Z[0]);
        double complex ZBT = fasor_to_complex(Z[1]);
        double complex ZCT = fasor_to_complex(Z[2]);
        double complex ZAB = fasor_to_complex(Z[3]);
        double complex ZBC = fasor_to_complex(Z[4]);
        double complex ZCA = fasor_to_complex(Z[5]);

        Impedance ZrAT = {creal(ZAT), cimag(ZAT)};
        Impedance ZrBT = {creal(ZBT), cimag(ZBT)};
        Impedance ZrCT = {creal(ZCT), cimag(ZCT)};
        Impedance ZrAB = {creal(ZAB), cimag(ZAB)};
        Impedance ZrBC = {creal(ZBC), cimag(ZBC)};
        Impedance ZrCA = {creal(ZCA), cimag(ZCA)};


        fprintf(file, "%.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f\n", ZrAT.R, ZrAT.X, ZrBT.R, ZrBT.X, ZrCT.R, ZrCT.X, ZrAB.R, ZrAB.X, ZrBC.R, ZrBC.X, ZrCA.R, ZrCA.X);
    }

    fclose(file);
    return 0;
}

