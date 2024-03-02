# Demonstration of queue testing framework
# Use help command to see list of commands and options
# Initial queue is NULL.
show

# Test of insert_head, insert_tail, delete duplicate, sort, descend and reverseK

new
ih RAND 4
it gerbil 3
it lion 2
it zebra 2
# sort
sort
# dedup
# dedup
free

new
ih a
ih b
ih c
ih d
ih a
ih c
# descend
descend
rh d
rh c
rh b
rh a
free

new
ih a 3
ih b
ih c
ih d
ih e 2
# reverseK
reverseK 3
rh d
rh e
rh e
rh a
rh b
rh c
rh a
rh a

# Delete queue.  Goes back to a NULL queue.
free
# Exit program
quit
