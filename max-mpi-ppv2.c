#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<mpi.h>

#define MAX_LEN 100000

int main(int argc, char *argv[]) {
	MPI_Init(&argc, &argv);
	int myid, numprocs;
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	
	int numbers[MAX_LEN];
	int recv_data[MAX_LEN];
	int length;
	int scatter_size;
	
	double t_e, t_s;
	if(myid == 0) {
		t_s = MPI_Wtime();
		FILE *fp = fopen(argv[1], "r");
		int i;
		//read values
		for(i=0; fscanf(fp, " %d", &numbers[i]) != EOF; i++);
		//complete the array size to a perfect multiple of numprocs
		for(; i % numprocs > 0; i++) {
			numbers[i] = INT_MIN;
		}
		length = i;
		fclose(fp);
		scatter_size = length/numprocs;
	}
	MPI_Bcast(&scatter_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Scatter(numbers, scatter_size, MPI_INT, recv_data, scatter_size, MPI_INT, 0, MPI_COMM_WORLD);
	
	int local_max = INT_MIN;
	int i;
	for(i=0; i<scatter_size; i++) {
		if(local_max < recv_data[i]) {
			local_max = recv_data[i];
		}
	}
	
	int global_max;
	MPI_Allreduce(&local_max, &global_max, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
	
	if(myid == 0) {
		FILE *fp = fopen(argv[2],"w");
		fprintf(fp, "max-mpi-ppv2, number of processors: %d, input file: %s\n", numprocs, argv[1]);
		fprintf(fp, "%d\n", global_max);
		fclose(fp);
		t_e = MPI_Wtime();
		printf("time: %lf\n", t_e-t_s);
	}

	MPI_Finalize();
	return 0;
}