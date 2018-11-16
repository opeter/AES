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

/* Iteration steps for w0, w1, w... within key schedule */
#define AES_SUBKEY_PARTS AES_KEY_SIZE / AES_WORD_LENGTH

/*
 * The non-reducible AES Polynomial in GF(2^8)
 *      P(x) = x^8 + x^4 + x^3 + x + 1
 */
#define AES_POLYNOMIAL 0b100011011
/* 0x03 / x+1 is the smallest generator in AES */
#define AES_GENERATOR 0x03

void rotate_word(uint32_t *aes_word);
uint8_t lsr8(uint8_t k, uint8_t p);

int8_t get_shifts(uint16_t *factor);
uint8_t aes_polynomial_division(uint16_t *factor);
uint8_t multiplicative_inverse(uint8_t input);
uint16_t multiply_polynomial(uint8_t factor1, uint8_t factor2);
uint16_t exp_polynomial(uint16_t factor, uint8_t exponent);

void key_addition();
void byte_substitution();
void shift_rows();
void mix_column();
uint8_t key_schedule(const uint16_t *key);

uint8_t sub_byte(uint8_t b);
uint8_t inv_sub_byte(uint8_t b);

void inv_byte_substitution();
void inv_shift_rows();
void inv_mix_column();

void gen_bytesub(void);
void gen_inv_bytesub(void);

uint8_t aes_init(const uint16_t *key);
uint8_t aes_encrypt(const uint16_t *key, const uint16_t *input, uint16_t *output);
uint8_t aes_decrypt(const uint16_t *key, const uint16_t *input, uint16_t *output);

#endif