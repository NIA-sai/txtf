#define GLFW_EXPOSE_NATIVE_WIN32
#include "front_end.hpp"
#include <GLFW/glfw3native.h>
#include <fstream>
#include <iostream>
#include <thread>

namespace front_end
{


	void ApplyLightTheme()
	{
		using namespace palette;
		ImGuiStyle &st = ImGui::GetStyle();
		ImVec4 *c = st.Colors;

		// ---- 窗口 ----
		c[ImGuiCol_WindowBg] = BgBase;
		c[ImGuiCol_ChildBg] = ImVec4( 0, 0, 0, 0 );
		c[ImGuiCol_PopupBg] = BgPanel;

		// ---- 文本 ----
		c[ImGuiCol_Text] = Text;
		c[ImGuiCol_TextDisabled] = TextDim;

		// ---- 边框 ----
		c[ImGuiCol_Border] = Border;
		c[ImGuiCol_BorderShadow] = ImVec4( 0, 0, 0, 0 );

		// ---- 输入框 / 复选框背景 ----
		// c[ImGuiCol_FrameBg] = ImVec4( 0.94f, 0.95f, 0.96f, 1.0f );
		// c[ImGuiCol_FrameBgHovered] = ImVec4( 0.91f, 0.93f, 0.95f, 1.0f );
		// c[ImGuiCol_FrameBgActive] = ImVec4( 0.87f, 0.89f, 0.92f, 1.0f );
		c[ImGuiCol_FrameBg] = ImVec4( 0.93f, 0.94f, 0.96f, 1.0f );
		c[ImGuiCol_FrameBgHovered] = ImVec4( 0.82f, 0.88f, 0.96f, 1.0f );
		c[ImGuiCol_FrameBgActive] = ImVec4( 0.65f, 0.78f, 0.95f, 1.0f );
		c[ImGuiCol_InputTextCursor] = ImVec4( 0.0f, 0.0f, 0.0f, 1.0f );

		// ---- 标题栏 ----
		c[ImGuiCol_TitleBg] = BgPanel;
		c[ImGuiCol_TitleBgActive] = BgPanel;
		c[ImGuiCol_TitleBgCollapsed] = BgPanel;

		// ---- 菜单栏 ----
		c[ImGuiCol_MenuBarBg] = BgPanel;

		// ---- 滚动条 ----
		c[ImGuiCol_ScrollbarBg] = ImVec4( 0.98f, 0.98f, 0.99f, 1.0f );
		c[ImGuiCol_ScrollbarGrab] = ImVec4( 0.84f, 0.86f, 0.89f, 1.0f );
		c[ImGuiCol_ScrollbarGrabHovered] = ImVec4( 0.74f, 0.77f, 0.81f, 1.0f );
		c[ImGuiCol_ScrollbarGrabActive] = ImVec4( 0.64f, 0.67f, 0.72f, 1.0f );

		// ---- 按钮 ----
		c[ImGuiCol_Button] = Primary;
		c[ImGuiCol_ButtonHovered] = PrimaryHover;
		c[ImGuiCol_ButtonActive] = PrimaryActive;

		// ---- Header (Selectable / TreeNode) ----
		c[ImGuiCol_Header] = PrimaryLight;
		c[ImGuiCol_HeaderHovered] = PrimaryMid;
		c[ImGuiCol_HeaderActive] = ImVec4( 0.22f, 0.55f, 0.87f, 0.35f );

		// ---- 分割线 ----
		c[ImGuiCol_Separator] = Border;
		c[ImGuiCol_SeparatorHovered] = Primary;
		c[ImGuiCol_SeparatorActive] = PrimaryActive;

		// ---- Tab ----
		c[ImGuiCol_Tab] = ImVec4( 0.93f, 0.94f, 0.96f, 1.0f );
		c[ImGuiCol_TabHovered] = Primary;
		c[ImGuiCol_TabActive] = Primary;
		c[ImGuiCol_TabUnfocused] = ImVec4( 0.93f, 0.94f, 0.96f, 1.0f );
		c[ImGuiCol_TabUnfocusedActive] = ImVec4( 0.85f, 0.90f, 0.96f, 1.0f );

		// ---- 勾选 ----
		c[ImGuiCol_CheckMark] = Primary;

		// ---- Slider ----
		c[ImGuiCol_SliderGrab] = Primary;
		c[ImGuiCol_SliderGrabActive] = PrimaryActive;

		// ---- Table ----
		c[ImGuiCol_TableHeaderBg] = ImVec4( 0.94f, 0.95f, 0.97f, 1.0f );
		c[ImGuiCol_TableBorderStrong] = Border;
		c[ImGuiCol_TableBorderLight] = ImVec4( 0.92f, 0.93f, 0.95f, 1.0f );
		c[ImGuiCol_TableRowBg] = ImVec4( 0, 0, 0, 0 );
		c[ImGuiCol_TableRowBgAlt] = ImVec4( 0.97f, 0.97f, 0.98f, 1.0f );

		// ---- 圆角 & 间距 ----
		st.WindowRounding = 0.0f;
		st.ChildRounding = 8.0f;
		st.FrameRounding = 5.0f;
		st.PopupRounding = 6.0f;
		st.ScrollbarRounding = 8.0f;
		st.GrabRounding = 5.0f;
		st.TabRounding = 5.0f;

		st.WindowPadding = ImVec2( 14, 14 );
		st.FramePadding = ImVec2( 10, 6 );
		st.ItemSpacing = ImVec2( 10, 7 );
		st.ItemInnerSpacing = ImVec2( 6, 4 );
		st.ScrollbarSize = 13.0f;

		st.WindowBorderSize = 0.0f;
		st.ChildBorderSize = 1.0f;
		st.FrameBorderSize = 0.0f;
		st.PopupBorderSize = 1.0f;
	}



	void create_index_( AppState &s )
	{
		auto t0 = std::chrono::high_resolution_clock::now();
		switch ( s.splitterMode )
		{
		case AppState::SplitterMode::Simple :
			s.finder->_create_index< SimpleSpliter >( s.downCount, s.currentDocIndex );
			break;
		case AppState::SplitterMode::Single :
			s.finder->_create_index< SingleCharSpliter >( s.downCount, s.currentDocIndex );
			break;
		default :
			break;
		}
		auto t1 = std::chrono::high_resolution_clock::now();
		s.indexTimeMs = std::chrono::duration< double, std::milli >( t1 - t0 ).count();
		s.indexStatus = AppState::IndexStatus::Built;
		s.indexHistory.push_back(
		    IndexRecord{ s.totalCount,
		                 s.finder->index.size(),
		                 s.finder->posCnt,
		                 s.indexTimeMs } );
	}

	void CreateIndex( AppState &s )
	{
		s.totalCount = 0;
		for ( size_t i = 0; i < s.docs.size(); ++i )
		{
			if ( s.docs[i].selected )
			{
				s.finder->select( i );
				if ( s.finder->first_unindexed != -1 && s.finder->first_unindexed <= i )
					++s.totalCount;
			}
			else
				s.finder->deselect( i );
		}
		s.indexStatus = AppState::IndexStatus::Building;

		std::thread( [&s]
		             { create_index_( s ); } )
		    .detach();
	}

	void Badge( const DocEntry &doc, ImVec2 badgeSize )
	{
		if ( doc.isFile )
		{
			ImGui::PushID( &doc );

			bool clicked = false;

			clicked = ImGui::Button( "##filebtn", badgeSize );

			bool hovered = ImGui::IsItemHovered();

			ImVec2 min = ImGui::GetItemRectMin();
			ImVec2 max = ImGui::GetItemRectMax();

			ImDrawList *dl = ImGui::GetWindowDrawList();

			ImVec4 col = hovered ? palette::Primary : palette::TagFile;

			dl->AddRectFilled( min, max,
			                   ImGui::ColorConvertFloat4ToU32( col ),
			                   4.0f );

			const char *text = hovered ? "OPEN" : "FILE";

			ImVec2 textSize = ImGui::CalcTextSize( text );
			ImVec2 center(
			    ( min.x + max.x - textSize.x ) * 0.5f,
			    ( min.y + max.y - textSize.y ) * 0.5f );

			dl->AddText( center,
			             ImGui::GetColorU32( palette::TextWhite ),
			             text );

			if ( clicked )
			{
				ShellExecuteW( nullptr, L"open",
				               doc.filePath.c_str(),
				               nullptr, nullptr, SW_SHOW );
			}

			ImGui::PopID();
		}
		else
		{
			ImGui::PushID( &doc );


			bool clicked = ImGui::Button( "##textbtn", badgeSize );
			bool hovered = ImGui::IsItemHovered();

			ImVec2 min = ImGui::GetItemRectMin();
			ImVec2 max = ImGui::GetItemRectMax();

			ImDrawList *dl = ImGui::GetWindowDrawList();

			ImVec4 col = hovered ? palette::Primary : palette::TagText;

			dl->AddRectFilled( min, max,
			                   ImGui::ColorConvertFloat4ToU32( col ),
			                   4.0f );
			const char *text = hovered ? "COPY" : "TEXT";

			ImVec2 textSize = ImGui::CalcTextSize( text );

			ImVec2 center(
			    ( min.x + max.x - textSize.x ) * 0.5f,
			    ( min.y + max.y - textSize.y ) * 0.5f );

			dl->AddText( center,
			             ImGui::GetColorU32( palette::TextWhite ),
			             text );

			if ( clicked )
			{
				ImGui::SetClipboardText( doc.rawText.c_str() );
			}

			ImGui::PopID();
		}
	}
	void Badge( const char *label, ImVec4 bg )
	{
		ImGui::PushStyleColor( ImGuiCol_Button, bg );
		ImGui::PushStyleColor( ImGuiCol_ButtonHovered, bg );
		ImGui::PushStyleColor( ImGuiCol_ButtonActive, bg );
		ImGui::PushStyleColor( ImGuiCol_Text, palette::TextWhite );
		ImGui::PushStyleVar( ImGuiStyleVar_FrameRounding, 3.0f );
		ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2( 6, 2 ) );
		ImGui::SmallButton( label );
		ImGui::PopStyleVar( 2 );
		ImGui::PopStyleColor( 4 );
	}
	std::string WtoU8( const std::wstring &ws )
	{
		std::string out;
		for ( wchar_t wc : ws )
		{
			if ( wc < 0x80 )
				out += static_cast< char >( wc );
			else if ( wc < 0x800 )
			{
				out += static_cast< char >( 0xC0 | ( wc >> 6 ) );
				out += static_cast< char >( 0x80 | ( wc & 0x3F ) );
			}
			else
			{
				out += static_cast< char >( 0xE0 | ( wc >> 12 ) );
				out += static_cast< char >( 0x80 | ( ( wc >> 6 ) & 0x3F ) );
				out += static_cast< char >( 0x80 | ( wc & 0x3F ) );
			}
		}
		return out;
	}

	std::wstring U8toW( const std::string &s )
	{
		std::wstring out;
		for ( size_t i = 0; i < s.size(); )
		{
			unsigned char ch = s[i];
			wchar_t wc = 0;
			if ( ch < 0x80 )
			{
				wc = ch;
				i += 1;
			}
			else if ( ch < 0xE0 )
			{
				wc = ( ch & 0x1F ) << 6 | ( s[i + 1] & 0x3F );
				i += 2;
			}
			else if ( ch < 0xF0 )
			{
				wc = ( ch & 0x0F ) << 12 | ( s[i + 1] & 0x3F ) << 6 | ( s[i + 2] & 0x3F );
				i += 3;
			}
			else
			{
				wc = L'?';
				i += 4;
			}
			out += wc;
		}
		return out;
	}

