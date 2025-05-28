#ifndef GOOSE_SUB_H
#define GOOSE_SUB_H

#ifdef __cplusplus
extern "C" {
#endif

void goose_listen_loop(const char* interface);
void set_trip_ptr(volatile int* ptr);

#ifdef __cplusplus
}
#endif

#endif // GOOSE_SUB_H
