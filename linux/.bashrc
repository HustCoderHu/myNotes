alias ..='cd ..'
## a quick way to get out of current directory ##
alias ..='cd ..'
alias ...='cd ../../../'
alias ....='cd ../../../../'
alias .....='cd ../../../../'
alias .4='cd ../../../../'
alias .5='cd ../../../../..'


## Colorize the ls output ##
alias ls='ls --color=auto'

## Use a long listing format ##
alias ll='ls -la'

## Show hidden files ##
alias l.='ls -d . .. .git .gitignore .gitmodules .travis.yml --color=auto'

## Colorize the grep command output for ease of use (good for log files)##
alias grep='grep --color=auto'
alias egrep='egrep --color=auto'
alias fgrep='fgrep --color=auto'

alias mount='mount |column -t'

# handy short cuts #
alias h='history' 
alias j='jobs -l'

# reference
# https://www.zcfy.cc/article/30-handy-bash-shell-aliases-for-linux-unix-mac-os-x