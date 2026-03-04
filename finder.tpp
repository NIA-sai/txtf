#pragma once
#include "map.hpp"
#include "spliter.hpp"
#include "txt.hpp"
#include "util.cpp"
#include <atomic>
#include <filesystem>
#include <memory>
#include <utility>
#include <vector>
#define Finder_Add_File                                                                     \
	if ( fs::exists( filename ) )                                                           \
		if ( fs::is_regular_file( filename ) && end_with_in( filename, Allow_Extensions ) ) \
			txts.emplace_back( std::make_unique< TXT_File< C > >( filename ) );             \
		else if ( fs::is_directory( filename ) )                                            \
			for ( const auto &entry : fs::directory_iterator( filename ) )                  \
				if ( entry.is_regular_file() && end_with_in( entry.path().string< char >(), Allow_Extensions ) ) txts.emplace_back( std::make_unique< TXT_File< C > >( entry.path().string< wchar_t >() ) );


template < typename T >
std::ostream &operator<<( std::ostream &os, const std::vector< T > &v )
{
	if ( v.empty() ) return os << "[]";
	os << "[ ";
	for ( size_t i = 0; i < v.size() - 1; ++i ) os << v[i] << ",";
	os << v.back() << " ]";
	return os;
}
template < typename C, typename Index_Creator >
struct Finder
{
	template < typename T >
	using ptr = std::unique_ptr< T >;
	template < typename T >
	using vector = std::vector< T >;
	using filename_t = std::string;
	using string_view = std::basic_string_view< C >;
	using string = std::basic_string< C >;
	using map =
	    // std::unordered_map<
	    HashMap< std::basic_string< C >, vector< vector< size_t > > >;
	// todo
	inline static vector< filename_t > Allow_Extensions = { ".txt", ".cpp", ".h", ".md" };
	// size_t count = 0;
	size_t first_unindexed = 0, posCnt = 0;
	size_t first_un_step_indexed = 0;
	vector< ptr< TXT< C > > > txts;
	map index;
	Index_Creator index_creator;
	Finder( const vector< filename_t > &filenames = {}, const vector< string > &texts = {} )
	{
		namespace fs = std::filesystem;
		for ( const auto &filename : filenames ) Finder_Add_File;
		for ( const auto &text : texts )
			txts.emplace_back( std::make_unique< TXT_String< C > >( text ) );
	}
	Finder( const Finder & ) = delete;
	Finder &operator=( const Finder & ) = delete;
	~Finder()
	{
	}
	void add( const filename_t &filename )
	{
		namespace fs = std::filesystem;
		Finder_Add_File
	}

	void add( const vector< filename_t > &filenames )
	{
		namespace fs = std::filesystem;
		for ( const auto &filename : filenames )
			Finder_Add_File
	}
	void addString( const string &text )
	{
		txts.emplace_back( std::make_unique< TXT_String< C > >( text ) );
	}
	void addString( const vector< string > &texts )
	{
		for ( const auto &text : texts )
			txts.emplace_back( std::make_unique< TXT_String< C > >( text ) );
	}
	void select( size_t index )
	{
		if ( index >= txts.size() ) return;
		txts[index]->select();
	}
	void deselect( size_t index )
	{
		if ( index >= txts.size() ) return;
		txts[index]->deselect();
	}
	void delete_( size_t index )
	{
		if ( index >= txts.size() ) return;
		txts[index]->delete_();
	}
	template < typename S = SimpleSpliter >
	void create_index( S &&s = S() )
	{
		for ( ; first_unindexed < txts.size(); ++first_unindexed )
		{
			posCnt += txts[first_unindexed]->create_index( index, txts.size(), index_creator, first_unindexed, std::forward< S >( s ) );
		}
	}

	template < typename S = SimpleSpliter >
	void _create_index( std::atomic< size_t > &doneCount, std::atomic< size_t > &current, S &&s = S() )
	{
		for ( ; first_unindexed < txts.size(); ++first_unindexed )
		{
			//  yield progress
			if ( txts[first_unindexed]->isSelected() )
			{
				current = first_unindexed;
				posCnt += txts[first_unindexed]->create_index( index, txts.size(), index_creator, first_unindexed, std::forward< S >( s ) );
				++doneCount;
			}
		}
	}
	void clear_index()
	{
		posCnt = 0;
		first_unindexed = 0;
		index.clear();
	}
	void restart()
	{
		for ( auto &txt : txts ) txt->restart();
	}
	void step_restart()
	{
		first_un_step_indexed = 0;
		for ( auto &txt : txts ) txt->restart();
	}
	template < typename S = SimpleSpliter >
	bool step_next( S &&s, size_t &start, size_t &end, size_t &txtId, std::basic_string< C > &str )
	{
		while ( first_un_step_indexed < txts.size() && !txts[first_un_step_indexed]->isSelected() )
			++first_un_step_indexed;
		if ( first_un_step_indexed >= txts.size() ) return false;
		if ( s.step( txts[first_un_step_indexed].get(), start, end, str ) )
		{
			txtId = first_un_step_indexed;
			return true;
		}
		++first_un_step_indexed;
		return step_next( std::forward< S >( s ), start, end, txtId, str );
	}

