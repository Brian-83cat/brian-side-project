#if !defined(_JXMLTEXTREADER_INCLUDED_)
#define _JXMLTEXTREADER_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IJXmlTextReader.h"

class CJXmlTextReader :
	public IJXmlTextReader
{
public:
	CJXmlTextReader(void);
	~CJXmlTextReader(void);

	HRESULT loadFile( LPCTSTR lpszFilename, BOOL bValidate );
	// to load specifid file

	HRESULT loadXML( LPCTSTR lpszXmlString, int nLength, BOOL bValidate );
	// to load specified XML string data

	void rewind( void );
	// rewind reader pointer to header of stream

	BOOL isEmptyElement( void );
	// Tests if the current content node is a <.../> empty element

	BOOL isStartElement( LPCTSTR lpszName, INT nLength );
	// Tests if the current content node is a start tag.

	BOOL isStartElement( void );
	// Tests if the current content node is a start tag.

	BOOL hasAttributes( void );
	// Gets a value indicating whether the current node has any attributes.
	// true if the current node has attributes; otherwise, false.

	BOOL moveToNextAttribute( void );
	// Moves to the next attribute.
	// true if there is a next attribute; false if there are no more attributes.

	BOOL moveToAttribute( LPCTSTR lpszName );
	// move to specified attribute
	// true if there is a next attribute; false if there are no more attributes.

	void moveToElement( void );
	// move cursor from attribute to element

	BOOL read( void );
	// Reads the next node from the stream.
	// true if the next node was read successfully; false if there are no more nodes to read.

	void skip( void );
	// Skips the children of the current node.

	BOOL readString( TCHAR *lpBuffer, UINT nLength );
	// reads the contents of an element or text node as a string.

	BOOL readElementString( TCHAR *lpBuffer, UINT nLength );
	// to read element string

	BOOL readOuterXml( TCHAR *lpBuffer, UINT nLength );
	// reads the content, including markup, representing this node and all its children.

	BOOL readInnerXml( TCHAR *lpBuffer, UINT nLength );
	// reads the content, including markup, representing this node and all its children.

	void get_Name( TCHAR *lpBuffer, UINT nLength );
	// Gets the qualified name of the current node.

	void get_Value( TCHAR *lpBuffer, UINT nLength );
	// Gets the text value of the current node.

protected:
	enum EMaxBounds {
		LEN_TagName			= 64,
		SIZE_ElementStack	= 32,
		SIZE_StateStack		= 4
	};

	enum EXmlNodeType {
		NODE_Element,
		NODE_EndElement,
		NODE_Text,
		NODE_Attribute,
		NODE_ProcessingInstruction,
		NODE_Comment,
		NODE_Whitespace
	};
	// node type constant

	enum EReaderState {
		READERSTATE_EOF = 1,
		READERSTATE_Ready = 0,
		READERSTATE_ElementNameNotMatch = -1,
		READERSTATE_TooEarlyEOF = -2
	};

	struct TNode {
		// raw token property
		INT		nNodeType;
		UINT	nBegin;
		UINT	nLength;
		BOOL	bIsEmpty;

		UINT	nNameBegin;
		UINT	nNameLen;
		BOOL	bHasAttribute;

		UINT	nValueBegin;
		UINT	nValueLen;
		UINT	nNextChar;
	};

	BOOL scanAttribute( void );
	BOOL scanToken( TNode *lpToken );
	BOOL scanRawToken( TNode *lpToken );

	BOOL isElementNameMatch( TNode *lpElement1, TNode *lpElement2 );
	static INT PositionOfSpace( LPCTSTR lpData, INT nLength );
	static INT PositionOfNonSpace( LPCTSTR lpData, INT nLength );
	static INT PositionOfTailingSpace( LPCTSTR lpData, INT nLength );
	static INT PositionOfEndTag( LPCTSTR lpData, INT nLength );
	static INT PositionOfChar( TCHAR ch, LPCTSTR lpData, INT nLength );
	static BOOL isSameString( LPCTSTR lpStr1, INT nStr1Len,  LPCTSTR lpStr2, INT nStr2Len );
	static BOOL isLeadWith( LPCTSTR lpLead, INT nLeadLen, LPCTSTR lpData, INT nLength );
	static BOOL isUnicodeString( LPBYTE lpData, DWORD dwLength );
	static BOOL isSpaceChar( TCHAR ch );
	static WCHAR toDecimal( LPCTSTR lpData, DWORD dwLength );
	static WCHAR toHex( LPCTSTR lpData, DWORD dwLength );
	static INT decodeEntityReference( LPCTSTR lpData, UINT nLength, TCHAR *lpBuffer );
	void copyEntityReferenceString( LPCTSTR lpData, INT nDataLen, TCHAR *lpBuffer, INT nLength );

private:

	TNode *m_lpElementStack;
	DWORD m_dwStackTop;
	TNode *m_pCurrent;
	LONG m_nReaderState;

	TCHAR *m_lpXmlStream;
	UINT m_nXmlStreamSize;
	LPBYTE m_lpDataBuf;
	UINT m_nStreamCursor;
};

#endif // _JXMLTEXTREADER_INCLUDED_
