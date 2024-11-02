// CubeView.h : interface of the CCubeView class
//


#pragma once

#include <vector>

struct Piece
{
    unsigned char m_Color;
    unsigned char m_Row;
    unsigned char m_Column;
};

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
    virtual void OnEditOpen();
	virtual void OnViewRotateX();
	void RotateX(bool update = true);
    virtual void OnViewRotateCX();
    void RotateCX(bool update = true);
	virtual void OnViewRotateY();
	void RotateY(bool update = true);
    virtual void OnViewRotateCY();
    void RotateCY(bool update = true);
	virtual void OnViewRotateZ();
	void RotateZ(bool update = true);
    virtual void OnViewRotateCZ();
    void RotateCZ(bool update = true);
    virtual void OnSpeedSlower();
    virtual void OnSpeedFaster();
    virtual LRESULT OnDisplayStatus(WPARAM wParam, LPARAM lParam);
    virtual LRESULT OnDisplayStatus2(WPARAM wParam, LPARAM lParam);
    virtual LRESULT OnDoUp(WPARAM wParam, LPARAM lParam);

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
    int m_WindowSizeX;
    int m_WindowSizeY;
    CPoint m_Down;

    double m_AngleX;
    double m_AngleY;
    double m_AngleZ;
    std::vector<double> m_AnglesX;
    std::vector<double> m_AnglesY;
    std::vector<double> m_AnglesZ;

    std::vector<Piece> m_Sides[6];
    std::vector<Piece> m_SidesCopy[6];
    int m_SideAngles[6];
    int m_SideAnglesCopy[6];
    int m_Size;
    int m_Middle;
    int m_Moves;
    int m_Speed;
    bool m_Mixing;

    std::vector<Move> m_MovesToPlay;

    int m_lSleep;
    double m_dStep;
    volatile bool m_Abort;
    HANDLE m_Thread;
    FILE *m_LogFile;

    static int           m_defaultOverride[13];
    static PALETTEENTRY  m_defaultPalEntry[20];
    static CenterRelativePositions m_CentersPositions[6][6];
    static CornerPositions m_CornerPositions[8];
    static CornerMoves m_CornerMoves[8][8][3];
    bool m_CornerTableUsed[8][8][3];

    void Draw3D(int side, int x, int y, GLdouble cubieSize, GLdouble stickerSize);

    void InitCube();

    void RotateSideClockwise(int side);
    void RotateSideCounterClockwise(int side);
    void RotateSide180(int side);

    void RotateRight(int depth, bool update = true); // Type 0
    void RotateLeft(int depth, bool update = true);  // Type 1

    void RotateFront(int depth, bool update = true); // Type 2
    void RotateBack(int depth, bool update = true);  // Type 3

    void RotateTop(int depth, bool update = true);   // Type 4
    void RotateBottom(int depth, bool update = true);// Type 5

    void IncrementMoves();

    static unsigned int ScrambleStartEntry( void* arg );
    unsigned int ScrambleStart();

    static unsigned int SolveStartEntry( void* arg );
    unsigned int SolveStart();
    void Solve();

    bool IsEdge(int side1, int side2, int edge, int color1, int color2);
    inline bool IsColor(int side, int row, int column, int color)
    {
        if (m_MovesToPlay.size())
        {
            PlayMoves();
        }
        return (m_Sides[side][row*m_Size+column].m_Color == color);
    }

    inline bool IsColor(int side, int row, int column, int color, int row1, int column1)
    {
        if (m_MovesToPlay.size())
        {
            PlayMoves();
        }
        return (m_Sides[side][row*m_Size+column].m_Color == color)
            && (m_Sides[side][row*m_Size+column].m_Row == row1)
            && (m_Sides[side][row*m_Size+column].m_Column == column1);
    }

    inline bool HasCenterType(int side, int &row, int &column, Piece p, int minRow = 1, bool skipMiddle = false)
    {
        if (IsColor(side, row, column, p.m_Color, p.m_Row, p.m_Column) && (row >= minRow) && (!skipMiddle || (row != m_Middle)))
        {
            return true;
        }
        int temp = column;
        column = m_Size-1-row;
        row = temp;
        if (IsColor(side, row, column, p.m_Color, p.m_Row, p.m_Column) && (row >= minRow) && (!skipMiddle || (row != m_Middle)))
        {
            return true;
        }
        temp = column;
        column = m_Size-1-row;
        row = temp;
        if (IsColor(side, row, column, p.m_Color, p.m_Row, p.m_Column) && (row >= minRow) && (!skipMiddle || (row != m_Middle)))
        {
            return true;
        }
        temp = column;
        column = m_Size-1-row;
        row = temp;
        if (IsColor(side, row, column, p.m_Color, p.m_Row, p.m_Column) && (row >= minRow) && (!skipMiddle || (row != m_Middle)))
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

    int FixTopCenters();
    int FixBottomCenters();
    int FixEdgeCenters(int side1, int side2, int side3, int side4, int type1, int type2, bool preserve4);
    int FixEdgeCenter(int side1, int side2, int side3, int side4, int color, int typeA, int typeB, int column, int min, bool preserve4);

    void FixTopEdge(int side1, int side2, int side3, int side4, int type1, int type2);
    void FixBottomEdge(int side1, int side2, int side3, int side4, int type1, int type2);
    void FixEdgeCenter(int side1, int side2);
    void FlipCenterEdge();

    void FixSideEdge(int side1, int side2);
    void FixSecondLastSideEdge();
    bool FixLastSideEdge();

    void FlipEdge(int type1, int type2);

    bool FixEdgesOnSide(int side1, int side2, int side3, int side4, int color1, int color2, int type, bool top);
    bool FixEdgesOnSide(int side1, int side2, int side3, int side4, int color1, int color2, int type1, int type2, bool top);

    void FixCenter(int type1, int type2, int type3, int row, int column);
    void FixCenter(int side1, int side2, int row1, int column1, int row2, int column2);
    int FixSecondLastCenters();
    int FixLastCenters();

    void FixCorners(bool first = true);
    void FixCorner1(int type1, int type2);
    void FixCorner2(int type1, int type2);
    void RotateCorner(int type1, int type2, int type3);
    void FixCorner(int num, int color1, int color2, int color3);
    void FixCorner(CornerMoves &cornerMoves);

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

    void PlayMoves(std::vector<Move> &moves);
    inline void PlayMoves()
    {
        PlayMoves(m_MovesToPlay);
    }
    void PlayMovesReverse(std::vector<Move> &moves);
    void AddMove(int type, int depth, std::vector<Move> &moves);
    inline void AddMove(int type, int depth)
    {
        AddMove(type, depth, m_MovesToPlay);
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

