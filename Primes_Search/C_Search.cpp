#include "C_Search.h"

constexpr uint_fast64_t
C_Search::numbers_per_block,
C_Search::bytes_per_block;

C_Search::C_Search(PCL::C_Event_Log& ev_log_obj) :
	event_log_obj(ev_log_obj) {
	bool
		error = false,
		file_remove;
	do {
		if (experimental::filesystem::exists(file_path)) { //file exists
			if (experimental::filesystem::file_size(file_path) >= bytes_per_block) { //file has at least the required size
				error = false;
				event_log_obj() << "File seems to be ok" << _endl_;
			}
			else { //file size is incorrect
				file_remove = false;
				error = true;
				event_log_obj() << "Error detected in primes file!" << _endl_;
				cout << line << endl << "Wykryto blad w pliku z liczbami pierwszymi!" << endl << "Usunac? [0/1]: ";
				cin >> file_remove;
				abort = !file_remove;
				if (file_remove) {
					experimental::filesystem::remove(file_path);
					event_log_obj() << "File deleted" << _endl_;
				}
				else {
					event_log_obj() << "File saved" << _endl_;
					error = false;
				}
			}
		}
		else { //file does not exist
			error = false;
			file.open(file_path, ios::binary | ios::out);
			event_log_obj() << "Creating file" << operation_evaluation<true>(file.is_open() && file) << _endl_;
			tc.start();
			primes_vector.resize(0);
			primes_vector.push_back(2);
			primes_vector.push_back(3);
			primes_vector.push_back(5);
			primes_bitset = false;
			primes_bitset[2] = true;
			primes_bitset[3] = true;
			primes_bitset[5] = true;
			uint_fast64_t
				rounded_sqrt,
				divider_index,
				reminder;
			for (uint_fast64_t index = 6; index < numbers_per_block; index++) {
				rounded_sqrt = static_cast<uint_fast64_t>(ceil(sqrt(index)));
				divider_index = 0;
				reminder = 1;
				while ((primes_vector[divider_index] <= rounded_sqrt) && (reminder = index % primes_vector[divider_index++]));
				if (reminder != 0) {
					~primes_bitset[index];
					primes_vector.push_back(index);
				}
			}
			tc.stop();
			event_log_obj() << "Generation time:" << tc.measured_timespan().count() << _endl_;
			cout << "Czas generowania pierwszego bloku: " << tc.measured_timespan().count() << endl;
			file.seekp(0);
			file.write(reinterpret_cast<char*>(&primes_bitset), bytes_per_block);
			event_log_obj() << "First block saving" << operation_evaluation<false>(experimental::filesystem::file_size(file_path) == bytes_per_block && file) << _endl_;
			file.close();
		}
	} while (error);
	if (!abort) {
		file.clear();
		file.open(file_path, ios::binary | ios::in | ios::out);
		file.seekg(0);
		file.read(reinterpret_cast<char*>(&primes_bitset), bytes_per_block);
		file.close();
		primes_vector.resize(0);
		for (uint_fast64_t index = 0; index < numbers_per_block; index++) {
			if (primes_bitset[index]) {
				primes_vector.push_back(index);
			}
		}
	}
}

C_Search::~C_Search() {}

