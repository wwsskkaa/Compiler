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
using std::cout;
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
map<string,pair<int, map<string, pair<string, int> > > > ST;
set<string> terminal;
string INT="int";
string INTSTAR="int*";
int offset=0;
int lc=0;
int dc=0;
int Ic=0;

struct Tree
{
	vector<Tree*> children;
	//vector of children tree pointers
	vector<string> tokenvar;
	//splited token from breaking the derivation string 
};

/*
Below are the forward declaration of functions:
*/
void RecursiveParam(Tree* tree,vector<string> &params,int flag);
vector<string> getParameters(Tree* tree);
string returnType(Tree* t);
void processStatements(Tree* tree);
void processTest(Tree* tree);
void processDsls(Tree* tree);
string processFactor(Tree* tree);
string processExpr(Tree* tree);
string processLvalue(Tree* tree);
string processTerm(Tree* tree);
void processID(Tree* tree);
/*
Above is the A8 stuff
Below is the A9A10 stuff
*/
void dclsASM(Tree* tree);
void statementsASM(Tree* tree);
void statementASM(Tree* tree);
void exprASM(Tree* tree);
void termASM(Tree* tree);
void factorASM(Tree* tree);
void lvalueASM(Tree* tree);
void arglistASM(Tree* tree);
void testASM(Tree* tree);

 /*
	RecursiveParam:
	A recursive function to handle paramlist ->dcl COMMA paramlist
	recursively add the type of var in parameters to an vector.
 */
void RecursiveParam(Tree* tree,vector<string> &params,int flag)
{
	if(tree->children.size()==3&&flag==0)
	{
	//when paramlist ->dcl COMMA paramlist, keep recursing
      params.push_back(returnType(tree->children[0]));
	  RecursiveParam(tree->children[2],params,flag);
	}
	else if(tree->children.size()==1&&flag==0)
	{//for the last and the only one paramlist ->dcl
	  flag=1;
      params.push_back(returnType(tree->children[0]));
	}
} 

 /*
	getParameters:
	call RecursiveParam to return a vector of parameter types
	mainly for A8
	*/
vector<string> getParameters(Tree* tree)
{
  int flag=0;//for handling the last paramlist.dcl
  vector<string> params;
  //if there are parameters
  if(tree->children.size()!=0 )
  {
	RecursiveParam(tree->children[0],params,flag);
  }
  return params;
}

/*
There are some useless checks and throws from A8 which I just simply deleted them for running speed.
*/
void processStatements(Tree* tree)
{
	if(tree->tokenvar.size()==1) 
	{//statements ->
	}
	if(tree->tokenvar[1]=="statements")
	{
		//statements ->statements statement
		processStatements(tree->children[0]);
		processStatements(tree->children[1]);
	 }
	else if(tree->tokenvar[1]=="lvalue")
	{
		//statement ->lvalue BECOMES expr SEMI
	}
	else if(tree->tokenvar[1]=="IF")
	{//statement ->IF LPAREN test RPAREN LBRACE statements RBRACE ELSE LBRACE statements RBRACE
		processTest(tree->children[2]);
		processStatements(tree->children[5]);
		processStatements(tree->children[9]);
	}
	else if(tree->tokenvar[1]=="WHILE")
	{//statement ->WHILE LPAREN test RPAREN LBRACE statements RBRACE 
		processTest(tree->children[2]);
		processStatements(tree->children[5]);
	}
	else if(tree->tokenvar[1]=="PRINTLN")
	//statement ->PRINTLN LPAREN expr RPAREN SEMI
	{}
	else if(tree->tokenvar[1]=="DELETE")
	//statement ->DELETE LBRACK RBRACK expr SEMI
	{}
}
/*
 Copyright Shuang Wu 20640582
 DO NOT COPY ANY PORTION OF THIS CODE. THANK YOU.
 */
void processDsls(Tree* tree)
{
  if(tree->tokenvar.size()==1)
  {//dcls ->    then do nothing
    return;
  }
  processDsls(tree->children[0]);
}

