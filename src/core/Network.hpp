#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdint>

class Network
{
    public:

    // Function to convert a decimal number to an IPv4 address
    std::string decimalToIPv4(uint32_t decimal) {
        uint8_t bytes[4];
        bytes[0] = (decimal >> 24) & 0xFF;
        bytes[1] = (decimal >> 16) & 0xFF;
        bytes[2] = (decimal >> 8) & 0xFF;
        bytes[3] = decimal & 0xFF;

        std::stringstream ss;
        ss << static_cast<int>(bytes[3]) << '.' << static_cast<int>(bytes[2]) << '.'
        << static_cast<int>(bytes[1]) << '.' << static_cast<int>(bytes[0]);

        return ss.str();
    }

    std::vector<int> getUsedPorts()
    {
        std::ifstream tcpFile("/proc/net/tcp");
        std::string line;
        if (!tcpFile.is_open()) {
            std::cerr << "Impossible d'ouvrir le fichier /proc/net/tcp." << std::endl;
            return {};
        }
        std::vector<int> usedPorts;
        // Ignorer la première ligne d'en-têtes
        std::getline(tcpFile, line);
        while (std::getline(tcpFile, line)) {
            
            std::istringstream iss(line);
            std::string token;
            iss >> token;
            iss >> token;
            // Le format de l'adresse IP et du port local est sous la forme "0100007F:AABB"
            // où "0100007F" est l'adresse IP en hexadécimal et "AABB" est le port en hexadécimal.
            // Nous devons extraire le port et le convertir en décimal.
            size_t colonPos = token.find(':');
            
            if (colonPos != std::string::npos) {
                std::string ipHex = token.substr(0, colonPos);
                std::string portHex = token.substr(colonPos + 1);
                //int ipDecimal, 
                int portDecimal;
                //std::stringstream(ipHex) >> std::hex >> ipDecimal;
                std::stringstream(portHex) >> std::hex >> portDecimal;

                //std::string res = this->decimalToIPv4(static_cast<uint32_t>(ipDecimal));
                
                usedPorts.push_back(portDecimal);
            }
        }
        tcpFile.close();
        
        return usedPorts;
    }
};