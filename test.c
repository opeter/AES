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