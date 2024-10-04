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
 * Mirror image horizontally by reversing the columns.
 * 
 * Parameters:
 *   %rdi - pointer to original struct Image
 *   %rsi - pointer to output struct Image
 */
    .globl imgproc_mirror_h
imgproc_mirror_h:
    # Prologue
    pushq %rbp
    movq %rsp, %rbp
    subq $16, %rsp # reserve stack space for local vars

    # Save callee-saved registers
    pushq %rbx
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15
    pushq %rsi # Save %rsi (pixel index)
    pushq %rbp # Save %rbp (temporary storage)

    # Load input image dimensions
    movl IMAGE_HEIGHT_OFFSET(%rdi), %r12d # %r12d = input image height
    movl IMAGE_WIDTH_OFFSET(%rdi), %r13d  # %r13d = input image width

    # Load data pointers
    movq IMAGE_DATA_OFFSET(%rdi), %r8 # %r8 = input image data ptr
    movq IMAGE_DATA_OFFSET(%rsi), %r9 # %r9 = output image data ptr

    xorl %r10d, %r10d # %r10d = row i (outer loop counter)

.LOuterLoopMirror:
    cmpl %r12d, %r10d # compare row index and height
    jge .LOuterLoopMirrorDone # exit if all rows are processed

    xorl %r11d, %r11d # %r11d = column i (inner loop counter)

.LInnerLoopMirror:
    cmpl %r13d, %r11d # compare column index and width
    jge .LInnerLoopMirrorDone # exit if all columns are processed

    # Calculate the source pixel index (width * row + col)
    movl %r10d, %esi # %esi = row index
    imull %r13d, %esi # %esi = row * width
    addl %r11d, %esi # %esi = pixel index (row * width + col)

    # Calculate the destination pixel index (row * width + (width - col - 1))
    movl %r13d, %edx # %edx = width
    subl %r11d, %edx # %edx = width - col
    decl %edx        # %edx = width - col - 1
    movl %r10d, %ebx # %ebx = row index
    imull %r13d, %ebx # %ebx = row * width
    addl %edx, %ebx # %ebx = destination pixel index

    # Load the source pixel value
    leaq (%r8, %rsi, 4), %r14 # %r14 = input image data ptr + source offset
    movl (%r14), %ecx  # %ecx = source pixel value

    # Calculate the destination pixel address and store the pixel value
    leaq (%r9, %rbx, 4), %r15 # %r15 = output image data ptr + destination offset
    movl %ecx, (%r15) # Store source pixel value in output image

    # Increment column i
    incl %r11d
    jmp .LInnerLoopMirror

.LInnerLoopMirrorDone:
    # Increment row i
    incl %r10d
    jmp .LOuterLoopMirror

