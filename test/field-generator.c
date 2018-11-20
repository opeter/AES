/*
    A FIELD GENERATOR

*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define GENERATOR 0x03  // 0x03 / x+1 is the smallest generator in AES

#define AES_POLYNOMIAL 0b1000110110000000

#define bitmask(x) (x) >= (UINT32_MAX) ? (0x1 >> 1) : ((uint32_t) 0x1 << (x)) - 0x1

uint64_t
power(uint64_t factor, uint8_t exponent)
{
    uint64_t result = factor;
    for(int i = 1; i < exponent; i++)
        result *= factor;
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
aes_polynomial_division(uint16_t factor)
{
    uint16_t result = 0;
    if(factor == 0)
        return 0;

    /* We need to find out the necessary shifts
       to start reducing the polynomial *factor
       by aes_polynomial */
    int8_t shift = get_shifts(factor);
    result = factor;
    while (shift < 8 && shift >= 0)
    {
        result ^= AES_POLYNOMIAL >> shift;
        shift = get_shifts(result);
    } 
    return (uint8_t) result;
}

uint16_t
multiply_polynomial(uint8_t factor1, uint8_t factor2)
{
    uint16_t result = 0;
    if(factor1 == 0 || factor2 == 0)
        return 0;
    if(factor1 == 1)
        return factor2;
    if(factor2 == 1)
        return factor1;

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

uint64_t
power_polynomial(uint64_t factor, uint8_t exponent)
{
    if(exponent == 0)
    {
        return 0;
    }
    uint64_t result = factor;
    for(int i = 1; i < exponent; i++)
    {
        result = multiply_polynomial(result, factor);
        result = aes_polynomial_division(result);
    }
    return result;
}

void
show_polynomial(uint8_t polynomial)
{
    bool first = true;
    if(polynomial > 0)
    {
        printf("0x%02X -> ", polynomial);
        for(int i = 0; i < 8; i++)
            printf("%d", (polynomial >> (7-i)) & 0x1 ? 1 : 0);
        printf(" -> ");
        for(int i = 0; i < 8; i++)
        {
            uint8_t bit = (polynomial >> (7-i)) & 0x1;
            if(bit == 1)
            {
                if(first == true)
                    first = false;
                else
                    printf("+ ");
                switch(7-i)
                {
                    case 0:
                        printf("1");
                        break;
                    case 1:
                        printf("x ");
                        break;
                    default:
                        printf("x^%d ", 7-i);
                        break;
                }
            }
        }
        puts("");
    }
    return;
}

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
    printf("(%3u / 0x%02X)\n", *bytes, *bytes);
}

/*
 * left shift rotate a 8bit block k
 */
uint8_t
lsr8(uint8_t k, uint8_t p)
{
    while(p--)
        k = ((k << 1) & 0x0FE) | ((k >> 7) & 0x1);
    return k;
}

uint8_t
multiplicative_inverse(uint8_t input)
{
    if(input == 0)
        return 0^0x63;

    /*
     * Create logarithm chart
     */
    uint16_t poly = 1;
    uint8_t logarithm[16][16] = { 0 };  // 0xF * 0xF Elements
    for(int i = 1; i < 255; i++)
    {
        poly = multiply_polynomial(poly, GENERATOR);
        poly = aes_polynomial_division(poly);
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
        result = multiply_polynomial(result, GENERATOR);
        result = aes_polynomial_division(result);
    }
    uint8_t intermediate = result;
    for(int i = 1; i <= 4; i++)
        result ^= lsr8(intermediate, i);
    result ^= 0x63;
    return result;
}

struct Generator {
    bool valid;
    uint8_t g;
    uint8_t field[256];
    uint8_t identical;
};

struct Generators {
    struct Generator generators[256-2];
};

