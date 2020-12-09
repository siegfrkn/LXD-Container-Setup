#!/usr/bin/env bash

echo "Stopping and removing hw4a...."
lxc stop hw4a
lxc delete hw4a
echo " "

echo "Stopping and removing hw4a...."
lxc stop hw4b
lxc delete hw4b
echo " "

echo "Delete the hw4_image...."
lxc image delete hw4_image
echo " "

echo "Cleanup Complete!"