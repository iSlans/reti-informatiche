/**
 * Module about the specific "Elementals" escape room game
 *
 * If wanna add a new escape room, would need to implement a game2.c file
 */

#include "game1.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "connection.h"
#include "logging.h"
#include "time.h"

// #define GAME_TIME 10
#define GAME_TIME 60 * 10  // 10 min

#define OBJ_libro 0
#define OBJ_cristallo 1
#define OBJ_bastone 2
#define OBJ_parte_rovinata 3
#define TOTAL_FLAGS 4

enum GameFlag {
    FLAG_FREE = 0,
    FLAG_IN_BAG,
    FLAG_LOCKED,
    FLAG_ENIGMA_BLOCKED,
    FLAG_ENIGMA_SOLVED,
};

enum GameStatus {
    GAME_STATUS_NOT_STARTED = 0,
    GAME_STATUS_PLAYING = 100,
    GAME_STATUS_WIN = 200,
    GAME_STATUS_LOSE = 300,
    GAME_STATUS_TIMEOUT = 301,
};

struct DictMessage {
    char* id;
    char* msg;
};
static struct DictMessage messages[];
static char* getMessage(char* id);

static int equals(char* a, char* b) { return strcmp(a, b) == 0; }

int handle_game1(int fd, struct ClientState* client, char* request_type, char* request_args) {
    // char* response;

    if (equals(request_type, "init")) {
        int n_flags = TOTAL_FLAGS;
        time_t end_time = time(NULL) + GAME_TIME;

        client->is_playing = 1;

        client->game_data.end_time = end_time;
        client->game_data.status = GAME_STATUS_PLAYING;
        client->game_data.phase = 1;
        client->game_data.bag_size = 2;
        client->game_data.token_earned = 0;
        client->game_data.token_total = 4;
        client->game_data.flags = calloc(n_flags, sizeof(int));
        client->game_data.flags_len = n_flags;

        client->game_data.flags[OBJ_libro] = FLAG_ENIGMA_BLOCKED;
        client->game_data.flags[OBJ_cristallo] = FLAG_FREE;
        client->game_data.flags[OBJ_bastone] = FLAG_FREE;
        client->game_data.flags[OBJ_parte_rovinata] = FLAG_LOCKED;

        char* data = serialize_game_data(&(client->game_data));
        char response[1024] = "";
        snprintf(response, 1024, "OK %s", data);
        connection.send(fd, response);
        free(data);
        return 1;
    }

    time_t diff = client->game_data.end_time - time(NULL);
    if (diff <= 0) {
        client->game_data.status = GAME_STATUS_TIMEOUT;
        client->is_playing = 0;
        // connection.send(fd, "NK Game timeout");
        // return 1;
    }

    /* -------------------------------------------------------------------------- */
    /*                                    LOOK                                    */
    /* -------------------------------------------------------------------------- */
    // look doesn't need the object to be in bag
    if (equals(request_type, "look")) {
        char* response;
        response = "wrong.look";  // DEFAULT RESPONSE

        if (client->game_data.phase >= 1) {
            if (equals(request_args, "")) {
                response = "phase1.look";

            } else if (equals(request_args, "tavolo")) {
                response = "phase1.look tavolo";

            } else if (equals(request_args, "incisioni")) {
                response = "phase1.look incisioni";

            } else if (equals(request_args, "libro")) {
                response = "phase1.look libro";

            } else if (equals(request_args, "cristallo")) {
                response = "phase1.look cristallo";

            } else if (equals(request_args, "bastone")) {
                response = "phase1.look bastone";

            } else if (equals(request_args, "parte-rovinata")) {
                response = "phase1.look parte-rovinata";
            }
        }

        if (client->game_data.phase >= 2) {
            if (equals(request_args, "libro")) {
                response = "phase2.look libro";
            }
        }

        if (client->game_data.phase >= 3) {
            if (equals(request_args, "bastone")) {
                response = "phase3.look bastone";
            }
        }

        if (client->game_data.phase >= 4) {
            if (equals(request_args, "bastone")) {
                response = "phase4.look bastone";
            } else if (equals(request_args, "libro")) {
                response = "phase4.look libro";
            }
        }

        if (client->game_data.phase >= 5) {
            if (equals(request_args, "incisioni")) {
                response = "phase5.look incisioni";
            }
        }

        if (client->game_data.phase >= 6) {
            if (equals(request_args, "tavolo")) {
                response = "phase6.look tavolo";
            } else if (equals(request_args, "libro")) {
                response = "phase6.look libro";
            }
        }

        response = getMessage(response);
        char resp[1024] = "";
        sprintf(resp, "OK %s", response);
        connection.send(fd, resp);
        return 1;
    }

    /* -------------------------------------------------------------------------- */
    /*                                    TAKE                                    */
    /* -------------------------------------------------------------------------- */
    if (equals(request_type, "take")) {
        char* response;
        // CHECK IF ALREADY REACHED MAX BAG SIZE
        {
            int bag_quantity = 0;
            int* flags = client->game_data.flags;
            int objects[] = {
                OBJ_bastone,
                OBJ_cristallo,
                OBJ_libro,
                OBJ_parte_rovinata};
            for (int i = 0; i < ((sizeof objects) / (sizeof objects[0])); i++) {
                bag_quantity += flags[objects[i]] == FLAG_IN_BAG ? 1 : 0;
            }

            if (bag_quantity >= client->game_data.bag_size) {
                response = "full.take";
                // int a = 0;
                goto take_send_response;
            }
        }

        int next_phase = client->game_data.phase;
        response = "wrong.take";

        if (equals(request_args, "parte-rovinata")) {
            response = "phase1.take parte-rovinata";

        } else if (equals(request_args, "bastone")) {
            switch (client->game_data.flags[OBJ_bastone]) {
                case FLAG_IN_BAG:
                    response = "wrong.already.take";
                    break;
                case FLAG_FREE:
                    response = "phase1.take bastone";
                    client->game_data.flags[OBJ_bastone] = FLAG_IN_BAG;
                    break;
                default:
                    break;
            }
        } else if (equals(request_args, "cristallo")) {
            switch (client->game_data.flags[OBJ_cristallo]) {
                case FLAG_IN_BAG:
                    response = "wrong.already.take";
                    break;
                case FLAG_FREE:
                    response = "phase1.take cristallo";
                    client->game_data.flags[OBJ_cristallo] = FLAG_IN_BAG;
                    break;
                default:
                    break;
            }
        } else if (equals(request_args, "libro")) {
            switch (client->game_data.flags[OBJ_libro]) {
                case FLAG_IN_BAG:
                    response = "wrong.already.take";
                    break;
                case FLAG_FREE:
                    response = "phase1.take libro";
                    if (client->game_data.phase == 6) {
                        response = "phase6.take libro";
                    }
                    client->game_data.flags[OBJ_libro] = FLAG_IN_BAG;
                    break;

                case FLAG_ENIGMA_SOLVED:
                    response = "phase1.take libro";
                    client->game_data.flags[OBJ_libro] = FLAG_IN_BAG;
                    if (client->game_data.phase == 1) {
                        client->game_data.token_earned++;
                        next_phase = 2;
                    }
                    break;

                case FLAG_ENIGMA_BLOCKED: {
                    response = "phase1.enigma.question.take libro";
                    // response = getMessage(response);
                    break;
                }
                default:
                    break;
            }
        } else if (strstr(request_args, "libro SOLUTION")) {
            char reply[128] = "";
            sscanf(request_args, "libro SOLUTION %127[^\\0]", reply);

            if (client->game_data.phase == 1) {
                char* solution = getMessage("phase1.enigma.solution.take libro");
                if (equals(reply, solution)) {
                    client->game_data.flags[OBJ_libro] = FLAG_ENIGMA_SOLVED;
                    response = "phase1.enigma.solution.right.take libro";
                } else {
                    response = "phase1.enigma.solution.wrong.take libro";
                }
            }
        }

    take_send_response:
        // Update game phase in case user advanced to next phase
        client->game_data.phase = next_phase;

        response = getMessage(response);
        char resp[1024] = "";
        sprintf(resp, "OK %s", response);
        connection.send(fd, resp);
        return 1;
    }

    /* -------------------------------------------------------------------------- */
    /*                                    DROP                                    */
    /* -------------------------------------------------------------------------- */
    if (equals(request_type, "drop")) {
        char* response = "wrong.drop";

        int* flags = client->game_data.flags;

        if (equals(request_args, "bastone")) {
            if (flags[OBJ_bastone] == FLAG_IN_BAG) {
                flags[OBJ_bastone] = FLAG_FREE;
                response = "success.drop";
            }
        } else if (equals(request_args, "cristallo")) {
            if (flags[OBJ_cristallo] == FLAG_IN_BAG) {
                flags[OBJ_cristallo] = FLAG_FREE;
                response = "success.drop";
            }
        } else if (equals(request_args, "libro")) {
            if (flags[OBJ_libro] == FLAG_IN_BAG) {
                flags[OBJ_libro] = FLAG_FREE;
                response = "success.drop";
            }
        }

        response = getMessage(response);
        char resp[1024] = "";
        sprintf(resp, "OK %s", response);
        connection.send(fd, resp);
        return 1;
    }

    /* -------------------------------------------------------------------------- */
    /*                                     USE                                    */
    /* -------------------------------------------------------------------------- */
    if (equals(request_type, "use")) {
        char* response;
        response = "wrong.use";
        int* flags = client->game_data.flags;
        int game_phase = client->game_data.phase;

        if (game_phase == 2) {
            if (equals(request_args, "cristallo bastone")) {
                if (flags[OBJ_cristallo] == FLAG_IN_BAG &&
                    flags[OBJ_bastone] == FLAG_IN_BAG) {
                    flags[OBJ_cristallo] = FLAG_LOCKED;
                    response = "phase2.use cristallo bastone";
                    client->game_data.phase = 3;
                } else {
                    response = "wrong.missing.use";
                }
            }
        }

        if (game_phase == 3) {
            if (equals(request_args, "bastone libro")) {
                if (flags[OBJ_bastone] == FLAG_IN_BAG &&
                    flags[OBJ_libro] == FLAG_IN_BAG) {
                    response = "phase3.use bastone libro";
                    client->game_data.flags[OBJ_cristallo] = FLAG_LOCKED;
                    client->game_data.token_earned++;
                    client->game_data.phase = 4;
                } else {
                    response = "wrong.missing.use";
                }
            }
        }

        if (game_phase == 4) {
            if (equals(request_args, "libro")) {
                if (flags[OBJ_libro] == FLAG_IN_BAG) {
                    response = "phase4.use libro";
                    client->game_data.flags[OBJ_libro] = FLAG_LOCKED;
                    client->game_data.token_earned++;
                    client->game_data.phase = 5;
                } else {
                    response = "wrong.missing.use";
                }
            }
        }

        if (game_phase == 5) {
            if (equals(request_args, "bastone parte-rovinata")) {
                if (flags[OBJ_bastone] == FLAG_IN_BAG) {
                    response = "phase5.use bastone parte-rovinata";
                    client->game_data.flags[OBJ_libro] = FLAG_FREE;
                    client->game_data.token_earned++;
                    client->game_data.phase = 6;
                } else {
                    response = "wrong.missing.use";
                }
            }
        }

        if (game_phase == 6) {
            if (equals(request_args, "libro")) {
                if (flags[OBJ_libro] == FLAG_IN_BAG) {
                    response = "phase6.use libro";
                    client->game_data.flags[OBJ_libro] = FLAG_FREE;
                    client->game_data.phase = 7;
                    if (client->game_data.status == GAME_STATUS_PLAYING) {
                        client->game_data.status = GAME_STATUS_WIN;
                        client->is_playing = 0;
                    }
                } else {
                    response = "wrong.missing.use";
                }
            }
        }

        response = getMessage(response);
        char resp[1024] = "";
        sprintf(resp, "OK %s", response);
        connection.send(fd, resp);
        return 1;
    }

    if (equals(request_type, "objs")) {
        char resp[128] = "";
        int* flags = client->game_data.flags;
        if (flags[OBJ_bastone] == FLAG_IN_BAG) {
            strcat(resp, "-bastone\n");
        }
        if (flags[OBJ_cristallo] == FLAG_IN_BAG) {
            strcat(resp, "-cristallo\n");
        }
        if (flags[OBJ_libro] == FLAG_IN_BAG) {
            strcat(resp, "-libro\n");
        }

        if (equals(resp, "")) {
            strcat(resp, "Non hai niente in mano\n");
        }

        connection.send(fd, resp);
        return 1;
    }

    if (equals(request_type, "status")) {
        char* data = serialize_game_data(&(client->game_data));
        char response[1024] = "";
        snprintf(response, 1024, "OK %s", data);
        connection.send(fd, response);
        free(data);
        return 1;
    }

    connection.send(fd, "NK Error unknown command");
    return 0;
}

