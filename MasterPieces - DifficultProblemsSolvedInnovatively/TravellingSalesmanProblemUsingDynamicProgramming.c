/*Programmed by: Saurabh Naresh Netravalkar
                 saurabh.netravalkar@gmail.com


This program implements an SPACE-TIME OPTIMIZED DYNAMIC PROGRAMMING algorithm for the TRAVELLING SALESMAN PROBLEM, which
runs in O( n^2 * 2^n) time, which is blazingly faster than the Brute Force Method which runs in O(n!) time.

The Input data file describes a TSP instance. The first line indicates the number of cities. Each city is a point in the plane,
and each subsequent line indicates the x- and y-coordinates of a single city.
The distance between two cities is defined as the Euclidean distance --- that is, two cities at locations (x,y) and (z,w) have
distance √((x−z)^2+(y−w)^2) between them.

The task is to find the minimum cost of a traveling salesman tour for this instance, rounded down to the nearest integer.

WARNING: Higher-level languages (like Scala or Python) might find the size of this data set challenging.

The Solution:

Optimal Substructure:

Consider L(i,j) = Length of a Shortest Path from Source City '1' to 'j' with Exactly 'i' edges and NO REPEATED VERTICES
Then L(i,j) = for k!=1,j: Miniumum of (L(i-1,k) + C(k,j)), where C(k,j) is the Cost of Final Hop.
*/

//Include Files
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<limits.h>
#include<float.h>
#include<stdbool.h>

//Euclidean Distance Function
#define C(x1,y1,x2,y2) sqrt( pow( x2 - x1, 2 ) + pow( y2 - y1, 2 ) )

//n->Number of Cities
int n;

//An Array of Cities. Each city has an 'x' and 'y' co-ordinate.
struct City
{
    double x,y;
}*city;

//Function to Display the Cities and their Co-ordinates
void display_cities()
{
    printf("Cities:\n");
    int i;
    for(i=0;i<n;i++)
        printf("City %2d: %lf %lf\n",i+1,city[i].x,city[i].y);
}

/*An Array of Linked Lists named 'set_of_length' which stores all Subsets of the set of 'n' cities

Note: Each Individual Subset is stored as an 'n' bit integer,
where the cities included in the subset have their corresponding bits set to '1'.
e.g. If n=5 cities exist, then the subset containing cities 2,4 and 5 is given by '11010' in binary i.e. '26' as an integer

The row 'set_of_length[i]' contains all subsets having length 'i', i.e. having 'i' cities selected of of 'n'.
*/
struct Subset
{
    long value;

    struct Subset *next;
}**set_of_length;
//An Array to Store the Lengths of Every Linked List
int *length_of_set_of_length;

/*Insert function into 'set_of_length'
Parameters: index->the size of the subset to be inserted, value->the subset to be inserted
*/
void insert_into_set_of_length(int index,long value)
{
    struct Subset *subset=malloc(sizeof(struct Subset));
    subset->value=value;
    subset->next=set_of_length[index];
    set_of_length[index]=subset;

    length_of_set_of_length[index]++;
}

//Function to Display all the Subsets: Used for Debugging
void display_set_of_length()
{
    printf("Power Set of Length %d:\n",n);

    int i;
    for(i=1;i<=n;i++)
    {
        printf("Subsets of Size %d\n",i);
        struct Subset *subset;
        for(subset=set_of_length[i-1];subset!=NULL;subset=subset->next)
            printf("%ld ",subset->value);
        printf("\n");
    }
}

//Prototype for an Insert Function Into our Hash Table which is Defined Later
void insert_into_hashtable(int,int);

//Insert all our Subsets into HashTable to make them available for O(1) look up later
void insert_set_of_length_into_hashtable()
{
    int i;
    for(i=0;i<n;i++)
    {
        struct Subset *subset;
        int index;
        for(index=0,subset=set_of_length[i];subset!=NULL;index++,subset=subset->next)
            insert_into_hashtable(subset->value,index);
    }
}

