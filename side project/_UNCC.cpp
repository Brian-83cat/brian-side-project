// _UNCC.cpp: implementation of the _UNCC class.
//
//////////////////////////////////////////////////////////////////////

#define _AFXDLL

#include "stdafx.h"
#include "_UNCC.h"

//------------------------------------------------------------------
// Description
// 1.it is adapted to be consistent with UTF16,
// 2.the interface converts a whole buffer to avoid function-call overhead
//   constants have been gathered.
// 3.loops & conditionals have been removed as much as possible for
//   efficiency, in favor of drop-through switch statements.
//------------------------------------------------------------------

typedef unsigned long	UCS4;
typedef unsigned short	UTF16;
typedef unsigned char	UTF8;

typedef enum {
	CR_ok, 				/* conversion successful */
	CR_sourceExhausted,	/* partial character in source, but hit end */
	CR_targetExhausted		/* insuff. room in target for conversion */
} ConversionResult;

static const UCS4 kReplacementCharacter	= 0x0000FFFDUL;
static const UCS4 kMaximumUCS2			= 0x0000FFFFUL;
static const UCS4 kMaximumUTF16			= 0x0010FFFFUL;
static const UCS4 kMaximumUCS4			= 0x7FFFFFFFUL;

static const int halfShift				= 10;
static const UCS4 halfBase				= 0x0010000UL;
static const UCS4 halfMask				= 0x3FFUL;
static const UCS4 kSurrogateHighStart	= 0xD800UL;
static const UCS4 kSurrogateHighEnd		= 0xDBFFUL;
static const UCS4 kSurrogateLowStart	= 0xDC00UL;
static const UCS4 kSurrogateLowEnd		= 0xDFFFUL;

static const UCS4 offsetsFromUTF8[6] = {
	0x00000000UL, 0x00003080UL, 0x000E2080UL, 
	0x03C82080UL, 0xFA082080UL, 0x82082080UL
};

static const char bytesFromUTF8[256] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5
};

static const UTF8 firstByteMark[7] = {
	0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC
};

static ConversionResult ConvertUTF16toUTF8 (
		UTF16** sourceStart, const UTF16* sourceEnd, 
		UTF8** targetStart, const UTF8* targetEnd);

static ConversionResult ConvertUTF8toUTF16 (
		UTF8** sourceStart, UTF8* sourceEnd, 
		UTF16** targetStart, const UTF16* targetEnd);

ConversionResult ConvertUTF16toUTF8 (
		UTF16** sourceStart, const UTF16* sourceEnd, 
		UTF8** targetStart, const UTF8* targetEnd)
{
	ConversionResult result = CR_ok;
	register UTF16* source = *sourceStart;
	register UTF8* target = *targetStart;
	while (source < sourceEnd) {
		register UCS4 ch;
		register unsigned short bytesToWrite = 0;
		register const UCS4 byteMask = 0xBF;
		register const UCS4 byteMark = 0x80; 
		ch = *source++;
		if (ch >= kSurrogateHighStart && ch <= kSurrogateHighEnd
				&& source < sourceEnd) {
			register UCS4 ch2 = *source;
			if (ch2 >= kSurrogateLowStart && ch2 <= kSurrogateLowEnd) {
				ch = ((ch - kSurrogateHighStart) << halfShift)
					+ (ch2 - kSurrogateLowStart) + halfBase;
				++source;
			};
		};
		if (ch < 0x80) {				bytesToWrite = 1;
		} else if (ch < 0x800) {		bytesToWrite = 2;
		} else if (ch < 0x10000) {		bytesToWrite = 3;
		} else if (ch < 0x200000) {		bytesToWrite = 4;
		} else if (ch < 0x4000000) {	bytesToWrite = 5;
		} else if (ch <= kMaximumUCS4){	bytesToWrite = 6;
		} else {						bytesToWrite = 2;
										ch = kReplacementCharacter;
		}; /* I wish there were a smart way to avoid this conditional */
		
		target += bytesToWrite;
		if (target > targetEnd) {
			target -= bytesToWrite; result = CR_targetExhausted; break;
		};
		switch (bytesToWrite) {	/* note: code falls through cases! */
			case 6:	*--target = (ch | byteMark) & byteMask; ch >>= 6;
			case 5:	*--target = (ch | byteMark) & byteMask; ch >>= 6;
			case 4:	*--target = (ch | byteMark) & byteMask; ch >>= 6;
			case 3:	*--target = (ch | byteMark) & byteMask; ch >>= 6;
			case 2:	*--target = (ch | byteMark) & byteMask; ch >>= 6;
			case 1:	*--target =  ch | firstByteMark[bytesToWrite];
		};
		target += bytesToWrite;
	};
	*sourceStart = source;
	*targetStart = target;
	return result;
};

