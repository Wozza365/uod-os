#include <math.h>
#define PI 3.14159265


float cos(float x){
	if( x < 0.0f ) 
		x = -x;
	while( PI < x )
		x -= PI*2;
	return 1.0f - (x*x/2.0f)*( 1.0f - (x*x/12.0f) * ( 1.0f - (x*x/30.0f) * (1.0f - x*x/56.0f )));
}

float sin(float x){return cos(x-(PI/2));}

int abs(int a) {
	if (a < 0) {
		return -a;
	} else {
		return a;
	}
}