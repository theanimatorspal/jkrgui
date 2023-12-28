#pragma once
#include "include.hpp"
#include <cinttypes>
#include <climits>
constexpr int8_t first_reserved = INT8_MAX + 1;
constexpr uint32_t UTF8BUFFZ = 8;
enum Token : int8_t {
    And = first_reserved,
    Break,
    Do,
    Else,
    Elseif,
    End,
    False,
    For,
    Function,
    Goto,
    If,
    In,
    Local,
    Nil,
    Not,
    Or,
    Repeat,
    Return,
    Then,
    True,
    Until,
    While,
    /* other terminal symbols */ Idiv,
    Concat,
    Dots,
    Eq,
    Ge,
    Le,
    Ne,
    Shl,
    Shr,
    Dbcolon,
    Eos,
    Flt,
    Int,
    Name,
    String
};

using SemanticInfo = var<Number, Integer, s>;

struct TokenSematic {
    int token;
    SemanticInfo s;
    std::strong_ordering operator<=>(int inToken)
    {
        return inToken <=> token;
    }
    std::strong_ordering operator<=>(Token inToken)
    {
        return inToken <=> token;
    }
    bool operator!=(int inToken)
    {
        return inToken != token;
    }
    void operator=(int inToken)
    {
        token = inToken;
    }
    template <typename T>
    T Get()
    {
        return get<T>(s);
    }
};

static sv const TokenStrings[] = {
    "and", "break", "do", "else", "elseif", "end", "false", "for", "function", "goto",
    "if", "in", "local", "nil", "not", "or", "repeat", "return", "then", "true", "until", "while",
    "//", "..", "...", "==", ">=", "<=", "~=", "<<", ">>", "::", "<eof>", "<number>", "<integer>", "<name>", "<string>"
};

constexpr s TokenToString(int inToken)
{
    if (inToken < first_reserved) {
        if (std::isprint(inToken)) {
            return s().append("'").append(std::to_string((char)inToken)).append("'");
        } else {
            return s().append("'<").append(std::to_string((char)inToken)).append(">'");
        }
    }
    return s(TokenStrings[inToken - first_reserved]);
}

struct gse {
    constexpr gse(s instr)
        : mstr(mv(instr))
    {
    }
    gse& operator()()
    {
        std::cout << mstr;
        return *this;
    }
    gse& operator<<(sv inString)
    {
        std::cout << inString;
        return *this;
    }
    gse& operator<<(char inC)
    {
        std::cout << inC;
        return *this;
    }
    gse& operator++(int)
    {
        std::cout << '\n';
        return *this;
    }

private:
    s mstr;
};

static gse SyntaxError("SyntaxError:");
static gse LexError("LexError:");

struct BlockState {
};

struct FunctionState {
};

struct LexerState {
    int mcurrent;
    int mlinenumber;
    int mlastline;
    TokenSematic mt;
    TokenSematic mlookahead;
    s mbuff;
    std::istream* mz;

    void next() { mcurrent = mz->get(); }
    void save(int c) { mbuff.push_back(c); }
    void save_and_next()
    {
        save(mcurrent);
        next();
    }

    void inclinenumber()
    {
        int old = mcurrent;
        assert(currIsNewline());
        next();
        if (currIsNewline() and mcurrent != old) {
            next();
        }
        if (++mlinenumber >= std::numeric_limits<int>::max()) {
            (LexError << "Chunk has too many lines")++;
        }
    }

    int check_next(int c)
    {
        if (mcurrent == c) {
            next();
            return 1;
        } else
            return 0;
    }

    int check_next(sv set)
    {
        assert(set[2] == '\0');
        if (mcurrent == set[0] or mcurrent == set[1]) {
            next();
            return 1;
        } else
            return 0;
    }

    int currIsNewline()
    {
        return mcurrent == '\n' or mcurrent == '\r';
    }

    int read_numeral(SemanticInfo& inseminfo)
    {
        sv expo = "Ee";
        int first = mcurrent;
        assert(std::isdigit(mcurrent));
        save_and_next();
        if (first == 0 and check_next("xX")) {
            expo = "Pp";
        }
        while (true) {
            if (check_next(expo)) {
                check_next("-+");
            } else if (std::isdigit(mcurrent) or mcurrent == '.') {
                save_and_next();
            } else {
                break;
            }
        }
        if (std::isalpha(mcurrent)) {
            save_and_next();
        }
        save('\0');
        Number num = std::strtod(mbuff.c_str(), nullptr);
        Number wholepart;
        Number fractionalpart = std::modf(num, &wholepart);
        if (fractionalpart == 0) // is integer
        {
            inseminfo = (int)wholepart;
            return Token::Int;
        } else {
            inseminfo = num;
            return Token::Flt;
        }
    }

    size_t skipsep_sqbrackts()
    {
        size_t count = 0;
        int s = mcurrent;
        assert(s == '[' or s == ']');
        save_and_next();
        while (mcurrent == '=') {
            save_and_next();
            count++;
        }
        return (mcurrent == s) ? count + 2 : (count == 0) ? 1
                                                          : 0;
    }

