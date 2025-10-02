/**
 * \copyright (C) Synapse Imaging 2024
 *
 * @File Name
 *      Our_Algorithms.h
 *
 * @Summary
 *
 * @Description
 *
 *
 * @authors Igor Dunin-Barkowski, Iurii Kim
 * @version 1.0
 * @date 8/27/2024 6:51:27 AM
 */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
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

#else
#define FWMDLL_API

#endif


/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */
#define POINTS_SUB_LEN	1000000
#define REGION_SUB_CNT	1000

struct sub_buff_t
{
	double *rows_sub = NULL;
	double *cols_sub = NULL;
	int *len_sub = NULL;
	int reg_count;
};


FWMDLL_API int fnFWMDLL(void);

/* ************************************************************************** */
/* Section: Function Prototypes                                               */
/* ************************************************************************** */
FWMDLL_API void Alloc_Sub(sub_buff_t *m_buff);
FWMDLL_API bool Test_Hole(UINT8 *pIm, int im_w, int im_h, UINT64* rows, UINT64* cols, int count_len, int thr);
FWMDLL_API UINT8 Get_Gray_Val(UINT8 *pIm, int im_w, int im_h, int y, int x);
FWMDLL_API int Region_Threshold_SubPix(HObject ho_Im, HObject ho_RegSel, int thr, double *rows_sub, double *cols_sub);
void Example_Sub(HTuple file_name_Im, HTuple file_name_save);
/* ************************************************************************** */
/* End of file                                                                */
/* ************************************************************************** */

