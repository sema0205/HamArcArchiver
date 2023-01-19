#include "../headers/HamArc.h"

HamArc::HamArc(std::vector<std::string>& files_list, const std::string& name, int block_bits) {
    archive_name = name + ".haf";
    bits_per_block_amount = block_bits;
    list_of_archive_files.assign(files_list.begin(), files_list.end());
    SetAdditional_bits_amount();
}

void HamArc::WriteFilesToArchive() {

    SetAdditional_bits_amount();
    WriteHeader();
    std::ofstream archive_file(archive_name, std::ios::app|std::ios::binary);

    for (const auto& path : list_of_archive_files) {
        std::ifstream temp_file(path, std::ios::binary);
        char single_byte_block;
        if (temp_file.is_open()) {

            while(temp_file.get(single_byte_block)) {
                std::string test = ConvertFromASCIIToBinary(single_byte_block);
                std::string converted_byte = GetHammingCode(ConvertFromASCIIToBinary(single_byte_block));
                for (int i = 1; i <= additional_bits_amount_global; i++) {
                    converted_byte += "0";
                }

                std::string first_part = converted_byte.substr(0, bits_per_block_amount);
                std::string second_part = converted_byte.substr(bits_per_block_amount, bits_per_block_amount);

                char first_char = ConvertFromBinaryToASCII(first_part);
                char second_char = ConvertFromBinaryToASCII(second_part);

                archive_file.write((char*) &first_char, bits_per_block_amount / 8);
                archive_file.write((char*) &second_char, bits_per_block_amount / 8);
            }

        }
    }
}

void HamArc::TakeAllFilesFromArchive() {

    std::ifstream archive(archive_name, std::ios::binary);
    char info_header[11];
    archive.read(info_header, 11);

    char* files_info = new char[atoi(info_header)];
    archive.read(files_info, atoi(info_header));

    std::vector<std::string> list_files;
    char* info_block = std::strtok(files_info,"|");
    int amount = 0;

    bool flag = true;

    while(info_block && flag) {

        if (strlen(info_block) == 0) {
            flag = false;
        }

        list_files.push_back(info_block);

        info_block = strtok(nullptr, "|");
        amount++;

    }

    char first_bits_block[1];
    char second_bits_block[1];

    for (int i = 0; i < amount / 2; i++) {

        std::string file_size = list_files[i * 2];
        std::string file_name = list_files[i * 2 + 1];
        std::string unpack_path = path_to_extract_files + "/" + ExtractFileName(file_name);

        std::ofstream unpacked_file(unpack_path,std::ios::binary|std::ios::app);

        for (int j = 1; j <= stoi(file_size)/2; j++) {

            archive.read(first_bits_block, bits_per_block_amount / 8);
            archive.read(second_bits_block, bits_per_block_amount / 8);

            char first_char = first_bits_block[0];
            char second_char = second_bits_block[0];

            std::string full_byte = ConvertFromASCIIToBinary(first_char) + ConvertFromASCIIToBinary(second_char);
            full_byte = full_byte.substr(0, 12);
            std::string hamming_check = ReverseHammingCode(full_byte);
            char write_byte = ConvertFromBinaryToASCII(hamming_check);
            unpacked_file.write((char*) &write_byte, 1);
        }

    }

}

std::string HamArc::GetHammingCode(std::string input) {

    std::string answer;
    int string_size = input.size();
    int additional_bits_amount = 1;
    while(pow(2,additional_bits_amount) < (string_size + additional_bits_amount + 1)) {additional_bits_amount++;}

    std::vector<std::string> hamming_input(additional_bits_amount + string_size);
    for (int i = 0; i < additional_bits_amount; i++) {
        hamming_input[pow(2,i)-1] = "2";
    }

    int string_counter = 0;

    for (int i = 0; i < hamming_input.size(); i++) {
        if (hamming_input[i] != "2") {
            hamming_input[i] = input[string_counter];
            string_counter++;
        }
    }

    for (int i = 0; i < hamming_input.size(); i++) {
        if (hamming_input[i] == "2") {
            int parity_sum = 0;
            int position = log2(i + 1);
            for (int j = i + 2; j <= hamming_input.size(); j++) {
                if (j & (1 << position)) {
                    if (hamming_input[j - 1] == "1") {
                        parity_sum++;
                    }
                }
            }
            if (parity_sum % 2 != 0) {
                hamming_input[i] = "1";
            } else {
                hamming_input[i] = "0";
            }
        }
    }

    for (auto & bit : hamming_input) {
        answer += bit;
    }

    return answer;
}

