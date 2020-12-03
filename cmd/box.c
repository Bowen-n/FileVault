#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <linux/netlink.h>

#include "user.h"
#include "base64.h"
#include "exec.h"

// CONSTANTS
#define MAX_PAYLOAD 1024
#define NETLINK_SAFEBOX 29

// GLOBALS
int sock_fd;
struct msghdr msg;
struct nlmsghdr *nlh = NULL;
struct sockaddr_nl src_addr, dest_addr;
struct iovec iov;
extern char pwd[MAX_PATH_LEN];
extern char root[MAX_PATH_LEN];
extern char display_pwd[MAX_PATH_LEN];

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

int main(int argc, char *argv[])
{
    // socket
    sock_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_SAFEBOX);
    if (sock_fd < 0)
        return -1;
    send_pid();

    int authentication = 0; // check if authentication succeed
    struct passwd *pw; pw = get_user(); // get user info
    
    // check login before, and get password
    uint8_t pswd_hash[SHA256_BYTES];
    int login_before = check_user(pw->pw_uid, pswd_hash);

    // get password input
    char input[100]; memset(input, 0, 100);
    
    if (login_before)
    {
        printf("Please enter your password \n");
        scanf("%s", input); getchar();

        uint8_t input_hash[SHA256_BYTES];
        sha256(input, strlen(input), input_hash);

        // check password
        int password_check = 1;
        for(int i=0; i<SHA256_BYTES; i++)
        {
            if(input_hash[i] != pswd_hash[i])
            {
                printf("Wrong password \n");
                password_check = 0;
                authentication = 0;
                break;
            }
        }

        if(password_check == 1)
        {
            printf("Welcome \n");
            authentication = 1;
        }
	
    }
    else
    {
        printf("Creating a new safebox, please set the password\n");
        scanf("%s", input); getchar();
        
        set_password(input);

        printf("Successfully create a safebox, welcome!\n");
        authentication = 1;
	
        // create dir
        chdir("/home/safebox");
        mkdir(pw->pw_name, 0777);
    }

    if (authentication == 0)
        return -1;
    
    memset(pwd, 0, MAX_PATH_LEN);
    memset(root, 0, MAX_PATH_LEN); 
    memset(display_pwd, 0, MAX_PATH_LEN);
    strcpy(pwd, "/home/safebox/");
    strcpy(root, "/home/safebox/");
    strcat(pwd, pw->pw_name); // TODO: should be modified to current user safebox directory
    strcat(root, pw->pw_name);
    strcpy(display_pwd, "");
    
    chdir(root);


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