/*
This function counts the number of 1s in the bit representation of an integer in almost O(1) time!!
Reference: http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel
*/
int NumberOfSetBits(int i)
{
     i = i - ((i >> 1) & 0x55555555);
     i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
     return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}

/*A Function to Calculate the Power Set i.e. all Subsets of our set of 'n' cities
Note: Since we require the source city 'city 1' to be part of every subset in our Algorithm,
We compute all subsets of the other 'n-1' cities and append our source city to each subset
*/
void calculate_power_set()
{
    int i;

    //Power Set of 'n-1' cities has size 2^(n-1)
    int power_set_size=pow(2,n-1);

    //Each integer from '0' to '2^(n-1)-1' represents a unique subset
    for(i=0;i<power_set_size;i++)
    {
        /*NumberofSetBits gives the size of the computed Subset
        We append our source vertex '1' to the computed subset and insert it into our 'set_of_length' array*/
        insert_into_set_of_length(NumberOfSetBits(i),i*2+1);
    }
}

//----------Dynamic Programming Algorithm Parameters---------
/*
According to Algorithm,
'A' is a 2-D array, indexed by
1. Subset belonging to {1,2,...,n} which contain the source city '1'
2. Destinations j = {1,2,...,n}
A[s][j] indicates the shortest path discovered so far from '1' to 'j' having visited all the cities in subset 's'
exactly once.

Our Computation in the Dynamic Programming Loop is:
A[s][j] = for (k belonging to 's' and k!=j): Miniumum of (A(s-{j},k) + C(k,j))

MY SPACE OPTIMIZATION:
Since We Require only subsets of size 'm-1' in the 'm'th iteration of the Algorithm,
I store only 2 rows viz. 'A_predecessor' and 'A_current' indexed by subsets of size 'm-1' and 'm' respectively
*/
double **A_predecessor;
double **A_current;

//The Linked List 'set_of_length[m-1]' and 'set_of_length[m]' converted to arrays for O(1) indexing in each loop of the algorithm
int *subset_label_predecessor;
int *subset_label_current;


//------Hash Table Data Structure-----------
//A Prime Number Hash Table Size
#define HASH_TABLE_SIZE 33554432

//Hashtable Structure
struct hashTable
{
    //Along with the 'key' i.e. the point, also store the 'index' at which the key is stored in the 'subset_of_length' array
    int key;
    int index;

    //A 'next pointer' Indicating that every Hash table position is a Linked List where colliding values are chained
    struct hashTable *next;
}*hash_table[HASH_TABLE_SIZE];

/*Hash Function
Note: Our Algorithm will get better if a Better Hash Function is Used.
I have used Knuth's Multiplicative Hash Function*/
int h(int x)
{
    return ((long long)x * 33350693) % HASH_TABLE_SIZE;
}

/*Insert Function for Hash Table.
Uses Chaining for Collision Resolving*/
void insert_into_hashtable(int x,int i)
{
    int hx=h(x);

    struct hashTable *new_elem = malloc(sizeof(struct hashTable));
    new_elem->key=x;
    new_elem->index=i;
    new_elem->next=hash_table[hx];
    hash_table[hx]=new_elem;
}

#define NOT_AN_INDEX -1
//Hash Table Lookup function
int lookup(int x)
{
    int hx=h(x);

    struct hashTable *ptr;
    for(ptr=hash_table[hx];ptr!=NULL;ptr=ptr->next)
    {
        if(ptr->key==x)
            return ptr->index;
    }

    return NOT_AN_INDEX;
}


