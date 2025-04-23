#include <stdio.h>
#include <afx.h>

#include "SafeMemory.h"
#include "TypeDefine.h"
#include "UI.h"

#include "AccountList.h"
#include "JXmlTextReader.h"
#include "JXmlTextWriter.h"
#include "XmlLoader.h"
#include "XmlSaver.h"
#include "IBank.h"
#include "Bank.h"

#define DataMinLength ( 4 )
#define DataMaxLength ( 6 )
#define FileNameCatLength ( 4 )
#define MaxMnoey ( 999999 )
#define Round( Number ) ( INT )( Number + 0.5 )

DOUBLE ExchangeRateTable[ EMK_Num ][ EEK_Num ] = {
	// NTD exchange rate
	{ 1.0, 1.0 },
	// US exchange rate
	{ 28.425, 28.915 }
};

CBank::CBank( TBankSettingData SettingData )
// Constructor
{
	m_pAccountList = NULL;
	m_pLogInAccount = NULL;
	m_bAdminLogIn = FALSE;

	// Copy setting data to member data
	memcpy( &m_SettingData, &SettingData, sizeof( TBankSettingData ) );

	// create a empty linked list.
	SAFENEW( m_pAccountList, CAccountList );

	// Load data
	DataLoader( m_SettingData.szBankName );
}

CBank::~CBank( void )
// Destructor
{
	// Save data
	DataSaver( m_SettingData.szBankName );

	// Delete linked list and log in account
	SAFEDELETE( m_pAccountList );

	m_pLogInAccount = NULL;
}

ECreateAccountMsg CBank::CreateAccount( TAccount* pAccount )
// Check tne account unit that user wnat to create and return status.
{
	if( pAccount == NULL ) {
		return ECAM_ACCOUNT_INVALID;
	}

	// Check account ID whether exist.
	if( CheckAccountID( pAccount->szAccountID ) == TRUE ) {
		return ECAM_ID_EXIST;
	}
	
	// Check account ID range.
	if( CheckDataRange( pAccount->szAccountID ) == FALSE ) {
		return ECAM_ID_RANGE_FAIL;
	}

	// Check account password range.
	if( CheckDataRange( pAccount->szAccountPassword ) == FALSE ) {
		return ECAM_PASSWORD_RANGE_FAIL;
	}

	// Check account NTD balance.
	if( CheckInputBalance( pAccount->nBalance[ EMK_NTD ] ) == FALSE ) {
		return ECAM_BALANCE_FAIL;
	}

	// Check account US balance.
	if( CheckInputBalance( pAccount->nBalance[ EMK_US ] ) == FALSE ) {
		return ECAM_US_BALANCE_FAIL;
	}

	// New account and set account information in it.
	m_pAccountList->NewAccount( pAccount );

	// Add create account record
	m_pLogInAccount = m_pAccountList->SearchAccount( pAccount->szAccountID );
	for( INT i = 0; i < EMK_Num; i++ ) {
		AddRecord( "Opening Deposit", NULL, NULL, i, 0, pAccount->nBalance[ i ], m_pLogInAccount );
	}
	m_pLogInAccount = NULL;

	return ECAM_CREATE_SUCCESS;
}

ELogInMsg CBank::LogInAccount( TAccount* pAccount )
// Check tne account ID and password that user wnat to log in and return status.
{
	if( pAccount == NULL ) {
		return ELIM_NOT_LOG_IN;
	}

	// Check account ID whether exist.
	if( CheckAccountID( pAccount->szAccountID ) == FALSE ) {
		return ELIM_ID_NOT_EXIST;
	}

	// Check Password whether correct.
	if( IsPasswordCorrect( pAccount->szAccountID, pAccount->szAccountPassword ) == FALSE ) {
		return ELIM_INVALID_PASSWORD;
	}

	TAccount* TempAccount = m_pAccountList->SearchAccount( pAccount->szAccountID );
	m_pLogInAccount = TempAccount;
	return ELIM_LOGIN_SUCCESS;
}

