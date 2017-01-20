#include "BigInt.hpp"

#include <algorithm>
#include <cmath>
#include <numeric>

BigInt::BigInt(const BigInt &b):
	_data{b._data},
	_sign{b._sign} {
	
}

BigInt::BigInt(string _n) {
	if (_n == "")
		return;
	if (_n == "0") {
		_data.push_back(0);
		return;
	}
	if (_n[0] == '-') {
		_sign = -1;
		_n = _n.substr(1, _n.length()-1);
	}
	int right = _n.length();
	int left = max(right-max_bucket_digits, 0);
	string str;
	while (right != left) {
		str = _n.substr(left, right-left);
		right = left;
		left = max(right-max_bucket_digits, 0);
		_data.push_back(stol(str));
	}
}

BigInt BigInt::operator + (BigInt other) {
	if (this->_sign == 1 && other._sign == 1)
		return _pos_pos_sum(*this, other);	
	if (this->_sign == -1 && other._sign == -1)
		return -(-*this + -other);
	if (this->_sign == 1 && other._sign == -1)
		return *this - -other;
	if (this->_sign == -1 && other._sign == 1)
		return other - -*this;	
	return BigInt();
}

BigInt BigInt::operator - (BigInt other) {
	if (this->_sign == 1 && other._sign == -1) {
		return *this + -other;
	} else if (this->_sign == -1 && other._sign == 1) {
		return -(-*this + other);
	} else if (this->_sign == 1 && other._sign == 1) {
		if (*this > other)
			return _pos_pos_diff(*this, other);
		if (other > *this)
			return -_pos_pos_diff(other, *this);
		return BigInt();
	} else if (this->_sign == -1 && other._sign == -1) {
		return *this + -other;
	}
	return BigInt();
}

BigInt BigInt::operator - () {
	BigInt res{*this};
	res._sign *= -1;
	return res;
}

BigInt BigInt::_pos_pos_sum(BigInt a, BigInt b) {
		BigInt res{""};
		int max_i = max(b._data.size(), a._data.size());
		res._data.resize(max_i);
		int carry = 0;
		for (int i = 0; i < max_i; ++i) {
			int a_val = (i >= a._data.size()) ? 0 : a._data[i];
			int b_val = (i >= b._data.size()) ? 0 : b._data[i];
			int sum = a_val + b_val + carry;
			int res_val = sum%max_bucket_val;
			carry = sum/max_bucket_val;
			res._data[i] = res_val;
		}
		if (carry != 0)
			res._data.push_back(carry);
		// Trim _data so no leading zeros
		while (res._data.back() == 0)
			res._data.pop_back();
		res._data.shrink_to_fit();
		return res;	
}
BigInt BigInt::_pos_pos_diff(BigInt a, BigInt b) {
	int max_i = max(a._data.size(), b._data.size())-1; //cuz want to deal with end separately
	for (int i = 0; i < max_i; ++i) {
		if (a._data[i] >= b._data[i]) {
			a._data[i] -= b._data[i];
		} else if (b._data[i] > a._data[i]) {
			a._data[i] = max_bucket_val + a._data[i] - b._data[i];
			a._data[i+1] -= 1;
		}
	}
	if (a._data[max_i] >= b._data[max_i]) {
		a._data[max_i] -= b._data[max_i];
	} else if (b._data[max_i] > a._data[max_i]) {
		a._data[max_i] = b._data[max_i] - a._data[max_i];
		a._sign = -1;
	}
	while (a._data.back() == 0)
		a._data.pop_back();
	a._data.shrink_to_fit();
	return a;
}

bool BigInt::operator > (BigInt other) {
	if (this->_data.size() > other._data.size())
		return true;
	if (this->_data.size() < other._data.size())
		return false;
	for (int i = this->_data.size()-1; i >= 0; --i) {
		if (this->_data[i] > other._data[i])
			return true;
		if (this->_data[i] < other._data[i])
			return false;
	}
	return false; //are equal
}

string BigInt::represent() const {
	string representation = accumulate(_data.begin(), _data.end(), string{}, [](string str, int n) { return str + string{","}  + ::to_string(n); });
	string sign = _sign == 1 ? string{""} : string{"-"};
	return sign + representation;
}

string BigInt::to_string() const {
	auto temp = _data;
	reverse(temp.begin(), temp.end());
	auto num_to_string = [](int n) -> string {
		string str = ::to_string(n);
		string pad = "";
		for (int i = 0; i < max_bucket_digits-str.length(); ++i) 
			pad += "0";
		return pad + str;
	};
	string str = ::to_string(*temp.begin()) + accumulate(temp.begin()+1, temp.end(), string{}, [&](string str, int n) { return str + num_to_string(n); });
	string sign = _sign == 1 ? string{""} : string{"-"};
	return sign + str;
}

ostream& operator << (ostream &os, const BigInt &n) {
	return os << n.to_string();
}

