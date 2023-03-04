# Syntax highlighting

## Colors

All the configuration happens in `config.h`. By default, edit expects to work
with a 256 colors compliant terminal, but you can change to 8 colors mode by
commenting/deleting the following line:

    #define TERMINAL_SUPPORTS_256_COLORS

For each color mode, the following colors are defined (their name is
self-explanatory for most of them):

    COLOR_DEFAULT
    COLOR_RULER
    COLOR_DIALOG
    COLOR_KEYWORD
    COLOR_FLOW_CONTROL
    COLOR_BUILT_IN
    COLOR_NUMBER
    COLOR_STRING
    COLOR_COMMENT
    COLOR_BG_DEFAULT
    COLOR_BG_SELECTIONS     # highlighting selections
    COLOR_BG_MATCHING       # highlighting the matching bracket

Feel free to tailor it to your tastes. Colors of 8 colors mode are named by
`termbox.h`. For 256 colors mode, you can refer to this [color palette](TODO).


## Syntax elements

The syntax highlighting system is not very capable. Let's see what it does.

Each line is processed independently. That means multiline elements, such as
some comments or strings, won't be highlighted correctly.

First, it tries to match a "rule". A rule is a marker that can starts at either
the beginning or the first non-blank characters of the line. If a rule matches
the line, the mark and the rest of the line will be colored with the colors
associated with the rule.

If no rule match the line, the system will color:
* keywords, flow control and built-ins words
* comments
* numbers
* strings

Specific words (keywords, flow control and built-ins) are declared in a single
space-separated string.


## Adding a new language

The default configuration provides a limited number of languages, and most of
them are commented out by default to not bloat the executable with languages the
user will never use.

Adding a language is therefore pretty common and simple, given the system is not
very powerful. First, check if your language is not already in the configuration
file, commented or not. If not, you will have to declare it manually.

Declaring it manually is quite easy by copying the way other languages are
already declared. Just make sure the strings are space terminated. You also need
to end the rules array with a non-significant rule with empty ("") mark.

To get the lists of specific words, you can look at syntax files for other
editors. The best ressource is kibi[^1], but there are not many languages.
vim[^2] and micro[^3] provides more languages but the syntax files are harder to
use (as the coloration system is much more complete, and therefore complex).

[^1]: [kibi](https://github.com/ilai-deutel/kibi/tree/master/syntax.d)
[^2]: [vim](https://github.com/vim/vim/tree/master/runtime/syntax)
[^3]: [micro](https://github.com/zyedidia/micro/tree/master/runtime/syntax)

The last step is to add the language to the `languages` array at the end of the
file. It is needed, wether the language was already there in comments or not.
