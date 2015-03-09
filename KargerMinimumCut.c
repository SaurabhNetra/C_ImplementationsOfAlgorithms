/*Programmed by: Saurabh Naresh Netravalkar
                 saurabh.netravalkar@gmail.com

This Program uses the Karger's Random Edge Contraction Algorithm to Compute
the Minimum Cut of a Graph in O(n^3*log(n)*m) time,
where n-> Number of Vertices and m-> Number of Edges*/

//Include Files
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<limits.h>
#include<stdbool.h>
#include<math.h>

//The Number of Vertices
#define N 200

//Structure for Storing Edges of a Graph as a Linked List
struct edge
{
    int u,v;
    struct edge *next;
};

int main()
{
    //Open the Input File in Read Mode
    FILE *f_in=fopen("res\\kargerMinCut.txt","r");

    //Create an Empty Linked List of Edges
    struct edge *E=NULL;
    int m=0;

    //A Buffer for reading string representations of upto 3-digit integers
    char buf[4];
    int buf_ind=0;

    int first_column=true;

    int u;

    /*Read from the Input File character by Character and Extract Edges of the Graph
    The file contains the adjacency list representation of a simple undirected graph.
    There are 200 vertices labeled 1 to 200. The first column in the file represents
    the vertex label, and the particular row (other entries except the first column)
    tells all the vertices that the vertex is adjacent to.*/
    char c;
    while((c=getc(f_in))!=EOF)
    {
        switch(c)
        {
            case '\n':
                first_column=true;
                break;
            case '\t':
                buf[buf_ind]='\0';
                int buf_int=atoi(buf);
                buf_ind=0;
                if(first_column)
                {
                    u=buf_int;
                    first_column=false;
                }
                else
                {
                    int v=buf_int;
                    if(u<v)
                    {
                        //Insert the Extracted Edge into the Linked List 'E'
                        struct edge *edge_new=(struct edge *)malloc(sizeof(struct edge));
                        edge_new->u=u;
                        edge_new->v=v;
                        edge_new->next=E;
                        E=edge_new;
                        m++;
                    }
                }
                break;
            default:
                buf[buf_ind++]=c;
                break;
        }
    }
    fclose(f_in);

    //Stores a Copy of the Edge List to be operated upon in every trial
    struct edge *E_copy=NULL;
    int m_copy=0;

    srand(time(NULL));

    //Initialize the Minimm Cut value to be Infinity
    int min_cut=INT_MAX;

    //Perform n^2*log(n) trials which brings down the Failure Probability of the Algorithm to 1/n
    int trial_number;
    for(trial_number=1;trial_number<=N*N*log(N);trial_number++)
    {
        //Clear the Edge Copy Linked List
        while(E_copy!=NULL)
        {
            struct edge *edge_old=E_copy;
            E_copy=E_copy->next;
            edge_old->next=NULL;
            free(edge_old);
        }

        //Copy the Edge List 'E' into 'E_copy'
        struct edge *ptr;
        for(ptr=E;ptr!=NULL;ptr=ptr->next)
        {
            struct edge *edge_new=(struct edge *)malloc(sizeof(struct edge));
            edge_new->u=ptr->u;
            edge_new->v=ptr->v;
            edge_new->next=E_copy;
            E_copy=edge_new;
        }
        m_copy=m;

        /*Repeatedly contract an edge in every iteration till 2 vertices remain
        i.e. N-2 iterations*/
        int counter;
        for(counter=1;counter<=N-2;counter++)
        {
            //Pick a Random edge
            int rand_ind = rand()%m_copy;
            struct edge *edge_picked=E_copy;
            while(rand_ind>0)
            {
                edge_picked=edge_picked->next;
                rand_ind--;
            }

            //Vertices of the Random Edge
            int u_picked=edge_picked->u,v_picked=edge_picked->v;

            /*Contract the Picked Edge
            i.e. Replace every occurence of 'v_picked' with 'u_picked'
            and delete self-looping edges*/
            ptr=E_copy;
            struct edge *edge_previous=NULL;
            while(ptr!=NULL)
            {
                if(ptr->u==v_picked)
                    ptr->u=u_picked;
                else if(ptr->v==v_picked)
                    ptr->v=u_picked;
                if(ptr->u==ptr->v)
                {
                    if(edge_previous!=NULL)
                        edge_previous->next=ptr->next;
                    else
                        E_copy=E_copy->next;
                    struct edge *edge_old=ptr;
                    ptr=ptr->next;
                    edge_old->next=NULL;
                    free(edge_old);
                    m_copy--;
                }
                else
                {
                    edge_previous=ptr;
                    ptr=ptr->next;
                }
            }
        }

        /*If the Computed Minimum Cut in this Trial is Better than
        the Minimum Obtained so far, then stor it as the new minimum*/
        if(m_copy<min_cut)
            min_cut=m_copy;
    }

    //Print the Minimum Cut Obtained
    printf("\nMinimum Cut: %d\n",min_cut);
    return 0;
}
