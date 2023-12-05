#pragma once
#include <iostream>
#include <vector>
#include <unordered_set>

class Tokenizer
{
    public:
    enum Token_type{OPEN_BRACK, CLOSE_BRACK, OPERATOR, NUMBER, CHAR, STOP_SYMBOL};

    struct Token
    {
        Token_type type;
        char symbol;
        double value;
        std::string keyword;
    };
    
};

std::ifstream& operator>>(std::ifstream& is, Tokenizer::Token& t);
std::istream &operator>>(std::istream &is, Tokenizer::Token &t);
std::ostream& operator<<(std::ostream &out, const Tokenizer::Token &t);

class Base
{
    public:

    struct node
    {
        char key;
        double value;

        bool operator==(const node& other);

    };

    std::vector<node*> arr; 

    ~Base();
    
    void readFromFile(std::istream& is);

    void setCommandInterp(std::istream &is, char &next);

    void printCommandInterp(std::istream &is, char &next);

    int recEval(std::istream &in);

    int RPNeval(std::istream &in);
};




