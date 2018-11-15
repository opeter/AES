#ifndef H_LIBAES
#define H_LIBAES

#include "config.h"

#include <stdint.h>

/*
 * The original Rijndael Algorithm would have support for different
 * input block sizes (128, 192, 256) howerver the AES paper only uses
 * a single fixed block size which is 128 bit
 */
#define AES_BLOCK_SIZE 128

#define WORD_LENGTH 32

#define AES_KEY_SIZE 128
// #define AES_KEY_SIZE 192
// #define AES_KEY_SIZE 256

/* Section 5. Algorithm Specification, FIPS 197 */
#define AES_ROUNDS (AES_KEY_SIZE / WORD_LENGTH) + (AES_BLOCK_SIZE / WORD_LENGTH) + 2

/* Each round needs a subkey, plus 1 for the additional key whitening */
#define AES_SUBKEYS AES_ROUNDS + 1

void key_addition();
void byte_substitution();
void shift_rows();
void mix_column();
void key_schedule();

void inv_byte_substitution();
void inv_shift_rows();
void inv_mix_column();

uint8_t aes_encrypt(const uint16_t *key, const uint16_t *input, uint16_t *output);
uint8_t aes_decrypt(const uint16_t *key, const uint16_t *input, uint16_t *output);

#endif