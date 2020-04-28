        ;
        ; Cette ROM definit les commandes:
        ; |PACKSCR,adr,lg Qui compacte l'écran entier à l'adresse <adr>,
        ;          lg contient la longueur compactée
        ; |PACKWIN,xd,yd,xa,ya,adr,lg Qui compacte la fenêtre écran définie par
        ;          les bornes xd,yd xa,ya à l'adresse <adr>
        ;          lg contient la longueur compactée
        ; |DEPKSCR,adr Qui décompacte l'écran depuis l'adresse <adr>
        ; |DEPKWIN,adr Qui décompacte la fenêtre depuis l'adresse <adr>
        ; |PACKWINDC,adr,lg Qui compacte la fenêtre écran utilisée par
        ; l'affichage de la mémoire écran
        ; |PACKBUFF,AdrSrc,Longueur,AdrDest,lg
        ; |DEPKBUFF,AdrScr,AdrDest,lg
        ; |PACKWINDIFF,AdrDest,lg Qui compacte la fenêtre écran utilisée
        ; par la différence entre le buffer interne et la mémoire écran, et
        ; ensuite recopie la mémoire écrand dans le buffer interne.
        ; |INITBUFFDIFF,AdrDest Qui initialise le buffer interne avec le
        ; contenu de la mémoire à l'adresse AdrDest
        ; |CONVERTBITMAP,NomFic Qui lit un fichier BMP directement depuis
        ; l'emplacement précisé dans le nom (disque dur du PC), le convertit
        ; en image CPC et l'affiche à l'écran (#C000)
        ; |CONVLIGNE,Etat,ModeAdresse Qui change l'état de conversion
        ; des donnéées mémoire écran vers les fenêtres pour compactage :
        ; si Etat=0, la conversion sera effectuée par colonnes (mode par défaut)
        ; sinon, la conversion sera effectuée par lignes (plus rapide, mais
        ; moins efficace à la compression.). Si ModeAdresse=0, les coordonnées
        ; XD,YD,XA,YA seront stockées au début des données de la fenêtre (mode
        ; par défaut), sinon, l'adresse écran (2 octets), le nombre de lignes
        ; et le nombre de colonnes seront stockées.
        ; |SAUVEBITMAP,NomFic qui sauvegarde l'image courante (à l'écran de
        ; l'émulateur) dans un fichier .BMP
        ; |TURBOON qui active le mode turbo
        ; |TURBOOFF qui désactive le mode turbo
        ; |READSNAP,NomFicSna qui permet de lire un fichier snapshot
        ; |SAVESNAP,NomFicSna qui permet de sauvegarder un fichier snapshot
        ; |DEBUGON qui active le mode debug
        ; |SETDSKFILE,NomFicDsk qui permet de choisir un fichier .DSK à utiliser
        ; |SAVEAVI,NomFicAvi qui démarre l'enregistrement de l'écran 
        ; d'émulation dans un fichier avi
        ; |ENDAVI qui arrête l'enregistrement du fichier avi
        ; |SAVEYM,NomFicYM qui démarre l'enregistrement de la sortie sonore dans
        ; un fichier au format YM5
        ; |ENDYM qui arrête l'enregistrement du fichier YM
        ; |SETDIRECTDIR,Directory qui permet de choisir le répertoire vers lequel
        ; seront dirigés les accès disques (accès direct à un répertoire)
        ; |SETNEWDISC,Etat qui permet de modifier l'etat des acces disques
        ; |SYMBVIDOPEN,NomFic qui ouvre un fichier au format VID (format vidéo
        ; de SymbOs)
        ; |SYMBVIDREAD,adr qui lit à l'adresse adr la trame en cours du fichier
        ; VID ouvert, et passe le pointeur de fichier à la trame suivante
        ; |SYMBVIDPLAY équivalent à |SYMBVIDREAD, mais affiche directement la
        ; trame à partir de l'adresse #C000
        ; passe le pointeur de fichier à la trame suivante (comme |SYMBVIDREAD)
        ; |SYMBVIDCLOSE qui ferme le fichier VID ouvert.
        ; |PROFILESTART qui démarre le profiling
        ; |PROFILESTOP qui stoppe le profiling
        ; |DRAWCADREWIN,xd,yd,xa,ya qui trace un cadre aux coordonnées définies
        ; |ASSEMBLE,NomFic qui assemble le fichier NomFic
        ; |SETCOORDDIFF,xd,yd,xa,ya qui définit la zone de différences inter-
        ; images
        ; |FILLTRIANGLE,x1,y1,x2,y2,x3,y3,color qui trace un triangle rempli
        ; en MODE 1 aux coordonnées définies
        ;
        ORG     #C000
        ;BANK    #C0
        DB      #01                     ; bg ROM
Version:
        DB      #01,#01,#08             ; Version 1.18
        DW      RSX_TABLE               ; Adresses des noms des Commandes externes

        JP      INIT                    ; Adresse d'execution lors du demarrage
        JP      HELP
        JP      PACK_SCR
        JP      PACK_WIN
        JP      DEPACK_SCR
        JP      DEPACK_WIN
        JP      PACK_WIN_DC
        JP      PACK_BUFF
        JP      DEPACK_BUFF
        JP      PACK_WIN_DIFF
        JP      INIT_BUFF_DIFF
        JP      CONVERT_BITMAP
        JP      CONV_LIGNE
        JP      SAUVE_BITMAP
        JP      TURBO_ON
        JP      TURBO_OFF
        JP      READ_SNAP
        JP      SAVE_SNAP
        JP      DEBUG_ON
        JP      SETDSKFILE
        JP      SAVEAVI
        JP      ENDAVI
        JP      SAVEYM
        JP      ENDYM
        JP      SETDIRECTDIR
        JP      SYMBVIDOPEN
        JP      SYMBVIDREAD
        JP      SYMBVIDPLAY
        JP      SYMBVIDCLOSE
        JP      PROFILESTART
        JP      PROFILESTOP
        JP      SETNEWDISC
        JP      DRAW_CADRE_WIN
        JP      ASSEMBLE
        JP      SET_COORD_DIFF
        JP      FILLTRIANGLE

RSX_TABLE
        DB      "ROM PAC","K"+#80
        DB      "ROMPACKHEL","P"+#80
        DB      "PACKSC","R"+#80
        DB      "PACKWI","N"+#80
        DB      "DEPKSC","R"+#80
        DB      "DEPKWI","N"+#80
        DB      "PACKWIND","C"+#80
        DB      "PACKBUF","F"+#80
        DB      "DEPKBUF","F"+#80
        DB      "PACKWINDIF","F"+#80
        DB      "INITBUFFDIF","F"+#80
        DB      "CONVERTBITMA","P"+#80
        DB      "CONVLIGN","E"+#80
        DB      "SAUVEBITMA","P"+#80
        DB      "TURBOO","N"+#80
        DB      "TURBOOF","F"+#80
        DB      "READSNA","P"+#80
        DB      "SAVESNA","P"+#80
        DB      "DEBUGO","N"+#80
        DB      "SETDSKFIL","E"+#80
        DB      "SAVEAV","I"+#80
        DB      "ENDAV","I"+#80
        DB      "SAVEY","M"+#80
        DB      "ENDY","M"+#80
        DB      "SETDIRECTDI","R"+#80
        DB      "SYMBVIDOPE","N"+#80
        DB      "SYMBVIDREA","D"+#80
        DB      "SYMBVIDPLA","Y"+#80
        DB      "SYMBVIDCLOS","E"+#80
        DB      "PROFILESTAR","T"+#80
        DB      "PROFILESTO","P"+#80
        DB      "SETNEWDIS","C"+#80
        DB      "DRAWCADREWI","N"+#80
        DB      "ASSEMBL","E"+#80
        DB      "SETCOORDDIF", "F"+#80
        DB      "FILLTRIANGL","E"+#80
        DB      0                       ; FIN

INIT:
        PUSH    BC
        PUSH    DE
        PUSH    HL
        LD      BC,#FEFE
        IN      A,(C)
        CP      #AA
        JR      NZ,BadInit
        LD      HL,MessInitRom1
        CALL    PrintMess
        LD      HL,Version
        LD      BC,#2E30
        LD      A,(HL)
        INC     HL
        OR      C
        CALL    #BB5A
        LD      A,B
        CALL    #BB5A
        LD      A,(HL)
        INC     HL
        OR      C
        CALL    #BB5A
        LD      A,(HL)
        OR      C
        CALL    #BB5A
        LD      HL,MessInitRom2
        CALL    PrintMess
        SCF
        JR      FinInit
BadInit:
        LD      HL,MesBadInit
        CALL    PrintMess
        XOR     A
FinInit:
        POP     HL
        POP     DE
        POP     BC
        RET

GetChaineArg:
        LD      H,(IX+1)
        LD      L,(IX+0)
        LD      B,(HL)                  ; Longueur chaine
        INC     HL
        LD      A,(HL)
        INC     HL
        LD      H,(HL)                  ; Pointeur début chaine
        LD      L,A
        RET

PACK_SCR
        LD      HL,MessBadArgPackScr
        CP      2
        JP      NZ,PrintMess
        LD      H,(IX+1)
        LD      L,(IX+0)
        PUSH    HL
        LD      H,(IX+3)
        LD      L,(IX+2)
        POP     IX
        DB      #ED,#01                 ; Code spécial pour émulateur
        RET

PACK_WIN
        LD      HL,MessBadArgPackWin
        CP      6
        JP      NZ,PrintMess
        LD      E,(IX+10)
        LD      D,(IX+8)
        LD      C,(IX+6)
        LD      B,(IX+4)
        LD      H,(IX+1)
        LD      L,(IX+0)
        PUSH    HL
        LD      H,(IX+3)
        LD      L,(IX+2)
        POP     IX
        DB      #ED,#02                 ; Code spécial pour émulateur
        RET

DEPACK_SCR
        LD      HL,MessBadArgDepackScr
        DEC     A
        JP      NZ,PrintMess
        LD      H,(IX+1)
        LD      L,(IX+0)
        DB      #ED,#03                 ; Code spécial pour émulateur
        RET

DEPACK_WIN
        LD      HL,MessBadArgDepackWin
        DEC     A
        JP      NZ,PrintMess
        LD      H,(IX+1)
        LD      L,(IX+0)
        DB      #ED,#04                 ; Code spécial pour émulateur
        RET

PACK_WIN_DC
        LD      HL,MessBadArgPackWinDC
        CP      2
        JP      NZ,PrintMess
        LD      H,(IX+1)
        LD      L,(IX+0)
        PUSH    HL
        LD      H,(IX+3)
        LD      L,(IX+2)
        POP     IX
        DB      #ED,#05                 ; Code spécial pour émulateur
        RET

PACK_BUFF
        LD      HL,MessBadArgPackBuff
        CP      4
        JP      NZ,PrintMess
        LD      H,(IX+1)
        LD      L,(IX+0)
        PUSH    HL
        LD      H,(IX+7)
        LD      L,(IX+6)
        LD      D,(IX+5)
        LD      E,(IX+4)
        LD      B,(IX+3)
        LD      C,(IX+2)
        POP     IX
        DB      #ED,#06                 ; Code spécial pour émulateur
        RET

DEPACK_BUFF
        LD      HL,MessBadArgDepackBuff
        CP      3
        JP      NZ,PrintMess
        LD      H,(IX+1)
        LD      L,(IX+0)
        PUSH    HL
        LD      H,(IX+5)
        LD      L,(IX+4)
        LD      D,(IX+3)
        LD      E,(IX+2)
        POP     IX
        DB      #ED,#07                 ; Code spécial pour émulateur
        RET

PACK_WIN_DIFF
        LD      HL,MessBadArgPackWinDiff
        CP      2
        JP      NZ,PrintMess
        LD      H,(IX+1)
        LD      L,(IX+0)
        PUSH    HL
        LD      H,(IX+3)
        LD      L,(IX+2)
        POP     IX
        DB      #ED,#08                 ; Code spécial pour émulateur
        RET

INIT_BUFF_DIFF:
        LD      HL,MessBadArgInitBuffDiff
        DEC     A
        JP      NZ,PrintMess
        LD      H,(IX+1)
        LD      L,(IX+0)
        DB      #ED,#09                 ; Code spécial pour émulateur
        RET

CONVERT_BITMAP:
        LD      HL,MessBadArgConvertBitmap
        DEC		A
        JP      C,PrintMess				; Au moins un paramètre est demandé
        LD      H,(IX+1)
        LD      L,(IX+0)
        LD      B,(HL)                  ; Longueur chaine
        INC     HL
        LD      C,(HL)
        INC     HL
        LD      H,(HL)                  ; Pointeur début chaine
        LD      L,C
        LD      C,A
        JR      Z,ConvertBitmap2		; Si un seul paramètre
        PUSH    HL
        LD      H,(IX+3)
        LD      L,(IX+2)
        LD      C,(HL)                  ; Longueur chaine
        INC     HL
        LD      E,(HL)
        INC     HL
        LD      D,(HL)                  ; Pointeur début chaine
        POP     HL
        EX      DE,HL
        LD      A,B
        LD      B,C
        LD      C,A
ConvertBitmap2:
        DB      #ED,#0A                 ; Code spécial pour émulateur
        RET

CONV_LIGNE:
        LD      HL,MessBadArgConvLigne
        CP      2
        JP      NZ,PrintMess
        LD      A,(IX+2)                ; Etat ConvLigne
        LD      B,(IX+0)                ; Etat ModeAdresse
        DB      #ED,#0B                 ; Code spécial pour émulateur
        RET

SAUVE_BITMAP:
        DB      #ED,#0C                 ; Code spécial pour émulateur
        RET

TURBO_ON:
        LD      A,1
        DB      #ED,#0D                 ; Code spécial pour émulateur
        RET

TURBO_OFF:
        XOR     A
        DB      #ED,#0D                 ; Code spécial pour émulateur
        RET

READ_SNAP:
        LD      HL,MessBadArgReadSnap
        DEC     A
        JP      NZ,PrintMess
        CALL    GetChaineArg
        DB      #ED,#0E                 ; Code spécial pour émulateur
        RET

SAVE_SNAP:
        LD      HL,MessBadArgSaveSnap
        DEC     A
        JR      NZ,PrintMess
        CALL    GetChaineArg
        DB      #ED,#0F                 ; Code spécial pour émulateur
        RET

DEBUG_ON:
        DB      #ED,#10                 ; Code spécial pour émulateur
        RET

SETDSKFILE:
        LD      HL,MessBadArgSetDskFile
        DEC     A
        JR      NZ,PrintMess
        CALL    GetChaineArg
        DB      #ED,#11                 ; Code spécial pour émulateur
        RET

SAVEAVI:
        LD      HL,MessBadArgSaveAvi
        DEC     A
        JR      NZ,PrintMess
        CALL    GetChaineArg
        DB      #ED,#12                 ; Code spécial pour émulateur
        RET

ENDAVI:
        DB      #ED,#13                 ; Code spécial pour émulateur
        RET

SAVEYM:
        LD      HL,MessBadArgSaveYM
        LD      DE,0
        DEC     A
        JR      Z,SaveYmSuite
        DEC     A
        JR      NZ,PrintMess
        LD      D,(IX+1)
        LD      E,(IX+0)
        INC     IX
        INC     IX
SaveYmSuite
        CALL    GetChaineArg
        DB      #ED,#14                 ; Code spécial pour émulateur
        RET

ENDYM:
        DB      #ED,#15                 ; Code spécial pour émulateur
        RET

SETDIRECTDIR:
        LD      HL,MessBadArgSetDirectDir
        DEC     A
        JR      NZ,PrintMess
        CALL    GetChaineArg
        DB      #ED,#16                 ; Code spécial pour émulateur
        RET

SYMBVIDOPEN:
        LD      HL,MessBadArgSymbVidOpen
        DEC     A
        JR      NZ,PrintMess
        CALL    GetChaineArg
        DB      #ED,#17                 ; Code spécial pour émulateur
        LD      HL,MessBadSymbVidOpen
        JR      Z,PrintMess
        RET

SYMBVIDREAD:
        LD      HL,MessBadArgSymbVidRead
        DEC     A
        JR      NZ,PrintMess
        LD      H,(IX+1)
        LD      L,(IX+0)
        DB      #ED,#18                 ; Code spécial pour émulateur
        LD      HL,MessBadSymbVidPlay
        JR      Z,PrintMess
        RET

PrintMess:
        LD      A,(HL)
        AND     A
        RET     Z
        INC     HL
        CP      32
        JR      C,PrintMessCtrl
        PUSH    HL
        CALL    #BB5D
        POP     HL
        JR      PrintMess
PrintMessCtrl:
        CALL    #BB5A
        JR      PrintMess

SYMBVIDPLAY:
        DB      #ED,#19                 ; Code spécial pour émulateur
        LD      HL,MessBadSymbVidPlay
        JR      Z,PrintMess
        RET

SYMBVIDCLOSE:
        DB      #ED,#1A                 ; Code spécial pour émulateur
        LD      HL,MessBadSymbVidClose
        JR      Z,PrintMess
        RET

PROFILESTART:
        DB      #ED,#1B                 ; Code spécial pour émulateur
        RET

PROFILESTOP:
        DB      #ED,#1C                 ; Code spécial pour émulateur
        RET

SETNEWDISC:
        LD      HL,MessBadArgSetNewDisc
        DEC     A
        JR      NZ,PrintMess
        LD      A,(IX+0)
        DB      #ED,#1D                 ; Code spécial pour émulateur
        RET

DRAW_CADRE_WIN:
        LD      HL,MessBadArgDrawCadreWin
        CP      4
        JR      NZ,PrintMess
        LD      B,(IX+6)
        LD      C,(IX+4)
        LD      D,(IX+2)
        LD      E,(IX+0)
        DB      #ED,#1E                 ; Code spécial pour émulateur
        RET

ASSEMBLE:
        LD      HL,MessBadArgAsm
        DEC     A
        JR      NZ,PrintMess
        CALL    GetChaineArg
        DB      #ED,#1F                 ; Code spécial pour émulateur
        RET

SET_COORD_DIFF:
        LD      HL,MessBadArgSetCoordDiff
        CP      4
        JR      NZ,PrintMess
        LD      B,(IX+6)
        LD      C,(IX+4)
        LD      D,(IX+2)
        LD      E,(IX+0)
        DB      #ED,#20                 ; Code spécial pour émulateur
        RET

FILLTRIANGLE:
        LD      HL,MessBadArgFillTriangle
        CP      7
        JR      NZ,PrintMess
        LD      B,(IX+12)
        LD      C,(IX+10)
        LD      D,(IX+8)
        LD      E,(IX+6)
        LD      H,(IX+4)
        LD      L,(IX+2)
        LD      A,(IX+0)
        DB      #ED,#21                 ; Code spécial pour émulateur
        RET
        
HELP:
        LD      HL,MessHelp
BclHelp:
        LD      A,(HL)
        CP      #FF
        RET     Z
        CP      #FE
        JR      Z,BclHelp2
        CALL    PrintMess
        INC     HL
        JR      BclHelp
BclHelp2:
        INC     HL
        CALL    #BB18
        CP      252
        JR      NZ,BclHelp
        RET

MessInitRom1:
        DB      15,2," Rom Pack version ",0
MessInitRom2:
        DB      " install{e.",10,13," (|ROMPACKHELP pour info.)",10,13,15,1,10,13,0

MesBadInit:
        DB      15,3," Erreur initialisation Rom Pack", 10,13,15,1,10,13,0

MessHelp:
        DB      15,3,"*** RSX Disponnibles ROM-PACK ***", 15,1,13,10

MessBadArgPackScr:
        DB      "|PACKSCR,adr,lg",10,13,0

MessBadArgPackWin:
        DB      "|PACKWIN,xd,yd,xa,ya,adr,lg",10,13,0

MessBadArgDepackScr:
        DB      "|DEPKSCR,adr",10,13,0

MessBadArgDepackWin:
        DB      "|DEPKWIN,adr",10,13,0

MessBadArgPackWinDC:
        DB      "|PACKWINDC,adr,lg",10,13,0

MessBadArgPackBuff:
        DB      "|PACKBUFF,AdrSrc,Longueur,AdrDest,lg",10,13,0

MessBadArgDepackBuff:
        DB      "|DEPKBUFF,AdrScr,AdrDest,lg",10,13,0

MessBadArgPackWinDiff:
        DB      "|PACKWINDIFF,AdrSrc,AdrDest,lg",10,13,0

MessBadArgInitBuffDiff:
        DB      "|INITBUFFDIFF,AdrScr",10,13,0

MessBadArgConvertBitmap:
        DB      "|CONVERTBITMAP,",34,"Nom_fichier",34,"[,",34,"Param}tres"
        DB      34,"]",10,13,0

MessBadArgConvLigne:
        DB      "|CONVLIGNE,Etat,TypeAdr",10,13,0

        DB      "|SAUVEBITMAP",10,13,0

        DB      "|TURBOON",10,13,0

        DB      "|TURBOOFF",10,13,0

MessBadArgReadSnap:
        DB      "|READSNAP,",34,"Nom_fichier.SNA",34,10,13,0

MessBadArgSaveSnap:
        DB      "|SAVESNAP,",34,"Nom_fichier.SNA",34,10,13,0

        DB      "|DEBUGON",10,13,0

MessBadArgSetDskFile:
        DB      "|SETDSKFILE,",34,"Nom_fichier.DSK",34,10,13,0

MessBadArgSaveAvi:
        DB      "|SAVEAVI,",34,"Nom_fichier.AVI",34,10,13,0

        DB      "|ENDAVI",10,13,0

MessBadArgSaveYM:
        DB      "|SAVEYM,",34,"Nom_fichier.YM",34,10,13,0

        DB      "|ENDYM",10,13,0

        DB      15,3,"Appuyez sur une touche...",15,1,13,0,#FE

MessBadArgSetDirectDir:
        DB      "|SETDIRECTDIR,",34,"Chemin_Complet",34,10,13,0

MessBadArgSetNewDisc:
        DB      "|SETNEWDISC,",34,"Etat",34,10,13,0

MessBadArgSymbVidOpen:
        DB      "|SYMBVIDOPEN,",34,"Chemin_Complet.VID",34,10,13,0

MessBadArgSymbVidRead:
        DB      "|SYMBVIDREAD,Adresse",10,13,0

        DB      "|SYMBVIDPLAY",10,13,0

        DB      "|SYMBVIDCLOSE",10,13,0

        DB      "|PROFILESTART",10,13,0

        DB      "|PROFILESTOP",10,13,0

MessBadArgDrawCadreWin:
        DB      "|DRAWCADREWIN,xd,yd,xa,ya",10,13,0

MessBadArgAsm:
        DB      "|ASSEMBLE,",34,"Chemin_Complet.ASM",34,10,13,0

MessBadArgSetCoordDiff:
        DB      "|SETCOORDDIFF,xd,yd,xa,ya",10,13,0

MessBadArgFillTriangle:
        DB      "|FILLTRIANGLE,x1,y1,x2,y2,x3,y3,color",10,13,0

        DB      #FF

MessBadSymbVidOpen:
        DB      "Impossible d'ouvrir le flux vid{o.",10,13,0

MessBadSymbVidPlay:
        DB      "Flux vid{o non ouvert.",10,13,0

MessBadSymbVidClose:
        DB      "Flux vid{o d{j@ ferm{.",10,13,0

_End:
