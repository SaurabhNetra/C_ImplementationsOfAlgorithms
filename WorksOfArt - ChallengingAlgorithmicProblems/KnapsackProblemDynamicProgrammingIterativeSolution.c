/*Programmed by: Saurabh Naresh Netravalkar
                 saurabh.netravalkar@gmail.com

This program demonstates the knapsack algorithm which runs in O(nW) time, where
'n' is the number of items and 'W' is the knapsack capacity.

The input file describes a knapsack instance, and it has the following format:

[knapsack_size][number_of_items]
[value_1] [weight_1]
[value_2] [weight_2]
...
For example, the third line of the file is "50074 659", indicating that the second item has value 50074 and size 659,
respectively.
All numbers are positive. Item weights and the knapsack capacity are integers.
*/

//Include Files
#include<stdlib.h>
#include<stdio.h>

//Main Function
int main()
{
    //Open the Input File in Read Mode
    FILE *fin=fopen("InputFiles\\knapsack1.txt","r");

    //W->Knapsack Capacity, n->Number of Items in Consideration
    int W,n;

    //Data Structure to store items
    struct item
    {
        //v->value, w->weight
        int v,w;
    }*item;

    //2-D Array to store solutions to sub-problems, which will be available for O(1) look up later
    int **A;

    int i,x;

    fscanf(fin,"%d%d",&W,&n);

    //Initialze memory for various parameters
    item=(struct item *)malloc(n*sizeof(struct item));

    A=(int **)malloc((n+1)*sizeof(int *));
    for(i=0;i<=n;i++)
        A[i]=(int *)malloc((W+1)*sizeof(int));

    //Read the Items List
    for(i=0;i<n;i++)
        fscanf(fin,"%d%d",&item[i].v,&item[i].w);


//---------Dynamic Programming Algorithm Begins------------------

//Note: A[i][x] represents a solution to a Knapsack with Capacity 'x' and considering only the 1st 'i' items in the list

    //Set A[0][x] to '0', since there are zero items in knapsack for them
    for(x=0;x<=W;x++)
        A[0][x]=0;

    //Iteratively start solving bigger sub-problems using solutions of smaller sub-problems
    for(i=1;i<=n;i++)
    {
        for(x=0;x<=W;x++)
        {
            /*A[i][x] is the Maximum of
            1. A[i-1][x] considering ith item to not be part of the optimal solution
            2. A[i-1][x-item[i-1].w]+item[i-1].v considering ith item to be part of the optimal solution,
               where its value is added to the solution of 'i-1' items having knapsack capacity of 'x - ith item's weight'*/
            A[i][x]=A[i-1][x];
            if(x>=item[i-1].w)
            {
                if(A[i-1][x-item[i-1].w]+item[i-1].v>A[i][x])
                    A[i][x]=A[i-1][x-item[i-1].w]+item[i-1].v;
            }
        }
    }

    //Print the Optimal Total Value of our Knapsack Solution
    printf("Optimal Knapsack Solution: %d\n",A[n][W]);

    return 0;
}