void processTest(Tree* tree)
{
    string firsttype = processExpr(tree->children[0]);
    string secondtype = processExpr(tree->children[2]);
}

string processFactor(Tree* tree)
{
  if(tree->tokenvar.size()==2&&tree->tokenvar[1]=="ID")
  {//factor ->ID
	 //The type of a factor deriving ID, NUM, or NULL is the same as the type of that token.
    return ST[procedurename].second[tree->children[0]->tokenvar[1]].first;
  }
  else if(tree->tokenvar[1]=="NUM")
  {//factor ->NUM
    return INT;
  }
  else if(tree->tokenvar[1]=="NULL")
  {//factor ->NULL
    return INTSTAR;
  }
  else if(tree->tokenvar[1]=="LPAREN")
  {//factor ->LPAREN expr RPAREN
    return processExpr(tree->children[1]);
  }
  else if(tree->tokenvar[1]=="AMP")
  {//factor ->AMP lvalue
    if(processLvalue(tree->children[1])=="int")
    {
		return INTSTAR;
    }
    else
	{
	    string err="ERROR: should be int";
		throw err;
	}
  }
  else if(tree->tokenvar[1]=="STAR")
  //factor ->STAR factor
  {
    if(processFactor(tree->children[1])=="int*")
    {
		return INT;
	}
    else
    {}
  }
  else if(tree->tokenvar[1]=="NEW")
  {//factor ->NEW INT LBRACK expr RBRACK
    if(processExpr(tree->children[3])=="int")
    {
      return INTSTAR;
    }
    else
    {}
  }
  else if(tree->tokenvar.size()==4&&tree->tokenvar[1]=="ID")
  // factor ->ID LPAREN RPAREN, nothing in the () 
  {}
  else if ( tree->tokenvar.size()==5&&tree->tokenvar[1]=="ID") 
  //factor ->ID LPAREN arglist RPAREN,taking param
  {}
  return INT;
}

string processTerm(Tree* tree)
{
  if(tree->tokenvar[1]=="factor")
  {//term ->factor
    return processFactor(tree->children[0]);
  }
  else
  {
	  /*
	  term ->term STAR factor 
	  term ->term SLASH factor 
	  term ->term PCT factor
	  */
    if(processTerm(tree->children[0])=="int*"||processFactor(tree->children[2])=="int*")
    {}
    return INT;
  }
}

string processExpr(Tree* tree)
{
  if(tree->tokenvar[1]=="term")
  {//expr ->term 
    string t=processTerm(tree->children[0]);
    return t;
  }
  else if(tree->tokenvar[2]=="PLUS")
  {
	//expr ->expr PLUS term 
    string firsttype = processExpr(tree->children[0]);
    string secondtype = processTerm(tree->children[2]);
      if(firsttype=="int"&&secondtype=="int")
	  {
		return INT;
	  }
      else if (firsttype=="int*"&&secondtype=="int")
	  {
		return INTSTAR;
	  }
      else if (firsttype=="int"&&secondtype=="int*")
	  {
		return INTSTAR;
	  }
      else
	  {}
  }
  else if (tree->tokenvar[2]=="MINUS")
   {
	//expr ->expr MINUS term
    string firsttype = processExpr(tree->children[0]);
    string secondtype = processTerm(tree->children[2]);
	  if(firsttype=="int"&&secondtype=="int")
	  {
		return INT;
	  }
      else if (firsttype=="int*"&&secondtype=="int")
	  {
		return INTSTAR;
	  }
      else if (firsttype=="int*"&&secondtype=="int*")
	  {
		return INT;
	  }
	  else
	  {}
   }
   else{}
  }

