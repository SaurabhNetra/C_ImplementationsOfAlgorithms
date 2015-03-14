/*Programmed by: Saurabh Naresh Netravalkar
                 saurabh.netravalkar@gmail.com

This programming problem demonstrates the clustering algorithm (A Modification of the Ksuskal's
Minimum spanning tree Algorithm using the UNION FIND Data Structure WITH RANKS and PATH COMPRESSION
for obtaing O(m*alpha(n)) running time, where alpha(n) is the Inverse Ackermann Function, much better than log(n))
for computing a max-spacing k-clustering in O(m*log(n))

THE CHALLENGE:

The input is VERY VERY BIG GRAPH.
So big, in fact, that the distances (i.e., edge costs) are only defined implicitly, rather than being
provided as an explicit list in the Input file.

The data set format in the Input File is:

[# of nodes] [# of bits for each node's label]
[first bit of node 1] ... [last bit of node 1]
[first bit of node 2] ... [last bit of node 2]
...

For example, the third line of the file "0 1 1 0 0 1 1 0 0 1 0 1 1 1 1 1 1 0 1 0 1 1 0 1" denotes the 24 bits associated
with node #2.

The distance between two nodes u and v in this problem is defined as the Hamming distance--- the number of differing
bits --- between the two nodes' labels. For example, the Hamming distance between the 24-bit label of node #2 above
and the label "0 1 0 0 0 1 0 0 0 1 0 1 1 1 1 1 1 0 1 0 0 1 0 1" is 3 (since they differ in the 3rd, 7th, and 21st bits).

THE PROBLEM:

The task is to find the largest value of k such that there is a k-clustering with spacing at least 3. That is, the number of
clusters that are needed to ensure that no pair of nodes with all but 2 bits in common get split into different clusters.

NOTE: The graph implicitly defined by the data file is so big that we probably can't write it out explicitly, let alone sort
the edges by cost. So I had to be a little creative to complete this part of the question.
For example, I found a way to identify the smallest distances without explicitly looking at every pair of nodes.

MY SOLUTION:
1. I converted all the data points containg 24 characters each (0 or 1 for each bit) into their equivalent Integer values

2. I inserted all Data Points in a Hash Table to facilitate O(1) look up of their array indices

3. For every data point 'p', I calculated 3 kinds of adjacent points(in O(n) time using the hash table!):
    i. Points having a Hamming Distance of '0' with 'p' , i.e. other duplicates of 'p' in the points array
    ii. Points having a Hamming Distance of '1' with 'p'
        i.e. I flipped each of the 24 bits of the point 'p' one by one to compute an adjacent point
            and checked the Hash Table to see whether our computed point exists in our Data set in O(1) time!!
    iii. Extended step 'ii' by flipping one more bit (making it two bits flipped, implying Hamming Distance of two)
        and checked the Hash Table for existence of such a computed point

    All Edges thus Extracted from the Above Steps have a hamming distance of 0,1 or 2 respectively

4. I used a modification of Kruskal's Minimum Spanning tree algorithm, where I merged clusters by using all of the
   extracted edges above

   Thus, I computed the Maximum number 'k' of CLusters such that the min-spacing of the clustering is atleast 3
*/

//Include Files
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

//'n'-> Number of Vertices, 'bpi'-> Bits per Integer
int n,bpi;

//Our Array of Data Points
int *point;

/*
This function counts the number of 1s in the bit representation of an integer in almost O(1) time!!

Reference: http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel

Used it only for debugging purposes, but kept it since its a cool function to have in the repo!!
*/
int NumberOfSetBits(int i)
{
     i = i - ((i >> 1) & 0x55555555);
     i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
     return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}

//Calculates the Hamming Distance between 2 integers
int hamming_distance(int n1,int n2)
{
    /*Return the Number of 1s in the BITWISE XOR of the two numbers
    Note: Bitwise XOR operation '^' places a '1' only in those bit positions where the values of
    'n1' and 'n2' differ */
    return NumberOfSetBits(n1^n2);
}

//Data Structure for storing Edges as a Linked List
struct Edge_List_Node
{
    int u,v,cost;
    struct Edge_List_Node *next;
}*edge_list;

//Function to insert an Edge into the Edge List
void add_edge(int u,int v,int cost)
{
    struct Edge_List_Node *new_edge=malloc(sizeof(struct Edge_List_Node));
    new_edge->u=u;
    new_edge->v=v;
    new_edge->cost=cost;

    new_edge->next=edge_list;
    edge_list=new_edge;
}

//A Prime Number Hash Table Size (Approximately thrice of that of our data set which is 20,000)
#define HASH_TABLE_SIZE 65521

