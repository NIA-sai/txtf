#pragma once
#include "map.hpp"
#include "txt.hpp"
#include <unordered_map>
#include <vector>
template < typename K >
struct SimpleInvertIndexCreatorWithPos
{
	template < typename T >
	using vector = std::vector< T >;
	using map =  // std::unordered_map
	    HashMap< K, vector< vector< size_t > > >;
	using mmap =  // std::unordered_map
	    HashMap< K, vector< size_t > >;
	template < typename S, typename T >
	size_t operator()( map &m, size_t file_cnt, S &&s, size_t txt_index, T *txt )
	{
		size_t cnt = 0;
		mmap m1 = s( txt_index, txt );
		for ( auto &[k, v] : m1 )
		{
			cnt += v.size() >> 1;
			auto it = m.find( k );
			if ( it == m.end() )
			{
				m.try_emplace( k, vector< vector< size_t > >{} );
				it = m.find( k );
				it->second.reserve( file_cnt );
			}
			it->second.push_back( v );
		}
		return cnt;
	}
};