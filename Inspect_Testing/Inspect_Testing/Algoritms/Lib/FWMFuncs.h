#pragma once

#ifdef USE_DLL
#include "HalconCpp.h"
#include "HDevThread.h"

using namespace HalconCpp;
using namespace std;

#ifdef FWMDLL_EXPORTS
#define FWMDLL_API __declspec(dllexport)
#else
#define FWMDLL_API __declspec(dllimport)
#endif

class FWMDLL_API CFWM;
#endif



#define ND 1024 // Max. number of MB and SP per skeleton part
#define MAX_FWM_DEFECT			10000
#define MEDOSIZE			    280000

class CFWM
{
public:
	CFWM();
	~CFWM();

public:
	int SPnumS = 0, MBnumS = 0;
	int SPnumP = 0, MBnumP = 0;

	int skipbegP = 1300;
	int skipendP = 900;

	HObject /*ho_ImCLC, */ho_ImIRBP, ho_ImIRBS, ho_RegionInoD;
	UINT16 *pImIRBP, *pImIRBS, *pImCLC, *pImCLCS;// image pointers

	float *pWidth = NULL, *pWmin = NULL, *pmedo = NULL, *pmedoe;
	float *pPInt1x = NULL, *pPInt1y = NULL, *pPInt2x = NULL, *pPInt2y = NULL, *pPInt3x, *pPInt3y, *pPInt4x, *pPInt4y;	// intersection points arrays
	float *pShift = NULL;

	float *pWidthS = NULL, *pWminS = NULL, *pmedoS = NULL;
	float *pPInt1Sx = NULL, *pPInt1Sy = NULL, *pPInt2Sx = NULL, *pPInt2Sy = NULL, *pPInt3Sx, *pPInt3Sy, *pPInt4Sx, *pPInt4Sy;	// intersection points arrays
	float *pShiftS = NULL;

	//Defect pointers for FWM /
	int SPnum, MBnum;
	float pDefMB[ND*MAX_FWM_DEFECT]; // defects MB
	float pDefMBprc[ND*MAX_FWM_DEFECT]; // defects MB%
	float pXdefMB[ND*MAX_FWM_DEFECT]; // defect coordinates x MB
	float pYdefMB[ND*MAX_FWM_DEFECT]; // defect coordinates y MB
	float pDefSP[ND*MAX_FWM_DEFECT]; // defects SP
	float pDefSPprc[ND*MAX_FWM_DEFECT]; // defects SP%
	float pXdefSP[ND*MAX_FWM_DEFECT]; // defect coordinates x 
	float pYdefSP[ND*MAX_FWM_DEFECT]; // defect coordinates y 

	//Defect pointers for FWMS (narrow space) /
	int SPSnum, MBSnum;// SPACE DEFETCS NUMBER

	float pDefMBS[ND*MAX_FWM_DEFECT]; // defects MBS
	float pDefMBSprc[ND*MAX_FWM_DEFECT]; // defects MBS%
	float pXdefMBS[ND*MAX_FWM_DEFECT]; // defect coordinates x MBS
	float pYdefMBS[ND*MAX_FWM_DEFECT]; // defect coordinates y MBS
	float pDefSPS[ND*MAX_FWM_DEFECT]; // defects SPS
	float pDefSPSprc[ND*MAX_FWM_DEFECT]; // defects SPS%
	float pXdefSPS[ND*MAX_FWM_DEFECT]; // defect coordinates x 
	float pYdefSPS[ND*MAX_FWM_DEFECT]; // defect coordinates y 


	/// ????
	int *pPuseint = NULL, *pPuseintS = NULL;
	///


public:
	void SubPixCrdS(int *g0, int i0, int *PPx, int *PPy, float *pIntX, float *pIntY, float thr);
	float SubpixS1sq(UINT8 *pIm, int i0, int *PPx, int *PPy, float thr, int imw);
	void CPU_MSSpProc36B2(UINT8 *pIm, UINT16 *pImIRBS, UINT16 *pImCLCS, int imw, int imh, int pst, int pn,
		float thr, int dclcmax, float *dP1x, float *dP1y, float *dP2x, float *dP2y, int *dPnum, int *dPnum2,
		float *dW, float *dWmin, float *dPint1x, float *dPint1y, float *dPint2x, float *dPint2y, float *dShift, int *dPuseint);
	void CPU_SpMeasGray(UINT8 *pIm, int imw, int imh, int pn,
		float *dP1x, float *dP1y, float *dP2x, float *dP2y,
		float *dGraymin);
	void SubPixCrdP(int *g0, int i0, int *PPx, int *PPy, float *pIntX, float *pIntY, float thr);
	void CPU_MSPtProc39B2(UINT8 *pIm, UINT16 *pImIRB, UINT16 *pImCLC, int imw, int imh, int pst, int pn,
		float thr, int dclcmax, float *dP1x, float *dP1y, float *dP2x, float *dP2y, int *dPnum, int *dPnum2,
		float *dW, float *dWmin, float *dPint1x, float *dPint1y, float *dPint2x, float *dPint2y, float *dShift, int *dPuseint);
	void CPU_MSPtProc40B2(UINT8 *pIm, UINT16 *pImIRB, UINT16 *pImCLC, int imw, int imh, int pst, int pn,
		float thr, int dclcmax, float *dP1x, float *dP1y, float *dP2x, float *dP2y, int *dPnum, int *dPnum2,
		float *dW, float *dWmin, float *dPint1x, float *dPint1y, float *dPint2x, float *dPint2y, float *dShift, int *dPuseint);

