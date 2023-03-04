# Regular expressions

## Introduction, notations

A regular expression is a pattern (string) describing a set of strings.

From now on, the shorter name "regex" will be used to refer to a regular
expression. A regex is said to "match" a string when the string is in the
set of strings described by the regex.

The simplest regexes, the ones that do not use any special feature, describe a
set reduced to a single string: themselve. For example, the regex "edit" only
matches the string "edit", and the regex "murcielago" only matches the string
"murcielago".

The whole point of regex expressions is to provide a syntax to easily describes
sets of structurally similar strings. Let's explore these different syntax
elements with examples.


## Syntax elements

### Assertions

Some elements mark a specific position: `^` (resp. `$`) marks a line start
(resp. end), `\A` (resp. `\Z`) marks a selection start (resp. end). Let's
consider the following line:

    This is a file. This is the first line of this file.
              -----                                -----
                A                                    B

Substring B is matched by both "file." and "file.$" regexes, but only the former
matchs substring A.

Finally, `\b` (resp. `\B`) marks a word boundary (resp. not a word boundary). A
word boundary is the transition between a word character and a non-word
character.

This is particularly helpful for searching the occurrences of a specific
variable in code: searching "i" in "print(i)" will match both "i", while "\bi\b"
only matches the second one. In edit, the `*` command automatically delimits the
word under the cursor and search for it, padded with `\b` delimiters.

### Characters

The simplest way to match a character is to use the character in the regex.

But some characters have a special meaning in a regex. We previously saw that
`$` marks an end of line. These characters needs to be "escaped" with an
antislash. For example, you can use the regex `\$` to match `$`, and `\\` to
match `\`.

Some syntax elements refers to classes of characters. For example, `\d` will
match any digit: `0`, `1` and all the way to `9`. `\D` matches non digits, and
`\w` (resp. `\W`) matches word characters (resp. non-word characters).

For more specific needs, you can create custom classes of characters. These
classes are characters lists encapsulated in square bracket. The potential `^`
symbol at the beginning of the list indicates that the class matches characters
that are NOT in the rest of the list; else the class matches any characters in
the list. Let's see some examples:

    "[abc]" matches "a" and "c" but not "e"
    "[^abc]" matches "e" but not "a" and "c"

You can also use ranges in the list:

    "[a-z]" matches any lowercase letter
    "[^5-9]" matches "a" and "4" but neither "5" nor "8"

Use `-` at the begininng of the list so that it is not interpreted as a range:

    "[-09]" matches "-", "0" and "9" while "[0-9]" is equivalent to "\d"
    "[^-a-z]" matches all characters but "-" and lowercase letters

Finally you can combine all of these elements. For example, `[^-a-cX-Z02468]`
matches all characters but `-`, `a`, `b`, `c`, `X`, `Y`, `Z` and even digits.

### Repeaters

You can follow a character expression by a repeater. A repeater specifies how
many times the preceding character should be matched:

* `*` means 0 or more times (any number of times)
* `+` means 1 or more times (at least once)
* `?` means 0 or 1 time (at most once)
* `{n}` means n times
* `{min,}` means at least min times
* `{,max}` means at most min times
* `{min,max}` means between min and max times

Let's see some examples:

    "a*" matches "", "a" and "aaa"
    "a+" matches "a" and "aaa" but not ""
    "a?" matches "" and "a" but not "aaa"
    "a{5}" only matches "aaaaa"
    "a{1,3}" only matches "a", "aa" and "aaa"


## Building a pattern

Now that you read about all the syntax elements supported by edit, let's see how
to build a pattern.

### Concatenation

The simplest rule is concatenation. Just concatenate regexes to create a new
one, that matches all possible concatenations of matches. For example, as "a"
matches "a" and "\d" matches "0", the concatenated regex "a\d" matches "a0".

### OR logic

The `|` symbol is a logical OR between the two adjacent elements:

    "\D|6" does not match digits except "6"
    "a+|b" matches "a", "aaa" and "b"

### Grouping

You can group characters of the regex in blocks with delimiters `\(` and `\)`.
It might be useful for several purposes.

First, blocks also support repeaters and OR logic:

    "a\(ba\){4}" only matches "ababababa"
    "\(apple\)|\(orange\)" only matches "apple" and "orange"

Lastly, characters matched by this portion of the regex can be reused in a
replace pattern, which is very powerful. Please read `manual.md` for more
information on how searching and replacing interact.


## Summing it up

Search and replace syntax are reminded in `cheatsheet.md`, in a Backus-Naur form
style.
