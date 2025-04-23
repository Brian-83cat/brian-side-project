#include <stdio.h>
#include <afx.h>

#include "SafeMemory.h"
#include "TypeDefine.h"
#include "UI.h"

#include "IBank.h"
#include "Bank.h"
#include "BankManager.h"
#include "ATMOP.h"

#define NOT_SELECT_BANK ( -1 )

CATMOP::CATMOP( CBankManager *pBankManager )
// Constructor
{
	m_nBankIndex = NOT_SELECT_BANK;
	m_pBankManager = pBankManager;
}

CATMOP::~CATMOP( void )
// Destructor
{
	m_nBankIndex = NOT_SELECT_BANK;
	m_pBankManager = NULL;
}

void CATMOP::StartApplication( void )
// Start application
{
	BOOL bRun = TRUE;
	while( bRun == TRUE ) {
		bRun = CoreProcess();
	}
}

BOOL CATMOP::CoreProcess( void )
// Core process
{
	INT nMainMenuUserInput = 0;
	CUI UI;
	BOOL bBankRun = FALSE;
	IBank *pSelectBank = NULL;

	// Show main menu and let user input
	UI.ShowATMOPMsg( EBOPM_SHOW_MAIN_MENU );
	UI.GetInteger( nMainMenuUserInput );
	GetUserServiceSelect( nMainMenuUserInput, EMM_EXIT, EMM_Num );

	// User input 0 and want to leave the main menu
	if( nMainMenuUserInput == EMM_EXIT ) {
		return FALSE;
	}
	else {
		// Bank index equal user input minus exit item
		m_nBankIndex = nMainMenuUserInput - 1;

		bBankRun = m_pBankManager->GetOPBank( &pSelectBank, m_nBankIndex );

		// User select bank is not exist
		if( bBankRun == FALSE ) {
			UI.ShowATMOPMsg( EBOPM_MENU_BANK_FAIL );
		}

		// Run select bank process
		while ( bBankRun == TRUE ) {
			bBankRun = BankProcess( pSelectBank );
		}
	}
	return TRUE;
}

BOOL CATMOP::BankProcess( IBank *pSelectBank )
// Core process
{
	ASSERT( m_nBankIndex != NOT_SELECT_BANK );
	INT nBankMenuUserInput = 0;
	CUI UI;

	// Show bank menu and let user input
	UI.ShowATMOPMsg( EBOPM_SHOW_BANK_MENU );
	UI.GetInteger( nBankMenuUserInput );
	// Check user input whether correct. If not correct get user input again
	GetUserServiceSelect( nBankMenuUserInput, EMMF_EXIT, EMMF_Num );

	switch( nBankMenuUserInput ) {

	case EMMF_EXIT:

		// Leave the bank
		m_nBankIndex = NOT_SELECT_BANK;
		return FALSE;

	case EMMF_CREATEACCOUNT:

		// Create new account.
		CreateAccount( pSelectBank );
		break;

	case EMMF_LOGINACCOUNT:

		// Log in process.
		LogInProcess( pSelectBank );
		break;

	case EMMF_ADMIN:

		// Admin log in
		AdminLogIn( pSelectBank );
		break;
	}
	return TRUE;
}

void CATMOP::LogInProcess( IBank *pSelectBank )
// Log in process.
{
	ASSERT( m_nBankIndex != NOT_SELECT_BANK );
	CUI UI;
	INT nLogInMenuUserInput = 0;
	BOOL bIsLogIn = FALSE;
	CHAR szLogInAccountID[ DATA_LIMIT ] = { '\0' };

	// Log in account at main menu.
	bIsLogIn = LogInAccount( szLogInAccountID );

	// Enter log in menu
	while( bIsLogIn ) {

		// Show log in menu and let user input.
		UI.ShowATMOPMsg( EBOPM_SHOW_LOG_IN_MENU );
		UI.GetInteger( nLogInMenuUserInput );
		// Check user input whether 0 to 6.
		GetUserServiceSelect( nLogInMenuUserInput, ELMF_EXIT, ELMF_Num );

		switch( nLogInMenuUserInput ) {

		case ELMF_EXIT:

			// Log out account
			bIsLogIn = FALSE;
			pSelectBank->LogOutAccount();
			break;

		case ELMF_INQUIREBALANCE:

			// Show the account balance
			InquireBalance( pSelectBank );
			break;

		case ELMF_DEPOSIT:

			// Deposit money
			Deposit( pSelectBank );
			break;

		case ELMF_WITHDRAW:

			// Withdraw money
			Withdraw( pSelectBank );
			break;

		case ELMF_EXCHANGE:

			// Exchange money
			Exchange( pSelectBank );
			break;

		case ELMF_TRANSFER:

			// Transfer money
			Transfer( pSelectBank );
			break;

		case ELMF_TRANSACTION_RECORD:

			// Show transaction record
			TransactionRecord( pSelectBank );
			break;

		case ELMF_BANKTRANSFER:

			// Bank transfer.
			BankTransfer( szLogInAccountID );
			break;
		}
	}
}

