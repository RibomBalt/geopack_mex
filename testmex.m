x = 2:0.1:9;
y = 2:0.1:9;
[X,Y] = meshgrid(x,y);
Z = zeros(size(X));
% wrapper
t04 = @(parmod,ps,x,y,z)igrft04(1,parmod,ps,x,y,z);
recalc = @(y,d,h,m,s,vx,vy,vz)igrft04(2,y,d,h,m,s,vx,vy,vz);
igrfgsw = @(x,y,z)igrft04(3,x,y,z);
% run at 2 given params
recalc(2013,34,1,50,0,-445.4,-0.4,-16.2);
[tx1,ty1,tz1] = t04([1.34 -10 -3.25 -1.02 0.22 0.17 0.33 0.11 0.30 0.37], -0.399, X, Y, Z);
[ix1,ix2,ix3] = igrfgsw(X, Y, Z);
ixx1 = igrfgsw(X, Y, Z); % test 1,2 outputs
[ixxx1,ixxx2] = igrfgsw(X, Y, Z);

recalc(2013,34,2,50,0,-426.8,-7.9,9.7); % test COMMON with another params
[tx2,ty2,ty3] = t04([1.21 -12 -3.15 -0.15 0.23 0.20 0.32 0.13 0.34 0.39], -0.4293, X, Y, Z);
[ix1,ix2,ix3] = igrfgsw(X, Y, Z);

clear igrft04;