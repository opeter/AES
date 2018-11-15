/*
 *
 * Advanced Encryption Standard Tests
 *          olpet / 2018-11
 */

#include <stdio.h>

#include "libaes.h"

/*
 * Test inpus taken from Appendix A
 *  Key Expansion Examples
 */

/* We have 4 input rounds */
#define ROUNDS 4

static const uint16_t TEST_INPUT[ROUNDS][16] =
{
    /* bc1bee22e409f96e93d7e117393172a */
    {
        0x6b, 0xc1, 0xbe, 0xe2,
        0x2e, 0x40, 0x9f, 0x96,
        0xe9, 0x3d, 0x7e, 0x11,
        0x73, 0x93, 0x17, 0x2a
    },
    /* ae2d8a571e03ac9c9eb76fac45af8e51 */
    {
        0xae, 0x2d, 0x8a, 0x57,
        0x1e, 0x03, 0xac, 0x9c,
        0x9e, 0xb7, 0x6f, 0xac,
        0x45, 0xaf, 0x8e, 0x51
    },
    /* 30c81c46a35ce411e5fbc1191a0a52ef */
    {
        0x30, 0xc8, 0x1c, 0x46,
        0xa3, 0x5c, 0xe4, 0x11,
        0xe5, 0xfb, 0xc1, 0x19,
        0x1a, 0x0a, 0x5, 0x2ef
    },
    /* f69f2445df4f9b17ad2b417be66c3710 */
    {
        0xf6, 0x9f, 0x24, 0x45,
        0xdf, 0x4f, 0x9b, 0x17,
        0xad, 0x2b, 0x41, 0x7b,
        0xe6, 0x6c, 0x37, 0x10
    }
};

static const uint16_t TEST_KEY[16] =
{
    /* 2b7e151628aed2a6abf7158809cf4f3c */
    0x2b, 0x7e, 0x15, 0x16,
    0x28, 0xae, 0xd2, 0xa6,
    0xab, 0xf7, 0x15, 0x88,
    0x09, 0xcf, 0x4f, 0x3c
};

static const uint16_t TEST_OUTPUT[ROUNDS][16] =
{
    /* 3ad77bb40d7a3660a89ecaf32466ef97 */
    {
        0x3a, 0xd7, 0x7b, 0xb4,
        0x0d, 0x7a, 0x36, 0x60,
        0xa8, 0x9e, 0xca, 0xf3,
        0x24, 0x66, 0xef, 0x97
    },
    /* f5d3d58503b9699de785895a96fdbaaf */
    {
        0xf5, 0xd3, 0xd5, 0x85,
        0x03, 0xb9, 0x69, 0x9d,
        0xe7, 0x85, 0x89, 0x5a,
        0x96, 0xfd, 0xba, 0xaf
    },
    /* 43b1cd7f598ece23881b00e3ed030688 */
    {
        0x43, 0xb1, 0xcd, 0x7f,
        0x59, 0x8e, 0xce, 0x23,
        0x88, 0x1b, 0x00, 0xe3,
        0xed, 0x03, 0x06, 0x88
    },
    /* 7b0c785e27e8ad3f8223207104725dd4 */
    {
        0x7b, 0x0c, 0x78, 0x5e,
        0x27, 0xe8, 0xad, 0x3f,
        0x82, 0x23, 0x20, 0x71,
        0x04, 0x72, 0x5d, 0xd4
    },
};

int
main(void)
{
    puts("*** BEGIN AES TESTS");

    /* INIT */
    aes_init(TEST_KEY);
    puts("EXIT");
    exit(0);

    /* Encryption */
    for(int i = 0; i < ROUNDS; i++)
    {
        printf(" * AES Encryption Round %2d\n", i);
        uint16_t output[16] = { 0 };
        aes_encrypt(TEST_KEY, TEST_INPUT[0], output);
        for(int j = 0; j < 16; j++)
        {
            if(output[j] != TEST_OUTPUT[i][j])
            {
                printf("  x Round %d, Block %2d do not match: 0x%02x != 0x%02x\n",
                    i, j, output[j], TEST_OUTPUT[i][j]);
            }
            else
                puts("  * OK");
        }
        puts("");
    }
    puts("-------------------------------------------------------------");
    /* Decryption */
    for(int i = 0; i < ROUNDS; i++)
    {
        printf(" * AES Decryption Round %2d\n", i);
        uint16_t output[16] = { 0 };
        aes_decrypt(TEST_KEY, TEST_OUTPUT[0], output);
        for(int j = 0; j < 16; j++)
        {
            if(output[j] != TEST_INPUT[i][j])
            {
                printf("  x Round %d, Block %2d do not match: 0x%02x != 0x%02x\n",
                    i, j, output[j], TEST_OUTPUT[i][j]);
            }
            else
                puts("  * OK");
        }
    }
    puts("*** END OF PROGRAM");
    return 0;
}