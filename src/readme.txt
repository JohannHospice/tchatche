#Manuel d'utilisation Tchache

groupe: CHASLE Louis & HOSPICE Johann & BETTINELLI J�r�me

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
	- message priv�
		> private:<pseudo_destinataire> .*
	- liste utilisateurs connect�s
		> list
	- d�connexion
		> byee
	- arr�t serveur
		> shut