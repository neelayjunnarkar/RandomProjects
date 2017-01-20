#include <vector>
#include <string>
#include <iostream>

using namespace std;

class BigInt {
public:
	BigInt(const BigInt &b);
	BigInt(string _n = "0");

	BigInt operator + (BigInt other);

	BigInt operator - (BigInt other);

	BigInt operator - ();

	bool operator > (BigInt other);

	string represent() const;

	string to_string() const;

private: 
	static BigInt _pos_pos_sum(BigInt a, BigInt b);

	// a must be greater than b
	static BigInt _pos_pos_diff(BigInt a, BigInt b);

	int _sign = 1;

	vector<int> _data;
	
	static constexpr int max_bucket_val = 1000000000;
	static constexpr int max_bucket_digits = 9;
	
};

ostream& operator << (ostream &os, const BigInt &n);
