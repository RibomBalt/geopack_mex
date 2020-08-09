t04 = @(parmod,ps,x,y,z)igrft04(1,parmod,ps,x,y,z);
recalc = @(y,d,h,m,s,vx,vy,vz)igrft04(2,y,d,h,m,s,vx,vy,vz);
igrfgsw = @(x,y,z)igrft04(3,x,y,z);

smgsw = @(x,y,z,d)igrft04(4,x,y,z,d);
magsm = @(x,y,z,d)igrft04(5,x,y,z,d);
geigeo = @(x,y,z,d)igrft04(6,x,y,z,d);
geomag = @(x,y,z,d)igrft04(7,x,y,z,d);
gswgse = @(x,y,z,d)igrft04(8,x,y,z,d);
geogsw = @(x,y,z,d)igrft04(9,x,y,z,d);