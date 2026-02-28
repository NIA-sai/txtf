#pragma once
#include <filesystem>
#include <fstream>
template < typename C >
struct TXT
{
	enum struct Status
	{
		Selected,
		Fail_Loaded,
		Not_Selected
	};
	using char_type = C;
	size_t position = 0, last_position = -1;
	Status status = Status::Selected;
	void select()
	{
		if ( status != Status::Fail_Loaded ) status = Status::Selected;
	}
	void deselect()
	{
		if ( status != Status::Fail_Loaded ) status = Status::Not_Selected;
	}
	bool isSelected() const { return status == Status::Selected; }
	template < typename M, typename IC, typename S >
	size_t create_index( M &index, size_t file_cnt, IC &&ic, size_t txt_index, S &&s )
	{
		if ( status == Status::Selected )
		{
			this->restart();
			return ic( index, file_cnt, std::move( s ), txt_index, this );
		}
		return 0;
	}
	virtual void restart() = 0;
	virtual std::wstring getFileName() const = 0;
	virtual bool isRightOpened() const = 0;
	virtual bool readNext() = 0;
	virtual C currentChar() const = 0;
	virtual ~TXT() = default;
};

template < typename C >
struct TXT_File : TXT< C >
{
	std::basic_ifstream< C > file;
	const std::filesystem::path filename;
	C current;
	TXT_File( const std::filesystem::path &filename ) : filename( filename )
	{
		file.open( filename, std::ios::binary );
		if ( !file.is_open() )
			TXT< C >::status = TXT< C >::Status::Fail_Loaded;
	}
	bool isRightOpened() const { return file.is_open(); }
	bool readNext()
	{
		if ( !file.get( current ) )
			return false;
		TXT< C >::last_position = TXT< C >::position;
		TXT< C >::position = file.tellg();
		return true;
	}
	C currentChar() const { return current; }
	std::wstring getFileName() const
	{
		return filename.wstring();
	}
	void restart()
	{
		if ( file.is_open() )
		{
			TXT< C >::position = 0;
			TXT< C >::last_position = -1;
			file.clear();
			file.seekg( 0 );
		}
	}
	~TXT_File()
	{
		file.close();
	}
};
// template <>
// struct TXT_File< char8_t > : TXT< char8_t >
// {
// 	std::ifstream file;
// 	const std::filesystem::path filename;
// 	char current{};

// 	TXT_File( const std::filesystem::path &filename )
// 	    : filename( filename )
// 	{
// 		file.open( filename, std::ios::binary );

// 		if ( !file.is_open() )
// 			TXT< char8_t >::status = TXT< char8_t >::Status::Fail_Loaded;
// 	}

// 	bool isRightOpened() const
// 	{
// 		return file.is_open();
// 	}

// 	bool readNext()
// 	{
// 		if ( !file.get( current ) )
// 			return false;

// 		TXT< char8_t >::last_position = TXT< char8_t >::position;
// 		TXT< char8_t >::position = file.tellg();

// 		return true;
// 	}

// 	char8_t currentChar() const
// 	{
// 		return current;
// 	}

// 	std::wstring getFileName() const
// 	{
// 		return filename.wstring();
// 	}

// 	~TXT_File()
// 	{
// 		if ( file.is_open() )
// 			file.close();
// 	}
// };
template < typename C >
struct TXT_String : TXT< C >
{
	std::basic_string< C > str;
	TXT_String( const std::basic_string< C > &str ) : str( str )
	{
		if ( str.empty() )
			TXT< C >::status = TXT< C >::Status::Fail_Loaded;
	}
	bool isRightOpened() const { return str.empty(); }
	bool readNext()
	{
		if ( TXT< C >::position > str.size() )
			return false;
		++TXT< C >::position;
		++TXT< C >::last_position;
		return true;
	}
	C currentChar() const { return str[TXT< C >::last_position]; }
	void restart()
	{
		TXT< C >::position = 0;
		TXT< C >::last_position = -1;
	}
	std::wstring getFileName() const { return L"temp string"; }
};