//  Windows 原生文件 / 文件夹选择器
#ifdef _WIN32

	// 打开文件选择对话框, 支持多选, 返回选中的文件路径列表
	std::vector< std::wstring > OpenFileDialog( GLFWwindow * /*win*/ )
	{
		std::vector< std::wstring > result;

		// 分配一个大缓冲区存放多选路径
		constexpr DWORD BUF_SZ = 65536;
		std::vector< wchar_t > buf( BUF_SZ, 0 );

		OPENFILENAMEW ofn = {};
		ofn.lStructSize = sizeof( ofn );
		ofn.hwndOwner = nullptr;  // 可用 glfwGetWin32Window(win) 如果链接了 glfw
		ofn.lpstrFilter = L"All Files (*.*)\0*.*\0Text Files (*.txt)\0*.txt\0";
		ofn.lpstrFile = buf.data();
		ofn.nMaxFile = BUF_SZ;
		ofn.lpstrTitle = L"选择文件";
		ofn.Flags = OFN_EXPLORER | OFN_ALLOWMULTISELECT |
		            OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;

		if ( GetOpenFileNameW( &ofn ) )
		{
			// 单选: buf 是完整路径 (以 \0 结尾)
			// 多选: buf = 目录 \0 文件1 \0 文件2 \0 ... \0\0
			const wchar_t *p = buf.data();
			std::wstring dir = p;
			p += dir.size() + 1;

			if ( *p == L'\0' )
			{
				// 只选了一个文件
				result.push_back( dir );
			}
			else
			{
				// 多选: dir 是目录, 后续每一段是文件名
				while ( *p )
				{
					std::wstring fname = p;
					result.push_back( dir + L"\\" + fname );
					p += fname.size() + 1;
				}
			}
		}
		return result;
	}

	// 打开文件夹选择对话框, 返回文件夹路径 (空表示取消)
	std::wstring OpenFolderDialog( GLFWwindow * /*win*/ )
	{
		std::wstring result;

		// 使用 IFileOpenDialog (Vista+) 以获得现代风格
		CoInitializeEx( nullptr, COINIT_APARTMENTTHREADED );

		IFileOpenDialog *pDialog = nullptr;
		HRESULT hr = CoCreateInstance( CLSID_FileOpenDialog, nullptr,
		                               CLSCTX_INPROC_SERVER, IID_PPV_ARGS( &pDialog ) );

		if ( SUCCEEDED( hr ) )
		{
			DWORD options = 0;
			pDialog->GetOptions( &options );
			pDialog->SetOptions( options | FOS_PICKFOLDERS | FOS_NOCHANGEDIR );
			pDialog->SetTitle( L"选择文件夹" );

			hr = pDialog->Show( nullptr );
			if ( SUCCEEDED( hr ) )
			{
				IShellItem *pItem = nullptr;
				hr = pDialog->GetResult( &pItem );
				if ( SUCCEEDED( hr ) )
				{
					wchar_t *path = nullptr;
					pItem->GetDisplayName( SIGDN_FILESYSPATH, &path );
					if ( path )
					{
						result = path;
						CoTaskMemFree( path );
					}
					pItem->Release();
				}
			}
			pDialog->Release();
		}

		CoUninitialize();
		return result;
	}

	// 递归枚举文件夹下所有文件路径
	void EnumFilesRecursive( const std::wstring &dir, std::vector< std::wstring > &out )
	{
		WIN32_FIND_DATAW fd;
		HANDLE h = FindFirstFileW( ( dir + L"\\*" ).c_str(), &fd );
		if ( h == INVALID_HANDLE_VALUE ) return;
		do
		{
			std::wstring name = fd.cFileName;
			if ( name == L"." || name == L".." ) continue;
			std::wstring full = dir + L"\\" + name;
			if ( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
				EnumFilesRecursive( full, out );
			else
				out.push_back( full );
		}
		while ( FindNextFileW( h, &fd ) );
		FindClose( h );
	}

#endif  // _WIN32


	std::string ReadDocContent( AppState &s, size_t docIndex, size_t start, size_t end, int len, size_t &newStart, size_t &newEnd )
	{
		if ( docIndex >= s.docs.size() ) return "";
		if ( start >= end ) return "";
		auto &doc = s.docs[docIndex];
		size_t startByte = start >= ( 4 + len << 2 ) ? start - ( 4 + len << 2 ) : 0;
		size_t endByte;
		std::string content;
		if ( doc.isFile )
		{
			std::string path = WtoU8( doc.filePath );
			std::ifstream ifs( path, std::ios::binary );
			if ( !ifs.is_open() )
			{
				newStart = 0;
				newEnd = 0;
				return "[无法读取文件]";
			}
			ifs.seekg( 0, std::ios::end );
			size_t fileLen = ifs.tellg();
			endByte = end + ( 4 + len << 2 );
			if ( end > fileLen ) end = fileLen;
			if ( endByte > fileLen ) endByte = fileLen;
			ifs.seekg( startByte );
			content.resize( endByte - startByte );
			ifs.read( &content[0], endByte - startByte );
			ifs.close();
		}
		else
		{
			endByte = end + ( 4 + len << 2 );
			if ( end > doc.rawText.size() ) end = doc.rawText.size();
			if ( endByte > doc.rawText.size() ) endByte = doc.rawText.size();
			content = doc.rawText.substr( startByte, endByte - startByte );
		}
		int beforeLen = len;
		size_t startByte1 = start - startByte;
		while ( beforeLen > 0 && startByte1 > 0 )
		{
			--startByte1;
			while ( startByte1 > 0 && ( content[startByte1] & 0xC0 ) == 0x80 )
				--startByte1;
			--beforeLen;
		}
		int afterLen = len;
		size_t endByte1 = end - startByte;
		while ( afterLen > 0 && endByte1 < content.size() )
		{
			unsigned char ch = content[endByte1];
			if ( ch < 0x80 )
				++endByte1;
			else if ( ch < 0xE0 )
				endByte1 += 2;
			else if ( ch < 0xF0 )
				endByte1 += 3;
			else
				endByte1 += 4;
			if ( endByte1 > content.size() ) endByte1 = content.size();
			--afterLen;
		}
		newStart = start - startByte - startByte1;
		newEnd = end - startByte - startByte1;


		return content.substr( startByte1, endByte1 - startByte1 );
	}

	void ShowContextSnippet( std::string content, size_t start,
	                         size_t end )
	{
		for ( auto &c : content )
			if ( c == '\n' || c == '\r' ) c = ' ';


		ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2( 6, 3 ) );

		ImDrawList *draw = ImGui::GetWindowDrawList();
		ImVec2 pos = ImGui::GetCursorScreenPos();
		float line_start_x = pos.x;
		float wrap_x = pos.x + ImGui::GetContentRegionAvail().x;
		ImVec2 size;
		for ( size_t i = 0; i < content.size(); ++i )
		{
			const char *endc = i + 1 == content.size() ? nullptr : &content[i + 1];
			size = ImGui::CalcTextSize( &content[i],
			                            endc );
			if ( pos.x + size.x > wrap_x )
			{
				pos.x = line_start_x;
				pos.y += size.y;
			}
			if ( i >= start && i < end )
			{
				draw->AddRectFilled( pos, ImVec2( pos.x + size.x, pos.y + size.y ), ImGui::ColorConvertFloat4ToU32( palette::Highlight ) );
				draw->AddText( pos, ImGui::ColorConvertFloat4ToU32( palette::Error ), &content[i], endc );
			}
			else
				draw->AddText( pos, ImGui::ColorConvertFloat4ToU32( palette::TextDim ), &content[i], endc );
			pos.x += size.x;
		}
		ImGui::Dummy( ImVec2( 0, pos.y - ImGui::GetCursorScreenPos().y + ( pos.x > line_start_x ? size.y : 0 ) ) );
		// ImGui::PushStyleColor( ImGuiCol_Text, palette::TextDim );
		// ImGui::PushTextWrapPos( ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x );

		// if ( start )
		// {
		// 	ImGui::TextUnformatted( content.substr( 0, start ).c_str() );
		// 	ImGui::SameLine( 0, 0 );
		// }
		// ImGui::PopStyleColor();

		// const char *match = content.substr( start, end - start ).c_str();
		// ImVec2 p = ImGui::GetCursorScreenPos();
		// ImVec2 textSize = ImGui::CalcTextSize( match );
		// ImGui::GetWindowDrawList()->AddRectFilled(
		//     ImVec2( p.x - 2, p.y - 1 ),
		//     ImVec2( p.x + textSize.x + 2, p.y + textSize.y + 1 ),
		//     ImGui::ColorConvertFloat4ToU32( palette::Highlight ),
		//     3.0f );
		// ImGui::PushStyleColor( ImGuiCol_Text, palette::Error );
		// ImGui::TextUnformatted( match );
		// ImGui::PopStyleColor();
		// ImGui::SameLine( 0, 0 );

		// ImGui::PushStyleColor( ImGuiCol_Text, palette::TextDim );
		// if ( end - start )
		// 	ImGui::TextUnformatted( content.substr( end ).c_str() );
		// else
		// 	ImGui::TextUnformatted( " " );  // 占位
		// ImGui::PopTextWrapPos();
		// ImGui::PopStyleColor();

		ImGui::PopStyleVar();
	}




	void RenderDocPanel( AppState &s )
	{
		ImGui::SameLine();
		ImGui::Text( "文档管理" );
		ImGui::Separator();
		ImGui::Spacing();

		// ---- 操作按钮 ----
		float btnW = ( ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x ) * 0.5f;

		ImGui::PushStyleColor( ImGuiCol_Text, palette::TextWhite );
		if ( ImGui::Button( "+ 添加文件", ImVec2( btnW, 0 ) ) )
			s.popAddFile = true;
		ImGui::SameLine();
		ImGui::PushStyleColor( ImGuiCol_Button, palette::Success );
		ImGui::PushStyleColor( ImGuiCol_ButtonHovered, ImVec4( 0.16f, 0.68f, 0.42f, 1.0f ) );
		ImGui::PushStyleColor( ImGuiCol_ButtonActive, ImVec4( 0.14f, 0.60f, 0.38f, 1.0f ) );
		if ( ImGui::Button( "+ 添加文本", ImVec2( btnW, 0 ) ) )
			s.popAddText = true;
		ImGui::PopStyleColor( 3 );
		ImGui::PopStyleColor();  // TextWhite

		ImGui::Spacing();

		if ( !s.docs.empty() )
		{
			float fullW = ImGui::GetContentRegionAvail().x;
			if ( s.indexStatus == AppState::IndexStatus::Built )
			{
				ImGui::PushStyleColor( ImGuiCol_Button, palette::Success );
				ImGui::PushStyleColor( ImGuiCol_ButtonHovered, palette::Success );
				ImGui::PushStyleColor( ImGuiCol_ButtonActive, palette::Success );
			}
			else if ( s.indexStatus == AppState::IndexStatus::Building )
			{
				ImGui::PushStyleColor( ImGuiCol_Button, palette::Warning );
				ImGui::PushStyleColor( ImGuiCol_ButtonHovered, ImVec4( 0.90f, 0.70f, 0.15f, 1.0f ) );
				ImGui::PushStyleColor( ImGuiCol_ButtonActive, ImVec4( 0.84f, 0.64f, 0.10f, 1.0f ) );
			}
			else
			{
				ImGui::PushStyleColor( ImGuiCol_Button, palette::Primary );
				ImGui::PushStyleColor( ImGuiCol_ButtonHovered, palette::PrimaryHover );
				ImGui::PushStyleColor( ImGuiCol_ButtonActive, palette::PrimaryActive );
			}
			ImGui::PushStyleColor( ImGuiCol_Text, palette::TextWhite );
			char *label = nullptr;
			switch ( s.indexStatus )
			{
			case AppState::IndexStatus::Built :
				label = "[ok] 索引已建立";
				break;
			case AppState::IndexStatus::Building :
				label = "[wait] 正在创建索引...";
				break;
			default :
				label = ">> 创建索引";
				break;
			}
			if ( ImGui::Button( label, ImVec2( fullW, 0 ) ) && ( s.indexStatus == AppState::IndexStatus::NotBuilt || s.indexStatus == AppState::IndexStatus::Append ) )
				CreateIndex( s );
			ImGui::PopStyleColor( 4 );
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		{
			char buf[64];
			snprintf( buf, sizeof( buf ), "文档列表  (%zu)", s.docs.size() );
			ImGui::Text( "%s", buf );
		}
		ImGui::Spacing();

		if ( !s.docs.empty() )
		{
			if ( ImGui::SmallButton( "全选" ) )
				for ( auto &d : s.docs ) d.selected = true;
			ImGui::SameLine();
			if ( ImGui::SmallButton( "全不选" ) )
				for ( auto &d : s.docs ) d.selected = false;
			ImGui::SameLine();
			ImGui::TextDisabled( "| 勾选参与查询" );
			ImGui::Spacing();
		}

		ImGui::BeginChild( "##DocList", ImVec2( 0, 0 ), ImGuiChildFlags_None );
		float lineH = ImGui::GetFrameHeight();
		const ImVec2 badgeSize = ImGui::CalcTextSize( "OPEN" );
		float offsetY = ( lineH - badgeSize.y ) * 0.5f;

		for ( size_t i = 0; i < s.docs.size(); ++i )
		{
			auto &doc = s.docs[i];
			ImGui::PushID( static_cast< int >( i ) );

			bool prev = doc.selected;
			ImGui::Checkbox( "##sel", &doc.selected );
			if ( prev != doc.selected )
			{
				if ( doc.selected )
					s.finder->select( i );
				else
					s.finder->deselect( i );
			}
			ImGui::SameLine();

			// if ( doc.isFile )
			// 	Badge( "FILE", palette::TagFile );
			// else
			// 	Badge( "TEXT", palette::TagText );
			// ImGui::SameLine();

			ImGui::SetCursorPosY( ImGui::GetCursorPosY() + offsetY );
			Badge( doc, badgeSize );

			ImGui::SameLine();

			if ( !doc.selected )
				ImGui::PushStyleColor( ImGuiCol_Text, palette::TextDim );
			ImGui::Text( "%zu: %s", i, doc.displayName.c_str() );
			if ( !doc.selected )
				ImGui::PopStyleColor();

			ImGui::PopID();
		}
		ImGui::EndChild();
	}

	void RenderSearchPanel( AppState &s )
	{
		ImGui::Text( "查询" );
		ImGui::Separator();
		ImGui::Spacing();

		if ( s.queryTerms.empty() )
			s.queryTerms.push_back( QueryTerm{} );

		float availW = ImGui::GetContentRegionAvail().x;
		bool enterHit = false;

		for ( size_t i = 0; i < s.queryTerms.size(); ++i )
		{
			ImGui::PushID( static_cast< int >( i ) );

			if ( i > 0 )
			{
				auto &prevTerm = s.queryTerms[i - 1];


				ImGui::PushStyleVar( ImGuiStyleVar_FrameRounding, 3.0f );
				ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2( 8, 3 ) );

				bool isAnd = ( prevTerm.op == 0 );
				if ( isAnd )
				{
					ImGui::PushStyleColor( ImGuiCol_Button, palette::Primary );
					ImGui::PushStyleColor( ImGuiCol_ButtonHovered, palette::PrimaryHover );
					ImGui::PushStyleColor( ImGuiCol_ButtonActive, palette::PrimaryActive );
					ImGui::PushStyleColor( ImGuiCol_Text, palette::TextWhite );
				}
				else
				{
					ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 0.92f, 0.93f, 0.95f, 1.0f ) );
					ImGui::PushStyleColor( ImGuiCol_ButtonHovered, ImVec4( 0.86f, 0.88f, 0.92f, 1.0f ) );
					ImGui::PushStyleColor( ImGuiCol_ButtonActive, ImVec4( 0.80f, 0.83f, 0.88f, 1.0f ) );
					ImGui::PushStyleColor( ImGuiCol_Text, palette::TextSub );
				}
				if ( ImGui::SmallButton( "AND" ) )
					prevTerm.op = 0;
				ImGui::PopStyleColor( 4 );

				ImGui::SameLine();

				bool isOr = ( prevTerm.op == 1 );
				if ( isOr )
				{
					ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 0.80f, 0.45f, 0.10f, 0.85f ) );
					ImGui::PushStyleColor( ImGuiCol_ButtonHovered, ImVec4( 0.75f, 0.40f, 0.08f, 0.90f ) );
					ImGui::PushStyleColor( ImGuiCol_ButtonActive, ImVec4( 0.70f, 0.35f, 0.06f, 0.95f ) );
					ImGui::PushStyleColor( ImGuiCol_Text, palette::TextWhite );
				}
				else
				{
					ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 0.92f, 0.93f, 0.95f, 1.0f ) );
					ImGui::PushStyleColor( ImGuiCol_ButtonHovered, ImVec4( 0.86f, 0.88f, 0.92f, 1.0f ) );
					ImGui::PushStyleColor( ImGuiCol_ButtonActive, ImVec4( 0.80f, 0.83f, 0.88f, 1.0f ) );
					ImGui::PushStyleColor( ImGuiCol_Text, palette::TextSub );
				}
				if ( ImGui::SmallButton( "OR" ) )
					prevTerm.op = 1;
				ImGui::PopStyleColor( 4 );

				ImGui::PopStyleVar( 2 );
			}

			{
				float removeBtnW = ( s.queryTerms.size() > 1 ) ? 12.0f : 0.0f;
				float spacing = ( removeBtnW > 1.0f ) ? ImGui::GetStyle().ItemSpacing.x : 0.0f;
				float inputW = ImGui::GetContentRegionAvail().x - removeBtnW - spacing * 5;

				char hint[64];
				snprintf( hint, sizeof( hint ), "查询词 %zu ...", i + 1 );
				ImGui::SetNextItemWidth( inputW );
				if ( ImGui::InputTextWithHint( "##word", hint, s.queryTerms[i].word,
				                               sizeof( s.queryTerms[i].word ), ImGuiInputTextFlags_EnterReturnsTrue ) )
				{
					enterHit = true;
				}

				if ( s.queryTerms.size() > 1 )
				{
					ImGui::SameLine();
					ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 0.95f, 0.90f, 0.90f, 1.0f ) );
					ImGui::PushStyleColor( ImGuiCol_ButtonHovered, ImVec4( 0.92f, 0.82f, 0.82f, 1.0f ) );
					ImGui::PushStyleColor( ImGuiCol_ButtonActive, ImVec4( 0.88f, 0.74f, 0.74f, 1.0f ) );
					ImGui::PushStyleColor( ImGuiCol_Text, palette::Error );
					if ( ImGui::SmallButton( " × " ) )
					{
						ImGui::PopStyleColor( 4 );
						ImGui::PopID();
						s.queryTerms.erase( s.queryTerms.begin() + i );
						return;
					}
					ImGui::PopStyleColor( 4 );
				}
			}

			ImGui::PopID();
		}

		ImGui::Spacing();
		ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 0.90f, 0.94f, 1.00f, 1.0f ) );
		ImGui::PushStyleColor( ImGuiCol_ButtonHovered, ImVec4( 0.82f, 0.88f, 0.98f, 1.0f ) );
		ImGui::PushStyleColor( ImGuiCol_ButtonActive, ImVec4( 0.74f, 0.82f, 0.96f, 1.0f ) );
		ImGui::PushStyleColor( ImGuiCol_Text, palette::Primary );
		if ( ImGui::SmallButton( " + " ) )
		{
			s.queryTerms.push_back( QueryTerm{} );
		}
		ImGui::PopStyleColor( 4 );

		if ( enterHit ) DoSearch( s );

		ImGui::Spacing();

		{
			bool canSearch = s.indexStatus != AppState::IndexStatus::NotBuilt &&
			                 s.indexStatus != AppState::IndexStatus::Building && std::strlen( s.queryTerms[0].word ) > 0;
			if ( !canSearch ) ImGui::BeginDisabled();

			ImGui::PushStyleColor( ImGuiCol_Text, palette::TextWhite );
			float searchBtnW = 100.0f;
			if ( ImGui::Button( "搜索", ImVec2( searchBtnW, 0 ) ) )
				DoSearch( s );
			ImGui::PopStyleColor();

			if ( !canSearch ) ImGui::EndDisabled();

			ImGui::SameLine();

			if ( s.indexStatus == AppState::IndexStatus::NotBuilt && !s.docs.empty() )
			{
				ImGui::PushStyleColor( ImGuiCol_Text, palette::Warning );
				ImGui::Text( "* 请先创建索引" );
				ImGui::PopStyleColor();
				ImGui::SameLine();
			}

			ImGui::SameLine();
			ImGui::PushStyleColor( ImGuiCol_Text, palette::TextSub );
			ImGui::Text( "上下文:" );
			ImGui::PopStyleColor();
			ImGui::SameLine();
			ImGui::SetNextItemWidth( ImGui::GetContentRegionAvail().x );
			ImGui::SliderInt( "##ctx", &s.contextChars, 2, 200, "%d 字符" );
		}
	}



	void DoSearch( AppState &s )
	{
		if ( s.indexStatus == AppState::IndexStatus::Building ) return;
		if ( s.queryTerms.empty() || std::strlen( s.queryTerms[0].word ) == 0 ) return;

		s.results.clear();
		for ( size_t i = 0; i < s.docs.size(); ++i )
		{
			if ( s.docs[i].selected )
				s.finder->select( i );
			else
				s.finder->deselect( i );
		}
		std::string qDesc;
		std::string modeStr;

		struct ValidTerm
		{
			std::string word;
			int op;
		};
		std::vector< ValidTerm > terms;
		for ( auto &qt : s.queryTerms )
		{
			if ( std::strlen( qt.word ) == 0 ) continue;
			terms.push_back( { std::string( qt.word ), qt.op } );
		}
		if ( terms.empty() ) return;

		for ( size_t i = 0; i < terms.size(); ++i )
		{
			if ( i > 0 )
				qDesc += ( terms[i - 1].op == 0 ) ? " AND " : " OR ";
			qDesc += terms[i].word;
		}

		if ( terms.size() == 1 )
			modeStr = "SINGLE";
		else
		{
			bool hasAnd = false, hasOr = false;
			for ( size_t i = 0; i + 1 < terms.size(); ++i )
			{
				if ( terms[i].op == 0 )
					hasAnd = true;
				else
					hasOr = true;
			}
			if ( hasAnd && hasOr )
				modeStr = "MIXED";
			else if ( hasAnd )
				modeStr = "AND";
			else
				modeStr = "OR";
		}

		auto t0 = std::chrono::high_resolution_clock::now();

		auto accumulated = s.finder->find( terms[0].word );
		for ( size_t i = 1; i < terms.size(); ++i )
		{
			auto ri = s.finder->find( terms[i].word );
			if ( terms[i - 1].op == 0 )
				accumulated = accumulated && ri;
			else
				accumulated = accumulated || ri;
		}

		auto &arr = accumulated.array();
		// std::cout << arr;
		for ( auto &v : arr )
		{
			if ( v.empty() ) continue;
			ResultEntry re;
			re.docIndex = static_cast< size_t >( v[0] );
			if ( re.docIndex < s.docs.size() )
				re.docName = std::to_string( re.docIndex ) + ": " + s.docs[re.docIndex].displayName;
			else
				re.docName = "Doc#" + std::to_string( re.docIndex );
			for ( size_t k = 1; k < v.size(); ++k )
				re.positions.push_back( static_cast< size_t >( v[k] ) );
			re.currentPos = 0;
			s.results.push_back( std::move( re ) );
		}

		auto t1 = std::chrono::high_resolution_clock::now();
		s.lastQueryMs = std::chrono::duration< double, std::milli >( t1 - t0 ).count();
		s.lastQueryStr = qDesc;
		s.searched = true;

		int totalHits = 0;
		for ( auto &r : s.results ) totalHits += static_cast< int >( r.positions.size() >> 1 );
		s.perfHistory.push_back( PerfRecord{
		    qDesc, modeStr.c_str(), s.lastQueryMs,
		    static_cast< int >( s.results.size() ), totalHits } );
	}
	void ShowTxtInMiddle( const char *txt )
	{
		float w = ImGui::CalcTextSize( txt ).x;
		ImGui::SetCursorPosX( ( ImGui::GetContentRegionAvail().x - w ) * 0.5f + ImGui::GetCursorPosX() );
		ImGui::PushStyleColor( ImGuiCol_Text, palette::TextDim );
		ImGui::Text( txt );
		ImGui::PopStyleColor();
		return;
	}

	void RenderResultPanel( AppState &s )
	{
		if ( !s.searched )
		{
			ImGui::Spacing();
			ShowTxtInMiddle( "请输入查询词后点击搜索" );
			return;
		}

		ImGui::PushStyleColor( ImGuiCol_Text, palette::TextSub );
		ImGui::Text( "查询: " );
		ImGui::PopStyleColor();
		ImGui::SameLine();
		ImGui::PushStyleColor( ImGuiCol_Text, palette::Primary );
		ImGui::Text( "%s", s.lastQueryStr.c_str() );
		ImGui::PopStyleColor();
		ImGui::SameLine();
		ImGui::PushStyleColor( ImGuiCol_Text, palette::TextDim );
		ImGui::Text( "  |  %zu 个文档匹配  |  %.5f ms", s.results.size(), s.lastQueryMs );
		ImGui::PopStyleColor();

		if ( !s.results.empty() )
		{
			ImGui::SameLine();
			float btnX = ImGui::GetContentRegionAvail().x;
			float btnW = ImGui::CalcTextSize( "一键勾选" ).x + 20.0f;
			if ( btnX > btnW )
				ImGui::SetCursorPosX( ImGui::GetCursorPosX() + btnX - btnW );

			ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 0.90f, 0.94f, 1.00f, 1.0f ) );
			ImGui::PushStyleColor( ImGuiCol_ButtonHovered, ImVec4( 0.82f, 0.88f, 0.98f, 1.0f ) );
			ImGui::PushStyleColor( ImGuiCol_ButtonActive, ImVec4( 0.74f, 0.82f, 0.96f, 1.0f ) );
			ImGui::PushStyleColor( ImGuiCol_Text, palette::Primary );
			if ( ImGui::SmallButton( "一键勾选" ) )
			{
				for ( auto &d : s.docs ) d.selected = false;
				for ( auto &r : s.results )
				{
					if ( r.docIndex < s.docs.size() )
						s.docs[r.docIndex].selected = true;
				}
				for ( size_t i = 0; i < s.docs.size(); ++i )
				{
					if ( s.docs[i].selected )
						s.finder->select( i );
					else
						s.finder->deselect( i );
				}
			}
			ImGui::PopStyleColor( 4 );
		}

		ImGui::Separator();
		ImGui::Spacing();

		if ( s.results.empty() )
		{
			ImGui::PushStyleColor( ImGuiCol_Text, palette::TextDim );
			ImGui::Text( "未找到匹配结果。" );
			ImGui::PopStyleColor();
			return;
		}


		float lineH = ImGui::GetFrameHeight();
		const ImVec2 badgeSize = ImGui::CalcTextSize( "OPEN" );
		float offsetY = ( lineH - badgeSize.y ) * 0.5f;
		for ( size_t i = 0; i < s.results.size(); ++i )
		{
			auto &r = s.results[i];
			if ( r.docIndex >= s.docs.size() )
				break;
			ImGui::PushID( static_cast< int >( i ) );

			ImGui::PushStyleColor( ImGuiCol_ChildBg, ImVec4( 0.97f, 0.98f, 0.99f, 1.0f ) );
			float rowH = ImGui::GetTextLineHeightWithSpacing() * 3.6f;
			ImGui::BeginChild( "##row", ImVec2( 0, rowH ), ImGuiChildFlags_Borders );
			{
				Badge( s.docs[r.docIndex], badgeSize );
				ImGui::SameLine();
				ImGui::SetCursorPosY( ImGui::GetCursorPosY() - offsetY );

				ImGui::PushStyleColor( ImGuiCol_Text, palette::Primary );
				ImGui::Text( "%s", r.docName.c_str() );
				ImGui::PopStyleColor();
				ImGui::SameLine();
				ImGui::SetCursorPosY( ImGui::GetCursorPosY() - offsetY );

				ImGui::PushStyleColor( ImGuiCol_Text, palette::TextDim );
				ImGui::Text( "  (%zu 处匹配)", r.positions.size() >> 1 );
				ImGui::PopStyleColor();


				if ( !r.positions.empty() )
				{
					ImGui::SameLine();

					ImGui::Spacing();
					ImGui::SameLine();
					ImGui::SetCursorPosY( ImGui::GetCursorPosY() - offsetY );

					ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 0.90f, 0.92f, 0.95f, 1.0f ) );
					ImGui::PushStyleColor( ImGuiCol_ButtonHovered, ImVec4( 0.82f, 0.86f, 0.92f, 1.0f ) );
					ImGui::PushStyleColor( ImGuiCol_ButtonActive, ImVec4( 0.74f, 0.78f, 0.86f, 1.0f ) );
					ImGui::PushStyleColor( ImGuiCol_Text, palette::Text );

					bool atFirst = ( r.currentPos <= 0 );
					if ( atFirst ) ImGui::BeginDisabled();
					if ( ImGui::SmallButton( " < " ) )
					{
						if ( r.currentPos > 0 ) --r.currentPos;
					}
					if ( atFirst ) ImGui::EndDisabled();

					ImGui::SameLine();
					ImGui::SetCursorPosY( ImGui::GetCursorPosY() - offsetY );

					ImGui::PushStyleColor( ImGuiCol_Text, palette::Primary );
					ImGui::Text( " %d / %zu ", r.currentPos + 1, r.positions.size() >> 1 );
					ImGui::PopStyleColor();
					ImGui::SameLine();
					ImGui::SetCursorPosY( ImGui::GetCursorPosY() - offsetY );

					ImGui::PushStyleColor( ImGuiCol_Text, palette::TextSub );
					ImGui::Text( "[pos: %zu]", r.positions[r.currentPos] );
					ImGui::PopStyleColor();
					ImGui::SameLine();
					ImGui::SetCursorPosY( ImGui::GetCursorPosY() - offsetY );

					bool atLast = ( r.currentPos >= static_cast< int >( r.positions.size() >> 1 ) - 1 );
					if ( atLast ) ImGui::BeginDisabled();
					if ( ImGui::SmallButton( " > " ) )
					{
						if ( r.currentPos < static_cast< int >( r.positions.size() >> 1 ) - 1 )
							++r.currentPos;
					}
					if ( atLast ) ImGui::EndDisabled();

					ImGui::PopStyleColor( 4 );

					size_t start, end;

					std::string content = ReadDocContent( s, r.docIndex, r.positions[r.currentPos << 1], r.positions[1 + ( r.currentPos << 1 )], s.contextChars, start, end );
					if ( !content.empty() )
						ShowContextSnippet( content, start, end );
				}
				else
				{
					ImGui::PushStyleColor( ImGuiCol_Text, palette::TextDim );
					ImGui::Text( "  (无具体位置信息)" );
					ImGui::PopStyleColor();
				}
			}
			ImGui::EndChild();
			ImGui::PopStyleColor();

			ImGui::PopID();
		}
	}




	void RenderIndexPanel( AppState &s )
	{
		if ( s.indexStatus == AppState::IndexStatus::NotBuilt )
		{
			ImGui::Spacing();
			ShowTxtInMiddle( "索引尚未建立, 请先创建索引" );
			return;
		}

		auto &idx = s.finder->index;


		ImGui::PushStyleColor( ImGuiCol_Text, palette::TextSub );
		ImGui::Text( "索引词条总数: " );
		ImGui::PopStyleColor();
		ImGui::SameLine();
		ImGui::PushStyleColor( ImGuiCol_Text, palette::Primary );
		ImGui::Text( "%zu", idx.size() );
		ImGui::PopStyleColor();

		ImGui::SameLine( 0, 30 );


		ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 0.90f, 0.94f, 1.00f, 1.0f ) );
		ImGui::PushStyleColor( ImGuiCol_ButtonHovered, ImVec4( 0.82f, 0.88f, 0.98f, 1.0f ) );
		ImGui::PushStyleColor( ImGuiCol_ButtonActive, ImVec4( 0.74f, 0.82f, 0.96f, 1.0f ) );
		ImGui::PushStyleColor( ImGuiCol_Text, palette::Primary );
		if ( ImGui::SmallButton( "清空索引" ) )
		{
			auto t0 = std::chrono::high_resolution_clock::now();
			s.finder->clear_index();
			s.indexStatus = AppState::IndexStatus::NotBuilt;
			auto t1 = std::chrono::high_resolution_clock::now();
			s.indexTimeMs = std::chrono::duration< double, std::milli >( t1 - t0 ).count();
		}
		ImGui::PopStyleColor( 4 );

		ImGui::Spacing();


		ImGui::PushStyleColor( ImGuiCol_Text, palette::TextSub );
		ImGui::Text( "筛选:" );
		ImGui::PopStyleColor();
		ImGui::SameLine();
		ImGui::SetNextItemWidth( ImGui::GetContentRegionAvail().x );

		const float lineH = ImGui::GetFrameHeight();
		const float tH = ImGui::CalcTextSize( "筛选:" ).y;
		ImGui::SetCursorPosY( ImGui::GetCursorPosY() - ( lineH - tH ) / 2 );
		ImGui::InputTextWithHint( "##idxfilter", "输入关键词筛选索引...", s.indexFilter, sizeof( s.indexFilter ) );

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();


		ImGuiTableFlags tflags =
		    ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerH |
		    ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY |
		    ImGuiTableFlags_SizingStretchProp;

		if ( ImGui::BeginTable( "##IndexTable", 4, tflags ) )
		{
			ImGui::TableSetupColumn( "索引词", ImGuiTableColumnFlags_WidthFixed, 120.0f );
			ImGui::TableSetupColumn( "文档数", ImGuiTableColumnFlags_WidthFixed, 60.0f );
			ImGui::TableSetupColumn( "匹配数", ImGuiTableColumnFlags_WidthFixed, 60.0f );
			ImGui::TableSetupColumn( "详细 (文档ID: 位置...)", ImGuiTableColumnFlags_WidthStretch );
			ImGui::TableSetupScrollFreeze( 0, 1 );
			ImGui::TableHeadersRow();

			std::string filterStr( s.indexFilter );
			int shown = 0;

			for ( auto &[word, entries] : idx )
			{
				if ( !filterStr.empty() && word.find( filterStr ) == std::string::npos )
					continue;


				if ( ++shown > 500 )
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex( 0 );
					ImGui::PushStyleColor( ImGuiCol_Text, palette::TextDim );
					ImGui::Text( "... 已截断 (共 %zu 条)", idx.size() );
					ImGui::PopStyleColor();
					break;
				}

				ImGui::TableNextRow();


				ImGui::TableSetColumnIndex( 0 );
				ImGui::PushStyleColor( ImGuiCol_Text, palette::Primary );
				ImGui::Text( word.c_str() );
				ImGui::PopStyleColor();


				ImGui::TableSetColumnIndex( 1 );
				ImGui::Text( "%zu", entries.size() );


				ImGui::TableSetColumnIndex( 2 );
				size_t totalPos = 0;
				for ( auto &e : entries )
					totalPos += ( e.size() >> 1 );
				ImGui::Text( "%zu", totalPos );


				ImGui::TableSetColumnIndex( 3 );
				std::ostringstream oss;
				for ( size_t ei = 0; ei < entries.size(); ++ei )
				{
					auto &e = entries[ei];
					if ( e.empty() ) continue;
					if ( ei > 0 ) oss << " | ";


					size_t docId = static_cast< size_t >( e[0] );

					oss << docId << ": [";
					for ( size_t pi = 1; pi < e.size(); ++pi )
					{
						if ( pi > 1 ) oss << ",";
						oss << "(" << e[pi] << "," << e[++pi] << ")";
						if ( pi > 7 && pi < e.size() - 1 )
						{
							oss << "...(+" << e.size() - 1 - pi << ")";
							break;
						}
					}
					oss << "]";

					if ( ei >= 5 && ei < entries.size() - 1 )
					{
						oss << " ... (+" << entries.size() - 1 - ei << " docs)";
						break;
					}
				}
				ImGui::TextWrapped( "%s", oss.str().c_str() );
			}
			ImGui::EndTable();
		}
	}




	void RenderPerfPanel( AppState &s )
	{
		float tableH = ImGui::GetContentRegionAvail().y;

		if ( s.perfHistory.empty() && s.indexHistory.empty() )
		{
			ImGui::Spacing();
			ShowTxtInMiddle( "暂无" );
			return;
		}



		{
			double totalMs = 0, minMs = 1e9, maxMs = 0;
			for ( auto &p : s.indexHistory )
			{
				totalMs += p.timeMs;
				if ( p.timeMs < minMs ) minMs = p.timeMs;
				if ( p.timeMs > maxMs ) maxMs = p.timeMs;
			}
			double avgMs = totalMs / s.indexHistory.size();
			ImGui::Text( "[索引概览]" );
			ImGui::Spacing();
			ImGui::Columns( 4, "##indexSummary", false );

			ImGui::PushStyleColor( ImGuiCol_Text, palette::TextSub );
			ImGui::Text( "建立索引次数" );
			ImGui::PopStyleColor();
			ImGui::PushStyleColor( ImGuiCol_Text, palette::Primary );
			ImGui::Text( "%zu", s.indexHistory.size() );
			ImGui::PopStyleColor();
			ImGui::NextColumn();

			ImGui::PushStyleColor( ImGuiCol_Text, palette::TextSub );
			ImGui::Text( "平均耗时" );
			ImGui::PopStyleColor();
			ImGui::PushStyleColor( ImGuiCol_Text, palette::Primary );
			ImGui::Text( "%.5f ms", avgMs );
			ImGui::PopStyleColor();
			ImGui::NextColumn();

			ImGui::PushStyleColor( ImGuiCol_Text, palette::TextSub );
			ImGui::Text( "最短耗时" );
			ImGui::PopStyleColor();
			ImGui::PushStyleColor( ImGuiCol_Text, palette::Success );
			ImGui::Text( "%.5f ms", minMs );
			ImGui::PopStyleColor();
			ImGui::NextColumn();

			ImGui::PushStyleColor( ImGuiCol_Text, palette::TextSub );
			ImGui::Text( "最长耗时" );
			ImGui::PopStyleColor();
			ImGui::PushStyleColor( ImGuiCol_Text, palette::Error );
			ImGui::Text( "%.5f ms", maxMs );
			ImGui::PopStyleColor();
			ImGui::NextColumn();

			ImGui::Columns( 1 );
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();


			ImGui::Text( "[最近建立索引耗时分布]" );
			ImGui::Spacing();

			if ( maxMs > 0 )
			{
				float barMaxH = 80.0f;
				float barW = 24.0f;
				float spacing = 4.0f;
				ImVec2 cursor = ImGui::GetCursorScreenPos();
				ImDrawList *dl = ImGui::GetWindowDrawList();
				float xOff = 0.0f;

				size_t showCount = s.indexHistory.size();
				size_t maxShowCount = ImGui::GetContentRegionAvail().x / ( barW + spacing );
				if ( showCount > maxShowCount ) showCount = maxShowCount;

				for ( size_t i = s.indexHistory.size() - showCount; i < s.indexHistory.size(); ++i )
				{
					float ratio = static_cast< float >( s.indexHistory[i].timeMs / maxMs );
					float barH = std::max( 3.0f, ratio * barMaxH );

					ImVec2 p0( cursor.x + xOff, cursor.y + barMaxH - barH );
					ImVec2 p1( cursor.x + xOff + barW, cursor.y + barMaxH );

					ImVec4 col = ( ratio > 0.8f )   ? palette::Error
					             : ( ratio > 0.5f ) ? palette::Warning
					                                : palette::Primary;
					dl->AddRectFilled( p0, p1, ImGui::ColorConvertFloat4ToU32( col ), 3.0f );

					if ( ImGui::IsMouseHoveringRect( p0, p1 ) )
					{
						ImGui::BeginTooltip();
						ImGui::Text( "参与文档数: %zu", s.indexHistory[i].docCount );
						ImGui::Text( "索引词数: %zu", s.indexHistory[i].keyCount );
						ImGui::Text( "总词数: %zu", s.indexHistory[i].posCount );
						ImGui::Text( "耗时: %.5f ms", s.indexHistory[i].timeMs );
						ImGui::EndTooltip();
					}
					xOff += barW + spacing;
				}
				ImGui::Dummy( ImVec2( xOff, barMaxH + 4 ) );
			}

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();
		}

		{
			double totalMs = 0, minMs = 1e9, maxMs = 0;
			for ( auto &p : s.perfHistory )
			{
				totalMs += p.timeMs;
				if ( p.timeMs < minMs ) minMs = p.timeMs;
				if ( p.timeMs > maxMs ) maxMs = p.timeMs;
			}
			double avgMs = totalMs / s.perfHistory.size();
			ImGui::Text( "[查询概览]" );
			ImGui::Spacing();
			ImGui::Columns( 4, "##PerfSummary", false );

			ImGui::PushStyleColor( ImGuiCol_Text, palette::TextSub );
			ImGui::Text( "总查询数" );
			ImGui::PopStyleColor();
			ImGui::PushStyleColor( ImGuiCol_Text, palette::Primary );
			ImGui::Text( "%zu", s.perfHistory.size() );
			ImGui::PopStyleColor();
			ImGui::NextColumn();

			ImGui::PushStyleColor( ImGuiCol_Text, palette::TextSub );
			ImGui::Text( "平均耗时" );
			ImGui::PopStyleColor();
			ImGui::PushStyleColor( ImGuiCol_Text, palette::Primary );
			ImGui::Text( "%.5f ms", s.perfHistory.empty() ? 0 : avgMs );
			ImGui::PopStyleColor();
			ImGui::NextColumn();

			ImGui::PushStyleColor( ImGuiCol_Text, palette::TextSub );
			ImGui::Text( "最短耗时" );
			ImGui::PopStyleColor();
			ImGui::PushStyleColor( ImGuiCol_Text, palette::Success );
			ImGui::Text( "%.5f ms", s.perfHistory.empty() ? 0 : minMs );
			ImGui::PopStyleColor();
			ImGui::NextColumn();

			ImGui::PushStyleColor( ImGuiCol_Text, palette::TextSub );
			ImGui::Text( "最长耗时" );
			ImGui::PopStyleColor();
			ImGui::PushStyleColor( ImGuiCol_Text, palette::Error );
			ImGui::Text( "%.5f ms", s.perfHistory.empty() ? 0 : maxMs );
			ImGui::PopStyleColor();
			ImGui::NextColumn();

			ImGui::Columns( 1 );
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();


			ImGui::Text( "[最近查询耗时分布]" );
			ImGui::Spacing();

			if ( maxMs > 0 )
			{
				float barMaxH = 80.0f;
				float barW = 24.0f;
				float spacing = 4.0f;
				ImVec2 cursor = ImGui::GetCursorScreenPos();
				ImDrawList *dl = ImGui::GetWindowDrawList();
				float xOff = 0.0f;

				size_t showCount = s.perfHistory.size();
				size_t maxShowCount = ImGui::GetContentRegionAvail().x / ( barW + spacing );
				if ( showCount > maxShowCount ) showCount = maxShowCount;

				for ( size_t i = s.perfHistory.size() - showCount; i < s.perfHistory.size(); ++i )
				{
					float ratio = static_cast< float >( s.perfHistory[i].timeMs / maxMs );
					float barH = std::max( 3.0f, ratio * barMaxH );

					ImVec2 p0( cursor.x + xOff, cursor.y + barMaxH - barH );
					ImVec2 p1( cursor.x + xOff + barW, cursor.y + barMaxH );

					ImVec4 col = ( ratio > 0.8f )   ? palette::Error
					             : ( ratio > 0.5f ) ? palette::Warning
					                                : palette::Primary;
					dl->AddRectFilled( p0, p1, ImGui::ColorConvertFloat4ToU32( col ), 3.0f );

					if ( ImGui::IsMouseHoveringRect( p0, p1 ) )
					{
						ImGui::BeginTooltip();
						ImGui::Text( "查询: %s", s.perfHistory[i].query.c_str() );
						ImGui::Text( "模式: %s", s.perfHistory[i].mode.c_str() );
						ImGui::Text( "耗时: %.5f ms", s.perfHistory[i].timeMs );
						ImGui::Text( "文档: %d", s.perfHistory[i].matchedDocs );
						ImGui::Text( "匹配: %d", s.perfHistory[i].totalHits );
						ImGui::EndTooltip();
					}
					xOff += barW + spacing;
				}
				ImGui::Dummy( ImVec2( xOff, barMaxH + 4 ) );
			}

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();
		}

		ImGui::Text( "[查询历史]" );
		ImGui::Spacing();

		if ( ImGui::SmallButton( "清空历史" ) )
			s.perfHistory.clear();
		ImGui::Spacing();

		ImGuiTableFlags tflags =
		    ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerH |
		    ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY |
		    ImGuiTableFlags_SizingStretchProp;
		if ( ImGui::BeginTable( "##PerfTable", 5, tflags, ImVec2( 0, tableH ) ) )
		{
			ImGui::TableSetupColumn( "#", ImGuiTableColumnFlags_WidthFixed, 40 );
			ImGui::TableSetupColumn( "查询内容", ImGuiTableColumnFlags_WidthStretch );
			ImGui::TableSetupColumn( "模式", ImGuiTableColumnFlags_WidthFixed, 100 );
			ImGui::TableSetupColumn( "耗时 (ms)", ImGuiTableColumnFlags_WidthFixed, 100 );
			ImGui::TableSetupColumn( "文档:匹配", ImGuiTableColumnFlags_WidthFixed, 150 );
			ImGui::TableSetupScrollFreeze( 0, 1 );

			ImGui::TableHeadersRow();

			for ( int i = static_cast< int >( s.perfHistory.size() ) - 1; i >= 0; --i )
			{
				ImGui::PushID( i );

				auto &p = s.perfHistory[i];
				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex( 0 );
				ImGui::Text( "%d", i + 1 );

				ImGui::TableSetColumnIndex( 1 );
				ImGui::Text( "%s", p.query.c_str() );

				ImGui::TableSetColumnIndex( 2 );
				if ( p.mode == "AND" )
					Badge( "AND", palette::Primary );
				else if ( p.mode == "OR" )
					Badge( "OR", ImVec4( 0.80f, 0.45f, 0.10f, 0.85f ) );
				else if ( p.mode == "MIXED" )
					Badge( "MIXED", ImVec4( 0.55f, 0.35f, 0.70f, 0.85f ) );
				else
					Badge( "SINGLE", palette::TextSub );

				ImGui::TableSetColumnIndex( 3 );
				ImGui::Text( "%.3f", p.timeMs );

				ImGui::TableSetColumnIndex( 4 );
				ImGui::Text( "%d:%d", p.matchedDocs, p.totalHits );
				ImGui::PopID();
			}
			ImGui::EndTable();
		}
	}




	void RenderStatusBar( AppState &s )
	{
		ImGui::Separator();
		ImGui::PushStyleColor( ImGuiCol_Text, palette::TextSub );

		ImGui::Text( "文档: %zu", s.docs.size() );
		ImGui::SameLine( 0, 20 );

		size_t selCount = 0;
		for ( auto &d : s.docs )
			if ( d.selected ) ++selCount;
		ImGui::Text( "已选: %zu", selCount );
		ImGui::SameLine( 0, 20 );

		if ( s.indexStatus == AppState::IndexStatus::Built )
		{
			ImGui::PushStyleColor( ImGuiCol_Text, palette::Success );
			ImGui::Text( "[INDEX BUILT] %.5f ms", s.indexTimeMs );
			ImGui::PopStyleColor();
		}
		else if ( s.indexStatus == AppState::IndexStatus::Building )
		{
			ImGui::PushStyleColor( ImGuiCol_Text, palette::Primary );
			ImGui::Text( "[INDEX BUILDING] (%zu/%zu) now:%zu", s.downCount.load(), s.totalCount, s.currentDocIndex.load() );
			ImGui::PopStyleColor();
		}
		else
		{
			ImGui::PushStyleColor( ImGuiCol_Text, palette::TextDim );
			if ( s.indexStatus == AppState::IndexStatus::NotBuilt )
				ImGui::Text( "[INDEX NOT BUILT]" );
			else if ( s.indexStatus == AppState::IndexStatus::Append )
				ImGui::Text( "[INDEX INCOMPLETE]" );
			ImGui::PopStyleColor();
		}
		ImGui::SameLine( 0, 20 );
		if ( s.searched )
			ImGui::Text( "| 上次查询: %.5f ms", s.lastQueryMs );

		ImGui::PopStyleColor();
	}




	void RenderPopups( AppState &s )
	{
		if ( s.popAddFile )
			ImGui::OpenPopup( "popup_add_file" );

		ImGui::SetNextWindowSize( ImVec2( 560, 0 ), ImGuiCond_Always );
		if ( ImGui::BeginPopupModal( "添加文件###popup_add_file", &s.popAddFile,
		                             ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize ) )
		{
			ImGui::Separator();
			ImGui::Spacing();


#ifdef _WIN32
			{
				float bw3 = ( ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x * 2 ) / 3.0f;

				ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 0.90f, 0.94f, 1.00f, 1.0f ) );
				ImGui::PushStyleColor( ImGuiCol_ButtonHovered, ImVec4( 0.82f, 0.88f, 0.98f, 1.0f ) );
				ImGui::PushStyleColor( ImGuiCol_ButtonActive, ImVec4( 0.74f, 0.82f, 0.96f, 1.0f ) );
				ImGui::PushStyleColor( ImGuiCol_Text, palette::Primary );

				if ( ImGui::Button( "浏览文件...", ImVec2( bw3, 0 ) ) )
				{
					auto paths = OpenFileDialog( nullptr );
					for ( auto &wp : paths )
					{
						std::string path = WtoU8( wp );

						size_t curLen = std::strlen( s.filePathBuf );
						if ( curLen > 0 && s.filePathBuf[curLen - 1] != '\n' )
						{
							if ( curLen + 1 < sizeof( s.filePathBuf ) )
								s.filePathBuf[curLen++] = '\n';
						}
						size_t remain = sizeof( s.filePathBuf ) - curLen - 1;
						if ( path.size() <= remain )
						{
							std::memcpy( s.filePathBuf + curLen, path.c_str(), path.size() );
							s.filePathBuf[curLen + path.size()] = '\0';
						}
					}
				}

				ImGui::SameLine();
				if ( ImGui::Button( "浏览文件夹...", ImVec2( bw3, 0 ) ) )
				{
					std::wstring folder = OpenFolderDialog( nullptr );
					if ( !folder.empty() )
					{
						std::vector< std::wstring > files;
						EnumFilesRecursive( folder, files );
						for ( auto &wp : files )
						{
							std::string path = WtoU8( wp );
							size_t curLen = std::strlen( s.filePathBuf );
							if ( curLen > 0 && s.filePathBuf[curLen - 1] != '\n' )
							{
								if ( curLen + 1 < sizeof( s.filePathBuf ) )
									s.filePathBuf[curLen++] = '\n';
							}
							size_t remain = sizeof( s.filePathBuf ) - curLen - 1;
							if ( path.size() <= remain )
							{
								std::memcpy( s.filePathBuf + curLen, path.c_str(), path.size() );
								s.filePathBuf[curLen + path.size()] = '\0';
							}
						}
					}
				}

				ImGui::SameLine();

				ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 0.95f, 0.90f, 0.90f, 1.0f ) );
				ImGui::PushStyleColor( ImGuiCol_ButtonHovered, ImVec4( 0.92f, 0.82f, 0.82f, 1.0f ) );
				ImGui::PushStyleColor( ImGuiCol_ButtonActive, ImVec4( 0.88f, 0.74f, 0.74f, 1.0f ) );
				ImGui::PushStyleColor( ImGuiCol_Text, palette::Error );
				if ( ImGui::Button( "清空列表", ImVec2( bw3, 0 ) ) )
					std::memset( s.filePathBuf, 0, sizeof( s.filePathBuf ) );
				ImGui::PopStyleColor( 4 );

				ImGui::PopStyleColor( 4 );
				ImGui::Spacing();
			}
