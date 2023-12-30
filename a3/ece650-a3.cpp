#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <csignal>
#include <limits>
#include <termios.h>
#include <cstring>


int main (int argc, char **argv) {
    //int pipe_01[2];
    int pipe_12[2];
    int pipe_23[2];
    //pipe(pipe_01);
    pipe(pipe_12);
    pipe(pipe_23);
    pid_t pid_1;
    pid_t pid_2;
    pid_t pid_3;
    int status1;
    int status2;
    int status3;
    pid_1=fork();
    if (pid_1==0){
        //close(pipe_01[0]);
        //./rgen and parsing data to the pipe
        close(pipe_12[0]);
        close(pipe_23[0]);
        close(pipe_23[1]);
        dup2(pipe_12[1], STDOUT_FILENO);
        close (pipe_12[1]);
        execvp("./rgen",argv);
        perror("execvp");
        return 1;
        waitpid(pid_1, &status1, 0);
        

    }
    pid_2=fork();
    if (pid_2==0){
        //run a1 and receving data from pipe_01
        close(pipe_12[1]);
        close(pipe_23[0]);
        //close(pipe_23[1]);
        dup2(pipe_12[0], STDIN_FILENO);
        close(pipe_12[0]);
        dup2(pipe_23[1], STDOUT_FILENO);
        close(pipe_23[1]);
        execlp("python3","python3","ece650-a1.py",nullptr);
        perror("execlp");
        return 1;
        waitpid(pid_2, &status2, 0);
        
    }
    pid_3=fork();
    if (pid_3==0){
        close(pipe_12[0]);
        close(pipe_12[1]);
        close(pipe_23[1]);
        dup2(pipe_23[0], STDIN_FILENO);
        close(pipe_23[0]);
        execvp("./ece650-a2",nullptr);
        perror("execvp");
        return 1;
        waitpid(pid_3, &status3, 0);
    }
    std::string line;
    //non-blocking input https://blog.csdn.net/qq_33471732/article/details/130095655
    while(!std::cin.eof()){
        waitpid(pid_1, &status1, WNOHANG);
        if(WIFEXITED(status1)){
            break;
        }
        fd_set read_fd;
        FD_ZERO(&read_fd);
        FD_SET(STDIN_FILENO, &read_fd);
        timeval time_wait;
        time_wait.tv_sec = 0;
        time_wait.tv_usec = 0;
        int read_line=select(STDIN_FILENO+1, &read_fd, NULL, NULL, &time_wait);
        if (read_line==-1){
            perror("read error");
            break;
        }
        if (read_line>0 && FD_ISSET(STDIN_FILENO, &read_fd)){
            char buffer[1024];
            close(pipe_23[0]);
            ssize_t read_size=read(STDIN_FILENO, buffer, sizeof(buffer)-1);
            if (read_size>0){
                buffer[read_size]='\0';
                line+=buffer;
                if (strchr(buffer,'\n')!=NULL){
                    line.pop_back();
                    dup2(pipe_23[1],STDOUT_FILENO);
                    std::cout<<line<<std::endl;
                    line.clear();
                }
                /*
                size_t change_line=line.find('\n');
                while (change_line!=std::string::npos){
                    std::string line_1=line.substr(0,change_line);
                    dup2(pipe_23[1], STDOUT_FILENO);
                    std::cout<<line_1<<std::endl;
                    //waitpid(pid_1, &status1, WNOHANG);
                    line=line.substr(change_line+1);
                    change_line=line.find('\n');
                }
*/
            }    
            
        }
    }
    
    close(pipe_12[0]);
    close(pipe_12[1]);
    close(pipe_23[0]);
    close(pipe_23[1]);
    kill(pid_1, SIGTERM);
    waitpid(pid_1, nullptr, 0);
    kill(pid_2, SIGTERM);
    waitpid(pid_2, nullptr, 0);
    kill(pid_3, SIGTERM);
    waitpid(pid_3, nullptr, 0);


    return 0;
}