void C_Search::run() {
	if (!abort) {
		file.open(file_path, ios::out | ios::binary | ios::app);
		if (file.is_open() && file) {
			file.close();
			file.open(file_path, ios::out | ios::in | ios::binary | ios::ate);
			event_log_obj() << "File size (B): " << experimental::filesystem::file_size(file_path) << _endl_;
			desired_block_index = block_error_check(experimental::filesystem::file_size(file_path) / bytes_per_block);
			file.seekp(desired_block_index * bytes_per_block);
			break_searching = false;
			scheduler = make_unique<PCL::C_Scheduler<uint_fast64_t, PCL::C_Indexer<uint_fast64_t>>>(PCL::C_Indexer<uint_fast64_t>(desired_block_index, numeric_limits<uint_fast64_t>::max()));
			set.start_async(thread::hardware_concurrency(), &C_Search::_thread_, this);
			cout << line << endl << "Wcisnij 's', aby zakonczyc..." << endl;
			do {} while (_getch() != 's');
			break_searching = true;
			cout << line << endl << "Zakonczanie wyszukiwania..." << endl;
			set.wait_for_async();
		}
		else {
			cout << line << endl << "Wystapil blad pliku!" << endl;
			event_log_obj() << "File error detected" << _endl_;
			if (!file.is_open()) {
				event_log_obj() << "Unable to open file" << _endl_;
			}
			if (!file) {
				event_log_obj() << "File error flag have been set" << _endl_;
			}
			if (!(experimental::filesystem::file_size("primes.bin") % 2500000 == 0)) {
				event_log_obj() << "Incorrect file size" << _endl_;
			}
		}
		if (file.is_open()) {
			file.close();
		}
	}
}

void C_Search::dump_bitset(pair<uint_fast64_t, unsigned char*> block) {
	unique_lock<mutex> guard(mut_ex);
	saving_queue.push(block);
	while (!saving_queue.empty() && get<uint_fast64_t>(saving_queue.top()) == desired_block_index) {
		file.write(reinterpret_cast<char*>(get<unsigned char*>(saving_queue.top())), bytes_per_block);
		desired_block_index++;
		delete[] get<unsigned char*>(saving_queue.top());
		saving_queue.pop();
	}
}

uint_fast64_t C_Search::block_error_check(uint_fast64_t block_index) {
	if (block_index == 1) {
		return block_index;
	}
	else {
		checkpoint(event_log_obj);
		PCL::bitset<numbers_per_block>
			temp_block_bitset;
		PCL::C_Indexer<>
			indexer{ thread::hardware_concurrency() };
		atomic_bool
			checking_state = true;
		checkpoint(event_log_obj);
		const uint_fast64_t
			block_in_checking_index = (block_index - 1),
			starting_number = block_in_checking_index * numbers_per_block,
			ending_number = block_index * numbers_per_block;
		checkpoint(event_log_obj);
		checkpoint(event_log_obj);
		event_log_obj() << "Checking block " << block_in_checking_index << _endl_;
		checkpoint(event_log_obj);
		file.seekg(bytes_per_block * block_in_checking_index);
		checkpoint(event_log_obj);
		for (uint_fast64_t index = 0; index < temp_block_bitset.size(); index++) {
			temp_block_bitset[index] = false;
		}
		checkpoint(event_log_obj);
		for (uint_fast64_t index = 0; index < temp_block_bitset.byte_size(); index++) {
			(&temp_block_bitset)[index] = 0x00;
		}
		checkpoint(event_log_obj);
		file.read((char*)(&temp_block_bitset), temp_block_bitset.byte_size());
		checkpoint(event_log_obj);
		auto checking_thread = [&]() {
			uint_fast64_t
				divider_index = 0,
				rounded_sqrt = 0,
				reminder = 0;
			PCL::bitset<numbers_per_block>
				local_temp_block_bitset;
			memcpy(&local_temp_block_bitset, &temp_block_bitset, temp_block_bitset.byte_size());
			const uint_fast64_t
				thread_index = indexer();
			uint_fast64_t
				local_starting_number = starting_number + static_cast<decltype(local_starting_number)>(floor(numbers_per_block * double(thread_index) / thread::hardware_concurrency())),
				local_ending_number = starting_number + static_cast<decltype(local_starting_number)>(floor(numbers_per_block * double(thread_index + 1) / thread::hardware_concurrency()));
			for (uint_fast64_t number = local_starting_number; number < local_ending_number; number++) {
				rounded_sqrt = static_cast<uint_fast64_t>(ceil(sqrt(number)));
				divider_index = 0;
				reminder = 1;
				while ((primes_vector[divider_index] <= rounded_sqrt) && (reminder = number % primes_vector[divider_index++]));
				[[unlikely]] if (local_temp_block_bitset[number % numbers_per_block] == (reminder == 0)) {
					event_log_obj() << "Detected error in block " << block_in_checking_index << "! Number: " << number << " Reminder:" << reminder << _endl_;
					checking_state = false;
					return;
				}
				[[unlikely]] if (number % 10000 == 0) {
					[[unlikely]] if (!((bool)checking_state)) {
						return;
					}
				}
			}
		};
		PCL::C_thread_set().start(thread::hardware_concurrency(), checking_thread);
		checkpoint(event_log_obj);
		if (checking_state) {
			return block_index;
		}
		else {
			return block_error_check(block_in_checking_index);
		}
	}
}