void HamArc::WriteHeader() {

    std::string header_info;
    // byte reserved for header separator
    int header_size = 1;

    for (const auto& path : list_of_archive_files) {
//        std::string temp_name = ExtractFileName(path);
        std::ifstream temp_file(path, std::ios::binary|std::ios::ate);
        if (temp_file.is_open()) {
            int file_size = GetNewFileSize(temp_file.tellg());
            header_info += std::to_string(file_size) + "|" + path + "|";
        }
    }

    std::ofstream archive_file(archive_name, std::ios::binary|std::ios::app);
    header_size += header_info.size();

    int header_digits = 0;
    int count_header = header_size;
    while(count_header) {count_header /= 10; header_digits++;}

    std::string header_size_string = std::to_string(header_size);
    for (int i = 0; i <= 10-header_digits; i++) {
        header_size_string = "0" + header_size_string;
    }

    archive_file << header_size_string << "|" << header_info;
}

std::string HamArc::ConvertFromASCIIToBinary(char convert) {

    std::string answer;
    answer += std::bitset<8>(convert).to_string();

    return answer;
}

char HamArc::ConvertFromBinaryToASCII(const std::string& convert) {

    std::stringstream stream(convert);
    std::string answer;

    while(stream.good()) {
        std::bitset<8> bits;
        stream >> bits;
        answer += static_cast<char>(bits.to_ulong());
    }


    return answer[0];
}

std::string HamArc::ExtractFileName(const std::string& absolute_path) {

    std::string path;
    path = absolute_path.substr(absolute_path.find_last_of('/') + 1, absolute_path.size());

    return path;
}

int HamArc::GetNewFileSize(int current_size) {

    int future_size;
    int additional_bits_amount = 1;
    while(pow(2,additional_bits_amount) < (bits_per_block_amount + additional_bits_amount + 1)) {additional_bits_amount++;}
    int round_size = ceil(((bits_per_block_amount + additional_bits_amount) / 8)) + 1;
    future_size = round_size * current_size;

    return future_size;
}

std::string HamArc::ReverseHammingCode(std::string input) {

    std::string hamming_base;
    std::vector<std::string> convert_copy(input.size(), "0");

    for (int i = 0; i < input.size(); i++) {
        convert_copy[i] = input[i];
    }

    int additional_bits_amount = -1;

    while(pow(2,additional_bits_amount) < (input.size() + additional_bits_amount + 1)) {additional_bits_amount++;}
    additional_bits_amount--;

    for (int i = 0; i < additional_bits_amount; i++) {
        convert_copy[pow(2,i)-1] = "2";
    }


    for (int i = 0; i < convert_copy.size(); i++) {
        if (convert_copy[i] != "2") {
            hamming_base += convert_copy[i];
        }
    }


    std::string compare_string = GetHammingCode(hamming_base);
    int error_position = -1;

    for (int i = 0; i < additional_bits_amount; i++) {
        if (input[pow(2, i) - 1] != compare_string[pow(2, i) - 1]) {
            error_position += pow(2, i);
        }
    }

    std::string answer;

    if (error_position == -1) {
        answer = hamming_base;
    } else {
        if (convert_copy[error_position] == "1") {
            convert_copy[error_position] = '0';
        } else {
            convert_copy[error_position] = "1";
        }
        for (int i = 0; i < convert_copy.size(); i++) {
            if (convert_copy[i] != "2") {
                answer += convert_copy[i];
            }
        }
    }

    return answer;
}

void HamArc::SetAdditional_bits_amount() {

    additional_bits_amount_global = 1;
    while(pow(2,additional_bits_amount_global) < (bits_per_block_amount + additional_bits_amount_global + 1)) {additional_bits_amount_global++;}

}

std::string HamArc::ShowFilesList() {

    std::ifstream archive(archive_name, std::ios::binary);
    char info_header[11];
    archive.read(info_header, 11);

    char* files_info = new char[atoi(info_header)];
    archive.read(files_info, atoi(info_header));

    std::vector<std::string> list_files;
    char* info_block = std::strtok(files_info,"|");
    int amount = 0;

    bool flag = true;

    while(info_block && flag) {

        if (strlen(info_block) == 0) {
            flag = false;
        }

        list_files.push_back(info_block);

        info_block = strtok(nullptr, "|");
        amount++;

    }

    std::string answer;

    for (int i = 0; i < amount / 2; i++) {
        answer += ExtractFileName(list_files[i * 2 + 1]) + " ";
    }

    std::cout << answer;
    return answer;
}

