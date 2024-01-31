#include <vector>
#include <string>

class JenkinsFactory: public Writer
{
    public:
    std::string name = "jenkins";

    private:
    std::string imageName = "jenkins/jenkins:lts";
    std::string picUrl = "https://e7.pngegg.com/pngimages/815/9/png-clipart-jenkins-docker-continuous-delivery-installation-software-deployment-github-fictional-character-plugin.png";
    std::vector<std::string> volumes = {"'$JENKINS_DATA:/var/jenkins_home'", "'/var/run/docker.sock:/var/run/docker.sock'"};
    std::vector<int> ports = {8080, 50000};

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