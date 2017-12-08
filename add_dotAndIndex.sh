set -x 
git add .
echo 
python createREADME.py
echo
echo $1
git commit -m "$1"
git push