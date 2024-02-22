#ifndef LEXER_H // conditional compilations(checks whether a macro is not defined)
#define LEXER_H

#include "llvm/ADT/StringRef.h"        // encapsulates a pointer to a C string and its length
#include "llvm/Support/MemoryBuffer.h" // read-only access to a block of memory, filled with the content of a file

class Lexer;

class Token
{
    friend class Lexer; // Lexer can access private and protected members of Token

public:
    enum TokenType : unsigned short
    
    {
        eoi,        // end of input
        unknown,    // in case of error at the lexical level

        KW_int,     // int
        
        KW_begin,   // begin
        KW_end,     // end
        
        KW_if,      // if
        KW_elif,    // elif
        KW_else,    // else
        
        KW_loopc,   // loopc

        KW_and,     // and
        KW_or,      // or
        KW_true,    // True
        KW_false    // False

        not_equal,  // !=
        ident,      // a
        number,     // 1
        comma,      // ,
        semicolon,  // ;
        l_paren,    // (
        r_paren,    // )
        power,      // ^
        star,       // *
        slash,      // /
        percent,    // %
        plus,       // +
        minus,      // -
        g_than,     // >
        l_than,     // <
        g_than_eq,  // >=
        l_than_eq,  // <=
        equality,   // ==
        equal,      // =
        add,        // +=
        multi,      // *=
        div,        // /=
        sub,        // -=
        left_over,  // %=
        colon,      // :
    };

private:
    TokenType Kind;
    llvm::StringRef Text; // points to the start of the text of the token

public:
    TokenType getKind() const { return Kind; }
    llvm::StringRef getText() const { return Text; }

    // to test if the token is of a certain kind
    bool is(TokenType K) const { return Kind == K; }
    bool isOneOf(TokenType K1, TokenType K2) const
    {
        return is(K1) || is(K2);
    }
    template <typename... Ts>
    bool isOneOf(TokenType K1, TokenType K2, Ts... Ks)
        const { return is(K1) || isOneOf(K2, Ks...); }
};

class Lexer
{
    const char *BufferStart; // pointer to the beginning of the input
    const char *BufferPtr;   // pointer to the next unprocessed character

public:
    Lexer(const llvm::StringRef &Buffer)
    {
        BufferStart = Buffer.begin();
        BufferPtr = BufferStart;
    }

    void next(Token &token); // return the next token

private:
    void formToken(Token &Result, const char *TokEnd, Token::TokenType Kind);
};
#endif