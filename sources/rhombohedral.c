const char *rhombohedral_ver(void) { return "$Id: rhombohedral.c,v 1.3 2006/10/20 00:59:22 ghiorso Exp $"; }
/*
MELTS Source Code: RCS $Log: rhombohedral.c,v $
MELTS Source Code: RCS Revision 1.3  2006/10/20 00:59:22  ghiorso
MELTS Source Code: RCS (1) Made initial modifications for thread safe code.
MELTS Source Code: RCS (2) Added support for XML I/O in batch mode
MELTS Source Code: RCS (3) Added support for Melts-batch listener for eventual integration into VIGMCS
MELTS Source Code: RCS
MELTS Source Code: RCS Revision 1.2  2006/08/17 16:47:19  ghiorso
MELTS Source Code: RCS Made modifications to protect strings.  These modifications allow removal
MELTS Source Code: RCS of the flag -fwritable-strings during gcc compilation.  This brings the
MELTS Source Code: RCS code up to gcc 4.x standards.
MELTS Source Code: RCS
MELTS Source Code: RCS Other minor rearrangements and cleanup.
MELTS Source Code: RCS
MELTS Source Code: RCS Revision 1.1.1.1  2006/08/15 16:57:36  ghiorso
MELTS Source Code: RCS xMELTS gcc 3.x sources
MELTS Source Code: RCS
MELTS Source Code: RCS Revision 1.1.1.1  2004/01/02 19:21:49  cvsaccount
MELTS Source Code: RCS CTserver University of Chicago
MELTS Source Code: RCS
MELTS Source Code: RCS Revision 1.1.1.1  2001/12/20 03:25:03  ghiorso
MELTS Source Code: RCS Sources for MELTS 5.x (xMELTS)
MELTS Source Code: RCS
MELTS Source Code: RCS Revision 5.1  2000/02/15 17:58:12  ghiorso
MELTS Source Code: RCS MELTS 5.0 - xMELTS (associated solutions, multiple liquids)
MELTS Source Code: RCS
 * Revision 3.8  1997/06/27  17:26:46  ghiorso
 * *** empty log message ***
 *
 * Revision 3.7  1997/06/21  22:49:28  ghiorso
 * June 1997 MELTS 3.0.x release
 * (prior to new entropy and regression model being introduced)
 *
 * Revision 3.6  1997/05/03  20:23:07  ghiorso
 * *** empty log message ***
 *
 * Revision 3.5  1997/03/27  17:03:11  ghiorso
 * *** empty log message ***
 *
 * Revision 3.4  1996/09/24  20:33:22  ghiorso
 * Version modified for OSF/1 4.0
 *
 * Revision 3.3  1995/12/09  19:26:38  ghiorso
 * Interface revisions for status box and graphics display
 *
 * Revision 3.2  1995/09/09  23:37:35  ghiorso
 * Modifications by Asimow to include new Derivatives of gmix and convert
 * options for liquid-absent fO2 buffering. Also new olivine components
 * implemented.
 *
 * Revision 3.2  1995/09/09  23:37:35  ghiorso
 * Modifications by Asimow to include new Derivatives of gmix and convert
 * options for liquid-absent fO2 buffering. Also new olivine components
 * implemented.
 *
 * Revision 3.1  1995/08/18  19:12:47  ghiorso
 * MELTS Version 3 - Initial Entry
 *
*/
 
/*
**++
**  FACILITY:  Silicate Melts Regression/Crystallization Package
**
**  MODULE DESCRIPTION:
**
**      Routines to compute rhombohedral oxide solution properties 
**      (file: RHOMBOHEDRAL.C) 
**
**  MODIFICATION HISTORY:
**
**      V1.0-1  Mark S. Ghiorso  June 12, 1991   Original Version - started
**                               June 13, 1991                      finished
**      V1.0-2  Mark S. Ghiorso  June 19, 1991
**              Added testRhm and revised conRhm
**      V1.0-3  Mark S. Ghiorso  July 5, 1991
**              Added exclusion criteria option to actRhm
**      V1.0-4  Mark S. Ghiorso  August 26, 1991
**              Extended conRhm to convert indep comp var -> mole frac comp
**      V1.0-5  Mark S. Ghiorso  September 16, 1991
**              Altered parameter list and structure of conRhm in order
**              to provide derivatives of the independent compositional
**              variables with respect to moles of endmember components.
**              Added several options and tests to conRhm.
**      V1.0-6  Mark S. Ghiorso  September 24, 1991
**              (1) Altered parameter list and structure of conRhm in order
**                  to provide derivatives of the endmember mole fractions with
**                  respect to independent compositional variables.
**              (2) Altered parameter list and structure of testRhm to
**                  provide for bounds testing on both the independent
**                  compositional variables and on endmember moles
**      V1.0-7  Mark S. Ghiorso  September 25, 1991
**              Added bounds on site mole fractions in order to avoid 
**              overflow conditions when an endmember component has zero
**              concentration
**      V1.0-8  Mark S. Ghiorso  September 26, 1991
**              Revised convergence criteria in ordering routines to
**              be 10.0*DBL_EPSILON
**      V1.0-9  Mark S. Ghiorso  October 4, 1991
**              Added printf statements to ordering routine to detect and
**              report nonconvergence conditions
**      V1.0-10 Mark S. Ghiorso  January 31, 1992
**              Revised initialization algorithm for automatic arrays to
**              conform to the ANSI standard (DEC C RISC)
**      V1.0-11 Mark S. Ghiorso  February 19, 1992
**              Corrected minor casting violations for ANSI C
**      V1.0-12 Mark S. Ghiorso  March 4, 1992
**              Added MAX_ITER count to ordering loop convergence algorithm
**      V1.0-13 Mark S. Ghiorso  March 23, 1992
**              (1) Corrected indexing error in d2x[][] for vmixRhm
**              (2) Replaced illegal calls to function pow() with references
**                  to macros SQUARE and CUBE
**      V1.0-14 Mark S. Ghiorso  April 1, 1992
**              Added test to catch negative mole fractions in order()
**              created as a consequence of round-off error
**      V1.0-15 Mark S. Ghiorso  July 1, 1992
**              Revised calculation of hematite content in conRhm to prevent
**              negative mole quantities
**      V1.1-1  Mark S. Ghiorso  July 13, 1992
**              Added (*display) function
**      V2.0-1  Mark S. Ghiorso  May 10, 1994
**              (1) Began modifications for new isenthalpic, isentropic,
**                  isochoric derivatives
**      V2.0-2  Mark S. Ghiorso  May 17, 1994
**              (1) Changed definition of the enthalpy of mixing
**	V3.0-1  Paul D. Asimow  July 31, 1995
**		(1) Added d3rdm3 to (*convert) and d3gdr3 to (*gmix)
**--
*/

#ifdef DEBUG
#undef DEBUG
#endif

#include "silmin.h"  /* Structure definitions foor SILMIN package */
#include "recipes.h" /* Numerical recipes routines                */

#define SQUARE(x) ((x)*(x))
#define CUBE(x)   ((x)*(x)*(x))
#define QUARTIC(x) ((x)*(x)*(x)*(x))

/*
 *=============================================================================
 * Rhombohedral solution parameters:
 * Ghiorso, M.S., Sack, R.O. (1991)
 * Fe-Ti Oxide Geothermometry: Thermodynamic formulation and the estimation
 * of intensive variables in silicic magmas
 * Contributions to Mineralogy and Petrology 108: 485-510
 */

static const double hil      =  17000.0;  /* joules */
static const double hgk      =  17000.0;  /* joules */
static const double hpy      =  17000.0;  /* joules */
static const double wilgkDIS =  25000.0;  /* joules */
static const double wilgkORD =   5000.0;  /* joules */
static const double dwilgk   =  12000.0;  /* joules */
static const double dwgkil   =   8000.0;  /* joules */
static const double wilpyDIS =   2200.0;  /* joules */
static const double wilpyORD =   2200.0;  /* joules */
static const double dwilpy   =  17000.0;  /* joules */
static const double dwpyil   =  17000.0;  /* joules */
static const double wilhmDIS =   5000.0;  /* joules */
static const double dwilhm   = -19000.0;  /* joules */
static const double wgkhmDIS =   5000.0;  /* joules */
static const double dwgkhm   = -19000.0;  /* joules */
static const double wgkpyDIS =  25000.0;  /* joules */
static const double wgkpyORD =   5000.0;  /* joules */
static const double dwgkpy   =   8000.0;  /* joules */
static const double dwpygk   =  12000.0;  /* joules */
static const double wpyhmDIS =   5000.0;  /* joules */
static const double dwpyhm   = -19000.0;  /* joules */

/*
 * Global (to this file): variables 
 */

#define R  8.3143
#define NR 3      /* Three independent composition variables  */
#define NS 3      /* Three ordering parameters                */
#define NA 4      /* Four endmember compositions              */

#define MAX_ITER 100  /* Max number of iterations allowed in ordering alg */

/*************************************/
/* Statics for Ordering Calculations */
/*************************************/

static MTHREAD_ONCE_T initThreadOBlock = MTHREAD_ONCE_INIT;

static MTHREAD_KEY_T tOldKey;
static MTHREAD_KEY_T pOldKey;
static MTHREAD_KEY_T rOldKey;
static MTHREAD_KEY_T sOldKey;
static MTHREAD_KEY_T d2gds2Key;
static MTHREAD_KEY_T ptToD2gds2Key;
static MTHREAD_KEY_T tOldPureKey;
static MTHREAD_KEY_T pOldPureKey;
static MTHREAD_KEY_T sOldPureKey;
static MTHREAD_KEY_T d2gds2PureKey;

static void freeNSarray(void *NSarray) {
  free_vector((double *) NSarray, 0, NS-1);
}

static void freeD2gds2(void *d2gds2) {
  free_matrix((double **) d2gds2, 0, NS-1, 0, NS-1);
}

static void freePtToD2gds2(void *ptToD2gds2) {
  free_submatrix((double **) ptToD2gds2, 1, NS, 1, NS);
}

static void threadOInit(void) {
  MTHREAD_KEY_CREATE(&tOldKey,       free);
  MTHREAD_KEY_CREATE(&pOldKey,       free);
  MTHREAD_KEY_CREATE(&rOldKey,       freeNSarray);
  MTHREAD_KEY_CREATE(&sOldKey,       freeNSarray);
  MTHREAD_KEY_CREATE(&d2gds2Key,     freeD2gds2);
  MTHREAD_KEY_CREATE(&ptToD2gds2Key, freePtToD2gds2);
  MTHREAD_KEY_CREATE(&tOldPureKey,   free);
  MTHREAD_KEY_CREATE(&pOldPureKey,   free);
  MTHREAD_KEY_CREATE(&sOldPureKey,   freeNSarray);
  MTHREAD_KEY_CREATE(&d2gds2PureKey, freeNSarray);
}

static double getTOld() {
  double *tOldPt;
  MTHREAD_ONCE(&initThreadOBlock, threadOInit);
  
  tOldPt = (double *) MTHREAD_GETSPECIFIC(tOldKey);   
  if (tOldPt == NULL) {
    tOldPt  = (double *) malloc(sizeof(double)); 
    *tOldPt = -9999.0;    
    MTHREAD_SETSPECIFIC(tOldKey, (void *) tOldPt);
  }
  return *tOldPt;
}

static void setTOld(double tOld) {
  double *tOldPt;
  MTHREAD_ONCE(&initThreadOBlock, threadOInit);
  
  tOldPt = (double *) MTHREAD_GETSPECIFIC(tOldKey);   
  if (tOldPt == NULL) {
    tOldPt  = (double *) malloc(sizeof(double)); 
    *tOldPt = -9999.0;    
    MTHREAD_SETSPECIFIC(tOldKey, (void *) tOldPt);
  }
  *tOldPt = tOld;
}

static double getPOld() {
  double *pOldPt;
  MTHREAD_ONCE(&initThreadOBlock, threadOInit);
  
  pOldPt = (double *) MTHREAD_GETSPECIFIC(pOldKey);   
  if (pOldPt == NULL) {
    pOldPt  = (double *) malloc(sizeof(double)); 
    *pOldPt = -9999.0;    
    MTHREAD_SETSPECIFIC(pOldKey, (void *) pOldPt);
  }
  return *pOldPt; 
}

static void setPOld(double pOld) {
  double *pOldPt;
  MTHREAD_ONCE(&initThreadOBlock, threadOInit);
  
  pOldPt = (double *) MTHREAD_GETSPECIFIC(pOldKey);   
  if (pOldPt == NULL) {
    pOldPt  = (double *) malloc(sizeof(double)); 
    *pOldPt = -9999.0;    
    MTHREAD_SETSPECIFIC(pOldKey, (void *) pOldPt);
  }
  *pOldPt = pOld; 
}

static double *getROld() {
  double *rOldPt;
  MTHREAD_ONCE(&initThreadOBlock, threadOInit);
  
  rOldPt = (double *) MTHREAD_GETSPECIFIC(rOldKey);   
  if (rOldPt == NULL) {
    int i;
    rOldPt = vector(0, NR-1);
    for (i=0; i<NR; i++) rOldPt[i] = -9999.0;    
    MTHREAD_SETSPECIFIC(rOldKey, (void *) rOldPt);
  }
  return rOldPt; 
}

static double *getSOld() {
  double *sOldPt;
  MTHREAD_ONCE(&initThreadOBlock, threadOInit);
  
  sOldPt = (double *) MTHREAD_GETSPECIFIC(sOldKey);   
  if (sOldPt == NULL) {
    int i;
    sOldPt = vector(0, NS-1);
    for (i=0; i<NS; i++) sOldPt[i] = 2.0;    
    MTHREAD_SETSPECIFIC(sOldKey, (void *) sOldPt);
  }
  return sOldPt;
}

static double **getD2gds2() {
  double **d2gds2Pt;
  MTHREAD_ONCE(&initThreadOBlock, threadOInit);
  
  d2gds2Pt = (double **) MTHREAD_GETSPECIFIC(d2gds2Key);   
  if (d2gds2Pt == NULL) {
    int i, j;
    d2gds2Pt  = matrix(0, NS-1, 0, NS-1);
    for (i=0; i<NS; i++) for (j=0; j<NS; j++) d2gds2Pt[i][j] = 0.0;
    MTHREAD_SETSPECIFIC(d2gds2Key, (void *) d2gds2Pt);
  }
  return d2gds2Pt; 
}

static double **getPtToD2gds2() {
  double **ptToD2gds2Pt;
  MTHREAD_ONCE(&initThreadOBlock, threadOInit);
  
  ptToD2gds2Pt = (double **) MTHREAD_GETSPECIFIC(ptToD2gds2Key);   
  if (ptToD2gds2Pt == NULL) {
    ptToD2gds2Pt = submatrix(getD2gds2(), 0, NS-1, 0, NS-1, 1, 1);
    MTHREAD_SETSPECIFIC(ptToD2gds2Key, (void *) ptToD2gds2Pt);
  }
  return ptToD2gds2Pt;
}

static double getTOldPure() {
  double *tOldPurePt;
  MTHREAD_ONCE(&initThreadOBlock, threadOInit);
  
  tOldPurePt = (double *) MTHREAD_GETSPECIFIC(tOldPureKey);   
  if (tOldPurePt == NULL) {
    tOldPurePt  = (double *) malloc(sizeof(double)); 
    *tOldPurePt = -9999.0;    
    MTHREAD_SETSPECIFIC(tOldPureKey, (void *) tOldPurePt);
  }
  return *tOldPurePt;
}

static void setTOldPure(double tOldPure) {
  double *tOldPurePt;
  MTHREAD_ONCE(&initThreadOBlock, threadOInit);
  
  tOldPurePt = (double *) MTHREAD_GETSPECIFIC(tOldPureKey);   
  if (tOldPurePt == NULL) {
    tOldPurePt  = (double *) malloc(sizeof(double)); 
    *tOldPurePt = -9999.0;    
    MTHREAD_SETSPECIFIC(tOldPureKey, (void *) tOldPurePt);
  }
  *tOldPurePt = tOldPure;
}

static double getPOldPure() {
  double *pOldPurePt;
  MTHREAD_ONCE(&initThreadOBlock, threadOInit);
  
  pOldPurePt = (double *) MTHREAD_GETSPECIFIC(pOldPureKey);   
  if (pOldPurePt == NULL) {
    pOldPurePt  = (double *) malloc(sizeof(double)); 
    *pOldPurePt = -9999.0;    
    MTHREAD_SETSPECIFIC(pOldPureKey, (void *) pOldPurePt);
  }
  return *pOldPurePt;
}

static void setPOldPure(double pOldPure) {
  double *pOldPurePt;
  MTHREAD_ONCE(&initThreadOBlock, threadOInit);
  
  pOldPurePt = (double *) MTHREAD_GETSPECIFIC(pOldPureKey);   
  if (pOldPurePt == NULL) {
    pOldPurePt  = (double *) malloc(sizeof(double)); 
    *pOldPurePt = -9999.0;    
    MTHREAD_SETSPECIFIC(pOldPureKey, (void *) pOldPurePt);
  }
  *pOldPurePt = pOldPure;
}

static double *getSOldPure() {
  double *sOldPurePt;
  MTHREAD_ONCE(&initThreadOBlock, threadOInit);
  
  sOldPurePt = (double *) MTHREAD_GETSPECIFIC(sOldPureKey);   
  if (sOldPurePt == NULL) {
    int i;
    sOldPurePt = vector(0, NS-1);
    for (i=0; i<NS; i++) sOldPurePt[i] = 2.0;    
    MTHREAD_SETSPECIFIC(sOldPureKey, (void *) sOldPurePt);
  }
  return sOldPurePt;
}

static double *getD2gds2Pure() {
  double *d2gds2PurePt;
  MTHREAD_ONCE(&initThreadOBlock, threadOInit);
  
  d2gds2PurePt = (double *) MTHREAD_GETSPECIFIC(d2gds2PureKey);   
  if (d2gds2PurePt == NULL) {
    int i;
    d2gds2PurePt  = vector(0, NS-1);
    for (i=0; i<NS; i++) d2gds2PurePt[i] = 0.0;
    MTHREAD_SETSPECIFIC(d2gds2PureKey, (void *) d2gds2PurePt);
  }
  return d2gds2PurePt;
}

/***********************************/
/* Statics for Site Mole Fractions */
/***********************************/

#define DECLARE_SITE_FRACTIONS \
  double xmg2A, xfe2A, xmn2A, xti4A, xfe3A, xmg2B, xfe2B, xmn2B, xti4B, xfe3B;

#define XMG2A 0
#define XFE2A 1
#define XMN2A 2
#define XTI4A 3
#define XFE3A 4
#define XMG2B 5
#define XFE2B 6
#define XMN2B 7
#define XTI4B 8
#define XFE3B 9

#define NX   10

static MTHREAD_ONCE_T initThreadXBlock = MTHREAD_ONCE_INIT;

static MTHREAD_KEY_T xKey[NX];

static void threadXInit(void) {
  int i;
  for (i=0; i<NX; i++) MTHREAD_KEY_CREATE(&xKey[i], free);
}

static double getX(int n) {
  double *xPt;  
  MTHREAD_ONCE(&initThreadXBlock, threadXInit);
  
  xPt = (double *) MTHREAD_GETSPECIFIC(xKey[n]);   
  if (xPt == NULL) { 
    xPt = (double *) malloc(sizeof(double)); *xPt = 0.0;
    MTHREAD_SETSPECIFIC(xKey[n], (void *) xPt);
  }
  return *xPt;
}

static void setX(int n, double x) {
  double *xPt;  
  MTHREAD_ONCE(&initThreadXBlock, threadXInit);
  
  xPt = (double *) MTHREAD_GETSPECIFIC(xKey[n]);   
  if (xPt == NULL) { 
    xPt = (double *) malloc(sizeof(double));
    MTHREAD_SETSPECIFIC(xKey[n], (void *) xPt);
  }
  *xPt = x;
}

#define GET_SITE_FRACTIONS \
  xmg2A = getX(XMG2A); \
  xfe2A = getX(XFE2A); \
  xmn2A = getX(XMN2A); \
  xti4A = getX(XTI4A); \
  xfe3A = getX(XFE3A); \
  xmg2B = getX(XMG2B); \
  xfe2B = getX(XFE2B); \
  xmn2B = getX(XMN2B); \
  xti4B = getX(XTI4B); \
  xfe3B = getX(XFE3B);
  
#define SET_SITE_FRACTIONS \
  setX(XMG2A, xmg2A); \
  setX(XFE2A, xfe2A); \
  setX(XMN2A, xmn2A); \
  setX(XTI4A, xti4A); \
  setX(XFE3A, xfe3A); \
  setX(XMG2B, xmg2B); \
  setX(XFE2B, xfe2B); \
  setX(XMN2B, xmn2B); \
  setX(XTI4B, xti4B); \
  setX(XFE3B, xfe3B);

