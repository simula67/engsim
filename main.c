#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "struct.h"
#define pi M_PI
#define Rgas 8314.
#define Re 80000.
#define tol 1.e-1
#define MBr 33.32
#define MBp 28.88
void InitDat (char *file, struct data *pdat);
void check (double *ch);
double error (double er1, double er2);
double enthalpy (double t, double MB);
double volume (double angle, struct data *pdat);
double density (double temp, double pres, double MB);
double dQwall (int cycl, double angle, double t, struct data *pdat);
void Wiebe (double angle, double angleS, double angleF, double *xb,
	    double *dxb);
double dhdp (char *gas, double p1, double p2, double h1, double h2,
	     struct data *pdat);
double drdp (char *process, double t, double p1, double p2, double r1,
	     double r2, struct data *pdat);
double drdt (char *process, double t1, double t2, double r1, double r2,
	     struct data *pdat);
double Bhta (char *process, char *gas, double p1, double p2, double t1,
	     double t2, double r1, double r2, double h1, double h2,
	     struct data *pdat);
double Alpha (char *process, char *gas, double p1, double p2, double t1,
	      double t2, double r1, double r2, double h1, double h2,
	      struct data *pdat);
double funcf1 (char *process, char *gas, int cycl, double angle, double t1,
	       double t2, double t3, double p1, double p2, double v1,
	       double v2, double v1p, double v2p, double dv1, double dv2,
	       double m1, double m2, double m1p, double m2p, double dm1,
	       double dm2, struct data *pdat);
double funcf2 (char *process, char *gas, int cycl, double angle, double t1,
	       double t2, double t3, double p1, double p2, double v1,
	       double v2, double v1p, double v2p, double dv1, double dv2,
	       double m1, double m2, double m1p, double m2p, double dm1,
	       double dm2, struct data *pdat);
void rk4 (char *process, char *gas, int cycl, double sangle, double angle,
	  double p1, double t1, double *p2, double *t2, double v, double m,
	  struct data *pdat);
void engine (struct data *pdat);
// ************************************************
// *
//Main Programm
// *
// ************************************************
int main(int argc,char **argv){
  int i,j;
  struct data dat, *pdat;
  int cycl;
  char file[10];
  printf("\n\nMathematical and Numerical Modeling of Flow\n");
  printf("and Combustion Processes in a SI Engine.\n\n");
  if (argc!=2){
    printf("Error in opening the file.\n");
    printf("Usage:\n %s datafile\n",argv[0]);
    exit(1);
  }

  pdat=&dat;
  InitDat(argv[1],&dat);
  engine(pdat);
  return 0;
}
// ===================================================================
// ===================================================================
// Function which read the data from a dat file
void InitDat(char *file,struct data *pdat){
  FILE *fp;
  double at, ap, tw, a1, a2;
  at=ap=tw=a1=a2=0.;
  if ((fp=fopen(file, "r")) == NULL){
    printf("Error in opening the file.\n");
    exit(1);
  }
  fscanf(fp,"%lf %lf %lf %lf %d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
	 &at,&ap,
	 &(*pdat).rpm,&(*pdat).compratio,&(*pdat).ncycl,
	 &(*pdat).gamma,
	 &tw,
	 &(*pdat).B,&(*pdat).L,&(*pdat).R,
	 &(*pdat).Ui,&(*pdat).Bv,&(*pdat).Liv,
	 &a1,&a2);
  (*pdat).Ap=pi*((*pdat).B/2.)*((*pdat).B/2.);
  (*pdat).Av=pi*((*pdat).Bv/2.)*((*pdat).Bv/2.);
  (*pdat).atmopres=ap;
  (*pdat).atmotemp=at + 273.;
  (*pdat).Tw=tw + 273.;
  (*pdat).sacomb=a1*pi/180.;
  (*pdat).facomb=a2*pi/180.;
  (*pdat).Vc=(1./((*pdat).compratio-1.))*0.25*pi*(*pdat).B*(*pdat).B*(*pdat).L;
  (*pdat).omega=2.*pi*(*pdat).rpm/60.;
}
    // ===================================================================
    // Function which simulates the engine.
