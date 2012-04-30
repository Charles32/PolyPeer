#ifndef PACKETSENDOPERATION_H
#define PACKETSENDOPERATION_H


/*
 * Classe PacketSendOperation
 *
 * Hérite de la classe Packet
 * Gérer un type de packet spécifique
 *
 *
 *
 *
 * par Olivier
 */


// Entêtes
#include <iostream>
#include <Chunk.hpp>

#include <Packet.hpp>
#include <Data.hpp>

using namespace std;

class PacketSendOperation : public Packet
{
private:
	

public:
	/**
	* Création d'un PacketSendOperation pret à être envoyé
	* 
	*/
	PacketSendOperation(string target, Chunk& chunk);
	
	/**
	* Constructeur sur un Packet (une maniere de caster un Packet)
	*/
	PacketSendOperation(const Packet& p);
	
	/**
	* Destructeur
	*/
	virtual ~PacketSendOperation();
	
	
	string getTarget ();
	
	Chunk getChunk ();
	

private:
	PacketSendOperation() {}

};

#endif // PACKETSENDOPERATION_H