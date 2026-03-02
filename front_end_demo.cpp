#include "front_end.hpp"

namespace front_end
{
	bool IsAsciiLetterLocal( char c )
	{
		return ( c >= 'a' && c <= 'z' ) || ( c >= 'A' && c <= 'Z' );
	}

	std::string ReadDocAll( const DocEntry &doc )
	{
		if ( !doc.isFile ) return doc.rawText;
		std::ifstream ifs( doc.filePath, std::ios::binary );
		if ( !ifs.is_open() ) return "";
		ifs.seekg( 0, std::ios::end );
		std::string content;
		content.resize( static_cast< size_t >( ifs.tellg() ) );
		ifs.seekg( 0, std::ios::beg );
		ifs.read( &content[0], static_cast< std::streamsize >( content.size() ) );
		return content;
	}

	bool BuildIndexDemoOneStep( AppState &s )
	{
		bool ok = false;
		switch ( s.splitterMode )
		{
		case AppState::SplitterMode::Single :
			ok = s.finder->step_next( s.singleCharSplitter,
			                          s.demoIndex.step.start, s.demoIndex.step.end, s.demoIndex.step.docIndex, s.demoIndex.step.word );
			break;
		default :
			ok = s.finder->step_next( s.simpleSplitter,
			                          s.demoIndex.step.start, s.demoIndex.step.end, s.demoIndex.step.docIndex, s.demoIndex.step.word );
			break;
		}
		if ( ok )
		{
			s.demoIndex.step.hash = s.demoIndex.index.hash( s.demoIndex.step.word );

			auto &q = s.demoIndex.index[s.demoIndex.step.word];
			q.push( s.demoIndex.step.docIndex );
			q.push( s.demoIndex.step.start );
			q.push( s.demoIndex.step.end );
			s.demoIndex.currentIt = s.demoIndex.index.find( s.demoIndex.step.word );
			s.demoWheelRotation = 0.0f;
			++s.demoIndex.current;
		}
		return ok;
		// s.demoIndex.step.docIndex = 0;
		// s.demoIndex.step.start = 0;
		// s.demoIndex.step.end = 1;
		// s.demoIndex.step.word = "mock";
		// auto &q = s.demoIndex.index[s.demoIndex.step.word];
		// q.push( s.demoIndex.step.docIndex );
		// q.push( s.demoIndex.step.start );
		// q.push( s.demoIndex.step.end );
		// s.demoIndex.currentIt = s.demoIndex.index.find( s.demoIndex.step.word );
		// ++s.demoIndex.current;
		// return true;
	}



	AppState::DemoMergeStep BuildMergeStepTrace( const std::vector< std::vector< size_t > > &lhsRows,
	                                             const std::vector< std::vector< size_t > > &rhsRows,
	                                             const std::string &lhs,
	                                             const std::string &rhs,
	                                             const std::string &op )
	{
		AppState::DemoMergeStep st;
		st.lhs = lhs;
		st.rhs = rhs;
		st.op = op;
		st.lhsRows = lhsRows;
		st.rhsRows = rhsRows;
		st.lhsDocs = lhsRows.size();
		st.rhsDocs = rhsRows.size();

		size_t i = 0, j = 0;
		while ( i < lhsRows.size() && j < rhsRows.size() )
		{
			size_t a = lhsRows[i][0], b = rhsRows[j][0];
			if ( a == b )
			{
				std::vector< size_t > merged = lhsRows[i];
				if ( op == "OR" ) merged.insert( merged.end(), rhsRows[j].begin() + 1, rhsRows[j].end() );
				st.outRows.push_back( std::move( merged ) );
				st.pointerSteps.push_back( AppState::DemoMergePointerStep{ i, j, static_cast< int >( a ), static_cast< int >( b ),
				                                                           "左右相同 收录", 3 } );
				++i;
				++j;
			}
			else if ( a < b )
			{
				if ( op == "OR" )
				{
					st.outRows.push_back( lhsRows[i] );
					st.pointerSteps.push_back( AppState::DemoMergePointerStep{ i, j, static_cast< int >( a ), static_cast< int >( b ), "左小，收录左侧", 1 } );
				}
				else
					st.pointerSteps.push_back( AppState::DemoMergePointerStep{ i, j, static_cast< int >( a ), static_cast< int >( b ), "左小 跳过左侧", 0 } );
				++i;
			}
			else
			{
				if ( op == "OR" )
				{
					st.outRows.push_back( rhsRows[j] );
					st.pointerSteps.push_back( AppState::DemoMergePointerStep{ i, j, static_cast< int >( a ), static_cast< int >( b ), "右小 收录右侧", 2 } );
				}
				else
					st.pointerSteps.push_back( AppState::DemoMergePointerStep{ i, j, static_cast< int >( a ), static_cast< int >( b ), "右小 跳过右侧", 0 } );
				++j;
			}
		}

		if ( op == "OR" )
		{
			while ( i < lhsRows.size() )
			{
				st.outRows.push_back( lhsRows[i] );
				st.pointerSteps.push_back( AppState::DemoMergePointerStep{ i, j, static_cast< int >( lhsRows[i][0] ), -1, "右侧耗尽, 追加左侧", 1 } );
				++i;
			}
			while ( j < rhsRows.size() )
			{
				st.outRows.push_back( rhsRows[j] );
				st.pointerSteps.push_back( AppState::DemoMergePointerStep{ i, j, -1, static_cast< int >( rhsRows[j][0] ), "左侧耗尽, 追加右侧", 2 } );
				++j;
			}
		}
		else
		{
			if ( i < lhsRows.size() )
			{
				st.outRows.push_back( lhsRows[i] );
				st.pointerSteps.push_back( AppState::DemoMergePointerStep{ i, j, -1, -1, "右侧耗尽, 结束", 0 } );
			}
			else if ( j < rhsRows.size() )
			{
				st.outRows.push_back( rhsRows[j] );
				st.pointerSteps.push_back( AppState::DemoMergePointerStep{ i, j, -1, -1, "左侧耗尽, 结束", 0 } );
			}
		}

		st.outDocs = st.outRows.size();
		return st;
	}

