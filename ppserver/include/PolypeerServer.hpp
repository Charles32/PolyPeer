#ifndef POLYPEERSERVER_H
#define POLYPEERSERVER_H

// header
#include <BaseServer.hpp>

// forward declaration
class ServerData;
class WebServer;

/**
 * Serveur principale de Polypeer.
 * gère le lancement de tous les threads de traitement
 */
class PolypeerServer : public BaseServer
{
public:
	/**
	 * Lancement du seveur
	 */
	void start();
	
	/**
	 * Arret du serveur
	 */
	void stop();
	
	/**
	 * Redémarrage du serveur
	 */
	void restart();
	
	/**
	 * Récupérer la structure principale
	 */
	inline ServerData& getServerData() { return *(this->sData); }
	
	/**
	 * Destructeur de PolypeerServer
	 */
	~PolypeerServer();
	
	/**
	 * Accès au singleton du serveur
	 */
	static PolypeerServer* getInstance();
	
	
private:
	/**
	 * Respet du patern singleton
	 */
	PolypeerServer();
	PolypeerServer(const PolypeerServer&);
	PolypeerServer& operator=(const PolypeerServer&);
	
	/**
	 * Instance du server
	 */
	static PolypeerServer* instance;
	
	/**
	 * Méthode de run
	 */
	static void run();
	
	/**
	 * Structure principale du programe
	 */
	ServerData* sData;
	
	/**
	 * Instance du webserver
	 */
	WebServer* webserver;
	
	/**
	 * fonctions pour l'algo de distribution
	 */
};


#endif //POLYPEERSERVER_H
