#if !defined(_IJXMLTEXTWRITER_INCLUDED_)
#define _IJXMLTEXTWRITER_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum EIndetType {
	EIT_Space,
	EIT_Tab,
};

class IJXmlTextWriter
{
public:
	virtual ~IJXmlTextWriter( void ) {}
	// destructor

	// properties
	virtual BOOL get_Indented( EIndetType &nType, INT &nShift ) = 0;
	virtual void put_Indented( BOOL bIndented, EIndetType nType = EIT_Space, INT nShift = 2 ) = 0;
	virtual BOOL get_SaveToFile( void ) = 0;
	virtual void put_SaveToFile( BOOL bSaveToFile ) = 0;
	virtual void get_FileName( TCHAR *lpBuffer, UINT nLength ) = 0;
	virtual void put_FileName( LPCTSTR lpszFileName ) = 0;

	// access output data
	virtual void get_Xml( TCHAR **ppData, LPDWORD lpdwLength ) = 0;
	virtual void get_XmlUTF8( LPBYTE *ppData, LPDWORD lpdwLength ) = 0;

	// format functions
	virtual void WriteStartDocument( BOOL bXmlHeader, BOOL bUTF8 = TRUE ) = 0;
	virtual void WriteStartElement( LPCTSTR name ) = 0;
	virtual void WriteAttributeString( LPCTSTR name, LPCTSTR value ) = 0;
	virtual void WriteElementString( LPCTSTR name, LPCTSTR value ) = 0;
	virtual void WriteRaw( LPCTSTR lpData, INT nLength ) = 0;
	virtual void WriteEndElement( void ) = 0;
	virtual void WriteEndDocument( void ) = 0;
};

#endif // _IJXMLTEXTWRITER_INCLUDED_