// Would be cool to use json...
static struct DictMessage messages[] = {

    {.id = "wrong.look", .msg = "Niente da vedere..."},
    {.id = "wrong.take", .msg = "Non si può prendere..."},
    {.id = "full.take", .msg = "Hai le mani piene, lascia qualcosa prima per liberarti una mano"},
    {.id = "wrong.already.take", .msg = "L'hai già in mano..."},
    {.id = "wrong.drop", .msg = "Non puoi lasciarlo se non lo avevi nemmeno in mano..."},
    {.id = "success.drop", .msg = "Lasciato da una parte, ora hai una mano libera."},
    {.id = "wrong.use", .msg = "C'è tempo e luogo per ogni cosa, ma non ora."},
    {.id = "wrong.missing.use", .msg = "Non hai le cose in mano per usarle!"},

    // FASE 1
    {.id = "phase1.look",
     .msg = "Sei in una stanza. C'è un ++tavolo++ tutto disordinato. Tra la roba, vedi un **libro** strano ed un **cristallo**. Sui muri si vedono delle ++incisioni++ rovinate. A terra un **bastone**. Ti chiedi come sei finito qui e quanto tempo ti rimanga prima che finisca l'ossigeno in questo luogo chiuso senza porte e finestre..."},

    {.id = "phase1.look tavolo",
     .msg = "Sul tavolo c'è un grande disordine con tutti gli oggetti che ci sono. Magari c'è qualcosa di utile."},

