/// Creates a closure of that takes a &regex::Captures and returns an &str.
/// The closure expects there to be 2 groups in the &regex::Captures,
/// both of them parsable into i32s.
/// The closure returns op(a, b) as an &str, where a and b are the i32s.
/// Used to pass into regex::Regex::replace_all to create the replacement text.
macro_rules! simplify_op {
	($op:tt) => (
		|caps: &regex::Captures| {
			if let (Some(a), Some(b)) = (caps.get(1), caps.get(2)) {
				let a_val = a.as_str().parse::<i32>().unwrap();
				let b_val = b.as_str().parse::<i32>().unwrap();
				return format!(" {} ", a_val $op b_val);
			}
			format!("ERROR")
		}		
	)
}


extern crate regex;
use std::num::ParseIntError;

/// Evaluates a piece of text as a postfix notation mathematical expression,
/// and returns the simplified result
///
/// # Errors
/// If the input is not successfully evaluated (the input is not a valid postfix expression of integers),
/// returns a ParseIntError
///
/// # Examples
///
/// ```
/// let expression = "2 4 -2 / 3 * - 1 +".to_string();
/// let result = calculator::postfix::eval(&expression);
/// assert_eq!(result.unwrap(), 9);
/// ```
pub fn eval(input: &String) -> Result<i32, ParseIntError> {
	let sum_re  = regex::Regex::new(r"(-?\d+)\s+(-?\d+)\s+\+").unwrap();
	let diff_re = regex::Regex::new(r"(-?\d+)\s+(-?\d+)\s+-\s+").unwrap();
	let mult_re = regex::Regex::new(r"(-?\d+)\s+(-?\d+)\s+\*").unwrap();
	let div_re  = regex::Regex::new(r"(-?\d+)\s+(-?\d+)\s+/").unwrap();

	let mut text = input.clone().trim().to_string();
	text.push(' ');
	let mut simplification_complete = false;

	while !simplification_complete {
		let text_old = text.clone();
		text = sum_re.replace_all(text.as_str(),simplify_op!(+)).into_owned();
		text = diff_re.replace_all(text.as_str(),simplify_op!(-)).into_owned();
		text = mult_re.replace_all(text.as_str(),simplify_op!(*)).into_owned();
		text = div_re.replace_all(text.as_str(),simplify_op!(/)).into_owned();
		if text == text_old {
			simplification_complete = true;
		}
	}
	text.trim().parse::<i32>()
}
