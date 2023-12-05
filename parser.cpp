#include "parser.hpp"
#include <string>
#include <fstream>
#include <sstream>
#include <cassert>
#include <functional>
#include <stack>
#include <set>

int priority(char op)
{
    switch (op)
    {
    case '+':
    case '-':
        return 10;
    case '*':
    case '/':
        return 20;
    default:
        assert(false);
    }
    return -1;
}

int applyOperator(char op, int left, int right)
{
    switch (op)
    {
    case '+':
        return left + right;
    case '-':
        return left - right;
    case '*':
        return left * right;
    case '/':
        return left / right;

    default:
        assert(false);
    }

    return -1;
}

std::function<int(int, int)> fn(char c)
{
    switch (c)
    {
    case '+':
        return [](int x, int y) -> int
        { return x + y; };
    case '-':
        return [](int x, int y) -> int
        { return x - y; };
    case '*':
        return [](int x, int y) -> int
        { return x * y; };
    case '/':
        return [](int x, int y) -> int
        { return x / y; };

    default:
        assert(false);
    }

    return [](int, int) -> int
    {assert(false);return -1; };
}

bool Base::node::operator==(const node& other)
{
    return this->key == other.key;
}

int Base::RPNeval(std::istream &in)
{
    Tokenizer::Token t;
    in >> t;

    std::stack<int> s;

    while (t.type != Tokenizer::STOP_SYMBOL)
    {
        assert(t.type == Tokenizer::NUMBER || t.type == Tokenizer::OPERATOR || t.type == Tokenizer::CHAR);
        if(t.type == Tokenizer::NUMBER)
        {
            s.push(t.value);
        } else if(t.type == Tokenizer::CHAR)   //!
        {                      
            bool found = 0;
                for (std::size_t i = 0; i < this->arr.size(); i++)
                {
                    if (t.symbol == this->arr[i]->key)
                    {
                        t.value = this->arr[i]->value;
                        found = 1;
                    }
                }
                if (found == 0)
                {
                    throw "undefined symbol";
                }
                else
                {
                    s.push(t.value);
                }                
            // s.push(t.value);                   
        }                                      
        else 
        { //t.type == Tokenizer::OPERATOR
            assert(s.size() > 1);
            int right = s.top(); s.pop();
            int left = s.top(); s.pop();
            s.push(fn(t.symbol)(left,right));
        }
        in >> t;
    }
    assert(s.size() == 1);
    return s.top();
}

std::string infixToRPN(std::istream &in)
{
    Tokenizer::Token t;
    in >> t;

    std::vector<Tokenizer::Token> output;
    std::stack<Tokenizer::Token> shuntingYard;

    while (t.type != Tokenizer::STOP_SYMBOL)
    {
        if(t.type == Tokenizer::NUMBER)
        {
            output.push_back(t);
        } else if(t.type == Tokenizer::CHAR)
        {
            output.push_back(t);
        }
        else if (t.type == Tokenizer::OPEN_BRACK)
        {
            shuntingYard.push(t);
        } else if (t.type == Tokenizer::CLOSE_BRACK)
        {
            assert(shuntingYard.size() > 0);
            while(shuntingYard.top().type != Tokenizer::OPEN_BRACK)
            {
                assert(shuntingYard.size() > 0);
                output.push_back(shuntingYard.top()); 
                shuntingYard.pop();
            }
            shuntingYard.pop();

        } else if (t.type == Tokenizer::OPERATOR)
        {
                while(shuntingYard.size() > 0 &&
                        shuntingYard.top().type != Tokenizer::OPEN_BRACK &&
                        priority(shuntingYard.top().symbol) >= priority(t.symbol))
                {
                    Tokenizer::Token waiting;
                    output.push_back(shuntingYard.top());
                    shuntingYard.pop();
                }
                shuntingYard.push(t);
        }

        in >> t;
    }

    while(shuntingYard.size() > 0)
    {
        output.push_back(shuntingYard.top());
        shuntingYard.pop();        
    }

    std::stringstream rpn;

    for(Tokenizer::Token t : output)
    {
        rpn << t << " ";
    }

    return rpn.str() + ";";
    
}

void SpaceRemover(std::istream &is, char &next)
{
    while (next == ' ' || next == '\n' || next == '\t')
    {
        is.get();
        next = is.peek();
    }
}

void Base::setCommandInterp(std::istream &is, char &next)
{
    SpaceRemover(is, next);
    Base::node *n = new Base::node;

    n->key = next;
    while (next != '=')
    {
        is.get();
        next = is.peek();
    }

    is.get();
    next = is.peek();
    std::string exp;
    getline(is, exp);

    std::stringstream expression(exp);
    // std::string res = infixToRPN(expression); //!
    std::stringstream RPexpression(infixToRPN(expression));  //!
    n->value = RPNeval(RPexpression);       //!
    // n->value = recEval(expression);
    this->arr.push_back(n);
}

