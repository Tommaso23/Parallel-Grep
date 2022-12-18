#include <mpi.h>
#include "grep.h"
#include <iostream>



#define LINELENGTH 100

int main () {

    MPI_Init(NULL, NULL);
    double start = MPI_Wtime();
    int rank, size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    std::vector<std::string> input_string;
    char *char_file;
    std::vector<std::string> local_file;
    grep::lines_found local_filtered_lines;
    unsigned int local_lines_number = 0;
    int chunk_size;


    if(rank == 0) {
        grep::get_lines(input_string, "input_file.txt");
        char_file = grep::string_to_char(input_string);
        chunk_size = input_string.size() * LINELENGTH / size;
    }


    MPI_Bcast(&chunk_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
    char *receive_char = (char*) malloc(chunk_size);
    MPI_Scatter(char_file, chunk_size, MPI_CHAR, receive_char, chunk_size, MPI_CHAR, 0, MPI_COMM_WORLD);
    int number_strings = chunk_size / LINELENGTH;
    grep::char_to_string(receive_char, local_file, number_strings);
    grep::search_string(local_file, "only", local_filtered_lines, local_lines_number, chunk_size);

    grep::print_result(local_filtered_lines);
    double end = MPI_Wtime();
    std::cout << end - start;

    MPI_Finalize();

    return 0;
}


