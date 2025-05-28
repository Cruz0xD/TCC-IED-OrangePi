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
// Convencional: gcc -Wall -Iincludes_libiec61850 -Llibiec61850-1.5.1/build Algoritmos/Distancia21_A.c Algoritmos/goose_pub.c Algoritmos/fft.c Algoritmos/phasor.c Algoritmos/mergingUnit.c -liec61850 -o IED_A -lm -lpthread
// Orange Pi: gcc -Wall -Iincludes_libiec61850 -Llibiec61850-1.5.1/build Algoritmos/Distancia21_A.c Algoritmos/goose_pub.c Algoritmos/fft.c Algoritmos/phasor.c Algoritmos/mergingUnit.c -liec61850 -o IED_A -lm -lpthread -lwiringPi
// Para executar: sudo chrt -f 80 taskset -c 2 ./IED_A

#include "mergingUnit.h"
#include "phasor.h"
#include "goose_pub.h" // O IED A apenas publica as mensagens GOOSE

#define GPIO_256 19  // wPi 19 corresponde ao GPIO 256
#define GPIO_271 20  // wPi 20 corresponde ao GPIO 271

// Dados da linha de transmissão e definições
#define ZL0 1.06955250923 // Módulo da impedância/km de sequência zero da linha
#define ZL1 0.318744239164 // Módulo da impedância/km de sequência positiva da linha
#define K0 ((ZL0 - ZL1) / (3.0 * ZL1)) // Fator de compensação de sequência zero
#define L 200 // Comprimento da linha em km

volatile int trip = 0; // Sinal global de TRIP
volatile int* trip_ptr = &trip;

// Função de espera em ms
void wait_ms(double milliseconds) {
    struct timespec ts;
    ts.tv_sec = (time_t)(milliseconds / 1000);
    ts.tv_nsec = (long)((milliseconds - ts.tv_sec * 1000) * 1e6);
    nanosleep(&ts, NULL);
}

// Função que retorna o tempo decorrido desde o início da execução do código
double time_since_start() {
    static struct timespec start_time;
    static int initialized = 0;
    struct timespec now;
    // Inicializa o tempo base na primeira chamada
    if (!initialized) {
        clock_gettime(CLOCK_MONOTONIC, &start_time);
        initialized = 1;
    }
    clock_gettime(CLOCK_MONOTONIC, &now);
    int64_t elapsed_sec = now.tv_sec - start_time.tv_sec;
    int64_t elapsed_nsec = now.tv_nsec - start_time.tv_nsec;
    // Corrige caso o campo tv_nsec do tempo atual seja menor que o do tempo inicial
    if (elapsed_nsec < 0) {
        elapsed_sec -= 1;
        elapsed_nsec += 1000000000L;
    }
    return elapsed_sec + elapsed_nsec / 1e9;
}

// Função que aciona os GPIOS para o trip
void trip_(const char* msg) {
    printf("%s\n", msg);
    //digitalWrite(GPIO_256, LOW); // 1º saída de trip // 1 = desliga 0 = liga // Comente essa linha ao rodar o código em um computador convencional
    //digitalWrite(GPIO_271, LOW); // 2º saída de trip // 1 = desliga 0 = liga // Comente essa linha ao rodar o código em um computador convencional
    *trip_ptr = 1;
}

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
    return p.magnitude * cexp(I * p.phase);
}

// Thread que executa a publicação GOOSE
void* goose_thread(void* arg) {
    goose_publisher_trip("end0", (volatile int*)arg); // Mude a interface Ethernet conforme necessário
    return NULL;
}

