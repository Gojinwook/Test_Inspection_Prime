
// FWMTestDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "FWMTest.h"
#include "FWMTestDlg.h"
#include "afxdialogex.h"
#include "FWMFunc.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//HTuple hv_path = "C:\\Samsung\\spir5-240216\\Teach\\Tuples\\";
//HTuple hv_pathd = "C:\\Samsung\\spir5-240216\\Teach\\";
HTuple hv_path = "C:\\WORK_CURR\\SAMSUNG\\Inspection\\Inspect_Testing\\Inspect_Testing\\Algoritms\\Halcon\\I1\\spir5-240216\\Teach\\Tuples\\";
HTuple hv_pathd = "C:\\WORK_CURR\\SAMSUNG\\Inspection\\Inspect_Testing\\Inspect_Testing\\Algoritms\\Halcon\\I1\\spir5-240216\\Teach\\";


//  DATA
// input:
//UINT8 *pIm; // Im image
//UINT8 *pImIRBP; // IRB image
//UINT8 *pImIRBS; // IRBS image
//UINT16 *pImCLC; // CLC image



	/*extern void CPU_MSPtProc39B2(UINT8 *pIm, UINT8 *pImIRB, UINT16 *pImCLC, int imw, int imh, int pst, int pn,
		float thr, int dclcmax, float *dP1x, float *dP1y, float *dP2x, float *dP2y, int *dPnum, int *dPnum2,
		float *dW, float *dWmin, float *dPint1x, float *dPint1y, float *dPint2x, float *dPint2y, float *dShift, int *dPuseint);*/

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
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CFWMTestDlg dialog



CFWMTestDlg::CFWMTestDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FWMTEST_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFWMTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFWMTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_STARTFWM, &CFWMTestDlg::OnBnClickedStartfwm)
	ON_BN_CLICKED(IDC_LOADTUP, &CFWMTestDlg::OnBnClickedLoadtup)
	ON_BN_CLICKED(IDC_LOADIM, &CFWMTestDlg::OnBnClickedLoadim)
	ON_BN_CLICKED(IDCDISPIM, &CFWMTestDlg::OnBnClickedCdispim)
	ON_BN_CLICKED(IDCDISPIRBP, &CFWMTestDlg::OnBnClickedCdispirbp)
	ON_BN_CLICKED(IDCDISPIRBS, &CFWMTestDlg::OnBnClickedCdispirbs)
	ON_BN_CLICKED(IDC_ALLOC, &CFWMTestDlg::OnBnClickedAlloc)
	ON_BN_CLICKED(IDC_STARTFWM2, &CFWMTestDlg::OnBnClickedStartfwm2)
	ON_BN_CLICKED(IDC_FWMSPP, &CFWMTestDlg::OnBnClickedFwmspp)
END_MESSAGE_MAP()


// CFWMTestDlg message handlers

BOOL CFWMTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

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
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	//imopen = 1;
	thr = 75;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFWMTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFWMTestDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFWMTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}





void CFWMTestDlg::OnBnClickedStartfwm()
{
	HTuple tl, h, w, t, hiptr, hiptrIRB, hiptrCLC;

	GetImagePointer1(ho_ImCLC, &hiptrCLC, &t, &w, &h);
	GetImagePointer1(ho_ImIRBP, &hiptrIRB, &t, &w, &h);
	GetImagePointer1(ho_Im, &hiptr, &t, &w, &h);
	pImCLC = (UINT16 *)(Hlong *)hiptrCLC.L();
	pImIRBP = (UINT16 *)(Hlong *)hiptrIRB.L();
	pIm = (UINT8 *)(Hlong *)hiptr.L();
	int iw = w.L();
	int ih = h.L();

	//	FWM_BR_CPU_LaunchN(mmpIm, mmpImIRB, mmpImIRBS, mmpImCLC, iw, ih, 0, mmNp, mmthr, 0, mmpP1x, mmpP1y, mmpP2x, mmpP2y, mmpPnum, mmpWidth, mmpWmin, mmpPInt1x, mmpPInt1y, mmpPInt2x, mmpPInt2y, mmpShift, mmpPuseint);
	dclcmax = 72;
	thr = 75;
	CPU_MSPtProc39B2(pIm, pImIRBP, pImCLC, iw, ih, 0, Np,
		thr, dclcmax, pP1x, pP1y, pP2x, pP2y, pPnum, pPnum2,
		pWidth, pWmin, pPInt1x, pPInt1y, pPInt2x, pPInt2y, pShift, pPuseint);

	HTuple PWidth_tuple((float *)pWidth, Np * 1);
	WriteTuple(PWidth_tuple, hv_path + "PWidth_tuple.tup");
	AfxMessageBox(_T("Done...PWidth_tuple.tup saved!"));
}

