## IPK23-Proj1: Klient ke vzdálené kalkulačce
## Autor: Tomáš Běhal

### Úvod
Klient pro vzdálenou kalkulačku je napsán v jazyce C++ na verzi 20 a je kompatibilní s operačním systémem Linux. Podpora pro Windows není zajištěna a vyžadovala by si úpravu určitých knihoven a funkcí.

Za pomocí příkazu `make` je možné vytvořit spustitelný soubor `ipkcpc` a spustit jej.
```console
$ ./ipkcpc -h <host_name> -p <port_number> -m <mode>
```
kde `<host_name>` je IPv4 adresa, `<port_number>` je číslo portu a `<mode>` je režim, ve kterém bude probíhat komunikace. Validní porty jsou od 1024 do 65535. Režimy jsou `tcp` a `udp`.\
Program nejdříve validuje argumenty příkazové řádky, poté přiřadí IP adresu a port. Dále se program rozděluje na dvě větve a to TCP a UDP. Dále se vytváří sokety a připojuje se k serveru. Následně se kontrolují délky zpráv a odesílá se zpráva na server. Poté se čeká na odpověď a vypíše se na standardní výstup. V případě chyby se vypíše chybová hláška na standardní chybový výstup a klient se ukončí. Funkcionality klienta je inspirována poskytnutými příklady ze stubs.

#### TCP
 - Zpráva je limitováná délkou 1022 znaků a dále očekává `'\n'` a `'\0'`.
 - Spojení lze ukončit pomocí `BYE` na standardní vstupu, nebo `C-c`.
#### UDP
 - Zpráva je limitováná délkou 255 znaků a dále očekává `'\0'`.
 - Zpráva je poté upravena na tvar `\0` + délka zprávy + zpráva a odeslána na server.
 - Spojení lze ukončit pomocí `C-c`.

### Testování
- Program jsem testoval vůči referenčnímu serveru na OS Ubuntu 20.04.
- Testování probíhalo na 0.0.0.0:2023 a 127.0.0.1:20231 jak pro TCP, tak pro UDP.

#### Příklady
```console
$ ./ipkcpc -h 0.0.0.0 -p 2023 -m tcp

INPUT:
HELLO
SOLVE (+ 3 4)
SOLVE (+ 3 (+ 3 2))
BYE

OUTPUT:
HELLO
RESULT 3
RESULT 8
BYE
```
```console
$ ./ipkcpc -h 127.0.0.1 -p 20231 -m udp

INPUT:
(+ 1 2)
(+ 1 (+ 1 2))
(+ a s 1 2)
HELLO
`EOF`

OUTPUT:
OK:3
OK:4
ERR:Could not parse the message
ERR:Could not parse the message
```

Následně testuji správnost zpracování argumentů příkazové řádky. Všechny tyto příkazy vyústí v chybu (error code 1).
```console
$ ./ipkcpc
$ ./ipkcpc -h 127.0.0.1
$ ./ipkcpc -p ahoj
...
```
dále následovaly sémanticky stejné testy.

### Reference
- https://git.fit.vutbr.cz/NESFIT/IPK-Projekty/src/branch/master/Stubs/cpp
- https://stackoverflow.com/questions/1641182/how-can-i-catch-a-ctrl-c-event
- https://stackoverflow.com/questions/13547721/udp-socket-set-timeout
