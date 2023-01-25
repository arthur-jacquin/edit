int
is_blank(char c)
{
    return (c == ' ');
}

int
is_word_char(char c)
{
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || (c == '_');
}

int
is_number(char c)
{
    return ('0' <= c && c <= '9');
}

int
type(char c)
{
    // TODO enumeration ?
    if (is_blank(c)) {
        return BLANK;
    } else if (is_word_char(c)) {
        return WORD_CHAR;
    } else if (is_number(c)) {
        return DIGIT;
    } else {
        return ELSE;
    }
}

int
is_in(const char *list, const char *chars, int x, int length)
{
    int i, j, ok;
    char c;

    i = j = 0;
    ok = 1;
    while (c = list[i++]) {
        if (c == ' ') {
            if (ok && j == length) {
                return 1;
            } else {
                j = 0;
                ok = 1;
            }
        } else if (c != chars[x+(j++)]) {
            ok = 0;
        }
    }

    return 0;
}
