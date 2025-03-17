#ifndef PHASOR_H
#define PHASOR_H

/**
 * @brief Estrutura que representa um fasor.
 * 
 * Ele contém:
 *  - A magnitude (amplitude do sinal)
 *  - A fase (ângulo de deslocamento em relação à origem, em graus)
 */
typedef struct {
    double magnitude;  ///< Amplitude do sinal analisado
    double phase;      ///< Fase do sinal em graus
} Phasor;

/**
 * @brief Obtém o fasor de um canal específico de medição.
 * 
 * Esta função calcula o fasor correspondente ao sinal de um canal específico,
 * utilizando a Transformada Rápida de Fourier para extrair magnitude e fase.
 * 
 * @param channel Índice do canal a ser analisado (0 a NUM_CHANNELS-1)
 * @param mode Indica se é um canal de tensão (1) ou corrente (0)
 * @return Phasor Estrutura contendo a magnitude e fase do sinal do canal
 */
Phasor get_phasor(int channel, int mode);

#endif // PHASOR_H