void HamArc::ExtractCertainFiles() {

    std::ifstream archive(archive_name, std::ios::binary);
    char info_header[11];
    archive.read(info_header, 11);

    char* files_info = new char[atoi(info_header)];
    archive.read(files_info, atoi(info_header));

    std::vector<std::string> list_files;
    char* info_block = std::strtok(files_info,"|");
    int amount = 0;

    bool flag = true;

    while(info_block && flag) {

        if (strlen(info_block) == 0) {
            flag = false;
        }

        list_files.push_back(info_block);

        info_block = strtok(nullptr, "|");
        amount++;

    }

    char first_bits_block[1];
    char second_bits_block[1];
    bool file_checked = false;

    for (int i = 0; i < amount / 2; i++) {

        std::string file_name = list_files[i * 2 + 1];
        std::string file_size = list_files[i * 2];
        std::string unpack_path = path_to_extract_files + "/" + ExtractFileName(file_name);

        for (int s = 0; s < GetListOfUnpackedFiles().size(); s++) {
            if (file_name == GetListOfUnpackedFiles()[s]) {

                file_checked = true;
                std::ofstream unpacked_file(unpack_path,std::ios::binary|std::ios::app);

                for (int j = 1; j <= stoi(file_size)/2; j++) {


                    archive.read(first_bits_block, 1);
                    archive.read(second_bits_block, 1);

                    char first_char = first_bits_block[0];
                    char second_char = second_bits_block[0];

                    std::string full_byte = ConvertFromASCIIToBinary(first_char) + ConvertFromASCIIToBinary(second_char);
                    full_byte = full_byte.substr(0, bits_per_block_amount+additional_bits_amount_global);
                    std::string hamming_check = ReverseHammingCode(full_byte);
                    char write_byte = ConvertFromBinaryToASCII(hamming_check);
                    unpacked_file.write((char*) &write_byte, 1);
                }
            }
        }
        if (!file_checked) {
            for (int j = 1; j <= stoi(file_size) / 2; j++) {
                archive.read(first_bits_block, 1);
                archive.read(second_bits_block, 1);
            }
        }
    }

}

std::string HamArc::GetArchiveName() {

    return archive_name;
}

std::vector<std::string> HamArc::GetListOfArchiveFiles() {

    return list_of_archive_files;
}

int HamArc::GetBitsPerBlockAmount() const {

    return bits_per_block_amount;
}

int HamArc::GetAdditionalBitsAmountGlobal() const {

    return additional_bits_amount_global;
}

std::vector<std::string> HamArc::GetListOfUnpackedFiles() {

    return list_of_unpacked_files;
}


void HamArc::SetArchiveName(std::string name) {
    archive_name = name;
}

void HamArc::SetListOfArchiveFiles(std::vector<std::string> archive_files) {
    list_of_archive_files = archive_files;
}

void HamArc::SetBitsPerBlockAmount(int bits_amount) {
    bits_per_block_amount = bits_amount;
}

void HamArc::SetAdditionalBitsAmountGlobal(int add_bits_amount) {
    additional_bits_amount_global = add_bits_amount;
}

void HamArc::SetListOfUnpackedFiles(std::vector<std::string> unpack_files) {
    list_of_unpacked_files = unpack_files;
}

HamArc::HamArc(const std::string &name, int block_bits) {
    archive_name = name + ".haf";
    bits_per_block_amount = block_bits;
    SetAdditional_bits_amount();
}

void HamArc::AddFileToArchive(std::string new_file) {

    WriteListOfArchivedFiles();
    remove(archive_name.c_str());
    list_of_archive_files.push_back(new_file);
    WriteFilesToArchive();
}

void HamArc::DeleteFileFromArchive(std::string delete_file) {

    WriteListOfArchivedFiles();
    remove(archive_name.c_str());
    list_of_archive_files.erase(std::remove(list_of_archive_files.begin(), list_of_archive_files.end(), delete_file), list_of_archive_files.end());
    WriteFilesToArchive();
}

void HamArc::WriteListOfArchivedFiles() {

    std::ifstream archive(archive_name, std::ios::binary);
    char info_header[11];
    archive.read(info_header, 11);

    char* files_info = new char[atoi(info_header)];
    archive.read(files_info, atoi(info_header));

    std::vector<std::string> list_files;
    char* info_block = std::strtok(files_info,"|");
    int amount = 0;

    bool flag = true;

    while(info_block && flag) {

        if (strlen(info_block) == 0) {
            flag = false;
        }

        list_files.push_back(info_block);

        info_block = strtok(nullptr, "|");
        amount++;

    }

    for (int i = 0; i < amount / 2; i++) {

        std::string file_name = list_files[i * 2 + 1];
        list_of_archive_files.push_back(file_name);
    }

}

void HamArc::SetPathToExtractFiles(std::string unpack_path) {
    path_to_extract_files = unpack_path;
}


