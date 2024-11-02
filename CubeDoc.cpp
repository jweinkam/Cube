// CubeDoc.cpp : implementation of the CCubeDoc class
//

#include "stdafx.h"
#include "Cube.h"

#include "CubeDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCubeDoc

IMPLEMENT_DYNCREATE(CCubeDoc, CDocument)

BEGIN_MESSAGE_MAP(CCubeDoc, CDocument)
END_MESSAGE_MAP()


// CCubeDoc construction/destruction

CCubeDoc::CCubeDoc()
{
	// TODO: add one-time construction code here

}

CCubeDoc::~CCubeDoc()
{
}

BOOL CCubeDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CCubeDoc serialization

void CCubeDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// CCubeDoc diagnostics

#ifdef _DEBUG
void CCubeDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CCubeDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CCubeDoc commands
