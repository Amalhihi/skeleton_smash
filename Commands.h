#ifndef SMASH_COMMAND_H_
#define SMASH_COMMAND_H_


#include <time.h>
#include <vector>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <memory>
#include "signal.h"
#include <unistd.h>
#include <linux/limits.h>


#define COMMAND_ARGS_MAX_LENGTH (200)
#define COMMAND_MAX_ARGS (20)

class Command {
public:
    std::string cmd_line;
    std::vector<std::string> arguments;
    int num_of_arguments;
    //bool ignoring_background;
    Command(const char* _cmd_line);
    virtual ~Command()=default;
    virtual void execute() = 0;
    //virtual void prepare();
    //virtual void cleanup();
    // TODO: Add your extra methods if needed
};

class BuiltInCommand : public Command {
public:
    BuiltInCommand(const char* cmd_line);
    virtual ~BuiltInCommand() {}
};

class ExternalCommand : public Command {
    bool is_complex;
    bool is_backGround;
    bool is_alarm;
public:
    ExternalCommand(const char* cmd_line,bool is_alarm);
    virtual ~ExternalCommand() {}
    void execute() override;
    void is_it_complex(){
        is_complex= (cmd_line.find("*")!=std::string::npos||cmd_line.find("?")!=std::string::npos);
    }
    void is_it_backGround(){
        char * last_character;
        (* last_character)= (this->arguments[arguments.size() - 1].back());
        is_backGround= ((last_character)=="&")?true:false;

    }
};

class PipeCommand : public Command {
 const   char* command1;
  const   char* command2;
    bool is_stderr_to_write;
    // TODO: Add your data members
public:
    PipeCommand(const char* cmd_line);
    virtual ~PipeCommand() {}
    void execute() override;
    void is_it_stderr_to_write(){
        is_stderr_to_write= (cmd_line.find("|&")!=std::string::npos);
    }
};

class RedirectionCommand : public Command {
    // TODO: Add your data members

    //bool is_override;
    bool is_append;
    std::string command;
    std::string file_name;
    bool is_alarm;
    int position;
   // bool is_backGround;
public:
    explicit RedirectionCommand(const char* cmd_line,bool is_alarm,bool is_append,int position);
    virtual ~RedirectionCommand() {}
    void execute() override;
    //void prepare() override;
    //void cleanup() override;
};
//////////////////////////helper function to redirection ////////////////////
bool IsBuiltInCommand(const std::string& command_name);

class ChpromptCommand : public BuiltInCommand {
    // TODO: Add your data members
public:
    ChpromptCommand(const char* cmd_line);
    virtual ~ChpromptCommand() {}
    void execute() override;
};

class ChangeDirCommand : public BuiltInCommand {
public:
   // std::string last_Pwd;
// TODO: Add your data members public:
    ChangeDirCommand(const char* cmd_line);
    virtual ~ChangeDirCommand() {}
    void execute() override;
};

class GetCurrDirCommand : public BuiltInCommand {
public:
    GetCurrDirCommand(const char* cmd_line);
    virtual ~GetCurrDirCommand() {}
    void execute() override;
};

class ShowPidCommand : public BuiltInCommand {
public:
    ShowPidCommand(const char* cmd_line);
    virtual ~ShowPidCommand() {}
    void execute() override;
};

class JobsList;
class QuitCommand : public BuiltInCommand {
    JobsList* jobs;
// TODO: Add your data members
public:
    QuitCommand(const char* cmd_line, JobsList* jobs);
    virtual ~QuitCommand() {}
    void execute() override;
};


class JobsList {
public:
    class JobEntry {
    public:
        // TODO: Add your data members
        std::string cmd_line;
        int JobID;
        pid_t Job_PID;
        time_t insertation_Time;//start time
        bool is_stopped;
      //  Command* my_command;
        JobEntry( std::string cmd_line,int JobID, pid_t Job_PID,time_t insertation_Time, bool is_stopped);//constructor

        ~JobEntry()=default;
    };
public:
    int Max_jobID;
    std::vector<JobEntry> jobs_List;
    JobsList();
    ~JobsList()=default;

