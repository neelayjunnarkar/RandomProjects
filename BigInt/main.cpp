#include "BigInt.hpp"

int main(int argc, char **argv) {
	if (argc != 2 && argc != 4) {
		std::cerr << "exec in format: <exe> <num>" << std::endl;
		std::cerr << "OR" << std::endl;
		std::cerr << "exec in format: <exe> <num1> <plus or minus> <num2>" << std::endl;
		return 1;
	}
	if (argc == 2) {
		BigInt a{argv[1]};
		cout << "a: " << a << "    " << a.represent() << endl;
	} else if (argc == 4) {
		if (string{argv[2]} == string{"+"}) {
			BigInt a{argv[1]};
			BigInt b{argv[3]};
			cout << "a: " << a << "    " << a.represent() << endl;
			cout << "b: " << b << "    " << b.represent() << endl;
			cout << "a + b = " << a+b << "    " << (a+b).represent() << endl; 
		} else if (string{argv[2]} == string{"-"}) {
			BigInt a{argv[1]};
			BigInt b{argv[3]};
			cout << "a: " << a << "    " << a.represent() << endl;
			cout << "b: " << b << "    " << b.represent() << endl;
			cout << "a - b = " << a-b << "    " << (a-b).represent() << endl; 
		}
	}
}
