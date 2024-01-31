#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <boost/asio.hpp>
#include <boost/program_options.hpp>
#include <map>
#include <functional>
#include <nlohmann/json.hpp>
#include <pcap.h>

#include "core/Writer.hpp"
#include "core/GitlabCEFactory.hpp"
#include "core/JenkinsFactory.hpp"
#include "core/PrometheusFactory.hpp"
#include "core/AlertmanagerFactory.hpp"
#include "core/GrafanaFactory.hpp"
#include "core/NodeExporterFactory.hpp"
#include "core/CadavisorFactory.hpp"
#include "html/indexHTML.hpp"

#include <netinet/in.h>
#include <arpa/inet.h>

struct NetworkInfo {
    std::string interfaceName;
    std::string ipAddress;
    std::string cidr;
};

namespace po = boost::program_options;

using namespace std;
using namespace boost::asio;
using ip::tcp;
using boost::asio::ip::tcp;
using json = nlohmann::json;

std::string generateResponse(const std::string& content) {
    return "HTTP/1.1 200 OK\r\nContent-Length: " + std::to_string(content.length()) + "\r\n\r\n" + content;
}

std::string generateJsonResponse(const std::string& content) {
    return "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " + std::to_string(content.length()) + "\r\n\r\n" + content;
}

std::string handleGetRequest(const std::string& request_path) {
    std::string root_path = "/home/zero/Documents/Entreprise/devops/src/html";  // Change this to the path of your HTML files folder.
    // Check if the requested file exists in the folder.
    std::ifstream file(root_path + request_path);
    if (file) {
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        return generateResponse(content);
    } else {
        // File not found.
        return "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
    }
}

void generateDockerComposeYaml(
    std::vector<std::string> choice
) {
    /*Network network;
    std::vector<std::string> ports = network.getUsedPorts();
    std::vector<int> dockerYamlPors;

    std::cout << "Ports utilisés : ";
    for (const int& port : ports) {
        std::cout << port << " ";
    }*/
  
    Writer* services[] = {
      new GitlabCEFactory(), new JenkinsFactory(), new PrometheusFactory(),
      new AlertmanagerFactory(), new GrafanaFactory(), new NodeExporterFactory(),
      new CadavisorFactory(),
    };
  
    ofstream myfile;
    myfile.open ("docker-compose.yml");
    myfile << "version: '3.9'\n\n";
    myfile << "services: \n\n";
  
    for (Writer* svc : services) {
      // void
    
      for (const std::string& c : choice) {
          if (svc->getName() == c) {
              myfile << svc->space << svc->generate();
          }
      }
    }
    
    myfile.close();

    //internalDePortNonDefinis();

    cout << "\nDone !\n";
}

const int max_length = 1024;

json parse_json(const std::string& json_str) {
    try {
        return json::parse(json_str);
    } catch (const json::exception& e) {
        std::cerr << "JSON parsing error: " << e.what() << std::endl;
        // Handle the parsing error as needed
    }
    return json();
}

