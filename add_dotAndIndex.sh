set -e
# set -x
comment=$1 # -m "message"
CMD_0='git add .'
CMD_1='python createREADME.py'
CMD_2="git commit -m ${comment}"
CMD_3='git push'
echo ${CMD_2}

echo -e "\e[42;37m ${CMD_0} \e[0m "
${CMD_0}

echo -e "\e[42;37m ${CMD_1} \e[0m"
${CMD_1}
# python createREADME.py

echo -e "\e[42;37m ${CMD_2} \e[0m"
echo ${CMD_2}
set -x
git commit -m "\"$comment\""
# ${CMD_2}

exit

echo -e "\e[42;37m ${CMD_3} \e[0m"
${CMD_3}
# echo $1
# git commit -m "$1"
# git push