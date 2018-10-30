/*
    Some tests with polynomial rings and GF(2^8)
                    olpet / 2018-10
*/

#include <stdio.h>
#include <stdint.h>

#define bitmask(x) (x) >= (UINT32_MAX) ? (0x1 >> 1) : ((uint32_t) 0x1 << (x)) - 0x1

void
show_bits8(uint8_t *bytes)
{
    printf(".... .... .... .... .... .... ");
    for(int i = 0; i < 8; i++)
    {
        printf("%d", ((*bytes >> (7-i)) & 0x1) ? 1 : 0);
        if(i % 4 == 3)
            printf(" ");
    }
    printf("(%u / %X)\n", *bytes, *bytes);
}

void
show_bits16(uint16_t *bytes)
{
    printf(".... .... .... .... ");
    for(int i = 0; i < 16; i++)
    {
        printf("%d", ((*bytes >> (15-i)) & 0x1) ? 1 : 0);
        if(i % 4 == 3)
            printf(" ");
    }
    printf("(%u / %X)\n", *bytes, *bytes);
}

void
show_bits32(uint32_t *bytes)
{
    for(int i = 0; i < 32; i++)
    {
        printf("%d", ((*bytes >> (31-i)) & 0x1) ? 1 : 0);
        if(i % 4 == 3)
            printf(" ");
    }
    printf("(%u / %X)\n", *bytes, *bytes);
}

void
show_polynomial(uint32_t p, uint8_t size)
{
    puts("Showing Polynomial: ");
    for(int i = 0; i < size; i++)
    {
        if((p >> (size-1-i) & 0x1) == 1)
            printf("%2d ", size-1-i);
        else
            printf("-- ");
    }
    printf("\n");
    puts("*** END OF POLYNOMIAL");
    return;
}
 
uint16_t
multiply_polynomial(uint8_t *factor1, uint8_t *factor2)
{
    uint16_t result = 0;
    for(int i = 0; i < 8; i++)
    {
        if(((*factor2 >> (i)) & 0x1) == 1)
        {
            uint16_t intermediate = *factor1;
            result ^= (intermediate <<= i);
        }
    }
    return result;
}

int
main(void)
{
    puts("*** POLYNOMIAL TESTS");
    // 8 4 3 1 0 / the irreducibly AES polynomial
    uint16_t aes_polynomial = 0b100011011;
    show_bits16(&aes_polynomial);

    uint8_t factor1 = 0xb6;
    uint8_t factor2 = 0x53;

    show_bits8(&factor1);
    show_bits8(&factor2);

    puts("Running polynomial multiplication");

    uint16_t result = 0;
    result = multiply_polynomial(&factor1, &factor2);
    puts("RESULT:");
    show_bits16(&result);
    show_polynomial(result, 16);

    uint16_t looking = 0b10011100111010;
    puts("But we are looking for");
    show_bits16(&looking);
    show_polynomial(looking, 16);

    puts("*** END OF PROGRAM");
    return 0;
}