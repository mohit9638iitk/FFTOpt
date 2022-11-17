#include<mpi.h>
#include<fftw3.h>
#include<fftw3-mpi.h>
#include<math.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
int global_rank,global_size,my_leader;
MPI_Comm sameNodeComm,leaderComm;
int sameHost_rank,sameHost_size;
MPI_Request* non_leader_request1;
MPI_Request* non_leader_request2;
MPI_Request* leader_request;
MPI_Request* same_request;
int helper,count;
double* send_buf;
double* recv_buf;
int flag;
int leader_size;
int position;
int limit;

int getNodeID(char *name,int len)//hash function is written to convert string(name of host) to unique int value
{
   int ans=0;
   int p = 31;
   int m = 1e9 + 9;
   long long power_of_p = 1;
   int i;

   for( i=0;name[i]!='.'&&i<len;i++)
   {
      ans=ans+(name[i])*power_of_p;
      power_of_p= (power_of_p * p) % m;
   }
   return ans%10007;

}

int  MPI_Init(int *argc,char **argv[])
{
	
   int ret=PMPI_Init(argc,argv);
   int len;
   char name[MPI_MAX_PROCESSOR_NAME];
   int nodeID;
   MPI_Comm_rank(MPI_COMM_WORLD, &global_rank);
   MPI_Comm_size(MPI_COMM_WORLD, &global_size);
	
   MPI_Get_processor_name(name, &len);
   nodeID = getNodeID(name,len);
   

   MPI_Comm_split(MPI_COMM_WORLD, nodeID , global_rank, &sameNodeComm);
   MPI_Comm_rank(sameNodeComm, &sameHost_rank);
   MPI_Comm_size(sameNodeComm,&sameHost_size);

   int leader;
   leader=global_rank/sameHost_size;
   leader*=sameHost_size;
   leader+=(sameHost_size-1);
   my_leader=leader;

   
   if(global_rank==my_leader)
   {
        MPI_Comm_split(MPI_COMM_WORLD, 0 , global_rank, &leaderComm);
        MPI_Comm_size(leaderComm,&leader_size);
   }
   else
   {
        MPI_Comm_split(MPI_COMM_WORLD, MPI_UNDEFINED, global_rank, &leaderComm);
   }

 
   non_leader_request1=(MPI_Request*)malloc(sizeof(MPI_Request)*(global_size-1));
   non_leader_request2=(MPI_Request*)malloc(sizeof(MPI_Request)*(global_size-1));
   limit=(global_size-sameHost_size)*(sameHost_size-1);
   limit+=(sameHost_size-1);
   limit+=((sameHost_size-1)*(sameHost_size-2));
   leader_request=(MPI_Request*)malloc(sizeof(MPI_Request)*(limit));
   same_request=(MPI_Request*)malloc(sizeof(MPI_Request)*(global_size-1));
     
   helper=0;
   flag=0;
   count=0;
   position=0;
   send_buf=NULL;
   recv_buf=NULL;
   return ret;
}