	void ResetMergeDemo( AppState &s )
	{
		s.demoMergeSteps.clear();
		s.demoMergeCursor = 0;
		s.demoMergeLastStepAt = ImGui::GetTime();
		s.demoExpr.clear();
		s.demoCurrentDocs = 0;
		s.demoLastStepSeen = -1;
		s.demoFlyActive = false;
		s.demoFlyT = 0.0f;
		s.demoFlyAddFrom = 0;
		s.demoFlyOutIndex = 0;
		// for ( size_t i = 0; i < s.docs.size(); ++i )
		// {
		// 	if ( s.docs[i].selected )
		// 		s.finder->select( i );
		// 	else
		// 		s.finder->deselect( i );
		// }

		// if ( s.indexStatus == AppState::IndexStatus::NotBuilt || s.indexStatus == AppState::IndexStatus::Append )
		// {
		// 	s.finder->clear_index();
		// 	if ( s.splitterMode == AppState::SplitterMode::Single )
		// 		s.finder->create_index( SingleCharSpliter{} );
		// 	else
		// 		s.finder->create_index( SimpleSpliter{} );
		// 	s.indexStatus = AppState::IndexStatus::Built;
		// }

		struct ValidTerm
		{
			std::string word;
			int op;
		};
		std::vector< ValidTerm > terms;
		for ( auto &qt : s.queryTerms )
			if ( std::strlen( qt.word ) > 0 ) terms.push_back( { std::string( qt.word ), qt.op } );
		if ( terms.empty() ) return;

		auto acc = s.finder->find( terms[0].word ).array();
		s.demoCurrentDocs = acc.size();
		s.demoExpr = terms[0].word;

		for ( size_t i = 1; i < terms.size(); ++i )
		{
			auto rhsResult = s.finder->find( terms[i].word ).array();
			std::string op = terms[i - 1].op == 0 ? "AND" : "OR";
			auto step = BuildMergeStepTrace( acc, rhsResult, s.demoExpr, terms[i].word, op );
			acc = step.outRows;
			s.demoMergeSteps.push_back( std::move( step ) );
			s.demoExpr = "(" + s.demoExpr + " " + op + " " + terms[i].word + ")";
		}
	}



	void AdvanceMergeDemo( AppState &s )
	{
		if ( s.demoMergeCursor >= s.demoMergeSteps.size() ) return;
		auto &ms = s.demoMergeSteps[s.demoMergeCursor];
		if ( ms.pointerCursor < ms.pointerSteps.size() )
		{
			++ms.pointerCursor;
			if ( ms.pointerCursor == ms.pointerSteps.size() )
			{
				s.demoCurrentDocs = ms.outDocs;
				++s.demoMergeCursor;
			}
		}
		else
			++s.demoMergeCursor;
	}

	void RenderRowsBrief( const std::vector< std::vector< size_t > > &rows, int hi, ImVec4 hiCol )
	{
		for ( int r = 0; r < static_cast< int >( rows.size() ) && r < 6; ++r )
		{
			if ( r == hi )
			{
				ImGui::PushStyleColor( ImGuiCol_Text, hiCol );
				ImGui::Text( "[%zu] doc=%zu", rows[r].size() > 0 ? rows[r].size() : 0, rows[r].empty() ? 0 : rows[r][0] );
				ImGui::PopStyleColor();
			}
			else
				ImGui::Text( "[%zu] doc=%zu", rows[r].size() > 0 ? rows[r].size() : 0, rows[r].empty() ? 0 : rows[r][0] );
		}
	}

