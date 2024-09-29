/*
 * x86-64 assembly language implementations of functions
 */

	.section .text

/* Offsets of struct Image fields */
#define IMAGE_WIDTH_OFFSET   0
#define IMAGE_HEIGHT_OFFSET  4
#define IMAGE_DATA_OFFSET    8

/*
 * TODO: define your helper functions here.
 * Don't forget to use the .globl directive to make
 * them visible to the test code.
 */

/*
 * Implementations of API functions
 */

/*
 * void imgproc_mirror_h( struct Image *input_img, struct Image *output_img );
 *
 * Transform image by mirroring its pixels horizontally.
 * This transformation always succeeds.
 *
 * Parameters:
 *   %rdi - pointer to original struct Image
 *   %rsi - pointer to output struct Image
 */
	.globl imgproc_mirror_h
imgproc_mirror_h:

	/*NOTES: Must use 32-bit versions of the registers if I'm only operating on 32 bits of them.*/
	/* TODO: implement */
	pushq %rbp /* -8 from rsp. Aligned at this point.*/
	movq %rsp, %rbp
	subq $24, %rsp; /*1 local var: height (r12d). rsp will be shifted down by 8. Was aligned from above. 
	; so shift down by another 8, so that after adding the local var, it's aligned to mult of 16 again.
	; so right now it's misaligned but this is anticipating the local var push, after which will be aligned.
	; EDIT: Actually, shift by 16 to leave space at (rbp - 8?) for another local var. */

	/*PUSH CALLEE SAVED/LOCAL VARS*/
	pushq %r12 /*height. rdi has the width... NEED IT BE r12d? NO - stack is designed to push & pop 8 bytes at a time.
	; so push all 64 bits (pushq) even if only using bottom 32 (movl)*/
	/*movq %rdi, -8(%rbp) /*Local variable to hold val of rdi for malloc should be commented out I think. */

	/*Replace height assignment:*/
	movl IMAGE_HEIGHT_OFFSET(%rdi), %eax      # Load the height value into eax
	movl %eax, -8(%rbp)                        # Store the value from eax into the local variable at -8(%rbp)
	movq %rdi, %r12                           # r12 replaces rdi


	/* DYNAMIC ALLOCATION BEGIN: Calculate the size needed for the output image data */
	/*movl IMAGE_WIDTH_OFFSET(%rdi), %eax     /* Get width */
	/*mull IMAGE_HEIGHT_OFFSET(%rdi)           /* Multiply by height */
	/*imul $4, %eax                             /* Multiply by 4 (size of uint32_t) */
	

	/*movq rax, rdi       # Move the calculated size into rdi (malloc argument)

	/*call malloc          # Call malloc

	# After the call, the pointer to the allocated memory is now in rax
	/*movq rax, IMAGE_DATA_OFFSET(%r12)  # Store the pointer to the allocated data in the struct
	/*doesn't this just mean the input pointer points somewhere else so we can't get hte original image's data*/

 

	/*REST OF FUNCTION BEGIN*/
	movl $0, %r10d /*outer loop counter. Use caller saved. No need to reserve more space on stack. "d" for just 32 bits.*/
	movl $0, %r11d /*inner loop counter. Caller-saved*/
	/*at this point, r10d and r11d contain 0 (correct) */


	.LOuterLoop:
		cmpl -8(%rbp), %r10d /* if the outer loop counter r10d >= r12d = height, jump out.*/
		jge .LOuterLoopDone

		.LInnerLoop:
			
			cmpl (%r12d), %r11d			
			jge .LInnerLoopDone

			/*INITIAL CALCULATION FOR SOURCE INDEX*/

			movq (%r12), %rdx /* rdx gets the width, to be used in the [i][j] = col + (width * row) calculation*/
			/*BUG FIX: Changed %edi to (%edi) (before it was loading like 400,000 into edx)*/
			imul %r10d, %edx /* rdx gets (rdx * r10d) i.e. rdx = width * row. Use just lower 32 bits.*/
			/*r10d cannot be dereferenced. Multiply the literal value.*/

			addl %r11d, %edx /* rdx is now (width * row) + col, since r11d has the current col
			 this should be addl %r11d, %edx to use just the 32-bit versions!
			 I don't think I need to do any -1 here because curCol starts from 0 already, going left to right.
			 Up to this point, edx is 0 for the first loop iter - correct (for input image) */

			/*INITIAL CALCULATION FOR DEST INDEX*/

			/* outer loop counter * width + (width - curCol)*/
			movq (%rsi), %rcx/* rcx will be index of output img. load width (the value) into rcx
			ecx is 2 after this (first loop iter)
			This used to be edi. I think it should be esi. 
			Then I also changed to 64 bit versions cuz esi was negative so seg fault.*/
			subl %r11d, %ecx /*rcx now has (width - curCol) note curCol = inner loop counter. It is a literal value
			; so no need to dereference r11d
			after this, rcx is 2. Is that right? */

			movl %r10d, %r8d /* r8 will have (outer loop counter * width) to add to rcx. Currently has outer loop counter.
			; A literal value so no need to add.
			; QUESTION: should this be just the lower 32 bits?
			After this, rcx is 2. Is that right?*/
			imul (%r12d), %r8d /* r8 now has (outer loop counter * width). Just used lower 32 bits. Not sure if right.
			; dereferenced edi (lower 32 of rdi) to get the literal width value.
			After this, %r8d is 0 - correct, I think. */
			addl %r8d, %ecx /* rcx now has the output img index, (curRow * width + (width - curCol))
			; where curRow = outer loop counter, curCol = inner loop counter
			; using just the lower 32 bits. */
			/*for 2x2 image, ecx was 2 at this point*/

			dec %ecx /*needs to be (curRow * width + (width - curCol - 1) because of 0-based indexing
			it starts at width and goes from right to left, so need the -1 there I think. 
			used to be r8d, I think should be ecx
			ecx = 1 after this*/

			/*BEGINNING OF FINISHING CALCULATION FOR SOURCE INDEX*/

			imul $4, %edx /* multiply by size of uint32_t*/


			mov IMAGE_DATA_OFFSET(%r12), %r8 /*access "data" field of output - a memory address. 
			r8d contains the address of the first pixel now. (the address - not the value) */

			addl %r8d, %edx /*to get address of current pixel, add offset factor (edx) to address of first pixel
			edx = memory address of current source pixel*/

			/*GDB: for 2x2 image, edx was 8 at this point*/

			/*DEBUGGING QUESTION: What were edi, edx before this leal?
			$edx was 8
			$ecx was 1 (i think)
			*/

			/*Purpose: get to addres of current pixel by doing rdi + edx*/
			/*GDB:  (gdb) x/x $edi
					 0x40b040:       0x00000002
				    (gdb) x/x $edx
					 0x40b048:       0x0040b060
				    (gdb)   
			GDB: on first iter, edx is 8 more than edi. Should be right: edi is base of image, edx is first pixel 8 away
			in GDB, x/x $edx = x/x $rdi + 8 = x/x $edi + 8
			Plus, dereferencing 0x0040b060 yields the desired 0xff0000ff pixel val*/


			/*BEGINNING OF FINISHING CALCULATION FOR DEST INDEX*/
			imul $4, %ecx /* multiply by size of uint32_t*/
			/*GDB: After this, p $ecx = 4
			x/x $ecx fails: It is simply an address, not a value.*/
			movl IMAGE_DATA_OFFSET(%rsi), %r8d /* access the "data" field of output - this should be a mem address. 
			r8d should now have a mem address in it*/

			addl %r8d, %ecx /*ecx should now have the mem address value, plus the calculated offset.*/


			/*END OF CALCULATION FOR DEST INDEX*/

			/*GDB: After the above line, esi which should just be last 32 bits of rsi, is negative, but rsi is positive.

			CURRENT INPUT PIXEL: rdx ---- goes left to right
			CURRENT OUTPUT PIXEL: rcx ----- goes right to left

			*/
			movl (%rdx), %r9d      /* Load the 32-bit pixel data into r9d */
			movl %r9d, (%rcx)      /* Store the 32-bit value from r9d into the memory address in rcx */



			inc %r11d /*increment inner loop counter*/
			jmp .LInnerLoop /*next inner loop iter*/
			

		.LInnerLoopDone:
		inc %r10d /*increment outer loop counter*/
		movq $0, %r11 /*reset inner loop counter*/
		jmp .LOuterLoop /* next outer loop iter*/

	.LOuterLoopDone:
    /* epilogue.*/
	popq %r12  /*pop callee-saved registers (local vars)*/
	addq $24, %rsp
	popq %rbp
	ret

