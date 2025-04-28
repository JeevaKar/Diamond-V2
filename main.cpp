#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
using namespace std;

vector<string> variables;
vector<string> values;
vector<string> stack;

void printvector(vector<string> vec);
bool isNumber(const std::string s);
void evaluate(vector<string> line);

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
    evaluate(line=line);
    printvector(stack);
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
    // int i = 0;
    // for (string token) {
    //     if (!token.empty() && token[0] == '"') {
    //         i = i + 1;
    //         continue;
    //     }
    //     else if (isNumber(token)) {
    //         i = i + 1;
    //         continue;
    //     }
    //     int j = 0;
    //     for (string variable : variables) {
    //         if (variable == token) {
    //             stack[i] = values.at(j);
    //         }
    //         j = j + 1;
    //     }
    //     i = i + 1;
    // }
    int i = 0;
    bool add = false;
    for (string token : line) {
        stack.push_back(token);
        string s = "+";
        if (token == s) {
            add = true;
            stack.pop_back();
        }
        else if (add == true) {
            float rhs = stof(stack.back());
            stack.pop_back();
            float lhs = stof(stack.back());
            stack.pop_back();
            stack.push_back(to_string(rhs+lhs));
        }
    }
}