	std::string BuildTailInfo( const std::vector< std::vector< size_t > > &rows )
	{
		if ( rows.empty() ) return "(empty)";
		std::string out;
		size_t fromRow = rows.size() > 3 ? rows.size() - 3 : 0;
		for ( size_t r = fromRow; r < rows.size(); ++r )
		{
			auto &v = rows[r];
			out += "doc=" + std::to_string( v.empty() ? 0 : v[0] ) + " [";
			if ( v.size() > 1 )
			{
				size_t from = v.size() > 7 ? v.size() - 6 : 1;
				for ( size_t i = from; i < v.size(); ++i )
				{
					out += std::to_string( v[i] );
					if ( i + 1 < v.size() ) out += ",";
				}
			}
			out += "]";
			if ( r + 1 < rows.size() ) out += "\n";
		}
		return out;
	}
	void ShowTextInCenter( ImDrawList *dl,
	                       ImVec2 center,
	                       const char *text,
	                       float scale )
	{
		if ( !text ) return;

		ImGuiIO &io = ImGui::GetIO();

		ImVec2 text_size = ImGui::CalcTextSize( text );

		float padding = 12.0f * scale;

		ImVec2 box_size(
		    text_size.x * scale + padding * 2,
		    text_size.y * scale + padding * 2 );

		ImVec2 box_min(
		    center.x - box_size.x * 0.5f,
		    center.y - box_size.y * 0.5f );

		ImVec2 box_max(
		    box_min.x + box_size.x,
		    box_min.y + box_size.y );

		dl->AddRectFilled(
		    box_min,
		    box_max,
		    IM_COL32( 0, 0, 0, 0 ),
		    6.0f * scale );
		ImVec2 text_pos(
		    center.x - text_size.x * 0.5f * scale,
		    center.y - text_size.y * 0.5f * scale );

		dl->AddText(
		    ImGui::GetFont(),
		    ImGui::GetFontSize() * scale,
		    text_pos,
		    IM_COL32( 0, 0, 0, 255 ),
		    text );
	}
	void RenderIndexHashWheel( AppState &s, float canvasH )
	{
		ImGui::BeginChild( "##HashWheelCanvas", ImVec2( 0, canvasH ), ImGuiChildFlags_Borders );
		ImVec2 p0 = ImGui::GetCursorScreenPos();
		ImVec2 sz = ImGui::GetContentRegionAvail();
		if ( sz.x < 50 || sz.y < 50 )
		{
			ImGui::EndChild();
			return;
		}
		ImGui::InvisibleButton( "##wheel_area", sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonMiddle | ImGuiButtonFlags_MouseButtonRight );
		bool hovered = ImGui::IsItemHovered();
		ImGuiIO &io = ImGui::GetIO();
		if ( hovered )
		{
			if ( io.MouseWheel != 0.0f )
			{
				s.demoWheelScale *= ( io.MouseWheel > 0 ? 1.1f : 0.9f );
				if ( s.demoWheelScale < 0.35f ) s.demoWheelScale = 0.35f;
				if ( s.demoWheelScale > 3.0f ) s.demoWheelScale = 3.0f;
			}
			if ( ImGui::IsMouseDragging( ImGuiMouseButton_Left ) )
				s.demoWheelPan.x += io.MouseDelta.x, s.demoWheelPan.y += io.MouseDelta.y;
			if ( ImGui::IsMouseDragging( ImGuiMouseButton_Right ) )
				s.demoWheelRotation += ( io.MouseDelta.y + io.MouseDelta.x ) * 0.01f;
		}

		ImDrawList *dl = ImGui::GetWindowDrawList();
		ImVec2 center = ImVec2( p0.x + sz.x * 0.5f + s.demoWheelPan.x, p0.y + sz.y * 0.5f + s.demoWheelPan.y );
		float radius = ( std::min( sz.x, sz.y ) * 0.43f ) * s.demoWheelScale;
		float inner = radius * 0.5f;
		dl->AddRectFilled( p0, ImVec2( p0.x + sz.x, p0.y + sz.y ), ImGui::ColorConvertFloat4ToU32( ImVec4( 0.985f, 0.99f, 1.0f, 0.6f ) ) );

		if ( s.demoIndex.index.empty() )
		{
			ShowTxtInMiddle( "索引为空" );
			ImGui::EndChild();
			return;
		}

		const int maxShow = 20;
		int n = static_cast< int >( s.demoIndex.index.size() );
		int showN = n > maxShow ? maxShow : n;
		bool folded = n > maxShow;
		float oneAngel = ( ( 2.0f ) / static_cast< float >( showN ) ) * IM_PI;

		while ( s.demoWheelRotation > oneAngel )
		{
			s.demoWheelRotation -= oneAngel, s.demoIndex.currentIt.to_prev();
			// std::cout << s.demoIndex.currentIt.index << std::endl;
		}
		while ( s.demoWheelRotation < -oneAngel )
		{
			s.demoWheelRotation += oneAngel, s.demoIndex.currentIt.to_next();
			// std::cout << s.demoIndex.currentIt.index << std::endl;
		}
		// std::cout << s.demoWheelRotation << std::endl;
		auto it = s.demoIndex.currentIt;
		auto rit = s.demoIndex.currentIt.next();

		auto &e = s.demoWheelRotation > oneAngel * 0.5f ? *it.prev() : ( s.demoWheelRotation < -oneAngel * 0.5f ? *( rit ) : *it );
		auto &q = e.second;

		int r = showN / 2;
		int l = showN - r;
		if ( folded && s.demoWheelRotation > 0.0f )
			++l, --r;
		float a0 = s.demoWheelRotation - oneAngel * 0.5f + IM_PI * 1.5f;
		while ( a0 > IM_PI * 2.0f )
			a0 -= IM_PI * 2.0f;
		while ( a0 < -IM_PI * 2.0f )
			a0 += IM_PI * 2.0f;
		float a1 = a0 + oneAngel;
		float a2 = a1;
		float a3 = a2 + oneAngel;
		size_t size = s.demoIndex.index.size();

		for ( int i = 0; i < r; ++i, rit.to_next() )
		{
			ImU32 col;
			// if ( folded && a0 < IM_PI * 0.5f && a1 > IM_PI * 0.5f )
			// 	col = ImGui::ColorConvertFloat4ToU32( ImVec4( 0.75f, 0.75f, 0.78f, 0.6f ) );
			// else
			// {
			float h = static_cast< float >( rit.index ) /
			          size;
			col = ImGui::GetColorU32(  // ImVec4( 0.75f, 0.75f, 0.78f, 0.6f )
			    ImVec4( ImColor::HSV( h, 0.6f, 0.88f, 0.8f ) ) );
			// }
			dl->PathClear();
			dl->PathArcTo( center, radius, a2, a3, 40 );
			dl->PathArcTo( center, inner, a3, a2, 40 );
			dl->PathFillConcave( col );

			if ( a3 > IM_PI * 2.0f )
				a3 -= IM_PI * 2.0f;
			a2 = a3;
			a3 += oneAngel;
		}

		for ( int i = 0; i < l; ++i, it.to_prev() )
		{
			ImU32 col;
			// if ( folded && a0 < IM_PI * 0.5f && a1 > IM_PI * 0.5f )
			// 	col = ImGui::ColorConvertFloat4ToU32( ImVec4( 0.75f, 0.75f, 0.78f, 0.6f ) );
			// else
			// {
			float h = static_cast< float >( it.index ) /
			          size;
			col = ImGui::GetColorU32( ImVec4( ImColor::HSV( h, 0.6f, 0.88f, 0.8f ) ) );
			// }
			dl->PathClear();
			dl->PathArcTo( center, radius, a0, a1, 40 );
			dl->PathArcTo( center, inner, a1, a0, 40 );
			dl->PathFillConcave( col );

			if ( a0 < -IM_PI * 2.0f )
				a0 += IM_PI * 2.0f;
			a1 = a0;
			a0 -= oneAngel;
		}
		if ( folded )
		{
			// 	dl->PathClear();
			// 	dl->PathArcTo( center, radius, a0, a1, 40 );
			// 	dl->PathArcTo( center, inner, a1, a0, 40 );
			// 	dl->PathFillConcave( ImGui::GetColorU32( ImVec4( 0.75f, 0.75f, 0.75f, 1.0f )
			// 	//  ImVec4( ImColor::HSV( static_cast< float >( it.index ) /size ,0.6f, 0.88f, 0.8f ))
			//  ));
			const float down1 = ( IM_PI - oneAngel ) * 0.5f;
			const float down2 = ( IM_PI + oneAngel ) * 0.5f;
			dl->PathClear();
			dl->PathArcTo( center, radius, down1, down2, 40 );
			dl->PathArcTo( center, inner, down2, down1, 40 );
			dl->PathFillConcave( ImGui::ColorConvertFloat4ToU32( ImVec4( 0.75f, 0.75f, 0.75f, 1.0f ) ) );
		}

		dl->AddCircle( center, radius, ImGui::GetColorU32( palette::Border ), 0, 2.0f );
		dl->AddCircle( center, inner, ImGui::GetColorU32( palette::Border ), 0, 2.0f );
		dl->AddLine( ImVec2( center.x, center.y - radius - 8 ), ImVec2( center.x, center.y - radius + 8 ), ImGui::GetColorU32( palette::Error ), 3.0f );
		std::string label;
		label.reserve( 100 );


		label.append( "key : \"" + e.first + "\"\npos:[" );
		if ( q.full() )
		{
			label.append( "..." );
			for ( size_t i = 2; i < q.size; )
			{
				char buf[70];
				sprintf_s( buf, "(%zu:%zu,%zu)\n", q[++i], q[++i], q[++i] );
				label.append( buf );
			}
		}
		else if ( q.size > 0 )
		{
			for ( size_t i = 0; i < q.size; )
			{
				char buf[70];
				sprintf_s( buf, "(%zu:%zu,%zu)\n", q[i++], q[i++], q[i++] );
				label.append( buf );
			}
		}
		label.append( "]" );
		ShowTextInCenter( dl, center, label.c_str(), s.demoWheelScale );

		// if ( infoSeg >= 0 )
		// {
		// 	std::string k = ( folded && infoSeg == showN - 1 ) ? "..." : s.demoIndex.currentIt;
		// 	std::string body;
		// 	if ( k == "..." )
		// 		body = "键过多，已折叠显示";
		// 	else if ( s.finder->index.find( k ) != s.finder->index.end() )
		// 		body = BuildTailInfo( s.finder->index.at( k ) );
		// 	ImVec2 tip = ImVec2( p0.x + 14, p0.y + 14 );
		// 	dl->AddRectFilled( tip, ImVec2( tip.x + 340, tip.y + 90 ), ImGui::GetColorU32( ImVec4( 1, 1, 1, 0.92f ) ), 6.0f );
		// 	dl->AddRect( tip, ImVec2( tip.x + 340, tip.y + 90 ), ImGui::GetColorU32( palette::Border ), 6.0f, 0, 1.2f );
		// 	dl->AddText( ImVec2( tip.x + 10, tip.y + 8 ), ImGui::GetColorU32( palette::Primary ), ( "key: " + k ).c_str() );
		// 	dl->AddText( ImVec2( tip.x + 10, tip.y + 32 ), ImGui::GetColorU32( palette::TextSub ), body.c_str() );
		// }


		ImGui::EndChild();
	}

