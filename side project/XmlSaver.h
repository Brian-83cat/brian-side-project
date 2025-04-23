#ifndef _XMLEXPORTER_H_
#define _XMLEXPORTER_H_

class CAccountList;

class CXmlSaver
{
public:
	CXmlSaver( void );
	// Constructor

	~CXmlSaver( void );
	// Destructor

	BOOL SaveXmlFile( CHAR *pFileName, CAccountList *pList );
	// Export to xml file

private:
	CJXmlTextWriter m_XmlWriter;
	// Xml writer

	void SaveList( CAccountList* pList );
	// save list

	void SaveAccount( TAccount* pAccount );
	// save account

	void SaveBalance( TAccount* pAccount );
	// save balance

	void SaveRecord( TAccount* pAccount, INT &nIndex );
	// save Record
};

#endif
