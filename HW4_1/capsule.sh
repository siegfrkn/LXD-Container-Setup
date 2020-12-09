#!/usr/bin/env bash

IMAGE=${1?Error: no tar file given}

# Import the image
echo "Importing image...."
lxc image import $IMAGE --alias hw4_image &&
echo " "

# Create container hw4a
echo "Setting up container hw4a...."
lxc launch hw4_image hw4a &&
sleep 1 &&
lxc profile assign hw4a limited_network_a &&
lxc restart hw4a &&
#lxc exec hw4a -- unshare --mount --fork --pid --mount-proc /bin/bash
echo " "

# Create container hw4b
echo "Setting up container hw4b...."
lxc launch hw4_image hw4b &&
sleep 1 &&
lxc profile assign hw4b limited_network_b &&
lxc restart hw4b &&
#lxc exec hw4b -- unshare --mount --fork --pid --mount-proc /bin/bash
echo " "

# Give some time for settings to update
echo "Applying settings...."
sleep 5 &&
lxc list
echo  " "

echo "Setup Complete!"


# RESOURCES
# How much memory is lxc using https://discuss.linuxcontainers.org/t/how-can-i-tell-how-much-memory-an-lxc-container-is-using/879/4
# Known issue with lxc memory limits: https://discuss.linuxcontainers.org/t/problems-with-lxc-memory-limits/1242/9
