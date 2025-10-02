/**
 * \copyright (C) Synapse Imaging 2024
 *
 * @File Name
 *      LowGray.h
 *
 * @Summary
 *
 * @Description
 *
 *
 * @authors Igor Dunin-Barkowski, Iurii Kim
 * @version 1.0
 * @date 3/15/2024 1:07:56 PM
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
void LowGrayProc(HObject ho_SkeletonsFWMS, HObject ho_Im, HObject *ho_RegionLG,
	HTuple hv_maxexp, HTuple hv_thr, HTuple hv_autothr, HTuple hv_fixedthr, HTuple hv_dt,
	HTuple *hv_thrlga);


/* ************************************************************************** */
/* End of file                                                                */
/* ************************************************************************** */