.LOuterLoopMirrorDone:
    # Restore callee-saved registers
    popq %rbp
    popq %rsi
    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %rbx
    addq $16, %rsp
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
	movq IMAGE_HEIGHT_OFFSET(%rdi), %r12; /*r12 will hold the VALUE (not mem address) of the height*/

 

	/*REST OF FUNCTION BEGIN*/
	movq $0, %r10 /*outer loop counter. Use caller saved. No need to reserve more space on stack. "d" for just 32 bits.*/
	movq $0, %r11 /*inner loop counter. Caller-saved*/
	/*at this point, r10d and r11d contain 0 (correct) */


	.LOuterLoopV:
		cmpq %r12, %r10 /* if the outer loop counter r10d >= r12d = height, jump out.*/
		jge .LOuterLoopDoneV

		.LInnerLoopV:
			
			cmpq (%rdi), %r11			
			jge .LInnerLoopDoneV

			/*INITIAL CALCULATION FOR SOURCE INDEX*/

			movq (%rdi), %rdx /* rdx gets the width, to be used in the [i][j] = col + (width * row) calculation*/
			/*BUG FIX: Changed %edi to (%edi) (before it was loading like 400,000 into edx)*/
			imulq %r10, %rdx /* rdx gets (rdx * r10d) i.e. rdx = width * row. Use just lower 32 bits.*/
			/*r10d cannot be dereferenced. Multiply the literal value.*/

			addq %r11, %rdx /* rdx is now (width * row) + col, since r11d has the current col
			 this should be addl %r11d, %edx to use just the 32-bit versions!
			 I don't think I need to do any -1 here because curCol starts from 0 already, going left to right.
			 Up to this point, edx is 0 for the first loop iter - correct (for input image) */



			/*INITIAL CALCULATION FOR DEST INDEX*/

			/* width(height - 1) - (width * row) + col
			NOTE row = outer loop counter; col = inner loop counter*/
			movq IMAGE_WIDTH_OFFSET(%rsi), %rcx/* rcx will be index of output img. load width (the value) into rcx 
			rcx = 0x200000002 but ecx = 0x2. Fine?*/ 

			movq %r12,%r8 /*r8d will get (height - 1)*/
			dec %r8d /*r8d has (height - 1)
			r8d = 1 3rd iter of 2x2. Good I think.*/
			
			imulq %r8, %rcx /*ecx now has width * (height - 1)
			p $ecx = 2 3rd iter of 2x2. Good I think.*/

			movq IMAGE_WIDTH_OFFSET(%rdi), %r8 /*r8 has the width
			DEBUGGING: Changed %edi to (%edi)*/
			imulq %r10, %r8 /*r8 now has width * row*/

			subq %r8, %rcx /*ecx now has widh * (height - 1) - (width * row)*/

			addq %r11, %rcx /*ecx now has width * (height - 1) - (width * row) + col*/
			/*inc %ecx /*ecx now has width * (height - 1) - (width * row) + col + 1
			Not sure if the last + 1 is needed...*/


			/*BEGINNING OF FINISHING CALCULATION FOR SOURCE INDEX*/

			imul $4, %rdx /* multiply by size of uint32_t*/


			movq IMAGE_DATA_OFFSET(%rdi), %r8 /*access "data" field of output - a memory address. 
			r8d contains the address of the first pixel now. (the address - not the value) */

			addq %r8, %rdx /*to get address of current pixel, add offset factor (edx) to address of first pixel
			edx = memory address of current source pixel*/


			/*BEGINNING OF FINISHING CALCULATION FOR DEST INDEX*/
			imulq $4, %rcx /* multiply by size of uint32_t*/
			/*GDB: After this, p $ecx = 4
			x/x $ecx fails: It is simply an address, not a value.*/
			movq IMAGE_DATA_OFFSET(%rsi), %r8 /* access the "data" field of output - this should be a mem address. 
			r8d should now have a mem address in it*/

			addq %r8, %rcx /*ecx should now have the mem address value, plus the calculated offset.*/


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
    # prologue *MOST RECENT PUSH*
    pushq %rbp
    movq %rsp, %rbp
    subq $16, %rsp # reserve stack space for local vars

    # save callee-saved registers
    pushq %rbx               
    pushq %r12               
    pushq %r13               
    pushq %r14               
    pushq %r15               
    pushq %rsi # Save %rsi (pixel index)
    pushq %rbp # Save %rbp (temporary storage)

    movl IMAGE_HEIGHT_OFFSET(%rdi), %r12d # %r12d = input image height
    movl IMAGE_WIDTH_OFFSET(%rdi), %r13d # %r13d = input image width

    # data ptrs
    movq IMAGE_DATA_OFFSET(%rdi), %r8 # %r8 = input image data ptr
    movq IMAGE_DATA_OFFSET(%rsi), %r9 # %r9 = output image data ptr

    xorl %r10d, %r10d # %r10d = row i (outer loop counter)

.LOuterLoopGray:
    cmpl %r12d, %r10d # compare row index and height
    jge .LOuterLoopGrayDone # exit if all rows are processed

    xorl %r11d, %r11d # %r11d = column i (inner loop counter)