    void read_long_string(optref<SemanticInfo> ininfo, size_t insep)
    {
        int line = mlinenumber;
        save_and_next();
        if (currIsNewline()) {
            inclinenumber();
        }
        bool run = true;
        while (run) {
            switch (mcurrent) {
            case EOF: {
                sv what = (ininfo.has_value() ? "string" : "comment");
                (LexError << "unfinished long"
                          << what << "(starting at line " << line << ")")++;
            }
            case ']': {
                if (skipsep_sqbrackts() == insep) {
                    save_and_next();
                    run = false;
                }
                break;
            }
            case '\n':
            case '\r': {
                save('\n');
                inclinenumber();
                if (not ininfo.has_value()) {
                    mbuff.clear();
                }
                break;
            }
            default: {
                if (ininfo.has_value()) {
                    save_and_next();
                } else {
                    next();
                }
            }
            }
        }
        if (ininfo.has_value()) {
            ininfo.value().get() = s(mbuff.begin(), mbuff.end() - 2);
        }
    }

    void esccheck(int c, sv msg)
    {
        if (not c) {
            if (mcurrent != EOF) {
                save_and_next();
            }
            (LexError << msg)++;
        }
    }

    int hexvaluefrom_charint(int c)
    {
        if (std::isdigit(c))
            return c - '0';
        else
            return (std::tolower(c) - 'a') + 10;
    }

    int gethexa()
    {
        save_and_next();
        esccheck(std::isdigit(mcurrent), "hexadecimal digit expected");
        return hexvaluefrom_charint(mcurrent);
    }

    int readhexaesc()
    {
        int r = gethexa();
        r = (r << 4) + gethexa();
        mbuff.resize(mbuff.size() - 2);
        return r;
    }

    unsigned long readutf8esc()
    {
        unsigned long r;
        int i = 4;
        save_and_next();
        esccheck(mcurrent == '{', "missing '{'");
        r = gethexa();
        auto is_digit_and_save_next = [&]() -> bool {
            save_and_next();
            return std::isdigit(mcurrent);
        };

        while (is_digit_and_save_next()) {
            i++;
            esccheck(r <= (0x7FFFFFFFu >> 4), "UTF-8 value too large");
            r = (r << 4) + hexvaluefrom_charint(mcurrent);
        }
        esccheck(mcurrent == '}', "missing '}'");
        next();
        mbuff.resize(mbuff.size() - i);
        return r;
    }

    int utf8esc(char* buff, unsigned long x)
    {
        int n = 1;
        assert(x <= 0x7FFFFFFFu);
        if (x < 0x80) /* is ascii */
        {
            buff[UTF8BUFFZ - 1] = (char)x; // paxadi bata haal ascii ho vane 255 vitrai parxa
        } else {
            unsigned int mfb = 0x3f; // maximum that fits in the first byte
            do {
                buff[UTF8BUFFZ - (n++)] = (char)(0x80 | (x & 0x3f));
                x >>= 6;
                mfb >>= 1;
            } while (x > mfb);
            buff[UTF8BUFFZ - n] = (char)((~mfb << 1) | x);
        }
        return n;
    }

    void utf8esc()
    {
        char buff[UTF8BUFFZ];
        int n = utf8esc(buff, readutf8esc());
        for (; n > 0; n--) {
            save(buff[UTF8BUFFZ] - n);
        }
    }

    int readdecesc()
    {
        int i;
        int r = 0;
        for (i = 0; i < 3 and std::isdigit(mcurrent); i++) {
            r = 10 * r + mcurrent - '0';
            save_and_next();
        }
        esccheck(r <= UCHAR_MAX, "decimal escape too large");
        mbuff.resize(mbuff.size() - i);
        return r;
    }

    /*
    - Decimal Escape Sequence
        print("Decimal Escape Sequence: \097\098\099") -- Outputs: abc

        -- UTF-8 Escape Sequence
        print("UTF-8 Escape Sequence: \u{1F604}") -- Outputs: 😄
    */

