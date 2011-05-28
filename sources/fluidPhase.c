#include "silmin.h"

#define FILE_TIME_STAMP_BUFFER_SIZE 30

static char fileTimeStamp[FILE_TIME_STAMP_BUFFER_SIZE];

const char *fluidPhase_ver(void) { 
  strncpy(fileTimeStamp, __BASE_FILE__, FILE_TIME_STAMP_BUFFER_SIZE);
  strncat(fileTimeStamp, __TIMESTAMP__, FILE_TIME_STAMP_BUFFER_SIZE - strlen(fileTimeStamp) - 1);
  return fileTimeStamp; 
}

#define SQUARE(x) ((x)*(x))
#define CUBE(x)   ((x)*(x)*(x))
#define QUARTIC(x) ((x)*(x)*(x)*(x))

#define NR         1
#define NS         0
#define NA         2
#define FR0(i)     (i == 0) ? 1.0 - x[0] : - x[0]
#define DFR0DR0(i) - 1.0

#define R          8.3143

/*
 * H2O-CO2 model of Duan, Z, Zhang, Z (2006)
 * Equation of state of the H2O, CO2, and H2O-CO2 systems up to 10 GPa and 2573.15 K:
 * Molecular dynamics simulations with ab initio potential surface
 * Geochimica et Cosmochimica Acta, 70, 2311-2324
*/

/* pure EOSterms for 0 to 0.2 GPa */
static const double H2OLa1  =  4.38269941E-02; 
static const double H2OLa2  = -1.68244362E-01; 
static const double H2OLa3  = -2.36923373E-01; 
static const double H2OLa4  =  1.13027462E-02; 
static const double H2OLa5  = -7.67764181E-02; 
static const double H2OLa6  =  9.71820593E-02; 
static const double H2OLa7  =  6.62674916E-05; 
static const double H2OLa8  =  1.06637349E-03; 
static const double H2OLa9  = -1.23265258E-03; 
static const double H2OLa10 = -8.93953948E-06; 
static const double H2OLa11 = -3.88124606E-05; 
static const double H2OLa12 =  5.61510206E-05; 
static const double H2OLa   =  7.51274488E-03; 
static const double H2OLb   =  2.51598931E+00; 
static const double H2OLc   =  3.94000000E-02; 

static const double CO2La1  =  1.14400435E-01; 
static const double CO2La2  = -9.38526684E-01;
static const double CO2La3  =  7.21857006E-01;
static const double CO2La4  =  8.81072902E-03;
static const double CO2La5  =  6.36473911E-02;
static const double CO2La6  = -7.70822213E-02;
static const double CO2La7  =  9.01506064E-04;
static const double CO2La8  = -6.81834166E-03;
static const double CO2La9  =  7.32364258E-03;
static const double CO2La10 = -1.10288237E-04;
static const double CO2La11 =  1.26524193E-03;
static const double CO2La12 = -1.49730823E-03;
static const double CO2La   =  7.81940730E-03;
static const double CO2Lb   = -4.22918013E+00;
static const double CO2Lc   =  1.58500000E-01;

/* pure EOS terms for 0.2 to 10 GPa */
static const double H2OHa1  =  4.68071541E-02;  
static const double H2OHa2  = -2.81275941E-01;  
static const double H2OHa3  = -2.43926365E-01;  
static const double H2OHa4  =  1.10016958E-02;  
static const double H2OHa5  = -3.86603525E-02;  
static const double H2OHa6  =  9.30095461E-02;  
static const double H2OHa7  = -1.15747171E-05;  
static const double H2OHa8  =  4.19873848E-04;  
static const double H2OHa9  = -5.82739501E-04;  
static const double H2OHa10 =  1.00936000E-06;  
static const double H2OHa11 = -1.01713593E-05;  
static const double H2OHa12 =  1.63934213E-05;  
static const double H2OHa   = -4.49505919E-02;  
static const double H2OHb   = -3.15028174E-01;  
static const double H2OHc   =  1.25000000E-02; 

static const double CO2Ha1  =  5.72573440E-03;
static const double CO2Ha2  =  7.94836769E+00;
static const double CO2Ha3  = -3.84236281E+01;
static const double CO2Ha4  =  3.71600369E-02;
static const double CO2Ha5  = -1.92888994E+00;
static const double CO2Ha6  =  6.64254770E+00;
static const double CO2Ha7  = -7.02203950E-06;
static const double CO2Ha8  =  1.77093234E-02;
static const double CO2Ha9  = -4.81892026E-02;
static const double CO2Ha10 =  3.88344869E-06;
static const double CO2Ha11 = -5.54833167E-04;
static const double CO2Ha12 =  1.70489748E-03;
static const double CO2Ha   = -4.13039220E-01;
static const double CO2Hb   = -8.47988634E+00;
static const double CO2Hc   =  2.80000000E-02;

/* H2O, critical constants, K, bars, J/bar */

static const double H2OTc = 647.25;
static const double H2OPc = 221.19;
#define H2OVc (8.314467*H2OTc/H2OPc)

/* CO2 */

static const double CO2Tc = 304.1282;
static const double CO2Pc = 73.773;
#define CO2Vc (8.314467*CO2Tc/CO2Pc)

#define H2O 0
#define CO2 1

static const double idealCoeff[13][2] = {
  {  3.10409601236035e+01, -0.18188731e+01 },
  { -3.91422080460869e+01,  0.12903022e+02 },
  {  3.79695277233575e+01, -0.96634864e+01 },
  { -2.18374910952284e+01,  0.42251879e+01 },
  {  7.42251494566339e+00, -0.10421640e+01 },
  { -1.38178929609470e+00,  0.12683515e+00 },
  {  1.08807067571454e-01, -0.49939675e-02 },
  { -1.20771176848589e+01,  0.24950242e+01 },
  {  3.39105078851732e+00, -0.82723750e+00 },
  { -5.84520979955060e-01,  0.15372481e+00 },
  {  5.89930846488082e-02, -0.15861243e-01 },
  { -3.12970001415882e-03,  0.86017150e-03 },
  {  6.57460740981757e-05, -0.19222165e-04 }
};

static double powSum(double a, double fa, double b, double fb) {
  double sum = 0.0;
  sum += (a >= 0.0) ? fa*pow(a, 1.0/3.0) : -fa*pow(-a, 1.0/3.0);
  sum += (b >= 0.0) ? fb*pow(b, 1.0/3.0) : -fb*pow(-b, 1.0/3.0);
  return (sum >= 0.0) ? pow(sum/(fa+fb), 3.0) : -pow(-sum/(fa+fb), 3.0);
}

static double DpowSum(double a, double fa, double b, double fb, double da, double db) {
  double sum = 0.0, dsum = 0.0;
  sum += (a >= 0.0) ? fa*pow(a, 1.0/3.0) : -fa*pow(-a, 1.0/3.0);
  sum += (b >= 0.0) ? fb*pow(b, 1.0/3.0) : -fb*pow(-b, 1.0/3.0);
  sum = (sum >= 0.0) ? pow(sum/(fa+fb), 2.0) : pow(-sum/(fa+fb), 2.0);
  dsum += (a >= 0.0) ? da*fa/pow(a, 2.0/3.0)/3.0 : da*fa/pow(-a, 2.0/3.0)/3.0;
  dsum += (b >= 0.0) ? db*fb/pow(b, 2.0/3.0)/3.0 : db*fb/pow(-b, 2.0/3.0)/3.0;
  return 3.0*sum*dsum/(fa+fb);
}

static double D2powSum(double a, double fa, double b, double fb, double da, double db, double d2a, double d2b) {
  double sum = 0.0, dsum = 0.0, d2sum = 0.0;
  sum += (a >= 0.0) ? fa*pow(a, 1.0/3.0) : -fa*pow(-a, 1.0/3.0);
  sum += (b >= 0.0) ? fb*pow(b, 1.0/3.0) : -fb*pow(-b, 1.0/3.0);
  sum /= fa + fb;
  
  dsum += (a >= 0.0) ? da*fa/pow(a, 2.0/3.0)/3.0 : da*fa/pow(-a, 2.0/3.0)/3.0;
  dsum += (b >= 0.0) ? db*fb/pow(b, 2.0/3.0)/3.0 : db*fb/pow(-b, 2.0/3.0)/3.0;
  dsum /= fa + fb;
  
  d2sum += (a >= 0.0) ? d2a*fa/pow(a, 2.0/3.0)/3.0 : d2a*fa/pow(-a, 2.0/3.0)/3.0;
  d2sum += (b >= 0.0) ? d2b*fb/pow(b, 2.0/3.0)/3.0 : d2b*fb/pow(-b, 2.0/3.0)/3.0;
  d2sum += (a >= 0.0) ? -2.0*da*da*fa/pow(a, 5.0/3.0)/9.0 : 2.0*da*da*fa/pow(-a, 5.0/3.0)/9.0;
  d2sum += (b >= 0.0) ? -2.0*db*db*fb/pow(b, 5.0/3.0)/9.0 : 2.0*db*db*fb/pow(-b, 5.0/3.0)/9.0;
  d2sum /= fa + fb;
  
  return 6.0*sum*dsum*dsum + 3.0*sum*sum*d2sum;
}

static double D3powSum(double a, double fa, double b, double fb, double da, double db, double d2a, double d2b, double d3a, double d3b) {
  double sum = 0.0, dsum = 0.0, d2sum = 0.0, d3sum = 0.0;
  sum += (a >= 0.0) ? fa*pow(a, 1.0/3.0) : -fa*pow(-a, 1.0/3.0);
  sum += (b >= 0.0) ? fb*pow(b, 1.0/3.0) : -fb*pow(-b, 1.0/3.0);
  sum /= fa + fb;
  
  dsum += (a >= 0.0) ? da*fa/pow(a, 2.0/3.0)/3.0 : da*fa/pow(-a, 2.0/3.0)/3.0;
  dsum += (b >= 0.0) ? db*fb/pow(b, 2.0/3.0)/3.0 : db*fb/pow(-b, 2.0/3.0)/3.0;
  dsum /= fa + fb;
  
  d2sum += (a >= 0.0) ? d2a*fa/pow(a, 2.0/3.0)/3.0 : d2a*fa/pow(-a, 2.0/3.0)/3.0;
  d2sum += (b >= 0.0) ? d2b*fb/pow(b, 2.0/3.0)/3.0 : d2b*fb/pow(-b, 2.0/3.0)/3.0;
  d2sum += (a >= 0.0) ? -2.0*da*da*fa/pow(a, 5.0/3.0)/9.0 : 2.0*da*da*fa/pow(-a, 5.0/3.0)/9.0;
  d2sum += (b >= 0.0) ? -2.0*db*db*fb/pow(b, 5.0/3.0)/9.0 : 2.0*db*db*fb/pow(-b, 5.0/3.0)/9.0;
  d2sum /= fa + fb;
  
  d3sum += (a >= 0.0) ? d3a*fa/pow(a, 2.0/3.0)/3.0 : d3a*fa/pow(-a, 2.0/3.0)/3.0;
  d3sum += (b >= 0.0) ? d3b*fb/pow(b, 2.0/3.0)/3.0 : d3b*fb/pow(-b, 2.0/3.0)/3.0;
  d3sum += (a >= 0.0) ? -6.0*da*d2a*fa/pow(a, 5.0/3.0)/9.0 : 6.0*da*d2a*fa/pow(-a, 5.0/3.0)/9.0;
  d3sum += (b >= 0.0) ? -6.0*db*d2b*fb/pow(b, 5.0/3.0)/9.0 : 6.0*db*d2b*fb/pow(-b, 5.0/3.0)/9.0;
  d3sum += (a >= 0.0) ? 10.0*da*da*da*fa/pow(a, 8.0/3.0)/27.0 : 10.0*da*da*da*fa/pow(-a, 8.0/3.0)/27.0;
  d3sum += (b >= 0.0) ? 10.0*db*db*db*fb/pow(b, 8.0/3.0)/27.0 : 10.0*db*db*db*fb/pow(-b, 8.0/3.0)/27.0;  
  d3sum /= fa + fb;
  
  return 6.0*dsum*dsum*dsum + 18.0*sum*dsum*d2sum + 3.0*sum*sum*d3sum;
}

