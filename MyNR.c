#include <stdio.h>

#define EX4_EPSILON 0.0001
#define DELTA 0.000001
#define MAX_ITERATIONS 1000

double MyNR(double (*f)(double), double (*g)(double), double start){
	double prev = start;
	int i;
	double next;
	double diff;
	double fValue;
	double gValue;
	for(i=0; i<MAX_ITERATIONS; i++){
		fValue = (*f)(prev);
		gValue = (*g)(prev);
		if(0 == gValue){
			gValue = DELTA;
		}
		next = prev - (fValue/gValue);
		diff = next - prev;
		if(diff < 0){
			diff = (-1) * diff;
		}
		if(diff < EX4_EPSILON){
			return next;
		}
		prev = next;
	}
	return next;
}
