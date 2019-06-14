#include "C_main.h"


void C_main::convert() {
	static constexpr unsigned
		primes_in_file = 1000000,
		bitset_size = 200000000;
	PCL::bitset<bitset_size> 
		bs;
	fstream
		input, output(main_path / "primes.bin", ios::out | ios::binary);
	unique_ptr<uint_fast64_t[]>
		primes_array = make_unique<uint_fast64_t[]>(primes_in_file);
	uint_fast64_t
		bitset_index = 0;
	if (output.is_open() && output) {
		for (unsigned file_index = 1;; file_index++) {
			input.open(main_path / (string("pierwsze[") + to_string(file_index) + "].bin"), ios::in | ios::binary);
			if (!input.is_open() || !input) {
				break;
			}
			else {
				cout << "Otwarto plik numer " << file_index << endl;
				event_log_obj() << "File number " << file_index << " opened" << _endl_;
			}
			input.read(reinterpret_cast<char*>(primes_array.get()), primes_in_file * sizeof(uint_fast64_t));
			input.close();
			for (unsigned array_index = 0; array_index < primes_in_file; array_index++) {
				if (primes_array[array_index] / bitset_size == bitset_index) {
					bs[primes_array[array_index] % bitset_size] = true;
				}
				else {
					bitset_index++;
					output.write(reinterpret_cast<char*>(&bs), bs.byte_size());
					bs = false;
					continue;
				}
			}
		}
	}
	else {
		event_log_obj() << "Couldn't open output file" << _endl_;
	}
}

C_main::C_main() {
	t_counter.start();
	cout << line << endl;
	cout << "Primes Search - Alpha 0.1" << endl;
	event_log_obj() << "Program's initialization..." << _endl_;
	path_create();
	event_log_obj.create_log_file();
	event_log_obj() << "Initialization complete." << _endl_;
}


C_main::~C_main() {
	event_log_obj.finish_expected();
}

void C_main::run() {
	t_counter.stop();
	event_log_obj() << "Initialization time: " << t_counter.measured_timespan().count() << _endl_;
	do {
		cout << line << endl
			<< "Menu Glowne" << endl;
		menu_choice();
		switch (choice)
		{
		case 0: {
			break;
		}
		case 1: {
			C_Search(event_log_obj).run();
			break;
		}
		case 2: {
			C_Primes_Explorer().run();
			break;
		}
		}
	} while (choice != 0);
}

void C_main::path_create() {
	if (!experimental::filesystem::exists(main_path)) {
		experimental::filesystem::create_directory(main_path);
		event_log_obj() << "Creating program's main path" << operation_evaluation<true>(experimental::filesystem::exists(main_path)) << _endl_;
	}
	experimental::filesystem::current_path(main_path);
	bool b = (experimental::filesystem::current_path() == main_path);
	event_log_obj() << "Setting current directory" << operation_evaluation<true>(b) << _endl_;
	if (!experimental::filesystem::exists(logs_path)) {
		experimental::filesystem::create_directory(logs_path);
		event_log_obj() << "Creating program's event log path" << operation_evaluation<true>(experimental::filesystem::exists(logs_path)) << _endl_;
	}
}

void C_main::menu_choice() {
	do {
		cout << line << endl
			<< "0 - Zamknij program" << endl
			<< "1 - Szukaj liczb pierwszych" << endl
			<< "2 - Eksploruj zbior liczb pierwszych" << endl
			<< "3 - Ustawienia" << endl
			<< "Twoj wybor: ";
		cin >> choice;
	} while (incorrect_value(choice > 3) || input_error());
}