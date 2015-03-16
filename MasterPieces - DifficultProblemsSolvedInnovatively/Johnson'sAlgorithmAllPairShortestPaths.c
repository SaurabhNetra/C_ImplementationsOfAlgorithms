/*Programmed by: Saurabh Naresh Netravalkar
                 saurabh.netravalkar@gmail.com

This  program implements the Johnson's Algorithm for solving the all-pairs shortest-path problem in O(m*n*log(n))

Reference: http://en.wikipedia.org/wiki/Johnson's_algorithm

Johnson's Algorithm consists of the following steps:

1. Invocation of a Bellman Ford single pair shortest path algorithm(which runs in O(m*n)) time on the original graph
augmented with an additional vertex 's' as a source vertex and has edges of zero length with all vertices of the original graph.
The shortest paths from 's' to all other vertices 'v' are stored as labels 'Pv'

2. All edges 'ce' of the original graph are re-weighted by the formula: ce_new = ce + Pu - Pv
This formula makes all edge lengths non-negative and simultaneously also preserves shortest paths.

3. Non-negative edge lengths now allow us to use the faster Dijktra's Single Pair Shortest Path
algorithm (which runs in O(m*log(n)) time using heaps) 'n' times, i.e. for all vertices as sources.

4. We can then re-compute the original s-t shortest path edge lengths by subtracting the factor (Ps - Pt)

5. We're Done!! The Overall running time is O(m*n*log(n)) as opposed to O(n^3) for Floyd Warshall's Algorithm

The Problem:
There are 3 input data files g1.txt, g2.txt and g3.txt respectively describing three graphs: graph #1; graph #2; graph #3.

The Input File Format is as follows:
The first line indicates the number of vertices and edges, respectively.
Each subsequent line describes an edge (the first two numbers are its tail and head, respectively)
and its length (the third number).
NOTE: some of the edge lengths are negative.
NOTE: These graphs may or may not have negative-cost cycles.

The task is to compute the "shortest shortest path". Precisely, we must first identify which, if any, of the three graphs
have no negative cycles. For each such graph, we should compute all-pairs shortest paths and remember the smallest one
(i.e., compute minimum of d(u,v) for u,v∈V, where d(u,v) denotes the shortest-path distance from u to v).
*/

//Include Files
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<stdbool.h>
#include<string.h>

//------Graph Parameters-----------------------------

//n -> Number of Vertices, m -> Numbr of Edges
int n,m;

//Data Structure to store a Linked list of Graph Edges
struct Edge
{
    int u,v,cost;

    struct Edge *next;
}*edge_list;

//Add an Edge to the graph
void insert_into_edge_list(int u,int v,int cost)
{
    struct Edge *edge=malloc(sizeof(struct Edge));
    edge->u=u;
    edge->v=v;
    edge->cost=cost;
    edge->next=edge_list;
    edge_list=edge;
}

//Free the memory allocated to the graph edges
void free_edge_list()
{
    while(edge_list!=NULL)
    {
        struct Edge *next_edge=edge_list->next;

        free(edge_list);

        edge_list = next_edge;
    }
}

//Structure to store the adjacency list
struct EdgePointer
{
    struct Edge *edge;

    struct EdgePointer *next;
}**adjacency_list;

//Function to Insert Into Adjacency List
void insert_into_adjacency_list(struct Edge *edge)
{
    struct EdgePointer *edge_pointer=malloc(sizeof(struct EdgePointer));
    edge_pointer->edge=edge;
    edge_pointer->next=adjacency_list[edge->u];
    adjacency_list[edge->u]=edge_pointer;

    edge_pointer=malloc(sizeof(struct EdgePointer));
    edge_pointer->edge=edge;
    edge_pointer->next=adjacency_list[edge->v];
    adjacency_list[edge->v]=edge_pointer;
}

//Free the memory allocated to the adjacency list
void free_adjacency_list()
{
    int u;
    for(u=0;u<=n;u++)
    {
        while(adjacency_list[u]!=NULL)
        {
            struct EdgePointer *next_edge_pointer = adjacency_list[u]->next;

            free(adjacency_list[u]);

            adjacency_list[u]=next_edge_pointer;
        }
    }
}

//Print the Adjacency List
void display_adjacency_list()
{
    printf("Adjacency List:\n");
    int u;
    for(u=0;u<=n;u++)
    {
        struct EdgePointer *edge_pointer;
        for(edge_pointer=adjacency_list[u];edge_pointer!=NULL;edge_pointer=edge_pointer->next)
        {
            printf("(%d,%d)=%d ",edge_pointer->edge->u,edge_pointer->edge->v,edge_pointer->edge->cost);
        }
        printf("\n");
    }
}

//An Array to Store the Vertex Labels Pv computed by the Bellman Ford invocation (Step 1 of our Algorithm)
int *P;

//To Print 'P' for Debugging Purposes
void display_P()
{
    printf("Shortest Paths from the Bellman Ford Algorithm:\n");
    int i;
    for(i=1;i<=n;i++)
        printf("(0,%d)=%d ",i,P[i]);
    printf("\n");
}

