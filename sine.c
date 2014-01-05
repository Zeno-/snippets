/* Very simple implementation to calcuate sine */

#include <stdio.h>
#include <math.h>

#define RUN_TEST

#define PI_DIV180   0.01745329251994329576923690768489

enum {
    nterms = 10 /* Max iterations for mysin() */
};


double rrduce(double x, double *sign);
double deg2rad(double x);
double mysin(double x);

/****************************************************************************/
#ifdef RUN_TEST
void testsin(void);


int main(void)
{
    testsin();

    return 0;
}

void testsin(void)
{
    int i;
    double esign;
    
    printf(" %6s %22s %22s %22s\n", "Angle", "Expect", "Got", "Error");
    
    for (i = 0; i <= 360; i += 1) {
        double angle = i/4.0;
        double r = mysin(angle);            
        double expected = sin(deg2rad(rrduce(angle, &esign)));
        expected *= esign;
        
        printf(" %6.2f %22.18f", angle, expected);
        printf(" %22.18f", r);
        printf(" %22.18f", r - expected);
        putchar('\n');
    }
}
#endif      /* END OF TESTS */
/****************************************************************************/

/* Constrain the degrees to 0 <= x < 360 */
double rrduce(double x, double *sign)
{
    if (x < 0)
        x = -x;         /* sin(-x) == -sin(x) */
    
    x = fmod(x, 360);
        
    if (x > 180) {       /* Mirror values > 180; those below the x-axis */
        x = x - 180; 
        *sign = -1;
    } else 
        *sign = 1;
    if (x > 90)         /* Reflect around x = 90 */
        x = 180 - x;
    
    return x;
}

double deg2rad(double x)
{
    return x * PI_DIV180;
}

/* Calculate sin(x). 'x' is the angle in **degrees**. 'nterms' is the number of
 * times to "refine" the calculated value for sine
 */
double mysin(double x)
{
    double sign;
    unsigned i;
    double term, sum;
    double xsq;
    
    x = deg2rad(rrduce(x, &sign));
    xsq = x*x;
         
    /* Sum successive terms of the sine Taylor series */
    term = sum = x;
    for(i = 0; i < nterms; i++) {
                
        term = -term / (10*i + 4*i*i + 6) * xsq;

#ifdef MYSINEEARLYEXIT              /* Makes no difference to final result */
        if (fabs(term) <= 1e-17) {
            break;
        }
#endif
        
        sum += term;
    }

    return sum * sign;
    
    /*
          Various rearrangement of terms to maximise precision
          (used in the sum loop above)
          
          term = - term * x*x / (2*i+2) / (2*i+3);  // Less precise than (1)...
          term = -term * x*x / ((2*i+2) * (2*i+3)); // Less precise than (1)...
            (1) term = -term / ((2*i+2) * (2*i+3)) * x*x;
            (2) term = -term / (10*i + 4*i*i + 6) * x*x;
            (3) term = -term * 1 / (10*i + 4*i*i + 6) * x*x;
                
          term = -term * 1 / (4*i*i + 10*i + 6) * x*x;    // same as (1)...
          term = 1 / (4*i*i + 10*i + 6) * x*x * (-term);  // a LOT less accurate
          term = -term * x * 1 / (4*i*i + 10*i + 6) * x;  // a bit less accurate
        
          Also experimented with calcuating series using fraction and
          2^(exp); c.f. frexp(). No improvement.
        */
}
