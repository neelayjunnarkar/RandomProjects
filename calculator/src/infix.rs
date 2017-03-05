// A macro that creates a closure that given 2 values a and b, and an operation op, returns op(a, b)
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


// A macro hat creates a function that given a specific operation and an expression without parentheses, simplifies all instances of that operation
macro_rules! eval_base_expr_op {
	($func:ident, $op:tt, $re:expr) => (
		fn $func(input: &String) -> String {
			let re = $re;
			let mut text = input.clone().trim().to_string();
			let mut simplification_complete = false;
			while !simplification_complete {
				let text_old = text.clone();
				text = re.replace_all(text.as_str(), simplify_op!($op)).into_owned();
				if text == text_old {
					simplification_complete = true;
				}
			}
			text
		}
	)
}

extern crate regex;
use std::num::ParseIntError;

// Creating functions that simplify expressions for the operations mult, div, sum, diff
eval_base_expr_op!(eval_base_expr_mult, *, regex::Regex::new(r"(-?\d+)\s*\*\s*(-?\d+)").unwrap());
eval_base_expr_op!(eval_base_expr_div, /, regex::Regex::new(r"(-?\d+)\s*/\s*(-?\d+)").unwrap());
eval_base_expr_op!(eval_base_expr_sum, +,  regex::Regex::new(r"(-?\d+)\s*\+\s*(-?\d+)").unwrap());
eval_base_expr_op!(eval_base_expr_diff, -,  regex::Regex::new(r"(-?\d+)\s*-\s*(-?\d+)").unwrap()); 


// Expects an expression that has no parenthesis and computes the value, returning as text
fn eval_base_expr(input: &String) -> String {
	let mut text = input.clone().trim().to_string();
	let mut simplification_complete = false;

	while !simplification_complete {
		let text_old = text.clone();

		// By PEMDAS, simplify all instances of mult and div, then sum and diff
		text = eval_base_expr_mult(&text);
		text = eval_base_expr_div(&text);
		text = eval_base_expr_sum(&text);
		text = eval_base_expr_diff(&text);

		if text == text_old {
			simplification_complete = true;
		}
	}
	text
}

// Expects an infix expression, with parentheses or no, and computes the value, returning it as a Result<i32, ParseIntError>
// Supports +, -, *, /, and parentheses
pub fn eval(input: &String) -> Result<i32, ParseIntError> {
	// Regular expression to find expressions surrounded by parentheses
	let parenth_expr = regex::Regex::new(r"\((-?\d+\s*(\+|-|\*|/)\s*-?\d+(\s*(\+|-|\*|/)\s*-?\d+)*)\)").unwrap();

	// Regular expression to find any expression, not necessarily surrounded by parentheses
	let nonparenth_expr = regex::Regex::new(r"(-?\d+\s*(\+|-|\*|/)\s*-?\d+(\s*(\+|-|\*|/)\s*-?\d+)*)").unwrap();

	let mut text: String = input.clone().trim().to_string();

	// Simplify expessions surrounded by parentheses
	while parenth_expr.is_match(text.as_str()) {
		text = parenth_expr.replace_all(text.as_str(), |caps: &regex::Captures| {
			let expr = caps.get(1).map_or("ERROR", |m| m.as_str()).to_string();
			eval_base_expr(&expr)
		}).into_owned();
	}

	// Simplify expressions that are not surrounded with parentheses
	while nonparenth_expr.is_match(text.as_str()) {
		text = nonparenth_expr.replace_all(text.as_str(), |caps: &regex::Captures| {
			let expr = caps.get(1).map_or("ERROR", |m| m.as_str()).to_string();
			eval_base_expr(&expr)
		}).into_owned();
	}
	text.parse::<i32>()
}
