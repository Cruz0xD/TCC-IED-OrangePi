#ifndef GOOSE_PUB_H
#define GOOSE_PUB_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Publica mensagens GOOSE na interface de rede especificada.
 * 
 * @param interface Nome da interface de rede (ex: "eth0").
 *                  Se NULL, usa "eth0" por padrão.
 * @param trip_ptr     Se trip == 1 há curto e o disjuntor abre, se trip == 0 o sistema esta no estado normal
 */
void goose_publisher_trip(const char* interface, volatile int* trip_ptr);

#ifdef __cplusplus
}
#endif

#endif /* GOOSE_PUB_H */
