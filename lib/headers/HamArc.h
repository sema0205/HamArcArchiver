#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <bitset>
#include <sstream>
#include <cstring>


struct HamArc {

private:
    std::string archive_name;
    std::vector <std::string> list_of_archive_files;
    int bits_per_block_amount;
    int additional_bits_amount_global;
    std::vector <std::string> list_of_unpacked_files;
    std::string path_to_extract_files;

public:
    HamArc(std::vector<std::string>& files_list, const std::string& name, int block_bits);
    HamArc(const std::string& name, int block_bits);
    void SetAdditional_bits_amount();

    // get private value
    std::string GetArchiveName();
    std::vector<std::string> GetListOfArchiveFiles();
    int GetBitsPerBlockAmount() const;
    int GetAdditionalBitsAmountGlobal() const;
    std::vector <std::string> GetListOfUnpackedFiles();

    // set private value
    void SetArchiveName(std::string name);
    void SetListOfArchiveFiles(std::vector<std::string> archive_files);
    void SetBitsPerBlockAmount(int bits_amount);
    void SetAdditionalBitsAmountGlobal(int add_bits_amount);
    void SetListOfUnpackedFiles(std::vector<std::string> unpack_files);
    void SetPathToExtractFiles(std::string unpack_path);

    static std::string GetHammingCode(std::string input);
    static std::string ReverseHammingCode(std::string input);
    void WriteListOfArchivedFiles();
    void WriteHeader();

    // Convert Functions
    static std::string ConvertFromASCIIToBinary(char convert);
    static char ConvertFromBinaryToASCII(const std::string& convert);
    std::string ExtractFileName(const std::string& absolute_path);
    int GetNewFileSize(int current_size);

    void WriteFilesToArchive();
    void TakeAllFilesFromArchive();

    std::string ShowFilesList();
    void ExtractCertainFiles();
    void AddFileToArchive(std::string new_file);
    void DeleteFileFromArchive(std::string delete_file);
};

void ConcatenateArchives(HamArc archive_first, HamArc archive_second, std::string new_archive_name);
void SetVariables(int arguments_amount, char* arguments_massive[]);