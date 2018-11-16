#include "libaes.h"

/*
 * GLOBALS
 */

/* Defines if algorithm has been initialized already */
uint8_t INITIALIZED = 0;

/* AES Key Schedule */
uint32_t AES_KEY_SCHEDULE[AES_SUBKEYS][AES_SUBKEY_PARTS] = { { 0 } };

/*** TODO CREATE MACRO DEFINITIONS TO HARDCODE SUBSTITUTION TABLES ***/

/* AES Byte Substitution Table */
uint8_t AES_BYTE_SUB[16][16] = { { 0 } };

/* AES Inverse Byte Substitution Table */
uint8_t AES_INV_BYTE_SUB[16][16] = { { 0 } };

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
    {
        gen_bytesub();
        gen_inv_bytesub();
        key_schedule(key);
        INITIALIZED = 1;
    }
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
            wi++;
    }

    // Start actual key schedule iterations
    for(int i = 0; i < (AES_ROUNDS*4*nk); i++)
    {
        uint32_t temp = *(wi-1);
        if(i % nk == 0)
        {
            rotate_word(&temp);
            uint32_t sub[4] = { 0 };
            sub[0] = sub_byte(temp >> 24 & 0xFF) << 24;
            sub[1] = sub_byte(temp >> 16 & 0xFF) << 16;
            sub[2] = sub_byte(temp >>  8 & 0xFF) << 8;
            sub[3] = sub_byte(temp       & 0xFF);
            temp = sub[0] | sub[1] | sub[2] | sub[3];
            uint32_t rcon = 1 << (i / nk);
            rcon = aes_polynomial_division((uint16_t *) &rcon);
            temp ^= (rcon << 24);
        }
        *wi = *(wi-nk) ^ temp;
        wi++;
    }
    puts("* Key Schedule");
    for(int i = 0; i < AES_SUBKEYS; i++)
    {
        for(int j = 0; j < AES_SUBKEY_PARTS; j++)
            printf("0x%08x ",AES_KEY_SCHEDULE[i][j]);
        puts("");
    }
    return 0;
}

/* Left-Shift-Rotate a uint32_t aka AES Word */
void
rotate_word(uint32_t *aes_word)
{
    *aes_word = (*aes_word << 8) | ((*aes_word >> 24) & 0xFF);
}

/* Retrieve the position of the left most bit */
int8_t
get_shifts(uint16_t *factor)
{
    for(int i = 0; i < 16; i++)
    {
        if(((*factor >> (16-i)) & 0x1) == 1)
            return i-1;
    }
    return -1;
}

/* Polynomial division under GF(2^8) */
uint8_t
aes_polynomial_division(uint16_t *factor)
{
    uint16_t result = *factor;
    if(result == 0)
        return 0;
    /* We need to find out the necessary shifts
       to start reducing the polynomial *factor
       by AES_POLYNOMIAL */
    int8_t shift = get_shifts(&result);
    while (shift < 8 && shift >= 0)
    {
        result ^= (AES_POLYNOMIAL << (7 - shift));
        shift = get_shifts(&result);
    } 
    return (uint8_t) result;
}

/* Polynomial multiplication under GF(2^8) */
uint16_t
multiply_polynomial(uint8_t factor1, uint8_t factor2)
{
    /* Some sanity checks */
    if(factor1 == 0 || factor2 == 0)
        return 0;
    if(factor1 == 1)
        return factor2;
    if(factor2 == 1)
        return factor1;

    /* The actual calculation */
    uint16_t result = 0;
    for(int i = 0; i < 8; i++)
    {
        if(((factor2 >> i) & 0x1) == 1)
        {
            uint16_t intermediate = factor1;
            result ^= (intermediate <<= i);
        }
    }
    return result;
}

/* Calculate the Multiplicative of a Byte in GF(2^8) */
uint8_t
multiplicative_inverse(uint8_t input)
{
    /* 0 maps to 0x63; */
    if(input == 0)
        return 0^0x63;

    /*
     * Create logarithm chart
     */
    uint16_t poly = 1;
    uint8_t logarithm[16][16] = { { 0 } };  // 0xF * 0xF Elements
    for(int i = 1; i < 255; i++)
    {
        poly = multiply_polynomial(poly, AES_GENERATOR);
        poly = aes_polynomial_division(&poly);
        uint8_t x = poly & 0xF;
        uint8_t y = poly >> 4;
        logarithm[y][x] = i;
    }

    uint8_t x = input & 0xF;
    uint8_t y = input >> 4;
    uint8_t k = logarithm[y][x];
    k ^= 0xFF;  // substract 255 in GF(2^8)
    uint16_t result = 1;
    for(int j = 0; j < k; j++)
    {
        result = multiply_polynomial(result, AES_GENERATOR);
        result = aes_polynomial_division(&result);
    }
    uint8_t intermediate = result;
    for(int i = 1; i <= 4; i++)
        result ^= lsr8(intermediate, i);
    result ^= 0x63;
    return result;
}

/* left shift rotate a 8bit block k */
uint8_t
lsr8(uint8_t k, uint8_t p)
{
    while(p--)
        k = ((k << 1) & 0x0FE) | ((k >> 7) & 0x1);
    return k;
}

/* Generate and fill global AES Byte Substitution Table */
void
gen_bytesub()
{
    for(int i = 0; i < 256; i++)
    {
        uint8_t result = multiplicative_inverse(i);
        uint8_t x = i & 0xF;
        uint8_t y = i >> 4;
        AES_BYTE_SUB[y][x] = result;
    }
    return;
}

/* Inverse the Byte Substitution Table */
void
gen_inv_bytesub()
{
    for(int i = 0; i < 256; i++)
    {
        uint8_t x = i & 0xF;
        uint8_t y = i >> 4;
        uint8_t inverse = AES_BYTE_SUB[y][x];
        x = inverse & 0xF;
        y = inverse >> 4;
        AES_INV_BYTE_SUB[y][x] = i;
    }
    return;
}

/* Return value of byte substitutin table */
uint8_t
sub_byte(uint8_t b)
{
    uint8_t x = b & 0xF;
    uint8_t y = b >> 4;
    return AES_BYTE_SUB[y][x];
}

/* Return value of inverse byte substitution table */
uint8_t
inv_sub_byte(uint8_t b)
{
    uint8_t x = b & 0xF;
    uint8_t y = b >> 4;
    return AES_INV_BYTE_SUB[y][x];
}

/* Exponent in GF(2^8) */
uint16_t
exp_polynomial(uint16_t factor, uint8_t exponent)
{
    if(exponent == 0)
        return 0;
    uint16_t result = factor;
    for(int i = 0; i < exponent; i++)
    {
        result = multiply_polynomial(result, exponent);
        result = aes_polynomial_division(&result);
    }
    return result;
}