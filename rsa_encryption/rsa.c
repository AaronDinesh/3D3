#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <inttypes.h>
#include <stdbool.h>


#define MAX_VALUE 65535//largest number that can be represented by 16 bits

//function to check if a value is prime.
bool check_if_prime(int number){

	if(number == 0 || number == 1) return false;
    
	for(int i = 2; i < number; i++)
		if(number%i == 0) return false;

	return true;
  
}

//function to get a prime number in the range 5-65540
int getprime()
{
	int n;
	do
	{
		srand(time(NULL));
		n = rand() % MAX_VALUE + 5;
	}while  (check_if_prime(n) == false);
	
	return n;
}

//initialise the values of p and q (make sure they aren't equal)
void setprimes(int *p, int *q)
{
	
	*p = getprime();
	do{
		*q = getprime();
	}while(*p == *q);

		
}

//calculate φ(n) and n
void calculate_nz(int *p, int *q, int *n, int *z){
	*z = (*p - 1) * (*q - 1);
	*n = *p * *q;
}

//function to swap two values
void swap(int x, int y) {
    int temp = x;
    x = y;
    y = temp;
}

//function to return the greatest common divisor of two variables.
int gcd(int n1, int n2)
{
	int i, gcd;
	if (n1 > n2)
	{
		swap(n1, n2);
	}
	 for(i=1; i <= n1 && i <= n2; ++i)
    {
        // Checks if i is factor of both integers
        if(n1%i==0 && n2%i==0)
            gcd = i;
    }
	return gcd;
} 

//function to calculate e(should be less than n and coprime with z(φ(n)))
int generate_e(int *p, int *q, int e, int *z, int *n){
    
    for (e = 5; e < *z; e++) {
        if (gcd(e, *z) == 1 && e < *n) {
            return e;
        }
    }
    return -1;
}

//calculate d(loop until (e*d %(modulo) z) is 1)
int calculate_d(int e, int z){
	int x = e;
	int y = z;

	for(int i = 5; i < 999999999; i++){
		int k = (i * x)%y;
		if (k == 1){
			return i;
		}
	}
	return -1;
}

    //write private and public keys to text files so they are suitable for use by the encryption and decryption algorithms.
int main ()
{
	int e;
	int p, q;
	int n, z, d;
	
	setprimes(&p, &q);	
	calculate_nz(&p, &q, &n, &z);
	e = generate_e(&p, &q, e, &z, &n);
	d = calculate_d(e,z);

	printf("Public Key: (%d, %d)\n", n, e);
	printf("Private Key: (%d, %d)\n", n, d);

	FILE *outp = fopen("public_key.txt", "w");
	fprintf(outp, "%d %d", n, e);
	fclose(outp);

	outp = fopen("private_key.txt", "w");
	fprintf(outp, "%d %d", n, d);
	fclose(outp);

	outp = fopen("p_and_q.txt", "w");
    fprintf(outp, "%d %d", p, q);
    fclose(outp);
	return 0;
}