ConversionResult ConvertUTF8toUTF16 (
		UTF8** sourceStart, UTF8* sourceEnd, 
		UTF16** targetStart, const UTF16* targetEnd)
{
	ConversionResult result = CR_ok;
	register UTF8* source = *sourceStart;
	register UTF16* target = *targetStart;
	while (source < sourceEnd) {
		register UCS4 ch = 0;
		register unsigned short extraBytesToWrite = bytesFromUTF8[*source];
		if (source + extraBytesToWrite > sourceEnd) {
			result = CR_sourceExhausted; break;
		};
		switch(extraBytesToWrite) {	/* note: code falls through cases! */
			case 5:	ch += *source++; ch <<= 6;
			case 4:	ch += *source++; ch <<= 6;
			case 3:	ch += *source++; ch <<= 6;
			case 2:	ch += *source++; ch <<= 6;
			case 1:	ch += *source++; ch <<= 6;
			case 0:	ch += *source++;
		};
		ch -= offsetsFromUTF8[extraBytesToWrite];

		if (target >= targetEnd) {
			result = CR_targetExhausted; break;
		};
		if (ch <= kMaximumUCS2) {
			*target++ = ch;
		} else if (ch > kMaximumUTF16) {
			*target++ = kReplacementCharacter;
		} else {
			if (target + 1 >= targetEnd) {
				result = CR_targetExhausted; break;
			};
			ch -= halfBase;
			*target++ = (ch >> halfShift) + kSurrogateHighStart;
			*target++ = (ch & halfMask) + kSurrogateLowStart;
		};
	};
	*sourceStart = source;
	*targetStart = target;
	return result;
};

// Grobal variable, ANSI Code Page
UINT g_nAnsiCodePage;
//------------------------------------------------------------------
// Internal helper function for _UNCC class
//------------------------------------------------------------------

static int UTF8ToWideChar( LPBYTE lpUTF8, int cchUTF8, LPWSTR lpBuffer, int nLength );
// the return value is the number of wide characters

static int WideCharToUTF8( LPWSTR lpWideChar, int cchWideChar, LPBYTE lpBuffer, int nLength );
// the return value is the number of UTF-8 characters

int UTF8ToWideChar( LPBYTE lpUTF8, int cchUTF8, LPWSTR lpBuffer, int nLength )
// the return value is the number of wide characters
{
	LPWSTR lpCursor = lpBuffer;
	ConversionResult cr = ConvertUTF8toUTF16( &lpUTF8, (UTF8 *)(lpUTF8 + cchUTF8), (UTF16 **)&lpCursor, (UTF16 *)(lpBuffer + nLength) );
	if( cr == CR_ok ) {
		return lpCursor - lpBuffer;
	}
	return 0;
}

int WideCharToUTF8( LPWSTR lpWideChar, int cchWideChar, LPBYTE lpBuffer, int nLength )
// the return value is the number of UTF-8 characters
{
	LPBYTE lpCursor = lpBuffer;
	ConversionResult cr = ConvertUTF16toUTF8( (UTF16 **)&lpWideChar, (UTF16 *)(lpWideChar + cchWideChar), &lpCursor, lpBuffer + nLength );
	if( cr == CR_ok ) {
		return lpCursor - lpBuffer;
	}
	return 0;
}

//------------------------------------------------------------------
// Class _UNCC: unicode conversion calss
//------------------------------------------------------------------
_UNCC::_UNCC( BOOL bFreeSource )
{
	m_uniString = NULL;
	m_ansiString = NULL;
	m_utf8String = NULL;
	m_nUniLength = 0;
	m_nAnsiLength = 0;
	m_nUtf8Length = 0;
	m_bFreeSource = bFreeSource;
	m_nSourceType = SOURCE_None;
}
_UNCC::_UNCC( LPCSTR sz,LONG nLanguageValue )
{
	CheckPageCode(nLanguageValue);
	SetStrValue(sz);
}
_UNCC::_UNCC( LPCWSTR sz,LONG nLanguageValue )
{
	CheckPageCode(nLanguageValue);
	SetStrValue(sz);
}
_UNCC::_UNCC( LPCSTR sz )
{
	SetStrValue(sz);
}