    {.id = "phase1.look incisioni",
     .msg = "Guardando da vicino le incisioni rovinate sui muri sembra sia un disegno con forme geometriche e simboli strani. Forse completando la **parte-rovinata** si scoprirà qualcosa..."},

    {.id = "phase1.look libro",
     .msg = "Il libro salta molto all'occhio con quella pietra in mezzo alla copertina."},

    {.id = "phase1.look cristallo",
     .msg = "Un cristallo opaco. A vista nulla di speciale, ma riesci a sentire che emana una debole energia."},

    {.id = "phase1.look bastone",
     .msg = "Sembra un bastone di legno pregiato. Sono incisi disegni geometrici e lettere. In cima ha una cavità."},

    {.id = "phase1.look parte-rovinata",
     .msg = "Parte rovinata delle incisioni sul muro. Emana una debole strana energia."},

    {.id = "phase1.take parte-rovinata",
     .msg = "E' una parte rovinata del muro, non si può prendere..."},

    {.id = "phase1.take bastone",
     .msg = "Hai preso il bastone da terra"},

    {.id = "phase1.take cristallo",
     .msg = "Hai preso il cristallo dal tavolo"},

    {.id = "phase1.take libro",
     .msg = "Hai preso il libro dal tavolo. Il **cristallo** ha avuto una reazione quando hai risolto l'enigma..."},