void CFWMTestDlg::OnBnClickedStartfwm2()
{
	HTuple tl, h, w, t, hiptr, hiptrIRBS, hiptrCLCS;

	GetImagePointer1(ho_ImCLC, &hiptrCLCS, &t, &w, &h);
	GetImagePointer1(ho_ImIRBS, &hiptrIRBS, &t, &w, &h);
	GetImagePointer1(ho_Im, &hiptr, &t, &w, &h);
	pImCLCS = (UINT16 *)(Hlong *)hiptrCLCS.L();
	pImIRBS = (UINT16 *)(Hlong *)hiptrIRBS.L();
	pIm = (UINT8 *)(Hlong *)hiptr.L();
	int iw = w.L();
	int ih = h.L();

	//	FWM_BR_CPU_LaunchN(mmpIm, mmpImIRB, mmpImIRBS, mmpImCLC, iw, ih, 0, mmNp, mmthr, 0, mmpP1x, mmpP1y, mmpP2x, mmpP2y, mmpPnum, mmpWidth, mmpWmin, mmpPInt1x, mmpPInt1y, mmpPInt2x, mmpPInt2y, mmpShift, mmpPuseint);
	dclcmax = 72;
	thr = 75;
	CPU_MSSpProc36B2(pIm, pImIRBS, pImCLCS, iw, ih, 0, NpS,
		thr, dclcmax, pP1Sx, pP1Sy, pP2Sx, pP2Sy, pPnumS, pPnum2S,
		pWidthS, pWminS, pPInt1Sx, pPInt1Sy, pPInt2Sx, pPInt2Sy, pShiftS, pPuseintS);

	HTuple PWidthS_tuple((float *)pWidthS, NpS * 1);
	WriteTuple(PWidthS_tuple, hv_path + "PWidthS_tuple.tup");
	AfxMessageBox(_T("Done...PWidthS_tuple.tup saved!"));
	int nvoid = 7; ///  not used points number in the begining and end of skeleton part
	int lMBS = 0;
	int sz1 = 50;
	int sz2 = 50;			// Running median sizes for FWM and FWMS
	float ang = 3.;						// angle for defect 
	int rmskip = 0; /// skip in run. med.
	int dlen = 3;
	float prcMBS = 30.; 		// Prc. nearby and gerb for MB Space
	float absMBS = 10.;
	float minMBS = 1.5;

	//float prcMB = 40.;
	//float prcSP = 40.;		// Prc. for MB ans SP
	//float absMB = 10.;
	//
	int lMB = 1;
	int lSP = 1;				// defect length for MB and SP
	float prcSPS = 100.;
	float absSPS = 100.;
	int SPnumS = 0, MBnumS = 0;
	int spnum, mbnum;
	// Post processing (finding defects)
	FWMS_PostProcCPU(pWidthS, pWGS, pmedoS, pP3Sx, pP3Sy, NaCS, NpS, nvoid, pPaddrCS, sz1, sz2, rmskip, ang, prcMBS, prcSPS, absMBS, absSPS, minMBS, dlen, pDefMBS, pXdefMBS, pYdefMBS, pDefSPS, pXdefSPS, pYdefSPS, &MBnumS, &SPnumS);
	HTuple medoS_tuple((float *)pmedoS, 7472);
	WriteTuple(medoS_tuple, "C:\\Samsung\\medoS_tuple.tup");
	AfxMessageBox(_T("WidthS tuple saved!"));

	//HTuple hv_RowMBSs=ConvertFloatToTuple(pYdefMBS, MBnumS);
	//HTuple hv_ColumnMBSs = ConvertFloatToTuple(pXdefMBS, MBnumS);
	INT64 *PSx = ConvertFloatToINT64(pXdefMBS, MBnumS);
	INT64 *PSy = ConvertFloatToINT64(pYdefMBS, MBnumS);

	HTuple PMBSx_tuple;
	HTuple PMBSy_tuple;

	for (int i = 0; i < MBnumS; i++)
	{
		PMBSx_tuple[i] = (HTuple)*(PSx + i);
		PMBSy_tuple[i] = (HTuple)*(PSy + i);
	}

	//float *buff;
	HTuple hv_Buff = HTuple(PSx, MBnumS);
	/*HTuple PMBSx_tuple((Hlong *)PSx, MBnumS*8);
	HTuple PMBSy_tuple((Hlong *)PSy, MBnumS*8);*/

	HTuple hv_TR;
	HTuple  hv_nMBS = HTuple(MBnumS);

	HObject CircleMBSs;
	TupleGenConst(hv_nMBS, 30, &hv_TR);
	GenCircle(&CircleMBSs, PMBSy_tuple, PMBSx_tuple, hv_TR);
	//Disp
	if (HDevWindowStack::IsOpen())
		SetColor(HDevWindowStack::GetActive(), "red");
	SetDraw(HDevWindowStack::GetActive(), "margin");
	DispObj(CircleMBSs, HDevWindowStack::GetActive());
}