//Hashtable Data Structure
struct hashTable
{
    //Along with the 'key' i.e. the point, also store the 'index' at which the key is stored in our 'point' array
    int key;
    int index;

    //A 'next pointer' Indicating that every Hash table position is a Linked List where colliding values are chained
    struct hashTable *next;
}*hash_table[HASH_TABLE_SIZE];

//Store the Lengths of the Chains of every position in the Hash Table
int hash_table_chain_len[HASH_TABLE_SIZE];

/*Simple Modulo Division Hash Function.
Note: Our Algorithm will get better if a Better Hash Function is Used.
But since hashing is not our learning goal in this problem, I have used a simple hash function
which works reasonably well too, since our table size is prime*/
int h(int x)
{
    return x % HASH_TABLE_SIZE;
}

/*Insert Function for Hash Table.
Uses Chaining for Collision Resolving.

Note: I Tried Using Linear Probing Earlier, which turns out to be incorrect for our specific problem
since, our hash table's 'lookup' function has been modified to return not the 'key'
but a list of all 'indices' in the 'point' array, where a point having an integer representation of 'key' is stored.

Thus, Linear Probing and Other such techniques actually might distribute multiple inserts of points which hash
to the same key(integer representation) in a way which cannot be tracked by the 'lookup' function*/
void insert(int x,int i)
{
    int hx=h(x);

    struct hashTable *new_elem = malloc(sizeof(struct hashTable));
    new_elem->key=x;
    new_elem->index=i;
    new_elem->next=hash_table[hx];
    hash_table[hx]=new_elem;

    hash_table_chain_len[hx]++;
}

#define NOT_AN_INDEX -1
/*Hash Table Lookup function
Returns a list of indices of all values having the same 'hash key'*/
int * lookup(int x)
{
    int hx=h(x);

    if(hash_table_chain_len[hx]==0)
        return NULL;

    int *indices=malloc(hash_table_chain_len[hx]*sizeof(int));
    int i;
    for(i=0;i<hash_table_chain_len[hx];i++)
        indices[i]=NOT_AN_INDEX;

    int indices_len=0;

    struct hashTable *ptr;
    for(ptr=hash_table[hx];ptr!=NULL;ptr=ptr->next)
    {
        if(ptr->key==x)
            indices[indices_len++]=ptr->index;
    }

    return indices;
}

/*The Union-Find Data Structure
Refer to: http://en.wikipedia.org/wiki/Disjoint-set_data_structure
Note:
1. The Traditional version of Union Find enables us to run our clustering algorithm in O(m*log(n)) time
2. The below implemented Union Find Data Structure is further Optimized and incorporates 'Union by Rank'
with 'Path Compression', thus enabling us to run our algorithm in O(m*alpha(n)), where alpha(n) is the Inverse
Ackermann function and grows much slower than log(n)
*/

//Array to store parent and ranks of vertices respectively
int *parent,*rank;

//Find function, finds the Leader Vertex of the group of which vertex 'x' is part of
int find(int x)
{
    //Keep traversing up the tree till the leader is found and store the leader in root
    int root=x;
    while(parent[root]!=root)
        root=parent[root];

    /*Loop for Path Compression.
    Keep updating parent pointers all the way up the tree starting from 'x'.
    This ultimately leads to a flatter tree which boosts the speed of further 'find' operations
    */
    while(parent[x]!=x)
    {
        int next_x=parent[x];

        parent[x]=root;

        x=next_x;
    }

    return x;
}

/*Function to merge two vertex groups.
the function uses 'ranks' to maintain O(log(n)) tree depth and to prevent the worst case of O(n)*/
void union_by_rank(int x,int y)
{
    //Find the leaders of both vertices whose groups are to be merged
    int s1=find(x),s2=find(y);

    //Merge According to Ranks and Update the necessary ranks
    if(rank[s1]>rank[s2])
        parent[s2]=s1;
    else
    {
        parent[s1]=s2;
        if(rank[s1]==rank[s2])
            rank[s2]++;
    }
}

