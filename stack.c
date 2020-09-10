/****************************************************************************

Matthew Roth, A15685519
CSE 12, Winter 2019
January 22, 2019
cs12xcm
Assignment Three

File Name:      stack.c
Description:    The stack will emulate a "Stack" data structure. The program 
will implement an array-based stack of longs and modify the driver.c file 
which allows for the testing of the stack. The array-based stack will be 
dynamically allocated from the heap with an offset of -3 to -1 structure
and start the index for the user at 0, with -3 to -1 hidden data structure.

****************************************************************************/

#include <malloc.h>
#include <stdio.h>
#include "mylib.h"
#include "stack.h"

#define STACK_POINTER_INDEX (-1)        /* Index of next available space */
#define STACK_SIZE_INDEX (-2)           /* Index of size of the stack */
#define STACK_COUNT_INDEX (-3)          /* Index of which stack allocated */
#define STACK_OFFSET 3  /* offset from allocation to where user info begins */

/* define the keyword "NULL" as 0 */
#ifdef NULL
#undef NULL
#endif
#define NULL 0


/* catastrophic error messages */
static const char DELETE_NONEXIST[] = "Deleting a non-existent stack!!!\n";
static const char EMPTY_NONEXIST[] = "Emptying a non-existent stack!!!\n";
static const char INCOMING_NONEXIST[] = 
"Incoming parameter does not exist!!!\n";
static const char ISEMPTY_NONEXIST[] = 
"Is empty check from a non-existent stack!!!\n";
static const char ISFULL_NONEXIST[] = 
"Isfull check from a non-existent stack!!!\n";
static const char NUM_NONEXIST[] = 
"get_occupancy check from a non-existent stack!!!\n";
static const char POP_NONEXIST[] = "Popping from a non-existent stack!!!\n";
static const char POP_EMPTY[] = "Popping from an empty stack!!!\n"; 
static const char PUSH_NONEXIST[] = "Pushing to a non-existent stack!!!\n";
static const char PUSH_FULL[] = "Pushing to a full stack!!!\n";
static const char TOP_NONEXIST[] = "Topping from a non-existent stack!!!\n";
static const char TOP_EMPTY[] = "Topping from an empty stack!!!\n";
static const char WRITE_NONEXIST_FILE[] = 
"Attempt to write using non-existent file pointer!!!\n";
static const char WRITE_NONEXIST_STACK[] = 
"Attempt to write to a non-existent stack!!!\n";

/* Debug messages */
static const char ALLOCATED[] = "[Stack %ld has been allocated]\n";
static const char DEALLOCATE[] = "[Stack %ld has been deallocated]\n";
static const char HEXPOP[] = "[Stack %ld - Popping 0x%lx]\n";
static const char HEXPUSH[] = "[Stack %ld - Pushing 0x%lx]\n";
static const char HEXTOP[] = "[Stack %ld - Topping 0x%lx]\n";
static const char POP[] = "[Stack %ld - Popping %ld]\n";
static const char PUSH[] = "[Stack %ld - Pushing %ld]\n";
static const char TOP[] = "[Stack %ld - Topping %ld]\n";

/* static variable allocation */
static int debug = FALSE; /* allocation of debug flag */
static int stack_counter = 0; /* number of stacks allocated so far */

/* Debug state methods */
void debug_off (void) {
	debug = FALSE;
}

void debug_on (void) {
	debug = TRUE;
}

/*--------------------------------------------------------------------------
Function Name:         delete_Stack
Purpose:               deallocate (free) the memory for the Main Stack 
Description:           make the call to free() and set the paramter pointer 
back to a Null reference
Input:                 spp: the pointer to a pointer of the main stack   
Result:                The stack is deallocated 
Return:				   void.
--------------------------------------------------------------------------*/
void delete_Stack (Stack ** spp) {

	/*check for null pointer*/
	if(spp == NULL){
		fprintf(stderr, "Stack is Null pointer!");
		return;
	}
	/*check for null pointer*/
	if(*spp == NULL){
		fprintf(stderr, "Pointer to Stack is Null!");
	}

	/*deallocate the memory*/
	free(*spp-STACK_OFFSET);

	/*set the pointer of the Stack to null*/
	*spp = NULL;

	/*in debug mode print the current action of delete_Stack*/
	if(debug){
		fprintf(stderr, DEALLOCATE, (long)stack_counter);
	}

	/*decrement the satck counter*/
	--stack_counter;

}

/*--------------------------------------------------------------------------
Function Name:         empty_Stack
Purpose:               Empty the stack contents (NOT deallocate)
Description:           set the stack pointer index to contain 0  
Input:                 this_Stack: the stack pointer  
Result:                The stack is empty 
Return:				   void. 
--------------------------------------------------------------------------*/
void empty_Stack (Stack * this_Stack) {

	/*check if the stack exists*/
	if(this_Stack == NULL){

		fprintf(stderr, EMPTY_NONEXIST);
		return;
	}

	/*set the curret element to item*/
	this_Stack[STACK_POINTER_INDEX] = NULL;

}

