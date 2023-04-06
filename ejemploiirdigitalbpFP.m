clc;
close all;
clear all;

%Frecuencia de muestreo
f_s=44.1e3;

f_c=10000;
BW=3000;


f_p1=f_c-BW/2;
f_p2=f_c+BW/2;


delta=1000;

f_s1=f_p1+delta;
f_s2=f_p2-delta;



%Frecuencia de corte de la banda de rechazo del pasa-bajo
Omega_s2=2*pi*f_s2/f_s;

%Frecuencia de corte de la banda de paso del pasa-bajo
Omega_p2=2*pi*f_p2/f_s;





Omega_p1=2*pi*f_p1/f_s;



Omega_s1=2*pi*f_s1/f_s;

%Atenuación en la banda de paso y rechazo
A_p=3;
A_s=30;
%Diseño digital directo del filtro pasa-bajo
[N, Omega_c] = buttord([Omega_p1 Omega_p2]/pi, [Omega_s1 Omega_s2]/pi, A_p, A_s);
[b_d, a_d]=butter(N,Omega_c);



Nbits=16;
max_c=max([abs(b_d) abs(a_d)]);
Q=floor(Nbits-1-log2(max_c));

b_d_Q=int16(round(b_d*2^Q));
a_d_Q=int16(round(a_d*2^Q));

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
T=5e-3;
t=0:(1/f_s):T;
f=9400;

x=cos(2*pi*f*t);
x_Q=int16(2^Q*x);

    
      %y_Q=myFilter2FP(b_d_Q(:),a_d_Q(:),x_Q(:),int8(Q));
    
      y_Q_1=irr3_fp(b_d_Q(:),a_d_Q(:),x_Q(:),int8(Q));
    
      %y=myFilter2(b_d(:),a_d(:),x(:));
   
      
     

figure;
plot(t,x_Q);
hold on;
plot(t,y_Q,'r');
grid on;figure;
plot(t,x);
hold on;
plot(t,y,'r');
grid on;

[H, Omega]=freqz(b_d,a_d,65536);
[H_Q, Omega]=freqz(double(b_d_Q),double(a_d_Q),65536);
figure;
plot(Omega/(2*pi)*f_s,(abs(H)));
hold on;
plot(Omega/(2*pi)*f_s,(abs(H_Q)),'r');
xlabel('f [Hz]');
ylabel('|H(\Omega)|_{dB}');
grid on;


handle = fopen('coefs_fp16.h', 'wt');
fprintf(handle, '#define N %d\n',length(b_d));
fprintf(handle, '#define M %d\n',length(a_d));
fprintf(handle, '#define m %d\n\n',Q);
fprintf(handle, 'const short b_k[N] = {\n');
fprintf(handle, ' %d,\n', int16(b_d*2^Q));
fprintf(handle, '};\n\n');
fprintf(handle, 'const short a_k[M] = {\n');
fprintf(handle, ' %d,\n', int16(a_d*2^Q));
fprintf(handle, '};\n\n');
fclose(handle);





