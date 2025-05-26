/*  
 * Fast Fourier Transform  
 * Cooley-Tukey algorithm with 2-radix DFT  
 *  
 * Baseado no código original de Brendan Ashworth (2019)  
 * Modificado para utilizar valores do tipo double no array de entrada  
 *  
 * Licença MIT - Consulte o arquivo fft.c para detalhes  
 */

#include <stdint.h>
#include <complex.h>

void fft_slow(double* x, double complex* X, uint16_t N);

void fft(double* x, double complex* X, uint16_t N);
