/*Programmed by: Saurabh Naresh Netravalkar
                 saurabh.netravalkar@gmail.com

This Program Counts the Number of Comparision Operations
performed by the Quick Sort Algorithm in the following three
cases:
1. Using the First Element as the Pivot
2. Using the Last Element as the Pivot
3. Using the Median of the First, Middle and Last Element as the Pivot*/

//Include Files
#include<stdio.h>
#include<string.h>

//Length of the Array
#define N 10000

//Global Variables
//Stores the Number of Comparisions made
int count;
/*To Identify the case number of the Pivot Selection Method
used by the QuickSort Function*/
int case_number;

//Macro for a Generic Swap Function
#define swap(x,y,T) {T temp=x; x=y; y=temp;}

//Function Prototypes
//The Quick Sort Algorithm
void QuickSort(int *,int);
//The Partition Function of the Quick Sort Algorithm
int partition(int *,int,int);

//Main Function
int main()
{
    /*Open the Input File in Read Mode
    The file contains all of the integers between 1 and 10,000 (inclusive, with no repeats)
    in unsorted order*/
    FILE *f_in=fopen("res\\QuickSort.txt","r");

    //Declare the Array and a Counter to Iterate over its Elements
    int a[N],i;

    //Read the Input Array from File and Close the File
    for(i=0;i<N;i++)
        fscanf(f_in,"%d",&a[i]);
    fclose(f_in);

    printf("Total Number of Comparisions:\n");
    //Repeat for all the above defined Three Cases of Pivot Selection
    for(case_number=1;case_number<=3;case_number++)
    {
        //Create a Duplicate of 'a'
        int duplicate[N];
        //Copy the contents of 'a' into 'duplicate'
        memcpy(duplicate,a,N*sizeof(int));

        /*Since all 'N-1' elements get compared exactly once with the pivot element in the
        QuickSort Function, Store 'N-1' in 'count' and Call the Quicksort Function*/
        count = N-1;
        QuickSort(duplicate,N);

        //Print the Total Number of Comparisions Made by Quick Sort
        printf("Case %d: %d\n",case_number,count);
    }

    return 0;
}

/*The Quick Sort Algorithm
Parameters: a->Array to be sorted, n-> Length of 'a'*/
void QuickSort(int *a,int n)
{
    if(n==1)
        return;
    else
    {
        /*Partition the Array around the Pivot Element 'p' returned by the Partition Function
        and call QuickSort on both its Left as well as Right Sub-Array*/
        int p=partition(a,0,n-1),i;
        if(p>0)
        {
            count+=p-1;
            QuickSort(a,p);
        }
        if(n-p-1>0)
        {
            count+=n-p-2;
            QuickSort(a+p+1,n-p-1);
        }
    }
}

/*The Partition Function works on a sub-array of 'a' from index 'l' to ''r'
It selects a pivot element and places it in such a way that all elements less than or equal to it,
lie to its left and all elements greater than it lie to its right
Parameters: a->Array under consideration, l->lower index of the array, r->upper index of the array
Returns the position of the pivot element in the array*/
int partition(int *a,int l,int r)
{
    int p,i=l+1,j,m,median;

    switch(case_number)
    {
        case 1:
            //The First Element is the Pivot
            break;
        case 2:
            //Select the Final Element as Pivot and Place it at the First Position
            swap(a[l],a[r],int);
            break;
        case 3:
            //Select the Median of the First, Middle and Last element as the Pivot
            m=l+(r-l)/2;
            if(a[l]>a[m])
            {
                if(a[m]>a[r])
                    median=m;
                else if(a[l]>a[r])
                    median=r;
                else
                    median=l;
            } else
            {
                if(a[l]>a[r])
                    median=l;
                else if(a[m]>a[r])
                    median=r;
                else
                    median=m;
            }
            //Place the Pivot at the First Position
            swap(a[l],a[median],int);
            break;
    }

    //Assign the Pivot Element to 'p'
    p=a[l];

    //Place every element less than 'p' to the left of it
    for(j=l+1;j<=r;j++)
    {
        if(a[j]<p)
        {
            swap(a[j],a[i],int);
            i++;
        }
    }
    swap(a[l],a[i-1],int);

    //Return the Position of the Pivot
    return i-1;
}
