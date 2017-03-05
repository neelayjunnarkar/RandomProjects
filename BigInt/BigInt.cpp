#include "BigInt.hpp"

#include <algorithm>
#include <cmath>
#include <numeric>
#include <cassert>
#include <cctype>
#include <exception>

#define ERROR(TEST, MESSAGE) if ((TEST)) { throw std::exception(MESSAGE); }

bool is_integer(const string &str) {
	for(const auto &digit : str) {
		if(!isdigit(digit))
			return false;
	}
	return true;
}

BigInt::BigInt(const BigInt &b) :
	_data{b._data},
	_sign{b._sign} {
}

BigInt::BigInt(string _n) {
	if(_n == "") {
		_data.clear();
		_sign = 1;
		return;
	}
	if(_n == "0") {
		_data.push_back(0);
		_sign = 1;
		return;
	}
	if(_n[0] == '-') {
		_sign = -1;
		_n = _n.substr(1, _n.length() - 1);
	} else {
		_sign = 1;
	}
	ERROR(!is_integer(_n), "ERROR: BigInt cstor: Input is neither a blank string nor an integer");

	int right = _n.length();
	int left = max(right - max_bucket_digits, 0);
	string str;
	while(right != left) {
		str = _n.substr(left, right - left);
		right = left;
		left = max(right - max_bucket_digits, 0);
		_data.push_back(stol(str));
	}
}

BigInt BigInt::operator + (BigInt other) {
	if(this->_sign == 1 && other._sign == 1)
		return _pos_pos_sum(*this, other);
	if(this->_sign == -1 && other._sign == -1)
		return -(-*this + -other);
	if(this->_sign == 1 && other._sign == -1)
		return *this - -other;
	if(this->_sign == -1 && other._sign == 1)
		return other - -*this;
	return BigInt();
}

BigInt BigInt::operator - (BigInt other) {
	if(this->_sign == 1 && other._sign == -1) {
		return *this + -other;
	} else if(this->_sign == -1 && other._sign == 1) {
		return -(-*this + other);
	} else if(this->_sign == 1 && other._sign == 1) {
		if(*this > other)
			return _pos_pos_diff(*this, other);
		if(other > *this)
			return -_pos_pos_diff(other, *this);
		return BigInt();
	} else if(this->_sign == -1 && other._sign == -1) {
		return *this + -other;
	}
	return BigInt();
}

BigInt BigInt::operator * (BigInt other) {
	if(this->_sign == 1 && other._sign == 1)
		return _pos_pos_mult(*this, other);
	if(this->_sign == -1 && other._sign == 1)
		return -_pos_pos_mult(-*this, other);
	if(this->_sign == 1 && other._sign == -1)
		return -_pos_pos_mult(*this, -other);
	if(this->_sign == -1 && other._sign == -1)
		return _pos_pos_mult(-*this, -other);
	ERROR(true, "ERROR: operator *: somehow bigint sign conditions were not matched");
}

BigInt BigInt::operator / (BigInt other) {
	if(this->_sign == 1 && other._sign == 1)
		return _pos_pos_div(*this, other);
	if(this->_sign == -1 && other._sign == 1)
		return -_pos_pos_div(-*this, other);
	if(this->_sign == 1 && other._sign == -1)
		return -_pos_pos_div(*this, -other);
	if(this->_sign == -1 && other._sign == -1)
		return _pos_pos_div(-*this, -other);
	ERROR(true, "ERROR: operator /: somehow bigint sign conditions were not matched");
}

BigInt BigInt::operator % (BigInt other) {
	if(this->_sign == 1 && other._sign == 1)
		return _pos_pos_mod(*this, other);
	if(this->_sign == -1 && other._sign == 1)
		return -_pos_pos_mod(-*this, other);
	if(this->_sign == 1 && other._sign == -1)
		return _pos_pos_mod(*this, -other);
	if(this->_sign == -1 && other._sign == -1)
		return -_pos_pos_mod(-*this, -other);
	ERROR(true, "ERROR: operator /: somehow bigint sign conditions were not matched");
}

BigInt BigInt::operator - () {
	BigInt res{*this};
	res._sign *= -1;
	return res;
}

BigInt BigInt::_pos_pos_sum(BigInt a, BigInt b) {
	ERROR(a < BigInt{"0"} || b < BigInt{"0"}, "ERROR: _pos_pos_sum: either a or b is not greater than zero");

	BigInt res{""};
	int max_i = max(b._data.size(), a._data.size());
	res._data.resize(max_i);
	int carry = 0;
	for(int i = 0; i < max_i; ++i) {
		int a_val = (i >= a._data.size()) ? 0 : a._data[i];
		int b_val = (i >= b._data.size()) ? 0 : b._data[i];
		int sum = a_val + b_val + carry;
		int res_val = sum%max_bucket_val;
		carry = sum / max_bucket_val;
		res._data[i] = res_val;
	}
	if(carry != 0)
		res._data.push_back(carry);
	// Trim _data so no leading zeros
	while(res._data.back() == 0)
		res._data.pop_back();
	res._data.shrink_to_fit();
	return res;
}

