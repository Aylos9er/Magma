const char *test_eos_ver(void) { return "$Id: test_SAK.c,v 1.2 2009/05/14 04:24:00 ghiorso Exp $"; }

/*
**++
**  FACILITY:  Silicate Melts Regression/Crystallization Package
**
**  MODULE DESCRIPTION:
**
**      Test routine to evaluate the functions defined for the computation
**      of EOS properties in the liquid 
**      (file: TEST_EOS.C)
**
**  MODIFICATION HISTORY:
**
**      V1.0-1  Mark S. Ghiorso  January 16, 2009 Original Version
**--
*/

#include <X11/Intrinsic.h>
#include <Xm/Xm.h>

#include "interface.h"
Widget main_window, silmin_adb, topLevel;
MeltsEnviron meltsEnviron;

#include "silmin.h"
#include "recipes.h"

#include "liq_struct_data_SiO2_Al2O3_CaO_Na2O_K2O.h"
#include "sol_struct_data_SiO2_Al2O3_CaO_Na2O_K2O.h"
#include "param_struct_data_SiO2_Al2O3_CaO_Na2O_K2O.h"

#include <unistd.h>
#include <grace_np.h>

SilminState *silminState;
int calculationMode = MODE_DEFAULT;

static void grace_error_function(const char *msg)
{
  fprintf(stderr, "library message : \"%s\"\n", msg);
}

