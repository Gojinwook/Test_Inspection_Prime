
// FWMTest.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#pragma once
#include "HalconCpp.h"
#include "HDevThread.h"
//#include "HProc.h"
//#include "FWM.h"
//#include "NonFWM.h"
//#include "TeachingFWMnFWM.h"
//#include "Proc.h"
//#include "Plots.h"
//#include "Mil.h"
//#include "TimeCounter.h"
//#include "./util/XIni.h"

using namespace HalconCpp;


// CFWMTestApp:
// See FWMTest.cpp for the implementation of this class
//

class CFWMTestApp : public CWinApp
{
public:
	CFWMTestApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CFWMTestApp theApp;
