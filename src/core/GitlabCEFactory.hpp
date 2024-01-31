#include <vector>
#include <string>

class GitlabCEFactory: public Writer
{
    public:
    std::string name = "gitlabce";

    private:
    std::string imageName = "gitlab/gitlab-ce:latest";
    std::string picUrl = "https://humancoders-formations.s3.amazonaws.com/uploads/course/logo/155/formation-gitlab.png";
    std::vector<std::string> volumes = {
        "'$GITLAB_DATA/config:/etc/gitlab'",
        "'$GITLAB_DATA/logs:/var/log/gitlab'",
        "'$GITLAB_DATA/data:/var/opt/gitlab'"
    };
    std::vector<int> ports = {80, 443, 22};

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

    std::string generate() override
    {
        return 
          this->name + 
          this->sectionDefinition + 
          this->image(this->imageName) + 
          this->containername(this->name) + 
          this->hostname() + 
          this->getPorts(this->ports) + 
          this->getVolume(this->volumes) + "\n";
    }

    private:

    std::string hostname()
    {
        return this->fourspace + "hostname: 'gitlab.example.com'\n";
    }
};