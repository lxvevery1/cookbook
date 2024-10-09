#include "../include/FileLoader.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

std::string FileLoader::load_write(const std::string& _file_path) const {
    // open file using ifstream
    std::ifstream file(_file_path);

    // confirm file opening
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << _file_path << std::endl;
        file.close();

        exit(1);
    }

    std::stringstream buffer;

    buffer << file.rdbuf();

    // empty buffer case
    if (buffer.rdbuf()->str().empty()) {
        std::cerr << "File is empty: " << _file_path << std::endl;
        file.close();

        exit(1);
    }

    file.close();

    return buffer.str();
}

std::string FileLoader::load_write_lines(const std::string& _file_path) const {
    // open file using ifstream
    std::ifstream file(_file_path);
    std::string fileContent;

    // confirm file opening
    if (file.is_open()) {
        std::string line;

        while (std::getline(file, line)) {
            fileContent += line;
            fileContent += "\n";
        }

        file.close();
    } else {
        std::cerr << "Failed to open file: " << _file_path << std::endl;
        file.close();

        exit(1);
    }

    file.close();

    return fileContent;
}
