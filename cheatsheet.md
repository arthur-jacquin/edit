## Default keybinds

### General

            <ESC>   return to default mode
                ?   help
              q/Q   quit/force quit
              w/W   write/write as
                r   reload
                i   get in insert mode
                s   change a parameter

### Other ways to get insert mode (after removing selections)

              I/A   at start/end of line
              o/O   on an empty line created below/above cursor line

### Manage lines

              y/Y   yank <n> lines/blocks
              d/D   delete <n> lines/blocks
              p/P   paste after/before <n> times
     SHIFT+arrows   move cursor line <n> lines up/down

### Move around

                m   matching bracket
            0/^/$   start/first non-blank character/end of line
              g/G   go to line <n>/end of file
      arrows, l/h   <n> next/previous character
      arrows, j/k   <n> next/previous line
              t/T   <n> next/previous word
              {/}   <n> next/previous block
              n/N   <n> next/previous match

### Manage selections

                c   display number of saved selections
            <ESC>   forget saved selections, unanchor
                .   select cursor line
            %/b/:   select all lines of file/<n> following blocks/custom range
             f, /   search for pattern
                v   anchor/unanchor
                a   push running selection to pile; unanchor
                z   duplicate the current selection on the <n> following lines

### Act on selections

              </>   <n> decrease/increase line indent
                K   comment/uncomment line
                x   suppress
                R   replace with pattern elements and fields
              u/U   switch to lowercase/uppercase

## Runtime-modifiable parameters

    SHORT NAME  LONG NAME               TYPE    DEFAULT
    sh          syntax_highlight        bool    yes
    h           highlight_selections    bool    yes
    c           case_sensitive          bool    yes
    fs          field_separator         char    ,
    tw          tab_width               int     4
    l           language                string  (from extension)
