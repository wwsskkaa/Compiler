/*
 Copyright Shuang Wu
 DO NOT COPY ANY PORTION OF THIS CODE. THANK YOU.
 */
#include <iostream>
#include <string>
#include <set>
#include <vector>
#include <sstream>
#include <map>
using std::vector;
using std::stringstream;
using std::map;
using std::pair;
using std::cerr;
using std::cin;
using std::endl;
using std::set;
using std::string;
using std::istream;
using std::ostream;

/*
Below are the global vars I created:
*/
int counter;
string procedurename; 
vector<string>cininput;//store the input line by line
map<string,pair<vector<string>, map<string,string> > > ST;
set<string> terminal;
string INT="int";
string INTSTAR="int*";

struct Tree
{
	vector<Tree> children;
	//vector of children trees
	vector<string> tokenvar;
	//splited token from breaking the derivation string 
};

/*
Below are the forward declaration of functions:
*/
void processID(Tree tree);
vector<string> argType(Tree tree);
string processFactor(Tree tree);
string processExpr(Tree tree);
string processLvalue(Tree tree);
void processStatements(Tree tree);
void processTest(Tree tree);
void processDsls(Tree tree);

/*
  buildTree:
  build up the tree from an preorder input stored in cininput vector 
*/
Tree buildTree()
{
	Tree tree;//construct a tree
	stringstream ss(cininput[counter]);
	string tt;
  while(ss>>tt)
  {
    tree.tokenvar.push_back(tt);
  }
	counter++;//need to increment counter so can go on
    string checktermi=tree.tokenvar[0];
    if(terminal.count(checktermi))
    {
	  //if it is a terminal, just return 
	  return tree;
    }
	//if not a terminal, recursively build tree for the non-terminal terms
	int derivationlength=tree.tokenvar.size()-1;
	for(int i=0;i<derivationlength;i++)
	{
		tree.children.push_back(buildTree());
	}
	return tree;
}

 /*
	returnType:
	identify the variable type and return it as a string
 */
string returnType(Tree t){
  string type;
  if(t.children[0].tokenvar.size()==2)//type → INT
  {
    type="int";
  }
  else if(t.children[0].tokenvar.size()==3)//type → INT STAR
  {
    type="int*";
  }
  else
  {
    string error="ERROR: can't identify the type";
	throw error;
  }
  return type;
}

 /*
	idType:
	It is just a function which return a pair of the label id and its type
	for the type part, it calls returnType as its helper function
 */
pair<string,string> idType(Tree t)
{
   pair<string,string> idtype;
   idtype.first=t.children[1].tokenvar[1];
   idtype.second=returnType(t);
   return idtype;
}
/*
 Copyright Shuang Wu
 DO NOT COPY ANY PORTION OF THIS CODE. THANK YOU.
 */

 /*
	RecursiveParam:
	A recursive function to handle paramlist → dcl COMMA paramlist
	recursively add the type of var in parameters to an vector.
 */
void RecursiveParam(Tree tree,vector<string> &params,int flag)
{
	if(tree.children.size()==3&&flag==0)
	{
	//when paramlist → dcl COMMA paramlist, keep recursing
      params.push_back(returnType(tree.children[0]));
	  RecursiveParam(tree.children[2],params,flag);
	}
	else if(tree.children.size()==1&&flag==0)
	{//for the last and the only one paramlist → dcl
	  flag=1;
      params.push_back(returnType(tree.children[0]));
	}
} 

vector<string> getParameters(Tree tree)
{
  int flag=0;//for handling the last paramlist.dcl
  vector<string> params;
  //if there are parameters
  if(tree.children.size()!=0 )
  {
	RecursiveParam(tree.children[0],params,flag);
  }
  return params;
}