INT CBank::GetBalance( INT nMoneyKind )
// Get the balance after log in success. If not log in return 0.
{
	if( m_pLogInAccount == NULL ) {
		return 0;
	}
	return m_pLogInAccount->nBalance[ nMoneyKind ];
}

DOUBLE CBank::GetHandlingFee( INT nBankIndexSelect )
// Get bank rate after log in success. If not log in return 0.
{
	if( m_pLogInAccount == NULL ) {
		return 0.0;
	}
	return m_SettingData.HandlingFee[ nBankIndexSelect ];
}

void CBank::LogOutAccount( void )
// Log out the account
{
	m_pLogInAccount = NULL;
}

ELogInMsg CBank::Deposit( INT nMoney, INT nKind )
// Deposit money
{
	// Check user whether log in
	if( m_pLogInAccount == NULL ) {
		return ELIM_NOT_LOG_IN;
	}

	// Deposit money invalid.
	if( nMoney <= 0 || nMoney > ( MaxMnoey - m_pLogInAccount->nBalance[ nKind ] ) ) {
		return ELIM_INVALID_BALANCE;
	}

	// Deposit money
	m_pLogInAccount->nBalance[ nKind ] += nMoney;
	// Add deposit record
	AddRecord( "Deposit", NULL, NULL, nKind, nMoney, m_pLogInAccount->nBalance[ nKind ], m_pLogInAccount );
	return ELIM_DEPOSIT_SUCCESS;
}

ELogInMsg CBank::Withdraw( INT nMoney, INT nKind )
// Withdraw money
{
	// Check user whether log in
	if( m_pLogInAccount == NULL ) {
		return ELIM_NOT_LOG_IN;
	}

	// No balance in account
	if( m_pLogInAccount->nBalance[ nKind ] == 0 ) {
		return ELIM_WHITHDRAW_FAIL;
	}

	// Withdraw money invalid.
	if( nMoney <= 0 || nMoney > m_pLogInAccount->nBalance[ nKind ] ) {
		return ELIM_INVALID_BALANCE_WHITHDRAW;
	}

	// Withdraw money
	m_pLogInAccount->nBalance[ nKind ] -= nMoney;
	// Add Withdraw record
	AddRecord( "Withdraw", NULL, NULL, nKind, nMoney, m_pLogInAccount->nBalance[ nKind ], m_pLogInAccount );
	return ELIM_WHITHDRAW_SUCCESS;
}

