#pragma once
#include "HalconCpp.h"
#include "HDevThread.h"
//#include "HProc.h"

using namespace HalconCpp;

#define ND 1024 // Max. number of MB and SP per skeleton part
#define MAX_FWM_DEFECT			10000
#define MEDOSIZE			    280000


	int thr;
	int dclcmax;
	/// for FWM ***
	int Np;		// number of MSs
	int Na;		// number of addresses (SkeletonsGthin regions)
	int NaC;		// number of addresses (contours)

	/// for FWMs ***
	int NpS;		// number of MSSs
	int NaS;		// number of addresses (SkeletonsGthinS regions)
	int NaCS;		// number of addresses (contoursS)

	// teaching:
	// FWM
	UINT8 *pIm/*, *Empty*/;
	UINT16 *pImIRBP, *pImIRBS, *pImCLC, *pImCLCS;// image pointers

	//int mmimw, mmimh;  // image width/height
	int *pPnum, *pPnum2, *pPaddrR, *pPaddrC, *pPuseint;
	float *pP1x, *pP1y, *pP2x, *pP2y, *pP3x, *pP3y; // measurement tool points arrays
	// proc. results:
	float *pWidth, *pWmin, *pmedo, *pmedoe;
	float *pPInt1x, *pPInt1y, *pPInt2x, *pPInt2y, *pPInt3x, *pPInt3y, *pPInt4x, *pPInt4y;	// intersection points arrays
	float *pShift;




	// post proc. results:
	//float *pDefMB, *pXdefMB, *pYdefMB, *pDefSP, *pXdefSP, *pYdefSP;
	int  *pWG, *pWGS;
	// FWMS
	int *pPnumS, *pPnum2S, *pPaddrRS, *pPaddrCS, *pPuseintS;
	float *pP1Sx, *pP1Sy, *pP2Sx, *pP2Sy, *pP3Sx, *pP3Sy; // measurement tool points arrays
	// proc. results:
	float *pWidthS, *pWminS, *pmedoS;
	float *pPInt1Sx, *pPInt1Sy, *pPInt2Sx, *pPInt2Sy, *pPInt3Sx, *pPInt3Sy, *pPInt4Sx, *pPInt4Sy;	// intersection points arrays
	float *pShiftS;
	// post proc. results:
	//float *pDefMBS, *pXdefMBS, *pYdefMBS, *pDefSPS, *pXdefSPS, *pYdefSPS;


	//Defect pointers for FWM /
	int SPnum, MBnum;
	float pDefMB[ND*MAX_FWM_DEFECT]; // defects MB
	float pXdefMB[ND*MAX_FWM_DEFECT]; // defect coordinates x MB
	float pYdefMB[ND*MAX_FWM_DEFECT]; // defect coordinates y MB
	float pDefSP[ND*MAX_FWM_DEFECT]; // defects SP
	float pXdefSP[ND*MAX_FWM_DEFECT]; // defect coordinates x 
	float pYdefSP[ND*MAX_FWM_DEFECT]; // defect coordinates y 

	//Defect pointers for FWMS (narrow space) /
	int SPSnum, MBSnum;// SPACE DEFETCS NUMBER

	float pDefMBS[ND*MAX_FWM_DEFECT]; // defects MBS
	float pXdefMBS[ND*MAX_FWM_DEFECT]; // defect coordinates x MBS
	float pYdefMBS[ND*MAX_FWM_DEFECT]; // defect coordinates y MBS
	float pDefSPS[ND*MAX_FWM_DEFECT]; // defects SPS
	float pXdefSPS[ND*MAX_FWM_DEFECT]; // defect coordinates x 
	float pYdefSPS[ND*MAX_FWM_DEFECT]; // defect coordinates y 

