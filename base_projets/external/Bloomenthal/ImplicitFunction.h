#ifndef IMPLICITFUNCTION
#define IMPLICITFUNCTION

#include <math.h>
#include <iostream.h>

class ImplicitFunction{
public:
  virtual float value(float x, float y, float z)=0;
  virtual void gradient(float g[3], float x, float y, float z)=0;
  virtual float valueAndGradient(float g[3], float x, float y, float z)=0;

  //Used for curvature estimations.
  //Double precision is required for more accuarte estimations.
  virtual double valueAndGradient1(double g[3], float x, float y, float z)=0;
  virtual double valueAndGradient12(double g[3], double gg[3][3],
                                    float x, float y, float z)=0;
  virtual double valueAndGradient123(double g1[3], double g2[3][3],
                                     double g3[3][3][3],
                                     float x, float y, float z)=0;
  virtual double valueAndGradient1234(double g1[3], double g2[3][3],
                                      double g3[3][3][3], double g4[3][3][3][3],
                                      float x, float y, float z)=0;

  void newton(float p[3], float x, float y, float z, float eps){
	  //int c = 0;
	for(int i=0; i<10; i++){
      double g[3];
      double f = valueAndGradient1(g, x, y, z);
	  double dt = g[0]*g[0] + g[1]*g[1] + g[2]*g[2];

      if((float)dt == 0)
        break;
      double dt1 = -f/dt;
      x += dt1*g[0];
      y += dt1*g[1];
      z += dt1*g[2];

	  //c++;
	  if(fabs(f/sqrt(dt)) < eps){
		//cout << i << ":" << fabs(f/sqrt(dt)) << endl;
        break;
	  }
    }
	//if(i == 10)
	  //cout << i << endl;//"Reached Max Iter" << endl;
    p[0] = x;
    p[1] = y;
    p[2] = z;
  }

  //From here, original code from Prof. Alexander G. Belyaev
  void curvatureHK(double &H, double &K, float x, float y, float z){
    double pd1[3], pd2[3][3];
    valueAndGradient12(pd1, pd2, x, y, z);

    /* temporary derivatives*/
    double fxfx = pd1[0]*pd1[0];
    double fxfy = pd1[0]*pd1[1];
    double fxfz = pd1[0]*pd1[2];
    double fyfy = pd1[1]*pd1[1];
    double fyfz = pd1[1]*pd1[2];
    double fzfz = pd1[2]*pd1[2];

    double fxx = pd2[0][0];
    double fxy = pd2[0][1];
    double fxz = pd2[0][2];
    double fyy = pd2[1][1];
    double fyz = pd2[1][2];
    double fzz = pd2[2][2];

    double g2 = pd1[0]*pd1[0]+pd1[1]*pd1[1]+pd1[2]*pd1[2];
    double g1 = sqrt(g2);
    double g3 = g2*g1;
    double g4 = g2*g2;

    /* mean and gaussian curvatures */
    H = (fxx*(fyfy+fzfz) + fyy*(fxfx+fzfz) + fzz*(fxfx+fyfy)
		- 2*(fxy*fxfy+fxz*fxfz+fyz*fyfz))/2;
    H /= g3;

    K = fxfx*(fyy*fzz-fyz*fyz)
      + fyfy*(fxx*fzz-fxz*fxz)
        + fzfz*(fxx*fyy-fxy*fxy)
          + 2*(fxfy*(fxz*fyz-fxy*fzz)
               + fxfz*(fxy*fyz-fxz*fyy)
               + fyfz*(fxy*fxz-fxx*fyz));
    K /= g4;
  }

