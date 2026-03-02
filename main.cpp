/*
auther NIA_sai
front_end draw up by v0
*/
#include "front_end.hpp"

// [[no_use]]
// void manul()
// {
// 	Finder finder( { L"./sadw.txt", L"./2.md", L"./3.cpp" }  // 文件路径 w
// 	               ,
// 	               { "hello!!me" }  // 简单文本(直接参与查询
// 	);
// 	finder.add( L"./4.md" );                                            // 添加文件
// 	finder.add( vector< wstring >{ L"./6.txt", L"./7.txt" } );          // 添加多个文件
// 	finder.addString( "hello!!you" );                                   // 添加简单文本
// 	finder.addString( vector< string >{ "hello!!me", "hello!!you" } );  // 添加多个简单文本
// 	finder.deselect( 1 );                                               // 取消下标为1(文件"./2.md")的文件或文字参与查询
// 	finder.select( 1 );                                                 // 选择下标为1(文件"./2.md")的文件或文字参与查询, 默认加入时即选中
// 	finder.create_index< SingleCharSpliter >();                         // 创建索引
// 	finder.deselect( 1 );                                               // 创建索引后任然可以取消下标为1(文件"./2.md")的文件或文字参与查询

// 	// 查询
// 	auto r1 = finder.find( "中" );
// 	auto r2 = finder.find( "文" );

// 	// 其中结果array()返回一个vector< vector< ull > >,每个vector< ull >表示一个文本,第一个数值为文本在finder里的下标,后续数值为文本中匹配位置(文件按字节位置)
// 	r1.array()[0][0];  // 第一个匹配文本id
// 	r1.array()[0][1];  // 第一个匹配文本的第一个位置
// 	r1.array()[0][2];  // 第一个匹配文本的第二个位置
// 	//...

// 	auto r3 = r1 || r2;  // 查询结果合并,结果为r1和r2的并集
// 	auto r4 = r1 && r2;  // 查询结果合并,结果为r1和r2的交集

// 	finder.index;  // 返回索引对象unordered_map<string,vector<vector<ull>>>
// 	               // 结构类似:"word":[[匹配文本1id,位置1,位置2,...],[匹配文本2id,位置1,位置2,...],...]
// }
std::vector< std::string > split_with_quotes( const std::string &input )
{
	std::vector< std::string > result;
	std::string current;
	bool in_quotes = false;

	for ( size_t i = 0; i < input.size(); ++i )
	{
		char c = input[i];

		if ( c == '"' )
		{
			in_quotes = !in_quotes;
		}
		else if ( std::isspace( c ) && !in_quotes )
		{
			if ( !current.empty() )
			{
				result.push_back( current );
				current.clear();
			}
		}
		else
		{
			current += c;
		}
	}

	if ( !current.empty() )
		result.push_back( current );

	return result;
}
int cli( int argc, char *argv[] )
{
	std::vector< std::string > files;


	for ( int i = 2; i < argc; ++i )
		files.push_back( argv[i] );

	Finder< char > finder( files, {} );
	std::string line;

	while ( true )
	{
		std::cout << "\n>> ";
		std::getline( std::cin, line );

		if ( line == "exit" )
			break;

		if ( line == "--help" || line == "-h" )
		{
			std::cout << "-f file       add file\n"
			             "-t text       add simple text\n"
			             "-q expr       query\n"
			             "exit          exit\n"
			             "-i index      create index\n"
			             "	-s spliter    1:use single char spliter\n"
			             "              	2:(default)use simple spliter\n";
			continue;
		}

		if ( line.rfind( "-f", 0 ) == 0 )
		{
			std::string file = line.substr( 2 );
			finder.add( split_with_quotes( file ) );
			std::cout << "file(s) added.\n";
			continue;
		}
		if ( line.rfind( "--file", 0 ) == 0 )
		{
			std::string file = line.substr( 7 );
			finder.add( split_with_quotes( file ) );
			std::cout << "file(s) added.\n";
			continue;
		}
		if ( line.rfind( "-t ", 0 ) == 0 )
		{
			std::string text = line.substr( 3 );
			finder.addString( text );
			std::cout << "text added.\n";
			continue;
		}

		if ( line.rfind( "--text ", 0 ) == 0 )
		{
			std::string text = line.substr( 7 );
			finder.addString( text );
			std::cout << "text added.\n";
			continue;
		}


		if ( line.rfind( "-i", 0 ) == 0 )
		{
			if ( line.rfind( "-s 1", 0 ) == 0 )
				finder.create_index< SingleCharSpliter >();
			else
				finder.create_index< SimpleSpliter >();
			std::cout << "index created.\n";
			continue;
		}
		if ( line.rfind( "-q ", 0 ) == 0 )
		{
			std::string expr = line.substr( 3 );

			std::istringstream iss( expr );
			std::string token;

			std::vector< std::vector< std::string > > or_groups( 1 );

			while ( iss >> token )
			{
				if ( token == "OR" )
					or_groups.emplace_back();
				else if ( token != "AND" )
					or_groups.back().push_back( token );
			}

			bool first_or = true;
			Finder< char >::Result final_result;

			for ( auto &group : or_groups )
			{
				if ( group.empty() ) continue;

				auto group_result = finder.find( group[0] );

				for ( size_t i = 1; i < group.size(); ++i )
					group_result = group_result && finder.find( group[i] );

				if ( first_or )
				{
					final_result = group_result;
					first_or = false;
				}
				else
				{
					final_result = final_result || group_result;
				}
			}

			auto arr = final_result.array();

			for ( auto &text : arr )
			{
				if ( text.empty() ) continue;

				std::cout << "Text ID: " << text[0] << "\n";

				for ( size_t i = 1; i < text.size(); ++i )
					std::cout << "  Pos: " << text[i] << "\n";
			}

			continue;
		}
	}
	return 0;
}

