// Shell.

#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

// Parsed command representation
#define EXEC  1
#define REDIR 2
#define PIPE  3
#define LIST  4
#define BACK  5

#define MAXARGS 10

struct cmd {
  int type;
};

struct execcmd {
  int type;
  char *argv[MAXARGS];
  char *eargv[MAXARGS];
};

struct redircmd {
  int type;
  struct cmd *cmd;
  char *file;
  char *efile;
  int mode;
  int fd;
};

struct pipecmd {
  int type;
  struct cmd *left;
  struct cmd *right;
};

struct listcmd {
  int type;
  struct cmd *left;
  struct cmd *right;
  char op;
};

struct backcmd {
  int type;
  struct cmd *cmd;
};

int fork1(void);  // Fork but panics on failure.
void panic(char*);
struct cmd *parsecmd(char*);

struct node {
  char * str;
  struct node * next;
};

static struct node head;

// Execute cmd.  Never returns.
void
runcmd(struct cmd *cmd)
{
  int p[2];
  struct backcmd *bcmd;
  struct execcmd *ecmd;
  struct listcmd *lcmd;
  struct pipecmd *pcmd;
  struct redircmd *rcmd;

  if(cmd == 0)
    exit();

  switch(cmd->type){
  default:
    panic("runcmd");

  case EXEC:
    ecmd = (struct execcmd*)cmd;
    int se;
    if(ecmd->argv[0] == 0)
      exit();
    if (fork1() == 0) {
      int ffdd = open(ecmd->argv[0], 0);
      if (ffdd < 0) {
        struct node * cur = head.next;
        while (cur) {
          int cl = strlen(cur->str);
          int slen = cl + strlen(ecmd->argv[0]) + 1;
          char str[slen + 2];
          memset(str, 0, slen + 2);
          strcpy(str, cur->str);
          str[cl] = '/';
          int ii = cl + 1;
          for (; ii < slen; ++ii) {
            str[ii] = ecmd->argv[0][ii - cl - 1];
          }
          ffdd = open(str, 0);
          if (ffdd > 0) {
            exec(str, ecmd->argv);
            printf(2, "exec %s failed\n", ecmd->argv[0]);
            exit1(1);
          }
          cur = cur->next;
        }
      }
      else {
        exec(ecmd->argv[0], ecmd->argv);
      }
      printf(2, "exec %s failed\n", ecmd->argv[0]);
      exit1(1);
    }
    wait1(&se);
    exit1(se);
    break;

  case REDIR:
    rcmd = (struct redircmd*)cmd;
    close(rcmd->fd);
    if(open(rcmd->file, rcmd->mode) < 0){
      printf(2, "open %s failed\n", rcmd->file);
      exit1(1);
    }
    runcmd(rcmd->cmd);
    break;

  case LIST:
    lcmd = (struct listcmd*)cmd;
    int status;
    if(fork1() == 0)
      runcmd(lcmd->left);
    wait1(&status);
    if (lcmd->op == 'n') {
      runcmd(lcmd->right);
    }
    else if (lcmd->op == 'a') {
      if (status == 0) {
        runcmd(lcmd->right);
      }
      else {
        exit1(1);
      }
    }
    else if (lcmd->op == 'o') {
      if (status == 1) {\
        runcmd(lcmd->right);
      }
      else {
        exit1(0);
      }
    }
    break;

  case PIPE:
    pcmd = (struct pipecmd*)cmd;
    if(pipe(p) < 0)
      panic("pipe");
    if(fork1() == 0){
      close(1);
      dup(p[1]);
      close(p[0]);
      close(p[1]);
      runcmd(pcmd->left);
    }
    if(fork1() == 0){
      close(0);
      dup(p[0]);
      close(p[0]);
      close(p[1]);
      runcmd(pcmd->right);
    }
    close(p[0]);
    close(p[1]);
    int ss1, ss2;
    wait1(&ss1);
    wait1(&ss2);
    exit1(ss2);
    break;

  case BACK:
    bcmd = (struct backcmd*)cmd;
    if(fork1() == 0)
      runcmd(bcmd->cmd);
    break;
  }
  exit();
}

