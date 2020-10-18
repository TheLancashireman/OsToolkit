#!/bin/sh
FAIL=0
for i in 0 20 50 23 42 69 100 ; do
	echo -n "testing ${i}: "
	./cpuload-linux ${i} | diff -q - tests/test-${i}.expected > /dev/null 2>&1
	if [ $? ] ; then
		echo "PASSED"
	else
		FAIL=1
		 echo "FAILED"
	fi
done
exit ${FAIL}

