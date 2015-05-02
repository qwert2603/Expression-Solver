#ifndef EXPRESSION
#define EXPRESSION

#include <list>
#include <string>
#include <stdexcept>
#include <algorithm>

namespace expression_namespace {

	class Expression {
	public:
		explicit Expression(const std::string &_expr) : expr(_expr) {}
		double compute() const;	// get result of expresion
	private:
		// separate digits and operations in expr
		void separating(std::list<double> &_digits, std::list<char> &_symbols, std::list<unsigned> &_priorities) const;
		// get operations and brackets in _symbol
		// make _priorities and delete brackets from _symbols
		void prioritization(std::list<char> &_symbols, std::list<unsigned> &_priorities) const;
		std::string expr;	// expression itself
	};

	// ? is this correct symbol of operation
	bool is_oper(char _c) {
		return _c == '+'
			|| _c == '-'
			|| _c == '*'
			|| _c == '/';
	}

	double Expression::compute() const {
		std::list<double> digits;	// digits from expr
		std::list<char> symbols;	// symbols of operations
		std::list<unsigned> priorities;	// priorities of operations in symbols. priorities.size() == symbols.size()
		separating(digits, symbols, priorities);
		// get max priority in expression
		unsigned cur_pr = *std::max_element(priorities.cbegin(), priorities.cend());
		while (cur_pr != 0) {
			// iterator to the begin of containers
			std::list<double>::iterator dig_iter = digits.begin();
			std::list<char>::iterator sym_iter = symbols.begin();
			std::list<unsigned>::iterator pri_iter = priorities.begin();
			// iterator to the end of priorities
			std::list<unsigned>::iterator pri_iter_end = priorities.end();
			while (pri_iter != pri_iter_end) {
				// if this priority is equal to processed priority
				if (*pri_iter == cur_pr) {
					std::list<double>::iterator local_dig_iter = dig_iter;
					// get operands
					double d1 = *local_dig_iter;
					double d2 = *++local_dig_iter;
					switch (*sym_iter) {
					case '+':	*dig_iter = d1 + d2; break;
					case '-':	*dig_iter = d1 - d2; break;
					case '*':	*dig_iter = d1 * d2; break;
					case '/': {
								  if (d2 != 0) {
									  *dig_iter = d1 / d2; break;
								  }
								  else {
									  throw std::runtime_error("! division by 0");
								  }
					}
					}
					// erase finished elements from containers
					digits.erase(local_dig_iter);
					sym_iter = symbols.erase(sym_iter);
					pri_iter = priorities.erase(pri_iter);
				}
				else {
					//move to the next operation
					++dig_iter;
					++sym_iter;
					++pri_iter;
				}
			}
			// move to lower priority
			--cur_pr;
		}
		// result is first element in digits
		return digits.front();
	}

	void Expression::separating(std::list<double> &_digits, std::list<char> &_symbols, std::list<unsigned> &_priorities) const {
		std::string::size_type lenght = expr.size();
		std::string::size_type current = 0;
		// size of digit (in symbols)
		std::string::size_type digit_size = 0;
		// digit getted from expr
		double digit;
		// ? is previous symbol - digit
		// it is necessary for sharing operator '-' and negative digits, those have '-'. for example: (3 - -4)
		bool post_digit = true;
		while (current != lenght) {
			try {
				digit = stod(expr.substr(current), &digit_size);
			}
			catch (std::invalid_argument ia) {
				;	// if we are here, then current symbol in expr is not digit. 
					// this mean we just push this sumbol to _symbols.
			}
			if (digit_size != 0 && !post_digit) {
				// add digit to _digits
				_digits.emplace_back(digit);
				// move to the symbol in _expr after this digit
				current += digit_size;
				digit_size = 0;
				post_digit = true;
			}
			else {
				if (!isspace(expr[current])) {
					// add symbol to _symbols
					_symbols.push_back(expr[current]);
					post_digit = false;
				}
				// move to next symbol in _expr
				++current;
			}
		}
		// check brackets' correct
		if (std::count(_symbols.cbegin(), _symbols.cend(), '(') != std::count(_symbols.cbegin(), _symbols.cend(), ')')) {
			throw std::runtime_error("! incorrect number of brackets");
		}
		prioritization(_symbols, _priorities);
		if (_symbols.empty()) {
			throw std::runtime_error("! no operations");
		}
	}

	void Expression::prioritization(std::list<char> &_symbols, std::list<unsigned> &_priorities) const {
		unsigned cur_pr = 1;	// priority of current processed operation
		// each bracket changes current priority on 2,
		// because we need to differ priority of {'+', '-'} and {'*', '/'}
		for (char c : _symbols) {
			switch (c) {
			case '(': {
						  cur_pr += 2;
						  break;
			}
			case ')': {
						  if (cur_pr >= 2) {
							  cur_pr -= 2;
						  }
						  else {
							  throw std::runtime_error("! wrong closing bracket");
						  }

						  break;
			}
			case '+':
			case '-': {
						  _priorities.emplace_back(cur_pr);
						  break;
			}
			case '*':
			case '/': {
						  _priorities.emplace_back(cur_pr + 1);
						  break;
			}
			default: {
						 throw std::runtime_error("! wrong symbol");
			}
			}
		}
		// delete brackets from _symbols
		auto del_iter = stable_partition(_symbols.begin(), _symbols.end(), is_oper);
		_symbols.erase(del_iter, _symbols.end());
	}

}	// end of expression_namespace

#endif
