#ifndef ENACT_COMPILER_H
#define ENACT_COMPILER_H

#include "../ast/Stmt.h"
#include "Chunk.h"
#include "Object.h"

struct Local {
    Token name;
    uint32_t depth;
    bool initialized;
    bool isCaptured;
};

struct Upvalue {
    uint32_t index;
    bool isLocal;
};

enum class FunctionKind {
    FUNCTION,
    SCRIPT
};

class Compiler : private StmtVisitor<void>, private ExprVisitor<void> {
    friend class GC;

    Compiler* m_enclosing;

    FunctionObject* m_currentFunction{nullptr};
    FunctionKind m_functionType;

    std::vector<Local> m_locals;
    uint32_t m_scopeDepth = 0;

    std::vector<Upvalue> m_upvalues{};

    void compile(Stmt stmt);
    void compile(Expr expr);

    void visitBlockStmt(BlockStmt& stmt) override;
    void visitBreakStmt(BreakStmt& stmt) override;
    void visitContinueStmt(ContinueStmt& stmt) override;
    void visitEachStmt(EachStmt& stmt) override;
    void visitExpressionStmt(ExpressionStmt& stmt) override;
    void visitForStmt(ForStmt& stmt) override;
    void visitFunctionStmt(FunctionStmt& stmt) override;
    void visitGivenStmt(GivenStmt& stmt) override;
    void visitIfStmt(IfStmt& stmt) override;
    void visitReturnStmt(ReturnStmt& stmt) override;
    void visitStructStmt(StructStmt& stmt) override;
    void visitTraitStmt(TraitStmt& stmt) override;
    void visitWhileStmt(WhileStmt& stmt) override;
    void visitVariableStmt(VariableStmt& stmt) override;

    void visitAnyExpr(AnyExpr& expr) override;
    void visitArrayExpr(ArrayExpr& expr) override;
    void visitAssignExpr(AssignExpr& expr) override;
    void visitBinaryExpr(BinaryExpr& expr) override;
    void visitBooleanExpr(BooleanExpr& expr) override;
    void visitCallExpr(CallExpr& expr) override;
    void visitFieldExpr(FieldExpr& expr) override;
    void visitFloatExpr(FloatExpr& expr) override;
    void visitIntegerExpr(IntegerExpr& expr) override;
    void visitLogicalExpr(LogicalExpr& expr) override;
    void visitMemoryExpr(MemoryExpr& expr) override;
    void visitNilExpr(NilExpr& expr) override;
    void visitStringExpr(StringExpr& expr) override;
    void visitSubscriptExpr(SubscriptExpr& expr) override;
    void visitTernaryExpr(TernaryExpr& expr) override;
    void visitUnaryExpr(UnaryExpr& expr) override;
    void visitVariableExpr(VariableExpr& expr) override;

    void beginScope();
    void endScope();

    void addLocal(const Token& name);
    uint32_t resolveLocal(const Token& name);

    void addUpvalue(uint32_t index, bool isLocal);
    uint32_t resolveUpvalue(const Token& name);

    void defineNative(std::string name, Type functionType, NativeFn function);

    void emitByte(uint8_t byte);
    void emitByte(OpCode byte);

    void emitShort(uint16_t value);
  
    void emitLong(uint32_t value);

    void emitConstant(Value constant);

    size_t emitJump(OpCode jump);
    void patchJump(size_t index, Token where);

    void emitLoop(size_t loopStartIndex, Token where);

    Chunk& currentChunk();
  
    class CompileError : public std::runtime_error {
        Token m_token;
        std::string m_message;

    public:
        CompileError(Token token, std::string message) : std::runtime_error{"Internal error, raising exception:\nUncaught CompileError!"}, m_token{std::move(token)}, m_message{std::move(message)} {}
        const Token& getToken() const { return m_token; }
        const std::string& getMessage() const { return m_message; }
    };

    bool m_hadError = false;

    CompileError errorAt(const Token &token, const std::string &message);

public:
    explicit Compiler(Compiler* enclosing = nullptr);

    void init(FunctionKind functionKind, Type functionType, const std::string& name);
    FunctionObject* end();

    void compile(std::vector<Stmt> ast);

    bool hadError();
};

#endif //ENACT_COMPILER_H
