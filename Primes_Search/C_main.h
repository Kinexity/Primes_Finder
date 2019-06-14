#pragma once
#include <iostream>
#include <cstdint>
#include <string>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include "PureCPPLib/io.h"
#include "PureCPPLib/C_Event_Log.h"
#include "PureCPPLib/C_Time_Counter.h"
#include "C_Search.h"
#include "C_Primes_Explorer.h"
using namespace std;

class C_main {
private:
	uint_fast32_t
		choice;
	experimental::filesystem::path
		main_path = wstring(L"C:/Users/") + getenv_wstr("USERNAME") + L"/Documents/Primes_Search",
		logs_path = main_path / L"logs";
	uint_fast32_t
		main_choice = 0;
	PCL::C_Event_Log
		event_log_obj{true};
	PCL::C_Time_Counter
		t_counter;
public:
	C_main();
	~C_main();
	void
		run(),
		menu_choice(),
		path_create(),
		convert();
};