int
getcmd(char *buf, int nbuf)
{
  printf(2, "$ ");
  memset(buf, 0, nbuf);
  gets(buf, nbuf);
  if(buf[0] == 0) // EOF
    return -1;
  return 0;
}

int runScript(char * scriptName)
{
  int fd = open(scriptName, O_RDONLY), res;
  if (fd < 0)
  {
    exit1(1);
  }
  struct stat st;
  stat(scriptName, &st);
  char buf[st.size + 1];
  memset(buf, 0, st.size);
  res = read(fd, buf, st.size);
  if (res != st.size)
  {
    exit1(1);
  }
  close(fd);
  uint left = 0;
  int status;
  while (left < st.size)
  {
    int right = left;
    while (right < st.size && buf[right] != '\n')
    {
      ++right;
    }
    char curcmd[right - left + 1];
    memset(curcmd, 0, right - left + 1);
    int i;
    for (i = 0; i < right - left; ++i)
    {
      curcmd[i] = buf[left + i];
    }
    if(fork1() == 0)
      runcmd(parsecmd(curcmd));
    wait1(&status);
    left = right + 1;
  }
  return status;
}

void init_profile()
{
  int fd = open("/.profile", 0);
  if (fd < 0) {
    fd = open("/.profile", 513);
    char default_path[] = "PATH=/:/bin\n";
    write(fd, default_path, strlen(default_path));
    close(fd);
    fd = open("/.profile", 0);
  }
  struct stat st;
  fstat(fd, &st);
  uint size = st.size;
  if (size == 0) {
    return;
  }
  char content[size + 1];
  if (read(fd, content, size) != size) {
    return;
  }
  int cnt_line = 0;
  int idx = 0;
  while (idx < size) {
    if (content[idx++] == '\n') {
      ++cnt_line;
    }
  }
  if (content[size - 1] != '\n') ++cnt_line;
  head.next = 0;
  idx = 0;
  int i;
  for (i = 0; i < cnt_line; ++i) {
    int j = idx;
    while (j < size && content[j] != '\n') ++j;
    if (j > idx) {
      int line_len = j - idx;
      char line[line_len + 1];
      memset(line, 0, line_len);
      int k = 0;
      for (k = 0; k < line_len; ++k) {
        line[k] = content[idx + k];
      }
      int line_start = -1;
      for (k = 0; k < line_len; ++k) {
        if (line[k] == '=') {
          line_start = k + 1;
          break;
        }
      }
      if (line_start >= 0) {
        k = line_start;
        while (1) {
          if ((k == line_len || line[k] == ':')) {
            if (k == line_len) {
              if (k > line_start) {
                struct node * nn = malloc(sizeof(struct node));
                nn->next = head.next;
                nn->str = malloc(k - line_start + 1);
                memset(nn->str, 0, k - line_start + 1);
                int l = 0;
                for (l = 0; l < k - line_start; ++l) {
                  nn->str[l] = line[line_start + l];
                }
                head.next = nn;
                line_start = k = k + 1;
              }
              break;
            }
            else {
              if (k > line_start) {
                struct node * nn = malloc(sizeof(struct node));
                nn->next = head.next;
                nn->str = malloc(k - line_start + 1);
                memset(nn->str, 0, k - line_start + 1);
                int l = 0;
                for (l = 0; l < k - line_start; ++l) {
                  nn->str[l] = line[line_start + l];
                }
                head.next = nn;
              }
              ++k;
              line_start = k;
            }
          }
          else {
            ++k;
          }
        }
      }
      idx = j + 1;
    }
    else {
      ++idx;
    }
  }
  // struct node * cur = head.next;
  // while (cur) {
  //   printf(1, "%s\n", cur->str);
  //   cur = cur->next;
  // }
}

int
main(int argc, char ** argv)
{
  init_profile();
  static char buf[100];
  int fd;

  // Ensure that three file descriptors are open.
  while((fd = open("console", O_RDWR)) >= 0){
    if(fd >= 3){
      close(fd);
      break;
    }
  }

  if (argc == 2) {
    int status = runScript(argv[1]);
    exit1(status);
  }

  // Read and run input commands.
  while(getcmd(buf, sizeof(buf)) >= 0){
    if(buf[0] == 'c' && buf[1] == 'd' && buf[2] == ' '){
      // Chdir must be called by the parent, not the child.
      buf[strlen(buf)-1] = 0;  // chop \n
      if(chdir(buf+3) < 0)
        printf(2, "cannot cd %s\n", buf+3);
      continue;
    }
    if(fork1() == 0)
      runcmd(parsecmd(buf));
    wait();
  }
  exit();
}

