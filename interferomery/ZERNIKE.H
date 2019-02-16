#ifndef ZERNIKE_FLG
#define ZERNIKE_FLG
struct polar {
	float r;
	float theta;
	};
	
#ifdef ZERNIKE
float	f_zern(float r,float theta,int m);
void	ldlt(float a[],float b[], int n);
float	*get_float1(int size);
#else
extern void	zernike(int *w, int size, 
	struct polar *r_t, float *z, int n);
#endif             

#endif
