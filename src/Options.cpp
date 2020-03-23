#include <iostream>
#include "h/Options.h"
#include "h/EnactContext.h"

Options::Options(int argc, char **argv) : Options{} {
    if (argc == 0) return;
    std::vector<std::string> args{argv + 1, argv + argc};

    size_t current = 0;
    for (; current < args.size(); ++current) {
        std::string arg = args[current];

        if (arg.size() >= 2 && arg[0] == '-' && arg[1] == '-') {
            parseString(arg);
        } else if (arg.size() >= 1 && arg[0] == '-') {
            for (char c : arg.substr(1)) {
                parseString(std::string{"-"} + c);
            }
        } else {
            break;
        }
    }

    if (m_hadError) {
        exit((int)InterpretResult::INVALID_ARGUMENTS);
    }

    if (current >= args.size()) {
        // There are no more arguments for us to parse.
        return;
    }

    m_filename = args[current++];

    while (current < args.size()) {
        m_programArgs.push_back(args[current++]);
    }
}

void Options::parseString(const std::string& string) {
    if (m_parseTable.count(string) > 0) {
        m_parseTable[string]();
    } else {
        std::cerr << "[enact] Error:\n    Unknown interpreter flag '" << string <<
                "'.\nUsage: enact [interpreter flags] [filename] [program flags]\n\n";
        m_hadError = true;
    }
}

void Options::enableFlag(Flag flag) {
    m_flags.insert(flag);
}

void Options::enableFlags(std::vector<Flag> flags) {
    for (Flag flag : flags) {
        enableFlag(flag);
    }
}

bool Options::flagEnabled(Flag flag) const {
    return m_flags.count(flag) > 0;
}

const std::string& Options::filename() const {
    return m_filename;
}

const std::vector<std::string>& Options::programArgs() const {
    return m_programArgs;
}
