/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*      boot.c - Load and start Minix.      Author: Kees J. Bot
 *                    27 Dec 1991
 */
char version[]=   "2.20";

#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <termios.h>
#include "include/sys/stat.h"
#include "include/fs/rawfs.h"
#include "edparams.h"

environment *env;        /* Lists the environment. */
MNX(u32_t) lowsec;       /* Offset to the file system on the boot device. */

#define arraysize(a)      (sizeof(a) / sizeof((a)[0]))
#define arraylimit(a)     ((a) + arraysize(a))
#define between(a, c, z)  ((unsigned) ((c) - (a)) <= ((z) - (a)))

int fsok= -1;     /* File system state.  Initially unknown. */

static int block_size;

/* The Minix boot block must start with these bytes: */
char boot_magic[] = { 0x31, 0xC0, 0x8E, 0xD8, 0xFA, 0x8E, 0xD0, 0xBC };

struct biosdev {
  char *name;       /* Name of device. */
  int device;       /* Device to edit parameters. */
} bootdev;

struct termios termbuf;
int istty;

int run_trailer(void);

void quit(int status)
{
  if (istty) (void) tcsetattr(0, TCSANOW, &termbuf);
  exit(status);
}

#define exit(s) quit(s)

void report(char *label)
/* edparams: label: No such file or directory */
{
  fprintf(stderr, "edparams: %s: %s\n", label, strerror(errno));
}

void fatal(char *label)
{
  report(label);
  exit(1);
}

void *alloc(void *m, size_t n)
{
  m = (m == 0) ? malloc(n) : realloc(m, n);
  if (m == 0) fatal("");
  return m;
}

#define malloc(n)       alloc(0, n)
#define realloc(m, n)   alloc(m, n)

#define mon2abs(addr)   ((void *) (addr))

int rwsectors(int rw, void *addr, MNX(u32_t) sec, int nsec)
{
  ssize_t r;
  size_t len= nsec * SECTOR_SIZE;

  if (lseek(bootdev.device, sec * SECTOR_SIZE, SEEK_SET) == -1)
    return errno;

  if (rw == 0) {
    r= read(bootdev.device, (char *) addr, len);
  } else {
    r= write(bootdev.device, (char *) addr, len);
  }
  if (r == -1) return errno;
  if (r != len) return EIO;
  return 0;
}

#define readsectors(a, s, n)     rwsectors(0, (a), (s), (n))
#define writesectors(a, s, n)    rwsectors(1, (a), (s), (n))
#define readerr(sec, err)       (errno= (err), report(bootdev.name))
#define writerr(sec, err)       (errno= (err), report(bootdev.name))
#define putch(c)    putchar(c)
#define unix_err(err)     strerror(err)

void readblock(off_t blk, char *buf, int block_size)
/* Read blocks for the rawfs package. */
{
  if(!block_size) fatal("block_size 0");
  errno= EIO;
  if (lseek(bootdev.device, blk * block_size, SEEK_SET) == -1
    || read(bootdev.device, buf, block_size) != block_size)
  {
    fatal(bootdev.name);
  }
}

sig_atomic_t trapsig;

void trap(int sig)
{
  trapsig= sig;
  signal(sig, trap);
}

int escape(void)
{
  if (trapsig == SIGINT) {
    trapsig= 0;
    return 1;
  }
  return 0;
}

static unsigned char unchar;

int getch(void)
{
  unsigned char c;

  fflush(stdout);

  if (unchar != 0) {
    c= unchar;
    unchar= 0;
    return c;
  }

  switch (read(0, &c, 1)) {
  case -1:
    if (errno != EINTR) fatal("");
    return(ESC);
  case 0:
    if (istty) putch('\n');
    exit(0);
  default:
    if (istty && c == termbuf.c_cc[VEOF]) {
      putch('\n');
      exit(0);
    }
    return c;
  }
}

#define ungetch(c)      ((void) (unchar = (c)))

#define get_tick()        ((MNX(u32_t)) time(0))
#define clear_screen()    printf("[clear]")
#define boot_device(device)     printf("[boot %s]\n", device)
#define ctty(line)        printf("[ctty %s]\n", line)
#define bootminix()       run_trailer(); printf("[boot]\n")
#define off()       printf("[off]")

