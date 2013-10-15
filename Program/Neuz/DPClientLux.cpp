#include "stdafx.h"
#include "defineText.h"
#include "defineSound.h"
#include "defineItem.h"

#if __VER >= 19
#include "defineItemGrade.h"
#include "defineItemType.h"
#endif//__VER >= 19

#include "defineSkill.h"
#include "appdefine.h"
#include "dploginclient.h"
#include "dpCertified.h"
#include "dpclient.h"
#include "MsgHdr.h"
#include "dialogmsg.h"
#include "misc.h"
#include "defineObj.h" 
#include "WndQuest.h" 
#include "mover.h"
#include "Ship.h"
#include "Sfx.h"
#include "..\_UnhandledException\ExceptionHandler.h"
#include "..\_Network\ErrorCode.h"
#include "Environment.h"

#include "Network.h"

#ifndef __ENVIRONMENT_EFFECT
extern	CEnvironment	g_Environment;
#endif // __ENVIRONMENT_EFFECT

extern	CDPCertified	g_dpCertified;
extern	CDPLoginClient	g_dpLoginClient;

