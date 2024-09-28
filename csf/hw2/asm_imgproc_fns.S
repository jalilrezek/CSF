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
	/* TODO: implement */
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
	/* TODO: implement */
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
	/* TODO: implement */
    # prologue
    pushq %rbp # old base ptr
    movq %rsp, %rbp # new base ptr

    # save registers to use
    pushq %rbx                
    pushq %r12               
    pushq %r13                
    pushq %r14                
    pushq %r15               

    movq %rdi, %r14 # %r14 = input_img ptr
    movq %rsi, %r15 # %r15 = output_img ptr

    movl IMAGE_WIDTH_OFFSET(%r14), %eax # load input_img->width into %eax
    movl %eax, IMAGE_WIDTH_OFFSET(%r15) # store %eax to output_img->width
    movl IMAGE_HEIGHT_OFFSET(%r14), %eax # load input_img->height into %eax
    movl %eax, IMAGE_HEIGHT_OFFSET(%r15) # store %eax to output_img->height
    movl IMAGE_WIDTH_OFFSET(%r14), %r12d  # %r12d = input_img->width
    movl IMAGE_HEIGHT_OFFSET(%r14), %r13d # %r13d = input_img->height

    # size = width * height * sizeof(uint32_t)
    imull %r13d, %r12d # %r12d = width * height
    shll $2, %r12d # %r12d = size in bytes * 4

    # allocate memory
    movl %r12d, %edi # %edi = size
    call malloc # malloc(size)
    movq %rax, IMAGE_DATA_OFFSET(%r15) # store the memory to output_img->data

    movl IMAGE_WIDTH_OFFSET(%r14), %r10d # %r10d = input_img->width
    imull IMAGE_HEIGHT_OFFSET(%r14), %r10d # %r10d = total pixels
    movl $0, %eax # %eax = 0 to use as loop counter

    movq IMAGE_DATA_OFFSET(%r14), %rdx # %rdx = input_img->data
    movq IMAGE_DATA_OFFSET(%r15), %rcx # %rcx = output_img->data

# Loop over each pixel
grayscale_loop:
    cmpl %r10d, %eax # compare i with total pixels
    jge grayscale_loop # if i is greater than or equal to total pixels, exit the loop

    # get the addresses 
    leaq (%rdx, %rax, 4), %r8 # %r8 = &input_img->data[i]
    leaq (%rcx, %rax, 4), %r9 # %r9 = &output_img->data[i]

    movl (%r8), %r11d # %r11d = pixel
    movl %r11d, %r12d # copy to %r12d
    andl $0xFF, %r12d # %r12d = alpha
    movl %r11d, %r13d # copy to %r13d
    shrl $8, %r13d # shift to the right by 8 bits
    andl $0xFF, %r13d # %r13d = blue
    movl %r11d, %r14d # copy to %r14d
    shrl $16, %r14d # shift to the right by 16 bits
    andl $0xFF, %r14d # %r14d = green
    movl %r11d, %r15d # copy to %r15d
    shrl $24, %r15d # shift to the right by 24 bits
    andl $0xFF, %r15d # %r15d = red

    # grayscale = (79*red + 128*green + 49*blue) / 256
    movl %r15d, %ebx # %ebx = red
    imull $79, %ebx # %ebx = 79 * red
    movl %r14d, %edi # %edi = green
    imull $128, %edi # %edi = 128 * green
    addl %edi, %ebx # %ebx += 128 * green
    movl %r13d, %edi # %edi = blue
    imull $49, %edi # %edi = 49 * blue
    addl %edi, %ebx # %ebx += 49 * blue
    shrl $8, %ebx # %ebx = gray = numerator / 256

    # create the new pixel with grayscale and alpha values
    movl %ebx, %edi # %edi = gray
    shll $24, %edi # shift the gray to red's position 
    movl %ebx, %r10d # %r10d = gray
    shll $16, %r10d # shift the gray to green's position
    movl %ebx, %r9d # %r9d = gray
    shll $8, %r9d # shift the gray to blue's position
    orl %r10d, %edi # combine g
    orl %r9d, %edi # combine b
    orl %r12d, %edi # combine alpha 

    movl %edi, (%r9) # output_img->data[i] = new pixel

    incl %eax # increment
    jmp grayscale_loop # Repeat loop

grayscale_done:
    # restore the registers
    popq %r15                
    popq %r14                
    popq %r13                
    popq %r12                
    popq %rbx                
    popq %rbp                # old base ptr
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
	/* TODO: implement (this stub implementation just returns 0) */
	xorl %eax, %eax
	ret

/*
vim:ft=gas:
*/
