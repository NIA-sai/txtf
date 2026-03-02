#pragma once
#include "spliter.hpp"
#include "static_queue.tpp"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstring>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <sstream>
#include <string>
#include <vector>
#include <windows.h>
#ifdef _WIN32
	#ifndef NOMINMAX
		#define NOMINMAX
	#endif
	#include <commdlg.h>
	#include <shlobj.h>
	#pragma comment( lib, "comdlg32.lib" )
	#pragma comment( lib, "shell32.lib" )
	#pragma comment( lib, "ole32.lib" )
#endif
#include "finder.hpp"

namespace front_end
{

	// ============================================================================
	//  调色板 -- 浅色系
	// ============================================================================
	namespace palette
	{
		// 主色 (蓝)
		inline constexpr ImVec4 Primary{ 0.22f, 0.55f, 0.87f, 1.00f };  // #3A8DDE
		inline constexpr ImVec4 PrimaryHover{ 0.18f, 0.49f, 0.80f, 1.00f };
		inline constexpr ImVec4 PrimaryActive{ 0.14f, 0.43f, 0.73f, 1.00f };
		inline constexpr ImVec4 PrimaryLight{ 0.22f, 0.55f, 0.87f, 0.12f };
		inline constexpr ImVec4 PrimaryMid{ 0.22f, 0.55f, 0.87f, 0.25f };

		// 背景
		inline constexpr ImVec4 BgBase{ 0.96f, 0.97f, 0.98f, 1.00f };   // #F4F7FA
		inline constexpr ImVec4 BgPanel{ 1.00f, 1.00f, 1.00f, 1.00f };  // #FFFFFF
		inline constexpr ImVec4 BgChild{ 0.98f, 0.98f, 0.99f, 1.00f };

		// 文本
		inline constexpr ImVec4 Text{ 0.17f, 0.24f, 0.31f, 1.00f };     // #2C3E50
		inline constexpr ImVec4 TextSub{ 0.47f, 0.55f, 0.62f, 1.00f };  // #788C9E
		inline constexpr ImVec4 TextDim{ 0.62f, 0.69f, 0.75f, 1.00f };
		inline constexpr ImVec4 TextWhite{ 1.00f, 1.00f, 1.00f, 1.00f };

		// 边框
		inline constexpr ImVec4 Border{ 0.88f, 0.90f, 0.92f, 1.00f };  // #E1E6EB

		// 语义色
		inline constexpr ImVec4 Success{ 0.18f, 0.75f, 0.48f, 1.00f };  // #2EBF7A
		inline constexpr ImVec4 Warning{ 0.96f, 0.76f, 0.19f, 1.00f };  // #F5C230
		inline constexpr ImVec4 Error{ 0.90f, 0.32f, 0.28f, 1.00f };    // #E65248

		// 高亮匹配标记背景
		inline constexpr ImVec4 Highlight{ 1.00f, 0.85f, 0.30f, 0.45f };  // 黄色高亮

		// 标签色
		inline constexpr ImVec4 TagFile{ 0.22f, 0.55f, 0.87f, 0.80f };
		inline constexpr ImVec4 TagText{ 0.18f, 0.75f, 0.48f, 0.80f };

	}


	struct DocEntry
	{
		std::string displayName;
		std::string filePath;
		std::string rawText;
		bool isFile = true;
		bool selected = true;
	};


	struct QueryTerm
	{
		char word[256] = "";
		int op = 0;
	};

	struct ResultEntry
	{
		size_t docIndex;
		std::string docName;
		std::vector< size_t > positions;
		int currentPos = 0;
	};

	struct PerfRecord
	{
		std::string query;
		std::string mode;
		double timeMs;
		int matchedDocs;
		int totalHits;
	};

	struct IndexRecord
	{
		size_t docCount;
		size_t keyCount;
		size_t posCount;
		double timeMs;
	};

	struct AppState
	{
		struct DemoIndexStep
		{
			size_t docIndex = 0;
			size_t start = 0;
			size_t end = 0;
			size_t hash = 0;
			std::string word;
		};

		struct DemoMergePointerStep
		{
			size_t i = 0;
			size_t j = 0;
			int leftDoc = -1;
			int rightDoc = -1;

			std::string action;
			int addFrom = 0;  // 0:none 1:left 2:right 3:both
		};

