// Tiny_File_System.cpp
#include "stdafx.h"
#include <iostream>
#include <time.h>
using namespace std;
const int N = 4;

class FileDirectory
{
	unsigned char fileDirectory[N][32];
	//File Name [7:0]
	//Extension [8:10]
	//Date [25:24]
	//Time [23:22]
	//Address [27:26]
	//File Size [31:28]
	unsigned short int FAT16[256];
	unsigned char data[1024];

public:

	FileDirectory()
	{
		//Constructor to initialize all arrays to 0
		for (int i = 0; i < N; i++)
			for (int j = 0; j < 32; j++)
				fileDirectory[i][j] = 0;

		for (int i = 0; i < 256; i++)
			FAT16[i] = 0;

		for (int i = 0; i < 1024; i++)
			data[i] = 0;
	}
	
	bool Create(const char filename[], int numberBytes)
	{
		int i;
		//Check for unused entry in File Directory
		for (i = 0; i < N; i++)	//Check all directories
		{
			//return false if first character is used (fileDirectory[i][7] = first character)
			if (fileDirectory[i][7] != 0)
				continue;
		}
		if (i = N - 1)
			return false;

		//Check for enough unused clusters
		int count = 0;
		int numClusters = 0;
		for (int i = 0; i < 256; i++)
		{
			if(FAT16[i] != 0 || FAT16[i] != 1)	//if FAT16[i] != 0 or != 1, its used up
				count++;		//count number of used bytes
		}
		numClusters = (count + 3) / 4; //Count number of used clusters
		if (numClusters >(numberBytes + 3) / 4)	//if number of used clusters > needed clusters, its too full
			return false;

		//If false checks fail, return true
		return true;
	}

	bool Delete(const char filename[])
	{
		//firstClusterAddress to find where to start deleting
		//counter to count used bytes
		//clusters array to find which clusters in FAT16 to write
		//numClusters to know how many clusters to delete in FAT16
		unsigned short int firstClusterAddress, counter, clusters[256], numClusters;

		//Check if file to be deleted is in directory
		for (int i = 0; i < N; i++)	//Check all directories
		{
				if (fileDirectory[i][7] != filename[7])	//if name isnt in directory
					return false;
				//Change the first character of the file name in the directory to 0
				else if (fileDirectory[i][7] == filename[7])
					fileDirectory[i][7] = 0;
		}

		//Change all entries of the clusters of this file in the FAT to 1
		//Find first cluster address
		for (firstClusterAddress = 2; firstClusterAddress < 256; firstClusterAddress++)
		{
			if (FAT16[firstClusterAddress] == 0 || FAT16[firstClusterAddress] == 1);
			break;
		}

		//Next unused entry as next cluster address. Write into FAT16
		counter = 1;
		clusters[0] = firstClusterAddress;
		for (int i = firstClusterAddress + 1; i < 256; i++)
		{
			if (FAT16[i] != 0 || FAT16[i] != 1)
			{
				clusters[counter] = i;	//Write used clusters
				counter++;	//Count number of used bytes
			}
		}
		numClusters = (counter + 3) / 4;	//Find number of used clusters
		//Repeat until all clusters are found and FAT16 is updated
		for (int i = 0; i < numClusters - 1; i++)
			FAT16[clusters[i]] = 1;	//Change all entries of fat to 1

		return true;
	}

