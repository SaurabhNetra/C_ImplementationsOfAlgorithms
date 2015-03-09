/*Programmed by: Saurabh Naresh Netravalkar
                 saurabh.netravalkar@gmail.com

This Program is an implementation of the Dijkstra's Shortest Path Algorithm
which uses Heaps to obtain a running time of O(m*log(n))*/

//Include Files
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<limits.h>

//The Number of Vertices
#define N 200

#define INFINITY 1000000
#define NOT_IN_HEAP -1

//A Generic Swap Function
#define swap(X,Y,T) {T t=X; X=Y; Y=t;}

//A Structure for storing the Adjacency List Representation of the Graph
struct Vertex
{
    //The adjacent Vertex's label
    int label;
    //The length of that edge
    int edge_len;

    struct Vertex *next;
}*V[N];

//Heap Data Structure Implementation
struct HeapNode
{
    int vertex;
    int key;
}Heap[N];
int heap_len;

/*An Array which Maps a Particular Vertex to its Position in the Heap
e.g. heap_pos[1] gives the position of vertex labelled '1' in the 'Heap' array*/
int heap_pos[N];

/*Insert Function for the heap
Runs in O(log(n)) time*/
void insert(struct HeapNode heap_node)
{
    //Append the new Node to the End of the Heap
    int i=heap_len;
    heap_pos[heap_node.vertex]=i;
    Heap[heap_len++]=heap_node;

    //Keep Swapping the element with its parent till the Heap Property is Satisfied
    int parent=(i-1)/2;
    while(parent>=0 && Heap[i].key<Heap[parent].key)
    {
        swap(heap_pos[Heap[i].vertex],heap_pos[Heap[parent].vertex],int);
        swap(Heap[i],Heap[parent],struct HeapNode);
        i=parent;
        parent=(i-1)/2;
    }
}

/*Auxiliary Function to Extract an Element from Position 'pos' from the Heap
Runs in O(log(n)) time*/
struct HeapNode extract_aux(int pos)
{
    //Swap the Element with the last element in the Heap and Remove it
    heap_len--;
    swap(heap_pos[Heap[pos].vertex],heap_pos[Heap[heap_len].vertex],int);
    swap(Heap[pos],Heap[heap_len],struct HeapNode);

    //Swap the Newly placed element in 'pos' with its Children till the Heap Property is Satisfied
    int i=pos;
    while(1)
    {
        int left_child=2*i+1,right_child=left_child+1,next_node;

        if(left_child>=heap_len)
            break;
        else if(right_child>=heap_len)
            next_node=left_child;
        else
        {
            if(Heap[left_child].key<Heap[right_child].key)
                next_node=left_child;
            else
                next_node=right_child;
        }
        if(Heap[next_node].key>=Heap[i].key)
            break;
        swap(heap_pos[Heap[next_node].vertex],heap_pos[Heap[i].vertex],int);
        swap(Heap[next_node],Heap[i],struct HeapNode);

        i=next_node;
    }

    heap_pos[Heap[heap_len].vertex]=NOT_IN_HEAP;

    //Return the Element which was Removed from the Heap
    return Heap[heap_len];
}

//Extract the Minimum Element from the Heap
struct HeapNode extract_min()
{
    return extract_aux(0);
}

//Delete a Node stored t position 'pos' from the Heap
struct HeapNode delete_At_pos(int pos)
{
    return extract_aux(pos);
};

//Main Function
int main()
{
    //Open the Input File in Read Mode
    FILE *fin=fopen("res\\dijkstraData.txt","r");

    /*Extract Edges from the Input file and create the Adjacencyt List
    The file contains an adjacency list representation of an undirected
    weighted graph with 200 vertices labeled 1 to 200. Each row consists
    of the node tuples that are adjacent to that particular vertex along
    with the length of that edge. For example, the 6th row has 6 as the
    first entry indicating that this row corresponds to the vertex labeled 6.
    The next entry of this row "141,8200" indicates that there is an edge
    between vertex 6 and vertex 141 that has length 8200. The rest of the pairs
    of this row indicate the other vertices adjacent to vertex 6 and the lengths
    of the corresponding edges.*/
    int i,j,len;
    int args;
    struct Vertex *v_new;
    while((args=fscanf(fin,"%d,%d",&j,&len))!=EOF)
    {
        switch(args)
        {
            case 1:
                i=j-1;
                break;
            case 2:
                v_new=(struct Vertex *)malloc(sizeof(struct Vertex));
                v_new->label=j-1;
                v_new->edge_len=len;
                v_new->next=V[i];
                V[i]=v_new;
                break;
        }
    }
    fclose(fin);

    //Initialize the 'heap_pos' array
    for(i=0;i<N;i++)
        heap_pos[i]=NOT_IN_HEAP;

    //Source Vertex
    int s=0;

    //Array to store the computed shortest s-t paths
    int A[N];

    //Initialize 'A' with all paths having Infinite Length
    for(i=0;i<N;i++)
    {
        A[i]=INFINITY;
    }

    //s-s path has length 0
    A[s]=0;

    /*Update the shortest path array 'A' considering all edges originating from 's'
    and Insert all vertices adjacent to 's' into the 'Heap' with their 'key' attribute set to
    the corresponding edge lengths*/
    struct Vertex *ptr;
    for(ptr=V[s];ptr!=NULL;ptr=ptr->next)
    {
        struct HeapNode node={ptr->label,ptr->edge_len};
        A[ptr->label]=ptr->edge_len;
        insert(node);
    }
    //For All other Vertices, Insert them into the Heap with a 'key' value of Infinity
    for(i=0;i<N;i++)
    {
        if(i!=s && heap_pos[i]==-1)
        {
            struct HeapNode node={i,INFINITY};
            insert(node);
        }
    }

    //Explore one Vertex in each loop till all 'N' vetices are explored
    for(i=0;i<N-1;i++)
    {
        //Extract the Nearest Vertex from the Heap
        struct HeapNode min_node=extract_min();

        /*Update the 'key' attributes of all Vertices 'v' in the Heap
        to contain the minimum of
        1. key of 'v'
        2. key of 'min_node'+dist('min_node','v')
        */
        struct Vertex *ptr;
        for(ptr=V[min_node.vertex];ptr!=NULL;ptr=ptr->next)
        {
            if(heap_pos[ptr->label]!=NOT_IN_HEAP)
            {
                //Extract the vertex 'label' from the Heap
                struct HeapNode node=delete_At_pos(heap_pos[ptr->label]);

                /*Find the Shorter Path among the two options stated above
                and re-insert it into the Heap*/
                int path = A[min_node.vertex] + ptr->edge_len;
                if(path<node.key)
                {
                    node.key=path;
                    A[node.vertex]=path;
                }
                insert(node);
            }
        }
    }

    printf("Shortest Paths from Node %d to: \n",s+1);
    //Print the obtained Shortest Paths to the Required Nodes
    printf("Node %d: %d\nNode %d: %d\nNode %d: %d\nNode %d: %d\nNode %d: %d\nNode %d: %d\nNode %d: %d\nNode %d: %d\nNode %d: %d\nNode %d: %d\n",7,A[6],37,A[36],59,A[58],82,A[81],99,A[98],115,A[114],133,A[132],165,A[164],188,A[187],197,A[196]);

    return 0;
}
