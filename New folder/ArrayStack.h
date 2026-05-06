//	This is an updated version of code originally
//  created by Frank M. Carrano and Timothy M. Henry.
//  Copyright (c) 2017 Pearson Education, Hoboken, New Jersey.

/** ADT stack: Array-based implementation.
 @file ArrayStack.h */

#ifndef ARRAY_STACK_
#define ARRAY_STACK_

#include "StackADT.h"

//Unless spesificed by the stack user, the default size is 100
template<typename T>
class ArrayStack : public StackADT<T>
{
private:
	enum { MAX_SIZE = 100 };

void printRec(int index) const {
	if (index < 0) return; // Base Case

	cout << OrderIDOnly(items[index]);

	if (index > 0) {
		cout << ", ";
		printRec(index - 1); // Recursive Step: Move to the next item down
	}
}
protected:
	T items[MAX_SIZE];		// Array of stack items
	int top;                   // Index to top of stack
	int count = 0; // Additional data member to keep track of the number of items in the queue (Initialized to 0)
	
public:

	ArrayStack()
	{
		top = -1;
	}  // end default constructor

	bool isEmpty() const
	{
		return top == -1;	
	}  // end isEmpty

	bool push(const T& newEntry)
	{
		if( top == MAX_SIZE-1 ) return false;	//Stack is FULL

		top++;
		items[top] = newEntry;  
		count++; // Increment the count of items in the stack
		return true;
	}  // end push

	bool pop(T& TopEntry)
	{
		if (isEmpty()) return false;
		
		TopEntry = items[top];		 
		top--;
		count--; // Decrement the count of items in the stack
		return true;
	}  // end pop
	
	bool peek(T& TopEntry) const
	{
		if (isEmpty()) return false;
		
		TopEntry = items[top];		 
		return true;
	}  // end peek

	int getCount() const {
		return count;
	} // end getCount

	void printStack() const {
		printRec(top);
	}

	

}; // end ArrayStack

#endif
