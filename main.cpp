#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
using namespace std;

vector<string> variables;
vector<string> values;
vector<string> stack;

const string ADDITION_SIGN = "+";
const string MINUS_SIGN = "-";
const string MULTIPLICATION_SIGN = "*";
const string DIVISION_SIGN = "/";
const string EQUAL_SIGN = "=";
const vector<string> KEYWORDS = {"out", "var"};

void printvector(vector<string> vec);
bool isNumber(const string s);
bool isKeyword(string word);

void evaluate(vector<string> line);
void output();
void setVariable();

int main () {
  // Create a text string, which is used to output the text file
  vector<vector<string>> code;
  vector<string> line;
  string text;

  // Read from the text file
  ifstream MyReadFile("main.di");

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
  
  for (vector<string> line : code) {
    stack = {};
    evaluate(line=line);
    if (stack.front() == "out") {
        output();
    }
    else if (stack.front() == "var") {
        setVariable();
    }
  }
}

void printvector(vector<string> vec) {
    for (string item : vec) {
        cout << item << ", ";
    }
}

bool isNumber(const std::string s) {
    std::istringstream iss(s);
    float f;
    // Try to parse the string as a float and ensure there's no leftover content
    return (iss >> f) && (iss.eof());
}

void evaluate(vector<string> line) {
    int i = 0;
    bool add = false;
    bool subtract = false;
    bool multiplication = false;
    bool division = false;
    for (string token : line) {
        if (!token.empty() && (token[0] == '"' || isNumber(token) || isKeyword(token))) {
            stack.push_back(token);
        }
        else if (line.front() == "var" && i == 1) {
            stack.push_back(token);
        }
        else {
            int j = 0;
            bool found = false;
            for (string variable : variables) {
                if (variable == token) {
                    stack.push_back(values[j]);
                    found = true;
                    break;
                }
                j = j + 1;
            }
            if (!found) {
                cout << "Error, variable/keyword " << token << " not found!";
                abort();
            }
        }

        if (token == ADDITION_SIGN) {
            add = true;
            stack.pop_back();
        }
        else if (token == MINUS_SIGN) {
            subtract = true;
            stack.pop_back();
        }
        if (token == MULTIPLICATION_SIGN) {
            multiplication = true;
            stack.pop_back();
        }
        else if (token == DIVISION_SIGN) {
            division = true;
            stack.pop_back();
        }

        else if (add == true) {
            float rhs = stof(stack.back());
            stack.pop_back();
            float lhs = stof(stack.back());
            stack.pop_back();
            stack.push_back(to_string(lhs+rhs));
        }
        else if (subtract == true) {
            float rhs = stof(stack.back());
            stack.pop_back();
            float lhs = stof(stack.back());
            stack.pop_back();
            stack.push_back(to_string(lhs-rhs));
        }
        else if (multiplication == true) {
            float rhs = stof(stack.back());
            stack.pop_back();
            float lhs = stof(stack.back());
            stack.pop_back();
            stack.push_back(to_string(lhs*rhs));
        }
        else if (division == true) {
            float rhs = stof(stack.back());
            stack.pop_back();
            float lhs = stof(stack.back());
            stack.pop_back();
            stack.push_back(to_string(lhs/rhs));
        }

        i = i + 1;
    }
}

bool isKeyword(string word) {
    for (string keyword: KEYWORDS) {
        if (word == keyword) {
            return true;
        }
    }
    if (word == ADDITION_SIGN || word == MINUS_SIGN || word == DIVISION_SIGN || word == MULTIPLICATION_SIGN || word == EQUAL_SIGN) {
        return true;
    }
    return false;
}

void output() {
    int i = 1;
    while (i < stack.size()) {
        bool backslash = false;
        for (char letter : stack.at(i)) {
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
        if (i != (stack.size()-1)) {
            cout << " ";
        }
        i = i + 1;
    }
}

void setVariable() {
    int i = 0;
    bool exists = false;
    for (string variable: variables) {
        if (variable == stack.at(1)) {
            exists = true;
            break;
        }
        i = i + 1;
    }
    if (exists) {
        values[i] = stack.at(2);
    }
    else {
        variables.push_back(stack.at(1));
        values.push_back(stack.at(2));
    }
}