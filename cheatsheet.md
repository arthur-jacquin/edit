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
