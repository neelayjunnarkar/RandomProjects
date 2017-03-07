extern crate calculator;

#[cfg(test)]
mod infix {

	#[cfg(test)]
	mod sum {
		use calculator::infix;

		#[test]
		fn pos_pos() { assert_eq!(infix::eval(&"1 + 2".to_string()).unwrap(), 3); }

		#[test]
		fn pos_neg() { assert_eq!(infix::eval(&"1 + -2".to_string()).unwrap(), -1); }

		#[test]
		fn neg_pos() { assert_eq!(infix::eval(&"-1 + 2".to_string()).unwrap(), 1);	}

		#[test]
		fn neg_neg() { assert_eq!(infix::eval(&"-1 + -2".to_string()).unwrap(), -3); }
	}
	
	#[cfg(test)]
	mod sub {
		use calculator::infix;

		#[test]
		fn pos_pos() { assert_eq!(infix::eval(&"1 - 2".to_string()).unwrap(), -1); }

		#[test]
		fn pos_neg() { assert_eq!(infix::eval(&"1 - -2".to_string()).unwrap(), 3); }

		#[test]
		fn neg_pos() { assert_eq!(infix::eval(&"-1 - 2".to_string()).unwrap(), -3); }

		#[test]
		fn neg_neg() { assert_eq!(infix::eval(&"-1 - -2".to_string()).unwrap(), 1); }
	}

	#[cfg(test)]
	mod mul {
		use calculator::infix;

		#[test]
		fn pos_pos() { assert_eq!(infix::eval(&"1 * 2".to_string()).unwrap(), 2); }

		#[test]
		fn pos_neg() { assert_eq!(infix::eval(&"1 * -2".to_string()).unwrap(), -2); }

		#[test]
		fn neg_pos() { assert_eq!(infix::eval(&"-1 * 2".to_string()).unwrap(), -2); }

		#[test]
		fn neg_neg() { assert_eq!(infix::eval(&"-1 * -2".to_string()).unwrap(), 2); }
	}	

	#[cfg(test)]
	mod div {
		use calculator::infix;

		#[test]
		fn less_greater() { assert_eq!(infix::eval(&"1 / 2".to_string()).unwrap(), 0);	}

		#[test]
		fn pos_pos() { assert_eq!(infix::eval(&"10 / 2".to_string()).unwrap(), 5); }

		#[test]
		fn pos_neg() { assert_eq!(infix::eval(&"10 / -2".to_string()).unwrap(), -5); }

		#[test]
		fn neg_pos() { assert_eq!(infix::eval(&"10 / 2".to_string()).unwrap(), 5);	}

		#[test]
		fn neg_neg() { assert_eq!(infix::eval(&"-10 / -2".to_string()).unwrap(), 5); }
	}	
	
}
