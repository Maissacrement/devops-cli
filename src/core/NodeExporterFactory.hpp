#include <vector>
#include <string>

class NodeExporterFactory: public Writer
{
    public:
    std::string name = "node-exporter";

    private:
    std::string imageName = "quay.io/prometheus/node-exporter:latest";
    std::string picUrl = "https://gitlab.developers.cam.ac.uk/uploads/-/system/project/avatar/1783/node_exporter_logo.png";
    std::vector<std::string> volumes = {
        "'/:/rootfs:ro'",
        "'/var/run:/var/run:ro'",
        "'/sys:/sys:ro'",
        "'/var/lib/docker/:/var/lib/docker:ro'",
        "'/cgroup:/cgroup:ro'",
        "'/etc/localtime:/etc/localtime:ro'"
    };
    std::vector<int> ports = {9100};

    public:

    std::string getName()
    {
        return this->name;
    }

    std::string getImageName()
    {
        return this->imageName;
    }

    std::string getPictureURL()
    {
        return this->picUrl;
    }

    std::string generate()
    {
        return 
          this->name + 
          this->sectionDefinition + 
          this->image(this->imageName) + 
          this->containername(this->name) + 
          this->hostname() + 
          this->getPorts(ports) + 
          this->getVolume(volumes) + "\n";
    }

    private:

    std::string hostname()
    {
        return "";
    }
};