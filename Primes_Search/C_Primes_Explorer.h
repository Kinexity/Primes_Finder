#pragma once
#include <fstream>
#include <iostream>
#include <conio.h>
#include <memory>
#include <filesystem>
#include <vector>
#include <immintrin.h>
#include "PureCPPLib/bitset_.h"
#include "PureCPPLib/io.h"
using namespace std;

class C_Primes_Explorer {
private:
	fstream
		primes_file;
	PCL::bitset<20000000>
		primes_bitset;
public:
	C_Primes_Explorer() = default;
	~C_Primes_Explorer() = default;
	void
		print(),
		run();
};