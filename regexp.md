# Regular expressions

## Introduction, notations

A regular expression is a pattern (string) describing a set of strings.

From now on, the shorter name "regex" will be used to refer to a regular
expression. A regex and a string are said to "match" when the string is in the
set of strings described by the regex.

The simplest regexes, the ones that do not use any special feature, describe a
set reduced to a single string: themselve. For example, the regex "edit" only
matches the string "edit", and the regex "murcielago" only matches the string
"murcielago".


    e3: ASSERTION
      | CHARACTER
      | '\(' e1 '\)'                    # group for repeaters/subpatterns reuse

    e2: e3
      | e2 REP                          # match a repeated pattern

    e1: e2
      | e1 e2                           # match the concatenation of 2 patterns
      | e1 '|' e2                       # match one of the 2 patterns

    ASSERTION: '^'                      # match the beginning of the line
             | '$'                      # match the end of the line
             | '\A'                     # match the beginning of the selection
             | '\Z'                     # match the end of the selection
             | '\b'                     # match word boundaries
             | '\B'                     # match non word boundaries
    
    CHARACTER: LITERAL                  # match the LITERAL only
             | '\' ESCAPED_LITERAL      # match to-be-escaped ESCAPED_LITERAL
             | CHARCLASS                # match characters in CHARCLASS 
             | '.'                      # match any character

    CHARCLASS: '[' LIST ']'             # match characters in LIST
             | '[^' LIST ']'            # match characters not in LIST
             | '\d'                     # match digits
             | '\D'                     # match non digits
             | '\w'                     # match word characters
             | '\w'                     # match non word characters
    
    LIST: LIST_MINUS                    # match literals in LIST_MINUS
        | '-' LIST_MINUS                # match '-' and literals in LIST_MINUS

    LIST_MINUS: LITERAL                 # match the LITERAL only 
              | LITERAL1 '-' LITERAL2   # match literals between the 2 
              | l1 l2                   # match literals in l1 or l2

    REP: '*'                            # 0 or more (any number)
       | '+'                            # 1 or more (at least once)
       | '?'                            # 0 or 1 (at most once)
       | '{' INTEGER '}'                # exactly INTEGER
       | '{' INTEGER ',' '}'            # at least INTEGER
       | '{' ',' INTEGER '}'            # at most INTEGER
       | '{' INTEGER1 ',' INTEGER2 '}'  # between INTEGER1 and INTEGER2    

[^1]: [PCRE](https://www.pcre.org/)

### Using subpatterns and fields in the replace pattern

The replace pattern can contain special sequences:

- `$0` and `\0` will be expended to the whole initial string
- `${i}` (1 <= {i} <= 9) will be expended to the {i}-th field of the initial
    string, where fields are delimited by the runtime-modifiable field separator
- `\{i}` (1 <= {i} <= 9) will be expended to the {i}-th subpattern matched by
    the search pattern in the initial string
- `\\` and `\$` produces repectively `\` and `$` (escaping sequences)

Here are some examples:

    INITIAL STRING      SEARCH PATTERN  REPLACE PATTERN RESULT
    "vim"               none            "edit"          "edit"
    "edit"              none            "$0 \$ \0"      "edit $ edit"
    "field 1,field 2"   none            "$2: $1"        "field 2: field 1"
    "murcielago"        "mur\(.{4}\)"   "\1 ($1)"       "ciel (murcielago)"
