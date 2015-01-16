#! /bin/csh

if ($#argv != 1) then
	echo "Usage: $0 <logFile>"
	exit
endif

set traceFiles=$1

set totalRequests=`grep "Request summary" $traceFiles | wc -l`
set requestsSuccessful=`grep "Request summary" $traceFiles | cut -d '|' -f6 | grep "Status: Success" | wc -l`
@ requestsFailed=$totalRequests - $requestsSuccessful


echo "Total Requests: $totalRequests"
echo "Request successful: $requestsSuccessful"
echo "Request failed: $requestsFailed"
echo ""

echo "User agents used:"
grep "Request summary" $traceFiles | cut -d '|' -f5 | sort | uniq | cut -d':' -f2

echo ""
echo "All the URL requested sucessful and the seconds streamed are in /tmp/requestsTimes.txt"
grep "Request summary" $traceFiles | grep "Status: Success" | cut -d '|' -f3,4,28 > /tmp/requestsTimes.txt
echo ""

echo "The verified errors are saved in /tmp/catraerrors.txt file"
grep "Request summary" $traceFiles | cut -d '|' -f6 | grep -v "Status: Success" | sort | uniq > /tmp/catraerrors.txt

