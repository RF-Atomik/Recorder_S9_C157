gnome-terminal --working-directory="/home/odroidh2/Documents/Recorder_S9_C157" -- /bin/sh -c 'sleep 5; ./upload; exec bash'
gnome-terminal --working-directory="/home/odroidh2/Documents/Recorder_S9_C157" -- /bin/sh -c 'sleep 5;./recorder -c 1536p -l 3600 -i -a -cd X264 -cr 2 -fps 5 ; exec bash'