/*
 *=============================================================================
 * Local function to compute ordering state and associated derivatives of
 * pure component endmembers
 */

#define IL_S  - R*(  (1.0+s[0])*log(1.0+s[0]) + (1.0-s[0])*log(1.0-s[0]) \
                   - 2.0*log(2.0) )
#define IL_H  hil*(1.0-s[0]*s[0])
#define IL_G  IL_H - t*(IL_S)

#define DIL_GDS0        R*t*( log(1.0+s[0]) - log(1.0-s[0]) ) - 2.0*hil*s[0]
#define DIL_GDT         -(IL_S)
#define DIL_GDP         0.0

#define D2IL_GDS0S0     R*t*( 1.0/(1.0+s[0]) + 1.0/(1.0-s[0]) ) - 2.0*hil
#define D2IL_GDS0DT     R*( log(1.0+s[0]) - log(1.0-s[0]) )
#define D2IL_GDS0DP     0.0
#define D2IL_GDT2       0.0
#define D2IL_GDTP       0.0
#define D2IL_GDP2       0.0

#define D3IL_GDS0S0S0   - R*t*( 1.0/SQUARE(1.0+s[0]) - 1.0/SQUARE(1.0-s[0]) ) 
#define D3IL_GDS0S0DT   R*( 1.0/(1.0+s[0]) + 1.0/(1.0-s[0]) )
#define D3IL_GDS0S0DP   0.0
#define D3IL_GDS0DT2    0.0
#define D3IL_GDS0DTDP   0.0
#define D3IL_GDS0DP2    0.0
#define D3IL_GDT3       0.0
#define D3IL_GDT2DP     0.0
#define D3IL_GDTDP2     0.0
#define D3IL_GDP3       0.0

#define GK_S  - R*(  (1.0+s[1])*log(1.0+s[1]) + (1.0-s[1])*log(1.0-s[1]) \
                   - 2.0*log(2.0) )
#define GK_H  hgk*(1.0-s[1]*s[1])
#define GK_G  GK_H - t*(GK_S)

#define DGK_GDS1       R*t*( log(1.0+s[1]) - log(1.0-s[1]) ) - 2.0*hgk*s[1]
#define DGK_GDT        -(GK_S)
#define DGK_GDP        0.0

#define D2GK_GDS1S1    R*t*( 1.0/(1.0+s[1]) + 1.0/(1.0-s[1]) ) - 2.0*hgk
#define D2GK_GDS1DT    R*( log(1.0+s[1]) - log(1.0-s[1]) )
#define D2GK_GDS1DP    0.0
#define D2GK_GDT2      0.0
#define D2GK_GDTP      0.0
#define D2GK_GDP2      0.0

#define D3GK_GDS1S1S1  - R*t*( 1.0/SQUARE(1.0+s[1]) - 1.0/SQUARE(1.0-s[1]) ) 
#define D3GK_GDS1S1DT  R*( 1.0/(1.0+s[1]) + 1.0/(1.0-s[1]) )
#define D3GK_GDS1S1DP  0.0
#define D3GK_GDS1DT2   0.0
#define D3GK_GDS1DTDP  0.0
#define D3GK_GDS1DP2   0.0
#define D3GK_GDT3      0.0
#define D3GK_GDT2DP    0.0
#define D3GK_GDTDP2    0.0
#define D3GK_GDP3      0.0

#define PY_S  - R*(  (1.0+s[2])*log(1.0+s[2]) + (1.0-s[2])*log(1.0-s[2]) \
                   - 2.0*log(2.0) )
#define PY_H  hpy*(1.0-s[2]*s[2])
#define PY_G  PY_H - t*(PY_S)

#define DPY_GDS2       R*t*( log(1.0+s[2]) - log(1.0-s[2]) ) - 2.0*hpy*s[2]
#define DPY_GDT        -(PY_S)
#define DPY_GDP        0.0

#define D2PY_GDS2S2    R*t*( 1.0/(1.0+s[2]) + 1.0/(1.0-s[2]) ) - 2.0*hpy
#define D2PY_GDS2DT    R*( log(1.0+s[2]) - log(1.0-s[2]) )
#define D2PY_GDS2DP    0.0
#define D2PY_GDT2      0.0
#define D2PY_GDTP      0.0
#define D2PY_GDP2      0.0

#define D3PY_GDS2S2S2  - R*t*( 1.0/SQUARE(1.0+s[2]) - 1.0/SQUARE(1.0-s[2]) )
#define D3PY_GDS2S2DT  R*( 1.0/(1.0+s[2]) + 1.0/(1.0-s[2]) )
#define D3PY_GDS2S2DP  0.0
#define D3PY_GDS2DT2   0.0
#define D3PY_GDS2DTDP  0.0
#define D3PY_GDS2DP2   0.0
#define D3PY_GDT3      0.0
#define D3PY_GDT2DP    0.0
#define D3PY_GDTDP2    0.0
#define D3PY_GDP3      0.0

#define HM_S  0.0
#define HM_H  0.0
#define HM_G  HM_H - t*(HM_S)      

#define DHM_GDT      -(HM_S)
#define DHM_GDP      0.0
#define D2HM_GDT2    0.0
#define D2HM_GDTP    0.0
#define D2HM_GDP2    0.0
#define D3HM_GDT3    0.0
#define D3HM_GDT2DP  0.0
#define D3HM_GDTDP2  0.0
#define D3HM_GDP3    0.0

#define fillD2GDSDT \
 d2gdsdt[0] = D2IL_GDS0DT; d2gdsdt[1] = D2GK_GDS1DT; d2gdsdt[2] = D2PY_GDS2DT;

#define fillD2GDSDP \
 d2gdsdp[0] = D2IL_GDS0DP; d2gdsdp[1] = D2GK_GDS1DP; d2gdsdp[2] = D2PY_GDS2DP;

#define fillD3GDS3 \
 d3gds3[0] = D3IL_GDS0S0S0; d3gds3[1] = D3GK_GDS1S1S1; \
 d3gds3[2] = D3PY_GDS2S2S2;

#define fillD3GDS2DT \
 d3gds2dt[0] = D3IL_GDS0S0DT; d3gds2dt[1] = D3GK_GDS1S1DT; \
 d3gds2dt[2] = D3PY_GDS2S2DT;

#define fillD3GDS2DP \
 d3gds2dp[0] = D3IL_GDS0S0DP; d3gds2dp[1] = D3GK_GDS1S1DP; \
 d3gds2dp[2] = D3PY_GDS2S2DP;

#define fillD3GDSDT2 \
 d3gdsdt2[0] = D3IL_GDS0DT2; d3gdsdt2[1] = D3GK_GDS1DT2; \
 d3gdsdt2[2] = D3PY_GDS2DT2;

#define fillD3GDSDTDP \
 d3gdsdtdp[0] = D3IL_GDS0DTDP; d3gdsdtdp[1] = D3GK_GDS1DTDP; \
 d3gdsdtdp[2] = D3PY_GDS2DTDP;

#define fillD3GDSDP2 \
 d3gdsdp2[0] = D3IL_GDS0DP2; d3gdsdp2[1] = D3GK_GDS1DP2; \
 d3gdsdp2[2] = D3PY_GDS2DP2;

static void 
pureOrder(int mask, double t, double p, 
      double s[NS],   /* s[NS]       BINARY MASK: 000001 */
      double dt[NS],  /* ds[NS]/dt   BINARY MASK: 000010 */
      double dp[NS],  /* ds[NS]/dp   BINARY MASK: 000100 */
      double dt2[NS], /* d2s[NS]/dt2 BINARY MASK: 001000 */
      double dtp[NS], /* d2s[NS]/dtp BINARY MASK: 010000 */
      double dp2[NS]  /* d2s[NS]/dp2 BINARY MASK: 100000 */
      )
{
  double tOld    = getTOldPure();
  double pOld    = getPOldPure();
  double *sOld   = getSOldPure();
  double *d2gds2 = getD2gds2Pure();
  int i, iter=0;

  if ( (t != tOld) || (p != pOld) ) {
    double dgds[NS], sNew[NS];
    for (i=0; i<NS; i++) { sOld[i] = 2.0; sNew[i] = 0.9; }
    while (((ABS(sNew[0]-sOld[0]) > 10.0*DBL_EPSILON) || 
            (ABS(sNew[1]-sOld[1]) > 10.0*DBL_EPSILON) ||
            (ABS(sNew[2]-sOld[2]) > 10.0*DBL_EPSILON))  && (iter < MAX_ITER)) {
      double s[NS];

      for (i=0; i<NS; i++) s[i] = sNew[i];

      dgds[0] = DIL_GDS0;
      dgds[1] = DGK_GDS1;
      dgds[2] = DPY_GDS2;

      d2gds2[0] = D2IL_GDS0S0;
      d2gds2[1] = D2GK_GDS1S1;
      d2gds2[2] = D2PY_GDS2S2;

      for (i=0; i<NS; i++) sOld[i] = s[i];

      for (i=0; i<NS; i++) {
         s[i] += - dgds[i]/d2gds2[i];
         s[i] = MIN(s[i], 1.0 - DBL_EPSILON);
         s[i] = MAX(s[i], DBL_EPSILON);
      }

      for (i=0; i<NS; i++) sNew[i] = s[i];
      iter++;
    }
    tOld = t;
    pOld = p;
    
    setTOldPure(tOld);
    setPOldPure(pOld);
    /* sOld and d2gds2 are automatically saved */
  }

  if (mask & FIRST  ) {   /* return s        */
    for (i=0; i<NS; i++) s[i] = sOld[i];
  }   

  if (mask & SECOND ) {   /* compute ds/dt:  */
    double *s = sOld;
    double d2gdsdt[NS];

    fillD2GDSDT

    for (i=0; i<NS; i++) dt[i] = - d2gdsdt[i]/d2gds2[i];
  }

  if (mask & THIRD  ) {   /* compute ds/dp:  */
    double d2gdsdp[NS];

    fillD2GDSDP

    for (i=0; i<NS; i++) dp[i] = - d2gdsdp[i]/d2gds2[i];
  }

  if (mask & FOURTH ) {   /* compute d2s/dt2 */
    double *s = sOld;
    double d2gdsdt[NS], d3gds3[NS], d3gds2dt[NS], d3gdsdt2[NS], dsdt[NS];

    fillD2GDSDT
    fillD3GDS3
    fillD3GDS2DT
    fillD3GDSDT2

    for (i=0; i<NS; i++) dsdt[i] = - d2gdsdt[i]/d2gds2[i];
    for (i=0; i<NS; i++) dt2[i] = - (d3gdsdt2[i] + 2.0*d3gds2dt[i]*dsdt[i] 
      + d3gds3[i]*dsdt[i]*dsdt[i])/d2gds2[i];
  }

  if (mask & FIFTH  ) {   /* compute d2s/dtp */
    double *s = sOld;
    double d2gdsdt[NS], d2gdsdp[NS], d3gds3[NS], d3gds2dt[NS], d3gds2dp[NS],
      d3gdsdtdp[NS], dsdt[NS], dsdp[NS];

    fillD2GDSDT
    fillD2GDSDP
    fillD3GDS3
    fillD3GDS2DT
    fillD3GDS2DP
    fillD3GDSDTDP

    for (i=0; i<NS; i++) dsdt[i] = - d2gdsdt[i]/d2gds2[i];
    for (i=0; i<NS; i++) dsdp[i] = - d2gdsdp[i]/d2gds2[i];

    for (i=0; i<NS; i++)
      dtp[i] = - (d3gdsdtdp[i] + d3gds2dt[i]*dsdp[i] + d3gds2dp[i]*dsdt[i] 
             + d3gds3[i]*dsdt[i]*dsdp[i])/d2gds2[i];

  }

  if (mask & SIXTH  ) {   /* compute d2s/dp2 */
    double *s = sOld;
    double d2gdsdp[NS], d3gds3[NS], d3gds2dp[NS], d3gdsdp2[NS], dsdp[NS];

    fillD2GDSDP
    fillD3GDS3
    fillD3GDS2DP
    fillD3GDSDP2

    for (i=0; i<NS; i++) dsdp[i] = - d2gdsdp[i]/d2gds2[i];

    for (i=0; i<NS; i++) dp2[i] = - (d3gdsdp2[i] + 2.0*d3gds2dp[i]*dsdp[i]
      + d3gds3[i]*dsdp[i]*dsdp[i])/d2gds2[i];
  }
}

#undef fillD2GDSDT
#undef fillD2GDSDP
#undef fillD3GDS3
#undef fillD3GDS2DT
#undef fillD3GDS2DP
#undef fillD3GDSDT2
#undef fillD3GDSDTDP
#undef fillD3GDSDP2

#define fillD2GDS2 \
 d2gds2[0][0] = 0.0;         d2gds2[0][1] = D2GK_GDS1S1; \
 d2gds2[0][2] = 0.0; \
 d2gds2[1][0] = 0.0;         d2gds2[1][1] = 0.0; \
 d2gds2[1][2] = 0.0; \
 d2gds2[2][0] = D2IL_GDS0S0; d2gds2[2][1] = 0.0; \
 d2gds2[2][2] = 0.0; \
 d2gds2[3][0] = 0.0;         d2gds2[3][1] = 0.0; \
 d2gds2[3][2] = D2PY_GDS2S2;

#define fillD2GDSDT \
 d2gdsdt[0][0] = 0.0;         d2gdsdt[0][1] = D2GK_GDS1DT; \
 d2gdsdt[0][2] =  0.0; \
 d2gdsdt[1][0] = 0.0;         d2gdsdt[1][1] = 0.0; \
 d2gdsdt[1][2] =  0.0; \
 d2gdsdt[2][0] = D2IL_GDS0DT; d2gdsdt[2][1] = 0.0; \
 d2gdsdt[2][2] =  0.0; \
 d2gdsdt[3][0] = 0.0;         d2gdsdt[3][1] = 0.0; \
 d2gdsdt[3][2] = D2PY_GDS2DT;

#define fillD2GDSDP \
 d2gdsdp[0][0] = 0.0;         d2gdsdp[0][1] = D2GK_GDS1DP; \
 d2gdsdp[0][2] = 0.0; \
 d2gdsdp[1][0] = 0.0;         d2gdsdp[1][1] = 0.0; \
 d2gdsdp[1][2] = 0.0; \
 d2gdsdp[2][0] = D2IL_GDS0DP; d2gdsdp[2][1] = 0.0; \
 d2gdsdp[2][2] = 0.0; \
 d2gdsdp[3][0] = 0.0;         d2gdsdp[3][1] = 0.0; \
 d2gdsdp[3][2] = D2PY_GDS2DP;

#define fillD2GDT2 \
 d2gdt2[0] = D2GK_GDT2; d2gdt2[1] = D2HM_GDT2; d2gdt2[2] = D2IL_GDT2; \
 d2gdt2[3] = D2PY_GDT2;

#define fillD2GDTDP \
 d2gdtdp[0] = D2GK_GDTP; d2gdtdp[1] = D2HM_GDTP; d2gdtdp[2] = D2IL_GDTP; \
 d2gdtdp[3] = D2PY_GDTP;

#define fillD2GDP2 \
 d2gdp2[0] = D2GK_GDP2; d2gdp2[1] = D2HM_GDP2; d2gdp2[2] = D2IL_GDP2; \
 d2gdp2[3] = D2PY_GDP2;

#define fillD3GDS3 \
 d3gds3[0][0] = 0.0;           d3gds3[0][1] = D3GK_GDS1S1S1; \
 d3gds3[0][2] = 0.0; \
 d3gds3[1][0] = 0.0;           d3gds3[1][1] = 0.0; \
 d3gds3[1][2] = 0.0; \
 d3gds3[2][0] = D3IL_GDS0S0S0; d3gds3[2][1] = 0.0; \
 d3gds3[2][2] = 0.0; \
 d3gds3[3][0] = 0.0;           d3gds3[3][1] = 0.0; \
 d3gds3[3][2] = D3PY_GDS2S2S2;

#define fillD3GDS2DT \
 d3gds2dt[0][0] = 0.0;           d3gds2dt[0][1] = D3GK_GDS1S1DT; \
 d3gds2dt[0][2] = 0.0; \
 d3gds2dt[1][0] = 0.0;           d3gds2dt[1][1] = 0.0; \
 d3gds2dt[1][2] = 0.0; \
 d3gds2dt[2][0] = D3IL_GDS0S0DT; d3gds2dt[2][1] = 0.0; \
 d3gds2dt[2][2] = 0.0; \
 d3gds2dt[3][0] = 0.0;           d3gds2dt[3][1] = 0.0; \
 d3gds2dt[3][2] = D3PY_GDS2S2DT;

#define fillD3GDS2DP \
 d3gds2dp[0][0] = 0.0;           d3gds2dp[0][1] = D3GK_GDS1S1DP; \
 d3gds2dp[0][2] = 0.0; \
 d3gds2dp[1][0] = 0.0;           d3gds2dp[1][1] = 0.0; \
 d3gds2dp[1][2] = 0.0; \
 d3gds2dp[2][0] = D3IL_GDS0S0DP; d3gds2dp[2][1] = 0.0; \
 d3gds2dp[2][2] = 0.0; \
 d3gds2dp[3][0] = 0.0;           d3gds2dp[3][1] = 0.0; \
 d3gds2dp[3][2] = D3PY_GDS2S2DP;

#define fillD3GDSDT2 \
 d3gdsdt2[0][0] = 0.0;          d3gdsdt2[0][1] = D3GK_GDS1DT2; \
 d3gdsdt2[0][2] = 0.0; \
 d3gdsdt2[1][0] = 0.0;          d3gdsdt2[1][1] = 0.0; \
 d3gdsdt2[1][2] = 0.0; \
 d3gdsdt2[2][0] = D3IL_GDS0DT2; d3gdsdt2[2][1] = 0.0; \
 d3gdsdt2[2][2] = 0.0; \
 d3gdsdt2[3][0] = 0.0;          d3gdsdt2[3][1] = 0.0; \
 d3gdsdt2[3][2] =  D3PY_GDS2DT2;

#define fillD3GDSDTDP \
 d3gdsdtdp[0][0] = 0.0;           d3gdsdtdp[0][1] = D3GK_GDS1DTDP; \
 d3gdsdtdp[0][2] = 0.0; \
 d3gdsdtdp[1][0] = 0.0;           d3gdsdtdp[1][1] = 0.0; \
 d3gdsdtdp[1][2] = 0.0; \
 d3gdsdtdp[2][0] = D3IL_GDS0DTDP; d3gdsdtdp[2][1] = 0.0; \
 d3gdsdtdp[2][2] = 0.0; \
 d3gdsdtdp[3][0] = 0.0;           d3gdsdtdp[3][1] = 0.0; \
 d3gdsdtdp[3][2] = D3PY_GDS2DTDP;

#define fillD3GDSDP2 \
 d3gdsdp2[0][0] = 0.0;          d3gdsdp2[0][1] = D3GK_GDS1DP2; \
 d3gdsdp2[0][2] = 0.0; \
 d3gdsdp2[1][0] = 0.0;          d3gdsdp2[1][1] = 0.0; \
 d3gdsdp2[1][2] = 0.0; \
 d3gdsdp2[2][0] = D3IL_GDS0DP2; d3gdsdp2[2][1] = 0.0; \
 d3gdsdp2[2][2] = 0.0; \
 d3gdsdp2[3][0] = 0.0;          d3gdsdp2[3][1] = 0.0; \
 d3gdsdp2[3][2] = D3PY_GDS2DP2;

#define fillD3GDT3 \
 d3gdt3[0] = D3GK_GDT3; d3gdt3[1] = D3HM_GDT3; d3gdt3[2] = D3IL_GDT3; \
 d3gdt3[3] = D3PY_GDT3;

#define fillD3GDT2DP \
 d3gdt2dp[0] = D3GK_GDT2DP; d3gdt2dp[1] = D3HM_GDT2DP; \
 d3gdt2dp[2] = D3IL_GDT2DP; d3gdt2dp[3] = D3PY_GDT2DP;

#define fillD3GDTDP2 \
 d3gdtdp2[0] = D3GK_GDTDP2; d3gdtdp2[1] = D3HM_GDTDP2; \
 d3gdtdp2[2] = D3IL_GDTDP2; d3gdtdp2[3] = D3PY_GDTDP2;

#define fillD3GDP3 \
 d3gdp3[0] = D3GK_GDP3; d3gdp3[1] = D3HM_GDP3; d3gdp3[2] = D3IL_GDP3; \
 d3gdp3[3] = D3PY_GDP3;

