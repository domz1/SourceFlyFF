#ifndef __PATH_H
#define __PATH_H

//CString MakePath( LPCTSTR lpszFileName );
CString MakePath( LPCTSTR lpszDirName, LPCTSTR lpszFileName );
CString MakePath( LPCTSTR lpDir, DWORD dwLan, LPCTSTR lpFile );
void MakePath( CString& strFullPath, LPCTSTR lpszDirName, LPCTSTR lpszFileName );
void MakePath( TCHAR* lpszFullPath, LPCTSTR lpszDirName, LPCTSTR lpszFileName );
//void SetResourcePath();
//LPCTSTR GetResourcePath();

#define DIR_MUSIC        _T( "Music\\"   )
#define DIR_SOUND        _T( "Sound\\"   )
#define DIR_THEME        _T( "Theme\\Default\\"  )
#define DIR_TEXT         _T( "Text\\"  )
#define DIR_DIALOG       _T( "Dialog\\"  )
#define DIR_WORLD        _T( "World\\"   )
#define DIR_WORLD_GUILDCOMBAT  _T( "World\\WdGuildWar\\"   )
#define DIR_WORLDPLAY    _T( "World\\Play\\"  )
#define DIR_WORLDTEX     _T( "World\\Texture\\"  ) 
#define DIR_WORLDTEXMID  _T( "World\\TextureMid\\"  ) 
#define DIR_WORLDTEXLOW  _T( "World\\TextureLow\\"  ) 
#define DIR_WORLDTILE    _T( "World\\Tile\\"   )
#define DIR_TEXTURE      _T( "Texture\\" )
#define DIR_FONT         _T( "Font\\"    )
#define DIR_MODEL        _T( "Model\\"   )
#define DIR_ASE		     _T( "ModeL\\Ase\\"   )
#define DIR_MODELTEX     _T( "Model\\Texture\\"   )
#define DIR_MODELTEXMID  _T( "Model\\TextureMid\\"   )
#define DIR_MODELTEXLOW  _T( "Model\\TextureLow\\"   )
#define DIR_ICON         _T( "Icon\\" )
#define DIR_ITEM         _T( "Item\\" )
#define DIR_EFFECT       _T( "Effect\\" )
#define DIR_WEATHER      _T( "Weather\\"   )
#define DIR_CLIENT       _T( "Client\\"   )
#define DIR_SFX          _T( "Sfx\\"   )
#define DIR_SFXTEX       _T( "Sfx\\Texture\\"   )
#define DIR_LOGO         _T( "Model\\Logo\\"   )
#ifdef __BS_EFFECT_LUA
#define DIR_SFXLUA		 _T( "Sfx\\Scripts\\" )
#endif //__BS_EFFECT_LUA
#endif