/*
  buildST:
  Based on the tree constructed, build up SYMBOL TABLE
  Check types while building.
*/
void buildST(Tree tree)
{
  if(tree.tokenvar[0] == "main")
  {
  //main → INT WAIN LPAREN dcl COMMA dcl RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE 
	   procedurename="wain";//update current procedurename
		/*if(ST.find(procedurename)!=ST.end())
	    {
			string errormessage="ERROR: wain defined already";
			throw errormessage;
		}*/
	   vector<string> param;
	   param.push_back(returnType(tree.children[3]));//first parameter arg
	   string secondparam=returnType(tree.children[5]);
	    if(secondparam!="int")
	    {
			string errormessage="ERROR: second param has to be int";
			throw errormessage;
		}
		//if second parameter for wain is int, then push back.
	   param.push_back(secondparam);
		//create a symbol table for the procedure
		map<string,string> procedureST;
		//update wain entry in global ST		
		ST[procedurename].first=param;
		ST[procedurename].second=procedureST;
		processID(tree);//process the ID 
		processDsls(tree.children[8]);//check the dsls
		processStatements(tree.children[9]);//check the statements
		if(processExpr(tree.children[11])!="int")//check the expr type
		{
			string msg="ERROR: wain has to return int, but here isn't";
			throw msg;
		}
	}
	else if(tree.tokenvar[0]=="procedure")
	{//procedure->INT ID LPAREN params RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE
		procedurename=tree.children[1].tokenvar[1];
		//get the procedurename
		if(ST.find(procedurename)!=ST.end())
		//if procedure already exists in ST, cannot double define
	   {
			string errormessage1="ERROR: procedure defined already";
			throw errormessage1;
		}
		vector<string> param = getParameters(tree.children[3]);
		//go to parameter and get the ID declaration
		map<string,string> procedureST;
		//update procedure entry in global ST
		ST[procedurename].first=param;
		ST[procedurename].second=procedureST;
		processID(tree);
		processDsls(tree.children[6]);//check dsls
		processStatements(tree.children[7]);//check statements
		//The expr in the sequence directly derived from procedure must have type int.
		if(processExpr(tree.children[9])!="int")
		{
			string errormessage1="ERROR: procedure must have int expr";
			throw errormessage1;
		}
	}
	else//every other case, keep going.
	{
		for(vector<Tree>::iterator it = tree.children.begin();it!=tree.children.end();++it)
		{
		  buildST(*it);
		}
	}
}

/*
processTerm:
term → factor 
term → term STAR factor 
term → term SLASH factor 
term → term PCT factor 
The type of a term deriving factor is the same as the type of the derived factor.
The type of a term directly deriving anything other than just factor is int. 
The term and factor directly derived from such a term must have type int.
*/
string processTerm(Tree tree)
{
  if(tree.tokenvar[1]=="factor")
  {//term → factor
    return processFactor(tree.children[0]);
  }
  else
  {
  /*
  term → term STAR factor 
  term → term SLASH factor 
  term → term PCT factor
  */
    if(processTerm(tree.children[0])=="int*"||processFactor(tree.children[2])=="int*")
    {
	  string err="ERROR: can't be int*";
      throw err;
    }
    return INT;
  }
}
/*
 Copyright Shuang Wu
 DO NOT COPY ANY PORTION OF THIS CODE. THANK YOU.
 */
