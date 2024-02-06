# NUFS: a FUSE Filesystem Driver
##  Overview
NUFS (Northeastern University Filesystem) a [FUSE](https://libfuse.github.io/doxygen/) (Filesystem in Userspace) filesystem driver for Linux. The driver allows you to mount a 1MB disk image as a filesystem, providing basic and advanced file system functionalities. 

This project was the third challenge assignment for [CS3650 Computer Systems](https://searchneu.com/NEU/202110/search/CS3650) at Northeastern University.

## Table of Contents
- [Features](#features)
- [Usage](#usage)
- [Contribution](#contribution)

## Features
### Basic Features
- Create files.
- List files in the filesystem root directory.
- Write to small files (under 4k).
- Read from small files (under 4k).
- Delete files.
- Rename files.

### Advanced Features
- Create hard links (multiple names for the same file blocks).
- Read and write from files larger than one block.
- Create directories and nested directories (limited by space).
- Remove directories.
- Support metadata (permissions and timestamps) for files and directories.

## Usage
To use the NUFS, follow the steps below:
1. **Launch Linux Environment**:

    Launch any version of Debian/Ubuntu or any other local modern Linux distribution.
    Ensure that your working directory is a proper Linux filesystem, not a remote-mounted Windows or Mac directory.
2. **Clone Repository**:

    Clone this GitHub repository to your preferred location. This is where the filesystem will be mounted.
3. **Install Required Packages**:

    Before starting, ensure that FUSE is installed on your local Debian/Ubuntu VM. You can install the required package using the following command:
    ```bash
    sudo apt-get install gcc pkg-config make libbsd-dev fuse libfuse-dev
    ```
4. **Prepare for Mounting**:

    In the directory where you cloned the project, enter the following command to prepare for mounting:
    ```bash
    make clean
    ```
5. **Mount NUFS**:

    In the same directory, enter the following command to mount NUFS:
    ```bash
    make mount
    ```
6. **Navigate to Root Folder**:
    
    Open a separate terminal window, then navigate to the 'mnt' folder. This is the root folder of the filesystem.
    ```bash
    cd mnt
    ```
Now, you can use standard Linux filesystem operations (e.g., mkdir, rename, chmod, etc.) listed above to interact with the NUFS filesystem.

## Contribution
This project was completed in collaboration with my partner aditya in Spring 2021.