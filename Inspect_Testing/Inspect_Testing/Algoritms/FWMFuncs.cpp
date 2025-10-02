#ifndef USE_DLL
#include "Inspect_Common.h"
#endif

#include "FWMFuncs.h"

#if !defined(USE_DLL) || defined(FWMDLL_EXPORTS)


CFWM::CFWM()
{


}
CFWM::~CFWM()
{

}

void CFWM::Alloc2(int np, int nps)
{
	/// Pattern
	if (pWidth != 0)
	{
		delete(pWidth);
		delete(pWmin);
		delete(pPInt1x);
		delete(pPInt1y);
		delete(pPInt2x);
		delete(pPInt2y);
		delete(pShift);
		delete(pPuseint);

		delete(pWidthS);
		delete(pWminS);
		delete(pPInt1Sx);
		delete(pPInt1Sy);
		delete(pPInt2Sx);
		delete(pPInt2Sy);
		delete(pShiftS);
		delete(pPuseintS);

		delete(pmedoS);
		delete(pmedo);
	}

	/// Pattern
	pWidth = (float *)malloc(np * 1 * sizeof(float));
	pWmin = (float *)malloc(np * 1 * sizeof(float));
	pPInt1x = (float *)malloc(np * 2 * sizeof(UINT64));
	pPInt1y = (float *)malloc(np * 2 * sizeof(UINT64));

	pPInt2x = (float *)malloc(np * 2 * sizeof(UINT64));
	pPInt2y = (float *)malloc(np * 2 * sizeof(UINT64));
	pShift = (float *)malloc(np * 1 * sizeof(UINT64));
	pPuseint = (int *)malloc(np * 1 * sizeof(UINT64));

	/// Space
	pWidthS = (float *)malloc(nps * 1 * sizeof(float));
	pWminS = (float *)malloc(nps * 1 * sizeof(float));
	pPInt1Sx = (float *)malloc(nps * 2 * sizeof(UINT64));
	pPInt1Sy = (float *)malloc(nps * 2 * sizeof(UINT64));

	pPInt2Sx = (float *)malloc(nps * 2 * sizeof(UINT64));
	pPInt2Sy = (float *)malloc(nps * 2 * sizeof(UINT64));
	pShiftS = (float *)malloc(nps * 2 * sizeof(UINT64));
	pPuseintS = (int *)malloc(nps * 1 * sizeof(UINT64));

	pmedoS = (float *)malloc(MEDOSIZE * 1 * sizeof(float));

	pmedo = (float *)malloc(MEDOSIZE * 1 * sizeof(float));

}


void CFWM::Alloc(int np, int nps)
{
	/// Pattern
	if (pWidth == 0)
		pWidth = (float *)malloc(np * 1 * sizeof(float));
	if (pWmin == 0)
		pWmin = (float *)malloc(np * 1 * sizeof(float));
	if (pPInt1x == 0)
		pPInt1x = (float *)malloc(np * 2 * sizeof(UINT64));
	if (pPInt1y == 0)
		pPInt1y = (float *)malloc(np * 2 * sizeof(UINT64));

	if (pPInt2x == 0)
		pPInt2x = (float *)malloc(np * 2 * sizeof(UINT64));
	if (pPInt2y == 0)
		pPInt2y = (float *)malloc(np * 2 * sizeof(UINT64));
	if (pShift == 0)
		pShift = (float *)malloc(np * 1 * sizeof(UINT64));
	if (pPuseint == 0)
		pPuseint = (int *)malloc(np * 1 * sizeof(UINT64));
	/// Space
	if (pWidthS == 0)
		pWidthS = (float *)malloc(nps * 1 * sizeof(float));
	if (pWminS == 0)
		pWminS = (float *)malloc(nps * 1 * sizeof(float));
	if (pPInt1Sx == 0)
		pPInt1Sx = (float *)malloc(nps * 2 * sizeof(UINT64));
	if (pPInt1Sy == 0)
		pPInt1Sy = (float *)malloc(nps * 2 * sizeof(UINT64));

	if (pPInt2Sx == 0)
		pPInt2Sx = (float *)malloc(nps * 2 * sizeof(UINT64));
	if (pPInt2Sy == 0)
		pPInt2Sy = (float *)malloc(nps * 2 * sizeof(UINT64));
	if (pShiftS == 0)
		pShiftS = (float *)malloc(nps * 2 * sizeof(UINT64));
	if (pPuseintS == 0)
		pPuseintS = (int *)malloc(nps * 1 * sizeof(UINT64));

	if (pmedoS != NULL)
	{
		free(pmedoS);
		pmedoS = NULL;
	}
	pmedoS = (float *)malloc(MEDOSIZE * 1 * sizeof(float));

	if (pmedo != NULL)
	{
		free(pmedo);
		pmedo = NULL;
	}
	pmedo = (float *)malloc(MEDOSIZE * 1 * sizeof(float));

}

// new version for space 20220908 
void CFWM::SubPixCrdS(int *g0, int i0, int *PPx, int *PPy, float *pIntX, float *pIntY, float thr)
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
		*pIntX = (float)Px[jmax];
		*pIntY = (float)Py[jmax];
	}
	else if (gmin > thr)
	{
		*pIntX = (float)Px[jmin];
		*pIntY = (float)Py[jmin];
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
			*pIntX = (float)((Px[jmid] + Px[jmin]) / 2.);
			*pIntX = (float)((Py[jmid] + Py[jmin]) / 2.);
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
			*pIntX = (float)((Px[jmid] + Px[jmax]) / 2.);
			*pIntY = (float)((Py[jmid] + Py[jmax]) / 2.);
		}
	}
}

float CFWM::SubpixS1sq(UINT8 *pIm, int i0, int *PPx, int *PPy, float thr, int imw)
{
	int gmid, gmm, gpp, g0m, gm0, g0p, gp0, gp, gm;
	float dp, rp, dm, rm, w;
	
	gmid = *(pIm + PPy[i0] * imw + PPx[i0]);
	gmm = *(pIm + PPy[i0 - 1] * imw + PPx[i0 - 1]);
	gm0 = *(pIm + PPy[i0 - 1] * imw + PPx[i0]);
	g0m = *(pIm + PPy[i0] * imw + PPx[i0 - 1]);
	gpp = *(pIm + PPy[i0 + 1] * imw + PPx[i0 + 1]);
	gp0 = *(pIm + PPy[i0 + 1] * imw + PPx[i0]);
	g0p = *(pIm + PPy[i0] * imw + PPx[i0 + 1]);

	dp = sqrt((PPy[i0] - PPy[i0 + 1])*(PPy[i0] - PPy[i0 + 1]) + (PPx[i0] - PPx[i0 + 1])*(PPx[i0] - PPx[i0 + 1]));
	if (dp == 1.)
		rp = (gmid - thr) / (gmid - gpp);
	else
	{
		gp = (gp0 + g0p) / 2.;
		if (gp < thr)
			rp = 0.707*(1. + (thr - gp) / (gpp - gp));
		else
			rp = 0.707*(abs(thr - gmid) / (gp - gmid));
		
	}
	dm = sqrt((PPy[i0] - PPy[i0 - 1])*(PPy[i0] - PPy[i0 - 1]) + (PPx[i0] - PPx[i0 - 1])*(PPx[i0] - PPx[i0 - 1]));
	if (dm == 1.)
		rm = (gmid - thr) / (gmid - gpp);
	else
	{
		gm = (gm0 + g0m) / 2.;
		if (gm < thr)
			rm = 0.707*(1. + (thr - gm) / (gpp - gm));
		else
			rm = 0.707*(abs(thr - gmid) / (gm - gmid));
	}
	w = rp + rm;

	return(w);
}





// // new version for Space  2 Pnum (from Pat 36)
void CFWM::CPU_SpMeasGray(UINT8 *pIm, int imw, int imh, int pn,
	float *dP1x, float *dP1y, float *dP2x, float *dP2y,
	float *dGraymin)