void CATMOP::CreateAccount( IBank *pSelectBank )
// Create account at main menu
{
	ASSERT( m_nBankIndex != NOT_SELECT_BANK );
	TAccount NewAccount;
	CUI UI;
	BOOL bDoWhile = TRUE;
	INT nCreateAccountStatus = 0;

	// Show ID message and input the ID you want to create.
	UI.ShowCreateAccountMsg( ECAM_INPUT_ID );
	UI.GetString( NewAccount.szAccountID );

	// Show password message and input password.
	UI.ShowCreateAccountMsg( ECAM_INPUT_PASSWORD );
	UI.GetString( NewAccount.szAccountPassword );

	// Show NTD balance message and input balance
	UI.ShowCreateAccountMsg( ECAM_INPUT_BALANCE );
	UI.GetInteger( NewAccount.nBalance[ EMK_NTD ] );

	// Show US balance message and input balance
	UI.ShowCreateAccountMsg( ECAM_INPUT_US_BALANCE );
	UI.GetInteger( NewAccount.nBalance[ EMK_US ] );

	while( bDoWhile ) {

		// Check account information input and return status.
		nCreateAccountStatus = pSelectBank->CreateAccount( &NewAccount );
		switch( nCreateAccountStatus ) {
		
		case ECAM_ACCOUNT_INVALID:

			UI.ShowCreateAccountMsg( ECAM_ACCOUNT_INVALID );
			bDoWhile = FALSE;
			break;

		case ECAM_ID_EXIST:

			// AccountID exist, show the message and get string again.
			UI.ShowCreateAccountMsg( ECAM_ID_EXIST );
			UI.GetString( NewAccount.szAccountID );
			break;

		case ECAM_ID_RANGE_FAIL:
			// AccountID range fail
			UI.ShowCreateAccountMsg( ECAM_ID_RANGE_FAIL );
			UI.GetString( NewAccount.szAccountID );
			break;

		case ECAM_PASSWORD_RANGE_FAIL:

			// Password range fail
			UI.ShowCreateAccountMsg( ECAM_PASSWORD_RANGE_FAIL );
			UI.GetString( NewAccount.szAccountPassword );
			break;

		case ECAM_BALANCE_FAIL:

			// Input NTD balance fail
			UI.ShowCreateAccountMsg( ECAM_BALANCE_FAIL );
			UI.GetInteger( NewAccount.nBalance[ EMK_NTD ] );
			break;

		case ECAM_US_BALANCE_FAIL:

			// Input US balance fail
			UI.ShowCreateAccountMsg( ECAM_US_BALANCE_FAIL );
			UI.GetInteger( NewAccount.nBalance[ EMK_US ] );
			break;

		case ECAM_CREATE_SUCCESS:

			// Create account success
			UI.ShowCreateAccountMsg( ECAM_CREATE_SUCCESS );
			bDoWhile = FALSE;
			break;
		}
	}
}