static void 
pureRhm(int mask, double t, double p, 
  double a[NA],        /* activities              BINARY MASK: 0000000000001 */
  double mu[NA],       /* chemical potentials     BINARY MASK: 0000000000010 */
  double gmix[NA],     /* Gibbs energy            BINARY MASK: 0000000000100 */
  double hmix[NA],     /* Enthalpy of mixing      BINARY MASK: 0000000001000 */
  double smix[NA],     /* Entropy of mixing       BINARY MASK: 0000000010000 */
  double cpmix[NA],    /* Heat capacity of mixing BINARY MASK: 0000000100000 */
  double cpmixdt[NA],  /* d(cp)/d(t)              BINARY MASK: 0000001000000 */
  double vmix[NA],     /* Volume of mixing        BINARY MASK: 0000010000000 */
  double vmixdt[NA],   /* d(v)/d(t)               BINARY MASK: 0000100000000 */
  double vmixdp[NA],   /* d(v)/d(p)               BINARY MASK: 0001000000000 */
  double vmixdt2[NA],  /* d2(v)/d(t)2             BINARY MASK: 0010000000000 */
  double vmixdtdp[NA], /* d2(v)/d(t)d(p)          BINARY MASK: 0100000000000 */
  double vmixdp2[NA]   /* d2(v)/d(p)2             BINARY MASK: 1000000000000 */
  )
{
  double s[NS];
  int i, j;
  
  pureOrder(FIRST, t, p, 
            s,               (double *) NULL, (double *) NULL, (double *) NULL, 
            (double *) NULL, (double *) NULL);

  if (mask & FIRST) {
    a[0] = GK_G;
    a[0] = exp(a[0]/(R*t));
    a[1] = HM_G;
    a[1] = exp(a[1]/(R*t));
    a[2] = IL_G;
    a[2] = exp(a[2]/(R*t));
    a[3] = PY_G;
    a[3] = exp(a[3]/(R*t));
  }

  if (mask & SECOND) {
    mu[0] = GK_G;
    mu[1] = HM_G;
    mu[2] = IL_G;
    mu[3] = PY_G;
  }

  if (mask & THIRD) {
    gmix[0] = GK_G;
    gmix[1] = HM_G;
    gmix[2] = IL_G;
    gmix[3] = PY_G;
  }

  if (mask & FOURTH) {
    hmix[0] = (GK_G) + t*(GK_S);
    hmix[1] = (HM_G) + t*(HM_S);
    hmix[2] = (IL_G) + t*(IL_S);
    hmix[3] = (PY_G) + t*(PY_S);
  }

  if (mask & FIFTH) {
    smix[0] = GK_S;
    smix[1] = HM_S;
    smix[2] = IL_S;
    smix[3] = PY_S;
  }
  
  if (mask & SIXTH) {
    double d2gdsdt[NA][NS], d2gds2[NA][NS], dsdt[NS];        

    fillD2GDS2
    fillD2GDSDT

    pureOrder(SECOND, t, p, 
            (double *) NULL, dsdt,            (double *) NULL, (double *) NULL, 
            (double *) NULL, (double *) NULL);

    cpmix[0] = D2GK_GDT2;
    cpmix[1] = D2HM_GDT2;
    cpmix[2] = D2IL_GDT2;
    cpmix[3] = D2PY_GDT2;

    for (i=0; i<NA; i++) {
      for (j=0; j<NS; j++) 
        cpmix[i] += 2.0*d2gdsdt[i][j]*dsdt[j] + d2gds2[i][j]*SQUARE(dsdt[j]);
      cpmix[i] *= -t;
    }
  }

  if(mask & SEVENTH) {
    double d2gdsdt[NA][NS], d2gds2[NA][NS], d2gdt2[NA], d3gds3[NA][NS],
      d3gds2dt[NA][NS], d3gdsdt2[NA][NS], d3gdt3[NA], dsdt[NS], d2sdt2[NS], 
      temp;

    fillD2GDT2
    fillD2GDSDT
    fillD2GDS2
    fillD3GDS3
    fillD3GDS2DT
    fillD3GDSDT2
    fillD3GDT3

    pureOrder(SECOND | FOURTH, t, p, 
            (double *) NULL, dsdt,            (double *) NULL, d2sdt2,          
            (double *) NULL, (double *) NULL);

    for (i=0; i<NA; i++) {
      temp = d2gdt2[i];
      for (j=0; j<NS; j++) 
        temp += 2.0*d2gdsdt[i][j]*dsdt[j] + d2gds2[i][j]*SQUARE(dsdt[j]); 

      cpmixdt[i] = d3gdt3[i];
      for (j=0; j<NS; j++) 
        cpmixdt[i] += 3.0*d3gdsdt2[i][j]*dsdt[j] 
                    + 3.0*d2gdsdt[i][j]*d2sdt2[j]
                    + 3.0*d2gds2[i][j]*dsdt[j]*d2sdt2[j] 
                    + 3.0*d3gds2dt[i][j]*dsdt[j]*dsdt[j]
                    + d3gds3[i][j]*dsdt[j]*dsdt[j]*dsdt[j]; 

      cpmixdt[i] = -t*cpmixdt[i] - temp;
    }
  }

  if (mask & EIGHTH) {
    vmix[0] = DGK_GDP;
    vmix[1] = DHM_GDP;
    vmix[2] = DIL_GDP;
    vmix[3] = DPY_GDP;
  }

  if(mask & NINTH) {
    double d2gdsdt[NA][NS], d2gdsdp[NA][NS], d2gds2[NA][NS], d2gdtdp[NA],
      dsdt[NS], dsdp[NS];

    fillD2GDSDT
    fillD2GDS2
    fillD2GDSDP
    fillD2GDTDP

    pureOrder(SECOND | THIRD, t, p, 
            (double *) NULL, dsdt,            dsdp,            (double *) NULL, 
            (double *) NULL, (double *) NULL);

    for (i=0; i<NA; i++) {
      vmixdt[i] = d2gdtdp[i];
      for (j=0; j<NS; j++)
        vmixdt[i] += d2gdsdt[i][j]*dsdp[j] + d2gdsdp[i][j]*dsdt[j] 
                + d2gds2[i][j]*dsdt[j]*dsdp[j];
    } 
  }

  if(mask & TENTH) {
    double d2gdsdp[NA][NS], d2gds2[NA][NS], d2gdp2[NA], dsdp[NS];

    fillD2GDS2
    fillD2GDSDP
    fillD2GDP2

    pureOrder(THIRD, t, p, 
            (double *) NULL, (double *) NULL, dsdp,            (double *) NULL, 
            (double *) NULL, (double *) NULL);

    for (i=0; i<NA; i++) {
      vmixdp[i] = d2gdp2[i];
      for (j=0; j<NS; j++)
        vmixdp[i] += 2.0*d2gdsdp[i][j]*dsdp[j] + d2gds2[i][j]*dsdp[j]*dsdp[j];
    }
  }

  if(mask & ELEVENTH) {
    double d2gdsdt[NA][NS], d2gdsdp[NA][NS], d2gds2[NA][NS], d3gds3[NA][NS],
      d3gds2dt[NA][NS], d3gdsdt2[NA][NS], d3gds2dp[NA][NS], d3gdsdtdp[NA][NS],
      d3gdt2dp[NA], dsdt[NS], dsdp[NS], d2sdt2[NS], d2sdtdp[NS];

    fillD2GDSDT
    fillD2GDS2
    fillD3GDS3
    fillD3GDS2DT
    fillD3GDSDT2
    fillD2GDSDP
    fillD3GDS2DP
    fillD3GDSDTDP
    fillD3GDT2DP

    pureOrder(SECOND | THIRD | FOURTH | FIFTH, t, p, 
            (double *) NULL, dsdt,            dsdp,            d2sdt2,          
            d2sdtdp,         (double *) NULL);

    for (i=0; i<NA; i++) {
      vmixdt2[i] = d3gdt2dp[i];
      for (j=0; j<NS; j++) 
        vmixdt2[i] += d3gdsdt2[i][j]*dsdp[j] 
                    + 2.0*d2gdsdt[i][j]*d2sdtdp[j] 
                    + d2gdsdp[i][j]*d2sdt2[j] + 2.0*d3gdsdtdp[i][j]*dsdt[j]
                    + 2.0*d3gds2dt[i][j]*dsdt[j]*dsdp[j]
                    + d2gds2[i][j]*d2sdt2[j]*dsdp[j]
                    + 2.0*d2gds2[i][j]*dsdt[j]*d2sdtdp[j]
                    + d3gds2dp[i][j]*dsdt[j]*dsdt[j]
                    + d3gds3[i][j]*dsdt[j]*dsdt[j]*dsdp[j]; 
    }
  }

  if(mask & TWELFTH) {
    double d2gdsdt[NA][NS], d2gdsdp[NA][NS], d2gds2[NA][NS], d3gds3[NA][NS],
      d3gds2dt[NA][NS], d3gds2dp[NA][NS], d3gdsdtdp[NA][NS], d3gdsdp2[NA][NS],
      d3gdtdp2[NA], dsdt[NS], dsdp[NS], d2sdtdp[NS], d2sdp2[NS];

    fillD2GDSDT
    fillD2GDS2
    fillD3GDS3
    fillD3GDS2DT
    fillD2GDSDP
    fillD3GDS2DP
    fillD3GDSDTDP
    fillD3GDSDP2
    fillD3GDTDP2

    pureOrder(SECOND | THIRD | FIFTH | SIXTH, t, p, 
            (double *) NULL, dsdt,            dsdp,            (double *) NULL, 
            d2sdtdp,         d2sdp2);

    for (i=0; i<NA; i++) {
      vmixdtdp[i] = d3gdtdp2[i];
      for (j=0; j<NS; j++)
        vmixdtdp[i] += 2.0*d3gdsdtdp[i][j]*dsdp[j] + d2gdsdt[i][j]*d2sdp2[j]  
                     + 2.0*d2gdsdp[i][j]*d2sdtdp[j] + d3gdsdp2[i][j]*dsdt[j]
                     + 2.0*d3gds2dp[i][j]*dsdt[j]*dsdp[j] 
                     + d2gds2[i][j]*dsdt[j]*d2sdp2[j] 
                     + 2.0*d2gds2[i][j]*d2sdtdp[j]*dsdp[j]
                     + d3gds2dt[i][j]*dsdp[j]*dsdp[j]
                     + d3gds3[i][j]*dsdt[j]*dsdp[j]*dsdp[j]; 
    }
  }

  if(mask & THIRTEENTH) {
    double d2gdsdp[NA][NS], d2gds2[NA][NS], d3gds3[NA][NS], d3gds2dp[NA][NS],
      d3gdsdp2[NA][NS], d3gdp3[NA], dsdp[NS], d2sdp2[NS];

    fillD2GDS2
    fillD3GDS3
    fillD2GDSDP
    fillD3GDS2DP
    fillD3GDSDP2
    fillD3GDP3

    pureOrder(THIRD | SIXTH, t, p, 
            (double *) NULL, (double *) NULL, dsdp,            (double *) NULL, 
            (double *) NULL, d2sdp2);

    for (i=0; i<NA; i++) {
      vmixdp2[i] = d3gdp3[i];
      for (j=0; j<NS; j++) 
        vmixdp2[i] += 3.0*d3gdsdp2[i][j]*dsdp[j] + 3.0*d2gdsdp[i][j]*d2sdp2[j]
                    + 3.0*d2gds2[i][j]*dsdp[j]*d2sdp2[j]
                    + 3.0*d3gds2dp[i][j]*dsdp[j]*dsdp[j]
                    + d3gds3[i][j]*dsdp[j]*dsdp[j]*dsdp[j];
    }
  }

}

#undef fillD2GDS2
#undef fillD2GDSDT
#undef fillD2GDSDP
#undef fillD2GDT2
#undef fillD2GDTDP
#undef fillD2GDP2
#undef fillD3GDS3
#undef fillD3GDS2DT
#undef fillD3GDS2DP
#undef fillD3GDSDT2
#undef fillD3GDSDTDP
#undef fillD3GDSDP2
#undef fillD3GDT3
#undef fillD3GDT2DP
#undef fillD3GDTDP2
#undef fillD3GDP3

#undef IL_S
#undef IL_H
#undef IL_G
#undef DIL_GDS0
#undef DIL_GDT
#undef DIL_GDP
#undef D2IL_GDS0S0
#undef D2IL_GDS0DT
#undef D2IL_GDS0DP
#undef D2IL_GDT2
#undef D2IL_GDTP
#undef D2IL_GDP2
#undef D3IL_GDS0S0S0
#undef D3IL_GDS0S0DT
#undef D3IL_GDS0S0DP
#undef D3IL_GDS0DT2  
#undef D3IL_GDS0DTDP
#undef D3IL_GDS0DP2 
#undef D3IL_GDT3    
#undef D3IL_GDT2DP  
#undef D3IL_GDTDP2  
#undef D3IL_GDP3    

#undef GK_S
#undef GK_H
#undef GK_G
#undef DGK_GDS1
#undef DGK_GDT
#undef DGK_GDP
#undef D2GK_GDS1S1
#undef D2GK_GDS1DT
#undef D2GK_GDS1DP
#undef D2GK_GDT2
#undef D2GK_GDTP
#undef D2GK_GDP2
#undef D3GK_GDS1S1S1
#undef D3GK_GDS1S1DT
#undef D3GK_GDS1S1DP
#undef D3GK_GDS1DT2  
#undef D3GK_GDS1DTDP
#undef D3GK_GDS1DP2 
#undef D3GK_GDT3    
#undef D3GK_GDT2DP  
#undef D3GK_GDTDP2  
#undef D3GK_GDP3    

#undef PY_S
#undef PY_H
#undef PY_G
#undef DPY_GDS2
#undef DPY_GDT
#undef DPY_GDP
#undef D2PY_GDS2S2
#undef D2PY_GDS2DT
#undef D2PY_GDS2DP
#undef D2PY_GDT2
#undef D2PY_GDTP
#undef D2PY_GDP2
#undef D3PY_GDS2S2S2
#undef D3PY_GDS2S2DT
#undef D3PY_GDS2S2DP
#undef D3PY_GDS2DT2  
#undef D3PY_GDS2DTDP
#undef D3PY_GDS2DP2 
#undef D3PY_GDT3    
#undef D3PY_GDT2DP  
#undef D3PY_GDTDP2  
#undef D3PY_GDP3    

#undef HM_S        
#undef HM_H        
#undef HM_G        
#undef DHM_GDT
#undef DHM_GDP     
#undef D2HM_GDT2   
#undef D2HM_GDTP   
#undef D2HM_GDP2   
#undef D3HM_GDT3   
#undef D3HM_GDT2DP 
#undef D3HM_GDTDP2 
#undef D3HM_GDP3   

/*
 * Global (to this file): activity definitions and component transforms
 *    The function conRhm defines the conversion from m[i], to r[j]
 */
                   /* Order: Xil, Xgk, Xpy */
#define FR0(i)     (i == 2) ? 1.0 - r[0] : - r[0]
#define FR1(i)     (i == 0) ? 1.0 - r[1] : - r[1]
#define FR2(i)     (i == 3) ? 1.0 - r[2] : - r[2]

                   /* Order: s, t, u */
#define GS0(i)     (i == 2) ? 1.0 - s[0] : - s[0]
#define GS1(i)     (i == 0) ? 1.0 - s[1] : - s[1]
#define GS2(i)     (i == 3) ? 1.0 - s[2] : - s[2]

#define DFR0DR0(i) - 1.0                                 
#define DFR1DR1(i) - 1.0
#define DFR2DR2(i) - 1.0

#define DGS0DS0(i) - 1.0
#define DGS1DS1(i) - 1.0
#define DGS2DS2(i) - 1.0

#define ENDMEMBERS (r[1]*ends[0] + (1.0-r[0]-r[1]-r[2])*ends[1] + r[0]*ends[2] \
                    + r[2]*ends[3])

#define DENDDR0    (ends[2] - ends[1])
#define DENDDR1    (ends[0] - ends[1])
#define DENDDR2    (ends[3] - ends[1])

/*
 * Global (to this file): derivative definitions
 */

#define S  - R*(  xfe2A*log(xfe2A) + xmg2A*log(xmg2A) + xmn2A*log(xmn2A) \
                + xti4A*log(xti4A) + xfe3A*log(xfe3A) \
                + xfe2B*log(xfe2B) + xmg2B*log(xmg2B) + xmn2B*log(xmn2B) \
                + xti4B*log(xti4B) + xfe3B*log(xfe3B) )
#define H  hil*(r[0]-s[0]*s[0]) + hgk*(r[1]-s[1]*s[1]) + hpy*(r[2]-s[2]*s[2]) \
           + wilhmDIS*r[0]*(1.0-r[0]-r[1]-r[2]) + wilgkDIS*r[0]*r[1] \
           + wilpyDIS*r[0]*r[2] + wgkhmDIS*r[1]*(1.0-r[0]-r[1]-r[2]) \
           + wgkpyDIS*r[1]*r[2] + wpyhmDIS*r[2]*(1.0-r[0]-r[1]-r[2]) \
           - 2.0*s[0]*s[0]*(1.0-r[0]-r[1]-r[2])*(dwilhm+hil) \
           - 2.0*s[1]*s[1]*(1.0-r[0]-r[1]-r[2])*(dwgkhm+hgk) \
           - 2.0*s[2]*s[2]*(1.0-r[0]-r[1]-r[2])*(dwpyhm+hpy) \
           + 2.0*s[0]*s[0]*r[1]*(dwilgk-hil) + 2.0*s[0]*s[0]*r[2]*(dwilpy-hil) \
           + 2.0*s[1]*s[1]*r[0]*(dwgkil-hgk) + 2.0*s[1]*s[1]*r[2]*(dwgkpy-hgk) \
           + 2.0*s[2]*s[2]*r[0]*(dwpyil-hpy) + 2.0*s[2]*s[2]*r[1]*(dwpygk-hpy) \
           + s[0]*s[1]*(wilgkORD-wilgkDIS-dwilgk-dwgkil) \
           + s[0]*s[2]*(wilpyORD-wilpyDIS-dwilpy-dwpyil) \
           + s[1]*s[2]*(wgkpyORD-wgkpyDIS-dwgkpy-dwpygk)
#define G  H - t*(S)

/*----------------------------------------------------------------------------*/

#define DGDR0  R*t*(  0.5*log(xfe2A) + 0.5*log(xti4A) - log(xfe3A) \
                    + 0.5*log(xfe2B) + 0.5*log(xti4B) - log(xfe3B) ) \
               + hil + wilhmDIS*(1.0-2.0*r[0]-r[1]-r[2]) + wilgkDIS*r[1] \
               + wilpyDIS*r[2] - wgkhmDIS*r[1] - wpyhmDIS*r[2] \
               + 2.0*s[0]*s[0]*(dwilhm+hil) + 2.0*s[1]*s[1]*(dwgkhm+hgk) \
               + 2.0*s[2]*s[2]*(dwpyhm+hpy) + 2.0*s[1]*s[1]*(dwgkil-hgk) \
               + 2.0*s[2]*s[2]*(dwpyil-hpy) 
#define DGDR1  R*t*(  0.5*log(xmg2A) + 0.5*log(xti4A) - log(xfe3A) \
                    + 0.5*log(xmg2B) + 0.5*log(xti4B) - log(xfe3B) ) \
               + hgk - wilhmDIS*r[0] + wilgkDIS*r[0] \
               + wgkhmDIS*(1.0-r[0]-2.0*r[1]-r[2]) + wgkpyDIS*r[2] \
               - wpyhmDIS*r[2] + 2.0*s[0]*s[0]*(dwilhm+hil) \
               + 2.0*s[1]*s[1]*(dwgkhm+hgk) + 2.0*s[2]*s[2]*(dwpyhm+hpy) \
               + 2.0*s[0]*s[0]*(dwilgk-hil) + 2.0*s[2]*s[2]*(dwpygk-hpy)
#define DGDR2  R*t*(  0.5*log(xmn2A) + 0.5*log(xti4A) - log(xfe3A) \
                    + 0.5*log(xmn2B) + 0.5*log(xti4B) - log(xfe3B) ) \
               + hpy - wilhmDIS*r[0] + wilpyDIS*r[0] - wgkhmDIS*r[1] \
               + wgkpyDIS*r[1] + wpyhmDIS*(1.0-r[0]-r[1]-2.0*r[2]) \
               + 2.0*s[0]*s[0]*(dwilhm+hil) + 2.0*s[1]*s[1]*(dwgkhm+hgk) \
               + 2.0*s[2]*s[2]*(dwpyhm+hpy) + 2.0*s[0]*s[0]*(dwilpy-hil) \
               + 2.0*s[1]*s[1]*(dwgkpy-hgk)
