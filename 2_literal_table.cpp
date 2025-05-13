#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <set>
#include <vector>
#include <iomanip>

using namespace std;

// Predefined sets
set<string> directives = {"START", "END", "DC", "DS", "LTORG", "EQU", "USING", "DROP"};
set<string> instructions = {"MOVER", "MOVEM", "ADD", "SUB", "MUL", "DIV", "COMP", "BC", "READ", "PRINT", "STOP"};
set<string> registers = {"AREG", "BREG", "CREG", "DREG"};
set<string> conditionCodes = {"LT", "LE", "EQ", "GT", "GE", "ANY"};

map<string, int> symbolTable;
map<string, int> literalTable;

int locationCounter = 0;

// Utility to trim spaces
string trim(const string &s)
{
    size_t start = s.find_first_not_of(" \t");
    size_t end = s.find_last_not_of(" \t");
    return (start == string::npos || end == string::npos) ? "" : s.substr(start, end - start + 1);
}

// Check if a string is a literal
bool isLiteral(const string &s)
{
    return s.size() >= 3 && s[0] == '=' && s[1] == '\'';
}

// Check if a string is a symbol
bool isSymbol(const string &s)
{
    return !s.empty() && !instructions.count(s) && !directives.count(s) &&
           !registers.count(s) && !conditionCodes.count(s) &&
           !isLiteral(s) && s != ",";
}

void processLine(const string &line)
{
    if (line.empty())
        return;

    stringstream ss(line);
    vector<string> tokens;
    string token;
    while (ss >> token)
    {
        tokens.push_back(trim(token));
    }

    if (tokens.empty())
        return;

    size_t idx = 0;

    // Handle START directive
    if (tokens[idx] == "START")
    {
        if (tokens.size() > idx + 1)
        {
            locationCounter = stoi(tokens[idx + 1]);
        }
        return;
    }

    string label = "";
    // If there's a label
    if (!instructions.count(tokens[idx]) && !directives.count(tokens[idx]))
    {
        label = tokens[idx++];
    }

    // Opcode
    if (idx >= tokens.size())
        return;
    string opcode = tokens[idx++];

    // Handle DS/DC
    if ((opcode == "DS" || opcode == "DC") && idx < tokens.size())
    {
        int size = stoi(tokens[idx]);
        if (!label.empty())
        {
            symbolTable[label] = locationCounter;
        }
        locationCounter += size;
        return;
    }

    // If there's a label, define it now
    if (!label.empty())
    {
        symbolTable[label] = locationCounter;
    }

    // Handle END
    if (opcode == "END")
        return;

    // Parse remaining operands
    while (idx < tokens.size())
    {
        string operand = tokens[idx++];
        if (operand == "," || operand.empty())
            continue;

        if (isLiteral(operand))
        {
            if (literalTable.find(operand) == literalTable.end())
            {
                literalTable[operand] = 0; // to be assigned later
            }
        }
    }

    // For most instructions
    locationCounter++;
}

int main()
{
    ifstream fin("3_input.asm");
    if (!fin)
    {
        cerr << "Cannot open input.asm file!" << endl;
        return 1;
    }

    string line;
    while (getline(fin, line))
    {
        processLine(line);
    }

    // Assign addresses to literals at the end
    for (auto &lit : literalTable)
    {
        lit.second = locationCounter++;
    }

    // Display SYMBOL TABLE
    cout << "\nSYMBOL TABLE\n";
    cout << left << setw(10) << "Symbol" << "Address\n";
    cout << "---------------------\n";
    for (const auto &sym : symbolTable)
    {
        cout << left << setw(10) << sym.first << sym.second << "\n";
    }

    // Display LITERAL TABLE
    cout << "\nLITERAL TABLE\n";
    cout << left << setw(10) << "Literal" << "Address\n";
    cout << "---------------------\n";
    for (const auto &lit : literalTable)
    {
        cout << left << setw(10) << lit.first << lit.second << "\n";
    }

    return 0;
}
