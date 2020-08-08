#include "mex.h"
#include "matrix.h"
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// COMMON BLOCKS in geopack_08
extern struct geopack1{
    double cb[34];
}GEOPACK1;
extern struct geopack2{
    double cb[315];
}GEOPACK2;

// SUBROUTINE T04_s (IOPT,PARMOD,PS,X,Y,Z,BX,BY,BZ)
extern void T04_S(int*,double*,double*,double*,double*,double*,double*,double*,double*);
// SUBROUTINE RECALC_08 (IYEAR,IDAY,IHOUR,MIN,ISEC,VGSEX,VGSEY,VGSEZ)
extern void RECALC_08(int*,int*,int*,int*,int*,double*,double*,double*);
// SUBROUTINE IGRF_GSW_08 (XGSW,YGSW,ZGSW,HXGSW,HYGSW,HZGSW)
extern IGRF_GSW_08(double*,double*,double*,double*,double*,double*);

void mexT04(int nlhs, mxArray *plhs[], 
                 int nrhs, const mxArray *prhs[]){
    // declaration
    int iopt = 0;
    size_t i;
    mxArray *oBX,*oBY,*oBZ;
    mxDouble *parmod,*ps,*x,*y,*z,*bx,*by,*bz;
    // check nlhs/nrhs
    if(nrhs!=5) {
        mexErrMsgIdAndTxt("mex:igrft04","T04: 5 inputs required: PARMOD,PS,X,Y,Z");
    }
    if(nlhs>3) {
        mexErrMsgIdAndTxt("mex:igrft04","T04: too many outputs. Max 3.");
    }
    // check parmod
    if(mxGetNumberOfElements(prhs[0]) != 10){
        mexErrMsgIdAndTxt("mex:igrft04","T04: len(Parmod) != 10");
    }
    // check XYZ dimensions
    mwSize dim_in = mxGetNumberOfDimensions(prhs[2]);
    if(dim_in != mxGetNumberOfDimensions(prhs[3]) || dim_in != mxGetNumberOfDimensions(prhs[4])){
        mexErrMsgIdAndTxt("mex:igrft04","T04: X,Y,Z dimension dismatch");
    }
    const mwSize *sx = mxGetDimensions(prhs[2]);
    const mwSize *sy = mxGetDimensions(prhs[3]);
    const mwSize *sz = mxGetDimensions(prhs[4]);
    const size_t totnum = mxGetNumberOfElements(prhs[2]);
    for(i=0;i<dim_in;i++){
        if(sx[i]!=sy[i] || sx[i]!=sz[i]){
            mexErrMsgIdAndTxt("mex:igrft04","T04: X,Y,Z dimension dismatch");
        }
    }
    // init BXBYBZ, to be assigned to plhs later
    oBX = mxDuplicateArray(prhs[2]);
    oBY = mxDuplicateArray(prhs[2]);
    oBZ = mxDuplicateArray(prhs[2]);
    // link in-data* to mxarray*
    parmod = mxGetDoubles(prhs[0]);
    ps = mxGetDoubles(prhs[1]);
    x = mxGetDoubles(prhs[2]);
    y = mxGetDoubles(prhs[3]);
    z = mxGetDoubles(prhs[4]);
    bx = mxGetDoubles(oBX);
    by = mxGetDoubles(oBY);
    bz = mxGetDoubles(oBZ);
    // CALCULATION
    for(i=0;i<totnum;i++){
        T04_S(&iopt, parmod, ps, x+i, y+i, z+i, bx+i, by+i, bz+i);
    }
    // return process
    switch(nlhs){
        case 2:
            plhs[0] = oBX;
            plhs[1] = oBY;
            mxDestroyArray(oBZ);
            break;
        case 3:
            plhs[0] = oBX;
            plhs[1] = oBY;
            plhs[2] = oBZ;
            break;
    default: // 1 & 0
        plhs[0] = oBX;
        mxDestroyArray(oBY);
        mxDestroyArray(oBZ);
    }
}

void mexRecalc(int nlhs, mxArray *plhs[], 
                 int nrhs, const mxArray *prhs[]){
    if(nrhs!=8){
        mexErrMsgIdAndTxt("mex:igrft04","Recalc: Too few inputs => 8");
    }
    if(nlhs!=0){
        mexErrMsgIdAndTxt("mex:igrft04","Recalc: Too much outputs => 0");
    }
    int yy,dd,hh,mm,ss;
    yy = mat2int(prhs[0]);
    dd = mat2int(prhs[1]);
    hh = mat2int(prhs[2]);
    mm = mat2int(prhs[3]);
    ss = mat2int(prhs[4]);
    double *vx,*vy,*vz,ansout=0.;
    vx = mxGetDoubles(prhs[5]);
    vy = mxGetDoubles(prhs[6]);
    vz = mxGetDoubles(prhs[7]);
    
    RECALC_08(&yy,&dd,&hh,&mm,&ss,vx,vy,vz);
    plhs[0] = mxCreateDoubleScalar(ansout);
}

