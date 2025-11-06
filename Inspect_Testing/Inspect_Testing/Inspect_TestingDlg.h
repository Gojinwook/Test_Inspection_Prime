
// Inspect_TestingDlg.h : header file
//
#pragma once

#include "Common.h"
#include "Inspect_Common.h"
#include "Inspect_Call.h"
#include "CParams_Dialog.h"
#include "CParams.h"

// CInspectTestingDlg dialog
class CInspectTestingDlg : public CDialog
{


// Construction
public:
	CInspectTestingDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INSPECT_TESTING_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT message);

	BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	void Set_Thread();
	void CInspectTestingDlg::Read_Default_Values();
	void UpdateFileName(HTuple hv_name, HTuple hv_spl, HTuple hv_nch, HTuple hv_rch,
		HTuple *hv_fname);
	void Bath_Enable();
	void Bath_Insert_Defect_List();
	void Bath_Update();
	int Disp_Defects();
	LRESULT OnImageFit(WPARAM, LPARAM);
	void DispObjImage(HObject obj, HTuple color);
	LRESULT CInspectTestingDlg::OnBathInspectEvent(WPARAM, LPARAM);

	LRESULT OnBathInspectEventDefect(WPARAM, LPARAM);

	LRESULT OnBathInspectEventADDDefect(WPARAM, LPARAM);

private:
	HObject m_ho_Im;
	HTuple hv_WindowHandle;
	HalconCpp::HWindow m_Window;
	HWND m_Image_HWND;
	HObject m_Cross_Point;

	CParams *alg_params;
	CParams_Dialog *m_parms_dlg;


