#include "stdafx.h"
#include "IJXmlTextWriter.h"
#include "jxmltextwriter.h"
#include "_UNCC.h"

CJXmlTextWriter::CJXmlTextWriter( DWORD dwMaxDataLength )
{
	m_nOutCursor = 0;
	m_dwStackTop = 0;
	m_bOutOfBuffer = FALSE;
	m_pUTF8Data = NULL;
	m_dwUTF8Size = 0;
	m_hOutFile = INVALID_HANDLE_VALUE;

	// allocate initial work buffer
	DWORD nActualIncLen = dwMaxDataLength;
	if( nActualIncLen < 2048 ) {
		nActualIncLen = 2048;
	}
	m_lpOutBuf = ( TCHAR * )::LocalAlloc( LPTR, nActualIncLen * sizeof( TCHAR ) );
	ATLASSERT( m_lpOutBuf != NULL );
	m_dwOutDataLen = nActualIncLen;
	m_dwOutBufInc = nActualIncLen;

	// to set properties default value
	m_bIndented = FALSE;
	m_chIndented = _T( ' ' );
	m_nIndentedShift = 2;
	m_nIndentedType = EIT_Space;
	m_bSaveToFile = FALSE;
	m_szFileName[ 0 ] = _T( '\0' );
}

CJXmlTextWriter::~CJXmlTextWriter( void )
{
	rewind();
	::LocalFree( m_lpOutBuf );
}

BOOL CJXmlTextWriter::get_Indented( EIndetType &nType, INT &nShift )
{
	nType = m_nIndentedType;
	nShift = m_nIndentedShift;

	return m_bIndented;
}

void CJXmlTextWriter::put_Indented( BOOL bIndented, EIndetType nType, INT nShift )
{
	ATLASSERT( bIndented == FALSE || bIndented == TRUE );

	switch( nType ) {
	case EIT_Space:
		m_nIndentedType = EIT_Space;
		m_chIndented = _T( ' ' );
		break;

	case EIT_Tab:
		m_nIndentedType = EIT_Tab;
		m_chIndented = _T( '\t' );
		break;

	default:
		ATLASSERT( FALSE );
		break;
	}

	m_nIndentedShift = nShift;
	m_bIndented = bIndented;
}

BOOL CJXmlTextWriter::get_SaveToFile( void )
{
	return m_bSaveToFile;
}

void CJXmlTextWriter::put_SaveToFile( BOOL bSaveToFile )
{
	ATLASSERT( bSaveToFile == FALSE || bSaveToFile == TRUE );
	m_bSaveToFile = bSaveToFile;
}

void CJXmlTextWriter::get_FileName( TCHAR *lpBuffer, UINT nLength )
{
	ATLASSERT( lpBuffer != NULL );
	ATLASSERT( nLength > 0 );

	_tcsncpy( lpBuffer, m_szFileName, nLength );
	lpBuffer[ nLength - 1 ] = _T( '\0' );
}

void CJXmlTextWriter::put_FileName( LPCTSTR lpszFileName )
{
	_tcsncpy( m_szFileName, lpszFileName, 256 );
	m_szFileName[ 255 ] = _T( '\0' );
}

void CJXmlTextWriter::rewind( void )
{
	m_nOutCursor = 0;
	m_dwStackTop = 0;
	m_bOutOfBuffer = FALSE;

	// free UTF8 buffer
	if( m_pUTF8Data != NULL ) {
		::LocalFree( m_pUTF8Data );
		m_pUTF8Data = NULL;
		m_dwUTF8Size = 0;
	}

	// close opened file
	if( m_hOutFile != INVALID_HANDLE_VALUE ) {
		CloseHandle( m_hOutFile );
		m_hOutFile = INVALID_HANDLE_VALUE;
	}
}

