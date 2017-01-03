See jigtest for a demo application that uses jiglib.

Applications should add this directory/include to their include path,
and this directory/lib to their link path (after building jiglib)

Coding style:

Types defined by JigLib start with a "t" and the first letter of each
word is capitalised - e.g. tCollSkin.

Variables used by JigLib start with lowercase, and the first letter of
each subsequent word is capitalised - e.g. collSkin.

Member variables start with "m" - e.g. mCollSkin. No more than this -
i.e. NOT mpCollSkin

Functions start with a capital letter and the first letter of each
word is capitalised.

Macros are in capitals (unless they're pretending not to be macros)
and underscore is used to separate words.

Everything is in namespace "JigLib". 

Indentation is two spaces - NOT TABS

Every header/file has to have a newline at the end (MSVC doesn't
enforce this, but gcc does!).

JigLib is for physics - it shouldn't contain any render code! If you
want to see something, use another library or write render code at the
application level.

