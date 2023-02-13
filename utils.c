int
utf8_char_length(char c)
{
    // compute the length in bytes of character starting by byte c

    if ((char) (c & 0x80) == utf8_start[0]) {
        return 1;
    } else if ((char) (c & 0xe0) == utf8_start[1]) {
        return 2;
    } else if ((char) (c & 0xf0) == utf8_start[2]) {
        return 3;
    } else if ((char) (c & 0xf8) == utf8_start[3]) {
        return 4;
    }
}

int
unicode_char_length(uint32_t c)
{
    // compute the length in bytes of unicode codepoint c

    if (c < 0x80) {
        return 1;
    } else if (c < 0x800) {
        return 2;
    } else if (c < 0x10000) {
        return 3;
    } else if (c < 0x200000) {
        return 4;
    }
}

int
get_str_index(struct line *l, int x)
{
    // compute the index in chars corresponding to column x

    int i, k;

    for (k = 0, i = 0; i < x; i++)
        k += utf8_char_length(l->chars[k]);

    return k;
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