void engine(struct data *pdat){
  FILE *fp1, *fp2;
  char b, u;
  char COMBUSTION, COMPRESSION, EXPANSION;
  int cycl, logical;
  double xb, dxb;
  double angle, sangle;
  double Af, Ut, lt, Sl, hc;
  double Vf, Vu, dVu, Vb, dVb;
  double densi, denstot, densu ,densb;
  double me, dme, mu, dmu, mb, dmb, MBtot;
  double Temp, Pres, Temp1, Pres1, Temp2, Pres2, Temp3, Pres3;
  double Tempu, Presu, Tempb, Presb,					\
    Tempu1, Presu1, Tempb1, Presb1, Tempu2, Tempb2, Presb2, Presu2;
  double dTempu, dPresu, dTempb, dPresb, dTempu1, dPresu1, dTempb1, dPresb1;
  double mu_p, mb_p, Vu_p, Vb_p;
  cycl=0;
  xb=dxb=0.;
  sangle=pi/180.;
  Af=Ut=lt=Sl=hc=0.;
  Vf=Vu=dVu=Vb=dVb=0.;
  densi=denstot=densu=densb=0.;
  MBtot=me=dme=mu=dmu=mb=dmb=0.;
  Temp=Pres=Temp1=Pres1=Temp2=Pres2=Temp3=Pres3=0.;
  Tempu=Presu=Tempb=Presb=Tempu1=Presu1=Tempb1=Presb1=Tempu2=Presu2=Tempb2=Presb2=0.;
  dTempu=dPresu=dTempb=dPresb=dTempu1=dPresu1=dTempb1=dPresb1=0.;
  mu_p = mb_p = Vu_p = Vb_p = 0.0;
  if (((fp1=fopen("results1.dat","w")) == NULL)){
    printf("Programm couldn’t open the file.\n");
    exit(1);
  }
  if (((fp2=fopen("results2.dat","w")) == NULL)){
    printf("Programm couldn’t open the file.\n");
    exit(1);
  }
  {
    fprintf(fp1,"Results1.dat:\n");
    fprintf(fp2,"Results2.dat:\n");
    for(cycl=1;cycl<=(*pdat).ncycl;cycl++){
      //
      //Beginning of Intake Process
	printf("In loop\n");
	densi=density((*pdat).atmotemp,(*pdat).atmopres,MBr);
	if(cycl == 1){
	  Pres=(*pdat).atmopres-1.e-5*.5*densi*pow((*pdat).Ui,2.)*(pow((*pdat).Ap,2.)-pow((*pdat).Av,2.))/pow((*pdat).Ap,2.);
	  Temp=(*pdat).atmotemp;
	  printf(".");
	  fprintf(fp1,"%d \t%lf %lf %lf \n",cycl,270.,Pres,Temp);
	}else{
	  Pres=(*pdat).atmopres-1.e-5*.5*densi*pow((*pdat).Ui,2.)*(pow((*pdat).Ap,2.)-pow((*pdat).Av,2.))/pow((*pdat).Ap,2.);
	  Temp=(1./3.)*((*pdat).Tw + 2*Temp1);
	  printf(".");
	  fprintf(fp1,"%d \t%lf %lf %lf \n",cycl,270.,Pres,Temp);
	}
	Vu=(*pdat).Vc+.25*pi*(*pdat).B*(*pdat).B*(*pdat).L;
	mu=density(Temp,Pres,MBr)*Vu;
	//
	//
	//Beginning of compression Process
	angle=pi;
	sangle=pi/180.;
	Temp1=Pres1=Temp2=Pres2=0.;
	while(angle<2*pi-(*pdat).sacomb-2.*pi/180.){
	  logical=1;
	  while(logical == 1){
	    Pres1=Pres;Temp1=Temp;
	    Pres2=Pres;Temp2=Temp;
	    Vu=volume(angle,pdat);
	    check(&Vu);
	    rk4("COMPRESSION","u",cycl,sangle,angle,Pres,Temp,&Pres1,&Temp1,Vu,mu,pdat);
	    //	    BYPASS PRESSURE CALCULATION-CALCULATE USING IDEAL GAS LAW
	      Pres1 = (mu/Vu) * ( Rgas*Temp1/MBr ) *1.e-5;
	    rk4("COMPRESSION","u",cycl,.5*sangle,angle,Pres,Temp,&Pres2,&Temp2,Vu,mu,pdat);
	    Pres2 = (mu/Vu) * ( Rgas*Temp2/MBr ) *1.e-5;
	    Vu=volume(angle+.5*sangle,pdat) ;
	    check(&Vu);
	    rk4("COMPRESSION","u",cycl,.5*sangle,angle+.5*sangle,Pres,Temp,&Pres3,&Temp3, \
		Vu,mu,pdat);
	    Pres3 = (mu/Vu) * ( Rgas*Temp3/MBr ) *1.e-5;
	    if((error(Pres1,Pres3)<=tol)&&(error(Temp1,Temp3)<=tol)){
	      Pres=Pres1;Temp=Temp1;
	      printf(".");
	      fprintf(fp1,"%d %lf %lf %lf %lf \n",cycl,180.*sangle/pi,180.*angle/pi,Pres,Temp);
	      Temp1=Pres1=Temp2=Pres2=Temp3=Pres3=0.;
	      angle += sangle;
	      logical=0;
	    }else{
	      sangle=.5*sangle;
	      if(sangle<1.e-6){
		Pres=Pres1;Temp=Temp1;
		Temp1=Pres1=Temp2=Pres2=Temp3=Pres3=0.;
		angle += sangle;
		logical=0;
	      }
	    }
	  }
	}
	//
	//
	/*Still in Compression, 1 deg before the start of Combustion
	  Reduce step to 1/10 of what it is
	  Save values at time (n-1) in order to use in combustion
	*/
	sangle=.1*sangle;
	while(angle<2.*pi-(*pdat).sacomb){
	  Pres1=Pres;Temp1=Temp;
	  Vu=volume(angle,pdat);
	  check(&Vu);
	  rk4("COMPRESSION","u",cycl,sangle,angle,Pres,Temp,&Pres1,&Temp1,Vu,mu,pdat);
	  //BYPASS PRESSURE CALCULATION-CALCULATE USING IDEAL GAS LAW
	  Pres1 = (mu/Vu) * ( Rgas*Temp1/MBr ) *1.e-5;
	  Temp2=Pres2=0.;
	  Pres2=Pres;Temp2=Temp;
	  Pres=Pres1;Temp=Temp1;
	  Pres1=Pres2;Temp1=Temp2;
	  printf(".");
	  fprintf(fp1,"%d %lf %lf %lf %lf \n",cycl,180.*sangle/pi,180.*angle/pi,Pres,Temp);
	  mu_p = mu;
	  Vu_p = Vu;
	  angle += sangle;
	}
	//Beginning of Combustion Process
	Vu = volume(angle,pdat);
	densu=mu/Vu;
	check(&densu);
	Presu=Pres;Presu1=Pres1;
	Tempu=Temp;Tempu1=Temp1;
	while(angle<2.*pi+(*pdat).facomb){
	  Wiebe(angle,2.*pi-(*pdat).sacomb,2.*pi+(*pdat).facomb,&xb,&dxb);
	  //CAREFUL!! INCONSISTENCY BETWEEN VOLUMES
	  //SWITCH SUDDENLY BETWEEN Vc+sthing small TO FRACTION OF Vc
	  Vb=xb*volume(angle,pdat);
	  check(&Vb);
	  dVb=dxb*volume(angle,pdat)+xb*(*pdat).Vc+.25*pi*(*pdat).B*(*pdat).B*.5*(*pdat).L*(sin(angle)+(*pdat).R*sin(angle)*cos(angle));
	  check(&dVb);
	  Vu=(1-xb)*volume(angle,pdat);
  check(&Vb);
  dVu=-dVb;
  check(&dVu);
  if((mb<=1.e-9)&&(Vb<=1.e-10)){ densb=1.e-10;}
  else{ if(mb<=1.e-9){
      densb=1.e-10;}
    else{
      densb=mb/Vb;
      check(&densb);}
  }
  Vf=Vb+(me-mb)/densu;
  if (Vf<=0.0){Vf = 1.e-10;}
  check(&Vf);
  hc=Vf/(pi*pow((*pdat).B,2.)/4.);
  check(&hc);
  Af=.25*pi*pow(.5*hc,2.);
  check(&Af);
  Ut=.08*(*pdat).Ui*sqrt(densu/densi);
  check(&Ut);
  lt=.8*(*pdat).Liv*pow(densi/densu,3./4.);
  check(&lt);
  Sl=(*pdat).B*(*pdat).omega/( (*pdat).facomb+(*pdat).sacomb );
  check(&Sl);
  dme=densu*Af*(Ut+Sl)*(1./(*pdat).omega);
  check(&dme);
  me += sangle*dme;
  check(&me);
  dmb=(densu*Af*Sl+Sl*(me-mb)/lt)*(1./(*pdat).omega); check(&dmb);
  mb += sangle*dmb;
  check(&mb);
  dmu=-dmb;
  check(&dmu);
  mu += sangle*dmu;
  check(&mu);
  dTempb=funcf1("COMBUSTION","b",cycl,angle,\
		Tempb,Tempb1,Tempu,Presb,Presb1, \
		Vu,Vb,Vu_p,Vb_p,dVu,dVb,mu,mb,mu_p,mb_p,dmu,dmb,pdat);
  Tempb2 += sangle*dTempb;
  dPresb=funcf2("COMBUSTION","b",cycl,angle,\
		Tempb,Tempb1,Tempu,Presb,Presb1,			\
		Vu,Vb,Vu_p,Vb_p,dVu,dVb,mu,mb,mu_p,mb_p,dmu,dmb,pdat);
  Presb2 += sangle*dPresb;
  //Bypass Pressure calculation-Use ideal gas law
  if (mb<=1.e-5) {Presb2 = 1.e-10;}
  //Bypass Pressure calculation-Use ideal gas law
  if (mb<=1.e-5) {Presb2 = 1.e-10;}
  else
    {Presb2 = (mb/Vb) * (Rgas/MBp) * Tempb2 *1.e-5;}
  dTempu=funcf1("COMBUSTION","u",cycl,angle,	\
		Tempu,Tempu1,0.,Presu,Presu1,				\
		Vu,Vb,Vu_p,Vb_p,dVu,dVb,mu,mb,mu_p,mb_p,dmu,dmb,pdat);
  Tempu2 += sangle*dTempu;
  dPresu=funcf2("COMBUSTION","u",cycl,angle,	\
		Tempu,Tempu1,0.,Presu,Presu1,				\
		Vu,Vb,Vu_p,Vb_p,dVu,dVb,mu,mb,mu_p,mb_p,dmu,dmb,pdat);
  Presu2 += sangle*dPresu;
  //Bypass Pressure calculation-Use ideal gas law
  if (mu<=1.e-5) {Presu2 = 1.e-10;}
  else
    Presu2 = (mu/Vu) * (Rgas/MBr) * Tempu2 *1.e-5;
  
  Pres=Presu2+Presb2;
  MBtot=xb*MBp+(1.-xb)*MBr;
  denstot=xb*densb+(1.-xb)*densu;
  Temp=(Pres*MBtot)/(denstot*Rgas);
  printf(".");
  fprintf(fp1,"%d %lf %lf %lf %lf \t %lf %lf %lf %lf \n"		\
	  ,cycl,180.*sangle/pi,180.*angle/pi,Pres,Temp,Presu2,Tempu2,Presb2,Tempb2);
  fprintf(fp2,"%d %lf %lf %lf %lf \n"			\
	  ,cycl,180.*sangle/pi,180.*angle/pi,xb,1.-xb);
  Presu1=Presu;Presb1=Presb;
  Tempu1=Tempu;Presb1=Tempb;
  Presu=Presu2;Presb=Presb2;
  Tempu=Tempu2;Presb=Tempb2;
  if((mu<=1.e-10)&&(Vu<=1.e-10)){ densu=1.e-10;}
  //Added conditions
  else{ if(mu<=1.e-10){
      densu=1.e-10;}
    else{
      densu=mu/Vu;
      check(&densu);}
  }
  //Allocate mass-volume previous values
  mu_p = mu;
  Vu_p = Vu;
  mb_p = mb;
  Vb_p = Vb;
  angle += sangle;
	}
	// Beginning of Expansion Process
	sangle=pi/180.;
	Temp1=Pres1=Temp2=Pres2=0.;
	while(angle<=3*pi){
	  logical=1;
	  while(logical == 1){
	    Pres1=Pres;Temp1=Temp;
	    Pres2=Pres;Temp2=Temp;
	    rk4("EXPANSION","b",cycl,sangle,angle	\
		,Pres,Temp,&Pres1,&Temp1,Vb,mb,pdat);
	    rk4("EXPANSION","b",cycl,.5*sangle,angle	\
		,Pres,Temp,&Pres2,&Temp2,Vb,mb,pdat);
	    Vb=volume(angle+.5*sangle,pdat);
	    check(&Vu);
	    rk4("EXPANSION","b",cycl,.5*sangle,angle+.5*sangle	\
		,Pres2,Temp2,&Pres3,&Temp3,Vb,mb,pdat);
	    if((error(Pres1,Pres3)<=tol)&&(error(Temp1,Temp3)<=tol)){
	      Pres=Pres1;Temp=Temp1;
	      printf(".");
	      fprintf(fp1,"%d %lf %lf %lf \n",cycl,180.*angle/pi,Pres,Temp);
	      Pres1=Temp1=Pres2=Temp2=Pres3=Temp3=0.;
	      angle += sangle;
	      logical=0;
	    }else{
	      sangle=.5*sangle;
	    }
	  }
	}
	// Beginning of Exhaust Process
	Pres1=1.15*(*pdat).atmopres;
	Temp1=Temp / pow((Pres/Pres1),(1./3.));
	Pres=Pres1;Temp=Temp1;
	printf(".");
	fprintf(fp1,"%d \t%lf %lf %lf",cycl,180.*angle/pi,Pres,Temp);
	fprintf(fp1,"\n");
	fprintf(fp2,"\n");
      }
    }
    fclose(fp1);
    fclose(fp2);
  }
  // ===================================================================
  // Function for the Fourth order Runge-Kutta
  void rk4(char *process, char *gas, int cycl, double sangle, double
	   angle, double p1, double t1, double *p2, double *t2, double v,
	   double m, struct data *pdat){
    double k1,k2,k3,k4,k;
    double l1,l2,l3,l4,l;
    k1=k2=k3=k4=k=0.;
    l1=l2=l3=l4=l=0.;
    k1=sangle*funcf1(process,gas,cycl,angle,				\
		     t1,0.,0.,p1,0.,v,0.,0.,0.,0.,0.,m,0.,0.,0.,0.,0.,pdat);
    l1=sangle*funcf2(process,gas,cycl,angle,				\
		     t1,0.,0.,p1,0.,v,0.,0.,0.,0.,0.,m,0.,0.,0.,0.,0.,pdat);
    k2=sangle*funcf1(process,gas,cycl,angle+.5*sangle,\
		     t1+.5*k1,0.,0.,p1+.5*l1,0.,v,0.,0.,0.,0.,0.,m,0.,0.,0.,0.,0.,pdat);
    l2=sangle*funcf2(process,gas,cycl,angle+.5*sangle,			\
		     t1+.5*k1,0.,0.,p1+.5*l1,0.,v,0.,0.,0.,0.,0.,m,0.,0.,0.,0.,0.,pdat);
    k3=sangle*funcf1(process,gas,cycl,angle+.5*sangle,			\
		     t1+.5*k2,0.,0.,p1+.5*l2,0.,v,0.,0.,0.,0.,0.,m,0.,0.,0.,0.,0.,pdat);
    l3=sangle*funcf2(process,gas,cycl,angle+.5*sangle,			\
		     t1+.5*k2,0.,0.,p1+.5*l2,0.,v,0.,0.,0.,0.,0.,m,0.,0.,0.,0.,0.,pdat);
    k4=sangle*funcf1(process,gas,cycl,angle+sangle,\
		     t1+k3,0.,0.,p1+l3,0.,v,0.,0.,0.,0.,0.,m,0.,0.,0.,0.,0.,pdat);
    l4=sangle*funcf2(process,gas,cycl,angle+sangle,			\
		     t1+k3,0.,0.,p1+l3,0.,v,0.,0.,0.,0.,0.,m,0.,0.,0.,0.,0.,pdat);
    k=(1./6.)*(k1+2.*k2+2.*k3+k4);
    *p2 =p1 + k;
    l=(1./6.)*(l1+2.*l2+2.*l3+l4);
    *t2 =t1 + l;
  }
  // ===================================================================
  // Function which calculates the derivative of temperature
  double funcf1(char *process, char *gas, int cycl, double angle,
		double t1, double t2, double t3, double p1,
		double p2, double v1, double v2, double v1p, double v2p, double
		dv1, double dv2, double m1, double m2, double m1p, double m2p,
		double dm1, double dm2, struct data *pdat){
    double w,rho1,rho2,h1,h2;
    double AlphaU, BhtaU, AlphaB, BhtaB;
    w=rho1=rho2=h1=h2=0.;
    if(strcmp(process,"COMPRESSION")==0){
      if(v1<=(*pdat).Vc){
	rho1=m1/(*pdat).Vc;
	check(&rho1);
      }else{
	rho1=m1/v1;
	check(&rho1);
      }
      w=(Bhta(process,gas,p1,p2,t1,t2,rho1,0.,0.,0.,pdat)	\
	 /Alpha(process,gas,p1,p2,t1,t2,rho1,0.,0.,0.,pdat));
      w*=(-((sin(angle)+.5*sin(2.*angle)*(*pdat).R)/(1.-cos(angle)	\
						     +.5*(*pdat).R*pow(sin(angle),2.)))-(1./(*pdat).omega) \
	  *dQwall(cycl,angle,t1,pdat)/(Bhta(process,gas,p1,p2,t1,t2,rho1,0.,0.,0.,pdat)*m1));
    }
    else if(strcmp(process,"COMBUSTION")==0){
      if(strcmp(gas,"u")==0){
	h1=enthalpy(t1,MBr);
	if((m1<=1.e-10)&&(v1<=1.e-10)){ rho1=1.e-10;}
	else{ if(m1<=1.e-10){
	    rho1=1.e-10;}
	  else{
	    rho1=m1/v1;
	    check(&rho1);}
	}
	h2=enthalpy(t2,MBr);
	if((m1p<=1.e-10)&&(v1p<=1.e-10)){
	  rho2=1.e-10;}
	else{ if(m1p<=1.e-10){ rho2=1.e-10;}
	  else{
	    rho2=m1p/v1p;
	    check(&rho2);}
	}
	AlphaU = Alpha(process,gas,p1,p2,t1,t2,rho1,rho2,h1,h2,pdat); //check(&AlphaU);
	if (AlphaU >=1.+20) AlphaU = 1.e+20;
	if (AlphaU <=-1.+20) AlphaU = -1.e+20;
	BhtaU = Bhta(process,gas,p1,p2,t1,t2,rho1,rho2,h1,h2,pdat); //check(&BhtaU);
	if (BhtaU >=1.+20) BhtaU = 1.e+20;
	if (BhtaU <=-1.+20) BhtaU = -1.e+20;
	w = BhtaU / AlphaU;
	printf("%lf \t bhtaU=%lg, alphaU=%lg\n",180.*angle/pi,BhtaU, AlphaU);
	w*=((1./(*pdat).omega)*(dm1/m1)*(1.-h1/BhtaU)-(dv1/v1)+(1./(*pdat).omega)*\
	    (-dQwall(cycl,angle,t1,pdat)+dm1*(5./2.)*(Rgas/MBr)*t1)/(BhtaU*m1));
      }
      else if(strcmp(gas,"b")==0){
	h1=enthalpy(t1,MBp);
	if((m2<=1.e-10)&&(v2<=1.e-10)){ rho1=1.e-10;}
	else{ if(m2<=1.e-10){
	    rho1=1.e-10;}
	  else{
	    rho1=m2/v2;
	    check(&rho1);}
	}
	h2=enthalpy(t2,MBp);
	if((m2p<=1.e-10)&&(v2p<=1.e-10)){
	  rho2=1.e-10;}
	else{ if(m2p<=1.e-10){ rho2=1.e-10;}
	  else{
	    rho2=m2p/v2p;
	    check(&rho2);}
	}
	AlphaB = Alpha(process,gas,p1,p2,t1,t2,rho1,rho2,h1,h2,pdat); //check(&AlphaB);
	if (AlphaB >=1.+20) AlphaB = 1.e+20;
	if (AlphaB <=-1.+20) AlphaB = -1.e+20;
	BhtaB = Bhta(process,gas,p1,p2,t1,t2,rho1,rho2,h1,h2,pdat); //check(&BhtaB);
	if (BhtaB >=1.+20) BhtaB = 1.e+20;
	if (BhtaB <=-1.+20) BhtaB = -1.e+20;
	w = BhtaB / AlphaB;
	printf("%lf \t bhtaB=%lg, alphaB=%lg\n",180.*angle/pi,BhtaB, AlphaB);
	w*=((1./(*pdat).omega)*(dm2/m2)*(1.-h1/BhtaB)-(dv2/v2)+(1./(*pdat).omega)*\
	    (-dQwall(cycl,angle,t1,pdat)+dm2*(5./2.)*(Rgas/MBr)*t3)/(BhtaB*m2));
      }
    }
    else if(strcmp(process,"EXPANSION")==0){
      if(v1<=(*pdat).Vc){
	rho1=m1/(*pdat).Vc;
	check(&rho1);
      }else{
	rho1=m1/v1;
	check(&rho1);
      }
      w=(Bhta(process,gas,p1,p2,t1,t2,rho1,0.,0.,0.,pdat)\
	 /Alpha(process,gas,p1,p2,t1,t2,rho1,0.,0.,0.,pdat));
      w*=(-((sin(angle)+.5*sin(2*angle)*(*pdat).R)/(1.-cos(angle)\
						    +.5*(*pdat).R*pow(sin(angle),2.)))-(1./(*pdat).omega)\
	  *(dQwall(cycl,angle,t1,pdat))/(Bhta(process,gas,p1,p2,t1,t2,rho1,0.,0.,0.,pdat)*m2));
    }
    return(w);
  }
  // Function which calculates the derivative of pressure
  double funcf2(char *process, char *gas, int cycl, double
		angle,double t1, double t2, double t3, double p1, double p2,
		double v1, double v2, double v1p, double v2p, double dv1, double
		dv2, double m1, double m2, double m1p, double m2p, double dm1,
		double dm2, struct data *pdat){
    double w,rho1,rho2,h1,h2;
    w=rho1=rho2=h1=h2=0.;
    if(strcmp(process,"COMPRESSION")==0){
      if(v1<=(*pdat).Vc){
	rho1=m1/(*pdat).Vc;
	check(&rho1);
      }else{
	rho1=m1/v1;
	check(&rho1);
      }
      w=1.e-5*rho1*(1./drdp(process,t1,p1,p2,rho1,rho2,pdat));
      w*=(-((sin(angle)+.5*sin(2.*angle)\
	     *(*pdat).R)/(1.-cos(angle)+.5*(*pdat).R*pow(sin(angle),2.)))-\
	  (1./rho1)*drdt(process,t1,t2,rho1,0.,pdat)*funcf1(process,gas,cycl,angle, \
							    t1,t2,t3,p1,p2,v1,v2,v1p,v2p,dv1,dv2,m1,m2,m1p,m2p,dm1,dm2,pdat));
    }
    else if(strcmp(process,"COMBUSTION")==0){
      if(strcmp(gas,"u")==0){
	h1=enthalpy(t1,MBr);
	if((m1<=1.e-10)&&(v1<=1.e-10)){ rho1=1.e-10;}
	else{ if(m1<=1.e-10){
	    rho1=1.e-10;}
	  else{
	    rho1=m1/v1;
	    check(&rho1);}
	}
	h2=enthalpy(t2,MBr);
	if((m1p<=1.e-10)&&(v1p<=1.e-10)){
	  rho1=1.e-10;}
	else{ if(m1p<=1.e-10){ rho2=1.e-10;}
	  else{
	    rho2=m1p/v1p;
	    check(&rho2);}
	}
	w=rho1*(1./drdp(process,t1,p1,p2,rho1,rho2,pdat))*(-(dv1/v1)-(1./rho1)*	\
							   drdt(process,t1,t2,rho1,rho2,pdat)* \
							   funcf1(process,gas,cycl,angle, \
								  t1,t2,t3,p1,p2,v1,v2,v1p,v2p,dv1,dv2,	\
								  m1,m2,m1p,m2p,dm1,dm2,pdat)+(dm1/m1));
      }
      else if(strcmp(gas,"b")==0){
	h1=enthalpy(t1,MBp);
	if((m2<=1.e-10)&&(v2<=1.e-10)){ rho1=1.e-10;}
	else{ if(m2<=1.e-10){
	    rho1=1.e-10;}
	  else{
	    rho1=m2/v2;
	    check(&rho1);}
	}
	h2=enthalpy(t2,MBp);
	if((m2p<=1.e-10)&&(v2p<=1.e-10)){
	  rho1=1.e-10;}
	else{ if(m2p<=1.e-10){ rho2=1.e-10;}
	  else{
	    rho2=m2p/v2p;
	    check(&rho2);}
	}
	w=rho1*(1./drdp(process,t1,p1,p2,rho1,rho2,pdat))*(-(dv2/v2)-(1./rho2)*\
							   drdt(process,t1,t2,rho1,rho2,pdat)*\
							   funcf1(process,gas,cycl,angle,\
								  t1,t2,t3,p1,p2,v1,v2,v1p,v2p,dv1,dv2,\
								  m1,m2,m1p,m2p,dm1,dm2,pdat)+(dm2/m2));
      }
    }
    else if(strcmp(process,"EXPANSION")==0){
      if(v1<=(*pdat).Vc){
	rho1=m1/(*pdat).Vc;
	check(&rho1);
      }else{
	rho1=m1/v1;
	check(&rho1);
      }
      w=1.e-5*rho1*(1./drdp(process,t1,p1,p2,rho1,0.,pdat));
      w*=(-((sin(angle)+.5*sin(2.*angle)*(*pdat).R)/(1.-cos(angle)+.5*(*pdat).R*\
						     pow(cos(angle),2.)))-(1./rho1)*drdt(process,t1,t2,rho1,0.,pdat)*\
	  funcf1(process,gas,cycl,angle,\
		 t1,t2,t3,p1,p2,v1,v2,v1p,v2p,dv1,dv2,\
		 m1,m2,m1p,m2p,dm1,dm2,pdat));
    }
    return(w);
  }
  // Function which calculates A
