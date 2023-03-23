#include "stdlib.h"
#include "stdio.h"

unsigned long long int mod_power(unsigned long long int b, int p, int m) //m = c^d % n
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

int inverse(int a, int mod)     //get the modular inverse
{                               
        int aprev, iprev, i = 1, atemp, itemp;

        aprev = mod, iprev = mod;
        while (a != 1)
        {
                atemp = a;
                itemp = i;
                a = aprev - aprev / atemp * a;
                i = iprev - aprev / atemp * i;
                aprev = atemp;
                iprev = itemp;
                while (i < 0)
                        i += mod;
        }

        return i;
}

int main ()
{
        unsigned long long int c, dmod_P, dmod_Q, mp, mq;
        int p, q, d, n, h, m, mod_inv, mp_mq;
        FILE *input;

        input = fopen("private_key.txt", "r");
        fscanf(input, "%d %d", &n, &d);
        fclose(input);

        input = fopen("p_and_q.txt", "r");
        fscanf(input, "%d %d", &p, &q);
        fclose(input);

	printf("message: ");
	input = fopen("cipher.txt", "r");

	while (fscanf(input, "%llu", &c) != EOF)//while not end of file.
	{
        	dmod_P = d % (p - 1);
        	dmod_Q = d % (q - 1);

        	mod_inv = inverse(q,p);

        	mp = mod_power(c,dmod_P,p);
        	mq = mod_power(c,dmod_Q,q);
                
        	mp_mq = mp - mq;
        	if (mp_mq < 0)
                	mp_mq += p;
       		h = (mod_inv * mp_mq) % p;
        	m = mq + h * q;
		printf("%c", m);
	}

	fclose(input);
	printf("\n");
        

        return 0;
}