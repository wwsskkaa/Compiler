/*
 Copyright Shuang Wu
 DO NOT COPY ANY PORTION OF THIS CODE. THANK YOU.
 */
#include "lexer.h"
#include "kind.h"
#include <sstream>
using std::string;
using std::vector;
using std::istringstream;
// Use the annonymous namespace to prevent external linking
namespace {
  // An array represents the Token kind that each state represents
  // The ERR Kind represents an invalid token
  WLP4::Kind stateKinds[] = {
	WLP4::ERR,   			         // ST_ERR
	WLP4::NUM,				// ST_ZERO,
	WLP4::ERR,				// ST_ZEROSTART,
	WLP4::ERR,				// ST_START
	WLP4::ID,				// ST_ID,
	WLP4::NUM,				// ST_NUM,
	WLP4::ERR,				// ST_NUMSTART,
	WLP4::LPAREN,				// ST_LPAREN,
	WLP4::RPAREN,				// ST_RPAREN,
	WLP4::LBRACE,				// ST_LBRACE,
	WLP4::RBRACE,				// ST_RBRACE,
	WLP4::RETURN,				// ST_RETURN,
	WLP4::IF,				// ST_IF,
	WLP4::ELSE,				// ST_ELSE,
	WLP4::WHILE,				 //ST_WHILE,
	WLP4::PRINTLN,				 // ST_PRINTLN,
	WLP4::WAIN,				 //ST_WAIN,
	WLP4::BECOMES,				 // ST_BECOMES,
	WLP4::INT,				 //ST_INT,
	WLP4::EQ,				 // ST_EQ,
	WLP4::ERR,				 //ST_NOT,
	WLP4::NE,				 //ST_NE,
	WLP4::LT,				 //ST_LT,
	WLP4::GT,				 //ST_GT,
	WLP4::LE,				 //ST_LE,
	WLP4::GE,				 //ST_GE,
	WLP4::PLUS,				 //ST_PLUS,
	WLP4::MINUS,				 //ST_MINUS,
	WLP4::STAR,				 //ST_STAR,
	WLP4::SLASH,				 // ST_SLASH,
	WLP4::PCT,				 //ST_PCT,
	WLP4::COMMA,				 //ST_COMMA,
	WLP4::SEMI,				 //ST_SEMI,
	WLP4::NEW,				 // ST_NEW,
	WLP4::DELETE,				 //ST_DELETE,
	WLP4::LBRACK,				 //ST_LBRACK,
	WLP4::RBRACK,				 //ST_RBRACK,
	WLP4::AMP,				 //ST_AMP,
	WLP4::NULLSTATE,			 //ST_NULL,
	WLP4::WHITESPACE,			 //ST_COMMENT
	WLP4::WHITESPACE			 // ST_WHITESPACE
  };
  const string whitespace = "\t\n\r ";
  const string letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
  const string digits = "0123456789";
  const string oneToNine =  "123456789";

}


WLP4::Lexer::Lexer(){
  // Set default transitions to the Error state
  for(int i=0; i < maxStates; ++i){
    for(int j=0; j < maxTrans; ++j){
      delta[i][j] = ST_ERR;
    }
  }
  // Change transitions as appropriate for the MIPS recognizer
  
  
  setTrans( ST_START,      whitespace,     ST_WHITESPACE );
  setTrans( ST_WHITESPACE, whitespace,     ST_WHITESPACE );
  setTrans( ST_START,      letters,        ST_ID         );
  setTrans( ST_ID,         letters+digits, ST_ID         );
  setTrans( ST_START,      oneToNine,      ST_NUM        );
  setTrans( ST_START,       "0",           ST_ZERO       );
  setTrans( ST_ZERO,       letters+digits, ST_ZEROSTART  );
  setTrans( ST_NUM,        digits,         ST_NUM        );
  setTrans(ST_NUM,        letters,        ST_NUMSTART    );
  setTrans( ST_START,      "(",            ST_LPAREN     );
  setTrans( ST_START,      ")",            ST_RPAREN     );
  setTrans( ST_START,      "{",            ST_LBRACE     );
  setTrans( ST_START,      "}",            ST_RBRACE     );
   setTrans(ST_START,      "=",            ST_BECOMES   );
   setTrans(ST_BECOMES,    "=",            ST_EQ        );
   setTrans(ST_START,      "!",            ST_NOT        );
   setTrans(ST_NOT,         "=",            ST_NE        );
   setTrans(ST_START,      "<",            ST_LT        );
   setTrans(ST_LT,         "=",            ST_LE        );
   setTrans(ST_START,      ">",            ST_GT        );
   setTrans(ST_GT,         "=",            ST_GE        );
   setTrans(ST_START,      "+",            ST_PLUS      );
   setTrans(ST_START,      "-",            ST_MINUS     );
   setTrans(ST_START,      "*",            ST_STAR      );
   setTrans(ST_START,      "/",            ST_SLASH     );
   setTrans(ST_START,      "%",            ST_PCT       );
   setTrans(ST_START,      ",",            ST_COMMA     );
   setTrans(ST_START,      ";",            ST_SEMI      );
   setTrans(ST_START,      "[",            ST_LBRACK    );
   setTrans(ST_START,      "]",            ST_RBRACK    );
   setTrans(ST_START,      "&",            ST_AMP       );
   setTrans(ST_SLASH,      "/",            ST_COMMENT   );
  // A comment can only ever lead to the comment state
  for(int j=0; j < maxTrans; ++j) delta[ST_COMMENT][j] = ST_COMMENT;
}

  
// Set the transitions from one state to another state based upon characters in the
// given string
void WLP4::Lexer::setTrans(WLP4::State from, const string& chars, WLP4::State to){
  for(string::const_iterator it = chars.begin(); it != chars.end(); ++it)
    delta[from][static_cast<unsigned int>(*it)] = to;
}

