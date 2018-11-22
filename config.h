/*
 * LibAES - config.h
 * 
 * In this file we statically configure the algorithm's
 * properties, like sizes for keys and input blocks.
 * 
 */
#ifndef H_CONFIG
#define H_CONFIG

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

/*
 * Debug Mode (0 = off / 1 = on)
 */
#define DEBUG 0

/*
 * AES_CALCULATE_LOOKUP_TABLES defines if the library
 * uses pre-calculated, harcoded lookup tables to run
 * the algorithm.  If calculation is enabled (=1) the
 * program initially calculates the tables using
 * "Finite Field" arithmetic.
 * 
 * NB:  This makes no difference in memory consumption!
 *      It is more like a proof of concept instead.
 * 
 * (0 = hardcoded / 1 = runtime)
 */
#define AES_CALCULATE_LOOKUP_TABLES 1

/*
 * The original Rijndael Algorithm would have support for different
 * input block sizes (128, 192, 256) however the AES paper only uses
 * a single fixed block size which is 128 bit
 */
#define AES_BLOCK_SIZE 128

/*
 * An AES Word consists of 32bit
 */
#define AES_WORD_LENGTH 32

/*
 * LibAES Default is a key size of 128bit
 *  (Possible values are 128/192/256)
 */
#define AES_KEY_SIZE 128

/*
 * Depending on the key length above we have to run
 * a different number of {enc,de}cryption rounds.
 * 
 * Section 5. Algorithm Specification, FIPS 197
 */
#define AES_ROUNDS ((AES_KEY_SIZE / AES_WORD_LENGTH) + (AES_BLOCK_SIZE / AES_WORD_LENGTH) + 2)

/*
 * Each round has a dedicated subkey,
 * plus 1 for the additional key whitening
 */
#define AES_SUBKEYS (AES_ROUNDS + 1)

/*
 * Depending on the key size, the key in our key schedule
 * has a different amount of parts (each part is a AES_WORD_LENGTH)
 */
#define AES_SUBKEY_PARTS (AES_KEY_SIZE / AES_WORD_LENGTH)

/* 
 * Iteration steps for w0, w1, w... within key schedule
 * AFTER the initial set has been created (depending
 * on the key size!)
 */
#define AES_WI_RUNS (AES_ROUNDS * 4)

#endif