.LInnerLoopGray:
    cmpl %r13d, %r11d # compare column index and width
    jge .LInnerLoopGrayDone # exit if all columns are processed

    # pixel index (row * width + column)
    movl %r10d, %esi # %esi = row index (using %rsi/%esi)
    imull %r13d, %esi # %esi = row * width
    addl %r11d, %esi # %esi = pixel index

    # input pixel address
    leaq (%r8, %rsi, 4), %r14 # %r14 = input image data ptr + offset

    movl (%r14), %ecx  # %ecx = pixel value

    # output pixel address
    leaq (%r9, %rsi, 4), %r15 # %r15 = output image data pointer + offset

    # registers for temporary storage -> %eax, %edx, %ebx, %edi
    movl %ecx, %eax # %eax = pixel value
    shrl $24, %eax # %eax = red 

    movl %ecx, %edx # %edx = pixel value
    shrl $16, %edx # shift right 16 bits
    andl $0xFF, %edx # mask to get green 

    movl %ecx, %ebx # %ebx = pixel value
    shrl $8, %ebx # shift right 8 bits
    andl $0xFF, %ebx # mask to get blue 

    # grayscale: y = (79*r + 128*g + 49*b) / 256
    imull $79, %eax, %eax # %eax = 79 * red
    imull $128, %edx, %edx # %edx = 128 * green
    addl %edx, %eax # %eax += 128 * green
    imull $49, %ebx, %ebx # %ebx = 49 * blue
    addl %ebx, %eax # %eax += 49 * blue
    shrl $8, %eax # %eax = grayscale value

    # grayscale pixel
    movl %eax, %ecx # %ecx = grayscale 

    shll $24, %eax # shift grayscale to red pos
    movl %ecx, %edx # %edx = grayscale value
    shll $16, %edx # shift grayscale to green pos
    orl %edx, %eax # green into %eax

    movl %ecx, %ebx # %ebx = grayscale value
    shll $8, %ebx # shift grayscale to blue pos
    orl %ebx, %eax # blue into %eax

    orl $0xFF, %eax # alpha to 0xFF or 0x000000FF

    # grayscale pixel into output image
    movl %eax, (%r15)

    # increment column i
    incl %r11d
    jmp .LInnerLoopGray

.LInnerLoopGrayDone:
    # increment row i
    incl %r10d
    jmp .LOuterLoopGray

.LOuterLoopGrayDone:
    # restore callee-saved registers
    popq %rbp               
    popq %rsi               
    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %rbx
    addq $16, %rsp
    popq %rbp
    ret

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
    # Function prologue
    pushq %rbp
    movq %rsp, %rbp
    subq $40, %rsp # for the three local vars below.
   # subq $8, %rsp # Total: call + pushq + 2 LV + 5 reg = 9 --> odd mult of 8, sub $8 %rsp

    # Save callee-saved registers
    # LOCAL VAR at -8(%rbp)
    # LOCAL VAR at -16(%rbp)
    # LOCAL VAR at -24(%rbp)
    # LOCAL VAR at -32(%rbp) - for the width x height originally in r11 which was being modified after malloc
    pushq %rbx                       # Save %rbx
    pushq %r12                       # Save %r12
    pushq %r13                       # Save %r13
    pushq %r14                       # Save %r14
    pushq %r15                       # Save %r15


    # Copy function arguments into callee-saved registers
    # Arguments:
    #   %rdi - base_img
    #   %rsi - overlay_img
    #   %rdx - output_img

    # movq %rdi, %r13                  # %r13 = base_img pointer
    movq %rdi, -8(%rbp)                # LV for base_img pointer
    movq -8(%rbp), %r13                # %r13 temp holds base_img ptr

    # movq %rsi, %r14                  # %r14 = overlay_img pointer
    movq %rsi, -16(%rbp)               # LV for overlay_img pointer
    movq -16(%rbp), %r14               # %r14 temp holds overlay_img ptr

    movq %rdx, %r15                    # %r15 = output_img pointer

    # Load base_img->width and base_img->height
    movl IMAGE_WIDTH_OFFSET(%r13), %r11d   # %r11 = base_img->width (size_t)
    movl IMAGE_HEIGHT_OFFSET(%r13), %r12d  # %r12 = base_img->height (size_t)

    # Load overlay_img->width and overlay_img->height
    movl IMAGE_WIDTH_OFFSET(%r14), %r8d    # %r8 = overlay_img->width (size_t)
    movl IMAGE_HEIGHT_OFFSET(%r14), %r9d   # %r9 = overlay_img->height (size_t)

    # Compare widths
    cmpl %r8d, %r11d   # Compare base_img->width and overlay_img->width
    jne dimensions_mismatch

    # Compare heights
    cmpl %r9d, %r12d    # Compare base_img->height and overlay_img->height
    jne dimensions_mismatch

    # Dimensions match
    # Set output_img->width and output_img->height
    movq %r11, IMAGE_WIDTH_OFFSET(%r15)    # output_img->width = base_img->width
    movq %r12, IMAGE_HEIGHT_OFFSET(%r15)   # output_img->height = base_img->height

    # Compute total number of pixels: total_pixels = width * height
    imulq %r12, %r11                        # %r11 = width * height (total_pixels)
    # EDIT (ADDING IN)
    movq %r11, -32(%rbp)                    # r11 changed after malloc

    # Allocate memory for output_img->data
    movq %r11, %rax                         # %rax = total_pixels
    shlq $2, %rax                           # %rax = total_pixels * 4 (bytes per pixel)
    movq %rax, %rdi                         # %rdi = size in bytes (for malloc)
    call malloc                             # Allocate memory
    movq %rax, IMAGE_DATA_OFFSET(%r15)      # output_img->data = allocated memory

    movq -32(%rbp), %r11                    # restore original value of r11 = width x height

    # Load data pointers
   /*NOW DONE INSIDE LOOP: movq IMAGE_DATA_OFFSET(%r13), %r8       # %r8 = base_img->data
    movq IMAGE_DATA_OFFSET(%r14), %r9       # %r9 = overlay_img->data*/
    movq %rax, -24(%rbp)
    /*BEFORE USING -24(%rbp) & NOW DONE INSIDE LOOP: movq %rax, %r10                         # %r10 = output_img->data*/

    # Load 255 into %r14d for division
   # EDIT: Do this inside loop. movl $255, %r14d                        # %r14d = 255 (divisor)

    # Set up loop counter
    xorq %rsi, %rsi                         # %rsi = i = 0 (pixel index)

    # total_pixels is in %r11

