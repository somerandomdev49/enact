#ifndef ENACT_ENACTCONTEXT_H
#define ENACT_ENACTCONTEXT_H

#include <string>
#include "Parser.h"
#include "Analyser.h"
#include "Compiler.h"
#include "VM.h"
#include "Options.h"
#include "GC.h"

enum class InterpretResult {
    OK = 0,
    INVALID_ARGUMENTS = 65,
    FILE_ERROR = 66,
    PARSE_ERROR = 67,
    ANALYSIS_ERROR = 68,
    COMPILE_ERROR = 69,
    RUNTIME_ERROR = 70,
};

class EnactContext {
    Options m_options;
    std::string m_currentFile;

    std::string m_source;

    GC m_gc;

    Parser m_parser;
    Analyser m_analyser;
    Compiler m_compiler;
    VM m_vm;

    InterpretResult runSource(const std::string &source);

    InterpretResult runFile(const std::string &path);
    InterpretResult runPrompt();
public:
    explicit EnactContext(const Options& options);

    InterpretResult run();

    std::string getSourceLine(const line_t line);
    void reportErrorAt(const Token &token, const std::string &message);

    const Options& options() const;

    const std::string& source() const;

    GC& gc();

    Parser& parser();
    Analyser& analyser();
    Compiler& compiler();
    VM& vm();
};

template <typename T, typename... Args>
inline T* GC::allocateObject(Args&&... args) {
    static_assert(std::is_base_of_v<Object, T>,
                  "GC::allocateObject<T>: T must derive from Object.");

    m_bytesAllocated += sizeof(T);
    if (m_bytesAllocated > m_nextRun || m_context.options().flagEnabled(Flag::DEBUG_STRESS_GC)) {
        collectGarbage();
    }

    T* object = new T{args...};

    m_objects.push_back(object);

    if (m_context.options().flagEnabled(Flag::DEBUG_LOG_GC)) {
        std::cout << static_cast<void*>(object) << ": allocated object of size " << sizeof(T) << " and type " <<
                  static_cast<int>(static_cast<Object *>(object)->m_type) << ".\n";
    }

    return object;
}


int main(int argc, char *argv[]);

#endif //ENACT_ENACTCONTEXT_H
