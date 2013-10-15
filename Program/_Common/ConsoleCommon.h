
// Desc : CommonUtils
// Author : gmpbigsun
// Date : 20090714

#pragma once

#ifdef __BS_CONSOLE

namespace con
{
	template<typename T>
	class Singleton
	{
		public:
			~Singleton( void )
			{  assert( ms_pSingleton);  ms_pSingleton = 0;  }
			static T& GetSingleton( void )
			{  assert( ms_pSingleton && "Singleton참조 Error");  return ( *ms_pSingleton );  }
			static T* GetSingletonPtr( void )
			{  return ( ms_pSingleton );  }
			
		public:
			Singleton()
			{
				assert(!ms_pSingleton && "Singleton 생성 Error" );
				int offset = (int)((T*)1) - (int)(Singleton <T>*)((T*)1);
				ms_pSingleton = (T*)((int)this + offset);
			}
			
		private:
			static T* ms_pSingleton;
	};
	template<typename T>
			T* Singleton<T>::ms_pSingleton=NULL;

	template< typename T >
	struct Deleter : public unary_function< T, void >
	{
		void operator( ) ( T& t )
		{
			if( t )
			{
				delete t;
				t = 0;
			}
		}
	};

	template< typename T >
	struct PairDeleter : public unary_function< T, void >
	{
		void operator( ) ( T& t )
		{
			if( t.second )
			{
				delete t.second;
				t.second = 0;
			}
		}
	};


	template< typename T >
	struct SeqDeleter
	{
		void operator( ) ( T& t )
		{
			for_each( t.begin(), t.end(), Deleter< T::value_type > ( ) );
			t.clear();
		}
	};

	template< typename T >
	struct AssDeleter
	{
		void operator( ) ( T& t )
		{
			for_each( t.begin(), t.end(), PairDeleter< T::value_type > ( ) );
			t.clear();
		}
	};
}

#endif	//__BS_CONSOLE