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

char pwd[MAX_PATH_LEN];
char root[MAX_PATH_LEN];
char display_pwd[MAX_PATH_LEN];

// help functions
int _ls(char path[COMMAND_MAX_LEN]);

// special execute
int exec_cd(char splited_cmd[][COMMAND_MAX_LEN], int cmd_count);
int exec_ls(char splited_cmd[][COMMAND_MAX_LEN], int cmd_count);

// main execute
int execute(char splited_cmd[][COMMAND_MAX_LEN], int cmd_count);

#endif