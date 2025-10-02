/**
 * \copyright (C) Synapse Imaging 2024
 *
 * @File Name
 *      Inspect_Call.h.h
 *
 * @Summary
 *
 * @Description
 *
 *
 * @authors Igor Dunin-Barkowski, Iurii Kim
 * @version 1.0
 * @date 2/28/2024 2:52:50 PM
 */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
#pragma once
#include "Inspect_Common.h"
#include ".\Lib\FWMFuncs.h"
//#include "PinholeIsland.h"
//#include "OpenShort.h"
//#include "Alig2.h"
#include ".\Lib\Slice.h"
#include ".\Lib\Open_Short_2.h"

#define  WRTEST 0


namespace inspect_call
{
	/** ---------------------------------------------------------------------------
	 * @Summary
	 *      Structure of Thread Inspection parameters
	 */
	typedef struct ThreadParam
	{
		bool thread_run;
		unsigned int thread_num;
		unsigned int processor_num;
		HANDLE thread;


		TSQueueSM<CString> queue_log;

	} THREAD_PARAMS;

	/** ---------------------------------------------------------------------------
	* @Summary
	*      Structure of Thread Inspection parameters
	*/
	class CResultSaveData
	{
	public:
		std::mutex m_mutex;
		TSQueueSM<int> queue_iFrameNo;
		TSQueueSM<int> queue_iUnitNo;

		TSQueueSM<HObject>	queue_ho_Defect_Image;

		TSQueueSM<CString>	queue_sDefectCode;
		TSQueueSM<CString>	queue_sDefectName;

		TSQueueSM<double>	queue_dDefectPosX;
		TSQueueSM<double>	queue_dDefectPosY;
	};
	///** ---------------------------------------------------------------------------
	//	* @Summary
	//	*      Types of inspection
	//	*/
	//enum INSPECT_TYPE
	//{
	//	INSPECT_PANEL,
	//	INSPECT_ONE_IMAGE,

	//};

	///** ---------------------------------------------------------------------------
	//* @Summary
	//*      Structure of Inspection parameters
	//*/
	//typedef struct InspectParam_t
	//{
	//	int iFovNo;
	//	int iFovNoX;
	//	int iFovNoY;

	//	int iFrameNo;
	//	int iUnitNo;
	//	int unit_index;

	//	HObject image;

	//	INSPECT_TYPE inspect_type;
	//} InspectParam_t;

	//struct Inspection_Params
	//{
	//	/// --------------------------------------------
	//	/// TODO: OpenShort2: Add this to GUI
	//	/// --------------------------------------------
	//	int cv_Min_CAD_Area_Open = -1;						// Minimum allowed CAD region area for identifying fill numbers Open
	//	int cv_Min_CAD_Area_Short = -1;						// Minimum allowed CAD region area for identifying fill numbers Short
	//	int cv_Gray_Gap_Open = -1;							// Acceptable gray gap Open
	//	int cv_Gray_Gap_Short = -1;							// Acceptable gray gap Shore

	//	int check_diagonals = 1;							// Check diagonal pixels when filling
	//	/// --------------------------------------------

	//	/// TODO: Take from parameters
	//	/// --------------------------------------------

	//	bool union_defects = true;
	//	int union_defect_dist = 30;

	//	

	//	//  Thresholding
	//	bool inspect_Auto_Thr = false;  // Auto thresholding //specifies if use automatic threshold (or not)
	//	bool inspect_Preproc = false;  // Auto thresholding //specifies if use automatic threshold (or not)
	//	int threashold = 130;			// Main threshold

	//	// Inspection stages
	//	bool inspect_Pinhole = false;   // Inspect Pinholes// specifies if inspect pinholes (or not)
	//	bool inspect_Island = false;    // Inspect Island // specifies if inspect islands (or not)
	//	bool inspect_Open = false;		// Inspect Open // specifies if inspect opens (or not)
	//	bool inspect_Short = false;		// Inspect Short // specifies if inspect shorts (or not)
	//	bool inspect_Open2 = true;
	//	bool inspect_Short2 = true;		// Inspect Short2 // specifies if inspect shorts v2 (or not)

