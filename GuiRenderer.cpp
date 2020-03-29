#include "Includes.h"
#include "GuiRenderer.h"
#include "DataView.h"

char* json_template = "{ \n\t\"Usuario\": \"USERNAME\", \n\t\"Password\": \"SENHA\"\n}";
char* json_template2 = "{ \n\t\"concluidas\": 0\n}";

bool gui_renderer::load_image( int id )
{
	if ( this->imgs.find( id ) != this->imgs.end( ) )
		return true;

	std::stringstream filename;
	filename << "images\\" << id << ".png";

	PDIRECT3DTEXTURE9 texture;
	HRESULT hr = D3DXCreateTextureFromFileA( this->device, filename.str( ).c_str( ), &texture );
	if ( hr != S_OK )
		return false;

	D3DSURFACE_DESC my_image_desc;
	texture->GetLevelDesc( 0, &my_image_desc );
	this->imgs[ id ] = texture;
	this->imgs_w[ id ] = ( int ) my_image_desc.Width;
	this->imgs_h[ id ] = ( int ) my_image_desc.Height;

	return true;
}

bool CenterInputTextWithHint( const char* label, const char* hint, char* buf, size_t buf_size, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL )
{
	const char* base_str = strlen( buf ) > 0 ? buf : hint;

	int text_size = ImGui::CalcTextSize( base_str ).x;

	float prev_y = ImGui::GetStyle( ).FramePadding.y;

	float next_w = ImGui::CalcItemWidth( );

	ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2( next_w / 2 - text_size / 2, prev_y ) );

	bool result = ImGui::InputTextWithHint( label, hint, buf, buf_size, flags, callback, user_data );

	ImGui::PopStyleVar( );

	return result;
}

void gui_renderer::draw( )
{
	ImGui::Begin( "dummy" );
	ImGui::Button( "hey" );
	ImGui::End( );

	static bool checked_file = false;

	if ( !checked_file )
	{
		std::ifstream result( "result_data.json" );

		if ( result.good( ) )
		{
			result >> this->hw_data;

			if ( !this->hw_data[ "Sucesso" ].is_null( ) && this->hw_data[ "Sucesso" ] )
			{
				this->is_logged = true;
			}
			else
			{
				this->hw_data = 0;
			}
		}

		result.close( );

		result.open( "login_data.json" );

		if ( result.good( ) )
		{
			nlohmann::json login;
			result >> login;

			std::string username = login[ "Usuario" ];
			strcpy( this->username, username.c_str( ) );

			std::string password = login[ "Password" ];
			strcpy( this->password, password.c_str( ) );
		}

		result.close( );

		checked_file = true;
	}

	if ( !this->is_logged )
	{
		this->draw_login_form( );
	}
	else
	{
		this->draw_homework_tab( );
	}
}

void gui_renderer::draw_login_form( )
{
	ImGui::SetNextWindowSize( ImVec2( 620, 500 ) );
	if ( ImGui::Begin( "Poliedro Homework GUI", &this->b_draw, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings ) ) {
		ImGuiViewport* vp = ImGui::GetWindowViewport( );

		if ( this->set_wnd_focus ) {
			SetActiveWindow( ( HWND ) vp->PlatformHandle );
			this->set_wnd_focus = false;
		}

		ImGui::SetCursorPosX( ImGui::GetWindowWidth( ) / 2 - 100 );
		ImGui::SetCursorPosY( 150 );

		ImGui::SetNextItemWidth( 200 );
		CenterInputTextWithHint( "###login", "Login", this->username, 128 );

		ImGui::SetCursorPosY( 195 );
		ImGui::SetCursorPosX( ImGui::GetWindowWidth( ) / 2 - 100 );
		ImGui::SetNextItemWidth( 200 );
		CenterInputTextWithHint( "###password", "Senha", this->password, 128 );

		ImGui::SetCursorPosY( 285 );
		ImGui::SetCursorPosX( ImGui::GetWindowWidth( ) / 2 - 100 );
		
		if ( ImGui::Button( "Login", ImVec2( 200, 0 ) ) )
		{
			this->failed_login = false;

			if ( this->login( ) )
			{
				this->is_logged = true;
			}
			else
			{
				this->failed_login = true;
			}
		}

		if ( this->failed_login )
		{
			ImGui::SetCursorPosY( 325 );
			ImGui::SetCursorPosX( ImGui::GetWindowWidth( ) / 2 - ImGui::CalcTextSize( "Login ou senha incorretos!" ).x / 2 );
			ImGui::TextColored( ImVec4( 1, 0, 0, 1 ), "Login ou senha incorretos!" );
		}

		ImGui::End( );
	}
}

