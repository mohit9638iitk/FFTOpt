#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "mpi.h"
#include<fftw3.h>
#include<blitz/array.h>
#include<cstring>
int global_rank,global_size,my_leader,leader_size;
MPI_Comm sameNodeComm,leaderComm;
MPI_Comm sameSocketComm;
int sameHost_rank;
int sameHost_size;
int sameSocket_rank;
int sameSocket_size;
double s_time,e_time;
int flag;
char*send_buf;
char*recv_buf;
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
	

    int num_leader_per_node=2;
    int color=(sameHost_rank*num_leader_per_node)/(sameHost_size);

    MPI_Comm_split(sameNodeComm,color,sameHost_rank,&sameSocketComm);

    MPI_Comm_rank(sameSocketComm,&sameSocket_rank);
    MPI_Comm_size(sameSocketComm,&sameSocket_size);

	int leader;
	leader=global_rank/sameSocket_size;
    leader*=sameSocket_size;
    leader+=(sameSocket_size-1);	
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
    send_buf=NULL;
    recv_buf=NULL;
	return ret;
}
int MPI_Alltoall(const void *sendbuf, int sendcount, MPI_Datatype sendtype,void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm)
{

	
	int datatype_size;
	MPI_Type_size(sendtype,&datatype_size);
	if(global_rank!=my_leader)
	{
		const char* send_buf=(const char*)sendbuf;
        MPI_Request request[global_size];
		for(int i=0;i<global_size;i++)
        {
            MPI_Isend(send_buf+i*sendcount*datatype_size,sendcount*datatype_size,MPI_CHAR,sameSocket_size-1,i,sameSocketComm,&request[i]);
        }
        MPI_Waitall(global_size,request,MPI_STATUS_IGNORE);
               
        char* recv_buf=(char*)recvbuf;
        for(int i=0;i<global_size;i++)
        {
            MPI_Irecv(recv_buf+i*sendcount*datatype_size,sendcount*datatype_size,MPI_CHAR,sameSocket_size-1,i,sameSocketComm,&request[i]);
        }
        MPI_Waitall(global_size,request,MPI_STATUS_IGNORE);
	}      
	else
	{
               
		unsigned long long send_size=global_size*sameSocket_size;
        send_size*=sendcount;
        send_size*=datatype_size;
        if(!flag)
        {
            send_buf=(char*)malloc(send_size*sizeof(char));
            recv_buf=(char*)malloc(send_size*sizeof(char));
            flag=1;
        }
		const char* temp_buf=(const char*)sendbuf;
        MPI_Request request[global_size*(sameHost_size-1)];
        int position=0,index=0,tag=0;
        for(int i=0;i<global_size;i++)
        {
            for(int j=0;j<sameSocket_size;j++)
            {
                if(j==sameSocket_size-1)
                {
                    memcpy(send_buf+index*sendcount*datatype_size,temp_buf+position*sendcount*datatype_size,sendcount*datatype_size);
                    position++;  
                }
                else
                {
                    MPI_Irecv(send_buf+index*sendcount*datatype_size,sendcount*datatype_size,MPI_CHAR,j,i,sameSocketComm,&request[tag]);
                    tag++;
                }
                index++;
            }
        }
        MPI_Waitall(tag,request,MPI_STATUS_IGNORE);
        unsigned long long count_new=(send_size/leader_size);
		PMPI_Alltoall(send_buf,count_new,MPI_BYTE,recv_buf,count_new,MPI_BYTE,leaderComm);

        char * buffer_recv=(char*)recvbuf;
        position=0,index=0,tag=0;
        for(int i=0;i<global_size;i++)
        {
            for(int j=0;j<sameSocket_size;j++)
            {
                if(j==sameSocket_size-1)
                {
                    memcpy(buffer_recv+position*sendcount*datatype_size,recv_buf+index*sendcount*datatype_size,sendcount*datatype_size);
                    position++;
                }
                else
                {
                    MPI_Isend(recv_buf+index*sendcount*datatype_size,sendcount*datatype_size,MPI_CHAR,j,i,sameSocketComm,&request[tag]);
                    tag++;
                }
                index++;
            }
        }
        MPI_Waitall(tag,request,MPI_STATUS_IGNORE);
	}
     		
	return MPI_SUCCESS;
}