  void curvatureDerivative(double &Kmax, double &Kmin,
                           double &Rmax, double &Rmin,
                           double Tmax[3], double Tmin[3],
                           float x, float y, float z){

    double pd1[3], pd2[3][3], pd3[3][3][3];
    valueAndGradient123(pd1, pd2, pd3, x, y, z);
    fillPd2(pd2);
    fillPd3(pd3);

    /* temporary derivatives*/
    double fxfx = pd1[0]*pd1[0];
    double fxfy = pd1[0]*pd1[1];
    double fxfz = pd1[0]*pd1[2];
    double fyfy = pd1[1]*pd1[1];
    double fyfz = pd1[1]*pd1[2];
    double fzfz = pd1[2]*pd1[2];

    double fxx = pd2[0][0];
    double fxy = pd2[0][1];
    double fxz = pd2[0][2];
    double fyy = pd2[1][1];
    double fyz = pd2[1][2];
    double fzz = pd2[2][2];

    double g2 = pd1[0]*pd1[0]+pd1[1]*pd1[1]+pd1[2]*pd1[2];
	if(g2 == 0){
		Kmax = 0;
		Kmin = 0;

		Rmax=0;
		Rmin=0;

		Tmax[0] = Tmax[1] = Tmax[2] = 0;
		Tmin[0] = Tmin[1] = Tmin[2] = 0;

		return;
	}
    double g1 = sqrt(g2);
    double g3 = g2*g1;
    double g4 = g2*g2;

    /* mean and gaussian curvatures */
    double H = (fxx*(fyfy+fzfz) + fyy*(fxfx+fzfz) + fzz*(fxfx+fyfy)
         - 2*(fxy*fxfy+fxz*fxfz+fyz*fyfz))/2;
    H /= g3;

    double K = fxfx*(fyy*fzz-fyz*fyz)
      + fyfy*(fxx*fzz-fxz*fxz)
        + fzfz*(fxx*fyy-fxy*fxy)
          + 2*(fxfy*(fxz*fyz-fxy*fzz)
               + fxfz*(fxy*fyz-fxz*fyy)
               + fyfz*(fxy*fxz-fxx*fyz));

    K /= g4;

    /* principal curvatures */
    double discr = sqrt(fabs(H*H-K));

    Kmax = H + discr;
    Kmin = H - discr;

    Rmax=0;
    Rmin=0;

    Tmax[0] = Tmax[1] = Tmax[2] = 0;
    Tmin[0] = Tmin[1] = Tmin[2] = 0;

    double EPS = 0.0001;
    if(discr > EPS){/* it is not an umbilic */

      /* matrix entries */
      double m11 = ((-1 + fxfx/g2)*fxx)/g1 + (fxfy*fxy)/g3 + (fxfz*fxz)/g3;
      double m12 = ((-1 + fxfx/g2)*fxy)/g1 + (fxfy*fyy)/g3 + (fxfz*fyz)/g3;
      double m13 = ((-1 + fxfx/g2)*fxz)/g1 + (fxfy*fyz)/g3 + (fxfz*fzz)/g3;
      double m21 = (fxfy*fxx)/g3 + ((-1 + fyfy/g2)*fxy)/g1 + (fyfz*fxz)/g3;
      double m22 = (fxfy*fxy)/g3 + ((-1 + fyfy/g2)*fyy)/g1 + (fyfz*fyz)/g3;
      double m23 = (fxfy*fxz)/g3 + ((-1 + fyfy/g2)*fyz)/g1 + (fyfz*fzz)/g3;
      double m31 = (fxfz*fxx)/g3 + (fyfz*fxy)/g3 + ((-1 + fzfz/g2)*fxz)/g1;
      double m32 = (fxfz*fxy)/g3 + (fyfz*fyy)/g3 + ((-1 + fzfz/g2)*fyz)/g1;
      double m33 = (fxfz*fxz)/g3 + (fyfz*fyz)/g3 + ((-1 + fzfz/g2)*fzz)/g1;

      /* solve for eigenvectors */
      double tmp1 = m11+Kmax;
      double tmp2 = m22+Kmax;
      double tmp3 = m33+Kmax;

      double ux[3], uy[3], uz[3], len[3];
      ux[0] = m12*m23-m13*tmp2;
      uy[0] = m13*m21-m23*tmp1;
      uz[0] = tmp1*tmp2-m12*m21;
      len[0] = sqrt(ux[0]*ux[0]+uy[0]*uy[0]+uz[0]*uz[0]);

      ux[1] = m12*tmp3-m13*m32;
      uy[1] = m13*m31-tmp1*tmp3;
      uz[1] = tmp1*m32-m12*m31;
      len[1] = sqrt(ux[1]*ux[1]+uy[1]*uy[1]+uz[1]*uz[1]);

      ux[2] = tmp2*tmp3-m23*m32;
      uy[2] = m23*m31-m21*tmp3;
      uz[2] = m21*m32-m31*tmp2;
      len[2] = sqrt(ux[2]*ux[2]+uy[2]*uy[2]+uz[2]*uz[2]);

      int index = 0;
      double max = len[0];
      if ( len[1] > max ) {
        index  = 1;
        max = len[1];
      }
      if ( len[2] > max ) {
        index = 2;
        max = len[2];
      }

      Tmax[0] = ux[index]/len[index];
      Tmax[1] = uy[index]/len[index];
      Tmax[2] = uz[index]/len[index];

      /* second tangent is cross product of first tangent and normal */
      double N[3];
      int i;
      for(i=0; i<3; i++){
        N[i] = - pd1[i]/g1;
      }

      Tmin[0] = Tmax[1]*N[2]-Tmax[2]*N[1];
      Tmin[1] = Tmax[2]*N[0]-Tmax[0]*N[2];
      Tmin[2] = Tmax[0]*N[1]-Tmax[1]*N[0];

      Rmax = 0;
      Rmin = 0;

      for(i=0; i<3; i++){
        for(int j=0; j<3; j++){
          Rmax += pd2[i][j]*Tmax[i]*N[j];
          Rmin += pd2[i][j]*Tmin[i]*N[j];
        }
      }

      Rmax *= (3*Kmax);
      Rmin *= (3*Kmin);

      for(i=0; i<3; i++)
        for(int j=0; j<3; j++)
          for(int k=0; k<3; k++){
            Rmax += pd3[i][j][k]*Tmax[i]*Tmax[j]*Tmax[k];
            Rmin += pd3[i][j][k]*Tmin[i]*Tmin[j]*Tmin[k];
          }

      Rmax /= g1;
      Rmin /= g1;
    }
  }

