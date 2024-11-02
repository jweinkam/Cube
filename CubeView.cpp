// CubeView.cpp : implementation of the CCubeView class
//

#include "stdafx.h"
#include "Cube.h"

#include "CubeDoc.h"
#include "CubeView.h"
#include "MainFrm.h"
#include <math.h>
#include <process.h>
#include <share.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const double g_dAngleX = 45.0;
const double g_dAngleY = -acos( sqrt(2.0) / sqrt(3.0) ) * 45.0 / atan(1.0);
const double g_dAngleZ = -30.0;
const double g_dAngleY1 = atan( 1.0/5.5 ) * 45.0 / atan(1.0);
const double g_dAngleY2 = atan( 2.0/5.5 ) * 45.0 / atan(1.0); 

int CCubeView::m_defaultOverride[13] = { 0, 3, 24, 27, 64, 67, 88, 173, 181, 236, 247, 164, 91 };

PALETTEENTRY CCubeView::m_defaultPalEntry[20] =
{
    { 0,   0,   0,    0 }, //0
    { 0x80,0,   0,    0 }, 
    { 0,   0x80,0,    0 }, 
    { 0x80,0x80,0,    0 }, 
    { 0,   0,   0x80, 0 },
    { 0x80,0,   0x80, 0 },
    { 0,   0x80,0x80, 0 },
    { 0xC0,0xC0,0xC0, 0 }, //7

    { 192, 220, 192,  0 }, //8
    { 166, 202, 240,  0 },
    { 255, 251, 240,  0 },
    { 160, 160, 164,  0 }, //11

    { 0x80,0x80,0x80, 0 }, //12
    { 0xFF,0,   0,    0 },
    { 0,   0xFF,0,    0 },
    { 0xFF,0xFF,0,    0 },
    { 0,   0,   0xFF, 0 },
    { 0xFF,0,   0xFF, 0 },
    { 0,   0xFF,0xFF, 0 },
    { 0xFF,0xFF,0xFF, 0 }  //19
};

static GLdouble colors[7][3] =
{
    {3.0, 3.0, 3.0},
    {3.0, 1.5, 0.0},
    {0.0, 3.0, 0.0},
    {3.0, 0.0, 0.0},
    {3.0, 3.0, 0.0},
    {0.0, 0.0, 3.0},
    {0.5, 0.5, 0.5}
}; 

CCubeView::CenterRelativePositions CCubeView::m_CentersPositions[6][6] =
{
    { // 0
        { 0,   0,   0, 0, 0, false},
        { 270, 0,   4, 3, 1, false},
        { 0,   0,   4, 1, 2, false},
        { 90,  0,   4, 2, 0, false},
        { 0,   0,   4, 1, 2, true},
        { 180, 180, 4, 0, 3, false}
    },

    { // 1
        { 180, 90, 1, 2, 4, false},
        { 0,   0,  0, 0, 0, false},
        { 90,  90, 1, 5, 2, false},
        { 90,  90, 1, 5, 2, true},
        { 0,   90, 1, 3, 5, false},
        { 270, 90, 1, 4, 3, false}
    },

    {  // 2
        { 180, 180, 2, 0, 4, false},
        { 270, 270, 2, 4, 1, false},
        { 0,   0,   0, 0, 0, false},
        { 90,  90,  2, 5, 0, false},
        { 0,   0,   2, 1, 5, false},
        { 90,  270, 2, 5, 0, true},
    },

    { // 3
        { 180, 270, 0, 3, 4, false},
        { 270, 270, 0, 4, 2, true},
        { 270, 270, 0, 4, 2, false},
        { 0,   0,   0, 0, 0, false},
        { 0,   270, 0, 2, 5, false},
        { 90,  270, 0, 5, 3, false}
    },

    {  // 4
        { 0,   0,   5, 1, 3, true},
        { 270, 180, 5, 2, 1, false},
        { 180, 180, 5, 0, 2, false},
        { 90,  180, 5, 3, 0, false},
        { 0,   0,   0, 0, 0, false},
        { 0,   0,   5, 1, 3, false}
    },

    { // 5
        { 0,   0,   3, 1, 4, false},
        { 270, 90,  3, 5, 1, false},
        { 0,   0,   3, 1, 4, true},
        { 90,  270, 3, 4, 0, false},
        { 180, 180, 3, 0, 5, false},
        { 0,   0,   0, 0, 0, false}
    }
};

CCubeView::CornerPositions CCubeView::m_CornerPositions[8] = 
{
    {0, 0, 0, 1, 0, 0, 5, 1, 0},
    {0, 0, 1, 5, 1, 1, 3, 0, 1},
    {0, 1, 1, 3, 0, 0, 2, 0, 1},
    {0, 1, 0, 2, 0, 0, 1, 0, 1},
    {4, 0, 0, 1, 1, 1, 2, 1, 0},
    {4, 0, 1, 2, 1, 1, 3, 1, 0},
    {4, 1, 1, 3, 1, 1, 5, 0, 1},
    {4, 1, 0, 5, 0, 0, 1, 1, 0}
};

CCubeView::CornerMoves CCubeView::m_CornerMoves[8][8][3] =
{
    {
        { {0, 0, 0, -1, 0, -1, 0, -1, 0}, {5, 1, 3, -1, 0, -1, 0, -1, 0}, {2, 0, 4, -1, 0, -1, 0, -1, 0} }, // done
        { {4, 0, 1,  3, 1,  1, 1, -1, 0}, {4, 1, 1, -1, 0, -1, 0, -1, 0}, {3, 1, 2,  0, 0,  0, 0,  5, 0} }, // done
        { {3, 5, 1,  2, 0,  5, 0, -1, 0}, {4, 1, 2,  3, 1, -1, 0, -1, 0}, {4, 3, 2, -1, 0, -1, 0, -1, 0} }, // done
        { {4, 3, 2,  0, 0,  2, 0, -1, 0}, {1, 2, 1,  2, 0,  2, 0,  4, 1}, {4, 2, 2, -1, 0, -1, 0, -1, 0} }, // done
        { {4, 0, 1,  5, 0,  0, 0, -1, 0}, {1, 3, 2,  4, 1, -1, 0, -1, 0}, {1, 4, 2, -1, 0, -1, 0, -1, 0} }, // done
        { {4, 3, 2,  3, 1, -1, 0, -1, 0}, {3, 5, 1,  5, 0, -1, 0, -1, 0}, {3, 0, 1,  5, 0,  5, 0, -1, 0} }, // done
        { {4, 3, 2,  4, 1,  3, 1, -1, 0}, {3, 5, 1, -1, 0, -1, 0, -1, 0}, {3, 0, 1,  5, 0, -1, 0, -1, 0} }, // done
        { {4, 3, 2,  5, 0,  5, 0,  3, 1}, {1, 3, 2, -1, 0, -1, 0, -1, 0}, {3, 0, 1, -1, 0, -1, 0, -1, 0} }  // done
    },

    {
        { {0, 0, 0, -1, 0, -1, 0, -1, 0}, {0, 0, 0, -1, 0, -1, 0, -1, 0}, {0, 0, 0, -1, 0, -1, 0, -1, 0} }, // done
        { {0, 0, 0, -1, 0, -1, 0, -1, 0}, {5, 3, 0, -1, 0, -1, 0, -1, 0}, {1, 2, 4, -1, 0, -1, 0, -1, 0} }, // done
        { {4, 2, 1,  0, 1,  3, 1, -1, 0}, {4, 3, 1, -1, 0, -1, 0, -1, 0}, {0, 3, 2,  2, 0,  2, 0,  5, 0} }, // done
        { {0, 5, 1,  1, 0,  5, 0, -1, 0}, {4, 3, 2,  0, 1, -1, 0, -1, 0}, {4, 0, 2, -1, 0, -1, 0, -1, 0} }, // done
        { {4, 0, 2,  0, 1, -1, 0, -1, 0}, {0, 5, 1,  5, 0, -1, 0, -1, 0}, {0, 2, 1,  5, 0,  5, 0, -1, 0} }, // done
        { {4, 0, 2,  4, 1,  0, 1, -1, 0}, {0, 5, 1, -1, 0, -1, 0, -1, 0}, {0, 2, 1,  5, 0, -1, 0, -1, 0} }, // done
        { {4, 0, 2,  5, 0,  5, 0,  0, 1}, {0, 5, 1,  4, 1, -1, 0, -1, 0}, {0, 2, 1, -1, 0, -1, 0, -1, 0} }, // done
        { {4, 0, 2,  5, 0,  0, 1, -1, 0}, {0, 5, 1,  4, 1,  4, 1, -1, 0}, {0, 2, 1,  4, 1, -1, 0, -1, 0} }  // done
    },

    {
        { {0, 0, 0, -1, 0, -1, 0, -1, 0}, {0, 0, 0, -1, 0, -1, 0, -1, 0}, {0, 0, 0, -1, 0, -1, 0, -1, 0} }, // done
        { {0, 0, 0, -1, 0, -1, 0, -1, 0}, {0, 0, 0, -1, 0, -1, 0, -1, 0}, {0, 0, 0, -1, 0, -1, 0, -1, 0} }, // done
        { {0, 0, 0, -1, 0, -1, 0, -1, 0}, {5, 0, 2, -1, 0, -1, 0, -1, 0}, {3, 1, 4, -1, 0, -1, 0, -1, 0} }, // done
        { {2, 1, 2, -1, 0, -1, 0, -1, 0}, {2, 0, 2,  5, 0,  1, 0, -1, 0}, {4, 1, 1,  1, 0,  1, 0,  2, 1} }, // done
        { {4, 2, 2,  4, 1,  2, 1, -1, 0}, {2, 5, 1, -1, 0, -1, 0, -1, 0}, {2, 1, 1,  5, 0, -1, 0, -1, 0} }, // done
        { {4, 2, 2,  4, 1,  4, 1,  2, 1}, {2, 5, 1,  4, 1, -1, 0, -1, 0}, {2, 1, 1, -1, 0, -1, 0, -1, 0} }, // done
        { {4, 2, 2,  5, 0,  2, 1, -1, 0}, {2, 5, 1,  4, 1,  4, 1, -1, 0}, {2, 1, 1,  4, 1, -1, 0, -1, 0} }, // done
        { {4, 2, 2,  2, 1, -1, 0, -1, 0}, {2, 5, 1,  5, 0, -1, 0, -1, 0}, {2, 1, 1,  4, 1,  4, 1, -1, 0} }  // done
    },

    {
        { {0, 0, 0, -1, 0, -1, 0, -1, 0}, {0, 0, 0, -1, 0, -1, 0, -1, 0}, {0, 0, 0, -1, 0, -1, 0, -1, 0} }, // done
        { {0, 0, 0, -1, 0, -1, 0, -1, 0}, {0, 0, 0, -1, 0, -1, 0, -1, 0}, {0, 0, 0, -1, 0, -1, 0, -1, 0} }, // done
        { {0, 0, 0, -1, 0, -1, 0, -1, 0}, {0, 0, 0, -1, 0, -1, 0, -1, 0}, {0, 0, 0, -1, 0, -1, 0, -1, 0} }, // done
        { {0, 0, 0, -1, 0, -1, 0, -1, 0}, {0, 4, 2, -1, 0, -1, 0, -1, 0}, {3, 5, 1, -1, 0, -1, 0, -1, 0} }, // done
        { {5, 0, 1,  5, 0,  5, 0,  3, 1}, {2, 5, 2, -1, 0, -1, 0, -1, 0}, {1, 2, 2,  4, 1, -1, 0, -1, 0} }, // done
        { {2, 4, 2,  0, 1,  5, 1, -1, 0}, {2, 0, 1, -1, 0, -1, 0, -1, 0}, {1, 4, 1,  4, 1, -1, 0, -1, 0} }, // done
        { {3, 4, 1,  0, 1,  0, 1, -1, 0}, {2, 0, 1,  4, 1, -1, 0, -1, 0}, {1, 4, 1,  4, 1,  4, 1, -1, 0} }, // done
        { {2, 5, 1,  4, 1,  0, 0,  0, 0}, {2, 0, 1,  4, 1,  4, 1, -1, 0}, {1, 4, 1,  5, 0,  0, 0, -1, 0} }  // done
    },

    {
        { {0, 0, 0, -1, 0, -1, 0, -1, 0}, {0, 0, 0, -1, 0, -1, 0, -1, 0}, {0, 0, 0, -1, 0, -1, 0, -1, 0} }, // done
        { {0, 0, 0, -1, 0, -1, 0, -1, 0}, {0, 0, 0, -1, 0, -1, 0, -1, 0}, {0, 0, 0, -1, 0, -1, 0, -1, 0} }, // done
        { {0, 0, 0, -1, 0, -1, 0, -1, 0}, {0, 0, 0, -1, 0, -1, 0, -1, 0}, {0, 0, 0, -1, 0, -1, 0, -1, 0} }, // done
        { {0, 0, 0, -1, 0, -1, 0, -1, 0}, {0, 0, 0, -1, 0, -1, 0, -1, 0}, {0, 0, 0, -1, 0, -1, 0, -1, 0} }, // done
        { {0, 0, 0, -1, 0, -1, 0, -1, 0}, {4, 1, 2, -1, 0, -1, 0, -1, 0}, {3, 0, 5, -1, 0, -1, 0, -1, 0} }, // done
        { {5, 0, 1,  2, 1,  1, 1, -1, 0}, {5, 1, 1, -1, 0, -1, 0, -1, 0}, {2, 1, 2,  0, 0,  0, 0,  4, 0} }, // done
        { {2, 4, 1,  3, 0,  4, 0, -1, 0}, {5, 1, 2,  2, 1, -1, 0, -1, 0}, {5, 2, 2, -1, 0, -1, 0, -1, 0} }, // done
        { {5, 2, 2,  0, 0,  3, 0, -1, 0}, {1, 3, 1,  3, 0,  3, 0,  5, 1}, {5, 3, 2, -1, 0, -1, 0, -1, 0} }  // done
    },

    {
        { {0, 0, 0, -1, 0, -1, 0, -1, 0}, {0, 0, 0, -1, 0, -1, 0, -1, 0}, {0, 0, 0, -1, 0, -1, 0, -1, 0} }, // done
        { {0, 0, 0, -1, 0, -1, 0, -1, 0}, {0, 0, 0, -1, 0, -1, 0, -1, 0}, {0, 0, 0, -1, 0, -1, 0, -1, 0} }, // done
        { {0, 0, 0, -1, 0, -1, 0, -1, 0}, {0, 0, 0, -1, 0, -1, 0, -1, 0}, {0, 0, 0, -1, 0, -1, 0, -1, 0} }, // done
        { {0, 0, 0, -1, 0, -1, 0, -1, 0}, {0, 0, 0, -1, 0, -1, 0, -1, 0}, {0, 0, 0, -1, 0, -1, 0, -1, 0} }, // done
        { {0, 0, 0, -1, 0, -1, 0, -1, 0}, {0, 0, 0, -1, 0, -1, 0, -1, 0}, {0, 0, 0, -1, 0, -1, 0, -1, 0} }, // done
        { {0, 0, 0, -1, 0, -1, 0, -1, 0}, {4, 2, 0, -1, 0, -1, 0, -1, 0}, {1, 3, 5, -1, 0, -1, 0, -1, 0} }, // done
        { {5, 3, 1,  0, 1,  2, 1, -1, 0}, {5, 2, 1, -1, 0, -1, 0, -1, 0}, {0, 2, 2,  3, 0,  3, 0,  4, 0} }, // done
        { {0, 4, 1,  1, 0,  4, 0, -1, 0}, {5, 2, 2,  0, 1, -1, 0, -1, 0}, {5, 0, 2, -1, 0, -1, 0, -1, 0} }  // done
    },

    {
        { {0, 0, 0, -1, 0, -1, 0, -1, 0}, {0, 0, 0, -1, 0, -1, 0, -1, 0}, {0, 0, 0, -1, 0, -1, 0, -1, 0} }, // done
        { {0, 0, 0, -1, 0, -1, 0, -1, 0}, {0, 0, 0, -1, 0, -1, 0, -1, 0}, {0, 0, 0, -1, 0, -1, 0, -1, 0} }, // done
        { {0, 0, 0, -1, 0, -1, 0, -1, 0}, {0, 0, 0, -1, 0, -1, 0, -1, 0}, {0, 0, 0, -1, 0, -1, 0, -1, 0} }, // done
        { {0, 0, 0, -1, 0, -1, 0, -1, 0}, {0, 0, 0, -1, 0, -1, 0, -1, 0}, {0, 0, 0, -1, 0, -1, 0, -1, 0} }, // done
        { {0, 0, 0, -1, 0, -1, 0, -1, 0}, {0, 0, 0, -1, 0, -1, 0, -1, 0}, {0, 0, 0, -1, 0, -1, 0, -1, 0} }, // done
        { {0, 0, 0, -1, 0, -1, 0, -1, 0}, {0, 0, 0, -1, 0, -1, 0, -1, 0}, {0, 0, 0, -1, 0, -1, 0, -1, 0} }, // done
        { {0, 0, 0, -1, 0, -1, 0, -1, 0}, {4, 0, 3, -1, 0, -1, 0, -1, 0}, {2, 1, 5, -1, 0, -1, 0, -1, 0} }, // done
        { {0, 0, 0, -1, 0, -1, 0, -1, 0}, {5, 2, 2,  0, 1, -1, 0, -1, 0}, {5, 0, 2, -1, 0, -1, 0, -1, 0} }  // done
    },
};

void OutputGlError(char* label) 
{
    GLenum errorno = glGetError();
    if (errorno != GL_NO_ERROR)
        TRACE("%s had error: #(%d) %s\r\n", label, errorno, gluErrorString(errorno));
}

IMPLEMENT_DYNCREATE(CCubeView, CView)

BEGIN_MESSAGE_MAP(CCubeView, CView)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_DESTROY()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_ERASEBKGND()
    ON_COMMAND(ID_EDIT_LESS, CCubeView::OnEditLess)
    ON_COMMAND(ID_EDIT_MORE, CCubeView::OnEditMore)
    ON_COMMAND(ID_EDIT_SCRAMBLE, CCubeView::OnEditScramble)
    ON_COMMAND(ID_EDIT_SOLVE, CCubeView::OnEditSolve)
    ON_COMMAND(ID_EDIT_REDO, CCubeView::OnEditRedo)
    ON_COMMAND(ID_EDIT_OPEN, CCubeView::OnEditOpen)
	ON_COMMAND(ID_VIEW_ROTATEX, CCubeView::OnViewRotateX)
    ON_COMMAND(ID_VIEW_ROTATECX, CCubeView::OnViewRotateCX)
	ON_COMMAND(ID_VIEW_ROTATEY, CCubeView::OnViewRotateY)
    ON_COMMAND(ID_VIEW_ROTATECY, CCubeView::OnViewRotateCY)
    ON_COMMAND(ID_VIEW_ROTATEZ, CCubeView::OnViewRotateZ)
	ON_COMMAND(ID_VIEW_ROTATECZ, CCubeView::OnViewRotateCZ)
    ON_COMMAND(ID_SPEED_SLOWER, CCubeView::OnSpeedSlower)
    ON_COMMAND(ID_SPEED_FASTER, CCubeView::OnSpeedFaster)
    ON_MESSAGE(WM_DISPLAY_STATUS, OnDisplayStatus)
    ON_MESSAGE(WM_DISPLAY_STATUS_2, OnDisplayStatus2)
    ON_MESSAGE(WM_DO_UP, OnDoUp)
END_MESSAGE_MAP()

// CCubeView construction/destruction

