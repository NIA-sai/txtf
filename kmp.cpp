#include "kmp.hpp"
#include "trie.tpp"
#include <array>
#include <queue>
namespace kmp
{
	ACTrie< char > buildACTrie( const std::vector< std::string > &patterns )
	{
		std::vector< ACNode< char > > trieNodes( 1 );
		std::vector< size_t > patternLens( patterns.size(), 0 );

		for ( size_t i = 0; i < patterns.size(); ++i )
		{
			const auto &p = patterns[i];
			if ( p.empty() ) continue;
			patternLens[i] = p.size();
			int cur = 0;
			for ( unsigned char ch : p )
			{
				if ( trieNodes[cur].next[ch] == -1 )
				{
					trieNodes[cur].next[ch] = static_cast< int >( trieNodes.size() );
					trieNodes.emplace_back();
				}
				cur = trieNodes[cur].next[ch];
			}
			trieNodes[cur].out.push_back( i );
		}

		std::queue< int > q;
		for ( int c = 0; c < 256; ++c )
		{
			int nxt = trieNodes[0].next[c];
			if ( nxt != -1 )
			{
				trieNodes[nxt].fail = 0;
				q.push( nxt );
			}
			else
			{
				trieNodes[0].next[c] = 0;
			}
		}

		while ( !q.empty() )
		{
			int v = q.front();
			q.pop();

			for ( int c = 0; c < 256; ++c )
			{
				int u = trieNodes[v].next[c];
				if ( u != -1 )
				{
					trieNodes[u].fail = trieNodes[trieNodes[v].fail].next[c];
					auto &out = trieNodes[u].out;
					auto &failOut = trieNodes[trieNodes[u].fail].out;
					out.insert( out.end(), failOut.begin(), failOut.end() );
					q.push( u );
				}
				else
				{
					trieNodes[v].next[c] = trieNodes[trieNodes[v].fail].next[c];
				}
			}
		}
		return { trieNodes, patternLens };
	}

	std::vector< MatchOccurrence > findMultiMatches_AhoCorasick(
	    const std::string &text,
	    const std::vector< std::string > &patterns )
	{
		std::vector< MatchOccurrence > occurrences;
		if ( text.empty() || patterns.empty() ) return occurrences;

		auto acTrie = buildACTrie( patterns );

		int state = 0;
		for ( size_t pos = 0; pos < text.size(); ++pos )
		{
			unsigned char ch = static_cast< unsigned char >( text[pos] );
			state = acTrie.trie[state].next[ch];
			for ( size_t patternIndex : acTrie.trie[state].out )
			{
				size_t len = acTrie.patternLens[patternIndex];
				if ( len == 0 || pos + 1 < len ) continue;
				size_t start = pos + 1 - len;
				occurrences.push_back( MatchOccurrence{
				    patternIndex,
				    start,
				    pos + 1 } );
			}
		}

		return occurrences;
	}
}
