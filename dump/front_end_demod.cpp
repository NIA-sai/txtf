bool IsAsciiLetterLocal( char c )
{
	return ( c >= 'a' && c <= 'z' ) || ( c >= 'A' && c <= 'Z' );
}

std::string ReadDocAll( const DocEntry &doc )
{
	if ( !doc.isFile ) return doc.rawText;
	std::ifstream ifs( WtoU8( doc.filePath ), std::ios::binary );
	if ( !ifs.is_open() ) return "";
	ifs.seekg( 0, std::ios::end );
	std::string content;
	content.resize( static_cast< size_t >( ifs.tellg() ) );
	ifs.seekg( 0, std::ios::beg );
	ifs.read( &content[0], static_cast< std::streamsize >( content.size() ) );
	return content;
}


void ShowTxtInMiddle( const char *txt );
void BuildIndexDemoSteps( AppState &s )
{
	s.demoIndexSteps.clear();
	s.demoIndexCursor = 0;
	s.demoIndexLastStepAt = ImGui::GetTime();
	for ( size_t docIndex = 0; docIndex < s.docs.size(); ++docIndex )
	{
		auto &doc = s.docs[docIndex];
		if ( !doc.selected ) continue;
		std::string content = ReadDocAll( doc );
		if ( content.empty() ) continue;

		if ( s.splitterMode == AppState::SplitterMode::Simple )
		{
			std::string buf;
			size_t tokenStart = static_cast< size_t >( -1 );
			for ( size_t i = 0; i < content.size(); ++i )
			{
				if ( IsAsciiLetterLocal( content[i] ) )
				{
					buf.push_back( content[i] );
					if ( tokenStart == static_cast< size_t >( -1 ) ) tokenStart = i;
				}
				else if ( !buf.empty() )
				{
					AppState::DemoIndexStep step;
					step.docIndex = docIndex;
					step.docName = doc.displayName;
					step.token = buf;
					step.start = tokenStart;
					step.end = i;
					step.context = ReadDocContent( s, docIndex, step.start, step.end, 14, step.contextStart, step.contextEnd );
					s.demoIndexSteps.push_back( std::move( step ) );
					buf.clear();
					tokenStart = static_cast< size_t >( -1 );
				}
			}
			if ( !buf.empty() )
			{
				AppState::DemoIndexStep step;
				step.docIndex = docIndex;
				step.docName = doc.displayName;
				step.token = buf;
				step.start = tokenStart;
				step.end = content.size();
				step.context = ReadDocContent( s, docIndex, step.start, step.end, 14, step.contextStart, step.contextEnd );
				s.demoIndexSteps.push_back( std::move( step ) );
			}
		}
		else
		{
			for ( size_t i = 0; i < content.size(); )
			{
				unsigned char c = static_cast< unsigned char >( content[i] );
				size_t len = 0;
				if ( c < 0x80 )
					len = 1;
				else if ( ( c & 0xE0 ) == 0xC0 )
					len = 2;
				else if ( ( c & 0xF0 ) == 0xE0 )
					len = 3;
				else if ( ( c & 0xF8 ) == 0xF0 )
					len = 4;
				if ( len == 0 )
				{
					++i;
					continue;
				}
				if ( i + len > content.size() ) len = content.size() - i;
				AppState::DemoIndexStep step;
				step.docIndex = docIndex;
				step.docName = doc.displayName;
				step.start = i;
				step.end = i + len;
				step.token = content.substr( i, len );
				step.context = ReadDocContent( s, docIndex, step.start, step.end, 10, step.contextStart, step.contextEnd );
				s.demoIndexSteps.push_back( std::move( step ) );
				i += len;
			}
		}
	}
}

