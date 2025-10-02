/**
 * \copyright (C) Synapse Imaging 2023
 *
 * @File Name
 *      OpenShort.h
 *
 * @Summary
 *
 * @Description
 *
 *
 * @authors Igor Dunin-Barkowski, Iurii Kim
 * @version 1.0
 * @date 2023.09.24
 */
#pragma once
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */

/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */


/* ************************************************************************** */
/* Section: Prototype functions                                               */
/* ************************************************************************** */
void MakeSkeletonGnoR(HObject ho_Im, HObject ho_RegionI, HObject ho_R, HObject ho_SkeletonG,
	HObject *ho_SkeletonGnoR);
void SkeletonsCheckPrep(HObject ho_RegionFWM, HObject ho_RegionInoIslnoPh, HObject *ho_RegionFWMdil,
	HObject *ho_RegSkel, HObject *ho_RegSkeld, HObject *ho_SkeletonI, HObject *ho_EndPointsI,
	HObject *ho_JuncPointsI, HObject *ho_SkelPointsI, HObject *ho_RegionCheck, HTuple hv_dilFWM,
	HTuple hv_s1, HTuple hv_dilS);
void DetectShortN4(HObject ho_SkeletonGS, HObject ho_RegionInoPHnoIsl, HObject ho_RegionISnoPHnoIsl,
	HObject ho_WidthImageS, HObject ho_DirImageS, HObject ho_RegionG, HObject ho_RegionFWM,
	HObject ho_RegionCheck, HObject *ho_RegionsSh);
void DetectOpenN5(HObject ho_RegionG, HObject ho_SkeletonG, HObject ho_RegionInoPHnoIsl,
	HObject ho_RegionISnoPHnoIsl, HObject ho_WidthImageP, HObject ho_DirImageP, HObject ho_RegionFWM,
	HObject ho_RegionCheck, HObject *ho_RegionsOp, HObject *ho_RegionsOpCon);
int DetectShortN5(HObject ho_SkeletonGS, HObject ho_RegionInoPHnoIsl, HObject ho_RegionISnoPHnoIsl,
	HObject ho_WidthImageS, HObject ho_DirImageS, HObject ho_RegionG, HObject ho_RegionFWM,
	HObject ho_RegionCheck, HObject *ho_RegionsSh, HTuple hv_short_max_active);
//int DetectShortN6(HObject ho_SkeletonGS, HObject ho_RegionInoPHnoIsl, HObject ho_RegionISnoPHnoIsl,
//	HObject ho_WidthImageS, HObject ho_DirImageS, HObject ho_RegionG, HObject ho_RegionFWM,
//	HObject ho_RegionCheck, HObject ho_RegNoProc, HObject *ho_RegionsSh, HTuple hv_short_max_active);
void DetectShortN6(HObject ho_SkeletonGS, HObject ho_RegionInoPHnoIsl, HObject ho_RegionISnoPHnoIsl,
	HObject ho_WidthImageS, HObject ho_DirImageS, HObject ho_RegionG, HObject ho_RegionFWM,
	HObject ho_RegionCheck, HObject ho_RegNoProc, HObject *ho_RegionsSh);


/* ************************************************************************** */
/* End of file                                                                */
/* ************************************************************************** */
