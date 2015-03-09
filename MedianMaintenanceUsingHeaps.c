/*Programmed by: Saurabh Naresh Netravalkar
                 saurabh.netravalkar@gmail.com

This program implements the "Median Maintenance" algorithm in O(n*log(n)) time using two Heaps

The input file contains a list of the integers from 1 to 10000
in unsorted order, which is to be treated as a stream of numbers,
arriving one by one. Letting xi denote the ith number of the file,
the kth median mk is defined as the median of the numbers x1,…,xk.
(So, if k is odd, then mk is ((k+1)/2)th smallest number among x1,…,xk;
if k is even, then mk is the (k/2)th smallest number among x1,…,xk.)

The task is to compute the sum of these 10000 medians, modulo 10000 (i.e., only the last 4 digits).
That is, to compute (m1+m2+m3+...+m10000)mod10000.*/

//Include Files
#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>

//Length of the input Array of numbers
#define N 10000

//A Generic Swap Function
#define swap(X,Y,T) {T t=X; X=Y; Y=t;}

/*Heap Data Structure
also defines 2 heaps of size n/2 each
1st Heap is a Max Heap and 2nd Heap is a Min Heap */
struct HeapNode
{
    int key;
}Heap[2][N/2];
int heap_len[2];

void insert(struct HeapNode heap_node, bool heap_id)
{
    //Append the new Node to the End of the Heap
    int i=heap_len[heap_id];
    Heap[heap_id][heap_len[heap_id]++]=heap_node;

    /*Keep Swapping the element with its parent till the Heap Property is Satisfied
    Note: The swap condition changes with 'heap_id' since 1st Heap is a Max Heap while 2nd one is a Min Heap*/
    int parent=(i-1)/2;
    while(parent>=0 && ((heap_id && Heap[heap_id][i].key<Heap[heap_id][parent].key) || (!heap_id && Heap[heap_id][i].key>Heap[heap_id][parent].key)))
    {
        swap(Heap[heap_id][i],Heap[heap_id][parent],struct HeapNode);
        i=parent;
        parent=(i-1)/2;
    }
}

/*Extracts
1. The Maximum element of 1st Heap when 'heap_id = 0'
2. The minimum element of the 2nd Heap when 'heap_id = 1'*/
struct HeapNode extract(bool heap_id)
{
    //Swap the 1st Element with the last element in the Heap and Remove it
    heap_len[heap_id]--;
    swap(Heap[heap_id][0],Heap[heap_id][heap_len[heap_id]],struct HeapNode);

    //Swap the Newly placed element at the 1st position with its Children till the Heap Property is Satisfied
    int i=0;
    while(1)
    {
        int left_child=2*i+1,right_child=left_child+1,next_node;

        if(left_child>=heap_len[heap_id])
            break;
        else if(right_child>=heap_len[heap_id])
            next_node=left_child;
        else
        {
            if((heap_id && Heap[heap_id][left_child].key<Heap[heap_id][right_child].key) || (!heap_id && Heap[heap_id][left_child].key>Heap[heap_id][right_child].key))
                next_node=left_child;
            else
                next_node=right_child;
        }
        if((heap_id && Heap[heap_id][next_node].key>=Heap[heap_id][i].key) || (!heap_id && Heap[heap_id][next_node].key<=Heap[heap_id][i].key))
            break;

        swap(Heap[heap_id][next_node],Heap[heap_id][i],struct HeapNode);

        i=next_node;
    }

    //Return the Element which was Removed from the Heap
    return Heap[heap_id][heap_len[heap_id]];
}

//Look at 1st Element of Heap(Minimum element for Heap 1 and Maximum element for Heap2 respectively
#define peek(heap_id) Heap[heap_id][0]

//To check if a Heap is empty
#define isEmpty(heap_id) heap_len[heap_id]==0

//Main Function
int main()
{
    //Open the input File in Read Mode
    FILE *fin=fopen("Median.txt","r");

    //Initialize the Computed Sum of Medians
    int m=0;

    /*Loop to extract integers from the input file one at a time and to successively compute medians
    of elements processed upto the last read number
    Note:
    Loop Invariant: Both heaps contain half of the elements processed
    (In case of an odd number of elements, the 1st heap contains 1 extra element)
    The numbers in 1st heap are all less than numbers in the 2nd heap
    Thus the Median is always the 1st element(Maximum element) of the 1st Heap*/
    int i,n;
    for(i=0;i<N;i++)
    {
        fscanf(fin,"%d",&n);

        struct HeapNode heap_node={n};

        //Insert the element according to its value into the appropriate heap
        if(isEmpty(0) || n<=peek(0).key)
            insert(heap_node,0);
        else
            insert(heap_node,1);

        //Balance the two heaps to have the same number of elements
        if(heap_len[0]-heap_len[1]>1)
        {
            heap_node=extract(0);
            insert(heap_node,1);
        }
        else if(heap_len[1]-heap_len[0]==1)
        {
            heap_node=extract(1);
            insert(heap_node,0);
        }

        //Peek at the median, which is the topmost element of the 1st heap and add it to the computed sum
        m=(m+peek(0).key)%N;
    }

    //Print the resultant sum
    printf("The computed sum is %d\n",m);

    return 0;
}
