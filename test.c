/*
 *
 * Advanced Encryption Standard Tests
 *          olpet / 2018-11
 */

#include "test.h"

int
main(void)
{
    puts("*** BEGIN AES TESTS");

    /* INIT */
    aes_init(TEST_KEY);

    /* Encryption */
    for(int i = 0; i < ROUNDS; i++)
    {
        printf(" * AES Encryption Round %2d\n", i);
        uint8_t state[16] = { 0 };
        memcpy(state, TEST_INPUT, 16U);
        aes_encrypt(TEST_KEY, state);
        for(int j = 0; j < 16; j++)
        {
            if(state[j] != TEST_OUTPUT[i][j])
            {
                printf("  x Round %d, Block %2d do not match: 0x%02x != 0x%02x\n",
                    i, j, state[j], TEST_OUTPUT[i][j]);
            }
            else
                puts("  * OK");
        }
        puts("");
        exit(1);
    }
    puts("-------------------------------------------------------------");
    /* Decryption */
    for(int i = 0; i < ROUNDS; i++)
    {
        printf(" * AES Decryption Round %2d\n", i);
        uint8_t state[16] = { 0 };
        memcpy(state, TEST_OUTPUT, 16U);
        aes_decrypt(TEST_KEY, state);
        for(int j = 0; j < 16; j++)
        {
            if(state[j] != TEST_INPUT[i][j])
            {
                printf("  x Round %d, Block %2d do not match: 0x%02x != 0x%02x\n",
                    i, j, state[j], TEST_OUTPUT[i][j]);
            }
            else
                puts("  * OK");
        }
    }
    puts("*** END OF PROGRAM");
    return 0;
}