template < typename T, unsigned long long N >
struct StaticQueue
{
	T *_data;
	size_t front, back, size;
	StaticQueue() : _data( new T[N] ), front( 0 ), back( 0 ), size( 0 ) {}
	~StaticQueue() { delete[] _data; }
	void push( const T &value )
	{
		_data[back] = value;
		back = ( back + 1 ) % N;
		if ( size == N )
			front = ( front + 1 ) % N;
		else
			++size;
	}
	bool full() const { return size == N; }
	T &operator[]( unsigned long long i )
	{
		// if ( i >= size )
		//   throw std::out_of_range( "Index out of range" );
		return _data[( front + i ) % N];
	}
	StaticQueue &operator=( const StaticQueue &other )
	{
		if ( this != &other )
		{
			delete[] _data;
			_data = new T[N];
			front = other.front;
			back = other.back;
			size = other.size;
			for ( size_t i = 0; i < size; ++i )
				_data[i] = other._data[i];
		}
		return *this;
	}
	StaticQueue( const StaticQueue &other )
	    : _data( new T[N] ), front( other.front ), back( other.back ), size( other.size )
	{
		for ( size_t i = 0; i < size; ++i )
			_data[i] = other._data[i];
	}
	StaticQueue( StaticQueue &&other )
	    : _data( other._data ), front( other.front ), back( other.back ), size( other.size )
	{
		other._data = nullptr;
		other.front = 0;
		other.back = 0;
		other.size = 0;
	}
	StaticQueue &operator=( StaticQueue &&other )
	{
		if ( this != &other )
		{
			delete[] _data;
			_data = other._data;
			front = other.front;
			back = other.back;
			size = other.size;
			other._data = nullptr;
			other.front = 0;
			other.back = 0;
		}
		return *this;
	}
};