/*
processFactor:
factor → ID  
factor → NUM  
factor → NULL  
factor → LPAREN expr RPAREN  
factor → AMP lvalue
factor → STAR factor
factor → NEW INT LBRACK expr RBRACK
factor → ID LPAREN RPAREN
factor → ID LPAREN arglist RPAREN
The type of an ID is int if the dcl in which the ID is declared derives a sequence containing a type that derives INT.
The type of an ID is int* if the dcl in which the ID is declared derives a sequence containing a type that derives INT STAR.
The type of a NUM is int.
The type of a NULL token is int*.
The type of a factor deriving ID, NUM, or NULL is the same as the type of that token.
The type of a factor deriving LPAREN expr RPAREN is the same as the type of the expr.
The type of a factor deriving AMP lvalue is int*. The type of the derived lvalue (i.e. the one preceded by AMP) must be int.
The type of a factor or lvalue deriving STAR factor is int. The type of the derived factor (i.e. the one preceded by STAR) must be int*.
The type of a factor deriving NEW INT LBRACK expr RBRACK is int*. The type of the derived expr must be int.
The type of a factor deriving ID LPAREN RPAREN is int. The procedure whose name is ID must have an empty signature.
The type of a factor deriving ID LPAREN arglist RPAREN is int. The procedure whose name is ID must have a signature whose length is equal to the number of expr strings (separated by COMMA) that are derived from arglist. 
Further the types of these expr strings must exactly match, in order, the types in the procedure's signature.
*/
string processFactor(Tree tree)
{
  if(tree.tokenvar.size()==2&&tree.tokenvar[1]=="ID")
  {//factor → ID
    if(ST[procedurename].second.find(tree.children[0].tokenvar[1]) == ST[procedurename].second.end())
      {//search procedure's own symbol table for the id, if not exist, then ERROR
        throw "ERROR: Have not yet declared variable : "+ tree.children[0].tokenvar[1];
      }
	 //The type of a factor deriving ID, NUM, or NULL is the same as the type of that token.
    return ST[procedurename].second[tree.children[0].tokenvar[1]];
  }
  else if(tree.tokenvar[1]=="NUM")
  {//factor → NUM
    return INT;
  }
  else if(tree.tokenvar[1]=="NULL")
  {//factor → NULL
    return INTSTAR;
  }
  else if(tree.tokenvar[1]=="LPAREN")
  {//factor → LPAREN expr RPAREN
    return processExpr(tree.children[1]);
  }
  else if(tree.tokenvar[1]=="AMP")
  {//factor → AMP lvalue
    if(processLvalue(tree.children[1])=="int")
    {
		return INTSTAR;
    }
    else
	{
	    string err="ERROR: should be int";
		throw err;
	}
  }
  else if(tree.tokenvar[1]=="STAR")
  //factor → STAR factor
  {
    if(processFactor(tree.children[1])=="int*")
      {return INT;}
    else
      {
	  	    string err="ERROR: need a int*?";
			throw err;
	  }
  }
  else if(tree.tokenvar[1]=="NEW")
  {//factor → NEW INT LBRACK expr RBRACK
    if(processExpr(tree.children[3])=="int")
    {
      return INTSTAR;
    }
    else
    {
	  	string err="ERROR: need int";
		throw err;
	}
  }
  else if(tree.tokenvar.size()==4&&tree.tokenvar[1]=="ID")
  // factor → ID LPAREN RPAREN, nothing in the () 
  {
	  if(ST[procedurename].second.find(tree.children[0].tokenvar[1]) != ST[procedurename].second.end())
      {
	    string err="ERROR: var and function same name";
		throw err;
      }
      if(ST.find(tree.children[0].tokenvar[1])==ST.end())
      {//search global symbol table for the procedure name, if not exist, then ERROR
        throw "ERROR: Have not yet declared procedure: " +  tree.children[0].tokenvar[1];
      }
	  if(ST[tree.children[0].tokenvar[1]].first.size()!=0)
	  {
	     throw "ERROR: wrong amount of args: " + tree.children[0].tokenvar[1];
	 } 
  }
  else if ( tree.tokenvar.size()==5&&tree.tokenvar[1]=="ID") 
  //factor → ID LPAREN arglist RPAREN,taking param
  {
	 if(ST[procedurename].second.find(tree.children[0].tokenvar[1]) != ST[procedurename].second.end())
      {
	    string err="ERROR: var and function same name";
		throw err;
      }
      if(ST.find(tree.children[0].tokenvar[1])==ST.end())
      {////search global symbol table for the procedure name, if not exist, then ERROR
        throw "ERROR: Have not yet declared procedure: " +  tree.children[0].tokenvar[1];
      }
    vector<string> args= argType(tree.children[2]);

    if(args.size()!=ST[tree.children[0].tokenvar[1]].first.size())
    {
	     throw "ERROR: wrong amount of args: " + tree.children[0].tokenvar[1];   
    }

    for(int i=0;i<args.size();i++)
    {
      if(args[i]!= ST[tree.children[0].tokenvar[1]].first[i])
	  {
	    string errormsg="ERROR: parsed-in args have different types from required parameter types";
        throw errormsg;
	  }
    }
  }
  return INT;
}
/*
 Copyright Shuang Wu
 DO NOT COPY ANY PORTION OF THIS CODE. THANK YOU.
 */
