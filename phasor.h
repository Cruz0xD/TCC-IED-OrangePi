#ifndef PHASOR_H
#define PHASOR_H

/**
 * @brief Estrutura que representa um fasor.
 * 
 * Contém:
 *  - magnitude: Amplitude do sinal
 *  - phase: Fase do sinal (em graus)
 */
typedef struct {
    double magnitude;  ///< Amplitude do sinal
    double phase;      ///< Fase do sinal em graus
} Phasor;

/**
 * @brief Obtém os fasores de todos os canais de medição.
 * 
 * A função calcula os fasores correspondentes a cada canal de entrada, 
 * utilizando a FFT para obter magnitude e fase.
 * 
 * @return Phasor* Vetor com NUM_CHANNELS elementos representando os fasores.
 */
Phasor* get_phasors();

#endif // PHASOR_H

