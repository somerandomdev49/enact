#include <csignal>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <sstream>

#include "h/Chunk.h"
#include "h/Parser.h"
#include "h/EnactContext.h"
#include "h/VM.h"
#include "h/AstPrinter.h"
#include "h/Analyser.h"

#include "h/Value.h"
#include "h/Object.h"
#include "h/Compiler.h"
#include "h/GC.h"

EnactContext::EnactContext(const Options &options) :
        m_options{options},
        m_currentFile{options.filename()},
        m_gc{*this},
        m_parser{*this},
        m_analyser{*this},
        m_compiler{*this},
        m_vm{*this} {
}

InterpretResult EnactContext::run() {
    if (!m_currentFile.empty()) {
        runFile(m_currentFile);
    } else {
        runPrompt();
    }
}

InterpretResult EnactContext::runFile(const std::string &path) {
    // Get the file contents.
    std::ifstream file{path};

    // Check that the file opened successfully
    if (!file.is_open()) {
        std::cerr << "[enact] Error: Unable to read file '" + path + "'.";
        return InterpretResult::FILE_ERROR;
    }

    std::stringstream fileContents;
    std::string currentLine;

    while (std::getline(file, currentLine)) {
        fileContents << currentLine << "\n";
    }

    return runSource(fileContents.str());
}

InterpretResult EnactContext::runPrompt() {
    m_source = "!repl!";

    while (true) {
        std::cout << "enact > ";

        std::string input;
        std::getline(std::cin, input);

        runSource(input + "\n");
    }

    return InterpretResult::OK;
}

InterpretResult EnactContext::runSource(const std::string& source) {
    m_source = source;

    std::vector<std::unique_ptr<Stmt>> ast = m_parser.parse();
    if (m_parser.hadError()) return InterpretResult::PARSE_ERROR;

    ast = m_analyser.analyse(std::move(ast));
    if (m_analyser.hadError()) return InterpretResult::ANALYSIS_ERROR;

    if (m_options.flagEnabled(Flag::DEBUG_PRINT_AST)) {
        AstPrinter astPrinter;
        for (const auto& stmt : ast) {
            astPrinter.print(*stmt);
            std::cout << "\n";
        }
    }

    m_compiler.init(FunctionKind::SCRIPT, std::make_shared<FunctionType>(NOTHING_TYPE, std::vector<Type>{}), "");
    m_compiler.compile(std::move(ast));
    FunctionObject* script = m_compiler.end();

    if (m_compiler.hadError()) return InterpretResult::COMPILE_ERROR;

    if (m_options.flagEnabled(Flag::DEBUG_DISASSEMBLE_CHUNK)) {
        std::cout << script->getChunk().disassemble();
    }

    InterpretResult result = m_vm.run(script);

    m_gc.freeObjects();
    return result;
}

std::string EnactContext::getSourceLine(const line_t line) {
    std::istringstream source{m_source};
    line_t lineNumber{1};
    std::string lineContents;

    while (std::getline(source, lineContents) && lineNumber < line) {
        ++lineNumber;
    }

    return lineContents;
}

void EnactContext::reportErrorAt(const Token &token, const std::string &message) {
    std::cerr << "[line " << token.line << "] Error";

    if (token.type == TokenType::ENDFILE) {
        std::cerr << " at end: " << message << "\n\n";
    } else {
        if (token.type == TokenType::ERROR) {
            std::cerr << ":\n";
        } else {
            std::cerr << " at " << (token.lexeme == "\n" ? "newline" : "'" + token.lexeme + "'") << ":\n";
        }

        std::cerr << "    " << getSourceLine(token.lexeme == "\n" ? token.line - 1 : token.line) << "\n    ";
        for (int i = 1; i <= token.col - token.lexeme.size(); ++i) {
            std::cerr << " ";
        }

        for (int i = 1; i <= token.lexeme.size(); ++i) {
            std::cerr << "^";
        }
        std::cerr << "\n" << message << "\n\n";
    }
}

const Options& EnactContext::options() const {
    return m_options;
}

const std::string& EnactContext::source() const {
    return m_source;
}

GC& EnactContext::gc() {
    return m_gc;
}

Parser& EnactContext::parser() {
    return m_parser;
}

Analyser& EnactContext::analyser() {
    return m_analyser;
}

Compiler& EnactContext::compiler() {
    return m_compiler;
}

VM& EnactContext::vm() {
    return m_vm;
}

int main(int argc, char *argv[]) {
    Options flags{argc, argv};
    EnactContext context{flags};

    return static_cast<int>(context.run());
}