/*The Bellman Ford Single Source Shortest Path Algorithm
using Dynamic Programming(Also Uses Space Optimization and Early Stopping)
It runs in O(m*n) running time

Parameters:
's' -> source vertex

Return Value:
If the graph has no negatie edge cycles, the function sets the vertex labels 'P' for all vertices 'v' and returns true
If negative edge cycles are found, the algorithm returns false
*/
bool BellmanFord(int s)
{
    /*Initialze our 2-D array with only two rows: A[0] is the predecessor and A[1] is the row being computed
    A[i][v] signifies the shortest path from source 's' to vertex 'v' containing only the first 'i' vertices
    as interior nodes of the path
    */
    int A[2][n+1];

    //Initialize the 1st row

    //Distance to source with no interior nodes is zero
    A[0][s]=0;

    //Distance to all other destination with no interior nodes is Infinity
    int v;
    for(v=1;v<=n;v++)
        A[0][v]=INT_MAX;

    /*Keep solving subproblems A[i][v] row by row till we compute A[n-1][v]
    Note: A shortest negative cycle free path has atmost n-1 intermediate vertices
    Note: We use one extra iteration to compute A[n][v]. Negative edge cycles exist
    if the shortest paths show further reduction in this last iteration*/
    int i;
    bool stop_early;
    bool negativeEdgeCycle=false;
    for(i=1;i<=n;i++)
    {
        stop_early = true;

        for(v=0;v<=n;v++)
        {
            /*Optimal Sub-structure
            A[i][v] = minimum of
            1. A[i-1][v]
            2. A[i-1][w] + edge length of edge(w,v), for all vertices'w'
            */
            A[1][v]=A[0][v];

            struct EdgePointer *edge_pointer;
            for(edge_pointer=adjacency_list[v];edge_pointer!=NULL;edge_pointer=edge_pointer->next)
            {
                if(edge_pointer->edge->v==v && (A[0][edge_pointer->edge->u]!=INT_MAX) && (A[0][edge_pointer->edge->u] + edge_pointer->edge->cost < A[1][v]))
                {
                    A[1][v] = A[0][edge_pointer->edge->u] + edge_pointer->edge->cost;

                    stop_early = false;
                }
            }
        }

        //Our Newly Computed Row now becomes the Predecessor
        for(v=0;v<=n;v++)
            A[0][v] = A[1][v];

        /*If for all vertices 'v', A[i][v] was equal to A[i-1][v], then we have already found the shortest paths
         and it is safe to halt*/
        if(stop_early)
            break;
    }

    /*If for all vertices 'v' A[n][v] was not equal to A[n-1][v], that means paths are still decreasing due to a
    negative edge cycle. Hence return false*/
    if(i==n+1 && !stop_early)
        return false;

    //Set the Vertex Labels by extracting the shortest paths computed in A[0]
    for(i=1;i<=n;i++)
        P[i]=A[0][i];

    return true;

}

//--------------------Parameters for Dijkstra's Algorithm---------------

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

//The Result to be Computed. Initialize it to infinity
int ShortestShortestPath = INT_MAX;