public:
	CString m_Folder_Teach;
	afx_msg void OnBnClickedButtonSetTeachFolder();
	afx_msg void OnBnClickedButtonSetDataFolder();
	afx_msg void OnBnClickedButton_ConvertToBin();
	CString m_Image_Name;
	CString m_Image_File_Name;
	afx_msg	void Disp_Fit_Image(HObject ho_Im);
	void Read_Image(CString image_name);
	void Image_Name_Cursor_End();
	void OnBnClickedButtonSetImage();
	void Clear_Defects();
	afx_msg void OnBnClickedButtonLoadTeach();

	CString m_model_Path;
	CString m_bin_Path;

	afx_msg void OnBnClickedButtonAlignment();
	afx_msg void OnBnClickedButtonPinholeIsland();
	BOOL m_Raw_Image;
	BOOL m_Auto_Thr;
	BOOL m_Preproc;
	float m_Prepr_K;
	afx_msg void OnBnClickedCheckRawImage();
	afx_msg void OnBnClickedButtonBuildFwm();
	afx_msg
		void Enable_Inspection_Buttons(bool en);
	void OnBnClickedButtonInspectionInit();

	afx_msg void OnBnClickedButtonReadIrb();
	afx_msg	void DispObject(HObject ho_Obj, HTuple hv_Color);
	void OnBnClickedButtonBuildFwms();
	afx_msg void OnBnClickedButtonOpenShort();
	afx_msg void OnBnClickedButtonBuildIrb();
	afx_msg void OnEnChangeEditImage();
	afx_msg void OnEnChangeEditprcmbs();
	afx_msg void OnBnClickedCheckInitAuto();
	BOOL m_Init_Auto;
	afx_msg void OnBnClickedViewirbp();
	afx_msg void OnBnClickedViewirbs();
	afx_msg void OnBnClickedViewim();
	afx_msg void OnBnClickedButtonInspectionAll();
	afx_msg void OnEnChangeEditprcsps();
	afx_msg void OnEnChangeEditthr();
	afx_msg
		void Update_Dlg_Params();
	void OnBnClickedButtonLoadParams();
	afx_msg void OnBnClickedButtonSaveParams();
	afx_msg void OnBnClickedButtonLowGray();
	afx_msg void OnBnClickedButtonAutoThr();
	afx_msg void OnEnChangeEditprcmbp();
	afx_msg void OnEnChangeEditprcspp();
	afx_msg void OnBnClickedButtonInspectionAll2();
	afx_msg void OnBnClickedViewinod();
	afx_msg void OnBnClickedButtonReadImage();
	afx_msg void OnBnClickedCheckAutothr();
	afx_msg void OnEnChangeEditmabs();
	afx_msg void OnEnChangeEditminabs();
	afx_msg void OnBnClickedButtonEditParams();
	afx_msg void OnBnClickedWizfinish();
	afx_msg void OnBnClickedWizfinish2();
	afx_msg void OnBnClickedWizfinish3();
	afx_msg void OnBnClickedWizfinish4();
	afx_msg void OnBnClickedButton1();
	void Zoom_Point(int x, int y);
	void SaveDefectImage(int x, int y, int winsz, CString defname);
	afx_msg void OnEnChangeEditminabs2();
	afx_msg void OnBnClickedViewskelg();
	afx_msg void OnBnClickedButtonBathInit();
	BOOL m_Check_If_Defect;
	BOOL m_Check_Inspect_LG;
	BOOL m_Check_Inspect_FWM;
	BOOL m_Check_Inspect_FWMS;
	CProgressCtrl m_Inspect_Progress;
	CString m_Ed_Bath_Unit;
	CString m_Edit_Inspection_Pos;
	CString m_Bath_Count;
	afx_msg
		void Bath_Set_Inspect_Unit();
	void OnBnClickedButtonBathNext();
	afx_msg void OnBnClickedButtonBathStart();
	afx_msg void OnBnClickedButtonBathStop();
	afx_msg void OnBnClickedButtonBathPrevious();
	CListCtrl m_Bath_Defects_List;
	afx_msg void OnBnClickedButtonBathClear();
	afx_msg
		void Add_List_Units_Defects(CString defect_name, HObject ho_Defects);
	void Insert_List_Units_Defects();
	void OnLvnItemchangedListDefects(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCheckInspectLg();
	afx_msg void OnBnClickedCheckInspectFwm();
	afx_msg void OnBnClickedCheckInspectFwms();
	afx_msg void OnBnClickedButtonMing();
	BOOL m_Auto_Threshold;
	afx_msg void OnBnClickedCheckAutothrBath();
	afx_msg void OnBnClickedCheckAutoThrBath();
	CString m_Ed_Defect_Count;
	afx_msg void OnBnClickedButtonBathSave();
	afx_msg
		void Zoom_Image(double zoom);
	void OnStnDblclickImage();
	void Disp_Image();
	afx_msg void OnRButtonDownImage(UINT nFlags, CPoint point);
	CStatic m_Image_Control;
	afx_msg void OnBnClickedButtonFit();

	afx_msg void OnEnChangeEditmmats();
	afx_msg void OnEnChangeEditmid();
	afx_msg void OnEnChangeEditmargin();
	afx_msg void OnBnClickedCheckMmatoff();

	afx_msg void OnBnClickedButtonSave();

	afx_msg void OnBnClickedButtonSaveDefects();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButtonMaxmin();
	BOOL m_Check_Dip_Defects;
	BOOL m_Check_Union_Defects;
	afx_msg void OnBnClickedCheckDispDefects();
	afx_msg void OnBnClickedCheckUnionDefects();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnEnChangeEditlgprc();
	afx_msg void OnBnClickedButtonRead();
	void Test_Sub_Threshold();
	afx_msg void OnEnChangeEditInspectionCount();
	afx_msg void OnEnChangeEditInspectionPos();
	afx_msg void OnEnChangeEditInspectionFrom();
	afx_msg void OnEnChangeEditInspectionTo();
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedViewskelg2();
	afx_msg void OnBnClickedButTeachOpenshort2();
	afx_msg void OnBnClickedButtonOpenShort2();
	BOOL m_Check_Open_Short_2;
	afx_msg void OnBnClickedViewregnoprooc();
	afx_msg void OnBnClickedButton8();
	afx_msg void OnEnChangeEditszp();
	afx_msg void OnEnChangeEditszs();
	afx_msg void OnEnChangeEditminwp();
	CString m_Lg_Prs_P;
	CString m_Lg_Prs_S;
	CString m_Lg_Szw_P;
	CString m_Lg_Szw_S;
	CString m_Lg_Avg_P;
	afx_msg void OnEnChangeEditLgPrcS();
	afx_msg void OnEnChangeEditLgPrcP();
	afx_msg void OnEnChangeEditLgSzwP();
	afx_msg void OnEnChangeEditLgSzwS();
	afx_msg void OnEnChangeEditLgAvgP();
	afx_msg void OnBnClickedCheckAutothr3();
	afx_msg void OnBnClickedCheckPreproc();
	afx_msg void OnEnChangeEditprk();
	CListCtrl m_List_Unit_Defects;
	CListCtrl m_Defect_Attribute;
	afx_msg void OnLvnItemchangedListUnitDefects(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditnarwp();
	afx_msg void OnBnClickedViewregiong();
	afx_msg void OnBnClickedButtonParamsDlg();
	afx_msg void OnBnClickedViewregioncut();
	afx_msg void OnEnChangeEditwidewp();

	afx_msg void OnEnChangeEditAbsmbMeander();
	afx_msg void OnEnChangeEditAbsspMeander();
	afx_msg void OnEnChangeEditRmsizeMeander();
	afx_msg void OnEnChangeEditAbsmbPad();
	afx_msg void OnEnChangeEditAbsspPad();
	afx_msg void OnEnChangeEditRmsizePad();
	afx_msg void OnEnChangeEditAbsmbConnPad();
	afx_msg void OnEnChangeEditAbsspConnPad();
	afx_msg void OnEnChangeEditRmsizeConnPad();
	afx_msg void OnEnChangeEditAbsmbWireAngle();
	afx_msg void OnEnChangeEditAbsspWireAngle();
	afx_msg void OnEnChangeEditRmsizeWireAngle();

	afx_msg void OnEnChangeEditDtDarkSpace();
	afx_msg void OnEnChangeEditDtBrightSpace();
	afx_msg void OnEnChangeEditDtMeanSpace();
	afx_msg void OnEnChangeEditDtMarginSpace();
	afx_msg void OnEnChangeEditDtAreaSpace();

	afx_msg void OnEnChangeEditDtDarkPattern();
	afx_msg void OnEnChangeEditDtBrightPattern();
	afx_msg void OnEnChangeEditDtMarginPattern();
	afx_msg void OnEnChangeEditDtAreaPattern();
	afx_msg void OnEnChangeEditDtLengthPattern();
	afx_msg void OnEnChangeEditDtWidthPattern();
	afx_msg void OnBnClickedCheckDtConnection();
	afx_msg void OnEnChangeEditDtConnectionDistance();

protected:
	afx_msg LRESULT OnUpdateParamsEvents(WPARAM wParam, LPARAM lParam);
public:
	BOOL m_Check_Alig2;
	afx_msg void OnBnClickedCheckAlig2();
	BOOL m_Check_AutoThres2;
	afx_msg void OnBnClickedCheckAutoThres2();
protected:
	afx_msg LRESULT OnParamApplyEvents(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedViewregioni();
public:
	afx_msg void OnBnClickedViewimmod();
	afx_msg void OnBnClickedCheckOp2();
	afx_msg void OnBnClickedWpset();
	afx_msg void OnBnClickedWpseti();
	afx_msg void OnBnClickedButtonProcCbmMeander();
	afx_msg void OnBnClickedButtonProcCbmPad();
	afx_msg void OnBnClickedButtonProcCbmConnPad();
	afx_msg void OnBnClickedViewCbmMeander();
	afx_msg void OnBnClickedViewCbmPad();
	afx_msg void OnBnClickedViewCbmConnPad();
	afx_msg void OnBnClickedButtonProcCbmWireAngle();
	afx_msg void OnBnClickedViewCbmWireAngle();
	afx_msg void OnBnClickedButtonProcDtSpace();
	afx_msg void OnBnClickedButtonProcDtPattern();
};
