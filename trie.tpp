#pragma once
#include <array>
#include <vector>
template < typename T >
struct TrieNode
{
	std::array< int, 1 << 8 * sizeof( T ) > next;
	std::vector< size_t > out;
	TrieNode()
	{
		next.fill( -1 );
	}
};
