macro_rules! simplify_op {
	($op:tt) => (
		|caps: &regex::Captures| {
			if let (Some(a), Some(b)) = (caps.get(1), caps.get(2)) {
				let a_val = a.as_str().parse::<i32>().unwrap();
				let b_val = b.as_str().parse::<i32>().unwrap();
				return format!("{}", a_val $op b_val);
			}
			format!("ERROR")
		}		
	)
}


extern crate regex;
use std::num::ParseIntError;

pub fn eval(input: &String) -> Result<i32, ParseIntError> {
	let sum_re  = regex::Regex::new(r"(-?\d+)\s+(-?\d+)\s*\+").unwrap();
	let diff_re = regex::Regex::new(r"(-?\d+)\s+(-?\d+)\s*-").unwrap();
	let mult_re = regex::Regex::new(r"(-?\d+)\s+(-?\d+)\s*\*").unwrap();
	let div_re  = regex::Regex::new(r"(-?\d+)\s+(-?\d+)\s*/").unwrap();

	let mut text = input.clone().trim().to_string();
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
	text.parse::<i32>()
}
