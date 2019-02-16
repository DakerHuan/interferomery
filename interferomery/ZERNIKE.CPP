/*subroutine to obtain the coefficents of zernike polymids
void	zernike(int *w, int size, struct polar  *r_t, float *z, int n)
	*w--------the wavefront
	size------the dimension of wavefront
	*r_t-------------the polar coordination
	*z-------------zernike coefficents
	n--------------the number of zernike coefficents                 */
#include	"stdafx.h"
#include	<math.h>
//#include    "wavfront.h"

#define ZERNIKE
#include	"zernike.h"

void	zernike(int *w, int size, 
	struct polar *r_t, float *z, int n)
{
	float	*a,*b;
	register int i,j;
	int k,l,nn;
	float x,y;

	a=get_float1(n*(n+1)/2*sizeof(float));
	b=get_float1(n*sizeof(float));

	/*generate the equation---matrix a & z*/
	nn=n*(n+1)/2;
	for(i=0;i<nn;i++) *(a+i)=0.0;
	for(i=0;i<n;i++) *(z+i)=0.0;
	for(i=0;i<size;i++)
	   for(j=0;j<size;j++)
	      if(*(w+i*size+j)!= -2147483647){
	         x=(r_t+i*size+j)->r;
	         y=(r_t+i*size+j)->theta;
	         for(k=0;k<n;k++) {
			*(b+k)=f_zern(x,y,k+1);
			(*(z+k))+=(*(b+k))*(*(w+i*size+j));
			}
	         for(k=0;k<n;k++)
		  for(l=k;l<n;l++) {
			int ii;
			ii=(l+1)*l/2+k;
			(*(a+ii))+=(*(b+k))*(*(b+l));
			}
	         }


	/*solve the equation*/
	ldlt(a,z,n);

	/*free space*/
	delete [] a;
	delete [] b;
	}
float *get_float1(int size)
{
	float* p = new float[size];
	if(!p){
		//AfxMessageBox("allocation error-aborting");
		exit(1);
		}
	return p;
	}

