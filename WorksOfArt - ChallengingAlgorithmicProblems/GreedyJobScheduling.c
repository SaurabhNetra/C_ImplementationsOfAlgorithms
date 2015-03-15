/*Programmed by: Saurabh Naresh Netravalkar
                 saurabh.netravalkar@gmail.com

This program problem demonstrates greedy algorithms for minimizing
the weighted sum of job completion times. The input file describes a
set of jobs with positive and integral weights and lengths. It has the format

[number_of_jobs]
[job_1_weight] [job_1_length]
[job_2_weight] [job_2_length]
...

For example, the third line of the file is "74 59", indicating that the second job
has weight 74 and length 59.

The task in this problem is to run the greedy algorithm that schedules jobs:

1. In decreasing order of the difference (weight - length)
Note: This algorithm is not always optimal

2. In decreasing order of the ratio (weight/length)
Note: This Algorithm is Optimal
*/

//Include Files
#include<stdio.h>
#include<stdlib.h>

//Structure for a Job entry
struct job{
    int weight;
    int length;
};

//A Comparision Function for Comparing two Jobs using 'weight - length' as the comparision key
int cmpjob1(const void *a,const void *b)
{
    struct job *item1=(struct job *)a,*item2=(struct job *)b;

    int val1=item1->weight-item1->length,val2=item2->weight-item2->length;

    if(val1==val2)
        return item2->weight-item1->weight;
    else
        return val2-val1;
}

//A Comparision Function for Comparing two Jobs using the ratio 'weight/length' as the comparision key
int cmpjob2(const void *a,const void *b)
{
    struct job *item1=(struct job *)a,*item2=(struct job *)b;

    float val1=(float)item1->weight/item1->length,val2=(float)item2->weight/item2->length;

    return val2>val1?1:-1;
}

/*A Function to calculate the Sum of Weighted Completion times of all Jobs
Parameters: job -> Array of Jobs n -> Number of Jobs in the Array
Note: The Jobs are assumed to have been executed in the order in which they appear in the Array*/
long long weighted_completion_time(struct job *job,int n)
{
    //Initialize the Completion Time and Sum of Weighted Completion Times to zero
    int c=0;
    long long sum=0;

    //Process jobs one by one and update the completion times and sum variables
    int i;
    for(i=0;i<n;i++)
    {
        c+=job[i].length;
        sum+=job[i].weight*c;
    }

    //Return the Total sum of weigthed completion times
    return sum;
}


//Main Function
int main()
{
    //Open Input File in Read Mode
    FILE *fin=fopen("InputFiles\\jobs.txt","r");

    int n;
    fscanf(fin,"%d",&n);

    struct job *job=malloc(n*sizeof(struct job));

    //Extract the jobs and store them in the job array
    int i;
    for(i=0;i<n;i++)
        fscanf(fin,"%d%d",&job[i].weight,&job[i].length);
    fclose(fin);

    printf("Total Sum of Weighted Completion Times for our two Job Scheduling Algorithms:\n");

    //Part 1: Sort jobs in decreasing order of 'weight - length'
    qsort(job,n,sizeof(struct job),cmpjob1);

    //Calculate the Sum of Weighted completion times and print the result
    printf("Decreasing Order of 'weight - length': %lld\n",weighted_completion_time(job,n));

    //Part 2: Sort jobs in decreasing order of 'weight/length'
    qsort(job,n,sizeof(struct job),cmpjob2);

    //Calculate the Sum of Weighted completion times and print the result
    printf("Decreasing Order of 'weight/length' : %lld\n",weighted_completion_time(job,n));

    return 0;
}
