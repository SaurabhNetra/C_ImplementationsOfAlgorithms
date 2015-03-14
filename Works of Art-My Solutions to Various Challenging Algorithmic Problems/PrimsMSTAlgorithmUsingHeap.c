/*Programmed by: Saurabh Naresh Netravalkar
                 saurabh.netravalkar@gmail.com

This program problem demonstrates the Prim's minimum spanning tree(MST)
algorithm using Binary Heaps to obtain O(m*log(n)) running time. The input file
describes an undirected graph with integer edge costs. It has the format

[number_of_nodes] [number_of_edges]
[one_node_of_edge_1] [other_node_of_edge_1] [edge_1_cost]
[one_node_of_edge_2] [other_node_of_edge_2] [edge_2_cost]
...

For example, the third line of the file is "2 3 -8874", indicating that there
is an edge connecting vertex #2 and vertex #3 that has cost -8874.

The task is to run Prim's minimum spanning tree(MST) algorithm on this graph and to report
the overall cost of a minimum spanning tree --- an integer.
*/

//Include Files
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<limits.h>
#include<stdbool.h>

//Number of Vertices and Edges respectively
int n,m;

//Structure to store the adjacency list
struct AdjacentVertex
{
    int v;
    int cost;

    struct AdjacentVertex *next;
}**adjacency_list;

void insert_into_adjacency_list(int u,int v,int cost)
{
    struct AdjacentVertex *vertex=malloc(sizeof(struct AdjacentVertex));
    vertex->v=v;
    vertex->cost=cost;
    vertex->next=adjacency_list[u];
    adjacency_list[u]=vertex;
}

void display_adjacency_list()
{
    printf("Adjacency List:\n");
    int u;
    for(u=0;u<n;u++)
    {
        struct AdjacentVertex *v;
        for(v=adjacency_list[u];v!=NULL;v=v->next)
        {
            printf("(%d,%d)=%d ",u+1,v->v+1,v->cost);
        }
        printf("\n");
    }
}

//Heap Data Structure
struct HeapVertex
{
    int label;
    int key;
}*heap;
int heap_len;

//An Array which maps vertices to their position in the heap
int *heap_position;

#define NOT_IN_HEAP -1

//Generic Swap routine
#define swap(x,y,T) {T temp=x; x=y; y=temp;}

/*Insert Function for the heap
Runs in O(log(n)) time*/
void insert_into_heap(int label,int key)
{
    //Append the new Node to the End of the Heap
    struct HeapVertex vertex={label,key};
    heap[heap_len]=vertex;
    heap_position[label]=heap_len;
    heap_len++;

    //Keep Swapping the element with its parent till the Heap Property is Satisfied
    int child,parent;
    for(child=heap_len-1,parent=(child-1)/2;parent>=0 && heap[parent].key>heap[child].key;child=parent,parent=(child-1)/2)
    {
        swap(heap_position[heap[child].label],heap_position[heap[parent].label],int);
        swap(heap[child],heap[parent],struct HeapVertex);
    }
}

/*Function to Extract an Element from Position 'position' from the Heap.
Runs in O(log(n)) time*/
struct HeapVertex delete_from(int position)
{
    //Swap the Element with the last element in the Heap and Remove it
    heap_len--;
    swap(heap_position[heap[position].label],heap_position[heap[heap_len].label],int);
    swap(heap[position],heap[heap_len],struct HeapVertex);
    heap_position[heap[heap_len].label]=NOT_IN_HEAP;

    //Keep Swapping the element at 'position'with its parent till the Heap Property is Satisfied
    int child,parent;
    for(child=position,parent=(child-1)/2;parent>=0 && heap[parent].key>heap[child].key;child=parent,parent=(child-1)/2)
    {
        swap(heap_position[heap[child].label],heap_position[heap[parent].label],int);
        swap(heap[child],heap[parent],struct HeapVertex);
    }

    //Keep Swapping the element at 'position'with its minimum child till the Heap Property is Satisfied
    while(true)
    {
        int left_child=2*position+1,right_child=left_child+1,min_child;

        if(left_child>=heap_len)
            break;
        if(right_child>=heap_len)
            min_child=left_child;
        else
        {
            if(heap[left_child].key<heap[right_child].key)
                min_child=left_child;
            else
                min_child=right_child;
        }

        if(heap[position].key<=heap[min_child].key)
            break;

        swap(heap_position[heap[position].label],heap_position[heap[min_child].label],int);
        swap(heap[position],heap[min_child],struct HeapVertex);

        position=min_child;
    }

    //Return the Removed Element
    return heap[heap_len];
}

//Extracts the Minimum Element i.e. the element at 1st position from the Heap
struct HeapVertex extract_min()
{
    return delete_from(0);
}