BOOL CATMOP::LogInAccount( CHAR *pAccountID )
// Log in account at main menu.
{
	ASSERT( m_nBankIndex != NOT_SELECT_BANK );
	BOOL bIsLogInSuccess = FALSE;
	TAccount LogInAccount;
	CUI UI;
	BOOL bDoWhile = TRUE;
	INT nLogInStatus = 0;
	IBank *pBank = NULL;
	m_pBankManager->GetOPBank( &pBank, m_nBankIndex );

	// Show ID message and input the ID you want to log in.
	UI.ShowLogInMsg( ELIM_INPUT_ID );
	UI.GetString( LogInAccount.szAccountID );

	// Show password message and input password.
	UI.ShowLogInMsg( ELIM_INPUT_PASSWORD );
	UI.GetString( LogInAccount.szAccountPassword );

	while( bDoWhile ) {

		// Check account information input and return status.
		nLogInStatus = pBank->LogInAccount( &LogInAccount );
		switch( nLogInStatus ) {

		case ELIM_NOT_LOG_IN:

			// Not log in yet.
			UI.ShowLogInMsg( ELIM_NOT_LOG_IN );
			bDoWhile = FALSE;
			break;

		case ELIM_ID_NOT_EXIST:

			// Account ID not exist.
			UI.ShowLogInMsg( ELIM_ID_NOT_EXIST );
			bDoWhile = FALSE;
			break;

		case ELIM_INVALID_PASSWORD:

			// Password is not correct
			UI.ShowLogInMsg( ELIM_INVALID_PASSWORD );
			UI.GetString( LogInAccount.szAccountPassword );
			break;

		case ELIM_LOGIN_SUCCESS:

			// Log in success
			UI.ShowLogInMsg( ELIM_LOGIN_SUCCESS );
			bIsLogInSuccess = TRUE;
			bDoWhile = FALSE;
			break;
		}
	}
	strcpy( pAccountID, LogInAccount.szAccountID );
	return bIsLogInSuccess;
}

void CATMOP::InquireBalance( IBank *pSelectBank )
// Inquire balance
{
	ASSERT( m_nBankIndex != NOT_SELECT_BANK );
	INT nMoneyKindSelect = 0;
	CUI UI;

	// Show currency select message and get user input
	UI.ShowLogInMsg( ELIM_MONEY_KIND_INQUIRE );
	UI.GetInteger( nMoneyKindSelect );
	GetUserServiceSelect( nMoneyKindSelect, EMK_NTD, EMK_Num );
	// Show balance
	UI.ShowInquireMsg( nMoneyKindSelect, pSelectBank->GetBalance( nMoneyKindSelect ) );
}

void CATMOP::Deposit( IBank *pSelectBank )
// Deposit money
{
	ASSERT( m_nBankIndex != NOT_SELECT_BANK );
	INT nMoneyKindSelect = 0;
	INT nMoney = 0;
	CUI UI;
	BOOL bDoWhile = TRUE;
	INT nDepositStatus = 0;

	// Show currency message and get money kind
	UI.ShowLogInMsg( ELIM_MONEY_KIND );
	UI.GetInteger( nMoneyKindSelect );
	GetUserServiceSelect( nMoneyKindSelect, EMK_NTD, EMK_Num );

	// Show message and get deposit money
	UI.ShowLogInMsg( ELIM_INPUT_MONEY );
	UI.GetInteger( nMoney );

	while( bDoWhile ) {

		nDepositStatus = pSelectBank->Deposit( nMoney, nMoneyKindSelect );
		switch( nDepositStatus ) {

		case ELIM_NOT_LOG_IN:

			// Not log in yet
			UI.ShowLogInMsg( ELIM_NOT_LOG_IN );
			bDoWhile = FALSE;
			break;

		case ELIM_INVALID_BALANCE:

			// Invalid balance
			UI.ShowLogInMsg( ELIM_INVALID_BALANCE );
			UI.GetInteger( nMoney );
			break;

		case ELIM_DEPOSIT_SUCCESS:

			// Deposit success
			InquireBalance( pSelectBank );
			UI.ShowLogInMsg( ELIM_DEPOSIT_SUCCESS );
			bDoWhile = FALSE;
			break;
		}
	}
}

