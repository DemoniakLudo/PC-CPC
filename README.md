# PC-CPC

Emulateur Amstrad CPC 6128, écrit 100% en C/C++ (Avec les classes MFC Visual studio)

Utilisation :
-------------
Lancer l'émulateur (Z80.EXE)
Pour pouvoir fonctionner, il a besoin :
- du fichier config.cpc
- les roms d'un cpc (dans le répertoire définit par le fichier config.cpc)
- éventuellement, un ou plusieurs fichiers images disquette (au format .DSK)

Touches de fonctions :
----------------------
F1 -> Mode turbo 'ON' : Ne synchronise plus l'affichage à 50Hz. Cette
    option peut être utile avec un PC assez lent.

F2 -> Mode turbo 'OFF' : Synchronise l'affichage à 50Hz, comme sur un vrai CPC.

F3 -> Bouton 'STOP' de la multiface II (attention, cette option n'est pas
    encore totalement fonctionnelle). 

F4 -> Lecture d'un snapshot. Un snapshot est un fichier contenant l'état
    de chaque composant du cpc ainsi qu'une copie de la mémoire à un instant
    déterminé, pour permettre par exemple une sauvegarde d'un jeu à un endroit
    précis.
    

F5 -> Sauvegarde d'un snapshot

F6 -> Appel du débuggeur Z80 intégré (en option...) Touches : Espace = pas à
      pas. Nouvel appui sur F6 : sortie du débuggeur

