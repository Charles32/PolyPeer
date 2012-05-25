#ifndef __H__FILE__
#define __H__FILE__

#include <set>
#include <iostream>

#include <Entity.hpp>
#include <FileManager.hpp>

class Entity;

using namespace std;

class File
{

public :
	File(int id, string path, int size, int chunkSize);
	~File();
	FileManager* getFileManager() { return fileM; }
	int getId() { return fileM->getIdFile(); }
	string getFilePath() { return fileM->getFileName(); }
	int getSize() { return fileM->getFileSize(); }
	int getChunkSize() { return fileM->getChunkSize(); };
	vector<Entity*>* getDeploysOn() { return &deploysOn; }
	vector<vector<Entity*>* >* getSortedHosts();
	void addEntity(Entity* entity);
	
protected :
	FileManager* fileM;
	vector<Entity*> deploysOn;
	
};

#endif

