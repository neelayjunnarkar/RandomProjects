#include <map>
#include "BigInt.hpp"
#include <functional>


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
		std::map<string, std::function<void()>> fns;
		BigInt a(argv[1]);
		BigInt b(argv[3]);
		cout << "a: " << a << "    " << a.represent() << endl;
		cout << "b: " << b << "    " << b.represent() << endl;
		cout << boolalpha << endl;
		fns["+"] = [&](){ cout << "a + b = " << a+b << "    " << (a+b).represent() << endl; };
		fns["-"] = [&](){ cout << "a - b = " << a-b << "    " << (a-b).represent() << endl; };
		fns["gt"] = [&]() { cout << "a > b = " << (a > b) << endl; };
		fns["lte"] = [&]() { cout << "a <= b = " << (a <= b)  << endl; };
		fns["lt"] = [&]() { cout << "a < b = " << (a < b) << endl; };
		fns["gte"] = [&]() { cout << "a >= b = " << (a >= b) << endl; };
		fns["=="] = [&]() { cout << "a == b = " << (a == b) << endl; };
		fns["!="] = [&]() { cout << "a != b = " << (a != b) << endl; };
		fns[argv[2]]();
	}
}
