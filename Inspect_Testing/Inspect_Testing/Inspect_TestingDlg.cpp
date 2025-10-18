
// Inspect_TestingDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "Inspect_Testing.h"
#include "Inspect_TestingDlg.h"
#include "afxdialogex.h"

#include <filesystem>

#include <Windows.h>
#include <iostream>
#pragma comment(lib, "Winmm.lib")
#include <mmsystem.h>

#include "HalconCpp.h"

#include "Our_Algorithms.h"
#include "Halcon_Exchange.h"
#include "Open_Short_2.h"
#include "CParams_Dialog.h"

//#include "CDialog_Params.h"
//#include "Alg_Params.h"
#include "CParams.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Выключим перехват клавиатуры
#define _DEBUG_KEY

#define INSPECT_TMP_FOLDER					"C:\\TmpIm\\Inspect_Testing"
#define WM_BATH_INSPECT_EVENTS				WM_USER + 100
#define WM_BATH_INSPECT_EVENTS_DEFECT		WM_USER + 101	
#define WM_BATH_INSPECT_EVENTS_ADD_DEFECT	WM_USER + 102	
#define WM_IMAGE_FIT						WM_USER + 103	

using namespace inspect_utils;
using namespace inspect_call;

namespace fs = std::filesystem;


CString m_Image_Path;
CResultSaveData* result_data;
THREAD_PARAMS* thread_params;		// Thread parameters
CInspect_Call* m_inspect = nullptr;
int m_count_processors = 1;
HWND m_HWND;
static int m_init = 0;


HalconCpp::HImage m_Disp_Image;


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
//protected:
//	DECLARE_MESSAGE_MAP()
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnNMReturnListUnitDefects(NMHDR *pNMHDR, LRESULT *pResult);
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}



// CInspectTestingDlg dialog



CInspectTestingDlg::CInspectTestingDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_INSPECT_TESTING_DIALOG, pParent)
	, m_Folder_Teach(_T(""))
	, m_Image_Name(_T(""))
	, m_Raw_Image(FALSE)
	, m_Auto_Thr(TRUE)
	, m_Init_Auto(FALSE)
	, m_Check_If_Defect(FALSE)
	, m_Check_Inspect_LG(TRUE)
	, m_Check_Inspect_FWM(TRUE)
	, m_Check_Inspect_FWMS(TRUE)
	, m_Ed_Bath_Unit(_T(""))
	, m_Edit_Inspection_Pos(_T(""))
	, m_Bath_Count(_T(""))
	, m_Auto_Threshold(TRUE)
	, m_Ed_Defect_Count(_T(""))
	, m_Check_Dip_Defects(TRUE)
	, m_Check_Union_Defects(FALSE)
	, m_Check_Open_Short_2(TRUE)
	, m_Lg_Prs_P(_T(""))
	, m_Lg_Prs_S(_T(""))
	, m_Lg_Szw_P(_T(""))
	, m_Lg_Szw_S(_T(""))
	, m_Lg_Avg_P(_T(""))
	, m_Check_Alig2(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CInspectTestingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_FOLDER_TEACH, m_Folder_Teach);
	DDX_Text(pDX, IDC_EDIT_IMAGE, m_Image_Name);
	DDX_Check(pDX, IDC_CHECK_RAW_IMAGE, m_Raw_Image);
	DDX_Check(pDX, IDC_CHECK_INIT_AUTO, m_Init_Auto);
	DDX_Check(pDX, IDC_CHECK_AUTOTHR, m_Auto_Thr);
	DDX_Check(pDX, IDC_CHECK_DEFECT_FOUND_STOP, m_Check_If_Defect);
	DDX_Check(pDX, IDC_CHECK_INSPECT_LG, m_Check_Inspect_LG);
	DDX_Check(pDX, IDC_CHECK_INSPECT_FWM, m_Check_Inspect_FWM);
	DDX_Check(pDX, IDC_CHECK_INSPECT_FWMS, m_Check_Inspect_FWMS);
	DDX_Control(pDX, IDC_PROGRESS, m_Inspect_Progress);
	DDX_Text(pDX, IDC_EDIT_INSPECTION_UNIT, m_Ed_Bath_Unit);
	DDX_Text(pDX, IDC_EDIT_INSPECTION_POS, m_Edit_Inspection_Pos);
	DDX_Text(pDX, IDC_EDIT_INSPECTION_COUNT, m_Bath_Count);
	DDX_Control(pDX, IDC_LIST_DEFECTS, m_Bath_Defects_List);
	DDX_Check(pDX, IDC_CHECK_AUTO_THR_BATH, m_Auto_Threshold);
	DDX_Text(pDX, IDC_EDIT_DEFECTS_COUNT, m_Ed_Defect_Count);
	DDX_Control(pDX, IDC_IMAGE, m_Image_Control);
	DDX_Check(pDX, IDC_CHECK_DISP_DEFECTS, m_Check_Dip_Defects);
	DDX_Check(pDX, IDC_CHECK_UNION_DEFECTS, m_Check_Union_Defects);
	DDX_Check(pDX, IDC_CHECK_OP2, m_Check_Open_Short_2);
	DDX_Text(pDX, IDC_EDIT_LG_PRC_P, m_Lg_Prs_P);
	DDX_Text(pDX, IDC_EDIT_LG_PRC_S, m_Lg_Prs_S);
	DDX_Text(pDX, IDC_EDIT_LG_SZW_P, m_Lg_Szw_P);
	DDX_Text(pDX, IDC_EDIT_LG_SZW_S, m_Lg_Szw_S);
	DDX_Text(pDX, IDC_EDIT_LG_AVG_P, m_Lg_Avg_P);
	DDX_Control(pDX, IDC_LIST_UNIT_DEFECTS, m_List_Unit_Defects);
	DDX_Control(pDX, IDC_LIST_DEFECT_ATTRIBUTE, m_Defect_Attribute);
	DDX_Check(pDX, IDC_CHECK_ALIG2, m_Check_Alig2);
}

BEGIN_MESSAGE_MAP(CInspectTestingDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_MESSAGE(WM_BATH_INSPECT_EVENTS, OnBathInspectEvent)
	ON_MESSAGE(WM_BATH_INSPECT_EVENTS_DEFECT, OnBathInspectEventDefect)
	ON_MESSAGE(WM_BATH_INSPECT_EVENTS_ADD_DEFECT, OnBathInspectEventADDDefect)
	ON_MESSAGE(WM_IMAGE_FIT, OnImageFit)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_SET_TEACH_FOLDER, &CInspectTestingDlg::OnBnClickedButtonSetTeachFolder)
	ON_BN_CLICKED(IDC_BUTTON_SET_DATA_FOLDER, &CInspectTestingDlg::OnBnClickedButtonSetDataFolder)
	ON_BN_CLICKED(IDC_BUTTON_CONVERT_TO_BIN, &CInspectTestingDlg::OnBnClickedButton_ConvertToBin)
	ON_BN_CLICKED(IDC_BUTTON_SET_IMAGE, &CInspectTestingDlg::OnBnClickedButtonSetImage)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_TEACH, &CInspectTestingDlg::OnBnClickedButtonLoadTeach)
	ON_BN_CLICKED(IDC_BUTTON_ALIGNMENT, &CInspectTestingDlg::OnBnClickedButtonAlignment)
	ON_BN_CLICKED(IDC_BUTTON_PINHOLE_ISLAND, &CInspectTestingDlg::OnBnClickedButtonPinholeIsland)
	ON_BN_CLICKED(IDC_CHECK_RAW_IMAGE, &CInspectTestingDlg::OnBnClickedCheckRawImage)
	ON_BN_CLICKED(IDC_BUTTON_BUILD_FWM, &CInspectTestingDlg::OnBnClickedButtonBuildFwm)
	ON_BN_CLICKED(IDC_BUTTON_INSPECTION_INIT, &CInspectTestingDlg::OnBnClickedButtonInspectionInit)
	ON_BN_CLICKED(IDC_BUTTON_READ_IRB, &CInspectTestingDlg::OnBnClickedButtonReadIrb)
	ON_BN_CLICKED(IDC_BUTTON_BUILD_FWMS, &CInspectTestingDlg::OnBnClickedButtonBuildFwms)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_SHORT, &CInspectTestingDlg::OnBnClickedButtonOpenShort)
	ON_BN_CLICKED(IDC_BUTTON_BUILD_IRB, &CInspectTestingDlg::OnBnClickedButtonBuildIrb)
	ON_EN_CHANGE(IDC_EDIT_IMAGE, &CInspectTestingDlg::OnEnChangeEditImage)
	ON_EN_CHANGE(IDC_EDITPRCMBS, &CInspectTestingDlg::OnEnChangeEditprcmbs)
	ON_BN_CLICKED(IDC_CHECK_INIT_AUTO, &CInspectTestingDlg::OnBnClickedCheckInitAuto)
	ON_BN_CLICKED(IDC_VIEWIRBP, &CInspectTestingDlg::OnBnClickedViewirbp)
	ON_BN_CLICKED(IDC_VIEWIRBS, &CInspectTestingDlg::OnBnClickedViewirbs)
	ON_BN_CLICKED(IDC_VIEWIM, &CInspectTestingDlg::OnBnClickedViewim)
	ON_BN_CLICKED(IDC_BUTTON_INSPECTION_ALL, &CInspectTestingDlg::OnBnClickedButtonInspectionAll)
	ON_EN_CHANGE(IDC_EDITPRCSPS, &CInspectTestingDlg::OnEnChangeEditprcsps)
	ON_BN_CLICKED(IDC_BUTTON_INSPECTION_ALL, &CInspectTestingDlg::OnBnClickedButtonInspectionAll)
	ON_EN_CHANGE(IDC_EDITTHR, &CInspectTestingDlg::OnEnChangeEditthr)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_PARAMS, &CInspectTestingDlg::OnBnClickedButtonLoadParams)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_PARAMS, &CInspectTestingDlg::OnBnClickedButtonSaveParams)
	ON_BN_CLICKED(IDC_BUTTON_LOW_GRAY, &CInspectTestingDlg::OnBnClickedButtonLowGray)
	ON_BN_CLICKED(IDC_BUTTON_AUTO_THR, &CInspectTestingDlg::OnBnClickedButtonAutoThr)
	ON_EN_CHANGE(IDC_EDITPRCMBP, &CInspectTestingDlg::OnEnChangeEditprcmbp)
	ON_EN_CHANGE(IDC_EDITPRCSPP, &CInspectTestingDlg::OnEnChangeEditprcspp)
	ON_BN_CLICKED(IDC_BUTTON_INSPECTION_ALL2, &CInspectTestingDlg::OnBnClickedButtonInspectionAll2)
	ON_BN_CLICKED(IDC_VIEWINOD, &CInspectTestingDlg::OnBnClickedViewinod)
	ON_BN_CLICKED(IDC_BUTTON_READ_IMAGE, &CInspectTestingDlg::OnBnClickedButtonReadImage)
	ON_BN_CLICKED(IDC_CHECK_AUTOTHR, &CInspectTestingDlg::OnBnClickedCheckAutothr)
	ON_EN_CHANGE(IDC_EDITMABS, &CInspectTestingDlg::OnEnChangeEditmabs)
	ON_EN_CHANGE(IDC_EDITMINABS, &CInspectTestingDlg::OnEnChangeEditminabs)
	ON_BN_CLICKED(IDC_BUTTON_EDIT_PARAMS, &CInspectTestingDlg::OnBnClickedButtonEditParams)
	ON_BN_CLICKED(ID_WIZFINISH, &CInspectTestingDlg::OnBnClickedWizfinish)
	ON_BN_CLICKED(ID_WIZFINISH2, &CInspectTestingDlg::OnBnClickedWizfinish2)
	ON_BN_CLICKED(ID_WIZFINISH3, &CInspectTestingDlg::OnBnClickedWizfinish3)
	ON_BN_CLICKED(ID_WIZFINISH4, &CInspectTestingDlg::OnBnClickedWizfinish4)
	ON_BN_CLICKED(IDC_BUTTON1, &CInspectTestingDlg::OnBnClickedButton1)
	ON_EN_CHANGE(IDC_EDITMINABS2, &CInspectTestingDlg::OnEnChangeEditminabs2)
	ON_BN_CLICKED(IDC_VIEWSKELG, &CInspectTestingDlg::OnBnClickedViewskelg)
	ON_BN_CLICKED(IDC_BUTTON_BATH_INIT, &CInspectTestingDlg::OnBnClickedButtonBathInit)
	ON_BN_CLICKED(IDC_BUTTON_BATH_NEXT, &CInspectTestingDlg::OnBnClickedButtonBathNext)
	ON_BN_CLICKED(IDC_BUTTON_BATH_START, &CInspectTestingDlg::OnBnClickedButtonBathStart)
	ON_BN_CLICKED(IDC_BUTTON_BATH_STOP, &CInspectTestingDlg::OnBnClickedButtonBathStop)
	ON_BN_CLICKED(IDC_BUTTON_BATH_PREVIOUS, &CInspectTestingDlg::OnBnClickedButtonBathPrevious)
	ON_BN_CLICKED(IDC_BUTTON_BATH_CLEAR, &CInspectTestingDlg::OnBnClickedButtonBathClear)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DEFECTS, &CInspectTestingDlg::OnLvnItemchangedListDefects)
	ON_BN_CLICKED(IDC_CHECK_INSPECT_LG, &CInspectTestingDlg::OnBnClickedCheckInspectLg)
	ON_BN_CLICKED(IDC_CHECK_INSPECT_FWM, &CInspectTestingDlg::OnBnClickedCheckInspectFwm)
	ON_BN_CLICKED(IDC_CHECK_INSPECT_FWMS, &CInspectTestingDlg::OnBnClickedCheckInspectFwms)
	ON_BN_CLICKED(IDC_BUTTON_MING, &CInspectTestingDlg::OnBnClickedButtonMing)
	ON_BN_CLICKED(IDC_CHECK_AUTO_THR_BATH, &CInspectTestingDlg::OnBnClickedCheckAutoThrBath)
	ON_STN_DBLCLK(IDC_IMAGE, &CInspectTestingDlg::OnStnDblclickImage)
	ON_WM_NCRBUTTONUP(IDC_IMAGE, &CInspectTestingDlg::OnRButtonDownImage)
	ON_WM_SETCURSOR(IDC_IMAGE, &CInspectTestingDlg::OnSetCursor)

	ON_EN_CHANGE(IDC_EDITMMATS, &CInspectTestingDlg::OnEnChangeEditmmats)
	ON_EN_CHANGE(IDC_EDITMID, &CInspectTestingDlg::OnEnChangeEditmid)
	ON_EN_CHANGE(IDC_EDITMARGIN, &CInspectTestingDlg::OnEnChangeEditmargin)
	ON_BN_CLICKED(IDC_CHECK_MMATOFF, &CInspectTestingDlg::OnBnClickedCheckMmatoff)
	ON_WM_MOUSEWHEEL(IDC_IMAGE, &CInspectTestingDlg::OnMouseWheel)


	ON_BN_CLICKED(IDC_BUTTON_SAVE_DEFECTS, &CInspectTestingDlg::OnBnClickedButtonSaveDefects)
	ON_BN_CLICKED(IDC_BUTTON5, &CInspectTestingDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON_MAXMIN, &CInspectTestingDlg::OnBnClickedButtonMaxmin)
	ON_BN_CLICKED(IDC_CHECK_DISP_DEFECTS, &CInspectTestingDlg::OnBnClickedCheckDispDefects)
	ON_BN_CLICKED(IDC_CHECK_UNION_DEFECTS, &CInspectTestingDlg::OnBnClickedCheckUnionDefects)

	ON_EN_CHANGE(IDC_EDIT_ABSMB_MEANDER, &CInspectTestingDlg::OnEnChangeEditAbsmbMeander)
	ON_EN_CHANGE(IDC_EDIT_ABSSP_MEANDER, &CInspectTestingDlg::OnEnChangeEditAbsspMeander)
	ON_EN_CHANGE(IDC_EDIT_RMSIZE_MEANDER, &CInspectTestingDlg::OnEnChangeEditRmsizeMeander)

	ON_EN_CHANGE(IDC_EDIT_ABSMB_PAD, &CInspectTestingDlg::OnEnChangeEditAbsmbPad)
	ON_EN_CHANGE(IDC_EDIT_ABSSP_PAD, &CInspectTestingDlg::OnEnChangeEditAbsspPad)
	ON_EN_CHANGE(IDC_EDIT_RMSIZE_PAD, &CInspectTestingDlg::OnEnChangeEditRmsizePad)

	ON_EN_CHANGE(IDC_EDIT_ABSMB_CONN_PAD, &CInspectTestingDlg::OnEnChangeEditAbsmbConnPad)
	ON_EN_CHANGE(IDC_EDIT_ABSSP_CONN_PAD, &CInspectTestingDlg::OnEnChangeEditAbsspConnPad)
	ON_EN_CHANGE(IDC_EDIT_RMSIZE_CONN_PAD, &CInspectTestingDlg::OnEnChangeEditRmsizeConnPad)

	ON_EN_CHANGE(IDC_EDIT_ABSMB_WIRE_ANGLE, &CInspectTestingDlg::OnEnChangeEditAbsmbWireAngle)
	ON_EN_CHANGE(IDC_EDIT_ABSSP_WIRE_ANGLE, &CInspectTestingDlg::OnEnChangeEditAbsspWireAngle)
	ON_EN_CHANGE(IDC_EDIT_RMSIZE_WIRE_ANGLE, &CInspectTestingDlg::OnEnChangeEditRmsizeWireAngle)

	ON_EN_CHANGE(IDC_EDIT_DT_DARK_SPACE, &CInspectTestingDlg::OnEnChangeEditDtDarkSpace)
	ON_EN_CHANGE(IDC_EDIT_DT_BRIGHT_SPACE, &CInspectTestingDlg::OnEnChangeEditDtBrightSpace)
	ON_EN_CHANGE(IDC_EDIT_DT_MEAN_SPACE, &CInspectTestingDlg::OnEnChangeEditDtMeanSpace)
	ON_EN_CHANGE(IDC_EDIT_DT_MARGIN_SPACE, &CInspectTestingDlg::OnEnChangeEditDtMarginSpace)
	ON_EN_CHANGE(IDC_EDIT_DT_AREA_SPACE, &CInspectTestingDlg::OnEnChangeEditDtAreaSpace)

	ON_EN_CHANGE(IDC_EDIT_DT_DARK_PATTERN, &CInspectTestingDlg::OnEnChangeEditDtDarkPattern)
	ON_EN_CHANGE(IDC_EDIT_DT_BRIGHT_PATTERN, &CInspectTestingDlg::OnEnChangeEditDtBrightPattern)
	ON_EN_CHANGE(IDC_EDIT_DT_MARGIN_PATTERN, &CInspectTestingDlg::OnEnChangeEditDtMarginPattern)
	ON_EN_CHANGE(IDC_EDIT_DT_AREA_PATTERN, &CInspectTestingDlg::OnEnChangeEditDtAreaPattern)
	ON_EN_CHANGE(IDC_EDIT_DT_LENGTH_PATTERN, &CInspectTestingDlg::OnEnChangeEditDtLengthPattern)
	ON_EN_CHANGE(IDC_EDIT_DT_WIDTH_PATTERN, &CInspectTestingDlg::OnEnChangeEditDtWidthPattern)
	ON_EN_CHANGE(IDC_EDIT_DT_CONNECTION_DISTANCE, &CInspectTestingDlg::OnEnChangeEditDtConnectionDistance)

	ON_WM_CTLCOLOR()
	//ON_EN_CHANGE(IDC_EDITLGPRC, &CInspectTestingDlg::OnEnChangeEditlgprc)
	ON_BN_CLICKED(IDC_BUTTON_READ, &CInspectTestingDlg::OnBnClickedButtonRead)
	ON_EN_CHANGE(IDC_EDIT_INSPECTION_COUNT, &CInspectTestingDlg::OnEnChangeEditInspectionCount)
	ON_EN_CHANGE(IDC_EDIT_INSPECTION_POS, &CInspectTestingDlg::OnEnChangeEditInspectionPos)
	ON_EN_CHANGE(IDC_EDIT_INSPECTION_FROM, &CInspectTestingDlg::OnEnChangeEditInspectionFrom)
	ON_EN_CHANGE(IDC_EDIT_INSPECTION_TO, &CInspectTestingDlg::OnEnChangeEditInspectionTo)
	ON_BN_CLICKED(IDC_BUTTON7, &CInspectTestingDlg::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_VIEWSKELG2, &CInspectTestingDlg::OnBnClickedViewskelg2)
	ON_BN_CLICKED(IDC_BUT_TEACH_OPENSHORT2, &CInspectTestingDlg::OnBnClickedButTeachOpenshort2)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_SHORT2, &CInspectTestingDlg::OnBnClickedButtonOpenShort2)
	ON_BN_CLICKED(IDC_BUTTON8, &CInspectTestingDlg::OnBnClickedButton8)
	ON_EN_CHANGE(IDC_EDITSZP, &CInspectTestingDlg::OnEnChangeEditszp)
	ON_EN_CHANGE(IDC_EDITSZS, &CInspectTestingDlg::OnEnChangeEditszs)
	ON_EN_CHANGE(IDC_EDITMINWP, &CInspectTestingDlg::OnEnChangeEditminwp)
	ON_EN_CHANGE(IDC_EDIT_LG_PRC_S, &CInspectTestingDlg::OnEnChangeEditLgPrcS)
	ON_EN_CHANGE(IDC_EDIT_LG_PRC_P, &CInspectTestingDlg::OnEnChangeEditLgPrcP)
	ON_EN_CHANGE(IDC_EDIT_LG_SZW_P, &CInspectTestingDlg::OnEnChangeEditLgSzwP)
	ON_EN_CHANGE(IDC_EDIT_LG_SZW_S, &CInspectTestingDlg::OnEnChangeEditLgSzwS)
	ON_EN_CHANGE(IDC_EDIT_LG_AVG_P, &CInspectTestingDlg::OnEnChangeEditLgAvgP)
	ON_BN_CLICKED(IDC_CHECK_AUTOTHR3, &CInspectTestingDlg::OnBnClickedCheckAutothr3)
	ON_BN_CLICKED(IDC_CHECK_PREPROC, &CInspectTestingDlg::OnBnClickedCheckPreproc)
	ON_EN_CHANGE(IDC_EDITPRK, &CInspectTestingDlg::OnEnChangeEditprk)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_UNIT_DEFECTS, &CInspectTestingDlg::OnLvnItemchangedListUnitDefects)
	ON_EN_CHANGE(IDC_EDITNARWP, &CInspectTestingDlg::OnEnChangeEditnarwp)
	ON_BN_CLICKED(IDC_VIEWREGIONG, &CInspectTestingDlg::OnBnClickedViewregiong)
	ON_BN_CLICKED(IDC_BUTTON_PARAMS_DLG, &CInspectTestingDlg::OnBnClickedButtonParamsDlg)
	ON_BN_CLICKED(IDC_VIEWREGIONCUT, &CInspectTestingDlg::OnBnClickedViewregioncut)
	ON_EN_CHANGE(IDC_EDITWIDEWP, &CInspectTestingDlg::OnEnChangeEditwidewp)
		ON_MESSAGE(WM_UPDATE_PARAMS_EVENTS, &CInspectTestingDlg::OnUpdateParamsEvents)
		ON_BN_CLICKED(IDC_CHECK_ALIG2, &CInspectTestingDlg::OnBnClickedCheckAlig2)
		ON_MESSAGE(WM_PARAM_APPLY_EVENTS, &CInspectTestingDlg::OnParamApplyEvents)
		ON_BN_CLICKED(IDC_VIEWREGIONI, &CInspectTestingDlg::OnBnClickedViewregioni)
		ON_BN_CLICKED(IDC_VIEWIMMOD, &CInspectTestingDlg::OnBnClickedViewimmod)
		ON_BN_CLICKED(IDC_CHECK_OP2, &CInspectTestingDlg::OnBnClickedCheckOp2)
		ON_BN_CLICKED(IDC_WPSET, &CInspectTestingDlg::OnBnClickedWpset)
		ON_BN_CLICKED(IDC_WPSETI, &CInspectTestingDlg::OnBnClickedWpseti)
		ON_BN_CLICKED(IDC_BUTTON_PROC_CBM_MEANDER, &CInspectTestingDlg::OnBnClickedButtonProcCbmMeander)
		ON_BN_CLICKED(IDC_BUTTON_PROC_CBM_PAD, &CInspectTestingDlg::OnBnClickedButtonProcCbmPad)
		ON_BN_CLICKED(IDC_BUTTON_PROC_CBM_CONN_PAD, &CInspectTestingDlg::OnBnClickedButtonProcCbmConnPad)
		ON_BN_CLICKED(IDC_VIEW_CBM_MEANDER, &CInspectTestingDlg::OnBnClickedViewCbmMeander)
		ON_BN_CLICKED(IDC_VIEW_CBM_PAD, &CInspectTestingDlg::OnBnClickedViewCbmPad)
		ON_BN_CLICKED(IDC_VIEW_CBM_CONN_PAD, &CInspectTestingDlg::OnBnClickedViewCbmConnPad)
		ON_BN_CLICKED(IDC_BUTTON_PROC_CBM_WIRE_ANGLE, &CInspectTestingDlg::OnBnClickedButtonProcCbmWireAngle)
		ON_BN_CLICKED(IDC_VIEW_CBM_WIRE_ANGLE, &CInspectTestingDlg::OnBnClickedViewCbmWireAngle)
		ON_BN_CLICKED(IDC_BUTTON_PROC_DT_SPACE, &CInspectTestingDlg::OnBnClickedButtonProcDtSpace)
		ON_BN_CLICKED(IDC_BUTTON_PROC_DT_PATTERN, &CInspectTestingDlg::OnBnClickedButtonProcDtPattern)
		ON_BN_CLICKED(IDC_CHECK_DT_CONNECTION, &CInspectTestingDlg::OnBnClickedCheckDtConnection)

		END_MESSAGE_MAP()

