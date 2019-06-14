#include "C_main.h"

int main() {
	try {
		C_main().run();
	}
	catch (std::exception t) {
		cout << "Error detected: " << t.what() << endl;
		system("pause");
	}
    return 0;
}