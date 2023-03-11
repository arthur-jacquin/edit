static void
file_connection_error(void)
{
    // exit after failure

    tb_shutdown();
    fprintf(stderr, "%s\n", FILE_CONNECTION_MESSAGE);
    exit(ERR_FILE_CONNECTION);
}

void
load_file(int first_line_on_screen_nb)
{
    // reads the file and store it in first_line list

    FILE *src_file = NULL;
    int buf_size = DEFAULT_BUF_SIZE;
    char *buf, *new_buf;
    struct line *line, *last_line;
    int line_nb;
    int c, reached_EOF, ml, dl, l, k;

    // reset variables
    forget_lines(first_line);
    reset_selections();
    first_line = first_line_on_screen = NULL;

    // open connection to src_file
    if ((src_file = fopen(file_name_int.current, "r")) == NULL)
        file_connection_error();
    reached_EOF = 0;
    line_nb = 1;

    // prepare buffer
    buf = (char *) _malloc(buf_size);

    // read content into memory
    while (!reached_EOF) {
        ml = dl = 0;
        while (1) {
            if ((c = getc(src_file)) == EOF) {
                reached_EOF = 1;
                break;
            } else if (c == '\n') {
                break;
            } else {
                // compute number of bytes to add
                if (c == '\t') {
                    l = 1;
                    while ((dl + l)%(settings.tab_width))
                        l++;
                } else {
                    l = utf8_char_length(c);
                }
                // potentially resize buf
                if (ml + l > buf_size) {
                    while (ml + l > buf_size)
                        buf_size <<= 1;
                    new_buf = (char *) _malloc(buf_size);
                    strncpy(new_buf, buf, ml);
                    free(buf);
                    buf = new_buf;
                }
                // store bytes
                if (c == '\t') {
                    for (k = 0; k < l; k++)
                        buf[ml + k] = ' ';
                    ml += l; dl += l;
                } else {
                    // check UTF-8 compliance
                    buf[ml] = (char) c;
                    for (k = 1; k < l; k++) {
                        if (((c = getc(src_file)) == EOF) ||
                            (((char) c & (char) 0xc0) != (char) 0x80))
                            exit(ERR_UNICODE_OR_UTF8);
                        buf[ml + k] = (char) c;
                    }
                    ml += l; dl++;
                }
            }
        }

        // detect an empty line file ending
        if (reached_EOF && ml == 0)
            break;

        // store line
        line = create_line(line_nb, ml + 1, dl);
        if (first_line == NULL) {
            first_line = last_line = line;
        } else {
            link_lines(last_line, line);
            last_line = line;
        }
        strncpy(line->chars, buf, ml);
        line->chars[ml] = '\0';
        if (line_nb == first_line_on_screen_nb)
            first_line_on_screen = line;
        line_nb++;
    }

    if (first_line_on_screen == NULL)
        first_line_on_screen = last_line;
    last_line->next = NULL;
    free(buf);

    // close connection to src_file
    if (fclose(src_file) == EOF)
        file_connection_error();

    // refresh parameters
    nb_lines = line_nb - 1;
    has_been_changes = 0;
    load_lang(file_name_int.current);
}

void
write_file(const char *file_name)
{
    // reads the first_line list and store the content in file_name file

    FILE *dest_file = NULL;
    struct line *l;
    char *chars;
    int c;

    // open connection to dest_file
    if ((dest_file = fopen(file_name, "w")) == NULL)
        file_connection_error();

    // copy content of first_line list to dest_file
    l = first_line;
    while (l != NULL) {
        chars = l->chars;
        while (c = *chars++)
            if (putc(c, dest_file) == EOF)
                file_connection_error();
        if (putc('\n', dest_file) == EOF)
            file_connection_error();
        l = l->next;
    }

    // close connection to dest_file
    if (fclose(dest_file) == EOF)
        file_connection_error();
}
