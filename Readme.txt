# Bunu Nichita-Adrian 323CA nichita_adrian.bunu@stud.acs.upb.ro
# Client web. Comunicatie cu REST API.

Mentionez de la inceput ca am pornit implementare cu laboratorul 9.

Programul executa comenzile citite de la tastatura.

# register
Sunt citite username-ul si parola si este trimis pachetul cu acestea
catre server, la url-ul specific.
Daca username-ul contine spatii sau tab-uri, se afiseaza eroare
si nu se trimite nimic spre server.
Programul intoarce eroare daca exista deja un cont cu username-ul
citit.

# login
Sunt citite username-ul si parola si este trimis pachetul cu acestea
catre server, la url-ul specific.
Daca username-ul contine spatii sau tab-uri, se afiseaza eroare
si nu se trimite nimic spre server.
Programul intoarce eroare daca username-ul si parola nu se potrivesc.
Cookie-urile sunt salvate petntru a putea efectua comenzile ce
necesita un user logat.

# enter_library
Este trimis un pachet care cere de la server un token.
Token-ul ne va folosi ulterior la comenzile ce il necesita.
Acesta va fi trimis in header dupa parametrul "Host".

Un user care nu este logat nu poate executa comanda.

# get_books
In functie de contul logat, sunt afisate id-ul si titlul fiecarei
carti adaugate de user-ul respectiv.

Un user care nu este logat nu poate executa comanda.
Un user care nu detine token-ul nu poate executa comanda.

# get_book
Este citit id-ul cartii cerute si trimite pachetul cu acesta catre
server la url-ul specific.
Raspunsul serverului va contine toate detaliile despre cartea
respectiva.
Detaliile afisate sunt in format JSON.

Daca id-ul nu este un numar se afiseaza eroare.
Un user care nu este logat nu poate executa comanda.
Un user care nu detine token-ul nu poate executa comanda.

# add_book
Sunt citite datele despre carti. Este trimis pachetul cu detaliile
in format JSON.
Dupa adaugare, biblioteca user-ului va contine noua carte.

Daca numarul de pagini nu este un numar se afiseaza eroare.

Un user care nu este logat nu poate executa comanda.
Un user care nu detine token-ul nu poate executa comanda.

# delete_book
Este citit id-ul cartii cerute si trimite pachetul cu acesta catre
server la url-ul specific.
Serverul va sterge cartea cu id-ul respectiv din biblioteca.
Daca id-ul nu este un numar se afiseaza eroare.

Un user care nu este logat nu poate executa comanda.
Un user care nu detine token-ul nu poate executa comanda.

# logout
User-ul curent este delogat. Cookie-urile acestuia nu mai sunt
luate in considerare (cookie_count = 0). Din acest motiv, user-ul
nu va mai putea sa execute comenzile ce necesita un user logat.

Un user care nu este logat nu poate executa comanda.

# exit
Executia programului este intrerupta, indiferent de stadiul in
care este programul.

## Detalii despre implementare

In fisierul requests.cpp exista 4 functii:

compute_get_request - creeaza un header petru "GET"
                      (sir de caractere)
compute_post_request - creeaza un header pentru "POST"
                       (sir de caractere)
compute_delete_request - creeaza un header pentru "DELETE"
                         (sir de caractere)

parse_response - creeaza raspunsul pentru fiecare comanda,
                iar in functie de comanda, poate returna cookie-ul/
                token-ul
               - in aux_data este retinut numarul de cookie-uri
                 parsat

Am ales sa folosesc biblioteca JSON "nlohmann" pentru siguranta
codului si simplitatea acestuia.
Aceasta este folosita cand creez un obiect json care va fi trimis
catre server.