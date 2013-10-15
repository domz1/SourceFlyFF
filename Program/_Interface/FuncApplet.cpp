#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndParty.h"
#include "WndNotice.h"
#include "WndMessenger.h"
#include "WndBank.h"		 // 은행 / 창고
#include "WndRepairItem.h"	 // 수리창
#include "WndDebugInfo.h"    // 디버그 창 
#include "WndHelp.h"
#include "WndWebBox.h"
#include "WndVendor.h"
#include "WndCommItem.h"
#include "WndQuest.h"
#include "WndMotion.h"
#include "WndPvp.h"
#include "WndGuildVote.h"
#ifdef __IMPROVE_MAP_SYSTEM
#include "WndMapEx.h"
#else // __IMPROVE_MAP_SYSTEM
#include "WndMap.h"
#endif // __IMPROVE_MAP_SYSTEM
#include "WndBagEx.h"

#if __VER >= 8 //__CSC_VER8_2
#include "WndPartyQuick.h"
#endif //__CSC_VER8_2
#if __VER >= 13 // __HOUSING
#include "WndHousing.h"
#endif // __HOUSING

#if __VER >= 15 // __GUILD_HOUSE
#include "WndGuildHouse.h"
#endif

#if __VER >= 15 // __2ND_PASSWORD_SYSTEM
#include "Wnd2ndPassword.h"
#endif // __2ND_PASSWORD_SYSTEM

#ifdef __NEW_WEB_BOX
#include "WndHelperWebBox.h"
#endif // __NEW_WEB_BOX
/*
     여기서 정의된 애플랫 클래스는 테스크 매뉴에 추가될 수 있는 세팅을 포함한다.
	 태스크 매뉴에 애플렛을 추가하기 위해서는 

	 Step. 1 - Applet ID 정의
	   ResData.h를 열고 Applet ID를 정의한다. 매인프로세서는 Applet ID로 Applet을 인식한다.
	   만약 Daisy를 사용할 경우라면 Daisy에서 Id를 지정해주면 된다. 이경우 직접 ResData.h를 수정할 필요는 없다.

	 Step. 2 - DECLAREAPPLET 생성자 함수 선언하기
	   DECLAREAPPLET는 실행 명령이 내려졌을 때 해당 class를 할당하여 매인 프로세서에게 포인터를
	   돌려주는 역할을 한다. 이 함수가 정의되어있지 않으면 매인 프로세서는 class를 실행 시킬 수 없게 된다.
	   DECLAREAPPLET은 AppMain 함수를 만드는 것을 간단하게 정의한 define이다. 용법은 다음과 같다.
	   DECLAREAPPLET( 함수명, new className ); 

	 Step. 3 - map에 Add하기. 마지막 필드의 툴팁 세팅은 Resource\textClient.inc를 참고. 
*/
   