{
	int x1, y1, x2, y2, xc, yc, pnum, pnum2/*, useint*/;
	int v, vp, vn, /*v1, v2, */i, j;
	float gmin, gmin2, gmin3, gsel, gminp;		 // min g value
	float shift = 0.0, xmid, ymid; // for shift calculation
	int PPx[128];  // Bresenham's MS points array
	int PPy[128];
	int Pval[128]; // pnum values array for all MS points


	//int g, gp; // current and previous gray values
	int jSPdn;
	int jSPup;
	int cIdx; // MS index

	float M1[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int iM1[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };
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
	//int vpnums;
	//** Intersection point IRBS val,
	int	PIRBS[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };
	//int irbs;
	float ixc;
	float iyc;
	int ptype = 0, begtype = 0, pnfound = 0, btype;
	int PB[8];
	int PE[8];
	int ib = 0;
	int ie = 0;
	//int cIdx = threadIdx.x + blockIdx.x*blockDim.x;
	//int tid = blockDim.x * blockIdx.x + threadIdx.x;
	gminp = 150;
	gsel = -1;
	for (cIdx = 0; cIdx <  pn; cIdx++)
	{
		ib = 0;
		ie = 0;
	//	if (cIdx == 6521)
	//		cIdx = cIdx;
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

		int ii = 0, /*clc, */pclc = -2, fi1 = 0;
		//pnum = (*(dPnum + cIdx));
		//pnum2 = (*(dPnum2 + cIdx));
		//useint = (*(dPuseint + cIdx));
		x1 = (int)round(*(dP1x + cIdx)); // getting measurement sensor endpoints
		y1 = (int)round(*(dP1y + cIdx));
		x2 = (int)round(*(dP2x + cIdx));
		y2 = (int)round(*(dP2y + cIdx));
		ixc = (float)((x1 + x2) / 2.);
		iyc = (float)((y1 + y2) / 2.);

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
		gmin = gmin2 = gmin3 =255.;
		while (x1 != x2 || y1 != y2)
		{
			PPx[pind] = x1;
			PPy[pind] = y1;
			v = *(pIm + y1 * imw + x1);
			Pval[pind] = v;
			if (gmin > v)
			{
				gmin3 = gmin2;
				gmin2 = gmin;
				gmin = v;
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
			if (pind < 127)
				pind++;
			else
				break;
		} // while
		// Going forward from PS1

		if (gmin2 < 240)
			gsel = gmin2;
		//else 
		//	if(gsel = gmin+10 >250)
		//		gsel=250;
		//if (gmin > 250 && gmin2 > 250 && gmin3 > 250)
		//	gsel = gminp;
		////else if (gmin < 200)
		//else if (gmin < gminp)
		//	gminp = gsel;

		*(dGraymin + cIdx) = gsel;
		if (gsel > 200)
			gsel = gsel;
		
		//********************************************	
	} // index cycle end
}

// // new version for Space  2 Pnum (from Pat 36)
void CFWM::CPU_MSSpProc36B2(UINT8 *pIm, UINT16 *pImIRBS, UINT16 *pImCLCS, int imw, int imh, int pst, int pn,
	float thr, int dclcmax, float *dP1x, float *dP1y, float *dP2x, float *dP2y, int *dPnum, int *dPnum2,
	float *dW, float *dWmin, float *dPint1x, float *dPint1y, float *dPint2x, float *dPint2y, float *dShift, int *dPuseint)
{
	int x1, y1, x2, y2, xc, yc, pnum, pnum2/*, useint*/;
	int v, vp, vn, /*v1, v2, */i, j;
	float wmin, wmax;		 // for width calculation
	float shift = 0.0, xmid, ymid; // for shift calculation
	int PPx[128];  // Bresenham's MS points array
	int PPy[128];
	int Pval[128]; // pnum values array for all MS points


	//int g, gp; // current and previous gray values
	int jSPdn;
	int jSPup;
	int cIdx; // MS index

	float M1[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int iM1[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };
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
	//int vpnums;
	//** Intersection point IRBS val,
	int	PIRBS[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };
	//int irbs;
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
		if (cIdx == 6521)
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

		int ii = 0, /*clc, */pclc = -2, fi1 = 0;
		pnum = (*(dPnum + cIdx));
		pnum2 = (*(dPnum2 + cIdx));
		if(pnum2==pnum)
			pnum = (*(dPnum + cIdx-1));
		//useint = (*(dPuseint + cIdx));
		x1 = (int)round(*(dP1x + cIdx)); // getting measurement sensor endpoints
		y1 = (int)round(*(dP1y + cIdx));
		x2 = (int)round(*(dP2x + cIdx));
		y2 = (int)round(*(dP2y + cIdx));
		ixc = (float)((x1 + x2) / 2.);
		iyc = (float)((y1 + y2) / 2.);

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
				PIntY[ii] = (float)yc;
				PIntX[ii] = (float)xc;
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
			//btype = 0;
			if (v == pnum || v == pnum2)
			{
				if (vp == 0 && (vn == pnum || vn == pnum2 || vn == 1))
					btype = 1;
				else if ((vp == 1 && (vp != pnum || vp != pnum2)) || (vn == 0 && vp != 0))
					btype = -1;
				// 240315
				else if (vp == 0 && vn == 0)
					btype = 2;//
				else
				{
					btype = 0;

				}
				PType[ii] = btype;
				pnfound = 1;
				if (btype == 0)
					continue;
				PIntY[ii] = (float)yc;
				PIntX[ii] = (float)xc;
				PInti[ii] = i;
				//Psubpix[ii] = 1;
				//PCLC[ii] = clc;
				if (ii < 7)
					ii = ii + 1;
			}
			if (i == pind - 1 && v == 0 && pnfound == 1)
			{
				PIntY[ii] = (float)yc;
				PIntX[ii] = (float)xc;
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
		int /*dclc, */dmin = 55555; // min CLC distance
		//int d, d1, d2; // CLC distances
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

		if (intnum == 0) //** single pix. space case
		{
			wmin = 0.;
			wmax = 0.;
			jSPdn = 0;
			jSPup = 0;
		}
		else if (intnum == 1)
		{	// 240315
			if (PType[0] == 2)
			{
				int i0 = PInti[0];
				wmax = SubpixS1sq(pIm, i0, PPx, PPy, thr, imw);
				if (wmax < -0.01)
					wmax = 0.0;
				wmin = wmax;
				jSPdn = 0;
				jSPup = 0;
			}
			else
			{
				if (int irbs = *(pImIRBS + y2 * imw + x2) == 1)  // IRBSC pixel value)
				{
					wmin = 10.;
					wmax = 10.;
					jSPdn = 0;
					jSPup = 0;
				}
				else
				{
					wmin = 1.;
					wmax = 1.;
					jSPdn = 0;
					jSPup = 0;
				}

			}

		}
		else
		{ ///**** NORMAL CASE ******
	//**************** SUBPIX COORD. ******************************
			for (j = 0; j < ii; j++)
			{
				//if (PType[j] == 1)  //24.07.12
				if (PType[j] >= 1)
					PB[ib++] = j;
				//ib : = ib + 1
				else if (PType[j] == -1)
					PE[ie++] = j;
				//ie : = ie + 1
				if (PType[j] == 2 && ie>0)
					PE[ie-1] = j;
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
						fIntX = (float)PPx[PInti[j]];
						fIntY = (float)PPy[PInti[j]];
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
				xmid = (float)((FIntX[jSPup] + FIntX[jSPdn]) / 2.);
				ymid = (float)((FIntY[jSPup] + FIntY[jSPdn]) / 2.);
				shift = sqrt((ixc - xmid)*(ixc - xmid) + (iyc - ymid)*(iyc - ymid));
			}

			//} // **** normal case end ***


		}
		//if (wmax < -0.1)
		//	wmax = wmax;
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
void CFWM::SubPixCrdP(int *g0, int i0, int *PPx, int *PPy, float *pIntX, float *pIntY, float thr)
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
		*pIntX = (float)Px[jmax];
		*pIntY = (float)Py[jmax];
	}
	else if (gmin > thr)
	{
		*pIntX = (float)Px[jmin];
		*pIntY = (float)Py[jmin];
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
			*pIntX = (float)((Px[jmid] + Px[jmin]) / 2.);
			*pIntY = (float)((Py[jmid] + Py[jmin]) / 2.);
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
			*pIntX = (float)((Px[jmid] + Px[jmax]) / 2.);
			*pIntY = (float)((Py[jmid] + Py[jmax]) / 2.);
		}
	}
}

void CFWM::CPU_MSPtProc40B2(UINT8 *pIm, UINT16 *pImIRB, UINT16 *pImCLC, int imw, int imh, int pst, int pn,
	float thr, int dclcmax, float *dP1x, float *dP1y, float *dP2x, float *dP2y, int *dPnum, int *dPnum2,
	float *dW, float *dWmin, float *dPint1x, float *dPint1y, float *dPint2x, float *dPint2y, float *dShift, int *dPuseint)
{
	int x1, y1, x2, y2, xc, yc, pnum, pnum2/*, useint*/;
	int v, vp, vn, /*v1, v2, */i, j;
	float wmin, wmax;		 // for width calculation
	float shift = 0.0, xmid, ymid; // for shift calculation
	int PPx[256];  // Bresenham's MS points array
	int PPy[256];
	int Pval[256]; // pnum values array for all MS points


	//int g, gp; // current and previous gray values
	int jSPdn;
	int jSPup;
	int jMBdn;
	int jMBup;
	int cIdx; // MS index
	int dif0;
	int dif1;

	float M1[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int iM1[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };
	int iM0[8] = { 0,0,0,0,0,0,0,0 };
	float PIntX[16] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1.,-1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. }; // intersection points coordinates
	float PIntY[16] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1.,-1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	float FIntX[16] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1.,-1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. }; // intersection points coordinates (subpixel)
	float FIntY[16] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1.,-1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	float fIntX, fIntY; // subpix coord. measured by 
	int Psubpix[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	int PInti[16] = { -1,-1,-1,-1,-1,-1,-1,-1,-1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };	  // index array of intersection points 
	int PCLC[16] = { -1,-1,-1,-1,-1,-1,-1,-1,-1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };   // CLC array of intersection points 
	int PType[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	int Pnums[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	//int vpnums;
	//** Intersection point IRBS val,
	int	PIRBS[16] = { -1,-1,-1,-1,-1,-1,-1,-1,-1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	//int irbs;
	float ixc;
	float iyc;
	int ptype = 0, begtype = 0, pnfound = 0, btype;
	int pin = 1;
	int PB[32];
	int PE[32];
	int ib = 0;
	int ie = 0;
	int intnum;
	//int cIdx = threadIdx.x + blockIdx.x*blockDim.x;
	//int tid = blockDim.x * blockIdx.x + threadIdx.x;
	for (cIdx = pst + pn - 0; cIdx < pst + pn; cIdx++)
	{
		*(dW + cIdx) = 0.;
		*(dWmin + cIdx) = 0.;
	}
	for (cIdx = pst; cIdx < pst + pn-0; cIdx++)
	{
		ib = 0;
		ie = 0;
		if (cIdx == 2)
			cIdx = cIdx;
		memcpy(&PIntX[0], &M1[0], 16 * sizeof(float));
		memcpy(&PIntY[0], &M1[0], 16 * sizeof(float));
		memcpy(&FIntX[0], &M1[0], 16 * sizeof(float));
		memcpy(&FIntY[0], &M1[0], 16 * sizeof(float));
		memcpy(&PInti[0], &iM1[0], 16 * sizeof(int));
		memcpy(&PCLC[0], &iM1[0], 16 * sizeof(int));
		memcpy(&Psubpix[0], &iM0[0], 16 * sizeof(int));
		memcpy(&PType[0], &iM0[0], 16 * sizeof(int));
		//fw = *(d_P1xl + cIdx);
		//*(d_Wl + cIdx) = fw;
		//if (cIdx == 1883423)
		//if (cIdx == 157421)
		/*if (cIdx == 162006)
			cIdx = cIdx;*/
			//int ii = 0;

		int ii = 0/*, clc*/, pclc = -2, fi1 = 0/*, ip*/;
		int skipnext = 0;
		pnum = (*(dPnum + cIdx));
		pnum2 = (*(dPnum2 + cIdx));
		if (pnum2 == pnum)
			pnum2 = (*(dPnum + cIdx-1));

		//useint = (*(dPuseint + cIdx));
		x1 = (int)round(*(dP1x + cIdx)); // getting measurement sensor endpoints
		y1 = (int)round(*(dP1y + cIdx));
		x2 = (int)round(*(dP2x + cIdx));
		y2 = (int)round(*(dP2y + cIdx));
		ixc = (float)((x1 + x2) / 2.);
		iyc = (float)((y1 + y2) / 2.);
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
			if (pind < 255)
				pind++;
			else
				break;
		} // while

		  // Going forward from PS1
		ptype = begtype = pnfound = 0;
		pin = 1;
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
			if (v == 0)
				pin = 0;
			//irbs = *(pImIRBS + yc * imw + xc);  // IRBSC pixel value
			if (i == 1 && v == 1)
			{
				ptype = 1;
				PIntY[ii] = (float)yc;
				PIntX[ii] = (float)xc;
				PInti[ii] = i;
				PCLC[ii] = -2;
				begtype = 1;
				if (ii < 15)
					ii = ii + 1;
			}
			if (i == pind - 2 && v == 1)
			{ //**** inside of Spur ***
				ptype = -1;
				PIntY[ii] = (float)yc;
				PIntX[ii] = (float)xc;
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
				if (ii > 1)
					ii = ii - 1;
			}
			//btype = 0;
			if (v == pnum || v == pnum2)
			{
				if (pin == 1)
				{
					PIntY[ii] = PPy[1];
					PIntX[ii] = PPx[1];
					PInti[ii] = 1;
					PCLC[ii] = -2;
					PType[ii] = 1;
					if (ii < 15)
						ii = ii + 1;
				}
				//*** 2 points ******
				//* changed index ip
				//ip = ii;
				//if (((vp == pnum || vp == pnum2) && (i != 1)) || ((vn == pnum || vn == pnum2) && (i != 1)))
				if ((i != pind - 2) && ((vp == pnum || vn == pnum) && (i != 1) || (vp == pnum2 || vn == pnum2) && (i != 1)))

					continue;
				//else if (vp == 0 && (vn == pnum || vn == pnum2 || vn == 1 || vn == 0))
				else if ((vp == 0 || vp > 1 && vp != pnum && vp != pnum2) && (vn == pnum || vn == pnum2 || vn == 1 || vn == 0))
					btype = 1;
				else if ((vp == 1 && vp != pnum && vp != pnum2) || (vn == 0 && vp != 0))
					btype = -1;
				else if ((vn == 1 && ((vp == pnum) || vp == pnum2)) || (vp == 1 && (vn == pnum || vn == pnum2)))
					btype = 1;
				else if (vn == 1 && (pnfound == 0))
					btype = 1;
				else
				{
					btype = 0;
					//continue;
				}
				if (btype == 1)
				{
					pnfound = 1;
				}
				PType[ii] = btype;
				//pnfound = 1;
				if (btype == 0)
				{
					continue;
				}
				PIntY[ii] = (float)yc;
				PIntX[ii] = (float)xc;
				PInti[ii] = i;
				//Psubpix[ii] = 1;
				if (ii < 15)
					ii = ii + 1;
			}
			if ( (i==pind-2) && ( (v == 1 || v == pnum || v == pnum2) && pnfound == 1))
			{
				PIntY[ii] = (float)yc;
				PIntX[ii] = (float)xc;
				PInti[ii] = i;
				PCLC[ii] = -2;
				PType[ii] = -1;
				if (ii < 15)
					ii = ii + 1;
			}
			vp = v;
		}
		//********************************************************************
		int ga[3]; // array of 3 neighboring g
		int nmwc = 0;//*number of MW (main wire) candidates
		int mw = -1;//* MW segment
		int /*dclc, */dmin = 55555; // min CLC distance
		//int d, d1, d2; // CLC distances
		intnum = ii; /// number of intersection points found
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
			wmax = 0.0;
			wmin = 0.0;
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

			if (ib == 0 || ie == 0)
			{
				jMBdn = jSPdn = 0;
				jMBup = jSPup = ii - 1;
			}
			else
			{
				// ***Hong
				/*if (jSPdn == 0 || jSPdn == 1 || jSPdn == 2)
					jSPdn = jSPdn;
				else
					jSPdn = intnum - 2;*/
					// *****			



				if (intnum <= 3)
				{
					jMBdn = jSPdn;
					jMBup = jSPup;
				}
				else if (intnum == 4)
				{
					if (ib < 2 || ie < 2)
					{
						jMBdn = jSPdn;
						jMBup = jSPup;
					}
					else
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

				}
				else
				{
					jMBdn = jSPdn;
					jMBup = jSPup;
				}
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
			if (ib == 0 || ie == 0)
			{
				jSPdn = 0;
				jSPup = ii - 1;
			}
			else
			{
				jSPdn = PB[0];
				jSPup = PE[ie - 1];
			}

			/*if (jSPup == 0 || jSPup == 1 || jSPup == 2)
			{
				jSPup = jSPup;
			}
			else
			{
				jSPup = intnum - 1;
			}*/
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
						fIntX = (float)PPx[PInti[j]];
						fIntY = (float)PPy[PInti[j]];
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
				xmid = (float)((FIntX[jSPup] + FIntX[jSPdn]) / 2.);
				ymid = (float)((FIntY[jSPup] + FIntY[jSPdn]) / 2.);
				shift = sqrt((ixc - xmid)*(ixc - xmid) + (iyc - ymid)*(iyc - ymid));
			}

		} // **** normal case end ***

		//if (wmax < 2.0)
		//	wmax = wmax;
		//

		//*(dW + cIdx) = wmax;
		//*(dWmin + cIdx) = wmin;
		//*(dShift + cIdx) = shift;

		float intux, intuy, intdx, intdy, int1x, int1y, int2x, int2y, du2, dd2, dm2;
		intux = PIntX[jSPup];
		intuy = PIntY[jSPup];
		intdx = PIntX[jSPdn];
		intdy = PIntY[jSPdn];

		du2 = (intux - ixc)*(intux - ixc) + (intuy - iyc)*(intuy - iyc);
		dd2 = (intdx - ixc)*(intdx - ixc) + (intdy - iyc)*(intdy - iyc);
		dm2 = (du2 + dd2) / 2.;

		if (du2 >= dm2 * 1.5)
		{
			int1x = intux;
			int1y = intuy;
			int2x = intdx;
			int2y = intdy;
		}
		else if (dd2 >= dm2 * 1.5)
		{
			int1x = intdx;
			int1y = intdy;
			int2x = intux;
			int2y = intuy;
		}
		else
		{
			int1x = ixc;
			int1y = iyc;
			int2x = ixc;
			int2y = iyc;
		}


		*(dW + cIdx) = wmax;
		*(dWmin + cIdx) = wmin;
		*(dShift + cIdx) = shift;

		*(dPint1x + cIdx) = int1x;
		*(dPint2x + cIdx) = int2x;

		*(dPint1y + cIdx) = int1y;
		*(dPint2y + cIdx) = int2y;

		/**(dPint1x + cIdx) = PIntX[jSPup];
		*(dPint2x + cIdx) = PIntX[jSPdn];

		*(dPint1y + cIdx) = PIntY[jSPup];
		*(dPint2y + cIdx) = PIntY[jSPdn];*/

		/// saving number of intersection points ( 2 points) (left name Puseint for simplicity of changes)
		*(dPuseint + cIdx) = intnum;
		//********************************************	
	} // index cycle end
}

// new version for Pattern 231024 -fixed multiple pnums in a row, fixed neighboring same region wires
void CFWM::CPU_MSPtProc39B2(UINT8 *pIm, UINT16 *pImIRB, UINT16 *pImCLC, int imw, int imh, int pst, int pn,
	float thr, int dclcmax, float *dP1x, float *dP1y, float *dP2x, float *dP2y, int *dPnum, int *dPnum2,
	float *dW, float *dWmin, float *dPint1x, float *dPint1y, float *dPint2x, float *dPint2y, float *dShift, int *dPuseint)
{
	int x1, y1, x2, y2, xc, yc, pnum, pnum2/*, useint*/;
	int v, vp, vn, /*v1, v2, */i, j;
	float wmin, wmax;		 // for width calculation
	float shift = 0.0, xmid, ymid; // for shift calculation
	int PPx[128];  // Bresenham's MS points array
	int PPy[128];
	int Pval[128]; // pnum values array for all MS points


	//int g, gp; // current and previous gray values
	int jSPdn;
	int jSPup;
	int jMBdn;
	int jMBup;
	int cIdx; // MS index
	int dif0;
	int dif1;

	float M1[8] = { -1.,-1.,-1.,-1.,-1.,-1.,-1.,-1. };
	int iM1[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };
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
	//int vpnums;
	//** Intersection point IRBS val,
	int	PIRBS[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };
	//int irbs;
	float ixc;
	float iyc;
	int ptype = 0, begtype = 0, pnfound = 0, btype;
	int pin = 1;
	int PB[16];
	int PE[16];
	int ib = 0;
	int ie = 0;
	//int cIdx = threadIdx.x + blockIdx.x*blockDim.x;
	//int tid = blockDim.x * blockIdx.x + threadIdx.x;
	for (cIdx = pst; cIdx < pst + pn; cIdx++)
	{
		ib = 0;
		ie = 0;
		if (cIdx == 2777)
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

		int ii = 0/*, clc*/, pclc = -2, fi1 = 0/*, ip*/;
		int skipnext = 0;
		pnum = (*(dPnum + cIdx));
		pnum2 = (*(dPnum2 + cIdx));
		//useint = (*(dPuseint + cIdx));
		x1 = (int)round(*(dP1x + cIdx)); // getting measurement sensor endpoints
		y1 = (int)round(*(dP1y + cIdx));
		x2 = (int)round(*(dP2x + cIdx));
		y2 = (int)round(*(dP2y + cIdx));
		ixc = (float)((x1 + x2) / 2.);
		iyc = (float)((y1 + y2) / 2.);
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
			if (v == 0)
				pin = 0;
			//irbs = *(pImIRBS + yc * imw + xc);  // IRBSC pixel value
			if (i == 1 && v == 1)
			{
				ptype = 1;
				PIntY[ii] = (float)yc;
				PIntX[ii] = (float)xc;
				PInti[ii] = i;
				PCLC[ii] = -2;
				begtype = 1;
				ii = ii + 1;
			}
			if (i == pind - 2 && v == 1)
			{ //**** inside of Spur ***
				ptype = -1;
				PIntY[ii] = (float)yc;
				PIntX[ii] = (float)xc;
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
				if (pin == 1)
				{
					PIntY[ii] = PPy[1];
					PIntX[ii] = PPx[1];
					PInti[ii] = 1;
					PCLC[ii] = -2;
					PType[ii] = 1;
					ii = ii + 1;
				}
				//*** 2 points ******
				//* changed index ip
				//ip = ii;
				if (((vp == pnum || vp == pnum2)&&(i!=1)) || ((vn == pnum || vn == pnum2) && (i != 1)))
					continue;
				//else if (vp == 0 && (vn == pnum || vn == pnum2 || vn == 1 || vn == 0))
				else if ((vp == 0 ||  vp>1 && vp!=pnum && vp != pnum2) && (vn == pnum || vn == pnum2 || vn == 1 || vn == 0))
					btype = 1;
				else if ((vp == 1 && vp != pnum && vp != pnum2) || (vn == 0 && vp != 0))
					btype = -1;
				else if ((vn == 1 && ((vp == pnum) || vp == pnum2)) || (vp == 1 && (vn == pnum || vn == pnum2)))
					btype = 1;
				/*else if(vn == 1 && (pnfound == 0))
					btype = 1;*/
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
				PIntY[ii] = (float)yc;
				PIntX[ii] = (float)xc;
				PInti[ii] = i;
				//Psubpix[ii] = 1;
				if (ii < 7)
					ii = ii + 1;
			}
			if (i == pind - 2 && (v == 1 || v == pnum || v == pnum2) && pnfound == 1)
			{
				PIntY[ii] = (float)yc;
				PIntX[ii] = (float)xc;
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
		int /*dclc, */dmin = 55555; // min CLC distance
		//int d, d1, d2; // CLC distances
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
			wmax = 0.0;
			wmin = 0.0;
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

			if (ib == 0 || ie == 0)
			{
				jMBdn = jSPdn = 0;
				jMBup = jSPup = ii-1;
			}
			else
			{
				// ***Hong
				/*if (jSPdn == 0 || jSPdn == 1 || jSPdn == 2)
					jSPdn = jSPdn;
				else
					jSPdn = intnum - 2;*/
					// *****			



				if (intnum <= 3)
				{
					jMBdn = jSPdn;
					jMBup = jSPup;
				}
				else if (intnum == 4)
				{
					if(ib<2 || ie<2)
					{
						jMBdn = jSPdn;
						jMBup = jSPup;
					}
					else
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
					
				}
				else
				{
					jMBdn = jSPdn;
					jMBup = jSPup;
				}
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
			if (ib == 0 || ie == 0)
			{
				jSPdn = 0;
				jSPup = ii - 1;
			}
			else
			{
				jSPdn = PB[0];
				jSPup = PE[ie - 1];
			}
			
			/*if (jSPup == 0 || jSPup == 1 || jSPup == 2)
			{
				jSPup = jSPup;
			}
			else
			{
				jSPup = intnum - 1;
			}*/
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
						fIntX = (float)PPx[PInti[j]];
						fIntY = (float)PPy[PInti[j]];
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
				xmid = (float)((FIntX[jSPup] + FIntX[jSPdn]) / 2.);
				ymid = (float)((FIntY[jSPup] + FIntY[jSPdn]) / 2.);
				shift = sqrt((ixc - xmid)*(ixc - xmid) + (iyc - ymid)*(iyc - ymid));
			}

		} // **** normal case end ***

		float intux, intuy, intdx, intdy, int1x, int1y, int2x, int2y, du2, dd2, dm2;
		intux = PIntX[jSPup];
		intuy = PIntY[jSPup];
		intdx = PIntX[jSPdn];
		intdy = PIntY[jSPdn];

		du2 = (intux - ixc)*(intux - ixc) + (intuy - iyc)*(intuy - iyc);
		dd2 = (intdx - ixc)*(intdx - ixc) + (intdy - iyc)*(intdy - iyc);
		dm2 = (du2 + dd2) / 2.;
		
		if (du2 >= dm2*1.9)
		{
			int1x = intux;
			int1y = intuy;
			int2x = intdx;
			int2y = intdy;
		}
		else if(dd2 >= dm2 * 1.9)
		{
			int1x = intdx;
			int1y = intdy;
			int2x = intux;
			int2y = intuy;
		}
		else
		{
			int1x = ixc;
			int1y = iyc;
			int2x = ixc;
			int2y = iyc;
		}


		*(dW + cIdx) = wmax;
		*(dWmin + cIdx) = wmin;
		*(dShift + cIdx) = shift;

		*(dPint1x + cIdx) = int1x;
		*(dPint2x + cIdx) = int2x;

		*(dPint1y + cIdx) = int1y;
		*(dPint2y + cIdx) = int2y;

		/// saving number of intersection points ( 2 points) (left name Puseint for simplicity of changes)
		*(dPuseint + cIdx) = intnum;
		//********************************************	
	} // index cycle end
}

///////////////////////////////////////////////////////////
//			Running Median Filter
///////////////////////////////////////////////////////////

int CFWM::compare(const void* a, const void* b)
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
void CFWM::SrunmedSLIExt(float* yinp, float* y/*extended*/, float* smo, long n1, int bw_tot, int end_rule, int skip) // 2022.08.01 - running median with skipping [skip] values, with linear interpolation
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
				smo[i] = (float)rmed;
		}
	}
	smo[band2*step + shift] = (float)rmed;
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
		smo[ismo] = (float)rmed;
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
				smo[x] = (float)(fa + j * (fb - fa) / step); //b-a=step, x-a=j
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