int main( int argc, char *argv[] )

{
	if ( argc >= 2 )
	{
		if ( std::string( argv[1] ) == "-f" )
			return cli( argc, argv );

		else
		{
			std::cout << "-f file       add file\n"
			             "-t text       add simple text\n"
			             "-q expr       query\n"
			             "  exit       exit\n"
			             "-i index      create index\n"
			             "	-s spliter     1:use single char spliter\n"
			             "              	 2:(default)use simple spliter"
			          << std::endl;
		}

		return 0;
	}
	ShowWindow( GetConsoleWindow(), SW_HIDE );
	return front_end::run();
}
/*
g++ -std=c++20 -g  main.cpp  front_end.cpp  -I "D:/Code/Cpp/imgui-1.92.6" -I "D:/Code/Cpp/imgui-1.92.6/backends" -L "D:/Code/Cpp/imgui-1.92.6/build" -I "D:/Code/Cpp/GLFW/glfw-3.4.bin.WIN64/include" -L "D:/Code/Cpp/GLFW/glfw-3.4.bin.WIN64/lib-mingw-w64" -lglfw3 -lopengl32 -limgui -lgdi32 -luser32 -lshell32 -lcomdlg32 -lole32 -luuid  -o ./output/demo.exe
g++ -std=c++20 -O2 -flto main.cpp  front_end.cpp  front_end_demo.cpp -I "D:/Code/Cpp/imgui-1.92.6" -I "D:/Code/Cpp/imgui-1.92.6/backends" -L "D:/Code/Cpp/imgui-1.92.6/build" -I "D:/Code/Cpp/GLFW/glfw-3.4.bin.WIN64/include" -L "D:/Code/Cpp/GLFW/glfw-3.4.bin.WIN64/lib-mingw-w64" -lglfw3 -lopengl32 -limgui -lgdi32 -luser32 -lshell32 -lcomdlg32 -lole32 -luuid "-Wl,--subsystem,windows" app.res -o ./output/txtf.exe

*/