ELogInMsg CBank::Exchange( EExchangeKind nExchangeKind, EMoneyKind nMoneyKind, INT &nExchangeMoney )
// Exchange money
{
	// Check user whether log in
	if( m_pLogInAccount == NULL ) {
		return ELIM_NOT_LOG_IN;
	}

	DOUBLE Rate;
	// Exchange rate
	Rate = ExchangeRateTable[ nMoneyKind ][ nExchangeKind ];

	switch( nExchangeKind ) {

	// Bank buy the currency from user
	case EEK_CASHBUY:

		// No balance in the account
		if( m_pLogInAccount->nBalance[ nMoneyKind ] == 0 ) {
			return ELIM_EXCHANGE_FAIL;
		}

		// Check account balance and exchange money
		if( m_pLogInAccount->nBalance[ nMoneyKind ] < nExchangeMoney || nExchangeMoney <= 0 ) {
			// Calculate how much money user can exchange.
			nExchangeMoney =  m_pLogInAccount->nBalance[ nMoneyKind ];
			return ELIM_BALANCE_NOT_ENOUGH;
		}

		// Exchange money
		m_pLogInAccount->nBalance[ EMK_NTD ] += Round( nExchangeMoney*Rate );
		m_pLogInAccount->nBalance[ nMoneyKind ] -= nExchangeMoney;
		// Exchange money record
		AddRecord( "Exchange Out", NULL, NULL, nMoneyKind, nExchangeMoney, m_pLogInAccount->nBalance[ nMoneyKind ], m_pLogInAccount );
		AddRecord( "Exchange In", NULL, NULL, EMK_NTD, Round( nExchangeMoney*Rate ), m_pLogInAccount->nBalance[ EMK_NTD ], m_pLogInAccount );
		break;

	// Bank sale the currency to user
	case EEK_CASHSALE:

		// Not enough to exchange one coin
		if( m_pLogInAccount->nBalance[ EMK_NTD ] < Rate ) {
			return ELIM_EXCHANGE_FAIL;
		}

		// Check balance enough or not and is exchange money bigger than rate
		if( m_pLogInAccount->nBalance[ EMK_NTD ] < nExchangeMoney || nExchangeMoney < Rate ) {
			nExchangeMoney = m_pLogInAccount->nBalance[ EMK_NTD ];
			return ELIM_BALANCE_NOT_ENOUGH_RATE;
		}

		// Real exchange money
		INT nRealExchangeCurrency = ( INT ) ( ( DOUBLE )nExchangeMoney / Rate );
		nExchangeMoney = Round( nRealExchangeCurrency * Rate );

		// Exchange money
		m_pLogInAccount->nBalance[ EMK_NTD ] -= nExchangeMoney;
		m_pLogInAccount->nBalance[ nMoneyKind ] += nRealExchangeCurrency ;
		// Exchange money record
		AddRecord( "Exchange Out", NULL, NULL, EMK_NTD, nExchangeMoney, m_pLogInAccount->nBalance[ EMK_NTD ], m_pLogInAccount );
		AddRecord( "Exchange In", NULL, NULL, nMoneyKind, nRealExchangeCurrency, m_pLogInAccount->nBalance[nMoneyKind ], m_pLogInAccount );
		break;
	}
	return ELIM_EXCHANGE_SUCCESS;
}

ELogInMsg CBank::Transfer( INT nMoney, INT nKind, CHAR *pSelectID )
// Transfer money
{
	// Check user whether log in
	if( m_pLogInAccount == NULL ) {
		return ELIM_NOT_LOG_IN;
	}

	// No money in the account
	if( m_pLogInAccount->nBalance[ nKind ] == 0 ) {
		return ELIM_TRANSFER_FAIL;
	}

	// Invalid select account ID
	if( strcmp( m_pLogInAccount->szAccountID, pSelectID ) == 0 ) {
		return ELIM_INVALID_SELECT_ACCOUNT;
	}

	// Find the account that user want to transfer money.
	TAccount* pTargetAccount = m_pAccountList->SearchAccount( pSelectID );

	if( pTargetAccount == NULL ) {
		return ELIM_INVALID_SELECT_ACCOUNT;
	}

	// Invalid transfer balance
	if( nMoney <= 0 || nMoney > m_pLogInAccount->nBalance[ nKind ] ) {
		return ELIM_INVALID_BALANCE_TRANSFER;
	}

	// Transfer money
	pTargetAccount->nBalance[ nKind ] += nMoney;
	m_pLogInAccount->nBalance[ nKind ] -= nMoney;
	// Transfer money record
	AddRecord( "Transfer Out", NULL, pSelectID, nKind, nMoney, m_pLogInAccount->nBalance[ nKind ], m_pLogInAccount );
	AddRecord( "Transfer In", NULL, m_pLogInAccount->szAccountID, nKind, nMoney, pTargetAccount->nBalance[ nKind ], pTargetAccount );

	return ELIM_TRANSFER_SUCCESS;
}

TTransactionRecord* CBank::GetTransactionRecord( INT &nNum )
// Get transaction record
{
	if( m_pLogInAccount == NULL ) {
		return NULL;
	}
	nNum = m_pLogInAccount->nRecordNum;
	return m_pLogInAccount->Record;
}