	int Read(const char filename[], char data1[])
	{
		unsigned short int numBytes = 0;	//number of bytes read
		unsigned short int date,time,firstClusterAddress,counter,clusters[256],numClusters;
		//Check if file name is in directoy, if not return 0
		for (int i = 0; i < N; i++)	//Check all directories
		{
				if (fileDirectory[i][7] != filename[7])	//if name isnt in directory
					return 0;
		}

		//Use file name to get file info from directory
		//date[25:24], time[23:22], number bytes, and first cluster address[FAT16]
		for (int i = 0; i < N; i++)	//Check all directories
		{
			if (fileDirectory[i][7] == filename[7])
			{	//if name is in directory
				date = fileDirectory[i][25] + fileDirectory[i][24];
				time = fileDirectory[i][23] + fileDirectory[i][22];

				//Find first cluster address
				for (firstClusterAddress = 2; firstClusterAddress < 256; firstClusterAddress++)
				{
					if (FAT16[firstClusterAddress] == 0 || FAT16[firstClusterAddress] == 1);
					break;
				}

				//Find number of bytes
				for (int i = firstClusterAddress; i < 256; i++)
				{
					if (FAT16[i] != 0 || FAT16[i] != 1)
						numBytes++;
				}

				//Use first cluster address to get all the cluster addresses of this file from FAT16
				numClusters = (numBytes + 3) / 4;
				counter = 1;
				clusters[0] = firstClusterAddress;
				for (int i = firstClusterAddress + 1; i < 256; i++)
				{
					if (FAT16[i] == 0 || FAT16[i] == 1)
					{
						clusters[counter] = i;
						counter++;
					}
				}

				//use all the cluster addresses to read the data from the disk and store the data into the data1 array
				for (int i = 0; i < numClusters - 1; i++)
					data1[i] = FAT16[clusters[i]];

				//return the number of data bytes read (length of file)
				return numBytes;
			}

			else
				return 0;

		}

	}

	bool Write(const char filename[], int numberBytes, char data[], int year, int month, int day, int hour, int minute, int second)
	{
		//firstClusterAddress to find where to start writing
		//counter to count used bytes
		//clusters array to find which clusters in FAT16 to write
		//numClusters to know how many clusters to write in FAT16
		//n to store where the file begins in the directory
		unsigned short int firstClusterAddress, numClusters, counter, time, date, clusters[256], n;
		
		// First unused entry in the FAT16 use as first cluster address
		for (firstClusterAddress = 2; firstClusterAddress < 256; firstClusterAddress++)
		{
			if (FAT16[firstClusterAddress] == 0 || FAT16[firstClusterAddress] == 1);
			break;
		}

		//Next unused entry as next cluster address. Write into FAT16
		numClusters = (numberBytes + 3) / 4;
		counter = 1;
		clusters[0] = firstClusterAddress;
		for (int i = firstClusterAddress + 1; i < 256; i++)
		{
			if (FAT16[i] == 0 || FAT16[i] == 1)
			{
				clusters[counter] = i;
				counter++;
			}
		}

		//Repeat until all clusters are found and FAT16 is updated
		for (int i = 0; i < numClusters - 1; i++)
			FAT16[clusters[i]] = clusters[i + 1];
		FAT16[clusters[numClusters]] = EOF;

		//If not enough unused clusters return false
		if (counter < numClusters)
			return false;

		//Write file name, extension, date, time, length, and first cluster address
		for (n = 0; n < N; n++)	//Look for unused entry
		{
			if (fileDirectory[n][0] == 0)
				break;
		}
		if (n == N)
			return false;	//if no unused return false

		//Write name
		for (int i = 0; i < 8; i++)	//First 8 bits are name
			fileDirectory[n][i] = filename[i];	//fileName[7:0]
		//Write extension
		for (int i = 8; i < 11; i++)
			fileDirectory[n][i] = filename[i+4];	//Extension[10:8]
		//Write date
		date = (year - 1980) << 11 + (month << 5) + day;
		fileDirectory[n][25] = date >> 8;
		//Write time
		time = (hour << 11) + (minute << 5) + second / 2;
		fileDirectory[n][22] = time; //less significant
		fileDirectory[n][23] = time >> 8;	//more significant
		//Write length
		fileDirectory[n][28] = numberBytes;
		fileDirectory[n][29] = numberBytes >> 8;
		fileDirectory[n][30] = numberBytes >> 16;
		fileDirectory[n][31] = numberBytes >> 24;
		//Write first cluster address
		fileDirectory[n][26] = firstClusterAddress;	//less significant
		fileDirectory[n][27] = firstClusterAddress >> 8;	//more significant

		return true;
	}