/*
processLvalue:
lvalue ->ID  
lvalue ->STAR factor
lvalue ->LPAREN lvalue RPAREN 
The type of an lvalue deriving ID is the same as the type of that ID.
The type of an lvalue deriving LPAREN lvalue RPAREN is the same as the type of the derived lvalue.
*/
string processLvalue(Tree* tree)
{
  if(tree->tokenvar[1]=="ID")
  // value ->ID  
  {
	string t=ST[procedurename].second[tree->children[0]->tokenvar[1]].first;
    return t;
  }
  else if (tree->tokenvar[1]=="STAR")
  //lvalue ->STAR factor
  {
    if(processFactor(tree->children[1])=="int*")
    {
      return INT;
    }
    else
    {}
  }
  else if (tree->tokenvar[1]=="LPAREN")
  // lvalue ->LPAREN lvalue RPAREN 
  {
	string f=processLvalue(tree->children[1]);
    return f;
  }
  else
  {}
}

/*
  buildTree:
  build up the tree from an preorder input stored in cininput vector 
*/
Tree* buildTree()
{
	Tree* tree=new Tree;//construct a tree
	stringstream ss(cininput[counter]);
	string tt;
	while(ss>>tt)
	{
		tree->tokenvar.push_back(tt);
	}
	counter++;//need to increment counter so can go on
    string checktermi=tree->tokenvar[0];
    if(terminal.count(checktermi))
    {
	  //if it is a terminal, just return 
	  return tree;
    }
	//if not a terminal, recursively build tree for the non-terminal terms
	int derivationlength=tree->tokenvar.size()-1;
	for(int i=0;i<derivationlength;i++)
	{
		tree->children.push_back(buildTree());
	}
	return tree;
}

 /*
	returnType:
	identify the variable type and return it as a string
 */
string returnType(Tree* t){
  string type;
  if(t->children[0]->tokenvar.size()==2)//type ->INT
  {
    type="int";
  }
  else if(t->children[0]->tokenvar.size()==3)//type ->INT STAR
  {
    type="int*";
  }
  else
  {}
  return type;
}

/*
  buildST:
  Based on the tree constructed, build up SYMBOL TABLE
  Check types while building.
*/
void buildST(Tree* tree)
{
	if(tree->tokenvar[0]=="main")
	{
		//main ->INT WAIN LPAREN dcl COMMA dcl RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE 
		offset=0;
		procedurename="wain";//update current procedurename
		//update wain entry in global ST		
		ST[procedurename].first=2;
		processID(tree);//process the ID 
	}	
	else if(tree->tokenvar[0]=="procedure")
	{//procedure.INT ID LPAREN params RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE
		offset=0;
		procedurename=tree->children[1]->tokenvar[1];
		//update procedure entry in global ST
		ST[procedurename].first=getParameters(tree->children[3]).size();
		processID(tree);
	}
	else//every other case, keep going.
	{
		for(vector<Tree*>::iterator it = tree->children.begin();it!=tree->children.end();++it)
		{
		  buildST(*it);
		}
	}
}
/*
 Copyright Shuang Wu 20640582
 DO NOT COPY ANY PORTION OF THIS CODE. THANK YOU.
 */
/*
processID:
check and process ID to the global Symbol Table.
*/
void processID(Tree* tree)
{
  if(tree->tokenvar[0]=="dcl")
  {//dcl -> type ID
	ST[procedurename].second[tree->children[1]->tokenvar[1]].first=returnType(tree);
	ST[procedurename].second[tree->children[1]->tokenvar[1]].second=offset;	
	offset=offset-4;
	return;
  }
  else
  {
    //iterate through children for ID
    for(vector<Tree*>::iterator it=tree->children.begin();it!=tree->children.end();++it)
    {
      processID(*it);
    }
  }
}

//-----------------------------------------------------------------
//P9 P10 STUFF starts here
//-----------------------------------------------------------------

//write Comment
void writeComment(string c)
{
  //cout<<";"<<c<<endl;
  //comment it out for prettier output
}

//.import ...
void import(string l)
{
  cout<<".import "<<l<<endl;
}

//.word int
void word(int intval)
{
  cout<<".word "<<intval<<endl;
}

//.word label
void word(string label)
{
  cout<<".word "<<label<<endl;
}

