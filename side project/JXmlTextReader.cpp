#include "SafeMemory.h"
#include "jxmltextreader.h"
#include "_UNCC.h"

CJXmlTextReader::CJXmlTextReader(void)
{
	m_dwStackTop = 0;
	m_nStreamCursor = 0;
	m_pCurrent = NULL;
	m_lpDataBuf = NULL;
	m_lpXmlStream = NULL;
	m_nXmlStreamSize = 0;
	m_nReaderState = READERSTATE_EOF;

	// allocate element stack
	m_lpElementStack = NULL;
	SAFENEW( m_lpElementStack, TNode[ SIZE_ElementStack ] );
}

CJXmlTextReader::~CJXmlTextReader(void)
{
	if( m_lpDataBuf ) {
		::LocalFree( m_lpDataBuf );
	}

	SAFEDELETE_ARRAY( m_lpElementStack );
}

HRESULT CJXmlTextReader::loadFile( LPCTSTR lpszFilename, BOOL bValidate )
{
	// free state
	if( m_lpDataBuf ) {
		::LocalFree( m_lpDataBuf );

		m_dwStackTop = 0;
		m_nStreamCursor = 0;
		m_pCurrent = NULL;
		m_nReaderState = READERSTATE_EOF;
		m_lpDataBuf = NULL;
		m_lpXmlStream = NULL;
		m_nXmlStreamSize = 0;
	}

	HANDLE hFile = CreateFile(
					lpszFilename,
					GENERIC_READ,
					FILE_SHARE_READ,
					NULL,
					OPEN_EXISTING,
					FILE_ATTRIBUTE_NORMAL,
					NULL );

	if( hFile == INVALID_HANDLE_VALUE ) {
		// can't open specified file
		return E_FAIL;
	}

	BOOL bUnicode = FALSE;
	BOOL bHasUnicodeSignature = FALSE;

	// get file size
	LPBYTE lpDataBuf;
	DWORD dwDataSize = ::GetFileSize( hFile , NULL );
	DWORD dwWorkBufferSize = dwDataSize * sizeof(WCHAR);

	lpDataBuf = (LPBYTE)::LocalAlloc( LMEM_FIXED, dwDataSize );
	ATLASSERT( lpDataBuf != NULL );

	DWORD BytesRead;
	ReadFile( hFile, lpDataBuf, dwDataSize, &BytesRead, NULL );
	ATLASSERT( dwDataSize == BytesRead );
	dwDataSize = BytesRead;

	// close data file
	CloseHandle( hFile );

	// put data into converter
	_UNCC converter( TRUE );
	if( isUnicodeString( lpDataBuf, dwDataSize ) ) {
		converter.setSource( (LPWSTR)lpDataBuf, dwDataSize / sizeof(WCHAR) ); 
	} else {
		converter.setUTF8Source( lpDataBuf, dwDataSize );
	}

	// get desired format data from converter
	converter.detachConverted( (LPTSTR *)&m_lpDataBuf, &m_nXmlStreamSize );
	m_lpXmlStream = (TCHAR *)m_lpDataBuf;

#if defined(UNICODE)
	// skip unicode signature character
	if( *(TCHAR *)m_lpDataBuf == 0xFEFF ) {
		m_lpXmlStream++;
		m_nXmlStreamSize--;
	}
#endif

	m_nReaderState = READERSTATE_Ready;

	if( bValidate ) {
		while( read() );
		if( m_nReaderState < 0 ) {
			rewind();
			return E_FAIL;
		}
		rewind();
	}

	return S_OK;
}
	
HRESULT CJXmlTextReader::loadXML( LPCTSTR lpszXmlString, int nLength, BOOL bValidate )
// to load specified XML string data
{
	// free state
	if( m_lpDataBuf ) {
		::LocalFree( m_lpDataBuf );

		m_dwStackTop = 0;
		m_nStreamCursor = 0;
		m_pCurrent = NULL;
		m_nReaderState = READERSTATE_EOF;
		m_lpDataBuf = NULL;
		m_lpXmlStream = NULL;
		m_nXmlStreamSize = 0;
	}

	if( nLength == -1 ) {
		nLength = _tcslen( lpszXmlString );
	}
	ATLASSERT( nLength >= 0 );

	DWORD dwWorkBufferSize =(nLength + 1) * sizeof(TCHAR);
	TCHAR *lpBuffer = (TCHAR *)::LocalAlloc( LMEM_FIXED, dwWorkBufferSize );
	ATLASSERT( lpBuffer != NULL );
	if( lpBuffer == NULL ) {
		return E_OUTOFMEMORY;
	}

	// copy data into internal buffer
	memmove( lpBuffer, lpszXmlString, dwWorkBufferSize );

	m_lpDataBuf = (LPBYTE)lpBuffer;
	m_lpXmlStream = lpBuffer;
	m_nXmlStreamSize = nLength;

	m_nReaderState = READERSTATE_Ready;

	if( bValidate ) {
		while( read() );
		if( m_nReaderState < 0 ) {
			rewind();
			return E_FAIL;
		}
		rewind();
	}

	return S_OK;
}

