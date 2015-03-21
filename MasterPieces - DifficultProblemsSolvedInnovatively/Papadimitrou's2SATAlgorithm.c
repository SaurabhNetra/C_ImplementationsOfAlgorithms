/*Programmed by: Saurabh Naresh Netravalkar
                 saurabh.netravalkar@gmail.com

This program Implements PAPADIMITROU's RANDOMIZED LOCAL SEARCH ALGORITHM to solve the 2SAT problem.

There are 6 different 2SAT instances: 2sat1.txt, 2sat2.txt, ..., 2sat6.txt.

The file format is as follows:
In each instance, the number of variables and the number of clauses is the same,
and this number is specified on the first line of the file.
Each subsequent line specifies a clause via its two literals, with a number denoting the variable
and a "-" sign denoting logical "not". For example, the second line of the first data file is "-16808 75250",
which indicates the clause ¬x16808 ∨ x75250.

The task is to determine which of the 6 instances are satisfiable, and which are unsatisfiable.

NOTE: There are Multiple Algorithms to Solve the 2SAT problem.
1. 2SAT reduces to computing the strongly connected components of a suitable graph (with two vertices
per variable and two directed edges per clause). Hence, I could have used my Implementation of Kosaraju's SCC Algorithm
to solve this Problem.
2. An approach via limited backtracking.
3. MY DECISION: I decided to use Papadimitriou's randomized local search algorithm, since I wanted to learn something new
and experience the power of randomized algorithms.

THE CHALLENGE: The Original Algorithm runs extremely slow, especially on our dataset containg lacs of clauses.
It runs in O(n^2*log(n)). For n = c*10^5, this algorithm proves to be very slow as compared to the blazingly fast SCC algorithm.

MY OPTIMIZATION:
My Main idea is PREPROCESSING. I USE the following SIMPLE RULE ITERATIVELY to preprocess information
and REDUCE the PROBLEM SIZE till no more reductions are possible in an iteration:
Rule: For All variables which need the same value to satisfy all of their conditions,
 - set them to the corresponding values and delete all conditions as already satisfied.

OBSERVATIONS:
After this preprocessing was done iteratively, problem size dropped to:
1. 6 variables for part 1, which originally had 1,00,000 Variables.
2. 54 variables for part 2, which originally had 2,00,000 Variables.
3. 288 variables for part 3, which originally had 3,00,000 Variables.
4. 11 variables for part 4, which originally had 4,00,000 Variables.
5. 98 variables for part 5, which originally had 5,00,000 Variables.
6. 25 variables for part 6, which originally had 6,00,000 Variables.

With such numbers O(n^2 * log(n)) is really not that expensive.

*/

//Include Files
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<stdbool.h>
#include<time.h>
#include<string.h>

//n -> The Number of Clauses
int n;

//Structure for a Clause
struct clause
{
    int x,y;
}*clause;

//Array of values of variables in our 2sat instance i.e. the solution array
bool *variable;
//Array To indicate whether the assignment of a particular variable is final and not to be changed
bool *isfinal;

//Function to check whether the clause 'i' is satisfied by the current assignment of its variables
bool is_satisfied(i)
{
    //'abs' is used since, the variable 'x' in the clause may be stored as '-x' to indicate logical not
    //'1' is subtracted for zero based array indexing
    int x = variable[abs(clause[i].x)-1],y = variable[abs(clause[i].y)-1];

    //negaative value indicates a logical not
    if(clause[i].x<0)
        x=!x;
    if(clause[i].y<0)
        y=!y;

    //Return the result of satisfiability
    return x || y;
}

/*Structure for implementing a Map of Variables to Clauses
i.e. Each Variable has a Linked List of Clauses of which it is part of*/
struct clause_list_node
{
    int index_of_clause;
    struct clause_list_node *next_list_node;
}**clauses;

//Function to Add a Clause to a Variable in the Map
void add_clause_to(int variable,int index_of_clause)
{
    struct clause_list_node *new_clause_list_node=malloc(sizeof(struct clause_list_node));
    new_clause_list_node->index_of_clause=index_of_clause;
    new_clause_list_node->next_list_node=clauses[variable];
    clauses[variable]=new_clause_list_node;
}

