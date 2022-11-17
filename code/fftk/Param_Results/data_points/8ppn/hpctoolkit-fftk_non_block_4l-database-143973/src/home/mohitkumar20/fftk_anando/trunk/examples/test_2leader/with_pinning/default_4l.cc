

#include <mpi.h>
#include <fftw3.h>

#include <fftk.h>
#include "communication.h"
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
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
MPI_Datatype newvtype;
MPI_Datatype newvtype2;
int leader_rank;
MPI_Request *request;
MPI_Request *all2all_req;
int datatype_size;
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
	

        int num_leader_per_node=4;
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
                MPI_Comm_rank(leaderComm,&leader_rank);
                all2all_req=(MPI_Request*)malloc(2*sizeof(MPI_Request)*(leader_size-1));
                request=(MPI_Request*)malloc(sizeof(MPI_Request)*(sameSocket_size+1));

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
void Init_buff(int datatype_size,int sendcount)
{
        unsigned long long send_size=global_size*sameSocket_size;
        send_size*=sendcount;
        send_size*=datatype_size;
        send_buf=(char*)malloc(send_size*sizeof(char));
        recv_buf=(char*)malloc(send_size*sizeof(char));

        return ;
}
void Init_vector(int datatype_size,int sendcount)
{
	MPI_Type_vector(global_size,sendcount*datatype_size*sizeof(char),sendcount*sameSocket_size*datatype_size*sizeof(char),MPI_CHAR,&newvtype);
        MPI_Type_commit(&newvtype);

        unsigned long long send_size=global_size*sameSocket_size;
        send_size*=sendcount;
        send_size*=datatype_size;
         
        unsigned long long  count_new=(send_size*sizeof(char))/leader_size;
        MPI_Type_vector(leader_size,sameSocket_size*sendcount*datatype_size*sizeof(char),count_new,MPI_CHAR,&newvtype2);
        MPI_Type_commit(&newvtype2);
    
        return ;
}      
int MPI_Alltoall(const void *sendbuf, int sendcount, MPI_Datatype sendtype,void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm)
{

	
	if(global_rank!=my_leader)
	{
                if(!flag)
                {
                   MPI_Type_size(sendtype,&datatype_size);
                   flag=1;
                }
		unsigned long long  s=global_size*sendcount*datatype_size;
		
                MPI_Send(sendbuf,s,MPI_BYTE,sameSocket_size-1,sameSocket_rank,sameSocketComm);
                
                MPI_Recv(recvbuf,s,MPI_BYTE,sameSocket_size-1,sameSocket_rank,sameSocketComm,MPI_STATUS_IGNORE);
		
	}
	else
	{
                if(!flag)
                {
                     MPI_Type_size(sendtype,&datatype_size);
                     Init_buff(datatype_size,sendcount);
                     Init_vector(datatype_size,sendcount);
                     flag=1;
                }
                unsigned long long send_size=global_size*sameSocket_size;
                send_size*=sendcount;
                send_size*=datatype_size;

		int index=0;
                unsigned long long  s=global_size*sendcount*datatype_size;
                int req_count=0;
                MPI_Isend(sendbuf,s,MPI_BYTE,0,global_rank,MPI_COMM_SELF,&request[req_count++]);
                for(int i=0;i<sameSocket_size-1;i++)
                {
                        MPI_Irecv(send_buf+(index*sendcount*datatype_size*sizeof(char)),1,newvtype,i,i,sameSocketComm,&request[req_count++]);
                        index++;
		}
                MPI_Irecv(send_buf+(index*sendcount*datatype_size*sizeof(char)),1,newvtype,0,global_rank,MPI_COMM_SELF,&request[req_count++]);
                MPI_Waitall(req_count,request,MPI_STATUS_IGNORE);
                	
		unsigned long long  each_share=(send_size*sizeof(char))/leader_size;
                int all2all_count=0;
                int all2all_index=0;
                for(int i=0;i<leader_size;i++)
                {
                     if(i==leader_rank)
                     {
                         memcpy(recv_buf+all2all_index*each_share,send_buf+all2all_index*each_share,each_share);
                     }
                     else
                     {
                         MPI_Isend(send_buf+all2all_index*each_share,each_share,MPI_CHAR,i,leader_rank,leaderComm,&all2all_req[all2all_count++]);
                         MPI_Irecv(recv_buf+all2all_index*each_share,each_share,MPI_CHAR,i,i,leaderComm,&all2all_req[all2all_count++]);
                     }
                     all2all_index++;
                }
                MPI_Waitall(all2all_count,all2all_req,MPI_STATUS_IGNORE);


                index=0;
                req_count=0;
                for(int i=0;i<sameSocket_size-1;i++)
                {
                        MPI_Isend(recv_buf+(index*sameSocket_size*sendcount*datatype_size*sizeof(char)),1, newvtype2, i, i,sameSocketComm,&request[req_count++]);
                        index++;
                }
                MPI_Isend(recv_buf+(index*sameSocket_size*sendcount*datatype_size*sizeof(char)),1, newvtype2,0,global_rank,MPI_COMM_SELF,&request[req_count++]);
                MPI_Irecv(recvbuf,s,MPI_BYTE,0,global_rank,MPI_COMM_SELF,&request[req_count++]);
                MPI_Waitall(req_count,request,MPI_STATUS_IGNORE);	
	}
     		
	return MPI_SUCCESS;
}