double Alpha(char *process, char *gas, double p1, double p2,
	     double t1, double t2, double rho1, double rho2,
	     double h1, double h2, struct data *pdat){
  
  double w, Cpu, Cpb;
  w=Cpu=Cpb=0.;
  if(strcmp(process,"COMPRESSION")==0){
      Cpu=.08*2220. + .92*1000.;
      w=(drdt(process,t1,t2,rho1,rho2,pdat)\
	 /drdp(process,t1,p1,p2,rho1,rho2,pdat))*(1./rho1)+Cpu;
    }
    else if(strcmp(process,"COMBUSTION")==0){
      if(strcmp(gas,"u")==0){
	Cpu=.08*2220. + .92*1000.;
	w=(drdt(process,t1,t2,rho1,rho2,pdat)\
	   /drdp(process,t1,p1,p2,rho1,rho2,pdat))*(1./rho1)+Cpu;
      }
      else if(strcmp(gas,"b")==0){
	Cpb=.18*850.+.1*4180.+.72*1042.;
	w=(drdt(process,t1,t2,rho1,rho2,pdat)\
	   /drdp(process,t1,p1,p2,rho1,rho2,pdat));
	w*=((1./rho1)-(1.e-5)*dhdp(gas,p1,p2,h1,h2,pdat))+Cpb;
      }
    }
    else if(strcmp(process,"EXPANSION")==0){
      Cpb=.18*850.+.1*4180.+.72*1042.;
      w=(drdt(process,t1,t2,rho1,rho2,pdat)\
	 /drdp(process,t1,p1,p2,rho1,rho2,pdat));
      w*=((1./rho1)-(1.e-5)*dhdp(gas,p1,p2,h1,h2,pdat))+Cpb;
    }
    return(w);
  }
  // Function which calculates Bhta
  double Bhta(char *process, char *gas, double p1, double p2, double
	      t1, double t2, double rho1, \
	      double rho2, double h1, double h2, struct data *pdat){
    double w=0.;
    if(strcmp(process,"COMPRESSION")==0){
      w=(1./drdp(process,t1,p1,p2,rho1,rho2,pdat));
    }
    else if(strcmp(process,"COMBUSTION")==0){
      if(strcmp(gas,"u")==0){
	w=(1./drdp(process,t1,p1,p2,rho1,rho2,pdat));
      }
      else if(strcmp(gas,"b")==0){
	w=(1./drdp(process,t1,p1,p2,rho1,rho2,pdat))\
	  *(1.-(1.e-5)*rho1*dhdp(gas,p1,p2,h1,h2,pdat));
      }
    }
    else if(strcmp(process,"EXPANSION")==0){
      w=(1./drdp(process,t1,p1,p2,rho1,rho2,pdat))\
	*(1.-(1.e-5)*rho1*dhdp(gas,p1,p2,h1,h2,pdat));
    }
    return(w);
  }
  // Function which calculates the pressure derivative of density
  double drdp(char *process, double t, double p1, double p2, double
	      rho1, double rho2, struct data *pdat){
    double w=0.;
    if(strcmp(process,"COMPRESSION")==0){
      w=MBr/(Rgas*t*(*pdat).gamma);
    }
    else if(strcmp(process,"COMBUSTION")==0){
      if(fabs(rho1-rho2)<=1.e-20){
	w=1.e-10;}
      else{
	w=(rho1-rho2)/( 1.e5*(p1-p2) );}
      // Pressure converted in Pa for consistency
    }
    else if(strcmp(process,"EXPANSION")==0){
      w=MBp/(Rgas*t*(*pdat).gamma);
    }
    return(w);
  }
  // Function which calculates the temperature derivative of density
  double drdt(char *process, double t1, double t2, double rho1,
	      double rho2, struct data *pdat){
    double w, Cvu, Cvb;
    w=Cvu=Cvb=0.;
    if(strcmp(process,"COMPRESSION")==0){
      Cvu=.08*2220. + .92*1000. - (Rgas/MBr);
      w=rho1*Cvu/(Rgas*t1);
    }
    else if(strcmp(process,"COMBUSTION")==0){
      if(fabs(rho1-rho2)<=1.e-20){
	w=1.e-10;}
      else{
	w=(rho1-rho2)/(t1-t2);}
    }
    else if(strcmp(process,"EXPANSION")==0){
      Cvb=.18*850.+.1*4180.+.72*1042. - (Rgas/MBp);
      w=rho1*Cvb/(Rgas*t1);
    }
    return(w);
  }
  // Function which calculates the pressure derivative of enthalpy
  double dhdp(char *gas, double p1, double p2, double h1, double h2,
	      struct data *pdat){
    double w=0.;
    if(fabs(h1-h2)<=1.e-20){
      w=1.e-10;}
    else{
      w=(h1-h2)/(p1-p2);}
    return(w);
  }
  // Function which calculates the derivative of heat at wall
  double dQwall(int cycl, double angle, double t, struct data
		*pdat){
    double q, aw, awm, Twd;
    q=aw=Twd=awm=0.;
    if((cycl == 1)&&(angle<=2.*pi+(*pdat).facomb)){
      Twd=273.;
    }else{
      Twd=(*pdat).Tw;
    }
    awm=2.*pi*((*pdat).B)*(*pdat).L/((*pdat).compratio-1.);
    aw=2.*pi*((*pdat).B)*(.5*(*pdat).L)*(1.-cos(angle)+.5*(*pdat).R*sin(angle)*sin(angle)) + awm;
    if(aw<=awm){aw=awm;}
    q=aw*(.575*(155.)*(1./(*pdat).B)*pow(Re,0.7)*(t-Twd)+4.3e-9*(pow(t,4)-pow(Twd,4)));
    q=1.e+0*q;
    return(q);
  }
  // Function which calculates enthalpy
  double enthalpy(double t, double MB){
    double w=0.;
    w=(5./2.)*Rgas*(1./MB)*t;
    return(w);
  }
  // Function which calculates the burned mass fraction versus
  // crank angle curve and its derivative.Wiebe Function
  void Wiebe(double angle, double angleS, double angleF, double *xb,double *dxb){
    double w=0.;
    w=(angle-angleS)/(angleF-angleS);
    *xb=1.-(1./exp(5.*pow(w,3.)));
    if(fabs(*xb)<=1.e-10){ *xb=1.e-10;}
    *dxb =5.*(1./exp(5.*pow(w,3.)))*(1./pow(angleF-angleS,3.));
      if(fabs(*dxb)<=1.e-10){ *dxb=1.e-10;}
      if (*xb>1.0){ *xb = 1.0; *dxb = 0.0;
      }
      //Check that xb can not exceed the value of 1
  }
  // Function which calculates density
  double density(double t, double p, double MB){
    double w=0.;
    if (fabs(t)<=1.e-10){w = 0.0;}
    else{
      w=1.e5*p/(t*(Rgas/MB));
    }
    return(w);
  }
  // Function which calculates volume by angle
  double volume(double angle, struct data *pdat){
    double w=0.;
    w=(*pdat).Vc+.25*pi*(*pdat).B*(*pdat).B*.5*(*pdat).L*(1.-cos(angle)+.5*(*pdat).R*sin(angle)*sin(angle));
    return(w);
  }
  // Function which ensures 
  void check(double *ch){
    double w,tol1;
    w=tol1=0.;
    w=*ch;
    tol1=1.e-10;
    if(fabs(*ch)<=tol1){ *ch=tol1;
    }else{ *ch=w;
    }
  }
  // Function which calculates the percent difference between two values
  double error(double er1, double er2){
    double w1, w2;
    w1=w2=0.;
    w1=(fabs(er1) > fabs(er2)) ? fabs(er1) : fabs(er2);
    w2=fabs(er1 - er2)/fabs(w1);
    return(w2);
  }