/*
 * void imgproc_mirror_v( struct Image *input_img, struct Image *output_img );
 *
 * Transform image by mirroring its pixels vertically.
 * This transformation always succeeds.
 *
 * Parameters:
 *   %rdi - pointer to original struct Image
 *   %rsi - pointer to output struct Image
 */
	.globl imgproc_mirror_v
imgproc_mirror_v:
/*NOTES: Must use 32-bit versions of the registers if I'm only operating on 32 bits of them.*/
	/* TODO: implement */
	pushq %rbp /* -8 from rsp. Aligned at this point.*/
	movq %rsp, %rbp
	subq $8, %rsp; /*1 local var: height (r12d). rsp will be shifted down by 8. Was aligned from above. 
	; so shift down by another 8, so that after adding the local var, it's aligned to mult of 16 again.
	; so right now it's misaligned but this is anticipating the local var push, after which will be aligned.
	; EDIT: Actually, shift by 16 to leave space at (rbp - 8?) for another local var. */

	/*PUSH CALLEE SAVED/LOCAL VARS*/
	pushq %r12 /*height. rdi has the width... NEED IT BE r12d? NO - stack is designed to push & pop 8 bytes at a time.
	; so push all 64 bits (pushq) even if only using bottom 32 (movl)*/

	/*movl IMAGE_HEIGHT_OFFSET(%rdi), %r12d; /*r12 will hold the VALUE (not mem address) of the height*/
	/*at this point, p $r12d = 2 for 2x2 image (correct) */

	/*Replace height assignment:*/
	/*movl IMAGE_HEIGHT_OFFSET(%rdi), %eax      # Load the height value into eax
	movl %eax, -8(%rbp)                        # Store the value from eax into the local variable at -8(%rsp)
	movq %rdi, %r12*/
	movl IMAGE_HEIGHT_OFFSET(%edi), %r12d; /*r12 will hold the VALUE (not mem address) of the height*/

 

	/*REST OF FUNCTION BEGIN*/
	movl $0, %r10d /*outer loop counter. Use caller saved. No need to reserve more space on stack. "d" for just 32 bits.*/
	movl $0, %r11d /*inner loop counter. Caller-saved*/
	/*at this point, r10d and r11d contain 0 (correct) */


	.LOuterLoopV:
		cmpl %r12d, %r10d /* if the outer loop counter r10d >= r12d = height, jump out.*/
		jge .LOuterLoopDoneV

		.LInnerLoopV:
			
			cmpl (%edi), %r11d			
			jge .LInnerLoopDoneV

			/*INITIAL CALCULATION FOR SOURCE INDEX*/

			movl (%edi), %edx /* rdx gets the width, to be used in the [i][j] = col + (width * row) calculation*/
			/*BUG FIX: Changed %edi to (%edi) (before it was loading like 400,000 into edx)*/
			imul %r10d, %edx /* rdx gets (rdx * r10d) i.e. rdx = width * row. Use just lower 32 bits.*/
			/*r10d cannot be dereferenced. Multiply the literal value.*/

			addl %r11d, %edx /* rdx is now (width * row) + col, since r11d has the current col
			 this should be addl %r11d, %edx to use just the 32-bit versions!
			 I don't think I need to do any -1 here because curCol starts from 0 already, going left to right.
			 Up to this point, edx is 0 for the first loop iter - correct (for input image) */



			/*INITIAL CALCULATION FOR DEST INDEX*/

			/* width(height - 1) - (width * row) + col
			NOTE row = outer loop counter; col = inner loop counter*/
			movq (%rsi), %rcx/* rcx will be index of output img. load width (the value) into rcx 
			rcx = 0x200000002 but ecx = 0x2. Fine?*/ 

			movl %r12d,%r8d /*r8d will get (height - 1)*/
			dec %r8d /*r8d has (height - 1)
			r8d = 1 3rd iter of 2x2. Good I think.*/
			
			imul %r8d, %ecx /*ecx now has width * (height - 1)
			p $ecx = 2 3rd iter of 2x2. Good I think.*/

			movl (%edi), %r8d /*r8 has the width
			DEBUGGING: Changed %edi to (%edi)*/
			imul %r10d, %r8d /*r8 now has width * row*/

			subl %r8d, %ecx /*ecx now has widh * (height - 1) - (width * row)*/

			addl %r11d, %ecx /*ecx now has width * (height - 1) - (width * row) + col*/
			/*inc %ecx /*ecx now has width * (height - 1) - (width * row) + col + 1
			Not sure if the last + 1 is needed...*/


			/*BEGINNING OF FINISHING CALCULATION FOR SOURCE INDEX*/

			imul $4, %edx /* multiply by size of uint32_t*/


			mov IMAGE_DATA_OFFSET(%rdi), %r8 /*access "data" field of output - a memory address. 
			r8d contains the address of the first pixel now. (the address - not the value) */

			addl %r8d, %edx /*to get address of current pixel, add offset factor (edx) to address of first pixel
			edx = memory address of current source pixel*/


			/*BEGINNING OF FINISHING CALCULATION FOR DEST INDEX*/
			imul $4, %ecx /* multiply by size of uint32_t*/
			/*GDB: After this, p $ecx = 4
			x/x $ecx fails: It is simply an address, not a value.*/
			movl IMAGE_DATA_OFFSET(%rsi), %r8d /* access the "data" field of output - this should be a mem address. 
			r8d should now have a mem address in it*/

			addl %r8d, %ecx /*ecx should now have the mem address value, plus the calculated offset.*/


			/*END OF CALCULATION FOR DEST INDEX*/

			/*GDB: After the above line, esi which should just be last 32 bits of rsi, is negative, but rsi is positive.

			CURRENT INPUT PIXEL: rdx ---- goes left to right
			CURRENT OUTPUT PIXEL: rcx ----- goes right to left

			*/
			movl (%rdx), %r9d      /* Load the 32-bit pixel data into r9d */
			movl %r9d, (%rcx)      /* Store the 32-bit value from r9d into the memory address in rcx */
			/*after one loop iter, x/x 8($rsi) + 8 (location of cur output pixel, the 3rd output pixel) correctly has
			val of first input pixel*/
			/*after two loop iter, looks like bottom row was mapped correctly: 
				gdb) x/x 0x0040c330 + 12
				0x40c33c:       0x00ff00ff
				(gdb) x/x 0x0040c330 + 8
				0x40c338:       0xff0000ff
				(gdb) */



			inc %r11d /*increment inner loop counter*/
			jmp .LInnerLoopV /*next inner loop iter*/
			

		.LInnerLoopDoneV:
		inc %r10d /*increment outer loop counter*/
		movq $0, %r11 /*reset inner loop counter*/
		jmp .LOuterLoopV /* next outer loop iter*/

	.LOuterLoopDoneV:
    /* epilogue.*/
	popq %r12  /*pop callee-saved registers (local vars)*/
	addq $8, %rsp
	popq %rbp
	ret