// Scan a line of input (as a string) and return a vector
// of Tokens representing the MIPS instruction in that line
vector<WLP4::Token*> WLP4::Lexer::scan(const string& line){
  // Return vector
  vector<Token*> ret;
  if(line.size() == 0) return ret;
  // Always begin at the start state
  State currState = ST_START;
  // startIter represents the beginning of the next Token
  // that is to be recognized. Initially, this is the beginning
  // of the line.
  // Use a const_iterator since we cannot change the input line
  string::const_iterator startIter = line.begin();
  // Loop over the the line
  for(string::const_iterator it = line.begin();;){
    // Assume the next state is the error state
    State nextState = ST_ERR;
    // If we aren't done then get the transition from the current
    // state to the next state based upon the current character of
    //input
    if(it != line.end())
      nextState = delta[currState][static_cast<unsigned int>(*it)];
    // If there is no valid transition then we have reach then end of a
    // Token and can add a new Token to the return vector
    if(ST_ERR == nextState){
      // Get the kind corresponding to the current state
      Kind currKind = stateKinds[currState];
      // If we are in an Error state then we have reached an invalid
      // Token - so we throw and error and delete the Tokens parsed
      // thus far
      if(ERR == currKind){
        vector<Token*>::iterator vit;
        for(vit = ret.begin(); vit != ret.end(); ++vit)
          delete *vit;
        throw "ERROR in lexing after reading " + string(line.begin(),it);
      }

      // If we are not in Whitespace then we push back a new token
      // based upon the kind of the State we end in
      // Whitespace is ignored for practical purposes
      if(WHITESPACE!=currKind)
      {
	string currlexeme = string(startIter,it);
        if(currlexeme=="wain")
	{
          currKind = stateKinds[ST_WAIN];
	}
	else if(currlexeme=="int")
	{
          currKind = stateKinds[ST_INT];
	}
	else if(currlexeme == "if")
	{         
	  currKind = stateKinds[ST_IF];
        }
	else if(currlexeme == "else")
	{
          currKind= stateKinds[ST_ELSE];
        }
	else if(currlexeme == "while")
	{
          currKind = stateKinds[ST_WHILE];
	}
	else if(currlexeme == "println")
	{
          currKind = stateKinds[ST_PRINTLN] ;
	}
	else if(currlexeme == "return")
	{
          currKind = stateKinds[ST_RETURN];
        }
	else if(currlexeme == "NULL")
	{
          currKind = stateKinds[ST_NULL];
        }
	else if(currlexeme == "new")
	{
          currKind = stateKinds[ST_NEW];
        }
	else if(currlexeme == "delete")
	{
          currKind = stateKinds[ST_DELETE];
        }
        ret.push_back(Token::makeToken(currKind,currlexeme));
	  }
      // Start of next Token begins here
      startIter = it;
      // Revert to start state to begin recognizing next token
      currState = ST_START;
      if(it == line.end()) break;
    } else {
      // Otherwise we proceed to the next state and increment the iterator
      currState = nextState;
      ++it;
    }
  }
  return ret;
}
/*
 Copyright Shuang Wu
 DO NOT COPY ANY PORTION OF THIS CODE. THANK YOU.
 */