		struct DemoMergeStep
		{
			std::string lhs;
			std::string rhs;
			std::string op;
			size_t lhsDocs = 0;
			size_t rhsDocs = 0;
			size_t outDocs = 0;
			std::vector< std::vector< size_t > > lhsRows;
			std::vector< std::vector< size_t > > rhsRows;
			std::vector< std::vector< size_t > > outRows;
			std::vector< DemoMergePointerStep > pointerSteps;
			size_t pointerCursor = 0;
		};
		std::vector< DocEntry > docs;


		Finder<> *finder = nullptr;
		std::atomic< size_t > downCount;
		std::atomic< size_t > currentDocIndex;
		size_t totalCount;

		enum struct IndexStatus
		{
			NotBuilt,
			Append,
			Built,
			Building
		};
		IndexStatus indexStatus = IndexStatus::NotBuilt;
		double indexTimeMs = 0.0;

		enum struct SplitterMode
		{
			Single,
			Simple
		};
		SplitterMode splitterMode = SplitterMode::Simple;
		SimpleSpliter simpleSplitter{};
		SingleCharSpliter singleCharSplitter{};
		char filePathBuf[32768] = "";
		char textBuf[4096] = "";


		std::vector< QueryTerm > queryTerms = { QueryTerm{} };


		bool popAddFile = false;
		bool popAddText = false;
		bool popAbout = false;


		std::vector< ResultEntry > results;
		std::string lastQueryStr;
		double lastQueryMs = 0.0;
		bool searched = false;


		std::vector< PerfRecord > perfHistory;
		std::vector< IndexRecord > indexHistory;

		float leftRatio = 0.35f;
		bool draggingSplit1 = false;


		float splitRatio = 0.35f;
		bool draggingSplit = false;


		int contextChars = 20;


		char indexFilter[128] = "";
		bool showIndexDemoPage = false;
		bool showMergeDemoPage = false;
		struct DemoIndex
		{
			bool isAuto = false;
			int delayMs = 600;
			double lastStepAt = 0.0;
			DemoIndexStep step;
			size_t current = 0;
			HashMap< std::string, StaticQueue< size_t, 6 * 3 > > index;
			HashMap< std::string, StaticQueue< size_t, 6 * 3 > >::iterator currentIt = index.begin();
			void clear()
			{
				isAuto = false;
				index.clear();
				current = 0;
				currentIt = index.begin();
			}
		};
		DemoIndex demoIndex;
		float demoWheelScale = 1.0f;
		ImVec2 demoWheelPan = ImVec2( 0.0f, 0.0f );
		float demoWheelRotation = 0.0f;
		float demoWheelTargetRotation = 0.0f;

		bool demoMergeAuto = false;
		int demoMergeDelayMs = 700;
		double demoMergeLastStepAt = 0.0;
		std::vector< DemoMergeStep > demoMergeSteps;
		size_t demoMergeCursor = 0;
		size_t demoCurrentDocs = 0;
		std::string demoExpr;

		float demoMergeCanvasScale = 1.0f;
		ImVec2 demoMergeCanvasPan = ImVec2( 0.0f, 0.0f );
		float demoAnimI = 0.0f;
		float demoAnimJ = 0.0f;
		float demoAnimOut = 0.0f;
		int demoLastStepSeen = -1;
		bool demoFlyActive = false;
		float demoFlyT = 0.0f;
		ImVec2 demoFlyFrom = ImVec2( 0, 0 );
		ImVec2 demoFlyTo = ImVec2( 0, 0 );
		int demoFlyDoc = -1;
		int demoFlyAddFrom = 0;
		int demoFlyOutIndex = 0;
	};




	void ApplyLightTheme();
	void ShowTxtInMiddle( const char * );

	void RenderDocPanel( AppState &s );
	void RenderSearchPanel( AppState &s );
	void RenderResultPanel( AppState &s );
	void RenderIndexPanel( AppState &s );
	void RenderPerfPanel( AppState &s );
	void RenderStatusBar( AppState &s );
	void RenderPopups( AppState &s );
	void RenderIndexDemoPage( AppState &s );
	void RenderMergeDemoPage( AppState &s );
	void DoSearch( AppState &s );


	std::string ReadDocContent( AppState &s, size_t docIndex, size_t start, size_t end, int len, size_t &newStart, size_t &newEnd );
	std::string WtoU8( const std::wstring &ws );
	void ShowContextSnippet( std::string content, size_t start,
	                         size_t end );

#ifdef _WIN32
	std::vector< std::wstring > OpenFileDialog( GLFWwindow *win );
	std::wstring OpenFolderDialog( GLFWwindow *win );
#endif

	int run();

}
