#ifndef __KIND_H__
#define __KIND_H__
#include <string>
#include <iostream>
namespace WLP4{
  // The different kinds of Tokens that are part of different WLP4 instructions
  // Used for determining the correct Token to create in the WLP4 recognizer
  enum Kind{
		ID,
		NUM,
		LPAREN,			//"("
		RPAREN,			//")"
		LBRACE,			//"{"
		RBRACE,			//"}"
		RETURN,			//the string "return" (in lower case)
		IF,		        //the string "if"
		ELSE,			// the string "else"
		WHILE,			// the string "while"
		PRINTLN,		// the string "println"
		WAIN,			//the string "wain"
		BECOMES,		// the string "="
		INT,			//the string "int"
		EQ,			//the string "=="
		NE,			//the string "!="
		LT,			//the string "<"
		GT,			//the string ">"
		LE,			//the string "<="
		GE,			//the string ">="
		PLUS,			//the string "+"
		MINUS,			// the string "-"
		STAR,			// the string "*"
		SLASH,			// the string "/"
		PCT,			// the string "%"
		COMMA,			// the string ","
		SEMI,			// the string ";"
		NEW,			// the string "new"
		DELETE,			// the string "delete"
		LBRACK,			// the string "["
		RBRACK,			// the string "]"
		AMP,			// the string "&"
		NULLSTATE,		// the string "NULL" 
		WHITESPACE,  		// Whitespace
		ERR         		// Bad/Invalid Token
  };

  // A Token class representing the concrete functions we
  // might want to apply to a WLP4 Token

  class Token{
  protected:
    // The kind of the Token
    Kind kind;
    // The actual string representing the Token
    std::string lexeme;
  public:
    // A Factory Method that allows us to make specific Tokens
    // based upon the Kind
    static Token* makeToken(Kind kind, std::string lexeme);
    Token(Kind kind, std::string lexeme);
    // Convenience functions for operations we might like to
    // use on a Token
    virtual int toInt() const;
    std::string toString() const;
    std::string getLexeme() const;
    Kind getKind() const;
  };

  // Subclass the Token class to allow specialized implementations
  // of the toInt method, since each type of token has different ranges
  // of valid values.

  // Token class for Integers
  class IntToken : public Token{
  public:
    IntToken(Kind kind, std::string lexeme);
    virtual int toInt() const;
  };
  
  // Overload the output operator for Tokens
  std::ostream& operator<<(std::ostream& out, const Token& t);
}
#endif
