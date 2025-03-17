#ifndef MERGING_UNIT_H_  
#define MERGING_UNIT_H_

/** @brief Número total de canais de aquisição. */
#define NUM_CHANNELS 9   

/** @brief Tamanho do buffer de amostras por canal (tamanho da janela de amostragem). */
#define BUFFER_SIZE 32  

/** @brief Relação de transformação de corrente. */
#define RTC 1 // Mudar de acordo com o sistema elétrico analisado

/** @brief Relação de transformação de potêncial. */
#define RTP 1 // Mudar de acordo com o sistema elétrico analisado

/**
 * @brief Lê um conjunto de amostras de um canal específico.
 * 
 * @param channel Índice do canal (0 a NUM_CHANNELS - 1).
 * @param mode Indica se é um canal de tensão (1) ou corrente (0).
 * @return Ponteiro para o buffer de amostras ou `NULL` se a leitura falhar.
 */
double* sv_read(int channel, int mode); 

#endif  // MERGING_UNIT_H_

