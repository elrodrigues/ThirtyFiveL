#!/usr/bin/sh

export LC_ALL='C'

echo '== Question 1 =='
curdir=$(pwd)
dir=~eggert/src/gnu/emacs-CS-35L/
cd $dir
echo "@@ Working Files @@"
git count-objects -vH
echo "@@ Git Repository @@"
# du -sh --exclude-from=$dir/.gitignore $dir
du -sh $dir
echo "@@ Largest Files @@"
find -type f | tr "\n" "\0" | du --files0-from="-" | sort -rsn -k 1,1| head -n 5
echo

echo '== Question 2 =='
# rem=$(git branch -r | wc -l)
# loc=$(git branch | wc -l)

echo -e "Remote: "`git branch -r | wc -l`
echo -e "Local: "`git branch | wc -l`
echo

echo '== Question 3 =='
git remote -v
echo

echo '== Question 4 =='
for k in `git branch | sed s/^..//`
do
    echo -e `git log -1 --pretty=format:"%Cgreen%ci %Cblue%cr%Creset" $k --`\\t"$k"
    # -<number> is number of commits to output. -e enables backslash interp. for echo.
done | sort -r | head -n 10 # credit to commanlinefu.com
echo

echo '== Question 5 =='
# Slower way: git log master --oneline | wc -l
# or: git log master --pretty=oneline | wc -l
M=$(git rev-list --count master)
echo "Number of master commits: $M"
echo

echo '== Question 6 =='
tmp=$(mktemp -d)
touch $tmp/utop
# touch ~/utop
# <stdin> | cat <file> -
echo "Number of unique commits: "
for k in `git branch | sed s/^..//`
do
    git rev-list $k >> $tmp/utop
done
sort -u -o $tmp/utop < $tmp/utop
cnt=$(wc -l < $tmp/utop)
echo $cnt
rm -fr $tmp
echo

echo '== Question 7 =='
git log --pretty=format:"%an" --date=short --reverse --all --since=2012-12-31 \
    | sort | sed 's@([^)]*) @@g' | uniq -c | sort -nr -k 1,1 | \
    sed 's/^ *[0-9]* //' | head -n 10

#    sed 's@([^)]*) @@g' | uniq -uc | sort -n -k 1,1 | \
#    tail -n 10
echo

echo "## END ##"
cd $curdir
