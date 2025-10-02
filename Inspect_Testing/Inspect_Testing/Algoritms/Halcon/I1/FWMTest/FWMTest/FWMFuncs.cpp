//#include "HalconCpp.h"
//#include "HDevThread.h"
using namespace HalconCpp;
//#include "math.h"
//#include "cuda_runtime_api.h"


//#include "math_functions.h"

#include <stdio.h>
#include <basetsd.h>
#include <memory.h>

//#include "windows.h"
#include "iostream"

#include <set>
#include <stdlib.h>

//#include <omp.h>  // OpenMP
//#include "opencv2/opencv.hpp"

#include <stdio.h>
#include "pch.h"

//#include "FWMFunc.h"



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

// // new version for Space  2 Pnum (from Pat 36)
void CPU_MSSpProc36B2(UINT8 *pIm, UINT16 *pImIRBS, UINT16 *pImCLCS, int imw, int imh, int pst, int pn, 
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
// new version for Pattern 231024 -fixed multiple pnums in a row, fixed neighboring same region wires
void CPU_MSPtProc39B2(UINT8 *pIm, UINT16 *pImIRB, UINT16 *pImCLC, int imw, int imh, int pst, int pn, 
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
				if ((vp == pnum || vp == pnum2) && (vn == pnum || vn == pnum2))
					continue;
				else if (vp == 0 && (vn == pnum || vn == pnum2 || vn == 1 || vn == 0))
					btype = 1;
				else if ((vp == 1 && vp != pnum && vp != pnum2) || (vn == 0 && vp != 0))
					btype = -1;
				else if ((vn == 1 && ((vp == pnum) || vp == pnum2)) || (vp == 1 && (vn == pnum || vn == pnum2)))
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

///////////////////////////////////////////////////////////
//			Running Median Filter
///////////////////////////////////////////////////////////

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

void FWMS_PostProcCPU(float *pwS, int *pwgS, float *medoS, float *pXCS, float *pYCS, int naS, int nPS, int nvoid, int *pPaddrS, int sz1, int sz2, int skip, float ang, float mprc, float sprc, float mabs, float sabs, float minmbs, int dlen, float *pDefMB, float *pXdefMB, float *pYdefMB, float *pDefSP, float *pXdefSP, float *pYdefSP, int *MBnumS, int *SPnumS)
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
	for (j = 0; j < naS - 0; j++)
	{
		caddrb = *(pPaddrS + j + 0);
		if (j + 1 < naS)
			caddre = *(pPaddrS + j + 1) - 1;
		else
			caddre = nPS - 1;
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

			if (dnprc <= -mprc && abs(dn) >= minmbs)
				def = dn;


			if (def < -0.01)
			{
				/**(pDefMB++) = def;
				*(pXdefMB++) = *(pXC + j-0);
				*(pYdefMB++) = *(pYC + j-0);*/


				//pbuf = (int *)malloc(sizeof(int) * (2 * bw + 1));
				//memcpy(pbuf, pwgS + j - bw, 2 * bw + 1);
				//qsort(pbuf, 2 * bw + 1, sizeof(int), compare);
				//wgmed = pbuf[bw];
				//free(pbuf);

				//prcGS = (wgs - wgmed) * 100.0 / (wgmed + 0.001);
				//// prcGS shouldn't increase prcReal, changed 25.07.2023
				//if (prcGS > 0.)
				//	prcGS = 0.;
				//prcReal = dnprc - prcGS;
				prcReal = dnprc;

				if (prcReal <= -mprc)
				{
					*(pDefMB + mbnum) = def;
					*(pXdefMB + mbnum) = *(pXCS + caddrb + jj - 0);
					*(pYdefMB + mbnum) = *(pYCS + caddrb + jj - 0);
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
	//HTuple PWidthS_tuple((float *)medoS, cnum * 1);
	//WriteTuple(PWidthS_tuple, "C:\\Samsung\\medoS_tuple.tup");
	////AfxMessageBox(_T("WidthS tuple saved!"));
}