// CInspectTestingDlg message handlers

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode == HC_ACTION) {
		HWND hwndActiveWin = GetForegroundWindow();

		PKBDLLHOOKSTRUCT hookStruct = (PKBDLLHOOKSTRUCT)lParam;
		switch (wParam)
		{
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			switch (hookStruct->vkCode)
			{
			case VK_CONTROL:
				break;
			case VK_ESCAPE:
					SendMessage(m_HWND, WM_IMAGE_FIT, 0, 0);

					return 1;
				break;
			case VK_MENU:
				break;
			case 'N':
				break;
			}
			break;

		case WM_KEYUP:
		case WM_SYSKEYUP:
			switch (hookStruct->vkCode)
			{
			case VK_CONTROL:
				break;
			case VK_MENU:
				break;
			case 'N':
				break;
			}
			break;
		}

		//if ((hookStruct->vkCode == 'A') && locked) {
		//	cout << "The key A is blocked" << endl;
		//	return 1;
		//}
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

BOOL CInspectTestingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	// Add "About..." menu item to system menu.
	m_HWND = this->m_hWnd;


#ifndef _DEBUG_KEY
	HHOOK hhkLowLevelKybd = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, 0, 0);
#endif

	alg_params = new CParams(this);

	m_parms_dlg = new CParams_Dialog(this);
	m_parms_dlg->Create(IDD_DIALOG_PARAMS, this);



	CRect rect;
	m_Bath_Defects_List.GetClientRect(&rect);
	int nWidthUser = rect.Width() - 20;
	int nColInterval = nWidthUser / 11;

	m_Bath_Defects_List.SetView(LVS_REPORT);
	m_Bath_Defects_List.InsertColumn(0, "Unit", LVCFMT_RIGHT, nWidthUser - 7 * nColInterval);
	m_Bath_Defects_List.InsertColumn(1, "MB", LVCFMT_LEFT, nColInterval);
	m_Bath_Defects_List.InsertColumn(2, "SP", LVCFMT_LEFT, nColInterval);
	m_Bath_Defects_List.InsertColumn(3, "MBS", LVCFMT_LEFT, nColInterval);
	m_Bath_Defects_List.InsertColumn(4, "SPS", LVCFMT_LEFT, nColInterval);
	m_Bath_Defects_List.InsertColumn(5, "Open", LVCFMT_LEFT, nColInterval);
	m_Bath_Defects_List.InsertColumn(6, "Short", LVCFMT_LEFT, nColInterval);
	m_Bath_Defects_List.InsertColumn(7, "LG", LVCFMT_LEFT, nColInterval);



	m_List_Unit_Defects.SetView(LVS_REPORT);
	m_List_Unit_Defects.InsertColumn(0, "Type", LVCFMT_RIGHT, 70);
	m_List_Unit_Defects.InsertColumn(1, "Pos", LVCFMT_LEFT, 40);
	m_List_Unit_Defects.InsertColumn(2, "X", LVCFMT_LEFT, 55);
	m_List_Unit_Defects.InsertColumn(3, "Y", LVCFMT_LEFT, 55);


	m_Defect_Attribute.SetView(LVS_REPORT);
	m_Defect_Attribute.InsertColumn(0, "Name", LVCFMT_RIGHT, 80);
	m_Defect_Attribute.InsertColumn(1, "Value", LVCFMT_LEFT, 80);
	

	GenEmptyObj(&m_Cross_Point);

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	m_Image_HWND = m_Image_Control.m_hWnd;
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	inspect_utils::Init_Console();
	GetDlgItem(IDC_BUTTON_ALIGNMENT)->EnableWindow(m_Raw_Image);

	HSystem::SetWindowAttr("border_width", HTuple(0));
	HSystem::SetCheck("~father");

	m_Window.NewExternWindow(m_Image_HWND, 0, 0, 1000, 1000);
	//m_Window.OpenWindow(
	//	0, 0, 1000, 1000,
	//	(Hlong)m_Image_HWND, "visible", "");
	HSystem::SetCheck("father");

	//SetWindowAttr("background_color", "black");
	//SetWindowAttr("window_title", "Image");
	//OpenWindow(0, 0, 400, 200, 0, "visible", "", &hv_WindowHandle);
	//HDevWindowStack::Push(hv_WindowHandle);

	Set_Thread();
	m_inspect = new CInspect_Call(0, &thread_params[0].queue_log);
	Read_Default_Values();

	GetDlgItem(IDC_BUTTON_BATH_INIT)->EnableWindow(true);
	GetDlgItem(IDC_BUTTON_BATH_START)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON_BATH_STOP)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON_BATH_PREVIOUS)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON_BATH_NEXT)->EnableWindow(false);
	GetDlgItem(IDC_CHECK_DEFECT_FOUND_STOP)->EnableWindow(false);
	GetDlgItem(IDC_CHECK_AUTO_THR_BATH)->EnableWindow(false);
	GetDlgItem(IDC_EDIT_INSPECTION_UNIT)->EnableWindow(false);
	GetDlgItem(IDC_CHECK_INSPECT_LG)->EnableWindow(false);
	GetDlgItem(IDC_CHECK_INSPECT_FWM)->EnableWindow(false);
	GetDlgItem(IDC_CHECK_INSPECT_FWMS)->EnableWindow(false);


	Clear_Defects();
	//m_Check_Inspect_LG = true;
	//m_Check_Inspect_FWM = true;
	//m_Check_Inspect_FWMS = true;
	m_Prepr_K = 0.5;

	Halcon_Exchange::Init_Exchange();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CInspectTestingDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CInspectTestingDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CInspectTestingDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

CString Path2CString(fs::path path)
{
	std::string path_string{ path.u8string() };
	CString str(path_string.c_str());
	return str;
}


void CInspectTestingDlg::OnBnClickedButtonSetTeachFolder()
{
	static char BASED_CODE szFilter[] = "All (*.*)|*.*||)";
	CFileDialog	fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter);
	fileDlg.m_ofn.lpstrInitialDir = m_model_Path;

	if (fileDlg.DoModal() == IDOK)
	{
		fs::path path = fs::path((LPCTSTR)fileDlg.GetPathName()).remove_filename();
		//fs::path path = fs::path((LPCTSTR)m_Image_Name).parent_path();
		CString path_name = Path2CString(path);


		UpdateData(TRUE);
		m_Folder_Teach = path_name;
		UpdateData(FALSE);

		m_model_Path.Format("%s\\", m_Folder_Teach);
		m_bin_Path.Format("%s\\Bin\\", m_Folder_Teach);
		SetRegString("m_Folder_Teach", m_Folder_Teach);

	}
}

// ---------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------

//struct Par_Struct
//{
//	int int1;
//	double d1;
//	char str1[256];
//	char str2[256];
//	//TCHAR str1[256];
//	//WCHAR str1[256];
//};
struct Par_Struct
{
	int access;
	int order;
	double default_value;
	double min_value;
	double max_value;
	int decimal_value;
	double value;
	// ----------------------------------------------------------------------
	char text_default_value[128];
	char text_value[128];
	char folder[128];
	char id_name[128];
	char description[512];
};

struct Ret_Buf
{
	int count;
	Par_Struct *par;
};


struct Params_Struct
{
	Par_Struct *par1;
	Par_Struct *par2;
};

struct Params_Struct10
{
	Par_Struct *par1;
};
//struct Params_Struct10
//{
//	int par1[10];
//};

// -------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------

typedef int(__stdcall *_Params_Dlg)(int, LPTSTR);
typedef BYTE*(__stdcall *_Get_Params_From_File)(LPTSTR);
//typedef BYTE*(__stdcall *_Get_Params)();

void CInspectTestingDlg::OnBnClickedButtonEditParams()
{
	int ret;
	//HINSTANCE hDll = LoadLibrary("C:\\WORK_CURR\\SAMSUNG\\Inspection\\DLL\\NET_DLL\\Params_DLL\\DLL\\bin\\Debug\\DLL_NET.dll");
	HINSTANCE hDll = LoadLibrary("C:\\WORK_CURR\\SAMSUNG\\Inspection\\DLL\\NET_DLL\\Params_DLL\\Params_DLL\\Param_DLL\\bin\\Debug\\Param_DLL_NET.dll");
	if (!hDll)
	{
		ret = GetLastError();
		return;
	}


	int ret_params = 0;
	int permission = 0;
	_Params_Dlg Params_Dlg;
	Params_Dlg = (_Params_Dlg)GetProcAddress(hDll, "Params_Dlg");
	if (NULL != Params_Dlg)
	{
		// call the function
		ret_params = Params_Dlg(permission, "C:\\WORK_CURR\\SAMSUNG\\Inspection\\DLL\\NET_DLL\\Params_DLL\\Test\\Params.json");
	}

	// ---------------------------------------------------
	Ret_Buf *ret_buff;
	Par_Struct *ptr_par;
	int *par_count;

	_Get_Params_From_File Get_Params_From_File;    // Function pointer
	Get_Params_From_File = (_Get_Params_From_File)GetProcAddress(hDll, "Get_Params_From_File");
	if (NULL != Get_Params_From_File)
	{
		BYTE* pArray = Get_Params_From_File("C:\\WORK_CURR\\SAMSUNG\\Inspection\\DLL\\NET_DLL\\Params_DLL\\Test\\Params.json");
		//BYTE* pArray = Get_Params();
		par_count = (int*)pArray;
		ptr_par = (Par_Struct*)(pArray + 4);
		//ret_buff->par = (Par_Struct*)(pArray + 4);
		//ret_buff->par = (Par_Struct*)(pArray + 8);
	}


	//int ret;
	//HINSTANCE hDll = LoadLibrary("C:\\WORK_CURR\\SAMSUNG\\Inspection\\DLL\\NET_DLL\\Params_DLL\\Params_DLL\\bin\\Debug\\Params_DLL.dll");
	//if (!hDll)
	//{
	//	ret = GetLastError();
	//	return;
	//}


	//int ret_params = 0;
	//Params_Dlg params_dlg;
	//params_dlg = (Params_Dlg)GetProcAddress(hDll, "Params_Dlg");
	//if (NULL != params_dlg)
	//{
	//	// call the function
	//	ret_params = params_dlg();
	//}





}
// -------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------


typedef int(__stdcall *HelloFunc)(int);
typedef int(__stdcall *HelloFunc1)(int*);
typedef LPTSTR(__stdcall *StrFunc1)(LPTSTR);

typedef Par_Struct(__stdcall *StructFunc1)();
typedef Params_Struct(__stdcall *StructFunc2)();

typedef Params_Struct10(__stdcall *StructFunc10)();


typedef int(__stdcall *_Func_Init)();

unsigned char *m_byte;
//typedef unsigned char*(__stdcall *_Func_Get_Params)();
//typedef unsigned int(__stdcall *_Func_Get_Params)(int values[], int count);
typedef BYTE*(__stdcall *_Func_Get_Params)(int& count);



void CInspectTestingDlg::OnBnClickedButtonSetDataFolder()
{

	int ret;
	HINSTANCE hDll = LoadLibrary("C:\\WORK_CURR\\DLL\\NET_DLL\\Test\\Test_DLL_1\\Test_DLL_1\\bin\\Debug\\Test_DLL_1.dll");
	if (!hDll)
	{
		ret = GetLastError();
		return;
	}

	//HelloFunc lpfnDllFunc1;    // Function pointer
	//lpfnDllFunc1 = (HelloFunc)GetProcAddress(hDll, "Start");
	//int val = 0;
	//if (NULL != lpfnDllFunc1)
	//{
	//	// call the function

	//	val = lpfnDllFunc1(100);
	//}

	//HelloFunc1 lpfnDllFunc2;    // Function pointer
	//lpfnDllFunc2 = (HelloFunc1)GetProcAddress(hDll, "StartForms");
	//if (NULL != lpfnDllFunc2)
	//{
	//	// call the function
	//	lpfnDllFunc2(&val);
	//}
	int count;
	m_byte = (unsigned char*)malloc(50 * sizeof(Par_Struct));
	int values[] = { 1, 2, 3, 4, 5 };

	Par_Struct *param;
	BYTE* pArray;
	Ret_Buf *ret_buff;

	_Func_Get_Params Get_Params;    // Function pointer
	Get_Params = (_Func_Get_Params)GetProcAddress(hDll, "Get_Params");
	if (NULL != Get_Params)
	{
		// call the function
		//pArray = Get_Params(count);
		//ret_buff = (Ret_Buf*)pArray;
		//ret_buff->par = (Par_Struct*)(pArray + 8);
	}


	//StrFunc1 lpfnDllFunc3;    // Function pointer
	//lpfnDllFunc3 = (StrFunc1)GetProcAddress(hDll, "StartStr");
	////char str[100];
	////char *str_ptr = &str[0];
	//char *str_ptr;
	//if (NULL != lpfnDllFunc3)
	//{
	//	// call the function
	//	str_ptr = lpfnDllFunc3("Synapse Image");
	//}

	// ----------------------------------------
	Params_Struct10 m_params;
	//Par_Struct m_par[10];
	//int values[] = { 1, 2, 3, 4, 5 };
	//int *val_ptr = &values[0];
	//int ret1;

	StructFunc10 lpfnDllFunc_Struct10;    // Function pointer
	lpfnDllFunc_Struct10 = (StructFunc10)GetProcAddress(hDll, "TestStruct10");
	if (NULL != lpfnDllFunc_Struct10)
	{	
		// call the function
		m_params = lpfnDllFunc_Struct10();
	}



	Par_Struct params1;
	StructFunc1 lpfnDllFunc_Struct1;    // Function pointer
	lpfnDllFunc_Struct1 = (StructFunc1)GetProcAddress(hDll, "TestStruct1");
	if (NULL != lpfnDllFunc_Struct1)
	{
		// call the function
		params1 = lpfnDllFunc_Struct1();
	}

	//CString str;
	//str.Format("%s", params1.str1);




	Params_Struct params2;

	StructFunc2 lpfnDllFunc_Struct;    // Function pointer
	lpfnDllFunc_Struct = (StructFunc2)GetProcAddress(hDll, "TestStruct2");
	if (NULL != lpfnDllFunc_Struct)
	{
		// call the function
		params2 = lpfnDllFunc_Struct();
	}



	//static char BASED_CODE szFilter[] = "Image (*.tif; *.jpg; *.png)|*.tif;*.jpg; *.png||";
	//CFileDialog	fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter);
	//if (fileDlg.DoModal() == IDOK)
	//{
	//	CString path_name = fileDlg.GetFolderPath();
	//	UpdateData(TRUE);
	//	m_Folder_Data = path_name;
	//	UpdateData(FALSE);
	//}
}


void CInspectTestingDlg::OnBnClickedButton_ConvertToBin()
{
	CString path_teach = m_model_Path + "Teach\\";

	CString path_bin;
	path_bin.Format("%sTuples\\Bin\\", path_teach);
	if (!inspect_utils::Check_Path(path_bin))
	{
		CreateDirectory(path_bin, NULL);
	}


	Convert_To_Bin(path_teach, true);

}



HANDLE hThread;
DWORD dwThreadId;
DWORD WINAPI Inspect_Thread(LPVOID lpParam)
{
	CString log;
	while (true)
	{
		bool set_sleep = true;

		try
		{
			if (!m_main_queue_log.Empty())
			{
				log = m_main_queue_log.Pop();
				Inspect_Write_Log(log);
#ifdef _CONSOLE
				printf("%s\n", log);
#endif
				set_sleep = false;
			}


			for (int p = 0; p < m_count_processors; p++)
			{
				if (!thread_params[p].queue_log.Empty())
				{
					if (log = thread_params[p].queue_log.Pop())
					{
						Inspect_Write_Log(log);
#ifdef _CONSOLE
						printf("%s\n", log);
#endif
						set_sleep = false;
					}
				}
			}

		}
		catch (HException &except)
		{
			HTuple HExp;
			HTuple HOperatorName, HErrMsg;
			except.ToHTuple(&HExp);
			except.GetExceptionData(HExp, "operator", &HOperatorName);
			except.GetExceptionData(HExp, "error_message", &HErrMsg);

			CString sOperatorName, sErrMsg;
			sOperatorName = HOperatorName.S();
			sErrMsg = HErrMsg.S();

			Queue_Add(&m_main_queue_log, NULL, "*-- Inspect_Thread() Halcon Exception: <%s>, <%s>",
				sOperatorName, sErrMsg);
		}
		catch (...)
		{
			Queue_Add(&m_main_queue_log, NULL, "*-- Inspect_Thread() Exception");
		}

		if (set_sleep)
		{
			Sleep(5);
		}
	}
	return 0;
}
void CInspectTestingDlg::Set_Thread()
{
	//Inspect_Init_Log();

	Queue_Add(&m_main_queue_log, NULL, "*----------------------------------------------------");
	Queue_Add(&m_main_queue_log, NULL, "*-- Set_Thread(): Start Thread");

	// --- Thread parameters init -----------------------------------------------
	thread_params = new THREAD_PARAMS[m_count_processors];
	result_data = new CResultSaveData[m_count_processors];

	for (int i = 0; i < m_count_processors; i++)
	{
		thread_params[i].thread_run = true;
		thread_params[i].thread_num = i;
		thread_params[i].processor_num = i + 1;
		thread_params[i].thread = NULL;
	}


	hThread = CreateThread(
		NULL,									// default security attributes
		0,										// use default stack size  
		&Inspect_Thread,						// thread function name
		NULL,									// argument to thread function 
		0,										// use default creation flags 
		&dwThreadId);							// returns the thread identifier 
}


