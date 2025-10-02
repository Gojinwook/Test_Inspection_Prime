/**
 * \copyright (C) Synapse Imaging 2023
 *
 * @File Name
 *      PinholeIsland.h
 *
 * @Summary
 *
 * @Description
 *
 *
 * @authors Igor Dunin-Barkowski, Iurii Kim
 * @version 1.0
 * @date 2023.09.22
 */

/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
#pragma once


/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */




/* ************************************************************************** */
/* Section: Function Prototypes                                               */
/* ************************************************************************** */
void ThresholdPHIsp(HObject ho_Im, HObject ho_RegProc, HObject *ho_RegionsPH, HObject *ho_RegionsIsl,
	HObject *ho_RegionISnoPhnoIsl, HObject *ho_RegionInoPhnoIsl, HObject *ho_RegionI,
	HTuple hv_thr, HTuple hv_PHrmin, HTuple hv_PHamin, HTuple hv_Islrmin, HTuple hv_Islamin, HTuple hv_PHash, HObject *m_local_ImModified);
void ImagePrepThr(HObject ho_Im, HObject ho_R, HObject *ho_Domain,
	HObject *ho_RegionI, HObject *ho_RegionIi, HTuple hv_thr, HTuple *hv_Width, HTuple *hv_Height);
void PrepRC(HObject ho_Im, HObject ho_R, HObject *ho_Domain, HObject *ho_RC);


/* ************************************************************************** */
/* End of file                                                                */
/* ************************************************************************** */
