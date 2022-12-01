#include "mex.h"
#include "matrix.h"
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


void mexT04(int nlhs, mxArray *plhs[], 
                 int nrhs, const mxArray *prhs[]);
void mexRecalc(int nlhs, mxArray *plhs[], 
                 int nrhs, const mxArray *prhs[]);
void mexIGRFGSW(int nlhs, mxArray *plhs[], 
                 int nrhs, const mxArray *prhs[]);
void mex_cvtfun(int nlhs, mxArray *plhs[], 
                 int nrhs, const mxArray *prhs[], 
                 void(*cvtfun)(double*,double*,double*,double*,double*,double*,int*));
void mexFunction(int nlhs, mxArray *plhs[], 
                 int nrhs, const mxArray *prhs[]);
int mat2int(const mxArray *mat);
int matchecksize(int narray, mwSize ndim, const mwSize *sz, ...);