    {.id = "phase1.enigma.question.take libro",
     .msg = "ENIGMA Appare in aria una domanda: Quale tra questi non è uno dei 5 elementi: \nFuoco, Terra, Aria, Acqua, Metallo, Legno"},

    {.id = "phase1.enigma.solution.take libro",
     .msg = "Aria"},

    {.id = "phase1.enigma.solution.right.take libro",
     .msg = "Scompare la domanda nell'aria, ora puoi prendere il libro"},

    {.id = "phase1.enigma.solution.wrong.take libro",
     .msg = "Risposta sbagliata, il libro è protetto da una strana energia che non ti fa avvicinare..."},

    // FASE 2
    {.id = "phase2.look libro",
     .msg = "Un libro strano, ha una pietra in mezzo alla copertina e le pagine sono tutte vuote. Chissà se sarà utile..."},

    {.id = "phase2.use cristallo bastone",
     .msg = "Il cristallo si è incastrato perfettamente nella cavità in cima al bastone.\n"
            "Ti sei liberato pure la mano."},

    // FASE 3
    {.id = "phase3.look bastone",
     .msg = "Adesso il bastone emana un'aura potentissima! O.O\n"
            "Sta reagendo a qualcosa nella stanza..."},

    {.id = "phase3.use bastone libro",
     .msg = "L'energia del bastone sta reagendo al libro...\n"
            "Il libro sta assorbendo l'energia circostante...\n"
            "Sulle pagine stanno comparendo testi e disegni!"},