HRESULT CJXmlTextWriter::flushCacheToFile( void )
{
	ATLASSERT( m_bSaveToFile == TRUE );
	ATLASSERT( m_bOutOfBuffer == FALSE );

	// create file
	if( m_hOutFile == INVALID_HANDLE_VALUE ) {
		m_hOutFile = CreateFile(
								m_szFileName,
								GENERIC_WRITE,
								0,
								NULL,
								CREATE_ALWAYS,
								FILE_ATTRIBUTE_NORMAL,
								NULL
								);
		if( m_hOutFile == INVALID_HANDLE_VALUE ) {
			return E_FAIL;
		}
	}

	ATLASSERT( m_hOutFile != INVALID_HANDLE_VALUE );

	// convert into UTF8 format
	LPBYTE lpUTF8Data = NULL;
	UINT nUTF8Size;
	_UNCC converter( FALSE );
	converter.setSource( m_lpOutBuf, m_nOutCursor );
	converter.detachUTF8Converted( &lpUTF8Data, &nUTF8Size );

	if( lpUTF8Data != NULL ) {

		// write into file
		DWORD nNumberOfBytesWritten;
		WriteFile( m_hOutFile, lpUTF8Data, nUTF8Size, &nNumberOfBytesWritten, NULL );
		::LocalFree( lpUTF8Data );

		// check whether write operation successful.
		ATLASSERT( ( DWORD )nUTF8Size == nNumberOfBytesWritten );
		if( ( DWORD )nUTF8Size != nNumberOfBytesWritten ) {
			return E_FAIL;
		}
	}

	// reset output cursor
	m_nOutCursor = 0;

	return S_OK;
}

void CJXmlTextWriter::get_Xml( TCHAR **ppData, LPDWORD lpdwLength )
{
	ATLASSERT( ppData != NULL );
	ATLASSERT( lpdwLength != NULL );
	ATLASSERT( m_bSaveToFile == FALSE );

	// return empty under SaveToFile mode
	if( m_bSaveToFile == TRUE ) {
		*ppData = NULL;
		*lpdwLength = 0;
		return;
	}

	*ppData = m_lpOutBuf;
	*lpdwLength = m_nOutCursor;
}

void CJXmlTextWriter::get_XmlUTF8( LPBYTE *ppData, LPDWORD lpdwLength )
{
	ATLASSERT( ppData != NULL );
	ATLASSERT( lpdwLength != NULL );
	ATLASSERT( m_bSaveToFile == FALSE );

	// return empty under SaveToFile mode
	if( m_bSaveToFile == TRUE ) {
		*ppData = NULL;
		*lpdwLength = 0;
		return;
	}

	// convert into UTF8 format
	if( m_pUTF8Data == NULL ) {
		_UNCC converter( FALSE );
		converter.setSource( m_lpOutBuf, m_nOutCursor );
		converter.detachUTF8Converted( &m_pUTF8Data, ( UINT * )&m_dwUTF8Size );
	}

	*ppData = m_pUTF8Data;
	*lpdwLength = m_dwUTF8Size;
}

void CJXmlTextWriter::WriteStartDocument( BOOL bXmlHeader, BOOL bUTF8 )
{
	// rewind output buffer
	rewind();

	if( bXmlHeader == FALSE ) {
		return;
	}

	// output document header
	if( bUTF8 == TRUE ) {
		WriteOut( _T("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"), -1 );
	}
	else {
		WriteOut( _T("<?xml version=\"1.0\"?>"), -1 );
	}
}

