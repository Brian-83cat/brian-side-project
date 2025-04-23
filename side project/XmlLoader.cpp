#include "stdafx.h"

#include "TypeDefine.h"

#include "AccountList.h"
#include "JXmlTextReader.h"
#include "XmlLoader.h"

CXmlLoader::CXmlLoader( void )
// Constructor
{
}

CXmlLoader::~CXmlLoader( void )
// Destructor
{
}

BOOL CXmlLoader::LoadXmlFile( CHAR* pFileName, CAccountList* pList )
// Load xml file
{
	if( pList ==NULL ) {
		return FALSE;
	}

	HRESULT hr;
	hr = m_XmlReader.loadFile( pFileName, TRUE );

	// Load fail
	if( FAILED( hr ) ) {
		return FALSE;
	}

	// Enter
	m_XmlReader.read();

	// Load account list
	if( IsStartElement( "AccountList" ) ) {
		LoadList( pList );
	}

	return TRUE;
}

BOOL CXmlLoader::LoadList( CAccountList* pList )
// Load list
{
	ASSERT( pList != NULL );
	CHAR szBuffer[ DATA_LIMIT ];

	// Enter
	m_XmlReader.read();

	while( m_XmlReader.isStartElement() ) {

		// Get name
		m_XmlReader.get_Name( szBuffer, DATA_LIMIT );

		// Load account
		if( strcmp( "Account", szBuffer ) == 0 ) {
			if( LoadAccount( pList ) == FALSE ) {
				return FALSE;
			}
		}

		// Skip
		m_XmlReader.skip();
	}
	return TRUE;
}

BOOL CXmlLoader::LoadAccount( CAccountList* pList )
// Load account
{
	ASSERT( pList != NULL );
	CHAR szBuffer[ DATA_LIMIT ];
	TAccount TempAccount;
	INT nIndex = 0;

	// Enter
	m_XmlReader.read();

	while( m_XmlReader.isStartElement() ) {

		// Get name
		m_XmlReader.get_Name( szBuffer, DATA_LIMIT );

		// ID element
		if( strcmp( "ID", szBuffer ) == 0 ) {
			m_XmlReader.readString( szBuffer, DATA_LIMIT );
			strncpy( TempAccount.szAccountID, szBuffer, sizeof( szBuffer ) );
		}
		// Password element
		else if( strcmp( "Password", szBuffer ) == 0 ) {
			m_XmlReader.readString( szBuffer, DATA_LIMIT );
			strncpy( TempAccount.szAccountPassword, szBuffer, sizeof( szBuffer ) );
		}
		// Balance element
		else if( strcmp( "Balance", szBuffer ) == 0 ) {
			if( LoadBalance( &TempAccount ) == FALSE ) {
				return FALSE;
			}
		}
		// Record number element
		else if( strcmp( "RecordNum", szBuffer ) == 0 ) {
			m_XmlReader.readString( szBuffer, DATA_LIMIT );
			TempAccount.nRecordNum = atoi( szBuffer );
		}
		// Load transaction record
		else if( strcmp( "TransactionRecord", szBuffer ) == 0 ) {
			if( LoadRecord( &TempAccount, nIndex ) == FALSE ) {
				return FALSE;
			}
		}

		// Skip
		m_XmlReader.skip();

	}
	pList->NewAccount( &TempAccount );
	return TRUE;
}

BOOL CXmlLoader::LoadBalance( TAccount* pAccount )
// Load balance
{
	ASSERT( pAccount != NULL );
	CHAR szBuffer[ DATA_LIMIT ];

	// Enter
	m_XmlReader.read();

	while( m_XmlReader.isStartElement() ) {

		m_XmlReader.get_Name( szBuffer, DATA_LIMIT );

		// NTD element
		if( strcmp( "NTD", szBuffer ) == 0 ) {
			m_XmlReader.readString( szBuffer, DATA_LIMIT );
			pAccount->nBalance[ EMK_NTD ] = atoi( szBuffer );
		}
		// US element
		else if( strcmp( "US", szBuffer ) == 0 ) {
			m_XmlReader.readString( szBuffer, DATA_LIMIT );
			pAccount->nBalance[ EMK_US ] = atoi( szBuffer );
		}

		// Skip
		m_XmlReader.skip();
	}
	return TRUE;
}

BOOL CXmlLoader::LoadRecord( TAccount* pAccount, INT &nIndex )
// Load transaction record
{
	ASSERT( pAccount != NULL );
	CHAR szBuffer[ DATA_LIMIT ];

	// Enter
	m_XmlReader.read();

	while( m_XmlReader.isStartElement() ) {

		m_XmlReader.get_Name( szBuffer, DATA_LIMIT );

		// Operation element
		if( strcmp( "Operation", szBuffer ) == 0 ) {
			m_XmlReader.readString( szBuffer, DATA_LIMIT );
			strncpy( pAccount->Record[ nIndex ].szOperation, szBuffer, sizeof( szBuffer ) );
		}
		// Slave bank name element
		else if( strcmp( "SlaveBankName", szBuffer ) == 0 ) {
			m_XmlReader.readString( szBuffer, DATA_LIMIT );
			strncpy( pAccount->Record[ nIndex ].szSlaveBankName, szBuffer, sizeof( szBuffer ) );
		}
		// Slave accountID element
		else if( strcmp( "SlaveAccountID", szBuffer ) == 0 ) {
			m_XmlReader.readString( szBuffer, DATA_LIMIT );
			strncpy( pAccount->Record[ nIndex ].szSlaveAccountID, szBuffer, sizeof( szBuffer ) );
		}
		// Money kind element
		else if( strcmp( "MoneyKind", szBuffer ) == 0 ) {
			m_XmlReader.readString( szBuffer, DATA_LIMIT );
			strncpy( pAccount->Record[ nIndex ].szMoneyKind, szBuffer, sizeof( szBuffer ) );
		}
		// Transfer money element
		else if( strcmp( "TransferMoney", szBuffer ) == 0 ) {
			m_XmlReader.readString( szBuffer, DATA_LIMIT );
			pAccount->Record[ nIndex ].nTransferMoney = atoi( szBuffer );
		}
		// Current balance element
		else if( strcmp( "CurrentBalance", szBuffer ) == 0 ) {
			m_XmlReader.readString( szBuffer, DATA_LIMIT );
			pAccount->Record[ nIndex ].nCurrentBalance = atoi( szBuffer );
		}

		// Skip
		m_XmlReader.skip();
	}
	nIndex++;
	return TRUE;
}

BOOL CXmlLoader::IsStartElement( LPCTSTR Buffer )
// Check element whether corrent
{
	LONG nlength = ( LONG )strlen( Buffer );
	return m_XmlReader.isStartElement( Buffer, nlength );
}
