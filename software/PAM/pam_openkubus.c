#include <sys/param.h>

#include <pwd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <security/pam_modules.h>
#include <security/pam_appl.h>

#include "../libraries/C/openkubus.h"
#include "pam_openkubus.h"

#define BLOCKSIZE 16
#define BUFFER    500
#define PASSWD_FILE "/etc/openkubus-passwd"

#ifndef _OPENPAM
static char password_prompt[] = "OpenKubus-Pad: ";
#endif

#ifndef PAM_EXTERN
#define PAM_EXTERN
#endif

// heap
char line[BUFFER];

// called before openkubus_authenticate returns
int _cleanup(FILE *file, int ret)
{

  if(file != NULL)
  {
    fclose(file);
    file = NULL;
  }

  return ret;
}

// called when authentication was successfull
// updates number of last sucessfull authentication to prevent replay attacks
// XXX: still buggy: corrupts short lines ###HAS_TO_BE_FIXED##
void _update_passwd(FILE *file, char *num, int k)
{
  long int len = strlen(num);
  char num_inc[10];
  sprintf(num_inc, "%d", k);
  int diff = strlen(num_inc) - strlen(num);

  // XXX
  if(diff > 0)
  {
    int i;
    unsigned long pos = ftell(file);

    // seek
    while(fgets(line, BUFFER, file) != NULL)
    {
      i = strlen(line);
      fseek(file, -i+diff, SEEK_CUR);
      fputs(line, file);
    }

    fseek(file, pos-len-1, SEEK_SET);
  }
  else
    fseek(file, -len-1, SEEK_CUR);

  fprintf(file, "%s\n", num_inc);
}

// removes trailing newlines
int _chomp(char *str)
{
  int len = strlen(str)-1;
  int i = len;

  while(str[i] == '\n' && i >= 0)
    i--;

  str[i+1] = 0;

  return len-i;
}

// parses openkubus-passwd file
// returns 0 on success; if successfull user, key, data and num are filled with
// data from file
int _parse_passwd(FILE *file, const char *user, char **pw, char **offset, char **num)
{
  int found = 0, i;
  const char *delim = " ";
  int len_user = strlen(user);

  while(fgets(line, BUFFER, file) != NULL)
  {
    int len = strlen(line);

    // line too long
    if(_chomp(line) == 0)
      continue;

    if(len > len_user)
    {
      if(strncmp(user, line, len_user) == 0)
      {
        for(i = 0; i < len; i++)
          if(line[i] == '\t')
            line[i] = ' ';

        if(strtok(line, delim) == NULL)             continue;
        if((*pw     = strtok(NULL, delim)) == NULL) continue;
        if((*offset = strtok(NULL, delim)) == NULL) continue;
        if((*num    = strtok(NULL, delim)) == NULL) continue;

        found = 1;
        break;
      }
    }
  }
  
  if(!found)
    return 1;
  else
    return 0;
}


void cleanup(pam_handle_t *pamh, void *data, int error_status)
{
  if(data != NULL) free(data);
}

PAM_EXTERN int
pam_sm_authenticate(pam_handle_t *pamh, int flags,
  int argc, const char *argv[])
{
#ifndef _OPENPAM
  const void *ptr;
  const struct pam_conv *conv;
  struct pam_message msg;
  const struct pam_message *msgp;
  struct pam_response *resp;
#endif
  struct passwd *pwd;
  const char *user;
  char *pad;
  int pam_err, retry;

  FILE *file = NULL;
  char *pw = NULL, *offset = NULL, *num = NULL;

  /* identify user */
  if ((pam_err = pam_get_user(pamh, &user, NULL)) != PAM_SUCCESS)
    return (pam_err);
  if ((pwd = getpwnam(user)) == NULL)
    return (PAM_USER_UNKNOWN);

  /* get password */
#ifndef _OPENPAM
  pam_err = pam_get_item(pamh, PAM_CONV, &ptr);
  if (pam_err != PAM_SUCCESS)
    return (PAM_SYSTEM_ERR);
  conv = ptr;
  msg.msg_style = PAM_PROMPT_ECHO_ON;
  msg.msg = password_prompt;
  msgp = &msg;
#endif
  pad = NULL;
  for (retry = 0; retry < 3; ++retry) {
#ifdef _OPENPAM
    pam_err = pam_get_authtok(pamh, PAM_AUTHTOK,
        (const char **)&pad, NULL);
#else
    resp = NULL;
    pam_err = (*conv->conv)(1, &msgp, &resp, conv->appdata_ptr);
    if (resp != NULL) {
      if (pam_err == PAM_SUCCESS)
        pad = resp->resp;
      else
        free(resp->resp);
      free(resp);
    }
  msg.msg_style = PAM_PROMPT_ECHO_ON; // XXX
#endif
    if (pam_err == PAM_SUCCESS)
      break;
  }

  if (pam_err == PAM_CONV_ERR)
    return (pam_err);
  if (pam_err != PAM_SUCCESS)
    return (PAM_AUTH_ERR);

  if((file = fopen(PASSWD_FILE, "r+")) == NULL)
    return PAM_AUTH_ERR;

  if(_parse_passwd(file, user, &pw, &offset, &num) != 0)
    return _cleanup(file, PAM_AUTH_ERR);

  int i;
  if((i = openkubus_authenticate(pad, pw, atoi(offset), atoi(num))) > 0)
  {
    _update_passwd(file, num, i+1);

    return _cleanup(file, PAM_SUCCESS);
  }
  else
    printf("%d\n", i);

  return _cleanup(file, PAM_AUTH_ERR);
}

PAM_EXTERN int
pam_sm_setcred(pam_handle_t *pamh, int flags,
  int argc, const char *argv[])
{

  return (PAM_SUCCESS);
}

PAM_EXTERN int
pam_sm_acct_mgmt(pam_handle_t *pamh, int flags,
  int argc, const char *argv[])
{

  return (PAM_SUCCESS);
}

PAM_EXTERN int
pam_sm_open_session(pam_handle_t *pamh, int flags,
  int argc, const char *argv[])
{

  return (PAM_SUCCESS);
}

PAM_EXTERN int
pam_sm_close_session(pam_handle_t *pamh, int flags,
  int argc, const char *argv[])
{

  return (PAM_SUCCESS);
}

PAM_EXTERN int
pam_sm_chauthtok(pam_handle_t *pamh, int flags,
  int argc, const char *argv[])
{

  return (PAM_SERVICE_ERR);
}


#ifdef PAM_MODULE_ENTRY
PAM_MODULE_ENTRY("pam_stick.so");
#endif
