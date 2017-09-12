# Tchache

Système de chat client/server

## Compilation:

pour compiler le serveur:
```bash
$ make server
```

pour compiler le client:
```bash
$ make client
```

pour tout compiler:
```bash
$ make all
```

## Execution:

Pour executer le serveur:
```bash
./server
```

Pour executer le client:
```bash
./client
```

## Utilisation/Commandes du client:

| Commande | Description |
| --- | --- |
| > .* | message public |
| > private:<pseudo_destinataire> .* | message privé |
| > list | liste utilisateurs connectés |
| > byee | déconnexion | 
| > shut | arrêt serveur | 