//Main Function
int main()
{
    //Open the Input file in Read Mode
    FILE *fin=fopen("Inputfiles\\tsp.txt","r");
    if(!fin)
    {
        printf("Unable to Read from File");
        exit(0);
    }

    //Read 'n', the numbe of cities
    fscanf(fin,"%d",&n);
    //printf("Number of Cities: %d\n",n);

    //Allocate memort to the'city' array
    city=malloc(n*sizeof(struct City));

    //Read co-ordinates of every city one by one
    int i;
    for(i=0;i<n;i++)
        fscanf(fin,"%lf%lf",&city[i].x,&city[i].y);

    //display_cities();

    fclose(fin);

    //Initialize the Array of Linked Lists 'set_of_length' which stores a Lisr of subsets for every subset size
    set_of_length=calloc(n,sizeof(struct Subset *));
    length_of_set_of_length=calloc(n,sizeof(int));

    //Calculate the Power Set, i.e All subsets of 'n' cities and insert them into 'set_of_length'
    calculate_power_set();

    //Also insert all the subsets from 'set_of_length into hashtable for O(1) look up later in the Algorithm
    insert_set_of_length_into_hashtable();

    //display_set_of_length();

    /*'max-set' is a subset where all 'n' cities are selected i.e. all 'n' bits of the integer are '1'
    Its equivalent integer representation is '2^(n) - 1'*/
    int max_set = pow(2,n)-1;
    //printf("max_set = %d\n",max_set);

    //Store whether 'n' is even or odd
    bool is_n_even = n%2==0;
    //printf(is_n_even ? "N is Even\n" : "N is Odd\n");

    /*Store the Last Iteration of the Dynamic Programming Algorithm in 'last_m'
    Note: The original Dynamic Programming algorithm runs for subset sizes iterating
    over 1,2,3,...,n-1, where 'm+1' gives the subset size at each iteeration.
    i.e. in 1st iteration, we work with subsets of size 2, and so on.

    MY TIME OPTIMIZATION:
    It is Observed that at 'set size = (n-1)/2'(Since source city '1' exists in all paths),
    we have already computed the shortest path upto n/2 cities,
    travelling each city exactly once. But, for every such subset of (n-1)/2 cities traversed, there exists a
    complement subset path which traverses the other n/2 cities(Only common city in the two paths is the source city '1').
    Hence, such complement pairs can be found, and their path lengths can be merged to find the cost of the entire tour.
    Further the shortest such tour, gives us our optimal solution.
    Note: When 'n-1' is odd, complement paths exist across two successive iterations.
    e.g. for 'n' =  6, last_m = 3,
    But since 'n-1' is odd we consider iteration m = 2,3 i.e. subsets of size 3 and 4,
    e.g. there exists a subset 000111b in 2nd iteration and subset 111001 in iteration 3.
    these two subsets form complements as they have only '1' common city, i.e source city and their
    union gives us the entire tour length.
    */
    int last_m = n/2;
    //printf("last_m = %d\n",last_m);

    //-------Dynamic Progrmming Algorithm Begins----------------------

    struct Subset *subset;

    //Copy contents of the linked list of Subsets of size '1' into an array for O(1) indexing
    subset_label_predecessor = malloc(length_of_set_of_length[0]*sizeof(int));
    for(subset=set_of_length[0],i=0;subset!=NULL;subset=subset->next,i++)
        subset_label_predecessor[i]=subset->value;

    //Initialize 'A' the dynamic programming algorithm's storage array
    //Set all path lengths to 'Infinity'
    A_predecessor=malloc(length_of_set_of_length[0]*sizeof(double *));
    for(i=0;i<length_of_set_of_length[0];i++)
    {
        A_predecessor[i]=malloc(n*sizeof(double));
        int p;
        for(p=0;p<n;p++)
            A_predecessor[i][p]=DBL_MAX;
    }

    //Tour starting from source, considering only the source has length '0'
    A_predecessor[0][0]=0;

    //Initialize counters and temp storage variables
    int m;
    int s;
    int j;
    int value_j;
    int s_minus_j,s_minus_j_index;
    int k;
    int value_k;

    printf("Dynamic Programming Loop Begins\n");
    //Iterate over subsets of size 2 to 'n/2'
    for(m=1;m<=last_m;m++)
    {
        //Print Status Message to indicate percentage completion
        printf("%.2f%% Complete\n",(float)2*m/(n-1)*100);

        //Copy the Linked List of subsets of size 'm+1' into an array for O(1) look up
        subset_label_current = malloc(length_of_set_of_length[m]*sizeof(int));
        for(subset=set_of_length[m],i=0;subset!=NULL;subset=subset->next,i++)
            subset_label_current[i]=subset->value;

        /*Allocate memory for the 'A' array to store solutions to subproblems indexed by subsets of size 'm+1',
        i.e. the 'current' iteration*/
        A_current=malloc(length_of_set_of_length[m]*sizeof(double *));
        for(i=0;i<length_of_set_of_length[m];i++)
        {
            A_current[i]=malloc(n*sizeof(double));
            int p;
            for(p=0;p<n;p++)
                A_current[i][p]=DBL_MAX;
        }


        //printf("Length of Set Size %d: %d\n",m+1,length_of_set_of_length[m]);
        //For all subsets of size 'm+1'
        for(s=0;s<length_of_set_of_length[m];s++)
        {
            //For all vetices 'j' acting as end vertices for a path
            for(j=1,value_j=2;j<n;j++,value_j=value_j<<1)
            {
                /*Our Computation in the Dynamic Programming Loop is:
                A[s][j] = for (k belonging to 's' and k!=j): Miniumum of (A(s-{j},k) + C(k,j))*/

                //Check if the 'j'th city exists in our subset i.e. see if jth bit is set
                if(subset_label_current[s] & value_j)
                {
                    /*Calculate the subset after removal of jth city, i.e. turning off the jth bit from
                    the subset integer representation*/
                    s_minus_j = subset_label_current[s] & (~value_j);
                    //printf("s-j = %d\n",s_minus_j);

                    //Look Up Index of s_minus_j in previous subset label i.e. in subsets of size 'm' in O(1) time
                    s_minus_j_index = lookup(s_minus_j);
                    /*for(s_minus_j_index=0;s_minus_j_index<length_of_set_of_length[m-1];s_minus_j_index++)
                        if(subset_label_predecessor[s_minus_j_index]==s_minus_j)
                            break;*/
                    //printf("s-j index = %d\n",s_minus_j_index);

                    //printf("K values:\n");

                    //For all cities 'k' acting as endpoints of a path
                    for(k=0,value_k=1;k<n;k++,value_k=value_k<<1)
                    {
                        //If 'k' is not equal to 'j' and 'k' belongs to the subset
                        if((k!=j) && (subset_label_current[s] & value_k))
                        {
                            //printf("%d ",k);

                            //Calculate dist(k,j) i.e Ckj
                            double Ckj = C(city[k].x,city[k].y,city[j].x,city[j].y);

                            //printf(" A_current[s][j]=%e A_pred=%e C%d%d = %e - ",A_current[s][j],A_predecessor[s_minus_j_index][k],k+1,j+1,Ckj);

                            /*Check if A[s-{j}][k] + Ckj is lesser than the minimum path currently stored in A[s][j]
                            If yes, store it in A[s][j]*/
                            if((A_predecessor[s_minus_j_index][k]!=DBL_MAX) && (A_predecessor[s_minus_j_index][k] + Ckj < A_current[s][j]))
                                A_current[s][j] = A_predecessor[s_minus_j_index][k] + Ckj;

                            //printf("After A_current[s][j]=%e - ",A_current[s][j]);
                        }
                    }
                    //printf("\n");
                }
            }
        }

        /*If 'n' is even we need the 'A' and 'subset' arrays of both 'm-1' and 'm'th iteration, so break
         without freeing memory used by variables of 'm-1'th iteration*/
        if(is_n_even && m==last_m)
            break;

        //Free the memory allocated to variables of 'm-1'th iteration

        for(i=0;i<length_of_set_of_length[m-1];i++)
            free(A_predecessor[i]);
        free(A_predecessor);

        A_predecessor=A_current;

        free(subset_label_predecessor);

        subset_label_predecessor=subset_label_current;

    }


    //Initialze the optimal tour to be Infinity
    double min_tour_cost=DBL_MAX;

    int s1,s2;
    int j1,value1,j2,value2;
    int cost_of_tour;

    if(!is_n_even)
        printf("Computing the Optimal Tour by merging pairs from %d Subsets of Size %d\n",length_of_set_of_length[last_m],last_m);
    else
        printf("Computing the Optimal Tour by merging pairs from %d Subsets of Size %d and %d Subsets of Size %d respectively\n",length_of_set_of_length[last_m],last_m,length_of_set_of_length[last_m-1],last_m-1);

    //If 'n' is even compute shortest tour by merging 2 complement subsets of size 'n/2' each and finding their shortest tour
    if(!is_n_even)
    {
    //For all subsets of size 'n/2'
    for(s1=0;s1<length_of_set_of_length[last_m];s1++)
    {
        //Find complement set by BITWISE XOR with 'max_set'(which has all n bits '1') and then adding '1', the common source city
        int complement_set = (max_set ^ subset_label_current[s1]) + 1;

        //Check whether this complement subset exists by HashTable Lookup
        s2 = lookup(complement_set);
        //If the complement exists
        if(s2!=NOT_AN_INDEX)
        {
            //printf("Pairs: %d %d\n",subset_label_current[s1],subset_label_current[s2]);
            /*By considering all A[s1][j1] and A[s2][j2],
            compute their tour cost by 'A[s1][j1] + A[s2][j] + Cj1j2'
            and store the shortest tour found in optimal tour*/
            for(j1=1,value1=2;j1<n;j1++,value1<<=1)
            {
                if(subset_label_current[s1] & value1)
                {
                    for(j2=1,value2=2;j2<n;j2++,value2<<=1)
                    {
                        if(subset_label_current[s2] & value2)
                        {
                            cost_of_tour = A_current[s1][j1] + C(city[j1].x,city[j1].y,city[j2].x,city[j2].y) + A_current[s2][j2];
                            if(cost_of_tour < min_tour_cost)
                                min_tour_cost = cost_of_tour;
                        }
                    }
                }
            }
        }
    }
    }

    //If 'n' is Odd consider complement subset from previous iteration 'm-1'
    else
    {
    //For al subsets of size n/2
    for(s1=0;s1<length_of_set_of_length[last_m];s1++)
    {
        //Find Complement subset and check if it exists
        int complement_set = (max_set ^ subset_label_current[s1]) + 1;
        s2 = lookup(complement_set);

        //If the complement exists
        if(s2!=NOT_AN_INDEX)
        {
            //printf("Pairs: %d %d\n",subset_label_current[s1],subset_label_predecessor[s2]);

            /*By considering all A[s1][j1] and A[s2][j2],
            compute their tour cost by 'A[s1][j1] + A[s2][j] + Cj1j2'
            and store the shortest tour found in optimal tour
            Note: In this case 's2' is a subset of size 'm' which is a variable of the penultimate iteration
            i.e stored in 'A_predecessor'*/
            for(j1=1,value1=2;j1<n;j1++,value1<<=1)
            {
                if(subset_label_current[s1] & value1)
                {
                    for(j2=1,value2=2;j2<n;j2++,value2<<=1)
                    {
                        if(subset_label_predecessor[s2] & value2)
                        {
                            cost_of_tour = A_current[s1][j1] + C(city[j1].x,city[j1].y,city[j2].x,city[j2].y) + A_predecessor[s2][j2];
                            if(cost_of_tour < min_tour_cost)
                                min_tour_cost = cost_of_tour;
                        }
                    }
                }
            }
        }
    }
    }

    /*for(j=1;j<n;j++)
    {
        double Cj1 = C(city[j].x,city[j].y,city[0].x,city[0].y);
        if((A_predecessor[0][j]!=DBL_MAX) && (A_predecessor[0][j] + Cj1 < min_tour_cost))
            min_tour_cost=A_predecessor[0][j] + Cj1;
    }*/

    //Print the Optimal Tour Found
    printf("Cost of an Optimal Tour: %d\n",(int)min_tour_cost);

    return 0;
}
