#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <nlohmann/json.hpp>

#define FILE_NAME ".voy"

using json = nlohmann::json;
int NEXT_ID;

enum COMMANDS {
    HELP,
    INIT,
    LIST_ALL_ITEMS,
    LIST_TODO_ITEMS,
    ADD_ITEM,
    DELETE_ITEM,
    COMPLETE_ITEM
};
COMMANDS COMMAND;

COMMANDS ParseArguments(int argc, char* argv[])
{
    if(argc == 2)
    {
        if(!strcmp(argv[1], "init"))
        {
            return COMMANDS::INIT;
        }
        else if(!strcmp(argv[1], "help"))
        {
            return COMMANDS::HELP;
        }
        else if(!strcmp(argv[1], "todo"))
        {
            return COMMANDS::LIST_TODO_ITEMS;
        }
        else if(!strcmp(argv[1], "all"))
        {
            return COMMANDS::LIST_ALL_ITEMS;
        }
    }
    else if(argc == 3)
    {
        if(!strcmp(argv[1], "add"))
        {
            return COMMANDS::ADD_ITEM;
        }
        else if(!strcmp(argv[1], "rm"))
        {
            return COMMANDS::DELETE_ITEM;
        }
        else if(!strcmp(argv[1], "ft"))
        {
            return COMMANDS::COMPLETE_ITEM;
        }
    }
    return COMMANDS::HELP;
}

void LoadJson(json& j)
{
    std::ifstream inputFileStream(FILE_NAME);
    inputFileStream >> j;
}

void ListAllTasks(json& data)
{
    std::cout << "Showing all tasks: " << std::endl << std::endl;
    std::cout << "Tasks with status 'todo' are shown in green, completed tasks in red" << std::endl;
    std::cout << 
        "To complete a task use command \"<program> ft <id>\" where id is the id of the task." 
        << std::endl << std::endl;
    json tasks = data["missions"];
    for (json::iterator it=tasks.begin(); it != tasks.end(); it++)
    {
        if ((*it)["status"] == "todo")
        {
            std::cout << "\t\033[32m"<< (*it)["id"] << ". "  << (*it)["task"] << std::endl;
        }
        else
        {
            std::cout << "\t\033[31m"<< (*it)["id"] << ". "  << (*it)["task"] << std::endl;
        }
    }
    std::cout << std::endl << std::endl;
}

void ListTodoTasks(json& data)
{
    std::cout << "Showing todo tasks: " << std::endl << std::endl;
    std::cout << 
        "To complete a task use command \"<program> ft <id>\" where id is the id of the task." 
        << std::endl << std::endl;
    json tasks = data["missions"];
    for (json::iterator it=tasks.begin(); it != tasks.end(); it++)
    {
        if ((*it)["status"] == "todo")
        {
            std::cout << "\t\033[32m"<< (*it)["id"] << ". "  << (*it)["task"] << std::endl;
        }
    }
    std::cout << std::endl << std::endl;
}

void AddNewTask(json& data, std::string task)
{
    data["missions"].push_back(
        {
            {"id", NEXT_ID++},
            {"task", task},
            {"status", "todo"}
        }
    );

    std::ofstream outputFileStream(FILE_NAME);
    outputFileStream << std::setw(4) << data << std::endl;

    std::cout << "Added new task with id: " << NEXT_ID - 1 << std::endl;
}
void CompleteTask(json& data, int id)
{
    json& missions = data["missions"];
    for (json::iterator it=missions.begin(); it!=missions.end(); it++)
    {
        if ((*it)["id"] == id)
        {
            (*it)["status"] = "done"; 
            // TODO: rm when write functionality has been separated
            std::ofstream outputFileStream(FILE_NAME);
            outputFileStream << std::setw(4) << data << std::endl;
            break;
        }
    }
    std::cout << "Completed task with id: " << id << std::endl;
}

void DeleteTask(json& data, int taskId)
{
    json& missions = data["missions"]; 

    for(json::iterator it=missions.begin(); it!=missions.end(); it++)
    {
        if((*it)["id"] == taskId)
        {
            missions.erase(it);
            // TODO: rm when write functionality has been separated
            std::ofstream outputFileStream(FILE_NAME);
            outputFileStream << std::setw(4) << data << std::endl;
        }
    }

    std::cout << "Deleted task with id: " << taskId << std::endl;
}

void LoadTaskId(json& j)
{
    if(j["missions"].size() > 0)
    {
        json::iterator it = j["missions"].end() - 1;
        NEXT_ID = (*it)["id"];
        NEXT_ID++;
    }
    else
    {
        NEXT_ID = 1;
    }
}

void OutputHelp()
{
    std::cout << "Usage: voy <command> [<args>]" << std::endl << std::endl;

    std::cout << "Commands for initializing the mission log:" << std::endl;
    std::cout << "\tinit\tCreate an empty todolist" << std::endl;
    std::cout << std::endl;
    std::cout << "Commands for changing the mission log:" << std::endl;
    std::cout << "\tadd\t<arg:task>\tAdd a new task to the list" << std::endl;
    std::cout << "\trm\t<arg:id>\tRemove a task from the list" << std::endl;
    std::cout << "\tft\t<arg:id>\tFinish a task (change its status to done)" << std::endl;
    std::cout << std::endl;
    std::cout << "Commands for viewing the mission log:" << std::endl;
    std::cout << "\ttodo\tShow all unfinished tasks" << std::endl;
    std::cout << "\tall\tShow all tasks" << std::endl;
    std::cout << std::endl;
}

bool fileExists(const char* filename)
{
    std::ifstream ifile(filename);
    return (bool) ifile;
}


int main(int argc, char* argv[])
{
    COMMAND = ParseArguments(argc, argv);  
    if(COMMAND == COMMANDS::HELP)
    {
        OutputHelp();
        exit(0);
    }

    if(COMMAND == COMMANDS::INIT)
    {
        json d;
        d["missions"] = json::array();
        std::ofstream outputFileStream(FILE_NAME);
        outputFileStream << std::setw(4) << d << std::endl;
        std::cout << "New todolist initialized, use \"voy add <task>\" to add tasks." << std::endl;
        exit(0);
    }

    if (!fileExists(FILE_NAME))
    {
        std::cout << "No list detected, run 'init'  to create a new list." << std::endl;
        exit(0);
    }

    json j;
    LoadJson(j);
    LoadTaskId(j);

    switch(COMMAND)
    {
        case COMMANDS::LIST_ALL_ITEMS:
            ListAllTasks(j);
            break;
        case COMMANDS::LIST_TODO_ITEMS:
            ListTodoTasks(j);
            break;
        case COMMANDS::ADD_ITEM:
            AddNewTask(j, argv[2]);
            break;
        case COMMANDS::DELETE_ITEM:
            DeleteTask(j, atoi(argv[2]));
            break;
        case COMMANDS::COMPLETE_ITEM:
            CompleteTask(j, atoi(argv[2]));
            break;
        case COMMANDS::HELP:
            break;
        case COMMANDS::INIT:
            break;
    }

    exit(0);
}
