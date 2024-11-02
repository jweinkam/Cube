// CubeView.h : interface of the CCubeView class
//


#pragma once

#include <vector>

class CCubeView : public CView
{
protected: // create from serialization only
	CCubeView();
	DECLARE_DYNCREATE(CCubeView)

    DECLARE_MESSAGE_MAP()

public:
    virtual ~CCubeView();

	CCubeDoc* GetDocument() const;

	virtual void OnDraw(CDC* pDC);
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

    virtual void OnInitialUpdate();

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnDestroy();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    virtual void OnEditLess();
    virtual void OnEditMore();
    virtual void OnEditScramble();
    virtual void OnEditSolve();
	virtual void OnViewRotateX();
    virtual void OnViewRotateCX();
	virtual void OnViewRotateY();
    virtual void OnViewRotateCY();
	virtual void OnViewRotateZ();
    virtual void OnViewRotateCZ();
    virtual void OnSpeedSlow();
    virtual void OnSpeedMedium();
    virtual void OnSpeedFast();

private:
    struct Move
    {
        int Type;
        int Depth;
    };

    HGLRC m_hrc;
    CPalette* m_pPal;
    int m_WindowSizeX;
    int m_WindowSizeY;
    CPoint m_Down;

    double m_AngleX;
    double m_AngleY;
    double m_AngleZ;
    std::vector<double> m_AnglesX;
    std::vector<double> m_AnglesY;
    std::vector<double> m_AnglesZ;

    std::vector<char> m_Sides[6];
    int m_Size;
    int m_Moves;
    bool m_Mixing;

    int m_lSleep;
    double m_dStep;

    static unsigned char m_oneto8[2];
    static unsigned char m_twoto8[4];
    static unsigned char m_threeto8[8];
    static int           m_defaultOverride[13];
    static PALETTEENTRY  m_defaultPalEntry[20];

    BOOL CreateRGBPalette(HDC hDC);
    unsigned char ComponentFromIndex(int i, UINT nbits, UINT shift);

    void InitCube();

    void RotateSideClockwise(int side);
    void RotateSideCounterClockwise(int side);
    void RotateSide180(int side);

    void RotateRight(int depth); // Type 0
    void RotateLeft(int depth);  // Type 1

    void RotateFront(int depth); // Type 2
    void RotateBack(int depth);  // Type 3

    void RotateTop(int depth);   // Type 4
    void RotateBottom(int depth);// Type 5

    void IncrementMoves();

    bool IsEdge(int side1, int side2, int edge, int color1, int color2);
    inline bool IsColor(int side, int row, int column, int color)
    {
        return (m_Sides[side][row*m_Size+column] == color);
    }

    void FixTopEdge(int side1, int side2, int side3, int side4, int type1, int type2, int edge, int &count, Move* moves);