void gui_renderer::draw_homework_tab( )
{
	ImGui::SetNextWindowSize( ImVec2( 620, 500 ) );
	if ( ImGui::Begin( "Poliedro Homework GUI", &this->b_draw, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings ) ) {
		ImGuiViewport* vp = ImGui::GetWindowViewport( );

		if ( this->set_wnd_focus ) {
			SetActiveWindow( ( HWND ) vp->PlatformHandle );
			this->set_wnd_focus = false;
		}

		std::string nome			= this->hw_data[ "Dados" ][ "Pessoa" ][ "Nome" ];
		int hw_count						= this->hw_data[ "Dados" ][ "DadosPagina" ][ "Tarefas" ].size( );
		static int selected_hw		= 0;
		static bool hide_completed	= false;
		static bool hide_no_date	= false;

		static char subject_filter			[ 1024 ] = "";
		static char teacher_filter			[ 1024 ] = "";
		static char date_published_filter	[ 1024 ] = "";
		static char due_date_filter			[ 1024 ] = "";

		ImGui::Text( "Logado como %s", nome.c_str( ) );

		ImGui::SameLine( 425 );

		if ( ImGui::Button( "Atualizar" ) )
		{
			this->is_logged = this->login( );
		}

		ImGui::SameLine( );

		this->is_logged = !ImGui::Button( "Trocar Conta" );
		
		char* cols[ ] = { "Materia", "Professor", "Data da Postagem", "Data de Entrega", "Status" };
		DataView* data_view = new DataView( 0, ImGui::GetWindowHeight( ) - 140, cols, IM_ARRAYSIZE( cols ), hw_count );

		data_view->Begin( "###hw_cont" );
		ImVec4 textcolor = ImGui::GetStyle( ).Colors[ ImGuiCol_Text ];

		for ( int i = 0; i < hw_count; i++ )
		{
			auto hw_obj = this->hw_data[ "Dados" ][ "DadosPagina" ][ "Tarefas" ][ i ];

			std::string materia = hw_obj[ "Materia" ];
			std::string professor = hw_obj[ "Apelido" ];

			bool tem_entrega = !hw_obj[ "DataEntregaUniversal" ].is_null( );
			std::string entrega = tem_entrega ? hw_obj[ "DataEntregaUniversal" ] : "Sem Data de Entrega";

			if ( tem_entrega )
			{
				entrega = entrega.substr( 0, entrega.find( "T" ) );
				std::stringstream ss;

				ss << entrega.substr( 8, 2 ) << "/" << entrega.substr( 5, 2 ) << "/" << entrega.substr( 0, 4 );
				entrega = ss.str( );
			}

			bool sts = hw_obj[ "DataRealizacaoUniversal" ].is_null( );
			std::string status = sts ? "Pendente" : "Concluida";

			std::string postagem = hw_obj[ "DataPublicacaoUniversal" ];
			postagem = postagem.substr( 0, postagem.find( "T" ) );
			std::stringstream ss;

			ss << postagem.substr( 8, 2 ) << "/" << postagem.substr( 5, 2 ) << "/" << postagem.substr( 0, 4 );
			postagem = ss.str( );
			
			if ( strlen( subject_filter ) > 0 )
				if ( materia.find( subject_filter ) == std::string::npos )
					continue;

			if ( strlen( teacher_filter ) > 0 )
				if ( professor.find( teacher_filter ) == std::string::npos )
					continue;

			if ( strlen( date_published_filter ) > 0 )
				if ( postagem.find( date_published_filter ) == std::string::npos )
					continue;

			if ( strlen( due_date_filter ) > 0 )
				if ( entrega.find( due_date_filter ) == std::string::npos )
					continue;

			if ( !sts && hide_completed )
				continue;

			if ( !tem_entrega && hide_no_date )
				continue;

			int res = data_view->AddItem( textcolor, selected_hw == i, true, ImGuiSelectableFlags_AllowDoubleClick | ImGuiSelectableFlags_SpanAllColumns, "%s", materia.c_str( ) );

			switch ( res )
			{
			case 1:
				selected_hw = i;
				break;
			case 2:
				selected_hw = i;
				this->show_hw_wnd = true;
				this->selected_hw_index = i;
				break;
			}

			data_view->AddItem( textcolor, false, false, 0, "%s", professor.c_str( ) );
			data_view->AddItem( textcolor, false, false, 0, "%s", postagem.c_str( ) );
			data_view->AddItem( textcolor, false, false, 0, "%s", entrega.c_str( ) );
			data_view->AddItem( textcolor, false, false, 0, "%s", status.c_str( ) );
		}

		data_view->End( );

		ImGui::SetNextItemWidth( 198 );

		CenterInputTextWithHint( "###subject_filter", "Filtrar materia", subject_filter, 1024 );
		ImGui::SameLine( );

		ImGui::SetNextItemWidth( 198 );

		CenterInputTextWithHint( "###teacher_filter", "Filtrar professor", teacher_filter, 1024 );
		ImGui::SameLine( );

		ImGui::Checkbox( "Esconder Finalizadas", &hide_completed );
		
		ImGui::SetNextItemWidth( 198 );

		CenterInputTextWithHint( "###date_pub_filter", "Filtrar Data Publicacao", date_published_filter, 1024 );
		ImGui::SameLine( );

		ImGui::SetNextItemWidth( 198 );

		CenterInputTextWithHint( "###due_date_filter", "Filtrar Data Entrega", due_date_filter, 1024 );
		ImGui::SameLine( );

		ImGui::Checkbox( "Esconder Sem Entrega", &hide_no_date );
	}

	ImGui::End( );

	if ( !this->show_hw_wnd )
		return;

	auto hw_obj = this->hw_data[ "Dados" ][ "DadosPagina" ][ "Tarefas" ][ this->selected_hw_index ];
	std::string tarefa = hw_obj[ "Descricao" ];

	int height = ImGui::CalcTextSize( tarefa.c_str( ), 0, false, 500 - ImGui::GetStyle( ).WindowPadding.x * 2 ).y + 160;

	if ( hw_obj[ "Imagens" ].size( ) )
		height += ImGui::GetTextLineHeight( ) * hw_obj[ "Imagens" ].size( ) + 40;

	height = min( height, 500 );

	ImGui::SetNextWindowSize( ImVec2( 500, height ) );
	ImGui::SetNextWindowPos( ImVec2( 15, 15 ), ImGuiCond_Once );

	if ( ImGui::Begin( "Conteudo da Tarefa", &this->show_hw_wnd, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings ) )
	{
		std::string materia = hw_obj[ "Materia" ];
		std::string professor = hw_obj[ "Apelido" ];
		int ID = hw_obj[ "Id" ];

		bool tem_entrega = !hw_obj[ "DataEntregaUniversal" ].is_null( );
		std::string entrega = tem_entrega ? hw_obj[ "DataEntregaUniversal" ] : "Sem Data de Entrega";

		if ( tem_entrega )
		{
			entrega = entrega.substr( 0, entrega.find( "T" ) );
			std::stringstream ss;

			ss << entrega.substr( 8, 2 ) << "/" << entrega.substr( 5, 2 ) << "/" << entrega.substr( 0, 4 );
			entrega = ss.str( );
		}

		bool sts = hw_obj[ "DataRealizacaoUniversal" ].is_null( );
		std::string status = sts ? "Pendente" : "Concluida";

		std::string postagem = hw_obj[ "DataPublicacaoUniversal" ];
		postagem = postagem.substr( 0, postagem.find( "T" ) );
		std::stringstream ss;

		ss << postagem.substr( 8, 2 ) << "/" << postagem.substr( 5, 2 ) << "/" << postagem.substr( 0, 4 );
		postagem = ss.str( );

		ImGui::Text( "Tarefa de %s - Prof. %s", materia.c_str( ), professor.c_str( ) );
		ImGui::Separator( );

		ImGui::Text( "Data da Postagem: %s", postagem.c_str( ) );
		ImGui::Text( "Data de Entrega: %s", entrega.c_str( ) );

		ImGui::Separator( );

		ImGui::TextWrapped( "%s", tarefa.c_str( ) );

		if ( ImGui::BeginPopupContextItem( "copy menu" ) )
		{
			std::regex url( R"((https?:\/\/)?(www\.)?[-a-zA-Z0-9@:%._\+~#=]{1,256}\.[a-zA-Z0-9()]{1,6}\b([-a-zA-Z0-9()@:%_\+.~#?&//=]*))", std::regex::ECMAScript | std::regex::nosubs );

			bool has_link = false;

			std::sregex_iterator iter( tarefa.begin( ), tarefa.end( ), url );
			std::sregex_iterator end;

			while ( iter != end )
			{
				for ( unsigned i = 0; i < iter->size( ); ++i )
				{
					if ( !has_link ){
						ImGui::Text( "Selecione o link para copiar:" );
						has_link = true;
					}

					if ( ImGui::Button( ( *iter )[ i ].str( ).c_str( ) ) )
					{
						ImGui::SetClipboardText( ( *iter )[ i ].str( ).c_str( ) );
						ImGui::CloseCurrentPopup( );
					}
				}
				++iter;
			}

			if ( has_link )
				ImGui::Separator( );

			if ( ImGui::Button( "Copiar Tudo" ) )
			{
				ImGui::SetClipboardText( tarefa.c_str( ) );
				ImGui::CloseCurrentPopup( );
			}
			
			ImGui::EndPopup( );
		}

		ImGui::Separator( );

		if ( hw_obj[ "Imagens" ].size( ) )
		{
			ImGui::Text( "Imagens:" );

			for ( int j = 0; j < hw_obj[ "Imagens" ].size( ); j++ )
			{
				std::string legenda = hw_obj[ "Imagens" ][ j ][ "Legenda" ];
				int id = hw_obj[ "Imagens" ][ j ][ "Id" ];

				ImGui::Text( "\tLegenda: %s", legenda.c_str( ) );
				ImGui::SameLine( );

				std::stringstream button_name;
				button_name << ">>###" << id;

				if ( ImGui::Button( button_name.str( ).c_str( ) ) )
				{
					if ( this->load_image( id ) )
					{
						this->show_image_wnd = true;
						this->selected_image_id = id;
					}
				}
			}

			ImGui::Separator( );
		}

		if ( tem_entrega )
		{
			entrega = entrega.substr( 0, entrega.find( "T" ) );
			std::stringstream ss;

			ss << entrega.substr( 8, 2 ) << "/" << entrega.substr( 5, 2 ) << "/" << entrega.substr( 0, 4 );
			entrega = ss.str( );

			bool sts = !hw_obj[ "DataRealizacaoUniversal" ].is_null( );

			if ( sts )
			{
				ImGui::PushStyleVar( ImGuiStyleVar_Alpha, ImGui::GetStyle( ).Alpha * 0.5f );
				ImGui::PushStyleColor( ImGuiCol_ButtonHovered, ImVec4( 0.10f, 0.09f, 0.12f, 1.00f ) );
				ImGui::PushStyleColor( ImGuiCol_ButtonActive, ImVec4( 0.10f, 0.09f, 0.12f, 1.00f ) );

				ImGui::SetCursorPosX( ImGui::GetWindowSize( ).x / 2 - 100 );

				ImGui::Button( "Concluir Tarefa", ImVec2( 200, 0 ) );

				ImGui::PopStyleColor( 2 );
				ImGui::PopStyleVar( );
			}
			else
			{
				ImGui::SetCursorPosX( ImGui::GetWindowSize( ).x / 2 - 100 );
				if ( ImGui::Button( "Concluir Tarefa", ImVec2( 200, 0 ) ) )
				{
					std::string json_final = json_template2;

					int index = json_final.find( "0" );
					std::stringstream id_str;
					id_str << ID;
					json_final = json_final.replace( index, 1, id_str.str( ) );

					std::ofstream file;
					file.open( "homework.json", std::ios::trunc );
					file << json_final.c_str( );
					file.close( );

					system( "python post_completed.py" );
				}
			}
		}
		else
		{
			ImGui::SetCursorPosX( ImGui::GetWindowSize( ).x / 2 - 100 );
			if ( ImGui::Button( "Concluir Tarefa", ImVec2( 200, 0 ) ) )
			{
				std::string json_final = json_template2;

				int index = json_final.find( "0" );
				std::stringstream id_str;
				id_str << ID;
				json_final = json_final.replace( index, 1, id_str.str( ) );

				std::ofstream file;
				file.open( "homework.json", std::ios::trunc );
				file << json_final.c_str( );
				file.close( );

				system( "python post_completed.py" );
			}
		}
	}

	ImGui::End( );

	if ( !this->show_image_wnd )
		return;

	RECT desktop;
	HWND desktop_hwnd = GetDesktopWindow( );
	GetWindowRect( desktop_hwnd, &desktop );

	if ( this->imgs_w[ this->selected_image_id ] >= desktop.right - 200 )
	{
		float factor = ( float ) ( desktop.right - 25 ) / ( float ) this->imgs_w[ this->selected_image_id ];
		this->imgs_w[ this->selected_image_id ] = floor( this->imgs_w[ this->selected_image_id ] * factor );
		this->imgs_h[ this->selected_image_id ] = floor( this->imgs_h[ this->selected_image_id ] * factor );
	}
	else if ( this->imgs_h[ this->selected_image_id ] >= desktop.bottom - 200 )
	{
		float factor = ( float ) ( desktop.bottom - 25 ) / ( float ) this->imgs_h[ this->selected_image_id ];
		this->imgs_w[ this->selected_image_id ] = floor( this->imgs_w[ this->selected_image_id ] * factor );
		this->imgs_h[ this->selected_image_id ] = floor( this->imgs_h[ this->selected_image_id ] * factor );
	}

	ImGui::SetNextWindowSize( ImVec2( this->imgs_w[ this->selected_image_id ] + ImGui::GetStyle( ).WindowPadding.x * 2, this->imgs_h[ this->selected_image_id ] + ImGui::GetStyle( ).WindowPadding.y * 2 + 30 ) );

	if ( ImGui::Begin( "Image", &this->show_image_wnd, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings ) )
	{
		ImGuiIO& io = ImGui::GetIO( );
		ImVec2 pos = ImGui::GetCursorScreenPos( );
		ImGui::Image( ( void* ) this->imgs[ this->selected_image_id ], ImVec2( this->imgs_w[ this->selected_image_id ], this->imgs_h[ this->selected_image_id ] ) );

		if ( ImGui::IsItemHovered( ) )
		{
			ImGui::BeginTooltip( );
			float region_sz = 64.0f;
			float region_x = io.MousePos.x - pos.x - region_sz * 0.5f; if ( region_x < 0.0f ) region_x = 0.0f; else if ( region_x > this->imgs_w[ this->selected_image_id ] - region_sz ) region_x = this->imgs_w[ this->selected_image_id ] - region_sz;
			float region_y = io.MousePos.y - pos.y - region_sz * 0.5f; if ( region_y < 0.0f ) region_y = 0.0f; else if ( region_y > this->imgs_h[ this->selected_image_id ] - region_sz ) region_y = this->imgs_h[ this->selected_image_id ] - region_sz;
			float zoom = 2.0f;
			ImVec2 uv0 = ImVec2( ( region_x ) / this->imgs_w[ this->selected_image_id ], ( region_y ) / this->imgs_h[ this->selected_image_id ] );
			ImVec2 uv1 = ImVec2( ( region_x + region_sz ) / this->imgs_w[ this->selected_image_id ], ( region_y + region_sz ) / this->imgs_h[ this->selected_image_id ] );
			ImGui::Image( ( void* ) this->imgs[ this->selected_image_id ], ImVec2( region_sz * zoom, region_sz * zoom ), uv0, uv1, ImVec4( 1.0f, 1.0f, 1.0f, 1.0f ), ImVec4( 1.0f, 1.0f, 1.0f, 0.5f ) );
			ImGui::EndTooltip( );
		}
	}

	ImGui::End( );
}

bool gui_renderer::login( )
{
	if ( !strlen( this->username ) || !strlen( this->password ) )
		return false;

	remove( "result_data.json" );
	std::string json_final = json_template;

	int index = json_final.find( "USERNAME" );
	json_final = json_final.replace( index, 8, this->username );

	index = json_final.find( "SENHA" );
	json_final = json_final.replace( index, 5, this->password );

	std::ofstream file;
	file.open( "login_data.json", std::ios::trunc );
	file << json_final.c_str( );
	file.close( );

	system( "python get_hw_info.py" );

	std::ifstream result( "result_data.json" );

	result >> this->hw_data;

	result.close( );

	return this->hw_data[ "Sucesso" ];
}