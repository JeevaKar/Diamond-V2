#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>
using namespace std;

// Key data structures and functions declarations
/**
 * Core data that the interpreter uses
 */
struct core {
	// Core data that the interpreter uses
	vector<string> variables; // List of variable names
	vector<string> values; // List of variable values (index of `value` corresponds to index of `variables`)
	vector<string> stack; // Stack, serves as working memory for the interpreter as it moves through each line
	vector<string> label; // List of label name
	vector<int> lineNumber; // List of line numbers (index of `lineNumber corresponds to index of `labal`)
	string returnString; // String for return data
};
/**
 * Constants that represent reserved tokens and keywords of the language
 */
struct keyTokens {
	const string ADDITION_SIGN = "+";
	const string MINUS_SIGN = "-";
	const string MULTIPLICATION_SIGN = "*";
	const string DIVISION_SIGN = "/";
	const string EQUAL_SIGN = "=";
	const string GREATER_THAN_SIGN = ">";
	const string LESS_THAN_SIGN = "<";
	const vector<string> KEYWORDS = {"out", "var", "clear", "label", "jump", "True", "False", "if", "endif", "else"};
};
/**
 * Contains all function are are called only within the code
 * These functions do not handle diamond code directly
*/
namespace helperfunctions {

	// Namespace-wide variables
	keyTokens reservedKeywords;

	void printvector(vector<string> vec) {
		// Prints vectors of strings
		for (string item : vec) {
			cout << item << ", ";
		}
		cout << "\n";
	}
	
	void printvector(vector<int> vec) {
		// Prints vectors of int
		for (int item : vec) {
			cout << item << ", ";
		}
		cout << "\n";
	}

	bool isNumber(const std::string s) {
		// Returns a bool of whether or not the input string is a number 
		istringstream iss(s);
		float f;
		return (iss >> f) && (iss.eof());
	}

	bool isKeyword(string word) {
		// Returns a bool of whether or not the input word is a keyword
		// for loop loops through keywords
		for (string keyword: reservedKeywords.KEYWORDS) {
			if (word == keyword) {
				return true;
			}
		}
		// Checks if the word is a math symbol
		if (word == reservedKeywords.ADDITION_SIGN || word == reservedKeywords.MINUS_SIGN || word == reservedKeywords.DIVISION_SIGN || word == reservedKeywords.MULTIPLICATION_SIGN) {
			return true;
		}
		// Checks if the word is a logic symbol
		if (word == reservedKeywords.EQUAL_SIGN || word == reservedKeywords.GREATER_THAN_SIGN || word == reservedKeywords.LESS_THAN_SIGN) {
			return true;
		}
		// Default condition, returns false
		return false;
	}

	string strip(string token) {
		// Removes blank space from input tokens
		string strippedToken;
		for (char letter : token) {
			if (!::isspace(letter)) {
				strippedToken.push_back(letter);
			}
		}
		return strippedToken;
	}
}

//Global variables
int globalProgramCounter = 0;
bool globalProgramCounterChanged = false;
keyTokens reservedKeywords;

//Declaration of functions which handle code execution
core execute(vector<vector<string>> code, core mainCore, int start);
core evaluate(vector<string> line, core inputCore);
void output(core inputCore);
core setVariable(core inputCore);
core createLabel(core inputCore, int lineNumber);
void jump(core inputCore);
core ifStatement(vector<vector<string>> code, int lineNumber, core inputCore);

int main () {
	// Variables necessary for reading the code
	vector<vector<string>> code;
	vector<string> line;
	string text;
	string filename;

	// Gets filename from user
	cout << "Enter a filename: ";
	cin >> filename;

	// Reads file
	ifstream MyReadFile(filename);
	system("cls");

	// Reads all lines and adds to the variable `code`
	while (getline (MyReadFile, text)) {
		string word;
		line = {};
		bool strexeption = false;

		// Tokenizes every line by spaces or strings
		for (char character : text){
			if (string(1, character) != " ") {
				if (string(1, character) == string(1, '"') && strexeption == false) {
					strexeption = true;
				}
				else if (string(1, character) == string(1, '"') && strexeption == true) {
					strexeption = false;
				}
				word = word + string(1, character);
			}
			if (string(1, character) == " " && !strexeption) {
				line.push_back(word);
				word = "";
			}
			if (string(1, character) == " " && strexeption) {
				word = word + string(1, character);
			}
		}
		if (word != "") {
			line.push_back(word);
			word = "";
		}
		code.push_back(line);
	}

	MyReadFile.close();
	core mainCore;
	execute(code, mainCore, 0);
}

