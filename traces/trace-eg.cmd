# Demonstration of queue testing framework
# Use help command to see list of commands and options
# Initial queue is NULL.
show
# Create empty queue
new
# See how long it is
size
# Fill it with some values.  First at the head
ih dolphin
ih bear
ih gerbil
# Now at the tail
it meerkat
it jimmy
# Reverse it
reverse
# See how long it is
size
# dedup
dedup
# swap
swap
# reverseK
reverseK 3
# sort
sort
#merge
new
ih r
ih c
ih z
sort
merge
reverse
rh z
rh r
rh r
rh n
# Delete queue.  Goes back to a NULL queue.
free
# Exit program
quit
