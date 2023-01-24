int
load_file(char *file_name, int first_line_on_screen_nb)
{
    FILE *src_file = NULL;
    int buf_size = DEFAULT_BUF_SIZE;
    char *buf = NULL;
    char *new_buf = NULL;
    struct line *ptr;
    struct line *last_line;
    int i;
    int c;
    int line_nb;
    int reached_EOF;

    // liberate lines
    free_lines(first_line);
    first_line = first_line_on_screen = NULL;

    // open connection to src_file
    src_file = fopen(file_name, "r");
    reached_EOF = 0;
    line_nb = 1;

    // prepare buffer
    buf = (char *) malloc(buf_size);

    // read content into memory
    while (!reached_EOF) {
        i = 0;
        while (1) {
            if (i == buf_size - 1) {
                buf_size <<= 1;
                new_buf = (char *) malloc(buf_size);
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
                buf[i++] = (char) c;
            }
        }
        buf[i++] = '\0';

        // store line
        ptr = new_line(line_nb, i);
        if (first_line == NULL) {
            first_line = last_line = ptr;
            ptr->prev = NULL;
        } else {
            link_lines(last_line, ptr);
            last_line = ptr;
        }
        strcpy(ptr->chars, buf);
        if (line_nb == first_line_on_screen_nb)
            first_line_on_screen = ptr;
        line_nb++;
    }

    if (first_line_on_screen == NULL)
        first_line_on_screen = last_line;
    last_line->next = NULL;
    free(buf);

    // close connection to src_file
    fclose(src_file);

    // refresh parameters
    nb_line = line_nb - 1;
    has_been_changes = 0;

    return 0;
}

int
write_file(char *file_name)
{
    FILE *dest_file = NULL;
    struct line *ptr;
    char *chars;
    int c;

    if (first_line != NULL) {
        dest_file = fopen(file_name, "w");

        ptr = first_line;
        while (1) {
            chars = ptr->chars;
            while (c = *chars++)
                putc(c, dest_file);
            if (ptr->next == NULL) {
                break;
            } else {
                putc('\n', dest_file);
                ptr = ptr->next;
            }
        }

        fclose(dest_file);
    }

    return 0;
}

void
get_extension(void)
{
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
    int i;

    for (i = 0; i < sizeof(languages)/sizeof(struct lang); i++) {
        syntax = &languages[i];
        if (is_in(*(syntax->names), settings.language, 0, strlen(settings.language))) {
            return;
        }
    }

    syntax = NULL;
    strcpy(settings.language, "none");
}
