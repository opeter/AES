/*
    A FIELD GENERATOR

*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

uint64_t
power(uint64_t factor, uint8_t exponent)
{
    return factor << (exponent-1);
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
    puts("END OF PROGRAM");
    return 0;
}