//Main Function
int main()
{
    //Open Input file in read mode
    FILE *fin=fopen("InputFiles\\clustering2.txt","r");
    if(!fin)
    {
        printf("Error Opening Input File");
        return 0;
    }

    //Read 'n' and 'bpi'
    fscanf(fin,"%d%d",&n,&bpi);

    printf("n = %d\nbits per integer = %d\n",n,bpi);

    //Initialize the 'point' array to store the data points
    point=calloc(n,sizeof(int));

    //A Multiplier used to convert 'bit representation using characters' to its integer form
    int max_multiplier=pow(2,bpi-1);

    //Extract Data Points One by One and store their Integer Forms in the 'point' array
    int i;
    for(i=0;i<n;i++)
    {
        int multiplier=max_multiplier;
        int j;
        for(j=0;j<bpi;j++)
        {
            int value;
            fscanf(fin,"%d",&value);

            point[i]+=value*multiplier;

            multiplier>>=1;

            //printf("multiplier = %d\n",multiplier);
        }
        //printf("point[%d] = %d\n",i+1,point[i]);
    }

    //printf("point[%d] = %d\n",n,point[n-1]);

    //printf("%d",hamming_distance(point[0],point[1]));

    //Insert All the Points into the Hash Table
    for(i=0;i<n;i++)
    {
        insert(point[i],i);
    }

    /*--------------Algorithm to Compute the Edges having Distances less than 3 i.e. 0,1 or 2--------------
    Note: This Algorithm Works in O(n) time as opposed to O(n^2) where all pairs of points are compared*/

    //For every Data Point
    for(i=0;i<n;i++)
    {
        //---Calculate the other points at hamming distance of zero and create edges between them---

        //The Point we are looking for in the hash table
        int point_at_dist_0 = point[i];
        //Look up that point and get all indices having this point value in the 'point' array
        int *point_at_dist_0_indices = lookup(point_at_dist_0);

        //For all Obtained Points Create an edge between 'i' and the other point's index
        int ind0;
        for(ind0=0;ind0<hash_table_chain_len[h(point_at_dist_0)];ind0++)
        {
            if(point_at_dist_0_indices[ind0]==NOT_AN_INDEX)
                break;

            //If the Other Point is 'i' itself, there should'nt be a self loop
            if(i==point_at_dist_0_indices[ind0])
                continue;

            add_edge(i,point_at_dist_0_indices[ind0],1);
        }

        //---Calculate the other points at hamming distance of 1 and create edges between them---

        //For all 'bpi'(24 in our case) bits
        int j,multiplier1;
        for(j=0,multiplier1=1;j<bpi;j++,multiplier1<<=1)
        {
            //flip jth bit of our ith point to obtain the adjacent point
            int point_at_dist_1 = point[i] ^ multiplier1;

            //Look up that point and get all indices having this point value in the 'point' array
            int *point_at_dist_1_indices = lookup(point_at_dist_1);

            //For all Obtained Points Create an edge between 'i' and the other point's index
            int ind1;
            for(ind1=0;ind1<hash_table_chain_len[h(point_at_dist_1)];ind1++)
            {
                if(point_at_dist_1_indices[ind1]==NOT_AN_INDEX)
                    break;

                add_edge(i,point_at_dist_1_indices[ind1],1);
            }

            //---Calculate the other points at hamming distance of 2 and create edges between them---

            //For all 'bpi'(24 in our case) bits of 'point_at_dist_1'
            int k,multiplier2;
            for(k=0,multiplier2=1;k<bpi;k++,multiplier2<<=1)
            {
                //If the Same bit is flipped again, it will lead to our original point 'i'
                if(k==j)
                    continue;

                //flip kth bit of 'point_at_dist_1' to obtain the adjacent point
                int point_at_dist_2 = point_at_dist_1 ^ multiplier2;

                //Look up that point and get all indices having this point value in the 'point' array
                int *point_at_dist_2_indices = lookup(point_at_dist_2);

                //For all Obtained Points Create an edge between 'i' and the other point's index
                int ind2;
                for(ind2=0;ind2<hash_table_chain_len[h(point_at_dist_2)];ind2++)
                {
                    if(point_at_dist_2_indices[ind2]==NOT_AN_INDEX)
                        break;

                    add_edge(i,point_at_dist_2_indices[ind2],2);
                }
            }
        }
    }

    //---------------------Modified Kruskal's Minimum Spanning Tree Algorithm Begins-------------------

    /*Initialize the union find data structure so that every vertex is in its own cluster
    i.e. there 'n' clusters of size 1 each*/
    parent=malloc(n*sizeof(int));
    for(i=0;i<n;i++)
        parent[i]=i;
    rank=calloc(n,sizeof(int));

    int k=n;

    /*Traverse the Computed Edge List
    Note: The List Need not be Sorted since we already know that we want to merge all these edges anyway*/
    struct Edge_List_Node *edge;
    for(edge=edge_list;edge!=NULL;edge=edge->next)
    {
        //If the end points of both edges are from different clusters
        if(find(edge->u)!=find(edge->v))
        {
            //merge the 2 clusters
            union_by_rank(edge->u,edge->v);
            k--;
        }
    }

    //Print the Result!!!
    printf("Largest value of k such that there is a k-clustering with spacing at least 3 :\nk = %d\n",k);

    return 0;
}
