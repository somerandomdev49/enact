#ifndef ENACT_OPTIONS_H
#define ENACT_OPTIONS_H

#include <string>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <functional>

enum class Flag {
    DEBUG_PRINT_AST,
    DEBUG_DISASSEMBLE_CHUNK,
    DEBUG_TRACE_EXECUTION,
    DEBUG_STRESS_GC,
    DEBUG_LOG_GC
};

class Options {
    std::unordered_set<Flag> m_flags{};
    std::string m_filename{};
    std::vector<std::string> m_programArgs{};
    bool m_hadError{false};

    void parseString(const std::string& string);

public:
    Options() = default;
    Options(int argc, char** argv);

    void enableFlag(Flag flag);
    void enableFlags(std::vector<Flag> flags);

    bool flagEnabled(Flag flag) const;
    const std::string& filename() const;
    const std::vector<std::string>& programArgs() const;

private:
    std::unordered_map<std::string, std::function<void()>> m_parseTable{
            {"--debug-print-ast",         std::bind(&Options::enableFlag, this, Flag::DEBUG_PRINT_AST)},
            {"--debug-disassemble-chunk", std::bind(&Options::enableFlag, this, Flag::DEBUG_DISASSEMBLE_CHUNK)},
            {"--debug-trace-execution",   std::bind(&Options::enableFlag, this, Flag::DEBUG_TRACE_EXECUTION)},
            {"--debug-stress-gc",         std::bind(&Options::enableFlag, this, Flag::DEBUG_STRESS_GC)},
            {"--debug-log-gc",            std::bind(&Options::enableFlag, this, Flag::DEBUG_LOG_GC)},

            {"--debug",                   std::bind(&Options::enableFlags, this, std::vector<Flag>{
                Flag::DEBUG_PRINT_AST,
                Flag::DEBUG_DISASSEMBLE_CHUNK,
                Flag::DEBUG_TRACE_EXECUTION,
                Flag::DEBUG_STRESS_GC,
                Flag::DEBUG_LOG_GC
            })},
    };
};

#endif //ENACT_OPTIONS_H
