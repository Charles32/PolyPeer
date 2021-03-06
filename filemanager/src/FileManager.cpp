#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include <sys/stat.h>
#include <sys/types.h>

#include <FileManager.hpp>
#include <DiskFullException.hpp>
#include <OpenFileException.hpp>

using namespace std;


FileManager::FileManager(const char* path, int idFile, long sizeChunk)
{
	isComplete = true;
	init(path,0,sizeChunk,idFile);
}

FileManager::FileManager(const char* path, long size, long sizeChunk, int idFile)
{
	isComplete = false;
	init(path,size,sizeChunk,idFile);
}

void FileManager::init(const char* path, long size, long sizeChunk, int idFile)
{
	this->idFile = idFile;
	this->sizeChunk = sizeChunk;
	this->sizeFile = size;

	string pathStrTmp(path);
	pathFile.assign(pathStrTmp);


	checkDirectory(pathStrTmp);


	if(!isComplete)
	{
		string pathStr(path);
		pathStr += ".STATE";

		pathFileState.assign(pathStr);

		//on vérifi si le fichier d'enregistrement de l'état existe

		if (!existFile(pathFileState.c_str()))
		{
			//si non, on le crée et on l'initialise à 0
			ofstream tmp(pathStr.c_str());
			tmp << 0;
			tmp.close();

		}
		else
		{
			if(getState()==getNumberChunk())
			{
				isComplete = true;
			}


		}
	}


	if(!isComplete)
	{
		pathStrTmp+=".TMP";
		//on réserve l'emplacement du fichier sur le disque dur si il n'existe pas encore
		if (!existFile(pathStrTmp.c_str()))
		{
			if(getFreeDiskSpace() >= (unsigned)size)
			{
				reserveFile(pathStrTmp.c_str(), size);
			}else
			{
				//si il n'y a pas assez d'espace libre sur le dd on lance un exception
				throw DiskFullException();
			}

		}

		//on ouvre le fichier en mode binaire
		file.open(pathStrTmp.c_str(),ios::binary|ios::in|ios::out|ios::ate);
		if (!file)
		{
			throw OpenFileException();
		}
	}
	else
	{

		file.open(pathFile.c_str(),ios::binary|ios::in|ios::ate);
	}



	//le curseur est à la fin donc la position du curseur donne la taille du fichier
	sizeFile = file.tellp();




	currentData = new char[sizeChunk];
}

bool FileManager::existFile(const char* path)
{
	ifstream fichier(path);
	return !fichier.fail();
}

void FileManager::reserveFile(const char* path, long size)
{
	ofstream create(path, ios::out|ios::app);

	//on remplie le fichier avec des '6' => un caractère = 1 octet
	for (int i = 0; i < size; i++)
	{
		create<<'6';
	}

	create.close();
}

FileManager::~FileManager()
{
	file.close();
	delete [] currentData;
}

Chunk FileManager::getChunk(long number)
{
	file.seekp(number*sizeChunk, ios::beg);
	file.read(currentData, sizeChunk);

	if (getNumberChunk()-1==number)
	{
		return Chunk(number, sizeFile-number*sizeChunk, currentData, idFile);
	}
	else
	{
		return Chunk(number, sizeChunk, currentData, idFile);
	}
}
bool FileManager::saveChunk(Chunk &chunk)
{
	//on ne peut modifier le fichier que sil le fichier n'est pas fini
	bool toReturn = false;
	if(!isComplete)
	{
		currentChunk = getState();

		if (currentChunk == chunk.getNumber())
		{
			file.seekp(currentChunk*sizeChunk, ios::beg);
			file.write(chunk.getData(), chunk.getSize());

			currentChunk++;
			saveState();


			if(currentChunk == getNumberChunk())
			{
				setCompleted();
			}


			toReturn = true;
		}
		else
		{
			toReturn = false;
		}
	}
	return toReturn;
}

long FileManager::getNumberChunk()
{
	//formule de la division arrondie à l'entier supérieur
	return (sizeFile+sizeChunk-1)/sizeChunk;
}

long FileManager::getState()
{
	ifstream tmp(pathFileState.c_str());
	int i;
	tmp >> i;
	tmp.close();

	return i;
}
void FileManager::saveState()
{
    ofstream tmp(pathFileState.c_str(), ios::trunc);
    tmp << currentChunk;
    tmp.close();

}

long FileManager::getFileSize()
{
    return sizeFile;
}


int FileManager::getIdFile()
{
	return idFile;
}

std::string FileManager::getFileName()
{
	int last = pathFileState.find_last_of("/");

	if(last > 0)
	{
		//le 6 correspond au .STATE
		return pathFileState.substr( last,pathFileState.size() - last - 6);
	}

	return pathFileState.substr( 0,pathFileState.size() - 6);

}

long FileManager::getChunkSize()
{
	return sizeChunk;
}

long FileManager::getCurrentNumberChunk()
{
	return currentChunk;
}

int64_t FileManager::getFreeDiskSpace()
{
	 int64_t available;

int ind = pathFile.find_last_of("/\\");
        string tmp;

        if(ind>0)
        {
            tmp = pathFile.substr(0,ind);
        }
        else
        {
            tmp = "./";
        }
    #ifdef WIN32

		GetDiskFreeSpaceEx(tmp.c_str(),(PULARGE_INTEGER)&available,NULL,NULL);
    #elif defined (linux)
        struct statfs sf;
        statfs(tmp.c_str(), &sf);
        //f_bavail contient le nombre de blocks disponibles, f_bsize contient la taille d'un block en octet
        available=sf.f_bavail*sf.f_bsize;
    #endif

	return available;
}

void FileManager::setCompleted()
{
	isComplete = true;
	string strTmp(pathFile);
	strTmp+=".TMP";
	file.close();
	rename(strTmp.c_str(),pathFile.c_str());
	file.open(pathFile.c_str(),ios::binary|ios::in);
}

void FileManager::checkDirectory(std::string pathDirectory)
{

    createDirectory( pathDirectory, "");

}

void FileManager::createDirectory(std::string pathDirectory, std::string currentPath)
{
    //tout les traitement sont fait même si les répertoires existent déjà

	//on découpe la chaîne par le séparateur / ou \ pour pouvoir créer dosier par dossier en caas d'imbrication
	int ind = pathDirectory.find_first_of("/\\");

	//si il y a un séparateur de dossier c'est qu'il faut en crée un
	if(ind>0)
	{
		string tmp = currentPath + pathDirectory.substr(0,ind);
    #ifdef WIN32
		CreateDirectory(tmp.c_str(), NULL);
    #elif defined(linux)
        mkdir(tmp.c_str(),655);
    #endif
		createDirectory(pathDirectory.substr(ind+1), tmp+"/");
	}


}