void CJXmlTextReader::rewind( void )
// rewind reader pointer to header of stream
{
	m_dwStackTop = 0;
	m_nStreamCursor = 0;
	m_pCurrent = NULL;
	m_nReaderState = READERSTATE_Ready;
}

BOOL CJXmlTextReader::isUnicodeString( LPBYTE lpData, DWORD dwLength )
{
	if( (dwLength >= 2) && (lpData[0] == 0xFF) && (lpData[1] == 0xFE) ) {
		return TRUE;
	}

	return FALSE;
}

BOOL CJXmlTextReader::isEmptyElement( void )
// Tests if the current content node is a <.../> empty element
{
	if( isStartElement() ) {
		return m_pCurrent->bIsEmpty;
	}

	return FALSE;
}

BOOL CJXmlTextReader::isStartElement( LPCTSTR lpszName, INT nLength )
// Tests if the current content node is a start tag.
{
	if( isStartElement() ) {
		// compare node name
		if( nLength == -1 ) {
			nLength = _tcslen( lpszName );
		}
		ATLASSERT( nLength >= 0 );

		return isSameString( lpszName, nLength, &m_lpXmlStream[m_pCurrent->nNameBegin], m_pCurrent->nNameLen );
	}

	return FALSE;
}

BOOL CJXmlTextReader::isStartElement( void )
// Tests if the current content node is a start tag.
{
	if( !m_pCurrent ) {

		// read next node
		read();

		// exit when EOF
		if( m_nReaderState ) {
			return FALSE;
		}

		ATLASSERT( m_pCurrent != NULL );
	}

	return m_pCurrent->nNodeType == NODE_Element;
}

BOOL CJXmlTextReader::hasAttributes( void )
// Gets a value indicating whether the current node has any attributes.
// true if the current node has attributes; otherwise, false.
{
	ATLASSERT( m_pCurrent != NULL );
	ATLASSERT( m_pCurrent->nNodeType == NODE_Element || m_pCurrent->nNodeType == NODE_Attribute );

	return m_pCurrent->bHasAttribute; 
}

BOOL CJXmlTextReader::moveToNextAttribute( void )
// Moves to the next attribute.
// true if there is a next attribute; false if there are no more attributes.
{
	ATLASSERT( m_pCurrent != NULL );
	ATLASSERT( (m_pCurrent->nNodeType == NODE_Element) || (m_pCurrent->nNodeType == NODE_Attribute) );

	// process first move case
	if( m_pCurrent->nNodeType == NODE_Element ) {
		memcpy( m_pCurrent + 1, m_pCurrent, sizeof(TNode) );
		m_pCurrent++;
		m_dwStackTop++;
		m_pCurrent->nNodeType = NODE_Attribute;
		m_pCurrent->nNextChar = m_pCurrent->nNameBegin + m_pCurrent->nNameLen;
	}

	return scanAttribute();
}

BOOL CJXmlTextReader::moveToAttribute( LPCTSTR lpszName )
// move to specified attribute
// true if there is a next attribute; false if there are no more attributes.
{
	INT nNameLen = _tcslen(lpszName);

	while( moveToNextAttribute() ) {
		if( isSameString( lpszName, nNameLen , &m_lpXmlStream[m_pCurrent->nNameBegin], m_pCurrent->nNameLen ) ) {
			return TRUE;
		}
	}
	return FALSE;
}

