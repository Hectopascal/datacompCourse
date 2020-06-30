for f in ./tests/*.txt
do
    if [[ ${f} =~ ./([0-9]+)[a-z]*.txt ]]
    then
        echo $f
        ./aencode < $f | ./adecode > result${BASH_REMATCH[1]}.txt
        diff $f result${BASH_REMATCH[1]}.txt
    fi
done
