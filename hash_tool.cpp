#include <cstring>
#include <fstream>
#include <iostream>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <thread>
#include <vector>
#include <string>

// Function to count lines in file
size_t count_lines(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file: " + filename);
    }
    
    size_t count = 0;
    std::string line;
    while (std::getline(file, line)) {
        count++;
    }
    return count;
}

void sha256calc(const std::string &input, unsigned char *hash_output, 
                unsigned int &hash_length) {  // referencing to input string which will be
                                            // hashed, hash result storage
                                            // length of the hash
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();         // allocate memory for hashing
    EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL); // init hash context use sha256
    EVP_DigestUpdate(mdctx, input.c_str(), input.size()); // update hash context with input data
    EVP_DigestFinal_ex(mdctx, hash_output, &hash_length); // calculate hash function and store in hash_output, shows new hash length
    EVP_MD_CTX_free(mdctx);           // free memory
}

void hex_byte_conv(const std::string &hex_str,
                   unsigned char *bytes) { // iterates through hex_str reference
                                         // to str that has hex
                                         // input, pointer to array where result will be stored
    for (size_t i = 0; i < hex_str.length(); i += 2) { // gets 2 chars per byte
        std::string byte_str = hex_str.substr(i, 2); // get substrings from hex_str 2 char len
        bytes[i / 2] = static_cast<unsigned char>(std::stoi(byte_str, nullptr, 16)); // convert 2 char hex value to bin and store in bytes
    }
}

void process_lines(const std::string& wordlist_path, int start, int end, 
                  const unsigned char *target_hash, unsigned int target_hashlength, 
                  bool &found, std::string &target_password) {
    unsigned char hashed_pass[EVP_MAX_MD_SIZE];
    unsigned int hash_length = 0;

    std::ifstream file(wordlist_path); //open wordlist to read
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open wordlist file");
    }

    file.clear(); //clear error flags on file stream
    file.seekg(0); //starts from the very beginning of wordlist
    for (int i = 0; i < start; ++i) { //skipping lines until reaching to start line
        file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    std::string line;
    for (int processed_lines = 0; processed_lines < (end - start) && !found; ++processed_lines) { // read lines from wordlist until found flag is equal to true
        if (!std::getline(file, line)) {
            break; // Stop if cant read a line
        }
        sha256calc(line, hashed_pass, hash_length);
        if (memcmp(hashed_pass, target_hash, target_hashlength) == 0) { //checking 2 arrays of bytes if they are equal, if they are then:
            target_password = line; //target pass is the current line
            found = true; //change status of found 
            return; //exit
        }
    }
}

void print_usage(const char* program_name) {
    std::cout << "Usage: " << program_name << " -w <wordlist_path> -h <hash> [-t <thread_count>]\n"
              << "Options:\n"
              << "  -w <path>    Path to the wordlist file\n"
              << "  -h <hash>    Target hash (SHA-256 in hex format)\n"
              << "  -t <number>  Number of threads (default: 8)\n";
}

int main(int argc, char* argv[]) {
    std::string wordlist_path;
    std::string input_hash;
    int thread_num = 8; // number of threads I have; 4 physical + 4 logical

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-w" && i + 1 < argc) {
            wordlist_path = argv[++i];
        } else if (arg == "-h" && i + 1 < argc) {
            input_hash = argv[++i];
        } else if (arg == "-t" && i + 1 < argc) {
            thread_num = std::stoi(argv[++i]);
        } else {
            print_usage(argv[0]);
            return 1;
        }
    }

    // Validate input
    if (wordlist_path.empty() || input_hash.empty()) {
        print_usage(argv[0]);
        return 1;
    }

    if (input_hash.length() != 64) {
        std::cerr << "Error: SHA-256 hash must be 64 characters long\n";
        return 1;
    }

    try {
        // Count total lines in wordlist
        size_t lines_full = count_lines(wordlist_path);
        std::cout << "Wordlist contains " << lines_full << " lines\n";

        unsigned char target_hash[32]; // array that stores binary form of target hash
        hex_byte_conv(input_hash, target_hash); // calling function to convert given hex to a binary
        unsigned int target_hashlength = 32;

        bool found = false;           // boolean that shows if match found default is false edited in the loop
        std::string target_password; // store the found pass here
        int lines_per_thread = lines_full / thread_num; // deciding number of lines to give for each thread

        std::vector<std::thread> threads; // holding thread objects here
        std::cout << "Starting search with " << thread_num << " threads...\n";

        for (int i = 0; i < thread_num; ++i) { // preparing to create threads
            int start = i * lines_per_thread; // calculating start line and end line for each thread
            int end;
            if (i == thread_num - 1) { // if last thread, assign remaining lines to that thread
                end = lines_full;
            } else {
                end = start + lines_per_thread; //if not last thread, end index is start + lines_per_thread
            }

            threads.emplace_back(process_lines, 
                               std::ref(wordlist_path),
                               start, 
                               end, 
                               target_hash,
                               target_hashlength, 
                               std::ref(found), 
                               std::ref(target_password));
        }

        for (auto &t : threads) {
            t.join();
        } // joining threads

        if (found) {
            std::cout << "Match found! The password is: " << target_password << "\n";
        } else {
            std::cout << "No match found in the given dictionary.\n";
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