CCubeView::CCubeView()
{
    m_Abort = true;
    m_Thread = NULL;
    m_LogFile = _fsopen( "Cube.log", "a", _SH_DENYWR );

    m_lSleep = 1;
    m_dStep = 2;
    m_Speed = 7;

    m_AngleX = 0;
    m_AngleY = 0;
    m_AngleZ = 0;

    m_hrc = NULL;

    m_Size = 15;
    InitCube();

    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            for (int k = 0; k < 3; ++k)
            {
                m_CornerTableUsed[i][j][k] = false;
            }
        }
    }
}

CCubeView::~CCubeView()
{
    fprintf_s( m_LogFile, "CornerTableUsed\n" );
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            if ( j > 0)
            {
                fprintf_s( m_LogFile, ", " );
            }
            fprintf_s( m_LogFile, "(" );
            for (int k = 0; k < 3; ++k)
            {
                if ( k > 0)
                {
                    fprintf_s( m_LogFile, ", " );
                }
                fprintf_s( m_LogFile, "%i", (int)(m_CornerTableUsed[i][j][k]) );
            }
            fprintf_s( m_LogFile, ")" );
        }
        fprintf_s( m_LogFile, "\n" );
    }
    fclose( m_LogFile );
}

BOOL CCubeView::OnEraseBkgnd(CDC *pDC)
{
    return true;
}

void CCubeView::OnDraw(CDC* pDC)
{
    CCubeDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    if (!pDoc)
        return;

    BOOL bResult = wglMakeCurrent(pDC->m_hDC, m_hrc);
    if (!bResult)
    {
        TRACE("wglMakeCurrent Failed %x\r\n", GetLastError() );
    }

    glEnable(GL_LIGHTING);
    OutputGlError("glEnable (GL_LIGHTING);");

    glEnable(GL_LIGHT0);
    OutputGlError("glEnable (GL_LIGHT0);");

    glEnable(GL_DEPTH_TEST);
    OutputGlError("glEnable (GL_DEPTH_TEST);");

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f) ;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

    glMatrixMode(GL_MODELVIEW);
    OutputGlError("MatrixMode");

    GLdouble stickerSize = 0.45/(m_Size);
    GLdouble cubieSize = 0.5/(m_Size);

    bool bInclude3D1 = (m_WindowSizeX > 1.37 * m_WindowSizeY);
    bool bInclude3D2 = (m_WindowSizeX > 2.05 * m_WindowSizeY);
    int side;
    for ( side = 0; side < 6; ++side )
    {
        int x;
        for ( x=0; x < m_Size; ++x )
        {
            int y;
            for ( y = 0; y < m_Size; ++y )
            {
                glLoadIdentity();

                glTranslated( (bInclude3D1 ? (bInclude3D2 ? -2.0 : -1.0) : 0.0), 0.0, -5.5);

                double dScale = 2.0/3;

                switch ( side )
                {
                case 0:
                    glTranslated(0.0, 1.5*dScale, 0.0);

                    if ( m_AnglesX[m_Size-1-x] != 0 )
                    {
                        glTranslated(0.0, -m_AnglesX[m_Size-1-x]*dScale/90, 0.0);
                    }
                    if ( m_AnglesY[0] != 0 )
                    {
                        glRotated(m_AnglesY[0], 0.0, 0.0, 1.0);
                    }
                    if ( m_AnglesZ[m_Size-1-y] != 0 )
                    {
                        glTranslated(-m_AnglesZ[m_Size-1-y]*dScale/90, 0.0, 0.0);
                    }
                    break;
                case 1:
                    glTranslated(-dScale, 0.5*dScale, 0.0);

                    if ( m_AnglesX[m_Size-1] != 0 )
                    {
                        glRotated(-m_AnglesX[m_Size-1], 0.0, 0.0, 1.0);
                    }
                    if ( m_AnglesY[y] != 0 )
                    {
                        glTranslated(m_AnglesY[y]*dScale/90, 0.0, 0.0);
                    }
                    if ( m_AnglesZ[m_Size-1-x] != 0 )
                    {
                        glTranslated(0.0, -m_AnglesZ[m_Size-1-x]*dScale/90, 0.0);
                    }
                    break;
                case 2:
                    glTranslated(0.0, 0.5*dScale, 0.0);

                    if ( m_AnglesX[m_Size-1-x] != 0 )
                    {
                        glTranslated(0.0, -m_AnglesX[m_Size-1-x]*dScale/90, 0.0);
                    }
                    if ( m_AnglesY[y] != 0 )
                    {
                        glTranslated(m_AnglesY[y]*dScale/90, 0.0, 0.0);
                    }
                    if ( m_AnglesZ[0] != 0 )
                    {
                        glRotated(m_AnglesZ[0], 0.0, 0.0, 1.0);
                    }
                    break;
                case 3:
                    glTranslated(dScale, 0.5*dScale, 0.0);

                    if ( m_AnglesX[0] != 0 )
                    {
                        glRotated(m_AnglesX[0], 0.0, 0.0, 1.0);
                    }
                    if ( m_AnglesY[y] != 0 )
                    {
                        glTranslated(m_AnglesY[y]*dScale/90, 0.0, 0.0);
                    }
                    if ( m_AnglesZ[x] != 0 )
                    {
                        glTranslated(0.0, m_AnglesZ[x]*dScale/90, 0.0);
                    }
                    break;
                case 4:
                    glTranslated(0.0, -0.5*dScale, 0.0);

                    if ( m_AnglesX[m_Size-1-x] != 0 )
                    {
                        glTranslated(0.0, -m_AnglesX[m_Size-1-x]*dScale/90, 0.0);
                    }
                    if ( m_AnglesY[m_Size-1] != 0 )
                    {
                        glRotated(-m_AnglesY[m_Size-1], 0.0, 0.0, 1.0);
                    }
                    if ( m_AnglesZ[y] != 0 )
                    {
                        glTranslated(m_AnglesZ[y]*dScale/90, 0.0, 0.0);
                    }
                    break;
                case 5:
                    glTranslated(0.0, -1.5*dScale, 0.0);

                    if ( m_AnglesX[m_Size-1-x] != 0 )
                    {
                        glTranslated(0.0, -m_AnglesX[m_Size-1-x]*dScale/90, 0.0);
                    }
                    if ( m_AnglesY[m_Size-1-y] != 0 )
                    {
                        glTranslated(-m_AnglesY[m_Size-1-y]*dScale/90, 0.0, 0.0);
                    }
                    if ( m_AnglesZ[m_Size-1] != 0 )
                    {
                        glRotated(-m_AnglesZ[m_Size-1], 0.0, 0.0, 1.0);
                    }
                    break;
                }

                glTranslated((x-double(m_Size-1)/2)*dScale/m_Size, (double(m_Size-1)/2-y)*dScale/m_Size, 0.0);

                glColor3dv(colors[6]);

                glBegin(GL_POLYGON);
                glNormal3d(  0.0,  0.0,  cubieSize*dScale);
                glVertex3d( cubieSize*dScale,  cubieSize*dScale, 0); 
                glVertex3d(-cubieSize*dScale,  cubieSize*dScale, 0);            
                glVertex3d(-cubieSize*dScale, -cubieSize*dScale, 0);           
                glVertex3d( cubieSize*dScale, -cubieSize*dScale, 0);            
                glEnd();

                glTranslated(0.0, 0.0, 0.001);

                double weight = m_Sides[side][y*m_Size+x].m_Row * m_Size + m_Sides[side][y*m_Size+x].m_Column;
                weight = 0.5 + 0.5 * (weight / (m_Size*m_Size-1));
                GLdouble pieceColor[3];
                pieceColor[0] = colors[ m_Sides[side][y*m_Size+x].m_Color ][0] * weight;
                pieceColor[1] = colors[ m_Sides[side][y*m_Size+x].m_Color ][1] * weight;
                pieceColor[2] = colors[ m_Sides[side][y*m_Size+x].m_Color ][2] * weight;

                glColor3dv(pieceColor);

                if ( (x == m_Middle) && (y == m_Middle) )
                {
                    switch( m_SideAngles[side] )
                    {
                    case 0:
                        glBegin(GL_POLYGON);
                        glNormal3d(  0.0,  0.0,  stickerSize*dScale);
                        glVertex3d( stickerSize*dScale,  0, 0); 
                        glVertex3d(-stickerSize*dScale,  0, 0);            
                        glVertex3d(-stickerSize*dScale, -stickerSize*dScale, 0);           
                        glVertex3d( stickerSize*dScale, -stickerSize*dScale, 0);            
                        glEnd();
                        break;
                    case 1:
                        glBegin(GL_POLYGON);
                        glNormal3d(  0.0,  0.0,  stickerSize*dScale);
                        glVertex3d( stickerSize*dScale,  stickerSize*dScale, 0); 
                        glVertex3d( 0,                   stickerSize*dScale, 0);            
                        glVertex3d( 0,                  -stickerSize*dScale, 0);           
                        glVertex3d( stickerSize*dScale, -stickerSize*dScale, 0);            
                        glEnd();
                        break;
                    case 2:
                        glBegin(GL_POLYGON);
                        glNormal3d(  0.0,  0.0,  stickerSize*dScale);
                        glVertex3d( stickerSize*dScale,  stickerSize*dScale, 0); 
                        glVertex3d(-stickerSize*dScale,  stickerSize*dScale, 0);            
                        glVertex3d(-stickerSize*dScale,  0, 0);           
                        glVertex3d( stickerSize*dScale,  0, 0);            
                        glEnd();
                        break;
                    case 3:
                        glBegin(GL_POLYGON);
                        glNormal3d(  0.0,  0.0,  stickerSize*dScale);
                        glVertex3d( 0,                   stickerSize*dScale, 0); 
                        glVertex3d(-stickerSize*dScale,  stickerSize*dScale, 0);            
                        glVertex3d(-stickerSize*dScale, -stickerSize*dScale, 0);           
                        glVertex3d( 0,                  -stickerSize*dScale, 0);            
                        glEnd();
                        break;
                    }
                }
                else
                {
                    glBegin(GL_POLYGON);
                    glNormal3d(  0.0,  0.0,  stickerSize*dScale);
                    glVertex3d( stickerSize*dScale,  stickerSize*dScale, 0); 
                    glVertex3d(-stickerSize*dScale,  stickerSize*dScale, 0);            
                    glVertex3d(-stickerSize*dScale, -stickerSize*dScale, 0);           
                    glVertex3d( stickerSize*dScale, -stickerSize*dScale, 0);            
                    glEnd();
                }

                if (bInclude3D1)
                {
                    glLoadIdentity();

                    glTranslated((bInclude3D2 ? 0.0 : 1.0), 0.0, -5.5);

                    glRotated((bInclude3D2 ? 0.0 : -g_dAngleY1), 0.0, 1.0, 0.0);

                    glRotated(g_dAngleZ, 0.0, 0.0, 1.0);
                    glRotated(g_dAngleY, 0.0, 1.0, 0.0);
                    glRotated(g_dAngleX, 1.0, 0.0, 0.0);

                    glRotated(m_AngleZ, 0.0, 0.0, 1.0);
                    glRotated(m_AngleY, 0.0, 1.0, 0.0);
                    glRotated(m_AngleX, 1.0, 0.0, 0.0);

                    Draw3D(side, x, y, cubieSize, stickerSize);
                }

                if (bInclude3D2)
                {
                    glLoadIdentity();

                    glTranslated(2.0, 0.0, -5.5);

                    glRotated(180, 0.0, 1.0, 0.0);
                    glRotated(-g_dAngleY2, 0.0, 1.0, 0.0);

                    glRotated(g_dAngleZ, 0.0, 0.0, 1.0);
                    glRotated(g_dAngleY, 0.0, 1.0, 0.0);
                    glRotated(g_dAngleX, 1.0, 0.0, 0.0);

                    glRotated(m_AngleZ, 0.0, 0.0, 1.0);
                    glRotated(m_AngleY, 0.0, 1.0, 0.0);
                    glRotated(m_AngleX, 1.0, 0.0, 0.0);

                    Draw3D(side, x, y, cubieSize, stickerSize);
                }
            }
        }
    }

    glFlush();	

    SwapBuffers(pDC->m_hDC);

    wglMakeCurrent(NULL, NULL);
}

void CCubeView::Draw3D(int side, int x, int y, GLdouble cubieSize, GLdouble stickerSize)
{
    bool bIncludeInside = false;
    switch ( side )
    {
    case 0:
        if ( m_AnglesX[m_Size-1-x] != 0 )
        {
            glRotated(m_AnglesX[m_Size-1-x], 1.0, 0.0, 0.0);
            bIncludeInside = true;
        }
        if ( m_AnglesY[0] != 0 )
        {
            glRotated(m_AnglesY[0], 0.0, 1.0, 0.0);
        }
        if ( m_AnglesZ[m_Size-1-y] != 0 )
        {
            glRotated(m_AnglesZ[m_Size-1-y], 0.0, 0.0, 1.0);
            bIncludeInside = true;
        }

        glRotated(-90.0, 1.0, 0.0, 0.0);
        break;
    case 1:
        if ( m_AnglesX[m_Size-1] != 0 )
        {
            glRotated(m_AnglesX[m_Size-1], 1.0, 0.0, 0.0);
        }
        if ( m_AnglesY[y] != 0 )
        {
            glRotated(m_AnglesY[y], 0.0, 1.0, 0.0);
            bIncludeInside = true;
        }
        if ( m_AnglesZ[m_Size-1-x] != 0 )
        {
            glRotated(m_AnglesZ[m_Size-1-x], 0.0, 0.0, 1.0);
            bIncludeInside = true;
        }

        glRotated(-90.0, 0.0, 1.0, 0.0);
        break;
    case 2:
        if ( m_AnglesX[m_Size-1-x] != 0 )
        {
            glRotated(m_AnglesX[m_Size-1-x], 1.0, 0.0, 0.0);
            bIncludeInside = true;
        }
        if ( m_AnglesY[y] != 0 )
        {
            glRotated(m_AnglesY[y], 0.0, 1.0, 0.0);
            bIncludeInside = true;
        }
        if ( m_AnglesZ[0] != 0 )
        {
            glRotated(m_AnglesZ[0], 0.0, 0.0, 1.0);
        }
        break;
    case 3:
        if ( m_AnglesX[0] != 0 )
        {
            glRotated(m_AnglesX[0], 1.0, 0.0, 0.0);
        }
        if ( m_AnglesY[y] != 0 )
        {
            glRotated(m_AnglesY[y], 0.0, 1.0, 0.0);
            bIncludeInside = true;
        }
        if ( m_AnglesZ[x] != 0 )
        {
            glRotated(m_AnglesZ[x], 0.0, 0.0, 1.0);
            bIncludeInside = true;
        }

        glRotated(90.0, 0.0, 1.0, 0.0);
        break;
    case 4:
        if ( m_AnglesX[m_Size-1-x] != 0 )
        {
            glRotated(m_AnglesX[m_Size-1-x], 1.0, 0.0, 0.0);
            bIncludeInside = true;
        }
        if ( m_AnglesY[m_Size-1] != 0 )
        {
            glRotated(m_AnglesY[m_Size-1], 0.0, 1.0, 0.0);
        }
        if ( m_AnglesZ[y] != 0 )
        {
            glRotated(m_AnglesZ[y], 0.0, 0.0, 1.0);
            bIncludeInside = true;
        }

        glRotated(90.0, 1.0, 0.0, 0.0);
        break;
    case 5:
        if ( m_AnglesX[m_Size-1-x] != 0 )
        {
            glRotated(m_AnglesX[m_Size-1-x], 1.0, 0.0, 0.0);
            bIncludeInside = true;
        }
        if ( m_AnglesY[m_Size-1-y] != 0 )
        {
            glRotated(m_AnglesY[m_Size-1-y], 0.0, 1.0, 0.0);
            bIncludeInside = true;
        }
        if ( m_AnglesZ[m_Size-1] != 0 )
        {
            glRotated(m_AnglesZ[m_Size-1], 0.0, 0.0, 1.0);
        }

        glRotated(180.0, 1.0, 0.0, 0.0);
        break;
    }
    glTranslated(0.0, 0.0, 0.5);

    glTranslated((x-double(m_Size-1)/2)/m_Size, (double(m_Size-1)/2-y)/m_Size, 0.0);

    glColor3dv(colors[6]);

    glBegin(GL_POLYGON); 
    glNormal3d(  0.0,  0.0,  cubieSize);
    glVertex3d( cubieSize,  cubieSize, 0); 
    glVertex3d(-cubieSize,  cubieSize, 0);            
    glVertex3d(-cubieSize, -cubieSize, 0);           
    glVertex3d( cubieSize, -cubieSize, 0);            
    glEnd();

    if ( bIncludeInside )
    {
        glBegin(GL_POLYGON); 
        glNormal3d(  0.0,  -cubieSize,  0.0 );
        glVertex3d( cubieSize, -cubieSize, 0); 
        glVertex3d(-cubieSize, -cubieSize, 0);
        glVertex3d(-cubieSize, -cubieSize, -cubieSize*m_Size/2);           
        glVertex3d( cubieSize, -cubieSize, -cubieSize*m_Size/2);            
        glEnd();

        glBegin(GL_POLYGON); 
        glNormal3d(  0.0,  cubieSize,  0.0 );
        glVertex3d( cubieSize, cubieSize, 0); 
        glVertex3d(-cubieSize, cubieSize, 0);
        glVertex3d(-cubieSize, cubieSize, -cubieSize*m_Size/2);           
        glVertex3d( cubieSize, cubieSize, -cubieSize*m_Size/2);            
        glEnd();

        glBegin(GL_POLYGON); 
        glNormal3d(  cubieSize,  0.0,  0.0 );
        glVertex3d( cubieSize,  cubieSize, 0); 
        glVertex3d( cubieSize, -cubieSize, 0);
        glVertex3d( cubieSize, -cubieSize, -cubieSize*m_Size/2);           
        glVertex3d( cubieSize,  cubieSize, -cubieSize*m_Size/2);            
        glEnd();

        glBegin(GL_POLYGON); 
        glNormal3d(  cubieSize,  0.0,  0.0 );
        glVertex3d( cubieSize,  cubieSize, 0); 
        glVertex3d( cubieSize, -cubieSize, 0);
        glVertex3d( cubieSize, -cubieSize, -cubieSize*m_Size/2);           
        glVertex3d( cubieSize,  cubieSize, -cubieSize*m_Size/2);            
        glEnd();
    }

    glTranslated(0.0, 0.0, 0.001);

    double weight = m_Sides[side][y*m_Size+x].m_Row * m_Size + m_Sides[side][y*m_Size+x].m_Column;
    weight = 0.5 + 0.5 * (weight / (m_Size*m_Size-1));
    GLdouble pieceColor[3];
    pieceColor[0] = colors[ m_Sides[side][y*m_Size+x].m_Color ][0] * weight;
    pieceColor[1] = colors[ m_Sides[side][y*m_Size+x].m_Color ][1] * weight;
    pieceColor[2] = colors[ m_Sides[side][y*m_Size+x].m_Color ][2] * weight;

    glColor3dv(pieceColor);

    glBegin(GL_POLYGON);
    glNormal3d(  0.0,  0.0,  stickerSize);
    glVertex3d( stickerSize,  stickerSize, 0); 
    glVertex3d(-stickerSize,  stickerSize, 0);            
    glVertex3d(-stickerSize, -stickerSize, 0);           
    glVertex3d( stickerSize, -stickerSize, 0);            
    glEnd();
}

BOOL CCubeView::PreCreateWindow(CREATESTRUCT& cs)
{
    cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN; 
	return CView::PreCreateWindow(cs);
}

void CCubeView::OnInitialUpdate() 
{
    CClientDC dc(this);
    BOOL bResult = wglMakeCurrent(dc.m_hDC, m_hrc);	 
    if (!bResult)
    {
        TRACE("wglMakeCurrent Failed %x\r\n", GetLastError() );
    }

    wglMakeCurrent(NULL, NULL);

    CView::OnInitialUpdate();
}

