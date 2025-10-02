// ---12/05/20
//#include "stdafx.h"
#include "Header.h"
#define  BLOCK_SIZE			1024
#define MBSPMAX				20000
#define MEDOSIZE			280000

//#include "cuda.h"
#include "cuda_runtime_api.h"
#include "math.h"
//#include "cuda_runtime_api.h"

//#include "host_defines.h"
#include "device_launch_parameters.h"
#include "math_constants.h"
//#include "math_functions.h"

#include <stdio.h>
#include <basetsd.h>
#include <memory.h>

#include "windows.h"
#include "iostream"

#include <set>
#include <stdlib.h>

//#include <omp.h>  // OpenMP
//#include "opencv2/opencv.hpp"


#include <thrust/device_ptr.h>
#include <thrust/copy.h>
#include <stdio.h>

//void FilterRMed(float *pw, int *pwg, float *medo, float *pXC, float *pYC, int n, int sz1, float mprc, float sprc, float mabs, float pabs, int dlen, float *pDefMB, float *pXdefMB, float *pYdefMB, float *pDefSP, float *pXdefSP, float *pYdefSP, int *MBnum, int *SPnum, int mode);

struct is_not_zero
{
	__host__ __device__
		bool operator()(const float x)
	{
		return (x != 0.);
	}
};

//using namespace cv;
//using namespace cuda;
/*
__global__ void add(int *a, int *b, int *c)
{ int tid = blockIdx.x;
	if(tid < BLOCK_SIZE)
		c[tid] = a[tid] + b[tid];
}

__global__ void gpuInvert( int *image, int w, int h)
{
}
*/
#define PATSIZE	1024	// max. size of pattern
#define POLSIZE	256		// max. size of polygon

//__device__ float *d_P1xo, *d_P1y, *d_P2x, *d_P2y, *d_Pnum;
int ThrPerBlock = 128;
///// for FWM
//extern int NaC;
extern int NaCN;
__device__ float *d_W, *d_Wmin;
extern float *d_P1x, *d_P1y, *d_P2x, *d_P2y, *d_P3x, *d_P3y, *d_Wl, *d_Wlmin, *dmedo, *dmedoN, *d_PShift;
extern int *d_WG, *d_Puseint;
extern float *d_Pint1x, *d_Pint1y, *d_Pint2x, *d_Pint2y; // intersection points for testing (on device), d_Pint2 is now used

extern int *d_Pnum;
extern int *d_PaddrR, *d_PaddrC;
extern float *d_pDefMB, *d_pXdefMB, *d_pYdefMB, *d_pDefSP, *d_pXdefSP, *d_pYdefSP;
extern float *d_DefMB, *d_XdefMB, *d_YdefMB, *d_DefSP, *d_XdefSP, *d_YdefSP; // condensed
extern int *d_pMBnum, *d_pSPnum;

///// for Non-FWM
__device__ float *d_WN, *d_WminN;
extern float *d_P1Nx, *d_P1Ny, *d_P2Nx, *d_P2Ny, *d_WlN, *d_WlminN;
extern UINT64 *d_Pint1Nx, *d_Pint1Ny, *d_Pint2Nx, *d_Pint2Ny; // intersection points for testing (on device), d_Pint2 is now used

extern int *d_PnumN;
extern int *d_PaddrCN, *d_PindCN, *d_PaddrRN;
extern float *d_pDefMBN, *d_pXdefMBN, *d_pYdefMBN, *d_pDefSPN, *d_pXdefSPN, *d_pYdefSPN;
extern float *d_DefMBN, *d_XdefMBN, *d_YdefMBN, *d_DefSPN, *d_XdefSNP, *d_YdefSPN; // condensed
extern int *d_pMBnumN, *d_pSPnumN;
extern float *pDefMBN, *pXdefMBN, *pYdefMBN, *pDefSPN, *pXdefSPN, *pYdefSPN;

extern UINT8 *d_Im, *d_ImIRB, *d_ImIRBS;
extern UINT16 *d_ImCLC;
extern float *pP1x, *pP1y, *pP2x, *pP2y, *pP3x, *pP3y;

extern int *pPnum, *pPaddrR, *pPaddrCm, *pPuseint, *pTInoDtoIRB;													// measurement tool points arrays
extern float *pPInt1x, *pPInt1y, *pPInt2x, *pPInt2y, *pPInt3x, *pPInt3y, *pPInt4x, *pPInt4y;	// intersection points arrays
extern float *pPInt1Nx, *pPInt1Ny, *pPInt2Nx, *pPInt2Ny;	// intersection points arrays (Non-FWM)
extern float *pShift;
//// Non-FWM ///
extern float *pP1Nx, *pP1Ny, *pP2Nx, *pP2Ny, pWiN;
extern int *pPnumN, *pWG;
extern int *pPaddrCN, *pPaddrRN, *pPindCN;
extern float *pWidthN;
extern float *pWminN;

extern UINT8 *pIm; // Im image
extern UINT8 *pImIRB; // IRB image
extern UINT8 *pImIRBS; // IRBS image
extern float *pWidth, *pWmin, *medo, *medoN;
extern float *pDefMB, *pXdefMB, *pYdefMB, *pDefSP, *pXdefSP, *pYdefSP;

extern float *d_y, *d_smo;

//extern int imw, imh;
//extern CRITICAL_SECTION CriticalSection;
//
//extern int ngpu, cGPU;
//

typedef struct 
{
	float y;
	float x;
}Point2f;

//
//bool intersection(Point2f start1, Point2f end1, Point2f start2, Point2f end2, Point2f *out_intersection)
//{
//	Point2f dir1 = end1 - start1;
//	Point2f dir2 = end2 - start2;
//
//	//??????? ????????? ?????? ?????????? ????? ???????
//	float a1 = -dir1.y;
//	float b1 = +dir1.x;
//	float d1 = -(a1*start1.x + b1 * start1.y);
//
//	float a2 = -dir2.y;
//	float b2 = +dir2.x;
//	float d2 = -(a2*start2.x + b2 * start2.y);
//
//	//??????????? ????? ????????, ??? ????????? ? ????? ????????????? ???
//	float seg1_line2_start = a2 * start1.x + b2 * start1.y + d2;
//	float seg1_line2_end = a2 * end1.x + b2 * end1.y + d2;
//
//	float seg2_line1_start = a1 * start2.x + b1 * start2.y + d1;
//	float seg2_line1_end = a1 * end2.x + b1 * end2.y + d1;
//
//	//???? ????? ?????? ??????? ????? ???? ????, ?????? ?? ? ????? ????????????? ? ??????????? ???.
//	if (seg1_line2_start * seg1_line2_end >= 0 || seg2_line1_start * seg2_line1_end >= 0)
//		return false;
//
//	float u = seg1_line2_start / (seg1_line2_start - seg1_line2_end);
//	*out_intersection = start1 + u * dir1;
//
//	return true;
//}

void AngleDist(float y, float x, float y1, float x1, float y2, float x2, float *d, float *tang)
{
	float A, B, C, k;

	A = y1 - y2;
	B = x2 - x1;
	C = x1*y2 - x2*y1;
	*d = (A*x + B*y + C) / sqrt(A*A + B*B);
	k = A / B;
	//*ang = atan(k);
	*tang = k;
}
__device__ void AngleDistG(float x, float y, float x1, float y1, float x2, float y2, float *d, float *ang)
{
	float A, B, C, k;

	A = y1 - y2;
	B = x2 - x1;
	C = x1 * y2 - x2 * y1;
	*d = (A*x + B * y + C) / sqrt(A*A + B * B);
	k = A / B;
	*ang = atan(k);
	//*ang = k;
}

// findig distace from point to segment and segment's angle for CUDA
__device__ void AngleDistCUDA(float y, float x, float y1, float x1, float y2, float x2, float *d, float *tang)
{
	float A, B, C, k;

	A = y1 - y2;
	B = x2 - x1;
	C = x1 * y2 - x2 * y1;
	*d = (A*x + B * y + C) / sqrt(A*A + B * B);
	k = A / B;
	//*ang = atan(k);
	*tang = k;
}

void FilterM7(float *pw, int *pwg, float *pXC, float *pYC, int n, int nvoid, int sz1, int sz2, float ang, float mprc, float sprc, float mabs, float pabs, int dlen, float *pDefMB, float *pXdefMB, float *pYdefMB, float *pDefSP, float *pXdefSP, float *pYdefSP, int *MBnum, int *SPnum, int mode, int andNOor)
{
	/// mode==0  - not usinf CAD width
	/// andNOor  - AND==1 / OR==0 /Aver==2 (for two filters)
	int j, jm, jp, jm2, jp2, jm15, jp15;
	float x, xm, xm2, xm15, xp, xp2, y, ym, ym15, ym2, yp, yp15, yp2, yg, d, tangl, d15, tangl15, d2, tangl2, ta, ta2, ta15, dm, dm15, dm2, yav, yav15, yav2, def;
	int spnum = 0, mbnum = 0;
	float tang = tan(ang*3.14159 / 180.);
	int sz15 = (sz1 + sz2) / 2;

	if (n == 0)
	{
		*SPnum = spnum;
		*MBnum = mbnum;
		return;
	}
	for (j = nvoid; j < n - nvoid - 1; j++)
	{
		def = 0.;
		if ((jp = j + sz1) > n - 1- nvoid)
			jp = n - 1- nvoid;
		if ((jm = j - sz1) < nvoid)
			jm = nvoid;
		if ((jp2 = j + sz2) > n - 1- nvoid)
			jp2 = n - 1- nvoid;
		if ((jm2 = j - sz2) < nvoid)
			jm2 = nvoid;
		if ((jp15 = j + sz15) > n - 1- nvoid)
			jp15 = n - 1- nvoid;
		if ((jm15 = j - sz15) < nvoid)
			jm15 = nvoid;
		x = j;
		xm = jm;
		xm2 = jm2;
		xm15 = jm15;
		xp = jp;
		xp2 = jp2;

		y = *(pw + j);
		yg = *(pwg + j);

		ym = *(pw + jm);
		//ymg = *(pwg + jm);
		yp = *(pw + jp);
		//ypg = *(pwg + jp);
		//if (mode & 1 == 0)
		//	ymg = ypg = yg = 0;
		//AngleDist(y - yg, x, ym - ymg, xm, yp - ypg, xp, &d, &tangl);
		AngleDist(y, x, ym, xm, yp, xp, &d, &tangl);

		ym2 = *(pw + jm2);
		yp2 = *(pw + jp2);
		AngleDist(y, x, ym2, xm2, yp2, xp2, &d2, &tangl2);

		ym15 = *(pw + jm15);
		yp15 = *(pw + jp15);
		AngleDist(y, x, ym15, xm15, yp15, xp2, &d15, &tangl15);

		//a = abs(angl)*180. / 3.1416;
		//a2 = abs(angl2)*180. / 3.1416;
		ta = abs(tangl);
		ta2 = abs(tangl2);
		ta15 = abs(tangl15);
		/*ta = tangl;
		ta2 = tangl2;*/

		if (ta <= tang)
			dm = d;
		else dm = 0.;

		if (ta2 <= tang)
			dm2 = d2;
		else
			dm2 = 0.;

		if (ta15 <= tang)
			dm15 = d15;
		else
			dm15 = 0.;

		/*if ((ta <= tang) )
			dm = d;
		else if ((ta2 <= tang))
			dm = d2;
		else
			dm = 0;*/
		yav = (ym + yp) / 2.;
		yav2 = (ym2 + yp2) / 2.;
		yav15 = (ym15 + yp15) / 2.;
		//d = y - yav;
		//d2 = y - yav2;

		//dm = d;
		int s = 0, s15 = 0, s2 = 0; // sign - 1 positive/ -1 negative
		if ((y > ym) && (y > yp))
			s = 1;
		if ((y > ym2) && (y > yp2))
			s2 = 1;
		if ((y > ym15) && (y > yp15))
			s15 = 1;
		if ((y < ym) && (y < yp))
			s = -1;
		if ((y < ym2) && (y < yp2))
			s2 = -1;
		if ((y < ym15) && (y < yp15))
			s15 = -1;

		int n = 0, n15 = 0, n2 = 0, p = 0, p15 = 0, p2 = 0; // over positive or negative thresholds
		if (dm > abs(yav)*sprc / 100.)
			p = 1;
		if (dm < -abs(yav)*mprc / 100.)
			n = 1;
		if (dm15 > abs(yav15)*sprc / 100.)
			p15 = 1;
		if (dm15 < -abs(yav15)*sprc / 100.)
			n15 = 1;
		if (dm2 > abs(yav2)*mprc / 100.)
			p2 = 1;
		if (dm2 < -abs(yav2)*mprc / 100.)
			n2 = 1;

		if ((y == 0) || (ym == 0) || (yp == 0))
			def = 0.;
		else
		{
			if ((s == 1) && (s15 == 1) || (s == 1) && (s2 == 1) || (s15 == 1) && (s2 == 1)) // spur candidate
			{
				if ((p == 1) && (p15 == 1))
				{
					if (abs(dm) > abs(dm15))
						def = dm;
					else
						def = dm15;
				}
				if ((p == 1) && (p2 == 1))
				{
					if (abs(dm) > abs(dm2))
						def = dm;
					else
						def = dm2;
				}
				if ((p15 == 1) && (p2 == 1))
				{
					if (abs(dm15) > abs(dm2))
						def = dm15;
					else
						def = dm2;
				}
			}

			if ((s == -1) && (s15 == -1) || (s == -1) && (s2 == -1) || (s15 == -1) && (s2 == -1)) // MB candidate
			{
				if ((n == 1) && (n15 == 1))
				{
					if (abs(dm) > abs(dm15))
						def = dm;
					else
						def = dm15;
				}
				if ((n == 1) && (n2 == 1))
				{
					if (abs(dm) > abs(dm2))
						def = dm;
					else
						def = dm2;
				}
				if ((n15 == 1) && (n2 == 1))
				{
					if (abs(dm15) > abs(dm2))
						def = dm15;
					else
						def = dm2;
				}
			}
		}
		if (def > 0.01)
		{
			/*if (abs(*(pXC + j) - 1350) < 50)
				j = j;*/
			*(pDefSP++) = def;
			*(pXdefSP++) = *(pXC + j);
			*(pYdefSP++) = *(pYC + j);
			spnum++;
			// *(pXC + j)>6330 && *(pXC + j)<6340 && *(pYC + j)>17730 && *(pYC + j)<17750
		}
		else if (def < -0.01)
		{
			*(pDefMB++) = def;
			*(pXdefMB++) = *(pXC + j);
			*(pYdefMB++) = *(pYC + j);
			mbnum++;
		}
	}
	*SPnum = spnum;
	*MBnum = mbnum;
}

void SrunmedS(float* y, float* smo, long n, int bw_tot, int end_rule, int skip) // 2022.07.29 - running median with skipping [skip] values
{
	/*
		 *  Computes "Running Median" smoother ("Stuetzle" algorithm) with medians of 'band'

		 *  Input:
		 *	y(n)	- responses in order of increasing predictor values
		 *	n	- number of observations
		 *	bw_tot	- span of running medians (MUST be ODD !!)
		 *	end_rule -- 0: Keep original data at ends {j; j < b2 | j > n-b2}
		 *		     -- 1: Constant ends = median(y[1],..,y[bw]) "robust"
		 *	skip   - how many values to skip between each successive value taken in the computation of the median
		 *  Output:
		 *	smo(n)	- smoothed responses

		 * NOTE:  The 'end' values are just copied !! this is fast but not too nice !
		 */

		 /* Local variables */
	double rmed, rmin, temp, rnew, yout, yi;
	double rbe, rtb, rse, yin, rts;
	int imin, ismo, i, j, first, last, band2, kminus, kplus, shift, bw, step;

	step = skip + 1;
	//bw = (bw_tot - 1) / step + 1;
	bw = (((bw_tot - 1) / 2) / step) * 2 + 1;
	if (n < bw_tot)
		return;

	//double *scrat = (double *)malloc(bw, sizeof(double));
	//was  
	double *scrat = (double *)malloc((unsigned)bw * sizeof(double));

	//if ((bw-1)*step+1 > n)
	//	error(_("bandwidth/span of running medians is larger than n"));

	/* 1. Compute  'rmed' := Median of the first 'band' values
	   ======================================================== */

	for (shift = 0;
		shift < step;
		shift++) {

		for (i = 0; i < bw; ++i)
			scrat[i] = y[i*step + shift];

		/* find minimal value  rmin = scrat[imin] <= scrat[j] */
		rmin = scrat[0]; imin = 0;
		for (i = 1; i < bw; ++i)
			if (scrat[i] < rmin) {
				rmin = scrat[i]; imin = i;
			}
		/* swap scrat[0] <-> scrat[imin] */
		temp = scrat[0]; scrat[0] = rmin; scrat[imin] = temp;
		/* sort the rest of of scrat[] by bubble (?) sort -- */
		for (i = 2; i < bw; ++i) {
			if (scrat[i] < scrat[i - 1]) {/* find the proper place for scrat[i] */
				temp = scrat[i];
				j = i;
				do {
					scrat[j] = scrat[j - 1];
					--j;
				} while (scrat[j - 1] > temp); /* now:  scrat[j-1] <= temp */
				scrat[j] = temp;
			}
		}
		band2 = bw / 2;
		rmed = scrat[band2];/* == Median( y[(1:band2)-1] ) */
		/* "malloc" had  free( (char*) scrat);*/ /*-- release scratch memory --*/
		//free((char*)scrat);
		if (shift == 0) {
			if (end_rule == 0) { /*-- keep DATA at end values */
				for (i = 0; i < band2*step; ++i)
					smo[i] = y[i];
			}
			else { /* if(end_rule == 1)  copy median to CONSTANT end values */
				for (i = 0; i < band2*step; ++i)
					smo[i] = rmed;
			}
		}
		smo[band2*step + shift] = rmed;
		band2++; /* = bw / 2 + 1*/;

		/* Big	FOR Loop: RUNNING median, update the median 'rmed'
		   ======================================================= */
		for (first = step + shift, last = bw * step + shift, ismo = band2 * step + shift;
			last < n;
			first += step, last += step, ismo += step) {

			yin = y[last];
			yout = y[first - step];
			rnew = rmed; /* New median = old one   in all the simple cases --*/

			if (yin < rmed) {
				if (yout >= rmed) {
					kminus = 0;
					if (yout > rmed) {/*	--- yin < rmed < yout --- */
						//if (print_level >= 2) REprintf(": yin < rmed < yout ");
						rnew = yin;/* was -rinf */
						for (i = first; i <= last; i += step) {
							yi = y[i];
							if (yi < rmed) {
								++kminus;
								if (yi > rnew)	rnew = yi;
							}
						}
						if (kminus < band2)		rnew = rmed;
					}
					else {/*		--- yin < rmed = yout --- */
						rse = rts = yin;/* was -rinf */
						for (i = first; i <= last; i += step) {
							yi = y[i];
							if (yi <= rmed) {
								if (yi < rmed) {
									++kminus;
									if (yi > rts)	rts = yi;
									if (yi > rse)	rse = yi;
								}
								else		rse = yi;

							}
						}
						rnew = (kminus == band2) ? rts : rse;
					}
				} /* else: both  yin, yout < rmed -- nothing to do .... */
			}
			else if (yin != rmed) { /* yin > rmed */
				if (yout <= rmed) {
					kplus = 0;
					if (yout < rmed) {/* -- yout < rmed < yin --- */
						rnew = yin; /* was rinf */
						for (i = first; i <= last; i += step) {
							yi = y[i];
							if (yi > rmed) {
								++kplus;
								if (yi < rnew)	rnew = yi;
							}
						}
						if (kplus < band2)	rnew = rmed;
					}
					else { /* -- yout = rmed < yin --- */
						rbe = rtb = yin; /* was rinf */
						for (i = first; i <= last; i += step) {
							yi = y[i];
							if (yi >= rmed) {
								if (yi > rmed) {
									++kplus;
									if (yi < rtb)	rtb = yi;
									if (yi < rbe)	rbe = yi;
								}
								else		rbe = yi;
							}
						}
						rnew = (kplus == band2) ? rtb : rbe;
					}
				} /* else: both  yin, yout > rmed --> nothing to do */
			} /* else: yin == rmed -- nothing to do .... */
			rmed = rnew;
			smo[ismo] = rmed;
		} /*     END FOR ------------ big Loop -------------------- */
	}

	band2 = bw / 2;
	if (end_rule == 0) { /*-- keep DATA at end values */
		for (i = n - band2 * step; i < n; ++i)
			smo[i] = y[i];
	}
	else { /* if(end_rule == 1)  copy median to CONSTANT end values */
		for (i = n - band2 * step; i < n; ++i)
			smo[i] = smo[n - band2 * step - 1];
	}
	free(scrat);
} /* SrunmedS */

// Prototype
void SrunmedProto(float* y, float* smo, long n, int bw_tot, int skip) // 2022.08.01 - running median with skipping [skip] values, with linear interpolation
{
	/* Local variables */
	double rmed, rmin, temp, rnew, yout, yi;
	double rbe, rtb, rse, yin, rts, fa, fb;
	int imin, ismo, i, j, first, last, band2, kminus, kplus, shift, bw, step, num_meds, last_med, a, b, x, rad;

	step = skip + 1;
	bw = (((bw_tot - 1) / 2) / step) * 2 + 1;

	rad = ((bw - 1) / 2)*step;
	//std::cout << "new: bw=" << bw << ", radius=" << bw / 2 << ", step=" << step << "\n";

	if (n < bw_tot)
		return;
	//double *scrat = (double *)malloc(bw, sizeof(double));
	//was
	double *scrat = (double *)malloc((unsigned)bw * sizeof(double));

	//if ((bw-1)*step+1 > n)
	//	error(_("bandwidth/span of running medians is larger than n"));

	/* 1. Compute  'rmed' := Median of the first 'band' values
	   ======================================================== */

	shift = 0;

	for (i = 0; i < bw; ++i)
	{
		scrat[i] = y[i*step + shift];
	}

	/* find minimal value  rmin = scrat[imin] <= scrat[j] */
	rmin = scrat[0]; imin = 0;
	for (i = 1; i < bw; ++i)
		if (scrat[i] < rmin) {
			rmin = scrat[i]; imin = i;
		}
	/* swap scrat[0] <-> scrat[imin] */
	temp = scrat[0]; scrat[0] = rmin; scrat[imin] = temp;
	/* sort the rest of of scrat[] by bubble (?) sort -- */
	for (i = 2; i < bw; ++i) {
		if (scrat[i] < scrat[i - 1]) {/* find the proper place for scrat[i] */
			temp = scrat[i];
			j = i;
			do {
				scrat[j] = scrat[j - 1];
				--j;
			} while (scrat[j - 1] > temp); /* now:  scrat[j-1] <= temp */
			scrat[j] = temp;
		}
	}
	band2 = (bw - 1) / 2;
	rmed = scrat[band2];/* == Median( y[(1:band2)-1] ) */
	/* "malloc" had  free( (char*) scrat);*/ /*-- release scratch memory --*/

	/*
	if (shift == 0) {
		if (end_rule == 0) { //-- keep DATA at end values
			for (i = 0; i < band2*step; ++i)
				smo[i] = y[i];
		}
		else { // if(end_rule == 1)  copy median to CONSTANT end values
			for (i = 0; i < band2*step; ++i)
				smo[i] = rmed;
		}
	}
	*/

	smo[/*band2*step +*/ shift] = rmed;
	num_meds = 1;
	band2++; /* = bw / 2 + 1*/;


	/* Big	FOR Loop: RUNNING median, update the median 'rmed'
	   ======================================================= */
	for (first = step + shift, last = bw * step + shift, ismo = band2 * step + shift;
		last < n;
		first += step, last += step, ismo += step) {

		yin = y[last];
		yout = y[first - step];
		rnew = rmed; /* New median = old one   in all the simple cases --*/

		if (yin < rmed) {
			if (yout >= rmed) {
				kminus = 0;
				if (yout > rmed) {/*	--- yin < rmed < yout --- */
					//if (print_level >= 2) REprintf(": yin < rmed < yout ");
					rnew = yin;/* was -rinf */
					for (i = first; i <= last; i += step) {
						yi = y[i];
						if (yi < rmed) {
							++kminus;
							if (yi > rnew)	rnew = yi;
						}
					}
					if (kminus < band2)		rnew = rmed;
				}
				else {/*		--- yin < rmed = yout --- */
					rse = rts = yin;/* was -rinf */
					for (i = first; i <= last; i += step) {
						yi = y[i];
						if (yi <= rmed) {
							if (yi < rmed) {
								++kminus;
								if (yi > rts)	rts = yi;
								if (yi > rse)	rse = yi;
							}
							else		rse = yi;

						}
					}
					rnew = (kminus == band2) ? rts : rse;
				}
			} /* else: both  yin, yout < rmed -- nothing to do .... */
		}
		else if (yin != rmed) { /* yin > rmed */
			if (yout <= rmed) {
				kplus = 0;
				if (yout < rmed) {/* -- yout < rmed < yin --- */
					rnew = yin; /* was rinf */
					for (i = first; i <= last; i += step) {
						yi = y[i];
						if (yi > rmed) {
							++kplus;
							if (yi < rnew)	rnew = yi;
						}
					}
					if (kplus < band2)	rnew = rmed;
				}
				else { /* -- yout = rmed < yin --- */
					rbe = rtb = yin; /* was rinf */
					for (i = first; i <= last; i += step) {
						yi = y[i];
						if (yi >= rmed) {
							if (yi > rmed) {
								++kplus;
								if (yi < rtb)	rtb = yi;
								if (yi < rbe)	rbe = yi;
							}
							else		rbe = yi;
						}
					}
					rnew = (kplus == band2) ? rtb : rbe;
				}
			} /* else: both  yin, yout > rmed --> nothing to do */
		} /* else: yin == rmed -- nothing to do .... */
		rmed = rnew;
		smo[ismo - rad] = rmed;
		num_meds++;
	} /*     END FOR ------------ big Loop -------------------- */

	free(scrat);

	band2 = (bw - 1) / 2;
	last_med = band2 * step + (num_meds - 1)*step;


	/*if (end_rule == 0) { //-- keep DATA at end values
		for (i = last_med + 1; i < n; ++i)
			smo[i] = y[i];
	}
	else { // if(end_rule == 1)  copy median to CONSTANT end values
		for (i = last_med + 1; i < n; ++i)
			smo[i] = smo[last_med];
	}*/

	if (skip > 0) {
		for (i = 0; i < num_meds - 1; i++) {
			for (j = 1; j < step; j++) {
				x = (band2 + i)*step + j;
				a = (band2 + i)*step;
				b = (band2 + i + 1)*step;
				fa = smo[a - rad];
				fb = smo[b - rad];
				smo[x - rad] = fa + j * (fb - fa) / step; //b-a=step, x-a=j
			}
		}
	}
}

void SrunmedSLIExtNoS(float* yinp, float* y/*extended*/, float* smo, long n1, int bw_tot, /*int end_rule,*/ int skip, int noskipsz) // 2022.08.01 - running median with skipping [skip] values, with linear interpolation
{
	/*
		 *  Computes "Running Median" smoother ("Stuetzle" algorithm) with medians of 'band'

		 *  Input:
		 *	yinp(n1)	- responses in order of increasing predictor values
		 *	y(n)		- auxillary array; n = n1 + bw_tot - 1
		 *	n1		- number of observations
		 *	bw_tot		- span of running medians (MUST be ODD !!)
		 //*	end_rule 		-- 0: Keep original data at ends {j; j < b2 | j > n-b2}
		 *		     		-- 1: Constant ends = median(y[1],..,y[bw]) "robust"
		 *	skip		- how many data points to skip between two used points when doing interpolation
		 *	noskipsz	- length of the regions at the beginning and at the end where interpolation is not used; must be no greater than n1/2 and no lesser than bw_tot/2
		 *  Output:
		 *	smo(n)		- smoothed responses

		 //* NOTE:  The 'end' values are just copied !! this is fast but not too nice !
	*/

	/* Local variables */
	int i, rad, rad2n, step, dif;

	rad = (bw_tot - 1) / 2;

	step = skip + 1;
	rad2n = (((bw_tot - 1) / 2) / step) * step;

	dif = n1 - 2 * noskipsz - 1 - ((n1 - 2 * noskipsz - 1) / step)*step;


	for (i = 0; i < rad; i++)
		y[i] = yinp[0];
	//memcpy(y + bw_tot, yinp, n1 * sizeof(float));
	for (i = 0; i < n1; i++)
		y[rad + i] = yinp[i];
	for (i = 0; i < rad; i++)
		y[rad + n1 + i] = yinp[n1 - 1];


	SrunmedProto(y, smo /*- ((bw_tot - 1) / 2)*/, noskipsz + bw_tot - 1, bw_tot, 0);

	SrunmedProto(y + (n1 - noskipsz) - dif, smo + (n1 - noskipsz /*- (bw_tot - 1) / 2*/) - dif, noskipsz + bw_tot - 1 + dif, bw_tot, 0);

	SrunmedProto(y + noskipsz + rad - rad2n, smo + (noskipsz /*- (bw_tot - 1) / 2*/), n1 - 2 * noskipsz + 2 * rad2n, bw_tot, skip);

} /* SrunmedSLIExt */


// SrunmedSLIExt=SrunmedSLI + Extention
void SrunmedSLIExt(float* yinp, float* y/*extended*/, float* smo, long n1, int bw_tot, int end_rule, int skip) // 2022.08.01 - running median with skipping [skip] values, with linear interpolation
{
	/*
		 *  Computes "Running Median" smoother ("Stuetzle" algorithm) with medians of 'band'

		 *  Input:
		 *	y(n)	- responses in order of increasing predictor values
		 *	n	- number of observations
		 *	bw_tot	- span of running medians (MUST be ODD !!)
		 *	end_rule -- 0: Keep original data at ends {j; j < b2 | j > n-b2}
		 *		     -- 1: Constant ends = median(y[1],..,y[bw]) "robust"
		 *  Output:
		 *	smo(n)	- smoothed responses

		 * NOTE:  The 'end' values are just copied !! this is fast but not too nice !
		 */

		 /* Local variables */
	double rmed, rmin, temp, rnew, yout, yi;
	double rbe, rtb, rse, yin, rts, fa, fb;
	int imin, ismo, i, j, first, last, band2, kminus, kplus, shift, bw, step, num_meds, last_med, a, b, x;
	float v;
	for (i = 0; i < bw_tot; i++)
		y[i] = yinp[0];
	//memcpy(y + bw_tot, yinp, n1 * sizeof(float));
	for (i = 0; i < n1; i++)
		y[bw_tot + i] = yinp[i];
	for (i = 0; i < bw_tot - 1; i++)
		y[bw_tot + n1 + i] = yinp[n1 - 1];
	int n = n1 + 2 * bw_tot;
	step = skip + 1;
	bw = (((bw_tot - 1) / 2) / step) * 2 + 1;
	//std::cout << "new: bw=" << bw << ", radius=" << bw / 2 << ", step=" << step << "\n";

	if (n < bw)
		return;
	//double *scrat = (double *)malloc(bw, sizeof(double));
	//was
	double *scrat = (double *)malloc((unsigned)bw * sizeof(double));

	//if ((bw-1)*step+1 > n)
	//	error(_("bandwidth/span of running medians is larger than n"));

	/* 1. Compute  'rmed' := Median of the first 'band' values
	   ======================================================== */

	shift = 0;

	for (i = 0; i < bw; ++i)
		scrat[i] = y[i*step + shift];

	/* find minimal value  rmin = scrat[imin] <= scrat[j] */
	rmin = scrat[0]; imin = 0;
	for (i = 1; i < bw; ++i)
		if (scrat[i] < rmin) {
			rmin = scrat[i]; imin = i;
		}
	/* swap scrat[0] <-> scrat[imin] */
	temp = scrat[0]; scrat[0] = rmin; scrat[imin] = temp;
	/* sort the rest of of scrat[] by bubble (?) sort -- */
	for (i = 2; i < bw; ++i) {
		if (scrat[i] < scrat[i - 1]) {/* find the proper place for scrat[i] */
			temp = scrat[i];
			j = i;
			do {
				scrat[j] = scrat[j - 1];
				--j;
			} while (scrat[j - 1] > temp); /* now:  scrat[j-1] <= temp */
			scrat[j] = temp;
		}
	}
	band2 = bw / 2;
	rmed = scrat[band2];/* == Median( y[(1:band2)-1] ) */
	/* "malloc" had  free( (char*) scrat);*/ /*-- release scratch memory --*/
	if (shift == 0) {
		if (end_rule == 0) { /*-- keep DATA at end values */
			for (i = 0; i < band2*step; ++i)
				smo[i] = y[i];
		}
		else { /* if(end_rule == 1)  copy median to CONSTANT end values */
			for (i = 0; i < band2*step; ++i)
				smo[i] = rmed;
		}
	}
	smo[band2*step + shift] = rmed;
	num_meds = 1;
	band2++; /* = bw / 2 + 1*/;

	/* Big	FOR Loop: RUNNING median, update the median 'rmed'
	   ======================================================= */
	for (first = step + shift, last = bw * step + shift, ismo = band2 * step + shift;
		last < n;
		first += step, last += step, ismo += step) {

		yin = y[last];
		yout = y[first - step];
		rnew = rmed; /* New median = old one   in all the simple cases --*/

		if (yin < rmed) {
			if (yout >= rmed) {
				kminus = 0;
				if (yout > rmed) {/*	--- yin < rmed < yout --- */
					//if (print_level >= 2) REprintf(": yin < rmed < yout ");
					rnew = yin;/* was -rinf */
					for (i = first; i <= last; i += step) {
						yi = y[i];
						if (yi < rmed) {
							++kminus;
							if (yi > rnew)	rnew = yi;
						}
					}
					if (kminus < band2)		rnew = rmed;
				}
				else {/*		--- yin < rmed = yout --- */
					rse = rts = yin;/* was -rinf */
					for (i = first; i <= last; i += step) {
						yi = y[i];
						if (yi <= rmed) {
							if (yi < rmed) {
								++kminus;
								if (yi > rts)	rts = yi;
								if (yi > rse)	rse = yi;
							}
							else		rse = yi;

						}
					}
					rnew = (kminus == band2) ? rts : rse;
				}
			} /* else: both  yin, yout < rmed -- nothing to do .... */
		}
		else if (yin != rmed) { /* yin > rmed */
			if (yout <= rmed) {
				kplus = 0;
				if (yout < rmed) {/* -- yout < rmed < yin --- */
					rnew = yin; /* was rinf */
					for (i = first; i <= last; i += step) {
						yi = y[i];
						if (yi > rmed) {
							++kplus;
							if (yi < rnew)	rnew = yi;
						}
					}
					if (kplus < band2)	rnew = rmed;
				}
				else { /* -- yout = rmed < yin --- */
					rbe = rtb = yin; /* was rinf */
					for (i = first; i <= last; i += step) {
						yi = y[i];
						if (yi >= rmed) {
							if (yi > rmed) {
								++kplus;
								if (yi < rtb)	rtb = yi;
								if (yi < rbe)	rbe = yi;
							}
							else		rbe = yi;
						}
					}
					rnew = (kplus == band2) ? rtb : rbe;
				}
			} /* else: both  yin, yout > rmed --> nothing to do */
		} /* else: yin == rmed -- nothing to do .... */
		rmed = rnew;
		smo[ismo] = rmed;
		num_meds++;
	} /*     END FOR ------------ big Loop -------------------- */

	free(scrat);

	band2 = bw / 2;
	last_med = band2 * step + (num_meds - 1)*step;

	if (end_rule == 0) { /*-- keep DATA at end values */
		for (i = last_med + 1; i < n; ++i)
			smo[i] = y[i];
	}
	else { /* if(end_rule == 1)  copy median to CONSTANT end values */
		for (i = last_med + 1; i < n; ++i)
			smo[i] = smo[last_med];
	}

	if (skip > 0) {
		for (i = 0; i < num_meds - 1; i++) {
			for (j = 1; j < step; j++) {
				x = (band2 + i)*step + j;
				a = (band2 + i)*step;
				b = (band2 + i + 1)*step;
				fa = smo[a];
				fb = smo[b];
				smo[x] = fa + j * (fb - fa) / step; //b-a=step, x-a=j
			}
		}
	}
	//memcpy(smo, smo + bw_tot, n * sizeof(float));
	for (i = 0; i < n; i++)
	{
		v = smo[i + bw_tot];
		smo[i] = v;
	}
} /* SrunmedSLIExt */

void SrunmedSLI(float* y, float* smo, long n, int bw_tot, int end_rule, int skip) // 2022.08.01 - running median with skipping [skip] values, with linear interpolation
{
	/*
		 *  Computes "Running Median" smoother ("Stuetzle" algorithm) with medians of 'band'

		 *  Input:
		 *	y(n)	- responses in order of increasing predictor values
		 *	n	- number of observations
		 *	bw_tot	- span of running medians (MUST be ODD !!)
		 *	end_rule -- 0: Keep original data at ends {j; j < b2 | j > n-b2}
		 *		     -- 1: Constant ends = median(y[1],..,y[bw]) "robust"
		 *  Output:
		 *	smo(n)	- smoothed responses

		 * NOTE:  The 'end' values are just copied !! this is fast but not too nice !
		 */

		 /* Local variables */
	double rmed, rmin, temp, rnew, yout, yi;
	double rbe, rtb, rse, yin, rts, fa, fb;
	int imin, ismo, i, j, first, last, band2, kminus, kplus, shift, bw, step, num_meds, last_med, a, b, x;

	step = skip + 1;
	bw = (((bw_tot - 1) / 2) / step) * 2 + 1;
	if (n < bw_tot)
		return;
	//std::cout << "new: bw=" << bw << ", radius=" << bw / 2 << ", step=" << step << "\n";

	if (n < bw)
		return;
	//double *scrat = (double *)malloc(bw, sizeof(double));
	//was
	double *scrat = (double *)malloc((unsigned)bw * sizeof(double));

	//if ((bw-1)*step+1 > n)
	//	error(_("bandwidth/span of running medians is larger than n"));

	/* 1. Compute  'rmed' := Median of the first 'band' values
	   ======================================================== */

	shift = 0;

	for (i = 0; i < bw; ++i)
		scrat[i] = y[i*step + shift];

	/* find minimal value  rmin = scrat[imin] <= scrat[j] */
	rmin = scrat[0]; imin = 0;
	for (i = 1; i < bw; ++i)
		if (scrat[i] < rmin) {
			rmin = scrat[i]; imin = i;
		}
	/* swap scrat[0] <-> scrat[imin] */
	temp = scrat[0]; scrat[0] = rmin; scrat[imin] = temp;
	/* sort the rest of of scrat[] by bubble (?) sort -- */
	for (i = 2; i < bw; ++i) {
		if (scrat[i] < scrat[i - 1]) {/* find the proper place for scrat[i] */
			temp = scrat[i];
			j = i;
			do {
				scrat[j] = scrat[j - 1];
				--j;
			} while (scrat[j - 1] > temp); /* now:  scrat[j-1] <= temp */
			scrat[j] = temp;
		}
	}
	band2 = bw / 2;
	rmed = scrat[band2];/* == Median( y[(1:band2)-1] ) */
	/* "malloc" had  free( (char*) scrat);*/ /*-- release scratch memory --*/
	if (shift == 0) {
		if (end_rule == 0) { /*-- keep DATA at end values */
			for (i = 0; i < band2*step; ++i)
				smo[i] = y[i];
		}
		else { /* if(end_rule == 1)  copy median to CONSTANT end values */
			for (i = 0; i < band2*step; ++i)
				smo[i] = rmed;
		}
	}
	smo[band2*step + shift] = rmed;
	num_meds = 1;
	band2++; /* = bw / 2 + 1*/;

	/* Big	FOR Loop: RUNNING median, update the median 'rmed'
	   ======================================================= */
	for (first = step + shift, last = bw * step + shift, ismo = band2 * step + shift;
		last < n;
		first += step, last += step, ismo += step) {

		yin = y[last];
		yout = y[first - step];
		rnew = rmed; /* New median = old one   in all the simple cases --*/

		if (yin < rmed) {
			if (yout >= rmed) {
				kminus = 0;
				if (yout > rmed) {/*	--- yin < rmed < yout --- */
					//if (print_level >= 2) REprintf(": yin < rmed < yout ");
					rnew = yin;/* was -rinf */
					for (i = first; i <= last; i += step) {
						yi = y[i];
						if (yi < rmed) {
							++kminus;
							if (yi > rnew)	rnew = yi;
						}
					}
					if (kminus < band2)		rnew = rmed;
				}
				else {/*		--- yin < rmed = yout --- */
					rse = rts = yin;/* was -rinf */
					for (i = first; i <= last; i += step) {
						yi = y[i];
						if (yi <= rmed) {
							if (yi < rmed) {
								++kminus;
								if (yi > rts)	rts = yi;
								if (yi > rse)	rse = yi;
							}
							else		rse = yi;

						}
					}
					rnew = (kminus == band2) ? rts : rse;
				}
			} /* else: both  yin, yout < rmed -- nothing to do .... */
		}
		else if (yin != rmed) { /* yin > rmed */
			if (yout <= rmed) {
				kplus = 0;
				if (yout < rmed) {/* -- yout < rmed < yin --- */
					rnew = yin; /* was rinf */
					for (i = first; i <= last; i += step) {
						yi = y[i];
						if (yi > rmed) {
							++kplus;
							if (yi < rnew)	rnew = yi;
						}
					}
					if (kplus < band2)	rnew = rmed;
				}
				else { /* -- yout = rmed < yin --- */
					rbe = rtb = yin; /* was rinf */
					for (i = first; i <= last; i += step) {
						yi = y[i];
						if (yi >= rmed) {
							if (yi > rmed) {
								++kplus;
								if (yi < rtb)	rtb = yi;
								if (yi < rbe)	rbe = yi;
							}
							else		rbe = yi;
						}
					}
					rnew = (kplus == band2) ? rtb : rbe;
				}
			} /* else: both  yin, yout > rmed --> nothing to do */
		} /* else: yin == rmed -- nothing to do .... */
		rmed = rnew;
		smo[ismo] = rmed;
		num_meds++;
	} /*     END FOR ------------ big Loop -------------------- */

	free(scrat);

	band2 = bw / 2;
	last_med = band2 * step + (num_meds - 1)*step;

	if (end_rule == 0) { /*-- keep DATA at end values */
		for (i = last_med + 1; i < n; ++i)
			smo[i] = y[i];
	}
	else { /* if(end_rule == 1)  copy median to CONSTANT end values */
		for (i = last_med + 1; i < n; ++i)
			smo[i] = smo[last_med];
	}

	if (skip > 0) {
		for (i = 0; i < num_meds - 1; i++) {
			for (j = 1; j < step; j++) {
				x = (band2 + i)*step + j;
				a = (band2 + i)*step;
				b = (band2 + i + 1)*step;
				fa = smo[a];
				fb = smo[b];
				smo[x] = fa + j * (fb - fa) / step; //b-a=step, x-a=j
			}
		}
	}
} /* SrunmedSLI */

void SrunmedMSet(float* y, float* smo, long n, int bw, int end_rule) // 2022.07.29 - running median using multiset
{
	std::multiset<float> window;
	int i, k, radius;
	if (n < bw)
		return;
	radius = bw / 2;

	for (k = 0; k < bw; k++) {
		window.insert(y[k]);
	}

	auto mid = next(window.begin(), radius);
	smo[radius] = *mid;

	for (i = bw; i < n; i++) {
		window.insert(y[i]);

		if (y[i] < *mid)
			mid--;

		if (y[i - bw] <= *mid)
			mid++;

		window.erase(window.lower_bound(y[i - bw]));

		smo[i - radius] = *mid;
	}


	if (end_rule == 0) { /*-- keep DATA at end values */
		for (i = 0; i < radius; ++i)
			smo[i] = y[i];
		for (i = n - radius; i < n; ++i)
			smo[i] = y[i];
	}
	else { /* if(end_rule == 1)  copy median to CONSTANT end values */
		for (i = 0; i < radius; ++i)
			smo[i] = smo[radius];
		for (i = n - radius; i < n; ++i)
			smo[i] = smo[n - radius - 1];
	}

	window.clear();
}



void Srunmed(float* y, float* smo, long n, int bw,	int end_rule)
{
	/*
	 *  Computes "Running Median" smoother ("Stuetzle" algorithm) with medians of 'band'

	 *  Input:
	 *	y(n)	- responses in order of increasing predictor values
	 *	n	- number of observations
	 *	bw	- span of running medians (MUST be ODD !!)
	 *	end_rule -- 0: Keep original data at ends {j; j < b2 | j > n-b2}
	 *		     -- 1: Constant ends = median(y[1],..,y[bw]) "robust"
	 *  Output:
	 *	smo(n)	- smoothed responses

	 * NOTE:  The 'end' values are just copied !! this is fast but not too nice !
	 */

	 /* Local variables */
	double rmed, rmin, temp, rnew, yout, yi;
	double rbe, rtb, rse, yin, rts;
	int imin, ismo, i, j, first, last, band2, kminus, kplus;

	if (n < bw)
		return;
	//double *scrat = (double *)malloc(bw, sizeof(double));
	//was  
	double *scrat = (double *)malloc((unsigned)bw * sizeof(double));

	//if (bw > n)
	//	error(_("bandwidth/span of running medians is larger than n"));

	/* 1. Compute  'rmed' := Median of the first 'band' values
	   ======================================================== */

	for (int i = 0; i < bw; ++i)
		scrat[i] = y[i];

	/* find minimal value  rmin = scrat[imin] <= scrat[j] */
	rmin = scrat[0]; imin = 0;
	for (int i = 1; i < bw; ++i)
		if (scrat[i] < rmin) {
			rmin = scrat[i]; imin = i;
		}
	/* swap scrat[0] <-> scrat[imin] */
	temp = scrat[0]; scrat[0] = rmin; scrat[imin] = temp;
	/* sort the rest of of scrat[] by bubble (?) sort -- */
	for (int i = 2; i < bw; ++i) {
		if (scrat[i] < scrat[i - 1]) {/* find the proper place for scrat[i] */
			temp = scrat[i];
			j = i;
			do {
				scrat[j] = scrat[j - 1];
				--j;
			} while (scrat[j - 1] > temp); /* now:  scrat[j-1] <= temp */
			scrat[j] = temp;
		}
	}
	band2 = bw / 2;
	rmed = scrat[band2];/* == Median( y[(1:band2)-1] ) */
	/* "malloc" had  free( (char*) scrat);*/ /*-- release scratch memory --*/

	if (end_rule == 0) { /*-- keep DATA at end values */
		for (i = 0; i < band2; ++i)
			smo[i] = y[i];
	}
	else { /* if(end_rule == 1)  copy median to CONSTANT end values */
		for (i = 0; i < band2; ++i)
			smo[i] = rmed;
	}
	smo[band2] = rmed;
	band2++; /* = bw / 2 + 1*/;

		/* Big	FOR Loop: RUNNING median, update the median 'rmed'
		   ======================================================= */
	for (first = 1, last = bw, ismo = band2;
		last < n;
		++first, ++last, ++ismo) {

		yin = y[last];
		yout = y[first - 1];
		rnew = rmed; /* New median = old one   in all the simple cases --*/

		if (yin < rmed) {
			if (yout >= rmed) {
				kminus = 0;
				if (yout > rmed) {/*	--- yin < rmed < yout --- */
					//if (print_level >= 2) REprintf(": yin < rmed < yout ");
					rnew = yin;/* was -rinf */
					for (i = first; i <= last; ++i) {
						yi = y[i];
						if (yi < rmed) {
							++kminus;
							if (yi > rnew)	rnew = yi;
						}
					}
					if (kminus < band2)		rnew = rmed;
				}
				else {/*		--- yin < rmed = yout --- */
					rse = rts = yin;/* was -rinf */
					for (i = first; i <= last; ++i) {
						yi = y[i];
						if (yi <= rmed) {
							if (yi < rmed) {
								++kminus;
								if (yi > rts)	rts = yi;
								if (yi > rse)	rse = yi;
							}
							else		rse = yi;

						}
					}
					rnew = (kminus == band2) ? rts : rse;
				}
			} /* else: both  yin, yout < rmed -- nothing to do .... */
		}
		else if (yin != rmed) { /* yin > rmed */
			if (yout <= rmed) {
				kplus = 0;
				if (yout < rmed) {/* -- yout < rmed < yin --- */
					rnew = yin; /* was rinf */
					for (i = first; i <= last; ++i) {
						yi = y[i];
						if (yi > rmed) {
							++kplus;
							if (yi < rnew)	rnew = yi;
						}
					}
					if (kplus < band2)	rnew = rmed;
				}
				else { /* -- yout = rmed < yin --- */
					rbe = rtb = yin; /* was rinf */
					for (i = first; i <= last; ++i) {
						yi = y[i];
						if (yi >= rmed) {
							if (yi > rmed) {
								++kplus;
								if (yi < rtb)	rtb = yi;
								if (yi < rbe)	rbe = yi;
							}
							else		rbe = yi;
						}
					}
					rnew = (kplus == band2) ? rtb : rbe;
				}
			} /* else: both  yin, yout > rmed --> nothing to do */
		} /* else: yin == rmed -- nothing to do .... */
		rmed = rnew;
		smo[ismo] = rmed;
	} /*     END FOR ------------ big Loop -------------------- */

	if (end_rule == 0) { /*-- keep DATA at end values */
		for (i = ismo; i < n; ++i)
			smo[i] = y[i];
	}
	else { /* if(end_rule == 1)  copy median to CONSTANT end values */
		for (i = ismo; i < n; ++i)
			smo[i] = rmed;
	}
	free(scrat);
} /* Srunmed */


///   Running median for CUDA
__global__ void SrunmedG(float* y, float* smo, long n, int bw, int end_rule)
{
	/*
	 *  Computes "Running Median" smoother ("Stuetzle" algorithm) with medians of 'band'

	 *  Input:
	 *	y(n)	- responses in order of increasing predictor values
	 *	n	- number of observations
	 *	bw	- span of running medians (MUST be ODD !!)
	 *	end_rule -- 0: Keep original data at ends {j; j < b2 | j > n-b2}
	 *		     -- 1: Constant ends = median(y[1],..,y[bw]) "robust"
	 *  Output:
	 *	smo(n)	- smoothed responses

	 * NOTE:  The 'end' values are just copied !! this is fast but not too nice !
	 */

	 /* Local variables */
	double rmed, rmin, temp, rnew, yout, yi;
	double rbe, rtb, rse, yin, rts;
	int imin, ismo, i, j, first, last, band2, kminus, kplus;


	//double *scrat = (double *)malloc(bw, sizeof(double));
	//was  
	double *scrat = (double *)malloc((unsigned)bw * sizeof(double));

	//if (bw > n)
	//	error(_("bandwidth/span of running medians is larger than n"));

	/* 1. Compute  'rmed' := Median of the first 'band' values
	   ======================================================== */

	for (int i = 0; i < bw; ++i)
		scrat[i] = y[i];

	/* find minimal value  rmin = scrat[imin] <= scrat[j] */
	rmin = scrat[0]; imin = 0;
	for (int i = 1; i < bw; ++i)
		if (scrat[i] < rmin) {
			rmin = scrat[i]; imin = i;
		}
	/* swap scrat[0] <-> scrat[imin] */
	temp = scrat[0]; scrat[0] = rmin; scrat[imin] = temp;
	/* sort the rest of of scrat[] by bubble (?) sort -- */
	for (int i = 2; i < bw; ++i) {
		if (scrat[i] < scrat[i - 1]) {/* find the proper place for scrat[i] */
			temp = scrat[i];
			j = i;
			do {
				scrat[j] = scrat[j - 1];
				--j;
			} while (scrat[j - 1] > temp); /* now:  scrat[j-1] <= temp */
			scrat[j] = temp;
		}
	}
	band2 = bw / 2;
	rmed = scrat[band2];/* == Median( y[(1:band2)-1] ) */
	/* "malloc" had  free( (char*) scrat);*/ /*-- release scratch memory --*/

	if (end_rule == 0) { /*-- keep DATA at end values */
		for (i = 0; i < band2; ++i)
			smo[i] = y[i];
	}
	else { /* if(end_rule == 1)  copy median to CONSTANT end values */
		for (i = 0; i < band2; ++i)
			smo[i] = rmed;
	}
	smo[band2] = rmed;
	band2++; /* = bw / 2 + 1*/;

	/* Big	FOR Loop: RUNNING median, update the median 'rmed'
	   ======================================================= */
	for (first = 1, last = bw, ismo = band2;
		last < n;
		++first, ++last, ++ismo) {

		yin = y[last];
		yout = y[first - 1];
		rnew = rmed; /* New median = old one   in all the simple cases --*/

		if (yin < rmed) {
			if (yout >= rmed) {
				kminus = 0;
				if (yout > rmed) {/*	--- yin < rmed < yout --- */
					//if (print_level >= 2) REprintf(": yin < rmed < yout ");
					rnew = yin;/* was -rinf */
					for (i = first; i <= last; ++i) {
						yi = y[i];
						if (yi < rmed) {
							++kminus;
							if (yi > rnew)	rnew = yi;
						}
					}
					if (kminus < band2)		rnew = rmed;
				}
				else {/*		--- yin < rmed = yout --- */
					rse = rts = yin;/* was -rinf */
					for (i = first; i <= last; ++i) {
						yi = y[i];
						if (yi <= rmed) {
							if (yi < rmed) {
								++kminus;
								if (yi > rts)	rts = yi;
								if (yi > rse)	rse = yi;
							}
							else		rse = yi;

						}
					}
					rnew = (kminus == band2) ? rts : rse;
				}
			} /* else: both  yin, yout < rmed -- nothing to do .... */
		}
		else if (yin != rmed) { /* yin > rmed */
			if (yout <= rmed) {
				kplus = 0;
				if (yout < rmed) {/* -- yout < rmed < yin --- */
					rnew = yin; /* was rinf */
					for (i = first; i <= last; ++i) {
						yi = y[i];
						if (yi > rmed) {
							++kplus;
							if (yi < rnew)	rnew = yi;
						}
					}
					if (kplus < band2)	rnew = rmed;
				}
				else { /* -- yout = rmed < yin --- */
					rbe = rtb = yin; /* was rinf */
					for (i = first; i <= last; ++i) {
						yi = y[i];
						if (yi >= rmed) {
							if (yi > rmed) {
								++kplus;
								if (yi < rtb)	rtb = yi;
								if (yi < rbe)	rbe = yi;
							}
							else		rbe = yi;
						}
					}
					rnew = (kplus == band2) ? rtb : rbe;
				}
			} /* else: both  yin, yout > rmed --> nothing to do */
		} /* else: yin == rmed -- nothing to do .... */
		rmed = rnew;
		smo[ismo] = rmed;
	} /*     END FOR ------------ big Loop -------------------- */

	if (end_rule == 0) { /*-- keep DATA at end values */
		for (i = ismo; i < n; ++i)
			smo[i] = y[i];
	}
	else { /* if(end_rule == 1)  copy median to CONSTANT end values */
		for (i = ismo; i < n; ++i)
			smo[i] = rmed;
	}
} /* SrunmedG */

void SrunmedG_CUDA_Launch(int pn, int cgpu, int n, int bw, int end_rule)
{
	dim3 dimBlock(BLOCK_SIZE);
	dim3 dimGrid((pn + 1) / dimBlock.x);

	int numBlock = (pn + 1) / ThrPerBlock;

	cudaSetDevice(cgpu);
	cudaError_t Cerror = cudaGetLastError();

	cudaMallocManaged((void**)&d_y, n * sizeof(float));
	cudaMallocManaged((void**)&d_smo, n * sizeof(float));

	cudaMemcpy(d_y, pP3x, n * sizeof(float), cudaMemcpyHostToDevice);

	SrunmedG << <numBlock, ThrPerBlock >> > (d_y, d_smo, n, bw, end_rule);
	if ((Cerror = cudaGetLastError()))
		Beep(2000, 100);
	//cudaThreadSynchronize();
	cudaDeviceSynchronize();

	// check for error
	Cerror = cudaGetLastError();

}

__device__ void FilterRMedG(float *pw, int *pwg, float *medo, float *pXC, float *pYC, int n, int sz1, float mprc, float sprc, float mabs, float pabs, int dlen, float *pDefMB, float *pXdefMB, float *pYdefMB, float *pDefSP, float *pXdefSP, float *pYdefSP, int *MBnum, int *SPnum, int mode)
{	/// processing of single seleton part of length=n, RMed window=sz1
	/// mode==0  - not using CAD width

	int j;
	int spnum = 0, mbnum = 0;
	//	float *medo;
	int bw = sz1;

	float w, wg, wn;
	float dg, dgprc; // deviation from CAD
	float dn, dnprc; // deviation from neighbor
	float def; // defect size
	if (n == 0)
	{
		*SPnum = spnum;
		*MBnum = mbnum;
		return;
	}

	//medo = (float *)malloc(n * sizeof(float));
//	SrunmedG(pw, medo, n, bw, 0); // Running median GPU

	for (j = 0; j < n - 1; j++)
	{
		w = *(pw + j);
		wg = *(pwg + j);
		wn = *(medo + j);

		dg = w - wg;
		dn = w - wn;

		dgprc = dg * 100. / wg;
		dnprc = dn * 100. / (wn + 0.001);

		def = 0.;
		/*if (dgprc >= sprc)
			def = dg;*/
		if (dnprc >= sprc)
			def = dn;

		/*if (dgprc <= -mprc)
			def = dg;*/
		if (dnprc <= -mprc)
			def = dn;

		if (def > 0.01)
		{
			*(pDefSP++) = def;
			*(pXdefSP++) = *(pXC + j);
			*(pYdefSP++) = *(pYC + j);
			spnum++;
		}
		else if (def < -0.01)
		{
			*(pDefMB++) = def;
			*(pXdefMB++) = *(pXC + j);
			*(pYdefMB++) = *(pYC + j);
			mbnum++;
		}
	}
	*SPnum = spnum;
	*MBnum = mbnum;
	//free(medo);
}


void FilterRMedNFWM(float *pwN, float *medo, float *pXCN, float *pYCN, int n, int sz1, int skip, float mprcN, float sprcN, float mabsN, float pabsN, float *pDefMBN, float *pXdefMBN, float *pYdefMBN, float *pDefSPN, float *pXdefSPN, float *pYdefSPN, int *MBnumN, int *SPnumN, int modeN)
{	/// processing of single NFWM contour of length=n, RMed window=sz1
	/// modeN=0  - not using CAD width

	int j;
	int spnum = 0, mbnum = 0;
	//	float *medo;
	int bw = sz1;

	float w, wn;
	float dg; // deviation from CAD
	float dn; // deviation from neighbor
	float def; // defect size
	if (n == 0 || n > 1000000)
	{
		*SPnumN = spnum;
		*MBnumN = mbnum;
		return;
	}

	//medo = (float *)malloc(n * sizeof(float));
	//Srunmed(pwN, medo, n, bw, 0); // Running median
	SrunmedSLI(pwN, medo, n, bw, 0, skip); // Running median with skip and linear interpolation
	for (j = 0; j < n - 1; j++)
	{
		w = *(pwN + j);
		wn = *(medo + j);

		dg = w;
		dn = w - wn;

		/*dgprc = dg * 100. / wg;
		dnprc = dn * 100. / (wn + 0.001);*/

		def = 0.;
		if (dg >= pabsN)
			def = dg;
		if (dn >= sprcN)
			def = dn;

		if (dg <= -mabsN)
			def = dg;
		if (dn <= -mprcN)
			def = dn;

		if (def > 0.01)
		{
			*(pDefSPN++) = def;
			*(pXdefSPN++) = *(pXCN + j);
			*(pYdefSPN++) = *(pYCN + j);
			spnum++;
		}
		else if (def < -0.01)
		{
			*(pDefMBN++) = def;
			*(pXdefMBN++) = *(pXCN + j);
			*(pYdefMBN++) = *(pYCN + j);
			mbnum++;
		}
	}
	*SPnumN = spnum;
	*MBnumN = mbnum;
}


float median(float *pa, int n)
{
	float t, median;
	
	/* Sorting begins */
	for (int i = 1; i <= n - 1; i++) { /* Trip-i begins */
		for (int j = 1; j <= n - i; j++) {
			if (*(pa + j) <= *(pa + j+1)) { /* Interchanging values */
				t = *(pa + j);
				*(pa + j) = *(pa + j+1);
				*(pa + j+1) = t;
			}
			else
				continue;
		}
	} /* sorting ends */
	/* calculation of median */
	if (n % 2 == 0)
		median = (*(pa + n/2) + *(pa + n / 2 + 1)) / 2.0;
	else
		median = *(pa + n / 2 + 1);

	return(median);
}

int compare(const void* a, const void* b)
{
	const int* x = (int*)a;
	const int* y = (int*)b;

	if (*x > *y)
		return 1;
	else if (*x < *y)
		return -1;

	return 0;
}

void FilterRMed(float *pw, int *pwg, float *medo, float *pXC, float *pYC, int n, int sz1, int skip, float mprc, float sprc, float mabs, float pabs, int dlen, float *pDefMB, float *pXdefMB, float *pYdefMB, float *pDefSP, float *pXdefSP, float *pYdefSP, int *MBnum, int *SPnum, int mode)
{	/// processing of single seleton part of length=n, RMed window=sz1
	/// mode==0  - not using CAD width
	
	int j;
	int spnum = 0, mbnum = 0;
//	float *medo;
	int bw = min(sz1, n);
	int wgmed;
	int *pbuf;

	float prcG, prcReal;

	float w, wg, wn;
	float dg, dgprc; // deviation from CAD
	float dn, dnprc; // deviation from neighbor
	float def; // defect size
	float defw; // defect width
	if (n == 0)
	{
		*SPnum = spnum;
		*MBnum = mbnum;
		return;
	}
	
	//medo = (float *)malloc(n * sizeof(float));
	//Srunmed(pw, medo, n, bw, 0); // Running median
	//Srunmed(pw, medo, n, bw, 1);
	//SrunmedS(pw, medo, n, bw, 0, 4); // Running median with skip
	//SrunmedSLI(pw, medo, n, bw, 1, skip); // Running median with skip and linear interp.
	//float *buf=(float *)malloc((n+2*bw)*sizeof(float));
	//SrunmedSLIExt(pw,  medo+MEDOSIZE/2, medo, n, bw, 1, skip); // Running median with Extension at the ends, skip and linear interp.
	SrunmedSLIExtNoS(pw, medo + MEDOSIZE / 2, medo, n, bw, skip, 10); // Running median with Extension at the ends, skip and linear interp (with skip at ends).

	for (j = 0; j < n - 1; j++)
	{
		w  = *(pw + j);
		wg = *(pwg + j);
		wn = *(medo + j);

		dg = w - wg;
		dn = w - wn;

		dgprc = dg * 100. / wg;
		dnprc = dn * 100. / (wn+0.001);
		
		def = 0.;
		/*if (dgprc >= sprc)
			def = dg;*/
		if (dnprc >= sprc)
			def = dn;
		
		/*if (dgprc <= -mprc)
			def = dg;*/
		if (dnprc <= -mprc)
			def = dn;

		if (def > 0.01)
		{
			//int *pbuf = malloc((size_t)(2 * bw + 1));
			pbuf = (int *)malloc(sizeof(int) * (2 * bw + 1));
			memcpy(pbuf, pwg + j - bw, 2 * bw + 1);
			qsort(pbuf, 2*bw+1, sizeof(int), compare);
			wgmed = pbuf[bw];
			free(pbuf);

			prcG = (wg - wgmed) * 100.0 / (wgmed + 0.001);
			// prcG shouldn't increase prcReal, changed 31.07.2023
			if (prcG < 0.)
				prcG = 0.;
			prcReal = dnprc - prcG;
			if (prcReal >= sprc)
			{
				*(pDefSP++) = def;
				*(pXdefSP++) = *(pXC + j - 1);
				*(pYdefSP++) = *(pYC + j - 1);
				spnum++;
			}
			
		}
		else if (def < -0.01)
		{
			pbuf = (int *)malloc(sizeof(int) * (2 * bw + 1));
			memcpy(pbuf, pwg + j - bw, 2 * bw + 1);
			qsort(pbuf, 2 * bw + 1, sizeof(int), compare);
			wgmed = pbuf[bw];
			free(pbuf);

			prcG = (wg - wgmed) * 100.0 / (wgmed + 0.001);
			// prcGS shouldn't increase prcReal, changed 31.07.2023
			if (prcG > 0.)
				prcG = 0.;
			prcReal = dnprc - prcG;
			
			if (prcReal <= mprc)
			{
				int ju, jd;
				for (ju = 1; ju < 10; ju++)
				{
					w = *(pw + j+ju);
					wn = *(medo + j+ju);
					dn = w - wn;
					dnprc = dn * 100. / (wn + 0.001);
					if (dnprc < 0.5*mprc)
						break;
				}
				for (jd = 1; jd < 10; jd++)
				{
					w = *(pw + j - jd);
					wn = *(medo + j - jd);
					dn = w - wn;
					dnprc = dn * 100. / (wn + 0.001);
					if (dnprc < 0.5*mprc)
						break;
				}
				defw = ju + jd;

				*(pDefMB++) = def;
				*(pXdefMB++) = *(pXC + j - 1);
				*(pYdefMB++) = *(pYC + j - 1);
				mbnum++;
			}
		}
	}
	*SPnum = spnum;
	*MBnum = mbnum;
	//free(medo);
	//free(buf);
}

// 24.08.2023 - version with checking MB and Spur width
void FilterRMedW(float *pw, int *pwg, float *medo, float *pXC, float *pYC, int n, int sz1, int skip, float mprc, float sprc, float mprcw, float sprcw, float mbwmin, float spwmin, float mabs, float pabs, int dlen, float *pDefMB, float *pXdefMB, float *pYdefMB, float *pDefSP, float *pXdefSP, float *pYdefSP, int *MBnum, int *SPnum, int mode)
{	/// processing of single seleton part of length=n, RMed window=sz1
	/// mode==0  - not using CAD width
	//  mprcw, sprcw % for Spur and MB to detect width
	//  mbwmin,  spwmin minimal width for MB and Spur
	int j;
	int spnum = 0, mbnum = 0;
	//	float *medo;
	int bw = min(sz1, n);
	int wgmed;
	int *pbuf;

	float prcG, prcReal;

	float w, wg, wn;
	float dg, dgprc; // deviation from CAD
	float dn, dnprc; // deviation from neighbor
	float def; // defect size
	float defw; // defect width
	if (n == 0)
	{
		*SPnum = spnum;
		*MBnum = mbnum;
		return;
	}

	//medo = (float *)malloc(n * sizeof(float));
	//Srunmed(pw, medo, n, bw, 0); // Running median
	//Srunmed(pw, medo, n, bw, 1);
	//SrunmedS(pw, medo, n, bw, 0, 4); // Running median with skip
	//SrunmedSLI(pw, medo, n, bw, 1, skip); // Running median with skip and linear interp.
	//float *buf=(float *)malloc((n+2*bw)*sizeof(float));
	//SrunmedSLIExt(pw,  medo+MEDOSIZE/2, medo, n, bw, 1, skip); // Running median with Extension at the ends, skip and linear interp.
	SrunmedSLIExtNoS(pw, medo + MEDOSIZE / 2, medo, n, bw, skip, 10); // Running median with Extension at the ends, skip and linear interp (with skip at ends).

	for (j = 0; j < n - 1; j++)
	{
		w = *(pw + j);
		wg = *(pwg + j);
		wn = *(medo + j);

		dg = w - wg;
		dn = w - wn;

		dgprc = dg * 100. / wg;
		dnprc = dn * 100. / (wn + 0.001);

		def = 0.;
		/*if (dgprc >= sprc)
			def = dg;*/
		if (dnprc >= sprc)
			def = dn;

		/*if (dgprc <= -mprc)
			def = dg;*/
		if (dnprc <= -mprc)
			def = dn;

		if (def > 0.01) 
		{
			//int *pbuf = malloc((size_t)(2 * bw + 1));
			pbuf = (int *)malloc(sizeof(int) * (2 * bw + 1));
			memcpy(pbuf, pwg + j - bw, 2 * bw + 1);
			qsort(pbuf, 2 * bw + 1, sizeof(int), compare);
			wgmed = pbuf[bw];
			free(pbuf);

			prcG = (wg - wgmed) * 100.0 / (wgmed + 0.001);
			// prcG shouldn't increase prcReal, changed 31.07.2023
			if (prcG < 0.)
				prcG = 0.;
			prcReal = dnprc - prcG;
			if (prcReal >= sprc) // Spur check
			{
				int ju, jd;
				for (ju = 1; ju < 10; ju++)
				{
					w = *(pw + j + ju);
					wn = *(medo + j + ju);
					dn = w - wn;
					dnprc = dn * 100. / (wn + 0.001);
					if (dnprc < sprcw)
						break;
				}
				for (jd = 1; jd < 10; jd++)
				{
					w = *(pw + j - jd);
					wn = *(medo + j - jd);
					dn = w - wn;
					dnprc = dn * 100. / (wn + 0.001);
					if (dnprc < sprcw)
						break;
				}
				defw = ju + jd - 1;
				if (defw > spwmin)
				{
					*(pDefSP++) = def;
					*(pXdefSP++) = *(pXC + j - 1);
					*(pYdefSP++) = *(pYC + j - 1);
					spnum++;
				}
				
			}

		}
		else if (def < -0.01)
		{
			pbuf = (int *)malloc(sizeof(int) * (2 * bw + 1));
			memcpy(pbuf, pwg + j - bw, 2 * bw + 1);
			qsort(pbuf, 2 * bw + 1, sizeof(int), compare);
			wgmed = pbuf[bw];
			free(pbuf);

			prcG = (wg - wgmed) * 100.0 / (wgmed + 0.001);
			// prcGS shouldn't increase prcReal, changed 31.07.2023
			if (prcG > 0.)
				prcG = 0.;
			prcReal = dnprc - prcG;

			if (prcReal <= mprc) // MB check
			{
				int ju, jd;
				for (ju = 1; ju < 10; ju++)
				{
					w = *(pw + j + ju);
					wn = *(medo + j + ju);
					dn = w - wn;
					dnprc = dn * 100. / (wn + 0.001);
					if (dnprc > mprcw)
						break;
				}
				for (jd = 1; jd < 10; jd++)
				{
					w = *(pw + j - jd);
					wn = *(medo + j - jd);
					dn = w - wn;
					dnprc = dn * 100. / (wn + 0.001);
					if (dnprc > mprcw)
						break;
				}
				defw = ju + jd-1;
				if (defw > mbwmin)
				{
					*(pDefMB++) = def;
					*(pXdefMB++) = *(pXC + j - 1);
					*(pYdefMB++) = *(pYC + j - 1);
					mbnum++;
				}

				
			}
		}
	}
	*SPnum = spnum;
	*MBnum = mbnum;
	//free(medo);
	//free(buf);
}




void FilterRMedS(float *pw, int *pwg, float *medo, float *pXC, float *pYC, int n, int sz1, int skip, float mprc, float sprc, float mabs, float pabs, int dlen, float *pDefMB, float *pXdefMB, float *pYdefMB, float *pDefSP, float *pXdefSP, float *pYdefSP, int *MBnum, int *SPnum, int mode)
{	/// processing of single skeleton part of length=n, RMed window=sz1
	/// mode==0  - not using CAD width

	int j;
	int spnum = 0, mbnum = 0;

	int bw = min(sz1, n/2);
	int wgmed;
	int *pbuf;
	float prcGS, prcReal;

	float w, wg, wn;
	float dg, dgprc; // deviation from CAD
	float dn, dnprc; // deviation from neighbor
	float def; // defect size
	if (n == 0)
	{
		*SPnum = spnum;
		*MBnum = mbnum;
		return;
	}

	

	//medo = (float *)malloc(n * sizeof(float));
	//Srunmed(pw, medo, n, bw, 0); // Running median
	//Srunmed(pw, medo, n, bw, 1);
	//SrunmedS(pw, medo, n, bw, 0, 4); // Running median with skip
	SrunmedSLI(pw, medo, n, bw, 1, skip); // Running median with skip and linear interp.

	for (j = 0; j < n - 1; j++)
	{
		w = *(pw + j);
		wg = *(pwg + j);
		wn = *(medo + j);

		dg = w - wg;
		dn = w - wn;

		dgprc = dg * 100. / wg;
		dnprc = dn * 100. / (wn + 0.001);

		def = 0.;
		/*if (dgprc >= sprc)
			def = dg;*/
		/*if (dnprc >= sprc)
			def = dn;*/

		/*if (dgprc <= -mprc)
			def = dg;*/
		if (dnprc <= -mprc)
			def = dn;

		/*if (def > 0.01)
		{
			*(pDefSP++) = def;
			*(pXdefSP++) = *(pXC + j);
			*(pYdefSP++) = *(pYC + j);
			spnum++;
		}
		else */
		if (def < -0.01)
		{
			/**(pDefMB++) = def;
			*(pXdefMB++) = *(pXC + j-0);
			*(pYdefMB++) = *(pYC + j-0);*/
			//qsort(pwg + j+, n, sizeof(int), compare);

			pbuf = (int *)malloc(sizeof(int) * (2 * bw + 1));
			memcpy(pbuf, pwg + j - bw, 2 * bw + 1);
			qsort(pbuf, 2 * bw + 1, sizeof(int), compare);
			wgmed = pbuf[bw];
			free(pbuf);

			prcGS = (wg - wgmed) * 100.0 / (wgmed + 0.001);
			prcReal = dnprc - prcGS;

			if (prcReal <= -mprc)
			{
				*(pDefMB + mbnum) = def;
				*(pXdefMB + mbnum) = *(pXC + j - 0);
				*(pYdefMB + mbnum) = *(pYC + j - 0);
				mbnum++;
			}
			
		}
	}
	*SPnum = spnum;
	*MBnum = mbnum;
	//free(medo);
}

void FilterM6(float *pw, int *pwg, float *pXC, float *pYC, int n, int sz1, int sz2, float ang, float mprc, float sprc, float mabs, float pabs, int dlen, float *pDefMB, float *pXdefMB, float *pYdefMB, float *pDefSP, float *pXdefSP, float *pYdefSP, int *MBnum, int *SPnum, int mode, int andNOor)
{
	/// mode==0  - not usinf CAD width
	/// andNOor  - AND==1 / OR==0 /Aver==2 (for two filters)
	int j, jm, jp, jm2, jp2, jm15, jp15;
	float x, xm, xm2, xm15, xp, xp15, xp2, y, ym, ym15, ym2, yg, ymg, ymg15, ymg2, ypg, ypg15, ypg2, yp, yp15, yp2, d, tangl, d15, tangl15, d2, tangl2, ta, ta2, ta15, dm, dm15, dm2, yav, yav15, yav2, def;
	int spnum = 0, mbnum = 0;
	float tang = tan(ang*3.14159 / 180.);
	int sz15 = (sz1 + sz2) / 2;
	
	if (n == 0)
	{
		*SPnum = spnum;
		*MBnum = mbnum;
		return;
	}
	for (j = 0; j < n - 1; j++)
	{
		def = 0.;
		if ((jp = j + sz1) > n - 1)
			jp = n - 1;
		if ((jm = j - sz1) < 0)
			jm = 0;
		if ((jp2 = j + sz2) > n - 1)
			jp2 = n - 1;
		if ((jm2 = j - sz2) < 0)
			jm2 = 0;
		if ((jp15 = j + sz15) > n - 1)
			jp15 = n - 1;
		if ((jm15 = j - sz15) < 0)
			jm15 = 0;
		x = j;
		xm = jm;
		xm2 = jm2;
		xm15 = jm15;
		xp = jp;
		xp2 = jp2;

		y = *(pw + j);
		yg = *(pwg + j);

		ym = *(pw + jm);
		//ymg = *(pwg + jm);
		yp = *(pw + jp);
		//ypg = *(pwg + jp);
		//if (mode & 1 == 0)
		//	ymg = ypg = yg = 0;
		//AngleDist(y - yg, x, ym - ymg, xm, yp - ypg, xp, &d, &tangl);
		AngleDist(y, x, ym, xm, yp, xp, &d, &tangl);

		ym2 = *(pw + jm2);
		yp2 = *(pw + jp2);
		AngleDist(y, x, ym2, xm2, yp2, xp2, &d2, &tangl2);

		ym15 = *(pw + jm15);
		yp15 = *(pw + jp15);
		AngleDist(y, x, ym15, xm15, yp15, xp2, &d15, &tangl15);

		//a = abs(angl)*180. / 3.1416;
		//a2 = abs(angl2)*180. / 3.1416;
		ta = abs(tangl);
		ta2 = abs(tangl2);
		ta15 = abs(tangl15);
		/*ta = tangl;
		ta2 = tangl2;*/

		if (ta <= tang)
			dm = d;
		else dm = 0.;

		if (ta2 <= tang)
			dm2 = d2;
		else
			dm2 = 0.;

		if (ta15 <= tang)
			dm15 = d15;
		else
			dm15 = 0.;

		/*if ((ta <= tang) )
			dm = d;
		else if ((ta2 <= tang))
			dm = d2;
		else
			dm = 0;*/
		yav = (ym + yp) / 2.;
		yav2 = (ym2 + yp2) / 2.;
		yav15 = (ym15 + yp15) / 2.;
		//d = y - yav;
		//d2 = y - yav2;

		//dm = d;
		int s=0, s15 = 0, s2 = 0; // sign - 1 positive/ -1 negative
		if ((y > ym) && (y > yp))
			s = 1;
		if ((y > ym2) && (y > yp2))
			s2 = 1;
		if ((y > ym15) && (y > yp15))
			s15 = 1;
		if ((y < ym) && (y < yp))
			s = -1;
		if ((y < ym2) && (y < yp2))
			s2 = -1;
		if ((y < ym15) && (y < yp15))
			s15 = -1;

		int n = 0, n15 = 0, n2 = 0, p = 0, p15 = 0, p2 = 0; // over positive or negative thresholds
		if (dm > abs(yav)*sprc / 100.)
			p = 1;
		if (dm < -abs(yav)*mprc / 100.)
			n = 1;
		if (dm15 > abs(yav15)*sprc / 100.)
			p15 = 1;
		if(dm15 < -abs(yav15)*sprc / 100.)
			n15 = 1;
		if (dm2 > abs(yav2)*mprc / 100.)
			p2 = 1;
		if (dm2 < -abs(yav2)*mprc / 100.)
			n2 = 1;

		if ((y == 0) || (ym == 0) || (yp == 0))
			def = 0.;
		else
		{
			if ((s == 1) && (s15 == 1) || (s == 1) && (s2 == 1) || (s15 == 1) && (s2 == 1)) // spur candidate
			{
				if ((p == 1) && (p15 == 1))
				{
					if (abs(dm) > abs(dm15))
						def = dm;
					else
						def = dm15;
				}
				if ((p == 1) && (p2 == 1))
				{
					if (abs(dm) > abs(dm2))
						def = dm;
					else
						def = dm2;
				}
				if ((p15 == 1) && (p2 == 1))
				{
					if (abs(dm15) > abs(dm2))
						def = dm15;
					else
						def = dm2;
				}
			}

			if ((s == -1) && (s15 == -1) || (s == -1) && (s2 == -1) || (s15 == -1) && (s2 == -1)) // MB candidate
			{
				if ((n == 1) && (n15 == 1))
				{
					if (abs(dm) > abs(dm15))
						def = dm;
					else
						def = dm15;
				}
				if ((n == 1) && (n2 == 1))
				{
					if (abs(dm) > abs(dm2))
						def = dm;
					else
						def = dm2;
				}
				if ((n15 == 1) && (n2 == 1))
				{
					if (abs(dm15) > abs(dm2))
						def = dm15;
					else
						def = dm2;
				}
			}
		}
		if (def > 0.01)
		{
			/*if (abs(*(pXC + j) - 1350) < 50)
				j = j;*/
			*(pDefSP++) = def;
			*(pXdefSP++) = *(pXC + j);
			*(pYdefSP++) = *(pYC + j);
			spnum++;
			// *(pXC + j)>6330 && *(pXC + j)<6340 && *(pYC + j)>17730 && *(pYC + j)<17750
		}
		else if (def < -0.01)
		{
			*(pDefMB++) = def;
			*(pXdefMB++) = *(pXC + j);
			*(pYdefMB++) = *(pYC + j);
			mbnum++;
		}
	}
	*SPnum = spnum;
	*MBnum = mbnum;
}

void FilterM5(float *pw, int *pwg, float *pXC, float *pYC, int n, int sz1, int sz2, float ang, float mprc, float sprc, float mabs, float pabs, int dlen, float *pDefMB, float *pXdefMB, float *pYdefMB, float *pDefSP, float *pXdefSP, float *pYdefSP, int *MBnum, int *SPnum, int mode, int andNOor)
{
	/// mode==0  - not usinf CAD width
	/// andNOor  - AND==1 / OR==0 /Aver==2 (for two filters)
	int j, jm, jp, jm2, jp2;
	float x, xm, xm2, xp, xp2, y, ym,  ym2, yg, ymg, ymg2, ypg, ypg2, yp, yp2, d, tangl, d2, tangl2, ta, ta2, dm, dm2, yav, yav2, def;
	int spnum = 0, mbnum = 0;
	float tang = tan(ang*3.14159 / 180.);
	if (n == 0)
	{
		*SPnum = spnum;
		*MBnum = mbnum;
		return;
	}
	for (j = 0; j < n - 1; j++)
	{
		if ((jp = j + sz1) > n - 1)
			jp = n - 1;
		if ((jm = j - sz1) < 0)
			jm = 0;
		if ((jp2 = j + sz2) > n - 1)
			jp2 = n - 1;
		if ((jm2 = j - sz2) < 0)
			jm2 = 0;
		x = j;
		xm = jm;
		xm2 = jm2;
		xp = jp;
		xp2 = jp2;

		y = *(pw + j);
		yg = *(pwg + j);

		ym = *(pw + jm);
		//ymg = *(pwg + jm);
		yp = *(pw + jp);
		//ypg = *(pwg + jp);
		//if (mode & 1 == 0)
		//	ymg = ypg = yg = 0;
		//AngleDist(y - yg, x, ym - ymg, xm, yp - ypg, xp, &d, &tangl);
		AngleDist(y, x, ym, xm, yp, xp, &d, &tangl);

		ym2 = *(pw + jm2);
		//ymg2 = *(pwg + jm2);
		yp2 = *(pw + jp2);
		//ypg2 = *(pwg + jp2);
		//if (mode & 1 == 0)
		//	ymg2 = ypg2 = 0;
		AngleDist(y, x, ym2, xm2, yp2, xp2, &d2, &tangl2);

		//a = abs(angl)*180. / 3.1416;
		//a2 = abs(angl2)*180. / 3.1416;
		ta = abs(tangl);
		ta2 = abs(tangl2);
		/*ta = tangl;
		ta2 = tangl2;*/

		if (ta <= tang)
			dm = d;
		else dm = 0.;
		
		if (ta2 <= tang)
			dm2 = d2;
		else
			dm2 = 0.;

		/*if ((ta <= tang) )
			dm = d;
		else if ((ta2 <= tang))
			dm = d2;
		else
			dm = 0;*/
		yav = (ym + yp) / 2.;
		yav2 = (ym2 + yp2) / 2.;

		//d = y - yav;
		//d2 = y - yav2;

		//dm = d;
		if (andNOor == 0) // 0 (or)
		{
			if ((y == 0) || (ym == 0) || (yp == 0))
				def = 0.;
			else if ( (dm > abs(yav)*sprc / 100.) || (dm < abs(yav)*mprc / 100.))
				def = dm;
			else if ((dm2 > abs(yav2)*sprc / 100.) || (dm2 < abs(yav2)*mprc / 100.))
				def = dm2;
			else if ((dm2 > abs(yav2)*sprc / 100.) || (dm2 < abs(yav2)*mprc / 100.))
				def = dm2;
			else if (( (y - yg) / yg *100. > pabs) || ((yg - y) / yg * 100. > mabs))
			{
				def = y - yg;
			}
			else
				def = 0;// 
		}
		else // 1 (and)
		{
			if ((y == 0) || (ym == 0) || (yp == 0))
				def = 0.;
			else if ((dm > abs(yav)*sprc / 100.) && (dm2 > abs(yav2)*sprc / 100.) && ( (y > ym) && (y > yp) ) || (dm < -abs(yav)*mprc / 100.) && (dm2 < -abs(yav2)*mprc / 100.) && ((y < ym) && (y < yp)) )
			{
				if (abs(dm) > abs(dm2))
					def = dm;
				else
					def = dm2;
			}
			else if( ((y-yg)/yg * 100. >pabs) || ((yg-y)/yg * 100. > mabs) )
			{
				def = y - yg;
			}

			else
				def = 0;// 
		}

		if (def > 0.01)
		{
			if (abs(*(pXC + j) - 1350) < 50)
				j = j;
			*(pDefSP++) = def;
			*(pXdefSP++) = *(pXC + j);
			*(pYdefSP++) = *(pYC + j);
			spnum++;
		}
		else if (def < -0.01)
		{
			*(pDefMB++) = def;
			*(pXdefMB++) = *(pXC + j);
			*(pYdefMB++) = *(pYC + j);
			mbnum++;
		}
	}
	*SPnum = spnum;
	*MBnum = mbnum;
}
void FWM_PostProc(float *pw, int *pwg, float *medo, float *pXC, float *pYC, int na, int nvoid, int *pPaddr, int sz1, int sz2, int skip, float ang, float mprc, float sprc, float mabs, float sabs, int dlen, float *pDefMB, float *pXdefMB, float *pYdefMB, float *pDefSP, float *pXdefSP, float *pYdefSP, int *MBnum, int *SPnum)
{
	int caddrb, caddre, cnum;
	int mbnum, spnum , j;
	for (j = 1; j < na - 1; j++)
	{
		caddrb = *(pPaddr + j + 0);
		caddre = *(pPaddr + j + 1)-1;
		cnum = (caddre - caddrb);
		if (cnum > 20)
		{
			//int nvoid = 7;
	//		FilterM7(pw + caddrb, pwg + caddrb, pXC + caddrb, pYC + caddrb, cnum, nvoid, sz1, sz2, ang, mprc, sprc, mabs, sabs, dlen,  pDefMB + *MBnum, pXdefMB + *MBnum, pYdefMB +* MBnum, pDefSP + *SPnum, pXdefSP + *SPnum, pYdefSP + *SPnum, &mbnum, &spnum, 0,  1);
			FilterRMed(pw + caddrb, pwg + caddrb, medo, pXC + caddrb, pYC + caddrb, cnum, sz1, skip, mprc, sprc, mabs, sabs, dlen, pDefMB + *MBnum, pXdefMB + *MBnum, pYdefMB + *MBnum, pDefSP + *SPnum, pXdefSP + *SPnum, pYdefSP + *SPnum, &mbnum, &spnum, 0);
			// medo - buffer for RMed
			*SPnum += spnum;
			*MBnum += mbnum;
		}
	}
	j = j;
}
void FWMS_PostProc(float *pwS, int *pwgS, float *medoS, float *pXCS, float *pYCS, int naS, int nvoid, int *pPaddrS, int sz1, int sz2, int skip, float ang, float mprc, float sprc, float mabs, float sabs, int dlen, float *pDefMB, float *pXdefMB, float *pYdefMB, float *pDefSP, float *pXdefSP, float *pYdefSP, int *MBnumS, int *SPnumS)
{
	int caddrb, caddre, cnum;
	int mbnum, spnum, j;

	for (j = 0; j < naS - 1; j++)
	{
		caddrb = *(pPaddrS + j + 0);
		caddre = *(pPaddrS + j + 1) - 1;
		cnum = (caddre - caddrb);
		if (caddrb == 158840)
			caddrb = caddrb;
		mbnum = 0;
		spnum = 0;
		//int nvoid = 7;
//		FilterM7(pw + caddrb, pwg + caddrb, pXC + caddrb, pYC + caddrb, cnum, nvoid, sz1, sz2, ang, mprc, sprc, mabs, sabs, dlen,  pDefMB + *MBnum, pXdefMB + *MBnum, pYdefMB +* MBnum, pDefSP + *SPnum, pXdefSP + *SPnum, pYdefSP + *SPnum, &mbnum, &spnum, 0,  1);
		FilterRMedS(pwS + caddrb, pwgS + caddrb, medoS, pXCS + caddrb, pYCS + caddrb, cnum, sz2, skip, mprc, sprc, mabs, sabs, dlen, pDefMB + *MBnumS, pXdefMB + *MBnumS, pYdefMB + *MBnumS, pDefSP + *SPnumS, pXdefSP + *SPnumS, pYdefSP + *SPnumS, &mbnum, &spnum, 0);
		// medoS - buffer for RMed (SPACE)
		*SPnumS += spnum;
		*MBnumS += mbnum;
	}
	//j = j;
}

void FWMS_PostProcCPU(float *pwS, int *pwgS, float *medoS, float *pXCS, float *pYCS, int naS, int nvoid, int *pPaddrS, int sz1, int sz2, int skip, float ang, float mprc, float sprc, float mabs, float sabs, float minmbs, int dlen, float *pDefMB, float *pXdefMB, float *pYdefMB, float *pDefSP, float *pXdefSP, float *pYdefSP, int *MBnumS, int *SPnumS)
{
	int caddrb, caddre, cnum;
	int mbnum, spnum, j, jj, i;
	float v;

	//int spnum = 0, mbnum = 0;
	int wgmed;
	int *pbuf;
	float prcGS, prcReal;
	//

	float ws, wgs, wns;
	float dg, dgprc; // deviation from CAD
	float dn, dnprc; // deviation from neighbor
	float def; // defect size

	/*for (i = 0; i < 166000; i++)
	{
		if (v = *(pYCS + i) < 0)
			i = i;
	}*/
	//*MBnumS = 0;
	mbnum = 0;
	for (j = 0; j < naS - 1; j++)
	{
		caddrb = *(pPaddrS + j + 0);
		caddre = *(pPaddrS + j + 1) - 1;
		cnum = (caddre - caddrb);

		int bw = min(sz2, cnum / 2);

		/*if (caddrb == 158840)
			caddrb = caddrb;*/
		//mbnum = *MBnumS;
		//spnum = 0;
		//int nvoid = 7;
//		FilterM7(pw + caddrb, pwg + caddrb, pXC + caddrb, pYC + caddrb, cnum, nvoid, sz1, sz2, ang, mprc, sprc, mabs, sabs, dlen,  pDefMB + *MBnum, pXdefMB + *MBnum, pYdefMB +* MBnum, pDefSP + *SPnum, pXdefSP + *SPnum, pYdefSP + *SPnum, &mbnum, &spnum, 0,  1);
		//FilterRMedS(pwS + caddrb, pwgS + caddrb, medoS, pXCS + caddrb, pYCS + caddrb, cnum, sz2, skip, mprc, sprc, mabs, sabs, dlen, pDefMB + *MBnumS, pXdefMB + *MBnumS, pYdefMB + *MBnumS, pDefSP + *SPnumS, pXdefSP + *SPnumS, pYdefSP + *SPnumS, &mbnum, &spnum, 0);
		SrunmedSLI(pwS + caddrb, medoS, cnum, bw, 1, skip); // Running median with skip and linear interp.
		//SrunmedSLIExt(pwS, medoS + MEDOSIZE / 2, medoS, cnum, bw, 1, skip); // Running median with Extension at the ends, skip and linear interp.
		for (jj = 0; jj < cnum - 1; jj++)
		{
			ws = *(pwS + caddrb + jj);
			if (ws <= 3.5)
				ws = ws;
			wgs = *(pwgS + caddrb + jj);
			wns = *(medoS + jj);

			dg = ws - wgs;
			dn = ws - wns;

			dgprc = dg * 100. / wgs;
			dnprc = dn * 100. / (wns + 0.001);

			def = 0.;
			
			if (dnprc <= -mprc && abs(dn)>=minmbs)
				def = dn;

			
			if (def < -0.01)
			{
				/**(pDefMB++) = def;
				*(pXdefMB++) = *(pXC + j-0);
				*(pYdefMB++) = *(pYC + j-0);*/


				pbuf = (int *)malloc(sizeof(int) * (2 * bw + 1));
				memcpy(pbuf, pwgS + j - bw, 2 * bw + 1);
				qsort(pbuf, 2 * bw + 1, sizeof(int), compare);
				wgmed = pbuf[bw];
				free(pbuf);

				prcGS = (wgs - wgmed) * 100.0 / (wgmed + 0.001);
	// prcGS shouldn't increase prcReal, changed 25.07.2023
				if (prcGS > 0.)
					prcGS = 0.;
				prcReal = dnprc - prcGS;

				if (prcReal <= -mprc)
				{
					*(pDefMB + mbnum) = def;
					*(pXdefMB + mbnum) = *(pXCS + j - 0);
					*(pYdefMB + mbnum) = *(pYCS + j - 0);
					mbnum++;
				}
			}
		}
		// medoS - buffer for RMed (SPACE)
		//*SPnumS += spnum;
		//*MBnumS += mbnum;
	}
	*MBnumS = mbnum;
	//j = j;
}

__device__ void FilterM3NCUDA(int ind, float *pw, float *pwg, UINT64 *pXInt1, UINT64 *pYInt1 /*(1st Intersect. point coords)*/, int n, int sz1, int sz2, float ang, float prcmb, float prcsp, float mabs, float pabs, float *pDefMB, float *pXdefMB, float *pYdefMB, float *pDefSP, float *pXdefSP, float *pYdefSP,  int *pMBnum, int *pSPnum)
{
	int j, jm, jp, jm2, jp2;
	float x, xm, xm2, xp, xp2, y, ym, ym2, yg, ymg, ymg2, ypg, ypg2, yp, yp2, d, tangl, d2, tangl2, ta, ta2, dm, yav, yav2, def;
	int spnum = 0, mbnum = 0;
	float tang = tan(ang*3.14159 / 180.);

	for (j = 0; j < n - 2; j++)
	{
		if ((jp = j + sz1) > n - 1)
			jp = n - 1;
		if ((jm = j - sz1) < 0)
			jm = 0;
		if ((jp2 = j + sz2) > n - 1)
			jp2 = n - 1;
		if ((jm2 = j - sz2) < 0)
			jm2 = 0;
		x = j;
		xm = jm;
		xm2 = jm2;
		xp = jp;
		xp2 = jp2;

		y = *(pw + j);
		//yg = *(pwg + j);

		ym = *(pw + jm);
		//ymg = *(pwg + jm);
		yp = *(pw + jp);
		//ypg = *(pwg + jp);
		//if (mode == 0)
		ymg = ypg = yg = 0;
		AngleDistCUDA(y - yg, x, ym - ymg, xm, yp - ypg, xp, &d, &tangl);

		ym2 = *(pw + jm2);
		//ymg2 = *(pwg + jm2);
		yp2 = *(pw + jp2);
		//ypg2 = *(pwg + jp2);
		//if (mode == 0)
		ymg2 = ypg2 = 0;
		AngleDistCUDA(y - yg, x, ym2 - ymg2, xm2, yp2 - ypg2, xp2, &d2, &tangl2);

		ta = abs(tangl);
		ta2 = abs(tangl2);

		if ((ta <= tang) && (ta2 <= tang))
			dm = d;
		else
			dm = 0;
		yav = (ym + yp) / 2.;
		yav2 = (ym2 + yp2) / 2.;

		def = 0.;
		if (dm < -mabs * prcmb / 100.)
			def = dm;
		else if (dm > pabs * prcsp / 100.)
			def = dm;

		if (def > 0.01)
		{
			*(pDefSP + ND * ind + spnum) = def;
			*(pXdefSP + ND * ind + spnum) = *(pXInt1 + j);
			*(pYdefSP + ND * ind + spnum) = *(pYInt1 + j);
			spnum++;
		}
		else if (def < -0.01)
		{
			*(pDefMB + ND * ind + mbnum) = def;
			*(pXdefMB + ND * ind + mbnum) = *(pXInt1 + j);
			*(pYdefMB + ND * ind + mbnum) = *(pYInt1 + j);
			mbnum++;
		}
	}
	*pSPnum = spnum;
	*pMBnum = mbnum;
		

		
}

void FilterM3N(float *pw, int *pwg, UINT64 *pXInt1, UINT64 *pYInt1 /*(1st Intersect. point coords)*/, int n, int sz1, int sz2, float ang, float prc, float mabs, float pabs, float *pDefMB, float *pXdefMB, float *pYdefMB, float *pDefSP, float *pXdefSP, float *pYdefSP, int *MBnum, int *SPnum, int mode)
{
	int j, jm, jp, jm2, jp2;
	float x, xm, xm2, xp, xp2, y, ym, ym2, yg, ymg, ymg2, ypg, ypg2, yp, yp2, d, tangl, d2, tangl2, ta, ta2, dm, yav, yav2, def;
	int spnum = 0, mbnum = 0;
	float tang = tan(ang*3.14159 / 180.);

	for (j = 0; j < n - 1; j++)
	{
		if ((jp = j + sz1) > n - 1)
			jp = n - 1;
		if ((jm = j - sz1) < 0)
			jm = 0;
		if ((jp2 = j + sz2) > n - 1)
			jp2 = n - 1;
		if ((jm2 = j - sz2) < 0)
			jm2 = 0;
		x = j;
		xm = jm;
		xm2 = jm2;
		xp = jp;
		xp2 = jp2;

		y = *(pw + j);
		//yg = *(pwg + j);

		ym = *(pw + jm);
		//ymg = *(pwg + jm);
		yp = *(pw + jp);
		//ypg = *(pwg + jp);
		//if (mode == 0)
		ymg = ypg = yg = 0;
		AngleDist(y - yg, x, ym - ymg, xm, yp - ypg, xp, &d, &tangl);

		ym2 = *(pw + jm2);
		//ymg2 = *(pwg + jm2);
		yp2 = *(pw + jp2);
		//ypg2 = *(pwg + jp2);
		//if (mode == 0)
		ymg2 = ypg2 = 0;
		AngleDist(y - yg, x, ym2 - ymg2, xm2, yp2 - ypg2, xp2, &d2, &tangl2);

		ta = abs(tangl);
		ta2 = abs(tangl2);

		if ((ta <= tang) && (ta2 <= tang))
			dm = d;
		else
			dm = 0;
		yav = (ym + yp) / 2.;
		yav2 = (ym2 + yp2) / 2.;

		def = 0.;
		if (dm < -mabs * prc / 100.)
			def = dm;
		else if (dm > pabs * prc / 100.)
			def = dm;


		if (def > 0.01)
		{
			*(pDefSP++) = def;
			*(pXdefSP++) = *(pXInt1 + j);
			*(pYdefSP++) = *(pYInt1 + j);
			spnum++;
		}
		else if (def < -0.01)
		{
			*(pDefMB++) = def;
			*(pXdefMB++) = *(pXInt1 + j);
			*(pYdefMB++) = *(pYInt1 + j);
			mbnum++;
		}
	}
	*SPnum = spnum;
	*MBnum = mbnum;
}

void FilterM5N(float *pw, int *pwg, float *pXInt1, float *pYInt1 /*(1st Intersect. point coords)*/, int n, int sz1, int sz2, float ang, float mprc, float sprc, float mabs, float pabs, float *pDefMB, float *pXdefMB, float *pYdefMB, float *pDefSP, float *pXdefSP, float *pYdefSP, int *MBnum, int *SPnum, int mode)
{
	int j, jm, jp, jm15, jp15, jm2, jp2;
	float x, xm, xm15, xm2, xp, xp15, xp2, y, ym, ym15, ym2, yg, ymg, ymg15, ymg2, ypg, ypg15, ypg2, yp, yp15, yp2, d, tangl, d15, tangl15, d2, tangl2, ta, ta15, ta2, dm, dm15, dm2, yav, yav15, yav2, def;
	int spnum = 0, mbnum = 0;
	float tang = tan(ang*3.14159 / 180.);
	int sz15 = (sz1 + sz2) / 2;

	for (j = 0; j < n - 1; j++)
	{
		if ((jp = j + sz1) > n - 1)
			jp = n - 1;
		if ((jm = j - sz1) < 0)
			jm = 0;
		if ((jp2 = j + sz2) > n - 1)
			jp2 = n - 1;
		if ((jm2 = j - sz2) < 0)
			jm2 = 0;
		if ((jp15 = j + sz15) > n - 1)
			jp15 = n - 1;
		if ((jm15 = j - sz15) < 0)
			jm15 = 0;
		x = j;
		xm = jm;
		xm15 = jm15;
		xm2 = jm2;
		xp = jp;
		xp15 = jp15;
		xp2 = jp2;

		y = *(pw + j);
		//yg = *(pwg + j);

		ym = *(pw + jm);
		//ymg = *(pwg + jm);
		yp = *(pw + jp);
		//ypg = *(pwg + jp);
		//if (mode == 0)
		ymg = ypg = yg = 0;
		AngleDist(y - yg, x, ym - ymg, xm, yp - ypg, xp, &d, &tangl);

		ym15 = *(pw + jm15);
		yp15 = *(pw + jp15);
		ymg15 = ypg15 = 0;
		AngleDist(y - yg, x, ym15 - ymg15, xm15, yp15 - ypg15, xp15, &d15, &tangl15);

		ym2 = *(pw + jm2);
		//ymg2 = *(pwg + jm2);
		yp2 = *(pw + jp2);
		//ypg2 = *(pwg + jp2);
		//if (mode == 0)
		ymg2 = ypg2 = 0;
		AngleDist(y - yg, x, ym2 - ymg2, xm2, yp2 - ypg2, xp2, &d2, &tangl2);

		ta = abs(tangl);
		ta15 = abs(tangl15);
		ta2 = abs(tangl2);

		if ((ta <= tang))
		{
			dm = d;
		}
		else dm = 0.;

		if ((ta15 <= tang))
		{
			dm15 = d15;
		}
		else
			dm15 = 0;

		if ((ta2 <= tang))
		{
			dm2 = d2;
		}
		else
			dm2 = 0;

		/*if ((ta <= tang) && (ta2 <= tang))
			dm = d;
		else
			dm = 0;*/

		yav = (ym + yp) / 2.;
		yav15 = (ym15 + yp15) / 2.;
		yav2 = (ym2 + yp2) / 2.;


		int s = 0, s15 = 0, s2 = 0; // sign - 1 positive/ -1 negative
		if ((y > ym) && (y > yp))
			s = 1;
		if ((y > ym2) && (y > yp2))
			s2 = 1;
		if ((y > ym15) && (y > yp15))
			s15 = 1;
		if ((y < ym) && (y < yp))
			s = -1;
		if ((y < ym2) && (y < yp2))
			s2 = -1;
		if ((y < ym15) && (y < yp15))
			s15 = -1;

		int n = 0, n15 = 0, n2 = 0, p = 0, p15 = 0, p2 = 0; // over positive or negative thresholds
		if (dm > sprc)
			p = 1;
		if (dm < -mprc)
			n = 1;
		if (dm15 > sprc)
			p15 = 1;
		if (dm15 < -mprc)
			n15 = 1;
		if (dm2 > sprc)
			p2 = 1;
		if (dm2 < -mprc)
			n2 = 1;

		def = 0.;
		if ((y == 0) || (ym == 0) || (yp == 0))
			def = 0.;
		else
		{
			if ((s == 1) && (s15 == 1) || (s == 1) && (s2 == 1) || (s15 == 1) && (s2 == 1)) // spur candidate
			{
				if ((p == 1) && (p15 == 1))
				{
					if (abs(dm) > abs(dm15))
						def = dm;
					else
						def = dm15;
				}
				if ((p == 1) && (p2 == 1))
				{
					if (abs(dm) > abs(dm2))
						def = dm;
					else
						def = dm2;
				}
				if ((p15 == 1) && (p2 == 1))
				{
					if (abs(dm15) > abs(dm2))
						def = dm15;
					else
						def = dm2;
				}
			}

			if ((s == -1) && (s15 == -1) || (s == -1) && (s2 == -1) || (s15 == -1) && (s2 == -1)) // MB candidate
			{
				if ((n == 1) && (n15 == 1))
				{
					if (abs(dm) > abs(dm15))
						def = dm;
					else
						def = dm15;
				}
				if ((n == 1) && (n2 == 1))
				{
					if (abs(dm) > abs(dm2))
						def = dm;
					else
						def = dm2;
				}
				if ((n15 == 1) && (n2 == 1))
				{
					if (abs(dm15) > abs(dm2))
						def = dm15;
					else
						def = dm2;
				}
			}
		}

		if (y > pabs || y < -mabs)
			def = y;

		if (def > 0.01)
		{
			*(pDefSP++) = def;
			*(pXdefSP++) = *(pXInt1 + j);
			*(pYdefSP++) = *(pYInt1 + j);
			spnum++;
		}
		else if (def < -0.01)
		{
			*(pDefMB++) = def;
			*(pXdefMB++) = *(pXInt1 + j);
			*(pYdefMB++) = *(pYInt1 + j);
			mbnum++;
		}
	}
	*SPnum = spnum;
	*MBnum = mbnum;
}

void FilterM4N(float *pw, int *pwg, UINT64 *pXInt1, UINT64 *pYInt1 /*(1st Intersect. point coords)*/,  int n, int sz1, int sz2, float ang, float prcm, float prcp, float mabs, float pabs, float *pDefMB, float *pXdefMB, float *pYdefMB, float *pDefSP, float *pXdefSP, float *pYdefSP, int *MBnum, int *SPnum, int mode)
{
	int j, jm, jp, jm2, jp2;
	float x, xm, xm2, xp, xp2, y, ym, ym2, yg, ymg, ymg2, ypg, ypg2, yp, yp2, d, tangl, d2, tangl2, ta, ta2, dm, dm2, yav, yav2, def;
	int spnum = 0, mbnum = 0;
	float tang = tan(ang*3.14159 / 180.);
	
	for (j = 0; j < n - 1; j++)
	{
		if ((jp = j + sz1) > n - 1)
			jp = n - 1;
		if ((jm = j - sz1) < 0)
			jm = 0;
		if ((jp2 = j + sz2) > n - 1)
			jp2 = n - 1;
		if ((jm2 = j - sz2) < 0)
			jm2 = 0;
		x = j;
		xm = jm;
		xm2 = jm2;
		xp = jp;
		xp2 = jp2;

		y = *(pw + j);
		//yg = *(pwg + j);

		ym = *(pw + jm);
		//ymg = *(pwg + jm);
		yp = *(pw + jp);
		//ypg = *(pwg + jp);
		//if (mode == 0)
		ymg = ypg = yg = 0;
		AngleDist(y - yg, x, ym - ymg, xm, yp - ypg, xp, &d, &tangl);

		ym2 = *(pw + jm2);
		//ymg2 = *(pwg + jm2);
		yp2 = *(pw + jp2);
		//ypg2 = *(pwg + jp2);
		//if (mode == 0)
		ymg2 = ypg2 = 0;
		AngleDist(y - yg, x, ym2 - ymg2, xm2, yp2 - ypg2, xp2, &d2, &tangl2);

		ta = abs(tangl);
		ta2 = abs(tangl2);

		if ((ta <= tang) )
		{
			dm = d;
		}
		else dm = 0.;

		if ( (ta2 <= tang))
		{
			dm2 = d2;
		}
		else
			dm2 = 0;

		/*if ((ta <= tang) && (ta2 <= tang))
			dm = d;
		else
			dm = 0;*/

		yav = (ym + yp) / 2.;
		yav2 = (ym2 + yp2) / 2.;

		
		if ( (dm < -mabs) && (dm2 < -mabs))
			def = min(dm, dm2);
		else if ((dm > pabs) && (dm > pabs))
			def = max(dm, dm2);
		else if (y > prcp || y < -prcm)
				def = y;
		else def = 0.;


		if (def > 0.01)
		{
			*(pDefSP++) = def;
			*(pXdefSP++) = *(pXInt1 + j);
			*(pYdefSP++) = *(pYInt1 + j);
			spnum++;
		}
		else if (def < -0.01)
		{
			*(pDefMB++) = def;
			*(pXdefMB++) = *(pXInt1 + j);
			*(pYdefMB++) = *(pYInt1 + j);
			mbnum++;
		}
	}
	*SPnum = spnum;
	*MBnum = mbnum;
}

void NonFWM_PostProc(float *pw, int *pwg, float *medoN, float *pXC, float *pYC, int na, int *pPaddr, int sz1, int sz2, int skip, float ang, float prcm, float prcp, float mabs, float pabs, float *pDefMB, float *pXdefMB, float *pYdefMB, float *pDefSP, float *pXdefSP, float *pYdefSP, int *MBnum, int *SPnum)
{
	int caddrb, caddre, cnum;
	int mbnum, spnum, j;
	for (j = 0; j < na - 1; j++)
	{
		if (j == 325)
		{
			j = j;
		}
		caddrb = *(pPaddr + j);
		caddre = *(pPaddr + j + 1) - 1;
		cnum = (caddre - caddrb);
		int s1=sz1, s2=sz2;
		if (s1 > cnum / 5)
			s1 = cnum / 5;
		if (s2 > cnum / 3)
			s2 = cnum / 3;
//		FilterM4N(pw + caddrb, pwg + caddrb, pXC + caddrb, pYC + caddrb, cnum, s1, s2, ang, prcm, prcp, mabs, pabs, pDefMB + *MBnum, pXdefMB + *MBnum, pYdefMB + *MBnum, pDefSP + *SPnum, pXdefSP + *SPnum, pYdefSP + *SPnum, &mbnum, &spnum, 0);
		//FilterM5N(pw + caddrb, pwg + caddrb, pXC + caddrb, pYC + caddrb, cnum, s1, s2, ang, prcm, prcp, mabs, pabs, pDefMB + *MBnum, pXdefMB + *MBnum, pYdefMB + *MBnum, pDefSP + *SPnum, pXdefSP + *SPnum, pYdefSP + *SPnum, &mbnum, &spnum, 0);
		//FilterM3N(pWidthN + caddrb, pwg + caddrb, pXC + caddrb, pYC + caddrb, cnum, s1, s2, ang, prcm, prcp, mabs, pDefMB + *MBnum, pXdefMB + *MBnum, pYdefMB + *MBnum, pDefSP + *SPnum, pXdefSP + *SPnum, pYdefSP + *SPnum, &mbnum, &spnum, 0);
		//FilterRMedNFWM(pw + caddrb, dmedoN, pCols, pRows, clen, sz1, mmprcMBN, mmprcSPN, mmabsMBN, mmabsSPN, mmpDefMBN + mmMBnumN, mmpXdefMBN + mmMBnumN, mmpYdefMBN + mmMBnumN, mmpDefSPN + mmSPnumN, mmpXdefSPN + mmSPnumN, mmpYdefSPN + mmSPnumN, &mbnum, &spnum, 1);
		
 //void FilterRMedNFWM(float *pwN, float *medo, float *pXCN, float *pYCN, int n, int sz1, float mprcN, float sprcN, float mabsN, float pabsN, float *pDefMBN, float *pXdefMBN, float *pYdefMBN, float *pDefSPN, float *pXdefSPN, float *pYdefSPN, int *MBnumN, int *SPnumN, int modeN)
		FilterRMedNFWM(pw + caddrb, medoN, pXC + caddrb, pYC + caddrb, cnum, sz1, skip, prcm, prcp, mabs, pabs,  pDefMB + *MBnum, pXdefMB + *MBnum, pYdefMB + *MBnum, pDefSP + *SPnum, pXdefSP + *SPnum, pYdefSP + *SPnum, &mbnum, &spnum, 0);

		*SPnum += spnum;
		*MBnum += mbnum;
	}
}

__global__ void NonFWM_PostProcCUDA(float *pw, float *pwg, UINT64 *pXC, UINT64 *pYC, int na, int *pPaddr, int sz1, int sz2, float ang, float prcmb, float prcsp, float mabs, float pabs, float *pDefMB, float *pXdefMB, float *pYdefMB, float *pDefSP, float *pXdefSP, float *pYdefSP, int *MBnum, int *SPnum)
{
	int caddrb, caddre, cnum;
	int mbnum, spnum, j;
	int cIdx = threadIdx.x + blockIdx.x*blockDim.x; // GPU tread index
	if (cIdx < na - 2)
	{	j = cIdx;
	
		/*if (j == 457)
		{
			j = j;
		}*/
		caddrb = *(pPaddr + j);
		caddre = *(pPaddr + j + 1) - 0;
		cnum = (caddre - caddrb);
		int s1 = sz1, s2 = sz2;
		if (s1 > cnum / 5)
			s1 = cnum / 5;
		if (s2 > cnum / 3)
			s2 = cnum / 3;
		FilterM3NCUDA(j, pw + caddrb, pwg + caddrb, pXC + caddrb, pYC + caddrb, cnum, s1, s2, ang, prcmb, prcsp, mabs, pabs, pDefMB + *MBnum, pXdefMB + *MBnum, pYdefMB + *MBnum, pDefSP + *SPnum, pXdefSP + *SPnum, pYdefSP + *SPnum, &mbnum, &spnum);

		*SPnum += spnum;
		*MBnum += mbnum;
	}
}




__device__ void FilterM3CUDA(int ind, float *pw, float *pXC, float *pYC, int n, int na, int sz1, int sz2, float ang, float prcmb, float prcsp, float mabs, float pabs, float *pDefMB, float *pXdefMB, float *pYdefMB, float *pDefSP, float *pXdefSP, float *pYdefSP, int *pMBnum, int *pSPnum)
{
	int j, jm, jp, jm2, jp2;
	float x, xm, xm2, xp, xp2, y, ym, ym2, yp, yp2, d, tangl, d2, tangl2, ta, ta2, dm, yav, yav2, def;
	int spnum = 0, mbnum = 0;
	float tang = tan(ang*3.14159 / 180.);
	if (n < 2)
	{
		*pSPnum = spnum;
		*pMBnum = mbnum;
		return;
	}
	for (j = 0; j < n - 2; j++)
	{
		if ((jp = j + sz1) > n - 1)
			jp = n - 1;
		if ((jm = j - sz1) < 0)
			jm = 0;
		if ((jp2 = j + sz2) > n - 1)
			jp2 = n - 1;
		if ((jm2 = j - sz2) < 0)
			jm2 = 0;
		x = j;
		xm = jm;
		xm2 = jm2;
		xp = jp;
		xp2 = jp2;

		y = *(pw + j);
		ym = *(pw + jm);
		yp = *(pw + jp);
		AngleDistCUDA(y, x, ym, xm, yp, xp, &d, &tangl);

		ym2 = *(pw + jm2);
		yp2 = *(pw + jp2);
		AngleDistCUDA(y, x, ym2, xm2, yp2, xp2, &d2, &tangl2);

		//a = abs(angl)*180. / 3.1416;
		//a2 = abs(angl2)*180. / 3.1416;
		ta = abs(tangl);
		ta2 = abs(tangl2);
		/*ta = tangl;
		ta2 = tangl2;*/
		if ((ta <= tang) && (ta2 <= tang))
			dm = d;
		else
			dm = 0;
		yav = (ym + yp) / 2.;
		yav2 = (ym2 + yp2) / 2.;

		//d = y - yav;
		//d2 = y - yav2;

		//dm = d;

		//if ((y == 0) || (ym == 0) || (yp == 0))
		//	def = 0.;
		//else if ((dm > abs(yav)*prcsp / 100.) && (dm2 > abs(yav2)*prcsp / 100.) || (dm < -abs(yav)*prcmb / 100.) && (dm2 < -abs(yav2)*prcmb / 100.))
		//{
		//	if (abs(dm) > abs(dm2))
		//		def = dm;
		//	else
		//		def = dm2;
		//}
		//else
		//	def = 0;// 

		if ((abs(dm) < abs(yav)*prcmb / 100.) && (abs(dm) < abs(yav2)*prcmb / 100.))
			def = 0.;
		else if ((y == 0) || (ym == 0) || (yp == 0))
			def = 0.;
		else
			def = dm / yav * 100.;

		//		if (d > pabs || d < -mabs)
		//			def = d;

		if (def > 0.01)
		{
			*(pDefSP + ND * ind + spnum) = def;
			*(pXdefSP + ND * ind + spnum) = *(pXC + j);
			*(pYdefSP + ND * ind + spnum) = *(pYC + j);
			spnum++;
		}
		else if (def < -0.01)
		{
			*(pDefMB + ND * ind + mbnum) = def;
			*(pXdefMB + ND  * ind + mbnum) = *(pXC + j);
			*(pYdefMB + ND  * ind + mbnum) = *(pYC + j);
			mbnum++;
		}
	}
	*pSPnum = spnum;
	*pMBnum = mbnum;
}
// two point flter for fiding  and spur for singe seleton branch for CUDA
//FilterM4CUDA(j, pw + caddrb, pXC + caddrb, pYC + caddrb, cnum, na, sz1, sz2, 3.0, 35.0, 35.0, 10., 10., pDefMB, pXdefMB, pYdefMB, pDefSP, pXdefSP, pYdefSP, &mbnum, &spnum, andNOor);
__device__ void FilterM4CUDA(int ind, float *pw, float *pwg, float *pXC, float *pYC, int n, int sz1, int sz2, float ang, float mprc, float sprc, float mabs, float pabs, int dlen, float *pDefMB, float *pXdefMB, float *pYdefMB, float *pDefSP, float *pXdefSP, float *pYdefSP, int *MBnum, int *SPnum, int mode, int andNOor)
{
	/// mode==0  - not usinf CAD width
	/// andNOor  - AND==1 / OR==0 /Aver==2 (for two filters)
	int j, jm, jp, jm2, jp2;
	float x, xm, xm2, xp, xp2, y, ym, ym2, yg, ymg, ymg2, ypg, ypg2, yp, yp2, d, tangl, d2, tangl2, ta, ta2, dm, dm2, yav, yav2, def;
	int spnum = 0, mbnum = 0;
	float tang = tan(ang*3.14159 / 180.);
	if (n == 0)
	{
		*SPnum = spnum;
		*MBnum = mbnum;
		return;
	}
	for (j = 0; j < n - 1; j++)
	{
		if ((jp = j + sz1) > n - 1)
			jp = n - 1;
		if ((jm = j - sz1) < 0)
			jm = 0;
		if ((jp2 = j + sz2) > n - 1)
			jp2 = n - 1;
		if ((jm2 = j - sz2) < 0)
			jm2 = 0;
		x = j;
		xm = jm;
		xm2 = jm2;
		xp = jp;
		xp2 = jp2;

		y = *(pw + j);
		yg = *(pwg + j);

		ym = *(pw + jm);
		ymg = *(pwg + jm);
		yp = *(pw + jp);
		ypg = *(pwg + jp);
		if (mode & 1 == 0)
			ymg = ypg = yg = 0;
		AngleDistCUDA(y - yg, x, ym - ymg, xm, yp - ypg, xp, &d, &tangl);

		ym2 = *(pw + jm2);
		ymg2 = *(pwg + jm2);
		yp2 = *(pw + jp2);
		ypg2 = *(pwg + jp2);
		if (mode & 1 == 0)
			ymg2 = ypg2 = 0;
		AngleDistCUDA(y - yg, x, ym2 - ymg2, xm2, yp2 - ypg2, xp2, &d2, &tangl2);

		//a = abs(angl)*180. / 3.1416;
		//a2 = abs(angl2)*180. / 3.1416;
		ta = abs(tangl);
		ta2 = abs(tangl2);
		/*ta = tangl;
		ta2 = tangl2;*/

		if (ta <= tang)
			dm = d;
		else dm = 0.;

		if (ta2 <= tang)
			dm2 = d2;
		else
			dm2 = 0.;

		/*if ((ta <= tang) )
			dm = d;
		else if ((ta2 <= tang))
			dm = d2;
		else
			dm = 0;*/
		yav = (ym + yp) / 2.;
		yav2 = (ym2 + yp2) / 2.;

		//d = y - yav;
		//d2 = y - yav2;

		//dm = d;
		if (andNOor == 0)
		{
			if ((y == 0) || (ym == 0) || (yp == 0))
				def = 0.;
			else if ((dm > abs(yav)*sprc / 100.) || (dm < abs(yav)*mprc / 100.))
				def = dm;
			else if ((dm2 > abs(yav2)*sprc / 100.) || (dm2 < abs(yav2)*mprc / 100.))
				def = dm2;
			else
				def = 0;// 
		}
		else
		{
			if ((y == 0) || (ym == 0) || (yp == 0))
				def = 0.;
			else if ((dm > abs(yav)*sprc / 100.) && (dm2 > abs(yav2)*sprc / 100.) || (dm < -abs(yav)*mprc / 100.) && (dm2 < -abs(yav2)*mprc / 100.))
			{
				if (abs(dm) > abs(dm2))
					def = dm;
				else
					def = dm2;
			}
			else
				def = 0;// 
		}

		if (def > 0.01)
		{
			*(pDefSP + ND * ind + spnum) = def;
			*(pXdefSP + ND * ind + spnum) = *(pXC + j);
			*(pYdefSP + ND * ind + spnum) = *(pYC + j);
			spnum++;
		}
		else if (def < -0.01)
		{
			*(pDefMB + ND * ind + mbnum) = def;
			*(pXdefMB + ND * ind + mbnum) = *(pXC + j);
			*(pYdefMB + ND * ind + mbnum) = *(pYC + j);
			mbnum++;
		}
	}
	*SPnum = spnum;
	*MBnum = mbnum;
}

__device__ void FilterM4CUDAo(int ind, float *pw, float *pXC, float *pYC, int n, int na, int sz1, int sz2, float ang, float mprc, float sprc, float mabs, float pabs, float *pDefMB, float *pXdefMB, float *pYdefMB, float *pDefSP, float *pXdefSP, float *pYdefSP, int *pMBnum, int *pSPnum, int andNOor)
{
	int j, jm, jp, jm2, jp2;
	float x, xm, xm2, xp, xp2, y, ym, ym2, yp, yp2, d, tangl, d2, tangl2, ta, ta2, dm, dm2, yav, yav2, def;
	int spnum = 0, mbnum = 0;
	float tang = tan(ang*3.14159 / 180.);
	if (n < 2)
	{
		*pSPnum = spnum;
		*pMBnum = mbnum;
		return;
	}
	for (j = 0; j < n - 2; j++)
	{
		if ((jp = j + sz1) > n - 1)
			jp = n - 1;
		if ((jm = j - sz1) < 0)
			jm = 0;
		if ((jp2 = j + sz2) > n - 1)
			jp2 = n - 1;
		if ((jm2 = j - sz2) < 0)
			jm2 = 0;
		x = j;
		xm = jm;
		xm2 = jm2;
		xp = jp;
		xp2 = jp2;

		y = *(pw + j);
		ym = *(pw + jm);
		yp = *(pw + jp);
		AngleDistCUDA(y, x, ym, xm, yp, xp, &d, &tangl);

		ym2 = *(pw + jm2);
		yp2 = *(pw + jp2);
		AngleDistCUDA(y, x, ym2, xm2, yp2, xp2, &d2, &tangl2);

		//a = abs(angl)*180. / 3.1416;
		//a2 = abs(angl2)*180. / 3.1416;
		ta = abs(tangl);
		ta2 = abs(tangl2);
		/*ta = tangl;
		ta2 = tangl2;*/
		if (ta <= tang)
			dm = d;
		else dm = 0.;

		if (ta2 <= tang)
			dm2 = d2;
		else
			dm2 = 0.;
		yav = (ym + yp) / 2.;
		yav2 = (ym2 + yp2) / 2.;

		//d = y - yav;
		//d2 = y - yav2;

		//dm = d;

		if (andNOor == 0)
		{
			if ((y == 0) || (ym == 0) || (yp == 0))
				def = 0.;
			else if ((dm > abs(yav)*sprc / 100.) || (dm < abs(yav)*mprc / 100.))
				def = dm;
			else if ((dm2 > abs(yav2)*sprc / 100.) || (dm2 < abs(yav2)*mprc / 100.))
				def = dm2;
			else
				def = 0;// 
		}
		else
		{
			if ((y == 0) || (ym == 0) || (yp == 0))
				def = 0.;
			else if ((dm > abs(yav)*sprc / 100.) && (dm2 > abs(yav2)*sprc / 100.) || (dm < -abs(yav)*mprc / 100.) && (dm2 < -abs(yav2)*mprc / 100.))
			{
				if (abs(dm) > abs(dm2))
					def = dm;
				else
					def = dm2;
			}
			else
				def = 0;// 
		}

		if (def > 0.01)
		{
			*(pDefSP + ND * ind + spnum) = def;
			*(pXdefSP + ND * ind + spnum) = *(pXC + j);
			*(pYdefSP + ND * ind + spnum) = *(pYC + j);
			spnum++;
		}
		else if (def < -0.01)
		{
			*(pDefMB + ND * ind + mbnum) = def;
			*(pXdefMB + ND * ind + mbnum) = *(pXC + j);
			*(pYdefMB + ND * ind + mbnum) = *(pYC + j);
			mbnum++;
		}
	}
	*pSPnum = spnum;
	*pMBnum = mbnum;
}

__global__ void FWM_PostProcCUDA(float *pw, int *pwg, float *pXC, float *pYC, int na, int *pPaddr, int sz1, int sz2, float ang, float prcmb, float prcsp, float mabs, float pabs, float *pDefMB, float *pXdefMB, float *pYdefMB, float *pDefSP, float *pXdefSP, float *pYdefSP, int *pMBnum, int *pSPnum, int andNOor, float *dmedo)
{
	int caddrb, caddre, cnum;
	int mbnum, spnum, j;

	
	int cIdx = threadIdx.x + blockIdx.x*blockDim.x; // GPU tread index
	if (cIdx < na - 2)
	{
		j = cIdx;

		caddrb = *(pPaddr + j);
		caddre = *(pPaddr + j + 1);
		cnum = (caddre - caddrb);
		FilterRMedG(pw + caddrb, pwg + caddrb, dmedo, pXC + caddrb, pYC + caddrb, cnum, 71, prcmb, prcsp, mabs, pabs, 1, pDefMB, pXdefMB, pYdefMB, pDefSP, pXdefSP, pYdefSP, &mbnum, &spnum, 0);
		//FilterM4CUDAo(j, pw + caddrb, pXC + caddrb, pYC + caddrb, cnum, na, sz1, sz2, ang, prcmb, prcsp, mabs, pabs, pDefMB, pXdefMB, pYdefMB, pDefSP, pXdefSP, pYdefSP, &mbnum, &spnum, andNOor);
		//FilterM4CUDA(j, pw + caddrb, pw + caddrb, pXC + caddrb, pYC + caddrb, cnum, na, sz1, sz2, 3.0, 35.0, 35.0, 10., 10., pDefMB, pXdefMB, pYdefMB, pDefSP, pXdefSP, pYdefSP, &mbnum, &spnum, 0, andNOor);
		//FilterM3CUDA(j, pw + caddrb, pXC + caddrb, pYC + caddrb, cnum, na, sz1, sz2, 3.0, 35.0, 35.0, 10., 10., pDefMB, pXdefMB, pYdefMB, pDefSP, pXdefSP, pYdefSP, &mbnum, &spnum);

		*(pSPnum + j) = spnum;
		*(pMBnum + j) = mbnum;

	}
}

// postprocessing cycle (for all FWM seleton pieces) for CUDA
void FWM_PostProc_CUDA_Launch(int na, int cgpu, int sz1, int sz2, float ang, float prcMB, float prcSP, float mabs, float pabs, int *pMBnum, int *pSPnum, int andNOor)
{
	dim3 dimBlock(BLOCK_SIZE);
	dim3 dimGrid((na + 0) / dimBlock.x);

	int numBlock = (na + 0) / ThrPerBlock;

	cudaSetDevice(cgpu);
	cudaError_t Cerror = cudaGetLastError();
	//cudaDeviceReset();
	//GPU_FWM_Br14 << <numBlock, ThrPerBlock >> > (d_Im, d_ImIRB, imw, imh, pst, pn - 256, d_P1x, d_P1y, d_P2x, d_P2y, d_Pnum, d_Wl, d_Wlmin, d_Pint1x, d_Pint1y, d_Pint2x, d_Pint2y);
	//*d_pMBnum = *d_pSPnum = 0;

	/// realocating data buffers

	/*Cerror = cudaFree((void**)&d_pDefMB);
	Cerror = cudaFree((void**)&d_pXdefMB);
	Cerror = cudaFree((void**)&d_pYdefMB);
	Cerror = cudaFree((void**)&d_pDefSP);
	Cerror = cudaFree((void**)&d_pXdefSP);
	Cerror = cudaFree((void**)&d_pYdefSP);
	Cerror = cudaFree((void**)&d_pMBnum);
	Cerror = cudaFree((void**)&d_pSPnum);*/

	Cerror = cudaMallocManaged((void**)&d_pDefMB, ND * na * sizeof(float)); // MB defect output array
	Cerror = cudaMallocManaged((void**)&d_pXdefMB, ND * na * sizeof(float)); // MB defect X coord. output array
	Cerror = cudaMallocManaged((void**)&d_pYdefMB, ND * na * sizeof(float)); // MB defect Y coord. output array
	Cerror = cudaMallocManaged((void**)&d_pDefSP, ND * na * sizeof(float)); // SP defect output array
	Cerror = cudaMallocManaged((void**)&d_pXdefSP, ND * na * sizeof(float)); // SP defect X coord. output array
	Cerror = cudaMallocManaged((void**)&d_pYdefSP, ND * na * sizeof(float)); // SP defect Y coord. output array
	Cerror = cudaMallocManaged((void**)&d_pMBnum, na * sizeof(int)); // pointer to MBnum array on device
	Cerror = cudaMallocManaged((void**)&d_pSPnum, na * sizeof(int)); // pointer to SPnum array on device
	
	FWM_PostProcCUDA << <numBlock, ThrPerBlock >> > (d_Wl, d_WG, d_P3x, d_P3y, na, d_PaddrC, sz1, sz2, ang, prcMB, prcSP, mabs, pabs, d_pDefMB, d_pXdefMB, d_pYdefMB, d_pDefSP, d_pXdefSP, d_pYdefSP, d_pMBnum, d_pSPnum, andNOor, dmedo);

	if ((Cerror = cudaGetLastError()))
		Beep(2000, 100);
	//cudaThreadSynchronize();
	cudaDeviceSynchronize();

	// check for error
	Cerror = cudaGetLastError();

}

void NonFWM_PostProc_CUDA_Launch(int na, int cgpu, int sz1, int sz2, float ang, float prcMB, float prcSP, float mabs, float pabs, int *pMBnum, int *pSPnum)
{
	dim3 dimBlock(BLOCK_SIZE);
	dim3 dimGrid((na + 0) / dimBlock.x);

	int numBlock = (na + 0) / ThrPerBlock;

	cudaSetDevice(cgpu);
	cudaError_t Cerror = cudaGetLastError();
	*d_pMBnumN = 0;
	*d_pSPnumN = 0;
	//cudaDeviceReset();
	//GPU_FWM_Br14 << <numBlock, ThrPerBlock >> > (d_Im, d_ImIRB, imw, imh, pst, pn - 256, d_P1x, d_P1y, d_P2x, d_P2y, d_Pnum, d_Wl, d_Wlmin, d_Pint1x, d_Pint1y, d_Pint2x, d_Pint2y);
	//*d_pMBnum = *d_pSPnum = 0;
//__global__ void NonFWM_PostProcCUDA(float *pw, float *pwg, UINT64 *pXC, UINT64 *pYC, int na, int *pPaddr, int sz1, int sz2, float ang, float prc, float mabs, float pabs, float *pDefMB, float *pXdefMB, float *pYdefMB, float *pDefSP, float *pXdefSP, float *pYdefSP, int *MBnum, int *SPnum)
	NonFWM_PostProcCUDA << <numBlock, ThrPerBlock >> > (d_WlN, d_WlN, d_Pint1Nx, d_Pint1Ny, na, d_PaddrCN, sz1, sz2, ang, prcMB, prcSP, mabs, pabs, d_pDefMBN, d_pXdefMBN, d_pYdefMBN, d_pDefSPN, d_pXdefSPN, d_pYdefSPN, d_pMBnumN, d_pSPnumN);

	if ((Cerror = cudaGetLastError()))
		Beep(2000, 100);
	//cudaThreadSynchronize();
	cudaDeviceSynchronize();

	// check for error
	Cerror = cudaGetLastError();

}

void CUDA_UnloadPPcond(int cgpu, int na)
{
	cudaError_t Cerror = cudaSetDevice(cgpu);
	Cerror = cudaGetLastError();
	Cerror = cudaMallocManaged((void**)&d_DefMB, MBSPMAX * sizeof(float)); // MB condensed output array
	Cerror = cudaMallocManaged((void**)&d_XdefMB, MBSPMAX * sizeof(float)); // MB condensed output array
	Cerror = cudaMallocManaged((void**)&d_YdefMB, MBSPMAX * sizeof(float)); // MB condensed output array
	Cerror = cudaMallocManaged((void**)&d_DefSP, MBSPMAX * sizeof(float)); // MB condensed output array
	Cerror = cudaMallocManaged((void**)&d_XdefSP, MBSPMAX * sizeof(float)); // MB condensed output array
	Cerror = cudaMallocManaged((void**)&d_YdefSP, MBSPMAX * sizeof(float)); // MB condensed output array
	thrust::device_ptr<float> dp_V(d_pDefMB);
	thrust::device_ptr<float> dp_R(d_DefMB);
	thrust::copy_if(dp_V, dp_V + na * ND, dp_R, is_not_zero());
	Cerror = cudaGetLastError();
	thrust::device_ptr<float> dp_V2(d_pXdefMB);
	thrust::device_ptr<float> dp_R2(d_XdefMB);
	thrust::copy_if(dp_V2, dp_V2 + na * ND, dp_R2, is_not_zero());
	thrust::device_ptr<float> dp_V3(d_pYdefMB);
	thrust::device_ptr<float> dp_R3(d_YdefMB);
	thrust::copy_if(dp_V3, dp_V3 + na * ND, dp_R3, is_not_zero());

	thrust::device_ptr<float> dp_V4(d_pXdefSP);
	thrust::device_ptr<float> dp_R4(d_XdefSP);
	thrust::copy_if(dp_V4, dp_V4 + na * ND, dp_R4, is_not_zero());
	thrust::device_ptr<float> dp_V5(d_pYdefSP);
	thrust::device_ptr<float> dp_R5(d_YdefSP);
	thrust::copy_if(dp_V5, dp_V5 + na * ND, dp_R5, is_not_zero());

	thrust::device_ptr<float> dp_V6(d_pXdefSP);
	thrust::device_ptr<float> dp_R6(d_XdefSP);
	thrust::copy_if(dp_V6, dp_V6 + na * ND, dp_R6, is_not_zero());
	Cerror = cudaGetLastError();
}


void CUDA_UnloadPP(int cgpu, int *pMBnum, int *pSPnum, int na)
{
	//int pn = pni + 0;
	*pMBnum = 0;
	*pSPnum = 0;
	cudaError_t Cerror = cudaSetDevice(cgpu);
	for (int j = 0; j < na-1; j++)
	{
		*pMBnum += *(d_pMBnum+j);
		*pSPnum += *(d_pSPnum+j);
	}
	Cerror = cudaGetLastError();
	Cerror = cudaMemcpy(pDefMB, d_pDefMB, ND * na* sizeof(float), cudaMemcpyDeviceToHost);
	Cerror = cudaMemcpy(pXdefMB, d_pXdefMB, ND * na * sizeof(float), cudaMemcpyDeviceToHost);
	Cerror = cudaMemcpy(pYdefMB, d_pYdefMB, ND * na * sizeof(float), cudaMemcpyDeviceToHost);
	
	Cerror = cudaMemcpy(pDefSP, d_pDefSP, ND * na * sizeof(float), cudaMemcpyDeviceToHost);
	Cerror = cudaMemcpy(pXdefSP, d_pXdefSP, ND * na * sizeof(float), cudaMemcpyDeviceToHost);
	Cerror = cudaMemcpy(pYdefSP, d_pYdefSP, ND * na * sizeof(float), cudaMemcpyDeviceToHost);
	Cerror = cudaGetLastError();
}

void CUDA_UnloadPPN(int cgpu, int *pMBnumN, int *pSPnumN, int nacn)
{
	//int pn = pni + 0;
	*pMBnumN = 0;
	*pSPnumN = 0;
	int nSP=1, nMB=1;
	cudaError_t Cerror = cudaSetDevice(cgpu);
	for (int j = 0; j < nacn - 1; j++)
	{
		*pMBnumN += *(d_pMBnumN + j);
		*pSPnumN += *(d_pSPnumN + j);
		nMB += *(d_pMBnumN + j);
		nSP += *(d_pSPnumN + j);
	}
	Cerror = cudaGetLastError();
	Cerror = cudaMemcpy(pDefMBN, d_pDefMBN, ND * nacn * sizeof(float), cudaMemcpyDeviceToHost);
	Cerror = cudaMemcpy(pXdefMBN, d_pXdefMBN, ND * nacn * sizeof(float), cudaMemcpyDeviceToHost);
	Cerror = cudaMemcpy(pYdefMBN, d_pYdefMBN, ND * nacn * sizeof(float), cudaMemcpyDeviceToHost);

	Cerror = cudaMemcpy(pDefSPN, d_pDefSP, ND * nacn * sizeof(float), cudaMemcpyDeviceToHost);
	Cerror = cudaMemcpy(pXdefSPN, d_pXdefSP, ND * nacn * sizeof(float), cudaMemcpyDeviceToHost);
	Cerror = cudaMemcpy(pYdefSPN, d_pYdefSP, ND * nacn * sizeof(float), cudaMemcpyDeviceToHost);

}

// new version for pattern 20220914
void SubPixCrdP(int *g0, int i0, int *PPx, int *PPy, float *pIntX, float *pIntY, float thr)
{
	//int PIntX[4], PIntY[4];
	int gmin, gmid, gmax, jmin, jmid, jmax; // points values and numbers
	int Px[3], Py[3];						// 3 conseq. points coordinates
	int dg;
	float dd;

	Px[1] = PPx[i0];
	Px[0] = PPx[i0 - 1];
	Px[2] = PPx[i0 + 1];
	Py[1] = PPy[i0];
	Py[0] = PPy[i0 - 1];
	Py[2] = PPy[i0 + 1];

	jmid = 1;
	gmid = g0[1];

	if (g0[0] > g0[2])
	{
		jmax = 0;
		gmax = g0[0];
		jmin = 2;
		gmin = g0[2];
	}
	else
	{
		jmax = 2;
		gmax = g0[2];
		jmin = 0;
		gmin = g0[0];
	}

	if (gmax < thr)
	{
		*pIntX = Px[jmax];
		*pIntY = Py[jmax];
	}
	else if (gmin > thr)
	{
		*pIntX = Px[jmin];
		*pIntY = Py[jmin];
	}
	//* * normal interpolation case
	else if (gmid >= thr)
	{
		dg = gmid - gmin;
		if (dg != 0)
		{
			dd = (g0[jmid] - thr) / dg;
			*pIntX = Px[jmid] - (Px[jmid] - Px[jmin])*dd;
			*pIntY = Py[jmid] - (Py[jmid] - Py[jmin])*dd;
		}
		else
		{
			*pIntX = (Px[jmid] + Px[jmin]) / 2.;
			*pIntX = (Py[jmid] + Py[jmin]) / 2.;
		}

	}
	else if (gmid < thr)
	{
		dg = gmax - gmid;
		if (dg != 0)
		{
			dd = (g0[jmax] - thr) / dg;
			*pIntX = Px[jmax] - (Px[jmid] - Px[jmax])*dd;
			*pIntY = Py[jmax] - (Py[jmid] - Py[jmax])*dd;
		}
		else
		{
			*pIntX = (Px[jmid] + Px[jmax]) / 2.;
			*pIntY = (Py[jmid] + Py[jmax]) / 2.;
		}
	}
}

// new version for space 20220908 
void SubPixCrdS(int *g0, int i0, int *PPx, int *PPy, float *pIntX, float *pIntY, float thr)
{
	//int PIntX[4], PIntY[4];
	int gmin, gmid, gmax, jmin, jmid, jmax; // points values and numbers
	int Px[3], Py[3];						// 3 conseq. points coordinates
	int dg;
	float dd;

	Px[1] = PPx[i0];
	Px[0] = PPx[i0 - 1];
	Px[2] = PPx[i0 + 1];
	Py[1] = PPy[i0];
	Py[0] = PPy[i0 - 1];
	Py[2] = PPy[i0 + 1];

	jmid = 1;
	gmid = g0[1];

	if (g0[0] > g0[2])
	{
		jmax = 0;
		gmax = g0[0];
		jmin = 2;
		gmin = g0[2];
	}
	else
	{
		jmax = 2;
		gmax = g0[2];
		jmin = 0;
		gmin = g0[0];
	}

	if (gmax < thr)
	{
		*pIntX = Px[jmax];
		*pIntY = Py[jmax];
	}
	else if (gmin > thr)
	{
		*pIntX = Px[jmin];
		*pIntY = Py[jmin];
	}
	//* * normal interpolation case
	else if (gmid > thr)
	{
		dg = gmid - gmin;
		if (dg != 0)
		{
			dd = (g0[jmid] - thr) / dg;
			*pIntX = Px[jmid] - (Px[jmid] - Px[jmin])*dd;
			*pIntY = Py[jmid] - (Py[jmid] - Py[jmin])*dd;
		}
		else
		{
			*pIntX = (Px[jmid] + Px[jmin]) / 2.;
			*pIntX = (Py[jmid] + Py[jmin]) / 2.;
		}
			
	}
	else if (gmid <= thr)
	{
		dg = gmax - gmid;
		if (dg != 0)
		{
			dd = (g0[jmax] - thr) / dg;
			*pIntX = Px[jmax] + (Px[jmid] - Px[jmax])*dd;
			*pIntY = Py[jmax] + (Py[jmid] - Py[jmax])*dd;
		}
		else
		{
			*pIntX = (Px[jmid] + Px[jmax]) / 2.;
			*pIntY = (Py[jmid] + Py[jmax]) / 2.;
		}
	}
}

// modified 20220323 from T&T_LG-LS3
void SubPixCrd(int *g0, int i0, int *PPx, int *PPy, float *pIntX, float *pIntY, float thr)
{
	//int PIntX[4], PIntY[4];
	if (((g0[0] > thr) && (g0[1] > thr) && (g0[2] > thr)) )
	{
		if (g0[0] <= g0[1] && g0[0] <= g0[2])
		{
			*pIntX = PPx[i0];
			*pIntY = PPy[i0];
		}
		else if (g0[1] <= g0[0] && g0[1] <= g0[2])
		{ 
			*pIntX = PPx[i0 + 1];
			*pIntY = PPy[i0 + 1];
		}
		else if(g0[2] <= g0[1] && g0[2] <= g0[0])
		{ 
			*pIntX = PPx[i0 - 1];
			*pIntY = PPy[i0 - 1];
		}
		//*pIntX = PPx[i0];
		//*pIntY = PPy[i0];
		return;
	}
	if((g0[0] < thr) && (g0[1] < thr) && (g0[2] < thr))
	{
		if (g0[0] >= g0[1] && g0[0] >= g0[2])
		{
			*pIntX = PPx[i0];
			*pIntY = PPy[i0];
		}
		else if (g0[1] >= g0[0] && g0[1] >= g0[2])
		{
			*pIntX = PPx[i0 + 1];
			*pIntY = PPy[i0 + 1];
		}
		else if (g0[2] >= g0[1] && g0[2] >= g0[0])
		{
			*pIntX = PPx[i0 - 1];
			*pIntY = PPy[i0 - 1];
		}
		//*pIntX = PPx[i0];
		//*pIntY = PPy[i0];
		return;
	}
	if ((g0[1] - g0[0]) > (g0[1] - g0[2]))
	{
		int dg10 = g0[1] - g0[0];
		if (dg10 != 0)
		{
			float dd0 = -(g0[1] - thr) / dg10;
			if (abs(dd0) < 1.)
			{ 
				*pIntX = PPx[i0] + (PPx[i0] - PPx[i0 - 1])*dd0;
				*pIntY = PPy[i0] + (PPy[i0] - PPy[i0 - 1])*dd0;
			}
			else
			{ 
				*pIntX = PPx[i0];
				*pIntY = PPy[i0];
			}
		}
		else
		{
			*pIntX = PPx[i0];
			*pIntY = PPy[i0];
		}
	}
	else
	{
		int dg20 = g0[2] - g0[1];
		if (dg20 != 0)
		{
			float dd0 = (g0[1] - thr) / dg20;
			if (abs(dd0) < 1.)
			{
				*pIntX = PPx[i0] + (PPx[i0] - PPx[i0 + 1])*dd0;
				*pIntY = PPy[i0] + (PPy[i0] - PPy[i0 + 1])*dd0;
			}
			else
			{
				*pIntX = PPx[i0];
				*pIntY = PPy[i0];
			}
		}
		else
		{
			*pIntX = PPx[i0];
			*pIntY = PPy[i0];
		}
	}
}

// before 20220323 
void SubPixCrd_o(int *g0, int i0, int *PPx, int *PPy, float *pIntX, float *pIntY, float thr)
{
	//int PIntX[4], PIntY[4];
	if (((g0[0] > thr) && (g0[1] > thr) && (g0[2] > thr)) || ((g0[0] < thr) && (g0[1] < thr) && (g0[2] < thr)))
	{
		*pIntX = PPx[i0];
		*pIntY = PPy[i0];
		return;
	}
	if ((g0[1] - g0[0]) > (g0[1] - g0[2]))
	{
		int dg10 = g0[1] - g0[0];
		if (dg10 != 0)
		{
			float dd0 = -(g0[1] - thr) / dg10;
			*pIntX = PPx[i0] + (PPx[i0] - PPx[i0 - 1])*dd0;
			*pIntY = PPy[i0] + (PPy[i0] - PPy[i0 - 1])*dd0;
		}
		else
		{
			*pIntX = PPx[i0];
			*pIntY = PPy[i0];
		}
	}
	else
	{
		int dg20 = g0[2] - g0[1];
		if (dg20 != 0)
		{
			float dd0 = (g0[1] - thr) / dg20;
			*pIntX = PPx[i0] + (PPx[i0] - PPx[i0 + 1])*dd0;
			*pIntY = PPy[i0] + (PPy[i0] - PPy[i0 + 1])*dd0;
		}
		else
		{
			*pIntX = PPx[i0];
			*pIntY = PPy[i0];
		}
	}

}

// Find subpixel MS intersection using Bresenham's line algorithm for Non-FWM with CPU 

// Find subpixel MS intersection using Bresenham's line algorithm for Non-FWM with CPU 
void CPU_NonFWM_Br15(UINT8 *pIm, UINT8 *pImIRB, int imw, int imh, int pst, int pn, float thr, float *dP1x, float *dP1y, float *dP2x, float *dP2y, int *dPnum, float *dW, float *dWmin, float *dPint1x, float *dPint1y, float *dPint2x, float *dPint2y)
{
	int x1, y1, x2, y2, xc, yc, pnum;
	int v, vp, v1, v2, i;
	float wmin, wmax;
	int PPx[128];  // Bresenham's points array
	int PPy[128];
	float PIntX[4] = { 0.,0.,0.,0. };
	float PIntY[4];
	int Pval[128];

	int pi1x = 0, pi1y = 0, pi2x = 0, pi2y = 0;
	int cIdx;
	int xb, yb;
	
	//int cIdx = threadIdx.x + blockIdx.x*blockDim.x;
	//int tid = blockDim.x * blockIdx.x + threadIdx.x;
	for (cIdx = pst; cIdx < pst + pn; cIdx++)
	{	//fw = *(d_P1xl + cIdx);
		//*(d_Wl + cIdx) = fw;
		//if (cIdx == 1883423)
		if (cIdx == 106781)
			cIdx = cIdx;
		//pnum = (*(dPnum + cIdx)) % 250 + 3;
		pnum = *(dPnum + cIdx);
		x1 = round(*(dP1x + cIdx)); // getting measurement sensor endpoints
		y1 = round(*(dP1y + cIdx));
		x2 = round(*(dP2x + cIdx));
		y2 = round(*(dP2y + cIdx));
		xc = x1; yc = y1;
		// begining (P1) coordinates
		xb = x1; yb = y1;
		//pnum = (*(dPnum + cIdx)) % 250 + 3;
		int deltaX = abs(x2 - x1);
		int deltaY = abs(y2 - y1);
		int signX = x1 < x2 ? 1 : -1;
		int signY = y1 < y2 ? 1 : -1;
		int error = deltaX - deltaY;
		int error2;
		int intind = 0; // intersection index (0 or 1)
		int pind = 0;   // B's point index [0...63]
		int xintc = x1, yintc = y1, xintp, yintp; // current and previous intersection points
		//v1 = *(pIm + y1 * imw + x1);  // 1st IRB pixel

		vp = 0;  // previous pixel
		/// filling in Bresenham's array
		while (x1 != x2 || y1 != y2)
		{
			PPx[pind] = x1;
			PPy[pind] = y1;
			v = *(pImIRB + y1 * imw + x1);
			Pval[pind] = v;
			error2 = error * 2;
			if (error2 > -deltaY)
			{
				error -= deltaY;
				x1 += signX;
			}
			if (error2 < deltaX)
			{
				error += deltaX;
				y1 += signY;
			}
			if (pind < 127)
				pind++;
			else
				break;
		} // while
		int np = 0;
		// Going forward from PS1
		int i0 = -1;
		int i1 = -1;
		int i2 = -1;
		int bfound = 0;
		for (i = 1; i < pind - 2; i++)
		{
			xc = PPx[i]; yc = PPy[i];
			v = Pval[i];
			if (v == pnum)
			{
				if (bfound == 0)
				{  // real def. beginning
					PIntX[0] = xc; PIntY[0] = yc;
					i0 = i;
					bfound = 1;
					np = 1;
				}
				else if (bfound == 2)
				{
					bfound = 1;
				}
				else if (bfound == 3)
				{
					PIntX[2] = xc; PIntY[2] = yc;
					i2 = i;
					bfound = 4;
					np = 3;
				}
			}
			else if (v == 1)
			{
				if (bfound == 0)
				{	// preliminary def. beginning
					PIntX[0] = xc; PIntY[0] = yc;
					i0 = i;
					bfound = 2;
					np = 1;
				}
			}
			else if (v == 0)
			{ // space pixel
				if (bfound == 1)
				{	// end of real def. beginning search
					PIntX[1] = PPx[i - 1]; PIntY[1] = PPy[i - 1];
					i1 = i - 1;
					np = 2;
					bfound = 3;
					/*if (np == 1)
						np = 2;*/
						//break;
				}
				else if (bfound == 2)
					bfound = 0;
			}
		}

		float PCX, PCY, wc;  // reference point and distance to P2 (wc)
		PCX = (xb + x2) / 2.;
		PCY = (yb + y2) / 2.;
		wc = sqrt((PCX - x2)*(PCX - x2) + (PCY - y2)*(PCY - y2));

		if (np == 0)
		{
			wmax = -wc;
			wmin = -wc;
			PIntX[0] = x2;
			PIntY[0] = y2;
			*(dW + 1 * cIdx) = wmax;
			*(dWmin + 1 * cIdx) = wmin;

			*(dPint1x + 1 * cIdx + 0) = PIntX[0];
			*(dPint2x + 1 * cIdx + 0) = PIntX[1];

			*(dPint1y + 1 * cIdx + 0) = PIntY[0];
			*(dPint2y + 1 * cIdx + 0) = PIntY[1];
			continue;

		}
		//** removing last intersection if P2 is outside of pattern
		if (Pval[pind - 1] == 0 && np > 1)
		{
			np--;
			PIntX[np] = 0;
			PIntY[np] = 0;

		}

		int g0[3], g1[3];
		int g2[3];
		float px, py;

		if (i0 == -1)
		{
			wmin = wmax = 0.0;
			//return;
		}
		else
		{
			int ppi00x = PPx[i0];
			int ppi00y = PPy[i0];
			int ppi0p1x = PPx[i0 + 1];
			int ppi0p1y = PPy[i0 + 1];
			int ppi0m1x = PPx[i0 - 1];
			int ppi0m1y = PPy[i0 - 1];
			g0[1] = *(pIm + ppi00y * imw + ppi00x);
			g0[0] = *(pIm + ppi0m1y * imw + ppi0m1x);
			g0[2] = *(pIm + ppi0p1y * imw + ppi0p1x);
			SubPixCrd(&g0[0], i0, &PPx[0], &PPy[0], &px, &py, thr);
			PIntX[0] = px;
			PIntY[0] = py;
		}

		if ((i1 >= 0) && (i1 < pind - 1))
		{
			int ppi10x = PPx[i1];
			int ppi10y = PPy[i1];
			int ppi1p1x = PPx[i1 + 1];
			int ppi1p1y = PPy[i1 + 1];
			int ppi1m1x = PPx[i1 - 1];
			int ppi1m1y = PPy[i1 - 1];
			g1[1] = *(pIm + ppi10y * imw + ppi10x);
			g1[0] = *(pIm + ppi1m1y * imw + ppi1m1x);
			g1[2] = *(pIm + ppi1p1y * imw + ppi1p1x);
			SubPixCrd(&g1[0], i1, &PPx[0], &PPy[0], &px, &py, thr);
			PIntX[1] = px;
			PIntY[1] = py;
		}
		if ((i2 >= 0) && (i2 < pind - 1))
		{
			int ppi20x = PPx[i2];
			int ppi20y = PPy[i2];
			int ppi2p1x = PPx[i2 + 1];
			int ppi2p1y = PPy[i2 + 1];
			int ppi2m1x = PPx[i2 - 1];
			int ppi2m1y = PPy[i2 - 1];
			g2[1] = *(pIm + PPy[i2] * imw + PPx[i2]);
			g2[0] = *(pIm + PPy[i2 - 1] * imw + PPx[i2 - 1]);
			g2[2] = *(pIm + PPy[i2 + 1] * imw + PPx[i2 + 1]);
			SubPixCrd(g2, i2, PPx, PPy, &px, &py, thr);
			PIntX[2] = px;
			PIntY[2] = py;
		}

		

		wmin = 0.;
		if (np > 0)
		{
			if (PIntX[0] != 0)
			{
				wmax = sqrt((float)(PIntX[0] - x2)*(float)(PIntX[0] - x2) + (float)(PIntY[0] - y2)*(float)(PIntY[0] - y2));
				wmin = wmax;
			}
			if (np > 1)
			{
				wmin = sqrt((x2 - PIntX[1])*(x2 - PIntX[1]) + (y2 - PIntY[1])*(y2 - PIntY[1]));

			}
			if (np > 2)
			{
				wmin = sqrt((x2 - PIntX[2])*(x2 - PIntX[2]) + (y2 - PIntY[2])*(y2 - PIntY[2]));

			}
			wmax = wmax - wc;
			wmin = wmin - wc;
		}
		else
		{
			wmax = 0.;
			wmin = 0.;
		}



		*(dW + 1 * cIdx) = wmax;
		*(dWmin + 1 * cIdx) = wmin;

		*(dPint1x + 1 * cIdx + 0) = PIntX[0];
		*(dPint2x + 1 * cIdx + 0) = PIntX[1];

		*(dPint1y + 1 * cIdx + 0) = PIntY[0];
		*(dPint2y + 1 * cIdx + 0) = PIntY[1];
	}
	//return;
///////////////////////////////////////////////

		/*if ((pi1x != 0) && (pi2x != 0))
			fw = (float)sqrt((pi1x - pi2x)*(pi1x - pi2x) + (pi1y - pi2y)*(pi1y - pi2y));
		else
			fw = 0.;
		*(dW + 1 * cIdx) = fw;*/
		//} // for
	cIdx = cIdx;
}

// Find MS intersection using Bresenham's line algorithm for CPU test with added endpoints analysis
void CPU_FWM_Br15(UINT8 *pIm, UINT8 *pImIRB, int imw, int imh, int pst, int pn, float thr, float *dP1x, float *dP1y, float *dP2x, float *dP2y, int *dPnum, float *dW, float *dWmin, float *dPint1x, float *dPint1y, float *dPint2x, float *dPint2y)
{
	int x1, y1, x2, y2, xc, yc, pnum;
	int v, vp, v1, v2, i;
	float wmin, wmax;
	int PPx[128];  // Bresenham's points array
	int PPy[128];
	float PIntX[4] = { 0.,0.,0.,0. };
	float PIntY[4];
	int Pval[128];

	int pi1x = 0, pi1y = 0, pi2x = 0, pi2y = 0;
	int cIdx;

	//int cIdx = threadIdx.x + blockIdx.x*blockDim.x;
	//int tid = blockDim.x * blockIdx.x + threadIdx.x;
	for (cIdx = pst; cIdx < pst + pn; cIdx++)
	{	//fw = *(d_P1xl + cIdx);
		//*(d_Wl + cIdx) = fw;
		//if (cIdx == 1883423)
		//if (cIdx == 157421)
		if (cIdx == 2110457)
			cIdx = cIdx;
		pnum = (*(dPnum + cIdx));
		x1 = round(*(dP1x + cIdx)); // getting measurement sensor endpoints
		y1 = round(*(dP1y + cIdx));
		x2 = round(*(dP2x + cIdx));
		y2 = round(*(dP2y + cIdx));
		//pnum = (*(dPnum + cIdx)) % 250 + 3;
		int deltaX = abs(x2 - x1);
		int deltaY = abs(y2 - y1);
		int signX = x1 < x2 ? 1 : -1;
		int signY = y1 < y2 ? 1 : -1;
		int error = deltaX - deltaY;
		int error2;
		int intind = 0; // intersection index (0 or 1)
		int pind = 0;   // B's point index [0...63]
		int xintc = x1, yintc = y1, xintp, yintp; // current and previous intersection points
		//v1 = *(pIm + y1 * imw + x1);  // 1st IRB pixel

		vp = 0;  // previous pixel
		/// filling in Bresenham's array
		while (x1 != x2 || y1 != y2)
		{
			PPx[pind] = x1;
			PPy[pind] = y1;
			v = *(pImIRB + y1 * imw + x1);
			Pval[pind] = v;
			error2 = error * 2;
			if (error2 > -deltaY)
			{
				error -= deltaY;
				x1 += signX;
			}
			if (error2 < deltaX)
			{
				error += deltaX;
				y1 += signY;
			}
			if (pind < 127)
				pind++;
			else
				break;
		} // while
		int np = 0;
		// Going forward from PS1
		int i0 = -1;
		int i2 = -1;
		int bfound = 0;
		for (i = 1; i < pind - 1; i++)
		{
			xc = PPx[i]; yc = PPy[i];
			v = Pval[i];
			if (v == pnum)
			{
				if (bfound == 0)
				{  // real def. beginning
					PIntX[0] = xc; PIntY[0] = yc;
					i0 = i;
					bfound = 1;
					/*if (np == 0)
						np = 1;*/
				}
				else if (bfound == 2)
				{
					bfound = 1;
				}
			}
			else if (v == 1)
			{
				if (bfound == 0)
				{	// preliminary def. beginning
					PIntX[0] = xc; PIntY[0] = yc;
					i0 = i;
					bfound = 2;
				}
			}
			else if (v == 0)
			{ // space pixel
				if (bfound == 1)
				{	// end of real def. beginning search
					PIntX[2] = PPx[i-1]; PIntY[2] = PPy[i - 1];
					i2 = i-1;
					np = 4;
					/*if (np == 1)
						np = 2;*/
					break;
				}
				else
					bfound = 0;
			}
		}
		// Going wackward from PS2
		int i1 = -1;
		int i3 = -1;
		int bfound2 = 0;
		for (i = pind - 2; i >= 0; i--)
		{
			xc = PPx[i]; yc = PPy[i];
			v = Pval[i];
			if (v == pnum)
			{
				if (bfound2 == 0)
				{  // real def. beginning
					PIntX[1] = xc; PIntY[1] = yc;
					i1 = i;
					bfound2 = 1;
					/*if (np == 1)
						np = 2;
					else if (np == 2)
						np = 3;*/
				}
				else if (bfound2 == 2)
				{
					bfound2 = 1;
				}
			}
			else if (v == 1)
			{
				if (bfound2 == 0)
				{	// preliminary def. beginning
					PIntX[1] = xc; PIntY[1] = yc;
					i1 = i;
					bfound2 = 2;
				}
			}
			else if (v == 0)
			{ // space pixel
				if (bfound2 == 1)
				{	// end of real def. beginning search
					PIntX[3] = PPx[i + 1]; PIntY[3] = PPy[i + 1];
					i3 = i + 1;
					np = 4;
					break;
					/*if (np == 2)
						np = 3;
					else if (np == 3)
						np = 4;*/
				}
				else
					bfound2 = 0;
			}
		}
		int g0[3], g1[3];
		int g2[3], g3[3];

		//if ((i0 == -1) || (i1 == -1) || (i2 == -1) || (i3 == -1))
		if ((i0 == -1) || (i1 == -1) )
		{
			wmin = wmax = 0.0;
			//return;
		}
		else
		{
			int ppi00x = PPx[i0];
			int ppi00y = PPy[i0];
			int ppi0p1x = PPx[i0 + 1];
			int ppi0p1y = PPy[i0 + 1];
			int ppi0m1x = PPx[i0 - 1];
			int ppi0m1y = PPy[i0 - 1];

			int ppi10x = PPx[i1];
			int ppi10y = PPy[i1];
			int ppi1p1x = PPx[i1 + 1];
			int ppi1p1y = PPy[i1 + 1];
			int ppi1m1x = PPx[i1 - 1];
			int ppi1m1y = PPy[i1 - 1];

			
			g0[1] = *(pIm + ppi00y * imw + ppi00x);
			g0[0] = *(pIm + ppi0m1y * imw + ppi0m1x);
			g0[2] = *(pIm + ppi0p1y * imw + ppi0p1x);
			g1[1] = *(pIm + ppi10y * imw + ppi10x);
			g1[0] = *(pIm + ppi1m1y * imw + ppi1m1x);
			g1[2] = *(pIm + ppi1p1y * imw + ppi1p1x);

			float px, py;
			SubPixCrd(&g0[0], i0, &PPx[0], &PPy[0], &px, &py, thr);
			PIntX[0] = px;
			PIntY[0] = py;
			SubPixCrd(&g1[0], i1, &PPx[0], &PPy[0], &px, &py, thr);
			PIntX[1] = px;
			PIntY[1] = py;

		}
		int twolines = 0;
		
		if (i2 > 0 && i2 != i1 && np > 2)
		{		
			//get_grayval(Im, PPy[i2], PPx[i2], g)
			g2[1] = *(pIm + PPy[i2] * imw + PPx[i2]);
			//get_grayval(Im, PPy[i2 - 1], PPx[i2 - 1], g)
				g2[0]  = *(pIm + PPy[i2 - 1] * imw + PPx[i2 - 1]);
				g2[2] = *(pIm + PPy[i2 + 1] * imw + PPx[i2 + 1]);
				 
				float px, py;
				SubPixCrd(g2, i2, PPx, PPy, &px, &py, thr);
				PIntX[2] = px;
				PIntY[2] = py;
				twolines = 1;
		}
		if (i3 > 0 && i3 != i0 && np > 2)
		{
			//get_grayval(Im, PPy[i2], PPx[i2], g)
			g3[1] = *(pIm + PPy[i3] * imw + PPx[i3]);
			//get_grayval(Im, PPy[i2 - 1], PPx[i2 - 1], g)
			g3[0] = *(pIm + PPy[i3 - 1] * imw + PPx[i3 - 1]);
			g3[2] = *(pIm + PPy[i3 + 1] * imw + PPx[i3 + 1]);

			float px, py;
			SubPixCrd(g3, i3, PPx, PPy, &px, &py, thr);
			PIntX[3] = px;
			PIntY[3] = py;
			twolines = 1;
		}
		wmin = 0.;
		if (np > 1)
		{
			if (PIntX[0] != 0 && PIntX[1] != 0 && (twolines == 0))
			{
				wmax = sqrt((float)(PIntX[0] - PIntX[1])*(float)(PIntX[0] - PIntX[1]) + (float)(PIntY[0] - PIntY[1])*(float)(PIntY[0] - PIntY[1]));
				wmin = wmax;
			}
			if (twolines == 1)
			{
				wmax = sqrt((PIntX[0] - PIntX[1])*(PIntX[0] - PIntX[1]) + (PIntY[0] - PIntY[1])*(PIntY[0] - PIntY[1]));
				// selecting line which includes skeleton
				xc = (x1 + x2) / 2;
				yc = (y1 + y2) / 2;
				int dX = x2 - x1;
				int dY = y2 - y1;
				if (dX != 0)
				{
					if (xc <= PIntX[0] && xc >= PIntX[2] || xc >= PIntX[0] && xc <= PIntX[2])
						wmin = sqrt((PIntX[0] - PIntX[2])*(PIntX[0] - PIntX[2]) + (PIntY[0] - PIntY[2])*(PIntY[0] - PIntY[2]));
					else if (xc <= PIntX[1] && xc >= PIntX[3] || xc >= PIntX[1] && xc <= PIntX[3])
						wmin = sqrt((PIntX[1] - PIntX[3])*(PIntX[1] - PIntX[3]) + (PIntY[1] - PIntY[3])*(PIntY[1] - PIntY[3]));
					else
						wmin = -1.;
				}
				else
				{
					if (yc <= PIntY[0] && yc >= PIntY[2] || yc >= PIntY[0] && yc <= PIntY[2])
						wmin = sqrt((PIntX[0] - PIntX[2])*(PIntX[0] - PIntX[2]) + (PIntY[0] - PIntY[2])*(PIntY[0] - PIntY[2]));
					else if (xc <= PIntX[1] && xc >= PIntX[3] || xc >= PIntX[1] && xc <= PIntX[3])
						wmin = sqrt((PIntX[1] - PIntX[3])*(PIntX[1] - PIntX[3]) + (PIntY[1] - PIntY[3])*(PIntY[1] - PIntY[3]));

					else
						wmin = -1;
				}
			}
		}
		else
		{
			wmax = 0.;
			wmin = 0.;
		}

		*(dW + 1 * cIdx) = wmax;
		*(dWmin + 1 * cIdx) = wmin;

		*(dPint1x + 1 * cIdx + 0) = PIntX[0];
		*(dPint2x + 1 * cIdx + 0) = PIntX[1];

		*(dPint1y + 1 * cIdx + 0) = PIntY[0];
		*(dPint2y + 1 * cIdx + 0) = PIntY[1];
	}
		//return;
///////////////////////////////////////////////
		
		/*if ((pi1x != 0) && (pi2x != 0))
			fw = (float)sqrt((pi1x - pi2x)*(pi1x - pi2x) + (pi1y - pi2y)*(pi1y - pi2y));
		else
			fw = 0.;
		*(dW + 1 * cIdx) = fw;*/
	//} // for
	//cIdx = cIdx;
}
// new version for Pattern 230308 - fixed neighboring same region wires, wmin (not finished), 0 inside pnum
void CPU_MSPtProc37(UINT8 *pIm, UINT8 *pImIRB, UINT8 *pImIRBS, UINT16 *pImCLC, int imw, int imh, int pst, int pn, float thr, int dclcmax, int pat0space1, float *dP1x, float *dP1y, float *dP2x, float *dP2y, int *dPnum, float *dW, float *dWmin, float *dPint1x, float *dPint1y, float *dPint2x, float *dPint2y, float *dShift, int *dPuseint)
{
	int x1, y1, x2, y2, xc, yc, pnum, useint;
	int v, vp, vn, v1, v2, i, j;
	float wmin, wmax;		 // for width calculation
	float shift = 0.0, xmid, ymid; // for shift calculation
	int PPx[128];  // Bresenham's MS points array
	int PPy[128];
	int Pval[128]; // pnum values array for all MS points


	int g, gp; // current and previous gray values
	int jSPdn;
	int jSPup;
	int jMBdn;
	int jMBup;
	int cIdx; // MS index

	float M1[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int iM1[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int iM0[8] = { 0,0,0,0,0,0,0,0 };
	float PIntX[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. }; // intersection points coordinates
	float PIntY[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	float FIntX[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. }; // intersection points coordinates (subpixel)
	float FIntY[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	float fIntX, fIntY; // subpix coord. measured by 
	int Psubpix[8] = { 0,0,0,0,0,0,0,0 };
	int PInti[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };	  // index array of intersection points 
	int PCLC[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };   // CLC array of intersection points 
	int PType[8] = { 0,0,0,0,0,0,0,0 };
	int Pnums[8] = { 0,0,0,0,0,0,0,0 };
	int vpnums;
	//** Intersection point IRBS val,
	int	PIRBS[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int irbs;
	float ixc;
	float iyc;
	int ptype = 0, begtype = 0, pnfound = 0, btype;
	int PB[8];
	int PE[8];
	int ib = 0;
	int ie = 0;
	//int cIdx = threadIdx.x + blockIdx.x*blockDim.x;
	//int tid = blockDim.x * blockIdx.x + threadIdx.x;
	for (cIdx = pst; cIdx < pst + pn; cIdx++)
	{
		ib = 0;
		ie = 0;
		if (cIdx == 615549)
			cIdx = cIdx;
		memcpy(&PIntX[0], &M1[0], 8 * sizeof(float));
		memcpy(&PIntY[0], &M1[0], 8 * sizeof(float));
		memcpy(&FIntX[0], &M1[0], 8 * sizeof(float));
		memcpy(&FIntY[0], &M1[0], 8 * sizeof(float));
		memcpy(&PInti[0], &iM1[0], 8 * sizeof(int));
		memcpy(&PCLC[0], &iM1[0], 8 * sizeof(int));
		memcpy(&Psubpix[0], &iM0[0], 8 * sizeof(int));
		memcpy(&PType[0], &iM0[0], 8 * sizeof(int));
		//fw = *(d_P1xl + cIdx);
		//*(d_Wl + cIdx) = fw;
		//if (cIdx == 1883423)
		//if (cIdx == 157421)
		/*if (cIdx == 162006)
			cIdx = cIdx;*/
			//int ii = 0;

		int ii = 0, clc, pclc = -2, fi1 = 0;
		pnum = (*(dPnum + cIdx));
		//useint = (*(dPuseint + cIdx));
		x1 = round(*(dP1x + cIdx)); // getting measurement sensor endpoints
		y1 = round(*(dP1y + cIdx));
		x2 = round(*(dP2x + cIdx));
		y2 = round(*(dP2y + cIdx));
		ixc = (x1 + x2) / 2.;
		iyc = (y1 + y2) / 2.;
		//pnum = (*(dPnum + cIdx)) % 250 + 3;
		int deltaX = abs(x2 - x1);
		int deltaY = abs(y2 - y1);
		int signX = x1 < x2 ? 1 : -1;
		int signY = y1 < y2 ? 1 : -1;
		int error = deltaX - deltaY;
		int error2;
		int intind = 0; // intersection index (0 or 1)
		int pind = 0;   // B's point index [0...63]
		int xintc = x1, yintc = y1; // current and previous intersection points
		//v1 = *(pIm + y1 * imw + x1);  // 1st IRB pixel


		vp = 0;  // previous pixel
		/// filling in Bresenham's array
		while (x1 != x2 || y1 != y2)
		{
			PPx[pind] = x1;
			PPy[pind] = y1;
			v = *(pImIRB + y1 * imw + x1);
			Pval[pind] = v;
			error2 = error * 2;
			if (error2 > -deltaY)
			{
				error -= deltaY;
				x1 += signX;
			}
			if (error2 < deltaX)
			{
				error += deltaX;
				y1 += signY;
			}
			if (pind < 127)
				pind++;
			else
				break;
		} // while
		// Going forward from PS1

		ptype = begtype = pnfound = 0;
		vp = Pval[0];
		for (i = 1; i <= pind - 2; i++)
		{
			xc = PPx[i]; yc = PPy[i];
			v = Pval[i];
			vn = Pval[i + 1];
			//irbs = *(pImIRBS + yc * imw + xc);  // IRBSC pixel value
			if (i == 1 && v == 1)
			{
				ptype = 1;
				PIntY[ii] = yc;
				PIntX[ii] = xc;
				PInti[ii] = i;
				PCLC[ii] = -2;
				begtype = 1;
				ii = ii + 1;
			}
			if (v == 0)
			{
				begtype = 0;
				pnfound = 0;
			}
			if (v > 1 && v != pnum && begtype == 1)
			{
				begtype = 0;
				pnfound = 0;
				ii = ii - 1;
			}
			btype = 0;
			if (v == pnum)
			{
				if (vp == 0 && (vn == pnum || vn == 1 || vn == 0))
					btype = 1;
				else if ((vp == 1 && vp != pnum) || (vn == 0 && vp != 0))

					btype = -1;
				else
				{
					btype = 0;
					continue;
				}
				PType[ii] = btype;
				pnfound = 1;
				PIntY[ii] = yc;
				PIntX[ii] = xc;
				PInti[ii] = i;
				Psubpix[ii] = 1;
				if (ii < 7)
					ii = ii + 1;
			}
			if (i == pind - 1 && v == 1 && pnfound == 1)
			{
				PIntY[ii] = yc;
				PIntX[ii] = xc;
				PInti[ii] = i;
				PCLC[ii] = -2;
				ii = ii + 1;
			}
			vp = v;
		}
		//********************************************************************
		int ga[3]; // array of 3 neighboring g
		int nmwc = 0;//*number of MW (main wire) candidates
		int mw = -1;//* MW segment
		int dclc, dmin = 55555; // min CLC distance
		int d, d1, d2; // CLC distances
		int intnum = ii; /// number of intersection points found
		jSPdn = 0;
		float dif0, dif1;
		//if (ii < 7)
		//{
		intnum = ii;
		jSPdn = 0;
		jSPup = ii - 1;
		//}
		/*else
		{
			intnum = 7;
			jSPup = 7;
		}*/

		if (intnum <= 1) //** single pix. space case
		{
			wmin = -1.;
			wmax = -1.;
			jSPdn = 0;
			jSPup = 0;
		}
		else
		{ ///**** NORMAL CASE ******
	
			for (j = 0; j < ii; j++)
			{
				if (PType[j] == 1)
					PB[ib++] = j;
				//ib : = ib + 1
				else if (PType[j] == -1)
					PE[ie++] = j;
				//ie : = ie + 1

			}
			jSPdn = PB[0];
			jSPup = PE[ie - 1];
			if (jSPup == 0 || jSPup == 1 || jSPup == 2)
			{
				jSPup = jSPup;
			}
			else
			{
				jSPup = intnum - 1;
			}
			//*****
			if (intnum < 3)
			{
				jMBdn = jSPdn;
				jMBup = jSPup;
			}
			else if (intnum == 4)
			{
				dif0 = abs(PCLC[PB[0]] - PCLC[PE[0]]);
				dif1 = abs(PCLC[PB[1]] - PCLC[PE[1]]);
				if (dif0 > dif1)
				{
					jMBdn = PB[0];
					jMBup = PE[0];
				}
				else
				{
					jMBdn = PB[1];
					jMBup = PE[1];
				}
			}
			else
			{
				jMBdn = jSPdn;
				jMBup = jSPup;
			}
			//*******

			//* *************** SUBPIX COORD. ******************************
			for (j = 0; j < intnum; j++)
			{
				if (PInti[j] >= 0)
				{
					ga[1] = *(pIm + PPy[PInti[j]] * imw + PPx[PInti[j]]);
					if (PInti[j] > 0)
						ga[0] = *(pIm + PPy[PInti[j] - 1] * imw + PPx[PInti[j] - 1]);
					else
						ga[0] = ga[1];
					if (PInti[j] < pind - 1)
						ga[2] = *(pIm + PPy[PInti[j] + 1] * imw + PPx[PInti[j] + 1]);
					else
						ga[2] = ga[1];
					if (PInti[j] < pind - 1 && PInti[j]>0)
					{
						SubPixCrdP(ga, PInti[j], PPx, PPy, &fIntX, &fIntY, thr);
					}
					else
					{
						fIntX = PPx[PInti[j]];
						fIntY = PPy[PInti[j]];
					}
					FIntX[j] = fIntX;
					FIntY[j] = fIntY;

				}
			}

			//*************************************************
			wmax = sqrt((FIntX[jSPup] - FIntX[jSPdn])*(FIntX[jSPup] - FIntX[jSPdn]) + (FIntY[jSPup] - FIntY[jSPdn])*(FIntY[jSPup] - FIntY[jSPdn]));
			//* wmax : = sqrt((FIntX[intnum - 1] - FIntX[0])*(FIntX[intnum - 1] - FIntX[0]) + (FIntY[intnum - 1] - FIntY[0])*(FIntY[intnum - 1] - FIntY[0]))
			//wmin = sqrt((FIntX[mw] - FIntX[mw + 1])*(FIntX[mw] - FIntX[mw + 1]) + (FIntY[mw] - FIntY[mw + 1])*(FIntY[mw] - FIntY[mw + 1]));
			wmin = sqrt((FIntX[jMBup] - FIntX[jMBdn])*(FIntX[jMBup] - FIntX[jMBdn]) + (FIntY[jMBup] - FIntY[jMBdn])*(FIntY[jMBup] - FIntY[jMBdn]));
			if (wmax > 3.0)
			{ // calculating shift
				xmid = (FIntX[jSPup] + FIntX[jSPdn]) / 2.;
				ymid = (FIntY[jSPup] + FIntY[jSPdn]) / 2.;
				shift = sqrt((ixc - xmid)*(ixc - xmid) + (iyc - ymid)*(iyc - ymid));
			}

			//} // **** normal case end ***


		}

		*(dW + cIdx) = wmax;
		*(dWmin + cIdx) = wmin;
		*(dShift + cIdx) = shift;

		*(dPint1x + cIdx) = PIntX[jSPup];
		*(dPint2x + cIdx) = PIntX[jSPdn];

		*(dPint1y + cIdx) = PIntY[jSPup];
		*(dPint2y + cIdx) = PIntY[jSPdn];

		/// saving number of intersection points ( 2 points) (left name Puseint for simplicity of changes)
		*(dPuseint + cIdx) = intnum;
		//********************************************	
	} // index cycle end
}

// // new version for Space  2 Pnum (from Pat 36)
void CPU_MSSpProc36B2(UINT8 *pIm, UINT8 *pImIRBS, UINT16 *pImCLCS, int imw, int imh, int pst, int pn, int pn2, 
	float thr, int dclcmax, float *dP1x, float *dP1y, float *dP2x, float *dP2y, int *dPnum, int *dPnum2, 
	float *dW, float *dWmin, float *dPint1x, float *dPint1y, float *dPint2x, float *dPint2y, float *dShift, int *dPuseint)
{
	int x1, y1, x2, y2, xc, yc, pnum, pnum2, useint;
	int v, vp, vn, v1, v2, i, j;
	float wmin, wmax;		 // for width calculation
	float shift = 0.0, xmid, ymid; // for shift calculation
	int PPx[128];  // Bresenham's MS points array
	int PPy[128];
	int Pval[128]; // pnum values array for all MS points


	int g, gp; // current and previous gray values
	int jSPdn;
	int jSPup;
	int cIdx; // MS index

	float M1[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int iM1[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int iM0[8] = { 0,0,0,0,0,0,0,0 };
	float PIntX[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. }; // intersection points coordinates
	float PIntY[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	float FIntX[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. }; // intersection points coordinates (subpixel)
	float FIntY[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	float fIntX, fIntY; // subpix coord. measured by 
	int Psubpix[8] = { 0,0,0,0,0,0,0,0 };
	int PInti[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };	  // index array of intersection points 
	int PCLC[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };   // CLC array of intersection points 
	int PType[8] = { 0,0,0,0,0,0,0,0 };
	int Pnums[8] = { 0,0,0,0,0,0,0,0 };
	int vpnums;
	//** Intersection point IRBS val,
	int	PIRBS[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int irbs;
	float ixc;
	float iyc;
	int ptype = 0, begtype = 0, pnfound = 0, btype;
	int PB[8];
	int PE[8];
	int ib = 0;
	int ie = 0;
	//int cIdx = threadIdx.x + blockIdx.x*blockDim.x;
	//int tid = blockDim.x * blockIdx.x + threadIdx.x;
	for (cIdx = pst; cIdx < pst + pn; cIdx++)
	{
		ib = 0;
		ie = 0;
		/*if (cIdx == 23268)
			cIdx = cIdx;*/
		memcpy(&PIntX[0], &M1[0], 8 * sizeof(float));
		memcpy(&PIntY[0], &M1[0], 8 * sizeof(float));
		memcpy(&FIntX[0], &M1[0], 8 * sizeof(float));
		memcpy(&FIntY[0], &M1[0], 8 * sizeof(float));
		memcpy(&PInti[0], &iM1[0], 8 * sizeof(int));
		memcpy(&PCLC[0], &iM1[0], 8 * sizeof(int));
		memcpy(&Psubpix[0], &iM0[0], 8 * sizeof(int));
		memcpy(&PType[0], &iM0[0], 8 * sizeof(int));
		//fw = *(d_P1xl + cIdx);
		//*(d_Wl + cIdx) = fw;
		//if (cIdx == 1883423)
		//if (cIdx == 157421)
		/*if (cIdx == 162006)
			cIdx = cIdx;*/
			//int ii = 0;

		int ii = 0, clc, pclc = -2, fi1 = 0;
		pnum = (*(dPnum + cIdx));
		pnum2 = (*(dPnum2 + cIdx));
		//useint = (*(dPuseint + cIdx));
		x1 = round(*(dP1x + cIdx)); // getting measurement sensor endpoints
		y1 = round(*(dP1y + cIdx));
		x2 = round(*(dP2x + cIdx));
		y2 = round(*(dP2y + cIdx));
		ixc = (x1 + x2) / 2.;
		iyc = (y1 + y2) / 2.;

		int deltaX = abs(x2 - x1);
		int deltaY = abs(y2 - y1);
		int signX = x1 < x2 ? 1 : -1;
		int signY = y1 < y2 ? 1 : -1;
		int error = deltaX - deltaY;
		int error2;
		int intind = 0; // intersection index (0 or 1)
		int pind = 0;   // B's point index [0...63]
		int xintc = x1, yintc = y1; // current and previous intersection points
		//v1 = *(pIm + y1 * imw + x1);  // 1st IRB pixel


		vp = 0;  // previous pixel
		/// filling in Bresenham's array
		while (x1 != x2 || y1 != y2)
		{
			PPx[pind] = x1;
			PPy[pind] = y1;
			v = *(pImIRBS + y1 * imw + x1);
			Pval[pind] = v;
			error2 = error * 2;
			if (error2 > -deltaY)
			{
				error -= deltaY;
				x1 += signX;
			}
			if (error2 < deltaX)
			{
				error += deltaX;
				y1 += signY;
			}
			if (pind < 127)
				pind++;
			else
				break;
		} // while
		// Going forward from PS1

		ptype = begtype = pnfound = 0;
		vp = Pval[0];
		for (i = 1; i <= pind - 2; i++)
		{
			xc = PPx[i]; yc = PPy[i];
			v = Pval[i];
			vn = Pval[i + 1];
			//irbs = *(pImIRBS + yc * imw + xc);  // IRBSC pixel value
			if (i == 1 && v == 1)
			{
				ptype = 1;
				PIntY[ii] = yc;
				PIntX[ii] = xc;
				PInti[ii] = i;
				PCLC[ii] = -2;
				begtype = 1;
				ii = ii + 1;
			}
			if (v == 0)
			{
				begtype = 0;
				pnfound = 0;
			}
			if (v > 1 && v != pnum && v != pnum2 && begtype == 1)
			{
				begtype = 0;
				pnfound = 0;
				ii = ii - 1;
			}
			btype = 0;
			if (v == pnum || v == pnum2)
			{
				if (vp == 0 && (vn == pnum || vn == pnum2 || vn == 1))
					btype = 1;
				else if ((vp == 1 && vp != pnum && vp != pnum2) || (vn == 0 && vp != 0))

					btype = -1;
				else
				{
					btype = 0;
					continue;
				}
				PType[ii] = btype;
				pnfound = 1;
				PIntY[ii] = yc;
				PIntX[ii] = xc;
				PInti[ii] = i;
				Psubpix[ii] = 1;
				if (ii < 7)
					ii = ii + 1;
			}
			if (i == pind - 1 && v == 1 && pnfound == 1)
			{
				PIntY[ii] = yc;
				PIntX[ii] = xc;
				PInti[ii] = i;
				PCLC[ii] = -2;
				ii = ii + 1;
			}
			vp = v;
		}
		//********************************************************************
		int ga[3]; // array of 3 neighboring g
		int nmwc = 0;//*number of MW (main wire) candidates
		int mw = -1;//* MW segment
		int dclc, dmin = 55555; // min CLC distance
		int d, d1, d2; // CLC distances
		int intnum = ii; /// number of intersection points found
		jSPdn = 0;

		//if (ii < 7)
		//{
		intnum = ii;
		jSPdn = 0;
		jSPup = ii - 1;
		//}
		/*else
		{
			intnum = 7;
			jSPup = 7;
		}*/

		if (intnum <= 1) //** single pix. space case
		{
			wmin = 10.;
			wmax = 10.;
			jSPdn = 0;
			jSPup = 0;
		}
		else
		{ ///**** NORMAL CASE ******
	//**************** SUBPIX COORD. ******************************
			for (j = 0; j < ii; j++)
			{
				if (PType[j] == 1)
					PB[ib++] = j;
				//ib : = ib + 1
				else if (PType[j] == -1)
					PE[ie++] = j;
				//ie : = ie + 1

			}
			if (ib == 0 || ie == 0)
			{
				wmin = 11.;
				wmax = 11.;
				jSPdn = 0;
				jSPup = 0;
			}
			else
			{
				jSPdn = PB[0];
				jSPup = PE[ie - 1];
				if (jSPdn == 0 || jSPdn == 1 || jSPdn == 2)
				{
					jSPdn = jSPdn;
				}
				else
				{
					jSPdn = intnum - 2;
				}
				for (j = 0; j < intnum; j++)
				{
					ga[1] = *(pIm + PPy[PInti[j]] * imw + PPx[PInti[j]]);
					if (PInti[j] > 0)
						ga[0] = *(pIm + PPy[PInti[j] - 1] * imw + PPx[PInti[j] - 1]);
					else
						ga[0] = ga[1];
					if (PInti[j] < pind - 1)
						ga[2] = *(pIm + PPy[PInti[j] + 1] * imw + PPx[PInti[j] + 1]);
					else
						ga[2] = ga[1];
					if (PInti[j] < pind - 1 && PInti[j]>0)
					{
						SubPixCrdS(ga, PInti[j], PPx, PPy, &fIntX, &fIntY, thr);
					}
					else
					{
						fIntX = PPx[PInti[j]];
						fIntY = PPy[PInti[j]];
					}
					FIntX[j] = fIntX;
					FIntY[j] = fIntY;
				}

				//*************************************************
				wmax = sqrt((FIntX[jSPup] - FIntX[jSPdn])*(FIntX[jSPup] - FIntX[jSPdn]) + (FIntY[jSPup] - FIntY[jSPdn])*(FIntY[jSPup] - FIntY[jSPdn]));
				//* wmax : = sqrt((FIntX[intnum - 1] - FIntX[0])*(FIntX[intnum - 1] - FIntX[0]) + (FIntY[intnum - 1] - FIntY[0])*(FIntY[intnum - 1] - FIntY[0]))
				//wmin = sqrt((FIntX[mw] - FIntX[mw + 1])*(FIntX[mw] - FIntX[mw + 1]) + (FIntY[mw] - FIntY[mw + 1])*(FIntY[mw] - FIntY[mw + 1]));
				wmin = wmax;

			}


			if (wmax > 3.0)
			{ // calculating shift
				xmid = (FIntX[jSPup] + FIntX[jSPdn]) / 2.;
				ymid = (FIntY[jSPup] + FIntY[jSPdn]) / 2.;
				shift = sqrt((ixc - xmid)*(ixc - xmid) + (iyc - ymid)*(iyc - ymid));
			}

			//} // **** normal case end ***


		}
		*(dW + cIdx) = wmax;
		*(dWmin + cIdx) = wmin;
		*(dShift + cIdx) = shift;

		*(dPint1x + cIdx) = PIntX[jSPup];
		*(dPint2x + cIdx) = PIntX[jSPdn];

		*(dPint1y + cIdx) = PIntY[jSPup];
		*(dPint2y + cIdx) = PIntY[jSPdn];

		/// saving number of intersection points ( 2 points) (left name Puseint for simplicity of changes)
		*(dPuseint + cIdx) = intnum;
		//********************************************	
	} // index cycle end
}

// new version for Pattern 231024 -fixed multiple pnums in a row, fixed neighboring same region wires
void CPU_MSPtProc39B2(UINT8 *pIm, UINT8 *pImIRB, UINT16 *pImCLC, int imw, int imh, int pst, int pn, int pn2,
	float thr, int dclcmax, float *dP1x, float *dP1y, float *dP2x, float *dP2y, int *dPnum, int *dPnum2,
	float *dW, float *dWmin, float *dPint1x, float *dPint1y, float *dPint2x, float *dPint2y, float *dShift, int *dPuseint)
{
	int x1, y1, x2, y2, xc, yc, pnum, pnum2, useint;
	int v, vp, vn, v1, v2, i, j;
	float wmin, wmax;		 // for width calculation
	float shift = 0.0, xmid, ymid; // for shift calculation
	int PPx[128];  // Bresenham's MS points array
	int PPy[128];
	int Pval[128]; // pnum values array for all MS points


	int g, gp; // current and previous gray values
	int jSPdn;
	int jSPup;
	int jMBdn;
	int jMBup;
	int cIdx; // MS index
	int dif0;
	int dif1;

	float M1[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int iM1[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int iM0[8] = { 0,0,0,0,0,0,0,0 };
	float PIntX[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. }; // intersection points coordinates
	float PIntY[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	float FIntX[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. }; // intersection points coordinates (subpixel)
	float FIntY[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	float fIntX, fIntY; // subpix coord. measured by 
	int Psubpix[8] = { 0,0,0,0,0,0,0,0 };
	int PInti[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };	  // index array of intersection points 
	int PCLC[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };   // CLC array of intersection points 
	int PType[8] = { 0,0,0,0,0,0,0,0 };
	int Pnums[8] = { 0,0,0,0,0,0,0,0 };
	int vpnums;
	//** Intersection point IRBS val,
	int	PIRBS[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int irbs;
	float ixc;
	float iyc;
	int ptype = 0, begtype = 0, pnfound = 0, btype;
	int PB[8];
	int PE[8];
	int ib = 0;
	int ie = 0;
	//int cIdx = threadIdx.x + blockIdx.x*blockDim.x;
	//int tid = blockDim.x * blockIdx.x + threadIdx.x;
	for (cIdx = pst; cIdx < pst + pn; cIdx++)
	{
		ib = 0;
		ie = 0;
		if (cIdx == 615549)
			cIdx = cIdx;
		memcpy(&PIntX[0], &M1[0], 8 * sizeof(float));
		memcpy(&PIntY[0], &M1[0], 8 * sizeof(float));
		memcpy(&FIntX[0], &M1[0], 8 * sizeof(float));
		memcpy(&FIntY[0], &M1[0], 8 * sizeof(float));
		memcpy(&PInti[0], &iM1[0], 8 * sizeof(int));
		memcpy(&PCLC[0], &iM1[0], 8 * sizeof(int));
		memcpy(&Psubpix[0], &iM0[0], 8 * sizeof(int));
		memcpy(&PType[0], &iM0[0], 8 * sizeof(int));
		//fw = *(d_P1xl + cIdx);
		//*(d_Wl + cIdx) = fw;
		//if (cIdx == 1883423)
		//if (cIdx == 157421)
		/*if (cIdx == 162006)
			cIdx = cIdx;*/
			//int ii = 0;

		int ii = 0, clc, pclc = -2, fi1 = 0, ip;
		int skipnext = 0;
		pnum = (*(dPnum + cIdx));
		pnum2 = (*(dPnum2 + cIdx));
		//useint = (*(dPuseint + cIdx));
		x1 = round(*(dP1x + cIdx)); // getting measurement sensor endpoints
		y1 = round(*(dP1y + cIdx));
		x2 = round(*(dP2x + cIdx));
		y2 = round(*(dP2y + cIdx));
		ixc = (x1 + x2) / 2.;
		iyc = (y1 + y2) / 2.;
		//pnum = (*(dPnum + cIdx)) % 250 + 3;
		int deltaX = abs(x2 - x1);
		int deltaY = abs(y2 - y1);
		int signX = x1 < x2 ? 1 : -1;
		int signY = y1 < y2 ? 1 : -1;
		int error = deltaX - deltaY;
		int error2;
		int intind = 0; // intersection index (0 or 1)
		int pind = 0;   // B's point index [0...63]
		int xintc = x1, yintc = y1; // current and previous intersection points
		//v1 = *(pIm + y1 * imw + x1);  // 1st IRB pixel


		vp = 0;  // previous pixel
		/// filling in Bresenham's array
		while (x1 != x2 || y1 != y2)
		{
			PPx[pind] = x1;
			PPy[pind] = y1;
			v = *(pImIRB + y1 * imw + x1);
			Pval[pind] = v;
			error2 = error * 2;
			if (error2 > -deltaY)
			{
				error -= deltaY;
				x1 += signX;
			}
			if (error2 < deltaX)
			{
				error += deltaX;
				y1 += signY;
			}
			if (pind < 127)
				pind++;
			else
				break;
		} // while
		
		  // Going forward from PS1
		ptype = begtype = pnfound = 0;
		vp = Pval[0];
		for (i = 1; i <= pind - 2; i++)
		{
			if (skipnext == 1)
			{
				skipnext = 0;
				continue;
			}
			xc = PPx[i]; yc = PPy[i];
			v = Pval[i];
			vn = Pval[i + 1];
			//irbs = *(pImIRBS + yc * imw + xc);  // IRBSC pixel value
			if (i == 1 && v == 1)
			{
				ptype = 1;
				PIntY[ii] = yc;
				PIntX[ii] = xc;
				PInti[ii] = i;
				PCLC[ii] = -2;
				begtype = 1;
				ii = ii + 1;
			}
			if (i == pind - 2 && v == 1)
			{ //**** inside of Spur ***
				ptype = 1;
				PIntY[ii] = yc;
				PIntX[ii] = xc;
				PInti[ii] = i;
				PCLC[ii] = -2;
				/*begtype = 1;
				ii = ii + 1;*/
			}
			if (v == 0)
			{ // *** reset ***
				begtype = 0;
				pnfound = 0;
			}
			if (v > 1 && v != pnum && v != pnum2 && begtype == 1)
			{ // *** met wrong region, so reset ***
				begtype = 0;
				pnfound = 0;
				ii = ii - 1;
			}
			//btype = 0;
			if (v == pnum || v == pnum2)
			{
				//*** 2 points ******
				//* changed index ip
				//ip = ii;
				if ((vp == pnum ||vp == pnum2) && (vn == pnum || vn == pnum2))
					continue;
				else if (vp == 0 && (vn == pnum || vn == pnum2 || vn == 1 || vn == 0))
					btype = 1;
				else if ((vp == 1 && vp != pnum && vp != pnum2) || (vn == 0 && vp != 0))
					btype = -1;
				else if ((vn == 1 && ((vp == pnum) || vp == pnum2)) || (vp == 1 && (vn == pnum ||vn == pnum2)))
					btype = 1;
				else
				{
					btype = 0;
					//continue;
				}
				PType[ii] = btype;
				pnfound = 1;
				if (btype == 0)
				{
					continue;
				}
				PIntY[ii] = yc;
				PIntX[ii] = xc;
				PInti[ii] = i;
				//Psubpix[ii] = 1;
				if (ii < 7)
					ii = ii + 1;
			}
			if (i == pind - 2 && (v == 1 || v == pnum || v == pnum2) && pnfound == 1)
			{
				PIntY[ii] = yc;
				PIntX[ii] = xc;
				PInti[ii] = i;
				PCLC[ii] = -2;
				PType[ii] = -1;
				if (ii < 7)
					ii = ii + 1;
			}
			vp = v;
		}
		//********************************************************************
		int ga[3]; // array of 3 neighboring g
		int nmwc = 0;//*number of MW (main wire) candidates
		int mw = -1;//* MW segment
		int dclc, dmin = 55555; // min CLC distance
		int d, d1, d2; // CLC distances
		int intnum = ii; /// number of intersection points found
		jSPdn = 0;

		//if (ii < 7)
		//{
		intnum = ii;
		jSPdn = 0;
		jSPup = ii - 1;

		if (intnum == 0)
		{
			wmax = 0.0;
			wmin = 0.0;
			shift = 0.0;
			//return ;
		}
		else if (intnum == 1)
		{
			wmax = 1.0;
			wmin = 1.0;
			shift = 0.0;
			//return ;
		}
		else  ///**** NORMAL CASE ******
		{


			// * *** finding parts beg. and ends
			ib = 0;
			ie = 0;
			for (j = 0; j <= ii - 1; j++)
			{
				if (PType[j] == 1)
				{
					PB[ib] = j;
					ib = ib + 1;
				}
				else if (PType[j] == -1)
				{
					PE[ie] = j;
					ie = ie + 1;
				}
			}
			// ***Hong
			if (jSPdn == 0 || jSPdn == 1 || jSPdn == 2)
				jSPdn = jSPdn;
			else
				jSPdn = intnum - 2;
			// *****			



			if (intnum < 3)
			{
				jMBdn = jSPdn;
				jMBup = jSPup;
			}
			else if (intnum == 4)
			{
				dif0 = abs(PCLC[PB[0]] - PCLC[PE[0]]);
				dif1 = abs(PCLC[PB[1]] - PCLC[PE[1]]);
				if (dif0 > dif1)
				{
					jMBdn = PB[0];
					jMBup = PE[0];

				}
				else
				{
					jMBdn = PB[1];
					jMBup = PE[1];
				}
			}
			else
			{
				jMBdn = jSPdn;
				jMBup = jSPup;
			}


			///**** NORMAL CASE ******
	   //**************** SUBPIX COORD. ******************************
			for (j = 0; j < ii; j++)
			{
				if (PType[j] == 1)
					PB[ib++] = j;
				//ib : = ib + 1
				else if (PType[j] == -1)
					PE[ie++] = j;
				//ie : = ie + 1

			}
			jSPdn = PB[0];
			jSPup = PE[ie - 1];
			if (jSPup == 0 || jSPup == 1 || jSPup == 2)
			{
				jSPup = jSPup;
			}
			else
			{
				jSPup = intnum - 1;
			}
			for (j = 0; j < intnum; j++)
			{
				if (PInti[j] >= 0)
				{
					ga[1] = *(pIm + PPy[PInti[j]] * imw + PPx[PInti[j]]);
					if (PInti[j] > 0)
						ga[0] = *(pIm + PPy[PInti[j] - 1] * imw + PPx[PInti[j] - 1]);
					else
						ga[0] = ga[1];
					if (PInti[j] < pind - 1)
						ga[2] = *(pIm + PPy[PInti[j] + 1] * imw + PPx[PInti[j] + 1]);
					else
						ga[2] = ga[1];
					if (PInti[j] < pind - 1 && PInti[j]>0)
					{
						SubPixCrdP(ga, PInti[j], PPx, PPy, &fIntX, &fIntY, thr);
					}
					else
					{
						fIntX = PPx[PInti[j]];
						fIntY = PPy[PInti[j]];
					}
					FIntX[j] = fIntX;
					FIntY[j] = fIntY;

				}
			}

			//*************************************************
			wmax = sqrt((FIntX[jSPup] - FIntX[jSPdn])*(FIntX[jSPup] - FIntX[jSPdn]) + (FIntY[jSPup] - FIntY[jSPdn])*(FIntY[jSPup] - FIntY[jSPdn]));
			//* wmax : = sqrt((FIntX[intnum - 1] - FIntX[0])*(FIntX[intnum - 1] - FIntX[0]) + (FIntY[intnum - 1] - FIntY[0])*(FIntY[intnum - 1] - FIntY[0]))
			//wmin = sqrt((FIntX[mw] - FIntX[mw + 1])*(FIntX[mw] - FIntX[mw + 1]) + (FIntY[mw] - FIntY[mw + 1])*(FIntY[mw] - FIntY[mw + 1]));
			wmin = wmax;
			if (wmax > 3.0)
			{ // calculating shift
				xmid = (FIntX[jSPup] + FIntX[jSPdn]) / 2.;
				ymid = (FIntY[jSPup] + FIntY[jSPdn]) / 2.;
				shift = sqrt((ixc - xmid)*(ixc - xmid) + (iyc - ymid)*(iyc - ymid));
			}

		} // **** normal case end ***




		*(dW + cIdx) = wmax;
		*(dWmin + cIdx) = wmin;
		*(dShift + cIdx) = shift;

		*(dPint1x + cIdx) = PIntX[jSPup];
		*(dPint2x + cIdx) = PIntX[jSPdn];

		*(dPint1y + cIdx) = PIntY[jSPup];
		*(dPint2y + cIdx) = PIntY[jSPdn];

		/// saving number of intersection points ( 2 points) (left name Puseint for simplicity of changes)
		*(dPuseint + cIdx) = intnum;
		//********************************************	
	} // index cycle end
}

// new version for Pattern 231024 -fixed multiple pnums in a row, fixed neighboring same region wires
void CPU_MSPtProc39(UINT8 *pIm, UINT8 *pImIRB, UINT8 *pImIRBS, UINT16 *pImCLC, int imw, int imh, int pst, int pn, 
	float thr, int dclcmax, int pat0space1, float *dP1x, float *dP1y, float *dP2x, float *dP2y, int *dPnum, float *dW, 
	float *dWmin, float *dPint1x, float *dPint1y, float *dPint2x, float *dPint2y, float *dShift, int *dPuseint)
{
	int x1, y1, x2, y2, xc, yc, pnum, useint;
	int v, vp, vn, v1, v2, i, j;
	float wmin, wmax;		 // for width calculation
	float shift = 0.0, xmid, ymid; // for shift calculation
	int PPx[128];  // Bresenham's MS points array
	int PPy[128];
	int Pval[128]; // pnum values array for all MS points


	int g, gp; // current and previous gray values
	int jSPdn;
	int jSPup;
	int jMBdn;
	int jMBup;
	int cIdx; // MS index
	int dif0;
	int dif1;

	float M1[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int iM1[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int iM0[8] = { 0,0,0,0,0,0,0,0 };
	float PIntX[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. }; // intersection points coordinates
	float PIntY[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	float FIntX[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. }; // intersection points coordinates (subpixel)
	float FIntY[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	float fIntX, fIntY; // subpix coord. measured by 
	int Psubpix[8] = { 0,0,0,0,0,0,0,0 };
	int PInti[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };	  // index array of intersection points 
	int PCLC[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };   // CLC array of intersection points 
	int PType[8] = { 0,0,0,0,0,0,0,0 };
	int Pnums[8] = { 0,0,0,0,0,0,0,0 };
	int vpnums;
	//** Intersection point IRBS val,
	int	PIRBS[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int irbs;
	float ixc;
	float iyc;
	int ptype = 0, begtype = 0, pnfound = 0, btype;
	int PB[8];
	int PE[8];
	int ib = 0;
	int ie = 0;
	//int cIdx = threadIdx.x + blockIdx.x*blockDim.x;
	//int tid = blockDim.x * blockIdx.x + threadIdx.x;
	for (cIdx = pst; cIdx < pst + pn; cIdx++)
	{
		ib = 0;
		ie = 0;
		if (cIdx == 615549)
			cIdx = cIdx;
		memcpy(&PIntX[0], &M1[0], 8 * sizeof(float));
		memcpy(&PIntY[0], &M1[0], 8 * sizeof(float));
		memcpy(&FIntX[0], &M1[0], 8 * sizeof(float));
		memcpy(&FIntY[0], &M1[0], 8 * sizeof(float));
		memcpy(&PInti[0], &iM1[0], 8 * sizeof(int));
		memcpy(&PCLC[0], &iM1[0], 8 * sizeof(int));
		memcpy(&Psubpix[0], &iM0[0], 8 * sizeof(int));
		memcpy(&PType[0], &iM0[0], 8 * sizeof(int));
		//fw = *(d_P1xl + cIdx);
		//*(d_Wl + cIdx) = fw;
		//if (cIdx == 1883423)
		//if (cIdx == 157421)
		/*if (cIdx == 162006)
			cIdx = cIdx;*/
			//int ii = 0;

		int ii = 0, clc, pclc = -2, fi1 = 0, ip;
		int skipnext = 0;
		pnum = (*(dPnum + cIdx));
		//useint = (*(dPuseint + cIdx));
		x1 = round(*(dP1x + cIdx)); // getting measurement sensor endpoints
		y1 = round(*(dP1y + cIdx));
		x2 = round(*(dP2x + cIdx));
		y2 = round(*(dP2y + cIdx));
		ixc = (x1 + x2) / 2.;
		iyc = (y1 + y2) / 2.;
		//pnum = (*(dPnum + cIdx)) % 250 + 3;
		int deltaX = abs(x2 - x1);
		int deltaY = abs(y2 - y1);
		int signX = x1 < x2 ? 1 : -1;
		int signY = y1 < y2 ? 1 : -1;
		int error = deltaX - deltaY;
		int error2;
		int intind = 0; // intersection index (0 or 1)
		int pind = 0;   // B's point index [0...63]
		int xintc = x1, yintc = y1; // current and previous intersection points
		//v1 = *(pIm + y1 * imw + x1);  // 1st IRB pixel


		vp = 0;  // previous pixel
		/// filling in Bresenham's array
		while (x1 != x2 || y1 != y2)
		{
			PPx[pind] = x1;
			PPy[pind] = y1;
			v = *(pImIRB + y1 * imw + x1);
			Pval[pind] = v;
			error2 = error * 2;
			if (error2 > -deltaY)
			{
				error -= deltaY;
				x1 += signX;
			}
			if (error2 < deltaX)
			{
				error += deltaX;
				y1 += signY;
			}
			if (pind < 127)
				pind++;
			else
				break;
		} // while
		// Going forward from PS1

		ptype = begtype = pnfound = 0;
		vp = Pval[0];
		for (i = 1; i <= pind - 2; i++)
		{
			if (skipnext == 1)
			{
				skipnext = 0;
				continue;
			}
			xc = PPx[i]; yc = PPy[i];
			v = Pval[i];
			vn = Pval[i + 1];
			//irbs = *(pImIRBS + yc * imw + xc);  // IRBSC pixel value
			if (i == 1 && v == 1)
			{
				ptype = 1;
				PIntY[ii] = yc;
				PIntX[ii] = xc;
				PInti[ii] = i;
				PCLC[ii] = -2;
				begtype = 1;
				ii = ii + 1;
			}
			if (i == pind - 2 && v == 1)
			{ //**** inside of Spur ***
				ptype = 1;
				PIntY[ii] = yc;
				PIntX[ii] = xc;
				PInti[ii] = i;
				PCLC[ii] = -2;
				/*begtype = 1;
				ii = ii + 1;*/
			}
			if (v == 0)
			{ // *** reset ***
				begtype = 0;
				pnfound = 0;
			}
			if (v > 1 && v != pnum && begtype == 1)
			{ // *** met wrong region, so reset ***
				begtype = 0;
				pnfound = 0;
				ii = ii - 1;
			}
			//btype = 0;
			if (v == pnum)
			{
				//*** 2 points ******
				//* changed index ip
				//ip = ii;
				if (vp == pnum && vn == pnum)
					continue;
				else if (vp == 0 && (vn == pnum || vn == 1 || vn == 0))
					btype = 1;
				else if ((vp == 1 && vp != pnum) || (vn == 0 && vp != 0))
					btype = -1;
				else if ((vn == 1 && vp == pnum) || (vp == 1 && vn == pnum))
					btype = 1;
				else
				{
					btype = 0;
					//continue;
				}
				PType[ii] = btype;
				pnfound = 1;
				if (btype == 0)
				{
					continue;
				}
				PIntY[ii] = yc;
				PIntX[ii] = xc;
				PInti[ii] = i;
				//Psubpix[ii] = 1;
				if (ii < 7)
					ii = ii + 1;
			}
			if (i == pind - 2 && (v == 1 || v == pnum) && pnfound == 1)
			{
				PIntY[ii] = yc;
				PIntX[ii] = xc;
				PInti[ii] = i;
				PCLC[ii] = -2;
				PType[ii] = -1;
				if (ii < 7)
					ii = ii + 1;
			}
			vp = v;
		}
		//********************************************************************
		int ga[3]; // array of 3 neighboring g
		int nmwc = 0;//*number of MW (main wire) candidates
		int mw = -1;//* MW segment
		int dclc, dmin = 55555; // min CLC distance
		int d, d1, d2; // CLC distances
		int intnum = ii; /// number of intersection points found
		jSPdn = 0;

		//if (ii < 7)
		//{
		intnum = ii;
		jSPdn = 0;
		jSPup = ii - 1;

		if (intnum == 0)
		{
			wmax = 0.0;
			wmin = 0.0;
			shift = 0.0;
			//return ;
		}
		else if(intnum == 1)
		{
			wmax = 1.0;
			wmin = 1.0;
			shift = 0.0;
			//return ;
	     }
		else  ///**** NORMAL CASE ******
		{
			

			// * *** finding parts beg. and ends
			ib = 0;
			ie = 0;
			for (j = 0; j <= ii - 1; j++)
			{
				if (PType[j] == 1)
				{
					PB[ib] = j;
					ib = ib + 1;
				}
				else if (PType[j] == -1)
				{
					PE[ie] = j;
					ie = ie + 1;
				}
			}
			// ***Hong
			if (jSPdn == 0 || jSPdn == 1 || jSPdn == 2)
				jSPdn = jSPdn;
			else
				jSPdn = intnum - 2;
			// *****			



			if (intnum < 3)
			{
				jMBdn = jSPdn;
				jMBup = jSPup;
			}
			else if (intnum == 4)
			{
				dif0 = abs(PCLC[PB[0]] - PCLC[PE[0]]);
				dif1 = abs(PCLC[PB[1]] - PCLC[PE[1]]);
				if (dif0 > dif1)
				{
					jMBdn = PB[0];
					jMBup = PE[0];

				}
				else
				{
					jMBdn = PB[1];
					jMBup = PE[1];
				}
			}
			else
			{
				jMBdn = jSPdn;
				jMBup = jSPup;
			}
		
		
		 ///**** NORMAL CASE ******
	//**************** SUBPIX COORD. ******************************
			for (j = 0; j < ii; j++)
			{
				if (PType[j] == 1)
					PB[ib++] = j;
				//ib : = ib + 1
				else if (PType[j] == -1)
					PE[ie++] = j;
				//ie : = ie + 1

			}
			jSPdn = PB[0];
			jSPup = PE[ie - 1];
			if (jSPup == 0 || jSPup == 1 || jSPup == 2)
			{
				jSPup = jSPup;
			}
			else
			{
				jSPup = intnum - 1;
			}
			for (j = 0; j < intnum; j++)
			{
				if (PInti[j] >= 0)
				{
					ga[1] = *(pIm + PPy[PInti[j]] * imw + PPx[PInti[j]]);
					if (PInti[j] > 0)
						ga[0] = *(pIm + PPy[PInti[j] - 1] * imw + PPx[PInti[j] - 1]);
					else
						ga[0] = ga[1];
					if (PInti[j] < pind - 1)
						ga[2] = *(pIm + PPy[PInti[j] + 1] * imw + PPx[PInti[j] + 1]);
					else
						ga[2] = ga[1];
					if (PInti[j] < pind - 1 && PInti[j]>0)
					{
						SubPixCrdP(ga, PInti[j], PPx, PPy, &fIntX, &fIntY, thr);
					}
					else
					{
						fIntX = PPx[PInti[j]];
						fIntY = PPy[PInti[j]];
					}
					FIntX[j] = fIntX;
					FIntY[j] = fIntY;

				}
			}

			//*************************************************
			wmax = sqrt((FIntX[jSPup] - FIntX[jSPdn])*(FIntX[jSPup] - FIntX[jSPdn]) + (FIntY[jSPup] - FIntY[jSPdn])*(FIntY[jSPup] - FIntY[jSPdn]));
			//* wmax : = sqrt((FIntX[intnum - 1] - FIntX[0])*(FIntX[intnum - 1] - FIntX[0]) + (FIntY[intnum - 1] - FIntY[0])*(FIntY[intnum - 1] - FIntY[0]))
			//wmin = sqrt((FIntX[mw] - FIntX[mw + 1])*(FIntX[mw] - FIntX[mw + 1]) + (FIntY[mw] - FIntY[mw + 1])*(FIntY[mw] - FIntY[mw + 1]));
			wmin = wmax;
			if (wmax > 3.0)
			{ // calculating shift
				xmid = (FIntX[jSPup] + FIntX[jSPdn]) / 2.;
				ymid = (FIntY[jSPup] + FIntY[jSPdn]) / 2.;
				shift = sqrt((ixc - xmid)*(ixc - xmid) + (iyc - ymid)*(iyc - ymid));
			}

		} // **** normal case end ***


		

		*(dW + cIdx) = wmax;
		*(dWmin + cIdx) = wmin;
		*(dShift + cIdx) = shift;

		*(dPint1x + cIdx) = PIntX[jSPup];
		*(dPint2x + cIdx) = PIntX[jSPdn];

		*(dPint1y + cIdx) = PIntY[jSPup];
		*(dPint2y + cIdx) = PIntY[jSPdn];

		/// saving number of intersection points ( 2 points) (left name Puseint for simplicity of changes)
		*(dPuseint + cIdx) = intnum;
		//********************************************	
	} // index cycle end
}


// new version for Pattern 221110 -fixed neighboring same region wires
void CPU_MSPtProc36(UINT8 *pIm, UINT8 *pImIRB, UINT8 *pImIRBS, UINT16 *pImCLC, int imw, int imh, int pst, int pn, float thr, int dclcmax, int pat0space1, float *dP1x, float *dP1y, float *dP2x, float *dP2y, int *dPnum, float *dW, float *dWmin, float *dPint1x, float *dPint1y, float *dPint2x, float *dPint2y, float *dShift, int *dPuseint)
{
	int x1, y1, x2, y2, xc, yc, pnum, useint;
	int v, vp, vn, v1, v2, i, j;
	float wmin, wmax;		 // for width calculation
	float shift = 0.0, xmid, ymid; // for shift calculation
	int PPx[128];  // Bresenham's MS points array
	int PPy[128];
	int Pval[128]; // pnum values array for all MS points


	int g, gp; // current and previous gray values
	int jSPdn;
	int jSPup;
	int cIdx; // MS index

	float M1[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int iM1[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int iM0[8] = { 0,0,0,0,0,0,0,0 };
	float PIntX[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. }; // intersection points coordinates
	float PIntY[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	float FIntX[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. }; // intersection points coordinates (subpixel)
	float FIntY[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	float fIntX, fIntY; // subpix coord. measured by 
	int Psubpix[8] = { 0,0,0,0,0,0,0,0 };
	int PInti[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };	  // index array of intersection points 
	int PCLC[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };   // CLC array of intersection points 
	int PType[8] = { 0,0,0,0,0,0,0,0 };
	int Pnums[8] = { 0,0,0,0,0,0,0,0 };
	int vpnums;
	//** Intersection point IRBS val,
	int	PIRBS[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int irbs;
	float ixc;
	float iyc;
	int ptype = 0, begtype = 0, pnfound = 0, btype;
	int PB[8];
	int PE[8];
	int ib = 0;
	int ie = 0;
	//int cIdx = threadIdx.x + blockIdx.x*blockDim.x;
	//int tid = blockDim.x * blockIdx.x + threadIdx.x;
	for (cIdx = pst; cIdx < pst + pn; cIdx++)
	{
		ib = 0;
		ie = 0;
		if (cIdx == 615549)
			cIdx = cIdx;
		memcpy(&PIntX[0], &M1[0], 8 * sizeof(float));
		memcpy(&PIntY[0], &M1[0], 8 * sizeof(float));
		memcpy(&FIntX[0], &M1[0], 8 * sizeof(float));
		memcpy(&FIntY[0], &M1[0], 8 * sizeof(float));
		memcpy(&PInti[0], &iM1[0], 8 * sizeof(int));
		memcpy(&PCLC[0], &iM1[0], 8 * sizeof(int));
		memcpy(&Psubpix[0], &iM0[0], 8 * sizeof(int));
		memcpy(&PType[0], &iM0[0], 8 * sizeof(int));
		//fw = *(d_P1xl + cIdx);
		//*(d_Wl + cIdx) = fw;
		//if (cIdx == 1883423)
		//if (cIdx == 157421)
		/*if (cIdx == 162006)
			cIdx = cIdx;*/
			//int ii = 0;

		int ii = 0, clc, pclc = -2, fi1 = 0;
		pnum = (*(dPnum + cIdx));
		//useint = (*(dPuseint + cIdx));
		x1 = round(*(dP1x + cIdx)); // getting measurement sensor endpoints
		y1 = round(*(dP1y + cIdx));
		x2 = round(*(dP2x + cIdx));
		y2 = round(*(dP2y + cIdx));
		ixc = (x1 + x2) / 2.;
		iyc = (y1 + y2) / 2.;
		//pnum = (*(dPnum + cIdx)) % 250 + 3;
		int deltaX = abs(x2 - x1);
		int deltaY = abs(y2 - y1);
		int signX = x1 < x2 ? 1 : -1;
		int signY = y1 < y2 ? 1 : -1;
		int error = deltaX - deltaY;
		int error2;
		int intind = 0; // intersection index (0 or 1)
		int pind = 0;   // B's point index [0...63]
		int xintc = x1, yintc = y1; // current and previous intersection points
		//v1 = *(pIm + y1 * imw + x1);  // 1st IRB pixel


		vp = 0;  // previous pixel
		/// filling in Bresenham's array
		while (x1 != x2 || y1 != y2)
		{
			PPx[pind] = x1;
			PPy[pind] = y1;
			v = *(pImIRB + y1 * imw + x1);
			Pval[pind] = v;
			error2 = error * 2;
			if (error2 > -deltaY)
			{
				error -= deltaY;
				x1 += signX;
			}
			if (error2 < deltaX)
			{
				error += deltaX;
				y1 += signY;
			}
			if (pind < 127)
				pind++;
			else
				break;
		} // while
		// Going forward from PS1

		ptype = begtype = pnfound = 0;
		vp = Pval[0];
		for (i = 1; i <= pind - 2; i++)
		{
			xc = PPx[i]; yc = PPy[i];
			v = Pval[i];
			vn = Pval[i+1];
			//irbs = *(pImIRBS + yc * imw + xc);  // IRBSC pixel value
			if (i == 1 && v == 1)
			{
				ptype = 1;
				PIntY[ii] = yc;
				PIntX[ii] = xc;
				PInti[ii] = i;
				PCLC[ii] = -2;
				begtype = 1;
				ii = ii + 1;
			}
			if (v == 0)
			{
				begtype = 0;
				pnfound = 0;
			}
			if (v > 1 && v != pnum && begtype == 1)
			{
				begtype = 0;
				pnfound = 0;
				ii = ii - 1;
			}
			btype = 0;
			if (v == pnum)
			{
				if (vp == 0 && (vn == pnum || vn == 1 ))
					btype = 1;
				else if ((vp == 1 && vp != pnum) || (vn == 0 && vp != 0))
				
					btype = -1;
				else
				{
					btype = 0;
					continue;
				}
				PType[ii] = btype;
				pnfound = 1;
				PIntY[ii] = yc;
				PIntX[ii] = xc;
				PInti[ii] = i;
				Psubpix[ii] = 1;
				if (ii < 7)
					ii = ii + 1;
			}
			if (i == pind - 1 && v == 1 && pnfound == 1)
			{
				PIntY[ii] = yc;
				PIntX[ii] = xc;
				PInti[ii] = i;
				PCLC[ii] = -2;
				ii = ii + 1;
			}
			vp = v;
		}
		//********************************************************************
		int ga[3]; // array of 3 neighboring g
		int nmwc = 0;//*number of MW (main wire) candidates
		int mw = -1;//* MW segment
		int dclc, dmin = 55555; // min CLC distance
		int d, d1, d2; // CLC distances
		int intnum = ii; /// number of intersection points found
		jSPdn = 0;

		//if (ii < 7)
		//{
		intnum = ii;
		jSPdn = 0;
		jSPup = ii - 1;
		//}
		/*else
		{
			intnum = 7;
			jSPup = 7;
		}*/

		if (intnum <= 1) //** single pix. space case
		{
			wmin = -1.;
			wmax = -1.;
			jSPdn = 0;
			jSPup = 0;
		}
		else
		{ ///**** NORMAL CASE ******
	//**************** SUBPIX COORD. ******************************
			for (j = 0; j < ii ; j++)
			{
				if (PType[j] == 1)
					PB[ib++] = j;
				//ib : = ib + 1
				else if (PType[j] == -1)
					PE[ie++] = j;
				//ie : = ie + 1
				
			}
			jSPdn = PB[0];
			jSPup = PE[ie - 1];
			if(jSPup == 0 || jSPup ==1 || jSPup == 2)
			{
				jSPup = jSPup;
			}
			else
			{
				jSPup = intnum - 1;
			}
			for (j = 0; j < intnum; j++)
			{
				if (PInti[j] >= 0)
				{
					ga[1] = *(pIm + PPy[PInti[j]] * imw + PPx[PInti[j]]);
					if (PInti[j] > 0)
						ga[0] = *(pIm + PPy[PInti[j] - 1] * imw + PPx[PInti[j] - 1]);
					else
						ga[0] = ga[1];
					if (PInti[j] < pind - 1)
						ga[2] = *(pIm + PPy[PInti[j] + 1] * imw + PPx[PInti[j] + 1]);
					else
						ga[2] = ga[1];
					if (PInti[j] < pind - 1 && PInti[j]>0)
					{
						SubPixCrdP(ga, PInti[j], PPx, PPy, &fIntX, &fIntY, thr);
					}
					else
					{
						fIntX = PPx[PInti[j]];
						fIntY = PPy[PInti[j]];
					}
					FIntX[j] = fIntX;
					FIntY[j] = fIntY;

				}
			}

			//*************************************************
			wmax = sqrt((FIntX[jSPup] - FIntX[jSPdn])*(FIntX[jSPup] - FIntX[jSPdn]) + (FIntY[jSPup] - FIntY[jSPdn])*(FIntY[jSPup] - FIntY[jSPdn]));
			//* wmax : = sqrt((FIntX[intnum - 1] - FIntX[0])*(FIntX[intnum - 1] - FIntX[0]) + (FIntY[intnum - 1] - FIntY[0])*(FIntY[intnum - 1] - FIntY[0]))
			//wmin = sqrt((FIntX[mw] - FIntX[mw + 1])*(FIntX[mw] - FIntX[mw + 1]) + (FIntY[mw] - FIntY[mw + 1])*(FIntY[mw] - FIntY[mw + 1]));
			wmin = wmax;
			if (wmax > 3.0)
			{ // calculating shift
				xmid = (FIntX[jSPup] + FIntX[jSPdn]) / 2.;
				ymid = (FIntY[jSPup] + FIntY[jSPdn]) / 2.;
				shift = sqrt((ixc - xmid)*(ixc - xmid) + (iyc - ymid)*(iyc - ymid));
			}

			//} // **** normal case end ***


		}

		*(dW + cIdx) = wmax;
		*(dWmin + cIdx) = wmin;
		*(dShift + cIdx) = shift;

		*(dPint1x + cIdx) = PIntX[jSPup];
		*(dPint2x + cIdx) = PIntX[jSPdn];

		*(dPint1y + cIdx) = PIntY[jSPup];
		*(dPint2y + cIdx) = PIntY[jSPdn];

		/// saving number of intersection points ( 2 points) (left name Puseint for simplicity of changes)
		*(dPuseint + cIdx) = intnum;
		//********************************************	
	} // index cycle end
}

// new version for Pattern 221107
void CPU_MSPtProc35(UINT8 *pIm, UINT8 *pImIRB, UINT8 *pImIRBS, UINT16 *pImCLC, int imw, int imh, int pst, int pn, float thr, int dclcmax, int pat0space1, float *dP1x, float *dP1y, float *dP2x, float *dP2y, int *dPnum, float *dW, float *dWmin, float *dPint1x, float *dPint1y, float *dPint2x, float *dPint2y, float *dShift, int *dPuseint)
{
	int x1, y1, x2, y2, xc, yc, pnum, useint;
	int v, vp, vn, v1, v2, i, j;
	float wmin, wmax;		 // for width calculation
	float shift = 0.0, xmid, ymid; // for shift calculation
	int PPx[128];  // Bresenham's MS points array
	int PPy[128];
	int Pval[128]; // pnum values array for all MS points


	int g, gp; // current and previous gray values
	int jSPdn;
	int jSPup;
	int cIdx; // MS index

	float M1[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int iM1[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int iM0[8] = { 0,0,0,0,0,0,0,0 };
	float PIntX[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. }; // intersection points coordinates
	float PIntY[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	float FIntX[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. }; // intersection points coordinates (subpixel)
	float FIntY[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	float fIntX, fIntY; // subpix coord. measured by 
	int Psubpix[8] = { 0,0,0,0,0,0,0,0 };
	int PInti[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };	  // index array of intersection points 
	int PCLC[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };   // CLC array of intersection points 
	int Pdir[8] = { 0,0,0,0,0,0,0,0 };
	int Pnums[8] = { 0,0,0,0,0,0,0,0 };
	int vpnums;
	//** Intersection point IRBS val,
	int	PIRBS[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int irbs;
	float ixc;
	float iyc;
	int ptype = 0, begtype = 0, pnfound = 0;
	//int cIdx = threadIdx.x + blockIdx.x*blockDim.x;
	//int tid = blockDim.x * blockIdx.x + threadIdx.x;
	for (cIdx = pst; cIdx < pst + pn; cIdx++)
	{
		/*if (cIdx == 451718)
			cIdx = cIdx;*/
		memcpy(&PIntX[0], &M1[0], 8 * sizeof(float));
		memcpy(&PIntY[0], &M1[0], 8 * sizeof(float));
		memcpy(&FIntX[0], &M1[0], 8 * sizeof(float));
		memcpy(&FIntY[0], &M1[0], 8 * sizeof(float));
		memcpy(&PInti[0], &iM1[0], 8 * sizeof(int));
		memcpy(&PCLC[0], &iM1[0], 8 * sizeof(int));
		memcpy(&Psubpix[0], &iM0[0], 8 * sizeof(int));
		//fw = *(d_P1xl + cIdx);
		//*(d_Wl + cIdx) = fw;
		//if (cIdx == 1883423)
		//if (cIdx == 157421)
		/*if (cIdx == 162006)
			cIdx = cIdx;*/
			//int ii = 0;

		int ii = 0, clc, pclc = -2, fi1 = 0;
		pnum = (*(dPnum + cIdx));
		//useint = (*(dPuseint + cIdx));
		x1 = round(*(dP1x + cIdx)); // getting measurement sensor endpoints
		y1 = round(*(dP1y + cIdx));
		x2 = round(*(dP2x + cIdx));
		y2 = round(*(dP2y + cIdx));
		ixc = (x1 + x2) / 2.;
		iyc = (y1 + y2) / 2.;
		//pnum = (*(dPnum + cIdx)) % 250 + 3;
		int deltaX = abs(x2 - x1);
		int deltaY = abs(y2 - y1);
		int signX = x1 < x2 ? 1 : -1;
		int signY = y1 < y2 ? 1 : -1;
		int error = deltaX - deltaY;
		int error2;
		int intind = 0; // intersection index (0 or 1)
		int pind = 0;   // B's point index [0...63]
		int xintc = x1, yintc = y1; // current and previous intersection points
		//v1 = *(pIm + y1 * imw + x1);  // 1st IRB pixel
		

		vp = 0;  // previous pixel
		/// filling in Bresenham's array
		while (x1 != x2 || y1 != y2)
		{
			PPx[pind] = x1;
			PPy[pind] = y1;
			v = *(pImIRB + y1 * imw + x1);
			Pval[pind] = v;
			error2 = error * 2;
			if (error2 > -deltaY)
			{
				error -= deltaY;
				x1 += signX;
			}
			if (error2 < deltaX)
			{
				error += deltaX;
				y1 += signY;
			}
			if (pind < 127)
				pind++;
			else
				break;
		} // while
		// Going forward from PS1
		
		ptype = begtype = pnfound = 0;
		for (i = 0; i <= pind - 1; i++)
		{
			xc = PPx[i]; yc = PPy[i];
			v = Pval[i];
			irbs = *(pImIRBS + yc * imw + xc);  // IRBSC pixel value
			if (i == 0 && v == 1)
			{
				ptype = 1;
				PIntY[ii] = yc;
				PIntX[ii] = xc;
				PInti[ii] = i;
				PCLC[ii] = -2;
				begtype = 1;
				ii = ii + 1;
			}
			if (v == 0)
			{ 
				begtype = 0;
				pnfound = 0;
			}
			if (v > 1 && v != pnum && begtype == 1)
			{
				begtype = 0;
				pnfound = 0;
				ii = ii - 1;
			}
			
			if (v == pnum)
			{
				pnfound = 1;
				PIntY[ii] = yc;
				PIntX[ii] = xc;
				PInti[ii] = i;
				Psubpix[ii] = 1;
				if (ii < 7)
					ii = ii + 1;
			}
			if (i == pind - 1 && v == 1 && pnfound == 1)
			{
				PIntY[ii] = yc;
				PIntX[ii] = xc;
				PInti[ii] = i;
				PCLC[ii] = -2;
				ii = ii + 1;
			}

		}
		//********************************************************************
		int ga[3]; // array of 3 neighboring g
		int nmwc = 0;//*number of MW (main wire) candidates
		int mw = -1;//* MW segment
		int dclc, dmin = 55555; // min CLC distance
		int d, d1, d2; // CLC distances
		int intnum = ii; /// number of intersection points found
		jSPdn = 0;

		//if (ii < 7)
		//{
		intnum = ii;
		jSPdn = 0;
		jSPup = ii - 1;
		//}
		/*else
		{
			intnum = 7;
			jSPup = 7;
		}*/

		if (intnum <= 1) //** single pix. space case
		{
			wmin = -1.;
			wmax = -1.;
			jSPdn = 0;
			jSPup = 0;
		}
		else
		{ ///**** NORMAL CASE ******
	//**************** SUBPIX COORD. ******************************
			for (j = 0; j < intnum; j++)
			{
				ga[1] = *(pIm + PPy[PInti[j]] * imw + PPx[PInti[j]]);
				if (PInti[j] > 0)
					ga[0] = *(pIm + PPy[PInti[j] - 1] * imw + PPx[PInti[j] - 1]);
				else
					ga[0] = ga[1];
				if (PInti[j] < pind - 1)
					ga[2] = *(pIm + PPy[PInti[j] + 1] * imw + PPx[PInti[j] + 1]);
				else
					ga[2] = ga[1];
				if (PInti[j] < pind - 1 && PInti[j]>0)
				{
					SubPixCrdP(ga, PInti[j], PPx, PPy, &fIntX, &fIntY, thr);
				}
				else
				{
					fIntX = PPx[PInti[j]];
					fIntY = PPy[PInti[j]];
				}
				FIntX[j] = fIntX;
				FIntY[j] = fIntY;
			}

			//*************************************************
			wmax = sqrt((FIntX[jSPup] - FIntX[jSPdn])*(FIntX[jSPup] - FIntX[jSPdn]) + (FIntY[jSPup] - FIntY[jSPdn])*(FIntY[jSPup] - FIntY[jSPdn]));
			//* wmax : = sqrt((FIntX[intnum - 1] - FIntX[0])*(FIntX[intnum - 1] - FIntX[0]) + (FIntY[intnum - 1] - FIntY[0])*(FIntY[intnum - 1] - FIntY[0]))
			//wmin = sqrt((FIntX[mw] - FIntX[mw + 1])*(FIntX[mw] - FIntX[mw + 1]) + (FIntY[mw] - FIntY[mw + 1])*(FIntY[mw] - FIntY[mw + 1]));
			wmin = wmax;
			if (wmax > 3.0)
			{ // calculating shift
				xmid = (FIntX[jSPup] + FIntX[jSPdn]) / 2.;
				ymid = (FIntY[jSPup] + FIntY[jSPdn]) / 2.;
				shift = sqrt((ixc - xmid)*(ixc - xmid) + (iyc - ymid)*(iyc - ymid));
			}

			//} // **** normal case end ***


		}
		*(dW + cIdx) = wmax;
		*(dWmin + cIdx) = wmin;
		*(dShift + cIdx) = shift;

		*(dPint1x + cIdx) = PIntX[jSPup];
		*(dPint2x + cIdx) = PIntX[jSPdn];

		*(dPint1y + cIdx) = PIntY[jSPup];
		*(dPint2y + cIdx) = PIntY[jSPdn];

		/// saving number of intersection points ( 2 points) (left name Puseint for simplicity of changes)
		*(dPuseint + cIdx) = intnum;
		//********************************************	
	} // index cycle end
}

// new version for Pattern
void CPU_MSPtProc34(UINT8 *pIm, UINT8 *pImIRB, UINT8 *pImIRBS, UINT16 *pImCLC, int imw, int imh, int pst, int pn, float thr, int dclcmax, int pat0space1, float *dP1x, float *dP1y, float *dP2x, float *dP2y, int *dPnum, float *dW, float *dWmin, float *dPint1x, float *dPint1y, float *dPint2x, float *dPint2y, float *dShift, int *dPuseint)
{
	int x1, y1, x2, y2, xc, yc, pnum, useint;
	int v, vp, vn, v1, v2, i, j;
	float wmin, wmax;		 // for width calculation
	float shift = 0.0, xmid, ymid; // for shift calculation
	int PPx[128];  // Bresenham's MS points array
	int PPy[128];
	int Pval[128]; // pnum values array for all MS points


	int g, gp; // current and previous gray values
	int jSPdn;
	int jSPup;
	int cIdx; // MS index

	float M1[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int iM1[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int iM0[8] = { 0,0,0,0,0,0,0,0 };
	float PIntX[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. }; // intersection points coordinates
	float PIntY[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	float FIntX[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. }; // intersection points coordinates (subpixel)
	float FIntY[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	float fIntX, fIntY; // subpix coord. measured by 
	int Psubpix[8] = { 0,0,0,0,0,0,0,0 };
	int PInti[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };	  // index array of intersection points 
	int PCLC[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };   // CLC array of intersection points 
	int Pdir[8] = { 0,0,0,0,0,0,0,0 };
	int Pnums[8] = { 0,0,0,0,0,0,0,0 };
	int vpnums;
	//** Intersection point IRBS val,
	int	PIRBS[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int irbs;
	float ixc;
	float iyc;
	//int cIdx = threadIdx.x + blockIdx.x*blockDim.x;
	//int tid = blockDim.x * blockIdx.x + threadIdx.x;
	for (cIdx = pst; cIdx < pst + pn; cIdx++)
	{
		if (cIdx == 451718)
			cIdx = cIdx;
		memcpy(&PIntX[0], &M1[0], 8 * sizeof(float));
		memcpy(&PIntY[0], &M1[0], 8 * sizeof(float));
		memcpy(&FIntX[0], &M1[0], 8 * sizeof(float));
		memcpy(&FIntY[0], &M1[0], 8 * sizeof(float));
		memcpy(&PInti[0], &iM1[0], 8 * sizeof(int));
		memcpy(&PCLC[0], &iM1[0], 8 * sizeof(int));
		memcpy(&Psubpix[0], &iM0[0], 8 * sizeof(int));
		//fw = *(d_P1xl + cIdx);
		//*(d_Wl + cIdx) = fw;
		//if (cIdx == 1883423)
		//if (cIdx == 157421)
		/*if (cIdx == 162006)
			cIdx = cIdx;*/
		//int ii = 0;

		int ii = 0, clc, pclc = -2, fi1 = 0;
		pnum = (*(dPnum + cIdx));
		//useint = (*(dPuseint + cIdx));
		x1 = round(*(dP1x + cIdx)); // getting measurement sensor endpoints
		y1 = round(*(dP1y + cIdx));
		x2 = round(*(dP2x + cIdx));
		y2 = round(*(dP2y + cIdx));
		ixc = (x1 + x2) / 2.;
		iyc = (y1 + y2) / 2.;
		//pnum = (*(dPnum + cIdx)) % 250 + 3;
		int deltaX = abs(x2 - x1);
		int deltaY = abs(y2 - y1);
		int signX = x1 < x2 ? 1 : -1;
		int signY = y1 < y2 ? 1 : -1;
		int error = deltaX - deltaY;
		int error2;
		int intind = 0; // intersection index (0 or 1)
		int pind = 0;   // B's point index [0...63]
		int xintc = x1, yintc = y1; // current and previous intersection points
		//v1 = *(pIm + y1 * imw + x1);  // 1st IRB pixel

		vp = 0;  // previous pixel
		/// filling in Bresenham's array
		while (x1 != x2 || y1 != y2)
		{
			PPx[pind] = x1;
			PPy[pind] = y1;
			v = *(pImIRB + y1 * imw + x1);
			Pval[pind] = v;
			error2 = error * 2;
			if (error2 > -deltaY)
			{
				error -= deltaY;
				x1 += signX;
			}
			if (error2 < deltaX)
			{
				error += deltaX;
				y1 += signY;
			}
			if (pind < 127)
				pind++;
			else
				break;
		} // while
		// Going forward from PS1

		for (i = 0; i <= pind - 1; i++)
		{
			xc = PPx[i]; yc = PPy[i];
			v = Pval[i];
			irbs = *(pImIRBS + yc * imw + xc);  // IRBSC pixel value
			if (v == pnum)
			{
				PIntY[ii] = yc;
				PIntX[ii] = xc;
				PInti[ii] = i;
				Psubpix[ii] = 1;
				if (ii < 7)
					ii = ii + 1;
			}

		}
		//********************************************************************
		int ga[3]; // array of 3 neighboring g
		int nmwc = 0;//*number of MW (main wire) candidates
		int mw = -1;//* MW segment
		int dclc, dmin = 55555; // min CLC distance
		int d, d1, d2; // CLC distances
		int intnum = ii; /// number of intersection points found
		jSPdn = 0;
		
		//if (ii < 7)
		//{
			intnum = ii;
			jSPdn = 0;
			jSPup = ii - 1;
		//}
		/*else
		{
			intnum = 7;
			jSPup = 7;
		}*/

		if (intnum <= 1) //** single pix. space case
		{
			wmin = -1.;
			wmax = -1.;
			jSPdn = 0;
			jSPup = 0;
		}
		else
		{ ///**** NORMAL CASE ******
	//**************** SUBPIX COORD. ******************************
			for (j = 0; j < intnum; j++)
			{
				ga[1] = *(pIm + PPy[PInti[j]] * imw + PPx[PInti[j]]);
				if (PInti[j] > 0)
					ga[0] = *(pIm + PPy[PInti[j] - 1] * imw + PPx[PInti[j] - 1]);
				else
					ga[0] = ga[1];
				if (PInti[j] < pind - 1)
					ga[2] = *(pIm + PPy[PInti[j] + 1] * imw + PPx[PInti[j] + 1]);
				else
					ga[2] = ga[1];
				if (PInti[j] < pind - 1 && PInti[j]>0)
				{
					SubPixCrdP(ga, PInti[j], PPx, PPy, &fIntX, &fIntY, thr);
				}
				else
				{
					fIntX = PPx[PInti[j]];
					fIntY = PPy[PInti[j]];
				}
				FIntX[j] = fIntX;
				FIntY[j] = fIntY;
			}

			//*************************************************
			wmax = sqrt((FIntX[jSPup] - FIntX[jSPdn])*(FIntX[jSPup] - FIntX[jSPdn]) + (FIntY[jSPup] - FIntY[jSPdn])*(FIntY[jSPup] - FIntY[jSPdn]));
			//* wmax : = sqrt((FIntX[intnum - 1] - FIntX[0])*(FIntX[intnum - 1] - FIntX[0]) + (FIntY[intnum - 1] - FIntY[0])*(FIntY[intnum - 1] - FIntY[0]))
			//wmin = sqrt((FIntX[mw] - FIntX[mw + 1])*(FIntX[mw] - FIntX[mw + 1]) + (FIntY[mw] - FIntY[mw + 1])*(FIntY[mw] - FIntY[mw + 1]));
			wmin = wmax;
			if (wmax > 3.0)
			{ // calculating shift
				xmid = (FIntX[jSPup] + FIntX[jSPdn]) / 2.;
				ymid = (FIntY[jSPup] + FIntY[jSPdn]) / 2.;
				shift = sqrt((ixc - xmid)*(ixc - xmid) + (iyc - ymid)*(iyc - ymid));
			}

			//} // **** normal case end ***

			
		}
		*(dW + cIdx) = wmax;
		*(dWmin + cIdx) = wmin;
		*(dShift + cIdx) = shift;

		*(dPint1x + cIdx) = PIntX[jSPup];
		*(dPint2x + cIdx) = PIntX[jSPdn];

		*(dPint1y + cIdx) = PIntY[jSPup];
		*(dPint2y + cIdx) = PIntY[jSPdn];

		/// saving number of intersection points ( 2 points) (left name Puseint for simplicity of changes)
		*(dPuseint + cIdx) = intnum;
		//********************************************	
	} // index cycle end
}


// // new version for Space (from Pat 36)
void CPU_MSSpProc36(UINT8 *pIm, UINT8 *pImIRB, /*UINT8 *pImIRBS,*/ UINT8 *pEmpty, UINT16 *pImCLC, int imw, int imh, int pst, int pn, float thr, int dclcmax, int pat0space1, float *dP1x, float *dP1y, float *dP2x, float *dP2y, int *dPnum, float *dW, float *dWmin, float *dPint1x, float *dPint1y, float *dPint2x, float *dPint2y, float *dShift, int *dPuseint)
{
	int x1, y1, x2, y2, xc, yc, pnum, useint;
	int v, vp, vn, v1, v2, i, j;
	float wmin, wmax;		 // for width calculation
	float shift = 0.0, xmid, ymid; // for shift calculation
	int PPx[128];  // Bresenham's MS points array
	int PPy[128];
	int Pval[128]; // pnum values array for all MS points


	int g, gp; // current and previous gray values
	int jSPdn;
	int jSPup;
	int cIdx; // MS index

	float M1[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int iM1[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int iM0[8] = { 0,0,0,0,0,0,0,0 };
	float PIntX[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. }; // intersection points coordinates
	float PIntY[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	float FIntX[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. }; // intersection points coordinates (subpixel)
	float FIntY[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	float fIntX, fIntY; // subpix coord. measured by 
	int Psubpix[8] = { 0,0,0,0,0,0,0,0 };
	int PInti[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };	  // index array of intersection points 
	int PCLC[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };   // CLC array of intersection points 
	int PType[8] = { 0,0,0,0,0,0,0,0 };
	int Pnums[8] = { 0,0,0,0,0,0,0,0 };
	int vpnums;
	//** Intersection point IRBS val,
	int	PIRBS[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int irbs;
	float ixc;
	float iyc;
	int ptype = 0, begtype = 0, pnfound = 0, btype;
	int PB[8];
	int PE[8];
	int ib = 0;
	int ie = 0;
	//int cIdx = threadIdx.x + blockIdx.x*blockDim.x;
	//int tid = blockDim.x * blockIdx.x + threadIdx.x;
	for (cIdx = pst; cIdx < pst + pn; cIdx++)
	{
		ib = 0;
		ie = 0;
		/*if (cIdx == 23268)
			cIdx = cIdx;*/
		memcpy(&PIntX[0], &M1[0], 8 * sizeof(float));
		memcpy(&PIntY[0], &M1[0], 8 * sizeof(float));
		memcpy(&FIntX[0], &M1[0], 8 * sizeof(float));
		memcpy(&FIntY[0], &M1[0], 8 * sizeof(float));
		memcpy(&PInti[0], &iM1[0], 8 * sizeof(int));
		memcpy(&PCLC[0], &iM1[0], 8 * sizeof(int));
		memcpy(&Psubpix[0], &iM0[0], 8 * sizeof(int));
		memcpy(&PType[0], &iM0[0], 8 * sizeof(int));
		//fw = *(d_P1xl + cIdx);
		//*(d_Wl + cIdx) = fw;
		//if (cIdx == 1883423)
		//if (cIdx == 157421)
		/*if (cIdx == 162006)
			cIdx = cIdx;*/
			//int ii = 0;

		int ii = 0, clc, pclc = -2, fi1 = 0;
		pnum = (*(dPnum + cIdx));
		//useint = (*(dPuseint + cIdx));
		x1 = round(*(dP1x + cIdx)); // getting measurement sensor endpoints
		y1 = round(*(dP1y + cIdx));
		x2 = round(*(dP2x + cIdx));
		y2 = round(*(dP2y + cIdx));
		ixc = (x1 + x2) / 2.;
		iyc = (y1 + y2) / 2.;
		//pnum = (*(dPnum + cIdx)) % 250 + 3;
		int deltaX = abs(x2 - x1);
		int deltaY = abs(y2 - y1);
		int signX = x1 < x2 ? 1 : -1;
		int signY = y1 < y2 ? 1 : -1;
		int error = deltaX - deltaY;
		int error2;
		int intind = 0; // intersection index (0 or 1)
		int pind = 0;   // B's point index [0...63]
		int xintc = x1, yintc = y1; // current and previous intersection points
		//v1 = *(pIm + y1 * imw + x1);  // 1st IRB pixel


		vp = 0;  // previous pixel
		/// filling in Bresenham's array
		while (x1 != x2 || y1 != y2)
		{
			PPx[pind] = x1;
			PPy[pind] = y1;
			v = *(pImIRB + y1 * imw + x1);
			Pval[pind] = v;
			error2 = error * 2;
			if (error2 > -deltaY)
			{
				error -= deltaY;
				x1 += signX;
			}
			if (error2 < deltaX)
			{
				error += deltaX;
				y1 += signY;
			}
			if (pind < 127)
				pind++;
			else
				break;
		} // while
		// Going forward from PS1

		ptype = begtype = pnfound = 0;
		vp = Pval[0];
		for (i = 1; i <= pind - 2; i++)
		{
			xc = PPx[i]; yc = PPy[i];
			v = Pval[i];
			vn = Pval[i + 1];
			//irbs = *(pImIRBS + yc * imw + xc);  // IRBSC pixel value
			if (i == 1 && v == 1)
			{
				ptype = 1;
				PIntY[ii] = yc;
				PIntX[ii] = xc;
				PInti[ii] = i;
				PCLC[ii] = -2;
				begtype = 1;
				ii = ii + 1;
			}
			if (v == 0)
			{
				begtype = 0;
				pnfound = 0;
			}
			if (v > 1 && v != pnum && begtype == 1)
			{
				begtype = 0;
				pnfound = 0;
				ii = ii - 1;
			}
			btype = 0;
			if (v == pnum)
			{
				if (vp == 0 && (vn == pnum || vn == 1))
					btype = 1;
				else if ((vp == 1 && vp != pnum) || (vn == 0 && vp != 0))

					btype = -1;
				else
				{
					btype = 0;
					continue;
				}
				PType[ii] = btype;
				pnfound = 1;
				PIntY[ii] = yc;
				PIntX[ii] = xc;
				PInti[ii] = i;
				Psubpix[ii] = 1;
				if (ii < 7)
					ii = ii + 1;
			}
			if (i == pind - 1 && v == 1 && pnfound == 1)
			{
				PIntY[ii] = yc;
				PIntX[ii] = xc;
				PInti[ii] = i;
				PCLC[ii] = -2;
				ii = ii + 1;
			}
			vp = v;
		}
		//********************************************************************
		int ga[3]; // array of 3 neighboring g
		int nmwc = 0;//*number of MW (main wire) candidates
		int mw = -1;//* MW segment
		int dclc, dmin = 55555; // min CLC distance
		int d, d1, d2; // CLC distances
		int intnum = ii; /// number of intersection points found
		jSPdn = 0;

		//if (ii < 7)
		//{
		intnum = ii;
		jSPdn = 0;
		jSPup = ii - 1;
		//}
		/*else
		{
			intnum = 7;
			jSPup = 7;
		}*/

		if (intnum <= 1) //** single pix. space case
		{
			wmin = 10.;
			wmax = 10.;
			jSPdn = 0;
			jSPup = 0;
		}
		else
		{ ///**** NORMAL CASE ******
	//**************** SUBPIX COORD. ******************************
			for (j = 0; j < ii; j++)
			{
				if (PType[j] == 1)
					PB[ib++] = j;
				//ib : = ib + 1
				else if (PType[j] == -1)
					PE[ie++] = j;
				//ie : = ie + 1

			}
			if (ib == 0 || ie == 0)
			{
				wmin = 11.;
				wmax = 11.;
				jSPdn = 0;
				jSPup = 0;
			}
			else
			{
				jSPdn = PB[0];
				jSPup = PE[ie - 1];
				if (jSPdn == 0 || jSPdn == 1 || jSPdn == 2)
				{
					jSPdn = jSPdn;
				}
				else
				{
					jSPdn = intnum - 2;
				}
				for (j = 0; j < intnum; j++)
				{
					ga[1] = *(pIm + PPy[PInti[j]] * imw + PPx[PInti[j]]);
					if (PInti[j] > 0)
						ga[0] = *(pIm + PPy[PInti[j] - 1] * imw + PPx[PInti[j] - 1]);
					else
						ga[0] = ga[1];
					if (PInti[j] < pind - 1)
						ga[2] = *(pIm + PPy[PInti[j] + 1] * imw + PPx[PInti[j] + 1]);
					else
						ga[2] = ga[1];
					if (PInti[j] < pind - 1 && PInti[j]>0)
					{
						SubPixCrdS(ga, PInti[j], PPx, PPy, &fIntX, &fIntY, thr);
					}
					else
					{
						fIntX = PPx[PInti[j]];
						fIntY = PPy[PInti[j]];
					}
					FIntX[j] = fIntX;
					FIntY[j] = fIntY;
				}

				//*************************************************
				wmax = sqrt((FIntX[jSPup] - FIntX[jSPdn])*(FIntX[jSPup] - FIntX[jSPdn]) + (FIntY[jSPup] - FIntY[jSPdn])*(FIntY[jSPup] - FIntY[jSPdn]));
				//* wmax : = sqrt((FIntX[intnum - 1] - FIntX[0])*(FIntX[intnum - 1] - FIntX[0]) + (FIntY[intnum - 1] - FIntY[0])*(FIntY[intnum - 1] - FIntY[0]))
				//wmin = sqrt((FIntX[mw] - FIntX[mw + 1])*(FIntX[mw] - FIntX[mw + 1]) + (FIntY[mw] - FIntY[mw + 1])*(FIntY[mw] - FIntY[mw + 1]));
				wmin = wmax;

			}

			
			if (wmax > 3.0)
			{ // calculating shift
				xmid = (FIntX[jSPup] + FIntX[jSPdn]) / 2.;
				ymid = (FIntY[jSPup] + FIntY[jSPdn]) / 2.;
				shift = sqrt((ixc - xmid)*(ixc - xmid) + (iyc - ymid)*(iyc - ymid));
			}

			//} // **** normal case end ***


		}
		*(dW + cIdx) = wmax;
		*(dWmin + cIdx) = wmin;
		*(dShift + cIdx) = shift;

		*(dPint1x + cIdx) = PIntX[jSPup];
		*(dPint2x + cIdx) = PIntX[jSPdn];

		*(dPint1y + cIdx) = PIntY[jSPup];
		*(dPint2y + cIdx) = PIntY[jSPdn];

		/// saving number of intersection points ( 2 points) (left name Puseint for simplicity of changes)
		*(dPuseint + cIdx) = intnum;
		//********************************************	
	} // index cycle end
}


// new version for Space
void CPU_MSSpProc34(UINT8 *pIm, UINT8 *pImIRB, UINT8 *pImIRBS, UINT16 *pImCLC, int imw, int imh, int pst, int pn, float thr, int dclcmax, int pat0space1, float *dP1x, float *dP1y, float *dP2x, float *dP2y, int *dPnum, float *dW, float *dWmin, float *dPint1x, float *dPint1y, float *dPint2x, float *dPint2y, float *dShift, int *dPuseint)
{
	int x1, y1, x2, y2, xc, yc, pnum, useint;
	int v, vp, vn, v1, v2, i, j;
	float wmin, wmax;		 // for width calculation
	float shift = 0.0, xmid, ymid; // for shift calculation
	int PPx[128];  // Bresenham's MS points array
	int PPy[128];
	int Pval[128]; // pnum values array for all MS points


	int g, gp; // current and previous gray values
	int jSPdn;
	int jSPup;
	int cIdx; // MS index

	float M1[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int iM1[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int iM0[8] = { 0,0,0,0,0,0,0,0 };
	float PIntX[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. }; // intersection points coordinates
	float PIntY[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	float FIntX[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. }; // intersection points coordinates (subpixel)
	float FIntY[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	float fIntX, fIntY; // subpix coord. measured by 
	int Psubpix[8] = { 0,0,0,0,0,0,0,0 };
	int PInti[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };	  // index array of intersection points 
	int PCLC[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };   // CLC array of intersection points 
	int Pdir[8] = { 0,0,0,0,0,0,0,0 };
	int Pnums[8] = { 0,0,0,0,0,0,0,0 };
	int vpnums;
	//** Intersection point IRBS val,
	int	PIRBS[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int irbs;
	float ixc;
	float iyc;
	//int cIdx = threadIdx.x + blockIdx.x*blockDim.x;
	//int tid = blockDim.x * blockIdx.x + threadIdx.x;
	for (cIdx = pst; cIdx < pst + pn; cIdx++)
	{
		if (cIdx == 926360)
			cIdx = cIdx;
		memcpy(&PIntX[0], &M1[0], 8 * sizeof(float));
		memcpy(&PIntY[0], &M1[0], 8 * sizeof(float));
		memcpy(&FIntX[0], &M1[0], 8 * sizeof(float));
		memcpy(&FIntY[0], &M1[0], 8 * sizeof(float));
		memcpy(&PInti[0], &iM1[0], 8 * sizeof(int));
		memcpy(&PCLC[0], &iM1[0], 8 * sizeof(int));
		memcpy(&Psubpix[0], &iM0[0], 8 * sizeof(int));
		//fw = *(d_P1xl + cIdx);
		//*(d_Wl + cIdx) = fw;
		//if (cIdx == 1883423)
		//if (cIdx == 157421)
		//if (cIdx == 162006)
		//	cIdx = cIdx;
		//int ii = 0;

		int ii = 0, clc, pclc = -2, fi1 = 0;
		pnum = (*(dPnum + cIdx));
		//useint = (*(dPuseint + cIdx));
		x1 = round(*(dP1x + cIdx)); // getting measurement sensor endpoints
		y1 = round(*(dP1y + cIdx));
		x2 = round(*(dP2x + cIdx));
		y2 = round(*(dP2y + cIdx));
		ixc = (x1 + x2) / 2.;
		iyc = (y1 + y2) / 2.;
		//pnum = (*(dPnum + cIdx)) % 250 + 3;
		int deltaX = abs(x2 - x1);
		int deltaY = abs(y2 - y1);
		int signX = x1 < x2 ? 1 : -1;
		int signY = y1 < y2 ? 1 : -1;
		int error = deltaX - deltaY;
		int error2;
		int intind = 0; // intersection index (0 or 1)
		int pind = 0;   // B's point index [0...63]
		int xintc = x1, yintc = y1; // current and previous intersection points
		//v1 = *(pIm + y1 * imw + x1);  // 1st IRB pixel

		vp = 0;  // previous pixel
		/// filling in Bresenham's array
		while (x1 != x2 || y1 != y2)
		{
			PPx[pind] = x1;
			PPy[pind] = y1;
			v = *(pImIRB + y1 * imw + x1);
			Pval[pind] = v;
			error2 = error * 2;
			if (error2 > -deltaY)
			{
				error -= deltaY;
				x1 += signX;
			}
			if (error2 < deltaX)
			{
				error += deltaX;
				y1 += signY;
			}
			if (pind < 127)
				pind++;
			else
				break;
		} // while
		// Going forward from PS1

		for (i = 0; i <= pind - 1; i++)
		{
			xc = PPx[i]; yc = PPy[i];
			v = Pval[i];
			//irbs = *(pImIRBS + yc * imw + xc);  // IRBSC pixel value
			if (v == pnum)
			{	PIntY[ii] = yc;
				PIntX[ii] = xc;
				PInti[ii] = i;
				Psubpix[ii] = 1;
				if (ii < 7)
					ii = ii + 1;
			}
			
		}
		//********************************************************************
		int ga[3]; // array of 3 neighboring g
		int nmwc = 0;//*number of MW (main wire) candidates
		int mw = -1;//* MW segment
		int dclc, dmin = 55555; // min CLC distance
		int d, d1, d2; // CLC distances
		int intnum = ii; /// number of intersection points found
		jSPdn = 0;
		jSPup = ii - 1;
		if (intnum == 0)
		{
			wmax = 0.5;
			wmin = 0.5;
			shift = 0.0;
			jSPdn = 0;
			jSPup = 0;
		}
		else 
		{	if (intnum == 1) //** single pix. space case
			{
				i = PInti[0];
				wmin = 1.;
				wmax = 1.;
				jSPdn = 0;
				//jSPup = 0;
				intnum = 2;
				PInti[0] = i - 1;
				PInti[1] = i + 1;
				jSPup = 1;
			}
		 ///**** NORMAL CASE ******
	//**************** SUBPIX COORD. ******************************
			for (j = 0; j < intnum; j++)
			{
				if (PInti[j] >= 0)
				{
					ga[1] = *(pIm + PPy[PInti[j]] * imw + PPx[PInti[j]]);
					if (PInti[j] > 0)
						ga[0] = *(pIm + PPy[PInti[j] - 1] * imw + PPx[PInti[j] - 1]);
					else
						ga[0] = ga[1];
					if (PInti[j] < pind - 1)
						ga[2] = *(pIm + PPy[PInti[j] + 1] * imw + PPx[PInti[j] + 1]);
					else
						ga[2] = ga[1];

					SubPixCrdS(ga, PInti[j], PPx, PPy, &fIntX, &fIntY, thr);
					FIntX[j] = fIntX;
					FIntY[j] = fIntY;
				}
			}

			//*************************************************
			wmax = sqrt((FIntX[jSPup] - FIntX[jSPdn])*(FIntX[jSPup] - FIntX[jSPdn]) + (FIntY[jSPup] - FIntY[jSPdn])*(FIntY[jSPup] - FIntY[jSPdn]));
			//* wmax : = sqrt((FIntX[intnum - 1] - FIntX[0])*(FIntX[intnum - 1] - FIntX[0]) + (FIntY[intnum - 1] - FIntY[0])*(FIntY[intnum - 1] - FIntY[0]))
			//wmin = sqrt((FIntX[mw] - FIntX[mw + 1])*(FIntX[mw] - FIntX[mw + 1]) + (FIntY[mw] - FIntY[mw + 1])*(FIntY[mw] - FIntY[mw + 1]));
			wmin = wmax;
			if (wmax > 3.0)
			{ // calculating shift
				xmid = (FIntX[jSPup] + FIntX[jSPdn]) / 2.;
				ymid = (FIntY[jSPup] + FIntY[jSPdn]) / 2.;
				shift = sqrt((ixc - xmid)*(ixc - xmid) + (iyc - ymid)*(iyc - ymid));
			}

			//} // **** normal case end ***

			
		}

		if(PIntY[jSPup]<0 || PIntY[jSPdn] < 0)
			cIdx = cIdx;
		*(dW + cIdx) = wmax;
		*(dWmin + cIdx) = wmin;
		*(dShift + cIdx) = shift;

		*(dPint1x + cIdx) = FIntX[jSPup];
		*(dPint2x + cIdx) = FIntX[jSPdn];

		*(dPint1y + cIdx) = FIntY[jSPup];
		*(dPint2y + cIdx) = FIntY[jSPdn];

		/// saving number of intersection points ( 2 points) (left name Puseint for simplicity of changes)
		*(dPuseint + cIdx) = intnum;
		//********************************************	
	} // index cycle end
}


// 2022.06.28 - added NINT (Puseint)
// 2022.07.07 - added MSS (for both FWM/FWMS)
// 2022.08.18 - multiple changes
//** 2022.08.08 MSSProc33() checkes IRBS value to avoid selecting wrong int. point
void CPU_MSSProc33(UINT8 *pIm, UINT8 *pImIRB, UINT8 *pImIRBS, UINT16 *pImCLC, int imw, int imh, int pst, int pn, float thr, int dclcmax, int pat0space1, float *dP1x, float *dP1y, float *dP2x, float *dP2y, int *dPnum, float *dW, float *dWmin, float *dPint1x, float *dPint1y, float *dPint2x, float *dPint2y, float *dShift, int *dPuseint)
{
	int x1, y1, x2, y2, xc, yc, pnum, useint;
	int v, vp, vn, v1, v2, i, j;
	float wmin, wmax;		 // for width calculation
	float shift = 0.0, xmid, ymid; // for shift calculation
	int PPx[128];  // Bresenham's MS points array
	int PPy[128];
	int Pval[128]; // pnum values array for all MS points


	int g, gp; // current and previous gray values
	int jSPdn;
	int jSPup;
	int cIdx; // MS index

	float M1[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int iM1[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int iM0[8] = { 0,0,0,0,0,0,0,0 };
	float PIntX[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. }; // intersection points coordinates
	float PIntY[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	float FIntX[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. }; // intersection points coordinates (subpixel)
	float FIntY[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	float fIntX, fIntY; // subpix coord. measured by 
	int Psubpix[8] = { 0,0,0,0,0,0,0,0 };
	int PInti[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };	  // index array of intersection points 
	int PCLC[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };   // CLC array of intersection points 
	int Pdir[8] = { 0,0,0,0,0,0,0,0 };
	int Pnums[8] ={ 0,0,0,0,0,0,0,0 };
	int vpnums;
	//** Intersection point IRBS val,
	int	PIRBS[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int irbs;
	float ixc;
	float iyc;
	//int cIdx = threadIdx.x + blockIdx.x*blockDim.x;
	//int tid = blockDim.x * blockIdx.x + threadIdx.x;
	for (cIdx = pst; cIdx < pst + pn; cIdx++)
	{
		if (cIdx == 35875)
			cIdx = cIdx;
		memcpy(&PIntX[0], &M1[0], 8 * sizeof(float));
		memcpy(&PIntY[0], &M1[0], 8 * sizeof(float));
		memcpy(&FIntX[0], &M1[0], 8 * sizeof(float));
		memcpy(&FIntY[0], &M1[0], 8 * sizeof(float));
		memcpy(&PInti[0], &iM1[0], 8 * sizeof(int));
		memcpy(&PCLC[0],  &iM1[0], 8 * sizeof(int));
		memcpy(&Psubpix[0], &iM0[0], 8 * sizeof(int));
		//fw = *(d_P1xl + cIdx);
		//*(d_Wl + cIdx) = fw;
		//if (cIdx == 1883423)
		//if (cIdx == 157421)
		if (cIdx == 1960095)
			cIdx = cIdx;
		//int ii = 0;

		int ii = 0, clc, pclc = -2, fi1 = 0;
		pnum = (*(dPnum + cIdx));
		//useint = (*(dPuseint + cIdx));
		x1 = round(*(dP1x + cIdx)); // getting measurement sensor endpoints
		y1 = round(*(dP1y + cIdx));
		x2 = round(*(dP2x + cIdx));
		y2 = round(*(dP2y + cIdx));
		ixc = (x1 + x2) / 2.;
		iyc = (y1 + y2) / 2.;
		//pnum = (*(dPnum + cIdx)) % 250 + 3;
		int deltaX = abs(x2 - x1);
		int deltaY = abs(y2 - y1);
		int signX = x1 < x2 ? 1 : -1;
		int signY = y1 < y2 ? 1 : -1;
		int error = deltaX - deltaY;
		int error2;
		int intind = 0; // intersection index (0 or 1)
		int pind = 0;   // B's point index [0...63]
		int xintc = x1, yintc = y1; // current and previous intersection points
		//v1 = *(pIm + y1 * imw + x1);  // 1st IRB pixel

		vp = 0;  // previous pixel
		/// filling in Bresenham's array
		while (x1 != x2 || y1 != y2)
		{
			PPx[pind] = x1;
			PPy[pind] = y1;
			v = *(pImIRB + y1 * imw + x1);
			Pval[pind] = v;
			error2 = error * 2;
			if (error2 > -deltaY)
			{
				error -= deltaY;
				x1 += signX;
			}
			if (error2 < deltaX)
			{
				error += deltaX;
				y1 += signY;
			}
			if (pind < 127)
				pind++;
			else
				break;
		} // while
		// Going forward from PS1
//******* checking 1st MS pixel  *****
		if (Pval[0] == 1 || Pval[0] == pnum)
		{
			PIntY[ii] = PPy[0];
			PIntX[ii] = PPx[0];
			PInti[ii] = 0;
			clc = *(pImCLC + y1 * imw + x1);  // CLC pixel value
			//get_grayval(ImageCLC, PPy[0], PPx[0], clc)
			if (Pval[0] == pnum)
				PCLC[ii] = clc;
			else // no clc yet
				PCLC[ii] = -2;
			if (ii < 7)
				ii++;
			if (Pval[0] == 1) // 1st pix is 1
				fi1 = 1; //flag of non-confirmed inters. point
			if (Pval[1] == 0 && Pval[0] == pnum)
			{
				PIntY[ii] = PPy[0];
				PIntX[ii] = PPx[0];
				PInti[ii] = 0;
				PCLC[ii] = clc;
				if (ii < 7)ii++;
			}
		}
		else
			fi1 = 0;
		// all MS pixels except 1st and last ******************************
		for (i = 1; i <= pind - 2; i++)
		{
			xc = PPx[i]; yc = PPy[i];
			v = Pval[i];
			if (fi1 == 1)
			{
				if (v == 0 || (v > 1 && v != pnum))
				{
					if (ii > 0)
						ii--;
					fi1 = 0;
				}
			}
			if (v == pnum)
			{	//previous and next values
				fi1 = 0; // removing flag of non-confirmed inters. point
				vp = Pval[i - 1];
				vn = Pval[i + 1];
				if (vp == pat0space1 ||  vn == pat0space1)
				{
					clc = *(pImCLC + yc * imw + xc);  // CLC pixel value
					irbs = *(pImIRBS + yc * imw + xc);  // CLC pixel value
					PIntY[ii] = yc;
					PIntX[ii] = xc;
					PInti[ii] = i;
					Psubpix[ii] = 1;
					if ((vp == 0) && vn == 0) //* single point special case
					{
						if (ii < 7)
							ii = ii + 1;
						PIntY[ii - 1] = yc - 0.35*signY;
						PIntX[ii - 1] = xc - 0.35*signX;
						Psubpix[ii - 1] = 0;
						PIntY[ii] = yc + 0.35*signY;
						PIntX[ii] = xc + 0.35*signX;
						PInti[ii] = i;
						Psubpix[ii] = 0;
					}
					PCLC[ii] = clc;
					PIRBS[ii] = irbs;
					if (ii < 7)
						ii = ii + 1;
					pclc = clc;
				}
			}
			vp = v;
			//gp = g;
		}
		//********************************************************************
		int ga[3]; // array of 3 neighboring g
		int nmwc = 0;//*number of MW (main wire) candidates
		int mw = -1;//* MW segment
		int dclc, dmin = 55555; // min CLC distance
		int d, d1, d2; // CLC distances

			//, g1[3];
		//int g2[3], g3[3];
// *checking last MS pixel **********************************************
		if ((Pval[pind - 1] == 1 && ii % 2 == 1) || Pval[pind - 1] == pnum)
		{
			if (Pval[pind - 1] == pnum)
			{
				clc = *(pImCLC + PPy[pind - 1] * imw + PPx[pind - 1]);  // CLC pixel value
				irbs = *(pImIRBS + PPy[pind - 1] * imw + PPx[pind - 1]);  // IRBS pixel value
			}
				//get_grayval(ImageCLC, PPy[pind - 1], PPx[pind - 1], clc)
			else
			{
				clc = PCLC[ii - 1];
				irbs = PIRBS[ii - 1];
			}
			PIntY[ii] = PPy[pind - 1];
			PIntX[ii] = PPx[pind - 1];
			PInti[ii] = pind - 1;
			PCLC[ii] = clc;
			if (ii < 7)
				ii = ii + 1;
			if (Pval[pind - 2] == 0)
			{
				PIntY[ii] = PPy[pind - 1];
				PIntX[ii] = PPx[pind - 1];
				PInti[ii] = pind - 1;
				PCLC[ii] = clc;
				PIRBS[ii] = irbs;
				if (ii < 7)
					ii = ii + 1;
			}
		}
		int intnum = ii; /// number of intersection points found
		if (intnum <= 1) //** no pattern case
		{
			wmin = 0.;
			wmax = 0.;
			jSPdn = 0;
			jSPup = 0;
		}
		else
		{ ///**** NORMAL CASE ******
	//**************** SUBPIX COORD. ******************************
			//for (j = 0; j < intnum; j++)
			//{
			//	if (Psubpix[j] == 0)
			//	{
			//		FIntX[j] = PIntX[j];
			//		FIntY[j] = PIntY[j];
			//	}

			//	else
			//	{
			//		ga[1] = *(pIm + PPy[PInti[j]] * imw + PPx[PInti[j]]);
			//		//get_grayval(Im, PPy[i2 - 1], PPx[i2 - 1], g)
			//		ga[0] = *(pIm + PPy[PInti[j] - 1] * imw + PPx[PInti[j] - 1]);
			//		ga[2] = *(pIm + PPy[PInti[j] + 1] * imw + PPx[PInti[j] + 1]);
			//		SubPixCrd(ga, PInti[j], PPx, PPy, &fIntX, &fIntY, thr);
			//		FIntX[j] = fIntX;
			//		FIntY[j] = fIntY;
			//	}
			//}

			//*** previous dir
			int pdir = -1;
			//**** new
			for (j = 0; j < intnum; j++)
			{
				ga[1] = *(pIm + PPy[PInti[j]] * imw + PPx[PInti[j]]);
				if (PInti[j] > 0)
					ga[0] = *(pIm + PPy[PInti[j] - 1] * imw + PPx[PInti[j] - 1]);
				else
					ga[0] = ga[1];
				if (PInti[j] < pind-1)
					ga[2] = *(pIm + PPy[PInti[j] + 1] * imw + PPx[PInti[j] + 1]);
				else
					ga[2] = ga[1];

				if (Psubpix[j] == 0)
				{
					FIntX[j] = PIntX[j];
					FIntY[j] = PIntY[j];
				}

				else
				{
					SubPixCrd(ga, PInti[j], PPx, PPy, &fIntX, &fIntY, thr);
					FIntX[j] = fIntX;
					FIntY[j] = fIntY;
				}

				if (ga[2] - ga[0] > 0)
					Pdir[j] = 1;
				else if (ga[0] - ga[2] > 0)
					Pdir[j] = -1;
				if (pdir > 0)
					Pdir[j] = -1;
				else if (pdir < 0)
					Pdir[j] = 1;
				if (PInti[j] == 0 && Pdir[j] != 0 && j == 0)
					Pdir[j] = 1;
				if(PInti[j] == pind - 1 && Pdir[j] != 0 && j == intnum - 1)
					Pdir[j] = -1;
				pdir = Pdir[j];
			}
			//****
			//* ******************* Check Segments ********
				//*** finding MW segment
			    //*** fixing PCLC
			int vclc;
				for (j = 0; j < intnum; j++)
				{
					//get_grayval(ImageCLC, PIntY[j], PIntX[j], vclc)
					vclc = *(pImCLC + (int)PIntY[j] * imw + (int)PIntX[j]);  // CLC pixel value
					vpnums = *(pImIRBS + (int)PIntY[j] * imw + (int)PIntX[j]);  // IRBS pixel value
					//PCLC[j] = clc;
					if (vclc == 0)
					{
						clc = 0;
						for (int dx = -3; dx <= 3; dx++)
						{
							for (int dy = -3; dy <= 3; dy++)
							{
								//get_grayval(ImageCLC, PIntY[j] + dy, PIntX[j] + dx, vclc)
								vclc = *(pImCLC + ((int)PIntY[j] + dy) * imw + (int)PIntX[j] + dx);
								if (vclc != 0)
								{
									clc = vclc;
									break;
								}
							}

							PCLC[j] = clc;
						}
					}
					Pnums[j] = vpnums;
				}
					
///		********* checking segments for main wire (mw) *****
			for (j = 0; j < intnum; j += 2)
			{
				pclc = PCLC[j];
				clc = PCLC[j + 1];
				dclc = min(abs(clc - pclc), abs(clc - 65536 - pclc));
				if (dclc > dclcmax)
				{
					if (Pdir[j] != 1 || Pdir[j + 1] != -1)
						continue;
					//* could be MW, so check distance from P1 and P2
					//*         da : = (ixc - (FIntX[j] + FIntX[j + 1]) / 2.)*(ixc - (FIntX[j] + FIntX[j + 1]) / 2.) + (iyc - (FIntY[j] + FIntY[j + 1]) / 2.)*(iyc - (FIntY[j] + FIntY[j + 1]) / 2.)
					d1 = (ixc - FIntX[j])*(ixc - FIntX[j]) + (iyc - FIntY[j])*(iyc - FIntY[j]);
					d2 = (ixc - FIntX[j + 1])*(ixc - FIntX[j + 1]) + (iyc - FIntY[j + 1])*(iyc - FIntY[j + 1]);
					d = sqrt(min(d1, d2));
					if (d < dmin)
					{
						mw = j;
						nmwc = nmwc + 1;
						dmin = d;
					}
				}
			}
			if (mw == -1)
				mw = 0;
			//*** finding SP (spur) points
			jSPdn = mw;
			jSPup = mw + 1;
			int clc1, clc2, dclc1, dclc2;
			for (j = 0; j < intnum; j += 2)
			{
				if (j == mw)
					continue;
				else
				{
					clc = PCLC[j];
					clc1 = PCLC[mw];
					clc2 = PCLC[mw + 1];
					dclc1 = min(abs(clc - clc1), abs(clc - 65536 - clc1));
					dclc2 = min(abs(clc - clc2), abs(clc - 65536 - clc2));
					if (dclc1 > dclcmax && dclc2 > dclcmax)
						continue;//* not SP because of big dCLC
					else
					{
						if (j < mw)
						{
							dclc = min(abs(PCLC[mw] - clc1), abs(PCLC[mw] - 65536 - clc1));
							if (dclc > dclcmax)
								continue;//* not SP because of big dCLC from MW point
							else
								jSPdn = j;
						}
						else
						{
							dclc = min(abs(PCLC[mw + 1] - clc), abs(PCLC[mw + 1] - 65536 - clc));
							if (dclc > dclcmax)
								continue; //* not SP because of big dCLC from MW point
							else
								jSPup = j + 1;
						}
					}
				}
			}// for
			//*************************************************
			wmax = sqrt((FIntX[jSPup] - FIntX[jSPdn])*(FIntX[jSPup] - FIntX[jSPdn]) + (FIntY[jSPup] - FIntY[jSPdn])*(FIntY[jSPup] - FIntY[jSPdn]));
			//* wmax : = sqrt((FIntX[intnum - 1] - FIntX[0])*(FIntX[intnum - 1] - FIntX[0]) + (FIntY[intnum - 1] - FIntY[0])*(FIntY[intnum - 1] - FIntY[0]))
			wmin = sqrt((FIntX[mw] - FIntX[mw + 1])*(FIntX[mw] - FIntX[mw + 1]) + (FIntY[mw] - FIntY[mw + 1])*(FIntY[mw] - FIntY[mw + 1]));
			if (wmax > 3.0)
			{ // calculating shift
				xmid = (FIntX[jSPup] + FIntX[jSPdn]) / 2.;
				ymid = (FIntY[jSPup] + FIntY[jSPdn]) / 2.;
				shift = sqrt((ixc - xmid)*(ixc - xmid) + (iyc - ymid)*(iyc - ymid));
			}

		} // **** normal case end ***

		*(dW + cIdx) = wmax;
		*(dWmin + cIdx) = wmin;
		*(dShift + cIdx) = shift;

		*(dPint1x + cIdx) = PIntX[jSPup];
		*(dPint2x + cIdx) = PIntX[jSPdn];

		*(dPint1y + cIdx) = PIntY[jSPup];
		*(dPint2y + cIdx) = PIntY[jSPdn];

		/// saving number of intersection points ( 2 points) (left name Puseint for simplicity of changes)
		*(dPuseint + cIdx) = intnum;

		/*if (intnum == 2)
			*(dPuseint + cIdx) = 1;
		else
			*(dPuseint + cIdx) = 0;
		*/
		//********************************************	
	} // index cycle end
}

// 2022.06.28 - added NINT (Puseint)
// 2022.07.07 - added MSS (for both FWM/FWMS)
void CPU_MSSProc32(UINT8 *pIm, UINT8 *pImIRB, UINT16 *pImCLC, int imw, int imh, int pst, int pn, float thr, int dclcmax, int pat0space1, float *dP1x, float *dP1y, float *dP2x, float *dP2y, int *dPnum, float *dW, float *dWmin, float *dPint1x, float *dPint1y, float *dPint2x, float *dPint2y, float *dShift, int *dPuseint)
{
	int x1, y1, x2, y2, xc, yc, pnum, useint;
	int v, vp, vn, v1, v2, i, j;
	float wmin, wmax;		 // for width calculation
	float shift = 0.0, xmid, ymid; // for shift calculation
	int PPx[128];  // Bresenham's MS points array
	int PPy[128];
	int Pval[128]; // pnum values array for all MS points


	int g, gp; // current and previous gray values
	int jSPdn;
	int jSPup;
	int cIdx; // MS index

	float M1[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int iM1[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int iM0[8] = { 0,0,0,0,0,0,0,0 };
	float PIntX[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. }; // intersection points coordinates
	float PIntY[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	float FIntX[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. }; // intersection points coordinates (subpixel)
	float FIntY[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	float fIntX, fIntY; // subpix coord. measured by 
	int Psubpix[8] = { 0,0,0,0,0,0,0,0 };
	int PInti[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };	  // index array of intersection points 
	int PCLC[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };   // CLC array of intersection points 

	float ixc;
	float iyc;
	//int cIdx = threadIdx.x + blockIdx.x*blockDim.x;
	//int tid = blockDim.x * blockIdx.x + threadIdx.x;
	for (cIdx = pst; cIdx < pst + pn; cIdx++)
	{
		memcpy(&PIntX[0], &M1[0], 8 * sizeof(float));
		memcpy(&PIntY[0], &M1[0], 8 * sizeof(float));
		memcpy(&FIntX[0], &M1[0], 8 * sizeof(float));
		memcpy(&FIntY[0], &M1[0], 8 * sizeof(float));
		memcpy(&PInti[0], &iM1[0], 8 * sizeof(int));
		memcpy(&PCLC[0], &iM1[0], 8 * sizeof(int));
		memcpy(&Psubpix[0], &iM0[0], 8 * sizeof(int));
		//fw = *(d_P1xl + cIdx);
		//*(d_Wl + cIdx) = fw;
		//if (cIdx == 1883423)
		//if (cIdx == 157421)
		if (cIdx == 1960095)
			cIdx = cIdx;
		//int ii = 0;

		int ii = 0, clc, pclc = -2, fi1 = 0;
		pnum = (*(dPnum + cIdx));
		//useint = (*(dPuseint + cIdx));
		x1 = round(*(dP1x + cIdx)); // getting measurement sensor endpoints
		y1 = round(*(dP1y + cIdx));
		x2 = round(*(dP2x + cIdx));
		y2 = round(*(dP2y + cIdx));
		ixc = (x1 + x2) / 2.;
		iyc = (y1 + y2) / 2.;
		//pnum = (*(dPnum + cIdx)) % 250 + 3;
		int deltaX = abs(x2 - x1);
		int deltaY = abs(y2 - y1);
		int signX = x1 < x2 ? 1 : -1;
		int signY = y1 < y2 ? 1 : -1;
		int error = deltaX - deltaY;
		int error2;
		int intind = 0; // intersection index (0 or 1)
		int pind = 0;   // B's point index [0...63]
		int xintc = x1, yintc = y1; // current and previous intersection points
		//v1 = *(pIm + y1 * imw + x1);  // 1st IRB pixel

		vp = 0;  // previous pixel
		/// filling in Bresenham's array
		while (x1 != x2 || y1 != y2)
		{
			PPx[pind] = x1;
			PPy[pind] = y1;
			v = *(pImIRB + y1 * imw + x1);
			Pval[pind] = v;
			error2 = error * 2;
			if (error2 > -deltaY)
			{
				error -= deltaY;
				x1 += signX;
			}
			if (error2 < deltaX)
			{
				error += deltaX;
				y1 += signY;
			}
			if (pind < 127)
				pind++;
			else
				break;
		} // while
		// Going forward from PS1
//******* checking 1st MS pixel  *****
		if (Pval[0] == 1 || Pval[0] == pnum)
		{
			PIntY[ii] = PPy[0];
			PIntX[ii] = PPx[0];
			PInti[ii] = 0;
			clc = *(pImCLC + y1 * imw + x1);  // CLC pixel value
			//get_grayval(ImageCLC, PPy[0], PPx[0], clc)
			if (Pval[0] == pnum)
				PCLC[ii] = clc;
			else // no clc yet
				PCLC[ii] = -2;
			if (ii < 7)
				ii++;
			if (Pval[0] == 1) // 1st pix is 1
				fi1 = 1; //flag of non-confirmed inters. point
			if (Pval[1] == 0 && Pval[0] == pnum)
			{
				PIntY[ii] = PPy[0];
				PIntX[ii] = PPx[0];
				PInti[ii] = 0;
				PCLC[ii] = clc;
				if (ii < 7)ii++;
			}
		}
		else
			fi1 = 0;
		// all MS pixels except 1st and last ******************************
		for (i = 1; i <= pind - 2; i++)
		{
			xc = PPx[i]; yc = PPy[i];
			v = Pval[i];
			if (fi1 == 1)
			{
				if (v == 0 || (v > 1 && v != pnum))
				{
					if (ii > 0)
						ii--;
					fi1 = 0;
				}
			}
			if (v == pnum)
			{	//previous and next values
				fi1 = 0; // removing flag of non-confirmed inters. point
				vp = Pval[i - 1];
				vn = Pval[i + 1];
				if (vp == pat0space1 || vn == pat0space1)
				{
					clc = *(pImCLC + yc * imw + xc);  // CLC pixel value
					PIntY[ii] = yc;
					PIntX[ii] = xc;
					PInti[ii] = i;
					Psubpix[ii] = 1;
					if ((vp == 0) && vn == 0) //* single point special case
					{
						if (ii < 7)
							ii = ii + 1;
						PIntY[ii - 1] = yc - 0.35*signY;
						PIntX[ii - 1] = xc - 0.35*signX;
						Psubpix[ii - 1] = 0;
						PIntY[ii] = yc + 0.35*signY;
						PIntX[ii] = xc + 0.35*signX;
						PInti[ii] = i;
						Psubpix[ii] = 0;
					}
					PCLC[ii] = clc;
					if (ii < 7)
						ii = ii + 1;
					pclc = clc;
				}
			}
			vp = v;
			//gp = g;
		}
		//********************************************************************
		int ga[3]; // array of 3 neighboring g
		int nmwc = 0;//*number of MW (main wire) candidates
		int mw = -1;//* MW segment
		int dclc, dmin = 55555; // min CLC distance
		int d, d1, d2; // CLC distances

			//, g1[3];
		//int g2[3], g3[3];
// *checking last MS pixel **********************************************
		if ((Pval[pind - 1] == 1 && ii % 2 == 1) || Pval[pind - 1] == pnum)
		{
			if (Pval[pind - 1] == pnum)
				clc = *(pImCLC + PPy[pind - 1] * imw + PPx[pind - 1]);  // CLC pixel value
				//get_grayval(ImageCLC, PPy[pind - 1], PPx[pind - 1], clc)
			else
				clc = PCLC[ii - 1];

			PIntY[ii] = PPy[pind - 1];
			PIntX[ii] = PPx[pind - 1];
			PInti[ii] = pind - 1;
			PCLC[ii] = clc;
			if (ii < 7)
				ii = ii + 1;
			if (Pval[pind - 2] == 0)
			{
				PIntY[ii] = PPy[pind - 1];
				PIntX[ii] = PPx[pind - 1];
				PInti[ii] = pind - 1;
				PCLC[ii] = clc;
				if (ii < 7)
					ii = ii + 1;
			}
		}
		int intnum = ii; /// number of intersection points found
		if (intnum <= 1) //** no pattern case
		{
			wmin = 0.;
			wmax = 0.;
			jSPdn = 0;
			jSPup = 0;
		}
		else
		{ ///**** NORMAL CASE ******
	//**************** SUBPIX COORD. ******************************
			for (j = 0; j < intnum; j++)
			{
				if (Psubpix[j] == 0)
				{
					FIntX[j] = PIntX[j];
					FIntY[j] = PIntY[j];
				}

				else
				{
					ga[1] = *(pIm + PPy[PInti[j]] * imw + PPx[PInti[j]]);
					//get_grayval(Im, PPy[i2 - 1], PPx[i2 - 1], g)
					ga[0] = *(pIm + PPy[PInti[j] - 1] * imw + PPx[PInti[j] - 1]);
					ga[2] = *(pIm + PPy[PInti[j] + 1] * imw + PPx[PInti[j] + 1]);
					SubPixCrd(ga, PInti[j], PPx, PPy, &fIntX, &fIntY, thr);
					FIntX[j] = fIntX;
					FIntY[j] = fIntY;
				}
			}
			//* ******************* Check Segments ********
				//*** finding MW segment
				//*** fixing PCLC
			for (j = 0; j < intnum; j++)
			{
				//get_grayval(ImageCLC, PIntY[j], PIntX[j], vclc)
				clc = *(pImCLC + (int)PIntY[j] * imw + (int)PIntX[j]);  // CLC pixel value
				PCLC[j] = clc;
			}


			for (j = 0; j < intnum; j += 2)
			{
				pclc = PCLC[j];
				clc = PCLC[j + 1];
				dclc = min(abs(clc - pclc), abs(clc - 65536 - pclc));
				if (dclc > dclcmax)
				{
					//* could be MW, so check distance from P1 and P2
					//*         da : = (ixc - (FIntX[j] + FIntX[j + 1]) / 2.)*(ixc - (FIntX[j] + FIntX[j + 1]) / 2.) + (iyc - (FIntY[j] + FIntY[j + 1]) / 2.)*(iyc - (FIntY[j] + FIntY[j + 1]) / 2.)
					d1 = (ixc - FIntX[j])*(ixc - FIntX[j]) + (iyc - FIntY[j])*(iyc - FIntY[j]);
					d2 = (ixc - FIntX[j + 1])*(ixc - FIntX[j + 1]) + (iyc - FIntY[j + 1])*(iyc - FIntY[j + 1]);
					d = sqrt(min(d1, d2));
					if (d < dmin)
					{
						mw = j;
						nmwc = nmwc + 1;
						dmin = d;
					}
				}
			}
			if (mw == -1)
				mw = 0;
			//*** finding SP (spur) points
			jSPdn = mw;
			jSPup = mw + 1;
			int clc1, clc2, dclc1, dclc2;
			for (j = 0; j < intnum; j += 2)
			{
				if (j == mw)
					continue;
				else
				{
					clc = PCLC[j];
					clc1 = PCLC[mw];
					clc2 = PCLC[mw + 1];
					dclc1 = min(abs(clc - clc1), abs(clc - 65536 - clc1));
					dclc2 = min(abs(clc - clc2), abs(clc - 65536 - clc2));
					if (dclc1 > dclcmax && dclc2 > dclcmax)
						continue;//* not SP because of big dCLC
					else
					{
						if (j < mw)
						{
							dclc = min(abs(PCLC[mw] - clc1), abs(PCLC[mw] - 65536 - clc1));
							if (dclc > dclcmax)
								continue;//* not SP because of big dCLC from MW point
							else
								jSPdn = j;
						}
						else
						{
							dclc = min(abs(PCLC[mw + 1] - clc), abs(PCLC[mw + 1] - 65536 - clc));
							if (dclc > dclcmax)
								continue; //* not SP because of big dCLC from MW point
							else
								jSPup = j + 1;
						}
					}
				}
			}// for
			//*************************************************
			wmax = sqrt((FIntX[jSPup] - FIntX[jSPdn])*(FIntX[jSPup] - FIntX[jSPdn]) + (FIntY[jSPup] - FIntY[jSPdn])*(FIntY[jSPup] - FIntY[jSPdn]));
			//* wmax : = sqrt((FIntX[intnum - 1] - FIntX[0])*(FIntX[intnum - 1] - FIntX[0]) + (FIntY[intnum - 1] - FIntY[0])*(FIntY[intnum - 1] - FIntY[0]))
			wmin = sqrt((FIntX[mw] - FIntX[mw + 1])*(FIntX[mw] - FIntX[mw + 1]) + (FIntY[mw] - FIntY[mw + 1])*(FIntY[mw] - FIntY[mw + 1]));
			if (wmax > 3.0)
			{ // calculating shift
				xmid = (FIntX[jSPup] + FIntX[jSPdn]) / 2.;
				ymid = (FIntY[jSPup] + FIntY[jSPdn]) / 2.;
				shift = sqrt((ixc - xmid)*(ixc - xmid) + (iyc - ymid)*(iyc - ymid));
			}

		} // **** normal case end ***

		*(dW + cIdx) = wmax;
		*(dWmin + cIdx) = wmin;
		*(dShift + cIdx) = shift;

		*(dPint1x + cIdx) = PIntX[jSPup];
		*(dPint2x + cIdx) = PIntX[jSPdn];

		*(dPint1y + cIdx) = PIntY[jSPup];
		*(dPint2y + cIdx) = PIntY[jSPdn];

		/// saving number of intersection points ( 2 points) (left name Puseint for simplicity of changes)
		*(dPuseint + cIdx) = intnum;

		/*if (intnum == 2)
			*(dPuseint + cIdx) = 1;
		else
			*(dPuseint + cIdx) = 0;
		*/
		//********************************************	
	} // index cycle end
}

// Find MS intersection using Bresenham's line algorithm for CPU test, based on the HDev procedure MSProc30, 2022.03.22 (CPU version)
// 2022.06.14 - Added shift calculation
// 2022.06.16 - removed Int. points at begining and end (not yet)
// 2022.06.28 - added NINT (Puseint)
void CPU_MSProc32(UINT8 *pIm, UINT8 *pImIRB, UINT16 *pImCLC, int imw, int imh, int pst, int pn, float thr, int dclcmax, float *dP1x, float *dP1y, float *dP2x, float *dP2y, int *dPnum, float *dW, float *dWmin, float *dPint1x, float *dPint1y, float *dPint2x, float *dPint2y, float *dShift, int *dPuseint)
{
	int x1, y1, x2, y2, xc, yc, pnum, useint;
	int v, vp, vn, v1, v2, i, j;
	float wmin, wmax;		 // for width calculation
	float shift = 0.0, xmid, ymid; // for shift calculation
	int PPx[128];  // Bresenham's MS points array
	int PPy[128];
	int Pval[128]; // pnum values array for all MS points


	int g, gp; // current and previous gray values
	int jSPdn;
	int jSPup;
	int cIdx; // MS index

	float M1[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int iM1[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int iM0[8] = { 0,0,0,0,0,0,0,0 };
	float PIntX[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. }; // intersection points coordinates
	float PIntY[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	float FIntX[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. }; // intersection points coordinates (subpixel)
	float FIntY[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	float fIntX, fIntY; // subpix coord. measured by 
	int Psubpix[8] = { 0,0,0,0,0,0,0,0 };
	int PInti[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };	  // index array of intersection points 
	int PCLC[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };   // CLC array of intersection points 

	float ixc;
	float iyc;
	//int cIdx = threadIdx.x + blockIdx.x*blockDim.x;
	//int tid = blockDim.x * blockIdx.x + threadIdx.x;
	for (cIdx = pst; cIdx < pst + pn; cIdx++)
	{
		memcpy(&PIntX[0], &M1[0], 8 * sizeof(float));
		memcpy(&PIntY[0], &M1[0], 8 * sizeof(float));
		memcpy(&FIntX[0], &M1[0], 8 * sizeof(float));
		memcpy(&FIntY[0], &M1[0], 8 * sizeof(float));
		memcpy(&PInti[0], &iM1[0], 8 * sizeof(int));
		memcpy(&PCLC[0], &iM1[0], 8 * sizeof(int));
		memcpy(&Psubpix[0], &iM0[0], 8 * sizeof(int));
		//fw = *(d_P1xl + cIdx);
		//*(d_Wl + cIdx) = fw;
		//if (cIdx == 1883423)
		//if (cIdx == 157421)
		if (cIdx == 1099292)
			cIdx = cIdx;
		//int ii = 0;

		int ii = 0, clc, pclc = -2, fi1 = 0;
		pnum = (*(dPnum + cIdx));
		//useint = (*(dPuseint + cIdx));
		x1 = round(*(dP1x + cIdx)); // getting measurement sensor endpoints
		y1 = round(*(dP1y + cIdx));
		x2 = round(*(dP2x + cIdx));
		y2 = round(*(dP2y + cIdx));
		ixc = (x1 + x2) / 2.;
		iyc = (y1 + y2) / 2.;
		//pnum = (*(dPnum + cIdx)) % 250 + 3;
		int deltaX = abs(x2 - x1);
		int deltaY = abs(y2 - y1);
		int signX = x1 < x2 ? 1 : -1;
		int signY = y1 < y2 ? 1 : -1;
		int error = deltaX - deltaY;
		int error2;
		int intind = 0; // intersection index (0 or 1)
		int pind = 0;   // B's point index [0...63]
		int xintc = x1, yintc = y1; // current and previous intersection points
		//v1 = *(pIm + y1 * imw + x1);  // 1st IRB pixel

		vp = 0;  // previous pixel
		/// filling in Bresenham's array
		while (x1 != x2 || y1 != y2)
		{
			PPx[pind] = x1;
			PPy[pind] = y1;
			v = *(pImIRB + y1 * imw + x1);
			Pval[pind] = v;
			error2 = error * 2;
			if (error2 > -deltaY)
			{
				error -= deltaY;
				x1 += signX;
			}
			if (error2 < deltaX)
			{
				error += deltaX;
				y1 += signY;
			}
			if (pind < 127)
				pind++;
			else
				break;
		} // while
		// Going forward from PS1
//******* checking 1st MS pixel  *****
		if (Pval[0] == 1 || Pval[0] == pnum)
		{
			PIntY[ii] = PPy[0];
			PIntX[ii] = PPx[0];
			PInti[ii] = 0;
			clc = *(pImCLC + y1 * imw + x1);  // CLC pixel value
			//get_grayval(ImageCLC, PPy[0], PPx[0], clc)
			if (Pval[0] == pnum)
				PCLC[ii] = clc;
			else // no clc yet
				PCLC[ii] = -2;
			if (ii < 7)
				ii++;
			if (Pval[0] == 1) // 1st pix is 1
				fi1 = 1; //flag of non-confirmed inters. point
			if (Pval[1] == 0 && Pval[0] == pnum)
			{
				PIntY[ii] = PPy[0];
				PIntX[ii] = PPx[0];
				PInti[ii] = 0;
				PCLC[ii] = clc;
				if (ii < 7)ii++;
			}
		}
		else
			fi1 = 0;
		// all MS pixels except 1st and last ******************************
		for (i = 1; i <= pind - 2; i++)
		{
			xc = PPx[i]; yc = PPy[i];
			v = Pval[i];
			if (fi1 == 1)
			{
				if (v == 0 || (v > 1 && v != pnum))
				{
					if (ii > 0)
						ii--;
					fi1 = 0;
				}
			}
			if (v == pnum)
			{	//previous and next values
				fi1 = 0; // removing flag of non-confirmed inters. point
				vp = Pval[i - 1];
				vn = Pval[i + 1];
				if ((vp == 0 || (vp > 1 && vp != pnum)) || vn == 0)
				{
					clc = *(pImCLC + yc * imw + xc);  // CLC pixel value
					PIntY[ii] = yc;
					PIntX[ii] = xc;
					PInti[ii] = i;
					Psubpix[ii] = 1;
					if ((vp == 0) && vn == 0) //* single point special case
					{
						if (ii < 7)
							ii = ii + 1;
						PIntY[ii - 1] = yc - 0.35*signY;
						PIntX[ii - 1] = xc - 0.35*signX;
						Psubpix[ii - 1] = 0;
						PIntY[ii] = yc + 0.35*signY;
						PIntX[ii] = xc + 0.35*signX;
						PInti[ii] = i;
						Psubpix[ii] = 0;
					}
					PCLC[ii] = clc;
					if (ii < 7)
						ii = ii + 1;
					pclc = clc;
				}
			}
			vp = v;
			//gp = g;
		}
		//********************************************************************
		int ga[3]; // array of 3 neighboring g
		int nmwc = 0;//*number of MW (main wire) candidates
		int mw = -1;//* MW segment
		int dclc, dmin = 55555; // min CLC distance
		int d, d1, d2; // CLC distances

			//, g1[3];
		//int g2[3], g3[3];
// *checking last MS pixel **********************************************
		if ((Pval[pind - 1] == 1 && ii % 2 == 1) || Pval[pind - 1] == pnum)
		{
			if (Pval[pind - 1] == pnum)
				clc = *(pImCLC + PPy[pind - 1] * imw + PPx[pind - 1]);  // CLC pixel value
				//get_grayval(ImageCLC, PPy[pind - 1], PPx[pind - 1], clc)
			else
				clc = PCLC[ii - 1];

			PIntY[ii] = PPy[pind - 1];
			PIntX[ii] = PPx[pind - 1];
			PInti[ii] = pind - 1;
			PCLC[ii] = clc;
			if (ii < 7)
				ii = ii + 1;
			if (Pval[pind - 2] == 0)
			{
				PIntY[ii] = PPy[pind - 1];
				PIntX[ii] = PPx[pind - 1];
				PInti[ii] = pind - 1;
				PCLC[ii] = clc;
				if (ii < 7)
					ii = ii + 1;
			}
		}
		int intnum = ii; /// number of intersection points found
		if (intnum <= 1) //** no pattern case
		{
			wmin = 0.;
			wmax = 0.;
			jSPdn = 0;
			jSPup = 0;
		}
		else
		{ ///**** NORMAL CASE ******
	//**************** SUBPIX COORD. ******************************
			for (j = 0; j < intnum; j++)
			{
				if (Psubpix[j] == 0)
				{
					FIntX[j] = PIntX[j];
					FIntY[j] = PIntY[j];
				}

				else
				{
					ga[1] = *(pIm + PPy[PInti[j]] * imw + PPx[PInti[j]]);
					//get_grayval(Im, PPy[i2 - 1], PPx[i2 - 1], g)
					ga[0] = *(pIm + PPy[PInti[j] - 1] * imw + PPx[PInti[j] - 1]);
					ga[2] = *(pIm + PPy[PInti[j] + 1] * imw + PPx[PInti[j] + 1]);
					SubPixCrd(ga, PInti[j], PPx, PPy, &fIntX, &fIntY, thr);
					FIntX[j] = fIntX;
					FIntY[j] = fIntY;
				}
			}
			//* ******************* Check Segments ********
				//*** finding MW segment
			for (j = 0; j < intnum; j += 2)
			{
				pclc = PCLC[j];
				clc = PCLC[j + 1];
				dclc = min(abs(clc - pclc), abs(clc - 65536 - pclc));
				if (dclc > dclcmax)
				{
					//* could be MW, so check distance from P1 and P2
					//*         da : = (ixc - (FIntX[j] + FIntX[j + 1]) / 2.)*(ixc - (FIntX[j] + FIntX[j + 1]) / 2.) + (iyc - (FIntY[j] + FIntY[j + 1]) / 2.)*(iyc - (FIntY[j] + FIntY[j + 1]) / 2.)
					d1 = (xc - FIntX[j])*(xc - FIntX[j]) + (yc - FIntY[j])*(yc - FIntY[j]);
					d2 = (xc - FIntX[j + 1])*(xc - FIntX[j + 1]) + (yc - FIntY[j + 1])*(yc - FIntY[j + 1]);
					d = sqrt(min(d1, d2));
					if (d < dmin)
					{
						mw = j;
						nmwc = nmwc + 1;
						dmin = d;
					}
				}
			}
			if (mw == -1)
				mw = 0;
			//*** finding SP (spur) points
			jSPdn = mw;
			jSPup = mw + 1;
			int clc1, clc2, dclc1, dclc2;
			for (j = 0; j < intnum; j += 2)
			{
				if (j == mw)
					continue;
				else
				{
					clc = PCLC[j];
					clc1 = PCLC[mw];
					clc2 = PCLC[mw + 1];
					dclc1 = min(abs(clc - clc1), abs(clc - 65536 - clc1));
					dclc2 = min(abs(clc - clc2), abs(clc - 65536 - clc2));
					if (dclc1 > dclcmax && dclc2 > dclcmax)
						continue;//* not SP because of big dCLC
					else
					{
						if (j < mw)
						{
							dclc = min(abs(PCLC[mw] - clc1), abs(PCLC[mw] - 65536 - clc1));
							if (dclc > dclcmax)
								continue;//* not SP because of big dCLC from MW point
							else
								jSPdn = j;
						}
						else
						{
							dclc = min(abs(PCLC[mw + 1] - clc), abs(PCLC[mw + 1] - 65536 - clc));
							if (dclc > dclcmax)
								continue; //* not SP because of big dCLC from MW point
							else
								jSPup = j + 1;
						}
					}
				}
			}// for
			//*************************************************
			wmax = sqrt((FIntX[jSPup] - FIntX[jSPdn])*(FIntX[jSPup] - FIntX[jSPdn]) + (FIntY[jSPup] - FIntY[jSPdn])*(FIntY[jSPup] - FIntY[jSPdn]));
			//* wmax : = sqrt((FIntX[intnum - 1] - FIntX[0])*(FIntX[intnum - 1] - FIntX[0]) + (FIntY[intnum - 1] - FIntY[0])*(FIntY[intnum - 1] - FIntY[0]))
			wmin = sqrt((FIntX[mw] - FIntX[mw + 1])*(FIntX[mw] - FIntX[mw + 1]) + (FIntY[mw] - FIntY[mw + 1])*(FIntY[mw] - FIntY[mw + 1]));
			if (wmax > 3.0)
			{ // calculating shift
				xmid = (FIntX[jSPup] + FIntX[jSPdn]) / 2.;
				ymid = (FIntY[jSPup] + FIntY[jSPdn]) / 2.;
				shift = sqrt((ixc - xmid)*(ixc - xmid) + (iyc - ymid)*(iyc - ymid));
			}

		} // **** normal case end ***

		*(dW + cIdx) = wmax;
		*(dWmin + cIdx) = wmin;
		*(dShift + cIdx) = shift;
		
		*(dPint1x + cIdx) = PIntX[jSPup];
		*(dPint2x + cIdx) = PIntX[jSPdn];

		*(dPint1y + cIdx) = PIntY[jSPup];
		*(dPint2y + cIdx) = PIntY[jSPdn];

		/// checking NINT (left name Puseint for simplicity of changes
		*(dPuseint + cIdx) = intnum;
		/*if (intnum == 2)
			*(dPuseint + cIdx) = 1;
		else
			*(dPuseint + cIdx) = 0;
		*/
		//********************************************	
	} // index cycle end
}

// Find MS intersection using Bresenham's line algorithm for CPU test, based on the HDev procedure MSProc30, 2022.03.22 (CPU version)
// 2022.06.14 - Added shift calculation
// 2022.06.16 - removed Int. points at begining and end (not yet)
void CPU_MSProc31(UINT8 *pIm, UINT8 *pImIRB, UINT16 *pImCLC, int imw, int imh, int pst, int pn, float thr, int dclcmax, float *dP1x, float *dP1y, float *dP2x, float *dP2y, int *dPnum, float *dW, float *dWmin, float *dPint1x, float *dPint1y, float *dPint2x, float *dPint2y, float *dShift, int *dPuseint)
{
	int x1, y1, x2, y2, xc, yc, pnum, useint;
	int v, vp, vn, v1, v2, i, j;
	float wmin, wmax;		 // for width calculation
	float shift=0.0, xmid, ymid; // for shift calculation
	int PPx[128];  // Bresenham's MS points array
	int PPy[128];
	int Pval[128]; // pnum values array for all MS points


	int g, gp; // current and previous gray values
	int jSPdn;
	int jSPup;
	int cIdx; // MS index

	float M1[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int iM1[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int iM0[8] = { 0,0,0,0,0,0,0,0 };
	float PIntX[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. }; // intersection points coordinates
	float PIntY[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	float FIntX[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. }; // intersection points coordinates (subpixel)
	float FIntY[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	float fIntX, fIntY; // subpix coord. measured by 
	int Psubpix[8] = { 0,0,0,0,0,0,0,0 };
	int PInti[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };	  // index array of intersection points 
	int PCLC[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };   // CLC array of intersection points 

	float ixc;
	float iyc;
	//int cIdx = threadIdx.x + blockIdx.x*blockDim.x;
	//int tid = blockDim.x * blockIdx.x + threadIdx.x;
	for (cIdx = pst; cIdx < pst + pn; cIdx++)
	{
		memcpy(&PIntX[0], &M1[0], 8 * sizeof(float));
		memcpy(&PIntY[0], &M1[0], 8 * sizeof(float));
		memcpy(&FIntX[0], &M1[0], 8 * sizeof(float));
		memcpy(&FIntY[0], &M1[0], 8 * sizeof(float));
		memcpy(&PInti[0], &iM1[0], 8 * sizeof(int));
		memcpy(&PCLC[0], &iM1[0], 8 * sizeof(int));
		memcpy(&Psubpix[0], &iM0[0], 8 * sizeof(int));
		//fw = *(d_P1xl + cIdx);
		//*(d_Wl + cIdx) = fw;
		//if (cIdx == 1883423)
		//if (cIdx == 157421)
		if (cIdx == 324910)
			cIdx = cIdx;
		//int ii = 0;

		int ii = 0, clc, pclc = -2, fi1 = 0;
		pnum = (*(dPnum + cIdx));
		useint = (*(dPuseint + cIdx));
		x1 = round(*(dP1x + cIdx)); // getting measurement sensor endpoints
		y1 = round(*(dP1y + cIdx));
		x2 = round(*(dP2x + cIdx));
		y2 = round(*(dP2y + cIdx));
		ixc = (x1 + x2) / 2.;
		iyc = (y1 + y2) / 2.;
		//pnum = (*(dPnum + cIdx)) % 250 + 3;
		int deltaX = abs(x2 - x1);
		int deltaY = abs(y2 - y1);
		int signX = x1 < x2 ? 1 : -1;
		int signY = y1 < y2 ? 1 : -1;
		int error = deltaX - deltaY;
		int error2;
		int intind = 0; // intersection index (0 or 1)
		int pind = 0;   // B's point index [0...63]
		int xintc = x1, yintc = y1; // current and previous intersection points
		//v1 = *(pIm + y1 * imw + x1);  // 1st IRB pixel

		vp = 0;  // previous pixel
		/// filling in Bresenham's array
		while (x1 != x2 || y1 != y2)
		{
			PPx[pind] = x1;
			PPy[pind] = y1;
			v = *(pImIRB + y1 * imw + x1);
			Pval[pind] = v;
			error2 = error * 2;
			if (error2 > -deltaY)
			{
				error -= deltaY;
				x1 += signX;
			}
			if (error2 < deltaX)
			{
				error += deltaX;
				y1 += signY;
			}
			if (pind < 127)
				pind++;
			else
				break;
		} // while
		// Going forward from PS1
//******* checking 1st MS pixel  *****
		if (Pval[0] == 1 || Pval[0] == pnum)
		{
			PIntY[ii] = PPy[0];
			PIntX[ii] = PPx[0];
			PInti[ii] = 0;
			clc = *(pImCLC + y1 * imw + x1);  // CLC pixel value
			//get_grayval(ImageCLC, PPy[0], PPx[0], clc)
			if (Pval[0] == pnum)
				PCLC[ii] = clc;
			else // no clc yet
				PCLC[ii] = -2;
			if (ii < 7)
				ii++;
			if (Pval[0] == 1) // 1st pix is 1
				fi1 = 1; //flag of non-confirmed inters. point
			if (Pval[1] == 0 && Pval[0] == pnum)
			{
				PIntY[ii] = PPy[0];
				PIntX[ii] = PPx[0];
				PInti[ii] = 0;
				PCLC[ii] = clc;
				if (ii < 7)ii++;
			}
		}
		else
			fi1 = 0;
		// all MS pixels except 1st and last ******************************
		for (i = 1; i <= pind - 2; i++)
		{
			xc = PPx[i]; yc = PPy[i];
			v = Pval[i];
			if (fi1 == 1)
			{
				if (v == 0 || (v > 1 && v != pnum))
				{
					if (ii > 0)
						ii--;
					fi1 = 0;
				}
			}
			if (v == pnum)
			{	//previous and next values
				fi1 = 0; // removing flag of non-confirmed inters. point
				vp = Pval[i - 1];
				vn = Pval[i + 1];
				if ((vp == 0 || (vp > 1 && vp != pnum)) || vn == 0)
				{
					clc = *(pImCLC + yc * imw + xc);  // CLC pixel value
					PIntY[ii] = yc;
					PIntX[ii] = xc;
					PInti[ii] = i;
					Psubpix[ii] = 1;
					if ((vp == 0) && vn == 0) //* single point special case
					{
						if (ii < 7)
							ii = ii + 1;
						PIntY[ii - 1] = yc - 0.35*signY;
						PIntX[ii - 1] = xc - 0.35*signX;
						Psubpix[ii - 1] = 0;
						PIntY[ii] = yc + 0.35*signY;
						PIntX[ii] = xc + 0.35*signX;
						PInti[ii] = i;
						Psubpix[ii] = 0;
					}
					PCLC[ii] = clc;
					if (ii < 7)
						ii = ii + 1;
					pclc = clc;
				}
			}
			vp = v;
			//gp = g;
		}
		//********************************************************************
		int ga[3]; // array of 3 neighboring g
		int nmwc = 0;//*number of MW (main wire) candidates
		int mw = -1;//* MW segment
		int dclc, dmin = 55555; // min CLC distance
		int d, d1, d2; // CLC distances

			//, g1[3];
		//int g2[3], g3[3];
// *checking last MS pixel **********************************************
		if ((Pval[pind - 1] == 1 && ii % 2 == 1) || Pval[pind - 1] == pnum)
		{
			if (Pval[pind - 1] == pnum)
				clc = *(pImCLC + PPy[pind - 1] * imw + PPx[pind - 1]);  // CLC pixel value
				//get_grayval(ImageCLC, PPy[pind - 1], PPx[pind - 1], clc)
			else
				clc = PCLC[ii - 1];

			PIntY[ii] = PPy[pind - 1];
			PIntX[ii] = PPx[pind - 1];
			PInti[ii] = pind - 1;
			PCLC[ii] = clc;
			if (ii < 7)
				ii = ii + 1;
			if (Pval[pind - 2] == 0)
			{
				PIntY[ii] = PPy[pind - 1];
				PIntX[ii] = PPx[pind - 1];
				PInti[ii] = pind - 1;
				PCLC[ii] = clc;
				if (ii < 7)
					ii = ii + 1;
			}
		}
		int intnum = ii; /// number of intersection points found
		if (intnum <= 1) //** no pattern case
		{
			wmin = 0.;
			wmax = 0.;
			jSPdn = 0;
			jSPup = 0;
		}
		else
		{ ///**** NORMAL CASE ******
	//**************** SUBPIX COORD. ******************************
			for (j = 0; j < intnum; j++)
			{
				if (Psubpix[j] == 0)
				{
					FIntX[j] = PIntX[j];
					FIntY[j] = PIntY[j];
				}

				else
				{
					ga[1] = *(pIm + PPy[PInti[j]] * imw + PPx[PInti[j]]);
					//get_grayval(Im, PPy[i2 - 1], PPx[i2 - 1], g)
					ga[0] = *(pIm + PPy[PInti[j] - 1] * imw + PPx[PInti[j] - 1]);
					ga[2] = *(pIm + PPy[PInti[j] + 1] * imw + PPx[PInti[j] + 1]);
					SubPixCrd(ga, PInti[j], PPx, PPy, &fIntX, &fIntY, thr);
					FIntX[j] = fIntX;
					FIntY[j] = fIntY;
				}
			}
			//* ******************* Check Segments ********
				//*** finding MW segment
			for (j = 0; j < intnum; j += 2)
			{
				pclc = PCLC[j];
				clc = PCLC[j + 1];
				dclc = min(abs(clc - pclc), abs(clc - 65536 - pclc));
				if (dclc > dclcmax)
				{
					//* could be MW, so check distance from P1 and P2
					//*         da : = (ixc - (FIntX[j] + FIntX[j + 1]) / 2.)*(ixc - (FIntX[j] + FIntX[j + 1]) / 2.) + (iyc - (FIntY[j] + FIntY[j + 1]) / 2.)*(iyc - (FIntY[j] + FIntY[j + 1]) / 2.)
					d1 = (xc - FIntX[j])*(xc - FIntX[j]) + (yc - FIntY[j])*(yc - FIntY[j]);
					d2 = (xc - FIntX[j + 1])*(xc - FIntX[j + 1]) + (yc - FIntY[j + 1])*(yc - FIntY[j + 1]);
					d = sqrt(min(d1, d2));
					if (d < dmin)
					{
						mw = j;
						nmwc = nmwc + 1;
						dmin = d;
					}
				}
			}
			if (mw == -1)
				mw = 0;
			//*** finding SP (spur) points
			jSPdn = mw;
			jSPup = mw + 1;
			int clc1, clc2, dclc1, dclc2;
			for (j = 0; j < intnum; j += 2)
			{
				if (j == mw)
					continue;
				else
				{
					clc = PCLC[j];
					clc1 = PCLC[mw];
					clc2 = PCLC[mw + 1];
					dclc1 = min(abs(clc - clc1), abs(clc - 65536 - clc1));
					dclc2 = min(abs(clc - clc2), abs(clc - 65536 - clc2));
					if (dclc1 > dclcmax && dclc2 > dclcmax)
						continue;//* not SP because of big dCLC
					else
					{
						if (j < mw)
						{
							dclc = min(abs(PCLC[mw] - clc1), abs(PCLC[mw] - 65536 - clc1));
							if (dclc > dclcmax)
								continue;//* not SP because of big dCLC from MW point
							else
								jSPdn = j;
						}
						else
						{
							dclc = min(abs(PCLC[mw + 1] - clc), abs(PCLC[mw + 1] - 65536 - clc));
							if (dclc > dclcmax)
								continue; //* not SP because of big dCLC from MW point
							else
								jSPup = j + 1;
						}
					}
				}
			}// for
			//*************************************************
			wmax = sqrt((FIntX[jSPup] - FIntX[jSPdn])*(FIntX[jSPup] - FIntX[jSPdn]) + (FIntY[jSPup] - FIntY[jSPdn])*(FIntY[jSPup] - FIntY[jSPdn]));
			//* wmax : = sqrt((FIntX[intnum - 1] - FIntX[0])*(FIntX[intnum - 1] - FIntX[0]) + (FIntY[intnum - 1] - FIntY[0])*(FIntY[intnum - 1] - FIntY[0]))
			wmin = sqrt((FIntX[mw] - FIntX[mw + 1])*(FIntX[mw] - FIntX[mw + 1]) + (FIntY[mw] - FIntY[mw + 1])*(FIntY[mw] - FIntY[mw + 1]));
			if (wmax > 3.0)
			{
				xmid = (FIntX[jSPup] + FIntX[jSPdn]) / 2.;
				ymid = (FIntY[jSPup] + FIntY[jSPdn]) / 2.;
				shift = sqrt((ixc - xmid)*(ixc - xmid) + (iyc - ymid)*(iyc - ymid));
			}
			
		} // **** normal case end ***

		*(dW + cIdx) = wmax;
		*(dWmin + cIdx) = wmin;
		*(dShift + cIdx) = shift;
		
		*(dPint1x + cIdx) = FIntX[jSPup];
		*(dPint2x + cIdx) = FIntX[jSPdn];

		*(dPint1y + cIdx) = FIntY[jSPup];
		*(dPint2y + cIdx) = FIntY[jSPdn];

		
		
		

		//********************************************	
	} // index cycle end
}

// Find MS intersection using Bresenham's line algorithm for CPU test, based on the HDev procedure MSProc30, 2022.03.22 (CPU version)
void CPU_MSProc30(UINT8 *pIm, UINT8 *pImIRB, UINT16 *pImCLC, int imw, int imh, int pst, int pn, float thr, int dclcmax, float *dP1x, float *dP1y, float *dP2x, float *dP2y, int *dPnum, float *dW, float *dWmin, float *dPint1x, float *dPint1y, float *dPint2x, float *dPint2y)
{
	int x1, y1, x2, y2, xc, yc, pnum;
	int v, vp, vn, v1, v2, i, j;
	float wmin, wmax;
	int PPx[128];  // Bresenham's MS points array
	int PPy[128];
	int Pval[128]; // pnum values array for all MS points

	
	int g, gp; // current and previous gray values
	int jSPdn;
	int jSPup;
	int cIdx; // MS index

	float M1[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int iM1[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int iM0[8] = { 0,0,0,0,0,0,0,0 };
	float PIntX[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. }; // intersection points coordinates
	float PIntY[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	float FIntX[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. }; // intersection points coordinates (subpixel)
	float FIntY[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	float fIntX, fIntY; // subpix coord. measured by 
	int Psubpix[8] = { 0,0,0,0,0,0,0,0 };
	int PInti[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };	  // index array of intersection points 
	int PCLC[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };   // CLC array of intersection points 


	//int cIdx = threadIdx.x + blockIdx.x*blockDim.x;
	//int tid = blockDim.x * blockIdx.x + threadIdx.x;
	for (cIdx = pst; cIdx < pst + pn; cIdx++)
	{
		memcpy(&PIntX[0], &M1[0], 8 * sizeof(float));
		memcpy(&PIntY[0], &M1[0], 8 * sizeof(float));
		memcpy(&FIntX[0], &M1[0], 8 * sizeof(float));
		memcpy(&FIntY[0], &M1[0], 8 * sizeof(float));
		memcpy(&PInti[0], &iM1[0], 8 * sizeof(int));
		memcpy(&PCLC[0], &iM1[0], 8 * sizeof(int));
		memcpy(&Psubpix[0], &iM0[0], 8 * sizeof(int));
		//fw = *(d_P1xl + cIdx);
		//*(d_Wl + cIdx) = fw;
		//if (cIdx == 1883423)
		//if (cIdx == 157421)
		if (cIdx == 324910)
			cIdx = cIdx;
		//int ii = 0;
		
		int ii = 0, clc, pclc = -2, fi1 = 0;
		pnum = (*(dPnum + cIdx));
		x1 = round(*(dP1x + cIdx)); // getting measurement sensor endpoints
		y1 = round(*(dP1y + cIdx));
		x2 = round(*(dP2x + cIdx));
		y2 = round(*(dP2y + cIdx));
		//pnum = (*(dPnum + cIdx)) % 250 + 3;
		int deltaX = abs(x2 - x1);
		int deltaY = abs(y2 - y1);
		int signX = x1 < x2 ? 1 : -1;
		int signY = y1 < y2 ? 1 : -1;
		int error = deltaX - deltaY;
		int error2;
		int intind = 0; // intersection index (0 or 1)
		int pind = 0;   // B's point index [0...63]
		int xintc = x1, yintc = y1; // current and previous intersection points
		//v1 = *(pIm + y1 * imw + x1);  // 1st IRB pixel

		vp = 0;  // previous pixel
		/// filling in Bresenham's array
		while (x1 != x2 || y1 != y2)
		{
			PPx[pind] = x1;
			PPy[pind] = y1;
			v = *(pImIRB + y1 * imw + x1);
			Pval[pind] = v;
			error2 = error * 2;
			if (error2 > -deltaY)
			{
				error -= deltaY;
				x1 += signX;
			}
			if (error2 < deltaX)
			{
				error += deltaX;
				y1 += signY;
			}
			if (pind < 127)
				pind++;
			else
				break;
		} // while
		// Going forward from PS1
//******* checking 1st MS pixel  *****
		if (Pval[0] == 1 || Pval[0] == pnum)
		{
			PIntY[ii] = PPy[0];
			PIntX[ii] = PPx[0];
			PInti[ii] = 0;
			clc = *(pImCLC + y1 * imw + x1);  // CLC pixel value
			//get_grayval(ImageCLC, PPy[0], PPx[0], clc)
			if (Pval[0] == pnum)
				PCLC[ii] = clc;
			else // no clc yet
				PCLC[ii] = -2;
			if(ii<7)
				ii++;
			if (Pval[0] == 1) // 1st pix is 1
				fi1 = 1; //flag of non-confirmed inters. point
			if (Pval[1] == 0 && Pval[0] == pnum)
			{
				PIntY[ii] = PPy[0];
				PIntX[ii] = PPx[0];
				PInti[ii] = 0;
				PCLC[ii] = clc;
				if (ii < 7)ii++;
			}
		}
		else
			fi1 = 0;
		// all MS pixels except 1st and last ******************************
		for (i = 1; i <= pind - 2; i++)
		{
			xc = PPx[i]; yc = PPy[i];
			v = Pval[i];
			if (fi1 == 1)
			{
				if (v == 0 || (v > 1 && v != pnum))
				{
					if(ii>0)
						ii--;
					fi1 = 0;
				}
			}
			if (v == pnum)
			{	//previous and next values
				fi1 = 0; // removing flag of non-confirmed inters. point
				vp = Pval[i - 1];
				vn = Pval[i + 1];
				if ((vp == 0 || (vp > 1 && vp != pnum)) || vn == 0)
				{
					clc = *(pImCLC + yc * imw + xc);  // CLC pixel value
					PIntY[ii] = yc;
					PIntX[ii] = xc;
					PInti[ii] = i;
					Psubpix[ii] = 1;
					if ((vp == 0) && vn == 0) //* single point special case
					{
						if (ii < 7)
							ii = ii + 1;
						PIntY[ii - 1] = yc - 0.35*signY;
						PIntX[ii - 1] = xc - 0.35*signX;
						Psubpix[ii - 1] = 0;
						PIntY[ii] = yc + 0.35*signY;
						PIntX[ii] = xc + 0.35*signX;
						PInti[ii] = i;
						Psubpix[ii] = 0;
					}
					PCLC[ii] = clc;
					if (ii < 7)
						ii = ii + 1;
					pclc = clc;
				}
			}
			vp = v;
			//gp = g;
		}
		//********************************************************************
		int ga[3]; // array of 3 neighboring g
		int nmwc = 0;//*number of MW (main wire) candidates
		int mw = -1;//* MW segment
		int dclc, dmin = 55555; // min CLC distance
		int d, d1, d2; // CLC distances

			//, g1[3];
		//int g2[3], g3[3];
// *checking last MS pixel **********************************************
		if ((Pval[pind - 1] == 1 && ii % 2 == 1) || Pval[pind - 1] == pnum)
		{
			if (Pval[pind - 1] == pnum)
				clc = *(pImCLC + PPy[pind - 1] * imw + PPx[pind - 1]);  // CLC pixel value
				//get_grayval(ImageCLC, PPy[pind - 1], PPx[pind - 1], clc)
			else
				clc = PCLC[ii - 1];

			PIntY[ii] = PPy[pind - 1];
			PIntX[ii] = PPx[pind - 1];
			PInti[ii] = pind - 1;
			PCLC[ii] = clc;
			if (ii < 7)
				ii = ii + 1;
			if (Pval[pind - 2] == 0)
			{
				PIntY[ii] = PPy[pind - 1];
				PIntX[ii] = PPx[pind - 1];
				PInti[ii] = pind - 1;
				PCLC[ii] = clc;
				if (ii < 7)
					ii = ii + 1;
			}
		}
		int intnum = ii; /// number of intersection points found
		if (intnum <= 1) //** no pattern case
		{
			wmin = 0.;
			wmax = 0.;
			jSPdn = 0;
			jSPup = 0;
		}
		else
		{ ///**** NORMAL CASE ******
	//**************** SUBPIX COORD. ******************************
			for (j = 0; j < intnum; j++)
			{
				if (Psubpix[j] == 0)
				{
					FIntX[j] = PIntX[j];
					FIntY[j] = PIntY[j];
				}

				else
				{
					ga[1] = *(pIm + PPy[PInti[j]] * imw + PPx[PInti[j]]);
					//get_grayval(Im, PPy[i2 - 1], PPx[i2 - 1], g)
					ga[0] = *(pIm + PPy[PInti[j] - 1] * imw + PPx[PInti[j] - 1]);
					ga[2] = *(pIm + PPy[PInti[j] + 1] * imw + PPx[PInti[j] + 1]);
					SubPixCrd(ga, PInti[j], PPx, PPy, &fIntX, &fIntY, thr);
					FIntX[j] = fIntX;
					FIntY[j] = fIntY;
				}
			}
			//* ******************* Check Segments ********
				//*** finding MW segment
			for (j = 0; j < intnum; j += 2)
			{
				pclc = PCLC[j];
				clc = PCLC[j + 1];
				dclc = min(abs(clc - pclc), abs(clc - 65536 - pclc));
				if (dclc > dclcmax)
				{
					//* could be MW, so check distance from P1 and P2
					//*         da : = (ixc - (FIntX[j] + FIntX[j + 1]) / 2.)*(ixc - (FIntX[j] + FIntX[j + 1]) / 2.) + (iyc - (FIntY[j] + FIntY[j + 1]) / 2.)*(iyc - (FIntY[j] + FIntY[j + 1]) / 2.)
					d1 = (xc - FIntX[j])*(xc - FIntX[j]) + (yc - FIntY[j])*(yc - FIntY[j]);
					d2 = (xc - FIntX[j + 1])*(xc - FIntX[j + 1]) + (yc - FIntY[j + 1])*(yc - FIntY[j + 1]);
					d = sqrt(min(d1, d2));
					if (d < dmin)
					{
						mw = j;
						nmwc = nmwc + 1;
						dmin = d;
					}
				}
			}
			if (mw == -1)
				mw = 0;
			//*** finding SP (spur) points
			jSPdn = mw;
			jSPup = mw + 1;
			int clc1, clc2, dclc1, dclc2;
			for (j = 0; j < intnum; j += 2)
			{
				if (j == mw)
					continue;
				else
				{
					clc = PCLC[j];
					clc1 = PCLC[mw];
					clc2 = PCLC[mw + 1];
					dclc1 = min(abs(clc - clc1), abs(clc - 65536 - clc1));
					dclc2 = min(abs(clc - clc2), abs(clc - 65536 - clc2));
					if (dclc1 > dclcmax && dclc2 > dclcmax)
						continue;//* not SP because of big dCLC
					else
					{
						if (j < mw)
						{
							dclc = min(abs(PCLC[mw] - clc1), abs(PCLC[mw] - 65536 - clc1));
							if (dclc > dclcmax)
								continue;//* not SP because of big dCLC from MW point
							else
								jSPdn = j;
						}
						else
						{
							dclc = min(abs(PCLC[mw + 1] - clc), abs(PCLC[mw + 1] - 65536 - clc));
							if (dclc > dclcmax)
								continue; //* not SP because of big dCLC from MW point
							else
								jSPup = j + 1;
						}
					}
				}
			}// for
			//*************************************************
			wmax = sqrt((FIntX[jSPup] - FIntX[jSPdn])*(FIntX[jSPup] - FIntX[jSPdn]) + (FIntY[jSPup] - FIntY[jSPdn])*(FIntY[jSPup] - FIntY[jSPdn]));
			//* wmax : = sqrt((FIntX[intnum - 1] - FIntX[0])*(FIntX[intnum - 1] - FIntX[0]) + (FIntY[intnum - 1] - FIntY[0])*(FIntY[intnum - 1] - FIntY[0]))
			wmin = sqrt((FIntX[mw] - FIntX[mw + 1])*(FIntX[mw] - FIntX[mw + 1]) + (FIntY[mw] - FIntY[mw + 1])*(FIntY[mw] - FIntY[mw + 1]));
		} // **** normal case end ***

		*(dW + 1 * cIdx) = wmax;
		*(dWmin + 1 * cIdx) = wmin;

		*(dPint1x + 1 * cIdx + 0) = FIntX[jSPup];
		*(dPint2x + 1 * cIdx + 0) = PIntX[jSPdn];

		*(dPint1y + 1 * cIdx + 0) = PIntY[jSPup];
		*(dPint2y + 1 * cIdx + 0) = PIntY[jSPdn];

		//********************************************	
	} // index cycle end
}






// Find MS intersection using Bresenham's line algorithm for CPU test
void CPU_FWM_Br(UINT8 *pIm, int imw, int imh, int pst, int pn, float *dP1x, float *dP1y, float *dP2x, float *dP2y, int *dPnum, float *dW)
{
	int x1, y1, x2, y2, pnum;
	int v;
	float fw;

	//int cIdx = threadIdx.x + blockIdx.x*blockDim.x;
	//int tid = blockDim.x * blockIdx.x + threadIdx.x;
	for(int cIdx =pst; cIdx<pn; cIdx++)
	{	//fw = *(d_P1xl + cIdx);
		//*(d_Wl + cIdx) = fw;
		if (cIdx == 1000000)
			cIdx = cIdx;
		x1 = round(*(dP1x + cIdx)); // getting measurement sensor endpoints
		y1 = round(*(dP1y + cIdx));
		x2 = round(*(dP2x + cIdx));
		y2 = round(*(dP2y + cIdx));
		pnum = (*(dPnum + cIdx)) % 250 + 3;
		int deltaX = abs(x2 - x1);
		int deltaY = abs(y2 - y1);
		int signX = x1 < x2 ? 1 : -1;
		int signY = y1 < y2 ? 1 : -1;
		int error = deltaX - deltaY;
		int error2;
		int intind = 0;
		int xintc = x1, yintc = y1, xintp, yintp; // current and previous intersection points
		while (x1 != x2 || y1 != y2)
		{
			v = *(pIm + y1 * imw + x1);  // IRB pixel
			//if (v >1 )// should be (v==pnum) when Pnum is fixed
			if  (v==pnum)
			{	xintp = xintc;
				yintp = yintc;
				xintc = x1;
				yintc = y1;
				if (intind > 0)
				{	fw = (float)sqrt((float)(xintc - xintp)*(float)(xintc - xintp) + (float)(yintc - yintp)*(float)(yintc - yintp));// sqrt only for float or double in CUDA
					*(dW + 4*cIdx + intind-1) = fw;
				}
				intind++;
			}
			error2 = error * 2;
			if (error2 > -deltaY)
			{
				error -= deltaY;
				x1 += signX;
			}
			if (error2 < deltaX)
			{
				error += deltaX;
				y1 += signY;
			}
			//if (cnt++ > 50)
				//break;
		}
		/*v = *(pIm + y2 * imw + x2);  // last IRB pixel
		if (v == pn)
		{	if (intind > 0)
			{	xintp = xintc;
				yintp = yintc;
				xintc = x2;
				yintc = y2;
				fw = (float)sqrt((float)(xintc - xintp)*(float)(xintc - xintp) + (float)(yintc - yintp)*(float)(yintc - yintp));
				*(dW + cIdx) = fw;
			}
		}*/
		//cend:
 v = v;
	}
}



// Find width using Bresenham's line algorithm for GPU
__global__ void GPU_FWM_Br(UINT8 *dIm, int imw, int imh, int pn, float *dP1x, float *dP1y, float *dP2x, float *dP2y, int *dPnum, float *dW)
{
	int x1, y1, x2, y2, pnum;
	int v;
	float fw;
	int cIdx = threadIdx.x + blockIdx.x*blockDim.x; // GPU tread index
	if (cIdx < pn - 1)
	{
		if (cIdx == 1000000)
			cIdx = cIdx;
		x1 = round(*(dP1x + cIdx)); // getting measurement sensor endpoints
		y1 = round(*(dP1y + cIdx));
		x2 = round(*(dP2x + cIdx));
		y2 = round(*(dP2y + cIdx));
		pnum = (*(dPnum + cIdx)) % 250 + 3;
		int deltaX = abs(x2 - x1);
		int deltaY = abs(y2 - y1);
		int signX = x1 < x2 ? 1 : -1;
		int signY = y1 < y2 ? 1 : -1;
		int error = deltaX - deltaY;
		int error2;
		int intind = 0;
		int xintc = x1, yintc = y1, xintp, yintp; // current and previous intersection points
		while (x1 != x2 || y1 != y2)
		{
			v = *(dIm + y1 * imw + x1);  // IRB pixel
			if (v > 1) // should be (v==pnum) when Pnum is fixed
			{
				xintp = xintc;
				yintp = yintc;
				xintc = x1;
				yintc = y1;
				if (intind > 0) // sqrt only for float or double in CUDA
				{
					fw = (float)sqrt((float)(xintc - xintp)*(float)(xintc - xintp) + (float)(yintc - yintp)*(float)(yintc - yintp));
					*(dW + 4 * cIdx + intind - 1) = fw;
				}
				intind++;
			}
			error2 = error * 2;
			if (error2 > -deltaY)
			{
				error -= deltaY;
				x1 += signX;
			}
			if (error2 < deltaX)
			{
				error += deltaX;
				y1 += signY;
			}
		}
	}
}

// new version for pattern GPU
__device__ void SubPixCrdPG(int *g0, int i0, int *PPx, int *PPy, float *pIntX, float *pIntY, float thr)
{
	//int PIntX[4], PIntY[4];
	int gmin, gmid, gmax, jmin, jmid, jmax; // points values and numbers
	int Px[3], Py[3];						// 3 conseq. points coordinates
	int dg;
	float dd;

	Px[1] = PPx[i0];
	Px[0] = PPx[i0 - 1];
	Px[2] = PPx[i0 + 1];
	Py[1] = PPy[i0];
	Py[0] = PPy[i0 - 1];
	Py[2] = PPy[i0 + 1];

	jmid = 1;
	gmid = g0[1];

	if (g0[0] > g0[2])
	{
		jmax = 0;
		gmax = g0[0];
		jmin = 2;
		gmin = g0[2];
	}
	else
	{
		jmax = 2;
		gmax = g0[2];
		jmin = 0;
		gmin = g0[0];
	}

	if (gmax < thr)
	{
		*pIntX = Px[jmax];
		*pIntY = Py[jmax];
	}
	else if (gmin > thr)
	{
		*pIntX = Px[jmin];
		*pIntY = Py[jmin];
	}
	//* * normal interpolation case
	else if (gmid >= thr)
	{
		dg = gmid - gmin;
		if (dg != 0)
		{
			dd = (g0[jmid] - thr) / dg;
			*pIntX = Px[jmid] - (Px[jmid] - Px[jmin])*dd;
			*pIntY = Py[jmid] - (Py[jmid] - Py[jmin])*dd;
		}
		else
		{
			*pIntX = (Px[jmid] + Px[jmin]) / 2.;
			*pIntX = (Py[jmid] + Py[jmin]) / 2.;
		}

	}
	else if (gmid < thr)
	{
		dg = gmax - gmid;
		if (dg != 0)
		{
			dd = (g0[jmax] - thr) / dg;
			*pIntX = Px[jmax] - (Px[jmid] - Px[jmax])*dd;
			*pIntY = Py[jmax] - (Py[jmid] - Py[jmax])*dd;
		}
		else
		{
			*pIntX = (Px[jmid] + Px[jmax]) / 2.;
			*pIntY = (Py[jmid] + Py[jmax]) / 2.;
		}
	}
}

// new version for space 20220908 GPU
__device__ void SubPixCrdSG(int *g0, int i0, int *PPx, int *PPy, float *pIntX, float *pIntY, float thr)
{
	//int PIntX[4], PIntY[4];
	int gmin, gmid, gmax, jmin, jmid, jmax; // points values and numbers
	int Px[3], Py[3];						// 3 conseq. points coordinates
	int dg;
	float dd;

	Px[1] = PPx[i0];
	Px[0] = PPx[i0 - 1];
	Px[2] = PPx[i0 + 1];
	Py[1] = PPy[i0];
	Py[0] = PPy[i0 - 1];
	Py[2] = PPy[i0 + 1];

	jmid = 1;
	gmid = g0[1];

	if (g0[0] > g0[2])
	{
		jmax = 0;
		gmax = g0[0];
		jmin = 2;
		gmin = g0[2];
	}
	else
	{
		jmax = 2;
		gmax = g0[2];
		jmin = 0;
		gmin = g0[0];
	}

	if (gmax < thr)
	{
		*pIntX = Px[jmax];
		*pIntY = Py[jmax];
	}
	else if (gmin > thr)
	{
		*pIntX = Px[jmin];
		*pIntY = Py[jmin];
	}
	//* * normal interpolation case
	else if (gmid > thr)
	{
		dg = gmid - gmin;
		if (dg != 0)
		{
			dd = (g0[jmid] - thr) / dg;
			*pIntX = Px[jmid] - (Px[jmid] - Px[jmin])*dd;
			*pIntY = Py[jmid] - (Py[jmid] - Py[jmin])*dd;
		}
		else
		{
			*pIntX = (Px[jmid] + Px[jmin]) / 2.;
			*pIntX = (Py[jmid] + Py[jmin]) / 2.;
		}

	}
	else if (gmid <= thr)
	{
		dg = gmax - gmid;
		if (dg != 0)
		{
			dd = (g0[jmax] - thr) / dg;
			*pIntX = Px[jmax] + (Px[jmid] - Px[jmax])*dd;
			*pIntY = Py[jmax] + (Py[jmid] - Py[jmax])*dd;
		}
		else
		{
			*pIntX = (Px[jmid] + Px[jmax]) / 2.;
			*pIntY = (Py[jmid] + Py[jmax]) / 2.;
		}
	}
}

// modified 20220426 from T&T_LG-LS3 (made same as CPU version)
__device__ void SubPixCrdG(int *g0, int i0, int *PPx, int *PPy, float *pIntX, float *pIntY, float thr)
{
	//int PIntX[4], PIntY[4];
	if (((g0[0] > thr) && (g0[1] > thr) && (g0[2] > thr)))
	{
		if (g0[0] <= g0[1] && g0[0] <= g0[2])
		{
			*pIntX = PPx[i0];
			*pIntY = PPy[i0];
		}
		else if (g0[1] <= g0[0] && g0[1] <= g0[2])
		{
			*pIntX = PPx[i0 + 1];
			*pIntY = PPy[i0 + 1];
		}
		else if (g0[2] <= g0[1] && g0[2] <= g0[0])
		{
			*pIntX = PPx[i0 - 1];
			*pIntY = PPy[i0 - 1];
		}
		//*pIntX = PPx[i0];
		//*pIntY = PPy[i0];
		return;
	}
	if ((g0[0] < thr) && (g0[1] < thr) && (g0[2] < thr))
	{
		if (g0[0] >= g0[1] && g0[0] >= g0[2])
		{
			*pIntX = PPx[i0];
			*pIntY = PPy[i0];
		}
		else if (g0[1] >= g0[0] && g0[1] >= g0[2])
		{
			*pIntX = PPx[i0 + 1];
			*pIntY = PPy[i0 + 1];
		}
		else if (g0[2] >= g0[1] && g0[2] >= g0[0])
		{
			*pIntX = PPx[i0 - 1];
			*pIntY = PPy[i0 - 1];
		}
		//*pIntX = PPx[i0];
		//*pIntY = PPy[i0];
		return;
	}
	if ((g0[1] - g0[0]) > (g0[1] - g0[2]))
	{
		int dg10 = g0[1] - g0[0];
		if (dg10 != 0)
		{
			float dd0 = -(g0[1] - thr) / dg10;
			if (abs(dd0) < 1.)
			{
				*pIntX = PPx[i0] + (PPx[i0] - PPx[i0 - 1])*dd0;
				*pIntY = PPy[i0] + (PPy[i0] - PPy[i0 - 1])*dd0;
			}
			else
			{
				*pIntX = PPx[i0];
				*pIntY = PPy[i0];
			}
		}
		else
		{
			*pIntX = PPx[i0];
			*pIntY = PPy[i0];
		}
	}
	else
	{
		int dg20 = g0[2] - g0[1];
		if (dg20 != 0)
		{
			float dd0 = (g0[1] - thr) / dg20;
			if (abs(dd0) < 1.)
			{
				*pIntX = PPx[i0] + (PPx[i0] - PPx[i0 + 1])*dd0;
				*pIntY = PPy[i0] + (PPy[i0] - PPy[i0 + 1])*dd0;
			}
			else
			{
				*pIntX = PPx[i0];
				*pIntY = PPy[i0];
			}
		}
		else
		{
			*pIntX = PPx[i0];
			*pIntY = PPy[i0];
		}
	}
}

// Find MS intersection using Bresenham's line algorithm for GPU test with added endpoints analysis (Bres. 16)
__global__ void GPU_FWM_Br17(UINT8 *pIm, UINT8 *pImIRB, int imw, int imh, int pst, int pni, float thr, float *dP1x, float *dP1y, float *dP2x, float *dP2y, int *dPnum, float *dW, float *dWmin, float *dPint1x, float *dPint1y, float *dPint2x, float *dPint2y)
{
	int pn = pni + 256;
	int x1, y1, x2, y2, xc, yc, pnum;
	int v, vp, v1, v2, i;
	float wmin, wmax;
	int PPx[128];  // Bresenham's points array
	int PPy[128];
	float PIntX[4] = { 0.,0.,0.,0. };
	float PIntY[4];
	int Pval[128];

	int pi1x = 0, pi1y = 0, pi2x = 0, pi2y = 0;
	//int cIdx;

	//int cIdx = threadIdx.x + blockIdx.x*blockDim.x;
	//int tid = blockDim.x * blockIdx.x + threadIdx.x;
	//for (cIdx = pst; cIdx < pst + pn; cIdx++)
	int cIdx = pst + threadIdx.x + blockIdx.x*blockDim.x; // GPU tread index
	if (cIdx < pst + pn - 1)
	{	//fw = *(d_P1xl + cIdx);
		//*(d_Wl + cIdx) = fw;
		//if (cIdx == 1883423)
		//if (cIdx == 157421)
			//if (cIdx == 5037)
			//cIdx = cIdx;
		if (cIdx == 2950038)
			cIdx = cIdx;
		pnum = (*(dPnum + cIdx));
		x1 = round(*(dP1x + cIdx)); // getting measurement sensor endpoints
		y1 = round(*(dP1y + cIdx));
		x2 = round(*(dP2x + cIdx));
		y2 = round(*(dP2y + cIdx));
		//pnum = (*(dPnum + cIdx)) % 250 + 3;
		int deltaX = abs(x2 - x1);
		int deltaY = abs(y2 - y1);
		int signX = x1 < x2 ? 1 : -1;
		int signY = y1 < y2 ? 1 : -1;
		int error = deltaX - deltaY;
		int error2;
		int intind = 0; // intersection index (0 or 1)
		int pind = 0;   // B's point index [0...63]
		int xintc = x1, yintc = y1, xintp, yintp; // current and previous intersection points
		//v1 = *(pIm + y1 * imw + x1);  // 1st IRB pixel

		vp = 0;  // previous pixel
		/// filling in Bresenham's array
		while (x1 != x2 || y1 != y2)
		{
			PPx[pind] = x1;
			PPy[pind] = y1;
			v = *(pImIRB + y1 * imw + x1);
			Pval[pind] = v;
			error2 = error * 2;
			if (error2 > -deltaY)
			{
				error -= deltaY;
				x1 += signX;
			}
			if (error2 < deltaX)
			{
				error += deltaX;
				y1 += signY;
			}
			if (pind < 127)
				pind++;
			else
				break;
		} // while
		int np = 0;
		// Going forward from PS1
		int i0 = -1;
		int i2 = -1;
		int bfound = 0;
		vp = Pval[1];  // previous pixel
		for (i = 1; i < pind - 1; i++)
		{
			xc = PPx[i]; yc = PPy[i];
			v = Pval[i];
			if (v == pnum)
			{
				if (bfound == 0)
				{  // real def. beginning
					PIntX[0] = xc; PIntY[0] = yc;
					i0 = i;
					bfound = 1;
					/*if (np == 0)
						np = 1;*/
				}
				else if (bfound == 2)
				{
					bfound = 1;
				}
			}
			else if (v == 1)
			{	// wire body
				if (bfound == 0)
				{	// preliminary def. beginning
					PIntX[0] = xc; PIntY[0] = yc;
					i0 = i;
					bfound = 2;
				}
			}
			else if (v > 1)
			{	// possibly neighbouring wire
				if ((bfound == 0) && (i < pind / 2 + 4))
				{
					PIntX[0] = xc; PIntY[0] = yc;
					i0 = i;

				}
				bfound = 3;
			}
			else if (v == 0)
			{ // space pixel
				if (bfound == 1)
				{	// end of real def. beginning search
					PIntX[2] = PPx[i - 1]; PIntY[2] = PPy[i - 1];
					i2 = i - 1;
					np = 4;
					/*if (np == 1)
						np = 2;*/
					break;
				}
				else if (vp == 0)
					bfound = 0;
			}
			vp = v;
		}
		// Going backward from PS2
		int i1 = -1;
		int i3 = -1;
		int bfound2 = 0;
		vp = Pval[pind - 1];
		for (i = pind - 2; i >= 0; i--)
		{
			xc = PPx[i]; yc = PPy[i];
			v = Pval[i];
			if (v == pnum)
			{
				if (bfound2 == 0)
				{  // real def. beginning
					PIntX[1] = xc; PIntY[1] = yc;
					i1 = i;
					bfound2 = 1;
					/*if (np == 1)
						np = 2;
					else if (np == 2)
						np = 3;*/
				}
				else if (bfound2 == 2)
				{
					bfound2 = 1;
				}
			}
			else if (v == 1)
			{
				if (bfound2 == 0)
				{	// preliminary def. beginning
					PIntX[1] = xc; PIntY[1] = yc;
					i1 = i;
					bfound2 = 2;
				}
			}
			else if (v > 1)
			{	// possibly neighbouring wire
				if ((bfound2 == 0) && (i > pind / 2 - 4))
				{
					PIntX[0] = xc; PIntY[0] = yc;
					i1 = i;
				}
				bfound2 = 3;
			}
			else if (v == 0)
			{ // space pixel
				if (bfound2 == 1)
				{	// end of real def. beginning search
					PIntX[3] = PPx[i + 1]; PIntY[3] = PPy[i + 1];
					i3 = i + 1;
					np = 4;
					break;
					/*if (np == 2)
						np = 3;
					else if (np == 3)
						np = 4;*/
				}
				else
					bfound2 = 0;
			}
			vp = v;
		}
		int g0[3], g1[3];
		int g2[3], g3[3];

		//if ((i0 == -1) || (i1 == -1) || (i2 == -1) || (i3 == -1))
		if ((i0 == -1) || (i1 == -1))
		{
			wmin = wmax = 0.0;
			//return;
		}
		else
		{
			int ppi00x = PPx[i0];
			int ppi00y = PPy[i0];
			int ppi0p1x = PPx[i0 + 1];
			int ppi0p1y = PPy[i0 + 1];
			int ppi0m1x = PPx[i0 - 1];
			int ppi0m1y = PPy[i0 - 1];

			int ppi10x = PPx[i1];
			int ppi10y = PPy[i1];
			int ppi1p1x = PPx[i1 + 1];
			int ppi1p1y = PPy[i1 + 1];
			int ppi1m1x = PPx[i1 - 1];
			int ppi1m1y = PPy[i1 - 1];


			g0[1] = *(pIm + ppi00y * imw + ppi00x);
			g0[0] = *(pIm + ppi0m1y * imw + ppi0m1x);
			g0[2] = *(pIm + ppi0p1y * imw + ppi0p1x);
			g1[1] = *(pIm + ppi10y * imw + ppi10x);
			g1[0] = *(pIm + ppi1m1y * imw + ppi1m1x);
			g1[2] = *(pIm + ppi1p1y * imw + ppi1p1x);

			float px, py;
			SubPixCrdG(&g0[0], i0, &PPx[0], &PPy[0], &px, &py, thr);
			PIntX[0] = px;
			PIntY[0] = py;
			SubPixCrdG(&g1[0], i1, &PPx[0], &PPy[0], &px, &py, thr);
			PIntX[1] = px;
			PIntY[1] = py;

		}
		int twolines = 0;

		if (i2 > 0 && i2 != i1 && np > 2)
		{
			//get_grayval(Im, PPy[i2], PPx[i2], g)
			g2[1] = *(pIm + PPy[i2] * imw + PPx[i2]);
			//get_grayval(Im, PPy[i2 - 1], PPx[i2 - 1], g)
			g2[0] = *(pIm + PPy[i2 - 1] * imw + PPx[i2 - 1]);
			g2[2] = *(pIm + PPy[i2 + 1] * imw + PPx[i2 + 1]);

			float px, py;
			SubPixCrdG(g2, i2, PPx, PPy, &px, &py, thr);
			PIntX[2] = px;
			PIntY[2] = py;
			twolines = 1;
		}
		if (i3 > 0 && i3 != i0 && np > 2)
		{
			//get_grayval(Im, PPy[i2], PPx[i2], g)
			g3[1] = *(pIm + PPy[i3] * imw + PPx[i3]);
			//get_grayval(Im, PPy[i2 - 1], PPx[i2 - 1], g)
			g3[0] = *(pIm + PPy[i3 - 1] * imw + PPx[i3 - 1]);
			g3[2] = *(pIm + PPy[i3 + 1] * imw + PPx[i3 + 1]);

			float px, py;
			SubPixCrdG(g3, i3, PPx, PPy, &px, &py, thr);
			PIntX[3] = px;
			PIntY[3] = py;
			twolines = 1;
		}
		wmin = 0.;
		if (np > 1)
		{
			if (PIntX[0] != 0 && PIntX[1] != 0 && (twolines == 0))
			{
				wmax = sqrt((float)(PIntX[0] - PIntX[1])*(float)(PIntX[0] - PIntX[1]) + (float)(PIntY[0] - PIntY[1])*(float)(PIntY[0] - PIntY[1]));
				wmin = wmax;
			}
			if (twolines == 1)
			{
				wmax = sqrt((PIntX[0] - PIntX[1])*(PIntX[0] - PIntX[1]) + (PIntY[0] - PIntY[1])*(PIntY[0] - PIntY[1]));
				// selecting line which includes skeleton
				xc = (x1 + x2) / 2;
				yc = (y1 + y2) / 2;
				int dX = x2 - x1;
				int dY = y2 - y1;
				if (dX != 0)
				{
					if (xc <= PIntX[0] && xc >= PIntX[2] || xc >= PIntX[0] && xc <= PIntX[2])
						wmin = sqrt((PIntX[0] - PIntX[2])*(PIntX[0] - PIntX[2]) + (PIntY[0] - PIntY[2])*(PIntY[0] - PIntY[2]));
					else if (xc <= PIntX[1] && xc >= PIntX[3] || xc >= PIntX[1] && xc <= PIntX[3])
						wmin = sqrt((PIntX[1] - PIntX[3])*(PIntX[1] - PIntX[3]) + (PIntY[1] - PIntY[3])*(PIntY[1] - PIntY[3]));
					else
						wmin = -1.;
				}
				else
				{
					if (yc <= PIntY[0] && yc >= PIntY[2] || yc >= PIntY[0] && yc <= PIntY[2])
						wmin = sqrt((PIntX[0] - PIntX[2])*(PIntX[0] - PIntX[2]) + (PIntY[0] - PIntY[2])*(PIntY[0] - PIntY[2]));
					else if (xc <= PIntX[1] && xc >= PIntX[3] || xc >= PIntX[1] && xc <= PIntX[3])
						wmin = sqrt((PIntX[1] - PIntX[3])*(PIntX[1] - PIntX[3]) + (PIntY[1] - PIntY[3])*(PIntY[1] - PIntY[3]));

					else
						wmin = -1;
				}
			}
		}
		else
		{
			wmax = 0.;
			wmin = 0.;
		}

		*(dW + 1 * cIdx) = wmax;
		*(dWmin + 1 * cIdx) = wmin;

		*(dPint1x + 1 * cIdx + 0) = PIntX[0];
		*(dPint2x + 1 * cIdx + 0) = PIntX[1];

		*(dPint1y + 1 * cIdx + 0) = PIntY[0];
		*(dPint2y + 1 * cIdx + 0) = PIntY[1];
	}
	//return;
///////////////////////////////////////////////

	cIdx = cIdx;
}


// Find MS intersection using Bresenham's line algorithm for GPU test with added endpoints analysis (Bres. 16)
__global__ void GPU_FWM_Br16(UINT8 *pIm, UINT8 *pImIRB, int imw, int imh, int pst, int pni, float thr, float *dP1x, float *dP1y, float *dP2x, float *dP2y, int *dPnum, float *dW, float *dWmin, UINT64 *dPint1x, UINT64 *dPint1y, UINT64 *dPint2x, UINT64 *dPint2y)
{
	int pn = pni + 256;
	int x1, y1, x2, y2, xc, yc, pnum;
	int v, vp, v1, v2, i;
	float wmin, wmax;
	int PPx[128];  // Bresenham's points array
	int PPy[128];
	float PIntX[4] = { 0.,0.,0.,0. };
	float PIntY[4];
	int Pval[128];

	int pi1x = 0, pi1y = 0, pi2x = 0, pi2y = 0;
	//int cIdx;

	//int cIdx = threadIdx.x + blockIdx.x*blockDim.x;
	//int tid = blockDim.x * blockIdx.x + threadIdx.x;
	//for (cIdx = pst; cIdx < pst + pn; cIdx++)
	int cIdx = pst + threadIdx.x + blockIdx.x*blockDim.x; // GPU tread index
	if (cIdx < pst + pn - 1)
	{	//fw = *(d_P1xl + cIdx);
		//*(d_Wl + cIdx) = fw;
		//if (cIdx == 1883423)
		//if (cIdx == 157421)
			//if (cIdx == 5037)
			//cIdx = cIdx;
		if (cIdx == 2950038)
			cIdx = cIdx;
		pnum = (*(dPnum + cIdx));
		x1 = round(*(dP1x + cIdx)); // getting measurement sensor endpoints
		y1 = round(*(dP1y + cIdx));
		x2 = round(*(dP2x + cIdx));
		y2 = round(*(dP2y + cIdx));
		//pnum = (*(dPnum + cIdx)) % 250 + 3;
		int deltaX = abs(x2 - x1);
		int deltaY = abs(y2 - y1);
		int signX = x1 < x2 ? 1 : -1;
		int signY = y1 < y2 ? 1 : -1;
		int error = deltaX - deltaY;
		int error2;
		int intind = 0; // intersection index (0 or 1)
		int pind = 0;   // B's point index [0...63]
		int xintc = x1, yintc = y1, xintp, yintp; // current and previous intersection points
		//v1 = *(pIm + y1 * imw + x1);  // 1st IRB pixel

		vp = 0;  // previous pixel
		/// filling in Bresenham's array
		while (x1 != x2 || y1 != y2)
		{
			PPx[pind] = x1;
			PPy[pind] = y1;
			v = *(pImIRB + y1 * imw + x1);
			Pval[pind] = v;
			error2 = error * 2;
			if (error2 > -deltaY)
			{
				error -= deltaY;
				x1 += signX;
			}
			if (error2 < deltaX)
			{
				error += deltaX;
				y1 += signY;
			}
			if (pind < 127)
				pind++;
			else
				break;
		} // while
		int np = 0;
		// Going forward from PS1
		int i0 = -1;
		int i2 = -1;
		int bfound = 0;
		vp = Pval[1];  // previous pixel
		for (i = 1; i < pind - 1; i++)
		{
			xc = PPx[i]; yc = PPy[i];
			v = Pval[i];
			if (v == pnum)
			{
				if (bfound == 0)
				{  // real def. beginning
					PIntX[0] = xc; PIntY[0] = yc;
					i0 = i;
					bfound = 1;
					/*if (np == 0)
						np = 1;*/
				}
				else if (bfound == 2)
				{
					bfound = 1;
				}
			}
			else if (v == 1)
			{	// wire body
				if (bfound == 0)
				{	// preliminary def. beginning
					PIntX[0] = xc; PIntY[0] = yc;
					i0 = i;
					bfound = 2;
				}
			}
			else if (v > 1)
			{	// possibly neighbouring wire
				if ((bfound == 0) && (i < pind / 2 + 4))
				{
					PIntX[0] = xc; PIntY[0] = yc;
					i0 = i;

				}
				bfound = 3;
			}
			else if (v == 0)
			{ // space pixel
				if (bfound == 1)
				{	// end of real def. beginning search
					PIntX[2] = PPx[i - 1]; PIntY[2] = PPy[i - 1];
					i2 = i - 1;
					np = 4;
					/*if (np == 1)
						np = 2;*/
					break;
				}
				else if (vp == 0)
					bfound = 0;
			}
			vp = v;
		}
		// Going backward from PS2
		int i1 = -1;
		int i3 = -1;
		int bfound2 = 0;
		vp = Pval[pind - 1];
		for (i = pind - 2; i >= 0; i--)
		{
			xc = PPx[i]; yc = PPy[i];
			v = Pval[i];
			if (v == pnum)
			{
				if (bfound2 == 0)
				{  // real def. beginning
					PIntX[1] = xc; PIntY[1] = yc;
					i1 = i;
					bfound2 = 1;
					/*if (np == 1)
						np = 2;
					else if (np == 2)
						np = 3;*/
				}
				else if (bfound2 == 2)
				{
					bfound2 = 1;
				}
			}
			else if (v == 1)
			{
				if (bfound2 == 0)
				{	// preliminary def. beginning
					PIntX[1] = xc; PIntY[1] = yc;
					i1 = i;
					bfound2 = 2;
				}
			}
			else if (v > 1)
			{	// possibly neighbouring wire
				if ((bfound2 == 0) && (i > pind / 2 - 4))
				{
					PIntX[0] = xc; PIntY[0] = yc;
					i1 = i;
				}
				bfound2 = 3;
			}
			else if (v == 0)
			{ // space pixel
				if (bfound2 == 1)
				{	// end of real def. beginning search
					PIntX[3] = PPx[i + 1]; PIntY[3] = PPy[i + 1];
					i3 = i + 1;
					np = 4;
					break;
					/*if (np == 2)
						np = 3;
					else if (np == 3)
						np = 4;*/
				}
				else
					bfound2 = 0;
			}
			vp = v;
		}
		int g0[3], g1[3];
		int g2[3], g3[3];

		//if ((i0 == -1) || (i1 == -1) || (i2 == -1) || (i3 == -1))
		if ((i0 == -1) || (i1 == -1))
		{
			wmin = wmax = 0.0;
			//return;
		}
		else
		{
			int ppi00x = PPx[i0];
			int ppi00y = PPy[i0];
			int ppi0p1x = PPx[i0 + 1];
			int ppi0p1y = PPy[i0 + 1];
			int ppi0m1x = PPx[i0 - 1];
			int ppi0m1y = PPy[i0 - 1];

			int ppi10x = PPx[i1];
			int ppi10y = PPy[i1];
			int ppi1p1x = PPx[i1 + 1];
			int ppi1p1y = PPy[i1 + 1];
			int ppi1m1x = PPx[i1 - 1];
			int ppi1m1y = PPy[i1 - 1];


			g0[1] = *(pIm + ppi00y * imw + ppi00x);
			g0[0] = *(pIm + ppi0m1y * imw + ppi0m1x);
			g0[2] = *(pIm + ppi0p1y * imw + ppi0p1x);
			g1[1] = *(pIm + ppi10y * imw + ppi10x);
			g1[0] = *(pIm + ppi1m1y * imw + ppi1m1x);
			g1[2] = *(pIm + ppi1p1y * imw + ppi1p1x);

			float px, py;
			SubPixCrdG(&g0[0], i0, &PPx[0], &PPy[0], &px, &py, thr);
			PIntX[0] = px;
			PIntY[0] = py;
			SubPixCrdG(&g1[0], i1, &PPx[0], &PPy[0], &px, &py, thr);
			PIntX[1] = px;
			PIntY[1] = py;

		}
		int twolines = 0;

		if (i2 > 0 && i2 != i1 && np > 2)
		{
			//get_grayval(Im, PPy[i2], PPx[i2], g)
			g2[1] = *(pIm + PPy[i2] * imw + PPx[i2]);
			//get_grayval(Im, PPy[i2 - 1], PPx[i2 - 1], g)
			g2[0] = *(pIm + PPy[i2 - 1] * imw + PPx[i2 - 1]);
			g2[2] = *(pIm + PPy[i2 + 1] * imw + PPx[i2 + 1]);

			float px, py;
			SubPixCrdG(g2, i2, PPx, PPy, &px, &py, thr);
			PIntX[2] = px;
			PIntY[2] = py;
			twolines = 1;
		}
		if (i3 > 0 && i3 != i0 && np > 2)
		{
			//get_grayval(Im, PPy[i2], PPx[i2], g)
			g3[1] = *(pIm + PPy[i3] * imw + PPx[i3]);
			//get_grayval(Im, PPy[i2 - 1], PPx[i2 - 1], g)
			g3[0] = *(pIm + PPy[i3 - 1] * imw + PPx[i3 - 1]);
			g3[2] = *(pIm + PPy[i3 + 1] * imw + PPx[i3 + 1]);

			float px, py;
			SubPixCrdG(g3, i3, PPx, PPy, &px, &py, thr);
			PIntX[3] = px;
			PIntY[3] = py;
			twolines = 1;
		}
		wmin = 0.;
		if (np > 1)
		{
			if (PIntX[0] != 0 && PIntX[1] != 0 && (twolines == 0))
			{
				wmax = sqrt((float)(PIntX[0] - PIntX[1])*(float)(PIntX[0] - PIntX[1]) + (float)(PIntY[0] - PIntY[1])*(float)(PIntY[0] - PIntY[1]));
				wmin = wmax;
			}
			if (twolines == 1)
			{
				wmax = sqrt((PIntX[0] - PIntX[1])*(PIntX[0] - PIntX[1]) + (PIntY[0] - PIntY[1])*(PIntY[0] - PIntY[1]));
				// selecting line which includes skeleton
				xc = (x1 + x2) / 2;
				yc = (y1 + y2) / 2;
				int dX = x2 - x1;
				int dY = y2 - y1;
				if (dX != 0)
				{
					if (xc <= PIntX[0] && xc >= PIntX[2] || xc >= PIntX[0] && xc <= PIntX[2])
						wmin = sqrt((PIntX[0] - PIntX[2])*(PIntX[0] - PIntX[2]) + (PIntY[0] - PIntY[2])*(PIntY[0] - PIntY[2]));
					else if (xc <= PIntX[1] && xc >= PIntX[3] || xc >= PIntX[1] && xc <= PIntX[3])
						wmin = sqrt((PIntX[1] - PIntX[3])*(PIntX[1] - PIntX[3]) + (PIntY[1] - PIntY[3])*(PIntY[1] - PIntY[3]));
					else
						wmin = -1.;
				}
				else
				{
					if (yc <= PIntY[0] && yc >= PIntY[2] || yc >= PIntY[0] && yc <= PIntY[2])
						wmin = sqrt((PIntX[0] - PIntX[2])*(PIntX[0] - PIntX[2]) + (PIntY[0] - PIntY[2])*(PIntY[0] - PIntY[2]));
					else if (xc <= PIntX[1] && xc >= PIntX[3] || xc >= PIntX[1] && xc <= PIntX[3])
						wmin = sqrt((PIntX[1] - PIntX[3])*(PIntX[1] - PIntX[3]) + (PIntY[1] - PIntY[3])*(PIntY[1] - PIntY[3]));

					else
						wmin = -1;
				}
			}
		}
		else
		{
			wmax = 0.;
			wmin = 0.;
		}

		*(dW + 1 * cIdx) = wmax;
		*(dWmin + 1 * cIdx) = wmin;

		*(dPint1x + 1 * cIdx + 0) = PIntX[0];
		*(dPint2x + 1 * cIdx + 0) = PIntX[1];

		*(dPint1y + 1 * cIdx + 0) = PIntY[0];
		*(dPint2y + 1 * cIdx + 0) = PIntY[1];
	}
	//return;
///////////////////////////////////////////////

		/*if ((pi1x != 0) && (pi2x != 0))
			fw = (float)sqrt((pi1x - pi2x)*(pi1x - pi2x) + (pi1y - pi2y)*(pi1y - pi2y));
		else
			fw = 0.;
		*(dW + 1 * cIdx) = fw;*/
		//} // for
	cIdx = cIdx;
}

// new version for Pattern 221110 -v.36 GPU
__global__ void GPU_MSPtProc36(UINT8 *pIm, UINT8 *pImIRB, UINT8 *pImIRBS, UINT16 *pImCLC, int imw, int imh, int pst, int pn, float thr, int dclcmax, int pat0space1, float *dP1x, float *dP1y, float *dP2x, float *dP2y, int *dPnum, float *dW, float *dWmin, float *dPint1x, float *dPint1y, float *dPint2x, float *dPint2y, float *dShift, int *dPuseint)
{
	int x1, y1, x2, y2, xc, yc, pnum, useint;
	int v, vp, vn, v1, v2, i, j;
	float wmin, wmax;		 // for width calculation
	float shift = 0.0, xmid, ymid; // for shift calculation
	int PPx[128];  // Bresenham's MS points array
	int PPy[128];
	int Pval[128]; // pnum values array for all MS points


	int g, gp; // current and previous gray values
	int jSPdn;
	int jSPup;
	int cIdx; // MS index

	float M1[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int iM1[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int iM0[8] = { 0,0,0,0,0,0,0,0 };
	float PIntX[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. }; // intersection points coordinates
	float PIntY[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	float FIntX[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. }; // intersection points coordinates (subpixel)
	float FIntY[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	float fIntX, fIntY; // subpix coord. measured by 
	int Psubpix[8] = { 0,0,0,0,0,0,0,0 };
	int PInti[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };	  // index array of intersection points 
	int PCLC[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };   // CLC array of intersection points 
	int PType[8] = { 0,0,0,0,0,0,0,0 };
	int Pnums[8] = { 0,0,0,0,0,0,0,0 };
	int vpnums;
	//** Intersection point IRBS val,
	int	PIRBS[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int irbs;
	float ixc;
	float iyc;
	int ptype = 0, begtype = 0, pnfound = 0, btype;
	int PB[8];
	int PE[8];
	int ib = 0;
	int ie = 0;
	//int cIdx = threadIdx.x + blockIdx.x*blockDim.x;
	//int tid = blockDim.x * blockIdx.x + threadIdx.x;
	cIdx = pst + threadIdx.x + blockIdx.x*blockDim.x; // GPU tread index
	if (cIdx < pst + pn - 1)
	{
		ib = 0;
		ie = 0;
		/*if (cIdx == 98200)
			cIdx = cIdx;*/
		memcpy(&PIntX[0], &M1[0], 8 * sizeof(float));
		memcpy(&PIntY[0], &M1[0], 8 * sizeof(float));
		memcpy(&FIntX[0], &M1[0], 8 * sizeof(float));
		memcpy(&FIntY[0], &M1[0], 8 * sizeof(float));
		memcpy(&PInti[0], &iM1[0], 8 * sizeof(int));
		memcpy(&PCLC[0], &iM1[0], 8 * sizeof(int));
		memcpy(&Psubpix[0], &iM0[0], 8 * sizeof(int));
		memcpy(&PType[0], &iM0[0], 8 * sizeof(int));
		//fw = *(d_P1xl + cIdx);
		//*(d_Wl + cIdx) = fw;
		//if (cIdx == 1883423)
		//if (cIdx == 157421)
		/*if (cIdx == 162006)
			cIdx = cIdx;*/
			//int ii = 0;

		int ii = 0, clc, pclc = -2, fi1 = 0;
		pnum = (*(dPnum + cIdx));
		//useint = (*(dPuseint + cIdx));
		x1 = round(*(dP1x + cIdx)); // getting measurement sensor endpoints
		y1 = round(*(dP1y + cIdx));
		x2 = round(*(dP2x + cIdx));
		y2 = round(*(dP2y + cIdx));
		ixc = (x1 + x2) / 2.;
		iyc = (y1 + y2) / 2.;
		//pnum = (*(dPnum + cIdx)) % 250 + 3;
		int deltaX = abs(x2 - x1);
		int deltaY = abs(y2 - y1);
		int signX = x1 < x2 ? 1 : -1;
		int signY = y1 < y2 ? 1 : -1;
		int error = deltaX - deltaY;
		int error2;
		int intind = 0; // intersection index (0 or 1)
		int pind = 0;   // B's point index [0...63]
		int xintc = x1, yintc = y1; // current and previous intersection points
		//v1 = *(pIm + y1 * imw + x1);  // 1st IRB pixel


		vp = 0;  // previous pixel
		/// filling in Bresenham's array
		while (x1 != x2 || y1 != y2)
		{
			PPx[pind] = x1;
			PPy[pind] = y1;
			v = *(pImIRB + y1 * imw + x1);
			Pval[pind] = v;
			error2 = error * 2;
			if (error2 > -deltaY)
			{
				error -= deltaY;
				x1 += signX;
			}
			if (error2 < deltaX)
			{
				error += deltaX;
				y1 += signY;
			}
			if (pind < 127)
				pind++;
			else
				break;
		} // while
		// Going forward from PS1

		ptype = begtype = pnfound = 0;
		vp = Pval[0];
		for (i = 1; i <= pind - 2; i++)
		{
			xc = PPx[i]; yc = PPy[i];
			v = Pval[i];
			vn = Pval[i + 1];
			irbs = *(pImIRBS + yc * imw + xc);  // IRBSC pixel value
			if (i == 1 && v == 1)
			{
				ptype = 1;
				PIntY[ii] = yc;
				PIntX[ii] = xc;
				PInti[ii] = i;
				PCLC[ii] = -2;
				begtype = 1;
				ii = ii + 1;
			}
			if (v == 0)
			{
				begtype = 0;
				pnfound = 0;
			}
			if (v > 1 && v != pnum && begtype == 1)
			{
				begtype = 0;
				pnfound = 0;
				ii = ii - 1;
			}
			btype = 0;
			if (v == pnum)
			{
				if (vp == 0 && (vn == pnum || vn == 1))
					btype = 1;
				else if ((vp == 1 && vp != pnum) || (vn == 0 && vp != 0))

					btype = -1;
				else
				{
					btype = 0;
					continue;
				}
				PType[ii] = btype;
				pnfound = 1;
				PIntY[ii] = yc;
				PIntX[ii] = xc;
				PInti[ii] = i;
				Psubpix[ii] = 1;
				if (ii < 7)
					ii = ii + 1;
			}
			if (i == pind - 1 && v == 1 && pnfound == 1)
			{
				PIntY[ii] = yc;
				PIntX[ii] = xc;
				PInti[ii] = i;
				PCLC[ii] = -2;
				ii = ii + 1;
			}
			vp = v;
		}
		//********************************************************************
		int ga[3]; // array of 3 neighboring g
		int nmwc = 0;//*number of MW (main wire) candidates
		int mw = -1;//* MW segment
		int dclc, dmin = 55555; // min CLC distance
		int d, d1, d2; // CLC distances
		int intnum = ii; /// number of intersection points found
		jSPdn = 0;

		//if (ii < 7)
		//{
		intnum = ii;
		jSPdn = 0;
		jSPup = ii - 1;
		//}
		/*else
		{
			intnum = 7;
			jSPup = 7;
		}*/

		if (intnum <= 1) //** single pix. space case
		{
			wmin = -1.;
			wmax = -1.;
			jSPdn = 0;
			jSPup = 0;
		}
		else
		{ ///**** NORMAL CASE ******
	//**************** SUBPIX COORD. ******************************
			for (j = 0; j < ii; j++)
			{
				if (PType[j] == 1)
					PB[ib++] = j;
				//ib : = ib + 1
				else if (PType[j] == -1)
					PE[ie++] = j;
				//ie : = ie + 1

			}
			jSPdn = PB[0];
			jSPup = PE[ie - 1];

			for (j = 0; j < intnum; j++)
			{
				ga[1] = *(pIm + PPy[PInti[j]] * imw + PPx[PInti[j]]);
				if (PInti[j] > 0)
					ga[0] = *(pIm + PPy[PInti[j] - 1] * imw + PPx[PInti[j] - 1]);
				else
					ga[0] = ga[1];
				if (PInti[j] < pind - 1)
					ga[2] = *(pIm + PPy[PInti[j] + 1] * imw + PPx[PInti[j] + 1]);
				else
					ga[2] = ga[1];
				if (PInti[j] < pind - 1 && PInti[j]>0)
				{
					SubPixCrdPG(ga, PInti[j], PPx, PPy, &fIntX, &fIntY, thr);
				}
				else
				{
					fIntX = PPx[PInti[j]];
					fIntY = PPy[PInti[j]];
				}
				FIntX[j] = fIntX;
				FIntY[j] = fIntY;
			}

			//*************************************************
			wmax = sqrt((FIntX[jSPup] - FIntX[jSPdn])*(FIntX[jSPup] - FIntX[jSPdn]) + (FIntY[jSPup] - FIntY[jSPdn])*(FIntY[jSPup] - FIntY[jSPdn]));
			//* wmax : = sqrt((FIntX[intnum - 1] - FIntX[0])*(FIntX[intnum - 1] - FIntX[0]) + (FIntY[intnum - 1] - FIntY[0])*(FIntY[intnum - 1] - FIntY[0]))
			//wmin = sqrt((FIntX[mw] - FIntX[mw + 1])*(FIntX[mw] - FIntX[mw + 1]) + (FIntY[mw] - FIntY[mw + 1])*(FIntY[mw] - FIntY[mw + 1]));
			wmin = wmax;
			if (wmax > 3.0)
			{ // calculating shift
				xmid = (FIntX[jSPup] + FIntX[jSPdn]) / 2.;
				ymid = (FIntY[jSPup] + FIntY[jSPdn]) / 2.;
				shift = sqrt((ixc - xmid)*(ixc - xmid) + (iyc - ymid)*(iyc - ymid));
			}

			//} // **** normal case end ***


		}
		*(dW + cIdx) = wmax;
		*(dWmin + cIdx) = wmin;
		*(dShift + cIdx) = shift;

		*(dPint1x + cIdx) = PIntX[jSPup];
		*(dPint2x + cIdx) = PIntX[jSPdn];

		*(dPint1y + cIdx) = PIntY[jSPup];
		*(dPint2y + cIdx) = PIntY[jSPdn];

		/// saving number of intersection points ( 2 points) (left name Puseint for simplicity of changes)
		*(dPuseint + cIdx) = intnum;
		//********************************************	
	} // index cycle end
}

// new version for Pattern 221107(GPU)
__global__ void GPU_MSPtProc35(UINT8 *pIm, UINT8 *pImIRB, UINT8 *pImIRBS, UINT16 *pImCLC, int imw, int imh, int pst, int pn, float thr, int dclcmax, int pat0space1, float *dP1x, float *dP1y, float *dP2x, float *dP2y, int *dPnum, float *dW, float *dWmin, float *dPint1x, float *dPint1y, float *dPint2x, float *dPint2y, float *dShift, int *dPuseint)
{
	int x1, y1, x2, y2, xc, yc, pnum, useint;
	int v, vp, vn, v1, v2, i, j;
	float wmin, wmax;		 // for width calculation
	float shift = 0.0, xmid, ymid; // for shift calculation
	int PPx[128];  // Bresenham's MS points array
	int PPy[128];
	int Pval[128]; // pnum values array for all MS points


	int g, gp; // current and previous gray values
	int jSPdn;
	int jSPup;
	int cIdx; // MS index

	float M1[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int iM1[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int iM0[8] = { 0,0,0,0,0,0,0,0 };
	float PIntX[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. }; // intersection points coordinates
	float PIntY[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	float FIntX[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. }; // intersection points coordinates (subpixel)
	float FIntY[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	float fIntX, fIntY; // subpix coord. measured by 
	int Psubpix[8] = { 0,0,0,0,0,0,0,0 };
	int PInti[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };	  // index array of intersection points 
	int PCLC[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };   // CLC array of intersection points 
	int Pdir[8] = { 0,0,0,0,0,0,0,0 };
	int Pnums[8] = { 0,0,0,0,0,0,0,0 };
	int vpnums;
	//** Intersection point IRBS val,
	int	PIRBS[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int irbs;
	float ixc;
	float iyc;
	int ptype = 0, begtype = 0, pnfound = 0;
	//int cIdx = threadIdx.x + blockIdx.x*blockDim.x;
	//int tid = blockDim.x * blockIdx.x + threadIdx.x;
	cIdx = pst + threadIdx.x + blockIdx.x*blockDim.x; // GPU tread index
	if (cIdx < pst + pn - 1)
	{
		if (cIdx == 451718)
			cIdx = cIdx;
		memcpy(&PIntX[0], &M1[0], 8 * sizeof(float));
		memcpy(&PIntY[0], &M1[0], 8 * sizeof(float));
		memcpy(&FIntX[0], &M1[0], 8 * sizeof(float));
		memcpy(&FIntY[0], &M1[0], 8 * sizeof(float));
		memcpy(&PInti[0], &iM1[0], 8 * sizeof(int));
		memcpy(&PCLC[0], &iM1[0], 8 * sizeof(int));
		memcpy(&Psubpix[0], &iM0[0], 8 * sizeof(int));
		//fw = *(d_P1xl + cIdx);
		//*(d_Wl + cIdx) = fw;
		//if (cIdx == 1883423)
		//if (cIdx == 157421)
		/*if (cIdx == 162006)
			cIdx = cIdx;*/
			//int ii = 0;

		int ii = 0, clc, pclc = -2, fi1 = 0;
		pnum = (*(dPnum + cIdx));
		//useint = (*(dPuseint + cIdx));
		x1 = round(*(dP1x + cIdx)); // getting measurement sensor endpoints
		y1 = round(*(dP1y + cIdx));
		x2 = round(*(dP2x + cIdx));
		y2 = round(*(dP2y + cIdx));
		ixc = (x1 + x2) / 2.;
		iyc = (y1 + y2) / 2.;
		//pnum = (*(dPnum + cIdx)) % 250 + 3;
		int deltaX = abs(x2 - x1);
		int deltaY = abs(y2 - y1);
		int signX = x1 < x2 ? 1 : -1;
		int signY = y1 < y2 ? 1 : -1;
		int error = deltaX - deltaY;
		int error2;
		int intind = 0; // intersection index (0 or 1)
		int pind = 0;   // B's point index [0...63]
		int xintc = x1, yintc = y1; // current and previous intersection points
		//v1 = *(pIm + y1 * imw + x1);  // 1st IRB pixel


		vp = 0;  // previous pixel
		/// filling in Bresenham's array
		while (x1 != x2 || y1 != y2)
		{
			PPx[pind] = x1;
			PPy[pind] = y1;
			v = *(pImIRB + y1 * imw + x1);
			Pval[pind] = v;
			error2 = error * 2;
			if (error2 > -deltaY)
			{
				error -= deltaY;
				x1 += signX;
			}
			if (error2 < deltaX)
			{
				error += deltaX;
				y1 += signY;
			}
			if (pind < 127)
				pind++;
			else
				break;
		} // while
		// Going forward from PS1

		ptype = begtype = pnfound = 0;
		for (i = 0; i <= pind - 1; i++)
		{
			xc = PPx[i]; yc = PPy[i];
			v = Pval[i];
			irbs = *(pImIRBS + yc * imw + xc);  // IRBSC pixel value
			if (i == 0 && v == 1)
			{
				ptype = 1;
				PIntY[ii] = yc;
				PIntX[ii] = xc;
				PInti[ii] = i;
				PCLC[ii] = -2;
				begtype = 1;
				ii = ii + 1;
			}
			if (v == 0)
			{
				begtype = 0;
				pnfound = 0;
			}
			if (v > 1 && v != pnum && begtype == 1)
			{
				begtype = 0;
				pnfound = 0;
				ii = ii - 1;
			}

			if (v == pnum)
			{
				pnfound = 1;
				PIntY[ii] = yc;
				PIntX[ii] = xc;
				PInti[ii] = i;
				Psubpix[ii] = 1;
				if (ii < 7)
					ii = ii + 1;
			}
			if (i == pind - 1 && v == 1 && pnfound == 1)
			{
				PIntY[ii] = yc;
				PIntX[ii] = xc;
				PInti[ii] = i;
				PCLC[ii] = -2;
				ii = ii + 1;
			}

		}
		//********************************************************************
		int ga[3]; // array of 3 neighboring g
		int nmwc = 0;//*number of MW (main wire) candidates
		int mw = -1;//* MW segment
		int dclc, dmin = 55555; // min CLC distance
		int d, d1, d2; // CLC distances
		int intnum = ii; /// number of intersection points found
		jSPdn = 0;

		//if (ii < 7)
		//{
		intnum = ii;
		jSPdn = 0;
		jSPup = ii - 1;
		//}
		/*else
		{
			intnum = 7;
			jSPup = 7;
		}*/

		if (intnum <= 1) //** single pix. space case
		{
			wmin = -1.;
			wmax = -1.;
			jSPdn = 0;
			jSPup = 0;
		}
		else
		{ ///**** NORMAL CASE ******
	//**************** SUBPIX COORD. ******************************
			for (j = 0; j < intnum; j++)
			{
				ga[1] = *(pIm + PPy[PInti[j]] * imw + PPx[PInti[j]]);
				if (PInti[j] > 0)
					ga[0] = *(pIm + PPy[PInti[j] - 1] * imw + PPx[PInti[j] - 1]);
				else
					ga[0] = ga[1];
				if (PInti[j] < pind - 1)
					ga[2] = *(pIm + PPy[PInti[j] + 1] * imw + PPx[PInti[j] + 1]);
				else
					ga[2] = ga[1];
				if (PInti[j] < pind - 1 && PInti[j]>0)
				{
					SubPixCrdPG(ga, PInti[j], PPx, PPy, &fIntX, &fIntY, thr);
				}
				else
				{
					fIntX = PPx[PInti[j]];
					fIntY = PPy[PInti[j]];
				}
				FIntX[j] = fIntX;
				FIntY[j] = fIntY;
			}

			//*************************************************
			wmax = sqrt((FIntX[jSPup] - FIntX[jSPdn])*(FIntX[jSPup] - FIntX[jSPdn]) + (FIntY[jSPup] - FIntY[jSPdn])*(FIntY[jSPup] - FIntY[jSPdn]));
			//* wmax : = sqrt((FIntX[intnum - 1] - FIntX[0])*(FIntX[intnum - 1] - FIntX[0]) + (FIntY[intnum - 1] - FIntY[0])*(FIntY[intnum - 1] - FIntY[0]))
			//wmin = sqrt((FIntX[mw] - FIntX[mw + 1])*(FIntX[mw] - FIntX[mw + 1]) + (FIntY[mw] - FIntY[mw + 1])*(FIntY[mw] - FIntY[mw + 1]));
			wmin = wmax;
			if (wmax > 3.0)
			{ // calculating shift
				xmid = (FIntX[jSPup] + FIntX[jSPdn]) / 2.;
				ymid = (FIntY[jSPup] + FIntY[jSPdn]) / 2.;
				shift = sqrt((ixc - xmid)*(ixc - xmid) + (iyc - ymid)*(iyc - ymid));
			}

			//} // **** normal case end ***


		}

		*(dW + cIdx) = wmax;
		*(dWmin + cIdx) = wmin;
		*(dShift + cIdx) = shift;

		*(dPint1x + cIdx) = PIntX[jSPup];
		*(dPint2x + cIdx) = PIntX[jSPdn];

		*(dPint1y + cIdx) = PIntY[jSPup];
		*(dPint2y + cIdx) = PIntY[jSPdn];

		/// saving number of intersection points ( 2 points) (left name Puseint for simplicity of changes)
		*(dPuseint + cIdx) = intnum;
		//********************************************	
	} // index cycle end
}

// new version for Pattern (GPU)
__global__ void GPU_MSPtProc34(UINT8 *pIm, UINT8 *pImIRB, UINT8 *pImIRBS, UINT16 *pImCLC, int imw, int imh, int pst, int pn, float thr, int dclcmax, int pat0space1, float *dP1x, float *dP1y, float *dP2x, float *dP2y, int *dPnum, float *dW, float *dWmin, float *dPint1x, float *dPint1y, float *dPint2x, float *dPint2y, float *dShift, int *dPuseint)
{
	int x1, y1, x2, y2, xc, yc, pnum, useint;
	int v, vp, vn, v1, v2, i, j;
	float wmin, wmax;		 // for width calculation
	float shift = 0.0, xmid, ymid; // for shift calculation
	int PPx[128];  // Bresenham's MS points array
	int PPy[128];
	int Pval[128]; // pnum values array for all MS points


	int g, gp; // current and previous gray values
	int jSPdn;
	int jSPup;
	int cIdx; // MS index

	float M1[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int iM1[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int iM0[8] = { 0,0,0,0,0,0,0,0 };
	float PIntX[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. }; // intersection points coordinates
	float PIntY[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	float FIntX[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. }; // intersection points coordinates (subpixel)
	float FIntY[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	float fIntX, fIntY; // subpix coord. measured by 
	int Psubpix[8] = { 0,0,0,0,0,0,0,0 };
	int PInti[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };	  // index array of intersection points 
	int PCLC[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };   // CLC array of intersection points 
	int Pdir[8] = { 0,0,0,0,0,0,0,0 };
	int Pnums[8] = { 0,0,0,0,0,0,0,0 };
	int vpnums;
	//** Intersection point IRBS val,
	int	PIRBS[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int irbs;
	float ixc;
	float iyc;
	//int cIdx = threadIdx.x + blockIdx.x*blockDim.x;
	//int tid = blockDim.x * blockIdx.x + threadIdx.x;
	cIdx = pst + threadIdx.x + blockIdx.x*blockDim.x; // GPU tread index
	if (cIdx < pst + pn - 1)
	{
		/*if (cIdx == 162006)
			cIdx = cIdx;*/
		memcpy(&PIntX[0], &M1[0], 8 * sizeof(float));
		memcpy(&PIntY[0], &M1[0], 8 * sizeof(float));
		memcpy(&FIntX[0], &M1[0], 8 * sizeof(float));
		memcpy(&FIntY[0], &M1[0], 8 * sizeof(float));
		memcpy(&PInti[0], &iM1[0], 8 * sizeof(int));
		memcpy(&PCLC[0], &iM1[0], 8 * sizeof(int));
		memcpy(&Psubpix[0], &iM0[0], 8 * sizeof(int));
		//fw = *(d_P1xl + cIdx);
		//*(d_Wl + cIdx) = fw;
		//if (cIdx == 1883423)
		//if (cIdx == 157421)
		/*if (cIdx == 162006)
			cIdx = cIdx;*/
			//int ii = 0;

		int ii = 0, clc, pclc = -2, fi1 = 0;
		pnum = (*(dPnum + cIdx));
		//useint = (*(dPuseint + cIdx));
		x1 = round(*(dP1x + cIdx)); // getting measurement sensor endpoints
		y1 = round(*(dP1y + cIdx));
		x2 = round(*(dP2x + cIdx));
		y2 = round(*(dP2y + cIdx));
		ixc = (x1 + x2) / 2.;
		iyc = (y1 + y2) / 2.;
		//pnum = (*(dPnum + cIdx)) % 250 + 3;
		int deltaX = abs(x2 - x1);
		int deltaY = abs(y2 - y1);
		int signX = x1 < x2 ? 1 : -1;
		int signY = y1 < y2 ? 1 : -1;
		int error = deltaX - deltaY;
		int error2;
		int intind = 0; // intersection index (0 or 1)
		int pind = 0;   // B's point index [0...63]
		int xintc = x1, yintc = y1; // current and previous intersection points
		//v1 = *(pIm + y1 * imw + x1);  // 1st IRB pixel

		vp = 0;  // previous pixel
		/// filling in Bresenham's array
		while (x1 != x2 || y1 != y2)
		{
			PPx[pind] = x1;
			PPy[pind] = y1;
			v = *(pImIRB + y1 * imw + x1);
			Pval[pind] = v;
			error2 = error * 2;
			if (error2 > -deltaY)
			{
				error -= deltaY;
				x1 += signX;
			}
			if (error2 < deltaX)
			{
				error += deltaX;
				y1 += signY;
			}
			if (pind < 127)
				pind++;
			else
				break;
		} // while
		// Going forward from PS1

		for (i = 0; i <= pind - 1; i++)
		{
			xc = PPx[i]; yc = PPy[i];
			v = Pval[i];
			irbs = *(pImIRBS + yc * imw + xc);  // IRBSC pixel value
			if (v == pnum)
			{
				PIntY[ii] = yc;
				PIntX[ii] = xc;
				PInti[ii] = i;
				Psubpix[ii] = 1;
				if (ii < 7)
					ii = ii + 1;
			}

		}
		//********************************************************************
		int ga[3]; // array of 3 neighboring g
		int nmwc = 0;//*number of MW (main wire) candidates
		int mw = -1;//* MW segment
		int dclc, dmin = 55555; // min CLC distance
		int d, d1, d2; // CLC distances
		int intnum = ii; /// number of intersection points found
		jSPdn = 0;
		jSPup = ii - 1;
		if (intnum == 1) //** single pix. space case
		{
			wmin = 1.;
			wmax = 1.;
			jSPdn = 0;
			jSPup = 0;
		}
		else
		{ ///**** NORMAL CASE ******
	//**************** SUBPIX COORD. ******************************
			for (j = 0; j < intnum; j++)
			{
				ga[1] = *(pIm + PPy[PInti[j]] * imw + PPx[PInti[j]]);
				if (PInti[j] > 0)
					ga[0] = *(pIm + PPy[PInti[j] - 1] * imw + PPx[PInti[j] - 1]);
				else
					ga[0] = ga[1];
				if (PInti[j] < pind - 1)
					ga[2] = *(pIm + PPy[PInti[j] + 1] * imw + PPx[PInti[j] + 1]);
				else
					ga[2] = ga[1];
				if (PInti[j] < pind - 1 && PInti[j]>0)
				{
					SubPixCrdPG(ga, PInti[j], PPx, PPy, &fIntX, &fIntY, thr);
				}
				else
				{
					fIntX = PPx[PInti[j]];
					fIntY = PPy[PInti[j]];
				}
				FIntX[j] = fIntX;
				FIntY[j] = fIntY;
			}

			//*************************************************
			wmax = sqrt((FIntX[jSPup] - FIntX[jSPdn])*(FIntX[jSPup] - FIntX[jSPdn]) + (FIntY[jSPup] - FIntY[jSPdn])*(FIntY[jSPup] - FIntY[jSPdn]));
			//* wmax : = sqrt((FIntX[intnum - 1] - FIntX[0])*(FIntX[intnum - 1] - FIntX[0]) + (FIntY[intnum - 1] - FIntY[0])*(FIntY[intnum - 1] - FIntY[0]))
			//wmin = sqrt((FIntX[mw] - FIntX[mw + 1])*(FIntX[mw] - FIntX[mw + 1]) + (FIntY[mw] - FIntY[mw + 1])*(FIntY[mw] - FIntY[mw + 1]));
			wmin = wmax;
			if (wmax > 3.0)
			{ // calculating shift
				xmid = (FIntX[jSPup] + FIntX[jSPdn]) / 2.;
				ymid = (FIntY[jSPup] + FIntY[jSPdn]) / 2.;
				shift = sqrt((ixc - xmid)*(ixc - xmid) + (iyc - ymid)*(iyc - ymid));
			}

			//} // **** normal case end ***

			*(dW + cIdx) = wmax;
			*(dWmin + cIdx) = wmin;
			*(dShift + cIdx) = shift;

			*(dPint1x + cIdx) = PIntX[jSPup];
			*(dPint2x + cIdx) = PIntX[jSPdn];

			*(dPint1y + cIdx) = PIntY[jSPup];
			*(dPint2y + cIdx) = PIntY[jSPdn];

			/// saving number of intersection points ( 2 points) (left name Puseint for simplicity of changes)
			*(dPuseint + cIdx) = intnum;
		}
		//********************************************	
	} // index cycle end
}

// new version for Space GPU
__global__ void GPU_MSSpProc34(UINT8 *pIm, UINT8 *pImIRB, UINT8 *pImIRBS, UINT16 *pImCLC, int imw, int imh, int pst, int pn, float thr, int dclcmax, int pat0space1, float *dP1x, float *dP1y, float *dP2x, float *dP2y, int *dPnum, float *dW, float *dWmin, float *dPint1x, float *dPint1y, float *dPint2x, float *dPint2y, float *dShift, int *dPuseint)
{
	int x1, y1, x2, y2, xc, yc, pnum, useint;
	int v, vp, vn, v1, v2, i, j;
	float wmin, wmax;		 // for width calculation
	float shift = 0.0, xmid, ymid; // for shift calculation
	int PPx[128];  // Bresenham's MS points array
	int PPy[128];
	int Pval[128]; // pnum values array for all MS points


	int g, gp; // current and previous gray values
	int jSPdn;
	int jSPup;
	int cIdx; // MS index

	float M1[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int iM1[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int iM0[8] = { 0,0,0,0,0,0,0,0 };
	float PIntX[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. }; // intersection points coordinates
	float PIntY[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	float FIntX[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. }; // intersection points coordinates (subpixel)
	float FIntY[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	float fIntX, fIntY; // subpix coord. measured by 
	int Psubpix[8] = { 0,0,0,0,0,0,0,0 };
	int PInti[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };	  // index array of intersection points 
	int PCLC[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };   // CLC array of intersection points 
	int Pdir[8] = { 0,0,0,0,0,0,0,0 };
	int Pnums[8] = { 0,0,0,0,0,0,0,0 };
	int vpnums;
	//** Intersection point IRBS val,
	int	PIRBS[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int irbs;
	float ixc;
	float iyc;
	//int cIdx = threadIdx.x + blockIdx.x*blockDim.x;
	//int tid = blockDim.x * blockIdx.x + threadIdx.x;
	cIdx = pst + threadIdx.x + blockIdx.x*blockDim.x; // GPU tread index
	if (cIdx < pst + pn - 1)
	{
		if (cIdx == 69698)
			cIdx = cIdx;
		memcpy(&PIntX[0], &M1[0], 8 * sizeof(float));
		memcpy(&PIntY[0], &M1[0], 8 * sizeof(float));
		memcpy(&FIntX[0], &M1[0], 8 * sizeof(float));
		memcpy(&FIntY[0], &M1[0], 8 * sizeof(float));
		memcpy(&PInti[0], &iM1[0], 8 * sizeof(int));
		memcpy(&PCLC[0], &iM1[0], 8 * sizeof(int));
		memcpy(&Psubpix[0], &iM0[0], 8 * sizeof(int));
		//fw = *(d_P1xl + cIdx);
		//*(d_Wl + cIdx) = fw;
		//if (cIdx == 1883423)
		//if (cIdx == 157421)
		if (cIdx == 162006)
			cIdx = cIdx;
		//int ii = 0;

		int ii = 0, clc, pclc = -2, fi1 = 0;
		pnum = (*(dPnum + cIdx));
		//useint = (*(dPuseint + cIdx));
		x1 = round(*(dP1x + cIdx)); // getting measurement sensor endpoints
		y1 = round(*(dP1y + cIdx));
		x2 = round(*(dP2x + cIdx));
		y2 = round(*(dP2y + cIdx));
		ixc = (x1 + x2) / 2.;
		iyc = (y1 + y2) / 2.;
		//pnum = (*(dPnum + cIdx)) % 250 + 3;
		int deltaX = abs(x2 - x1);
		int deltaY = abs(y2 - y1);
		int signX = x1 < x2 ? 1 : -1;
		int signY = y1 < y2 ? 1 : -1;
		int error = deltaX - deltaY;
		int error2;
		int intind = 0; // intersection index (0 or 1)
		int pind = 0;   // B's point index [0...63]
		int xintc = x1, yintc = y1; // current and previous intersection points
		//v1 = *(pIm + y1 * imw + x1);  // 1st IRB pixel

		vp = 0;  // previous pixel
		/// filling in Bresenham's array
		while (x1 != x2 || y1 != y2)
		{
			PPx[pind] = x1;
			PPy[pind] = y1;
			v = *(pImIRB + y1 * imw + x1);
			Pval[pind] = v;
			error2 = error * 2;
			if (error2 > -deltaY)
			{
				error -= deltaY;
				x1 += signX;
			}
			if (error2 < deltaX)
			{
				error += deltaX;
				y1 += signY;
			}
			if (pind < 127)
				pind++;
			else
				break;
		} // while
		// Going forward from PS1

		for (i = 0; i <= pind - 1; i++)
		{
			xc = PPx[i]; yc = PPy[i];
			v = Pval[i];
			irbs = *(pImIRBS + yc * imw + xc);  // IRBSC pixel value
			if (v == pnum)
			{
				PIntY[ii] = yc;
				PIntX[ii] = xc;
				PInti[ii] = i;
				Psubpix[ii] = 1;
				if (ii < 7)
					ii = ii + 1;
			}

		}
		//********************************************************************
		int ga[3]; // array of 3 neighboring g
		int nmwc = 0;//*number of MW (main wire) candidates
		int mw = -1;//* MW segment
		int dclc, dmin = 55555; // min CLC distance
		int d, d1, d2; // CLC distances
		int intnum = ii; /// number of intersection points found
		jSPdn = 0;
		jSPup = ii - 1;
		if (intnum == 0)
		{
			wmax = 0.5;
			wmin = 0.5;
			shift = 0.0;
			jSPdn = 0;
			jSPup = 0;
		}
		else
		{
			if (intnum == 1) //** single pix. space case
			{
				i = PInti[0];
				wmin = 1.;
				wmax = 1.;
				jSPdn = 0;
				//jSPup = 0;
				intnum = 2;
				PInti[0] = i - 1;
				PInti[1] = i + 1;
				jSPup = 1;
			}
			///**** NORMAL CASE ******
	   //**************** SUBPIX COORD. ******************************
			for (j = 0; j < intnum; j++)
			{
				ga[1] = *(pIm + PPy[PInti[j]] * imw + PPx[PInti[j]]);
				if (PInti[j] > 0)
					ga[0] = *(pIm + PPy[PInti[j] - 1] * imw + PPx[PInti[j] - 1]);
				else
					ga[0] = ga[1];
				if (PInti[j] < pind - 1)
					ga[2] = *(pIm + PPy[PInti[j] + 1] * imw + PPx[PInti[j] + 1]);
				else
					ga[2] = ga[1];

				SubPixCrdSG(ga, PInti[j], PPx, PPy, &fIntX, &fIntY, thr);
				FIntX[j] = fIntX;
				FIntY[j] = fIntY;
			}

			//*************************************************
			wmax = sqrt((FIntX[jSPup] - FIntX[jSPdn])*(FIntX[jSPup] - FIntX[jSPdn]) + (FIntY[jSPup] - FIntY[jSPdn])*(FIntY[jSPup] - FIntY[jSPdn]));
			//* wmax : = sqrt((FIntX[intnum - 1] - FIntX[0])*(FIntX[intnum - 1] - FIntX[0]) + (FIntY[intnum - 1] - FIntY[0])*(FIntY[intnum - 1] - FIntY[0]))
			//wmin = sqrt((FIntX[mw] - FIntX[mw + 1])*(FIntX[mw] - FIntX[mw + 1]) + (FIntY[mw] - FIntY[mw + 1])*(FIntY[mw] - FIntY[mw + 1]));
			wmin = wmax;
			if (wmax > 3.0)
			{ // calculating shift
				xmid = (FIntX[jSPup] + FIntX[jSPdn]) / 2.;
				ymid = (FIntY[jSPup] + FIntY[jSPdn]) / 2.;
				shift = sqrt((ixc - xmid)*(ixc - xmid) + (iyc - ymid)*(iyc - ymid));
			}

			//} // **** normal case end ***


		}
		*(dW + cIdx) = wmax;
		*(dWmin + cIdx) = wmin;
		*(dShift + cIdx) = shift;

		*(dPint1x + cIdx) = PIntX[jSPup];
		*(dPint2x + cIdx) = PIntX[jSPdn];

		*(dPint1y + cIdx) = PIntY[jSPup];
		*(dPint2y + cIdx) = PIntY[jSPdn];

		/// saving number of intersection points ( 2 points) (left name Puseint for simplicity of changes)
		*(dPuseint + cIdx) = intnum;
		//********************************************	
	} // index cycle end
}

__global__ void GPU_MSSProc33(UINT8 *pIm, UINT8 *pImIRB, UINT8 *pImIRBS, UINT16 *pImCLC, int imw, int imh, int pst, int pn, float thr, int dclcmax, int pat0space1, float *dP1x, float *dP1y, float *dP2x, float *dP2y, int *dPnum, float *dW, float *dWmin, float *dPint1x, float *dPint1y, float *dPint2x, float *dPint2y, float *dShift, int *dPuseint)
{
	int x1, y1, x2, y2, xc, yc, pnum, useint;
	int v, vp, vn, v1, v2, i, j;
	float wmin, wmax;		 // for width calculation
	float shift = 0.0, xmid, ymid; // for shift calculation
	int PPx[128];  // Bresenham's MS points array
	int PPy[128];
	int Pval[128]; // pnum values array for all MS points


	int g, gp; // current and previous gray values
	int jSPdn;
	int jSPup;
	int cIdx; // MS index

	float M1[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int iM1[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int iM0[8] = { 0,0,0,0,0,0,0,0 };
	float PIntX[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. }; // intersection points coordinates
	float PIntY[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	float FIntX[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. }; // intersection points coordinates (subpixel)
	float FIntY[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	float fIntX, fIntY; // subpix coord. measured by 
	int Psubpix[8] = { 0,0,0,0,0,0,0,0 };
	int PInti[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };	  // index array of intersection points 
	int PCLC[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };   // CLC array of intersection points 
	int Pdir[8] = { 0,0,0,0,0,0,0,0 };
	int Pnums[8] = { 0,0,0,0,0,0,0,0 };
	int vpnums;
	//** Intersection point IRBS val,
	int	PIRBS[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int irbs;
	float mind;
	float ixc;
	float iyc;
	//int cIdx = threadIdx.x + blockIdx.x*blockDim.x;
	//int tid = blockDim.x * blockIdx.x + threadIdx.x;
	cIdx = pst + threadIdx.x + blockIdx.x*blockDim.x; // GPU tread index
	if (cIdx < pst + pn - 1)
	{
		memcpy(&PIntX[0], &M1[0], 8 * sizeof(float));
		memcpy(&PIntY[0], &M1[0], 8 * sizeof(float));
		memcpy(&FIntX[0], &M1[0], 8 * sizeof(float));
		memcpy(&FIntY[0], &M1[0], 8 * sizeof(float));
		memcpy(&PInti[0], &iM1[0], 8 * sizeof(int));
		memcpy(&PCLC[0], &iM1[0], 8 * sizeof(int));
		memcpy(&Psubpix[0], &iM0[0], 8 * sizeof(int));
		//fw = *(d_P1xl + cIdx);
		//*(d_Wl + cIdx) = fw;
		//if (cIdx == 1883423)
		//if (cIdx == 157421)
		if (cIdx == 1960095)
			cIdx = cIdx;
		//int ii = 0;

		int ii = 0, clc, pclc = -2, fi1 = 0;
		pnum = (*(dPnum + cIdx));
		//useint = (*(dPuseint + cIdx));
		x1 = round(*(dP1x + cIdx)); // getting measurement sensor endpoints
		y1 = round(*(dP1y + cIdx));
		x2 = round(*(dP2x + cIdx));
		y2 = round(*(dP2y + cIdx));
		ixc = (x1 + x2) / 2.;
		iyc = (y1 + y2) / 2.;
		//pnum = (*(dPnum + cIdx)) % 250 + 3;
		int deltaX = abs(x2 - x1);
		int deltaY = abs(y2 - y1);
		int signX = x1 < x2 ? 1 : -1;
		int signY = y1 < y2 ? 1 : -1;
		int error = deltaX - deltaY;
		int error2;
		int intind = 0; // intersection index (0 or 1)
		int pind = 0;   // B's point index [0...63]
		int xintc = x1, yintc = y1; // current and previous intersection points
		//v1 = *(pIm + y1 * imw + x1);  // 1st IRB pixel

		vp = 0;  // previous pixel
		/// filling in Bresenham's array
		while (x1 != x2 || y1 != y2)
		{
			PPx[pind] = x1;
			PPy[pind] = y1;
			v = *(pImIRB + y1 * imw + x1);
			Pval[pind] = v;
			error2 = error * 2;
			if (error2 > -deltaY)
			{
				error -= deltaY;
				x1 += signX;
			}
			if (error2 < deltaX)
			{
				error += deltaX;
				y1 += signY;
			}
			if (pind < 127)
				pind++;
			else
				break;
		} // while
		// Going forward from PS1
//******* checking 1st MS pixel  *****
		if (Pval[0] == 1 || Pval[0] == pnum)
		{
			PIntY[ii] = PPy[0];
			PIntX[ii] = PPx[0];
			PInti[ii] = 0;
			clc = *(pImCLC + y1 * imw + x1);  // CLC pixel value
			//get_grayval(ImageCLC, PPy[0], PPx[0], clc)
			if (Pval[0] == pnum)
				PCLC[ii] = clc;
			else // no clc yet
				PCLC[ii] = -2;
			if (ii < 7)
				ii++;
			if (Pval[0] == 1) // 1st pix is 1
				fi1 = 1; //flag of non-confirmed inters. point
			if (Pval[1] == 0 && Pval[0] == pnum)
			{
				PIntY[ii] = PPy[0];
				PIntX[ii] = PPx[0];
				PInti[ii] = 0;
				PCLC[ii] = clc;
				if (ii < 7)ii++;
			}
		}
		else
			fi1 = 0;
		// all MS pixels except 1st and last ******************************
		for (i = 1; i <= pind - 2; i++)
		{
			xc = PPx[i]; yc = PPy[i];
			v = Pval[i];
			if (fi1 == 1)
			{
				if (v == 0 || (v > 1 && v != pnum))
				{
					if (ii > 0)
						ii--;
					fi1 = 0;
				}
			}
			if (v == pnum)
			{	//previous and next values
				fi1 = 0; // removing flag of non-confirmed inters. point
				vp = Pval[i - 1];
				vn = Pval[i + 1];
				if (vp == pat0space1 || vn == pat0space1)
				{
					clc = *(pImCLC + yc * imw + xc);  // CLC pixel value
					irbs = *(pImIRBS + yc * imw + xc);  // CLC pixel value
					PIntY[ii] = yc;
					PIntX[ii] = xc;
					PInti[ii] = i;
					Psubpix[ii] = 1;
					if ((vp == 0) && vn == 0) //* single point special case
					{
						if (ii < 7)
							ii = ii + 1;
						PIntY[ii - 1] = yc - 0.35*signY;
						PIntX[ii - 1] = xc - 0.35*signX;
						Psubpix[ii - 1] = 0;
						PIntY[ii] = yc + 0.35*signY;
						PIntX[ii] = xc + 0.35*signX;
						PInti[ii] = i;
						Psubpix[ii] = 0;
					}
					PCLC[ii] = clc;
					PIRBS[ii] = irbs;
					if (ii < 7)
						ii = ii + 1;
					pclc = clc;
				}
			}
			vp = v;
			//gp = g;
		}
		//********************************************************************
		int ga[3]; // array of 3 neighboring g
		int nmwc = 0;//*number of MW (main wire) candidates
		int mw = -1;//* MW segment
		int dclc, dmin = 55555; // min CLC distance
		int d, d1, d2; // CLC distances

			//, g1[3];
		//int g2[3], g3[3];
// *checking last MS pixel **********************************************
		if ((Pval[pind - 1] == 1 && ii % 2 == 1) || Pval[pind - 1] == pnum)
		{
			if (Pval[pind - 1] == pnum)
			{
				clc = *(pImCLC + PPy[pind - 1] * imw + PPx[pind - 1]);  // CLC pixel value
				irbs = *(pImIRBS + PPy[pind - 1] * imw + PPx[pind - 1]);  // IRBS pixel value
			}
			//get_grayval(ImageCLC, PPy[pind - 1], PPx[pind - 1], clc)
			else
			{
				clc = PCLC[ii - 1];
				irbs = PIRBS[ii - 1];
			}
			PIntY[ii] = PPy[pind - 1];
			PIntX[ii] = PPx[pind - 1];
			PInti[ii] = pind - 1;
			PCLC[ii] = clc;
			if (ii < 7)
				ii = ii + 1;
			if (Pval[pind - 2] == 0)
			{
				PIntY[ii] = PPy[pind - 1];
				PIntX[ii] = PPx[pind - 1];
				PInti[ii] = pind - 1;
				PCLC[ii] = clc;
				PIRBS[ii] = irbs;
				if (ii < 7)
					ii = ii + 1;
			}
		}
		int intnum = ii; /// number of intersection points found
		if (intnum <= 1) //** no pattern case
		{
			wmin = 0.;
			wmax = 0.;
			jSPdn = 0;
			jSPup = 0;
		}
		else
		{ ///**** NORMAL CASE ******
	//**************** SUBPIX COORD. ******************************
			//for (j = 0; j < intnum; j++)
			//{
			//	if (Psubpix[j] == 0)
			//	{
			//		FIntX[j] = PIntX[j];
			//		FIntY[j] = PIntY[j];
			//	}

			//	else
			//	{
			//		ga[1] = *(pIm + PPy[PInti[j]] * imw + PPx[PInti[j]]);
			//		//get_grayval(Im, PPy[i2 - 1], PPx[i2 - 1], g)
			//		ga[0] = *(pIm + PPy[PInti[j] - 1] * imw + PPx[PInti[j] - 1]);
			//		ga[2] = *(pIm + PPy[PInti[j] + 1] * imw + PPx[PInti[j] + 1]);
			//		SubPixCrd(ga, PInti[j], PPx, PPy, &fIntX, &fIntY, thr);
			//		FIntX[j] = fIntX;
			//		FIntY[j] = fIntY;
			//	}
			//}

			//*** previous dir
			int pdir = -1;
			//**** new
			for (j = 0; j < intnum; j++)
			{
				ga[1] = *(pIm + PPy[PInti[j]] * imw + PPx[PInti[j]]);
				if (PInti[j] > 0)
					ga[0] = *(pIm + PPy[PInti[j] - 1] * imw + PPx[PInti[j] - 1]);
				else
					ga[0] = ga[1];
				if (PInti[j] < pind - 1)
					ga[2] = *(pIm + PPy[PInti[j] + 1] * imw + PPx[PInti[j] + 1]);
				else
					ga[2] = ga[1];

				if (Psubpix[j] == 0)
				{
					FIntX[j] = PIntX[j];
					FIntY[j] = PIntY[j];
				}

				else
				{
					SubPixCrdG(ga, PInti[j], PPx, PPy, &fIntX, &fIntY, thr);
					FIntX[j] = fIntX;
					FIntY[j] = fIntY;
				}

				if (ga[2] - ga[0] > 0)
					Pdir[j] = 1;
				else if (ga[0] - ga[2] > 0)
					Pdir[j] = -1;
				if (pdir > 0)
					Pdir[j] = -1;
				else if (pdir < 0)
					Pdir[j] = 1;
				if (PInti[j] == 0 && Pdir[j] != 0 && j == 0)
					Pdir[j] = 1;
				if (PInti[j] == pind - 1 && Pdir[j] != 0 && j == intnum - 1)
					Pdir[j] = -1;
				pdir = Pdir[j];
			}
			//****
			//* ******************* Check Segments ********
				//*** finding MW segment
				//*** fixing PCLC
			int vclc;
			for (j = 0; j < intnum; j++)
			{
				//get_grayval(ImageCLC, PIntY[j], PIntX[j], vclc)
				vclc = *(pImCLC + (int)PIntY[j] * imw + (int)PIntX[j]);  // CLC pixel value
				vpnums = *(pImIRBS + (int)PIntY[j] * imw + (int)PIntX[j]);  // IRBS pixel value
				//PCLC[j] = clc;
				if (vclc == 0)
				{
					clc = 0;
					for (int dx = -3; dx <= 3; dx++)
					{
						for (int dy = -3; dy <= 3; dy++)
						{
							//get_grayval(ImageCLC, PIntY[j] + dy, PIntX[j] + dx, vclc)
							vclc = *(pImCLC + ((int)PIntY[j] + dy) * imw + (int)PIntX[j] + dx);
							if (vclc != 0)
							{
								clc = vclc;
								break;
							}
						}

						PCLC[j] = clc;
					}
				}
				Pnums[j] = vpnums;
			}

			///		********* checking segments for main wire (mw) *****
			for (j = 0; j < intnum; j += 2)
			{
				pclc = PCLC[j];
				clc = PCLC[j + 1];
				dclc = min(abs(clc - pclc), abs(clc - 65536 - pclc));
				if (dclc > dclcmax)
				{
					if (Pdir[j] != 1 || Pdir[j + 1] != -1)
						continue;
					//* could be MW, so check distance from P1 and P2
					//*         da : = (ixc - (FIntX[j] + FIntX[j + 1]) / 2.)*(ixc - (FIntX[j] + FIntX[j + 1]) / 2.) + (iyc - (FIntY[j] + FIntY[j + 1]) / 2.)*(iyc - (FIntY[j] + FIntY[j + 1]) / 2.)
					d1 = (ixc - FIntX[j])*(ixc - FIntX[j]) + (iyc - FIntY[j])*(iyc - FIntY[j]);
					d2 = (ixc - FIntX[j + 1])*(ixc - FIntX[j + 1]) + (iyc - FIntY[j + 1])*(iyc - FIntY[j + 1]);
					//d = sqrt(min(d1, d2));
					mind = min(d1, d2);
					d = sqrt(mind);
					if (d < dmin)
					{
						mw = j;
						nmwc = nmwc + 1;
						dmin = d;
					}
				}
			}
			if (mw == -1)
				mw = 0;
			//*** finding SP (spur) points
			jSPdn = mw;
			jSPup = mw + 1;
			int clc1, clc2, dclc1, dclc2;
			for (j = 0; j < intnum; j += 2)
			{
				if (j == mw)
					continue;
				else
				{
					clc = PCLC[j];
					clc1 = PCLC[mw];
					clc2 = PCLC[mw + 1];
					dclc1 = min(abs(clc - clc1), abs(clc - 65536 - clc1));
					dclc2 = min(abs(clc - clc2), abs(clc - 65536 - clc2));
					if (dclc1 > dclcmax && dclc2 > dclcmax)
						continue;//* not SP because of big dCLC
					else
					{
						if (j < mw)
						{
							dclc = min(abs(PCLC[mw] - clc1), abs(PCLC[mw] - 65536 - clc1));
							if (dclc > dclcmax)
								continue;//* not SP because of big dCLC from MW point
							else
								jSPdn = j;
						}
						else
						{
							dclc = min(abs(PCLC[mw + 1] - clc), abs(PCLC[mw + 1] - 65536 - clc));
							if (dclc > dclcmax)
								continue; //* not SP because of big dCLC from MW point
							else
								jSPup = j + 1;
						}
					}
				}
			}// for
			//*************************************************
			wmax = sqrt((FIntX[jSPup] - FIntX[jSPdn])*(FIntX[jSPup] - FIntX[jSPdn]) + (FIntY[jSPup] - FIntY[jSPdn])*(FIntY[jSPup] - FIntY[jSPdn]));
			//* wmax : = sqrt((FIntX[intnum - 1] - FIntX[0])*(FIntX[intnum - 1] - FIntX[0]) + (FIntY[intnum - 1] - FIntY[0])*(FIntY[intnum - 1] - FIntY[0]))
			wmin = sqrt((FIntX[mw] - FIntX[mw + 1])*(FIntX[mw] - FIntX[mw + 1]) + (FIntY[mw] - FIntY[mw + 1])*(FIntY[mw] - FIntY[mw + 1]));
			if (wmax > 3.0)
			{ // calculating shift
				xmid = (FIntX[jSPup] + FIntX[jSPdn]) / 2.;
				ymid = (FIntY[jSPup] + FIntY[jSPdn]) / 2.;
				shift = sqrt((ixc - xmid)*(ixc - xmid) + (iyc - ymid)*(iyc - ymid));
			}

		} // **** normal case end ***

		*(dW + cIdx) = wmax;
		*(dWmin + cIdx) = wmin;
		*(dShift + cIdx) = shift;

		*(dPint1x + cIdx) = PIntX[jSPup];
		*(dPint2x + cIdx) = PIntX[jSPdn];

		*(dPint1y + cIdx) = PIntY[jSPup];
		*(dPint2y + cIdx) = PIntY[jSPdn];

		/// saving number of intersection points ( 2 points) (left name Puseint for simplicity of changes)
		*(dPuseint + cIdx) = intnum;

		/*if (intnum == 2)
			*(dPuseint + cIdx) = 1;
		else
			*(dPuseint + cIdx) = 0;
		*/
		//********************************************	
	} // index cycle end
}

// 2022.07.07 - added MSS (for both FWM/FWMS)
__global__ void GPU_MSSProc32(UINT8 *pIm, UINT8 *pImIRB, UINT16 *pImCLC, int imw, int imh, int pst, int pn, float thr, int dclcmax, int pat0space1, float *dP1x, float *dP1y, float *dP2x, float *dP2y, int *dPnum, float *dW, float *dWmin, float *dPint1x, float *dPint1y, float *dPint2x, float *dPint2y, float *dShift, int *dPuseint)
{
	int x1, y1, x2, y2, xc, yc, pnum, useint;
	int v, vp, vn, v1, v2, i, j;
	float wmin, wmax;		 // for width calculation
	float shift = 0.0, xmid, ymid; // for shift calculation
	int PPx[128];  // Bresenham's MS points array
	int PPy[128];
	int Pval[128]; // pnum values array for all MS points


	int g, gp; // current and previous gray values
	int jSPdn;
	int jSPup;
	int cIdx; // MS index

	float M1[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int iM1[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int iM0[8] = { 0,0,0,0,0,0,0,0 };
	float PIntX[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. }; // intersection points coordinates
	float PIntY[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	float FIntX[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. }; // intersection points coordinates (subpixel)
	float FIntY[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	float fIntX, fIntY; // subpix coord. measured by 
	int Psubpix[8] = { 0,0,0,0,0,0,0,0 };
	int PInti[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };	  // index array of intersection points 
	int PCLC[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };   // CLC array of intersection points 

	float mind;

	float ixc;
	float iyc;
	//int cIdx = threadIdx.x + blockIdx.x*blockDim.x;
	//int tid = blockDim.x * blockIdx.x + threadIdx.x;
	cIdx = pst + threadIdx.x + blockIdx.x*blockDim.x; // GPU tread index
	if (cIdx < pst + pn - 1)
	{
		memcpy(&PIntX[0], &M1[0], 8 * sizeof(float));
		memcpy(&PIntY[0], &M1[0], 8 * sizeof(float));
		memcpy(&FIntX[0], &M1[0], 8 * sizeof(float));
		memcpy(&FIntY[0], &M1[0], 8 * sizeof(float));
		memcpy(&PInti[0], &iM1[0], 8 * sizeof(int));
		memcpy(&PCLC[0], &iM1[0], 8 * sizeof(int));
		memcpy(&Psubpix[0], &iM0[0], 8 * sizeof(int));
		//fw = *(d_P1xl + cIdx);
		//*(d_Wl + cIdx) = fw;
		//if (cIdx == 1883423)
		//if (cIdx == 157421)
		if (cIdx == 1960095)
			cIdx = cIdx;
		//int ii = 0;

		int ii = 0, clc, pclc = -2, fi1 = 0;
		pnum = (*(dPnum + cIdx));
		//useint = (*(dPuseint + cIdx));
		x1 = round(*(dP1x + cIdx)); // getting measurement sensor endpoints
		y1 = round(*(dP1y + cIdx));
		x2 = round(*(dP2x + cIdx));
		y2 = round(*(dP2y + cIdx));
		ixc = (x1 + x2) / 2.;
		iyc = (y1 + y2) / 2.;
		//pnum = (*(dPnum + cIdx)) % 250 + 3;
		int deltaX = abs(x2 - x1);
		int deltaY = abs(y2 - y1);
		int signX = x1 < x2 ? 1 : -1;
		int signY = y1 < y2 ? 1 : -1;
		int error = deltaX - deltaY;
		int error2;
		int intind = 0; // intersection index (0 or 1)
		int pind = 0;   // B's point index [0...63]
		int xintc = x1, yintc = y1; // current and previous intersection points
		//v1 = *(pIm + y1 * imw + x1);  // 1st IRB pixel

		vp = 0;  // previous pixel
		/// filling in Bresenham's array
		while (x1 != x2 || y1 != y2)
		{
			PPx[pind] = x1;
			PPy[pind] = y1;
			v = *(pImIRB + y1 * imw + x1);
			Pval[pind] = v;
			error2 = error * 2;
			if (error2 > -deltaY)
			{
				error -= deltaY;
				x1 += signX;
			}
			if (error2 < deltaX)
			{
				error += deltaX;
				y1 += signY;
			}
			if (pind < 127)
				pind++;
			else
				break;
		} // while
		// Going forward from PS1
//******* checking 1st MS pixel  *****
		if (Pval[0] == 1 || Pval[0] == pnum)
		{
			PIntY[ii] = PPy[0];
			PIntX[ii] = PPx[0];
			PInti[ii] = 0;
			clc = *(pImCLC + y1 * imw + x1);  // CLC pixel value
			//get_grayval(ImageCLC, PPy[0], PPx[0], clc)
			if (Pval[0] == pnum)
				PCLC[ii] = clc;
			else // no clc yet
				PCLC[ii] = -2;
			if (ii < 7)
				ii++;
			if (Pval[0] == 1) // 1st pix is 1
				fi1 = 1; //flag of non-confirmed inters. point
			if (Pval[1] == 0 && Pval[0] == pnum)
			{
				PIntY[ii] = PPy[0];
				PIntX[ii] = PPx[0];
				PInti[ii] = 0;
				PCLC[ii] = clc;
				if (ii < 7)ii++;
			}
		}
		else
			fi1 = 0;
		// all MS pixels except 1st and last ******************************
		for (i = 1; i <= pind - 2; i++)
		{
			xc = PPx[i]; yc = PPy[i];
			v = Pval[i];
			if (fi1 == 1)
			{
				if (v == 0 || (v > 1 && v != pnum))
				{
					if (ii > 0)
						ii--;
					fi1 = 0;
				}
			}
			if (v == pnum)
			{	//previous and next values
				fi1 = 0; // removing flag of non-confirmed inters. point
				vp = Pval[i - 1];
				vn = Pval[i + 1];
				if (vp == pat0space1 || vn == pat0space1)
				{
					clc = *(pImCLC + yc * imw + xc);  // CLC pixel value
					PIntY[ii] = yc;
					PIntX[ii] = xc;
					PInti[ii] = i;
					Psubpix[ii] = 1;
					if ((vp == 0) && vn == 0) //* single point special case
					{
						if (ii < 7)
							ii = ii + 1;
						PIntY[ii - 1] = yc - 0.35*signY;
						PIntX[ii - 1] = xc - 0.35*signX;
						Psubpix[ii - 1] = 0;
						PIntY[ii] = yc + 0.35*signY;
						PIntX[ii] = xc + 0.35*signX;
						PInti[ii] = i;
						Psubpix[ii] = 0;
					}
					PCLC[ii] = clc;
					if (ii < 7)
						ii = ii + 1;
					pclc = clc;
				}
			}
			vp = v;
			//gp = g;
		}
		//********************************************************************
		int ga[3]; // array of 3 neighboring g
		int nmwc = 0;//*number of MW (main wire) candidates
		int mw = -1;//* MW segment
		int dclc, dmin = 55555; // min CLC distance
		int d, d1, d2; // CLC distances

			//, g1[3];
		//int g2[3], g3[3];
// *checking last MS pixel **********************************************
		if ((Pval[pind - 1] == 1 && ii % 2 == 1) || Pval[pind - 1] == pnum)
		{
			if (Pval[pind - 1] == pnum)
				clc = *(pImCLC + PPy[pind - 1] * imw + PPx[pind - 1]);  // CLC pixel value
				//get_grayval(ImageCLC, PPy[pind - 1], PPx[pind - 1], clc)
			else
				clc = PCLC[ii - 1];

			PIntY[ii] = PPy[pind - 1];
			PIntX[ii] = PPx[pind - 1];
			PInti[ii] = pind - 1;
			PCLC[ii] = clc;
			if (ii < 7)
				ii = ii + 1;
			if (Pval[pind - 2] == 0)
			{
				PIntY[ii] = PPy[pind - 1];
				PIntX[ii] = PPx[pind - 1];
				PInti[ii] = pind - 1;
				PCLC[ii] = clc;
				if (ii < 7)
					ii = ii + 1;
			}
		}
		int intnum = ii; /// number of intersection points found
		if (intnum <= 1) //** no pattern case
		{
			wmin = 0.;
			wmax = 0.;
			jSPdn = 0;
			jSPup = 0;
		}
		else
		{ ///**** NORMAL CASE ******
	//**************** SUBPIX COORD. ******************************
			for (j = 0; j < intnum; j++)
			{
				if (Psubpix[j] == 0)
				{
					FIntX[j] = PIntX[j];
					FIntY[j] = PIntY[j];
				}

				else
				{
					ga[1] = *(pIm + PPy[PInti[j]] * imw + PPx[PInti[j]]);
					//get_grayval(Im, PPy[i2 - 1], PPx[i2 - 1], g)
					ga[0] = *(pIm + PPy[PInti[j] - 1] * imw + PPx[PInti[j] - 1]);
					ga[2] = *(pIm + PPy[PInti[j] + 1] * imw + PPx[PInti[j] + 1]);
					SubPixCrdG(ga, PInti[j], PPx, PPy, &fIntX, &fIntY, thr);
					FIntX[j] = fIntX;
					FIntY[j] = fIntY;
				}
			}
			//* ******************* Check Segments ********
				//*** finding MW segment
			//*** fixing PCLC
			for (j = 0; j < intnum; j++)
			{
				//get_grayval(ImageCLC, PIntY[j], PIntX[j], vclc)
				clc = *(pImCLC + (int)PIntY[j] * imw + (int)PIntX[j]);  // CLC pixel value
				PCLC[j] = clc;
			}

			for (j = 0; j < intnum; j += 2)
			{
				pclc = PCLC[j];
				clc = PCLC[j + 1];
				dclc = min(abs(clc - pclc), abs(clc - 65536 - pclc));
				if (dclc > dclcmax)
				{
					//* could be MW, so check distance from P1 and P2
					//*         da : = (ixc - (FIntX[j] + FIntX[j + 1]) / 2.)*(ixc - (FIntX[j] + FIntX[j + 1]) / 2.) + (iyc - (FIntY[j] + FIntY[j + 1]) / 2.)*(iyc - (FIntY[j] + FIntY[j + 1]) / 2.)
					d1 = (ixc - FIntX[j])*(ixc - FIntX[j]) + (iyc - FIntY[j])*(iyc - FIntY[j]);
					d2 = (ixc - FIntX[j + 1])*(ixc - FIntX[j + 1]) + (iyc - FIntY[j + 1])*(iyc - FIntY[j + 1]);
					//d = sqrt(min(d1, d2));
					mind = min(d1, d2);
					d = sqrt(mind);
					if (d < dmin)
					{
						mw = j;
						nmwc = nmwc + 1;
						dmin = d;
					}
				}
			}
			if (mw == -1)
				mw = 0;
			//*** finding SP (spur) points
			jSPdn = mw;
			jSPup = mw + 1;
			int clc1, clc2, dclc1, dclc2;
			for (j = 0; j < intnum; j += 2)
			{
				if (j == mw)
					continue;
				else
				{
					clc = PCLC[j];
					clc1 = PCLC[mw];
					clc2 = PCLC[mw + 1];
					dclc1 = min(abs(clc - clc1), abs(clc - 65536 - clc1));
					dclc2 = min(abs(clc - clc2), abs(clc - 65536 - clc2));
					if (dclc1 > dclcmax && dclc2 > dclcmax)
						continue;//* not SP because of big dCLC
					else
					{
						if (j < mw)
						{
							dclc = min(abs(PCLC[mw] - clc1), abs(PCLC[mw] - 65536 - clc1));
							if (dclc > dclcmax)
								continue;//* not SP because of big dCLC from MW point
							else
								jSPdn = j;
						}
						else
						{
							dclc = min(abs(PCLC[mw + 1] - clc), abs(PCLC[mw + 1] - 65536 - clc));
							if (dclc > dclcmax)
								continue; //* not SP because of big dCLC from MW point
							else
								jSPup = j + 1;
						}
					}
				}
			}// for
			//*************************************************
			wmax = sqrt((FIntX[jSPup] - FIntX[jSPdn])*(FIntX[jSPup] - FIntX[jSPdn]) + (FIntY[jSPup] - FIntY[jSPdn])*(FIntY[jSPup] - FIntY[jSPdn]));
			//* wmax : = sqrt((FIntX[intnum - 1] - FIntX[0])*(FIntX[intnum - 1] - FIntX[0]) + (FIntY[intnum - 1] - FIntY[0])*(FIntY[intnum - 1] - FIntY[0]))
			wmin = sqrt((FIntX[mw] - FIntX[mw + 1])*(FIntX[mw] - FIntX[mw + 1]) + (FIntY[mw] - FIntY[mw + 1])*(FIntY[mw] - FIntY[mw + 1]));
			if (wmax > 3.0)
			{ // calculating shift
				xmid = (FIntX[jSPup] + FIntX[jSPdn]) / 2.;
				ymid = (FIntY[jSPup] + FIntY[jSPdn]) / 2.;
				shift = sqrt((ixc - xmid)*(ixc - xmid) + (iyc - ymid)*(iyc - ymid));
			}

		} // **** normal case end ***

		*(dW + cIdx) = wmax;
		*(dWmin + cIdx) = wmin;
		*(dShift + cIdx) = shift;

		*(dPint1x + cIdx) = PIntX[jSPup];
		*(dPint2x + cIdx) = PIntX[jSPdn];

		*(dPint1y + cIdx) = PIntY[jSPup];
		*(dPint2y + cIdx) = PIntY[jSPdn];

		/// saving number of intersection points ( 2 points) (left name Puseint for simplicity of changes)
		*(dPuseint + cIdx) = intnum;

		/*if (intnum == 2)
			*(dPuseint + cIdx) = 1;
		else
			*(dPuseint + cIdx) = 0;
		*/
		//********************************************	
	} // index cycle end
}

__global__ void GPU_MSProc32(UINT8 *pIm, UINT8 *pImIRB, UINT16 *pImCLC, int imw, int imh, int pst, int pn, float thr, int dclcmax, float *dP1x, float *dP1y, float *dP2x, float *dP2y, int *dPnum, float *dW, float *dWmin, float *dPint1x, float *dPint1y, float *dPint2x, float *dPint2y, float *dShift, int *dPuseint)
{
	int x1, y1, x2, y2, xc, yc, pnum, useint;
	int v, vp, vn, v1, v2, i, j;
	float wmin, wmax;		 // for width calculation
	float shift = 0.0, xmid, ymid; // for shift calculation
	int PPx[128];  // Bresenham's MS points array
	int PPy[128];
	int Pval[128]; // pnum values array for all MS points


	int g, gp; // current and previous gray values
	int jSPdn;
	int jSPup;
	int cIdx; // MS index

	float M1[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int iM1[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int iM0[8] = { 0,0,0,0,0,0,0,0 };
	float PIntX[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. }; // intersection points coordinates
	float PIntY[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	float FIntX[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. }; // intersection points coordinates (subpixel)
	float FIntY[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	float fIntX, fIntY; // subpix coord. measured by 
	int Psubpix[8] = { 0,0,0,0,0,0,0,0 };
	int PInti[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };	  // index array of intersection points 
	int PCLC[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };   // CLC array of intersection points 

	float mind;
	float ixc;
	float iyc;
	//int cIdx = threadIdx.x + blockIdx.x*blockDim.x;
	//int tid = blockDim.x * blockIdx.x + threadIdx.x;
	cIdx = pst + threadIdx.x + blockIdx.x*blockDim.x; // GPU tread index
	if (cIdx < pst + pn - 1)
	{
		memcpy(&PIntX[0], &M1[0], 8 * sizeof(float));
		memcpy(&PIntY[0], &M1[0], 8 * sizeof(float));
		memcpy(&FIntX[0], &M1[0], 8 * sizeof(float));
		memcpy(&FIntY[0], &M1[0], 8 * sizeof(float));
		memcpy(&PInti[0], &iM1[0], 8 * sizeof(int));
		memcpy(&PCLC[0], &iM1[0], 8 * sizeof(int));
		memcpy(&Psubpix[0], &iM0[0], 8 * sizeof(int));
		//fw = *(d_P1xl + cIdx);
		//*(d_Wl + cIdx) = fw;
		//if (cIdx == 1883423)
		//if (cIdx == 157421)
		if (cIdx == 324910)
			cIdx = cIdx;
		//int ii = 0;

		int ii = 0, clc, pclc = -2, fi1 = 0;
		pnum = (*(dPnum + cIdx));
		//useint = (*(dPuseint + cIdx));
		x1 = round(*(dP1x + cIdx)); // getting measurement sensor endpoints
		y1 = round(*(dP1y + cIdx));
		x2 = round(*(dP2x + cIdx));
		y2 = round(*(dP2y + cIdx));
		ixc = (x1 + x2) / 2.;
		iyc = (y1 + y2) / 2.;
		//pnum = (*(dPnum + cIdx)) % 250 + 3;
		int deltaX = abs(x2 - x1);
		int deltaY = abs(y2 - y1);
		int signX = x1 < x2 ? 1 : -1;
		int signY = y1 < y2 ? 1 : -1;
		int error = deltaX - deltaY;
		int error2;
		int intind = 0; // intersection index (0 or 1)
		int pind = 0;   // B's point index [0...63]
		int xintc = x1, yintc = y1; // current and previous intersection points
		//v1 = *(pIm + y1 * imw + x1);  // 1st IRB pixel

		vp = 0;  // previous pixel
		/// filling in Bresenham's array
		while (x1 != x2 || y1 != y2)
		{
			PPx[pind] = x1;
			PPy[pind] = y1;
			v = *(pImIRB + y1 * imw + x1);
			Pval[pind] = v;
			error2 = error * 2;
			if (error2 > -deltaY)
			{
				error -= deltaY;
				x1 += signX;
			}
			if (error2 < deltaX)
			{
				error += deltaX;
				y1 += signY;
			}
			if (pind < 127)
				pind++;
			else
				break;
		} // while
		// Going forward from PS1
//******* checking 1st MS pixel  *****
		if (Pval[0] == 1 || Pval[0] == pnum)
		{
			PIntY[ii] = PPy[0];
			PIntX[ii] = PPx[0];
			PInti[ii] = 0;
			clc = *(pImCLC + y1 * imw + x1);  // CLC pixel value
			//get_grayval(ImageCLC, PPy[0], PPx[0], clc)
			if (Pval[0] == pnum)
				PCLC[ii] = clc;
			else // no clc yet
				PCLC[ii] = -2;
			if (ii < 7)
				ii++;
			if (Pval[0] == 1) // 1st pix is 1
				fi1 = 1; //flag of non-confirmed inters. point
			if (Pval[1] == 0 && Pval[0] == pnum)
			{
				PIntY[ii] = PPy[0];
				PIntX[ii] = PPx[0];
				PInti[ii] = 0;
				PCLC[ii] = clc;
				if (ii < 7)ii++;
			}
		}
		else
			fi1 = 0;
		// all MS pixels except 1st and last ******************************
		for (i = 1; i <= pind - 2; i++)
		{
			xc = PPx[i]; yc = PPy[i];
			v = Pval[i];
			if (fi1 == 1)
			{
				if (v == 0 || (v > 1 && v != pnum))
				{
					if (ii > 0)
						ii--;
					fi1 = 0;
				}
			}
			if (v == pnum)
			{	//previous and next values
				fi1 = 0; // removing flag of non-confirmed inters. point
				vp = Pval[i - 1];
				vn = Pval[i + 1];
				if ((vp == 0 || (vp > 1 && vp != pnum)) || vn == 0)
				{
					clc = *(pImCLC + yc * imw + xc);  // CLC pixel value
					PIntY[ii] = yc;
					PIntX[ii] = xc;
					PInti[ii] = i;
					Psubpix[ii] = 1;
					if ((vp == 0) && vn == 0) //* single point special case
					{
						if (ii < 7)
							ii = ii + 1;
						PIntY[ii - 1] = yc - 0.35*signY;
						PIntX[ii - 1] = xc - 0.35*signX;
						Psubpix[ii - 1] = 0;
						PIntY[ii] = yc + 0.35*signY;
						PIntX[ii] = xc + 0.35*signX;
						PInti[ii] = i;
						Psubpix[ii] = 0;
					}
					PCLC[ii] = clc;
					if (ii < 7)
						ii = ii + 1;
					pclc = clc;
				}
			}
			vp = v;
			//gp = g;
		}
		//********************************************************************
		int ga[3]; // array of 3 neighboring g
		int nmwc = 0;//*number of MW (main wire) candidates
		int mw = -1;//* MW segment
		int dclc, dmin = 55555; // min CLC distance
		int d, d1, d2; // CLC distances

			//, g1[3];
		//int g2[3], g3[3];
// *checking last MS pixel **********************************************
		if ((Pval[pind - 1] == 1 && ii % 2 == 1) || Pval[pind - 1] == pnum)
		{
			if (Pval[pind - 1] == pnum)
				clc = *(pImCLC + PPy[pind - 1] * imw + PPx[pind - 1]);  // CLC pixel value
				//get_grayval(ImageCLC, PPy[pind - 1], PPx[pind - 1], clc)
			else
				clc = PCLC[ii - 1];

			PIntY[ii] = PPy[pind - 1];
			PIntX[ii] = PPx[pind - 1];
			PInti[ii] = pind - 1;
			PCLC[ii] = clc;
			if (ii < 7)
				ii = ii + 1;
			if (Pval[pind - 2] == 0)
			{
				PIntY[ii] = PPy[pind - 1];
				PIntX[ii] = PPx[pind - 1];
				PInti[ii] = pind - 1;
				PCLC[ii] = clc;
				if (ii < 7)
					ii = ii + 1;
			}
		}
		int intnum = ii; /// number of intersection points found
		if (intnum <= 1) //** no pattern case
		{
			wmin = 0.;
			wmax = 0.;
			jSPdn = 0;
			jSPup = 0;
		}
		else
		{ ///**** NORMAL CASE ******
	//**************** SUBPIX COORD. ******************************
			for (j = 0; j < intnum; j++)
			{
				if (Psubpix[j] == 0)
				{
					FIntX[j] = PIntX[j];
					FIntY[j] = PIntY[j];
				}

				else
				{
					ga[1] = *(pIm + PPy[PInti[j]] * imw + PPx[PInti[j]]);
					//get_grayval(Im, PPy[i2 - 1], PPx[i2 - 1], g)
					ga[0] = *(pIm + PPy[PInti[j] - 1] * imw + PPx[PInti[j] - 1]);
					ga[2] = *(pIm + PPy[PInti[j] + 1] * imw + PPx[PInti[j] + 1]);
					SubPixCrdG(ga, PInti[j], PPx, PPy, &fIntX, &fIntY, thr);
					FIntX[j] = fIntX;
					FIntY[j] = fIntY;
				}
			}
			//* ******************* Check Segments ********
				//*** finding MW segment
			for (j = 0; j < intnum; j += 2)
			{
				pclc = PCLC[j];
				clc = PCLC[j + 1];
				dclc = min(abs(clc - pclc), abs(clc - 65536 - pclc));
				if (dclc > dclcmax)
				{
					//* could be MW, so check distance from P1 and P2
					//*         da : = (ixc - (FIntX[j] + FIntX[j + 1]) / 2.)*(ixc - (FIntX[j] + FIntX[j + 1]) / 2.) + (iyc - (FIntY[j] + FIntY[j + 1]) / 2.)*(iyc - (FIntY[j] + FIntY[j + 1]) / 2.)
					d1 = (ixc - FIntX[j])*(ixc - FIntX[j]) + (iyc - FIntY[j])*(iyc - FIntY[j]);
					d2 = (ixc - FIntX[j + 1])*(ixc - FIntX[j + 1]) + (iyc - FIntY[j + 1])*(iyc - FIntY[j + 1]);
					mind = min(d1, d2);
					d = sqrt(mind);
					//d = sqrt(min(d1, d2));
					if (d < dmin)
					{
						mw = j;
						nmwc = nmwc + 1;
						dmin = d;
					}
				}
			}
			if (mw == -1)
				mw = 0;
			//*** finding SP (spur) points
			jSPdn = mw;
			jSPup = mw + 1;
			int clc1, clc2, dclc1, dclc2;
			for (j = 0; j < intnum; j += 2)
			{
				if (j == mw)
					continue;
				else
				{
					clc = PCLC[j];
					clc1 = PCLC[mw];
					clc2 = PCLC[mw + 1];
					dclc1 = min(abs(clc - clc1), abs(clc - 65536 - clc1));
					dclc2 = min(abs(clc - clc2), abs(clc - 65536 - clc2));
					if (dclc1 > dclcmax && dclc2 > dclcmax)
						continue;//* not SP because of big dCLC
					else
					{
						if (j < mw)
						{
							dclc = min(abs(PCLC[mw] - clc1), abs(PCLC[mw] - 65536 - clc1));
							if (dclc > dclcmax)
								continue;//* not SP because of big dCLC from MW point
							else
								jSPdn = j;
						}
						else
						{
							dclc = min(abs(PCLC[mw + 1] - clc), abs(PCLC[mw + 1] - 65536 - clc));
							if (dclc > dclcmax)
								continue; //* not SP because of big dCLC from MW point
							else
								jSPup = j + 1;
						}
					}
				}
			}// for
			//*************************************************
			wmax = sqrt((FIntX[jSPup] - FIntX[jSPdn])*(FIntX[jSPup] - FIntX[jSPdn]) + (FIntY[jSPup] - FIntY[jSPdn])*(FIntY[jSPup] - FIntY[jSPdn]));
			//* wmax : = sqrt((FIntX[intnum - 1] - FIntX[0])*(FIntX[intnum - 1] - FIntX[0]) + (FIntY[intnum - 1] - FIntY[0])*(FIntY[intnum - 1] - FIntY[0]))
			wmin = sqrt((FIntX[mw] - FIntX[mw + 1])*(FIntX[mw] - FIntX[mw + 1]) + (FIntY[mw] - FIntY[mw + 1])*(FIntY[mw] - FIntY[mw + 1]));
			if (wmax > 3.0)
			{ // calculating shift
				xmid = (FIntX[jSPup] + FIntX[jSPdn]) / 2.;
				ymid = (FIntY[jSPup] + FIntY[jSPdn]) / 2.;
				shift = sqrt((ixc - xmid)*(ixc - xmid) + (iyc - ymid)*(iyc - ymid));
			}

		} // **** normal case end ***

		*(dW + cIdx) = wmax;
		*(dWmin + cIdx) = wmin;
		*(dShift + cIdx) = shift;

		*(dPint1x + cIdx) = PIntX[jSPup];
		*(dPint2x + cIdx) = PIntX[jSPdn];

		*(dPint1y + cIdx) = PIntY[jSPup];
		*(dPint2y + cIdx) = PIntY[jSPdn];

		/// checking NINT (left name Puseint for simplicity of changes
		*(dPuseint + cIdx) = intnum;
		/*if (intnum == 2)
			*(dPuseint + cIdx) = 1;
		else
			*(dPuseint + cIdx) = 0;
		*/
		//********************************************	
	} // index cycle end
}

// Find MS intersection using Bresenham's line algorithm for CPU test, based on the HDev procedure MSProc30, 2022.04.26 (GPU version)
__global__ void GPU_MSProc30(UINT8 *pIm, UINT8 *pImIRB, UINT16 *pImCLC, int imw, int imh, int pst, int pn, float thr, int dclcmax, float *dP1x, float *dP1y, float *dP2x, float *dP2y, int *dPnum, float *dW, float *dWmin, float *dPint1x, float *dPint1y, float *dPint2x, float *dPint2y)
{
	int x1, y1, x2, y2, xc, yc, pnum;
	int v, vp, vn, v1, v2, i, j;
	float wmin, wmax;
	int PPx[128];  // Bresenham's MS points array
	int PPy[128];
	int Pval[128]; // pnum values array for all MS points


	int g, gp; // current and previous gray values
	int jSPdn;
	int jSPup;
	int cIdx; // MS index

	float M1[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int iM1[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int iM0[8] = { 0,0,0,0,0,0,0,0 };
	float PIntX[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. }; // intersection points coordinates
	float PIntY[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	float FIntX[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. }; // intersection points coordinates (subpixel)
	float FIntY[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	float fIntX, fIntY; // subpix coord. measured by 
	int Psubpix[8] = { 0,0,0,0,0,0,0,0 };
	int PInti[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };	  // index array of intersection points 
	int PCLC[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };   // CLC array of intersection points 

	float mind;
	//int cIdx = threadIdx.x + blockIdx.x*blockDim.x;
	//int tid = blockDim.x * blockIdx.x + threadIdx.x;
	//for (cIdx = pst; cIdx < pst + pn; cIdx++)
	cIdx = pst + threadIdx.x + blockIdx.x*blockDim.x; // GPU tread index
	if (cIdx < pst + pn - 1)
	{
		memcpy(&PIntX[0], &M1[0], 8 * sizeof(float));
		memcpy(&PIntY[0], &M1[0], 8 * sizeof(float));
		memcpy(&FIntX[0], &M1[0], 8 * sizeof(float));
		memcpy(&FIntY[0], &M1[0], 8 * sizeof(float));
		memcpy(&PInti[0], &iM1[0], 8 * sizeof(int));
		memcpy(&PCLC[0], &iM1[0], 8 * sizeof(int));
		memcpy(&Psubpix[0], &iM0[0], 8 * sizeof(int));
		//fw = *(d_P1xl + cIdx);
		//*(d_Wl + cIdx) = fw;
		//if (cIdx == 1883423)
		//if (cIdx == 157421)
		/*if (cIdx == 2465)
			cIdx = cIdx;*/
		//int ii = 0;

		int ii = 0, clc, pclc = -2, fi1 = 0;
		pnum = (*(dPnum + cIdx));
		x1 = round(*(dP1x + cIdx)); // getting measurement sensor endpoints
		y1 = round(*(dP1y + cIdx));
		x2 = round(*(dP2x + cIdx));
		y2 = round(*(dP2y + cIdx));
		//pnum = (*(dPnum + cIdx)) % 250 + 3;
		int deltaX = abs(x2 - x1);
		int deltaY = abs(y2 - y1);
		int signX = x1 < x2 ? 1 : -1;
		int signY = y1 < y2 ? 1 : -1;
		int error = deltaX - deltaY;
		int error2;
		int intind = 0; // intersection index (0 or 1)
		int pind = 0;   // B's point index [0...63]
		int xintc = x1, yintc = y1, xintp, yintp; // current and previous intersection points
		//v1 = *(pIm + y1 * imw + x1);  // 1st IRB pixel

		vp = 0;  // previous pixel
		/// filling in Bresenham's array
		while (x1 != x2 || y1 != y2)
		{
			PPx[pind] = x1;
			PPy[pind] = y1;
			v = *(pImIRB + y1 * imw + x1);
			Pval[pind] = v;
			error2 = error * 2;
			if (error2 > -deltaY)
			{
				error -= deltaY;
				x1 += signX;
			}
			if (error2 < deltaX)
			{
				error += deltaX;
				y1 += signY;
			}
			if (pind < 127)
				pind++;
			else
				break;
		} // while
		// Going forward from PS1
//******* checking 1st MS pixel  *****
		if (Pval[0] == 1 || Pval[0] == pnum)
		{
			PIntY[ii] = PPy[0];
			PIntX[ii] = PPx[0];
			PInti[ii] = 0;
			clc = *(pImCLC + y1 * imw + x1);  // CLC pixel value
			//get_grayval(ImageCLC, PPy[0], PPx[0], clc)
			if (Pval[0] == pnum)
				PCLC[ii] = clc;
			else // no clc yet
				PCLC[ii] = -2;
			if (ii < 7)
				ii++;
			if (Pval[0] == 1) // 1st pix is 1
				fi1 = 1; //flag of non-confirmed inters. point
			if (Pval[1] == 0 && Pval[0] == pnum)
			{
				PIntY[ii] = PPy[0];
				PIntX[ii] = PPx[0];
				PInti[ii] = 0;
				PCLC[ii] = clc;
				if (ii < 7)ii++;
			}
		}
		else
			fi1 = 0;
		// all MS pixels except 1st and last ******************************
		for (i = 1; i <= pind - 2; i++)
		{
			xc = PPx[i]; yc = PPy[i];
			v = Pval[i];
			if (fi1 == 1)
			{
				if (v == 0 || (v > 1 && v != pnum))
				{
					if (ii > 0)
						ii--;
					fi1 = 0;
				}
			}
			if (v == pnum)
			{	//previous and next values
				fi1 = 0; // removing flag of non-confirmed inters. point
				vp = Pval[i - 1];
				vn = Pval[i + 1];
				if ((vp == 0 || (vp > 1 && vp != pnum)) || vn == 0)
				{
					clc = *(pImCLC + yc * imw + xc);  // CLC pixel value
					PIntY[ii] = yc;
					PIntX[ii] = xc;
					PInti[ii] = i;
					Psubpix[ii] = 1;
					if ((vp == 0) && vn == 0) //* single point special case
					{
						if (ii < 7)
							ii = ii + 1;
						PIntY[ii - 1] = yc - 0.35*signY;
						PIntX[ii - 1] = xc - 0.35*signX;
						Psubpix[ii - 1] = 0;
						PIntY[ii] = yc + 0.35*signY;
						PIntX[ii] = xc + 0.35*signX;
						PInti[ii] = i;
						Psubpix[ii] = 0;
					}
					PCLC[ii] = clc;
					if (ii < 7)
						ii = ii + 1;
					pclc = clc;
				}
			}
			vp = v;
			//gp = g;
		}
		//********************************************************************
		int ga[3]; // array of 3 neighboring g
		int nmwc = 0;//*number of MW (main wire) candidates
		int mw = -1;//* MW segment
		int dclc, dmin = 55555; // min CLC distance
		int d, d1, d2; // CLC distances

			//, g1[3];
		//int g2[3], g3[3];
// *checking last MS pixel **********************************************
		if ((Pval[pind - 1] == 1 && ii % 2 == 1) || Pval[pind - 1] == pnum)
		{
			if (Pval[pind - 1] == pnum)
				clc = *(pImCLC + PPy[pind - 1] * imw + PPx[pind - 1]);  // CLC pixel value
				//get_grayval(ImageCLC, PPy[pind - 1], PPx[pind - 1], clc)
			else
				clc = PCLC[ii - 1];

			PIntY[ii] = PPy[pind - 1];
			PIntX[ii] = PPx[pind - 1];
			PInti[ii] = pind - 1;
			PCLC[ii] = clc;
			if (ii < 7)
				ii = ii + 1;
			if (Pval[pind - 2] == 0)
			{
				PIntY[ii] = PPy[pind - 1];
				PIntX[ii] = PPx[pind - 1];
				PInti[ii] = pind - 1;
				PCLC[ii] = clc;
				if (ii < 7)
					ii = ii + 1;
			}
		}
		int intnum = ii; /// number of intersection points found
		if (intnum <= 1) //** no pattern case
		{
			wmin = 0.;
			wmax = 0.;
			jSPdn = 0;
			jSPup = 0;
		}
		else
		{ ///**** NORMAL CASE ******
	//**************** SUBPIX COORD. ******************************
			for (j = 0; j < intnum; j++)
			{
				if (Psubpix[j] == 0)
				{
					FIntX[j] = PIntX[j];
					FIntY[j] = PIntY[j];
				}

				else
				{
					ga[1] = *(pIm + PPy[PInti[j]] * imw + PPx[PInti[j]]);
					//get_grayval(Im, PPy[i2 - 1], PPx[i2 - 1], g)
					ga[0] = *(pIm + PPy[PInti[j] - 1] * imw + PPx[PInti[j] - 1]);
					ga[2] = *(pIm + PPy[PInti[j] + 1] * imw + PPx[PInti[j] + 1]);
					SubPixCrdG(ga, PInti[j], PPx, PPy, &fIntX, &fIntY, thr);
					FIntX[j] = fIntX;
					FIntY[j] = fIntY;
				}
			}
			//* ******************* Check Segments ********
				//*** finding MW segment
			for (j = 0; j < intnum; j += 2)
			{
				pclc = PCLC[j];
				clc = PCLC[j + 1];
				dclc = min(abs(clc - pclc), abs(clc - 65536 - pclc));
				if (dclc > dclcmax)
				{
					//* could be MW, so check distance from P1 and P2
					//*         da : = (ixc - (FIntX[j] + FIntX[j + 1]) / 2.)*(ixc - (FIntX[j] + FIntX[j + 1]) / 2.) + (iyc - (FIntY[j] + FIntY[j + 1]) / 2.)*(iyc - (FIntY[j] + FIntY[j + 1]) / 2.)
					d1 = (xc - FIntX[j])*(xc - FIntX[j]) + (yc - FIntY[j])*(yc - FIntY[j]);
					d2 = (xc - FIntX[j + 1])*(xc - FIntX[j + 1]) + (yc - FIntY[j + 1])*(yc - FIntY[j + 1]);
					mind = min(d1, d2);
					d = sqrt(mind);
					if (d < dmin)
					{
						mw = j;
						nmwc = nmwc + 1;
						dmin = d;
					}
				}
			}
			if (mw == -1)
				mw = 0;
			//*** finding SP (spur) points
			jSPdn = mw;
			jSPup = mw + 1;
			int clc1, clc2, dclc1, dclc2;
			for (j = 0; j < intnum; j += 2)
			{
				if (j == mw)
					continue;
				else
				{
					clc = PCLC[j];
					clc1 = PCLC[mw];
					clc2 = PCLC[mw + 1];
					dclc1 = min(abs(clc - clc1), abs(clc - 65536 - clc1));
					dclc2 = min(abs(clc - clc2), abs(clc - 65536 - clc2));
					if (dclc1 > dclcmax && dclc2 > dclcmax)
						continue;//* not SP because of big dCLC
					else
					{
						if (j < mw)
						{
							dclc = min(abs(PCLC[mw] - clc1), abs(PCLC[mw] - 65536 - clc1));
							if (dclc > dclcmax)
								continue;//* not SP because of big dCLC from MW point
							else
								jSPdn = j;
						}
						else
						{
							dclc = min(abs(PCLC[mw + 1] - clc), abs(PCLC[mw + 1] - 65536 - clc));
							if (dclc > dclcmax)
								continue; //* not SP because of big dCLC from MW point
							else
								jSPup = j + 1;
						}
					}
				}
			}// for
			//*************************************************
			wmax = sqrt((FIntX[jSPup] - FIntX[jSPdn])*(FIntX[jSPup] - FIntX[jSPdn]) + (FIntY[jSPup] - FIntY[jSPdn])*(FIntY[jSPup] - FIntY[jSPdn]));
			//* wmax : = sqrt((FIntX[intnum - 1] - FIntX[0])*(FIntX[intnum - 1] - FIntX[0]) + (FIntY[intnum - 1] - FIntY[0])*(FIntY[intnum - 1] - FIntY[0]))
			wmin = sqrt((FIntX[mw] - FIntX[mw + 1])*(FIntX[mw] - FIntX[mw + 1]) + (FIntY[mw] - FIntY[mw + 1])*(FIntY[mw] - FIntY[mw + 1]));
		} // **** normal case end ***

		*(dW + 1 * cIdx) = wmax;
		*(dWmin + 1 * cIdx) = wmin;

		*(dPint1x + 1 * cIdx + 0) = FIntX[jSPup];
		*(dPint2x + 1 * cIdx + 0) = PIntX[jSPdn];

		*(dPint1y + 1 * cIdx + 0) = PIntY[jSPup];
		*(dPint2y + 1 * cIdx + 0) = PIntY[jSPdn];

		//********************************************	
	} // index cycle end
}




// Find MS intersection using Bresenham's line algorithm for GPU test with added endpoints analysis
__global__ void GPU_FWM_Br15(UINT8 *pIm, UINT8 *pImIRB, int imw, int imh, int pst, int pni, float thr, float *dP1x, float *dP1y, float *dP2x, float *dP2y, int *dPnum, float *dW, float *dWmin, UINT64 *dPint1x, UINT64 *dPint1y, UINT64 *dPint2x, UINT64 *dPint2y)
{
	int pn = pni + 256;
	int x1, y1, x2, y2, xc, yc, pnum;
	int v, vp, v1, v2, i;
	float wmin, wmax;
	int PPx[128];  // Bresenham's points array
	int PPy[128];
	float PIntX[4] = { 0.,0.,0.,0. };
	float PIntY[4];
	int Pval[128];

	int pi1x = 0, pi1y = 0, pi2x = 0, pi2y = 0;
	//int cIdx;

	//int cIdx = threadIdx.x + blockIdx.x*blockDim.x;
	//int tid = blockDim.x * blockIdx.x + threadIdx.x;
	//for (cIdx = pst; cIdx < pst + pn; cIdx++)
	int cIdx = pst + threadIdx.x + blockIdx.x*blockDim.x; // GPU tread index
	if (cIdx < pst + pn - 1)
	{	//fw = *(d_P1xl + cIdx);
		//*(d_Wl + cIdx) = fw;
		//if (cIdx == 1883423)
		//if (cIdx == 157421)
			//if (cIdx == 5037)
			//cIdx = cIdx;
		if (cIdx == 2950038)
			cIdx = cIdx;
		pnum = (*(dPnum + cIdx));
		x1 = round(*(dP1x + cIdx)); // getting measurement sensor endpoints
		y1 = round(*(dP1y + cIdx));
		x2 = round(*(dP2x + cIdx));
		y2 = round(*(dP2y + cIdx));
		//pnum = (*(dPnum + cIdx)) % 250 + 3;
		int deltaX = abs(x2 - x1);
		int deltaY = abs(y2 - y1);
		int signX = x1 < x2 ? 1 : -1;
		int signY = y1 < y2 ? 1 : -1;
		int error = deltaX - deltaY;
		int error2;
		int intind = 0; // intersection index (0 or 1)
		int pind = 0;   // B's point index [0...63]
		int xintc = x1, yintc = y1, xintp, yintp; // current and previous intersection points
		//v1 = *(pIm + y1 * imw + x1);  // 1st IRB pixel

		vp = 0;  // previous pixel
		/// filling in Bresenham's array
		while (x1 != x2 || y1 != y2)
		{
			PPx[pind] = x1;
			PPy[pind] = y1;
			v = *(pImIRB + y1 * imw + x1);
			Pval[pind] = v;
			error2 = error * 2;
			if (error2 > -deltaY)
			{
				error -= deltaY;
				x1 += signX;
			}
			if (error2 < deltaX)
			{
				error += deltaX;
				y1 += signY;
			}
			if (pind < 127)
				pind++;
			else
				break;
		} // while
		int np = 0;
		// Going forward from PS1
		int i0 = -1;
		int i2 = -1;
		int bfound = 0;
		for (i = 1; i < pind - 1; i++)
		{
			xc = PPx[i]; yc = PPy[i];
			v = Pval[i];
			if (v == pnum)
			{
				if (bfound == 0)
				{  // real def. beginning
					PIntX[0] = xc; PIntY[0] = yc;
					i0 = i;
					bfound = 1;
					/*if (np == 0)
						np = 1;*/
				}
				else if (bfound == 2)
				{
					bfound = 1;
				}
			}
			else if (v == 1)
			{
				if (bfound == 0)
				{	// preliminary def. beginning
					PIntX[0] = xc; PIntY[0] = yc;
					i0 = i;
					bfound = 2;
				}
			}
			else if (v == 0)
			{ // space pixel
				if (bfound == 1)
				{	// end of real def. beginning search
					PIntX[2] = PPx[i - 1]; PIntY[2] = PPy[i - 1];
					i2 = i - 1;
					np = 4;
					/*if (np == 1)
						np = 2;*/
					break;
				}
				else
					bfound = 0;
			}
		}
		// Going wackward from PS2
		int i1 = -1;
		int i3 = -1;
		int bfound2 = 0;
		for (i = pind - 2; i >= 0; i--)
		{
			xc = PPx[i]; yc = PPy[i];
			v = Pval[i];
			if (v == pnum)
			{
				if (bfound2 == 0)
				{  // real def. beginning
					PIntX[1] = xc; PIntY[1] = yc;
					i1 = i;
					bfound2 = 1;
					/*if (np == 1)
						np = 2;
					else if (np == 2)
						np = 3;*/
				}
				else if (bfound2 == 2)
				{
					bfound2 = 1;
				}
			}
			else if (v == 1)
			{
				if (bfound2 == 0)
				{	// preliminary def. beginning
					PIntX[1] = xc; PIntY[1] = yc;
					i1 = i;
					bfound2 = 2;
				}
			}
			else if (v == 0)
			{ // space pixel
				if (bfound2 == 1)
				{	// end of real def. beginning search
					PIntX[3] = PPx[i + 1]; PIntY[3] = PPy[i + 1];
					i3 = i + 1;
					np = 4;
					break;
					/*if (np == 2)
						np = 3;
					else if (np == 3)
						np = 4;*/
				}
				else
					bfound2 = 0;
			}
		}
		int g0[3], g1[3];
		int g2[3], g3[3];

		//if ((i0 == -1) || (i1 == -1) || (i2 == -1) || (i3 == -1))
		if ((i0 == -1) || (i1 == -1) )
		{
			wmin = wmax = 0.0;
			//return;
		}
		else
		{
			int ppi00x = PPx[i0];
			int ppi00y = PPy[i0];
			int ppi0p1x = PPx[i0 + 1];
			int ppi0p1y = PPy[i0 + 1];
			int ppi0m1x = PPx[i0 - 1];
			int ppi0m1y = PPy[i0 - 1];

			int ppi10x = PPx[i1];
			int ppi10y = PPy[i1];
			int ppi1p1x = PPx[i1 + 1];
			int ppi1p1y = PPy[i1 + 1];
			int ppi1m1x = PPx[i1 - 1];
			int ppi1m1y = PPy[i1 - 1];


			g0[1] = *(pIm + ppi00y * imw + ppi00x);
			g0[0] = *(pIm + ppi0m1y * imw + ppi0m1x);
			g0[2] = *(pIm + ppi0p1y * imw + ppi0p1x);
			g1[1] = *(pIm + ppi10y * imw + ppi10x);
			g1[0] = *(pIm + ppi1m1y * imw + ppi1m1x);
			g1[2] = *(pIm + ppi1p1y * imw + ppi1p1x);

			float px, py;
			SubPixCrdG(&g0[0], i0, &PPx[0], &PPy[0], &px, &py, thr);
			PIntX[0] = px;
			PIntY[0] = py;
			SubPixCrdG(&g1[0], i1, &PPx[0], &PPy[0], &px, &py, thr);
			PIntX[1] = px;
			PIntY[1] = py;

		}
		int twolines = 0;

		if (i2 > 0 && i2 != i1 && np > 2)
		{
			//get_grayval(Im, PPy[i2], PPx[i2], g)
			g2[1] = *(pIm + PPy[i2] * imw + PPx[i2]);
			//get_grayval(Im, PPy[i2 - 1], PPx[i2 - 1], g)
			g2[0] = *(pIm + PPy[i2 - 1] * imw + PPx[i2 - 1]);
			g2[2] = *(pIm + PPy[i2 + 1] * imw + PPx[i2 + 1]);

			float px, py;
			SubPixCrdG(g2, i2, PPx, PPy, &px, &py, thr);
			PIntX[2] = px;
			PIntY[2] = py;
			twolines = 1;
		}
		if (i3 > 0 && i3 != i0 && np > 2)
		{
			//get_grayval(Im, PPy[i2], PPx[i2], g)
			g3[1] = *(pIm + PPy[i3] * imw + PPx[i3]);
			//get_grayval(Im, PPy[i2 - 1], PPx[i2 - 1], g)
			g3[0] = *(pIm + PPy[i3 - 1] * imw + PPx[i3 - 1]);
			g3[2] = *(pIm + PPy[i3 + 1] * imw + PPx[i3 + 1]);

			float px, py;
			SubPixCrdG(g3, i3, PPx, PPy, &px, &py, thr);
			PIntX[3] = px;
			PIntY[3] = py;
			twolines = 1;
		}
		wmin = 0.;
		if (np > 1)
		{
			if (PIntX[0] != 0 && PIntX[1] != 0 && (twolines == 0))
			{
				wmax = sqrt((float)(PIntX[0] - PIntX[1])*(float)(PIntX[0] - PIntX[1]) + (float)(PIntY[0] - PIntY[1])*(float)(PIntY[0] - PIntY[1]));
				wmin = wmax;
			}
			if (twolines == 1)
			{
				wmax = sqrt((PIntX[0] - PIntX[1])*(PIntX[0] - PIntX[1]) + (PIntY[0] - PIntY[1])*(PIntY[0] - PIntY[1]));
				// selecting line which includes skeleton
				xc = (x1 + x2) / 2;
				yc = (y1 + y2) / 2;
				int dX = x2 - x1;
				int dY = y2 - y1;
				if (dX != 0)
				{
					if (xc <= PIntX[0] && xc >= PIntX[2] || xc >= PIntX[0] && xc <= PIntX[2])
						wmin = sqrt((PIntX[0] - PIntX[2])*(PIntX[0] - PIntX[2]) + (PIntY[0] - PIntY[2])*(PIntY[0] - PIntY[2]));
					else if (xc <= PIntX[1] && xc >= PIntX[3] || xc >= PIntX[1] && xc <= PIntX[3])
						wmin = sqrt((PIntX[1] - PIntX[3])*(PIntX[1] - PIntX[3]) + (PIntY[1] - PIntY[3])*(PIntY[1] - PIntY[3]));
					else
						wmin = -1.;
				}
				else
				{
					if (yc <= PIntY[0] && yc >= PIntY[2] || yc >= PIntY[0] && yc <= PIntY[2])
						wmin = sqrt((PIntX[0] - PIntX[2])*(PIntX[0] - PIntX[2]) + (PIntY[0] - PIntY[2])*(PIntY[0] - PIntY[2]));
					else if (xc <= PIntX[1] && xc >= PIntX[3] || xc >= PIntX[1] && xc <= PIntX[3])
						wmin = sqrt((PIntX[1] - PIntX[3])*(PIntX[1] - PIntX[3]) + (PIntY[1] - PIntY[3])*(PIntY[1] - PIntY[3]));

					else
						wmin = -1;
				}
			}
		}
		else
		{
			wmax = 0.;
			wmin = 0.;
		}

		*(dW + 1 * cIdx) = wmax;
		*(dWmin + 1 * cIdx) = wmin;

		*(dPint1x + 1 * cIdx + 0) = PIntX[0];
		*(dPint2x + 1 * cIdx + 0) = PIntX[1];

		*(dPint1y + 1 * cIdx + 0) = PIntY[0];
		*(dPint2y + 1 * cIdx + 0) = PIntY[1];
	}
	//return;
///////////////////////////////////////////////

		/*if ((pi1x != 0) && (pi2x != 0))
			fw = (float)sqrt((pi1x - pi2x)*(pi1x - pi2x) + (pi1y - pi2y)*(pi1y - pi2y));
		else
			fw = 0.;
		*(dW + 1 * cIdx) = fw;*/
		//} // for
	cIdx = cIdx;
}

// Find subpixel MS intersection using Bresenham's line algorithm for Non-FWM with CPU 
__global__ void GPU_NonFWM_Br15(UINT8 *pIm, UINT8 *pImIRB, int imw, int imh, int pst, int pn, float thr, float *dP1x, float *dP1y, float *dP2x, float *dP2y, int *dPnum, float *dW, float *dWmin, UINT64 *dPint1x, UINT64 *dPint1y, UINT64 *dPint2x, UINT64 *dPint2y)
{
	int x1, y1, x2, y2, xc, yc, pnum, xb, yb;
	int v, vp, v1, v2, i;
	float wmin, wmax;
	int PPx[128];  // Bresenham's points array
	int PPy[128];
	float PIntX[4] = { 0.,0.,0.,0. };
	float PIntY[4];
	int Pval[128];

	int pi1x = 0, pi1y = 0, pi2x = 0, pi2y = 0;
	//int cIdx;

	//int cIdx = threadIdx.x + blockIdx.x*blockDim.x;
	//int tid = blockDim.x * blockIdx.x + threadIdx.x;
	int cIdx = pst + threadIdx.x + blockIdx.x*blockDim.x; // GPU tread index
	if (cIdx < pst + pn - 1)
	{	//fw = *(d_P1xl + cIdx);
		//*(d_Wl + cIdx) = fw;
		//if (cIdx == 1883423)
		pnum = *(dPnum + cIdx);
		x1 = round(*(dP1x + cIdx)); // getting measurement sensor endpoints
		y1 = round(*(dP1y + cIdx));
		x2 = round(*(dP2x + cIdx));
		y2 = round(*(dP2y + cIdx));
		xc = x1; yc = y1;
		// begining (P1) coordinates
		xb = x1; yb = y1;
		//pnum = (*(dPnum + cIdx)) % 250 + 3;
		int deltaX = abs(x2 - x1);
		int deltaY = abs(y2 - y1);
		int signX = x1 < x2 ? 1 : -1;
		int signY = y1 < y2 ? 1 : -1;
		int error = deltaX - deltaY;
		int error2;
		int intind = 0; // intersection index (0 or 1)
		int pind = 0;   // B's point index [0...63]
		int xintc = x1, yintc = y1, xintp, yintp; // current and previous intersection points
		//v1 = *(pIm + y1 * imw + x1);  // 1st IRB pixel

		vp = 0;  // previous pixel
		/// filling in Bresenham's array
		while (x1 != x2 || y1 != y2)
		{
			PPx[pind] = x1;
			PPy[pind] = y1;
			v = *(pImIRB + y1 * imw + x1);
			Pval[pind] = v;
			error2 = error * 2;
			if (error2 > -deltaY)
			{
				error -= deltaY;
				x1 += signX;
			}
			if (error2 < deltaX)
			{
				error += deltaX;
				y1 += signY;
			}
			if (pind < 127)
				pind++;
			else
				break;
		} // while
		int np = 0;
		// Going forward from PS1
		int i0 = -1;
		int i1 = -1;
		int i2 = -1;
		int bfound = 0;
		for (i = 1; i < pind - 2; i++)
		{
			xc = PPx[i]; yc = PPy[i];
			v = Pval[i];
			if (v == pnum)
			{
				if (bfound == 0)
				{  // real def. beginning
					PIntX[0] = xc; PIntY[0] = yc;
					i0 = i;
					bfound = 1;
					np = 1;
				}
				else if (bfound == 2)
				{
					bfound = 1;
				}
				else if (bfound == 3)
				{
					PIntX[2] = xc; PIntY[2] = yc;
					i2 = i;
					bfound = 4;
					np = 3;
				}
			}
			else if (v == 1)
			{
				if (bfound == 0)
				{	// preliminary def. beginning
					PIntX[0] = xc; PIntY[0] = yc;
					i0 = i;
					bfound = 2;
					np = 1;
				}
			}
			else if (v == 0)
			{ // space pixel
				if (bfound == 1)
				{	// end of real def. beginning search
					PIntX[1] = PPx[i - 1]; PIntY[1] = PPy[i - 1];
					i1 = i - 1;
					np = 2;
					bfound = 3;
					/*if (np == 1)
						np = 2;*/
						//break;
				}
				else if (bfound == 2)
					bfound = 0;
			}
		}
		//** removing last intersection if P2 is outside of pattern
		if (Pval[pind - 1] == 0 && np > 1)
		{
			np--;
			PIntX[np] = 0;
			PIntY[np] = 0;
		}

		int g0[3], g1[3];
		int g2[3];
		float px, py;

		if (i0 == -1)
		{
			wmin = wmax = 0.0;
			//return;
		}
		else
		{
			int ppi00x = PPx[i0];
			int ppi00y = PPy[i0];
			int ppi0p1x = PPx[i0 + 1];
			int ppi0p1y = PPy[i0 + 1];
			int ppi0m1x = PPx[i0 - 1];
			int ppi0m1y = PPy[i0 - 1];
			g0[1] = *(pIm + ppi00y * imw + ppi00x);
			g0[0] = *(pIm + ppi0m1y * imw + ppi0m1x);
			g0[2] = *(pIm + ppi0p1y * imw + ppi0p1x);
			SubPixCrdG(&g0[0], i0, &PPx[0], &PPy[0], &px, &py, thr);
			PIntX[0] = px;
			PIntY[0] = py;
		}

		if ((i1 >= 0) && (i1 < pind - 1))
		{
			int ppi10x = PPx[i1];
			int ppi10y = PPy[i1];
			int ppi1p1x = PPx[i1 + 1];
			int ppi1p1y = PPy[i1 + 1];
			int ppi1m1x = PPx[i1 - 1];
			int ppi1m1y = PPy[i1 - 1];
			g1[1] = *(pIm + ppi10y * imw + ppi10x);
			g1[0] = *(pIm + ppi1m1y * imw + ppi1m1x);
			g1[2] = *(pIm + ppi1p1y * imw + ppi1p1x);
			SubPixCrdG(&g1[0], i1, &PPx[0], &PPy[0], &px, &py, thr);
			PIntX[1] = px;
			PIntY[1] = py;
		}
		if ((i2 >= 0) && (i2 < pind - 1))
		{
			int ppi20x = PPx[i2];
			int ppi20y = PPy[i2];
			int ppi2p1x = PPx[i2 + 1];
			int ppi2p1y = PPy[i2 + 1];
			int ppi2m1x = PPx[i2 - 1];
			int ppi2m1y = PPy[i2 - 1];
			g2[1] = *(pIm + PPy[i2] * imw + PPx[i2]);
			g2[0] = *(pIm + PPy[i2 - 1] * imw + PPx[i2 - 1]);
			g2[2] = *(pIm + PPy[i2 + 1] * imw + PPx[i2 + 1]);
			SubPixCrdG(g2, i2, PPx, PPy, &px, &py, thr);
			PIntX[2] = px;
			PIntY[2] = py;
		}

		float PCX, PCY, wc;  // reference point and distance to P2 (wc)
		PCX = (xb + x2) / 2.;
		PCY = (yb + y2) / 2.;
		wc = sqrt((PCX - x2)*(PCX - x2) + (PCY - y2)*(PCY - y2));

		wmin = 0.;
		if (np > 0)
		{
			if (PIntX[0] != 0)
			{
				wmax = sqrt((float)(PIntX[0] - x2)*(float)(PIntX[0] - x2) + (float)(PIntY[0] - y2)*(float)(PIntY[0] - y2));
				wmin = wmax;
			}
			if (np > 1)
			{
				wmin = sqrt((x2 - PIntX[1])*(x2 - PIntX[1]) + (y2 - PIntY[1])*(y2 - PIntY[1]));

			}
			if (np > 2)
			{
				wmin = sqrt((x2 - PIntX[2])*(x2 - PIntX[2]) + (y2 - PIntY[2])*(y2 - PIntY[2]));

			}
			wmax = wmax - wc;
			wmin = wmin - wc;
		}
		else
		{
			wmax = 0.;
			wmin = 0.;
		}



		*(dW + 1 * cIdx) = wmax;
		*(dWmin + 1 * cIdx) = wmin;

		*(dPint1x + 1 * cIdx + 0) = PIntX[0];
		*(dPint2x + 1 * cIdx + 0) = PIntX[1];

		*(dPint1y + 1 * cIdx + 0) = PIntY[0];
		*(dPint2y + 1 * cIdx + 0) = PIntY[1];
	}
}
__global__ void GPU_NonFWM_Br14(UINT8 *pIm, UINT8 *pImIRB, int imw, int imh, int pst, int pn, float thr, float *dP1x, float *dP1y, float *dP2x, float *dP2y, int *dPnum, float *dW, float *dWmin, UINT64 *dPint1x, UINT64 *dPint1y, UINT64 *dPint2x, UINT64 *dPint2y)
{
	int x1, y1, x2, y2, xc, yc, pnum;
	int v, vp, i;
	float wmin, wmax;
	int PPx[128];  // Bresenham's points array
	int PPy[128];
	float PIntX[4] = { 0.,0.,0.,0. };
	float PIntY[4];
	int Pval[128];

	//int pi1x = 0, pi1y = 0, pi2x = 0, pi2y = 0;
	//int cIdx;

	//int cIdx = threadIdx.x + blockIdx.x*blockDim.x;
	//int tid = blockDim.x * blockIdx.x + threadIdx.x;
	int cIdx = pst + threadIdx.x + blockIdx.x*blockDim.x; // GPU tread index
	if (cIdx < pst + pn - 1)
	{	//fw = *(d_P1xl + cIdx);
		//*(d_Wl + cIdx) = fw;
		//if (cIdx == 1883423)
		if (cIdx == 100000)
			//if (cIdx == 5037)
			cIdx = cIdx;
		pnum = (*(dPnum + cIdx)) % 250 + 3;
		x1 = round(*(dP1x + cIdx)); // getting measurement sensor endpoints
		y1 = round(*(dP1y + cIdx));
		x2 = round(*(dP2x + cIdx));
		y2 = round(*(dP2y + cIdx));
		xc = x1; yc = y1;
		//pnum = (*(dPnum + cIdx)) % 250 + 3;
		int deltaX = abs(x2 - x1);
		int deltaY = abs(y2 - y1);
		int signX = x1 < x2 ? 1 : -1;
		int signY = y1 < y2 ? 1 : -1;
		int error = deltaX - deltaY;
		int error2;
		int intind = 0; // intersection index (0 or 1)
		int pind = 0;   // B's point index [0...63]
		int xintc = x1, yintc = y1; // current and previous intersection points
		//v1 = *(pIm + y1 * imw + x1);  // 1st IRB pixel

		vp = 0;  // previous pixel
		/// filling in Bresenham's array
		while (x1 != x2 || y1 != y2)
		{
			PPx[pind] = x1;
			PPy[pind] = y1;
			v = *(pImIRB + y1 * imw + x1);
			Pval[pind] = v;
			error2 = error * 2;
			if (error2 > -deltaY)
			{
				error -= deltaY;
				x1 += signX;
			}
			if (error2 < deltaX)
			{
				error += deltaX;
				y1 += signY;
			}
			if (pind < 127)
				pind++;
			else
				break;
		} // while
		int np = 0;
		// Going forward from PS1
		int i0 = -1;
		int i1 = -1;
		int i2 = -1;
		int bfound = 0;
		for (i = 1; i < pind - 2; i++)
		{
			xc = PPx[i]; yc = PPy[i];
			v = Pval[i];
			if (v == pnum)
			{
				if (bfound == 0)
				{  // real def. beginning
					PIntX[0] = xc; PIntY[0] = yc;
					i0 = i;
					bfound = 1;
					np = 1;
				}
				else if (bfound == 2)
				{
					bfound = 1;
				}
				else if (bfound == 3)
				{
					PIntX[2] = xc; PIntY[2] = yc;
					i2 = i;
					bfound = 4;
					np = 3;
				}
			}
			else if (v == 1)
			{
				if (bfound == 0)
				{	// preliminary def. beginning
					PIntX[0] = xc; PIntY[0] = yc;
					i0 = i;
					bfound = 2;
					np = 1;
				}
			}
			else if (v == 0)
			{ // space pixel
				if (bfound == 1)
				{	// end of real def. beginning search
					PIntX[1] = PPx[i - 1]; PIntY[1] = PPy[i - 1];
					i1 = i - 1;
					np = 2;
					bfound = 3;
					/*if (np == 1)
						np = 2;*/
						//break;
				}
				else if (bfound == 2)
					bfound = 0;
			}
		}
		//** removing last intersection if P2 is outside of pattern
		if (Pval[pind - 1] == 0 && np > 1)
		{
			np--;
			PIntX[np] = 0;
			PIntY[np] = 0;
		}

		int g0[3], g1[3];
		int g2[3];
		float px, py;

		if (i0 == -1)
		{
			wmin = wmax = 0.0;
			//return;
		}
		else
		{
			int ppi00x = PPx[i0];
			int ppi00y = PPy[i0];
			int ppi0p1x = PPx[i0 + 1];
			int ppi0p1y = PPy[i0 + 1];
			int ppi0m1x = PPx[i0 - 1];
			int ppi0m1y = PPy[i0 - 1];
			g0[1] = *(pIm + ppi00y * imw + ppi00x);
			g0[0] = *(pIm + ppi0m1y * imw + ppi0m1x);
			g0[2] = *(pIm + ppi0p1y * imw + ppi0p1x);
			SubPixCrdG(&g0[0], i0, &PPx[0], &PPy[0], &px, &py, thr);
			PIntX[0] = px;
			PIntY[0] = py;
		}

		if ((i1 >= 0) && (i1 < pind - 1))
		{
			int ppi10x = PPx[i1];
			int ppi10y = PPy[i1];
			int ppi1p1x = PPx[i1 + 1];
			int ppi1p1y = PPy[i1 + 1];
			int ppi1m1x = PPx[i1 - 1];
			int ppi1m1y = PPy[i1 - 1];
			g1[1] = *(pIm + ppi10y * imw + ppi10x);
			g1[0] = *(pIm + ppi1m1y * imw + ppi1m1x);
			g1[2] = *(pIm + ppi1p1y * imw + ppi1p1x);
			SubPixCrdG(&g1[0], i1, &PPx[0], &PPy[0], &px, &py, thr);
			PIntX[1] = px;
			PIntY[1] = py;
		}
		if ((i2 >= 0) && (i2 < pind - 1))
		{
			int ppi20x = PPx[i2];
			int ppi20y = PPy[i2];
			int ppi2p1x = PPx[i2 + 1];
			int ppi2p1y = PPy[i2 + 1];
			int ppi2m1x = PPx[i2 - 1];
			int ppi2m1y = PPy[i2 - 1];
			g2[1] = *(pIm + PPy[i2] * imw + PPx[i2]);
			g2[0] = *(pIm + PPy[i2 - 1] * imw + PPx[i2 - 1]);
			g2[2] = *(pIm + PPy[i2 + 1] * imw + PPx[i2 + 1]);
			SubPixCrdG(g2, i2, PPx, PPy, &px, &py, thr);
			PIntX[2] = px;
			PIntY[2] = py;
		}

		float PCX, PCY, wc;  // reference point and distance to P2 (wc)
		PCX = (xc + x2) / 2.;
		PCY = (yc + y2) / 2.;
		wc = sqrt((PCX - x2)*(PCX - x2) + (PCY - y2)*(PCY - y2));

		wmin = 0.;
		if (np > 0)
		{
			if (PIntX[0] != 0)
			{
				wmax = sqrt((float)(PIntX[0] - x2)*(float)(PIntX[0] - x2) + (float)(PIntY[0] - y2)*(float)(PIntY[0] - y2));
				wmin = wmax;
			}
			if (np > 1)
			{
				wmin = sqrt((x2 - PIntX[1])*(x2 - PIntX[1]) + (y2 - PIntY[1])*(y2 - PIntY[1]));

			}
			if (np > 2)
			{
				wmin = sqrt((x2 - PIntX[2])*(x2 - PIntX[2]) + (y2 - PIntY[2])*(y2 - PIntY[2]));

			}
			wmax = wmax - wc;
			wmin = wmin - wc;
		}
		else
		{
			wmax = 0.;
			wmin = 0.;
		}



		*(dW + 1 * cIdx) = wmax;
		*(dWmin + 1 * cIdx) = wmin;

		*(dPint1x + 1 * cIdx + 0) = PIntX[0];
		*(dPint2x + 1 * cIdx + 0) = PIntX[1];

		*(dPint1y + 1 * cIdx + 0) = PIntY[0];
		*(dPint2y + 1 * cIdx + 0) = PIntY[1];
	}
	//return;
///////////////////////////////////////////////

		/*if ((pi1x != 0) && (pi2x != 0))
			fw = (float)sqrt((pi1x - pi2x)*(pi1x - pi2x) + (pi1y - pi2y)*(pi1y - pi2y));
		else
			fw = 0.;
		*(dW + 1 * cIdx) = fw;*/
		//} // for
	cIdx = cIdx;
}






//void SubPixCrd(int *g0, int i0, int *PPx, int *PPy, float *pIntX, float *pIntY)
//{
//	//int PIntX[4], PIntY[4];
//	if (((g0[0] > 50) && (g0[1] > 50) && (g0[2] > 50)) || ((g0[0] < 50) && (g0[1] < 50) && (g0[2] < 50)))
//	{
//		*pIntX = PPx[i0];
//		*pIntY = PPy[i0];
//		return;
//	}
//	if ((g0[1] - g0[0]) > (g0[1] - g0[2]))
//	{
//		int dg10 = g0[1] - g0[0];
//		if (dg10 != 0)
//		{
//			float dd0 = -(g0[1] - 50.0) / dg10;
//			*pIntX = PPx[i0] + (PPx[i0] - PPx[i0 - 1])*dd0;
//			*pIntY = PPy[i0] + (PPy[i0] - PPy[i0 - 1])*dd0;
//		}
//		else
//		{
//			*pIntX = PPx[i0];
//			*pIntY = PPy[i0];
//		}
//	}
//	else
//	{
//		int dg20 = g0[2] - g0[1];
//		if (dg20 != 0)
//		{
//			float dd0 = (g0[1] - 50.0) / dg20;
//			*pIntX  = PPx[i0] + (PPx[i0] - PPx[i0 + 1])*dd0;
//			*pIntY  = PPy[i0] + (PPy[i0] - PPy[i0 + 1])*dd0;
//		}
//		else
//		{
//			*pIntX = PPx[i0];
//			*pIntY = PPy[i0];
//		}
//	}
//
//}

void CUDA_Load1(int imw, int imh, int cgpu, UINT8 *ppIm, UINT8 *ppImIRB, UINT8 *ppImIRBS, UINT16 *ppImCLC)
{
	cudaSetDevice(cgpu);
	//cudaDeviceReset();
	cudaError_t Cerror = cudaGetLastError();

	Cerror = cudaMemcpy(d_Im, ppIm, imw * imh * sizeof(UINT8), cudaMemcpyHostToDevice);
	Cerror = cudaMemcpy(d_ImIRB, ppImIRB, imw * imh * sizeof(UINT8), cudaMemcpyHostToDevice);
	Cerror = cudaMemcpy(d_ImIRBS, ppImIRBS, imw * imh * sizeof(UINT8), cudaMemcpyHostToDevice);
	Cerror = cudaMemcpy(d_ImCLC, ppImCLC, imw * imh * sizeof(UINT16), cudaMemcpyHostToDevice);
	//cudaMemcpy(d_Im, pIm, imw * imh * sizeof(UINT8), cudaMemcpyHostToDevice);
	//cudaGetLastError();
}




void CUDA_Unload1(int pn, int cgpu, float *pWidth, float *pWmin, float *pPInt1x, float *pPInt1y, float *pPInt2x, float *pPInt2y, float *pShift)
{
	//int pn = pni + 0;
	cudaSetDevice(cgpu);
	cudaError_t Cerror = cudaGetLastError();
	Cerror = cudaMemcpy(pWidth, d_Wl, pn  * sizeof(float), cudaMemcpyDeviceToHost);
	Cerror = cudaMemcpy(pWmin, d_Wlmin, pn  * sizeof(float), cudaMemcpyDeviceToHost);
	Cerror = cudaMemcpy(pPInt1x, d_Pint1x, pn * 1  * sizeof(UINT64), cudaMemcpyDeviceToHost);
	Cerror = cudaMemcpy(pPInt1y, d_Pint1y, pn * 1  * sizeof(UINT64), cudaMemcpyDeviceToHost);
	Cerror = cudaMemcpy(pPInt2x, d_Pint2x, pn * 1  * sizeof(UINT64), cudaMemcpyDeviceToHost);
	Cerror = cudaMemcpy(pPInt2y, d_Pint2y, pn * 1  * sizeof(UINT64), cudaMemcpyDeviceToHost);

	Cerror = cudaMemcpy(pShift, d_PShift, pn * 1 * sizeof(float), cudaMemcpyDeviceToHost);
	//cudaGetLastError();
	/*cudaFree(d_Pint1x);
	cudaFree(d_Pint1y);
	cudaFree(d_Wl);*/
	//Cerror = cudaGetLastError();
}

void CUDA_UnloadN(int pn, int cgpu, float *pWidthN, float *pWminN, float *pPInt1Nx, float *pPInt1Ny, float *pPInt2Nx, float *pPInt2Ny)  /// 
{
	//int pn = pni + 0;
	cudaSetDevice(cgpu);
	cudaError_t Cerror = cudaGetLastError();
	Cerror = cudaMemcpy(pWidthN, d_WlN, pn  * sizeof(float), cudaMemcpyDeviceToHost);
	Cerror = cudaMemcpy(pWminN, d_WlminN, pn  * sizeof(float), cudaMemcpyDeviceToHost);
	Cerror = cudaMemcpy(pPInt1Nx, d_Pint1Nx, pn * 1  * sizeof(UINT64), cudaMemcpyDeviceToHost);
	Cerror = cudaMemcpy(pPInt1Ny, d_Pint1Ny, pn * 1  * sizeof(UINT64), cudaMemcpyDeviceToHost);
	/*Cerror = cudaMemcpy(pPInt2x, d_Pint2x, pn * 1 / ngpu * sizeof(UINT64), cudaMemcpyDeviceToHost);
	Cerror = cudaMemcpy(pPInt2y, d_Pint2y, pn * 1 / ngpu * sizeof(UINT64), cudaMemcpyDeviceToHost);*/
	//cudaGetLastError();
	/*cudaFree(d_Pint1x);
	cudaFree(d_Pint1y);
	cudaFree(d_Wl);*/
	//Cerror = cudaGetLastError();
}



void FWM_BR_CUDA_LaunchN( int imw, int imh, int pst, int pn, int cgpu, float thr)
{
	dim3 dimBlock(BLOCK_SIZE);
	dim3 dimGrid((pn + 1) / dimBlock.x);

	int numBlock = (pn + 1) / ThrPerBlock;

	cudaSetDevice(cgpu);
	cudaError_t Cerror = cudaGetLastError();
	Sleep(20); /// without delay error 702 is generated, strange
	//cudaDeviceReset();
	//GPU_FWM_Br15 << <numBlock, ThrPerBlock >> > (d_Im, d_ImIRB, imw, imh, pst, pn-256, thr, d_P1x, d_P1y, d_P2x, d_P2y, d_Pnum, d_Wl, d_Wlmin, d_Pint1x, d_Pint1y, d_Pint2x, d_Pint2y);
	//GPU_FWM_Br17 << <numBlock, ThrPerBlock >> > (d_Im, d_ImIRB, imw, imh, pst, pn - 0, thr, d_P1x, d_P1y, d_P2x, d_P2y, d_Pnum, d_Wl, d_Wlmin, d_Pint1x, d_Pint1y, d_Pint2x, d_Pint2y);
	//__global__ void GPU_MSProc30(UINT8 *pIm, UINT8 *pImIRB, UINT16 *pImCLC, int imw, int imh, int pst, int pn, float thr, int dclcmax, float *dP1x, float *dP1y, float *dP2x, float *dP2y, int *dPnum, float *dW, float *dWmin, float *dPint1x, float *dPint1y, float *dPint2x, float *dPint2y)
	//FWM_BR_CPU_LaunchN(mmpIm, mmpImIRB, mmpImCLC, iw, ih, 0, mmNp, mmthr, mmpP1x, mmpP1y, mmpP2x, mmpP2y, mmpPnum, mmpWidth, mmpWmin, mmpPInt1x, mmpPInt1y, mmpPInt2x, mmpPInt2y);
	int dclcmax = 72;
	//GPU_MSSProc32 << <numBlock, ThrPerBlock >> > (d_Im, d_ImIRB, d_ImCLC, imw, imh, pst, pn - 0, thr, dclcmax, 0, d_P1x, d_P1y, d_P2x, d_P2y, d_Pnum, d_Wl, d_Wlmin, d_Pint1x, d_Pint1y, d_Pint2x, d_Pint2y, d_PShift, d_Puseint);
	GPU_MSPtProc36 << <numBlock, ThrPerBlock >> > (d_Im, d_ImIRB, d_ImIRBS, d_ImCLC, imw, imh, pst, pn - 0, thr, dclcmax, 0, d_P1x, d_P1y, d_P2x, d_P2y, d_Pnum, d_Wl, d_Wlmin, d_Pint1x, d_Pint1y, d_Pint2x, d_Pint2y, d_PShift, d_Puseint);

	if ((Cerror = cudaGetLastError()))
		Beep(2000, 100);
	//cudaThreadSynchronize();
	cudaDeviceSynchronize();

	// check for error
	Cerror = cudaGetLastError();
	
}

void NonFWM_BR_CUDA_Launch(int imw, int imh, int pst, int pnN, int cgpu, float thr)
{
	dim3 dimBlock(BLOCK_SIZE);
	dim3 dimGrid((pnN + 1) / dimBlock.x);

	int numBlock = (pnN + 1) / ThrPerBlock;

	cudaSetDevice(cgpu);
	cudaError_t Cerror = cudaGetLastError();
	//cudaDeviceReset();
	GPU_NonFWM_Br15 << <numBlock, ThrPerBlock >> > (d_Im, d_ImIRB, imw, imh, pst, pnN - 256, thr, d_P1Nx, d_P1Ny, d_P2Nx, d_P2Ny, d_PnumN, d_WlN, d_WlminN, d_Pint1Nx, d_Pint1Ny, d_Pint2Nx, d_Pint2Ny);
	if ((Cerror = cudaGetLastError()))
		Beep(2000, 100);
	//cudaThreadSynchronize();
	cudaDeviceSynchronize();

	// check for error
	Cerror = cudaGetLastError();

}




void FWM_BR_CUDA_Launch(UINT8 *pIm, int imw, int imh, int pn)
{
	dim3 dimBlock(BLOCK_SIZE);
	dim3 dimGrid((pn + 1) / dimBlock.x);
	//float *d_P1x, *d_P1y, *d_P2x, *d_P2y, *d_Wl;
	//int *d_Pnum;

	//CPU_FWM_Br (pIm, imw, imh, pn, pP1x, pP1y, pP2x, pP2y, pPnum, pWidth);

	int numThread = 32;
	int numBlock = (pn + 1) / numThread;

	
	cudaError_t error = cudaGetLastError();

	cudaMallocManaged((void**)&d_Wl, pn * 4 * sizeof(float)); // measured width output array
	cudaMallocManaged((void**)&d_WG, pn * 4 * sizeof(float)); // measured width output array
	cudaMallocManaged((void**)&dmedo, pn * 4 * sizeof(float)); // measured width output array

	cudaMallocManaged((void**)&d_Im, imw * imh * sizeof(UINT8));
	//cudaMallocManaged((void**)&d_ImIRB, imw * imh * sizeof(UINT8));
	cudaMemcpy(d_Im, pIm, imw * imh * sizeof(UINT8), cudaMemcpyHostToDevice);
	//cudaMemcpy(d_ImIRB, pImIRB, imw * imh * sizeof(UINT8), cudaMemcpyHostToDevice);
	error = cudaGetLastError();

	GPU_FWM_Br <<<numBlock, numThread >>> (d_Im, imw, imh, pn, d_P1x, d_P1y, d_P2x, d_P2y, d_Pnum, d_Wl);
	error = cudaGetLastError();
	//cudaThreadSynchronize();
	cudaDeviceSynchronize();
	// check for error
	error = cudaGetLastError();
	//if (error > 0)
		//cudaThreadExit();
	//cudaMemcpy(pWidth, d_W, pn * sizeof(float), cudaMemcpyDeviceToHost);
	cudaMemcpy(pWidth, d_Wl, pn * 4 * sizeof(float), cudaMemcpyDeviceToHost);

	/*cudaFree(d_P1x);
	cudaFree(d_P1y);
	cudaFree(d_P2x);
	cudaFree(d_P2y);
	cudaFree(d_Pnum);*/
	cudaFree(d_Wl);
	cudaFree(d_Im);
}

void FWM_BR_CPU_Launch(UINT8 *pIm, int imw, int imh, int pst, int pn)
{
	/*dim3 dimBlock(BLOCK_SIZE);
	dim3 dimGrid((pn + 1) / dimBlock.x);
	float *d_P1x, *d_P1y, *d_P2x, *d_P2y, *d_Wl;
	int *d_Pnum;*/

	CPU_FWM_Br (pIm, imw, imh, pst, pn, pP1x, pP1y, pP2x, pP2y, pPnum, pWidth);

}

void FWM_BR_CPU_LaunchN(UINT8 *pIm, UINT8 *pImIRB, UINT8 *pImIRBS, UINT16 *pImCLC, int imw, int imh, int pst, int pn, float thr, int pat0space1, float *pP1x, float *pP1y, float *pP2x, float *pP2y, int *pPnum, float *pWidth, float *pWmin, float *pPInt1x, float *pPInt1y, float *pPInt2x, float *pPInt2y, float *pShift, int *pPuseint)
{
	/*dim3 dimBlock(BLOCK_SIZE);
	dim3 dimGrid((pn + 1) / dimBlock.x);
	float *d_P1x, *d_P1y, *d_P2x, *d_P2y, *d_Wl;
	int *d_Pnum;*/

	//CPU_FWM_Br11(pIm, imw, imh, pst,pn, pP1x, pP1y, pP2x, pP2y, pPnum, pWidth, pPInt1x, pPInt1y, pPInt2x, pPInt2y);
	//CPU_FWM_Br15(pIm, pImIRB, imw, imh, pst, pn, thr, pP1x, pP1y, pP2x, pP2y, pPnum, pWidth, pWmin, pPInt1x, pPInt1y, pPInt2x, pPInt2y);
	int dclcmax = 72;
	//CPU_MSProc30(pIm, pImIRB, pImCLC, imw, imh, pst, pn, thr, dclcmax, pP1x, pP1y, pP2x, pP2y, pPnum, pWidth, pWmin, pPInt1x, pPInt1y, pPInt2x, pPInt2y);
	//CPU_MSSProc32(pIm, pImIRB, pImCLC, imw, imh, pst, pn, thr, dclcmax, pat0space1, pP1x, pP1y, pP2x, pP2y, pPnum, pWidth, pWmin, pPInt1x, pPInt1y, pPInt2x, pPInt2y, pShift, pPuseint);
	CPU_MSPtProc37(pIm, pImIRB, pImIRBS, pImCLC, imw, imh, pst, pn, thr, dclcmax, pat0space1, pP1x, pP1y, pP2x, pP2y, pPnum, pWidth, pWmin, pPInt1x, pPInt1y, pPInt2x, pPInt2y, pShift, pPuseint);

}

//  For narrow space processing 2022.09.08
void FWM_BR_CPU_LaunchS(UINT8 *pIm, UINT8 *pImIRB, UINT8 *pImIRBS, UINT16 *pImCLC, int imw, int imh, int pst, int pn, float thr, int pat0space1, float *pP1x, float *pP1y, float *pP2x, float *pP2y, int *pPnum, float *pWidth, float *pWmin, float *pPInt1x, float *pPInt1y, float *pPInt2x, float *pPInt2y, float *pShift, int *pPuseint)
{	
	/*dim3 dimBlock(BLOCK_SIZE);
	dim3 dimGrid((pn + 1) / dimBlock.x);
	float *d_P1x, *d_P1y, *d_P2x, *d_P2y, *d_Wl;
	int *d_Pnum;*/

	//CPU_FWM_Br11(pIm, imw, imh, pst,pn, pP1x, pP1y, pP2x, pP2y, pPnum, pWidth, pPInt1x, pPInt1y, pPInt2x, pPInt2y);
	//CPU_FWM_Br15(pIm, pImIRB, imw, imh, pst, pn, thr, pP1x, pP1y, pP2x, pP2y, pPnum, pWidth, pWmin, pPInt1x, pPInt1y, pPInt2x, pPInt2y);
	int dclcmax = 72;
	//CPU_MSProc30(pIm, pImIRB, pImCLC, imw, imh, pst, pn, thr, dclcmax, pP1x, pP1y, pP2x, pP2y, pPnum, pWidth, pWmin, pPInt1x, pPInt1y, pPInt2x, pPInt2y);
	//CPU_MSSProc32(pIm, pImIRB, pImCLC, imw, imh, pst, pn, thr, dclcmax, pat0space1, pP1x, pP1y, pP2x, pP2y, pPnum, pWidth, pWmin, pPInt1x, pPInt1y, pPInt2x, pPInt2y, pShift, pPuseint);
	//CPU_MSSpProc34(pIm, pImIRB, pImIRBS, pImCLC, imw, imh, pst, pn, thr, dclcmax, pat0space1, pP1x, pP1y, pP2x, pP2y, pPnum, pWidth, pWmin, pPInt1x, pPInt1y, pPInt2x, pPInt2y, pShift, pPuseint);
	CPU_MSSpProc36(pIm, pImIRB, pImIRBS, pImCLC, imw, imh, pst, pn, thr, dclcmax, pat0space1, pP1x, pP1y, pP2x, pP2y, pPnum, pWidth, pWmin, pPInt1x, pPInt1y, pPInt2x, pPInt2y, pShift, pPuseint);

}

void NonFWM_BR_CPU_Launch(UINT8 *pIm, UINT8 *pImIRB, int imw, int imh, int pst, int pn, float thr, float *pP1Nx, float *pP1Ny, float *pP2Nx, float *pP2Ny, int *pPnumN, float *pWidthN, float *pWminN, float *pPInt1Nx, float *pPInt1Ny, float *pPInt2Nx, float *pPInt2Ny)
{
	CPU_NonFWM_Br15(pIm, pImIRB, imw, imh, pst, pn, thr, pP1Nx, pP1Ny, pP2Nx, pP2Ny, pPnumN, pWidthN, pWminN, pPInt1Nx, pPInt1Ny, pPInt2Nx, pPInt2Ny);
}


//MStoGP (fwm)
void AllocationMStoGPU(int cGPU, int pn, int na, int nac, int imw, int imh, float  *pP1x, float  *pP1y, float  *pP2x, float  *pP2y, float  *pP3x, float  *pP3y, int *pPnum, int  *pWG, int *pPaddrC, int *pPaddrR)
{
	
	cudaError_t Cerror;
	Cerror = cudaSetDevice(cGPU);
	Cerror = cudaDeviceReset();
	///// Imputs
	Cerror = cudaMallocManaged((void**)&d_P1x, pn * sizeof(float));
	memcpy(d_P1x, pP1x, pn * sizeof(float));
	Cerror = cudaMallocManaged((void**)&d_P1y, pn  * sizeof(float) );
	memcpy(d_P1y, pP1y, pn * sizeof(float));
	Cerror = cudaMallocManaged((void**)&d_P2x, pn  * sizeof(float));
	
	memcpy(d_P2x, pP2x, pn * sizeof(float));
	Cerror = cudaMallocManaged((void**)&d_P2y, pn * sizeof(float));
	memcpy(d_P2y, pP2y, pn * sizeof(float));
	Cerror = cudaMallocManaged((void**)&d_Pnum, pn * sizeof(int));
	memcpy(d_Pnum, pPnum , pn * sizeof(int));

	Cerror = cudaMallocManaged((void**)&d_P3x, pn * sizeof(float));
	memcpy(d_P3x, pP3x, pn * sizeof(float));
	Cerror = cudaMallocManaged((void**)&d_P3y, pn * sizeof(int));
	memcpy(d_P3y, pP3y, pn * sizeof(float));
	
	Cerror = cudaMallocManaged((void**)&d_Pnum, pn * sizeof(int));
	memcpy(d_Pnum, pPnum, pn * sizeof(int));

	Cerror = cudaMallocManaged((void**)&d_PaddrR, na * sizeof(int));
	memcpy(d_PaddrR, pPaddrR, na * sizeof(int));

	Cerror = cudaMallocManaged((void**)&d_PaddrC, nac * sizeof(int));
	memcpy(d_PaddrC, pPaddrC, nac * sizeof(int));

	///// Outputs
	Cerror = cudaMallocManaged((void**)&d_Wl, pn * 1 * sizeof(float)); // measured width output array
	Cerror = cudaMallocManaged((void**)&d_Wlmin, pn * 1 * sizeof(float)); // measured min width output array
	Cerror = cudaMallocManaged((void**)&d_Pint1x, pn * 1 * sizeof(UINT64)); // intersection points output array
	Cerror = cudaMallocManaged((void**)&d_Pint1y, pn * 1 * sizeof(UINT64)); // intersection points output array
	Cerror = cudaMallocManaged((void**)&d_Pint2x, pn * 1 * sizeof(UINT64)); // intersection points output array
	Cerror = cudaMallocManaged((void**)&d_Pint2y, pn * 1 * sizeof(UINT64)); // intersection points output array
	Cerror = cudaMallocManaged((void**)&d_Im, imw * imh * sizeof(UINT8));
	Cerror = cudaMallocManaged((void**)&d_ImIRB, imw * imh * sizeof(UINT8));
	Cerror = cudaMallocManaged((void**)&d_ImIRBS, imw * imh * sizeof(UINT8));
	Cerror = cudaMallocManaged((void**)&d_ImCLC, imw * imh * sizeof(UINT16));

	Cerror = cudaMallocManaged((void**)&d_PShift, pn * 1 * sizeof(float)); // shift output array
	Cerror = cudaMallocManaged((void**)&d_Puseint, pn * 1 * sizeof(int)); // intersection valid output array

	Cerror = cudaMallocManaged((void**)&d_pDefMB, ND * na * sizeof(float)); // MB defect output array
	Cerror = cudaMallocManaged((void**)&d_pXdefMB, ND * na * sizeof(float)); // MB defect X coord. output array
	Cerror = cudaMallocManaged((void**)&d_pYdefMB, ND * na * sizeof(float)); // MB defect Y coord. output array
	Cerror = cudaMallocManaged((void**)&d_pDefSP, ND * na * sizeof(float)); // SP defect output array
	Cerror = cudaMallocManaged((void**)&d_pXdefSP, ND * na * sizeof(float)); // SP defect X coord. output array
	Cerror = cudaMallocManaged((void**)&d_pYdefSP, ND * na * sizeof(float)); // SP defect Y coord. output array
	Cerror = cudaMallocManaged((void**)&d_pMBnum, na * sizeof(int)); // pointer to MBnum array on device
	Cerror = cudaMallocManaged((void**)&d_pSPnum, na * sizeof(int)); // pointer to SPnum array on device

}

//MSNtoGP (for Non-FWM)
void AllocationMSNtoGPU(int cGPU, int pnN, int narN, int nacN, int imw, int imh, float  *pP1Nx, float  *pP1Ny, float  *pP2Nx, float  *pP2Ny, int *pPnumN, int  *pWGN, int *pPaddrCN, int *pPaddrRN)
{

	cudaError_t Cerror;
	Cerror = cudaSetDevice(cGPU);
	//Cerror = cudaDeviceReset();
	///// Imputs
	Cerror = cudaMallocManaged((void**)&d_P1Nx, pnN * sizeof(float));
	memcpy(d_P1Nx, pP1Nx, pnN * sizeof(float));
	Cerror = cudaMallocManaged((void**)&d_P1Ny, pnN * sizeof(float));
	memcpy(d_P1Ny, pP1Ny, pnN * sizeof(float));
	Cerror = cudaMallocManaged((void**)&d_P2Nx, pnN * sizeof(float));

	memcpy(d_P2Nx, pP2Nx, pnN * sizeof(float));
	Cerror = cudaMallocManaged((void**)&d_P2Ny, pnN * sizeof(float));
	memcpy(d_P2Ny, pP2Ny, pnN * sizeof(float));
	Cerror = cudaMallocManaged((void**)&d_PnumN, pnN * sizeof(int));
	memcpy(d_PnumN, pPnumN, pnN * sizeof(int));

	
	Cerror = cudaMallocManaged((void**)&d_PindCN, nacN * sizeof(int));
	memcpy(d_PindCN, pPindCN, nacN * sizeof(int));
	Cerror = cudaMallocManaged((void**)&d_PaddrCN, nacN * sizeof(int));
	memcpy(d_PaddrCN, pPaddrCN, nacN * sizeof(int));
	Cerror = cudaMallocManaged((void**)&d_PaddrRN, narN * sizeof(int));
	memcpy(d_PaddrRN, pPaddrRN, narN * sizeof(int));

	///// Outputs
	Cerror = cudaMallocManaged((void**)&d_WlN, pnN * 1 * sizeof(float)); // measured width output array
	Cerror = cudaMallocManaged((void**)&d_WlminN, pnN * 1 * sizeof(float)); // measured min width output array
	Cerror = cudaMallocManaged((void**)&d_Pint1Nx, pnN * 1 * sizeof(UINT64)); // intersection points output array
	Cerror = cudaMallocManaged((void**)&d_Pint1Ny, pnN * 1 * sizeof(UINT64)); // intersection points output array
	Cerror = cudaMallocManaged((void**)&d_Pint2Nx, pnN * 1 * sizeof(UINT64)); // intersection points output array
	Cerror = cudaMallocManaged((void**)&d_Pint2Ny, pnN * 1 * sizeof(UINT64)); // intersection points output array
	/*Cerror = cudaMallocManaged((void**)&d_Im, imw * imh * sizeof(UINT8));
	Cerror = cudaMallocManaged((void**)&d_ImIRB, imw * imh * sizeof(UINT8));*/

	Cerror = cudaMallocManaged((void**)&d_pDefMBN, ND * nacN * sizeof(float)); // MB defect output array
	Cerror = cudaMallocManaged((void**)&d_pXdefMBN, ND * nacN * sizeof(float)); // MB defect X coord. output array
	Cerror = cudaMallocManaged((void**)&d_pYdefMBN, ND * nacN * sizeof(float)); // MB defect Y coord. output array
	Cerror = cudaMallocManaged((void**)&d_pDefSPN, ND * nacN * sizeof(float)); // SP defect output array
	Cerror = cudaMallocManaged((void**)&d_pXdefSPN, ND * nacN * sizeof(float)); // SP defect X coord. output array
	Cerror = cudaMallocManaged((void**)&d_pYdefSPN, ND * nacN * sizeof(float)); // SP defect Y coord. output array
	Cerror = cudaMallocManaged((void**)&d_pMBnumN, nacN * sizeof(int)); // pointer to MBnum array on device
	Cerror = cudaMallocManaged((void**)&d_pSPnumN, nacN * sizeof(int)); // pointer to SPnum array on device

}


__global__ void gpuInvert(    UINT8* image,    int width,    int height    )
{
    int row = blockIdx.y * BLOCK_SIZE + threadIdx.y;
    int col = blockIdx.x * BLOCK_SIZE + threadIdx.x;

 //   int cIdx =  row * width + col; 
	int cIdx =  row * blockDim.x * gridDim.x + col; 
	if(cIdx < width*height-1)
    // *( image + cIdx     ) = 255;// -  *( image + cIdx); // 
        image[cIdx] = 0xff -  *( image + cIdx);
}

void deviceInvertLaunch( int width, int height )
{
    // launch kernel
    dim3 dimBlock( BLOCK_SIZE, BLOCK_SIZE );
    dim3 dimGrid( width / dimBlock.x, height / dimBlock.y );
////	src.upload(frame);
	 UINT16 *d_Image; 
	cudaMalloc( (void**) &d_Image,  width * height * sizeof(UINT8)  );
	 cudaMemcpy(d_Image, pIm,  width * height * sizeof(UINT8), cudaMemcpyHostToDevice);
////	 cudaMemcpy(d_Image, iframe.data,  width * height * sizeof(UINT8), cudaMemcpyHostToDevice);
	 
//	gpuInvert<<<dimGrid, dimBlock>>>( d_Image, width, height);
	//cudaThreadSynchronize(); //depricated
	cudaDeviceSynchronize();
//	cudaMemcpy(pIm, d_Image,   width * height * sizeof(UINT8), cudaMemcpyDeviceToHost);

	cudaFree(d_Image);

}


template <typename T>
__global__ void imageFilteringKernel(const T *d_f, const unsigned int paddedW, const unsigned int paddedH,
	const T *d_g, const int S,
	T *d_h, const unsigned int W, const unsigned int H)
{

	// Set the padding size and filter size
	unsigned int paddingSize = S;
	unsigned int filterSize = 2 * S + 1;

	// Set the pixel coordinate
	const unsigned int j = blockIdx.x * blockDim.x + threadIdx.x + paddingSize;
	const unsigned int i = blockIdx.y * blockDim.y + threadIdx.y + paddingSize;

	// The multiply-add operation for the pixel coordinate ( j, i )
	if (j >= paddingSize && j < paddedW - paddingSize && i >= paddingSize && i < paddedH - paddingSize) {
		unsigned int oPixelPos = (i - paddingSize) * W + (j - paddingSize);
		d_h[oPixelPos] = 0.0;
		for (int k = -S; k <= S; k++) {
			for (int l = -S; l <= S; l++) {
				unsigned int iPixelPos = (i + k) * paddedW + (j + l);
				unsigned int coefPos = (k + S) * filterSize + (l + S);
				d_h[oPixelPos] += d_f[iPixelPos] * d_g[coefPos];
			}
		}
	}

}
void deviceFilterLaunch(int W, int H)
{
	// Allocate the memory space for a 3x3 filter on a host
	float *h_g;
	int S = 1; // the parameter determines the size of a filter; note the signed type
	unsigned int filterSize = 2 * S + 1;
	h_g = new float[filterSize * filterSize];

	// Read the coefficients of a filter from the file, gradient_filter.dat
	//readFilter(h_g, "gradient_filter.dat");

	// Perform padding for the image
	// Note that the padding size is S.
	unsigned int paddedW = W + 2 * S;
	unsigned int paddedH = H + 2 * S;
	float *h_f;
	h_f = new float[paddedW * paddedH];
	//replicationPadding(h_tmpF, W, H, S, h_f, paddedW, paddedH);

	// Allocate the memory space for the padded image on a device
	float *d_f;
	unsigned int paddedImageSizeByte = paddedW * paddedH * sizeof(float);
	cudaMalloc(reinterpret_cast<void **>(&d_f), paddedImageSizeByte);

	// Transfer the padded image from a host to a device
	cudaMemcpy(d_f, h_f, paddedImageSizeByte, cudaMemcpyHostToDevice); // Host to Device

	// Allocate the memory space for the filter on a device
	float *d_g;
	unsigned int filterKernelSizeByte = filterSize * filterSize * sizeof(float);
	cudaMalloc(reinterpret_cast<void **>(&d_g), filterKernelSizeByte);

	// Transfer the filter from a host to a device
	cudaMemcpy(d_g, h_g, filterKernelSizeByte, cudaMemcpyHostToDevice); // Host to Device

	//////////////////
	dim3 dimBlock(BLOCK_SIZE, BLOCK_SIZE);
	dim3 dimGrid(W / dimBlock.x, H / dimBlock.y);

	float *d_h; // the filtering result
	unsigned int imageSizeByte = W * H * sizeof(float);
	cudaMalloc(reinterpret_cast<void **>(&d_h), imageSizeByte);

	imageFilteringKernel <<<dimGrid, dimBlock >>> (d_f, paddedW, paddedH,
		d_g, S,	d_h, W, H);

	cudaDeviceSynchronize();
}
/*  
cudaError_t addWithCuda(int *c, const int *a, const int *b, unsigned int size);

__global__ void addKernel(int *c, const int *a, const int *b)
{
    int i = threadIdx.x;
    c[i] = a[i] + b[i];
}

int main()
{
    const int arraySize = 5;
    const int a[arraySize] = { 1, 2, 3, 4, 5 };
    const int b[arraySize] = { 10, 20, 30, 40, 50 };
    int c[arraySize] = { 0 };

    // Add vectors in parallel. 
    cudaError_t cudaStatus = addWithCuda(c, a, b, arraySize);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "addWithCuda failed!");
        return 1;
    }

    printf("{1,2,3,4,5} + {10,20,30,40,50} = {%d,%d,%d,%d,%d}\n",
        c[0], c[1], c[2], c[3], c[4]);

    // cudaDeviceReset must be called before exiting in order for profiling and
    // tracing tools such as Nsight and Visual Profiler to show complete traces.
    cudaStatus = cudaDeviceReset();
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaDeviceReset failed!");
        return 1;
    }

    return 0;
}

// Helper function for using CUDA to add vectors in parallel.
cudaError_t addWithCuda(int *c, const int *a, const int *b, unsigned int size)
{
    int *dev_a = 0;
    int *dev_b = 0;
    int *dev_c = 0;
    cudaError_t cudaStatus;

    // Choose which GPU to run on, change this on a multi-GPU system.
    cudaStatus = cudaSetDevice(0);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaSetDevice failed!  Do you have a CUDA-capable GPU installed?");
        goto Error;
    }

    // Allocate GPU buffers for three vectors (two input, one output)    .
    cudaStatus = cudaMalloc((void**)&dev_c, size * sizeof(int));
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMalloc failed!");
        goto Error;
    }

    cudaStatus = cudaMalloc((void**)&dev_a, size * sizeof(int));
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMalloc failed!");
        goto Error;
    }

    cudaStatus = cudaMalloc((void**)&dev_b, size * sizeof(int));
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMalloc failed!");
        goto Error;
    }

    // Copy input vectors from host memory to GPU buffers.
    cudaStatus = cudaMemcpy(dev_a, a, size * sizeof(int), cudaMemcpyHostToDevice);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMemcpy failed!");
        goto Error;
    }

    cudaStatus = cudaMemcpy(dev_b, b, size * sizeof(int), cudaMemcpyHostToDevice);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMemcpy failed!");
        goto Error;
    }

    // Launch a kernel on the GPU with one thread for each element.
    addKernel<<<1, size>>>(dev_c, dev_a, dev_b);

    // Check for any errors launching the kernel
    cudaStatus = cudaGetLastError();
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "addKernel launch failed: %s\n", cudaGetErrorString(cudaStatus));
        goto Error;
    }
    
    // cudaDeviceSynchronize waits for the kernel to finish, and returns
    // any errors encountered during the launch.
    cudaStatus = cudaDeviceSynchronize();
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching addKernel!\n", cudaStatus);
        goto Error;
    }

    // Copy output vector from GPU buffer to host memory.
    cudaStatus = cudaMemcpy(c, dev_c, size * sizeof(int), cudaMemcpyDeviceToHost);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMemcpy failed!");
        goto Error;
    }

Error:
    cudaFree(dev_c);
    cudaFree(dev_a);
    cudaFree(dev_b);
    
    return cudaStatus;
}
*/


