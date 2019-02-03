#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <nlohmann/json.hpp>

#include <stdio.h>
#include <unistd.h>
#define GetCurrentDir getcwd

using json = nlohmann::json;
int NEXT_ID;
enum COMMANDS {
    EXIT,
    LIST_PROFILES,
    LIST_TASKS_ALL,
    LIST_TASKS,
    ADD_TASK,
    FINISH_TASK,
    CREATE_PROFILE,
    DELETE_PROFILE
};

COMMANDS Command;

COMMANDS ParseArguments(int argc, char* argv[])
{
    if(argc == 2)
    {
        if(!strcmp(argv[1], "-lp"))
        {
            return COMMANDS::LIST_PROFILES;
        }
    }
    if(argc == 3)
    {
        if(!strcmp(argv[1], "-lt"))
        {
            return COMMANDS::LIST_TASKS;
        }
        else if(!strcmp(argv[1], "-np"))
        {
            return COMMANDS::CREATE_PROFILE;
        }
        else if(!strcmp(argv[1], "-dp"))
        {
            return COMMANDS::DELETE_PROFILE;
        }
    }
    if(argc == 4)
    {
        if(!strcmp(argv[1], "-lt"))
        {
            return COMMANDS::LIST_TASKS_ALL;
        }
        else if(!strcmp(argv[1], "-add"))
        {
            return COMMANDS::ADD_TASK;
        }
        else if(!strcmp(argv[1], "-ft"))
        {
            return COMMANDS::FINISH_TASK;
        }
    }
    return COMMANDS::EXIT;
}

void ListAllProfiles(json& data)
{
    json profiles = data["profiles"];
    for (json::iterator it=profiles.begin(); it != profiles.end(); it++)
    {
        std::cout << std::setw(4) << *it << std::endl; 
    }
}

void LoadJson(json& j)
{
    std::ifstream inputFileStream("data.json");
    inputFileStream >> j;
}

void ListTasksForProfile(json& data, std::string profile, std::string state)
{
    json tasks = data["todolists"];
    for (json::iterator it=tasks.begin(); it != tasks.end(); it++)
    {
        if ((*it)["profile"] == profile && state == "all")
        {
            //std::cout << (*it)["id"] << ": " << (*it)["task"] << "<" << (*it)["status"] << ">" << std::endl; 
            std::cout << std::setw(4) << *it << std::endl;
        }
        else if ((*it)["profile"] == profile && state == (*it)["status"])
        {
            //std::cout << (*it)["id"] << ": " << (*it)["task"] << "<" << (*it)["status"] << ">" << std::endl; 
            std::cout << std::setw(4) << *it << std::endl;
        }
    }
}

void AddNewTask(json& data, std::string profile, std::string task)
{
    data["todolists"].push_back(
        {
            {"id", NEXT_ID++},
            {"profile", profile},
            {"task", task},
            {"status", "todo"}
        }
    );

    std::ofstream outputFileStream("data.json");
    outputFileStream << std::setw(4) << data << std::endl;
}

void AddNewProfile(json& data, std::string profile)
{
    //TODO: ensure profile name is unique
    data["profiles"].push_back(profile);

    // TODO: extract write to separate function
    std::ofstream outputFileStream("data.json");
    outputFileStream << std::setw(4) << data << std::endl;
}

void CompleteTask(json& data, std::string profile, int id)
{
    json& tasks = data["todolists"];
    for (json::iterator it=tasks.begin(); it!=tasks.end(); it++)
    {
        if ((*it)["id"] == id && (*it)["profile"] == profile)
        {
            std::cout << "Found task" << std::endl;
            (*it)["status"] = "done"; 
            // TODO: rm when write functionality has been separated
            std::ofstream outputFileStream("data.json");
            outputFileStream << std::setw(4) << data << std::endl;
            break;
        }
    }
}

void DeleteProfile(json& data, std::string profile)
{
    std::vector<std::string> profiles = data["profiles"]; 

    std::vector<std::string>::iterator found = std::remove(profiles.begin(), profiles.end(), profile);
    if (found != profiles.end())
    {
        profiles.resize(profiles.size() - 1);
        data["profiles"] = profiles;
        // TODO: rm when write functionality has been separated
        std::ofstream outputFileStream("data.json");
        outputFileStream << std::setw(4) << data << std::endl;
    }

}

void LoadTaskId(json& j)
{
    json::iterator it = j["todolists"].end() - 1;
    NEXT_ID = (*it)["id"];
    NEXT_ID++;
}

void OutputHelp()
{
    std::cout << "HELP" << std::endl;
}

int main(int argc, char* argv[])
{
    char buff[FILENAME_MAX];
    GetCurrentDir(buff, FILENAME_MAX);
    std::string current_working_dir(buff);
    std::cout << current_working_dir << endl;
    json j; 
    LoadJson(j);
    LoadTaskId(j);

    if (argc == 1)
    {
        OutputHelp();
        exit(0);
    }

    Command = ParseArguments(argc, argv);
    switch(Command)
    {
        case COMMANDS::EXIT:
            OutputHelp();
            exit(0);
            break;
        case COMMANDS::LIST_PROFILES:
            ListAllProfiles(j);
            break;
        case COMMANDS::LIST_TASKS_ALL:
            ListTasksForProfile(j, argv[2], "all");
            break;
        case COMMANDS::LIST_TASKS:
            ListTasksForProfile(j, argv[2], "todo");
            break;
        case COMMANDS::ADD_TASK:
            AddNewTask(j, argv[2], argv[3]);
            break;
        case COMMANDS::FINISH_TASK:
            CompleteTask(j, argv[2], std::stoi(argv[3]));
            break;
        case COMMANDS::CREATE_PROFILE:
            AddNewProfile(j, argv[2]);
            break;
        case COMMANDS::DELETE_PROFILE:
            break;
            DeleteProfile(j, argv[2]);
    }

    //ListAllProfiles(j);
    //ListTasksForProfile(j, "profile1", "all");
    //AddNewTask(j, "profile1", "new task");
    //ListTasksForProfile(j, "profile1", "all");
    //CompleteTask(j, "profile1", 1);
    //AddNewProfile(j, "profile5");
    //DeleteProfile(j, "profile1");
    return 0;
}
