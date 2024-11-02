// CubeView.cpp : implementation of the CCubeView class
//

#include "stdafx.h"
#include "Cube.h"

#include "CubeDoc.h"
#include "CubeView.h"
#include "MainFrm.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const double g_dAngleX = 45.0;
const double g_dAngleY = -acos( sqrt(2.0) / sqrt(3.0) ) * 45.0 / atan(1.0);
const double g_dAngleZ = -30.0;
const double g_dAngleY2 = atan( 1/5.5 ) * 45.0 / atan(1.0); 

unsigned char CCubeView::m_threeto8[8] = { 0, 0111>>1, 0222>>1, 0333>>1, 0444>>1, 0555>>1, 0666>>1, 0377 };
unsigned char CCubeView::m_twoto8[4] = { 0, 0x55, 0xaa, 0xff };
unsigned char CCubeView::m_oneto8[2] = { 0, 255 };

int CCubeView::m_defaultOverride[13] = { 0, 3, 24, 27, 64, 67, 88, 173, 181, 236, 247, 164, 91 };

PALETTEENTRY CCubeView::m_defaultPalEntry[20] = {
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
    ON_COMMAND(ID_EDIT_LESS, CCubeView::OnEditLess)
    ON_COMMAND(ID_EDIT_MORE, CCubeView::OnEditMore)
    ON_COMMAND(ID_EDIT_SCRAMBLE, CCubeView::OnEditScramble)
    ON_COMMAND(ID_EDIT_SOLVE, CCubeView::OnEditSolve)
	ON_COMMAND(ID_VIEW_ROTATEX, CCubeView::OnViewRotateX)
    ON_COMMAND(ID_VIEW_ROTATECX, CCubeView::OnViewRotateCX)
	ON_COMMAND(ID_VIEW_ROTATEY, CCubeView::OnViewRotateY)
    ON_COMMAND(ID_VIEW_ROTATECY, CCubeView::OnViewRotateCY)
    ON_COMMAND(ID_VIEW_ROTATEZ, CCubeView::OnViewRotateZ)
	ON_COMMAND(ID_VIEW_ROTATECZ, CCubeView::OnViewRotateCZ)
    ON_COMMAND(ID_SPEED_SLOW, CCubeView::OnSpeedSlow)
    ON_COMMAND(ID_SPEED_MEDIUM, CCubeView::OnSpeedMedium)
    ON_COMMAND(ID_SPEED_FAST, CCubeView::OnSpeedFast)
END_MESSAGE_MAP()

// CCubeView construction/destruction

CCubeView::CCubeView()
{
    m_lSleep = 20;
    m_dStep = 2;

    m_AngleX = 0;
    m_AngleY = 0;
    m_AngleZ = 0;

    m_hrc = NULL;

    m_pPal = NULL;

    m_Size = 3;
    InitCube();
}

CCubeView::~CCubeView()
{
    if ( m_pPal )
    {
        delete m_pPal;
    }
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

    static GLdouble colors[7][3] = {
        {3.0, 3.0, 3.0},
        {3.0, 1.5, 0.0},
        {0.0, 3.0, 0.0},
        {3.0, 0.0, 0.0},
        {3.0, 3.0, 0.0},
        {0.0, 0.0, 3.0},
        {0.5, 0.5, 0.5}
    }; 

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

                glTranslated(-1.0, 0.0, -5.5);

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

                glColor3dv(colors[ m_Sides[side][y*m_Size+x] ]);

                glBegin(GL_POLYGON);
                glNormal3d(  0.0,  0.0,  stickerSize*dScale);
                glVertex3d( stickerSize*dScale,  stickerSize*dScale, 0); 
                glVertex3d(-stickerSize*dScale,  stickerSize*dScale, 0);            
                glVertex3d(-stickerSize*dScale, -stickerSize*dScale, 0);           
                glVertex3d( stickerSize*dScale, -stickerSize*dScale, 0);            
                glEnd();

                glLoadIdentity();

                glTranslated(1.0, 0.0, -5.5);

                glRotated(-g_dAngleY2, 0.0, 1.0, 0.0);

                glRotated(g_dAngleZ, 0.0, 0.0, 1.0);
                glRotated(g_dAngleY, 0.0, 1.0, 0.0);
                glRotated(g_dAngleX, 1.0, 0.0, 0.0);

                glRotated(m_AngleZ, 0.0, 0.0, 1.0);
                glRotated(m_AngleY, 0.0, 1.0, 0.0);
                glRotated(m_AngleX, 1.0, 0.0, 0.0);

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

                glColor3dv(colors[ m_Sides[side][y*m_Size+x] ]);

                glBegin(GL_POLYGON);
                glNormal3d(  0.0,  0.0,  stickerSize);
                glVertex3d( stickerSize,  stickerSize, 0); 
                glVertex3d(-stickerSize,  stickerSize, 0);            
                glVertex3d(-stickerSize, -stickerSize, 0);           
                glVertex3d( stickerSize, -stickerSize, 0);            
                glEnd();
            }
        }
    }

    glFlush();	

    SwapBuffers(pDC->m_hDC);

    wglMakeCurrent(NULL, NULL);
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

    BOOL bResult = SetPixelFormat(dc.m_hDC, nPixelFormat, &pfd);
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

    CreateRGBPalette(dc.m_hDC) ;

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

        double dLeft = (m_WindowSizeX / 2.0) - dSquareSize * 3 * m_Size; 
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

        if ( abs(diffX) > abs(diffY))
        {
            switch ( side )
            {
            case 0:
                if ( diffX > 0 )
                {
                    RotateFront(m_Size-1-y);
                }
                else
                {
                    RotateBack(y);
                }
                break;
            case 1:
            case 2:
            case 3:
                if ( diffX > 0 )
                {
                    RotateBottom(m_Size-1-y);
                }
                else
                {
                    RotateTop(y);
                }
                break;
            case 4:
                if ( diffX > 0 )
                {
                    RotateBack(m_Size-1-y);
                }
                else
                {
                    RotateFront(y);
                }
                break;
            case 5:
                if ( diffX > 0 )
                {
                    RotateTop(m_Size-1-y);
                }
                else
                {
                    RotateBottom(y);
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
                    RotateBack(x);
                }
                else
                {
                    RotateFront(m_Size-1-x);
                }
                break;
            case 0:
            case 2:
            case 4:
            case 5:
                if ( diffY > 0 )
                {
                    RotateLeft(x);
                }
                else
                {
                    RotateRight(m_Size-1-x);
                }
                break;
            case 3:
                if ( diffY > 0 )
                {
                    RotateFront(x);
                }
                else
                {
                    RotateBack(m_Size-1-x);
                }
                break;
            }
        }
    }
}

