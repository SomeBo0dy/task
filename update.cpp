#include <iostream>
#include <fstream>
#include <string.h>
#include <string>
#include <cstring>
#include <algorithm>
#include <set>
#include <map>
#include <stack>
#include <queue>
#include <utility>
#include <iomanip>
#include <codecvt>
using namespace std;

std::string& trim(std::string&);

map<string, string> symbolMap = {
        {"auto",     "key"},
        {"break",    "key"},
        {"case",     "key"},
        {"char",     "key"},
        {"const",    "key"},
        {"continue", "key"},
        {"default",  "key"},
        {"do",       "key"},
        {"double",   "key"},
        {"else",     "key"},
        {"enum",     "key"},
        {"extern",   "key"},
        {"float",    "key"},
        {"for",      "key"},
        {"goto",     "key"},
        {"if",       "key"},
        {"int",      "key"},
        {"long",     "key"},
        {"register", "key"},
        {"return",   "key"},
        {"short",    "key"},
        {"signed",   "key"},
        {"sizeof",   "key"},
        {"static",   "key"},
        {"struct",   "key"},
        {"switch",   "key"},
        {"typedef",  "key"},
        {"union",    "key"},
        {"unsigned", "key"},
        {"void",     "key"},
        {"volatile", "key"},
        {"while",    "key"},
        {"true",     "key"},
        {"false",    "key"},
        {"<",        "relop"},
        {">",        "relop"},
        {"<=",       "relop"},
        {">=",       "relop"},
        {"==",       "relop"},
        {"!=",       "relop"},
        {"=",        "赋值"},
        {"+=",       "赋值"},
        {"-=",       "赋值"},
        {"*=",       "赋值"},
        {"/=",       "赋值"},
        {"%=",       "赋值"},
        {"<<=",      "赋值"},
        {">>=",      "赋值"},
        {"&=",       "赋值"},
        {"^=",       "赋值"},
        {"|=",       "赋值"},
        {"+",        "算术op"},
        {"-",        "算术op"},
        {"*",        "算术op"},
        {"/",        "算术op"},
        {"++",       "算术op"},
        {"--",       "算术op"},
        {"%",        "算术op"},
        {"|",        "位op"},
        {"&",        "位op"},
        {"<<",       "位op"},
        {">>",       "位op"},
        {"^",        "位op"},
        {"~",        "位op"},
        {"&&",       "逻辑op"},
        {"||",       "逻辑op"},
        {"!",        "逻辑op"},
        {"{",        "界符"},
        {"}",        "界符"},
        {"<",        "界符"},
        {">",        "界符"},
        {"(",        "界符"},
        {")",        "界符"},
        {"[",        "界符"},
        {"]",        "界符"},
        {"/*",       "界符"},
        {"*/",       "界符"},
        {"\"",       "界符"},
        {"'",        "界符"},
        {"//",       "界符"},
        {";",        "标点"},
        {",",        "标点"},
        {":",        "单独一类"},
        {"ID",       "标识符"},
        {"NUM",      "数值常量"},
        {"STR",      "字符串常量"}
};

int syn = 0;

string token;

string target;

int i = 0;

int tag = 1;

std::string& trim(std::string& s)
{
    if (s.empty())
    {
        return s;
    }

    s.erase(0, s.find_first_not_of(" "));
    s.erase(s.find_last_not_of(" ") + 1);
    return s;
}

bool IsLetter(char ch);

bool IsDigit(char ch);

void scan(string s);

void lex(string input_file, string output_file);

