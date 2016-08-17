/*
 Copyright Shuang Wu
 DO NOT COPY ANY PORTION OF THIS CODE. THANK YOU.
 */
#include "kind.h"
#include <sstream>
using std::string;
using std::istringstream;

// Use the annonymous namespace to prevent external linking
namespace {
  // Template function to convert from a string to some Type
  // Primarily used for string to integer conversions
  template <typename T>
  T fromString(const string& s, bool hex = false){
    istringstream iss(s);
    T n;
    if(hex)
      iss >> std::hex;
    if(iss >> n)
      return n;
    else
      throw string("ERROR: Type not convertible from string.");
  }
  // Strings representing the Kind of Token for printing
  const string kindStrings[] ={
		"ID",//ID
		"NUM",//NUM
		"LPAREN",//LPAREN
		"RPAREN",//RPAREN
		"LBRACE",//LBRACE
		"RBRACE",//RBRACE
		"RETURN",//RETURN
		"IF",//IF
		"ELSE",//ELSE
		"WHILE",//WHILE
		"PRINTLN",//PRINTLN
		"WAIN",//WAIN
		"BECOMES",//BECOMES
		"INT",//INT
		"EQ",//EQ
		"NE",//NE
		"LT",//LT
		"GT",//GT
		"LE",//LE
		"GE",//GE
		"PLUS",//PLUS
		"MINUS",//MINUS
		"STAR",//STAR
		"SLASH",//SLASH
		"PCT",//PCT
		"COMMA",//COMMA
		"SEMI",//SEMI
		"NEW",//NEW
		"DELETE",//DELETE
		"LBRACK",//LBRACK
		"RBRACK",//RBRACK
		"AMP",//AMP
		"NULL",//NULL
		"ERR",//ERR
  };
}

WLP4::Token::Token(WLP4::Kind kind, string lexeme)
: kind(kind), lexeme(lexeme){}

// Cannot convert Tokens that do not represent numbers
// to an Integer
int WLP4::Token::toInt() const{
  throw "ERROR: attempt to convert non-integer token: " + lexeme;
}

string WLP4::Token::toString() const{
  return ::kindStrings[kind];
}

string WLP4::Token::getLexeme() const{
  return lexeme;
}

WLP4::Kind WLP4::Token::getKind() const{
  return kind;
}

WLP4::IntToken::IntToken(WLP4::Kind kind, string lexeme)
: Token(kind,lexeme){}

// Ensure that the IntToken is a valid Integer for conversion
// Arguably, we could do this check in the constructor
int WLP4::IntToken::toInt() const{
  long long l = ::fromString<long long>(lexeme);
    if(l > 2147483647LL){
      throw "ERROR: constant out of range: " + lexeme;}
  return l;
}

// Print the pertinent information to the ostream
std::ostream& WLP4::operator<<(std::ostream& out, const WLP4::Token& t){
 if(t.getKind() == WLP4::NUM)
 {
	t.toInt();//check if out of range
 }
 out << t.toString() << " " << t.getLexeme();
 return out;
}

// Make a new Token depending on the Kind provided
// Integral Kinds correspond to the appropriate Token type
WLP4::Token* WLP4::Token::makeToken(WLP4::Kind kind, string lexeme){
  switch(kind){
    case NUM:
      return new IntToken(kind,lexeme);
    default:
      return new Token(kind,lexeme);
  }
}
/*
 Copyright Shuang Wu
 DO NOT COPY ANY PORTION OF THIS CODE. THANK YOU.
 */