int CCubeView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CView::OnCreate(lpCreateStruct) == -1)
        return -1;

    CClientDC dc(this);

    PIXELFORMATDESCRIPTOR pfd;
    memset(&pfd,0, sizeof(PIXELFORMATDESCRIPTOR));

    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);   
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 32;
    pfd.iLayerType = PFD_MAIN_PLANE;

    int nPixelFormat = ChoosePixelFormat(dc.m_hDC, &pfd);
    if (nPixelFormat == 0)
    {
        TRACE("ChoosePixelFormat Failed %d\r\n",GetLastError());
        return -1;
    }

	BOOL bResult = SetPixelFormat(dc.m_hDC, theApp.m_MSAAPixelFormat == 0 ? nPixelFormat : theApp.m_MSAAPixelFormat, &pfd);
    if (!bResult)
    {
        TRACE("SetPixelFormat Failed %d\r\n",GetLastError());
        return -1;
    }

    m_hrc = wglCreateContext(dc.m_hDC);
    if (!m_hrc)
    {
        TRACE("wglCreateContext Failed %x\r\n", GetLastError());
        return -1;
    }

    if(wglMakeCurrent(dc.m_hDC, m_hrc) == FALSE)
    {
        TRACE("wglMakeCurrent Failed %x\r\n", GetLastError());
        return false;
    }

    if(glewInit() != GLEW_OK)
    {
        TRACE("glewInit Failed %x\r\n", GetLastError());
        return -1;
    }


    int Samples = 4;
    while(Samples > 0)
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
            WGL_SAMPLES_ARB, Samples,
            0
        };

        if(wglChoosePixelFormatARB(dc.m_hDC, PFAttribs, NULL, 1, &(theApp.m_MSAAPixelFormat), &NumFormats) == TRUE && NumFormats > 0) break;

        --Samples;
    }

    return 0;
}

void CCubeView::OnSize(UINT nType, int cx, int cy) 
{
    CView::OnSize(nType, cx, cy);

    m_WindowSizeX = cx;
    m_WindowSizeY = cy;

    if ( (cx <= 0) || (cy <= 0) )
        return;

    CClientDC dc(this);

    BOOL bResult = wglMakeCurrent(dc.m_hDC, m_hrc);
    if (!bResult)
    {
        TRACE("wglMakeCurrent Failed %x\r\n", GetLastError() );
        return;
    }

    GLdouble gldAspect = (GLdouble) cx/ (GLdouble) cy;
    glMatrixMode(GL_PROJECTION); OutputGlError("MatrixMode");
    glLoadIdentity();
    gluPerspective(30.0, gldAspect, 1.0, 10.0);
    glViewport(0, 0, cx, cy);

    wglMakeCurrent(NULL, NULL);	
}

void CCubeView::OnDestroy() 
{
    CView::OnDestroy();

    wglMakeCurrent(NULL,NULL); 
    if (m_hrc) 
    {
        wglDeleteContext(m_hrc);
        m_hrc = NULL;
    }		
}

void CCubeView::OnLButtonDown(UINT nFlags, CPoint point)
{
    CView::OnLButtonDown(nFlags, point);
    m_Down = point;
    m_Abort = true;
}

void CCubeView::OnLButtonUp(UINT nFlags, CPoint point)
{
    CView::OnLButtonUp(nFlags, point);

    int diffX = point.x-m_Down.x;
    int diffY = point.y-m_Down.y;

    if ( abs(diffX) < 10 )
    {
        diffX = 0;
    }
    if ( abs(diffY) < 10 )
    {
        diffY = 0;
    }
    if ( diffX || diffY )
    {
        double dSquareSize = 0.9 * m_WindowSizeY / (m_Size * 4);

        double dTop = (m_WindowSizeY / 2.0) - dSquareSize * 2 * m_Size;
        int row = int(floor((m_Down.y - dTop) / dSquareSize));

        bool bInclude3D1 = (m_WindowSizeX > 1.37 * m_WindowSizeY);
        bool bInclude3D2 = (m_WindowSizeX > 2.05 * m_WindowSizeY);

        double offset = (bInclude3D1 ? (bInclude3D2 ? 1.5 : 1.0) : 0.5);
        double dLeft = (m_WindowSizeX / 2.0) - (dSquareSize * 3 * m_Size * offset); 
        int column = int(floor((m_Down.x - dLeft) / dSquareSize));

        int side = -1;
        int x = -1;
        int y = -1;
        if ( row < 0 )
        {
            return;
        }
        else if ( row < m_Size )
        {
            if ( column < m_Size )
            {
                return;
            }
            else if ( column < m_Size*2 )
            {
                x = column - m_Size;
                y = row;
                side = 0;
            }
            else
            {
                return;
            }
        }
        else if ( row < m_Size*2 )
        {
            if ( column < 0 )
            {
                return;
            }
            else if ( column < m_Size )
            {
                x = column;
                y = row - m_Size;
                side = 1;
            }
            else if ( column < m_Size*2 )
            {
                x = column - m_Size;
                y = row - m_Size;
                side = 2;
            }
            else if ( column < m_Size*3 )
            {
                x = column - m_Size*2;
                y = row - m_Size;
                side = 3;
            }
            else
            {
                return;
            }
        }
        else if ( row < m_Size*3 )
        {
            if ( column < m_Size )
            {
                return;
            }
            else if ( column < m_Size*2 )
            {
                x = column - m_Size;
                y = row - m_Size*2;
                side = 4;
            }
            else
            {
                return;
            }
        }
        else if ( row < m_Size*4 )
        {
            if ( column < m_Size )
            {
                return;
            }
            else if ( column < m_Size*2 )
            {
                x = column - m_Size;
                y = row - m_Size*3;
                side = 5;
            }
            else
            {
                return;
            }
        }
        else
        {
            return;
        }

        int type = -1;
        int depth = 0;
        if ( abs(diffX) > abs(diffY))
        {
            switch ( side )
            {
            case 0:
                if ( diffX > 0 )
                {
                    type = 2;
                    depth = m_Size-1-y;
                }
                else
                {
                    type = 3;
                    depth = y;
                }
                break;
            case 1:
            case 2:
            case 3:
                if ( diffX > 0 )
                {
                    type = 5;
                    depth = m_Size-1-y;
                }
                else
                {
                    type = 4;
                    depth = y;
                }
                break;
            case 4:
                if ( diffX > 0 )
                {
                    type = 3;
                    depth = m_Size-1-y;
                }
                else
                {
                    type = 2;
                    depth = y;
                }
                break;
            case 5:
                if ( diffX > 0 )
                {
                    type = 4;
                    depth = m_Size-1-y;
                }
                else
                {
                    type = 5;
                    depth = y;
                }
                break;
            }
        }
        else
        {
            switch ( side )
            {
            case 1:
                if ( diffY > 0 )
                {
                    type = 3;
                    depth = x;
                }
                else
                {
                    type = 2;
                    depth = m_Size-1-x;
                }
                break;
            case 0:
            case 2:
            case 4:
            case 5:
                if ( diffY > 0 )
                {
                    type = 0;
                    depth = x;
                }
                else
                {
                    type = 1;
                    depth = m_Size-1-x;
                }
                break;
            case 3:
                if ( diffY > 0 )
                {
                    type = 2;
                    depth = x;
                }
                else
                {
                    type = 3;
                    depth = m_Size-1-x;
                }
                break;
            }
        }
        PostMessage( WM_DO_UP, type, depth );
    }
}

LRESULT CCubeView::OnDoUp(WPARAM wParam, LPARAM lParam)
{
    if ( m_Thread )
    {
        m_Abort = true;
        SetThreadPriority( m_Thread, THREAD_PRIORITY_HIGHEST );
        if ( WaitForSingleObject( m_Thread, 10 ) != WAIT_OBJECT_0 )
        {
            PostMessage( WM_DO_UP, wParam, lParam );
            return 0;
        }
        CloseHandle( m_Thread );
        m_Thread = NULL;
    }

    switch ((int)wParam)
    {
    case 0:
        RotateLeft(lParam);
        break;
    case 1:
        RotateRight(lParam);
        break;
    case 2:
        RotateFront(lParam);
        break;
    case 3:
        RotateBack(lParam);
        break;
    case 4:
        RotateTop(lParam);
        break;
    case 5:
        RotateBottom(lParam);
        break;
    }
    return 0;
}

void CCubeView::OnEditLess()
{
    if ( m_Size > 2 )
    {
        --m_Size;
        InitCube();

        Invalidate(FALSE);
        UpdateWindow();
        PostMessage(WM_DISPLAY_STATUS);
    }
}

void CCubeView::OnEditMore()
{
    if ( m_Size < 100 )
    {
        ++m_Size;
        InitCube();

        Invalidate(FALSE);
        UpdateWindow();
        PostMessage(WM_DISPLAY_STATUS);
    }
}

void CCubeView::OnEditScramble()
{
    if ( m_Thread )
    {
        m_Abort = true;
        SetThreadPriority( m_Thread, THREAD_PRIORITY_HIGHEST );
        WaitForSingleObject( m_Thread, INFINITE );
        CloseHandle( m_Thread );
        m_Thread = NULL;
    }

    m_Abort = false;
    unsigned int id = 0;
    m_Thread = (HANDLE)_beginthreadex( NULL, 0, (unsigned int (__stdcall *)(void*))&ScrambleStartEntry, this, 0, &id );
}

unsigned int CCubeView::ScrambleStartEntry( void* arg )
{
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_IDLE);
    return ((CCubeView*)arg)->ScrambleStart();
}

unsigned int CCubeView::ScrambleStart()
{
    srand(GetTickCount());
    m_Mixing = true;

    int numMoves = ((rand() * m_Size)/RAND_MAX + 3) * 5;
    int prevType = -1;
    int moveType = rand() * 3 / RAND_MAX;
    fprintf_s( m_LogFile, "Scrambling %i\n", numMoves );
    fflush(m_LogFile);
    for ( ; numMoves; --numMoves )
    {
        if (m_Abort)
        {
            break;
        }
        PostMessage(WM_DISPLAY_STATUS_2, numMoves);
        int depth = rand() * m_Size / RAND_MAX;
        int amount = rand() * 3 / RAND_MAX;
        switch ( moveType )
        {
        case 0:
            switch ( amount )
            {
            case 0:
                RotateLeft( m_Size-1-depth, false );
                break;
            case 1:
                RotateRight( depth, false );
            case 2:
                RotateRight( depth, false );
            }
            break;
        case 1:
            switch ( amount )
            {
            case 0:
                RotateTop( m_Size-1-depth, false );
                break;
            case 1:
                RotateBottom( depth, false );
            case 2:
                RotateBottom( depth, false );
            }
            break;
        case 2:
            switch ( amount )
            {
            case 0:
                RotateFront( m_Size-1-depth, false );
                break;
            case 1:
                RotateBack( depth, false );
            case 2:
                RotateBack( depth, false );
            }
            break;
        }
        prevType = moveType;
        moveType = rand() * 2 / RAND_MAX;
        if ( moveType >= prevType )
        {
            moveType++;
        }
    }
    PostMessage(WM_DISPLAY_STATUS);

    m_Moves = 0;
    m_Mixing = false;
    return true;
}

void CCubeView::IncrementMoves()
{
    ++m_Moves;
    if (!m_Mixing)
    {
        PostMessage(WM_DISPLAY_STATUS);
    }
}

void CCubeView::OnEditSolve()
{
    if (m_Thread)
    {
        m_Abort = true;
        SetThreadPriority( m_Thread, THREAD_PRIORITY_HIGHEST );
        WaitForSingleObject( m_Thread, INFINITE );
        CloseHandle( m_Thread );
        m_Thread = NULL;
    }

    m_Abort = false;
    unsigned int id = 0;
    m_Thread = (HANDLE)_beginthreadex( NULL, 0, (unsigned int (__stdcall *)(void*))&SolveStartEntry, this, 0, &id );
}

unsigned int CCubeView::SolveStartEntry( void* arg )
{
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_IDLE);
    return ((CCubeView*)arg)->SolveStart();
}

unsigned int CCubeView::SolveStart()
{
    m_SidesCopy[0] = m_Sides[0];
    m_SidesCopy[1] = m_Sides[1];
    m_SidesCopy[2] = m_Sides[2];
    m_SidesCopy[3] = m_Sides[3];
    m_SidesCopy[4] = m_Sides[4];
    m_SidesCopy[5] = m_Sides[5];

    m_SideAnglesCopy[0] = m_SideAngles[0];
    m_SideAnglesCopy[1] = m_SideAngles[1];
    m_SideAnglesCopy[2] = m_SideAngles[2];
    m_SideAnglesCopy[3] = m_SideAngles[3];
    m_SideAnglesCopy[4] = m_SideAngles[4];
    m_SideAnglesCopy[5] = m_SideAngles[5];

    Solve();
    
    bool found = false;
    for (int side = 0; side < 6; ++side)
    {
        for (int row = 0; row < m_Size; ++row)
        {
            for (int col = 0; col < m_Size; ++col)
            {
                found = found || !IsColor(side, row, col, side, row, col);
            }
        }
    }
    if (m_Size&1)
    {
        for (int side = 0; side < 6; ++side)
        {
            found = found || (m_SideAngles[side] != 0);
        }
    }
    if (found)
    {
        FILE *fp = NULL;
        fopen_s(&fp, "FailedCube.txt", "a");
        fprintf_s(fp, "Failed to solve\n");
        fprintf_s(fp, "Size %i\n", m_Size);
        for (int side = 0; side < 6; ++side)
        {
            for (int row = 0; row < m_Size; ++row)
            {
                for (int col = 0; col < m_Size; ++col)
                {
                    if ( col > 0)
                    {
                        fprintf_s( fp, "," );
                    }
                    fprintf_s( fp, "%i %i %i %i", m_SidesCopy[side][row*m_Size+col].m_Row, m_SidesCopy[side][row*m_Size+col].m_Column, m_SidesCopy[side][row*m_Size+col].m_Color, m_SideAnglesCopy[side] );
                }
                fprintf_s( fp, "\n" );
            }
            fprintf_s( fp, "\n" );
        }
        fclose(fp);
    }
    fprintf_s( m_LogFile, "Solve Complete %i, status %i\n", m_Moves, (int)found );
    fflush(m_LogFile);

    return 0;
}