void CFWMTestDlg::OnBnClickedLoadtup()
{
	HTuple tl;
	int v;
	HTuple dir;
	//dir = "\\PATTERN\\NT3\\";
	dir = hv_path;
	//   for FWM
	ReadTuple(dir + "PaddrC.tup", &hv_PaddrC);
	pPaddrC = ConvertTupleToInt(hv_PaddrC);
	TupleLength(hv_PaddrC, &tl);
	NaC = tl.L();

	ReadTuple(dir + "PaddrR.tup", &hv_PaddrR);
	pPaddrR = ConvertTupleToInt(hv_PaddrR);
	ReadTuple(dir + "Pnum.tup", &hv_Pnum);
	pPnum = ConvertTupleToInt(hv_Pnum);
	TupleLength(hv_Pnum, &tl);
	Np = tl.L();
	ReadTuple(dir + "Pnum2.tup", &hv_Pnum2);
	pPnum2 = ConvertTupleToInt(hv_Pnum2);
	ReadTuple(dir + "X1.tup", &hv_P1x);
	pP1x = ConvertTupleToFloat(hv_P1x);
	ReadTuple(dir + "Y1.tup", &hv_P1y);
	pP1y = ConvertTupleToFloat(hv_P1y);
	ReadTuple(dir + "X2.tup", &hv_P2x);
	pP2x = ConvertTupleToFloat(hv_P2x);
	ReadTuple(dir + "Y2.tup", &hv_P2y);
	pP2y = ConvertTupleToFloat(hv_P2y);
	ReadTuple(dir + "XC.tup", &hv_P3x);
	pP3x = ConvertTupleToFloat(hv_P3x);
	ReadTuple(dir + "YC.tup", &hv_P3y);
	TupleMin(hv_P3y, &tl);
	pP3y = ConvertTupleToFloat(hv_P3y);
	/*for (int i = 0; i < NaC; i++)
		if (v = *(pP3y + i) < 0)
			i = i;*/
	ReadTuple(dir + "WG.tup", &hv_WG);
	pWG = ConvertTupleToInt(hv_WG);

	ReadTuple(dir + "PuseintP.tup", &hv_Puseint);
	pPuseint = ConvertTupleToInt(hv_Puseint);

	///   for FWMS
	ReadTuple(dir + "PaddrCS.tup", &hv_PaddrCS);
	pPaddrCS = ConvertTupleToInt(hv_PaddrCS);
	TupleLength(hv_PaddrCS, &tl);
	NaCS = tl.L();

	ReadTuple(dir + "PaddrRS.tup", &hv_PaddrRS);
	pPaddrRS = ConvertTupleToInt(hv_PaddrRS);
	ReadTuple(dir + "PnumS.tup", &hv_PnumS);
	pPnumS = ConvertTupleToInt(hv_PnumS);
	TupleLength(hv_PnumS, &tl);
	NpS = tl.L();
	ReadTuple(dir + "Pnum2S.tup", &hv_Pnum2S);
	pPnum2S = ConvertTupleToInt(hv_Pnum2S);
	ReadTuple(dir + "X1S.tup", &hv_P1Sx);
	pP1Sx = ConvertTupleToFloat(hv_P1Sx);
	ReadTuple(dir + "Y1S.tup", &hv_P1Sy);
	pP1Sy = ConvertTupleToFloat(hv_P1Sy);
	ReadTuple(dir + "X2S.tup", &hv_P2Sx);
	pP2Sx = ConvertTupleToFloat(hv_P2Sx);
	ReadTuple(dir + "Y2S.tup", &hv_P2Sy);
	pP2Sy = ConvertTupleToFloat(hv_P2Sy);
	ReadTuple(dir + "XCS.tup", &hv_P3Sx);
	pP3Sx = ConvertTupleToFloat(hv_P3Sx);
	ReadTuple(dir + "YCS.tup", &hv_P3Sy);
	pP3Sy = ConvertTupleToFloat(hv_P3Sy);
	ReadTuple(dir + "WGS.tup", &hv_WGS);
	pWGS = ConvertTupleToInt(hv_WGS);

	AfxMessageBox(_T("Teaching tuples are loaded!"));
	/*ReadTuple(hp.hv_drv + dir+"PregN.tup", &hv_PregN);
	pPregN = ConvertTupleToInt(hv_PregN);
	ReadTuple(hp.hv_drv + dir+"PindCN.tup", &hv_PindCN);
	pPindCN = ConvertTupleToInt(hv_PindCN);*/

	//(CButton*)(AfxGetMainWnd()->GetDlgItem(IDC_SAVEF))->EnableWindow(TRUE);
}
float * CFWMTestDlg::ConvertTupleToFloat(HTuple T)
{
	double *dptr, d;
	//float f;
	HTuple tl;
	float *dp;

	TupleLength(T, &tl);
	int itl = tl.L();

	dptr = T.ToDArr();
	//iptr = hv_tst.ToIArr();
	dp = (float *)malloc(itl * sizeof(float));
	for (int i = 0; i < itl; i++)
	{
		d = *(dptr + i);
		*(dp + i) = d;
	}
	return dp;
}