void handle_request(tcp::socket& socket, boost::asio::streambuf& request) {
    try {
        // Extract the headers, if needed
        std::string headers(boost::asio::buffers_begin(request.data()), boost::asio::buffers_begin(request.data()) + request.size());
        // Read the request body
        std::string body;
        while (true) {
            boost::array<char, 1024> buf;
            boost::system::error_code error;
            size_t length = socket.read_some(boost::asio::buffer(buf), error);
            if (error == boost::asio::error::eof) {
                break; // End of data
            } else if (error) {
                throw boost::system::system_error(error); // Handle other errors
            }
            body.append(buf.data(), length);
        }
        // Access and use the request body
        std::cout << "Received request body: " << body << std::endl;

    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

std::string getService(const std::string& pic, const std::string& name, const std::string& img) {
    return "{\"picture_url\": \""+ pic + "\" ,\"name\": \""+ name + "\", \"image\": \"" + img + "\" }";;
}

std::vector<NetworkInfo> linuxInterfaces() {
    char errbuf[PCAP_ERRBUF_SIZE];

    // Get a list of available network interfaces
    pcap_if_t *alldevs;
    if (pcap_findalldevs(&alldevs, errbuf) == -1) {
        std::cerr << "Error finding network devices: " << errbuf << std::endl;
        // return 1;
    }

    // Create a vector to store network information
    std::vector<NetworkInfo> networkInfoList;

    // Loop through the list of network interfaces
    pcap_if_t *dev;
    for (dev = alldevs; dev != nullptr; dev = dev->next) {
        // Loop through the addresses associated with this interface
        for (pcap_addr_t *addr = dev->addresses; addr != nullptr; addr = addr->next) {
            if (addr->addr == nullptr)
                continue;

            if (addr->addr->sa_family == AF_INET) {
                struct sockaddr_in *ipv4 = reinterpret_cast<struct sockaddr_in*>(addr->addr);
                struct sockaddr_in *subnet = reinterpret_cast<struct sockaddr_in*>(addr->netmask);
                char ipAddr[INET_ADDRSTRLEN];

                // Convert the IP address to a string
                inet_ntop(AF_INET, &(ipv4->sin_addr), ipAddr, INET_ADDRSTRLEN);

                // Calculate CIDR notation
                unsigned int prefixLength = __builtin_popcountl(subnet->sin_addr.s_addr);
                std::string cidr = ipAddr + ("/" + std::to_string(prefixLength));

                // Store network information in the vector
                NetworkInfo info;
                info.interfaceName = dev->name;
                info.ipAddress = ipAddr;
                info.cidr = cidr;
                networkInfoList.push_back(info);
            }
        }
    }

    // Free the list when done
    pcap_freealldevs(alldevs);

    // Print the stored network information
    /*for (const auto& info : networkInfoList) {
        std::cout << "Interface Name: " << info.interfaceName << std::endl;
        std::cout << "IP Address: " << info.ipAddress << std::endl;
        std::cout << "CIDR: " << info.cidr << std::endl;
        std::cout << std::endl;
    }*/

    return networkInfoList;
}

int main(int argc, char* argv[]) {
  std::vector<NetworkInfo> interfaces = linuxInterfaces();
  
  std::size_t servicesArraySize = 7;
  Writer* services[] = {
    new GitlabCEFactory(), new JenkinsFactory(), new PrometheusFactory(),
    new AlertmanagerFactory(), new GrafanaFactory(), new NodeExporterFactory(),
    new CadavisorFactory()
  };
  
  std::vector<std::string> allChoice = {"gitlabce", "jenkins", "prometheus", "alertmanager", "grafana", "node-exporter", "cadvisor"};
  std::string allServices = "generate docker-compose.yaml: \n\t";
  std::string space = " ";

  char charArray[100]; // Assuming a sufficiently large buffer

  for (const std::string& choice : allChoice) {
    allServices+= choice + space;
  }

  // Copy the contents of the std::string to the char array
  strcpy(charArray, allServices.c_str());

  po::options_description desc("Allowed options");
  desc.add_options()
      ("help", "Produce help message")
      ("generate-yaml", po::value<std::vector<std::string>>()->multitoken(), charArray)
      ("port", po::value<int>(), "Internal server port");

  po::variables_map vm;

  try {
      // Parse command-line arguments
      po::store(po::parse_command_line(argc, argv, desc), vm);
      po::notify(vm);
  } catch (const po::error& ex) {
      std::cerr << "Error: " << ex.what() << std::endl;
      return 1;
  }  
  // Check for the presence of specific flags and process accordingly.
  if (vm.count("help")) {
      std::cout << desc << std::endl;
      return 0;
  }

  if (vm.count("generate-yaml")) {
      // std::vector<std::string> allChoice = {"gitlabce", "jenkins", "prometheus", "alertmanager", "grafana", "node-exporter", "cadvisor"};
      std::vector<std::string> choice = vm["generate-yaml"].as<std::vector<std::string>>();
      generateDockerComposeYaml(choice);
      return 0;
  }

  int server_port = 8081;
  if (vm.count("port")) {
    server_port = vm["port"].as<int>();
  }

  system("clear");
  std::cout << "Listening on port " << server_port << " ..." << std::endl;

  io_service io;
  tcp::acceptor acceptor(io, tcp::endpoint(tcp::v4(), server_port));

  while (true) {
      // Server DEFINITION
      tcp::socket socket(io);
      acceptor.accept(socket);
      boost::system::error_code ignored_error;
      boost::asio::streambuf request;
      read_until(socket, request, "\r\n\r\n", ignored_error);
      std::istream request_stream(&request);
      std::string request_header;
      getline(request_stream, request_header);

      // GET HEADER
      std::string header;
      if (!ignored_error) {
        while (std::getline(request_stream, header) && header != "\r") {
            // You can inspect headers here, but it's not required for JSON handling.
            std::cout << header << "\n";
        }
      }
      
      // GET METHOD
      if (request_header.substr(0, 3) == "GET") {
          // Parse the request path from the HTTP GET request.
          size_t start = request_header.find(" ") + 1;
          size_t end = request_header.find(" ", start);
          std::string request_path = request_header.substr(start, end - start);
          if (request_path == (std::string) "/" || request_path == "/index.html") {
            // if html file is not compiled
            request_path = "/index.html";
            // handleGetRequest("/index.html");
            // end if
            std::string html(reinterpret_cast<char*>(src_html_index_html), src_html_index_html_len);
            std::string response = generateResponse(html);// handleGetRequest("/index.html"); //
            write(socket, buffer(response), ignored_error);
          } 
          else if(request_path == "/services/getall") {
            std::string data = "[";
            for (Writer* svc : services) {
                std::string name = svc->getName();
                data += getService(svc->getPictureURL(), name, svc->getImageName());
                if (name != services[servicesArraySize - 1]->getName()) data+=",";
            }
            data+="]";
            
            std::string response = generateJsonResponse("{ \"status\": \"200\", \"data\": " + data + "}");
            write(socket, buffer(response), ignored_error);
          }
      }
      else if (request_header.substr(0, 4) == "POST") {
          size_t start = request_header.find(" ") + 1;
          size_t end = request_header.find(" ", start);
          std::string request_path = request_header.substr(start, end - start);
          if(request_path == "/services/download") {
              std::string request_string(boost::asio::buffers_begin(request.data()), boost::asio::buffers_end(request.data()));

              // Print the string
              std::cout << "Request as string: " << request_string << std::endl;
              json body = parse_json(request_string);
              std::vector<std::string> cmd = body;
              generateDockerComposeYaml(cmd); 

              // Read the requested file (you should validate the file path)
              std::string file_path = "docker-compose.yml";
              std::ifstream file(file_path, std::ios::binary);  
              if (file) {
                  file.seekg(0, std::ios::end);
                  std::streampos fileSize = file.tellg();

                  // Send the file as a response
                  std::string response = "HTTP/1.1 200 OK\r\n";
                  response += "Content-Type: application/octet-stream\r\n";
                  response += "Content-Disposition: attachment; filename=\"docker-compose.yml\"\r\n";
                  response += "Content-Length: " + std::to_string(fileSize) + "\r\n\r\n"; 

                  std::cout << "[RESPONSE]: \n" << response;
                  
                  // Send the response header
                  boost::asio::write(socket, boost::asio::buffer(response));  
                  // Send the file content
                  char bufferr[2048];
                  std::streamsize bytesRead;

                  file.seekg(0, std::ios::beg);
                  bytesRead = file.readsome(bufferr, sizeof(bufferr));
                  file.seekg(0, std::ios::beg);
                  file.close();
                  
                  boost::asio::write(socket, boost::asio::buffer(bufferr, bytesRead), ignored_error);
                  
                  
              } else {
                  // Handle file not found or other errors
                  std::string error_response = "HTTP/1.1 404 Not Found\r\n\r\nFile not found";
                  boost::asio::write(socket, boost::asio::buffer(error_response));
              }
              
              
              //handle_request(socket, request);
              //std::string response = generateJsonResponse("{ \"status\": \"200\" }");
              //write(socket, buffer(response), ignored_error);
          } else {
              std::string response = generateResponse("Nothing here");
              write(socket, buffer(response), ignored_error);
          }
      }
  }

  return 0;
}