//add $d, $s, $t 
void add(int d,int s,int t)
{
  cout<<"add $"<<d<<", $"<<s<<", $"<<t<<endl;
}

//sub $d, $s, $t
void sub(int d, int s, int t)
{
  cout<<"sub $"<<d<<", $"<<s<<", $"<<t<<endl;
}

//lw $t, i($s)
void lw(int t,int i,int s)
{
  cout<<"lw $"<<t<<", "<<i<<"($"<<s<<")"<<endl;
}

//lis $d
void lis(int d)
{
  cout<<"lis $"<<d<<endl;
}

//sw $t, i($s)
void sw(int t,int i,int s)
{
  cout << "sw $"<<t<<", "<<i<<"($"<<s<<")"<<endl;
}

//mult $s, $t
void mult(int s,int t)
{
  cout<<"mult $"<<s<<", $"<<t<<endl;
}

//div $s, $t
void div(int s,int t)
{
  cout<< "div $"<<s<<", $"<<t<<endl;
}

//mfhi $d
void mfhi(int d)
{
	 cout<<"mfhi $"<<d<<endl;
}
//mflo $d
void mflo(int d)
{
	 cout<<"mflo $"<<d<<endl;
}

//slt $d, $s, $t
void slt(int d,int s,int t)
{
  cout<<"slt $"<<d<<", $"<<s<<", $"<<t<<endl;
}

//sltu $d, $s, $t
void sltu(int d,int s,int t)
{
  cout<<"sltu $"<<d<<", $"<<s<<", $"<<t<<endl;
}

//beq $s, $t, i for label
void beq(int s,int t,string i)
{
  cout<<"beq $"<<s<<",$"<<t<<","<<i<<endl;
}

//beq $s, $t, i for offset
void beq(int s,int t,int i)
{
  cout<<"beq $"<<s<<",$"<<t<<","<<i<<endl;
}

//bne $s, $t, i for label
void bne(int s,int t,string i)
{
  cout<<"bne $"<<s<<",$"<<t<<","<<i<<endl;
}

//bne $s, $t, i for offset 
void bne(int s,int t,int i)
{
  cout<<"bne $"<<s<<",$"<<t<<","<<i<<endl;
}

//jr $s 
void jr(int s)
{
  cout<<"jr $"<<s<<endl;
}

//jalr $s
void jalr(int s)
{
  cout<<"jalr $"<<s<<endl;
}

void labelloc(string l)
{
  cout<<l<<":"<<endl;
}

void pop(int r)
{
  lw(r,0,30);
  add(30,30,4);
}

void push(int r)
{
  sw(r,-4,30);
  sub(30,30,4);
}

void prologue()
{
	writeComment("-----start prologue----");

	import("print");//import print
	import("init");//import init
	import("new");//import new
	import("delete");//import delete
	lis(4);//$4=4
	word(4);
	lis(11);//$11=1
	word(1);
	lis(12);//12 stores print
	word("print");
	lis(17);//17 stores init
	word("init");
	lis(18);//18 stores new
	word("new");
	lis(19);//19 stores delete
	word("delete");
	beq(0,0,"wain");//go to main
	cout<<endl;
	writeComment("-----end prologue----");
}
/*
 Copyright Shuang Wu 20640582
 DO NOT COPY ANY PORTION OF THIS CODE. THANK YOU.
 */
void mainprologue()
{//prologue for main
  push(31);//save registers on the stack 
  sub(29,30,4);//initially set the fram pointer $29 to $30-4
  push(1); 
  push(2);
  writeComment("-----start main----");
}

void mainepilogue()
{//epilogue for main 
    pop(2);
    pop(1);
    add(30,29,4);
    pop(31);
    jr(31);
}

//allocating a new array 
void newarr()
{
  push(31);
  jalr(18);//call new
  pop(31);
}

//deleting the array 
void deletearr()
{
  push(31);
  jalr(19);//call delete
  pop(31);
}