char *readline(void)
/* Read a line including a newline with echoing. */
{
  char *line;
  size_t i, z;
  int c;

  i= 0;
  z= 20;
  line= malloc(z * sizeof(char));

  do {
    c= getch();

    if (strchr("\b\177\25\30", c) != 0) {
      /* Backspace, DEL, ctrl-U, or ctrl-X. */
      do {
        if (i == 0) break;
        printf("\b \b");
        i--;
      } while (c == '\25' || c == '\30');
    } else
    if (c < ' ' && c != '\n') {
      putch('\7');
    } else {
      putch(c);
      line[i++]= c;
      if (i == z) {
        z*= 2;
        line= realloc(line, z * sizeof(char));
      }
    }
  } while (c != '\n');
  line[i]= 0;
  return line;
}

int sugar(char *tok)
/* Recognize special tokens. */
{
  return strchr("=(){};\n", tok[0]) != 0;
}

char *onetoken(char **aline)
/* Returns a string with one token for tokenize. */
{
  char *line= *aline;
  size_t n;
  char *tok;

  /* Skip spaces and runs of newlines. */
  while (*line == ' ' || (*line == '\n' && line[1] == '\n')) line++;

  *aline= line;

  /* Don't do odd junk (nor the terminating 0!). */
  if ((unsigned) *line < ' ' && *line != '\n') return 0;

  if (*line == '(') {
    /* Function argument, anything goes but () must match. */
    int depth= 0;

    while ((unsigned) *line >= ' ') {
      if (*line == '(') depth++;
      if (*line++ == ')' && --depth == 0) break;
    }
  } else
  if (sugar(line)) {
    /* Single character token. */
    line++;
  } else {
    /* Multicharacter token. */
    do line++; while ((unsigned) *line > ' ' && !sugar(line));
  }
  n= line - *aline;
  tok= malloc((n + 1) * sizeof(char));
  memcpy(tok, *aline, n);
  tok[n]= 0;
  if (tok[0] == '\n') tok[0]= ';';  /* ';' same as '\n' */

  *aline= line;
  return tok;
}

/* Typed commands form strings of tokens. */

typedef struct token {
  struct token    *next;  /* Next in a command chain. */
  char      *token;
} token;

token **tokenize(token **acmds, char *line)
/* Takes a line apart to form tokens.  The tokens are inserted into a command
 * chain at *acmds.  Tokenize returns a reference to where another line could
 * be added.  Tokenize looks at spaces as token separators, and recognizes only
 * ';', '=', '{', '}', and '\n' as single character tokens.  One token is
 * formed from '(' and ')' with anything in between as long as more () match.
 */
{
  char *tok;
  token *newcmd;

  while ((tok= onetoken(&line)) != 0) {
    newcmd= malloc(sizeof(*newcmd));
    newcmd->token= tok;
    newcmd->next= *acmds;
    *acmds= newcmd;
    acmds= &newcmd->next;
  }
  return acmds;
}

static token *cmds;       /* String of commands to execute. */
static int err;   /* Set on an error. */

char *poptoken(void)
/* Pop one token off the command chain. */
{
  token *cmd= cmds;
  char *tok= cmd->token;

  cmds= cmd->next;
  free(cmd);

  return tok;
}

void voidtoken(void)
/* Remove one token from the command chain. */
{
  free(poptoken());
}

void parse_code(char *code)
/* Tokenize a string of monitor code, making sure there is a delimiter.  It is
 * to be executed next.  (Prepended to the current input.)
 */
{
  if (cmds != 0 && cmds->token[0] != ';') (void) tokenize(&cmds, ";");
  (void) tokenize(&cmds, code);
}

int interrupt(void)
/* Clean up after an ESC has been typed. */
{
  if (escape()) {
    printf("[ESC]\n");
    err= 1;
    return 1;
  }
  return 0;
}