static void BVcAndDerivative(int useLowPcoeff, double t, double x[2], 
  double *bv, double bvPrime[2], double *dbvdt, double *d2bvdt2, double *d3bvdt3, double dbvPrimedt[2], double d2bvPrimedt2[2], double d3bvPrimedt3[2]) {
  double bEnd[2], dbEnddt[2], d2bEnddt2[2], d3bEnddt3[2];
  double H2OTr    = t/H2OTc;
  double CO2Tr    = t/CO2Tc;
  double k1       = 0.0;
  double dH2OTrdt = 1.0/H2OTc;
  double dCO2Trdt = 1.0/CO2Tc;
  double dk1dt    = 0.0;
  double d2k1dt2  = 0.0;
  double d3k1dt3  = 0.0;

  if (useLowPcoeff) {
    bEnd[H2O] = H2OLa1 + H2OLa2/H2OTr/H2OTr + H2OLa3/H2OTr/H2OTr/H2OTr;
    bEnd[CO2] = CO2La1 + CO2La2/CO2Tr/CO2Tr + CO2La3/CO2Tr/CO2Tr/CO2Tr;
    k1 = 3.131 - 5.0624e-3*t + 1.8641e-6*t*t - 31.409/t;
    dbEnddt[H2O] = - 2.0*H2OLa2*dH2OTrdt/H2OTr/H2OTr/H2OTr - 3.0*H2OLa3*dH2OTrdt/H2OTr/H2OTr/H2OTr/H2OTr;
    dbEnddt[CO2] = - 2.0*CO2La2*dCO2Trdt/CO2Tr/CO2Tr/CO2Tr - 3.0*CO2La3*dCO2Trdt/CO2Tr/CO2Tr/CO2Tr/CO2Tr;
    dk1dt = - 5.0624e-3 + 2.0*1.8641e-6*t + 31.409/t/t;
    d2bEnddt2[H2O] = 6.0*H2OLa2*dH2OTrdt*dH2OTrdt/H2OTr/H2OTr/H2OTr/H2OTr + 12.0*H2OLa3*dH2OTrdt*dH2OTrdt/H2OTr/H2OTr/H2OTr/H2OTr/H2OTr;
    d2bEnddt2[CO2] = 6.0*CO2La2*dCO2Trdt*dCO2Trdt/CO2Tr/CO2Tr/CO2Tr/CO2Tr + 12.0*CO2La3*dCO2Trdt*dCO2Trdt/CO2Tr/CO2Tr/CO2Tr/CO2Tr/CO2Tr;
    d2k1dt2 = 2.0*1.8641e-6 - 2.0*31.409/t/t/t;
    d3bEnddt3[H2O] = - 24.0*H2OLa2*dH2OTrdt*dH2OTrdt*dH2OTrdt/H2OTr/H2OTr/H2OTr/H2OTr/H2OTr - 60.0*H2OLa3*dH2OTrdt*dH2OTrdt*dH2OTrdt/H2OTr/H2OTr/H2OTr/H2OTr/H2OTr/H2OTr;
    d3bEnddt3[CO2] = - 24.0*CO2La2*dCO2Trdt*dCO2Trdt*dCO2Trdt/CO2Tr/CO2Tr/CO2Tr/CO2Tr/CO2Tr - 60.0*CO2La3*dCO2Trdt*dCO2Trdt*dCO2Trdt/CO2Tr/CO2Tr/CO2Tr/CO2Tr/CO2Tr/CO2Tr;
    d3k1dt3 =  6.0*31.409/t/t/t/t;
  } else {
    bEnd[H2O] = H2OHa1 + H2OHa2/H2OTr/H2OTr + H2OHa3/H2OTr/H2OTr/H2OTr;
    bEnd[CO2] = CO2Ha1 + CO2Ha2/CO2Tr/CO2Tr + CO2Ha3/CO2Tr/CO2Tr/CO2Tr;
    k1 = 9.034 - 7.9212e-3*t + 2.3285e-6*t*t - 2.4221e3/t;
    dbEnddt[H2O] = - 2.0*H2OHa2*dH2OTrdt/H2OTr/H2OTr/H2OTr - 3.0*H2OHa3*dH2OTrdt/H2OTr/H2OTr/H2OTr/H2OTr;
    dbEnddt[CO2] = - 2.0*CO2Ha2*dCO2Trdt/CO2Tr/CO2Tr/CO2Tr - 3.0*CO2Ha3*dCO2Trdt/CO2Tr/CO2Tr/CO2Tr/CO2Tr;
    dk1dt = - 7.9212e-3 + 2.0*2.3285e-6*t + 2.4221e3/t/t;
    d2bEnddt2[H2O] = 6.0*H2OHa2*dH2OTrdt*dH2OTrdt/H2OTr/H2OTr/H2OTr/H2OTr + 12.0*H2OHa3*dH2OTrdt*dH2OTrdt/H2OTr/H2OTr/H2OTr/H2OTr/H2OTr;
    d2bEnddt2[CO2] = 6.0*CO2Ha2*dCO2Trdt*dCO2Trdt/CO2Tr/CO2Tr/CO2Tr/CO2Tr + 12.0*CO2Ha3*dCO2Trdt*dCO2Trdt/CO2Tr/CO2Tr/CO2Tr/CO2Tr/CO2Tr;
    d2k1dt2 = 2.0*2.3285e-6 - 2.0*2.4221e3/t/t/t;
    d3bEnddt3[H2O] = - 24.0*H2OHa2*dH2OTrdt*dH2OTrdt*dH2OTrdt/H2OTr/H2OTr/H2OTr/H2OTr/H2OTr - 60.0*H2OHa3*dH2OTrdt*dH2OTrdt*dH2OTrdt/H2OTr/H2OTr/H2OTr/H2OTr/H2OTr/H2OTr;
    d3bEnddt3[CO2] = - 24.0*CO2Ha2*dCO2Trdt*dCO2Trdt*dCO2Trdt/CO2Tr/CO2Tr/CO2Tr/CO2Tr/CO2Tr - 60.0*CO2Ha3*dCO2Trdt*dCO2Trdt*dCO2Trdt/CO2Tr/CO2Tr/CO2Tr/CO2Tr/CO2Tr/CO2Tr;
    d3k1dt3 = 6.0*2.4221e3/t/t/t/t;
  }
  
  if ((x[H2O] == 0.0) && (x[CO2] == 0.0)) {
    *bv = 0.0;
    bvPrime[H2O] = 0.0;
    bvPrime[CO2] = 0.0;
    
    *dbvdt   = 0.0;
    *d2bvdt2 = 0.0;
    *d3bvdt3 = 0.0;
    
    dbvPrimedt[H2O] = 0.0;
    dbvPrimedt[CO2] = 0.0; 
       
    d2bvPrimedt2[H2O] = 0.0;
    d2bvPrimedt2[CO2] = 0.0;  
      
    d3bvPrimedt3[H2O] = 0.0;
    d3bvPrimedt3[CO2] = 0.0;
    
  } else if ((x[H2O] == 1.0) && (x[CO2] == 0.0)) {
    *bv = bEnd[H2O]*H2OVc;
    bvPrime[H2O] = 2.0*bEnd[H2O]*H2OVc;
    bvPrime[CO2] = 2.0*powSum(bEnd[H2O], 1.0, bEnd[CO2], 1.0)*k1*powSum(H2OVc, 1.0, CO2Vc, 1.0);
    
    *dbvdt   = dbEnddt[H2O]*H2OVc;
    *d2bvdt2 = d2bEnddt2[H2O]*H2OVc;
    *d3bvdt3 = d3bEnddt3[H2O]*H2OVc;
    
    dbvPrimedt[H2O] = 2.0*dbEnddt[H2O]*H2OVc;
    dbvPrimedt[CO2] = 2.0*(  DpowSum(bEnd[H2O], 1.0, bEnd[CO2], 1.0, dbEnddt[H2O], dbEnddt[CO2])*k1
                           + powSum(bEnd[H2O], 1.0, bEnd[CO2], 1.0)*dk1dt
                          )*powSum(H2OVc, 1.0, CO2Vc, 1.0);
    
    d2bvPrimedt2[H2O] = 2.0*d2bEnddt2[H2O]*H2OVc;
    d2bvPrimedt2[CO2] = 2.0*(      D2powSum(bEnd[H2O], 1.0, bEnd[CO2], 1.0, dbEnddt[H2O], dbEnddt[CO2], d2bEnddt2[H2O], d2bEnddt2[CO2])*k1
                             + 2.0*DpowSum(bEnd[H2O], 1.0, bEnd[CO2], 1.0, dbEnddt[H2O], dbEnddt[CO2])*dk1dt
                             +     powSum(bEnd[H2O], 1.0, bEnd[CO2], 1.0)*d2k1dt2
                            )*powSum(H2OVc, 1.0, CO2Vc, 1.0);
    
    d3bvPrimedt3[H2O] = 2.0*d3bEnddt3[H2O]*H2OVc;
    d3bvPrimedt3[CO2] = 2.0*(      D3powSum(bEnd[H2O], 1.0, bEnd[CO2], 1.0, dbEnddt[H2O], dbEnddt[CO2], d2bEnddt2[H2O], d2bEnddt2[CO2], d3bEnddt3[H2O], d3bEnddt3[CO2])*k1
    			     + 3.0*D2powSum(bEnd[H2O], 1.0, bEnd[CO2], 1.0, dbEnddt[H2O], dbEnddt[CO2], d2bEnddt2[H2O], d2bEnddt2[CO2])*dk1dt
			     + 3.0*DpowSum(bEnd[H2O], 1.0, bEnd[CO2], 1.0, dbEnddt[H2O], dbEnddt[CO2])*d2k1dt2
			     +     powSum(bEnd[H2O], 1.0, bEnd[CO2], 1.0)*d3k1dt3
    			    )*powSum(H2OVc, 1.0, CO2Vc, 1.0);
    
  } else if ((x[H2O] == 0.0) && (x[CO2] == 1.0)) {
    *bv = bEnd[CO2]*CO2Vc;
    bvPrime[H2O] = 2.0*powSum(bEnd[H2O], 1.0, bEnd[CO2], 1.0)*k1*powSum(H2OVc, 1.0, CO2Vc, 1.0);
    bvPrime[CO2] = 2.0*bEnd[CO2]*CO2Vc;
    
    *dbvdt   = dbEnddt[CO2]*CO2Vc;
    *d2bvdt2 = d2bEnddt2[CO2]*CO2Vc;
    *d3bvdt3 = d3bEnddt3[CO2]*CO2Vc;
    
    dbvPrimedt[H2O] = 2.0*(  DpowSum(bEnd[H2O], 1.0, bEnd[CO2], 1.0, dbEnddt[H2O], dbEnddt[CO2])*k1
                           + powSum(bEnd[H2O], 1.0, bEnd[CO2], 1.0)*dk1dt
                          )*powSum(H2OVc, 1.0, CO2Vc, 1.0);
    dbvPrimedt[CO2] = 2.0*dbEnddt[CO2]*CO2Vc;
    
    d2bvPrimedt2[H2O] = 2.0*(      D2powSum(bEnd[H2O], 1.0, bEnd[CO2], 1.0, dbEnddt[H2O], dbEnddt[CO2], d2bEnddt2[H2O], d2bEnddt2[CO2])*k1
                             + 2.0*DpowSum(bEnd[H2O], 1.0, bEnd[CO2], 1.0, dbEnddt[H2O], dbEnddt[CO2])*dk1dt
                             +     powSum(bEnd[H2O], 1.0, bEnd[CO2], 1.0)*d2k1dt2
                            )*powSum(H2OVc, 1.0, CO2Vc, 1.0);
    d2bvPrimedt2[CO2] = 2.0*d2bEnddt2[CO2]*CO2Vc;
    
    d3bvPrimedt3[H2O] = 2.0*(      D3powSum(bEnd[H2O], 1.0, bEnd[CO2], 1.0, dbEnddt[H2O], dbEnddt[CO2], d2bEnddt2[H2O], d2bEnddt2[CO2], d3bEnddt3[H2O], d3bEnddt3[CO2])*k1
                             + 3.0*D2powSum(bEnd[H2O], 1.0, bEnd[CO2], 1.0, dbEnddt[H2O], dbEnddt[CO2], d2bEnddt2[H2O], d2bEnddt2[CO2])*dk1dt
                             + 3.0*DpowSum(bEnd[H2O], 1.0, bEnd[CO2], 1.0, dbEnddt[H2O], dbEnddt[CO2])*d2k1dt2
                             +     powSum(bEnd[H2O], 1.0, bEnd[CO2], 1.0)*d3k1dt3
                            )*powSum(H2OVc, 1.0, CO2Vc, 1.0);
    d3bvPrimedt3[CO2] = 2.0*d3bEnddt3[CO2]*CO2Vc;
    
  } else {
    *bv  = 0.0;
    *bv += bEnd[H2O]*H2OVc*x[H2O]*x[H2O];
    *bv += 2.0*powSum(bEnd[H2O], 1.0, bEnd[CO2], 1.0)*k1*powSum(H2OVc, 1.0, CO2Vc, 1.0)*x[H2O]*x[CO2];
    *bv += bEnd[CO2]*CO2Vc*x[CO2]*x[CO2];
  
    bvPrime[H2O]  = 0.0;
    bvPrime[H2O] += 2.0*bEnd[H2O]*H2OVc*x[H2O];
    bvPrime[H2O] += 2.0*powSum(bEnd[H2O], 1.0, bEnd[CO2], 1.0)*k1*powSum(H2OVc, 1.0, CO2Vc, 1.0)*x[CO2];

    bvPrime[CO2]  = 0.0;
    bvPrime[CO2] += 2.0*powSum(bEnd[H2O], 1.0, bEnd[CO2], 1.0)*k1*powSum(H2OVc, 1.0, CO2Vc, 1.0)*x[H2O];
    bvPrime[CO2] += 2.0*bEnd[CO2]*CO2Vc*x[CO2];
    
    *dbvdt  = 0.0;
    *dbvdt += dbEnddt[H2O]*H2OVc*x[H2O]*x[H2O];
    *dbvdt += 2.0*(DpowSum(bEnd[H2O], 1.0, bEnd[CO2], 1.0, dbEnddt[H2O], dbEnddt[CO2])*k1 + powSum(bEnd[H2O], 1.0, bEnd[CO2], 1.0)*dk1dt
		  )*powSum(H2OVc, 1.0, CO2Vc, 1.0)*x[H2O]*x[CO2];
    *dbvdt += dbEnddt[CO2]*CO2Vc*x[CO2]*x[CO2];
    
    *d2bvdt2  = 0.0;
    *d2bvdt2 += d2bEnddt2[H2O]*H2OVc*x[H2O]*x[H2O];
    *d2bvdt2 += 2.0*(  D2powSum(bEnd[H2O], 1.0, bEnd[CO2], 1.0, dbEnddt[H2O], dbEnddt[CO2], d2bEnddt2[H2O], d2bEnddt2[CO2])*k1 
                     + 2.0*DpowSum(bEnd[H2O], 1.0, bEnd[CO2], 1.0, dbEnddt[H2O], dbEnddt[CO2])*dk1dt
                     + powSum(bEnd[H2O], 1.0, bEnd[CO2], 1.0)*d2k1dt2
		    )*powSum(H2OVc, 1.0, CO2Vc, 1.0)*x[H2O]*x[CO2];
    *d2bvdt2 += d2bEnddt2[CO2]*CO2Vc*x[CO2]*x[CO2];

    *d3bvdt3  = 0.0;
    *d3bvdt3 += d3bEnddt3[H2O]*H2OVc*x[H2O]*x[H2O];
    *d3bvdt3 += 2.0*(      D3powSum(bEnd[H2O], 1.0, bEnd[CO2], 1.0, dbEnddt[H2O], dbEnddt[CO2], d2bEnddt2[H2O], d2bEnddt2[CO2], d3bEnddt3[H2O], d3bEnddt3[CO2])*k1 
                     + 3.0*D2powSum(bEnd[H2O], 1.0, bEnd[CO2], 1.0, dbEnddt[H2O], dbEnddt[CO2], d2bEnddt2[H2O], d2bEnddt2[CO2])*dk1dt
		     + 3.0*DpowSum(bEnd[H2O], 1.0, bEnd[CO2], 1.0, dbEnddt[H2O], dbEnddt[CO2])*d2k1dt2
		     +     powSum(bEnd[H2O], 1.0, bEnd[CO2], 1.0)*d3k1dt3
		    )*powSum(H2OVc, 1.0, CO2Vc, 1.0)*x[H2O]*x[CO2];
    *d3bvdt3 += d3bEnddt3[CO2]*CO2Vc*x[CO2]*x[CO2];

    dbvPrimedt[H2O]  = 0.0;
    dbvPrimedt[H2O] += 2.0*dbEnddt[H2O]*H2OVc*x[H2O];
    dbvPrimedt[H2O] += 2.0*(  DpowSum(bEnd[H2O], 1.0, bEnd[CO2], 1.0, dbEnddt[H2O], dbEnddt[CO2])*k1
                            + powSum(bEnd[H2O], 1.0, bEnd[CO2], 1.0)*dk1dt
                           )*powSum(H2OVc, 1.0, CO2Vc, 1.0)*x[CO2];

    dbvPrimedt[CO2]  = 0.0;
    dbvPrimedt[CO2] += 2.0*(  DpowSum(bEnd[H2O], 1.0, bEnd[CO2], 1.0, dbEnddt[H2O], dbEnddt[CO2])*k1
                            + powSum(bEnd[H2O], 1.0, bEnd[CO2], 1.0)*dk1dt
                           )*powSum(H2OVc, 1.0, CO2Vc, 1.0)*x[H2O];
    dbvPrimedt[CO2] += 2.0*dbEnddt[CO2]*CO2Vc*x[CO2];
    
    d2bvPrimedt2[H2O]  = 0.0;
    d2bvPrimedt2[H2O] += 2.0*d2bEnddt2[H2O]*H2OVc*x[H2O];
    d2bvPrimedt2[H2O] += 2.0*(      D2powSum(bEnd[H2O], 1.0, bEnd[CO2], 1.0, dbEnddt[H2O], dbEnddt[CO2], d2bEnddt2[H2O], d2bEnddt2[CO2])*k1
                              + 2.0*DpowSum(bEnd[H2O], 1.0, bEnd[CO2], 1.0, dbEnddt[H2O], dbEnddt[CO2])*dk1dt
                              +     powSum(bEnd[H2O], 1.0, bEnd[CO2], 1.0)*d2k1dt2
                           )*powSum(H2OVc, 1.0, CO2Vc, 1.0)*x[CO2];

    d2bvPrimedt2[CO2]  = 0.0;
    d2bvPrimedt2[CO2] += 2.0*(      D2powSum(bEnd[H2O], 1.0, bEnd[CO2], 1.0, dbEnddt[H2O], dbEnddt[CO2], d2bEnddt2[H2O], d2bEnddt2[CO2])*k1
                              + 2.0*DpowSum(bEnd[H2O], 1.0, bEnd[CO2], 1.0, dbEnddt[H2O], dbEnddt[CO2])*dk1dt
                              +     powSum(bEnd[H2O], 1.0, bEnd[CO2], 1.0)*d2k1dt2
                           )*powSum(H2OVc, 1.0, CO2Vc, 1.0)*x[H2O];
    d2bvPrimedt2[CO2] += 2.0*d2bEnddt2[CO2]*CO2Vc*x[CO2];
    
    d3bvPrimedt3[H2O]  = 0.0;
    d3bvPrimedt3[H2O] += 2.0*d3bEnddt3[H2O]*H2OVc*x[H2O];
    d3bvPrimedt3[H2O] += 2.0*(      D3powSum(bEnd[H2O], 1.0, bEnd[CO2], 1.0, dbEnddt[H2O], dbEnddt[CO2], d2bEnddt2[H2O], d2bEnddt2[CO2], d3bEnddt3[H2O], d3bEnddt3[CO2])*k1
                              + 3.0*D2powSum(bEnd[H2O], 1.0, bEnd[CO2], 1.0, dbEnddt[H2O], dbEnddt[CO2], d2bEnddt2[H2O], d2bEnddt2[CO2])*dk1dt
                              + 3.0*DpowSum(bEnd[H2O], 1.0, bEnd[CO2], 1.0, dbEnddt[H2O], dbEnddt[CO2])*d2k1dt2
                              +     powSum(bEnd[H2O], 1.0, bEnd[CO2], 1.0)*d3k1dt3
                           )*powSum(H2OVc, 1.0, CO2Vc, 1.0)*x[CO2];

    d3bvPrimedt3[CO2]  = 0.0;
    d3bvPrimedt3[CO2] += 2.0*(      D3powSum(bEnd[H2O], 1.0, bEnd[CO2], 1.0, dbEnddt[H2O], dbEnddt[CO2], d2bEnddt2[H2O], d2bEnddt2[CO2], d3bEnddt3[H2O], d3bEnddt3[CO2])*k1
                              + 3.0*D2powSum(bEnd[H2O], 1.0, bEnd[CO2], 1.0, dbEnddt[H2O], dbEnddt[CO2], d2bEnddt2[H2O], d2bEnddt2[CO2])*dk1dt
                              + 3.0*DpowSum(bEnd[H2O], 1.0, bEnd[CO2], 1.0, dbEnddt[H2O], dbEnddt[CO2])*d2k1dt2
                              +     powSum(bEnd[H2O], 1.0, bEnd[CO2], 1.0)*d3k1dt3
                           )*powSum(H2OVc, 1.0, CO2Vc, 1.0)*x[H2O];
    d3bvPrimedt3[CO2] += 2.0*d3bEnddt3[CO2]*CO2Vc*x[CO2];
    
  }
  
  return;
}

