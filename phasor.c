#include <stdio.h>
#include <complex.h>
#include "mergingUnit.h"
#include "phasor.h"
#include "fft.h"

#define PI 3.14159265358979323846

/**
 * @brief Calcula o fasor de uma janela de amostras utilizando a Transformada Rápida de Fourier.
 * 
 * O algoritmo aplica a FFT no buffer de entrada e identifica a componente de maior magnitude
 * dentro da metade inferior do espectro, assumindo que corresponde à frequência fundamental.
 * 
 * @param buffer Ponteiro para o buffer de amostras (sinal de entrada)
 * @return Phasor Estrutura contendo a magnitude e fase do sinal analisado
 */
static Phasor compute_phasor(double *buffer) {
    double complex fft_output[BUFFER_SIZE];

    // Aplica a FFT ao sinal de entrada
    fft(buffer, fft_output, BUFFER_SIZE);

    // Determina a componente de maior magnitude no espectro de frequência
    int max_index = 0;
    double max_magnitude = 0.0;
    for (int i = 0; i < BUFFER_SIZE / 2; i++) { // Considera apenas até a metade do espectro
        double magnitude = cabs(fft_output[i]);
        if (magnitude > max_magnitude) {
            max_magnitude = magnitude;
            max_index = i;
        }
    }

    return (Phasor){
        .magnitude = max_magnitude * (2.0 / BUFFER_SIZE),  // Ajuste da magnitude conforme o Algoritmo de Fourier de Um Ciclo
        .phase = carg(fft_output[max_index]) * (180.0 / PI) // Determina a fase da componente de maior magnitude no espectro de frequência
    };
}


/**
 * @brief Obtém o fasor de um canal específico da unidade de medição (Merging Unit).
 * 
 * O fasor é calculado a partir das amostras do canal especificado, utilizando a FFT.
 * Em caso de erro (canal inválido), um fasor nulo é retornado.
 * 
 * @param channel Índice do canal a ser analisado (0 a NUM_CHANNELS-1)
 * @param mode Indica se é um canal de tensão (1) ou corrente (0)
 * @return Phasor Estrutura contendo a magnitude e fase do sinal do canal especificado
 */
Phasor get_phasor(int channel, int mode) {

    // Verifica se o canal é válido
    if (channel < 0 || channel >= NUM_CHANNELS) {
        printf("Erro: Canal inválido (%d)\n", channel);
        return (Phasor){0.0, 0.0}; // Retorna um fasor nulo em caso de erro
    }

    // Obtém as amostras do canal e calcula o fasor correspondente
    return compute_phasor(sv_read(channel, mode));
}