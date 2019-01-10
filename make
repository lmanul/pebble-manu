#!/usr/bin/python3

import os

os.chdir("..")
os.system("cp -r pebble-manu pebblemanu")
os.chdir("pebblemanu")
os.system("rm -rf .git make *.swp")
os.chdir("..")
os.system("mkzip pebblemanu")
os.system("mv pebblemanu.zip pebblemanu.pbw")
os.system("rm -rf pebblemanu")

print("pbw file is in parent dir!")
