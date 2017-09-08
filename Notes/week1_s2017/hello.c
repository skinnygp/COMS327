#include <stdio.h>
#include <math.h>
int main()
{
double w=1234.789;
double x=0;
double y;
double *z;
 z=(double *)&w;
 scanf("%f", &y);
 *z=sqrt(w);
 y=y*2.0;
 printf("y=%f, *z=%f\n", y, *z);
 y=y/x;
 w=y*z;
 return 0;
}
