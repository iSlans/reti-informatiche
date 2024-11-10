# Implementation
using TCP transport protocol, need a reliable data transport 

communication between client and server consist of 2 send/recv:
    - 1 send/recv first to tell how long the next message will be (binary data protocol)
    - 1 send/recv for the effective message (text data protocol)

The state is server-side, where keeps track of user and game state.

## Server
server implementation is iterative, I/O multiplexing. 
It's designed so it can serve multiple clients, multiple requests from clients and stdin.
It can scale up, not as much as a concurrent server but It's not expected to reach very high numbers of connections and requests.
Also being iterative makes the implementation simpler than a concurrent server.

## Client
Upon tcp connection, a client is always connected, until errors and client close.
The client mainly awaits a user input, then send the request to server and expect a response from it. 








```
USR SIGN
USR LOGIN
USR LOGOUT

// GET GAMELIST
// GET GAMEDATA 
or client side file
time, locations, objects, handlimit, enigms, ...

start room client side

GAM START
GAM ACTION

```

| client request       | accepted response |
| -------------------- | ----------------- |
| USR LOGIN name pass  | OK / NK           |
| USR SIGNUP name pass | OK / NK           |
| USR LOGOUT           | OK                |


GAME
- status
- endtime
- phase

phases: Game phases, to be unlocked one after one to reach the end of game
1. start
2. p2
3. p3
4. p4
5. p5
...
7. end
8. extra(?)



server track game phase, time
client have game data

---------------------------------------------------------

# GAME WALKTHROUGH

enigmi:
    - domanda e risposta su take libro
    - sequenza <use cristallo bastone> <use bastone libro> <use libro> <use bastone> per sbloccare ed avanzare nel gioco


### FASE 1

look 
Sei in una stanza. C'è un ++tavolo++ tutto disordinato. 
Tra la roba, vedi un **libro** strano ed un **cristallo**.
Sui muri si vedono delle ++incisioni++ rovinate. A terra un **bastone**. Ti chiedi come sei finito qui e quanto tempo ti rimane prima che finisca l'ossigeno in questo luogo chiuso senza porte e finestre... T.T

look tavolo
Sul tavolo c'è un grande disordine con tutti gli oggetti che ci sono. Magari c'è qualcosa di utile.

look incisioni
Guardando da vicino le incisioni rovinate sui muri sembra sia un disegno con forme geometriche e simboli strani. Forse completando la **parte rovinata** si scoprirà qualcosa...

look libro
Il libro salta molto all'occhio con quella pietra in mezzo alla copertina.

look cristallo
Un cristallo opaco. A vista nulla di speciale, ma riesci a sentire che emana una debole energia.

look bastone
Sembra un bastone di legno pregiato. Sono incisi disegni geometrici e lettere. In cima ha una cavità.

look parte rovinata
Parte rovinata delle incisioni sul muro. Emana una strana energia debole.

take parte rovinata 
    no effect: E' una parte del muro, non si può prendere...
take bastone
    Hai preso il bastone da terra
take cristallo
    Hai preso il cristallo dal tavolo

take libro 
    enigma: Appare in aria una domanda: Quale tra questi non è uno dei 5 elementi: Fuoco, Terra, Aria, Acqua, Metallo, Legno
    risposta: Aria
    - Risposta giusta, ora puoi prendere il libro.
    - Risposta sbagliata, il libro è protetto da una strana energia che non ti fa avvicinare...
    - Hai preso il libro dal tavolo, il **cristallo** ha avuto una reazione quando hai risolto l'enigma...

get a progress token, access to next phase

### FASE 2

look libro
Un libro strano, ha una pietra in mezzo alla copertina e le pagine sono tutte vuote. Chissà se sarà utile...

use cristallo bastone
Il cristallo si è incastrato perfettamente nella cavità in coma al bastone!
- cristallo removed from bag
- next phase

### FASE 3

look bastone
Adesso il bastone emana un'aura potentissima! O.O
Sta reagendo a qualcosa nella stanza...

use bastone libro
L'energia del bastone sta reagendo al libro...\n
Il libro sta assorbendo l'energia circostante...\n
Sulle pagine stanno comparendo testi e disegni!

+1 token, access next phase

### FASE 4

look bastone
L'energia emanata è diminuita rispetto a prima, ma ne ha ancora un bel po'...

look libro
Adesso il libro fluttua in aria ed emana un'energia piacevole... che sia mana?
Tra le pagine, vedi una figura simile a quella incisa sul muro!
Che sia la chiave per completare la **parte rovinata**?

use libro
Il libro fluttua (mano libera) verso il centro della stanza e sprigiona un'energia a 5 colori.
Sta proiettando la figura geometrica sui muri, e coincide con le ++incisioni++!

-libro removed from bag 
+1 token, access next phase

### FASE 5

look incisioni
Grazie alle proiezioni dal libro, si vede bene come sarebbe il disegno sul muro, completando la **parte rovinata**. Adesso il problema è ripararlo...

use bastone parte rovinata
Ti senti come guidato dal bastone, che inizia a raccogliere l'energia emanata dal libro e disegnare simboli sui muri...
Dopo un po' vedi che la parte rovinata si è riparata ed il **libro** è atterrato sul ++tavolo++.

+1 token, access next phase

### FASE 6

look tavolo
Vedi il **libro** atterrato sul tavolo, non emana più energia come prima ma la pietra sulla copertina si è tinta di 5 colori elementari.

look libro
La pietra sulla copertina si è tinta di 5 colori elementari. 

take libro 

use libro
Appoggi la mano sulla pietra del libro, attivando le incisioni sul muro della stanza. Senti in testa la parola "Link" e lo ripeti a tua volta, si innalza una luce accecante e senti un po di nausea...
Riapri gli occhi e ti ritrovi adesso in casa tua, con in mano il libro... di reti informatiche.

END STORY

