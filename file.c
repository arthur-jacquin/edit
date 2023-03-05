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
    int c, reached_EOF, ml, dl;

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
    buf[buf_size - 1] = '\0';

    // read content into memory
    while (!reached_EOF) {
        ml = dl = 0;
        while (1) {
            if (ml == buf_size - 1) {
                buf_size <<= 1;
                new_buf = (char *) _malloc(buf_size);
                new_buf[buf_size - 1] = '\0';
                strcpy(new_buf, buf);
                free(buf);
                buf = new_buf;
            }
            if ((c = getc(src_file)) == EOF) {
                reached_EOF = 1;
                break;
            } else if (c == '\n') {
                break;
            } else {
                buf[ml] = (char) c;
                if ((buf[ml] & (char) 0xc0) != (char) 0x80)
                    dl++;
                ml++;
            }
        }
        buf[ml++] = '\0';

        // store line
        line = create_line(line_nb, ml, dl);
        if (first_line == NULL) {
            first_line = last_line = line;
        } else {
            link_lines(last_line, line);
            last_line = line;
        }
        strcpy(line->chars, buf);
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
