set MATLABROOT=C:\Program Files\MATLAB\R2022a
set MW_MINGW64_LOC=C:\ProgramData\MATLAB\SupportPackages\R2022a\3P.instrset\mingw_w64.instrset

set GFORT="%MW_MINGW64_LOC%\bin\gfortran.exe"
set CC="%MW_MINGW64_LOC%\bin\gcc.exe"
set MEX="%MATLABROOT%\bin\mex.bat"
set MATLAB_INC="%MATLABROOT%\extern\include"
set CC_LIB="%MW_MINGW64_LOC%\lib\gcc\x86_64-w64-mingw32\6.3.0"

call %GFORT% -O3 -o "tmp\geopack.o" -c "src\Geopack-2008_dp.for"
call %GFORT% -O3 -o "tmp\ts04.o" -c "src\TS04c.for"
call %MEX% -R2018a -O -outdir "tmp" -c "src\igrft04.c"
call %MEX% -R2018a -outdir "." -output igrft04 "tmp\geopack.o" "tmp\ts04.o" "tmp\igrft04.obj" -L%CC_LIB% -lgfortran -lquadmath