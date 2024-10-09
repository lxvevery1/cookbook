#include <cstdio>
#include <string>

class FileLoader {
  public:
    FileLoader() = default;
    ~FileLoader() = default;

    std::string load_write(const std::string& _file_path) const;
    std::string load_write_lines(const std::string& _file_path) const;
    std::string text_from_file;

    const std::string FILE_PATH = "text.txt";
};
