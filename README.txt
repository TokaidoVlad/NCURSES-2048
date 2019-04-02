    ----- PROLOGUE -----

    RO:

    2048, jocul faimos care a schimbat lumea, a fost acum reinventat de un simplu
    student. Ati putea crede ca este o evolutie, dar nu, acum acesta este 
    mult inferior originalui. Cum? Arta schimbata: ASCII revolution.
    Dar cu toate partile negative vin si parti bune, precum numeroasele functii
    noi, precum schimbarea setului de culori predefinit si posibilitatea de a
    activa un bot care va efectua miscari automate, despre care ati putea crede
    ca sunt miscari optime, dar care defapt sunt doar miscarile care lasa cele
    mai multe spatii libere. Cu adevarat miraculos.
    Dar stati, asta nu e tot, acum jocul 2048 poate sa arate si data si timpul
    curent. Absolut revolutionar.

    EN:

    2048, the famous game that changed the world with it's appearance, now
    reinvented by a simple student. You might think this is a better version,
    but no, it is far inferior to the first onw. How exactly? Changed art style:
    ASCII revolution.
    But with all the bad parts come some good ones too, like the many features
    added to the original: changing the color scheme, the posibility of letting
    an AI make automatic moves, which you might think are optimal, but actually,
    they are nothing more than the moves which leave the most free spaces. Truly
    miraculous.
    But wait, that's not it, now the 2048 game can also show you what the time
    and date is. Absolutely revolutionary.

    ----- CHAPTER 1 - THE MAIN MENU -----

    RO:

    Meniul principal este unul minimalist, pozitionat in coltul de stanga sus
    al ecranului (coordonatele 0, 0), de dimensiune prestabilita 30 x 20
    (MENU_X x MENU_Y), si care prezinta optiunile din vectorul de stringuri
    "choices", mai precis: "New Game", "Resume", "Settings", "Exit".
    Pentru a selecta una dintre aceste optiuni, se vor folosi tastele
    ARROW_UP si ARROW_DOWN de pe tastatura, iar pentru a confirma selectia
    se va apasa tasta ENTER. Cele patru optiuni vor avea efecte separate,
    dintre care:

    New Game - deschide un joc nou cu dimensiunea prestabilita in setari;

    Resume - deschide jocul salvat in cazul in care acesta exista in fisierul
    cu numele dimensiunii de joc ****, fapt detaliat in capitolul urmator, insa
    in cazul in care nu exista, acest buton va fi dezactivat si nu va putea fi
    selectat, si va fi colorat cu gri, cu perechea UNAVAILABLE_COLOR;

    Settings - deschide fereastre de setari, explicata in CHAPTER 2;

    Exit - inchide aplicatia.
    
    Pentru a ne da seama de optiunea selectata, aceasta va fi colorata cu
    perechea de culori HIGHLIGHT_COLOR, definita in functie de schema de
    culoare selectata.

    EN:
    
    The main menu is a very simple one, positioned on the up left corner of the
    screen (coordinates 0, 0), with a set dimension of 30 x 20 (MENU_X x MENU_Y),
    which shows the option located in the string vector "choices". These are:
    "New Game", "Resume", "Settings", "Exit". For choosing one of these options,
    keys ARROW_UP and ARROW_DOWN on the keyboard will be used, while the RETURN
    key will confirm the selection. The four options will have separate effects,
    namely:

    New Game - opens a new game with the dimension set in the settings menu;

    Resume - opens the game you left at, if a save file with the specified
    table dimension exists ****, fact that will be detailed in the next chapter.
    If a savefile doesn't exist, this button will be deactivated and you will not
    be able to select it, and it will be colored gray, the color pair
    UNAVAILABLE_COLOR;

    Settings - opens the settings window, explained in the next chapter.

    Exit - shuts down the application.

    To be able to know what the selected option is, it will be colored with the
    color pair HIGHLIGHT_COLOR, defined based on the selected color scheme.

    ----- CHAPTER 2 - THE SETTINGS MENU -----

    RO:

    Meniul de setari functioneaza asemanator cu cel de main menu, avand
    aceeaasi dimensiune si acelasi numar de optiuni, diferenta principala fiind
    ca este posibila utilizarea tastelor ARROW_LEFT si ARROW_RIGHT, pentru
    a schimba anumite setari. Lista setarilor ce pot fi modificate este:

    Table Size - modifica dimensiunea tablei de joc intre valorile 2 si 6,
    intrucat jocul nu functioneaza pe o dimensiune mai mica de 2x2 patratele,
    iar pe o dimensiune mai mare de 6x6, patratele nu vor aparea integral pe
    ecran, nici macar pe o fereastra full screen (150 x 40).

    Bot Delay - modifica timpul pe care AI-ul il asteapta pana cand acesta face
    o miscare, daca este activ. In cazul in care este setat la zero, miscarile 
    se vor face la un interval de 0.1 secunde, in orice alt caz va atepta
    numarul de secunde selectat + 0.1 secunde.

    Color Scheme - ofera posibilitatea de a schimba schema de culori intre
    10 culori prestabilite, care vor afecta jocul astfel: in meniuri, culoarea
    de selectie HIGHLIGHT_COLOR se schimba in culoarea selectata, iar in joc,
    casutele se vor colora gradiental de la alb spre culoarea selectata, in
    functie de valoarea casutei. Seturile disponibile sunt: 1-Rosu, 2-Verde,
    3-Galben, 4-Albastru, 5-Magenta, 6-Turqoise, 7-Violet, 8-Portocaliu, 9-Roz,
    10-Albastru Deschis.

    Back - intoarcere la meniul principal. Pe langa acesta se pot folosi
    tastele Q si ESC pentru a indeplini aceasta functie.

    Setarile vor fi salvate la iesirea din program in fisierul binar settings.txt
    pentru a nu fi nevoit sa schimbi setarile la cele preferate la intrarea in
    joc.

    EN:

    The settings menu behaves pretty much the same as the main menu, having the
    same dimension, and the same number of options, with the main difference that
    you can use the ARROW_LEFT and ARROW_RIGHT keys to switch between certain
    settings. The list of settings that may be modified is:

    Table Size - changes the dimension of the game table, from 2 and up to 6,
    since the game doesn't work with less than 2x2 squares, and with a dimension
    bigger than 6x6, the squers will not entirely fit the screen, not even on
    full screen windows (150 x 40 characters).

    Bot Delay - changes the time the AI waits before making a move, if it is
    activated. If this value is set to 0, the moves will be made with a 0.1
    seconds delay, with any other value, it will wait the selected number of
    seconds + 0.1 seconds.

    Color Scheme - offers the posibility of changing the color scheme with one of
    the 10 preset ones, which will change the HIGHLIGHT_COLOR in menus and in the
    game, the squares will color themselves with a gradient of that color, based
    on the value of the square. The preset colors are: 1-Red, 2-Green, 3-Yellow,
    4-Blue, 5-Magenta, 6-Turqoise, 7-Purple, 8-Orange, 9-Pink, 10-Light Blue.

    Back - goes back to the main menu. Keys Q and ESC can also be used for
    the same task.

    The settigns will be saved upon exitting the application in the binary file
    settings.txt so you don't have to change the setting again whenever you enter
    the game.

    ----- CHAPTER 3 - ACTUAL GAME FOOTAGE -----

    RO:

    Culorile patratelelor se stabilesc in functie de valoarea lor si de schema
    de culoare selectata, astfel incat valorile rgb scad in gradient de la alb
    spre culoarea selectata si care apoi devine din ce in ce mai intunecata, 
    printr-un contrast de negru, in 10 trepte succesive, de la 2 la 2048 
    (de la 2 la 1 pana la 2 la 10).
    Taste speciale:

    Q - intoarcere la meniul principal
    U - undo
    B - toggle bot on/off

    Highscore-ul se modifica odata cu scorul in cazul in care acesta depaseste
    highscore-ul curent pentru dimensiunea selectata a tablei de joc.

    EN:

    The colors of the squares will be set based on their value and the selected
    color scheme, with the rgb colors lower themselves from white down to the
    selected color, then becoming darker and darker, through a dark contrast, in
    10 succesive steps, from 2, up to 2048 (2^1 up to 2^10).
    Special keys:

    Q - goes back to the main menu
    U - undo
    B - toggle bot on/off

    Highscore changes together with the score when the current highscore for the
    selcted dimension is beaten.

    ----- CHAPTER 4 - SAVE FILES -----

    RO:

    Pe parcursul jocului se vor creea mai multe fisiere ".save" in folderul in
    care se afla executabilul 2048, iar fisierul makefile prezinta si optiunea
    "pclean" de clean partial, care nu sterge si fisierele de tip ".save".

    EN:

    During the game, several ".save" files will be created in the folder that
    contains the 2048 executable, which can be removed easily using the makefile
    "clean" option, whereas the option "pclean" does not remove the savefiles.

    ----- THE END -----