void CCubeView::Solve()
{
    fprintf_s( m_LogFile, "Starting solve %i\n", m_Moves );
    fflush(m_LogFile);

    // rotate full cube to get absolute center right
    int middle = (m_Size*m_Size-1)/2;
    if ((m_Size & 1) == 1)
    {
        if (m_Sides[0][middle].m_Color != 0)
        {
            if (m_Sides[1][middle].m_Color == 0)
            {
                RotateCZ(false);
            }
            else if (m_Sides[2][middle].m_Color == 0)
            {
                RotateCX(false);
            }
            else if (m_Sides[3][middle].m_Color == 0)
            {
                RotateZ(false);
            }
            else if (m_Sides[4][middle].m_Color == 0)
            {
                RotateZ(false);
                RotateZ(false);
            }
            else if (m_Sides[5][middle].m_Color == 0)
            {
                RotateX(false);
            }
        }
    }

    // solve centers
    int savedMoves = m_Moves;
    int numPiecesChanged = FixTopCenters();
    for (int row = 1; row < m_Size-1; ++row)
    {
        for (int col = 1; col < m_Size-1; ++col)
        {
            m_Abort = m_Abort || !IsColor(0, row, col, 0, row, col);
        }
    }
    if (m_Size&1)
    {
        m_Abort = m_Abort || (m_SideAngles[0] != 0);
    }
    fprintf_s( m_LogFile, "Solved top centers %i, pieces changed %i, moves per piece %f, status %i\n", m_Moves, numPiecesChanged, double(m_Moves-savedMoves)/numPiecesChanged, (int)m_Abort );
    fflush(m_LogFile);
    if (m_Abort)
    {
        return;
    }
    savedMoves = m_Moves;
    numPiecesChanged = FixBottomCenters();
    for (int row = 1; row < m_Size-1; ++row)
    {
        for (int col = 1; col < m_Size-1; ++col)
        {
            m_Abort = m_Abort || !IsColor(4, row, col, 4, row, col);
        }
    }
    if (m_Size&1)
    {
        m_Abort = m_Abort || (m_SideAngles[0] != 0) || (m_SideAngles[4] != 0);
    }
    fprintf_s( m_LogFile, "Solved bottom centers %i, pixels changed %i, moves per piece %f, status %i\n", m_Moves, numPiecesChanged, double(m_Moves-savedMoves)/numPiecesChanged, (int)m_Abort );
    fflush(m_LogFile);
    if (m_Abort)
    {
        return;
    }
    savedMoves = m_Moves;
    numPiecesChanged = FixEdgeCenters(1, 2, 3, 5, 1, 2, false);
    for (int row = 1; row < m_Size-1; ++row)
    {
        for (int col = 1; col < m_Size-1; ++col)
        {
            m_Abort = m_Abort || !IsColor(1, row, col, 1, row, col);
        }
    }
    if (m_Size&1)
    {
        m_Abort = m_Abort || (m_SideAngles[0] != 0) || (m_SideAngles[4] != 0) || (m_SideAngles[1] != 0);
    }
    fprintf_s( m_LogFile, "Solved left centers %i, pieces changed = %i, moves per piece %f, status %i\n", m_Moves, numPiecesChanged, double(m_Moves-savedMoves)/numPiecesChanged, (int)m_Abort );
    fflush(m_LogFile);
    if (m_Abort)
    {
        return;
    }
    savedMoves = m_Moves;
    numPiecesChanged = FixEdgeCenters(2, 3, 5, 1, 2, 0, true);
    for (int row = 1; row < m_Size-1; ++row)
    {
        for (int col = 1; col < m_Size-1; ++col)
        {
            m_Abort = m_Abort || !IsColor(2, row, col, 2, row, col);
        }
    }
    if (m_Size&1)
    {
        m_Abort = m_Abort || (m_SideAngles[0] != 0) || (m_SideAngles[4] != 0) || (m_SideAngles[1] != 0) || (m_SideAngles[2] != 0);
    }
    fprintf_s( m_LogFile, "Solved front centers %i, pieces changed = %i, moves per piece %f, status %i\n", m_Moves, numPiecesChanged, double(m_Moves-savedMoves)/numPiecesChanged, (int)m_Abort );
    fflush(m_LogFile);
    if (m_Abort)
    {
        return;
    }
    savedMoves = m_Moves;
    numPiecesChanged = FixSecondLastCenters();
    for (int row = 1; row < m_Size-1; ++row)
    {
        for (int col = 1; col < m_Size-1; ++col)
        {
            m_Abort = m_Abort || !IsColor(3, row, col, 3, row, col);
        }
    }
    if (m_Size&1)
    {
        m_Abort = m_Abort || (m_SideAngles[0] != 0) || (m_SideAngles[4] != 0) || (m_SideAngles[1] != 0) || (m_SideAngles[2] != 0) || (m_SideAngles[3] != 0);
    }
    fprintf_s( m_LogFile, "Solved right centers %i, pieces changed = %i, moves per piece %f, status %i\n", m_Moves, numPiecesChanged, double(m_Moves-savedMoves)/numPiecesChanged, (int)m_Abort );
    fflush(m_LogFile);
    if (m_Abort)
    {
        return;
    }

    if (m_Size&1)
    {
        switch ( m_SideAngles[5] )
        {
        case 0:
            break;
        case 1:
            AddMove(3, 0);
            break;
        case 2:
            AddMove(3, 0);
            AddMove(3, 0);
            break;
        case 3:
            AddMove(2, m_Size-1);
            break;
        }
    }

    // solve edges
    // do 3 on top
    FixTopEdge(5, 1, 2, 3, 3, 1);
    if (m_Size&1)
    {
        PlayMoves();
        m_Abort = m_Abort || (m_SideAngles[0] != 0) || (m_SideAngles[4] != 0) || (m_SideAngles[1] != 0) || (m_SideAngles[2] != 0) || (m_SideAngles[3] != 0) || (m_SideAngles[5] != 0);
    }
    if (m_Abort)
    {
        return;
    }
    FixTopEdge(1, 2, 3, 5, 1, 2);
    if (m_Size&1)
    {
        PlayMoves();
        m_Abort = m_Abort || (m_SideAngles[0] != 0) || (m_SideAngles[4] != 0) || (m_SideAngles[1] != 0) || (m_SideAngles[2] != 0) || (m_SideAngles[3] != 0) || (m_SideAngles[5] != 0);
    }
    if (m_Abort)
    {
        return;
    }
    FixTopEdge(2, 3, 5, 1, 2, 0);
    if (m_Size&1)
    {
        PlayMoves();
        m_Abort = m_Abort || (m_SideAngles[0] != 0) || (m_SideAngles[4] != 0) || (m_SideAngles[1] != 0) || (m_SideAngles[2] != 0) || (m_SideAngles[3] != 0) || (m_SideAngles[5] != 0);
    }
    if (m_Abort)
    {
        return;
    }
    // do 4 on bottom
    FixBottomEdge(5, 1, 2, 3, 3, 1);
    if (m_Size&1)
    {
        PlayMoves();
        m_Abort = m_Abort || (m_SideAngles[0] != 0) || (m_SideAngles[4] != 0) || (m_SideAngles[1] != 0) || (m_SideAngles[2] != 0) || (m_SideAngles[3] != 0) || (m_SideAngles[5] != 0);
    }
    if (m_Abort)
    {
        return;
    }
    FixBottomEdge(1, 2, 3, 5, 1, 2);
    if (m_Size&1)
    {
        PlayMoves();
        m_Abort = m_Abort || (m_SideAngles[0] != 0) || (m_SideAngles[4] != 0) || (m_SideAngles[1] != 0) || (m_SideAngles[2] != 0) || (m_SideAngles[3] != 0) || (m_SideAngles[5] != 0);
    }
    if (m_Abort)
    {
        return;
    }
    FixBottomEdge(2, 3, 5, 1, 2, 0);
    if (m_Size&1)
    {
        PlayMoves();
        m_Abort = m_Abort || (m_SideAngles[0] != 0) || (m_SideAngles[4] != 0) || (m_SideAngles[1] != 0) || (m_SideAngles[2] != 0) || (m_SideAngles[3] != 0) || (m_SideAngles[5] != 0);
    }
    if (m_Abort)
    {
        return;
    }
    FixBottomEdge(3, 5, 1, 2, 0, 3);
    if (m_Size&1)
    {
        PlayMoves();
        m_Abort = m_Abort || (m_SideAngles[0] != 0) || (m_SideAngles[4] != 0) || (m_SideAngles[1] != 0) || (m_SideAngles[2] != 0) || (m_SideAngles[3] != 0) || (m_SideAngles[5] != 0);
    }
    if (m_Abort)
    {
        return;
    }
    PlayMoves();
    fprintf_s( m_LogFile, "Solved 3 top and 4 bottom edges %i, status %i\n", m_Moves, (int)m_Abort );
    fflush(m_LogFile);
    
    // solve corners
    FixCorners();
    if (m_Size&1)
    {
        PlayMoves();
        m_Abort = m_Abort || (m_SideAngles[0] != 0) || (m_SideAngles[4] != 0) || (m_SideAngles[1] != 0) || (m_SideAngles[2] != 0) || (m_SideAngles[3] != 0) || (m_SideAngles[5] != 0);
    }
    if (m_Abort)
    {
        return;
    }
    PlayMoves();
    fprintf_s( m_LogFile, "Solved corners %i, status %i\n", m_Moves, (int)m_Abort );
    fflush(m_LogFile);

    // fix middle center
    if (m_Size&1)
    {
        FixEdgeCenter(5, 1);
        if (m_Size&1)
        {
            PlayMoves();
            m_Abort = m_Abort || (m_SideAngles[0] != 0) || (m_SideAngles[4] != 0) || (m_SideAngles[1] != 0) || (m_SideAngles[2] != 0) || (m_SideAngles[3] != 0) || (m_SideAngles[5] != 0);
        }
        if (m_Abort)
        {
            return;
        }
        FixEdgeCenter(1, 2);
        if (m_Size&1)
        {
            PlayMoves();
            m_Abort = m_Abort || (m_SideAngles[0] != 0) || (m_SideAngles[4] != 0) || (m_SideAngles[1] != 0) || (m_SideAngles[2] != 0) || (m_SideAngles[3] != 0) || (m_SideAngles[5] != 0);
        }
        if (m_Abort)
        {
            return;
        }
        FixEdgeCenter(2, 3);
        if (m_Size&1)
        {
            PlayMoves();
            m_Abort = m_Abort || (m_SideAngles[0] != 0) || (m_SideAngles[4] != 0) || (m_SideAngles[1] != 0) || (m_SideAngles[2] != 0) || (m_SideAngles[3] != 0) || (m_SideAngles[5] != 0);
        }
        if (m_Abort)
        {
            return;
        }
        FlipCenterEdge();
        if (m_Size&1)
        {
            PlayMoves();
            m_Abort = m_Abort || (m_SideAngles[0] != 0) || (m_SideAngles[4] != 0) || (m_SideAngles[1] != 0) || (m_SideAngles[2] != 0) || (m_SideAngles[3] != 0) || (m_SideAngles[5] != 0);
        }
        if (m_Abort)
        {
            return;
        }
    }

    bool first = true;
    do
    {
        if (m_Size&1)
        {
            PlayMoves();
            m_Abort = m_Abort || (m_SideAngles[0] != 0) || (m_SideAngles[4] != 0) || (m_SideAngles[1] != 0) || (m_SideAngles[2] != 0) || (m_SideAngles[3] != 0) || (m_SideAngles[5] != 0);
        }
        if (m_Abort)
        {
            return;
        }
        FixSideEdge(5, 1);
        if (m_Size&1)
        {
            PlayMoves();
            m_Abort = m_Abort || (m_SideAngles[0] != 0) || (m_SideAngles[4] != 0) || (m_SideAngles[1] != 0) || (m_SideAngles[2] != 0) || (m_SideAngles[3] != 0) || (m_SideAngles[5] != 0);
        }
        if (m_Abort)
        {
            return;
        }
        FixSideEdge(1, 2);
        if (m_Size&1)
        {
            PlayMoves();
            m_Abort = m_Abort || (m_SideAngles[0] != 0) || (m_SideAngles[4] != 0) || (m_SideAngles[1] != 0) || (m_SideAngles[2] != 0) || (m_SideAngles[3] != 0) || (m_SideAngles[5] != 0);
        }
        if (m_Abort)
        {
            return;
        }
        FixSideEdge(2, 3);
        if (m_Size&1)
        {
            PlayMoves();
            m_Abort = m_Abort || (m_SideAngles[0] != 0) || (m_SideAngles[4] != 0) || (m_SideAngles[1] != 0) || (m_SideAngles[2] != 0) || (m_SideAngles[3] != 0) || (m_SideAngles[5] != 0);
        }
        if (m_Abort)
        {
            return;
        }

        // solve remaining sides
        FixSecondLastSideEdge();
        if (m_Size&1)
        {
            PlayMoves();
            m_Abort = m_Abort || (m_SideAngles[0] != 0) || (m_SideAngles[4] != 0) || (m_SideAngles[1] != 0) || (m_SideAngles[2] != 0) || (m_SideAngles[3] != 0) || (m_SideAngles[5] != 0);
        }
        if (m_Abort)
        {
            return;
        }
        if (!first)
            break;
        first = false;
    } while( FixLastSideEdge() );

    PlayMoves();
    fprintf_s( m_LogFile, "Solved side edges %i, status %i\n", m_Moves, (int)m_Abort );
    fflush(m_LogFile);

    savedMoves = m_Moves;
    numPiecesChanged = FixLastCenters();
    PlayMoves();
    fprintf_s( m_LogFile, "Solved last centers %i, pieces changed = %i, moves per piece %f, status %i\n", m_Moves, numPiecesChanged, double(m_Moves-savedMoves)/numPiecesChanged, (int)m_Abort );
    fflush(m_LogFile);

    PlayMoves();
}

void CCubeView::OnEditRedo()
{
    m_Moves = 0;
    m_Sides[0] = m_SidesCopy[0];
    m_Sides[1] = m_SidesCopy[1];
    m_Sides[2] = m_SidesCopy[2];
    m_Sides[3] = m_SidesCopy[3];
    m_Sides[4] = m_SidesCopy[4];
    m_Sides[5] = m_SidesCopy[5];
    m_SideAngles[0] = m_SideAnglesCopy[0];
    m_SideAngles[1] = m_SideAnglesCopy[1];
    m_SideAngles[2] = m_SideAnglesCopy[2];
    m_SideAngles[3] = m_SideAnglesCopy[3];
    m_SideAngles[4] = m_SideAnglesCopy[4];
    m_SideAngles[5] = m_SideAnglesCopy[5];
    Invalidate(FALSE);
    UpdateWindow();
}

void CCubeView::OnEditOpen()
{
    FILE *fp = NULL;
    fopen_s( &fp, "Cube.txt", "r" );
    fscanf_s( fp, "Size %i\n", &m_Size );

    InitCube();

    for (int side = 0; side < 6; ++side)
    {
        for (int row = 0; row < m_Size; ++row)
        {
            for (int col = 0; col < m_Size; ++col)
            {
                if ( col > 0)
                {
                    fscanf_s( fp, "," );
                }
                int row1, column1, color1, angle1;
                fscanf_s( fp, "%i %i %i %i", &row1, &column1, &color1, &angle1 );
                m_Sides[side][row*m_Size+col].m_Row = row1;
                m_Sides[side][row*m_Size+col].m_Column = column1;
                m_Sides[side][row*m_Size+col].m_Color = color1;
                if (angle1 != 0)
                {
                    m_SideAngles[side] = angle1;
                }
            }
            fscanf_s( fp, "\n" );
        }
        fscanf_s( fp, "\n" );
    }
    fclose(fp);
    Invalidate(FALSE);
    UpdateWindow();
}

void CCubeView::OnViewRotateX()
{
    RotateX();
}

void CCubeView::RotateX(bool update)
{
    while ( m_AngleX + m_dStep < 90 )
    {
        long lLastTick = GetTickCount();
        m_AngleX += m_dStep;
        Invalidate(FALSE);
        if (update)
        {
            UpdateWindow();
        }
        else
        {
            if (m_Abort)
            {
                break;
            }
            Sleep(0);
        }
        long ticks = (lLastTick + m_lSleep)- GetTickCount();
        if ( ticks < 0 )
        {
            ticks = 0;
        }
        Sleep(ticks);
    }
    m_AngleX = 0.0;

    std::vector<Piece> Side;
    Side = m_Sides[0];
    m_Sides[0] = m_Sides[5];
    m_Sides[5] = m_Sides[4];
    m_Sides[4] = m_Sides[2];
    m_Sides[2] = Side;

    RotateSideClockwise(1);
    RotateSideCounterClockwise(3);
    Invalidate(FALSE);
    UpdateWindow();
}

void CCubeView::OnViewRotateCX()
{
    RotateCX();
}

void CCubeView::RotateCX(bool update)
{
    while ( m_AngleX - m_dStep > -90 )
    {
        long lLastTick = GetTickCount();
        m_AngleX -= m_dStep;
        Invalidate(FALSE);
        if (update)
        {
            UpdateWindow();
        }
        else
        {
            if (m_Abort)
            {
                break;
            }
            Sleep(0);
        }
        long ticks = (lLastTick + m_lSleep)- GetTickCount();
        if ( ticks < 0 )
        {
            ticks = 0;
        }
        Sleep(ticks);
    }
    m_AngleX = 0.0;

    std::vector<Piece> Side;
    Side = m_Sides[0];
    m_Sides[0] = m_Sides[2];
    m_Sides[2] = m_Sides[4];
    m_Sides[4] = m_Sides[5];
    m_Sides[5] = Side;

    RotateSideCounterClockwise(1);
    RotateSideClockwise(3);
    Invalidate(FALSE);
    UpdateWindow();
}

void CCubeView::OnViewRotateY()
{
    RotateY();
}

void CCubeView::RotateY(bool update)
{
    while ( m_AngleY + m_dStep < 90 )
    {
        long lLastTick = GetTickCount();
        m_AngleY += m_dStep;
        Invalidate(FALSE);
        if (update)
        {
            UpdateWindow();
        }
        else
        {
            if (m_Abort)
            {
                break;
            }
            Sleep(0);
        }
        long ticks = (lLastTick + m_lSleep)- GetTickCount();
        if ( ticks < 0 )
        {
            ticks = 0;
        }
        Sleep(ticks);
    }
    m_AngleY = 0.0;

    std::vector<Piece> Side;
    Side = m_Sides[1];
    m_Sides[1] = m_Sides[5];
    RotateSide180(1);
    m_Sides[5] = m_Sides[3];
    RotateSide180(5);
    m_Sides[3] = m_Sides[2];
    m_Sides[2] = Side;

    RotateSideCounterClockwise(0);
    RotateSideClockwise(4);
    Invalidate(FALSE);
    UpdateWindow();
}

void CCubeView::OnViewRotateCY()
{
    RotateCY();
}

void CCubeView::RotateCY(bool update)
{
    while ( m_AngleY - m_dStep > -90 )
    {
        long lLastTick = GetTickCount();
        m_AngleY -= m_dStep;
        Invalidate(FALSE);
        if (update)
        {
            UpdateWindow();
        }
        else
        {
            if (m_Abort)
            {
                break;
            }
            Sleep(0);
        }
        long ticks = (lLastTick + m_lSleep)- GetTickCount();
        if ( ticks < 0 )
        {
            ticks = 0;
        }
        Sleep(ticks);
    }
    m_AngleY = 0.0;

    std::vector<Piece> Side;
    Side = m_Sides[1];
    m_Sides[1] = m_Sides[2];
    m_Sides[2] = m_Sides[3];
    m_Sides[3] = m_Sides[5];
    RotateSide180(3);
    m_Sides[5] = Side;
    RotateSide180(5);

    RotateSideClockwise(0);
    RotateSideCounterClockwise(4);
    Invalidate(FALSE);
    UpdateWindow();
}

void CCubeView::OnViewRotateZ()
{
    RotateZ();
}

void CCubeView::RotateZ(bool update)
{
    while ( m_AngleZ + m_dStep < 90 )
    {
        long lLastTick = GetTickCount();
        m_AngleZ += m_dStep;
        Invalidate(FALSE);
        if (update)
        {
            UpdateWindow();
        }
        else
        {
            if (m_Abort)
            {
                break;
            }
            Sleep(0);
        }
        long ticks = (lLastTick + m_lSleep)- GetTickCount();
        if ( ticks < 0 )
        {
            ticks = 0;
        }
        Sleep(ticks);
    }
    m_AngleZ = 0.0;

    std::vector<Piece> Side;
    Side = m_Sides[0];
    m_Sides[0] = m_Sides[3];
    RotateSideCounterClockwise(0);
    m_Sides[3] = m_Sides[4];
    RotateSideCounterClockwise(3);
    m_Sides[4] = m_Sides[1];
    RotateSideCounterClockwise(4);
    m_Sides[1] = Side;
    RotateSideCounterClockwise(1);

    RotateSideCounterClockwise(2);
    RotateSideClockwise(5);
    Invalidate(FALSE);
    UpdateWindow();
}

void CCubeView::OnViewRotateCZ()
{
    RotateCZ();
}

void CCubeView::RotateCZ(bool update)
{
    while ( m_AngleZ - m_dStep > -90 )
    {
        long lLastTick = GetTickCount();
        m_AngleZ -= m_dStep;
        Invalidate(FALSE);
        if (update)
        {
            UpdateWindow();
        }
        else
        {
            if (m_Abort)
            {
                break;
            }
            Sleep(0);
        }
        long ticks = (lLastTick + m_lSleep)- GetTickCount();
        if ( ticks < 0 )
        {
            ticks = 0;
        }
        Sleep(ticks);
    }
    m_AngleZ = 0.0;

    std::vector<Piece> Side;
    Side = m_Sides[0];
    m_Sides[0] = m_Sides[1];
    RotateSideClockwise(0);
    m_Sides[1] = m_Sides[4];
    RotateSideClockwise(1);
    m_Sides[4] = m_Sides[3];
    RotateSideClockwise(4);
    m_Sides[3] = Side;
    RotateSideClockwise(3);

    RotateSideClockwise(2);
    RotateSideCounterClockwise(5);
    Invalidate(FALSE);
    UpdateWindow();
}

void CCubeView::OnSpeedSlower()
{
    if ( m_Speed > 0 )
    {
        --m_Speed;
        m_dStep /= 1.5;
    }
    PostMessage(WM_DISPLAY_STATUS);
}

void CCubeView::OnSpeedFaster()
{
    if ( (m_Speed < 20) && (m_dStep < 90) )
    {
        ++m_Speed;
        m_dStep *= 1.5;
    }
    PostMessage(WM_DISPLAY_STATUS);
}

LRESULT CCubeView::OnDisplayStatus(WPARAM wParam, LPARAM lParam)
{
    char buffer[100];
    sprintf_s( buffer, 50, "Moves Made = %i, Speed = %i, Size = %i", m_Moves, m_Speed, m_Size );
    ((CMainFrame*)AfxGetMainWnd())->SetStatusText(buffer);
    return 0;
}

LRESULT CCubeView::OnDisplayStatus2(WPARAM wParam, LPARAM lParam)
{
    char buffer[50];
    sprintf_s( buffer, 50, "Moves Remaining = %i", (int)wParam );
    ((CMainFrame*)AfxGetMainWnd())->SetStatusText(buffer);
    return 0;
}

void CCubeView::InitCube()
{
    int side;
    for ( side = 0; side < 6; ++side )
    {
        m_SideAngles[side] = 0;
        m_Sides[side].resize(m_Size * m_Size);
        Piece p;
        p.m_Color = side;
        int y;
        for ( y = 0; y < m_Size; ++y )
        {
            p.m_Row = y;
            int x;
            for ( x = 0; x < m_Size; ++x )
            {
                p.m_Column = x;
                m_Sides[side][y*m_Size+x] = p;
            }
        }
    }

    m_AngleX = 0;
    m_AngleY = 0;
    m_AngleZ = 0;

    m_AnglesX.resize(m_Size);
    m_AnglesY.resize(m_Size);
    m_AnglesZ.resize(m_Size);
    int i;
    for ( i = 0; i < m_Size; ++i )
    {
        m_AnglesX[i] = 0; 
        m_AnglesY[i] = 0; 
        m_AnglesZ[i] = 0; 
    }

    m_Moves = 0;
    m_Mixing = false;
    m_Middle = (m_Size&1) ? (m_Size-1)/2 : m_Size;
    m_MovesToPlay.resize(0);
    fprintf_s( m_LogFile, "Created cube of size %i\n", m_Size );
    fflush(m_LogFile);
}

