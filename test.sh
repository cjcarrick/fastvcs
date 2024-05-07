hyperfine -w 20 --shell /bin/sh \
	'ZDOTDIR=./test/slowvcs/ zsh -i -c exit' \
	'ZDOTDIR=./test/fastvcs/ zsh -i -c exit' \
	'ZDOTDIR=./test/baseline/ zsh -i -c exit' \
	2>/dev/null