//  H-objects and tuples ////////////////////////////////////////////////////////////////////////////////////////////
	//HObject  *ho_pIm, *ho_pImIRBP, *ho_pImIRBS, *ho_pImCLC, *ho_pImCLCS, *ho_G, *ho_WidthImageP, *ho_DirImageP, *ho_WidthImageS, *ho_DirImageS;
	//HObject  ho_Gi, ho_R /* non-processing region*/, ho_DomainG;
	HObject  ho_Im, ho_ImCLC;
	//HObject  ho_RegionsSpur, ho_RegionsMB, ho_RegionsSpurOut, ho_RegionsMBOut;
	//HObject  ho_RegionsG, ho_RegionsGS, ho_RegionG, ho_RegionGS, ho_RegionGm;
	//HObject  ho_RegionGSe, ho_SkeletonG, ho_SkeletonGi, ho_SkeletonsGbr, ho_SkeletonGs, ho_RegionsPN, ho_DomainI;
	//HObject  ho_RP, ho_RegionI, ho_RegionsI, ho_RegionIS, ho_RegionM;
	//HObject  ho_RegionE, ho_RegionsE, ho_RegionsM, ho_RegionsMphc, ho_RegionsMphc1;
	//HObject  ho_RegionsEisc, ho_RegionsEisc1, ho_RegionsPH, ho_RegionsIsland;
	//HObject  ho_RegionInoIsl, ho_RegionsInoIsli, ho_RegionsInoIsl;
	//HObject  ho_ImageIRB, ho_RegionsIA, ho_RegionIA, ho_RegionsINB;
	//HObject  ho_RegionINB, ho_RPInt1;
	HObject  ho_ImIRBP, ho_ImIRBS/*, ho_ImCLCS*/;
	// 210918 add-ons ================================================================
	//HObject  ho_RegionInoD;		// real image region with defects removed (PH,Isl, Open, Short)
	//HObject  ho_RegionInoDC;		// complement to real image region with defects removed (PH,Isl, Open, Short)
	//HObject  ho_SkeletonsFWM;	// Skeleton parts regions for FWM
	//HObject  ho_BoundariesNFWM; // CAD region boundary parts  for Non-FWM
	//HObject  ho_RegionsPNi;		// Preparation region for ImageIRB based on CAD
	//HObject  ho_SkelsEPUver;

	//HTuple hv_tst, hv_l, hv_R, hv_C, hv_L, hv_Phi, hv_j, hv_len, hv_s1, hv_s2, hv_s, hv_WindowHandle, hv_WindowHandle2;
	//HTuple hv_ExpSkels, hv_nPol, hv_v, hv_h, hv_w, hImIRB, hv_Type;
	//HObject ho_PolygonsI, ho_Pol, /*ho_ImIRB,*/ ho_ExpSkels, ho_ImPolVx, ho_ImPolVy, ho_ImL;
	//HObject ho_IRB, ho_RegInt;
	HTuple hv_P1x, hv_P1y, hv_P2x, hv_P2y, hv_P3x, hv_P3y, hv_Pnum, hv_Pnum2, hv_PaddrR, hv_PaddrC, hv_WG, hv_Wi, hv_Puseint/*, hv_TInoDtoIRB*/;
	/*  for Non-FWM*/
	//HTuple hv_P1Nx, hv_P1Ny, hv_P2Nx, hv_P2Ny, hv_PnumN, hv_PaddrCN, hv_PaddrRN, hv_PindCN, hv_PregN;
	//HTuple hv_pn, hv_nst; //MS point number, start
	//HTuple hIm;
	/// for FWMS
	//HObject ho_IRBS;
	HTuple hv_P1Sx, hv_P1Sy, hv_P2Sx, hv_P2Sy, hv_P3Sx, hv_P3Sy, hv_PnumS, hv_Pnum2S, hv_PaddrRS, hv_PaddrCS, hv_WGS, hv_WiS, hv_PuseintS;



	extern void SubPixCrdS(int *g0, int i0, int *PPx, int *PPy, float *pIntX, float *pIntY, float thr);
	extern void CPU_MSSpProc36B2(UINT8 *pIm, UINT16 *pImIRBS, UINT16 *pImCLCS, int imw, int imh, int pst, int pn,
		float thr, int dclcmax, float *dP1x, float *dP1y, float *dP2x, float *dP2y, int *dPnum, int *dPnum2,
		float *dW, float *dWmin, float *dPint1x, float *dPint1y, float *dPint2x, float *dPint2y, float *dShift, int *dPuseint);
	extern void SubPixCrdP(int *g0, int i0, int *PPx, int *PPy, float *pIntX, float *pIntY, float thr);
	extern void CPU_MSPtProc39B2(UINT8 *pIm, UINT16 *pImIRB, UINT16 *pImCLC, int imw, int imh, int pst, int pn,
		float thr, int dclcmax, float *dP1x, float *dP1y, float *dP2x, float *dP2y, int *dPnum, int *dPnum2,
		float *dW, float *dWmin, float *dPint1x, float *dPint1y, float *dPint2x, float *dPint2y, float *dShift, int *dPuseint);
	
	extern void FWMS_PostProcCPU(float *pwS, int *pwgS, float *medoS, float *pXCS, float *pYCS, int naS, int nPS, int nvoid, int *pPaddrS,
		int sz1, int sz2, int skip, float ang, float mprc, float sprc, float mabs, float sabs, float minmbs, int dlen, 
		float *pDefMB, float *pXdefMB, float *pYdefMB, float *pDefSP, float *pXdefSP, float *pYdefSP, int *MBnumS, int *SPnumS);