/*--------------------------------------------------------------------------
Function Name:         isempty_Stack
Purpose:               Check if the stack is empty
Description:           if the stack pointer index contains 0 as a value then
the satck is empty, so return TRUE (1) else return false (0).
Input:                 this_Stack: the stack pointer  
Result:                The stack is empty or its not empty 
Return:				   TRUE/FALSE.
--------------------------------------------------------------------------*/
long isempty_Stack (Stack * this_Stack) {

	/*check the stack exists*/
	if(this_Stack == NULL){

		fprintf(stderr, ISEMPTY_NONEXIST);
		return FALSE;
	}

	/*if the stack is empty return 1*/
	if( this_Stack[STACK_POINTER_INDEX] == NULL ){
		return TRUE;
	}else {
		return FALSE;
	}

	/*debug statement*/
	if(debug == TRUE && this_Stack[STACK_COUNT_INDEX] == NULL){

		/*if the stack doesnt exist*/
		fprintf(stderr, ISEMPTY_NONEXIST);
	}

	return TRUE;
}

/*--------------------------------------------------------------------------
Function Name:         isfull_Stack
Purpose:               Check if the stack is full
Description:           Check if the stack size index is equal to the stack
pointer index and return true or false based on validity.
Input:                 this_Stack: the stack pointer  
Result:                The stack is full or its not full 
Return:				   TRUE/FALSE.
--------------------------------------------------------------------------*/
long isfull_Stack (Stack * this_Stack) {

	long status;	/*return the status */

	/*check the stack exists*/	
	if(this_Stack == NULL){
		fprintf(stderr, ISFULL_NONEXIST);
		return 0;
	}

	/*if the stack is full return 1*/
	if(this_Stack[STACK_SIZE_INDEX] == this_Stack[STACK_POINTER_INDEX]){

		status = 1;

	}else{

		status = 0;
	}

	/*debug statement*/
	if(debug == TRUE && this_Stack[STACK_COUNT_INDEX] == NULL){

		/*if the stack doesnt exist*/
		fprintf(stderr, ISFULL_NONEXIST);
	}

	return status;
}

/*--------------------------------------------------------------------------
Function Name:         new_Stack
Purpose:               Constructor to initialize the data structure and create
a new stack.
Description:           Start by allocated memory the size that will be indicated
by the parameter plus the offset. Then create the Stack object, and initialize 
all the cooresponding elements to their starting values. 
Input:                 stacksize: the size of the stack to make (amount of 
elements)
Result:                The new stack is created  
Return:				   this_Stack: the Stack 
--------------------------------------------------------------------------*/
Stack * new_Stack (unsigned long stacksize) {

	/*allocate memory specified size to be the parameter stacksize*/
	void *memory = malloc(sizeof(long)*(stacksize + STACK_OFFSET));

	/*create a Stack using the allocated memory*/
	Stack * this_Stack = (Stack *)memory+STACK_OFFSET;

	/*initialize the first pointer index to zero */
	this_Stack[STACK_POINTER_INDEX] = 0;

	/*set the stack size from the parameter*/
	this_Stack[STACK_SIZE_INDEX] = stacksize;

	/*set the amount of Stack data structures*/
	this_Stack[STACK_COUNT_INDEX] = ++stack_counter;

	/*print the amount that has been allocated in debug mode*/
	if(debug == TRUE){

		fprintf(stderr, ALLOCATED, (long)stack_counter);
	}

	return this_Stack;
}

/*--------------------------------------------------------------------------
Function Name:         get_occupancy
Purpose:               Get the amount of elements in the stack 
Description:           perform error and validity checks and then return the
data at the stack pointer index. 
Input:                 this_Stack: the stack pointer  
Result:                The amount of elements in the stack is returned  
Return:				   this_Stack[STACK_POINTER_INDEX]: the amount of elements
--------------------------------------------------------------------------*/
long get_occupancy (Stack * this_Stack) {

	/*check if the stack exists*/
	if(this_Stack == NULL){

		fprintf(stderr, NUM_NONEXIST);
		return FALSE;
	}

	return this_Stack[STACK_POINTER_INDEX];
}

/*--------------------------------------------------------------------------
Function Name:         pop
Purpose:               pop the last item on the stack, remove the last element
Description:           perform validity and error checks, then set the pointer
from the incoming parameter to the stack pointer index, and decrement the 
pointer to shift down an element
Input:                 this_Stack: the stack pointer *item: the new pointer   
Result:                the last element is removed from the stack   
Return:			       TRUE/FALSE					  
--------------------------------------------------------------------------*/
long pop (Stack * this_Stack, long * item) {

	int sp;

	/*check for null pointer*/
	if(this_Stack == NULL){
		fprintf(stderr,POP_NONEXIST);
		return FALSE;
	}
	/*check for null pointer ref*/
	if(item == NULL){
		fprintf(stderr, INCOMING_NONEXIST);
		return FALSE;
	}
	/*check if the stack is empty*/
	if(isempty_Stack(this_Stack)){
		fprintf(stderr, POP_EMPTY);
		return FALSE;
	}
	/*debug mode action on POP*/
	if(debug){
		fprintf(stderr, POP, this_Stack[STACK_COUNT_INDEX], 
				this_Stack[STACK_POINTER_INDEX]);
	}

	/*decrement the stack pointer index*/
	--this_Stack[STACK_POINTER_INDEX];

	 sp = this_Stack[STACK_POINTER_INDEX];

	*item = this_Stack[sp];


	return TRUE;
}

