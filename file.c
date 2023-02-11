int
load_file(char *file_name, int first_line_on_screen_nb)
{
    // reads the file file_name and store it in first_line list

    FILE *src_file = NULL;
    int buf_size = DEFAULT_BUF_SIZE;
    char *buf, *new_buf;
    struct line *line, *last_line;
    int line_nb;
    int c, reached_EOF, ml, dl;

    // get sure first_line list is empty
    forget_lines_list(first_line);
    first_line = first_line_on_screen = NULL;

    // open connection to src_file
    if ((src_file = fopen(file_name, "r")) == NULL)
        return ERR_FILE_CONNECTION;
    reached_EOF = 0;
    line_nb = 1;

    // prepare buffer
    buf = (char *) malloc(buf_size);
    buf[buf_size - 1] = '\0';

    // read content into memory
    while (!reached_EOF) {
        ml = dl = 0;
        while (1) {
            if (ml == buf_size - 1) {
                buf_size <<= 1;
                new_buf = (char *) malloc(buf_size);
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
                if ((buf[ml] & 0xC0) != 0x80)
                    dl++;
                ml++;
            }
        }
        buf[ml++] = '\0';

        // store line
        line = create_line(line_nb, ml, dl);
        if (first_line == NULL) {
            first_line = last_line = line;
            line->prev = NULL;
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
        return ERR_FILE_CONNECTION;

    // refresh parameters
    nb_lines = line_nb - 1;
    has_been_changes = 0;

    return 0;
}

int
write_file(char *file_name)
{
    // reads the first_line list and store the content in file_name file

    FILE *dest_file = NULL;
    struct line *l;
    char *chars;
    int c;

    // open connection to dest_file
    if ((dest_file = fopen(file_name, "w")) == NULL)
        return ERR_FILE_CONNECTION;

    // copy content of first_line list to dest_file
    l = first_line;
    while (l != NULL) {
        chars = l->chars;
        while (c = *chars++)
            if (putc(c, dest_file) == EOF)
                return ERR_FILE_CONNECTION;
        if (putc('\n', dest_file) == EOF)
            return ERR_FILE_CONNECTION;
        l = l->next;
    }

    // close connection to dest_file
    if (fclose(dest_file))
        return ERR_FILE_CONNECTION;

    return 0;
}

void
get_extension(void)
{
    // extract extension from file name interface

    int i, j;

    for (i = strlen(file_name_int.current) - 1; i >= 0 &&
        (file_name_int.current[i] != '.'); i--)
        ;
    i++;
    j = 0;
    while (i < strlen(file_name_int.current))
        settings.language[j++] = file_name_int.current[i++];
}

void
load_lang(void)
{
    // try to select a syntax

    int i;

    // iterate over possible languages
    for (i = 0; i < sizeof(languages)/sizeof(struct lang); i++) {
        settings.syntax = &languages[i];
        if (is_in(*((settings.syntax)->names), settings.language, 0,
            strlen(settings.language))) {
            return;
        }
    }

    // fail
    settings.syntax = NULL;
    strcpy(settings.language, "none");
}
