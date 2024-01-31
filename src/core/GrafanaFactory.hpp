#include <vector>
#include <string>

class GrafanaFactory: public Writer
{
    public:
    std::string name = "grafana";

    private:
    std::string imageName = "grafana/grafana";
    std::string picUrl = "https://docs.checkmk.com/latest/images/grafana_logo.png";
    std::vector<std::string> volumes = {
        "'$GRAFANA_CONF/dashboards:/etc/grafana/provisioning/dashboards'",
        "'$GRAFANA_CONF/datasources:/etc/grafana/provisioning/datasources'"
    };
    std::vector<int> ports = {3000};

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