#endif

			ImGui::Text( "文件路径 (支持多个, 每行一个):" );
			ImGui::InputTextMultiline( "##filepath", s.filePathBuf, sizeof( s.filePathBuf ),
			                           ImVec2( -1, 120 ) );

			ImGui::Spacing();


			auto addFilesFromBuf = [&]()
			{
				std::istringstream iss( s.filePathBuf );
				std::string line;
				while ( std::getline( iss, line ) )
				{
					while ( !line.empty() && ( line.front() == ' ' || line.front() == '\t' ) )
						line.erase( line.begin() );
					while ( !line.empty() && ( line.back() == ' ' || line.back() == '\t' || line.back() == '\r' ) )
						line.pop_back();
					if ( line.empty() ) continue;

					std::wstring wpath = U8toW( line );
					s.finder->add( wpath );

					DocEntry de;
					de.displayName = line;
					de.filePath = wpath;
					de.isFile = true;
					de.selected = true;
					s.docs.push_back( std::move( de ) );
				}
				std::memset( s.filePathBuf, 0, sizeof( s.filePathBuf ) );
				if ( s.finder->index.empty() )
					s.indexStatus = AppState::IndexStatus::NotBuilt;
				else
					s.indexStatus = AppState::IndexStatus::Append;
			};

			float w2 = ( ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x ) * 0.5f;

			ImGui::PushStyleColor( ImGuiCol_Text, palette::TextWhite );
			if ( ImGui::Button( "确认添加", ImVec2( w2, 0 ) ) )
			{
				addFilesFromBuf();
				ImGui::CloseCurrentPopup();
				s.popAddFile = false;
			}
			ImGui::PopStyleColor();

			ImGui::SameLine();
			ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 0.85f, 0.87f, 0.90f, 1.0f ) );
			ImGui::PushStyleColor( ImGuiCol_ButtonHovered, ImVec4( 0.78f, 0.80f, 0.84f, 1.0f ) );
			ImGui::PushStyleColor( ImGuiCol_ButtonActive, ImVec4( 0.72f, 0.74f, 0.78f, 1.0f ) );
			ImGui::PushStyleColor( ImGuiCol_Text, palette::Text );
			if ( ImGui::Button( "取消", ImVec2( w2, 0 ) ) )
			{
				ImGui::CloseCurrentPopup();
				s.popAddFile = false;
			}
			ImGui::PopStyleColor( 4 );

			ImGui::EndPopup();
		}


		if ( s.popAddText )
			ImGui::OpenPopup( "popup_add_text" );

		ImGui::SetNextWindowSize( ImVec2( 500, 0 ), ImGuiCond_Always );
		if ( ImGui::BeginPopupModal( "添加文本###popup_add_text", &s.popAddText,
		                             ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize ) )
		{
			ImGui::Separator();
			ImGui::Spacing();

			ImGui::Text( "输入文本内容 (将作为一个文档参与查询):" );
			ImGui::InputTextMultiline( "##textcontent", s.textBuf, sizeof( s.textBuf ),
			                           ImVec2( -1, 140 ) );

			ImGui::Spacing();
			float w2 = ( ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x ) * 0.5f;

			ImGui::PushStyleColor( ImGuiCol_Text, palette::TextWhite );
			if ( ImGui::Button( "确认添加", ImVec2( w2, 0 ) ) )
			{
				std::string txt( s.textBuf );
				if ( !txt.empty() )
				{
					s.finder->addString( txt );

					DocEntry de;
					de.displayName = ( txt.size() > 30 )
					                     ? txt.substr( 0, 30 ) + "..."
					                     : txt;
					de.rawText = txt;
					de.isFile = false;
					de.selected = true;
					s.docs.push_back( std::move( de ) );
				}
				std::memset( s.textBuf, 0, sizeof( s.textBuf ) );
				if ( s.finder->index.empty() )
					s.indexStatus = AppState::IndexStatus::NotBuilt;
				else
					s.indexStatus = AppState::IndexStatus::Append;
				ImGui::CloseCurrentPopup();
				s.popAddText = false;
			}
			ImGui::PopStyleColor();

			ImGui::SameLine();
			ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 0.85f, 0.87f, 0.90f, 1.0f ) );
			ImGui::PushStyleColor( ImGuiCol_ButtonHovered, ImVec4( 0.78f, 0.80f, 0.84f, 1.0f ) );
			ImGui::PushStyleColor( ImGuiCol_ButtonActive, ImVec4( 0.72f, 0.74f, 0.78f, 1.0f ) );
			ImGui::PushStyleColor( ImGuiCol_Text, palette::Text );
			if ( ImGui::Button( "取消", ImVec2( w2, 0 ) ) )
			{
				ImGui::CloseCurrentPopup();
				s.popAddText = false;
			}
			ImGui::PopStyleColor( 4 );

			ImGui::EndPopup();
		}


		if ( s.popAbout )
			ImGui::OpenPopup( "popup_about" );

		ImGui::SetNextWindowSize( ImVec2( 380, 0 ), ImGuiCond_Always );
		if ( ImGui::BeginPopupModal( "说明###popup_about", &s.popAbout,
		                             ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize ) )
		{
			ImGui::Spacing();
			ImGui::PushStyleColor( ImGuiCol_Text, palette::Primary );
			float tw = ImGui::CalcTextSize( "文档集合查询系统" ).x;
			ImGui::SetCursorPosX( ( ImGui::GetContentRegionAvail().x - tw ) * 0.5f + ImGui::GetCursorPosX() );
			ImGui::Text( "文档集合查询系统" );
			ImGui::PopStyleColor();

			ImGui::Spacing();

			ImGui::Spacing();

			ImGui::TextWrapped( "基于倒排索引的文档集合查询工具。\n"
			                    "支持单词查询及多词 AND / OR 链式组合查询。" );

			ImGui::Spacing();
			ImGui::TextWrapped( "数据结构: 倒排索引, Hash Map\n"
			                    "算法: 双指针合并\n"
			                    "分词方式: 单字分词, 简单英文单词分词" );

			ImGui::Spacing();
			float bw = 120;
			ImGui::SetCursorPosX( ( ImGui::GetContentRegionAvail().x - bw ) * 0.5f + ImGui::GetCursorPosX() );
			ImGui::PushStyleColor( ImGuiCol_Text, palette::TextWhite );
			if ( ImGui::Button( "确定", ImVec2( bw, 0 ) ) )
			{
				ImGui::CloseCurrentPopup();
				s.popAbout = false;
			}
			ImGui::PopStyleColor();
			ImGui::Spacing();
			ImGui::EndPopup();
		}
	}



	int run()
	{
		if ( !glfwInit() ) return -1;

		glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
		glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
		glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );


		GLFWwindow *window = glfwCreateWindow( 1280, 800, "文档集合查询系统", nullptr, nullptr );
		if ( !window )
		{
			glfwTerminate();
			return -1;
		}
		glfwMakeContextCurrent( window );
		glfwSwapInterval( 1 );
