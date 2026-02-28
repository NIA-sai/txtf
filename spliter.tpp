#pragma once
#include "map.hpp"
#include "txt.hpp"
#include "util.cpp"
#include <iostream>
#include <unordered_map>
#include <vector>
struct SimpleSpliter
{
	constexpr SimpleSpliter() = default;
	template < typename C >
	// std::unordered_map
	HashMap< std::basic_string< C >, std::vector< size_t > >
	operator()( size_t txt_index, TXT< C > *txt )
	{
		// std::unordered_map
		HashMap< std::basic_string< C >, std::vector< size_t > > pos;
		std::basic_string< C > buf{};
		size_t last_position = -1;
		while ( txt->readNext() )
		{
			if ( isLetter( txt->currentChar() ) )
			{
				buf += txt->currentChar();
				if ( last_position == -1 )
					last_position = txt->last_position;
			}
			else
			{
				if ( buf.empty() )
					continue;
				auto &v = pos.try_emplace( buf, std::initializer_list< size_t >{ txt_index } ).first->second;
				v.insert( v.end(), { last_position, txt->last_position } );
				buf.clear();
				last_position = -1;
			}
		}
		if ( !buf.empty() )
		{
			auto &v = pos.try_emplace( buf, std::initializer_list< size_t >{ txt_index } ).first->second;
			v.insert( v.end(), { last_position, txt->last_position } );
		}
		return pos;
	}
	template < typename C >
	bool step( TXT< C > *txt, size_t &start, size_t &end, std::basic_string< C > &str )
	{
		str.clear();
		bool unStart = true;
		while ( txt->readNext() )
		{
			if ( isLetter( txt->currentChar() ) )
			{
				str += txt->currentChar();
				if ( unStart )
				{
					unStart = false;
					start = txt->last_position;
				}
			}
			else
			{
				if ( str.empty() )
					continue;
				end = txt->last_position;
				return true;
			}
		}
		if ( str.empty() )
			return false;
		end = txt->last_position;
		return true;
	}
};
struct SingleCharSpliter
{
	constexpr SingleCharSpliter() = default;
	// std::unordered_map
	template < typename C >

	HashMap< std::string, std::vector< size_t > >
	operator()( size_t txt_index, TXT< C > *txt )
	{
		// std::unordered_map
		HashMap< std::string, std::vector< size_t > > pos;
		size_t last_position;
		while ( txt->readNext() )
		{
			std::string utf8_char;

			unsigned char c = static_cast< unsigned char >( txt->currentChar() );

			unsigned short len = 0;
			if ( c < 0x80 )
				len = 1;
			else if ( ( c & 0xE0 ) == 0xC0 )
				len = 2;
			else if ( ( c & 0xF0 ) == 0xE0 )
				len = 3;
			else if ( ( c & 0xF8 ) == 0xF0 )
				len = 4;
			else
				continue;
			utf8_char.push_back( static_cast< char >( c ) );
			last_position = txt->last_position;
			while ( --len )
			{
				if ( !txt->readNext() )
					break;
				utf8_char.push_back( txt->currentChar() );
			}
			auto &v = pos.try_emplace(
			                 utf8_char,
			                 std::initializer_list< size_t >{ txt_index } )
			              .first->second;
			v.insert( v.end(), { last_position, txt->position } );
		}

		return pos;
	}


	template < typename C >
	bool step( TXT< C > *txt, size_t &start, size_t &end, std::basic_string< C > &str )
	{
		str.clear();
		while ( txt->readNext() )
		{
			std::string utf8_char;

			unsigned char c = static_cast< unsigned char >( txt->currentChar() );

			unsigned short len = 0;
			if ( c < 0x80 )
				len = 1;
			else if ( ( c & 0xE0 ) == 0xC0 )
				len = 2;
			else if ( ( c & 0xF0 ) == 0xE0 )
				len = 3;
			else if ( ( c & 0xF8 ) == 0xF0 )
				len = 4;
			else
				continue;
			utf8_char.push_back( static_cast< char >( c ) );
			start = txt->last_position;
			while ( --len )
			{
				if ( !txt->readNext() )
					break;
			}
			end = txt->position;
			return true;
		}
		return false;
	}
};