void CCubeView::RotateSideClockwise(int side)
{
    int x;
    for ( x = 0; x < (m_Size+1)/2; ++x )
    {
        int y;
        for ( y = 0; y < m_Size/2; ++y )
        {
            int index1 = y*m_Size+x;
            int index2 = (m_Size-1-x)*m_Size+y;
            int index3 = (m_Size-1-y)*m_Size+(m_Size-1-x);
            int index4 = x*m_Size+(m_Size-1-y);
            Piece temp = m_Sides[side][index1];
            m_Sides[side][index1] = m_Sides[side][index2];
            m_Sides[side][index2] = m_Sides[side][index3];
            m_Sides[side][index3] = m_Sides[side][index4];
            m_Sides[side][index4] = temp;
        }
    }
    if (m_Size&1)
    {
        m_SideAngles[side]--;
        m_SideAngles[side] &= 3;
    }
}

void CCubeView::RotateSideCounterClockwise(int side)
{
    int x;
    for ( x = 0; x < (m_Size+1)/2; ++x )
    {
        int y;
        for ( y = 0; y < m_Size/2; ++y )
        {
            int index1 = y*m_Size+x;
            int index2 = x*m_Size+(m_Size-1-y);
            int index3 = (m_Size-1-y)*m_Size+(m_Size-1-x);
            int index4 = (m_Size-1-x)*m_Size+y;
            Piece temp = m_Sides[side][index1];
            m_Sides[side][index1] = m_Sides[side][index2];
            m_Sides[side][index2] = m_Sides[side][index3];
            m_Sides[side][index3] = m_Sides[side][index4];
            m_Sides[side][index4] = temp;
        }
    }
    if (m_Size&1)
    {
        m_SideAngles[side]++;
        m_SideAngles[side] &= 3;
    }
}

void CCubeView::RotateSide180(int side)
{
    int y;
    for ( y = 0; y < m_Size/2; ++y )
    {
        int x;
        for ( x = 0; x < m_Size; ++x )
        {
            int index1 = y*m_Size+x;
            int index2 = (m_Size-1-y)*m_Size+(m_Size-1-x);
            Piece temp = m_Sides[side][index1];
            m_Sides[side][index1] = m_Sides[side][index2];
            m_Sides[side][index2] = temp;
        }
    }
    if ( m_Size & 1 )
    {
        int x;
        for ( x = 0; x < m_Size/2; ++x )
        {
            int index1 = (m_Size/2)*m_Size+x;
            int index2 = (m_Size/2)*m_Size+(m_Size-1-x);
            Piece temp = m_Sides[side][index1];
            m_Sides[side][index1] = m_Sides[side][index2];
            m_Sides[side][index2] = temp;
        }

        m_SideAngles[side] += 2;
        m_SideAngles[side] &= 3;
    }
}

void CCubeView::RotateRight(int depth, bool update)
{
    if ( depth < m_Size )
    {
        while ( m_AnglesX[depth] - m_dStep > -90 )
        {
            long lLastTick = GetTickCount();
            m_AnglesX[depth] -= m_dStep;
            Invalidate(FALSE);
            if ( update )
            {
                UpdateWindow();
            }
            else
            {
                if (m_Abort)
                {
                    break;
                }
                Sleep(0);
            }
            long ticks = (lLastTick + m_lSleep)- GetTickCount();
            if ( ticks < 0 )
            {
                ticks = 0;
            }
            Sleep(ticks);
        }
        m_AnglesX[depth] = 0.0;

        if ( depth == 0 )
        {
            RotateSideClockwise(3);
        }
        else if ( depth == m_Size-1 )
        {
            RotateSideCounterClockwise(1);
        }
        int y;
        for ( y = 0; y < m_Size; ++y )
        {
            int index = y*m_Size+(m_Size-1-depth);
            Piece temp = m_Sides[0][index];
            m_Sides[0][index] = m_Sides[2][index];
            m_Sides[2][index] = m_Sides[4][index];
            m_Sides[4][index] = m_Sides[5][index];
            m_Sides[5][index] = temp;
        }

        if (depth == m_Middle)
        {
            int temp = m_SideAngles[0];
            m_SideAngles[0] = m_SideAngles[2];
            m_SideAngles[2] = m_SideAngles[4];
            m_SideAngles[4] = m_SideAngles[5];
            m_SideAngles[5] = temp;
        }

        Invalidate(FALSE);
        if ( update )
        {
            UpdateWindow();
        }
        else
        {
            Sleep(10);
        }
    }
    IncrementMoves();
}

void CCubeView::RotateLeft(int depth, bool update)
{
    if ( depth < m_Size )
    {
        while ( m_AnglesX[m_Size-1-depth] + m_dStep < 90 )
        {
            long lLastTick = GetTickCount();
            m_AnglesX[m_Size-1-depth] += m_dStep;
            Invalidate(FALSE);
            if ( update )
            {
                UpdateWindow();
            }
            else
            {
                if (m_Abort)
                {
                    break;
                }
                Sleep(0);
            }
            long ticks = (lLastTick + m_lSleep)- GetTickCount();
            if ( ticks < 0 )
            {
                ticks = 0;
            }
            Sleep(ticks);
        }
        m_AnglesX[m_Size-1-depth] = 0.0;

        if ( depth == 0 )
        {
            RotateSideClockwise(1);
        }
        else if ( depth == m_Size-1 )
        {
            RotateSideCounterClockwise(3);
        }
        int y;
        for ( y = 0; y < m_Size; ++y )
        {
            int index = y*m_Size+depth;
            Piece temp = m_Sides[0][index];
            m_Sides[0][index] = m_Sides[5][index];
            m_Sides[5][index] = m_Sides[4][index];
            m_Sides[4][index] = m_Sides[2][index];
            m_Sides[2][index] = temp;
        }

        if (depth == m_Middle)
        {
            int temp = m_SideAngles[0];
            m_SideAngles[0] = m_SideAngles[5];
            m_SideAngles[5] = m_SideAngles[4];
            m_SideAngles[4] = m_SideAngles[2];
            m_SideAngles[2] = temp;
        }

        Invalidate(FALSE);
        if ( update )
        {
            UpdateWindow();
        }
        else
        {
            Sleep(10);
        }
    }
    IncrementMoves();
}

void CCubeView::RotateFront(int depth, bool update)
{
    if ( depth < m_Size )
    {
        while ( m_AnglesZ[depth] - m_dStep > -90 )
        {
            long lLastTick = GetTickCount();
            m_AnglesZ[depth] -= m_dStep;
            Invalidate(FALSE);
            if ( update )
            {
                UpdateWindow();
            }
            else
            {
                if (m_Abort)
                {
                    break;
                }
                Sleep(0);
            }
            long ticks = (lLastTick + m_lSleep)- GetTickCount();
            if ( ticks < 0 )
            {
                ticks = 0;
            }
            Sleep(ticks);
        }
        m_AnglesZ[depth] = 0.0;

        if ( depth == 0 )
        {
            RotateSideClockwise(2);
        }
        else if ( depth == m_Size-1 )
        {
            RotateSideCounterClockwise(5);
        }
        int y;
        for ( y = 0; y < m_Size; ++y )
        {
            int index1 = (m_Size-1-depth)*m_Size+(m_Size-1-y);
            int index2 = y*m_Size+(m_Size-1-depth);
            int index3 = depth*m_Size+y;
            int index4 = (m_Size-1-y)*m_Size+depth;
            Piece temp = m_Sides[0][index1];
            m_Sides[0][index1] = m_Sides[1][index2];
            m_Sides[1][index2] = m_Sides[4][index3];
            m_Sides[4][index3] = m_Sides[3][index4];
            m_Sides[3][index4] = temp;
        }

        if (depth == m_Middle)
        {
            int temp = m_SideAngles[0];
            m_SideAngles[0] = m_SideAngles[1];
            m_SideAngles[1] = m_SideAngles[4];
            m_SideAngles[4] = m_SideAngles[3];
            m_SideAngles[3] = temp;

            m_SideAngles[0]--;
            m_SideAngles[0]&=3;
            m_SideAngles[1]--;
            m_SideAngles[1]&=3;
            m_SideAngles[4]--;
            m_SideAngles[4]&=3;
            m_SideAngles[3]--;
            m_SideAngles[3]&=3;
        }

        Invalidate(FALSE);
        if ( update )
        {
            UpdateWindow();
        }
        else
        {
            Sleep(10);
        }
    }
    IncrementMoves();
}

void CCubeView::RotateBack(int depth, bool update)
{
    if ( depth < m_Size )
    {
        while ( m_AnglesZ[m_Size-1-depth] + m_dStep < 90 )
        {
            long lLastTick = GetTickCount();
            m_AnglesZ[m_Size-1-depth] += m_dStep;
            Invalidate(FALSE);
            if ( update )
            {
                UpdateWindow();
            }
            else
            {
                if (m_Abort)
                {
                    break;
                }
                Sleep(0);
            }
            long ticks = (lLastTick + m_lSleep)- GetTickCount();
            if ( ticks < 0 )
            {
                ticks = 0;
            }
            Sleep(ticks);
        }
        m_AnglesZ[m_Size-1-depth] = 0.0;

        if ( depth == 0 )
        {
            RotateSideClockwise(5);
        }
        else if ( depth == m_Size-1 )
        {
            RotateSideCounterClockwise(2);
        }
        int y;
        for ( y = 0; y < m_Size; ++y )
        {
            int index1 = depth*m_Size+(m_Size-1-y);
            int index2 = (m_Size-1-y)*m_Size+(m_Size-1-depth);
            int index3 = (m_Size-1-depth)*m_Size+y;
            int index4 = y*m_Size+depth;
            Piece temp = m_Sides[0][index1];
            m_Sides[0][index1] = m_Sides[3][index2];
            m_Sides[3][index2] = m_Sides[4][index3];
            m_Sides[4][index3] = m_Sides[1][index4];
            m_Sides[1][index4] = temp;
        }

        if (depth == m_Middle)
        {
            int temp = m_SideAngles[0];
            m_SideAngles[0] = m_SideAngles[3];
            m_SideAngles[3] = m_SideAngles[4];
            m_SideAngles[4] = m_SideAngles[1];
            m_SideAngles[1] = temp;

            m_SideAngles[0]++;
            m_SideAngles[0]&=3;
            m_SideAngles[1]++;
            m_SideAngles[1]&=3;
            m_SideAngles[4]++;
            m_SideAngles[4]&=3;
            m_SideAngles[3]++;
            m_SideAngles[3]&=3;
        }

        Invalidate(FALSE);
        if ( update )
        {
            UpdateWindow();
        }
        else
        {
            Sleep(10);
        }
    }
    IncrementMoves();
}

void CCubeView::RotateTop(int depth, bool update)
{
    if ( depth < m_Size )
    {
        while ( m_AnglesY[depth] - m_dStep > -90 )
        {
            long lLastTick = GetTickCount();
            m_AnglesY[depth] -= m_dStep;
            Invalidate(FALSE);
            if ( update )
            {
                UpdateWindow();
            }
            else
            {
                if (m_Abort)
                {
                    break;
                }
                Sleep(0);
            }
            long ticks = (lLastTick + m_lSleep)- GetTickCount();
            if ( ticks < 0 )
            {
                ticks = 0;
            }
            Sleep(ticks);
        }
        m_AnglesY[depth] = 0.0;

        if ( depth == 0 )
        {
            RotateSideClockwise(0);
        }
        else if ( depth == m_Size-1 )
        {
            RotateSideCounterClockwise(4);
        }
        int x;
        for ( x = 0; x < m_Size; ++x )
        {
            int index1 = depth*m_Size+x;
            int index2 = (m_Size-1-depth)*m_Size+(m_Size-1-x);
            Piece temp = m_Sides[1][index1];
            m_Sides[1][index1] = m_Sides[2][index1];
            m_Sides[2][index1] = m_Sides[3][index1];
            m_Sides[3][index1] = m_Sides[5][index2];
            m_Sides[5][index2] = temp;
        }

        if (depth == m_Middle)
        {
            int temp = m_SideAngles[1];
            m_SideAngles[1] = m_SideAngles[2];
            m_SideAngles[2] = m_SideAngles[3];
            m_SideAngles[3] = m_SideAngles[5];
            m_SideAngles[5] = temp;

            m_SideAngles[3]+=2;
            m_SideAngles[3]&=3;
            m_SideAngles[5]+=2;
            m_SideAngles[5]&=3;
        }

        Invalidate(FALSE);
        if ( update )
        {
            UpdateWindow();
        }
        else
        {
            Sleep(10);
        }
    }
    IncrementMoves();
}

void CCubeView::RotateBottom(int depth, bool update)
{
    if ( depth < m_Size )
    {
        while ( m_AnglesY[m_Size-1-depth] + m_dStep < 90 )
        {
            long lLastTick = GetTickCount();
            m_AnglesY[m_Size-1-depth] += m_dStep;
            Invalidate(FALSE);
            if ( update )
            {
                UpdateWindow();
            }
            else
            {
                if (m_Abort)
                {
                    break;
                }
                Sleep(0);
            }
            long ticks = (lLastTick + m_lSleep)- GetTickCount();
            if ( ticks < 0 )
            {
                ticks = 0;
            }
            Sleep(ticks);
        }
        m_AnglesY[m_Size-1-depth] = 0.0;

        if ( depth == m_Size-1 )
        {
            RotateSideCounterClockwise(0);
        }
        else if ( depth == 0 )
        {
            RotateSideClockwise(4);
        }
        int x;
        for ( x = 0; x < m_Size; ++x )
        {
            int index1 = (m_Size-1-depth)*m_Size+x;
            int index2 = depth*m_Size+(m_Size-1-x);
            Piece temp = m_Sides[1][index1];
            m_Sides[1][index1] = m_Sides[5][index2];
            m_Sides[5][index2] = m_Sides[3][index1];
            m_Sides[3][index1] = m_Sides[2][index1];
            m_Sides[2][index1] = temp;
        }

        if (depth == m_Middle)
        {
            int temp = m_SideAngles[1];
            m_SideAngles[1] = m_SideAngles[5];
            m_SideAngles[5] = m_SideAngles[3];
            m_SideAngles[3] = m_SideAngles[2];
            m_SideAngles[2] = temp;

            m_SideAngles[1]+=2;
            m_SideAngles[1]&=3;
            m_SideAngles[5]+=2;
            m_SideAngles[5]&=3;
        }

        Invalidate(FALSE);
        if ( update )
        {
            UpdateWindow();
        }
        else
        {
            Sleep(10);
        }
    }
    IncrementMoves();
}

bool CCubeView::IsEdge(int side1, int side2, int edge, int color1, int color2)
{
    switch (side1)
    {
    case 0:
        switch (side2)
        {
        case 1:
            return IsColor(side1, edge, 0, color1) && IsColor(side2, 0, edge, color2);
        case 2:
            return IsColor(side1, m_Size-1, edge, color1) && IsColor(side2, 0, edge, color2);
        case 3:
            return IsColor(side1, m_Size-1-edge, m_Size-1, color1) && IsColor(side2, 0, edge, color2);
        case 5:
            return IsColor(side1, 0, m_Size-1-edge, color1) && IsColor(side2, m_Size-1, m_Size-1-edge, color2);
        };
        break;
    case 1:
        switch (side2)
        {
        case 0:
            return IsColor(side1, 0, m_Size-1-edge, color1) && IsColor(side2, m_Size-1-edge, 0, color2);
        case 2:
            return IsColor(side1, m_Size-1-edge, m_Size-1, color1) && IsColor(side2, m_Size-1-edge, 0, color2);
        case 4:
            return IsColor(side1, m_Size-1, edge, color1) && IsColor(side2, m_Size-1-edge, 0, color2);
        case 5:
            return IsColor(side1, edge, 0, color1) && IsColor(side2, m_Size-1-edge, 0, color2);
        };
        break;
    case 2:
        switch (side2)
        {
        case 0:
            return IsColor(side1, 0, m_Size-1-edge, color1) && IsColor(side2, m_Size-1, m_Size-1-edge, color2);
        case 3:
            return IsColor(side1, m_Size-1-edge, m_Size-1, color1) && IsColor(side2, m_Size-1-edge, 0, color2);
        case 4:
            return IsColor(side1, m_Size-1, edge, color1) && IsColor(side2, 0, edge, color2);
        case 1:
            return IsColor(side1, edge, 0, color1) && IsColor(side2, edge, m_Size-1, color2);
        };
        break;
    case 3:
        switch (side2)
        {
        case 0:
            return IsColor(side1, 0, m_Size-1-edge, color1) && IsColor(side2, edge, m_Size-1, color2);
        case 5:
            return IsColor(side1, m_Size-1-edge, m_Size-1, color1) && IsColor(side2, edge, m_Size-1, color2);
        case 4:
            return IsColor(side1, m_Size-1, edge, color1) && IsColor(side2, edge, m_Size-1, color2);
        case 2:
            return IsColor(side1, edge, 0, color1) && IsColor(side2, edge, m_Size-1, color2);
        };
        break;
    case 4:
        switch (side2)
        {
        case 2:
            return IsColor(side1, 0, m_Size-1-edge, color1) && IsColor(side2, m_Size-1, m_Size-1-edge, color2);
        case 3:
            return IsColor(side1, m_Size-1-edge, m_Size-1, color1) && IsColor(side2, m_Size-1, m_Size-1-edge, color2);
        case 5:
            return IsColor(side1, m_Size-1, edge, color1) && IsColor(side2, 0, edge, color2);
        case 1:
            return IsColor(side1, edge, 0, color1) && IsColor(side2, m_Size-1, m_Size-1-edge, color2);
        };
        break;
    case 5:
        switch (side2)
        {
        case 4:
            return IsColor(side1, 0, m_Size-1-edge, color1) && IsColor(side2, m_Size-1, m_Size-1-edge, color2);
        case 3:
            return IsColor(side1, m_Size-1-edge, m_Size-1, color1) && IsColor(side2, edge, m_Size-1, color2);
        case 0:
            return IsColor(side1, m_Size-1, edge, color1) && IsColor(side2, 0, edge, color2);
        case 1:
            return IsColor(side1, edge, 0, color1) && IsColor(side2, m_Size-1-edge, 0, color2);
        };
        break;
    }
    return false;
}