void CInspectTestingDlg::Read_Default_Values()
{

	// ---
	UpdateData(TRUE);
	bool ret_folder_teach = GetRegString("m_Folder_Teach", &m_Folder_Teach);
	UpdateData(FALSE);
	if (ret_folder_teach)
	{
		//bool file_exists = Check_Path(m_Folder_Teach);
		//m_Folder_Teach = "E:\\250613-big short\\01_model\\p2412-014d101_LayerL1-T-SHORT3.1-0200\\Model\\Teach\\";
		m_model_Path.Format("%s", m_Folder_Teach);
		m_bin_Path.Format("%s\\Bin\\", m_Folder_Teach);

		Queue_Add(&m_main_queue_log, NULL, "*-- Read_Default_Values(): Folder_Teach=%s", m_Folder_Teach);

		bool ret = Check_Path(m_model_Path + "Gi.tif");
		if (!ret)
		{
			return;
		}

		OnBnClickedButtonLoadTeach();
	}

	// ---
	UpdateData(TRUE);
	bool ret_img = GetRegString("m_Image_Name", &m_Image_Name);
	UpdateData(FALSE);
	if (ret_img)
	{
		GetRegString("m_Image_Path", &m_Image_Path);
		GetRegString("m_Image_File_Name", &m_Image_File_Name);
		Queue_Add(&m_main_queue_log, NULL, "*-- Read_Default_Values(): m_Image_Path=%s", m_Image_Path);
		//m_Image_Name = "D:\\p2312-015d1_2P0_240523\\p2312-015d1_2P0_240523\\RawImage_1_113.bmp";

		bool ret = Check_Path(m_Image_Name);
		if (!ret)
		{
			return;
		}

		ReadImage(&m_ho_Im, HTuple(m_Image_Name));
	}

	// ---
	UpdateData(TRUE);
	bool ret_CheckRawImage = GetRegBool("CheckRawImage", (bool*)&m_Raw_Image);
	UpdateData(FALSE);
	if (ret_CheckRawImage && m_Raw_Image)
	{
		m_Raw_Image = 1;

		//OnBnClickedButtonAlignment();
	}
	else
	{
		m_Raw_Image = 0;

	}
	GetDlgItem(IDC_BUTTON_ALIGNMENT)->EnableWindow(m_Raw_Image);
	Enable_Inspection_Buttons(!m_Raw_Image);


	// ---
	UpdateData(TRUE);
	bool ret_Init = GetRegBool("m_Init_Auto", (bool*)&m_Init_Auto);
	UpdateData(FALSE);
	if (ret_Init && ret_folder_teach && m_Init_Auto)
	{
		//OnBnClickedButtonInspectionInit();
	}

	OnBnClickedButtonLoadParams();


}

void CInspectTestingDlg::Read_Image(CString image_name)
{
	HTuple hv_fe;
	FileExists(HTuple(image_name), &hv_fe);
	if (int(hv_fe) != 0)
	{
		ReadImage(&m_ho_Im, HTuple(image_name));
		if (m_Preproc == true)
			Emphasize(m_ho_Im, &m_ho_Im, 525, 525, m_Prepr_K);
	}
	else
		AfxMessageBox(_T("Image file not found!"));

	m_Window.ClearWindow();
	Disp_Fit_Image(m_ho_Im);
}

void CInspectTestingDlg::Image_Name_Cursor_End()
{
	CEdit* h_image_name = (CEdit*)GetDlgItem(IDC_EDIT_IMAGE);
	int length = m_Image_Name.GetLength();
	h_image_name->SetSel(length, length);
}
void CInspectTestingDlg::OnBnClickedButtonSetImage()
{
//	static char BASED_CODE szFilter[] = "Image (*.tif; *.jpg; *.png; *.bmp)|*.tif;*.jpg; *.png; *.bmp||";
	static char BASED_CODE szFilter[] = "All (*.*)|*.*||)";
	CFileDialog	fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter);
	fileDlg.m_ofn.lpstrInitialDir = m_Image_Path;

	if (fileDlg.DoModal() == IDOK)
	{

		UpdateData(TRUE);
		m_Image_Name = fileDlg.GetPathName();
		m_Image_File_Name = fileDlg.GetFileName();

		//fs::path path = fs::path((LPCTSTR)m_Image_Name).remove_filename();
		fs::path path = fs::path((LPCTSTR)m_Image_Name).parent_path();
		m_Image_Path = Path2CString(path);
		UpdateData(FALSE);

		Read_Image(m_Image_Name);

		SetRegString("m_Image_Name", m_Image_Name);
		SetRegString("m_Image_Path", m_Image_Path);
		SetRegString("m_Image_File_Name", m_Image_File_Name);

		Image_Name_Cursor_End();

		Queue_Add(&m_main_queue_log, NULL, "*-- OnBnClickedButtonSetImage(): Open image: %s", m_Image_Name);
	}
}

void CInspectTestingDlg::Clear_Defects()
{
	GenEmptyObj(&m_inspect->m_ho_RegionsShi);
	GenEmptyObj(&m_inspect->m_ho_RegionsMBS);
	GenEmptyObj(&m_inspect->m_ho_RegionsSPS);
	GenEmptyObj(&m_inspect->m_ho_RegionsMB);
	GenEmptyObj(&m_inspect->m_ho_RegionsSP);
	GenEmptyObj(&m_inspect->m_ho_RegionsOp);
	GenEmptyObj(&m_inspect->m_ho_RegionsLG);
	GenEmptyObj(&m_inspect->m_ho_RegionPH);

	GenEmptyObj(&m_inspect->op2->ho_Open);
	GenEmptyObj(&m_inspect->op2->ho_Short);

	GenEmptyObj(&m_inspect->m_ho_RegionsMeanderMB);
	GenEmptyObj(&m_inspect->m_ho_RegionsMeanderSP);
	GenEmptyObj(&m_inspect->m_ho_RegionsPadMB);
	GenEmptyObj(&m_inspect->m_ho_RegionsPadSP);
	GenEmptyObj(&m_inspect->m_ho_RegionsConnPadMB);
	GenEmptyObj(&m_inspect->m_ho_RegionsConnPadSP);
	GenEmptyObj(&m_inspect->m_ho_RegionsWireAngleMB);
	GenEmptyObj(&m_inspect->m_ho_RegionsWireAngleSP);

	GenEmptyObj(&m_inspect->m_ho_RegionsDT_S);
	GenEmptyObj(&m_inspect->m_ho_RegionsDT_P);
}

void CInspectTestingDlg::OnBnClickedButtonReadImage()
{
	bool ret = Check_Path(m_Image_Name);
	if (!ret)
	{
		return;
	}

	UpdateData(TRUE);

	Read_Image(m_Image_Name);

	fs::path path = fs::path((LPCTSTR)m_Image_Name).filename();
	m_Image_File_Name = Path2CString(path);

	SetRegString("m_Image_Name", m_Image_Name);
	SetRegString("m_Image_Path", m_Image_Path);
	SetRegString("m_Image_File_Name", m_Image_File_Name);

	Clear_Defects();

	Image_Name_Cursor_End();

	UpdateData(FALSE);
	Queue_Add(&m_main_queue_log, NULL, "*-- OnBnClickedButtonReadImage(): Read image: %s", m_Image_Name);
}

void CInspectTestingDlg::OnBnClickedButtonLoadTeach()
{
	bool ret = Check_Path(m_model_Path + "Gi.tif");
	if (ret)
	{
		Load_Model(m_model_Path);
		Load_Teach(m_model_Path);

		int iNoMeander, iNoPad, iNoConnPad, iNoWireAngle;
		Load_Teach3(m_model_Path, iNoMeander, iNoPad, iNoConnPad, iNoWireAngle);
		CString cstr;
		cstr.Format(_T("%d"), iNoMeander);
		SetDlgItemText(IDC_EDIT_CBM_MEANDER, cstr);
		cstr.Format(_T("%d"), iNoPad);
		SetDlgItemText(IDC_EDIT_CBM_PAD, cstr);
		cstr.Format(_T("%d"), iNoConnPad);
		SetDlgItemText(IDC_EDIT_CBM_CONN_PAD, cstr);
		cstr.Format(_T("%d"), iNoWireAngle);
		SetDlgItemText(IDC_EDIT_CBM_WIRE_ANGLE, cstr);

		// Temporarily load a CAD file for testing.
		Load_Teach_DT(m_model_Path);

		inspect_call::m_Path_Model = m_model_Path;
		m_inspect->Init();
		m_init = 1;

		//m_inspect->op2->Init_Inspection(HTuple(m_model_Path));

		//Load_Model(path_teach);
	}
}

void CInspectTestingDlg::OnBnClickedCheckAlig2()
{
	UpdateData(TRUE);
	m_Inspection_Params.alignment_type = m_Check_Alig2 ? 2 : 1;
	UpdateData(FALSE);
}

void CInspectTestingDlg::OnBnClickedButtonAlignment()
{
	UpdateData(TRUE);
	bool ret = m_inspect->Alignment();
	if (ret)
	{
		Disp_Fit_Image(m_inspect->m_local_Im);
		Enable_Inspection_Buttons(TRUE);

	//	Queue_Add(&m_main_queue_log, NULL, "*-- OnBnClickedButtonAlignment(): OK");
	//	WriteImage(m_inspect->m_local_Im, "tiff", 0, (LPCTSTR)(m_Image_Path + "\\Im_Alignment.tif"));

		int iFrameNo, iUnitNo;
		sscanf(m_Image_File_Name, "RawImage_%d_%d.bmp", &iFrameNo, &iUnitNo);
		CString file_name;
		file_name.Format("C:\\TmpIm\\Im%d_%d.tif", iFrameNo, iUnitNo);
		WriteImage(m_inspect->m_local_Im, "tiff", 0, (HTuple)file_name);
	}

	UpdateData(FALSE);
}

void CInspectTestingDlg::OnBnClickedButtonPinholeIsland()
{
	m_inspect->Pinhole_Island();

	DispObject(m_inspect->m_ho_RegionsIsl, "yellow");
	DispObject(m_inspect->m_ho_RegionPH, "sky blue");
}

void CInspectTestingDlg::OnBnClickedCheckRawImage()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_BUTTON_ALIGNMENT)->EnableWindow(m_Raw_Image);

	SetRegBool("CheckRawImage", m_Raw_Image);

	if (!m_Raw_Image)
	{
		//m_local_Im = m_pIm;
		//GetImageSize(m_local_Im, &m_hv_Width, &m_hv_Height);

	}
	else
	{
	}

	Enable_Inspection_Buttons(!m_Raw_Image);

	UpdateData(FALSE);

}


void CInspectTestingDlg::Enable_Inspection_Buttons(bool en)
{
	GetDlgItem(IDC_BUTTON_INSPECTION_ALL)->EnableWindow(en);
	GetDlgItem(IDC_BUTTON_PINHOLE_ISLAND)->EnableWindow(en);
	GetDlgItem(IDC_BUTTON_OPEN_SHORT)->EnableWindow(en);
	GetDlgItem(IDC_BUTTON_BUILD_IRB)->EnableWindow(en);
	GetDlgItem(IDC_BUTTON_BUILD_FWMS)->EnableWindow(en);
	GetDlgItem(IDC_BUTTON_BUILD_FWM)->EnableWindow(en);

}
void CInspectTestingDlg::OnBnClickedButtonInspectionInit()
{
	GenEmptyObj(&m_Cross_Point);

	InspectParam_t inspect_param;
	inspect_param.iFovNo = 1;
	inspect_param.iFovNoX = 1;
	inspect_param.iFovNoY = 1;
	inspect_param.iFrameNo = 1;
	inspect_param.iUnitNo = 1;
	inspect_param.unit_index = 0;
	inspect_param.inspect_type = INSPECT_ONE_IMAGE;
	inspect_param.image = m_ho_Im;


	m_inspect->inspect_param = inspect_param;
	if (!m_init)
	{
		inspect_call::m_Path_Model = m_model_Path;
		m_inspect->Init();
		m_init = 1;
	}

	m_Window.ClearWindow();
	Disp_Fit_Image(m_ho_Im);

	if (!m_Raw_Image)
	{
		m_inspect->m_local_Im = m_ho_Im;

		GetImageSize(m_ho_Im, &m_inspect->m_hv_Width, &m_inspect->m_hv_Height);
		Queue_Add(&m_main_queue_log, NULL, "**** TEST1");
	}

	Enable_Inspection_Buttons(!m_Raw_Image);

	Clear_Defects();

	Queue_Add(&m_main_queue_log, NULL, "*-- OnBnClickedButtonInspectionInit() - inspect.Init(): OK", m_Image_Name);
}

void CInspectTestingDlg::OnBnClickedButtonBuildFwm()
{
	//m_Inspection_Params.sz1 = 35;
	//m_Inspection_Params.sz2 = 55;			// Running median sizes for FWM

	float stdwidth = m_ho_FWMpar.D();
	CString cstr;
	cstr.Format(_T("%.1f"), stdwidth);
	SetDlgItemText(IDC_STDWP, cstr);

	m_inspect->Proc_FWM();


	if (0)
	{
		//WriteObject(m_ho_ImCLC, "C:\\Temp\\m_ho_ImCLC");
		WriteObject(m_inspect->m_ho_RegionsMB, "C:\\Temp\\m_ho_RegionsMB");
		WriteObject(m_inspect->m_ho_RegionsSP, "C:\\Temp\\m_ho_RegionsSP");


		//AfxMessageBox(_T("Done...PWidthS_tuple.tup saved!"));

	}
	DispObject(m_inspect->m_ho_RegionsMB, "dark turquoise");
	DispObject(m_inspect->m_ho_RegionsSP, "magenta");

}

void CInspectTestingDlg::DispObject(HObject ho_Obj, HTuple hv_Color)
{
	DispObjImage(ho_Obj, hv_Color);

	//HObject Obu;
	//HTuple a, row, col, no;
	//Union1(ho_Obj, &Obu);
	//CountObj(Obu,&no);
	//if (no.L() > 0)
	//{
	//	AreaCenter(Obu, &a, &row, &col);
	//	if (a > 0)
	//	{
	//		SetColor(HDevWindowStack::GetActive(), hv_Color);
	//		SetLineWidth(HDevWindowStack::GetActive(), 3);
	//		SetDraw(HDevWindowStack::GetActive(), "margin");
	//		DispObj(ho_Obj, HDevWindowStack::GetActive());
	//	}
	//}
	
}

void CInspectTestingDlg::OnBnClickedButtonMing()
{
}

void CInspectTestingDlg::OnBnClickedButtonBuildFwms()
{
	//m_Inspection_Params.sz1 = 45;
	//m_Inspection_Params.sz2 = 45;			// Running median sizes for FWM and FWMS

	m_inspect->Proc_FWMS();


	DispObject(m_inspect->m_ho_RegionsMBS, "orange");
	DispObject(m_inspect->m_ho_RegionsSPS, "dark orchid");

	if (WRTEST) //WRTEST
	{
		WriteObject(m_inspect->m_ho_RegionsMBS, "C:\\Temp\\m_ho_RegionsMBS");
		WriteObject(m_inspect->m_ho_RegionsSPS, "C:\\Temp\\m_ho_RegionsSPS");
		WriteObject(m_ho_SkeletonsFWMcut, "C:\\Temp1\\m_ho_SkeletonsFWMcut");
		WriteObject(m_ho_SkeletonsFWMScut, "C:\\Temp1\\m_ho_SkeletonsFWMScut");
		
	}

	//HObject MBu;
	//HTuple a, row, col;
	//Union1(m_inspect->m_ho_RegionsMBS, &MBu);
	//AreaCenter(MBu, &a, &row, &col);
	//if (a > 0)
	//{
	//	DispObj(m_inspect->m_ho_RegionsMBS, HDevWindowStack::GetActive());
	//}
	//
	//DispObj(m_inspect->m_ho_RegionsSPS, HDevWindowStack::GetActive());


	///*HTuple medoS_tuple((float*)inspect.m_pFWM->pmedoS, 7472);
	//WriteTuple(medoS_tuple, "C:\\Temp\\medoS_tuple.tup");
	//AfxMessageBox(_T("medoS tuple saved!"));*/
	//Queue_Add(&m_main_queue_log, NULL, "*-- OnBnClickedButtonProcwms() -FWMS finished!");

	////HTuple hv_RowMBSs=ConvertFloatToTuple(pYdefMBS, MBnumS);
	////HTuple hv_ColumnMBSs = ConvertFloatToTuple(pXdefMBS, MBnumS);
	//INT64 *PSx = ConvertFloatToINT64(inspect.m_pFWM->pXdefMBS, inspect.m_pFWM->MBnumS);
	//INT64 *PSy = ConvertFloatToINT64(inspect.m_pFWM->pYdefMBS, inspect.m_pFWM->MBnumS);

	//HTuple PMBSx_tuple;
	//HTuple PMBSy_tuple;

	//for (int i = 0; i < inspect.m_pFWM->MBnumS; i++)
	//{
	//	PMBSx_tuple[i] = (HTuple)*(PSx + i);
	//	PMBSy_tuple[i] = (HTuple)*(PSy + i);
	//}


	//HTuple PMBSx_tuple1;
	//HTuple PMBSy_tuple1;
	//PMBSy_tuple1 = HTuple(PSx, inspect.m_pFWM->MBnumS);
	//PMBSy_tuple1 = HTuple(PSy, inspect.m_pFWM->MBnumS);




	//HTuple  hv_nMBS = HTuple(inspect.m_pFWM->MBnumS);
	////Disp
	//if (hv_nMBS > 0)
	//{
	//	HObject CircleMBSs;
	//	HTuple hv_TR;
	//	if (HDevWindowStack::IsOpen())
	//		SetColor(HDevWindowStack::GetActive(), "magenta");
	//	SetDraw(HDevWindowStack::GetActive(), "margin");
	//	TupleGenConst(hv_nMBS, 30, &hv_TR);
	//	GenCircle(&CircleMBSs, PMBSy_tuple, PMBSx_tuple, hv_TR);
	//	DispObj(CircleMBSs, HDevWindowStack::GetActive());
	//}

	//INT64 *PSSPx = ConvertFloatToINT64(inspect.m_pFWM->pXdefSPS, inspect.m_pFWM->SPnumS);
	//INT64 *PSSPy = ConvertFloatToINT64(inspect.m_pFWM->pYdefSPS, inspect.m_pFWM->SPnumS);

	//HTuple PSPSx_tuple;
	//HTuple PSPSy_tuple;

	//for (int i = 0; i < inspect.m_pFWM->SPnumS; i++)
	//{
	//	PSPSx_tuple[i] = (HTuple)*(PSSPx + i);
	//	PSPSy_tuple[i] = (HTuple)*(PSSPy + i);
	//}

	//HTuple  hv_nSPS = HTuple(inspect.m_pFWM->SPnumS);
	////Disp
	//if (hv_nSPS > 0)
	//{
	//	HObject CircleSPSs;
	//	HTuple hv_TR;
	//	if (HDevWindowStack::IsOpen())
	//		SetColor(HDevWindowStack::GetActive(), "cyan");
	//	SetDraw(HDevWindowStack::GetActive(), "margin");
	//	TupleGenConst(hv_nSPS, 30, &hv_TR);
	//	GenCircle(&CircleSPSs, PSPSy_tuple, PSPSx_tuple, hv_TR);
	//	DispObj(CircleSPSs, HDevWindowStack::GetActive());
	//}
	//
}

void CInspectTestingDlg::OnBnClickedButtonReadIrb()
{

	//HTuple hv_path = m_model_Path + "Teach\\Test\\";

	////inspect.m_local_Im = m_ho_Im;
	////ReadImage(&inspect.m_local_Im, hv_path + "Im.tif");
	//ReadImage(&m_ho_ImCLC, hv_path + "ImCLC.tif");
	//ReadImage(&inspect.m_pFWM->ho_ImIRBP, hv_path + "ImIRBP.tif");
	////ReadImage(&hp.ho_ImIRB, path + "ImIRBo.tif");
	//ReadImage(&inspect.m_pFWM->ho_ImIRBS, hv_path + "ImIRBS.tif");
	//////ReadImage(&ho_ImCLCS, path + "ImCLCS.tif");
	//////int div = 28;
	//float div = 1.5;
	//HTuple w, h;
	//GetImageSize(inspect.m_local_Im, &w, &h);
	//////if (!HDevWindowStack::IsOpen())
	//static int imopen = 0;
	//if (imopen != 1)
	//{
	//	SetWindowAttr("background_color", "black");
	//	//OpenWindow(0, 0, 400, 800, 0, "visible", "", &hv_WindowHandle);
	//	SetWindowAttr("window_title", "Image");
	//	OpenWindow(0, 0, w / div, h / div, 0, "visible", "", &hv_WindowHandle);
	//	HDevWindowStack::Push(hv_WindowHandle);
	//	imopen = 1;
	//}
	//SetPart(hv_WindowHandle, h/2, w/2, h - 1, w - 1);
	//SetPart(hv_WindowHandle, 0, 0, h - 1, w - 1);
	//DispObj(m_ho_Im, HDevWindowStack::GetActive());

	//Queue_Add(&m_main_queue_log, NULL, "*-- OnBnClickedButtonReadIrb(): Read IRB image: OK");

}

void CInspectTestingDlg::OnBnClickedButtonOpenShort2()
{

}

void CInspectTestingDlg::OnBnClickedButtonOpenShort()
{
	UpdateData(TRUE);

	if (m_Check_Open_Short_2)
	{
		
		m_inspect->OpenShort2();

		DispObject(m_inspect->m_ho_RegionsShort2, "red");
		DispObject(m_inspect->m_ho_RegionsOpen2, "blue");
	}
	else
	{
		m_inspect->OpenShort();
		//DispObject(m_inspect->m_ho_RegionsShi, "red");
		DispObject(m_inspect->m_ho_RegionsOp, "blue");
	}

	UpdateData(FALSE);
}


void CInspectTestingDlg::OnBnClickedButtonBuildIrb()
{
	m_inspect->Build_IRB();

}


void CInspectTestingDlg::OnEnChangeEditImage()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}