static void CVcAndDerivative(int useLowPcoeff, double t, double x[2], double *cv, double cvPrime[2], double *dcvdt, double *d2cvdt2) {
  double cEnd[2], dcEnddt[2], d2cEnddt2[2];
  double H2OTr    = t/H2OTc;
  double CO2Tr    = t/CO2Tc;
  double k2       = 0.0;
  double dH2OTrdt = 1.0/H2OTc;
  double dCO2Trdt = 1.0/CO2Tc;
  double dk2dt    = 0.0;
  double d2k2dt2  = 0.0;

  if (useLowPcoeff) {
    cEnd[H2O] = H2OLa4 + H2OLa5/H2OTr/H2OTr + H2OLa6/H2OTr/H2OTr/H2OTr;
    cEnd[CO2] = CO2La4 + CO2La5/CO2Tr/CO2Tr + CO2La6/CO2Tr/CO2Tr/CO2Tr;
    k2 = -46.646 + 4.2877e-2*t - 1.0892e-5*t*t + 1.5782e4/t;
    dcEnddt[H2O] = - 2.0*H2OLa5*dH2OTrdt/H2OTr/H2OTr/H2OTr - 3.0*H2OLa6*dH2OTrdt/H2OTr/H2OTr/H2OTr/H2OTr;
    dcEnddt[CO2] = - 2.0*CO2La5*dCO2Trdt/CO2Tr/CO2Tr/CO2Tr - 3.0*CO2La6*dCO2Trdt/CO2Tr/CO2Tr/CO2Tr/CO2Tr;
    dk2dt = 4.2877e-2 - 2.0*1.0892e-5*t - 1.5782e4/t/t;
    d2cEnddt2[H2O] = 6.0*H2OLa5*dH2OTrdt*dH2OTrdt/H2OTr/H2OTr/H2OTr/H2OTr + 12.0*H2OLa6*dH2OTrdt*dH2OTrdt/H2OTr/H2OTr/H2OTr/H2OTr/H2OTr;
    d2cEnddt2[CO2] = 6.0*CO2La5*dCO2Trdt*dCO2Trdt/CO2Tr/CO2Tr/CO2Tr/CO2Tr + 12.0*CO2La6*dCO2Trdt*dCO2Trdt/CO2Tr/CO2Tr/CO2Tr/CO2Tr/CO2Tr;
    d2k2dt2 = - 2.0*1.0892e-5 + 2.0*1.5782e4/t/t/t;
  } else {
    cEnd[H2O] = H2OHa4 + H2OHa5/H2OTr/H2OTr + H2OHa6/H2OTr/H2OTr/H2OTr;
    cEnd[CO2] = CO2Ha4 + CO2Ha5/CO2Tr/CO2Tr + CO2Ha6/CO2Tr/CO2Tr/CO2Tr;
    k2 = -1.068 + 1.8756e-3*t - 4.9371e-7*t*t + 6.6180e2/t;
    dcEnddt[H2O] = - 2.0*H2OHa5*dH2OTrdt/H2OTr/H2OTr/H2OTr - 3.0*H2OHa6*dH2OTrdt/H2OTr/H2OTr/H2OTr/H2OTr;
    dcEnddt[CO2] = - 2.0*CO2Ha5*dCO2Trdt/CO2Tr/CO2Tr/CO2Tr - 3.0*CO2Ha6*dCO2Trdt/CO2Tr/CO2Tr/CO2Tr/CO2Tr;
    dk2dt = 1.8756e-3 - 2.0*4.9371e-7*t - 6.6180e2/t/t;
    d2cEnddt2[H2O] = 6.0*H2OHa5*dH2OTrdt*dH2OTrdt/H2OTr/H2OTr/H2OTr/H2OTr + 12.0*H2OHa6*dH2OTrdt*dH2OTrdt/H2OTr/H2OTr/H2OTr/H2OTr/H2OTr;
    d2cEnddt2[CO2] = 6.0*CO2Ha5*dCO2Trdt*dCO2Trdt/CO2Tr/CO2Tr/CO2Tr/CO2Tr + 12.0*CO2Ha6*dCO2Trdt*dCO2Trdt/CO2Tr/CO2Tr/CO2Tr/CO2Tr/CO2Tr;
    d2k2dt2 = - 2.0*4.9371e-7 + 2.0*6.6180e2/t/t/t;
  }
  
  if ((x[H2O] == 0.0) && (x[CO2] == 0.0)) {
    *cv = 0.0;
    cvPrime[H2O] = 0.0;
    cvPrime[CO2] = 0.0;
    
    *dcvdt   = 0.0;
    *d2cvdt2 = 0.0;
    
  } else if ((x[H2O] == 1.0) && (x[CO2] == 0.0)) {
    *cv = cEnd[H2O]*H2OVc*H2OVc;
    cvPrime[H2O] = 3.0*cEnd[H2O]*H2OVc*H2OVc;
    cvPrime[CO2] = 3.0*powSum(cEnd[H2O], 2.0, cEnd[CO2], 1.0)*k2*pow(powSum(H2OVc, 2.0, CO2Vc, 1.0), 2.0);

    *dcvdt   = dcEnddt[H2O]*H2OVc*H2OVc;
    *d2cvdt2 = d2cEnddt2[H2O]*H2OVc*H2OVc;
    
  } else if ((x[H2O] == 0.0) && (x[CO2] == 1.0)) {
    *cv = cEnd[CO2]*CO2Vc*CO2Vc;
    cvPrime[H2O] = 3.0*powSum(cEnd[H2O], 1.0, cEnd[CO2], 2.0)*k2*pow(powSum(H2OVc, 1.0, CO2Vc, 2.0), 2.0);
    cvPrime[CO2] = 3.0*cEnd[CO2]*CO2Vc*CO2Vc;

    *dcvdt   = dcEnddt[CO2]*CO2Vc*CO2Vc;
    *d2cvdt2 = d2cEnddt2[CO2]*CO2Vc*CO2Vc;
    
  } else {    
    *cv  = 0.0;
    *cv += cEnd[H2O]*H2OVc*H2OVc*x[H2O]*x[H2O]*x[H2O];
    *cv += 3.0*powSum(cEnd[H2O], 2.0, cEnd[CO2], 1.0)*k2*pow( powSum(H2OVc, 2.0, CO2Vc, 1.0), 2.0)*x[H2O]*x[H2O]*x[CO2];
    *cv += 3.0*powSum(cEnd[H2O], 1.0, cEnd[CO2], 2.0)*k2*pow( powSum(H2OVc, 1.0, CO2Vc, 2.0), 2.0)*x[H2O]*x[CO2]*x[CO2];      
    *cv += cEnd[CO2]*CO2Vc*CO2Vc*x[CO2]*x[CO2]*x[CO2];
  
    cvPrime[H2O]  = 0.0;
    cvPrime[H2O] += 3.0*cEnd[H2O]*H2OVc*H2OVc*x[H2O]*x[H2O];
    cvPrime[H2O] += 6.0*powSum(cEnd[H2O], 2.0, cEnd[CO2], 1.0)*k2*pow(powSum(H2OVc, 2.0, CO2Vc, 1.0), 2.0)*x[H2O]*x[CO2];
    cvPrime[H2O] += 3.0*powSum(cEnd[H2O], 1.0, cEnd[CO2], 2.0)*k2*pow(powSum(H2OVc, 1.0, CO2Vc, 2.0), 2.0)*x[CO2]*x[CO2];

    cvPrime[CO2]  = 0.0;
    cvPrime[CO2] += 3.0*powSum(cEnd[H2O], 2.0, cEnd[CO2], 1.0)*k2*pow(powSum(H2OVc, 2.0, CO2Vc, 1.0), 2.0)*x[H2O]*x[H2O];
    cvPrime[CO2] += 6.0*powSum(cEnd[H2O], 1.0, cEnd[CO2], 2.0)*k2*pow(powSum(H2OVc, 1.0, CO2Vc, 2.0), 2.0)*x[H2O]*x[CO2];
    cvPrime[CO2] += 3.0*cEnd[CO2]*CO2Vc*CO2Vc*x[CO2]*x[CO2];

    *dcvdt  = 0.0;
    *dcvdt += dcEnddt[H2O]*H2OVc*H2OVc*x[H2O]*x[H2O]*x[H2O];
    *dcvdt += 3.0*(DpowSum(cEnd[H2O], 2.0, cEnd[CO2], 1.0, dcEnddt[H2O], dcEnddt[CO2])*k2 + powSum(cEnd[H2O], 2.0, cEnd[CO2], 1.0)*dk2dt
                  )*pow( powSum(H2OVc, 2.0, CO2Vc, 1.0), 2.0)*x[H2O]*x[H2O]*x[CO2];
    *dcvdt += 3.0*(DpowSum(cEnd[H2O], 1.0, cEnd[CO2], 2.0, dcEnddt[H2O], dcEnddt[CO2])*k2 + powSum(cEnd[H2O], 1.0, cEnd[CO2], 2.0)*dk2dt
                  )*pow( powSum(H2OVc, 1.0, CO2Vc, 2.0), 2.0)*x[H2O]*x[CO2]*x[CO2];      
    *dcvdt += dcEnddt[CO2]*CO2Vc*CO2Vc*x[CO2]*x[CO2]*x[CO2];

    *d2cvdt2  = 0.0;
    *d2cvdt2 += d2cEnddt2[H2O]*H2OVc*H2OVc*x[H2O]*x[H2O]*x[H2O];
    *d2cvdt2 += 3.0*(  D2powSum(cEnd[H2O], 2.0, cEnd[CO2], 1.0, dcEnddt[H2O], dcEnddt[CO2], d2cEnddt2[H2O], d2cEnddt2[CO2])*k2 
                     + 2.0*DpowSum(cEnd[H2O], 2.0, cEnd[CO2], 1.0, dcEnddt[H2O], dcEnddt[CO2])*dk2dt
                     + powSum(cEnd[H2O], 2.0, cEnd[CO2], 1.0)*d2k2dt2
                    )*pow( powSum(H2OVc, 2.0, CO2Vc, 1.0), 2.0)*x[H2O]*x[H2O]*x[CO2];
    *d2cvdt2 += 3.0*(  D2powSum(cEnd[H2O], 1.0, cEnd[CO2], 2.0, dcEnddt[H2O], dcEnddt[CO2], d2cEnddt2[H2O], d2cEnddt2[CO2])*k2 
                     + 2.0*DpowSum(cEnd[H2O], 1.0, cEnd[CO2], 2.0, dcEnddt[H2O], dcEnddt[CO2])*dk2dt
                     + powSum(cEnd[H2O], 1.0, cEnd[CO2], 2.0)*d2k2dt2
                    )*pow( powSum(H2OVc, 1.0, CO2Vc, 2.0), 2.0)*x[H2O]*x[CO2]*x[CO2];      
    *d2cvdt2 += d2cEnddt2[CO2]*CO2Vc*CO2Vc*x[CO2]*x[CO2]*x[CO2];
  }
  
  return;
}

static void DVcAndDerivative(int useLowPcoeff, double t, double x[2], double *dv, double dvPrime[2], double *ddvdt, double *d2dvdt2) {
  double dEnd[2], ddEnddt[2], d2dEnddt2[2];
  double H2OTr = t/H2OTc;
  double CO2Tr = t/CO2Tc;
  double dH2OTrdt = 1.0/H2OTc;
  double dCO2Trdt = 1.0/CO2Tc;

  if (useLowPcoeff) {
    dEnd[H2O] = H2OLa7 + H2OLa8/H2OTr/H2OTr + H2OLa9/H2OTr/H2OTr/H2OTr;
    dEnd[CO2] = CO2La7 + CO2La8/CO2Tr/CO2Tr + CO2La9/CO2Tr/CO2Tr/CO2Tr;
    ddEnddt[H2O] = - 2.0*H2OLa8*dH2OTrdt/H2OTr/H2OTr/H2OTr - 3.0*H2OLa9*dH2OTrdt/H2OTr/H2OTr/H2OTr/H2OTr;
    ddEnddt[CO2] = - 2.0*CO2La8*dCO2Trdt/CO2Tr/CO2Tr/CO2Tr - 3.0*CO2La9*dCO2Trdt/CO2Tr/CO2Tr/CO2Tr/CO2Tr;
    d2dEnddt2[H2O] = 6.0*H2OLa8*dH2OTrdt*dH2OTrdt/H2OTr/H2OTr/H2OTr/H2OTr + 12.0*H2OLa9*dH2OTrdt*dH2OTrdt/H2OTr/H2OTr/H2OTr/H2OTr/H2OTr;
    d2dEnddt2[CO2] = 6.0*CO2La8*dCO2Trdt*dCO2Trdt/CO2Tr/CO2Tr/CO2Tr/CO2Tr + 12.0*CO2La9*dCO2Trdt*dCO2Trdt/CO2Tr/CO2Tr/CO2Tr/CO2Tr/CO2Tr;
  } else {
    dEnd[H2O] = H2OHa7 + H2OHa8/H2OTr/H2OTr + H2OHa9/H2OTr/H2OTr/H2OTr;
    dEnd[CO2] = CO2Ha7 + CO2Ha8/CO2Tr/CO2Tr + CO2Ha9/CO2Tr/CO2Tr/CO2Tr;
    ddEnddt[H2O] = - 2.0*H2OHa8*dH2OTrdt/H2OTr/H2OTr/H2OTr - 3.0*H2OHa9*dH2OTrdt/H2OTr/H2OTr/H2OTr/H2OTr;
    ddEnddt[CO2] = - 2.0*CO2Ha8*dCO2Trdt/CO2Tr/CO2Tr/CO2Tr - 3.0*CO2Ha9*dCO2Trdt/CO2Tr/CO2Tr/CO2Tr/CO2Tr;
    d2dEnddt2[H2O] = 6.0*H2OHa8*dH2OTrdt*dH2OTrdt/H2OTr/H2OTr/H2OTr/H2OTr + 12.0*H2OHa9*dH2OTrdt*dH2OTrdt/H2OTr/H2OTr/H2OTr/H2OTr/H2OTr;
    d2dEnddt2[CO2] = 6.0*CO2Ha8*dCO2Trdt*dCO2Trdt/CO2Tr/CO2Tr/CO2Tr/CO2Tr + 12.0*CO2Ha9*dCO2Trdt*dCO2Trdt/CO2Tr/CO2Tr/CO2Tr/CO2Tr/CO2Tr;
  }
  
  if ((x[H2O] == 0.0) && (x[CO2] == 0.0)) {
    *dv = 0.0;
    dvPrime[H2O] = 0.0;
    dvPrime[CO2] = 0.0;

    *ddvdt   = 0.0;
    *d2dvdt2 = 0.0;
    
  } else if ((x[H2O] == 1.0) && (x[CO2] == 0.0)) {
    *dv = dEnd[H2O]*H2OVc*H2OVc*H2OVc*H2OVc;
    dvPrime[H2O] = 5.0*dEnd[H2O]*H2OVc*H2OVc*H2OVc*H2OVc;
    dvPrime[CO2] = 5.0*powSum(dEnd[H2O], 1.0, dEnd[CO2], 4.0)*pow(powSum(H2OVc, 1.0, CO2Vc, 4.0), 4.0);

    *ddvdt   = ddEnddt[H2O]*H2OVc*H2OVc*H2OVc*H2OVc;
    *d2dvdt2 = d2dEnddt2[H2O]*H2OVc*H2OVc*H2OVc*H2OVc;
    
  } else if ((x[H2O] == 0.0) && (x[CO2] == 1.0)) {
    *dv = dEnd[CO2]*CO2Vc*CO2Vc*CO2Vc*CO2Vc;
    dvPrime[H2O] = 5.0*powSum(dEnd[H2O], 4.0, dEnd[CO2], 1.0)*pow(powSum(H2OVc, 4.0, CO2Vc, 1.0), 4.0);
    dvPrime[CO2] = 5.0*dEnd[CO2]*CO2Vc*CO2Vc*CO2Vc*CO2Vc;

    *ddvdt   = ddEnddt[CO2]*CO2Vc*CO2Vc*CO2Vc*CO2Vc;
    *d2dvdt2 = d2dEnddt2[CO2]*CO2Vc*CO2Vc*CO2Vc*CO2Vc;
    
  } else {    
    *dv  = 0.0;
    *dv += dEnd[H2O]*H2OVc*H2OVc*H2OVc*H2OVc*x[H2O]*x[H2O]*x[H2O]*x[H2O]*x[H2O];
    *dv +=  5.0*powSum(dEnd[H2O], 4.0, dEnd[CO2], 1.0)*pow(powSum(H2OVc, 4.0, CO2Vc, 1.0), 4.0)*x[H2O]*x[H2O]*x[H2O]*x[H2O]*x[CO2];
    *dv += 10.0*powSum(dEnd[H2O], 3.0, dEnd[CO2], 2.0)*pow(powSum(H2OVc, 3.0, CO2Vc, 2.0), 4.0)*x[H2O]*x[H2O]*x[H2O]*x[CO2]*x[CO2];
    *dv += 10.0*powSum(dEnd[H2O], 2.0, dEnd[CO2], 3.0)*pow(powSum(H2OVc, 2.0, CO2Vc, 3.0), 4.0)*x[H2O]*x[H2O]*x[CO2]*x[CO2]*x[CO2];
    *dv +=  5.0*powSum(dEnd[H2O], 1.0, dEnd[CO2], 4.0)*pow(powSum(H2OVc, 1.0, CO2Vc, 4.0), 4.0)*x[H2O]*x[CO2]*x[CO2]*x[CO2]*x[CO2];
    *dv += dEnd[CO2]*CO2Vc*CO2Vc*CO2Vc*CO2Vc*x[CO2]*x[CO2]*x[CO2]*x[CO2]*x[CO2];
  
    dvPrime[H2O]  =  0.0;
    dvPrime[H2O] +=  5.0*dEnd[H2O]*H2OVc*H2OVc*H2OVc*H2OVc*x[H2O]*x[H2O]*x[H2O]*x[H2O];
    dvPrime[H2O] += 20.0*powSum(dEnd[H2O], 4.0, dEnd[CO2], 1.0)*pow(powSum(H2OVc, 4.0, CO2Vc, 1.0), 4.0)*x[H2O]*x[H2O]*x[H2O]*x[CO2];
    dvPrime[H2O] += 30.0*powSum(dEnd[H2O], 3.0, dEnd[CO2], 2.0)*pow(powSum(H2OVc, 3.0, CO2Vc, 2.0), 4.0)*x[H2O]*x[H2O]*x[CO2]*x[CO2];
    dvPrime[H2O] += 20.0*powSum(dEnd[H2O], 2.0, dEnd[CO2], 3.0)*pow(powSum(H2OVc, 2.0, CO2Vc, 3.0), 4.0)*x[H2O]*x[CO2]*x[CO2]*x[CO2];
    dvPrime[H2O] +=  5.0*powSum(dEnd[H2O], 1.0, dEnd[CO2], 4.0)*pow(powSum(H2OVc, 1.0, CO2Vc, 4.0), 4.0)*x[CO2]*x[CO2]*x[CO2]*x[CO2];

    dvPrime[CO2]  =  0.0;
    dvPrime[CO2] +=  5.0*powSum(dEnd[H2O], 4.0, dEnd[CO2], 1.0)*pow(powSum(H2OVc, 4.0, CO2Vc, 1.0), 4.0)*x[H2O]*x[H2O]*x[H2O]*x[H2O];
    dvPrime[CO2] += 20.0*powSum(dEnd[H2O], 3.0, dEnd[CO2], 2.0)*pow(powSum(H2OVc, 3.0, CO2Vc, 2.0), 4.0)*x[H2O]*x[H2O]*x[H2O]*x[CO2];
    dvPrime[CO2] += 30.0*powSum(dEnd[H2O], 2.0, dEnd[CO2], 3.0)*pow(powSum(H2OVc, 2.0, CO2Vc, 3.0), 4.0)*x[H2O]*x[H2O]*x[CO2]*x[CO2];
    dvPrime[CO2] += 20.0*powSum(dEnd[H2O], 1.0, dEnd[CO2], 4.0)*pow(powSum(H2OVc, 1.0, CO2Vc, 4.0), 4.0)*x[H2O]*x[CO2]*x[CO2]*x[CO2];
    dvPrime[CO2] += 5.0*dEnd[CO2]*CO2Vc*CO2Vc*CO2Vc*CO2Vc*x[CO2]*x[CO2]*x[CO2]*x[CO2];

    *ddvdt  = 0.0;
    *ddvdt += ddEnddt[H2O]*H2OVc*H2OVc*H2OVc*H2OVc*x[H2O]*x[H2O]*x[H2O]*x[H2O]*x[H2O];
    *ddvdt +=  5.0*DpowSum(dEnd[H2O], 4.0, dEnd[CO2], 1.0, ddEnddt[H2O], ddEnddt[CO2])*pow(powSum(H2OVc, 4.0, CO2Vc, 1.0), 4.0)*x[H2O]*x[H2O]*x[H2O]*x[H2O]*x[CO2];
    *ddvdt += 10.0*DpowSum(dEnd[H2O], 3.0, dEnd[CO2], 2.0, ddEnddt[H2O], ddEnddt[CO2])*pow(powSum(H2OVc, 3.0, CO2Vc, 2.0), 4.0)*x[H2O]*x[H2O]*x[H2O]*x[CO2]*x[CO2];
    *ddvdt += 10.0*DpowSum(dEnd[H2O], 2.0, dEnd[CO2], 3.0, ddEnddt[H2O], ddEnddt[CO2])*pow(powSum(H2OVc, 2.0, CO2Vc, 3.0), 4.0)*x[H2O]*x[H2O]*x[CO2]*x[CO2]*x[CO2];
    *ddvdt +=  5.0*DpowSum(dEnd[H2O], 1.0, dEnd[CO2], 4.0, ddEnddt[H2O], ddEnddt[CO2])*pow(powSum(H2OVc, 1.0, CO2Vc, 4.0), 4.0)*x[H2O]*x[CO2]*x[CO2]*x[CO2]*x[CO2];
    *ddvdt += ddEnddt[CO2]*CO2Vc*CO2Vc*CO2Vc*CO2Vc*x[CO2]*x[CO2]*x[CO2]*x[CO2]*x[CO2];

    *d2dvdt2  = 0.0;
    *d2dvdt2 += d2dEnddt2[H2O]*H2OVc*H2OVc*H2OVc*H2OVc*x[H2O]*x[H2O]*x[H2O]*x[H2O]*x[H2O];
    *d2dvdt2 +=  5.0*D2powSum(dEnd[H2O], 4.0, dEnd[CO2], 1.0, ddEnddt[H2O], ddEnddt[CO2], d2dEnddt2[H2O], d2dEnddt2[CO2])*pow(powSum(H2OVc, 4.0, CO2Vc, 1.0), 4.0)*x[H2O]*x[H2O]*x[H2O]*x[H2O]*x[CO2];
    *d2dvdt2 += 10.0*D2powSum(dEnd[H2O], 3.0, dEnd[CO2], 2.0, ddEnddt[H2O], ddEnddt[CO2], d2dEnddt2[H2O], d2dEnddt2[CO2])*pow(powSum(H2OVc, 3.0, CO2Vc, 2.0), 4.0)*x[H2O]*x[H2O]*x[H2O]*x[CO2]*x[CO2];
    *d2dvdt2 += 10.0*D2powSum(dEnd[H2O], 2.0, dEnd[CO2], 3.0, ddEnddt[H2O], ddEnddt[CO2], d2dEnddt2[H2O], d2dEnddt2[CO2])*pow(powSum(H2OVc, 2.0, CO2Vc, 3.0), 4.0)*x[H2O]*x[H2O]*x[CO2]*x[CO2]*x[CO2];
    *d2dvdt2 +=  5.0*D2powSum(dEnd[H2O], 1.0, dEnd[CO2], 4.0, ddEnddt[H2O], ddEnddt[CO2], d2dEnddt2[H2O], d2dEnddt2[CO2])*pow(powSum(H2OVc, 1.0, CO2Vc, 4.0), 4.0)*x[H2O]*x[CO2]*x[CO2]*x[CO2]*x[CO2];
    *d2dvdt2 += d2dEnddt2[CO2]*CO2Vc*CO2Vc*CO2Vc*CO2Vc*x[CO2]*x[CO2]*x[CO2]*x[CO2]*x[CO2];
  }
  
  return;
}

