/// A macro that creates a function that,
/// given a specific operation and an expression without parentheses,
/// simplifies all instances of that operation.
macro_rules! eval_base_expr_op {
	($func:ident, $re:expr, $op:expr) => (
		fn $func(input: &String) -> String {
			let re = $re;
			let mut text = input.clone().trim().to_string();
			let mut simplification_complete = false;
			while !simplification_complete {
				let text_old = text.clone();
				text = re.replace_all(text.as_str(), |caps: &regex::Captures| {
					if let (Some(a_m), Some(b_m)) = (caps.get(1), caps.get(2)) {
						let a_r = a_m.as_str().parse::<i32>();
						let b_r = b_m.as_str().parse::<i32>();
						if let (Ok(a), Ok(b)) = (a_r, b_r) {
							return format!("{}", $op(a, b));
						}
					}
					format!("ERROR")
				}).into_owned();
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

// Creating functions that simplify expressions for the operations mult, div, sum, diff.
eval_base_expr_op!(eval_base_expr_exp, regex::Regex::new(r"(-?\d+)\s*\^\s*(\d+)").unwrap(), |a: i32, b: i32| a.pow(b as u32));
eval_base_expr_op!(eval_base_expr_mult, regex::Regex::new(r"(-?\d+)\s*\*\s*(-?\d+)").unwrap(), |a, b| a * b);
eval_base_expr_op!(eval_base_expr_div,regex::Regex::new(r"(-?\d+)\s*/\s*(-?\d+)").unwrap(), |a, b| if b == 0 { "ERROR_DivByZero".to_string() } else { format!("{}", a / b) } );
eval_base_expr_op!(eval_base_expr_sum,  regex::Regex::new(r"(-?\d+)\s*\+\s*(-?\d+)").unwrap(), |a, b| a + b);
eval_base_expr_op!(eval_base_expr_diff,  regex::Regex::new(r"(-?\d+)\s*-\s*(-?\d+)").unwrap(), |a, b| a - b); 

/// Evaluates a piece of text as an infix notation mathematical expression without parentheses,
/// and returns the simplified result as a String.
///
/// Supports +, -, *, /, and evaluates by MDAS (no parentheses nor exponents).
///
/// # Errors
/// If there are errors in simplification, 
/// the region that caused an error will be replaced with "ERROR".
fn eval_base_expr(input: &String) -> String {
	let mut text = input.clone().trim().to_string();
	let mut simplified = false;

	while !simplified {
		let text_old = text.clone();
		// By PEMDAS, simplify all instances of mult and div, then sum and diff
		text = eval_base_expr_exp(&text);
		text = eval_base_expr_mult(&text);
		text = eval_base_expr_div(&text);
		text = eval_base_expr_sum(&text);
		text = eval_base_expr_diff(&text);
		if text == text_old {
			simplified = true;
		}
	}
	text
}


/// Evaluates a piece of text as an infix notation mathematical expression 
/// with or without parentheses, and returns the simplified result.
///
/// Supports +, -, *, /, and parentheses, and evaluates by PMDAS (no exponents)
///
/// # Errors
/// If the input is not successfully evaluated (the input is not a valid infix expression of integers),
/// returns a ParseIntError
///
/// Without parentheses:
/// # Examples
/// ```
/// let expression = "1 + 2 - 3 * 4 / -2".to_string();
/// let result = calculator::infix::eval(&expression);
/// assert_eq!(result.unwrap(), 9);
/// ```
/// With parentheses:
/// # Examples
/// ```
/// let expression = "1 + (2 - 3) * 4 / -2".to_string();
/// let result = calculator::infix::eval(&expression);
/// assert_eq!(result.unwrap(), 3);
/// ```
pub fn eval(input: &String) -> Result<i32, ParseIntError> {
	// Regular expression to find expressions surrounded by parentheses
	let parenth_expr = regex::Regex::new(r"\((-?\d+\s*(\+|-|\*|/|\^)\s*-?\d+(\s*(\+|-|\*|/|\^)\s*-?\d+)*)\)").unwrap();

	// Regular expression to find any expression, not necessarily surrounded by parentheses
	let nonparenth_expr = regex::Regex::new(r"(-?\d+\s*(\+|-|\*|/|\^)\s*-?\d+(\s*(\+|-|\*|/|\^)\s*-?\d+)*)").unwrap();

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
