
mount -o loop /mnt/e/rescue/mx/manix.img /mnt/tmp
objdump -D microkernel/kern > microkernel/dump.txt
cp microkernel/kern /mnt/tmp/sys/
cp microkernel/kern /mnt/manRiX/sys/

objdump -D servers/console/conmngr > servers/console/condump.txt
cp servers/console/conmngr /mnt/tmp/sys/
cp servers/console/conmngr /mnt/manRiX/sys/

objdump -D servers/fs-io/fsiomngr > servers/fs-io/fsdmp.txt
cp servers/fs-io/fsiomngr /mnt/tmp/sys/
cp servers/fs-io/fsiomngr /mnt/manRiX/sys/

cp servers/busmngr/busmngr /mnt/tmp/sys/
cp servers/busmngr/busmngr /mnt/manRiX/sys/

cp servers/miscio/misciomngr /mnt/tmp/sys/
cp servers/miscio/misciomngr /mnt/manRiX/sys/

cp soft/init/init /mnt/tmp/bin/
cp soft/init/init /mnt/manRiX/bin/

cp soft/login/login /mnt/tmp/bin/
cp soft/login/login /mnt/manRiX/bin/

umount /mnt/tmp
