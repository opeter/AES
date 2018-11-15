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

#define AES_WORD_LENGTH 32

#define AES_KEY_SIZE 128
// #define AES_KEY_SIZE 192
// #define AES_KEY_SIZE 256

/* Section 5. Algorithm Specification, FIPS 197 */
#define AES_ROUNDS (AES_KEY_SIZE / AES_WORD_LENGTH) + (AES_BLOCK_SIZE / AES_WORD_LENGTH) + 2

/* Each round needs a subkey, plus 1 for the additional key whitening */
#define AES_SUBKEYS AES_ROUNDS + 1

#define AES_SUBKEY_PARTS AES_KEY_SIZE / AES_WORD_LENGTH

void rotate_word(uint32_t *aes_word);

void key_addition();
void byte_substitution();
void shift_rows();
void mix_column();
uint8_t key_schedule(const uint16_t *key);

void inv_byte_substitution();
void inv_shift_rows();
void inv_mix_column();

uint8_t aes_init(const uint16_t *key);
uint8_t aes_encrypt(const uint16_t *key, const uint16_t *input, uint16_t *output);
uint8_t aes_decrypt(const uint16_t *key, const uint16_t *input, uint16_t *output);

#endif