    // FASE 4
    {.id = "phase4.look bastone",
     .msg = "L'energia emanata è diminuita rispetto a prima, ma ne ha ancora un bel po'..."},

    {.id = "phase4.look libro",
     .msg = "Adesso il libro fluttua sulla tua mano ed emana un'energia piacevole, che sia mana? Non sembra...\n"
            "Tra le pagine, vedi una figura simile a quella incisa sul muro!\n"
            "Che sia la chiave per completare la **parte-rovinata**?"},

    {.id = "phase4.use libro",
     .msg = "Il libro fluttua verso il centro della stanza e sprigiona un'energia a cinque colori.\n"
            "Sta proiettando la figura geometrica sui muri, e coincide con le ++incisioni++!"},

    // FASE 5
    {.id = "phase5.look incisioni",
     .msg = "Grazie alle proiezioni dal libro, si vede bene come sarebbe il disegno sul muro, completando la **parte-rovinata**. Adesso il problema è ripararlo..."},
    {.id = "phase5.use bastone parte-rovinata",
     .msg = "Ti senti come guidato dal bastone, che inizia a raccogliere l'energia emanata dal libro e disegnare simboli sui muri...\n"
            "Dopo un po' vedi che la parte-rovinata si è riparata ed il **libro** è atterrato sul ++tavolo++."},

    // FASE 6
    {.id = "phase6.look tavolo",
     .msg = "Vedi il **libro** atterrato sul tavolo, non emana più energia come prima ma la **pietra** sulla copertina si è tinta di cinque colori elementari."},

    {.id = "phase6.look libro",
     .msg = "La **pietra** sulla copertina si è tinta di cinque colori elementari."},

    {.id = "phase6.take libro",
     .msg = "Hai preso il libro"},

    {.id = "phase6.use libro",
     .msg = "Appoggi la mano sulla pietra del libro, attivando le incisioni sul muro della stanza.\n"
            "Senti in testa la parola \"Link\" e lo ripeti a tua volta, si innalza una luce accecante e senti un po di nausea...\n"
            "Riapri gli occhi, ti ritrovi a casa tua, con in mano il libro... di reti informatiche.\n"},

    // END LIST
    {.id = NULL, .msg = NULL},

};

static char* getMessage(char* id) {
    // int len = (sizeof messages) / (sizeof messages[0]);
    for (int i = 0; messages[i].id != NULL; i++) {
        if (strcmp(messages[i].id, id) == 0) {
            return messages[i].msg;
        }
    }
    logging_error("No game message found!!! id: %s", id);
    return "Server Game Error";
}