DECLAREAPPLET( AppMain_WndNavigator    , new CWndNavigator   );
DECLAREAPPLET( AppMain_WndCharacter    , new CWndCharacter   );
DECLAREAPPLET( AppMain_WndSkill2       , new CWndSkillTreeEx );
DECLAREAPPLET( AppMain_WndEmotion      , new CWndEmotion     );
DECLAREAPPLET( AppMain_WndMotion       , new CWndMotion      );
DECLAREAPPLET( AppMain_WndTrade        , new CWndTrade       );
DECLAREAPPLET( AppMain_WndVendor       , new CWndVendor );
DECLAREAPPLET( AppMain_WndQuest        , new CWndQuest       );
DECLAREAPPLET( AppMain_WndPvp          , new CWndPvp       );
DECLAREAPPLET( AppMain_WndInventory    , new CWndInventory   );
DECLAREAPPLET( AppMain_WndWebBox       , new CWndWebBox  );
DECLAREAPPLET( AppMain_WndWebBox2      , new CWndWebBox2 );
DECLAREAPPLET( AppMain_WndParty        , new CWndParty  );
DECLAREAPPLET( AppMain_WndGuild        , new CWndGuild  );
DECLAREAPPLET( AppMain_WndCommItem     , new CWndCommItem  );
DECLAREAPPLET( AppMain_WndUpgradeBase  , new CWndUpgradeBase  );
DECLAREAPPLET( AppMain_WndPiercing     , new CWndPiercing  );
DECLAREAPPLET( AppMain_WndChat         , new CWndChat    );
#if __VER >= 11 // __CSC_VER11_4
DECLAREAPPLET( AppMain_WndMessenger    , new CWndMessengerEx );
#else //__CSC_VER11_4
DECLAREAPPLET( AppMain_WndMessenger    , new CWndMessenger );
#endif //__CSC_VER11_4
DECLAREAPPLET( AppMain_WndOptSound     , new CWndOptSound    );
DECLAREAPPLET( AppMain_WndOptWindow    , new CWndOptWindow   );
DECLAREAPPLET( AppMain_WndOptMyInfo    , new CWndOptMyInfo   );
DECLAREAPPLET( AppMain_WndOption       , new CWndTotalOption );
DECLAREAPPLET( AppMain_WndInfoNotice   , new CWndInfoNotice  );
DECLAREAPPLET( AppMain_WndHelpHelp     , new CWndHelp        );
DECLAREAPPLET( AppMain_WndHelpTip      , new CWndHelpTip     );
DECLAREAPPLET( AppMain_WndHelpFAQ      , new CWndHelpFAQ     );
DECLAREAPPLET( AppMain_WndLogOut       , new CWndLogOut      );
DECLAREAPPLET( AppMain_WndQuit         , new CWndQuit        );
DECLAREAPPLET( AppMain_WndWorld        , new CWndWorld       );
DECLAREAPPLET( AppMain_WndDebugInfo    , new CWndDebugInfo   );
DECLAREAPPLET( AppMain_WndStatus       , new CWndStatus       );
DECLAREAPPLET( AppMain_WndLogin        , new CWndLogin        );
DECLAREAPPLET( AppMain_WndSelectServer , new CWndSelectServer );
DECLAREAPPLET( AppMain_WndCreateChar   , new CWndCreateChar   );
DECLAREAPPLET( AppMain_WndSelectChar   , new CWndSelectChar   );

#if __VER >= 9  // __INSERT_MAP
#ifdef __IMPROVE_MAP_SYSTEM
DECLAREAPPLET( AppMain_WndMap   , new CWndMapEx   );
#else // __IMPROVE_MAP_SYSTEM
DECLAREAPPLET( AppMain_WndMap   , new CWndMap   );
#endif // __IMPROVE_MAP_SYSTEM
#endif
#if __VER >= 12 // __LORD
DECLAREAPPLET( AppMain_LordSkill, new CWndLordSkill  );
#endif
#if __VER >= 8 //__CSC_VER8_2
DECLAREAPPLET( AppMain_WndPartyQuick   , new CWndPartyQuick   );
#endif //__CSC_VER8_2
#if __VER >= 8 //__CSC_VER8_3
DECLAREAPPLET( AppMain_WndBuffStatus   , new CWndBuffStatus   );
#endif //__CSC_VER8_3

#ifdef __GUILDVOTE
DECLAREAPPLET( AppMain_WndGuildVote   , new CWndGuildVote  );
#endif
DECLAREAPPLET( AppMain_WndInvenRemoveItem, new CWndInvenRemoveItem );
#if __VER >= 11 // __SYS_POCKET
#ifndef __TMP_POCKET
DECLAREAPPLET( AppMain_BagEx    , new CWndBagEx   );
#endif
#endif

#if __VER >= 12 // __MOD_TUTORIAL
DECLAREAPPLET( AppMain_InfoPang    , new CWndInfoPang   );
#endif
#if __VER >= 13 // __HOUSING
DECLAREAPPLET( AppMain_Housing    , new CWndHousing   );
#endif // __HOUSING

#if __VER >= 15 // __GUILD_HOUSE
DECLAREAPPLET( AppMain_GuildHousing    , new CWndGuildHousing   );
#endif

#if __VER >= 13 // __CSC_VER13_2
DECLAREAPPLET( AppMain_Couple    , new CWndCoupleManager );
#endif //__CSC_VER13_2