int main()
{
  int i, j, k, type;
  int nr = nlc - 1;
  float ftemp;
  double t, pb;
  double p = 1.0;
  double *grams, *moles, *x, *mu, *xSpecies;
  int list[] = { 0, 1, 4, 8, 10, 18, 20, 21, 22, 23, 29, 30, 31, 32, 33, 34}; 
  int nCrst = -1, nTrid = -1, nQtz = -1, nkDi = -1, nkMet = -1, nkTet = -1, nCor = -1, nMul = -1, 
      nSan = -1, nLeu = -1, nKalO = -1, nKalH = -1;
  
  printf("***************************************************\n");
  printf("Calling function: InitComputeDataStruct().	     \n");
  printf("***************************************************\n\n");
  InitComputeDataStruct();
  setModeToMixingLiq(FALSE);

  printf("1    = speciation\n");
  printf("2    = phase diagram\n");
  printf("3(1) = Gibbs energy (mix)\n");
  printf("4(1) = Entropy (mix)\n");
  printf("5(1) = Enthalpy (mix)\n");
  printf("6    = activities\n");
  printf("Input plot type: "); scanf("%d", &type);
  getchar();
  if (type != 2) {
    printf("Input a T (C): "); scanf("%f", &ftemp); t = (double) ftemp + 273.15;
    getchar();
  } else t = 1800.0;
  printf("9999 = SiO2-K2O join\n8888 = SiO2-Al2O3 join\n7777 = K2O-Al2O3 join\n6666 = SiO2-KAlO2 join\n");
  printf("5555 = SiO2-K2O_60_Al2O3_40 join\n4444 = SiO2-K2O_70_Al2O3_30 join\n3333 = SiO2-K2O_80_Al2O3_20 join\n");
  printf("2222 = SiO2-K2O_90_Al2O3_10 join\n1111 = SiO2-K2O_95_Al2O3_05 join\n1234 = SiO2-K2O_40_Al2O3_60 join\n");
  printf("else input a wt ratio of SiO2/Al2O3: "); scanf("%f", &ftemp); pb = (double) ftemp;
  getchar();

  grams    = vector(0, nc-1);
  moles    = vector(0, nlc-1);
  x	   = vector(0, nr-1);
  mu	   = vector(0, nlc-1);
  xSpecies = vector(0, nls-1);

  GraceRegisterErrorFunction (grace_error_function);
  if(GraceOpen (2048) < 0) { printf("Cannot open pipe to Grace.\n"); exit(0); }

  GracePrintf("PAGE LAYOUT PORTRAIT\n");

  GracePrintf("WITH G0\n");
   GracePrintf("VIEW  %lf, %lf, %lf, %lf\n", 0.15, 0.15, 0.85, 1.144118);
   GracePrintf("WORLD %lf, %lf, %lf, %lf\n",  0.0, 500.0, 100.0, 1800.0);

   GracePrintf("XAXIS TICK MAJOR 10.0\n");
   GracePrintf("XAXIS TICK MINOR  5.0\n");
   GracePrintf("XAXIS TICK MAJOR GRID ON\n");
   GracePrintf("XAXIS TICKLABEL CHAR SIZE 0.8\n");

   GracePrintf("XAXIS LABEL \"Wt% K2O\"\n");
   GracePrintf("XAXIS LABEL CHAR SIZE 0.8\n");
   GracePrintf("XAXIS LABEL COLOR 2\n");

   GracePrintf("YAXIS TICK MAJOR 100.00\n");
   GracePrintf("YAXIS TICK MINOR  50.00\n");
   GracePrintf("YAXIS TICK MAJOR GRID ON\n");
   GracePrintf("YAXIS TICKLABEL CHAR SIZE 0.8\n");

   GracePrintf("YAXIS LABEL \"various\"\n");
   GracePrintf("YAXIS LABEL CHAR SIZE 0.8\n");
   GracePrintf("YAXIS LABEL COLOR 2\n");
   
   GracePrintf("LEGEND ON\n");
   GracePrintf("LEGEND 0.5, 1.2\n");
   GracePrintf("LEGEND FONT 6\n");
   GracePrintf("LEGEND CHAR SIZE 0.750000\n");
   GracePrintf("LEGEND VGAP 0\n");
   
   GracePrintf("FRAME ON\n");
 
   if (type == 2) {
     if (pb != 7777.0) {
       GracePrintf("g0.s%1.1d LINESTYLE 1\n", 0);
       GracePrintf("g0.s%1.1d COLOR %1.1d\n", 0, 0+1);
       GracePrintf("g0.s%1.1d SYMBOL 0\n",    0);
       GracePrintf("g0.s%1.1d LEGEND \"%s %s\"\n", 0, "eq", "cristobalite");

       GracePrintf("g0.s%1.1d LINESTYLE 1\n", 1);
       GracePrintf("g0.s%1.1d COLOR %1.1d\n", 1, 1+1);
       GracePrintf("g0.s%1.1d SYMBOL 0\n",    1);
       GracePrintf("g0.s%1.1d LEGEND \"%s %s\"\n", 1, "eq", "tridymite");

       GracePrintf("g0.s%1.1d LINESTYLE 1\n", 2);
       GracePrintf("g0.s%1.1d COLOR %1.1d\n", 2, 2+1);
       GracePrintf("g0.s%1.1d SYMBOL 0\n",    2);
       GracePrintf("g0.s%1.1d LEGEND \"%s %s\"\n", 2, "eq", "quartz");
     }

     if ((pb != 7777.0) && (pb != 8888.0)) {
       GracePrintf("g0.s%1.1d LINESTYLE 1\n", 3);
       GracePrintf("g0.s%1.1d COLOR %1.1d\n", 3, 3+1);
       GracePrintf("g0.s%1.1d SYMBOL 0\n",    3);
       GracePrintf("g0.s%1.1d LEGEND \"%s %s\"\n", 3, "eq", "kdisilicate");

       GracePrintf("g0.s%1.1d LINESTYLE 1\n", 4);
       GracePrintf("g0.s%1.1d COLOR %1.1d\n", 4, 4+1);
       GracePrintf("g0.s%1.1d SYMBOL 0\n",    4);
       GracePrintf("g0.s%1.1d LEGEND \"%s %s\"\n", 4, "eq", "kmetasilicate");

       GracePrintf("g0.s%1.1d LINESTYLE 1\n", 5);
       GracePrintf("g0.s%1.1d COLOR %1.1d\n", 5, 5+1);
       GracePrintf("g0.s%1.1d SYMBOL 0\n",    5);
       GracePrintf("g0.s%1.1d LEGEND \"%s %s\"\n", 5, "eq", "ktetrasilicate");
     }
     
     if (pb != 9999.0) {
       GracePrintf("g0.s%1.1d LINESTYLE 1\n", 6);
       GracePrintf("g0.s%1.1d COLOR %1.1d\n", 6, 6+1);
       GracePrintf("g0.s%1.1d SYMBOL 0\n",    6);
       GracePrintf("g0.s%1.1d LEGEND \"%s %s\"\n", 6, "eq", "corundum");

       GracePrintf("g0.s%1.1d LINESTYLE 1\n", 7);
       GracePrintf("g0.s%1.1d COLOR %1.1d\n", 7, 7+1);
       GracePrintf("g0.s%1.1d SYMBOL 0\n",    7);
       GracePrintf("g0.s%1.1d LEGEND \"%s %s\"\n", 7, "eq", "mullite");
     }
     
     if ((pb != 7777.0) && (pb != 8888.0) && (pb != 9999.0)) {
       GracePrintf("g0.s%1.1d LINESTYLE 1\n", 8);
       GracePrintf("g0.s%1.1d COLOR %1.1d\n", 8, 8+1);
       GracePrintf("g0.s%1.1d SYMBOL 0\n",    8);
       GracePrintf("g0.s%1.1d LEGEND \"%s %s\"\n", 8, "eq", "sanidine");

       GracePrintf("g0.s%1.1d LINESTYLE 1\n", 9);
       GracePrintf("g0.s%1.1d COLOR %1.1d\n", 9, 9+1);
       GracePrintf("g0.s%1.1d SYMBOL 0\n",    9);
       GracePrintf("g0.s%1.1d LEGEND \"%s %s\"\n", 9, "eq", "leucite");

       GracePrintf("g0.s%1.1d LINESTYLE 1\n", 10);
       GracePrintf("g0.s%1.1d COLOR %1.1d\n", 10, 10+1);
       GracePrintf("g0.s%1.1d SYMBOL 0\n",    10);
       GracePrintf("g0.s%1.1d LEGEND \"%s %s\"\n", 10, "eq", "ortho kalsilite");

       GracePrintf("g0.s%1.1d LINESTYLE 1\n", 11);
       GracePrintf("g0.s%1.1d COLOR %1.1d\n", 11, 11+1);
       GracePrintf("g0.s%1.1d SYMBOL 0\n",    11);
       GracePrintf("g0.s%1.1d LEGEND \"%s %s\"\n", 11, "eq", "hexag kalsilite");
     }

     
   } else if ((type == 4) || (type == 41)) {  
     GracePrintf("g0.s%1.1d LINESTYLE 1\n", 5);
     GracePrintf("g0.s%1.1d COLOR %1.1d\n", 5, 5+1);
     GracePrintf("g0.s%1.1d SYMBOL 0\n",    5);
     GracePrintf("g0.s%1.1d LEGEND \"%s %s\"\n", 5, "Entropy", "J/K-100 gm");
     
     GracePrintf("g0.s%1.1d LINESTYLE 1\n", 6);
     GracePrintf("g0.s%1.1d COLOR %1.1d\n", 6, 6+1);
     GracePrintf("g0.s%1.1d SYMBOL 0\n",    6);
     GracePrintf("g0.s%1.1d LEGEND \"%s %s\"\n", 6, "KAlSi2O6 liquid", "J/K-100");
     
     GracePrintf("g0.s%1.1d LINESTYLE 1\n", 7);
     GracePrintf("g0.s%1.1d COLOR %1.1d\n", 7, 7+1);
     GracePrintf("g0.s%1.1d SYMBOL 0\n",    7);
     GracePrintf("g0.s%1.1d LEGEND \"%s %s\"\n", 7, "Leucite", "J/K-100");
   } else if ((type == 5) || (type == 51)) {  
     GracePrintf("g0.s%1.1d LINESTYLE 1\n", 7);
     GracePrintf("g0.s%1.1d COLOR %1.1d\n", 7, 7+1);
     GracePrintf("g0.s%1.1d SYMBOL 0\n",    7);
     GracePrintf("g0.s%1.1d LEGEND \"%s %s\"\n", 7, "Enhalpy", "kJ/100 gm");
     if ((type == 5) &&  (pb == 9999.0)) {
       GracePrintf("g0.s%1.1d LINESTYLE 0\n", 8);
       GracePrintf("g0.s%1.1d COLOR %1.1d\n", 8, 8+1);
       GracePrintf("g0.s%1.1d SYMBOL 1\n",    8);
       GracePrintf("g0.s%1.1d SYMBOL COLOR %1.1d\n", 8, 0+2);
       GracePrintf("g0.s%1.1d LEGEND \"%s %s\"\n", 8, "H (data)", "kJ/100 gm");
     }
   } else if (type == 1) {  
     for (i=10; i<(10+16); i++) {
       GracePrintf("g0.s%2.2d LINESTYLE 1\n", i);
       GracePrintf("g0.s%2.2d COLOR %1.1d\n", i, i-9);
       GracePrintf("g0.s%2.2d SYMBOL 0\n",    i);
       GracePrintf("g0.s%2.2d LEGEND \"%s %s\"\n", i, "X", liquid[list[i-10]].label);
     }
   } else if ((type == 3) || (type == 31)) {
       GracePrintf("g0.s%1.1d LINESTYLE 1\n", 0);
       GracePrintf("g0.s%1.1d COLOR %1.1d\n", 0, 0+1);
       GracePrintf("g0.s%1.1d SYMBOL 0\n",    0);
       GracePrintf("g0.s%1.1d LEGEND \"%s %s\"\n", 0, "Gibbs Energy", "kJ/mol");
   } else if (type == 6) {
       GracePrintf("g0.s%1.1d LINESTYLE 1\n", 0);
       GracePrintf("g0.s%1.1d COLOR %1.1d\n", 0, 0+1);
       GracePrintf("g0.s%1.1d SYMBOL 0\n",    0);
       GracePrintf("g0.s%1.1d LEGEND \"%s %s\"\n", 0, "Si2O4", "kJ/mol");

       GracePrintf("g0.s%1.1d LINESTYLE 1\n", 1);
       GracePrintf("g0.s%1.1d COLOR %1.1d\n", 1, 1+1);
       GracePrintf("g0.s%1.1d SYMBOL 0\n",    1);
       GracePrintf("g0.s%1.1d LEGEND \"%s %s\"\n", 1, "Al2O3", "kJ/mol");

       GracePrintf("g0.s%1.1d LINESTYLE 1\n", 2);
       GracePrintf("g0.s%1.1d COLOR %1.1d\n", 2, 2+1);
       GracePrintf("g0.s%1.1d SYMBOL 0\n",    2);
       GracePrintf("g0.s%1.1d LEGEND \"%s %s\"\n", 2, "K2SiO3", "kJ/mol");
   }

   /*************************************************************************** 
    Compute composition of the liquid phase (moles)
    ***************************************************************************/

   for (i=1; i<100; i++) {
     double wtPotash = (double) i;
     double smix, hmix, gmix;
     grams[4] = wtPotash;                       /* K2O  */
     if (pb == 9999.0) { /* SiO2 - K2O join */
       grams[1] = 0.0;
       grams[0] = 100.0 - wtPotash;

     } else if (pb == 8888.0) { /* SiO2 - Al2O3 join */
       grams[1] = wtPotash;
       grams[0] = 100.0 - wtPotash;
       grams[4] = 0.0;
     
     } else if (pb == 7777.0) { /* K2O - Al2O3 join */
       grams[0] = 0.0;
       grams[1] = 100.0 - wtPotash;

     } else if (pb == 6666.0) { /* KAlO2 - SiO2 join */
       double spb = 48.02/51.98;
       grams[0] = 100.0 - wtPotash;
       grams[1] =     (100.0-grams[0])/(1.0+spb); /* Al2O3 */
       grams[4] = spb*(100.0-grams[0])/(1.0+spb); /* K2O   */
     
     } else if (pb == 5555.0) { /* K2O 60% Al2O3 40% - SiO2 join */
       double spb = 60.0/40.0;
       grams[0] = 100.0 - wtPotash;
       grams[1] =     (100.0-grams[0])/(1.0+spb); /* Al2O3 */
       grams[4] = spb*(100.0-grams[0])/(1.0+spb); /* K2O   */
     
     } else if (pb == 4444.0) { /* K2O 60% Al2O3 40% - SiO2 join */
       double spb = 70.0/30.0;
       grams[0] = 100.0 - wtPotash;
       grams[1] =     (100.0-grams[0])/(1.0+spb); /* Al2O3 */
       grams[4] = spb*(100.0-grams[0])/(1.0+spb); /* K2O   */
     
     } else if (pb == 3333.0) { /* K2O 60% Al2O3 40% - SiO2 join */
       double spb = 80.0/20.0;
       grams[0] = 100.0 - wtPotash;
       grams[1] =     (100.0-grams[0])/(1.0+spb); /* Al2O3 */
       grams[4] = spb*(100.0-grams[0])/(1.0+spb); /* K2O   */
     
     } else if (pb == 2222.0) { /* K2O 60% Al2O3 40% - SiO2 join */
       double spb = 90.0/10.0;
       grams[0] = 100.0 - wtPotash;
       grams[1] =     (100.0-grams[0])/(1.0+spb); /* Al2O3 */
       grams[4] = spb*(100.0-grams[0])/(1.0+spb); /* K2O   */
     
     } else if (pb == 1111.0) { /* K2O 60% Al2O3 40% - SiO2 join */
       double spb = 95.0/5.0;
       grams[0] = 100.0 - wtPotash;
       grams[1] =     (100.0-grams[0])/(1.0+spb); /* Al2O3 */
       grams[4] = spb*(100.0-grams[0])/(1.0+spb); /* K2O   */
     
     } else if (pb == 1234.0) { /* K2O 60% Al2O3 40% - SiO2 join */
       double spb = 40.0/60.0;
       grams[0] = 100.0 - wtPotash;
       grams[1] =     (100.0-grams[0])/(1.0+spb); /* Al2O3 */
       grams[4] = spb*(100.0-grams[0])/(1.0+spb); /* K2O   */
     
     } else {
       grams[1] =    (100.0-grams[4])/(1.0+pb); /* Al2O3 */
       grams[0] = pb*(100.0-grams[4])/(1.0+pb); /* SiO2  */
     }
     grams[2] = 0.0; grams[3] = 0.0; grams[5] = 0.0;

     for (j=0; j<nc; j++) grams[j] /= bulkSystem[j].mw;
     for (j=0; j<nlc; j++) for (k=0,moles[j]=0.0; k<nc; k++) moles[j] += grams[k]*(bulkSystem[k].oxToLiq)[j];

     if (testLiq(SIXTH, t, p, 0, 0, NULL, NULL, NULL, moles)) {
       conLiq(SECOND, THIRD, t, p, NULL, moles, x, NULL, NULL, NULL, NULL);
       conLiq(THIRD, FOURTH | EIGHTH, t, p, NULL, NULL, x, xSpecies, NULL, NULL, NULL);
   
       if (type == 2) {
         static double tCrst = 2000.0, tTrid = 2000.0, tQtz = 1700.0, tkDi = 1900.0, tkMet = 1800.0, tkTet = 1700.0,
	               tCor = 2000.0, tMul = 2000.0, tSan = 2000.0, tLeu = 2000.0, tKalO = 2000.0, tKalH = 2000.0;
	 double resid, tinc;
	 int count;
         if ((nCrst == -1) || (nTrid == -1) || (nQtz == -1) || (nkDi == -1) || (nkMet == -1)|| (nkTet == -1)
	  || (nCor  == -1) || (nMul  == -1) || (nSan == -1) || (nLeu == -1) || (nKalO == -1)|| (nKalH == -1) ) {
           for (k=0; k<npc; k++) {
	     if (!strcmp("cristobalite",   solids[k].label) && (solids[k].type == PHASE)) nCrst = k;
	     if (!strcmp("tridymite",      solids[k].label) && (solids[k].type == PHASE)) nTrid = k;
	     if (!strcmp("quartz",         solids[k].label) && (solids[k].type == PHASE)) nQtz  = k;
	     if (!strcmp("kdisilicate",    solids[k].label) && (solids[k].type == PHASE)) nkDi  = k;
	     if (!strcmp("kmetasilicate",  solids[k].label) && (solids[k].type == PHASE)) nkMet = k;
	     if (!strcmp("ktetrasilicate", solids[k].label) && (solids[k].type == PHASE)) nkTet = k;
	     if (!strcmp("corundum",       solids[k].label) && (solids[k].type == PHASE)) nCor  = k;
	     if (!strcmp("mullite",        solids[k].label) && (solids[k].type == PHASE)) nMul  = k;
	     if (!strcmp("highsanidine",   solids[k].label) && (solids[k].type == PHASE)) nSan  = k;
	     if (!strcmp("leucite",        solids[k].label) && (solids[k].type == PHASE)) nLeu  = k;
	     if (!strcmp("kalsiliteortho", solids[k].label) && (solids[k].type == PHASE)) nKalO = k;
	     if (!strcmp("kalsilitehex",   solids[k].label) && (solids[k].type == PHASE)) nKalH = k;
	   }
	   if ((nCrst == -1) || (nTrid == -1) || (nQtz == -1) || (nkDi == -1) || (nkMet == -1)|| (nkTet == -1)
	    || (nCor  == -1) || (nMul  == -1) || (nSan == -1) || (nLeu == -1) || (nKalO == -1)|| (nKalH == -1) ) {
	     printf("Cannot find index of one or more solid phases:\n");
	     printf("  nCrst(%d), nTrid(%d), nQtz(%d)  nkDi(%d), nkMet(%d), nkTet(%d), nCor(%d), nMul(%d)\n", 
	       nCrst, nTrid, nQtz, nkDi, nkMet, nkTet, nCor, nMul);
	     printf("  nSan(%d), nLeu(%d), nKalO(%d)  nKalH(%d)\n", nSan, nLeu, nKalO, nKalH);
	     return 0;
	   }
	 }
	 printf("Computing phase stablity at wt = %5.1f ", wtPotash);
	 
	 if (pb != 7777.0) {

         printf("C");
	 tinc = -25.0;
	 count = 0;
	 do {
           actLiq(SECOND, tCrst, p, x, NULL, mu, NULL, NULL);	 
	   gibbs(tCrst, p, (char *) solids[nCrst].label, &(solids[nCrst].ref), NULL, NULL, &(solids[nCrst].cur));
	   resid = (mu[0]/2.0-solids[nCrst].cur.g)/1000.0;
	   /* liquid too stable, temperature too high */
	   if ((resid < 0.0) && (tinc > 0.0)) tinc *= -0.5; 
	   /* solid too stable,  temperature too low  */
	   else if ((resid > 0.0) && (tinc < 0.0)) tinc *= -0.5;
	   tCrst += tinc;
	   count++;
	   if (tCrst < 800.0) { count = 100; tCrst = 2000.0; } 
	   /* printf("tCrst = %f, resid = %g, count = %d\n", tCrst, resid, count); */
	 } while ((fabs(resid) > 0.00001) && (count < 100));
         if (count < 100) GracePrintf("g0.s%1.1d POINT %lf, %lf\n", 0, 100.0-wtPotash, tCrst-273.15);

         printf("T");
	 tinc = -25.0;
	 count = 0;
	 do {
           actLiq(SECOND, tTrid, p, x, NULL, mu, NULL, NULL);	 
	   gibbs(tTrid, p, (char *) solids[nTrid].label, &(solids[nTrid].ref), NULL, NULL, &(solids[nTrid].cur));
	   resid = (mu[0]/2.0-solids[nTrid].cur.g)/1000.0;
	   /* liquid too stable, temperature too high */
	   if ((resid < 0.0) && (tinc > 0.0)) tinc *= -0.5; 
	   /* solid too stable,  temperature too low  */
	   else if ((resid > 0.0) && (tinc < 0.0)) tinc *= -0.5;
	   tTrid += tinc;
	   count++;
	   if (tTrid < 800.0) { count = 100; tTrid = 2000.0; } 
	   /* printf("tTrid = %f, resid = %g, count = %d\n", tTrid, resid, count); */
	 } while ((fabs(resid) > 0.00001) && (count < 100));
         if (count < 100) GracePrintf("g0.s%1.1d POINT %lf, %lf\n", 1, 100.0-wtPotash, tTrid-273.15);

         printf("Q");
	 tinc = -25.0;
	 count = 0;
	 do {
           actLiq(SECOND, tQtz, p, x, NULL, mu, NULL, NULL);	 
	   gibbs(tQtz, p, (char *) solids[nQtz].label, &(solids[nQtz].ref), NULL, NULL, &(solids[nQtz].cur));
	   resid = (mu[0]/2.0-solids[nQtz].cur.g)/1000.0;
	   /* liquid too stable, temperature too high */
	   if ((resid < 0.0) && (tinc > 0.0)) tinc *= -0.5; 
	   /* solid too stable,  temperature too low  */
	   else if ((resid > 0.0) && (tinc < 0.0)) tinc *= -0.5;
	   tQtz += tinc;
	   count++;
	   if (tQtz < 800.0) { count = 100; tQtz = 2000.0; } 
	   /* printf("tQtz = %f, resid = %g, count = %d\n", tQtz, resid, count); */
	 } while ((fabs(resid) > 0.00001) && (count < 100));
         if (count < 100) GracePrintf("g0.s%1.1d POINT %lf, %lf\n", 2, 100.0-wtPotash, tQtz-273.15);
	 
	 }
	 if ((pb != 7777.0) && (pb != 8888.0)) {

         printf("D");
	 tinc = -25.0;
	 count = 0;
	 do {
           actLiq(SECOND, tkDi, p, x, NULL, mu, NULL, NULL);	 
	   gibbs(tkDi, p, (char *) solids[nkDi].label, &(solids[nkDi].ref), NULL, NULL, &(solids[nkDi].cur));
	   resid = (mu[0]/2.0+3.0*mu[4]/2.0-solids[nkDi].cur.g)/1000.0;
	   /* liquid too stable, temperature too high */
	   if ((resid < 0.0) && (tinc > 0.0)) tinc *= -0.5; 
	   /* solid too stable,  temperature too low  */
	   else if ((resid > 0.0) && (tinc < 0.0)) tinc *= -0.5;
	   tkDi += tinc;
	   count++;
	   if (tkDi < 800.0) { count = 100; tkDi = 2000.0; } 
	   /* printf("tkDi = %f, resid = %g, count = %d\n", tkDi, resid, count); */
	 } while ((fabs(resid) > 0.00001) && (count < 100));
         if (count < 100) GracePrintf("g0.s%1.1d POINT %lf, %lf\n", 3, 100.0-wtPotash, tkDi-273.15);

         printf("M");
	 tinc = -25.0;
	 count = 0;
	 do {
           actLiq(SECOND, tkMet, p, x, NULL, mu, NULL, NULL);	 
	   gibbs(tkMet, p, (char *) solids[nkMet].label, &(solids[nkMet].ref), NULL, NULL, &(solids[nkMet].cur));
	   resid = (3.0*mu[4]/2.0-solids[nkMet].cur.g)/1000.0;
	   /* liquid too stable, temperature too high */
	   if ((resid < 0.0) && (tinc > 0.0)) tinc *= -0.5; 
	   /* solid too stable,  temperature too low  */
	   else if ((resid > 0.0) && (tinc < 0.0)) tinc *= -0.5;
	   tkMet += tinc;
	   count++;
	   if (tkMet < 800.0) { count = 100; tkMet = 2000.0; } 
	   /* printf("tkMet = %f, resid = %g, count = %d\n", tkMet, resid, count); */
	 } while ((fabs(resid) > 0.00001) && (count < 100));
         if (count < 100) GracePrintf("g0.s%1.1d POINT %lf, %lf\n", 4, 100.0-wtPotash, tkMet-273.15);

         printf("T");
	 tinc = -25.0;
	 count = 0;
	 do {
           actLiq(SECOND, tkTet, p, x, NULL, mu, NULL, NULL);	 
	   gibbs(tkTet, p, (char *) solids[nkTet].label, &(solids[nkTet].ref), NULL, NULL, &(solids[nkTet].cur));
	   resid = (3.0*mu[0]/2.0+3.0*mu[4]/2.0-solids[nkTet].cur.g)/1000.0;
	   /* liquid too stable, temperature too high */
	   if ((resid < 0.0) && (tinc > 0.0)) tinc *= -0.5; 
	   /* solid too stable,  temperature too low  */
	   else if ((resid > 0.0) && (tinc < 0.0)) tinc *= -0.5;
	   tkTet += tinc;
	   count++;
	   if (tkTet < 800.0) { count = 100; tkTet = 2000.0; } 
	   /* printf("tkTet = %f, resid = %g, count = %d\n", tkTet, resid, count); */
	 } while ((fabs(resid) > 0.00001) && (count < 100));
         if (count < 100) GracePrintf("g0.s%1.1d POINT %lf, %lf\n", 5, 100.0-wtPotash, tkTet-273.15);
	 
	 }
	 if (pb != 9999.0) {
	 
         printf("C");
	 tinc = -25.0;
	 count = 0;
	 do {
           actLiq(SECOND, tCor, p, x, NULL, mu, NULL, NULL);	 
	   gibbs(tCor, p, (char *) solids[nCor].label, &(solids[nCor].ref), NULL, NULL, &(solids[nCor].cur));
	   resid = (mu[1]-solids[nCor].cur.g)/1000.0;
	   /* liquid too stable, temperature too high */
	   if ((resid < 0.0) && (tinc > 0.0)) tinc *= -0.5; 
	   /* solid too stable,  temperature too low  */
	   else if ((resid > 0.0) && (tinc < 0.0)) tinc *= -0.5;
	   tCor += tinc;
	   count++;
	   if (tCor < 800.0) { count = 100; tCor = 2000.0; } 
	   /* printf("tCor = %f, resid = %g, count = %d\n", tCor, resid, count); */
	 } while ((fabs(resid) > 0.00001) && (count < 100));
         if (count < 100) GracePrintf("g0.s%1.1d POINT %lf, %lf\n", 6, 100.0-wtPotash, tCor-273.15);
	 
         printf("M");
	 tinc = -25.0;
	 count = 0;
	 do {
           actLiq(SECOND, tMul, p, x, NULL, mu, NULL, NULL);	 
	   gibbs(tMul, p, (char *) solids[nMul].label, &(solids[nMul].ref), NULL, NULL, &(solids[nMul].cur));
	   resid = (mu[0]+3.0*mu[1]-solids[nMul].cur.g)/1000.0;
	   /* liquid too stable, temperature too high */
	   if ((resid < 0.0) && (tinc > 0.0)) tinc *= -0.5; 
	   /* solid too stable,  temperature too low  */
	   else if ((resid > 0.0) && (tinc < 0.0)) tinc *= -0.5;
	   tMul += tinc;
	   count++;
	   if (tMul < 800.0) { count = 100; tMul = 2000.0; } 
	   /* printf("tMul = %f, resid = %g, count = %d\n", tMul, resid, count); */
	 } while ((fabs(resid) > 0.00001) && (count < 100));
         if (count < 100) GracePrintf("g0.s%1.1d POINT %lf, %lf\n", 7, 100.0-wtPotash, tMul-273.15);
	 
	 }
	 
	 if ((pb != 7777.0) && (pb != 8888.0) && (pb != 9999.0)) {

         printf("S");
	 tinc = -25.0;
	 count = 0;
	 do {
           actLiq(SECOND, tSan, p, x, NULL, mu, NULL, NULL);	 
	   gibbs(tSan, p, (char *) solids[nSan].label, &(solids[nSan].ref), NULL, NULL, &(solids[nSan].cur));
	   resid = (5.0*mu[0]/4.0+mu[1]/2.0+3.0*mu[4]/4.0-solids[nSan].cur.g)/1000.0;
	   /* liquid too stable, temperature too high */
	   if ((resid < 0.0) && (tinc > 0.0)) tinc *= -0.5; 
	   /* solid too stable,  temperature too low  */
	   else if ((resid > 0.0) && (tinc < 0.0)) tinc *= -0.5;
	   tSan += tinc;
	   count++;
	   if (tSan < 800.0) { count = 100; tSan = 2000.0; } 
	   /* printf("tSan = %f, resid = %g, count = %d\n", tSan, resid, count); */
	 } while ((fabs(resid) > 0.00001) && (count < 100));
         if (count < 100) GracePrintf("g0.s%1.1d POINT %lf, %lf\n", 8, 100.0-wtPotash, tSan-273.15);

         printf("L");
	 tinc = -25.0;
	 count = 0;
	 do {
           actLiq(SECOND, tLeu, p, x, NULL, mu, NULL, NULL);	 
	   gibbs(tLeu, p, (char *) solids[nLeu].label, &(solids[nLeu].ref), NULL, NULL, &(solids[nLeu].cur));
	   resid = (3.0*mu[0]/4.0+mu[1]/2.0+3.0*mu[4]/4.0-solids[nLeu].cur.g)/1000.0;
	   /* liquid too stable, temperature too high */
	   if ((resid < 0.0) && (tinc > 0.0)) tinc *= -0.5; 
	   /* solid too stable,  temperature too low  */
	   else if ((resid > 0.0) && (tinc < 0.0)) tinc *= -0.5;
	   tLeu += tinc;
	   count++;
	   if (tLeu < 800.0) { count = 100; tLeu = 2000.0; } 
	   /* printf("tLeu = %f, resid = %g, count = %d\n", tLeu, resid, count); */
	 } while ((fabs(resid) > 0.00001) && (count < 100));
         if (count < 100) GracePrintf("g0.s%1.1d POINT %lf, %lf\n", 9, 100.0-wtPotash, tLeu-273.15);

         printf("O");
	 tinc = -25.0;
	 count = 0;
	 do {
           actLiq(SECOND, tKalO, p, x, NULL, mu, NULL, NULL);	 
	   gibbs(tKalO, p, (char *) solids[nKalO].label, &(solids[nKalO].ref), NULL, NULL, &(solids[nKalO].cur));
	   resid = (mu[0]/4.0+mu[1]/2.0+3.0*mu[4]/4.0-solids[nKalO].cur.g)/1000.0;
	   /* liquid too stable, temperature too high */
	   if ((resid < 0.0) && (tinc > 0.0)) tinc *= -0.5; 
	   /* solid too stable,  temperature too low  */
	   else if ((resid > 0.0) && (tinc < 0.0)) tinc *= -0.5;
	   tKalO += tinc;
	   count++;
	   if (tKalO < 800.0) { count = 100; tKalO = 2000.0; } 
	   /* printf("tKalO = %f, resid = %g, count = %d\n", tKalO, resid, count); */
	 } while ((fabs(resid) > 0.00001) && (count < 100));
         if (count < 100) GracePrintf("g0.s%1.1d POINT %lf, %lf\n", 10, 100.0-wtPotash, tKalO-273.15);

         printf("H");
	 tinc = -25.0;
	 count = 0;
	 do {
           actLiq(SECOND, tKalH, p, x, NULL, mu, NULL, NULL);	 
	   gibbs(tKalH, p, (char *) solids[nKalH].label, &(solids[nKalH].ref), NULL, NULL, &(solids[nKalH].cur));
	   resid = (mu[0]/4.0+mu[1]/2.0+3.0*mu[4]/4.0-solids[nKalH].cur.g)/1000.0;
	   /* liquid too stable, temperature too high */
	   if ((resid < 0.0) && (tinc > 0.0)) tinc *= -0.5; 
	   /* solid too stable,  temperature too low  */
	   else if ((resid > 0.0) && (tinc < 0.0)) tinc *= -0.5;
	   tKalH += tinc;
	   count++;
	   if (tKalH < 800.0) { count = 100; tKalH = 2000.0; } 
	   /* printf("tKalH = %f, resid = %g, count = %d\n", tKalH, resid, count); */
	 } while ((fabs(resid) > 0.00001) && (count < 100));
         if (count < 100) GracePrintf("g0.s%1.1d POINT %lf, %lf\n", 11, 100.0-wtPotash, tKalH-273.15);

	 }
         
	 printf("\n");
       }
       if ((type == 31) || (type == 41) || (type == 51) || (type == 6)) setModeToMixingLiq(TRUE);
       if ((type ==  4) || (type == 41)) smixLiq(FIRST, t, p, x, &smix, NULL, NULL, NULL);
       if ((type ==  5) || (type == 51)) hmixLiq(FIRST, t, p, x, &hmix, NULL);
       if ((type ==  3) || (type == 31)) gmixLiq(FIRST, t, p, x, &gmix, NULL, NULL);
       if (type == 6) actLiq(FIRST, t, p, x, mu, NULL, NULL, NULL);
       if ((type == 31) || (type == 41) || (type == 51) || (type == 6)) setModeToMixingLiq(FALSE);

     
       if ((type == 4) || (type == 41)) {
         static int nLeu = -1;
	 static double *mu2;
         if (nLeu == -1) {
           for (k=0; k<npc; k++) if (!strcmp("leucite", solids[k].label) && (solids[k].type == PHASE)) nLeu  = k;
	   if (nLeu == -1) {
	     printf("Cannot find index of leucite.\n");
	     return 0;
	   }
	   mu2 = vector(0, nlc-1);
	 }
         GracePrintf("g0.s%1.1d POINT %lf, %lf\n", 5, wtPotash, smix);
	 gibbs(t, p, (char *) solids[nLeu].label, &(solids[nLeu].ref), NULL, NULL, &(solids[nLeu].cur));
	 actLiq(SECOND, t,       p, x, NULL, mu,  NULL, NULL);
	 actLiq(SECOND, t+0.001, p, x, NULL, mu2, NULL, NULL);
	 for (k=0; k<nlc; k++) mu[k] = -(mu2[k]-mu[k])/0.001;
         GracePrintf("g0.s%1.1d POINT %lf, %lf\n", 6, wtPotash, (3.0*mu[0]/4.0+mu[1]/2.0+3.0*mu[4]/4.0)/2.182472);
         GracePrintf("g0.s%1.1d POINT %lf, %lf\n", 7, wtPotash, (solids[nLeu].cur.s)/2.182472);

       }
       if ((type == 5) || (type == 51)) GracePrintf("g0.s%1.1d POINT %lf, %lf\n", 7, wtPotash, hmix/1000.0);
       if ((type == 3) || (type == 31)) GracePrintf("g0.s%1.1d POINT %lf, %lf\n", 1, wtPotash, gmix/1000.0);
       if (type == 1) for (j=10; j<(10+16); j++) GracePrintf("g0.s%2.2d POINT %lf, %lf\n", j, wtPotash, xSpecies[list[j-10]]);
       if (type == 6) {
         GracePrintf("g0.s%1.1d POINT %lf, %lf\n", 0, wtPotash, mu[0]);
         GracePrintf("g0.s%1.1d POINT %lf, %lf\n", 1, wtPotash, mu[1]);
         GracePrintf("g0.s%1.1d POINT %lf, %lf\n", 2, wtPotash, mu[4]);
       }

       GracePrintf("REDRAW\n");
     }
   }
    
   free_vector(    grams, 0, nlc-1);
   free_vector(    moles, 0, nlc-1);
   free_vector(        x, 0, nr-1);
   free_vector(       mu, 0, nlc-1);
   free_vector( xSpecies, 0, nls-1);
    
   return 0;
}

/* End file: TEST_SACNK.C */
