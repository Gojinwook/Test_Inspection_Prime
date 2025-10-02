#pragma once


// CParams_Dialog dialog
#define WM_UPDATE_PARAMS_EVENTS				WM_USER + 500
#define WM_SAVE_PARAMS_EVENTS				WM_USER + 501

class CParams_Dialog : public CDialog
{
	DECLARE_DYNAMIC(CParams_Dialog)

public:
	CParams_Dialog(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CParams_Dialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_PARAMS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	int m_Min_Area_Open;
	int m_Min_Area_Short;
	int m_Gray_Gap_Open;
	int m_Gray_Gap_Short;
	afx_msg void OnBnClickedButOpenShortAuto();

public:
	HWND m_parent;
	CString m_model_Path;


protected:
//	afx_msg LRESULT OnUpdateParamsEvents(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateParamsEvents(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedButUpdate();
	afx_msg void OnBnClickedButton2();
};
