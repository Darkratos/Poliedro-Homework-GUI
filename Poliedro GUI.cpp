#include "Includes.h"
#include "GuiRenderer.h"

LPDIRECT3D9 d3d;
LPDIRECT3DDEVICE9 d3ddev;
HWND hWnd;
ImGuiContext* main_context;

D3DPRESENT_PARAMETERS d3dpp;

void create_d3d_devices( HWND hWnd, LPDIRECT3DDEVICE9& pDevice );
void render_frame( void );
void clean_d3d_devices( void );
void init_imgui( LPDIRECT3DDEVICE9 pDevice, HWND window, ImGuiContext*& Context );
void reset_devices( );
extern LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

LRESULT CALLBACK WindowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow ) {

	WNDCLASSEX wc;

	ZeroMemory( &wc, sizeof( WNDCLASSEX ) );

	wc.cbSize = sizeof( WNDCLASSEX );
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor( NULL, IDC_ARROW );
	wc.lpszClassName = "poliedro_gui";

	if ( !RegisterClassExA( &wc ) )
		MessageBox( 0, "Failed to register class", "", 0 );

	hWnd = CreateWindowExA( NULL,
		"poliedro_gui",
		"gui",
		WS_POPUP,
		100, 100,
		620, 500,
		NULL,
		NULL,
		hInstance,
		NULL );

	if ( !hWnd ) {
		MessageBox( 0, "Failed to create window", "", 0 );
		return 0;
	}

	ShowWindow( hWnd, nCmdShow );

	create_d3d_devices( hWnd, d3ddev );

	init_imgui( d3ddev, hWnd, main_context );

	MSG msg;

	while ( TRUE )
	{
		while ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}

		if ( msg.message == WM_QUIT )
			break;

		render_frame( );
		static bool bOnce = false;

		if ( !bOnce ) {
			ShowWindow( hWnd, false );
			bOnce = true;
		}
	}

	clean_d3d_devices( );

	return msg.wParam;
}

LRESULT CALLBACK WindowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam ) {
	if ( ImGui_ImplWin32_WndProcHandler( hWnd, message, wParam, lParam ) )
		return true;

	POINT p;
	int pos;

	switch ( message )
	{
	case WM_CLOSE:
	case WM_DESTROY:
		PostQuitMessage( 0 );
		return 0;
		break;
	case WM_LBUTTONDOWN:
		pos = GetMessagePos( );
		POINTS ps = MAKEPOINTS( pos );
		p = { ps.x,ps.y };

		ScreenToClient( hWnd, &p );

		if ( p.y < 25 && p.x < 460 ) {
			ReleaseCapture( );
			SendMessage( hWnd, WM_SYSCOMMAND, 0xf012, 0 );
		}
		break;
	case WM_SIZE:
		if ( d3ddev != NULL && wParam != SIZE_MINIMIZED )
		{
			d3dpp.BackBufferWidth = LOWORD( lParam );
			d3dpp.BackBufferHeight = HIWORD( lParam );
			reset_devices( );
		}
		return 0;
	case WM_SYSCOMMAND:
		if ( ( wParam & 0xfff0 ) == SC_KEYMENU )
			return 0;
		break;
	case WM_DPICHANGED:
		if ( ImGui::GetIO( ).ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports )
		{
			const RECT* suggested_rect = ( RECT* ) lParam;
			::SetWindowPos( hWnd, NULL, suggested_rect->left, suggested_rect->top, suggested_rect->right - suggested_rect->left, suggested_rect->bottom - suggested_rect->top, SWP_NOZORDER | SWP_NOACTIVATE );
		}
		break;
	}

	return DefWindowProc( hWnd, message, wParam, lParam );
}