	void RenderIndexDemoPage( AppState &s )
	{
		if ( !s.showIndexDemoPage ) return;
		ImGui::SetNextWindowSize( ImVec2( 900, 700 ), ImGuiCond_FirstUseEver );
		ImGui::SetNextWindowBgAlpha( 0.6f );
		if ( !ImGui::Begin( "索引演示", &s.showIndexDemoPage ) )
		{
			ImGui::End();
			return;
		}

		if ( s.indexHistory.empty() )
		{
			ShowTxtInMiddle( "索引历史为空，先创建直接创建索引" );
			ImGui::End();
			return;
		}
		const size_t total = s.indexHistory.back().posCount;

		ImGui::BeginDisabled( s.indexStatus != AppState::IndexStatus::Built || s.demoIndex.current >= total );
		if ( ImGui::Button( "步进", ImVec2( 80, 0 ) ) )
		{
			if ( s.demoIndex.current == 0 )
				s.finder->step_restart();
			BuildIndexDemoOneStep( s );
		}
		ImGui::EndDisabled();

		ImGui::SameLine();
		ImGui::BeginDisabled( !s.demoIndex.isAuto && s.indexStatus != AppState::IndexStatus::Built && s.demoIndex.current >= total );
		if ( ImGui::Button( s.demoIndex.isAuto ? "停止" : "自动", ImVec2( 100, 0 ) ) )
		{
			s.demoIndex.isAuto = !s.demoIndex.isAuto;
			s.indexStatus = s.demoIndex.isAuto ? AppState::IndexStatus::Building : AppState::IndexStatus::Built;
			s.demoIndex.lastStepAt = ImGui::GetTime();
		}
		ImGui::EndDisabled();

		ImGui::SameLine();
		ImGui::SetNextItemWidth( 160 );
		ImGui::SliderInt( "间隔(ms)", &s.demoIndex.delayMs, 50, 2500 );
		ImGui::SameLine();
		if ( ImGui::Button( "重置", ImVec2( 80, 0 ) ) )
		{
			s.demoIndex.clear();
			s.finder->step_restart();
			s.indexStatus = AppState::IndexStatus::Built;
		}
		if ( s.demoIndex.isAuto && s.demoIndex.current < total )
		{
			if ( s.demoIndex.current == 0 )
				s.finder->step_restart();
			double now = ImGui::GetTime();
			if ( ( now - s.demoIndex.lastStepAt ) * 1000.0 >= s.demoIndex.delayMs )
			{
				BuildIndexDemoOneStep( s );
				s.demoIndex.lastStepAt = now;
			}
		}

		ImGui::Text( "进度: %zu / %zu", s.demoIndex.current, total );
		if ( s.demoIndex.current > 0 && s.demoIndex.current <= total )
		{
			auto &step = s.demoIndex.step;
			ImGui::Text( "文档[%zu]: %s", step.docIndex, s.docs[step.docIndex].displayName.c_str() );
			ImGui::Text( "位置: [%zu, %zu) 词: %s -> hash: %zu", step.start, step.end, step.word.c_str(), step.hash );

			ImGui::Text( "", step.word.c_str() );

			size_t newStart, newEnd;
			auto context = ReadDocContent( s, step.docIndex, step.start, step.end, s.contextChars, newStart, newEnd );
			ShowContextSnippet( context, newStart, newEnd );
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Text( "( 滚轮缩放 | 左键拖动画布 | 右键拖动旋转 )" );
			RenderIndexHashWheel( s, ImGui::GetWindowSize().y );
		}
		else
			ShowTxtInMiddle( "点击步进或自动开始逐词演示" );


		ImGui::End();
	}

