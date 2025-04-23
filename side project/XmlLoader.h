#ifndef _XMLLOADER_H_
#define _XMLLOADER_H_

class CAccountList;

class CXmlLoader
{
public :
	CXmlLoader( void );
	// Constructor

	~CXmlLoader( void );
	// Destructor

	BOOL LoadXmlFile( CHAR FileName[], CAccountList* pList );
	// Load xml file

private:
	CJXmlTextReader m_XmlReader;
	// Xml reader

	BOOL LoadList( CAccountList* pList );
	// Load list

	BOOL LoadAccount( CAccountList* pList );
	// Load account

	BOOL LoadBalance( TAccount* pAccount );
	// Load balance

	BOOL LoadRecord( TAccount* pAccount, INT &nIndex );
	// Load Record

	BOOL IsStartElement( LPCTSTR Buffer );
	// Check element whether corrent
};

#endif