#define DGDS0  0.5*R*t*( log(xfe2A) - log(xti4A) - log(xfe2B) + log(xti4B) ) \
               - 2.0*hil*s[0] - 4.0*s[0]*(1.0-r[0]-r[1]-r[2])*(dwilhm+hil) \
               + 4.0*s[0]*r[1]*(dwilgk-hil) + 4.0*s[0]*r[2]*(dwilpy-hil) \
               + s[1]*(wilgkORD-wilgkDIS-dwilgk-dwgkil) \
               + s[2]*(wilpyORD-wilpyDIS-dwilpy-dwpyil)
#define DGDS1  0.5*R*t*( log(xmg2A) - log(xti4A) - log(xmg2B) + log(xti4B) ) \
               - 2.0*hgk*s[1] - 4.0*s[1]*(1.0-r[0]-r[1]-r[2])*(dwgkhm+hgk) \
               + 4.0*s[1]*r[0]*(dwgkil-hgk) + 4.0*s[1]*r[2]*(dwgkpy-hgk) \
               + s[0]*(wilgkORD-wilgkDIS-dwilgk-dwgkil) \
               + s[2]*(wgkpyORD-wgkpyDIS-dwgkpy-dwpygk)
#define DGDS2  0.5*R*t*( log(xmn2A) - log(xti4A) - log(xmn2B) + log(xti4B) ) \
               - 2.0*hpy*s[2] - 4.0*s[2]*(1.0-r[0]-r[1]-r[2])*(dwpyhm+hpy) \
               + 4.0*s[2]*r[0]*(dwpyil-hpy) + 4.0*s[2]*r[1]*(dwpygk-hpy) \
               + s[0]*(wilpyORD-wilpyDIS-dwilpy-dwpyil) \
               + s[1]*(wgkpyORD-wgkpyDIS-dwgkpy-dwpygk)
#define DGDT   - (S)
#define DGDP   0.0

/*----------------------------------------------------------------------------*/

#define D2GDR0R0  R*t*(  0.25/xfe2A + 0.25/xti4A + 1.0/xfe3A \
                       + 0.25/xfe2B + 0.25/xti4B + 1.0/xfe3B ) - 2.0*wilhmDIS
#define D2GDR0R1  R*t*(  0.25/xti4A + 1.0/xfe3A + 0.25/xti4B + 1.0/xfe3B ) \
                  - wilhmDIS + wilgkDIS - wgkhmDIS
#define D2GDR0R2  R*t*(  0.25/xti4A + 1.0/xfe3A + 0.25/xti4B + 1.0/xfe3B ) \
                  - wilhmDIS + wilpyDIS - wpyhmDIS
#define D2GDR0S0  0.25*R*t*( 1.0/xfe2A - 1.0/xti4A - 1.0/xfe2B + 1.0/xti4B ) \
                  + 4.0*s[0]*(dwilhm+hil) 
#define D2GDR0S1  0.25*R*t*( - 1.0/xti4A + 1.0/xti4B ) \
                  + 4.0*s[1]*(dwgkhm+hgk) + 4.0*s[1]*(dwgkil-hgk) 
#define D2GDR0S2  0.25*R*t*( - 1.0/xti4A + 1.0/xti4B ) \
                  + 4.0*s[2]*(dwpyhm+hpy) + 4.0*s[2]*(dwpyil-hpy) 
#define D2GDR0DT  R*(  0.5*log(xfe2A) + 0.5*log(xti4A) - log(xfe3A) \
                     + 0.5*log(xfe2B) + 0.5*log(xti4B) - log(xfe3B) )
#define D2GDR0DP  0.0

#define D2GDR1R1  R*t*(  0.25/xmg2A + 0.25/xti4A + 1.0/xfe3A \
                       + 0.25/xmg2B + 0.25/xti4B + 1.0/xfe3B ) - 2.0*wgkhmDIS 
#define D2GDR1R2  R*t*( 0.25/xti4A + 1.0/xfe3A + 0.25/xti4B + 1.0/xfe3B ) \
                  - wgkhmDIS + wgkpyDIS - wpyhmDIS 
#define D2GDR1S0  0.25*R*t*( - 1.0/xti4A + 1.0/xti4B ) \
                  + 4.0*s[0]*(dwilhm+hil) + 4.0*s[0]*(dwilgk-hil)
#define D2GDR1S1  0.25*R*t*( 1.0/xmg2A - 1.0/xti4A - 1.0/xmg2B + 1.0/xti4B ) \
                  + 4.0*s[1]*(dwgkhm+hgk)
#define D2GDR1S2  0.25*R*t*( - 1.0/xti4A + 1.0/xti4B ) \
                  + 4.0*s[2]*(dwpyhm+hpy) + 4.0*s[2]*(dwpygk-hpy)
#define D2GDR1DT  R*(  0.5*log(xmg2A) + 0.5*log(xti4A) - log(xfe3A) \
                     + 0.5*log(xmg2B) + 0.5*log(xti4B) - log(xfe3B) )
#define D2GDR1DP  0.0

#define D2GDR2R2  R*t*(  0.25/xmn2A + 0.25/xti4A + 1.0/xfe3A \
                       + 0.25/xmn2B + 0.25/xti4B + 1.0/xfe3B ) - 2.0*wpyhmDIS 
#define D2GDR2S0  0.25*R*t*( - 1.0/xti4A + 1.0/xti4B ) \
                  + 4.0*s[0]*(dwilhm+hil) + 4.0*s[0]*(dwilpy-hil)
#define D2GDR2S1  0.25*R*t*( - 1.0/xti4A + 1.0/xti4B ) \
                  + 4.0*s[1]*(dwgkhm+hgk) + 4.0*s[1]*(dwgkpy-hgk)
#define D2GDR2S2  0.25*R*t*( 1.0/xmn2A - 1.0/xti4A - 1.0/xmn2B + 1.0/xti4B ) \
                  + 4.0*s[2]*(dwpyhm+hpy)
#define D2GDR2DT  R*(  0.5*log(xmn2A) + 0.5*log(xti4A) - log(xfe3A) \
                     + 0.5*log(xmn2B) + 0.5*log(xti4B) - log(xfe3B) )
#define D2GDR2DP  0.0

#define D2GDS0S0  0.25*R*t*( 1.0/xfe2A + 1.0/xti4A + 1.0/xfe2B + 1.0/xti4B ) \
                  - 2.0*hil - 4.0*(1.0-r[0]-r[1]-r[2])*(dwilhm+hil) \
                  + 4.0*r[1]*(dwilgk-hil) + 4.0*r[2]*(dwilpy-hil) 
#define D2GDS0S1  0.25*R*t*( 1.0/xti4A + 1.0/xti4B ) \
                  + (wilgkORD-wilgkDIS-dwilgk-dwgkil)
#define D2GDS0S2  0.25*R*t*( 1.0/xti4A + 1.0/xti4B ) \
                  + (wilpyORD-wilpyDIS-dwilpy-dwpyil)
#define D2GDS0DT  0.5*R*( log(xfe2A) - log(xti4A) - log(xfe2B) + log(xti4B) )
#define D2GDS0DP  0.0

#define D2GDS1S1  0.25*R*t*( 1.0/xmg2A + 1.0/xti4A + 1.0/xmg2B + 1.0/xti4B ) \
                  - 2.0*hgk - 4.0*(1.0-r[0]-r[1]-r[2])*(dwgkhm+hgk) \
                  + 4.0*r[0]*(dwgkil-hgk) + 4.0*r[2]*(dwgkpy-hgk)
#define D2GDS1S2  0.25*R*t*( 1.0/xti4A + 1.0/xti4B ) \
                  + (wgkpyORD-wgkpyDIS-dwgkpy-dwpygk)
#define D2GDS1DT  0.5*R*( log(xmg2A) - log(xti4A) - log(xmg2B) + log(xti4B) )
#define D2GDS1DP  0.0 

#define D2GDS2S2  0.25*R*t*( 1.0/xmn2A + 1.0/xti4A + 1.0/xmn2B + 1.0/xti4B ) \
                  - 2.0*hpy - 4.0*(1.0-r[0]-r[1]-r[2])*(dwpyhm+hpy) \
                  + 4.0*r[0]*(dwpyil-hpy) + 4.0*r[1]*(dwpygk-hpy)
#define D2GDS2DT  0.5*R*( log(xmn2A) - log(xti4A) - log(xmn2B) + log(xti4B) )
#define D2GDS2DP  0.0

#define D2GDT2    0.0
#define D2GDTDP   0.0
#define D2GDP2    0.0

/*----------------------------------------------------------------------------*/

#define D3GDR0R0R0  - 0.125*R*t*( 1.0/SQUARE(xfe2A) + 1.0/SQUARE(xti4A) \
                      - 8.0/SQUARE(xfe3A) + 1.0/SQUARE(xfe2B) + \
		      1.0/SQUARE(xti4B) - 8.0/SQUARE(xfe3B) )
#define D3GDR0R0R1  - 0.125*R*t*( 1.0/SQUARE(xti4A) - 8.0/SQUARE(xfe3A) \
		      + 1.0/SQUARE(xti4B) - 8.0/SQUARE(xfe3B) )
#define D3GDR0R0S0  - 0.125*R*t*( 1.0/SQUARE(xfe2A) - 1.0/SQUARE(xti4A) \
                      - 1.0/SQUARE(xfe2B) + 1.0/SQUARE(xti4B) )
#define D3GDR0R0S1  - 0.125*R*t*( - 1.0/SQUARE(xti4A) + 1.0/SQUARE(xti4B) )
#define D3GDR0R0S2  - 0.125*R*t*( - 1.0/SQUARE(xti4A) + 1.0/SQUARE(xti4B) )
#define D3GDR0R0DT  R*(  0.25/xfe2A + 0.25/xti4A + 1.0/xfe3A \
                       + 0.25/xfe2B + 0.25/xti4B + 1.0/xfe3B )
#define D3GDR0R0DP  0.0

#define D3GDR0R1S0  - 0.125*R*t*( - 1.0/SQUARE(xti4A) + 1.0/SQUARE(xti4B) )
#define D3GDR0R1S1  - 0.125*R*t*( - 1.0/SQUARE(xti4A) + 1.0/SQUARE(xti4B) )
#define D3GDR0R1S2  - 0.125*R*t*( - 1.0/SQUARE(xti4A) + 1.0/SQUARE(xti4B) )
#define D3GDR0R1DT  R*(  0.25/xti4A + 1.0/xfe3A + 0.25/xti4B + 1.0/xfe3B )
#define D3GDR0R1DP  0.0
  
#define D3GDR0R2S0  - 0.125*R*t*( - 1.0/SQUARE(xti4A) + 1.0/SQUARE(xti4B) )
#define D3GDR0R2S1  - 0.125*R*t*( - 1.0/SQUARE(xti4A) + 1.0/SQUARE(xti4B) )
#define D3GDR0R2S2  - 0.125*R*t*( - 1.0/SQUARE(xti4A) + 1.0/SQUARE(xti4B) )
#define D3GDR0R2DT  R*(  0.25/xti4A + 1.0/xfe3A + 0.25/xti4B + 1.0/xfe3B )
#define D3GDR0R2DP  0.0
  
#define D3GDR0S0S0  - 0.125*R*t*( 1.0/SQUARE(xfe2A) + 1.0/SQUARE(xti4A) \
                      + 1.0/SQUARE(xfe2B) + 1.0/SQUARE(xti4B) ) \
                    + 4.0*(dwilhm+hil)
#define D3GDR0S0S1  - 0.125*R*t*( 1.0/SQUARE(xti4A) + 1.0/SQUARE(xti4B) )
#define D3GDR0S0S2  - 0.125*R*t*( 1.0/SQUARE(xti4A) + 1.0/SQUARE(xti4B) )
#define D3GDR0S0DT  0.25*R*( 1.0/xfe2A - 1.0/xti4A - 1.0/xfe2B + 1.0/xti4B )
#define D3GDR0S0DP  0.0

#define D3GDR0S1S1  - 0.125*R*t*( 1.0/SQUARE(xti4A) + 1.0/SQUARE(xti4B) ) \
                    + 4.0*(dwgkhm+hgk) + 4.0*(dwgkil-hgk)
#define D3GDR0S1S2  - 0.125*R*t*( 1.0/SQUARE(xti4A) + 1.0/SQUARE(xti4B) )
#define D3GDR0S1DT  0.25*R*( - 1.0/xti4A + 1.0/xti4B )
#define D3GDR0S1DP  0.0

#define D3GDR0S2S2  - 0.125*R*t*( 1.0/SQUARE(xti4A) + 1.0/SQUARE(xti4B) ) \
                    + 4.0*(dwpyhm+hpy) + 4.0*(dwpyil-hpy)
#define D3GDR0S2DT  0.25*R*( - 1.0/xti4A + 1.0/xti4B )
#define D3GDR0S2DP  0.0

#define D3GDR1R1R1  - 0.125*R*t*( 1.0/SQUARE(xmg2A) + 1.0/SQUARE(xti4A) \
                      - 8.0/SQUARE(xfe3A) + 1.0/SQUARE(xmg2B) + \
		      1.0/SQUARE(xti4B) - 8.0/SQUARE(xfe3B) )
#define D3GDR1R1S0  - 0.125*R*t*( - 1.0/SQUARE(xti4A) + 1.0/SQUARE(xti4B) )
#define D3GDR1R1S1  - 0.125*R*t*( 1.0/SQUARE(xmg2A) - 1.0/SQUARE(xti4A) \
                       - 1.0/SQUARE(xmg2B) + 1.0/SQUARE(xti4B) )
#define D3GDR1R1S2  - 0.125*R*t*( - 1.0/SQUARE(xti4A) + 1.0/SQUARE(xti4B) )
#define D3GDR1R1DT  R*(  0.25/xmg2A + 0.25/xti4A + 1.0/xfe3A \
                       + 0.25/xmg2B + 0.25/xti4B + 1.0/xfe3B )
#define D3GDR1R1DP  0.0

#define D3GDR1R2S0  - 0.125*R*t*( - 1.0/SQUARE(xti4A) + 1.0/SQUARE(xti4B) )
#define D3GDR1R2S1  - 0.125*R*t*( - 1.0/SQUARE(xti4A) + 1.0/SQUARE(xti4B) )
#define D3GDR1R2S2  - 0.125*R*t*( - 1.0/SQUARE(xti4A) + 1.0/SQUARE(xti4B) )
#define D3GDR1R2DT  R*( 0.25/xti4A + 1.0/xfe3A + 0.25/xti4B + 1.0/xfe3B )
#define D3GDR1R2DP  0.0

#define D3GDR1S0S0  - 0.125*R*t*( 1.0/SQUARE(xti4A) + 1.0/SQUARE(xti4B) ) \
                    + 4.0*(dwilhm+hil) + 4.0*(dwilgk-hil)
#define D3GDR1S0S1  - 0.125*R*t*( 1.0/SQUARE(xti4A) + 1.0/SQUARE(xti4B) )
#define D3GDR1S0S2  - 0.125*R*t*( 1.0/SQUARE(xti4A) + 1.0/SQUARE(xti4B) )
#define D3GDR1S0DT  0.25*R*( - 1.0/xti4A + 1.0/xti4B )
#define D3GDR1S0DP  0.0

#define D3GDR1S1S1  - 0.125*R*t*( 1.0/SQUARE(xmg2A) + 1.0/SQUARE(xti4A) \
                        + 1.0/SQUARE(xmg2B) + 1.0/SQUARE(xti4B) ) \
                    + 4.0*(dwgkhm+hgk)
#define D3GDR1S1S2  - 0.125*R*t*( 1.0/SQUARE(xti4A) + 1.0/SQUARE(xti4B) )
#define D3GDR1S1DT  0.25*R*( 1.0/xmg2A - 1.0/xti4A - 1.0/xmg2B + 1.0/xti4B )
#define D3GDR1S1DP  0.0

#define D3GDR1S2S2  - 0.125*R*t*( 1.0/SQUARE(xti4A) + 1.0/SQUARE(xti4B) ) \
                    + 4.0*(dwpyhm+hpy) + 4.0*(dwpygk-hpy)
#define D3GDR1S2DT  0.25*R*( - 1.0/xti4A + 1.0/xti4B )
#define D3GDR1S2DP  0.0

#define D3GDR2R2R2  - 0.125*R*t*( 1.0/SQUARE(xmn2A) + 1.0/SQUARE(xti4A) \
                      - 8.0/SQUARE(xfe3A) + 1.0/SQUARE(xmn2B) + \
		      1.0/SQUARE(xti4B) - 8.0/SQUARE(xfe3B) )
#define D3GDR2R2S0  - 0.125*R*t*( - 1.0/SQUARE(xti4A) + 1.0/SQUARE(xti4B) )
#define D3GDR2R2S1  - 0.125*R*t*( - 1.0/SQUARE(xti4A) + 1.0/SQUARE(xti4B) )
#define D3GDR2R2S2  - 0.125*R*t*( 1.0/SQUARE(xmn2A) - 1.0/SQUARE(xti4A) \
                        - 1.0/SQUARE(xmn2B) + 1.0/SQUARE(xti4B) )
#define D3GDR2R2DT  R*(  0.25/xmn2A + 0.25/xti4A + 1.0/xfe3A \
                       + 0.25/xmn2B + 0.25/xti4B + 1.0/xfe3B )
#define D3GDR2R2DP  0.0 

#define D3GDR2S0S0  - 0.125*R*t*( 1.0/SQUARE(xti4A) + 1.0/SQUARE(xti4B) ) \
                    + 4.0*(dwilhm+hil) + 4.0*(dwilpy-hil)
#define D3GDR2S0S1  - 0.125*R*t*( 1.0/SQUARE(xti4A) + 1.0/SQUARE(xti4B) )
#define D3GDR2S0S2  - 0.125*R*t*( 1.0/SQUARE(xti4A) + 1.0/SQUARE(xti4B) )
#define D3GDR2S0DT  0.25*R*( - 1.0/xti4A + 1.0/xti4B )
#define D3GDR2S0DP  0.0

#define D3GDR2S1S1  - 0.125*R*t*( 1.0/SQUARE(xti4A) + 1.0/SQUARE(xti4B) ) \
                    + 4.0*(dwgkhm+hgk) + 4.0*(dwgkpy-hgk)
#define D3GDR2S1S2  - 0.125*R*t*( 1.0/SQUARE(xti4A) + 1.0/SQUARE(xti4B) )
#define D3GDR2S1DT  0.25*R*( - 1.0/xti4A + 1.0/xti4B )
#define D3GDR2S1DP  0.0

#define D3GDR2S2S2  - 0.125*R*t*( 1.0/SQUARE(xmn2A) + 1.0/SQUARE(xti4A) \
                        + 1.0/SQUARE(xmn2B) + 1.0/SQUARE(xti4B) ) \
                    + 4.0*(dwpyhm+hpy)
#define D3GDR2S2DT  0.25*R*( 1.0/xmn2A - 1.0/xti4A - 1.0/xmn2B + 1.0/xti4B )
#define D3GDR2S2DP  0.0

#define D3GDS0S0S0  - 0.125*R*t*( 1.0/SQUARE(xfe2A) - 1.0/SQUARE(xti4A) \
                       - 1.0/SQUARE(xfe2B) + 1.0/SQUARE(xti4B) )
#define D3GDS0S0S1  - 0.125*R*t*( - 1.0/SQUARE(xti4A) + 1.0/SQUARE(xti4B) )
#define D3GDS0S0S2  - 0.125*R*t*( - 1.0/SQUARE(xti4A) + 1.0/SQUARE(xti4B) )
#define D3GDS0S0DT  0.25*R*( 1.0/xfe2A + 1.0/xti4A + 1.0/xfe2B + 1.0/xti4B )
#define D3GDS0S0DP  0.0
#define D3GDS0S1S1  - 0.125*R*t*( - 1.0/SQUARE(xti4A) + 1.0/SQUARE(xti4B) )
#define D3GDS0S1S2  - 0.125*R*t*( - 1.0/SQUARE(xti4A) + 1.0/SQUARE(xti4B) )
#define D3GDS0S1DT  0.25*R*( 1.0/xti4A + 1.0/xti4B )
#define D3GDS0S1DP  0.0
#define D3GDS0S2S2  - 0.125*R*t*( - 1.0/SQUARE(xti4A) + 1.0/SQUARE(xti4B) )
#define D3GDS0S2DT  0.25*R*( 1.0/xti4A + 1.0/xti4B )
#define D3GDS0S2DP  0.0
#define D3GDS0DT2   0.0
#define D3GDS0DTDP  0.0
#define D3GDS0DP2   0.0
  