/* Reserved names: */
enum resnames {
  R_NULL, R_BOOT, R_CTTY, R_DELAY, R_ECHO, R_EXIT, R_HELP,
  R_LS, R_MENU, R_OFF, R_SAVE, R_SET, R_TRAP, R_UNSET
};

char resnames[][6] = {
  "", "boot", "ctty", "delay", "echo", "exit", "help",
  "ls", "menu", "off", "save", "set", "trap", "unset",
};

/* Using this for all null strings saves a lot of memory. */
#define null (resnames[0])

int reserved(char *s)
/* Recognize reserved strings. */
{
  int r;

  for (r= R_BOOT; r <= R_UNSET; r++) {
    if (strcmp(s, resnames[r]) == 0) return r;
  }
  return R_NULL;
}

void sfree(char *s)
/* Free a non-null string. */
{
  if (s != 0 && s != null) free(s);
}

char *copystr(char *s)
/* Copy a non-null string using malloc. */
{
  char *c;

  if (*s == 0) return null;
  c= malloc((strlen(s) + 1) * sizeof(char));
  strcpy(c, s);
  return c;
}

int is_default(environment *e)
{
  return (e->flags & E_SPECIAL) && e->defval == 0;
}

environment **searchenv(char *name)
{
  environment **aenv= &env;

  while (*aenv != 0 && strcmp((*aenv)->name, name) != 0) {
    aenv= &(*aenv)->next;
  }

  return aenv;
}

#define b_getenv(name)  (*searchenv(name))
/* Return the environment *structure* belonging to name, or 0 if not found. */

char *b_value(char *name)
/* The value of a variable. */
{
  environment *e= b_getenv(name);

  return (e == 0 || !(e->flags & E_VAR) ? 0 : e->value);
}

char *b_body(char *name)
/* The value of a function. */
{
  environment *e= b_getenv(name);

  return (e == 0 || !(e->flags & E_FUNCTION) ? 0 : e->value);
}

int b_setenv(int flags, char *name, char *arg, char *value)
/* Change the value of an environment variable.  Returns the flags of the
 * variable if you are not allowed to change it, 0 otherwise.
 */
{
  environment **aenv, *e;

  if (*(aenv= searchenv(name)) == 0) {
    if (reserved(name)) return E_RESERVED;
    e= malloc(sizeof(*e));
    e->name= copystr(name);
    e->flags= flags;
    e->defval= 0;
    e->next= 0;
    *aenv= e;
  } else {
    e= *aenv;

    /* Don't change special variables to functions or vv. */
    if (e->flags & E_SPECIAL
      && (e->flags & E_FUNCTION) != (flags & E_FUNCTION)
    ) return e->flags;

    e->flags= (e->flags & E_STICKY) | flags;
    if (is_default(e)) {
      e->defval= e->value;
    } else {
      sfree(e->value);
    }
    sfree(e->arg);
  }
  e->arg= copystr(arg);
  e->value= copystr(value);

  return 0;
}

int b_setvar(int flags, char *name, char *value)
/* Set variable or simple function. */
{
  int r;

  if((r=b_setenv(flags, name, null, value))) {
    return r;
  }

  return r;
}

void b_unset(char *name)
/* Remove a variable from the environment.  A special variable is reset to
 * its default value.
 */
{
  environment **aenv, *e;

  if ((e= *(aenv= searchenv(name))) == 0) return;

  if (e->flags & E_SPECIAL) {
    if (e->defval != 0) {
      sfree(e->arg);
      e->arg= null;
      sfree(e->value);
      e->value= e->defval;
      e->defval= 0;
    }
  } else {
    sfree(e->name);
    sfree(e->arg);
    sfree(e->value);
    *aenv= e->next;
    free(e);
  }
}

long a2l(char *a)
/* Cheap atol(). */
{
  int sign= 1;
  long n= 0;

  if (*a == '-') { sign= -1; a++; }

  while (between('0', *a, '9')) n= n * 10 + (*a++ - '0');

  return sign * n;
}

