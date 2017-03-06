// This file is used for testing purposes

extern crate calculator;

use std::io;
use std::env;

fn main() {
	let args = env::args().collect::<Vec<String>>();
	if args.len() != 2 {
		println!("Program should be run in format: <exe> <pre or post or in>");
		println!("Pass whether input should be evaluated as prefix or postfix or infix notation by passing \"pre\" or \"post\" or \"in\"");
		return;
	}

	let eval_type = &args[1];

	let mut input = String::new();
	loop {
		io::stdin().read_line(&mut input).expect("Failed to read input line");
		if eval_type == "pre" {
			if let Ok(eval) = calculator::prefix::eval(&input) {
				println!("= {}", eval);	
			} else {
				println!("Prefix evaluation failed");
			}
		} else if eval_type == "post" {
			if let Ok(eval) = calculator::postfix::eval(&input) {
				println!("= {}", eval);	
			} else {
				println!("Postfix evaluation failed");
			}
		} else if eval_type == "in" {
			if let Ok(eval) = calculator::infix::eval(&input) {
				println!("= {}", eval);
			} else { 
				println!("Infix evaluation failed");
			}
		}
		input.clear();
	}
}
