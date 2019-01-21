# Projet de modélisation surfacique

Ce dossier contient les sources d'un projet de reconstruction de surface
à partir d'un nuage de point.

La méthode de reconstruction est décrite dans le papier que vous trouverez
sous doc/mpu_implicits.pdf

## Compilation et exécution
```console
me@machine:~$ cd base_projets/
me@machine:~$ mkdir build
me@machine:~$ cd build/
me@machine:~$ make
me@machine:~$ ./release/projet <size_maillage> <eps_0>
```

size_maillage : taille du maillage pour la reconstruction (ex. 20)
eps_0         : erreur d'approximation (ex. 0.1)         