int CCubeView::FixTopCenters()
{
    std::vector<Move> undoMoves;
    int numPiecesChanged = 0;
    if (m_Size&1)
    {
        switch ( m_SideAngles[0] )
        {
        case 0:
            break;
        case 1:
            AddMove(4, 0);
            break;
        case 2:
            AddMove(4, 0);
            AddMove(4, 0);
            break;
        case 3:
            AddMove(5, m_Size-1);
            break;
        }
    }
    for (int row = 1; row < (m_Size-1); ++row)
    {
        if ( m_Abort )
        {
            break;
        }
        bool found = false;
        int column;
        for (column = 1; column < (m_Size-1); ++column)
        {
            found = found || !IsColor(0, row, column, 0, row, column);
        }
        if (!found)
        {
            continue;
        }
        AddMove(3, row);
        for (column = 1; column < (m_Size-1); ++column)
        {
            if ( m_Abort )
            {
                break;
            }
            if ((row==m_Middle) && (column==m_Middle))
            {
                continue;
            }
            int type = 0;

            int row1 = m_Size-1-column;
            int column1 = row;

            if (IsColor(1, row1, column1, 0, row, column))
            {
                continue;
            }
         
            ++numPiecesChanged;
            int row2 = row1;
            int column2 = column1;
            int side;
            Piece p;
            p.m_Color = 0;
            p.m_Row = row;
            p.m_Column = column;
            if (HasCenterType(2, row2, column2, p))
            {
                side=2;
            }
            else if (HasCenterType(3, row2, column2, p))
            {
                side=3;
            }
            else if (HasCenterType(5, row2, column2, p))
            {
                side=5;
            }
            else if (HasCenterType(4, row2, column2, p))
            {
                AddMove(0, m_Size-1-column2);
                int angle;
                if ( (m_Size-1-row2) == column2 )
                {
                    AddMove(3, m_Size-1);
                    angle = 270;
                }
                else
                {
                    AddMove(2, 0);
                    angle = 90;
                }
                AddMove(1, column2);
                side=2;
                RotatePosition(row2, column2, angle);
            }
            else if (HasCenterType(1, row2, column2, p))
            {
                if ( column2 == row )
                {
                    RotatePosition(row2, column2, 90);
                    HasCenterType(1, row2, column2, p);
                }
                if ( column2 != row )
                {
                    AddMove(1, 0);
                    AddMove(4, column2);
                    AddMove(0, m_Size-1);
                    side=5;
                    RotatePosition(row2, column2, 270);
                }
                else
                {
                    AddMove(5, m_Size-1-row2);
                    AddMove(2, 0);
                    AddMove(2, 0);
                    AddMove(4, row2);
                    if (HasCenterType(2, row2, column2, p))
                    {
                        side=2;
                    }
                }
            }
            else if (HasCenterType(0, row2, column2, p, row))
            {
                if (row2 == m_Middle)
                {
                    AddMove(2, m_Size-1-row2, undoMoves);
                }
                AddMove(2, m_Size-1-row2);
                int angle = 90;
                if ((row2 == row) || (row2 == m_Middle))
                {
                    if (column2 == m_Size-1-row)
                    {
                        if (row2 == m_Middle)
                        {
                            AddMove(0, 0, undoMoves);
                        }
                        AddMove(0, 0);
                        angle = 180;
                    }
                    else
                    {
                        if (row2 == m_Middle)
                        {
                            AddMove(1, m_Size-1, undoMoves);
                        }
                        AddMove(1, m_Size-1);
                        angle = 0;
                    }

                    if (row2 == m_Middle)
                    {
                        AddMove(3, row2, undoMoves);
                    }
                    AddMove(3, row2);
                }
                side=3;
                RotatePosition(row2, column2, angle);
            }
            else
            {
                continue;
            }

            CenterRelativePositions &centerPositions = m_CentersPositions[1][side];
            RotatePosition(row1, column1, centerPositions.angle1);
            RotatePosition(row2, column2, centerPositions.angle2);

            int type1 = (centerPositions.twoMoves) ? centerPositions.type1^1 : centerPositions.type3;
            int type2 = centerPositions.type2;

            if ((row1 == row2) && (column1 == column2))
            {
            }
            else if ((row1 == column2) && (column1 == m_Size-1-row2))
            {
                AddMove(type1, 0);
            }
            else if ((row1 == m_Size-1-row2) && (column1 == m_Size-1-column2))
            {
                AddMove(type1, 0);
                AddMove(type1, 0);
            }
            else
            {
                AddMove(type1^1, m_Size-1);
            }
            AddMove(type2^1, m_Size-1-column1);
            if (centerPositions.twoMoves)
            {
                AddMove(type2^1, m_Size-1-column1);
            }
            PlayMovesReverse(undoMoves);

            column1 = row;
        }
        AddMove(2, m_Size-1-row);
    }
    return numPiecesChanged;
}

int CCubeView::FixBottomCenters()
{
    int numPiecesChanged = 0;
    if (m_Size&1)
    {
        switch ( m_SideAngles[4] )
        {
        case 0:
            break;
        case 1:
            AddMove(5, 0);
            break;
        case 2:
            AddMove(5, 0);
            AddMove(5, 0);
            break;
        case 3:
            AddMove(4, m_Size-1);
            break;
        }
    }
    for (int row = 1; row < (m_Size-1); ++row)
    {
        if ( m_Abort )
        {
            break;
        }
        bool found = false;
        int column;
        for (column = 1; column < (m_Size-1); ++column)
        {
            found = found || !IsColor(4, row, column, 4, row, column);
        }
        if (!found)
        {
            continue;
        }
        AddMove(2, row);
        AddMove(0, m_Size-1);
        if (row != m_Middle)
        {
            AddMove(0, m_Size-1);
        }
        AddMove(3, m_Size-1-row);
        if (row == m_Middle)
        {
            AddMove(0, m_Size-1);
        }
        for (column = 1; column < (m_Size-1); ++column)
        {
            if ( m_Abort )
            {
                break;
            }
            if ((row==m_Middle) && (column==m_Middle))
            {
                continue;
            }
            int type = 0;

            int row1 = m_Size-1-column;
            int column1 = row;

            if (IsColor(1, row1, column1, 4, row, column))
            {
                continue;
            }
         
            ++numPiecesChanged;
            int row2 = row1;
            int column2 = column1;
            int side;
            Piece p;
            p.m_Color = 4;
            p.m_Row = row;
            p.m_Column = column;
            if (HasCenterType(2, row2, column2, p))
            {
                side=2;
            }
            else if (HasCenterType(3, row2, column2, p))
            {
                side=3;
            }
            else if (HasCenterType(5, row2, column2, p))
            {
                side=5;
            }
            else if (HasCenterType(1, row2, column2, p))
            {
                if ( column2 == row )
                {
                    RotatePosition(row2, column2, 90);
                    HasCenterType(1, row2, column2, p);
                }
                if ( column2 != row )
                {
                    AddMove(1, 0);
                    AddMove(4, column2);
                    AddMove(0, m_Size-1);
                    side=5;
                    RotatePosition(row2, column2, 270);
                }
                else
                {
                    AddMove(5, m_Size-1-row2);
                    AddMove(2, 0);
                    AddMove(2, 0);
                    AddMove(4, row2);
                    if (HasCenterType(2, row2, column2, p))
                    {
                        side=2;
                    }
                }
            }
            else if (HasCenterType(4, row2, column2, p, row))
            {
                AddMove(3, m_Size-1-row2);
                if (row2 == m_Middle)
                {
                    if (row == m_Size-1-column2)
                    {
                        AddMove(0, m_Size-1);
                    }
                    else
                    {
                        AddMove(1, 0);
                    }
                    AddMove(4, m_Size-1-column2);
                    if (row == m_Size-1-column2)
                    {
                        AddMove(1, 0);
                    }
                    else
                    {
                        AddMove(0, m_Size-1);
                    }
                    AddMove(2, row2);
                    side=2;
                    row2=m_Size-1-column2;
                    column2=m_Middle;
                }
                else
                {
                    int angle;
                    if ( column2 == row2 )
                    {
                        AddMove(1, m_Size-1);
                        angle = 180;
                    }
                    else
                    {
                        AddMove(0, 0);
                        angle = 0;
                    }
                    AddMove(2, row2);
                    side=3;
                    RotatePosition(row2, column2, angle);
                }
            }
            else
            {
                continue;
            }

            CenterRelativePositions &centerPositions = m_CentersPositions[1][side];
            RotatePosition(row1, column1, centerPositions.angle1);
            RotatePosition(row2, column2, centerPositions.angle2);

            int type1 = (centerPositions.twoMoves) ? centerPositions.type1^1 : centerPositions.type3;
            int type2 = centerPositions.type2;

            if ((row1 == row2) && (column1 == column2))
            {
            }
            else if ((row1 == column2) && (column1 == m_Size-1-row2))
            {
                AddMove(type1, 0);
            }
            else if ((row1 == m_Size-1-row2) && (column1 == m_Size-1-column2))
            {
                AddMove(type1, 0);
                AddMove(type1, 0);
            }
            else
            {
                AddMove(type1^1, m_Size-1);
            }
            AddMove(type2^1, m_Size-1-column1);
            if (centerPositions.twoMoves)
            {
                AddMove(type2^1, m_Size-1-column1);
            }

            column1 = row;
        }
        if (row == m_Middle)
        {
            AddMove(1, 0);
        }
        AddMove(2, row);
        AddMove(1, 0);
        if (row != m_Middle)
        {
            AddMove(1, 0);
        }
        AddMove(3, m_Size-1-row);
    }
    return numPiecesChanged;
}

int CCubeView::FixEdgeCenter(int side1, int side2, int side3, int side4, int color, int typeA, int typeB, int column, int min, bool preserve4)
{
    int numPiecesChanged = 0;
    for (int row = 1; row < (m_Size-1); ++row)
    {
        if ( m_Abort )
        {
            break;
        }
        if (IsColor(side1, row, column, color, column, m_Size-1-row))
        {
            continue;
        }

        ++numPiecesChanged;
        int row2 = row;
        int column2 = column;
        int side;
        Piece p;
        p.m_Color = color;
        p.m_Row = column;
        p.m_Column = m_Size-1-row;
        if (HasCenterType(side2, row2, column2, p))
        {
            side=side2;
        }
        else if (HasCenterType(side3, row2, column2, p))
        {
            side=side3;
        }
        else if (HasCenterType(side1, row2, column2, p))
        {
            if ( column2 == column )
            {
                RotatePosition(row2, column2, 90);
                HasCenterType(side1, row2, column2, p);
            }
            if ( column2 != column )
            {
                AddMove(typeA, 0);
                AddMove(5, m_Size-1-column2);
                int angle;
                if (row2 == column2)
                {
                    AddMove(typeB, 0);
                    angle = 180;
                }
                else
                {
                    AddMove(typeB^1, m_Size-1);
                    angle = 0;
                }
                AddMove(4, column2);
                AddMove(typeA^1, m_Size-1);
                side=side2;
                RotatePosition(row2, column2, (side2 == 5) ? 180 - angle : angle);
            }
            else
            {
                AddMove(5, m_Size-1-row2);
                AddMove(typeB, 0);
                AddMove(typeB, 0);
                AddMove(4, row2);
                if (HasCenterType(side2, row2, column2, p))
                {
                    side=side2;
                }
            }
        }
        else if (HasCenterType(side4, row2, column2, p, min, preserve4))
        {
            side=side4;
            if (preserve4)
            {
                side=side2;
                AddMove(4, row2);
                AddMove(4, row2);
                if  (column2 == row2)
                {
                    AddMove(typeB^1, m_Size-1);
                    AddMove(5, m_Size-1-row2);
                    AddMove(5, m_Size-1-row2);
                    RotatePosition(row2, column2, (side == 5) ? 90 : 270);
                }
                else
                {
                    AddMove(typeB, 0);
                    AddMove(5, m_Size-1-row2);
                    AddMove(5, m_Size-1-row2);
                    RotatePosition(row2, column2, (side == 5) ? 270 : 90);
                }
            }
        }
        else
        {
            continue;
        }

        int row1 = row;
        int column1 = column;
        CenterRelativePositions &centerPositions = m_CentersPositions[side1][side];
        RotatePosition(row1, column1, centerPositions.angle1);
        RotatePosition(row2, column2, centerPositions.angle2);

        int type1 = (centerPositions.twoMoves) ? centerPositions.type1^1 : centerPositions.type3;
        int type2 = centerPositions.type2;

        int rotateAmount = 0;
        if ((row1 == row2) && (column1 == column2))
        {
        }
        else if ((row1 == column2) && (column1 == m_Size-1-row2))
        {
            rotateAmount = 1;
            AddMove(type1, 0);
        }
        else if ((row1 == m_Size-1-row2) && (column1 == m_Size-1-column2))
        {
            rotateAmount = 2;
            AddMove(type1, 0);
            AddMove(type1, 0);
        }
        else
        {
            rotateAmount = 3;
            AddMove(type1^1, m_Size-1);
        }
        AddMove(type2^1, m_Size-1-column1);
        if (centerPositions.twoMoves)
        {
            AddMove(type2^1, m_Size-1-column1);
        }

        if (preserve4)
        {
            if (column1 == row1)
            {
                AddMove(typeA, 0);
            }
            else
            {
                AddMove(typeA^1, m_Size-1);
            }

            AddMove(type2, column1);
            if (centerPositions.twoMoves)
            {
                AddMove(type2, column1);
            }

            if (column1 != row1)
            {
                AddMove(typeA, 0);
            }
            else
            {
                AddMove(typeA^1, m_Size-1);
            }

            if (side == side4)
            {
                while (rotateAmount)
                {
                    AddMove(type1^1, m_Size-1);
                    --rotateAmount;
                }
            }
        }

        column1 = column;
    }
    return numPiecesChanged;
}

int CCubeView::FixEdgeCenters(int side1, int side2, int side3, int side4, int type1, int type2, bool preserve4)
{
    int numPiecesChanged = 0;
    bool found = false;
    int column;
    if (m_Size&1)
    {
        if (!IsColor(side1, (m_Size-1)/2, (m_Size-1)/2, side1))
        {
            ++numPiecesChanged;
            if (IsColor(side2, (m_Size-1)/2, (m_Size-1)/2, side1))
            {
                AddMove(4, m_Middle);
            }
            else if (IsColor(side3, (m_Size-1)/2, (m_Size-1)/2, side1))
            {
                AddMove(4, m_Middle);
                AddMove(4, m_Middle);
            }
            else if (IsColor(side4, (m_Size-1)/2, (m_Size-1)/2, side1))
            {
                AddMove(5, m_Middle);
            }
            PlayMoves();
        }
        
        switch ( m_SideAngles[side1] )
        {
        case 0:
            break;
        case 1:
            AddMove(type1, 0);
            break;
        case 2:
            AddMove(type1, 0);
            AddMove(type1, 0);
            break;
        case 3:
            AddMove(type1^1, m_Size-1);
            break;
        }

        for (column = 1; column < (m_Size-1); ++column)
        {
            found = found || !IsColor(side1, m_Middle, column, side1, m_Middle, column);
        }
        if (found)
        {
            AddMove(type1^1, m_Size-1);
            numPiecesChanged += FixEdgeCenter(side1, side2, side3, side4, side1, type1, type2, m_Middle, 0, preserve4);
            AddMove(type1, 0);
        }
    }

    for (int row = 1; row < (m_Size-1); ++row)
    {
        if ( m_Abort )
        {
            break;
        }
        if (row == m_Middle)
        {
            continue;
        }
        found = false;
        for (column = 1; column < (m_Size-1); ++column)
        {
            found = found || !IsColor(side1, row, column, side1, row, column);
        }
        if (!found)
        {
            continue;
        }

        AddMove(5, m_Size-1-row);
        AddMove(type2, 0);
        AddMove(type2, 0);
        AddMove(4, row);
        AddMove(type2, 0);
        numPiecesChanged += FixEdgeCenter(side2, side3, side4, side1, side1, type2, type1^1, row, row, true);
        AddMove(type2^1, m_Size-1);
        AddMove(5, m_Size-1-row);
        AddMove(type2^1, m_Size-1);
        AddMove(type2^1, m_Size-1);
        AddMove(4, row);
    }
    return numPiecesChanged;
}

// side1 is equivalent to back
// side2 is equivalent to left
// side3 is equivalent to front
// side4 is equivalent to right
// type1 rotates side1
// type2 rotates side2
void CCubeView::FixTopEdge(int side1, int side2, int side3, int side4, int type1, int type2)
{
    bool found = false;
    int edge;
    for (edge = 1; edge < m_Size-1; ++edge)
    {
        found = found || !IsEdge(0, side1, edge, 0, side1);
    }
    if (!found)
    {
        return;
    }
    AddMove(type2, 0);

    found = !FixEdgesOnSide(side1, side2, side3, side4, 0, side1, type1, type2, true);
    if (found)
    {
        found = !FixEdgesOnSide(side1, side2, side3, side4, 0, side1, type1, type2, false);
    }

    int topCount = 0;
    int bottomCount = 0;
    int edgeCount = 0;
    while (found)
    {
        found = false;
        // check if any on the top with 0 on top
        for (edge = 1; edge < m_Size-1; ++edge)
        {
            if (IsEdge(0, side1, edge, 0, side1))
            {
                found = true;
                break;
            }
            else if (IsEdge(0, side2, edge, 0, side1))
            {
                AddMove(4, 0);
                topCount += 1;
                found = true;
                break;
            }
            else if (IsEdge(0, side3, edge, 0, side1))
            {
                AddMove(4, 0);
                AddMove(4, 0);
                topCount += 2;
                found = true;
                break;
            }
            else if (IsEdge(0, side4, edge, 0, side1))
            {
                AddMove(5, m_Size-1);
                topCount += 3;
                found = true;
                break;
            }
        }
        // check if any on the bottom with 0 on the side
        for (edge = 1; edge < m_Size-1; ++edge)
        {
            if (IsEdge(side1, 4, edge, 0, side1))
            {
                found = true;
                break;
            }
            else if (IsEdge(side2, 4, edge, 0, side1))
            {
                AddMove(4, m_Size-1);
                bottomCount += 3;
                found = true;
                break;
            }
            else if (IsEdge(side3, 4, edge, 0, side1))
            {
                AddMove(5, 0);
                AddMove(5, 0);
                bottomCount += 2;
                found = true;
                break;
            }
            else if (IsEdge(side4, 4, edge, 0, side1))
            {
                AddMove(5, 0);
                bottomCount += 1;
                found = true;
                break;
            }
        }
        if (found)
        {
            edgeCount += 1;
            AddMove(type1, 0);
            while (topCount)
            {
                AddMove(5, m_Size-1);
                --topCount;
            }
        }
        else
        {
            // check if any on the top with 0 on side
            for (edge = 1; edge < m_Size-1; ++edge)
            {
                if (IsEdge(side1, 0, edge, 0, side1))
                {
                    found = true;
                    break;
                }
                else if (IsEdge(side2, 0, edge, 0, side1))
                {
                    AddMove(4, 0);
                    topCount += 1;
                    found = true;
                    break;
                }
                else if (IsEdge(side3, 0, edge, 0, side1))
                {
                    AddMove(4, 0);
                    AddMove(4, 0);
                    topCount += 2;
                    found = true;
                    break;
                }
                else if (IsEdge(side4, 0, edge, 0, side1))
                {
                    AddMove(5, m_Size-1);
                    topCount += 3;
                    found = true;
                    break;
                }
            }
            // check if any on the bottom with 0 on the bottom
            for (edge = 1; edge < m_Size-1; ++edge)
            {
                if (IsEdge(4, side1, edge, 0, side1))
                {
                    found = true;
                    break;
                }
                else if (IsEdge(4, side2, edge, 0, side1))
                {
                    AddMove(4, m_Size-1);
                    bottomCount += 3;
                    found = true;
                    break;
                }
                else if (IsEdge(4, side3, edge, 0, side1))
                {
                    AddMove(5, 0);
                    AddMove(5, 0);
                    bottomCount += 2;
                    found = true;
                    break;
                }
                else if (IsEdge(4, side4, edge, 0, side1))
                {
                    AddMove(5, 0);
                    bottomCount += 1;
                    found = true;
                    break;
                }
            }
            if (found)
            {
                edgeCount += 3;
                AddMove(type1^1, m_Size-1);
                while (topCount)
                {
                    AddMove(5, m_Size-1);
                    --topCount;
                }
            }
        }
        found = !FixEdgesOnSide(side1, side2, side3, side4, 0, side1, type1, type2, true);
        if (found)
        {
            found = !FixEdgesOnSide(side1, side2, side3, side4, 0, side1, type1, type2, false);
        }
    }

    while (edgeCount)
    {
        AddMove(type1^1, m_Size-1);
        --edgeCount;
    }
    while (bottomCount)
    {
        AddMove(4, m_Size-1);
        --bottomCount;
    }
    // all assembled, now move to top
    AddMove(5, m_Size-1);
    AddMove(type2^1, m_Size-1);
    AddMove(4, 0);
}

