#ifndef H_LIBAES
#define H_LIBAES

#include "config.h"

#if AES_CALCULATE_LOOKUP_TABLES == 1
/*
 * The non-reducible AES Polynomial in GF(2^8)
 *      P(x) = x^8 + x^4 + x^3 + x + 1
 */
static const uint16_t AES_POLYNOMIAL = 0b100011011;

/* 0x03 / x+1 is the smallest generator in AES */
static const uint8_t AES_GENERATOR = 0x03;

int8_t get_shifts(uint16_t *factor);
uint8_t lsr8(uint8_t k, uint8_t p);
uint8_t aes_polynomial_division(uint16_t *factor);
uint8_t multiplicative_inverse(uint8_t input);
uint16_t multiply_polynomial(uint8_t factor1, uint8_t factor2);
uint16_t exp_polynomial(uint16_t factor, uint8_t exponent);

void gen_rcon_table(void);
void gen_bytesub(void);
void gen_inv_bytesub(void);
void gen_multiplication(void);
#endif

/*
 * AES Macro Definitions to handle lookup tables
 */
#define AES_MULTIPLY_0x2(x) (MULTIPLY_0x2[(x)])
#define AES_MULTIPLY_0x3(x) (MULTIPLY_0x3[(x)])
#define AES_MULTIPLY_0xE(x) (MULTIPLY_0xE[(x)])
#define AES_MULTIPLY_0xB(x) (MULTIPLY_0xB[(x)])
#define AES_MULTIPLY_0xD(x) (MULTIPLY_0xD[(x)])
#define AES_MULTIPLY_0x9(x) (MULTIPLY_0x9[(x)])

#define AES_RC(x) (RC[(x)])
#define SUB_BYTE(x) AES_BYTE_SUB[((x) >> 4)][((x) & 0xF)]
#define INV_SUB_BYTE(x) AES_INV_BYTE_SUB[((x) >> 4)][((x) & 0xF)]

int8_t key_addition(uint32_t *key, uint8_t *state);
int8_t byte_substitution(uint8_t *state);
int8_t shift_rows(uint8_t *state);
int8_t mix_column(uint8_t *state);
int8_t key_schedule(const uint8_t *key);

void rotate_word(uint32_t *aes_word);
int8_t inv_byte_substitution(uint8_t *state);
int8_t inv_shift_rows(uint8_t *state);
int8_t inv_mix_column(uint8_t *state);

uint8_t aes_init(const uint8_t *key);
uint8_t aes_encrypt(const uint8_t *key, uint8_t *state);
uint8_t aes_decrypt(const uint8_t *key, uint8_t *state);

#endif