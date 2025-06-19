#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <stdbool.h>
#include <pthread.h>
#include <math.h>
#include <time.h>
#include <stdint.h>  
//#include <wiringPi.h>  // Comente essa linha ao rodar o código em um computador convencional

// Comandos para rodar o código em um computador: 
// Convencional: gcc -Wall -Iincludes_libiec61850 -Llibiec61850-1.5.1/build Algoritmos/Compser.c Algoritmos/fft.c Algoritmos/phasor.c Algoritmos/dataModule.c -o IED_Compser -lm 
// Orange Pi: gcc -Wall -Iincludes_libiec61850 -Llibiec61850-1.5.1/build Algoritmos/Compser.c Algoritmos/fft.c Algoritmos/phasor.c Algoritmos/dataModule.c -o IED_Compser -lm -lwiringPi
// Para executar: sudo chrt -f 80 taskset -c 2 ./IED_Compser

#include "dataModule.h"
#include "phasor.h"

#define GPIO_256 19  // wPi 19 corresponde ao GPIO 256
#define GPIO_271 20  // wPi 20 corresponde ao GPIO 271

#define ZL0 1.06955250923
#define ZL1 0.318744239164
#define K0 ((ZL0 - ZL1) / (3.0 * ZL1))
#define L 200

volatile int trip = 0; // Sinal global de TRIP
volatile int* trip_ptr = &trip;

// Função que aciona os GPIOS para o trip
void trip_(const char* msg) {
    printf("%s\n", msg);
    //digitalWrite(GPIO_256, LOW); // 1º saída de trip // 1 = desliga 0 = liga // Comente essa linha ao rodar o código em um computador convencional
    //digitalWrite(GPIO_271, LOW); // 2º saída de trip // 1 = desliga 0 = liga // Comente essa linha ao rodar o código em um computador convencional
    *trip_ptr = 1;
}


// Cria um fasor complexo a partir de magnitude e fase em graus
static inline double complex fasor_to_complex(Phasor p) {
    return p.magnitude * cexp(I * p.phase);
}

// Retorna ângulo de fase em graus da divisão de dois fasores no intervalo 0 a 180 ou 0 a -180
static inline double phase_angle_div(Phasor p1, Phasor p2) {
    double angle = p1.phase*(180/3.14159265358979323846) - p2.phase*(180/3.14159265358979323846); 

    // Normaliza para o intervalo (-180, 180]
    while (angle <= -180.0) angle += 360.0;
    while (angle > 180.0) angle -= 360.0;

    // Força para intervalo [0,180] ou [0,-180]
    if (angle < 0)
        angle = fmax(angle, -180.0);
    else
        angle = fmin(angle, 180.0);

    // Arredonda para 3 casas decimais
    angle = round(angle * 1000.0) / 1000.0;

    return angle;
}


