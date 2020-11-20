#include "exec.h"

//NMJ's modification begins here.
int _cd(char path[COMMAND_MAX_LEN])
{
    return chdir(path);
}

int exec_cd(char splited_cmd[][COMMAND_MAX_LEN], int cmd_count)
{
    switch(cmd_count)
    {
        case 1:
        {
            _cd(pwd);
            break;
        }
        case 2:
        {
            char cd_path[COMMAND_MAX_LEN]; memset(cd_path, 0, COMMAND_MAX_LEN);
            char display_path[COMMAND_MAX_LEN]; memset(display_path, 0, COMMAND_MAX_LEN);
            char target_path[COMMAND_MAX_LEN]; memset(target_path, 0, COMMAND_MAX_LEN);
            strcpy(cd_path, splited_cmd[1]);
            
            if (strncmp(cd_path, "/", 1) == 0)//abspath
            {
                strcpy(target_path, pwd);
                strcat(target_path, cd_path);
                strcpy(display_path, display_pwd);
                strcat(display_path, cd_path);
                
                int suc = _cd(target_path)
                if (suc == -1)
                {
                    printf("Invalid cd path.\n");
                }
                else
                {
                    memset(display_pwd, 0, COMMAND_MAX_LEN); // change display path
                    strcpy(display_pwd, display_path);   
                }
                break;
            }
            else//relpath
            {
                strcpy(target_path, pwd);
                strcat(target_path, "/");
                strcat(target_path, cd_path); 
                char norm[COMMAND_MAX_LEN]; memset(norm, 0, COMMAND_MAX_LEN);
                
                normpath(norm, target_path);
                
                if (strncmp(norm, pwd, strlen(pwd)) != 0) //root doesn't have parent directory.
                {
                    printf("Illegal dir.\n");
                    break;
                }
                
                strcpy(display_path, display_pwd);
                strcat(display_path, cd_path);
                char norm_display[COMMAND_MAX_LEN]; memset(norm_display, 0, COMMAND_MAX_LEN);
                
                normpath(norm_display, display_path);
                
                int suc = _cd(norm);
                if (suc == -1)
                {
                    printf("Invalid cd path.\n");
                }
                else
                {
                    memset(display_pwd, 0, COMMAND_MAX_LEN); // change display path
                    strcpy(display_pwd, norm_display);
                }
                break;
            }
        default:
            printf("Arguments error. Format: cd [path]");
        }
    }
    return 0;
}
//NMJ's modification ends here.

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