static void EVcAndDerivative(int useLowPcoeff, double t, double x[2], double *ev, double evPrime[2], double *devdt, double *d2evdt2) {
  double eEnd[2], deEnddt[2], d2eEnddt2[2];
  double H2OTr = t/H2OTc;
  double CO2Tr = t/CO2Tc;
  double dH2OTrdt = 1.0/H2OTc;
  double dCO2Trdt = 1.0/CO2Tc;

  if (useLowPcoeff) {
    eEnd[H2O] = H2OLa10 + H2OLa11/H2OTr/H2OTr + H2OLa12/H2OTr/H2OTr/H2OTr;
    eEnd[CO2] = CO2La10 + CO2La11/CO2Tr/CO2Tr + CO2La12/CO2Tr/CO2Tr/CO2Tr;
    deEnddt[H2O] = - 2.0*H2OLa11*dH2OTrdt/H2OTr/H2OTr/H2OTr - 3.0*H2OLa12*dH2OTrdt/H2OTr/H2OTr/H2OTr/H2OTr;
    deEnddt[CO2] = - 2.0*CO2La11*dCO2Trdt/CO2Tr/CO2Tr/CO2Tr - 3.0*CO2La12*dCO2Trdt/CO2Tr/CO2Tr/CO2Tr/CO2Tr;
    d2eEnddt2[H2O] = 6.0*H2OLa11*dH2OTrdt*dH2OTrdt/H2OTr/H2OTr/H2OTr/H2OTr + 12.0*H2OLa12*dH2OTrdt*dH2OTrdt/H2OTr/H2OTr/H2OTr/H2OTr/H2OTr;
    d2eEnddt2[CO2] = 6.0*CO2La11*dCO2Trdt*dCO2Trdt/CO2Tr/CO2Tr/CO2Tr/CO2Tr + 12.0*CO2La12*dCO2Trdt*dCO2Trdt/CO2Tr/CO2Tr/CO2Tr/CO2Tr/CO2Tr;
  } else {
    eEnd[H2O] = H2OHa10 + H2OHa11/H2OTr/H2OTr + H2OHa12/H2OTr/H2OTr/H2OTr;
    eEnd[CO2] = CO2Ha10 + CO2Ha11/CO2Tr/CO2Tr + CO2Ha12/CO2Tr/CO2Tr/CO2Tr;
    deEnddt[H2O] = - 2.0*H2OHa11*dH2OTrdt/H2OTr/H2OTr/H2OTr - 3.0*H2OHa12*dH2OTrdt/H2OTr/H2OTr/H2OTr/H2OTr;
    deEnddt[CO2] = - 2.0*CO2Ha11*dCO2Trdt/CO2Tr/CO2Tr/CO2Tr - 3.0*CO2Ha12*dCO2Trdt/CO2Tr/CO2Tr/CO2Tr/CO2Tr;
    d2eEnddt2[H2O] = 6.0*H2OHa11*dH2OTrdt*dH2OTrdt/H2OTr/H2OTr/H2OTr/H2OTr + 12.0*H2OHa12*dH2OTrdt*dH2OTrdt/H2OTr/H2OTr/H2OTr/H2OTr/H2OTr;
    d2eEnddt2[CO2] = 6.0*CO2Ha11*dCO2Trdt*dCO2Trdt/CO2Tr/CO2Tr/CO2Tr/CO2Tr + 12.0*CO2Ha12*dCO2Trdt*dCO2Trdt/CO2Tr/CO2Tr/CO2Tr/CO2Tr/CO2Tr;
  }
  
  if ((x[H2O] == 0.0) && (x[CO2] == 0.0)) {
    *ev = 0.0;
    evPrime[H2O] = 0.0;
    evPrime[CO2] = 0.0;

    *devdt   = 0.0;
    *d2evdt2 = 0.0;
    
  } else if ((x[H2O] == 1.0) && (x[CO2] == 0.0)) {
    *ev = eEnd[H2O]*H2OVc*H2OVc*H2OVc*H2OVc*H2OVc;
    evPrime[H2O] = 6.0*eEnd[H2O]*H2OVc*H2OVc*H2OVc*H2OVc*H2OVc;
    evPrime[CO2] = 6.0*powSum(eEnd[H2O], 5.0, eEnd[CO2], 1.0)*pow(powSum(H2OVc, 5.0, CO2Vc, 1.0), 5.0);

    *devdt = deEnddt[H2O]*H2OVc*H2OVc*H2OVc*H2OVc*H2OVc;
    *d2evdt2 = d2eEnddt2[H2O]*H2OVc*H2OVc*H2OVc*H2OVc*H2OVc;
    
  } else if ((x[H2O] == 0.0) && (x[CO2] == 1.0)) {
    *ev = eEnd[CO2]*CO2Vc*CO2Vc*CO2Vc*CO2Vc*CO2Vc;
    evPrime[H2O] = 6.0*powSum(eEnd[H2O], 1.0, eEnd[CO2], 5.0)*pow(powSum(H2OVc, 1.0, CO2Vc, 5.0), 5.0);
    evPrime[CO2] = 6.0*eEnd[CO2]*CO2Vc*CO2Vc*CO2Vc*CO2Vc*CO2Vc;

    *devdt = deEnddt[CO2]*CO2Vc*CO2Vc*CO2Vc*CO2Vc*CO2Vc;
    *d2evdt2 = d2eEnddt2[CO2]*CO2Vc*CO2Vc*CO2Vc*CO2Vc*CO2Vc;
    
  } else {    
    *ev  = 0.0;
    *ev += eEnd[H2O]*H2OVc*H2OVc*H2OVc*H2OVc*H2OVc*x[H2O]*x[H2O]*x[H2O]*x[H2O]*x[H2O]*x[H2O];
    *ev +=  6.0*powSum(eEnd[H2O], 5.0, eEnd[CO2], 1.0)*pow(powSum(H2OVc, 5.0, CO2Vc, 1.0), 5.0)*x[H2O]*x[H2O]*x[H2O]*x[H2O]*x[H2O]*x[CO2];
    *ev += 15.0*powSum(eEnd[H2O], 4.0, eEnd[CO2], 2.0)*pow(powSum(H2OVc, 4.0, CO2Vc, 2.0), 5.0)*x[H2O]*x[H2O]*x[H2O]*x[H2O]*x[CO2]*x[CO2];
    *ev += 20.0*powSum(eEnd[H2O], 3.0, eEnd[CO2], 3.0)*pow(powSum(H2OVc, 3.0, CO2Vc, 3.0), 5.0)*x[H2O]*x[H2O]*x[H2O]*x[CO2]*x[CO2]*x[CO2];
    *ev += 15.0*powSum(eEnd[H2O], 2.0, eEnd[CO2], 4.0)*pow(powSum(H2OVc, 2.0, CO2Vc, 4.0), 5.0)*x[H2O]*x[H2O]*x[CO2]*x[CO2]*x[CO2]*x[CO2];
    *ev +=  6.0*powSum(eEnd[H2O], 1.0, eEnd[CO2], 5.0)*pow(powSum(H2OVc, 1.0, CO2Vc, 5.0), 5.0)*x[H2O]*x[CO2]*x[CO2]*x[CO2]*x[CO2]*x[CO2];
    *ev += eEnd[CO2]*CO2Vc*CO2Vc*CO2Vc*CO2Vc*CO2Vc*x[CO2]*x[CO2]*x[CO2]*x[CO2]*x[CO2]*x[CO2];
  
    evPrime[H2O]  =  0.0;
    evPrime[H2O] +=  6.0*eEnd[H2O]*H2OVc*H2OVc*H2OVc*H2OVc*H2OVc*x[H2O]*x[H2O]*x[H2O]*x[H2O]*x[H2O];
    evPrime[H2O] += 30.0*powSum(eEnd[H2O], 5.0, eEnd[CO2], 1.0)*pow(powSum(H2OVc, 5.0, CO2Vc, 1.0), 5.0)*x[H2O]*x[H2O]*x[H2O]*x[H2O]*x[CO2];
    evPrime[H2O] += 60.0*powSum(eEnd[H2O], 4.0, eEnd[CO2], 2.0)*pow(powSum(H2OVc, 4.0, CO2Vc, 2.0), 5.0)*x[H2O]*x[H2O]*x[H2O]*x[CO2]*x[CO2];
    evPrime[H2O] += 60.0*powSum(eEnd[H2O], 3.0, eEnd[CO2], 3.0)*pow(powSum(H2OVc, 3.0, CO2Vc, 3.0), 5.0)*x[H2O]*x[H2O]*x[CO2]*x[CO2]*x[CO2];
    evPrime[H2O] += 30.0*powSum(eEnd[H2O], 2.0, eEnd[CO2], 4.0)*pow(powSum(H2OVc, 2.0, CO2Vc, 4.0), 5.0)*x[H2O]*x[CO2]*x[CO2]*x[CO2]*x[CO2];
    evPrime[H2O] +=  6.0*powSum(eEnd[H2O], 1.0, eEnd[CO2], 5.0)*pow(powSum(H2OVc, 1.0, CO2Vc, 5.0), 5.0)*x[CO2]*x[CO2]*x[CO2]*x[CO2]*x[CO2];;

    evPrime[CO2]  =  0.0;
    evPrime[CO2] +=  6.0*powSum(eEnd[H2O], 5.0, eEnd[CO2], 1.0)*pow(powSum(H2OVc, 5.0, CO2Vc, 1.0), 5.0)*x[H2O]*x[H2O]*x[H2O]*x[H2O]*x[H2O];
    evPrime[CO2] += 30.0*powSum(eEnd[H2O], 4.0, eEnd[CO2], 2.0)*pow(powSum(H2OVc, 4.0, CO2Vc, 2.0), 5.0)*x[H2O]*x[H2O]*x[H2O]*x[H2O]*x[CO2];
    evPrime[CO2] += 60.0*powSum(eEnd[H2O], 3.0, eEnd[CO2], 3.0)*pow(powSum(H2OVc, 3.0, CO2Vc, 3.0), 5.0)*x[H2O]*x[H2O]*x[H2O]*x[CO2]*x[CO2];
    evPrime[CO2] += 60.0*powSum(eEnd[H2O], 2.0, eEnd[CO2], 4.0)*pow(powSum(H2OVc, 2.0, CO2Vc, 4.0), 5.0)*x[H2O]*x[H2O]*x[CO2]*x[CO2]*x[CO2];
    evPrime[CO2] += 30.0*powSum(eEnd[H2O], 1.0, eEnd[CO2], 5.0)*pow(powSum(H2OVc, 1.0, CO2Vc, 5.0), 5.0)*x[H2O]*x[CO2]*x[CO2]*x[CO2]*x[CO2];
    evPrime[CO2] +=  6.0*eEnd[CO2]*CO2Vc*CO2Vc*CO2Vc*CO2Vc*CO2Vc*x[CO2]*x[CO2]*x[CO2]*x[CO2]*x[CO2];

    *devdt  = 0.0;
    *devdt += deEnddt[H2O]*H2OVc*H2OVc*H2OVc*H2OVc*H2OVc*x[H2O]*x[H2O]*x[H2O]*x[H2O]*x[H2O]*x[H2O];
    *devdt +=  6.0*DpowSum(eEnd[H2O], 5.0, eEnd[CO2], 1.0, deEnddt[H2O], deEnddt[CO2])*pow(powSum(H2OVc, 5.0, CO2Vc, 1.0), 5.0)*x[H2O]*x[H2O]*x[H2O]*x[H2O]*x[H2O]*x[CO2];
    *devdt += 15.0*DpowSum(eEnd[H2O], 4.0, eEnd[CO2], 2.0, deEnddt[H2O], deEnddt[CO2])*pow(powSum(H2OVc, 4.0, CO2Vc, 2.0), 5.0)*x[H2O]*x[H2O]*x[H2O]*x[H2O]*x[CO2]*x[CO2];
    *devdt += 20.0*DpowSum(eEnd[H2O], 3.0, eEnd[CO2], 3.0, deEnddt[H2O], deEnddt[CO2])*pow(powSum(H2OVc, 3.0, CO2Vc, 3.0), 5.0)*x[H2O]*x[H2O]*x[H2O]*x[CO2]*x[CO2]*x[CO2];
    *devdt += 15.0*DpowSum(eEnd[H2O], 2.0, eEnd[CO2], 4.0, deEnddt[H2O], deEnddt[CO2])*pow(powSum(H2OVc, 2.0, CO2Vc, 4.0), 5.0)*x[H2O]*x[H2O]*x[CO2]*x[CO2]*x[CO2]*x[CO2];
    *devdt +=  6.0*DpowSum(eEnd[H2O], 1.0, eEnd[CO2], 5.0, deEnddt[H2O], deEnddt[CO2])*pow(powSum(H2OVc, 1.0, CO2Vc, 5.0), 5.0)*x[H2O]*x[CO2]*x[CO2]*x[CO2]*x[CO2]*x[CO2];
    *devdt += deEnddt[CO2]*CO2Vc*CO2Vc*CO2Vc*CO2Vc*CO2Vc*x[CO2]*x[CO2]*x[CO2]*x[CO2]*x[CO2]*x[CO2];

    *d2evdt2  = 0.0;
    *d2evdt2 += d2eEnddt2[H2O]*H2OVc*H2OVc*H2OVc*H2OVc*H2OVc*x[H2O]*x[H2O]*x[H2O]*x[H2O]*x[H2O]*x[H2O];
    *d2evdt2 +=  6.0*D2powSum(eEnd[H2O], 5.0, eEnd[CO2], 1.0, deEnddt[H2O], deEnddt[CO2], d2eEnddt2[H2O], d2eEnddt2[CO2])*pow(powSum(H2OVc, 5.0, CO2Vc, 1.0), 5.0)*x[H2O]*x[H2O]*x[H2O]*x[H2O]*x[H2O]*x[CO2];
    *d2evdt2 += 15.0*D2powSum(eEnd[H2O], 4.0, eEnd[CO2], 2.0, deEnddt[H2O], deEnddt[CO2], d2eEnddt2[H2O], d2eEnddt2[CO2])*pow(powSum(H2OVc, 4.0, CO2Vc, 2.0), 5.0)*x[H2O]*x[H2O]*x[H2O]*x[H2O]*x[CO2]*x[CO2];
    *d2evdt2 += 20.0*D2powSum(eEnd[H2O], 3.0, eEnd[CO2], 3.0, deEnddt[H2O], deEnddt[CO2], d2eEnddt2[H2O], d2eEnddt2[CO2])*pow(powSum(H2OVc, 3.0, CO2Vc, 3.0), 5.0)*x[H2O]*x[H2O]*x[H2O]*x[CO2]*x[CO2]*x[CO2];
    *d2evdt2 += 15.0*D2powSum(eEnd[H2O], 2.0, eEnd[CO2], 4.0, deEnddt[H2O], deEnddt[CO2], d2eEnddt2[H2O], d2eEnddt2[CO2])*pow(powSum(H2OVc, 2.0, CO2Vc, 4.0), 5.0)*x[H2O]*x[H2O]*x[CO2]*x[CO2]*x[CO2]*x[CO2];
    *d2evdt2 +=  6.0*D2powSum(eEnd[H2O], 1.0, eEnd[CO2], 5.0, deEnddt[H2O], deEnddt[CO2], d2eEnddt2[H2O], d2eEnddt2[CO2])*pow(powSum(H2OVc, 1.0, CO2Vc, 5.0), 5.0)*x[H2O]*x[CO2]*x[CO2]*x[CO2]*x[CO2]*x[CO2];
    *d2evdt2 += d2eEnddt2[CO2]*CO2Vc*CO2Vc*CO2Vc*CO2Vc*CO2Vc*x[CO2]*x[CO2]*x[CO2]*x[CO2]*x[CO2]*x[CO2];
  }
  
  return;
}

