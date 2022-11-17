#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "mpi.h"
#include<fftw3.h>

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
	
	
	void * temp_buf=fftw_malloc(global_size*sendcount*sizeof(MPI_DOUBLE));
	memcpy(temp_buf,sendbuf,global_size*sendcount*sizeof(MPI_DOUBLE));
	


	if(global_rank!=my_leader)
	{
		double * send_buf=static_cast<double*>(temp_buf);
		MPI_Request request;
		MPI_Isend(send_buf,global_size*sendcount,MPI_DOUBLE,my_leader,global_rank,comm,&request);
		MPI_Wait(&request,MPI_STATUS_IGNORE);
		
		
		MPI_Irecv(recvbuf,global_size*recvcount,MPI_DOUBLE,my_leader,global_rank,comm,&request);
		MPI_Wait(&request,MPI_STATUS_IGNORE);
		
		/*double *temp_recv=static_cast<double*>(recvbuf);//For debug purpose
		if(global_rank==10)
		{
			for(int i=0;i<global_size*recvcount;i++)
			{
				printf("%lf ",temp_recv[i]);
			}
			printf("\n");
		}*/
		
	}
	else
	{
		double ** send_buf=(double**)malloc(sameHost_size*sizeof(double*));
		send_buf[0]=(double*)malloc(sameHost_size*global_size*sendcount*sizeof(double*));
		
		for(int i=1;i<sameHost_size;i++)
		{
			send_buf[i]=&(send_buf[0][i*global_size*sendcount]);
		}
	
		int index=0;
		MPI_Request request[sameHost_size-1];
		for(int i=global_rank-sameHost_size+1;i<global_rank;i++)
		{
			MPI_Irecv(send_buf[index],global_size*sendcount,MPI_DOUBLE,i,i,comm,&request[index]);
			index++;
		}
		MPI_Waitall(sameHost_size-1,request,MPI_STATUS_IGNORE);
		
		send_buf[index]=static_cast<double*>(temp_buf);
		
		
		double *send_transpose=(double*) malloc (sameHost_size*sendcount*global_size*sizeof(double));
		index=0;
		for(int i=0;i<global_size;i++)
		{
			for(int j=0;j<sameHost_size;j++)
			{
				for(int k=0;k<sendcount;k++)
				{				
					send_transpose[index++]=send_buf[j][sendcount*i+k];
				}
			}
		}
		

		double * recv_buf=(double*) malloc (sameHost_size*recvcount*global_size*sizeof(double));
		
		int count_new=(sameHost_size*sendcount*global_size)/leader_size;

		PMPI_Alltoall(send_transpose,count_new,MPI_DOUBLE,recv_buf,count_new,MPI_DOUBLE,leaderComm);

		
		MPI_Datatype newvtype;
		MPI_Type_vector(leader_size,sameHost_size*sendcount,count_new,MPI_DOUBLE,&newvtype);
		MPI_Type_commit(&newvtype);
		index=0;
		for(int i=global_rank-sameHost_size+1;i<global_rank;i++)
		{
			MPI_Isend(recv_buf+(index*sameHost_size*sendcount),1, newvtype, i, i,comm,&request[index]);
			index++;
		}
		MPI_Waitall(sameHost_size-1,request,MPI_STATUS_IGNORE);
		MPI_Type_free (&newvtype);

		
		
	}
	
	//e_time=MPI_Wtime()-s_time;
	//MPI_Reduce (&e_time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
	//if(global_rank==0)
	//{
	//	printf("(%lf)\n",max_time);
	//}
	return MPI_SUCCESS;
}