    void read_string(int del, SemanticInfo& inseminfo)
    {
        save_and_next(); /* Keep delimeter */
        while (mcurrent != del) {
            switch (mcurrent) {
            case EOF:
                (LexError << "Unfinished string")++;
                break;
            case '\n':
            case '\r':
                (LexError << "Unfinished String")++;
                break;
            case '\\': {
                int c;
                save_and_next();
                switch (mcurrent) {
                case 'a':
                    c = '\a';
                    goto read_save;
                case 'b':
                    c = '\b';
                    goto read_save;
                case 'f':
                    c = '\f';
                    goto read_save;
                case 'n':
                    c = '\n';
                    goto read_save;
                case 'r':
                    c = '\r';
                    goto read_save;
                case 't':
                    c = '\t';
                    goto read_save;
                case 'v':
                    c = '\v';
                    goto read_save;
                case 'x':
                    c = readhexaesc();
                    goto read_save;
                case 'u':
                    utf8esc();
                    goto no_save;
                case '\n':
                case '\r':
                    inclinenumber();
                    c = '\n';
                    goto only_save;
                case '\\':
                case '\"':
                case '\'':
                    c = mcurrent;
                    goto read_save;
                case EOF:
                    goto no_save; /* will raise an error next loop */
                case 'z': { /* zap following span of spaces */
                    mbuff.resize(mbuff.size() - 1);
                    next(); /* skip the 'z' */
                    while (std::isspace(mcurrent)) {
                        if (currIsNewline())
                            inclinenumber();
                        else
                            next();
                    }
                    goto no_save;
                }
                default: {
                    esccheck(std::isdigit(mcurrent), "invalid escape sequence");
                    c = readdecesc(); /* digital escape '\ddd' */
                    goto only_save;
                }
                }
            read_save:
                next();
            only_save:
                mbuff.resize(mbuff.size() - 1);
                save(c);
            no_save:
                break;
            }
            default:
                save_and_next();
            }
        }
        save_and_next();
        inseminfo = s(mbuff.begin() + 1, mbuff.end() - 2);
    }

    auto isreserved(sv inStr)
    {
        return std::find(TokenStrings->begin(), TokenStrings->end(), inStr);
    }

    int Lex(SemanticInfo& inS)
    {
        mbuff.clear();
        while (true) {
            switch (mcurrent) {

            case '\n':
            case '\r': {
                inclinenumber();
                break;
            }
            case ' ':
            case '\f':
            case '\t':
            case '\v': { /* spaces */
                next();
                break;
            }

            case '-': {
                next();
                if (mcurrent != '-') {
                    return '-';
                }
                next();
                if (mcurrent == '[') {
                    size_t sep = skipsep_sqbrackts();
                    mbuff.clear();
                    if (sep >= 2) {
                        read_long_string(std::nullopt, sep);
                        mbuff.clear();
                        break;
                    }
                }
                /* haina vane short comment ho*/
                while (not currIsNewline() and mcurrent != EOF) {
                    next();
                    break;
                }
            }
            case '[': {
                size_t sep = skipsep_sqbrackts();
                if (sep >= 2) {
                    read_long_string(inS, sep);
                    return Token::String;
                } else if (sep == 0) {
                    (LexError << "invalid long string delimeter")++;
                }
                return '[';
            }
            case '=': {
                next();
                if (check_next('=')) {
                    return Token::Eq;
                } else {
                    return '=';
                }
            }
            case '<': {
                next();
                if (check_next('=')) {
                    return Token::Le;
                } else if (check_next('<')) {
                    return Token::Shl;
                } else {
                    return '<';
                }
            }
            case '>': {
                next();
                if (check_next('=')) {
                    return Token::Ge;
                } else if (check_next('>')) {
                    return Token::Shr;
                } else {
                    return '>';
                }
            }
            case '/': {
                next();
                if (check_next('/'))
                    return Token::Idiv; /* '//' */
                else
                    return '/';
            }
            case '~': {
                next();
                if (check_next('='))
                    return Token::Ne; /* '~=' */
                else
                    return '~';
            }
            case ':': {
                next();
                if (check_next(':'))
                    return Token::Dbcolon; /* '::' */
                else
                    return ':';
            }
            case '"':
            case '\'': { /* short literal strings */
                read_string(mcurrent, inS);
                return Token::String;
            }
            case '.': { /* '.', '..', '...', or number */
                save_and_next();
                if (check_next('.')) {
                    if (check_next('.'))
                        return Token::Dots; /* '...' */
                    else
                        return Token::Concat; /* '..' */
                } else if (not std::isdigit(mcurrent))
                    return '.';
                else
                    return read_numeral(inS);
            }
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9': {
                return read_numeral(inS);
            }
            case EOF: {
                return Token::Eos;
            }
            default: {
                if (std::isalpha(mcurrent)) { /* identifier or reserved word? */
                    s ts;
                    do {
                        save_and_next();
                    } while (std::isalnum(mcurrent));
                    ts = s(mbuff.begin(), mbuff.end());
                    inS = ts;
                    auto itr = isreserved(ts);
                    if (itr != TokenStrings->end()) { // Vanesi yo reserved token ho
                        return std::distance(TokenStrings->begin(), itr); // Get iterator distance
                    } else {
                        return Token::Name;
                    }
                } else { /* single-char tokens ('+', '*', '%', '{', '}', ...) */
                    int c = mcurrent;
                    next();
                    return c;
                }
            }
            }
        }
    }

    void collectnext ( )
    {
        mlastline = mlinenumber;
	   if (mlookahead != Token::Eos)
	   {
            mt = mlookahead;
            mlookahead = Token::Eos;
	   }
	   else {
            mt = Lex(mt.s);
	   }
    }

    int lookahead ( )
    {
           assert(mlookahead.token == Token::Eos);
           mlookahead = Lex(mlookahead.s);
           return mlookahead.token;
    }

};

struct ParserState {
    v<s> localvars;
    FunctionState* fs;
};
