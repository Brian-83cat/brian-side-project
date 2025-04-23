#if !defined(_IJXMLTEXTREADER_INCLUDED_)
#define _IJXMLTEXTREADER_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class IJXmlTextReader
{
public:

	virtual HRESULT loadFile( LPCTSTR lpszFilename, BOOL bValidate ) = 0;
	// to load specifid file

	virtual HRESULT loadXML( LPCTSTR lpszXmlString, int nLength, BOOL bValidate ) = 0;
	// to load specified XML string data

	virtual void rewind( void ) = 0;
	// rewind reader pointer to header of stream

	virtual BOOL isEmptyElement( void ) = 0;
	// Tests if the current content node is a <.../> empty element

	virtual BOOL isStartElement( LPCTSTR lpszName, INT nLength ) = 0;
	// Tests if the current content node is a start tag.

	virtual BOOL isStartElement( void ) = 0;
	// Tests if the current content node is a start tag.

	virtual BOOL hasAttributes( void ) = 0;
	// Gets a value indicating whether the current node has any attributes.
	// true if the current node has attributes; otherwise, false.

	virtual BOOL moveToNextAttribute( void ) = 0;
	// Moves to the next attribute.
	// true if there is a next attribute; false if there are no more attributes.

	virtual BOOL moveToAttribute( LPCTSTR lpszName ) = 0;
	// move to specified attribute
	// true if there is a next attribute; false if there are no more attributes.

	virtual void moveToElement( void ) = 0;
	// move cursor from attribute to element

	virtual BOOL read( void ) = 0;
	// Reads the next node from the stream.
	// true if the next node was read successfully; false if there are no more nodes to read.

	virtual void skip( void ) = 0;
	// Skips the children of the current node.

	virtual BOOL readString( TCHAR *lpBuffer, UINT nLength ) = 0;
	// reads the contents of an element or text node as a string.

	virtual BOOL readElementString( TCHAR *lpBuffer, UINT nLength ) = 0;
	// to read element string

	virtual BOOL readOuterXml( TCHAR *lpBuffer, UINT nLength ) = 0;
	// reads the content, including markup, representing this node and all its children.

	virtual BOOL readInnerXml( TCHAR *lpBuffer, UINT nLength ) = 0;
	// reads the content, including markup, representing this node and all its children.

	virtual void get_Name( TCHAR *lpBuffer, UINT nLength ) = 0;
	// Gets the qualified name of the current node.

	virtual void get_Value( TCHAR *lpBuffer, UINT nLength ) = 0;
	// Gets the text value of the current node.
};

#endif // _IJXMLTEXTREADER_INCLUDED_