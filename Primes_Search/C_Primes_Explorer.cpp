#include "C_Primes_Explorer.h"
#define print_var(v) #v << " = " << v << "	"


void C_Primes_Explorer::print() {
	fstream
		primes_other("primes.bin", ios::in | ios::binary);
	uint_fast64_t
		block_index,
		primes_number = 0;
	PCL::bitset<20000000>
		block;
	cout << "Liczba blokow: " << experimental::filesystem::file_size("primes.bin") / 2500000 << endl << "Wybrany blok: ";
	cin >> block_index;
	primes_other.seekg(2500000 * block_index);
	primes_other.read(reinterpret_cast<char*>(&block), 2500000);
	primes_other.close();
	for (uint_fast64_t i = 0; i < 20000000; i++) {
		if (block[i]) {
			primes_number++;
		}
	}
	cout << line << endl << "Ilosc liczb pierwszych: " << primes_number << endl;
	primes_other.close();
}

void C_Primes_Explorer::run() {
	uint_fast64_t
		mirroring_itself = 0,
		mirrored_pairs = 0,
		unmirrored = 0,
		prime = 0,
		prime_bitshift = 0,
		prime_flipped = 0;
	vector<uint_fast64_t>
		divisors_array;
	fstream
		primes("primes.bin", ios::in | ios::binary | ios::ate);
	auto is_prime = [&](uint_fast64_t number) {
		uint_fast64_t rounded_sqrt = static_cast<uint_fast64_t>(ceil(sqrt(number)));
		for (auto& divisor : divisors_array) {
			if (number % divisor == 0) {
				return false;
			}
			if (divisor >= rounded_sqrt) {
				return true;
			}
		}
	};
	auto bit_reverse = [](uint_fast64_t number) {
		number = _byteswap_uint64(number);
		auto reverse_bits_in_byte = [](unsigned char& b) {
			b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
			b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
			b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
		};
		for (uint_fast8_t byte_index = 0; byte_index < sizeof(number); byte_index++) {
			reverse_bits_in_byte(reinterpret_cast<unsigned char*>(&number)[byte_index]);
		}
		return number;
	};
	primes.seekg(0);
	primes.read(reinterpret_cast<char*>(&primes_bitset), primes_bitset.byte_size());
	for (uint_fast64_t index = 0; index < primes_bitset.size(); index++) {
		if (primes_bitset[index]) {
			divisors_array.push_back(index);
		}
	}
	for (uint_fast64_t index = 3; index < primes_bitset.size(); index++) {
		if (primes_bitset[index]) {
			prime = index;
			prime_bitshift = 63 - static_cast<uint_fast64_t>(floor(log2(prime)));
			prime_flipped = bit_reverse(prime) >> prime_bitshift;
			if (is_prime(prime_flipped)) {
				if (prime == prime_flipped) {
					++mirroring_itself;
				}
				else if (prime < prime_flipped) {
					++mirrored_pairs;
				}
			}
			else {
				++unmirrored;
			}
		}
	}
	cout << "0: " << print_var(mirrored_pairs) << print_var(mirroring_itself) << print_var(unmirrored) << endl;
	for (uint_fast64_t block_index = 1; block_index < filesystem::file_size("primes.bin") / primes_bitset.byte_size(); block_index++) {
		primes.read(reinterpret_cast<char*>(&primes_bitset), primes_bitset.byte_size());
		for (uint_fast64_t index = 3; index < primes_bitset.size(); index++) {
			if (primes_bitset[index]) {
				prime = index + block_index * primes_bitset.size();
				prime_bitshift = 63 - static_cast<uint_fast64_t>(floor(log2(prime)));
				prime_flipped = bit_reverse(prime) >> prime_bitshift;
				if (is_prime(prime_flipped)) {
					if (prime == prime_flipped) {
						++mirroring_itself;
					}
					else if (prime < prime_flipped) {
						++mirrored_pairs;
					}
				}
				else {
					++unmirrored;
				}
			}
		}
		cout << block_index << ": " << print_var(mirrored_pairs) << print_var(mirroring_itself) << print_var(unmirrored) << endl;
	}
	primes.close();
}