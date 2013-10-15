#ifndef __SCRIPT_HELPER_H__
#define __SCRIPT_HELPER_H__

class CNpcDialogInfo;
void InitPredefineKey(); 
BOOL RunPredefineKey( LPCTSTR szKey, CNpcDialogInfo& info );

CWorld* __GetWorld( int nPcId, int nSrcId, int nCaller );
int __AddKey( int nPcId, LPCTSTR lpszWord, LPCTSTR lpszKey = "", DWORD dwParam = 0);
int __AddAnswer( int nPcId, LPCTSTR lpszWord, LPCTSTR lpszKey, DWORD dwParam1, int nQuest );
int __RemoveQuest( int nPcId, int nQuest );
int __RemoveAllKey( int nPcId );
int __SayQuest( int nPcId,int nQuestId, int nIdx );
int __RunQuest( int nPcId, int nNpcId, int nQuestId );
int __EndQuest( int nPcId, int nQuestId, BOOL IsEndQuestCondition = TRUE );
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
int __AddQuestKey( int nPcId, int nQuestId, LPCTSTR lpKey, int nParam = 0, BOOL bNew = FALSE );
#else // __IMPROVE_QUEST_INTERFACE
int __AddQuestKey( int nPcId, int nQuestId, LPCTSTR lpKey, int nParam = 0 );
#endif // __IMPROVE_QUEST_INTERFACE
void __QuestBegin( int nPcId, int nNpcId, int nQuestId );
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
void __QuestEnd( int nPcId, int nNpcId, int& nGlobal, int nQuestId = 0, BOOL bButtOK = FALSE );
#else // __IMPROVE_QUEST_INTERFACE
void __QuestEnd( int nPcId, int nNpcId, int& nGlobal );
#endif // __IMPROVE_QUEST_INTERFACE
void __QuestBeginYes( int nPcId, int nNpcId, int nQuestId );
void __QuestBeginNo( int nPcId, int nNpcId, int nQuestId );
void __QuestEndComplete( int nPcId, int nNpcId, int& nGlobal, int nVal, int nQuestId );

int __SetQuestState( DWORD dwIdMover, int nQuest, int nState );
void __SetQuest( DWORD dwIdMover, int nQuest );
#ifdef __JEFF_11
#if __VER >= 13 // __CHIPI_QUESTITEM_FLAG
void __CreateItem( DWORD dwIdMover, int v1, int v2, int v3 = 0, BYTE v4 = 0 );
#else	// __CHIPI_QUESTITEM_FLAG
void __CreateItem( DWORD dwIdMover, int v1, int v2, int v3 = 0 );
#endif // __CHIPI_QUESTITEM_FLAG
#else	// __JEFF_11
void __CreateItem( DWORD dwIdMover, int v1, int v2 );
#endif	// __JEFF_11
void __AddGold( DWORD dwIdMover, int nGold );
void __RemoveGold( DWORD dwIdMover, int nGold );
#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
void __QuestSort( vector<int> & vecQuestId );
#endif // __IMPROVE_QUEST_INTERFACE

#endif
