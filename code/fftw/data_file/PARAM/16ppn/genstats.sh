grep -n -A 54 -x "Communication Activity" stats_block_8n_32ppn_1024.txt | grep -w "Send\|Recv\|Wait\|SendRecv\|Reduce" > block.txt

grep -n -A 54 -x "Communication Activity" stats_non_1l_8n_32ppn_1024.txt | grep -w "Send\|Recv\|Wait\|SendRecv\|Reduce" > non_1l.txt

grep -n -A 54 -x "Communication Activity" stats_non_2l_8n_32ppn_1024.txt | grep -w "Send\|Recv\|Wait\|SendRecv\|Reduce" > non_2l.txt

grep -n -A 54 -x "Communication Activity" stats_non_4l_8n_32ppn_1024.txt | grep -w "Send\|Recv\|Wait\|SendRecv\|Reduce" > non_4l.txt
