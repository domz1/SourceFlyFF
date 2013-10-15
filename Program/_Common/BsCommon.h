#ifndef __BS_COMMON_H_
#define __BS_COMMON_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

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

#endif __BS_COMMON_H_