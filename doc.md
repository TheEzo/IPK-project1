# Dokumentace

### Obsah
* [Aplikační protokol](#aplikační-protokol)
* [Komunikace](#komunikace)
* [Kontroly](#kontroly)
* [Spuštění](#spuštění)
* [Zdroje](#zdroje)

#### Aplikační protokol
* Používá vždy jednu ze dvou základních služeb transportní vrstvy
	* TCP -- použito v projektu
	* UDP (nespolehlivá komunikace)
* Pro rozlišení různých druhů komunikací se používají porty
* Každé spojení musí obsahovat 
	* Číslo portu
	* Adresu cílového počítače
	* Transportní protokol

#### Komunikace
* Komunikace je zahájena 'welcome' paketem, skrze který se stanový důležité údaje přenosu dat;
	* Pokud není přijat žádný takový paket, server čeká na semaforu než nějaký paket přijde (vyhne se tak aktivnímu čekání)ů
* Klient pošle serveru data, podle kterých se rozparsuje cílový soubor;
	* Jakákoliv jiná komunikace je taktéž relalizována pomocí semaforů;
* Výsledná data se posílají pomocí TCP spojení zpátky klientovi;
	* Podle množství se pošlou buď v jedné dávce, nebo ve více;
* Přijatá data jsou vypsána na standartní výstup;
* Každá komunikace probíhá v nové větvi (forku), což zajišťuje možnost obsloužit více klientů ve stejném čase;

#### Kontroly
* Parametry jsou kontrolovány pomocí getopts;
* K zajištění pořadí a správnost všech paketů se klient po přijetí každého paketu posílá zpět na server zprávu, jestli zpráva došla v pořádku; 

#### Spuštění
* Pro překlad je vytvořen ```Makefile```, který vytvoří binární soubor pro server i pro klienta
* Spuštění serveru
	* -p port
```bash
./ipk-server -p port
```
* Spuštění klienta
	* Parametry
		* -h host
		* -p port
		* -n login   -- vrací plné uživatelské jméno a případné další informace 
		* -f login   -- vrací informaci o domovském adresáři
		* -l [login] -- vrací seznam všech uživatelských jmen s prefixem loginu
```bash
./ipk-client -h merlin.fit.vutbr.cz -p 55555 -p rysavy
Rysavy Ondrej,UIFS,541141118 

./ipk-client -h eva.fit.vutbr.cz -p 55555 -f rysavy
/homes/kazi/rysavy

./ipk-client -h host -p port -l
avahi
...
zizkaj

./ipk-client -h host -p port -l xa
xabduk00
...
xaygun00
```

#### Zdroje
-- Demo aplikace TCP
-- Demo aplikace Fork
-- Třetí prezentace IPK
-- StackOverflow (getopts)
-- [Wikipedie](https://cs.wikipedia.org/wiki/Aplika%C4%8Dn%C3%AD_vrstva)