void CATMOP::Withdraw( IBank *pSelectBank )
// Withdraw money
{
	ASSERT( m_nBankIndex != NOT_SELECT_BANK );
	INT nMoneyKindSelect = 0;
	INT nMoney = 0;
	CUI UI;
	BOOL bDoWhile = TRUE;
	INT nWithdrawStatus = 0;

	// Show withdraw message and get money kind
	UI.ShowLogInMsg( ELIM_MONEY_KIND_WHITHDRAW );
	UI.GetInteger( nMoneyKindSelect );
	GetUserServiceSelect( nMoneyKindSelect, EMK_NTD, EMK_Num );

	// Show message and get withdraw money
	UI.ShowLogInMsg( ELIM_WHITHDRAW_MONEY );
	UI.GetInteger( nMoney );

	while( bDoWhile ) {

		nWithdrawStatus = pSelectBank->Withdraw( nMoney, nMoneyKindSelect );
		switch( nWithdrawStatus ) {

		case ELIM_NOT_LOG_IN:

			// Not log in yet
			UI.ShowLogInMsg( ELIM_NOT_LOG_IN );
			bDoWhile = FALSE;
			break;

		case ELIM_WHITHDRAW_FAIL:

			// No money in account
			UI.ShowLogInMsg( ELIM_WHITHDRAW_FAIL );
			bDoWhile = FALSE;
			break;

		case ELIM_INVALID_BALANCE_WHITHDRAW:

			// Invalid balance
			UI.ShowLogInMsg( ELIM_INVALID_BALANCE_WHITHDRAW );
			UI.GetInteger( nMoney );
			break;

		case ELIM_WHITHDRAW_SUCCESS:

			// Withdraw success
			InquireBalance( pSelectBank );
			UI.ShowLogInMsg( ELIM_WHITHDRAW_SUCCESS );
			bDoWhile = FALSE;
			break;
		}
	}
}

void CATMOP::Exchange( IBank *pSelectBank )
// Exchange money
{
	ASSERT( m_nBankIndex != NOT_SELECT_BANK );
	INT nUserselect = 0;
	INT nExchangeKind;
	INT nMoneyKindSelect;
	INT nExchangeMoney = 0;
	CUI UI;
	BOOL bDoWhile = TRUE;
	INT nExchangeStatus = 0;

	// Show exchange message and select exchange kind
	UI.ShowLogInMsg( ELIM_EXCHANGE_KIND );
	UI.GetInteger( nExchangeKind );
	GetUserServiceSelect( nExchangeKind, EEK_CASHBUY, EEK_Num );

	// Select currency
	UI.ShowLogInMsg( ELIM_EXCHANGE_CURRENCY );
	UI.GetInteger( nMoneyKindSelect );
	GetUserServiceSelect( nMoneyKindSelect, EMK_US, EMK_Num );

	// Get money user want to exchange
	UI.ShowLogInMsg( ELIM_EXCHANGE_MONEY );
	UI.GetInteger( nExchangeMoney );

	while( bDoWhile ) {

		nExchangeStatus = pSelectBank->Exchange( ( EExchangeKind )nExchangeKind, ( EMoneyKind )nMoneyKindSelect, nExchangeMoney );
		switch( nExchangeStatus ) {

		case ELIM_NOT_LOG_IN:

			// Not log in yet
			UI.ShowLogInMsg( ELIM_NOT_LOG_IN );
			bDoWhile = FALSE;
			break;

		case ELIM_EXCHANGE_FAIL:

			// No money in account
			UI.ShowLogInMsg( ELIM_EXCHANGE_FAIL );
			bDoWhile = FALSE;
			break;

		case ELIM_BALANCE_NOT_ENOUGH:

			// Invalid exchange money for cash buy case
			UI.ShowLogInMsg( ELIM_BALANCE_NOT_ENOUGH );
			UI.ShowInteger( nExchangeMoney );
			// Show message and get exchange money again
			UI.ShowLogInMsg( ELIM_EXCHANGE_MONEY );
			UI.GetInteger( nExchangeMoney );
			break;

		case ELIM_BALANCE_NOT_ENOUGH_RATE:

			// Invalid exchange money for cash sale case
			UI.ShowLogInMsg( ELIM_BALANCE_NOT_ENOUGH_RATE );
			UI.ShowInteger( nExchangeMoney );
			// Show message and get exchange money again
			UI.ShowLogInMsg( ELIM_EXCHANGE_MONEY );
			UI.GetInteger( nExchangeMoney );
			break;

		case ELIM_EXCHANGE_SUCCESS:

			// Show real exchange money
			if( nExchangeKind == EEK_CASHSALE ) {
				UI.ShowLogInMsg( ELIM_REAL_EXCHANGE_MONEY );
				UI.ShowInteger( nExchangeMoney );
			}

			// Exchange success
			UI.ShowLogInMsg( ELIM_EXCHANGE_SUCCESS );
			bDoWhile = FALSE;
			break;
		}
	}
}

