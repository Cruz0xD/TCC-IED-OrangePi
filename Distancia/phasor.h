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

Phasor* get_phasors();

#endif // PHASOR_H

