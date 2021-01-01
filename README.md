# ImageEditor

## Introduction

This program is a simple editor written in C++. It takes in any 24-bit bitmap file as a bytestream and applies a series of transformations. At current the transformations are as follows:

- Isolate red channel.
- Isolate green channel.
- Isolate blue channel.
- Flip vertically.
- Flip horizontally.
- Convert to greyscale.

## Input

To apply the above transformations to a file, simply save the 24-bit bitmap file to the same directory as the code and then enter the file name when prompted.
