/* deepixel.h
 * deePixel lib - header file
 * by Robert Babin
 *
 * High Precision Numbers - Fixed Point Numbers
 * Calculating the Mandelbrot sequence with these High Precision Numbers
 */

#ifdef DEEPIXEL
#define DEEPIXEL

int check_sizes();

int test_hpn_add(double,double);
int test_hpn_mult(double,double);

int calc_escape_iteration(int,double,double,int,double,double,int,int,int);

int test_next_iteration(double,double,double,double);
void test_if_stdio(const char*)

#endif
