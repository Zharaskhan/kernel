# Simple kernel module to obtain root privileges

## How to use
### Install module
  1. `sudo su`
  1. `make`
  1. `insmod root.ko`
  
### Test
  1. `adduser testuser`
  1. `cd /dev`
  1. `id`
  1. `touch temp`
  1. `#permission error`
  1. `echo "show the mage" > ./magic`
  1. `id`
  1. `touch temp`
  1. `rm temp`
