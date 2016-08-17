/*
 Copyright Shuang Wu
 DO NOT COPY ANY PORTION OF THIS CODE. THANK YOU.
 */
#include <iostream>
#include <string>
#include <set>
#include <fstream>
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
using std::ifstream;
using std::string;
using std::istream;
using std::ostream;

vector<string> outputstack;
vector<string> inputstack;
vector<string> inputstack1; 
int counter;
set<string> terminal;

struct Tree
{
	vector<Tree> children;
	string derivation;
};

Tree structingTree();
void Traversal(Tree& t);

void skipLine(istream &in) 
{
	string s;
	getline(in,s);
}

void skipGrammar(istream &in) 
{
	int ruleAmount;
	//READ IN terminals 
	in>>ruleAmount;
	string term;
	skipLine(in);
	//insert the terminals in the terminal set
	for (int i =0;i<ruleAmount;i++) 
	{
		in>>term;
		terminal.insert(term);
	}
	//cuz EOF and BOF won't appear from the rules,
	//so I need to hardcode it.
	term="EOF";
	terminal.insert(term);
	term="BOF";
	terminal.insert(term);
	
	//skip the non terminals
	in>>ruleAmount;
	skipLine(in);
	for (int i=0;i<ruleAmount;i++) 
	{
		skipLine(in);
	}
	//skip the start state.
	skipLine(in);
}
/*
 Copyright Shuang Wu
 DO NOT COPY ANY PORTION OF THIS CODE. THANK YOU.
 */

pair<string,string> checkTrans(string state, string input,map<pair<string, string>, pair<string, string> >& transitions){
	pair<string,string> inputpair(state, input);
	if (transitions.find(inputpair) == transitions.end() ) 
	{
		pair<string, string> errorTrans("-ERROR", "-ERROR");
		return errorTrans;
	}
	return transitions.find(inputpair)->second;
}


void processRuleandTrans(istream &in, istream &in1, ostream &out){
	int ruleAmount,transAmount;
	string readin,Start;
	vector<string> rulestorage;
	vector<string> symbolstack;
	vector<string> statestack;
	map<pair<string, string>, pair<string, string> > transitions;
	
	in >> ruleAmount;
	skipLine(in);

	for (int i=0;i<ruleAmount;i++)
	{
		if (i==0)
		{
			getline(in,readin);
			rulestorage.push_back(readin);
			Start=readin;
		}
		else
		{
			getline(in,readin);
			rulestorage.push_back(readin);
		}
	}

	skipLine(in);
	in >> transAmount;
	skipLine(in);

	for (int i=0;i<transAmount;i++)
	{
		getline(in,readin);
		stringstream ss(readin);
		string t1,t2,t3,t4;
		ss >> t1>> t2>> t3>> t4;
		pair<string,string> firstpair(t1,t2);
		pair<string,string> followaction(t3,t4);
		transitions[firstpair] = followaction;
	}

	//split the input each line and then store into inputstack and inputstack1
	//need to hardcode BOF and EOF
	string input, input1;
	string begin="BOF";
	string ending="EOF";
	inputstack.push_back(begin);
	inputstack1.push_back(begin);

	while (in1>> input >> input1)
	{		
		inputstack.push_back(input);
        inputstack1.push_back(input1);
	}
	inputstack.push_back(ending);
	inputstack1.push_back(ending);
	
	//start by pushing a 0 to the stack.
	statestack.push_back("0");
	
	pair<string,string> followupTrans;

	for (int i=0;i<inputstack.size(); i++)
	{
		string currentprocessing=inputstack[i];
		string currentstate=statestack.back();
		followupTrans = checkTrans(currentstate,currentprocessing,transitions);
		string action=followupTrans.first;
		string corresrule=followupTrans.second;
		if (action=="reduce")
		{
			i--;//need to keep the i in the same spot
			int convertRule;
			stringstream cc(corresrule);
			cc>>convertRule;
			int rulelength=0;
			string breakdownrule,testrule;
			testrule=rulestorage[convertRule];
			stringstream ss1(testrule);
			while(ss1>>breakdownrule){
				rulelength++;
			}
			for (int j=1; j<rulelength; j++)
			{ //need to pop # of RHS from both statestack and symbolstack
				statestack.pop_back();
				symbolstack.pop_back();
			}
			string rulestart;
			stringstream ssssss(testrule);
			ssssss>>rulestart;
			symbolstack.push_back(rulestart);
			currentstate=statestack.back();
		    string currentsymbol=symbolstack.back();
			string newstate=checkTrans(currentstate,currentsymbol,transitions).second;
			statestack.push_back(newstate);
			string usedrules=rulestorage[convertRule];
			outputstack.push_back(usedrules);
		}
		else if (action=="shift")
		{
			symbolstack.push_back(currentprocessing);
			statestack.push_back(corresrule);
		}
		else if (action=="-ERROR")
		{
			cerr<<"ERROR at "<<i<<endl;
			return;
		}
	}
	outputstack.push_back(Start);

    counter=inputstack.size()-1;
    Tree t=structingTree();//build up the parse tree
	Traversal(t);//traverse the tree

}
/*
 Copyright Shuang Wu
 DO NOT COPY ANY PORTION OF THIS CODE. THANK YOU.
 */
void Traversal(Tree& t) //preorder traversal
{//since the children are stored from right most to left most,
// so the iterator for traverse need to go reversed order.
	cout<<t.derivation<<endl;
	for(vector<Tree>::reverse_iterator it = t.children.rbegin(); it != t.children.rend(); ++it) 
	{
		Traversal(*it); 
	}
}
/*
 Copyright Shuang Wu
 DO NOT COPY ANY PORTION OF THIS CODE. THANK YOU.
 */
Tree structingTree()
{//it is a recursive function.
	Tree newroot;
	newroot.derivation=outputstack.back();
	outputstack.pop_back();
	int rulelength=0;
	string breakdownrule;
	vector<string> derivationbreak;
	stringstream ss1(newroot.derivation);
			while(ss1>>breakdownrule)
			{
				derivationbreak.push_back(breakdownrule);
				rulelength++;
			}
	
	for(int i=rulelength-1;i>0;i--)
	{//=0 not in set
		if(terminal.count(derivationbreak[i]))
		{//terminal
			Tree tempchild;
			tempchild.derivation=inputstack[counter]+" "+inputstack1[counter];
			counter--;
			newroot.children.push_back(tempchild);
		}
		else
		{
			newroot.children.push_back(structingTree());
		}
	}
	return newroot;
}

int main() {
	ifstream file("WLP4rules.txt");//the rules containing WLP4 grammar rules
	skipGrammar(file);
	processRuleandTrans(file,cin,cout);
}	
/*
 Copyright Shuang Wu
 DO NOT COPY ANY PORTION OF THIS CODE. THANK YOU.
 */

