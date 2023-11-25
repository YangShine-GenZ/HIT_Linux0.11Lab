#!/bin/bash


sudo ./mount-hdc
cp ./producer.c ./hdc/usr/root/
cp ./consumer.c ./hdc/usr/root/
sudo umount hdc/

