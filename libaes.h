/*
 * LibAES - libaes.h
 * 
 * ADVANCED ENCRYPTION STANDARD (AES) Library in C
 *                          oliver.peter / 2018-11
 */
#ifndef H_LIBAES
#define H_LIBAES

/* The static library/algorithm config */
#include "config.h"

/* Some AES constants for "Finit field" arithmetic */
#if AES_CALCULATE_LOOKUP_TABLES == 1
/*
 * The irreducible AES Polynomial in GF(2^8)
 *      P(x) = x^8 + x^4 + x^3 + x + 1
 */
static const uint16_t AES_POLYNOMIAL = 0b100011011;

/* 0x03 / x+1 is the smallest generator in AES */
static const uint8_t AES_GENERATOR = 0x03;
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
#define SUB_BYTE(x) (AES_BYTE_SUB[((x) >> 4)][((x) & 0xF)])
#define INV_SUB_BYTE(x) (AES_INV_BYTE_SUB[((x) >> 4)][((x) & 0xF)])

/* Main AES functionality */
uint8_t aes_init(const uint8_t *key);
uint8_t aes_encrypt(uint8_t *state);
uint8_t aes_decrypt(uint8_t *state);

#endif