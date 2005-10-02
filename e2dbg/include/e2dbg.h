/*
**
** e2dbg.h for The Embedded ELF debugger 
**
** Started on Sun 05 Jun 2005 17:54:01 mm
**
*/

#ifndef __E2DBG_H__
 #define __E2DBG_H__

#define		__DEBUG_E2DBG__		0

/* Some internal defined values */
//#define	E2DBG_PROMPT		"(e2dbg-"ELFSH_VERSION") "

/*
#define E2DBG_PROMPT            "\033[00;01;30m("         \
                                "\033[00;01;31m"          \
				E2DBG_ARGV0		  \
                                "\033[00;01;30xbm-"       \
                                "\033[00;01;33m"          \
                                ELFSH_VERSION             \
                                "\033[00;01;300m-"        \
                                "\033[00;01;33m"          \
                                ELFSH_RELEASE             \
                                "\033[00;01;300m-"        \
                                "\033[00;901;32mdevhell"  \
                                "\033[00;01;30m) "        \
                                "\033[00m"
*/



#define		E2DBG_NAME		"Embedded ELF Debugger"
#define		E2DBG_DYNAMIC_LINKMAP	((elfshlinkmap_t *) 1)
#define		E2DBG_ARGV0		"e2dbg"
#define		E2DBG_PROFILER_BUFLEN	256
#define		E2DBG_SCRIPT_CONTINUE	1

/* Kernel related defines */
#define		E2DBG_VSYSCALL_RETADDR	((void *) 0xFFFFE420)
#define		E2DBG_KERNELBASE	((elfsh_Addr) 0xC0000000)

/* Generic register names */
#define	        ELFSH_SSPVAR		"ssp"	/* Saved stack ptr */
#define		ELFSH_SPVAR		"sp"
#define		ELFSH_FPVAR		"fp"
#define	        ELFSH_PCVAR		"pc"

/* IA32 registers namse */
#define		ELFSH_EAXVAR		"eax"
#define		ELFSH_EBXVAR		"ebx"
#define		ELFSH_ECXVAR		"ecx"
#define		ELFSH_EDXVAR		"edx"
#define	        ELFSH_ESIVAR		"esi"
#define	        ELFSH_EDIVAR		"edi"

/* ia32 under solaris */
#if (defined(__i386) && defined(sun))
#define		REG_UESP	UESP
#define		REG_EIP		EIP
#define		REG_EAX		EAX
#define		REG_EFL		EFL
#define		REG_ECX		ECX
#define		REG_EDX		EDX
#define		REG_EBX		EBX
#define		REG_ESP		ESP
#define		REG_EBP		EBP
#define		REG_ESI		ESI
#define		REG_EDI		EDI
#endif



/* Debugger commands */
#define		CMD_MODE		"mode"
#define		CMD_LINKMAP		"linkmap"
#define		CMD_BT			"bt"
#define		CMD_BT2			"backtrace"
#define		CMD_BP			"b"
#define		CMD_WATCH		"watch"
#define		CMD_BP2			"break"
#define		CMD_STACK		"stack"
#define		CMD_DBGSTACK		"dbgstack"
#define		CMD_DUMPREGS		"dumpregs"
#define		CMD_STEP		"step"
#define		CMD_DELETE		"delete"
#define		CMD_CONTINUE		"continue"
#define		CMD_CONTINUE2		"cont"
#define		CMD_RUN			"run"
#define		CMD_DISPLAY		"display"
#define		CMD_UNDISPLAY		"undisplay"
#define		CMD_RSHT		"rsht"
#define		CMD_RPHT		"rpht"

//sigaction(SIGSEGV, &ac, NULL);			    


/* Signal handling for debugger */
#define		CLRSIG	do {				\
 struct sigaction ac;					\
							\
 memset(&ac, 0x00, sizeof(ac));				\
 ac.sa_flags       = SA_SIGINFO;			\
 ac.sa_sigaction   = e2dbg_internal_sigsegv_handler;	\
 signal(SIGINT, SIG_IGN);				\
 signal(SIGTRAP, SIG_IGN);				\
}		while (0);

#define		SETSIG	do {				\
 struct sigaction ac;					\
							\
 memset(&ac, 0x00, sizeof(ac));				\
 ac.sa_flags       = SA_SIGINFO;			\
 ac.sa_sigaction   = e2dbg_generic_breakpoint;		\
 sigaction(SIGTRAP, &ac, NULL);				\
 /*ac.sa_sigaction   = e2dbg_sigsegv_handler;*/		\
 /*sigaction(SIGSEGV, &ac, NULL);*/				\
 ac.sa_sigaction   = e2dbg_sigint_handler;		\
 sigaction(SIGINT, &ac, NULL);				\
}		while (0);