void CJXmlTextWriter::WriteStartElement( LPCTSTR name )
{
	TNode *pNode;
	if( m_dwStackTop != 0 ) {
		pNode = &m_OpenTagStack[ m_dwStackTop - 1 ];

		// output ">" to close start tag before write new start element
		if( pNode->state == STATE_OpenForAttribute ) {
			WriteOut( _T( ">" ), 1 );
			pNode->state = STATE_OpenForElement;
		}
	}

	ATLASSERT( m_dwStackTop < SIZE_OpenTagStack );
	ATLASSERT( _tcslen( name ) < LEN_TagName );

	// put new element into open tag stack
	pNode =  &m_OpenTagStack[ m_dwStackTop ];
	pNode->state = STATE_OpenForAttribute;
	_tcscpy( pNode->szTagName, name );

	// output "<name"
	if( m_bIndented == TRUE ) {
		WriteIdented();
	}
	WriteOut( _T( "<" ), 1 );
	WriteOut( name, -1 );
	m_dwStackTop++;
}

void CJXmlTextWriter::WriteAttributeString( LPCTSTR name, LPCTSTR value )
{
	ATLASSERT( m_dwStackTop > 0 );

	TNode *pNode = &m_OpenTagStack[ m_dwStackTop - 1 ];
	ATLASSERT( pNode->state == STATE_OpenForAttribute );

	// output " name="value""
	WriteOut( _T( " " ), 1 );
	WriteOut( name, -1 );
	WriteOut( _T( "=\"" ), 2 );
	WriteOut( value, -1 );
	WriteOut( _T( "\"" ), 1 );
}

void CJXmlTextWriter::WriteElementString( LPCTSTR name, LPCTSTR value )
{
	ATLASSERT( m_dwStackTop > 0 );

	TNode *pNode = &m_OpenTagStack[ m_dwStackTop - 1 ];
	ATLASSERT( pNode->state == STATE_OpenForAttribute || pNode->state == STATE_OpenForElement );

	// output ">" to close start tag before write new elemet string
	if( pNode->state == STATE_OpenForAttribute ) {
		WriteOut( _T( ">" ), 1 );
		pNode->state = STATE_OpenForElement;
	}

	// output "<name>value</name>"
	if( m_bIndented == TRUE ) {
		WriteIdented();
	}
	WriteOut( _T( "<" ), 1 );
	WriteOut( name, -1 );
	WriteOut( _T( ">" ), 1 );
	WriteOut( value, -1 );
	WriteOut( _T( "</" ), 2 );
	WriteOut( name, -1 );
	WriteOut( _T( ">" ), 1 );
}

void CJXmlTextWriter::WriteRaw( LPCTSTR lpData, INT nLength )
{
	ATLASSERT( m_dwStackTop > 0 );

	TNode *pNode = &m_OpenTagStack[ m_dwStackTop - 1 ];
	ATLASSERT( pNode->state == STATE_OpenForAttribute || pNode->state == STATE_OpenForElement );

	// output ">" to close start tag before write new elemet string
	if( pNode->state == STATE_OpenForAttribute ) {
		WriteOut( _T( ">" ), 1 );
		pNode->state = STATE_OpenForElement;
	}

	// output "<name>value</name>"
	if( m_bIndented == TRUE ) {
		WriteIdented();
	}

	// output raw markup data
	WriteOut( lpData, nLength );
}

void CJXmlTextWriter::WriteEndElement( void )
{
	ATLASSERT( m_dwStackTop > 0 );

	TNode *pNode = &m_OpenTagStack[ m_dwStackTop - 1 ];
	ATLASSERT( pNode->state == STATE_OpenForAttribute || pNode->state == STATE_OpenForElement );

	// popup one closed element from stack
	m_dwStackTop--;

	// close topest opened element
	if( pNode->state == STATE_OpenForAttribute ) {
		// output "/>"
		WriteOut( _T( "/>" ), 2 );
	}
	else {
		// output "</name>"
		if( m_bIndented == TRUE ) {
			WriteIdented();
		}
		WriteOut( _T( "</" ), 2 );
		WriteOut( pNode->szTagName, -1 );
		WriteOut( _T( ">" ), 1 );
	}
}