//initializing the space
void initarr()
{
  push(31);
  jalr(17);//call init
  pop(31);
}

/*
dclsASM:
translating dcls to mips
*/
void dclsASM(Tree* tree)
{
	//A9P1 rule dcls ->  DO NOTHING
    if(tree->children.size()>0)
	{	//get offset from symbol table
		int i=ST[procedurename].second[tree->children[1]->children[1]->tokenvar[1]].second;
		if(tree->tokenvar[4]=="NUM")
		{	 //A9P5 dcls -> dcls dcl BECOMES NUM SEMI
			dclsASM(tree->children[0]);
			lis(7);
			string num=tree->children[3]->tokenvar[1];
			word(num);//store the value of the NUM to register 7
			sw(7,i,29);//var i is at 29+4*i
			sub(30,30,4);
		}
		else if(tree->tokenvar[4]=="NULL")
		{	//A10P1 dcls -> dcls dcl BECOMES NULL SEMI
			dclsASM(tree->children[0]);
			sw(11,i,29);//var i is at 29+4*i
			sub(30,30,4);
		}
	}
}

/*
statementsASM:
translating statements to mips
*/
void statementsASM(Tree* tree)
{
    //A9p1:statements ->    DO NOTHING
    if(tree->children.size()>=1&&tree->tokenvar[1]=="statements")
	{
		//A9p4:statements -> statements statement
		statementsASM(tree->children[0]);//statements
		statementASM(tree->children[1]);//statement
	}
}

/*
statementASM:
translating statement to mips
*/
void statementASM(Tree* tree)
{
	if(tree->tokenvar[1]=="PRINTLN")
	{ //A9p4: statement ->PRINTLN LPAREN expr RPAREN SEMI
	  writeComment("in statement ->PRINTLN LPAREN expr RPAREN SEMI");
	  exprASM(tree->children[2]);
	  add(1,3,0);//move to 1 for print
	  push(31);
	  jalr(12);//call print
	  pop(31);
	}
	else if(tree->tokenvar[1]=="lvalue")
	{
	//A9P5:  statement -> lvalue BECOMES expr SEMI
      writeComment("in statement -> lvalue BECOMES expr SEMI");
      exprASM(tree->children[2]);
      push(3);
	  lvalueASM(tree->children[0]);
      pop(5);
      sw(5,0,3); 
	}
	else if(tree->tokenvar[1]=="WHILE")
	{
		//A9P6:  statement -> WHILE LPAREN test RPAREN LBRACE statements RBRACE
		stringstream s1;
		s1<<lc;
		lc++;
		string fix=s1.str();
		labelloc("loop"+ fix);
		testASM(tree->children[2]);
		beq(3,0,"endloop"+fix);//endloop
		statementsASM(tree->children[5]);
		beq(0,0,"loop"+fix);//loop again
		labelloc("endloop"+fix);
	}
	else if(tree->tokenvar[1]=="IF")
	{
		//A9P8:   statement -> IF LPAREN test RPAREN LBRACE statements RBRACE ELSE LBRACE statements RBRACE
		writeComment("in statement -> IF LPAREN test RPAREN LBRACE statements RBRACE ELSE LBRACE statements RBRACE");
		testASM(tree->children[2]);
		stringstream s1;
		s1<<Ic;
		Ic++;
		string fix=s1.str();
		beq(3,0,"else"+fix);//else
		statementsASM(tree->children[5]);
		beq(0,0,"endif"+fix);//endif
		labelloc("else"+ fix);
		statementsASM(tree->children[9]);
		labelloc("endif"+ fix);
	}
	else if(tree->tokenvar[1]=="DELETE")
	{
		//A10P2: statement ¡ú DELETE LBRACK RBRACK expr SEMI
		exprASM(tree->children[3]);
		beq(11,3,6);//if delete returns 1, then skips 6 lines
		add(1,3,0);//put $3 content into $1
		deletearr();
	}
}

