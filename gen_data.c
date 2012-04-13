#include <stdio.h>
#include <stdlib.h>
/*
1. Pressure : 1 atm
2. Atmoshperic Temperature : 15 degree C
3. Engine Rotation: 3000rpm
4. Compression Ratio: 10
5. Number of perform Cycles: 4
6. Speed Heat Ratio: 1.4
7. Temperature of the cylinder wall: 200 degree C
8. Cylinder Bore: 0.08 m
9. Cylinder stroke: 0.1 m
10.Ratio of crank radius to connecting rod length: 0.3
11.Intake velocity : 60m/s
12.Intake Valve Diameter: 0.032m
13.maximum intake valve lift measured: 0.0065m
14.the angle when combustion starts?: 330 degree
15.the angle when combustion ends: 370 degress
*/
int main(int argc,char *argv[])
{
  FILE *fp;
  if ( argc !=2 ) {
    printf("Error in opening the file.\n");
    printf("Usage:\n %s datafile\n",argv[0]);
    exit(1);
  }

  double atmopres = 1;
  double atmotemp = 15;
  double rpm = 3000;
  double compratio = 10;
  int ncycl = 4;
  double gamma = 1.4;
  double tw = 200;
  double B = 0.08;
  double L = 0.1;
  double R = 0.3;
  double Ui = 60;
  double Bv = 0.032;
  double Liv = 0.0065;
  double a1 = 330;
  double a2 = 370;
  if ((fp=fopen(argv[1], "w")) == NULL){
    printf("Error in opening the file.\n");
    exit(1);
  }
  fprintf(fp,"%lf %lf %lf %lf %d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",atmotemp,atmopres,rpm,compratio,ncycl,gamma,tw,B,L,R,Ui,Bv,Liv,a1,a2);
  fclose(fp);
  return 0;
}
