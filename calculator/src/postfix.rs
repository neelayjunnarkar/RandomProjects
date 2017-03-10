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
	let general_re = regex::Regex::new(r"(-?\d+)\s+(-?\d+)\s+(\+|-\s+|\*|/|\^)").unwrap();

	let mut text = input.clone().trim().to_string();
	text.push(' ');
	let mut simplification_complete = false;

	while !simplification_complete {
		let text_old = text.clone();
		text = general_re.replace_all(text.as_str(), |caps: &regex::Captures| {
			if let (Some(a_m), Some(b_m), Some(op_m)) = (caps.get(1), caps.get(2), caps.get(3)) {
				let op = op_m.as_str();
				let a_r = a_m.as_str().parse::<i32>();
				let b_r = b_m.as_str().parse::<i32>();
				if let (Ok(a), Ok(b)) = (a_r, b_r) {
					let res = match op.trim() {
						"+" => format!("{}", a + b),
						"-" => format!("{}", a - b),
						"*" => format!("{}", a * b),
						"/" => format!("{}", a / b),
						"^" if b >= 0 => format!("{}", a.pow(b as u32)),
						_   => "Invalid Operation".to_string()
					};
					return format!(" {} ", res);
				}
			}
			format!("ERROR")
		}).into_owned(); // End general regex replace_all
		if text == text_old {
			simplification_complete = true;
		}
		println!("{}", text);
	}
	text.trim().parse::<i32>()
}