void CCubeView::OnEditLess()
{
    if ( m_Size > 2 )
    {
        --m_Size;
        InitCube();

        Invalidate(FALSE);
        UpdateWindow();
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
    }
}

void CCubeView::OnEditScramble()
{
    int savedSleep = m_lSleep;
    double savedStep = m_dStep;
    srand(GetTickCount());
    m_dStep = 2;
    m_lSleep = 0;
    m_Mixing = true;

    int numMoves;
    int prevType = -1;
    int moveType = rand() * 3 / RAND_MAX;
    for ( numMoves = ((rand() * m_Size)/RAND_MAX + 3) * 5 ; numMoves; --numMoves )
    {
        char buffer[50];
        sprintf_s( buffer, 50, "Moves Remaining = %i", numMoves );
        ((CMainFrame*)AfxGetMainWnd())->SetStatusText(buffer);

        int depth = rand() * m_Size / RAND_MAX;
        int amount = rand() * 3 / RAND_MAX;
        switch ( moveType )
        {
        case 0:
            switch ( amount )
            {
            case 0:
                RotateLeft( m_Size-1-depth );
                break;
            case 1:
                RotateRight( depth );
            case 2:
                RotateRight( depth );
            }
            break;
        case 1:
            switch ( amount )
            {
            case 0:
                RotateTop( m_Size-1-depth );
                break;
            case 1:
                RotateBottom( depth );
            case 2:
                RotateBottom( depth );
            }
            break;
        case 2:
            switch ( amount )
            {
            case 0:
                RotateFront( m_Size-1-depth );
                break;
            case 1:
                RotateBack( depth );
            case 2:
                RotateBack( depth );
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
    ((CMainFrame*)AfxGetMainWnd())->SetStatusText("Ready");

    m_dStep = savedStep;
    m_lSleep = savedSleep;
    m_Moves = 0;
    m_Mixing = false;
}

void CCubeView::IncrementMoves()
{
    ++m_Moves;
    if (!m_Mixing)
    {
        char buffer[50];
        sprintf_s( buffer, 50, "Moves Made = %i", m_Moves );
        ((CMainFrame*)AfxGetMainWnd())->SetStatusText(buffer);
    }
}

void CCubeView::OnEditSolve()
{
    int savedSleep = m_lSleep;
    double savedStep = m_dStep;
    srand(GetTickCount());
    m_dStep = 2;
    m_lSleep = 0;

    Move moves[16];
    int count = 0;
    
    // rotate full cube to get absolute center right
    if ((m_Size & 1) == 1)
    {
        if (m_Sides[0][(m_Size*m_Size-1)/2] != 0)
        {
            if (m_Sides[1][(m_Size*m_Size-1)/2] == 0)
            {
                OnViewRotateCZ();
            }
            else if (m_Sides[2][(m_Size*m_Size-1)/2] == 0)
            {
                OnViewRotateCX();
            }
            else if (m_Sides[3][(m_Size*m_Size-1)/2] == 0)
            {
                OnViewRotateZ();
            }
            else if (m_Sides[4][(m_Size*m_Size-1)/2] == 0)
            {
                OnViewRotateZ();
                OnViewRotateZ();
            }
            else if (m_Sides[5][(m_Size*m_Size-1)/2] == 0)
            {
                OnViewRotateX();
            }
        }
        while (m_Sides[1][(m_Size*m_Size-1)/2] != 1)
        {
            OnViewRotateY();
        }
    }

    // solve edges
    int edge = 1;
    // do top
    for (edge = 1; edge < (m_Size-1); ++edge)
    {
        FixTopEdge(5, 1, 2, 3, 3, 1, edge, count, moves);
        FixTopEdge(1, 2, 3, 5, 1, 2, edge, count, moves);
        FixTopEdge(2, 3, 5, 1, 2, 0, edge, count, moves);
        FixTopEdge(3, 5, 1, 2, 0, 3, edge, count, moves);
    }

    // solve corners

    // solve centers

    m_dStep = savedStep;
    m_lSleep = savedSleep;
}

void CCubeView::OnViewRotateX()
{
    while ( m_AngleX < 90 )
    {
        long lLastTick = GetTickCount();
        m_AngleX += m_dStep;
        Invalidate(FALSE);
        UpdateWindow();
        long ticks = (lLastTick + m_lSleep)- GetTickCount();
        if ( ticks < 0 )
        {
            ticks = 0;
        }
        Sleep(ticks);
    }
    m_AngleX = 0.0;

    std::vector<char> Side;
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
    while ( m_AngleX > -90 )
    {
        long lLastTick = GetTickCount();
        m_AngleX -= m_dStep;
        Invalidate(FALSE);
        UpdateWindow();
        long ticks = (lLastTick + m_lSleep)- GetTickCount();
        if ( ticks < 0 )
        {
            ticks = 0;
        }
        Sleep(ticks);
    }
    m_AngleX = 0.0;

    std::vector<char> Side;
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
    while ( m_AngleY < 90 )
    {
        long lLastTick = GetTickCount();
        m_AngleY += m_dStep;
        Invalidate(FALSE);
        UpdateWindow();
        long ticks = (lLastTick + m_lSleep)- GetTickCount();
        if ( ticks < 0 )
        {
            ticks = 0;
        }
        Sleep(ticks);
    }
    m_AngleY = 0.0;

    std::vector<char> Side;
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
    while ( m_AngleY > -90 )
    {
        long lLastTick = GetTickCount();
        m_AngleY -= m_dStep;
        Invalidate(FALSE);
        UpdateWindow();
        long ticks = (lLastTick + m_lSleep)- GetTickCount();
        if ( ticks < 0 )
        {
            ticks = 0;
        }
        Sleep(ticks);
    }
    m_AngleY = 0.0;

    std::vector<char> Side;
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
    while ( m_AngleZ < 90 )
    {
        long lLastTick = GetTickCount();
        m_AngleZ += m_dStep;
        Invalidate(FALSE);
        UpdateWindow();
        long ticks = (lLastTick + m_lSleep)- GetTickCount();
        if ( ticks < 0 )
        {
            ticks = 0;
        }
        Sleep(ticks);
    }
    m_AngleZ = 0.0;

    std::vector<char> Side;
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
    while ( m_AngleZ > -90 )
    {
        long lLastTick = GetTickCount();
        m_AngleZ -= m_dStep;
        Invalidate(FALSE);
        UpdateWindow();
        long ticks = (lLastTick + m_lSleep)- GetTickCount();
        if ( ticks < 0 )
        {
            ticks = 0;
        }
        Sleep(ticks);
    }
    m_AngleZ = 0.0;

    std::vector<char> Side;
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

void CCubeView::OnSpeedSlow()
{
    m_lSleep = 20;
}

void CCubeView::OnSpeedMedium()
{
    m_lSleep = 10;
}

void CCubeView::OnSpeedFast()
{
    m_lSleep = 0;
}

BOOL CCubeView::CreateRGBPalette(HDC hDC)
{
    PIXELFORMATDESCRIPTOR pfd;
    int n = GetPixelFormat(hDC);
    DescribePixelFormat(hDC, n, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
    if (!(pfd.dwFlags & PFD_NEED_PALETTE))
    {
        return FALSE;
    }

    LOGPALETTE* pPal = (LOGPALETTE*) malloc(sizeof(LOGPALETTE) + 256 * sizeof(PALETTEENTRY));
    if (!pPal) 
    {
        TRACE("Out of memory for logpal");
        return FALSE;
    }
    pPal->palVersion = 0x300;
    pPal->palNumEntries = 256;

    ASSERT( pfd.cColorBits == 8);
    n = 1 << pfd.cColorBits;
    for (int i=0; i<n; i++)
    {
        pPal->palPalEntry[i].peRed = ComponentFromIndex(i, pfd.cRedBits, pfd.cRedShift);
        pPal->palPalEntry[i].peGreen = ComponentFromIndex(i, pfd.cGreenBits, pfd.cGreenShift);
        pPal->palPalEntry[i].peBlue = ComponentFromIndex(i, pfd.cBlueBits, pfd.cBlueShift);
        pPal->palPalEntry[i].peFlags = 0;
    }

    if ((pfd.cColorBits == 8)                           &&
        (pfd.cRedBits   == 3) && (pfd.cRedShift   == 0) &&
        (pfd.cGreenBits == 3) && (pfd.cGreenShift == 3) &&
        (pfd.cBlueBits  == 2) && (pfd.cBlueShift  == 6)
        )
    {
        for (int j = 1 ; j <= 12 ; j++)
        {
            pPal->palPalEntry[m_defaultOverride[j]] = m_defaultPalEntry[j];
        }
    }

    if (m_pPal)
    {
        delete m_pPal;
    }
    m_pPal = new CPalette;

    BOOL bResult = m_pPal->CreatePalette(pPal);
    free (pPal);

    return bResult;
}

unsigned char CCubeView::ComponentFromIndex(int i, UINT nbits, UINT shift)
{
    unsigned char val;

    val = (unsigned char) (i >> shift);
    switch (nbits)
    {
    case 1:
        val &= 0x1;
        return m_oneto8[val];

    case 2:
        val &= 0x3;
        return m_twoto8[val];

    case 3:
        val &= 0x7;
        return m_threeto8[val];

    default:
        return 0;
    }
}

void CCubeView::InitCube()
{
    int side;
    for ( side = 0; side < 6; ++side )
    {
        m_Sides[side].resize(m_Size * m_Size);
        int x;
        for ( x = 0; x < m_Size*m_Size; ++x )
        {
            m_Sides[side][x] = side;
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
            char temp = m_Sides[side][index1];
            m_Sides[side][index1] = m_Sides[side][index2];
            m_Sides[side][index2] = m_Sides[side][index3];
            m_Sides[side][index3] = m_Sides[side][index4];
            m_Sides[side][index4] = temp;
        }
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
            char temp = m_Sides[side][index1];
            m_Sides[side][index1] = m_Sides[side][index2];
            m_Sides[side][index2] = m_Sides[side][index3];
            m_Sides[side][index3] = m_Sides[side][index4];
            m_Sides[side][index4] = temp;
        }
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
            char temp = m_Sides[side][index1];
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
            char temp = m_Sides[side][index1];
            m_Sides[side][index1] = m_Sides[side][index2];
            m_Sides[side][index2] = temp;
        }
    }
}

void CCubeView::RotateRight(int depth)
{
    if ( depth < m_Size )
    {
        while ( m_AnglesX[depth] > -90 )
        {
            long lLastTick = GetTickCount();
            m_AnglesX[depth] -= m_dStep;
            Invalidate(FALSE);
            UpdateWindow();
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
            char temp = m_Sides[0][index];
            m_Sides[0][index] = m_Sides[2][index];
            m_Sides[2][index] = m_Sides[4][index];
            m_Sides[4][index] = m_Sides[5][index];
            m_Sides[5][index] = temp;
        }

        Invalidate(FALSE);
        UpdateWindow();
    }
    IncrementMoves();
}

void CCubeView::RotateLeft(int depth)
{
    if ( depth < m_Size )
    {
        while ( m_AnglesX[m_Size-1-depth] < 90 )
        {
            long lLastTick = GetTickCount();
            m_AnglesX[m_Size-1-depth] += m_dStep;
            Invalidate(FALSE);
            UpdateWindow();
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
            char temp = m_Sides[0][index];
            m_Sides[0][index] = m_Sides[5][index];
            m_Sides[5][index] = m_Sides[4][index];
            m_Sides[4][index] = m_Sides[2][index];
            m_Sides[2][index] = temp;
        }

        Invalidate(FALSE);
        UpdateWindow();
    }
    IncrementMoves();
}

void CCubeView::RotateFront(int depth)
{
    if ( depth < m_Size )
    {
        while ( m_AnglesZ[depth] > -90 )
        {
            long lLastTick = GetTickCount();
            m_AnglesZ[depth] -= m_dStep;
            Invalidate(FALSE);
            UpdateWindow();
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
            char temp = m_Sides[0][index1];
            m_Sides[0][index1] = m_Sides[1][index2];
            m_Sides[1][index2] = m_Sides[4][index3];
            m_Sides[4][index3] = m_Sides[3][index4];
            m_Sides[3][index4] = temp;
        }

        Invalidate(FALSE);
        UpdateWindow();
    }
    IncrementMoves();
}

void CCubeView::RotateBack(int depth)
{
    if ( depth < m_Size )
    {
        while ( m_AnglesZ[m_Size-1-depth] < 90 )
        {
            long lLastTick = GetTickCount();
            m_AnglesZ[m_Size-1-depth] += m_dStep;
            Invalidate(FALSE);
            UpdateWindow();
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
            char temp = m_Sides[0][index1];
            m_Sides[0][index1] = m_Sides[3][index2];
            m_Sides[3][index2] = m_Sides[4][index3];
            m_Sides[4][index3] = m_Sides[1][index4];
            m_Sides[1][index4] = temp;
        }

        Invalidate(FALSE);
        UpdateWindow();
    }
    IncrementMoves();
}

void CCubeView::RotateTop(int depth)
{
    if ( depth < m_Size )
    {
        while ( m_AnglesY[depth] > -90 )
        {
            long lLastTick = GetTickCount();
            m_AnglesY[depth] -= m_dStep;
            Invalidate(FALSE);
            UpdateWindow();
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
            char temp = m_Sides[1][index1];
            m_Sides[1][index1] = m_Sides[2][index1];
            m_Sides[2][index1] = m_Sides[3][index1];
            m_Sides[3][index1] = m_Sides[5][index2];
            m_Sides[5][index2] = temp;
        }

        Invalidate(FALSE);
        UpdateWindow();
    }
    IncrementMoves();
}

void CCubeView::RotateBottom(int depth)
{
    if ( depth < m_Size )
    {
        while ( m_AnglesY[m_Size-1-depth] < 90 )
        {
            long lLastTick = GetTickCount();
            m_AnglesY[m_Size-1-depth] += m_dStep;
            Invalidate(FALSE);
            UpdateWindow();
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
            char temp = m_Sides[1][index1];
            m_Sides[1][index1] = m_Sides[5][index2];
            m_Sides[5][index2] = m_Sides[3][index1];
            m_Sides[3][index1] = m_Sides[2][index1];
            m_Sides[2][index1] = temp;
        }

        Invalidate(FALSE);
        UpdateWindow();
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

// side1 is equivalent to back
// side2 is equivalent to left
// side3 is equivalent to front
// side4 is equivalent to right
// type1 rotates side1
// type2 rotates side2
void CCubeView::FixTopEdge(int side1, int side2, int side3, int side4, int type1, int type2, int edge, int &count, Move* moves)
{
    if (!IsEdge(0, side1, edge, 0, side1))
    {
        // check 7 spots on top
        if (IsEdge(side1, 0, edge, 0, side1))
        {
            SwapTop1(type2^1, type1, edge, count, moves);
        }
        else if (IsEdge(0, side2, edge, 0, side1))
        {
            SwapTop2(type1, type2^1, edge, count, moves);
        }
        else if (IsEdge(0, side3, edge, 0, side1))
        {
            SwapTop3(type2, edge, count, moves);
        }
        else if (IsEdge(0, side4, edge, 0, side1))
        {
            SwapTop4(type1^1, type2^1, edge, count, moves);
        }
        else if (IsEdge(side2, 0, edge, 0, side1))
        {
            SwapTop5(type2, type1, edge, count, moves);
        }
        else if (IsEdge(side3, 0, edge, 0, side1))
        {
            SwapTop6(type1^1, edge, count, moves);
        }
        else if (IsEdge(side4, 0, edge, 0, side1))
        {
            SwapTop7(type2^1, type1, edge, count, moves);
        }
        // check 8 spots on edge
        else if (IsEdge(side1, side4, edge, 0, side1))
        {
            PromoteEdge1(3, type1, type2^1, edge, count, moves);
        }
        else if (IsEdge(side2, side1, edge, 0, side1))
        {
            PromoteEdge1(0, type1, type2^1, edge, count, moves);
        }
        else if (IsEdge(side3, side2, edge, 0, side1))
        {
            PromoteEdge1(1, type1, type2^1, edge, count, moves);
        }
        else if (IsEdge(side4, side3, edge, 0, side1))
        {
            PromoteEdge1(2, type1, type2^1, edge, count, moves);
        }
        else if (IsEdge(side4, side1, edge, 0, side1))
        {
            PromoteEdge2(0, type1, type2^1, edge, count, moves);
        }
        else if (IsEdge(side1, side2, edge, 0, side1))
        {
            PromoteEdge2(3, type1, type2^1, edge, count, moves);
        }
        else if (IsEdge(side2, side3, edge, 0, side1))
        {
            PromoteEdge2(2, type1, type2^1, edge, count, moves);
        }
        else if (IsEdge(side3, side4, edge, 0, side1))
        {
            PromoteEdge2(1, type1, type2^1, edge, count, moves);
        }
        // check 8 spots on bottom
        else if (IsEdge(side2, 4, edge, 0, side1))
        {
            PromoteBottom1(0, type2, type1, edge, count, moves);
        }
        else if (IsEdge(side3, 4, edge, 0, side1))
        {
            PromoteBottom1(3, type2, type1, edge, count, moves);
        }
        else if (IsEdge(side4, 4, edge, 0, side1))
        {
            PromoteBottom1(2, type2, type1, edge, count, moves);
        }
        else if (IsEdge(side1, 4, edge, 0, side1))
        {
            PromoteBottom1(1, type2, type1, edge, count, moves);
        }
        else if (IsEdge(4, side2, edge, 0, side1))
        {
            PromoteBottom2(3, type1, edge, count, moves);
        }
        else if (IsEdge(4, side3, edge, 0, side1))
        {
            PromoteBottom2(2, type1, edge, count, moves);
        }
        else if (IsEdge(4, side4, edge, 0, side1))
        {
            PromoteBottom2(1, type1, edge, count, moves);
        }
        else if (IsEdge(4, side1, edge, 0, side1))
        {
            PromoteBottom2(0, type1, edge, count, moves);
        }
    }
    PlayMoves(count, moves);
}

void CCubeView::PlayMoves(int &count, Move* moves)
{
    while (count)
    {
        switch (moves->Type)
        {
        case 0:
            RotateRight(moves->Depth);
            break;
        case 1:
            RotateLeft(moves->Depth);
            break;
        case 2:
            RotateFront(moves->Depth);
            break;
        case 3:
            RotateBack(moves->Depth);
            break;
        case 4:
            RotateTop(moves->Depth);
            break;
        case 5:
            RotateBottom(moves->Depth);
            break;
        }
        ++moves;
        --count;
    }
}

void CCubeView::AddMove(int type, int depth, int &count, Move* moves)
{

    if ((count > 0) && (moves[count-1].Type == (type^1)) && (moves[count-1].Depth == (m_Size-1-depth)))
    {
        // next move undoes prior, so simply remove prior move
        --count;
    }
    else if ((count > 1) && (moves[count-1].Type == type) && (moves[count-1].Depth == depth)
        && (moves[count-2].Type == type) && (moves[count-2].Depth == depth))
    {
        // next move is the same as the last two, so replace the three with one opposite direction
        --count;
        moves[count-1].Type = type^1;
        moves[count-1].Depth = m_Size-1-depth;
    }
    else
    {
        // cannot be merged into less moves, so simply add
        moves[count].Type = type;
        moves[count].Depth = depth;
        ++count;
    }
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
