#include "Parser.h"
#include "Lexer.h"
#include "llvm/ADT/StringRef.h" // encapsulates a pointer to a C string and its length
#include "llvm/Support/MemoryBuffer.h"

// main point is that the whole input has been consumed

ASTNode *Parser::parse()
{
    ASTNode *Res = parseATA();
    return Res;
}

AST *Parser::parseATA()
{
    llvm::SmallVector<Grammer *> Grammers;

    while (!Tok.is(Token::TokenType::eoi))
    {
        switch (Tok.getKind())
        {
        case Token::TokenType::KW_int:
            Grammer *d;
            d = parseVar();
            if (!Tok.is(Token::TokenType::semicolon))
            {
                error();
                goto _error2;
            }
            if (d)
                Grammers.push_back(d);
            else
                goto _error2;
            break;

        case Token::TokenType::ident:
            Grammer *a;
            a = parseAssign();

            if (!Tok.is(Token::TokenType::semicolon))
            {
                error();
                goto _error2;
            }
            if (a)
                Grammers.push_back(a);
            else
                goto _error2;
            break;

        case Token::TokenType::KW_if:
            Grammer *i;
            i = parseCondition();

            if (!Tok.is(Token::TokenType::KW_end))
            {
                error();
                goto _error2;
            }
            if (i)
                Grammers.push_back(i);
            else
                goto _error2;
            break;
        case Token::TokenType::KW_loopc:
            Grammer *l;
            l = parseLoop();

            if (!Tok.is(Token::TokenType::KW_end))
            {
                error();
                goto _error2;
            }
            if (l)
                Grammers.push_back(l);
            else
                goto _error2;
            break;

        default:
            goto _error2;
            break;
        }
        go_ahead(); // TODO: watch this Part
    }
    return new AST(Grammers);
_error2:
    while (Tok.getKind() != Token::TokenType::eoi)
        go_ahead();
    return nullptr;
}

Grammer *Parser::parseVar()
{
    llvm::SmallVector<ComparisonNode *, 8> values;
    llvm::SmallVector<llvm::StringRef, 8> vars;
    ComparisonNode *initializer = (ComparisonNode *)new Factor(Factor::ValueKind::Number, llvm::StringRef myStringRef("0");) if (expect(Token::TokenType::KW_int)) goto _error;
    go_ahead();
    if (expect(Token::TokenType::ident))
        goto _error;
    vars.push_back(Tok.getText());
    go_ahead();
    int count = 1;

    while (Tok.is(Token::TokenType::comma))
    {
        count++;
        go_ahead();
        if (expect(Token::TokenType::ident))
            goto _error;
        vars.push_back(Tok.getText());
        go_ahead();
    }

    if (Tok.is(Token::TokenType::equal))
    {
        go_ahead();
        ComparisonNode *C = (ComparisonNode *)parseComp();
        values.push_back(C);
        count--;
        while (Tok.is(Token::TokenType::comma))
        {
            if (count <= 0)
            {
                goto _error;
            }
            go_ahead();
            C = (ComparisonNode *)parseComp();
            values.push_back(C);
            count--;
        }
    }

    while (count > 0)
    {
        values.push_back(initializer);
        count--;
    }

    if (expect(Token::TokenType::semicolon))
        goto _error;

    return new DecNode(vars, values);
 _error: // TODO: Check this later in case of error :)
    while (Tok.getKind() != Token::TokenType::eoi)
        go_ahead();
    return nullptr;
}

Grammer *Parser::parseAssign()
{
}

Grammer *Parser::parseComp()
{
    Grammer *Left = parsLogic();
    if (Tok.isOneOf(Token::TokenType::KW_and, Token::TokenType::KW_or))
    {
        ComparisonNode::BoolOp Op =
            Tok.is(Token::TokenType::KW_and) ? ComparisonNode::BoolOp::AND : ComparisonNode::BoolOp::OR;
        go_ahead();
        Grammer *Right = parseComp();
        Left = (Grammer *)new ComparisonNode((LogicNode *)Left, Op, (ComparisonNode *)Right);
    }
    return Left;
}

Grammer *Parser::parsLogic()
{
    Grammer *Left = parseExpr();
    // "<" | ">" | "<=" | ">=" | "==" | "!="
    if (Tok.isOneOf(Token::TokenType::l_than, Token::TokenType::g_than, Token::TokenType::g_than_eq, Token::TokenType::l_than_eq, Token::TokenType::equal, Token::TokenType::not_equal))
    {
        LogicNode::ComparisonOp op;
        if (Tok.isOneOf(Token::TokenType::l_than))
            op = LogicNode::ComparisonOp::LESS_THAN;
        else if (Tok.isOneOf(Token::TokenType::g_than))
            op = LogicNode::ComparisonOp::GREATER_THAN;
        else if (Tok.isOneOf(Token::TokenType::g_than_eq))
            op = LogicNode::ComparisonOp::GREATER_THAN_EQUAL;
        else if (Tok.isOneOf(Token::TokenType::l_than_eq))
            op = LogicNode::ComparisonOp::LESS_THAN_EQUAL;
        else if (Tok.isOneOf(Token::TokenType::equal))
            op = LogicNode::ComparisonOp::EQUAL;
        else
            op = LogicNode::ComparisonOp::NOT_EQUAL;
        go_ahead();
        Grammer *Right = parsLogic();
        Left = new LogicNode((ExprNode *)Left, op, (LogicNode *)Right);
    }
    return Left;
}