void CFWM::SrunmedSLI(float* y, float* smo, long n, int bw_tot, int end_rule, int skip) // 2022.08.01 - running median with skipping [skip] values, with linear interpolation
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
		 *	smo(n)	- smoothed data output (should be allocated)

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
				smo[i] = (float)rmed;
		}
	}
	smo[band2*step + shift] = (float)rmed;
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
		smo[ismo] = (float)rmed;
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
				smo[x] = (float)(fa + j * (fb - fa) / step); //b-a=step, x-a=j
			}
		}
	}
} /* SrunmedSLI */

// FWMS defect types definition
#define FWMS_NONE	0

#define FWMS_NARROW 1
#define FWMS_MB_ABS	2
#define FWMS_MB_PRC	3
#define FWMS_SP_ABS	4
#define FWMS_SP_PRC	5
#define FWMS_SHORT	6

void CFWM::FWMS_PostProcCPUnew(float *pwS, int *pwgS, float *medoS, float *pXCS, float *pYCS, int naS, int nPS, int nvoid, int *pPaddrS, int sz1, int sz2, int skip, 
	float ang, float mprc, float sprc, float mabs, float sabs, float minmbs, float narWsp, int dlen, 
	float *pDefMB, float *pDefMBprc, float *pXdefMB, float *pYdefMB, float *pDefSP, float *pDefSPprc, float *pXdefSP, float *pYdefSP, int *MBnumS, int *SPnumS)
{
	int caddrb, caddre, cnum;
	int mbnum, spnum, j, jj/*, i*/;
	//float v;

	//int spnum = 0, mbnum = 0;
	//int wgmed;
	//int *pbuf;
	float /*prcGS, */prcReal;


	//

	float ws, wgs, wns;
	float dg, dgprc; // deviation from CAD
	float dn, dnprc; // deviation from neighbor
	float def, defprc; // defect size
	float defx, defy;

	float defspmax = 0., defspspprcmax = 0., defspspsum = 0;  /// for % sp
	int defspspact = 0, defspspcnt = 0;

	float defspmbmax = 0., defspmbprcmax = 0., defspmbsum = 0;  /// for % mb
	int defspmbact = 0, defspmbcnt = 0;
	int deftype, defin;
	//*MBnumS = 0;
	mbnum = 0;
	spnum = 0;
	defx = defy = 0.;
	for (j = 0; j < naS - 0; j++)
	{
		defspmbmax = 0.; defspmbprcmax = 0.;
		defspmbact = defspmbact = 0;

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
		SrunmedSLI(pwS + caddrb + 0, medoS, cnum - 0, bw, 0, skip); // Running median with skip and linear interp.
		//SrunmedSLIExtNoS(pw + caddrb, medo + MEDOSIZE / 2, medo, n, bw, skip, 0/*10*/);
																//SrunmedSLIExt(pwS, medoS + MEDOSIZE / 2, medoS, cnum, bw, 1, skip); // Running median with Extension at the ends, skip and linear interp.
		//SrunmedSLIExt(pwS + caddrb, medoS + MEDOSIZE / 2, medoS, cnum, bw, 0, skip); // Running median with Extension at the ends, skip and linear interp.
		//if (j==0 )
		//{
		//	HTuple PWidthSmedo_tuple((float*)medoS, cnum-10);
		//	WriteTuple(PWidthSmedo_tuple, "C:\\Temp\\PWidthSmedo_tuple.tup");
		//	//HTuple PWidthS_tuple((float *)medoS, cnum * 1);
		////WriteTuple(PWidthS_tuple, "C:\\Samsung\\medoS_tuple.tup");
		//}
		int je = cnum - 0;
		for (jj = 0; jj <= je; jj++)
		{
			/*if (j == 1 && jj == 9610)
				jj = jj;*/

			//if (j == 0 && jj == 1219)
			//	j = j;
			ws = *(pwS + caddrb + jj);
			if (ws < 6)
				ws = ws;
				/*if (ws <= 3.5)
					ws = ws;*/
			wgs = (float)*(pwgS + caddrb + jj);
			wns = (float)*(medoS + jj);

			dg = ws - wgs;
			dn = ws - wns;

			dgprc = (float)(dg * 100. / wgs);
			dnprc = (float)(dn * 100. / (wns + 0.001));

			def = defprc=0.;
			//deftype = FWMS_NONE;
			defin = 0;
			// dn - defect size; dnprc - defect %, ws - width, wns - local width
			// Defects detection
			//if (wns > minmbs) // % (rel.) criterion 
			if (wgs > minmbs && ws> narWsp && -dn < mabs) // % (rel.) criterion 
			{
				if (dnprc <= -mprc)
				{
					def = -dn;
					defprc = dnprc;
					deftype = FWMS_MB_PRC;
					defin = 1;
				}
				else if (dnprc >= sprc)
				{
					def = dn;
					defprc = dnprc;
					deftype = FWMS_SP_PRC;
					defin = 1;
				}

			}
			//else if (wns > minmbs) //abs.criterion ... if width is smaller than minmbs, then can't use % based detection!
			else if (dn <= -mabs || dn >= sabs) // small space mb
			{
				if (dn <= -mabs)
				{
					def = -dn;
					defprc = 100.;
					deftype = FWMS_MB_ABS;
					defin = 1;
				}
				else if (dn >= sabs)
				{
					def = dn;
					defprc = 100.;
					deftype = FWMS_SP_ABS;
					defin = 1;
				}
			}
			else if(ws < 0.01) // Short detected in FWMS processing
			{
				def = -5.0505;  /// special value for Short
				defprc = -100.;
				deftype = FWMS_SHORT;
				defin = 1;
			}
			else if (ws < narWsp) // too narrow space detected in FWMS processing
			{
				def = ws;  /// width
				defprc = -100.;
				deftype = FWMS_NARROW;
				defin = 1;
				if (jj == je)
				{  // for the case when the whole coil space is narrow
					defspmbact = 2;
					defin = 2;
				}
					
			}
		// saving TMS defects	
			if (defin == 1)
			{
				if (defspmbact == 0)
				{
					defspmbact = 1; // start
					defspmbmax = def;
					defspmbprcmax = dnprc;
					defx = *(pXCS + caddrb + jj - 0);
					defy = *(pYCS + caddrb + jj - 0);
				}
				else
				{
					if (abs(dnprc) > defspmbmax)
					{
						defspmbmax = def;
						defspmbprcmax = abs(dnprc);
						defx = *(pXCS + caddrb + jj - 0);
						defy = *(pYCS + caddrb + jj - 0);
					}
					/*if (dnprc < defspmbmin)
					{
						defspmbmax = def;
						defspmbprcmax = dnprc;
					}*/
					defspmbact = 2; // continue 
				}
			}
			else
			{
				if (defspmbact == 2)
				{
					if (deftype == FWMS_MB_ABS || deftype == FWMS_MB_PRC || deftype == FWMS_NARROW)
					{
						*(pDefMB + mbnum) = defspmbmax;
						*(pDefMBprc + mbnum) = defspmbprcmax;
						*(pXdefMB + mbnum) = defx;
						*(pYdefMB + mbnum) = defy;
						mbnum++;
					}
					else
					{
						*(pDefSP+ spnum) = defspmbmax;
						*(pDefSPprc + spnum) = defspmbprcmax;
						*(pXdefSP + spnum) = defx;
						*(pYdefSP + spnum) = defy;
						spnum++;
					}
	
				}
				defspmbact = 0;
				defin = 0;
			}
				
				

		
		} // for jj

	} // for j
	*MBnumS = mbnum;
	*SPnumS = spnum;
	//j = j;
	//HTuple PWidthS_tuple((float *)medoS, cnum * 1);
	//WriteTuple(PWidthS_tuple, "C:\\Samsung\\medoS_tuple.tup");
	////AfxMessageBox(_T("WidthS tuple saved!"));
}


