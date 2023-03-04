# Cheatsheet

## Commands

    MANAGING EDITOR

            <ESC>   return to normal mode
                ?   display the help message
              q/Q   quit/force quit
                s   change a setting

    MANAGING FILE

              w/W   write/write as
                R   reload

    GETTING IN INSERT MODE

                i   get in insert mode
              I/A   at start/end of line
              o/O   on an empty line created below/above cursor line

    MOVING AROUND

                m   matching bracket
            0/^/$   start/first non-blank character/end of line
              g/G   line {m}/last line of the file
      arrows, l/h   {m} next/previous character
      arrows, j/k   {m} next/previous line
              t/T   {m} next/previous word
              }/{   {m} next/previous block
              n/N   {m} next/previous match

    MANAGING SELECTIONS

            <ESC>   forget saved selections, unanchor
                c   display number of saved selections
                .   select cursor line
            %/b/:   select all lines of file/{m} following blocks/custom range
             f, /   search for pattern
                *   search for the word that is currently under the cursor
                v   anchor/unanchor
                a   save running selections; unanchor
                z   duplicate the current selection on the {m} following lines

    ACTING ON SELECTIONS

    [insert mode]   insert a character before selections
    >/<, TAB/^TAB   {m} increase/decrease line indent
                K   comment/uncomment line
          x/<DEL>   suppress selection if non-null, else before/after selection
                r   replace with pattern elements and fields
              u/U   switch to lowercase/uppercase

    MANAGING LINES

              y/Y   yank {m} lines/blocks
              d/D   delete {m} lines/blocks
              p/P   paste after/before {m} times
     SHIFT+arrows   move cursor line {m} lines up/down


## Settings management

    SETTING                 NAME    TYPE    DEFAULT VALUE
    syntax highlight        sh      bool    TRUE
    highlight selections    h       bool    TRUE
    case sensitive          c       bool    TRUE
    field separator         fs      char    ,
    tab width               tw      int     4
    language                l       string  (from extension)


## Search syntax

    e2: ASSERTION
      | CHARACTER
      | CHARACTER REP                   # match a character with a repeater
      | '\(' e1 '\)'                    # group for subpatterns reuse
      | '\(' e1 '\)' REP                # match a group with a repeater

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


## Replace syntax

    e1: LITERAL
      | '\0' | '$0'                     # whole initial selection
      | '\' DIGIT                       # DIGIT-th subpattern (DIGIT >= 1)
      | '$' DIGIT                       # DIGIT-th field (DIGIT >= 1)
      | e1 e2                           # concatenation
