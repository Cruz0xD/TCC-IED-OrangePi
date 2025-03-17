/**
 * @brief Simulação de um Assinante Sample Values (Subscriber SV).
 * 
 * Este código simula a aquisição de dados de tensão e corrente via protocolo Sample Values (SV), 
 * provenientes de uma Merging Unit (MU).
 * 
 * Os dados são obtidos a partir de arquivos de texto gerados pelo ATP (Alternative Transients Program),
 * ajustados conforme as relações de transformação de potencial (RTP) e corrente (RTC). Cada canal 
 * representa uma grandeza elétrica (tensão ou corrente de uma fase).
 * 
 * @details
 * - A frequência de amostragem é definida no ATP.
 * - O tamanho do buffer de amostras é configurado em mergingUnit.h.
 * - A cada chamada de `sv_read(int channel, int mode)`, são lidas `BUFFER_SIZE` amostras de TODOS os canais,
 *   mesmo que apenas um canal seja retornado.
 * - Em chamadas subsequentes, a leitura continua do ponto onde parou.
 * - Se o arquivo atingir o fim, ele é fechado e `sv_read` retorna `NULL`.
 */

#include <stdio.h>
#include <stdlib.h>
#include "mergingUnit.h"

// Ponteiros para os arquivos de dados de cada canal
static FILE *file[NUM_CHANNELS] = {NULL}; 

// Buffers para armazenar as amostras convertidas de cada canal
static double buffers[NUM_CHANNELS][BUFFER_SIZE];

/**
 * @brief Lê BUFFER_SIZE amostras de todos os canais, aplica a relação de transformação
 *        e retorna o buffer do canal solicitado.
 * 
 * @param channel Índice do canal desejado (0 a NUM_CHANNELS - 1).
 * @param mode Indica o tipo de transformação:
 *             - 1: Aplicar Relação de Transformação de Potencial (RTP).
 *             - 0: Aplicar Relação de Transformação de Corrente (RTC).
 * @return double* Ponteiro para o buffer de amostras transformadas ou NULL se houver erro.
 */
double* sv_read(int channel, int mode) {
    // Verifica e abre os arquivos, se necessário
    for (int ch = 0; ch < NUM_CHANNELS; ch++) {
        if (file[ch] == NULL) {
            char filename[50];
            snprintf(filename, sizeof(filename), "Dados_Canal_%d.txt", ch);
            file[ch] = fopen(filename, "r");
            if (file[ch] == NULL) {
                printf("Erro ao abrir o arquivo do canal %d\n", ch);
                return NULL;
            }
        }
    }

    // Lê e aplica a transformação a BUFFER_SIZE amostras de todos os canais
    for (int i = 0; i < BUFFER_SIZE; i++) {
        for (int ch = 0; ch < NUM_CHANNELS; ch++) {
            double raw_value;
            if (fscanf(file[ch], "%lf", &raw_value) != 1) {
                // Fim do arquivo ou erro de leitura
                fclose(file[ch]);
                file[ch] = NULL;
                return NULL;
            }
            // Aplica o fator de transformação conforme o modo
            if (mode == 1) {
                buffers[ch][i] = raw_value * RTP;
            } else {
                buffers[ch][i] = raw_value * RTC;
            }
        }
    }

    // Retorna o buffer do canal solicitado
    return buffers[channel];
}