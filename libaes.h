#ifndef H_LIBAES
#define H_LIBAES

#include "config.h"

#include <stdint.h>

/*
 * The original Rijndael Algorithm would have support for different
 * input block sizes (128, 192, 256) however the AES paper only uses
 * a single fixed block size which is 128 bit
 */
static const uint16_t AES_BLOCK_SIZE = 128;

static const uint8_t AES_WORD_LENGTH = 32;

static const uint16_t AES_KEY_SIZE = 128;
// static const uint16_t AES_KEY_SIZE = 192;
// static const uint16_t AES_KEY_SIZE = 256;

/* Section 5. Algorithm Specification, FIPS 197 */
static const uint16_t AES_ROUNDS = (AES_KEY_SIZE / AES_WORD_LENGTH) + (AES_BLOCK_SIZE / AES_WORD_LENGTH) + 2;

/* Each round needs a subkey, plus 1 for the additional key whitening */
static const uint16_t AES_SUBKEYS = AES_ROUNDS + 1;

/* Iteration steps for w0, w1, w... within key schedule */
static const uint16_t AES_SUBKEY_PARTS = AES_KEY_SIZE / AES_WORD_LENGTH;

/* TODO */
static const uint16_t AES_NK = AES_BLOCK_SIZE / AES_WORD_LENGTH;

/* TODO */
static const uint16_t AES_WI_RUNS = AES_ROUNDS * 4;

/*
 * We can either use hardcoded substitution tables
 * or calculate them during runtime
 * (0 = hardcoded / 1 = runtime)
 */
#define AES_CALCULATE_LOOKUP_TABLES 1

#if AES_CALCULATE_LOOKUP_TABLES == 1
/*
 * The non-reducible AES Polynomial in GF(2^8)
 *      P(x) = x^8 + x^4 + x^3 + x + 1
 */
static const uint16_t AES_POLYNOMIAL = 0b100011011;

/* 0x03 / x+1 is the smallest generator in AES */
static const uint8_t AES_GENERATOR = 0x03;

/* G L O B A L S */
extern uint8_t  INITIALIZED;
extern uint8_t  AES_BYTE_SUB[16][16];
extern uint8_t  AES_INV_BYTE_SUB[16][16];
extern uint32_t AES_KEY_SCHEDULE[AES_SUBKEYS][AES_SUBKEY_PARTS];
extern uint32_t AES_RCON[10];

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

void rotate_word(uint32_t *aes_word);

int8_t key_addition(uint32_t *key, uint8_t *state);
int8_t byte_substitution(uint8_t *state);
int8_t shift_rows(uint8_t *state);
int8_t mix_column(uint8_t *state);
int8_t key_schedule(const uint8_t *key);

uint8_t sub_byte(uint8_t b);
uint8_t inv_sub_byte(uint8_t b);
uint32_t rcon(uint8_t i);

void inv_byte_substitution();
void inv_shift_rows();
void inv_mix_column();

uint8_t aes_init(const uint8_t *key);
uint8_t aes_encrypt(const uint8_t *key, uint8_t *state);
uint8_t aes_decrypt(const uint8_t *key, uint8_t *state);

#endif