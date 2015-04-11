#ifndef __LINUXICE_COMMAND__
#define __LINUXICE_COMMAND__

typedef int (*PFN_EXEC_CMD)(char* arg);

struct ice_command{
	const char*		command;
	PFN_EXEC_CMD	func;
	const char*		helpline;
};

const char* get_helpline(const char* cmd);
int exec_command(const char* cmd);
#endif