char *ul2a(MNX(u32_t) n, unsigned b)
/* Transform a long number to ascii at base b, (b >= 8). */
{
  static char num[(CHAR_BIT * sizeof(n) + 2) / 3 + 1];
  char *a= arraylimit(num) - 1;
  static char hex[16] = "0123456789ABCDEF";

  do *--a = hex[(int) (n % b)]; while ((n/= b) > 0);
  return a;
}

char *ul2a10(MNX(u32_t) n)
/* Transform a long number to ascii at base 10. */
{
  return ul2a(n, 10);
}

unsigned a2x(char *a)
/* Ascii to hex. */
{
  unsigned n= 0;
  int c;

  for (;;) {
    c= *a;
    if (between('0', c, '9')) c= c - '0' + 0x0;
    else
    if (between('A', c, 'F')) c= c - 'A' + 0xA;
    else
    if (between('a', c, 'f')) c= c - 'a' + 0xa;
    else
      break;
    n= (n<<4) | c;
    a++;
  }
  return n;
}

void get_parameters(void)
{
  char params[SECTOR_SIZE + 1];
  token **acmds;
  int r, processor; // bus,
  memory *mp;
  static char bus_type[][4] = {
    "xt", "at", "mca"
  };
  static char vid_type[][4] = {
    "mda", "cga", "ega", "ega", "vga", "vga"
  };
  static char vid_chrome[][6] = {
    "mono", "color"
  };

  /* Variables that Minix needs: */
  b_setvar(E_SPECIAL|E_VAR|E_DEV, "rootdev", "ram");
  b_setvar(E_SPECIAL|E_VAR|E_DEV, "ramimagedev", "bootdev");
  b_setvar(E_SPECIAL|E_VAR, "ramsize", "0");

  b_setvar(E_SPECIAL|E_VAR, "processor", "?");
  b_setvar(E_SPECIAL|E_VAR, "bus", "?");
  b_setvar(E_SPECIAL|E_VAR, "video", "?");
  b_setvar(E_SPECIAL|E_VAR, "chrome", "?");
  b_setvar(E_SPECIAL|E_VAR, "memory", "?");
  b_setvar(E_SPECIAL|E_VAR, "c0", "?");

  /* Variables boot needs: */
  b_setvar(E_SPECIAL|E_VAR, "image", "boot/image");
  b_setvar(E_SPECIAL|E_FUNCTION, "leader", 
    "echo --- Welcome to MINIX 3. This is the boot monitor. ---\\n");
  b_setvar(E_SPECIAL|E_FUNCTION, "main", "menu");
  b_setvar(E_SPECIAL|E_FUNCTION, "trailer", "");

  /* Default hidden menu function: */
  b_setenv(E_RESERVED|E_FUNCTION, null, "=,Start MINIX", "boot");

  /* Tokenize bootparams sector. */
  if ((r= readsectors(mon2abs(params), lowsec+PARAMSEC, 1)) != 0) {
    readerr(lowsec+PARAMSEC, r);
    exit(1);
  }
  params[SECTOR_SIZE]= 0;
  acmds= tokenize(&cmds, params);

  /* Stuff the default action into the command chain. */
  (void) tokenize(acmds, ":;");
}

static char *addptr;

void addparm(char *n)
{
  while (*n != 0 && *addptr != 0) *addptr++ = *n++;
}

void save_parameters(void)
/* Save nondefault environment variables to the bootparams sector. */
{
  environment *e;
  char params[SECTOR_SIZE + 1];
  int r;

  /* Default filling: */
  memset(params, '\n', SECTOR_SIZE);

  /* Don't touch the 0! */
  params[SECTOR_SIZE]= 0;
  addptr= params;

  for (e= env; e != 0; e= e->next) {
    if (e->flags & E_RESERVED || is_default(e)) continue;

    addparm(e->name);
    if (e->flags & E_FUNCTION) {
      addparm("(");
      addparm(e->arg);
      addparm(")");
    } else {
      addparm((e->flags & (E_DEV|E_SPECIAL)) != E_DEV
              ? "=" : "=d ");
    }
    addparm(e->value);
    if (*addptr == 0) {
      printf("The environment is too big\n");
      return;
    }
    *addptr++= '\n';
  }

  /* Save the parameters on disk. */
  if ((r= writesectors(mon2abs(params), lowsec+PARAMSEC, 1)) != 0) {
    writerr(lowsec+PARAMSEC, r);
    printf("Can't save environment\n");
  }
}