int main() {
    // Cria um arquivo .txt para salvar os ângulo das razões entre as tensões 
    FILE *file = fopen("Dados/angulos.txt", "w");
    if (!file) {
        perror("Erro ao abrir arquivo");
        return EXIT_FAILURE;
    }
    fprintf(file, "VA1_VA2, VB1_VB2, VC1_VC2, VAB1_VAB2, VBC1_VBC2, VCA1_VCA2\n");

    // Comente o trecho abaixo ao rodar o código em um computador convencional
    /*
    // Inicializa a biblioteca wiringPi
    if (wiringPiSetup() == -1) {
        printf("Falha ao inicializar o wiringPi!\n");
        return 1;
    }
    // Configura os pinos como saída
    pinMode(GPIO_256, OUTPUT); 
    pinMode(GPIO_271, OUTPUT); 

    // Desativa o módulo relé (Trip) caso esteja ligado
    digitalWrite(GPIO_256, HIGH); 
    digitalWrite(GPIO_271, HIGH);  
    */
    

    while (1) {
        Phasor *ph = get_phasors(); // [VA1, VB1, VC1, VA2, VB2, VC2]

        // Declaração dos vetores de fasores
        Phasor Vl1[3], Vl2[3];

        // Tensões de fase atrás do banco de capacitores
        double complex VA1 = fasor_to_complex(ph[0]);
        double complex VB1 = fasor_to_complex(ph[1]);
        double complex VC1 = fasor_to_complex(ph[2]);

        // Tensões de fase a frente do banco de capacitores
        double complex VA2 = fasor_to_complex(ph[3]);
        double complex VB2 = fasor_to_complex(ph[4]);
        double complex VC2 = fasor_to_complex(ph[5]);

        // Tensões de linha atrás do banco de capacitores
        Vl1[0] = (Phasor){cabs(VA1 - VB1), carg(VA1 - VB1)};                  // VAB1
        Vl1[1] = (Phasor){cabs(VB1 - VC1), carg(VB1 - VC1)};                  // VBC1
        Vl1[2] = (Phasor){cabs(VC1 - VA1), carg(VC1 - VA1)};                  // VCA1

        // Tensões de linha a frente do banco de capacitores
        Vl2[0] = (Phasor){cabs(VA2 - VB2), carg(VA2 - VB2)};                  // VAB2
        Vl2[1] = (Phasor){cabs(VB2 - VC2), carg(VB2 - VC2)};                  // VBC2
        Vl2[2] = (Phasor){cabs(VC2 - VA2), carg(VC2 - VA2)};                  // VCA2

        // Ângulos de fase (phase) das razões Vf1/Vf2 (Tensões de fase)
        double ph_VA1_VA2 = phase_angle_div(ph[0], ph[3]); // ângulo da razão VA1/VA2
        double ph_VB1_VB2 = phase_angle_div(ph[1], ph[4]); // ângulo da razão VB1/VB2
        double ph_VC1_VC2 = phase_angle_div(ph[2], ph[5]); // ângulo da razão VC1/VC2

        // Ângulos de fase (phase) das razões Vl1/Vl2 (Tensões de linha)
        double ph_VAB1_VAB2 = phase_angle_div(Vl1[0], Vl2[0]); // ângulo da razão VAB1/VAB2
        double ph_VBC1_VBC2 = phase_angle_div(Vl1[1], Vl2[1]); // ângulo da razão VBC1/VBC2
        double ph_VCA1_VCA2 = phase_angle_div(Vl1[2], Vl2[2]); // ângulo da razão VCA1/VCA2

        fprintf(file, "%.3f, %.3f, %.3f, %.3f, %.3f, %.3f\n", ph_VA1_VA2, ph_VB1_VB2, ph_VC1_VC2, ph_VAB1_VAB2, ph_VBC1_VBC2, ph_VCA1_VCA2);


        // Para faltas AT
        if(ph_VA1_VA2 > 0.000 && ph_VBC1_VBC2 == 0.000){
            trip_("Falta AT atrás do banco de capacitores!");}
        if(ph_VA1_VA2 < 0.000 && ph_VBC1_VBC2 == 0.000){
            trip_("Falta AT a frente do banco de capacitores!");}

        // Para faltas BT
        if(ph_VB1_VB2 > 0.000 && ph_VCA1_VCA2 == 0.000){
            trip_("Falta BT atrás do banco de capacitores!");}
        if(ph_VB1_VB2 < 0.000 && ph_VCA1_VCA2 == 0.000){
            trip_("Falta BT a frente do banco de capacitores!");}

        // Para faltas CT
        if(ph_VC1_VC2 > 0.000 &&  ph_VAB1_VAB2 == 0.000){
            trip_("Falta CT atrás do banco de capacitores!");}
        if(ph_VC1_VC2 < 0.000 &&  ph_VAB1_VAB2 == 0.000){
            trip_("Falta CT a frente do banco de capacitores!");}

        // Para faltas AB
        if(ph_VAB1_VAB2 > 0.000 &&  ph_VC1_VC2 == 0.000){
            trip_("Falta AB atrás do banco de capacitores!");}
        if(ph_VAB1_VAB2 < 0.000 &&  ph_VC1_VC2 == 0.000){
           trip_("Falta AB a frente do banco de capacitores!");}

        // Para faltas BC
        if(ph_VBC1_VBC2 > 0.000 &&  ph_VA1_VA2 == 0.000){
            trip_("Falta BC atrás do banco de capacitores!");}
        if(ph_VBC1_VBC2 < 0.000 &&  ph_VA1_VA2 == 0.000){
            trip_("Falta BC a frente do banco de capacitores!");}

        // Para faltas CA
        if(ph_VCA1_VCA2 > 0.000 &&  ph_VB1_VB2 == 0.000){
            trip_("Falta CA atrás do banco de capacitores!");}
        if(ph_VCA1_VCA2 < 0.000 &&  ph_VB1_VB2 == 0.000){
            trip_("Falta CA a frente do banco de capacitores!");}

        // Para faltas trifásicas ABC ou ABCT
        if(ph_VA1_VA2 > 0.000 && ph_VB1_VB2 > 0.000 && ph_VC1_VC2 > 0.000 && ph_VAB1_VAB2 > 0.000 && ph_VBC1_VBC2 > 0.000 && ph_VCA1_VCA2 > 0.000){
            trip_("Falta trifásica atrás do banco de capacitores!");}
        if(ph_VA1_VA2 < 0.000 && ph_VB1_VB2 < 0.000 && ph_VC1_VC2 < 0.000 && ph_VAB1_VAB2 < 0.000 && ph_VBC1_VBC2 < 0.000 && ph_VCA1_VCA2 < 0.000){
            trip_("Falta trifásica a frente do banco de capacitores!");}

    }

    return 0;
}
