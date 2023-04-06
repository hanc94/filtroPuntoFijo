#include "mex.h"
#include <stdint.h>

//Tipo de dato para los coeficientes
typedef int16_t coeff_t;
//Tipo de dato para las líneas de retardos, considera 16 de bits de guarda para la ganancia de las etapas
typedef int32_t delay_t;
//Tipo de dato para los acumuladores antes de correr la coma
typedef int64_t acc_t;

delay_t iir2_fp(coeff_t,delay_t*,const coeff_t*,const coeff_t*,int8_t,int8_t);

delay_t iir2_fp(coeff_t x_n,delay_t *w,const coeff_t *b,const coeff_t *a,int8_t n,int8_t q){

acc_t y_n;
acc_t w_n;
int8_t k;

w_n=(acc_t)(x_n<<q);

        /*Cálculo de w[n]*/
        for(k=n-1; k>=1; k--)
                w_n -=((acc_t)a[k]*(acc_t)w[k]);
        /*w_n actual*/
        w[0]=w_n>>q;
        	y_n=0;
        /*Cálculo de la rama de salida*/
        for(k=n-1; k>=0; k--) {
                y_n +=(acc_t)b[k]*(acc_t)w[k];
                w[k] = w[k-1];
		}
return (delay_t)(y_n>>q);
}


	/* Función pasarela */
	void mexFunction( int nlhs, mxArray *plhs[],
		          int nrhs, const mxArray *prhs[])
	{
	   
	    //Variables locales
	    int nx,n;
	    coeff_t *x,*b,*a;
	    delay_t *y;
	    int8_t p,q;

	    /* Verifica que hayan tres argumentos de entrada */
	    if(nrhs!=4) {
		mexErrMsgIdAndTxt("myConv:nrhs","Se requieren cuatro entradas.");
	    }
	   /*Verifica que haya un argumento de salida*/
	    if(nlhs!=1) {
		mexErrMsgIdAndTxt("myConv:nlhs","Se requiere una salida");
	    }
	   
		 
	   
	    /* Punteros a la señal de entrada y respuesta al impulso*/
	    b = (coeff_t *)mxGetData(prhs[0]);
	    a = (coeff_t *)mxGetData(prhs[1]);    
	    x = (coeff_t *)mxGetData(prhs[2]);
	    q=  (int8_t)mxGetScalar(prhs[3]);

	    

	    /* Tamaños de la señal de entrada y respuesta al impulso (filas) */
	    nx = mxGetM(prhs[2]);
	    p = mxGetM(prhs[0]);


	    
	    /*Línea de retardos del filtro*/
	    delay_t w[p]={0};


	      /* Crea la matriz de salida */	
	     plhs[0] = mxCreateNumericMatrix((mwSize)nx, 1, mxINT32_CLASS, mxREAL);

	    /* Puntero a la matriz de salida */
	    y = (delay_t*)mxGetData(plhs[0]);

	    /* Llama a la función convolución */

	    for(n=0;n<nx;n++)
	    	y[n]= iir2_fp(x[n], w, b, a, p,q);


	}