class LR1 {
private:
    int number = 0;
    vector<string>T;//终结符号集合
    vector<string>NT;//非终结符号集合
    string S;//开始符号
    map<string, vector<string>>production;//产生式
    map<string, int>numPro;//编号的产生式集合，用于规约
    //set<State>States;//状态集合
    //vector<GOTO>GO;//转换函数
    map<string, set<string>>FIRST;//FIRST集
    map<string, set<string>>FOLLOW;//FOLLOW集
    map<pair<int, string>, string>actionTable;//action表
    map<pair<int, string>, int>gotoTable;//goto表

public:
    LR1(string grammar_file) {
        readGrammar(grammar_file);
    }
    map<string, vector<string>> get() {
        return production;
    }
    void readGrammar(string grammarFileName) {
        ifstream in_grammar(grammarFileName);
        if (!in_grammar)
        {
            cout << grammarFileName << "文件打开失败" << endl;
        }
        string line;
        while (getline(in_grammar, line)) {
            int i = 0;
            string left = "";//左部
            for (i = 0; line[i] != '_' && i < line.size(); i++)
            {
                left += line[i];
            }
            left = trim(left);
            NT.push_back(left);
            string right = line.substr(i + 1, line.size() - 1);
            right = trim(right);
            //cout << left << " → " << right << endl;
            addProduction(left, right);
        }
        addT();
        S = *NT.begin();
        in_grammar.close();

    }
    //产生式
    void addProduction(string left, string right) {
        right += "#";
        string pRight = "";
        for (int i = 0; i < right.size(); i++)
        {
            if (right[i] == '|'&&right[i+1] !='|'&&right[i-1]!='|' || right[i] == '#')
            {
                pRight = trim(pRight);
                production[left].push_back(pRight);
                pRight = "";
            }
            else
            {
                pRight += right[i];
            }
        }
    }
    //带标号的产生式集
    void addNumProduction()
    {
        int i = 0;
        for (string left : NT)
        {
            for (string right : production[left])
            {
                numPro[left + "->" + right] = i;
                i++;
            }
        }
    }
    //终结符
    void addT() {
        for (string left : NT)
        {
            for (string right : production[left])
            {
                //查看是否在产生式左边出现过
                if (find(NT.begin(), NT.end(), right) == NT.end())
                {
                    T.push_back(right);
                }
            }
        }
        //终结符去重
        sort(T.begin(), T.end());
        T.erase(unique(T.begin(), T.end()), T.end());
    }
    //判断是否是非终结符号
    bool isNT(string token)
    {
        if (find(NT.begin(), NT.end(), token) != NT.end())
            return true;
        return false;
    }
    //构造FIRST集
    void getFirst() {
        FIRST.clear();
        //终结符
        for (string t : T)
        {
            FIRST[t].insert(t);
        }
        //非终结符

    }

};


int main() {
    string input_file = "C:\\Users\\97908\\Desktop\\task\\input.txt";
    string output_file = "C:\\Users\\97908\\Desktop\\task\\output.txt";
    string grammar_file = "C:\\Users\\97908\\Desktop\\task\\grammar.txt";
    lex(input_file, output_file);
    LR1 grammar(grammar_file);
    map<string, vector<string>> p = grammar.get();
    for (map<string, vector<string>>::iterator it = p.begin();
        it != p.end(); it++) {
        for (int i = 0; i < it->second.size(); i++)
        {
            cout << it->first << "->" << it->second[i] << endl;
        }
    }

}

void lex(string input_file, string output_file) {
    string a;
    ifstream in(input_file);
    ofstream out;
    out.open(output_file, ios::trunc);
    while (getline(in, a)) {
        do {
            syn = 0;
            scan(a);
            switch (syn) {
            case -1:
                out << "error：非法字符" << endl;
                break;
            case -3:
                out << "error:非法常量" << endl;
                break;
            case -2:
                break;
            default:
                out << "< " << token << "," << target << " >" << endl;
                break;
            }
        } while (i < a.length());
        out << "================" << endl;
        token = "";
        target = "";
        i = 0;
        tag = 1;
    }
    cout << "处理完成" << endl;
    in.close();
    out.close();
}

bool IsLetter(char ch) {
    if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
        return true;
    else
        return false;

}

bool IsDigit(char ch) {
    if (ch >= '0' && ch <= '9')
        return true;
    else
        return false;

}

