#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define GIT_REPO_FOLDER ".git"
char path[255 + sizeof(GIT_REPO_FOLDER)];

char *GIT_CMD[] = { "/bin/git", "--git-dir", (char*)&path, "status", "-zbu", "--porcelain=v2", NULL };

void
die(const char *msg)
{
	perror(msg);
	exit(1);
}

int
findgit(void)
{
	char *p;

	if (!getcwd(path, sizeof(path)))
		die("getcwd");
	p = path + strlen(path);
	*p++ = '/';

	while (p > path) {
		/* append ".git" to cwd */
		memcpy(p, GIT_REPO_FOLDER, sizeof(GIT_REPO_FOLDER));

		if (access(path, R_OK) == 0) {
			return 1;
		}

		/* skip trailing '\0' and '/' */
		p -= 2;

		/* pop chars until we reach the next trailing '/' */
		while (p > path && *p != '/')
			--p;

		/* but keep the trailing '/' */
		++p;
	}

	return 0;
}

int
main(int argc, char **argv)
{
	char buf[BUFSIZ];
	const char *line = buf;
	int nread;

	const char *branch = 0;
	int has_staged = 0;
	int has_unstaged = 0;

	int fds[2], child;

	if (!findgit())
		return 0;

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
	writes(argv[1]);
	writes(branch);
	if (has_staged)
		writes(argv[2]);
	if (has_unstaged)
		writes(argv[3]);
	writes(argv[4]);

	return 0;
}
