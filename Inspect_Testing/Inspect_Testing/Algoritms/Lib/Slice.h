/**
 * \copyright (C) Synapse Imaging 2024
 *
 * @File Name
 *      Slice.h
 *
 * @Summary
 *
 * @Description
 *
 *
 * @authors Iurii Kim
 * @version 1.0
 * @date 4/3/2024 6:37:21 AM
 */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
#pragma once
#ifdef ALGORITHMSUB_EXPORTS
#define ALGORITHMSUB_API __declspec(dllexport)
#else
#include "HalconCpp.h"
#include "HDevThread.h"
#include <atlstr.h>
using namespace HalconCpp;
using namespace std;

#define ALGORITHMSUB_API __declspec(dllimport)

#endif

class ALGORITHMSUB_API CSlice_Teach
{
public:
	bool m_Readed;
	int *m_Cols_Sens;
	int *m_Rows_Sens;
	int *m_Pointer_Sens;
	int *m_Pointer_Skel;

	bool m_Readed_P;
	int *m_Cols_Sens_P;
	int *m_Rows_Sens_P;
	int *m_Pointer_Sens_P;
	int *m_Pointer_Skel_P;
public:
	bool Read_Teach(const char* path_model);

};


class ALGORITHMSUB_API CSlice
{

	/* ************************************************************************** */
	/* Section: File Scope or Global Data                                         */
	/* ************************************************************************** */
public:
	CSlice_Teach *teach;
	//static bool m_Readed;
	//static int *m_Cols_Sens;
	//static int *m_Rows_Sens;
	//static int *m_Pointer_Sens;
	//static int *m_Pointer_Skel;

	//static bool m_Readed_P;
	//static int *m_Cols_Sens_P;
	//static int *m_Rows_Sens_P;
	//static int *m_Pointer_Sens_P;
	//static int *m_Pointer_Skel_P;
public:
	// *** Параметры инспекции----------

	int m_Threshold;
	int m_SensLen;
	int m_SensLenD2;
	int m_szw;
	int m_prsLG;

	int m_szw_P;
	int m_prsLG_P;
	int m_Avg_Par;


	UINT8 *m_Gray;
	UINT8 *m_Gray_Avg;

	int m_Itr;
	double *m_Thr_Step;
	double *m_Intersect;

private:
	int m_Count_Skel = 0;
	int *m_Loc_Min;
	int *m_Loc_Min_Val;


public:
	int m_size;
	int m_Widt_Pos;
	float *m_Widt_S;
	float *m_Depth_S;
	float *m_Depth_Ppr_S;
	double *m_Rows_Cent;
	double *m_Cols_Cent;


	/* ************************************************************************** */
	/* Section: Function Prototypes                                               */
	/* ************************************************************************** */
public:
	void Convert_To_Bin(CString path_teach);
	void Convert_To_Bin_P(CString path_teach);
	void Init();

	void Proc(UINT8 * pIm, int im_w, int im_h, 
		HTuple *hv_Def_Rows, HTuple *hv_Def_Cols, 
		HTuple &hv_PrcM_Min, HTuple &hv_PrcM_Max, HTuple &hv_PrcM_Median);

	void Defect_Post(
		float * depth_prc, double * rows_cent, double * cols_cent, int len,
		double cv_Proc_Thr,
		HTuple & hv_Def_Rows, HTuple & hv_Def_Cols, HTuple & PrcM_Min, HTuple & PrcM_Max, HTuple & PrcM_Median);

	void Proc_P(UINT8 * pIm, int im_w, int im_h, HTuple * hv_Def_Rows, HTuple * hv_Def_Cols,
		HTuple &hv_PrcM_Min, HTuple &hv_PrcM_Max, HTuple &hv_PrcM_Median);



};



/* ************************************************************************** */
/* End of file                                                                */
/* ************************************************************************** */