HTuple CFWMTestDlg::ConvertFloatToTuple(float  *dp, int len)  /// not working yet!!
{
	double *dptr, d;
	//float f;
	HTuple tl;
	//float *dp;
	HTuple T;

	//TupleLength(T, &tl);
	//int itl = tl.L();

	dptr = T.ToDArr();
	//iptr = hv_tst.ToIArr();
	//dp = (float *)malloc(itl * sizeof(float));
	for (int i = 0; i < len; i++)
	{
		d = *(dptr + i);
		*(dp + i) = d;
	}
	return T;
}

INT64 * CFWMTestDlg::ConvertFloatToINT64(float *dp, int len)
{
	INT64 *dptr;
	int d;
	//float f;
	HTuple tl;
	//INT64 *dp;

	//TupleLength(T, &tl);
	int itl = len;

	//dptr = tl.ToDArr();
	//iptr = hv_tst.ToIArr();
	dptr = (INT64 *)malloc(itl * sizeof(INT64));
	for (int i = 0; i < itl; i++)
	{
		d = *(dp + i);
		*(dptr + i) = d;
	}
	return dptr;
}

double * CFWMTestDlg::ConvertFloatToDouble(float *dp, int len)
{
	double *dptr;
	int d;
	HTuple tl;
	int itl = len;

	dptr = (double *)malloc(itl * sizeof(double));
	for (int i = 0; i < itl; i++)
	{
		d = *(dp + i);
		*(dptr + i) = d;
	}
	return dptr;
}

double * CFWMTestDlg::ConvertINT64ToDouble(UINT64 *dp, int len)
{
	double *dptr;
	INT64 d;
	HTuple tl;
	int itl = len;

	dptr = (double *)malloc(itl * sizeof(double));
	for (int i = 0; i < itl; i++)
	{
		d = *(dp + i);
		*(dptr + i) = d;
	}
	return dptr;
}

double * CFWMTestDlg::ConvertIntToDouble(int *dp, int len)
{
	double *dptr;
	int d;
	HTuple tl;
	int itl = len;

	dptr = (double *)malloc(itl * sizeof(double));
	for (int i = 0; i < itl; i++)
	{
		d = *(dp + i);
		*(dptr + i) = d;
	}
	return dptr;
}

INT64 * CFWMTestDlg::ConvertIntToINT64(int *dp, int len)
{
	INT64 *dptr;
	int d;
	//float f;
	HTuple tl;

	int itl = len;

	//dptr = tl.ToDArr();
	//iptr = hv_tst.ToIArr();
	dptr = (INT64 *)malloc(itl * sizeof(INT64));
	for (int i = 0; i < itl; i++)
	{
		d = *(dp + i);
		*(dptr + i) = d;
	}
	return dptr;
}


int * CFWMTestDlg::ConvertTupleToInt(HTuple T)
{
	int *iptr, id;
	Hlong *hlptr;
	//float f;
	HTuple tl;
	int *ip;

	TupleLength(T, &tl);
	int itl = tl.L();

	hlptr = T.ToLArr();
	//iptr = hv_tst.ToIArr();
	ip = (int *)malloc(itl * sizeof(int));
	for (int i = 0; i < itl; i++)
	{
		id = (int)*(hlptr + i);
		*(ip + i) = id;
	}
	return ip;
}

