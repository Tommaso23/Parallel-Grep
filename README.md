# Parallel-Grep-with-openMPI
A small project for the Advanced Algorithm and Parallel Programming course at Politecnico di Milano.
This project performs a parallel grep search, using openMPI message passing interface implementation.
The input_file.txt is generated randomly, divided into equal chunks by process 0 and forwarded across all the processes alive.
Each process performs grep search and forwards resulting string to process 0. Process 0 recollects all the partial results and print the strings in program_result.txt file.
