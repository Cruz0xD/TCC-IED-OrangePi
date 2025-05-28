#include "goose_receiver.h"
#include "goose_subscriber.h"
#include "hal_thread.h"
#include "linked_list.h"

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>

// Ponteiro para variável global de trip (externa)
static volatile int* tripA_ptr = NULL;

// Função para permitir que o main defina onde está a variável trip
void set_trip_ptr(volatile int* ptr) {
    tripA_ptr = ptr;
}

// Listener GOOSE chamado a cada nova mensagem recebida
static void gooseListener(GooseSubscriber subscriber, void* parameter)
{
    MmsValue* values = GooseSubscriber_getDataSetValues(subscriber);
    char buffer[1024];

    MmsValue_printToBuffer(values, buffer, 1024);

    // Verifica se a mensagem contém "true" e aciona o TRIP
    if (strstr(buffer, "true") != NULL && tripA_ptr != NULL) {
        *tripA_ptr = 1;
        //printf("GOOSE: TRIP recebido! Sinal trip ativado.\n");
    }
}

// Função que roda indefinidamente ouvindo mensagens GOOSE
void goose_listen_loop(const char* interface)
{
    GooseReceiver receiver = GooseReceiver_create();

    if (interface != NULL)
        GooseReceiver_setInterfaceId(receiver, interface);
    else
        GooseReceiver_setInterfaceId(receiver, "end0"); // Interface Ethernet, mude se necessário

    GooseSubscriber subscriber = GooseSubscriber_create("simpleIOGenericIO/LLN0$GO$gcbTrip", NULL);

    uint8_t dstMac[6] = {0x01, 0x0c, 0xcd, 0x01, 0x00, 0x01};
    GooseSubscriber_setDstMac(subscriber, dstMac);
    GooseSubscriber_setAppId(subscriber, 1000);

    GooseSubscriber_setListener(subscriber, gooseListener, NULL);

    GooseReceiver_addSubscriber(receiver, subscriber);

    GooseReceiver_start(receiver);

    if (!GooseReceiver_isRunning(receiver)) {
        printf("Erro ao iniciar o receptor GOOSE. Verifique a interface de rede e permissões.\n");
        GooseReceiver_destroy(receiver);
        return;
    }

    printf("GOOSE listener iniciado na interface %s.\n", interface ? interface : "end0");

    while (1) {
        Thread_sleep(100); // Verifica a cada 100ms (evita uso excessivo de CPU)
    }

    // Caso queira parar o receiver futuramente:
    // GooseReceiver_stop(receiver);
    // GooseReceiver_destroy(receiver);
}