  void curvatureDerivative(double &Kmax, double &Kmin,
                           double &Rmax, double &Rmin,
                           double &Dmax, double &Dmin,
                           double Tmax[3], double Tmin[3],
                           float x, float y, float z){

    double pd1[3], pd2[3][3], pd3[3][3][3], pd4[3][3][3][3];
    valueAndGradient1234(pd1, pd2, pd3, pd4, x, y, z);
    fillPd2(pd2);
    fillPd3(pd3);
    fillPd4(pd4);

    /* temporary derivatives*/
    double fxfx = pd1[0]*pd1[0];
    double fxfy = pd1[0]*pd1[1];
    double fxfz = pd1[0]*pd1[2];
    double fyfy = pd1[1]*pd1[1];
    double fyfz = pd1[1]*pd1[2];
    double fzfz = pd1[2]*pd1[2];

    double fxx = pd2[0][0];
    double fxy = pd2[0][1];
    double fxz = pd2[0][2];
    double fyy = pd2[1][1];
    double fyz = pd2[1][2];
    double fzz = pd2[2][2];

    double g2 = pd1[0]*pd1[0]+pd1[1]*pd1[1]+pd1[2]*pd1[2];
	if(g2 == 0){
		Kmax = 0;
		Kmin = 0;

		Rmax=0;
		Rmin=0;

		Tmax[0] = Tmax[1] = Tmax[2] = 0;
		Tmin[0] = Tmin[1] = Tmin[2] = 0;

		return;
	}
    double g1 = sqrt(g2);
    double g3 = g2*g1;
    double g4 = g2*g2;

    /* mean and gaussian curvatures */
    double H = (fxx*(fyfy+fzfz) + fyy*(fxfx+fzfz) + fzz*(fxfx+fyfy)
         - 2*(fxy*fxfy+fxz*fxfz+fyz*fyfz))/2;
    H /= g3;

    double K = fxfx*(fyy*fzz-fyz*fyz)
      + fyfy*(fxx*fzz-fxz*fxz)
        + fzfz*(fxx*fyy-fxy*fxy)
          + 2*(fxfy*(fxz*fyz-fxy*fzz)
               + fxfz*(fxy*fyz-fxz*fyy)
               + fyfz*(fxy*fxz-fxx*fyz));

    K /= g4;

    /* principal curvatures */
    double discr = sqrt(fabs(H*H-K));

    Kmax = H + discr;
    Kmin = H - discr;

    Rmax=0;
    Rmin=0;

    Dmax = -1;
    Dmin = -1;

    Tmax[0] = Tmax[1] = Tmax[2] = 0;
    Tmin[0] = Tmin[1] = Tmin[2] = 0;

    double EPS = 0.01;
    if(discr > EPS){/* it is not an umbilic */

      /* matrix entries */
      double m11 = ((-1 + fxfx/g2)*fxx)/g1 + (fxfy*fxy)/g3 + (fxfz*fxz)/g3;
      double m12 = ((-1 + fxfx/g2)*fxy)/g1 + (fxfy*fyy)/g3 + (fxfz*fyz)/g3;
      double m13 = ((-1 + fxfx/g2)*fxz)/g1 + (fxfy*fyz)/g3 + (fxfz*fzz)/g3;
      double m21 = (fxfy*fxx)/g3 + ((-1 + fyfy/g2)*fxy)/g1 + (fyfz*fxz)/g3;
      double m22 = (fxfy*fxy)/g3 + ((-1 + fyfy/g2)*fyy)/g1 + (fyfz*fyz)/g3;
      double m23 = (fxfy*fxz)/g3 + ((-1 + fyfy/g2)*fyz)/g1 + (fyfz*fzz)/g3;
      double m31 = (fxfz*fxx)/g3 + (fyfz*fxy)/g3 + ((-1 + fzfz/g2)*fxz)/g1;
      double m32 = (fxfz*fxy)/g3 + (fyfz*fyy)/g3 + ((-1 + fzfz/g2)*fyz)/g1;
      double m33 = (fxfz*fxz)/g3 + (fyfz*fyz)/g3 + ((-1 + fzfz/g2)*fzz)/g1;

      /* solve for eigenvectors */
      double tmp1 = m11+Kmax;
      double tmp2 = m22+Kmax;
      double tmp3 = m33+Kmax;

      double ux[3], uy[3], uz[3], len[3];
      ux[0] = m12*m23-m13*tmp2;
      uy[0] = m13*m21-m23*tmp1;
      uz[0] = tmp1*tmp2-m12*m21;
      len[0] = sqrt(ux[0]*ux[0]+uy[0]*uy[0]+uz[0]*uz[0]);

      ux[1] = m12*tmp3-m13*m32;
      uy[1] = m13*m31-tmp1*tmp3;
      uz[1] = tmp1*m32-m12*m31;
      len[1] = sqrt(ux[1]*ux[1]+uy[1]*uy[1]+uz[1]*uz[1]);

      ux[2] = tmp2*tmp3-m23*m32;
      uy[2] = m23*m31-m21*tmp3;
      uz[2] = m21*m32-m31*tmp2;
      len[2] = sqrt(ux[2]*ux[2]+uy[2]*uy[2]+uz[2]*uz[2]);

      int index = 0;
      double max = len[0];
      if ( len[1] > max ) {
        index  = 1;
        max = len[1];
      }
      if ( len[2] > max ) {
        index = 2;
        max = len[2];
      }

      Tmax[0] = ux[index]/len[index];
      Tmax[1] = uy[index]/len[index];
      Tmax[2] = uz[index]/len[index];

      /* second tangent is cross product of first tangent and normal */
      double N[3];
      int i, j, k, l;
      for(i=0; i<3; i++){
        //N[i] = pd1[i]/g1;
        N[i] = - pd1[i]/g1;
      }

      Tmin[0] = Tmax[1]*N[2]-Tmax[2]*N[1];
      Tmin[1] = Tmax[2]*N[0]-Tmax[0]*N[2];
      Tmin[2] = Tmax[0]*N[1]-Tmax[1]*N[0];

      Rmax = 0;
      Rmin = 0;

      for(i=0; i<3; i++){
        for(int j=0; j<3; j++){
          Rmax += pd2[i][j]*Tmax[i]*N[j];
          Rmin += pd2[i][j]*Tmin[i]*N[j];
        }
      }

      Rmax *= (3*Kmax);
      Rmin *= (3*Kmin);

      for(i=0; i<3; i++)
        for(int j=0; j<3; j++)
          for(int k=0; k<3; k++){
            Rmax += pd3[i][j][k]*Tmax[i]*Tmax[j]*Tmax[k];
            Rmin += pd3[i][j][k]*Tmin[i]*Tmin[j]*Tmin[k];
          }

      Rmax /= g1;
      Rmin /= g1;

      Dmax = 0;
      Dmin = 0;

      double tmp = 0;

      for(i=0; i<3; i++)
        for(j=0; j<3; j++)
          for(k=0; k<3; k++)
            for(l=0; l<3; l++){
              tmp += pd4[i][j][k][l]*Tmax[i]*Tmax[j]*Tmax[k]*Tmax[l];
            }
      Dmax += tmp;
      tmp = 0;

      for(i=0; i<3; i++)
        for(j=0; j<3; j++)
          for(k=0; k<3; k++){
            tmp += pd3[i][j][k]*Tmax[i]*Tmax[j]*N[k];
          }
      tmp *= (6*Kmax);
      Dmax += tmp;
      tmp = 0;

      //************ it is zero on ridges ************/
      for(i=0; i<3; i++)
        for(j=0; j<3; j++){
          tmp += pd2[i][j]*Tmax[i]*N[j];
        }
      tmp *= (4*Rmax);
      Dmax += tmp;
      tmp = 0;
      /************ it is zero on ridges ************/

      for(i=0; i<3; i++)
        for(j=0; j<3; j++){
          tmp += pd2[i][j]*N[i]*N[j];
        }
      tmp *= (3*Kmax*Kmax);
      Dmax += tmp;
      tmp = 0;

      Dmax /= g1;
      Dmax -= (3*Kmax*Kmax*Kmax);


      /*** component for 2nd derivative along curvatute line ***/
      for(i=0; i<3; i++)
        for(j=0; j<3; j++){
          tmp += pd2[i][j]*Tmin[i]*N[j];
        }

      tmp *= Kmax;

      for(i=0; i<3; i++)
        for(j=0; j<3; j++)
          for(k=0; k<3; k++){
            tmp += pd3[i][j][k]*Tmax[i]*Tmax[j]*Tmin[k];
          }

      tmp /= g1;

      Dmax -= (tmp*tmp / (Kmax - Kmin));

      /*********************************************************/

      tmp = 0;

      for(i=0; i<3; i++)
        for(j=0; j<3; j++)
          for(k=0; k<3; k++)
            for(l=0; l<3; l++){
              tmp += pd4[i][j][k][l]*Tmin[i]*Tmin[j]*Tmin[k]*Tmin[l];
            }
      Dmin += tmp;
      tmp = 0;

      for(i=0; i<3; i++)
        for(j=0; j<3; j++)
          for(k=0; k<3; k++){
            tmp += pd3[i][j][k]*Tmin[i]*Tmin[j]*N[k];
          }
      tmp *= (6*Kmin);
      Dmin += tmp;
      tmp = 0;

      /************ it is zero on ridges ************/
      for(i=0; i<3; i++)
        for(j=0; j<3; j++){
          tmp += pd2[i][j]*Tmin[i]*N[j];
        }
      tmp *= (4*Rmin);
      Dmin += tmp;
      tmp = 0;
      /************ it is zero on ridges ************/

      for(i=0; i<3; i++)
        for(j=0; j<3; j++){
          tmp += pd2[i][j]*N[i]*N[j];
        }
      tmp *= (3*Kmin*Kmin);
      Dmin += tmp;
      tmp = 0;

      Dmin /= g1;
      Dmin -= (3*Kmin*Kmin*Kmin);

       /*** component for 2nd derivative along curvatute line ***/
      for(i=0; i<3; i++)
        for(j=0; j<3; j++){
          tmp += pd2[i][j]*Tmax[i]*N[j];
        }

      tmp *= Kmin;

      for(i=0; i<3; i++)
        for(j=0; j<3; j++)
          for(k=0; k<3; k++){
            tmp += pd3[i][j][k]*Tmin[i]*Tmin[j]*Tmax[k];
          }

      tmp /= g1;

      Dmin -= (tmp*tmp / (Kmin - Kmax));

      /*********************************************************/
      /*
      if(Dmax > 0)
        Dmax = 0.5;
      else
        Dmax = -0.5;

      if(Dmin > 0)
        Dmin = 0.5;
      else
        Dmin = -0.5;*/
    }
  }