// side1 is equivalent to back
// side2 is equivalent to left
// side3 is equivalent to front
// side4 is equivalent to right
// type1 rotates side1
// type2 rotates side2
// top has been rotated to have the non-fixed edge a back
void CCubeView::FixBottomEdge(int side1, int side2, int side3, int side4, int type1, int type2)
{
    bool found = false;
    int edge;
    for (edge = 1; edge < m_Size-1; ++edge)
    {
        found = found || !IsEdge(4, side1, edge, 4, side1);
    }
    if (!found)
    {
        return;
    }

    switch (side1)
    {
    case 2:
        AddMove(5, m_Size-1);
    case 1:
        AddMove(5, m_Size-1);
    case 5:
        AddMove(5, m_Size-1);
    }

    AddMove(5, m_Size-1);
    AddMove(5, 0);
    AddMove(type2^1, m_Size-1);
    AddMove(4, 0);
    AddMove(4, m_Size-1);

    found = !FixEdgesOnSide(side1, side2, side3, side4, 4, side1, type1, type2, false);
    if (found)
    {
        found = !FixEdgesOnSide(side1, side2, side3, side4, 4, side1, type1, type2, false);
    }

    int edgeCount = 0;
    int bottomCount = 0;
    while (found)
    {
        found = false;
        // check if any on the top with 4 on top
        for (edge = 1; edge < m_Size-1; ++edge)
        {
            if (IsEdge(0, side1, edge, 4, side1))
            {
                found = true;
                break;
            }
        }
        // check if any on the bottom with 4 on the side
        for (edge = 1; edge < m_Size-1; ++edge)
        {
            if (IsEdge(side1, 4, edge, 4, side1))
            {
                found = true;
                break;
            }
            else if (IsEdge(side2, 4, edge, 4, side1))
            {
                AddMove(4, m_Size-1);
                bottomCount += 3;
                found = true;
                break;
            }
            else if (IsEdge(side3, 4, edge, 4, side1))
            {
                AddMove(5, 0);
                AddMove(5, 0);
                bottomCount += 2;
                found = true;
                break;
            }
            else if (IsEdge(side4, 4, edge, 4, side1))
            {
                AddMove(5, 0);
                bottomCount += 1;
                found = true;
                break;
            }
        }
        if (found)
        {
            edgeCount += 1;
            AddMove(type1, 0);
            while (bottomCount)
            {
                AddMove(4, m_Size-1);
                --bottomCount;
            }
        }
        else
        {
            // check if any on the top with 4 on side
            for (edge = 1; edge < m_Size-1; ++edge)
            {
                if (IsEdge(side1, 0, edge, 4, side1))
                {
                    found = true;
                    break;
                }
            }
            // check if any on the bottom with 4 on the bottom
            for (edge = 1; edge < m_Size-1; ++edge)
            {
                if (IsEdge(4, side1, edge, 4, side1))
                {
                    found = true;
                    break;
                }
                else if (IsEdge(4, side2, edge, 4, side1))
                {
                    AddMove(4, m_Size-1);
                    bottomCount += 3;
                    found = true;
                    break;
                }
                else if (IsEdge(4, side3, edge, 4, side1))
                {
                    AddMove(5, 0);
                    AddMove(5, 0);
                    bottomCount += 2;
                    found = true;
                    break;
                }
                else if (IsEdge(4, side4, edge, 4, side1))
                {
                    AddMove(5, 0);
                    bottomCount += 1;
                    found = true;
                    break;
                }
            }
            if (found)
            {
                edgeCount += 3;
                AddMove(type1^1, m_Size-1);
                while (bottomCount)
                {
                    AddMove(4, m_Size-1);
                    --bottomCount;
                }
            }
        }
        found = !FixEdgesOnSide(side1, side2, side3, side4, 4, side1, type1, type2, false);
        if (found)
        {
            found = !FixEdgesOnSide(side1, side2, side3, side4, 4, side1, type1, type2, false);
        }
    }

    while ( edgeCount )
    {
        AddMove(type1^1, m_Size-1);
        --edgeCount;
    }

    // all assembled, now move to bottom
    AddMove(5, 0);
    AddMove(5, m_Size-1);
    AddMove(type2, 0);
    AddMove(4, m_Size-1);
    AddMove(4, 0);

    switch (side1)
    {
    case 2:
        AddMove(4, 0);
    case 1:
        AddMove(4, 0);
    case 5:
        AddMove(4, 0);
    }
}

void CCubeView::FixEdgeCenter(int side1, int side2)
{
    int edge = (m_Size-1)/2;
    if (IsEdge(side1, side2, edge, side1, side2))
    {
        return;
    }
    if (!IsEdge(3, 0, edge, side1, side2))
    {
        if (IsEdge(0, 3, edge, side1, side2))
        {
            AddMove(5, edge);
            AddMove(3, 0);
            AddMove(5, m_Size-1);
            AddMove(2, m_Size-1);
            AddMove(4, m_Size-1-edge);
            AddMove(3, 0);
            AddMove(4, 0);
            AddMove(2, m_Size-1);
        }
        if (IsEdge(1, 2, edge, side1, side2))
        {
            AddMove(3, m_Size-1);
            AddMove(4, 0);
            AddMove(2, 0);
            AddMove(5, edge);
            AddMove(3, m_Size-1);
            AddMove(5, m_Size-1);
            AddMove(2, 0);
            AddMove(4, m_Size-1-edge);
        }
        else if (IsEdge(2, 3, edge, side1, side2))
        {
            AddMove(4, m_Size-1-edge);
            AddMove(3, m_Size-1);
            AddMove(4, 0);
            AddMove(2, 0);
            AddMove(5, edge);
            AddMove(3, m_Size-1);
            AddMove(5, m_Size-1);
            AddMove(2, 0);
        }
        else if (IsEdge(3, 5, edge, side1, side2))
        {
            AddMove(3, m_Size-1);
            AddMove(4, 0);
            AddMove(2, 0);
            AddMove(4, m_Size-1-edge);
            AddMove(3, m_Size-1);
            AddMove(5, m_Size-1);
            AddMove(2, 0);
            AddMove(5, edge);
        }
        else if (IsEdge(5, 1, edge, side1, side2))
        {
            AddMove(3, m_Size-1);
            AddMove(4, 0);
            AddMove(2, 0);
            AddMove(4, m_Size-1-edge);
            AddMove(4, m_Size-1-edge);
            AddMove(3, m_Size-1);
            AddMove(5, m_Size-1);
            AddMove(2, 0);
            AddMove(5, edge);
            AddMove(5, edge);
        }
        else if (IsEdge(2, 1, edge, side1, side2))
        {
            AddMove(5, m_Size-1-edge);
            AddMove(3, 0);
            AddMove(5, m_Size-1);
            AddMove(2, m_Size-1);
            AddMove(5, m_Size-1-edge);
            AddMove(3, 0);
            AddMove(4, 0);
            AddMove(2, m_Size-1);
            AddMove(4, edge);
            AddMove(4, edge);
        }
        else if (IsEdge(3, 2, edge, side1, side2))
        {
            AddMove(3, 0);
            AddMove(5, m_Size-1);
            AddMove(2, m_Size-1);
            AddMove(5, m_Size-1-edge);
            AddMove(3, 0);
            AddMove(4, 0);
            AddMove(2, m_Size-1);
            AddMove(4, edge);
        }
        else if (IsEdge(5, 3, edge, side1, side2))
        {
            AddMove(5, m_Size-1-edge);
            AddMove(3, 0);
            AddMove(5, m_Size-1);
            AddMove(2, m_Size-1);
            AddMove(4, edge);
            AddMove(3, 0);
            AddMove(4, 0);
            AddMove(2, m_Size-1);
        }
        else if (IsEdge(1, 5, edge, side1, side2))
        {
            AddMove(3, 0);
            AddMove(5, m_Size-1);
            AddMove(2, m_Size-1);
            AddMove(4, edge);
            AddMove(3, 0);
            AddMove(4, 0);
            AddMove(2, m_Size-1);
            AddMove(5, m_Size-1-edge);
        }
    }
    int topRotates = 0;
    int type1;
    int type2;
    switch (side1)
    {
    case 3:
        type1 = 0;
        type2 = 3;
        break;
    case 5:
        type1 = 3;
        type2 = 1;
        AddMove(5, m_Size-1);
        topRotates = 1;
        break;
    case 1:
        type1 = 1;
        type2 = 2;
        AddMove(5, m_Size-1);
        AddMove(5, m_Size-1);
        topRotates = 2;
        break;
    case 2:
        type1 = 2;
        type2 = 0;
        AddMove(4, 0);
        topRotates = 3;
        break;
    }
    AddMove(type1,   0);
    AddMove(type2^1, m_Size-1);
    AddMove(type1^1, m_Size-1);
    AddMove(type2,   0);
    AddMove(5,       0);
    AddMove(5,       m_Size-1);
    AddMove(type1,   m_Size-1);
    AddMove(type2,   0);
    AddMove(type1^1, 0);
    AddMove(type2^1, m_Size-1);
    AddMove(4,       m_Size-1);
    AddMove(4,       0);
    while (topRotates)
    {
        AddMove(4, 0);
        --topRotates;
    }
}

void CCubeView::FlipCenterEdge()
{
    int edge = (m_Size-1)/2;
    if (IsEdge(3, 5, edge, 3, 5))
    {
        return;
    }
    AddMove(3, 0);
    AddMove(5, m_Size-1);
    AddMove(2, m_Size-1);
    AddMove(5, edge);
    AddMove(3, 0);
    AddMove(4, 0);
    AddMove(2, m_Size-1);
    AddMove(4, m_Size-1-edge);

    AddMove(2, 0);
    AddMove(5, 0);
    AddMove(1, m_Size-1);

    AddMove(5, edge);
    AddMove(3, 0);
    AddMove(5, m_Size-1);
    AddMove(2, m_Size-1);
    AddMove(4, m_Size-1-edge);
    AddMove(3, 0);
    AddMove(4, 0);
    AddMove(2, m_Size-1);

    AddMove(0, 0);
    AddMove(4, m_Size-1);
    AddMove(3, m_Size-1);
}

// only top next to side3 is not complete, bottom is complete
void CCubeView::FixSideEdge(int side1, int side2)
{
    bool found = false;
    for (int edge=1; edge<m_Size-1; ++edge)
    {
        found = found || !IsEdge(side1, side2, edge, side1, side2);
    }
    if (!found)
    {
        return;
    }
    int topRotates = 0;
    int type1;
    int type2;
    if (m_Size&1)
    {
        switch (side1)
        {
        case 3:
            type1 = 0;
            type2 = 3;
            break;
        case 5:
            type1 = 3;
            type2 = 1;
            AddMove(5, m_Size-1);
            topRotates = 1;
            break;
        case 1:
            type1 = 1;
            type2 = 2;
            AddMove(5, m_Size-1);
            AddMove(5, m_Size-1);
            topRotates = 2;
            break;
        case 2:
            type1 = 2;
            type2 = 0;
            AddMove(4, 0);
            topRotates = 3;
            break;
        }
        AddMove(5,       m_Size-1);
        AddMove(5,       0);
        AddMove(type2,   0);
        AddMove(type1,   m_Size-1);
        AddMove(type2^1, m_Size-1);
        AddMove(type1^1, 0);
        AddMove(4,       0);
        AddMove(4,       m_Size-1);
        AddMove(type2^1, m_Size-1);
        AddMove(type1,   0);
        AddMove(type2,   0);
        AddMove(type1^1, m_Size-1);
        while (topRotates)
        {
            AddMove(4, 0);
            --topRotates;
        }
    }
    for (int edge=1; edge<m_Size-1; ++edge)
    {
        if (!IsEdge(3, 0, edge, side1, side2))
        {
            if (IsEdge(0, 3, edge, side1, side2))
            {
                AddMove(5, edge);
                AddMove(3, 0);
                AddMove(5, m_Size-1);
                AddMove(2, m_Size-1);
                AddMove(4, m_Size-1-edge);
                AddMove(3, 0);
                AddMove(4, 0);
                AddMove(2, m_Size-1);
            }
            if (IsEdge(1, 2, edge, side1, side2))
            {
                AddMove(3, m_Size-1);
                AddMove(4, 0);
                AddMove(2, 0);
                AddMove(5, edge);
                AddMove(3, m_Size-1);
                AddMove(5, m_Size-1);
                AddMove(2, 0);
                AddMove(4, m_Size-1-edge);
            }
            else if (IsEdge(2, 3, edge, side1, side2))
            {
                AddMove(4, m_Size-1-edge);
                AddMove(3, m_Size-1);
                AddMove(4, 0);
                AddMove(2, 0);
                AddMove(5, edge);
                AddMove(3, m_Size-1);
                AddMove(5, m_Size-1);
                AddMove(2, 0);
            }
            else if (IsEdge(3, 5, edge, side1, side2))
            {
                AddMove(3, m_Size-1);
                AddMove(4, 0);
                AddMove(2, 0);
                AddMove(4, m_Size-1-edge);
                AddMove(3, m_Size-1);
                AddMove(5, m_Size-1);
                AddMove(2, 0);
                AddMove(5, edge);
            }
            else if (IsEdge(5, 1, edge, side1, side2))
            {
                AddMove(3, m_Size-1);
                AddMove(4, 0);
                AddMove(2, 0);
                AddMove(4, m_Size-1-edge);
                AddMove(4, m_Size-1-edge);
                AddMove(3, m_Size-1);
                AddMove(5, m_Size-1);
                AddMove(2, 0);
                AddMove(5, edge);
                AddMove(5, edge);
            }
            else if (IsEdge(2, 1, edge, side1, side2))
            {
                AddMove(5, m_Size-1-edge);
                AddMove(3, 0);
                AddMove(5, m_Size-1);
                AddMove(2, m_Size-1);
                AddMove(5, m_Size-1-edge);
                AddMove(3, 0);
                AddMove(4, 0);
                AddMove(2, m_Size-1);
                AddMove(4, edge);
                AddMove(4, edge);
            }
            else if (IsEdge(3, 2, edge, side1, side2))
            {
                AddMove(3, 0);
                AddMove(5, m_Size-1);
                AddMove(2, m_Size-1);
                AddMove(5, m_Size-1-edge);
                AddMove(3, 0);
                AddMove(4, 0);
                AddMove(2, m_Size-1);
                AddMove(4, edge);
            }
            else if (IsEdge(5, 3, edge, side1, side2))
            {
                AddMove(5, m_Size-1-edge);
                AddMove(3, 0);
                AddMove(5, m_Size-1);
                AddMove(2, m_Size-1);
                AddMove(4, edge);
                AddMove(3, 0);
                AddMove(4, 0);
                AddMove(2, m_Size-1);
            }
            else if (IsEdge(1, 5, edge, side1, side2))
            {
                AddMove(3, 0);
                AddMove(5, m_Size-1);
                AddMove(2, m_Size-1);
                AddMove(4, edge);
                AddMove(3, 0);
                AddMove(4, 0);
                AddMove(2, m_Size-1);
                AddMove(5, m_Size-1-edge);
            }
        }
    }
    topRotates = 0;
    switch (side1)
    {
    case 3:
        type1 = 0;
        type2 = 3;
        break;
    case 5:
        type1 = 3;
        type2 = 1;
        AddMove(5, m_Size-1);
        topRotates = 1;
        break;
    case 1:
        type1 = 1;
        type2 = 2;
        AddMove(5, m_Size-1);
        AddMove(5, m_Size-1);
        topRotates = 2;
        break;
    case 2:
        type1 = 2;
        type2 = 0;
        AddMove(4, 0);
        topRotates = 3;
        break;
    }
    AddMove(type1,   0);
    AddMove(type2^1, m_Size-1);
    AddMove(type1^1, m_Size-1);
    AddMove(type2,   0);
    AddMove(5,       0);
    AddMove(5,       m_Size-1);
    AddMove(type1,   m_Size-1);
    AddMove(type2,   0);
    AddMove(type1^1, 0);
    AddMove(type2^1, m_Size-1);
    AddMove(4,       m_Size-1);
    AddMove(4,       0);
    while (topRotates)
    {
        AddMove(4, 0);
        --topRotates;
    }
}

void CCubeView::FixSecondLastSideEdge()
{
    for (int edge=1; edge<m_Size-1; ++edge)
    {
        if (!IsEdge(3, 5, edge, 3, 5))
        {
            // is at other location same edge
            if (IsEdge(5, 3, edge, 3, 5))
            {
                // move to top
                AddMove(5, m_Size-1-edge);
                AddMove(3, 0);
                AddMove(5, m_Size-1);
                AddMove(2, m_Size-1);
                AddMove(4, edge);
                AddMove(3, 0);
                AddMove(4, 0);
                AddMove(2, m_Size-1);

                AddMove(4, edge);
                AddMove(3, m_Size-1);
                AddMove(4, 0);
                AddMove(2, 0);
                AddMove(5, m_Size-1-edge);
                AddMove(3, m_Size-1);
                AddMove(5, m_Size-1);
                AddMove(2, 0);
            }
            if (IsEdge(0, 3, edge, 3, 5))
            {
                AddMove(5, m_Size-1);

                AddMove(4, m_Size-1-edge);
                AddMove(1, m_Size-1);
                AddMove(4, 0);
                AddMove(0, 0);
                AddMove(5, edge);
                AddMove(1, m_Size-1);
                AddMove(5, m_Size-1);
                AddMove(0, 0);

                AddMove(5, edge);
                AddMove(1, 0);
                AddMove(5, m_Size-1);
                AddMove(0, m_Size-1);
                AddMove(4, m_Size-1-edge);
                AddMove(1, 0);
                AddMove(4, 0);
                AddMove(0, m_Size-1);

                AddMove(4, 0);
            }
            else if (IsEdge(3, 0, edge, 3, 5))
            {
                AddMove(5, edge);
                AddMove(3, 0);
                AddMove(5, m_Size-1);
                AddMove(2, m_Size-1);
                AddMove(4, m_Size-1-edge);
                AddMove(3, 0);
                AddMove(4, 0);
                AddMove(2, m_Size-1);

                AddMove(4, m_Size-1-edge);
                AddMove(3, m_Size-1);
                AddMove(4, 0);
                AddMove(2, 0);
                AddMove(5, edge);
                AddMove(3, m_Size-1);
                AddMove(5, m_Size-1);
                AddMove(2, 0);
            }
        }
    }
}

bool CCubeView::FixLastSideEdge()
{
    bool found = false;
    std::vector<int> edges;
    for (int edge=(m_Size-1)/2; edge>0; --edge)
    {
        if (!IsEdge(0, 3, edge, 0, 3))
        {
            edges.push_back(edge);
            found = true;
        }
    }
    if (found)
    {
        unsigned index;
        for (index = 0; index < edges.size(); ++index)
        {
            AddMove(4, edges[index]);
        }
        AddMove(3, 0);
        AddMove(3, 0);
        for (index = 0; index < edges.size(); ++index)
        {
            AddMove(4, edges[index]);
        }
        AddMove(3, 0);
        AddMove(3, 0);
        for (index = 0; index < edges.size(); ++index)
        {
            AddMove(4, edges[index]);
        }
        AddMove(3, 0);
        AddMove(3, 0);
        for (index = 0; index < edges.size(); ++index)
        {
            AddMove(4, edges[index]);
        }
        AddMove(3, 0);
        AddMove(3, 0);
        for (index = 0; index < edges.size(); ++index)
        {
            AddMove(4, edges[index]);
        }
    }
    return found;
}

