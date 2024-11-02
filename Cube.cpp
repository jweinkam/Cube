// Cube.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Cube.h"
#include "MainFrm.h"

#include "CubeDoc.h"
#include "CubeView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCubeApp

BEGIN_MESSAGE_MAP(CCubeApp, CWinApp)
END_MESSAGE_MAP()


// CCubeApp construction

CCubeApp::CCubeApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CCubeApp object

CCubeApp theApp;

// CCubeApp initialization

BOOL CCubeApp::InitInstance()
{
	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	CWinApp::InitInstance();

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)
	
    if (!Init())
    {
        return FALSE;
    }

    // Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CCubeDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CCubeView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);
	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	return TRUE;
}

// CCubeApp message handlers

LRESULT CALLBACK WndProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uiMsg)
	{
		case WM_CLOSE:
			PostQuitMessage(0);
			break;

		default:
			return DefWindowProc(hWnd, uiMsg, wParam, lParam);
	}
	return 0;
}

bool CCubeApp::Init()
{
	WNDCLASSEX WndClassEx;

	memset(&WndClassEx, 0, sizeof(WNDCLASSEX));

	WndClassEx.cbSize = sizeof(WNDCLASSEX);
	WndClassEx.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	WndClassEx.lpfnWndProc = WndProc;
	WndClassEx.hInstance = m_hInstance;
	WndClassEx.hIcon = NULL;
	WndClassEx.hIconSm = NULL;
	WndClassEx.hCursor = NULL;
	WndClassEx.lpszClassName = "Win32OpenGLWindow";

	if(RegisterClassEx(&WndClassEx) == 0)
	{
		TRACE("RegisterClassEx Failed %x\r\n", GetLastError());
		return false;
	}

	DWORD Style = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	HWND hWnd = CreateWindowEx(WS_EX_APPWINDOW, WndClassEx.lpszClassName, "", Style, 0, 0, 100, 100, NULL, NULL, m_hInstance, NULL);

	if(hWnd == NULL)
	{
		TRACE("CreateWindowEx Failed %x\r\n", GetLastError());
		return false;
	}

	HDC hDC = GetDC(hWnd);

	if(hDC == NULL)
	{
		TRACE("GetDC Failed %x\r\n", GetLastError());
		return false;
	}

	PIXELFORMATDESCRIPTOR pfd;

	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.iLayerType = PFD_MAIN_PLANE;

	int PixelFormat = ChoosePixelFormat(hDC, &pfd);

	if(PixelFormat == 0)
	{
		TRACE("ChoosePixelFormat Failed %x\r\n", GetLastError());
		return false;
	}

    m_MSAAPixelFormat = 0;
	if(SetPixelFormat(hDC, m_MSAAPixelFormat == 0 ? PixelFormat : m_MSAAPixelFormat, &pfd) == FALSE)
	{
		TRACE("SetPixelFormat Failed %x\r\n", GetLastError());
		return false;
	}

	HGLRC hGLRC = wglCreateContext(hDC);

	if(hGLRC == NULL)
	{
		TRACE("wglCreateContext Failed %x\r\n", GetLastError());
		return false;
	}

	if(wglMakeCurrent(hDC, hGLRC) == FALSE)
	{
		TRACE("wglMakeCurrent Failed %x\r\n", GetLastError());
		return false;
	}

	if(glewInit() != GLEW_OK)
	{
		TRACE("glewInit Failed %x\r\n", GetLastError());
		return false;
	}

    m_Samples = 8;
	if(m_MSAAPixelFormat == 0 && m_Samples > 0)
	{
		if(WGLEW_ARB_pixel_format && GLEW_ARB_multisample)
		{
			while(m_Samples > 0)
			{
				UINT NumFormats = 0;

				int PFAttribs[] =
				{
					WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
					WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
					WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
					WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
					WGL_COLOR_BITS_ARB, 32,
					WGL_DEPTH_BITS_ARB, 24,
					WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
					WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
					WGL_SAMPLES_ARB, m_Samples,
					0
				};

				if(wglChoosePixelFormatARB(hDC, PFAttribs, NULL, 1, &m_MSAAPixelFormat, &NumFormats) == TRUE && NumFormats > 0) break;

				m_Samples--;
			}
        }
		else
		{
			m_Samples = 0;
		}
	}

	wglDeleteContext(hGLRC);
	DestroyWindow(hWnd);
	UnregisterClass(WndClassEx.lpszClassName, m_hInstance);
    return true;
}