int main() {
    // Comente o trecho abaixo caso não queira simular a teleproteção
    /* 
    // Sincroniza a hora com o servidor NTP
    int sync_status = system("ntpdate -u pool.ntp.org");
    if (sync_status != 0) {
        fprintf(stderr, "Erro ao sincronizar com o servidor NTP.\n");
        return 1;
    }
    // Defina o horário de início desejado (24h format), deve ser o mesmo horário definido no IED_B
    int start_hour = 20;
    int start_min = 10;
    int start_sec = 0;

    time_t now;
    struct tm start_tm;

    time(&now);
    localtime_r(&now, &start_tm);

    start_tm.tm_hour = start_hour;
    start_tm.tm_min = start_min;
    start_tm.tm_sec = start_sec;

    time_t start_time = mktime(&start_tm);

    // Se a hora já passou hoje, programe para o mesmo horário no dia seguinte
    if (difftime(start_time, now) < 0) {
        start_time += 24 * 3600;
    }
    printf("Esperando até %02d:%02d:%02d para iniciar...\n", 
           start_hour, start_min, start_sec);
    // Espera ativa até o horário
    while (time(NULL) < start_time) {
        wait_ms(1);  // evita sobrecarregar o processador
    }
    printf("Iniciando execução às %02d:%02d:%02d.\n", 
           start_hour, start_min, start_sec);
    */    

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

    pthread_t goose_tid;
    pthread_create(&goose_tid, NULL, goose_thread, (void*)trip_ptr);

    // Cria um arquivo para salvar os intervalos de tempo de resposta, ou seja, o tempo de execução de cada ciclo do while
    FILE *fp = fopen("Dados/dados_intervalos.txt", "w");
    if (fp == NULL) {
        perror("Erro ao abrir o arquivo de saída");
        return 1;
    }
    double t_anterior = time_since_start();


    // Definição dos vértices do quadrilátero (em R, X) - Região de atuação do relé para faltas fase-terra - Zona 1 instantânea
    Impedance p1_g1 = {0.0, 0.0};  // Vértice 1, unidades de terra (R1, X1)
    Impedance p2_g1 = {-29.28321, 50.72};  // Vértice 2, unidades de terra (R2, X2)
    Impedance p3_g1 = {69.85812, 50.72}; // Vértice 3, unidades de terra (R3, X3)
    Impedance p4_g1 = {65.8066, -26.58759};  // Vértice 4, unidades de terra (R4, X4)
    
    // Definição dos vértices do quadrilátero (em R, X) - Região de atuação do relé para faltas fase-fase - Zona 1 instantânea
    Impedance p1_p1 = {0.0, 0.0};  // Vértice 1, unidades de fase (R1, X1)
    Impedance p2_p1 = {-29.28321, 50.72};  // Vértice 2, unidades de fase (R2, X2)
    Impedance p3_p1 = {47.45812, 50.72};  // Vértice 3, unidades de fase (R3, X3)
    Impedance p4_p1 = {43.87107, -17.72506}; // Vértice 4, unidades de fase (R4, X4)


    // Definição dos vértices do quadrilátero (em R, X) - Região de atuação do relé para faltas fase-terra - Zona 2 temporizada
    Impedance p1_g2 = {0.0, 0.0};  // Vértice 1, unidades de terra (R1, X1)
    Impedance p2_g2 = {-43.92481, 76.08};  // Vértice 2, unidades de terra (R2, X2)
    Impedance p3_g2 = {71.18719, 76.08}; // Vértice 3, unidades de terra (R3, X3)
    Impedance p4_g2 = {65.8066, -26.58759};  // Vértice 4, unidades de terra (R4, X4)
    
    // Definição dos vértices do quadrilátero (em R, X) - Região de atuação do relé para faltas fase-fase - Zona 2 temporizada
    Impedance p1_p2 = {0.0, 0.0};  // Vértice 1, unidades de fase (R1, X1)
    Impedance p2_p2 = {-43.92481, 76.08};  // Vértice 2, unidades de fase (R2, X2)
    Impedance p3_p2 = {48.78719, 76.08};  // Vértice 3, unidades de fase (R3, X3)
    Impedance p4_p2 = {43.87107, -17.72506}; // Vértice 4, unidades de fase (R4, X4)


    while (1) {
        // printf("Valor de trip: %d\n", *trip_ptr);
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
    
        // Zona 1 instantânea
        bool insideAT1 = isInsideQuadrilateral(ZrAT, p1_g1, p2_g1, p3_g1, p4_g1);
        bool insideBT1 = isInsideQuadrilateral(ZrBT, p1_g1, p2_g1, p3_g1, p4_g1);
        bool insideCT1 = isInsideQuadrilateral(ZrCT, p1_g1, p2_g1, p3_g1, p4_g1);
        bool insideAB1 = isInsideQuadrilateral(ZrAB, p1_p1, p2_p1, p3_p1, p4_p1);
        bool insideBC1 = isInsideQuadrilateral(ZrBC, p1_p1, p2_p1, p3_p1, p4_p1);
        bool insideCA1 = isInsideQuadrilateral(ZrCA, p1_p1, p2_p1, p3_p1, p4_p1);
    
        if (insideAT1 && !insideBT1 && !insideCT1 && !insideAB1 && !insideBC1 && !insideCA1){
            trip_("Falta fase-terra AT - Zona 1!");
        } else if (!insideAT1 && insideBT1 && !insideCT1 && !insideAB1 && !insideBC1 && !insideCA1){
            trip_("Falta fase-terra BT - Zona 1!");
        } else if (!insideAT1 && !insideBT1 && insideCT1 && !insideAB1 && !insideBC1 && !insideCA1){
            trip_("Falta fase-terra CT - Zona 1!");
        } else if (!insideAT1 && !insideBT1 && !insideCT1 && insideAB1 && !insideBC1 && !insideCA1){
            trip_("Falta fase-fase AB - Zona 1!");
        } else if (!insideAT1 && !insideBT1 && !insideCT1 && !insideAB1 && insideBC1 && !insideCA1){
            trip_("Falta fase-fase BC - Zona 1!");
        } else if (!insideAT1 && !insideBT1 && !insideCT1 && !insideAB1 && !insideBC1 && insideCA1){
            trip_("Falta fase-fase CA - Zona 1!");
        } else if (insideAT1 && insideBT1 && !insideCT1 && insideAB1 && !insideBC1 && !insideCA1){
            trip_("Falta fase-fase-terra ABT - Zona 1!");
        } else if (!insideAT1 && insideBT1 && insideCT1 && !insideAB1 && insideBC1 && !insideCA1){
            trip_("Falta fase-fase-terra BCT - Zona 1!");
        } else if (insideAT1 && !insideBT1 && insideCT1 && !insideAB1 && !insideBC1 && insideCA1){
            trip_("Falta fase-fase-terra CAT - Zona 1!");
        } else if (insideAT1 && insideBT1 && insideCT1 && insideAB1 && insideBC1 && insideCA1){
            trip_("Falta trifásica - Zona 1!");
        }

        // Zona 2 temporizada
        bool insideAT2 = isInsideQuadrilateral(ZrAT, p1_g2, p2_g2, p3_g2, p4_g2);
        bool insideBT2 = isInsideQuadrilateral(ZrBT, p1_g2, p2_g2, p3_g2, p4_g2);
        bool insideCT2 = isInsideQuadrilateral(ZrCT, p1_g2, p2_g2, p3_g2, p4_g2);
        bool insideAB2 = isInsideQuadrilateral(ZrAB, p1_p2, p2_p2, p3_p2, p4_p2);
        bool insideBC2 = isInsideQuadrilateral(ZrBC, p1_p2, p2_p2, p3_p2, p4_p2);
        bool insideCA2 = isInsideQuadrilateral(ZrCA, p1_p2, p2_p2, p3_p2, p4_p2);

        if (insideAT2 && !insideBT2 && !insideCT2 && !insideAB2 && !insideBC2 && !insideCA2){
            wait_ms(200.0); // Espera 200ms para o TRIP
            trip_("Falta fase-terra AT - Zona 2!");
        } else if (!insideAT2 && insideBT2 && !insideCT2 && !insideAB2 && !insideBC2 && !insideCA2){
            wait_ms(200.0); // Espera 200ms para o TRIP
            trip_("Falta fase-terra BT - Zona 2!");
        } else if (!insideAT2 && !insideBT2 && insideCT2 && !insideAB2 && !insideBC2 && !insideCA2){
            wait_ms(200.0); // Espera 200ms para o TRIP
            trip_("Falta fase-terra CT - Zona 2!");
        } else if (!insideAT2 && !insideBT2 && !insideCT2 && insideAB2 && !insideBC2 && !insideCA2){
            wait_ms(200.0); // Espera 200ms para o TRIP
            trip_("Falta fase-fase AB - Zona 2!");
        } else if (!insideAT2 && !insideBT2 && !insideCT2 && !insideAB2 && insideBC2 && !insideCA2){
            wait_ms(200.0); // Espera 200ms para o TRIP
            trip_("Falta fase-fase BC - Zona 2!");
        } else if (!insideAT2 && !insideBT2 && !insideCT2 && !insideAB2 && !insideBC2 && insideCA2){
            wait_ms(200.0); // Espera 200ms para o TRIP
            trip_("Falta fase-fase CA - Zona 2!");
        } else if (insideAT2 && insideBT2 && !insideCT2 && insideAB2 && !insideBC2 && !insideCA2){
            wait_ms(200.0); // Espera 200ms para o TRIP
            trip_("Falta fase-fase-terra ABT - Zona 2!");
        } else if (!insideAT2 && insideBT2 && insideCT2 && !insideAB2 && insideBC2 && !insideCA2){
            wait_ms(200.0); // Espera 200ms para o TRIP
            trip_("Falta fase-fase-terra BCT - Zona 2!");
        } else if (insideAT2 && !insideBT2 && insideCT2 && !insideAB2 && !insideBC2 && insideCA2){
            wait_ms(200.0); // Espera 200ms para o TRIP
            trip_("Falta fase-fase-terra CAT - Zona 2!");
        } else if (insideAT2 && insideBT2 && insideCT2 && insideAB2 && insideBC2 && insideCA2){
            wait_ms(200.0); // Espera 200ms para o TRIP
            trip_("Falta trifásica - Zona 2!");
        }

        // Calcula o intervalo de tempo de execução de um ciclo do while
        double t_atual = time_since_start();
        double delta_t = t_atual - t_anterior;
        t_anterior = t_atual;
        // Salva o intervalo no arquivo 
        fprintf(fp, "%.9f\n", delta_t);
        fflush(fp);  

     wait_ms(1000.0 / 60.0); // Simula o tempo de aquisição de uma janela de amostras (um ciclo 1/60) para cada fasor.

    }
    // pthread_join(goose_tid, NULL);
    return 0;
}
