void
init_interface(struct interface *interf, const char *chars)
{
    // copy chars in current and previous fields of interf

    strcpy(interf->current, chars);
    strcpy(interf->previous, chars);
}
