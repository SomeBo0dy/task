//
// Created by 熊中伟 on 2023/4/25.
//

#ifndef PCP2_GRAM_H
#define PCP2_GRAM_H

#include <iostream>
#include <fstream>
#include <cstring>
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

std::string &trim(std::string &s) {
    if (s.empty()) {
        return s;
    }

    s.erase(0, s.find_first_not_of(' '));
    s.erase(s.find_last_not_of(' ') + 1);
    return s;
}


vector<string> split(const string &str, const string &splits) {
    vector<string> res;
    if (str.empty()) return res;
    string strs = str + splits;
    size_t pos = strs.find(splits);
    unsigned step = splits.size();
    while (pos != std::string::npos) {
        string temp = strs.substr(0, pos);
        res.push_back(temp);
        strs = strs.substr(pos + step, strs.size());
        pos = strs.find(splits);
    }
    return res;
}

class LR1 {
private:
    int number = 0;
    vector<string> T;//终结符号集合
    vector<string> NT;//非终结符号集合
    string S;//开始符号
    map<string, vector<string>> production;//产生式
    map<string, int> numPro;//编号的产生式集合，用于规约
    //set<State>States;//状态集合
    //vector<GOTO>GO;//转换函数
    map<string, set<string>> FIRST;//FIRST集
    map<string, set<string>> FOLLOW;//FOLLOW集
    map<pair<int, string>, string> actionTable;//action表
    map<pair<int, string>, int> gotoTable;//goto表

public:


    void readGrammar(const string &grammarFileName) {
        ifstream in_grammar(grammarFileName);
        if (!in_grammar) {
            cout << grammarFileName << "文件打开失败" << endl;
        }
        string line;
        while (getline(in_grammar, line)) {
            vector<string> vs_production = split(line, "->");
            string left = trim(vs_production[0]);//左部
            NT.push_back(left);
            string right = trim(vs_production[1]);
            vector<string> vs_right = split(right, " | ");
            for (string candidate: vs_right) {
                production[left].push_back(trim(candidate));
            }
        }
        addT();
        S = *NT.begin();
        in_grammar.close();
    }

    //拓广文法
    void broadeningGrammar() {
        string newS = S + "'";
        NT.insert(NT.begin(), newS);
        production[newS].push_back(S);
        S = newS;
    }

    void printBroadeningGrammar() {
        cout << "==拓广文法：==" << endl;
        for (auto &it: production) {
            for (int i = 0; i < it.second.size(); i++) {
                cout << it.first << "->" << it.second[i] << endl;
            }
        }
        cout << "===========" << endl;
    }


    //终结符
    void addT() {
        for (const string &left: NT) {
            for (const string &right: production[left]) {
                for (const string &unknownToken: split(right, " ")) {
                    if (find(NT.begin(), NT.end(), unknownToken) == NT.end()) {
                        T.push_back(unknownToken);
                    }
                }
            }
        }
        //终结符去重
        sort(T.begin(), T.end());
        T.erase(unique(T.begin(), T.end()), T.end());
    }


    void findFirst(const string &nt, const string &current) {
        for (const string &candidate: production[current]) {
            string first_token = split(candidate, " ")[0];
            if (find(T.begin(), T.end(), first_token) != T.end())
                FIRST[nt].insert(first_token);
            else if (current == first_token) findFirst(nt, split(candidate, " ")[1]);
            else findFirst(nt, first_token);
        }
    }

    //构造FIRST集
    void getFirst() {
        for (const string &nt: NT) {
            findFirst(nt, nt);
        }
    }

    void printFirst() {
        cout << "==FIRST集合为：==" << endl;
        cout.setf(ios::left);
        for (const string &nt: NT) {
            cout << setw(20) << nt;
            for (const string &first: FIRST[nt])
                cout << first << " ";
            cout << endl;
        }
        cout << "===========" << endl;

    }

    void getFollow() {
        FOLLOW[S].insert("#");
        for (string current: NT) {
            for (string nt: NT) {
                for (string candidate: production[nt]) {
                    vector<string> ts = split(candidate, " ");
                    if (find(ts.begin(), ts.end(), current) != ts.end())
                        for (int i = 0; i < ts.size(); i++) {
                            if (ts[i] == current) {
                                if (i == ts.size() - 1) FOLLOW[current].insert("#");
                                else if (find(T.begin(), T.end(), ts[i + 1]) != T.end()) {
                                    FOLLOW[current].insert(ts[i + 1]);
                                } else {
                                    for (string s: FIRST[ts[i + 1]]) {
                                        if (s != "ε")
                                            FOLLOW[current].insert(s);
                                    }
                                }
                            }
                        }
                }
            }
        }

    }

    void printFollow() {
        cout << "==FOLLOW集合为：==" << endl;
        cout.setf(ios::left);
        for (const string &nt: NT) {
            cout << setw(20) << nt;
            for (const string &first: FOLLOW[nt])
                cout << first << " ";
            cout << endl;
        }
        cout << "===========" << endl;

    }

    LR1(const string &grammar_file) {
        cout << "==开始语法分析==" << endl;
        readGrammar(grammar_file);// 解析文法文本。产生式按行读取，左部成为非终结符集，右部转为候选式向量，从两者中找到终结符集，并取得起始非终结符。
        broadeningGrammar();// 拓广文法，增加一个额外的起始符
        printBroadeningGrammar();
        getFirst();// 得到非终结符的First集。
        printFirst();
        getFollow();// 得到非终结符的Follow集
        printFollow();
    }

};


#endif //PCP2_GRAM_H
