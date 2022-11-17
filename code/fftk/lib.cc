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
int getNodeID(char *name)//Write Hash function and generate int as ouput.
{ 
	char *end_ptr;
	return (int)strtol(name+5,&end_ptr,10);
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
	nodeID = getNodeID(name);
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
	
	void * temp_buf=fftw_malloc(global_size*sendcount*datatype_size);
	memcpy(temp_buf,sendbuf,global_size*sendcount*datatype_size);
	if(global_rank!=my_leader)
	{
		char * send_buf=static_cast<char*>(temp_buf);
		MPI_Request request;		
		MPI_Isend(send_buf,global_size*sendcount*datatype_size,MPI_CHAR,my_leader,global_rank,comm,&request);
		MPI_Wait(&request,MPI_STATUS_IGNORE);

		MPI_Irecv(recvbuf,global_size*recvcount*datatype_size,MPI_CHAR,my_leader,global_rank,comm,&request);
		MPI_Wait(&request,MPI_STATUS_IGNORE);
		
	}
	else
	{
		char ** send_buf=(char**)fftw_malloc(sameHost_size*sizeof(char*));
		send_buf[0]=(char*)fftw_malloc(sameHost_size*global_size*sendcount*datatype_size*sizeof(char));
		
		for(int i=1;i<sameHost_size;i++)
		{
			send_buf[i]=&(send_buf[0][i*global_size*sendcount*datatype_size]);
		}
		int index=0;
		MPI_Request request[sameHost_size-1];
		for(int i=global_rank-sameHost_size+1;i<global_rank;i++)
		{
			MPI_Irecv(send_buf[index],global_size*sendcount*datatype_size,MPI_CHAR,i,i,comm,&request[index]);
			index++;
		}
		MPI_Waitall(sameHost_size-1,request,MPI_STATUS_IGNORE);
		send_buf[index]=static_cast<char*>(temp_buf);
		
		char *send_transpose=(char*) fftw_malloc (sameHost_size*sendcount*global_size*datatype_size*sizeof(char));
		index=0;
		for(int i=0;i<global_size;i++)
		{
			for(int j=0;j<sameHost_size;j++)
			{
				for(int k=0;k<sendcount*datatype_size;k++)
				{				
					send_transpose[index++]=send_buf[j][(sendcount*datatype_size*i)+k];
				}
			}
		}
		
		char * recv_buf=(char*) fftw_malloc (sameHost_size*recvcount*global_size*datatype_size*sizeof(char));
		
		int count_new=(sameHost_size*sendcount*global_size*datatype_size*sizeof(char))/leader_size;

		PMPI_Alltoall(send_transpose,count_new,MPI_CHAR,recv_buf,count_new,MPI_CHAR,leaderComm);


		MPI_Datatype newvtype;
		MPI_Type_vector(leader_size,sameHost_size*sendcount*datatype_size*sizeof(char),count_new,MPI_CHAR,&newvtype);
		MPI_Type_commit(&newvtype);
		index=0;
		for(int i=global_rank-sameHost_size+1;i<global_rank;i++)
		{
			MPI_Isend(recv_buf+(index*sameHost_size*sendcount*datatype_size*sizeof(char)),1, newvtype, i, i,comm,&request[index]);
			index++;
		}
		MPI_Waitall(sameHost_size-1,request,MPI_STATUS_IGNORE);
		MPI_Request check;
		MPI_Isend(recv_buf+(index*sameHost_size*sendcount*datatype_size*sizeof(char)),1, newvtype,global_rank,global_rank,comm,&check);
		MPI_Status status;
		MPI_Recv(recvbuf,global_size*recvcount*datatype_size,MPI_CHAR,global_rank,global_rank,comm,&status);
		MPI_Wait(&check,MPI_STATUS_IGNORE);
		MPI_Type_free (&newvtype);
		
	}
		
	return MPI_SUCCESS;
}

