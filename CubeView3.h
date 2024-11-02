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
    afx_msg BOOL OnEraseBkgnd(CDC *pDC);
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
    virtual void OnEditRedo();
	virtual void OnViewRotateX();
    virtual void OnViewRotateCX();
	virtual void OnViewRotateY();
    virtual void OnViewRotateCY();
	virtual void OnViewRotateZ();
    virtual void OnViewRotateCZ();
    virtual void OnSpeedSlow();
    virtual void OnSpeedMedium();
    virtual void OnSpeedFast();
    virtual void OnSpeedVeryFast();

private:
    struct Move
    {
        int Type;
        int Depth;
    };
    struct CenterRelativePositions
    {
        int angle1;
        int angle2;
        int type1;
        int type2;
        int type3;
        bool twoMoves;
    };
    struct CornerPositions
    {
        int side1;
        int row1;
        int column1;
        int side2;
        int row2;
        int column2;
        int side3;
        int row3;
        int column3;
    };
    struct CornerMoves
    {
        int type1;
        int type2;
        int type3;
        int move1;
        int depth1;
        int move2;
        int depth2;
        int move3;
        int depth3;
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
    std::vector<char> m_SidesCopy[6];
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
    static CenterRelativePositions m_CentersPositions[6][6];
    static CornerPositions m_CornerPositions[8];
    static CornerMoves m_CornerMoves[8][8][3];

    void Draw3D(int side, int x, int y, GLdouble cubieSize, GLdouble stickerSize);

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

    inline bool HasCenterType(int side, int &row, int &column, int color)
    {
        if (IsColor(side, row, column, color))
        {
            return true;
        }
        int temp = column;
        column = m_Size-1-row;
        row = temp;
        if (IsColor(side, row, column, color))
        {
            return true;
        }
        temp = column;
        column = m_Size-1-row;
        row = temp;
        if (IsColor(side, row, column, color))
        {
            return true;
        }
        temp = column;
        column = m_Size-1-row;
        row = temp;
        if (IsColor(side, row, column, color))
        {
            return true;
        }
        return false;
    }

    inline bool IsCorner(int num, int color1, int color2, int color3)
    {
        return IsColor(m_CornerPositions[num].side1, m_CornerPositions[num].row1*(m_Size-1), m_CornerPositions[num].column1*(m_Size-1), color1)
            && IsColor(m_CornerPositions[num].side2, m_CornerPositions[num].row2*(m_Size-1), m_CornerPositions[num].column2*(m_Size-1), color2)
            && IsColor(m_CornerPositions[num].side3, m_CornerPositions[num].row3*(m_Size-1), m_CornerPositions[num].column3*(m_Size-1), color3);
    }

    void FixTopEdge(int side1, int side2, int side3, int side4, int type1, int type2, int &count, Move* moves);
    void FixBottomEdge(int side1, int side2, int side3, int side4, int type1, int type2, int &count, Move* moves);
    void FixEdgeCenter(int side1, int side2, int &count, Move* moves);
    void FlipCenterEdge(int &count, Move* moves);

    void FixSideEdge(int side1, int side2, int &count, Move* moves);
    void FixSecondLastSideEdge(int &count, Move* moves);
    bool FixLastSideEdge(int &count, Move* moves);

    void FixEdgesOnSide(int side1, int side2, int side3, int side4, int color1, int color2, int &count, Move* moves);
    void FixEdgesOnSide(int side1, int side2, int side3, int side4, int color1, int color2, int type1, int &count, Move* moves, bool restore = false);

    void FixCenter(int type1, int type2, int type3, int row, int column, int &count, Move* moves);
    void FixCenter(int side1, int side2, int row1, int column1, int row2, int column2, int &count, Move* moves, int &faceMoveCount, Move* faceMoves);
    void FixCenters(int side1, int side2, int side3, int side4, int side5, int side6, int &count, Move* moves, int &faceMoveCount, Move* faceMoves);

    void FixCorners(int &count, Move* moves);
    void FixCorner1(int type1, int type2, int &count, Move* moves);
    void FixCorner2(int type1, int type2, int &count, Move* moves);
    void RotateCorner(int type1, int type2, int type3, int &count, Move* moves);
    void FixCorner(int num, int color1, int color2, int color3, int &count, Move* moves);
    void FixCorner(CornerMoves &cornerMoves, int &count, Move* moves);

    inline void RotatePosition(int &row, int &column, int angle)
    {
        int temp;
        switch(angle)
        {
        case 0:
            break;
        case 90:
            temp = row;
            row = column;
            column = m_Size-1-temp;
            break;
        case 180:
            row = m_Size-1-row;
            column = m_Size-1-column;
            break;
        case 270:
            temp = row;
            row = m_Size-1-column;
            column = temp;
            break;
        }
    }

    void PlayMoves(int &count, Move* moves);
    void PlayMovesReverse(int &count, Move* moves);
    void AddMove(int type, int depth, int &count, Move* moves);

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

