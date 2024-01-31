#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>
#include <random>
#include "Network.hpp"

class Writer
{
    public:
    std::string name = "";
    std::string imageName = "";
    std::string picUrl = "";
    std::string space = "  ";
    std::string fourspace = "    ";
    std::string sectionDefinition = ":\n";

    public:

    virtual std::string generate()
    {
        return "";
    }

    virtual std::string getName()
    {
        return this->name;
    }

    virtual std::string getImageName()
    {
        return this->imageName;
    }

    virtual std::string getPictureURL()
    {
        return this->picUrl;
    }

    std::string image(const std::string& name)
    {
        return this->fourspace + "image: " + name + "\n";
    }

    std::string containername(const std::string& name)
    {
        return this->fourspace + "container_name: " + name + "\n";
    }

    std::string getPorts(const std::vector<int>& exposedPort)
    {
        std::string portsDefinition = this->fourspace + "ports:\n";

        try {
            Network network;
            std::vector<std::string> ports = this->intervalDePortNonDefinis(network.getUsedPorts(), exposedPort);
            for(const std::string& port : ports) {
                portsDefinition+= this->fourspace + this->space + "- " +  port + "\n";
            }

        } catch (const std::invalid_argument& ia) {
            std::cerr << "Invalid argument: " << ia.what() << std::endl;
        } catch (const std::out_of_range& oor) {
            std::cerr << "Out of range error: " << oor.what() << std::endl;
        }
        
        return portsDefinition;
    }

    std::string getVolume(std::vector<std::string> volumes)
    {
        int i = 0;
        std::string volumesDefinition = "";
        for (const std::string& volume : volumes) {
            if (i==0) {
                volumesDefinition = this->fourspace + "volumes:\n";
                i+=1;
            }
            volumesDefinition+= this->fourspace + this->space + "- " +  volume + "\n";
        }

        return volumesDefinition;
    }

    private:

    // Fonction pour générer un nombre aléatoire dans un intervalle
    int randomPort(int start, int end) {
        static std::mt19937 mersenne(static_cast<unsigned int>(std::time(nullptr)));
        std::uniform_int_distribution<int> distribution(start, end);
        return distribution(mersenne);
    }

    // Fonction pour ajouter un port au tableau des ports définis
    void addPort(std::vector<int>& ports_definis, int port) {
        ports_definis.push_back(port);
        std::sort(ports_definis.begin(), ports_definis.end());
    }

    // Fonction pour diviser un intervalle en deux à partir d'un point donné
    void divideInterval(std::vector<std::pair<int, int>>& intervals, int start, int end) {
        for (auto it = intervals.begin(); it != intervals.end(); ++it) {
            if (start >= it->first && end <= it->second) {
                int old_end = it->second;
                it->second = start - 1;
                intervals.insert(it, std::make_pair(end + 1, old_end));
                break;
            }
        }
    }

    std::vector<std::string> intervalDePortNonDefinis(std::vector<int> ports_definis, std::vector<int> ports_voulus) {
        // Tableaux de ports définis et voulus
        std::vector<std::string> allPort;
        
        // Triez les tableaux
        std::sort(ports_definis.begin(), ports_definis.end());
        std::sort(ports_voulus.begin(), ports_voulus.end());

        // Créez un vecteur pour stocker les intervalles de ports non définis
        std::vector<std::pair<int, int>> intervals;

        // Ajoutez un intervalle initial couvrant l'ensemble de l'espace des ports
        intervals.push_back(std::make_pair(0, 65535));

        // Seed pour la génération aléatoire
        std::srand(static_cast<unsigned int>(std::time(nullptr)));

        // Parcourez les ports définis et excluez-les des intervalles pour créer les intervalles de ports non définis
        for (int port_defini : ports_definis) {
            for (auto it = intervals.begin(); it != intervals.end(); ++it) {
                if (port_defini >= it->first && port_defini <= it->second) {
                    int start = it->first;
                    int end = it->second;
                    intervals.erase(it);
                    
                    if (port_defini > start) {
                        intervals.insert(it, std::make_pair(start, port_defini - 1));
                    }
                    
                    if (port_defini < end) {
                        intervals.insert(it, std::make_pair(port_defini + 1, end));
                    }
                    
                    break;
                }
            }
        }

        // Parcourez les ports voulus
        for (int port_voulu : ports_voulus) {
            bool port_reattribue = false;
            int new_port = 0;
            
            // Vérifiez si le port voulu est déjà défini
            if (std::binary_search(ports_definis.begin(), ports_definis.end(), port_voulu)) {
                std::cout << "Le port " << port_voulu << " est déjà défini." << std::endl;
                
                // Choisissez un port aléatoire dans un intervalle disponible
                if (!intervals.empty()) {
                    int index = randomPort(0, intervals.size() - 1);
                    new_port = randomPort(intervals[index].first, intervals[index].second);
                    addPort(ports_definis, new_port);
                    std::cout << "Port " << new_port << " a été attribué au lieu de " << port_voulu << std::endl;
                    divideInterval(intervals, new_port, new_port); // Divisez l'intervalle avec le nouveau port
                    port_reattribue = true;
                } else {
                    std::cout << "Il n'y a pas d'intervalle disponible pour regénérer le port." << std::endl;
                }
            } else {
                // Parcourez les intervalles actuels
                for (auto it = intervals.begin(); it != intervals.end(); ++it) {
                    if (port_voulu >= it->first && port_voulu <= it->second) {
                        // Le port voulu se trouve dans cet intervalle, divisez-le en deux
                        int start = it->first;
                        int end = it->second;
                        intervals.erase(it);
                        
                        if (port_voulu > start) {
                            intervals.insert(it, std::make_pair(start, port_voulu - 1));
                        }
                        
                        if (port_voulu < end) {
                            intervals.insert(it, std::make_pair(port_voulu + 1, end));
                        }
                        
                        port_reattribue = true;
                        break;
                    }
                }
            }
            
            // Si le port a été réattribué, ajoutez-le au tableau des ports définis
            if (port_reattribue) {
                addPort(ports_definis, port_voulu);
            }

            std::string str_port = new_port != 0 ? 
                std::to_string(new_port) + ":" + std::to_string(port_voulu) : 
                std::to_string(port_voulu) + ":" + std::to_string(port_voulu); 

            // Affichez les intervalles de ports non définis
            /*std::cout << "Intervalles de ports non définis : ";
            for (auto interval : intervals) {
                std::cout << "[" << interval.first << ", " << interval.second << "] ";
            }*/
            allPort.push_back(str_port);
        }

        return allPort;
    }
};