static void FVcAndDerivative(int useLowPcoeff, double t, double x[2], double *fv, double fvPrime[2], double *dfvdt, double *d2fvdt2) {
  double fEnd[2], dfEnddt[2], d2fEnddt2[2];
  double H2OTr = t/H2OTc;
  double CO2Tr = t/CO2Tc;
  double dH2OTrdt = 1.0/H2OTc;
  double dCO2Trdt = 1.0/CO2Tc;

  if (useLowPcoeff) {
    fEnd[H2O] = H2OLa/H2OTr/H2OTr/H2OTr;
    fEnd[CO2] = CO2La/CO2Tr/CO2Tr/CO2Tr;
    dfEnddt[H2O] = - 3.0*H2OLa*dH2OTrdt/H2OTr/H2OTr/H2OTr/H2OTr;
    dfEnddt[CO2] = - 3.0*CO2La*dCO2Trdt/CO2Tr/CO2Tr/CO2Tr/CO2Tr;
    d2fEnddt2[H2O] = 12.0*H2OLa*dH2OTrdt*dH2OTrdt/H2OTr/H2OTr/H2OTr/H2OTr/H2OTr;
    d2fEnddt2[CO2] = 12.0*CO2La*dCO2Trdt*dCO2Trdt/CO2Tr/CO2Tr/CO2Tr/CO2Tr/CO2Tr;
  } else {
    fEnd[H2O] = H2OHa/H2OTr/H2OTr/H2OTr;
    fEnd[CO2] = CO2Ha/CO2Tr/CO2Tr/CO2Tr;
    dfEnddt[H2O] = - 3.0*H2OHa*dH2OTrdt/H2OTr/H2OTr/H2OTr/H2OTr;
    dfEnddt[CO2] = - 3.0*CO2Ha*dCO2Trdt/CO2Tr/CO2Tr/CO2Tr/CO2Tr;
    d2fEnddt2[H2O] = 12.0*H2OHa*dH2OTrdt*dH2OTrdt/H2OTr/H2OTr/H2OTr/H2OTr/H2OTr;
    d2fEnddt2[CO2] = 12.0*CO2Ha*dCO2Trdt*dCO2Trdt/CO2Tr/CO2Tr/CO2Tr/CO2Tr/CO2Tr;
  }
  
  if ((x[H2O] == 0.0) && (x[CO2] == 0.0)) {
    *fv = 0.0;
    fvPrime[H2O] = 0.0;
    fvPrime[CO2] = 0.0;

    *dfvdt   = 0.0;
    *d2fvdt2 = 0.0;
    
  } else if ((x[H2O] == 1.0) && (x[CO2] == 0.0)) {
    *fv = fEnd[H2O]*H2OVc*H2OVc;
    fvPrime[H2O] = 2.0*fEnd[H2O]*H2OVc*H2OVc;
    fvPrime[CO2] = 2.0*powSum(fEnd[H2O], 1.0, fEnd[CO2], 1.0)*pow(powSum(H2OVc, 1.0, CO2Vc, 1.0), 2.0);

    *dfvdt   = dfEnddt[H2O]*H2OVc*H2OVc;
    *d2fvdt2 = d2fEnddt2[H2O]*H2OVc*H2OVc;
    
  } else if ((x[H2O] == 0.0) && (x[CO2] == 1.0)) {
    *fv = fEnd[CO2]*CO2Vc*CO2Vc;
    fvPrime[H2O] = 2.0*powSum(fEnd[H2O], 1.0, fEnd[CO2], 1.0)*pow(powSum(H2OVc, 1.0, CO2Vc, 1.0), 2.0);
    fvPrime[CO2] = 2.0*fEnd[CO2]*CO2Vc*CO2Vc;

    *dfvdt   = dfEnddt[CO2]*CO2Vc*CO2Vc;
    *d2fvdt2 = d2fEnddt2[CO2]*CO2Vc*CO2Vc;
    
  } else {
    *fv  = 0.0;
    *fv += fEnd[H2O]*H2OVc*H2OVc*x[H2O]*x[H2O];
    *fv += 2.0*powSum(fEnd[H2O], 1.0, fEnd[CO2], 1.0)*pow(powSum(H2OVc, 1.0, CO2Vc, 1.0), 2.0)*x[H2O]*x[CO2];
    *fv += fEnd[CO2]*CO2Vc*CO2Vc*x[CO2]*x[CO2];
  
    fvPrime[H2O]  = 0.0;
    fvPrime[H2O] += 2.0*fEnd[H2O]*H2OVc*H2OVc*x[H2O];
    fvPrime[H2O] += 2.0*powSum(fEnd[H2O], 1.0, fEnd[CO2], 1.0)*pow(powSum(H2OVc, 1.0, CO2Vc, 1.0), 2.0)*x[CO2];

    fvPrime[CO2]  = 0.0;
    fvPrime[CO2] += 2.0*powSum(fEnd[H2O], 1.0, fEnd[CO2], 1.0)*pow(powSum(H2OVc, 1.0, CO2Vc, 1.0), 2.0)*x[H2O];
    fvPrime[CO2] += 2.0*fEnd[CO2]*CO2Vc*CO2Vc*x[CO2];

    *dfvdt  = 0.0;
    *dfvdt += dfEnddt[H2O]*H2OVc*H2OVc*x[H2O]*x[H2O];
    *dfvdt += 2.0*DpowSum(fEnd[H2O], 1.0, fEnd[CO2], 1.0, dfEnddt[H2O], dfEnddt[CO2])*pow(powSum(H2OVc, 1.0, CO2Vc, 1.0), 2.0)*x[H2O]*x[CO2];
    *dfvdt += dfEnddt[CO2]*CO2Vc*CO2Vc*x[CO2]*x[CO2];

    *d2fvdt2  = 0.0;
    *d2fvdt2 += d2fEnddt2[H2O]*H2OVc*H2OVc*x[H2O]*x[H2O];
    *d2fvdt2 += 2.0*D2powSum(fEnd[H2O], 1.0, fEnd[CO2], 1.0, dfEnddt[H2O], dfEnddt[CO2], d2fEnddt2[H2O], d2fEnddt2[CO2])*pow(powSum(H2OVc, 1.0, CO2Vc, 1.0), 2.0)*x[H2O]*x[CO2];
    *d2fvdt2 += d2fEnddt2[CO2]*CO2Vc*CO2Vc*x[CO2]*x[CO2];
  }
  
  return;
}

static void BetaAndDerivative(int useLowPcoeff, double t, double x[2], double *beta, double betaPrime[2]) {
  double betaEnd[2];

  if (useLowPcoeff) {
    betaEnd[H2O] = H2OLb;
    betaEnd[CO2] = CO2Lb;
  } else {
    betaEnd[H2O] = H2OHb;
    betaEnd[CO2] = CO2Hb;
  }
  
  *beta  = betaEnd[H2O]*x[H2O] + betaEnd[CO2]*x[CO2];
  
  betaPrime[H2O]  = betaEnd[H2O];
  betaPrime[CO2]  = betaEnd[CO2];
  
  return;
}

static void GammaVcAndDerivative(int useLowPcoeff, double t, double x[2], double *gammav, double gammavPrime[2]) {
  double gammaEnd[2];
  double k3 = 0.0;

  if (useLowPcoeff) {
    gammaEnd[H2O] = H2OLc;
    gammaEnd[CO2] = CO2Lc;
    k3 = 0.9;
  } else {
    gammaEnd[H2O] = H2OHc;
    gammaEnd[CO2] = CO2Hc;
    k3 = 1.0;
  }
  
  if ((x[H2O] == 0.0) && (x[CO2] == 0.0)) {
    *gammav = 0.0;
    gammavPrime[H2O] = 0.0;
    gammavPrime[CO2] = 0.0;
    
  } else if ((x[H2O] == 1.0) && (x[CO2] == 0.0)) {
    *gammav = gammaEnd[H2O]*H2OVc*H2OVc;
    gammavPrime[H2O] = 3.0*gammaEnd[H2O]*H2OVc*H2OVc;
    gammavPrime[CO2] = 3.0*powSum(gammaEnd[H2O], 2.0, gammaEnd[CO2], 1.0)*k3*pow(powSum(H2OVc, 2.0, CO2Vc, 1.0), 2.0);
    
  } else if ((x[H2O] == 0.0) && (x[CO2] == 1.0)) {
    *gammav = gammaEnd[CO2]*CO2Vc*CO2Vc;
    gammavPrime[H2O] = 3.0*powSum(gammaEnd[H2O], 1.0, gammaEnd[CO2], 2.0)*k3*pow(powSum(H2OVc, 1.0, CO2Vc, 2.0), 2.0);
    gammavPrime[CO2] = 3.0*gammaEnd[CO2]*CO2Vc*CO2Vc;
    
  } else {
    *gammav  = 0.0;
    *gammav += gammaEnd[H2O]*H2OVc*H2OVc*x[H2O]*x[H2O]*x[H2O];
    *gammav += 3.0*powSum(gammaEnd[H2O], 2.0, gammaEnd[CO2], 1.0)*k3*pow(powSum(H2OVc, 2.0, CO2Vc, 1.0), 2.0)*x[H2O]*x[H2O]*x[CO2];
    *gammav += 3.0*powSum(gammaEnd[H2O], 1.0, gammaEnd[CO2], 2.0)*k3*pow(powSum(H2OVc, 1.0, CO2Vc, 2.0), 2.0)*x[H2O]*x[CO2]*x[CO2];      
    *gammav += gammaEnd[CO2]*CO2Vc*CO2Vc*x[CO2]*x[CO2]*x[CO2];
  
    gammavPrime[H2O]  = 0.0;
    gammavPrime[H2O] += 3.0*gammaEnd[H2O]*H2OVc*H2OVc*x[H2O]*x[H2O];
    gammavPrime[H2O] += 6.0*powSum(gammaEnd[H2O], 2.0, gammaEnd[CO2], 1.0)*k3*pow(powSum(H2OVc, 2.0, CO2Vc, 1.0), 2.0)*x[H2O]*x[CO2];
    gammavPrime[H2O] += 3.0*powSum(gammaEnd[H2O], 1.0, gammaEnd[CO2], 2.0)*k3*pow(powSum(H2OVc, 1.0, CO2Vc, 2.0), 2.0)*x[CO2]*x[CO2];

    gammavPrime[CO2]  = 0.0;
    gammavPrime[CO2] += 3.0*powSum(gammaEnd[H2O], 2.0, gammaEnd[CO2], 1.0)*k3*pow(powSum(H2OVc, 2.0, CO2Vc, 1.0), 2.0)*x[H2O]*x[H2O];
    gammavPrime[CO2] += 6.0*powSum(gammaEnd[H2O], 1.0, gammaEnd[CO2], 2.0)*k3*pow(powSum(H2OVc, 1.0, CO2Vc, 2.0), 2.0)*x[H2O]*x[CO2];
    gammavPrime[CO2] += 3.0*gammaEnd[CO2]*CO2Vc*CO2Vc*x[CO2]*x[CO2];
  }
  
  return;
}

static void idealGasH2O(double t, double *cp, double *s0, double *h0, double *dcpdt) {
  int i;
  
  for (i=0, *cp=0.0; i<7; i++) *cp += idealCoeff[i][H2O]*pow(t/1000.0, (double) i);
  for (i=7; i<13; i++)         *cp += idealCoeff[i][H2O]/pow(t/1000.0, (double) (i-6));
  
  for (i=1, *dcpdt=0.0; i<7; i++) *dcpdt +=  ((double) i)  *idealCoeff[i][H2O]*pow(t/1000.0, (double) i-1);
  for (i=7; i<13; i++)            *dcpdt += -((double) i-6)*idealCoeff[i][H2O]/pow(t/1000.0, (double) (i+1-6));
  
  for (i=0, *h0=0.0; i<7; i++) *h0 += idealCoeff[i][H2O]*pow(t/1000.0, (double) (i+1))/((double) (i+1));
  			       *h0 += idealCoeff[7][H2O]*log(t/1000.0);
  for (i=8; i<13; i++)         *h0 += idealCoeff[i][H2O]/pow(t/1000.0, (double) (i-7))/((double) (7-i));
  
  			       *s0  = idealCoeff[0][H2O]*log(t/1000.0);
  for (i=1; i<7; i++)	       *s0 += idealCoeff[i][H2O]*pow(t/1000.0, (double) i)/((double) i);
  for (i=7; i<13; i++)         *s0 += idealCoeff[i][H2O]/pow(t/1000.0, (double) (i-6))/((double) (6-i));

  *cp    *= 8.31451;
  *h0    *= 8.31451*1000.0;
  *s0    *= 8.31451;
  *dcpdt *= 8.31451/1000.0;
}  

static void idealGasCO2(double t, double *cp, double *s0, double *h0, double *dcpdt) {
  int i;
  
  for (i=0, *cp=0.0; i<7; i++) *cp += idealCoeff[i][CO2]*pow(t/1000.0, (double) i);
  for (i=7; i<13; i++)         *cp += idealCoeff[i][CO2]/pow(t/1000.0, (double) (i-6));
  
  for (i=1, *dcpdt=0.0; i<7; i++) *dcpdt +=  ((double) i)  *idealCoeff[i][H2O]*pow(t/1000.0, (double) i-1);
  for (i=7; i<13; i++)            *dcpdt += -((double) i-6)*idealCoeff[i][H2O]/pow(t/1000.0, (double) (i+1-6));
  
  for (i=0, *h0=0.0; i<7; i++) *h0 += idealCoeff[i][CO2]*pow(t/1000.0, (double) (i+1))/((double) (i+1));
  			       *h0 += idealCoeff[7][CO2]*log(t/1000.0);
  for (i=8; i<13; i++)         *h0 += idealCoeff[i][CO2]/pow(t/1000.0, (double) (i-7))/((double) (7-i));
  
  			       *s0  = idealCoeff[0][CO2]*log(t/1000.0);
  for (i=1; i<7; i++)	       *s0 += idealCoeff[i][CO2]*pow(t/1000.0, (double) i)/((double) i);
  for (i=7; i<13; i++)         *s0 += idealCoeff[i][CO2]/pow(t/1000.0, (double) (i-6))/((double) (6-i));

  *cp    *= 8.31451;
  *h0    *= 8.31451*1000.0;
  *s0    *= 8.31451;
  *dcpdt *= 8.31451/1000.0;
}  