_UNCC::_UNCC( LPCWSTR sz )
{
	SetStrValue(sz);
}

_UNCC::~_UNCC()
{
	releaseMemoryResource();
}

void _UNCC::setUTF8Source( LPBYTE lpData, UINT nLength )
{
	ATLASSERT( lpData != NULL );
	ATLASSERT( nLength > 0 );
	ATLASSERT( m_nSourceType == SOURCE_None ); 

	m_utf8String = lpData;
	m_nUtf8Length = (int)nLength;
	m_nSourceType = SOURCE_UTF8;
}

void _UNCC::setSource( LPWSTR lpData, UINT nLength )
{
	ATLASSERT( lpData != NULL );
	ATLASSERT( nLength > 0 );
	ATLASSERT( m_nSourceType == SOURCE_None ); 

	m_uniString = lpData;
	m_nUniLength = (int)nLength;
	m_nSourceType = SOURCE_Unicode;
}

void _UNCC::setSource( LPSTR lpData, UINT nLength )
{
	ATLASSERT( lpData != NULL );
	ATLASSERT( nLength > 0 );
	ATLASSERT( m_nSourceType == SOURCE_None ); 

	m_ansiString = lpData;
	m_nAnsiLength = (int)nLength;
	m_nSourceType = SOURCE_ANSI;
}

void _UNCC::releaseMemoryResource( void )
// to release memory resource
{
	if( m_bFreeSource ) {

		// free all data
		if( m_uniString ) {
			::LocalFree( m_uniString );
			m_uniString = NULL;
		}
		if( m_ansiString ) {
			::LocalFree( m_ansiString );
			m_ansiString = NULL;
		}
		if( m_utf8String ) {
			::LocalFree( m_utf8String );
			m_utf8String = NULL;
		}

	} else {

		// no release source data
		switch( m_nSourceType ) {
			case SOURCE_Unicode:
				if( m_ansiString ) {
					::LocalFree( m_ansiString );
					m_ansiString = NULL;
				}
				if( m_utf8String ) {
					::LocalFree( m_utf8String );
					m_utf8String = NULL;
				}
				break;

			case SOURCE_ANSI:
				if( m_uniString ) {
					::LocalFree( m_uniString );
					m_uniString = NULL;
				}
				if( m_utf8String ) {
					::LocalFree( m_utf8String );
					m_utf8String = NULL;
				}
				break;

			case SOURCE_UTF8:
				if( m_uniString ) {
					::LocalFree( m_uniString );
					m_uniString = NULL;
				}
				if( m_ansiString ) {
					::LocalFree( m_ansiString );
					m_ansiString = NULL;
				}
				break;
		}
	}

	m_nUniLength = 0;
	m_nAnsiLength = 0;
	m_nUtf8Length = 0;
	m_bFreeSource = FALSE;
	m_nSourceType = SOURCE_None;
}

void _UNCC::UTF82UNI( void )
{
	ATLASSERT( m_utf8String != NULL );
	ATLASSERT( m_uniString == NULL );

	if( !m_utf8String ) {
		return;
	}

	// calculate source string length include zero-terminate character
	if( m_nUtf8Length == -1 ) {
		register LPBYTE lpCursor = m_utf8String;
		while( *lpCursor++ );
		m_nUtf8Length = lpCursor - m_utf8String;
	}

	// allocate buffer for target string
	DWORD dwBufferSize = m_nUtf8Length * sizeof(WCHAR);
	LPWSTR lpBuffer = (LPWSTR)::LocalAlloc( LMEM_FIXED, dwBufferSize );
	ATLASSERT( lpBuffer != NULL );

	// convert it
	DWORD nCharCount = UTF8ToWideChar( m_utf8String, m_nUtf8Length, lpBuffer, m_nUtf8Length );
	ATLASSERT( nCharCount != 0 );

	// save it
	m_uniString = lpBuffer;
	m_nUniLength = nCharCount;
}