	size_t GetMergeDemoVisibleOutCount( const AppState::DemoMergeStep &ms )
	{
		size_t cnt = 0;
		size_t upto = std::min( ms.pointerCursor, ms.pointerSteps.size() );
		for ( size_t i = 0; i < upto; ++i )
			if ( ms.pointerSteps[i].addFrom != 0 ) ++cnt;
		return std::min( cnt, ms.outRows.size() );
	}
	void RenderMergeBlocksCanvas( AppState &s, AppState::DemoMergeStep &ms, int hiI, int hiJ, int addFrom )
	{
		float h = ImGui::GetContentRegionAvail().y;
		ImGui::BeginChild( "##MergeCanvas", ImVec2( 0, h ), ImGuiChildFlags_Borders );
		ImVec2 p0 = ImGui::GetCursorScreenPos();
		ImVec2 sz = ImGui::GetContentRegionAvail();
		ImGui::InvisibleButton( "##merge_canvas_btn", sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonMiddle );
		ImGuiIO &io = ImGui::GetIO();
		if ( ImGui::IsItemHovered() )
		{
			if ( io.MouseWheel != 0 )
			{
				s.demoMergeCanvasScale *= ( io.MouseWheel > 0 ? 1.1f : 0.9f );
				if ( s.demoMergeCanvasScale < 0.4f ) s.demoMergeCanvasScale = 0.4f;
				if ( s.demoMergeCanvasScale > 2.8f ) s.demoMergeCanvasScale = 2.8f;
			}
			if ( ImGui::IsMouseDragging( ImGuiMouseButton_Left ) ) s.demoMergeCanvasPan.x += io.MouseDelta.x, s.demoMergeCanvasPan.y += io.MouseDelta.y;
		}
		ImDrawList *dl = ImGui::GetWindowDrawList();
		dl->AddRectFilled( p0, ImVec2( p0.x + sz.x, p0.y + sz.y ), ImGui::GetColorU32( ImVec4( 0.985f, 0.99f, 1.0f, 1.0f ) ) );

		float blockW = 54.0f * s.demoMergeCanvasScale;
		float blockH = 30.0f * s.demoMergeCanvasScale;
		float gap = 14.0f * s.demoMergeCanvasScale;
		ImVec2 base = ImVec2( p0.x + 70 + s.demoMergeCanvasPan.x, p0.y + 36 + s.demoMergeCanvasPan.y );

		auto lerpf = []( float a, float b )
		{
			return a + ( b - a ) * 0.18f;
		};
		s.demoAnimI = lerpf( s.demoAnimI, static_cast< float >( hiI < 0 ? 0 : hiI ) );
		s.demoAnimJ = lerpf( s.demoAnimJ, static_cast< float >( hiJ < 0 ? 0 : hiJ ) );
		s.demoAnimOut = lerpf( s.demoAnimOut, static_cast< float >( s.demoCurrentDocs ) );

		int visible = 3;
		auto clampPtr = [&]( int ptr, size_t n )
		{
			if ( n == 0 ) return 0;
			if ( ptr < 0 ) return 0;
			int maxPtr = static_cast< int >( n - 1 );
			return ptr > maxPtr ? maxPtr : ptr;
		};
		auto drawGroup = [&]( const std::vector< std::vector< size_t > > &rows, ImVec2 org, int ptr, bool horizontal, bool highlightAdd, ImVec4 colBase )
		{
			const float textScale = 0.82f * s.demoMergeCanvasScale;
			ptr = clampPtr( ptr, rows.size() );
			for ( int i = 0; i < static_cast< int >( rows.size() ); ++i )
			{
				float dist = std::fabs( static_cast< float >( i - ptr ) );
				if ( dist > visible + 0.5f ) continue;
				float a = std::max( 0.1f, 1.0f - dist / ( visible + 1.0f ) );
				ImVec4 c = colBase;
				c.w = a;
				if ( i == ptr ) c = highlightAdd ? ImVec4( 0.10f, 0.72f, 0.40f, 1.0f ) : ImVec4( 0.14f, 0.48f, 0.82f, 1.0f );
				float x = org.x + ( horizontal ? ( i - ptr ) * ( blockW + gap ) : 0 );
				float y = org.y + ( horizontal ? 0 : ( i - ptr ) * ( blockH + gap ) );
				dl->AddRectFilled( ImVec2( x, y ), ImVec2( x + blockW, y + blockH ), ImGui::GetColorU32( c ), 5.0f * s.demoMergeCanvasScale );
				dl->AddRect( ImVec2( x, y ), ImVec2( x + blockW, y + blockH ), ImGui::GetColorU32( ImVec4( 1, 1, 1, 0.6f ) ), 5.0f * s.demoMergeCanvasScale );
				if ( !rows[i].empty() )
				{
					std::string str = std::to_string( rows[i][0] );

					float fontSize = ImGui::GetFontSize() * textScale;
					ImFont *font = ImGui::GetFont();
					ImVec2 textSize = font->CalcTextSizeA(
					    fontSize,
					    FLT_MAX,
					    0.0f,
					    str.c_str() );
					float centerX = x + blockW * 0.5f;
					float centerY = y + blockH * 0.5f;
					ImVec2 textPos(
					    centerX - textSize.x * 0.5f,
					    centerY - textSize.y * 0.5f );
					dl->AddText(
					    font,
					    fontSize,
					    textPos,
					    ImGui::GetColorU32( ImVec4( 1, 1, 1, 0.95f ) ),
					    str.c_str() );
				}
				// dl->AddText( ImGui::GetFont(), ImGui::GetFontSize() * textScale, ImVec2( x + 8 * s.demoMergeCanvasScale, y + 7 * s.demoMergeCanvasScale ), ImGui::GetColorU32( ImVec4( 1, 1, 1, 0.95f ) ), std::to_string( rows[i][0] ).c_str() );
			}
		};

		int pi = clampPtr( hiI < 0 ? 0 : static_cast< int >( s.demoAnimI + 0.5f ), ms.lhsRows.size() );
		int pj = clampPtr( hiJ < 0 ? 0 : static_cast< int >( s.demoAnimJ + 0.5f ), ms.rhsRows.size() );
		size_t visibleOutCount = GetMergeDemoVisibleOutCount( ms );
		int po = clampPtr( static_cast< int >( s.demoAnimOut + 0.5f ), ms.outRows.size() );

		ImVec2 leftOrg = base;
		ImVec2 rightOrg = ImVec2( base.x + 190 * s.demoMergeCanvasScale, base.y );
		ImVec2 outOrg = ImVec2( base.x + 95 * s.demoMergeCanvasScale, base.y + 180 * s.demoMergeCanvasScale );

		drawGroup( ms.lhsRows, leftOrg, pi, false, addFrom == 1 || addFrom == 3, ImVec4( 0.45f, 0.64f, 0.85f, 0.5f ) );
		drawGroup( ms.rhsRows, rightOrg, pj, false, addFrom == 2 || addFrom == 3, ImVec4( 0.42f, 0.62f, 0.84f, 0.5f ) );
		std::vector< std::vector< size_t > > shownOutRows;
		shownOutRows.reserve( visibleOutCount );
		for ( size_t i = 0; i < visibleOutCount; ++i ) shownOutRows.push_back( ms.outRows[i] );
		drawGroup( shownOutRows, outOrg, po, true, false, ImVec4( 0.64f, 0.72f, 0.80f, 0.45f ) );
		if ( s.demoFlyActive )
		{
			ImVec2 from = s.demoFlyAddFrom == 2 ? ImVec2( rightOrg.x, rightOrg.y ) : ImVec2( leftOrg.x, leftOrg.y );
			s.demoFlyFrom = from;
			s.demoFlyTo = outOrg;
			s.demoFlyT += 0.08f;
			if ( s.demoFlyT >= 1.0f )
			{
				s.demoFlyT = 1.0f;
				s.demoFlyActive = false;
			}
			ImVec2 pos = ImVec2( s.demoFlyFrom.x + ( s.demoFlyTo.x - s.demoFlyFrom.x ) * s.demoFlyT,
			                     s.demoFlyFrom.y + ( s.demoFlyTo.y - s.demoFlyFrom.y ) * s.demoFlyT );
			dl->AddRectFilled( pos, ImVec2( pos.x + blockW, pos.y + blockH ), ImGui::GetColorU32( ImVec4( 0.18f, 0.78f, 0.45f, 0.95f ) ), 5.0f * s.demoMergeCanvasScale );
			dl->AddText( ImGui::GetFont(), ImGui::GetFontSize() * ( 0.82f * s.demoMergeCanvasScale ), ImVec2( pos.x + 8 * s.demoMergeCanvasScale, pos.y + 7 * s.demoMergeCanvasScale ), ImGui::GetColorU32( ImVec4( 1, 1, 1, 1 ) ), std::to_string( s.demoFlyDoc ).c_str() );
		}
		ImGui::EndChild();
	}

