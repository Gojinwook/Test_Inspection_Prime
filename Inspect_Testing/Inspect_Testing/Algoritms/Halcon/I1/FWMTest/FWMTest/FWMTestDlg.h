
// FWMTestDlg.h : header file
//

#pragma once
//#include "FWMFunc.h"

// CFWMTestDlg dialog
class CFWMTestDlg : public CDialogEx
{
// Construction
public:
	int imopen; // image open
	HTuple hv_WindowHandle;
//WMF fwm;				// FWM class
	CFWMTestDlg(CWnd* pParent = nullptr);	// standard constructor


	float * ConvertTupleToFloat(HTuple T);
	HTuple ConvertFloatToTuple(float  *dp, int len);
	int * ConvertTupleToInt(HTuple T);
	INT64 * ConvertFloatToINT64(float *dp, int len);
	double * ConvertFloatToDouble(float *dp, int len);
	double * ConvertIntToDouble(int *dp, int len);
	double * ConvertINT64ToDouble(UINT64 *dp, int len);
	INT64 * ConvertIntToINT64(int *dp, int len);

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FWMTEST_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	//afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedStartfwm();
	afx_msg void OnBnClickedLoadtup();
	afx_msg void OnBnClickedLoadim();
	afx_msg void OnBnClickedCdispim();
	afx_msg void OnBnClickedCdispirbp();
	afx_msg void OnBnClickedCdispirbs();
	afx_msg void OnBnClickedAlloc();
	afx_msg void OnBnClickedStartfwm2();
	afx_msg void OnBnClickedFwmspp();
};