void _UNCC::UNI2UTF8( void )
{
	ATLASSERT( m_uniString != NULL );
	ATLASSERT( m_utf8String == NULL );

	if( !m_uniString ) {
		return;
	}

	// calculate source string length include zero-terminate character
	if( m_nUniLength == -1 ) {
		register WCHAR *lpCursor = m_uniString;
		while( *lpCursor++ );
		m_nUniLength = lpCursor - m_uniString;
	}

	// allocate buffer for target string
	DWORD dwBufferSize = m_nUniLength * sizeof(WCHAR);
	LPBYTE lpBuffer = (LPBYTE)::LocalAlloc( LMEM_FIXED, dwBufferSize );
	ATLASSERT( lpBuffer != NULL );

	// convert it
	DWORD nCharCount = WideCharToUTF8( m_uniString, m_nUniLength, lpBuffer, dwBufferSize );
	ATLASSERT( nCharCount != 0 );

	// save it
	m_utf8String = lpBuffer;
	m_nUtf8Length = nCharCount;
}

void _UNCC::ANSI2UNI( void )
{
	ATLASSERT( m_ansiString != NULL );
	ATLASSERT( m_uniString == NULL );

	if( !m_ansiString ) {
		return;
	}

	// calculate source string length include zero-terminate character
	if( m_nAnsiLength == -1 ) {
		register CHAR *lpCursor = m_ansiString;
		while( *lpCursor++ );
		m_nAnsiLength = lpCursor - m_ansiString;
	}

	// allocate buffer for target string
	// double buff length avoid convert error
	//		when buffer size is not enough MultiByteToWideChar would return 0 (Error code 122)
	int BufferLength = 2 * m_nAnsiLength;
	DWORD dwBufferSize = BufferLength * sizeof(WCHAR);
	LPWSTR lpBuffer = (LPWSTR)::LocalAlloc( LMEM_FIXED, dwBufferSize );
	ATLASSERT( lpBuffer != NULL );

	// convert it
	DWORD nCharCount = MultiByteToWideChar(
						g_nAnsiCodePage,	// code page
						MB_COMPOSITE,		// character-type options
						m_ansiString,		// address of string to map
						m_nAnsiLength,		// number of bytes in string
						lpBuffer,			// address of wide-character buffer
						BufferLength		// size of buffer in wchar
					);

	// if conver fail, then use ANSI code page convert again
	if(nCharCount == 0) {
		nCharCount = MultiByteToWideChar(
						0,					// 0 : CP_ACP	default is ANSI codepage
						MB_COMPOSITE,		// character-type options
						m_ansiString,		// address of string to map
						m_nAnsiLength,		// number of bytes in string
						lpBuffer,			// address of wide-character buffer
						BufferLength		// size of buffer in wchar
					);
	}

	// save it
	m_uniString = lpBuffer;
	m_nUniLength = nCharCount;
}

void _UNCC::UNI2ANSI( void )
{
	ATLASSERT( m_uniString != NULL );
	ATLASSERT( m_ansiString == NULL );

	if( !m_uniString ) {
		return;
	}

	if( m_nUniLength == -1 ) {
		register WCHAR *lpCursor = m_uniString;
		while( *lpCursor++ );
		m_nUniLength = lpCursor - m_uniString;
	}

	DWORD dwBufferSize = sizeof(WCHAR) * m_nUniLength;
	CHAR *lpBuffer = (CHAR *)::LocalAlloc( LMEM_FIXED, dwBufferSize );
	ATLASSERT( lpBuffer != NULL );

	// skip unicode signature character
	WCHAR *lpSrcBegin;
	DWORD nSrcSize;
	if( *(WCHAR *)m_uniString == 0xFEFF ) {
		lpSrcBegin = m_uniString + 1;
		nSrcSize = m_nUniLength - 1;
	} else {
		lpSrcBegin = m_uniString;
		nSrcSize = m_nUniLength;
	}

	// convert it
	int nCharCount = WideCharToMultiByte(
						g_nAnsiCodePage,				// code page
						0,					// performance and mapping flags
						lpSrcBegin,			// address of wide-character string
						nSrcSize,			// number of characters in string
						lpBuffer,			// address of buffer for new string
						dwBufferSize,		// size of buffer
						NULL,				// address of default for unmappable
											//  characters
						NULL				// address of flag set when default
											//  char. used
					);

	// if conver fail, then use ANSI code page convert again
	if(nCharCount == 0) {
		nCharCount = WideCharToMultiByte(
						0,					// 0 : CP_ACP	default is ANSI codepage
						0,					// performance and mapping flags
						lpSrcBegin,			// address of wide-character string
						nSrcSize,			// number of characters in string
						lpBuffer,			// address of buffer for new string
						dwBufferSize,		// size of buffer
						NULL,				// address of default for unmappable
											//  characters
						NULL				// address of flag set when default
											//  char. used
					);
	}

	// save it
	m_ansiString = lpBuffer;
	m_nAnsiLength = nCharCount;
}

