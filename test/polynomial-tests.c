/*
    Some tests with polynomial rings and GF(2^8)
                    olpet / 2018-10
*/

#include <stdio.h>
#include <stdint.h>

#define bitmask(x) (x) >= (UINT32_MAX) ? (0x1 >> 1) : ((uint32_t) 0x1 << (x)) - 0x1

/*
    The AES irreducible AES polynomial is
             100011011 / 0x11B
    For our polynomial division part it is
    much easier to calculate with an already
    shifted version the polynomial within
    a 16bit unsigned integer
*/

#define AES_POLYNOMIAL 0b1000110110000000


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
    for(int i = 0; i < size; i++)
    {
        if((p >> (size-1-i) & 0x1) == 1)
            printf("%2d ", size-1-i);
        else
            printf("-- ");
    }
    printf("\n");
    return;
}
 
uint16_t
multiply_polynomial(uint8_t *factor1, uint8_t *factor2)
{
    uint16_t result = 0;
    for(int i = 0; i < 8; i++)
    {
        if(((*factor2 >> i) & 0x1) == 1)
        {
            uint16_t intermediate = *factor1;
            result ^= (intermediate <<= i);
        }
    }
    return result;
}

int8_t
get_shifts(uint16_t factor)
{
    /*
     * Retrieve the position of the 
     * left most bit in 16 bit factor
    */
    for(int i = 0; i < 16; i++)
    {
        if(((factor >> (16-i)) & 0x1) == 1)
            return i-1;
    }
    return -1;
}

uint8_t
aes_polynomial_division(uint16_t *factor)
{
    uint16_t result = 0;

    /* We need to find out the necessary shifts
       to start reducing the polynomial *factor
       by aes_polynomial */
    int8_t shift = get_shifts(*factor);
    result = *factor;
    while (shift <= 8)
    {
        result ^= AES_POLYNOMIAL >> shift;
        shift = get_shifts(result);
    } 
    return (uint8_t) result;
}

int
main(void)
{
    puts("*** POLYNOMIAL TESTS");
    uint8_t factor1 = 0xb6;
    uint8_t factor2 = 0x53;

    puts("Running polynomial multiplication");

    uint16_t result = 0;
    result = multiply_polynomial(&factor1, &factor2);

    puts("Running polynomial division");

    uint8_t final = aes_polynomial_division(&result);

    show_polynomial(final, 8);

    puts("*** END OF PROGRAM");
    return 0;
}