/**
 * \copyright (C) Synapse Imaging 2024
 *
 * @File Name
 *      Common.h
 *
 * @Summary
 *
 * @Description
 *
 *
 * @authors Igor Dunin-Barkowski, Iurii Kim
 * @version 1.0
 * @date 2/28/2024 11:49:01 AM
 */
 /* ************************************************************************** */
 /* Section: Included Files                                                    */
 /* ************************************************************************** */
#pragma once
#include <sdkddkver.h>
#include <afx.h>

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// M%FC support for Internet Explorer 4 Common Controls


#include <iostream>
#include <vector>
#include <atlstr.h>
#include <fstream>
#include <cmath>

#include <cstring>
#include <string.h>

#include <stdio.h>
#include <basetsd.h>
#include <memory.h>
#include <set>
#include <stdlib.h>
#include <filesystem>

#include <algorithm>

#include "HalconCpp.h"
#include "HDevThread.h"

#include "TSQueueSM.h"

#include "CParams.h"

#include "../Include/json.hpp"
using json = nlohmann::json;

using namespace HalconCpp;
using namespace std;

/** ---------------------------------------------------------------------------
* @Summary
*      Types of inspection
*/
enum INSPECT_TYPE
{
	INSPECT_PANEL,
	INSPECT_ONE_IMAGE,

};
/** ---------------------------------------------------------------------------
* @Summary
*      Structure of Inspection parameters
*/
typedef struct InspectParam_t
{
	int iFovNo;
	int iFovNoX;
	int iFovNoY;

	int iFrameNo;
	int iUnitNo;
	int unit_index;

	HObject image;

	INSPECT_TYPE inspect_type;
} InspectParam_t;


namespace inspect_utils
{

	/* ************************************************************************** */
	/* Section: File Scope or Global Data                                         */
	/* ************************************************************************** */
	extern FILE* fp_console;




	/* ************************************************************************** */
	/* Section: Function Prototypes                                               */
	/* ************************************************************************** */
	void Init_Console();
	int* ConvertTupleToInt(HTuple T);
	float* ConvertTupleToFloat(HTuple T);
	double Tuple2Double(HTuple hv_Any);
	bool SaveTupleToBinInt(CString path_tuple, CString name, CString save_name);
	bool SaveTupleToBinFloat(CString path_tuple, CString name, CString save_name);

	int Read_Bin_File(CString file_Name, int **buff);
	int Read_Bin_File(CString file_Name, float **buff);

	INT64 * ConvertFloatToINT64(float *dp, int len);
	void FloatToTuple(float *dp, int len, HTuple &tuple);
	void Queue_Add(TSQueueSM<CString> *queue, CString *out_str, LPCSTR pszFormat, ...);

	bool SetRegString(CString value_name, CString value);
	bool SetRegBool(CString value_name, bool value);
	bool GetRegString(CString value_name, CString *value);
	bool GetRegBool(CString value_name, bool *value);
	bool GetRegString(CString value_name, CString *value, CString default_value);

	int Folder_Count_Files(CString sImageFileName);
	CString Get_Path_Image(int file_num, CString files_path);

	void Inspect_Init_Log(CString result_path, int pc_type);
	void Inspect_Write_Log(CString log);
	void GetAllFilesNames(CString files_path, CStringArray& files);
	bool Check_Path(CString files_path);
	void Delete_All_Folders(CString folder);
	void ConfirmFolder(CString folder);

	void SrunmedProto(float* y, float* smo, long n, int bw_tot, int skip);
	void SrunmedSLIExtNoS(float* yinp, float* y/*extended*/, float* smo, long n1, int bw_tot, /*int end_rule,*/ int skip, int noskipsz);
	void Moving_Avg(UINT8 *buff, int size, int period, UINT8 *out_buff);
	void Max_Buff(UINT8 *buff, int size, UINT8 &max, int &max_pos);
	void Max_Index(HTuple hv_Max21, HTuple hv_FAbsoluteHistosm1, HTuple *hv_Max_Index);
}


/* ************************************************************************** */
/* End of file                                                                */
/* ************************************************************************** */

