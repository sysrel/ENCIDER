To run:
 ./run.sh test1.bc foo 2>&1 | tee o.txt

 grep -r "Found V" o.txt | wc -l
Expected result:
 11

