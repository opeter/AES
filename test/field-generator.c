/*
    A FIELD GENERATOR

*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define AES_POLYNOMIAL 0b1000110110000000

uint64_t
power(uint64_t factor, uint8_t exponent)
{
    uint64_t result = factor;
    for(int i = 1; i < exponent; i++)
        result *= factor;
    return result;}

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


int
main(void)
{
    puts("F I E L D  G E N E R A T O R");

    // A generator is an element whose successive powers
    // take on every element except the zero

    /*
        list the generators in Zm, m in {2..13}
    */
    for(int m = 2; m <= 13; m++)
    {
        printf("Z(%d)\n", m);
        for(int generator = 6; generator > 1; generator--)
        {
            for(int exp = 2; exp < 13; exp++)
            {
                printf("%d^%d mod %d = %d\n", generator, exp, m, (int) (power(generator, exp) % m));
            }
            puts("");
        }
    }
    /*
     * Simplest Generator in GF(2^8) is 0x03
    */
    uint16_t poly = 1;
    uint16_t generator = 0x03;

    /*

        Generator: 0xe5
               | 0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f|
            ---|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|---
            00 |01 e5 4c b5 fb 9f fc 12 03 34 d4 c4 16 ba 1f 36| 00
            10 |05 5c 67 57 3a d5 21 5a 0f e4 a9 f9 4e 64 63 ee| 10
            20 |11 37 e0 10 d2 ac a5 29 33 59 3b 30 6d ef f4 7b| 20
            30 |55 eb 4d 50 b7 2a 07 8d ff 26 d7 f0 c2 7e 09 8c| 30
            40 |1a 6a 62 0b 5d 82 1b 8f 2e be a6 1d e7 9d 2d 8a| 40
            50 |72 d9 f1 27 32 bc 77 85 96 70 08 69 56 df 99 94| 50
            60 |a1 90 18 bb fa 7a b0 a7 f8 ab 28 d6 15 8e cb f2| 60
            70 |13 e6 78 61 3f 89 46 0d 35 31 88 a3 41 80 ca 17| 70
            80 |5f 53 83 fe c3 9b 45 39 e1 f5 9e 19 5e b6 cf 4b| 80
            90 |38 04 b9 2b e2 c1 4a dd 48 0c d0 7d 3d 58 de 7c| 90
            a0 |d8 14 6b 87 47 e8 79 84 73 3c bd 92 c9 23 8b 97| a0 
            b0 |95 44 dc ad 40 65 86 a2 a4 cc 7f ec c0 af 91 fd| b0
            c0 |f7 4f 81 2f 5b ea a8 1c 02 d1 98 71 ed 25 e3 24| c0
            d0 |06 68 b3 93 2c 6f 3e 6c 0a b8 ce ae 74 b1 42 b4| d0
            e0 |1e d3 49 e9 9c c8 c6 c7 22 6e db 20 bf 43 51 52| e0
            f0 |66 b2 76 60 da c5 f3 f6 aa cd 9a a0 75 54 0e 01| f0 
            ---|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|---
               | 0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f|
*/

    puts("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *");
    puts("* EXPONENTIATION CHART OF 0xE5 IN GF(2^8) mod x^8 + x^4 + x^3 + x + 1             *");
    puts("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *");
    printf("     ");
    for(int i = 0; i < 16; i++)
        printf("0x%X  ", i);
    puts("");

    for(int i = 1; i <= 256; i++)
    {
        if(i % 16 == 1)
            printf("0x%X ", i / 16);
        printf("  %2X ", poly);
        poly = multiply_polynomial(poly, generator);
        poly = aes_polynomial_division(poly);
        if(i % 16 == 0)
            puts("");
    }

    puts("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *");

    /*
               | 0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f|
            ---|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|---
            00 |-- 00 c8 08 91 10 d0 36 5a 3e d8 43 99 77 fe 18| 00
            10 |23 20 07 70 a1 6c 0c 7f 62 8b 40 46 c7 4b e0 0e| 10
            20 |eb 16 e8 ad cf cd 39 53 6a 27 35 93 d4 4e 48 c3| 20
            30 |2b 79 54 28 09 78 0f 21 90 87 14 2a a9 9c d6 74| 30
            40 |b4 7c de ed b1 86 76 a4 98 e2 96 8f 02 32 1c c1| 40
            50 |33 ee ef 81 fd 30 5c 13 9d 29 17 c4 11 44 8c 80| 50
            60 |f3 73 42 1e 1d b5 f0 12 d1 5b 41 a2 d7 2c e9 d5| 60
            70 |59 cb 50 a8 dc fc f2 56 72 a6 65 2f 9f 9b 3d ba| 70
            80 |7d c2 45 82 a7 57 b6 a3 7a 75 4f ae 3f 37 6d 47| 80
            90 |61 be ab d3 5f b0 58 af ca 5e fa 85 e4 4d 8a 05| 90
            a0 |fb 60 b7 7b b8 26 4a 67 c6 1a f8 69 25 b3 db bd| a0
            b0 |66 dd f1 d2 df 03 8d 34 d9 92 0d 63 55 aa 49 ec| b0
            c0 |bc 95 3c 84 0b f5 e6 e7 e5 ac 7e 6e b9 f9 da 8e| c0
            d0 |9a c9 24 e1 0a 15 6b 3a a0 51 f4 ea b2 97 9e 5d| d0
            e0 |22 88 94 ce 19 01 71 4c a5 e3 c5 31 bb cc 1f 2d| e0
            f0 |3b 52 6f f6 2e 89 f7 c0 68 1b 64 04 06 bf 83 38| f0 
            ---|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|--|---
               | 0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f|
    */

    puts("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *");
    puts("* LOGARITHM CHART OF 0xE5 IN GF(2^8) mod x^8 + x^4 + x^3 + x + 1                  *");
    puts("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *");

    poly = 1;
    uint8_t logarithm[16][16] = { 0 };  // 0xF * 0xF Elements
    for(int i = 1; i < 255; i++)
    {
        poly = multiply_polynomial(poly, generator);
        poly = aes_polynomial_division(poly);

        uint8_t x = poly & 0xF;
        uint8_t y = poly >> 4;
        logarithm[y][x] = i;
    }

    for(int y = 0; y < 16; y++)
    {
        for(int x = 0; x < 16; x++)
        {
            printf("%2X ", logarithm[y][x]);
        }
        puts("");
    }

    generator = 0xe5;
    puts("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *");
    puts("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *");
    puts("* MULTIPLICATIVE INVERSE GF(2^8)                                                  *");
    puts("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *");
    for(int i = 0; i < 256; i++)
    {
        if(i % 16 == 0)
            puts("");
        if(i == 0)
        {
            printf("-- ");
            continue;
        }
        uint16_t log = multiply_polynomial(i, generator);
        log = aes_polynomial_division(log);
        log ^= 255;
        uint8_t x = log & 0xF;
        uint8_t y = log >> 4;
        uint8_t mi = logarithm[y][x];
        printf("%2X ", mi);
    }
    puts("");



    puts("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *");



    puts("END OF PROGRAM");
    return 0;
}

