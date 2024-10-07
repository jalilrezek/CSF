Jalil Rezek
Tayseer Karrossi

*MS1*

Jalil: Mirror_h, mirror_v, tile, tests for my helper functions
Tayseer: Grayscale, composite, test functions for grayscale, composite, 
         mirror_h, mirror_v, and tile
-----------------------------------------------------------------------

*MS2*

Jalil: mirror_h in assembly, partial mirror_v in assembly (check notes below)
Tayseer: Grayscale in assembly (added a test forgot to mention it in MS2 submission)

NOTE:
We had a nefarious bug in mirror_v. For some reason, the program cannot access
the value stored in the memory address of the data array of the input image.
For instance, "$rdi + 8" should have the pointer to the data array.
Dereferencing $rdi + 8 should give us the pointer. It does give a pointer.
The pointer that it gives however (0xf7....) cannot be dereferenced.
Hence the pixel data for the input is inaccessible and the program segfaults.
I had the same issue with mirror_h but got a new mirror_h implementation working.
However, despite trying to copy these changes over to mirror_v, I was not able
to get it working in time. We are relatively sure that this bug is a minor issue 
since Gavi (TA) reviewed our code and couldn't find the bug, and it passes the 
unit tests perfectly fine. But, it is blocking the functionality completely.
Additionally, some of our MS1 helper functions (backTo1D, to2D) were for dealing with
2D arrays and not relevant to assembly so I commented out the unit tests for those 
functions, since leaving them in was blocking compilation of asm_imgproc_tests.

-----------------------------------------------------------------------------

*MS3*

Jalil: Debugged composite function and got it to pass unit tests and Gradescope tests.
Tayseer: Composite in assembly (also added another test for composite)