	void RenderMergeDemoPage( AppState &s )
	{
		if ( !s.showMergeDemoPage ) return;
		ImGui::SetNextWindowSize( ImVec2( 980, 700 ), ImGuiCond_FirstUseEver );
		ImGui::SetNextWindowBgAlpha( 0.86f );
		if ( !ImGui::Begin( "合并演示", &s.showMergeDemoPage ) )
		{
			ImGui::End();
			return;
		}
		if ( s.demoMergeSteps.empty() && s.demoMergeCursor == 0 ) ResetMergeDemo( s );

		if ( ImGui::Button( "步进", ImVec2( 80, 0 ) ) ) AdvanceMergeDemo( s );

		ImGui::SameLine();
		if ( ImGui::Button( s.demoMergeAuto ? "停止" : "自动", ImVec2( 100, 0 ) ) )
		{
			s.demoMergeAuto = !s.demoMergeAuto;
			s.demoMergeLastStepAt = ImGui::GetTime();
		}
		ImGui::SameLine();
		ImGui::SetNextItemWidth( 160 );

		ImGui::SliderInt( "间隔(ms)", &s.demoMergeDelayMs, 50, 2500 );
		ImGui::SameLine();
		if ( ImGui::Button( "重置", ImVec2( 80, 0 ) ) ) ResetMergeDemo( s );
		ImGui::Text( "表达式: %s", s.demoExpr.empty() ? "( 空 )" : s.demoExpr.c_str() );
		if ( s.demoMergeAuto && s.demoMergeCursor < s.demoMergeSteps.size() )
		{
			double now = ImGui::GetTime();
			if ( ( now - s.demoMergeLastStepAt ) * 1000.0 >= s.demoMergeDelayMs )
			{
				AdvanceMergeDemo( s );
				s.demoMergeLastStepAt = now;
			}
		}

		if ( !s.demoMergeSteps.empty() )
		{
			size_t stepIndex = s.demoMergeCursor < s.demoMergeSteps.size() ? s.demoMergeCursor : s.demoMergeSteps.size() - 1;
			auto &ms = s.demoMergeSteps[stepIndex];
			ImGui::SameLine();
			ImGui::Text( " 当前: %s %s %s", ms.lhs.c_str(), ms.op.c_str(), ms.rhs.c_str() );
			// ImGui::Text( "结果文档: %zu -> %zu", ms.lhsDocs + ms.rhsDocs, ms.outDocs );

			int hiI = -1, hiJ = -1, addFrom = 0, srcDoc = -1;
			if ( ms.pointerCursor > 0 )
			{
				auto &ps = ms.pointerSteps[ms.pointerCursor - 1];
				hiI = static_cast< int >( ps.i );
				hiJ = static_cast< int >( ps.j );
				addFrom = ps.addFrom;
				srcDoc = addFrom == 2 ? ps.rightDoc : ps.leftDoc;
				ImGui::PushStyleColor( ImGuiCol_Text, palette::Primary );
				ImGui::Text( "动作: %s", ps.action.c_str() );
				ImGui::PopStyleColor();
				if ( static_cast< int >( ms.pointerCursor ) - 1 != s.demoLastStepSeen && addFrom != 0 )
				{
					s.demoFlyActive = true;
					s.demoFlyT = 0.0f;
					s.demoFlyDoc = srcDoc;
					s.demoFlyAddFrom = addFrom;
					size_t visibleOutCount = GetMergeDemoVisibleOutCount( ms );
					s.demoFlyOutIndex = visibleOutCount == 0 ? 0 : static_cast< int >( visibleOutCount - 1 );
				}
				s.demoLastStepSeen = static_cast< int >( ms.pointerCursor ) - 1;
			}
			ImGui::Text( "步骤: %zu / %zu", ms.pointerCursor, ms.pointerSteps.size() );
			RenderMergeBlocksCanvas( s, ms, hiI, hiJ, addFrom );
		}
		else
			ShowTxtInMiddle( "至少两个查询词才能演示合并" );

		ImGui::End();
	}
}