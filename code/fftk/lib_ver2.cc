#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "mpi.h"
#include<fftw3.h>
#include<blitz/array.h>
#include<cstring>
int *rankMatrix,*rankData;
int global_rank,global_size,my_leader,leader_size;
MPI_Comm sameNodeComm,leaderComm;
int sameHost_rank;
int sameHost_size;
char *send_buf;
int flag=0;
char *recv_buf;
int flag1=0;
double s_time,e_time;
int getNodeID(char *name,int len)   
{
	int ans=0;
	int p = 31;
	int m = 1e9 + 9;
	long long power_of_p = 1;
	for(int i=0;name[i]!='.'&&i<len;i++) 
	{
		 ans=ans+(name[i])*power_of_p;
		 power_of_p= (power_of_p * p) % m;
	}
	return ans%1007;	
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
	MPI_Allreduce(&global_rank, &leader, 1, MPI_INT, MPI_MAX,sameNodeComm);	
	my_leader=leader;
	rankMatrix = (int*)malloc(sizeof(int)*2*global_size);
    	rankData = (int*)malloc(sizeof(int)*2);
	rankData[0]=global_rank;
	rankData[1]=leader;
	
	MPI_Allgather(rankData, 2, MPI_INT, rankMatrix, 2, MPI_INT, MPI_COMM_WORLD);

	if(global_rank==my_leader)
	{
		MPI_Comm_split(MPI_COMM_WORLD, 0 , global_rank, &leaderComm);
		MPI_Comm_size(leaderComm,&leader_size);
	}
	else
	{
		MPI_Comm_split(MPI_COMM_WORLD, MPI_UNDEFINED, global_rank, &leaderComm);
	}
	return ret;
}



int MPI_Alltoall(const void *sendbuf, int sendcount, MPI_Datatype sendtype,void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm)
{
	
	int datatype_size;
	MPI_Type_size(sendtype,&datatype_size);
	
	if(global_rank!=my_leader)
	{
		
		MPI_Request request;		
		MPI_Isend(sendbuf,global_size*sendcount*datatype_size,MPI_BYTE,sameHost_size-1,sameHost_rank,sameNodeComm,&request);
		MPI_Wait(&request,MPI_STATUS_IGNORE);

		MPI_Irecv(recvbuf,global_size*recvcount*datatype_size,MPI_BYTE,sameHost_size-1,MPI_ANY_TAG,sameNodeComm,&request);
		MPI_Wait(&request,MPI_STATUS_IGNORE);
		
	}
	else
	{

		if(flag==0)
		{
			send_buf=(char*)malloc(global_size*sameHost_size*sendcount*datatype_size*sizeof(char));
			flag=1;
		}
		MPI_Datatype newvtype;
		MPI_Type_vector(global_size,sendcount*datatype_size*sizeof(char),sendcount*sameHost_size*datatype_size*sizeof(char),MPI_CHAR,&newvtype);
		MPI_Type_commit(&newvtype);

		int index=0;
		MPI_Request request[sameHost_size-1];
		for(int i=0;i<sameHost_size-1;i++)
		{
			MPI_Irecv(send_buf+(index*sendcount*datatype_size*sizeof(char)),1,newvtype,i,MPI_ANY_TAG,sameNodeComm,&request[index]);
			index++;
		}
		MPI_Waitall(sameHost_size-1,request,MPI_STATUS_IGNORE);

		MPI_Request check;
		MPI_Isend(sendbuf,global_size*sendcount*datatype_size,MPI_BYTE,0,global_rank,MPI_COMM_SELF,&check);
		MPI_Status status;
		MPI_Recv(send_buf+(index*sendcount*datatype_size*sizeof(char)),1,newvtype,0,global_rank,MPI_COMM_SELF,&status);
		MPI_Wait(&check,MPI_STATUS_IGNORE);

		if(flag1==0)
		{
			recv_buf=(char*) malloc (sameHost_size*recvcount*global_size*datatype_size*sizeof(char));
			flag1=1;
		}
		
		int count_new=(sameHost_size*sendcount*global_size*datatype_size*sizeof(char))/leader_size;

		PMPI_Alltoall(send_buf,count_new,MPI_BYTE,recv_buf,count_new,MPI_BYTE,leaderComm);


		MPI_Datatype newvtype2;
		MPI_Type_vector(leader_size,sameHost_size*sendcount*datatype_size*sizeof(char),count_new,MPI_CHAR,&newvtype2);
		MPI_Type_commit(&newvtype2);
		index=0;
		for(int i=0;i<sameHost_size-1;i++)
		{
			MPI_Isend(recv_buf+(index*sameHost_size*sendcount*datatype_size*sizeof(char)),1, newvtype2, i, i,sameNodeComm,&request[index]);
			index++;
		}
		MPI_Waitall(sameHost_size-1,request,MPI_STATUS_IGNORE);
		
		MPI_Isend(recv_buf+(index*sameHost_size*sendcount*datatype_size*sizeof(char)),1, newvtype2,0,global_rank,MPI_COMM_SELF,&check);
		
		MPI_Recv(recvbuf,global_size*recvcount*datatype_size,MPI_BYTE,0,global_rank,MPI_COMM_SELF,&status);
		MPI_Wait(&check,MPI_STATUS_IGNORE);
		MPI_Type_free (&newvtype);
		MPI_Type_free (&newvtype2);
	
		
		
	}
		
	return MPI_SUCCESS;
}
                                                                                                                                                                                       }      