//Function to print the contents of Heap
void display_heap()
{
    printf("Heap Contents\n");
    if(heap_len==0)
        printf("Empty");
    int i;
    for(i=0;i<heap_len;i++)
    {
        if(heap[i].key==INT_MAX)
            printf("(label=%d,key=INF) ",heap[i].label+1);
        else
            printf("(label=%d,key=%d) ",heap[i].label+1,heap[i].key);
    }
    printf("\n");
}

//Function to Print the Contents of the Mapping Array
void display_heap_position()
{
    printf("Heap Position\n");
    int i;
    for(i=0;i<n;i++)
        printf("(%d,%d) ",i+1,heap_position[i]);
}

//Main Function
int main()
{
    //Open the Input File in Read Mode
    FILE *fin=fopen("InputFiles\\edges.txt","r");
    if(!fin)
    {
        printf("File cannot be opened");
        return 0;
    }

    //Read 'n' and 'm'
    fscanf(fin,"%d%d",&n,&m);
    printf("n=%d, m=%d\n",n,m);

    //Initialize adjacency list
    adjacency_list=calloc(n,sizeof(struct AdjacentVertex *));

    //Initialize Heap
    heap=malloc(n*sizeof(struct HeapVertex));

    //Initialize the Mapping Array
    heap_position=malloc(n*sizeof(int));
    int i;
    for(i=0;i<n;i++)
        heap_position[i]=NOT_IN_HEAP;

    //Read Edges from File and Insert them into the Adjacency List
    for(i=0;i<m;i++)
    {
        int u,v,cost;

        fscanf(fin,"%d%d%d",&u,&v,&cost);

        u--;
        v--;

        //Since it is an undirected graph, we have two inserts, one for each end vertex of the edge
        insert_into_adjacency_list(u,v,cost);
        insert_into_adjacency_list(v,u,cost);
    }

    //Used for Testing
    //display_adjacency_list();

    //Initialize Random number generator
    srand(time(NULL));

    //Select a random starting vertex for the Algorithm
    int s=rand()%n;

    printf("s=%d\n",s+1);

    //Insert all vertices adjacent to 's' into Heap with 'key' attribute being the edge cost
    struct AdjacentVertex *u;
    for(u=adjacency_list[s];u!=NULL;u=u->next)
    {
        //printf("\nInsert: (label=%d,key=%d)\n",u->v+1,u->cost);
        insert_into_heap(u->v,u->cost);
        //display_heap_position();
    }
    /*Insert all other vertices 't' into heap with 'key' attribute Infinity(Int_Max) since there
    is no known s-t path explored so far*/
    for(i=0;i<n;i++)
    {
        if(i!=s && heap_position[i]==NOT_IN_HEAP)
        {
            //printf("\nInsert: (label=%d,key=INF)\n",i+1);
            insert_into_heap(i,INT_MAX);
            //display_heap_position();
        }
    }

    //display_heap();

    //Initialize the mst cost to 0
    long long mst_cost=0;

    /*Loop till all vertices are visited, exploring one vertex in each iteration.
    Loop Invariant: All Unprocessed Vertices are stored in the Heap, with their 'key'
    attribute being their smallest edge cost with one of the vertices 'v' which are already
    visited i.e heap_index[v] = NOT_IN_HEAP meaning they are not in the heap*/
    for(i=0;i<n-1;i++)
    {
        /*Extract the minimum element from heap i.e vertex who forms the smallest edge with the other vertex
         of the edge being from the set of visited vertices*/
        struct HeapVertex vertex=extract_min();

        //printf("Extracted (label=%d,key=%d)\n",vertex.label+1,vertex.key);

        //Add that smallest edge to mst
        mst_cost+=vertex.key;

        //Update Contents of Heap to Maintain Loop Invariant
        //Repeat For all Vertices adjacent to our newly extracted vertex 'u'
        struct AdjacentVertex *u;
        for(u=adjacency_list[vertex.label];u!=NULL;u=u->next)
        {
            if(heap_position[u->v]==NOT_IN_HEAP)
                continue;

            //If our adjacent vertex is in heap, extract it
            struct HeapVertex v=delete_from(heap_position[u->v]);

            /*Update 'key's of all vertices 'v' in the heap to be the minimum of their current
            value and the edge length between the newly visited vertex 'u' and 'v' */
            if(u->cost<v.key)
                v.key=u->cost;

            //Re-insert it into heap
            insert_into_heap(v.label,v.key);
        }

        //display_heap();

    }

    //Print the Cost of our Resultant MST
    printf("mst_cost=%lld",mst_cost);

    return 0;
}
