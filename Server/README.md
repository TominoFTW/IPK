## IPK23-Proj2:  IOTA: Server kalkulačka 
## Autor: Tomáš Běhal

## Popis
Server jako kalkulačka výrazů v prefixové notaci. Zpracovává TCP a UDP dotazy.
```console
$ make
```
```console
$ ./ipkcpd -h host -p port -m mode
```
## TCP
Při TCP komunikaci dojde nejprve k vytvoření soketu, přiřazení portu a adresy, na které bude server přijímat zprávy od klienta. Poté se provede `bind()` a `listen()`. Následně využívá funkce `accept()`, a `select()` který čeká na příchozí spojení s klienty. Po příchozím spojení se vytvoří nový soket, a uloží se do pole soketů, který je použit pro komunikaci s klientem.\
Cyklus prochází jednotlivé klienty a přijímá zprávy. Jako první zpráva od klienta je vyžadována zpráva `HELLO`, na kterou server odpoví `HELLO` a uloží si, že klient zahájil komunikaci, a nadále `HELLO` neočekává. Poté čeká na možné zprávy `SOLVE` query, které mohou přijít. Po přijetí zprávy `BYE` se spojení uzavře a soket se odstraní z pole soketů. Pokud kdykoliv přijde neočekávaná zpráva, server odpoví `BYE` a spojení se uzavře. Server může maximálně přijmout 16 klientů a další spojení s klienty je zahozeno.

## UDP

Při UDP komunikaci dojde nejprve k vytvoření soketu, přiřazení portu a adresy, na které bude server přijímat zprávy od klienta. Poté se provede `bind()`. Následně se využívá funkce `recvfrom()`, která čeká na příchozí zprávy od klienta.\
Zde běží `while(true)` cyklus, který očekává zprávu ve formátu `OPCODE|PAYLOADLEN|PAYLOAD`. `OPCODE` musí být `\x00`, jinak je zpráva zahozena a server odpovídá `b"\x01\x01\x1dError: Invalid message format"`. `PAYLOADLEN` je délka zprávy (maximálně 255), která je očekávána a `PAYLOAD` je samotná zpráva. Server vezme `PAYLOADLEN` zprávy a kontroluje validitu. Pokud je zpráva validní, provede se výpočet a odešle se odpověď ve tvaru `b"\x01\x00\xXX1*DIGIT"`. Pokud je zpráva nevalidní, server odpoví `b"\x01\x01\x1dError: Invalid message format"`.

  

## Výpočet

Výpočet jednotlivých dotazů provádí funkce `parse_message`, která zpracuje zprávu a vrátí výsledek, nebo `ERROR`, pokud se jednalo o nevalidní dotaz. Při TCP se nejdříve kontroluje, zda zpráva začíná na `"SOLVE "`. Pokud ne, nastaví se `error_flag` a na další části zprávy nezáleží a funkce vrací `ERROR`. Pokud obsahuje, dále funkce pokračuje jako u UDP.\
Kontrolují se validní tokeny, a to jsou pouze: `"(", ")", "DIGIT", "SP", "+", "-", "*" nebo "/"`. Načtení `"("` očekává jako další token `OPERATOR`, v opačném případě se nastaví `error_flag`. Pokud se jedná o `DIGIT`, dávají se do zásobníku, který se převede jednotné číslo, pokud se načte jiný token. Při načtení `SP` se nastavuje `SP_flag` aby se nemohlo načíst více mezer. Pokud se načte token `")"`, volá se funkce `evaluate`, která redukuje dvojici závorek podle operátoru, a nakonec dá na vrchol zásobníku mezi výsledek. Výpočet je validně ukončen, pokud je jako poslední znak načtena `")"` a nedošlo k žádné chybě. \
Funkce vrací výsledek, který je nadále zpracován podle kontrétního protokolu.

## Testování

Testovaní probíhalo za pomoci testovacího skriptu napsaného v jazyku python. `X` ve sloupci `PASSED znamená`, že test prošel.
### TCP
U TCP testuji, zda server odpovídá správně na jednoduché dotazy, jelikož složitější dotazy jsou testovány v UDP a na výpočet dotazů je použita stejná funkce. 
| INPUT                               | EXPECTED OUTPUT               | PASSED |
|-------------------------------------|-------------------------------|--------|
| `b"HELLO\nSOLVE (+ 1 2)\nBYE\n"`    | `b"HELLO\nRESULT 3\nBYE\n"`   |   X|
| `b"HELLO\nSOLVE (- 4 3)\nBYE\n"`    | `b"HELLO\nRESULT 1\nBYE\n"`   |   X|
| `b"HELLO\nSOLVE (* 5 6)\nBYE\n"`    | `b"HELLO\nRESULT 30\nBYE\n"`  |   X|
| `b"HELLO\nSOLVE (/ 8 4)\nBYE\n"`    | `b"HELLO\nRESULT 2\nBYE\n"`   |   X|
| `b"HELLO\nSOLVE (/ 8 0)\n"`         | `b"HELLO\nBYE\n"`             |   X|
| `b"HELLO\nBYE\n"`                   | `b"HELLO\nBYE\n"`             |   X|
| `b"HELLO\nSOLVE (+ 1"`              | `b"HELLO\nBYE\n"`             |   |
| `b"SOLVE (+ 1 2)\n"`                | `b"BYE\n"`                    |   X|

### UDP
V UDP testuji složitější dotazy, které v sobě mají více operací, zdali se server chová podle délky zprávy z dotazy (viz. test 5 a 6) a ošetření dělení nulou (viz. test 7).
| INPUT                              | EXPECTED OUTPUT                | PASSED |
|------------------------------------|--------------------------------|--------|
| `b"\x00\x07(+ 1 5)"`               | `b"\x01\x00\x016"`             |   X|
| `b"\x00\x07(- 5 2)"`               | `b"\x01\x00\x013"`             |   X|
| `b"\x00\x15(* 2 (+ 3 4) (- 5 1))"` | `b"\x01\x00\x0256"`            |   X|
| `b"\x00\x12(/ 200 (+ 100 50))"`    | `b"\x01\x00\x011"`             |   X|
| `b"\x00\x08(/ 13 2)(asd)"`         | `b"\x01\x00\x016"`             |   X|
| `b"\x00\x06(/ 13 2)"`              | `b"\x01\x01\x1dError: Invalid message format"` |   X|
| `b"\x00\x07(/ 0 0)"`               | `b"\x01\x01\x1dError: Invalid message format"` |   X|

## Bibliografie
[Servery z C++ stubs](https://git.fit.vutbr.cz/NESFIT/IPK-Projekty/src/branch/master/Stubs/cpp)\
[Protokol kalkulačky](https://git.fit.vutbr.cz/NESFIT/IPK-Projekty/src/branch/master/Project%201/Protocol.md)\
[Augemented BNF](https://en.wikipedia.org/wiki/Augmented_Backus%E2%80%93Naur_form)\
[Sockets in C++](https://www.geeksforgeeks.org/socket-programming-cc/)
