#include <stdio.h>
#include <string.h>

#define MAX_BRANCH_SIZE 64

void eat_rest_of_line(FILE*file) {
	char c;
	while (!feof(file) && c != '\n') {
		c = fgetc(file);
	}
}

int main()
{
	int has_modified_files = 0;
	int has_staged_files = 0;
	char c;
	int exit_code = 0;
	char branch[MAX_BRANCH_SIZE] = "";

	FILE *proc = popen("/usr/bin/git status --branch -uno --porcelain=v1 2>&1", "r");
	if (feof(proc)) {
		exit_code = 1;
		goto cleanup;
	}

	// catch `fatal: not a git repository...'
	if ((c = fgetc(proc)) == 'f') {
		goto cleanup;
	}

	// output from git is `## branch-name`
	if (c == '#') {
		fgetc(proc); fgetc(proc);
		fgets(branch, MAX_BRANCH_SIZE - 5, proc);
		c = fgetc(proc);
		// remove traling newline
		branch[strcspn(branch, "\n")] = 0;
	}
	

	// Scan each line of git status to determine whether there are any
	// modified files or staged changes
	while (!feof(proc)) {
		if (!has_staged_files && c != ' ') {
			has_staged_files = 1;
		}
		else if (!has_modified_files && (c == ' ') && ((c = fgetc(proc)) != ' ')) {
			has_modified_files = 1;
		}
		if (has_modified_files && has_staged_files) {
			break;
		}
		eat_rest_of_line(proc);
		c = fgetc(proc);
	}

	fputs("%F{magenta}[%F{green}", stdout);
	fputs(branch, stdout);
	if (has_modified_files) {
		fputs("%F{red} *", stdout);
	}
	if (has_staged_files) {
		fputs("%F{yellow} +", stdout);
	}
	fputs("%F{magenta}]%f", stdout);

cleanup:
	pclose(proc);
	return exit_code;
}
