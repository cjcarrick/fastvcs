#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

char *GIT_CMD[] = { "/bin/git", "status", "-zbu", "--porcelain=v2", NULL };

void
die(const char *msg)
{
	perror(msg);
	exit(1);
}

void
put(const char *s)
{
	while (*s)
		putchar(*s++);
}

int main()
{
	char buf[BUFSIZ];
	const char *line = buf;
	int nread;

	const char *branch = 0;
	int has_staged = 0;
	int has_unstaged = 0;

	int fds[2], child;
	if (pipe(fds)) {
		die("pipe");
	}

	child = fork();
	if (child < 0) {
		die("fork");
	}
	if (child == 0) {
		close(fds[0]);
		dup2(fds[1], STDOUT_FILENO);
		dup2(fds[1], STDERR_FILENO);
		execve(GIT_CMD[0], GIT_CMD, NULL);
		die("execve");
	}

	nread = read(fds[0], buf, sizeof(buf));

	/* "fatal: not a git repository..." */
	if (buf[0] == 'f')
		return 0;

	while (line < buf + nread) {
		if (!branch && !strncmp(line, "# branch.head ", strlen("# branch.head ")))
			branch = line + strlen("# branch.head ");
		has_unstaged = has_unstaged || ((line[0] == '1' || line[0] == '2') && line[2] == 'M');
		has_staged = has_staged || ((line[0] == '1' || line[0] == '2') && line[3] == 'M');
		if (branch && has_staged && has_unstaged)
			break;
		line = strchr(line, 0);
		++line;
	}

	#define writes(s) write(STDOUT_FILENO, s, strlen(s))
	writes(" %F{8}[%F{green}");
	writes(branch);
	if (has_staged)
		writes("%F{red} *");
	if (has_unstaged)
		writes("%F{yellow} +");
	writes("%F{8}]");

	return 0;
}