#if __VER >= 15 // __2ND_PASSWORD_SYSTEM
DECLAREAPPLET( AppMain_Wnd2ndPassword  , new CWnd2ndPassword );
#endif // __2ND_PASSWORD_SYSTEM

#ifdef __NEW_WEB_BOX
DECLAREAPPLET( AppMain_WndHelperWebBox , new CWndHelperWebBox  );
#endif // __NEW_WEB_BOX

void CWndMgr::AddAllApplet()
{
	//             생성자                    ID                            타이틀                    아이콘                          툴팁 텍스트 
	AddAppletFunc( AppMain_WndNavigator    , APP_NAVIGATOR               , _T( "WndNavigator" )    , _T( "Icon_Navigator.dds" )    , GETTEXT( TID_TIP_NAVIGATOR      ),  'N'  );	
	AddAppletFunc( AppMain_WndStatus       , APP_STATUS1                 , _T( "WndStatus" )       , _T( "Icon_Status.dds"    )    , GETTEXT( TID_TIP_STATUS         ), 'T' );
#if __VER >= 9  // __INSERT_MAP
#ifdef __IMPROVE_MAP_SYSTEM
	AddAppletFunc( AppMain_WndMap       , APP_MAP_EX              , _T( "WndMap" )       , _T( "Icon_Applet.dds"    )    , GETTEXT(TID_TIP_MAP), 'M' );
#else // __IMPROVE_MAP_SYSTEM
	AddAppletFunc( AppMain_WndMap       , APP_MAP                 , _T( "WndMap" )       , _T( "Icon_Applet.dds"    )    , GETTEXT(TID_TIP_MAP), 'M' );
#endif // __IMPROVE_MAP_SYSTEM
#endif
#if __VER >= 12 // __LORD
	if( ::GetLanguage() == LANG_FRE )
		AddAppletFunc( AppMain_LordSkill       , APP_LORD_SKILL             , _T( "WndLordSkill" )       , _T( "Icon_Infopang.dds"    )    , GETTEXT(TID_TIP_INFOPANG), 'A' );
	else
		AddAppletFunc( AppMain_LordSkill       , APP_LORD_SKILL             , _T( "WndLordSkill" )       , _T( "Icon_Infopang.dds"    )    , GETTEXT(TID_TIP_INFOPANG), 'L' );
#endif	// __LORD
#if __VER >= 12 // __MOD_TUTORIAL
	AddAppletFunc( AppMain_InfoPang       , APP_INFOPANG                , _T( "WndInfoPang" )       , _T( "Icon_Infopang.dds"    )    , GETTEXT(TID_TIP_INFOPANG), 0 );
#endif
#if __VER >= 13 // __HOUSING
	AddAppletFunc( AppMain_Housing       , APP_HOUSING                , _T( "WndHousing" )       , _T( "Icon_Housing.dds"    )    , GETTEXT(TID_GAME_HOUSING_BOX), 'Y' );
#endif // __HOUSING

#if __VER >= 15 // __GUILD_HOUSE
	AddAppletFunc( AppMain_GuildHousing     , APP_GH_FURNITURE_STORAGE, _T( "WndGuildHousing" )     , _T( "Icon_Housing.dds"    )    , GETTEXT(TID_GAME_HOUSING_BOX), 'R' );
#endif 
#if __VER >= 13 // __RENEW_CHARINFO
	AddAppletFunc( AppMain_WndCharacter    , APP_CHARACTER3               , _T( "WndCharacter" )    , _T( "Icon_Character.dds" )    , GETTEXT( TID_TIP_CHARACTER      ), 'H' );
#elif __VER >= 9 // __CSC_VER9_2
	AddAppletFunc( AppMain_WndCharacter    , APP_CHARACTER2               , _T( "WndCharacter" )    , _T( "Icon_Character.dds" )    , GETTEXT( TID_TIP_CHARACTER      ), 'H' );
#else //__CSC_VER9_2
	AddAppletFunc( AppMain_WndCharacter    , APP_CHARACTER               , _T( "WndCharacter" )    , _T( "Icon_Character.dds" )    , GETTEXT( TID_TIP_CHARACTER      ), 'H' );
#endif //__CSC_VER9_2
	AddAppletFunc( AppMain_WndInventory    , APP_INVENTORY               , _T( "WndInventory" )    , _T( "Icon_Inventory.dds" )    , GETTEXT( TID_TIP_INVENTORY      ), 'I' );
	
	AddAppletFunc( AppMain_WndWebBox       , APP_WEBBOX                  , _T( "WebBox" )          , _T( "Icon_CitemMall.dds" )    , GETTEXT( TID_TIP_ITEMMALL       ), 0 );
#if __VER >= 10 // __CSC_VER9_1
#ifdef __NEW_SKILL_TREE
	AddAppletFunc( AppMain_WndSkill2       , APP_SKILL4                  , _T( "WndSkill"     )    , _T( "Icon_Skill.dds"     )    , GETTEXT( TID_TIP_SKILL          ), 'K' );
#else // __NEW_SKILL_TREE
	AddAppletFunc( AppMain_WndSkill2       , APP_SKILL3                  , _T( "WndSkill"     )    , _T( "Icon_Skill.dds"     )    , GETTEXT( TID_TIP_SKILL          ), 'K' );
#endif // __NEW_SKILL_TREE
#else
	AddAppletFunc( AppMain_WndSkill2       , APP_SKILL1                  , _T( "WndSkill"     )    , _T( "Icon_Skill.dds"     )    , GETTEXT( TID_TIP_SKILL          ), 'K' );
#endif //__CSC_VER9_1

	AddAppletFunc( AppMain_WndMotion       , APP_MOTION                  , _T( "WndMotion"    )    , _T( "Icon_Motion.dds"   )     , GETTEXT( TID_TIP_MOTION         ),  'O'  );
	AddAppletFunc( AppMain_WndTrade        , APP_TRADE                   , _T( "WndTrade"     )    , _T( "Icon_Trade.dds"     )    , GETTEXT( TID_TIP_TRADE          ),  0  );

#if __VER >= 8 // __S8_VENDOR_REVISION
	AddAppletFunc( AppMain_WndVendor	   , APP_VENDOR_REVISION         , _T( "WndVendor" )	   , _T( "Icon_Applet.dds" )       , GETTEXT( TID_TIP_VENDOR ), 0 );
#else // __VER >= 8 // __S8_VENDOR_REVISION
	AddAppletFunc( AppMain_WndVendor	   , APP_VENDOREX	             , _T( "WndVendor" )	   , _T( "Icon_Applet.dds" )       , GETTEXT( TID_TIP_VENDOR ), 0 );
#endif // __VER >= 8 // __S8_VENDOR_REVISION

#if __VER >= 15 // __IMPROVE_QUEST_INTERFACE
	AddAppletFunc( AppMain_WndQuest        , APP_QUEST_EX_LIST           , _T( "WndQuest"     )    , _T( "Icon_Quest.dds"     )    , GETTEXT( TID_TIP_QUEST          ), g_Neuz.Key.chQuest );
#else // __IMPROVE_QUEST_INTERFACE
	AddAppletFunc( AppMain_WndQuest        , APP_QUEST                   , _T( "WndQuest"     )    , _T( "Icon_Quest.dds"     )    , GETTEXT( TID_TIP_QUEST          ), g_Neuz.Key.chQuest );
#endif // __IMPROVE_QUEST_INTERFACE
	AddAppletFunc( AppMain_WndParty        , APP_PARTY                   , _T( "WndParty"     )    , _T( "Icon_Troupe.dds"    )    , GETTEXT( TID_TIP_PARTY          ),  'P' );

	AddAppletFunc( AppMain_WndGuild        , APP_GUILD                   , _T( "WndGuild"     )    , _T( "Icon_Troupe.dds"    )    , GETTEXT( TID_TIP_COMPANY          ),  'G' );

#ifdef __GUILDVOTE
	AddAppletFunc( AppMain_WndGuildVote    , APP_GUILD_VOTE              , _T( "WndGuildVote")     , _T( "Icon_Troupe.dds"    )    , GETTEXT( TID_TIP_COMPANY          ),  'V' );
#endif
	
#if __VER >= 11 // __SYS_POCKET
	#ifndef __TMP_POCKET
	AddAppletFunc( AppMain_BagEx       , APP_BAG_EX                , _T( "WndBagEx" )       , _T( "Icon_BagBag.tga"    )    , GETTEXT(TID_APP_BAG_EX), 'B' );
	#endif
#endif
	AddAppletFunc( AppMain_WndCommItem    , APP_COMM_ITEM                , _T( "WndCommItem"  )    , _T( "Icon_CItemTime.dds" )    , GETTEXT( TID_TIP_ITEMTIME   ),  'J' );
	AddAppletFunc( AppMain_WndUpgradeBase    , APP_TEST                  , _T( "WndUpgradeBase" )  , _T( "Icon_Troupe.dds"    )    , GETTEXT( TID_TIP_PARTY          ),  0 );
	AddAppletFunc( AppMain_WndPiercing    , APP_PIERCING                 , _T( "WndPiercing"     ) , _T( "Icon_Troupe.dds"    )    , GETTEXT( TID_TIP_PARTY          ),  0 );
	AddAppletFunc( AppMain_WndChat         , APP_COMMUNICATION_CHAT      , _T( "WndChat"      )    , _T( "Icon_Chat.dds"      )    , GETTEXT( TID_TIP_COMMUNICATION_CHAT    ),  0 );
	AddAppletFunc( AppMain_WndMessenger    , APP_MESSENGER_              , _T( "WndMessenger" )    , _T( "Icon_Messenger.dds"   )  , GETTEXT( TID_TIP_MESSENGER ),  'E'  );

	AddAppletFunc( AppMain_WndOptSound     , APP_OPTION_SOUND            , _T( "WndOptSound" )     , _T( "Icon_OptSound.dds" )     , GETTEXT( TID_TIP_OPTION_SOUND          ),  0  );
	AddAppletFunc( AppMain_WndOptWindow    , APP_OPTION_WINDOW           , _T( "WndOptWindow" )    , _T( "Icon_Applet.dds"      )  , GETTEXT( TID_TIP_OPTION_WINDOW         ),  0  );
	AddAppletFunc( AppMain_WndOptMyInfo    , APP_OPTION_MYINFO           , _T( "WndOptMyInfo" )    , _T( "Icon_Applet.dds" )       , GETTEXT( TID_TIP_OPTION_MYINFO         ),  0  );
	AddAppletFunc( AppMain_WndOption	   , APP_OPTIONEX2				 , _T( "WndOption" )	   , _T( "Icon_Applet.dds" )	   , GETTEXT( TID_APP_OPTION				),	0  );			

#ifdef __19_INTERFACE_UPDATE
	AddAppletFunc( AppMain_WndInfoNotice   , APP_NOTICE              , _T( "WndInfoNotice" )   , _T( "Icon_Applet.dds" )       , GETTEXT( TID_TIP_INFO_NOTICE           ),  0  );
#else
	AddAppletFunc( AppMain_WndInfoNotice   , APP_INFO_NOTICE         , _T( "WndInfoNotice" )   , _T( "Icon_Applet.dds" )       , GETTEXT( TID_TIP_INFO_NOTICE           ),  0  );
#endif//__19_INTERFACE_UPDATE
	AddAppletFunc( AppMain_WndHelpHelp     , APP_HELPER_HELP             , _T( "WndHelpHelp" )     , _T( "Icon_HelperHelp.dds" )   , GETTEXT( TID_TIP_HELPER_HELP           ),  0  );
	AddAppletFunc( AppMain_WndHelpTip      , APP_HELPER_TIP              , _T( "WndHelpTip" )      , _T( "Icon_HelperTip.dds" )    , GETTEXT( TID_TIP_HELPER_TIP            ),  0  );
	AddAppletFunc( AppMain_WndHelpFAQ      , APP_HELPER_FAQ              , _T( "WndHelpFAQ" )      , _T( "Icon_HelperFAQ.dds" )    , GETTEXT( TID_TIP_HELPER_FAQ            ),  0  );

	AddAppletFunc( AppMain_WndLogOut       , APP_LOGOUT                  , _T( "WndLogout"   )     , _T( "Icon_Logout.dds"   )     , GETTEXT( TID_TIP_LOGOUT              ),  0  );
	AddAppletFunc( AppMain_WndQuit         , APP_QUIT                    , _T( "WndQuit"      )    , _T( "Icon_Quit.dds"      )    , GETTEXT( TID_TIP_QUIT                  ),  0  );
//	if( GetLanguage() == LANG_TWN )
		AddAppletFunc( AppMain_WndWebBox2  , APP_WEBBOX2                 , _T( "WebBox2" )         , _T( "QOODO.dds" )             , GETTEXT( TID_TIP_QOODO	    ), 0 );

	AddAppletFunc( AppMain_WndWorld        , APP_WORLD                   , _T( "WndWorld" )        , _T( "Icon_Applet.dds" )       , GETTEXT( TID_TIP_WORLD     ),  0  );
	AddAppletFunc( AppMain_WndDebugInfo    , APP_DEBUGINFO               , _T( "WndDebugInfo" )    , _T( "Icon_Applet.dds" )       , GETTEXT( TID_TIP_DEBUGINFO ),  0  );
	AddAppletFunc( AppMain_WndLogin        , APP_LOGIN                   , _T( "WndLogin"      )   , _T( "Icon_Login.dds"      )   , GETTEXT( TID_TIP_APPLET    ),  0  );
	AddAppletFunc( AppMain_WndSelectServer , APP_SELECT_SERVER           , _T( "WndSelectServer")  , _T( "Icon_SelectServer.dds")  , GETTEXT( TID_TIP_WORLD     ),  0  );
	AddAppletFunc( AppMain_WndCreateChar   , APP_CREATE_CHAR             , _T( "WndCreateChar" )   , _T( "Icon_CreateChar.dds" )   , GETTEXT( TID_TIP_WORLD     ),  0  );
	AddAppletFunc( AppMain_WndSelectChar   , APP_SELECT_CHAR             , _T( "WndSelectChar" )   , _T( "Icon_SelectChar.dds" )   , GETTEXT( TID_TIP_DIALOG    ),  0  );

#if __VER >= 8 //__CSC_VER8_2
	AddAppletFunc( AppMain_WndPartyQuick   , APP_PARTY_QUICK             , _T( "WndPartyQuick" )   , NULL   , GETTEXT( TID_TIP_DIALOG    ),  0  );
#endif //__CSC_VER8_2
#if __VER >= 8 //__CSC_VER8_3
	AddAppletFunc( AppMain_WndBuffStatus   , APP_BUFF_STATUS             , _T( "WndBuffStatus" )   , NULL   , GETTEXT( TID_TIP_DIALOG    ),  0  );
#endif //__CSC_VER8_3
#if __VER >= 13 // __CSC_VER13_2
	if( ::GetLanguage() == LANG_FRE )
		AddAppletFunc( AppMain_Couple   , APP_COUPLE_MAIN             , _T( "WndCoupleManager" )   , _T( "Icon_Couple.dds" )   , GETTEXT( TID_GAME_COUPLE ),  'X' );
	else
		AddAppletFunc( AppMain_Couple   , APP_COUPLE_MAIN             , _T( "WndCoupleManager" )   , _T( "Icon_Couple.dds" )   , GETTEXT( TID_GAME_COUPLE ),  'F' );
#endif //__CSC_VER13_2
#if __VER >= 15 // __2ND_PASSWORD_SYSTEM
	AddAppletFunc( AppMain_Wnd2ndPassword  , APP_2ND_PASSWORD_NUMBERPAD  , _T( "Wnd2ndPassword" )   , _T( "Icon_Applet.dds" )   , GETTEXT( TID_2ND_PASSWORD_WINDOW_OPEN ),  0  );
#endif // __2ND_PASSWORD_SYSTEM

#ifdef __NEW_WEB_BOX
	AddAppletFunc( AppMain_WndHelperWebBox , APP_WEBBOX2              , _T( "HelperWebBox" )       , _T( "Icon_HelperHelp.dds" ) , GETTEXT( TID_GAME_HELPER_WEB_BOX_ICON_TOOLTIP ), 0 );
#endif // __NEW_WEB_BOX

}
