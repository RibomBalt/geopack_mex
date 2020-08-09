call mex -c t04.f -compatibleArrayDims
call mex -c geopack_08.f -compatibleArrayDims
call mex -R2018a -c igrft04.c
call mex -R2018a igrft04.obj t04.obj geopack_08.obj -L"C:\Program Files (x86)\Intel\Composer XE 2013 SP1\compiler\lib\intel64"
pause