/*
 * int imgproc_tile( struct Image *input_img, int n, struct Image *output_img );
 *
 * Transform image by generating a grid of n x n smaller tiles created by
 * sampling every n'th pixel from the original image.
 *
 * Parameters:
 *   %rdi - pointer to original struct Image
 *   %esi - tiling factor (how many rows and columns of tiles to generate)
 *   %rdx - pointer to the output Image (in which the transformed
 *          pixels should be stored)
 *
 * Returns (in %eax):
 *   1 if successful, or 0 if either
 *     - n is less than 1, or
 *     - the output can't be generated because at least one tile would
 *       be empty (i.e., have 0 width or height)
 */
	.globl imgproc_tile
imgproc_tile:
	/* TODO: implement (this stub implementation just returns 0) */
	xorl %eax, %eax
	ret

/*
 * void imgproc_grayscale( struct Image *input_img, struct Image *output_img );
 *
 * Transform image by converting each pixel to grayscale.
 * This transformation always succeeds.
 *
 * Parameters:
 *   %rdi - pointer to original struct Image
 *   %rsi - pointer to output struct Image
 */
	.globl imgproc_grayscale
imgproc_grayscale:
    pushq %rbp
    movq %rsp, %rbp
    subq $16, %rsp           # Reserve stack space for local variables

    pushq %rbx               # Save callee-saved registers
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15

    movl IMAGE_HEIGHT_OFFSET(%rdi), %r12d   # Load input image height into %r12d
    movl IMAGE_WIDTH_OFFSET(%rdi), %ebx     # Load input image width into %ebx

    movq IMAGE_DATA_OFFSET(%rsi), %r9       # Output image data pointer
    movq IMAGE_DATA_OFFSET(%rdi), %r8       # Input image data pointer

    xorl %r10d, %r10d        # Outer loop counter (row index)

