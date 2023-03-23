#include "stdlib.h"
#include "stdio.h"

unsigned long long int mod_power(int b, int p, int m) // calculate c = m^e % n
{
        unsigned long long int result;
        int i;
        result = 1;

        for (i = 0; i < p; i++)
        {
                result = (result * b) % m;
        }
        return result;
}

int main (int argc, char *argv[])
{
        int e, n, m;
        unsigned long long int c;

        FILE *inp = fopen("public_key.txt", "r");
        fscanf(inp, "%d %d", &n, &e);
        fclose(inp);

	printf("cipher c = ");

	int i;
	FILE *outp = fopen("cipher.txt", "w");

	for (i = 0; argv[1][i]!= '\0'; i++)
	{
        	c = mod_power(argv[1][i],e,n);
		printf("%llu ", c);
		fprintf(outp, "%llu\n", c);
	}

        printf("\n");
        fclose(outp);

        return 0;
}