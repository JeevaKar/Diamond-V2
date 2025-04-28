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
int setvar(vector<string> line);
int output(vector<string> line);
vector<string> variable(vector<string> line);

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

  for (vector<string> line : code) {
    if (line.front() == "var") {
        setvar(line=line);
    }
    else if (line.front() == "out") {
        output(line=line);
    }
  }

  // Close the file
  MyReadFile.close();
}

void printvector(vector<string> vec) {
    for (string item : vec) {
        cout << item << ", ";
    }
}

vector<string> variable(vector<string> line) {
    int i = 0;
    for (string token : line) {
        if (!token.empty() && token[0] == '"') {
            i = i + 1;
            continue;
        }
        else if (isNumber(token)) {
            i = i + 1;
            continue;
        }
        int j = 0;
        for (string variable : variables) {
            if (variable == token) {
                line[i] = values.at(j);
            }
            j = j + 1;
        }
        i = i + 1;
    }
    return line;
}

int setvar(vector<string> line) {
    int i = 0;
    for (string variable : variables) {
        if (variable == line.at(1)) {
            values[i] = line.at(2);
            return 0;
        }
        i = i + 1;
    }
    variables.push_back(line.at(1));
    values.push_back(line.at(2));
    return 0;
}

bool isNumber(const std::string s) {
    std::istringstream iss(s);
    float f;
    // Try to parse the string as a float and ensure there's no leftover content
    return (iss >> f) && (iss.eof());
}

int output(vector<string> line) {
    line = variable(line);
    bool i = false;
    for (string token : line) {
        if (i) {
            cout << token << "\n";
        }
        else {
            i = true;
        }
    }
    return 0;
}