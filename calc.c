/****************************************************************************

Matthew Roth, A15685519
CSE 12, Winter 2019
January 29, 2019
cs12xcm
Assignment Four

File Name:      calc.c
Description:    The program will simulate a calculator. The calculator will use
to Stack's to peform evaluations on the expression entered by the user. The 
calculator will perform basic functions such as: addition, subtraction, division
multiplication, and more complex evaluations, exponential and factorial. Upon 
success the resulting expression will be evaluated and diaplyed to the 
standard output along with the pre and post fix of the expression. 
****************************************************************************/
#include <ctype.h>
#include <stdio.h>
#include "calc.h"
#include "mylib.h"
#include "stack.h"

/*definitions*/
#define CHARACTER(word) ((char) ((word) & 0x000000FF))
#define INDEX(word) (((word) & 0x0000FF00) >> 8)
#define PRIORITY(word) ((word) & 0x0000FE00)
#define SIGN_BIT (1L << ((sizeof (long) << 3) - 1))
#define BYTE 8
#define BASE 2
#define TRUE 1
#define FALSE 0
#define LOOP_ONE 1
#define NULL 0

/*the character index array*/
static char operators[] = "()+-*/^ !";

/*function declarations*/
static long add (long, long);
static long divide (long, long);
static long exponent (long, long);
static long fact (long, long);
static long mult (long, long);
static long setupword (int);
static long sub (long, long);

/*array of pointers to functions to perform evaluations on expressions*/
static long (*functions[]) (long, long) =
{ NULL, NULL, add, sub, mult, divide, exponent, NULL, fact };

/*--------------------------------------------------------------------------
Function Name:         eval
Purpose:               evalaute the expression input by the uesr
Description:           using two stacks, empty the parameter stack onto 
the holder return stack, and call specific functions based on the type of
operator and operands in the stack 
Input:                 stack1: the pointer to a pointer of the incoming stack   
Result:                The evaluation is complete, and the final value is 
returned
Return:				   result: the final value. 
--------------------------------------------------------------------------*/
long eval (Stack * stack1) {

	long op1, op2, word;	/*for the operands and operators*/

	long item = 0;	/*placeholder item from the stack*/

	long result = 0;	/*the resulting expression evaluated*/


	/*local stack for operators*/
	Stack * stack2 = new_Stack(CALCSTACKSIZE);

	/*check for null pointer*/
	if(stack1 == NULL){

		fprintf(stderr, "Stack is Null pointer!");
		return EOF;
	}

	/*reverse stack1 onto stack2*/
	while(!isempty_Stack(stack1)){

		pop(stack1, &item);
		push(stack2, item);


	}

	while(!isempty_Stack(stack2)){

		/*pop the first item from stack2*/
		pop(stack2, &item);

		/*once an operator is reached on stack2*/
		if(item<0){

			/*pop the last two values on stack1 to evaluate*/
			word = item;
			pop(stack1, &item);
			op1 = item;

			/*if stack1 is now empty, skip the pop*/
			if(!isempty_Stack(stack1)){
				pop(stack1, &item);
				op2 = item;
			}
			/*call the specific function to evaluate the expression*/
			result = functions[INDEX(word)](op1,op2);

			/*push the result onto stack1*/
			push(stack1, result);
		}

		else {
			/*if an operator has not been reached, push to stack1*/
			push(stack1, item);
		}

	}
	/*pop the final result and return it*/
	pop(stack1, &item);

	/*set the result to item, for consistency*/
	result = item;

	/*deallocate the local stack*/
	delete_Stack(&stack2);

	return result;

}

