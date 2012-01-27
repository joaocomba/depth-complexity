#!/bin/bash

echo "To: $1" > toSend.txt
echo "From: warning.giver@gmail.com" >> toSend.txt
echo "Subject: Depth-complexity test finished" >> toSend.txt
echo "The depth-complexity test is done." >> toSend.txt

msmtp -t < toSend.txt

rm -f toSend.txt


