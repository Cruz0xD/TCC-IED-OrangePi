#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "mms_value.h"
#include "goose_publisher.h"
#include "hal_thread.h"

/* Executar como root! */
void goose_publisher_trip(const char* interface, volatile int* trip_ptr)
{
    if (interface == NULL){
        interface = "end0";
    }

    printf("Using interface %s\n", interface);

    LinkedList dataSetValues = LinkedList_create();

    // Define o valor do TRIP (1 = ativa, 0 = desativa)
    MmsValue* tripValue = MmsValue_newBoolean(*trip_ptr);
    LinkedList_add(dataSetValues, tripValue);

    CommParameters gooseCommParameters;

    gooseCommParameters.appId = 1000;
    gooseCommParameters.dstAddress[0] = 0x01;
    gooseCommParameters.dstAddress[1] = 0x0c;
    gooseCommParameters.dstAddress[2] = 0xcd;
    gooseCommParameters.dstAddress[3] = 0x01;
    gooseCommParameters.dstAddress[4] = 0x00;
    gooseCommParameters.dstAddress[5] = 0x01;
    gooseCommParameters.vlanId = 0;
    gooseCommParameters.vlanPriority = 4;

    GoosePublisher publisher = GoosePublisher_create(&gooseCommParameters, interface);

    if (publisher) {
        GoosePublisher_setGoCbRef(publisher, "simpleIOGenericIO/LLN0$GO$gcbTrip");
        GoosePublisher_setConfRev(publisher, 1);
        GoosePublisher_setDataSetRef(publisher, "simpleIOGenericIO/LLN0$TripDataset");
        GoosePublisher_setTimeAllowedToLive(publisher, 20);  // 20 ms

        // Retransmitir indefinidamente a cada 1 ms
        while (1) {
            // Atualiza valor booleano com o valor atual de trip_ptr
            MmsValue_setBoolean(tripValue, *trip_ptr);
            if (GoosePublisher_publish(publisher, dataSetValues) == -1) {
                printf("Erro ao enviar mensagem GOOSE!\n");
            }
             Thread_sleep(1);  // Retransmite antes de 20 ms
        }

        GoosePublisher_destroy(publisher); // nunca alcançado
    }
    else {
        printf("Falha ao criar o publisher GOOSE (interface inexistente ou sem permissões).\n");
    }

    LinkedList_destroyDeep(dataSetValues, (LinkedListValueDeleteFunction) MmsValue_delete);
}