BOOL CBank::BankTransferOut( TTransferData* pBankTransferData )
// Bank transfer out
{
	if( pBankTransferData == NULL ) {
		return FALSE;
	}

	INT nMoneyKind = pBankTransferData->nMoneyKind;
	INT nMoney = pBankTransferData->nTransferOutMoney;

	// Check user whether log in
	if( m_pLogInAccount == NULL ) {
		return FALSE;
	}

	// Do Bank transfer out
	m_pLogInAccount->nBalance[ nMoneyKind ] -= nMoney;
	// Add transfer out record
	AddRecord( "BankTransfer Out", BankName[ pBankTransferData->nSelectBankIdx ], pBankTransferData->szTransferInID, nMoneyKind, nMoney, m_pLogInAccount->nBalance[ nMoneyKind ], m_pLogInAccount );

	return TRUE;
}

BOOL CBank::BankTransferIn( TTransferData* pBankTransferData )
// Bank transfer out
{
	if( pBankTransferData == NULL ) {
		return FALSE;
	}

	INT nMoneyKind = pBankTransferData->nMoneyKind;
	INT nMoney = pBankTransferData->nMoney;

	m_pLogInAccount = m_pAccountList->SearchAccount( pBankTransferData->szTransferInID );

	// Check user whether find select account
	if( m_pLogInAccount == NULL ) {
		return FALSE;
	}

	// Do bank transfer in
	m_pLogInAccount->nBalance[ nMoneyKind ] += nMoney;
	// Add transfer in record
	AddRecord( "BankTransfer In", BankName[ pBankTransferData->nLogInBankIdx ], pBankTransferData->szTransferOutID, nMoneyKind, nMoney, m_pLogInAccount->nBalance[ nMoneyKind ], m_pLogInAccount );
	m_pLogInAccount = NULL;

	return TRUE;
}

BOOL CBank::CheckAccountID( CHAR *pInputID )
// Check account ID in the list. If find it, assign it be the current account
{
	ASSERT( pInputID != NULL );

	if( ( m_pAccountList->SearchAccount( pInputID ) ) == NULL ) {
		return FALSE;
	}
	return TRUE;
}

BOOL CBank::AdminLogIn( TAdminData &AdminData )
{
	if( IsAdminCorrect( AdminData ) == FALSE ) {
		return FALSE;
	}
	m_bAdminLogIn = TRUE;
	return TRUE;
}

EAdminMsg CBank::GetAllAccountInfo( TAdminData &AdminData, TAccount* pAllAccount, INT* pTotalMoney )
// Get all accounts if admin log in
{
	if( m_bAdminLogIn == FALSE ) {
		return EAM_NO_LOGIN;
	}

	if( IsAdminCorrect( AdminData ) == FALSE ) {
		return EAM_ADMIN_FAIL;
	}

	if( m_pAccountList->GetAllAccount() == NULL ) {
		return EAM_ADMIN_NOACCOUNT;
	}

	*pAllAccount = *( m_pAccountList->GetAllAccount() );
	TotalMoney( pAllAccount, pTotalMoney, EMK_Num );

	return EAM_ADMIN_SUCCESS;
}

BOOL CBank::CheckDataRange( CHAR *pInputData )
// Check ID and password whether in correct range.
{
	ASSERT( pInputData != NULL );

	if( strlen( pInputData ) < DataMinLength || strlen( pInputData ) > DataMaxLength ) {
		return FALSE;
	}
	return TRUE;
}

BOOL CBank::CheckInputBalance( INT nInputBalance )
// Check balance whether 0 to 999999
{
	if( nInputBalance < 0 || nInputBalance > 999999 ) {
		return FALSE;
	}
	return TRUE;
}