/*--------------------------------------------------------------------------
Function Name:         intopost
Purpose:               Get an expression from the user, and initialize the 
stack to hold the data for evaluation.
Description:           Using two stacks, push the user input values onto the 
local stack as longs, and store the values as signed or unsigned values based
on their data type (operator vs. operand)
Input:                 stack1: the pointer to a pointer of the incoming stack   
Result:                The stack is intialized with the data used to evaluate
the expression
Return:				   non zero value or EOF
--------------------------------------------------------------------------*/
long intopost (Stack * stack1) {

	long character = 0;	/*the character to store on stack*/

	long item = 0;	/*item from the stack*/

	/*local stack for operators*/
	Stack * stack2 = new_Stack(CALCSTACKSIZE);

	/*check for null pointer*/
	if(stack1 == NULL){

		fprintf(stderr, "Stack is Null pointer!");
		return EOF;
	}

	/*continue until all the chars in the loop are evaluated*/
	while(TRUE){

		/*get the first character*/
		character = fgetc(stdin);

		/*stop after newline is reached*/
		if(character == '\n'){
			break;
		}

		if(character == EOF){

			/*deallocated the local stack*/
			delete_Stack(&stack2);	
			return EOF;
		}

		/*ignore the space chars*/
		if(character == ' '){

			continue;
		}

		/*if this char is a digit*/
		if(character >= '0' && character <= '9'){

			ungetc(character, stdin);
			character = decin();
			push(stack1, character);

		}
		/*push the parenthesis*/
		else if (character == '('){
			push(stack2, character);

		}

		/*pop stack2 until the '(' is reached*/
		else if( character == ')'){

			while(TRUE){

				pop(stack2, &item);

				if(item == '('){

					break;
				}

				/*push the character into stack1*/
				push(stack1, item);

			}

			/*push the operator onto the stack2*/
		}else{

			if(isempty_Stack(stack2)){

				push(stack2, setupword((int)character));

			}
			else{

				character = setupword((int)character);
				/*top stack2 to initialize item*/
				top(stack2, &item);


				while((PRIORITY(item)>PRIORITY(character))) {

					/*pop then push value from stack2 to stack1*/
					pop(stack2, &item);
					push(stack1, item);

					/* check if stack2 is empty */
					if(!isempty_Stack(stack2)){
						break;
					}
					top(stack2, &item);

				}

				/*push the character onto stack2*/ 
				push(stack2, character);					

			}	
		}

	}

	/*empty stack2 onto stack1*/
	while(!isempty_Stack(stack2)){

		/*pop stack2 and push onto stack1 until stack is empty*/
		pop(stack2, &item);
		push(stack1, item);

	}

	/*deallocated the local stack*/
	delete_Stack(&stack2);

	return TRUE;

}

/*--------------------------------------------------------------------------
Function Name:         add
Purpose:               add the two operands 
Description:           Take in the parameters and add them together  
Input:                 augen: the operand addend: the other operand    
Result:                The two operands are summed
Return:				   result: the final sum 
--------------------------------------------------------------------------*/
static long add (long augend, long addend) {
	return augend + addend;

}

/*--------------------------------------------------------------------------
Function Name:         divide
Purpose:               divide the divisor operand by the dividend
Description:           Basic integer division will be performed 
Input:                 divisor: the operand dividend: the other operand    
Result:                The divisor is divided by the dividend 
Return:				   the division
--------------------------------------------------------------------------*/
static long divide (long divisor, long dividend) {
	return dividend / divisor;
}

/*--------------------------------------------------------------------------
Function Name:         exponent
Purpose:               To raise the base to a given power 
Description:           use resursion to raise to exponential 
Input:                 power: the exponent base: the number to raise to power   
Result:                The exponential is performed
Return:				   result: the exponential 
--------------------------------------------------------------------------*/
static long exponent (long power, long base) {
	
	/*if the base is 1 return*/
	if(base == 1){

		return 1;
	}
	/*once the power is less than 1*/
	else if(power < 1){

		return 1;
	}
	/*recursive call to multiply by base*/
	else{

		return base*exponent(power-1, base);	
	}
}
/*--------------------------------------------------------------------------
Function Name:         fact
Purpose:               perform a factorial on the expression
Description:           The function will use recursion to execute a factorial
on the expression 
Input:                 xxx: the number to factorial ignored: ignore paramater   
Result:                the factorial is performed on the number 
Return:				   the number expressed in factorial
--------------------------------------------------------------------------*/
static long fact (long xxx, long ignored) {

	/*if the parameter is less than 2 return base case 1*/
	if(xxx<BASE)
		return TRUE;

	/*rescursuve call to fact with one less each time until base case*/	
	else
		return xxx*fact(xxx-LOOP_ONE,0);

}

/*--------------------------------------------------------------------------
Function Name:         mult
Purpose:               to multiply the factorx by the factory
Description:           multiply factorx by factory and return the product
Input:                 factorx: the operand factory: the other operand   
Result:                the product 
Return:				   the product 
--------------------------------------------------------------------------*/
static long mult (long factorx, long factory) {
	return factorx * factory;
}
/*--------------------------------------------------------------------------
Function Name:         setupword
Purpose:               to determine which operator is incoming as a parameter
and perform bit manipulation to return an encoded word for the operator to be
uniquely stored on the stack.
Description:           determine which character is the incoming parameter and
return the encoded version of the operator 
Input:                 character: the operator    
Result:				   the operator is encoded                 
Return				   the encoded operator 
--------------------------------------------------------------------------*/
static long setupword (int character) {

	int index = 0;	/*track the index of the array*/

	/*check the character matching with operator in the array*/
	while(TRUE){

		if(character != operators[index]){
			index++;
		}
		else
			break;
	}

	/*return the encoded negative value for the character operator*/
	return SIGN_BIT | (index<<BYTE) | character;

}
/*--------------------------------------------------------------------------
Function Name:         sub
Purpose:               perform subtraction on the operands
Description:           subtract parameter subtrahend from minuend 
Input:                 subtrahend: operand to subctract minuend: operand
to sunctract from 
Result:                the subctraction is performed 
Return:				   the subctraction 
--------------------------------------------------------------------------*/
static long sub (long subtrahend, long minuend) {
	return minuend - subtrahend;
}

