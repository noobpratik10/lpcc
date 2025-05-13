#include <bits/stdc++.h>

using namespace std;

map<string, int> MNT;                  // Macro Name Table
vector<string> MDT;                    // Macro Definition Table
map<string, vector<string>> ALA_table; // Argument List Array
vector<string> expandedCode;           // Final Expanded Code

bool isMacroDef = false;
string currentMacro = "";

// Utility to trim spaces
string trim(const string &s)
{
    size_t start = s.find_first_not_of(" \t");
    size_t end = s.find_last_not_of(" \t");
    return (start == string::npos || end == string::npos) ? "" : s.substr(start, end - start + 1);
}

// Parse arguments from macro header
vector<string> parseArgs(const string &line)
{
    vector<string> args;
    stringstream ss(line);
    string token;
    ss >> token; // skip macro name
    while (ss >> token)
    {
        if (token.back() == ',')
            token.pop_back();
        args.push_back(token);
    }
    return args;
}

// Replaces actual arguments in macro body using ALA
string replaceArgs(const string &line, const vector<string> &ALA)
{
    stringstream ss(line), result;
    string word;
    while (ss >> word)
    {
        for (size_t i = 0; i < ALA.size(); ++i)
        {
            if (word == ALA[i])
            {
                word = "#" + to_string(i + 1); // Replace with position
                break;
            }
        }
        result << word << " ";
    }
    return trim(result.str());
}

// Expand macro by replacing formal args with actual args
void expandMacro(string macroCall)
{
    stringstream ss(macroCall);
    string macroName, arg;
    ss >> macroName;

    vector<string> actualArgs;
    while (ss >> arg)
    {
        if (arg.back() == ',')
            arg.pop_back();
        actualArgs.push_back(arg);
    }

    int mdtIndex = MNT[macroName];
    vector<string> formalArgs = ALA_table[macroName];

    for (int i = mdtIndex; i < MDT.size() && MDT[i] != "MEND"; ++i)
    {
        string line = MDT[i];
        stringstream ls(line);
        string word;
        string newLine;

        while (ls >> word)
        {
            if (word[0] == '#')
            {
                int idx = stoi(word.substr(1));
                if (idx < actualArgs.size())
                    word = actualArgs[idx];
            }
            newLine += word + " ";
        }
        expandedCode.push_back(trim(newLine));
    }
}

int main()
{
    ifstream fin("macro_input.asm");
    if (!fin)
    {
        cerr << "Cannot open input file!" << endl;
        return 1;
    }

    string line;
    while (getline(fin, line))
    {
        line = trim(line);
        if (line.empty())
            continue;

        stringstream ss(line);
        string firstWord;
        ss >> firstWord;

        if (firstWord == "MACRO")
        {
            isMacroDef = true;
            getline(fin, line);
            line = trim(line);
            stringstream defStream(line);
            defStream >> currentMacro;

            vector<string> args = parseArgs(line);
            ALA_table[currentMacro] = args;

            MNT[currentMacro] = MDT.size(); // MDT index
            continue;
        }

        if (line == "MEND")
        {
            MDT.push_back("MEND");
            isMacroDef = false;
            currentMacro = "";
            continue;
        }

        if (isMacroDef)
        {
            // Replace formal args with #n
            string replaced = replaceArgs(line, ALA_table[currentMacro]);
            MDT.push_back(replaced);
        }
        else
        {
            // Check if macro call
            stringstream check(line);
            string macroCall;
            check >> macroCall;
            if (MNT.count(macroCall))
            {
                expandMacro(line);
            }
            else
            {
                expandedCode.push_back(line);
            }
        }
    }

    fin.close();

    // Output tables and final expanded code
    cout << "\n--- MACRO NAME TABLE (MNT) ---\n";
    for (const auto &[name, idx] : MNT)
        cout << name << " -> MDT Index: " << idx << "\n";

    cout << "\n--- MACRO DEFINITION TABLE (MDT) ---\n";
    for (int i = 0; i < MDT.size(); ++i)
        cout << i << ": " << MDT[i] << "\n";

    cout << "\n--- EXPANDED CODE ---\n";
    for (const auto &line : expandedCode)
        cout << line << "\n";

    return 0;
}
