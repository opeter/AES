#ifndef H_CONFIG
#define H_CONFIG

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

/*
 * We can either use hardcoded substitution tables
 * or calculate them during runtime
 * (0 = hardcoded / 1 = runtime)
 * 
 * NB:  This makes no difference in memory consumption!
 * 
 */
#define AES_CALCULATE_LOOKUP_TABLES 0

/*
 * The original Rijndael Algorithm would have support for different
 * input block sizes (128, 192, 256) however the AES paper only uses
 * a single fixed block size which is 128 bit
 */
#define AES_BLOCK_SIZE 128

#define AES_WORD_LENGTH 32

#define AES_KEY_SIZE 128
// static const uint16_t AES_KEY_SIZE = 192;
// static const uint16_t AES_KEY_SIZE = 256;

/* Section 5. Algorithm Specification, FIPS 197 */
#define AES_ROUNDS ((AES_KEY_SIZE / AES_WORD_LENGTH) + (AES_BLOCK_SIZE / AES_WORD_LENGTH) + 2)

/* Each round needs a subkey, plus 1 for the additional key whitening */
#define AES_SUBKEYS (AES_ROUNDS + 1)

/* Iteration steps for w0, w1, w... within key schedule */
#define AES_SUBKEY_PARTS (AES_KEY_SIZE / AES_WORD_LENGTH)

/* TODO */
#define AES_NK (AES_BLOCK_SIZE / AES_WORD_LENGTH)

/* TODO */
#define AES_WI_RUNS (AES_ROUNDS * 4)

#endif