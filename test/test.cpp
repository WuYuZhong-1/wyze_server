#include <iostream>
#include "../depend/yaml-cpp/include/yaml.h"

//后续增加 spdlog 会进行替换
#define Cout   std::cout 


class TestYaml {
public:
    void operator()() {
        try{
            YAML::Node config = YAML::LoadFile("../test/config.yaml");

            Cout << "name = " << config["name"].as<std::string>() << std::endl;
            Cout << "sex = " << config["sex"].as<std::string>() << std::endl;
            Cout << "age = " << config["age"].as<int>() << std::endl;

            Cout << "c++ = " << config["skill"]["c++"].as<int>() << std::endl;
            for(auto it =  config["skill"].begin() ;
                    it != config["skill"].end(); ++it) {
                Cout << it->first << " = " << it->second << std::endl;
            }

            std::string str1 = "[1, 2, 3, 4]";
            YAML::Node node1 = YAML::Load(str1);
            Cout << str1 << " type is " << type_to_string(node1.Type()) << std::endl;

            std::string str2 = "wuyze";
            YAML::Node node2 = YAML::Load(str2);
            Cout << str2 << " type is " << type_to_string(node2.Type()) << std::endl;

            std::string str3 = "{name: wyze, id: 1}";
            YAML::Node node3 = YAML::Load(str3);
            Cout << str3 << " type is " << type_to_string(node3.Type()) << std::endl;
        }
        catch(...) {
            Cout << "error\n" ;
        }
    }

private:
    std::string type_to_string(YAML::NodeType::value value) {
        switch(value) {
            case YAML::NodeType::Undefined:
                return {"Undefined"}; 
            case YAML::NodeType::Null:
                return {"Null"};
            case YAML::NodeType::Scalar:
                return {"Scalar"};
            case YAML::NodeType::Sequence:
                return {"Sequence"};
            case YAML::NodeType::Map:
                return {"Map"};
        }
        return "";
    }
};


int main(int argc, char** argv)
{
    TestYaml()();
    return 0;
}