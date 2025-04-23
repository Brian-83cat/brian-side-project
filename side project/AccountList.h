#ifndef _ACCOUNTLIST_H_
#define _ACCOUNTLIST_H_

class CAccountList
{
public:
	CAccountList( void );
	// Constructor

	~CAccountList( void );
	// Destructor

	void NewAccount( TAccount* pAccount );
	// New Account at the end of list.

	TAccount* SearchAccount( CHAR* pInputID );
	// Search account from the account list.

	TAccount* GetAllAccount( void );
	// Get head account from the list

	void DeleteList( void );
	// Delete the list.

private:
	TAccount *m_pHeadAccount;
	// Head of list

	TAccount *m_pTailAccount;
	// Tail of list

	BOOL SetNewAccountData( TAccount* pAccount );
	// Set account data at new tail account.
};

#endif