_UNCC::operator LPSTR()
{
	// return ANSI string
	if( m_ansiString ) {
		return m_ansiString;
	}

	// convert ANSI string from UNICODE string
	if( m_nSourceType == SOURCE_Unicode ) {
		UNI2ANSI();
	}

	// conver ANSI string from UTF-8 string
	if( m_nSourceType == SOURCE_UTF8 ) {
		if( !m_uniString ) {
			UTF82UNI();
		}
		UNI2ANSI();
	}

	if( m_ansiString ) {
		return m_ansiString;
	}
	
	m_nullAnsiString[0] = '\0';
	return m_nullAnsiString;
}

_UNCC::operator LPWSTR()
{
	// return ANSI string
	if( m_uniString ) {
		return m_uniString;
	}

	// convert UNICODE string from ANSI string
	if( m_nSourceType == SOURCE_ANSI ) {
		ANSI2UNI();
	}

	// conver UNICODE string from UTF-8 string
	if( m_nSourceType == SOURCE_UTF8 ) {
		UTF82UNI();
	}

	if( m_uniString ) {
		return m_uniString;
	}

	m_nullUniString[0] = L'\0';
	return m_nullUniString;
}

void _UNCC::detachConverted( LPWSTR *lpData, UINT *lpnLength )
{
	// convert data from other data format
	if( !m_uniString ) {
		if( m_nSourceType == SOURCE_UTF8 ) {
			UTF82UNI();
		} else 	if( m_nSourceType == SOURCE_ANSI ) {
			ANSI2UNI();
		}
	}

	// to calculate data length
	if( m_uniString && (m_nUniLength == -1) ) {
		register WCHAR *lpCursor = m_uniString;
		while( *lpCursor++ );
		m_nUniLength = lpCursor - m_uniString;
	}

	// detach it
	*lpData = m_uniString;
	*lpnLength = m_nUniLength;
	m_uniString = NULL;
	m_nUniLength = 0;
	m_nSourceType = SOURCE_None;
}

void _UNCC::detachConverted( LPSTR *lpData, UINT *lpnLength )
{
	// convert data from other data format
	if( !m_ansiString ) {
		if( !m_uniString && (m_nSourceType == SOURCE_UTF8) ) {
			UTF82UNI();
		}
		UNI2ANSI();
	}

	// to calculate data length
	if( m_ansiString && (m_nAnsiLength == -1) ) {
		register CHAR *lpCursor = m_ansiString;
		while( *lpCursor++ );
		m_nAnsiLength = lpCursor - m_ansiString;
	}

	// detach it
	*lpData = m_ansiString;
	*lpnLength = m_nAnsiLength;
	m_ansiString = NULL;
	m_nAnsiLength = 0;
	m_nSourceType = SOURCE_None;
	return;
}

void _UNCC::detachUTF8Converted( LPBYTE *lpData, UINT *lpnLength )
{
	// convert data from other data format
	if( !m_utf8String ) {
		if( !m_uniString && (m_nSourceType == SOURCE_ANSI) ) {
			ANSI2UNI();
		}
		UNI2UTF8();
	}

	// to calculate data length
	if( m_utf8String && (m_nUtf8Length == -1) ) {
		register LPBYTE lpCursor = m_utf8String;
		while( *lpCursor++ );
		m_nUtf8Length = lpCursor - m_utf8String;
	}

	// detach it
	*lpData = m_utf8String;
	*lpnLength = m_nUtf8Length;
	m_utf8String = NULL;
	m_nUtf8Length = 0;
	m_nSourceType = SOURCE_None;
}

UINT _UNCC::HashKey(LPCTSTR key)
{
	ATLASSERT( key != NULL );

	register UINT nHash = 0;
	register const TCHAR *lpCursor = key;

	while( *lpCursor ) {
		nHash = (nHash<<5) + nHash + *lpCursor++;
	}

	return nHash;
}