/*
arglistASM:
translating arglist to mips
*/
void arglistASM(Tree* tree)
{
	if(tree->children.size()==1)
	{
		//A10P6 arglist -> expr
		exprASM(tree->children[0]);
		push(3);//push args to function
	}
	else if(tree->children.size()==3)
	{	//A10P6 arglist -> expr COMMA arglist
		exprASM(tree->children[0]);
		push(3);//push args to function
		arglistASM(tree->children[2]);
	}
}

/*
exprASM:
translating expr to mips
*/
void exprASM(Tree* tree)
{
    if(tree->children.size()==1)
    {//A9P1      expr ->term
      termASM(tree->children[0]);
    }
	else
	{
		exprASM(tree->children[0]);
		push(3);
		termASM(tree->children[2]);
		pop(5);
		if(tree->tokenvar[2]=="PLUS")
		{	
			string exprtype=processExpr(tree->children[0]);
			string termtype=processTerm(tree->children[2]);
			if(exprtype=="int"&&termtype=="int")
			{
				//A9P3 expr ->expr PLUS term 
				add(3,3,5);
			}
			else if(exprtype=="int*"&&termtype=="int")
			{//A10P3 expr has int* term has int
				mult(3,4);
				mflo(3);
				add(3,3,5);
			}
			else if(exprtype=="int"&&termtype=="int*")
			{//A10P3 expr has int term has int*
				mult(5,4);
				mflo(5);
				add(3,3,5);
			}
		}
		else if(tree->tokenvar[2]=="MINUS")
		{
			string exprtype=processExpr(tree->children[0]);
			string termtype=processTerm(tree->children[2]);
			if(exprtype=="int"&&termtype=="int")
			{
				//A9P3 expr ->expr MINUS term 
				sub(3,5,3);
			}
			else if(exprtype=="int*"&&termtype=="int")
			{//A10P3 expr has int* term has int
				mult(3,4);
				mflo(3);
				sub(3,5,3);
			}
			else if(exprtype=="int*"&&termtype=="int*")
			{//A10P3 expr has int* term has int*
				//$5 expr2 int *    $3 term int *
				sub(3,5,3);//gives num of bytes between the two 
				div(3,4);//need to div 4 to get the word distance
				mflo(3);
			}
		}
	}
}

/*
termASM:
translating term to mips
*/
void termASM(Tree* tree)
{
    if(tree->children.size()==1)
    {//A9P1 term ->factor 
      factorASM(tree->children[0]);
    }
	else
	{
		termASM(tree->children[0]);
		push(3);
		factorASM(tree->children[2]);
		pop(5);
		if(tree->tokenvar[2]=="STAR")
		{//A9P3 term ->term STAR factor 
			mult(5,3);
			mflo(3);
		}
		else if(tree->tokenvar[2]=="SLASH")
		{//A9P3 term ->term SLASH factor 
			div(5,3);
			mflo(3);
		}
		else if(tree->tokenvar[2]=="PCT")
		{//A9P3 term ->term PCT factor
			div(5,3);
			mfhi(3);	
		}
	}
}

