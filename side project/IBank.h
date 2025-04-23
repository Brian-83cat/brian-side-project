#ifndef _IBANK_H_
#define _IBANK_H_

class IBank
{
public:
	virtual ~IBank( void ) {}
	// Destructor

	virtual ECreateAccountMsg CreateAccount( TAccount* pAccount ) = 0;
	// Check tne account unit that user wnat to create and return status.

	virtual ELogInMsg LogInAccount( TAccount* pAccount ) = 0;
	// Check tne account ID and password that user wnat to log in and return status.

	virtual INT GetBalance( INT nMoneyKind ) = 0;
	// Get the balance after log in success.

	virtual DOUBLE GetHandlingFee( INT nBankIndexSelect ) = 0;
	// Get bank rate

	virtual void LogOutAccount( void ) = 0;
	// Log out the account and clear data member.

	virtual ELogInMsg Deposit( INT nMoney, INT nKind ) = 0;
	// Deposit money

	virtual ELogInMsg Withdraw( INT nMoney, INT nKind ) = 0;
	// Withdraw money

	virtual ELogInMsg Exchange( EExchangeKind nExchangeKind, EMoneyKind nMoneyKind, INT &nExchangeMoney ) = 0;
	// Exchange money

	virtual ELogInMsg Transfer( INT nMoney, INT nKind, CHAR *pSelectID ) = 0;
	// Transfer money to select ID

	virtual TTransactionRecord* GetTransactionRecord( INT &nNum ) = 0;
	// Get transaction record

	virtual BOOL CheckAccountID( CHAR *pInputID ) = 0;
	// Check ID whether in the account list.

	virtual BOOL AdminLogIn( TAdminData &AdminData ) = 0;
	// Admin log in

	virtual EAdminMsg GetAllAccountInfo( TAdminData &AdminData, TAccount* pAllAccount, INT* pTotalMoney ) = 0;
	// Get all accounts if admin log in
};

#endif