static void duan(double t, double p, double x[2], double *vPt, double *zPt, double phi[2], double *dvdt, double *dvdp, double *d2vdt2, double *d2vdtdp, double *d2vdp2) {
  double bv, cv, dv, ev, fv, beta, gammav, v, z, dzdv, dzdt, d2zdv2, d2zdvdt, d2zdt2;
  double dbvdt, dcvdt, ddvdt, devdt, dfvdt, d2bvdt2, d2cvdt2, d2dvdt2, d2evdt2, d2fvdt2, d3bvdt3;
  double bvPrime[2], cvPrime[2], dvPrime[2], evPrime[2], fvPrime[2], betaPrime[2], gammavPrime[2];
  double dbvPrimedt[2], d2bvPrimedt2[2], d3bvPrimedt3[2];
  double zRef, vRef, phiRef[2], lnPhiH2O, lnPhiCO2;
  int useLowPcoeff, iter;
  
  zRef = 1.0;	  
  vRef = DBL_MAX; 
  phiRef[0] = 1.0;
  phiRef[1] = 1.0;
  
  useLowPcoeff = (p <= 2000.0) ? 1 : 0;
  
  if (!useLowPcoeff) {
    BVcAndDerivative	(1, t, x, &bv,     bvPrime,    &dbvdt,    &d2bvdt2,  &d3bvdt3, dbvPrimedt, d2bvPrimedt2, d3bvPrimedt3);
    CVcAndDerivative	(1, t, x, &cv,     cvPrime,    &dcvdt,    &d2cvdt2);
    DVcAndDerivative	(1, t, x, &dv,     dvPrime,    &ddvdt,    &d2dvdt2);
    EVcAndDerivative	(1, t, x, &ev,     evPrime,    &devdt,    &d2evdt2);
    FVcAndDerivative	(1, t, x, &fv,     fvPrime,    &dfvdt,    &d2fvdt2);
    BetaAndDerivative	(1, t, x, &beta,   betaPrime);
    GammaVcAndDerivative(1, t, x, &gammav, gammavPrime);
    
    iter = 0;
    v = 8.314467*t/2000.0;
    while (iter < 100) {
      double delv;
      z = 1 + bv/v + cv/v/v + dv/v/v/v/v + ev/v/v/v/v/v + (fv/v/v) * (beta + gammav/v/v) * exp(-gammav/v/v);
      delv = z*8.314467*t/2000.0 - v;
      if (fabs(delv) < v*100.0*DBL_EPSILON) break;
      v = (z*8.314467*t/2000.0 + v)/2.0;
      iter++;
    }
    if (iter >= 100) printf("zRef = %g, vRef = %g, iter = %d\n", z, v, iter);
    
    lnPhiH2O  = log(phiRef[H2O]);
    lnPhiH2O += -log(z) + log(zRef);
    lnPhiH2O += bvPrime[H2O]/v;
    lnPhiH2O += cvPrime[H2O]/2.0/v/v;
    lnPhiH2O += dvPrime[H2O]/4.0/v/v/v/v;
    lnPhiH2O += evPrime[H2O]/5.0/v/v/v/v/v;
    lnPhiH2O += ((fvPrime[H2O]*beta + betaPrime[H2O]*fv)/2.0/gammav)*(1.0-exp(-gammav/v/v));
    lnPhiH2O += ((fvPrime[H2O]*gammav+gammavPrime[H2O]*fv-fv*beta*(gammavPrime[H2O]-gammav))/2.0/gammav/gammav)
    	       *(1.0 - (gammav/v/v + 1.0)*exp(-gammav/v/v));
    lnPhiH2O += ((gammavPrime[H2O]-gammav)*fv/2.0/gammav/gammav)*(-2.0 + (gammav*gammav/v/v/v/v + 2.0*gammav/v/v + 2.0)*exp(-gammav/v/v));
    
    lnPhiCO2  = log(phiRef[CO2]);
    lnPhiCO2 += -log(z) + log(zRef);
    lnPhiCO2 += bvPrime[CO2]/v;
    lnPhiCO2 += cvPrime[CO2]/2.0/v/v;
    lnPhiCO2 += dvPrime[CO2]/4.0/v/v/v/v;
    lnPhiCO2 += evPrime[CO2]/5.0/v/v/v/v/v;
    lnPhiCO2 += ((fvPrime[CO2]*beta + betaPrime[CO2]*fv)/2.0/gammav)*(1.0-exp(-gammav/v/v));
    lnPhiCO2 += ((fvPrime[CO2]*gammav+gammavPrime[CO2]*fv-fv*beta*(gammavPrime[CO2]-gammav))/2.0/gammav/gammav)
    	       *(1.0 - (gammav/v/v + 1.0)*exp(-gammav/v/v));
    lnPhiCO2 += ((gammavPrime[CO2]-gammav)*fv/2.0/gammav/gammav)*(-2.0 + (gammav*gammav/v/v/v/v + 2.0*gammav/v/v + 2.0)*exp(-gammav/v/v));

    phiRef[H2O] = exp(lnPhiH2O);
    phiRef[CO2] = exp(lnPhiCO2);
    /*
    printf("phi H2O ref = %g, phi CO2 ref = %g\n", phiRef[H2O], phiRef[CO2]);
    */
  }
  
  BVcAndDerivative    (useLowPcoeff, t, x, &bv,     bvPrime,    &dbvdt,    &d2bvdt2,  &d3bvdt3, dbvPrimedt, d2bvPrimedt2, d3bvPrimedt3);
  CVcAndDerivative    (useLowPcoeff, t, x, &cv,     cvPrime,    &dcvdt,    &d2cvdt2);
  DVcAndDerivative    (useLowPcoeff, t, x, &dv,     dvPrime,	&ddvdt,    &d2dvdt2);
  EVcAndDerivative    (useLowPcoeff, t, x, &ev,     evPrime,	&devdt,    &d2evdt2);
  FVcAndDerivative    (useLowPcoeff, t, x, &fv,     fvPrime,	&dfvdt,    &d2fvdt2);
  BetaAndDerivative   (useLowPcoeff, t, x, &beta,   betaPrime);
  GammaVcAndDerivative(useLowPcoeff, t, x, &gammav, gammavPrime);
  
  if (!useLowPcoeff) {
    iter = 0;
    v = 8.314467*t/2000.0;
    while (iter < 100) {
      double delv;
      z = 1 + bv/v + cv/v/v + dv/v/v/v/v + ev/v/v/v/v/v + (fv/v/v) * (beta + gammav/v/v) * exp(-gammav/v/v);
      delv = z*8.314467*t/2000.0 - v;
      if (fabs(delv) < v*100.0*DBL_EPSILON) break;
      v = (z*8.314467*t/2000.0 + v)/2.0;
      iter++;
    }
    if (iter >= 100) printf("zRef = %g, vRef = %g, iter = %d\n", z, v, iter);
    zRef = z;
    vRef = v;
  }

  iter = 0;
  v = 8.314467*t/p;
  while (iter < 100) {
    double delv;
    z = 1 + bv/v + cv/v/v + dv/v/v/v/v + ev/v/v/v/v/v + (fv/v/v) * (beta + gammav/v/v) * exp(-gammav/v/v);
    delv = z*8.314467*t/p - v;
    if (fabs(delv) < v*100.0*DBL_EPSILON) break;
    v = (z*8.314467*t/p + v)/2.0;
    iter++;
  }
  if (iter >= 100) printf("z = %g, v = %g, iter = %d\n", z, v, iter);

  lnPhiH2O  = log(phiRef[H2O]);
  lnPhiH2O += -log(z) + log(zRef);
  lnPhiH2O += bvPrime[H2O]/v             - (!useLowPcoeff ? bvPrime[H2O]/vRef                         : 0.0);
  lnPhiH2O += cvPrime[H2O]/2.0/v/v       - (!useLowPcoeff ? cvPrime[H2O]/2.0/vRef/vRef                : 0.0);
  lnPhiH2O += dvPrime[H2O]/4.0/v/v/v/v   - (!useLowPcoeff ? dvPrime[H2O]/4.0/vRef/vRef/vRef/vRef      : 0.0);
  lnPhiH2O += evPrime[H2O]/5.0/v/v/v/v/v - (!useLowPcoeff ? evPrime[H2O]/5.0/vRef/vRef/vRef/vRef/vRef : 0.0);
  lnPhiH2O += ((fvPrime[H2O]*beta + betaPrime[H2O]*fv)/2.0/gammav)*(1.0-exp(-gammav/v/v));
  lnPhiH2O += ((fvPrime[H2O]*gammav+gammavPrime[H2O]*fv-fv*beta*(gammavPrime[H2O]-gammav))/2.0/gammav/gammav)
             *(1.0 - (gammav/v/v + 1.0)*exp(-gammav/v/v));
  lnPhiH2O += ((gammavPrime[H2O]-gammav)*fv/2.0/gammav/gammav)*(-2.0 + (gammav*gammav/v/v/v/v + 2.0*gammav/v/v + 2.0)*exp(-gammav/v/v));
  if (!useLowPcoeff) {
    lnPhiH2O -= ((fvPrime[H2O]*beta + betaPrime[H2O]*fv)/2.0/gammav)*(1.0-exp(-gammav/vRef/vRef));
    lnPhiH2O -= ((fvPrime[H2O]*gammav+gammavPrime[H2O]*fv-fv*beta*(gammavPrime[H2O]-gammav))/2.0/gammav/gammav)
               *(1.0 - (gammav/vRef/vRef + 1.0)*exp(-gammav/vRef/vRef));
    lnPhiH2O -= ((gammavPrime[H2O]-gammav)*fv/2.0/gammav/gammav)
               *(-2.0 + (gammav*gammav/vRef/vRef/vRef/vRef + 2.0*gammav/vRef/vRef + 2.0)*exp(-gammav/vRef/vRef));
  }

  lnPhiCO2  = log(phiRef[CO2]);
  lnPhiCO2 += -log(z) + log(zRef);
  lnPhiCO2 += bvPrime[CO2]/v             - (!useLowPcoeff ? bvPrime[CO2]/vRef                         : 0.0);
  lnPhiCO2 += cvPrime[CO2]/2.0/v/v       - (!useLowPcoeff ? cvPrime[CO2]/2.0/vRef/vRef                : 0.0);
  lnPhiCO2 += dvPrime[CO2]/4.0/v/v/v/v   - (!useLowPcoeff ? dvPrime[CO2]/4.0/vRef/vRef/vRef/vRef      : 0.0);
  lnPhiCO2 += evPrime[CO2]/5.0/v/v/v/v/v - (!useLowPcoeff ? evPrime[CO2]/5.0/vRef/vRef/vRef/vRef/vRef : 0.0);
  lnPhiCO2 += ((fvPrime[CO2]*beta + betaPrime[CO2]*fv)/2.0/gammav)*(1.0-exp(-gammav/v/v));
  lnPhiCO2 += ((fvPrime[CO2]*gammav+gammavPrime[CO2]*fv-fv*beta*(gammavPrime[CO2]-gammav))/2.0/gammav/gammav)
             *(1.0 - (gammav/v/v + 1.0)*exp(-gammav/v/v));
  lnPhiCO2 += ((gammavPrime[CO2]-gammav)*fv/2.0/gammav/gammav)*(-2.0 + (gammav*gammav/v/v/v/v + 2.0*gammav/v/v + 2.0)*exp(-gammav/v/v));
  if (!useLowPcoeff) {
    lnPhiCO2 -= ((fvPrime[CO2]*beta + betaPrime[CO2]*fv)/2.0/gammav)*(1.0-exp(-gammav/vRef/vRef));
    lnPhiCO2 -= ((fvPrime[CO2]*gammav+gammavPrime[CO2]*fv-fv*beta*(gammavPrime[CO2]-gammav))/2.0/gammav/gammav)
               *(1.0 - (gammav/vRef/vRef + 1.0)*exp(-gammav/vRef/vRef));
    lnPhiCO2 -= ((gammavPrime[CO2]-gammav)*fv/2.0/gammav/gammav)
               *(-2.0 + (gammav*gammav/vRef/vRef/vRef/vRef + 2.0*gammav/vRef/vRef + 2.0)*exp(-gammav/vRef/vRef));
  }
  
  dzdv = - bv/v/v - 2.0*cv/v/v/v + - 4.0*dv/v/v/v/v/v - 5.0*ev/v/v/v/v/v/v 
         - 2.0*(fv/v/v/v) * (beta + gammav/v/v) * exp(-gammav/v/v)
	 - 2.0*(fv/v/v) * (gammav/v/v/v) * exp(-gammav/v/v)
	 + 2.0*(fv/v/v) * (beta + gammav/v/v) * (gammav/v/v/v) * exp(-gammav/v/v);
  dzdt = dbvdt/v + dcvdt/v/v + ddvdt/v/v/v/v + devdt/v/v/v/v/v + (dfvdt/v/v) * (beta + gammav/v/v) * exp(-gammav/v/v);

  d2zdv2 =  2.0*bv/v/v/v + 6.0*cv/v/v/v/v + 20.0*dv/v/v/v/v/v/v + 30.0*ev/v/v/v/v/v/v/v 
         +  6.0*(fv/v/v/v/v) * (beta + gammav/v/v) * exp(-gammav/v/v)
	 + 14.0*(fv/v/v/v) * (gammav/v/v/v) * exp(-gammav/v/v) 
	 - 14.0*(fv/v/v/v) * (gammav/v/v/v) * (beta + gammav/v/v) * exp(-gammav/v/v)
	 -  8.0*(fv/v/v) * (gammav/v/v/v) * (gammav/v/v/v) * exp(-gammav/v/v)
	 +  4.0*(fv/v/v) * (gammav/v/v/v) * (gammav/v/v/v) * (beta + gammav/v/v) * exp(-gammav/v/v);
  d2zdvdt = - dbvdt/v/v - 2.0*dcvdt/v/v/v + - 4.0*ddvdt/v/v/v/v/v - 5.0*devdt/v/v/v/v/v/v 
            - 2.0*(dfvdt/v/v/v) * (beta + gammav/v/v) * exp(-gammav/v/v)
	    - 2.0*(dfvdt/v/v) * (gammav/v/v/v) * exp(-gammav/v/v)
	    + 2.0*(dfvdt/v/v) * (beta + gammav/v/v) * (gammav/v/v/v) * exp(-gammav/v/v);
  d2zdt2 = d2bvdt2/v + d2cvdt2/v/v + d2dvdt2/v/v/v/v + d2evdt2/v/v/v/v/v + (d2fvdt2/v/v) * (beta + gammav/v/v) * exp(-gammav/v/v);
  
  *vPt = v;
  *zPt = z;
  phi[H2O] = exp(lnPhiH2O);
  phi[CO2] = exp(lnPhiCO2);
  *dvdp = 1.0/( p*(dzdv/z - 1.0/v) );
  *dvdt = (1.0/t + dzdt/z)/(1.0/v - dzdv/z);
  *d2vdp2  = p*(1.0/v-dzdv/z)/v - dzdv/(*dvdp)/z + 1.0/(*dvdp)/(*dvdp)/p + p*d2zdv2/z;
  *d2vdp2 *= -(*dvdp)*(*dvdp)*(*dvdp);
  *d2vdtdp = -(*dvdp)*(1.0/t + dzdt/z) - p*(*d2vdp2)*(1.0/t + dzdt/z) - p*(*dvdp)*(*dvdp)*(-dzdv*dzdt/z/z + d2zdvdt/z);
  *d2vdt2  = -p*(*d2vdtdp)*(1.0/t + dzdt/z) + p*(*dvdp)/t/t + p*(*dvdp)*dzdt*(dzdv*(*dvdt) + dzdt)/z/z - p*(*dvdp)*(*dvdt)*d2zdvdt/z - p*(*dvdp)*d2zdt2/z;
}

static void duanH2O(double t, double p, double *vPt, double *zPt, double *phi, double *dvdt, double *dvdp, double *d2vdt2, double *d2vdtdp, double *d2vdp2) {
  double bv, cv, dv, ev, fv, beta, gammav, v, z, dzdv, dzdt, d2zdv2, d2zdvdt, d2zdt2;
  double dbvdt, dcvdt, ddvdt, devdt, dfvdt, d2bvdt2, d2cvdt2, d2dvdt2, d2evdt2, d2fvdt2, d3bvdt3;
  double bvPrime[2], cvPrime[2], dvPrime[2], evPrime[2], fvPrime[2], betaPrime[2], gammavPrime[2];
  double dbvPrimedt[2], d2bvPrimedt2[2], d3bvPrimedt3[2];
  double zRef, vRef, phiRef, lnPhiH2O;
  double x[2] = { 1.0, 0.0 }; /* H2O */
  int useLowPcoeff, iter;
  
  zRef = 1.0;	  
  vRef = DBL_MAX; 
  phiRef = 1.0;
  
  useLowPcoeff = (p <= 2000.0) ? 1 : 0;
  
  if (!useLowPcoeff) {
    BVcAndDerivative	(1, t, x, &bv,     bvPrime,    &dbvdt,    &d2bvdt2,  &d3bvdt3, dbvPrimedt, d2bvPrimedt2, d3bvPrimedt3);
    CVcAndDerivative	(1, t, x, &cv,     cvPrime,    &dcvdt,    &d2cvdt2);
    DVcAndDerivative	(1, t, x, &dv,     dvPrime,    &ddvdt,    &d2dvdt2);
    EVcAndDerivative	(1, t, x, &ev,     evPrime,    &devdt,    &d2evdt2);
    FVcAndDerivative	(1, t, x, &fv,     fvPrime,    &dfvdt,    &d2fvdt2);
    BetaAndDerivative	(1, t, x, &beta,   betaPrime);
    GammaVcAndDerivative(1, t, x, &gammav, gammavPrime);
    
    iter = 0;
    v = 8.314467*t/2000.0;
    while (iter < 100) {
      double delv;
      z = 1 + bv/v + cv/v/v + dv/v/v/v/v + ev/v/v/v/v/v + (fv/v/v) * (beta + gammav/v/v) * exp(-gammav/v/v);
      delv = z*8.314467*t/2000.0 - v;
      if (fabs(delv) < v*100.0*DBL_EPSILON) break;
      v = (z*8.314467*t/2000.0 + v)/2.0;
      iter++;
    }
    if (iter >= 100) printf("zRef = %g, vRef = %g, iter = %d\n", z, v, iter);
    
    lnPhiH2O  = log(phiRef);
    lnPhiH2O += -log(z) + log(zRef);
    lnPhiH2O += bvPrime[H2O]/v;
    lnPhiH2O += cvPrime[H2O]/2.0/v/v;
    lnPhiH2O += dvPrime[H2O]/4.0/v/v/v/v;
    lnPhiH2O += evPrime[H2O]/5.0/v/v/v/v/v;
    lnPhiH2O += ((fvPrime[H2O]*beta + betaPrime[H2O]*fv)/2.0/gammav)*(1.0-exp(-gammav/v/v));
    lnPhiH2O += ((fvPrime[H2O]*gammav+gammavPrime[H2O]*fv-fv*beta*(gammavPrime[H2O]-gammav))/2.0/gammav/gammav)
    	       *(1.0 - (gammav/v/v + 1.0)*exp(-gammav/v/v));
    lnPhiH2O += ((gammavPrime[H2O]-gammav)*fv/2.0/gammav/gammav)*(-2.0 + (gammav*gammav/v/v/v/v + 2.0*gammav/v/v + 2.0)*exp(-gammav/v/v));
    
    phiRef = exp(lnPhiH2O);
  }
  
  BVcAndDerivative    (useLowPcoeff, t, x, &bv,     bvPrime,    &dbvdt,    &d2bvdt2,  &d3bvdt3, dbvPrimedt, d2bvPrimedt2, d3bvPrimedt3);
  CVcAndDerivative    (useLowPcoeff, t, x, &cv,     cvPrime,    &dcvdt,    &d2cvdt2);
  DVcAndDerivative    (useLowPcoeff, t, x, &dv,     dvPrime,	&ddvdt,    &d2dvdt2);
  EVcAndDerivative    (useLowPcoeff, t, x, &ev,     evPrime,	&devdt,    &d2evdt2);
  FVcAndDerivative    (useLowPcoeff, t, x, &fv,     fvPrime,	&dfvdt,    &d2fvdt2);
  BetaAndDerivative   (useLowPcoeff, t, x, &beta,   betaPrime);
  GammaVcAndDerivative(useLowPcoeff, t, x, &gammav, gammavPrime);
  
  if (!useLowPcoeff) {
    iter = 0;
    v = 8.314467*t/2000.0;
    while (iter < 100) {
      double delv;
      z = 1 + bv/v + cv/v/v + dv/v/v/v/v + ev/v/v/v/v/v + (fv/v/v) * (beta + gammav/v/v) * exp(-gammav/v/v);
      delv = z*8.314467*t/2000.0 - v;
      if (fabs(delv) < v*100.0*DBL_EPSILON) break;
      v = (z*8.314467*t/2000.0 + v)/2.0;
      iter++;
    }
    if (iter >= 100) printf("zRef = %g, vRef = %g, iter = %d\n", z, v, iter);
    zRef = z;
    vRef = v;
  }

  iter = 0;
  v = 8.314467*t/p;
  while (iter < 100) {
    double delv;
    z = 1 + bv/v + cv/v/v + dv/v/v/v/v + ev/v/v/v/v/v + (fv/v/v) * (beta + gammav/v/v) * exp(-gammav/v/v);
    delv = z*8.314467*t/p - v;
    if (fabs(delv) < v*100.0*DBL_EPSILON) break;
    v = (z*8.314467*t/p + v)/2.0;
    iter++;
  }
  if (iter >= 100) printf("z = %g, v = %g, iter = %d\n", z, v, iter);

  lnPhiH2O  = log(phiRef);
  lnPhiH2O += -log(z) + log(zRef);
  lnPhiH2O += bvPrime[H2O]/v             - (!useLowPcoeff ? bvPrime[H2O]/vRef                         : 0.0);
  lnPhiH2O += cvPrime[H2O]/2.0/v/v       - (!useLowPcoeff ? cvPrime[H2O]/2.0/vRef/vRef                : 0.0);
  lnPhiH2O += dvPrime[H2O]/4.0/v/v/v/v   - (!useLowPcoeff ? dvPrime[H2O]/4.0/vRef/vRef/vRef/vRef      : 0.0);
  lnPhiH2O += evPrime[H2O]/5.0/v/v/v/v/v - (!useLowPcoeff ? evPrime[H2O]/5.0/vRef/vRef/vRef/vRef/vRef : 0.0);
  lnPhiH2O += ((fvPrime[H2O]*beta + betaPrime[H2O]*fv)/2.0/gammav)*(1.0-exp(-gammav/v/v));
  lnPhiH2O += ((fvPrime[H2O]*gammav+gammavPrime[H2O]*fv-fv*beta*(gammavPrime[H2O]-gammav))/2.0/gammav/gammav)
             *(1.0 - (gammav/v/v + 1.0)*exp(-gammav/v/v));
  lnPhiH2O += ((gammavPrime[H2O]-gammav)*fv/2.0/gammav/gammav)*(-2.0 + (gammav*gammav/v/v/v/v + 2.0*gammav/v/v + 2.0)*exp(-gammav/v/v));
  if (!useLowPcoeff) {
    lnPhiH2O -= ((fvPrime[H2O]*beta + betaPrime[H2O]*fv)/2.0/gammav)*(1.0-exp(-gammav/vRef/vRef));
    lnPhiH2O -= ((fvPrime[H2O]*gammav+gammavPrime[H2O]*fv-fv*beta*(gammavPrime[H2O]-gammav))/2.0/gammav/gammav)
               *(1.0 - (gammav/vRef/vRef + 1.0)*exp(-gammav/vRef/vRef));
    lnPhiH2O -= ((gammavPrime[H2O]-gammav)*fv/2.0/gammav/gammav)
               *(-2.0 + (gammav*gammav/vRef/vRef/vRef/vRef + 2.0*gammav/vRef/vRef + 2.0)*exp(-gammav/vRef/vRef));
  }
  
  dzdv = - bv/v/v - 2.0*cv/v/v/v + - 4.0*dv/v/v/v/v/v - 5.0*ev/v/v/v/v/v/v 
         - 2.0*(fv/v/v/v) * (beta + gammav/v/v) * exp(-gammav/v/v)
	 - 2.0*(fv/v/v) * (gammav/v/v/v) * exp(-gammav/v/v)
	 + 2.0*(fv/v/v) * (beta + gammav/v/v) * (gammav/v/v/v) * exp(-gammav/v/v);
  dzdt = dbvdt/v + dcvdt/v/v + ddvdt/v/v/v/v + devdt/v/v/v/v/v + (dfvdt/v/v) * (beta + gammav/v/v) * exp(-gammav/v/v);

  d2zdv2 =  2.0*bv/v/v/v + 6.0*cv/v/v/v/v + 20.0*dv/v/v/v/v/v/v + 30.0*ev/v/v/v/v/v/v/v 
         +  6.0*(fv/v/v/v/v) * (beta + gammav/v/v) * exp(-gammav/v/v)
	 + 14.0*(fv/v/v/v) * (gammav/v/v/v) * exp(-gammav/v/v) 
	 - 14.0*(fv/v/v/v) * (gammav/v/v/v) * (beta + gammav/v/v) * exp(-gammav/v/v)
	 -  8.0*(fv/v/v) * (gammav/v/v/v) * (gammav/v/v/v) * exp(-gammav/v/v)
	 +  4.0*(fv/v/v) * (gammav/v/v/v) * (gammav/v/v/v) * (beta + gammav/v/v) * exp(-gammav/v/v);
  d2zdvdt = - dbvdt/v/v - 2.0*dcvdt/v/v/v + - 4.0*ddvdt/v/v/v/v/v - 5.0*devdt/v/v/v/v/v/v 
            - 2.0*(dfvdt/v/v/v) * (beta + gammav/v/v) * exp(-gammav/v/v)
	    - 2.0*(dfvdt/v/v) * (gammav/v/v/v) * exp(-gammav/v/v)
	    + 2.0*(dfvdt/v/v) * (beta + gammav/v/v) * (gammav/v/v/v) * exp(-gammav/v/v);
  d2zdt2 = d2bvdt2/v + d2cvdt2/v/v + d2dvdt2/v/v/v/v + d2evdt2/v/v/v/v/v + (d2fvdt2/v/v) * (beta + gammav/v/v) * exp(-gammav/v/v);
  
  *vPt = v;
  *zPt = z;
  *phi = exp(lnPhiH2O);
  *dvdp = 1.0/( p*(dzdv/z - 1.0/v) );
  *dvdt = (1.0/t + dzdt/z)/(1.0/v - dzdv/z);
  *d2vdp2  = p*(1.0/v-dzdv/z)/v - dzdv/(*dvdp)/z + 1.0/(*dvdp)/(*dvdp)/p + p*d2zdv2/z;
  *d2vdp2 *= -(*dvdp)*(*dvdp)*(*dvdp);
  *d2vdtdp = -(*dvdp)*(1.0/t + dzdt/z) - p*(*d2vdp2)*(1.0/t + dzdt/z) - p*(*dvdp)*(*dvdp)*(-dzdv*dzdt/z/z + d2zdvdt/z);
  *d2vdt2  = -p*(*d2vdtdp)*(1.0/t + dzdt/z) + p*(*dvdp)/t/t + p*(*dvdp)*dzdt*(dzdv*(*dvdt) + dzdt)/z/z - p*(*dvdp)*(*dvdt)*d2zdvdt/z - p*(*dvdp)*d2zdt2/z;
}

