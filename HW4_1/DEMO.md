O RUN THE CAPSULE
./capsule.sh <tar file>

TO TIME CREATE FILE AND SEE READ WRITE
time ./create test 1

TO TEST NAMESPACE LIMITS
lxc exec hw4 -- /bin/bash
ifconfig
ps -a
ipcs

COPY OVER SERVER CLIENT FILES
cp server_hw4a* /var/lib/lxd/containers/hw4a/rootfs/jailed/apps/
cp server_hw4a* /var/lib/lxd/containers/hw4b/rootfs/jailed/apps/
cp client_hw4b* /var/lib/lxd/containers/hw4a/rootfs/jailed/apps/
cp client_hw4b* /var/lib/lxd/containers/hw4b/rootfs/jailed/apps/

TO RUN CONTAINERS
lxc exec hw4a -- unshare --mount --fork --pid --mount-proc /bin/bash
lxc exec hw4b -- unshare --mount --fork --pid --mount-proc /bin/bash

