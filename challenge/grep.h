//
// Created by Tommaso Bucaioni on 09/12/22.
//

#ifndef GREP_H
#define GREP_H

#include <vector>
#include <string>
#include <utility>

namespace grep
{
    typedef std::pair<unsigned, std::string>  number_and_line;

    typedef std::vector<number_and_line> lines_found;

    void get_lines(
            std::vector<std::string> &input_string,
            const std::string &file_name);

    char *string_to_char(std::vector<std::string> &input_string);

    void char_to_string(char *array, std::vector<std::string> & file_name, int number_strings);

    void search_string(const std::vector<std::string> &file_name,
                       const std::string & search_string,
                       lines_found &lines, unsigned &local_lines_number,
                       int chunk_size);

    void print_result(const lines_found & lines);
}

#endif // GREP_H