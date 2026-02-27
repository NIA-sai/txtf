#pragma once

#include <string>
#include <vector>
inline bool end_with_in( const std::wstring_view &str, const std::vector< std::wstring > &sufixes )
{
	// todo
	return true;
	// for ( auto &suf : sufixes )
	// {
	// 	if ( str.ends_with( suf ) )
	// 		return true;
	// }
	// return false;
}
#include <cwctype>

inline bool isLetter( wchar_t c )
{
	return std::iswalpha( c );
}
inline bool isLetter( char c )
{
	return std::isalpha( c );
}