    void PlayMoves(int &count, Move* moves);
    void AddMove(int type, int depth, int &count, Move* moves);
    inline void SwapTop1(int type1, int type2, int edge, int &count, Move* moves)
    {
        AddMove(4,       0,             count, moves);
        AddMove(type1,   0,             count, moves);
        AddMove(5,       m_Size-1,      count, moves);
        AddMove(type2,   0,             count, moves);

        AddMove(type1^1, m_Size-1-edge, count, moves);

        AddMove(type2^1, m_Size-1,      count, moves);
        AddMove(4,       0,             count, moves);
        AddMove(type1^1, m_Size-1,      count, moves);
        AddMove(5,       m_Size-1,      count, moves);

        AddMove(type1,   edge,          count, moves);
    }
    inline void SwapTop2(int type1, int type2, int edge, int &count, Move* moves)
    {
        AddMove(type1,   edge,          count, moves);
        AddMove(type2,   edge,          count, moves);
        AddMove(4,       m_Size-1,      count, moves);
        AddMove(type2^1, m_Size-1-edge, count, moves);
        AddMove(type1^1, m_Size-1-edge, count, moves);
    }
    inline void SwapTop3(int type, int edge, int &count, Move* moves)
    {
        AddMove(type,    edge,          count, moves);
        AddMove(5,       0,             count, moves);
        AddMove(type^1,  m_Size-1-edge, count, moves);
        AddMove(type^1,  edge,          count, moves);
        AddMove(5,       0,             count, moves);
        AddMove(type,    m_Size-1-edge, count, moves);
    }
    inline void SwapTop4(int type1, int type2, int edge, int &count, Move* moves)
    {
        AddMove(type1,   edge,          count, moves);
        AddMove(type2,   edge,          count, moves);
        AddMove(5,       0,             count, moves);
        AddMove(type2^1, m_Size-1-edge, count, moves);
        AddMove(type1^1, m_Size-1-edge, count, moves);
    }
    inline void SwapTop5(int type1, int type2, int edge, int &count, Move* moves)
    {
        AddMove(type1,   0,             count, moves);
        AddMove(5,       edge,          count, moves);
        AddMove(type1^1, m_Size-1,      count, moves);
        AddMove(type2^1, m_Size-1,      count, moves);
        AddMove(5,       edge,          count, moves);
        AddMove(type2,   0,             count, moves);
    }
    inline void SwapTop6(int type, int edge, int &count, Move* moves)
    {
        AddMove(type,    0,             count, moves);
        AddMove(type,    m_Size-1,      count, moves);
        AddMove(5,       edge,          count, moves);
        AddMove(type^1,  0,             count, moves);
        AddMove(type^1,  m_Size-1,      count, moves);
    }
    inline void SwapTop7(int type1, int type2, int edge, int &count, Move* moves)
    {
        AddMove(type1^1, m_Size-1,      count, moves);
        AddMove(4,       edge,          count, moves);
        AddMove(type1,   0,             count, moves);
        AddMove(type2,   0,             count, moves);
        AddMove(4,       edge,          count, moves);
        AddMove(type2^1, m_Size-1,      count, moves);
    }
    inline void PromoteEdge1(int num, int type1, int type2, int edge, int &count, Move* moves)
    {
        while (num)
        {
            AddMove(4,   edge,          count, moves);
            --num;
        }
        AddMove(type1^1, m_Size-1,      count, moves);
        AddMove(type2^1, m_Size-1-edge, count, moves);
        AddMove(type1,   0,             count, moves);
        AddMove(type2,   edge,          count, moves);
    }
    inline void PromoteEdge2(int num, int type1, int type2, int edge, int &count, Move* moves)
    {
        while (num)
        {
            AddMove(5,   edge,          count, moves);
            --num;
        }
        AddMove(type1,   0,             count, moves);
        AddMove(type2^1, m_Size-1-edge, count, moves);
        AddMove(type1^1, m_Size-1,      count, moves);
        AddMove(type2,   edge,          count, moves);
    }
    inline void PromoteBottom1(int num, int type1, int type2, int edge, int &count, Move* moves)
    {
        while (num)
        {
            AddMove(5,   0,             count, moves);
            --num;
        }
        AddMove(type1,   0,             count, moves);
        AddMove(type2^1, m_Size-1,      count, moves);
        AddMove(type1,   m_Size-1-edge, count, moves);
        AddMove(type2,   0,             count, moves);
        AddMove(type1^1, edge,          count, moves);
        AddMove(type1^1, m_Size-1,      count, moves);
    }
    inline void PromoteBottom2(int num, int type, int edge, int &count, Move* moves)
    {
        while (num)
        {
            AddMove(5,   0,             count, moves);
            --num;
        }
        AddMove(type,    0,             count, moves);
        AddMove(type,    0,             count, moves);
        AddMove(5,       m_Size-1,      count, moves);
        AddMove(type^1,  m_Size-1-edge, count, moves);
        AddMove(4,       0,             count, moves);
        AddMove(type,    0,             count, moves);
        AddMove(type,    0,             count, moves);
        AddMove(5,       m_Size-1,      count, moves);
        AddMove(type,    edge,          count, moves);
        AddMove(4,       0,             count, moves);
    }

#ifdef _DEBUG
public:
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif
};

#ifndef _DEBUG  // debug version in CubeView.cpp
inline CCubeDoc* CCubeView::GetDocument() const
   { return reinterpret_cast<CCubeDoc*>(m_pDocument); }
#endif

