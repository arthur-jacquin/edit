## Search and replace

TODO: choose a known syntax, link official documentation

    SEARCH
        characters
        predefined classes: . (any), \w, \W (word, non word), \d, \D (digit, non digit)
        markers: ^, $ (start, end of selection)
        quantifiers: *, +, ?, {n}, {n,}, {,n}, {n,m}
        delimiters: \(...\)
        custom classes: [abc], [^abc], [0-9], [-6], [^-a-z#A-Z0-9], ...
        logic: |

    REPLACE
        chars
        \\ (backslash), \$ (dollar sign)
        \0 (whole pattern), \1 .. \9 (subpatterns in regex)
        $0 (whole selection), $1 .. $9 (fields)
