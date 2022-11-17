#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "mpi.h"
#include<fftw3.h>
#include<blitz/array.h>
#include<cstring>
int global_rank,global_size,my_leader,leader_size;
MPI_Comm sameNodeComm,leaderComm;
int sameHost_rank;
int sameHost_size;
MPI_Win window;
char*send_buf;
char*recv_buf;
int flag;
MPI_Datatype newvtype;
MPI_Datatype newvtype2;

int getNodeID(char * name,int len)
{
    int ans=0;
    int p=31;
    int m=1e9+9;
    long long power_of_p=1;
        
    for(int i=0;name[i]!='.'&&i<len;i++)
    {
        ans=ans+(name[i])*power_of_p;
        power_of_p=(power_of_p*p)%m;
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
        flag=0;
	return ret;
}      

int MPI_Alltoall(const void *sendbuf, int sendcount, MPI_Datatype sendtype,void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm)
{

	
	int datatype_size;
	MPI_Type_size(sendtype,&datatype_size);
	if(global_rank!=my_leader)
	{
	      
                
        if(!flag)
        {
            MPI_Win_create(NULL,0,1,MPI_INFO_NULL,sameNodeComm,&window);
            MPI_Type_vector(global_size,sendcount*datatype_size*sizeof(char),sendcount*sameHost_size*datatype_size*sizeof(char),MPI_CHAR,&newvtype);
            MPI_Type_commit(&newvtype);
            flag=1;
        }
        MPI_Win_fence(0,window);
                
        unsigned long long s=global_size*sendcount*datatype_size;
                 
        MPI_Put(sendbuf,s,MPI_BYTE,sameHost_size-1,sendcount*datatype_size*sameHost_rank,1,newvtype,window);

        MPI_Win_fence(0,window);
                
        MPI_Request request;
        MPI_Irecv(recvbuf,s,MPI_BYTE,sameHost_size-1,MPI_ANY_TAG,sameNodeComm,&request);
        MPI_Wait(&request,MPI_STATUS_IGNORE);        
                
	}
	else
	{
        unsigned long long send_size=global_size;
        send_size*=sendcount;
        send_size*=datatype_size;
        send_size*=sameHost_size;
                
        unsigned long long  count_new=(send_size*sizeof(char))/leader_size;
                
        if(!flag)
        {
            send_buf=(char*)malloc(send_size*sizeof(char));
            recv_buf=(char*)malloc(send_size*sizeof(char));
            MPI_Win_create(send_buf,send_size,1,MPI_INFO_NULL,sameNodeComm,&window);
                       
            MPI_Type_vector(leader_size,sameHost_size*sendcount*datatype_size*sizeof(char),count_new,MPI_CHAR,&newvtype2);
            MPI_Type_commit(&newvtype2);   
            flag=1;
        }
        MPI_Win_fence(0,window);

        MPI_Win_fence(0, window);
        PMPI_Alltoall(send_buf,count_new,MPI_BYTE,recv_buf,count_new,MPI_BYTE,leaderComm);

        int index=0;
        MPI_Request request[sameHost_size-1];
        for(int i=0;i<sameHost_size-1;i++)
        {
            MPI_Isend(recv_buf+(index*sameHost_size*sendcount*datatype_size*sizeof(char)),1, newvtype2, i, i,sameNodeComm,&request[index]);
            index++;
        }
        MPI_Waitall(sameHost_size-1,request,MPI_STATUS_IGNORE);

	}
     		
	return MPI_SUCCESS;
}