void CInspectTestingDlg::OnEnChangeEditprcmbs()
{
	CString cstr;
	GetDlgItemText(IDC_EDITPRCMBS, cstr);
	m_Inspection_Params.prcMBS = (float)_tstof(cstr);
}


void CInspectTestingDlg::OnBnClickedCheckInitAuto()
{
	UpdateData(TRUE);

	SetRegBool("m_Init_Auto", m_Init_Auto);

	UpdateData(FALSE);
}


void CInspectTestingDlg::OnBnClickedViewirbp()
{
	//SetWindowAttr("window_title", "Image IRBP");
	//DispObj(m_inspect->m_pFWM->ho_ImIRBP, HDevWindowStack::GetActive());

	m_Window.DispObj(m_inspect->m_pFWM->ho_ImIRBP);
	m_Disp_Image = m_inspect->m_pFWM->ho_ImIRBP;

}


void CInspectTestingDlg::OnBnClickedViewirbs()
{
	//SetWindowAttr("window_title", "Image IRBS");
	//DispObj(m_inspect->m_pFWM->ho_ImIRBS, HDevWindowStack::GetActive());

	m_Window.DispObj(m_inspect->m_pFWM->ho_ImIRBS);
	m_Disp_Image = m_inspect->m_pFWM->ho_ImIRBS;

	//WriteObject(m_inspect->m_pFWM->ho_ImIRBS, "C:\\Temp\\ho_ImIRBS");
	
}


void CInspectTestingDlg::OnBnClickedViewim()
{
	//SetWindowAttr("window_title", "Image Im");
	//DispObj(m_inspect->m_local_Im, HDevWindowStack::GetActive());
	m_Window.DispObj(m_inspect->m_local_Im);
	//Disp_Fit_Image(m_inspect->m_local_Im);
	m_Disp_Image = m_inspect->m_local_Im;
}


void CInspectTestingDlg::OnBnClickedButtonInspectionAll()
{
	for (int j = 1; j < 20; j++)
	{
		OnBnClickedButtonReadImage();
		OnBnClickedButtonInspectionAll2();
		Sleep(1000);
		OnBnClickedWizfinish();

	}

}


void CInspectTestingDlg::OnEnChangeEditprcsps()
{
	CString cstr;
	GetDlgItemText(IDC_EDITPRCSPS, cstr);
	m_Inspection_Params.prcSPS = (float)_tstof(cstr);
}




void CInspectTestingDlg::OnEnChangeEditthr()
{
	CString cstr;
	GetDlgItemText(IDC_EDITTHR, cstr);
	m_Inspection_Params.threashold = (float)_tstof(cstr);
}

void CInspectTestingDlg::Update_Dlg_Params()
{
	UpdateData(TRUE);

	CString cstr;

	cstr.Format(_T("%.0f"), m_Inspection_Params.prcMBS);
	SetDlgItemText(IDC_EDITPRCMBS, cstr);
	cstr.Format(_T("%.0f"), m_Inspection_Params.prcSPS);
	SetDlgItemText(IDC_EDITPRCSPS, cstr);

	cstr.Format(_T("%.0f"), m_Inspection_Params.prcMB);
	SetDlgItemText(IDC_EDITPRCMBP, cstr);
	cstr.Format(_T("%.0f"), m_Inspection_Params.prcSP);
	SetDlgItemText(IDC_EDITPRCSPP, cstr);

	cstr.Format(_T("%.2f"), m_Inspection_Params.absSP);
	SetDlgItemText(IDC_EDITMABS, cstr);

	cstr.Format(_T("%d"), m_Inspection_Params.mmat_shift);
	SetDlgItemText(IDC_EDITMMATS, cstr);

	cstr.Format(_T("%d"), m_Inspection_Params.LGE_mid);
	SetDlgItemText(IDC_EDITMID, cstr);

	cstr.Format(_T("%d"), m_Inspection_Params.LGE_margin);
	SetDlgItemText(IDC_EDITMARGIN, cstr);

	cstr.Format(_T("%.1f"), m_Inspection_Params.minMBS);
	SetDlgItemText(IDC_EDITMINABS, cstr);

	cstr.Format(_T("%.1f"), m_Inspection_Params.narWSpace);
	SetDlgItemText(IDC_EDITMINABS2, cstr);


	cstr.Format(_T("%d"), m_Inspection_Params.threashold);
	SetDlgItemText(IDC_EDITTHR, cstr);

	cstr.Format(_T("%d"), m_Inspection_Params.sz1);
	SetDlgItemText(IDC_EDITSZP, cstr);

	cstr.Format(_T("%d"), m_Inspection_Params.sz2);
	SetDlgItemText(IDC_EDITSZS, cstr);

	cstr.Format(_T("%.0f"), m_Inspection_Params.minwp);
	SetDlgItemText(IDC_EDITMINWP, cstr);

	cstr.Format(_T("%.1f"), m_Inspection_Params.narrowwp);
	SetDlgItemText(IDC_EDITNARWP, cstr);

	cstr.Format(_T("%.1f"), m_Inspection_Params.widewp);
	SetDlgItemText(IDC_EDITWIDEWP, cstr);



	m_Lg_Prs_P.Format("%d", m_Inspection_Params.LG_m_prsLG_P);
	m_Lg_Prs_S.Format("%d", m_Inspection_Params.LG_m_prsLG);
	m_Lg_Szw_P.Format("%d", m_Inspection_Params.LG_m_szw_P);
	m_Lg_Szw_S.Format("%d", m_Inspection_Params.LG_m_szw);
	m_Lg_Avg_P.Format("%d", m_Inspection_Params.LG_m_Avg_Par);


	m_Check_Alig2 = inspect_call::m_Inspection_Params.alignment_type == 2;

	cstr.Format(_T("%.1f"), m_Inspection_Params.fCBM_AbsMB_Meander);
	SetDlgItemText(IDC_EDIT_ABSMB_MEANDER, cstr);
	cstr.Format(_T("%.1f"), m_Inspection_Params.fCBM_AbsSP_Meander);
	SetDlgItemText(IDC_EDIT_ABSSP_MEANDER, cstr);
	cstr.Format(_T("%d"), m_Inspection_Params.iCBM_Rmsize_Meander);
	SetDlgItemText(IDC_EDIT_RMSIZE_MEANDER, cstr);
	cstr.Format(_T("%.1f"), m_Inspection_Params.fCBM_AbsMB_Pad);
	SetDlgItemText(IDC_EDIT_ABSMB_PAD, cstr);
	cstr.Format(_T("%.1f"), m_Inspection_Params.fCBM_AbsSP_Pad);
	SetDlgItemText(IDC_EDIT_ABSSP_PAD, cstr);
	cstr.Format(_T("%d"), m_Inspection_Params.iCBM_Rmsize_Pad);
	SetDlgItemText(IDC_EDIT_RMSIZE_PAD, cstr);
	cstr.Format(_T("%.1f"), m_Inspection_Params.fCBM_AbsMB_ConnPad);
	SetDlgItemText(IDC_EDIT_ABSMB_CONN_PAD, cstr);
	cstr.Format(_T("%.1f"), m_Inspection_Params.fCBM_AbsSP_ConnPad);
	SetDlgItemText(IDC_EDIT_ABSSP_CONN_PAD, cstr);
	cstr.Format(_T("%d"), m_Inspection_Params.iCBM_Rmsize_ConnPad);
	SetDlgItemText(IDC_EDIT_RMSIZE_CONN_PAD, cstr);
	cstr.Format(_T("%.1f"), m_Inspection_Params.fCBM_AbsMB_WireAngle);
	SetDlgItemText(IDC_EDIT_ABSMB_WIRE_ANGLE, cstr);
	cstr.Format(_T("%.1f"), m_Inspection_Params.fCBM_AbsSP_WireAngle);
	SetDlgItemText(IDC_EDIT_ABSSP_WIRE_ANGLE, cstr);
	cstr.Format(_T("%d"), m_Inspection_Params.iCBM_Rmsize_WireAngle);
	SetDlgItemText(IDC_EDIT_RMSIZE_WIRE_ANGLE, cstr);

	cstr.Format(_T("%d"), m_Inspection_Params.iDt_Dark_Space);
	SetDlgItemText(IDC_EDIT_DT_DARK_SPACE, cstr);
	cstr.Format(_T("%d"), m_Inspection_Params.iDt_Bright_Space);
	SetDlgItemText(IDC_EDIT_DT_BRIGHT_SPACE, cstr);
	cstr.Format(_T("%d"), m_Inspection_Params.iDt_Mean_Space);
	SetDlgItemText(IDC_EDIT_DT_MEAN_SPACE, cstr);
	cstr.Format(_T("%d"), m_Inspection_Params.iDt_Margin_Space);
	SetDlgItemText(IDC_EDIT_DT_MARGIN_SPACE, cstr);
	cstr.Format(_T("%d"), m_Inspection_Params.iDt_Area_Space);
	SetDlgItemText(IDC_EDIT_DT_AREA_SPACE, cstr);
	cstr.Format(_T("%d"), m_Inspection_Params.iDt_Dark_Pattern);
	SetDlgItemText(IDC_EDIT_DT_DARK_PATTERN, cstr);
	cstr.Format(_T("%d"), m_Inspection_Params.iDt_Bright_Pattern);
	SetDlgItemText(IDC_EDIT_DT_BRIGHT_PATTERN, cstr);
	cstr.Format(_T("%d"), m_Inspection_Params.iDt_Margin_Pattern);
	SetDlgItemText(IDC_EDIT_DT_MARGIN_PATTERN, cstr);
	cstr.Format(_T("%d"), m_Inspection_Params.iDt_Area_Pattern);
	SetDlgItemText(IDC_EDIT_DT_AREA_PATTERN, cstr);
	cstr.Format(_T("%d"), m_Inspection_Params.iDt_Length_Pattern);
	SetDlgItemText(IDC_EDIT_DT_LENGTH_PATTERN, cstr);
	cstr.Format(_T("%d"), m_Inspection_Params.iDt_Width_Pattern);
	SetDlgItemText(IDC_EDIT_DT_WIDTH_PATTERN, cstr);
	CButton *m_ctlCheck = (CButton*)GetDlgItem(IDC_CHECK_DT_CONNECTION);
	m_ctlCheck->SetCheck(m_Inspection_Params.bDtConnectDef);
	cstr.Format(_T("%d"), m_Inspection_Params.iDt_Connect_Distance);
	SetDlgItemText(IDC_EDIT_DT_CONNECTION_DISTANCE, cstr);
	
	UpdateData(FALSE);
}
void CInspectTestingDlg::OnBnClickedButtonLoadParams()
{
	//inspect_call::Load_Params(m_model_Path, 3000);
	inspect_call::Load_Params(m_model_Path, 3000, &inspect_call::m_Inspection_Params, &m_main_queue_log);

	Update_Dlg_Params();
}


void CInspectTestingDlg::OnBnClickedButtonSaveParams()
{

	inspect_call::Save_Params(m_model_Path, &inspect_call::m_Inspection_Params, &m_main_queue_log);
	//inspect_call::Save_Params(m_model_Path);
}


void CInspectTestingDlg::OnBnClickedButtonLowGray()
{
	m_Window.DispObj(m_inspect->m_local_Im);
	m_Disp_Image = m_inspect->m_local_Im;
	
	//m_Inspection_Params.LG_m_prsLG = 70;
	GenEmptyObj(&m_inspect->m_ho_RegionsLG);
	m_inspect->Low_Gray(m_inspect->threshold);
	DispObject(m_inspect->m_ho_RegionsLG, "green");

	//WriteImage(m_inspect->m_local_Im, "tiff", 0, "C:\\TmpIm\\Im0.tif");

	//m_inspect->Low_Gray();
	/*HObject ho_Dil;
	DilationCircle(m_inspect->m_ho_RegionsLG, &ho_Dil, 10);
	DispObject(ho_Dil, "red");*/
	//DispObject(m_inspect->m_ho_RegionsLG, "red");
}


void CInspectTestingDlg::OnBnClickedButtonAutoThr()
{
	HTuple hv_thr;
	//Auto_Threshold2(m_ho_SkeletonsFWMS, m_inspect->m_local_Im, &hv_thr);
	int threshold;
	if (m_Auto_Thr)
	{
		threshold = m_inspect->Proc_MinG(m_Inspection_Params.running_medians_span) + m_Inspection_Params.mmat_shift;
		if (threshold  > 250)
			threshold = 150;
		m_Inspection_Params.threashold = threshold;

		//Auto_Threshold2(m_ho_SkeletonsFWMS, m_inspect->m_local_Im, &hv_thr);
		//m_Inspection_Params.threashold = hv_thr.ToDArr()[0];
	}
	/*else
		hv_thr=m_Inspection_Params.threashold;*/

	//Auto_Threshold(m_ho_RegProc, m_inspect->m_local_Im, &hv_thr);

	m_inspect->threshold = m_Inspection_Params.threashold;

	CString cstr;
	cstr.Format(_T("%d"), m_Inspection_Params.threashold);
	SetDlgItemText(IDC_EDITTHR, cstr);
}


void CInspectTestingDlg::OnEnChangeEditprcmbp()
{
	CString cstr;
	GetDlgItemText(IDC_EDITPRCMBP, cstr);
	m_Inspection_Params.prcMB = (float)_tstof(cstr);
}


void CInspectTestingDlg::OnEnChangeEditprcspp()
{
	CString cstr;
	GetDlgItemText(IDC_EDITPRCSPP, cstr);
	m_Inspection_Params.prcSP = (float)_tstof(cstr);
}


void CInspectTestingDlg::OnBnClickedButtonInspectionAll2()
{
	OnBnClickedButtonInspectionInit();

	OnBnClickedButtonAlignment();

	OnBnClickedButtonAutoThr();
	CString cstr;
	cstr.Format(_T("%d"), m_Inspection_Params.threashold);
	SetDlgItemText(IDC_EDITTHR, cstr);

	OnBnClickedButtonPinholeIsland();
	OnBnClickedButtonOpenShort();
	OnBnClickedButtonBuildIrb();

	GenEmptyObj(&m_inspect->m_ho_RegionsLG);
	OnBnClickedButtonLowGray();
	//bool ret = m_inspect->Alignment();
	//if (ret)
	//{
	//	Disp_Image(m_inspect->m_local_Im);
	//	Enable_Inspection_Buttons(TRUE);


	//	//	Queue_Add(&m_main_queue_log, NULL, "*-- OnBnClickedButtonAlignment(): OK");
	//	WriteImage(m_inspect->m_local_Im, "tiff", 0, "C:\\TmpIm\\Im_Alignment.tif");
	//}

	//HTuple hv_thr;
	////Auto_Threshold(m_ho_RegProc, m_inspect->m_local_Im, &hv_thr);
	//Auto_Threshold2(m_ho_SkeletonsFWMS, m_inspect->m_local_Im, &hv_thr);
	//m_Inspection_Params.threashold = hv_thr.ToDArr()[0];

	//CString cstr;
	//cstr.Format(_T("%d"), m_Inspection_Params.threashold);
	//SetDlgItemText(IDC_EDITTHR, cstr);

	//SetWindowAttr("window_title", "Image Im");
	//Disp_Image(m_inspect->m_local_Im);

	//m_inspect->Init();
	//m_inspect->Pinhole_Island();
	//m_inspect->OpenShort();
	//m_inspect->Build_IRB();
	////inspect.Proc_FWM();
	OnBnClickedButtonBuildFwms();
	DispObject(m_inspect->m_ho_RegionsShi, "red");
	OnBnClickedButtonBuildFwm();


	int cnt = Disp_Defects();
	if (cnt)
	{
		PlaySound("..\\Inspect_Testing\\res\\ringout.wav", NULL, SND_FILENAME | SND_ASYNC);
		//Beep(1000, 500);
	}

}


void CInspectTestingDlg::OnBnClickedViewinod()
{
	//SetWindowAttr("window_title", "RegionInoD");
	////DispObj(m_inspect->m_pFWM->ho_RegionInoD, HDevWindowStack::GetActive());
	//DispObject(m_inspect->m_pFWM->ho_RegionInoD, "blue");

	m_Window.SetColor("coral");
	m_Window.DispObj(m_inspect->m_pFWM->ho_RegionInoD);

}


void CInspectTestingDlg::OnBnClickedViewskelg2()
{
	m_Window.SetColor("green");
	//m_Window.DispObj(m_ho_SkeletonsFWMcut);
	m_Window.DispObj(m_ho_SkeletonsFWM);
}

void CInspectTestingDlg::OnBnClickedViewskelg()
{
	//SetWindowAttr("window_title", "SkeletonsFWMScut");
	//DispObj(m_inspect->m_pFWM->ho_RegionInoD, HDevWindowStack::GetActive());
	//DispObject(m_ho_SkeletonsLG, "red");
	//DispObject(m_ho_SkeletonsFWMScut, "red");
	m_Window.SetColor("red");
	m_Window.DispObj(m_ho_SkeletonsFWMScut);

}


void CInspectTestingDlg::OnBnClickedCheckAutothr()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_BUTTON_ALIGNMENT)->EnableWindow(m_Auto_Thr);


	m_Inspection_Params.inspect_Auto_Thr = m_Auto_Thr;
	//SetRegBool("CheckRawImage", m_Raw_Image);

	//if (!m_Auto_Thr)
	//{
	//	//m_local_Im = m_pIm;
	//	//GetImageSize(m_local_Im, &m_hv_Width, &m_hv_Height);

	//}
	//else
	//{
	//}

	Enable_Inspection_Buttons(!m_Auto_Thr);

	UpdateData(FALSE);
}


void CInspectTestingDlg::OnEnChangeEditmabs()
{
	CString cstr;
	GetDlgItemText(IDC_EDITMABS, cstr);
	m_Inspection_Params.absSP = (float)_tstof(cstr);
}

void CInspectTestingDlg::OnEnChangeEditmmats()
{
	CString cstr;
	GetDlgItemText(IDC_EDITMMATS, cstr);
	m_Inspection_Params.mmat_shift = (int)_tstof(cstr);
}


void CInspectTestingDlg::OnEnChangeEditminabs()
{
	CString cstr;
	GetDlgItemText(IDC_EDITMINABS, cstr);
	m_Inspection_Params.minMBS = (float)_tstof(cstr);
}

void CInspectTestingDlg::OnEnChangeEditminabs2()
{
	CString cstr;
	GetDlgItemText(IDC_EDITMINABS2, cstr);
	m_Inspection_Params.narWSpace = (float)_tstof(cstr);
}


void CInspectTestingDlg::OnBnClickedWizfinish()
{
	HString newname;
	HTuple hv_name = HTuple(m_Image_Name), hv_newname;
	UpdateFileName(hv_name, "\\", 1, 0, &hv_newname);
	m_Image_Name = *hv_newname.ToSArr();
	UpdateData(FALSE);
	Queue_Add(&m_main_queue_log, NULL, "*-- OnBnClickedButtonSetImage(): Open image: %s", m_Image_Name);

}

void CInspectTestingDlg::UpdateFileName(HTuple hv_name, HTuple hv_spl, HTuple hv_nch, HTuple hv_rch,
	HTuple *hv_fname)
{

	// Local iconic variables

	// Local control variables
	HTuple  hv_Folders, hv_nf, hv_path, hv_j, hv_Substrings;
	HTuple  hv_n, hv_trow, hv_urow, hv_urstr, hv_tnum, hv_Ts;
	HTuple  hv_Tn, hv_unum, hv_unstr;

	//UpdateFileName
	//24.06.2024
	TupleSplit(hv_name, hv_spl, &hv_Folders);

	TupleLength(hv_Folders, &hv_nf);
	(*hv_fname) = HTuple(hv_Folders[hv_nf - 1]);
	hv_path = ((const HTuple&)hv_Folders)[0];
	{
		HTuple end_val7 = hv_nf - 2;
		HTuple step_val7 = 1;
		for (hv_j = 1; hv_j.Continue(end_val7, step_val7); hv_j += step_val7)
		{
			hv_path += hv_spl;
			hv_path += HTuple(hv_Folders[hv_j]);
		}
	}
	hv_path += hv_spl;
	//fname1 := Folders[0]+'/'+Folders[1]+'/'

	TupleSplit((*hv_fname), "_", &hv_Substrings);
	TupleLength(hv_Substrings, &hv_n);
	hv_trow = HTuple(hv_Substrings[hv_n - 2]);
	TupleNumber(hv_trow, &hv_urow);
	hv_urow += hv_rch;
	hv_urstr = hv_urow.TupleString("d");
	hv_tnum = HTuple(hv_Substrings[hv_n - 1]);
	TupleSplit(hv_tnum, ".", &hv_Ts);
	TupleNumber(hv_Ts, &hv_Tn);
	hv_unum = HTuple(hv_Tn[0]) + hv_nch;
	hv_unstr = hv_unum.TupleString("d");
	(*hv_fname) = (((((hv_path + "RawImage_") + hv_urstr) + "_") + hv_unstr) ) + ".bmp";
	return;
}



void CInspectTestingDlg::OnBnClickedWizfinish2()
{
	HString newname;
	HTuple hv_name = HTuple(m_Image_Name), hv_newname;
	UpdateFileName(hv_name, "\\", 0, 1, &hv_newname);
	m_Image_Name = *hv_newname.ToSArr();
	UpdateData(FALSE);
	Queue_Add(&m_main_queue_log, NULL, "*-- OnBnClickedButtonSetImage(): Open image: %s", m_Image_Name);
}


void CInspectTestingDlg::OnBnClickedWizfinish3()
{
	HString newname;
	HTuple hv_name = HTuple(m_Image_Name), hv_newname;
	UpdateFileName(hv_name, "\\", -1, 0, &hv_newname);
	m_Image_Name = *hv_newname.ToSArr();
	UpdateData(FALSE);
	Queue_Add(&m_main_queue_log, NULL, "*-- OnBnClickedButtonSetImage(): Open image: %s", m_Image_Name);
}