void CJXmlTextReader::moveToElement( void )
// move cursor from attribute to element
{
	ATLASSERT( m_pCurrent != NULL );
	ATLASSERT( (m_pCurrent->nNodeType == NODE_Element) || (m_pCurrent->nNodeType == NODE_Attribute) );

	if( m_pCurrent->nNodeType == NODE_Attribute ) {
		m_pCurrent--;
		m_dwStackTop--;
		ATLASSERT( m_dwStackTop >= 0 );
	}
}

BOOL CJXmlTextReader::read( void )
// Reads the next node from the stream.
// true if the next node was read successfully; false if there are no more nodes to read.
{
	// check if it is EOF
	if( m_nReaderState ) {
		return FALSE;
	}

	// shift current node
	if( m_pCurrent ) {
		if( m_pCurrent->nNodeType == NODE_Element && !m_pCurrent->bIsEmpty ) {
			m_dwStackTop++;
		} else if( m_pCurrent->nNodeType == NODE_EndElement ) {

			// detect start element and element name is not match exception
			ATLASSERT( m_dwStackTop >= 1 );
			if( !isElementNameMatch( m_pCurrent, m_pCurrent - 1 ) ) {
				m_nReaderState = READERSTATE_ElementNameNotMatch;
				m_pCurrent = NULL;
				return FALSE;
			}

			m_dwStackTop--;
			ATLASSERT( m_dwStackTop >= 0 );

		} else if( m_pCurrent->nNodeType == NODE_Attribute) {
			if( m_pCurrent[-1].bIsEmpty ) {
				m_dwStackTop--;
			}
		}
	}

	// read next node
	m_pCurrent = &m_lpElementStack[m_dwStackTop];
	BOOL bSuccess = scanToken( m_pCurrent );
	if( bSuccess ) {
		return TRUE;
	}

	ATLASSERT( m_dwStackTop == 0 );

	// detect too early EOF exception
	if( m_dwStackTop > 0 ) {
		m_nReaderState = READERSTATE_TooEarlyEOF;
		m_pCurrent = NULL;
		return FALSE;
	}

	// end of data stream
	m_nReaderState = READERSTATE_EOF;
	m_pCurrent = NULL;

	return FALSE;
}

void CJXmlTextReader::skip( void )
// Skips the children of the current node.
{
	// record skip depth
	UINT nSkipDepth;
	if( m_lpElementStack[m_dwStackTop].nNodeType == NODE_Element ) {
		nSkipDepth = m_dwStackTop;
	} else {
		nSkipDepth = m_dwStackTop - 1;
	}

	// read token until cursor back to skip depth or encounter EOF
	while( read() ) {
		if( nSkipDepth == m_dwStackTop ) {
			break;
		}
	}
}