//Main Function
int main()
{
    //Run the Entire Algorithm for all 3 input graphs
    int g;
    for(g=1;g<=3;g++)
    {

    printf("Graph %d\n",g);

    char *file_name;
    switch(g)
    {
    case 1:
        file_name="g1.txt";
        break;
    case 2:
        file_name="g2.txt";
        break;
    case 3:
        file_name="g3.txt";
        break;
    }

    char file_path[50] = "InputFiles\\";

    //Open the File containing the 1st Input Graph in Read Mode
    FILE *fin=fopen(strcat(file_path,file_name),"r");
    if(!fin)
    {
        printf("Cannot Read from File");
        return 0;
    }

    //Read 'n' and 'm'
    fscanf(fin,"%d%d",&n,&m);
    printf("n = %d, m = %d\n",n,m);

    /*Initialize adjacency list to have 'n + 1' vertices,
    since we will add a vertex 's = 0', with 's-v' edges of length zero to the graph
    to compute a reweighing of the graph edges to have non-negative weights using an
    invocation to the Bellman Ford algorithm with source 's'*/
    adjacency_list=calloc(n+1,sizeof(struct AdjacentVertex *));

    //Read Edges from File and Insert them into the Adjacency List
    int i;
    for(i=0;i<m;i++)
    {
        int u,v,cost;

        fscanf(fin,"%d%d%d",&u,&v,&cost);

        insert_into_edge_list(u,v,cost);

        insert_into_adjacency_list(edge_list);
    }

    fclose(fin);

//-------------Johnson's Algorithm Begins------------------------

    //----Step 1. Augment the Graph---------
    //Add a vertex 's' with label '0' to our graph and all 's-v' edges of length '0'
    int s=0;

    for(i=1;i<=n;i++)
    {
        insert_into_edge_list(s,i,0);

        insert_into_adjacency_list(edge_list);
    }

    //display_adjacency_list();


    //-----Bellman Ford Invocation----------

    //Initialize the 'P' array which stores the vertex labels
    P = malloc((n+1)*sizeof(int));

    //Call the BellmanFord Algorithm
    bool negativeCycleTest = BellmanFord(0);

    //Print whether Negative Cycles exist
    printf("%s\n", negativeCycleTest ? "No Negative Cycles" : "Negative Cycles Exist");

    //If Negative Cycles do not exist, Proceed to next Step
    if(negativeCycleTest)
    {
        //display_P();

        //-------Step 2. Re-Weigh the Graph using the 'P' labels-----------
        struct Edge *edge;
        //printf("Re-Weighted Graph:\n");
        for(edge=edge_list;edge!=NULL;edge=edge->next)
        {
            if(edge->u != s)
            {
                edge->cost = edge->cost + P[edge->u] - P[edge->v];
                //printf("(%d,%d)=%d ",edge->u,edge->v,edge->cost);
            }
        }

        //-----Step 3. 'n' Invocations to Dijkstra's Algorithm--------------

        //Initialize Heap
        heap=malloc((n+1)*sizeof(struct HeapVertex));

        //Initialize the Mapping Array
        heap_position=malloc((n+1)*sizeof(int));

        int i;
        for(i=0;i<=n;i++)
            heap_position[i]=NOT_IN_HEAP;

        //Array to store the computed shortest s-t paths
        int *ShortestPath = malloc((n+1)*sizeof(int));

        //printf("\nAll Pair Shortest Paths:\n");
        for(s=1;s<=n;s++)
        {
            ShortestPath[s]=0;

            //Insert all vertices adjacent to 's' into Heap with 'key' attribute being the edge cost
            struct EdgePointer *edge_pointer;
            for(edge_pointer=adjacency_list[s];edge_pointer!=NULL;edge_pointer=edge_pointer->next)
            {
                if(edge_pointer->edge->u == s)
                {
                    insert_into_heap(edge_pointer->edge->v,edge_pointer->edge->cost);
                    ShortestPath[edge_pointer->edge->v] = edge_pointer->edge->cost;
                }
            }

            /*Insert all other vertices 't' into heap with 'key' attribute Infinity(Int_Max) since there
            is no known s-t path explored so far*/
            for(i=1;i<=n;i++)
            {
                if(i!=s && heap_position[i]==NOT_IN_HEAP)
                {
                    insert_into_heap(i,INT_MAX);
                    ShortestPath[i] = INT_MAX;
                }
            }

            /*Explore one Vertex in each loop till all 'n' vertices are explored.
            Loop Invariant: All Unprocessed Vertices 't' are stored in the Heap, with their 'key'
            attribute being the shortest s-t path discovered so far only considering all those vertices 'v' which are already
            visited, to be part of those shortest paths*/
            for(i=1;i<=n-1;i++)
            {
                //Extract the Nearest Vertex from the Heap
                struct HeapVertex min_node=extract_min();

                /*The extracted vertex now becomes visited.
                Hence Update the 'key' attributes of all Vertices 'v' in the Heap
                to contain the minimum of
                1. key of 'v'
                2. key of 'min_node'+dist('min_node','v')
                to maintain the Loop Invariant
                */
                struct EdgePointer *ptr;
                for(ptr=adjacency_list[min_node.label];ptr!=NULL;ptr=ptr->next)
                {
                    if(ptr->edge->u==min_node.label && heap_position[ptr->edge->v]!=NOT_IN_HEAP)
                    {
                        //Extract the vertex 'v' from the Heap
                        struct HeapVertex node=delete_from(heap_position[ptr->edge->v]);

                        /*Find the Shorter Path among the two options stated above
                        and re-insert it into the Heap*/
                        int path = (ShortestPath[min_node.label]!=INT_MAX && ptr->edge->cost!=INT_MAX) ? ShortestPath[min_node.label] + ptr->edge->cost : INT_MAX;
                        if(path<node.key)
                        {
                            node.key=path;
                            ShortestPath[node.label]=path;
                        }
                        insert_into_heap(node.label,node.key);
                    }
                }
            }

            //printf("Shortest Paths from Source %d: \n",s);
            for(i=1;i<=n;i++)
            {
                if(ShortestPath[i]!=INT_MAX)
                    ShortestPath[i] += P[i]-P[s];
                //printf("%d=%d ",i,ShortestPath[i]);

                //Store the ShortestShortestPath computed so far
                if(ShortestPath[i] < ShortestShortestPath)
                    ShortestShortestPath = ShortestPath[i];
            }
            //printf("\n");
        }
        //Free the unrequired memory
        free(ShortestPath);

        //Print the Shortest Shortest Path for the graph under consideration
        printf("Shortest Shortest Path for Graph is %d\n",ShortestShortestPath);
    }

    //Free all Graph Parameter, Heap Parameters to prepare them to work with next graph
    free_edge_list();
    free_adjacency_list();
    free(P);
    free(heap);
    free(heap_position);

    }
    return 0;
}
