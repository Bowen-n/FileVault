#include "user.h"
#include "utils.h"


User::User()
{
    uid_t uid = getuid();
    pw = getpwuid(uid);
}

int User::pid()
{
    return pw->pw_uid;
}

char* User::pname()
{
    return pw->pw_name;
}

bool User::check_user(int uid, uint8_t ret_pswd[SHA256_BYTES])
{
    // check user if login before and return its password's hash
    uint8_t hash_uid[SHA256_BYTES];

    FILE *fp;
    if (!(fp=fopen("/home/safebox/password.dat","r")))
    {
        printf("Error in open password.dat while checking user!\n");
        exit(1);
    }

    char _uid[20]; sprintf(_uid, "%d", uid);
    sha256(_uid, strlen(_uid), hash_uid);

    while(true)
    {
        uint8_t hash[SHA256_BYTES];

        // read uid's hash
        for(int i=0; i<SHA256_BYTES; i++)
        {
            uint8_t tmp[1];
            fscanf(fp, "%02x", tmp);
            hash[i] = tmp[0];
        }
    
        // check if uid equal to current user
        bool flag = true;
        for(int i=0; i<SHA256_BYTES; i++)
        {
            if(hash_uid[i] != hash[i])
            {
                flag = false;
                break;
            }
        }

        // read pswd's hash
        fscanf(fp, " ");
        for(int i=0; i<SHA256_BYTES; i++)
        {
            uint8_t tmp[1];
            fscanf(fp, "%02x", tmp);
            ret_pswd[i] = tmp[0];
        }
        fscanf(fp, "\n");

        // if uid found, return pswd's hash
        if (flag == true)
        {
            fclose(fp);
            return true;
        }

        if(feof(fp))
            break;
    }

    for(int i=0; i<SHA256_BYTES; i++)
        ret_pswd[i] = 0;

    fclose(fp);
    return false;
}


int User::set_password(char* pswd)
{
    FILE *fp;
    if (!(fp=fopen("/home/safebox/password.dat","a+")))
    {
        printf("Error in open password.dat while setting password!\n");
        exit(1);
    }

    char uid[20]; sprintf(uid, "%d", pw->pw_uid);

    uint8_t hash[SHA256_BYTES];
    sha256(uid, strlen(uid), hash);

    for(int i=0; i<sizeof(hash); i++)
        fprintf(fp, "%02x", hash[i]);
    fprintf(fp, " ");

    sha256(pswd, strlen(pswd), hash);

    for(int i=0; i<sizeof(hash); i++)
        fprintf(fp, "%02x", hash[i]);
    fprintf(fp, "\n");

    fclose(fp);
    return 1;
}


void User::reset_password(char* pswd)
{
    FILE *fp;
    if (!(fp=fopen("/home/safebox/password.dat","r+")))
    {
        printf("Error in open password.dat while resetting password!\n");
        exit(1);
    }

    FILE *new_fp;
    if (!(new_fp=fopen("/home/safebox/password_temp.dat","w")))
    {
        printf("Error in create password_temp.dat while resetting password!\n");
        exit(1);
    }

    uint8_t hash_pswd[SHA256_BYTES];
    uint8_t hash_uid[SHA256_BYTES];

    uint8_t hash_orig_uid[SHA256_BYTES];
    uint8_t hash_orig_pswd[SHA256_BYTES];

    char _uid[20]; sprintf(_uid, "%d", pw->pw_uid);
    sha256(_uid, strlen(_uid), hash_uid);
    sha256(pswd, strlen(pswd), hash_pswd);

    while(true)
    {
        // read uid's hash
        for(int i=0; i<SHA256_BYTES; i++)
        {
            uint8_t tmp[1];
            fscanf(fp, "%02x", tmp);
            hash_orig_uid[i] = tmp[0];
        }
        fscanf(fp, " ");
        // read password's hash
        for(int i=0; i<SHA256_BYTES; i++)
        {
            uint8_t tmp[1];
            fscanf(fp, "%02x", tmp);
            hash_orig_pswd[i] = tmp[0];
        }
        fscanf(fp, "\n");

        // check if uid equals to current user's
        bool flag = true;
        for(int i=0; i<SHA256_BYTES; i++)
        {
            if(hash_uid[i] != hash_orig_uid[i])
            {
                flag = false;
                break;
            }
        }

        if (flag == true)
        {
            // write uid and new password to new file
            for(int i=0; i<SHA256_BYTES; i++)
                fprintf(new_fp, "%02x", hash_uid[i]);
            fprintf(new_fp, " ");
            for(int i=0; i<SHA256_BYTES; i++)
                fprintf(new_fp, "%02x", hash_pswd[i]);
            fprintf(new_fp, "\n");
        }
        else
        {
            // write uid and old password to new file
            for(int i=0; i<SHA256_BYTES; i++)
                fprintf(new_fp, "%02x", hash_orig_uid[i]);
            fprintf(new_fp, " ");
            for(int i=0; i<SHA256_BYTES; i++)
                fprintf(new_fp, "%02x", hash_orig_pswd[i]);
            fprintf(new_fp, "\n");
        }

        if (feof(fp))
            break;
    }

    fclose(fp);
    fclose(new_fp);
    QFile::remove(QString::fromStdString("/home/safebox/password.dat"));
    QFile::rename(QString::fromStdString("/home/safebox/password_temp.dat"),QString::fromStdString("/home/safebox/password.dat"));
}