BOOL CBank::IsPasswordCorrect( CHAR *pInputID, CHAR *pInputPassword )
// Check Password whether correct.
{
	ASSERT( pInputID != NULL && pInputPassword != NULL );

	TAccount* TempAccount;
	TempAccount = m_pAccountList->SearchAccount( pInputID );

	if( strcmp( TempAccount->szAccountPassword, pInputPassword ) == 0 ) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}

BOOL CBank::AddRecord( CHAR *pOperation, CHAR *pSlaveBankName, CHAR *pSlaveAccountID, INT nKind, INT nTransferMoney, INT nCurrentBalance, TAccount* pAccount )
// Add record
{
	ASSERT( pOperation != NULL );
	ASSERT( m_pLogInAccount != NULL );
	ASSERT( pAccount != NULL );

	if( m_pLogInAccount->nRecordNum >= RECORD_LIMIT ) {
		return FALSE;
	}

	// Add record
	strcpy( pAccount->Record[ m_pLogInAccount->nRecordNum ].szMoneyKind, MoneyKindList[ nKind ] );
	pAccount->Record[ m_pLogInAccount->nRecordNum ].nTransferMoney = nTransferMoney;
	pAccount->Record[ m_pLogInAccount->nRecordNum ].nCurrentBalance = nCurrentBalance;
	strcpy( pAccount->Record[ m_pLogInAccount->nRecordNum ].szOperation, pOperation );
	if( pSlaveAccountID != NULL ) {
		strcpy( pAccount->Record[ m_pLogInAccount->nRecordNum ].szSlaveAccountID, pSlaveAccountID );
	}
	if( pSlaveBankName != NULL ) {
		strcpy( pAccount->Record[ m_pLogInAccount->nRecordNum ].szSlaveBankName, pSlaveBankName );
	}

	pAccount->nRecordNum++;

	return TRUE;
}

BOOL CBank::DataLoader( CHAR *pBankName )
// Loader data from data base.
{
	if( pBankName == NULL ) {
		return FALSE;
	}
	ASSERT( strlen( pBankName ) < DATA_LIMIT - FileNameCatLength );

	BOOL bIsLoaderSuccess = FALSE;
	CXmlLoader Loader;

	CHAR szFileName[ DATA_LIMIT ] = { '\0' };
	strcpy( szFileName, pBankName );
	strcat( szFileName, ".xml" );
	// Load data to the list
	bIsLoaderSuccess = Loader.LoadXmlFile( szFileName, m_pAccountList );

	return bIsLoaderSuccess;
}

BOOL CBank::DataSaver( CHAR *pBankName )
// Write account list to file.
{
	if( pBankName == NULL ) {
		return FALSE;
	}
	ASSERT( strlen( pBankName ) < DATA_LIMIT - FileNameCatLength );

	BOOL bIsSaverSuccess = FALSE;
	CXmlSaver Saver;

	CHAR szFileName[ DATA_LIMIT ] = { '\0' };
	strcpy( szFileName, pBankName );
	strcat( szFileName, ".xml" );
	// Save data from the account list
	bIsSaverSuccess = Saver.SaveXmlFile( szFileName, m_pAccountList );

	return bIsSaverSuccess;
}

BOOL CBank::IsAdminCorrect( TAdminData &AdminData )
// Check admin name and password
{
	if( strcmp( m_SettingData.AdminData.szAdminPassword, AdminData.szAdminPassword ) == 0 && strcmp( m_SettingData.AdminData.szAdminID, AdminData.szAdminID ) == 0 ) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}

void CBank::TotalMoney( TAccount* pAllAccount, INT* pTotalMoney, INT nMoneyKindNum )
// Compute bank total money
{
	ASSERT( pAllAccount != NULL );
	while( pAllAccount != NULL ) {
		for( INT i = 0; i < nMoneyKindNum; i++ ) {
			*( pTotalMoney + i ) += pAllAccount->nBalance[ i ];
		}
		pAllAccount = pAllAccount->pNextAccount;
	}
}