int
main(void)
{
    // A generator is an element whose successive powers
    // take on every element except the zero

    /*
        Generate all possible generators in GF(2^8) mod P(x)
    */
    struct Generators generators;

    // Add all possible generators to the struct
    for(int i = 2; i < 256; i++)
    {
        uint16_t r = 1;
        struct Generator *generator = &generators.generators[i-2];
        generator->g = i;
        generator->identical = 0;
        generator->valid = false;   // not yet known
        for(int j = 0; j < 256; j++)
        {
            r = multiply_polynomial(r, i);
            r = aes_polynomial_division(r);
            generator->field[j] = r;
        }
    }

    // Evaluate how many elements in each generator are unique
    for(int i = 2; i < 256; i++)
    {
        struct Generator *generator = &generators.generators[i-2];
        for(int j = 0; j < 256/2; j++)
        {
            // all elements *after* current position
            // > j
            for(int k = j+1; k < 256; k++)
            {
                if(generator->field[k] == generator->field[j])
                    generator->identical++;
            }
            // all elements *before* current position
            // < j
            for(int k = 0; k < j-1; k++)
            {
                if(generator->field[k] == generator->field[j])
                    generator->identical++;
            }

        }
        /* 
           A valid generator means that only the first and the last
           element of the exponentiation table match each other, 
           the rest has to be unique
        */
        if(generator->identical == 1)
            generator->valid = true;
    }

    puts("* * * * * * * * * * * * * * * * * * * * * * * *");
    puts("* VALID GENERATORS UNDER GF(2^8)              *");
    puts("* * * * * * * * * * * * * * * * * * * * * * * *");
    uint8_t count = 0;
    for(int i = 2; i < 256; i++)
    {
        struct Generator *generator = &generators.generators[i-2];
        if(generator->valid == true)
        {
            if(count % 16 == 0 && count > 0)
                puts("");
            count++;
            printf("%3d ", generator->g);
        }
    }
    puts("");
    puts("* * * * * * * * * * * * * * * * * * * * * * * *");
    printf("%d total valid generators.\n", count);
    puts("* * * * * * * * * * * * * * * * * * * * * * * *");

    puts("* * * * * * * * * * * * * * * * * * * * * * * *");
    puts("* POLYNOMIALS OF GENERATORS UNDER GF(2^8)     *");
    puts("* * * * * * * * * * * * * * * * * * * * * * * *");

    for(int i = 2; i < 256; i++)
    {
        struct Generator *generator = &generators.generators[i-2];
        if(generator->valid == true)
            show_polynomial(generator->g);
    }
    puts("* * * * * * * * * * * * * * * * * * * * * * * *");
    printf("%d total valid generators.\n", count);
    puts("* * * * * * * * * * * * * * * * * * * * * * * *");

    /*
     * Simplest Generator in GF(2^8) is 0x03
    */
    uint16_t poly = 1;

    puts("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *");
    puts("* EXPONENTIATION CHART OF 0xE5 IN GF(2^8) mod x^8 + x^4 + x^3 + x + 1             *");
    puts("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *");

    for(int i = 1; i <= 256; i++)
    {
        printf("%02X ", poly);
        poly = multiply_polynomial(poly, GENERATOR);
        poly = aes_polynomial_division(poly);
        if(i % 16 == 0)
            puts("");
    }

    puts("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *");

    puts("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *");
    puts("* LOGARITHM CHART OF 0xE5 IN GF(2^8) mod x^8 + x^4 + x^3 + x + 1                  *");
    puts("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *");

    poly = 1;
    uint8_t logarithm[16][16] = { 0 };  // 0xF * 0xF Elements
    for(int i = 1; i < 255; i++)
    {
        poly = multiply_polynomial(poly, GENERATOR);
        poly = aes_polynomial_division(poly);

        uint8_t x = poly & 0xF;
        uint8_t y = poly >> 4;
        logarithm[y][x] = i;
    }

    for(int y = 0; y < 16; y++)
    {
        for(int x = 0; x < 16; x++)
        {
            printf("%02X ", logarithm[y][x]);
        }
        puts("");
    }

    puts("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *");
    puts("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *");
    puts("* MULTIPLICATIVE INVERSE GF(2^8)                                                  *");
    puts("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *");
    for(int i = 0; i < 256; i++)
    {
        if(i % 16 == 0 && i > 0)
            puts("");
        if(i == 0)
        {
            printf("-- ");
            continue;
        }
        uint8_t x = i & 0xF;
        uint8_t y = i >> 4;
        uint8_t k = logarithm[y][x];
        k ^= 0xFF;  // substract 255 in GF(2^8)
        uint16_t poly = 1;
        for(int j = 0; j < k; j++)
        {
            poly = multiply_polynomial(poly, GENERATOR);
            poly = aes_polynomial_division(poly);
        }
        printf("%02X ", poly);
    }
    puts("");

    puts("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *");
    puts("* BYTE SUBSTITUTION TABLE IN AES");
    puts("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *");
    uint8_t bytesub[16][16] = { 0 };
    for(int i = 0; i < 256; i++)
    {
        uint8_t result = multiplicative_inverse(i);
        uint8_t x = i & 0xF;
        uint8_t y = i >> 4;
        bytesub[y][x] = result;
    }

    for(int i = 0; i < 256; i++)
    {
        if(i % 16 == 0 && i > 0)
            puts("");
        uint8_t x = i & 0xF;
        uint8_t y = i >> 4;
        printf("%02X ", bytesub[y][x]);
    }
    puts("");
    puts("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *");

    puts("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *");
    puts("* INVERSE BYTE SUBSTITUTION TABLE IN AES");
    puts("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *");
    uint8_t inversebytesub[16][16] = { 0 };
    for(int i = 0; i < 256; i++)
    {
        uint8_t x = i & 0xF;
        uint8_t y = i >> 4;
        uint8_t inverse = bytesub[y][x];
        x = inverse & 0xF;
        y = inverse >> 4;
        inversebytesub[y][x] = i;
    }

    for(int i = 0; i < 256; i++)
    {
        if(i % 16 == 0 && i > 0)
            puts("");
        uint8_t x = i & 0xF;
        uint8_t y = i >> 4;
        printf("%02X ", inversebytesub[y][x]);
    }

    puts("");

    puts("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *");
    puts("* MULTIPLIKATION TABLE FOR 0x02");
    puts("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *");
    uint8_t multi2[256] = { 0 };
    for(int i = 0; i < 256; i++)
    {
        multi2[i] = aes_polynomial_division(multiply_polynomial(2, i));
    }

    for(int i = 0; i < 256; i++)
    {
        if(i % 16 == 0 && i > 0)
            puts("");
        printf("0x%02X, ", multi2[i]);
    }
    puts("");
    puts("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *");

    puts("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *");
    puts("* MULTIPLIKATION TABLE FOR 0x03");
    puts("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *");
    uint8_t multi3[256] = { 0 };
    for(int i = 0; i < 256; i++)
    {
        multi3[i] = aes_polynomial_division(multiply_polynomial(3, i));
    }

    for(int i = 0; i < 256; i++)
    {
        if(i % 16 == 0 && i > 0)
            puts("");
        printf("0x%02X, ", multi3[i]);
    }
    puts("");
    puts("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *");

    uint8_t mixed_column_inputs[4] = { 0x0e, 0x0b, 0x0d, 0x09 };
    for(int i = 0; i < 4; i++)
    {
        puts("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *");
        printf("* MULTIPLIKATION TABLE FOR 0x%02X\n", mixed_column_inputs[i]);
        puts("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *");
        uint8_t multi[256] = { 0 };
        for(int j = 0; j < 256; j++)
        {
            multi[j] = aes_polynomial_division(multiply_polynomial(mixed_column_inputs[i], j));
        }
        for(int j = 0; j < 256; j++)
        {
            if(j % 16 == 0 && j > 0)
                puts("");
            printf("0x%02X, ", multi[j]);
        }
        puts("");
        puts("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *");
    }    

    puts("END OF PROGRAM");
    return 0;
}

