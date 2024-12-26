#include <iostream>

#include <fstream>

#include <sstream>

#include <vector>

#include <string>

#include <filesystem>

#include <openssl/sha.h>

#include <iomanip>

#include <unordered_map>

class Watcher
{

private:
 
    std::unordered_map<std::string, std::string> fileHashes;

    std::string baseDirectory;

    std::string calculateFileHash(const std::string &filepath)
    {

        std::ifstream file(filepath, std::ios::binary);

        if (!file)
        {

            std::cerr << "Error opening file: " << filepath << std::endl;

            return "";
        }

        SHA256_CTX sha256;

        SHA256_Init(&sha256);

        char buffer[4096];

        while (file.read(buffer, sizeof(buffer)).gcount() > 0)
        {

            SHA256_Update(&sha256, buffer, file.gcount());
        }

        unsigned char hash[SHA256_DIGEST_LENGTH];

        SHA256_Final(hash, &sha256);

        std::stringstream ss;

        for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        {

            ss << std::hex << std::setw(2) << std::setfill('0')

               << static_cast<int>(hash[i]);
        }

        return ss.str();
    }

    void scanDirectory(const std::string &directory)
    {

        for (const auto &entry : std::filesystem::recursive_directory_iterator(directory))
        {

            if (entry.is_regular_file())
            {

                std::string filepath = entry.path().string();

                std::string relativePath = std::filesystem::relative(filepath, baseDirectory).string();

                fileHashes[relativePath] = calculateFileHash(filepath);
            }
        }
    }

    void saveHashDatabase(const std::string &dbFilePath)
    {

        std::ofstream dbFile(dbFilePath);

        if (!dbFile)
        {

            std::cerr << "Error creating hash database file." << std::endl;

            return;
        }

        for (const auto &[filepath, hash] : fileHashes)
        {

            dbFile << filepath << "," << hash << "\n";
        }

        dbFile.close();
    }

    bool loadHashDatabase(const std::string &dbFilePath)
    {

        std::ifstream dbFile(dbFilePath);

        if (!dbFile)
        {

            std::cerr << "No existing hash database found." << std::endl;

            return false;
        }

        fileHashes.clear();

        std::string line;

        while (std::getline(dbFile, line))
        {

            std::istringstream ss(line);

            std::string filepath, hash;

            std::getline(ss, filepath, ',');

            std::getline(ss, hash);

            fileHashes[filepath] = hash;
        }

        dbFile.close();

        return true;
    }

public:
    Watcher(const std::string &directory) : baseDirectory(directory) {}

    void createInitialDatabase(const std::string &dbFilePath)
    {

        scanDirectory(baseDirectory);

        saveHashDatabase(dbFilePath);

        std::cout << "Initial hash database created successfully." << std::endl;
    }

    void checkIntegrity(const std::string &dbFilePath)
    {

        if (!loadHashDatabase(dbFilePath))
        {

            std::cerr << "Cannot verify integrity without a baseline." << std::endl;

            return;
        }

        bool integrityIntact = true;

        for (const auto &entry : std::filesystem::recursive_directory_iterator(baseDirectory))
        {

            if (entry.is_regular_file())
            {

                std::string filepath = entry.path().string();

                std::string relativePath = std::filesystem::relative(filepath, baseDirectory).string();

            
                if (fileHashes.find(relativePath) == fileHashes.end())
                {

                    std::cout << "New file detected: " << relativePath << std::endl;

                    continue;
                }

                std::string currentHash = calculateFileHash(filepath);

                if (currentHash != fileHashes[relativePath])
                {

                    std::cout << "INTEGRITY COMPROMISED: " << relativePath << std::endl;

                    integrityIntact = false;
                }
            }
        }

    }
};

int main(int argc, char *argv[])
{

    if (argc < 3)
    {

        std::cerr << "usage: " << argv[0]

                  << " <directory> <mode> [database_path]\n";

        std::cerr << "modes:\n";

        std::cerr << "  create - Creates initial hash database\n";

        std::cerr << "  check  - Checks file system integrity\n";

        return 1;
    }

    std::string directory = argv[1];

    std::string mode = argv[2];

    std::string dbPath = (argc > 3) ? argv[3] : "watcher.db";

    Watcher checker(directory);

    try
    {

        if (mode == "create")
        {

            checker.createInitialDatabase(dbPath);
        }
        else if (mode == "check")
        {

            checker.checkIntegrity(dbPath);
        }
        else
        {

            std::cerr << "Invalid mode. Use 'create' or 'check'.\n";

            return 1;
        }
    }
    catch (const std::filesystem::filesystem_error &e)
    {

        std::cerr << "Filesystem error: " << e.what() << std::endl;

        return 1;
    }
    catch (const std::exception &e)
    {

        std::cerr << "Error: " << e.what() << std::endl;

        return 1;
    }

    return 0;
}