/**
 * Handles spinning off of code to other functions
 * Runs through the code line by line
 * 
 * WARNING THIS FUNCTION IS NOT EXPLICITLY RECURSIVE
 * BUT MAY BE CALLED ON BY A FUNCTION IT SPINS OFF CODE TO
 * 
 * @param code matrix of code, split by line, then by tokens
 * @param mainCore initial input data, usually from a previous scope
 * @param start line number of the first line of code of this scope
 * 
 * @return a modified version of `mainCore` after it is changed by execution
 */
core execute(vector<vector<string>> code, core mainCore, int start) {
	int programCounter = 0; // Counts the lines through the program
	int end = start + code.size() - 1; // Line number of the last line of code in this scope

	// This while loops runs through each line of code, preprocesses it, then spins it off to
	// the respective function that handles that operation
	while (programCounter < code.size() && !globalProgramCounterChanged) {
		vector<string> line = code.at(programCounter);
		mainCore.stack = {};
		mainCore = evaluate(line, mainCore);

		// Output function
		if (mainCore.stack.front() == "out") {
			output(mainCore);
		}
		// Declaring variables
		else if (mainCore.stack.front() == "var") {
			mainCore = setVariable(mainCore);
		}
		// Clearing screen
		else if (mainCore.stack.front() == "clear") {
			system("cls");
		}
		// Declaring a label
		else if (mainCore.stack.front() == "label") {
			mainCore = createLabel(mainCore, programCounter);
		}
		// Jumping to a label
		else if (mainCore.stack.front() == "jump") {
			jump(mainCore);
		}
		// If statements
		else if (mainCore.stack.front() == "if") {
			mainCore = ifStatement(code, programCounter, mainCore);
			programCounter = stof(mainCore.returnString)-1;
		}
		
		// Changes the program counter if it is within the scope
		if (globalProgramCounterChanged && (globalProgramCounter >= start) && (globalProgramCounter <= end)) {
			programCounter = globalProgramCounter;
			globalProgramCounter = 0;
			globalProgramCounterChanged = false;
		}
		// Returns the mainCore if it is not within the scope
		else if (globalProgramCounterChanged && ((globalProgramCounter <= start) || (globalProgramCounter >= end))) {
			return mainCore;
		}
		// Continues with the loop
		else {
			programCounter++;
		}
	}
	return mainCore;
}

/**
 * Preprocesses code before execution by removing blank spaces,
 * doing math, and replacing variables with their values
 * 
 * @param line input line, used as the raw input
 * @param inputCore the `line` is modified and pushed onto the stack
 * 
 * @return the core is returned with a modified stack so that it can be executed
 */