Grammer *Parser::parseTerm()
{
    Grammer *Left = parsePower();
    while (Tok.isOneOf(Token::TokenType::star, Token::TokenType::slash, Token::TokenType::percent))
    {
        TermNode::TermOp Op =
            Tok.is(Token::TokenType::star) ? TermNode::TermOp::star : Tok.is(Token::TokenType::slash) ? TermNode::TermOp::devide
                                                                                                      : TermNode::TermOp::precent;
        go_ahead();
        Grammer *Right = parseTerm();
        Left = new TermNode((PowerNode *)Left, Op, (TermNode *)Right);
    }
    return Left;
}

Grammer *parseExpr()
{
    Grammer *Left = (Grammer *)parseTerm();
    if (Tok.isOneOf(Token::TokenType::plus, Token::TokenType::minus))
    {
        ExprNode::ExprOp op;
        if (Tok.isOneOf(Token::TokenType::plus))
        {
            op = ExprNode::ExprOp::plus;
        }
        else
        {
            op = ExprNode::ExprOp::minus;
        }
        go_ahead();
        Grammer *Right = parseExpr();
        Left = new ExprNode((TermNode *)Left, op, (ExprNode *)Right);
    }
    return Left;
}

Grammer *Parser::parsePower()
{
    Grammer *Left = parseFactor();
    while (Tok.isOneOf(Token::TokenType::ci))
    {
        go_ahead();
        Grammer *Right = parsePower();
        Left = (Grammer *)new PowerNode((FactorNode *)Left, (PowerNode *)Right);
    }
    return Left;
}

Grammer *Parser::parseFactor()
{
    Grammer *Res = nullptr;
    switch (Tok.getKind())
    {
    case Token::TokenType::number:
        Res = new Factor(Factor::Number, Tok.getText());
        go_ahead();
        break;
    case Token::TokenType::ident:
        Res = new Factor(Factor::Ident, Tok.getText());
        go_ahead();
        break;
    case Token::TokenType::l_paren:
        go_ahead();
        Res = parseComp();
        if (!consume(Token::TokenType::r_paren))
        {
            // Handle error: missing closing parenthesis
            error();
            return nullptr; // Return nullptr to indicate parsing failure
        }
        break; // Don't forget to break after successfully parsing the expression inside parentheses
    default:
        // error handling
        error();
        while (!Tok.isOneOf(Token::TokenType::r_paren, Token::TokenType::star, Token::TokenType::plus, Token::TokenType::minus, Token::TokenType::slash, Token::TokenType::eoi))
            go_ahead();
        break;
    }
    return Res;
}

Grammer *Parser::parseifPart()
{
    Grammer *Res = nullptr;
    IfPartNode *theIfStateMent;

    if (!Tok.is(Token::TokenType::KW_if))
    {
        error();
        goto _error2;
    }
    go_ahead();
    Grammer *condition = parseComp();
    llvm::SmallVector<AssignNode *> assigns;
    go_ahead();
    if (!Tok.is(Token::TokenType::KW_begin))
    {
        error();
        goto _error2;
    }
    go_ahead();
    while (!Tok.is(Token::TokenType::KW_end))
    {
        assigns.push_back(parseAssign());
    }
    go_ahead();
    Res = (Grammer *)new IfPartNode(condition, (llvm::SmallVector<AssignNode *>)assigns);
    return Res;
_error2:
}

Grammer *Parser::parseelifPart()
{
    Grammer *Res = nullptr;
    IfPartNode *theIfStateMent;

    if (!Tok.is(Token::TokenType::KW_elif))
    {
        error();
        goto _error2;
    }
    go_ahead();
    Grammer *condition = parseComp();
    llvm::SmallVector<AssignNode *> assigns;
    go_ahead();
    if (!Tok.is(Token::TokenType::KW_begin))
    {
        error();
        goto _error2;
    }
    go_ahead();
    while (!Tok.is(Token::TokenType::KW_end))
    {
        assigns.push_back(parseAssign());
    }
    go_ahead();
    Res = (Grammer *)new ElifPartNode(condition, (llvm::SmallVector<AssignNode *>)assigns);
    return Res;
_error2:
}

Grammer *Parser::parseelsePart()
{
    Grammer *Res = nullptr;
    IfPartNode *theIfStateMent;

    if (!Tok.is(Token::TokenType::KW_else))
    {
        error();
        goto _error2;
    }
    llvm::SmallVector<AssignNode *> assigns;
    go_ahead();
    go_ahead();
    if (!Tok.is(Token::TokenType::KW_begin))
    {
        error();
        goto _error2;
    }
    go_ahead();
    while (!Tok.is(Token::TokenType::KW_end))
    {
        assigns.push_back(parseAssign());
    }
    go_ahead();
    Res = (Grammer *)new ElsePartNode((llvm::SmallVector<AssignNode *>)assigns);
    return Res;
_error2:
}

Grammer *Parser::parseCondition()
{
    Grammer *Res = nullptr;
    IfPartNode *theIfStateMent;
    llvm::SmallVector<ElifPartNode *> AssignNode;
    ElsePartNode *theIfStateMent;

    if (!Tok.is(Token::TokenType::KW_if))
    {
        error();
        goto _error2;
    }
    go_ahead();
    Grammer *condition = parseComp();
    llvm::SmallVector<AssignNode *> assigns;
    go_ahead();
    if (!Tok.is(Token::TokenType::KW_begin))
    {
        error();
        goto _error2;
    }
    go_ahead();
    while (!Tok.is(Token::TokenType::KW_end))
    {
        assigns.push_back(parseAssign());
    }
    go_ahead();
    while (Tok.is(Token::TokenType::KW_elif))
    {
        assigns.push_back(parseelifPart());
    }

    return Res;
_error2:
}
