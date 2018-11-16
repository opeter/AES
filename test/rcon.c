#include <stdio.h>
#include <stdint.h>

/* Pre-generate all possible values for RCON */

uint8_t rcon[256] = { 0 };

#define AES_POLYNOMIAL 0b100011011

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

int
main(void)
{
    /* Static AES Globals */
    static const uint16_t AES_KEY_SIZE[3] = { 128, 192, 256};
    static const uint16_t AES_BLOCK_SIZE = 128;
    static const uint8_t  AES_WORD_LENGTH = 32;

    /* Const AES Globals (some calculation needed) */
    const uint16_t AES_MAX_ROUNDS[3] =
    {
        (AES_KEY_SIZE[0] / AES_WORD_LENGTH) + (AES_BLOCK_SIZE / AES_WORD_LENGTH) + 2,
        (AES_KEY_SIZE[1] / AES_WORD_LENGTH) + (AES_BLOCK_SIZE / AES_WORD_LENGTH) + 2,
        (AES_KEY_SIZE[2] / AES_WORD_LENGTH) + (AES_BLOCK_SIZE / AES_WORD_LENGTH) + 2
    };
    const uint8_t AES_NK[3] =
    {
        AES_KEY_SIZE[0] / AES_WORD_LENGTH,
        AES_KEY_SIZE[1] / AES_WORD_LENGTH,
        AES_KEY_SIZE[2] / AES_WORD_LENGTH
    };
    const uint16_t AES_WI_RUNS[3] =
    {
        AES_MAX_ROUNDS[0] * 4,
        AES_MAX_ROUNDS[1] * 4,
        AES_MAX_ROUNDS[2] * 4,
    };

    for(int i = 0; i < 3; i++)
    {
        printf("** All possible RCON values for %dbit keyspace\n", AES_KEY_SIZE[i]);
        printf("AES_MAX_ROUNDS: %d\n", AES_MAX_ROUNDS[i]);
        printf("AES_NK        : %d\n", AES_NK[i]);
        printf("AES_WI_RUNS   : %d\n", AES_WI_RUNS[i]);
        printf("\nstatic const uint32_t AES_RCON[] = {\n\t");
        for(int j = 0; j < AES_WI_RUNS[i]; j++)
        {
            if(j % AES_NK[i] == 0)
            {
                uint32_t rcon = 1 << (j / AES_NK[i] );
                rcon = aes_polynomial_division((uint16_t *) &rcon);
                rcon <<= 24;
                printf("/* %2d */ 0x%08x, ", j / AES_NK[i], rcon);
            }
        }
        puts("\n};\n");
        printf("------------------------------------------------\n");
    }
    return 0;
}