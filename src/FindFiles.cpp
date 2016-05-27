#include "FindFiles.h"

std::vector<std::string> FindFiles::findFiles(char* lpPath, char* secName/* = ".*"*/)
{
	char szFind[MAX_PATH];
	char szFile[MAX_PATH];

	WIN32_FIND_DATA FindFileData;

	strcpy(szFind,lpPath);
	strcat(szFind,"\\*");
	strcat(szFind,secName);

	HANDLE hFind=::FindFirstFile(szFind,&FindFileData);

	if(INVALID_HANDLE_VALUE == hFind)
	{
		std::cout << "Empty folder!" << std::endl;
		return std::vector<std::string>();
	}

	do
	{
		if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if(FindFileData.cFileName[0]!='.')
 			{
				strcpy(szFile,lpPath);
				strcat(szFile,"\\");
				strcat(szFile,FindFileData.cFileName);
				findFiles(szFile);
			}
		}
		else
		{
			if ( szFile[0] )
			{
				std::string filePath = lpPath;
				filePath += "\\";
				filePath += FindFileData.cFileName;
				file_lists.push_back(filePath);
			}
			else
			{
				std::string filePath = szFile;
				filePath += FindFileData.cFileName;
				file_lists.push_back(filePath);
			}

//			std::cout << szFile << " " << FindFileData.cFileName << std::endl;
		}
	}while(::FindNextFile(hFind,&FindFileData));

	::FindClose(hFind);
	return file_lists;
}