void ConcatenateArchives(HamArc archive_first, HamArc archive_second, std::string new_archive_name) {

    std::vector<std::string> merged_files_list;
    archive_first.WriteListOfArchivedFiles();
    archive_second.WriteListOfArchivedFiles();

    for (int i = 0; i < archive_first.GetListOfArchiveFiles().size(); i++) {
        merged_files_list.push_back(archive_first.GetListOfArchiveFiles()[i]);
    }

    for (int i = 0; i < archive_second.GetListOfArchiveFiles().size(); i++) {
        merged_files_list.push_back(archive_second.GetListOfArchiveFiles()[i]);
    }

    HamArc concatenated_archive(merged_files_list, new_archive_name, archive_first.GetBitsPerBlockAmount());
    concatenated_archive.WriteFilesToArchive();
}

void SetVariables(int arguments_amount, char** arguments_massive) {

    bool create_archive = false;
    std::string main_archive_name;
    std::vector<std::string> list_of_some_files;

    bool show_list_of_files = false;

    bool extract_files = false;

    bool add_file_to_archive = false;
    std::string add_file;

    bool delete_files = false;
    std::string delete_file;

    bool concatenate_archives = false;
    std::string first_merged_archive;
    std::string second_merged_archive;

    int block_of_bits = 8;
    std::string path_for_extraction;

    std::vector<std::string> vector_of_arguments(arguments_massive+1, arguments_massive + arguments_amount);

    for (int i = 0; i < arguments_amount-1; i++) {
        if (vector_of_arguments[i] == "-c" || vector_of_arguments[i] == "--create") {
            create_archive = true;
        } else if (vector_of_arguments[i] == "-f") {
            main_archive_name = vector_of_arguments[i+1];
        } else if (vector_of_arguments[i].substr(0, vector_of_arguments[i].find_first_of('=')) == "--file") {
            main_archive_name = vector_of_arguments[i].substr(vector_of_arguments[i].find_first_of('=') + 1, vector_of_arguments[i].size());
        } else if (vector_of_arguments[i] == "-l" || vector_of_arguments[i] == "--list") {
            show_list_of_files = true;
        } else if (vector_of_arguments[i] == "-x" || vector_of_arguments[i] == "--extract") {
            extract_files = true;
        } else if (vector_of_arguments[i] == "-a" || vector_of_arguments[i] == "--append") {
            add_file_to_archive = true;
        } else if (vector_of_arguments[i] == "-d" || vector_of_arguments[i] == "--delete") {
            delete_files = true;
        } else if (vector_of_arguments[i] == "-p") {
            path_for_extraction = vector_of_arguments[i+1];
        } else if (vector_of_arguments[i] == "-A" || vector_of_arguments[i] == "--concantenate") {
            concatenate_archives = true;
        } else if (vector_of_arguments[i] == "-b") {
            block_of_bits = std::stoi(vector_of_arguments[i+1]);
        } else if (concatenate_archives && first_merged_archive.empty()) {
            first_merged_archive = vector_of_arguments[i];
            second_merged_archive = vector_of_arguments[i+1];
        } else if (vector_of_arguments[i-1] != "-f" && vector_of_arguments[i-1] != "-b" && vector_of_arguments[i-1] != "-p") {
            list_of_some_files.push_back(vector_of_arguments[i]);
        }
    }

    if (create_archive) {
        HamArc new_created_archive(list_of_some_files, main_archive_name, block_of_bits);
        new_created_archive.WriteFilesToArchive();
    } else if (show_list_of_files) {
        HamArc new_show_list_archive(main_archive_name, block_of_bits);
        new_show_list_archive.ShowFilesList();
    } else if (extract_files) {
        HamArc new_extract_some_files(main_archive_name, block_of_bits);
        new_extract_some_files.SetPathToExtractFiles(path_for_extraction);
        if (list_of_some_files.empty()) {
            new_extract_some_files.TakeAllFilesFromArchive();
        } else {
            new_extract_some_files.SetListOfUnpackedFiles(list_of_some_files);
            new_extract_some_files.ExtractCertainFiles();
        }
    } else if (add_file_to_archive) {
        HamArc new_add_file_to_archive(main_archive_name, block_of_bits);
        new_add_file_to_archive.AddFileToArchive(list_of_some_files[0]);
    } else if (delete_files) {
        HamArc new_delete_file_to_archive(main_archive_name, block_of_bits);
        new_delete_file_to_archive.DeleteFileFromArchive(list_of_some_files[0]);
    } else if (concatenate_archives) {
        HamArc first_to_merge_archive(first_merged_archive, block_of_bits);
        HamArc second_to_merge_archive(second_merged_archive, block_of_bits);
        ConcatenateArchives(first_to_merge_archive, second_to_merge_archive, main_archive_name);
    }

}