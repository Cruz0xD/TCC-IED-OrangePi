#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mergingUnit.h"

// Variáveis estáticas para controle interno
static long current_line = 0; // Linha atual sincronizada
static FILE* file = NULL;
static int call_count = 0;    // Contador de chamadas

// Função sv_read retorna matriz [NUM_CHANNELS][BUFFER_SIZE]
double** sv_read() {
    static double buffer[NUM_CHANNELS][BUFFER_SIZE];
    static double* matrix_ptrs[NUM_CHANNELS]; // Ponteiros para as linhas
    char line[MAX_LINE_LENGTH];

    if (file == NULL) {
        file = fopen("Dados/dados_ATP.txt", "r");  // Abre o arquivo de dados da simulação do ATP
        if (file == NULL) {
            perror("Erro ao abrir o arquivo");
            return NULL;
        }
    } else {
        rewind(file);
    }

    // Inicializa os ponteiros para as linhas
    for (int i = 0; i < NUM_CHANNELS; i++) {
        matrix_ptrs[i] = buffer[i];
    }

    // Pula até a linha correta
    for (long i = 0; i < current_line; ++i) {
        if (fgets(line, sizeof(line), file) == NULL) {
            fprintf(stderr, "Fim do arquivo antes da leitura esperada.\n");
            return NULL;
        }
    }

    int count = 0;
    while (count < BUFFER_SIZE && fgets(line, sizeof(line), file) != NULL) {
        char* token;
        char* rest = line;

        for (int ch = 0; ch < NUM_CHANNELS; ch++) {
            token = strtok(ch == 0 ? rest : NULL, ",");
            if (token == NULL) {
                fprintf(stderr, "Erro ao processar a linha %ld\n", current_line + count);
                return NULL;
            }

            double value = atof(token);
            if (ch < 3) {
                // Canais 0,1,2 -> tensão (multiplica por RTP)
                buffer[ch][count] = value * RTP;
            } else {
                 buffer[ch][count] = value * RTC; // Para o algoritmo de Distância: Canais 3,4,5 -> corrente (multiplica por RTC) : Mude conforme o algoritmo a ser testado
                // buffer[ch][count] = value * RTP; // Para o algoritmo da compensação série: Canais 3,4,5 -> tensão (multiplica por RTP) : Mude conforme o algoritmo a ser testado
            }
        }

        count++;
    }

    // Imprime qual chamada da função sv_read, ou seja, qual o número atual da janela de amostras. Comente se necessário
    call_count++;
    printf("Chamada%d - current_line = %ld\n", call_count, current_line);

    // Imprime a matriz resultante para fins de depuração apenas
    /*
    for (int ch = 0; ch < NUM_CHANNELS; ch++) {
        printf("Canal %d - Buffer = [", ch);
        for (int i = 0; i < BUFFER_SIZE; i++) {
            printf("%.3f", buffer[ch][i]);
            if (i < BUFFER_SIZE - 1) printf(", ");
        }
        printf("]\n");
    }
    */

    current_line += count;
    return matrix_ptrs;
}

