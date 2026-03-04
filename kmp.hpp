#pragma once

#include <string>
#include <vector>

namespace kmp
{
	struct MatchOccurrence
	{
		size_t patternIndex = 0;
		size_t start = 0;
		size_t end = 0;
	};

	std::vector< MatchOccurrence > findMultiMatches_AhoCorasick(
	    const std::string &text,
	    const std::vector< std::string > &patterns );

	template < typename T >
	struct ACNode
	{
		std::array< int, 1 << 8 * sizeof( T ) > next;
		std::vector< size_t > out;
		int fail = 0;
		ACNode()
		{
			next.fill( -1 );
		}
	};

	template < typename T >
	struct ACTrie
	{
		std::vector< ACNode< T > > trie;
		std::vector< size_t > patternLens;
	};
}