void show_env(void)
/* Show the environment settings. */
{
  environment *e;
  unsigned more= 0;
  int c;

  for (e= env; e != 0; e= e->next) {
    if (e->flags & E_RESERVED) continue;
    if (!istty && is_default(e)) continue;

    if (e->flags & E_FUNCTION) {
      printf("%s(%s) %s\n", e->name, e->arg, e->value);
    } else {
      printf(is_default(e) ? "%s = (%s)\n" : "%s = %s\n",
        e->name, e->value);
    }

    if (e->next != 0 && istty && ++more % 20 == 0) {
      printf("More? ");
      c= getch();
      if (c == ESC || c > ' ') {
        putch('\n');
        if (c > ' ') ungetch(c);
        break;
      }
      printf("\b\b\b\b\b\b");
    }
  }
}

int numprefix(char *s, char **ps)
/* True iff s is a string of digits.  *ps will be set to the first nondigit
 * if non zero, otherwise the string should end.
 */
{
  char *n= s;

  while (between('0', *n, '9')) n++;

  if (n == s) return 0;

  if (ps == 0) return *n == 0;

  *ps= n;
  return 1;
}

int numeric(char *s)
{
  return numprefix(s, (char **) 0);
}

void ls(char *dir)
/* List the contents of a directory. */
{
  MNX(ino_t) ino;
  struct MNX(stat) MNX(st);
  char name[NAME_MAX+1];

  if (fsok == -1) fsok= r_super(&block_size) != 0;
  if (!fsok) return;

  /* (,) construct because r_stat returns void */
  if ((ino= r_lookup(ROOT_INO, dir)) == 0 || 
      (r_stat(ino, &MNX(st)), r_readdir(name)) == -1)
  {
    printf("ls: %s: %s\n", dir, unix_err(errno));
    return;
  }

  (void) r_readdir(name); /* Skip ".." too. */

  while ((ino= r_readdir(name)) != 0) printf("%s/%s\n", dir, name);
}

MNX(u32_t) milli_time(void)
{
  return get_tick() * MSEC_PER_TICK;
}

MNX(u32_t) milli_since(MNX(u32_t) base)
{
  return (milli_time() + (TICKS_PER_DAY*MSEC_PER_TICK) - base)
      % (TICKS_PER_DAY*MSEC_PER_TICK);
}

static char *Thandler;
static MNX(u32_t) Tbase, Tcount;

void unschedule(void)
/* Invalidate a waiting command. */
{
  alarm(0);

  if (Thandler != 0) {
    free(Thandler);
    Thandler= 0;
  }
}

void schedule(long msec, char *cmd)
/* Schedule command at a certain time from now. */
{
  unschedule();
  Thandler= cmd;
  Tbase= milli_time();
  Tcount= msec;
  alarm(1);
}

int expired(void)
/* Check if the timer expired for getch(). */
{
  return (Thandler != 0 && milli_since(Tbase) >= Tcount);
}

void delay(char *msec)
/* Delay for a given time. */
{
  MNX(u32_t) base, count;

  if ((count= a2l(msec)) == 0) return;
  base= milli_time();

  alarm(1);

  do {
    pause();
  } while (!interrupt() && !expired() && milli_since(base) < count);
}

enum whatfun { NOFUN, SELECT, DEFFUN, USERFUN } menufun(environment *e)
{
  if (!(e->flags & E_FUNCTION) || e->arg[0] == 0) return NOFUN;
  if (e->arg[1] != ',') return SELECT;
  return e->flags & E_RESERVED ? DEFFUN : USERFUN;
}

