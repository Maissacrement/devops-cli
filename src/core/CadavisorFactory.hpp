#include <vector>
#include <string>

class CadavisorFactory: public Writer
{
    public:
    std::string name = "cadvisor";

    private:
    std::string imageName = "gcr.io/cadvisor/cadvisor:latest";
    std::string picUrl = "https://neellik.com/wp-content/uploads/2022/03/cadvisor.png";
    std::vector<std::string> volumes = {
       "'/:/rootfs:ro'",
       "'/var/run:/var/run:rw'",
       "'/sys:/sys:ro'",
       "'/var/lib/docker:/var/lib/docker:ro'",
       "'/sys/fs/cgroup:/cgroup:ro'",
       "'/dev/disk:/dev/disk:ro'",
    };
    std::vector<int> ports = {8080};

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