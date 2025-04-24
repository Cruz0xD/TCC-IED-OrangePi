#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <stdbool.h>
#include "mergingUnit.h"
#include "phasor.h"

#define DEG2RAD (3.14159265358979323846 / 180.0)
#define ZL0 1.06955250923
#define ZL1 0.318744239164
#define K0 ((ZL0 - ZL1) / (3.0 * ZL1))
#define L 200

typedef struct {
    double R;
    double X;
} Impedance;

// Produto vetorial 
double vectorProduct(Impedance p1, Impedance p2, Impedance p) {
    return (p2.R - p1.R) * (p.X - p1.X) - (p2.X - p1.X) * (p.R - p1.R); 
}

// Verifica se Zr está dentro do quadrilátero (zona de atuação)
bool isInsideQuadrilateral(Impedance Zr, Impedance p1, Impedance p2, Impedance p3, Impedance p4) {
    double prod1 = vectorProduct(p1, p2, Zr);
    double prod2 = vectorProduct(p2, p3, Zr);
    double prod3 = vectorProduct(p3, p4, Zr);
    double prod4 = vectorProduct(p4, p1, Zr);

    return (prod1 >= 0 && prod2 >= 0 && prod3 >= 0 && prod4 >= 0) ||
           (prod1 <= 0 && prod2 <= 0 && prod3 <= 0 && prod4 <= 0);
}

// Cria um fasor complexo a partir de magnitude e fase em graus
static inline double complex fasor_to_complex(Phasor p) {
    return p.magnitude * cexp(I * p.phase * DEG2RAD);
}


int main() {

    // Definição dos vértices do quadrilátero (em R, X) - Região de atuação do relé para faltas fase-terra
    Impedance p1_g = {0.0, 0.0};  // Vértice 1, unidades de terra (R1, X1)
    Impedance p2_g = {-36.6040070666, 63.4};  // Vértice 2, unidades de terra (R2, X2)
    Impedance p3_g = {63.3190743046, 63.4}; // Vértice 3, unidades de terra (R3, X3)
    Impedance p4_g = {58.75721, -23.73945};  // Vértice 4, unidades de terra (R4, X4)
    
    // Definição dos vértices do quadrilátero (em R, X) - Região de atuação do relé para faltas fase-fase
    Impedance p1_p = {0.0, 0.0};  // Vértice 1, unidades de fase (R1, X1)
    Impedance p2_p = {-36.6040070666, 63.4};  // Vértice 2, unidades de fase (R2, X2)
    Impedance p3_p = {93.3190743046, 63.4};  // Vértice 3, unidades de fase (R3, X3)
    Impedance p4_p = {88.13582, -35.60918}; // Vértice 4, unidades de fase (R4, X4)

    while (1) {
        Phasor *ph = get_phasors(); // [VA, VB, VC, IA, IB, IC]

        // Declaração dos vetores de fasores
        Phasor Is[4], Vl[3], Z[6];

        // Conversão dos fasores de corrente e tensão
        double complex IA = fasor_to_complex(ph[3]);
        double complex IB = fasor_to_complex(ph[4]);
        double complex IC = fasor_to_complex(ph[5]);
        double complex VA = fasor_to_complex(ph[0]);
        double complex VB = fasor_to_complex(ph[1]);
        double complex VC = fasor_to_complex(ph[2]);

        // Correntes
        Is[0] = (Phasor){cabs(IA + IB + IC), carg(IA + IB + IC)};        // IN
        Is[1] = (Phasor){cabs(IA - IB), carg(IA - IB)};                  // IAB
        Is[2] = (Phasor){cabs(IB - IC), carg(IB - IC)};                  // IBC
        Is[3] = (Phasor){cabs(IC - IA), carg(IC - IA)};                  // ICA

        // Tensões de linha
        Vl[0] = (Phasor){cabs(VA - VB), carg(VA - VB)};                  // VAB
        Vl[1] = (Phasor){cabs(VB - VC), carg(VB - VC)};                  // VBC
        Vl[2] = (Phasor){cabs(VC - VA), carg(VC - VA)};                  // VCA

        // Impedâncias fase-terra
        Z[0] = (Phasor){cabs(VA / (IA + K0 * fasor_to_complex(Is[0]))), carg(VA / (IA + K0 * fasor_to_complex(Is[0])))}; // ZAT
        Z[1] = (Phasor){cabs(VB / (IB + K0 * fasor_to_complex(Is[0]))), carg(VB / (IB + K0 * fasor_to_complex(Is[0])))}; // ZBT
        Z[2] = (Phasor){cabs(VC / (IC + K0 * fasor_to_complex(Is[0]))), carg(VC / (IC + K0 * fasor_to_complex(Is[0])))}; // ZCT

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

        bool insideAT = isInsideQuadrilateral(ZrAT, p1_g, p2_g, p3_g, p4_g);
        bool insideBT = isInsideQuadrilateral(ZrBT, p1_g, p2_g, p3_g, p4_g);
        bool insideCT = isInsideQuadrilateral(ZrCT, p1_g, p2_g, p3_g, p4_g);
        bool insideAB = isInsideQuadrilateral(ZrAB, p1_p, p2_p, p3_p, p4_p);
        bool insideBC = isInsideQuadrilateral(ZrBC, p1_p, p2_p, p3_p, p4_p);
        bool insideCA = isInsideQuadrilateral(ZrCA, p1_p, p2_p, p3_p, p4_p);
    
        // Em desenvolvimento, falta o acionamento do relé (TRIP)
        if (insideAT && !insideBT && !insideCT && !insideAB && !insideBC && !insideCA) printf("Falta fase-terra AT!\n");
        if (!insideAT && insideBT && !insideCT && !insideAB && !insideBC && !insideCA) printf("Falta fase-terra BT!\n");
        if (!insideAT && !insideBT && insideCT && !insideAB && !insideBC && !insideCA) printf("Falta fase-terra CT!\n");
        if (!insideAT && !insideBT && !insideCT && insideAB && !insideBC && !insideCA) printf("Falta fase-fase AB!\n");
        if (!insideAT && !insideBT && !insideCT && !insideAB && insideBC && !insideCA) printf("Falta fase-fase BC!\n");
        if (!insideAT && !insideBT && !insideCT && !insideAB && !insideBC && insideCA) printf("Falta fase-fase CA!\n");
        if (insideAT && insideBT && !insideCT && insideAB && !insideBC && !insideCA) printf("Falta fase-fase-terra ABT!\n");
        if (!insideAT && insideBT && insideCT && !insideAB && insideBC && !insideCA) printf("Falta fase-fase-terra BCT!\n");
        if (insideAT && !insideBT && insideCT && !insideAB && !insideBC && insideCA) printf("Falta fase-fase-terra CAT!\n");
        if (insideAT && insideBT && insideCT && insideAB && insideBC && insideCA) printf("Falta trifásica!\n");
    }

    return 0;
}