void menu(void)
/* By default:  Show a simple menu.
 * Multiple kernels/images:  Show extra selection options.
 * User defined function:  Kill the defaults and show these.
 * Wait for a keypress and execute the given function.
 */
{
  int c, def= 1;
  char *choice= 0;
  environment *e;

  /* Just a default menu? */
  for (e= env; e != 0; e= e->next) if (menufun(e) == USERFUN) def= 0;

  printf("\nHit a key as follows:\n\n");

  /* Show the choices. */
  for (e= env; e != 0; e= e->next) {
    switch (menufun(e)) {
    case DEFFUN:
      if (!def) break;
      /*FALL THROUGH*/
    case USERFUN:
      printf("    %c  %s\n", e->arg[0], e->arg+2);
      break;
    case SELECT:
      printf("    %c  Select %s kernel\n", e->arg[0],e->name);
      break;
    default:;
    }
  }

  /* Wait for a keypress. */
  do {
    c= getch();
    if (interrupt() || expired()) return;

    unschedule();

    for (e= env; e != 0; e= e->next) {
      switch (menufun(e)) {
      case DEFFUN:
        if (!def) break;
      case USERFUN:
      case SELECT:
        if (c == e->arg[0]) choice= e->value;
        break;
      default:
        break;
      }
    }
  } while (choice == 0);

  /* Execute the chosen function. */
  printf("%c\n", c);
  (void) tokenize(&cmds, choice);
}

void help(void)
/* Not everyone is a rocket scientist. */
{
  struct help {
    char    *thing;
    char    *help;
  } *pi;
  static struct help info[] = {
    { 0,  "Names:" },
    { "rootdev",      "Root device" },
    { "ramimagedev",  "Device to use as RAM disk image " },
    { "ramsize",      "RAM disk size (if no image device) " },
    { "bootdev",      "Special name for the boot device" },
    { "fd0, d0p2, c0d0p1s0",  "Devices (as in /dev)" },
    { "image",        "Name of the boot image to use" },
    { "main",         "Startup function" },
    { "bootdelay",    "Delay in msec after loading image" },
    { 0,  "Commands:" },
    { "name = [device] value",  "Set environment variable" },
    { "name() { ... }",   "Define function" },
    { "name(key,text) { ... }",
      "A menu option like: minix(=,Start MINIX) {boot}" },
    { "name",         "Call function" },
    { "boot [device]",      "Boot Minix or another O.S." },
    { "ctty [line]",  "Duplicate to serial line" },
    { "delay [msec]",       "Delay (500 msec default)" },
    { "echo word ...",      "Display the words" },
    { "ls [directory]",     "List contents of directory" },
    { "menu",         "Show menu and choose menu option" },
    { "save / set",   "Save or show environment" },
    { "trap msec command",  "Schedule command " },
    { "unset name ...",     "Unset variable or set to default" },
    { "exit / off",   "Exit the Monitor / Power off" },
  };

  for (pi= info; pi < arraylimit(info); pi++) {
    if (pi->thing != 0) printf("    %-24s- ", pi->thing);
    printf("%s\n", pi->help);
  }
}