void Base::printCommandInterp(std::istream &is, char &next)
{
    SpaceRemover(is, next);

    std::string exp;
    getline(is, exp);
    std::stringstream expression(exp);
    std::stringstream RPexpression(infixToRPN(expression));
    // std::stringstream expression(exp);
    std::cout << RPNeval(RPexpression) << '\n';
}

void Base::readFromFile(std::istream &is)
{
    while (is)
    {
        char next = is.peek();
        SpaceRemover(is, next);
        
        std::string command;
        is >> command;
        next = is.peek();

        if (command == "set")
        {
            this->setCommandInterp(is, next);
        }
        else if (command == "print")
        {
            this->printCommandInterp(is,next);   //! popravi printa da raboti s RPN 
        }
    }
}

std::ifstream &operator>>(std::ifstream &is, Tokenizer::Token &t)
{
    char next = is.peek();

    SpaceRemover(is, next);

    if (next == '(')
    {
        t.type = Tokenizer::OPEN_BRACK;
        t.symbol = next;
        is.get();
    }
    else if (next == ')')
    {
        t.type = Tokenizer::CLOSE_BRACK;
        t.symbol = next;
        is.get();
    }
    else if (next == '+' || next == '-' || next == '/' || next == '*')
    {
        t.type = Tokenizer::OPERATOR;
        t.symbol = next;
        is.get();
    }
    else if (next == ';')
    {
        t.type = Tokenizer::STOP_SYMBOL;
        t.symbol = next;
        is.get();
    }
    else if (next >= 'a' && next <= 'z')
    {
        std::cout << next;
        t.type = Tokenizer::CHAR;
        t.symbol = next;
        is.get();
    }
    else
    {
        if (!std::isdigit(next))
        {
            throw "Unexpected character type!";
        }
        t.type = Tokenizer::NUMBER;
        is >> t.value;
    }

    return is;
}

std::istream &operator>>(std::istream &is, Tokenizer::Token &t)
{
    char next = is.peek();

    SpaceRemover(is, next);

    if (next == '(')
    {
        t.type = Tokenizer::OPEN_BRACK;
        t.symbol = next;
        is.get();
    }
    else if (next == ')')
    {
        t.type = Tokenizer::CLOSE_BRACK;
        t.symbol = next;
        is.get();
    }
    else if (next == '+' || next == '-' || next == '/' || next == '*')
    {
        t.type = Tokenizer::OPERATOR;
        t.symbol = next;
        is.get();
    }
    else if (next == ';')
    {
        t.type = Tokenizer::STOP_SYMBOL;
        t.symbol = next;
        is.get();
    }
    else if (next >= 'a' && next <= 'z')
    {
        t.type = Tokenizer::CHAR;
        t.symbol = next;
        is.get();
    }
    else
    {
        if (!std::isdigit(next))
        {
            //! std::cout << next;
            throw "Unexpected character type!";

        }
        t.type = Tokenizer::NUMBER;
        is >> t.value;
    }

    return is;
}

std::ostream& operator<<(std::ostream &out, const Tokenizer::Token &t)
{
    switch(t.type)
    {
        case Tokenizer::NUMBER:
            out << t.value;
            break;
        case Tokenizer::OPERATOR:
        case Tokenizer::OPEN_BRACK:
        case Tokenizer::CLOSE_BRACK:
        case Tokenizer::STOP_SYMBOL:
        case Tokenizer::CHAR:
            out << t.symbol;
            break;
        default:
            out << t.keyword;
    }

    return out;
}

int Base::recEval(std::istream &in)
{
    Tokenizer::Token t;
    in >> t;

    if (t.type == Tokenizer::NUMBER)
    {
        return t.value;
    }

    if (t.type == Tokenizer::CHAR)
    {
        bool found = 0;
        for (std::size_t i = 0; i < this->arr.size(); i++)
        {
            if (t.symbol == this->arr[i]->key)
            {
                t.value = this->arr[i]->value;
                found = 1;
            }
        }
        if (found == 0)
        {
            throw "undefined symbol";
        }
        else
        {
            return t.value;
        }
    }

    assert(t.type == Tokenizer::OPEN_BRACK);

    int left = recEval(in);

    in >> t;
    assert(t.type == Tokenizer::OPERATOR);
    char op = t.symbol;

    int right = recEval(in);

    in >> t;
    assert(t.type == Tokenizer::CLOSE_BRACK);

    return fn(op)(left, right);
}



Base::~Base()
{
    for(std::size_t i = 0; i < this->arr.size(); i++)
    {
        delete arr[i];
    }
}



// int main()
// {
//     // Base a;

//     // std::stringstream expression("1+2*a;");

//     // std::string res = infixToRPN(expression);

//     // // std::cout << res;

//     // std::stringstream so(res);

//     // std::cout << a.RPNeval(so);


    


// }