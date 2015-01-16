#!/bin/csh

set noPacketsSent = `grep "Sent 0 Video RTP Packets and 0 Audio RTP Packets" trace/SystemLogs_0001_.trace trace/SystemLogs_0002_.trace trace/SystemLogs_0003_.trace | wc -l`

set packetsSent = `grep "Video RTP Packets and " trace/SystemLogs_0001_.trace trace/SystemLogs_0002_.trace trace/SystemLogs_0003_.trace | grep -v "Sent 0 Video RTP Packets and 0 Audio RTP Packets" | wc -l`

set totalPackets = `grep "Video RTP Packets and " trace/SystemLogs_0001_.trace trace/SystemLogs_0002_.trace trace/SystemLogs_0003_.trace | wc -l`

set suggestedTimesMoreThenMax = `grep "time < _ulSendRTPMaxSleepTimeInMilliSecs" trace/SystemLogs_0001_.trace trace/SystemLogs_0002_.trace trace/SystemLogs_0003_.trace | wc -l`

echo "noPacketsSent: $noPacketsSent"
echo "packetsSent: $packetsSent"
echo "totalPackets: $totalPackets"
echo "suggestedTimesMoreThenMax: $suggestedTimesMoreThenMax"

