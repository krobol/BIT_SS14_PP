#ifndef ALGORITHMCONFIG_H
#define ALGORITHMCONFIG_H

#include <iostream>
#include <map>
#include <memory>

struct ConfigValueDescription
{
    ConfigValueDescription(std::string _name, int _precision = 0, float _min = 1, float _max = 10) :
        name(_name),
        precision(_precision),
        min(_min),
        max(_max)
    {
    }

    std::string name;   // Name der angezeigt wird
    int precision;      // Wie viele kommastellen hat der Wert
    float min;            // Wie hoch darf der Wert minimal sein
    float max;            // Wie hoch darf der Wert maximal sein
};
typedef std::shared_ptr<ConfigValueDescription> ConfigValueDescriptionPtr;

class AlgorithmConfig
{
public:
    void addParameter(std::string param, ConfigValueDescription description)
    {
        if(DescriptionMap.find(param) == DescriptionMap.end())
        {
            DescriptionMap.insert(std::pair<std::string, ConfigValueDescriptionPtr>(param, ConfigValueDescriptionPtr(new ConfigValueDescription(description))));
            ValueMap.insert(std::pair<std::string, float>(param, 0.0f));
        }
        else
        {
            DescriptionMap[param] = ConfigValueDescriptionPtr(new ConfigValueDescription(description));
        }
    }

    float getValue(std::string param)
    {
        if(ValueMap.find(param) != ValueMap.end())
        {
            return ValueMap[param];
        }
        else
        {
            std::cout << "Undefinierter Optionswert wurde gelesen!";
            return -1.0f;
        }
    }

    void setValue(std::string param, float value)
    {
        if(ValueMap.find(param) != ValueMap.end())
        {
            ValueMap[param] = value;
        }
        else
        {
            std::cout << "Undefinierter Optionswert wurde beschrieben!";
        }
    }

    const std::map<std::string, ConfigValueDescriptionPtr>& getConfigDescriptionMap() const
    {
        return DescriptionMap;
    }

private:
    std::map<std::string, ConfigValueDescriptionPtr> DescriptionMap;
    std::map<std::string, float> ValueMap;
};


#endif // ALGORITHMCONFIG_H
