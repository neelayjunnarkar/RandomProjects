#include "BigInt.hpp"

#include <map>
#include <functional>
#include <iostream>
#include <string>
#include <sstream>
#include <iterator>

int main(int argc, char **argv) {

	while(true) {
		try {
			// Take in a line of input and split it into tokens
			string line;
			getline(cin, line);
			istringstream iss(line);
			vector<string> args;
			copy(istream_iterator<string>(iss), istream_iterator<string>(), back_inserter(args));

			if(args.size() != 1 && args.size() != 3) {
				std::cerr << "enter in format: <num>" << std::endl;
				std::cerr << "OR" << std::endl;
				std::cerr << "in format: <num1> <operation> <num2>" << std::endl;
				continue;
			}
			if(args.size() == 1) {
				if(args[0] == "q")
					break;
				BigInt a{args[0]};
				cout << "a: " << a << "    " << a.represent() << endl;
			} else if(args.size() == 3) {
				std::map<string, std::function<void()>> fns;
				BigInt a(args[0]);
				BigInt b(args[2]);
				cout << "a: " << a << "    " << a.represent() << endl;
				cout << "b: " << b << "    " << b.represent() << endl;
				cout << boolalpha;
				fns["+"]  = [&] () { cout << "a + b = " << a + b << "    " << (a + b).represent() << endl; };
				fns["-"]  = [&] () { cout << "a - b = " << a - b << "    " << (a - b).represent() << endl; };
				fns[">"]  = [&] () { cout << "a > b = " << (a > b) << endl; };
				fns["<="] = [&] () { cout << "a <= b = " << (a <= b) << endl; };
				fns["<"]  = [&] () { cout << "a < b = " << (a < b) << endl; };
				fns[">="] = [&] () { cout << "a >= b = " << (a >= b) << endl; };
				fns["=="] = [&] () { cout << "a == b = " << (a == b) << endl; };
				fns["!="] = [&] () { cout << "a != b = " << (a != b) << endl; };
				fns["*"]  = [&] () { cout << "a * b = " << (a * b) << endl; };
				fns["/"] = [&] () { cout << "a / b = " << (a / b) << endl; };
				fns["%"]  = [&] () { cout << "a % b = " << (a % b) << endl; };
				fns[args[1]]();
			}
			cout << endl;
		} catch(std::exception &e) {
			cerr << e.what() << endl;
			cout << endl;
		}
	}

}
