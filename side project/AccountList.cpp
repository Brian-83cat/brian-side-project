#include <stdio.h>
#include <afx.h>

#include "SafeMemory.h"
#include "TypeDefine.h"

#include "AccountList.h"

CAccountList::CAccountList( void )
// Constructor
{
	m_pHeadAccount = NULL;
	m_pTailAccount = NULL;
}

CAccountList::~CAccountList( void )
// Destructor
{
	// Delete linked list
	DeleteList();
}

void CAccountList::NewAccount( TAccount* pAccount )
// New Account at the end of list.
{
	// New first account ( Head is Tail )
	if( m_pHeadAccount == NULL ) {
		SAFENEW( m_pHeadAccount, TAccount );
		m_pHeadAccount->pNextAccount = NULL;
		m_pTailAccount = m_pHeadAccount;
	}
	// Add new Account at the end of list.
	else {
		SAFENEW( m_pTailAccount->pNextAccount, TAccount ); 
		m_pTailAccount = m_pTailAccount->pNextAccount;
	}
	m_pTailAccount->pNextAccount = NULL;
	// Set new account data
	SetNewAccountData( pAccount );
}

TAccount* CAccountList::SearchAccount( CHAR *pInputID )
// Search account from the account list.
{
	if( pInputID == NULL ) {
		return NULL;
	}

	TAccount* TempAccount = NULL;
	TempAccount = m_pHeadAccount;

	// Search from the head account.
	while( TempAccount != NULL ) {
		if( strcmp( pInputID, TempAccount->szAccountID ) == 0 ) {
			return TempAccount;
		}
		TempAccount = TempAccount->pNextAccount;
	}
	return NULL;
}

TAccount* CAccountList::GetAllAccount( void )
// Get all account from the list
{
	return m_pHeadAccount;
}

void CAccountList::DeleteList( void )
// Delete the list.
{
	TAccount *pTemp = m_pHeadAccount;

	while( pTemp != NULL ) {
		m_pHeadAccount = m_pHeadAccount->pNextAccount;
		SAFEDELETE( pTemp );
		pTemp = m_pHeadAccount;
	}

	m_pHeadAccount = NULL;
	m_pTailAccount = NULL;
}

BOOL CAccountList::SetNewAccountData( TAccount* pAccount )
// Set account data at new tail account.
{
	if( m_pTailAccount == NULL || pAccount == NULL ) {
		return FALSE;
	}
	strcpy( m_pTailAccount->szAccountID, pAccount->szAccountID );
	strcpy( m_pTailAccount->szAccountPassword, pAccount->szAccountPassword );
	m_pTailAccount->nBalance[ EMK_NTD ] = pAccount->nBalance[ EMK_NTD ];
	m_pTailAccount->nBalance[ EMK_US ] = pAccount->nBalance[ EMK_US ];
	m_pTailAccount->nRecordNum = pAccount->nRecordNum;

	for( INT i = 0; i < m_pTailAccount->nRecordNum; i++ ) {
		strncpy( m_pTailAccount->Record[ i ].szOperation, pAccount->Record[ i ].szOperation, sizeof( pAccount->Record[ i ].szOperation ) );
		strncpy( m_pTailAccount->Record[ i ].szSlaveAccountID, pAccount->Record[ i ].szSlaveAccountID, sizeof( pAccount->Record[ i ].szSlaveAccountID ) );
		strncpy( m_pTailAccount->Record[ i ].szMoneyKind, pAccount->Record[ i ].szMoneyKind, sizeof( pAccount->Record[ i ].szMoneyKind ) );
		m_pTailAccount->Record[ i ].nTransferMoney = pAccount->Record[ i ].nTransferMoney;
		m_pTailAccount->Record[ i ].nCurrentBalance = pAccount->Record[ i ].nCurrentBalance;
	}
	return TRUE;
}
