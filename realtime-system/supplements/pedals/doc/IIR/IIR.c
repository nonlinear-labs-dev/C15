/*
 *
 *
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

typedef unsigned char	uint8;
typedef	long			int32;
typedef long long		int64;

void _ERROR(int line)
{
	fprintf(stderr, "\n! Runtime error in line %d\n", line);
	exit(3);
}
#define ERROR() _ERROR(__LINE__)





FILE *OpenInputFile(const char path[])
{
	FILE *in_file;
	in_file = fopen(path, "rb");
	if (in_file == NULL) ERROR();
	return in_file;
}

FILE *OpenOutputFile(const char path[])
{
	FILE *out_file;
	out_file = fopen(path, "wb");
	if (out_file == NULL) ERROR();
	return out_file;
}




int main(int argc, char *argv[]) 
{
	FILE			*in_file;
	FILE			*out_file;
	double		x = 0;
	double		x1 = 0;
	double		x2 = 0;
	double		y = 0;
	double		y1 = 0;
	double		y2 = 0;


// analog prototype
// H(s) = ( D*s^2 + E*s + F ) / (A*s^2 + B*s + C)
#define D (0.0)
#define E (0.0)
#define F (1.0)
#define A (1.0)
#define B (1.0)    // ==1/q
#define C (1.0)
#define OmegaC (0.5)  // w.r.t. Nyquist

double T = 2.0 * tan(OmegaC * M_PI_2);
double Arg = (4.0*A + 2.0*B*T + C*T*T);
double a2 = (4.0*A - 2.0*B*T + C*T*T) / Arg;
double a1 = (2.0*C*T*T - 8.0*A) / Arg;
//double a0 = 1.0;
double b2 = (4.0*D - 2.0*E*T + F*T*T) / Arg * C/F;
double b1 = (2.0*F*T*T - 8.0*D) / Arg * C/F;
double b0 = (4*D + F*T*T + 2.0*E*T) / Arg * C/F;
	
	if (argc != 3)
	{
        puts("Usage: iir in_file  out_file");
		puts(" in_file        : plain array of double");
		puts(" out_file       : plain array of double");
		return 1;
	}

	in_file = OpenInputFile(argv[1]);
	out_file = OpenOutputFile(argv[2]);
	while (fread(&x, sizeof(x), 1, in_file) == 1)
	{
    y = 0.6*(b0*x + b1*x1 + b2*x2 + a1*y1 + a2*y2);
    if (fwrite(&y, sizeof(y), 1, out_file) != 1) ERROR();
    x2 = x1;
    x1 = x;
    y2 = y1;
    y1 = y;
	}
	
	return 0;
}