	void FWMS_PostProcCPUnew(float *pwS, int *pwgS, float *medoS, float *pXCS, float *pYCS, int naS, int nPS, int nvoid, int *pPaddrS,
		int sz1, int sz2, int skip, float ang, float mprc, float sprc, float mabs, float sabs, float minmbs, float minWspace, int dlen,
		float *pDefMB, float *pDefMBprc, float *pXdefMB, float *pYdefMB, float *pDefSP, float *pDefSPprc, float *pXdefSP, float *pYdefSP, int *MBnumS, int *SPnumS);
	void FWMS_PostProcCPU(float *pwS, int *pwgS, float *medoS, float *pXCS, float *pYCS, int naS, int nPS, int nvoid, int *pPaddrS,
		int sz1, int sz2, int skip, float ang, float mprc, float sprc, float mabs, float sabs, float minmbs, float minWspace, int dlen,
		float *pDefMB, float *pDefMBprc, float *pXdefMB, float *pYdefMB, float *pDefSP, float *pDefSPprc, float *pXdefSP, float *pYdefSP, int *MBnumS, int *SPnumS);
	void FWM_PostProcCPU(float *pw, int *pwg, float *medo, float *pXC, float *pYC, int nC, int n, int *pPaddr, int sz1, int skip, float mprc, float sprc, float mabs, float minwp, float pabs, int dlen,
		float *pDefMBa, float *pDefMBprc, float *pXdefMB, float *pYdefMB, float *pDefSPa, float *pDefSPprc, float *pXdefSP, float *pYdefSP, int *MBnum, int *SPnum);
	void FWM_PostProcCPUNew(float *pw, int *pwg, float *medo, float *pXC, float *pYC, int nC, int n, int *pPaddr, int sz1, int skip, float mprc, float sprc, float mabs, float minwp, float narpwidth, float widepwidth, float stdgpwidth, float pabs, int dlen,
		float *pDefMBa, float *pDefMBprc, float *pXdefMB, float *pYdefMB, float *pDefSPa, float *pDefSPprc, float *pXdefSP, float *pYdefSP, int *MBnum, int *SPnum);
	void FWM_PostProcCPU_2C(float *pw, int *pwg, float *medo, float *pXC, float *pYC, int nC, int n, int *pPaddr, int sz1, int skip, float mprc, float sprc, float mabs, float minwp, float narpwidth, float widepwidth, float stdgpwidth, float pabs, int dlen,
		float *pDefMBa, float *pDefMBprc, float *pXdefMB, float *pYdefMB, float *pDefSPa, float *pDefSPprc, float *pXdefSP, float *pYdefSP, int *dPuseint, int *MBnum, int *SPnum);

	void FWM_PostProcCPUI(float *pw, int *pwg, float *medo, float *pXC, float *pYC, int n, int sz1, int skip, float mprc, float sprc, float mabs, float pabs, int dlen,
		float *pDefMB, float *pXdefMB, float *pYdefMB, float *pDefSP, float *pXdefSP, float *pYdefSP, int *MBnum, int *SPnum);

	void Alloc(int np, int nps);
	void Alloc2(int np, int nps);

	void SrunmedSLI(float* y, float* smo, long n, int bw_tot, int end_rule, int skip);

	




private:
	int compare(const void* a, const void* b);
	void SrunmedSLIExt(float* yinp, float* y/*extended*/, float* smo, long n1, int bw_tot, int end_rule, int skip);
	