	//	bool inspect_LowGary = true;	// Inspect Low Gray // specifies if inspect low gray level defects  (or not)
	//	bool inspect_LowGary_P = true;	// Àú¸íµµ °áÇÔ(ÆÐÅÏ) °Ë»ç ¿©ºÎ // inspect low gray pattern

	//	bool inspect_FWMS = true; // FWMS °Ë»ç ¿©ºÎ // inspect FWMS
	//	bool inspect_FWMS_PT = true; // FWMS(PT) °Ë»ç ¿©ºÎ // inspect FWMS PT
	//	bool inspect_FWMS_MB = true; // FWMS(MB) °Ë»ç ¿©ºÎ // inspect FWMS MB
	//	bool inspect_FWM = true; // FWM °Ë»ç ¿©ºÎ // inspect FWM
	//	bool inspect_FWM_Spur = true; // FWM(½ºÆÛ) °Ë»ç ¿©ºÎ // inspect FWM spur
	//	bool inspect_FWM_MB = true; // FWM(MB) °Ë»ç ¿©ºÎ // inspect FWM MB


	//	// --- Pinhole_Island
	//	float PHrmin = 3.0;				// PH min.rad. // minimal pinhole internal radius
	//	int PHamin = 5;					//PH min.area // minimal pinhole area
	//	int Islrmin = 2;				// Isl.min.rad // minimal island internal radius
	//	int Islamin = 10;				// Isl.min.area // minimal island area

	//	HTuple hv_PHash = 200; // if pinhole area is bigger the value, than it's not removed from ho_RegionInoPhnoIsl (used in ThresholdPHIsp() )
	//	
	//	// --- OpenShort ------------------------------------
	//	HTuple hv_short_max_active = 0;  // Max def. on// specifies if maximal number of defects per unit is used(or not)
	//	int max_shorts_num = 100;		// Max def.num //maximal number of defects per unit 
	//	
	//	

	//	// Build_IRB -------------------------------------
	//	HTuple hv_expsize = 251;		// IRB expand // 19.5; //region expansion value for building IRB
	//	HTuple hv_dil_S = 7; // Space IRB »ý¼ºÀ» À§ÇÑ È®Àå °ª // IRB dil. dilation value for building IRB
	//	HTuple hv_dil_P = 11; // Pattern IRB »ý¼ºÀ» À§ÇÑ È®Àå °ª // IRB dil. dilation value for building IRB
	//	HTuple hv_stopperT = 7;

	//	HTuple hv_dilS = 9.5;			// dilation value for open/short prep.
	//	//HTuple hv_ClS = 2.5;			// closing value for shorts
	//	//HTuple hv_dil = 9;				// IRB dil.   //dilation value for building IRB

	//	// --- FWM ----------------------------------------
	//	int lMB = 1;			// MB length
	//	float prcMB = 40.; 		// MB % // % nearby for MB (Pattern)
	//	float absMB = 30.;		//  absolute MB value (Pattern)(in pixels)
	//	float minMB = 1.5;		//  minimal MB value (in pixels)
	//	float prcSP = 30.; 		// % nearby  for spur (Pattern)
	//	float absSP = 1.5;		//  absolute spur value (in pixels)
	//	float minwp = 10.;		//  minimal pattern width to use %
	//	float narrowwp = 7.;	//  pattern width to detect as narrow
	//	float widewp = 24.;	//  pattern width to detect as too wide
	//	float minSP = 10;		//  minimal spur value (in pixels)
	//	int sz1 = 50;			// Running median sizes for FWM 
	//	HTuple hv_dilFWM = 10;			// dilation value for FWM (used in SkeletonsCheckPrep() )
	//	
	//	// --- FWMs --------------------------------------
	//	int nvoid = 7; ///  not used points number in the begining and end of skeleton part
	//	int lMBS = 0;// 1;  // defect length for  space MB (narrow space)
	//	
	//	int sz2 = 50;			// Running median sizes for FWMS
	//	float ang = 3.;			// angle for defect 
	//	int rmskip = 0;			// skip value in run. med.
	//	int dlen = 3;			// (not used)
	//	float prcMBS = 30.; 		// % nearby for MB (Space)
	//	float absMBS = 10.;		//  absolute MB value (Space)(in pixels)
	//	
	//	float minMBS = 4.5;		//  minimal MB value (in pixels) for % Space
	//	float narWSpace = 0.4;  // narrow width for space (if less then it's a short defect)