void CATMOP::Transfer( IBank *pSelectBank )
// Transfer money
{
	ASSERT( m_nBankIndex != NOT_SELECT_BANK );
	INT nMoneyKindSelect = 0;
	INT nMoney = 0;
	CHAR szSelectID[ DATA_LIMIT ] = { '\0' };
	CUI UI;
	BOOL bDoWhile = TRUE;
	INT nTransferStatus = 0;

	// Show transfer message and get money kind
	UI.ShowLogInMsg( ELIM_MONEY_KIND_TRANSFER );
	UI.GetInteger( nMoneyKindSelect );
	GetUserServiceSelect( nMoneyKindSelect, EMK_NTD, EMK_Num );

	// Show message and get select ID
	UI.ShowLogInMsg( ELIM_TRANSFER_ACCOUNT );
	UI.GetString( szSelectID );

	// Show message and get transfer money
	UI.ShowLogInMsg( ELIM_TRANSFER_MONEY );
	UI.GetInteger( nMoney );

	while( bDoWhile ) {

		nTransferStatus = pSelectBank->Transfer( nMoney, nMoneyKindSelect, szSelectID );
		switch( nTransferStatus ) {

		case ELIM_NOT_LOG_IN:

			// Not log in yet
			UI.ShowLogInMsg( ELIM_NOT_LOG_IN );
			bDoWhile = FALSE;
			break;

		case ELIM_TRANSFER_FAIL:

			// No money in account
			UI.ShowLogInMsg( ELIM_TRANSFER_FAIL );
			bDoWhile = FALSE;
			break;

		case ELIM_INVALID_SELECT_ACCOUNT:

			// Invalid select account ID
			UI.ShowLogInMsg( ELIM_INVALID_SELECT_ACCOUNT );
			UI.GetString( szSelectID );
			break;

		case ELIM_INVALID_BALANCE_TRANSFER:

			// Invalid balance
			UI.ShowLogInMsg( ELIM_INVALID_BALANCE_TRANSFER );
			UI.GetInteger( nMoney );
			break;

		case ELIM_TRANSFER_SUCCESS:

			// Transfer success
			InquireBalance( pSelectBank );
			UI.ShowLogInMsg( ELIM_TRANSFER_SUCCESS );
			bDoWhile = FALSE;
			break;
		}
	}
}

void CATMOP::TransactionRecord( IBank *pSelectBank )
// Transaction record
{
	ASSERT( m_nBankIndex != NOT_SELECT_BANK );
	CUI UI;
	TTransactionRecord* pRecord = NULL;
	INT nRecordNum = 0;

	// Get transaction record form the bank.
	pRecord = pSelectBank->GetTransactionRecord( nRecordNum );

	if( pRecord == NULL ) {
		UI.ShowLogInMsg( ELIM_TRANSACTIONRECORD_FAIL );
	}
	else {
		UI.ShowTransactionRecord( pRecord, nRecordNum );
	}
}

