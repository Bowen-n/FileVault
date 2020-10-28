#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

#include <sys/socket.h>
#include <linux/netlink.h>

#include "utils.h"
// CONSTANTS
#define MAX_PAYLOAD 1024
#define COMMAND_MAX_LEN 40
#define NETLINK_SAFEBOX 29
#define MAX_PATH_LEN 256

#define CMD_CD "cd"
#define CMD_LS "ls"
#define CMD_EXIT "exit"

// GLOBALS
int sock_fd;
struct msghdr msg;
struct nlmsghdr *nlh = NULL;
struct sockaddr_nl src_addr, dest_addr;
struct iovec iov;
static char pwd[MAX_PATH_LEN];
static char root[MAX_PATH_LEN];
static char display_pwd[MAX_PATH_LEN];
static char delim[] = " ";

// send pid to kernel
void send_pid()
{
    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid = getpid();
    bind(sock_fd, (struct sockaddr*)&src_addr, sizeof(src_addr));

    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = 0; // for linux kernel
    dest_addr.nl_groups = 0;

    // netlink message header
    nlh = (struct nlmsghdr*)malloc(NLMSG_SPACE(MAX_PAYLOAD));
    memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
    nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
    nlh->nlmsg_pid = getpid();
    nlh->nlmsg_flags = 0;

    iov.iov_base = (void *)nlh;
    iov.iov_len = nlh->nlmsg_len;
    msg.msg_name = (void *)&dest_addr;
    msg.msg_namelen = sizeof(dest_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    // printf("Sending message to kernel\n");
    sendmsg(sock_fd, &msg, 0);
}

int exec_cd(char splited_cmd[][COMMAND_MAX_LEN], int cmd_count)
{
    // TODO
    printf("\n");
    return 0;
}

int _ls(char path[COMMAND_MAX_LEN])
{
    DIR* dp; struct dirent* ep;
    dp = opendir(path);
    if (dp != NULL)
    {
        while (ep=readdir(dp))
        {
            if(strcmp(ep->d_name, ".") == 0 || strcmp(ep->d_name, "..") == 0)
                continue;
            printf("%s  ", ep->d_name);
        }
        printf("\n");
        closedir(dp);
        return 0;
    }
    else
        return -1;
}

int exec_ls(char splited_cmd[][COMMAND_MAX_LEN], int cmd_count)
{
    switch(cmd_count)
    {
        // ls
        case 1:
        {
            int suc = _ls(pwd);
            if (suc == -1)
                printf("Couldn't open dir %s\n", display_pwd);
            break;
        } // case 1 

        // TODO: ls path
        // ls path
        case 2:
        {
            DIR* dp; struct dirent* ep;
            char ls_path[COMMAND_MAX_LEN]; memset(ls_path, 0, COMMAND_MAX_LEN);
            char display_path[COMMAND_MAX_LEN]; memset(display_path, 0, COMMAND_MAX_LEN);
            char target_path[COMMAND_MAX_LEN]; memset(target_path, 0, COMMAND_MAX_LEN);
            strcpy(ls_path, splited_cmd[1]);

            // ls_path is abspath
            if (strncmp(ls_path, "/", 1) == 0)
            {
                strcpy(target_path, pwd);
                strcat(target_path, ls_path);
                strcpy(display_path, display_pwd);
                strcat(display_path, ls_path);
                int suc = _ls(target_path);
                if (suc == -1)
                    printf("Couldn't open dir %s\n", display_path);
            }
            // ls_path is relpath
            else
            {
                strcpy(target_path, pwd);
                strcat(target_path, "/");
                strcat(target_path, ls_path); // /home/safebox/bowen/./ddd
                char norm[COMMAND_MAX_LEN]; memset(norm, 0, COMMAND_MAX_LEN);
                normpath(norm, target_path);
                if (strncmp(norm, root, strlen(root)) != 0)
                {
                    printf("Invalid dir.\n");
                    break;
                }
                
                strcpy(display_path, display_pwd);
                strcat(display_path, ls_path);
                char norm_display[COMMAND_MAX_LEN]; memset(norm_display, 0, COMMAND_MAX_LEN);
                normpath(norm_display, display_path);
                int suc = _ls(norm);
                if (suc == -1)
                    printf("Could't open dir %s\n", norm_display);
            }

            break;
        } // case 2

        default:
            printf("Command arguments error.\n");

    } // switch

    return 0;
}


int execute(char splited_cmd[][COMMAND_MAX_LEN], int cmd_count)
{
    char main_cmd[COMMAND_MAX_LEN]; memset(main_cmd, 0, COMMAND_MAX_LEN);
    strcpy(main_cmd, splited_cmd[0]);

    if(strcmp(main_cmd, CMD_CD) == 0)
        return exec_cd(splited_cmd, cmd_count);
    else if(strcmp(main_cmd, CMD_LS) == 0)
        return exec_ls(splited_cmd, cmd_count);
    else if(strcmp(main_cmd, CMD_EXIT) == 0)
        return -1;
    else
    {
        printf("Command not found.\n");
        return 0;
    }
}


int main(int argc, char *argv[])
{
    memset(pwd, 0, MAX_PATH_LEN);
    memset(root, 0, MAX_PATH_LEN); 
    memset(display_pwd, 0, MAX_PATH_LEN);
    strcpy(pwd, "/home/safebox/bowen"); // TODO: should be modified to current user safebox directory
    strcpy(root, "/home/safebox/bowen");
    strcpy(display_pwd, "");

    // socket
    sock_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_SAFEBOX);
    if (sock_fd < 0)
        return -1;

    // interactive terminal
    while(1)
    {
        // prompt
        printf("%s> ", display_pwd);

        // get command
        char command[COMMAND_MAX_LEN];
        char splited_cmd[5][COMMAND_MAX_LEN];
        memset(command, 0, COMMAND_MAX_LEN);
        scanf("%[^\n]", command); getchar();
        
        // split command
        char *token; int cmd_count = 0;
        for(token = strtok(command, delim); token != NULL; token = strtok(NULL, delim))
        {
		    strcpy(splited_cmd[cmd_count++], token);
            // printf("%s\n", token);
        }
        // printf("count: %d\n", cmd_count);
        
        send_pid(); // tell kernel the pid to enter the safebox
        int continue_ = execute(splited_cmd, cmd_count);

        if (continue_ == -1)
            break;

    } // while

    return 0;
}