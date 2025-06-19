#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include "dataModule.h"
#include "phasor.h"
#include "fft.h"


// Calcula o fasor de uma janela de amostras utilizando a Transformada Rápida de Fourier.

static Phasor compute_phasor(double *buffer) {
    double complex fft_output[BUFFER_SIZE];

    // Aplica a FFT ao sinal de entrada
    fft(buffer, fft_output, BUFFER_SIZE);

    // Determina a componente de maior magnitude no espectro de frequência
    int max_index = 0;
    double max_magnitude = 0.0;
    for (int i = 0; i < BUFFER_SIZE / 2; i++) {
        double magnitude = cabs(fft_output[i]);
        if (magnitude > max_magnitude) {
            max_magnitude = magnitude;
            max_index = i;
        }
    }

    return (Phasor){
        .magnitude = max_magnitude * (2.0 / BUFFER_SIZE),
        .phase = carg(fft_output[max_index])
    };
}


// Obtém os fasores de todos os canais do módulo da aquisição de dados (simulado) de uma só vez.

Phasor* get_phasors() {
    static Phasor phasors[NUM_CHANNELS];

    // Obtém a matriz de amostras
    double **matrix = data_read();
    if (matrix == NULL) {
        printf("Erro: Não foi possível obter a matriz de amostras.\n");
        // Retorna vetor com fasores nulos
        for (int i = 0; i < NUM_CHANNELS; i++) {
            phasors[i] = (Phasor){0.0, 0.0};
        }
        return phasors;
    }

    // Calcula o fasor de cada canal
    for (int ch = 0; ch < NUM_CHANNELS; ch++) {
        phasors[ch] = compute_phasor(matrix[ch]);
    }

    return phasors;
}