void CInspectTestingDlg::OnBnClickedWizfinish4()
{
	HString newname;
	HTuple hv_name = HTuple(m_Image_Name), hv_newname;
	UpdateFileName(hv_name, "\\", 0, -1, &hv_newname);
	m_Image_Name = *hv_newname.ToSArr();
	UpdateData(FALSE);
	Queue_Add(&m_main_queue_log, NULL, "*-- OnBnClickedButtonSetImage(): Open image: %s", m_Image_Name);
}


void Halcon_Test()
{
	HTuple  hv_s1, hv_Mx_Size, hv_Matrix_Gray, hv_Mx_Pos;
	HTuple  hv_Mx_Cnt, hv_Depth_Pos, hv_Gray_Len, hv_I, hv_Gray;
	HTuple  hv_Col_Seq, hv_Col_Seq1, hv_Row_Seq1, hv_Row_Seq, hv_Mx_Pos_Last, hv_Matrix_Gray_Min;
	HTuple  hv_Gray_Last, hv_Depth, hv_Depth_S, hv_s2, hv_s;



	hv_Mx_Size = 20;
	CreateMatrix(hv_Mx_Size, 100, 255, &hv_Matrix_Gray);
	hv_Mx_Pos = 0;
	hv_Mx_Cnt = 0;

	hv_Depth_Pos = 0;
	hv_Gray_Len = 50;
	TupleGenConst(hv_Gray_Len, 0, &hv_Row_Seq1);
	TupleGenSequence(0, hv_Gray_Len - 1, 1, &hv_Gray);
	TupleGenSequence(0, hv_Gray_Len - 1, 1, &hv_Col_Seq);

	{
		HTuple end_val11 = 20000 - 1;
		HTuple step_val11 = 1;
		for (hv_I = 0; hv_I.Continue(end_val11, step_val11); hv_I += step_val11)
		{

			TupleGenConst(hv_Gray_Len, hv_Mx_Pos, &hv_Row_Seq);
			SetValueMatrix(hv_Matrix_Gray, hv_Row_Seq, hv_Col_Seq, hv_Gray);
			hv_Mx_Pos += 1;
			if (0 != (hv_Mx_Pos >= (hv_Mx_Size - 1)))
			{
				hv_Mx_Pos = 0;
			}


			if (0 != (hv_Mx_Cnt < (hv_Mx_Size - 1)))
			{
				hv_Mx_Cnt += 1;
				hv_Mx_Pos_Last = 0;
			}
			else
			{
				MinMatrix(hv_Matrix_Gray, "columns", &hv_Matrix_Gray_Min);
				GetValueMatrix(hv_Matrix_Gray_Min, hv_Row_Seq1, hv_Col_Seq, &hv_Gray_Last);

				TupleMax(hv_Gray_Last, &hv_Depth);
				hv_Depth_S[hv_Depth_Pos] = hv_Depth;
				hv_Depth_Pos += 1;

			}



		}
	}
}

void CPP_Test()
{
	int mx_size = 20;
	int mx[20][50] = { 255 };
	int rec[50] = { 1 };
	int min_val[50] = { 100 };

	int mx_pos = 0;
	int mx_Cnt = 0;


	int depth[20000] = { 0 };

	for (int i = 0; i < 50; i++)
	{
		rec[i] = (rand() % 1000);
	}


	for (int i = 0; i < 20000; i++)
	{
		memcpy(mx[mx_pos], rec, sizeof(int) * 50);

		mx_pos = mx_pos + 1;
		if (mx_pos >= mx_size - 1)
		{
			mx_pos = 0;
		}

		if (mx_Cnt < mx_size)
		{
			mx_Cnt += 1;
		}
		else
		{
			for (int a = 0; a < 50; a++)
			{
				int _min_val = 1000000;
				//for (int m = 0; m < 20; m++)
				//{
				//	if (mx[m][a] < _min_val)
				//	{
				//		_min_val = mx[m][a];
				//	}

				//}
				min_val[a] = _min_val;
			}

			int _min_val = 1000000;
			for (int m = 0; m < 50; m++)
			{
				if (min_val[m] < _min_val)
				{
					_min_val = min_val[m];
				}
			}

			depth[i] = _min_val;

		}




	}








}

// -----------------------------------------------------------------------------------------------------------
// --- Bath_Inspect ------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
CStringArray m_inspect_folder_files;
InspectParam_t m_inspect_param;
int m_Bath_Pos;
int m_Bath_From;
int m_Bath_To;
int m_Bath_Image_Count;
bool m_Bath_Stop;
int m_Bath_Stop_Pos;
int m_Alignment_Type;

CString m_image_name;

HANDLE hThread_Bath;
DWORD dwThreadId_Bath;
std::mutex m_mutex_Bath;

bool Inspect_Units()
{
	std::unique_lock<std::mutex> lock(m_mutex_Bath);

	m_image_name = m_inspect_folder_files.GetAt(m_Bath_Pos);
	/*HTuple hv_imname = HTuple(m_image_name);
	HTuple hv_RIm;
	TupleSelectRange(hv_imname, HTuple(0), HTuple(7), &hv_RIm);
	if (hv_RIm != "RawImage");*/
	//char *s="        ";
	CString CS;
	CS=m_image_name.Left(8);
	if(CS!="RawImage")
		return false;
	HObject ho_Im;
	ReadImage(&ho_Im, HTuple(m_Image_Path + "\\" + m_image_name));

	m_inspect_param.iFovNo = 1;
	m_inspect_param.iFovNoX = 1;
	m_inspect_param.iFovNoY = 1;
	m_inspect_param.iFrameNo = 1;
	m_inspect_param.iUnitNo = 1;
	m_inspect_param.unit_index = 0;
	m_inspect_param.inspect_type = INSPECT_ONE_IMAGE;

	m_inspect_param.image = ho_Im;
	m_inspect->inspect_param = m_inspect_param;

	bool ret = m_inspect->Alignment();

	if (ret == false)
	{
		return true;
	}

	if (m_Inspection_Params.inspect_Auto_Thr)
	{
		m_Inspection_Params.threashold = m_inspect->Proc_MinG(m_Inspection_Params.running_medians_span) + m_Inspection_Params.mmat_shift;
		if (m_Inspection_Params.threashold  > 250)
			m_Inspection_Params.threashold = 150;
		m_inspect->threshold = m_Inspection_Params.threashold;
	}
	else
	{
		m_inspect->threshold = m_Inspection_Params.threashold;
	}

	m_inspect->Pinhole_Island();

	// --- Open_Short_2
	int cnt_OpenShort2 = 0;
	if (m_Inspection_Params.inspect_Short2)
	{
		cnt_OpenShort2 = m_inspect->OpenShort2();
	}
	else // why was it commented??
	{
		m_inspect->OpenShort();
	}

	m_inspect->Build_IRB();

	if (m_Inspection_Params.inspect_LowGary)
	{
		GenEmptyObj(&m_inspect->m_ho_RegionsLG);
		m_inspect->Low_Gray(m_inspect->threshold);
	}

	if (m_Inspection_Params.inspect_FWM)
	{
		//m_Inspection_Params.sz1 = 75;
		//m_Inspection_Params.sz2 = 75;			// Running median sizes for FWM and FWMS

		m_inspect->Proc_FWM();
	}

	if (m_Inspection_Params.inspect_FWMS)
	{
		//m_Inspection_Params.sz1 = 45;
		//m_Inspection_Params.sz2 = 45;			// Running median sizes for FWM and FWMS

		m_inspect->Proc_FWMS();
	}

	HTuple hv_nMB, hv_nMBS, hv_nSPS, hv_nSP, hv_nLG, hv_nOp, hv_c, hv_r, hv_a;

	CountObj(m_inspect->m_ho_RegionsMB, &hv_nMB);
	CountObj(m_inspect->m_ho_RegionsOp, &hv_nOp);
	CountObj(m_inspect->m_ho_RegionsMBS, &hv_nMBS);
	CountObj(m_inspect->m_ho_RegionsSPS, &hv_nSPS);
	CountObj(m_inspect->m_ho_RegionsLG, &hv_nLG);

	AreaCenter(m_inspect->m_ho_RegionsSPS, &hv_a, &hv_r, &hv_c);
	if (hv_a == 0) hv_nSPS = 0;
	AreaCenter(m_inspect->m_ho_RegionsMB, &hv_a, &hv_r, &hv_c);
	if (hv_a == 0) hv_nMB = 0;
	AreaCenter(m_inspect->m_ho_RegionsMBS, &hv_a, &hv_r, &hv_c);
	if (hv_a == 0) hv_nMBS = 0;

	CountObj(m_inspect->m_ho_RegionsSP, &hv_nSP);
	AreaCenter(m_inspect->m_ho_RegionsSP, &hv_a, &hv_r, &hv_c);
	if (hv_a == 0)
		hv_nSP = 0;


	return hv_nMB + hv_nSP + hv_nMBS + hv_nSPS + hv_nLG + cnt_OpenShort2 > 0;
}

bool Inc_Bath_Pos()
{
	std::unique_lock<std::mutex> lock(m_mutex_Bath);
	m_Bath_Pos += 1;
	return (m_Bath_Pos >= m_Bath_Image_Count) || m_Bath_Stop;
}

void CInspectTestingDlg::Bath_Insert_Defect_List()
{
	HTuple hv_nMB, hv_nSP, hv_nMBS, hv_nSPS, hv_nLG, hv_nOp, hv_c, hv_r, hv_a;

	CountObj(m_inspect->m_ho_RegionsMB, &hv_nMB);
	AreaCenter(m_inspect->m_ho_RegionsMB, &hv_a, &hv_r, &hv_c);
	if (hv_a == 0)
		hv_nMB = 0;

	CountObj(m_inspect->m_ho_RegionsSP, &hv_nSP);
	AreaCenter(m_inspect->m_ho_RegionsSP, &hv_a, &hv_r, &hv_c);
	if (hv_a == 0)
		hv_nSP = 0;

	CountObj(m_inspect->m_ho_RegionsMBS, &hv_nMBS);
	AreaCenter(m_inspect->m_ho_RegionsMBS, &hv_a, &hv_r, &hv_c);
	if (hv_a == 0)
		hv_nMBS = 0;
	CountObj(m_inspect->m_ho_RegionsSPS, &hv_nSPS);
	AreaCenter(m_inspect->m_ho_RegionsSPS, &hv_a, &hv_r, &hv_c);
	if (hv_a == 0)
		hv_nSPS = 0;

	//DispObject(m_inspect->m_ho_RegionsLG, "red");
	CountObj(m_inspect->m_ho_RegionsLG, &hv_nLG);

	// --- Open_Sjort_2
	int cnt_open = 0, cnt_short = 0;
	if (m_Check_Open_Short_2)
	{
		cnt_open = m_inspect->op2->params.count_defects_open;
		cnt_short = m_inspect->op2->params.count_defects_short;
	}
	else
	{
		CountObj(m_inspect->m_ho_RegionsOp, &hv_nOp);
		cnt_open = hv_nOp;
	}


	int num = m_Bath_Defects_List.GetItemCount();
	m_Bath_Defects_List.InsertItem(num, m_image_name);
	m_Bath_Defects_List.SetItem(num, 1, LVIF_TEXT, hv_nMB.ToString(), NULL, NULL, NULL, NULL);
	m_Bath_Defects_List.SetItem(num, 2, LVIF_TEXT, hv_nSP.ToString(), NULL, NULL, NULL, NULL);
	m_Bath_Defects_List.SetItem(num, 3, LVIF_TEXT, hv_nMBS.ToString(), NULL, NULL, NULL, NULL);
	m_Bath_Defects_List.SetItem(num, 4, LVIF_TEXT, hv_nSPS.ToString(), NULL, NULL, NULL, NULL);
	m_Bath_Defects_List.SetItem(num, 5, LVIF_TEXT, to_string(cnt_open).c_str(), NULL, NULL, NULL, NULL);
	m_Bath_Defects_List.SetItem(num, 6, LVIF_TEXT, to_string(cnt_short).c_str(), NULL, NULL, NULL, NULL);
	m_Bath_Defects_List.SetItem(num, 7, LVIF_TEXT, hv_nLG.ToString(), NULL, NULL, NULL, NULL);

	UpdateData(TRUE);
	m_Ed_Defect_Count.Format("%d", m_Bath_Defects_List.GetItemCount());
	UpdateData(FALSE);

	// -----------------------------------
	// Сохраним дефекты во временную папку
	int iFrameNo, iUnitNo;
	sscanf(m_image_name, "RawImage_%d_%d.bmp", &iFrameNo, &iUnitNo);
	CString file_name;
	file_name.Format("%s\\Im%d_%d.tif", INSPECT_TMP_FOLDER, iFrameNo, iUnitNo);
	WriteImage(m_inspect->m_local_Im, "tiff", 0, (HTuple)file_name);

	CString file_name_obj;
	file_name.Format("Im%d_%d", iFrameNo, iUnitNo);

	file_name_obj.Format("%s\\%s_RegionsMB", INSPECT_TMP_FOLDER, file_name);
	WriteObject(m_inspect->m_ho_RegionsMB, (HTuple)file_name_obj);

	file_name_obj.Format("%s\\%s_RegionsMBS", INSPECT_TMP_FOLDER, file_name);
	WriteObject(m_inspect->m_ho_RegionsMBS, (HTuple)file_name_obj);

	file_name_obj.Format("%s\\%s_RegionsSPS", INSPECT_TMP_FOLDER, file_name);
	WriteObject(m_inspect->m_ho_RegionsSPS, (HTuple)file_name_obj);

	file_name_obj.Format("%s\\%s_RegionsLG", INSPECT_TMP_FOLDER, file_name);
	WriteObject(m_inspect->m_ho_RegionsLG, (HTuple)file_name_obj);

	if (m_Check_Open_Short_2)
	{
		file_name_obj.Format("%s\\%s_RegionsOpen2", INSPECT_TMP_FOLDER, file_name);
		WriteObject(m_inspect->op2->ho_Open, (HTuple)file_name_obj);
		file_name_obj.Format("%s\\%s_RegionsShort2", INSPECT_TMP_FOLDER, file_name);
		WriteObject(m_inspect->op2->ho_Short, (HTuple)file_name_obj);

	}
	//else
	{
		file_name_obj.Format("%s\\%s_RegionsOp", INSPECT_TMP_FOLDER, file_name);
		WriteObject(m_inspect->m_ho_RegionsOp, (HTuple)file_name_obj);

	}

}

DWORD WINAPI Thread_Bath(LPVOID lpParam)
{
	bool *Check_If_Defect = (bool*)lpParam;

	while (true)
	{

		bool defects = Inspect_Units();

		SendMessage(m_HWND, WM_BATH_INSPECT_EVENTS, 0, 0);

		if (defects)
		{

			SendMessage(m_HWND, WM_BATH_INSPECT_EVENTS_ADD_DEFECT, 0, 0);


			if (*Check_If_Defect)
			{
				SendMessage(m_HWND, WM_BATH_INSPECT_EVENTS_DEFECT, 0, 0);
				break;
			}
		}


		if (Inc_Bath_Pos())
		{
			break;
		}


	}
	return 0;
}

void CInspectTestingDlg::Bath_Update()
{
	UpdateData(TRUE);
	m_Inspect_Progress.SetPos(m_Bath_Pos);
	m_Edit_Inspection_Pos.Format("%d", m_Bath_Pos);

	GetDlgItem(IDC_BUTTON_BATH_PREVIOUS)->EnableWindow(m_Bath_Pos > 0);
	GetDlgItem(IDC_BUTTON_BATH_NEXT)->EnableWindow(m_Bath_Pos < m_Bath_Image_Count);

	m_Ed_Bath_Unit = m_image_name;

	UpdateData(FALSE);
}


void CInspectTestingDlg::DispObjImage(HObject obj, HTuple color)
{
	UpdateData(TRUE);
	if (m_Check_Dip_Defects)
	{
		m_Window.SetLineWidth(3);
		m_Window.SetDraw("margin");

		m_Window.SetColor(color);
		m_Window.DispObj(obj);
	}
	UpdateData(FALSE);
}

int CInspectTestingDlg::Disp_Defects()
{

	HTuple hv_nMB, hv_nSP, hv_nMBS, hv_nSPS, hv_nLG, hv_nOp, hv_nPh, hv_c, hv_r, hv_a;
	HObject ho_RegionsMBS = m_inspect->m_ho_RegionsMBS;
	HObject ho_RegionsSPS = m_inspect->m_ho_RegionsSPS;
	HObject ho_RegionsMB = m_inspect->m_ho_RegionsMB;
	HObject ho_RegionsSP = m_inspect->m_ho_RegionsSP;
	HObject ho_RegionsLG = m_inspect->m_ho_RegionsLG;
	HObject ho_RegionsPH = m_inspect->m_ho_RegionPH;
	
	HObject ho_RegionsShi;
	HObject ho_RegionsOp;
	if (m_Inspection_Params.inspect_Short2)
	{
		ho_RegionsOp = m_inspect->op2->ho_Open;
		ho_RegionsShi = m_inspect->op2->ho_Short;
	}
	else
	{
		ho_RegionsOp = m_inspect->m_ho_RegionsOp;
		ho_RegionsShi = m_inspect->m_ho_RegionsShi;
	}


	//DispObj(m_inspect->m_local_Im, HDevWindowStack::GetActive());

	UpdateData(TRUE);
	if (m_Check_Union_Defects)
	{
		int test = 0;
		if (test != 0)
		{
			WriteObject(ho_RegionsShi, "C:\\Temp\\\\ho_RegionsShi");
			WriteObject(ho_RegionsOp, "C:\\Temp\\ho_RegionsOp");
			WriteObject(ho_RegionsSPS, "C:\\Temp\\ho_RegionsSPS");
			WriteObject(ho_RegionsLG, "C:\\Temp\\ho_RegionsLG");
			WriteObject(ho_RegionsMBS, "C:\\Temp\\ho_RegionsMBS");
			WriteObject(ho_RegionsMB, "C:\\Temp\\ho_RegionsMB");
			WriteObject(ho_RegionsSP, "C:\\Temp\\ho_RegionsSP");

		}


		Union_Defects(ho_RegionsShi, ho_RegionsOp, &ho_RegionsShi, &ho_RegionsOp, m_Inspection_Params.union_defect_dist);
		Union_Defects(ho_RegionsShi, ho_RegionsSPS, &ho_RegionsShi, &ho_RegionsSPS, m_Inspection_Params.union_defect_dist);
		Union_Defects(ho_RegionsShi, ho_RegionsLG, &ho_RegionsShi, &ho_RegionsLG, m_Inspection_Params.union_defect_dist);
		Union_Defects(ho_RegionsShi, ho_RegionsMBS, &ho_RegionsShi, &ho_RegionsMBS, m_Inspection_Params.union_defect_dist);
		Union_Defects(ho_RegionsShi, ho_RegionsMB, &ho_RegionsShi, &ho_RegionsMB, m_Inspection_Params.union_defect_dist);
		Union_Defects(ho_RegionsShi, ho_RegionsSP, &ho_RegionsShi, &ho_RegionsSP, m_Inspection_Params.union_defect_dist);

		Union_Defects(ho_RegionsOp, ho_RegionsSPS, &ho_RegionsOp, &ho_RegionsSPS, m_Inspection_Params.union_defect_dist);
		Union_Defects(ho_RegionsOp, ho_RegionsLG, &ho_RegionsOp, &ho_RegionsLG, m_Inspection_Params.union_defect_dist);
		Union_Defects(ho_RegionsOp, ho_RegionsMBS, &ho_RegionsOp, &ho_RegionsMBS, m_Inspection_Params.union_defect_dist);
		Union_Defects(ho_RegionsOp, ho_RegionsMB, &ho_RegionsOp, &ho_RegionsMB, m_Inspection_Params.union_defect_dist);
		Union_Defects(ho_RegionsOp, ho_RegionsSP, &ho_RegionsOp, &ho_RegionsSP, m_Inspection_Params.union_defect_dist);

		Union_Defects(ho_RegionsSPS, ho_RegionsLG, &ho_RegionsSPS, &ho_RegionsLG, m_Inspection_Params.union_defect_dist);
		Union_Defects(ho_RegionsSPS, ho_RegionsMBS, &ho_RegionsSPS, &ho_RegionsMBS, m_Inspection_Params.union_defect_dist);
		Union_Defects(ho_RegionsSPS, ho_RegionsMB, &ho_RegionsSPS, &ho_RegionsMB, m_Inspection_Params.union_defect_dist);
		Union_Defects(ho_RegionsSPS, ho_RegionsSP, &ho_RegionsSPS, &ho_RegionsSP, m_Inspection_Params.union_defect_dist);

		Union_Defects(ho_RegionsMBS, ho_RegionsLG, &ho_RegionsMBS, &ho_RegionsLG, m_Inspection_Params.union_defect_dist);
		Union_Defects(ho_RegionsMBS, ho_RegionsMB, &ho_RegionsMBS, &ho_RegionsMB, m_Inspection_Params.union_defect_dist);
		Union_Defects(ho_RegionsMBS, ho_RegionsSP, &ho_RegionsMBS, &ho_RegionsSP, m_Inspection_Params.union_defect_dist);

		Union_Defects(ho_RegionsLG, ho_RegionsMB, &ho_RegionsLG, &ho_RegionsMB, m_Inspection_Params.union_defect_dist);
		Union_Defects(ho_RegionsLG, ho_RegionsSP, &ho_RegionsLG, &ho_RegionsSP, m_Inspection_Params.union_defect_dist);

		Union_Defects(ho_RegionsMB, ho_RegionsSP, &ho_RegionsMB, &ho_RegionsSP, m_Inspection_Params.union_defect_dist);

		AreaCenter(ho_RegionsMB, &hv_r, &hv_c,  &hv_a);

		Connection(ho_RegionsOp, &ho_RegionsOp);
		Connection(ho_RegionsMBS, &ho_RegionsMBS);
		Connection(ho_RegionsSPS, &ho_RegionsSPS);
		Connection(ho_RegionsMB, &ho_RegionsMB);
		Connection(ho_RegionsSP, &ho_RegionsSP);
		Connection(ho_RegionsLG, &ho_RegionsLG);

	}
	UpdateData(FALSE);


	
	DispObjImage(ho_RegionsMBS, "orange");
	DispObjImage(ho_RegionsSPS, "dark orchid");
	DispObject(ho_RegionsSP, "magenta");
	DispObjImage(ho_RegionsMB, "dark turquoise");
	DispObjImage(ho_RegionsOp, "blue");
	DispObjImage(ho_RegionsShi, "red");
	CountObj(ho_RegionsMB, &hv_nMB);
	CountObj(ho_RegionsSP, &hv_nSP);
	CountObj(ho_RegionsOp, &hv_nOp);
	CountObj(ho_RegionsPH, &hv_nPh);
	AreaCenter(m_inspect->m_ho_RegionPH, &hv_a, &hv_r, &hv_c);
	if (hv_a == 0)
		hv_nPh = 0;
	
	AreaCenter(ho_RegionsMB, &hv_a, &hv_r, &hv_c);
	if (hv_a == 0)
		hv_nMB = 0;
	CountObj(ho_RegionsMBS, &hv_nMBS);
	AreaCenter(ho_RegionsMBS, &hv_a, &hv_r, &hv_c);
	if (hv_a == 0)
		hv_nMBS = 0;
	CountObj(ho_RegionsSPS, &hv_nSPS);
	AreaCenter(ho_RegionsSPS, &hv_a, &hv_r, &hv_c);
	if (hv_a == 0)
		hv_nSPS = 0;

	DispObjImage(ho_RegionsLG, "green");
	CountObj(ho_RegionsLG, &hv_nLG);
	//AreaCenter(ho_RegionsLG, &hv_a, &hv_r, &hv_c);
	//if (hv_a == 0)
	//	hv_nLG = 0;

	CString HColor;
	int ret = hv_nMB + hv_nMBS + hv_nSPS + hv_nSP + hv_nOp + hv_nLG + hv_nPh;
	if (ret == 0)
	{
		HColor = "green";
	}
	else
	{
		HColor = "red";
	}

	DispObjImage(m_inspect->m_ho_RegionsDT_S, "red");
	DispObjImage(m_inspect->m_ho_RegionsDT_P, "red");

	HTuple Txt = "MB=" + (hv_nMB)+" SP=" + (hv_nSP)+" MBS=" + (hv_nMBS)+" SPS=" + (hv_nSPS)+" Open=" + (hv_nOp)+" LG=" + (hv_nLG)+" Ph=" + (hv_nPh);
	CString str_Txt = Txt.ToString();
	m_Window.DispText(str_Txt, "window", 10, 10, HColor, HTuple(), HTuple());

	DispObjImage(m_Cross_Point, "red");

	return ret;
}
LRESULT CInspectTestingDlg::OnBathInspectEvent(WPARAM, LPARAM)
{
	Bath_Update();
	return 0;
}
LRESULT CInspectTestingDlg::OnBathInspectEventDefect(WPARAM, LPARAM)
{
	GetDlgItem(IDC_BUTTON_BATH_START)->EnableWindow(true);
	GetDlgItem(IDC_BUTTON_BATH_STOP)->EnableWindow(false);

	GetDlgItem(IDC_CHECK_DEFECT_FOUND_STOP)->EnableWindow(true);
	GetDlgItem(IDC_CHECK_AUTO_THR_BATH)->EnableWindow(true);
	GetDlgItem(IDC_CHECK_INSPECT_LG)->EnableWindow(true);
	GetDlgItem(IDC_CHECK_INSPECT_FWM)->EnableWindow(true);
	GetDlgItem(IDC_CHECK_INSPECT_FWMS)->EnableWindow(true);

	Disp_Defects();

	return 0;
}
LRESULT CInspectTestingDlg::OnBathInspectEventADDDefect(WPARAM, LPARAM)
{
	Bath_Insert_Defect_List();
	return 0;
}

