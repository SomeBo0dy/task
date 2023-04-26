//
// Created by 熊中伟 on 2023/4/25.
//

#ifndef PCP2_LEX_H
#define PCP2_LEX_H


#include <iostream>
#include <fstream>
#include <string>
#include <map>

using namespace std;

class Lex {
private:
    map<string, string> symbolMap = {
            {"break",    "key"},
            {"char",     "key"},
            {"do",       "key"},
            {"else",     "key"},
            {"float",    "key"},
            {"for",      "key"},
            {"if",       "key"},
            {"int",      "key"},
            {"while",    "key"},
            {"true",     "key"},
            {"false",    "key"},
            {"<",        "op_cmp"},
            {">",        "op_cmp"},
            {"<=",       "op_cmp"},
            {">=",       "op_cmp"},
            {"==",       "op_cmp"},
            {"!=",       "op_cmp"},
            {"=",        "assign"},
            {"+",        "op_comp"},
            {"-",        "op_comp"},
            {"*",        "op_comp"},
            {"/",        "op_comp"},
            {"&&",       "op_logic"},
            {"||",       "op_logic"},
            {"!",        "op_logic"},
            {"{",        "bound"},
            {"}",        "bound"},
            {"(",        "bound"},
            {")",        "bound"},
            {"[",        "bound"},
            {"]",        "bound"},
            {";",        "eos"},
            {"ID",       "ID"},
            {"NUM",      "NUM"},
            {"STR",      "STR"}
    };

    int syn = 0;

    string token;

    string target;

    int i = 0;

    int tag = 1;


    static bool IsLetter(char ch) {
        if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
            return true;
        else
            return false;

    }

    static bool IsDigit(char ch) {
        if (ch >= '0' && ch <= '9')
            return true;
        else
            return false;

    }

    void scan(string s) {
        if (s[i] == ' ') {
            syn = -2;
            i++;
        } else {
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
            } else if (IsLetter(s[i])) {

                token = "";
                while (IsDigit(s[i]) || IsLetter(s[i])) {
                    token += s[i];
                    i++;
                }


                target = symbolMap["ID"];

                if (!symbolMap[token].empty()) {
                    target = symbolMap[token];
                }
            } else {
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
                            } else {
                                token += s[i];
                                i++;
                            }
                        }
                        if (tag) {
                            token += s[i];
                            i++;
                            target = symbolMap["STR"];
                            break;
                        } else {
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

public:
    void outputResult(const string &sourcePath, const string &lexResultPath) {
        cout << "==开始词法分析==" << endl;
        string a;
        ifstream in(sourcePath);
        ofstream out;
        out.open(lexResultPath, ios::trunc);
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
            token = "";
            target = "";
            i = 0;
            tag = 1;
        }
        in.close();
        out.close();
        cout << "==已将源程序处理为单词二元组==" << endl;
    }
};


#endif //PCP2_LEX_H
