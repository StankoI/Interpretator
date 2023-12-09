#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"
#include "parser.hpp"
#include <string>
#include <fstream>
#include <sstream>
#include <cassert>
#include <functional>
#include <stack>
#include <set>

TEST_CASE("Test RPN Evaluation")
{
    std::vector<std::pair<std::string,int>> expressions = {{"8 1 + ;",9},
                                                           {"101 1 - 1 - ;",99},
                                                           {"1 2 + 3 4 + * ;",21},
                                                           {"1 1 + 2 3 + * 8 + ;",18},
                                                           {"21 2 * 41 - ;",1},
                                                           {"36 3 / ;",12},
                                                           {"1 6 * 6 /;",1}};    

    Base a;                                              
    for(std::pair<std::string,int> p : expressions)
    {
        std::stringstream e (p.first);
        CHECK(a.RPNeval(e) == p.second);
    }
}

TEST_CASE("Linking key with value")
{
    // std::ifstream is("test.stuck");

    // Base base;

    // base.readFromFile(is);

    // CHECK(base.arr[0]->value == 4);     //set a = 4;
    // CHECK(base.arr[1]->value == 10);    //set b = 6 + a;
    // CHECK(base.arr[2]->value == 17);    //set c = 7 + b;
    // CHECK(base.arr[3]->value == 12);    //set d = 8 + a;

    std::istringstream is( "set a = 5;");
    
    Base base;

    char n = is.peek();

    base.setCommandInterp(is,n);
}

TEST_CASE("hello world")
{
    std::istringstream is( "set h = '\"'HELLO WORLD'\"' ;");
    Base base;
    char n = is.peek();
    base.setCommandInterp(is,n);
}

int main()
{
    std::ifstream is("test.stuck");
    if (!is.is_open())
    {
        return 1;
    }

    Base a;

    a.readFromFile(is);

    // doctest::Context().run();


    return 0;
}