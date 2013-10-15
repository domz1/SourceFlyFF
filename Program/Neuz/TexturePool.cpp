
//gmpbigsun

#include "StdAfx.h"

#if __VER >= 15 // __BS_CHANGING_ENVIR

#include "vutil.h"

TexturePool::TexturePool( )
{
}

TexturePool::~TexturePool( )
{
	Free( );
}

TexturePool* TexturePool::Get( )
{
	static TexturePool texPool;
	return &texPool;
}

void TexturePool::Free( )
{
	for( Texture_Iter iter = _cTextures.begin(); iter != _cTextures.end(); ++iter )
	{
		IDirect3DTexture9* pTex = iter->second;
		SAFE_RELEASE( pTex );
	}

	_cTextures.clear();
}

IDirect3DTexture9* TexturePool::GetTexture( const string& dir, const string& filename )
{
	Texture_Iter iter =  _cTextures.find( filename );
	if( iter != _cTextures.end() )
		return iter->second;

	//create texture!

	assert( _pMyDevice );

	string fullname = dir + filename;
	IDirect3DTexture9* pNewTex = NULL;
	if( FAILED( LoadTextureFromRes( _pMyDevice, fullname.c_str(), &pNewTex ) ) )
	{
		assert( 0 && "cannot find texture name" );
		return NULL;
	}

	pair< Texture_Iter, bool > rst =_cTextures.insert( Texture_Container::value_type( filename, pNewTex ) );
	assert( rst.second );

	return pNewTex;
}

#endif	//__BS_CHANGING_ENVIR