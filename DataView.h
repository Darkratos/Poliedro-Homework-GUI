#pragma once
using namespace std;

class DataView {
	int SelectedIndex = -1;
	int Width = 0, Height = 0;
	char** Headers;

	int ColumnsNeeded;
	int ItemsPerCol;

	int CurrentColumn = 1;

	void CustomColorText( std::string Text, std::vector<std::string> wordsToColor, ImVec4 TextCustomColor ) {
		ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 0, 0 ) );
		char* str = ( char* ) Text.c_str( );
		int len = Text.length( );
		std::string holder = "";
		bool foundonce = false;

		for ( int i = 0; i < len; i++ ) {
			std::string tmp = str + i;
			std::string sWord = "";
			bool found = false;

			for ( auto word : wordsToColor ) {
				if ( tmp.rfind( word, 0 ) == 0 ) {
					found = true;
					sWord = word;
					break;
				}
			}

			if ( found ) {
				if ( foundonce )
					ImGui::SameLine( );

				foundonce = true;

				ImGui::Text( "%s", holder.c_str( ) );
				ImGui::SameLine( );

				ImGui::TextColored( TextCustomColor, "%s", sWord.c_str( ) );
				i += sWord.length( ) - 1;

				holder.clear( );
			}
			else {
				holder.append( 1, tmp[ 0 ] );
			}
		}

		if ( holder.length( ) > 0 ) {
			if ( !foundonce )
				ImGui::PopStyleVar( );

			ImGui::SameLine( );
			ImGui::Text( "%s", holder.c_str( ) );

			if ( !foundonce )
				return;
		}

		ImGui::PopStyleVar( );
	}

public:

	DataView( int Width, int Height, char** Headers, int Columns, int ItemsPerCol ) {
		IM_ASSERT( Columns >= 1 && "Cannot draw a DataView with 0 Columns!" );
		this->Width = Width;
		this->Height = Height;
		this->Headers = Headers;

		this->ColumnsNeeded = Columns;
		this->ItemsPerCol = ItemsPerCol;
	}

	void Begin( char* DataViewID ) {
		ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0, 0 ) );
		ImGui::BeginChild( DataViewID, ImVec2( Width, Height ), true, ImGuiWindowFlags_NoMove );
		ImGui::PopStyleVar( );

		ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 0, 0 ) );
		ImGui::Separator( );
		ImGui::PopStyleVar( );

		vector<int> colSizes;
		colSizes.clear( );

		ImGui::Columns( ColumnsNeeded, NULL, true );

		for ( int i = 0; i < ColumnsNeeded; i++ ) {
			colSizes.push_back( ImGui::GetColumnWidth( ) );
			ImGui::SetCursorPosY( ImGui::GetCursorPosY( ) + 8 );
			ImGui::Text( "%s", Headers[ i ] ); ImGui::NextColumn( );
		}

		ImGui::Columns( 1 );

		ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 0, 0 ) );
		ImGui::Separator( );
		ImGui::PopStyleVar( );

		ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0, 8 ) );
		ImGui::BeginChild( "###inner_items", ImVec2( Width, Height - 30 ), true, ImGuiWindowFlags_NoMove );
		ImGui::PopStyleVar( );

		ImGui::Columns( ColumnsNeeded, NULL, false );
		for ( int i = 0; i < ColumnsNeeded; i++ ) {
			ImGui::SetColumnWidth( i, colSizes[ i ] );
		}
	}

	int AddItem( ImVec4 ItemColor, bool bSelected, bool isSelectable, ImGuiSelectableFlags flags, const char* fmt, ... ) {
		char buf[ 1024 ] = { 0 };
		va_list va;

		va_start( va, fmt );
		_vsnprintf_s( buf, 1024, fmt, va );
		va_end( va );

		ImGui::PushStyleColor( ImGuiCol_Text, ItemColor );

		if ( isSelectable )
			ImGui::Selectable( buf, bSelected, flags );
		else
			ImGui::TextColored( ItemColor, buf );

		ImGui::NextColumn( );

		ImGui::PopStyleColor( );

		if ( CurrentColumn >= ColumnsNeeded )
			CurrentColumn = 1;
		else
			CurrentColumn++;

		if ( ImGui::IsItemHovered( ) ) {
			if ( ImGui::IsMouseDoubleClicked( 0 ) )
				return 2;
			else if ( ImGui::IsMouseClicked( 0 ) )
				return 1;
			else
				return 0;
		}
		else {
			return 0;
		}

		return 0;
	}

	int AddCustomColorText( ImVec4 CustomColor, std::vector<std::string> wordsToColor, const char* fmt, ... ) {
		char buf[ 1024 ] = { 0 };
		va_list va;

		va_start( va, fmt );
		_vsnprintf_s( buf, 1024, fmt, va );
		va_end( va );

		if ( CurrentColumn == 1 )
			ImGui::SetCursorPosX( ImGui::GetCursorPosX( ) + 6 );

		CustomColorText( buf, wordsToColor, CustomColor );

		ImGui::NextColumn( );

		if ( CurrentColumn >= ColumnsNeeded )
			CurrentColumn = 1;
		else
			CurrentColumn++;

		if ( ImGui::IsItemHovered( ) ) {
			if ( ImGui::IsMouseDoubleClicked( 0 ) )
				return 2;
			else if ( ImGui::IsMouseClicked( 0 ) )
				return 1;
			else
				return 0;
		}
		else {
			return 0;
		}

		return 0;
	}

	void AddCheckBox( bool* var, const char* fmt, ... ) {
		char buf[ 1024 ] = { 0 };
		va_list va;

		va_start( va, fmt );
		_vsnprintf_s( buf, 1024, fmt, va );
		va_end( va );

		if ( CurrentColumn == 1 )
			ImGui::SetCursorPosX( ImGui::GetCursorPosX( ) + 6 );

		ImGui::Checkbox( buf, var );

		ImGui::NextColumn( );

		if ( CurrentColumn >= ColumnsNeeded )
			CurrentColumn = 1;
		else
			CurrentColumn++;
	}

	void End( ) {
		ImGui::EndChild( );
		ImGui::EndChild( );
	}
};