HRESULT _UNCC::checkLocalTableSize( DWORD dwRequiredSize, void **lpTable, LPDWORD lpdwTableSize, DWORD nElementSize, DWORD dwIncremental )
{
	ATLASSERT( lpTable != NULL );
	ATLASSERT( lpdwTableSize != NULL );
	ATLASSERT( nElementSize > 0 );
	ATLASSERT( dwIncremental > 0 );

	// table room is enough
	if( dwRequiredSize < *lpdwTableSize ) {
		return S_OK;
	}

	DWORD nNewTableSize = ((dwRequiredSize + dwIncremental - 1) / dwIncremental) * dwIncremental;

	HLOCAL hMem = (HLOCAL)*lpTable;
	if( hMem ) {
		// enlarge exist table
		hMem = ::LocalReAlloc( hMem, nElementSize * nNewTableSize, LMEM_MOVEABLE );
	} else {
		// create a new table
		hMem = ::LocalAlloc( LPTR, nElementSize * nNewTableSize );

	}

	// check out of memory exception
	if( hMem == NULL ) {
		return E_OUTOFMEMORY;
	}

	// prepare output
	*lpTable = (void *)hMem;
	*lpdwTableSize = nNewTableSize;

	return S_OK;
}
void _UNCC::CheckPageCode(LONG nLanguageValue)
{
	// Ansi Code Page
	UINT AnsiCodePage[150] = {
				/* ID 100 ~109 */	1256,	1256,	1256,	1256,	1256,	1256,	1256,	1256,	1256,	1256,
				/* ID 110 ~119 */	1256,	1256,	1256,	1256,	1256,	1256,	1251,	1252,	 950,	 936,
				/* ID 120 ~129 */	 950,	 936,	 950,	1250,	1252,	1252,	1252,	1252,	1252,	1252,
				/* ID 130 ~139 */	1253,	1252,	1252,	1252,	1252,	1252,	1252,	1252,	1252,	1252,
				/* ID 140 ~149 */	1252,	1252,	1252,	1252,	1252,	1252,	1252,	1252,	1252,	1252,
				/* ID 150 ~159 */	1252,	1252,	1252,	1252,	1252,	1252,	1252,	1252,	1252,	1252,
				/* ID 160 ~169 */	1252,	1252,	1252,	1252,	1252,	1252,	1252,	1252,	1252,	1252,
				/* ID 170 ~179 */	1255,	1250,	1252,	1252,	1252,	 932,	 949,	1252,	1252,	1252,
				/* ID 180 ~189 */	1252,	1250,	1252,	1252,	1250,	1251,	1250,	1250,	1251,	1250,
				/* ID 190 ~199 */	1250,	1252,	1252,	 874,	1254,	1256,	1252,	1251,	1251,	1250,
				/* ID 200 ~209 */	1257,	1257,	1257,	1257,	1256,	1258,	   0,	1254,	1251,	1252,
				/* ID 210 ~219 */	1251,	1252,	   0,	1252,	   0,	1252,	1252,	1251,	1252,	1254,
				/* ID 220 ~229 */	1251,	1251,	   0,	   0,	   0,	   0,	   0,	   0,	   0,	   0,
				/* ID 230 ~233 */	   0,	   0,	   0,	   0 };
	
	if((nLanguageValue == 0) || (nLanguageValue==1) || (nLanguageValue >=100 && nLanguageValue <=233))
	{
		switch(nLanguageValue)
		{
		case 0:
			g_nAnsiCodePage = 0;
		case 1:
			g_nAnsiCodePage = 950; //Chinese
			break;
		default:
			g_nAnsiCodePage = AnsiCodePage[nLanguageValue-100];
		}
	}
}
void _UNCC::SetStrValue(LPCSTR sz)
{
	ATLASSERT( sz != NULL );

	m_uniString = NULL;
	m_ansiString = (LPSTR)sz;
	m_utf8String = NULL;
	m_nUniLength = 0;
	m_nAnsiLength = -1;
	m_nUtf8Length = 0;
	m_bFreeSource = FALSE;
	m_nSourceType = SOURCE_ANSI;
}
void _UNCC::SetStrValue(LPCWSTR sz)
{
	ATLASSERT( sz != NULL );

	m_uniString = (LPWSTR)sz;
	m_ansiString = NULL;
	m_utf8String = NULL;
	m_nUniLength = -1;
	m_nAnsiLength = 0;
	m_nUtf8Length = 0;
	m_bFreeSource = FALSE;
	m_nSourceType = SOURCE_Unicode;
}