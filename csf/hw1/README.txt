TODO: list team members, document who did what, discuss
anything interesting about your implementation.

MS1:
I worked with Tayseer Karrossi. We both worked on all the functions separately and discussed our results. I was a 
bit confused in particular on how to implement the to_hex() function and Tayseer explained the functionalities
of the required libraries and also implemented his version of it. I helped debug his version. It was a collaborative
effort. I also added some unit tests for some of the simpler functions like the unary minus operator. 


MS2: 

Did not get to mult, div, or dec and so their test functions have their contents copied out to avoid 
compilation errors. 


Tayseer worked on compare(), operator<<, and is_bit_set(). I worked on operator+ and operator-. 
Regarding the latter two, as suggested, operator- simply calls operator+ on an oppositely signed version of
the given "rhs" argument.
This means that operator+ does all the heavy lifting. It relies on three helper functions, addMagnitudes, 
subMagnitudes, and thisGreaterMagThanOther, which compares magnitudes.
Essentially, the operator+ function judges which to call based on the signs of the arguments it receives. 
For instance, if one is negative and one is positive, it always calls subMagnitudes, and in this case will decide
on the sign of the result based on whether we subtract from the number with smaller or larger magnitude.

Some test cases were generated with the help of Python in the terminal. A log of the commands used is given.
I also had a script that would convert giant hex numbers in normal big endian to the BigInts' internal representations,
in order to properly verify results. 

The process was, I would generate giant random numbers in hexadecimal using the terminal, then apply the 
appropriate operation. Then, I would convert the two operands (a and b in a+b = c) and the answer (c) and 
write a test function, and use check_contents to check the result obtained with the desired result, which was
(c) converted to the appropriate string format.



