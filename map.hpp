#pragma once

template < typename K, typename V >
struct Map
{
	V &operator[]( const K &key ) = 0;
	V &at( const K &key ) = 0;
	bool empty() const = 0;
	unsigned long long size() const = 0;
	void clear() = 0;

	Map &operator=( const Map &other ) = 0;
	Map &operator=( Map &&other ) = 0;


	Map( const Map &other ) = 0;
	Map() = 0;
	virtual ~Map() = 0;
};





#include "hash_map.tpp"
// #include <unordered_map>
// template < typename K, typename V >
// using HashMap = std::unordered_map< K, V >;