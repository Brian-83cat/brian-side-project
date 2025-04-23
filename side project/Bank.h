#ifndef _BANK_H_
#define _BANK_H_

class CAccountList;

class CBank : public IBank
{
public:
	CBank( TBankSettingData SettingData );
	// Constructor

	~CBank( void );
	// Destructor

	ECreateAccountMsg CreateAccount( TAccount* pAccount );
	// Check tne account unit that user wnat to create and return status.

	ELogInMsg LogInAccount( TAccount* pAccount );
	// Check tne account ID and password that user wnat to log in and return status.

	INT GetBalance( INT nMoneyKind );
	// Get the balance after log in success.

	DOUBLE GetHandlingFee( INT nBankIndexSelect );
	// Get bank rate

	void LogOutAccount( void );
	// Log out the account and clear data member.

	ELogInMsg Deposit( INT nMoney, INT nKind );
	// Deposit money

	ELogInMsg Withdraw( INT nMoney, INT nKind );
	// Withdraw money

	ELogInMsg Exchange( EExchangeKind nExchangeKind, EMoneyKind nMoneyKind, INT &nExchangeMoney );
	// Exchange money

	ELogInMsg Transfer( INT nMoney, INT nKind, CHAR *pSelectID );
	// Transfer money to select ID

	TTransactionRecord* GetTransactionRecord( INT &nNum );
	// Get transaction record

	BOOL BankTransferOut( TTransferData* pBankTransferData );
	// Bank transfer out

	BOOL BankTransferIn( TTransferData* pBankTransferData );
	// Bank transfer in

	BOOL CheckAccountID( CHAR *pInputID );
	// Check ID whether in the account list.

	BOOL AdminLogIn( TAdminData &AdminData );
	// Admin log in

	EAdminMsg GetAllAccountInfo( TAdminData &AdminData, TAccount* pAllAccount, INT* pTotalMoney );
	// Get all accounts if admin log in

private:

	CAccountList* m_pAccountList;
	// Account list

	TAccount* m_pLogInAccount;
	// Log in account

	TBankSettingData m_SettingData;
	// Setting data

	BOOL m_bAdminLogIn;
	// Admin log in status

	BOOL CheckDataRange( CHAR *pInputData );
	// Check Data whether in the correct range.

	BOOL CheckInputBalance( INT nInputBalance );
	// Check balance whether in the correct range.

	BOOL IsPasswordCorrect( CHAR *pInputID, CHAR *pInputPassword );
	// Check Password whether correct.

	BOOL AddRecord( CHAR *pOperation, CHAR *pBankName, CHAR *pSlaveAccountID, INT nKind, INT nTransferMoney, INT nCurrentBalance, TAccount* pAccount );
	// Add record

	BOOL DataLoader( CHAR *pFileName );
	// Loader data from data base.

	BOOL DataSaver( CHAR *pFileName );
	// Save account list to data base.

	BOOL IsAdminCorrect( TAdminData &pAdminData );
	// Check admin name and password

	void TotalMoney( TAccount* pAllAccount, INT* pTotalMoney, INT nMoneyKindNum );
	// Compute bank total money
};

#endif
