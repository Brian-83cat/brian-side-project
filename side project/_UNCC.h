// _UNCC.h: interface for the _UNCC class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX__UNCC_H__1EB31A44_2851_11D3_8436_0000E86B4150__INCLUDED_)
#define AFX__UNCC_H__1EB31A44_2851_11D3_8436_0000E86B4150__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class _UNCC
//-----------------------------------------------------------------
// UNICODE helper class
{
public:
	_UNCC( BOOL bFreeSource );
	virtual ~_UNCC();

	_UNCC( LPCSTR sz,LONG nLanguageValue );
	_UNCC( LPCWSTR sz,LONG nLanguageValue );
	void _UNCC::SetStrValue(LPCSTR sz);
	void _UNCC::SetStrValue(LPCWSTR sz);
	void _UNCC::CheckPageCode(LONG nLanguageValue);
	
	// ansi character
	_UNCC( LPCSTR sz );
	operator LPSTR();
	void setSource( LPSTR lpData, UINT nLength );
	void detachConverted( LPSTR *lpData, UINT *lpnLength );

	// wide character
	_UNCC( LPCWSTR sz );
	operator LPWSTR();
	void setSource( LPWSTR lpData, UINT nLength );
	void detachConverted( LPWSTR *lpData, UINT *lpnLength );

	// UTF-8 character
	void setUTF8Source( LPBYTE lpData, UINT nLength );
	void detachUTF8Converted( LPBYTE *lpData, UINT *lpnLength );

	static UINT HashKey(LPCTSTR key);
	static HRESULT checkLocalTableSize( DWORD dwRequiredSize, void **lpTable, LPDWORD lpdwTableSize, DWORD nElementSize, DWORD dwIncremental );

private:
	LPWSTR m_uniString;
	LPSTR m_ansiString;
	LPBYTE m_utf8String;
	int m_nSourceType;
	int m_nUniLength;
	int m_nAnsiLength;
	int m_nUtf8Length;
	BOOL m_bFreeSource;
	CHAR m_nullAnsiString[1];
	WCHAR m_nullUniString[1];

	enum ESourceType {
		SOURCE_None,
		SOURCE_Unicode,
		SOURCE_ANSI,
		SOURCE_UTF8
	};

	void releaseMemoryResource( void );
	// to release memory resource

	void UTF82UNI( void );
	void UNI2UTF8( void );
	void ANSI2UNI( void );
	void UNI2ANSI( void );
};

#endif // !defined(AFX__UNCC_H__1EB31A44_2851_11D3_8436_0000E86B4150__INCLUDED_)
