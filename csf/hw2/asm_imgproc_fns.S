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
	pushq %rbp # save base ptr onto the stack 
	movq %rsp, %rbp # set base ptr to the same value as stack ptr

	movq 8(%rdi), %r12 # width of input img into r12 (stored in Image struct with an offset of 8)
	movq 12(%rdi), %r13 # height of input img into r13 (stored in Image struct with an offset of 12)

	movq $0, %r10 # initialize pixel index to 0 so we can iterate over the pixels

grayscale_loop:
	movq %r12, %rcx # copy width to rcx (temp register)
	imulq %r13, %rcx # width, r12 times height, r13 = rcx, total # of pixel 

	cmpq %r10, %rcx # compare current pixel index (r10) with total # of pixels (rcx)
	jge grayscale_done # exit loop if r10 is greater than or equal to rcx

	# load 32bit pixel val from 'input_img->data[r10]'
	movl 16($rdi, %r10, 4), %eax # start of the data array is rdi+16

	movzbl %al, %ebx # move pixel alpha to ebx to preserve it (least significant byte)
	movzbl %ah, %ecx # move pixel blue to ecx (2nd byte)
	shrq $16, %rax # shift rax to the right by 16bits (bring green to lowest byte)
	movzbl %al, %edx # move pixel green into edx (3rd byte)
	shrq $8, %rax # shift rax to the right by 8bits (bring red to lowest byte)
	movzbl %al, %eax # move pixel red into eax (4th byte)

	# for grayscale: y = (79 * r + 128 * g + 49 * b) / 256
	movl $79, %edi # load 79 into edi for red
	imull %eax, %edi # multiply R by 79, store in edi again 
	movl $128, %eax # load 128 into eax for green
	imull %edx, %eax # multiply G by 128, store in eax again
	addl %eax, %edi # current total = (79 * r + 128 * g), store in edi
	movl $49, %eax # load 49 into eax for blue
	imull %ecx, %eax # multiply B by 49, store in eax again
	addl %eax, %edi # current total + (49 * b), store in edi

	shrq %8, %rdi # shift to the right by 8bits to divide by 256, store grayscale in rdi

	movzbl %bl, %eax # move alpha from ebx to eax
	shll $8, %eax # shift alpha to the left by 8bits
	orl %edi, %eax # combine grayscale value into pixel
	shll $8, %eax # shift this 'total' to the left by 8bits
	orl %edi, %eax # copy grayscale value to blue
	shll $8, %eax # shift this 'total' to the left by 8bits
	orl %ebx, %eax # add original alpha value from ebx to eax

	movl %eax, 16(%rsi, %r10, 4) # store 32bit grayscale pixel in 'output_img->data[r10]'

	incq %r10 # increment r10
	jmp grayscale_loop # jump back to the top of the loop for next pixel

grayscale_done:
	popq %rbp # restore old base ptr
	ret # return

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
