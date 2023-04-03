## Default keybinds

    BASICS
                ?   display the help message
              q/Q   quit/force quit
              w/W   write/write as
                R   reload
                s   change a setting
                e   execute a shell command and get back to edit

    SWITCHING TO INSERT MODE

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
              n/N   {m} next/previous selection
                J   jump to {m} next match for prompted pattern

    MANAGING SELECTIONS

            <ESC>   forget saved selections, unanchor, reset multiplier
                v   anchor/unanchor
                a   add running selections to saved selections; unanchor
                z   duplicate the running selection on the {m} following lines
                c   display number of saved selections
                .   select cursor line
            %/b/:   select all lines of file/{m} following blocks/custom range
             f, /   search for pattern
                *   search for the word that is currently under the cursor

    ACTING ON SELECTIONS

          x/<DEL>   suppress selection content if any, else char. before/after
              u/U   switch to lowercase/uppercase
    >/<, TAB/^TAB   {m} increase/decrease line indent
                K   comment/uncomment line
                r   replace (support for subpatterns and fields)
           CTRL+A   autocomplete at the end of each selection

    MANAGING LINES

              y/Y   yank {m} lines/blocks, starting at cursor
              d/D   delete {m} lines/blocks, starting at cursor
              p/P   paste after/before cursor line {m} times
     SHIFT+arrows   move lines of running selections {m} lines up/down


## Settings

    SETTING                 NAME    TYPE    DEFAULT VALUE
    syntax highlight        sh      bool    TRUE
    highlight selections    h       bool    TRUE
    case sensitive          c       bool    TRUE
    field separator         fs      char    ,
    tab width               tw      int     4
    language                l       string  (from extension)


## Search syntax

    <character> ::= <regular_char>          # match a non escaped character
                  | "\" <esc_char>          # match an escaped character
                  | "."                     # match any character
                  | "\d" | "\D"             # match any [non] digit
                  | "\w" | "\W"             # match any [non] word character
                  | "[" <set> "]"           # match any character in <set>
                  | "[^" <set> "]"          # match any character not in <set>

    <regular_char> ::= ...                  # any character not in <esc_char>

    <esc_char> ::= "\" | "^" | "$" | "|" | "(" | ")"
                 | "*" | "+" | "?" | "{" | "[" | "."

    <set> ::= <items>                       # match characters in <items>
            | "-" <items>                   # match "-" and <items> characters
            | <items> "-"                   # match "-" and <items> characters
            | "-" <items> "-"               # match "-" and <items> characters

    <items> ::= <non_minus>                 # match a character (not "-")
              | <non_minus> "-" <non_minus> # range (inclusive)
              | <items> <items>             # match characters in either <items>

    <non_minus> ::= ...                     # any character but "-"

    <repeater> ::= ""                       # exactly 1
                 | "*"                      # 0 or more (any number)
                 | "+"                      # 1 or more (at least once)
                 | "?"                      # 0 or 1 (at most once)
                 | "{}"                     # 0 or more (any number)
                 | "{" <int> "}"            # exactly <int>
                 | "{" <int> ",}"           # at least <int>
                 | "{," <int> "}"           # at most <int>
                 | "{" <int> "," <int> "}"  # range (inclusive)

    <int> ::= <positive_digit> | <int> "0" | <int> <int>

    <positive_digit> ::= "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9"

    <assertion> ::= "^"                     # match a start of line
                  | "$"                     # match an end of line
                  | "\A"                    # match a start of selection
                  | "\Z"                    # match an end of selection
                  | "\b"                    # match a words boundary
                  | "\B"                    # match a non words boundary

    <atom> ::= <assertion> | <character> <repeater>

    <OR_atom> ::= <atom> | <OR_atom> "|" <atom>

    <group> ::= "" | <group> <OR_atom>

    <block> ::= <atom> | "(" <group> ")" <repeater>

    <OR_block> ::= <block> | <OR_block> "|" <block>

    <pattern> ::= "" | <pattern> <OR_block>


## Replace syntax

    <pattern> ::= <regular_char>            # any character except "\" and "$"
                | "\\" | "\$"               # escaped "\" and "$"
                | "\0" | "$0"               # whole initial selection
                | "\" <positive_digit>      # <positive_digit>-th subpattern
                | "$" <positive_digit>      # <positive_digit>-th field
                | <pattern> <pattern>       # concatenation

    <regular_char> ::= ...                  # any character but "\" and "$"

    <positive_digit> ::= "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9"