void CFWM::FWMS_PostProcCPU(float *pwS, int *pwgS, float *medoS, float *pXCS, float *pYCS, int naS, int nPS, int nvoid, int *pPaddrS, int sz1, int sz2, int skip, float ang, float mprc, float sprc, float mabs, float sabs, float minmbs, float minWsp, int dlen, float *pDefMB, float *pDefMBprc, float *pXdefMB, float *pYdefMB, float *pDefSP, float *pXdefSP, float *pYdefSP, float *pYdefSPprc, int *MBnumS, int *SPnumS)
{
	int caddrb, caddre, cnum;
	int mbnum, spnum, j, jj/*, i*/;
	//float v;

	//int spnum = 0, mbnum = 0;
	//int wgmed;
	//int *pbuf;
	float /*prcGS, */prcReal;


	//

	float ws, wgs, wns;
	float dg, dgprc; // deviation from CAD
	float dn, dnprc; // deviation from neighbor
	float def; // defect size

	float defspmax = 0., defspspprcmax = 0., defspspsum = 0;  /// for % sp
	int defspspact = 0, defspspcnt = 0;

	float defspmbmax = 0., defspmbprcmax = 0., defspmbsum = 0;  /// for % mb
	int defspmbact = 0, defspmbcnt = 0;

	//*MBnumS = 0;
	mbnum = 0;
	spnum = 0;
	for (j = 0; j < naS - 0; j++)
	{
		defspmbmax = 0.; defspmbprcmax = 0.;
		defspmbact = defspmbact = 0;

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
		SrunmedSLI(pwS + caddrb+0, medoS, cnum-0, bw, 0, skip); // Running median with skip and linear interp.
		//SrunmedSLIExtNoS(pw + caddrb, medo + MEDOSIZE / 2, medo, n, bw, skip, 0/*10*/);
																//SrunmedSLIExt(pwS, medoS + MEDOSIZE / 2, medoS, cnum, bw, 1, skip); // Running median with Extension at the ends, skip and linear interp.
		//SrunmedSLIExt(pwS + caddrb, medoS + MEDOSIZE / 2, medoS, cnum, bw, 0, skip); // Running median with Extension at the ends, skip and linear interp.
		//if (j==0 )
		//{
		//	HTuple PWidthSmedo_tuple((float*)medoS, cnum-10);
		//	WriteTuple(PWidthSmedo_tuple, "C:\\Temp\\PWidthSmedo_tuple.tup");
		//	//HTuple PWidthS_tuple((float *)medoS, cnum * 1);
		////WriteTuple(PWidthS_tuple, "C:\\Samsung\\medoS_tuple.tup");
		//}
		for (jj = 3; jj < cnum - 3; jj++)
		{
			/*if (j == 1 && jj == 9610)
				jj = jj;*/
			ws = *(pwS + caddrb + jj);
			/*if (ws < 0.5)
				ws = ws;*/
			/*if (ws <= 3.5)
				ws = ws;*/
			wgs = (float)*(pwgS + caddrb + jj);
			wns = (float)*(medoS + jj);

			dg = ws - wgs;
			dn = ws - wns;

			dgprc = (float)(dg * 100. / wgs);
			dnprc = (float)(dn * 100. / (wns + 0.001));

			def = 0.;

			//if (dnprc <= -mprc && abs(dn) >= minmbs) // 18.03.2024
			/// if width is smaller than minmbs, then can't use % for MB detection!
			if (dnprc <= -mprc && ws >= minmbs /*&& ws!=0.*/)
				def = dn;
			if (ws < 0.01 /*&& ( jj > bw*1 && jj < cnum- bw * 1)*/) // // Short detected in FWMS processing
			{
				def = -5.0505;  /// special value for Short
				dnprc = -100.;
			}
			if (dnprc >= sprc)
				def = dn;

			if (ws <= minWsp)
			{

				*(pDefMB + mbnum) = def;
				*(pDefMBprc + mbnum) = dnprc;
				*(pXdefMB + mbnum) = *(pXCS + caddrb + jj - 0);
				*(pYdefMB + mbnum) = *(pYCS + caddrb + jj - 0);
				mbnum++;
			}
			else if (def < -0.01)
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

				

				if (prcReal <= -mprc && ws >= minmbs)
				{
					*(pDefMB + mbnum) = def;
					*(pDefMBprc + mbnum) = dnprc;
					*(pXdefMB + mbnum) = *(pXCS + caddrb + jj - 0);
					*(pYdefMB + mbnum) = *(pYCS + caddrb + jj - 0);
					mbnum++;
				}
			}
			else if (def > 0.01)
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

				if (prcReal >= sprc)
				{
					*(pDefSP + spnum) = def;
					*(pDefSPprc + spnum) = dnprc;
					*(pXdefSP + spnum) = *(pXCS + caddrb + jj - 0);
					*(pYdefSP + spnum) = *(pYCS + caddrb + jj - 0);
					spnum++;
				}
			}
		}
		// medoS - buffer for RMed (SPACE)
		//*SPnumS += spnum;
		//*MBnumS += mbnum;
	}
	*MBnumS = mbnum;
	*SPnumS = spnum;
	//j = j;
	//HTuple PWidthS_tuple((float *)medoS, cnum * 1);
	//WriteTuple(PWidthS_tuple, "C:\\Samsung\\medoS_tuple.tup");
	////AfxMessageBox(_T("WidthS tuple saved!"));
}