BigInt BigInt::_pos_pos_diff(BigInt a, BigInt b) {
	ERROR(a < BigInt{"0"} || b < BigInt{"0"}, "ERROR: _pos_pos_diff: either a or b is not greater than zero");
	ERROR(a < b, "ERROR: _pos_pos_diff: a is not greater than b");

	int max_i = max(a._data.size(), b._data.size()) - 1; //cuz want to deal with end separately
	for(int i = 0; i < max_i; ++i) {
		int a_val = (i >= a._data.size()) ? 0 : a._data[i];
		int b_val = (i >= b._data.size()) ? 0 : b._data[i];
		if(a_val >= b_val) {
			a._data[i] -= b_val;
		} else if(b_val > a_val) {
			a._data[i] = max_bucket_val + a_val - b_val;
			a._data[i + 1] -= 1;
		}
	}
	int a_val = (max_i >= a._data.size()) ? 0 : a._data[max_i];
	int b_val = (max_i >= b._data.size()) ? 0 : b._data[max_i];
	if(a_val >= b_val) {
		a._data[max_i] -= b_val;
	} else if(b_val > a_val) {
		a._data[max_i] = b_val - a_val;
		a._sign = -1;
	}
	while(a._data.back() == 0)
		a._data.pop_back();
	a._data.shrink_to_fit();
	return a;
}

BigInt BigInt::_pos_pos_mult(BigInt a, BigInt b) {
	ERROR(a < BigInt{"0"} || b < BigInt{"0"}, "ERROR: _pos_pos_mult: either a or b is not greater than zero");
	BigInt res{"0"};
	for(BigInt c{"0"}; c < b; c = c + BigInt{"1"}) {
		res = res + a;
	}
	return res;
}

BigInt BigInt::_pos_pos_div(BigInt a, BigInt b) {
	ERROR(a < BigInt{"0"} || b < BigInt{"0"}, "ERROR: _pos_pos_div: either a or b is not greater than zero");
	ERROR(b == BigInt{"0"}, "ERROR: _pos_pos_div: divide by zero");
	BigInt remainder{a};
	BigInt quotient{"0"};
	while(remainder >= b) {
		remainder = remainder - b;
		quotient = quotient + BigInt{"1"};
	}
	return quotient;
}

BigInt BigInt::_pos_pos_mod(BigInt a, BigInt b) {
	ERROR(a < BigInt{"0"} || b < BigInt{"0"}, "ERROR: _pos_pos_mod: either a or b is not greater than zero");
	ERROR(b == BigInt{"0"}, "ERROR: _pos_pos_mod: mod by zero");
	BigInt remainder{a};
	while(remainder >= b) {
		remainder = remainder - b;
	}
	return remainder;
}

bool BigInt::operator > (BigInt other) {
	if(this->_sign == 1 && other._sign == -1)
		return true;
	if(this->_sign == -1 && other._sign == 1)
		return false;
	if(this->_sign == -1 && other._sign == -1)
		return -*this < -other;
	if(this->_data.size() > other._data.size())
		return true;
	if(this->_data.size() < other._data.size())
		return false;
	for(int i = this->_data.size() - 1; i >= 0; --i) {
		if(this->_data[i] > other._data[i])
			return true;
		if(this->_data[i] < other._data[i])
			return false;
	}
	return false; //are equal
}

bool BigInt::operator <= (BigInt other) {
	return !(*this > other);
}

bool BigInt::operator < (BigInt other) {
	if(this->_sign == -1 && other._sign == 1)
		return true;
	if(this->_sign == 1 && other._sign == -1)
		return false;
	if(this->_sign == -1 && other._sign == -1)
		return -*this > -other;
	if(this->_data.size() < other._data.size())
		return true;
	if(this->_data.size() > other._data.size())
		return false;
	for(int i = this->_data.size() - 1; i >= 0; --i) {
		if(this->_data[i] < other._data[i])
			return true;
		if(this->_data[i] > other._data[i])
			return false;
	}
	return false; //are equal
}

bool BigInt::operator >= (BigInt other) {
	return !(*this < other);
}

bool BigInt::operator == (BigInt other) {
	if(this->_sign != other._sign)
		return false;
	if(this->_data.size() != other._data.size())
		return false;
	for(int i = this->_data.size() - 1; i >= 0; --i) {
		if(this->_data[i] != other._data[i])
			return false;
	}
	return true;
}

bool BigInt::operator != (BigInt other) {
	return !(*this == other);
}

string BigInt::represent() const {
	string representation = accumulate(_data.begin(), _data.end(), string{}, [] (string str, int n) { return str + string{","}  +::to_string(n); });
	string sign = _sign == 1 ? string{""} : string{"-"};
	return sign + representation;
}

string BigInt::to_string() const {
	auto temp = _data;
	reverse(temp.begin(), temp.end());
	auto num_to_string = [] (int n) -> string {
		string str = ::to_string(n);
		return string(max_bucket_digits - str.length(), '0') + str;
	};
	string str = ::to_string(*temp.begin()) + accumulate(temp.begin() + 1, temp.end(), string{}, [&] (string str, int n) { return str + num_to_string(n); });
	string sign = _sign == 1 ? string{""} : string{"-"};
	return sign + str;
}

ostream& operator << (ostream &os, const BigInt &n) {
	return os << n.to_string();
}