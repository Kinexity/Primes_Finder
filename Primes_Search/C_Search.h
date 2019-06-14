#pragma once
#include <memory>
#include <filesystem>
#include <vector>
#include <bitset>
#include <fstream>
#include <tuple>
#include <mutex>
#include <experimental/filesystem>
#include <conio.h>
#include <atomic>
#include <queue>
#include <limits>
#include <immintrin.h>
#include "PureCPPLib/C_Scheduler.h"
#include "PureCPPLib/C_Indexer.h"
#include "PureCPPLib/C_thread_set.h"
#include "PureCPPLib/C_Time_Counter.h"
#include "PureCPPLib/C_Event_Log.h"
#include "PureCPPLib/bitset_.h"
#include "PureCPPLib/io.h"
#include "PureCPPLib/C_thread_set.h"
#include "PureCPPLib/C_Random.h"
using namespace std;

namespace std {
	template <>
	struct greater<tuple<uint_fast64_t, unsigned char*>> {	// functor for operator<
		constexpr bool operator()(const tuple<uint_fast64_t, unsigned char*>& _Left, const tuple<uint_fast64_t, unsigned char*>& _Right) const
		{	// apply operator< to operands
			return (get<1>(_Left) > get<1>(_Right));
		}
	};
};

class C_Search {
private:
	bool
		abort = false;
	static constexpr uint_fast64_t
		numbers_per_block = 30000000,
		bytes_per_block = numbers_per_block / 8;
	uint_fast64_t
		desired_block_index;
	PCL::C_Event_Log&
		event_log_obj;
	unique_ptr<PCL::C_Scheduler<uint_fast64_t, PCL::C_Indexer<uint_fast64_t>>>
		scheduler;
	vector<uint_fast64_t>
		primes_vector;
	PCL::bitset<numbers_per_block>
		primes_bitset;
	PCL::C_Time_Counter
		tc;
	fstream 
		file;
	experimental::filesystem::path
		file_path = experimental::filesystem::current_path() / L"primes.bin";
	mutex
		mut_ex;
	atomic<bool>
		break_searching = true;
	priority_queue<pair<uint_fast64_t, unsigned char*>, vector< pair<uint_fast64_t, unsigned char*>>,greater<pair<uint_fast64_t, unsigned char*>>>
		saving_queue;
	PCL::C_thread_set
		set;
	uint_fast64_t
		block_error_check(uint_fast64_t correct_block_index);
	void
		_thread_(),
		avx_thread(),
		dump_bitset(pair<uint_fast64_t, unsigned char*> block);
public:
	C_Search(PCL::C_Event_Log& ev_log_obj);
	~C_Search();
	void
		run();
};