	struct Result
	{
		const vector< vector< size_t > > *results;
		bool hold;
		size_t len_;  // 0 means 复合
		Result() : hold( true ), results( new vector< vector< size_t > >() ), len_( 0 ) {};
		Result( const vector< vector< size_t > > *r, size_t len, bool hold = true ) : hold( hold ), results( r ), len_( len ) {};
		Result( const vector< vector< size_t > > &r, size_t len, const vector< ptr< TXT< C > > > *txts ) : hold( txts != nullptr ), len_( len )
		{
			if ( hold )
			{
				vector< vector< size_t > > resultss;
				resultss.reserve( r.size() );
				for ( const auto &res : r )
					if ( txts->at( res[0] )->isSelected() )
						resultss.emplace_back( res );
				results = new vector< vector< size_t > >( std::move( resultss ) );
			}
			else
				results = &r;
		}

		size_t posCnt() const
		{
			if ( len_ )
				return ( results->size() << 1 ) - 2;
			return results->size() - 1;
		}

		size_t start( size_t i, size_t j ) const
		{
			if ( len_ )
				return results->at( i ).at( j + 1 ) - len_;
			return results->at( i ).at( ( j << 1 ) + 1 );
		}
		size_t end( size_t i, size_t j ) const
		{
			if ( len_ )
				return results->at( i ).at( j + 1 );
			return results->at( i ).at( ( j << 1 ) + 2 );
		}
		~Result()
		{
			if ( hold ) delete results;
		}
		Result( const Result &r ) : hold( r.hold ), len_( r.len_ ), results( r.hold ? new vector< vector< size_t > >( *r.results ) : r.results )
		{
		}
		Result &operator=( const Result &r )
		{
			if ( hold )
				delete results;
			hold = r.hold;
			len_ = r.len_;
			results = r.results;
			if ( hold )
				results = new vector< vector< size_t > >( *results );
			return *this;
		};
		Result &operator=( Result &&r )
		{
			if ( this != &r )
			{
				if ( hold )
					delete results;
				hold = r.hold;
				len_ = r.len_;
				results = r.results;
				r.hold = false;
				r.results = nullptr;
			}
			return *this;
		}
		Result( Result &&r ) : hold( r.hold ), len_( r.len_ ), results( r.results )
		{
			r.hold = false;
		}
		Result operator&&( const Result &other ) const
		{
			if ( results->empty() || other.results->empty() )
				return Result{};
			vector< vector< size_t > > *andr_p = new vector< vector< size_t > >();
			vector< vector< size_t > > &andr = *andr_p;
			Result t = fmt();
			Result o = other.fmt();
			const vector< vector< size_t > > *results = t.results;
			andr.reserve( results->size() <= o.results->size() ? results->size() : o.results->size() );
			size_t i = 0, j = 0, a, b;
			while ( i < results->size() && j < o.results->size() )
			{
				a = results->at( i ).at( 0 );
				b = o.results->at( j ).at( 0 );
				if ( a == b )
				{
					andr.emplace_back( vector< size_t >{ a } );
					auto &v = andr.back();
					v.reserve( results->at( i ).size() + o.results->at( j ).size() - 1 );
					size_t k = 1, l = 1, c, d, e, f, resti;
					while ( k < results->at( i ).size() && l < o.results->at( j ).size() )
					{
						// c = results->at( i ).at( k ), d = other.results->at( j ).at( l );
						// ( e = len_ ? ( c + len_ ) : ( results->at( i ).at( k + 1 ) ) ), ( f = other.len_ ? ( d + other.len_ ) : ( other.results->at( j ).at( l + 1 ) ) );
						c = results->at( i ).at( k ), d = o.results->at( j ).at( l );
						e = results->at( i ).at( k + 1 ), f = o.results->at( j ).at( l + 1 );

						if ( c == d )
						{
							if ( e < f )
								v.insert( v.end(), { c, e, c, f } );
							else
								v.insert( v.end(), { c, f, c, e } );
							k += 2;
							l += 2;
						}
						else if ( c < d )
						{
							v.insert( v.end(), { c, e } );
							k += 2;
						}
						else
						{
							v.insert( v.end(), { d, f } );
							l += 2;
						}
					}
					if ( k >= results->at( i ).size() )
						resti = l;
					else
						resti = k;
					auto &rest = ( k >= results->at( i ).size() ) ? o.results->at( j ) : results->at( i );
					v.insert( v.end(), rest.begin() + resti, rest.end() );

					++i;
					++j;
				}
				else if ( a < b )
					++i;
				else
					++j;
			}
			size_t resti = i < results->size() ? i : j;
			const vector< vector< size_t > > *rest = i < results->size() ? results : o.results;
			andr.insert( andr.end(), rest->begin() + resti, rest->end() );
			// std::cout << *results << "\n"
			//           << *o.results << "\n"
			//           << andr << std::endl;
			return Result{ andr_p, 0, true };
		}
		Result operator||( const Result &other ) const
		{
			if ( results->empty() && other.results->empty() )
				return Result{};
			vector< vector< size_t > > *orr_p = new vector< vector< size_t > >();
			vector< vector< size_t > > &orr = *orr_p;
			// todo
			Result t = fmt();
			Result o = other.fmt();
			const vector< vector< size_t > > *results = t.results;

			orr.reserve( results->size() + o.results->size() );
			size_t i = 0, j = 0, a, b;
			while ( i < results->size() && j < o.results->size() )
			{
				a = results->at( i ).at( 0 );
				b = o.results->at( j ).at( 0 );
				if ( a == b )
				{
					orr.emplace_back( vector< size_t >{ a } );
					auto &v = orr.back();
					v.reserve( results->at( i ).size() + o.results->at( j ).size() - 1 );
					size_t k = 1, l = 1, c, d, e, f, resti;
					while ( k < results->at( i ).size() && l < o.results->at( j ).size() )
					{
						// c = results->at( i ).at( k ), d = other.results->at( j ).at( l );
						// ( e = len_ ? ( c + len_ ) : ( results->at( i ).at( k + 1 ) ) ), ( f = other.len_ ? ( d + other.len_ ) : ( other.results->at( j ).at( l + 1 ) ) );
						c = results->at( i ).at( k ), d = o.results->at( j ).at( l );
						e = results->at( i ).at( k + 1 ), f = o.results->at( j ).at( l + 1 );

						if ( c == d )
						{
							if ( e < f )
								v.insert( v.end(), { c, e, c, f } );
							else
								v.insert( v.end(), { c, f, c, e } );
							k += 2;
							l += 2;
						}
						else if ( c < d )
						{
							v.insert( v.end(), { c, e } );
							k += 2;
						}
						else
						{
							v.insert( v.end(), { d, f } );
							l += 2;
						}
					}
					if ( k >= results->at( i ).size() )
						resti = l;
					else
						resti = k;
					auto &rest = ( k >= results->at( i ).size() ) ? o.results->at( j ) : results->at( i );
					v.insert( v.end(), rest.begin() + resti, rest.end() );
					++i;
					++j;
				}
				else if ( a < b )
					orr.emplace_back( results->at( i++ ) );
				else
					orr.emplace_back( o.results->at( j++ ) );
			}
			return Result{ orr_p, 0, true };
		}
		const vector< vector< size_t > > &array() const
		{
			return *results;
		}
		Result fmt() const
		{
			if ( len_ )
			{
				vector< vector< size_t > > *res_p = new vector< vector< size_t > >( results->size() );
				for ( size_t i = 0; i < results->size(); ++i )
				{
					auto &v = results->at( i );
					auto &res = res_p->at( i );
					res.reserve( v.size() << 1 - 1 );
					res.emplace_back( v.at( 0 ) );
					for ( size_t j = 1; j < v.size(); ++j )
						res.insert( res.end(), { v.at( j ) - len_, v.at( j ) } );
				}
				return Result{ res_p, 0, true };
			}
			return *this;
		}
		const vector< vector< size_t > > &array_fmt()
		{
			if ( len_ )
			{
				vector< vector< size_t > > *res_p = new vector< vector< size_t > >( results->size() );
				for ( size_t i = 0; i < results->size(); ++i )
				{
					auto &v = results->at( i );
					auto &res = res_p->at( i );
					res.reserve( v.size() << 1 );
					res.emplace_back( v.at( 0 ) );
					for ( size_t j = 1; j < v.size(); ++j )
						res.insert( res.end(), { v.at( j ) - len_, v.at( j ) } );
				}
				if ( hold )
					delete results;
				results = res_p;
				hold = true;
				len_ = 0;
			}
			return *results;
		}
		friend std::ostream &operator<<( std::ostream &os, const Result &v )
		{
			return os << *( v.results );
		}
	};
	Result find( const string &text ) const
	{
		if ( index.find( text ) != index.end() )
			return Result{ index.at( text ), text.size(), &txts };
		return Result{};
	}
};