/*
processExpr:
expr → term 
expr → expr PLUS term 
expr → expr MINUS term
When expr derives expr PLUS term:
The derived expr and the derived term may both have type int, in which case the type of the expr deriving them is int.
The derived expr may have type int* and the derived term may have type int, in which case the type of the expr deriving them is int*.
The derived expr may have type int and the derived term may have type int*, in which case the type of the expr deriving them is int*.
When expr derives expr MINUS term:
The derived expr and the derived term may both have type int, in which case the type of the expr deriving them is int.
The derived expr may have type int* and the derived term may have type int, in which case the type of the expr deriving them is int*.
The derived expr and the derived term may both have type int*, in which case the type of the expr deriving them is int. 
*/
string processExpr(Tree tree)
{
  if(tree.tokenvar[1]=="term")
  {//expr → term 
    string t=processTerm(tree.children[0]);
    return t;
  }
  else if(tree.tokenvar[2]=="PLUS")
  {
	//expr → expr PLUS term 
    string firsttype = processExpr(tree.children[0]);
    string secondtype = processTerm(tree.children[2]);
      if(firsttype=="int" && secondtype =="int")
	  {return INT;}
      else if (firsttype=="int*" && secondtype=="int")
	  {return INTSTAR;}
      else if (firsttype=="int"&&secondtype=="int*")
	  {return INTSTAR;}
      else
	  {throw "ERROR: invalid PLUS with firsttype "+firsttype+" and secondtype "+secondtype;}
  }
  else if (tree.tokenvar[2]=="MINUS")
   {
	//expr → expr MINUS term
    string firsttype = processExpr(tree.children[0]);
    string secondtype = processTerm(tree.children[2]);
	if(firsttype=="int"&&secondtype=="int")
	  {return INT;}
      else if (firsttype=="int*"&&secondtype=="int")
	  {return INTSTAR;}
      else if (firsttype=="int*"&&secondtype=="int*")
	  {return INT;}
	  else
	  {throw "ERROR: invalid MINUS with firsttype "+firsttype+" and secondtype "+secondtype;}
   }
   else
   {
		string msg="ERROR: mistaking mode when processing expr";
		throw msg;
   }
  }

/*
processLvalue:
lvalue → ID  
lvalue → STAR factor
lvalue → LPAREN lvalue RPAREN 
The type of an lvalue deriving ID is the same as the type of that ID.
The type of an lvalue deriving LPAREN lvalue RPAREN is the same as the type of the derived lvalue.
*/
string processLvalue(Tree tree)
{
  if(tree.tokenvar[1]=="ID")
  // value → ID  
  {
	string t=ST[procedurename].second[tree.children[0].tokenvar[1]];
    return t;
  }
  else if (tree.tokenvar[1]=="STAR")
  //lvalue → STAR factor
  {
    if(processFactor(tree.children[1])=="int*")
    {
      return INT;
    }
    else
    {
	    string err="ERROR: should be int*";
		throw err;
	}
  }
  else if (tree.tokenvar[1]=="LPAREN")
  // lvalue → LPAREN lvalue RPAREN 
  {
	string f=processLvalue(tree.children[1]);
    return f;
  }
  else
  {
		string msg="ERROR: mistaking mode when processing lvalue";
		throw msg;
  }
}

