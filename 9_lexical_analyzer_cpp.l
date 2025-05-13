%{
    #include <stdio.h>
%}

%%
    // keywords
    "int"|"printf"|"char"|"float" { printf("%s -> keywords\n", yytext);}
    // Identifiers (variablee names)
    [a-zA-Z_][a-zA-Z0-9_]*        { printf("%s -> identifiers\n", yytext); }
    // numbers
    [0-9]+                        { printf("%s -> numbers\n", yytext); }
    // operators
    "-"|"+"|"="                   { printf("%s -> operators\n", yytext); }
    // Punctuations
    [.!?,]+                   { printf("%s -> Punctuations\n",yytext);}
    // whitespaces
    [\t\n\r] + {/* Ignore white spaces */} // unknowns
    [a - zA - Z] +            { printf("%s -> Unknowns\n", yytext);}
    
%%

int main()
{
    printf("Hello World\n");
    yylex(); // start lexical analysis
    return 0;
}

    // flex my_first_lexer.l
    // gcc lex.yy.c -o my_first_lexer.l -ll
    // ./my_first_lexer