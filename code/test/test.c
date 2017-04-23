/* sort.c 
 *    Test program to sort a large number of integers.
 *
 *    Intention is to stress virtual memory system.
 *
 *    Ideally, we could read the unsorted array off of the file system,
 *	and store the result back to the file system!
 */

#include "syscall.h"
#define N 20
#define M 20

int num[M][N];	/* size of physical memory; with code, we'll run out of space!*/

int
main()
{
	Halt();
	// Exit(0);
    // Exit(A[0]);		/* and then we're done -- should be 0! */
}
