/*
 *        Advanced Encryption Standard (2001) dummy program
 *           olpet / 2018-11
 *
 *  W A R N I N G  W A R N I N G  W A R N I N G  W A R N I N G
 *
 *      This program only reads input with a multiple of 16 bytes
 *
 *    %< %< %< %< %< %< %< %< %< %< %< %< %< %< %< %< %< %< %< %<
 *    %<  If your input size does not fit it will be cut off!  %<
 *    %< %< %< %< %< %< %< %< %< %< %< %< %< %< %< %< %< %< %< %<
 *
 *      AES uses an input size of 128bit aka 16 bytes, since we
 *      want to demonstrate the encryption and decryption itself
 *      with an optional CBC mode we don't want to mess around
 *      with a meaningful userland implementation nor do we want
 *      0-padding
 * 
 *  W A R N I N G  W A R N I N G  W A R N I N G  W A R N I N G
 *
 *  Program is for educational purpose only.
 */

#include "main.h"

// Static Encryption Key
static const uint8_t STATIC_KEY[16] = 
{
    0x01, 0x02, 0x03, 0x04,
    0xde, 0xad, 0xbe, 0xef,
    0xca, 0xfe, 0xba, 0xbe,
    0x12, 0x13, 0x15, 0x16
};

// Initialization vector in CBC Mode
#if CIPHER_BLOCK_CHAINING == 1
static const uint8_t IV[16] = 
{
    0x15, 0x14, 0x13, 0x12,
    0x11, 0x10, 0x09, 0x08,
    0x07, 0x06, 0x05, 0x04,
    0x03, 0x02, 0x01, 0x00
};
#endif

void wrong_usage()
{
    printf("Use -d to decrypt.\n");
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    /* Encrypt by default */
    bool decrypt = false;

    /* Command line parsing */
    if (argc != 1 && argc != 2)
        wrong_usage();
    if (argc == 2)
    {
        if (strlen(argv[1]) != 2 || argv[1][1] != 'd')
            wrong_usage();
        decrypt = true;
    }

#if CIPHER_BLOCK_CHAINING == 1
    uint8_t *cbc = malloc(16U);
    memcpy(cbc, IV, 16U);
#endif

    /* The main routines */
    if (decrypt == false)
    {
        /* AES Encryption Routine */
        uint8_t len = 0;
        uint8_t *state = calloc(16, sizeof(uint8_t));
        do
        {
            if (len == 16)
            {
#if CIPHER_BLOCK_CHAINING == 1
                for(int i = 0; i < 16; i++)
                    state[i] ^= cbc[i];
#endif
                aes_encrypt(STATIC_KEY, state);
                for(int i = 0; i < 16; i++)
                    putchar(state[i]);
#if CIPHER_BLOCK_CHAINING == 1
                memcpy(cbc, state, 16U);
#endif
                bzero(state, 16);
                len = 0;
            }
            if (fread(&state[len++], 1U, 1U, stdin) == 0)
                break;
        } while (!feof(stdin));
    }
    else
    {
        /* AES Decryption Routine */
        uint8_t len = 0;
        uint8_t *state = calloc(16, sizeof(uint8_t));
#if CIPHER_BLOCK_CHAINING == 1
        uint8_t *cbc_state = calloc(16, sizeof(uint8_t));
#endif
        do
        {
            if (len == 16)
            {
#if CIPHER_BLOCK_CHAINING == 1
                for(int i = 0; i < 16; i++)
                    cbc_state[i] = state[i];
#endif
                aes_decrypt(STATIC_KEY, state);
#if CIPHER_BLOCK_CHAINING == 1
                for(int i = 0; i < 16; i++)
                    state[i] ^= cbc[i];
#endif
                for(int i = 0; i < 16; i++)
                    putchar(state[i]);
#if CIPHER_BLOCK_CHAINING == 1
                memcpy(cbc, cbc_state, 16U);
#endif
                bzero(state, 16);
                len = 0;
            }
            if (fread(&state[len++], 1U, 1U, stdin) == 0)
                break;
        } while (!feof(stdin));
    }
    exit(EXIT_SUCCESS);
}