void create_d3d_devices( HWND hWnd, LPDIRECT3DDEVICE9& pDevice )
{
	d3d = Direct3DCreate9( D3D_SDK_VERSION );    // create the Direct3D interface

	ZeroMemory( &d3dpp, sizeof( d3dpp ) );    // clear out the struct for use
	d3dpp.Windowed = TRUE;    // program windowed, not fullscreen
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;    // discard old frames
	d3dpp.hDeviceWindow = hWnd;    // set the window to be used by Direct3D
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;


	// create a device class using this information and the info from the d3dpp stuct
	d3d->CreateDevice( D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp,
		&pDevice );

	if ( !pDevice )
		MessageBox( 0, "Failed to create device", "", 0 );

}

void init_imgui( LPDIRECT3DDEVICE9 pDevice, HWND window, ImGuiContext*& Context ) {
	IMGUI_CHECKVERSION( );
	ImGui_ImplWin32_EnableDpiAwareness( );

	Context = ImGui::CreateContext( );

	ImGuiIO* io = &ImGui::GetIO( );
	io->IniFilename = NULL;
	io->ConfigWindowsMoveFromTitleBarOnly = true;
	io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	ImGui_ImplDX9_Init( pDevice );
	ImGui_ImplWin32_Init( window );

	ImGui::StyleColorsDark( );

	ImGuiStyle* style = &ImGui::GetStyle( );

	style->WindowTitleAlign = ImVec2( 0.5f, 0.5f );
	style->WindowPadding = ImVec2( 15, 8 );
	style->WindowRounding = 5.0f;
	style->FramePadding = ImVec2( 5, 5 );
	style->FrameRounding = 4.0f;
	style->ItemSpacing = ImVec2( 12, 8 );
	style->ItemInnerSpacing = ImVec2( 8, 6 );
	style->IndentSpacing = 25.0f;
	style->ScrollbarSize = 15.0f;
	style->ScrollbarRounding = 9.0f;
	style->GrabMinSize = 5.0f;
	style->GrabRounding = 3.0f;
}

void render_frame( void )
{
	d3ddev->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB( 255, 255, 255 ), 1.0f, 0 );

	ImGui_ImplDX9_NewFrame( );
	ImGui_ImplWin32_NewFrame( );
	ImGui::NewFrame( );

	static gui_renderer* g_render = new gui_renderer( d3ddev );

	g_render->draw( );

	if ( !g_render->b_draw )
		SendMessage( hWnd, WM_DESTROY, 0, 0 );

	ImGui::EndFrame( );

	d3ddev->SetRenderState( D3DRS_ZENABLE, false );
	d3ddev->SetRenderState( D3DRS_ALPHABLENDENABLE, false );
	d3ddev->SetRenderState( D3DRS_SCISSORTESTENABLE, false );
	ImVec4 clear_color = ImVec4( 0.45f, 0.55f, 0.60f, 1.00f );
	D3DCOLOR clear_col_dx = D3DCOLOR_RGBA( ( int ) ( clear_color.x * 255.0f ), ( int ) ( clear_color.y * 255.0f ), ( int ) ( clear_color.z * 255.0f ), ( int ) ( clear_color.w * 255.0f ) );
	d3ddev->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0 );

	if ( d3ddev->BeginScene( ) >= 0 )
	{
		ImGui::Render( );
		ImGui_ImplDX9_RenderDrawData( ImGui::GetDrawData( ) );
		d3ddev->EndScene( );
	}

	ImGui::UpdatePlatformWindows( );
	ImGui::RenderPlatformWindowsDefault( );

	HRESULT result = d3ddev->Present( NULL, NULL, NULL, NULL );

	if ( result == D3DERR_DEVICELOST && d3ddev->TestCooperativeLevel( ) == D3DERR_DEVICENOTRESET )
		reset_devices( );
}

void clean_d3d_devices( void )
{
	d3ddev->Release( );
	d3d->Release( );
}

void reset_devices( )
{
	ImGui_ImplDX9_InvalidateDeviceObjects( );
	HRESULT hr = d3ddev->Reset( &d3dpp );
	if ( hr == D3DERR_INVALIDCALL )
		IM_ASSERT( 0 );
	ImGui_ImplDX9_CreateDeviceObjects( );
}