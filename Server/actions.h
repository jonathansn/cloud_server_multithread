#ifndef ACOES_H
#define ACOES_H

#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

#include <sys/wait.h>

int createFolder(char str[]);
int createFile(char str[]);
int copyFolder(char *sourcePath, char *destinationPath);
int copyFile(char *sourcePath, char *destinationPath);
int listDir(char str[]);
int deleteFolder(char str[]);
int deleteFile(char str[]);

#endif