std::vector< std::vector< size_t > > ResultRows( const Finder< char >::Result &r )
{
	std::vector< std::vector< size_t > > rows;
	for ( auto &v : r.array() ) rows.push_back( v );
	return rows;
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
			std::vector< size_t > merged{ a };
			AppState::DemoMergePointerStep outer;
			outer.i = i;
			outer.j = j;
			outer.leftDoc = static_cast< int >( a );
			outer.rightDoc = static_cast< int >( b );
			outer.action = "doc id 相同，进入位置双指针";
			st.pointerSteps.push_back( outer );

			size_t k = 1, l = 1, resti = 1;
			while ( k < lhsRows[i].size() && l < rhsRows[j].size() )
			{
				AppState::DemoMergePointerStep inner;
				inner.i = i;
				inner.j = j;
				inner.leftDoc = static_cast< int >( a );
				inner.rightDoc = static_cast< int >( b );
				inner.k = static_cast< int >( k );
				inner.l = static_cast< int >( l );
				inner.leftPos = lhsRows[i][k];
				inner.rightPos = rhsRows[j][l];

				if ( lhsRows[i][k] == rhsRows[j][l] )
				{
					if ( lhsRows[i][k + 1] < rhsRows[j][l + 1] )
					{
						merged.insert( merged.end(), { lhsRows[i][k++], lhsRows[i][k++] } );
						merged.insert( merged.end(), { rhsRows[j][l++], rhsRows[j][l++] } );
						inner.action = "同起点，先加左后加右";
					}
					else if ( lhsRows[i][k + 1] > rhsRows[j][l + 1] )
					{
						merged.insert( merged.end(), { rhsRows[j][l++], rhsRows[j][l++] } );
						merged.insert( merged.end(), { lhsRows[i][k++], lhsRows[i][k++] } );
						inner.action = "同起点，先加右后加左";
					}
					else
					{
						merged.insert( merged.end(), { lhsRows[i][k++], lhsRows[i][k++] } );
						inner.action = "区间完全重合，保留一份";
					}
					inner.addFrom = 3;
				}
				else if ( lhsRows[i][k] < rhsRows[j][l] )
				{
					merged.insert( merged.end(), { lhsRows[i][k++], lhsRows[i][k++] } );
					inner.action = "左位置更小，加入左并推进 k";
					inner.addFrom = 1;
				}
				else
				{
					merged.insert( merged.end(), { rhsRows[j][l++], rhsRows[j][l++] } );
					inner.action = "右位置更小，加入右并推进 l";
					inner.addFrom = 2;
				}
				st.pointerSteps.push_back( inner );
			}

			if ( k >= lhsRows[i].size() )
				resti = l;
			else
				resti = k;
			auto &rest = ( k >= lhsRows[i].size() ) ? rhsRows[j] : lhsRows[i];
			if ( resti < rest.size() )
			{
				merged.insert( merged.end(), rest.begin() + resti, rest.end() );
				AppState::DemoMergePointerStep tail;
				tail.i = i;
				tail.j = j;
				tail.leftDoc = static_cast< int >( a );
				tail.rightDoc = static_cast< int >( b );
				tail.action = "追加剩余区间";
				tail.addFrom = ( k >= lhsRows[i].size() ) ? 2 : 1;
				st.pointerSteps.push_back( tail );
			}

			st.outRows.push_back( std::move( merged ) );
			++i;
			++j;
		}
		else if ( a < b )
		{
			if ( op == "OR" )
			{
				st.outRows.push_back( lhsRows[i] );
				st.pointerSteps.push_back( AppState::DemoMergePointerStep{ i, j, static_cast< int >( a ), static_cast< int >( b ), -1, -1, 0, 0, "左 doc 更小，OR 收录左侧", 1 } );
			}
			else
				st.pointerSteps.push_back( AppState::DemoMergePointerStep{ i, j, static_cast< int >( a ), static_cast< int >( b ), -1, -1, 0, 0, "左 doc 更小，AND 跳过左侧", 0 } );
			++i;
		}
		else
		{
			if ( op == "OR" )
			{
				st.outRows.push_back( rhsRows[j] );
				st.pointerSteps.push_back( AppState::DemoMergePointerStep{ i, j, static_cast< int >( a ), static_cast< int >( b ), -1, -1, 0, 0, "右 doc 更小，OR 收录右侧", 2 } );
			}
			else
				st.pointerSteps.push_back( AppState::DemoMergePointerStep{ i, j, static_cast< int >( a ), static_cast< int >( b ), -1, -1, 0, 0, "右 doc 更小，AND 跳过右侧", 0 } );
			++j;
		}
	}

	if ( op == "OR" )
	{
		while ( i < lhsRows.size() )
		{
			st.outRows.push_back( lhsRows[i] );
			st.pointerSteps.push_back( AppState::DemoMergePointerStep{ i, j, static_cast< int >( lhsRows[i][0] ), -1, -1, -1, 0, 0, "右侧耗尽，追加左侧 doc", 1 } );
			++i;
		}
		while ( j < rhsRows.size() )
		{
			st.outRows.push_back( rhsRows[j] );
			st.pointerSteps.push_back( AppState::DemoMergePointerStep{ i, j, -1, static_cast< int >( rhsRows[j][0] ), -1, -1, 0, 0, "左侧耗尽，追加右侧 doc", 2 } );
			++j;
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

	for ( size_t i = 0; i < s.docs.size(); ++i )
	{
		if ( s.docs[i].selected )
			s.finder->select( i );
		else
			s.finder->deselect( i );
	}

	if ( s.indexStatus == AppState::IndexStatus::NotBuilt || s.indexStatus == AppState::IndexStatus::Append )
	{
		s.finder->clear_index();
		if ( s.splitterMode == AppState::SplitterMode::Single )
			s.finder->create_index( SingleCharSpliter{} );
		else
			s.finder->create_index( SimpleSpliter{} );
		s.indexStatus = AppState::IndexStatus::Built;
	}

	struct ValidTerm
	{
		std::string word;
		int op;
	};
	std::vector< ValidTerm > terms;
	for ( auto &qt : s.queryTerms )
		if ( std::strlen( qt.word ) > 0 ) terms.push_back( { std::string( qt.word ), qt.op } );
	if ( terms.empty() ) return;

	auto acc = s.finder->find( terms[0].word );
	auto accRows = ResultRows( acc );
	s.demoCurrentDocs = accRows.size();
	s.demoExpr = terms[0].word;

	for ( size_t i = 1; i < terms.size(); ++i )
	{
		auto rhsResult = s.finder->find( terms[i].word );
		auto rhsRows = ResultRows( rhsResult );
		std::string op = terms[i - 1].op == 0 ? "AND" : "OR";
		auto step = BuildMergeStepTrace( accRows, rhsRows, s.demoExpr, terms[i].word, op );
		accRows = step.outRows;
		s.demoMergeSteps.push_back( std::move( step ) );
		s.demoExpr = "(" + s.demoExpr + " " + op + " " + terms[i].word + ")";
	}
}

void AdvanceIndexDemo( AppState &s )
{
	if ( s.demoIndexCursor < s.demoIndexSteps.size() ) ++s.demoIndexCursor;
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
		if ( ImGui::IsMouseDragging( ImGuiMouseButton_Middle ) )
			s.demoWheelPan.x += io.MouseDelta.x, s.demoWheelPan.y += io.MouseDelta.y;
		if ( ImGui::IsMouseDragging( ImGuiMouseButton_Right ) )
			s.demoWheelRotation += io.MouseDelta.x * 0.01f;
	}

	ImDrawList *dl = ImGui::GetWindowDrawList();
	ImVec2 center = ImVec2( p0.x + sz.x * 0.5f + s.demoWheelPan.x, p0.y + sz.y * 0.5f + s.demoWheelPan.y );
	float radius = ( std::min( sz.x, sz.y ) * 0.43f ) * s.demoWheelScale;
	float inner = radius * 0.42f;
	dl->AddRectFilled( p0, ImVec2( p0.x + sz.x, p0.y + sz.y ), ImGui::ColorConvertFloat4ToU32( ImVec4( 0.985f, 0.99f, 1.0f, 1.0f ) ) );

	std::vector< std::string > keys;
	for ( auto it = s.finder->index.begin(); it != s.finder->index.end(); ++it ) keys.push_back( it->first );
	if ( keys.empty() )
	{
		ShowTxtInMiddle( "索引为空，先创建索引或步进索引演示" );
		ImGui::EndChild();
		return;
	}

	const int maxShow = 48;
	int n = static_cast< int >( keys.size() );
	int showN = n > maxShow ? maxShow : n;
	bool folded = n > maxShow;

	std::string currentKey;
	if ( s.demoIndexCursor > 0 && s.demoIndexCursor <= s.demoIndexSteps.size() )
		currentKey = s.demoIndexSteps[s.demoIndexCursor - 1].token;

	int hi = -1;
	for ( int i = 0; i < showN; ++i )
		if ( keys[i] == currentKey )
		{
			hi = i;
			break;
		}
	if ( hi >= 0 )
	{
		float centerA = s.demoWheelRotation + ( ( hi + 0.5f ) / static_cast< float >( showN ) ) * IM_PI * 2.0f;
		s.demoWheelTargetRotation = -IM_PI * 0.5f - centerA;
	}
	s.demoWheelRotation += ( s.demoWheelTargetRotation - s.demoWheelRotation ) * 0.16f;

	int hoveredSeg = -1;
	ImVec2 mouse = io.MousePos;
	float md = std::sqrt( ( mouse.x - center.x ) * ( mouse.x - center.x ) + ( mouse.y - center.y ) * ( mouse.y - center.y ) );
	if ( hovered && md >= inner && md <= radius )
	{
		float a = std::atan2( mouse.y - center.y, mouse.x - center.x ) - s.demoWheelRotation;
		while ( a < 0 ) a += IM_PI * 2.0f;
		while ( a >= IM_PI * 2.0f ) a -= IM_PI * 2.0f;
		hoveredSeg = static_cast< int >( a / ( IM_PI * 2.0f ) * showN );
		if ( hoveredSeg >= showN ) hoveredSeg = showN - 1;
	}

	for ( int i = 0; i < showN; ++i )
	{
		float a0 = s.demoWheelRotation + ( i / static_cast< float >( showN ) ) * IM_PI * 2.0f;
		float a1 = s.demoWheelRotation + ( ( i + 1 ) / static_cast< float >( showN ) ) * IM_PI * 2.0f;
		ImU32 col;
		if ( folded && i == showN - 1 )
			col = ImGui::ColorConvertFloat4ToU32( ImVec4( 0.75f, 0.75f, 0.78f, 1.0f ) );
		else
		{
			float h = static_cast< float >( i ) / static_cast< float >( showN );
			col = ImGui::GetColorU32(
			    ImVec4( ImColor::HSV( h, 0.6f, 0.88f ) ) );
		}
		if ( i == hi ) col = ImGui::GetColorU32( ImVec4( 0.98f, 0.55f, 0.22f, 1.0f ) );
		if ( i == hoveredSeg ) col = ImGui::GetColorU32( ImVec4( 0.22f, 0.55f, 0.87f, 1.0f ) );

		dl->PathClear();
		dl->PathArcTo( center, radius, a0, a1, 20 );
		dl->PathArcTo( center, inner, a1, a0, 20 );
		dl->PathFillConvex( col );
	}

	dl->AddCircle( center, radius, ImGui::GetColorU32( palette::Border ), 0, 2.0f );
	dl->AddCircle( center, inner, ImGui::GetColorU32( palette::Border ), 0, 2.0f );
	dl->AddLine( ImVec2( center.x, center.y - radius - 8 ), ImVec2( center.x, center.y - radius + 8 ), ImGui::GetColorU32( palette::Error ), 3.0f );

	int infoSeg = hoveredSeg >= 0 ? hoveredSeg : hi;
	if ( infoSeg >= 0 )
	{
		std::string k = ( folded && infoSeg == showN - 1 ) ? "..." : keys[infoSeg];
		std::string body;
		if ( k == "..." )
			body = "键过多，已折叠显示";
		else if ( s.finder->index.find( k ) != s.finder->index.end() )
			body = BuildTailInfo( s.finder->index.at( k ) );
		ImVec2 tip = ImVec2( p0.x + 14, p0.y + 14 );
		dl->AddRectFilled( tip, ImVec2( tip.x + 340, tip.y + 90 ), ImGui::GetColorU32( ImVec4( 1, 1, 1, 0.92f ) ), 6.0f );
		dl->AddRect( tip, ImVec2( tip.x + 340, tip.y + 90 ), ImGui::GetColorU32( palette::Border ), 6.0f, 0, 1.2f );
		dl->AddText( ImVec2( tip.x + 10, tip.y + 8 ), ImGui::GetColorU32( palette::Primary ), ( "key: " + k ).c_str() );
		dl->AddText( ImVec2( tip.x + 10, tip.y + 32 ), ImGui::GetColorU32( palette::TextSub ), body.c_str() );
	}

	if ( folded )
	{
		ImGui::SetCursorScreenPos( ImVec2( p0.x + 8, p0.y + sz.y - 24 ) );
		ImGui::TextDisabled( "keys=%d, 显示前 %d + ...", n, maxShow - 1 );
	}
	ImGui::EndChild();
}