#define D3GDS1S1S1  - 0.125*R*t*( 1.0/SQUARE(xmg2A) - 1.0/SQUARE(xti4A) \
                       - 1.0/SQUARE(xmg2B) + 1.0/SQUARE(xti4B) )
#define D3GDS1S1S2  - 0.125*R*t*( - 1.0/SQUARE(xti4A) + 1.0/SQUARE(xti4B) )
#define D3GDS1S1DT  0.25*R*( 1.0/xmg2A + 1.0/xti4A + 1.0/xmg2B + 1.0/xti4B )
#define D3GDS1S1DP  0.0
#define D3GDS1S2S2  - 0.125*R*t*( - 1.0/SQUARE(xti4A) + 1.0/SQUARE(xti4B) )
#define D3GDS1S2DT  0.25*R*( 1.0/xti4A + 1.0/xti4B )
#define D3GDS1S2DP  0.0
#define D3GDS1DT2   0.0
#define D3GDS1DTDP  0.0
#define D3GDS1DP2   0.0
  
#define D3GDS2S2S2  - 0.125*R*t*( 1.0/SQUARE(xmn2A) - 1.0/SQUARE(xti4A) \
                       - 1.0/SQUARE(xmn2B) + 1.0/SQUARE(xti4B) )
#define D3GDS2S2DT  0.25*R*( 1.0/xmn2A + 1.0/xti4A + 1.0/xmn2B + 1.0/xti4B )
#define D3GDS2S2DP  0.0
#define D3GDS2DT2   0.0
#define D3GDS2DTDP  0.0
#define D3GDS2DP2   0.0

#define D3GDT3      0.0
#define D3GDT2DP    0.0
#define D3GDTDP2    0.0
#define D3GDP3      0.0

#define D3GDR0DT2   0.0
#define D3GDR0DTDP  0.0
#define D3GDR0DP2   0.0
#define D3GDR1DT2   0.0
#define D3GDR1DTDP  0.0
#define D3GDR1DP2   0.0
#define D3GDR2DT2   0.0
#define D3GDR2DTDP  0.0
#define D3GDR2DP2   0.0

/*
 *=============================================================================
 * Macros for automatic array initialization
 */
#define fillD2GDR2 \
 d2gdr2[0][0] = D2GDR0R0;     d2gdr2[0][1] = D2GDR0R1; \
 d2gdr2[0][2] = D2GDR0R2; \
 d2gdr2[1][0] = d2gdr2[0][1]; d2gdr2[1][1] = D2GDR1R1; \
 d2gdr2[1][2] = D2GDR1R2; \
 d2gdr2[2][0] = d2gdr2[0][2]; d2gdr2[2][1] = d2gdr2[1][2]; \
 d2gdr2[2][2] = D2GDR2R2;

#define fillD2GDRDS \
 d2gdrds[0][0] = D2GDR0S0; d2gdrds[0][1] = D2GDR0S1; d2gdrds[0][2] = D2GDR0S2; \
 d2gdrds[1][0] = D2GDR1S0; d2gdrds[1][1] = D2GDR1S1; d2gdrds[1][2] = D2GDR1S2; \
 d2gdrds[2][0] = D2GDR2S0; d2gdrds[2][1] = D2GDR2S1; d2gdrds[2][2] = D2GDR2S2;

#define fillD2GDRDT \
 d2gdrdt[0] = D2GDR0DT; d2gdrdt[1] = D2GDR1DT; d2gdrdt[2] = D2GDR2DT;

#define fillD2GDRDP \
 d2gdrdp[0] = D2GDR0DP; d2gdrdp[1] = D2GDR1DP; d2gdrdp[2] = D2GDR2DP;

#define fillD2GDS2 \
 d2gds2[0][0] = D2GDS0S0;     d2gds2[0][1] = D2GDS0S1; \
 d2gds2[0][2] = D2GDS0S2; \
 d2gds2[1][0] = d2gds2[0][1]; d2gds2[1][1] = D2GDS1S1; \
 d2gds2[1][2] = D2GDS1S2; \
 d2gds2[2][0] = d2gds2[0][2]; d2gds2[2][1] = d2gds2[1][2]; \
 d2gds2[2][2] = D2GDS2S2;

#define fillD2GDSDT \
 d2gdsdt[0] = D2GDS0DT;  d2gdsdt[1] = D2GDS1DT; d2gdsdt[2] = D2GDS2DT;

#define fillD2GDSDP \
 d2gdsdp[0] = D2GDS0DP;  d2gdsdp[1] = D2GDS1DP; d2gdsdp[2] = D2GDS2DP;

#define fillD3GDR3 \
 d3gdr3[0][0][0] = D3GDR0R0R0;		d3gdr3[0][0][1] = D3GDR0R0R1; \
 d3gdr3[0][0][2] = d3gdr3[0][0][1];	d3gdr3[0][1][0] = d3gdr3[0][0][1]; \
 d3gdr3[0][1][1] = d3gdr3[0][0][1];	d3gdr3[0][1][2] = d3gdr3[0][0][1]; \
 d3gdr3[0][2][0] = d3gdr3[0][0][1];	d3gdr3[0][2][1] = d3gdr3[0][0][1]; \
 d3gdr3[0][2][2] = d3gdr3[0][0][1];	d3gdr3[1][0][0] = d3gdr3[0][0][1]; \
 d3gdr3[1][0][1] = d3gdr3[0][0][1];	d3gdr3[1][0][2] = d3gdr3[0][0][1]; \
 d3gdr3[1][1][0] = d3gdr3[0][0][1];	d3gdr3[1][1][1] = D3GDR1R1R1; \
 d3gdr3[1][1][2] = d3gdr3[0][0][1];	d3gdr3[1][2][0] = d3gdr3[0][0][1]; \
 d3gdr3[1][2][1] = d3gdr3[0][0][1];	d3gdr3[1][2][2] = d3gdr3[0][0][1]; \
 d3gdr3[2][0][0] = d3gdr3[0][0][1];	d3gdr3[2][0][1] = d3gdr3[0][0][1]; \
 d3gdr3[2][0][2] = d3gdr3[0][0][1];	d3gdr3[2][1][0] = d3gdr3[0][0][1]; \
 d3gdr3[2][1][1] = d3gdr3[0][0][1];	d3gdr3[2][1][2] = d3gdr3[0][0][1]; \
 d3gdr3[2][2][0] = d3gdr3[0][0][1];	d3gdr3[2][2][1] = d3gdr3[0][0][1]; \
 d3gdr3[2][2][2] = D3GDR2R2R2;

#define fillD3GDR2DS \
 d3gdr2ds[0][0][0] = D3GDR0R0S0;        d3gdr2ds[0][0][1] = D3GDR0R0S1; \
 d3gdr2ds[0][0][2] = D3GDR0R0S2; \
 d3gdr2ds[0][1][0] = D3GDR0R1S0;        d3gdr2ds[0][1][1] = D3GDR0R1S1; \
 d3gdr2ds[0][1][2] = D3GDR0R1S2; \
 d3gdr2ds[0][2][0] = D3GDR0R2S0;        d3gdr2ds[0][2][1] = D3GDR0R2S1; \
 d3gdr2ds[0][2][2] = D3GDR0R2S2; \
 d3gdr2ds[1][0][0] = d3gdr2ds[0][1][0]; d3gdr2ds[1][0][1] = d3gdr2ds[0][1][1]; \
 d3gdr2ds[1][0][2] = d3gdr2ds[0][1][2]; \
 d3gdr2ds[1][1][0] = D3GDR1R1S0;        d3gdr2ds[1][1][1] = D3GDR1R1S1; \
 d3gdr2ds[1][1][2] = D3GDR1R1S2; \
 d3gdr2ds[1][2][0] = D3GDR1R2S0;        d3gdr2ds[1][2][1] = D3GDR1R2S1; \
 d3gdr2ds[1][2][2] = D3GDR1R2S2; \
 d3gdr2ds[2][0][0] = d3gdr2ds[0][2][0]; d3gdr2ds[2][0][1] = d3gdr2ds[0][2][1]; \
 d3gdr2ds[2][0][2] = d3gdr2ds[0][2][2]; \
 d3gdr2ds[2][1][0] = d3gdr2ds[1][2][0]; d3gdr2ds[2][1][1] = d3gdr2ds[1][2][1]; \
 d3gdr2ds[2][1][2] = d3gdr2ds[1][2][2]; \
 d3gdr2ds[2][2][0] = D3GDR2R2S0;        d3gdr2ds[2][2][1] = D3GDR2R2S1; \
 d3gdr2ds[2][2][2] = D3GDR2R2S2; 

#define fillD3GDR2DT \
 d3gdr2dt[0][0] = D3GDR0R0DT;     d3gdr2dt[0][1] = D3GDR0R1DT; \
 d3gdr2dt[0][2] = D3GDR0R2DT; \
 d3gdr2dt[1][0] = d3gdr2dt[0][1]; d3gdr2dt[1][1] = D3GDR1R1DT; \
 d3gdr2dt[1][2] = D3GDR1R2DT; \
 d3gdr2dt[2][0] = d3gdr2dt[0][2]; d3gdr2dt[2][1] = d3gdr2dt[1][2]; \
 d3gdr2dt[2][2] = D3GDR2R2DT;

#define fillD3GDR2DP \
 d3gdr2dp[0][0] = D3GDR0R0DP;     d3gdr2dp[0][1] = D3GDR0R1DP; \
 d3gdr2dp[0][2] = D3GDR0R2DP; \
 d3gdr2dp[1][0] = d3gdr2dp[0][1]; d3gdr2dp[1][1] = D3GDR1R1DP; \
 d3gdr2dp[1][2] = D3GDR1R2DP; \
 d3gdr2dp[2][0] = d3gdr2dp[0][2]; d3gdr2dp[2][1] = d3gdr2dp[1][2]; \
 d3gdr2dp[2][2] = D3GDR2R2DP;

#define fillD3GDRDS2 \
 d3gdrds2[0][0][0] = D3GDR0S0S0;        d3gdrds2[0][0][1] = D3GDR0S0S1; \
 d3gdrds2[0][0][2] = D3GDR0S0S2; \
 d3gdrds2[0][1][0] = d3gdrds2[0][0][1]; d3gdrds2[0][1][1] = D3GDR0S1S1; \
 d3gdrds2[0][1][2] = D3GDR0S1S2; \
 d3gdrds2[0][2][0] = d3gdrds2[0][0][2]; d3gdrds2[0][2][1] = d3gdrds2[0][1][2]; \
 d3gdrds2[0][2][2] = D3GDR0S2S2; \
 d3gdrds2[1][0][0] = D3GDR1S0S0;        d3gdrds2[1][0][1] = D3GDR1S0S1; \
 d3gdrds2[1][0][2] = D3GDR1S0S2; \
 d3gdrds2[1][1][0] = d3gdrds2[1][0][1]; d3gdrds2[1][1][1] = D3GDR1S1S1; \
 d3gdrds2[1][1][2] = D3GDR1S1S2; \
 d3gdrds2[1][2][0] = d3gdrds2[1][0][2]; d3gdrds2[1][2][1] = d3gdrds2[1][1][2]; \
 d3gdrds2[1][2][2] = D3GDR1S2S2; \
 d3gdrds2[2][0][0] = D3GDR2S0S0;        d3gdrds2[2][0][1] = D3GDR2S0S1; \
 d3gdrds2[2][0][2] = D3GDR2S0S2; \
 d3gdrds2[2][1][0] = d3gdrds2[2][0][1]; d3gdrds2[2][1][1] = D3GDR2S1S1; \
 d3gdrds2[2][1][2] = D3GDR2S1S2; \
 d3gdrds2[2][2][0] = d3gdrds2[2][0][2]; d3gdrds2[2][2][1] = d3gdrds2[2][1][2]; \
 d3gdrds2[2][2][2] = D3GDR2S2S2;

#define fillD3GDRDSDT \
 d3gdrdsdt[0][0] = D3GDR0S0DT;  d3gdrdsdt[0][1] = D3GDR0S1DT; \
 d3gdrdsdt[0][2] = D3GDR0S2DT; \
 d3gdrdsdt[1][0] = D3GDR1S0DT;  d3gdrdsdt[1][1] = D3GDR1S1DT; \
 d3gdrdsdt[1][2] = D3GDR1S2DT; \
 d3gdrdsdt[2][0] = D3GDR2S0DT;  d3gdrdsdt[2][1] = D3GDR2S1DT; \
 d3gdrdsdt[2][2] = D3GDR2S2DT;

#define fillD3GDRDSDP \
 d3gdrdsdp[0][0] = D3GDR0S0DP; d3gdrdsdp[0][1] = D3GDR0S1DP; \
 d3gdrdsdp[0][2] = D3GDR0S2DP; \
 d3gdrdsdp[1][0] = D3GDR1S0DP; d3gdrdsdp[1][1] = D3GDR1S1DP; \
 d3gdrdsdp[1][2] = D3GDR1S2DP; \
 d3gdrdsdp[2][0] = D3GDR2S0DP; d3gdrdsdp[2][1] = D3GDR2S1DP; \
 d3gdrdsdp[2][2] = D3GDR2S2DP; 

#define fillD3GDS3 \
 d3gds3[0][0][0] = D3GDS0S0S0;      d3gds3[0][0][1] = D3GDS0S0S1; \
 d3gds3[0][0][2] = D3GDS0S0S2; \
 d3gds3[0][1][0] = d3gds3[0][0][1]; d3gds3[0][1][1] = D3GDS0S1S1; \
 d3gds3[0][1][2] = D3GDS0S1S2; \
 d3gds3[0][2][0] = d3gds3[0][0][2]; d3gds3[0][2][1] = d3gds3[0][1][2]; \
 d3gds3[0][2][2] = D3GDS0S2S2; \
 d3gds3[1][0][0] = d3gds3[0][0][1]; d3gds3[1][0][1] = d3gds3[0][1][1]; \
 d3gds3[1][0][2] = d3gds3[0][1][2]; \
 d3gds3[1][1][0] = d3gds3[0][1][1]; d3gds3[1][1][1] = D3GDS1S1S1; \
 d3gds3[1][1][2] = D3GDS1S1S2; \
 d3gds3[1][2][0] = d3gds3[0][1][2]; d3gds3[1][2][1] = d3gds3[1][1][2]; \
 d3gds3[1][2][2] = D3GDS1S2S2; \
 d3gds3[2][0][0] = d3gds3[0][0][2]; d3gds3[2][0][1] = d3gds3[0][1][2]; \
 d3gds3[2][0][2] = d3gds3[0][2][2]; \
 d3gds3[2][1][0] = d3gds3[0][1][2]; d3gds3[2][1][1] = d3gds3[1][1][2]; \
 d3gds3[2][1][2] = d3gds3[1][2][2]; \
 d3gds3[2][2][0] = d3gds3[0][2][2]; d3gds3[2][2][1] = d3gds3[1][2][2]; \
 d3gds3[2][2][2] = D3GDS2S2S2;  

#define fillD3GDS2DT \
 d3gds2dt[0][0] = D3GDS0S0DT;     d3gds2dt[0][1] = D3GDS0S1DT; \
 d3gds2dt[0][2] = D3GDS0S2DT; \
 d3gds2dt[1][0] = d3gds2dt[0][1]; d3gds2dt[1][1] = D3GDS1S1DT; \
 d3gds2dt[1][2] = D3GDS1S2DT; \
 d3gds2dt[2][0] = d3gds2dt[0][2]; d3gds2dt[2][1] = d3gds2dt[1][2]; \
 d3gds2dt[2][2] = D3GDS2S2DT;  

#define fillD3GDS2DP \
 d3gds2dp[0][0] = D3GDS0S0DP;     d3gds2dp[0][1] = D3GDS0S1DP; \
 d3gds2dp[0][2] = D3GDS0S2DP; \
 d3gds2dp[1][0] = d3gds2dp[0][1]; d3gds2dp[1][1] = D3GDS1S1DP; \
 d3gds2dp[1][2] = D3GDS1S2DP; \
 d3gds2dp[2][0] = d3gds2dp[0][2]; d3gds2dp[2][1] = d3gds2dp[1][2]; \
 d3gds2dp[2][2] = D3GDS2S2DP;

#define fillD3GDSDT2 \
 d3gdsdt2[0] = D3GDS0DT2; d3gdsdt2[1] = D3GDS1DT2; d3gdsdt2[2] = D3GDS2DT2;

#define fillD3GDSDTDP \
 d3gdsdtdp[0] = D3GDS0DTDP; d3gdsdtdp[1] = D3GDS1DTDP; \
 d3gdsdtdp[2] = D3GDS2DTDP;

#define fillD3GDSDP2 \
 d3gdsdp2[0] = D3GDS0DP2; d3gdsdp2[1] = D3GDS1DP2; d3gdsdp2[2] = D3GDS2DP2;

#define fillD3GDRDT2 \
 d3gdrdt2[0] = D3GDR0DT2; d3gdrdt2[1] = D3GDR1DT2; \
 d3gdrdt2[2] = D3GDR2DT2;

#define fillD3GDRDTDP \
 d3gdrdtdp[0] = D3GDR0DTDP; d3gdrdtdp[1] = D3GDR1DTDP; \
 d3gdrdtdp[2] = D3GDR2DTDP;

#define fillD3GDRDP2 \
 d3gdrdp2[0] = D3GDR0DP2; d3gdrdp2[1] = D3GDR1DP2; \
 d3gdrdp2[2] = D3GDR2DP2;

/*
 *=============================================================================
 * Local function to compute ordering state and associated derivatives
 */

