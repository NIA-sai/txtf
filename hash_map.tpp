#pragma once

#include <cstring>
#include <functional>
#include <iterator>
#include <stdexcept>
#include <utility>
#include <vector>

template <
    typename K,
    typename V,
    typename Hash,
    typename KeyEqual >
struct HashMap
{
	struct Bucket
	{
		size_t hash = 0;
		bool occupied = false;
		std::pair< const K, V > *data = nullptr;
		Bucket() = default;
		Bucket( const K &key, const V &value, size_t h, bool o ) : data( new std::pair< const K, V >( key, value ) ), hash( h ), occupied( o ) {}
		~Bucket()
		{
			delete data;
		}
	};
	std::vector< Bucket > table;
	size_t size_ = 0;

	Hash hasher;
	KeyEqual key_equal;
	static constexpr double LOAD_FACTOR = 0.7;
	class iterator
	{
		using Outer = HashMap< K, V, Hash, KeyEqual >;
	private:
		Outer *map;
		size_t index;
		void skip_empty()
		{
			while ( index < map->table.size() &&
			        !map->table[index].occupied )
				++index;
		}
	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type = std::pair< const K &, V & >;
		using difference_type = std::ptrdiff_t;

		iterator( Outer *m, size_t i )
		    : map( m ), index( i )
		{
			if ( map )
			{
				skip_empty();
			}
		}

		iterator &operator++()
		{
			++index;
			skip_empty();

			return *this;
		}

		bool operator==( const iterator &other ) const
		{
			return map == other.map && index == other.index;
		}

		bool operator!=( const iterator &other ) const
		{
			return !( *this == other );
		}

		std::pair< const K, V > &operator*() const
		{
			return *( map->table[index].data );
		}

		std::pair< const K, V > *operator->() const
		{
			return map->table[index].data;
		}
	};



	HashMap( size_t size = 16 )
	{
		reserve( size );
	}

	void reserve( size_t size )
	{
		size_t bucket_count = 1;

		while ( bucket_count < size / LOAD_FACTOR )
			bucket_count <<= 1;

		table.resize( bucket_count );
	}

	size_t size() const { return size_; }

	inline bool empty() const { return size_ == 0; }


	inline size_t probe_index( size_t hash, size_t i ) const
	{
		return ( hash + i ) & ( table.size() - 1 );
	}

	void rehash()
	{
		std::vector< Bucket > old_table = std::move( table );

		table.clear();
		table.resize( old_table.size() * 2 );

		size_ = 0;

		for ( auto &bucket : old_table )
		{
			if ( bucket.occupied )
				insert( bucket.data->first, bucket.data->second );
		}
	}


	iterator find( const K &key ) const
	{
		if ( table.empty() )
			return end();

		size_t hash = hasher( key );
		size_t mask = table.size() - 1;

		for ( size_t i = 0; i < table.size(); ++i )
		{
			size_t idx = probe_index( hash, i );
			auto &b = table[idx];

			if ( !b.occupied )
				return end();

			if ( b.hash == hash && key_equal( b.data->first, key ) )
				return iterator( (HashMap *)this, idx );
		}

		return end();
	}


	std::pair< iterator, bool > insert( const K &key, const V &value )
	{
		if ( table.empty() ||
		     size_ + 1 > table.size() * LOAD_FACTOR )
		{
			rehash();
		}
		size_t hash = hasher( key );
		size_t first_deleted = table.size();
		for ( size_t i = 0; i < table.size(); ++i )
		{
			size_t idx = probe_index( hash, i );
			auto &b = table[idx];

			if ( !b.occupied )
			{
				b.occupied = true;
				b.hash = hash;
				delete b.data;
				b.data = new std::pair< const K, V >( key, value );
				++size_;

				return { iterator( this, idx ), true };
			}

			if ( b.hash == hash && key_equal( b.data->first, key ) )
			{
				b.data->second = value;
				return { iterator( this, idx ), false };
			}
		}

		throw std::runtime_error( "HashMap insert failed" );
	}

	template < typename... Args >
	std::pair< iterator, bool >
	try_emplace( const K &key, Args &&...args )
	{
		if ( table.empty() ||
		     size_ + 1 > table.size() * LOAD_FACTOR )
			rehash();

		size_t hash = hasher( key );

		for ( size_t i = 0; i < table.size(); ++i )
		{
			size_t idx = probe_index( hash, i );
			auto &b = table[idx];

			if ( !b.occupied )
			{
				b.occupied = true;
				b.hash = hash;

				delete b.data;
				b.data = new std::pair< const K, V >( key, V( std::forward< Args >( args )... ) );

				++size_;

				return { iterator( this, idx ), true };
			}

			if ( b.hash == hash && key_equal( b.data->first, key ) )
				return { iterator( this, idx ), false };
		}

		throw std::runtime_error( "hashmap insert fail" );
	}
	V &operator[]( const K &key )
	{
		auto it = find( key );

		if ( it != end() )
			return ( *it ).second;

		auto res = insert( key, V() );
		return res.first.operator*().second;
	}
	V &at( const K &key ) const
	{
		auto it = find( key );
		if ( it != end() )
			return ( *it ).second;
		throw std::out_of_range( "HashMap at failed" );
	}
	iterator begin() const
	{
		return iterator( (HashMap *)this, 0 );
	}
	iterator end() const
	{
		return iterator( (HashMap *)this, table.size() );
	}
	void clear()
	{
		for ( auto &b : table )
		{
			if ( b.occupied )
			{
				b.occupied = false;
			}
		}
		size_ = 0;
	}
};
