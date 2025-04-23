#include <stdio.h>
#include <afx.h>
#include <math.h>

#include "SafeMemory.h"
#include "TypeDefine.h"
#include "UI.h"

#include "IBank.h"
#include "Bank.h"
#include "BankManager.h"

CBankManager::CBankManager( CBank* pBank[ EBN_Num ] )
// Constructor
{
	for( INT i = 0; i < EBN_Num; i++ ) {
		m_pBank[ i ] = pBank[ i ];
	}
}

CBankManager::~CBankManager( void )
// Destructor
{
	for( INT i = 0; i < EBN_Num; i++ ) {
		m_pBank[ i ] = NULL;
	}
}

BOOL CBankManager::GetOPBank( IBank **pBank, INT nBankIndex )
// Get operation bank
{
	if( nBankIndex >= EBN_Num || nBankIndex < 0 ) {
		return FALSE;
	}

	if( m_pBank[ nBankIndex ] == NULL ) {
		return FALSE;
	}
	*pBank = ( IBank* )m_pBank[ nBankIndex ];
	return TRUE;
}

ELogInMsg CBankManager::BankTransfer( TTransferData* pBankTransferData, INT &nMaxTransferMoney )
// Bank transfer function
{
	DOUBLE HandlingFee = 0.0;
	INT nRealMoney = 0;
	INT nMoney = pBankTransferData->nMoney;

	// Check function input
	if( m_pBank == NULL || pBankTransferData->szTransferInID == NULL ) {
		return ELIM_BANKTRANSFER_FAIL;
	}

	// Check account balance whether zero
	if( m_pBank[ pBankTransferData->nLogInBankIdx ]->GetBalance( pBankTransferData->nMoneyKind ) == 0 ) {
		return ELIM_BANKTRANSFER_MONEY_FAIL;
	}

	// The bank user log in is same as transfer bank.
	if( pBankTransferData->nLogInBankIdx == pBankTransferData->nSelectBankIdx ) {
		return ELIM_BANKTRANSFER_BANK_FAIL;
	}

	// The select account ID is not correct
	if( m_pBank[ pBankTransferData->nSelectBankIdx ]->CheckAccountID( pBankTransferData->szTransferInID ) == FALSE ) {
		return ELIM_BANKTRANSFER_SELECTID_FAIL;
	}

	// Get bank rate from bank
	HandlingFee = m_pBank[ pBankTransferData->nLogInBankIdx ]->GetHandlingFee( pBankTransferData->nSelectBankIdx );
	// Calculate real transfer money
	nRealMoney = ( INT )ceil( ( DOUBLE )nMoney * ( 1.0 + HandlingFee ) );

	// Invalid transfer money and calculate max transfer money
	if( nMoney <= 0 || nRealMoney > m_pBank[ pBankTransferData->nLogInBankIdx ]->GetBalance( pBankTransferData->nMoneyKind ) ) {
		nMaxTransferMoney = ( INT )floor( ( DOUBLE )( m_pBank[ pBankTransferData->nLogInBankIdx ]->GetBalance( pBankTransferData->nMoneyKind ) ) / ( 1.0 + HandlingFee ) );
		return ELIM_BANKTRANSFER_MONEY_INCORRECT;
	}

	pBankTransferData->nTransferOutMoney = nRealMoney;

	// Do Bank transfer
	BOOL bBankTransferOutSuccess = m_pBank[ pBankTransferData->nLogInBankIdx ]->BankTransferOut( pBankTransferData );
	BOOL bBankTransferInSuccess = m_pBank[ pBankTransferData->nSelectBankIdx ]->BankTransferIn( pBankTransferData );

	// Bank transfer success
	if( bBankTransferOutSuccess == TRUE && bBankTransferInSuccess == TRUE ) {
		return ELIM_BANKTRANSFER_SUCCESS;
	}
	return ELIM_BANKTRANSFER_FAIL;
}
