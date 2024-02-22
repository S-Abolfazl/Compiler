#ifndef AST_H
#define AST_H

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"

// Forward declarations of classes used in the AST
// AST class serves as the base class for all AST nodes
class AST;
class ASTVisitor;
class Grammer;
class DecNode;
class AssignNode;
class ComparisonNode;
class LogicNode;
class ExprNode;
class FactorNode;
class BinaryOp;
class ConditionNode;
class IfPartNode;
class ElifPartNode;
class ElsePartNode;
class LoopNode;
class GrammerNode;
class TermNode;
class PowerNode;

class AST
{
public:
  virtual ~AST() {}
  virtual void accept(ASTVisitor &V) = 0; // Accept a visitor for traversal
};

class ASTVisitor
{
public:
  // Virtual visit functions for each AST node type
  virtual void visit(AST &) {}               // Visit the base AST node
  virtual void visit(Expr &) {}              // Visit the expression node
  virtual void visit(GSM &) = 0;             // Visit the group of expressions node
  virtual void visit(Factor &) = 0;          // Visit the factor node
  virtual void visit(BinaryOp &) = 0;        // Visit the binary operation node
  virtual void visit(Assignment &) = 0;      // Visit the assignment expression node
  virtual void visit(Declaration &) = 0;     // Visit the variable declaration node
};

class Grammer
{
public:
  virtual ~Grammer() {}
  virtual void accept(ASTVisitor &V) = 0; // Accept a visitor for traversal
};

class DecNode : public Grammer
{
public:
  DecNode(
      const llvm::SmallVector<llvm::StringRef> identifiers,
      const llvm::SmallVector<ComparisonNode *> expressions) : identifiers(identifiers), expressions(expressions) {}

  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }

  void maker(DecNode &decNode)
  {
    // Implementation for creating a declaration node
    // ...
  }

  llvm::SmallVector<llvm::StringRef> identifiers;
  llvm::SmallVector<ComparisonNode *> expressions;
};

class AssignNode : public Grammer
{
  enum Token
  {
    EQUAL,
    PLUS_EQUAL,
    MINUS_EQUAL,
    MULT_EQUAL,
    DIVIDE_EQUAL
    MOD_EQUAL
  };

public:
  AssignNode(Factor *identifier, Token op, ComparisonNode *expression) : identifier(identifier), op(op), expression(expression){}
  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }

  void Grammer::maker(AssignNode &assignNode)
  {
    // Implementation for creating an assignment node
    // ...
  }

private:
  llvm::StringRef identifier;
  Token op; // "=", "+=", "-=", "*=", "/="
  ComparisonNode *expression;
};

// Now, let's implement the corresponding maker functions in your Grammer class:

class ComparisonNode : public ASTNode
{
public:
  enum BoolOp
  {
    AND,
    OR
  };

  ComparisonNode(
      LogicNode *left,
      BoolOp operation,
      ComparisonNode *right)
      : left(left), operation(operation), right(right) {}

  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }

  static ComparisonNode *maker(LogicNode *left, BoolOp operation, ComparisonNode *right)
  {
    return new ExprNode(left, operation, right);
  }

private:
  LogicNode *left;
  BoolOp operation;
  ComparisonNode *right;
};

class LogicNode : public ComparisonNode
{
public:
  enum ComparisonOp
  {
    LESS_THAN,
    GREATER_THAN,
    LESS_THAN_EQUAL,
    GREATER_THAN_EQUAL,
    EQUAL,
    NOT_EQUAL
  };

  LogicNode(
      ExprNode *ExprNode,
      ComparisonOp operation,
      LogicNode *LogicNode)
      : ExprNode(ExprNode), operation(operation), logicNode(LogicNode) {}

  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }

private:
  ExprNode *ExprNode;
  ComparisonOp operation;
  LogicNode *logicNode;
};

class ExprNode : public LogicNode
{
public:
  enum ExprOp
  {
    plus,
    minus
  };

  LogicNode(
      ExprNode *left,
      ExprOp operation,
      ExprNode *right)
      : left(left), operation(operation), right(right) {}

  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }

private:
  TermNode *left;
  ExprOp operation;
  ExprNode *right;
};

class TermNode : public ExprNode
{
public:
  enum TermOp
  {
    star,
    devide,
    precent
  };

  LogicNode(
      PowerNode *left,
      TermOp operation,
      TermNode *right)
      : left(left), operation(operation), right(right) {}

  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }

private:
  PowerNode *left;
  TermOp operation;
  TermNode *right;
};

class PowerNode : public TermNode
{
public:
  PowerNode(
      FactorNode *left,
      PowerNode *right)
      : left(left), right(right) {}

  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }

private:
  FactorNode *left;
  PowerNode *right;
};

class BinaryOp : public ExprNode
{
  // Represents begin-end block node
public:
  enum Operator
  {
    Plus,
    Minus,
    Mul,
    Div,
    power,
    mod,
  };

private:
  ExprNode *Left;  // Left-hand side expression
  ExprNode *Right; // Right-hand side expression
  Operator Op;     // Operator of the binary operation

public:
  BinaryOp(Operator Op, ExprNode *L, ExprNode *R) : Op(Op), Left(L), Right(R) {}

  ExprNode *getLeft() { return Left; }

  ExprNode *getRight() { return Right; }

  Operator getOperator() { return Op; }

  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }
};

class Factor : public Expr
{
public:
  enum ValueKind
  {
    Ident,
    Number
  };

private:
  ValueKind Kind;      // Stores the kind of factor (identifier or number)
  llvm::StringRef Val; // Stores the value of the factor

public:
  Factor(ValueKind Kind, llvm::StringRef Val) : Kind(Kind), Val(Val) {}

  ValueKind getKind() { return Kind; }

  llvm::StringRef getVal() { return Val; }

  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }
};

class ConditionNode : public ASTNode
{
  // Represents condition node
public:
  IfPartNode *ifPart;
  llvm::SmallVector<ElifPartNode *> elifParts;
  ElsePartNode *elseParts;
};

class IfPartNode : public ASTNode
{
  // Represents if Part node
public:
  ComparisonNode *condition;
  llvm::SmallVector<AssignNode *> assigns;

  IfPartNode(ComparisonNode *condition, llvm::SmallVector<AssignNode *> assigns) : condition(condition), assigns(assigns) {}

  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }
};

class ElifPartNode : public ASTNode
{
  // Represents elif Part node
public:
  ComparisonNode *condition;
  llvm::SmallVector<AssignNode *> assigns;

  ElifPartNode(ComparisonNode *condition, llvm::SmallVector<AssignNode *> assigns) : condition(condition), assigns(assigns) {}

  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }
};

class ElsePartNode : public ASTNode
{
  // Represents else Part node
public:
  llvm::SmallVector<AssignNode *> assigns;

  ElsePartNode(llvm::SmallVector<AssignNode *> assigns) : assigns(assigns) {}

  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }
};

class LoopNode : public ASTNode
{
  // Represents loop node
public:
  ComparisonNode *condition;
  llvm::SmallVector<AssignNode *> assigns;

  LoopNode(ComparisonNode *condition, llvm::SmallVector<AssignNode *> assigns) : condition(condition), assigns(assigns) {}

  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }
};

class GrammerNode : public ASTNode
{
  // Represents Grammer node
public:
  std::vector<std::shared_ptr<ASTNode>> statements;
};
