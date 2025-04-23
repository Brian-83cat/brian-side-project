#include "stdafx.h"

#include "TypeDefine.h"

#include "AccountList.h"
#include "JXmlTextWriter.h"
#include "XmlSaver.h"

CXmlSaver::CXmlSaver( void ):m_XmlWriter( 2048 )
// Constructor
{
	// Setting
	m_XmlWriter.put_SaveToFile( TRUE );
	m_XmlWriter.put_Indented( TRUE, EIT_Tab, 1 );
}

CXmlSaver::~CXmlSaver( void )
// Destructor
{
}

BOOL CXmlSaver::SaveXmlFile( CHAR *pFileName, CAccountList *pList )
// Export to xml file
{
	if( pList == NULL ) {
		return FALSE;
	}

	// Put file name
	m_XmlWriter.put_FileName( pFileName );
	// Start document
	m_XmlWriter.WriteStartDocument( TRUE, FALSE );
	// Start element of AccountIList
	m_XmlWriter.WriteStartElement( "AccountList" );

	// Save account list
	SaveList( pList );

	// End element of AccountList
	m_XmlWriter.WriteEndElement();
	// End document
	m_XmlWriter.WriteEndDocument();

	return TRUE;
}

void CXmlSaver::SaveList( CAccountList *pList )
// Save list
{
	TAccount *pTempAccount;
	pTempAccount = pList->GetAllAccount();

	while( pTempAccount != NULL ) {
		// Start element of Account
		m_XmlWriter.WriteStartElement( "Account" );

		// Save account
		SaveAccount( pTempAccount );

		// End element of Account.
		m_XmlWriter.WriteEndElement();

		pTempAccount = pTempAccount->pNextAccount;
	}
}

void CXmlSaver::SaveAccount( TAccount *pAccount )
// Save account
{
	CHAR szBufffer[ DATA_LIMIT ] = {'\0'};

	// Write ID
	m_XmlWriter.WriteElementString( "ID", pAccount->szAccountID );
	// Write password
	m_XmlWriter.WriteElementString( "Password", pAccount->szAccountPassword );

	// Start element of Balance
	m_XmlWriter.WriteStartElement( "Balance" );
	// save balance
	SaveBalance( pAccount );
	// End element of Balance
	m_XmlWriter.WriteEndElement();

	// Write record number
	_ltoa( pAccount->nRecordNum, szBufffer, 10 );
	m_XmlWriter.WriteElementString( "RecordNum", szBufffer );

	// Write transaction record
	for( INT nIndex = 0; nIndex < pAccount->nRecordNum; nIndex++ ) {
		// Start element of Transaction record 
		m_XmlWriter.WriteStartElement( "TransactionRecord" );
		// Save transaction record
		SaveRecord( pAccount, nIndex );
		// End element of Transaction record.
		m_XmlWriter.WriteEndElement();
	}
}

void CXmlSaver::SaveBalance( TAccount *pAccount )
// Save balance
{
	CHAR szBufffer[ DATA_LIMIT ] = {'\0'};

	// Integer to char array. Write NTD balance
	_ltoa( pAccount->nBalance[ EMK_NTD ], szBufffer, 10 );
	m_XmlWriter.WriteElementString( "NTD", szBufffer );

	// Integer to char array. Write NUS balance
	_ltoa( pAccount->nBalance[ EMK_US ], szBufffer, 10 );
	m_XmlWriter.WriteElementString( "US", szBufffer );
}

void CXmlSaver::SaveRecord( TAccount *pAccount, INT &nIndex )
// Save record
{
	CHAR szBufffer[ DATA_LIMIT ] = {'\0'};

	// Write operation
	m_XmlWriter.WriteElementString( "Operation", pAccount->Record[ nIndex ].szOperation );
	// Write slave bank name
	m_XmlWriter.WriteElementString( "SlaveBankName", pAccount->Record[ nIndex ].szSlaveBankName );
	// Write slave accountID
	m_XmlWriter.WriteElementString( "SlaveAccountID", pAccount->Record[ nIndex ].szSlaveAccountID );
	// Write money kind
	m_XmlWriter.WriteElementString( "MoneyKind", pAccount->Record[ nIndex ].szMoneyKind );
	// Write transfer money
	_ltoa( pAccount->Record[ nIndex ].nTransferMoney, szBufffer, 10 );
	m_XmlWriter.WriteElementString( "TransferMoney", szBufffer );
	// Write current balance
	_ltoa( pAccount->Record[ nIndex ].nCurrentBalance, szBufffer, 10 );
	m_XmlWriter.WriteElementString( "CurrentBalance", szBufffer );
}