void scan(string s) {
    if (s[i] == ' ') {
        syn = -2;
        i++;
    }
    else {
        token = "";

        if (IsDigit(s[i]) || s[i] == '.') {
            int dotCnt = 0;
            token = "";
            string builder;
            while (IsDigit(s[i]) || s[i] == '.') {
                if (s[i] == '.') dotCnt++;
                if (dotCnt >= 2) {
                    syn = -3;
                    break;
                }
                builder += s[i];
                i++;
                target = symbolMap["NUM"];
            }
            token += builder;
        }
        else if (IsLetter(s[i])) {

            token = "";
            while (IsDigit(s[i]) || IsLetter(s[i])) {
                token += s[i];
                i++;
            }


            target = symbolMap["ID"];

            if (!symbolMap[token].empty()) {
                target = symbolMap[token];
            }
        }
        else {
            token = "";
            switch (s[i]) {
            case '=':
                i++;
                token = "=";
                target = symbolMap[token];
                if (s[i] == '=') {
                    i++;
                    token = "==";
                    target = symbolMap[token];
                }
                break;

            case '+':
                i++;
                token = "+";
                target = symbolMap[token];
                if (s[i] == '=') {
                    i++;
                    token = "+=";
                    target = symbolMap[token];
                }
                if (s[i] == '+') {
                    i++;
                    token = "++";
                    target = symbolMap[token];
                }
                break;

            case '-':
                i++;
                token = "-";
                target = symbolMap[token];
                if (s[i] == '=') {
                    i++;
                    token = "-=";
                    target = symbolMap[token];
                }
                if (s[i] == '-') {
                    i++;
                    token = "--";
                    target = symbolMap[token];
                }
                break;

            case '*':
                i++;
                token = "*";
                target = symbolMap[token];
                if (s[i] == '=') {
                    i++;
                    token = "*=";
                    target = symbolMap[token];
                }
                if (s[i] == '/') {
                    i++;
                    token = "*/";
                    target = symbolMap[token];
                }
                break;

            case '/':
                i++;
                token = "/";
                target = symbolMap[token];
                if (s[i] == '=') {
                    i++;
                    token = "/=";
                    target = symbolMap[token];
                }
                if (s[i] == '/') {
                    i++;
                    token = "//";
                    target = symbolMap[token];
                }
                if (s[i] == '*') {
                    i++;
                    token = "/*";
                    target = symbolMap[token];
                }
                break;

            case '(':
                i++;
                token = "(";
                target = symbolMap[token];
                break;

            case ')':
                i++;
                token = ")";
                target = symbolMap[token];
                break;

            case '[':
                i++;
                token = "[";
                target = symbolMap[token];
                break;

            case ']':
                i++;
                token = "]";
                target = symbolMap[token];
                break;

            case '{':
                i++;
                token = "{";
                target = symbolMap[token];
                break;

            case '}':
                i++;
                token = "}";
                target = symbolMap[token];
                break;

            case ',':
                i++;
                token = ",";
                target = symbolMap[token];
                break;

            case ':':
                i++;
                token = ":";
                target = symbolMap[token];
                break;

            case ';':
                i++;
                token = ";";
                target = symbolMap[token];
                break;

            case '>':
                i++;
                token = ">";
                target = symbolMap[token];
                if (s[i] == '=') {
                    i++;
                    token = ">=";
                    target = symbolMap[token];
                }
                break;

            case '<':
                i++;
                token = "<";
                target = symbolMap[token];
                if (s[i] == '=') {
                    i++;
                    token = "<=";
                    target = symbolMap[token];
                }
                break;

            case '!':
                i++;
                target = symbolMap[token];
                if (s[i] == '=') {
                    i++;
                    token = "!=";
                    target = symbolMap[token];
                }
                break;

            case '"':
                token += s[i];
                i++;
                while (s[i] != '"') {
                    if (i >= s.length()) {
                        tag = 0;
                        break;
                    }
                    else {
                        token += s[i];
                        i++;
                    }
                }
                if (tag) {
                    token += s[i];
                    i++;
                    target = symbolMap["STR"];
                    break;
                }
                else {
                    syn = -3;
                    break;
                }

            case '|':
                i++;
                target = symbolMap[token];
                if (s[i] == '|') {
                    i++;
                    token = "||";
                    target = symbolMap[token];
                }
                break;
            case '&':
                i++;
                target = symbolMap[token];
                if (s[i] == '&') {
                    i++;
                    token = "&&";
                    target = symbolMap[token];
                }
                break;
            default:
                syn = -1;
                i++;
                break;
            }
        }
    }


}