void CInspectTestingDlg::Bath_Enable()
{
	//GetDlgItem(IDC_BUTTON_BATH_INIT)->EnableWindow(true);
	GetDlgItem(IDC_BUTTON_BATH_START)->EnableWindow(true);
	GetDlgItem(IDC_BUTTON_BATH_STOP)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON_BATH_PREVIOUS)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON_BATH_NEXT)->EnableWindow(false);
	GetDlgItem(IDC_CHECK_DEFECT_FOUND_STOP)->EnableWindow(true);
	GetDlgItem(IDC_CHECK_AUTO_THR_BATH)->EnableWindow(true);
	GetDlgItem(IDC_EDIT_INSPECTION_UNIT)->EnableWindow(true);

}

void CInspectTestingDlg::OnBnClickedButtonBathInit()
{

	if (!m_init)
	{
		m_inspect->Init();
		m_init = 1;
	}

	CString str_format;
	str_format.Format("%s\\RawImage_*.bmp", m_Image_Path);
	int count_files = Folder_Count_Files(str_format);

	CString sMsg;
	sMsg.Format("%d image files are found. Proceed batch inspection?", count_files);
	if (AfxMessageBox(sMsg, MB_YESNO | MB_ICONQUESTION) != IDYES)
	{
		return;
	}

	m_Image_Path.Format("%s", m_Image_Path);
	inspect_utils::GetAllFilesNames(m_Image_Path, m_inspect_folder_files);

	m_Bath_Image_Count = m_inspect_folder_files.GetCount();
	m_Bath_Stop_Pos = 0;
	if (m_Bath_Image_Count > 0)
	{

		GetDlgItem(IDC_BUTTON_BATH_START)->EnableWindow(true);
		GetDlgItem(IDC_BUTTON_BATH_STOP)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_BATH_PREVIOUS)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_BATH_NEXT)->EnableWindow(true);
		GetDlgItem(IDC_CHECK_DEFECT_FOUND_STOP)->EnableWindow(true);
		GetDlgItem(IDC_CHECK_AUTO_THR_BATH)->EnableWindow(true);
		GetDlgItem(IDC_EDIT_INSPECTION_UNIT)->EnableWindow(true);

		GetDlgItem(IDC_CHECK_INSPECT_LG)->EnableWindow(true);
		GetDlgItem(IDC_CHECK_INSPECT_FWM)->EnableWindow(true);
		GetDlgItem(IDC_CHECK_INSPECT_FWMS)->EnableWindow(true);

		m_Bath_Stop = true;

		UpdateData(TRUE);

		m_Bath_Pos = 0;
		m_Inspect_Progress.SetRange(0, m_Bath_Image_Count);
		m_Inspect_Progress.SetPos(m_Bath_Pos);
		m_Ed_Bath_Unit = "";

		m_Edit_Inspection_Pos.Format("%d", m_Bath_Pos);
		m_Bath_Count.Format("%d", m_Bath_Image_Count);


		UpdateData(FALSE);

		m_Inspection_Params.inspect_Auto_Thr = m_Auto_Threshold;
		m_Inspection_Params.inspect_LowGary = m_Check_Inspect_LG;
		m_Inspection_Params.inspect_FWM = m_Check_Inspect_FWM;
		m_Inspection_Params.inspect_FWMS = m_Check_Inspect_FWMS;

		m_Inspection_Params.inspect_Short2 = m_Check_Open_Short_2;



	}

	m_Bath_Pos = 0;

	Clear_Defects();

	Delete_All_Folders(INSPECT_TMP_FOLDER);
	ConfirmFolder(INSPECT_TMP_FOLDER);


}



void CInspectTestingDlg::Bath_Set_Inspect_Unit()
{
	Inspect_Units();
	this->SendMessage(WM_BATH_INSPECT_EVENTS, 0, 0);

	GenEmptyObj(&m_Cross_Point);

	m_ho_Im = m_inspect_param.image;
	//Disp_Image(m_ho_Im);
	Disp_Fit_Image(m_inspect->m_local_Im);


	int cnt = Disp_Defects();
	if (cnt)
	{
		PlaySound("..\\Inspect_Testing\\res\\ringout.wav", NULL, SND_FILENAME | SND_ASYNC);
		//Beep(1000, 500);
	}


	UpdateData(TRUE);
	m_Image_Name = m_Image_Path + "\\" + m_image_name;
	UpdateData(FALSE);

}

void CInspectTestingDlg::OnBnClickedButtonBathNext()
{
	Bath_Set_Inspect_Unit();
	m_Bath_Pos += 1;
}
void CInspectTestingDlg::OnBnClickedButtonBathPrevious()
{
	m_Bath_Pos -= 1;
	Bath_Set_Inspect_Unit();
}


void CInspectTestingDlg::OnBnClickedButtonBathStart()
{
	//if (m_Bath_Stop == false)
	//{
	//	m_Bath_Pos += 1;
	//}
	//else
	//{
	//	m_Bath_Stop = false;
	//}

	//m_Bath_Pos = m_Bath_Defects_List.GetItemCount();
	m_Bath_Stop = false;
	if(m_Bath_Pos>0)
		m_Bath_Pos=m_Bath_Stop_Pos+1;

	if (m_Bath_From != 0)
		m_Bath_Pos = m_Bath_From;
	if (m_Bath_To != 0)
		m_Bath_Image_Count = m_Bath_To;

	if (m_Bath_Pos >= m_Bath_Image_Count)
	{
		return;
	}

	UpdateData(TRUE);
	m_Alignment_Type = m_Check_Alig2 ? 2 : 1;

	hThread_Bath = CreateThread(
		NULL,									// default security attributes
		0,										// use default stack size  
		&Thread_Bath,							// thread function name
		&m_Check_If_Defect,						// argument to thread function 
		0,										// use default creation flags 
		&dwThreadId_Bath);						// returns the thread identifier 

	GetDlgItem(IDC_BUTTON_BATH_START)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON_BATH_STOP)->EnableWindow(true);

	GetDlgItem(IDC_CHECK_DEFECT_FOUND_STOP)->EnableWindow(false);
	GetDlgItem(IDC_CHECK_AUTO_THR_BATH)->EnableWindow(false);
	GetDlgItem(IDC_CHECK_INSPECT_LG)->EnableWindow(false);
	GetDlgItem(IDC_CHECK_INSPECT_FWM)->EnableWindow(false);
	GetDlgItem(IDC_CHECK_INSPECT_FWMS)->EnableWindow(false);

	UpdateData(FALSE);
}


void CInspectTestingDlg::OnBnClickedButtonBathStop()
{
	GetDlgItem(IDC_BUTTON_BATH_START)->EnableWindow(true);
	GetDlgItem(IDC_BUTTON_BATH_STOP)->EnableWindow(false);

	GetDlgItem(IDC_CHECK_DEFECT_FOUND_STOP)->EnableWindow(true);
	GetDlgItem(IDC_CHECK_AUTO_THR_BATH)->EnableWindow(true);
	GetDlgItem(IDC_CHECK_INSPECT_LG)->EnableWindow(true);
	GetDlgItem(IDC_CHECK_INSPECT_FWM)->EnableWindow(true);
	GetDlgItem(IDC_CHECK_INSPECT_FWMS)->EnableWindow(true);
	std::unique_lock<std::mutex> lock(m_mutex_Bath);
	m_Bath_Stop = true;
	m_Bath_Stop_Pos= m_Bath_Pos;
}

void CInspectTestingDlg::OnBnClickedButtonBathClear()
{
	UpdateData(TRUE);
	m_Bath_Defects_List.DeleteAllItems();
	m_Ed_Defect_Count.Format("%d", m_Bath_Defects_List.GetItemCount());
	UpdateData(FALSE);
}

int FindString(const CStringArray& arr, LPCTSTR pszString)
{
	const int nSize = arr.GetSize();
	for (int nIndex = 0; nIndex < nSize; nIndex++)
	{
		if (!arr[nIndex].Compare(pszString))
		{
			return nIndex;
		}
	}
	return -1;
}


void CInspectTestingDlg::Add_List_Units_Defects(CString defect_name, HObject ho_Defects)
{
	HObject ho_Defects_C, ho_Sel_Defect;
	HTuple hv_Number_Defects, hv_Area, hv_Row, hv_Col;

	Connection(ho_Defects, &ho_Defects_C);
	CountObj(ho_Defects_C, &hv_Number_Defects);
	Hlong cv_Number_Defects = hv_Number_Defects.ToLArr()[0];

	for (int cv_Idx = 1; cv_Idx <= cv_Number_Defects; cv_Idx++)
	{
		SelectObj(ho_Defects_C, &ho_Sel_Defect, cv_Idx);
		AreaCenter(ho_Sel_Defect, &hv_Area, &hv_Row, &hv_Col);

		TupleInt(hv_Row, &hv_Row);
		TupleInt(hv_Col, &hv_Col);

		int num = m_List_Unit_Defects.GetItemCount();
		m_List_Unit_Defects.InsertItem(num, defect_name);
		m_List_Unit_Defects.SetItem(num, 1, LVIF_TEXT, to_string(cv_Idx).c_str(), NULL, NULL, NULL, NULL);
		m_List_Unit_Defects.SetItem(num, 2, LVIF_TEXT, hv_Col.ToString(), NULL, NULL, NULL, NULL);
		m_List_Unit_Defects.SetItem(num, 3, LVIF_TEXT, hv_Row.ToString(), NULL, NULL, NULL, NULL);


	}

}

void CInspectTestingDlg::Insert_List_Units_Defects()
{
	m_List_Unit_Defects.DeleteAllItems();


	// Выведем все дефекты в список
	Add_List_Units_Defects("MBS", m_inspect->m_ho_RegionsMBS);
	Add_List_Units_Defects("SPS", m_inspect->m_ho_RegionsSPS);
	Add_List_Units_Defects("MB", m_inspect->m_ho_RegionsMB);
	Add_List_Units_Defects("SP", m_inspect->m_ho_RegionsSP);
	Add_List_Units_Defects("LG", m_inspect->m_ho_RegionsLG);
	Add_List_Units_Defects("Open2", m_inspect->op2->ho_Open);
	Add_List_Units_Defects("Short2", m_inspect->op2->ho_Short);

}

void CInspectTestingDlg::OnLvnItemchangedListUnitDefects(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	int nSelectItem = pNMLV->iItem;

	m_Defect_Attribute.DeleteAllItems();

	CString defect_name = m_List_Unit_Defects.GetItemText(nSelectItem, 0);
	int defect_pos = atoi(m_List_Unit_Defects.GetItemText(nSelectItem, 1));

	int posX = atoi(m_List_Unit_Defects.GetItemText(nSelectItem, 2));
	int posY = atoi(m_List_Unit_Defects.GetItemText(nSelectItem, 3));
	Zoom_Point(posX, posY);
	SaveDefectImage(posX, posY, 128, defect_name);

	if (defect_name == "MBS")
	{
		HTuple prs = m_inspect->m_hv_pDefMBSprc[defect_pos - 1];
		m_Defect_Attribute.InsertItem(0, "pDefMBSprc");
		m_Defect_Attribute.SetItem(0, 1, LVIF_TEXT, prs.ToString(), NULL, NULL, NULL, NULL);

		HTuple def = m_inspect->m_hv_pDefMBS[defect_pos - 1];
		m_Defect_Attribute.InsertItem(1, "pDefMBS");
		m_Defect_Attribute.SetItem(1, 1, LVIF_TEXT, def.ToString(), NULL, NULL, NULL, NULL);
	}
	else if (defect_name == "SPS")
	{
		HTuple prs = m_inspect->m_hv_pDefSPSprc[defect_pos - 1];
		m_Defect_Attribute.InsertItem(0, "pDefSPSprc");
		m_Defect_Attribute.SetItem(0, 1, LVIF_TEXT, prs.ToString(), NULL, NULL, NULL, NULL);

		HTuple def = m_inspect->m_hv_pDefSPS[defect_pos - 1];
		m_Defect_Attribute.InsertItem(1, "pDefSPS");
		m_Defect_Attribute.SetItem(1, 1, LVIF_TEXT, def.ToString(), NULL, NULL, NULL, NULL);
	}
	else if (defect_name == "MB")
	{
		HTuple prs = m_inspect->m_hv_pDefMBprc[defect_pos - 1];
		m_Defect_Attribute.InsertItem(0, "pDefMBprc");
		m_Defect_Attribute.SetItem(0, 1, LVIF_TEXT, prs.ToString(), NULL, NULL, NULL, NULL);

		HTuple def = m_inspect->m_hv_pDefMB[defect_pos - 1];
		m_Defect_Attribute.InsertItem(1, "pDefMB");
		m_Defect_Attribute.SetItem(1, 1, LVIF_TEXT, def.ToString(), NULL, NULL, NULL, NULL);
	}
	else if (defect_name == "SP")
	{
		HTuple prs = m_inspect->m_hv_pDefSPprc[defect_pos - 1];
		m_Defect_Attribute.InsertItem(0, "pDefSPprc");
		m_Defect_Attribute.SetItem(0, 1, LVIF_TEXT, prs.ToString(), NULL, NULL, NULL, NULL);

		HTuple def = m_inspect->m_hv_pDefSP[defect_pos - 1];
		m_Defect_Attribute.InsertItem(1, "pDefSP");
		m_Defect_Attribute.SetItem(1, 1, LVIF_TEXT, def.ToString(), NULL, NULL, NULL, NULL);
	}
	else if (defect_name == "LG")
	{
		HTuple prs_min = m_inspect->m_hv_PrcM_Min[defect_pos - 1];
		m_Defect_Attribute.InsertItem(0, "Prc_Min");
		m_Defect_Attribute.SetItem(0, 1, LVIF_TEXT, prs_min.ToString(), NULL, NULL, NULL, NULL);

		HTuple prs_max = m_inspect->m_hv_PrcM_Max[defect_pos - 1];
		m_Defect_Attribute.InsertItem(1, "Prc_Max");
		m_Defect_Attribute.SetItem(1, 1, LVIF_TEXT, prs_max.ToString(), NULL, NULL, NULL, NULL);

		HTuple prs_median = m_inspect->m_hv_PrcM_Median[defect_pos - 1];
		m_Defect_Attribute.InsertItem(2, "Prc_Median");
		m_Defect_Attribute.SetItem(2, 1, LVIF_TEXT, prs_median.ToString(), NULL, NULL, NULL, NULL);
	}
	else if (defect_name == "Open2" || defect_name == "Short2")
	{
		HTuple hv_Gray_Min, hv_Gray_Max, hv_Gray_Median;

		HObject ho_Reg_Defects, ho_Sel_Defect;
		ho_Reg_Defects = defect_name == "Open2" ? m_inspect->op2->ho_Open : m_inspect->op2->ho_Short;
		SelectObj(ho_Reg_Defects, &ho_Sel_Defect, defect_pos);

		m_inspect->op2->Get_Defect_Attribute(
			m_inspect->m_local_Im,
			ho_Sel_Defect,
			hv_Gray_Min, hv_Gray_Max, hv_Gray_Median);

		m_Defect_Attribute.InsertItem(0, "Gray_Min");
		m_Defect_Attribute.SetItem(0, 1, LVIF_TEXT, hv_Gray_Min.ToString(), NULL, NULL, NULL, NULL);

		m_Defect_Attribute.InsertItem(1, "Gray_Max");
		m_Defect_Attribute.SetItem(1, 1, LVIF_TEXT, hv_Gray_Max.ToString(), NULL, NULL, NULL, NULL);

		m_Defect_Attribute.InsertItem(2, "Gray_Median");
		m_Defect_Attribute.SetItem(2, 1, LVIF_TEXT, hv_Gray_Median.ToString(), NULL, NULL, NULL, NULL);
	}

	*pResult = 0;
}

void CInspectTestingDlg::OnLvnItemchangedListDefects(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	if (pNMLV->uNewState & LVIS_SELECTED)
	{
		int nSelectItem = pNMLV->iItem;
		CString unit_name = m_Bath_Defects_List.GetItemText(nSelectItem, 0);
		UpdateData(TRUE);
		m_Image_Name = m_Image_Path + "\\" + unit_name;

		int idx = FindString(m_inspect_folder_files, unit_name);
		if (idx != -1)
		{
			m_Bath_Pos = idx;
			Bath_Set_Inspect_Unit();

		}

		UpdateData(FALSE);
	}

	Insert_List_Units_Defects();


	*pResult = 0;
}




void CInspectTestingDlg::OnBnClickedCheckInspectLg()
{
	CButton *m_ctlCheck = (CButton*)GetDlgItem(IDC_CHECK_INSPECT_LG);
	m_Inspection_Params.inspect_LowGary = m_ctlCheck->GetCheck();
}


void CInspectTestingDlg::OnBnClickedCheckInspectFwm()
{
	CButton *m_ctlCheck = (CButton*)GetDlgItem(IDC_CHECK_INSPECT_FWM);
	m_Inspection_Params.inspect_FWM = m_ctlCheck->GetCheck();
}


void CInspectTestingDlg::OnBnClickedCheckInspectFwms()
{
	CButton *m_ctlCheck = (CButton*)GetDlgItem(IDC_CHECK_INSPECT_FWMS);
	m_Inspection_Params.inspect_FWMS = m_ctlCheck->GetCheck();
}








void CInspectTestingDlg::OnBnClickedCheckAutoThrBath()
{
	CButton *m_ctlCheck = (CButton*)GetDlgItem(IDC_CHECK_AUTO_THR_BATH);
	m_Inspection_Params.inspect_Auto_Thr = m_ctlCheck->GetCheck();

}