/*
factorASM:
translating factor to mips
*/
void factorASM(Tree* tree)
{
    if(tree->children.size()==1)
    {
      if(tree->tokenvar[1]=="ID")
      {//A9P1   factor ->ID  
        lw(3,ST[procedurename].second[tree->children[0]->tokenvar[1]].second,29);
      }
	  else if(tree->tokenvar[1]=="NUM")
      {	 //  A9P3   factor ->NUM    
        lis(3);
		string num=tree->children[0]->tokenvar[1];
        word(num);
      }
	  else if(tree->tokenvar[1]=="NULL")
      {	//  A10P1   factor ->NULL
		//put 1 into 3 which is not a valid address----DEREFERENCE NULL IS NOT OKAY!!!
		writeComment("**********CRASH************dereference NULL");
		add(3,0,11);
      }
    }
	else if(tree->tokenvar[1]=="LPAREN")
    {//A9P2 factor ->LPAREN expr RPAREN  
      exprASM(tree->children[1]);
    }
	else if(tree->tokenvar[1]=="STAR")
    {//A10P1 factor ->STAR factor 
      factorASM(tree->children[1]);
	  lw(3,0,3); //dereference 
    }
	else if(tree->tokenvar[1]=="AMP")
    {//A10P1 factor ->AMP lvalue
		lvalueASM(tree->children[1]);
    }
	else if(tree->tokenvar[1]=="ID")
	{
		if(tree->children.size()==3)
		{//A10P5 factor ->ID LPAREN RPAREN
			push(29);
			push(31);
			lis(13);
			word("F"+tree->children[0]->tokenvar[1]);
			jalr(13);
			pop(31);
			pop(29);
	    }
		else if(tree->children.size()==4)
		{
		//A10P6 factor ->ID LPAREN arglist RPAREN
			push(29);
			push(31);
			arglistASM(tree->children[2]);//do the pushes here
			lis(13);
			word("F"+tree->children[0]->tokenvar[1]);
			jalr(13);
			//pop all args
			int poptimes=ST[tree->children[0]->tokenvar[1]].first;
			for(int i=0;i<poptimes;i++)
			{
				pop(14);
			}
			pop(31);
			pop(29);
		}
	}
	else if(tree->tokenvar[1]=="NEW")
    {//A10P2 factor ->NEW INT LBRACK expr RBRACK
      exprASM(tree->children[3]);
      add(1,3,0);//put $3 content into $1
      newarr();
      bne(3,0,1);//if new doesn't return 0, gives back pointer to pointer to new array, $3 is base address
	  writeComment("**********CRASH************new says no");
      add(3,0,11);//put 1 into 3 which is not a valid address
    }
}

/*
lvalueASM:
translating lvalue to mips
*/
void lvalueASM(Tree* tree)
{
	if(tree->tokenvar[1]=="ID")
	{ //A9p5:  lvalue -> ID
	  lis(3);
      word(ST[procedurename].second[tree->children[0]->tokenvar[1]].second);
      add(3,29,3);//put the address of the value in r3
	}
	else if(tree->tokenvar[1]=="STAR")
    {
	//A10P1:  lvalue -> STAR factor
      factorASM(tree->children[1]);
    }
	else if(tree->tokenvar[1]=="LPAREN")
	{
	//A9P5:  lvalue -> LPAREN lvalue RPAREN
	   lvalueASM(tree->children[1]);
	}
}

/*
testASM:
translating test to mips
*/
void testASM(Tree* tree)
{
	string ex1type=processExpr(tree->children[0]);
	string ex2type=processExpr(tree->children[2]);
	exprASM(tree->children[0]);
	push(3);
	exprASM(tree->children[2]);
	pop(5);
	if(tree->tokenvar[2]=="LT")
	{ 
		if(ex1type=="int"&&ex2type=="int")
		{//A9p6:   test -> expr LT expr
			slt(3,5,3);
		}
		else if(ex1type=="int*"&&ex2type=="int*")
		{////A10p4
			sltu(3,5,3);
		}
	}
	else if(tree->tokenvar[2]=="EQ")
	{
	//A9P7: 	 test -> expr EQ expr
		if(ex1type=="int"&&ex2type=="int")
		{
			slt(6,5,3);
			slt(7,3,5);
		}
		else if(ex1type=="int*"&&ex2type=="int*")
		{
			sltu(6,5,3);
			sltu(7,3,5);
		}
		add(3,6,7);
		sub(3,11,3); //!(NE)
	}
	else if(tree->tokenvar[2]=="NE")
	{
	//A9P7: 	 test -> expr NE expr
		if(ex1type=="int"&&ex2type=="int")
		{
			slt(6,5,3);
			slt(7,3,5);
		}
		else if(ex1type=="int*"&&ex2type=="int*")
		{
			sltu(6,5,3);
			sltu(7,3,5);
		}
		add(3,6,7);
	}
	else if(tree->tokenvar[2]=="LE")
	{
	//A9P7: 	 test -> expr LE expr
		if(ex1type=="int"&&ex2type=="int")
		{	
			slt(3,3,5);
		}
		else if(ex1type=="int*"&&ex2type=="int*")
		{ 
			sltu(3,3,5);
		}
		sub(3,11,3);//! GT
	}
	else if(tree->tokenvar[2]=="GE")
	{
	//A9P7: 	 test -> expr GE expr
		if(ex1type=="int"&&ex2type=="int")
		{
			slt(3,5,3);
		}
		else if(ex1type=="int*"&&ex2type=="int*")
		{
			sltu(3,5,3);
		}
	    sub(3,11,3); //!LT
	}
	else if(tree->tokenvar[2]=="GT")
	{
		if(ex1type=="int"&&ex2type=="int")
		{	//A9P7: 	 test -> expr GT expr
			slt(3,3,5);
		}
		else if(ex1type=="int*"&&ex2type=="int*")
		{  //A10P4
			sltu(3,3,5);
		}
	}	
}
/*
 Copyright Shuang Wu 20640582
 DO NOT COPY ANY PORTION OF THIS CODE. THANK YOU.
 */