core evaluate(vector<string> line, core inputCore) {
	int i = 0;
	bool add = false;
	bool subtract = false;
	bool multiplication = false;
	bool division = false;
	bool equal = false;
	bool lessThan = false;
	bool greaterThan = false;
	bool lessThanEqual = false;
	bool greaterThanEqual = false;
	bool equalEqual = false;

	// Goes through each token per line
	for (string token : line) {
		token = helperfunctions::strip(token); // Removes whitespace from tokens

		// If the token is a recognized keyword it is pushed to the stack
		if (!token.empty() && (token[0] == '"' || helperfunctions::isNumber(token) || helperfunctions::isKeyword(token))) {
			inputCore.stack.push_back(token);
		}
		// If the token is a new variable name during variable declaration it is pushed to the stack
		else if (!token.empty() && inputCore.stack.front() == "var" && inputCore.stack.size() == 1) {
			inputCore.stack.push_back(token);
		}
		// If the token is a new label name during label creation it is pushed to the stack
		else if (!token.empty() && inputCore.stack.front() == "label" && inputCore.stack.size() == 1) {
			inputCore.stack.push_back(token);
		}
		// If the token is the name of an if, endif, or else statement, it is pushed to the stack
		else if (!token.empty() && inputCore.stack.front() == "if" && inputCore.stack.size() == 1){
			inputCore.stack.push_back(token);
		}
		else if (!token.empty() && inputCore.stack.front() == "endif" && inputCore.stack.size() == 1){
			inputCore.stack.push_back(token);
		}
		else if (!token.empty() && inputCore.stack.front() == "else" && inputCore.stack.size() == 1){
			inputCore.stack.push_back(token);
		}
		// Attempt to replace `token` with the value of a variable or label
		else if (!token.empty()) {
			// Loop indexes
			int j = 0;
			bool found = false;

			// Search through all variables for a match
			for (string variable : inputCore.variables) {
				if (variable == token) {
					inputCore.stack.push_back(inputCore.values[j]);
					found = true;
					break;
				}
				j = j + 1;
			}
			// Search through all labels for a match
			if (!found) {
				for (string label : inputCore.label) {
					if (label == token) {
						inputCore.stack.push_back(token);
						found = true;
						break;
					}
				}
			}
			// Return an error if the variable is not found
			if (!found) {
				cout << "Error, variable/keyword " << token << " not found!";
				abort();
			}
		}

		// Remove the token and raise a flag if `token` matches a math or logic symbol
		if (token == reservedKeywords.ADDITION_SIGN) {
			add = true;
			inputCore.stack.pop_back();
		}
		else if (token == reservedKeywords.MINUS_SIGN) {
			subtract = true;
			inputCore.stack.pop_back();
		}
		else if (token == reservedKeywords.MULTIPLICATION_SIGN) {
			multiplication = true;
			inputCore.stack.pop_back();
		}
		else if (token == reservedKeywords.DIVISION_SIGN) {
			division = true;
			inputCore.stack.pop_back();
		}
		else if (token == reservedKeywords.EQUAL_SIGN) {
			if (equal) {
				equalEqual = true;
				equal = false;
			}
			else if (!equal) {
				equal = true;
			}
			inputCore.stack.pop_back();
		}
		else if (token == reservedKeywords.GREATER_THAN_SIGN) {
			if (equal) {
				greaterThanEqual = true;
				equal = false;
			}
			else if (!equal) {
				greaterThan = true;
			}
			inputCore.stack.pop_back();
		}
		else if (token == reservedKeywords.LESS_THAN_SIGN) {
			if (equal) {
				lessThanEqual = true;
				equal = false;
			}
			else if (!equal) {
				lessThan = true;
			}
			inputCore.stack.pop_back();
		}

		// If a flag was raised on the previous execution,
		// carry out the logic or math
		else if (add) {
			float rhs = stof(inputCore.stack.back());
			inputCore.stack.pop_back();
			float lhs = stof(inputCore.stack.back());
			inputCore.stack.pop_back();
			inputCore.stack.push_back(to_string(lhs+rhs));
			add = false;
		}
		else if (subtract) {
			float rhs = stof(inputCore.stack.back());
			inputCore.stack.pop_back();
			float lhs = stof(inputCore.stack.back());
			inputCore.stack.pop_back();
			inputCore.stack.push_back(to_string(lhs-rhs));
			subtract = false;
		}
		else if (multiplication) {
			float rhs = stof(inputCore.stack.back());
			inputCore.stack.pop_back();
			float lhs = stof(inputCore.stack.back());
			inputCore.stack.pop_back();
			inputCore.stack.push_back(to_string(lhs*rhs));
			multiplication = false;
		}
		else if (division) {
			float rhs = stof(inputCore.stack.back());
			inputCore.stack.pop_back();
			float lhs = stof(inputCore.stack.back());
			inputCore.stack.pop_back();
			inputCore.stack.push_back(to_string(lhs/rhs));
			division = false;
		}
		else if (lessThan) {
			float rhs = stof(inputCore.stack.back());
			inputCore.stack.pop_back();
			float lhs = stof(inputCore.stack.back());
			inputCore.stack.pop_back();
			if (lhs<rhs) {
				inputCore.stack.push_back("True");
			}
			else {
				inputCore.stack.push_back("False");
			}
			lessThan = false;
		}
		else if (greaterThan) {
			float rhs = stof(inputCore.stack.back());
			inputCore.stack.pop_back();
			float lhs = stof(inputCore.stack.back());
			inputCore.stack.pop_back();
			if (lhs>rhs) {
				inputCore.stack.push_back("True");
			}
			else {
				inputCore.stack.push_back("False");
			}
			greaterThan = false;
		}
		else if (lessThanEqual) {
			float rhs = stof(inputCore.stack.back());
			inputCore.stack.pop_back();
			float lhs = stof(inputCore.stack.back());
			inputCore.stack.pop_back();
			if (lhs <= rhs) {
				inputCore.stack.push_back("True");
			}
			else {
				inputCore.stack.push_back("False");
			}
			lessThanEqual = false;
		}
		else if (greaterThanEqual) {
			float rhs = stof(inputCore.stack.back());
			inputCore.stack.pop_back();
			float lhs = stof(inputCore.stack.back());
			inputCore.stack.pop_back();
			if (lhs>=rhs){
				inputCore.stack.push_back("True");
			}
			else {
				inputCore.stack.push_back("False");
			}
			greaterThanEqual = false;
		}
		
		i++;
	}
	return inputCore;
}

