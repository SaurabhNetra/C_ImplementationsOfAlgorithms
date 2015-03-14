/*Programmed by: Saurabh Naresh Netravalkar
                 saurabh.netravalkar@gmail.com

The Program implements a variant of the 2-SUM algorithm in O(n) time using Hash Tables

The input file contains 500,000 positive integers (there might be some repetitions)
with the ith row of the file specifying the ith entry of the array.
The task is to compute the number of target values 't' in the interval [2500,4000] (inclusive)
such that there are distinct numbers x,y in the input file that satisfy x + y = t.
*/

//Include Files
#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>

//Size of the Hash Table
#define N 1000000

//Length of the Input Array of numbers
#define S 500000

//Hashtable Data Structure
struct hashTable
{
    int key;
    bool set;
}A[N];

//Simple Modulo Division Hash Function
int h(int x)
{
    return x%N;
}

/*Insert Function for Hash Table
Uses Linear Probing for resolving collisions*/
void insert(int x)
{
    int hx=h(x);

    while(A[hx].set)
        hx++;

    A[hx].key=x;
    A[hx].set=true;
}

//Hash Table Lookup function
bool lookup(int x)
{
    int hx=h(x);

    while(A[hx].key!=x && A[hx].set)
        hx++;

    if(A[hx].key==x && A[hx].set)
        return true;
    else
        return false;
}

//Main Function
int main()
{
    //Open the Input File in Read Mode
    FILE *fin=fopen("InputFiles\\HashInt.txt","r");

    //Extract the array of numbers and Insert them all into the Hash Table
    int i,x,t,count=0;
    for(i=0;i<S;i++)
    {
        fscanf(fin,"%d",&x);
        insert(x);
    }
    fclose(fin);

    /*For every required 't' value, test whether there exist 2 numbers in
    the hash table whose sum is 't' and count all such occurences*/
    for(t=2500;t<=4000;t++)
    {
        //Repeat for all positions in the Hash table
        for(i=0;i<N;i++)
        {
            /*If there exists a  number at the ith location of the hash table,
            check if there also exists a number 't-A[i]' by a hash table lookup
            and if so increment the 'count' by 1*/
            if(A[i].set && t!=(A[i].key<<1) && t-A[i].key>=0)
            {
                if(lookup(t-A[i].key))
                {
                    count++;
                    break;
                }
            }
        }
    }

    //Print the Solution
    printf("Number of target values: %d\n",count);
    return 0;
}