void CInspectTestingDlg::OnBnClickedButton5()
{
	static char BASED_CODE szFilter[] = "CSV (*.csv)|*.csv||)";
	CFileDialog	fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter);
	CString parth_str;
	GetRegString("m_Folder_Save", &parth_str, "C:\\TmpIm");
	//fileDlg.m_ofn.lpstrInitialDir = "C:\\TmpIm";
	fileDlg.m_ofn.lpstrInitialDir = parth_str;

	if (fileDlg.DoModal() == IDOK)
	{
		CString image_name = fileDlg.GetPathName();
		CString image_file_name = fileDlg.GetFileName();

		fs::path path = fs::path((LPCTSTR)fileDlg.GetPathName()).parent_path();
		parth_str = Path2CString(path);
		Delete_All_Folders(parth_str);

		SetRegString("m_Folder_Save", parth_str);

		int delay = 10;
		while (Check_Path(parth_str) && delay > 0)
		{
			Sleep(1000);
			delay -= 1;
		}


		//fs::rename(INSPECT_TMP_FOLDER, path);


		const auto copyOptions = 
			  fs::copy_options::update_existing
			| fs::copy_options::recursive
			| fs::copy_options::overwrite_existing
			;
		fs::copy(INSPECT_TMP_FOLDER, path, copyOptions);

		int len = image_name.GetLength();
		if (image_name.Find(".csv") != len - 4)
		{
			image_name += ".csv";
		}

		std::ofstream csvFile(image_name);
		csvFile << "Unit; MB; SP; MBS; SPS; Open; Short; LG\n";
		int num = m_Bath_Defects_List.GetItemCount();
		string str;
		for (int i = 0; i < num; i++)
		{
			csvFile << m_Bath_Defects_List.GetItemText(i, 0) + "; ";
			csvFile << m_Bath_Defects_List.GetItemText(i, 1) + "; ";
			csvFile << m_Bath_Defects_List.GetItemText(i, 2) + "; ";
			csvFile << m_Bath_Defects_List.GetItemText(i, 3) + "; ";
			csvFile << m_Bath_Defects_List.GetItemText(i, 4) + "; ";
			csvFile << m_Bath_Defects_List.GetItemText(i, 5) + "; ";
			csvFile << m_Bath_Defects_List.GetItemText(i, 6) + "; ";
			csvFile << m_Bath_Defects_List.GetItemText(i, 7);
			csvFile << "\n";


		}
		csvFile.close();


	}
}

void CInspectTestingDlg::OnBnClickedButtonRead()
{
	static char BASED_CODE szFilter[] = "CSV (*.csv)|*.csv||)";
	CFileDialog	fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter);
	CString parth_str;
	GetRegString("m_Folder_Read", &parth_str, "C:\\TmpIm");
	fileDlg.m_ofn.lpstrInitialDir = parth_str;

	if (fileDlg.DoModal() == IDOK)
	{
		fs::path path = fs::path((LPCTSTR)fileDlg.GetPathName()).parent_path();
		parth_str = Path2CString(path);
		SetRegString("m_Folder_Read", parth_str);


		CString image_name = fileDlg.GetPathName();
		CString image_file_name = fileDlg.GetFileName();

		std::ifstream csvFile(image_name);
		if (csvFile.is_open() == false)
		{
			return;
		}

		std::string line;
		std::getline(csvFile, line);

		m_Bath_Defects_List.DeleteAllItems();
		int num_item = 0;

		while (std::getline(csvFile, line))
		{
			m_Bath_Defects_List.InsertItem(num_item, m_image_name);


			CString data = line.c_str();
			int Position = 0;
			CString Token;

			int col_pos = 0;
			Token = data.Tokenize("; ", Position);
			while (!Token.IsEmpty())
			{
				if (Token != L"")
				{
					m_Bath_Defects_List.SetItem(num_item, col_pos, LVIF_TEXT, Token, NULL, NULL, NULL, NULL);
					col_pos += 1;
				}
				Token = data.Tokenize("; ", Position);
			}

			num_item += 1;

		}

		UpdateData(TRUE);
		m_Ed_Defect_Count.Format("%d", m_Bath_Defects_List.GetItemCount());
		UpdateData(FALSE);
	}
	OnBnClickedButtonBathInit();
}


void CInspectTestingDlg::OnBnClickedButton7()
{
	//Example_Sub("C:/TmpIm/Im1_3.tif", "C:/TmpIm/ho_Reg_Sub1");
	GetDlgItem(IDC_LIST_DEFECTS)->SetFocus();

	//Invalidate();
}


void CInspectTestingDlg::OnBnClickedButtonSaveDefects()
{


}





void CInspectTestingDlg::OnRButtonDownImage(UINT nFlags, CPoint point)
{
	//// Запомним позицию мышки как текущую позицию картинки
	//CPoint delta = m_MDownPos - m_pos_move;
	//m_HPos.x += delta.x / m_RatioX;
	//m_HPos.y += delta.y / m_RatioY;



	OnRButtonDown(nFlags, point);
}


// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

class CPointF
{
public:
	double x;
	double y;


};


double m_Image_Ratio;
double m_Zoom;
Hlong m_Width, m_Height;
CPointF m_HPos;
double m_RatioX;
double m_RatioY;			// Масштаб картинки относительно размеров окна
double m_img_width;
double m_img_height;

//HalconCpp::HImage m_Disp_Image;

void CInspectTestingDlg::Zoom_Image(double zoom)
{
	POINT pt;
	GetCursorPos(&pt);
	m_Image_Control.ScreenToClient((LPPOINT)&pt);

	CPointF poin1;
	poin1.x = (double)pt.x / m_RatioX * m_Zoom;
	poin1.y = (double)pt.y / m_RatioY * m_Zoom;

	m_Zoom = zoom;

	CPointF poin2;
	poin2.x = (double)pt.x / m_RatioX * m_Zoom;
	poin2.y = (double)pt.y / m_RatioY * m_Zoom;

	m_HPos.x = poin1.x - poin2.x + m_HPos.x;
	m_HPos.y = poin1.y - poin2.y + m_HPos.y;

	//printf("m_Zoom = %f, Point1=%d-%d, Point2=%d-%d \n", m_Zoom, poin1.x, poin1.y, poin2.x, poin2.y);

	// Расчет размера нового
	m_img_width = (double)m_Width * m_Zoom;
	m_img_height = (double)m_Height * m_Zoom;

	m_Window.ClearWindow();
	m_Window.SetPart(m_HPos.y, m_HPos.x, m_HPos.y + m_img_height - 1, m_HPos.x + m_img_width - 1);

	Disp_Image();
}

void CInspectTestingDlg::OnStnDblclickImage()
{
	double zoom;
	if (GetKeyState(VK_CONTROL) < 0)
	{
		zoom = m_Zoom + 0.5 * m_Zoom;
	}
	else
	{
		zoom = m_Zoom - 0.5 * m_Zoom;
	}
	Zoom_Image(zoom);
}

void CInspectTestingDlg::Disp_Image()
{
	// ----------------------------
	HTuple mid = m_Inspection_Params.LGE_mid;
	HTuple margin = m_Inspection_Params.LGE_margin;
	HTuple off = m_Inspection_Params.LGE_off;
	HTuple slope = m_Inspection_Params.LGE_slope;

	HObject image;
	m_inspect->LGEnhance2(m_Disp_Image, &image, mid, margin, slope, off);
	// ----------------------------

	m_Window.DispObj(image);
	Disp_Defects();
}

void CInspectTestingDlg::Disp_Fit_Image(HObject ho_Im)
{
	m_Disp_Image = ho_Im;


	m_Disp_Image.GetImageSize(&m_Width, &m_Height);
	m_Image_Ratio = (float)m_Width / m_Height;

	RECT rect;
	m_Image_Control.GetWindowRect(&rect);

	Hlong nWindowRow, nWindowColumn, nWindowWidth, nWindowHeight, nWindowHeight0;


	nWindowRow = 0;
	nWindowColumn = 0;
	nWindowWidth = rect.right - rect.left;
	nWindowHeight0 = rect.bottom - rect.top;
	nWindowHeight = nWindowWidth / m_Image_Ratio;

	if (nWindowHeight0 < nWindowHeight)
	{
		m_Zoom = (double)nWindowHeight / nWindowHeight0;

	}
	else
	{
		m_Zoom = 1.0;
	}
	m_HPos.x = 0;
	m_HPos.y = 0;
	m_img_width = (double)m_Width * m_Zoom;
	m_img_height = (double)m_Height * m_Zoom;

	m_Window.SetWindowExtents(nWindowRow, nWindowColumn, nWindowWidth, nWindowHeight);
	//HSystem::SetWindowAttr("border_width", HTuple(0));
	//HSystem::SetCheck("~father");
	//m_Window.OpenWindow(
	//	nWindowRow, nWindowColumn,
	//	nWindowWidth, nWindowHeight,
	//	(Hlong)m_Image_HWND, "visible", "");
	//HSystem::SetCheck("father");


	m_HPos.x = 0;
	m_HPos.y = 0;
	m_RatioX = (double)nWindowWidth / m_Width;
	m_RatioY = (double)nWindowHeight / m_Height;

	double m_img_width = (double)m_Width * m_Zoom;
	double m_img_height = (double)m_Height * m_Zoom;

	m_Window.SetPart(m_HPos.x, m_HPos.y, m_HPos.x + m_img_height - 1, m_HPos.y + m_img_width - 1);
	Disp_Image();
	//m_Window.DispObj(m_Disp_Image);


	//float div = 2.0;
	//HTuple w, h, hdiv;
	//GetImageSize(ho_Im, &w, &h);
	//TupleMax2(h / 1000., w / 1900., &hdiv);
	////TupleCeil(hdiv, &hdiv);
	//SetWindowExtents(hv_WindowHandle, 0, 0, w / div, h / div);
	//SetPart(hv_WindowHandle, h / 2, w / 2, h - 1, w - 1);
	//SetPart(hv_WindowHandle, 0, 0, h - 1, w - 1);
	//DispObj(ho_Im, HDevWindowStack::GetActive());
}

void CInspectTestingDlg::OnBnClickedButton1()
{

	m_Disp_Image = m_ho_Im;


	m_Disp_Image.GetImageSize(&m_Width, &m_Height);
	m_Image_Ratio = (float)m_Width / m_Height;

	RECT rect;
	m_Image_Control.GetWindowRect(&rect);

	Hlong nWindowRow, nWindowColumn, nWindowWidth, nWindowHeight;


	nWindowRow = 0;
	nWindowColumn = 0;
	nWindowWidth = rect.right - rect.left;
	nWindowHeight = nWindowWidth / m_Image_Ratio;

	HSystem::SetWindowAttr("border_width", HTuple(0));
	HSystem::SetCheck("~father");
	m_Window.OpenWindow(
		nWindowRow, nWindowColumn,
		nWindowWidth, nWindowHeight,
		(Hlong)m_Image_HWND, "visible", "");
	HSystem::SetCheck("father");


	m_HPos.x = 0;
	m_HPos.y = 0;
	m_RatioX = (double)nWindowWidth / m_Width;
	m_RatioY = (double)nWindowHeight / m_Height;
	m_Zoom = 1.0;
	m_Window.SetPart(m_HPos.x, m_HPos.y, m_HPos.x + m_Height - 1, m_HPos.y + m_Width - 1);
	//m_Window.SetLineWidth(3);
	m_Window.DispObj(m_Disp_Image);


}

void CInspectTestingDlg::Zoom_Point(int x, int y)
{

	GenCrossContourXld(&m_Cross_Point, y, x, 50, 0);
	Disp_Image();


}

void color_string_to_rgb(HTuple hv_Color, HTuple *hv_RGB)
{

	// Local iconic variables
	HObject  ho_Rectangle, ho_Image;

	// Local control variables
	HTuple  hv_WindowHandleBuffer, hv_Exception;

	OpenWindow(0, 0, 1, 1, 0, "buffer", "", &hv_WindowHandleBuffer);
	SetPart(hv_WindowHandleBuffer, 0, 0, -1, -1);
	GenRectangle1(&ho_Rectangle, 0, 0, 0, 0);
	try
	{
		SetColor(hv_WindowHandleBuffer, hv_Color);
	}
	// catch (Exception) 
	catch (HException &HDevExpDefaultException)
	{
		HDevExpDefaultException.ToHTuple(&hv_Exception);
		hv_Exception = "Wrong value of control parameter Color (must be a valid color string)";
		throw HException(hv_Exception);
	}
	DispObj(ho_Rectangle, hv_WindowHandleBuffer);
	DumpWindowImage(&ho_Image, hv_WindowHandleBuffer);
	CloseWindow(hv_WindowHandleBuffer);
	GetGrayval(ho_Image, 0, 0, &(*hv_RGB));
	(*hv_RGB) += ((HTuple(0).Append(0)).Append(0));
	return;
}

void GenCircleOnImage(HObject ho_ImIn, HObject *ho_ImOut, HTuple hv_x, HTuple hv_y, HTuple hv_crad, HTuple hv_color)
{
	HTuple hv_RGB, hv_w, hv_h;
	HObject ho_ImR, ho_ImG, ho_ImB, ho_Circ;

	GetImageSize(ho_ImIn, &hv_w, &hv_h);
	color_string_to_rgb(hv_color, &hv_RGB);
	CopyImage(ho_ImIn, &ho_ImR);
	CopyImage(ho_ImIn, &ho_ImG);
	CopyImage(ho_ImIn, &ho_ImB);

	GenCircle(&ho_Circ, hv_h / 2, hv_w / 2, hv_crad);
	PaintRegion(ho_Circ, ho_ImIn, &ho_ImR, hv_color[0], "margin");
	PaintRegion(ho_Circ, ho_ImIn, &ho_ImG, hv_color[1], "margin");
	PaintRegion(ho_Circ, ho_ImIn, &ho_ImB, hv_color[2], "margin");
	Compose3(ho_ImR, ho_ImG, ho_ImB, ho_ImOut);
}

void CInspectTestingDlg::SaveDefectImage(int x, int y, int winsz, CString defname)
{
	HObject ho_Itmp, ho_Rect, ho_Imr, ho_Imc, ho_Imcol, ho_Circ;
	HTuple hv_w, hv_h, hv_fname, hv_x, hv_y,hv_xc, hv_yc;

	GetImageSize(m_Disp_Image, &hv_w, &hv_h);
	GenImageConst(&ho_Itmp, "uint2", hv_w, hv_h);
	CopyImage(m_Disp_Image, &ho_Itmp);
	GenRectangle1(&ho_Rect, y - winsz / 2, x - winsz / 2, y + winsz / 2, x + winsz / 2);
	ReduceDomain(ho_Itmp, ho_Rect, &ho_Imr);
	CropDomain(ho_Imr, &ho_Imc);
	CopyImage(ho_Imc, &ho_Imr);

	CString s1 = m_Ed_Bath_Unit;
	
	//CString s3 = m_Image_File_Name;

	s1.Delete(0, 9);
	int l = s1.GetLength();
	s1.Delete(l-4, 4);
	CString s2 = s1 + "_"+ defname;
	hv_fname = (HTuple)s2;

	hv_x = HTuple(x);
	hv_y = HTuple(y);
	TupleString(hv_x, "d", &hv_xc);
	TupleString(hv_y, "d", &hv_yc);
	//WriteObject(ho_Imc, "C:\\DefIm\\"+ hv_fname);
	GenCircle(&ho_Circ, HTuple(winsz / 2), HTuple(winsz / 2), 40);
	PaintRegion(ho_Circ, ho_Imc, &ho_Imr, 255, "margin");
	Compose3(ho_Imr, ho_Imc, ho_Imc, &ho_Imcol);
	WriteImage(ho_Imcol, "jpg", 0, "C:\\DefIm\\" + hv_fname+"_Y"+hv_yc + "_X" + hv_xc);
	//WriteTuple
	//GenCrossContourXld(&m_Cross_Point, y, x, 50, 0);
	//Disp_Image();


}


LRESULT CInspectTestingDlg::OnImageFit(WPARAM, LPARAM)
{
	m_Zoom = 1;
	Disp_Fit_Image(m_Disp_Image);
	Disp_Defects();
	return 0;
}

BOOL CInspectTestingDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	//CWnd* pWndtooltip = GetDlgItem(IDC_IMAGE);
	static int f_MK_LBUTTON = false;
	static CPoint pos_but_down = {0, 0};
	static CPointF pos_start = { 0, 0 };
	
	if (m_Image_HWND == pWnd->m_hWnd)
	{
		if (GetKeyState(MK_LBUTTON) < 0)
		{
			if (!f_MK_LBUTTON)
			{
				GetCursorPos(&pos_but_down);
				m_Image_Control.ScreenToClient((LPPOINT)&pos_but_down);
				f_MK_LBUTTON = true;
				pos_start = m_HPos;
			}
			{
				CPoint m_pos_move;
				GetCursorPos(&m_pos_move);
				m_Image_Control.ScreenToClient((LPPOINT)&m_pos_move);

				CPointF delta;
				delta.x = (pos_but_down.x - m_pos_move.x);
				delta.y = (pos_but_down.y - m_pos_move.y);

				m_HPos.x = pos_start.x + (double)delta.x / m_RatioX * m_Zoom;
				m_HPos.y = pos_start.y + (double)delta.y / m_RatioY * m_Zoom;

				m_Window.ClearWindow();
				m_Window.SetPart(m_HPos.y, m_HPos.x, m_HPos.y + m_img_height - 1, m_HPos.x + m_img_width - 1);
				Disp_Image();
			}
		}
		else
		{
			f_MK_LBUTTON = false;
			// Значение серого курсора
			{
				CPoint pos_mouse = { 0, 0 };
				GetCursorPos(&pos_mouse);
				m_Image_Control.ScreenToClient((LPPOINT)&pos_mouse);

				CPoint pos_mouse_img = { 0, 0 };
				pos_mouse_img.x = m_HPos.x + (double)pos_mouse.x / m_RatioX * m_Zoom;
				pos_mouse_img.y = m_HPos.y + (double)pos_mouse.y / m_RatioY * m_Zoom;

				if (pos_mouse_img.y >= 0 && pos_mouse_img.x >= 0 && pos_mouse_img.y < m_Height && pos_mouse_img.x < m_Width)
				{
					HTuple gray;
					gray = m_Disp_Image.GetGrayval(pos_mouse_img.y, pos_mouse_img.x);

					RECT rect;
					m_Image_Control.GetWindowRect(&rect);
					int nWindowWidth = rect.right - rect.left;


					CString str_Txt;
					str_Txt.Format("Row: %*d, Col: %*d, Gray: %*d", 6, pos_mouse_img.y, 6, pos_mouse_img.x, 5, (int)(gray.DArr()[0]));
					m_Window.DispText(str_Txt, "window", 10, nWindowWidth - 220, "black", HTuple(), HTuple());
				}
			}
		}
	}
	//return CInspectTestingDlg::OnSetCursor(pWnd, nHitTest, message);
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	return true;
}

BOOL CInspectTestingDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	double zoom = m_Zoom - (double)zDelta / 1000 * m_Zoom;
	Zoom_Image(zoom);

	return true;
	//return OnMouseWheel(nFlags, zDelta, pt);
}

void CInspectTestingDlg::OnEnChangeEditAbsmbMeander()
{
	CString cstr;
	GetDlgItemText(IDC_EDIT_ABSMB_MEANDER, cstr);
	m_Inspection_Params.fCBM_AbsMB_Meander = (float)_tstof(cstr);
}

void CInspectTestingDlg::OnEnChangeEditAbsspMeander()
{
	CString cstr;
	GetDlgItemText(IDC_EDIT_ABSSP_MEANDER, cstr);
	m_Inspection_Params.fCBM_AbsSP_Meander = (float)_tstof(cstr);
}

void CInspectTestingDlg::OnEnChangeEditRmsizeMeander()
{
	CString cstr;
	GetDlgItemText(IDC_EDIT_RMSIZE_MEANDER, cstr);
	m_Inspection_Params.iCBM_Rmsize_Meander = (int)_tstof(cstr);
}

void CInspectTestingDlg::OnEnChangeEditAbsmbPad()
{
	CString cstr;
	GetDlgItemText(IDC_EDIT_ABSMB_PAD, cstr);
	m_Inspection_Params.fCBM_AbsMB_Pad = (float)_tstof(cstr);
}

void CInspectTestingDlg::OnEnChangeEditAbsspPad()
{
	CString cstr;
	GetDlgItemText(IDC_EDIT_ABSSP_PAD, cstr);
	m_Inspection_Params.fCBM_AbsSP_Pad = (float)_tstof(cstr);
}

void CInspectTestingDlg::OnEnChangeEditRmsizePad()
{
	CString cstr;
	GetDlgItemText(IDC_EDIT_RMSIZE_PAD, cstr);
	m_Inspection_Params.iCBM_Rmsize_Pad = (int)_tstof(cstr);
}

void CInspectTestingDlg::OnEnChangeEditAbsmbConnPad()
{
	CString cstr;
	GetDlgItemText(IDC_EDIT_ABSMB_CONN_PAD, cstr);
	m_Inspection_Params.fCBM_AbsMB_ConnPad = (float)_tstof(cstr);
}

void CInspectTestingDlg::OnEnChangeEditAbsspConnPad()
{
	CString cstr;
	GetDlgItemText(IDC_EDIT_ABSSP_CONN_PAD, cstr);
	m_Inspection_Params.fCBM_AbsSP_ConnPad = (float)_tstof(cstr);
}

void CInspectTestingDlg::OnEnChangeEditRmsizeConnPad()
{
	CString cstr;
	GetDlgItemText(IDC_EDIT_RMSIZE_CONN_PAD, cstr);
	m_Inspection_Params.iCBM_Rmsize_ConnPad = (int)_tstof(cstr);
}

void CInspectTestingDlg::OnEnChangeEditAbsmbWireAngle()
{
	CString cstr;
	GetDlgItemText(IDC_EDIT_ABSMB_WIRE_ANGLE, cstr);
	m_Inspection_Params.fCBM_AbsMB_WireAngle = (float)_tstof(cstr);
}

void CInspectTestingDlg::OnEnChangeEditAbsspWireAngle()
{
	CString cstr;
	GetDlgItemText(IDC_EDIT_ABSSP_WIRE_ANGLE, cstr);
	m_Inspection_Params.fCBM_AbsSP_WireAngle = (float)_tstof(cstr);
}