void C_Search::_thread_() {
	static mutex
		mutual_exec;
	PCL::C_Time_Counter
		time_counter;
	uint_fast64_t
		block_index = 0,
		starting_number = 0,
		ending_number = 0,
		divider_index = 0,
		rounded_sqrt = 0,
		reminder = 0;
	PCL::bitset<numbers_per_block>
		block_bitset;
	for (;;) {
		time_counter.start();
		block_index = (*scheduler)();
		block_bitset.reset();
		block_bitset = false;
		starting_number = block_index * numbers_per_block;
		ending_number = starting_number + numbers_per_block;
		for (uint_fast64_t number = starting_number; number < ending_number; number++) {
			rounded_sqrt = static_cast<uint_fast64_t>(ceil(sqrt(number)));
			divider_index = 0;
			reminder = 1;
			while ((primes_vector[divider_index] <= rounded_sqrt) && (reminder = number % primes_vector[divider_index++]));
			if (reminder != 0) {
				~block_bitset[number % numbers_per_block];
			}
		}
		dump_bitset({ block_index, reinterpret_cast<unsigned char*>(block_bitset.release()) });
		time_counter.stop();
		{
			lock_guard<mutex>
				locker(mutual_exec);
			event_log_obj() << "Block " << block_index << ": " << time_counter.measured_timespan().count() << " s" << _endl_;
			cout << "Blok " << block_index << ": " << time_counter.measured_timespan().count() << " s" << endl;
		}
		if ((bool)break_searching) {
			break;
		}
	}
}

void C_Search::avx_thread() {
	static mutex
		mutual_exec;
	PCL::C_Time_Counter
		time_counter;
	uint_fast64_t
		block_index = 0,
		starting_number = 0,
		ending_number = 0,
		divider_index = 0,
		rounded_sqrt = 0,
		reminder = 0;
	PCL::bitset<numbers_per_block>
		block_bitset;
	for (;;) {
		time_counter.start();
		block_index = (*scheduler)();
		block_bitset.reset();
		block_bitset = false;
		starting_number = block_index * numbers_per_block;
		ending_number = starting_number + numbers_per_block;
		for (uint_fast64_t number = starting_number + 1; number < ending_number;) {
			auto prime_test = [&]() {
				rounded_sqrt = static_cast<uint_fast64_t>(ceil(sqrt(number)));
				divider_index = 3;
				reminder = 1;
				while ((primes_vector[divider_index] <= rounded_sqrt) && (reminder = number % primes_vector[divider_index++]));
				if (reminder != 0) {
					~block_bitset[number % numbers_per_block];
				}
			};
			prime_test();
			number += 6;
			prime_test();
			number += 4;
			prime_test();
			number += 2;
			prime_test();
			number += 4;
			prime_test();
			number += 2;
			prime_test();
			number += 4;
			prime_test();
			number += 6;
			prime_test();
			number += 2;
		}
		dump_bitset({ block_index, reinterpret_cast<unsigned char*>(block_bitset.release()) });
		time_counter.stop();
		{
			lock_guard<mutex>
				locker(mutual_exec);
			event_log_obj() << "Block " << block_index << ": " << time_counter.measured_timespan().count() << " s" << _endl_;
			cout << "Blok " << block_index << ": " << time_counter.measured_timespan().count() << " s" << endl;
		}
		if ((bool)break_searching) {
			break;
		}
	}
}