  void curvatureTensor(double &Kmax, double &Kmin, double Tmax[3], double Tmin[3],
                       float x, float y, float z){
    double pd1[3], pd2[3][3];
    valueAndGradient12(pd1, pd2, x, y, z);
    fillPd2(pd2);

    /* temporary derivatives*/
    double fxfx = pd1[0]*pd1[0];
    double fxfy = pd1[0]*pd1[1];
    double fxfz = pd1[0]*pd1[2];
    double fyfy = pd1[1]*pd1[1];
    double fyfz = pd1[1]*pd1[2];
    double fzfz = pd1[2]*pd1[2];

    double fxx = pd2[0][0];
    double fxy = pd2[0][1];
    double fxz = pd2[0][2];
    double fyy = pd2[1][1];
    double fyz = pd2[1][2];
    double fzz = pd2[2][2];

    double g2 = pd1[0]*pd1[0]+pd1[1]*pd1[1]+pd1[2]*pd1[2];
    double g1 = sqrt(g2);
    double g3 = g2*g1;
    double g4 = g2*g2;

    /* mean and gaussian curvatures */
    double H = (fxx*(fyfy+fzfz) + fyy*(fxfx+fzfz) + fzz*(fxfx+fyfy)
         - 2*(fxy*fxfy+fxz*fxfz+fyz*fyfz))/2;
    H /= g3;

    double K = fxfx*(fyy*fzz-fyz*fyz)
      + fyfy*(fxx*fzz-fxz*fxz)
        + fzfz*(fxx*fyy-fxy*fxy)
          + 2*(fxfy*(fxz*fyz-fxy*fzz)
               + fxfz*(fxy*fyz-fxz*fyy)
               + fyfz*(fxy*fxz-fxx*fyz));

    K /= g4;

    /* principal curvatures */
    double discr = sqrt(fabs(H*H-K));

    Kmax = H + discr;
    Kmin = H - discr;

    double EPS = 0.000001;
    if(discr > EPS){/* it is not an umbilic */

      /* matrix entries */
      double m11 = ((-1 + fxfx/g2)*fxx)/g1 + (fxfy*fxy)/g3 + (fxfz*fxz)/g3;
      double m12 = ((-1 + fxfx/g2)*fxy)/g1 + (fxfy*fyy)/g3 + (fxfz*fyz)/g3;
      double m13 = ((-1 + fxfx/g2)*fxz)/g1 + (fxfy*fyz)/g3 + (fxfz*fzz)/g3;
      double m21 = (fxfy*fxx)/g3 + ((-1 + fyfy/g2)*fxy)/g1 + (fyfz*fxz)/g3;
      double m22 = (fxfy*fxy)/g3 + ((-1 + fyfy/g2)*fyy)/g1 + (fyfz*fyz)/g3;
      double m23 = (fxfy*fxz)/g3 + ((-1 + fyfy/g2)*fyz)/g1 + (fyfz*fzz)/g3;
      double m31 = (fxfz*fxx)/g3 + (fyfz*fxy)/g3 + ((-1 + fzfz/g2)*fxz)/g1;
      double m32 = (fxfz*fxy)/g3 + (fyfz*fyy)/g3 + ((-1 + fzfz/g2)*fyz)/g1;
      double m33 = (fxfz*fxz)/g3 + (fyfz*fyz)/g3 + ((-1 + fzfz/g2)*fzz)/g1;

      /* solve for eigenvectors */
      double tmp1 = m11+Kmax;
      double tmp2 = m22+Kmax;
      double tmp3 = m33+Kmax;

      double ux[3], uy[3], uz[3], len[3];
      ux[0] = m12*m23-m13*tmp2;
      uy[0] = m13*m21-m23*tmp1;
      uz[0] = tmp1*tmp2-m12*m21;
      len[0] = sqrt(ux[0]*ux[0]+uy[0]*uy[0]+uz[0]*uz[0]);

      ux[1] = m12*tmp3-m13*m32;
      uy[1] = m13*m31-tmp1*tmp3;
      uz[1] = tmp1*m32-m12*m31;
      len[1] = sqrt(ux[1]*ux[1]+uy[1]*uy[1]+uz[1]*uz[1]);

      ux[2] = tmp2*tmp3-m23*m32;
      uy[2] = m23*m31-m21*tmp3;
      uz[2] = m21*m32-m31*tmp2;
      len[2] = sqrt(ux[2]*ux[2]+uy[2]*uy[2]+uz[2]*uz[2]);

      int index = 0;
      double max = len[0];
      if ( len[1] > max ) {
        index  = 1;
        max = len[1];
      }
      if ( len[2] > max ) {
        index = 2;
        max = len[2];
      }

      Tmax[0] = ux[index]/len[index];
      Tmax[1] = uy[index]/len[index];
      Tmax[2] = uz[index]/len[index];

      /* second tangent is cross product of first tangent and normal */
      double N[3];
      int i;
      for(i=0; i<3; i++){
        //N[i] = pd1[i]/g1;
        N[i] = - pd1[i]/g1;
      }

      Tmin[0] = Tmax[1]*N[2]-Tmax[2]*N[1];
      Tmin[1] = Tmax[2]*N[0]-Tmax[0]*N[2];
      Tmin[2] = Tmax[0]*N[1]-Tmax[1]*N[0];
    }
    else{
      Tmax[0] = Tmax[1] = Tmax[2] =
      Tmin[0] = Tmin[1] = Tmin[2] = 0;
    }
  }

private:
  inline void fillPd2(double pd2[3][3]){
    pd2[1][0] = pd2[0][1];
    pd2[2][0] = pd2[0][2];
    pd2[2][1] = pd2[1][2];
  }

