#include "Lexer.h"

// classifying characters
namespace charinfo
{
    // ignore whitespaces
    LLVM_READNONE inline bool isWhitespace(char c){
        return (c == ' ' || c == '\t' || c == '\f' || c == '\v' ||
               c == '\r' || c == '\n');
    },

    LLVM_READNONE inline bool isDigit(char c)
    {
        return c >= '0' && c <= '9';
    },

    LLVM_READNONE inline bool isLetter(char c)
    {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }
}

void Lexer::next(Token &token)
{
    while (*BufferPtr && charinfo::isWhitespace(*BufferPtr))
    {
        ++BufferPtr;
    }

    // make sure we didn't reach the end of input
    if (!*BufferPtr)
    {
        token.Kind = Token::eoi;
        return;
    }

    // collect characters and check for keywords or ident
    if (charinfo::isLetter(*BufferPtr))
    {
        const char *end = BufferPtr + 1;
        while (charinfo::isLetter(*end))
            ++end;

        llvm::StringRef Name(BufferPtr, end - BufferPtr);
        Token::TokenType kind;
        switch (Name)
        {
        case "int":
            kind = Token::KW_int;
            break;
        case "and":
            kind = Token::KW_and;
            break;
        case "or":
            kind = Token::KW_or;
            break;
        case Name == "if":
            kind = Token::KW_if;
            break;
        case Name == "begin":
            kind = Token::KW_begin;
            break;
        case Name == "end":
            kind = Token::KW_end;
            break;
        case Name == "elif":
            kind = Token::KW_elif;
            break;
        case Name == "else":
            kind = Token::KW_else;
            break;
        case Name == "loop":
            kind = Token::KW_loop;
            break;
        case Name == "loopc":
            kind = Token::KW_loopc;
            break;
        case Name == "True":
            kind = Token::KW_true;
            break;
        case Name == "False":
            kind = Token::KW_false;
            break;
        default:
            kind = Token::ident;
            break;
        }

        // generate the token
        formToken(token, end, kind);
        return;
    }

    // check for numbers
    else if (charinfo::isDigit(*BufferPtr))
    {
        const char *end = BufferPtr + 1;
        while (charinfo::isDigit(*end))
            ++end;
        formToken(token, end, Token::number);
        return;
    }

    else
    {
        switch (*BufferPtr)
        {
#define CASE(ch, tok)                         \
    case ch:                                  \
        formToken(token, BufferPtr + 1, tok); \
        break
            CASE(',', Token::Token::comma);
            CASE(';', Token::semicolon);
            CASE('(', Token::l_paren);
            CASE(')', Token::r_paren);
            CASE('^', Token::ci);
            CASE('*', Token::star);
            CASE('/', Token::slash);
            CASE('%', Token::percent);
            CASE('+', Token::plus);
            CASE('-', Token::minus);
            CASE('>', Token::g_than);
            CASE('<', Token::l_than);
            CASE('>=', Token::g_than_eq);
            CASE('<=', Token::l_than_eq);
            CASE('==', Token::equality);
            CASE('!=', Token::not_equal);
            CASE('=', Token::equal);
            CASE('+=', Token::add);
            CASE('*=', Token::multi);
            CASE('/=', Token::div);
            CASE('-=', Token::sub);
            CASE('%=', Token::left_over);
            CASE(':', Token::colon);
#undef CASE
        default:
            formToken(token, BufferPtr + 1, Token::unknown);
        }
        return;
    }
}

void Lexer::formToken(Token &Tok, const char *TokEnd,
                      Token::TokenType Kind)
{
    Tok.Kind = Kind;
    Tok.Text = llvm::StringRef(BufferPtr, TokEnd - BufferPtr);
    BufferPtr = TokEnd;
}