/*
processDsls:
dcls → 
dcls → dcls dcl BECOMES NUM SEMI
dcls → dcls dcl BECOMES NULL SEMI
When dcls derives dcls dcl BECOMES NUM SEMI, the derived dcl must derive a sequence containing a type that derives INT.
When dcls derives dcls dcl BECOMES NULL SEMI, the derived dcl must derive a sequence containing a type that derives INT STAR.
*/
void processDsls(Tree tree)
{
  if(tree.tokenvar.size()==1)
  {//dcls →     then do nothing
    return;
  }
  processDsls(tree.children[0]);
  string t=returnType(tree.children[1]);
  if(tree.tokenvar[4]=="NUM")
  {//dcls → dcls dcl BECOMES NUM SEMI
    if(t!="int")
    {
      throw "ERROR: dcls NUM must be int but is a "+t;
    }
  }
  else if(tree.tokenvar[4]=="NULL")
  {//dcls → dcls dcl BECOMES NULL SEMI
    if(t!="int*")
    {
      throw "ERROR: dcls NULL must be int* but is a "+t;
    }
  }
}

/*
processTest:
Whenever test directly derives a sequence containing two exprs, they must both have the same type.
test → expr EQ expr  
test → expr NE expr  
test → expr LT expr 
test → expr LE expr  
test → expr GE expr  
test → expr GT expr 
*/
void processTest(Tree tree)
{
    string firsttype = processExpr(tree.children[0]);
    string secondtype = processExpr(tree.children[2]);
	if(firsttype!=secondtype)
	{
		throw "ERROR: different expr can't test: "+firsttype+" and "+secondtype;
	}
}

/*
processStatements:
statements →
statements → statements statement  
statement → lvalue BECOMES expr SEMI
statement → IF LPAREN test RPAREN LBRACE statements RBRACE ELSE LBRACE statements RBRACE 
statement → WHILE LPAREN test RPAREN LBRACE statements RBRACE 
statement → PRINTLN LPAREN expr RPAREN SEMI
statement → DELETE LBRACK RBRACK expr SEMI
When statement derives lvalue BECOMES expr SEMI, the derived lvalue and the derived expr must have the same type.
When statement derives PRINTLN LPAREN expr RPAREN SEMI, the derived expr must have type int.
When statement derives DELETE LBRACK RBRACK expr SEMI, the derived expr must have type int*.
*/
void processStatements(Tree tree)
{
	if(tree.tokenvar.size()==1) 
	{//statements →
		return;
	}
	if(tree.tokenvar[1]=="statements")
  {
  //statements → statements statement
    processStatements(tree.children[0]);
    processStatements(tree.children[1]);
  }
  else if(tree.tokenvar[1]=="lvalue")
  {
  //statement → lvalue BECOMES expr SEMI
    if(processLvalue(tree.children[0]) != processExpr(tree.children[2]))
    {string msg="ERROR: lvalue and the derived expr must have the same type";
	throw msg;}
  }
  else if(tree.tokenvar[1]=="IF")
  {//statement → IF LPAREN test RPAREN LBRACE statements RBRACE ELSE LBRACE statements RBRACE
    processTest(tree.children[2]);
    processStatements(tree.children[5]);
    processStatements(tree.children[9]);
  }
  else if(tree.tokenvar[1]=="WHILE")
  {//statement → WHILE LPAREN test RPAREN LBRACE statements RBRACE 
    processTest(tree.children[2]);
    processStatements(tree.children[5]);
  }
  else if(tree.tokenvar[1]=="PRINTLN")
  //statement → PRINTLN LPAREN expr RPAREN SEMI
  {
	string et=processExpr(tree.children[2]);
    if(et!="int")
	{
      string msg="ERROR: derived expr must have type int";
	  throw msg;}
  }
  else if(tree.tokenvar[1]=="DELETE")
  //statement → DELETE LBRACK RBRACK expr SEMI
  {	   string et=processExpr(tree.children[3]);
	  if(et!="int*")
      {
      string msg="ERROR: derived expr must have type int*";
	  throw msg;
	  }
  }
}
/*
 Copyright Shuang Wu
 DO NOT COPY ANY PORTION OF THIS CODE. THANK YOU.
 */