Real f(string basis, string basis_option, int rx, int ry, int rz);
void Print_large_Fourier_elements(Array<Complex,3> A, string array_name);

int Get_kx(int lx);
int Get_ky(int ly);
int Get_kz(int lz);

int my_id,numprocs, Nx,Ny,Nz, fx_start,fy_start,fz_start;

void find_prime(int n)
{
	for(int i=2;i<=n;i++)
	{
		bool flag=false;
		
		for(int j=2;j*j<=i;j++)
		{
			if(i%j==0)
			{
				flag=true;
			}
		}
	}
	return ;
}
		
string basis,basis_option;
int main(int argc, char **argv)
{
	MPI_Init(&argc, &argv);
	double s_time,e_time,max_time;
  	MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

    basis = argv[1];
    basis_option = argv[2];

  	Nx=atoi(argv[3]);
  	Ny=atoi(argv[4]);
  	Nz=atoi(argv[5]);

    int iter = atoi(argv[6]);

    int rows = 1;

    if (argc >=8)
    	rows = atoi(argv[7]);
    else if (numprocs > Nx)
    	rows = numprocs / Nx;

	FFTK::FFTK fftk;
	fftk.Init(basis,Nx,Ny,Nz,rows,MPI_COMM_WORLD);

#ifdef BASIS_FFZ
	Array<Complex, 3> Ar_init(fftk.global->field.RA_shape);
	Array<Complex, 3> Ar(fftk.global->field.RA_shape);
	Array<Complex, 3> A(fftk.global->field.FA_shape);
#else
	Array<Real, 3> Ar_init(fftk.global->field.RA_shape);
	Array<Real, 3> Ar(fftk.global->field.RA_shape);
	Array<Complex, 3> A(fftk.global->field.FA_shape);	
#endif

	int maxrx = fftk.global->field.maxrx;
	int maxry = fftk.global->field.maxry;
	int maxrz = fftk.global->field.maxrz;

	fx_start = fftk.global->field.fx_start;
	fy_start = fftk.global->field.fy_start;
	fz_start = fftk.global->field.fz_start;

	int rx_start = fftk.global->field.rx_start;
	int ry_start = fftk.global->field.ry_start;
	int rz_start = fftk.global->field.rz_start;
        //cout<<"data size is "<<maxrx<<" "<<maxry<<" "<<maxrz<<" "<<sizeof(Real)<<endl; 
	for (int rx=0; rx<maxrx; rx++)
		for (int ry=0; ry<maxry; ry++)
			for (int rz=0; rz<maxrz; rz++){
				Ar_init(rx, ry, rz) =rx+ry+rz;
				//cout<<Ar_init(rx,ry,rz)<<" "; //f(basis, basis_option, rx_start + rx, ry_start + ry, rz);
				// Ar_init(rx, ry, rz) =  my_id*Ar_init.size() + (rx*Ar_init.extent(1)*Ar_init.extent(2)) + ry*Ar_init.extent(2) + rz;
			}
			//cout<<endl;


#ifdef BASIS_FFZ
	Ar = Ar_init;

	//MPI_Barrier(MPI_COMM_WORLD);

	time_t start;
	time_t end;


	time (&start);

	if (Ny >1) {
		fftk.Forward_transform_3d_C2C(basis_option, Ar, A);

		Print_large_Fourier_elements(A, "A");

		fftk.Inverse_transform_3d_C2C(basis_option, A, Ar);
		
	}
	else {
		fftk.Forward_transform_2d_C2C(basis_option, Ar(Range::all(),0,Range::all()), A(Range::all(),0,Range::all()));

		Print_large_Fourier_elements(A, "A");

		fftk.Inverse_transform_2d_C2C(basis_option, A(Range::all(),0,Range::all()), Ar(Range::all(),0,Range::all()));
	}

	time (&end);
#else

	if (basis[2] != 'S')
		fftk.utilities.Zero_pad_last_plane(Ar_init);

	Ar = Ar_init;
	
	//MPI_Barrier(MPI_COMM_WORLD);

	time_t start;
	time_t end;
		

	time (&start);
	s_time=MPI_Wtime();
	if (Ny >1) {
		for(int i=0;i<iter;i++)
		{
			
			fftk.Forward_transform_3d(basis_option, Ar, A);
			//ind_prime(10000000);
                        
	        }

			//fftk.Inverse_transform_3d(basis_option, A, Ar);
		//Print_large_Fourier_elements(A, "A");
	}
	else {
		fftk.Forward_transform_2d(basis_option, Ar(Range::all(),0,Range::all()), A(Range::all(),0,Range::all()));

		Print_large_Fourier_elements(A, "A");

		fftk.Inverse_transform_2d(basis_option, A(Range::all(),0,Range::all()), Ar(Range::all(),0,Range::all()));
	}
	e_time=MPI_Wtime()-s_time;
	MPI_Reduce (&e_time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
	if (fftk.global->mpi.my_id == 0)
	{
		cout<<max_time<<endl;
	}
	time (&end);
#endif
	//if (fftk.global->mpi.my_id == 0)
		//cout << "max error on master process = " << max(abs(Ar_init - Ar)) << endl;

	//MPI_Barrier(MPI_COMM_WORLD);
	//###### timer
	double *timer_avg, *timer_max, *timer_min;
	Real max_error, local_max_error;

	timer_avg = new double[22];
	timer_max = new double[22];
	timer_min = new double[22];


	//MPI_Reduce(fftk.global->misc.timer, timer_avg, 22, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	//MPI_Reduce(fftk.global->misc.timer, timer_max, 22, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
	//MPI_Reduce(fftk.global->misc.timer, timer_min, 22, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);

	
	Real dif = difftime(end, start);

	if (my_id == 0) {
		
		//cout << "max(abs(Ar-Ar_init)) on all process = " << max_error << endl;
#ifdef TIMERS		

		cout << "Time taken on master process = " << dif << endl;
		for (int i=0; i<22; i++) {
			timer_avg[i] *= 1000.0/(numprocs*iter);
			timer_max[i] *= 1000.0/iter;
			timer_min[i] *= 1000.0/iter;
		}

		double T_comp = timer_avg[0] + timer_avg[2] + timer_avg[4] + timer_avg[6] + timer_avg[8] + timer_avg[10];
		double T_comm = timer_avg[1] + timer_avg[3] + timer_avg[7] + timer_avg[9];

		cout << "Averaged time per pair of transform over all processors (ms): \n";
		cout << "Total Time: " << T_comp + T_comm << endl;
		cout << "Computation time: " << T_comp << endl;
		cout << "Communication time: " << T_comm << endl;
		Real N3 = Real(Nx)*Real(Ny)*Real(Nz);
		cout << "GFlop: " << 10*N3*log(N3)/(log(2)*(T_comp + T_comm)*numprocs*1e9) << endl;
		for (int i=0; i<22; i++)
			printf("timer[%02d] (avg/max/min): %E %E %E \n", i, timer_avg[i], timer_max[i], timer_min[i]);
#endif
	}
	fftk.Finalize();
	MPI_Finalize();
	return 0;
}

Real f(string basis, string basis_option, int rx, int ry, int rz)
{
	Real k0 = 1;
	Real k1 = 2;
	Real k2 = 3;
	Real k3 = 4;
	Real k4 = 5;
	Real k5 = 6;
	Real x,y,z;
	
	if (basis == "FFF") {
		Real Lx=2*M_PI;
    	Real Ly=2*M_PI;
    	Real Lz=2*M_PI;

    	x = rx*Lx/Nx;
		y = ry*Ly/Ny;
		z = rz*Lz/Nz;
		if (Ny>1)
			return 8*sin(k0*x)*sin(k1*y)*sin(k2*z)
			      +8*sin(k3*x)*sin(k4*y)*sin(k5*z);
		else
			return 4*sin(k0*x)*sin(k2*z)+ 4*sin(k3*x)*sin(k5*z);
	}

	else if (basis == "FFZ") {
		Real Lx=2*M_PI;
    	Real Ly=2*M_PI;
    	Real Lz=2*M_PI;

    	x = rx*Lx/Nx;
		y = ry*Ly/Ny;
		z = rz*Lz/Nz;
		if (Ny>1)
			return 8*sin(k0*x)*sin(k0*y)*sin(k0*z);
			      // +8*sin(k3*x)*sin(k4*y)*sin(k5*z);
		else
			return 4*sin(k0*x)*sin(k2*z)+ 4*sin(k3*x)*sin(k5*z);
	}
	
	else if (basis == "SFF") {
		Real Lx=M_PI;
    	Real Ly=2*M_PI;
    	Real Lz=2*M_PI;

    	x = (rx+0.5)*Lx/Nx;
   		y = ry*Ly/Ny;
		z = rz*Lz/Nz;

		if (basis_option=="SFF") {
			if (Ny>1)
				return 8*sin(k0*x)*sin(k0*y)*sin(k0*z);
			else
				return 8*sin(k0*x)*sin(k0*z);
		}
		else if (basis_option=="CFF") {
			if (Ny>1)
				return 8*cos(k0*x)*cos(k0*y)*cos(k0*z);
			else
				return 8*cos(k0*x)*cos(k0*z);
		}
	}
	
	else if (basis == "SSF") {
		Real Lx=M_PI;
    	Real Ly=M_PI;
    	Real Lz=2*M_PI;

    	x = (rx+0.5)*Lx/Nx;
		y = (ry+0.5)*Ly/Ny;
		z = rz*Lz/Nz;

		if (basis_option=="SSF") {
			if (Ny>1)
				return 8*sin(k0*x)*sin(k0*y)*cos(k0*z);
			else
				return 8*sin(k0*x)*cos(k0*z);
		}
		else if (basis_option=="CCF") {
			if (Ny>1)
				return 8*cos(k0*x)*cos(k0*y)*cos(k0*z);
			else
				return 8*cos(k0*x)*cos(k0*z);
		}
	}
	
	else if (basis == "SSS") {
		Real Lx = M_PI;
    	Real Ly = M_PI;
    	Real Lz = M_PI;

    	x = (rx+0.5)*Lx/Nx;
		y = (ry+0.5)*Ly/Ny;
		z = (rz+0.5)*Lz/Nz;

		if (basis_option=="SSS") {
			if (Ny>1)
				return 8*sin(k0*x)*sin(k1*y)*sin(k2*z)+ 8*sin(k3*x)*sin(k4*y)*sin(k5*z);
			else
				return 8*sin(k0*x)*sin(k2*z);
		}
		else if (basis_option=="CCC") {
			if (Ny>1)
				return 8*cos(k0*x)*cos(k0*y)*cos(k0*z);
			else
				return 8*cos(k0*x)*cos(k0*z);
		}
	}
}

void Print_large_Fourier_elements(Array<Complex,3> A, string array_name)
{
	int lx,ly,lz;

	for (int p=0; p<numprocs; p++) {
		if (my_id==p) {
			for ( lx=0; lx<A.extent(0); lx++)
				for ( ly=0; ly<A.extent(1); ly++)
					for ( lz=0; lz<A.extent(2); lz++) 
						if (abs(A(lx, ly, lz)) > 1E-6) {
							cout << "my_id = " << my_id << "/" << numprocs <<  " vect(k) = (" << Get_kx(lx) << "," << Get_ky(ly) << "," << Get_kz(lz) <<");  " << array_name << "(k) = " << A(lx, ly, lz) << '\n';
						}
		}
		MPI_Barrier(MPI_COMM_WORLD);
	}
}

int Get_kx(int lx) {
	if (basis[0]=='F') {
		if (lx+fx_start>Nx/2)
			return (lx+fx_start-Nx);
		else
			return (lx+fx_start);
	}
	else if (basis[0]=='S') {
			return (lx+fx_start);
	}
	return 0;
}

int Get_ky(int ly) {
	if (basis[1]=='F') {
		if (ly+fy_start>Ny/2)
			return (ly+fy_start-Ny);
		else
			return (ly+fy_start);
	}
	else if (basis[1]=='S') {
			return (ly+fy_start);
	}

	return 0;
}

int Get_kz(int lz) {
	if (basis[2]=='F') {
		if (lz+fz_start>Nz/2)
			return (lz+fz_start-Nz);
		else
			return (lz+fz_start);
	}
	else if (basis[2]=='S') {
			return (lz+fz_start);
	}
	if (basis[2]=='Z') {
		if (lz+fz_start>Nz/2)
			return (lz+fz_start-Nz);
		else
			return (lz+fz_start);
	}

	return 0;
}
