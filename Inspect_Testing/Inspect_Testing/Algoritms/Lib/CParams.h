#pragma once


#ifdef EXPORTS_DLL
#include "CDialog_Params.h"
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

#define WM_PARAM_CHANGED_EVENTS				WM_USER + 500
#define WM_PARAM_APPLY_EVENTS				WM_USER + 501
#define WM_PARAM_SAVE_EVENTS				WM_USER + 502

using namespace HalconCpp;
using namespace std;


struct Inspection_Params
{
	/// --------------------------------------------
	/// TODO: OpenShort2: Add this to GUI
	/// --------------------------------------------
	int cv_Min_CAD_Area_Open = -1;						// Minimum allowed CAD region area for identifying fill numbers Open
	int cv_Min_CAD_Area_Short = -1;						// Minimum allowed CAD region area for identifying fill numbers Short
	int cv_Gray_Gap_Open = -1;							// Acceptable gray gap Open
	int cv_Gray_Gap_Short = -1;							// Acceptable gray gap Shore

	int check_diagonals = 1;							// Check diagonal pixels when filling

	int open_short_GM = 0;
	int open_threshold = 0;
	int short_threshold = 0;

	/// --------------------------------------------

	// --- OpenShort ------------------------------------
	HTuple hv_short_max_active = 0; // Max def. on// specifies if maximal number of defects per unit is used(or not)
	int max_shorts_num = 100;		// Max def.num //maximal number of defects per unit 
	HTuple hv_dilS = 9.5;			// dilation value for open/short prep.
	//HTuple hv_ClS = 2.5;			// closing value for shorts
	//HTuple hv_dil = 9;			// IRB dil.   //dilation value for building IRB




	/// TODO: Take from parameters
	/// --------------------------------------------

	bool union_defects = true;
	int union_defect_dist = 30;



	//  Thresholding
	bool inspect_Auto_Thr = false;  // Auto thresholding //specifies if use automatic threshold (or not)
	bool inspect_Preproc = false;  // Auto thresholding //specifies if use automatic threshold (or not)
	int threashold = 150;			// Main threshold

	// Inspection stages
	bool inspect_Pinhole = false;   // Inspect Pinholes// specifies if inspect pinholes (or not)
	bool inspect_Island = false;    // Inspect Island // specifies if inspect islands (or not)
	bool inspect_Open = false;		// Inspect Open // specifies if inspect opens (or not)
	bool inspect_Short = false;		// Inspect Short // specifies if inspect shorts (or not)
	bool inspect_Open2 = true;
	bool inspect_Short2 = true;		// Inspect Short2 // specifies if inspect shorts v2 (or not)

	bool inspect_LowGary = true;	// Inspect Low Gray // specifies if inspect low gray level defects  (or not)
	bool inspect_LowGary_P = true;	// 저명도 결함(패턴) 검사 여부 // inspect low gray pattern

	bool inspect_FWMS = true; // FWMS 검사 여부 // inspect FWMS
	bool inspect_FWMS_PT = true; // FWMS(PT) 검사 여부 // inspect FWMS PT
	bool inspect_FWMS_MB = true; // FWMS(MB) 검사 여부 // inspect FWMS MB
	bool inspect_FWM = true; // FWM 검사 여부 // inspect FWM
	bool inspect_FWM_Spur = true; // FWM(스퍼) 검사 여부 // inspect FWM spur
	bool inspect_FWM_MB = true; // FWM(MB) 검사 여부 // inspect FWM MB


	// --- Pinhole_Island
	float PHrmin = 3.0;				// PH min.rad. // minimal pinhole internal radius
	int PHamin = 5;					//PH min.area // minimal pinhole area
	int Islrmin = 2;				// Isl.min.rad // minimal island internal radius
	int Islamin = 10;				// Isl.min.area // minimal island area

	HTuple hv_PHash = 200; // if pinhole area is bigger the value, than it's not removed from ho_RegionInoPhnoIsl (used in ThresholdPHIsp() )




	// Build_IRB -------------------------------------
	HTuple hv_expsize = 251;		// IRB expand // 19.5; //region expansion value for building IRB
	HTuple hv_dil_S = 7;			// Space IRB 생성을 위한 확장 값 // IRB dil. dilation value for building IRB
	HTuple hv_dil_P = 11;			// Pattern IRB 생성을 위한 확장 값 // IRB dil. dilation value for building IRB
	HTuple hv_stopperT = 7;



	// --- FWM ----------------------------------------
	int lMB = 1;			// MB length
	float prcMB = 40.; 		// MB % // % nearby for MB (Pattern)
	float absMB = 30.;		//  absolute MB value (Pattern)(in pixels)
	float minMB = 1.5;		//  minimal MB value (in pixels)
	float prcSP = 30.; 		// % nearby  for spur (Pattern)
	float absSP = 1.5;		//  absolute spur value (in pixels)
	float minSP = 10;		//  minimal spur value (in pixels)
	float narrowwp = 7.;	//  pattern width to detect as narrow
	float widewp = 24.;	//  pattern width to detect as too wide