	bool Copy(const char filename[], const char filename1[])
	{
		int numBytes;
		char data[N];
		int year, month, day, hour, minute, second;

		//Read filename
		numBytes = Read(filename, data);
		if (numBytes == 0)
			return false;	//if file empty return false
		//Write read data from filename into filename1
		year = 2020;
		month = 6;
		day = 20;
		hour = 20;
		minute = 15;
		second = 30;

		Write(filename1, numBytes, data, year, month, day, hour, minute, second);	//write filename1
		if (Write(filename1, numBytes, data, year, month, day, hour, minute, second) == false)	//If write fails return false
			return false;
		return true;
	}

	bool Merge(const char filename1[], const char filename2[], const char filename[])
	{
		//Copy data from filename1 and filename2 into filename
		unsigned short int numBytes1, numBytes2, numBytes = 0;
		char data1[N], data2[N], data[N];
		int year, month, day, hour, minute, second;

		//Read file1
		numBytes1 = Read(filename1, data1);
		if (numBytes == 0)
			return false;
		//Read file2
		numBytes2 = Read(filename2, data2);
		if (numBytes == 0)
			return false;

		//Merge
		numBytes = numBytes1 + numBytes2;
		for (int i = 0; i < numBytes1; i++)
			data[i] = data1[i];	//Write data from numBytes1 at beginning
		for (int i = 0; i < numBytes2; i++)
			data[i + numBytes1] = data2[i];	//Write data after numBytes1

		year = 2020;
		month = 5;
		day = 15;
		hour = 18;
		minute = 45;
		second = 50;

		Write(filename, numBytes, data, year, month, day, hour, minute, second);
		if (Write(filename, numBytes, data, year, month, day, hour, minute, second) == false)
			return false;
		return true;
	}

	void printClusters(const char filename[])
	{
		unsigned short int numberBytes, firstClusterAddress,numClusters,counter,clusters[256];
		numberBytes = 0;
		//Check if the file to be printed, ,filename[], is not in the directory
		//if not, print "Filename[] is not in the directory
		for (int i = 0; i < N; i++)	//Check all directories
		{
				if (fileDirectory[i][7] != filename[7])	//if file isnt in directory
				{
					cout << filename << " is not in the directory" << endl;
					break;
				}
				else if (fileDirectory[i][7] == filename[7])
				{
					//Use the file name to get the file information from the File Directory, including the first
					//cluster address
					firstClusterAddress = fileDirectory[i][26] + (fileDirectory[i][27] << 8);
					break;
				}
		}

		//Use the first cluster address to get all cluster addresses from the FAT-16
		numClusters = (numberBytes + 3) / 4;
		counter = 1;
		clusters[0] = firstClusterAddress;
		for (int i = firstClusterAddress + 1; i < 256; i++)
		{
			if (FAT16[i] == 0 || FAT16[i] == 1)
			{
				clusters[counter] = i;
				counter++;
			}
		}
		//Print all the clusters of this file, and add “->” between clusters
		for (int i = 0; i < numClusters - 1; i++)
			cout << clusters[i] << "->";
		cout << endl;
	}

