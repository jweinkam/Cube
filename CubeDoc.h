// CubeDoc.h : interface of the CCubeDoc class
//


#pragma once

class CCubeDoc : public CDocument
{
protected: // create from serialization only
	CCubeDoc();
	DECLARE_DYNCREATE(CCubeDoc)

// Attributes
public:

// Operations
public:

// Overrides
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CCubeDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};


