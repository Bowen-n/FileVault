#ifndef __EXEC_H__
#define __EXEC_H__

#include <linux/string.h>
#include <stdio.h>
#include <dirent.h>
#include "utils.h"

#define COMMAND_MAX_LEN 40
#define MAX_PATH_LEN 256
#define COMMAND_MAX_LEN 40

#define CMD_CD "cd"
#define CMD_LS "ls"
#define CMD_EXIT "exit"
#define CMD_RM "rm"
#define CMD_MKDIR "mkdir"
#define CMD_MV "mv"
#define CMD_MVIN "mvin"
#define CMD_MVOUT "mvout"


char pwd[MAX_PATH_LEN];
char root[MAX_PATH_LEN];
char display_pwd[MAX_PATH_LEN];

// help functions
int _ls(char path[COMMAND_MAX_LEN]);
int _cd(char path[COMMAND_MAX_LEN]);
int _mkdir(char path[COMMAND_MAX_LEN]);
int _rm(char path[COMMAND_MAX_LEN]);
int _mv(char oldfile[COMMAND_MAX_LEN], char newfile[COMMAND_MAX_LEN]);


// special execute
int exec_cd(char splited_cmd[][COMMAND_MAX_LEN], int cmd_count);
int exec_ls(char splited_cmd[][COMMAND_MAX_LEN], int cmd_count);
int exec_rm(char splited_cmd[][COMMAND_MAX_LEN], int cmd_count);
int exec_mkdir(char splited_cmd[][COMMAND_MAX_LEN], int cmd_count);
int exec_mv(char splited_cmd[][COMMAND_MAX_LEN], int cmd_count);
int exec_mvin(char splited_cmd[][COMMAND_MAX_LEN], int cmd_count);
int exec_mvout(char splited_cmd[][COMMAND_MAX_LEN], int cmd_count);

// main execute
int execute(char splited_cmd[][COMMAND_MAX_LEN], int cmd_count);

#endif