.LOuterLoopGray:
    cmpl %r12d, %r10d        # Compare outer loop counter with height
    jge .LOuterLoopGrayDone  # Jump if all rows are processed

    xorl %r11d, %r11d        # Reset inner loop counter (column index)

.LInnerLoopGray:
    cmpl %ebx, %r11d         # Compare inner loop counter with width
    jge .LInnerLoopGrayDone  # Jump if all columns are processed

    # Compute input pixel address
    movl %r10d, %edx                        # edx = row index
    imull %ebx, %edx                        # edx = row index * image width
    addl %r11d, %edx                        # edx = edx + column index
    leaq (%r8, %rdx, 4), %r14               # r14 = input image data pointer + offset
    movl (%r14), %eax                       # Load pixel value into %eax

    # Extract color components (red, green, blue)
    # Use %r13d for blue to avoid overwriting %ebx
    movl %eax, %ecx                         # ecx = pixel value

    # Extract red component
    movl %ecx, %edx                         # edx = pixel value
    shrl $24, %edx                          # Shift red component into lower 8 bits
    movl %edx, %esi                         # esi = red

    # Extract green component
    movl %ecx, %edx                         # edx = pixel value
    shrl $16, %edx                          # Shift green component into lower 8 bits
    andl $0xFF, %edx                        # Mask to get only green component
    movl %edx, %edi                         # edi = green

    # Extract blue component
    movl %ecx, %edx                         # edx = pixel value
    shrl $8, %edx                           # Shift blue component into lower 8 bits
    andl $0xFF, %edx                        # Mask to get only blue component
    movl %edx, %r13d                        # r13d = blue

    # Compute grayscale value: y = (79 * r + 128 * g + 49 * b) / 256
    imull $79, %esi                         # Multiply red by 79
    imull $128, %edi                        # Multiply green by 128
    addl %edi, %esi                         # Add green contribution to red
    imull $49, %r13d                        # Multiply blue by 49
    addl %r13d, %esi                        # Add blue contribution to the result
    shrl $8, %esi                           # Divide by 256 to get grayscale value (y)

    # Build the grayscale pixel with correct shifts
    movl %esi, %eax                         # eax = grayscale value

    movl %esi, %edx                         # edx = grayscale value
    shll $24, %eax                          # Shift grayscale to red component position
    shll $16, %edx                          # Shift grayscale to green component position
    orl  %edx, %eax                         # Combine green component

    movl %esi, %edx                         # edx = grayscale value
    shll $8, %edx                           # Shift grayscale to blue component position
    orl  %edx, %eax                         # Combine blue component

    orl  $0xFF, %eax                        # Set alpha to 0xFF

    # Compute output pixel address
    movl %r10d, %edx                        # edx = row index
    imull %ebx, %edx                        # edx = row index * image width
    addl %r11d, %edx                        # edx = edx + column index
    leaq (%r9, %rdx, 4), %r15               # r15 = output image data pointer + offset

    movl %eax, (%r15)                       # Store grayscale pixel into output image

    inc %r11d                               # Increment inner loop counter
    jmp .LInnerLoopGray                     # Repeat for next column