void CInspectTestingDlg::OnEnChangeEditRmsizeWireAngle()
{
	CString cstr;
	GetDlgItemText(IDC_EDIT_RMSIZE_WIRE_ANGLE, cstr);
	m_Inspection_Params.iCBM_Rmsize_WireAngle = (int)_tstof(cstr);
}

void CInspectTestingDlg::OnEnChangeEditDtDarkSpace()
{
	CString cstr;
	GetDlgItemText(IDC_EDIT_DT_DARK_SPACE, cstr);
	m_Inspection_Params.iDt_Dark_Space = (int)_tstof(cstr);
}

void CInspectTestingDlg::OnEnChangeEditDtBrightSpace()
{
	CString cstr;
	GetDlgItemText(IDC_EDIT_DT_BRIGHT_SPACE, cstr);
	m_Inspection_Params.iDt_Bright_Space = (int)_tstof(cstr);
}

void CInspectTestingDlg::OnEnChangeEditDtMeanSpace()
{
	CString cstr;
	GetDlgItemText(IDC_EDIT_DT_MEAN_SPACE, cstr);
	m_Inspection_Params.iDt_Mean_Space = (int)_tstof(cstr);
}

void CInspectTestingDlg::OnEnChangeEditDtMarginSpace()
{
	CString cstr;
	GetDlgItemText(IDC_EDIT_DT_MARGIN_SPACE, cstr);
	m_Inspection_Params.iDt_Margin_Space = (int)_tstof(cstr);
}

void CInspectTestingDlg::OnEnChangeEditDtAreaSpace()
{
	CString cstr;
	GetDlgItemText(IDC_EDIT_DT_AREA_SPACE, cstr);
	m_Inspection_Params.iDt_Area_Space = (int)_tstof(cstr);
}

void CInspectTestingDlg::OnEnChangeEditDtDarkPattern()
{
	CString cstr;
	GetDlgItemText(IDC_EDIT_DT_DARK_PATTERN, cstr);
	m_Inspection_Params.iDt_Dark_Pattern = (int)_tstof(cstr);
}

void CInspectTestingDlg::OnEnChangeEditDtBrightPattern()
{
	CString cstr;
	GetDlgItemText(IDC_EDIT_DT_BRIGHT_PATTERN, cstr);
	m_Inspection_Params.iDt_Bright_Pattern = (int)_tstof(cstr);
}

void CInspectTestingDlg::OnEnChangeEditDtMarginPattern()
{
	CString cstr;
	GetDlgItemText(IDC_EDIT_DT_MARGIN_PATTERN, cstr);
	m_Inspection_Params.iDt_Margin_Pattern = (int)_tstof(cstr);
}

void CInspectTestingDlg::OnEnChangeEditDtAreaPattern()
{
	CString cstr;
	GetDlgItemText(IDC_EDIT_DT_AREA_PATTERN, cstr);
	m_Inspection_Params.iDt_Area_Pattern = (int)_tstof(cstr);
}

void CInspectTestingDlg::OnEnChangeEditDtLengthPattern()
{
	CString cstr;
	GetDlgItemText(IDC_EDIT_DT_LENGTH_PATTERN, cstr);
	m_Inspection_Params.iDt_Length_Pattern = (int)_tstof(cstr);
}

void CInspectTestingDlg::OnEnChangeEditDtWidthPattern()
{
	CString cstr;
	GetDlgItemText(IDC_EDIT_DT_WIDTH_PATTERN, cstr);
	m_Inspection_Params.iDt_Width_Pattern = (int)_tstof(cstr);
}

void CInspectTestingDlg::OnEnChangeEditDtConnectionDistance()
{
	CString cstr;
	GetDlgItemText(IDC_EDIT_DT_CONNECTION_DISTANCE, cstr);
	m_Inspection_Params.iDt_Connect_Distance = (int)_tstof(cstr);
}

void CInspectTestingDlg::OnBnClickedCheckDtConnection()
{
	CButton *m_ctlCheck = (CButton*)GetDlgItem(IDC_CHECK_DT_CONNECTION);
	m_Inspection_Params.bDtConnectDef = m_ctlCheck->GetCheck();
}

void CInspectTestingDlg::OnEnChangeEditmid()
{
	CString cstr;
	GetDlgItemText(IDC_EDITMID, cstr);
	m_Inspection_Params.LGE_mid = (float)_tstof(cstr);
}


void CInspectTestingDlg::OnEnChangeEditmargin()
{
	CString cstr;
	GetDlgItemText(IDC_EDITMARGIN, cstr);
	m_Inspection_Params.LGE_margin = (float)_tstof(cstr);
}


void CInspectTestingDlg::OnBnClickedCheckMmatoff()
{
	CButton *m_ctlCheck = (CButton*)GetDlgItem(IDC_CHECK_MMATOFF);
	m_Inspection_Params.LGE_off = m_ctlCheck->GetCheck();
	Disp_Image();
}

void CInspectTestingDlg::OnBnClickedButtonMaxmin()
{
	m_inspect->Proc_MinG(m_Inspection_Params.running_medians_span);
}


void CInspectTestingDlg::OnBnClickedCheckDispDefects()
{
	Disp_Image();
}


void CInspectTestingDlg::OnBnClickedCheckUnionDefects()
{
	Disp_Image();
}


HBRUSH CInspectTestingDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here
	if (pWnd->GetDlgCtrlID() == IDC_STATICMBP)
	{
		//set the static text color to cyan      
		pDC->SetTextColor(RGB(0, 200, 200));
	}
	if (pWnd->GetDlgCtrlID() == IDC_STATICSPP)
	{
		//set the static text color to magenta      
		pDC->SetTextColor(RGB(200, 0, 200));
	}
	if (pWnd->GetDlgCtrlID() == IDC_STATICSPS)
	{
		//set the static text color to dark orchid     
		pDC->SetTextColor(RGB(100, 10, 150));
	}
	if (pWnd->GetDlgCtrlID() == IDC_STATICMBS)
	{
		//set the static text color to orange     
		pDC->SetTextColor(RGB(200, 150, 0));
	}
	if (pWnd->GetDlgCtrlID() == IDC_STATIC4)
	{
		//set the static text color to orange     
		pDC->SetTextColor(RGB(0, 200, 0));
	}

	if (pWnd->GetDlgCtrlID() == IDC_STATICNWS)
	{
		//set the static text color to dark orchid     
		pDC->SetTextColor(RGB(10, 50, 150));
	}

	if (pWnd->GetDlgCtrlID() == IDC_STATICPABS)
	{
		//set the static text color to dark orchid     
		pDC->SetTextColor(RGB(100, 50, 50));
	}
	// TODO:  Return a different brush if the default is not desired
	return hbr;
}


void CInspectTestingDlg::OnEnChangeEditlgprc()
{
	//CString cstr;
	//GetDlgItemText(IDC_EDITLGPRC, cstr);
	//m_Inspection_Params.LG_m_prsLG = (float)_tstof(cstr);
}




void CInspectTestingDlg::OnEnChangeEditInspectionCount()
{
	CString cstr;
	GetDlgItemText(IDC_EDIT_INSPECTION_COUNT, cstr);
	m_Bath_Image_Count = (int)_tstof(cstr);
	//m_Bath_Image_Count
}


void CInspectTestingDlg::OnEnChangeEditInspectionPos()
{
	CString cstr;
	GetDlgItemText(IDC_EDIT_INSPECTION_POS, cstr);
	m_Bath_Pos = (int)_tstof(cstr);
}


void CInspectTestingDlg::OnEnChangeEditInspectionFrom()
{
	CString cstr;
	GetDlgItemText(IDC_EDIT_INSPECTION_FROM, cstr);
	m_Bath_From = (int)_tstof(cstr);
	
}


void CInspectTestingDlg::OnEnChangeEditInspectionTo()
{
	CString cstr;
	GetDlgItemText(IDC_EDIT_INSPECTION_TO, cstr);
	m_Bath_To = (int)_tstof(cstr);
}

void CInspectTestingDlg::OnBnClickedButTeachOpenshort2()
{
	// Получим CAD изображение
	HObject ho_Gi, ho_RegNoProc;
	ReadImage(&ho_Gi, HTuple(m_model_Path + "Gi.tif"));
	ReadObject(&ho_RegNoProc, HTuple(m_model_Path + "RegNoProc.hobj"));

	Open_Short_2 *op_teach;
	op_teach = new Open_Short_2();

	// Параметры
	{
		// Допустимый разрыв серого при обучении
		op_teach->params.cv_Gray_Gap_Teach = 20;
		// Размер буфера для Points_Buff. Размер изображения?
		op_teach->params.buff_size = 100000000;
		// Размер инкремента буффера для Cross_Gray
		op_teach->params.alloc_step = 1000000;
		op_teach->params.save_test_file = 0;

	}
	op_teach->Init_Teaching();
	op_teach->Teaching_Open_Short(ho_Gi, ho_RegNoProc, HTuple(m_model_Path));

	op_teach->Read_Auto_Params(
		HTuple(m_model_Path),
		&inspect_call::m_Inspection_Params.cv_Min_CAD_Area_Short,
		&inspect_call::m_Inspection_Params.cv_Min_CAD_Area_Open,
		&inspect_call::m_Inspection_Params.cv_Gray_Gap_Open,
		&inspect_call::m_Inspection_Params.cv_Gray_Gap_Short);
}






void CInspectTestingDlg::OnBnClickedButton8()
{
	m_Window.SetColor("yellow");
	m_Window.DispObj(m_ho_RegNoProc);
}


void CInspectTestingDlg::OnEnChangeEditszp()
{
	CString cstr;
	GetDlgItemText(IDC_EDITSZP, cstr);
	m_Inspection_Params.sz1 = (int)_tstof(cstr);;
}


void CInspectTestingDlg::OnEnChangeEditszs()
{
	CString cstr;
	GetDlgItemText(IDC_EDITSZS, cstr);
	m_Inspection_Params.sz2 = (int)_tstof(cstr);;
}


void CInspectTestingDlg::OnEnChangeEditminwp()
{
	CString cstr;
	GetDlgItemText(IDC_EDITMINWP, cstr);
	m_Inspection_Params.minwp = (float)_tstof(cstr);
}

void CInspectTestingDlg::OnEnChangeEditLgPrcS()
{
	UpdateData(TRUE);
	m_Inspection_Params.LG_m_prsLG = (float)_tstof(m_Lg_Prs_S);
	UpdateData(FALSE);
}

void CInspectTestingDlg::OnEnChangeEditLgPrcP()
{
	UpdateData(TRUE);
	m_Inspection_Params.LG_m_prsLG_P = (float)_tstof(m_Lg_Prs_P);
	UpdateData(FALSE);
}

void CInspectTestingDlg::OnEnChangeEditLgSzwP()
{
	UpdateData(TRUE);
	m_Inspection_Params.LG_m_szw_P = (float)_tstof(m_Lg_Szw_P);
	UpdateData(FALSE);
}

void CInspectTestingDlg::OnEnChangeEditLgSzwS()
{
	UpdateData(TRUE);
	m_Inspection_Params.LG_m_szw = (float)_tstof(m_Lg_Szw_S);
	UpdateData(FALSE);
}

void CInspectTestingDlg::OnEnChangeEditLgAvgP()
{
	UpdateData(TRUE);
	m_Inspection_Params.LG_m_Avg_Par = (float)_tstof(m_Lg_Avg_P);
	UpdateData(FALSE);
}


void CInspectTestingDlg::OnBnClickedCheckAutothr3()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_CHECK_AUTOTHR3)->EnableWindow(m_Preproc);


	m_Inspection_Params.inspect_Preproc = m_Preproc;
	
	Enable_Inspection_Buttons(!m_Preproc);

	UpdateData(FALSE);
	
}


void CInspectTestingDlg::OnBnClickedCheckPreproc()
{
	//UpdateData(TRUE);

	CButton *m_ctlCheck = (CButton*)GetDlgItem(IDC_CHECK_PREPROC);
	m_Preproc = m_ctlCheck->GetCheck();


	m_Inspection_Params.inspect_Preproc = m_Preproc;


}


void CInspectTestingDlg::OnEnChangeEditprk()
{
	CString cstr;
	GetDlgItemText(IDC_EDITPRK, cstr);
	m_Prepr_K = (float)_tstof(cstr);
}




void CInspectTestingDlg::OnEnChangeEditnarwp()
{
	CString cstr;
	GetDlgItemText(IDC_EDITNARWP, cstr);
	m_Inspection_Params.narrowwp = (float)_tstof(cstr);
}


void CInspectTestingDlg::OnBnClickedViewregiong()
{
	m_Window.SetColor("blue");
	m_Window.DispObj(m_ho_RegionG);
}

void CInspectTestingDlg::OnBnClickedButtonParamsDlg()
{

	CWnd* pParentWnd = this;

	HINSTANCE hRes_Main = AfxGetResourceHandle();
	alg_params->Show_Dialog_Params(pParentWnd, m_model_Path, &m_Inspection_Params, 6000);
	AfxSetResourceHandle(hRes_Main);


	//alg_params->Load_Params_Name(m_model_Path, "inspection.json", 6000, &m_Inspection_Params);




	//CTest test;
	//int i = test.Test(100);

	//CDialog_Params *m_dlg_params;
	//m_dlg_params = new CDialog_Params();

	//m_dlg_params->DoModal();
	//m_dlg_params->ShowWindow(SW_SHOW);

	//dlg_params.DoModal();


	//delete m_dlg_params;
	


	//dlg_params = new CDialog_Params(nullptr);
	//AFX_MANAGE_STATE(AfxGetStaticModuleState())



	//m_parms_dlg->UpdateData(TRUE);

	//m_parms_dlg->m_Min_Area_Open = m_Inspection_Params.cv_Min_CAD_Area_Open;				// Минмальная допустимая площадь региона CAD для идентификации номеров закрашивания
	//m_parms_dlg->m_Min_Area_Short = m_Inspection_Params.cv_Min_CAD_Area_Short;				// Минмальная допустимая площадь региона CAD для идентификации номеров закрашивания
	//m_parms_dlg->m_Gray_Gap_Open = m_Inspection_Params.cv_Gray_Gap_Open;					// Допустимый разрыв серого
	//m_parms_dlg->m_Gray_Gap_Short = m_Inspection_Params.cv_Gray_Gap_Short;					// Допустимый разрыв серого
	//m_parms_dlg->m_model_Path = m_model_Path;

	//m_parms_dlg->m_parent = this->m_hWnd;
	//m_parms_dlg->UpdateData(FALSE);





	//m_parms_dlg->ShowWindow(SW_SHOW);

	//if (m_parms_dlg->DoModal() == IDOK)
	//{
	//	m_Inspection_Params.cv_Min_CAD_Area_Open = m_parms_dlg->m_Min_Area_Open;
	//	m_Inspection_Params.cv_Min_CAD_Area_Short = m_parms_dlg->m_Min_Area_Short;
	//	m_Inspection_Params.cv_Gray_Gap_Open = m_parms_dlg->m_Gray_Gap_Open;
	//	m_Inspection_Params.cv_Gray_Gap_Short = m_parms_dlg->m_Gray_Gap_Short;

	//}

}


void CInspectTestingDlg::OnBnClickedViewregioncut()
{
	m_Window.SetColor("gold");
	m_Window.DispObj(m_ho_RegionCut);
}


void CInspectTestingDlg::OnEnChangeEditwidewp()
{
	CString cstr;
	GetDlgItemText(IDC_EDITWIDEWP, cstr);
	m_Inspection_Params.widewp = (float)_tstof(cstr);
}


afx_msg LRESULT CInspectTestingDlg::OnUpdateParamsEvents(WPARAM wParam, LPARAM lParam)
{

	m_Inspection_Params.cv_Min_CAD_Area_Open = m_parms_dlg->m_Min_Area_Open;
	m_Inspection_Params.cv_Min_CAD_Area_Short = m_parms_dlg->m_Min_Area_Short;
	m_Inspection_Params.cv_Gray_Gap_Open = m_parms_dlg->m_Gray_Gap_Open;
	m_Inspection_Params.cv_Gray_Gap_Short = m_parms_dlg->m_Gray_Gap_Short;



	return 0;
}




afx_msg LRESULT CInspectTestingDlg::OnParamApplyEvents(WPARAM wParam, LPARAM lParam)
{

	Update_Dlg_Params();
	return 0;
}

void CInspectTestingDlg::OnBnClickedViewregioni()
{
	m_Window.SetColor("orange");
	m_Window.DispObj(m_inspect->m_ho_RegionI);
}
BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_NOTIFY(NM_RETURN, IDC_LIST_UNIT_DEFECTS, &CAboutDlg::OnNMReturnListUnitDefects)
END_MESSAGE_MAP()


void CAboutDlg::OnNMReturnListUnitDefects(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_LIST_UNIT_DEFECTS)->SetFocus();

	Invalidate();
	*pResult = 0;
}


void CInspectTestingDlg::OnBnClickedViewimmod()
{
	m_Window.DispObj(m_inspect->m_local_ImModified);
	//Disp_Fit_Image(m_inspect->m_local_Im);
	m_Disp_Image = m_inspect->m_local_ImModified;
}

void CInspectTestingDlg::OnBnClickedViewCbmMeander()
{
	m_Window.SetColor("blue");
	m_Window.DispObj(m_ho_ContoursGrsm);
}

void CInspectTestingDlg::OnBnClickedViewCbmPad()
{
	m_Window.SetColor("cyan");
	m_Window.DispObj(m_ho_ContoursGrsmPi);
}

void CInspectTestingDlg::OnBnClickedViewCbmConnPad()
{
	m_Window.SetColor("magenta");
	m_Window.DispObj(m_ho_ContoursGrsmPc);
}

void CInspectTestingDlg::OnBnClickedViewCbmWireAngle()
{
	m_Window.SetColor("yellow");
	m_Window.DispObj(m_ho_ContoursWireAngles);
}

void CInspectTestingDlg::OnBnClickedCheckOp2()
{
	// TODO: Add your control notification handler code here
}


void CInspectTestingDlg::OnBnClickedWpset()
{
	//HTuple hv_narrowwp, hv_widewp;
	CString cstr;
	HTuple hv_kw = 1.3;
	HTuple hv_dw = 10.0;

	//hv_narrowwp = m_Inspection_Params.narrowwp;
	//hv_widewp = m_Inspection_Params.widewp;

	m_inspect->SetWpNarrowWide(m_ho_SkeletonsFWM, m_ho_WidthImageP, hv_kw, 	hv_dw, 
		&(m_Inspection_Params.narrowwp), &(m_Inspection_Params.widewp));

	cstr.Format(_T("%.1f"), m_Inspection_Params.narrowwp);
	SetDlgItemText(IDC_EDITNARWP, cstr);

	cstr.Format(_T("%.1f"), m_Inspection_Params.widewp);
	SetDlgItemText(IDC_EDITWIDEWP, cstr);
}


void CInspectTestingDlg::OnBnClickedWpseti()
{
	CString cstr;

	m_Inspection_Params.narrowwp = m_inspect->narrowwp;
	m_Inspection_Params.widewp = m_inspect->widewp;
		

	cstr.Format(_T("%.1f"), m_Inspection_Params.narrowwp);
	SetDlgItemText(IDC_EDITNARWP, cstr);

	cstr.Format(_T("%.1f"), m_Inspection_Params.widewp);
	SetDlgItemText(IDC_EDITWIDEWP, cstr);
}


void CInspectTestingDlg::OnBnClickedButtonProcCbmMeander()
{
	m_inspect->Proc_CBM_Meander();

	m_Window.DispObj(m_inspect->m_local_Im);
	DispObject(m_inspect->m_ho_RegionsMeanderMB, "red");
	//DispObject(m_inspect->m_ho_RegionsMeanderSP, "magenta");
}


void CInspectTestingDlg::OnBnClickedButtonProcCbmPad()
{
	m_inspect->Proc_CBM_Pad();

	m_Window.DispObj(m_inspect->m_local_Im);
	DispObject(m_inspect->m_ho_RegionsPadMB, "red");
	//DispObject(m_inspect->m_ho_RegionsPadSP, "magenta");
}


void CInspectTestingDlg::OnBnClickedButtonProcCbmConnPad()
{
	m_inspect->Proc_CBM_ConnPad();

	m_Window.DispObj(m_inspect->m_local_Im);
	DispObject(m_inspect->m_ho_RegionsConnPadMB, "red");
	//DispObject(m_inspect->m_ho_RegionsConnPadSP, "magenta");
}


void CInspectTestingDlg::OnBnClickedButtonProcCbmWireAngle()
{
	m_inspect->Proc_CBM_WireAngle();

	m_Window.DispObj(m_inspect->m_local_Im);
	DispObject(m_inspect->m_ho_RegionsWireAngleMB, "red");
	//DispObject(m_inspect->m_ho_RegionsWireAngleSP, "magenta");
}


void CInspectTestingDlg::OnBnClickedButtonProcDtSpace()
{
	m_Window.DispObj(m_inspect->m_local_Im);
	m_Disp_Image = m_inspect->m_local_Im;

	GenEmptyObj(&m_inspect->m_ho_RegionsDT_S);
	m_inspect->m_ho_RegionsDT_S = m_inspect->Proc_DynThres_Space();
	DispObject(m_inspect->m_ho_RegionsDT_S, "red");
}


void CInspectTestingDlg::OnBnClickedButtonProcDtPattern()
{
	m_Window.DispObj(m_inspect->m_local_Im);
	m_Disp_Image = m_inspect->m_local_Im;

	GenEmptyObj(&m_inspect->m_ho_RegionsDT_P);
	m_inspect->m_ho_RegionsDT_P = m_inspect->Proc_DynThres_Pattern();
	DispObject(m_inspect->m_ho_RegionsDT_P, "red");
}