static void duanCO2(double t, double p, double *vPt, double *zPt, double *phi, double *dvdt, double *dvdp, double *d2vdt2, double *d2vdtdp, double *d2vdp2) {
  double bv, cv, dv, ev, fv, beta, gammav, v, z, dzdv, dzdt, d2zdv2, d2zdvdt, d2zdt2;
  double dbvdt, dcvdt, ddvdt, devdt, dfvdt, d2bvdt2, d2cvdt2, d2dvdt2, d2evdt2, d2fvdt2, d3bvdt3;
  double bvPrime[2], cvPrime[2], dvPrime[2], evPrime[2], fvPrime[2], betaPrime[2], gammavPrime[2];
  double dbvPrimedt[2], d2bvPrimedt2[2], d3bvPrimedt3[2];
  double zRef, vRef, phiRef, lnPhiCO2;
  double x[2] = { 0.0, 1.0 }; /* CO2 */
  int useLowPcoeff, iter;
  
  zRef = 1.0;	  
  vRef = DBL_MAX; 
  phiRef = 1.0;
  
  useLowPcoeff = (p <= 2000.0) ? 1 : 0;
  
  if (!useLowPcoeff) {
    BVcAndDerivative	(1, t, x, &bv,     bvPrime,    &dbvdt,    &d2bvdt2,  &d3bvdt3, dbvPrimedt, d2bvPrimedt2, d3bvPrimedt3);
    CVcAndDerivative	(1, t, x, &cv,     cvPrime,    &dcvdt,    &d2cvdt2);
    DVcAndDerivative	(1, t, x, &dv,     dvPrime,    &ddvdt,    &d2dvdt2);
    EVcAndDerivative	(1, t, x, &ev,     evPrime,    &devdt,    &d2evdt2);
    FVcAndDerivative	(1, t, x, &fv,     fvPrime,    &dfvdt,    &d2fvdt2);
    BetaAndDerivative	(1, t, x, &beta,   betaPrime);
    GammaVcAndDerivative(1, t, x, &gammav, gammavPrime);
    
    iter = 0;
    v = 8.314467*t/2000.0;
    while (iter < 100) {
      double delv;
      z = 1 + bv/v + cv/v/v + dv/v/v/v/v + ev/v/v/v/v/v + (fv/v/v) * (beta + gammav/v/v) * exp(-gammav/v/v);
      delv = z*8.314467*t/2000.0 - v;
      if (fabs(delv) < v*100.0*DBL_EPSILON) break;
      v = (z*8.314467*t/2000.0 + v)/2.0;
      iter++;
    }
    if (iter >= 100) printf("zRef = %g, vRef = %g, iter = %d\n", z, v, iter);
    
    lnPhiCO2  = log(phiRef);
    lnPhiCO2 += -log(z) + log(zRef);
    lnPhiCO2 += bvPrime[CO2]/v;
    lnPhiCO2 += cvPrime[CO2]/2.0/v/v;
    lnPhiCO2 += dvPrime[CO2]/4.0/v/v/v/v;
    lnPhiCO2 += evPrime[CO2]/5.0/v/v/v/v/v;
    lnPhiCO2 += ((fvPrime[CO2]*beta + betaPrime[CO2]*fv)/2.0/gammav)*(1.0-exp(-gammav/v/v));
    lnPhiCO2 += ((fvPrime[CO2]*gammav+gammavPrime[CO2]*fv-fv*beta*(gammavPrime[CO2]-gammav))/2.0/gammav/gammav)
    	       *(1.0 - (gammav/v/v + 1.0)*exp(-gammav/v/v));
    lnPhiCO2 += ((gammavPrime[CO2]-gammav)*fv/2.0/gammav/gammav)*(-2.0 + (gammav*gammav/v/v/v/v + 2.0*gammav/v/v + 2.0)*exp(-gammav/v/v));

    phiRef = exp(lnPhiCO2);
  }
  
  BVcAndDerivative    (useLowPcoeff, t, x, &bv,     bvPrime,    &dbvdt,    &d2bvdt2,  &d3bvdt3, dbvPrimedt, d2bvPrimedt2, d3bvPrimedt3);
  CVcAndDerivative    (useLowPcoeff, t, x, &cv,     cvPrime,    &dcvdt,    &d2cvdt2);
  DVcAndDerivative    (useLowPcoeff, t, x, &dv,     dvPrime,	&ddvdt,    &d2dvdt2);
  EVcAndDerivative    (useLowPcoeff, t, x, &ev,     evPrime,	&devdt,    &d2evdt2);
  FVcAndDerivative    (useLowPcoeff, t, x, &fv,     fvPrime,	&dfvdt,    &d2fvdt2);
  BetaAndDerivative   (useLowPcoeff, t, x, &beta,   betaPrime);
  GammaVcAndDerivative(useLowPcoeff, t, x, &gammav, gammavPrime);
  
  if (!useLowPcoeff) {
    iter = 0;
    v = 8.314467*t/2000.0;
    while (iter < 100) {
      double delv;
      z = 1 + bv/v + cv/v/v + dv/v/v/v/v + ev/v/v/v/v/v + (fv/v/v) * (beta + gammav/v/v) * exp(-gammav/v/v);
      delv = z*8.314467*t/2000.0 - v;
      if (fabs(delv) < v*100.0*DBL_EPSILON) break;
      v = (z*8.314467*t/2000.0 + v)/2.0;
      iter++;
    }
    if (iter >= 100) printf("zRef = %g, vRef = %g, iter = %d\n", z, v, iter);
    zRef = z;
    vRef = v;
  }

  iter = 0;
  v = 8.314467*t/p;
  while (iter < 100) {
    double delv;
    z = 1 + bv/v + cv/v/v + dv/v/v/v/v + ev/v/v/v/v/v + (fv/v/v) * (beta + gammav/v/v) * exp(-gammav/v/v);
    delv = z*8.314467*t/p - v;
    if (fabs(delv) < v*100.0*DBL_EPSILON) break;
    v = (z*8.314467*t/p + v)/2.0;
    iter++;
  }
  if (iter >= 100) printf("z = %g, v = %g, iter = %d\n", z, v, iter);

  lnPhiCO2  = log(phiRef);
  lnPhiCO2 += -log(z) + log(zRef);
  lnPhiCO2 += bvPrime[CO2]/v             - (!useLowPcoeff ? bvPrime[CO2]/vRef                         : 0.0);
  lnPhiCO2 += cvPrime[CO2]/2.0/v/v       - (!useLowPcoeff ? cvPrime[CO2]/2.0/vRef/vRef                : 0.0);
  lnPhiCO2 += dvPrime[CO2]/4.0/v/v/v/v   - (!useLowPcoeff ? dvPrime[CO2]/4.0/vRef/vRef/vRef/vRef      : 0.0);
  lnPhiCO2 += evPrime[CO2]/5.0/v/v/v/v/v - (!useLowPcoeff ? evPrime[CO2]/5.0/vRef/vRef/vRef/vRef/vRef : 0.0);
  lnPhiCO2 += ((fvPrime[CO2]*beta + betaPrime[CO2]*fv)/2.0/gammav)*(1.0-exp(-gammav/v/v));
  lnPhiCO2 += ((fvPrime[CO2]*gammav+gammavPrime[CO2]*fv-fv*beta*(gammavPrime[CO2]-gammav))/2.0/gammav/gammav)
             *(1.0 - (gammav/v/v + 1.0)*exp(-gammav/v/v));
  lnPhiCO2 += ((gammavPrime[CO2]-gammav)*fv/2.0/gammav/gammav)*(-2.0 + (gammav*gammav/v/v/v/v + 2.0*gammav/v/v + 2.0)*exp(-gammav/v/v));
  if (!useLowPcoeff) {
    lnPhiCO2 -= ((fvPrime[CO2]*beta + betaPrime[CO2]*fv)/2.0/gammav)*(1.0-exp(-gammav/vRef/vRef));
    lnPhiCO2 -= ((fvPrime[CO2]*gammav+gammavPrime[CO2]*fv-fv*beta*(gammavPrime[CO2]-gammav))/2.0/gammav/gammav)
               *(1.0 - (gammav/vRef/vRef + 1.0)*exp(-gammav/vRef/vRef));
    lnPhiCO2 -= ((gammavPrime[CO2]-gammav)*fv/2.0/gammav/gammav)
               *(-2.0 + (gammav*gammav/vRef/vRef/vRef/vRef + 2.0*gammav/vRef/vRef + 2.0)*exp(-gammav/vRef/vRef));
  }
  
  dzdv = - bv/v/v - 2.0*cv/v/v/v + - 4.0*dv/v/v/v/v/v - 5.0*ev/v/v/v/v/v/v 
         - 2.0*(fv/v/v/v) * (beta + gammav/v/v) * exp(-gammav/v/v)
	 - 2.0*(fv/v/v) * (gammav/v/v/v) * exp(-gammav/v/v)
	 + 2.0*(fv/v/v) * (beta + gammav/v/v) * (gammav/v/v/v) * exp(-gammav/v/v);
  dzdt = dbvdt/v + dcvdt/v/v + ddvdt/v/v/v/v + devdt/v/v/v/v/v + (dfvdt/v/v) * (beta + gammav/v/v) * exp(-gammav/v/v);

  d2zdv2 =  2.0*bv/v/v/v + 6.0*cv/v/v/v/v + 20.0*dv/v/v/v/v/v/v + 30.0*ev/v/v/v/v/v/v/v 
         +  6.0*(fv/v/v/v/v) * (beta + gammav/v/v) * exp(-gammav/v/v)
	 + 14.0*(fv/v/v/v) * (gammav/v/v/v) * exp(-gammav/v/v) 
	 - 14.0*(fv/v/v/v) * (gammav/v/v/v) * (beta + gammav/v/v) * exp(-gammav/v/v)
	 -  8.0*(fv/v/v) * (gammav/v/v/v) * (gammav/v/v/v) * exp(-gammav/v/v)
	 +  4.0*(fv/v/v) * (gammav/v/v/v) * (gammav/v/v/v) * (beta + gammav/v/v) * exp(-gammav/v/v);
  d2zdvdt = - dbvdt/v/v - 2.0*dcvdt/v/v/v + - 4.0*ddvdt/v/v/v/v/v - 5.0*devdt/v/v/v/v/v/v 
            - 2.0*(dfvdt/v/v/v) * (beta + gammav/v/v) * exp(-gammav/v/v)
	    - 2.0*(dfvdt/v/v) * (gammav/v/v/v) * exp(-gammav/v/v)
	    + 2.0*(dfvdt/v/v) * (beta + gammav/v/v) * (gammav/v/v/v) * exp(-gammav/v/v);
  d2zdt2 = d2bvdt2/v + d2cvdt2/v/v + d2dvdt2/v/v/v/v + d2evdt2/v/v/v/v/v + (d2fvdt2/v/v) * (beta + gammav/v/v) * exp(-gammav/v/v);
  
  *vPt = v;
  *zPt = z;
  *phi = exp(lnPhiCO2);
  *dvdp = 1.0/( p*(dzdv/z - 1.0/v) );
  *dvdt = (1.0/t + dzdt/z)/(1.0/v - dzdv/z);
  *d2vdp2  = p*(1.0/v-dzdv/z)/v - dzdv/(*dvdp)/z + 1.0/(*dvdp)/(*dvdp)/p + p*d2zdv2/z;
  *d2vdp2 *= -(*dvdp)*(*dvdp)*(*dvdp);
  *d2vdtdp = -(*dvdp)*(1.0/t + dzdt/z) - p*(*d2vdp2)*(1.0/t + dzdt/z) - p*(*dvdp)*(*dvdp)*(-dzdv*dzdt/z/z + d2zdvdt/z);
  *d2vdt2  = -p*(*d2vdtdp)*(1.0/t + dzdt/z) + p*(*dvdp)/t/t + p*(*dvdp)*dzdt*(dzdv*(*dvdt) + dzdt)/z/z - p*(*dvdp)*(*dvdt)*d2zdvdt/z - p*(*dvdp)*d2zdt2/z;
}

/*
 *======================
 * Pure phase properties
 *
 */
void propertiesOfPureH2O(double t, double p, 
     double *g, double *h, double *s, double *cp, double *dcpdt,
     double *v, double *dvdt, double *dvdp, double *d2vdt2, double *d2vdtdp, double *d2vdp2) {
  double z, phi; 
     
  idealGasH2O(t, cp, s, h, dcpdt);
  duanH2O(t, p, v, &z, &phi, dvdt, dvdp, d2vdt2, d2vdtdp, d2vdp2);

  *g = *h - t*(*s) + R*t*log(phi*p);
       /* enthalpy requires correction      */
       /* entropy requires correction       */
       /* heat capacity requires correction */
       /* dcpdt requires correction         */
} 

void propertiesOfPureCO2(double t, double p, 
     double *g, double *h, double *s, double *cp, double *dcpdt,
     double *v, double *dvdt, double *dvdp, double *d2vdt2, double *d2vdtdp, double *d2vdp2) {
  double z, phi; 
     
  idealGasCO2(t, cp, s, h, dcpdt);
  duanCO2(t, p, v, &z, &phi, dvdt, dvdp, d2vdt2, d2vdtdp, d2vdp2);

  *g = *h - t*(*s) + R*t*log(phi*p);
       /* enthalpy requires correction      */
       /* entropy requires correction       */
       /* heat capacity requires correction */
       /* dcpdt requires correction         */
} 

/*
 *=============================================================================
 * Public functions:
 *    mask  -  bitwise mask for selecting output
 *    t     -  Temperature (K)
 *    p     -  Pressure (bars)
 *    *x    -  (pointer to x[]) Array of independent compositional variables
 */
int
testFlu(int mask, double t, double p,
  int na,          /* Expected number of endmember components                 */
  int nr,          /* Expected number of independent compositional variables  */
  char **names,    /* array of strings of names of endmember components       */
  char **formulas, /* array of strings of formulas of endmember components    */
  double *r,       /* array of indepependent compos variables, check bounds   */
  double *m)       /* array of moles of endmember components, check bounds    */
{
  const char *phase = "fluidPhase.c";
  const char *NAMES[NA]    = { "H2O", "CO2" };
  const char *FORMULAS[NA] = { "H2O", "CO2" };
  int result = TRUE, i;
  double sum;

  if (mask & FIRST) {
    result = result && (na == NA);
    if (!result) printf("<<%s>> Wrong number of components!\n", phase);
  }
  if (mask & SECOND) {
    result = result && (nr == NR);
    if (!result) printf("<<%s>> Wrong number of indep variables!\n", phase);
  }
  if (mask & THIRD) {
    for (i=0; i<NA; i++) {
      result = result && (strcmp(names[i],NAMES[i]) == 0);
      if (!result)
        printf("<<%s>> Component[%d] should be %s not %s.\n",
          phase, i, NAMES[i], names[i]);
    }
  }
  if (mask & FOURTH) {
    for (i=0; i<NA; i++) {
      result = result && (strcmp(formulas[i],FORMULAS[i]) == 0);
      if (!result)
        printf("<<%s>> Component[%d] should have formula %s not %s.\n",
          phase, i, FORMULAS[i], formulas[i]);
    }
  }
  /* Check bounds on the independent compositional variables */
  if (mask & FIFTH) {
    for (i=0, sum=0.0; i<NR; i++) {
      result = result && (r[i] >= 0.0) && (r[i] <= 1.0);
      sum += r[i];
    }
    result = result && (sum <= 1.0);
  }
  /* Check bounds on moles of endmember components */
  if (mask & SIXTH) {
    for (i=0; i<NA; i++) result = result && (m[i] >= 0.0);
  }

  return result;
}