/*
StructureCode:
handles wain, procedures, procedure*/
void StructureCode(Tree* tree)
{
  if(tree->tokenvar[0]=="procedures"&&tree->children.size() == 2)
  //A10P5 procedures  -> procedure procedures
  {
	StructureCode(tree->children[0]);
	StructureCode(tree->children[1]);
  }
  else if(tree->tokenvar[0]=="procedures"&&tree->children.size() == 1)
  //A9P1 procedures -> main
  {
	StructureCode(tree->children[0]);
  }
  else if(tree->tokenvar[0]=="main")
  {
	//A9P1 main ->INT WAIN LPAREN dcl COMMA dcl RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE 
    procedurename="wain";
	labelloc("wain");
    offset=0;
    mainprologue();//start it
	if(returnType(tree->children[3])=="int")
    {
		add(26,2,0);//in order to restore it back to 2.
		add(2,0,0);
	}
    initarr();
	add(2,26,0);//restore 2.
    dclsASM(tree->children[8]);//dcls
    statementsASM(tree->children[9]);//statements
    exprASM(tree->children[11]);//expr
    mainepilogue();//end it
  }
  else if(tree->tokenvar[0]=="procedure")
  {//A10P5 P6 procedure -> INT ID LPAREN params RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE
    procedurename=tree->children[1]->tokenvar[1];
    labelloc("F"+procedurename);
    sub(29,30,4);//set the frame pointer
	//need to fix symbol table by adding number of args to each entry
    int argsamt=ST[procedurename].first*4;
	for(map<string, pair<string, int> >::iterator it=(ST)[procedurename].second.begin();it!=(ST)[procedurename].second.end();it++)
    {
      (*it).second.second+=argsamt;
    }
	push(5);//SAVE USEFUL REGS
	push(6);
	push(7);
    dclsASM(tree->children[6]);
    statementsASM(tree->children[7]);
    exprASM(tree->children[9]);
	pop(7);
	pop(6);
	pop(5);//RESTORE USEFUL REGS
	add(30,29,4);
	jr(31);  
    }
  else
  {
    for(vector<Tree*>::iterator it = tree->children.begin();it!= tree->children.end(); it++)
    {
      StructureCode(*it);
    }
  }
}

int main() 
{
		  Tree *t;
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
		t=buildTree();//construct the tree from the input vector
		buildST(t);//construct the symbol table from the tree
		prologue();
		StructureCode(t);
	}catch(string &errormessage)
	{
		cerr<<errormessage<<endl;
    }
	 for(vector<Tree*>::iterator it = t->children.begin();it!=t->children.end();it++)
     {
        delete *it;
     }
	delete t;
}	
/*
 Copyright Shuang Wu 20640582
 DO NOT COPY ANY PORTION OF THIS CODE. THANK YOU.
 */
