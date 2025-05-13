#include <bits/stdc++.h>

using namespace std;

// Predefined sets
set<string> directives = {"START", "END", "DC", "DS", "LTORG", "EQU", "USING", "DROP"};
set<string> instructions = {"MOVER", "MOVEM", "ADD", "SUB", "MUL", "DIV", "COMP", "BC", "READ", "PRINT", "STOP"};
set<string> registers = {"AREG", "BREG", "CREG", "DREG"};
set<string> conditionCodes = {"LT", "LE", "EQ", "GT", "GE", "ANY"};

map<string, int> symbolTable;
map<string, int> literalTable;

// pool table ds
vector<int> poolTable;
vector<string> literalList;
set<string> pendingLiterals;

// opcode table
map<string, pair<string, int>> opcodeTable = {
    {"START", {"AD", 1}},
    {"END", {"AD", 2}},
    {"LTORG", {"AD", 5}},
    {"DS", {"DL", 1}},
    {"DC", {"DL", 2}},
    {"READ", {"IS", 10}},
    {"PRINT", {"IS", 11}},
    {"STOP", {"IS", 0}},
    {"MOVER", {"IS", 1}},
    {"MOVEM", {"IS", 2}},
    {"ADD", {"IS", 3}},
    {"SUB", {"IS", 4}},
    {"MUL", {"IS", 5}},
    {"DIV", {"IS", 6}},
    {"COMP", {"IS", 7}},
    {"BC", {"IS", 8}}};

// intermediate Code
vector<string> intermediateCode;

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

    // Handle LTORG and END
    if (opcode == "LTORG" || opcode == "END")
    {
        if (!pendingLiterals.empty())
        {
            poolTable.push_back(literalList.size() + 1); // 1 based indexing
            for (auto &literal : pendingLiterals)
            {
                if (literalTable[literal] == 0)
                {
                    literalTable[literal] = locationCounter++;
                    literalList.push_back(literal);
                }
            }
            pendingLiterals.clear();
        }
        return;
    }

    // Parse remaining operands
    int opIndex = idx;
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
                pendingLiterals.insert(operand);
            }
        }
    }

    // IC generation
    ostringstream icLine;
    if (opcodeTable.find(opcode) != opcodeTable.end())
    {
        auto [cls, opcodeNum] = opcodeTable[opcode];
        icLine << "(" << cls << "," << setw(2) << setfill('0') << opcodeNum << ")";

        while (opIndex < tokens.size())
        {
            string operand = tokens[opIndex++];
            if (operand == "," || operand.empty())
            {
                continue;
            }

            if (registers.count(operand))
            {
                icLine << "(" << operand[0] << ")";
            }
            else if (isLiteral(operand))
            {
                int litIndex = find(literalList.begin(), literalList.end(), operand) - literalList.begin() + 1;
                if (litIndex == literalList.size())
                {
                    literalList.push_back(operand);
                    litIndex = literalList.size() - 1;
                }
                icLine << "(L," << litIndex + 1 << ")";
            }
            else if (symbolTable.count(operand))
            {
                int symIndex = distance(symbolTable.begin(), symbolTable.find(operand));
                icLine << "(S," << symIndex + 1 << ")";
            }
            else if (isSymbol(operand))
            {
                symbolTable[operand] = -1;
                int symIndex = distance(symbolTable.begin(), symbolTable.find(operand));
                icLine << "(S," << symIndex + 1 << ")";
            }
        }
        intermediateCode.push_back(icLine.str());
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

    // Display Pool Table
    cout << "\n POOL TABLE\n";
    cout << left << setw(10) << "Pool#" << "Literal Index\n";
    cout << "---------------------\n";
    for (int i = 0; i < poolTable.size(); ++i)
    {
        cout << left << setw(10) << (i + 1) << poolTable[i] << "\n";
    }

    // IC Code
    cout << "\nINTERMEDIATE CODE\n";
    for (const auto &line : intermediateCode)
    {
        cout << line << "\n";
    }

    return 0;
}
