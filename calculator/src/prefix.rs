extern crate regex;
use std::num::ParseIntError;

/// Evaluates a piece of text as a prefix notation mathematical expression,
/// and returns the simplified result.
///
/// # Errors
/// If the input is not successfully evaluated (the input is not a valid prefix expression of integers), 
/// returns a ParseIntError.
///
/// # Examples
///
/// ```
/// let expression = "+ 1 - 2 * 3 / 4 -2".to_string();
/// let result = calculator::prefix::eval(&expression);
/// assert_eq!(result.unwrap(), 9);
/// ```
pub fn eval(input: &String) -> Result<i32, ParseIntError> {
	let general_re  = regex::Regex::new(r"(\+|-|\*|/|\^)\s+(-?\d+)\s+(-?\d+)").unwrap();

	let mut text = input.clone().trim().to_string();
	let mut simplification_complete = false;
	
	while !simplification_complete {
		let text_old = text.clone();
		text = general_re.replace_all(text.as_str(), |caps: &regex::Captures| {
						if let (Some(op_m), Some(a_m), Some(b_m)) = (caps.get(1), caps.get(2), caps.get(3)) {
							let op = op_m.as_str();
							let a_r = a_m.as_str().parse::<i32>();
							let b_r = b_m.as_str().parse::<i32>();
							if let (Ok(a), Ok(b)) = (a_r, b_r) {
								let res = match op {
									"+" => format!("{}", a + b),
									"-" => format!("{}", a - b),
									"*" => format!("{}", a * b),
									"/" => format!("{}", a / b),
									"^" if b >= 0 => format!("{}", a.pow(b as u32)),
									_ => "Invalid Operation".to_string()
								};
								return format!("{}", res);
							}
						}
						format!("ERROR")
		}).into_owned(); // End general regex replace_all
		if text == text_old {
			simplification_complete = true;
		}
	}

	text.parse::<i32>()
}
