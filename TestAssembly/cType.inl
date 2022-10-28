
namespace ANG::string {

constexpr inline bool isUpper(char c) noexcept { return 'A' <= c && c <= 'Z'; }
constexpr inline bool isLower(char c) noexcept { return 'a' <= c && c <= 'z'; }
constexpr inline bool isAlpha(char c) noexcept { return isUpper(c) || isLower(c); }
constexpr inline char toUpper(char c) noexcept { return isLower(c) ? c&~0x20 : c; }
constexpr inline char toLower(char c) noexcept { return isUpper(c) ? c|0x20 : c; }
constexpr inline bool isDigit(char c) noexcept { return '0' <= c && c <= '9'; }
constexpr inline bool isXDigit(char c)noexcept { return isDigit(c) || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F'); } ///< the optimizer will merge the last two braces
constexpr inline bool isAlnum(char c) noexcept { return isAlpha(c) || isDigit(c); }
constexpr inline bool isPrint(char c) noexcept { return ' ' <= c && c <= '~'; }
constexpr inline bool isSpace(char c) noexcept { return c == ' ' || ('\t' <= c && c <= '\r'); }

}