BOOL CJXmlTextReader::readString( TCHAR *lpBuffer, UINT nLength )
// reads the contents of an element or text node as a string.
{
	ATLASSERT( m_pCurrent != NULL );
	ATLASSERT( m_pCurrent->nNodeType == NODE_Element );
	ATLASSERT( lpBuffer != NULL );
	ATLASSERT( nLength > 0 );

	if( m_pCurrent->bIsEmpty ) {
		lpBuffer[0] = _T('\0');
		return TRUE;
	}

	BOOL bSuccess = read();
	if( bSuccess ) {
		if( m_pCurrent->nNodeType == NODE_Text ) {
			copyEntityReferenceString( &m_lpXmlStream[m_pCurrent->nBegin], m_pCurrent->nLength, lpBuffer, nLength );
			read();	// read end element
		} else {
			lpBuffer[0] = _T('\0');
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CJXmlTextReader::readElementString( TCHAR *lpBuffer, UINT nLength )
// to read element string
{
	ATLASSERT( m_pCurrent != NULL );
	ATLASSERT( m_pCurrent->nNodeType == NODE_Element );
	ATLASSERT( lpBuffer != NULL );
	ATLASSERT( nLength > 0 );

	if( m_pCurrent->bIsEmpty ) {
		lpBuffer[0] = _T('\0');
		// move to next elemnt
		read();
		return TRUE;
	}

	BOOL bSuccess = read();
	if( bSuccess ) {
		if( m_pCurrent->nNodeType == NODE_Text ) {
			copyEntityReferenceString( &m_lpXmlStream[m_pCurrent->nBegin], m_pCurrent->nLength, lpBuffer, nLength );
			read();	// read end element
		} else {
			lpBuffer[0] = _T('\0');
		}

		// move to next elemnt
		read();
		return TRUE;
	}

	return FALSE;
}

BOOL CJXmlTextReader::readOuterXml( TCHAR *lpBuffer, UINT nLength )
// reads the content, including markup, representing this node and all its children.
{
	ATLASSERT( m_pCurrent != NULL );
	ATLASSERT( m_pCurrent->nNodeType == NODE_Element );
	ATLASSERT( lpBuffer != NULL );
	ATLASSERT( nLength > 0 );

	// record skip depth
	UINT nSkipDepth = m_dwStackTop;
	UINT nBegin = m_pCurrent->nBegin;
	UINT nLastEnd = m_pCurrent->nBegin + m_pCurrent->nLength;

	// read token until cursor back to skip depth or encounter EOF
	while( read() ) {
		if( nSkipDepth == m_dwStackTop ) {
			break;
		} else {
			nLastEnd = m_pCurrent->nBegin + m_pCurrent->nLength;
		}
	}

	if( nSkipDepth == m_dwStackTop ) {
		UINT nDataLen = nLastEnd - nBegin;
		UINT nCopyLen = min( nLength-1, nDataLen );
		_tcsncpy( lpBuffer, &m_lpXmlStream[nBegin], nCopyLen );
		lpBuffer[nCopyLen] = _T('\0');
		return TRUE;
	}

	return FALSE;
}

BOOL CJXmlTextReader::readInnerXml( TCHAR *lpBuffer, UINT nLength )
// reads the content, including markup, representing this node and all its children.
{
	ATLASSERT( m_pCurrent != NULL );
	ATLASSERT( m_pCurrent->nNodeType == NODE_Element );
	ATLASSERT( lpBuffer != NULL );
	ATLASSERT( nLength > 0 );

	if( m_pCurrent->bIsEmpty ) {
		lpBuffer[0] = _T('\0');
		// move to next elemnt
		read();
		return TRUE;
	}

	// record skip depth
	UINT nSkipDepth = m_dwStackTop;
	UINT nBegin = m_pCurrent->nBegin + m_pCurrent->nLength;
	UINT nLastEnd = nBegin;

	// read token until cursor back to skip depth or encounter EOF
	while( read() ) {
		if( nSkipDepth == m_dwStackTop ) {
			break;
		} else {
			nLastEnd = m_pCurrent->nBegin;
		}
	}

	if( nSkipDepth == m_dwStackTop ) {
		UINT nDataLen = nLastEnd - nBegin;
		UINT nCopyLen = min( nLength-1, nDataLen );
		_tcsncpy( lpBuffer, &m_lpXmlStream[nBegin], nCopyLen );
		lpBuffer[nCopyLen] = _T('\0');
		return TRUE;
	}

	return FALSE;
}

void CJXmlTextReader::get_Name( TCHAR *lpBuffer, UINT nLength )
// Gets the qualified name of the current node.
{
	ATLASSERT( m_pCurrent != NULL );

	UINT nCopyLen = min( nLength - 1, m_pCurrent->nNameLen );
	_tcsncpy( lpBuffer, &m_lpXmlStream[m_pCurrent->nNameBegin], nCopyLen );
	lpBuffer[nCopyLen] = _T('\0');
}

void CJXmlTextReader::get_Value( TCHAR *lpBuffer, UINT nLength )
// Gets the text value of the current node.
{
	ATLASSERT( m_pCurrent != NULL );

	copyEntityReferenceString( &m_lpXmlStream[m_pCurrent->nValueBegin], m_pCurrent->nValueLen, lpBuffer, nLength );
}

BOOL CJXmlTextReader::scanAttribute( void )
{
	ATLASSERT( m_pCurrent != NULL );
	ATLASSERT( m_pCurrent->nNodeType == NODE_Attribute );

	UINT nCursor = m_pCurrent->nNextChar;
	UINT nIndexBound = m_pCurrent->nBegin + m_pCurrent->nLength - 1;

	// scan attribute name
	INT nPos = PositionOfNonSpace( &m_lpXmlStream[nCursor], nIndexBound - nCursor );
	if( nPos == -1 ) {
		m_pCurrent->nNextChar = nIndexBound;
		return FALSE;
	}
	nCursor += nPos;
	m_pCurrent->nNameBegin = nCursor;

	nPos = PositionOfChar( _T('='), &m_lpXmlStream[nCursor], nIndexBound - nCursor );
	if( nPos == -1 ) {
		m_pCurrent->nNextChar = nIndexBound;
		return FALSE;
	}

	// trim tail space of attribute name
	UINT nNameLen = PositionOfTailingSpace( &m_lpXmlStream[nCursor], nPos );
	ATLASSERT( nNameLen >= 0 );
	m_pCurrent->nNameLen = nNameLen;

	// advance cursor
	nCursor += nPos + 1;

	// scan attribute value
	nPos = PositionOfChar( _T('\"'), &m_lpXmlStream[nCursor], nIndexBound - nCursor );
	if( nPos == -1 ) {
		m_pCurrent->nNextChar = nIndexBound;
		return FALSE;
	}
	nCursor += nPos + 1;

	// move to first non-space character
	nPos = PositionOfNonSpace( &m_lpXmlStream[nCursor], nIndexBound - nCursor );
	if( nPos == -1 ) {
		m_pCurrent->nNextChar = nIndexBound;
		return FALSE;
	}
	nCursor += nPos;
	m_pCurrent->nValueBegin = nCursor;

	nPos = PositionOfChar( _T('\"'), &m_lpXmlStream[nCursor], nIndexBound - nCursor );
	if( nPos == -1 ) {
		m_pCurrent->nNextChar = nIndexBound;
		return FALSE;
	}

	// trim tail space of attribute name
	UINT nValueLen = PositionOfTailingSpace( &m_lpXmlStream[nCursor], nPos );
	ATLASSERT( nValueLen >= 0 );
	m_pCurrent->nValueLen = nValueLen;

	// update next character pointer
	m_pCurrent->nNextChar = nCursor + nPos + 1;

	return TRUE;
}

BOOL CJXmlTextReader::scanToken( TNode *lpToken )
{
	TCHAR ch;

	while( scanRawToken( lpToken ) ) {
		switch( lpToken->nNodeType ) {
			case NODE_EndElement:
				lpToken->nNameBegin = lpToken->nBegin + 2;
				lpToken->nNameLen = lpToken->nLength - 3;
				return TRUE;
				break;

			case NODE_Element:
				{
					// calculate maximum index bound
					UINT IndexBound;
					ch = m_lpXmlStream[lpToken->nBegin + lpToken->nLength - 2];
					if( ch == _T('/') ) {
						IndexBound = lpToken->nBegin + lpToken->nLength - 2;
						lpToken->bIsEmpty = TRUE;
					} else {
						IndexBound = lpToken->nBegin + lpToken->nLength - 1;
						lpToken->bIsEmpty = FALSE;
					}

					// extract element name
					lpToken->nNameBegin = lpToken->nBegin + 1;
					INT nPos = PositionOfSpace( &m_lpXmlStream[lpToken->nNameBegin], IndexBound - lpToken->nNameBegin );
					if( nPos == -1 ) {
						lpToken->nNameLen = IndexBound - lpToken->nNameBegin;
					} else {
						lpToken->nNameLen = nPos;
					}

					// calculate HasAttribute property
					INT nAttribBegin = lpToken->nNameBegin + lpToken->nNameLen;
					nPos = PositionOfChar( _T('='), &m_lpXmlStream[nAttribBegin], IndexBound - nAttribBegin );
					if( nPos == -1 ) {
						lpToken->bHasAttribute = FALSE;
					} else {
						lpToken->bHasAttribute = TRUE;
					}

					return TRUE;
				}
				break;

			case NODE_Text:
				{
					INT nPos = PositionOfNonSpace( &m_lpXmlStream[lpToken->nBegin], lpToken->nLength );
					if( nPos >= 0 ) {
						// return true when this is not whitespace string
						return TRUE;
					}
				}
				// skip whitespace string
				break;

			default:
				// skip processing instruction token
				// skip comment token
				// skip whitespace token
				break;
		}
	}
	return FALSE;
}

BOOL CJXmlTextReader::scanRawToken( TNode *lpToken )
{
	INT nPos;

	if( m_nStreamCursor >= m_nXmlStreamSize ) {
		return FALSE;
	}

	// save start psotion for later use
	UINT nBegin = m_nStreamCursor;

	// when begin character is a space character, then search whitespace token
	if( isSpaceChar( m_lpXmlStream[nBegin] ) ) {
		// to skip leading space character string
		nPos = PositionOfNonSpace(&m_lpXmlStream[m_nStreamCursor], m_nXmlStreamSize - m_nStreamCursor);
		if( nPos == -1 ) {
			m_nStreamCursor = m_nXmlStreamSize;
		} else {
			m_nStreamCursor += nPos;
		}
		ATLASSERT( m_nStreamCursor > nBegin );

		lpToken->nNodeType = NODE_Whitespace;
		lpToken->nBegin  = nBegin;
		lpToken->nLength = m_nStreamCursor - nBegin;
		return TRUE;
	}

	if( m_lpXmlStream[m_nStreamCursor] == _T('<') ) {
		// search xml node

		nPos = PositionOfEndTag( &m_lpXmlStream[m_nStreamCursor], m_nXmlStreamSize - m_nStreamCursor );
		if( nPos == -1 ) {
			return FALSE;
		}

		// update read buffer pointer
		m_nStreamCursor += nPos + 1;

		// identify node type
		TCHAR ch = m_lpXmlStream[nBegin + 1];
		if( ch == _T('/') ) {
		
			// end element
			lpToken->nNodeType = NODE_EndElement;
		} else if( ch == _T('!') ) {

			// comment node
			lpToken->nNodeType = NODE_Comment;
		} else if( ch == _T('?') ) {

			// processing instruction
			lpToken->nNodeType = NODE_ProcessingInstruction;
		} else {

			// start element
			lpToken->nNodeType = NODE_Element;
		}

	} else {
		// search string

		nPos = PositionOfChar( _T('<'), &m_lpXmlStream[m_nStreamCursor], m_nXmlStreamSize - m_nStreamCursor );
		if( nPos == -1 ) {
			m_nStreamCursor = m_nXmlStreamSize;
		} else {
			m_nStreamCursor += nPos;
		}

		// to skip tailing space character string
		nPos = PositionOfTailingSpace( &m_lpXmlStream[nBegin], m_nStreamCursor - nBegin );
		ATLASSERT( nPos >= 0 );
		m_nStreamCursor = nBegin + nPos;
		
		lpToken->nNodeType = NODE_Text;
	}
	lpToken->nBegin  = nBegin;
	lpToken->nLength = m_nStreamCursor - nBegin;

	return TRUE;
}

inline BOOL CJXmlTextReader::isSpaceChar( TCHAR ch )
{
	return (ch >= 0x09 && ch <= 0x0D) || (ch == 0x20);
}

inline BOOL CJXmlTextReader::isElementNameMatch( TNode *lpElement1, TNode *lpElement2 )
// to compare whether element name is match
{
	ATLASSERT( (lpElement1->nNodeType == NODE_Element) || (lpElement1->nNodeType == NODE_EndElement) );
	ATLASSERT( (lpElement2->nNodeType == NODE_Element) || (lpElement2->nNodeType == NODE_EndElement) );

	if( (lpElement1->nNameLen == lpElement2->nNameLen) &&
		!memcmp( &m_lpXmlStream[lpElement1->nNameBegin],  &m_lpXmlStream[lpElement2->nNameBegin], sizeof(TCHAR) * lpElement1->nNameLen ) ) {
		return TRUE;
	}

	return FALSE;
}

inline INT CJXmlTextReader::PositionOfSpace( LPCTSTR lpData, INT nLength )
{
	register const TCHAR *lpCursor = lpData;
	register const TCHAR *lpBound = lpData + nLength;

	while( lpCursor < lpBound ) {
		if( isSpaceChar(*lpCursor++) ) {
			return lpCursor - lpData - 1;
		}
	}
	return -1;
}

inline INT CJXmlTextReader::PositionOfNonSpace( LPCTSTR lpData, INT nLength )
{
	register const TCHAR *lpCursor = lpData;
	register const TCHAR *lpBound = lpData + nLength;

	while( lpCursor < lpBound ) {
		if( !isSpaceChar(*lpCursor++) ) {
			return lpCursor - lpData - 1;
		}
	}
	return -1;
}

inline INT CJXmlTextReader::PositionOfTailingSpace( LPCTSTR lpData, INT nLength )
{
	register const TCHAR *lpCursor = lpData + nLength;
	register const TCHAR *lpBound = lpData;

	while( --lpCursor >= lpBound ) {
		if( !isSpaceChar(*lpCursor) ) {
			return lpCursor - lpData + 1;
		}
	}
	return 0;
}

inline BOOL CJXmlTextReader::isSameString( LPCTSTR lpStr1, INT nStr1Len, LPCTSTR lpStr2, INT nStr2Len )
{
	if( nStr1Len != nStr2Len ) {
		return FALSE;
	}

	register const TCHAR *lpCursor1 = lpStr1;
	register const TCHAR *lpBound1 = lpStr1 + nStr1Len;
	register const TCHAR *lpCursor2 = lpStr2;
 
	while( lpCursor1 < lpBound1 ) {
		if( _totupper(*lpCursor1++) != _totupper(*lpCursor2++) ) {
			return FALSE;
		}
 	}

 	return TRUE;

}

inline BOOL CJXmlTextReader::isLeadWith( LPCTSTR lpLead, INT nLeadLen,  LPCTSTR lpData, INT nLength )
{
	if( nLength < nLeadLen ) {
		return FALSE;
	}

	register const TCHAR *lpCursor = lpData;
	register const TCHAR *lpBound = lpData + nLeadLen;
	register const TCHAR *lpKey = lpLead;

	while( lpCursor < lpBound ) {
		if( *lpKey++ != *lpCursor++ ) {
			return FALSE;
		}
	}

	return TRUE;
}

INT CJXmlTextReader::PositionOfEndTag( LPCTSTR lpData, INT nLength )
{
	register const TCHAR *lpCursor = lpData;
	register const TCHAR *lpBound = lpData + nLength;
	register TCHAR ch;

	// scan comment node
	if( isLeadWith( _T("<!--"), 4, lpData, nLength ) ) {
		int state = 0;
		lpCursor += 4;	// skip comment lead
		while ( (state < 3) && (lpCursor < lpBound) ) {
			ch = *lpCursor++;
			switch( state ) {
				case 0:
					if( ch == _T('-') ) {
						state++;
					}
					break;

				case 1:
					if( ch == _T('-') ) {
						state++;
					} else {
						state = 0;
					}
					break;

				case 2:
					if( ch == _T('-') ) {
						break;
					} else if( ch == _T('>') ) {
						state = 3;
					} else {
						state = 0;
					}
					break;
			}
		}
		return lpCursor - lpData - 1;
	}

	BOOL bOutQuatation = TRUE;
	while( lpCursor < lpBound ) {
		// count quotation mark
		ch = *lpCursor++;

		if( ch == _T('\"') ) {
			bOutQuatation = (bOutQuatation ? FALSE : TRUE);
		}

		// // check close tag character when ouside of quation mark
		if( bOutQuatation && (ch == _T('>')) ) {
			return lpCursor - lpData - 1;
		}
	}
	return -1;
}

inline INT CJXmlTextReader::PositionOfChar( TCHAR ch, LPCTSTR lpData, INT nLength )
{
	register const TCHAR *lpCursor = lpData;
	register const TCHAR *lpBound = lpData + nLength;

	while( lpCursor < lpBound ) {
		if( ch == *lpCursor++ ) {
			return lpCursor - lpData - 1;
		}
	}
	return -1;
}

inline WCHAR CJXmlTextReader::toDecimal( LPCTSTR lpData, DWORD dwLength )
{
	register WCHAR value = 0;
	register const TCHAR *lpCursor = lpData;
	register const TCHAR *lpUpper = lpData + dwLength;

	while( lpCursor < lpUpper ) {
		value = (value * 10) + (*lpCursor++ - _T('0'));
	}

	return value;
}

inline WCHAR CJXmlTextReader::toHex( LPCTSTR lpData, DWORD dwLength )
{
	register WCHAR value = 0;
	register TCHAR ch;
	register const TCHAR *lpCursor = lpData;
	register const TCHAR *lpUpper = lpData + dwLength;

	while( lpCursor < lpUpper ) {
		ch = *lpCursor++;
		if( ch >= _T('a') ) {
			ATLASSERT( ch >= _T('a') && ch <= _T('f') );
			ch -= _T('a') - 10;
		} else if( ch >= _T('A') ) {
			ATLASSERT( ch >= _T('A') && ch <= _T('F') );
			ch -= _T('A') - 10;
		} else {
			ATLASSERT( ch >= _T('0') && ch <= _T('9') );
			ch -= _T('0');
		}
		value = (value * 16) + ch;
	}

	return value;
}

INT CJXmlTextReader::decodeEntityReference( LPCTSTR lpData, UINT nLength, TCHAR *lpBuffer )
{
	// make sure it is entity reference
	ATLASSERT( lpData[0] == _T('&') );
	ATLASSERT( lpData[nLength-1] == _T(';') );

	const TCHAR *lpCursor = lpData + 1;
	const TCHAR *lpBound = lpData + nLength - 1;
	TCHAR ch;
	WCHAR chEntity = 0;

	ch = *lpCursor++;
	switch( ch ) {
		case _T('l'):	// &lt;
			if( *lpCursor == _T('t') ) {
				chEntity = L'<';
			}
			break;

		case _T('g'):	// &gt;
			if( *lpCursor == _T('t') ) {
				chEntity = L'>';
			}
			break;

		case _T('a'):	// &amp; &apos;
			if( isSameString( _T("mp"), 2, lpCursor, lpBound - lpCursor ) ) {
				chEntity = L'&';
			} else if( isSameString( _T("pos"), 3, lpCursor, lpBound - lpCursor ) ) {
				chEntity = L'\'';
			}
			break;

		case _T('q'):
			if( isSameString( _T("uot"), 3, lpCursor, lpBound - lpCursor ) ) {
				chEntity = L'\"';
			}
			break;

		case _T('#'):
			if( *lpCursor == _T('x') || *lpCursor == _T('X') ) {
				// hex input
				lpCursor++;
				chEntity = toHex( lpCursor, lpBound - lpCursor );
			} else {
				// decimal input
				chEntity = toDecimal( lpCursor, lpBound - lpCursor );
			}
			break;
	}

	if( chEntity ) {

		// process valid entity
		WCHAR szOutBuf[2];
		szOutBuf[0] = chEntity;
		szOutBuf[1] = 0;

		USES_CONVERSION;
		_tcscpy( lpBuffer, W2T(szOutBuf) );

		return _tcslen(lpBuffer);
	}

	return 0;
}

#define ERWriteOut( lpData, nDataLen ) \
{ \
	nCopyLen = min( nOutRemain , nDataLen ); \
	MoveMemory( lpOutCursor, lpData, sizeof(TCHAR) * nCopyLen ); \
	lpOutCursor += nCopyLen; \
	nOutRemain -= nCopyLen; \
}

void CJXmlTextReader::copyEntityReferenceString( LPCTSTR lpData, INT nDataLen, TCHAR *lpBuffer, INT nLength )
{
	UINT nCopyLen;

	// search begin of entity reference
	INT nPos = PositionOfChar( _T('&'), lpData, nDataLen );

	// no begin of entity reference
	if( nPos == -1 ) {
		nCopyLen = min( nLength - 1, nDataLen );
		_tcsncpy( lpBuffer, lpData, nCopyLen );
		lpBuffer[nCopyLen] = _T('\0');
		return;
	}

	register const TCHAR *lpCursor = lpData;
	register const TCHAR *lpBound = lpData + nDataLen;
	register TCHAR *lpOutCursor = lpBuffer;
	INT nOutRemain = nLength - 1;

	do {
		INT nLen;

		// write out characters before entity reference
		ERWriteOut( lpCursor, nPos );
		lpCursor += nPos;

		// search entity reference end
		nLen = lpBound - lpCursor;
		nPos = PositionOfChar( _T(';'), lpCursor, nLen );
		if( nPos == -1 ) {
			// no entity reference end, then copy all remain string to output buffer
			ERWriteOut( lpCursor, nLen );
			break;
		}

		// decode entity reference
		TCHAR lpEntity[3];
		nLen = nPos + 1;
		INT nEntitynLen = decodeEntityReference( lpCursor, nLen, lpEntity );
		if( nEntitynLen ) {

			// it is valid entity reference, then copy entity value
			ERWriteOut( lpEntity, nEntitynLen );
			lpCursor += nLen;
		} else {

			// it is invalid enitity reference, then copy raw entity data into output buffer
			ERWriteOut( lpCursor, nLen );
			lpCursor += nLen;
		}

		// search begin of entity reference
		nLen = lpBound - lpCursor;
		nPos = PositionOfChar( _T('&'), lpCursor, nLen );
		if( nPos == -1 ) {
			// no more entity reference, then write out all remain string.
			ERWriteOut( lpCursor, nLen );
			break;
		}
	} while( (lpCursor < lpBound) && (nOutRemain > 0) );

	// terminate it
	*lpOutCursor = _T('\0');
}
