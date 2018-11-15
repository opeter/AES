#include "libaes.h"

/*
 * GLOBALS
 */

/* Defines if algorithm has been initialized already */
uint8_t INITIALIZED = 0;

/* Our global key schedule */
uint32_t AES_KEY_SCHEDULE[AES_SUBKEYS][AES_SUBKEY_PARTS] = { { 0 } };

/* Main AES Encryption Routine */
uint8_t
aes_encrypt(const uint16_t *key, const uint16_t *input, uint16_t *output)
{
    // Cipher Key = 2b 7e 15 16 28 ae d2 a6 ab f7 15 88 09 cf 4f 3c
    //           -> a0 fa fe 17 88 54 2c b1 23 a3 39 39 2a 6c 76 05
    aes_init(key);
    return 0;
}

/* Main AES Decryption Routine */
uint8_t
aes_decrypt(const uint16_t *key, const uint16_t *input, uint16_t *output)
{
    aes_init(key);
    return 0;
}

/* Initialize Global Key Schedule */
uint8_t
aes_init(const uint16_t *key)
{
    if(INITIALIZED == 0)
        key_schedule(key);
    return 0;
}

/* AES Key schedule */
uint8_t
key_schedule(const uint16_t *key)
{
    // Fill in w0, w1, w2, w4
    uint32_t *wi = AES_KEY_SCHEDULE[0];
    uint8_t nk = AES_BLOCK_SIZE / AES_WORD_LENGTH;
    for(int i = 0; i < 16; i++)
    {
        *wi <<= 8;
        *wi |= key[i];
        if(i % nk == (nk-1))
        {
            printf("w: 0x%08x\n", *wi);
            wi++;
        }
    }

    // Start actual key schedule iterations
    printf("AES_ROUNDS: %d\n", AES_ROUNDS);
    for(int i = 0; i < AES_ROUNDS; i++)
    {
        uint32_t temp = *(wi-1);
        rotate_word(&temp);
        printf("w: 0x%08x\n", temp);


    }
    return 0;
}

/* Left-Shift-Rotate a uint32_t aka AES Word */
void
rotate_word(uint32_t *aes_word)
{
    *aes_word = (*aes_word << 8) | ((*aes_word >> 24) & 0xF);
}