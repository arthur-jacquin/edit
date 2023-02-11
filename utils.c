int
utf8_char_length(char c)
{
    if ((c & 0x80) == 0) {
        return 1;
    } else if ((c & 0xe0) == 0xc0) {
        return 2;
    } else if ((c & 0xf0) == 0xe0) {
        return 3;
    } else if ((c & 0xf8) == 0xf0) {
        return 4;
    } else {
        return 0;
    }
}

int
is_blank(char c)
{
    // check if c is a blank charater

    return (c == ' ');
}

int
is_word_char(char c)
{
    // check if c is a word character

    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || (c == '_');
}

int
is_digit(char c)
{
    // check if c is a digit

    return ('0' <= c && c <= '9');
}

int
is_in(const char *list, const char *chars, int x, int length)
{
    // check if chars substring starting at position x and of length length is
    // in space-separated list of words in list

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
