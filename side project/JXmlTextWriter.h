#if !defined(_JXMLTEXTWRITER_INCLUDED_)
#define _JXMLTEXTWRITER_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IJXmlTextWriter.h"

class CJXmlTextWriter :
	public IJXmlTextWriter
{
public:

	CJXmlTextWriter( DWORD dwMaxDataLength );
	~CJXmlTextWriter( void );

	// properties
	BOOL get_Indented( EIndetType &nType, INT &nShift );
	void put_Indented( BOOL bIndented, EIndetType nType = EIT_Space, INT nShift = 2 );
	BOOL get_SaveToFile( void );
	void put_SaveToFile( BOOL bSaveToFile );
	void get_FileName( TCHAR *lpBuffer, UINT nLength );
	void put_FileName( LPCTSTR lpszFileName );

	// access output data
	void get_Xml( TCHAR **ppData, LPDWORD lpdwLength );
	void get_XmlUTF8( LPBYTE *ppData, LPDWORD lpdwLength );

	// format functions
	void WriteStartDocument( BOOL bXmlHeader, BOOL bUTF8 = TRUE );
	void WriteStartElement( LPCTSTR name );
	void WriteAttributeString( LPCTSTR name, LPCTSTR value );
	void WriteElementString( LPCTSTR name, LPCTSTR value );
	void WriteRaw( LPCTSTR lpData, INT nLength );
	void WriteEndElement( void );
	void WriteEndDocument( void );

protected:
	void WriteOut( LPCTSTR lpData, INT nLength );
	void WriteIdented( void );
	HRESULT flushCacheToFile( void );
	void rewind( void );

public:
	enum EMaxBounds {
		LEN_TagName			= 64,
		SIZE_OpenTagStack	= 32
	};

private:
	enum ENodeState {
		STATE_Closed,
		STATE_OpenForAttribute,
		STATE_OpenForElement
	};

	struct TNode {
		int		state;
		TCHAR	szTagName[ LEN_TagName ];
	};

	// properties
	BOOL m_bIndented;
	TCHAR m_chIndented;
	INT m_nIndentedShift;
	EIndetType m_nIndentedType;
	BOOL m_bSaveToFile;
	TCHAR m_szFileName[ 256 ];

	HANDLE m_hOutFile;
	BOOL m_bOutOfBuffer;

	TCHAR *m_lpOutBuf;
	DWORD m_nOutCursor;
	LPBYTE m_pUTF8Data;
	DWORD m_dwUTF8Size;

	TNode m_OpenTagStack[ SIZE_OpenTagStack ];
	DWORD m_dwStackTop;

	DWORD m_dwOutDataLen;
	DWORD m_dwOutBufInc;
};

#endif // _JXMLTEXTWRITER_INCLUDED_