/* Create variable from register value */
#define			VM_GETREG(name, val)		\
do							\
{							\
  elfshpath_t		*r;				\
  r = hash_get(&vars_hash, name);			\
  if (!r)						\
    {							\
      r = vm_create_LONG(1, val);			\
      hash_add(&vars_hash, name, r);			\
    }							\
  else							\
    r->immed_val.ent = val;				\
}							\
while (0)

/* Set the register value */
#define			VM_SETREG(name, val)		\
do							\
{							\
  elfshpath_t		*r;				\
  r = hash_get(&vars_hash, name);			\
  if (!r)						\
    {							\
      r = vm_create_LONG(1, val);			\
      hash_add(&vars_hash, name, r);			\
    }							\
  else							\
   val = r->immed_val.ent;	                        \
}							\
while (0)



/* This structure contains the internal data of the debugger placed in the VM */
typedef struct		s_e2dbgworld
{
  char			preloaded;			/* Say if we were preloaded */
  hash_t		bp;				/* Breakpoints hash table */
  
#define			E2DBG_STEPCMD_MAX	50
  char			*displaycmd[E2DBG_STEPCMD_MAX];	/* Commands to be executed on step */
  u_short		displaynbr;			/* Number of global display cmd */

  /* Current e2dbg state information */
  e2dbgcontext_t	dbgcontext;			/* Current e2dbg scripting context */
  ucontext_t		*context;			/* Current debuggee context at bp */
  elfshbp_t		*curbp;				/* Current breakpoint if any */
  u_char		step;				/* Stepping flag */
  u_char		sourcing;			/* We are executing a debugger script */

}			e2dbgworld_t;


/* The Debugger world in the VM */
extern e2dbgworld_t	e2dbgworld;

/* e2dbg hooks */
void            e2dbg_default_getregs();
void            e2dbg_default_setregs();
elfsh_Addr*     e2dbg_default_getpc();
void            e2dbg_default_setstep();
void            e2dbg_default_resetstep();
void            e2dbg_setup_hooks();
void            e2dbg_get_regvars_ia32_bsd();
void            e2dbg_get_regvars_ia32_sysv();
void            e2dbg_set_regvars_ia32_bsd();
void            e2dbg_set_regvars_ia32_sysv();
int		e2dbg_getregs();
int		e2dbg_setregs();
elfsh_Addr*     e2dbg_getpc();
int             e2dbg_setstep();
int             e2dbg_resetstep();
elfsh_Addr*     e2dbg_getpc_bsd_ia32();
elfsh_Addr*     e2dbg_getpc_sysv_ia32();
void            e2dbg_setstep_bsd_ia32();
void            e2dbg_setstep_sysv_ia32();
void            e2dbg_resetstep_sysv_ia32();
void            e2dbg_resetstep_bsd_ia32();
int		e2dbg_register_sregshook(u_char at, u_char ht, u_char ost, void *f);
int		e2dbg_register_gregshook(u_char at, u_char ht, u_char ost, void *f);
int		e2dbg_register_getpchook(u_char at, u_char ht, u_char ost, void *f);
int		e2dbg_register_setstephook(u_char at, u_char ht, u_char ost, void *f);
int		e2dbg_register_resetstephook(u_char at, u_char ht, u_char ost, void *f);

/* e2dbg API */
elfshbp_t	*vm_get_bp_by_id(uint32_t bpid);
int		vm_linkmap(elfshobj_t *file);
int		vm_bt();
int		vm_bp_add(elfsh_Addr addr, u_char flags);
int		vm_dumpstack(uint32_t size, elfsh_Addr start);
elfshobj_t	*vm_get_parent_object(elfsh_Addr addr);
int		vm_display(char **cmd, u_int nbr);
char		*vm_get_prompt();
char		*vm_get_mode_name();
int		vm_restore_dbgcontext(int, char, elfshargv_t*, void *, char **, char*);
void		e2dbg_set_regvars();
void		e2dbg_get_regvars();
int		vm_is_watchpoint(elfshbp_t *b);
elfshbp_t	*vm_lookup_bp(char *name);

/* e2dbg context switching fonctions */
void		e2dbg_init(void) __attribute__((constructor));
int		e2dbg_setup(char *name);
int		e2dbg_entry(int ac, char **av);
void            e2dbg_sigsegv_handler(int signum, siginfo_t *info, void *context);
void            e2dbg_internal_sigsegv_handler(int signum, siginfo_t *info, void *pcontext);
void            e2dbg_sigint_handler(int signum, siginfo_t *info, void *context);
void            e2dbg_sigtrap_handler(int signum, siginfo_t *info, void *context);
void		e2dbg_generic_breakpoint(int signum, siginfo_t *info, void *context);

/* e2dbg commands */
int             cmd_mode();
int             cmd_linkmap();
int             cmd_bt();
int             cmd_stack();
int             cmd_dbgstack();
int             cmd_run();
int             cmd_bp();
int             cmd_watch();
int             cmd_display();
int             cmd_undisplay();
int             cmd_delete();
int             cmd_step();
int		cmd_dumpregs();

#endif
