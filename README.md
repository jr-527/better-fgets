This repository has some tools for better terminal I/O. This is only designed
for Windows terminals that use the standard Windows escaping, ie powershell,
or linux/Unix terminals which use the standard linux escape sequences.

How to use:  
get\_key.h defines a function nextkey() which blocks until a key is pressed,
returning the next key that's pressed. If it returns 0 or greater, that
represents the ASCII code of the key that was pressed. If it returns -1, that
means it couldn't understand the input. Other negative values represent keys
that don't have an ASCII value, such as F3. See get\_key.h for details.

enter\_line.h defines a function type\_line(char out[]). It takes user input
from the terminal, like fgets, although it's nicer, supporting the arrow keys,
home/end, history, etc. It writes the input to out. out is not bounds checked,
so if it's less than 1024 characters long you may get a segmentation fault if
the user's input is longer than out. It returns 0 on a success, -1 if the user
input is too big, -2 if it fails to recognize a keypress, and -3 if it catches
an interrupt.
