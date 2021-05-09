#include <stdlib.h>
#include <string.h>

#include "lexer.h"

/* Lexer Definition */
struct Lexer
{
    const char* input;
    int position;
    int readPosition;
    char ch;
};

/* Private Function Signatures */
void readChar(Lexer*);
char peekChar(const Lexer*);
void skipWhitespace(Lexer*);
String* readIdentifier(Lexer*);
String* readNumber(Lexer*);
int isLetter(char);
int isDigit(char);

Lexer* mkLexer(const char* input)
{
    Lexer* l = malloc(sizeof(Lexer));

    l->position = 0;
    l->readPosition = 0;
    l->ch = '\0';
    l->input = input;

    readChar(l);

    return l;
}

void freeLexer(Lexer* l)
{
    if (!l)
        return;
    free(l);
}

void readChar(Lexer* l)
{
    if ((size_t)l->readPosition >= strlen(l->input))
        l->ch = '\0';
    else
        l->ch = l->input[l->readPosition];
    l->position = l->readPosition;
    ++l->readPosition;
}

char peekChar(const Lexer* l)
{
    if ((size_t)l->readPosition >= strlen(l->input))
        return '\0';
    else
        return l->input[l->readPosition];
}

void skipWhitespace(Lexer* l)
{
    while (l->ch == ' ' || l->ch == '\t' || l->ch == '\n' || l->ch == '\r')
        readChar(l);
}

#define TOKENIZE2(tokType, __literal)      \
    do                                     \
    {                                      \
        tok.type = tokType;                \
        tok.literal = mkString(__literal); \
    } while (0)
#define TOKENIZE(expected, tokType, __literal) \
    case expected:                             \
        TOKENIZE2(tokType, __literal);         \
        break;
#define TOKENIZE_BRANCH(expected, tokType, __literal, branch) \
    case expected:                                            \
        branch else TOKENIZE2(tokType, __literal);            \
        break;

Token nextToken(Lexer* l)
{
    Token tok;

    skipWhitespace(l);

    switch (l->ch)
    {
        TOKENIZE_BRANCH(
            '=', T_ASSIGN, "=",
            if (peekChar(l) == '=')
            {
                readChar(l);
                TOKENIZE2(T_EQ, "==");
            })
        TOKENIZE_BRANCH(
            '!', T_BANG, "!",
            if (peekChar(l) == '=')
            {
                readChar(l);
                TOKENIZE2(T_NOT_EQ, "!=");
            })
        TOKENIZE('+', T_PLUS, "+");
        TOKENIZE('-', T_MINUS, "-");
        TOKENIZE('*', T_ASTERISK, "*");
        TOKENIZE('/', T_SLASH, "/");
        TOKENIZE('<', T_LT, "<");
        TOKENIZE('>', T_GT, ">");
        TOKENIZE(',', T_COMMA, ",");
        TOKENIZE(';', T_SEMICOLON, ";");
        TOKENIZE('(', T_LPAREN, "(");
        TOKENIZE(')', T_RPAREN, ")");
        TOKENIZE('{', T_LBRACE, "{");
        TOKENIZE('}', T_RBRACE, "}");
        TOKENIZE(0, T_EOF, "");
    default:
        if (isLetter(l->ch))
        {
            tok.literal = readIdentifier(l);
            tok.type = lookupIdent(getStr(tok.literal));
            return tok;
        }
        if (isDigit(l->ch))
        {
            tok.literal = readNumber(l);
            tok.type = T_INT;
            return tok;
        }
        TOKENIZE2(T_ILLEGAL, "");
        appendChar(tok.literal, l->ch);
        break;
    }

    readChar(l);
    return tok;
}

String* readIdentifier(Lexer* l)
{
    int position = l->position;
    while (isLetter(l->ch))
        readChar(l);

    return mkNString(l->input + position, (size_t)(l->position - position));
}

String* readNumber(Lexer* l)
{
    int position = l->position;
    while (isDigit(l->ch))
        readChar(l);

    return mkNString(l->input + position, (size_t)(l->position - position));
}

int isLetter(char ch)
{
    return ('A' <= ch && ch <= 'Z') || ('a' <= ch && ch <= 'z') || ch == '_';
}

int isDigit(char ch) { return '0' <= ch && ch <= '9'; }
