#pragma once
#include <functional>
#include <string>
template < typename K, typename V >
struct Map
{
	V &operator[]( const K &key ) = 0;
	V &at( const K &key ) = 0;
	bool empty() const = 0;
	size_t size() const = 0;
	void clear() = 0;

	Map &operator=( const Map &other ) = 0;
	Map &operator=( Map &&other ) = 0;


	Map( const Map &other ) = 0;
	Map() = 0;
	virtual ~Map() = 0;
};

struct Simple_Hasher
{
	template < typename C >
	size_t operator()( const std::basic_string< C > &s ) const
	{
		size_t hash = 0;
		for ( char c : s )
			hash = hash * 131 + c;
		return hash;
	}
};


template <
    typename K,
    typename V,
    typename Hash = Simple_Hasher,
    typename KeyEqual = std::equal_to< K > >
struct HashMap;
#include "hash_map.tpp"
// #include <unordered_map>
// template < typename K, typename V >
// using HashMap = std::unordered_map< K, V >;