//void CFWM::FWMS_PostProcCPU(float *pwS, int *pwgS, float *medoS, float *pXCS, float *pYCS, int naS, int nPS, int nvoid, int *pPaddrS, int sz1, int sz2, int skip, float ang, float mprc, float sprc, float mabs, float sabs, float minmbs, float minWsp, int dlen, float *pDefMB, float *pDefMBprc, float *pXdefMB, float *pYdefMB, float *pDefSP, float *pXdefSP, float *pYdefSP, float *pYdefSPprc, int *MBnumS, int *SPnumS)
//{
//	int caddrb, caddre, cnum;
//	int mbnum, spnum, j, jj/*, i*/;
//	//float v;
//
//	//int spnum = 0, mbnum = 0;
//	//int wgmed;
//	//int *pbuf;
//	float /*prcGS, */prcReal;
//
//
//	//
//
//	float ws, wgs, wns;
//	float dg, dgprc; // deviation from CAD
//	float dn, dnprc; // deviation from neighbor
//	float def; // defect size
//
//	float defspmax = 0., defspspprcmax = 0., defspspsum = 0;  /// for % sp
//	int defspspact = 0, defspspcnt = 0;
//
//	float defspmbmax = 0., defspmbprcmax = 0., defspmbsum = 0;  /// for % mb
//	int defspmbact = 0, defspmbcnt = 0;
//
//	//*MBnumS = 0;
//	mbnum = 0;
//	spnum = 0;
//	for (j = 0; j < naS - 0; j++)
//	{
//		defspmbmax = 0.; defspmbprcmax = 0.;
//		defspmbact = defspmbact = 0;
//
//		caddrb = *(pPaddrS + j + 0);
//		if (j + 1 < naS)
//			caddre = *(pPaddrS + j + 1) - 1;
//		else
//			caddre = nPS - 1;
//		cnum = (caddre - caddrb);
//
//		int bw = min(sz2, cnum / 2);
//
//		/*if (caddrb == 158840)
//			caddrb = caddrb;*/
//			//mbnum = *MBnumS;
//			//spnum = 0;
//			//int nvoid = 7;
//	//		FilterM7(pw + caddrb, pwg + caddrb, pXC + caddrb, pYC + caddrb, cnum, nvoid, sz1, sz2, ang, mprc, sprc, mabs, sabs, dlen,  pDefMB + *MBnum, pXdefMB + *MBnum, pYdefMB +* MBnum, pDefSP + *SPnum, pXdefSP + *SPnum, pYdefSP + *SPnum, &mbnum, &spnum, 0,  1);
//			//FilterRMedS(pwS + caddrb, pwgS + caddrb, medoS, pXCS + caddrb, pYCS + caddrb, cnum, sz2, skip, mprc, sprc, mabs, sabs, dlen, pDefMB + *MBnumS, pXdefMB + *MBnumS, pYdefMB + *MBnumS, pDefSP + *SPnumS, pXdefSP + *SPnumS, pYdefSP + *SPnumS, &mbnum, &spnum, 0);
//		SrunmedSLI(pwS + caddrb+0, medoS, cnum-0, bw, 0, skip); // Running median with skip and linear interp.
//		//SrunmedSLIExtNoS(pw + caddrb, medo + MEDOSIZE / 2, medo, n, bw, skip, 0/*10*/);
//																//SrunmedSLIExt(pwS, medoS + MEDOSIZE / 2, medoS, cnum, bw, 1, skip); // Running median with Extension at the ends, skip and linear interp.
//		//SrunmedSLIExt(pwS + caddrb, medoS + MEDOSIZE / 2, medoS, cnum, bw, 0, skip); // Running median with Extension at the ends, skip and linear interp.
//		//if (j==0 )
//		//{
//		//	HTuple PWidthSmedo_tuple((float*)medoS, cnum-10);
//		//	WriteTuple(PWidthSmedo_tuple, "C:\\Temp\\PWidthSmedo_tuple.tup");
//		//	//HTuple PWidthS_tuple((float *)medoS, cnum * 1);
//		////WriteTuple(PWidthS_tuple, "C:\\Samsung\\medoS_tuple.tup");
//		//}
//		for (jj = 3; jj < cnum - 3; jj++)
//		{
//			/*if (j == 1 && jj == 9610)
//				jj = jj;*/
//			ws = *(pwS + caddrb + jj);
//			/*if (ws < 0.5)
//				ws = ws;*/
//			/*if (ws <= 3.5)
//				ws = ws;*/
//			wgs = (float)*(pwgS + caddrb + jj);
//			wns = (float)*(medoS + jj);
//
//			dg = ws - wgs;
//			dn = ws - wns;
//
//			dgprc = (float)(dg * 100. / wgs);
//			dnprc = (float)(dn * 100. / (wns + 0.001));
//
//			def = 0.;
//
//			//if (dnprc <= -mprc && abs(dn) >= minmbs) // 18.03.2024
//			/// if width is smaller than minmbs, then can't use % for MB detection!
//			if (dnprc <= -mprc && ws >= minmbs /*&& ws!=0.*/)
//				def = dn;
//			if (ws < 0.01 /*&& ( jj > bw*1 && jj < cnum- bw * 1)*/) // // Short detected in FWMS processing
//			{
//				def = -5.0505;  /// special value for Short
//				dnprc = -100.;
//			}
//			if (dnprc >= sprc)
//				def = dn;
//
//			if (ws <= minWsp)
//			{
//
//				*(pDefMB + mbnum) = def;
//				*(pDefMBprc + mbnum) = dnprc;
//				*(pXdefMB + mbnum) = *(pXCS + caddrb + jj - 0);
//				*(pYdefMB + mbnum) = *(pYCS + caddrb + jj - 0);
//				mbnum++;
//			}
//			else if (def < -0.01)
//			{
//				/**(pDefMB++) = def;
//				*(pXdefMB++) = *(pXC + j-0);
//				*(pYdefMB++) = *(pYC + j-0);*/
//
//
//				//pbuf = (int *)malloc(sizeof(int) * (2 * bw + 1));
//				//memcpy(pbuf, pwgS + j - bw, 2 * bw + 1);
//				//qsort(pbuf, 2 * bw + 1, sizeof(int), compare);
//				//wgmed = pbuf[bw];
//				//free(pbuf);
//
//				//prcGS = (wgs - wgmed) * 100.0 / (wgmed + 0.001);
//				//// prcGS shouldn't increase prcReal, changed 25.07.2023
//				//if (prcGS > 0.)
//				//	prcGS = 0.;
//				//prcReal = dnprc - prcGS;
//				prcReal = dnprc;
//
//				
//
//				if (prcReal <= -mprc && ws >= minmbs)
//				{
//					*(pDefMB + mbnum) = def;
//					*(pDefMBprc + mbnum) = dnprc;
//					*(pXdefMB + mbnum) = *(pXCS + caddrb + jj - 0);
//					*(pYdefMB + mbnum) = *(pYCS + caddrb + jj - 0);
//					mbnum++;
//				}
//			}
//			else if (def > 0.01)
//			{
//				/**(pDefMB++) = def;
//				*(pXdefMB++) = *(pXC + j-0);
//				*(pYdefMB++) = *(pYC + j-0);*/
//
//
//				//pbuf = (int *)malloc(sizeof(int) * (2 * bw + 1));
//				//memcpy(pbuf, pwgS + j - bw, 2 * bw + 1);
//				//qsort(pbuf, 2 * bw + 1, sizeof(int), compare);
//				//wgmed = pbuf[bw];
//				//free(pbuf);
//
//				//prcGS = (wgs - wgmed) * 100.0 / (wgmed + 0.001);
//				//// prcGS shouldn't increase prcReal, changed 25.07.2023
//				//if (prcGS > 0.)
//				//	prcGS = 0.;
//				//prcReal = dnprc - prcGS;
//				prcReal = dnprc;
//
//				if (prcReal >= sprc)
//				{
//					*(pDefSP + spnum) = def;
//					*(pDefSPprc + spnum) = dnprc;
//					*(pXdefSP + spnum) = *(pXCS + caddrb + jj - 0);
//					*(pYdefSP + spnum) = *(pYCS + caddrb + jj - 0);
//					spnum++;
//				}
//			}
//		}
//		// medoS - buffer for RMed (SPACE)
//		//*SPnumS += spnum;
//		//*MBnumS += mbnum;
//	}
//	*MBnumS = mbnum;
//	*SPnumS = spnum;
//	//j = j;
//	//HTuple PWidthS_tuple((float *)medoS, cnum * 1);
//	//WriteTuple(PWidthS_tuple, "C:\\Samsung\\medoS_tuple.tup");
//	////AfxMessageBox(_T("WidthS tuple saved!"));
//}



// Prototype
void CFWM::SrunmedProto(float* y, float* smo, long n, int bw_tot, int skip) // 2022.08.01 - running median with skipping [skip] values, with linear interpolation
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

	smo[/*band2*step +*/ shift] = (float)rmed;
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
		smo[ismo - rad] = (float)rmed;
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
				smo[x - rad] = (float)(fa + j * (fb - fa) / step); //b-a=step, x-a=j
			}
		}
	}
}

