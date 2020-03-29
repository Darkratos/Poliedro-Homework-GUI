#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <iostream>
#include <d3d9.h>
#include <d3dx9.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <iterator>
#include <regex>

#include "ImGui\imgui.h"
#include "ImGui\imgui_impl_dx9.h"
#include "ImGui\imgui_impl_win32.h"

#pragma comment( lib, "d3d9.lib" )
#pragma comment( lib, "d3dx9.lib")
#pragma warning( disable: 4244 4482 )
#pragma warning( disable : 4996 )