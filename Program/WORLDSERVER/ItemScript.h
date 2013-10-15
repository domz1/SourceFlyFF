#ifndef __ITEMSCRIPT__H__
#define __ITEMSCRIPT__H__

enum ITEM_OP
{
	ITEM_OP_EQUIP,
	ITEM_OP_UNEQUIP,
	ITEM_OP_USE,
};

BOOL CompileItemScript( LPCTSTR szFile );
BOOL RunItemScript( CMover* pMover, DWORD dwItemID, ITEM_OP operation, int* pResult );

#endif // __ITEMSCRIPT__H__