	//	float minWSpace = 0.5;	// ÃÖ¼Ò °ø°£ ³Êºñ (Shortdyd) // minimal width for space

	//	//float prcMB = 40.;
	//	//float prcSP = 40.;		// Prc. for MB ans SP
	//	//float absMB = 10.;
	//	//
	//	//int lMB = 1;				// defect length for  MB
	//	int lSP = 1;				// defect length for  SP
	//	float prcSPS = 100.;		// % for space spur (not used)
	//	float absSPS = 100.;		// absolute value for space spur (not used)

	//	int dclcmax = 72;			// maximal difference for CLC value


	//	// --- LOW GRAY (LG)-------------------------------------
	//	int LG_SensLen = 50;			// Low Gray sensor length (not used)
	//	int LG_m_szw = 8;				// window size for Low Gray defect finding
	//	int LG_m_prsLG = 20;			// % setting for Low Gray defect finding

	//	int LG_m_szw_P = 50; 
	//	int LG_m_prsLG_P = 20; 
	//	int LG_m_Avg_Par = 10; 

	//	// --- Debug
	//	int InspectRowMin = 0; // °Ë»ç ÃÖ¼Ò Çà °ª // minimum row value for inspection
	//	int InspectRowMax = 0; // °Ë»ç ÃÖ´ë Çà °ª // maximum row value for inspection
	//	int InspectColMin = 0; // °Ë»ç ÃÖ¼Ò ¿­ °ª // minimum column value for inspection
	//	int InspectColMax = 0; // °Ë»ç ÃÖ´ë ¿­ °ª // maximum column value for inspection

	//	// LG enhancement ---------------------------------
	//	int LGE_mid=50;
	//	int LGE_margin=20;
	//	int LGE_off = 1;
	//	int LGE_slope = 3.1;

	//	// --- MAXMIN Autothreshold -------------------------------------
	//	int mmat_shift = 20; // thr shift
	//	int mmat_fsz = 251;   // filter size
	//	int running_medians_span = 351;	// Áß¾Ó°ª ½ÇÇà ¹üÀ§ // Running Median Filter Size


	//	int alignment_type = 1;

	//	void resetToDefault() {
	//		*this = Inspection_Params();  // »õ·Î¿î ±¸Á¶Ã¼ ÀÎ½ºÅÏ½º¸¦ µðÆúÆ® °ªÀ¸·Î »ý¼º
	//	}

	//	
	//};
	extern struct Inspection_Params m_Inspection_Params;


	/** ---------------------------------------------------------------------------
	* @Summary
	*      Thread Inspection Class
	*/
	class CInspect_Call
	{

	public:
		CInspect_Call(int thread_num, TSQueueSM<CString>* queue_log);
		~CInspect_Call();

	public:
		int m_thread_num;
		InspectParam_t inspect_param;
		TSQueueSM<CString>* m_queue_log;
		int m_total_defects;

		HTuple hv_Wmin, hv_Wmax, hv_Wmean, hv_Wmed; // pattern width stat. data
		

	private:
		HObject m_ho_RegionISnoPhnoIsl;
		HObject m_ho_RegionInoPhnoIsl;
		

	public:
		CFWM* m_pFWM;
		CSlice* m_slice;
		Open_Short_2 *op2;



		//HObject m_pIm;
		UINT8 *pIm/*, *Empty*/;
		HObject m_local_Im;
		HObject m_local_ImModified;
		HTuple m_hv_Width, m_hv_Height;
		HObject m_ho_RegionI;
		HObject m_ho_RegionISnoIslnoPh, m_ho_RegionInoIslnoPh;

