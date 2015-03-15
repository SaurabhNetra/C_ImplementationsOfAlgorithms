/*Programmed by: Saurabh Naresh Netravalkar
                 saurabh.netravalkar@gmail.com

This program demonstates the knapsack algorithm.

The input file describes a knapsack instance, and it has the following format:

[knapsack_size][number_of_items]
[value_1] [weight_1]
[value_2] [weight_2]
...
For example, the third line of the file is "50074 834558", indicating that the second
item has value 50074 and size 834558, respectively.
All numbers are positive. Item weights and the knapsack capacity are integers.

The Challenge:

This instance is so big that the straightforward iterative implemetation uses an infeasible amount
of time and space. One idea is to go back to a recursive implementation, solving subproblems --- and,
of course, caching the results to avoid redundant work --- only on an "as needed" basis.

My Solution:

I Used a 2-D Hash Table with Chaining for Resolving Collisions, to Cache Results for O(1) Look Up
*/

//Include Files
#include<stdlib.h>
#include<stdio.h>

/*A 2-D Hash Table Data Structure
e.g. A[i][h(W)] gives the list of solutions to subproblems considering the first 'i' items and
Knapsack capacities which hash to h(W). From this list we can lookup the precisely required solution with 'i' items
and 'W' knapsack capacity
*/
struct Node
{
    int W,V;
    struct Node *next;
}***A;

//Simpe Modulo Hash Function, the number 10000 is chosen by studying the distribution of the data in the input data set
#define h(x) (x % 10000)

//A function to insert into the 2-D hash table
void cache_insert(int i,int W,int V)
{
    struct Node *pnew=malloc(sizeof(struct Node));
    pnew->V=V;
    pnew->W=W;
    pnew->next=A[i][h(W)];
    A[i][h(W)]=pnew;
}

//Look-Up Fucntion for the Hash Table
#define NOT_IN_HASH_TABLE 0
int cache_lookup(int i,int W)
{
    //Look for the Node with Specific Weight 'W' in the chained list of solutions
    struct Node *itrtr=A[i][h(W)];
    while(itrtr)
    {
        if(itrtr->W==W)
            return itrtr->V;
        itrtr=itrtr->next;
    }

    return NOT_IN_HASH_TABLE;
}

//Data Structure for an Item
struct item
{
    int v,w;
};

/*The Knapsack Algorithm
Parameters: 'item' -> A list of items, 'i' -> the number of items from left to consider while solving the problem
'W' -> The Knapsack Capacity
*/
int knapsack(struct item *item,int i,int W)
{
    if(i==-1)
        return 0;
    else
    {
        /*Knapsack(items,i,x) is the Maximum of
            1. Knapsack(items,i-1,x) considering ith item to not be part of the optimal solution
            2. Knapsack(i-1,x-item[i-1].w)+item[i-1].v considering ith item to be part of the optimal solution,
               where its value is added to the solution of 'i-1' items having knapsack capacity of 'x - ith item's weight'*/
        int value1=0,value2=0,temp;

        //Case 1.
        if(i>0)
        {
            //Use the Cached Solution if the required Subproblem is solved
            temp=cache_lookup(i-1,W);
            if(temp!=0)
                value1=temp;
            //If the required Subproblem is not solved, solve it and cache it
            else
                value1=knapsack(item,i-1,W);
        }
        else
            value1=knapsack(item,i-1,W);

        //Case 2.
        if(item[i].w<=W)
        {
            if(i>0)
            {
                //Use the Cached Solution if the required Subproblem is solved
                temp=cache_lookup(i-1,W-item[i].w);
                if(temp!=0)
                    value2=temp+item[i].v;
                //If the required Subproblem is not solved, solve it and cache it
                else
                    value2=knapsack(item,i-1,W-item[i].w)+item[i].v;
            }
            else
                value2=knapsack(item,i-1,W-item[i].w)+item[i].v;
        }
        //Store the Maximum of the 2 Solved Cases
        if(value2>value1)
            value1=value2;

        //Cache the Newly Solved Problem's Solution
        cache_insert(i,W,value1);

        return value1;
    }
}

//Main Function
int main()
{
    //Open the Input File in Read Mode
    FILE *fin=fopen("InputFiles\\knapsack2.txt","r");

    //W->Knapsack Capacity, n->Number of Items in Consideration
    int W,n;

    //The array of items
    struct item *item;

    int i;

    fscanf(fin,"%d%d",&W,&n);

    item=(struct item *)malloc(n*sizeof(struct item));

    //Initialze memory for various parameters
    A=(struct Node ***)malloc(n*sizeof(struct Node **));
    for(i=0;i<n;i++)
        A[i]=(struct Node **)calloc(10000,sizeof(struct Node *));
    for(i=0;i<n;i++)
        fscanf(fin,"%d%d",&item[i].v,&item[i].w);

    //Call the Knapsack Function and print the Solution
    printf("Optimal Knapsack Solution: %d\n",knapsack(item,n-1,W));

    return 0;
}