	void printDirectory()
	{
		unsigned short int date, time;
		unsigned char name,hour, minute, second,year,month,day;
		//Print all the files with valid file names in this directory, including their names, date, time
		//file length, and first cluster address

		//Note all valid file names have ASCII characters; invalid file names have a null character in 
		//the first character of the file name

		for (int i = 0; i < N; i++)
		{
			if (fileDirectory[i][7] != 0)
			{
				date = fileDirectory[i][25] << 8;
				time = fileDirectory[i][22] + (fileDirectory[i][23] << 8);
				name = fileDirectory[i][7] + fileDirectory[i][6] + fileDirectory[i][5]
					+ fileDirectory[i][4] + fileDirectory[i][3] + fileDirectory[i][2]
					+ fileDirectory[i][1] + fileDirectory[i][0];

				hour = time >> 11;
				minute = time >> 5;
				second = (time & 0x1f) >> 1;

				year = (date >> 11) + 1980;
				month = date >> 5;
				day = date >> 1;
				
				cout << name << "	" << day << "/" << month << "/" << year << "	" << hour << ":" << minute << ":" << second << endl;
			}

		}
	}

	void printData(char filename[])
	{
		unsigned short int date, time, numberBytes, firstClusterAddress,numClusters,counter,clusters[256];
		unsigned char name, hour, minute, second, year, month, day;
		//Use the file name to get the file information from the File Directory, including the first
		//cluster address
		for (int i = 0; i < N; i++)	//Check all directories
		{
				if (fileDirectory[i][7] == filename[7])	//if file is in directory
				{
					date = fileDirectory[i][25] << 8;
					time = fileDirectory[i][22] + (fileDirectory[i][23] << 8);
					name = fileDirectory[i][7] + fileDirectory[i][6] + fileDirectory[i][5]
						+ fileDirectory[i][4] + fileDirectory[i][3] + fileDirectory[i][2]
						+ fileDirectory[i][1] + fileDirectory[i][0];

					hour = time >> 11;
					minute = time >> 5;
					second = (time & 0x1f) >> 1;

					year = (date >> 11) + 1980;
					month = date >> 5;
					day = date >> 1;

					numberBytes = fileDirectory[i][28] + fileDirectory[i][29] + fileDirectory[i][30] + fileDirectory[i][31];
					firstClusterAddress = fileDirectory[i][26] + fileDirectory[i][27];
				}
		}
		//Use the first cluster address to get all cluster addresses from the FAT - 16
		numClusters = (numberBytes + 3) / 4;
		counter = 1;
		clusters[0] = firstClusterAddress;
		for (int i = firstClusterAddress + 1; i < 256; i++)
		{
			if (FAT16[i] == 0 || FAT16[i] == 1)
			{
				clusters[counter] = i;
				counter++;
			}
		}
		//Use cluster addresses to read the data of the file. Use the file length to print these data
		//in hexadecimal format.
		for (int i = 0; i < numClusters - 1; i++)
		{
			cout << hex << FAT16[clusters[i]] << " ";
		}

	}
};

int main()
{
	//Driver function to test program
	int year, month, day, hour, minute, second;
	char data[1024];

	year = 2020;
	month = 4;
	day = 13;
	hour = 19;
	minute = 33;
	second = 20;

	// create and write a file, file1.cpp, of 200 bytes
	FileDirectory d;
	d.Create("file1.cpp", 200);
	d.Write("file1.cpp", 200, data, year, month, day, hour, minute, second);
	d.printDirectory();
	d.printClusters("file1.cpp");

	//copy file1.cpp into file2.cpp
	d.Copy("file1.cpp", "file2.cpp");
	d.printDirectory();
	d.printClusters("file2.cpp");

	//create and write a file, file3.asm, of 400 bytes
	d.Create("file3.asm", 400);

	d.Write("file3.asm", 400, data, year, month, day, hour, minute, second);
	d.printDirectory();
	d.printClusters("file3.asm");

	//merge file1.cpp and file3.asm into, file4.obj
	d.Merge("file1.cpp", "file3.asm", "file4.obj");
	d.printDirectory();
	d.printClusters("file4.obj");

	//delete file2.c
	d.Delete("file2.cpp");

	//merge file1.cpp and file3.asm into, file4.obj
	d.Merge("file1.cpp", "file3.asm", "file4.obj");
	d.printDirectory();
	d.printClusters("file4.obj");
}

