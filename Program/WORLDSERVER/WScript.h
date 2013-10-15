#ifndef __SCRIPTLIB20060908_H__
#define __SCRIPTLIB20060908_H__

struct lua_State;

lua_State* GetVM();
BOOL InitializeScriptLib();
void UninitializeScriptLib();

#endif // __SCRIPTLIB20060908_H__