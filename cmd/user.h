#ifndef __USER_H__
#define __USER_H__

#include <sys/types.h>
#include <pwd.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "sha256.h"

struct passwd * get_user();
int check_user(int uid, uint8_t ret_pswd[SHA256_BYTES]);
int set_password(char* pswd);
void reset_password(char* pswd);
#endif