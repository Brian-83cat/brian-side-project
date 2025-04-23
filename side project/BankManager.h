#ifndef _BANKMANAGER_H_
#define _BANKMANAGER_H_

class CBank;

class CBankManager
{
public:
	CBankManager( CBank* pBank[ EBN_Num ] );
	// Constructor

	~CBankManager( void );
	// Destructor

	BOOL GetOPBank( IBank **pBank, INT nBankIndex );
	// Get operation bank

	ELogInMsg BankTransfer( TTransferData* pBankTransferData, INT &nMaxTransferMoney );
	// Bank transfer

private:
	CBank *m_pBank[ EBN_Num ];
	// Class bank
};

#endif