	void SrunmedProto(float* y, float* smo, long n, int bw_tot, int skip); // 2022.08.01 - running median with skipping [skip] values, with linear interpolation
	void SrunmedSLIExtNoS(float* yinp, float* y/*extended*/, float* smo, long n1, int bw_tot, /*int end_rule,*/ int skip, int noskipsz); // 2022.08.01 - running median with skipping [skip] values, with linear interpolation




};
//	int thr;
//	int dclcmax;
//	/// for FWM ***
//	//int Np;		// number of MSs
//	//int Na;		// number of addresses (SkeletonsGthin regions)
//	//int NaC;		// number of addresses (contours)
//
//	/// for FWMs ***
//	//int NpS;		// number of MSSs
//	//int NaS;		// number of addresses (SkeletonsGthinS regions)
//	//int NaCS;		// number of addresses (contoursS)
//
//	// teaching:
//	// FWM
//	//UINT8 *pIm, *Empty;
//	//UINT16 *pImIRBP, *pImIRBS, *pImCLC, *pImCLCS;// image pointers
//
//	//int mmimw, mmimh;  // image width/height
//	//int *pPnum, *pPnum2, *pPaddrR, *pPaddrC, *pPuseint;
//	//float *pP1x, *pP1y, *pP2x, *pP2y, *pP3x, *pP3y; // measurement tool points arrays
//	// proc. results:
//	float *pWidth, *pWmin, *pmedo, *pmedoe;
//	float *pPInt1x, *pPInt1y, *pPInt2x, *pPInt2y, *pPInt3x, *pPInt3y, *pPInt4x, *pPInt4y;	// intersection points arrays
//	float *pShift;
//
//
//	// --- post proc. results: -------------------------------------------
//	//float *pDefMB, *pXdefMB, *pYdefMB, *pDefSP, *pXdefSP, *pYdefSP;
//	//int  *pWG, *pWGS;
//	// FWMS
//	//int *pPnumS, *pPnum2S, *pPaddrRS, *pPaddrCS, *pPuseintS;
//	//float *pP1Sx, *pP1Sy, *pP2Sx, *pP2Sy, *pP3Sx, *pP3Sy; // measurement tool points arrays
//
//	// --- proc. results: ------------------------------------------------
//	float *pWidthS, *pWminS, *pmedoS;
//	float *pPInt1Sx, *pPInt1Sy, *pPInt2Sx, *pPInt2Sy, *pPInt3Sx, *pPInt3Sy, *pPInt4Sx, *pPInt4Sy;	// intersection points arrays
//	float *pShiftS;
//	// post proc. results:
//	//float *pDefMBS, *pXdefMBS, *pYdefMBS, *pDefSPS, *pXdefSPS, *pYdefSPS;
//
//
//	//Defect pointers for FWM /
//	int SPnum, MBnum;
//	float pDefMB[ND*MAX_FWM_DEFECT]; // defects MB
//	float pXdefMB[ND*MAX_FWM_DEFECT]; // defect coordinates x MB
//	float pYdefMB[ND*MAX_FWM_DEFECT]; // defect coordinates y MB
//	float pDefSP[ND*MAX_FWM_DEFECT]; // defects SP
//	float pXdefSP[ND*MAX_FWM_DEFECT]; // defect coordinates x 
//	float pYdefSP[ND*MAX_FWM_DEFECT]; // defect coordinates y 
//
//	//Defect pointers for FWMS (narrow space) /
//	int SPSnum, MBSnum;// SPACE DEFETCS NUMBER
//
//	float pDefMBS[ND*MAX_FWM_DEFECT]; // defects MBS
//	float pXdefMBS[ND*MAX_FWM_DEFECT]; // defect coordinates x MBS
//	float pYdefMBS[ND*MAX_FWM_DEFECT]; // defect coordinates y MBS
//	float pDefSPS[ND*MAX_FWM_DEFECT]; // defects SPS
//	float pXdefSPS[ND*MAX_FWM_DEFECT]; // defect coordinates x 
//	float pYdefSPS[ND*MAX_FWM_DEFECT]; // defect coordinates y 
//
////  H-objects and tuples ////////////////////////////////////////////////////////////////////////////////////////////
//	//HObject  ho_Im, ho_ImCLC;
//	//HObject  ho_ImIRBP, ho_ImIRBS/*, ho_ImCLCS*/;
//	//HTuple hv_P1x, hv_P1y, hv_P2x, hv_P2y, hv_P3x, hv_P3y, hv_Pnum, hv_Pnum2, hv_PaddrR, hv_PaddrC, hv_WG, hv_Wi, hv_Puseint/*, hv_TInoDtoIRB*/;
//
//	//HObject  *ho_pIm, *ho_pImIRBP, *ho_pImIRBS, *ho_pImCLC, *ho_pImCLCS, *ho_G, *ho_WidthImageP, *ho_DirImageP, *ho_WidthImageS, *ho_DirImageS;
//	//HObject  ho_RegionsSpur, ho_RegionsMB, ho_RegionsSpurOut, ho_RegionsMBOut;
//	//HTuple hv_pn, hv_nst; //MS point number, start
//	//HTuple hv_P1Sx, hv_P1Sy, hv_P2Sx, hv_P2Sy, hv_P3Sx, hv_P3Sy, hv_PnumS, hv_Pnum2S, hv_PaddrRS, hv_PaddrCS, hv_WGS, hv_WiS, hv_PuseintS;
//
//	//HObject  ho_Gi, ho_R /* non-processing region*/, ho_DomainG;
//	//HObject  ho_RegionsG, ho_RegionsGS, ho_RegionG, ho_RegionGS, ho_RegionGm;
//	//HObject  ho_RegionGSe, ho_SkeletonG, ho_SkeletonGi, ho_SkeletonsGbr, ho_SkeletonGs, ho_RegionsPN, ho_DomainI;
//	//HObject  ho_RP, ho_RegionI, ho_RegionsI, ho_RegionIS, ho_RegionM;
//	//HObject  ho_RegionE, ho_RegionsE, ho_RegionsM, ho_RegionsMphc, ho_RegionsMphc1;
//	//HObject  ho_RegionsEisc, ho_RegionsEisc1, ho_RegionsPH, ho_RegionsIsland;
//	//HObject  ho_RegionInoIsl, ho_RegionsInoIsli, ho_RegionsInoIsl;
//	//HObject  ho_ImageIRB, ho_RegionsIA, ho_RegionIA, ho_RegionsINB;
//	//HObject  ho_RegionINB, ho_RPInt1;
//	// 210918 add-ons ================================================================
//	//HObject  ho_RegionInoD;		// real image region with defects removed (PH,Isl, Open, Short)
//	//HObject  ho_RegionInoDC;		// complement to real image region with defects removed (PH,Isl, Open, Short)
//	//HObject  ho_SkeletonsFWM;	// Skeleton parts regions for FWM
//	//HObject  ho_BoundariesNFWM; // CAD region boundary parts  for Non-FWM
//	//HObject  ho_RegionsPNi;		// Preparation region for ImageIRB based on CAD
//	//HObject  ho_SkelsEPUver;
//
//	//HTuple hv_tst, hv_l, hv_R, hv_C, hv_L, hv_Phi, hv_j, hv_len, hv_s1, hv_s2, hv_s, hv_WindowHandle, hv_WindowHandle2;
//	//HTuple hv_ExpSkels, hv_nPol, hv_v, hv_h, hv_w, hImIRB, hv_Type;
//	//HObject ho_PolygonsI, ho_Pol, /*ho_ImIRB,*/ ho_ExpSkels, ho_ImPolVx, ho_ImPolVy, ho_ImL;
//	//HObject ho_IRB, ho_RegInt;
//	/*  for Non-FWM*/
//	//HTuple hv_P1Nx, hv_P1Ny, hv_P2Nx, hv_P2Ny, hv_PnumN, hv_PaddrCN, hv_PaddrRN, hv_PindCN, hv_PregN;
//	//HTuple hIm;
//	/// for FWMS
//	//HObject ho_IRBS;
//
//
//
//	void SubPixCrdS(int *g0, int i0, int *PPx, int *PPy, float *pIntX, float *pIntY, float thr);
//	void CPU_MSSpProc36B2(UINT8 *pIm, UINT16 *pImIRBS, UINT16 *pImCLCS, int imw, int imh, int pst, int pn,
//		float thr, int dclcmax, float *dP1x, float *dP1y, float *dP2x, float *dP2y, int *dPnum, int *dPnum2,
//		float *dW, float *dWmin, float *dPint1x, float *dPint1y, float *dPint2x, float *dPint2y, float *dShift, int *dPuseint);
//	void SubPixCrdP(int *g0, int i0, int *PPx, int *PPy, float *pIntX, float *pIntY, float thr);
//	void CPU_MSPtProc39B2(UINT8 *pIm, UINT16 *pImIRB, UINT16 *pImCLC, int imw, int imh, int pst, int pn,
//		float thr, int dclcmax, float *dP1x, float *dP1y, float *dP2x, float *dP2y, int *dPnum, int *dPnum2,
//		float *dW, float *dWmin, float *dPint1x, float *dPint1y, float *dPint2x, float *dPint2y, float *dShift, int *dPuseint);
//	void FWMS_PostProcCPU(float *pwS, int *pwgS, float *medoS, float *pXCS, float *pYCS, int naS, int nPS, int nvoid, int *pPaddrS,
//		int sz1, int sz2, int skip, float ang, float mprc, float sprc, float mabs, float sabs, float minmbs, int dlen, 
//		float *pDefMB, float *pXdefMB, float *pYdefMB, float *pDefSP, float *pXdefSP, float *pYdefSP, int *MBnumS, int *SPnumS);

