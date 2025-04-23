#ifndef _ATMOP_H_
#define _ATMOP_H_

class CBank;

class CBankManager;

class CATMOP
{
public:
	CATMOP( CBankManager *BankManager );
	// Constructor

	~CATMOP( void );
	// Destructor

	void StartApplication( void );
	// Start application and run main core

private:
	CBankManager *m_pBankManager;
	// Bank manager

	INT m_nBankIndex;
	// The user select bank index

	BOOL CoreProcess( void );
	// Core process

	BOOL BankProcess( IBank *pSelectBank );
	// Bank process

	void LogInProcess( IBank *pSelectBank );
	// Log in process

	void CreateAccount( IBank *pSelectBank );
	// Create account at main menu

	BOOL LogInAccount( CHAR *pAccountID );
	// Log in account at main menu

	void InquireBalance( IBank *pSelectBank );
	// Inquire balance

	void Deposit( IBank *pSelectBank );
	// Deposit money

	void Withdraw( IBank *pSelectBank );
	// Withdraw money

	void Exchange( IBank *pSelectBank );
	// Exchange money

	void Transfer( IBank *pSelectBank );
	// Transfer money

	void TransactionRecord( IBank *pSelectBank );
	// Transaction record

	void BankTransfer( CHAR *pAccountID );
	// Bank Transfer

	void AdminLogIn( IBank *pSelectBank );
	// Admin log in

	void GetUserServiceSelect( INT &nMenuUserInput, INT nMin, INT nMax );
	// Get user service select
};

#endif