void execute(void)
/* Get one command from the command chain and execute it. */
{
  token *second=0, *third=0, *fourth=0, *sep;
  char *name;
  int res;
  size_t n= 0;

  if (err) {
    /* An error occured, stop interpreting. */
    while (cmds != 0) voidtoken();
    return;
  }

  if (expired()) {  /* Timer expired? */
    parse_code(Thandler);
    unschedule();
  }

  /* There must be a separator lurking somewhere. */
  for (sep= cmds; sep != 0 && sep->token[0] != ';'; sep= sep->next)
    n++;

  name= cmds->token;
  res= reserved(name);

  if ((second= cmds->next) != 0 && (third= second->next) != 0)
    fourth= third->next;

    /* Null command? */
  if (n == 0) {
    voidtoken();
    return;
  } else
    /* name = [device] value? */
  if ((n == 3 || n == 4) &&
      !sugar(name) &&
      second->token[0] == '=' &&
      !sugar(third->token) && 
      (n == 3 || (n == 4 && third->token[0] == 'd' &&
      !sugar(fourth->token)))) {
    char *value= third->token;
    int flags= E_VAR;

    if (n == 4) { value= fourth->token; flags|= E_DEV; }

    if ((flags= b_setvar(flags, name, value)) != 0) {
      printf("%s is a %s\n", name,
        flags & E_RESERVED ? "reserved word" :
            "special function");
      err= 1;
    }
    while (cmds != sep) voidtoken();
    return;
  } else
    /* name '(arg)' ... ? */
  if (n >= 3
    && !sugar(name)
    && second->token[0] == '('
  ) {
    token *fun;
    int c, flags, depth;
    char *body;
    size_t len;

    sep= fun= third;
    depth= 0;
    len= 1;
    while (sep != 0) {
      if ((c= sep->token[0]) == ';' && depth == 0) break;
      len+= strlen(sep->token) + 1;
      sep= sep->next;
      if (c == '{') depth++;
      if (c == '}' && --depth == 0) break;
    }

    body= malloc(len * sizeof(char));
    *body= 0;

    while (fun != sep) {
      strcat(body, fun->token);
      if (!sugar(fun->token)
        && !sugar(fun->next->token)
      ) strcat(body, " ");
      fun= fun->next;
    }
    second->token[strlen(second->token)-1]= 0;

    if (depth != 0) {
      printf("Missing '}'\n");
      err= 1;
    } else
    if ((flags= b_setenv(E_FUNCTION, name,
          second->token+1, body)) != 0) {
      printf("%s is a %s\n", name,
        flags & E_RESERVED ? "reserved word" :
            "special variable");
      err= 1;
    }
    while (cmds != sep) voidtoken();
    free(body);
    return;
  } else
    /* Grouping? */
  if (name[0] == '{') {
    token **acmds= &cmds->next;
    char *t;
    int depth= 1;

    /* Find and remove matching '}' */
    depth= 1;
    while (*acmds != 0) {
      t= (*acmds)->token;
      if (t[0] == '{') depth++;
      if (t[0] == '}' && --depth == 0) { t[0]= ';'; break; }
      acmds= &(*acmds)->next;
    }
    voidtoken();
    return;
  } else
    /* Command coming up, check if ESC typed. */
  if (interrupt()) {
    return;
  } else
    /* unset name ..., echo word ...? */
  if (n >= 1 && (res == R_UNSET || res == R_ECHO)) {
    char *arg= poptoken(), *p;

    for (;;) {
      free(arg);
      if (cmds == sep) break;
      arg= poptoken();
      if (res == R_UNSET) {   /* unset arg */
        b_unset(arg);
      } else {    /* echo arg */
        p= arg;
        while (*p != 0) {
          if (*p != '\\') {
            putch(*p);
          } else
          switch (*++p) {
          case 0:
            if (cmds == sep) return;
            continue;
          case 'n':
            putch('\n');
            break;
          case 'v':
            printf(version);
            break;
          case 'c':
            clear_screen();
            break;
          case 'w':
            for (;;) {
              if (interrupt())
                return;
              if (getch() == '\n')
                break;
            }
            break;
          default:
            putch(*p);
          }
          p++;
        }
        putch(cmds != sep ? ' ' : '\n');
      }
    }
    return;
  } else
    /* boot -opts? */
  if (n == 2 && res == R_BOOT && second->token[0] == '-') {
    static char optsvar[]= "bootopts";
    (void) b_setvar(E_VAR, optsvar, second->token);
    voidtoken();
    voidtoken();
    bootminix();
    b_unset(optsvar);
    return;
  } else
    /* boot device, ls dir, delay msec? */
  if (n == 2 && (res == R_BOOT || res == R_CTTY
      || res == R_DELAY || res == R_LS)
  ) {
    if (res == R_BOOT) boot_device(second->token);
    if (res == R_CTTY) ctty(second->token);
    if (res == R_DELAY) delay(second->token);
    if (res == R_LS) ls(second->token);
    voidtoken();
    voidtoken();
    return;
  } else
    /* trap msec command? */
  if (n == 3 && res == R_TRAP && numeric(second->token)) {
    long msec= a2l(second->token);

    voidtoken();
    voidtoken();
    schedule(msec, poptoken());
    return;
  } else
    /* Simple command. */
  if (n == 1) {
    char *body;
    int ok= 0;

    name= poptoken();

    switch (res) {
    case R_BOOT:    bootminix();    ok= 1;  break;
    case R_DELAY:   delay("500");   ok= 1;  break;
    case R_LS:      ls(null);       ok= 1;  break;
    case R_MENU:    menu();   ok= 1;  break;
    case R_SAVE:    save_parameters(); ok= 1;break;
    case R_SET:     show_env();     ok= 1;  break;
    case R_HELP:    help();   ok= 1;  break;
    case R_EXIT:    exit(0);
    case R_OFF:     off();    ok= 1;  break;
    }

    /* Command to check bootparams: */
    if (strcmp(name, ":") == 0) ok= 1;

    /* User defined function. */
    if (!ok && (body= b_body(name)) != 0) {
      (void) tokenize(&cmds, body);
      ok= 1;
    }
    if (!ok) printf("%s: unknown function", name);
    free(name);
    if (ok) return;
  } else {
    /* Syntax error. */
    printf("Can't parse:");
    while (cmds != sep) {
      printf(" %s", cmds->token); voidtoken();
    }
  }

  /* Getting here means that the command is not understood. */
  printf("\nTry 'help'\n");
  err= 1;
}