void
conFlu(int inpMask, int outMask, double t, double p,
  double *e,      /* comp of fluid phase in moles of elements                     */
  double *m,      /* comp of fluid phase in moles of endmember components         */
  double *r,      /* comp of fluid phase in terms of the independent comp var     */
  double *x,      /* comp of fluid phase in mole fractions of endmember comp      */
  double **dm,    /* Jacobian matrix: dm[i][j] = dr[i]/dm[j]                  */
  double ***d2m,  /* vector of matrices: d2m[i][j][k] = d2r[i]/dm[j]dm[k]     */
  double **dr,    /* Jacobian matrix: dr[i][j] = dx[i]/dr[j]                  */
  double ****d3m) /* 3rd deriv matrix: d3m[i][j][k][l]=d3r[i]/dm[j]dm[k]dm[l] */
{
  /*---------------------------------------------------------------------------
  Not all combinations of inpMask and outMask are feasible. Valid
    combinations are:

       inpMask          outMask
  (1)  FIRST            SECOND
  (2)  SECOND           THIRD  | FOURTH  | FIFTH | SIXTH | EIGHTH
  (3)  THIRD            FOURTH | SEVENTH

  (1) converts a vector of moles of elements into a vector of moles of 
      endmember fluid phase components.
  (2) calculates from a vector of moles of endmember components, one or
      all of: r[], x[], dr[]/dm[], d2r[]/dm[]dm[], or d3r[]/dm[]dm[]dm[]
  (3) calculates from a vector of independent compositional variables
      mole fractions of endmember components and/or the Jacobian matrix
      dx[]/dr[]

  In this routine it is assumed that the elements are in the order of atomic 
  numbers and that the order of fluid phase components has been verified as:
        m[0] = water          (H2O),
        m[1] = carbon dioxide (CO2) . 

  ----------------------------------------------------------------------------*/

  int i, j, k;

  if (inpMask == FIRST && outMask == SECOND) {
    static const int H = 1;
    static const int C = 6;

    m[0] = e[H]/2.0; /* moles of H2O */
    m[1] = e[C];     /* Moles of CO2 */

  } else if (inpMask == SECOND) {
    double sum;

    if (outMask & ~(THIRD | FOURTH | FIFTH | SIXTH | EIGHTH))
      printf("Illegal call to conFlu with inpMask = %o and outMask = %o\n",
        inpMask, outMask);

    for (i=0, sum=0.0; i<NA; i++) sum += m[i];

    if (outMask & THIRD) {
      for (i=0; i<NR; i++) r[i] = (sum != 0.0) ? m[i+1]/sum : 0.0; 
    }  

    if (outMask & FOURTH) {
      /* Converts a vector of moles of end-member components (m) into a vector
         of mole fractions of endmember components                            */
      for (i=0; i<NA; i++) x[i] = (sum != 0.0) ? m[i]/sum : 0.0; 
    }

    if (outMask & FIFTH) {
      /* Calculates the matrix dr[i]/dm[j] using m[] as input                 */
      if (sum == 0.0) {
        for (i=0; i<NR; i++) { for (j=0; j<NA; j++) dm[i][j] = 0.0; }
      } else {
        for (i=0; i<NR; i++) {
          for (j=0; j<NA; j++)
            dm[i][j] = (i+1 == j) ? (1.0-m[i+1]/sum)/sum : - m[i+1]/SQUARE(sum);
        }
      }

    }

    if (outMask & SIXTH) {
      /* Calculates the matrix d2r[i]/dm[j]dm[k] using m[] as input           */

      if (sum == 0.0) {
        for (i=0; i<NR; i++) {
          for (j=0; j<NA; j++)  {
            for (k=0; k<NA; k++) d2m[i][j][k] = 0.0;
          }
        }
      } else {
        for (i=0; i<NR; i++) {
          for (j=0; j<NA; j++)  {
            for (k=0; k<NA; k++) {
              d2m[i][j][k]  = 2.0*m[i+1]/CUBE(sum);
              d2m[i][j][k] -= (i+1 == j) ? 1.0/SQUARE(sum) : 0.0;
              d2m[i][j][k] -= (i+1 == k) ? 1.0/SQUARE(sum) : 0.0;
            }
          }
        }
      }
    }

    if (outMask & EIGHTH) {
      /* calculates the matrix d3r[i]/dm[j]dm[k]dm[l] using m[] as input	*/
      int l;

      if (sum == 0.0) {
        for (i=0; i<NR; i++) {
          for (j=0; j<NA; j++)  {
            for (k=0; k<NA; k++)  {
	      for (l=0; l<NA; l++) d3m[i][j][k][l] = 0.0;
	    }
          }
        }
      } else {
        for (i=0; i<NR; i++) {
          for (j=0; j<NA; j++)  {
            for (k=0; k<NA; k++)  {
	      for (l=0; l<NA; l++)  {
                d3m[i][j][k][l]  = -6.0*m[i+1]/QUARTIC(sum);
                d3m[i][j][k][l] += (i+1 == j) ? 2.0/CUBE(sum) : 0.0;
                d3m[i][j][k][l] += (i+1 == k) ? 2.0/CUBE(sum) : 0.0;
                d3m[i][j][k][l] += (i+1 == l) ? 2.0/CUBE(sum) : 0.0;
	      }
            }
          }
        }
      }

    }

  } else if (inpMask == THIRD) {

    if (outMask & ~(FOURTH | SEVENTH))
      printf("Illegal call to conFlu with inpMask = %o and outMask = %o\n",
        inpMask, outMask);

    if (outMask & FOURTH) {
      /* Converts a vector of independent compositional variables (r) 
         into a vector of mole fractions of endmember components (x).         */

      for (i=0, x[0]=1.0; i<NR; i++) { x[i+1] = r[i]; x[0] -= r[i]; }
    }

    if (outMask & SEVENTH) {
      /* computes the Jacobian matrix dr[i][j] = dx[i]/dr[j] */
      for (i=0; i<NR; i++) for (j=0; j<NR; j++) dr[i+1][j] = (i == j) ? 1.0 : 0.0;
                           for (j=0; j<NR; j++) dr[0][j]   = -1.0;
    }

  } else  {
    printf("Illegal call to conFlu with inpMask = %o and outMask = %o\n",
      inpMask, outMask);
  }

}

void
dispFlu(int mask, double t, double p, double *x,
  char **formula            /* Mineral formula for interface display MASK: 1 */
  )
{
  double *r = x;
  static char masterString[] = {
/*             1111111111222222222233333333334444444444555555555566666666667
     01234567890123456789012345678901234567890123456789012345678901234567890 */
    "H2O_.__CO2_.__" };

  if (mask & FIRST) {
    char *string = strcpy((char *) malloc((size_t) (strlen(masterString)+1)*sizeof(char)), masterString);
    char n[5];
    int i;

    (void) snprintf(n, 5, "%4.2f", r[0]    ); for (i=0; i<4; i++) string[ 3+i] = n[i];
    (void) snprintf(n, 5, "%4.2f", 1.0-r[0]); for (i=0; i<4; i++) string[10+i] = n[i];

    *formula = string;
  }
}

void 
actFlu(int mask, double t, double p, double *r, 
  double *a,  /* (pointer to a[]) activities              BINARY MASK: 0001 */
  double *mu, /* (pointer to mu[]) chemical potentials    BINARY MASK: 0010 */
  double **dx /* (pointer to dx[][]) d(a[])/d(x[])        BINARY MASK: 0100 */
  )           /* exclusion criteria applied to results if BINARY MASK: 1000 */
{
  double x[2], v, z, phi[2], dvdt, dvdp, d2vdt2, d2vdtdp, d2vdp2; 
  double vH2O, vCO2, zH2O, zCO2, phiH2O, phiCO2, dvdtH2O, dvdpH2O, dvdtCO2, dvdpCO2, d2vdt2H2O, d2vdtdpH2O, d2vdp2H2O, d2vdt2CO2, d2vdtdpCO2, d2vdp2CO2;
  double g, dgdr[NR], fr[NA][NR];
  int i, j;

  x[H2O] = 1.0 - r[0];
  x[CO2] = r[0];

  duan(t, p, x, &v, &z, phi, &dvdt, &dvdp, &d2vdt2, &d2vdtdp, &d2vdp2);
  duanH2O(t, p, &vH2O, &zH2O, &phiH2O, &dvdtH2O, &dvdpH2O, &d2vdt2H2O, &d2vdtdpH2O, &d2vdp2H2O);
  duanCO2(t, p, &vCO2, &zCO2, &phiCO2, &dvdtCO2, &dvdpCO2, &d2vdt2CO2, &d2vdtdpCO2, &d2vdp2CO2);
  
  for(i=0; i<NA; i++) fr[i][0] = FR0(i);

  g       = 0.0;
  dgdr[0] = 0.0;

  if (mask & FIRST) {
    a[H2O] = (phiH2O != 0.0) ? x[H2O]*phi[H2O]/phiH2O : 0.0;
    a[CO2] = (phiCO2 != 0.0) ? x[CO2]*phi[CO2]/phiCO2 : 0.0; 
  }

  if (mask & SECOND) {
    mu[H2O] = ((phi[H2O] != 0.0) || (phiH2O != 0.0)) ? R*t*log(x[H2O]*phi[H2O]/phiH2O) : 0.0;
    mu[CO2] = ((phi[CO2] != 0.0) || (phiCO2 != 0.0)) ? R*t*log(x[CO2]*phi[CO2]/phiCO2) : 0.0; 
  }

  if (mask & THIRD) {
    double d2gdr2[NR][NR], dfrdr[NA][NR], sum;
    int k;

    d2gdr2[0][0] = 0.0;

    for(i=0; i<NA; i++) dfrdr[i][0] = DFR0DR0(i);

    for (i=0; i<NA; i++) {
       for (k=0; k<NR; k++) {
          for (dx[i][k]=g, j=0; j<NR; j++) dx[i][k] += fr[i][j]*dgdr[j];
          dx[i][k] = exp(dx[i][k]/(R*t));
          sum = (1.0+dfrdr[i][k])*dgdr[k];
          for (j=0; j<NR; j++) sum += fr[i][j]*d2gdr2[j][k];
          dx[i][k] *= sum/(R*t);
       }
    }  
  }

  if (mask & FOURTH) {
    /* 
    static const double exclusion[NA] = { 0.05,  0.05 };

    for (i=0; i<NA; i++) {
      if (x[i] < exclusion[i]) {
        if (mask & FIRST)  a[i]  = 0.0;
        if (mask & SECOND) mu[i] = 0.0;
        if (mask & THIRD)  for (j=0; j<NR; j++) dx[i][j] = 0.0;
      }
    }
    */
  }

}

void 
gmixFlu(int mask, double t, double p, double *r, 
  double *gmix, /* Gibbs energy of mixing             BINARY MASK: 0001 */
  double *dx,   /* (pointer to dx[]) d(g)/d(x[])      BINARY MASK: 0010 */
  double **dx2, /* (pointer to dx2[][]) d2(g)/d(x[])2 BINARY MASK: 0100 */
  double ***dx3 /* (pointer to dx3[][][]) d3(g)/d(x[])3 NARY MASK: 1000 */
  )
{
  double x[2], v, z, phi[2], dvdt, dvdp, d2vdt2, d2vdtdp, d2vdp2; 
  double vH2O, vCO2, zH2O, zCO2, phiH2O, phiCO2, dvdtH2O, dvdpH2O, dvdtCO2, dvdpCO2, d2vdt2H2O, d2vdtdpH2O, d2vdp2H2O, d2vdt2CO2, d2vdtdpCO2, d2vdp2CO2;

  /*
  idealGas(t, cp, s0, h0);
  printf("H2O at t = %g K Cp = %g, s0 = %g, h0 = %g\n", t, cp[H2O], s0[H2O] + 359.6505, h0[H2O] - 355665.4136);
  printf("CO2 at t = %g K Cp = %g, s0 = %g, h0 = %g\n", t, cp[CO2], s0[CO2] + 210.0304, h0[CO2] - 385358.2260);
  */
  
  x[H2O] = 1.0 - r[0];
  x[CO2] = r[0];

  duan(t, p, x, &v, &z, phi, &dvdt, &dvdp, &d2vdt2, &d2vdtdp, &d2vdp2);
  duanH2O(t, p, &vH2O, &zH2O, &phiH2O, &dvdtH2O, &dvdpH2O, &d2vdt2H2O, &d2vdtdpH2O, &d2vdp2H2O);
  duanCO2(t, p, &vCO2, &zCO2, &phiCO2, &dvdtCO2, &dvdpCO2, &d2vdt2CO2, &d2vdtdpCO2, &d2vdp2CO2);

  if (mask & FIRST) {
    *gmix = ((phi[H2O] != 0.0) && (phi[CO2] != 0.0)) ? R*t*(x[H2O]*log(x[H2O]*phi[H2O]/phiH2O) + x[CO2]*log(x[CO2]*phi[CO2]/phiCO2)) : 0.0;
  }
  
  if(mask & SECOND) {
    dx[0] = ((phi[H2O] != 0.0) && (phi[CO2] != 0.0)) ? R*t*(log(x[CO2]*phi[CO2]/phiCO2) - log(x[H2O]*phi[H2O]/phiH2O)) : 0.0;;
  }

  if(mask & THIRD) {
    double d2gdr2[NR][NR];
    int i, j;

    d2gdr2[0][0] = 0.0;

    for (i=0; i<NR; i++) {
       for (j=0; j<NR; j++) dx2[i][j] = d2gdr2[i][j];
    }
  }

  if (mask & FOURTH) {
   double d3gdr3[NR][NR][NR];
   int i, j, k;

    d3gdr3[0][0][0] = 0.0;

    for (i=0; i<NR; i++) {
      for (j=0; j<NR; j++) {
	for (k=0; k<NR; k++) dx3[i][j][k] = d3gdr3[i][j][k];
      }
    }
  }

}

void 
hmixFlu(int mask, double t, double p, double *x, 
  double *hmix /* Enthalpy of mixing BINARY MASK: 1 */
  )
{
  *hmix = 0.0;
}

void 
smixFlu(int mask, double t, double p, double *x, 
  double *smix, /* Entropy of mixing                  BINARY MASK: 001 */
  double *dx,   /* (pointer to dx[]) d(s)/d(x[])      BINARY MASK: 010 */
  double **dx2  /* (pointer to dx2[][]) d2(s)/d(x[])2 BINARY MASK: 100 */
  )
{
  if (mask & FIRST) {
    *smix = 0.0; 
  }
  
  if(mask & SECOND) {
    double d2gdrdt[NR];
    int i;

    d2gdrdt[0] = 0.0;

    for (i=0; i<NR; i++) dx[i] = - d2gdrdt[i];
  }

  if(mask & THIRD) {
    double d3gdr2dt[NR][NR];
    int i, j;

    d3gdr2dt[0][0] = 0.0; 

    for (i=0; i<NR; i++) {
       for (j=0; j<NR; j++) dx2[i][j] = - d3gdr2dt[i][j];
    }
  }

}

void 
cpmixFlu(int mask, double t, double p, double *x, 
  double *cpmix, /* Heat capacity of mixing               BINARY MASK: 001 */
  double *dt,    /* d(cp)/d(t)                            BINARY MASK: 010 */
  double *dx     /* d(cp)/d(x[])d(t)                      BINARY MASK: 100 */
  )
{
  double d2gdt2 = 0.0;

  if (mask & FIRST) {
    *cpmix = - t*d2gdt2;
  }

  if(mask & SECOND) {
    double d3gdt3   = 0.0;

    *dt = -t*d3gdt3 - d2gdt2;
  }

  if(mask & THIRD) {
    double d3gdrdt2[NR];
    int i;

    d3gdrdt2[0] = 0.0;

    for (i=0; i<NR; i++) dx[i] = -t*d3gdrdt2[i];
  }

}

void 
vmixFlu(int mask, double t, double p, double *r, 
  double *vmix, /* Volume of mixing                BINARY MASK: 0000000001 */
  double *dx,   /* (pointer to dx[]) d(v)/d(x[])   BINARY MASK: 0000000010 */
  double **dx2, /* (point to dx2[][]) d(v)/d(x[])2 BINARY MASK: 0000000100 */
  double *dt,   /* d(v)/d(t)                       BINARY MASK: 0000001000 */
  double *dp,   /* d(v)/d(p)                       BINARY MASK: 0000010000 */
  double *dt2,  /* d2(v)/d(t)2                     BINARY MASK: 0000100000 */
  double *dtdp, /* d2(v)/d(t)d(p)                  BINARY MASK: 0001000000 */
  double *dp2,  /* d2(v)/d(p)2                     BINARY MASK: 0010000000 */
  double *dxdt, /* d2(v)/d(x[])d(t)                BINARY MASK: 0100000000 */
  double *dxdp  /* d2(v)/d(x[])d(p)                BINARY MASK: 1000000000 */
  )
{
  double x[2], v, z, phi[2], dvdt, dvdp, d2vdt2, d2vdtdp, d2vdp2; 
  double vH2O, vCO2, zH2O, zCO2, phiH2O, phiCO2, dvdtH2O, dvdpH2O, dvdtCO2, dvdpCO2, d2vdt2H2O, d2vdtdpH2O, d2vdp2H2O, d2vdt2CO2, d2vdtdpCO2, d2vdp2CO2;

  x[H2O] = 1.0 - r[0];
  x[CO2] = r[0];

  duan(t, p, x, &v, &z, phi, &dvdt, &dvdp, &d2vdt2, &d2vdtdp, &d2vdp2);
  duanH2O(t, p, &vH2O, &zH2O, &phiH2O, &dvdtH2O, &dvdpH2O, &d2vdt2H2O, &d2vdtdpH2O, &d2vdp2H2O);
  duanCO2(t, p, &vCO2, &zCO2, &phiCO2, &dvdtCO2, &dvdpCO2, &d2vdt2CO2, &d2vdtdpCO2, &d2vdp2CO2);

  if (mask & FIRST) {
    *vmix = v - x[0]*vH2O - x[1]*vCO2;
  }

  if(mask & SECOND) {
    double d2gdrdp[NR];
    int i;

    d2gdrdp[0] = 0.0; 

    for (i=0; i<NR; i++) dx[i] = d2gdrdp[i]; 
  }

  if(mask & THIRD) {
    double d3gdr2dp[NR][NR];
    int i, j;

    d3gdr2dp[0][0] = 0.0;

    for (i=0; i<NR; i++) {
       for (j=0; j<NR; j++) dx2[i][j] = d3gdr2dp[i][j];
    }
  }

  if(mask & FOURTH) {
    *dt = dvdt - x[0]*dvdtH2O - x[1]*dvdtCO2;
  }

  if(mask & FIFTH) {
    *dp = dvdp - x[0]*dvdpH2O - x[1]*dvdpCO2;
  }

  if(mask & SIXTH) {
    *dt2 = d2vdt2 - x[0]*d2vdt2H2O - x[1]*d2vdt2CO2;
  }

  if(mask & SEVENTH) {
    *dtdp = d2vdtdp - x[0]*d2vdtdpH2O - x[1]*d2vdtdpCO2;
  }

  if(mask & EIGHTH) {
    *dp2 = d2vdp2 - x[0]*d2vdp2H2O - x[1]*d2vdp2CO2;
  }

  if(mask & NINTH) {
    double d3gdrdtdp[NR];
    int i;

    d3gdrdtdp[0] = 0.0; 

    for (i=0; i<NR; i++) dxdt[i] = d3gdrdtdp[i]; 
  }

  if(mask & TENTH) {
    double d3gdrdp2[NR];
    int i;

    d3gdrdp2[0] = 0.0; 

    for (i=0; i<NR; i++) dxdp[i] = d3gdrdp2[i]; 
  }

}

/* end of file fluidPhase.c */