bool CCubeView::FixEdgesOnSide(int side1, int side2, int side3, int side4, int color1, int color2, int type, bool top)
{
    std::vector<Move> undoMoves;
    // check if any on the side in correct orientation to 
    // move up, if so move to between side3, and side2
    for (int edge = 1; edge < m_Size-1; ++edge)
    {
        if (IsEdge(side2, side1, edge, color1, color2))
        {
            AddMove(5, m_Size-1-edge);
            AddMove(5, m_Size-1-edge, undoMoves);
        }
        else if (IsEdge(side4, side3, edge, color1, color2))
        {
            AddMove(4, edge);
            AddMove(4, edge, undoMoves);
        }
        else if (IsEdge(side1, side4, edge, color1, color2))
        {
            AddMove(4, edge);
            AddMove(4, edge);
            AddMove(4, edge, undoMoves);
            AddMove(4, edge, undoMoves);
        }
    }
    bool done = true;
    for (int edge = 1; edge < m_Size-1; ++edge)
    {
        if (!IsEdge(side3, side2, edge, color1, color2))
        {
            done = false;
        }
    }

    if (done)
    {
        done = true;
    }
    if (undoMoves.size())
    {
        if (top)
        {
            AddMove(5,      m_Size-1);
            AddMove(type^1, m_Size-1);
            AddMove(4,      0);
            AddMove(type,   0);
            PlayMovesReverse(undoMoves);
            AddMove(type^1, m_Size-1);
            AddMove(5,      m_Size-1);
            AddMove(type,   0);
            AddMove(4,      0);
        }
        else
        {
            AddMove(type,   0);
            AddMove(5,      0);
            AddMove(type^1, m_Size-1);
            PlayMovesReverse(undoMoves);
            AddMove(type,   0);
            AddMove(4,      m_Size-1);
            AddMove(type^1, m_Size-1);
        }
    }
    return done;
}

void CCubeView::FlipEdge(int type1, int type2)
{
    AddMove(type2,   0);
    AddMove(5,       m_Size-1);
    AddMove(type2^1, m_Size-1);
    AddMove(4,       0);
    AddMove(type1^1, m_Size-1);
    AddMove(4,       0);
    AddMove(type1,   0);
    AddMove(5,       m_Size-1);
}

bool CCubeView::FixEdgesOnSide(int side1, int side2, int side3, int side4, int color1, int color2, int type1, int type2, bool top)
{
    if (FixEdgesOnSide(side1, side2, side3, side4, color1, color2, type2, top))
    {
        return true;
    }
    bool flip1 = false;
    bool flip3 = false;
    bool flip4 = false;

    std::vector<Move> undoMoves;
    // check if any on the side in flipped orientation
    for (int edge = 1; edge < m_Size-1; ++edge)
    {
        if (IsEdge(side1, side2, edge, color1, color2))
        {
            flip1 = true;
        }
        else if (IsEdge(side2, side3, edge, color1, color2))
        {
            AddMove(5, edge);
            AddMove(5, edge, undoMoves);
            flip3 = true;
        }
        else if (IsEdge(side3, side4, edge, color1, color2))
        {
            flip3 = true;
        }
        else if (IsEdge(side4, side1, edge, color1, color2))
        {
            flip4 = true;
        }
    }
    if (flip1)
    {
        FlipEdge(type1, type2);
    }
    if (flip3)
    {
        AddMove(5,       m_Size-1);
        AddMove(5,       m_Size-1);
        FlipEdge(type1^1, type2^1);
        AddMove(4,       0);
        AddMove(4,       0);
    }
    if (flip4)
    {
        AddMove(4,        0);
        FlipEdge(type2^1, type1);
        AddMove(5,        m_Size-1);
    }
    PlayMovesReverse(undoMoves);
    return FixEdgesOnSide(side1, side2, side3, side4, color1, color2, type2, top);
}

void CCubeView::FixCenter(int type1, int type2, int type3, int row, int column)
{
    AddMove(type3,   0);

    AddMove(type2,   column);
    AddMove(type1,   row);
    AddMove(type2^1, m_Size-1-column);
    AddMove(type1^1, m_Size-1-row);

    AddMove(type3^1, m_Size-1);

    AddMove(type1,   row);
    AddMove(type2,   column);
    AddMove(type1^1, m_Size-1-row);
    AddMove(type2^1, m_Size-1-column);
}

void CCubeView::FixCenter(int side1, int side2, int row1, int column1, int row2, int column2)
{
    CenterRelativePositions &centerPositions = m_CentersPositions[side1][side2];
    RotatePosition(row1, column1, centerPositions.angle1);
    RotatePosition(row2, column2, centerPositions.angle2);

    int type1 = centerPositions.type1;
    int type2 = centerPositions.type2;
    int type3 = centerPositions.type3;

    if (centerPositions.twoMoves)
    {
        AddMove(type2^1, m_Size-1-column2);
        AddMove(type1,   row2);
        AddMove(type2,   column2);
        AddMove(type1^1, m_Size-1-row2);
        AddMove(type3,   0);
        AddMove(type1,   row2);
        AddMove(type2^1, m_Size-1-column2);
        AddMove(type1^1, m_Size-1-row2);
        AddMove(type2,   column2);
        AddMove(type3^1, m_Size-1);
    }

    if ((row1 == row2) && (column1 == column2))
    {
        FixCenter(type1, type2, type3, row1, column1);
    }
    else if ((row1 == column2) && (column1 == m_Size-1-row2))
    {
        AddMove(type3, 0);
        FixCenter(type1, type2, type3, row1, column1);
    }
    else if ((row1 == m_Size-1-row2) && (column1 == m_Size-1-column2))
    {
        AddMove(type3, 0);
        AddMove(type3, 0);
        FixCenter(type1, type2, type3, row1, column1);
    }
    else
    {
        AddMove(type3^1, m_Size-1);
        FixCenter(type1, type2, type3, row1, column1);
    }
}

int CCubeView::FixSecondLastCenters()
{
    std::vector<Move> undoMoves;

    if (m_Size&1)
    {
        switch ( m_SideAngles[3] )
        {
        case 0:
            break;
        case 1:
            AddMove(0, 0);
            break;
        case 2:
            AddMove(0, 0);
            AddMove(0, 0);
            break;
        case 3:
            AddMove(1, m_Size-1);
            break;
        }
    }

    int numPiecesChanged = 0;
    int row1;
    int column1;
    int skip = (m_Size&1) ? (m_Size-1)/2 : m_Size;
    for (row1 = 1; row1 < m_Size-1; ++row1)
    {
        if (m_Abort)
        {
            break;
        }
        for (column1 = 1; column1 < m_Size-1; ++column1)
        {
            PlayMovesReverse(undoMoves);
            if (m_Abort)
            {
                break;
            }
            if ((row1 == skip) && (column1 == skip))
            {
                continue;
            }
            if (!IsColor(3, row1, column1, 3, row1, column1))
            {
                int row2 = row1;
                int column2 = column1;
                Piece p;
                p.m_Color = 3;
                p.m_Row = row1;
                p.m_Column = column1;
                if ( HasCenterType(3, row2, column2, p) )
                {
                    if ((row2 > row1) && (row2 != m_Middle))
                    {
                        AddMove(5, m_Size-1-row2);
                        AddMove(5, m_Size-1-row2, undoMoves);
                    }
                    else
                    {
                        FixCenter(3, 5, row2, column2, row2, column2);
                    }
                }
                if ( HasCenterType(5, row2, column2, p) )
                {
                    ++numPiecesChanged;
                    FixCenter(3, 5, row1, column1, row2, column2);
                    continue;
                }
            }
        }
    }
    PlayMovesReverse(undoMoves);
    return numPiecesChanged;
}

int CCubeView::FixLastCenters()
{
    std::vector<Move> undoMoves;

    int numPiecesChanged = 0;
    int row1;
    int column1;
    for (row1 = 1; row1 < m_Size-1; ++row1)
    {
        if (m_Abort)
        {
            break;
        }
        for (column1 = 1; column1 < m_Size-1; ++column1)
        {
            if (m_Abort)
            {
                break;
            }
            if (!IsColor(5, row1, column1, 5, row1, column1))
            {
                int row2 = row1;
                int column2 = column1;
                Piece p;
                p.m_Color = 5;
                p.m_Row = row1;
                p.m_Column = column1;
                if ( HasCenterType(5, row2, column2, p) )
                {
                    ++numPiecesChanged;
                    int angle1 = 0;
                    int angle2 = 0;
                    int row3 = row1;
                    int column3 = column1;
                    while ((row3 != row2) || (column3 != column2))
                    {
                        RotatePosition(row3, column3, 90);
                        angle1+=90;
                    }
                    row3 = row1;
                    column3 = column1;
                    while (((row3 == row1) && (column3 == column1)) || ((row3 == row2) && (column3 == column2)) || (row3 < row1) || ((row3 == row1) && (column3 < column1)))
                    {
                        RotatePosition(row3, column3, 90);
                        angle2+=90;
                        if (angle2 == 360)
                        {
                            break;
                        }
                    }
                    if (angle2 == 360)
                    {
                        continue;
                    }
                    AddMove(0, m_Size-1-column1);
                    AddMove(3, m_Size-1-row1);
                    AddMove(1, column1);
                    switch (angle2)
                    {
                    case 0:
                        AddMove(3, 0);
                        AddMove(3, 0, undoMoves);
                        break;
                    case 90:
                        break;
                    case 180:
                        AddMove(2, m_Size-1);
                        AddMove(2, m_Size-1, undoMoves);
                        break;
                    case 270:
                        AddMove(3, 0);
                        AddMove(3, 0, undoMoves);
                        AddMove(3, 0);
                        AddMove(3, 0, undoMoves);
                        break;
                    }
                    AddMove(2, row1);
                    AddMove(4, m_Size-1-column1);
                    AddMove(3, m_Size-1-row1);
                    AddMove(5, column1);
                    switch (angle1-angle2)
                    {
                    case -270:
                        AddMove(2, m_Size-1);
                        AddMove(2, m_Size-1, undoMoves);
                        break;
                    case -180:
                        AddMove(3, 0);
                        AddMove(3, 0, undoMoves);
                        AddMove(3, 0);
                        AddMove(3, 0, undoMoves);
                        break;
                    case -90:
                        AddMove(3, 0);
                        AddMove(3, 0, undoMoves);
                        break;
                    case 0:
                        break;
                    case 90:
                        AddMove(2, m_Size-1);
                        AddMove(2, m_Size-1, undoMoves);
                        break;
                    case 180:
                        AddMove(3, 0);
                        AddMove(3, 0, undoMoves);
                        AddMove(3, 0);
                        AddMove(3, 0, undoMoves);
                        break;
                    case 270:
                        AddMove(3, 0);
                        AddMove(3, 0, undoMoves);
                        break;
                    }
                    AddMove(4, m_Size-1-column1);
                    AddMove(2, row1);
                    AddMove(5, column1);
                    AddMove(3, m_Size-1-row1);

                    PlayMovesReverse(undoMoves);

                    AddMove(0, m_Size-1-column1);
                    AddMove(2, row1);
                    AddMove(1, column1);
                }
            }
        }
    }
    return numPiecesChanged;
}

void CCubeView::FixCorners(bool first)
{
    FixCorner(0, 0, 1, 5);
    if (m_Abort)
    {
        return;
    }
    FixCorner(1, 0, 5, 3);
    if (m_Abort)
    {
        return;
    }
    FixCorner(2, 0, 3, 2);
    if (m_Abort)
    {
        return;
    }
    FixCorner(3, 0, 2, 1);
    if (m_Abort)
    {
        return;
    }
    FixCorner(4, 4, 1, 2);
    if (m_Abort)
    {
        return;
    }
    FixCorner(5, 4, 2, 3);
    if (m_Abort)
    {
        return;
    }
    FixCorner(6, 4, 3, 5);
    if (!IsCorner(6, 4, 3, 5))
    {
        if (!first)
        {
            m_Abort = true;
            return;
        }
        if (m_Size > 2)
        {
            AddMove(5, m_Size-1);
            AddMove(5, 0);
            AddMove(3, 0);
            AddMove(4, m_Size-1);
            AddMove(3, 0);
            AddMove(3, 0);
            AddMove(5, 0);
            AddMove(3, 0);
            AddMove(4, m_Size-1);
            AddMove(3, 0);
            AddMove(4, 0);
        }
        else
        {
            AddMove(3, 0);
        }
        FixCorners(false);
    }
}

void CCubeView::FixCorner1(int type1, int type2)
{
    AddMove(type1,   0);
    AddMove(type2^1, m_Size-1);
    AddMove(type1,   m_Size-1);
    AddMove(type2,   0);
    AddMove(type1^1, m_Size-1);
    AddMove(type2^1, m_Size-1);
    AddMove(type1^1, 0);
    AddMove(type2,   0);
}

void CCubeView::FixCorner2(int type1, int type2)
{
    AddMove(type1,   m_Size-1);
    AddMove(type2,   0);
    AddMove(type1,   0);
    AddMove(type2^1, m_Size-1);
    AddMove(type1^1, 0);
    AddMove(type2,   0);
    AddMove(type1^1, m_Size-1);
    AddMove(type2^1, m_Size-1);
}

void CCubeView::RotateCorner(int type1, int type2, int type3)
{
    AddMove(type2^1, 0);
    AddMove(type1^1, m_Size-1);
    AddMove(type2,   m_Size-1);
    AddMove(type3^1, m_Size-1);
    AddMove(type1^1, m_Size-1);
    AddMove(type3,   0);
    AddMove(type1^1, 0);
    AddMove(type3^1, m_Size-1);
    AddMove(type1,   0);
    AddMove(type3,   0);
    AddMove(type2^1, 0);
    AddMove(type1,   0);
    AddMove(type2,   m_Size-1);
    AddMove(type1,   m_Size-1);
}

void CCubeView::FixCorner(int num1, int color1, int color2, int color3)
{
    if (IsCorner(num1, color1, color2, color3))
    {
        return;
    }
    if (IsCorner(num1, color2, color3, color1))
    {
        m_CornerTableUsed[num1][num1][1] = true;
        RotateCorner(m_CornerMoves[num1][num1][1].type1, m_CornerMoves[num1][num1][1].type2, m_CornerMoves[num1][num1][1].type3);
        if (!IsCorner(num1, color1, color2, color3))
        {
            fprintf_s(m_LogFile, "Failed position corner for %i rotate 0\n", num1);
            m_Abort = true;
        }
        return;
    }
    if (IsCorner(num1, color3, color1, color2))
    {
        m_CornerTableUsed[num1][num1][2] = true;
        RotateCorner(m_CornerMoves[num1][num1][2].type1, m_CornerMoves[num1][num1][2].type2, m_CornerMoves[num1][num1][2].type3);
        if (!IsCorner(num1, color1, color2, color3))
        {
            fprintf_s(m_LogFile, "Failed position corner for %i rotate 1\n", num1);
            m_Abort = true;
        }
        return;
    }
    if (num1 == 6)
    {
        return;
    }
    int num2;
    for (num2 = num1+1; num2 < 8; ++num2)
    {
        if (IsCorner(num2, color1, color2, color3))
        {
            m_CornerTableUsed[num1][num2][0] = true;
            FixCorner(m_CornerMoves[num1][num2][0]);
            if (!IsCorner(num1, color1, color2, color3))
            {
                fprintf_s(m_LogFile, "Failed position corner for %i, %i, 0\n", num1, num2);
                m_Abort = true;
            }
            break;
        }
        if (IsCorner(num2, color2, color3, color1))
        {
            m_CornerTableUsed[num1][num2][1] = true;
            FixCorner(m_CornerMoves[num1][num2][1]);
            if (!IsCorner(num1, color1, color2, color3))
            {
                fprintf_s(m_LogFile, "Failed position corner for %i, %i, 1\n", num1, num2);
                m_Abort = true;
            }
            break;
        }
        if (IsCorner(num2, color3, color1, color2))
        {
            m_CornerTableUsed[num1][num2][2] = true;
            FixCorner(m_CornerMoves[num1][num2][2]);
            if (!IsCorner(num1, color1, color2, color3))
            {
                fprintf_s(m_LogFile, "Failed position corner for %i, %i, 2\n", num1, num2);
                m_Abort = true;
            }
            break;
        }
    }
}

void CCubeView::FixCorner(CornerMoves &cornerMoves)
{
    if (cornerMoves.move1 != -1)
    {
        AddMove(cornerMoves.move1, cornerMoves.depth1*(m_Size-1));
    }
    if (cornerMoves.move2 != -1)
    {
        AddMove(cornerMoves.move2, cornerMoves.depth2*(m_Size-1));
    }
    if (cornerMoves.move3 != -1)
    {
        AddMove(cornerMoves.move3, cornerMoves.depth3*(m_Size-1));
    }
    if (cornerMoves.type3 == 1)
    {
        FixCorner1(cornerMoves.type2^1, cornerMoves.type1);
    }
    else
    {
        FixCorner2(cornerMoves.type2^1, cornerMoves.type1);
    }
    if (cornerMoves.move3 != -1)
    {
        AddMove(cornerMoves.move3^1, (1-cornerMoves.depth3)*(m_Size-1));
    }
    if (cornerMoves.move2 != -1)
    {
        AddMove(cornerMoves.move2^1, (1-cornerMoves.depth2)*(m_Size-1));
    }
    if (cornerMoves.move1 != -1)
    {
        AddMove(cornerMoves.move1^1, (1-cornerMoves.depth1)*(m_Size-1));
    }
}

void CCubeView::PlayMoves(std::vector<Move> &moves)
{
    for (unsigned i=0; i < moves.size(); ++i)
    {
        Move& move = moves[i];
        switch (move.Type)
        {
        case 0:
            RotateRight(move.Depth, false);
            break;
        case 1:
            RotateLeft(move.Depth, false);
            break;
        case 2:
            RotateFront(move.Depth, false);
            break;
        case 3:
            RotateBack(move.Depth, false);
            break;
        case 4:
            RotateTop(move.Depth, false);
            break;
        case 5:
            RotateBottom(move.Depth, false);
            break;
        }
    }
    moves.resize(0);
}

void CCubeView::PlayMovesReverse(std::vector<Move> &moves)
{
    if (m_MovesToPlay.size())
    {
        PlayMoves();
    }
    for (int i=(int)(moves.size())-1; i >= 0; --i)
    {
        const Move& move = moves[i];
        switch (move.Type^1)
        {
        case 0:
            RotateRight(m_Size-1-move.Depth, false);
            break;
        case 1:
            RotateLeft(m_Size-1-move.Depth, false);
            break;
        case 2:
            RotateFront(m_Size-1-move.Depth, false);
            break;
        case 3:
            RotateBack(m_Size-1-move.Depth, false);
            break;
        case 4:
            RotateTop(m_Size-1-move.Depth, false);
            break;
        case 5:
            RotateBottom(m_Size-1-move.Depth, false);
            break;
        }
    }
    moves.resize(0);
}

void CCubeView::AddMove(int type, int depth, std::vector<Move> &moves)
{
    if ((moves.size() > 0) && (moves[moves.size()-1].Type == (type^1)) && (moves[moves.size()-1].Depth == (m_Size-1-depth)))
    {
        // next move undoes prior, so simply remove prior move
        moves.pop_back();
    }
    else if ((moves.size() > 1) && (moves[moves.size()-1].Type == type) && (moves[moves.size()-1].Depth == depth)
        && (moves[moves.size()-2].Type == type) && (moves[moves.size()-2].Depth == depth))
    {
        // next move is the same as the last two, so replace the three with one opposite direction
        moves.pop_back();
        moves[moves.size()-1].Type = type^1;
        moves[moves.size()-1].Depth = m_Size-1-depth;
    }
    else
    {
        // cannot be merged into less moves, so simply add
        Move move;
        move.Type = type;
        move.Depth = depth;
        moves.push_back(move);
    }
#ifdef _DEBUG
    if (m_MovesToPlay.size())
    {
        PlayMoves();
    }
#endif
}

#ifdef _DEBUG
void CCubeView::AssertValid() const
{
	CView::AssertValid();
}

void CCubeView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CCubeDoc* CCubeView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCubeDoc)));
	return (CCubeDoc*)m_pDocument;
}
#endif //_DEBUG


// CCubeView message handlers
