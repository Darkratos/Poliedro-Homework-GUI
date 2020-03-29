#pragma once
#include "json.hpp"

class gui_renderer
{
public:
	void draw( );

	bool b_draw = true;

	gui_renderer( LPDIRECT3DDEVICE9 pdev )
	{
		this->device = pdev;
	}

private:
	void draw_login_form( );
	void draw_homework_tab( );

	bool load_image( int id );

	bool login( );

	bool is_logged = false;
	bool set_wnd_focus = true;
	bool remember = false;
	bool failed_login = false;

	char username[ 128 ] = "";
	char password[ 128 ] = "";

	int selected_image_id = 0;
	bool show_image_wnd = false;

	int selected_hw_index = 0;
	bool show_hw_wnd = false;

	std::map<int, IDirect3DTexture9*> imgs;
	std::map<int, int> imgs_w;
	std::map<int, int> imgs_h;

	LPDIRECT3DDEVICE9 device;

	nlohmann::json hw_data;
};