void CATMOP::BankTransfer( CHAR *pAccountID )
// Bank Transfer at log in menu
{
	ASSERT( m_nBankIndex != NOT_SELECT_BANK );
	CUI UI;
	TTransferData BankTransferData;
	INT nBankTransferStatus = 0;
	BOOL bDoWhile = TRUE;
	INT nMaxTransferMoney = 0;

	strcpy( BankTransferData.szTransferOutID, pAccountID );
	BankTransferData.nLogInBankIdx = m_nBankIndex;

	// Show bank select message, get bank select and check bank select
	UI.ShowLogInMsg( ELIM_BANK_SELECT );
	UI.GetInteger( BankTransferData.nSelectBankIdx );
	GetUserServiceSelect( BankTransferData.nSelectBankIdx, EBN_E_SUN, EBN_Num );

	// Show bank transfer account message and get select ID
	UI.ShowLogInMsg( ELIM_BANKTRANSFER_ACCOUNT );
	UI.GetString( BankTransferData.szTransferInID );

	// Show bank transfer money kind message and get money kind
	UI.ShowLogInMsg( ELIM_MONEY_KIND_BANKTRANSFER );
	UI.GetInteger( BankTransferData.nMoneyKind );
	GetUserServiceSelect( BankTransferData.nMoneyKind, EMK_NTD, EMK_Num );

	// Show bank transfer message and get bank transfer money
	UI.ShowLogInMsg( ELIM_BANKTRANSFER_MONEY );
	UI.GetInteger( BankTransferData.nMoney );

	while( bDoWhile ) {
	
		nBankTransferStatus = m_pBankManager->BankTransfer( &BankTransferData, nMaxTransferMoney );
		switch( nBankTransferStatus ) {

		case ELIM_BANKTRANSFER_FAIL:

			// Bank transfer fail and back to log in menu
			UI.ShowLogInMsg( ELIM_BANKTRANSFER_FAIL );
			bDoWhile = FALSE;
			break;

		case ELIM_BANKTRANSFER_MONEY_FAIL:

			// No balance in account and back to log in menu
			UI.ShowLogInMsg( ELIM_BANKTRANSFER_MONEY_FAIL );
			bDoWhile = FALSE;
			break;

		case ELIM_BANKTRANSFER_BANK_FAIL:

			// Select wring bank and let user select again
			UI.ShowLogInMsg( ELIM_BANKTRANSFER_BANK_FAIL );
			UI.GetInteger( BankTransferData.nSelectBankIdx );
			GetUserServiceSelect( BankTransferData.nSelectBankIdx, EBN_E_SUN, EBN_Num );
			break;

		case ELIM_BANKTRANSFER_SELECTID_FAIL:

			// User select ID is not in the select bank, so return back to log in menu
			UI.ShowLogInMsg( ELIM_BANKTRANSFER_SELECTID_FAIL );
			bDoWhile = FALSE;
			break;

		case ELIM_BANKTRANSFER_MONEY_INCORRECT:

			// Invalid transfer money and show the maximum transfer money
			UI.ShowLogInMsg( ELIM_BANKTRANSFER_MONEY_INCORRECT );
			UI.ShowInteger( nMaxTransferMoney );
			// Show message and get transfer money again
			UI.ShowLogInMsg( ELIM_BANKTRANSFER_MONEY );
			UI.GetInteger( BankTransferData.nMoney );
			break;

		case ELIM_BANKTRANSFER_SUCCESS:

			// Bank transfer success, show balance and transfer success message
			UI.ShowLogInMsg( ELIM_BANKTRANSFER_SUCCESS );
			bDoWhile = FALSE;
			break;
		}
	}
}

void CATMOP::AdminLogIn( IBank *pSelectBank )
// admin log in at bank menu.
{
	ASSERT( m_nBankIndex != NOT_SELECT_BANK );

	TAdminData Admin;
	TAccount AllAccount;
	CUI UI;
	INT nGetDataStatus = 0;
	INT nTotalMoney[ EMK_Num ] = { 0 };
	BOOL bAdminLogIn = FALSE;

	// Show admin ID message and input the admin ID
	UI.ShowAdminMsg( EAM_ADMIN_ID );
	UI.GetString( Admin.szAdminID );

	// Show password message and input password.
	UI.ShowAdminMsg( EAM_ADMIN_PASSWORD );
	UI.GetString( Admin.szAdminPassword );

	// Check admin dat
	bAdminLogIn = pSelectBank->AdminLogIn( Admin );
	if( bAdminLogIn == TRUE ) {
		// Get all account
		nGetDataStatus = pSelectBank->GetAllAccountInfo( Admin, &AllAccount, nTotalMoney );
		switch( nGetDataStatus ) {

		case EAM_ADMIN_NOACCOUNT:

			// No account in bank
			UI.ShowAdminMsg( EAM_ADMIN_NOACCOUNT );
			break;

		case EAM_ADMIN_SUCCESS:

			// Show bank record
			UI.ShowAdminMsg( EAM_ADMIN_SUCCESS );
			UI.ShowBankRecord( &AllAccount, nTotalMoney );
			break;
		}
	}
	else {
		UI.ShowAdminMsg( EAM_ADMIN_FAIL );
	}
}

void CATMOP::GetUserServiceSelect( INT &nMenuUserInput, INT nMin, INT nMax )
// Get menu user input
{
	CUI UI;
	while( nMenuUserInput < nMin || nMenuUserInput >= nMax ) {
		UI.ShowATMOPMsg( EBOPM_MENU_INPUT_FAIL );
		UI.GetInteger( nMenuUserInput );
	}
	ASSERT( nMenuUserInput >= nMin && nMenuUserInput <= nMax );
}
