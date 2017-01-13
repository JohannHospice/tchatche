#Manuel d'utilisation Tchache

groupe: CHASLE Louis & HOSPICE Johann & BETTINELLI Jérôme

compilation:
	- pour compiler le serveur 
		make server
	- pour compiler le client 
		make client
	- pour tout compiler
		make all

execution:
	- serveur:
		./server
	- client:
		./client

commande client:
	- message public
		> .*
	- message privé
		> private:<pseudo_destinataire> .*
	- liste utilisateurs connectés
		> list
	- déconnexion
		> byee
	- arrêt serveur
		> shut