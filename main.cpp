#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>
using namespace std;

struct core {
    vector<string> variables;
    vector<string> values;
    vector<string> stack;
    vector<string> label;
    vector<int> lineNumber;
    string returnString;
};

const string ADDITION_SIGN = "+";
const string MINUS_SIGN = "-";
const string MULTIPLICATION_SIGN = "*";
const string DIVISION_SIGN = "/";
const string EQUAL_SIGN = "=";
const string GREATER_THAN_SIGN = ">";
const string LESS_THAN_SIGN = "<";
const vector<string> KEYWORDS = {"out", "var", "clear", "label", "jump", "True", "False", "if", "endif", "else"};

int globalProgramCounter = 0;
bool globalProgramCounterChanged = false;

void printvector(vector<string> vec);
bool isNumber(const string s);
bool isKeyword(string word);
string strip(string token);

core execute(vector<vector<string>> code, core mainCore, int start);
core evaluate(vector<string> line, core inputCore);
void output(core inputCore);
core setVariable(core inputCore);
core createLabel(core inputCore, int lineNumber);
void jump(core inputCore);
core ifStatement(vector<vector<string>> code, int lineNumber, core inputCore);

int main () {
  // Create a text string, which is used to output the text file
  vector<vector<string>> code;
  vector<string> line;
  string text;
  string filename;

  cout << "Enter a filename:";
  cin >> filename;

  // Read from the text file
  ifstream MyReadFile(filename);
  system("cls");

  // Use a while loop together with the getline() function to read the file line by line
  while (getline (MyReadFile, text)) {
    string word;
    line = {};
    bool strexeption = false;

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
        if (string(1, character) == " " && strexeption == false) {
            line.push_back(word);
            word = "";
        }
        if (string(1, character) == " " && strexeption == true) {
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

core execute(vector<vector<string>> code, core mainCore, int start) {
    int programCounter = 0;
    int end = start + code.size() - 1;
    while (programCounter < code.size() && !globalProgramCounterChanged) {
        vector<string> line = code.at(programCounter);
        mainCore.stack = {};
        mainCore = evaluate(line, mainCore);
        if (mainCore.stack.front() == "out") {
            output(mainCore);
        }
        else if (mainCore.stack.front() == "var") {
            mainCore = setVariable(mainCore);
        }
        else if (mainCore.stack.front() == "clear") {
            system("cls");
        }
        else if (mainCore.stack.front() == "label") {
            mainCore = createLabel(mainCore, programCounter);
        }
        else if (mainCore.stack.front() == "jump") {
            jump(mainCore);
        }
        else if (mainCore.stack.front() == "if") {
            mainCore = ifStatement(code, programCounter, mainCore);
            programCounter = stof(mainCore.returnString)-1;
        }
        
        if (globalProgramCounterChanged && (globalProgramCounter >= start) && (globalProgramCounter <= end)) {
            programCounter = globalProgramCounter;
            globalProgramCounter = 0;
            globalProgramCounterChanged = false;
        }
        else {
            programCounter++;
        }
    }
    return mainCore;
}

void printvector(vector<string> vec) {
    for (string item : vec) {
        cout << item << ", ";
    }
    cout << "\n";
}

void printvector(vector<int> vec) {
    for (int item : vec) {
        cout << item << ", ";
    }
    cout << "\n";
}

bool isNumber(const std::string s) {
    std::istringstream iss(s);
    float f;
    // Try to parse the string as a float and ensure there's no leftover content
    return (iss >> f) && (iss.eof());
}

string strip(string token) {
    string strippedToken;
    for (char letter : token) {
        if (!::isspace(letter)) {
            strippedToken.push_back(letter);
        }
    }
    return strippedToken;
}

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

    for (string token : line) {
        token = strip(token);
        if (!token.empty() && (token[0] == '"' || isNumber(token) || isKeyword(token))) {
            inputCore.stack.push_back(token);
        }
        else if (!token.empty() && line.front() == "var" && inputCore.stack.size() == 1) {
            inputCore.stack.push_back(token);
        }
        else if (!token.empty() && line.front() == "label" && inputCore.stack.size() == 1) {
            inputCore.stack.push_back(token);
        }
        else if (!token.empty() && line.front() == "if" && inputCore.stack.size() == 1){
            inputCore.stack.push_back(token);
        }
        else if (!token.empty() && line.front() == "endif" && inputCore.stack.size() == 1){
            inputCore.stack.push_back(token);
        }
        else if (!token.empty()) {
            int j = 0;
            bool found = false;
            for (string variable : inputCore.variables) {
                if (variable == token) {
                    inputCore.stack.push_back(inputCore.values[j]);
                    found = true;
                    break;
                }
                j = j + 1;
            }
            if (!found) {
                for (string label : inputCore.label) {
                    if (label == token) {
                        inputCore.stack.push_back(token);
                        found = true;
                        break;
                    }
                }
            }
            if (!found) {
                cout << "Error, variable/keyword " << token << " not found!";
                abort();
            }
        }

        if (token == ADDITION_SIGN) {
            add = true;
            inputCore.stack.pop_back();
        }
        else if (token == MINUS_SIGN) {
            subtract = true;
            inputCore.stack.pop_back();
        }
        else if (token == MULTIPLICATION_SIGN) {
            multiplication = true;
            inputCore.stack.pop_back();
        }
        else if (token == DIVISION_SIGN) {
            division = true;
            inputCore.stack.pop_back();
        }
        else if (token == EQUAL_SIGN) {
            if (equal) {
                equalEqual = true;
                equal = false;
            }
            else if (!equal) {
                equal = true;
            }
            inputCore.stack.pop_back();
        }
        else if (token == GREATER_THAN_SIGN) {
            if (equal) {
                greaterThanEqual = true;
                equal = false;
            }
            else if (!equal) {
                greaterThan = true;
            }
            inputCore.stack.pop_back();
        }
        else if (token == LESS_THAN_SIGN) {
            if (equal) {
                lessThanEqual = true;
                equal = false;
            }
            else if (!equal) {
                lessThan = true;
            }
            inputCore.stack.pop_back();
        }

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

bool isKeyword(string word) {
    for (string keyword: KEYWORDS) {
        if (word == keyword) {
            return true;
        }
    }
    if (word == ADDITION_SIGN || word == MINUS_SIGN || word == DIVISION_SIGN || word == MULTIPLICATION_SIGN) {
        return true;
    }
    if (word == EQUAL_SIGN || word == GREATER_THAN_SIGN || word == LESS_THAN_SIGN) {
        return true;
    }
    return false;
}

void output(core inputCore) {
    int i = 1;
    while (i < inputCore.stack.size()) {
        bool backslash = false;
        for (char letter : inputCore.stack.at(i)) {
            if (letter == '"') {}
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
            else {
                cout << letter;
            }
        }
        if (i != (inputCore.stack.size()-1)) {
            cout << " ";
        }
        i = i + 1;
    }
}

core setVariable(core inputCore) {
    int i = 0;
    bool exists = false;
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