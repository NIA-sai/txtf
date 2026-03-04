#pragma once
#include <cstring>
#include <functional>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
struct Hasher
{
	// template < typename C >
	using C = char;
	virtual size_t operator()( const std::basic_string< C > &s ) const = 0;
};


// 131hash
struct Simple_Hasher : public Hasher
{
	// template < typename C >
	using C = char;

	size_t operator()( const std::basic_string< C > &s ) const
	{
		size_t hash = 0;
		for ( C c : s )
			hash = ( hash << 7 ) + ( hash << 1 ) + hash + c;
		return hash;
	}
};
// 31hash
struct Simpler_Hasher : public Hasher
{
	// template < typename C >
	using C = char;

	size_t operator()( const std::basic_string< C > &s ) const
	{
		size_t hash = 0;
		for ( C c : s )
			hash = ( hash << 5 ) - hash + c;
		return hash;
	}
};

struct Bin_Up_Hasher : public Hasher
{
	// template < typename C >
	using C = char;

	size_t operator()( const std::basic_string< C > &s ) const
	{
		size_t hash = 0;
		for ( C c : s )
			hash ^= c << ( hash & ( sizeof( C ) * 8 - 1 ) ) | c >> ( ~hash & ( sizeof( C ) * 8 - 1 ) );
		return hash;
	}
};
struct Shift_Hasher : public Hasher
{
	// template < typename C >
	using C = char;

	size_t operator()( const std::basic_string< C > &s ) const
	{
		size_t hash = 0;
		for ( C c : s )
			hash = ( hash << 1 ) + ( hash >> 63 ) + c;
		return hash;
	}
};

#define HASH_MAP_FIND__( i )                                                                                \
	{                                                                                                       \
		auto &b = data[i];                                                                                  \
		if ( !b.occupied ) return end();                                                                    \
		if ( b.hash == hash && key_equal( b.data->first, key ) ) return iterator( (HashMap *)this, ( i ) ); \
	}

#define HASH_MAP_INSERT__( i )                                   \
	{                                                            \
		auto &b = data[i];                                       \
		if ( !b.occupied )                                       \
		{                                                        \
			b.occupied = true;                                   \
			b.hash = hash;                                       \
			delete b.data;                                       \
			b.data = new std::pair< const K, V >( key, value );  \
			++size_;                                             \
			return { iterator( this, ( i ) ), true };            \
		}                                                        \
		if ( b.hash == hash && key_equal( b.data->first, key ) ) \
		{                                                        \
			b.data->second = value;                              \
			return { iterator( this, ( i ) ), false };           \
		}                                                        \
	}

#define HASH_MAP_TRY_EMPLACE__( i )                                                            \
	{                                                                                          \
		auto &b = data[i];                                                                     \
		if ( !b.occupied )                                                                     \
		{                                                                                      \
			b.occupied = true;                                                                 \
			b.hash = hash;                                                                     \
			delete b.data;                                                                     \
			b.data = new std::pair< const K, V >( key, V( std::forward< Args >( args )... ) ); \
			++size_;                                                                           \
			return { iterator( this, ( i ) ), true };                                          \
		}                                                                                      \
		if ( b.hash == hash && key_equal( b.data->first, key ) )                               \
			return { iterator( this, ( i ) ), false };                                         \
	}
#define HASH_MAP_REHASH_INSERT__( i )      \
	{                                      \
		if ( !data[i].occupied )           \
		{                                  \
			data[i] = std::move( bucket ); \
			goto outer;                    \
		}                                  \
	}


