// Cube.h : main header file for the Cube application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols


// CCubeApp:
// See Cube.cpp for the implementation of this class
//

class CCubeApp : public CWinApp
{
public:
	CCubeApp();

	int m_MSAAPixelFormat;
    int m_Samples;

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	DECLARE_MESSAGE_MAP()

private:
    bool Init();
};

extern CCubeApp theApp;