		HObject m_ho_RegionOpCon;
		HObject m_ho_RegionsShi;

		HObject m_ho_RegionsLG;
		HObject m_ho_RegionsLG_P;
		HTuple m_hv_PrcM_Min, m_hv_PrcM_Max, m_hv_PrcM_Median;
		HTuple m_hv_PrcM_Min_P, m_hv_PrcM_Max_P, m_hv_PrcM_Median_P;
		//HObject m_ho_ImageIRB, m_ho_ImageChainLength, m_ho_ImageCLC;

		UINT16 *pImIRBP, *pImIRBS, *pImCLCS;

		int m_iw, m_ih;
	public:
		HObject m_ho_RegionsOpen2;
		HObject m_ho_RegionsShort2;
		HObject m_ho_Inspect_ID;

	public:
		CString m_inspect_status;
		int threshold;
		int threshold_auto;

		int narrowwp, widewp;

		HTuple m_hv_Shift_Row, m_hv_Shift_Col;
		HTuple hv_ReturnHomMat2D;

	public:
		HObject m_ho_RegionPH;
		HObject m_ho_RegionsIsl;
		HObject m_ho_RegionsOp;

		HObject m_ho_RegionsMBS;
		HObject m_ho_RegionsSPS;
		HTuple m_hv_pDefMBSprc;
		HTuple m_hv_pDefSPSprc;
		HTuple m_hv_pDefMBS;
		HTuple m_hv_pDefSPS;

		HObject m_ho_RegionsMB;
		HObject m_ho_RegionsSP;
		HTuple m_hv_pDefMBprc;
		HTuple m_hv_pDefSPprc;
		HTuple m_hv_pDefMB;
		HTuple m_hv_pDefSP;

		

	public:
		void Init();
		bool Alignment();
		void GM_Auto_Threshold(int &Threshold_Open, int &Threshold_Short);
		void OpenShort2_Split_Defect(HObject ho_Defects, HObject & ho_Defects_Split, HTuple hv_Distanse_Defect);
		int OpenShort2();
		int Proc_FWMS();
		int Proc_FWM();
		int OpenShort();
		int Low_Gray(int threshold);
		int Pinhole_Island();
		int Build_IRB();
		void Inspect(InspectParam_t* inspect_paramI);

		int Proc_MinG(int iRunningMedianSpan);
		void LGEnhance2(HObject ho_Image, HObject *ho_ImageResult, HTuple hv_mid, HTuple hv_range,
			HTuple hv_lutexp, HTuple hv_off);
		void FixThinSpace(HObject ho_RegionInoD, HObject ho_DomainG, HObject *ho_RegionInoDr);
		int SetWpNarrowWide(HObject ho_SkeletonFWMi, HObject ho_WidthImageP, HTuple hv_kw, HTuple hv_dw,// Setting of narrowwp and widewp
			float *narrowwp, float *widewp);
	};


	/* ************************************************************************** */
	/* Section: File Scope or Global Data                                         */
	/* ************************************************************************** */
	extern 	TSQueueSM<CString> m_main_queue_log;

	extern 	CSlice_Teach m_Slice_Teach;


	extern struct Inspection_Params m_Inspection_Params;

	/// for FWM ***
	extern int Np;		// number of MSs
	extern int Na;		// number of addresses (for all SkeletonsFWM regions)
	extern int NaC;		// number contours
	extern int NaR;		// number of regions
	// cpu memory pointers for FWM /
	extern float *pP1x, *pP1y, *pP2x, *pP2y, *pP3x, *pP3y, *pWi;
	extern int *pPnum, *pPnum2, *pWG;
	extern int *pPaddrR, *pPaddrC;
	extern int *pPuseint;

	/// for FWMS ***
	extern int NpS;		// number of MSs
	extern int NaS;		// number of addresses (for all SkeletonsFWM regions)
	extern int NaCS;		// number contours
	extern int NaRS;		// number of regions
	extern float absNarS; // narrow space