composite_loop:
    cmpq %r11, %rsi                         # Compare i with total_pixels
    jge composite_done                      # If i >= total_pixels, exit loop

    # EDIT Load data pointers - now done inside loop since r8, r13, r14 modified inside loop
    movq -8(%rbp), %r13                     # %r13 temp holds base_img ptr
    movq -16(%rbp), %r14                    # %r14 temp holds overlay_img ptr

    movq IMAGE_DATA_OFFSET(%r13), %r8       # %r8 = base_img->data
    movq IMAGE_DATA_OFFSET(%r14), %r9       # %r9 = overlay_img->data
    movq -24(%rbp), %r10                    # %r10 = output_img->data


    # Compute addresses of pixels
    leaq (%r8,%rsi,4), %rax                 # %rax = &base_img->data[i]
    leaq (%r9,%rsi,4), %rbx                 # %rbx = &overlay_img->data[i]
    leaq (%r10,%rsi,4), %rcx                # %rcx = &output_img->data[i]

    # Load base pixel
    movl (%rax), %edx                       # %edx = base_pixel -- "p $rdx" contains the pixel value 0xff0000ff

    # Load overlay pixel
    movl (%rbx), %edi                       # %edi = overlay_pixel

    # Extract overlay alpha
    movl %edi, %r8d
    andl $0xFF, %r8d                        # %r8d = o_alpha

    # Check alpha value
    testl %r8d, %r8d                        # Test if o_alpha == 0
    jz copy_base_pixel                      # If zero, copy base pixel

    cmpl $255, %r8d                         # Compare o_alpha with 255
    je copy_overlay_pixel                   # If 255, copy overlay pixel

    # Compute inverse alpha
    movl $255, %r9d                         # %r9d = 255
    subl %r8d, %r9d                         # %r9d = inv_alpha = 255 - o_alpha

    # Extract base RGB components
    movl %edx, %r12d                        # %r12d = base_pixel
    shrl $8, %r12d                          # Discard alpha. Blue is now in front.
    # EDIT in the above - used to be "shrl". Shift right to get rid of alpha

    # Extract overlay RGB components
    movl %edi, %r13d                        # %r13d = overlay_pixel
    shrl $8, %r13d                          # Discard alpha
    # EDIT in the above - used to be "shrl". Shift right to get rid of alpha


    # Blend RGB components
    # Initialize blended_color to zero
    # FORMER: xorl %ebp, %ebp                         # %ebp = 0
    # r10 used to be the output image data ptr
    xorq %r10, %r10                         # %r10 = 0


    # --- Blend Blue Component ---
    # Base Blue
    movl %r12d, %eax
    andl $0xFF, %eax
    imull %r9d, %eax                        # base_blue * inv_alpha

    # Overlay Blue
    movl %r13d, %ebx
    andl $0xFF, %ebx
    imull %r8d, %ebx                        # o_blue * o_alpha

    # Add and divide
    addl %ebx, %eax                         # %eax = blended_blue * 255
    # addl $127, %eax                         # For rounding

    # Prepare for division
    movl $255, %r14d                        # %r14d = 255 (divisor)

    xorl %edx, %edx                         # Clear %edx before division (edx:eax pair)
    divl %r14d                              # %eax = blended_blue

    andl $0xFF, %eax                        # zero out the parts we aren't interested in
    movl %eax, %r10d                        # %r10d = blended_blue

    # --- Blend Green Component ---
    # Base Green
    movl %r12d, %eax
    shrl $8, %eax                           # shift green component to the front 
    # EDIT the above - used to be "shrl"
    andl $0xFF, %eax                        # zero out the rest
    imull %r9d, %eax                        # base_green * inv_alpha

    # Overlay Green
    movl %r13d, %ebx
    shrl $8, %ebx
    # EDIT the above - used to be "shrl"
    andl $0xFF, %ebx
    imull %r8d, %ebx                        # o_green * o_alpha

    # Add and divide
    addl %ebx, %eax                         # %eax = blended_green * 255 - Jalil: (oGreen * oAlpha) + (bGreen * invAlpha) (same)
    # addl $127, %eax                         # For rounding

    # Prepare for division
    xorl %edx, %edx                         # Clear %edx before division
    divl %r14d                              # %eax = blended_green

    andl $0xFF, %eax
    shll $8, %eax
    orl %eax, %r10d                          # %ebp |= blended_green << 8

    # --- Blend Red Component ---
    # Base Red
    movl %r12d, %eax
    shrl $16, %eax
    # EDIT the above - used to be "shrl"
    andl $0xFF, %eax
    imull %r9d, %eax                        # base_red * inv_alpha

    # Overlay Red
    movl %r13d, %ebx
    shrl $16, %ebx
    # EDIT the above - used to be "shrl"
    andl $0xFF, %ebx
    imull %r8d, %ebx                        # o_red * o_alpha

    # Add and divide
    addl %ebx, %eax                         # %eax = blended_red * 255
    # addl $127, %eax                         # For rounding

    # Prepare for division
    xorl %edx, %edx                         # Clear %edx before division
    divl %r14d                              # %eax = blended_red

    andl $0xFF, %eax
    shll $16, %eax
    orl %eax, %r10d                          # %ebp |= blended_red << 16

    # Set alpha to 0xFF
    shll $8, %r10d                          # make room for Alpha
    orl $0xFF, %r10d                        # Set alpha to 255*/

    # Store blended pixel into output image
    movl %r10d, (%rcx)

    # Increment index
    incq %rsi

    jmp composite_loop

copy_overlay_pixel:
    # Store overlay pixel into output image
    movl %edi, (%rcx)
    # Increment index
    incq %rsi
    jmp composite_loop

copy_base_pixel:
    # Store base pixel into output image
    movl %edx, (%rcx)
    # Increment index
    incq %rsi
    jmp composite_loop

dimensions_mismatch:
    # Set return value to 0
    movl $0, %eax
    jmp function_end

composite_done:
    # Set return value to 1
    movl $1, %eax

function_end:
    # Restore callee-saved registers in reverse order
    popq %r15               # Restore %r15
    popq %r14               # Restore %r14
    popq %r13               # Restore %r13
    popq %r12               # Restore %r12
    popq %rbx               # Restore %rbx
    addq $40, %rsp
    popq %rbp               # Restore base pointer
    ret                     # Return from function
/*
vim:ft=gas:
*/
