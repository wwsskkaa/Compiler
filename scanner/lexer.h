#ifndef __LEXER_H__
#define __LEXER_H__
#include <string>
#include <sstream>
#include <vector>

namespace WLP4{
  // The different states the the WLP4 recognizer uses
  // Judicious use of the pimpl idiom or the Bridge pattern
  // would allow us to place this in the implementation file
  // However, that's more complexity than is necessary
 
  enum State {
	 ST_ERR,
	 ST_ZERO,
	 ST_ZEROSTART,
	 ST_START,
	 ST_ID,
	 ST_NUM,
	 ST_NUMSTART,
	 ST_LPAREN,
	 ST_RPAREN,
	 ST_LBRACE,
	 ST_RBRACE,
	 ST_RETURN,
	 ST_IF,
	 ST_ELSE,
	 ST_WHILE,
	 ST_PRINTLN,
	 ST_WAIN,
	 ST_BECOMES,
	 ST_INT,
	 ST_EQ,
	 ST_NOT,
	 ST_NE,
	 ST_LT,
	 ST_GT,
	 ST_LE,
	 ST_GE,
	 ST_PLUS,
	 ST_MINUS,
	 ST_STAR,
	 ST_SLASH,
	 ST_PCT,
	 ST_COMMA,
	 ST_SEMI,
	 ST_NEW,
	 ST_DELETE,
	 ST_LBRACK,
	 ST_RBRACK,
	 ST_AMP,
	 ST_NULL,
	 ST_COMMENT,
	 ST_WHITESPACE
  };
  
  // Forward declare the Token class to reduce compilation dependencies
  class Token;

  // Class representing a WLP4 recognizer
  class Lexer {
    // At most 21 states and 256 transitions (max number of characters in ASCII)
    static const int maxStates = 41;
    static const int maxTrans = 256;
    // Transition function
    State delta[maxStates][maxTrans];
    // Private method to set the transitions based upon characters in the
    // given string
    void setTrans(State from, const std::string& chars, State to);
  public:
    Lexer();
    // Output a vector of Tokens representing the Tokens present in the
    // given line
    std::vector<Token*> scan(const std::string& line);
  };
}

#endif