/**
 * Handles output to the terminal
 * 
 * @param inputCore Works with the stack of the inputCore to know what to output
 */
void output(core inputCore) {
	int i = 1; // Loop index

	// Loop through every token in the stack that isn't the keyword
	while (i < inputCore.stack.size()) {
		bool backslash = false; // FLAG - Raised when a backslash is detected

		// Runs through every character in the tokens to be printed
		for (char letter : inputCore.stack.at(i)) {
			if (letter == '"') {} // Ignore " character from string declaration
			// Hangle special backslash notation
			else if (backslash) {
				if (letter == 'n') {
					cout << "\n";
				}
				else if (letter == '\\') {
					cout << "\\";
				}
				backslash = false;
			}
			else if (letter == '\\') {
				backslash = true;
			}
			// Output letter if no flags are raised
			else {
				cout << letter;
			}
		}
		// Add a space between different tokens
		if (i != (inputCore.stack.size()-1)) {
			cout << " ";
		}
		i = i + 1;
	}
}

/**
 * Sets/changes variable values
 * 
 * @param inputCore Uses the `inputCore.stack` to modify `inputCore.variables` and `inputCore.values`
 * 
 * @return Returns a modified `inputCore` with the changes to the variable
 */
core setVariable(core inputCore) {
	int i = 0; // List index
	bool exists = false; // FLAG - Raised if the variable already exists
	///////////////////////////////////////
	// TODO: COMMENT FROM HERE DOWNWARDS //
	///////////////////////////////////////
	for (string variable: inputCore.variables) {
		if (variable == inputCore.stack.at(1)) {
			exists = true;
			break;
		}
		i = i + 1;
	}
	if (exists) {
		inputCore.values[i] = inputCore.stack.at(2);
	}
	else {
		inputCore.variables.push_back(inputCore.stack.at(1));
		inputCore.values.push_back(inputCore.stack.at(2));
	}
	return inputCore;
}

core createLabel(core inputCore, int lineNumber) {
	int i = 0;
	for (string label : inputCore.label) {
		if (label == inputCore.stack.at(1)) {
			inputCore.lineNumber[i] = lineNumber;
			return inputCore;
		}
		i++;
	}
	inputCore.label.push_back(inputCore.stack.at(1));
	inputCore.lineNumber.push_back(lineNumber);
	return inputCore;
}

void jump(core inputCore) {
	string labelName = inputCore.stack.at(1);
	int i = 0;
	bool labelExists = false;
	for (string label : inputCore.label) {
		if (label == labelName) {
			globalProgramCounter = inputCore.lineNumber.at(i)+1;
			globalProgramCounterChanged = true;
			labelExists = true;
			break;
		}
		i++;
	}
	if (!labelExists) {
		cout << "Error, label " << labelName << " not found.";
		abort();
	}
}

core ifStatement(vector<vector<string>> code, int lineNumber, core inputCore) {
	vector<vector<string>> ifcode;
	vector<vector<string>> elsecode;
	bool elseStatement = false;
	bool flag = false;
	vector<int> lineNumbers;
	lineNumber ++;
	lineNumbers.push_back(lineNumber);
	while ((lineNumber < code.size()) && (!flag)) {
		vector<string> line = code.at(lineNumber);
		if (line.front() == "else" && line.back() == inputCore.stack.at(1)) {
			elseStatement = true;
			lineNumbers.push_back(lineNumber+1);
		}
		else if (line.front() == "endif" && line.back() == inputCore.stack.at(1)) {
			flag = true;
			lineNumbers.push_back(lineNumber+1);
		}
		else {
			if (!elseStatement) {
				ifcode.push_back(line);
			}
			else {
				elsecode.push_back(line);
			}
		}
		lineNumber ++;
	}
	if (!flag) {
		cout << "End if statement not found!";
		abort();
	}
	else if (inputCore.stack.at(2) == "True") {
		inputCore = execute(ifcode, inputCore, lineNumbers.front());
	}
	else if (elseStatement) {
		inputCore = execute(elsecode, inputCore, lineNumbers.at(1));
	}
	inputCore.returnString = to_string(lineNumbers.back());
	return inputCore;
}