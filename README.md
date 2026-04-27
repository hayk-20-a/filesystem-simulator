# Simple File System Simulator (C Project)

## Overview
This project is a simple file system simulator written in C. It mimics basic UNIX-like file system operations such as creating files and directories, navigating between directories, searching, and calculating disk usage.

## Features
- Create files (`touch`)
- Create directories (`mkdir`)
- Navigate file system (`cd`)
- Display disk usage (`du`)
- Search files (`find`)
- Persistent storage on exit

## Project Structure
- `main.c` → Command-line interface
- `fs.c` → Core file system logic
- `fs.h` → Data structures and function declarations

## How to Build
```bash
make
./fs
