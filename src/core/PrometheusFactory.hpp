#include <string>

class PrometheusFactory: public Writer
{
    public:
    std::string name = "prometheus";

    private:
    std::string imageName = "prom/prometheus";
    std::string picUrl = "https://user-images.githubusercontent.com/92439/89548648-938bba00-d807-11ea-9ded-624678640bf2.png";
    std::vector<std::string> volumes = {"'$CONF/prometheus.yml:/etc/prometheus/prometheus.yml'"};
    std::vector<int> ports = {9090};

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