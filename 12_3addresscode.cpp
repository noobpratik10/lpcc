#include <iostream>
#include <stack>
#include <vector>
#include <cctype>
using namespace std;

struct TAC
{
    string result, arg1, op, arg2;
};

int tempCount = 1;

// Helper to create temp variable name
string newTemp()
{
    return "t" + to_string(tempCount++);
}

// Check if a character is an operator
bool isOperator(char c)
{
    return c == '+' || c == '-' || c == '*' || c == '/';
}

// Operator precedence
int precedence(char op)
{
    if (op == '*' || op == '/')
        return 2;
    if (op == '+' || op == '-')
        return 1;
    return 0;
}

// Convert infix to postfix using Shunting Yard Algorithm
vector<string> infixToPostfix(string expr)
{
    stack<char> ops;
    vector<string> postfix;
    string operand = "";

    for (int i = 0; i < expr.length(); i++)
    {
        char c = expr[i];

        if (isalnum(c))
        {
            operand += c;
        }
        else
        {
            if (!operand.empty())
            {
                postfix.push_back(operand);
                operand = "";
            }

            if (c == '(')
            {
                ops.push(c);
            }
            else if (c == ')')
            {
                while (!ops.empty() && ops.top() != '(')
                {
                    postfix.push_back(string(1, ops.top()));
                    ops.pop();
                }
                ops.pop(); // pop '('
            }
            else if (isOperator(c))
            {
                while (!ops.empty() && precedence(ops.top()) >= precedence(c))
                {
                    postfix.push_back(string(1, ops.top()));
                    ops.pop();
                }
                ops.push(c);
            }
        }
    }

    if (!operand.empty())
        postfix.push_back(operand);

    while (!ops.empty())
    {
        postfix.push_back(string(1, ops.top()));
        ops.pop();
    }

    return postfix;
}

// Generate Three Address Code from postfix
vector<TAC> generateTAC(vector<string> postfix)
{
    stack<string> st;
    vector<TAC> code;

    for (auto token : postfix)
    {
        if (token == "+" || token == "-" || token == "*" || token == "/")
        {
            string arg2 = st.top();
            st.pop();
            string arg1 = st.top();
            st.pop();
            string temp = newTemp();
            code.push_back({temp, arg1, token, arg2});
            st.push(temp);
        }
        else
        {
            st.push(token);
        }
    }

    return code;
}

int main()
{
    string input;
    cout << "Enter expression (e.g. w = u*u - u*v + v*v): ";
    getline(cin, input);

    size_t eq = input.find('=');
    string lhs = input.substr(0, eq - 1); // variable name (e.g. w)
    string rhs = input.substr(eq + 2);    // actual expression

    vector<string> postfix = infixToPostfix(rhs);
    vector<TAC> code = generateTAC(postfix);

    for (auto line : code)
    {
        cout << line.result << " = " << line.arg1 << " " << line.op << " " << line.arg2 << endl;
    }

    cout << lhs << " = " << code.back().result << endl;
    return 0;
}