static void 
order(int mask, double t, double p, double r[NR], 
      double s[NS],           /* s[NS]                BINARY MASK: 0000000001 */
      double dr[NS][NR] ,     /* ds[NS]/dr[NR]        BINARY MASK: 0000000010 */
      double dt[NS],          /* ds[NS]/dt            BINARY MASK: 0000000100 */
      double dp[NS],          /* ds[NS]/dp            BINARY MASK: 0000001000 */
      double dr2[NS][NR][NR], /* d2s[NS]/dr[NR]dr[NR] BINARY MASK: 0000010000 */
      double drt[NS][NR],     /* d2s[NS]/dr[NR]dt     BINARY MASK: 0000100000 */
      double drp[NS][NR],     /* d2s[NS]/dr[NR]dp     BINARY MASK: 0001000000 */
      double dt2[NS],         /* d2s[NS]/dt2          BINARY MASK: 0010000000 */
      double dtp[NS],         /* d2s[NS]/dtp          BINARY MASK: 0100000000 */
      double dp2[NS]          /* d2s[NS]/dp2          BINARY MASK: 1000000000 */
      )
{
  DECLARE_SITE_FRACTIONS
  double tOld         = getTOld();
  double pOld         = getPOld();
  double *rOld        = getROld();
  double *sOld        = getSOld();
  double **d2gds2     = getD2gds2();
  double **ptToD2gds2 = getPtToD2gds2();
  int i, j, iter = 0;

  GET_SITE_FRACTIONS
  
  /* look-up or compute the current ordering state */
  if ( (t != tOld)       || (p != pOld) || 
       (r[0] != rOld[0]) || (r[1] != rOld[1]) || (r[2] != rOld[2]) ) {
    double dgds[NS], sNew[NS];
    for (i=0; i<NS; i++) { sOld[i] = 2.0; sNew[i] = 0.9*r[i]; }
    while ( ((ABS(sNew[0]-sOld[0]) > 10.0*DBL_EPSILON) || 
             (ABS(sNew[1]-sOld[1]) > 10.0*DBL_EPSILON) ||
             (ABS(sNew[2]-sOld[2]) > 10.0*DBL_EPSILON) ) && (iter < MAX_ITER)) {
      double s[NS];

      for (i=0; i<NS; i++) s[i] = sNew[i];

      xfe2A = (r[0] + s[0])/2.0;
      xmg2A = (r[1] + s[1])/2.0;
      xmn2A = (r[2] + s[2])/2.0;
      xti4A = (r[0] - s[0] + r[1] - s[1] + r[2] - s[2])/2.0;
      xfe3A = 1.0 - r[0] - r[1] - r[2];

      xfe2B = (r[0] - s[0])/2.0;
      xmg2B = (r[1] - s[1])/2.0;
      xmn2B = (r[2] - s[2])/2.0;
      xti4B = (r[0] + s[0] + r[1] + s[1] + r[2] + s[2])/2.0;
      xfe3B = 1.0 - r[0] - r[1] - r[2];

      if (xfe2A <= 0.0) xfe2A = DBL_EPSILON; /* added in V1.0-7 */
      if (xmg2A <= 0.0) xmg2A = DBL_EPSILON; /* added in V1.0-7 */
      if (xmn2A <= 0.0) xmn2A = DBL_EPSILON; /* added in V1.0-7 */
      if (xti4A <= 0.0) xti4A = DBL_EPSILON; /* added in V1.0-7 */
      if (xfe3A <= 0.0) xfe3A = DBL_EPSILON; /* added in V1.0-7 */

      if (xfe2B <= 0.0) xfe2B = DBL_EPSILON; /* added in V1.0-7 */
      if (xmg2B <= 0.0) xmg2B = DBL_EPSILON; /* added in V1.0-7 */
      if (xmn2B <= 0.0) xmn2B = DBL_EPSILON; /* added in V1.0-7 */
      if (xti4B <= 0.0) xti4B = DBL_EPSILON; /* added in V1.0-7 */
      if (xfe3B <= 0.0) xfe3B = DBL_EPSILON; /* added in V1.0-7 */

      dgds[0] = DGDS0;
      dgds[1] = DGDS1;
      dgds[2] = DGDS2;

      d2gds2[0][0] = D2GDS0S0;
      d2gds2[0][1] = D2GDS0S1;
      d2gds2[0][2] = D2GDS0S2;
      d2gds2[1][0] = d2gds2[0][1];
      d2gds2[1][1] = D2GDS1S1;
      d2gds2[1][2] = D2GDS1S2;
      d2gds2[2][0] = d2gds2[0][2];
      d2gds2[2][1] = d2gds2[1][2];
      d2gds2[2][2] = D2GDS2S2;

      for (i=0; i<NS; i++) sOld[i] = s[i];

      gaussj(ptToD2gds2, NS, (double **) NULL, 0);

      for (i=0; i<NS; i++) {
        for(j=0; j<NS; j++) s[i] += - d2gds2[i][j]*dgds[j];
        s[i] = MIN(s[i], r[i] - DBL_EPSILON);
        s[i] = MAX(s[i], 0.0);
      }

      for (i=0; i<NS; i++) sNew[i] = s[i];
      iter++;
    }
    tOld = t;
    pOld = p;
    for (i=0; i<NR; i++) rOld[i] = r[i];

#ifdef DEBUG
    for (i=0; i<NS; i++) {
      if (dgds[i] > sqrt(DBL_EPSILON) && ABS(sOld[i]) > DBL_EPSILON) {
        printf(
          "ERROR in RHOMBOHEDRAL.C (function ORDER). Failed to converge!\n");
        printf("  X Il  = %13.6g, X2 Gk = %13.6g, X3 Py = %13.6g\n",
          r[0], r[1], r[2]);
        printf("  s     = %13.6g, t     = %13.6g, u     = %13.6g\n",
          sOld[0], sOld[1], sOld[2]);
        printf("  dgds1 = %13.6g, dgds2 = %13.6g, dgds4 = %13.6g\n",
          dgds[0], dgds[1], dgds[2]);
        printf("  X Fe2+ A: %13.6g  X Fe2+ B: %13.6g\n", xfe2A, xfe2B);
        printf("  X Mg   A: %13.6g  X MG   B: %13.6g\n", xmg2A, xmg2B);
        printf("  X Mn   A: %13.6g  X Mn   B: %13.6g\n", xmn2A, xmn2B);
        printf("  X Ti   A: %13.6g  X Ti   B: %13.6g\n", xti4A, xti4B);
        printf("  X Fe3+ A: %13.6g  X Fe3+ B: %13.6g\n", xfe3A, xfe3B);
        break;
      }
    }
#endif

    setTOld(tOld);
    setPOld(pOld);
    /* arrays (rOld, sOld, d2gds2) should be preserved automatically */

    SET_SITE_FRACTIONS
  }

  if (mask & FIRST  ) {   /* return s        */
    for (i=0; i<NS; i++) s[i] = sOld[i];
  }   
  if (mask & SECOND ) {   /* compute ds/dr:  */
    double *s = sOld;
    double d2gdrds[NR][NS];
    int k;                    

    fillD2GDRDS

    for (i=0; i<NS; i++) {
       for (j=0; j<NR; j++) {
          dr[i][j] = 0.0; 
          for (k=0; k<NS; k++) dr[i][j] += - d2gds2[i][k]*d2gdrds[j][k];
       }
    }
  }
  if (mask & THIRD  ) {   /* compute ds/dt:  */
    double d2gdsdt[NS];

    fillD2GDSDT

    for (i=0; i<NS; i++) {
       dt[i] = 0.0;
       for (j=0; j<NS; j++) dt[i] += - d2gds2[i][j]*d2gdsdt[j];
    }
  }
  if (mask & FOURTH ) {   /* compute ds/dp:  */
    double d2gdsdp[NS];

    fillD2GDSDP

    for (i=0; i<NS; i++) {
       dp[i] = 0.0; 
       for (j=0; j<NS; j++) dp[i] += - d2gds2[i][j]*d2gdsdp[j];
    }
  }
  if (mask & FIFTH  ) {   /* compute d2s/dr2 */
    double *s = sOld;
    double d2gdrds[NR][NS], d3gdr2ds[NR][NR][NS], d3gdrds2[NR][NS][NS],
      d3gds3[NS][NS][NS], dsdr[NS][NR], temp[NS];
    int k, l, m, n;                    

    fillD2GDRDS
    fillD3GDR2DS
    fillD3GDRDS2
    fillD3GDS3

    /* compute dsdr matrix */
    for (i=0; i<NS; i++) {
      for (j=0; j<NR; j++) {
        dsdr[i][j] = 0.0; 
        for (k=0; k<NS; k++) dsdr[i][j] += - d2gds2[i][k]*d2gdrds[j][k];
      }
    }

    /* compute dsdr2 cube */
    for (i=0; i<NS; i++) {
      for (j=0; j<NR; j++) {
        for (k=0; k<NR; k++) {
          for (l=0; l<NS; l++) {
            temp[l] = d3gdr2ds[j][k][l];
            for (m=0; m<NS; m++) {
              temp[l] += d3gdrds2[j][l][m]*dsdr[m][k] 
                       + d3gdrds2[k][l][m]*dsdr[m][j];
              for (n=0; n<NS; n++) 
                temp[l] += d3gds3[l][m][n]*dsdr[m][j]*dsdr[n][k];
             }
          }
          dr2[i][j][k] = 0.0;
          for (l=0; l<NS; l++) dr2[i][j][k] += - d2gds2[i][l]*temp[l];
        }
      }
    }

  }
  if (mask & SIXTH  ) {   /* compute d2s/drt */
    double *s = sOld;
    double d2gdrds[NR][NS], d2gdsdt[NS], d3gdrds2[NR][NS][NS],
      d3gdrdsdt[NR][NS], d3gds3[NS][NS][NS], d3gds2dt[NS][NS], dsdr[NS][NR],
      dsdt[NS], temp[NS];
    int k, l, m;

    fillD2GDRDS
    fillD2GDSDT
    fillD3GDRDS2
    fillD3GDRDSDT
    fillD3GDS3
    fillD3GDS2DT

    /* compute dsdr matrix */
    for (i=0; i<NS; i++) {
      for (j=0; j<NR; j++) {
        dsdr[i][j] = 0.0; 
        for (k=0; k<NS; k++) dsdr[i][j] += - d2gds2[i][k]*d2gdrds[j][k];
      }
    }

    /* compute dsdt vector */
    for (i=0; i<NS; i++) {
      dsdt[i] = 0.0;
      for (j=0; j<NS; j++) dsdt[i] += - d2gds2[i][j]*d2gdsdt[j];
    }

    /* compute dsdrdt matrix */
    for (i=0; i<NS; i++) {
      for (j=0; j<NR; j++) {
        for (k=0; k<NS; k++) {
          temp[k] = d3gdrdsdt[j][k];
          for (l=0; l<NS; l++) {
             temp[k] += d3gdrds2[j][k][l]*dsdt[l] + d3gds2dt[k][l]*dsdr[l][j];
             for (m=0; m<NS; m++) temp[k] += d3gds3[k][l][m]*dsdr[l][j]*dsdt[m];
          }
        }
        drt[i][j] = 0.0;
        for (k=0; k<NS; k++) drt[i][j] += - d2gds2[i][k]*temp[k];
      }
    }

  }
  if (mask & SEVENTH) {   /* compute d2s/drp */
    double *s = sOld;
    double d2gdrds[NR][NS], d2gdsdp[NS], d3gdrds2[NR][NS][NS],
      d3gdrdsdp[NR][NS], d3gds3[NS][NS][NS], d3gds2dp[NS][NS], dsdr[NS][NR],
      dsdp[NS], temp[NS];
    int k, l, m;

    fillD2GDRDS
    fillD2GDSDP
    fillD3GDRDS2
    fillD3GDRDSDP
    fillD3GDS3
    fillD3GDS2DP

    /* compute dsdr matrix */
    for (i=0; i<NS; i++) {
      for (j=0; j<NR; j++) {
        dsdr[i][j] = 0.0; 
        for (k=0; k<NS; k++) dsdr[i][j] += - d2gds2[i][k]*d2gdrds[j][k];
      }
    }

    /* compute dsdp vector */
    for (i=0; i<NS; i++) {
      dsdp[i] = 0.0;
      for (j=0; j<NS; j++) dsdp[i] += - d2gds2[i][j]*d2gdsdp[j];
    }

    /* compute dsdrdp matrix */
    for (i=0; i<NS; i++) {
      for (j=0; j<NR; j++) {
        for (k=0; k<NS; k++) {
          temp[k] = d3gdrdsdp[j][k];
          for (l=0; l<NS; l++) {
             temp[k] += d3gdrds2[j][k][l]*dsdp[l] + d3gds2dp[k][l]*dsdr[l][j];
             for (m=0; m<NS; m++) temp[k] += d3gds3[k][l][m]*dsdr[l][j]*dsdp[m];
          }
        }
        drp[i][j] = 0.0;
        for (k=0; k<NS; k++) drp[i][j] += - d2gds2[i][k]*temp[k];
      }
    }

  }
  if (mask & EIGHTH ) {   /* compute d2s/dt2 */
    double d2gdsdt[NS], d3gds3[NS][NS][NS], d3gds2dt[NS][NS], d3gdsdt2[NS],
      dsdt[NS], temp[NS];
     int k, l;

    fillD2GDSDT
    fillD3GDS3
    fillD3GDS2DT
    fillD3GDSDT2

   /* compute dsdt vector */
    for (i=0; i<NS; i++) {
      dsdt[i] = 0.0;
      for (j=0; j<NS; j++) dsdt[i] += - d2gds2[i][j]*d2gdsdt[j];
    }

    /* compute dsdt2 vector */
    for (i=0; i<NS; i++) {
      for (j=0; j<NS; j++) { 
        temp[j] = d3gdsdt2[j];
        for (k=0; k<NS; k++) {
          temp[j] +=  2.0*d3gds2dt[j][k]*dsdt[k];
          for (l=0; l<NS; l++) temp[j] += d3gds3[j][k][l]*dsdt[k]*dsdt[l];
        }
      }
      dt2[i] = 0.0;
      for (j=0; j<NS; j++) dt2[i] += - d2gds2[i][j]*temp[j];
    } 

  }
  if (mask & NINTH  ) {   /* compute d2s/dtp */
    double d2gdsdt[NS], d2gdsdp[NS], d3gds3[NS][NS][NS], d3gds2dt[NS][NS],
      d3gds2dp[NS][NS], d3gdsdtdp[NS], dsdt[NS], dsdp[NS], temp[NS];
    int k, l;

    fillD2GDSDT
    fillD2GDSDP
    fillD3GDS3
    fillD3GDS2DT
    fillD3GDS2DP
    fillD3GDSDTDP

    /* compute dsdt vector */
    for (i=0; i<NS; i++) {
      dsdt[i] = 0.0;
      for (j=0; j<NS; j++) dsdt[i] += - d2gds2[i][j]*d2gdsdt[j];
    }

    /* compute dsdp vector */
    for (i=0; i<NS; i++) {
      dsdp[i] = 0.0;
      for (j=0; j<NS; j++) dsdp[i] += - d2gds2[i][j]*d2gdsdp[j];
    }

    /* compute dsdtp vector */
    for (i=0; i<NS; i++) {
      for (j=0; j<NS; j++) {
        temp[j] = d3gdsdtdp[j];
        for (k=0; k<NS; k++) {
          temp[j] += d3gds2dt[j][k]*dsdp[k] + d3gds2dp[j][k]*dsdt[k];
          for (l=0; l<NS; l++) temp[j] += d3gds3[j][k][l]*dsdt[k]*dsdp[l];
        }
      }
      dtp[i] = 0.0;
      for (j=0; j<NS; j++) dtp[i] += - d2gds2[i][j]*temp[j];
    }

  }
  if (mask & TENTH  ) {   /* compute d2s/dp2 */
    double d2gdsdp[NS], d3gds3[NS][NS][NS], d3gds2dp[NS][NS], d3gdsdp2[NS],
      dsdp[NS], temp[NS];
    int k, l;

    fillD2GDSDP
    fillD3GDS3
    fillD3GDS2DP
    fillD3GDSDP2

    /* compute dsdp vector */
    for (i=0; i<NS; i++) {
      dsdp[i] = 0.0;
      for (j=0; j<NS; j++) dsdp[i] += - d2gds2[i][j]*d2gdsdp[j];
    }

    /* compute dsdp2 vector */
    for (i=0; i<NS; i++) {
      for (j=0; j<NS; j++) { 
        temp[j] = d3gdsdp2[j];
        for (k=0; k<NS; k++) {
          temp[j] +=  2.0*d3gds2dp[j][k]*dsdp[k];
          for (l=0; l<NS; l++) temp[j] += d3gds3[j][k][l]*dsdp[k]*dsdp[l];
        }
      }
      dp2[i] = 0.0;
      for (j=0; j<NS; j++) dp2[i] += - d2gds2[i][j]*temp[j];
    } 

  }

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
testRhm(int mask, double t, double p,
  int na,          /* Expected number of endmember components                 */
  int nr,          /* Expected number of independent compositional variables  */
  char **names,    /* array of strings of names of endmember components       */
  char **formulas, /* array of strings of formulas of endmember components    */
  double *r,       /* array of indepependent compos variables, check bounds   */
  double *m)       /* array of moles of endmember components, check bounds    */
{
  const char *phase = "rhombohedral.c";
  const char *NAMES[NA]    = { "geikielite", "hematite", "ilmenite", "pyrophanite" };
  const char *FORMULAS[NA] = { "MgTiO3", "Fe2O3", "FeTiO3", "MnTiO3" };
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
conRhm(int inpMask, int outMask, double t, double p,
  double *e,      /* comp of rhm oxides in moles of elements                  */
  double *m,      /* comp of rhm oxides in moles of endmember components      */
  double *r,      /* comp of rhm oxides in terms of the independent comp var  */
  double *x,      /* comp of rhm oxides in mole fractions of endmember comp   */
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
  (2)  SECOND           THIRD | FOURTH | FIFTH | SIXTH | EIGHTH
  (3)  THIRD            FOURTH | SEVENTH

  (1) converts a vector of moles of elements into a vector of moles of
      endmember rhm oxides components.
  (2) calculates from a vector of moles of endmember components, one or
      all of: r[], x[], dr[]/dm[] d2r[]/dm[]dm[], or d3r[]/dm[]dm[]dm[]
  (3) calculates from a vector of independent compositional variables
      mole fractions of endmember components and/or the Jacobian matrix
      dx[]/dr[]

  In this routine it is assumed that the elements are in the order of atomic
  numbers and that the order of rhm oxides components has been verified as:
      m[0] = geikielite  (MgTiO3) ,
      m[1] = hematite    (Fe2O3), 
      m[2] = ilmenite    (FeTiO3),
      m[3] = pyrophanite (MnTiO3),

  ----------------------------------------------------------------------------*/

  int i, j, k;

  if (inpMask == FIRST && outMask == SECOND) {
    /* Converts a vector of moles of elements into a vector of moles of
       end-member components.                                                 */
    double sumcat, sumchg, fe2, fe3;
    static const int Mg = 12;
    static const int Al = 13;
    static const int Ti = 22;
    static const int Cr = 24;
    static const int Mn = 25;
    static const int Fe = 26;
    static const int Co = 27;
    static const int Ni = 28;

    sumcat = e[Mg] + e[Al] + e[Ti] + e[Cr] + e[Mn] + e[Fe] + e[Co] + e[Ni];

    sumchg = 2.0*e[Mg] + 3.0*e[Al] + 4.0*e[Ti] + 3.0*e[Cr] + 2.0*e[Mn]
           + 2.0*e[Co] + 2.0*e[Ni];
    fe3 = 6.0*sumcat/2.0 - sumchg - 2.0*e[Fe];
    fe2 = e[Fe] - fe3;

    m[0] = e[Mg];                     /* Moles of MgTiO3 */
    m[1] = (fe3 + e[Al] + e[Cr])/2.0; /* Moles of Fe2O3  */
    m[2] = fe2;                       /* Moles of FeTiO3 */
    m[3] = e[Mn] + e[Co] + e[Ni];     /* Moles of MnTiO3 */

    if (m[1] < 0.0) m[1] = 0.0;

   } else if (inpMask == SECOND) {
    double sum;

    if (outMask & ~(THIRD | FOURTH | FIFTH | SIXTH | EIGHTH))
      printf("Illegal call to conRhm with inpMask = %o and outMask = %o\n",
        inpMask, outMask);
 
    for (i=0, sum=0.0; i<NA; i++) sum += m[i];

    if (outMask & THIRD) {
      /* Converts a vector of moles of end-member components (m) into a vector 
         of independent compositional variables (r) required as input for the 
         remaining public functions.                                          */
      r[0] = (sum != 0.0) ? m[2]/sum : 0.0;  /* Xil = X FeTiO3 */ 
      r[1] = (sum != 0.0) ? m[0]/sum : 0.0;  /* Xgk = X MgTiO3 */
      r[2] = (sum != 0.0) ? m[3]/sum : 0.0;  /* Xpy = X MnTiO3 */
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
        for (j=0; j<NA; j++) {
          dm[0][j] = (j == 2) ? (1.0-m[2]/sum)/sum : -m[2]/SQUARE(sum);
          dm[1][j] = (j == 0) ? (1.0-m[0]/sum)/sum : -m[0]/SQUARE(sum);
          dm[2][j] = (j == 3) ? (1.0-m[3]/sum)/sum : -m[3]/SQUARE(sum);
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
        for (j=0; j<NA; j++) {
          for (k=0; k<NA; k++) {
            d2m[0][j][k]  = 2.0*m[2]/CUBE(sum);
            d2m[0][j][k] -= (j == 2) ? 1.0/SQUARE(sum) : 0.0; 
            d2m[0][j][k] -= (k == 2) ? 1.0/SQUARE(sum) : 0.0; 
            d2m[1][j][k]  = 2.0*m[0]/CUBE(sum);
            d2m[1][j][k] -= (j == 0) ? 1.0/SQUARE(sum) : 0.0; 
            d2m[1][j][k] -= (k == 0) ? 1.0/SQUARE(sum) : 0.0; 
            d2m[2][j][k]  = 2.0*m[3]/CUBE(sum);
            d2m[2][j][k] -= (j == 3) ? 1.0/SQUARE(sum) : 0.0; 
            d2m[2][j][k] -= (k == 3) ? 1.0/SQUARE(sum) : 0.0; 
          }
        }
      }
    }

    if (outMask & EIGHTH) {
      /* calculates the matrix d3r[i]/dm[j]dm[k]dm[l] using m[] as input        */
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
        for (j=0; j<NA; j++)  {
          for (k=0; k<NA; k++)  {
            for (l=0; l<NA; l++)  {
              d3m[0][j][k][l]  = -6.0*m[2]/QUARTIC(sum);
              d3m[0][j][k][l] += (j == 2) ? 2.0/CUBE(sum) : 0.0;
              d3m[0][j][k][l] += (k == 2) ? 2.0/CUBE(sum) : 0.0;
              d3m[0][j][k][l] += (l == 2) ? 2.0/CUBE(sum) : 0.0;
              d3m[1][j][k][l]  = -6.0*m[0]/QUARTIC(sum);
              d3m[1][j][k][l] += (j == 0) ? 2.0/CUBE(sum) : 0.0;
              d3m[1][j][k][l] += (k == 0) ? 2.0/CUBE(sum) : 0.0;
              d3m[1][j][k][l] += (l == 0) ? 2.0/CUBE(sum) : 0.0;
              d3m[2][j][k][l]  = -6.0*m[3]/QUARTIC(sum);
              d3m[2][j][k][l] += (j == 3) ? 2.0/CUBE(sum) : 0.0;
              d3m[2][j][k][l] += (k == 3) ? 2.0/CUBE(sum) : 0.0;
              d3m[2][j][k][l] += (l == 3) ? 2.0/CUBE(sum) : 0.0;
	    }
          }
        }
      }
    }

  } else if (inpMask == THIRD) {

    if (outMask & ~(FOURTH | SEVENTH))
      printf("Illegal call to conRhm with inpMask = %o and outMask = %o\n",
        inpMask, outMask);

    if (outMask & FOURTH) {
      /* Converts a vector of independent compositional variables (r) into a
         vector of mole fractions of endmember components (x).                */
      x[0] = r[1]; 
      x[1] = 1.0 - r[0] - r[1] - r[2];
      x[2] = r[0]; 
      x[3] = r[2]; 
    }

    if (outMask & SEVENTH) {
      /* computes the Jacobian matrix dr[i][j] = dx[i]/dr[j] */
      for (i=0; i<NA; i++) for (j=0; j<NR; j++) dr[i][j] = 0.0;
      dr[0][1] =  1.0;
      dr[1][0] = -1.0; dr[1][1] = -1.0; dr[1][2] = -1.0;
      dr[2][0] = 1.0;
      dr[3][2] = 1.0;
    }

  } else  {
    printf("Illegal call to conRhm with inpMask = %o and outMask = %o\n",
      inpMask, outMask);
  }

}

void
dispRhm(int mask, double t, double p, double *x,
  char **formula            /* Mineral formula for interface display MASK: 1 */
  )
{
  double *r = x;
  static char masterString[] = {
/*             1111111111222222222233333333334444444444555555555566666666667
     01234567890123456789012345678901234567890123456789012345678901234567890 */
    "Mn_.__Fe''_.__Mg_.__Fe'''_.__Ti_.__O3" };

  if (mask & FIRST) {
    char *string = strcpy((char *) malloc((size_t) (strlen(masterString)+1)*sizeof(char)), masterString);
    double totMn, totFe2, totMg, totFe3, totTi;
    char n[5];
    int i;

    totMn  = r[2];
    totFe2 = r[0];
    totMg  = r[1];
    totFe3 = 2.0*(1.0-r[0]-r[1]-r[2]);
    totTi  = r[0]+r[1]+r[2];

    (void) snprintf(n, 5, "%4.2f", totMn);  for (i=0; i<4; i++) string[ 2+i] = n[i];
    (void) snprintf(n, 5, "%4.2f", totFe2); for (i=0; i<4; i++) string[10+i] = n[i];
    (void) snprintf(n, 5, "%4.2f", totMg);  for (i=0; i<4; i++) string[16+i] = n[i];
    (void) snprintf(n, 5, "%4.2f", totFe3); for (i=0; i<4; i++) string[25+i] = n[i];
    (void) snprintf(n, 5, "%4.2f", totTi);  for (i=0; i<4; i++) string[31+i] = n[i];
 
    *formula = string;
  }
}

void 
actRhm(int mask, double t, double p, double *x, 
  double *a,  /* (pointer to a[]) activities              BINARY MASK: 0001 */
  double *mu, /* (pointer to mu[]) chemical potentials    BINARY MASK: 0010 */
  double **dx /* (pointer to dx[][]) d(a[])/d(x[])        BINARY MASK: 0100 */
  )           /* exclusion criteria applied to results if BINARY MASK: 1000 */
{
  DECLARE_SITE_FRACTIONS
  double *r = x;
  double s[NS], g, dgdr[NR];
  double fr[NA][NR];
  int i, j;
  
  for(i=0; i<NA; i++) {
     fr[i][0] = FR0(i); /* Xil */
     fr[i][1] = FR1(i); /* Xgk */
     fr[i][2] = FR2(i); /* Xpy */
  }

  order(FIRST, t, p, r, 
        s, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
  GET_SITE_FRACTIONS

  g       = G;
  dgdr[0] = DGDR0;          
  dgdr[1] = DGDR1;
  dgdr[2] = DGDR2;

  if (mask & FIRST) {
    double a0[NA];

    pureRhm(FIRST, t, p, 
       a0,              (double *) NULL, (double *) NULL, (double *) NULL,
       (double *) NULL, (double *) NULL, (double *) NULL, (double *) NULL,
       (double *) NULL, (double *) NULL, (double *) NULL, (double *) NULL, 
       (double *) NULL);

    for(i=0; i<NA; i++) {
       for (a[i]=g, j=0; j<NR; j++) a[i] += fr[i][j]*dgdr[j];
       a[i] = exp(a[i]/(R*t));
       if (a0[i] != 0.0) a[i] = a[i]/a0[i];
    }

  }

  if (mask & SECOND) {
    double mu0[NA];

    pureRhm(SECOND, t, p, 
       (double *) NULL, mu0,             (double *) NULL, (double *) NULL,
       (double *) NULL, (double *) NULL, (double *) NULL, (double *) NULL,
       (double *) NULL, (double *) NULL, (double *) NULL, (double *) NULL, 
       (double *) NULL);

    for(i=0; i<NA; i++) {
       for (mu[i]=g-mu0[i], j=0; j<NR; j++) mu[i] += fr[i][j]*dgdr[j];
    }

  }

  if (mask & THIRD) {
    double d2gdr2[NR][NR], d2gdrds[NR][NS], d2gds2[NS][NS], dsdr[NS][NR],
      dfrdr[NA][NR], gs[NA][NS], dgsds[NA][NS], sum, a0[NA];
    int k, l;

    fillD2GDR2
    fillD2GDRDS
    fillD2GDS2

    pureRhm(FIRST, t, p, 
       a0,              (double *) NULL, (double *) NULL, (double *) NULL,
       (double *) NULL, (double *) NULL, (double *) NULL, (double *) NULL,
       (double *) NULL, (double *) NULL, (double *) NULL, (double *) NULL, 
       (double *) NULL);

    for(i=0; i<NA; i++) {
       gs[i][0] = GS0(i);        /* s   */
       gs[i][1] = GS1(i);        /* t   */
       gs[i][2] = GS2(i);        /* u   */
       dfrdr[i][0] = DFR0DR0(i); /* Xil */
       dfrdr[i][1] = DFR1DR1(i); /* Xgk */
       dfrdr[i][2] = DFR2DR2(i); /* Xpy */
       dgsds[i][0] = DGS0DS0(i); /* s   */
       dgsds[i][1] = DGS1DS1(i); /* t   */
       dgsds[i][2] = DGS2DS2(i); /* u   */
    }

    order(SECOND, t, p, r, 
          NULL, dsdr, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

    for (i=0; i<NA; i++) {
      for (k=0; k<NR; k++) {
        /* compute activity of the i-th component */
        for (dx[i][k]=g, j=0; j<NR; j++) dx[i][k] += fr[i][j]*dgdr[j];
        dx[i][k] = exp(dx[i][k]/(R*t));
        if (a0[i] != 0.0) dx[i][k]  = dx[i][k]/a0[i];

        /* compute derivative of i-th activity with respect to r(k) */
        sum = (1.0+dfrdr[i][k])*dgdr[k];
        for (j=0; j<NR; j++) {
          sum += fr[i][j]*d2gdr2[j][k];
          for (l=0; l<NS; l++) sum += fr[i][j]*d2gdrds[j][l]*dsdr[l][k];
        }
        for (j=0; j<NS; j++) {
          sum += gs[i][j]*d2gdrds[k][j];
          for (l=0; l<NS; l++) sum += gs[i][j]*d2gds2[j][l]*dsdr[l][k];
        }
        dx[i][k] *= sum/(R*t);
      }
    }

  }

  if (mask & FOURTH) {
    /* implement exclusion criteria on quantities for preclb routines   */
    static const double exclusion[NA] = {
       0.05,  /* exclusion criteria on the mole fraction of geikielite  */
       0.05,  /* exclusion criteria on the mole fraction of hematite    */
       0.05,  /* exclusion criteria on the mole fraction of ilmenite    */
       0.05   /* exclusion criteria on the mole fraction of pyrophanite */
    };
    double x[NA];

    x[0] = r[1];                     /* mole fraction of geikielite     */
    x[1] = 1.0 - r[0] - r[1] - r[2]; /* mole fraction of hematite       */
    x[2] = r[0];                     /* mole fraction of ilmenite       */
    x[3] = r[2];                     /* mole fraction of pyrophanite    */

    for (i=0; i<NA; i++) {
      if (x[i] < exclusion[i]) {
        if (mask & FIRST)  a[i]  = 0.0;
        if (mask & SECOND) mu[i] = 0.0;
        if (mask & THIRD)  for (j=0; j<NR; j++) dx[i][j] = 0.0;
      }
    }
  }
}

void 
gmixRhm(int mask, double t, double p, double *x, 
  double *gmix, /* Gibbs energy of mixing             BINARY MASK: 0001 */
  double *dx,   /* (pointer to dx[]) d(g)/d(x[])      BINARY MASK: 0010 */
  double **dx2, /* (pointer to dx2[][]) d2(g)/d(x[])2 BINARY MASK: 0100 */
  double ***dx3 /* (pointer to dx3[][][]) d3(g)/d(x[])3 NARY MASK: 1000 */
  )
{
  DECLARE_SITE_FRACTIONS
  double *r = x;
  double s[NS];
  
  order(FIRST, t, p, r, 
        s, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
  GET_SITE_FRACTIONS

  if (mask & FIRST) {
    double ends[NA];

    *gmix = G;

    pureRhm(THIRD, t, p, 
       (double *) NULL, (double *) NULL, ends,            (double *) NULL,
       (double *) NULL, (double *) NULL, (double *) NULL, (double *) NULL,
       (double *) NULL, (double *) NULL, (double *) NULL, (double *) NULL, 
       (double *) NULL);

    *gmix -= ENDMEMBERS;
  }
  
  if(mask & SECOND) {
    double ends[NA];

    dx[0] = DGDR0;
    dx[1] = DGDR1;
    dx[2] = DGDR2;

    pureRhm(THIRD, t, p, 
       (double *) NULL, (double *) NULL, ends,           (double *) NULL,
       (double *) NULL, (double *) NULL, (double *) NULL, (double *) NULL,
       (double *) NULL, (double *) NULL, (double *) NULL, (double *) NULL, 
       (double *) NULL);

    dx[0] -= DENDDR0;
    dx[1] -= DENDDR1;
    dx[2] -= DENDDR2;
  }

  if(mask & THIRD) {
    double d2gdr2[NR][NR], d2gdrds[NR][NS], d2gds2[NS][NS], dsdr[NS][NR];
    int i, j, k, l;

    fillD2GDR2
    fillD2GDRDS
    fillD2GDS2

    order(SECOND, t, p, r, 
          NULL, dsdr, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

    for (i=0; i<NR; i++) {
      for (j=0; j<NR; j++) {
        dx2[i][j] = d2gdr2[i][j];
        for (k=0; k<NS; k++) {
          dx2[i][j] += d2gdrds[i][k]*dsdr[k][j] + d2gdrds[j][k]*dsdr[k][i]; 
          for (l=0; l<NS; l++) dx2[i][j] += d2gds2[k][l]*dsdr[k][i]*dsdr[l][j];
        }
      }
    }

  }

  if(mask & FOURTH) {
    double d3gdr3[NR][NR][NR], d3gdr2ds[NR][NR][NS], d3gdrds2[NR][NS][NS];
    double d3gds3[NS][NS][NS], dsdr[NS][NR];
    int i, j, k, l, m, n;

    fillD3GDR3
    fillD3GDR2DS
    fillD3GDRDS2
    fillD3GDS3

    order(SECOND, t, p, r, 
          NULL, dsdr, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

    for (i=0; i<NR; i++) {
      for (j=0; j<NR; j++) {
        for (k=0; k<NR; k++) {
          dx3[i][j][k] = d3gdr3[i][j][k];
          for (l=0; l<NS; l++) {
            dx3[i][j][k] += d3gdr2ds[i][j][l]*dsdr[l][k] +
              d3gdr2ds[j][k][l]*dsdr[l][i] + d3gdr2ds[k][i][l]*dsdr[l][j];
            for (m=0; m<NS; m++) {
              dx3[i][j][k] += 
                d3gdrds2[i][l][m]*dsdr[l][j]*dsdr[m][k] +
                d3gdrds2[j][l][m]*dsdr[l][k]*dsdr[m][i] +
                d3gdrds2[k][l][m]*dsdr[l][i]*dsdr[m][j];
              for (n=0; n<NS; n++)
                dx3[i][j][k] +=
                  d3gds3[l][m][n]*dsdr[l][i]*dsdr[m][j]*dsdr[n][k];
            }
          }
        }
      }
    }
  }

}

void 
hmixRhm(int mask, double t, double p, double *x, 
  double *hmix /* Enthalpy of mixing BINARY MASK: 1 */
  )
{
  DECLARE_SITE_FRACTIONS
  double *r = x;
  double s[NS], ends[NA];
  
  order(FIRST, t, p, r, 
        s, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
  GET_SITE_FRACTIONS

  *hmix = (G) + t*(S);

  pureRhm(FOURTH, t, p, 
       (double *) NULL, (double *) NULL, (double *) NULL, ends,
       (double *) NULL, (double *) NULL, (double *) NULL, (double *) NULL,
       (double *) NULL, (double *) NULL, (double *) NULL, (double *) NULL, 
       (double *) NULL);

  *hmix -= ENDMEMBERS;
}

void 
smixRhm(int mask, double t, double p, double *x, 
  double *smix, /* Entropy of mixing                  BINARY MASK: 001 */
  double *dx,   /* (pointer to dx[]) d(s)/d(x[])      BINARY MASK: 010 */
  double **dx2  /* (pointer to dx2[][]) d2(s)/d(x[])2 BINARY MASK: 100 */
  )
{
  DECLARE_SITE_FRACTIONS
  double *r = x;
  double s[NS];

  order(FIRST, t, p, r, 
        s, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
  GET_SITE_FRACTIONS

  if (mask & FIRST) {
    double ends[NA];

    *smix = S; 

    pureRhm(FIFTH, t, p, 
       (double *) NULL, (double *) NULL, (double *) NULL, (double *) NULL,
       ends,            (double *) NULL, (double *) NULL, (double *) NULL,
       (double *) NULL, (double *) NULL, (double *) NULL, (double *) NULL, 
       (double *) NULL);

    *smix -= ENDMEMBERS;
  }
  
  if(mask & SECOND) {
    double d2gdrds[NR][NS], d2gdrdt[NR], d2gds2[NS][NS], d2gdsdt[NS],
      dsdr[NS][NR], dsdt[NS], ends[NA];
    int i, k, l;

    fillD2GDRDS
    fillD2GDRDT
    fillD2GDS2
    fillD2GDSDT

    order(SECOND | THIRD, t, p, r, 
          NULL, dsdr, dsdt, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    
    for (i=0; i<NR; i++) {
      dx[i] = d2gdrdt[i];
      for (k=0; k<NS; k++) {
        dx[i] += d2gdrds[i][k]*dsdt[k] + d2gdsdt[k]*dsdr[k][i];
        for (l=0; l<NS; l++) dx[i] += d2gds2[k][l]*dsdt[k]*dsdr[l][i] ;
      }
      dx[i] *= -1.0;
    }

    pureRhm(FIFTH, t, p, 
       (double *) NULL, (double *) NULL, (double *) NULL, (double *) NULL,
       ends,            (double *) NULL, (double *) NULL, (double *) NULL,
       (double *) NULL, (double *) NULL, (double *) NULL, (double *) NULL, 
       (double *) NULL);

    dx[0] -= DENDDR0;
    dx[1] -= DENDDR1;
    dx[2] -= DENDDR2;
  }

  if(mask & THIRD) {
    double d2gdrds[NR][NS], d2gds2[NS][NS], d2gdsdt[NS], d3gdr2ds[NR][NR][NS],
      d3gdr2dt[NR][NR], d3gdrds2[NR][NS][NS], d3gdrdsdt[NR][NS],
      d3gds3[NS][NS][NS], d3gds2dt[NS][NS], dsdr[NS][NR], dsdt[NS],
      d2sdr2[NS][NR][NR], d2sdrdt[NS][NR];
    int i, j, k, l, m;

    fillD2GDRDS
    fillD2GDS2
    fillD2GDSDT
    fillD3GDR2DS
    fillD3GDR2DT
    fillD3GDRDS2
    fillD3GDRDSDT
    fillD3GDS3
    fillD3GDS2DT

    order(SECOND | THIRD | FIFTH | SIXTH, t, p, r, 
          NULL, dsdr, dsdt, NULL, d2sdr2, d2sdrdt, NULL, NULL, NULL, NULL);

    for (i=0; i<NR; i++) {
      for (j=0; j<NR; j++) { 
        dx2[i][j] = d3gdr2dt[i][j];
        for (k=0; k<NS; k++) {
          dx2[i][j] += d3gdr2ds[i][j][k]*dsdt[k] 
                     + d3gdrdsdt[i][k]*dsdr[k][j] 
                     + d3gdrdsdt[j][k]*dsdr[k][i] 
                     + d2gdsdt[k]*d2sdr2[k][i][j]
                     + d2gdrds[i][k]*d2sdrdt[k][j] 
                     + d2gdrds[j][k]*d2sdrdt[k][i];
          for (l=0; l<NS; l++) {
            dx2[i][j] += d3gdrds2[i][k][l]*dsdr[k][j]*dsdt[l]
                       + d3gdrds2[j][k][l]*dsdr[k][i]*dsdt[l]
                       + d2gds2[k][l]*d2sdr2[k][i][j]*dsdt[l] 
                       + d3gds2dt[k][l]*dsdr[k][i]*dsdr[l][j]
                       + d2gds2[k][l]*dsdr[k][i]*d2sdrdt[l][j] 
                       + d2gds2[k][l]*dsdr[k][j]*d2sdrdt[l][i];
            for (m=0; m<NS; m++) 
              dx2[i][j] += d3gds3[k][l][m]*dsdr[k][i]*dsdr[l][j]*dsdt[m];
          }
        }
        dx2[i][j] *= -1.0;
      }
    }

  }

}

void 
cpmixRhm(int mask, double t, double p, double *x, 
  double *cpmix, /* Heat capacity of mixing               BINARY MASK: 001 */
  double *dt,    /* d(cp)/d(t)                            BINARY MASK: 010 */
  double *dx     /* d(cp)/d(x[])d(t)                      BINARY MASK: 100 */
  )
{
  DECLARE_SITE_FRACTIONS
  double *r = x;
  double s[NS], dsdt[NS], d2gdsdt[NS], d2gds2[NS][NS], d2gdt2;
  int i, j;

  order(FIRST | THIRD, t, p, r, 
        s, NULL, dsdt, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
  GET_SITE_FRACTIONS

  fillD2GDS2
  fillD2GDSDT
  d2gdt2  = D2GDT2;

  if (mask & FIRST) {
    double ends[NA];

    *cpmix = d2gdt2;
    for (i=0; i<NS; i++) {
      *cpmix += 2.0*d2gdsdt[i]*dsdt[i];
      for (j=0; j<NS; j++) *cpmix += d2gds2[i][j]*dsdt[i]*dsdt[j]; 
    }
    *cpmix *= -t;

    pureRhm(SIXTH, t, p, 
       (double *) NULL, (double *) NULL, (double *) NULL, (double *) NULL,
       (double *) NULL, ends,            (double *) NULL, (double *) NULL,
       (double *) NULL, (double *) NULL, (double *) NULL, (double *) NULL, 
       (double *) NULL);

    *cpmix -= ENDMEMBERS;
  }

  if(mask & SECOND) {
    double d3gds3[NS][NS][NS], d3gds2dt[NS][NS], d3gdsdt2[NS], d2sdt2[NS],
      temp, ends[NA];
    double d3gdt3 = D3GDT3;
    int k;

    fillD3GDS3
    fillD3GDS2DT
    fillD3GDSDT2

    order(EIGHTH, t, p, r, 
          NULL, NULL, NULL, NULL, NULL, NULL, NULL, d2sdt2, NULL, NULL);

    /* compute d2gdt2 */
    temp = d2gdt2;
    for (i=0; i<NS; i++) {
      temp += 2.0*d2gdsdt[i]*dsdt[i];
      for (j=0; j<NS; j++) temp += d2gds2[i][j]*dsdt[i]*dsdt[j]; 
    }

    *dt = d3gdt3;
    for (i=0; i<NS; i++) {
      *dt += 3.0*d3gdsdt2[i]*dsdt[i] + 3.0*d2gdsdt[i]*d2sdt2[i]; 
      for (j=0; j<NS; j++) {
        *dt += 3.0*d2gds2[i][j]*dsdt[i]*d2sdt2[j] 
             + 3.0*d3gds2dt[i][j]*dsdt[i]*dsdt[j];
        for (k=0; k<NS; k++) *dt += d3gds3[i][j][k]*dsdt[i]*dsdt[j]*dsdt[k]; 
      }
    }
    *dt = -t*(*dt) - temp;

    pureRhm(SEVENTH, t, p, 
       (double *) NULL, (double *) NULL, (double *) NULL, (double *) NULL,
       (double *) NULL, (double *) NULL, ends,            (double *) NULL,
       (double *) NULL, (double *) NULL, (double *) NULL, (double *) NULL, 
       (double *) NULL);

    *dt -= ENDMEMBERS;
  }

  if(mask & THIRD) {
    double d3gds3[NS][NS][NS], d3gdrds2[NR][NS][NS], d3gdrdsdt[NR][NS], 
      d3gds2dt[NS][NS], d2gdrds[NR][NS], d3gdrdt2[NR], d3gdsdt2[NS],
      dsdr[NS][NR], d2sdrdt[NS][NR], d2sdt2[NS], ends[NA];
    int k, l;

    fillD2GDRDS
    fillD3GDRDS2
    fillD3GDRDSDT
    fillD3GDRDT2
    fillD3GDS3
    fillD3GDS2DT
    fillD3GDSDT2

    order(SECOND | SIXTH | EIGHTH, t, p, r, 
          NULL, dsdr, NULL, NULL, NULL, d2sdrdt, NULL, d2sdt2, NULL, NULL);

    for (i=0; i<NR; i++) {
      for (j=0,dx[i]=d3gdrdt2[i]; j<NS; j++) {
        dx[i] += d3gdsdt2[j]*dsdr[j][i] + 2.0*d2gdsdt[j]*d2sdrdt[j][i] +
                 2.0*d3gdrdsdt[i][j]*dsdt[j] + d2gdrds[i][j]*d2sdt2[j];
        for (k=0; k<NS; k++) {
          dx[i] += d3gdrds2[i][j][k]*dsdt[j]*dsdt[k] + 
                   2.0*d2gds2[j][k]*dsdt[j]*d2sdrdt[k][i] +
                   2.0*d3gds2dt[j][k]*dsdr[j][i]*dsdt[k] +
                   d2gds2[j][k]*dsdr[j][i]*d2sdt2[k];
          for (l=0; l<NS; l++) 
            dx[i] += d3gds3[j][k][l]*dsdr[j][i]*dsdt[k]*dsdt[l];
        }
      }
      dx[i] *= -t;
    }

    pureRhm(SIXTH, t, p, 
       (double *) NULL, (double *) NULL, (double *) NULL, (double *) NULL,
       (double *) NULL, ends,            (double *) NULL, (double *) NULL,
       (double *) NULL, (double *) NULL, (double *) NULL, (double *) NULL, 
       (double *) NULL);

    dx[0] -= DENDDR0;
    dx[1] -= DENDDR1;
    dx[2] -= DENDDR2;
  }

}

void 
vmixRhm(int mask, double t, double p, double *x, 
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
  DECLARE_SITE_FRACTIONS
  double *r = x;
  double s[NS];
  
  order(FIRST, t, p, r, 
        s, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
  GET_SITE_FRACTIONS

  if (mask & FIRST) {
    double ends[NA];

    *vmix = DGDP;

    pureRhm(EIGHTH, t, p, 
       (double *) NULL, (double *) NULL, (double *) NULL, (double *) NULL,
       (double *) NULL, (double *) NULL, (double *) NULL, ends,           
       (double *) NULL, (double *) NULL, (double *) NULL, (double *) NULL, 
       (double *) NULL);

    *vmix -= ENDMEMBERS;
  }

  if(mask & SECOND) {
    double d2gdrds[NR][NS], d2gdrdp[NR], d2gds2[NS][NS], d2gdsdp[NS],
      dsdr[NS][NR], dsdp[NS], ends[NA];
    int i, j, k;

    fillD2GDRDS
    fillD2GDRDP
    fillD2GDS2
    fillD2GDSDP

     order(SECOND | FOURTH, t, p, r, 
           NULL, dsdr, NULL, dsdp, NULL, NULL, NULL, NULL, NULL, NULL);

    for (i=0; i<NR; i++) {
      dx[i] = d2gdrdp[i];
      for (j=0; j<NS; j++) {
        dx[i] += d2gdrds[i][j]*dsdp[j] + d2gdsdp[j]*dsdr[j][i];
        for (k=0; k<NS; k++) dx[i] += d2gds2[j][k]*dsdp[j]*dsdr[k][i];
      }
    }

    pureRhm(EIGHTH, t, p, 
       (double *) NULL, (double *) NULL, (double *) NULL, (double *) NULL,
       (double *) NULL, (double *) NULL, (double *) NULL, ends,           
       (double *) NULL, (double *) NULL, (double *) NULL, (double *) NULL, 
       (double *) NULL);

    dx[0] -= DENDDR0;
    dx[1] -= DENDDR1;
    dx[2] -= DENDDR2;
  }

  if(mask & THIRD) {
    double d2gdrds[NR][NS], d2gds2[NS][NS], d2gdsdp[NS], d3gdr2ds[NR][NR][NS],
      d3gdr2dp[NR][NR], d3gdrds2[NR][NS][NS], d3gdrdsdp[NR][NS],
      d3gds3[NS][NS][NS], d3gds2dp[NS][NS], dsdr[NS][NR], dsdp[NS],
      d2sdr2[NS][NR][NR], d2sdrdp[NS][NR];
    int i, j, k, l, m;

    fillD2GDRDS
    fillD2GDS2
    fillD2GDSDP
    fillD3GDR2DS
    fillD3GDR2DP
    fillD3GDRDS2
    fillD3GDRDSDP
    fillD3GDS3
    fillD3GDS2DP

    order(SECOND | FOURTH | FIFTH | SEVENTH, t, p, r, 
          NULL, dsdr, NULL, dsdp, d2sdr2, NULL, d2sdrdp,  NULL, NULL, NULL);

    for (i=0; i<NR; i++) {
      for (j=0; j<NR; j++) {
        dx2[i][j] = d3gdr2dp[i][j]; 
        for (k=0; k<NS; k++) {
          dx2[i][j] += d3gdr2ds[i][j][k]*dsdp[k]
                     + d3gdrdsdp[i][k]*dsdr[k][j] 
                     + d3gdrdsdp[j][k]*dsdr[k][i]
                     + d2gdsdp[k]*d2sdr2[k][i][j]
                     + d2gdrds[i][k]*d2sdrdp[k][j] 
                     + d2gdrds[j][k]*d2sdrdp[k][i];
          for (l=0; l<NS; l++) {
            dx2[i][j] += d3gdrds2[i][k][l]*dsdr[k][j]*dsdp[l]
                       + d3gdrds2[j][k][l]*dsdr[k][i]*dsdp[l]
                       + d2gds2[k][l]*d2sdr2[k][i][j]*dsdp[l]
                       + d3gds2dp[k][l]*dsdr[k][i]*dsdr[l][j]
                       + d2gds2[k][l]*dsdr[k][i]*d2sdrdp[l][j] 
                       + d2gds2[k][l]*dsdr[k][j]*d2sdrdp[l][i];  
            for (m=0; m<NS; m++) 
              dx2[i][j] += d3gds3[k][l][m]*dsdr[k][i]*dsdr[l][j]*dsdp[m];
          }
        }
      }
    }

  }

  if(mask & FOURTH) {
    double d2gdtdp = D2GDTDP;
    double d2gds2[NS][NS], d2gdsdt[NS], d2gdsdp[NS], dsdt[NS], dsdp[NS],
      ends[NA];
    int i, j;

    fillD2GDS2
    fillD2GDSDT
    fillD2GDSDP

    order(THIRD | FOURTH, t, p, r, 
          NULL, NULL, dsdt, dsdp, NULL, NULL, NULL, NULL, NULL, NULL);

    *dt = d2gdtdp;
    for (i=0; i<NS; i++) {
      *dt += d2gdsdt[i]*dsdp[i] + d2gdsdp[i]*dsdt[i];
      for (j=0; j<NS; j++) *dt += d2gds2[i][j]*dsdt[i]*dsdp[j];
    } 

    pureRhm(NINTH, t, p, 
       (double *) NULL, (double *) NULL, (double *) NULL, (double *) NULL,
       (double *) NULL, (double *) NULL, (double *) NULL, (double *) NULL, 
       ends,            (double *) NULL, (double *) NULL, (double *) NULL, 
       (double *) NULL);

    *dt -= ENDMEMBERS;
  }

  if(mask & FIFTH) {
    double d2gdp2 = D2GDP2;
    double d2gds2[NS][NS], d2gdsdp[NS], dsdp[NS], ends[NA];
    int i,j;

    fillD2GDS2
    fillD2GDSDP

    order(FOURTH, t, p, r, 
          NULL, NULL, NULL, dsdp, NULL, NULL, NULL, NULL, NULL, NULL);

    *dp = d2gdp2;
    for (i=0; i<NS; i++) {
      *dp += 2.0*d2gdsdp[i]*dsdp[i];
      for (j=0; j<NS; j++) *dp += d2gds2[i][j]*dsdp[i]*dsdp[j];
    }

    pureRhm(TENTH, t, p, 
       (double *) NULL, (double *) NULL, (double *) NULL, (double *) NULL,
       (double *) NULL, (double *) NULL, (double *) NULL, (double *) NULL, 
       (double *) NULL, ends,            (double *) NULL, (double *) NULL, 
       (double *) NULL);

    *dp -= ENDMEMBERS;
  }

  if(mask & SIXTH) {
    double d3gdt2dp = D3GDT2DP;
    double d2gds2[NS][NS], d2gdsdt[NS], d2gdsdp[NS], d3gds3[NS][NS][NS],
      d3gds2dp[NS][NS], d3gds2dt[NS][NS], d3gdsdtdp[NS], d3gdsdt2[NS],
      dsdt[NS], dsdp[NS], d2sdt2[NS], d2sdtdp[NS], ends[NA];
    int i, j, k;

    fillD2GDS2
    fillD2GDSDT
    fillD2GDSDP
    fillD3GDS3
    fillD3GDS2DT
    fillD3GDS2DP
    fillD3GDSDT2
    fillD3GDSDTDP

    order(THIRD | FOURTH | EIGHTH | NINTH, t, p, r, 
          NULL, NULL, dsdt, dsdp, NULL, NULL, NULL, d2sdt2, d2sdtdp, NULL);

    *dt2 = d3gdt2dp;
    for (i=0; i<NS; i++) {
      *dt2 += d3gdsdt2[i]*dsdp[i] + 2.0*d2gdsdt[i]*d2sdtdp[i] 
            + d2gdsdp[i]*d2sdt2[i] + 2.0*d3gdsdtdp[i]*dsdt[i];
      for (j=0; j<NS; j++) {
        *dt2 += 2.0*d3gds2dt[i][j]*dsdt[i]*dsdp[j]
              + d2gds2[i][j]*d2sdt2[i]*dsdp[j]
              + 2.0*d2gds2[i][j]*dsdt[i]*d2sdtdp[j]
              + d3gds2dp[i][j]*dsdt[i]*dsdt[j];
        for (k=0; k<NS; k++) *dt2 += d3gds3[i][j][k]*dsdt[i]*dsdt[j]*dsdp[k]; 
      }
    }

    pureRhm(ELEVENTH, t, p, 
       (double *) NULL, (double *) NULL, (double *) NULL, (double *) NULL,
       (double *) NULL, (double *) NULL, (double *) NULL, (double *) NULL, 
       (double *) NULL, (double *) NULL, ends,            (double *) NULL, 
       (double *) NULL);

    *dt2 -= ENDMEMBERS;
  }

  if(mask & SEVENTH) {
    double d3gdtdp2 = D3GDTDP2;
    double d2gds2[NS][NS], d2gdsdt[NS], d2gdsdp[NS], d3gds3[NS][NS][NS],
      d3gds2dt[NS][NS], d3gds2dp[NS][NS], d3gdsdtdp[NS], d3gdsdp2[NS],
      dsdt[NS], dsdp[NS], d2sdtdp[NS], d2sdp2[NS], ends[NA];
    int i, j, k;

    fillD2GDS2
    fillD2GDSDT
    fillD2GDSDP
    fillD3GDS3
    fillD3GDS2DT
    fillD3GDS2DP
    fillD3GDSDTDP
    fillD3GDSDP2

    order(THIRD | FOURTH | NINTH | TENTH, t, p, r, 
          NULL, NULL, dsdt, dsdp, NULL, NULL, NULL, NULL, d2sdtdp, d2sdp2);

    *dtdp = d3gdtdp2;
    for (i=0; i<NS; i++) {
      *dtdp += 2.0*d3gdsdtdp[i]*dsdp[i] + d2gdsdt[i]*d2sdp2[i]  
             + 2.0*d2gdsdp[i]*d2sdtdp[i] + d3gdsdp2[i]*dsdt[i]; 
      for (j=0; j<NS; j++) {
        *dtdp += 2.0*d3gds2dp[i][j]*dsdt[i]*dsdp[j] 
               + d2gds2[i][j]*dsdt[i]*d2sdp2[j] 
               + 2.0*d2gds2[i][j]*d2sdtdp[i]*dsdp[j]
               + d3gds2dt[i][j]*dsdp[i]*dsdp[j];
        for (k=0; k<NS; k++) *dtdp += d3gds3[i][j][k]*dsdt[i]*dsdp[j]*dsdp[k]; 
      }
    }

    pureRhm(TWELFTH, t, p, 
       (double *) NULL, (double *) NULL, (double *) NULL, (double *) NULL,
       (double *) NULL, (double *) NULL, (double *) NULL, (double *) NULL, 
       (double *) NULL, (double *) NULL, (double *) NULL, ends,       
       (double *) NULL);

    *dtdp -= ENDMEMBERS;
  }

  if(mask & EIGHTH) {
    double d3gdp3 = D3GDP3;
    double d2gds2[NS][NS], d2gdsdp[NS], d3gds3[NS][NS][NS], d3gds2dp[NS][NS],
      d3gdsdp2[NS], dsdp[NS], d2sdp2[NS], ends[NA];
    int i, j, k;

    fillD2GDS2
    fillD2GDSDP
    fillD3GDS3
    fillD3GDS2DP
    fillD3GDSDP2

    order(FOURTH | TENTH, t, p, r, 
          NULL, NULL, NULL, dsdp, NULL, NULL, NULL, NULL, NULL, d2sdp2);

    *dp2 = d3gdp3;
    for (i=0; i<NS; i++) {
      *dp2 += 3.0*d3gdsdp2[i]*dsdp[i] + 3.0*d2gdsdp[i]*d2sdp2[i]; 
      for (j=0; j<NS; j++) {
        *dp2 += 3.0*d2gds2[i][j]*dsdp[i]*d2sdp2[j]
              + 3.0*d3gds2dp[i][j]*dsdp[i]*dsdp[j];
        for (k=0; k<NS; k++) *dp2 += d3gds3[i][j][k]*dsdp[i]*dsdp[j]*dsdp[k];
      }
    }

    pureRhm(THIRTEENTH, t, p, 
       (double *) NULL, (double *) NULL, (double *) NULL, (double *) NULL,
       (double *) NULL, (double *) NULL, (double *) NULL, (double *) NULL, 
       (double *) NULL, (double *) NULL, (double *) NULL, (double *) NULL, 
       ends);          

    *dp2 -= ENDMEMBERS;
  }

  if(mask & NINTH) {
    double d3gds3[NS][NS][NS], d3gdrds2[NR][NS][NS], d3gdrdsdt[NR][NS],
      d3gds2dp[NS][NS], d2gdrds[NR][NS], d3gdrdtdp[NR], d3gdsdtdp[NS],
      dsdt[NS], dsdp[NS], dsdr[NS][NR], d2sdrdt[NS][NR], d2sdrdp[NS][NR],
      d2gds2[NS][NS], d2gdsdt[NS], d3gdrdsdp[NR][NS], d2gdsdp[NS],
      d2sdtdp[NS], d3gds2dt[NS][NS], ends[NA];
    int i, j, k, l;

    fillD2GDRDS
    fillD2GDS2
    fillD2GDSDT
    fillD2GDSDP
    fillD3GDRDS2
    fillD3GDRDSDT
    fillD3GDRDSDP
    fillD3GDRDTDP
    fillD3GDS3
    fillD3GDS2DT
    fillD3GDSDTDP
    fillD3GDS2DP

    order(SECOND | THIRD | FOURTH | SIXTH | SEVENTH | NINTH, t, p, r,
      NULL, dsdr, dsdt, dsdp, NULL, d2sdrdt, d2sdrdp, NULL, d2sdtdp, NULL);

    for (i=0; i<NR; i++) {
      for (j=0,dxdt[i]=d3gdrdtdp[i]; j<NS; j++) {
        dxdt[i] += d3gdsdtdp[j]*dsdr[j][i] + d2gdsdt[j]*d2sdrdp[j][i] +
                   d3gdrdsdt[i][j]*dsdp[j] + d2gdrds[i][j]*d2sdtdp[j] +
                   d3gdrdsdp[i][j]*dsdt[j] + d2gdsdp[j]*d2sdrdt[j][i];
        for (k=0; k<NS; k++) {
          dxdt[i] += d3gdrds2[i][j][k]*dsdt[j]*dsdp[k] +
                     d2gds2[j][k]*dsdt[j]*d2sdrdp[k][i] +
                     d2gds2[j][k]*dsdp[j]*d2sdrdt[k][i] +
                     d3gds2dt[j][k]*dsdr[j][i]*dsdp[k] +
                     d3gds2dp[j][k]*dsdr[j][i]*dsdt[k] +
                     d2gds2[j][k]*dsdr[j][i]*d2sdtdp[k];
          for (l=0; l<NS; l++)
            dxdt[i] += d3gds3[j][k][l]*dsdr[j][i]*dsdt[k]*dsdp[l];
        }
      }
    }

    pureRhm(NINTH, t, p,
       (double *) NULL, (double *) NULL, (double *) NULL, (double *) NULL,
       (double *) NULL, (double *) NULL, (double *) NULL, (double *) NULL,
       ends,            (double *) NULL, (double *) NULL, (double *) NULL,
       (double *) NULL);

    dxdt[0] -= DENDDR0;
    dxdt[1] -= DENDDR1;
    dxdt[2] -= DENDDR2;
  }

  if(mask & TENTH) {
    double d3gds3[NS][NS][NS], d3gdrds2[NR][NS][NS], d3gds2dp[NS][NS],
      d2gdrds[NR][NS], dsdp[NS], dsdr[NS][NR], d2sdrdp[NS][NR], d2gds2[NS][NS],
      d3gdrdsdp[NR][NS], d3gdrdp2[NR], d3gdsdp2[NS], d2gdsdp[NS], d2sdp2[NS],
      ends[NA];
    int i, j, k, l;

    fillD2GDRDS
    fillD2GDS2
    fillD2GDSDP
    fillD3GDRDS2
    fillD3GDRDSDP
    fillD3GDRDP2
    fillD3GDS3
    fillD3GDS2DP
    fillD3GDSDP2

    order(SECOND | FOURTH | SEVENTH | TENTH, t, p, r,
      NULL, dsdr, NULL, dsdp, NULL, NULL, d2sdrdp, NULL, NULL, d2sdp2);

    for (i=0; i<NR; i++) {
      for (j=0,dxdp[i]=d3gdrdp2[i]; j<NS; j++) {
        dxdp[i] += d3gdsdp2[j]*dsdr[j][i] + d2gdsdp[j]*d2sdrdp[j][i] +
                   2.0*d3gdrdsdp[i][j]*dsdp[j] + d2gdrds[i][j]*d2sdp2[j] +
                   d2gdsdp[j]*d2sdrdp[j][i];
        for (k=0; k<NS; k++) {
          dxdp[i] += d3gdrds2[i][j][k]*dsdp[j]*dsdp[k] +
                     2.0*d2gds2[j][k]*dsdp[j]*d2sdrdp[k][i] +
                     2.0*d3gds2dp[j][k]*dsdr[j][i]*dsdp[k] +
                     d2gds2[j][k]*dsdr[j][i]*d2sdp2[k];
          for (l=0; l<NS; l++)
            dxdp[i] += d3gds3[j][k][l]*dsdr[j][i]*dsdp[k]*dsdp[l];
        }
      }
    }


    pureRhm(TENTH, t, p,
       (double *) NULL, (double *) NULL, (double *) NULL, (double *) NULL,
       (double *) NULL, (double *) NULL, (double *) NULL, (double *) NULL,
       (double *) NULL, ends,            (double *) NULL, (double *) NULL,
       (double *) NULL);

    dxdp[0] -= DENDDR0;
    dxdp[1] -= DENDDR1;
    dxdp[2] -= DENDDR2;
  }

}

/* end of file RHOMBOHEDRAL.C */