void
panic(char *s)
{
  printf(2, "%s\n", s);
  exit();
}

int
fork1(void)
{
  int pid;

  pid = fork();
  if(pid == -1)
    panic("fork");
  return pid;
}

//PAGEBREAK!
// Constructors

struct cmd*
execcmd(void)
{
  struct execcmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = EXEC;
  return (struct cmd*)cmd;
}

struct cmd*
redircmd(struct cmd *subcmd, char *file, char *efile, int mode, int fd)
{
  struct redircmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = REDIR;
  cmd->cmd = subcmd;
  cmd->file = file;
  cmd->efile = efile;
  cmd->mode = mode;
  cmd->fd = fd;
  return (struct cmd*)cmd;
}

struct cmd*
pipecmd(struct cmd *left, struct cmd *right)
{
  struct pipecmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = PIPE;
  cmd->left = left;
  cmd->right = right;
  return (struct cmd*)cmd;
}

struct cmd*
listcmd(struct cmd *left, struct cmd *right, char op)
{
  struct listcmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = LIST;
  cmd->left = left;
  cmd->right = right;
  cmd->op = op;
  return (struct cmd*)cmd;
}

struct cmd*
backcmd(struct cmd *subcmd)
{
  struct backcmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = BACK;
  cmd->cmd = subcmd;
  return (struct cmd*)cmd;
}
//PAGEBREAK!
// Parsing

char whitespace[] = " \t\r\n\v";
char symbols[] = "<|>&;()";

