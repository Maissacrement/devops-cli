#include <vector>
#include <string>

class AlertmanagerFactory: public Writer
{
    public:
    std::string name = "alertmanager";

    private:
    std::string imageName = "prom/alertmanager";
    std::string picUrl = "https://coralogix.com/wp-content/uploads/2021/03/Prometheus-1000X1000.png";
    std::vector<std::string> volumes = {};
    std::vector<int> ports = {9093};

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