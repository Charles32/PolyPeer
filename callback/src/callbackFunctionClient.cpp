// Class header
#include <callbackFunctionClient.hpp>

// STL
#include <iostream>

// Project header
#include <PolypeerClient.hpp>
#include <ClientData.hpp>
#include <Packet.hpp>
#include <includePacket.hpp>


using namespace std;

int callbackNewFile(Packet& p)
{
	PacketNewFile pp (p);

	cout << "callbackNewFile" << endl;


	Packet pReturn;

	// récupérer singleton serveur
	ClientData* cd = ClientData::getInstance();

	// -> un filemanager existe deja pour le fichier ? info ifFile et fileName
	FileManager* fm = cd->getFileManager(pp.getIdFile());
	if (fm == NULL)
	{
		// création du fileManager
		try
		{
			fm = cd->addFile(pp.getIdFile(), pp.getFileName(), pp.getFileSize(), pp.getChunkSize());
		} catch (DiskFullException)
		{
			// Création du paquet d'erreur
			pReturn = PacketDiskFull (pp.getIdFile());
			return 0;
		}
	}

	// -> récuppérer le chunk courant
	pReturn = PacketReady (pp.getIdFile(), fm->getCurrentNumberChunk());

	// -> créer le nouveau paquet PacketReady
	cd->getConnectionManager()->sendTo(cd->getAddressServ(), pReturn);

	return 1;
}

int callbackSendOperation(Packet& p)
{
	PacketSendOperation pp (p);

	cout << "callbackSendOperation" << endl;

	// récupérer singleton serveur
	ClientData* cd = ClientData::getInstance();

	// vérifier que l'on possède bien le chunk en question
	// --
	Chunk c = pp.getChunk();
	// envoie à la cible
	cd->getConnectionManager()->sendTo(pp.getTarget(), PacketSendChunk(c));

	// réponse au serveur du travail effectué
	cd->getConnectionManager()->sendTo(cd->getAddressServ(), PacketSendOperationFinished());

	return 1;
}

int callbackSendChunk(Packet& p)
{
	PacketSendChunk pp (p);

	cout << "callbackSendChunk" << endl;

	// récupérer singleton serveur
	ClientData* cd = ClientData::getInstance();

	Chunk tmp(pp.getChunk());

	if(tmp.isIntegrate())
    {
    	FileManager* fm = cd->getFileManager(tmp.getIdFile());
        fm->saveChunk(tmp);

        cd->getConnectionManager()->sendTo(cd->getAddressServ(), PacketChunkReceived(tmp.getIdFile(), tmp.getNumber()));
    }
    else
    {
        // probleme de fichier
        cd->getConnectionManager()->sendTo(cd->getAddressServ(), PacketMd5Error(tmp.getIdFile(), tmp.getNumber()));
    }

	return 1;
}







