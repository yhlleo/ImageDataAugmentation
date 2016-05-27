/*
   Find and generate a file list of the folder.

   - Editor: Yahui Liu.
   - Data:   2016-03-27
   - Email:  yahui.cvrs@gmail.com
   - Address: Computer Vision and Remote Sensing(CVRS), Lab.
*/

#ifndef FIND_FILES_H
#define FIND_FILES_H
#pragma once

#include <Windows.h>
#include<iostream>
#include <fstream>
#include <vector>
#include <string>
#include <direct.h>
#include <stdlib.h>  
#include <stdio.h> 

using namespace std;

class FindFiles
{
public:
	FindFiles(){}
	~FindFiles(){}

public:
	std::vector<std::string> file_lists;

public:
	std::vector<std::string> findFiles(char* lpPath, char* secName = ".*");

};

#endif // FIND_FILES_H