int
gettoken(char **ps, char *es, char **q, char **eq)
{
  char *s;
  int ret;

  s = *ps;
  while(s < es && strchr(whitespace, *s))
    s++;
  if(q)
    *q = s;
  ret = *s;
  switch(*s){
  case 0:
    break;
  case '(':
  case ')':
  case ';':
  case '<':
    s++;
    break;
  case '&':
    s++;
    if (*s == '&'){
      ret = 2 * '&';
      s++;
    }
    break;
  case '|':
    s++;
    if (*s == '|'){
      ret = 2 * '|';
      s++;
    }
    break;
  case '>':
    s++;
    if(*s == '>'){
      ret = '+';
      s++;
    }
    break;
  default:
    ret = 'a';
    while(s < es && !strchr(whitespace, *s) && !strchr(symbols, *s))
      s++;
    break;
  }
  if(eq)
    *eq = s;

  while(s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  return ret;
}

int
peek(char **ps, char *es, char *toks)
{
  char *s;

  s = *ps;
  while(s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  if (*toks == '&' && *(toks + 1) == '&') {
    return *s && strchr(toks, *s) && strchr(toks, *(s + 1));
  }
  else if (*toks == '&' && *(toks + 1) == 0) {
    return *s && strchr(toks, *s) && !strchr(toks, *(s + 1));
  }
  if (*toks == '|' && *(toks + 1) == '|') {
    return *s && strchr(toks, *s) && strchr(toks, *(s + 1));
  }
  else if (*toks == '|' && *(toks + 1) == 0) {
    return *s && strchr(toks, *s) && !strchr(toks, *(s + 1));
  }
  else {
    return *s && strchr(toks, *s);
  }
}

struct cmd *parseline(char**, char*);
struct cmd *parsepipe(char**, char*);
struct cmd *parseexec(char**, char*);
struct cmd *nulterminate(struct cmd*);

struct cmd*
parsecmd(char *s)
{
  char *es;
  struct cmd *cmd;

  es = s + strlen(s);
  cmd = parseline(&s, es);
  peek(&s, es, "");
  if(s != es){
    printf(2, "leftovers: %s\n", s);
    panic("syntax");
  }
  nulterminate(cmd);
  return cmd;
}

struct cmd*
parseline(char **ps, char *es)
{
  struct cmd *cmd;

  cmd = parsepipe(ps, es);
  while(peek(ps, es, "&")){
    gettoken(ps, es, 0, 0);
    cmd = backcmd(cmd);
  }
  if(peek(ps, es, ";")){
    gettoken(ps, es, 0, 0);
    cmd = listcmd(cmd, parseline(ps, es), 'n');
  }
  else if (peek(ps, es, "&&")) {
    gettoken(ps, es, 0, 0);
    cmd = listcmd(cmd, parseline(ps, es), 'a');
  }
  else if (peek(ps, es, "||")) {
    gettoken(ps, es, 0, 0);
    cmd = listcmd(cmd, parseline(ps, es), 'o');
  }
  return cmd;
}

struct cmd*
parsepipe(char **ps, char *es)
{
  struct cmd *cmd;

  cmd = parseexec(ps, es);
  if(peek(ps, es, "|")){
    gettoken(ps, es, 0, 0);
    cmd = pipecmd(cmd, parsepipe(ps, es));
  }
  return cmd;
}

struct cmd*
parseredirs(struct cmd *cmd, char **ps, char *es)
{
  int tok;
  char *q, *eq;

  while(peek(ps, es, "<>")){
    tok = gettoken(ps, es, 0, 0);
    if(gettoken(ps, es, &q, &eq) != 'a')
      panic("missing file for redirection");
    switch(tok){
    case '<':
      cmd = redircmd(cmd, q, eq, O_RDONLY, 0);
      break;
    case '>':
      cmd = redircmd(cmd, q, eq, O_WRONLY|O_CREATE, 1);
      break;
    case '+':  // >>
      cmd = redircmd(cmd, q, eq, O_WRONLY|O_CREATE, 1);
      break;
    }
  }
  return cmd;
}

struct cmd*
parseblock(char **ps, char *es)
{
  struct cmd *cmd;

  if(!peek(ps, es, "("))
    panic("parseblock");
  gettoken(ps, es, 0, 0);
  cmd = parseline(ps, es);
  if(!peek(ps, es, ")"))
    panic("syntax - missing )");
  gettoken(ps, es, 0, 0);
  cmd = parseredirs(cmd, ps, es);
  return cmd;
}

struct cmd*
parseexec(char **ps, char *es)
{
  char *q, *eq;
  int tok, argc;
  struct execcmd *cmd;
  struct cmd *ret;

  if(peek(ps, es, "("))
    return parseblock(ps, es);

  ret = execcmd();
  cmd = (struct execcmd*)ret;

  argc = 0;
  ret = parseredirs(ret, ps, es);
  while(!peek(ps, es, "|)&;")){
    if((tok=gettoken(ps, es, &q, &eq)) == 0)
      break;
    if(tok != 'a')
      panic("syntax");
    cmd->argv[argc] = q;
    cmd->eargv[argc] = eq;
    argc++;
    if(argc >= MAXARGS)
      panic("too many args");
    ret = parseredirs(ret, ps, es);
  }
  cmd->argv[argc] = 0;
  cmd->eargv[argc] = 0;
  return ret;
}

// NUL-terminate all the counted strings.
struct cmd*
nulterminate(struct cmd *cmd)
{
  int i;
  struct backcmd *bcmd;
  struct execcmd *ecmd;
  struct listcmd *lcmd;
  struct pipecmd *pcmd;
  struct redircmd *rcmd;

  if(cmd == 0)
    return 0;

  switch(cmd->type){
  case EXEC:
    ecmd = (struct execcmd*)cmd;
    for(i=0; ecmd->argv[i]; i++)
      *ecmd->eargv[i] = 0;
    break;

  case REDIR:
    rcmd = (struct redircmd*)cmd;
    nulterminate(rcmd->cmd);
    *rcmd->efile = 0;
    break;

  case PIPE:
    pcmd = (struct pipecmd*)cmd;
    nulterminate(pcmd->left);
    nulterminate(pcmd->right);
    break;

  case LIST:
    lcmd = (struct listcmd*)cmd;
    nulterminate(lcmd->left);
    nulterminate(lcmd->right);
    break;

  case BACK:
    bcmd = (struct backcmd*)cmd;
    nulterminate(bcmd->cmd);
    break;
  }
  return cmd;
}