float	f_zern(float r,float theta,int m)
{
	float	value;
	switch(m) {
	  /*n=0*/
	  case 1: value=1.0;
		break;

	  /*n=1*/
	  case 2: value=r*(float)cos(theta);
		break;
	  case 3: value=r*(float)sin(theta);
		break;
	  case 4: value=((float)2.0*r*r-(float)1.0);
		break;

	  /*n=2*/
	  case 5: value=r*r*(float)cos((float)2.0*theta);
		break;
	  case 6: value=r*r*(float)sin((float)2.0*theta);
		break;
	  case 7: value=((float)3.0*r*r-(float)2.0)*r*(float)cos(theta);
		break;
	  case 8: value=((float)3.0*r*r-(float)2.0)*r*(float)sin(theta);
		break;
	  case 9: value=(float)6.0*r*r*r*r-(float)6.0*r*r+(float)1.0;
		break;

	  /*n=3*/
	  case 10: value=r*r*r*(float)cos(3.0*theta);
		   break;
	  case 11: value=r*r*r*(float)sin(3.0*theta);
		   break;
	  case 12: value=((float)4.0*r*r*r*r-(float)3.0*r*r)*(float)cos((float)2.0*theta);
		   break;
	  case 13: value=((float)4.0*r*r*r*r-(float)3.0*r*r)*(float)sin((float)2.0*theta);
		   break;
	  case 14: value=(10*(float)pow(r,5)-12*(float)pow(r,3)+3*r)*(float)cos(theta);
		   break;
	  case 15: value=(10*(float)pow(r,5)-12*(float)pow(r,3)+3*r)*(float)sin(theta);
		   break;
	  case 16: value=20*(float)pow(r,6)-30*(float)pow(r,4)+12*(float)pow(r,2)-1;
		   break;

	  /*n=4*/
	  case 17: value=r*r*r*r*(float)cos(4.0*theta);
		   break;
	  case 18: value=r*r*r*r*(float)sin(4.0*theta);
		   break;
	  case 19: value=(5*(float)pow(r,5)-4*(float)pow(r,3))*(float)cos(3*theta);
		   break;
	  case 20: value=(5*(float)pow(r,5)-4*(float)pow(r,3))*(float)sin(3*theta);
		   break;
	  case 21: value=(15*(float)pow(r,6)-20*(float)pow(r,4)+6*(float)pow(r,2))*(float)cos(2*theta);
		   break;
	  case 22: value=(15*(float)pow(r,6)-20*(float)pow(r,4)+6*(float)pow(r,2))*(float)sin(2*theta);
		   break;
	  case 23: value=(35*(float)pow(r,7)-60*(float)pow(r,5)+30*(float)pow(r,3)-4*r)*(float)cos(theta);
		   break;
	  case 24: value=(35*(float)pow(r,7)-60*(float)pow(r,5)+30*(float)pow(r,3)-4*r)*(float)sin(theta);
		   break;
	  case 25: value=70*(float)pow(r,8)-140*(float)pow(r,6)+90*(float)pow(r,4)-20*(float)pow(r,2)+1;
		   break;

	  /*n=5*/
	  case 26: value=(float)pow(r,5)*(float)cos(5*theta);
		   break;
	  case 27: value=(float)pow(r,5)*(float)sin(5*theta);
		   break;
	  case 28: value=(6*(float)pow(r,6)-5*(float)pow(r,4))*(float)cos(4*theta);
		   break;
	  case 29: value=(6*(float)pow(r,6)-5*(float)pow(r,4))*(float)sin(4*theta);
		   break;
	  case 30: value=(21*(float)pow(r,7)-30*(float)pow(r,5)+10*(float)pow(r,3))*(float)cos(3*theta);
		   break;
	  case 31: value=(21*(float)pow(r,7)-30*(float)pow(r,5)+10*(float)pow(r,3))*(float)sin(3*theta);
		   break;
	  case 32: value=(56*(float)pow(r,8)-105*(float)pow(r,6)+60*(float)pow(r,4)-10*(float)pow(r,2))*
			(float)cos(2*theta);
		   break;
	  case 33: value=(56*(float)pow(r,8)-105*(float)pow(r,6)+60*(float)pow(r,4)-10*(float)pow(r,2))*
			(float)sin(2*theta);
		   break;
	  case 34: value=(126*(float)pow(r,8)-280*(float)pow(r,6)+210*(float)pow(r,4)-60*(float)pow(r,2)
			+5)*r*(float)cos(theta);
		   break;
	  case 35: value=(126*(float)pow(r,8)-280*(float)pow(r,6)+210*(float)pow(r,4)-60*(float)pow(r,2)
			+5)*r*(float)sin(theta);
		   break;
	  case 36: value=252*(float)pow(r,10)-630*(float)pow(r,8)+560*(float)pow(r,6)-210*(float)pow(r,4)
			+30*(float)pow(r,2)-1;
		   break;
	  default: 
		  //AfxMessageBox("Error in f_zernik");
		 	return 0.0;
	  }
	return value;
	}

// Soving equation by ldlt		
void	ldlt(float a[],float b[],int n)
{
	int	i,j,k;
	float	t,t0,d;        
	if(fabs(a[0]) < 1.0e-30) return;
	a[0]=(float)(1.0/a[0]);
	for(i=1;i<n;i++) {
		for(j=0;j<=i-1;j++)
			if(j!=0) {
				t=0;
				for(k=0;k<=j-1;k++)
					t+=a[i*(i+1)/2+k]*a[j*(j+1)/2+k];
				a[i*(i+1)/2+j]-=t;
				}
		d=a[i*(i+1)/2+i];
		for(j=0;j<=i-1;j++) {
			t0=a[i*(i+1)/2+j];
			a[i*(i+1)/2+j]=t0*a[j*(j+1)/2+j];
			d-=t0*a[i*(i+1)/2+j];
			}                                    
		if( fabs(d) < 1.0e-30) return;
		a[i*(i+1)/2+i]=(float)(1.0/d);
		}
	for(i=1;i<n;i++) {
		t=0.0;
		for(k=0;k<=i-1;k++)
			t+=a[i*(i+1)/2+k]*b[k];
		b[i]-=t;
		}
	b[n-1]*=a[n*(n-1)/2+n-1];
	for(i=n-2;i>=0;i--) {
		t=b[i]*a[i*(i+1)/2+i];
		for(k=i+1;k<n;k++) t-=a[k*(k+1)/2+i]*b[k];
		b[i]=t;
		}
} 