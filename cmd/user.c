#include "user.h"
#include "base64.h"



struct passwd * get_user(){
    uid_t uid;
    struct passwd *pw;
    uid = getuid();
    pw = getpwuid(uid); //通过uid获取passwd结构的成员信息
    return pw;
}

int check_user(int uid, uint8_t ret_pswd[SHA256_BYTES])
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

    while(1)
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
        int flag = 1;
        for(int i=0; i<SHA256_BYTES; i++)
        {
            if(hash_uid[i] != hash[i])
            {
                flag = 0;
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
        if (flag == 1)
        {
            fclose(fp);
            return 1;
        }

        if(feof(fp))
            break;
    }

    for(int i=0; i<SHA256_BYTES; i++)
        ret_pswd[i] = 0;

    fclose(fp);
    return 0;
}

int set_password(char* pswd)
{
    FILE *fp;
    if (!(fp=fopen("/home/safebox/password.dat","a+")))
    {
        printf("Error in open password.dat while setting password!\n");
        exit(1);
    }

    struct passwd *pw; pw = get_user();
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

void reset_password(char* pswd)
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

    struct passwd *pw; pw = get_user();
    char _uid[20]; sprintf(_uid, "%d", pw->pw_uid);
    sha256(_uid, strlen(_uid), hash_uid);
    sha256(pswd, strlen(pswd), hash_pswd);

    while(1)
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
        int flag = 1;
        for(int i=0; i<SHA256_BYTES; i++)
        {
            if(hash_uid[i] != hash_orig_uid[i])
            {
                flag = 0;
                break;
            }
        }

        if (flag == 1)
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
    remove("/home/safebox/password.dat");
    rename("/home/safebox/password_temp.dat", "/home/safebox/password.dat");
}
