int
get_str_index(struct line *l, int x)
{
    int i, k;

    for (k = 0, i = 0; i < x; i++)
        k += utf8_char_length(l->chars[k]);

    return k;
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
