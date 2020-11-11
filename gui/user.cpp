#include "user.h"
#include "utils.h"


User::User()
{
    uid_t uid = getuid();
    pw = getpwuid(uid);
}


char* User::check_user(int uid)
{
    char label[30], password[100];
    memset(label, 0, 30); memset(password, 0, 30);
    char *buf = (char *)malloc(40);
    FILE *fp;

    if (!(fp=fopen("/home/safebox/password.dat","r")))
    {
        printf("Error in open file!\n");
        exit(1);
    }

    while (fscanf(fp,"%s %s", label, password))
    {   
        base64_decode(label, &buf);
        if (atoi(buf) == uid)
        {
            base64_decode(password, &buf);
            fclose(fp);
            return buf;
        }
        if (feof(fp))
        {
            break;
        }
    }
    free(buf);
    fclose(fp);
    return NULL;
}


int User::pid()
{
    return pw->pw_uid;
}

char* User::pname()
{
    return pw->pw_name;
}

int User::set_password(char* pswd)
{
    FILE *fp;
    if (!(fp=fopen("/home/safebox/password.dat","a+")))
    {
        printf("Error in open file!\n");
        exit(1);
    }

    char uid[20]; sprintf(uid, "%d", pw->pw_uid);
    char *buf1 = (char *)malloc(40);
    base64_encode(uid, &buf1);
    fprintf(fp, "\n%s ", buf1);
    free(buf1);

    char *buf2 = (char *)malloc(40);
    base64_encode(pswd, &buf2);
    fprintf(fp, "%s", buf2);
    free(buf2);

    fclose(fp);
    return 1;
}