#ifdef _WIN32
		HWND hwnd = glfwGetWin32Window( window );

		HICON hIcon = (HICON)LoadImage(
		    GetModuleHandle( NULL ),
		    MAKEINTRESOURCE( 1 ),
		    IMAGE_ICON,
		    0, 0,
		    LR_DEFAULTSIZE );

		SendMessage( hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon );
		SendMessage( hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon );

#endif

		ImGui::CreateContext();
		ImGuiIO &io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigDebugHighlightIdConflicts = false;
		io.IniFilename = "txtf.config";
		ImFont *font28 = io.Fonts->AddFontFromFileTTF(
		    "C:/Windows/Fonts/msyh.ttc",
		    28.0f,
		    nullptr,
		    io.Fonts->GetGlyphRangesChineseFull() );

		ImFont *font20 = io.Fonts->AddFontFromFileTTF(
		    "C:/Windows/Fonts/msyh.ttc",
		    20.0f,
		    nullptr,
		    io.Fonts->GetGlyphRangesChineseFull() );
		ImFont *font24 = io.Fonts->AddFontFromFileTTF(
		    "C:/Windows/Fonts/msyh.ttc",
		    24.0f,
		    nullptr,
		    io.Fonts->GetGlyphRangesChineseFull() );
		ImGui_ImplGlfw_InitForOpenGL( window, true );
		ImGui_ImplOpenGL3_Init( "#version 330" );

		ApplyLightTheme();


		AppState state;
		state.finder = new Finder< char >();


		while ( !glfwWindowShouldClose( window ) )
		{
			glfwPollEvents();

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();


			const ImGuiViewport *vp = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos( vp->WorkPos );
			ImGui::SetNextWindowSize( vp->WorkSize );

			ImGuiWindowFlags mainFlags =
			    ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
			    ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
			    ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_MenuBar;

			ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0, 0 ) );
			ImGui::Begin( "##MainWindow", nullptr, mainFlags );
			ImGui::PopStyleVar();


			if ( ImGui::BeginMenuBar() )
			{
				ImGui::PushFont( font20 );
				if ( ImGui::BeginMenu( "文件" ) )
				{
					if ( ImGui::MenuItem( "添加文件..." ) ) state.popAddFile = true;
					if ( ImGui::MenuItem( "添加文本..." ) ) state.popAddText = true;
					ImGui::Separator();
					if ( ImGui::MenuItem( "退出", "Alt+F4" ) )
						glfwSetWindowShouldClose( window, true );
					ImGui::EndMenu();
				}
				if ( ImGui::BeginMenu( "索引" ) )
				{
					bool canBuild = !state.docs.empty();
					if ( ImGui::MenuItem( "创建索引", nullptr, false, canBuild ) )
						CreateIndex( state );
					if ( ImGui::MenuItem( "清空索引", nullptr, false, state.indexStatus == AppState::IndexStatus::Built ) )
					{
						auto t0 = std::chrono::high_resolution_clock::now();
						state.finder->clear_index();
						state.indexStatus = AppState::IndexStatus::NotBuilt;
						auto t1 = std::chrono::high_resolution_clock::now();
						state.indexTimeMs = std::chrono::duration< double, std::milli >( t1 - t0 ).count();
					}
					if ( ImGui::BeginMenu( "分词方式" ) )
					{
						if ( ImGui::MenuItem( "单字符分词器", "utf-8", state.splitterMode == AppState::SplitterMode::Single ) )
							state.splitterMode = AppState::SplitterMode::Single;
						if ( ImGui::MenuItem( "简单分词器", "仅支持英文", state.splitterMode == AppState::SplitterMode::Simple ) )
							state.splitterMode = AppState::SplitterMode::Simple;
						ImGui::EndMenu();
					}
					ImGui::EndMenu();
				}
				if ( ImGui::BeginMenu( "关于" ) )
				{
					if ( ImGui::MenuItem( "说明" ) ) state.popAbout = true;
					if ( ImGui::MenuItem( "GitHub" ) )
						ShellExecuteW( nullptr, L"open", L"https://github.com/NIA-sai/txtf", nullptr, nullptr, SW_SHOWNORMAL );
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
				ImGui::PopFont();
			}


			ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 14, 14 ) );

			float statusH = 48.0f;
			float contentH = ImGui::GetContentRegionAvail().y - statusH;
			float totalW = ImGui::GetContentRegionAvail().x;
			float leftW = totalW * state.leftRatio;
			float rightW = totalW - leftW - 10;


			ImGui::PushStyleColor( ImGuiCol_ChildBg, palette::BgPanel );
			ImGui::BeginChild( "##LeftPanel", ImVec2( leftW, contentH ), ImGuiChildFlags_Borders );
			RenderDocPanel( state );
			ImGui::EndChild();
			ImGui::PopStyleColor();

			ImGui::SameLine( 0, 0 );

			ImGui::InvisibleButton( "##Splitter1",
			                        ImVec2( 10.0f, contentH ) );
			{
				bool active = ImGui::IsItemActive();
				bool hovered = ImGui::IsItemHovered();

				if ( hovered || active )
					ImGui::SetMouseCursor( ImGuiMouseCursor_ResizeEW );

				if ( active )
				{
					leftW += ImGui::GetIO().MouseDelta.x;


					if ( leftW < 150.0f )
						leftW = 150.0f;
					if ( leftW > totalW - 150.0f )
						leftW = totalW - 150.0f;
					state.leftRatio = leftW / totalW;
				}
			}
			ImGui::SameLine( 0, 0 );


			ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 0, 0 ) );
			ImGui::BeginChild( "##RightOuter", ImVec2( rightW, contentH ), ImGuiChildFlags_None );

			{
				const float splitterH = 5.0f;


				float minTopH = 120.0f;
				float minBottomH = 120.0f;
				float searchH = state.splitRatio * contentH;

				if ( searchH < minTopH ) searchH = minTopH;
				if ( searchH > contentH - splitterH )
					searchH = contentH - splitterH;


				ImGui::PopStyleVar();
				ImGui::PushStyleColor( ImGuiCol_ChildBg, palette::BgPanel );
				ImGui::BeginChild( "##SearchCard", ImVec2( 0, searchH ), ImGuiChildFlags_Borders );
				RenderSearchPanel( state );
				ImGui::EndChild();
				ImGui::PopStyleColor();
				ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 0, 0 ) );


				{
					ImGui::InvisibleButton( "##splitter",
					                        ImVec2( ImGui::GetContentRegionAvail().x, splitterH ) );

					bool active = ImGui::IsItemActive();
					bool hovered = ImGui::IsItemHovered();

					if ( hovered || active )
						ImGui::SetMouseCursor( ImGuiMouseCursor_ResizeNS );

					if ( active )
					{
						searchH += ImGui::GetIO().MouseDelta.y;
						state.splitRatio = searchH / contentH;
					}
				}
				ImGui::PopStyleVar();

				ImGui::PushStyleColor( ImGuiCol_ChildBg, palette::BgPanel );
				ImGui::BeginChild( "##TabCard", ImVec2( 0, contentH - 14 - searchH ), ImGuiChildFlags_Borders );
				{
					if ( ImGui::BeginTabBar( "##MainTabs" ) )
					{
						if ( ImGui::BeginTabItem( "查询结果" ) )
						{
							RenderResultPanel( state );
							ImGui::EndTabItem();
						}

						if ( ImGui::BeginTabItem( "索引查看" ) )
						{
							RenderIndexPanel( state );
							ImGui::EndTabItem();
						}

						if ( ImGui::BeginTabItem( "性能统计" ) )
						{
							RenderPerfPanel( state );
							ImGui::EndTabItem();
						}
						ImGui::EndTabBar();
					}
				}
				ImGui::EndChild();
				ImGui::PopStyleColor();
			}

			ImGui::EndChild();

			ImGui::PopStyleVar();


			RenderStatusBar( state );


			RenderPopups( state );

			ImGui::End();


			ImGui::Render();
			int fbW, fbH;
			glfwGetFramebufferSize( window, &fbW, &fbH );
			glViewport( 0, 0, fbW, fbH );
			glClearColor( 0.96f, 0.97f, 0.98f, 1.0f );
			glClear( GL_COLOR_BUFFER_BIT );
			ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
			glfwSwapBuffers( window );
		}


		delete state.finder;
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		glfwDestroyWindow( window );
		glfwTerminate();
		return 0;
	}

}
