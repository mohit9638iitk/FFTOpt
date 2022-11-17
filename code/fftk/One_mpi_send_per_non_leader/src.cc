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
int flag,counter;
int flag1;
char*send_buf;
char**temp_buf;
char*recv_buf;
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
    counter=0;
    flag1=0;
	return ret;
}

void Init_buff(int datatype_size,int sendcount)
{
    unsigned long long send_size=global_size*sameSocket_size;
    send_size*=sendcount;
    send_size*=datatype_size;
    send_buf=(char*)malloc(send_size*sizeof(char));
    recv_buf=(char*)malloc(send_size*sizeof(char));
        
    unsigned long long  count_new=(send_size*sizeof(char))/leader_size;
    MPI_Type_vector(leader_size,sameSocket_size*sendcount*datatype_size*sizeof(char),count_new,MPI_CHAR,&newvtype2);
    MPI_Type_commit(&newvtype2);
    return ;
}
void buff_init(int sendcount,int datatype_size)
{
    temp_buf=(char**)malloc(sameSocket_size*sizeof(char*));
    unsigned long long s=sendcount;
    s*=datatype_size;
    s*=global_size;
    temp_buf[0]=(char*)malloc(sameSocket_size*s*sizeof(char*));
		
    for(int i=1;i<sameSocket_size;i++)
    {
        temp_buf[i]=&(temp_buf[0][i*s]);
    }

    return ;
}
             
int MPI_Alltoall(const void *sendbuf, int sendcount, MPI_Datatype sendtype,void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm)
{

	
	int datatype_size;
	MPI_Type_size(sendtype,&datatype_size);

	if(global_rank!=my_leader)
	{
		
		MPI_Request request;
		unsigned long long  s=global_size*sendcount*datatype_size;

		MPI_Isend(sendbuf,s,MPI_BYTE,sameSocket_size-1,sameHost_rank,sameSocketComm,&request);
		MPI_Wait(&request,MPI_STATUS_IGNORE);

        MPI_Irecv(recvbuf,s,MPI_BYTE,sameSocket_size-1,MPI_ANY_TAG,sameSocketComm,MPI_STATUS_IGNORE,&request);
		MPI_Wait(&request,MPI_STATUS_IGNORE);
	}
	else
	{
        unsigned long long s=global_size*sendcount*datatype_size;
        if(!flag)
        {
            buff_init(sendcount,datatype_size);
            Init_buff(sendcount,datatype_size);
            flag=1;
        }
        MPI_Request request[sameSocket_size-1];
        for(int i=0;i<sameSocket_size-1;i++)
        {
            MPI_Irecv(temp_buf[i],s,MPI_BYTE,i,MPI_ANY_TAG,sameSocketComm,&request[i]);
        }
        const char *temp=(char*)sendbuf;
        for(unsigned long long i=0;i<s;i++)
        {
            temp_buf[sameSocket_size-1][i]=temp[i];
        }       
        MPI_Waitall(sameSocket_size-1,request,MPI_STATUS_IGNORE);

        unsigned long long index=0;
        for(int i=0;i<global_size;i++)
        {
            for(int j=0;j<sameSocket_size;j++)
            {
                for(int k=0;k<sendcount*datatype_size;k++)
                {				
                    send_buf[index++]=temp_buf[j][sendcount*datatype_size*i+k];
                }
            }
        }
        unsigned long long send_size=global_size*sameSocket_size;
        send_size*=sendcount;
        send_size*=datatype_size;
        unsigned long long  count_new=(send_size*sizeof(char))/leader_size;
        PMPI_Alltoall(send_buf,count_new,MPI_BYTE,recv_buf,count_new,MPI_BYTE,leaderComm);

                
		index=0;
		for(int i=0;i<sameSocket_size-1;i++)
		{
			MPI_Isend(recv_buf+(index*sameSocket_size*sendcount*datatype_size*sizeof(char)),1, newvtype2, i, i,sameSocketComm,&request[index]);
			index++;
		}
		MPI_Waitall(sameSocket_size-1,request,MPI_STATUS_IGNORE);
		MPI_Request check;
        MPI_Isend(recv_buf+(index*sameSocket_size*sendcount*datatype_size*sizeof(char)),1, newvtype2,0,global_rank,MPI_COMM_SELF,&check);	
		MPI_Recv(recvbuf,s,MPI_BYTE,0,global_rank,MPI_COMM_SELF,MPI_STATUS_IGNORE);	
                
	}
     		
	return MPI_SUCCESS;
}

