#ifndef ENACT_SCANNER_H
#define ENACT_SCANNER_H

#include <string>

#include "Token.h"

class Scanner {
private:
    std::string m_source;
    size_t m_start, m_current = 0;

    line_t m_line = 1;
    col_t m_col = 0;

    int m_openParen = 0;
    int m_openSquare = 0;

    Token m_last;

    Token number();
    Token identifier();
    Token string();

    TokenType identifierType(std::string candidate);

    Token makeToken(TokenType type);
    Token errorToken(const std::string &what);

    void skipWhitespace();

    bool isAtEnd();
    char advance();
    char peek();
    char peekNext();
    char previous();
    bool match(char expected);

    bool isDigit(char c);
    bool isIdentifierStart(char c);
    bool isIdentifier(char c);
public:
    explicit Scanner(std::string source);
    ~Scanner() = default;

    Token scanToken();
    Token backtrack();
};


#endif //ENACT_SCANNER_H