/*--------------------------------------------------------------------------
Function Name:         push
Purpose:               push a new item onto the stack 
Description:           Set the current element in the stack to hold the 
incoming parameter as a long, and shift the pointer index to point at the 
next available element in the stack, return TRUE/FALSE based on success
Input:                 this_Stack: the stack pointer item: the number to 
push onto the stack 
Result:                the number is added to the next available element 
in the stack 
Return:			       TRUE/FALSE					  
--------------------------------------------------------------------------*/
long push (Stack * this_Stack, long item) {

	long i = this_Stack[STACK_POINTER_INDEX];	/*index of the Stack pointer*/

	/*check if the stack exists*/
	if(this_Stack == NULL){

		fprintf(stderr, PUSH_NONEXIST);
		return FALSE;
	}

	/*size of the last Stack prior to push() call*/

	if(isfull_Stack(this_Stack) == TRUE){
		fprintf(stderr, PUSH_FULL);
		return FALSE;
	}

	/*set the curret element to item*/
	this_Stack[i] = item;

	/*increment the stack index in pointer container*/
	++this_Stack[STACK_POINTER_INDEX]; 

	/*in debug mode print current action and success or fail*/
	if(debug == TRUE){

		fprintf(stderr, PUSH, this_Stack[STACK_COUNT_INDEX],
				this_Stack[STACK_POINTER_INDEX]);


		if(this_Stack[i] != item){
			fprintf(stderr, "The previous push failed!"); 
			return FALSE;
		}
		else{
			fprintf(stderr, "Stack push success!");
			return TRUE;
		}
	}

	return TRUE;
}

/*--------------------------------------------------------------------------
Function Name:         top
Purpose:               top will return the last popped element on the stack 
Description:           perform the basic validity checks and error checks 
and then dereference the pointer from parameter to the most recent 
element in the stack 
Input:                 this_Stack: the stack pointer *item: the new pointer   
Result:                the last element is returned from the stack   
Return:			       TRUE/FALSE					  
--------------------------------------------------------------------------*/
long top (Stack * this_Stack, long * item) {

	int sp;

	/*check for null pointer*/
	if(this_Stack == NULL){
		fprintf(stderr,TOP_NONEXIST);
		return FALSE;
	}
	/*check for null pointer ref*/
	if(item == NULL){
		fprintf(stderr, INCOMING_NONEXIST);
		return FALSE;
	}
	/*check if the stack is empty*/
	if(isempty_Stack(this_Stack)){
		fprintf(stderr, TOP_EMPTY);
		return FALSE;
	}
	/*debug mode action on TOP*/
	if(debug){
		fprintf(stderr, TOP, this_Stack[STACK_COUNT_INDEX], 
				this_Stack[STACK_POINTER_INDEX]);
	}

	sp = this_Stack[STACK_POINTER_INDEX] - 1;

	*item = this_Stack[sp];


	return TRUE;
}

/*--------------------------------------------------------------------------
Function Name:         write_Stack 
Purpose:               displays all the elements on the stack 
Description:           Perform the basic error and validity checks and then
print each element on the stack to the stream 
Input:                 this_Stack: the stack pointer *stream: file stream    
Result:                print all the elements in the stack consecutively   
Return:			       stream				  
--------------------------------------------------------------------------*/
FILE * write_Stack (Stack * this_Stack, FILE * stream) {

	long index = 0;         /* index into the stack */

	if (this_Stack == NULL) {
		fprintf (stderr, WRITE_NONEXIST_STACK);
		return stream;
	}

	if (stream == NULL) {
		fprintf (stderr, WRITE_NONEXIST_FILE);
		return stream;
	}

	if (stream == stderr)
		fprintf (stream, "Stack has %ld items in it.\n",
				get_occupancy (this_Stack));

	for (index = STACK_COUNT_INDEX + STACK_OFFSET;
			index < get_occupancy (this_Stack); index++) {

		if (stream == stderr)
			fprintf (stream, "Value on stack is |0x%lx|\n",
					this_Stack[index]);
		else {
			if (this_Stack[index] < 0)
				fprintf (stream, "%c ",
						(char) this_Stack[index]);
			else
				fprintf (stream, "%ld ", this_Stack[index]);
		}
	}

	return stream;
}
