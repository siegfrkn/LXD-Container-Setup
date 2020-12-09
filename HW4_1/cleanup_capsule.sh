#!/usr/bin/env bash

lxc stop hw4a
lxc delete hw4a

lxc stop hw4b
lxc delete hw4b

lxc image delete hw4_image