  inline void fillPd3(double pd3[3][3][3]){
    pd3[0][1][0] = pd3[0][0][1];
    pd3[0][2][0] = pd3[0][0][2];
    pd3[0][2][1] = pd3[0][1][2];

    pd3[1][0][0] = pd3[0][0][1];
    pd3[1][0][1] = pd3[0][1][1];
    pd3[1][0][2] = pd3[0][1][2];
    pd3[1][1][0] = pd3[0][1][1];
    pd3[1][2][0] = pd3[0][1][2];
    pd3[1][2][1] = pd3[1][1][2];

    pd3[2][0][0] = pd3[0][0][2];
    pd3[2][0][1] = pd3[0][1][2];
    pd3[2][0][2] = pd3[0][2][2];
    pd3[2][1][0] = pd3[0][1][2];
    pd3[2][1][1] = pd3[1][1][2];
    pd3[2][1][2] = pd3[1][2][2];
    pd3[2][2][0] = pd3[0][2][2];
    pd3[2][2][1] = pd3[1][2][2];
  }

  inline void fillPd4(double pd4[3][3][3][3]){
    pd4[0][0][1][0] = pd4[0][0][0][1];
    pd4[0][0][2][0] = pd4[0][0][0][2];
    pd4[0][0][2][1] = pd4[0][0][1][2];

    pd4[0][1][0][0] = pd4[0][0][0][1];
    pd4[0][1][0][1] = pd4[0][0][1][1];
    pd4[0][1][0][2] = pd4[0][0][1][2];
    pd4[0][1][1][0] = pd4[0][0][1][1];
    pd4[0][1][2][0] = pd4[0][0][1][2];
    pd4[0][1][2][1] = pd4[0][1][1][2];

    pd4[0][2][0][0] = pd4[0][0][0][2];
    pd4[0][2][0][1] = pd4[0][0][1][2];
    pd4[0][2][0][2] = pd4[0][0][2][2];
    pd4[0][2][1][0] = pd4[0][0][1][2];
    pd4[0][2][1][1] = pd4[0][1][1][2];
    pd4[0][2][1][2] = pd4[0][1][2][2];
    pd4[0][2][2][0] = pd4[0][0][2][2];
    pd4[0][2][2][1] = pd4[0][1][2][2];


    pd4[1][0][0][0] = pd4[0][0][0][1];
    pd4[1][0][0][1] = pd4[0][0][1][1];
    pd4[1][0][0][2] = pd4[0][0][1][2];
    pd4[1][0][1][0] = pd4[0][0][1][1];
    pd4[1][0][1][1] = pd4[0][1][1][1];
    pd4[1][0][1][2] = pd4[0][1][1][2];
    pd4[1][0][2][0] = pd4[0][0][1][2];
    pd4[1][0][2][1] = pd4[0][1][1][2];
    pd4[1][0][2][2] = pd4[0][1][2][2];

    pd4[1][1][0][0] = pd4[0][0][1][1];
    pd4[1][1][0][1] = pd4[0][1][1][1];
    pd4[1][1][0][2] = pd4[0][1][1][2];
    pd4[1][1][1][0] = pd4[0][1][1][1];
    pd4[1][1][2][0] = pd4[0][1][1][2];
    pd4[1][1][2][1] = pd4[1][1][1][2];

    pd4[1][2][0][0] = pd4[0][0][1][2];
    pd4[1][2][0][1] = pd4[0][1][1][2];
    pd4[1][2][0][2] = pd4[0][1][2][2];
    pd4[1][2][1][0] = pd4[0][1][1][2];
    pd4[1][2][1][1] = pd4[1][1][1][2];
    pd4[1][2][1][2] = pd4[1][1][2][2];
    pd4[1][2][2][0] = pd4[0][1][2][2];
    pd4[1][2][2][1] = pd4[1][1][2][2];


    pd4[2][0][0][0] = pd4[0][0][0][2];
    pd4[2][0][0][1] = pd4[0][0][1][2];
    pd4[2][0][0][2] = pd4[0][0][2][2];
    pd4[2][0][1][0] = pd4[0][0][1][2];
    pd4[2][0][1][1] = pd4[0][1][1][2];
    pd4[2][0][1][2] = pd4[0][1][2][2];
    pd4[2][0][2][0] = pd4[0][0][2][2];
    pd4[2][0][2][1] = pd4[0][1][2][2];
    pd4[2][0][2][2] = pd4[0][2][2][2];

    pd4[2][1][0][0] = pd4[0][0][1][2];
    pd4[2][1][0][1] = pd4[0][1][1][2];
    pd4[2][1][0][2] = pd4[0][1][2][2];
    pd4[2][1][1][0] = pd4[0][1][1][2];
    pd4[2][1][1][1] = pd4[1][1][1][2];
    pd4[2][1][1][2] = pd4[1][1][2][2];
    pd4[2][1][2][0] = pd4[0][1][2][2];
    pd4[2][1][2][1] = pd4[1][1][2][2];
    pd4[2][1][2][2] = pd4[1][2][2][2];

    pd4[2][2][0][0] = pd4[0][0][2][2];
    pd4[2][2][0][1] = pd4[0][1][2][2];
    pd4[2][2][0][2] = pd4[0][2][2][2];
    pd4[2][2][1][0] = pd4[0][1][2][2];
    pd4[2][2][1][1] = pd4[1][1][2][2];
    pd4[2][2][1][2] = pd4[1][2][2][2];
    pd4[2][2][2][0] = pd4[0][2][2][2];
    pd4[2][2][2][1] = pd4[1][2][2][2];
  }
};

#endif