	float minwp = 10.;		//  minimal pattern width to use %
	int sz1 = 50;			// Running median sizes for FWM 
	HTuple hv_dilFWM = 10;			// dilation value for FWM (used in SkeletonsCheckPrep() )

	float m_kw = 0.7;
	float m_dw = 10;

	float fwm_Wmin = 0;
	float fwm_Wmax = 0;
	float fwm_Wmean = 0;
	float fwm_Wmed = 0;

	// --- FWMs --------------------------------------
	int nvoid = 7; ///  not used points number in the begining and end of skeleton part
	int lMBS = 0;// 1;  // defect length for  space MB (narrow space)

	int sz2 = 50;			// Running median sizes for FWMS
	float ang = 3.;			// angle for defect 
	int rmskip = 0;			// skip value in run. med.
	int dlen = 3;			// (not used)
	float prcMBS = 30.; 		// % nearby for MB (Space)
	float absMBS = 10.;		//  absolute MB value (Space)(in pixels)

	float minMBS = 4.5;		//  minimal MB value (in pixels) for % Space
	float narWSpace = 0.4;  // narrow width for space (if less then it's a short defect)

	float minWSpace = 0.5;	// 최소 공간 너비 (Shortdyd) // minimal width for space

	//float prcMB = 40.;
	//float prcSP = 40.;		// Prc. for MB ans SP
	//float absMB = 10.;
	//
	//int lMB = 1;				// defect length for  MB
	int lSP = 1;				// defect length for  SP
	float prcSPS = 100.;		// % for space spur (not used)
	float absSPS = 100.;		// absolute value for space spur (not used)

	int dclcmax = 72;			// maximal difference for CLC value


	// --- LOW GRAY (LG)-------------------------------------
	int LG_SensLen = 50;			// Low Gray sensor length (not used)
	int LG_m_szw = 8;				// window size for Low Gray defect finding
	int LG_m_prsLG = 20;			// % setting for Low Gray defect finding

	int LG_m_szw_P = 50;
	int LG_m_prsLG_P = 20;
	int LG_m_Avg_Par = 10;

	// --- Debug
	int InspectRowMin = 0; // 검사 최소 행 값 // minimum row value for inspection
	int InspectRowMax = 0; // 검사 최대 행 값 // maximum row value for inspection
	int InspectColMin = 0; // 검사 최소 열 값 // minimum column value for inspection
	int InspectColMax = 0; // 검사 최대 열 값 // maximum column value for inspection

	// LG enhancement ---------------------------------
	int LGE_mid = 50;
	int LGE_margin = 20;
	int LGE_off = 1;
	int LGE_slope = 3.1;

	// --- MAXMIN Autothreshold -------------------------------------
	int mmat_shift = 20; // thr shift
	int mmat_fsz = 251;   // filter size
	int running_medians_span = 351;	// 중앙값 실행 범위 // Running Median Filter Size

	int alignment_type = 1;

	// --- CBMs --------------------------------------
	float fCBM_AbsMB_Meander = 0.9; 		// absolute MB value for Meander (in pixels)
	float fCBM_AbsSP_Meander = 0.9; 		// absolute SP value for Meander (in pixels)
	int   iCBM_Rmsize_Meander = 21;			// running median sizes for Meander (in pixels)
	float fCBM_AbsMB_Pad = 0.7; 			// absolute MB value for Pad (in pixels)
	float fCBM_AbsSP_Pad = 0.7; 			// absolute SP value for Pad (in pixels)
	int   iCBM_Rmsize_Pad = 15;				// running median sizes for Pad (in pixels)
	float fCBM_AbsMB_ConnPad = 0.7; 		// absolute MB value for Connnected Pad (in pixels)
	float fCBM_AbsSP_ConnPad = 0.7; 		// absolute SP value for Connnected Pad (in pixels)
	int   iCBM_Rmsize_ConnPad = 15;			// running median sizes for Connnected Pad (in pixels)
	float fCBM_AbsMB_WireAngle = 0.7; 		// absolute MB value for Wire Angle (in pixels)
	float fCBM_AbsSP_WireAngle = 0.7; 		// absolute SP value for Wire Angle (in pixels)
	int   iCBM_Rmsize_WireAngle = 15;		// running median sizes for Wire Angle (in pixels)

	void resetToDefault() {
		*this = Inspection_Params();  // 새로운 구조체 인스턴스를 디폴트 값으로 생성
	}


};

//extern struct Inspection_Params m_Inspection_Params;

// --------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------



class DLL_API CParams
{

public:
	CParams(CWnd* pParentWnd);
	int Show_Dialog_Params(CWnd * pParentWnd, CString model_path, Inspection_Params * in_params, int iVersion);

	bool Load_Params(CString szModelPath, int iVersion, Inspection_Params * params);
	bool Save_Params(CString szModelPath, Inspection_Params * params);

	bool Load_Params_Name(CString szModelPath, CString file_name, int iVersion, Inspection_Params * params);
	bool Save_Params_Name(CString szModelPath, CString file_name, Inspection_Params * params);

	void OpenShort_Set_Auto();

	// step : {1, 2, 3, 4, 5}
	void FWM_Set_Auto_Step(int step);

	void FWM_Set_Auto();






};