/*
argType: 
return a string vector of all the args parsed in when calling a procedure
*/
vector<string> argType(Tree tree)
{  
  vector<string> args;
  if(tree.tokenvar.size()==2)
  {  //arglist → expr
     args.push_back( processExpr(tree.children[0]));
  }
  else if (tree.tokenvar.size()==4)
  {
     //arglist → expr COMMA arglist
    args.push_back(processExpr(tree.children[0]));
    vector<string> furtherarg = argType(tree.children[2]);
    //need to recursively call this method to get all the args
    for(vector<string>::iterator it=furtherarg.begin();it!=furtherarg.end();it++)
    {//need to add the sub vector content to the main vector
		args.push_back(*it);
    }
  }
  else
  {
    string err="ERROR: things go wrong when deriving args from procedure calling";
    throw err;
  }
  return args;
}


/*
processID:
check and process ID to the global Symbol Table.
*/
void processID(Tree tree)
{
  if(tree.tokenvar[0]=="dcl")
  {//dcl → type ID
  string id=idType(tree).first;
  if(ST[procedurename].second.find(id)!=ST[procedurename].second.end())
  {//if id in symbol table.ERROR
	throw "ERROR : symbol \""+id +"\" already exists";
  }
  else
  {  //if id not in symbol table. update it with its type
	ST[procedurename].second[id] = returnType(tree);
  }
    return;
  }
  else if(tree.tokenvar[0]=="lvalue")
  {
  /*
	lvalue → ID  
	lvalue → STAR factor
	lvalue → LPAREN lvalue RPAREN 
  */
     if(tree.tokenvar.size()==2&&tree.tokenvar[1]=="ID")
	 {//  lvalue → ID  
      if(ST[procedurename].second.find(tree.children[0].tokenvar[1]) == ST[procedurename].second.end())
      {//search procedure's own symbol table for the id, if not exist, then ERROR
        throw "ERROR: Have not yet declared variable : "+tree.children[0].tokenvar[1];
      }
	 }
     else if(tree.tokenvar.size()==3)
	 {//lvalue → STAR factor
		processID(tree.children[1]);
	 }
     else if(tree.tokenvar.size()==4)
	 {//lvalue → LPAREN lvalue RPAREN 
		processID(tree.children[1]);
	 }	 
  }
  else if((tree.tokenvar[0]=="factor"))
  {
	//What I wrote previously was really redundant, so I pulled everything out to make serparate methods.
		processFactor(tree);
  }
  else
  {
    //iterate through children for ID
    for(vector<Tree>::iterator it=tree.children.begin();it!=tree.children.end();it++)
    {
      processID(*it);
    }
  }
}


int main() 
{
	try{
		string TER[] = {"BOF",
						"BECOMES",
						"COMMA",
						"ELSE",
						"EOF",
						"EQ",
						"GE",
						"GT",
						"ID",
						"IF",
						"INT",
						"LBRACE",
						"LE",
						"LPAREN",
						"LT", 
						"MINUS", 
						"NE", 
						"NUM", 
						"PCT",
						"PLUS",
						"PRINTLN", 
						"RBRACE", 
						"RETURN",
						"RPAREN",
						"SEMI", 
						"SLASH", 
						"STAR", 
						"WAIN", 
						"WHILE", 
						"AMP", 
						"LBRACK", 
						"RBRACK", 
						"NEW",
						"DELETE", 
						"NULL"};
		for (int i =0;i<35;i++) 
		{//insert them into the set which makes checking easier
			terminal.insert(TER[i]);
		}
		/*
		first, process the input and store them into a vector
		*/
		string tempinput;
		while(getline(cin,tempinput))
		{
			cininput.push_back(tempinput);
		}
		Tree t=buildTree();//construct the tree from the input vector
		buildST(t);//construct the symbol table from the tree
	}catch(string &errormessage)
	{
		cerr<<errormessage<<endl;
    }
}	


/*
 Copyright Shuang Wu
 DO NOT COPY ANY PORTION OF THIS CODE. THANK YOU.
 */