void CFWM::SrunmedSLIExtNoS(float* yinp, float* y/*extended*/, float* smo, long n1, int bw_tot, /*int end_rule,*/ int skip, int noskipsz) // 2022.08.01 - running median with skipping [skip] values, with linear interpolation
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


void CFWM::FWM_PostProcCPUI(float *pw, int *pwg, float *medo, float *pXC, float *pYC, int n, int sz1, int skip, float mprc, float sprc, float mabs, float pabs, int dlen,
	float *pDefMB, float *pXdefMB, float *pYdefMB, float *pDefSP, float *pXdefSP, float *pYdefSP, int *MBnum, int *SPnum)
{	/// processing of single seleton part of length=n, RMed window=sz1
	/// mode==0  - not using CAD width

	int j;
	int spnum = 0, mbnum = 0;
	//	float *medo;
	int bw = min(sz1, n);
	int wgmed;
	//int *pbuf;

	float prcG, prcReal;

	float w, wg, wn;
	float dg, dgprc; // deviation from CAD
	float dn, dnprc; // deviation from neighbor
	float def; // defect size
	float defw; // defect width

	float defspmax = 0., defspsum = 0;
	int defspact = 0, defspcnt=0;

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
		wg = (float)*(pwg + j);
		wn = *(medo + j);

		dg = w - wg;
		dn = w - wn;

		dgprc = (float)(dg * 100. / wg);
		dnprc = (float)(dn * 100. / (wn + 0.001));

		def = 0.;
		/*if (dgprc >= sprc)
			def = dg;*/
		if (dnprc >= sprc)
			def = dn;

		/*if (dgprc <= -mprc)
			def = dg;*/
		if (dnprc <= -mprc && w < mabs)
			def = dn;

		if (def > 0.01)
		{
			//int *pbuf = malloc((size_t)(2 * bw + 1));
			/*pbuf = (int *)malloc(sizeof(int) * (2 * bw + 1));
			memcpy(pbuf, pwg + j - bw, 2 * bw + 1);
			qsort(pbuf, 2 * bw + 1, sizeof(int), compare);*/
			//wgmed = pbuf[bw];
			//free(pbuf);
			wgmed = pwg[j];

			prcG = (float)((wg - wgmed) * 100.0 / (wgmed + 0.001));
			// prcG shouldn't increase prcReal, changed 31.07.2023
			if (prcG < 0.)
				prcG = 0.;
			prcReal = dnprc - prcG;
			
			if (prcReal >= sprc)
			{
				if (defspact == 0)
				{
					defspact = 1; // start
					defspmax = prcReal;
				}
				else
					defspact = 2; // continue
				//defspcnt++;
					
			}
			else
			{
				if (defspact == 2)
				{   
					*(pDefSP++) = def;
					*(pXdefSP++) = *(pXC + j - 1);
					*(pYdefSP++) = *(pYC + j - 1);
					spnum++;
				}
				
				defspact = 0;
				//defspcnt = 0;
			}
		}
		else if (def < -0.01)
		{
			/*pbuf = (int *)malloc(sizeof(int) * (2 * bw + 1));
			memcpy(pbuf, pwg + j - bw, 2 * bw + 1);
			qsort(pbuf, 2 * bw + 1, sizeof(int), compare);
			wgmed = pbuf[bw];
			free(pbuf);*/
			wgmed = pwg[j];

			prcG = (float)((wg - wgmed) * 100.0 / (wgmed + 0.001));
			// prcGS shouldn't increase prcReal, changed 31.07.2023
			if (prcG > 0.)
				prcG = 0.;
			prcReal = dnprc - prcG;

			if (prcReal <= mprc)
			{
				int ju, jd;
				for (ju = 1; ju < 10; ju++)
				{
					w = *(pw + j + ju);
					wn = *(medo + j + ju);
					dn = w - wn;
					dnprc = (float)(dn * 100. / (wn + 0.001));
					if (dnprc < 0.5*mprc)
						break;
				}
				for (jd = 1; jd < 10; jd++)
				{
					w = *(pw + j - jd);
					wn = *(medo + j - jd);
					dn = w - wn;
					dnprc = (float)(dn * 100. / (wn + 0.001));
					if (dnprc < 0.5*mprc)
						break;
				}
				defw = (float)(ju + jd);

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

#define FWM_NONE	0

#define FWM_NARROW 1
#define FWM_MB_ABS	2
#define FWM_MB_PRC	3
#define FWM_SP_ABS	4
#define FWM_SP_PRC	5
#define FWM_OPEN	6
#define FWM_WIDE	7

void CFWM::FWM_PostProcCPU_2C(float *pw, int *pwg, float *medo, float *pXC, float *pYC, int nc, int n, int *pPaddr, int sz1, int skip,
	float mprc, float sprc, float mabs, float minthp, float narpwidth, float widepwidth, float stdgpwidth, float pabs, int dlen,
	float *pDefMB, float *pDefMBprc, float *pXdefMB, float *pYdefMB, float *pDefSP, float *pDefSPprc, float *pXdefSP, float *pYdefSP, int *dPuseint, int *MBnum, int *SPnum)
{	/// processing of single seleton part of length=n, RMed window=sz1
	/// mode==0  - not using CAD width

	int j;
	int spnum = 0, mbnum = 0;
	//	float *medo;
	int bw = min(sz1, n);
	int wgmed;
	//int *pbuf;

	float prcG, prcReal;

	float w, wg, wn;
	float dg, dgprc; // deviation from CAD
	float dn, dnprc; // deviation from neighbor
	float def, defprc; // defect size

	float defspmax = 0., defspspprcmax = 0., defspspsum = 0;  /// for % sp
	int defspspact = 0, defspspcnt = 0;

	float defspmbmax = 0., defspmbprcmax = 0., defspmbsum = 0;  /// for % mb
	int defspmbact = 0, defspmbcnt = 0;

	int deftype, defin;
	float defx, defy;

	float defw; // defect width
	float wnorm;

	/*float defaspmax = 0., defaspprcmax = 0., defaspsum = 0;  /// for abs. sp
	int defaspact = 0, defaspcnt = 0;

	float defpspmax = 0., defpspprcmax = 0., defpspsum = 0;  /// for % sp
	int defpspact = 0, defpspcnt = 0;

	float defpmbmax = 0., defpmbprcmax = 0., defpmbsum = 0;  /// for % mb
	int defpmbact = 0, defpmbcnt = 0;

	float defnpmbmax = 0., defnpmbprcmax = 0., defnpmbsum = 0;  /// for non % mb
	int defnpmbact = 0, defnpmbcnt = 0;*/

	if (n == 0)
	{
		*SPnum = spnum;
		*MBnum = mbnum;
		return;
	}
	//nc = 1; // to save 1st coil data
	int caddrb, caddre, cnum;
	for (int j = 0; j < nc - 0; j++)
	{
		/*defaspmax = 0.; defaspprcmax = 0.;
		defpspmax = 0.; defpspprcmax = 0.;
		defpmbmax = 0.; defpmbprcmax = 0.;
		defnpmbmax = 0.; defnpmbprcmax = 0.;
		defaspact = defpspact = defpmbact = defnpmbact = 0;*/

		defspmbmax = 0.; defspmbprcmax = 0.;
		defspmbact = defspmbact = 0;

		caddrb = *(pPaddr + j + 0);
		if (j + 1 < nc)
			caddre = *(pPaddr + j + 1) - 1;
		else
			caddre = n - 1;
		cnum = (caddre - caddrb);

		int bw = min(sz1, cnum / 2);

		/*
		caddrb = *(pPaddr + j + 0);
		if (j + 1 < nc)
			caddre = *(pPaddr + j + 1) - 1;
		else
			caddre = n - 1;
		cnum = (caddre - caddrb);*/

		//medo = (float *)malloc(n * sizeof(float));
		//Srunmed(pw, medo, n, bw, 0); // Running median
		//Srunmed(pw, medo, n, bw, 1);
		//SrunmedS(pw, medo, n, bw, 0, 4); // Running median with skip
		//SrunmedSLI(pw, medo, n, bw, 1, skip); // Running median with skip and linear interp.
		//float *buf=(float *)malloc((n+2*bw)*sizeof(float));
		//SrunmedSLIExt(pw,  medo+MEDOSIZE/2, medo, n, bw, 0, skip); // Running median with Extension at the ends, skip and linear interp.
		//////////////////////
		//SrunmedSLIExtNoS(pw + caddrb, medo + MEDOSIZE / 2, medo, n, bw, skip, 0); // Running median with Extension at the ends, skip and linear interp (with skip at ends).

		SrunmedSLI(pw + caddrb + 0, medo, cnum - 0, bw, 0, skip);
		defin = 0;
		//SrunmedSLI(pw + caddrb + 0, medo + MEDOSIZE * j / 2, cnum - 0, bw, 0, skip); /// saving both profiles for testing
		int cnt = 0;
		//for (j = caddrb + bw / 2; j < caddre - bw / 2; j++)
		//for (int jj = bw / 2; jj < cnum - bw / 2; jj++)
		for (int jj = 3; jj < cnum - 3; jj++)
		{
			if (j == 1 && jj == 6612)
				j = j;
			w = *(pw + caddrb + jj);
			if (w >= widepwidth)
				w = w;
			if (w <= narpwidth)
				w = w;
			wg = (float)*(pwg + caddrb + jj);
			//wn = (float)*(medo + MEDOSIZE * j / 2 + jj);
			wn = (float)*(medo + jj);
			//defin = 0;
			//cnt++;
			//w = *(pw + caddrb + j);
			//wg = (float)*(pwg + j);
			//wn = *(medo + MEDOSIZE * jj / 2 + j - caddrb); /// for testing
			////wn = *(medo + j - caddrb);
			dg = w - wg;
			dn = w - wn;
			/*if (w < 2.0)
				w = w;*/

			if (dn >= pabs)
				w = w;

			dgprc = (float)(dg * 100. / wg);
			dnprc = (float)(dn * 100. / (wn + 0.001));

			//f = 0.;
			/*if (dgprc >= sprc)
				def = dg;*/
				// (dnprc >= sprc)
				//ef = dn;

				/*if (dgprc <= -mprc)
					def = dg;*/
			if (wg >= minthp && w > narpwidth && w < widepwidth && dn < pabs) // % (rel.) criterion 
			{
				defin = 0;
				if (dnprc <= -mprc)
				{
					def = -dn;
					defprc = dnprc;
					deftype = FWM_MB_PRC;
					defin = 1;
				}
				else if (dnprc >= sprc)
				{
					def = dn;
					defprc = dnprc;
					deftype = FWM_SP_PRC;
					defin = 1;

				}
			}
			else if (dn >= pabs) // small spur
			{
				/*if (dn <= -mabs)
				{*/
				def = dn;
				defprc = 100.;
				deftype = FWM_SP_ABS;
				defin = 1;
				//defx = *(pXC + caddrb + jj - 0);
				//defy = *(pYC + caddrb + jj - 0);
			//}
			}
			else if (/*wg < minthp*/1) //abs.criterion ... if width is smaller than minmbs, then can't use % based detection!
			{
				if (dn <= -mabs)
				{
					def = -dn;
					defprc = 100.;
					deftype = FWM_MB_ABS;
					defin = 1;
					/*				defx = *(pXC + caddrb + jj - 0);
									defy = *(pYC + caddrb + jj - 0);*/
				}
				else if (dn >= pabs)
				{
					def = dn;
					defprc = 100.;
					deftype = FWM_SP_ABS;
					defin = 1;
					/*defx = *(pXC + caddrb + jj - 0);
					defy = *(pYC + caddrb + jj - 0);*/
				}
				//else if (wn < narpwidth) // too narrow pattern detected in FWM processing
				//{
				//	def = w;  /// width
				//	defprc = -100.;
				//	deftype = FWM_NARROW;
				//	defin = 1;
				//}
			//}
				else if (wn < 0.01) // open detected in FWM processing
				{
					def = -5.0505;  /// special value for Short
					defprc = -100.;
					deftype = FWM_OPEN;
					defin = 1;
				}
				else if (w < narpwidth) // too narrow pattern detected in FWM processing
				{
					def = w;  /// width
					defprc = -100.;
					deftype = FWM_NARROW;
					defin = 1;
				}
				else if (w >= widepwidth * wg / stdgpwidth) // too wide pattern detected in FWM processing
				{
					def = w;  /// width
					defprc = 100.;
					deftype = FWM_WIDE;
					defin = 1;
				}
			}
			else
				defin = 0;
			// saving TMS defects	
			if (defin == 1)
			{
				if (defspmbact == 0)
				{
					defspmbact = 1; // start
					defspmbmax = def;
					defspmbprcmax = abs(dnprc);
					defx = *(pXC + caddrb + jj - 0);
					defy = *(pYC + caddrb + jj - 0);
				}
				else
				{
					if (abs(def) > defspmbmax)
					{
						defspmbmax = def;
						defspmbprcmax = abs(dnprc);
						defx = *(pXC + caddrb + jj - 0);
						defy = *(pYC + caddrb + jj - 0);
					}
					/*if (dnprc < defspmbmin)
					{
						defspmbmax = def;
						defspmbprcmax = dnprc;
					}*/
					defspmbact = 2; // continue 
				}
			}
			else
			{
				//if (defspmbact == 2)
				if (defspmbact > 0)
				{
					if (deftype == FWM_MB_ABS || deftype == FWM_MB_PRC || deftype == FWM_NARROW)
					{
						*(pDefMB + mbnum) = defspmbmax;
						*(pDefMBprc + mbnum) = defspmbprcmax;
						*(pXdefMB + mbnum) = defx;
						*(pYdefMB + mbnum) = defy;
						mbnum++;
					}
					else if (deftype == FWM_SP_ABS || deftype == FWM_SP_PRC || deftype == FWM_WIDE)
					{
						*(pDefSP + spnum) = defspmbmax;
						*(pDefSPprc + spnum) = defspmbprcmax;
						*(pXdefSP + spnum) = defx;
						*(pYdefSP + spnum) = defy;
						spnum++;
					}
				}
				else
				{
					deftype = FWM_NONE;
				}

				defspmbact = 0;
				defin = 0;
			}
		}
	}
	if (defspmbact == 2)
	{
		if (deftype == FWM_MB_ABS || deftype == FWM_MB_PRC || deftype == FWM_NARROW)
		{
			*(pDefMB + mbnum) = defspmbmax;
			*(pDefMBprc + mbnum) = defspmbprcmax;
			*(pXdefMB + mbnum) = defx;
			*(pYdefMB + mbnum) = defy;
			mbnum++;
		}
		else if (deftype == FWM_SP_ABS || deftype == FWM_SP_PRC || deftype == FWM_WIDE)
		{
			*(pDefSP + spnum) = defspmbmax;
			*(pDefSPprc + spnum) = defspmbprcmax;
			*(pXdefSP + spnum) = defx;
			*(pYdefSP + spnum) = defy;
			spnum++;
		}
	}
	*SPnum = spnum;
	*MBnum = mbnum;
	//free(medo);
	//free(buf);
}

void CFWM::FWM_PostProcCPUNew(float *pw, int *pwg, float *medo, float *pXC, float *pYC, int nc, int n, int *pPaddr, int sz1, int skip, 
	float mprc, float sprc, float mabs, float minthp, float narpwidth, float widepwidth, float stdgpwidth, float pabs, int dlen,
	float *pDefMB, float *pDefMBprc, float *pXdefMB, float *pYdefMB, float *pDefSP, float *pDefSPprc, float *pXdefSP, float *pYdefSP, int *MBnum, int *SPnum)
{	/// processing of single seleton part of length=n, RMed window=sz1
	/// mode==0  - not using CAD width

	int j;
	int spnum = 0, mbnum = 0;
	//	float *medo;
	int bw = min(sz1, n);
	int wgmed;
	//int *pbuf;

	float prcG, prcReal;

	float w, wg, wn;
	float dg, dgprc; // deviation from CAD
	float dn, dnprc; // deviation from neighbor
	float def, defprc; // defect size

	float defspmax = 0., defspspprcmax = 0., defspspsum = 0;  /// for % sp
	int defspspact = 0, defspspcnt = 0;

	float defspmbmax = 0., defspmbprcmax = 0., defspmbsum = 0;  /// for % mb
	int defspmbact = 0, defspmbcnt = 0;

	int deftype, defin;
	float defx, defy;

	float defw; // defect width
	float wnorm; 

	/*float defaspmax = 0., defaspprcmax = 0., defaspsum = 0;  /// for abs. sp
	int defaspact = 0, defaspcnt = 0;

	float defpspmax = 0., defpspprcmax = 0., defpspsum = 0;  /// for % sp
	int defpspact = 0, defpspcnt = 0;

	float defpmbmax = 0., defpmbprcmax = 0., defpmbsum = 0;  /// for % mb
	int defpmbact = 0, defpmbcnt = 0;

	float defnpmbmax = 0., defnpmbprcmax = 0., defnpmbsum = 0;  /// for non % mb
	int defnpmbact = 0, defnpmbcnt = 0;*/

	if (n == 0)
	{
		*SPnum = spnum;
		*MBnum = mbnum;
		return;
	}
	//nc = 1; // to save 1st coil data
	int caddrb, caddre, cnum;
	for (int j = 0; j < nc - 0; j++)
	{
		/*defaspmax = 0.; defaspprcmax = 0.;
		defpspmax = 0.; defpspprcmax = 0.;
		defpmbmax = 0.; defpmbprcmax = 0.;
		defnpmbmax = 0.; defnpmbprcmax = 0.;
		defaspact = defpspact = defpmbact = defnpmbact = 0;*/

		defspmbmax = 0.; defspmbprcmax = 0.;
		defspmbact = defspmbact = 0;

		caddrb = *(pPaddr + j + 0);
		if (j + 1 < nc)
			caddre = *(pPaddr + j + 1) - 1;
		else
			caddre = n - 1;
		cnum = (caddre - caddrb);

		int bw = min(sz1, cnum / 2);

		/*
		caddrb = *(pPaddr + j + 0);
		if (j + 1 < nc)
			caddre = *(pPaddr + j + 1) - 1;
		else
			caddre = n - 1;
		cnum = (caddre - caddrb);*/

		//medo = (float *)malloc(n * sizeof(float));
		//Srunmed(pw, medo, n, bw, 0); // Running median
		//Srunmed(pw, medo, n, bw, 1);
		//SrunmedS(pw, medo, n, bw, 0, 4); // Running median with skip
		//SrunmedSLI(pw, medo, n, bw, 1, skip); // Running median with skip and linear interp.
		//float *buf=(float *)malloc((n+2*bw)*sizeof(float));
		//SrunmedSLIExt(pw,  medo+MEDOSIZE/2, medo, n, bw, 0, skip); // Running median with Extension at the ends, skip and linear interp.
		//////////////////////
		//SrunmedSLIExtNoS(pw + caddrb, medo + MEDOSIZE / 2, medo, n, bw, skip, 0); // Running median with Extension at the ends, skip and linear interp (with skip at ends).

		SrunmedSLI(pw + caddrb + 0, medo, cnum - 0, bw, 0, skip);
		defin = 0;
		//SrunmedSLI(pw + caddrb + 0, medo + MEDOSIZE * j / 2, cnum - 0, bw, 0, skip); /// saving both profiles for testing
		int cnt = 0;
		//for (j = caddrb + bw / 2; j < caddre - bw / 2; j++)
		//for (int jj = bw / 2; jj < cnum - bw / 2; jj++)
		for (int jj = 3; jj < cnum - 3; jj++)
		{
			if (j == 1 && jj == 6612)
				j = j;
			w = *(pw + caddrb + jj);
			if (w >= widepwidth)
				w = w;
			if (w <= narpwidth)
				w = w;
			wg = (float)*(pwg + caddrb + jj);
			//wn = (float)*(medo + MEDOSIZE * j / 2 + jj);
			wn = (float)*(medo +  jj);
			//defin = 0;
			//cnt++;
			//w = *(pw + caddrb + j);
			//wg = (float)*(pwg + j);
			//wn = *(medo + MEDOSIZE * jj / 2 + j - caddrb); /// for testing
			////wn = *(medo + j - caddrb);
			dg = w - wg;
			dn = w - wn;
			/*if (w < 2.0)
				w = w;*/

			if (dn >= pabs)
				w = w;

			dgprc = (float)(dg * 100. / wg);
			dnprc = (float)(dn * 100. / (wn + 0.001));

			//f = 0.;
			/*if (dgprc >= sprc)
				def = dg;*/
				// (dnprc >= sprc)
				//ef = dn;

				/*if (dgprc <= -mprc)
					def = dg;*/
			if (wg >= minthp && w > narpwidth && w < widepwidth && dn < pabs) // % (rel.) criterion 
			{
				defin = 0;
				if (dnprc <= -mprc)
				{
					def = -dn;
					defprc = dnprc;
					deftype = FWM_MB_PRC;
					defin = 1;
				}
				else if (dnprc >= sprc)
				{
					def = dn;
					defprc = dnprc;
					deftype = FWM_SP_PRC;
					defin = 1;

				}
			}
			else if (dn >= pabs) // small spur
			{
				/*if (dn <= -mabs)
				{*/
				def = dn;
				defprc = 100.;
				deftype = FWM_SP_ABS;
				defin = 1;
				//defx = *(pXC + caddrb + jj - 0);
				//defy = *(pYC + caddrb + jj - 0);
			//}
			}
			else if (/*wg < minthp*/1) //abs.criterion ... if width is smaller than minmbs, then can't use % based detection!
			{
				if (dn <= -mabs)
				{
					def = -dn;
					defprc = 100.;
					deftype = FWM_MB_ABS;
					defin = 1;
					/*				defx = *(pXC + caddrb + jj - 0);
									defy = *(pYC + caddrb + jj - 0);*/
				}
				else if (dn >= pabs)
				{
					def = dn;
					defprc = 100.;
					deftype = FWM_SP_ABS;
					defin = 1;
					/*defx = *(pXC + caddrb + jj - 0);
					defy = *(pYC + caddrb + jj - 0);*/
				}
				//else if (wn < narpwidth) // too narrow pattern detected in FWM processing
				//{
				//	def = w;  /// width
				//	defprc = -100.;
				//	deftype = FWM_NARROW;
				//	defin = 1;
				//}
			//}
				else if (wn < 0.01) // open detected in FWM processing
				{
					def = -5.0505;  /// special value for Short
					defprc = -100.;
					deftype = FWM_OPEN;
					defin = 1;
				}
				else if (w < narpwidth) // too narrow pattern detected in FWM processing
				{
					def = w;  /// width
					defprc = -100.;
					deftype = FWM_NARROW;
					defin = 1;
				}
				else if (w >= widepwidth*wg/ stdgpwidth) // too wide pattern detected in FWM processing
				{
					def = w;  /// width
					defprc = 100.;
					deftype = FWM_WIDE;
					defin = 1;
				}
			}
			else
				defin = 0;
			// saving TMS defects	
			if (defin == 1)
			{
				if (defspmbact == 0)
				{
					defspmbact = 1; // start
					defspmbmax = def;
					defspmbprcmax = abs(dnprc);
					defx = *(pXC + caddrb + jj - 0);
					defy = *(pYC + caddrb + jj - 0);
				}
				else
				{
					if (abs(def) > defspmbmax)
					{
						defspmbmax = def;
						defspmbprcmax = abs(dnprc);
						defx = *(pXC + caddrb + jj - 0);
						defy = *(pYC + caddrb + jj - 0);
					}
					/*if (dnprc < defspmbmin)
					{
						defspmbmax = def;
						defspmbprcmax = dnprc;
					}*/
					defspmbact = 2; // continue 
				}
			}
			else
			{
				//if (defspmbact == 2)
				if (defspmbact > 0)
				{
					if (deftype == FWM_MB_ABS || deftype == FWM_MB_PRC || deftype == FWM_NARROW)
					{
						*(pDefMB + mbnum) = defspmbmax;
						*(pDefMBprc + mbnum) = defspmbprcmax;
						*(pXdefMB + mbnum) = defx;
						*(pYdefMB + mbnum) = defy;
						mbnum++;
					}
					else if (deftype == FWM_SP_ABS || deftype == FWM_SP_PRC || deftype == FWM_WIDE)
					{
						*(pDefSP + spnum) = defspmbmax;
						*(pDefSPprc + spnum) = defspmbprcmax;
						*(pXdefSP + spnum) = defx;
						*(pYdefSP + spnum) = defy;
						spnum++;
					}
				}
				else
				{
					deftype = FWM_NONE;
				}
				
				defspmbact = 0;
				defin = 0;
			}
		}
	}
	if (defspmbact == 2)
	{
		if (deftype == FWM_MB_ABS || deftype == FWM_MB_PRC || deftype == FWM_NARROW)
		{
			*(pDefMB + mbnum) = defspmbmax;
			*(pDefMBprc + mbnum) = defspmbprcmax;
			*(pXdefMB + mbnum) = defx;
			*(pYdefMB + mbnum) = defy;
			mbnum++;
		}
		else if (deftype == FWM_SP_ABS || deftype == FWM_SP_PRC || deftype == FWM_WIDE)
		{
			*(pDefSP + spnum) = defspmbmax;
			*(pDefSPprc + spnum) = defspmbprcmax;
			*(pXdefSP + spnum) = defx;
			*(pYdefSP + spnum) = defy;
			spnum++;
		}
	}
	*SPnum = spnum;
	*MBnum = mbnum;
	//free(medo);
	//free(buf);
}

void CFWM::FWM_PostProcCPU(float *pw, int *pwg, float *medo, float *pXC, float *pYC, int nc, int n, int *pPaddr, int sz1, int skip, float mprc, float sprc, float mabs, float minthp, float pabs, int dlen,
	float *pDefMB, float *pDefMBprc, float *pXdefMB, float *pYdefMB, float *pDefSP, float *pDefSPprc, float *pXdefSP, float *pYdefSP, int *MBnum, int *SPnum)
{	/// processing of single seleton part of length=n, RMed window=sz1
	/// mode==0  - not using CAD width

	int j;
	int spnum = 0, mbnum = 0;
	//	float *medo;
	int bw = min(sz1, n);
	int wgmed;
	//int *pbuf;

	float prcG, prcReal;

	float w, wg, wn;
	float dg, dgprc; // deviation from CAD
	float dn, dnprc; // deviation from neighbor
	float def; // defect size
	float defw; // defect width

	float defaspmax = 0., defaspprcmax = 0., defaspsum = 0;  /// for abs. sp
	int defaspact = 0, defaspcnt = 0;

	float defpspmax = 0., defpspprcmax = 0., defpspsum = 0;  /// for % sp
	int defpspact = 0, defpspcnt = 0;

	float defpmbmax = 0., defpmbprcmax = 0., defpmbsum = 0;  /// for % mb
	int defpmbact = 0, defpmbcnt = 0;

	float defnpmbmax = 0., defnpmbprcmax = 0., defnpmbsum = 0;  /// for non % mb
	int defnpmbact = 0, defnpmbcnt = 0;

	if (n == 0)
	{
		*SPnum = spnum;
		*MBnum = mbnum;
		return;
	}
	//nc = 1; // to save 1st coil data
	int caddrb, caddre, cnum;
	for (int jj = 0; jj < nc - 0; jj++)
	{
		defaspmax = 0.; defaspprcmax = 0.;
		defpspmax = 0.; defpspprcmax = 0.;
		defpmbmax = 0.; defpmbprcmax = 0.;
		defnpmbmax = 0.; defnpmbprcmax = 0.;
		defaspact = defpspact = defpmbact = defnpmbact = 0;
		caddrb = *(pPaddr + jj + 0);
		if (jj + 1 < nc)
			caddre = *(pPaddr + jj + 1) - 1;
		else
			caddre = n - 1;
		cnum = (caddre - caddrb);
		//medo = (float *)malloc(n * sizeof(float));
		//Srunmed(pw, medo, n, bw, 0); // Running median
		//Srunmed(pw, medo, n, bw, 1);
		//SrunmedS(pw, medo, n, bw, 0, 4); // Running median with skip
		//SrunmedSLI(pw, medo, n, bw, 1, skip); // Running median with skip and linear interp.
		//float *buf=(float *)malloc((n+2*bw)*sizeof(float));
		//SrunmedSLIExt(pw,  medo+MEDOSIZE/2, medo, n, bw, 0, skip); // Running median with Extension at the ends, skip and linear interp.
		//////////////////////
	    //SrunmedSLIExtNoS(pw + caddrb, medo + MEDOSIZE / 2, medo, n, bw, skip, 0); // Running median with Extension at the ends, skip and linear interp (with skip at ends).
		
		//SrunmedSLI(pw + caddrb + 0, medo, cnum - 0, bw, 0, skip);
		SrunmedSLI(pw + caddrb + 0, medo + MEDOSIZE*jj / 2, cnum - 0, bw, 0, skip); /// saving both profiles for testing
		int cnt = 0;
		for (j = caddrb+bw/2; j < caddre - bw/2; j++)
		{
			cnt++;
			w = *(pw + j);
			wg = (float)*(pwg + j);
			wn = *(medo + MEDOSIZE * jj / 2 + j- caddrb); /// for testing
			//wn = *(medo + j - caddrb);
			dg = w - wg;
			dn = w - wn;
			/*if (w < 2.0)
				w = w;*/

			dgprc = (float)(dg * 100. / wg);
			dnprc = (float)(dn * 100. / (wn + 0.001));

			def = 0.;
			/*if (dgprc >= sprc)
				def = dg;*/
			if (dnprc >= sprc)
				def = dn;

			/*if (dgprc <= -mprc)
				def = dg;*/


			if (w > 0 && wn < minthp) // 241223 - added
			{
				def = w;

				if (defnpmbact == 0)
				{
					defnpmbact = 1; // start
					defnpmbprcmax = -100;
					defnpmbmax = def;
				}
				else
				{
					if (dnprc < defnpmbprcmax)
					{
						defnpmbmax = def;
						defnpmbprcmax = dnprc;
					}
					defnpmbact = 2; // continue 
				}
				
				//mbnum++;
			}
			else
			{
				if (defnpmbact == 2)
				{
					*(pDefMB++) = def;
					*(pDefMBprc++) = defnpmbmax;
					*(pXdefMB++) = *(pXC + j - 1);
					*(pYdefMB++) = *(pYC + j - 1);
					mbnum++;
				}

				defnpmbact = 0;
			}

			if (dnprc <= -mprc && w < minthp)
				def = dn;

			if (dn >= pabs)  // absolute defect spur **********************
			{
				def = dn;
				if (defaspact == 0)
				{
					defaspact = 1; // start
					defaspmax = def;
					defaspprcmax = 100.;
				}
				else
				{
					defaspact = 2; // continue 
					if (def > defaspmax)
					{
						defaspmax = def;
						defaspprcmax = 100.;
					}
				}

				continue;
			}
			else
			{
				if (defaspact == 2)
				{
					*(pDefSP++) = defaspmax;
					*(pDefSPprc++) = defaspprcmax;
					*(pXdefSP++) = *(pXC + j - 1);
					*(pYdefSP++) = *(pYC + j - 1);
					spnum++;
				}

				defaspact = 0;
				//defspcnt = 0;
			}
			// absolute defect  end  **********************

			if (dn > 0.01) // % defect spur **********************
			{
				//int *pbuf = malloc((size_t)(2 * bw + 1));
				/*pbuf = (int *)malloc(sizeof(int) * (2 * bw + 1));
				memcpy(pbuf, pwg + j - bw, 2 * bw + 1);
				qsort(pbuf, 2 * bw + 1, sizeof(int), compare);*/
				//wgmed = pbuf[bw];
				//free(pbuf);
				wgmed = pwg[j];

				prcG = (float)((wg - wgmed) * 100.0 / (wgmed + 0.001));
				// prcG shouldn't increase prcReal, changed 31.07.2023
				if (prcG < 0.)
					prcG = 0.;
				prcReal = dnprc - prcG;
				if (prcReal >= sprc) // % spur defect
				{
					if (defpspact == 0)
					{
						defpspact = 1; // start
						defpspmax = prcReal;
						defpspmax = def;
					}
					else
					{
						if (prcReal > defpspmax)
						{
							defpspmax = def;
							defpspprcmax = prcReal;
						}
						defpspact = 2; // continue 
					}
				}
				else
				{
					if (defpspact == 2)
					{
						*(pDefSP++) = def;
						*(pDefSPprc++) = defpspmax;
						*(pXdefSP++) = *(pXC + j - 1);
						*(pYdefSP++) = *(pYC + j - 1);
						spnum++;
					}

					defpspact = 0;
					//defspcnt = 0;
				}
			}
			else if (dn < -0.01)  // % defect MB **********************
			{
				/*pbuf = (int *)malloc(sizeof(int) * (2 * bw + 1));
				memcpy(pbuf, pwg + j - bw, 2 * bw + 1);
				qsort(pbuf, 2 * bw + 1, sizeof(int), compare);
				wgmed = pbuf[bw];
				free(pbuf);*/
				wgmed = pwg[j];

				prcG = (float)((wg - wgmed) * 100.0 / (wgmed + 0.001));
				// prcGS shouldn't increase prcReal, changed 31.07.2023
				if (prcG > 0.)
					prcG = 0.;
				prcReal = dnprc - prcG;

				if (prcReal <= -mprc)
				{
					int ju, jd;
					for (ju = 1; ju < 10; ju++)
					{
						w = *(pw + j + ju);
						wn = *(medo + j + ju);
						dn = w - wn;
						dnprc = (float)(dn * 100. / (wn + 0.001));
						if (dnprc < 0.5*mprc)
							break;
					}
					for (jd = 1; jd < 10; jd++)
					{
						w = *(pw + j - jd);
						wn = *(medo + j - jd);
						dn = w - wn;
						dnprc = (float)(dn * 100. / (wn + 0.001));
						if (dnprc < 0.5*mprc)
							break;
					}
					defw = (float)(ju + jd);

					if (defpmbact == 0)
					{
						defpmbact = 1; // start
						defpmbprcmax = prcReal;
						defpmbmax = def;
					}
					else
					{
						if (prcReal < defpmbprcmax)
						{
							defpmbmax = def;
							defpmbprcmax = prcReal;
						}
						defpmbact = 2; // continue 
					}
				}
				else
				{
					if (defpmbact == 2)
					{
						*(pDefMB++) = def;
						*(pDefMBprc++) = defpmbmax;
						*(pXdefMB++) = *(pXC + j - 1);
						*(pYdefMB++) = *(pYC + j - 1);
						mbnum++;
					}

					defpmbact = 0;
					//defspcnt = 0;
				}

					
				//}
			}
		}
	}
	*SPnum = spnum;
	*MBnum = mbnum;
	//free(medo);
	//free(buf);
}

#endif



