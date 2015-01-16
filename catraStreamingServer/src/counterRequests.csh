#! /bin/csh

while (1)
	date
	echo "Request arrived: "
	grep "Connection arrived from" trace/SystemLogs_* | wc -l
	echo "Request closed: "
	grep "Connection from" trace/SystemLogs_* | wc -l

	echo ""

	echo "Subscriber logs: Success: "
	grep "Status: Success" trace/SubscriberLogs_* | wc -l
	echo "Subscriber logs: RTSP RTCP timeout: "
	grep "RTSP RTCP timeout" trace/SubscriberLogs_* | wc -l
	echo "Subscriber logs: Reached the end of file: "
	grep "Reached the end of file" trace/SubscriberLogs_* | wc -l
	echo "Subscriber logs: errno: 104: "
	grep "errno: 104" trace/SubscriberLogs_* | wc -l
	echo "Subscriber logs: other: "
	grep -v "Status: Success" trace/SubscriberLogs_* | grep -v "RTSP RTCP timeout" | grep -v "Reached the end of file" | grep -v "errno: 104" | wc -l

	echo "System logs: Nothing to read. File descriptor: "
	grep "Nothing to read. File descriptor" trace/SystemLogs_* | wc -l

	echo ""
	echo ""
	echo ""

	sleep 5
end

