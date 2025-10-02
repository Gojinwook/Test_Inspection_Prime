#pragma once
//#include "afx.h"
//#include "pch.h"
/* ************************************************************************** */
/* Section: Prototype functions                                               */
/* ************************************************************************** */
void BuildIRBP7B2(HObject ho_SkeletonsFWM, HObject ho_RegionInoD, HObject ho_ImIRBP,
	HObject ho_RegionsPNi, HObject *ho_ImIRBPOut, HTuple hv_expsize, HTuple hv_nEmpty,
	HTuple hv_dil, HTuple *hv_nFWMP);
//void BuildIRBS4B2(HObject ho_SkeletonsFWMS, HObject ho_RegionInoD, HObject ho_ImIRBS,
//	HObject ho_SkelSEP, HObject *ho_ImIRBSOut, HTuple hv_expsize, HTuple hv_nEmpty,
//	HTuple hv_dil, HTuple *hv_nFWMSB);
void BuildIRBS4B2(HObject ho_SkeletonsFWMS, HObject ho_RegionInoD, HObject ho_ImIRBS,
	HObject *ho_ImIRBSOut, HTuple hv_expsize, HTuple hv_nEmpty, HTuple hv_dil, HTuple *hv_nFWMSB);
void BuildRegionInoD(HObject ho_RegionsShi, HObject ho_SkeletonGS, HObject ho_RegionInoIslnoPh,
	HObject ho_RegionOpCon, HObject ho_R, HObject *ho_RegionInoD);
void BuildIRBS5B2(HObject ho_SkeletonsFWMS, HObject ho_SkeletonsFWMScut, HObject ho_ImIRBS,
	HObject ho_RegionInoD, HObject *ho_ImIRBSOut, HTuple hv_expsize, HTuple hv_nEmpty,
	HTuple hv_dil, HTuple *hv_nFWMSB);
void BuildIRBS6B2(HObject ho_SkeletonsFWMS, HObject ho_RegionInoD, HObject ho_ImIRBS,
	HObject ho_SkelSEP, HObject ho_RegionCut, HObject *ho_ImIRBSOut, HTuple hv_expsize,
	HTuple hv_nEmpty, HTuple hv_dil, HTuple *hv_nFWMSB);
void BuildIRBS41B2(HObject ho_SkeletonsFWMS, HObject ho_RegionInoD, HObject ho_ImIRBS,
	HObject ho_SkelEP, HObject *ho_ImIRBSOut, HTuple hv_expsize, HTuple hv_nEmpty,
	HTuple hv_dil, HTuple *hv_nFWMSB);
void BuildIRBS42B2(HObject ho_SkeletonsFWMS, HObject ho_RegionInoD, HObject ho_ImIRBS,
	HObject ho_SkelEP, HObject *ho_ImIRBSOut, HTuple hv_expsize, HTuple hv_nEmpty,
	HTuple hv_dil, HTuple *hv_nFWMSB);
void BuildIRBS42B3(HObject ho_SkeletonsFWMS, HObject ho_RegionInoD, HObject ho_ImIRBS,
	HObject ho_DirImageS, HObject ho_WidthImageS, HObject ho_SkeletonGS, HObject ho_SkeletonsFWMScut,
	HObject *ho_ImIRBSOut, HTuple hv_expsize, HTuple hv_nEmpty, HTuple hv_dil, HTuple hv_stopperT,
	HTuple *hv_nFWMSB);
void BuildIRBS42B3_GM(HObject ho_SkeletonsFWMS, HObject ho_RegionInoD, HObject ho_ImIRBS,
	HObject ho_DirImageS, HObject ho_WidthImageS, HObject ho_SkeletonGS, HObject ho_SkeletonsFWMScut,
	HObject *ho_ImIRBSOut, HTuple hv_expsize, HTuple hv_nEmpty, HTuple hv_dil, HTuple hv_stopperT,
	HTuple *hv_nFWMSB);
void BuildIRBS42B4(HObject ho_SkeletonsFWMS, HObject ho_RegionInoD, HObject ho_ImIRBS,
	HObject ho_DirImageS, HObject ho_WidthImageS, HObject ho_SkeletonGS, HObject ho_SkeletonsFWMScut,
	HObject *ho_ImIRBSOut, HTuple hv_expsize, HTuple hv_nEmpty, HTuple hv_dil, HTuple hv_stopperT,
	HTuple *hv_nFWMSB);

/* ************************************************************************** */
/* End of file                                                                */
/* ************************************************************************** */