    void addJob(Command *cmd,pid_t pid,bool isStopped = false);

    void printJobsList();

    void killAllJobs();

    void removeFinishedJobs();

    JobEntry * getJobById(int jobId);
    JobEntry * getJobByPid(int jobPid);
    void removeJobById(int jobId);

    JobEntry * getLastJob(int *lastJobId);
    JobEntry *getLastStoppedJob(int *jobId);
    // TODO: Add extra methods or modify exisitng ones as needed
};

class JobsCommand : public BuiltInCommand {
    // TODO: Add your data members
    JobsList* jobs;
public:
    JobsCommand(const char* cmd_line, JobsList* jobs);
    virtual ~JobsCommand() {}
    void execute() override;
};

class ForegroundCommand : public BuiltInCommand {
    // TODO: Add your data members

public:
    JobsList* jobs;
    ForegroundCommand(const char* cmd_line, JobsList* jobs);
    virtual ~ForegroundCommand() {}
    void execute() override;
};

class BackgroundCommand : public BuiltInCommand {

    // TODO: Add your data members
public:
    JobsList* jobs;
    BackgroundCommand(const char* cmd_line, JobsList* jobs);
    virtual ~BackgroundCommand() {}
    void execute() override;
};

class KillCommand : public BuiltInCommand {
    /* Bonus */
    // TODO: Add your data members
    JobsList* jobs;
public:
    KillCommand(const char* cmd_line, JobsList* jobs);
    virtual ~KillCommand() {}
    void execute() override;
};

/*
class FareCommand : public BuiltInCommand {
public:
    FareCommand(const char* cmd_line);
    virtual ~FareCommand() {}
    void execute(bool is_alarm) override;
};

class SetcoreCommand : public BuiltInCommand {

public:
    SetcoreCommand(const char* cmd_line);
    virtual ~SetcoreCommand() {}
    void execute(bool is_alarm) override;
};
*/

class TimeoutCommand : public BuiltInCommand {
    std::string my_cmd;
    time_t durration;
// TODO: Add your data members
public:
    explicit TimeoutCommand(const char* cmd_line);
    virtual ~TimeoutCommand() {}
    void execute() override;
};

class TimeOutList {
public:
    class Time_out_Data {
    public:
    public:
        ///who,when stated,when to kill,pid to kill
        pid_t my_pid;
        Command *command;
        time_t durration;
        time_t start_time;
        time_t kill_time;


        Time_out_Data(pid_t my_pid, Command *command, time_t durration) :
                my_pid(my_pid), command(command),durration(durration),start_time(time(nullptr)){
            kill_time=start_time+durration;
        }

        ~Time_out_Data()=default;

    };

    std::vector<Time_out_Data> T_list;
    void add_timeout_command(pid_t my_pid, Command *command){
        this->T_list.push_back(Time_out_Data(my_pid,command,time(nullptr)));

    }
//////////////////////////////unfinished

void add_alarm_to_list(pid_t my_pid, Command *command, time_t durration){

    }
    void delete_timeout_from_list(pid_t my_pid){


    }
    void end_finished_timeout_command(){

    }


};

class SmallShell {
    SmallShell() : cmd_line(""), shell_pid(getpid()), job_list(), smash_display_line("smash> "), pid_running_ForGround(-1){

        last_Pwd="";
    }
public:
    TimeOutList alarm_list;
    std::string cmd_line;
    pid_t shell_pid;
    JobsList job_list;
    std::string smash_display_line;
    pid_t pid_running_ForGround;
    Command*curr_command;
    std::string last_Pwd;

    Command *CreateCommand(const char* cmd_line,bool is_alarm=false);
    SmallShell(SmallShell const&)      = delete; // disable copy ctor
    void operator=(SmallShell const&)  = delete; // disable = operator
    static SmallShell& getInstance() // make SmallShell singleton
    {
        static SmallShell instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }
    ~SmallShell();
    void executeCommand(const char* cmd_line);
    // TODO: add extra methods as needed
};

#endif //SMASH_COMMAND_H_