void CFWMTestDlg::OnBnClickedLoadim()
{
	HTuple path;
	path = hv_pathd;
	ReadImage(&ho_Im, path + "Im.tif");
	ReadImage(&ho_ImCLC, path + "ImCLC.tif");
	ReadImage(&ho_ImIRBP, path + "ImIRBP.tif");
	//ReadImage(&hp.ho_ImIRB, path + "ImIRBo.tif");
	ReadImage(&ho_ImIRBS, path + "ImIRBS.tif");
	//ReadImage(&ho_ImCLCS, path + "ImCLCS.tif");
	//int div = 28;
	float div = 1.5;
	HTuple w, h;
	GetImageSize(ho_Im, &w, &h);
	//if (!HDevWindowStack::IsOpen())
	if (imopen != 1)
	{
		SetWindowAttr("background_color", "black");
		//OpenWindow(0, 0, 400, 800, 0, "visible", "", &hv_WindowHandle);
		SetWindowAttr("window_title", "Image");
		OpenWindow(0, 0, w / div, h / div, 0, "visible", "", &hv_WindowHandle);
		HDevWindowStack::Push(hv_WindowHandle);
		imopen = 1;
	}
	//SetPart(hv_WindowHandle, h/2, w/2, h - 1, w - 1);
	SetPart(hv_WindowHandle, 0, 0, h - 1, w - 1);
	DispObj(ho_Im, HDevWindowStack::GetActive());
	//AfxMessageBox(_T("Teaching tuples are loaded!"));
}


void CFWMTestDlg::OnBnClickedCdispim()
{
	SetWindowAttr("window_title", "Input Image");
	DispObj(ho_Im, HDevWindowStack::GetActive());
}


void CFWMTestDlg::OnBnClickedCdispirbp()
{
	SetWindowAttr("window_title", "Image IRBP");
	DispObj(ho_ImIRBP, HDevWindowStack::GetActive());
}


void CFWMTestDlg::OnBnClickedCdispirbs()
{
	SetWindowAttr("window_title", "Image IRBS");
	DispObj(ho_ImIRBS, HDevWindowStack::GetActive());
}


void CFWMTestDlg::OnBnClickedAlloc()
{

	/// Pattern
	if (pWidth == 0)
		pWidth = (float *)malloc(Np * 1 * sizeof(float));
	if (pWmin == 0)
		pWmin = (float *)malloc(Np * 1 * sizeof(float));
	if (pPInt1x == 0)
		pPInt1x = (float *)malloc(Np * 2 * sizeof(UINT64));
	if (pPInt1y == 0)
		pPInt1y = (float *)malloc(Np * 2 * sizeof(UINT64));

	if (pPInt2x == 0)
		pPInt2x = (float *)malloc(Np * 2 * sizeof(UINT64));
	if (pPInt2y == 0)
		pPInt2y = (float *)malloc(Np * 2 * sizeof(UINT64));
	if (pShift == 0)
		pShift = (float *)malloc(Np * 1 * sizeof(UINT64));
	if (pPuseint == 0)
		pPuseint = (int *)malloc(Np * 1 * sizeof(UINT64));
	/// Space
	if (pWidthS == 0)
		pWidthS = (float *)malloc(NpS * 1 * sizeof(float));
	if (pWminS == 0)
		pWminS = (float *)malloc(NpS * 1 * sizeof(float));
	if (pPInt1Sx == 0)
		pPInt1Sx = (float *)malloc(NpS * 2 * sizeof(UINT64));
	if (pPInt1Sy == 0)
		pPInt1Sy = (float *)malloc(NpS * 2 * sizeof(UINT64));

	if (pPInt2Sx == 0)
		pPInt2Sx = (float *)malloc(NpS * 2 * sizeof(UINT64));
	if (pPInt2Sy == 0)
		pPInt2Sy = (float *)malloc(NpS * 2 * sizeof(UINT64));
	if (pShiftS == 0)
		pShiftS = (float *)malloc(NpS * 2 * sizeof(UINT64));
	if (pPuseintS == 0)
		pPuseintS = (int *)malloc(NpS * 1 * sizeof(UINT64));

	if (pmedoS != NULL)
	{
		free(pmedoS);
		pmedoS = NULL;
	}
	pmedoS = (float *)malloc(150000 * 1 * sizeof(float));

	if (pmedo != NULL)
	{
		free(pmedo);
		pmedo = NULL;
	}
	pmedo = (float *)malloc(150000 * 1 * sizeof(float));
}




void CFWMTestDlg::OnBnClickedFwmspp()
{
	// TODO: Add your control notification handler code here
}
