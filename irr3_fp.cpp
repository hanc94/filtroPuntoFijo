#include "mex.h"
#include <stdint.h>
//Tipo de dato para los coeficientes
typedef int16_t coeff_t;
//Tipo de dato para las líneas de retardos, considera 16 de bits de guarda para la ganancia de las etapas
typedef int32_t delay_t;
//Tipo de dato para los acumuladores antes de correr la coma
typedef int64_t acc_t;

delay_t iir3_fp(coeff_t,delay_t*,const coeff_t*,const coeff_t*,int8_t,int8_t);

delay_t iir3_fp(coeff_t x_n,delay_t *v, const coeff_t *b, const coeff_t *a, int8_t n,int8_t q){
	int8_t k;
/*Cálculo de la salida una vez se lee el ADC*/
	acc_t y_n;
	
	y_n =((acc_t)b[0])*((acc_t)x_n)+((acc_t)v[0]<<q);
/*Cálculo de v[n]*/
	for (k = 0; k < n - 2; k++)
		v[k]=(((acc_t)b[k+1])*((acc_t)x_n))-(((acc_t)a[k+1])*((acc_t)y_n) + ((acc_t)v[k+1]<<q))>>q;
/*Cálculo de v_{n-1}*/
	v[n-2]=((acc_t)b[n-1])*((acc_t)x_n)-((acc_t)a[n-1])*((acc_t)y_n)>>q;

	return (acc_t)(y_n>>q);
}

//función pasarela
void mexFunction( int nlhs, mxArray *plhs[],int nrhs, const mxArray *prhs[]){
	
	coeff_t *xn,*b,*a;
	acc_t *y;
	int8_t p,nx,q;
	if(nrhs!=4) {
		mexErrMsgIdAndTxt("irr3:nrhs","Se requieren tres entradas.");
	}
/*Verifica que haya un argumento de salida*/
	if(nlhs!=1) {
		mexErrMsgIdAndTxt("irr3:nlhs","Se requiere una salida");
	}
	
	p=mxGetM(prhs[0]); // longitud de a y b
	nx=mxGetM(prhs[2]); // longitud de x

// Obtencion de los punteros

	xn=(coeff_t *)mxGetData(prhs[2]);	
	b=(coeff_t *)mxGetData(prhs[0]);
	a=(coeff_t *)mxGetData(prhs[1]);
	q=(int8_t)mxGetScalar(prhs[3]);
/* Crea la matriz de salida */
	plhs[0] = mxCreateNumericMatrix((mwSize)nx,1,mxINT32_CLASS,mxREAL);
/* Puntero a la matriz de salida */
	y = (acc_t*)mxGetPr(plhs[0]);

	delay_t v[p-1]={0};

	for (int i=0;i<nx;i++){
		y[i]=iir3_fp(xn[i],v,b,a,(mwSize)p,q);
	}

	
}