//Function to Display our Map data structure, for debugging
void display_map()
{
    printf("Map:\n");

    int i;
    for(i=0;i<n;i++)
    {
        printf("Variable %d\n",i+1);
        struct clause_list_node *ptr;
        for(ptr=clauses[i];ptr!=NULL;ptr=ptr->next_list_node)
            printf("( %d %d ) ",clause[ptr->index_of_clause].x,clause[ptr->index_of_clause].y);
        printf("\n");
    }

}

//Main Function
int main()
{
    //Open the Input File in Read Mode
    //There are 6 input files named 2sat1.txt - 2sat6.txt in the InputFiles folder
    FILE *fin=fopen("InputFiles\\2sat3.txt","r");
    if(!fin)
    {
        printf("Unable to Read from File");
        exit(0);
    }

    fscanf(fin,"%d",&n);
    printf("n = %d\n",n);

    //An Array to store the clauses
    clause = malloc(n * sizeof(struct clause));

    //Allocate memory to the solution array
    variable = malloc(n * sizeof(bool));
    isfinal = calloc(n, sizeof(bool));

    //An Array to store a Map of Variables to Clauses
    //Calloc is used since all pointers need to be initialized to NULL
    clauses = calloc(n,sizeof(struct clause_list_node *));

    //Read Clauses from file
    int i;
    for(i=0;i<n;i++)
    {
        //Read a clause and add it to our clause array
        fscanf(fin,"%d%d",&clause[i].x,&clause[i].y);
    }
    fclose(fin);

    /*To Print the Number of Clauses in which every variable 'x' participates
    either as itself i.e. 'x' or its inverted version i.e. 'not x'*/
    /*for(i=0;i<2;i++)
    {
        printf("Number of %s clauses:\n",i==true ? "True" : "False");
        int j;
        for(j=0;j<n;j++)
            printf("Variable %d: %d ",j+1,number_of_clauses[i][j]);
        printf("\n");
    }*/

    //--------Preprocessing Step----------------
    /*PREPROCESSING CONDITION: For all variables which need the same value to satisfy all of their conditions -
    set those variables to that value and delete all corresponding conditions as already satisfied
    */
    //To indicate the reduced problem size
    int reduced_n = n;
    int reduced_clause_len = n;

    //Flag to indicate whether the problem is further reducible
    bool reduce_more = true;

    //An Array to count the number of clauses each variable 'x' occurs in, and to indicate whether it occurs as 'x' or 'not x'
    int *number_of_clauses[2];
    number_of_clauses[false] = malloc(n * sizeof(int));
    number_of_clauses[true] = malloc(n * sizeof(int));

    //Iteratively Apply the Preprocessing Step till the problem can be reduced no more
    while(reduce_more)
    {
        reduce_more = false;

        //Initialize both arrays to 'zero' values
        memset(number_of_clauses[false],0,n*sizeof(int));
        memset(number_of_clauses[true],0,n*sizeof(int));

        //For all Clauses
        for(i=0;i<reduced_clause_len;i++)
        {
            //Increment the values of the ith clause's respective participating variables
            clause[i].x < 0 ? number_of_clauses[false][abs(clause[i].x)-1]++ : number_of_clauses[true][clause[i].x-1]++;
            clause[i].y < 0 ? number_of_clauses[false][abs(clause[i].y)-1]++ : number_of_clauses[true][clause[i].y-1]++;
        }

        /*Set the variables which satisfy our above Preprocessing Condition to their 'Final' value required to satisfy all their
        clauses*/
        //For all Variables
        for(i=0;i<n;i++)
        {
            //If this variable's assignment is already final, leave it as it is
            if(isfinal[i])
                continue;

            bool value;
            bool isValueSet = false;

            //If the variable 'i' has a positive sign in all its clauses, i.e. If there is no clause with 'not i'
            if(number_of_clauses[false][i]==0)
            {
                //Set 'i' to true
                value = true;
                isValueSet = true;
            }
            //If the variable 'i' has a negative sign in all its clauses, i.e. All clauses contain 'not i'
            else if(number_of_clauses[true][i]==0)
            {
                //Set 'i' to false
                value = false;
                isValueSet = true;
            }

            //If either of the 2 above conditions is satisfied, set the variable's final assignment value and remove all its clauses
            if(isValueSet)
            {
                variable[i] = value;
                isfinal[i] = true;
                reduced_n--;

                //Indicate that a reduction has occured in this iteration
                reduce_more = true;
            }
        }

        //Re-arrange the clause array to contain non-deleted clauses at successive indices for O(1) lookup later
        struct clause *temp_clause = malloc(reduced_clause_len * sizeof(struct clause));
        int temp_clause_len = 0;

        //For all Clauses
        for(i=0;i<reduced_clause_len;i++)
        {
            //If both variables of the clause have not been finally assigned, then keep that clause
            if(!isfinal[abs(clause[i].x)-1] && !isfinal[abs(clause[i].y)-1])
            {
                temp_clause[temp_clause_len] = clause[i];

                /*Add the clause in the map of both its participating variables
                Note: '1' is subtracted from variable values since C arrays use zero based indexing*/
                add_clause_to(abs(temp_clause[temp_clause_len].x)-1,temp_clause_len);
                add_clause_to(abs(temp_clause[temp_clause_len].y)-1,temp_clause_len);

                temp_clause_len++;
            }
        }
        reduced_clause_len = temp_clause_len;

        //display_map();

        //Set temp_clause to be our new reduced clause array
        free(clause);
        clause = temp_clause;
    }

    //Free Memory which is no more needed
    free(number_of_clauses[false]);
    free(number_of_clauses[true]);

    printf("Reduced Problem Size: %d Variables, %d Clauses\n",reduced_n,reduced_clause_len);

    //-----------Papadimitrou's Algorithm Begins--------------

    //Initialize the Random Number Generator
    srand(time(NULL));

    //Array to store indices of clauses that are not satisfied
    int *not_satisfied = malloc(reduced_clause_len * sizeof(int));
    int not_satisfied_len;

    int log2n = log2(reduced_n);
    int two_n_squared = 2*reduced_n*reduced_n;
    long long rep2;
    int rep;
    int j;
    //Iterate log2n times
    for(rep=0;rep<log2n;rep++)
    {
        //Choose a Random initial Assignment
        //printf("Random Initial Assignment\n");
        for(i=0;i<n;i++)
        {
            if(!isfinal[i])
                variable[i] = rand() % 2;
            //printf("%d=%s ",i+1,variable[i]?"True":"False");
        }
        //printf("\n");

        //Repeat 2*n^2 times
        for(rep2=0;rep2<two_n_squared;rep2++)
        {
            //Check whether all Clauses are satisfied
            //Store indices of unsatisfied clauses in 'not_satisfied' array
            //printf("Clauses Not satisfied:\n");
            for(j=0,not_satisfied_len=0;j<reduced_clause_len;j++)
            {
                if(!is_satisfied(j))
                {
                    not_satisfied[not_satisfied_len++] = j;
                    //printf("(%d %d) ",clause[j].x,clause[j].y);
                }
            }
            //printf("\n");

            //If current Assignment satisfies all clauses, halt and report the Answer
            if(not_satisfied_len == 0)
            {
                printf("The 2SAT instance is Satisfiable\n");
                /*Print the Solution Variable Assignment Values
                for(j=0;j<n;j++)
                    printf("%d=%s ",j+1,variable[j]?"True":"False");*/
                return 0;
            }

            //Pick an Arbitrary unsatisfied clause
            int rand__int = rand() % not_satisfied_len;

            //Choose one of its variables uniformly at random
            int variable_to_be_flipped = ((rand() % 2) ? abs(clause[not_satisfied[rand__int]].y)-1 : abs(clause[not_satisfied[rand__int]].x)-1);

            //Flip the variable's value
            variable[variable_to_be_flipped] = !variable[variable_to_be_flipped];
        }
    }

    printf("The 2SAT Instance is Unsatisfiable\n");
    return 0;
}
