/*Programmed by: Saurabh Naresh Netravalkar
                 saurabh.netravalkar@gmail.com

This programming problem demonstrates the clustering algorithm (A Modification of the Ksuskal's
Minimum spanning tree Algorithm using the UNION FIND Data Structure WITH RANKS and PATH COMPRESSION
for obtaing O(m*alpha(n)) running time, where alpha(n) is the Inverse Ackermann Function, much better than log(n))
for computing a max-spacing k-clustering

The input file describes a distance function (equivalently, a complete graph with edge costs).
It has the following format:

[number_of_nodes]
[edge 1 node 1] [edge 1 node 2] [edge 1 cost]
[edge 2 node 1] [edge 2 node 2] [edge 2 cost]
...

There is one edge (i,j) for each choice of 1 ≤ i < j ≤ n, where n is the number of nodes. For example, the third
line of the file is "1 3 5250", indicating that the distance between nodes 1 and 3 (equivalently, the cost of the edge (1,3))
is 5250. The distances are positive, but NOT necessarily distinct.

The task in this problem is to run the clustering algorithm on this data set, where the target number
'k' of clusters is set to 4. The goal is to find the maximum spacing of a 4-clustering of this data set.
*/

//Include Files
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>

//Numbber of Clusters to be formed
#define REQUIRED_NUMBER_OF_CLUSTERS 4

//To store the number of vertices and edges respectively
int n,m;

//An Array of Edges of the data set
struct Edge
{
    int u,v,cost;
}*edge;

//A function to compare to edges considering the edge costs
int edge_cmp(const void *edge1, const void *edge2)
{
    return ((struct Edge *)edge1)->cost - ((struct Edge *)edge2)->cost;
}

//A utility function to print the edges
void display_edges()
{
    int i;
    printf("Edges:\n");
    for(i=0;i<m;i++)
    {
        printf("(%d,%d,%d) ",edge[i].u+1,edge[i].v+1,edge[i].cost);
    }
    printf("\n");
}

/*The Union-Find Data Structure
Refer to: http://en.wikipedia.org/wiki/Disjoint-set_data_structure
Note:
1. The Traditional version of Union Find enables us to run our clustering algorithm in O(m*log(n)) time
2. The below implemented Union Find Data Structure is further Optimized and incorporates 'Union by Rank'
with 'Path Compression', thus enabling us to run our algorithm in O(m*alpha(n)), where alpha(n) is the Inverse
Ackermann function and grows much slower than log(n)
*/

//Array to store parent and ranks of vertices respecctively
int *parent,*rank;

//Find function, finds the Leader Vertex of the group of which vertex 'x' is part of
int find(int x)
{
    int root=x;
    //Keep traversing up the tree till the leader is found and store the leader in root
    while(parent[root]!=root)
        root=parent[root];

    /*Loop for Path Compression
    Keep updating parent pointers all the way up the tree starting from 'x'
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

/*Function to merge two vertex groups
the function uses 'ranks' to maintain O(log(n)) tree depth and to prevent the worst case of O(n)*/
void union_by_rank(int x,int y)
{
    //Find the leaders of both vertices whose groups are to be merged
    int s1=find(x),s2=find(y);

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
    FILE *fin=fopen("InputFiles\\clustering1.txt","r");
    if(!fin)
    {
        printf("Error Opening Input File");
        return 0;
    }

    //Read 'n' the number of vertices
    fscanf(fin,"%d",&n);

    //A Complete Graph has n(n-1)/2 edges
    m=n*(n-1)/2;

    printf("n=%d,m=%d\n",n,m);

    edge=malloc(m*sizeof(struct Edge));

    //Extract edges one by one
    int i;
    for(i=0;i<m;i++)
    {
        struct Edge new_edge;

        fscanf(fin,"%d%d%d",&new_edge.u,&new_edge.v,&new_edge.cost);

        //Since we use zero based indexing and Vertex numbers start from '1' in the input file
        new_edge.u--;
        new_edge.v--;

        edge[i]=new_edge;
    }
    fclose(fin);

    //display_edges();

    //------Kruskal's Algorithm Begins--------

    //Sort the Edges
    qsort(edge,m,sizeof(struct Edge),edge_cmp);

    //display_edges();

    /*Initialize the union find data structure so that every vertex is in its own cluster
    i.e. there 'n' clusters of size 1 each*/
    parent=malloc(n*sizeof(int));
    for(i=0;i<n;i++)
        parent[i]=i;
    rank=calloc(n,sizeof(int));

    //Number of Current Clusters
    int k=n;

    //Traverse the Sorted Edge List
    for(i=0;i<m;i++)
    {
        //If the end points of both edges are from different clusters
        if(find(edge[i].u)!=find(edge[i].v))
        {
            //merge the 2 clusters
            union_by_rank(edge[i].u,edge[i].v);

            //decrement the number of clusters by 1, till we reach the Required number of clusters, in our case '4'
            if(--k==REQUIRED_NUMBER_OF_CLUSTERS)
                break;
        }
    }

    /*Calculate the Maximum Spacing of our k - Clustering
    i.e. the minimum edge length having both end points in 2 different clusters*/
    int cluster_spacing=INT_MAX;
    for(i=0;i<m;i++)
    {
        if(find(edge[i].u)!=find(edge[i].v) && edge[i].cost<cluster_spacing)
            cluster_spacing=edge[i].cost;
    }

    //Print out the Result
    printf("Spacing of a %d-clustering is %d\n",REQUIRED_NUMBER_OF_CLUSTERS,cluster_spacing);

    return 0;
}
