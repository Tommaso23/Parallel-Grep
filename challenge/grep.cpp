//
// Created by Tommaso Bucaioni on 09/12/22.
//

#include <fstream>
#include <iostream>
#include <sstream>
#include <mpi.h>
#include "grep.h"

#define LINELENGTH 100

namespace grep {

    void get_lines(std::vector<std::string> &input_string,
                   const std::string &file_name) {
        int rk;
        int size;
        MPI_Comm_rank(MPI_COMM_WORLD, &rk);
        MPI_Comm_size(MPI_COMM_WORLD, &size);
        std::ostringstream file_name_builder;
        file_name_builder << file_name;
        std::cout.flush();
        std::string line;
        const std::string local_file_name = file_name_builder.str();
        std::ifstream f_stream(local_file_name);
        while (getline(f_stream, line)) {
            input_string.push_back(line);
        }
        int mod = input_string.size() % size;
        if(mod != 0){
            int add_strings = size - mod;
            for(int i=0; i < add_strings; i++){
                std::string s = "                                                                                                    ";
                input_string.push_back(s);
            }
        }
    }

    char *string_to_char(std::vector<std::string> &input_string) {

        char *array = (char*) malloc(input_string.size() * LINELENGTH);

        for(auto & i : input_string){
            i.insert(i.end(), LINELENGTH - i.size(), ' ');
        }


        for(int i=0; i<input_string.size();i++){
            for(int j=0; j < LINELENGTH; j++){
                char c = input_string[i][j];
                array[i * input_string[i].size() + j] = c;
            }
        }
        return array;

    }

    void char_to_string(char *receive, std::vector<std::string> &file_name, int number_strings){
        for(int i=0; i < number_strings; i++) {
            std::string s(&receive[i * LINELENGTH], LINELENGTH);
            file_name.push_back(s);
        }
    }

    void search_string(const std::vector<std::string> &file_name,
                       const std::string & search_string,
                       lines_found & local_filtered_lines,
                       unsigned &local_lines_number, int chunk_size) {
        int rk;
        MPI_Comm_rank(MPI_COMM_WORLD, &rk);

        local_lines_number = 0;
        for(auto line : file_name) {
            ++local_lines_number;
            if (line.find(search_string) != std::string::npos) {
                local_filtered_lines.push_back(number_and_line(local_lines_number, line));

            }
        }
        for(auto & local_filtered_line : local_filtered_lines){
            local_filtered_line.first += rk * chunk_size/LINELENGTH;
        }

    }

    void print_result (const lines_found & local_filtered_lines) {
        int rk;
        int size;
        MPI_Comm_size(MPI_COMM_WORLD, &size);
        MPI_Comm_rank(MPI_COMM_WORLD, &rk);
        std::vector<int> number_lines_of_processes;
        char *local_chars;


        int number_lines_found = local_filtered_lines.size();

        if(rk == 0) {
            number_lines_of_processes.resize(size);
        }

        MPI_Gather(&number_lines_found, 1, MPI_INT, &number_lines_of_processes[0], 1, MPI_INT, 0, MPI_COMM_WORLD);




        std::vector<std::string> local_strings_found;

        for(const auto & local_filtered_line : local_filtered_lines){
            local_strings_found.push_back(local_filtered_line.second);
        }


        for(int i = 0; i < local_filtered_lines.size(); i++){
            local_chars = grep::string_to_char(local_strings_found);
        }

        int global_number_lines_found = 0;

        if (rk == 0){
            for (int number_lines_of_process : number_lines_of_processes) {
                global_number_lines_found += number_lines_of_process;
            }
        }

        int number_local_chars_to_send = LINELENGTH * number_lines_found;

        char *global_receive_chars = (char *) malloc(LINELENGTH * global_number_lines_found);

        std::vector<int> displacement;
        displacement.push_back(0);

        for(int i = 1; i < number_lines_of_processes.size(); i++) {
            displacement.push_back(number_lines_of_processes[i - 1] + displacement[i - 1]);
        }

        for(int & i : displacement){
            i *= LINELENGTH;
        }

        for(int & number_lines_of_processe : number_lines_of_processes){
            number_lines_of_processe *= LINELENGTH;
        }


        MPI_Gatherv(local_chars, number_local_chars_to_send, MPI_CHAR, global_receive_chars, &number_lines_of_processes[0], &displacement[0], MPI_CHAR, 0, MPI_COMM_WORLD);

        std::vector<int> total_filtered_lines;

        if(rk == 0) {
            total_filtered_lines.resize(global_number_lines_found);
        }

        std::vector<int> local_lines;
        for (const auto & local_filtered_line : local_filtered_lines){
            local_lines.push_back(local_filtered_line.first);
        }

        for (int & number_lines_of_processe : number_lines_of_processes) {
            number_lines_of_processe /= LINELENGTH;
        }

        std::vector<int> number_lines_displacement;
        number_lines_displacement.push_back(0);

        for(int i=1; i < displacement.size(); i++){
            displacement[i] /= LINELENGTH;
        }


        MPI_Gatherv(&local_lines[0], local_lines.size(), MPI_INT, &total_filtered_lines[0], &number_lines_of_processes[0], &displacement[0], MPI_INT, 0, MPI_COMM_WORLD);

        if(rk == 0) {
            std::vector<std::string> global_strings_found;
            grep::char_to_string(global_receive_chars, global_strings_found, global_number_lines_found);

            std::vector<grep::number_and_line> global_filtered_number_and_lines;
            global_filtered_number_and_lines.resize(global_number_lines_found);

            for(int i=0; i < global_number_lines_found; i++){
                global_filtered_number_and_lines[i].first = total_filtered_lines[i];
                global_filtered_number_and_lines[i].second = global_strings_found[i];
            }

            std::ofstream fout("program_result.txt");
            for(auto & i : global_filtered_number_and_lines){
                fout << i.first << " " << i.second << "\n";
            }

        }

    }

}