void mexIGRFGSW(int nlhs, mxArray *plhs[], 
                 int nrhs, const mxArray *prhs[]){
    int i;
    if(nrhs!=3){
        mexErrMsgIdAndTxt("mex:igrft04","Recalc: Too few inputs => 3");
    }
    if(nlhs>3){
        mexErrMsgIdAndTxt("mex:igrft04","Recalc: num of outputs => 1-3");
    }
    // check XYZ dimensions
    mwSize dim_in = mxGetNumberOfDimensions(prhs[0]);
    const mwSize *sz = mxGetDimensions(prhs[0]);
    const size_t totnum = mxGetNumberOfElements(prhs[0]);
    if(!matchecksize(2,dim_in,sz,prhs[1],prhs[2])){
        mexErrMsgIdAndTxt("mex:igrft04","Recalc: X,Y,Z: dimension mismatch");
    }
    mxArray *oBX,*oBY,*oBZ;
    oBX = mxDuplicateArray(prhs[0]);
    oBY = mxDuplicateArray(prhs[0]);
    oBZ = mxDuplicateArray(prhs[0]);
    // get data
    double *x,*y,*z,*bx,*by,*bz;
    x = mxGetDoubles(prhs[0]);
    y = mxGetDoubles(prhs[1]);
    z = mxGetDoubles(prhs[2]);
    bx = mxGetDoubles(oBX);
    by = mxGetDoubles(oBY);
    bz = mxGetDoubles(oBZ);
    //CALCULATION
    for(i=0;i<totnum;i++){
        IGRF_GSW_08(x+i, y+i, z+i, bx+i, by+i, bz+i);
    }
    //arrange output
    switch(nlhs){
        case 2:
            plhs[0] = oBX;
            plhs[1] = oBY;
            mxDestroyArray(oBZ);
            break;
        case 3:
            plhs[0] = oBX;
            plhs[1] = oBY;
            plhs[2] = oBZ;
            break;
    default: // 1 & 0
        plhs[0] = oBX;
        mxDestroyArray(oBY);
        mxDestroyArray(oBZ);
    }
}


void mexFunction(int nlhs, mxArray *plhs[], 
                 int nrhs, const mxArray *prhs[]){
    // first input is mode number
    if(nrhs<2){
        mexErrMsgIdAndTxt("mex:igrft04","mexMain: few params");
    }
    const int mode = mat2int(prhs[0]); 
    // execution
    switch(mode){
        case 1:
            mexT04(nlhs,plhs,nrhs-1,prhs+1);
            break;
        case 2:
            mexRecalc(nlhs,plhs,nrhs-1,prhs+1);
            break;
        case 3:
            mexIGRFGSW(nlhs,plhs,nrhs-1,prhs+1);
            break;
        default:
        mexErrMsgIdAndTxt("mex:igrft04","mexMain: wrong mode");
    }
}

int mat2int(const mxArray *mat){
    // convenient get int args from double/int64 mxArray
    void *modeptr;
    int mode;
    if(mxIsInt64(mat)){
        modeptr = mxGetInt64s(mat);
        mode = (int)*(mxInt64*)modeptr;
    }else if(mxIsDouble(mat)){
        modeptr = mxGetDoubles(mat);
        mode = (int)*(mxDouble*)modeptr;
    }else if(mxIsSingle(mat)){
        modeptr = mxGetSingles(mat);
    mode = (int)*(mxSingle*)modeptr;
    }
    else{
        mexErrMsgIdAndTxt("mex:igrft04","mat2int: cannot convert to int");
    }
    return mode;
}

int matchecksize(int narray, mwSize ndim, mwSize *sz, ...){
    // fast routine to check whether multiple matrices have same size
    va_list args;
    va_start(args, sz);
    mxArray *mat;
    int i=0,j=0;
    mwSize ndim_,*sz_;
    for(i=0;i<narray;i++){
        mat = va_arg(args, mxArray*);
        ndim_ = mxGetNumberOfDimensions(mat);
        if(ndim != ndim_){
            va_end(args);
            return 0;
        }
        sz_ = mxGetDimensions(mat);
        for(j=0;j<ndim;j++){
            if(sz[j]!=sz_[j]){
                va_end(args);
                return 0;
            }
        }
    }
    va_end(args);
    return 1;
}