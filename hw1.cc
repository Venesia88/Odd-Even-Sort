#include <stdio.h>
#include <math.h>
#include <mpi.h>
#include <boost/sort/spreadsort/float_sort.hpp>

int main(int argc, char** argv) {
	int rank, size;

	MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int n = atoi(argv[1]);

    int n_per_process = n / size;
    int remainder = n % size;

    float* otherdata = (float*) malloc((n_per_process + 1)* sizeof(float));
    

    MPI_File f, output;

    int start_n = rank * n_per_process + remainder;
    if(rank < remainder){
        ++n_per_process;
        start_n = rank * n_per_process;
    }

    float* data = (float*) malloc(n_per_process * sizeof(float));
    
    MPI_File_open(MPI_COMM_WORLD, argv[2], MPI_MODE_RDONLY, MPI_INFO_NULL, &f);
    MPI_File_read_at(f, sizeof(float) * start_n, data, n_per_process, MPI_FLOAT, MPI_STATUS_IGNORE);
    boost::sort::spreadsort::float_sort(data, data + n_per_process);

    MPI_File_close(&f);

    int before = n_per_process, after = n_per_process;
    if(rank == remainder)
        before = n_per_process + 1;

    else if(rank == remainder - 1)
        after = n_per_process - 1;

    int terminate = 1, flag = 0;
    float* temp = (float*) malloc(n_per_process * sizeof(float));
    float* ptr = data;

    while(terminate){
        //even phase
        if(rank != size-1 && !(rank & 1)){ //even rank, take the minimum, rank < partner
            MPI_Sendrecv(&data[n_per_process-1], 1, MPI_FLOAT, rank + 1, 1, otherdata, 1, MPI_FLOAT, rank + 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            if(data[n_per_process - 1] > otherdata[0]) {
                MPI_Sendrecv(data, n_per_process, MPI_FLOAT, rank + 1, 1, otherdata, after, MPI_FLOAT, rank + 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
                for(int i = 0, j = 0, k = 0; i < n_per_process; ++i){
                    if(data[j] <= otherdata[k] || k >= after) temp[i] = data[j++];
                    else{
                        flag = 1;
                        temp[i] = otherdata[k++];
                    }
                }
                ptr = data;
                data = temp;
                temp = ptr;
            }
        } 
        else if(rank & 1){ //odd rank, take the maximum, rank > partner
            MPI_Sendrecv(&data[0], 1, MPI_FLOAT, rank - 1, 1, otherdata, 1, MPI_FLOAT, rank - 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    
            if(data[0] < otherdata[0]) {
                MPI_Sendrecv(data, n_per_process, MPI_FLOAT, rank - 1, 1, otherdata, before, MPI_FLOAT, rank - 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                for(int i = n_per_process - 1, j = n_per_process - 1, k = before - 1; i >= 0; --i){
                    if(data[j] >= otherdata[k]) {
                        temp[i] = data[j--];
                    }
                    else{
                        temp[i] = otherdata[k--];
                        flag = 1;
                    }
                }
                ptr = data;
                data = temp;
                temp = ptr;
            }
        }

        
        //odd phase
        if(rank != size-1 && (rank & 1)){ //odd rank, take the minimum, rank < partner
            MPI_Sendrecv(&data[n_per_process - 1], 1, MPI_FLOAT, rank + 1, 1, otherdata, 1, MPI_FLOAT, rank + 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            

            if(data[n_per_process - 1] > otherdata[0]) {
                MPI_Sendrecv(data, n_per_process, MPI_FLOAT, rank + 1, 1, otherdata, after, MPI_FLOAT, rank + 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                

                for(int i = 0, j = 0, k = 0; i < n_per_process; ++i){
                    if(data[j] <= otherdata[k] || k >= after) temp[i] = data[j++];
                    else{
                        flag = 1;
                        temp[i] = otherdata[k++];
                    }
                }
                ptr = data;
                data = temp;
                temp = ptr;
            }
        } 
        else if(rank != 0 && !(rank & 1)){ //even rank, take the max, rank > partner
            MPI_Sendrecv(&data[0], 1, MPI_FLOAT, rank - 1, 1, otherdata, 1, MPI_FLOAT, rank - 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            
            if(data[0] < otherdata[0]) {
                MPI_Sendrecv(data, n_per_process, MPI_FLOAT, rank - 1, 1, otherdata, before, MPI_FLOAT, rank - 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                

                for(int i = n_per_process - 1, j = n_per_process - 1, k = before - 1; i >= 0; --i){
                    if(data[j] >= otherdata[k]) {
                        temp[i] = data[j--];
                    }
                    else{
                        temp[i] = otherdata[k--];
                        flag = 1;
                    }
                }

                ptr = data;
                data = temp;
                temp = ptr;
            }
        }

        MPI_Allreduce(&flag, &terminate, 1, MPI_INT, MPI_LOR, MPI_COMM_WORLD);
        flag = 0;
        
    }

    MPI_File_open(MPI_COMM_WORLD, argv[3], MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &output);
    MPI_File_write_at(output, sizeof(float) * start_n, data, n_per_process, MPI_FLOAT, MPI_STATUS_IGNORE);
    MPI_File_close(&output);
    
    MPI_Finalize();

    
}