	// cpu memory pointers for FWM /
	extern float *pP1Sx, *pP1Sy, *pP2Sx, *pP2Sy, *pP3Sx, *pP3Sy, *pWiS;
	extern int *pPnumS, *pPnum2S, *pWGS;
	extern int *pPaddrRS, *pPaddrCS;

	extern int *pPuseintS;
	extern CString m_Path_Model;
	extern HObject m_ho_Gi;

	extern HObject m_ho_RegionGS, m_ho_RectangleCut, m_ho_RegionG, m_ho_RegionsEMPTY, m_ho_RegionsGND, m_ho_RegionsNFWM, m_ho_RegionFWM, m_ho_RegionsPNi, m_ho_RegionsPNPi,
		m_ho_RegionsPNSi, m_ho_RegNoProc, m_ho_SkeletonFWMi, m_ho_SkeletonG, m_ho_SkeletonGS, m_ho_SkeletonPartsP, m_ho_SkeletonsFWM, m_ho_SkeletonsFWMvalid, m_ho_SkeletonsFWMS, m_ho_SkeletonsFWMSvalid, m_ho_SkeletonsG;
	extern HObject m_ho_SkeletonsFWMScut, m_ho_SkelSEP, m_ho_RegionCut, m_ho_RegionI;
	extern HObject m_ho_SkeletonsFWMcut;

	extern HObject m_ho_DirImageP, m_ho_ImageIR, m_ho_ImageIR16, m_ho_ImageIRS, m_ho_ImDisplacement, m_ho_WidthImageP, m_ho_WidthImageS;
	extern HObject m_ho_ImCLC;

	extern UINT16 *pImIRBP, *pImIRBS, *pImCLC, *pImCLCS;// image pointers

	extern HObject m_ho_DirImageS;
	extern HObject m_ho_GiDomain;
	extern HObject m_ho_RegProc;

	extern HTuple m_ho_FWMpar;

	extern int m_num_teach;
	/* ************************************************************************** */
	/* Section: Function Prototypes                                               */
	/* ************************************************************************** */
	void Auto_Threshold(HObject ho_RegProc, HObject ho_Im, HTuple *hv_thr);
	void Auto_Threshold2(HObject ho_SkeletonsFWMS, HObject ho_Im, HTuple *hv_thr);
	void Auto_Threshold3(HObject ho_RegionFWM, HObject ho_Im, 
		HTuple *hv_thr);

	bool GetParamBool(json key, bool default = 0);
	int GetParamInt(json key, int default = 0);
	double GetParamDouble(json key, double default = 0);
	string GetParamString(json key, string default = "");
	bool Read_HObject(HObject* Object, const HTuple& FileName);
	bool Read_HImage(HObject* Image, const HTuple& FileName);
	void GenSkeletonFWMvalid(HObject ho_SkeletonsFWM, HObject *ho_SkeletonFWMvalid,
		HTuple hv_cutfrombeg, HTuple hv_cutfromend);

	bool Load_Teach(CString path_model);
	bool Load_Model(CString path_model);
	bool Convert_To_Bin(CString path_teach, bool replace);
	bool Load_Params(CString szModelPath, int iVersion, struct Inspection_Params *params, TSQueueSM<CString>* queue_log);
	bool Save_Params(CString szModelPath, struct Inspection_Params *params, TSQueueSM<CString>* queue_log);
	void Union_Defects(HObject ho_ROI1, HObject ho_ROI2, HObject *ho_ROI1_Ret, HObject *ho_ROI2_Ret,
		HTuple hv_Defect_Dist);


	void GetFWMStandardWidth(HObject ho_SkeletonFWMi, HObject ho_WidthImageP, HTuple *hv_stdWFWM);

	void VerifySPregnoproc(HObject ho_RegNoProc, HObject ho_RegionsInoD, HObject ho_RegionsSP,
		HObject *ho_RegionsSPv, HTuple hv_mA);

	


}
/* ************************************************************************** */
/* End of file                                                                */
/* ************************************************************************** */