// 奇偶双向线性探索
template <
    typename K,
    typename V,
    typename Hash = Hasher,
    // Simpler_Hasher,
    // Simple_Hasher,
    typename KeyEqual = std::equal_to< K > >
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
		Bucket( const Bucket &other ) : hash( other.hash ), occupied( other.occupied ), data( other.data ? new std::pair< const K, V >( *other.data ) : nullptr ) {}
		Bucket &operator=( const Bucket &other )
		{
			if ( this != &other )
			{
				hash = other.hash;
				occupied = other.occupied;
				delete data;
				data = other.data ? new std::pair< const K, V >( *other.data ) : nullptr;
			}
			return *this;
		}
		Bucket( Bucket &&other ) noexcept : hash( other.hash ), occupied( other.occupied ), data( other.data )
		{
			other.occupied = false;
			other.data = nullptr;
		}

		Bucket &operator=( Bucket &&other )
		{
			if ( this != &other )
			{
				delete data;
				hash = other.hash;
				occupied = other.occupied;
				data = other.data;
				other.occupied = false;
				other.data = nullptr;
			}
			return *this;
		}
	};
	std::vector< Bucket > data;
	size_t size_ = 0;

	Hash *hasher;
	KeyEqual key_equal;
	static constexpr double LOAD_FACTOR = 0.7;
	struct iterator
	{
		using Outer = HashMap< K, V, Hash, KeyEqual >;
		Outer *map;
		size_t index;
		void skip_empty()
		{
			while ( index < map->data.size() &&
			        !map->data[index].occupied )
				++index;
		}
		void skip_empty_r()
		{
			while ( index < map->data.size() &&
			        !map->data[index].occupied )
				++index;
			if ( index == map->data.size() )
			{
				index = 0;
				while ( !map->data[index].occupied )
					++index;
			}
		}
		void skip_empty_r_back()
		{
			while ( index > 0 &&
			        !map->data[index].occupied )
				--index;
			if ( index == 0 && !map->data[index].occupied )
			{
				index = map->data.size() - 1;
				while ( !map->data[index].occupied )
					--index;
			}
		}
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
		iterator( Outer *m, size_t i, bool ) : map( m ), index( i ) {}
		iterator &operator++()
		{
			++index;
			skip_empty();
			return *this;
		}

		void to_next()
		{
			++index;
			skip_empty_r();
		}


		void to_prev()
		{
			if ( index )
				--index;
			else
				index = map->data.size() - 1;
			skip_empty_r_back();
		}
		// iterator operator + ( size_t i ) const
		iterator next() const
		{
			iterator n( this->map, this->index + 1, true );
			n.skip_empty_r();
			return n;
		}
		iterator prev() const
		{
			iterator n( this->map, this->index ? this->index - 1 : this->map->data.size() - 1, true );
			n.skip_empty_r_back();
			return n;
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
			return *( map->data[index].data );
		}

		std::pair< const K, V > *operator->() const
		{
			return map->data[index].data;
		}
	};



	HashMap( size_t size = 16 )
	{
		reserve( size );
		hasher = new Simpler_Hasher();
	}
	void setHasher_( Hash *h )
	{
		delete hasher;
		hasher = h;
	}
	~HashMap()
	{
		delete hasher;
	}
	void reserve( size_t size )
	{
		size_t bucket_count = 1;

		while ( bucket_count < size / LOAD_FACTOR )
			bucket_count <<= 1;

		data.resize( bucket_count );
	}

	size_t size() const { return size_; }

	inline bool empty() const { return size_ == 0; }


	// inline size_t probe_index( size_t hash, size_t i ) const
	// {
	// 	return ( hash + i ) & ( data.size() - 1 );
	// }

	void rehash()
	{
		std::vector< Bucket > old_data = std::move( data );
		data.clear();
		data.resize( old_data.size() << 1 );
		size_t hash_mod;
		for ( auto &bucket : old_data )
		{
			if ( bucket.occupied )
			{
				hash_mod = bucket.hash & ( data.size() - 1 );
				if ( hash_mod & 1 )
				{
					for ( size_t i = hash_mod; i < data.size(); ++i )
						HASH_MAP_REHASH_INSERT__( i )
					for ( size_t i = 0; i < hash_mod; ++i )
						HASH_MAP_REHASH_INSERT__( i )
				}
				else
				{
					for ( size_t i = hash_mod; i > 0; --i )
						HASH_MAP_REHASH_INSERT__( i )
					HASH_MAP_REHASH_INSERT__( 0 )
					for ( size_t i = data.size() - 1; i > hash_mod; --i )
						HASH_MAP_REHASH_INSERT__( i )
				}
			}
outer:;
		}
	}


	inline size_t hash( const K &key ) const
	{
		return hasher->operator()( key );
	}
	iterator hash_to_idx( size_t hash ) const
	{
		return iterator( (HashMap *)this, hash & ( data.size() - 1 ) );
	}
	iterator find( const K &key ) const
	{
		if ( size_ > 0 )
		{
			size_t hash = hasher->operator()( key );
			size_t hash_mod = hash & ( data.size() - 1 );
			if ( hash & 1 )
			{
				for ( size_t i = hash_mod; i < data.size(); ++i )
					HASH_MAP_FIND__( i )
				for ( size_t i = 0; i < hash_mod; ++i )
					HASH_MAP_FIND__( i )
			}
			else
			{
				for ( size_t i = hash_mod; i > 0; --i )
					HASH_MAP_FIND__( i )
				HASH_MAP_FIND__( 0 )
				for ( size_t i = data.size() - 1; i > hash_mod; --i )
					HASH_MAP_FIND__( i )
			}
		}
		return end();
	}


	std::pair< iterator, bool > insert( const K &key, const V &value )
	{
		if ( size_ + 1 > data.size() * LOAD_FACTOR )
			rehash();

		size_t hash = hasher->operator()( key );
		size_t hash_mod = hash & ( data.size() - 1 );
		if ( hash & 1 )
		{
			for ( size_t i = hash_mod; i < data.size(); ++i )
				HASH_MAP_INSERT__( i )
			for ( size_t i = 0; i < hash_mod; ++i )
				HASH_MAP_INSERT__( i )
		}
		else
		{
			for ( size_t i = hash_mod; i > 0; --i )
				HASH_MAP_INSERT__( i )
			HASH_MAP_INSERT__( 0 )
			for ( size_t i = data.size() - 1; i > hash_mod; --i )
				HASH_MAP_INSERT__( i )
		}
		return { end(), false };
	}

	template < typename... Args >
	std::pair< iterator, bool >
	try_emplace( const K &key, Args &&...args )
	{
		if ( size_ + 1 > data.size() * LOAD_FACTOR )
			rehash();

		size_t hash = hasher->operator()( key );
		size_t hash_mod = hash & ( data.size() - 1 );
		if ( hash & 1 )
		{
			for ( size_t i = hash_mod; i < data.size(); ++i )
				HASH_MAP_TRY_EMPLACE__( i )
			for ( size_t i = 0; i < hash_mod; ++i )
				HASH_MAP_TRY_EMPLACE__( i )
		}
		else
		{
			for ( size_t i = hash_mod; i > 0; --i )
				HASH_MAP_TRY_EMPLACE__( i )
			HASH_MAP_TRY_EMPLACE__( 0 )
			for ( size_t i = data.size() - 1; i > hash_mod; --i )
				HASH_MAP_TRY_EMPLACE__( i )
		}
		return { end(), false };
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
		return iterator( (HashMap *)this, data.size() );
	}
	// todo free if too big
	void clear()
	{
		for ( auto &b : data )
		{
			if ( b.occupied )
			{
				b.occupied = false;
			}
		}
		size_ = 0;
	}
};
