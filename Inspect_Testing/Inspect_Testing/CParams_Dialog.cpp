// CParams_Dialog.cpp : implementation file
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

#include "Inspect_Testing.h"
#include "Lib/Open_Short_2.h"

#include "CParams_Dialog.h"

using namespace HalconCpp;
using namespace std;

// CParams_Dialog dialog

IMPLEMENT_DYNAMIC(CParams_Dialog, CDialog)

CParams_Dialog::CParams_Dialog(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DIALOG_PARAMS, pParent)
	, m_Min_Area_Open(0)
	, m_Min_Area_Short(0)
	, m_Gray_Gap_Open(0)
	, m_Gray_Gap_Short(0)
{

}

CParams_Dialog::~CParams_Dialog()
{
}

void CParams_Dialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_MIN_AREA_OPEN, m_Min_Area_Open);
	DDX_Text(pDX, IDC_MIN_AREA_SHORT, m_Min_Area_Short);
	DDX_Text(pDX, IDC_GRAY_GAP_OPEN, m_Gray_Gap_Open);
	DDX_Text(pDX, IDC_GRAY_GAP_SHORT, m_Gray_Gap_Short);
}


BEGIN_MESSAGE_MAP(CParams_Dialog, CDialog)
	ON_BN_CLICKED(IDC_BUT_OPEN_SHORT_AUTO, &CParams_Dialog::OnBnClickedButOpenShortAuto)
	ON_BN_CLICKED(IDC_BUT_UPDATE, &CParams_Dialog::OnBnClickedButUpdate)
	ON_BN_CLICKED(IDC_BUTTON2, &CParams_Dialog::OnBnClickedButton2)
END_MESSAGE_MAP()


// CParams_Dialog message handlers


void CParams_Dialog::OnBnClickedButOpenShortAuto()
{
	Open_Short_2 *op_teach;
	op_teach = new Open_Short_2();

	UpdateData(TRUE);
	op_teach->Read_Auto_Params(
		HTuple(m_model_Path),
		&m_Min_Area_Short,
		&m_Min_Area_Open,
		&m_Gray_Gap_Open,
		&m_Gray_Gap_Short);
	UpdateData(FALSE);

}



//void Send(HWND parent, WPARAM wParam, LPARAM lParam)
//{
//	SendMessage(parent, WM_UPDATE_PARAMS_EVENTS, wParam, lParam);
//}

void CParams_Dialog::OnBnClickedButUpdate()
{
	//Send(m_parent, 1, 2);
}


void CParams_Dialog::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
}