void RenderIndexDemoPage( AppState &s )
{
	if ( !s.showIndexDemoPage ) return;
	ImGui::SetNextWindowSize( ImVec2( 900, 700 ), ImGuiCond_FirstUseEver );
	if ( !ImGui::Begin( "索引演示", &s.showIndexDemoPage ) )
	{
		ImGui::End();
		return;
	}
	if ( s.demoIndexSteps.empty() ) BuildIndexDemoSteps( s );

	ImGui::SetNextItemWidth( 160 );
	ImGui::SliderInt( "延迟(ms)", &s.demoIndexDelayMs, 100, 2500 );
	if ( ImGui::Button( "步进", ImVec2( 80, 0 ) ) ) AdvanceIndexDemo( s );
	ImGui::SameLine();
	if ( ImGui::Button( s.demoIndexAuto ? "停止自动" : "自动播放", ImVec2( 100, 0 ) ) )
	{
		s.demoIndexAuto = !s.demoIndexAuto;
		s.demoIndexLastStepAt = ImGui::GetTime();
	}
	ImGui::SameLine();
	if ( ImGui::Button( "重置", ImVec2( 80, 0 ) ) ) BuildIndexDemoSteps( s );

	if ( s.demoIndexAuto && s.demoIndexCursor < s.demoIndexSteps.size() )
	{
		double now = ImGui::GetTime();
		if ( ( now - s.demoIndexLastStepAt ) * 1000.0 >= s.demoIndexDelayMs )
		{
			AdvanceIndexDemo( s );
			s.demoIndexLastStepAt = now;
		}
	}

	ImGui::Text( "进度: %zu / %zu", s.demoIndexCursor, s.demoIndexSteps.size() );
	if ( s.demoIndexCursor > 0 && s.demoIndexCursor <= s.demoIndexSteps.size() )
	{
		auto &step = s.demoIndexSteps[s.demoIndexCursor - 1];
		ImGui::Text( "文档[%zu]: %s", step.docIndex, step.docName.c_str() );
		ImGui::Text( "词: %s", step.token.c_str() );
		ImGui::Text( "位置: [%zu, %zu)", step.start, step.end );
		ShowContextSnippet( step.context, step.contextStart, step.contextEnd );
	}
	else
		ShowTxtInMiddle( "点击步进开始逐词演示" );

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Text( "HashMap 轮盘画布（滚轮缩放 / 中键拖动画布 / 右键拖动旋转）" );
	RenderIndexHashWheel( s, ImGui::GetContentRegionAvail().y );

	ImGui::End();
}


