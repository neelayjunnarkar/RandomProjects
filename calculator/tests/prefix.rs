extern crate calculator;

#[cfg(test)]
mod prefix {

	#[cfg(test)]
	mod sum {
		use calculator::prefix;

		#[test]
		fn pos_pos() { assert_eq!(prefix::eval(&"+ 1 2".to_string()).unwrap(), 3); }

		#[test]
		fn pos_neg() { assert_eq!(prefix::eval(&"+ 1 -2".to_string()).unwrap(), -1); }

		#[test]
		fn neg_pos() { assert_eq!(prefix::eval(&"+ -1 2".to_string()).unwrap(), 1);	}

		#[test]
		fn neg_neg() { assert_eq!(prefix::eval(&"+ -1 -2".to_string()).unwrap(), -3); }
	}
	
	#[cfg(test)]
	mod sub {
		use calculator::prefix;

		#[test]
		fn pos_pos() { assert_eq!(prefix::eval(&"- 1 2".to_string()).unwrap(), -1); }

		#[test]
		fn pos_neg() { assert_eq!(prefix::eval(&"- 1 -2".to_string()).unwrap(), 3); }

		#[test]
		fn neg_pos() { assert_eq!(prefix::eval(&"- -1 2".to_string()).unwrap(), -3); }

		#[test]
		fn neg_neg() { assert_eq!(prefix::eval(&"- -1 -2".to_string()).unwrap(), 1); }
	}

	#[cfg(test)]
	mod mul {
		use calculator::prefix;

		#[test]
		fn pos_pos() { assert_eq!(prefix::eval(&"* 1 2".to_string()).unwrap(), 2); }

		#[test]
		fn pos_neg() { assert_eq!(prefix::eval(&"* 1 -2".to_string()).unwrap(), -2); }

		#[test]
		fn neg_pos() { assert_eq!(prefix::eval(&"* -1 2".to_string()).unwrap(), -2); }

		#[test]
		fn neg_neg() { assert_eq!(prefix::eval(&"* -1 -2".to_string()).unwrap(), 2); }
	}	

	#[cfg(test)]
	mod div {
		use calculator::prefix;

		#[test]
		fn less_greater() { assert_eq!(prefix::eval(&"/ 1 2".to_string()).unwrap(), 0);	}

		#[test]
		fn pos_pos() { assert_eq!(prefix::eval(&"/ 10 2".to_string()).unwrap(), 5); }

		#[test]
		fn pos_neg() { assert_eq!(prefix::eval(&"/ 10 -2".to_string()).unwrap(), -5); }

		#[test]
		fn neg_pos() { assert_eq!(prefix::eval(&"/ 10 2".to_string()).unwrap(), 5);	}

		#[test]
		fn neg_neg() { assert_eq!(prefix::eval(&"/ -10 -2".to_string()).unwrap(), 5); }

		#[test]
		fn zero_zero() { assert!(prefix::eval(&"/ 0 0".to_string()).is_err()); }

		#[test]
		fn neg_zero() { assert!(prefix::eval(&"/ -2 0".to_string()).is_err()); }

		#[test]
		fn pos_zero() { assert!(prefix::eval(&"/ 2 0".to_string()).is_err()); }
	}	
	#[cfg(test)]
	mod modulo {
		use calculator::prefix;

		#[test]
		fn posless_posgreater() { assert_eq!(prefix::eval(&"% 1 2".to_string()).unwrap(), 1); }

		#[test]
		fn posgreater_posless() { assert_eq!(prefix::eval(&"% 11 2".to_string()).unwrap(), 1); }

		#[test]
		fn divisible_posless_posgreater() { assert_eq!(prefix::eval(&"% 0 2".to_string()).unwrap(), 0); }

		#[test]
		fn divisible_posgreater_posless() { assert_eq!(prefix::eval(&"% 10 2".to_string()).unwrap(), 0); }
	}

	#[cfg(test)]
	mod exp {
		use calculator::prefix;

		#[test]
		fn posone_0() { assert_eq!(prefix::eval(&"^ 1 0".to_string()).unwrap(), 1);	}
		
		#[test]
		fn pos_0() { assert_eq!(prefix::eval(&"^ 2 0".to_string()).unwrap(), 1); }

		#[test]
		fn negone_0() { assert_eq!(prefix::eval(&"^ -1 0".to_string()).unwrap(), 1); }

		#[test]
		fn neg_0() { assert_eq!(prefix::eval(&"^ -2 0".to_string()).unwrap(), 1); }

		#[test]
		fn pos_pos_odd() { assert_eq!(prefix::eval(&"^ 2 3".to_string()).unwrap(), 8); }

		#[test]
		fn pos_pos_even() { assert_eq!(prefix::eval(&"^ 3 2".to_string()).unwrap(), 9); }

		#[test]
		fn neg_pos_odd() { assert_eq!(prefix::eval(&"^ -2 3".to_string()).unwrap(), -8); }

		#[test]
		fn neg_pos_even() { assert_eq!(prefix::eval(&"^ -2 2".to_string()).unwrap(), 4); }

		#[test]
		fn pow_to_pow() { assert_eq!(prefix::eval(&"^ 2 ^ 3 2".to_string()).unwrap(), 512); }
	}	
}
