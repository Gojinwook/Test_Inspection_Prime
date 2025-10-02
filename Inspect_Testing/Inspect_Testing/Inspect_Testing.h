
// Inspect_Testing.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CInspectTestingApp:
// See Inspect_Testing.cpp for the implementation of this class
//

class CInspectTestingApp : public CWinApp
{
public:
	CInspectTestingApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CInspectTestingApp theApp;
