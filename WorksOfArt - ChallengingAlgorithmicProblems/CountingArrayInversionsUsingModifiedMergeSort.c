/*Programmed by: Saurabh Naresh Netravalkar
                 saurabh.netravalkar@gmail.com

This Program Counts the Number of Inversions in an Array in O(n*log(n)) time
using modifications in the Merge Sort Algorithm

Note: An inversion is a pair of places of a sequence where the elements on these places are out of their natural order.
*/

//Include Files
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

//The Size of the Input Array
#define N 100000

//-------Function Prototypes---------

//A Modification of the Merge Sort Algorithm
unsigned int sort_and_countinv(int *,int);
//A Modification of the Merge Function of the Merge Sort Algorithm
unsigned int merge_and_countsplitinv(int *,int);

//------Main Function--------
int main()
{
    /*Open the Input File in Read Mode
    This file contains all of the 100,000 integers between 1 and 100,000 (inclusive)
    in some order, with no integer repeated*/
    FILE *f_in=fopen("InputFiles\\IntegerArray.txt","r");

    //Declare the Array and a Counter to Iterate over its Elements
    int a[N],i;

    //Read the Input Array from File and Close the File
    for(i=0;i<N;i++)
        fscanf(f_in,"%d",&a[i]);
    fclose(f_in);

    //Call the Modified Merge Sort routine which returns the Number of Inversions of an Array
    printf("Total Number of Inversions: %u\n",sort_and_countinv(a,N));

    return 0;
}

/*A Modification of the Merge Sort Algorithm
Parameters: a->The Array to be sorted, n->Length of 'a'
Returns the Number of Inversions in 'a'*/
unsigned int sort_and_countinv(int *a,int n)
{
    //If Length is 1, then there are no Inversions
    if(n==1)
        return 0;
    else
    {
        unsigned int x,y,z;

        //Sort the Left Half of 'a' and Store the Number of Inversions in it in 'x'
        x=sort_and_countinv(a,n/2);

        //Sort the Right Half of 'a' and Store the Number of Inversions in it in 'y'
        y=sort_and_countinv(a+n/2,n-n/2);

        //Merge the two Halfs, compute the Number of Split Inversions and Store their number in 'z'
        z=merge_and_countsplitinv(a,n);

        //Return the Total Number of Inversions of 'a'
        return x+y+z;
    }
}

/*A Modification of the Merge Function of the Merge Sort Algorithm
which Merges the Sorted Left and Right Halves of an Array
Parameters: 'a'->The Array, 'n'->Length of 'a'
Returns the Number of Split Inversions between the Left and the
Right Half of 'a'*/
unsigned int merge_and_countsplitinv(int *a,int n)
{
    //Pointer to the Beginning of the Left Half of 'a'
    int i=0;

    //Pointer to the Beginning of the Right Half of 'a'
    int j=n/2;

    //A temporary Array to store the result of the Merge operation
    int *temp=(int *)malloc(n*sizeof(int));

    //Pointer to the Beginning of 'temp'
    int k=0;

    //Stores the Number of Inversions
    unsigned int count=0;

    //Repeat until Either the Left Half or the Right Half has Elements
    while(i<n/2 && j<n)
    {
        //Add the Smaller of the two elements into the 'temp' array
        if(a[i]<a[j])
            temp[k++]=a[i++];
        else
        {
            temp[k++]=a[j++];

            /*Since the Chosen Element 'a[j]' is Smaller than all the Remaining Elements
            in the Left Half of 'a', Increment the Number of Inversions by that amount respectively*/
            count+=n/2-i;
        }
    }

    //Add the remaining elements one by one into the 'temp' array
    while(i<n/2)
        temp[k++]=a[i++];
    while(j<n)
        temp[k++]=a[j++];

    //Copy the contents of the merged 'temp' array back into 'a'
    memcpy(a,temp,n*sizeof(int));

    //Free the memory allocated to the temp array
    free(temp);

    //Return the Number of Split Inversions
    return count;
}
