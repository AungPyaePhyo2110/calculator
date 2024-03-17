#include <vector>
#include <iostream>
#include <memory>

using namespace std;

class Token {
public:
	enum class Type {
		NUMBER,
		LPAR,
		RPAR,
		PLUS,
		MINUS,
		MUL,
		DIV,
		END,
		ERROR
	};

	std::string m_payload;
	Type m_type;

	Token ( std::string payload, Type type ) : m_payload ( payload ), m_type ( type ) { }
};

ostream & operator << ( ostream & os, const Token & token ) {
	switch ( token.m_type ) {
		case Token::Type::NUMBER:
			os << "NUMBER"; break;
		case Token::Type::LPAR:
			os << "LPAR"; break;
		case Token::Type::RPAR:
			os << "RPAR"; break;
		case Token::Type::PLUS:
			os << "PLUS"; break;
		case Token::Type::MINUS:
			os << "MINUS"; break;
		case Token::Type::MUL:
			os << "MUL"; break;
		case Token::Type::DIV:
			os << "DIV"; break;
		case Token::Type::END:
			os << "END"; break;
		case Token::Type::ERROR:
			os << "ERROR"; break;
	};
	return os << " " << token.m_payload;
}

Token nextToken ( istream & is ) {
	int c = is.get ( );
	while ( std::isspace ( c ) ) {
		c = is.get ( );
	}

	switch ( c ) {
	case EOF:
		return Token ( "", Token::Type::END );
	case '+':
		return Token ( "+", Token::Type::PLUS );
	case '-':
		return Token ( "-", Token::Type::MINUS );
	case '*':
		return Token ( "*", Token::Type::MUL );
	case '/':
		return Token ( "/", Token::Type::DIV );
	case '(':
		return Token ( "(", Token::Type::LPAR );
	case ')':
		return Token ( ")", Token::Type::RPAR );
	default:
		if ( c >= '0' && c <= '9' ) {
			std::string number;
			while ( c >= '0' && c <= '9' ) {
				number += c;
				c = is.get ( );
			}
			is.unget();
			return Token ( number, Token::Type::NUMBER );
		} else {
			return Token ( "", Token::Type::ERROR );
		}
	}
}

vector < Token > tokenize ( istream & is ) {
	vector < Token > res;
	res.push_back ( nextToken ( is ) );
	while ( res.back ( ).m_type != Token::Type::END ) {
		res.push_back ( nextToken ( is ) );
	}
	return res;
}

class Node {
public:
	virtual ~Node ( ) noexcept = default;
	virtual int eval ( ) const = 0;
};

class Number : public Node {
	int m_value;
public:
	Number ( int value ) : m_value ( value ) {}
	int eval ( ) const override {
		return m_value;
	}
};

class Add : public Node {
	unique_ptr < Node > m_left;
	unique_ptr < Node > m_right;
public:
	Add ( unique_ptr < Node > left, unique_ptr < Node > right ) : m_left ( move ( left ) ), m_right ( move ( right ) ) {}
	int eval ( ) const override {
		return m_left->eval ( ) + m_right->eval ( );
	}
};

class Sub : public Node {
	unique_ptr < Node > m_left;
	unique_ptr < Node > m_right;
public:
	Sub ( unique_ptr < Node > left, unique_ptr < Node > right ) : m_left ( move ( left ) ), m_right ( move ( right ) ) {}
	int eval ( ) const override {
		return m_left->eval ( ) - m_right->eval ( );
	}
};

class Mul : public Node {
	unique_ptr < Node > m_left;
	unique_ptr < Node > m_right;
public:
	Mul ( unique_ptr < Node > left, unique_ptr < Node > right ) : m_left ( move ( left ) ), m_right ( move ( right ) ) {}
	int eval ( ) const override {
		return m_left->eval ( ) * m_right->eval ( );
	}
};

class Div : public Node {
	unique_ptr < Node > m_left;
	unique_ptr < Node > m_right;
public:
	Div ( unique_ptr < Node > left, unique_ptr < Node > right ) : m_left ( move ( left ) ), m_right ( move ( right ) ) {}
	int eval ( ) const override {
		return m_left->eval ( ) / m_right->eval ( );
	}
};

unique_ptr < Node > term ( vector < Token >::iterator & tokens );
unique_ptr < Node > factor ( vector < Token >::iterator & tokens );

unique_ptr < Node > expression ( vector < Token >::iterator & tokens ) {
	unique_ptr < Node > node = term ( tokens );
	while ( tokens->m_type == Token::Type::PLUS || tokens->m_type == Token::Type::MINUS ) {
		if ( tokens->m_type == Token::Type::PLUS ) {
			tokens++;
			unique_ptr < Node > other = term ( tokens );
			node = make_unique < Add > ( move ( node ), move ( other ) );
		} else { // MINUS
			tokens++;
			unique_ptr < Node > other = term ( tokens );
			node = make_unique < Sub > ( move ( node ), move ( other ) );
		}
	}
	return node;
}

unique_ptr < Node > term ( vector < Token >::iterator & tokens ) {
	unique_ptr < Node > node = factor ( tokens );
	while ( tokens->m_type == Token::Type::MUL || tokens->m_type == Token::Type::DIV ) {
		if ( tokens->m_type == Token::Type::MUL ) {
			tokens++;
			unique_ptr < Node > other = factor ( tokens );
			node = make_unique < Mul > ( move ( node ), move ( other ) );
		} else { // DIV
			tokens++;
			unique_ptr < Node > other = factor ( tokens );
			node = make_unique < Div > ( move ( node ), move ( other ) );
		}
	}
	return node;
}

unique_ptr < Node > factor ( vector < Token >::iterator & tokens ) {
	if ( tokens->m_type == Token::Type::NUMBER ) {
		int val = stoi ( tokens->m_payload );
		tokens++;
		return make_unique < Number > ( val );
	} else if ( tokens->m_type == Token::Type::LPAR ) {
		tokens++;
		unique_ptr < Node > node = expression ( tokens );
		if ( tokens->m_type != Token::Type::RPAR )
			throw std::logic_error ( "Unmatched parenthesis." );
		tokens++;
		return node;
	} else {
		throw std::logic_error ( "Expression error." );
	}
}

int main ( ) {
	vector < Token > tokens = tokenize ( cin );
	/*for ( const Token & token : tokens ) {
		cout << token << endl;
	}*/
	auto tokensIterator = tokens.begin ( );
	unique_ptr < Node > root = expression ( tokensIterator );
	cout << root->eval ( ) << endl;
}