void RenderMergeDemoPage( AppState &s )
{
	if ( !s.showMergeDemoPage ) return;
	ImGui::SetNextWindowSize( ImVec2( 920, 620 ), ImGuiCond_FirstUseEver );
	if ( !ImGui::Begin( "合并演示", &s.showMergeDemoPage ) )
	{
		ImGui::End();
		return;
	}
	if ( s.demoMergeSteps.empty() && s.demoMergeCursor == 0 ) ResetMergeDemo( s );

	ImGui::SetNextItemWidth( 160 );
	ImGui::SliderInt( "延迟(ms)", &s.demoMergeDelayMs, 100, 2500 );
	if ( ImGui::Button( "步进", ImVec2( 80, 0 ) ) ) AdvanceMergeDemo( s );
	ImGui::SameLine();
	if ( ImGui::Button( s.demoMergeAuto ? "停止自动" : "自动播放", ImVec2( 100, 0 ) ) )
	{
		s.demoMergeAuto = !s.demoMergeAuto;
		s.demoMergeLastStepAt = ImGui::GetTime();
	}
	ImGui::SameLine();
	if ( ImGui::Button( "重置", ImVec2( 80, 0 ) ) ) ResetMergeDemo( s );
	ImGui::Text( "表达式: %s", s.demoExpr.empty() ? "(空)" : s.demoExpr.c_str() );

	if ( s.demoMergeAuto && s.demoMergeCursor < s.demoMergeSteps.size() )
	{
		double now = ImGui::GetTime();
		if ( ( now - s.demoMergeLastStepAt ) * 1000.0 >= s.demoMergeDelayMs )
		{
			AdvanceMergeDemo( s );
			s.demoMergeLastStepAt = now;
		}
	}

	if ( s.demoMergeCursor < s.demoMergeSteps.size() )
	{
		auto &ms = s.demoMergeSteps[s.demoMergeCursor];
		ImGui::Text( "当前: %s %s %s", ms.lhs.c_str(), ms.op.c_str(), ms.rhs.c_str() );
		ImGui::Text( "结果文档: %zu -> %zu", ms.lhsDocs + ms.rhsDocs, ms.outDocs );
		ImGui::Separator();

		int hiI = -1, hiJ = -1;
		int addFrom = 0;
		if ( ms.pointerCursor > 0 )
		{
			auto &ps = ms.pointerSteps[ms.pointerCursor - 1];
			hiI = static_cast< int >( ps.i );
			hiJ = static_cast< int >( ps.j );
			addFrom = ps.addFrom;
			ImGui::Text( "外层指针: i=%zu(doc=%d), j=%zu(doc=%d)", ps.i, ps.leftDoc, ps.j, ps.rightDoc );
			if ( ps.k >= 0 || ps.l >= 0 )
				ImGui::Text( "内层指针: k=%d(pos=%zu), l=%d(pos=%zu)", ps.k, ps.leftPos, ps.l, ps.rightPos );
			ImGui::PushStyleColor( ImGuiCol_Text, palette::Primary );
			ImGui::Text( "动作: %s", ps.action.c_str() );
			ImGui::PopStyleColor();
		}
		ImGui::Text( "步骤: %zu / %zu", ms.pointerCursor, ms.pointerSteps.size() );

		ImGui::Columns( 3, "##mergecols", false );
		ImGui::Text( "左 Result" );
		RenderRowsBrief( ms.lhsRows, hiI, addFrom == 1 || addFrom == 3 ? palette::Success : palette::Primary );
		ImGui::NextColumn();
		ImGui::Text( "右 Result" );
		RenderRowsBrief( ms.rhsRows, hiJ, addFrom == 2 || addFrom == 3 ? palette::Success : palette::Primary );
		ImGui::NextColumn();
		ImGui::Text( "输出 Result(预览)" );
		for ( int r = 0; r < static_cast< int >( ms.outRows.size() ) && r < 6; ++r )
			ImGui::Text( "doc=%zu", ms.outRows[r].empty() ? 0 : ms.outRows[r][0] );
		ImGui::Columns( 1 );
	}
	else if ( s.demoMergeSteps.empty() )
		ShowTxtInMiddle( "至少两个查询词才能演示合并" );
	else
		ShowTxtInMiddle( "合并演示完成" );

	ImGui::End();
}