int MPI_Sendrecv(const void* buffer_send,int count_send,MPI_Datatype datatype_send,int recipient,int tag_send,void* buffer_recv,int count_recv,MPI_Datatype datatype_recv,int sender,int tag_recv,MPI_Comm communicator,MPI_Status* status)
{
   /*Non-leader process ,if its rank is not equal to my_leader(leader on that node or host).
     1.Non-leader process will send the data to leader process by MPI_Isend().
     2.Non-leader process will recieve the data from leader process by MPI_Irecv().

   Every non-leader process will send  the data to  every other process and vice-versa.
   Number of times MPI_Isend() call = global_size-1.
   Number of times MPI_Irecv() call = global_size-1.*/
   if(global_rank!=my_leader)
   {
      MPI_Isend(buffer_send,count_send,MPI_DOUBLE,my_leader,recipient,MPI_COMM_WORLD,&non_leader_request1[count]);
      count++;
      MPI_Irecv(buffer_recv,count_recv,MPI_DOUBLE,my_leader,recipient,MPI_COMM_WORLD,&non_leader_request2[position]);
      position++;
      if(count==((global_size-1)))
      {
         MPI_Waitall(count,non_leader_request1,MPI_STATUS_IGNORE);
         MPI_Waitall(position,non_leader_request2,MPI_STATUS_IGNORE);
         count=0;
         position=0;
      }
   }
   else//Leader Process
   {
      /*
      Leader process will have a buffer where it will store all the data recieved from non-leader processes and also its data.
      
      size of buffer = (data send by one process )* number of process lying on same node or host.
      count_send, denotes the count of element send to one process.
      So, count_send*global_size ,denotes the amount of element send from one process.
      
      Leader process has to store the data for sameHost_size ,number of processes.

      Therefore, size of buffer = count_send*global_size*sameHost_size.*/
      unsigned long long send_size=sameHost_size;
      send_size*=count_send;
      send_size*=global_size;
      
      if(!flag)
      {
         send_buf=(double*)malloc(send_size*sizeof(double));
         recv_buf=(double*)malloc(send_size*sizeof(double));
         flag=1;
      }
             
      /*
      count=0 dentoes the first MPI_SendRecv() call by leader process.
      At this time, leader process will call MPI_Irecv() for recieving the data from the non-leader process .

      leader process will store the data in send_buf.
      index*count_send, denotes the place from where the data from a particular rank j, for a particular rank i will store .

      i!=j, help in checking sender and reciever is not same.*/
      if(count==0)
      {
         int tag=0;
         int index=0;
                 
         for(int i=0;i<global_size;i++)
         {
            for(int j=global_rank-sameHost_size+1;j<global_rank;j++)
            {
               if(i!=j)
               {
                  MPI_Irecv(send_buf+index*count_send,count_send,MPI_DOUBLE,j,i,MPI_COMM_WORLD,&leader_request[tag]);
                  tag++;  
               }
               index++;
            }
            index++;//here only index is incremented not MPI_Irecv() is called, bcz leader process will do memcpy for the data.  
         }
                  
      }

      unsigned long long stride=recipient*count_send;
      stride*=sameHost_size;
             
      unsigned long long displ=count_send;
      displ*=sameHost_size-1;
      /*stride + displ denotes the position at where leader process will copy its data in send_buf.*/ 
      memcpy(send_buf+displ+stride,buffer_send,count_send*sizeof(double));
             
      //for recieving the data after data-exchange happens with other leader process.       
      MPI_Irecv(buffer_recv,count_recv,MPI_DOUBLE,my_leader,recipient,MPI_COMM_WORLD,&same_request[count]);
      count++;
      /*Leader process will call MPI_Sendrecv() , global_size-1 time.
      Here, leader process will wait till it recieved the data from non-leader processes.

      After data is fully recieved, data is exchanged by MPI_Alltoall(), with send_buf as send buffer and recv_buf as recieve buffer.
      */
      if(count==(global_size-1))
      {
         
         MPI_Waitall(limit,leader_request,MPI_STATUS_IGNORE);           
         unsigned long long each_share=send_size/leader_size;
         MPI_Alltoall(send_buf,each_share,MPI_DOUBLE,recv_buf,each_share,MPI_DOUBLE,leaderComm);   
         
         /*
         After exchanging the data , data is transfered back to leader process by using MPI_Isend().

         1. MPI_Send() is used to send the data to itself , by leader process.
         2. MPI_Isend() is used to send the data for non-leader process.
         */
         int tag=0;
         int index=0;
         for(int i=0;i<global_size;i++)
         {
            for(int j=global_rank-sameHost_size+1;j<=global_rank;j++)
            {
               if(i!=j)
               {
                  if(j==global_rank)
                  {
                     MPI_Send(recv_buf+index*count_send,count_send,MPI_DOUBLE,j,i,MPI_COMM_WORLD);
                  }
                  else
                  {
                     MPI_Isend(recv_buf+index*count_send,count_send,MPI_DOUBLE,j,i,MPI_COMM_WORLD,&leader_request[tag]);
                     tag++;
                  }
               }
               index++;
            } 
         }
         
         MPI_Waitall(limit,leader_request,MPI_STATUS_IGNORE);
         MPI_Waitall(count,same_request,MPI_STATUS_IGNORE);
         count=0;
      }
   }
   return MPI_SUCCESS;
}
