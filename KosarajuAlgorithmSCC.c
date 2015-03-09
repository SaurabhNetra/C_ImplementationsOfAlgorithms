/*Programmed by: Saurabh Naresh Netravalkar
                 saurabh.netravalkar@gmail.com

This Program uses the Kosaraju's Two-Pass Algorithm to Compute
the Strongly Connected Components(SCCs) of a Graph in O(m+n) time,
where n-> Number of Vertices and m-> Number of Edges

Challenge Involved: The Input Graph is so big that the Program Stack Overflows whilst
recursively calling the Depth First Search(DFS) Function.
Solution: I implemented an Iterative Version of DFS*/

//Include Files
#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>

//Number of Vertices in the Input Graph
#define N 875714

//The List of Edges
struct Edge
{
    int v[2];
    struct Edge *next;
}*E;

//The list of Edge Pointers for every vertex
//V[1] is for the Normal Graph and V[0] is for the Reverse Graph
struct Node
{
    struct Edge *edge;
    struct Node *next;
}*V[2][N];

/*Inserts a Node At the Beginning of the List of Edge Pointers
and Returns a Ponter to the Head of the list*/
struct Node * insertNode(struct Node *ptr,struct Edge *edge)
{
    struct Node *pnew=(struct Node *)malloc(sizeof(struct Node));
    pnew->edge=edge;
    pnew->next=ptr;
    return pnew;
}

//Insert At Begin Function for the List of Edges
void insertEdge(int u,int v)
{
    struct Edge *pnew=(struct Edge *)malloc(sizeof(struct Edge));
    pnew->v[0]=u;
    pnew->v[1]=v;
    pnew->next=E;
    E=pnew;
};

//Global Variables as stated in the Algorithm
int t;
int s;
bool explored[N];
int f[N];
int leader[N];

//For the order of traversal of the Nodes in DFS_Loop
int order[N];

//Stack as a List
struct Item
{
    int i;
    struct Item *next;
}*Stack;

int peek()
{
    return Stack->i;
}

void push(int i)
{
    struct Item *pnew=(struct Item*)malloc(sizeof(struct Item));
    pnew->i=i;
    pnew->next=Stack;
    Stack=pnew;
}

int pop()
{
    struct Item *pold=Stack;
    int i=pold->i;
    Stack=Stack->next;
    free(pold);
    return i;
};

//Function Prototypes
/*Iterative Version of DFS
Parameters: 'g'->Indicates whether in Pass1 or Pass2 'i'->Source Vertex
*/
void DFS(bool,int);

/*DFS_Loop Function which Iterates over unexplored vertices in a given order
and calls the DFS function on each of them
Parameters: 'g'->Indicates whether in Pass1 or Pass2
*/
void DFS_Loop(bool);

//Main Function
int main()
{
    //Open the Input File in Read Mode
    FILE *fin=fopen("SCC.txt","r");

    int u,v;
    int i,j;

    /*Read from file and formation of the Adjacency Lists
    Forming Both the Forward as well as the Reverse Graphs for the 2 Passes
    of the Algorithm Respectively*/
    while(fscanf(fin,"%d%d",&u,&v)!=-1)
    {
        u--;
        v--;
        insertEdge(u,v);

        V[1][u]=insertNode(V[1][u],E);
        V[0][v]=insertNode(V[0][v],E);
    }

    /*Setting Order for Pass 1
    i.e. from vertex labelled N-1 to 0*/
    for(i=0;i<N;i++)
        order[i]=N-1-i;

    //Pass 1
    DFS_Loop(false);

    /*Setting order for Pass 2
    i.e in decreasing order of completion times of Pass1*/
    for(i=0;i<N;i++)
    {
        order[N-f[i]]=i;
        explored[i]=false;
    }

    //Pass 2
    DFS_Loop(true);

    for(i=0;i<N;i++)
    {
        f[i]=0;
        explored[i]=false;
    }

    //Storing Length of SCC's in f array at the position of their leader
    j=0;
    for(i=0;i<N;i++)
        f[leader[i]]++;

    //Selecting and printing the Best 5 SCCs
    printf("Length of the Best 5 SCCs:\n");
    int top[5]={0,0,0,0,0};
    for(i=0;i<N;i++)
    {
        if(f[i]>top[4])
        {
            top[4]=f[i];
            for(j=3;j>=0 && top[j]<top[j+1];j--)
            {
                int temp=top[j];
                top[j]=top[j+1];
                top[j+1]=temp;
            }
        }
    }
    for(i=0;i<5;i++)
        printf("%d ",top[i]);

    return 0;
}

/*DFS_Loop Function which Iterates over unexplored vertices in a given order
and calls the DFS function on each of them
Parameters: 'g'->Indicates whether in Pass1 or Pass2
*/
void DFS_Loop(bool g)
{
    int i;

    for(i=0;i<N;i++)
    {
        if(!explored[order[i]])
        {
            s=order[i];
            DFS(g,order[i]);
        }
    }
}

/*Iterative Version of DFS
Parameters: 'g'->Indicates whether in Pass1 or Pass2 'i'->Source Vertex
*/
void DFS(bool g,int i)
{
    struct Node *Vptr;
    int j;
    bool recurse;

    push(i);
    while(Stack)
    {
        //The Vertex to be Explored
        i=peek();

        recurse=false;

        //Mark it as Explored
        explored[i]=true;

        //Set its leader
        leader[i]=s;

        /*Check for an unexplored connected Vertex and if one exists,
        Push it onto Stack and go to beginning of the loop*/
        Vptr=V[g][i];
        while(Vptr!=NULL)
        {
            j=Vptr->edge->v[g];
            if(!explored[j])
            {
                push(j);
                recurse=true;
                break;
            }
            Vptr=Vptr->next;
        }
        if(recurse)
            continue;

        /*Since All Vertices having depth greater than 'i' have been explored,
        remove 'i' from stack*/
        i=pop();

        //Store its Finishing Time
        t++;
        f[i]=t;
    }
}