void CJXmlTextWriter::WriteEndDocument( void )
{
	// close all opened element
	while( m_dwStackTop > 0 ) {
		WriteEndElement();
	}
	m_lpOutBuf[ m_nOutCursor ] = _T( '\0' );

	// flush output cache into file
	if( m_bSaveToFile == TRUE ) {
		if( m_bOutOfBuffer == FALSE ) {
			flushCacheToFile();
		}

		if( m_hOutFile != INVALID_HANDLE_VALUE ) {
			CloseHandle( m_hOutFile );
			m_hOutFile = INVALID_HANDLE_VALUE;
		}
	}
}

void CJXmlTextWriter::WriteOut( LPCTSTR lpData, INT nLength )
{
	if( m_bOutOfBuffer == TRUE ) {
		return;
	}

	if( nLength == -1 ) {
		nLength = _tcslen( lpData );
	}
	ATLASSERT( nLength >= 0 );

	// check buffer size, reserve one character for null character
	if( m_nOutCursor + nLength + 1 > m_dwOutDataLen ) {
		if( m_bSaveToFile == TRUE ) {
			HRESULT hr = flushCacheToFile();
			if( FAILED( hr ) ) {
				m_bOutOfBuffer = TRUE;
				return;
			}
			// enlarge working buffer for large raw data write access
			if( ( DWORD )( nLength + 1 ) > m_dwOutDataLen ) {
				HRESULT hr = _UNCC::checkLocalTableSize( nLength + 1, ( void ** )&m_lpOutBuf, &m_dwOutDataLen, sizeof( TCHAR ), m_dwOutBufInc );
				if( FAILED( hr ) ) {
					m_bOutOfBuffer = TRUE;
					return;
				}
				ATLASSERT( m_lpOutBuf != NULL );
			}
		}
		else {
			HRESULT hr = _UNCC::checkLocalTableSize( m_nOutCursor + nLength + 1, ( void ** )&m_lpOutBuf, &m_dwOutDataLen, sizeof( TCHAR ), m_dwOutBufInc );
			if( FAILED( hr ) ) {
				m_bOutOfBuffer = TRUE;
				return;
			}
			ATLASSERT( m_lpOutBuf != NULL );
		}
	}
	ATLASSERT( m_nOutCursor + nLength + 1 <= m_dwOutDataLen );

	MoveMemory( &m_lpOutBuf[ m_nOutCursor ], lpData, nLength * sizeof( TCHAR ) );
	m_nOutCursor += nLength;
}

void CJXmlTextWriter::WriteIdented()
{
	ATLASSERT( m_bIndented == TRUE );

	if( m_bOutOfBuffer == TRUE ) {
		return;
	}

	INT nLength = m_nIndentedShift * m_dwStackTop + 2;

	// check buffer size, reserve one character for null character
	if( m_nOutCursor + nLength + 1 > m_dwOutDataLen ) {
		if( m_bSaveToFile == TRUE ) {
			HRESULT hr = flushCacheToFile();
			if( FAILED( hr ) ) {
				m_bOutOfBuffer = TRUE;
				return;
			}
		}
		else {
			HRESULT hr = _UNCC::checkLocalTableSize( m_nOutCursor + nLength + 1, ( void ** )&m_lpOutBuf, &m_dwOutDataLen, sizeof( TCHAR ), m_dwOutBufInc );
			if( FAILED( hr ) ) {
				m_bOutOfBuffer = TRUE;
				return;
			}
			ATLASSERT( m_lpOutBuf != NULL );
		}
	}
	ATLASSERT( m_nOutCursor + nLength + 1 <= m_dwOutDataLen );

	register TCHAR *lpCursor = &m_lpOutBuf[ m_nOutCursor ];
	register TCHAR *lpUpper = &m_lpOutBuf[ m_nOutCursor + nLength ];

	*lpCursor++ = _T( '\r' );
	*lpCursor++ = _T( '\n' );

	while( lpCursor < lpUpper ) {
		*lpCursor++ = m_chIndented;
	}

	m_nOutCursor += nLength;
}
