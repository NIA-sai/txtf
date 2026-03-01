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


int main( int argc, char *argv[] )
{

	SetConsoleOutputCP( CP_UTF8 );
	SetConsoleCP( CP_UTF8 );

	if ( argc < 2 )
		return front_end::run();
	else
	{
		// 命令行模式,后续实现
	}
}
/*
g++ -std=c++20 -g  main.cpp  front_end.cpp  -I "D:/Code/Cpp/imgui-1.92.6" -I "D:/Code/Cpp/imgui-1.92.6/backends" -L "D:/Code/Cpp/imgui-1.92.6/build" -I "D:/Code/Cpp/GLFW/glfw-3.4.bin.WIN64/include" -L "D:/Code/Cpp/GLFW/glfw-3.4.bin.WIN64/lib-mingw-w64" -lglfw3 -lopengl32 -limgui -lgdi32 -luser32 -lshell32 -lcomdlg32 -lole32 -luuid  -o ./output/demo.exe
g++ -std=c++20 -O2 -flto main.cpp  front_end.cpp  -I "D:/Code/Cpp/imgui-1.92.6" -I "D:/Code/Cpp/imgui-1.92.6/backends" -L "D:/Code/Cpp/imgui-1.92.6/build" -I "D:/Code/Cpp/GLFW/glfw-3.4.bin.WIN64/include" -L "D:/Code/Cpp/GLFW/glfw-3.4.bin.WIN64/lib-mingw-w64" -lglfw3 -lopengl32 -limgui -lgdi32 -luser32 -lshell32 -lcomdlg32 -lole32 -luuid "-Wl,--subsystem,windows" app.res -o ./output/txtf.exe
g++ -std=c++20 -g -fsanitize=address main.cpp  front_end.cpp front_end_demo.cpp -I "D:/Code/Cpp/imgui-1.92.6" -I "D:/Code/Cpp/imgui-1.92.6/backends" -L "D:/Code/Cpp/imgui-1.92.6/build" -I "D:/Code/Cpp/GLFW/glfw-3.4.bin.WIN64/include" -L "D:/Code/Cpp/GLFW/glfw-3.4.bin.WIN64/lib-mingw-w64" -lglfw3 -lopengl32 -limgui -lgdi32 -luser32 -lshell32 -lcomdlg32 -lole32 -luuid  app.res -o ./output/txtf4.exe
*/