.LInnerLoopGrayDone:
    inc %r10d                               # Increment outer loop counter
    jmp .LOuterLoopGray                     # Repeat for next row

.LOuterLoopGrayDone:
    popq %r15                               # Restore callee-saved registers
    popq %r14
    popq %r13
    popq %r12
    popq %rbx
    addq $16, %rsp                          # Restore stack pointer
    popq %rbp
    ret
/*
    pushq %rbp
    movq %rsp, %rbp
    subq $16, %rsp           # Reserve stack space for local variables

    pushq %rbx               # Save callee-saved registers
    pushq %r12
    pushq %r13
    pushq %r14

    movl IMAGE_HEIGHT_OFFSET(%rdi), %r12d   # Load input image height into %r12d
    movl IMAGE_WIDTH_OFFSET(%rdi), %ebx     # Load input image width into %ebx

    movq IMAGE_DATA_OFFSET(%rsi), %r9       # Output image data pointer
    movq IMAGE_DATA_OFFSET(%rdi), %r8       # Input image data pointer

    xorl %r10d, %r10d        # Outer loop counter (row index)

.LOuterLoopGray:
    cmpl %r12d, %r10d        # Compare outer loop counter with height
    jge .LOuterLoopGrayDone  # Jump if all rows are processed

    xorl %r11d, %r11d           # Reset inner loop counter (column index)

.LInnerLoopGray:
    cmpl %ebx, %r11d         # Compare inner loop counter with width
    jge .LInnerLoopGrayDone  # Jump if all columns are processed

    # Compute input pixel address
    movl %r10d, %edx                        # edx = row index
    imull %ebx, %edx                        # edx = row index * image width
    addl %r11d, %edx                        # edx = edx + column index
    leaq (%r8, %rdx, 4), %rcx               # rcx = input image data pointer + offset
    movl (%rcx), %eax                       # Load pixel value into %eax

    # Extract color components (red, green, blue)
    # Use %r13d for blue to avoid overwriting %ebx
    movl %eax, %edx                         # Copy pixel value to %edx
    shrl $24, %edx                          # Shift red component into lower 8 bits
    movl %edx, %esi                         # Save red component in %esi (r = %esi)

    movl %eax, %edx                         # Copy pixel value again to %edx
    shrl $16, %edx                          # Shift green component into lower 8 bits
    andl $0xFF, %edx                        # Mask to get only green component
    movl %edx, %edi                         # Save green component in %edi (g = %edi)

    movl %eax, %edx                         # Copy pixel value again to %edx
    shrl $8, %edx                           # Shift blue component into lower 8 bits
    andl $0xFF, %edx                        # Mask to get only blue component
    movl %edx, %r13d                        # Save blue component in %r13d (b = %r13d)

    # Compute grayscale value: y = (79 * r + 128 * g + 49 * b) / 256
    imull $79, %esi                         # Multiply red by 79
    imull $128, %edi                        # Multiply green by 128
    addl %edi, %esi                         # Add green contribution to red
    imull $49, %r13d                        # Multiply blue by 49
    addl %r13d, %esi                        # Add blue contribution to the result
    shrl $8, %esi                           # Divide by 256 to get grayscale value (y)

    # Build the grayscale pixel with correct shifts
    movl %esi, %eax                         # %eax = grayscale value

    shll $24, %eax                          # Shift grayscale to red component position

    movl %esi, %edx                         # %edx = grayscale value
    shll $16, %edx                          # Shift grayscale to green component position
    orl  %edx, %eax                         # Combine green component

    movl %esi, %edx                         # %edx = grayscale value
    shll $8, %edx                           # Shift grayscale to blue component position
    orl  %edx, %eax                         # Combine blue component

    orl  $0xFF, %eax                        # Set alpha to 0xFF

    # Compute output pixel address
    movl %r10d, %edx                        # edx = row index
    imull %ebx, %edx                        # edx = row index * image width
    addl %r11d, %edx                        # edx = edx + column index
    leaq (%r9, %rdx, 4), %rdx               # rdx = output image data pointer + offset

    movl %eax, (%rdx)                       # Store grayscale pixel into output image

    inc %r11d                               # Increment inner loop counter
    jmp .LInnerLoopGray                     # Repeat for next column

.LInnerLoopGrayDone:
    inc %r10d                               # Increment outer loop counter
    jmp .LOuterLoopGray                     # Repeat for next row

.LOuterLoopGrayDone:
    popq %r14                               # Restore callee-saved registers
    popq %r13
    popq %r12
    popq %rbx
    addq $16, %rsp                          # Restore stack pointer
    popq %rbp
    ret
*/
/*
    pushq %rbp
    movq %rsp, %rbp
    subq $16, %rsp           # Reserve stack space for local variables

    pushq %rbx               # Save callee-saved registers
    pushq %r12

    movl IMAGE_HEIGHT_OFFSET(%rdi), %r12d   # Load input image height into %r12d
    movl IMAGE_WIDTH_OFFSET(%rdi), %ebx     # Load input image width into %ebx

    movq IMAGE_DATA_OFFSET(%rsi), %r9       # Output image data pointer
    movq IMAGE_DATA_OFFSET(%rdi), %r8       # Input image data pointer

    xorl %r10d, %r10d        # Outer loop counter (row index)
    # Note: No need to initialize %r11d here; it will be initialized in the inner loop

.LOuterLoopGray:
    cmpl %r12d, %r10d        # Compare outer loop counter with height
    jge .LOuterLoopGrayDone  # Jump if all rows are processed

    movl $0, %r11d           # Reset inner loop counter (column index)

.LInnerLoopGray:
    cmpl %ebx, %r11d         # Compare inner loop counter with width
    jge .LInnerLoopGrayDone  # Jump if all columns are processed

    # Compute input pixel address
    movl %r10d, %ecx                        # ecx = row index
    imull %ebx, %ecx                        # ecx = row index * image width
    addl %r11d, %ecx                        # ecx = ecx + column index
    leaq (%r8, %rcx, 4), %rcx               # rcx = input image data pointer + offset
    movl (%rcx), %eax                       # Load pixel value into %eax

    # Extract color components (red, green, blue)
    movl %eax, %edx                         # Copy pixel value to %edx
    shrl $24, %edx                          # Shift red component into lower 8 bits
    movl %edx, %esi                         # Save red component in %esi (r = %esi)

    movl %eax, %edx                         # Copy pixel value again to %edx
    shrl $16, %edx                          # Shift green component into lower 8 bits
    andl $0xFF, %edx                        # Mask to get only green component
    movl %edx, %edi                         # Save green component in %edi (g = %edi)

    movl %eax, %edx                         # Copy pixel value again to %edx
    shrl $8, %edx                           # Shift blue component into lower 8 bits
    andl $0xFF, %edx                        # Mask to get only blue component
    movl %edx, %ebx                         # Save blue component in %ebx (b = %ebx)

    # Compute grayscale value: y = (79 * r + 128 * g + 49 * b) / 256
    imull $79, %esi                         # Multiply red by 79
    imull $128, %edi                        # Multiply green by 128
    addl %edi, %esi                         # Add green contribution to red
    imull $49, %ebx                         # Multiply blue by 49
    addl %ebx, %esi                         # Add blue contribution to the result
    shrl $8, %esi                           # Divide by 256 to get grayscale value (y)

    # Build the grayscale pixel with correct shifts
    xorl %eax, %eax                         # Clear %eax to zero
    movl %esi, %eax                         # Move grayscale value to %eax

    shll $24, %eax                          # Shift grayscale to red component position

    movl %esi, %edx                         # Copy grayscale value to %edx
    shll $16, %edx                          # Shift grayscale to green component position
    orl  %edx, %eax                         # Combine green component

    movl %esi, %edx                         # Copy grayscale value to %edx
    shll $8, %edx                           # Shift grayscale to blue component position
    orl  %edx, %eax                         # Combine blue component

    orl  $0xFF, %eax                        # Set alpha to 0xFF

    # Compute output pixel address
    movl %r10d, %edx                        # edx = row index
    imull %ebx, %edx                        # edx = row index * image width
    addl %r11d, %edx                        # edx = edx + column index
    leaq (%r9, %rdx, 4), %rdx               # rdx = output image data pointer + offset

    movl %eax, (%rdx)                       # Store grayscale pixel into output image

    inc %r11d                               # Increment inner loop counter
    jmp .LInnerLoopGray                     # Repeat for next column

.LInnerLoopGrayDone:
    inc %r10d                               # Increment outer loop counter
    jmp .LOuterLoopGray                     # Repeat for next row

.LOuterLoopGrayDone:
    popq %r12                               # Restore callee-saved registers
    popq %rbx
    addq $16, %rsp                          # Restore stack pointer
    popq %rbp
    ret
*/
/*
    pushq %rbp
    movq %rsp, %rbp
    subq $16, %rsp           # Reserve stack space for local variables

    pushq %rbx               # Save callee-saved registers
    pushq %r12

    movl IMAGE_HEIGHT_OFFSET(%rdi), %r12d   # Load input image height into r12d
    movl IMAGE_WIDTH_OFFSET(%rdi), %ebx     # Load input image width into ebx

    # added
    movq IMAGE_DATA_OFFSET(%rsi), %r9       
    movq IMAGE_DATA_OFFSET(%rdi), %r8

    xorl %r10d, %r10d        # Outer loop counter (row index)
    xorl %r11d, %r11d        # Inner loop counter (column index)

.LOuterLoopGray:
    cmpl %r12d, %r10d        # Compare outer loop counter with height
    jge .LOuterLoopGrayDone      # Jump if all rows are processed

    movl $0, %r11d           # Reset inner loop counter (column index)

.LInnerLoopGray:
    cmpl %ebx, %r11d         # Compare inner loop counter with width
    jge .LInnerLoopGrayDone      # Jump if all columns are processed

    # Compute input pixel address
    movq IMAGE_DATA_OFFSET(%rdi), %r8       # Load pointer to input image data
    movl %r10d, %ecx                        # Load row index into ecx
    imul %ebx, %ecx                         # Multiply row index by image width
    addl %r11d, %ecx                        # Add column index to ecx
    imul $4, %ecx                           # Multiply by 4 to get byte offset (uint32_t size)
    addq %r8, %rcx                          # Add base address of data
    movl (%rcx), %eax                       # Load pixel value into eax

    # Extract color components (red, green, blue)
    movl %eax, %edx                         # Copy pixel value to edx
    shrq $24, %rdx                          # Shift red component into lower 8 bits
    movl %edx, %esi                         # Save red component in esi (r = %esi)

    movl %eax, %edx                         # Copy pixel value again to edx
    shrq $16, %rdx                          # Shift green component into lower 8 bits
    andl $0xFF, %edx                        # Mask to get only green component
    movl %edx, %edi                         # Save green component in edi (g = %edi)

    movl %eax, %edx                         # Copy pixel value again to edx
    shrq $8, %rdx                           # Shift blue component into lower 8 bits
    andl $0xFF, %edx                        # Mask to get only blue component
    movl %edx, %ebx                         # Save blue component in ebx (b = %ebx)

    # Compute grayscale value: y = (79 * r + 128 * g + 49 * b) / 256
    imull $79, %esi                         # Multiply red by 79
    imull $128, %edi                        # Multiply green by 128
    imull $49, %ebx                         # Multiply blue by 49
    addl %edi, %esi                         # Add green contribution to red
    addl %ebx, %esi                         # Add blue contribution to the result
    shrl $8, %esi                           # Divide by 256 to get grayscale value (y)

    # Set output pixel: gray in RGB, alpha unchanged
    # andl $0xFF, %eax                  # Mask alpha from original pixel
    # orl %esi, %eax                          # Combine grayscale with alpha

    movl %esi, %eax                        # Copy grayscale value to %eax (preserve it for later use)

    shll $24, %esi                         # Shift grayscale value to red channel position (bits 24-31)
    movl %eax, %edx                        # Copy grayscale value to %edx
    shll $16, %edx                          # Shift grayscale value to green channel position (bits 16-23)

    orl %edx, %esi                         # Combine red and green channel grayscale values
    orl %eax, %esi                         # Combine blue channel (bits 8-15) grayscale value
    orl $0x000000FF, %esi                  # Set the alpha value to 0xFF

    movl %r10d, %ecx                        # Load row index into ecx
    imul %ebx, %ecx                         # Multiply row index by image width
    addl %r11d, %ecx                        # Add column index to ecx
    imul $4, %ecx                           # Multiply by 4 to get byte offset (uint32_t size)
    addq %r9, %rcx                          # Add base address of data
    movl %esi, (%rcx)                       # Store grayscale pixel into output image

    inc %r11d                               # Increment inner loop counter
    jmp .LInnerLoopGray                         # Repeat for next column

.LInnerLoopGrayDone:
    inc %r10d                               # Increment outer loop counter
    jmp .LOuterLoopGray                        # Repeat for next row

.LOuterLoopGrayDone:
    popq %r12                               # Restore callee-saved registers
    popq %rbx
    addq $16, %rsp                          # Restore stack pointer
    popq %rbp
    ret
*/

/*
 * int imgproc_composite( struct Image *base_img, struct Image *overlay_img );
 *
 * Overlay a foreground image on a background image, using each foreground
 * pixel's alpha value to determine its degree of opacity in order to blend
 * it with the corresponding background pixel.
 *
 * Parameters:
 *   %rdi - pointer to base (background) image
 *   %rsi - pointer to overlaid (foreground) image
 *   %rdx - pointer to output Image
 *
 * Returns:
 *   1 if successful, or 0 if the transformation fails because the base
 *   and overlay image do not have the same dimensions
 */
	.globl imgproc_composite
imgproc_composite:
	/* TODO: implement (this stub implementation just returns 0) */
	xorl %eax, %eax
	ret

/*
vim:ft=gas:
*/