F7 -> Changement de disquette. Une disquette est émulée par un fichier image
    (fichier avec l'extension .DSK). Ce mode désactive le mode de lecture
    depuis le disque dur du PC.

F8 -> Reset du CPC. Similaire à un switch du bouton ON/OFF

F9 -> Lecture d'un fichier cassette. Permet d'indiquer un nom de fichier au
    travers duquel l'émulation de la lecture cassette sera réalisé. Le fichier
    doit être un fichier au format .WAV, digitalisé sur 8 bits en mono, avec
    une fréquence de 11, 22 ou 44 Khz.

F11 -> Ecriture d'un fichier cassette. Permet de créer un fichier dans lequel
    sera restranscrit toute écriture sur cassette. Le fichier créé est un
    fichier .WAV digitalisé sur 8 bits en mono à la fréquence de 11 Khz.

F12 -> Changement du mode d'émulation disquette. Par défaut, les disquettes
     sont émulées par un fichier .DSK. Un nouveau mode permet d'émuler
     directement les fichiers depuis une partition ou un lecteur de disque
     du PC. Ce mode désactive la lecture/écriture par fichiers .DSK

SHIFT+F2 -> Permet de sauvegarder l'image actuelle de l'émulateur dans un
    fichier BMP.

SHIFT+F5 -> Permet de sauvegarder les sons dans un fichier au format YM.
    Un nouvel appui sur las touches SHIFT+F5 permet d'arrêter la sauvegarde, et
    affiche le nombre de "trames" sauvegardées (une trame = 1/50e de secondes).

SHIFT+F9 -> Permet, lorsqu'un fichier cassette est ouvert en lecture (par F9),
    de choisir la position dans le fichier via un "compteur de secondes".
    Une fenêtre popup s'ouvre alors, indiquand la position actuelle du fichier.
    Il sufit d'éditer cette valeur puis d'appuyer sur le bouton "OK" pour la
    modifier. La fermeture de la fenêtre par la petite croix en haut à droite
    ne fait aucune modification sur la position du compteur.

SHIFT+F11 -> Permet de sauvegarder les images de l'émulateur dans un fichier AVI.
    Un nouvel appui sur les touches SHIFT+F11 permet d'arrêter l'encodage, et
    affiche le nombre d'images encodées. Pour le moment, la vitesse est fixée
    à 25 images par secondes (soit une image toutes les 2 vbl pour le CPC).



Arguments ligne de commande :
-----------------------------
-disk nom_fichier_avec_chemin_complet.dsk : permer de charger un fichier .dsk
    au démarrage

-snapshot nom_fichier_avec_chemin_complet.sna : permet de charger un snapshot
    au démarrage pour lancement automatique

-run fichier : permet de démarrer automatiquement le fichier dont le nom est
    "fichier".

Nouvelles commandes RSX disponnibles
------------------------------------
PC-CPC contient une nouvelle rom dévelopée spécialement qui contient plusieurs
nouvelles commandes RSX ajoutées au BASIC Amstrad :

|PACKSCR,adr_buffer,adr_longueur
Permet de compacter l'écran affiché sur le cpc (adresse #C000->#FFCF).
adr_buffer représente l'adresse où sera stockée l'image compactée,
adr_longueur représente l'adresse où sera stockée sur 16 bits
la longueur de l'image compactée. adr_longueur peut être l'adresse
d'une variable basic entière (ex : @longueur%)

|DEPKSCR,adr_buffer
Permet de décompacter l'écran stocké à l'adresse adr_buffer.

|PACKWIN,xd,yd,xa,ya,adr_buffer,adr_longueur
Permet de compacter la fenêtre affichée sur le cpc en choisissant
les coordonnées (xd,yd) comme point de départ et (xa,ya) comme
point d'arrivée de la fenêtre. xd varie de 0 à 79 quel que soit
le mode écran (xd représente la colonne de départ en octets),
xa varie de 0 à 79 et doit être supérieur ou égal à xd.
yd varie de 0 à 199 quel que soit le mode écran (yd représente
la ligne de départ en octets), ya varie de 0 à 199 et doit être
supérieur ou égal à yd.
adr_buffer représente l'adresse où sera stockée la fenêtre compactée,
adr_longueur représente l'adresse où sera stockée sur 16 bits
la longueur de l'image compactée. adr_longueur peut être l'adresse
d'une variable basic entière (ex : @longueur%)

|DEPKWIN,adr_buffer
Permet de décompacter la fenêtre stocké à l'adresse adr_buffer.

|PACKWINDC,adr_buffer,adr_longueur
Permet de compacter la fenêtre affichée sur le cpc en calculant
automatiquement les coordonnées de celle-ci en fonction des points
présents sur l'écran. Pour résumer, il suffit d'afficher la fenêtre
que l'on veut compacter sur un écran vierge, et ce RSX calculera
automatiquement les coordonnées. Le format de la fenêtre compactée
est le même qu'avec l'instruction |PACKWIN.
adr_buffer représente l'adresse où sera stockée l'image compactée,
adr_longueur représente l'adresse où sera stockée sur 16 bits
la longueur de l'image compactée. adr_longueur peut être l'adresse
d'une variable basic entière (ex : @longueur%)

|PACKWINDIFF,adr_buffer,adr_diff,adr_longueur
Permet de compacter la fenêtre affichée sur le cpc en calculant
automatiquement les coordonnées de celle-ci en fonction des différences
entre l'image affichée à l'écran et l'image située à l'adresse adr_diff.
Le format de la fenêtre compactée est le même qu'avec l'instruction
|PACKWIN. adr_buffer représente l'adresse où sera stockée l'image
compactée, adr_longueur représente l'adresse où sera stockée sur 16 bits
la longueur de l'image compactée. adr_longueur peut être l'adresse
d'une variable basic entière (ex : @longueur%)

|INITBUFFDIFF,adr_buffer
Permet d'initialiser le buffer des différences inter-images.
Ceci permet, lors du premier appel de |PACKWINDIFF, d'avoir un buffer
initialisé avec un écran par défaut. En général, on pourra faire :
MODE 1:|INITBUFFDIFF,&C000

|PACKBUFF,adr_source,longueur,adr_dest,adr_longueur
Permet de compacter une zone mémoire d'adresse et de longueur
déterminée vers une zone de destination.
adr_source représente l'adresse de la zone mémoire à compacter,
longueur représente la longueur de la zone mémoire à compacter,
adr_dest représente l'adresse de la zone de destination (qui
contiendra les données compactées)
adr_longueur représente l'adresse où sera stockée sur 16 bits
la longueur de l'image compactée. adr_longueur peut être l'adresse
d'une variable basic entière (ex : @longueur%)
Ces nouveaux RSX font appel à une fonction de compactage LZW, codée
en C, et donc performante et rapide. Il est également possible d'appeler
les fonctions de compactage-décompactage en assembleur via les
op-codes suivants :

|DEPKBUFF,adr_source,adr_dest,adr_longueur
Permet de décompacter la zone mémoire stocké à l'adresse adr_source vers
adr_dest.
adr_longueur représente l'adresse où sera stockée sur 16 bits
la longueur de la zone décompactée. adr_longueur peut être l'adresse
d'une variable basic entière (ex : @longueur%)