int run_trailer(void)
/* Run the trailer function between loading Minix and handing control to it.
 * Return true iff there was no error.
 */
{
  token *save_cmds= cmds;

  cmds= 0;
  (void) tokenize(&cmds, "trailer");
  while (cmds != 0) execute();
  cmds= save_cmds;
  return !err;
}

void monitor(void)
/* Read a line and tokenize it. */
{
  char *line;

  unschedule();     /* Kill a trap. */
  err= 0;     /* Clear error state. */

  if (istty) printf("%s>", bootdev.name);
  line= readline();
  (void) tokenize(&cmds, line);
  free(line);
  (void) escape();  /* Forget if ESC typed. */
}

int main(int argc, char **argv)
/* Do not load or start anything, just edit parameters. */
{
  int i;
  char bootcode[SECTOR_SIZE];
  struct termios rawterm;

  istty= (argc <= 2 && tcgetattr(0, &termbuf) == 0);

  if (argc < 2) {
    fprintf(stderr, "Usage: edparams device [command ...]\n");
    exit(1);
  }

  /* Go over the arguments, changing control characters to spaces. */
  for (i= 2; i < argc; i++) {
    char *p;

    for (p= argv[i]; *p != 0; p++) {
      if ((unsigned) *p < ' ' && *p != '\n') *p= ' ';
    }
  }

  bootdev.name= argv[1];
  if (strncmp(bootdev.name, "/dev/", 5) == 0) bootdev.name+= 5;
  if ((bootdev.device= open(argv[1], O_RDWR, 0666)) < 0)
    fatal(bootdev.name);

  /* Check if it is a bootable Minix device. */
  if (readsectors(mon2abs(bootcode), lowsec, 1) != 0
    || memcmp(bootcode, boot_magic, sizeof(boot_magic)) != 0) {
    fprintf(stderr, "edparams: %s: not a bootable Minix device\n",
      bootdev.name);
    exit(1);
  }

  /* Print greeting message.  */
  if (istty) printf("Boot parameters editor.\n");

  signal(SIGINT, trap);
  signal(SIGALRM, trap);

  if (istty) {
    rawterm= termbuf;
    rawterm.c_lflag&= ~(ICANON|ECHO|IEXTEN);
    rawterm.c_cc[VINTR]= ESC;
    if (tcsetattr(0, TCSANOW, &rawterm) < 0) fatal("");
  }

  /* Get environment variables from the parameter sector. */
  get_parameters();

  i= 2;
  for (;;) {
    /* While there are commands, execute them! */
    while (cmds != 0 || i < argc) {
      if (cmds == 0) {
        /* A command line command. */
        parse_code(argv[i++]);
      }
      execute();

      /* Bail out on errors if not interactive. */
      if (err && !istty)
        exit(1);
    }

    /* Commands on the command line? */
    if (argc > 2)
      break;

    /* The "monitor" is just a "read one command" thing. */
    monitor();
  }
  exit(0);

  return 0;
}