|CONVERTBITMAP,"nom_image"[,"paramètres"]
Permet de convertir une image bitmap (.BMP) depuis son emplacement sur
le disque dur du PC et de l'afficher à l'écran du cpc (dans la mémoire
vidéo) convertie avec le mode courant.
"nom_image" est une chaine de caractères ou une variable alphanumérique
contenant le nom et le chemin complet de l'image source (par exemple:
"C:\WINNT\WINNT256.BMP" (sur cpc azerty, le '\' est remplacé par 'ç')
Les paramètres doivent être mis dans une chaine de caractères, séparés
par des espaces, exemple :
"-T1 -X2"
Les paramètres sont :
-Bnnn   valeur de la composante bleue (en %)
-L      bloquer la palette (utiliser les valeurs courantes)
-Pnnn   pourcentage de tramage (en %)
-Rnnn   valeur de la composante rouge (en %)
-Tnnn   type de trame (de 0 à 3, 0=rien)
-Vnnn   valeur de la composante verte (en %)
-Xnnn   type de matrice de tramage (2 ou 3)
-A      mode "transparent" : les points en couleur 0 ne sont pas affichés
-DXnnn  Déplacement en X de l'image de nnn points
-DYnnn  Déplacement en Y de l'image de nnn points

|CONVLIGNE,Etat,ModeAdresse
Permet de charger l'état de conversion des donnéées mémoire écran vers les
fenêtres pour compactage : si Etat=0, la conversion sera effectuée par colonnes
(mode par défaut) sinon, la conversion sera effectuée par lignes (plus rapide,
mais moins efficace à la compression.).
Si ModeAdresse=0, les coordonnées XD,YD,XA,YA seront stockées au début des
données de la fenêtre (mode par défaut), sinon, l'adresse écran (2 octets),
le nombre de lignes et le nombre de colonnes seront stockées.

|SAUVEBITMAP,"NomFic"
Pemet de sauvegarder l'image courante (à l'écran de l'émulateur) dans un fichier
au format BMP.

|TURBOON
Permet d'activer le mode turbo. (Equivalent touche F1)

|TURBOOFF
Permet de désactiver le mode turbo. (Equivalent touche F2)

|READSNAP,"NomFicSna"
Permet de lire un fichier snapshot. (Equivalent touche F4)

|SAVESNAP,"NomFicSna"
Permet de sauvegarder un fichier snapshot. (Equivalent touche F5)

|DEBUGON
Permet d'activer le mode debug. (Equivalent touche F6)

|SETDSKFILE,"NomFicDsk"
Permet de choisir un fichier .DSK à utiliser. (Equivalent touche F7)

|SAVEAVI,"NomFicAvi"
Permet de démarrer l'enregistrement de l'écran d'émulation dans un fichier avi.
(Equivalent touches SHIFT+F11)

|ENDAVI
Permet d'arrêter l'enregistrement du fichier avi.
(Equivalent touches SHIFT+F11)

|SAVEYM,NomFicYM
Permet de démarrer l'enregistrement de la sortie sonore dans un fichier au
format YM5.
(Equivalent touches SHIFT+F5)

|ENDYM
Permet d'arrêter l'enregistrement du fichier YM.
(Equivalent touches SHIFT+F5)

|SETDIRECTDIR,"Directory"
Permet de choisir le répertoire vers lequel seront dirigés les accès disques.
(accès direct à un répertoire, équivalent touche F12)


Certains OP-CODE Z80 ont étés redéfinis pour pouvoir utiliser directement ces
nouvelles fonctionnalités en assembleur, sans la ROM-PACK :


ED 01 : équivalent de |PACKSCR, avec en entrée :
HL = adresse du buffer de compactage
IX = adresse contenant la longueur compactée
Aucun registre n'est modifié au retour.

ED 02 : équivalent de |PACKWIN, avec en entrée :
E  = xd
D  = yd
C  = xa
B  = ya
HL = adresse du buffer de compactage
IX = adresse contenant la longueur compactée
Aucun registre n'est modifié au retour.

ED 03 : équivalent de |DEPKSCR, avec en entrée:
HL = adresse du buffer des données compactées
Aucun registre n'est modifié au retour.

ED 04 : équivalent de |DEPKWIN, avec en entrée:
HL = adresse du buffer des données compactées
Aucun registre n'est modifié au retour.

ED 05 : équivalent de |PACKWINDC, avec en entrée:
HL = adresse du buffer de compactage
IX = adresse contenant la longueur compactée
Aucun registre n'est modifié au retour.

ED 06 : équivalent de |PACKBUFF, avec en entrée:
HL = adresse de la zone à compacter (source)
DE = adresse de la zone destination
BC = longueur de la zone à compacter.
IX = adresse contenant la longueur compactée
Aucun registre n'est modifié au retour.

ED 07 : équivalent de |DEPKBUFF, avec en entrée:
HL = adresse de la zone source (zone compactée)
DE = adresse de la zone destination
IX = adresse contenant la longueur décompactée
Aucun registre n'est modifié au retour.

ED 08 : équivalent de |PACKWINDIFF, avec en entrée:
HL = adresse du buffer de compactage
DE = adresse du buffer écran pour comparaison des différences
IX = adresse contenant la longueur compactée
Aucun registre n'est modifié au retour.

ED 09 : équivalent de |INITBUFFDIFF, avec en entrée:
HL = adresse du buffer d'initialisation
Aucun registre n'est modifié au retour.

ED 0A : équivalent de |CONVERTBITMAP, avec en entrée:
HL = Pointeur vers nom du fichier BMP
B = Longueur nom fichier BMP
DE = Pointeur vers chaine contenant les arguments
C = Longueur chaine contenant les arguments
Aucun registre n'est modifié au retour.

ED 0B : équivalent de |CONVLIGNE, avec en entrée:
A = Etat
B = ModeAdresse
Aucun registre n'est modifié au retour.

ED 0C : équivalent de |SAUVEBITMAP, avec en entrée:
HL = Pointeur vers nom du fichier BMP
B = Longueur nom fichier BMP
Aucun registre n'est modifié au retour.

ED 0D : équivalent de |TURBOON ou de |TURBOOFF, avec en entrée:
A = mode turbo (0 = OFF, sinon ON)
Aucun registre n'est modifié au retour.

ED 0E : équivalent de |READSNAP, avec en entrée:
HL = Pointeur vers nom du fichier SNA
B = Longueur nom fichier SNA
Aucun registre n'est modifié au retour.

ED 0F : équivalent de |SAVESNAP, avec en entrée:
HL = Pointeur vers nom du fichier SNA
B = Longueur nom fichier SNA
Aucun registre n'est modifié au retour.

ED 10 : équivalent de |DEBUGON
Aucun registre n'est modifié au retour.

ED 11 : équivalent de |SETDSKFILE, avec en entrée:
HL = Pointeur vers nom du fichier DSK
B = Longueur nom fichier DSK
Aucun registre n'est modifié au retour.

ED 12 : équivalent de |SAVEAVI, avec en entrée:
HL = Pointeur vers nom du fichier AVI
B = Longueur nom fichier AVI
Aucun registre n'est modifié au retour.

ED 13 : équivalent de |ENDAVI
Aucun registre n'est modifié au retour.

ED 14 : équivalent de |SAVEYM, avec en entrée:
HL = Pointeur vers nom du fichier YM
B = Longueur nom fichier YM
Aucun registre n'est modifié au retour.

ED 15 : équivalent de |ENDYM
Aucun registre n'est modifié au retour.

ED 16 : équivalent de |SETDIRECTDIR, avec en entrée:
HL = Pointeur vers chemin complet
B = Longueur chemin complet
Aucun registre n'est modifié au retour.


* Exemple : Compactage d'un écran :

BUFFER  EQU     #4000
        LD      HL,BUFFER
        LD      IX,TAILLE
        DB      #ED,#01
        RET
TAILLE  DW      0

* Exemple : Décompactage d'un écran

BUFFER  EQU     #4000
        LD      HL,BUFFER
        DB      #ED,#02
        RET

Infos techniques
-----------------
- L'émulateur utilise un fichier de configuration nomé "Config.cpc",
qui doit se trouver dans le même répertoire que l'exécutable.
Le détail du paramétrage se trouve écrit dans le fichier en lui-même.
- L'émulateur est écrit 100% en C, sous environement Windows 32 bits,
et de manière à être porté facilement sur d'autres plateformes (ex: Amiga).
Les fonctions émulés sont les suivantes :
- Processeur Z80 : 99% des instructions,
- AY3-8912 (processeur sonore) : 3 voies + bruits blancs + enveloppes,
- CRTC 6845 (controlleur écran ) : Les registres fondamentaux à 50% 
    (1, 2, 6, 7, 9, 12, 13),
- PPI 8255 (interface parallèle ): 90% (Seul le mode 0 du 8255 est émulé,
    car c'est le seul utilisable dans un CPC),
- UPD 765 (controlleur disque ) : 70% (Permet la lecture des fichiers
    images standard),
- VGA (Video gate array) : 90 % (Sélection encres, Mode écran, Roms)
- Timings Hsync et Vsync (synchro horizontale et verticale),
- Gestion disquettes par fichier image standard (.DSK)
- Gestion sauvegarde état en cours par fichier snapshot standard (.SNA)
- Gestion cassettes par fichier au format WAV
- Utilisation du disque de la machine hote pour lecture/écriture de fichiers
