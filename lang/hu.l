# Emacs: -*- mode: fundamental; coding: binary; -*-

# IRC Services is copyright (c) 1996-2009 Andrew Church.
#     E-mail: <achurch@achurch.org>
# Parts written by Andrew Kempe and others.
# This program is free but copyrighted software; see the file GPL.txt for
# details.

# Hungarian language file (Magyar nyelvi fájl)

# =======================================================
# Translated by:
# =======================================================
#     Kapitány János 'Sarkanyka' (kapitany.janos@gmail.com)
#     Romek Krisztián 'AngryWolf' (not active)
#	       IRC.LAP.HU | http://irc.lap.hu
# =======================================================

# Amikor lefordítod ezt a fájlt más nyelvre, tartsd észben, hogy a sprintf()
# paramétereinek sorrendje rögzitett a forráskódban, tehát az üzeneteidnek
# ugyanazokat a paramétereket ugyanabban a sorrendben kell átvennie, mint
# ahogy az angol üzenetekben vannak. (Nyilvánvalóan, ez nem fogja megtartani az
# strftime() formátumú sorokat.) Ha nem tudod egyszerûen lefordítani
# az üzenetet anélkül, hogy megváltoztatnád a paraméterek sorredjét, kérlek,
# tájékoztasd a Service fejlesztõit.
#
# A help üzentekben, "%S" (Nagy S, nem kisbetû) az üzenetet küldõ service nevére
# vonatkozik; például a NickServ help üzenetekben az "%S" a "NickServ"
# helyettesítõje (vagy lehet bármi más, amire átnevezted a config.h fájlban).
# Az %S's nem számit sprintf() paraméternek, tehát ez
# átrendezhetõ, elmozdítható vagy hozzáadható, ha szükséges.
#
# Emellett a help üzenetekben, kérlek, próbáld meg korlátozni a sorok hosszát
# 60 karakteres szövegre (nem beleértve a sorkezdõ tabulátort).  Azért ezt a
# hosszúságot választottuk, mert nem bontja szét a sorokat a legtöbb jelenlegi
# IRC kliens alapbeállításaival. Ne felejtsd el, hogy a formázó karaktereket
# (control-B, control-_) nem tartalmazza a 60 karakteres korlátozás (mivel nem
# jelennek meg a felhasználó képernyõjén).  Arra is emlékezz, hogy a formátum-
# specifikusok (%S, stb.) kiterjednek, amikor megjelennek, tehát ne hagyd
# figyelmen kivül; észben kell tartanod, hogy az álkliens neve (%S helyettesitõje)
# 8 karakteres lesz, vagyis minden egyes %S karakter 6-tal csökkenti a maximális
# sorhosszúságot.
#
# Végül, ne felejts el tabulátort rakni minden sor elejére a szövegben
# (még az üres soroknál is)!  Ezeknek tabulátoroknak kell lenniük,
# nem szóközöknek.

###########################################################################
#
# A nyelv neve
#
###########################################################################

# Az angoltól különbözõ nyelvek esetén ennek a sornak a következõ formában
# kell kinéznie:
#     nyelv-neve-a-nyelvben (nyelv-neve-angolul)
# Például, "Español (Spanish)" vagy "Français (French)".

LANG_NAME
	Magyar (Hungarian)

# A nyelv által használt karakter készlet.  Ez a "charset" érték
# amit a levélolvasó és webböngészõ szoftver értelmez.

LANG_CHARSET
	iso-8859-2

###########################################################################
#
# Általános üzenetek
#
###########################################################################

# Ez forma sor a strftime()  man 3 strftime  informació a formátum specifikusok
# jelentésérõl.  Rövid verzió:
#	%a = hét napjának neve (rövid)	%H = óra
#	%b = hónap neve (rövid)			%M = perc
#	%d = hónap napja 				%S = másodperc
#	%Y = év					%Z = idõ zona

# Ezt használjuk formátum sztringként az strftime()-ban dátumhoz és idõhöz
# együtt.
STRFTIME_DATE_TIME_FORMAT
	%Y %b %d %H:%M:%S %Z
# Ezt használjuk formátumsztringként az strftime()-ban egyedül a dátumhoz,
# hosszú alakban (angolban: a hét napjának nevével együtt).
STRFTIME_LONG_DATE_FORMAT
	%a %b %d %Y
# Ezt használjuk formátumsztringként az strftime()-ban egyedül a dátumhoz,
# rövid alakban (angolban: a hét napjának neve nélkül).
STRFTIME_SHORT_DATE_FORMAT
	%b %d %Y

# Ezek mondják el a strftime()-nak, hogy mi a neve a hónapoknak és napoknak.
# Ha Te nem használsz %a, %A, %b vagy %B karaktereket a fenti strftime()
# sztringjeidben, akkor üresen hagyhatod.  De ha beírsz neveket,
# KÖTELEZÕ sorrendben szerepelniük, soronként egy álljon, és legyen TELJES
# a lista!

# %a
STRFTIME_DAYS_SHORT
	Vasárnap
	Hétfõ
	Kedd
	Szerda
	Csütörtök
	Péntek
	Szombat
# %A
STRFTIME_DAYS_LONG
	Vasárnap
	Hétfõ
	Kedd
	Szerda
	Csütörtök
	Péntek
	Szombat
# %b
STRFTIME_MONTHS_SHORT
	Jan
	Feb
	Márc
	Ápr
	Máj
	Jún
	Júl
	Aug
	Szept
	Oct
	Nov
	Dec
# %B
STRFTIME_MONTHS_LONG
	Január
	Február
	Március
	Április
	Május
	Június
	Július
	Augusztus
	Szeptember
	Október
	November
	December


# "nap", "óra", "perc", és "másodperc" egyes- és többes számban.
# Ezeket meg kell elõzni egy szóközzel, ha a nyelv igényli
# (pl. angolban: "10 hours", nem "10hours").
STR_DAY
	 nap
STR_DAYS
	 nap
STR_HOUR
	 óra
STR_HOURS
	 óra
STR_MINUTE
	 perc
STR_MINUTES
	 perc
STR_SECOND
	 másodperc
STR_SECONDS
	 másodperc
# Elválasztást használunk az idõegységek között; az angol forma szerint
# a vesszõt szóköz követi (a ", " a "3 óra, 10 perc" kifejezés között van).
STR_TIMESEP
	,

# Ez a ChanServ/NickServ INFO kijelzésekben szerepel, és van egy vesszõ, amit
# egy szóköz követ (", " -- lényeges az utána lévõ szóköz), vagy egyenértékû
# a nyelv karakterkészletével.
COMMA_SPACE
	,


# Különbözõ hibaüzenetek.
INTERNAL_ERROR
	Belsõ hiba--nem teljesíthetõ a kérés.
SERVICES_IS_BUSY
	Services jelenleg foglalt; kérlek próbáld késõbb.

UNKNOWN_COMMAND
	Ismeretlen parancs: %s.
UNKNOWN_COMMAND_HELP
	Ismeretlen parancs: %s. További segítségért írd be: /msg %s HELP.
SYNTAX_ERROR
	Szintaxis: %s
MORE_INFO
	További információért írd be: /msg %s HELP %s.
NO_HELP_AVAILABLE
	Nem áll rendelkezésre segítség: %s.
MISSING_QUOTE
	Idézõjel (") hiányzik.
BAD_EMAIL
	Az E-mail címnek usernév@hostnév alakban kell lennie. Nem
	használhatod a színeket, félkövér, aláhúzott vagy forditott karaktereket,
	illetve a következõ karaktereket: , : ; | \ " ( ) < > [ ]
REJECTED_EMAIL
	Sajnálom, ez az E-mail cím nem használható a regisztráláshoz.
BAD_URL
	Az URL-nek http://gazdanév[:port]/... alakban kell lennie
	(vagy ftp://, stb.).
BAD_USERHOST_MASK
	A maszknak user@host alakban kell lennie.
BAD_NICKUSERHOST_MASK
	A maszknak nick!user@host alakban kell lennie.
BAD_EXPIRY_TIME
	Érvénytelen lejárati idõ.
SENDMAIL_NO_RESOURCES
	A rendszer jelenleg túl forgalmas, hogy levelet küldjön;
	kérlek próbáld újra késõbb.
READ_ONLY_MODE
	Megjegyzés: A Services jelenleg csak olvasható módban fut;
	a változtatások nem kerülnek mentése!
PASSWORD_INCORRECT
	Hibás jelszót adtál meg.
PASSWORD_WARNING
	Figyelmeztetés: Még egy rossz jelszó megadása után a kapcsolatod
	meg fog szakadni.

# Hogy mi a különbség az "Hozzáférés megtagadva" és az "Engedély letiltva" között?
# Nagyon kicsi; biztonsággal nevezhetõ azonos üzenetnek, valódi veszteség nélkül.
# Ha meg akarod különböztetni õket, az "Hozzáférés letiltva" általában gyakrabban
# használt; például, a non-oper próbálkozás az OperServ-hez való hozzáférésnél
# "Hozzáférés megtagadva", amíg a non-Services admin hozzáférési próbálkozás
# a NickServ SET NOEXPIRE parancshoz "Engedély letiltva".
ACCESS_DENIED
	Hozzáférés megtagadva.
PERMISSION_DENIED
	Engedély megtagadva.
MORE_OBSCURE_PASSWORD
	Kérlek próbálkozz újra egy biztonságosabb jelszóval. A jelszónak
	legalább 5 karakter hosszúnak kell lennie, és nem lehet valami
	könnyen kitalálható.(Pl: A Te valódi neved vagy a nicked.), és nem
	tartalmazhatja a szóköz és tabulátor karaktereket.
NICK_NOT_REGISTERED
	A nicked nem regisztrált.
NICK_NOT_REGISTERED_HELP
	A nicked nem regisztrált. Írd be: /msg %s HELP, hogy információt
	kapj a neved regisztrálásának módjáról.
NICK_TOO_LONG
	A nickneved nem lehet hosszabb %d karakternél.
NICK_INVALID
	%s egy nem érvényes nicknév.
NICK_X_NOT_REGISTERED
	A megadott nick (%s) nem regisztrált.
NICK_X_ALREADY_REGISTERED
	A megadott nick (%s) nick már regisztrált!
NICK_X_NOT_IN_USE
	A megadott nick (%s) nick jelenleg nincs használatban.
NICK_X_FORBIDDEN
	A megadott nick (%s) nick nem használható.
NICK_X_SUSPENDED
	A megadott nick (%s) használata fel van függesztve,
	ezért nem használható.
NICK_X_SUSPENDED_MEMOS
	A megadott nick (%s) használata fel van függesztve,
	ezért nem fogadhat memokat..
NICK_IDENTIFY_REQUIRED
	Jelszavas hitelesítés szükséges ehhez a parancshoz.
	Próbáld újra, miután beírtad: /msg %s IDENTIFY jelszó.
NICK_PLEASE_AUTH
	A nickneved nincs hitelesítve. Kérlek hitelesítsd a nickneved
	a hitelesítõ kóddal, amit a levélben kaptál: %s
NICK_X_NOT_ON_CHAN_X
	A megadott nick (%s) nincs használatban ezen a csatornán: %s.
CHAN_INVALID
	%s nem érvényes csatornanév.
CHAN_X_NOT_REGISTERED
	A megadott csatorna (%s) nem regisztrált.
CHAN_X_NOT_IN_USE
	A megdott csatorna: (%s) jelenleg nincs használatban.
CHAN_X_FORBIDDEN
	A megadott csatorna: (%s) nem használható, regisztrálható.
CHAN_X_SUSPENDED
	A megadott csatorna (%s) használata fel van függesztve,
	nem használható.
CHAN_X_SUSPENDED_MEMOS
	A megadott csatornak (%s) használata fel van függesztve,
	nem fogadhat üzeneteket.
CHAN_IDENTIFY_REQUIRED
	Jelszavas azonosítás szükséges ehhez a parancshoz.
	Próbáld újra miután beirtad: /msg %s IDENTIFY %s jelszó.
SERV_X_NOT_FOUND
	A megadott szerver (%s) nem található.
EXPIRES_NONE
	nem elévülõ
EXPIRES_NOW
	elévült
EXPIRES_IN
	elévülési idõ: %s
LIST_RESULTS
	Lista vége; %d/%d találat megjelenítve.

###########################################################################
#
# NickServ üzenetek
#
###########################################################################

# Automatikus üzenetek
NICK_IS_REGISTERED
	Ez a nick már foglalt. Kérlek válassz másikat. (Ha ez a Te neved,
	írd be: /msg %s IDENTIFY jelszó.)
NICK_IS_SECURE
	Ez a nick regisztrált és védett. Ha ez a te neved, írd be:
	/msg %s IDENTIFY jelszó. Máskülönben kérlek, válassz egy másik nicknevet!
NICK_MAY_NOT_BE_USED
	Ez a nicknév nem használható. Kérlek válassz másikat!
DISCONNECT_IN_1_MINUTE
	Ha nem változtatod meg a nickneved egy percen belül,
	akkor a kapcsolatod meg fog szakadni.
DISCONNECT_IN_20_SECONDS
	Ha nem változtatod meg a nicknevedet 20 másodpercen belül,
	akkor a kapcsolatod meg fog szakadni.
DISCONNECT_NOW
	Ez a nicknév regisztrált; nem használhatod. Kérlek válassz
	másik nicknevet, mielõtt újracsatlakozol.
FORCENICKCHANGE_IN_1_MINUTE
	Ha nem valtoztatod meg a nicknevedet egy percen belul, akkor
	automatikusan meg fog valtozni a nickneved.
FORCENICKCHANGE_IN_20_SECONDS
	Ha nem valtoztatod meg a nicknevedet 20 masodpercen belul,
	akkor automatikusan meg fog valtozni a nickneved.
FORCENICKCHANGE_NOW
	Ez a nicknév regisztrált; te nem használhatod.  A te nickneved
	most meg lesz változtatva: %s.
NICK_EXPIRES_SOON
	Megjegyzés: A nickneved regisztrációja el fog évülni: %s.
	Ha továbbra is használni szeretnéd a nevedet, kérlek azonositsd
	%s nicked a következõ parancs beírásával: /msg %s IDENTIFY jelszó
NICK_EXPIRED
	Megjegyzés: A nickneved regisztrációja elévült. Kérlek
	regisztráld újra, ha folytatni szeretnéd a használatát.

# REGISTER válasz
NICK_REGISTER_SYNTAX
	REGISTER jelszó [E-mail]
NICK_REGISTER_REQ_EMAIL_SYNTAX
	REGISTER jelszó E-mail
NICK_REGISTRATION_DISABLED
	Sajnálom, a nicknév-regisztráció jelenleg nem mûködik.
NICK_REGISTRATION_FAILED
	Sajnálom, a regisztráció sikertelen.
NICK_REG_PLEASE_WAIT
	Kérlek várj %s mielõtt újra használnád a REGISTER parancsot.
NICK_REG_PLEASE_WAIT_FIRST
	Kérlek várj %s mielõtt újra használod a REGISTER parancsot.
NICK_CANNOT_BE_REGISTERED
	Ez a nicknév: %s nem regisztrálható.
NICK_REGISTER_EMAIL_UNAUTHED
	Az E-mail cím amit megadtál még nincs hitelesítve. Kélek használd
	az AUTH parancsot, hogy hitelesítsd a címet mielõtt új nickneveket
	regisztrálsz vele.
NICK_REGISTER_TOO_MANY_NICKS
	Neked már túl sok neved van regisztrálva (összesen %d,
	a maximum %d). Nem regisztrálhatsz újat, amíg nem dropolsz
	néhányat a jelenlegi nickneveid közül.
NICK_REGISTERED
	Ez a nicknév: %s számodra lett regisztrálva.
NICK_PASSWORD_IS
	A jelszavad: %s -- emlékezz erre a nicked késõbbi használatára!

# IDENTIFY válasz
NICK_IDENTIFY_SYNTAX
	IDENTIFY jelszó
NICK_IDENTIFY_FAILED
	Sajnálom, az azonosítás sikertelen.
NICK_IDENTIFY_EMAIL_MISSING
	Be kell állítanod egy E-mail címet a nicknevedhez mielõtt
	folytatnád a használatát. Kérlek, állíts be egyet most a következõ
	paranccsal: /msg %s SET EMAIL E-mail címed
NICK_IDENTIFY_SUCCEEDED
	Jelszavad elfogadva. -- Azonosítás sikeres.

# DROP válasz
NICK_DROP_SYNTAX
	DROP jelszó
NICK_DROP_WARNING
	Megjegyzés: a DROP parancs törli a regisztrációját minden
	nicknek ami linkelt!
NICK_DROP_DISABLED
	Sajnálom, a nicknevek regisztrációjának törlése jelenleg szünetel.
NICK_DROP_FAILED
	Sajnálom, a nicknév regisztrációjának törlése sikertelen.
NICK_DROPPED
	A nickneved már nem regisztrált.
NICK_DROPPED_LINKS
	A nickneved és minden hozzá linkelt nicknév már nem regisztrált.

# DROPNICK válasz
NICK_DROPNICK_SYNTAX
	DROPNICK nick
NICK_X_DROPPED
	Ez a nicknév: %s már nem regisztrált.
NICK_X_DROPPED_LINKS
	Ez a nicknév %s és az összes hozzá linkelt nicknév már
	nem regisztrált.

# DROPEMAIL válasz
NICK_DROPEMAIL_SYNTAX
	DROPEMAIL példa
NICK_DROPEMAIL_PATTERN_TOO_LONG
	A megadott példa túl hosszú (maximum %d karakter).
NICK_DROPEMAIL_NONE
	Egy nicknévnek E-mail címe a megadott példában.
NICK_DROPEMAIL_COUNT
	%d nicknévnek van E-mail címe a megadott példából.
	Hogy dropold ezeket a nickneveket, írd be: /msg %s DROPEMAIL-CONFIRM %s
NICK_DROPEMAIL_CONFIRM_SYNTAX
	DROPEMAIL-CONFIRM cím
NICK_DROPEMAIL_CONFIRM_UNKNOWN
	A megadott példa nem felismerhetõ. Kérlek használd a DROPEMAIL
	parancsot újra ezzel a példával mielõtt használod a
	DROPEMAIL-CONFIRM parancsot.
NICK_DROPEMAIL_CONFIRM_DROPPING
	%d nicknév törlése...
NICK_DROPEMAIL_CONFIRM_DROPPED
	Nicknevek dropolása kész.

# SET válasz
NICK_SET_SYNTAX
	SET opció paraméterek
NICK_SET_OPER_SYNTAX
	SET [!nick] opció paraméterek
NICK_SET_DISABLED
	Sajnálom, a nicknevek opcióinak beállítása jelenleg szünetel.
NICK_SET_UNKNOWN_OPTION
	Ismeretlen SET opció: %s.

# SET PASSWORD válasz
NICK_SET_PASSWORD_FAILED
	Sajnálom, nem változtathatod meg a jelszavad.
NICK_SET_PASSWORD_CHANGED
	Jelszó megváltoztatva.
NICK_SET_PASSWORD_CHANGED_TO
	Jelszavad megváltoztattam (%s).

# SET LANGUAGE válasz
NICK_SET_LANGUAGE_SYNTAX
	SET LANGUAGE szám
NICK_SET_LANGUAGE_UNKNOWN
	Ismeretlen nyelvi azonosító: %d.  Írd be:
	/msg %s HELP SET LANGUAGE a nyelvek listájához.
NICK_SET_LANGUAGE_CHANGED
	A nyelv %s változott.

# SET URL válasz
NICK_SET_URL_CHANGED
	Az URL, amely a megadott nickhez (%s) tartozik, megváltozott:  %s.

# SET EMAIL válasz
NICK_SET_EMAIL_PLEASE_WAIT
	Kérlek várj %s mielõtt ismét használod a SET EMAIL parancsot.
NICK_SET_EMAIL_TOO_MANY_NICKS
	%s már regisztrált %d nicknevet (a maximum %d). Az E-mail
	cím megváltoztatása sikertelen.
NICK_SET_EMAIL_CHANGED
	%s E-mail címe megváltozott a következõre: %s.

# SET INFO válasz
NICK_SET_INFO_CHANGED
	%s informácija megváltozott.

# SET KILL válasz
NICK_SET_KILL_SYNTAX
	SET KILL {ON | QUICK | OFF}
NICK_SET_KILL_IMMED_SYNTAX
	SET KILL {ON | QUICK | IMMED | OFF}
NICK_SET_KILL_ON
	A Kill protection opció most már BEKAPCSOLVA.
NICK_SET_KILL_QUICK
	A Kill protection opció most már BEKAPCSOLVA, egy kis idõcsúszással killel.
NICK_SET_KILL_IMMED
	A Kill protection opció most már BEKAPCSOLVA, azonnal killel.
NICK_SET_KILL_IMMED_DISABLED
	Az IMMED opció nem elérhetõ ebben a hálozatban.
NICK_SET_KILL_OFF
	A Kill protection opció most már KIKAPCSOLVA.

# SET SECURE válasz
NICK_SET_SECURE_SYNTAX
	SET SECURE {ON | OFF}
NICK_SET_SECURE_ON
	A Secure opció most már BEKAPCSOLVA.
NICK_SET_SECURE_OFF
	A Secure opció most már KIKAPCSOLVA.

# SET PRIVATE válasz
NICK_SET_PRIVATE_SYNTAX
	SET PRIVATE {ON | OFF}
NICK_SET_PRIVATE_ON
	A Private opció most már BEKAPCSOLVA.
NICK_SET_PRIVATE_OFF
	A Private opció most már KIKAPCSOLVA.

# SET NOOP válasz
NICK_SET_NOOP_SYNTAX
	SET NOOP {ON | OFF}
NICK_SET_NOOP_ON
	No-op opció most már BEKAPCSOLVA.
NICK_SET_NOOP_OFF
	No-op opció most már KIKAPCSOLVA.

# SET HIDE kérelem
NICK_SET_HIDE_SYNTAX
	SET HIDE {EMAIL | USERMASK | QUIT} {ON | OFF}
NICK_SET_HIDE_EMAIL_ON
	Az E-mail címed most már el lesz rejtve az %s INFO-i között.
NICK_SET_HIDE_EMAIL_OFF
	Az E-mail címed most már szerepelni fog az %s INFO-i között.
NICK_SET_HIDE_MASK_ON
	Az utolsó user@host maszkod nem fog szerepelni %s INFO-i között.
NICK_SET_HIDE_MASK_OFF
	Az utolsó user@host maszkod szerepelni fog %s INFO-i között.
NICK_SET_HIDE_QUIT_ON
	Az utolsó kilépõ üzeneted nem fog szerepelni %s INFO-i között.
NICK_SET_HIDE_QUIT_OFF
	Az utolsó kilépõ üzeneted szerepelni fog %s INFO-i között .

# SET TIMEZONE válasz
NICK_SET_TIMEZONE_SYNTAX
	SET TIMEZONE {UTC-offset | time-zone | DEFAULT}
NICK_SET_TIMEZONE_TO
	Az idõzónád a következõre lett beállítva: UTC%c%d:%02d.
	A jelenlegi idõ ebben az idõzónában: %s.
NICK_SET_TIMEZONE_DEFAULT
	Az idõzónád vissza lett állítva a service idõzónájához.

# SET MAINNICK válasz
NICK_SET_MAINNICK_NOT_FOUND
	A megadott nick (%s) nem linkelt a tieddel.
NICK_SET_MAINNICK_CHANGED
	A fõ nicked a következõre változott %s.

# SET NOEXPIRE válasz
NICK_SET_NOEXPIRE_SYNTAX
	SET [!nicknév] NOEXPIRE {ON | OFF}
NICK_SET_NOEXPIRE_ON
	A megadott nick (%s) nem fog elévülni.
NICK_SET_NOEXPIRE_OFF
	A megadott nick (%s) el fog elévülni.

# UNSET válasz
NICK_UNSET_SYNTAX
	UNSET {URL | EMAIL | INFO}
NICK_UNSET_SYNTAX_REQ_EMAIL
	UNSET {URL | INFO}
NICK_UNSET_OPER_SYNTAX
	UNSET [!nick] {URL | EMAIL | INFO}
NICK_UNSET_OPER_SYNTAX_REQ_EMAIL
	UNSET [!nick] {URL | INFO}
NICK_UNSET_URL
	%s URL címe vissza lett állítva.
NICK_UNSET_EMAIL
	%s E-mail címe vissza lett állítva.
NICK_UNSET_INFO
	%s információja vissza lett állítva..
NICK_UNSET_EMAIL_BAD
	Te nem törölheted az E-mail címed.
NICK_UNSET_EMAIL_OTHER_BAD
	Ennek a nicknévnek az E-mail címeit nem lehet törölni.

# ACCESS válasz
NICK_ACCESS_SYNTAX
	ACCESS {ADD | DEL | LIST} [mask]
NICK_ACCESS_DISABLED
	Sajnálom, a nicknév hozzáférési lista módosítása jelenleg nem elérhetõ
NICK_ACCESS_ALREADY_PRESENT
	A megadott maszk (%s) már szerepel a hozzáférési listádon.
NICK_ACCESS_REACHED_LIMIT
	Sajnálom, sajnálom neked csak %d hozzáférési helyed van ehhez a nickhez.
NICK_ACCESS_NO_NICKS
	Figyelmeztetés: A nicknév hozzáférési maszk nem tartalmazhat
	nickneveket. Kérlek ellenõrizd, és bizonyosodj meg róla, hogy a
	user@host maszkot helyesen adtad-e meg.
NICK_ACCESS_ADDED
	A megadott nick (%s) neve hozzáadva a hozzáférési listádhoz.
NICK_ACCESS_NOT_FOUND
	A megadott nick (%s) neve nem szerepel a hozzáférési listádon.
NICK_ACCESS_DELETED
	A megadott nick (%s) neve törölve a hozzáférési listádról.
NICK_ACCESS_LIST_EMPTY
	A te hozzáférési listád üres.
NICK_ACCESS_LIST
	Hozzáférési lista:
NICK_ACCESS_LIST_X_EMPTY
	%s hozzáférési listája üres.
NICK_ACCESS_LIST_X
	%s hozzáférési listája:

# LINK válasz
NICK_OLD_LINK_SYNTAX
	LINK nick jelszó
NICK_LINK_SYNTAX
	LINK nick
NICK_LINK_DISABLED
	Sajnálom, a nicknév linkelés jelenleg szünetel.
NICK_LINK_FAILED
	Sajnálom, a neved linkelése sikertelen.
NICK_CANNOT_BE_LINKED
	A %s nicknév nem linkelhetõ.
NICK_OLD_LINK_SAME
	Nem tudsz linkelõdni a saját nickedhez!
NICK_LINK_SAME
	Nem tudsz linkelõdni a jelenlegi nickedhez!
NICK_LINK_ALREADY_LINKED
	A következõ nick: %s már linkelve van a nevedhez.
NICK_LINK_IN_USE
	Ez a nick: %s jelenleg is használatban van.
NICK_LINK_TOO_MANY
	Neked nem lehet több, mint %d neved össze linkelve.
NICK_SET_EMAIL_UNAUTHED
	Az E-mail cím amit megadtál még nincs hitelesítve. Kérlek használd
	az AUTH parancsot, hogy hitelesítsd a címet elõször.
NICK_LINK_TOO_MANY_NICKS
	Neked már túl sok  %d nickneved van regisztrálva (maximum %d).
	Linkelés sikertelen.
NICK_OLD_LINK_TOO_MANY_CHANNELS
	A megadott nickhez (%s) nem linkelhetsz: túl sok csatorna
	van regisztrálva (kombinált maximum %d).
NICK_OLD_LINKED
	A nicked linkelõdött ehhez a nickhez: %s.
NICK_LINKED
	A megadott nick: (%s) linkelõdött a Te nickedhez.

# UNLINK válasz
NICK_OLD_UNLINK_SYNTAX
	UNLINK [nick jelszó]
NICK_UNLINK_SYNTAX
	UNLINK nick
NICK_UNLINK_OPER_SYNTAX
	UNLINK nick [FORCE]
NICK_UNLINK_DISABLED
	Sajnálom, az nicknév unlinkelés opció jelenleg szünetel.
NICK_UNLINK_FAILED
	Sajnálom, a linkelés törlése sikertelen.
NICK_UNLINK_SAME
	Nem tudod a jelenlegi nicked linkelését törölni!
NICK_OLD_UNLINK_NOT_LINKED
	A te nicked nincs linkelve másikhoz.
NICK_UNLINK_NOT_LINKED_YOURS
	A megadott nick (%s) nem linkelõdött a tiedhez.
NICK_UNLINK_NOT_LINKED
	A megadott nick (%s) nem linkelt.
NICK_OLD_UNLINKED
	A nicked linkelõdése törlödött a következõ nicktõl: %s.
NICK_UNLINKED
	%s többé nincs linkvelve a Tieddel.
NICK_X_UNLINKED
	%s többé nincs linkelve a megadott nickkel (%s).

# LISTLINKS válasz
NICK_OLD_LISTLINKS_SYNTAX
	LISTLINKS nick
NICK_LISTLINKS_SYNTAX
	LISTLINKS
NICK_LISTLINKS_HEADER
	%s linkelt nickjeinek listája:
NICK_LISTLINKS_FOOTER
	%d linkelt nicknév.

# INFO válasz
NICK_INFO_SYNTAX
	INFO nick [ALL]
NICK_INFO_REALNAME
	%s is %s
NICK_INFO_ADDRESS
	Utoljára látott cím: %s
NICK_INFO_ADDRESS_ONLINE
	Jelenleg aktív errõl a címrõl: %s
NICK_INFO_ADDRESS_ONLINE_NOHOST
	%s jelenleg aktív.
NICK_INFO_ADDRESS_OTHER_NICK
	A következõ nevet használja: %s
NICK_INFO_TIME_REGGED
	Regisztrálás ideje: %s
NICK_INFO_LAST_SEEN
	Utoljára ekkor volt: %s
NICK_INFO_LAST_QUIT
	Utolsó kilépõ üzenet: %s
NICK_INFO_URL
	URL: %s
NICK_INFO_EMAIL
	E-mail címek: %s
NICK_INFO_EMAIL_UNAUTHED
	E-mail címek: %s (igazolatlan)
NICK_INFO_INFO
	Informació: %s
NICK_INFO_OPTIONS
	Opciók: %s
# These strings MUST NOT be empty
NICK_INFO_OPT_KILL
	Kill protection
NICK_INFO_OPT_SECURE
	Security
NICK_INFO_OPT_PRIVATE
	Private
NICK_INFO_OPT_NOOP
	No-op
NICK_INFO_OPT_NONE
	Nincs
NICK_INFO_NO_EXPIRE
	Ez a nicknév nem fog elévülni.
NICK_INFO_SUSPEND_DETAILS
	Felfüggesztve %s által  %s (%s).
NICK_INFO_SUSPEND_REASON
	Indok: %s
NICK_INFO_SHOW_ALL
	További információért, írd be: /msg %s INFO %s ALL

# LISTCHANS válasz
NICK_LISTCHANS_SYNTAX
	LISTCHANS
NICK_LISTCHANS_NONE
	%s nem regisztrált egy szobát se.
NICK_LISTCHANS_HEADER
	%s regisztrált szobáinak listája:
NICK_LISTCHANS_END
	A lista vége (%d csatorna regisztrált).

# LIST/LISTEMAIL válasz
NICK_LIST_SYNTAX
	%s [+start] sablon
NICK_LIST_OPER_SYNTAX
	%s [+start] sablon [FORBIDDEN] [NOEXPIRE] [SUSPENDED]
NICK_LIST_OPER_SYNTAX_AUTH
	%s [+start] sablon [FORBIDDEN] [NOEXPIRE] [SUSPENDED] [NOAUTH]
NICK_LIST_HEADER
	A listán szereplõk száma: %s:
NICK_LIST_NO_MATCH
	Nem talált egyezõ nicknevet.
NICK_LISTEMAIL_NONE
	(nincs beállítva)

# RECOVER válasz
NICK_RECOVER_SYNTAX
	RECOVER nicknév [jelszó]
NICK_NO_RECOVER_SELF
	Nem tudod visszafoglalni a nicked!
NICK_RECOVERED
	A felhasználó követelésére a nicknév linkelése megszûnt.  Írd be:
	/msg %s RELEASE %s jelszó hogy visszakapd, mielõtt az egy perc letelik.

# RELEASE válasz
NICK_RELEASE_SYNTAX
	RELEASE nicknév [jelszó]
NICK_RELEASE_NOT_HELD
	Ez a nick: %s nem lett held-elve.
NICK_RELEASED
	Service tartja a te nicked felszabaditását.

# GHOST válasz
NICK_GHOST_SYNTAX
	GHOST nicknév [jelszó]
NICK_NO_GHOST_SELF
	Nem ghost-olhatod ki saját magad!
NICK_GHOST_KILLED
	Ghost, a nicked ki lett kill-elve.

# GETPASS válasz
NICK_GETPASS_SYNTAX
	GETPASS nicknév
NICK_GETPASS_UNAVAILABLE
	GETPASS parancs használhatatlan, mert az encryption használatban van.
NICK_GETPASS_PASSWORD_IS
	%s jelszava: %s.

# FORBID válasz
NICK_FORBID_SYNTAX
	FORBID nicknév
NICK_FORBID_SUCCEEDED
	A megadott nick (%s) ezentúl nem használható.
NICK_FORBID_FAILED
	A megadott nick (%s) használatát nem tilthatod le!

# SUSPEND válasz
NICK_SUSPEND_SYNTAX
	SUSPEND [+elévülés] nicknév indok
NICK_SUSPEND_SUCCEEDED
	%s és az összes hozzá linkelt nick használata fel lett függesztve.
NICK_SUSPEND_ALREADY_SUSPENDED
	A megadott nick (%s) használata fel lett függesztve.

# UNSUSPEND válasz
NICK_UNSUSPEND_SYNTAX
	UNSUSPEND nicknév
NICK_UNSUSPEND_SUCCEEDED
	A megadott nick (%s) felfüggesztése meg lett szüntetve.
NICK_UNSUSPEND_NOT_SUSPENDED
	A megadott nick (%s) nincs felfüggesztve.

################ autojoin module üzenetek/válaszok

# AJOIN válasz
NICK_AJOIN_SYNTAX
	AJOIN {ADD | DEL | LIST} [channel]
NICK_AJOIN_ADD_SYNTAX
	AJOIN ADD csatorna
NICK_AJOIN_DEL_SYNTAX
	AJOIN DEL csatorna
NICK_AJOIN_DISABLED
	Sajnálom, a nicknév autojoin listájának módosítása jelenleg nem elérhetõ.
NICK_AJOIN_ALREADY_PRESENT
	A megadott csatorna (%s) mostmár szerepel a te autojoin listádban.
NICK_AJOIN_LIST_FULL
	Sajnos, neked csak %d autojoin helyed van a nicknevedhez.
NICK_AJOIN_ADDED
	%s hozzáadva az autojoin listádhoz.
NICK_AJOIN_NOT_FOUND
	%s nem szerepel az autojoin listádon.
NICK_AJOIN_DELETED
	%s törölve az autojoin listádról.
NICK_AJOIN_LIST_EMPTY
	Az autojoin listád üres.
NICK_AJOIN_LIST
	Autojoin lista:
NICK_AJOIN_LIST_X_EMPTY
	%s autojoin listája üres.
NICK_AJOIN_LIST_X
	%s autojoin listája:
NICK_AJOIN_AUTO_REMOVE
	%s csatorna eltávolítva az autojoin listájádról,
	mert nem érvényes csatornanév.

################ mail-auth module üzenetek/válaszok

# Általános célú üzenetek
NICK_AUTH_SENDING
	A nevedhez egy érvényesítõ kód lett elküldve erre a címre: %s.
NICK_AUTH_FOR_REGISTER
	Amikor megkapod ezt az üzentet, írd be: /msg %s AUTH kód
	(helyettesítsd a kódot az érvényesitõ kódoddal) hogy befejezd a
	nickneved regisztracióját.
NICK_AUTH_FOR_SET_EMAIL
	Amikor megkapod ezt az üzenetet, írd be: /msg %s AUTH kód
	(helyettesítsd a kódot az érvényesítõ kódoddal) hogy befejezd
	az E-mail címed megváltoztatását.
	kérlek jegyezd meg, hogy nem fogod tudni folytatni a nickneved
	használatát amíg nem teljesíted a változtatást.
NICK_AUTH_NOT_NEEDED
	Jelenleg nincs szükség élesítésre.
NICK_AUTH_NO_AUTHCODE
	A megadott nickhez (%s) nincs élesítési kód.
NICK_AUTH_HAS_AUTHCODE
	A megadott nickhez (%s) már tartozik egy érvényesítõ kód.
PASSWORD_WARNING_FOR_AUTH
	Figyelmeztetés: Egy vagy több helytelen érvényesítõ kód
	beírása után a megszakad a kapcsolatod.

# Levél szövege. Az üzenettörzsben az utolsó "%s" (user@host elõtt)
# ki lesz cserélve a  NICK_AUTH_MAIL_TEXT_* üzenetek egyikével.
NICK_AUTH_MAIL_SUBJECT
	Az érvényesítõ kód ehhez a nickhez: %s
NICK_AUTH_MAIL_BODY
	Az érvényesítõ kód a te nickedhez: (%s) a következõ %09d
	Kérlek hitelesítsd a nicked %s a következõ paranccsal:
	/msg %s AUTH %09d
	
	Ezt az üzenetet a következõ küldte: %s a következõ okból: %s
	%s@%s.
NICK_AUTH_MAIL_TEXT_REG
	Regisztrálva a következõ által:
NICK_AUTH_MAIL_TEXT_EMAIL
	Az E-mail cím meg változtatva a következõ által:
NICK_AUTH_MAIL_TEXT_SENDAUTH
	A SENDAUTH parancs kiadva a következõ által:
NICK_AUTH_MAIL_TEXT_REAUTH
	a REAUTH parancstól
NICK_AUTH_MAIL_BODY_SETAUTH
	A hitelesítõ kód nicknevedhez: (%s) a következõ: %09d
	Kérlek használd a következõ kódot %s ezzel a paranccsal:
	/msg %s AUTH %09d

# AUTH válasz
NICK_AUTH_SYNTAX
	AUTH kód
NICK_AUTH_DISABLED
	Sajnálom, a nicknév érvényesítés jelenleg szünetel.
NICK_AUTH_FAILED
	A megadott érvényesítõ kód helytelen.
NICK_AUTH_SUCCEEDED_REGISTER
	Érvényesítés sikeresen megtörtént; a nickneved regisztrációja most már kész.
NICK_AUTH_SUCCEEDED_SET_EMAIL
	Érvényesítés sikeresen megtörtént; az E-mail cím változtatás kész.
NICK_AUTH_SUCCEEDED_REAUTH
	Érvényesítés sikeresen megtörtént; most már használhatod a nickneved.
NICK_AUTH_SUCCEEDED_SETAUTH
	Érvényesítés sikeresen megtörtént; most már használhatod a nickneved.

# SENDAUTH válasz
NICK_SENDAUTH_SYNTAX
	SENDAUTH
NICK_SENDAUTH_TOO_SOON
	Kérlek várj %s leteltéig, mielõtt újra használod a SENDAUTH parancsot.
NICK_SENDAUTH_NORESOURCES
	Az érvényesítõ kódodat tartalmazó levelet nem lehet
	elküldeni, mert a rendszer túl elfoglalt. Kérlek próbáld késõbb,
	vagy lépj kapcsolatba egy adminisztrátorral.
NICK_SENDAUTH_ERROR
	Az érvényesítõ kódodat tartalmazó levelet nem sikerült elküldeni.
	Kérlek próbáld késobb, vagy lépj kapcsolatba egy adminisztrátorral.

# REAUTH válasz
NICK_REAUTH_SYNTAX
	REAUTH
NICK_REAUTH_HAVE_AUTHCODE
	Az élesítõ kód már be van állítva a nicknevedhez.
NICK_REAUTH_NO_EMAIL
	A nicknevedhez nincs megadva E-mail cím.
NICK_REAUTH_AUTHCODE_SET
	Az új érvényesítõ kódot elküldtük a címedre, hamarosan megérkezik.
	Használd az AUTH paranccsal, hogy beazonosíts a nicknevedre.

# RESTOREMAIL responses
NICK_RESTOREMAIL_SYNTAX
	RESTOREMAIL jelszó
NICK_RESTOREMAIL_NOT_NOW
	RESTOREMAIL csak azután használható, hogy lecserélted az E-mail címed,
	és mielõtt érvényesítetted volna az új címed.
NICK_RESTOREMAIL_DONE
	Az elõzõ E-mail címed %s vissza lett állítva.

# SETAUTH válasz
NICK_SETAUTH_SYNTAX
	SETAUTH nick
NICK_SETAUTH_NO_EMAIL
	A megadott nickhez (%s) nincs E-mail cím beállítva.
NICK_SETAUTH_AUTHCODE_SET
	Az új érvényesítési kód (%09d) elküldtem a következõ nicknek: %s.
NICK_SETAUTH_SEND_REFUSED
	Figyelmeztetés: sikertelen üzenetküldés a következo nickhez: %s
	(üzenet visszautasítva a mail szerver által).
NICK_SETAUTH_SEND_TIMEOUT
	Figyelmeztetés: sikertelen üzenetküldés a következo nickhez:
	%s (kifagyott vagy nem lehet csatlakozni a mail szerverhez).
NICK_SETAUTH_SEND_NORESOURCES
	Figyelmeztetés: sikertelen üzenetküldés a következõ nickhez:
	%s (hibás forrás).
NICK_SETAUTH_SEND_ERROR
	Figyelmeztetés: sikertelen üzenetküldés a következo nickhez:
	%s (ismeretlen hiba).
NICK_SETAUTH_USER_NOTICE
	A nickneved regisztrációját élesítened kell, mielõtt használni
	szeretnéd. A hitelesítési kód el lett küldve a következõ E-mail
	címre: %s
	Írd be: /msg %s HELP AUTH további informaciókért.

# GETAUTH válasz
NICK_GETAUTH_SYNTAX
	GETAUTH nicknév
NICK_GETAUTH_AUTHCODE_IS
	%s hitelesítési kódja: %09d

# CLEARAUTH válasz
NICK_CLEARAUTH_SYNTAX
	CLEARAUTH nicknév
NICK_CLEARAUTH_CLEARED
	%s hitelesítési kódját töröltem.

###########################################################################
#
# ChanServ üzenetek
#
###########################################################################

# Access level meghatározások

# Access level descriptions
CHAN_LEVEL_AUTOOP
	Automatikus operátor (+o) státusz
CHAN_LEVEL_AUTOVOICE
	Automatikus voice (+v) státusz (hangadás)
CHAN_LEVEL_INVITE
	Engedélyezi az INVITE parancsok használatát
CHAN_LEVEL_AKICK
	Engedélyezi az AKICK parancsok használatát
CHAN_LEVEL_SET
	Engedélyezi a SET parancsok használatát
	(kivéve a FOUNDER/PASSWORD parancsokat)
CHAN_LEVEL_CLEAR
	Engedélyezi a CLEAR parancsok használatát
CHAN_LEVEL_UNBAN
	Engedélyezi az UNBAN parancs használatát
CHAN_LEVEL_OPDEOP
	Engedélyezi az OP/DEOP parancs használatát
CHAN_LEVEL_ACCESS_LIST
	Engedélyezi az access lista megtekintését
CHAN_LEVEL_ACCESS_CHANGE
	Engedélyezi az access lista módosítását
CHAN_LEVEL_MEMO
	Csatorna üzeneteket fogadhat
CHAN_LEVEL_VOICE
	Engedélyezi a VOICE/DEVOICE parancsok használatát
CHAN_LEVEL_AUTOHALFOP
	Automatikus halfop (+h) státus (félop)
CHAN_LEVEL_HALFOP
	Engedélyezi a HALFOP/DEHALFOP parancsok használatát
CHAN_LEVEL_AUTOPROTECT
	Automatikus  protect (+a) státus (védett felhasználó [sop])
CHAN_LEVEL_PROTECT
	Engedélyezi a PROTECT/DEPROTECT parancsok használatát
CHAN_LEVEL_KICK
	Engedélyezi a KICK parancs használatát
CHAN_LEVEL_STATUS
	Engedélyezi a STATUS parancs használatát
CHAN_LEVEL_TOPIC
	Engedélyezi a TOPIC parancs használatát

# Automatikus válaszok
CHAN_IS_REGISTERED
	Ez a csatorna regisztrálva van %s-vel.
CHAN_MAY_NOT_BE_USED
	Ez a csatorna nem használható.
CHAN_NOT_ALLOWED_TO_JOIN
	Nem tartózkodhatsz a csatornán.

# Multi-szándékos üzenetek/válaszok
CHAN_MUST_IDENTIFY_NICK
	Kérlek azonosíts %s elõször, használd a következõ parancsot:
	/msg %s IDENTIFY jelszó
CHAN_BOUNCY_MODES
	Sajnálom, a %s parancs jelenleg elérhetetlen.

# REGISTER válaszok
CHAN_REGISTER_SYNTAX
	REGISTER csatorna jelszó meghatározás
CHAN_REGISTER_DISABLED
	Sajnálom, a csatonra regisztració jelenleg szünetel.
CHAN_REGISTER_SHORT_CHANNEL
	A # csatorna nem regisztrálható.
CHAN_REGISTER_NOT_LOCAL
	Local csatornák nem regisztrálhatóak.
CHAN_REGISTER_INVALID_NAME
	Csak olyan szobákat regisztrálhatsz amelyek neve a következõképpen
	kezdõdik: #.
CHAN_MUST_REGISTER_NICK
	Elõször regisztrálnod kell a neved. Írd be: /msg %s HELP a
	névregisztrációval kapcsolatos információkért.
CHAN_MAY_NOT_BE_REGISTERED
	A megadott csatorna (%s) nem regisztrálható.
CHAN_ALREADY_REGISTERED
	A megadott csatorna (%s) már regisztrált!
CHAN_MUST_BE_CHANOP
	A csatornán operátornak kell lenned, hogy regisztrálhasd azt.
CHAN_REACHED_CHANNEL_LIMIT
	Sajnálom, elérted a felsõ korlátot (%d) csatornák számát tekintve.
CHAN_EXCEEDED_CHANNEL_LIMIT
	Sajnálom, már túllépted a felsõ korlátot (%d) a csatornán.
CHAN_REGISTRATION_FAILED
	Sajnálom, regisztráció sikertelen.
CHAN_REGISTERED
	Sikeresen regisztráltam a megadott csatornát (%s) a nickedre (%s).
CHAN_PASSWORD_IS
	A csatorna jelszava: %s -- Emlékezz erre a késõbbi használathoz.

# IDENTIFY válaszok
CHAN_IDENTIFY_SYNTAX
	IDENTIFY csatorna jelszó
CHAN_IDENTIFY_FAILED
	Sajnálom, azonosítás sikertelen.
CHAN_IDENTIFY_SUCCEEDED
	Jelszavad elfogadva -- founder-szintû a hozzáférésed van a megadott
	csatornához (%s).

# DROP válaszok
CHAN_DROP_SYNTAX
	DROP csatorna jelszó
CHAN_DROP_DISABLED
	Sajnálom, a csatornák regisztrációjának törlése jelenleg szünetel.
CHAN_DROPPED
	A megadott csatorna (%s) regisztrációja törlése sikerült.

# DROPCHAN válaszok
CHAN_DROPCHAN_SYNTAX
	DROPCHAN csatorna

# SET válaszok
CHAN_SET_SYNTAX
	SET csatorna opció paraméterek
CHAN_SET_DISABLED
	Sajnálom, a csatornák opcióinak beállitása jelenleg szünetel.
CHAN_SET_UNKNOWN_OPTION
	Ismeretlen SET opció %s.

# SET FOUNDER válaszok
CHAN_SET_FOUNDER_TOO_MANY_CHANS
	%s részére túl sok csatorna van regisztrálva.
CHAN_FOUNDER_CHANGED
	A megadott csatorna (%s) új foundere: %s.

# SET SUCCESSOR válaszok
CHAN_SUCCESSOR_CHANGED
	A megadott csatorna (%s) új successora: %s.
CHAN_SUCCESSOR_IS_FOUNDER
	A csatorna foundere és successora nem lehet azonos személy.

# SET PASSWORD válaszok
CHAN_SET_PASSWORD_FAILED
	Sajnálom, a jelszó beállítása sikertelen.
CHAN_PASSWORD_CHANGED
	Az új jelszó: %s.
CHAN_PASSWORD_CHANGED_TO
	%s új jelszava: %s.

# SET DESC válasz
CHAN_DESC_CHANGED
	A megadott csatorna (%s) új leírása: %s.

# SET URL válasz
CHAN_URL_CHANGED
	A megadott csatorna (%s) új URL címe: %s.

# SET EMAIL válasz
CHAN_EMAIL_CHANGED
	A megadott csatorna (%s) új E-mail címe: %s.

# SET ENTRYMSG válasz
CHAN_ENTRY_MSG_CHANGED
	%s csatorna belépõ üzenete megváltozott.

# SET MLOCK válaszok
CHAN_SET_MLOCK_NEED_PLUS_MINUS
	A lockolni kívánt módokat tartalmazó lista + (plus) vagy - (minus)
	jelekkel kell, hogy kezdõdjön.
CHAN_SET_MLOCK_NEED_PARAM
	A megadott flaghez (+%c) paraméter szükséges.
CHAN_SET_MLOCK_NEED_POSITIVE
	A paraméter az MLOCK +%c esetében pozitív szám kell, hogy legyen.
CHAN_SET_MLOCK_MODE_REG_BAD
	A megadott flag (+%c) mindig be lesz állítva a regisztrált csatornán,
	és nem használható az MLOCK paranccsal.
CHAN_SET_MLOCK_LINK_BAD
	A megadott flaghez (+%c) csatornanév szükséges.
CHAN_SET_MLOCK_LINK_SAME
	Az MLOCK +%c paraméternél egy másik szobát kell megadni.
CHAN_SET_MLOCK_REQUIRES
	A megadott flag (+%c) jó beállításához +%c flag is szükséges.
CHAN_SET_MLOCK_BAD_PARAM
	A megdott flag (+%c) érvénytelen.
CHAN_SET_MLOCK_UNKNOWN_CHAR
	%c: ismeretlen karakter, elutasitva.
CHAN_SET_MLOCK_CANNOT_LOCK
	%c mód nem lockolható.
CHAN_MLOCK_CHANGED
	A megadott csatornán (%s) az új Mode lock: %s.
CHAN_MLOCK_REMOVED
	A megadott csatornán (%s) már nincs Mode lock.

# SET HIDE válaszok
CHAN_SET_HIDE_SYNTAX
	SET csatorna HIDE {EMAIL | TOPIC | MLOCK} {ON | OFF}
CHAN_SET_HIDE_EMAIL_ON
	A megadott csatorna (%s) E-mail címe nem fog szerepelni a %s INFO-i közt .
CHAN_SET_HIDE_EMAIL_OFF
	A megadott csatorna (%s) E-mail címe szerepelni fog a %s INFO-i közt.
CHAN_SET_HIDE_TOPIC_ON
	A megadott csatorna (%s) témája nem fog szerepelni a %s INFO-i közt.
CHAN_SET_HIDE_TOPIC_OFF
	A megadott csatorna (%s) témája szerepelni fog a %s INFO-i közt.
CHAN_SET_HIDE_MLOCK_ON
	A megadott csatorna (%s) mode lockja nem fog szerepelni a %s INFO-i közt.
CHAN_SET_HIDE_MLOCK_OFF
	A megadott csatorna (%s) mode lockja szerepelni fog a %s INFO-i közt.

# SET KEEPTOPIC válaszok
CHAN_SET_KEEPTOPIC_SYNTAX
	SET csatorna KEEPTOPIC {ON | OFF}
CHAN_SET_KEEPTOPIC_ON
	Témamegõrzés BEKAPCSOLVA.
CHAN_SET_KEEPTOPIC_OFF
	Témamegõrzés KIKAPCSOLVA.

# SET TOPICLOCK válaszok
CHAN_SET_TOPICLOCK_SYNTAX
	SET csatorna TOPICLOCK {ON | OFF}
CHAN_SET_TOPICLOCK_ON
	Témalezárás BEKAPCSOLVA.
CHAN_SET_TOPICLOCK_OFF
	Témalezárás KIKAPCSOLVA.

# SET PRIVATE válaszok
CHAN_SET_PRIVATE_SYNTAX
	SET csatorna PRIVATE {ON | OFF}
CHAN_SET_PRIVATE_ON
	Private opció BEKAPCSOLVA.
CHAN_SET_PRIVATE_OFF
	Private opció BEKAPCSOLVA.

# SET SECUREOPS válaszok
CHAN_SET_SECUREOPS_SYNTAX
	SET csatorna SECUREOPS {ON | OFF}
CHAN_SET_SECUREOPS_ON
	Secure ops opció BEKAPCSOLVA.
CHAN_SET_SECUREOPS_OFF
	Secure ops opció KIKAPCSOLVA.

# SET LEAVEOPS válaszok
CHAN_SET_LEAVEOPS_SYNTAX
	SET csatorna LEAVEOPS {ON | OFF}
CHAN_SET_LEAVEOPS_ON
	Leave ops opció BEKAPCSOLVA.
CHAN_SET_LEAVEOPS_OFF
	Leave ops opció KIKAPCSOLVA.

# SET RESTRICTED válaszok
CHAN_SET_RESTRICTED_SYNTAX
	SET csatorna RESTRICTED {ON | OFF}
CHAN_SET_RESTRICTED_ON
	Restricted hozzáférés opció BEKAPCSOLVA.
CHAN_SET_RESTRICTED_OFF
	Restricted hozzáférés opció KIKAPCSOLVA.

# SET SECURE válaszok
CHAN_SET_SECURE_SYNTAX
	SET csatorna SECURE {ON | OFF}
CHAN_SET_SECURE_ON
	Secure opció BEKAPCSOLVA.
CHAN_SET_SECURE_OFF
	Secure opció KIKAPCSOLVA.

# SET OPNOTICE válaszok
CHAN_SET_OPNOTICE_SYNTAX
	SET csatorna OPNOTICE {ON | OFF}
CHAN_SET_OPNOTICE_ON
	Op-notice opció BEKAPCSOLVA.
CHAN_SET_OPNOTICE_OFF
	Op-notice opció KIKAPCSOLVA.

# SET ENFORCE válaszok
CHAN_SET_ENFORCE_SYNTAX
	SET csatornal ENFORCE {ON | OFF}
CHAN_SET_ENFORCE_ON
	Enforce opció BEKAPCSOLVA.
CHAN_SET_ENFORCE_OFF
	Enforce opció KIKAPCSOLVA.

# SET MEMO-RESTRICTED responses
CHAN_SET_MEMO_RESTRICTED_SYNTAX
	SET csatorna MEMO-RESTRICTED {ON | OFF}
CHAN_SET_MEMO_RESTRICTED_ON
	Korlátozott memo opció most már BEKAPCSOLVA.
CHAN_SET_MEMO_RESTRICTED_OFF
	Korlátozott memo opció most már KIKAPCSOLVA.

# SET NOEXPIRE válaszok
CHAN_SET_NOEXPIRE_SYNTAX
	SET csatorna NOEXPIRE {ON | OFF}
CHAN_SET_NOEXPIRE_ON
	A megadott csatorna (%s) nem fog elévülni.
CHAN_SET_NOEXPIRE_OFF
	A megadott csatorna (%s) el fog évülni.

# UNSET válaszok
CHAN_UNSET_SYNTAX
	UNSET csatorna {SUCCESSOR | URL | EMAIL | ENTRYMSG}
CHAN_SUCCESSOR_UNSET
	A megadott csatorna (%s) successora törölve.
CHAN_URL_UNSET
	A megadott csatorna (%s) URL címe törölve.
CHAN_EMAIL_UNSET
	A megadott csatorna (%s) E-mail címe törölve.
CHAN_ENTRY_MSG_UNSET
	A megadott csatorna (%s) belépõ üzenete törölve.

# ACCESS üzenetek
CHAN_ACCESS_SYNTAX
	ACCESS csatorna {ADD | DEL | LIST | LISTLEVEL | COUNT} [paraméterek]
CHAN_ACCESS_ADD_SYNTAX
	ACCESS csatorna ADD nicknév szint
CHAN_ACCESS_DEL_SYNTAX
	ACCESS csatorna DEL nicknév
CHAN_ACCESS_LIST_SYNTAX
	ACCESS csatorna LIST [+start] [maszk]
CHAN_ACCESS_LISTLEVEL_SYNTAX
	ACCESS csatorna LISTLEVEL [+start] szint
CHAN_ACCESS_COUNT_SYNTAX
	ACCESS channel COUNT
CHAN_ACCESS_DISABLED
	Sajnálom, a csatorna hozzáférési listájának módosítása átmenetileg szünetel.
CHAN_ACCESS_LEVEL_NONZERO
	A hozzáférési szint nem lehet nulla.
CHAN_ACCESS_LEVEL_RANGE
	A hozzáférési szintnek %d és %d között kell lennie.
CHAN_ACCESS_NICKS_ONLY
	A hozzáférési szintjeinek listája csak regisztrált nickneveket tartalmazhat.
CHAN_ACCESS_NOOP
	%s nem adható hozzá a csatorna access listájához.
CHAN_ACCESS_REACHED_LIMIT
	Sajnálom, neked csak %d számú helyed van a csatorna hozzáférési listáján.
CHAN_ACCESS_LEVEL_UNCHANGED
	%s hozzáférési szintje a megadott szobán (%s) visszaállt errõl a szintrõl: %d.
CHAN_ACCESS_LEVEL_CHANGED
	%s hozzáférési szintje a következõ szobához: %s megváltozott: %d.
CHAN_ACCESS_ADDED
	%s hozzáadva a %s csatorna hozzáférési listájához a következõ szinttel: %d.
CHAN_ACCESS_NOT_FOUND
	%s nem szerepel a megadott csatorna (%s) hozzáférési listáján.
CHAN_ACCESS_DELETED
	%s törölve a megadott csatorna (%s) hozzáférési listájáról.
CHAN_ACCESS_LIST_EMPTY
	%s hozzáférési listája üres.
CHAN_ACCESS_NO_MATCH
	Nincs találat a megadott csatorna (%s) hozzáférési listáján.
CHAN_ACCESS_LIST_HEADER
	%s hozzáférési listája:
	Szint  Nick
CHAN_ACCESS_LIST_FORMAT
	%4d  %s
CHAN_ACCESS_COUNT
	%s hozzáférési listáján %d bejegyzés van.

# XOP (SOP/AOP/etc.) válaszok
CHAN_SOP_SYNTAX
	SOP csatorna {ADD | DEL | LIST | COUNT} [paraméterek]
CHAN_AOP_SYNTAX
	AOP csatorna {ADD | DEL | LIST | COUNT} [paraméterek]
CHAN_HOP_SYNTAX
	HOP csatorna {ADD | DEL | LIST | COUNT} [paraméterek]
CHAN_VOP_SYNTAX
	VOP csatorna {ADD | DEL | LIST | COUNT} [paraméterek]
CHAN_NOP_SYNTAX
	NOP csatorna {ADD | DEL | LIST | COUNT} [paraméterek]
CHAN_SOP_LIST_SYNTAX
	SOP csatorna {ADD | DEL | LIST | COUNT} [+start] [maszk]
CHAN_AOP_LIST_SYNTAX
	AOP csatorna {ADD | DEL | LIST | COUNT} [+start] [maszk]
CHAN_HOP_LIST_SYNTAX
	HOP csatorna {ADD | DEL | LIST | COUNT} [+start] [maszk]
CHAN_VOP_LIST_SYNTAX
	VOP csatorna {ADD | DEL | LIST | COUNT} [+start] [maszk]
CHAN_NOP_LIST_SYNTAX
	NOP csatorna {ADD | DEL | LIST | COUNT} [+start] [maszk]
CHAN_XOP_ADDED
	%s hozzáadva %s %s listához.
CHAN_XOP_LEVEL_CHANGED
	%s áthelyezve a %s %s listához.
CHAN_XOP_LEVEL_UNCHANGED
	%s már szerepel a %s %s listán.
CHAN_XOP_NICKS_ONLY
	SOP, AOP, VOP, és NOP listák csak regisztrált neveket tartalmazhatnak.
CHAN_XOP_NICKS_ONLY_HOP
	SOP, AOP, HOP, VOP, és NOP listák csak regisztrált neveket tartalmazhatnak.
CHAN_XOP_NOOP
	%s nem adható hozzá a csatorna access listájához.
CHAN_XOP_REACHED_LIMIT
	Sajnálom, az SOP, AOP, VOP, és NOP listán lévõ bejegyzések száma
	nem lehet több, mint %d. Határérték elérve.
CHAN_XOP_REACHED_LIMIT_HOP
	Sajnálom, az SOP, AOP, HOP, VOP, és NOP listán lévõ bejegyzések
	száma nem lehet több, mint %d. Határérték elérve.
CHAN_XOP_NOT_FOUND
	%s nem található a %s %s listán.
CHAN_XOP_DELETED
	%s törölve %s %s listáról.
CHAN_XOP_LIST_EMPTY
	%s %s lista üres.
CHAN_XOP_NO_MATCH
	Nincs találat a %s %s listán.
CHAN_XOP_LIST_HEADER
	%s lista a csatornához: %s:
CHAN_XOP_COUNT
	A %s %s listán %d név található.

# AKICK válaszok
CHAN_AKICK_SYNTAX
	AKICK csatorna {ADD | DEL | LIST | VIEW | COUNT | ENFORCE} [paraméterek]
CHAN_AKICK_LIST_SYNTAX
	AKICK csatorna LIST [+start] [maszk]
CHAN_AKICK_VIEW_SYNTAX
	AKICK csatorna VIEW [+start] [maszk]
CHAN_AKICK_DISABLED
	Sajnálom, a csatorna autokick listájának módosítása átmenetileg szünetel.
CHAN_AKICK_ALREADY_EXISTS
	%s már szerepel a megadott csatorna (%s) autokick listáján.
CHAN_AKICK_REACHED_LIMIT
	Sajnálom, %d autokick maszknál nem lehet több a listán. Elérted a felsõ korlátot.
CHAN_AKICK_ADDED
	%s hozzáadva a megadott csatorna (%s) autokick listájához.
CHAN_AKICK_NOT_FOUND
	%s nem szerepel a megadott csatorna (%s) autokick listáján.
CHAN_AKICK_DELETED
	%s törölve a megadott csatorna (%s) autokick listájáról.
CHAN_AKICK_LIST_EMPTY
	%s autokick listája üres.
CHAN_AKICK_NO_MATCH
	Nincs találat a megadott csatorna (%s) autokick listáján.
CHAN_AKICK_LIST_HEADER
	%s autokick listája:
# maszk, feltevõ személy , felrakás ideje, utoljára-használva, indok
CHAN_AKICK_VIEW_FORMAT
	    %s (%s által, ekkor: %s; utoljára használva %s)
	        %s
# maszk, feltevõ személy, felrakás ideje, indok
CHAN_AKICK_VIEW_UNUSED_FORMAT
	    %s (%s által, ekkor: %s; sose használt)
	        %s
CHAN_AKICK_COUNT
	%s AKIK listáján összesen %d személy szerepel.
CHAN_AKICK_ENFORCE_DONE
	AKICK ENFORCE a(z) %s csatornán teljesítve; %d user eltávolítva.

# LEVELS válaszok
CHAN_LEVELS_SYNTAX
	LEVELS csatorna {SET | DIS[ABLE] | LIST | RESET} [opció [szint]]
CHAN_LEVELS_READONLY
	Sajnálom, a csatorna privilégiumok szintjeinek módosítása jelenleg nem elérhetõ.
CHAN_LEVELS_RANGE
	A szintnek kizárólag %d és %d között kell lennie.
CHAN_LEVELS_CHANGED
	%s szintjének új értéke a megadott csatornán (%s): %d.
CHAN_LEVELS_UNKNOWN
	A megadott beállítás (%s) ismeretlen. Az érvényes beállítások
	listáját a /msg %s HELP LEVELS DESC parancs beírásával találod meg.
CHAN_LEVELS_DISABLED
	%s letiltva a megadott csatornán (%s).
CHAN_LEVELS_LIST_HEADER
	%s privilégiumok szintjeinek beállításai:
CHAN_LEVELS_LIST_DISABLED
	    %-*s  (letiltva)
CHAN_LEVELS_LIST_FOUNDER
	    %-*s  (csak foundernek)
CHAN_LEVELS_LIST_NORMAL
	    %-*s  %4d
CHAN_LEVELS_RESET
	%s privilégiumok szintjeit visszaállítottam az alapbeállításokra.

# INFO válaszok
CHAN_INFO_SYNTAX
	INFO csatorna [ALL]
CHAN_INFO_HEADER
	%s információi:
CHAN_INFO_FOUNDER
	Tulajdonos: %s
CHAN_INFO_SUCCESSOR
	Jogutód: %s
CHAN_INFO_DESCRIPTION
	Leírás: %s
CHAN_INFO_ENTRYMSG
	Belépési üzenet: %s
CHAN_INFO_TIME_REGGED
	Regisztrálás ideje: %s
CHAN_INFO_LAST_USED
	Utoljára használva: %s
CHAN_INFO_LAST_TOPIC
	Utolsó téma: %s
CHAN_INFO_TOPIC_SET_BY
	A témát beállította: %s
CHAN_INFO_URL
	URL: %s
CHAN_INFO_EMAIL
	E-mail cím: %s
CHAN_INFO_OPTIONS
	Opciók: %s
CHAN_INFO_OPT_PRIVATE
	Privát
CHAN_INFO_OPT_KEEPTOPIC
	Témamegõrzés
CHAN_INFO_OPT_TOPICLOCK
	Témalezárás
CHAN_INFO_OPT_SECUREOPS
	Opok védelme
CHAN_INFO_OPT_LEAVEOPS
	Opok meghagyása
CHAN_INFO_OPT_RESTRICTED
	Korlátozott hozzáférés
CHAN_INFO_OPT_SECURE
	Secure
CHAN_INFO_OPT_OPNOTICE
	Opok értesítése
CHAN_INFO_OPT_ENFORCE
	Op jog eröltetése
CHAN_INFO_OPT_MEMO_RESTRICTED
	Korlátozott memo üzenetek
CHAN_INFO_OPT_NONE
	Nincs
CHAN_INFO_MODE_LOCK
	Mód lock: %s
CHAN_INFO_NO_EXPIRE
	Ez a csatorna nem fog lejárni.
CHAN_INFO_SUSPEND_DETAILS
	Felfüggesztette: %s; dátum: %s (%s).
CHAN_INFO_SUSPEND_REASON
	Indok: %s
CHAN_INFO_SHOW_ALL
	További információért írd be: /msg %s INFO %s ALL

# LIST válaszok
CHAN_LIST_SYNTAX
	LIST [+start] sablon
CHAN_LIST_OPER_SYNTAX
	LIST [+start] sablon [FORBIDDEN] [NOEXPIRE] [SUSPENDED]
CHAN_LIST_HEADER
	%s listájának bejegyzései:
CHAN_LIST_NO_MATCH
	Nem talált ilyen csatornát.


# INVITE válaszok
CHAN_INVITE_SYNTAX
	INVITE csatora
CHAN_INVITE_OK
	%s meghívva a megadott a csatornára (%s).

# OP/DEOP/VOICE/DEVOICE válaszok
CHAN_OPVOICE_SYNTAX
	%s csatorna [nick]
CHAN_OP_SUCCEEDED
	%s operátor lett a megadott csatornán (%s).
CHAN_OP_ALREADY
	%s már rendelkezik operátor joggal a megadott csatornán (%s).
CHAN_OP_FAILED
	%s nem kaphat operátor jogot a megadott csatornán (%s).
CHAN_DEOP_SUCCEEDED
	%s többé nem operátor a megadott csatornán (%s).
CHAN_DEOP_ALREADY
	%s még nem rendelkezik operátor joggal a megadott csatornán (%s).
CHAN_DEOP_FAILED
	%s operátor joga nem vehetõ el a megadott csatornán (%s).
CHAN_VOICE_SUCCEEDED
	%s beszédjogot kapott a megadott csatornán (%s).
CHAN_VOICE_ALREADY
	%s már rendelkezik beszédjoggal a megadott csatornán (%s).
CHAN_VOICE_FAILED
	%s kaphat beszédjogot a megadott csatornán (%s).
CHAN_DEVOICE_SUCCEEDED
	%s többé nem rendelkezik beszédjoggal a megadott csatornán (%s).
CHAN_DEVOICE_ALREADY
	%s még nem rendelkezik beszédjoggal a megadott csatornán (%s).
CHAN_DEVOICE_FAILED
	%s beszédjogát nem veheted el a megadott csatornán (%s).
CHAN_HALFOP_SUCCEEDED
	%s féloperátor lett a megadott csatornán (%s).
CHAN_HALFOP_ALREADY
	%s már rendelkezik féloperátor joggal a megadott csatornán (%s).
CHAN_HALFOP_FAILED
	%s nem lehet féloperátor a megadott csatornán (%s).
CHAN_DEHALFOP_SUCCEEDED
	%s többé nem féloperátor a megadott csatornán (%s).
CHAN_DEHALFOP_ALREADY
	%s még nem féloperátor a megadott csatornán (%s).
CHAN_DEHALFOP_FAILED
	%s féloperátori jogát nem veheted el a megadott csatornán (%s).
CHAN_PROTECT_SUCCEEDED
	%s védelmi jogot kapott a megadott csatornán (%s).
CHAN_PROTECT_ALREADY
	%s már rendelkezik védelmi joggal a megadott csatornán (%s).
CHAN_PROTECT_FAILED
	%s nem kaphat védelmi jogot a megadott csatornán (%s).
CHAN_DEPROTECT_SUCCEEDED
	%s többé nem rendelkezik védelmi joggal a megadott csatornán (%s).
CHAN_DEPROTECT_ALREADY
	%s még nem rendelkezik védelmi joggal a megadott csatornán (%s).
CHAN_DEPROTECT_FAILED
	%s védelmi jogát nem veheted el a megadott csatornán (%s).

# UNBAN válaszok
CHAN_UNBAN_SYNTAX
	UNBAN csatorna
CHAN_UNBANNED
	Már nem vagy kitiltva a megadott csatornán (%s).

# KICK válaszok
CHAN_KICK_SYNTAX
	KICK csatorna nick [indok]
CHAN_KICK_PROTECTED
	%s nem rúgható ki a megadott csatornáról (%s).
CHAN_KICKED
	%s ki lett rúgva a megadott csatornáról (%s).

# TOPIC válaszok
CHAN_TOPIC_SYNTAX
	TOPIC csatorna téma

# CLEAR válaszok
CHAN_CLEAR_SYNTAX
	CLEAR csatorna mit
CHAN_CLEARED_BANS
	A megadott csatornán (%s) minden kitiltás fel lett oldva.
CHAN_CLEARED_EXCEPTIONS
	A megadott csatornán (%s) minden kivétel el lett távolítva.
CHAN_CLEARED_INVITES
	A megadott csatornán (%s) minden meghívási maszk el lett távolítva.
CHAN_CLEARED_MODES
	A megadott csatornán (%s) minden mód el lett távolítva.
CHAN_CLEARED_OPS
	A megadott csatornán (%s) minden operátori jog el lett véve.
CHAN_CLEARED_HALFOPS
	A megadott csatornán (%s) minden féloperátori jog el lett véve.
CHAN_CLEARED_VOICES
	A megadott csatornán (%s) minden hangadás el lett véve.
CHAN_CLEARED_USERS
	A megadott csatornán (%s) minden felhasználó el lett távolítva.

# GETPASS válaszok
CHAN_GETPASS_SYNTAX
	GETPASS csatorna
CHAN_GETPASS_UNAVAILABLE
	A GETPASS parancs nem használható a %s csatornán,
	mert az encryption használatban van.
CHAN_GETPASS_PASSWORD_IS
	A megadott csatorna (%s) jelszava: %s.

# FORBID válaszok
CHAN_FORBID_SYNTAX
	FORBID csatorna
CHAN_FORBID_SHORT_CHANNEL
	A # csatornanév nem tiltható.
CHAN_FORBID_SUCCEEDED
	A megadott csatorna (%s) mostantól nem használható.
CHAN_FORBID_FAILED
	A megadott csatorna (%s) nem tiltható le!

# SUSPEND válaszok
CHAN_SUSPEND_SYNTAX
	SUSPEND [+elévülés] csatorna indok
CHAN_SUSPEND_SUCCEEDED
	A megadott csatorna (%s) használata fel lett függesztve.
CHAN_SUSPEND_ALREADY_SUSPENDED
	A megadott csatorna (%s) használata már fel van függesztve.

# UNSUSPEND válaszok
CHAN_UNSUSPEND_SYNTAX
	UNSUSPEND csatorna
CHAN_UNSUSPEND_SUCCEEDED
	A megadott csatorna (%s) újra használható.
CHAN_UNSUSPEND_NOT_SUSPENDED
	A megadott csatorna (%s) használata nincs felfüggesztve.

###########################################################################
#
# MemoServ üzenetek
#
###########################################################################

# Automatikus üzenetek
MEMO_HAVE_NEW_MEMO
	Van egy új üzeneted.
MEMO_HAVE_NEW_MEMOS
	%d új üzeneted van.
MEMO_TYPE_READ_LAST
	Az utolsó üzenet elolvasásához használd a /msg %s READ LAST parancsot.
MEMO_TYPE_READ_NUM
	Az üzenet elolvasásához írd be: /msg %s READ %d.
MEMO_TYPE_LIST_NEW
	Az üzenetek listájához írd be: /msg %s LIST NEW
MEMO_AT_LIMIT
	Figyelem: Elérted a fogadható üzenetek maximális számát (%d). Nem
	fogadhatsz több új üzenetet, amíg nem törölsz néhányat.
MEMO_OVER_LIMIT
	Figyelem: Túllépted a fogadható üzenetek maximális számát (%d). Nem
	fogadhatsz több új üzenetet, amíg nem törölsz néhányat.
MEMO_NEW_MEMO_ARRIVED
	%s küldött egy új üzenetet.
	Ha szeretnéd elolvasni, írd be: /msg %s READ %d.
MEMO_NEW_CHAN_MEMO_ARRIVED
	Érkezett új memo %s részérõl %s csatornára.
	Írd be /msg %s READ %d az olvasáshoz.

# Multi-felhasználási válaszok
MEMO_HAVE_NO_MEMOS
	Nincs üzeneted.
MEMO_HAVE_NO_NEW_MEMOS
	Nem kaptál új üzenetet.
MEMO_DOES_NOT_EXIST
	A megadott számú üzenet (%d.) nem szerepel az üzenetlistán!
MEMO_LIST_NOT_FOUND
	A megadott számú üzenetek nem szerepelnek az üzenetlistán.

# SEND válaszok
MEMO_SEND_SYNTAX
	SEND {nick | csatorna} üzenet
MEMO_SEND_DISABLED
	Sajnálom, az üzenetküldés jelenleg szünetel.
MEMO_SEND_PLEASE_WAIT
	Kérlek, várj %s mielõtt SEND parancsot újra használod!
MEMO_SEND_CHAN_NOT_AVAIL
	Sajnálom, a csatorna memok nem elérhetõk
MEMO_X_GETS_NO_MEMOS
	%s nem fogadhat üzeneteket.
MEMO_X_HAS_TOO_MANY_MEMOS
	%s túl sok üzenettel rendelkezik, és nem fogadhat többet.
MEMO_SENT
	Üzenet elküldve %s részére.
MEMO_SEND_FAILED
	Memo küldése %s részére sikertelen.

# LIST válaszok
MEMO_LIST_SYNTAX
	LIST [szám | list | NEW]
MEMO_LIST_MEMOS
	%s üzenetei. Olvasáshoz írd be: /msg %s READ szám
MEMO_LIST_NEW_MEMOS
	%s új üzenetei. Olvasáshoz írd be: /msg %s READ szám
MEMO_LIST_HEADER
	   Szám  Küldõ             Dátum/Idõ
MEMO_LIST_FORMAT
	%c%c%c%4d  %-16s  %s

# READ válaszok
MEMO_READ_SYNTAX
	READ {szám | list | LAST | NEW}
MEMO_HEADER
	Üzenetszám: %d; feladó: %s (%s). Törléshez írd be: /msg %s DEL %d
MEMO_CHAN_HEADER
	Üzenet %d innen %s; %s (%s). Törléshez írd be: /msg %s DEL %d

# SAVE válaszok
MEMO_SAVE_SYNTAX
	SAVE {szám | list}
MEMO_SAVED_ONE
	A megadott üzenet (%d) nem fog lejárni.
MEMO_SAVED_SEVERAL
	A megadott üzenetek (%d) nem fognak lejárni.

# DEL válaszok
MEMO_DEL_SYNTAX
	DEL {szám | lista | ALL}
MEMO_DELETED_NONE
	Nem töröltem egy üzenetet sem.
MEMO_DELETED_ONE
	A megadott üzenetet (%d) töröltem.
MEMO_DELETED_SEVERAL
	A megadott üzeneteket (%d) töröltem.
MEMO_DELETED_ALL
	Minden üzenetet töröltem.

#RENUMBER responses
MEMO_RENUMBER_ONLY_YOU
	Csak a saját nicknél használhatod a RENUMBER parancsot.
MEMO_RENUMBER_DONE
	A memoid újra lettek számozva

# SET válaszok
MEMO_SET_SYNTAX
	SET opció paraméterek
MEMO_SET_DISABLED
	Sajnálom, az üzenet opcióinak beállítása átmenetileg szünetel.
MEMO_SET_UNKNOWN_OPTION
	Ismeretlen SET opció: %s.

# SET NOTIFY válaszok
MEMO_SET_NOTIFY_SYNTAX
	SET NOTIFY {ON | LOGON | NEW | OFF}
MEMO_SET_NOTIFY_ON
	%s mostmár értesíteni fog az új üzenetekrõl minden csatlakozáskor.
MEMO_SET_NOTIFY_LOGON
	%s mostmár értesíteni fog az új üzenetekrõl minden csatlakozáskor
	illetve az /AWAY kikapcsolásakor.
MEMO_SET_NOTIFY_NEW
	%s mostmár értesíteni fog, ha új üzeneted érkezik.
MEMO_SET_NOTIFY_OFF
	%s mostmár nem fog értestíteni, ha új üzeneted érkezik.

# SET LIMIT válaszok
MEMO_SET_LIMIT_SYNTAX
	SET LIMIT korlát
MEMO_SET_LIMIT_OPER_SYNTAX
	SET LIMIT [nick] {korlát | NONE | DEFAULT} [HARD]
MEMO_SET_YOUR_LIMIT_FORBIDDEN
	Nem változtathatod meg a maximálisan fogadható üzenetek számát.
MEMO_SET_LIMIT_FORBIDDEN
	Nem változtathatod meg %s üzeneteinek számát.
MEMO_SET_YOUR_LIMIT_TOO_HIGH
	A maximálisan fogadható üzenetek száma nem lehet nagyobb, mint %d.
MEMO_SET_LIMIT_TOO_HIGH
	%s maximálisan fogadható üzeneteinek száma nem lehet nagyobb, mint %d.
MEMO_SET_LIMIT_OVERFLOW
	Túl nagy értéket adtál meg a maximálisan fogadható üzenetek számának,
	%d lesz helyette.
MEMO_SET_YOUR_LIMIT
	A maximálisan fogadható üzenetek számanak új értéke: %d.
MEMO_SET_YOUR_LIMIT_ZERO
	Ezentúl nem tudsz üzeneteket fogadni.
MEMO_SET_YOUR_LIMIT_DEFAULT
	A maximálisan fogadható üzenetek számát alapértelmezésre állítottam (%d).
MEMO_UNSET_YOUR_LIMIT
	Ezentúl korlátlan számú üzenetet fogadhatsz.
MEMO_SET_LIMIT
	%s maximálisan fogadható üzeneteinek száma: %d.
MEMO_SET_LIMIT_ZERO
	%s ezentúl nem fogadhat üzeneteket.
MEMO_SET_LIMIT_DEFAULT
	%s maximálisan fogadható üzeneteinek számát alapértelmezésre állítottam (%d).
MEMO_UNSET_LIMIT
	%s ezentúl korlátlan számú üzenetet fogadhat.

# INFO válaszok
MEMO_INFO_NO_MEMOS
	Jelenleg nincs üzeneted.
MEMO_INFO_MEMO
	Van 1 üzeneted.
MEMO_INFO_MEMO_UNREAD
	Van 1 olvasatlan üzeneted.
MEMO_INFO_MEMOS
	Van %d üzeneted.
MEMO_INFO_MEMOS_ONE_UNREAD
	Van %d üzeneted, ebbõl 1 olvasatlan.
MEMO_INFO_MEMOS_SOME_UNREAD
	Van %d üzeneted, ebbõl %d olvasatlan.
MEMO_INFO_MEMOS_ALL_UNREAD
	Van %d üzeneted; és mind olvasatlan.
MEMO_INFO_LIMIT
	A maximálisan fogadható üzentek száma:  %d.
MEMO_INFO_HARD_LIMIT
	A maximálisan fogadható üzentek száma: %d, és ez a beállítás
	nem változtatható meg.
MEMO_INFO_LIMIT_ZERO
	A maximálisan fogadható üzentek száma: 0; és nem fogadhatsz üzenteket.
MEMO_INFO_HARD_LIMIT_ZERO
	A maximálisan fogadható üzentek száma: 0; és nem fogadhatsz üzenteket.
	Ez a beállítás nem változtatható meg.
MEMO_INFO_NO_LIMIT
	Korlátlan számú üzenetet fogadhatsz.
MEMO_INFO_NOTIFY_OFF
	Nem kapsz értesítést az új üzeneteidrõl.
MEMO_INFO_NOTIFY_ON
	Értesítést kapsz új üzenetek érkezésérõl minden csatlakozáskor.
MEMO_INFO_NOTIFY_RECEIVE
	Értesítést kapsz új üzenetek érkezésekor.
MEMO_INFO_NOTIFY_SIGNON
	Értesítést kapsz az új üzeneteidrõl, amikor fellépsz az IRC-re.
MEMO_INFO_X_NO_MEMOS
	%s üzenetlistája üres.
MEMO_INFO_X_MEMO
	%s listáján van 1 üzenet.
MEMO_INFO_X_MEMO_UNREAD
	%s listáján van 1 olvasatlan üzenet.
MEMO_INFO_X_MEMOS
	%s számára %d üzenet érkezett.
MEMO_INFO_X_MEMOS_ONE_UNREAD
	%s számára %d üzenet érkezett, ebbõl 1 olvasatlan.
MEMO_INFO_X_MEMOS_SOME_UNREAD
	%s számára %d üzenet érkezett, ebbõl %d olvasatlan.
MEMO_INFO_X_MEMOS_ALL_UNREAD
	%s számára %d üzenet érkezett, és mind olvasatlan.
MEMO_INFO_X_LIMIT
	%s maximálisan fogadható üzeneteinek száma: %d.
MEMO_INFO_X_HARD_LIMIT
	%s maximálisan fogadható üzenteinek száma: %d, és ez
	nem változtatható meg.
MEMO_INFO_X_NO_LIMIT
	%s korlátlan számú üzenetet fogadhat.
MEMO_INFO_X_NOTIFY_OFF
	%s nem kért értesítést az üzeneteirõl.
MEMO_INFO_X_NOTIFY_ON
	%s kéte, hogy értesítést kapjon minden csatlakozáskor,
	ha új üzenete van, illetve ha új üzente érkezik.
MEMO_INFO_X_NOTIFY_RECEIVE
	%s kérte, hogy értesítést kapjon, ha új üzenete érkezik.
MEMO_INFO_X_NOTIFY_SIGNON
	%s kérte, hogy minden csatlakozáskor értesítést kapjon, ha új üzenete van.

################ továbbítási module üzenetek/válaszok

# Levél Formátum
MEMO_FORWARD_MAIL_SUBJECT
	Üzenet %s számára
MEMO_FORWARD_MULTIPLE_MAIL_SUBJECT
	Üzenet %s számára
MEMO_FORWARD_MAIL_BODY
	Üzenet küldõje: %s (%s)
	%s
MEMO_FORWARD_CHANMEMO_MAIL_BODY
	Üzenet küldõje: %s (%s)
	%s

# FORWARD válaszok
MEMO_FORWARD_SYNTAX
	FORWARD {szám | lista | ALL}
MEMO_FORWARD_NEED_EMAIL
	A FORWARD parancs használatához be kell állítanod egy E-mail címet.
MEMO_FORWARD_PLEASE_WAIT
	Kérlek, várj %s mielõtt újra használod a FORWARD parancsot.
MEMO_FORWARD_FAILED
	Sajálom, a küldés sikertelen.
MEMO_FORWARDED_NONE
	Nem lehet elküldve egy üzenet sem.
MEMO_FORWARDED_ONE
	A megadott számú üzenet (%d) elküldve.
MEMO_FORWARDED_SEVERAL
	A megadott számú üzenetek (%d) elküldve.
MEMO_FORWARDED_ALL
	Minden üzenet elküldve.

# SET FORWARD válaszok
MEMO_SET_FORWARD_SYNTAX
	SET FORWARD {ON | COPY | OFF}
MEMO_SET_FORWARD_ON
	Az üzenetek továbbítva lesznek a következõ E-mail címre: %s
MEMO_SET_FORWARD_COPY
	Az üzenetek továbbítva lesznek a következõ E-mail címre: %s
	Az üzeneteid a Services elmentette.
MEMO_SET_FORWARD_OFF
	A nevedre érkezõ memokat nem fogja az E-mail címedre továbbítani.


################ mellõzés module üzenetek/válaszok

# IGNORE válaszok
MEMO_IGNORE_SYNTAX
	IGNORE {ADD | DEL | LIST} [nick | mask]
MEMO_IGNORE_ADD_SYNTAX
	IGNORE ADD név | maszk
MEMO_IGNORE_DEL_SYNTAX
	IGNORE DEL név | maszk
MEMO_IGNORE_LIST_FULL
	Tele van a mellõzöttek listád!
MEMO_IGNORE_ALREADY_PRESENT
	%s már szerepel a mellõzöttek listán.
MEMO_IGNORE_ADDED
	%s hozzáadva a mellõzöttek listához.
MEMO_IGNORE_NOT_FOUND
	%s nem szerepel a mellõzöttek listán.
MEMO_IGNORE_DELETED
	%s törölve a mellõzöttek listáról.
MEMO_IGNORE_LIST_EMPTY
	Az mellõzöttek listád üres.
MEMO_IGNORE_LIST
	Mellõzöttek lista:
MEMO_IGNORE_LIST_X_EMPTY
	Mellõzöttek lista %s üres.
MEMO_IGNORE_LIST_X
	Mellõzöttek lista %s:

###########################################################################
#
# OperServ üzenetek
#
###########################################################################

# Általános üzenetek
OPER_BOUNCY_MODES
	A Services nem tud módokat állítani.
	A szervered megfelelõen konfiguráltad?
OPER_BOUNCY_MODES_U_LINE
	A Services nem tud módokat állítani.
	A szervered U:lines részét megfelelõen konfiguráltad?

# GLOBAL válaszok
OPER_GLOBAL_SYNTAX
	GLOBÁLIS üzenet

# STATS válaszok
OPER_STATS_UNKNOWN_OPTION
	Ismeretlen STATS opció: %s.
OPER_STATS_RESET_USER_COUNT
	A maximum felhasználók száma alapértékre állt.
OPER_STATS_CURRENT_USERS
	Jelenlegi felhasználók száma: %d (%d op)
OPER_STATS_MAX_USERS
	Maximum felhasználók száma: %d (%s)
# Paraméterek: napok, STR_DAYS vagy STR_DAY, órák, percek
OPER_STATS_UPTIME_DHM
	A Services aktív: %d%s, %02d:%02d óta
# Ha >= 1 óra, órák és percek; ha < 1 óra, percek és másodpercek
OPER_STATS_UPTIME_HM_MS
	A Services aktív %s óta.

OPER_STATS_KBYTES_READ
	Fogadott adat:     %10u kB
OPER_STATS_KBYTES_WRITTEN
	Elküldött adat:    %10u kB
OPER_STATS_NETBUF_SOCK
	Server socket bufferek: %5u kB
OPER_STATS_NETBUF_SOCK_PERCENT
	Server socket bufferek: %5u kB (%d%%)
OPER_STATS_NETBUF_TOTAL
	Összes socket buffer:  %5u kB
OPER_STATS_NETBUF_TOTAL_PERCENT
	Összes socket buffer:  %5u kB (%d%%)

OPER_STATS_ALL_USER_MEM
	Felhasználók  : %6d rekord, %5d kB
OPER_STATS_ALL_CHANNEL_MEM
	Csatornák     : %6d rekord, %5d kB
OPER_STATS_ALL_SERVER_MEM
	Szerverek     : %6d rekord, %5d kB
OPER_STATS_ALL_NICKGROUPINFO_MEM
	Névcsoportok  : %6d rekord, %5d kB
OPER_STATS_ALL_NICKINFO_MEM
	Nevek         : %6d rekord, %5d kB
OPER_STATS_ALL_CHANSERV_MEM
	ChanServ      : %6d rekord, %5d kB
OPER_STATS_ALL_STATSERV_MEM
	StatServ      : %6d rekord, %5d kB
OPER_STATS_ALL_NEWS_MEM
	Hírüzenetek   : %6d rekord, %5d kB
OPER_STATS_ALL_AKILL_MEM
	Autokillek    : %6d rekord, %5d kB
OPER_STATS_ALL_EXCEPTION_MEM
	Kivételek     : %6d rekord, %5d kB
OPER_STATS_ALL_SGLINE_MEM
	SGline-ok     : %6d rekord, %5d kB
OPER_STATS_ALL_SQLINE_MEM
	SQline-ok     : %6d rekord, %5d kB
OPER_STATS_ALL_SZLINE_MEM
	SZline-ok     : %6d rekord, %5d kB
OPER_STATS_ALL_SESSION_MEM
	Sessions      : %6d rekord, %5d kB

#GETKEY responses
OPER_GETKEY_SYNTAX
	GETKEY csatorna
OPER_GETKEY_KEY_IS
	A csatorna kulcsa a %s csatornán: %s
OPER_GETKEY_NO_KEY
	A %s csatornán nincs kulcs beállítva.


# MODE válaszok
OPER_MODE_SYNTAX
	MODE csatorna módok

# CLEARMODES válaszok
OPER_CLEARMODES_SYNTAX
	CLEARMODES csatorna [ALL]
OPER_CLEARMODES_DONE
	Minden csatornamód törölve a megadott csatornáról (%s).
OPER_CLEARMODES_ALL_DONE
	Minden csatorna és felhasználómód törölve a megadott csatornáról (%s).

# CLEARCHAN válaszok
OPER_CLEARCHAN_SYNTAX
	CLEARCHAN csatorna
OPER_CLEARCHAN_DONE
	Minden felhasználót eltávolítottam a megadott (%s) csatornáról.

# KICK válaszok
OPER_KICK_SYNTAX
	KICK csatorna felhasználó indok

# ADMIN válaszok
OPER_ADMIN_SYNTAX
	ADMIN {ADD | DEL | LIST} [nick]
OPER_ADMIN_ADD_SYNTAX
	ADMIN ADD név
OPER_ADMIN_DEL_SYNTAX
	ADMIN DEL név
OPER_ADMIN_NO_NICKSERV
	NickServ jelenleg nem mûködik; az ADMIN parancs nem használható.
OPER_ADMIN_EXISTS
	%s már szerepel a Services adminok listáján.
OPER_ADMIN_ADDED
	%s felvéve a Services adminok listájára.
OPER_ADMIN_TOO_MANY
	Túl sok bejegyzés szerepel a Services adminok listáján (%d);
	több nem vehetõ fel.
OPER_ADMIN_REMOVED
	%s eltávolítva a Services adminok listájáról.
OPER_ADMIN_NOT_FOUND
	%s nem található a Services adminok listáján.
OPER_ADMIN_LIST_HEADER
	Services adminok listája:

# OPER válaszok
OPER_OPER_SYNTAX
	OPER {ADD | DEL | LIST} [nick]
OPER_OPER_ADD_SYNTAX
	OPER ADD név
OPER_OPER_DEL_SYNTAX
	OPER DEL név
OPER_OPER_NO_NICKSERV
	NickServ jelenleg nem mûködik; az OPER parancs nem használható.
OPER_OPER_EXISTS
	%s már szerepel a Services operátorok listáján.
OPER_OPER_ADDED
	%s felvéve a Services operátorok listájára.
OPER_OPER_TOO_MANY
	Túl sok bejegyzés szerepel a Services operátorok listáján (%d);
	több nem vehetõ fel.
OPER_OPER_REMOVED
	%s eltávolítva a Services operátorok listájáról.
OPER_OPER_NOT_FOUND
	%s nem található a Services operátorok listáján.
OPER_OPER_LIST_HEADER
	Services operátorok listája:

# AKILL/EXCLUDE/SxLINE közös válaszok
OPER_MASKDATA_SYNTAX
	%s {ADD | DEL | CLEAR | LIST | VIEW | CHECK | COUNT} [paraméterek]
OPER_MASKDATA_ADD_SYNTAX
	%s ADD [+elévülés] maszk indok
OPER_MASKDATA_DEL_SYNTAX
	%s DEL maszk
OPER_MASKDATA_CLEAR_SYNTAX
	%s CLEAR ALL
OPER_MASKDATA_LIST_SYNTAX
	%s {LIST | VIEW} [+start] [sablon] [NOEXPIRE]
OPER_MASKDATA_CHECK_SYNTAX
	%s CHECK maszk
# maszk, indok
OPER_MASKDATA_LIST_FORMAT
	%-32s  %s
# maszk, felrakó személy, felrakás ideje, utoljára használva, elévülés, indok
OPER_MASKDATA_VIEW_FORMAT
	%s (felvette: %s; dátum: %s; utoljára használva: %s; %s)
	    %s
# maszk, felrakó személy, felrakás ideje, elévülés, indok
OPER_MASKDATA_VIEW_UNUSED_FORMAT
	%s (felvette: %s, dátum: %s; még nem volt használva; %s)
	    %s

# AKILL válaszok
OPER_TOO_MANY_AKILLS
	Tele van az AKILL lista!
OPER_AKILL_EXISTS
	%s már szerepel az autokill listán.
OPER_AKILL_NO_NICK
	Megjegyzés: az autokill maszk nem tartalmazhat nicknevet; gyõzödj meg róla,
	hogy nem tartalmaz nick részt a te maszkolásod.
OPER_AKILL_MASK_TOO_GENERAL
	Az autokill maszk túl általános.
OPER_AKILL_EXPIRY_LIMITED
	A lejárati idõ maximumálisan %s lehet.
OPER_AKILL_ADDED
	%s hozzáadva az autokill listához.
OPER_AKILL_REMOVED
	%s eltávolítva az autokill listáról.
OPER_AKILL_CLEARED
	Az autokill lista törölve.
OPER_AKILL_NOT_FOUND
	%s nem található az autokill listán.
OPER_AKILL_LIST_HEADER
	Jelenlegi autokill lista:
OPER_AKILL_LIST_EMPTY
	Az autokill lista üres.
OPER_AKILL_LIST_NO_MATCH
	Nincs egyezõ maszk az autokill listán.
OPER_AKILL_CHECK_NO_MATCH
	Nincs egyezõ maszk %s találat az autokill listán.
OPER_AKILL_CHECK_HEADER
	Maszkok amelyek egyeznek ezzel %s az autokill listán:
OPER_AKILL_CHECK_TRAILER
	%d egyezõ maszkot talált.
OPER_AKILL_COUNT
	%d darab hostmaszk szerepel az autokill listán.

# AKILLCHAN válaszok
OPER_AKILLCHAN_SYNTAX
	AKILLCHAN [KILL] [+elévülés] csatorna indok
OPER_AKILLCHAN_AKILLED
	%d felhasználó autokillelve.
OPER_AKILLCHAN_KILLED
	%d felhasználó killelve.
OPER_AKILLCHAN_AKILLED_ONE
	1 felhasználó autokillelve.
OPER_AKILLCHAN_KILLED_ONE
	1 felhasználó killelve.

# EXCLUDE válaszok
OPER_TOO_MANY_EXCLUDES
	Tele van az KIVÉTEL lista!
OPER_EXCLUDE_EXISTS
	%s már szerepel az autokill kivétel listán.
OPER_EXCLUDE_ADDED
	%s hozzáadva az autokill kivétel listához.
OPER_EXCLUDE_REMOVED
	%s eltávolítva az autokill kivétel listáról.
OPER_EXCLUDE_CLEARED
	Az autokill kivétel lista törölve.
OPER_EXCLUDE_NOT_FOUND
	%s nem található az autokill kivétel listán.
OPER_EXCLUDE_LIST_HEADER
	Jelenlegi autokill kivétel lista:
OPER_EXCLUDE_LIST_EMPTY
	Az autokill kivétel lista üres.
OPER_EXCLUDE_LIST_NO_MATCH
	Nincs egyezõ maszk, az autokill kivétel listán
OPER_EXCLUDE_CHECK_NO_MATCH
	Nincs egyezõ maszk ezzel %s az autokill kivétel listán.
OPER_EXCLUDE_CHECK_HEADER
	Egyezõ maszkok ezzel %s az autokill kivétel listán:
OPER_EXCLUDE_CHECK_TRAILER
	%d egyezõ maszkot talált.
OPER_EXCLUDE_COUNT
	%d darab hosztmaszk szerepel az autokill kivétel listán.

# SxLINE válaszok
OPER_TOO_MANY_SLINES
	%s lista tele van!
OPER_SLINE_EXISTS
	%s már szerepel a %s listán.
OPER_SLINE_MASK_TOO_GENERAL
	A %s maszk túl általános.
OPER_SLINE_ADDED
	%s hozzáadva a %s listához.
OPER_SLINE_REMOVED
	%s eltávolítva a %s listáról.
OPER_SLINE_CLEARED
	Az %s lista törölve.
OPER_SLINE_NOT_FOUND
	%s nem található a %s listán.
OPER_SLINE_LIST_HEADER
	Jelenlegi %s lista:
OPER_SLINE_LIST_EMPTY
	Az %s lista üres.
OPER_SLINE_LIST_NO_MATCH
	Nincs ezzel egyezõ maszk az %s listán.
OPER_SLINE_CHECK_NO_MATCH
	Nincs ezzel egyezõ maszk: %s az %s listán.
OPER_SLINE_CHECK_HEADER
	Egyezõ maszkok ezzel: %s az %s listán:
OPER_SLINE_CHECK_TRAILER
	%d egyezõ maszkot talált.
OPER_SLINE_COUNT
	%d bejegyzés szerepel a %s listán.
OPER_SZLINE_NOT_AVAIL
	SZLINE nem elérhetõ ezzel az IRC szerverrel.

# SU válaszok
OPER_SU_SYNTAX
	SU jelszó
OPER_SU_NO_PASSWORD
	Nem használhatod a SU parancsot, mert nincs beállítva super-user jelszó.
OPER_SU_SUCCEEDED
	Most már Services super-user jogaid vannak.
OPER_SU_FAILED
	A jelszó nem ellenõrizhetõ.

# SET válaszok
OPER_SET_SYNTAX
	SET opció beállítás
OPER_SET_IGNORE_ON
	Mellõzés kód használva lesz.
OPER_SET_IGNORE_OFF
	Mellõzés kód nem lesz használva.
OPER_SET_IGNORE_ERROR
	A MELLÕZÉS beállítás lehet ON vagy OFF.
OPER_SET_READONLY_ON
	A Services most már csak olvasható módban dolgozik.
OPER_SET_READONLY_OFF
	A Services most már írható, olvasható módban dolgozik.
OPER_SET_READONLY_ERROR
	A READONLY beállítás lehet ON vagy OFF.
OPER_SET_DEBUG_ON
	A Services most már debug módban dolgozik.
OPER_SET_DEBUG_OFF
	A Services most már non-debug módban dolgozik.
OPER_SET_DEBUG_LEVEL
	A Services most már debug módban dolgozik (szint %d).
OPER_SET_DEBUG_ERROR
	A DEBUG beállítás lehet ON, OFF, vagy egy pozitiv szám.
OPER_SET_SUPASS_FAILED
	Nem sikerült állítani a Services super-user jelszót.
OPER_SET_SUPASS_OK
	A Services super-user jelszó megváltozott.
OPER_SET_SUPASS_NONE
	A Services super-user jelszó használta letiltva.
OPER_SET_UNKNOWN_OPTION
	Ismeretlen opció: %s.

# JUPE válaszok
OPER_JUPE_SYNTAX
	JUPE szervernév [indok]
OPER_JUPE_INVALID_NAME
	Érvénytelen szervernév (a szervernévnek tartalmaznia kell egy pontot).
OPER_JUPE_ALREADY_JUPED
	A megadott szerver (%s) már jupe-olva van.

# RAW válaszok
OPER_RAW_SYNTAX
	RAW szöveg

# UPDATE válaszok
OPER_UPDATE_SYNTAX
	UPDATE [FORCE]
OPER_UPDATE_FORCE_FAILED
	Nem sikerült az adatbázist újranyitni.
OPER_UPDATING
	Menti az adatbázisokat...
OPER_UPDATE_COMPLETE
	Adatbázis mentése kész.
OPER_UPDATE_FAILED
	Adatbázis mentése sikertelen.

# REHASH válaszok
OPER_REHASHING
	A konfigurációs beállítások újraolvasása...
OPER_REHASHED
	Minden konfigurációs beállítás sikeresen frissítve.
OPER_REHASH_ERROR
	Hiba lépett fel a konfigurációs beállítások újraolvasása közben.
	Kérlek ellenõrizd a Services naplófájlt a részletekért.

# KILLCLONES válaszok
OPER_KILLCLONES_SYNTAX
	KILLCLONES név
OPER_KILLCLONES_UNKNOWN_NICK
	Ismeretlen felhasználó: %s.
OPER_KILLCLONES_KILLED
	%s KILLCLONES-t használt %s felhasználóra, kilõve %d klón.
OPER_KILLCLONES_KILLED_AKILL
	%s KILLCLONES-t használt %s felhasználóra, kilõve %d klón.
	Egy átmeneti AKILL hozzáadva %s maszkkal.


###########################################################################
#
# Session Limit és Exception üzenetek (exception=kivétel)
#
###########################################################################

# EXCEPTION válasz
OPER_EXCEPTION_SYNTAX
	EXCEPTION {ADD | DEL | CLEAR | MOVE | LIST | VIEW | CHECK | COUNT} [paraméterek]
OPER_EXCEPTION_ADD_SYNTAX
	EXCEPTION ADD [+lejárat] maszk korlát indok
OPER_EXCEPTION_DEL_SYNTAX
	EXCEPTION DEL {maszk | lista}
OPER_EXCEPTION_CLEAR_SYNTAX
	EXCEPTION CLEAR ALL
OPER_EXCEPTION_MOVE_SYNTAX
	EXCEPTION MOVE szám újszám
OPER_EXCEPTION_ALREADY_PRESENT
	A megadott maszk (%s) már szerepel az exception listán.
OPER_EXCEPTION_TOO_MANY
	A session-korlátozási kivétellista tele van!
OPER_EXCEPTION_ADDED
	%s session-korlátozása beállítva a következõre: %d.
OPER_EXCEPTION_MOVED
	%s kivétel bejegyzésének száma (#%d) átállítva a következõre: %d.
OPER_EXCEPTION_NO_SUCH_ENTRY
	A megadott bejegyzés (#%d) nem található a session-korlátozási kivétellistán.
OPER_EXCEPTION_NOT_FOUND
	%s nem található a session-korlátozási kivétellistán.
OPER_EXCEPTION_NO_MATCH
	Nincs egyezõ bejegyzés a session-korlátozási kivétellistán.
OPER_EXCEPTION_EMPTY
	Üres a session-korlátozási kivétellista.
OPER_EXCEPTION_DELETED
	%s törölve a session-korlátozási kivétellistáról.
OPER_EXCEPTION_DELETED_ONE
	Törölve 1 bejegyzés a session-korlátozási kivétellistáról.
OPER_EXCEPTION_DELETED_SEVERAL
	Törölve %d bejegyzés a session-korlátozási kivétellistáról.
OPER_EXCEPTION_CLEARED
	Session-korlátozás kivétel lista törölve.
OPER_EXCEPTION_LIST_SYNTAX
	EXCEPTION {LIST | VIEW} [[+start] sablon | lista]
OPER_EXCEPTION_LIST_HEADER
	Jelenlegi session-korlátozási kivétel lista:
# maszk, korlát
OPER_EXCEPTION_LIST_FORMAT
	%3d  %4d   %s
OPER_EXCEPTION_LIST_COLHEAD
	Sorszám  Korlát  Hoszt
# maszk, felrakó személy, felrakás ideje, elévülés, limit, indok
OPER_EXCEPTION_VIEW_FORMAT
	%3d.  %s  (%s által; ekkor:  %s; %s)
	    Korlát: %-4d  - %s
OPER_EXCEPTION_CHECK_SYNTAX
	EXCEPTION CHECK maszk
OPER_EXCEPTION_CHECK_NO_MATCH
	Nincs egyezõ maszk ezzel %s a session-korlátozási kivétel listán.
OPER_EXCEPTION_CHECK_HEADER
	Egyezõ maszkok ezzel %s a session-korlátozási kivétel listán:
OPER_EXCEPTION_CHECK_TRAILER
	%d egyezõ maszkot talált.
OPER_EXCEPTION_COUNT
	Összesen %d bejegyzés szerepel a session-korlát kivétel listán.
OPER_EXCEPTION_INVALID_LIMIT
	Érvénytelen session-korlátozási szám; ez a szám nem lehet
	nullánál kisebb, és nagyobb, mint %d.
OPER_EXCEPTION_INVALID_HOSTMASK
	Érvénytelen hosztmaszk. Csak valós hosztmaszkok érvényesek,
	vagyis nem szerepelhet bennük nick vagy felhasználónév.

# SESSION válaszok
OPER_SESSION_SYNTAX
	SESSION {LIST korlát | VIEW host}
OPER_SESSION_LIST_SYNTAX
	SESSION LIST korlát
OPER_SESSION_VIEW_SYNTAX
	SESSION VIEW host
OPER_SESSION_INVALID_THRESHOLD
	Érvénytelen határérték; a határérték legyen nagyobb, mint 1.
OPER_SESSION_NOT_FOUND
	%s nem található a session listán.
OPER_SESSION_LIST_HEADER
	Hostok legalább %d sessionnal:
OPER_SESSION_LIST_COLHEAD
	Session  Host
# korlát, host
OPER_SESSION_LIST_FORMAT
	%6d    %s
# host, sessions, korlát
OPER_SESSION_VIEW_FORMAT
	A megadott hostnak (%s) jelenleg %d sessionja van, sessin korlátja: %d.


###########################################################################
#
# Hirek rendszer üzenetek
#
###########################################################################

NEWS_LOGON_TEXT
	[Fellépési üzenet - %s] %s
NEWS_OPER_TEXT
	[Oper üzenet - %s] %s

NEWS_LOGON_SYNTAX
	LOGONNEWS {ADD|DEL|LIST} [szöveg|szám]
NEWS_LOGON_LIST_HEADER
	Fellépési üzeneti egységek:
NEWS_LOGON_LIST_ENTRY
	%5d (%s %s által)
	    %s
NEWS_LOGON_LIST_NONE
	Nincs egyetlen fellépési üzenet sem.
NEWS_LOGON_ADD_SYNTAX
	LOGONNEWS ADD szöveg
NEWS_LOGON_ADD_FULL
	Tele van a hírüzenetek listája!
NEWS_LOGON_ADDED
	Hozzáadva a fellépési üzenetekhez (#%d).
NEWS_LOGON_DEL_SYNTAX
	LOGONNEWS DEL {szám | ALL}
NEWS_LOGON_DEL_NOT_FOUND
	A megadott számú (#%d) fellépési üzenet nem található!
NEWS_LOGON_DELETED
	A megadott számú #%d fellépési üzenet törölve.
NEWS_LOGON_DEL_NONE
	Nincs törölhetõ fellépési üzenet!
NEWS_LOGON_DELETED_ALL
	Minden fellépési üzenet törölve.

NEWS_OPER_SYNTAX
	OPERNEWS {ADD|DEL|LIST} [szöveg|szám]
NEWS_OPER_LIST_HEADER
	Oper üzenetek:
NEWS_OPER_LIST_ENTRY
	%5d (%s %s által)
	    %s
NEWS_OPER_LIST_NONE
	Nincs oper hírüzenet.
NEWS_OPER_ADD_SYNTAX
	OPERNEWS ADD szöveg
NEWS_OPER_ADD_FULL
	Tele van a hírüzenetek listája!
NEWS_OPER_ADDED
	Hozzáadva az oper hírüzenetekhez (#%d).
NEWS_OPER_DEL_SYNTAX
	OPERNEWS DEL {szám | ALL}
NEWS_OPER_DEL_NOT_FOUND
	A megadott számú (#%d) oper üzenet nem található!
NEWS_OPER_DELETED
	A megadott számú (#%d) oper üzenet törölve.
NEWS_OPER_DEL_NONE
	Nincs törölhetõ oper üzenet!
NEWS_OPER_DELETED_ALL
	Minden oper üzenet törölve.

NEWS_HELP_LOGON
	Syntax: LOGONNEWS ADD szöveg
	        LOGONNEWS DEL {szám | ALL}
	        LOGONNEWS LIST
	
	Szerkeszti vagy megjeleniti a fellépõ üzenetek listáját. Amikor egy
	felhasználó csatlakozik a hálózathoz, meg fogja kapni ezek az
	üzeneteket. (Habár, nem több mint három lesz csak elküldve a
	usernek, hogy elkerüljük a user kifloodolását. Ha több, mint
	három hír üzenet is van, akkor csak a három legújabb lesz elküldve.)
	
	LOGONNEWS LIST arra használható, hogy bármelyik IRC operator listázhassa
	a jelenlegi hír üzeneteket. Az ADD és DEL parancsok csak Services
	operátorok által használhatóak.

NEWS_HELP_OPER
	Syntax: OPERNEWS ADD szöveg
	        OPERNEWS DEL {szám | ALL}
	        OPERNEWS LIST
	
	Szerkeszti vagy megjeleniti az oper hírek listáját.  Amikor egy
	user felopereli magát (a /oper paranccsal), meg fogja kapni
	ezek az üzeneteket. (Habár, nem több mint három lesz csak elküldve a
	usernek, hogy elkerüljük a user kifloodolását. Ha több, mint
	három hír üzenet is van, akkor csak a három legujabb lesz elküldve.)
	
	OPERNEWS LIST arra használható, hogy bármelyik IRC operator listázhassa
	a jelenlegi oper híreket. Az ADD és DEL parancsok csak Services operátorok
	által használhatóak.


###########################################################################
#
# StatServ üzenetek
#
############################################################################

# Általános SERVERS válaszok
STAT_SERVERS_REMOVE_SERV_FIRST
	A megadott szerver (%s) jelenleg online.
	Elõször távolitsd el a hálozatról, majd próbáld újra.
STAT_SERVERS_SERVER_EXISTS
	A megadott szerver (%s) már létezik.

# SERVERS parancs szintaktika
STAT_SERVERS_SYNTAX
	SERVERS opció paraméterek

# STATS válaszok
STAT_SERVERS_STATS_TOTAL
	Összes Szerver: %2d
STAT_SERVERS_STATS_ON_OFFLINE
	Aktív: %2d (%d%%)  Inaktív: %2d (%d%%)
STAT_SERVERS_LASTQUIT_WAS
	Utolsó kilépés: %-30s  %s

# LIST válaszok
STAT_SERVERS_LIST_HEADER
	Szervernév                    Felhasználók    Operek
STAT_SERVERS_LIST_FORMAT
	%-30s %3d (%2d%%)  %3d (%2d%%)
STAT_SERVERS_LIST_RESULTS
	Lista vége -- %d/%d aktív szerver listázva.

# VIEW válaszok
STAT_SERVERS_VIEW_HEADER_ONLINE
	%-30s  (aktív)
STAT_SERVERS_VIEW_HEADER_OFFLINE
	%-30s  (inaktív)
STAT_SERVERS_VIEW_LASTJOIN
	Utolsó belépés: %s
STAT_SERVERS_VIEW_LASTQUIT
	Utolsó kilépés: %s
STAT_SERVERS_VIEW_QUITMSG
	Kilépõ üzenet: %s
STAT_SERVERS_VIEW_USERS_OPERS
	Felhasználók: %3d (%2d%%)  Operek: %3d (%2d%%)
STAT_SERVERS_VIEW_RESULTS
	Lista vége -- %d/%d szerver listázva.

# DELETE válaszok
STAT_SERVERS_DELETE_SYNTAX
	SERVERS DELETE szerver
STAT_SERVERS_DELETE_DONE
	A megadott szerver (%s) statisztikáját töröltem.

# COPY válaszok
STAT_SERVERS_COPY_SYNTAX
	SERVERS COPY szerver újnév
STAT_SERVERS_COPY_DONE
	A megadott szerver (%s) statisztikáját átmásoltam %s névre.

# RENAME válaszok
STAT_SERVERS_RENAME_SYNTAX
	SERVERS RENAME szerver újnév
STAT_SERVERS_RENAME_DONE
	A megadott szerver (%s) statisztikáját átneveztem %s névre.

# USERS válaszok
STAT_USERS_SYNTAX
	USERS opció paraméterek
STAT_USERS_TOTUSERS
	Összes felhasználó: %d
STAT_USERS_TOTOPERS
	Összes operátor: %d
STAT_USERS_SERVUSERS
	Felhasználók átlaga szerverenként: %d
STAT_USERS_SERVOPERS
	Operátorok átlaga szerverenként: %d.%d


###########################################################################
#
# NickServ help üzenetek
#
###########################################################################

NICK_HELP
	A %S lehetõséget nyújt a "register" parancs használatával
	a nickneved regisztrálására és ezzel megelõzi azt, hogy
	más is használhassa a nicked. A %S különbözõ parancsokkal
	írányítható, amelyek a nicknevek regisztrációját és fenntartását
	segítik elõ. A parancsok pontos listájáért írd be bármelyik
	ablakba: /msg %S HELP COMMANDS; hogy a parancsok szintaktikáját
	is megkapd, írd be: /msg %S parancs, és ha további
	információra van szükséged, akkor írd be: /msg %S HELP parancs.
NICK_HELP_EXPIRES
	Ha egy regisztrált nicknevet %s idejéig nem használnak, a
	regisztráció elévül és a nicknév elérhetõ lesz mások számára is,
	hogy használhassák.
NICK_HELP_WARNING
	Megjegyzés: Ez a service arról akar gondoskodni, hogy az
	IRC felhasználók számára biztosítsa az azonosítást is.
	És NEM azért van, hogy megkönnyítse a nicknevek "lopását"
	vagy más alattomos akciót. A %S visszaélés azt is eredményezhati,
	hogy esetleg elveszik az a nicknév, amivel visszaéltek.
NICK_HELP_COMMANDS
	A következõ parancsok használhatóak a  %S: által
	
	    REGISTER     Regisztrálhatod a nicknevet
	    IDENTIFY     Azonosíthatod magad a jelszavaddal
	    DROP         Törli a nickneved regisztrációját
NICK_HELP_COMMANDS_AUTH
	    AUTH         A nickneved regisztrációjának élesítése
	    SENDAUTH     Elküldi neked a nicked élesítési kódját
	    REAUTH       Beállít egy új kódot, ha elfelejtetted a jelszavad
	    RESTOREMAIL  Visszaállítja az utolsó auth E-mail címet
NICK_HELP_COMMANDS_LINK
	    LINK         Más nicket linkelhetsz a tiedhez
	    UNLINK       Törölheted a linkelt nickneveket
NICK_HELP_COMMANDS_ACCESS
	    ACCESS       Módosithatod a listát az élesítet hozzáférési címekrõl
NICK_HELP_COMMANDS_AJOIN
	    AJOIN        Módosítja az autojoin listád
NICK_HELP_COMMANDS_SET
	    SET          Set opciók, tartalmazza pl: a kill protection-t
	    UNSET        Törli a nickneved információit
	    RECOVER      Killelhetsz más usert, aki megszerezte a neved
	    RELEASE      Visszaszerezheted a nickedet RECOVER parancs használata után
	    GHOST        Bentragadt nicked killeléséhez segítség
	    INFO         Lekérheted az információkat egy nicknévrõl
NICK_HELP_COMMANDS_LIST
	    LIST         Listázhatod az összes nicknevet, amelyek kaptak maszk-ot
	    LISTEMAIL    Listázhatod a nickneveket az E-mail cím alapján
NICK_HELP_COMMANDS_LISTCHANS
	    LISTCHANS    Listázhatod azokat a csatornákatt, amelyek a nicknévre lettek regisztrálva
	    STATUS       Egy nicknév státuszának lekérése
NICK_HELP_REGISTER
	Syntax: %s
	
	Regisztráld a nickneved a %S adatbázisába. Amint a nicked
	regisztrált, használhatod a SET és az ACCESS parancsokat
	és konfigurálhatod a nicked beállításait. Légy biztos abban,
	hogy a jelszavadra emlékezni fogsz, hogy használni tud a
	beállításokat.Késõbb szükséged lehet a nickeddel kapcsolatos
	beállítások végrehajtására.
	(Jegyezd meg ezt: PROBLÉMÁT OKOZHAT  FIDO, Fido, és fido
	mind különbözõ jelszavak!)
NICK_HELP_REGISTER_EMAIL
	A nickneved regisztrációjának tartalmaznia kell egy E-mail címet;
	vagy beállíthasz egyet késõbb a SET EMAIL paraccsal.
NICK_HELP_REGISTER_EMAIL_REQ
	Tartalmaznia kell egy E-mail címet amikor regisztrálod a
	nicknevet. Megelõzheted az E-mail címed az Info lekérésekor
	történõ kiírását a SET HIDE paranccsal (További információért
	írd be: /msg %S HELP SET HIDE).
NICK_HELP_REGISTER_EMAIL_AUTH
	Egy valós E-mail címet kell megadnod amikor regisztrálod a
	nicknevedet; %S küldeni fog egy üzenetet az élesítési kódoddal
	erre a címre, amit használhatsz az AUTH paraccsal, hogy
	befejezd a nickneved regisztrációját. Megelõzheted az E-mail
	címed az Info lekérésekor történõ kiírását a SET HIDE
	paranccsal (További információért,írd be:/msg %S HELP SET HIDE).
NICK_HELP_REGISTER_END
	Tanácsok a jelszó megváltasztásához:
	
	A jelszavad nem lehet könnyen kitalálható. Például,a real name-ed
	jelszónak használni rossz ötlet. A nicknevedet használni
	jelszónakmég rosszabb ötlet és, ezt a %S nem is engedélyezi.
	Ezenkívül, a rövid jelszavak könnyen kitalálhatóak, ha valaki
	keresi, ezért olyan jelszót kell választanod ami legalább
	5 karakter hosszú.
NICK_HELP_IDENTIFY
	Syntax: IDENTIFY jelszó
	
	Megmutatja %S ,hogy te vagy az igazi tulajdonosa ennek a
	nicknek. Sok parancshoz szükséges hogy érvényesítsd a nickneved
	mielõtt használnád azt a parancsot. A jelszónak ugyanannak kell
	lennie amit a regisztrációnál megadtál.
NICK_HELP_DROP
	Syntax: DROP jelszó
	
	Regisztráció törlése (dropolja) a nickneved a %S adatbázisából.
	Az a nick ami dropolva van már szabadon hsználható bárki által,
	hogy újra-regisztrálhassa.
NICK_HELP_DROP_LINK
	Vigyázz mert amikor droppolod a nicked, az összes hozzálinkelt név
	is droppolodik. Hogy eltávolitsd a hozzálinkelt nevet, használd az
	UNLINK parancsot.
NICK_HELP_DROP_END
	Hogy meglõzzük a véletlen használatát ennek a parancsnak, neked be
	kell írnod a jelszavadat a parancsban amivel droppolod a nickneved,
	azaz törlöd a regisztrációját.
NICK_HELP_AUTH
	Syntax: AUTH code
	
	Élesíteni egy nicknév regisztrációját vagy E-mail címet
	változtatni. Hogy megbizonyosodj arról, hogy az E-mail címed
	helyes, %S küldeni fog neked egy üzentet, ami tartalmazza
	az élesítési kódodat arra az E-mail címre amit a regisztrációnál
	megadtál az E-mail cím cseréhez a SET EMAIL parancsot használd.
	Ez a parancs lehetõséget nyújt, arra hogy megerõsitsd azt, hogy
	kaptál üzenetet és folytasd a a nickneved normális használatát.
	Helyettesíts a kódot a parancsba a kilenc karakteres élesítési
	kódot amit az E-mail üzenetben kaptál.
NICK_HELP_SENDAUTH
	Syntax: SENDAUTH
	
	Küld számodra egy másolatot az élesítési kódodról a REGISZTRÁCIÓ
	vagy SET EMAIL parancs használata után.Ha te véletlenül törölted
	vagy más okból elvesztetted az élesítési kódodat tartalmazó üzeneted,
	akkor használhatod a parancsot,hogy szerezz egy másolatot arról az
	élesítési kódot tartalmazó üzenterõl, amit kaptál.
	
	Hogy a visszaélést megelõzzük, ez a parancs naponta csak
	egyszer használható.
NICK_HELP_REAUTH
	Syntax: REAUTH
	
	Beállít egy érvényesítõ kódot, amellyel azonosítani tudsz a
	nicknevedre. Ha elfelejtetted a nickneved jelszavát, használd
	ezt a parancsot, hogy új érvényesítõ kódot csináltass a szerverre,
	utána add meg ezt a kódot az AUTH paranccsal, hogy beazonosíts a
	nicknevedre, utána megváltoztathatod a jelszódat a SET PASSWORD
	paranccsal.
NICK_HELP_RESTOREMAIL
	Syntax: RESTOREMAIL jelszó
	
	Visszaállítja az utolsó érvényesített E-mail címedet a nicknevedhez.
	Ha véletlennül rossz e-mail címet adtál meg a SET EMAIL paranccsal,
	akkor használhatod ezt a parancsot, hogy visszaállítsd az eredeti
	E-mail címet és törölheted az érvényesítõ kódot a nicknévhez,
	lehetõvé téve az azonosítást a nicknevedhez újra.
	Meg kell adnod a nick jelszavadat a parancshoz.
	
	RESTOREMAIL csak a SET EMAIL parancs kiadás utána használható
	és mielõtt beírod az új érvényesítõ kódot az AUTH paranccsal.
NICK_HELP_LINK
	Syntax: LINK nicknév
	
	Új nicknév linkelése a nickedhez amit jelenleg használsz,
	létrehoz egy új "alias"-t a nickedhez. Az új nick is meg fogja
	kapni a jelenlegi nicked access hozzáféréseit és operátori
	kiváltságait és memoit; például olvashatsz üzeneteket,
	amiket a linkelt nevedre küldtek, vagy ha valamelyik nickedre
	van autoop jogosultságod egy csatornán, akkor minden hozzá
	linkelt neved is autoopot fog kapni.
	
	A linkelés megszüntetéséhez az UNLINK parancsot használhatod.
	
	Ezen parancs kiadásakor, azonositanod kell magad mint a nick tulajdonosa
	(használva az IDENTIFY parancsot), és tudnod kell annak a nicket is a
	jelszavát, amelyhez linkelõdni/unlinkelõdni akarsz.
NICK_HELP_UNLINK
	Syntax: UNLINK név
	
	Eltávolítja a linkelést (aliast) amit a nicnevedhez létrehoztál a
	LINK paranccsal. Nem tudod unlinkelni a jelenlegi nicked, amit
	jelenleg használsz; változtass egy másik linkelt nickre elõször mielõtt
	használod ezt a parancsot.
	
	Ha a "fõ nicked" unlinkeled, akkor a SET MAINNICK paranccsal,
	a jelenlegi nicked lesz automatikusan a fõ nick.
NICK_HELP_LISTLINKS
	Syntax: LISTLINKS
	
	Listázza az összes nicket amit a nickedhez linkeltél. A fõ neved
	a linkelt nickek csoportjában jelezve lesz a "*" jellel a baloldalán
	a nicknévnek.
NICK_HELP_ACCESS
	Syntax: ACCESS ADD maszk
	        ACCESS DEL maszk
	        ACCESS LIST
	
	Módosítja vagy megmutatja a hozzáférési listát a nickedhez.
	Ez a listája azon címeknek amelyek automatikusan ugy tekintõdnek
	mint a %S engedélyezett nicke.  Ha használni akarod a nicked különbözõ
	címekrõl, küldened kell egy IDENTIFY parancsot, hogy úgy legyél
	tekintve, mint  %S a nick tulajdonosa.
	
	Példa:
	    ACCESS ADD achurch@*.dragonfire.net
	        Engedélyezi a hozzáférést az achurch nicknek bármilyen géprõl.
	        aminek a dragonfire.net a domainje.
	    ACCESS DEL achurch@*.dragonfire.net
	        Törli az elözõ parancsot.
	    ACCESS LIST
	        Megmutatja a hozzáférési listát.

NICK_HELP_SET
	Syntax: SET opció paraméterek
	
	Beállítható különbözõ nicknév opciók.  opció ezek egyike lehet:
	
	    PASSWORD   A nickneved jelszavának állítása
	    LANGUAGE   A Services nyelvezetének megválasztása
	    URL        Egy URL társítása a nicknevedhez
	    EMAIL      Egy E-mail cím társítása a nicknevedhez
	    INFO       A szöveg beállítása, ami az INFO parancs használatakor lászik
	    KILL       A kill protection be/ki kapcsolása
	    SECURE     A nickneved védelmének be/ki kapcsolása
	    PRIVATE    A nickneved /msg %S LIST parancs listájából való elrejtés
	    NOOP       Megelõzi, hogy a nickneved hozzáadják a
	                   csatorna hozzáférési listájához
	    HIDE       A nicknévhez tartozó információk elrejtése
	    TIMEZONE   Az idõzóna beállítása
NICK_HELP_SET_OPTION_MAINNICK
	    MAINNICK   A fõ nickneved ami a csatornák access listáján szerepelni fog.
NICK_HELP_SET_END
	Ahhoz, hogy használd ezt a parancsot, elõször használnod kell az identify
	parancsot. (További információért, írd be: /msg %S HELP IDENTIFY).
	
	További információért, írd be: /msg %S HELP SET opció
NICK_HELP_SET_PASSWORD
	Syntax: SET PASSWORD új-jelszó
	
	Megváltoztatja a nicknév tulajdonosának jelszavát.
NICK_HELP_SET_LANGUAGE
	Syntax: SET LANGUAGE szám
	
	Megváltoztatja a SERVICES nyelvezetét, amin az üzenteket írja.
	(Például: amikor válaszol egy parancsra, amit te adtál ki).
	szám ki kell választanod a neked megfelelõt nyelveket tartalmazó
	listából:
NICK_HELP_SET_URL
	Syntax: SET URL url
	
	Egy URL társítás a nicknevedhez. Ez az URL szerepelni fog
	az INFO-id között, amit bárki lekérhet az INFO paranccsal.
NICK_HELP_SET_EMAIL
	Syntax: SET EMAIL cím
	
	Egy E-mail cím társítása a nicknevedhez.
	Ez az E-mail cím szerepelni fog az INFO-id között,
	amit bárki lekérhet az INFO paranccsal.
NICK_HELP_SET_INFO
	Syntax: SET INFO szöveg
	
	Egy szöveg társítása a nicknevedhez; Ez az szöveg szerepelni
	fog az INFO-id között, amit bárki lekérhet az INFO paranccsal.
NICK_HELP_SET_KILL
	Syntax: SET KILL {ON | QUICK | IMMED | OFF}
	
	Be/kikapcsolja az automatikus kill protection opciót a
	nickedhez. A kill protection bekapcsolásakor, ha más user
	megpróbálja használni a nicked, kap egy percet mielõtt
	meg kell változtatnia a nickjét egy másikra, ha letelt az idõ
	akkor ki lesz killelve az IRC-rõl a %S által.
	
	Ha a QUICK -et választod, a user csak 20 másodpercet kap
	a nickváltásra eltérõen az általános 60 másodperctõl.
	Ha az IMMED -et választod a user azonnal ki lesz killelve
	Figyelmeztetés nélkül vagy kap egy esélyt a nickváltásra;
	kérlek ne használd ezt az opciót csak ha nagyon indokolt.
	Viszont, a network adminisztrátorok letilthatják ezt az opciót.
NICK_HELP_SET_SECURE
	Syntax: SET SECURE {ON | OFF}
	
	Be/kikapcsolja a %S biztonsági opcióit a nicknevedhez.
	A SECURE beállítással, be kell írnod a jelszavadat
	mielõtt a nicknév tulajdonsaként leszel tekintve,
NICK_HELP_SET_PRIVATE
	Syntax: SET PRIVATE {ON | OFF}
	
	Be/kikapcsolja a %S privát opciót a nicknevedhez.
	A PRIVATE beállítással, beállíthatod azt hogy
	a nickneved ne szerepeljen a nicknév listában ami lekérhetõ a
	%S LIST paranccsal.
	(Máskülönben, bárki kaphat információt a nicknevedrõl használva az
	INFO parancsot.)
NICK_HELP_SET_NOOP
	Syntax: SET NOOP {ON | OFF}
	
	Be/kikapcsolja %S no-op opcióját a nicknevedhez.
	A NOOP opció beállításával megelõzheted, hogy a nicknevedet
	hozzáadják valamelyik csatorna hozzáférési listájához. Habár
	ha a neved már szerepel a csatorna hozzáférési listáján,
	akkor nem lesz eltávolítva akkor se, ha bekapcsolod ezt az
	opciót.
NICK_HELP_SET_HIDE
	Syntax: SET HIDE {EMAIL | USERMASK | QUIT} {ON | OFF}
	
	Lehetõséget nyújt arra hogy megelõzd néhány információnak a
	szerepelését az INFO parancs használatakor lekért információk közül.
	Elrejtheted az E-mail címed (EMAIL), az utoljára használt
	user@host maszkod (USERMASK), és az utolsó kilépõ üzeneted(QUIT).
	A második paraméter meghatározza, hogy az opció ki legyen kapcsolva(OFF)
	vagy el legyen rejtve az információ(ON).
NICK_HELP_SET_TIMEZONE
	Syntax: SET TIMEZONE {UTC-offset | time-zone | DEFAULT}
	
	Beállíthatod az idõzónát ami szerint az idõt látni szeretnéd
	(mondjuk egy nicknév vagy csatorna utolsó használatának idejérõl
	kapott infóban), ha a Services által használt idõzónát szeretnéd;
	a DEFAULT beállítás használható,hogy beállítsd a Services' idõzónáját.
	
	MEgjegyzés a Services nem fog automatikus átállni az idõeltolodáskor
	(nyári idõszámítás); nekem kell beállítanod.
	
	Példa:
	
	    SET TIMEZONE -5
	        Beállítja az idõzónát 5 órával az UTC utánra.
	    SET TIMEZONE +3:30
	        Beállítja az idõzónát 3 óra, 30 percre az UTCtõl elõrébb .
	    SET TIMEZONE JST
	        Beállítja a Japan Standard Time szerinti idõzónát (UTC+9:00).
NICK_HELP_SET_MAINNICK
	Syntax: SET MAINNICK név
	
	Ha több nicket linkelsz össze meg kell választanod a fõ nicked
	(main nick). Ez a nick lesz az össze linkelt neveid közül amelyik
	minden hozzáférési jogot megkap.
NICK_HELP_UNSET
	Syntax: UNSET {URL | EMAIL | INFO}
	
	Engedélyezi, hogy töröld az URL-t (URL), E-mail címet (EMAIL),
	vagy az információs szöveget (INFO) amit a nickedhez társítottál.
NICK_HELP_UNSET_REQ_EMAIL
	Syntax: UNSET {URL | INFO}
	
	Engedélyezi, hogy töröld az URL-t (URL), E-mail címet (EMAIL),
	vagy az információs szöveget (INFO) amit a nickedhez társítottál.
NICK_HELP_RECOVER
	Syntax: RECOVER név [jelszó]
	
	Amikor engedélyezed ezt az opciót, a %S fel fog hozni egy
	ál felhasználót ugyanazzal a névvel, amit vissza akarsz venni
	a másik névtõl. Ennek következtében az IRC szerver megbontja a
	másik felhasználó kapcsolatát. A %S ál felhasználója online marad
	%s hogy a másik felhasználó ne tudjon automatikusa újracsatlakozni;
	ezután az idõ letelte után, te felveheted a nevedet.Alternatív
	megoldás, a RELEASE parancs használata (/msg %S HELP RELEASE)
	ezzel hamarabb is visszakaphatod a neved.
	
	Abban az esetben ha használod a RECOVER parancsot, akkor
	tudnod kell a pontos jelszót a nickhez vagy be kell azonosítanod
	"identify"-zned egy olyan nickre, amely linkelve van a megadott
	nickhez.
NICK_HELP_RELEASE
	Syntax: RELEASE név [jelszó]
	
	Utasítja a %S -et, hogy ne tartsa tovább az automatikus kill
	védelem vagy a RECOVER parancs használatából kifolyólag
	felvett neved. Alapbeállítás szerint csak %s; tartja, de
	ezzel a paranccsal hamarabb átveheto újra a neved.
	
	Ha a RELEASE parancsot akarod használni, tudnod kell a
	pontos jelszót a nickhez vagy be kell azonosítanod "identify"-zned
	egy olyan nickre, amely linkelve van a megadott nickhez.

NICK_HELP_GHOST
	Syntax: GHOST név [jelszó]
	
	Ezt a parancsot akkor használhatod ha a neved beragad, befagy
	például lefagy a géped vagy a kapcsolatod. A bent ragadt neved
	killelheted, ha ismered a jelszót a névhez vagy be kell azonosítanod
	egy olyan névre, amely linkelve van a befagyott nevedhez.
	
NICK_HELP_INFO
	Syntax: INFO név [ALL]
	
	Megmutatja a megadott névvel kapcsolatos információkat, mint pl:
	a név tulajdonosa, utoljára látott címe és ideje, és a név
	opciókat. Ha beazonosítasz egy névre akkor teljes informaciókat
	kaphatsz az ALL parancs használatával és látni fogod a rejtett
	információkat is.

NICK_HELP_INFO_AUTH
	
	Ha a neved E-mail címed még nincs élesítve az AUTH parancs
	használatával nem fog megjelenni a regisztrált nicknevek listáján.
	
NICK_HELP_LISTCHANS
	Syntax: LISTCHANS
	
	Listázza az összes nevet, csatornát ami a te neved
	alatt lett regisztrálva.

NICK_HELP_LIST
	Syntax: LIST [+start] minta
	
	Listázza az összes regisztrált nevet kivéve azokat,
	amelyeken fent van PRIVATE opció beállítás.
	Ha astart paraméter is meg van adva, akkor a megadott számú találatot
	kihagyja és csak az utána lévoket jeleniti meg (például +50
	azt eredményezi, hogy az elso 50 találatot átugorja.)
	
	Példák:
	    LIST joeuser@foo.com
	        Listázz minden regisztrált nicknevet, amelyet utoljára:
	        joeuser@foo.com címrõl használtak.
	    LIST *Bot*
	        Listázz minden nicket amelyben szerepelt a Bot
	    LIST *@*.bar.org
	        Listázz minden nicket amit utoljára olyan user használt akinek
	        a bar.org volt a domain-je.
NICK_HELP_LIST_OPERSONLY
	Csak IRC operátorok használhatják ezt a parancsot.

NICK_HELP_LISTEMAIL
	Syntax: %s
	
	Listázza a regisztrált nicknevek mint a LIST parancs, de
	kiírja az E-mail címeket is és a user@host címeket.
	Tekintsd meg a LIST parancs helpjét (/msg %S HELP LIST).

NICK_HELP_STATUS
	Syntax: STATUS név...
	
	A következõket kapod, ha a nicknévre, amit a felhasználó használ
	azonosította magát mint tulajdonos. Az eredmény formája a következõ:
	
	    nicknév státusz-kód
	
	ahol a név az a név amit küldtél a parancsban, és
	a státusz-kód a következõ lehet:
	
	    0 - nincs ilyen felhasználó aktív vagy a név nem regisztrált
	    1 - a felhasználó nem azonosított, mint a név tulajdonosa
	    2 - A felhasználó azonosított, mint tulajdonos csak a hozzáférési lista alapján
	    3 - A felhasználó azonosított, mint tulajdonos a jelszava alapján
	
	
NICK_HELP_AJOIN
	Syntax: AJOIN ADD csatorna
	        AJOIN DEL csatorna
	        AJOIN LIST
	
	Módosítja vagy megjeleníti az nevedhez tartozó autojoin listát.
	Ez a lista azon csatornákat tartalmazza, melyekbe automatikusan
	be akarsz lépni, ennek akkor van hatása, ha már beazonosítottál a %S felé.
	Csak olyan csatornákat helyezhetsz el az autojoin listán, melyek
	"#" jellel kezdõdnek (helyi csatornák nem adhatóak hozzá).
	
NICK_HELP_AJOIN_END
	Megjegyzés: az IRC server megtilthatja, hogy belépj néhány csatornára
	az autojoin listádról, mint például melyek csatornajelszóval védettek,
	(mode +k) vagy csak meghívásra lehet belépni (mode +i).
NICK_HELP_AJOIN_END_CHANSERV
	Megjegyzés: az IRC server megtilthatja, hogy belépj néhány csatornára
	az autojoin listádról, mint például melyek csatornajelszóval védettek,
	(mode +k) vagy csak meghívásra lehet belépni (mode +i).
	Habár, ha van hozzáférésed a %s INVITE parancsához egy regisztrált
	csatornán, akkor a %S automatikus meg fog hívni a csatornára téged.

NICK_OPER_HELP_COMMANDS
	
	A következõ parancsok állnak a Services adminok rendelkezésére:
	
	    DROPNICK     Név droppolása
NICK_OPER_HELP_COMMANDS_DROPEMAIL
	    DROPEMAIL    Dropol minden nevet, aminél szerepel a
	                     megadott E-mail cím
NICK_OPER_HELP_COMMANDS_GETPASS
	    GETPASS      Név jelszavának megtekintése
	                     (csak, ha az encryption nem aktív)
NICK_OPER_HELP_COMMANDS_FORBID
	    FORBID       Egy név használatának/regisztrálásának letiltása
	    SUSPEND      Egy név használatának felfüggesztése
	    UNSUSPEND    Felfüggesztés eltávolítása egy névrõl
NICK_OPER_HELP_COMMANDS_SETAUTH
	    SETAUTH      Új élesítési kód megadása
	    GETAUTH      Élesítési kód megtekintése
	    CLEARAUTH    Élesítési kód törlése
NICK_OPER_HELP_COMMANDS_END
	
	Services adminok használhatják a következõ parancsokat is bármelyik
	nicknéven anélkül, hogy be kellenne azonosítaniuk magukat jelszóval:
	
	    SET, ACCESS LIST, INFO ALL
	
	Nézd meg a parancsokhoz tartozó helpet a különleges részletekért.

NICK_OPER_HELP_DROPNICK
	Syntax: DROPNICK név
	
	Törli az név regisztrációját a %S adatbazisából, és természetesen
	minden hozzálinkelt nevet is
	
	Korlátozva Services adminoknak.

NICK_OPER_HELP_DROPEMAIL
	Syntax: DROPEMAIL példa
	        DROPEMAIL-CONFIRM példa
	
	Dropol minden nevet, aminél szerepel az E-mail cím, melyet
	megadtál a példában.  Ha a példa egy egyszerû karakter:
	"-", dropol minden nicknevet, melynél nincs E-mail cím társítva.
	
	Hogy megelõzzük a véletlenszerû név dropolásokat ezen a parancson
	keresztül, kétszer kell megadni: elõször használva a DROPEMAIL
	parancsot majd használva a DROPEMAIL-CONFIRM ugyanazzal a példával.
	A DROPEMAIL-CONFIRM parancs meg kell adni %s és ugyannak a személynek,
	aki kiadta a DROPEMAIL parancsot. Ha több mint 4 DROPEMAIL parancs van
	megadva egy sorban a legrégebbi elfelejtõdik és továbbiakban nem fog
	reagálni a DROPEMAIL-CONFIRM parancsra.
	
	Korlátozva Services adminoknak.

NICK_OPER_HELP_SET
	
	Services adminok használhatják a NOEXPIRE (nem elévülõ) opciót is,
	azokon a neveken, amelyeket meg akarnak õrizni az elévüléstõl.
	Mellesleg, Services admins minden set opciót használhatnak bármelyik
	néven jelszó beírása nélkül, a következõ forma használatával:
	SET !nicknév opció paraméterek.

NICK_OPER_HELP_SET_NOEXPIRE
	Syntax: SET [!nicknév] NOEXPIRE {ON | OFF}
	
	Beállítható, ha a megadott nick elévülõ. Az ON opcióval
	megelõzheted, hogy a nicknév elévüljön. Ha nincs megadott név,
	akkor a nem-elévülõ beállítás a saját neveden hajtja végre.
	
	Korlátozva Services adminoknak.


NICK_OPER_HELP_UNSET
	Services adminok használhatják az UNSET parancsot bármelyik néven
	a !nicknév paramétert használva az opciónév elõtt, lásd a
	SET parancsnál.

NICK_OPER_HELP_OLD_UNLINK
	Services adminok unlinkelhetik bármelyik nicket anélkül, hogy beírnák a
	jelszavát.

NICK_OPER_HELP_UNLINK
	Syntax: UNLINK név [FORCE]
	
	Eltávolítja a linket (aliast) amit a nevedhez a LINK paranccsal hoztál
	létre. Nem unlinkelheted azt a nevet, amit éppen használsz; elõször
	változtas egy másik linkelt névre mielõtt használod ezt a parancsot.
	
	Ha a "fõ nicked" unlinkeled, akkor a SET MAINNICK paranccsal,
	a jelenlegi neved lesz automatikusan a fõ nick.
	
	Services adminok hozzáadhatják a FORCE opciót, amely lehetõvé teszi
	más név linkelésének eltávolítását, mint ami a sajátjuk.

NICK_OPER_HELP_OLD_LISTLINKS
	Syntax: LISTLINKS név
	
	Listázza a megadott névhez linkelt neveket.
	
	Korlátozva Services adminoknak.

NICK_OPER_HELP_LISTLINKS
	Syntax: LISTLINKS [név]
	
	Listázza az összes nevet amit a nevedhez linkeltél. A fõ neved
	a linkelt nevek csoportjában jelezve lesz a "*" jellel a baloldalán
	a névnek. Services operátorok listázhatják más nevekhez
	tartozó linkeket meghatározva a parancsban az adott nevet.

NICK_OPER_HELP_ACCESS
	Services adminok megadhatnak névhez paramétert (a LIST
	paranccsal megjelenithetõ) az access lista bármelyik névhez.

NICK_OPER_HELP_INFO
	Services adminok használhatják az ALL paramétert bármelyik névhez.

NICK_OPER_HELP_LISTCHANS
	Services adminok listázhatják a csatornákat bármelyik névhez
	megadva a nevet a parancsban (például, /msg %S LISTCHANS MásikNév).

NICK_OPER_HELP_LIST
	Syntax: LIST [+start] sablon [FORBIDDEN] [NOEXPIRE] [SUSPENDED]
	
	Listáz minden regisztrált nevet, amely vagy név vagy az utoljára
	látott address (user@host formatumban), amely egyezik a megadott
	sablonnal. Nickek PRIVATE opcióval beállítva csak Services adminoknak
	jelennek meg. Services adminoknak, a nickek NOEXPIRE opcióval
	beállítva ! megelõzve (prefix) jelennek meg azok pedig, amelyek
	használata fel lett függesztve * megelõzve (prefix) fognak
	megjelenni. Pl: !név *név
	
	Ha FORBIDDEN, SUSPENDED vagy NOEXPIRE opció meg van adva,
	csak azok a nevek amelyek, kölcsönösen forbidoltak és suspendeltek, vagy
	a NOEXPIRE opció beállítás megjelenik. Ezek az opciók használhatóak
	bármilyen sorrendben vagy kombinációban.
	
	Korlátozva Services adminoknak.
	
	Példák:
	    LIST joeuser@foo.com
	       Listázza az összes regisztrált nicknevet, amelyet utoljára
	          a joeuser@foo.com címrol használtak.
	    LIST *Bot*
	       Listázza az összes regisztrált nicknevet, amelyben szerepel
	          a Bot részlet.
	    LIST * NOEXPIRE
	       Listázza az összes regisztrált nicknevet, amely
	          nem elévülõre van beállítva

NICK_OPER_HELP_LIST_AUTH
	Syntax: LIST [+start] sablon [FORBIDDEN] [NOEXPIRE] [SUSPENDED] [NOAUTH]
	
	Listázza az összes olyan regisztrált nevet vagy utoljára használt
	címet (user@host formátumban), ami megegyezik a megadott sablonnal.
	Azok a nickek, amelyeken PRIVATE beállítás van csak Services
	adminoknak fognak megjelenni. A Services adminoknak, azon nickek elõtt,
	amelyeken NOEXPIRE beállítás van ! (prefix) lesz a listában,
	amelyek használata fel lett függesztve azok elõtt pedig * (prefix)
	lesz. Például: !nick, *nick
	És azon nevek elõtt, amelyek még nem lettek élesítve (az AUTH parancs
	használatával) ? (prefix) lesz. Például: ?nick
	
	Ha a FORBIDDEN, SUSPENDED, NOEXPIRE, vagy NOAUTH beállítás van rajta,
	akkor azok a nickek, amelyek kölcsönösen forbidoltak és suspendeltek,
	NOEXPIRE van beállítva rájuk, vagy még nem élesítettek fognak megjelenni.
	A beállítások bármilyen sorrendben vagy kombinációban használhatóak.
	
	Korlátozva Services adminoknak.
	
	Példák:
	
	    LIST joeuser@foo.com
	       Listázza az összes regisztrált nicknevet, amelyet utoljára
	          a joeuser@foo.com címrõl használtak.
	    LIST *Bot*
	       Listázza az összes regisztrált nicknevet, amelyben szerepel
	          a Bot részlet
	    LIST * NOEXPIRE
	       Listázza az összes regisztrált nicknevet, amely
	          nem elévülõre van beállítva

NICK_OPER_HELP_LIST_END
	
	Ha a start paraméter meg van adva, meghatározza, hogy mennyi
	találatot ugorjon át mielõtt megjeleníti az eredményeket
	(például, a +50 esetén csak az elsõ 50 találatot ugorja át).
	
	Példák:
	
	    LIST joeuser@foo.com
	       Listázza az összes regisztrált nicknevet, amelyet utoljára
	          a joeuser@foo.com címrõl használtak.
	    LIST *Bot*
	       Listázza az összes regisztrált nicknevet, amelyben szerepel
	          a Bot részlet.
	    LIST * NOEXPIRE
	       Listázza az össze regisztrált nicknevet, amely
	          nem elévülõre van beállítva


NICK_OPER_HELP_GETPASS
	Syntax: GETPASS nicknév
	
	Megadja a nicknév jelszavát.  Megjegyzés amikor ezt a parancsot
	használod, egy üzenetben elküldi a szervíz, hogy ki használta
	és melyik nicknéven ezt a parancsot. Bekerül a naplófájlba és
	egy WALLOPS/GLOBOPS üzenetbe.
	
	Ha az encryption használatban van, ez a parancs nem elérhetõ,
	de ez függ attól is, hogy milyen típusú az encryption.
	
	Korlátozva: Services adminoknak.

NICK_OPER_HELP_FORBID
	Syntax: FORBID nicknév
	
	Letiltja egy nicknév regisztrálását és használatát.
	Eltûnik, olyan mintha droppolják a nicket.
	
	Korlátozva: Services adminoknak.

NICK_OPER_HELP_SUSPEND
	Syntax: SUSPEND [+elévülés] nicknév indok
	
	Megelõzi, hogy egy adott nicknevet és minden hozzá linkelt
	nicknevet használjanak, beazonosítsanak rá. A nicknév
	unsuspendelhetõ az UNSUSPEND paranccsal, ami lehetõvé teszi,
	hogy továbbra is használják, mint korábban.
	
	Az elévülés paraméter ugyanolyan, mint az
	%s AKILL parancs esetében.
	
	Korlátozva: Services adminoknak.

NICK_OPER_HELP_UNSUSPEND
	Syntax: UNSUSPEND nicknév
	
	Törli egy adott nicknév korábbi suspendjét és lehetõvé teszi
	újra a használatát.
	
	Korlátozva: Services adminoknak.

NICK_OPER_HELP_AJOIN
	
	A Services adminok listázhatják bármelyik nickhez tartozó autojoin listát
	ezzel a formátummal LIST nicknév.

NICK_OPER_HELP_SETAUTH
	Syntax: SETAUTH nicknév
	
	Beállít egy új élesítési kódot a megadott nicknévhez.
	A nicknév tulajdonosának használnia kell az AUTH parancsot
	a nicknév használatához mielõtt újra beazonosíthat.
	Ha a nicknév jelenleg is használatban van, egy notice üzenetet küld
	a nicknév használójának a szervíz és nem használhatja többet a nicknevet
	beazonosítás nélkül.
	
	Korlátozva: Services adminoknak.
	
NICK_OPER_HELP_GETAUTH
	Syntax: GETAUTH nicknév
	
	Kiírja a nicknévhez tartozó élesítési kódot,
	ha már van megadva.
	
	Korlátozva: Services adminoknak.

NICK_OPER_HELP_CLEARAUTH
	Syntax: CLEARAUTH nicknév
	
	Töröl minden a nicknévhez tartozó élesítési kódot,
	lehetõvé téve a nick normális használatát.
	
	Korlátozva: Services adminoknak.


###########################################################################
#
# ChanServ help messages
#
###########################################################################

# Used in OP/VOICE/etc for required access levels for commands
CHAN_HELP_REQSOP_LEVXOP
	level 100 (SOP)
CHAN_HELP_REQSOP_LEV
	level 100
CHAN_HELP_REQSOP_XOP
	SOP
CHAN_HELP_REQAOP_LEVXOP
	level 50 (AOP)
CHAN_HELP_REQAOP_LEV
	level 50
CHAN_HELP_REQAOP_XOP
	AOP
CHAN_HELP_REQHOP_LEVXOP
	level 40 (HOP)
CHAN_HELP_REQHOP_LEV
	level 40
CHAN_HELP_REQHOP_XOP
	HOP
CHAN_HELP_REQVOP_LEVXOP
	level 30 (VOP)
CHAN_HELP_REQVOP_LEV
	level 30
CHAN_HELP_REQVOP_XOP
	VOP

# A Valódi help üzenetek itt kezdõdnek
CHAN_HELP
	A %S lehetõséget ad számodra, hogy a csatornád regisztrálhasd.
	A %S használatával elkerülhetõ a szobádban a "taking over"
	jelenség, amit az opjoggal visszaélõ felhasználók okozhatnak.
	Meghatározható lesz, hogy ki kaphasson a csatornán operátori kiváltságokat.
	Írd be: /msg %S HELP COMMANDS, hogy megkapd a rendelkezésedre
	álló parancsok és lehetõségek listáját, ha a %S -et használod;
	a parancsok használatához írd be: /msg %S parancs
	vagy ha több információra van szükséged a paranccsal
	kapcsolatban, akkor írd be: /msg %S HELP parancs.
CHAN_HELP_EXPIRES
	
	Megjegyzés: azok a csatornák, amelyeket nem használnak %s
	idõ után, automatikusan elvesztik a regisztrációjukat.

CHAN_HELP_COMMANDS
	A következõ parancsok használhatók a %S segítségével:
	
	    REGISTER   Csatorna regisztrálása
	    IDENTIFY   Azonosíthatod magad a jelszavaddal
	    DROP       Törölheted a csatornád regisztrációját
	    SET        Beállíthatod a csatornád opcióit
	    UNSET      Törölheted a csatorna beállításkat
	    INFO       Megtekintheted a csatorna információit és beállításait
CHAN_HELP_COMMANDS_LIST
	    LIST       Listázhatod a regisztrált csatornákat
CHAN_HELP_COMMANDS_AKICK
	    AKICK      Autokirúgás listára teheted a "rossz" usereket
CHAN_HELP_COMMANDS_LEVELS
	    ACCESS     A szoba hozzáférési listájának módosítása
	    LEVELS     Átállíthatod az operátori privilégiumok szintjeit
CHAN_HELP_COMMANDS_XOP
	    SOP        SzuperOperátor lista beállítása
	    AOP        AutoOperátor lista beállítása
CHAN_HELP_COMMANDS_HOP
	    HOP        Autoféloperátorok listájának beállítása
CHAN_HELP_COMMANDS_XOP_2
	    VOP        AutoHangadók listájának beállítása
	    NOP        Azon tagok listája, akik soha nem lehetnek opok
CHAN_HELP_COMMANDS_OPVOICE
	    OP         Ideiglenes operátor státusz adása (+o)
	    DEOP       Ideiglenes operátor státusz elvétele (-o)
	    VOICE      Ideiglenes hangadás státusz adása(+v)
	    DEVOICE    Ideiglenes hangadás státusz elvétele(-v)
CHAN_HELP_COMMANDS_HALFOP
	    HALFOP     Ideiglenes féloperátor státusz adás (+h)
	    DEHALFOP   Ideiglenes féloperátor státusz elvétele (-h)
CHAN_HELP_COMMANDS_PROTECT
	    PROTECT    Védettség adása(+a)
	    DEPROTECT  Védettség elvétele (-a)
CHAN_HELP_COMMANDS_INVITE
	    INVITE     Meghívhatod magad a csatornára (pl: +i chmode estére hasznos)
	    UNBAN      Leveheted a tiltást magadról a csatornán
	    KICK       Kirúghatsz egy usert a csatornáról
	    TOPIC      Megváltoztathatod a topic-ot
	    CLEAR      Leveszi a szobamódokat vagy eltávolít minden usert
	    STATUS     Megkaphatod egy user hozzáférési szintjét a csatornán

CHAN_HELP_REGISTER
	Syntax: REGISTER szobanév jelszó leírás
	
	A csatornádat regisztrálja a %S adatbázisába.
	Hogy kiadhasd ezt a paracsot operátornak kell lenned
	az adott csatornán.
	A jelszót használhatod az IDENTIFY parancs segitségével
	azonosításra, mielõtt beállításokat eszközölsz a késõbbiekben
	az adott csatonán.
	
	Az utolsó paraméternek,tartalmaznia kell
	a csatorna általános meghatározását.
	
	Amikor regisztrálsz egy csatornát, te leszel a "founder".
	A csatorna founderjének van lehetõsége a szoba beállításokat
	a hozzáférési listát, az autókirúgás listát, az infókat
	megváltoztatni; a %S automatikusan megadja belépéskor a
	founder hozzáférési jogot és csatorna operátori jogokat.
	
	Megjegyzés: Ha regisztrálsz egy csatornát, elõször regisztrálnod kell
	a nickneved és azonosítanod kell magad(/ns identify jelszó)
	Ha még nem nézted meg, /msg %s HELP.
CHAN_HELP_REGISTER_ADMINONLY
	
	Csak a Services adminok regisztrálhatnak szobát.

CHAN_HELP_IDENTIFY
	Syntax: IDENTIFY csatorna jelszó
	
	A %S a parancs beírása utána founder szintet ad a csatornán.
	Több parancs szükséges, hogy használhasd ezt a parancsot.
	A jelszónak egyeznie kell a regisztrációkor megadott jelszóval.

CHAN_HELP_DROP
	Syntax: DROP csatorna jelszó
	
	Törli a megnevezett csatorna regisztrációját.
	Csak a csatorna founder használhatja. Meg kell adnod a jelszót,
	hogy elkerüljük a véletlen törléseket.

CHAN_HELP_SET
	Syntax: SET csatorna opció paraméterek
	
	Lehetõséget ad a foundernek, hogy a beállításokat
	és információkat megváltotassa.A foundernek az IDENTIFY
	parancsot kell használnia mielõtt a SET paracsokat használná.
	
	Elérhetõ opciók:
	
	    FOUNDER     A founder megváltoztatása
	    SUCCESSOR   A successor megadása
	    PASSWORD    A founder jelszó változtatása
	    DESC        A szoba meghatározás beállítása
	    URL         Egy URL társítása a szobához
	    EMAIL       Egy E-mail cím társítása a szobához
	    ENTRYMSG    A belépõüzenet állítása
	    KEEPTOPIC   A topic megtartása, ha a szoba nincs használatban
	    TOPICLOCK   Topic lezárása (topicváltás: /cs set #szoba topic szöveg)
	    MLOCK       Szoba módok lezárása (+flag -flag)
	    PRIVATE     A szoba elrejtése a listából
	    RESTRICTED  Korlátozott hozzáférés
	    SECURE      Aktiválja a %S biztonsági lehetõségeit
	    SECUREOPS   A csatornán opadás szigorú kezelése
	    LEAVEOPS    Nem veszi el az op-ot a szobanyitótól
	    OPNOTICE    Üzenetet küld az OP/VOICE parancsok használtáról
	    ENFORCE     Megvédi az auto-op, auto-voice státuszt (nincs deop, devoice)
	    MEMO-RESTRICTED
	                Korlátozza, hogy ki küldhet memo üzenetet a csatornára
	
	További információért egy adott opcióról írd be:
	/msg %S HELP SET opció.

CHAN_HELP_SET_FOUNDER
	Syntax: SET csatorna FOUNDER nick
	
	Megváltoztatja a csatorna founderjét.
	Az új név regisztrált kell, hogy legyen

CHAN_HELP_SET_SUCCESSOR
	Syntax: SET csatorna SUCCESSOR nick
	
	MEgváltoztatja a successort a csatornán. Ha a founder neve
	elévül vagy törlõdik, de a csatorna még regisztrált,
	a successor lesz a csatorna új founderje.
	Ha a successornak már túl sok regisztrált
	csatornája van (%d), a csatorna törlõdik, azaz
	megszünik a regisztrációja, akkor is ez történik,
	ha nem volt megadott successor.
	A nicknek regisztrálnak kell lennie.

CHAN_HELP_SET_PASSWORD
	Syntax: SET csatorna PASSWORD jelszó
	
	Beállítja a founder jelszót a csatornára, amivel
	a founder azonosíthatja magát.

CHAN_HELP_SET_DESC
	Syntax: SET csatorna DESC meghatározás
	
	Beállíthatod a szoba általános meghatározását,
	ami látszódni fog a LIST és INFO parancsok használatakor.

CHAN_HELP_SET_URL
	Syntax: SET csatorna URL url
	
	Egy URL társítása a szobához. Ha van honlapja a szobának,
	akkor azt bárki megtudhatja, ha lekérte a szobáról az INFO-t.

CHAN_HELP_SET_EMAIL
	Syntax: SET csatorna EMAIL E-mail cím
	
	Egy E-mail cím társítása a szobához. Ha van E-mail címe
	a szobának, a foundernek; akkor azt bárki
	megtudhatja, ha lekérte a szobáról az INFO-t.

CHAN_HELP_SET_ENTRYMSG
	Syntax: SET csatorna ENTRYMSG belépõ üzenet
	
	Ez egy üzenet, amit minden belépõ user megkap /NOTICE
	formájában, aki belép belép a szobába.

CHAN_HELP_SET_KEEPTOPIC
	Syntax: SET csatorna KEEPTOPIC {ON | OFF}
	
	Engedélyezi vagy letiltja a topic megtartása opciót a
	csatorna részére. Ha a topic megtartása be van állítva,
	a topicot a %S megjegyzi miután az utolsó user elhagyja a szobát
	és visszaállítja a következõ megnyitáskor.

CHAN_HELP_SET_TOPICLOCK
	Syntax: SET channel TOPICLOCK {ON | OFF}
	
	Engedélyezi vagy letiltja a topic lezárása opciót a csatornán.
	Ha a topic lezárása be van kapcsolva, a %S nem engedélyezi a
	csatornán a topic változtatását a TOPIC paranccsal. Csak a
	/cs set #szobanév topic szöveg paranccsal állítható a topic.

CHAN_HELP_SET_MLOCK
	Syntax: SET csatorna MLOCK módok
	
	Beállíthatod a mode-lock paramétert a csatornán. A %S
	lehetõséget nyújt arra, hogy meghatározz olyan csatorna módokat,
	amik mindig be (vagy ki)lesznek kapcsolva.
	Minden bináris mód (n, s, t, k, l, és így tovább)
	rögzíthetõ; néhány mód rögzíthetõ, a rögzítés levehetõ, vagy nem
	rögzíthetõ. A módok felrakhatóak a /MODE paranccsal is;
	azok a módok amelyek a + jel után vannak rögzítõdnek,
	és azok a módok amelyek a - jel után azok nem tehetõk fel a szobára.
	
	Figyelmeztetés:  Ha beállítasz egy mode-lock alatti kulcsot,
	mint azt a második példában látod, akkor be kell állítanod a
	RESTRICTED opciót a csatornán (lásd HELP SET RESTRICTED)
	vagy bárki beléphet a szobába amikor üres és lehetõsége lesz
	meglátni a kulcsot!  MLOCK +i nem elõzi meg, hogy belépjenek
	a szobádba amikor üres, meghívás nélkül.
	
	Példák:
	
	    SET #csatorna MLOCK +nt-ikl
	        Az  "n" és a "t" állandóan fent van, és az "i", "k", és "l" állandóan.
	        ki lesz kapcsolva. Minden más mód felrakása-levétele szabad.
	
	    SET #csatorna MLOCK +knst-ilmp my-key
	        A "k", "n", "s", és "t" állandóan fent van, és az "i", "l", "m",
	        és a "p" ki lesz kapcsolva. A szobába való belépéshez szküséges jelszó:
	        "my-key".
	
	    SET #channel MLOCK +
	        Eltávolítja az összes lockolt módot.
	        Minden mód szabadon ki/bekapcsolható.

CHAN_HELP_SET_HIDE
	Syntax: SET csatorna HIDE {EMAIL | TOPIC | MLOCK} {ON | OFF}
	
	Lehetõséget nyújt a szoba információnak lekérésekor kiírásra
	kerülõ adatok közül az email, a topic, az mlock elrejtésére.
	A topic, akkor sem látszik ha a szoba rejtett (+s).

CHAN_HELP_SET_PRIVATE
	Syntax: SET csatorna PRIVATE {ON | OFF}
	
	Engedélyezi vagy letiltja a private opciót a csatornán.
	Amikor a private be van kapcsolva, a /msg %S LIST parancs
	által kilistázott csatornák között nem szerepel a csatorna.

CHAN_HELP_SET_RESTRICTED
	Syntax: SET csatorna RESTRICTED {ON | OFF}
	
	Engedélyezi vagy letiltja a restricted access opciót a
	csatornán. Amikor a restricted access be van kapcsolva,
	a userek nem kaphatnak csatorna operátori jogokat
	(userek negativ access szinttel és, ha a secure ops
	opció be van kapcsolva, a userek, akik nem szerepelnek
	az access listán) azonnal ki lesznek rúgva és tiltva.

CHAN_HELP_SET_SECURE
	Syntax: SET csatorna SECURE {ON | OFF}
	
	Engedélyezi vagy letitlja a %S biztonsági lehetõségeit
	a szobában. Ha a SECURE be van kapcsolva, csak azok a felhasználók
	akik regisztrált névvel rendelkeznek és beazonosítottak a
	%s jelszavukkal használták a IDENTIFY'd parancsot csak azok
	kaphatnak a szobában privilégiumokat, a hozzáférési listán.
	Ha a RESTRICTED opció be van állítva akkor a felhasználók,
	akik szerepelnek a hozzáférési listán azok sem tudnak belépni,
	amíg nem azonosítottak be a %s -be.

CHAN_HELP_SET_SECUREOPS
	Syntax: SET szoba SECUREOPS {ON | OFF}
	
	Bekapcsolja vagy letiltja a secure ops opciót a szobában.
	Amikor a secure ops be van kapcsolva, akkor azok akik nem
	szerepelnek a hozzáférési listán nem kaphatnak szoba
	operátori státuszt.

CHAN_HELP_SET_LEAVEOPS
	Syntax: SET csatorna LEAVEOPS {ON | OFF}
	
	Engedélyezi vagy letiltja a leave ops opciót a szobában.
	Ha a leave ops be van állítva, %S meghagyja a szerver opot
	(+o az elsõ belépõnek a szobába) azokon a helyeken, ahol a felhasználó
	nem lenne normálisan auto-opolva.
	A SECUREOPS opció továbbra is végre lesz hajtva.
	Megjegyzés:  Így lehetõséget adsz az op "lopásra" egy netsplit esetén.

CHAN_HELP_SET_OPNOTICE
	Syntax: SET csatorna OPNOTICE {ON | OFF}
	
	Be/kikapcsolja az op-notice opciót a csatornán.
	Amikor az op-notice be van kapcsolva, a %S megjegyzést
	fog küldeni a csatornára, amikor a felhasznalók használják
	az OP, VOICE, vagy a hozzájuk kapcsolódó parancsokat a
	csatornán.

CHAN_HELP_SET_ENFORCE
	Syntax: SET csatorna ENFORCE {ON | OFF}
	
	Be/kikapcsolja az enforce opciót a csatornán.
	Amikor az enforce be van kapcsolva, a %S nem engedélyezi az auto-opok
	és auto-voice-osok  jogfosztását.  Például, ha egy usert
	aki auto-op deoppolnak, %S automatikusan oppolja a usert, és a %S nem
	engedélyezi a ChanServ használatával történõ deoppolását a usernek.

CHAN_HELP_SET_MEMO_RESTRICTED
	Syntax: SET csatorna MEMO-RESTRICTED {ON | OFF}
	
	Engedélyezi vagy letiltja a korlátozott memo üzenetek opciót
	a csatornán.  Ha a memo korlátozás érvényben van, csak
	azok a userek tudnak üzenetet küldeni a csatornára, akik
	rendelkeznek a szükséges privilégiumokkal.

CHAN_HELP_UNSET
	Syntax: UNSET csatorna {SUCCESSOR | URL | EMAIL | ENTRYMSG}
	
	Törli a megadott beállítást (Successor, URL, E-mail, Belépõ üzenet) a csatornán.

CHAN_HELP_SOP
	Syntax: SOP csatorna ADD nick
	        SOP csatorna DEL nick
	        SOP csatorna LIST [+start] [maszk]
	        SOP csatorna COUNT
	
CHAN_HELP_SOP_MID1
	SuperOp (SOP) lista a csatornán. Az SOP státusz
	a szobába való belépéskor automatikus oppolással (set mode +o)
	és automatikus védelemmel jár (set mode +a)
CHAN_HELP_SOP_MID1_CHANPROT
	SuperOp (SOP) lista a csatornán. Az SOP státusz
	a szobába való belépéskor automatikus oppolással (set mode +o)
	és automatikus védelemmel jár (set mode +a).
CHAN_HELP_SOP_MID2
	
	Az SOP módosíthatja az AOP, VOP, és NOP listát.  Csak a
	csatorna founderje módosíthatja az SOP listát.
CHAN_HELP_SOP_MID2_HALFOP
	
	Az SOP  módosíthatja az AOP, HOP, VOP, és NOP listát.  Csak a
	csatorna founderje módosíthatja az SOP listát.

CHAN_HELP_SOP_END
	CHAN_HELP_SOP_END
	
	Az ADD, DEL, LIST, és COUNT alparancsok funkciója
	ugyanúgy mûködik, mint az AOP parancsoknálsubcommands function the
	
	Az SOP LIST parancs megadja az SOP listát.
	Ha nem vagyunk kiváncsiak a teljes listára akkor a listában szereplõ
	Sop sorszámát megadva is megtudhatjuk a nevét, megadhatunk intervallumot is.
	
	Példák:
	
	   SOP #csatorna LIST 2-5,7-9
	      Listázza az SOP-okat 2 és 5 között és
	      7-tõl  9-ig.
	
	Az SOP COUNT parancs megadja, hogy hány darab Sop szerepel a listán

CHAN_HELP_AOP
	Syntax: AOP  ADD nick
	        AOP csatorna DEL nick
	        AOP csatorna LIST [maszk]
	        AOP csatorna COUNT
	
	AOP lista a csatornán. Az AOP státusz
	a szobába való belépéskor automatikus oppolással (set mode +o)  jár.
CHAN_HELP_AOP_MID
	
	Az AOP módosíthatja a VOP és NOP listát; SOP-ok módosíthatják az AOP
	listát.
CHAN_HELP_AOP_MID_HALFOP
	
	Az AOP módosíthatja a HOP, VOP, és NOP listát; SOP-ok
	módosíthatják az AOP listát.
CHAN_HELP_AOP_END
	
	Az AOP LIST parancs megadja az AOP listát.
	Ha nem vagyunk kiváncsiak a teljes listára akkor a listában szereplõ
	Aop sorszámát megadva is megtudhatjuk a nevét, megadhatunk intervallumot is.
	
	Példák:
	
	   AOP #csatorna LIST 2-5,7-9
	      Listázza az AOP-okat 2 és 5 között és
	      7-tõl  9-ig.
	
	Az AOP COUNT parancs megadja, hogy hány darab Aop szerepel a listán

CHAN_HELP_HOP
	Syntax: HOP csatorna ADD nick
	        HOP csatorna DEL nick
	        HOP csatorna LIST [maszk]
	        HOP csatornal COUNT
	
	HOP lista a csatornán. A HOP státusz
	a szobába való belépéskor automatikus félop joggal  (set mode +h)  jár.
	
	A HOP módosíthatja a VOP és NOP listát; SOP-ok és AOP-ok
	módosíthatják a HOP listát.
	
	A HOP LIST parancs megadja az HOP listát.
	Ha nem vagyunk kiváncsiak a teljes listára akkor a listában szereplõ
	hop sorszámát megadva is megtudhatjuk a nevét, megadhatunk intervallumot is.
	
	Példák:
	
	   HOP #csatorna LIST 2-5,7-9
	      Listázza az HOP-okat 2 és 5 között és
	      7-tõl  9-ig.
	
	A HOP COUNT parancs megadja, hogy hány darab Hop szerepel a listán

CHAN_HELP_VOP
	Syntax: VOP csatorna ADD nick
	        VOP csatorna DEL nick
	        VOP csatorna LIST [mask]
	        VOP csatorna COUNT
	
	VOP lista a csatornán. A VOP státusz
	a szobába való belépéskor automatikus hangadási joggal  (set mode +v)  jár.
	
	A VOP nem módosíthat listát; SOP-ok, AOP-ok és HOP-ok módosíthatják a
	VOP listát.
	
	A VOP LIST parancs megadja az VOP listát.
	Ha nem vagyunk kiváncsiak a teljes listára akkor a listában szereplõ
	Vop sorszámát megadva is megtudhatjuk a nevét, megadhatunk intervallumot is.
	
	Példák:
	
	   VOP #csatorna LIST 2-5,7-9
	      Listázza az VOP-okat 2 és 5 között és
	      7-tõl  9-ig.
	
	A VOP COUNT parancs megadja, hogy hány darab Vop szerepel a listán

CHAN_HELP_NOP
	Syntax: NOP csatorna ADD nicknév
	        NOP csatornal DEL nicknév
	        NOP csatorna LIST [+start] [maszk]
	        NOP csatorna COUNT
	
	Módosíthatod a  NeverOp (NOP) listát a csatornán.
	Azok a felhasználók, akiknek NOP státuszuk van, nem kaphatnak csatorna
	operátori (mode +o) státuszt, és a ChanServ el fogja venni a  mode +o tõlük,
	hogyha valaki mégis adna nekik opot.
	
	Az ADD, DEL, LIST, és COUNT alparancs funkciók ugyanúgy mûködnek,
	mint az AOP parancsnál.

CHAN_HELP_ACCESS
	Syntax: ACCESS csatorna ADD nick szint
	        ACCESS csatorna DEL {nick | sorszám | lista}
	        ACCESS csatorna LIST [maszk | lista]
	        ACCESS csatorna COUNT
	
	Access lista  a csatornán.  A hozzáférési lista (access list)
	meghatározza, hogy mely userek kaphatnak op státuszt vagy
	hozzáférést a %S parancsokhoz a csatornán. Különbözõ
	user szintek lehetõséget nyújtanak arra, hogy eltérõ jogokat kaphassanak;
	bõvebb információért, írd be: /msg %S HELP ACCESS LEVELS.
	Minden nick amely nem szerepel az access listán  0-ás szintû.
	
	Az ACCESS ADD paranccsal hozzáadható a megadott nicknév az
	access listához a megadott user szinttel; ha a nick már szerepel a listán,
	akkor a hozzáférési szintje változik csak meg. A szint amit meghatározol,
	kisebb kell, hogy legyen mint a te szinted és ha a nick már szerepel a listán,
	akkor a jelenlegi szintjánek a tiédnél alacsonyabbnak kell lennie.
	
	Az ACCESS DEL parancs törli a megadott nicknevet az
	access listáról.  Ha intervallumot adsz meg,akkor minden azon intervallumban szerelõ
	nicknevet törölsz.
	
	Az ACCESS LIST paranccsal megtekinthetõ az access lista.
	Ha intervallumot is megadsz, akkor csak abban az intervallumban szereplõ nicknevek
	listázása fog megtörténni
	
	   ACCESS #csatorna LIST 2-5,7-9
	      Listázza azokat a usereket, akik 2 és 5 között illetve
	      7-tõl 9-ig szerepelnek a listán.
	
	Az ACCESS COUNT parancs megadja, hány user szerepel az access listán
CHAN_HELP_ACCESS_XOP
	
	Az access lista könnyebben állítható az SOP, AOP, VOP, és
	NOP parancsokkal.
	
CHAN_HELP_ACCESS_XOP_HALFOP
	
	Az access lista könnyebben állítható az SOP, AOP, HOP, VOP,
	és NOP parancsokkal.

CHAN_HELP_ACCESS_LEVELS
	Felhasználói privilégiumok szintjei
	
	Alapból, a következõ privilégium szintek vannak definiálva:
	   Founder   Teljes hozzáférés a %S funkciókhoz; automatikus
	                 opjog a csatornára való belépéskor.  Megjegyzem,
	                 csak egy embernek lehet founder státusa (ez státus
	                 (nem adható az ACCESS paranccsal.
	      %4d   Hozzáférés az AKICK listához; automatikus operátori jog.
	      %4d   Automatikus operátori jog.
CHAN_HELP_ACCESS_LEVELS_HALFOP
	      %4d   Automatikus féloperátori jog.
CHAN_HELP_ACCESS_LEVELS_END
	      %4d   Automatikus hangadói jog.
	         0   Semmi különleges jog; opolható a többi
	                 op által (kivéve, ha a SECUREOPS be van kapcsolva).
	        -1   Nem opolható.
	      -100   Nemléphet be a csatornára.
	
	Ezek a szintek (kivéve az utolsó kettõt) változtathatóak, vagy újak
	adhatóak meg a LEVELS parancs használatával; a részletekért írd be
	/msg %S HELP LEVELS.

CHAN_HELP_LEVELS
	Syntax: LEVELS csatorna SET tipus szintl
	        LEVELS csatorna {DIS | DISABLE} tipus
	        LEVELS csatorna LIST
	        LEVELS csatorna RESET
	
	A LEVELS lehetõséget nyújt a számokkal definiált
	hozzáférési szintek módosításhoz. Ezzel a paranccsal,
	definiálhatsz hozzáférési szinteket a %S funkcióihoz.
	Ezt a parancsot csak a founder használhatja.

CHAN_HELP_LEVELS_XOP
	
	Figyelmeztetés: Ez a parancs megváltoztatja a usereket szintjét!
	Ennek a levelnek az állítása elõidézheti azt, hogy az SOP,
	AOP, VOP, és NOP parancsok nem fognak rendesen müködni.

CHAN_HELP_LEVELS_XOP_HOP
	
	Figyelmeztetés: Ez a parancs megváltoztatja a usereket szintjét!
	Ennek a levelnek az állítása elõidézheti azt, hogy az SOP,
	AOP, HOP, VOP, és NOP parancsok nem fognak rendesen müködni.
	
CHAN_HELP_LEVELS_END
	
	A LEVELS SET Engedélyezi a hozzáférési szintek állítását.
	A LEVELS DISABLE (vagy röviden a DIS) megszünteti egy automatikus
	lehetõség mûködését vagy megakadályozza a hozzáférést (a founderen kivül)
	minden usernek.
	A LEVELS LIST megmutatja a szintek jelenlegi beállítását.
	LEVELS RESET visszállítja a szinteket az eredetire.
	(Tekintsd meg a HELP ACCESS LEVELS -t).
	
	A lehetõségek és funkciók listájáért , tekintsd meg a HELP LEVELS DESC-et.

CHAN_HELP_LEVELS_DESC
	A következõ lehetõségek/funkciók állnak rendelkezésre:

CHAN_HELP_AKICK
	Syntax: AKICK csatorna ADD maszk [indok]
	        AKICK csatorna DEL maszk
	        AKICK csatorna LIST [maszk]
	        AKICK csatorna VIEW [maszk]
	        AKICK csatorna COUNT
	        AKICK csatorna ENFORCE
	
	Változtatható a csatorna autokirúgás listája.  Ha egy olyan felhasználó
	próbál meg belépni a csatornára aki szerepel az autokick listán, a %S kitiltja
	azt a felhasználót és aztán ki is rúgja.
	
	Az AKICK ADD parancs hozzáadja a user@host vagy a
	nick!user@host maszkot az autokick listához. Ha az indok meg
	van adva a parancsban, akkor ez lesz az indok ha nincs megadva, akkor
	az alapbeállítás szerinti indok fog megjelenni a kirugáskor.
	
	Az AKICK DEL parancs eltávolítja a megadott maszkot az
	autokirúgás listáról.  Ez csak akkor mûködik, ha minden az
	adott nickre, identre, hostra szoló tiltást, ami az autókirugás listán
	szerepel eltávolítunk; mindegyiket egyenként kell törölni.
	
	Az AKICK LIST parancs megjeleníti az autókirúgás listát, vagy
	opcionális esetben csak azokat jeleníti meg, amelyek egyeznek a megadott maszkkal.
	Az AKICK VIEW ugyanaz, de több részletet jelenít meg.
	
	Az AKICK COUNT parancs megmutatja, hogy hány hely foglalt az akick listán.
	
	Az AKICK ENFORCE parancs hatására a %S ellenõrzi az akick listát és
	eltávolít minden olyan usert akinek a maszkja szerepel a listán.
	
	Alapbeállítás alapján, limitálva a %s access szinttel vagy magasabb szinttel
	rendelkezõ felhasználók számára a szobában.

CHAN_HELP_INFO
	Syntax: INFO csatorna [ALL]
	
	Listázza az összes információt a megnevezett regisztrált csatornáról,
	mint a foundert, a regisztrálás idõpontját, az utolsó használat idejét
	a meghatározást, a mode lockot. Ha beazonosítottad magad a csatorna
	founderjének akkor teljes informaciót ALL kaphatsz: az entrymsg,
	a successor és az elrejtett információk is látszódni fognak.

CHAN_HELP_LIST
	Syntax: LIST [+start] pattern
	
	Listázza a regisztrált csatornákat.
	(Ahol a PRIVATE opció be van kapcsolva nem fog megjelenni a listában.)
	Ha a start paraméter is meg van adva, akkor a megadott számú találatot
	kihagyja és csak az utána lévõket jeleniti meg (például +50
	azt eredményezi, hogy az elsõ 50 találatot átugorja.)

CHAN_HELP_LIST_OPERSONLY
	
	Csak IRC operátorok használhatják.

CHAN_HELP_OP
	Syntax: OP csatorna [nick]
	
	Opolja a megadott nicket a csatornán.
	Ha nem adsz meg  nicket, magadat Oppolod.
	Alap beállítás szerint csak a %s accessén szereplõ userek használhatják.

CHAN_HELP_DEOP
	Syntax: DEOP csatorna [nick]
	
	DeOpolja a megadott nicket a csatornán.
	Ha nem adsz meg  nicket, magadat DeOppolod.
	Alap beállítás szerint csak a %s accessén szereplõ userek használhatják

CHAN_HELP_VOICE
	Syntax: VOICE csatorna [nick]
	
	Hangadóvá teszi a megadott nicket a csatornán.
	Ha nem adsz meg  nicket, magadat teszed hangadóvá.
	Alap beállítás szerint csak a %s accessén szereplõ userek használhatják

CHAN_HELP_DEVOICE
	Syntax: DEVOICE csatorna [nick]
	
	Elveszi a hangadási jogot a megadott nicktõl a csatornán.
	Ha nem adsz meg  nicket, magadtól vonod meg a hangadási jogot.
	Alap beállítás szerint csak a %s accessén szereplõ userek használhatják

CHAN_HELP_HALFOP
	Syntax: HALFOP csatorna [nick]
	
	FélOperátori jogot kap a megadott nick a csatornán.
	Ha nem adsz meg  nicket, magadnak adsz FélOperátori jogot.
	Alap beállítás szerint csak a %s accessén szereplõ userek használhatják

CHAN_HELP_DEHALFOP
	Syntax: DEHALFOP csatorna [nick]
	
	Elveszi a FélOperátori jogot a megadott nicktõl a csatornán.
	Ha nem adsz meg  nicket, magadtól vonod meg a FélOperátori jogot.
	Alap beállítás szerint csak a %s accessén szereplõ userek használhatják

CHAN_HELP_PROTECT
	Syntax: PROTECT csatorna [nick]
	
	Védelmet ad a megadott nicknek a csatornán.
	Ha nem adsz meg  nicket, magadnak adsz védettséget.
	Alap beállítás szerint csak a %s accessén szereplõ userek használhatják

CHAN_HELP_DEPROTECT
	Syntax: DEPROTECT csatorna [nick]
	
	Elveszi a védelmet a megadott nicktõl a csatornán.
	Ha nem adsz meg  nicket, magadtól vonod meg a védelmet.
	Alap beállítás szerint csak a %s accessén szereplõ userek használhatják

CHAN_HELP_INVITE
	Syntax: INVITE csatorna
	
	A %S meghív a csatonára. Ha meghívásosra (+i) van állítva a szoba,
	akkor nagyon hasznos parancs.
	Alap beállítás szerint csak a %s accessén szereplõ userek használhatják
	
	Megjegyzés: Ha olyan üzenetet kapsz, hogy "channel is not in use",
	akkor szabadon beléphetsz ezen parancs használata nélkül.

CHAN_HELP_UNBAN
	Syntax: UNBAN csatorna
	
	A %S leveszi rólad a kitiltást és szabadon beléphetsz a csatonára.
	Alap beállítás szerint csak a %s accessén szereplõ userek használhatják

CHAN_HELP_KICK
	Syntax: KICK csatorna nick [indok]
	
	A %S kirúgja a megadott nicknevet a csatornáról.
	A kickmsg-ben szerepel a kirúgó személye és természtesen a kirúgás indoka.
	Alap beállítás szerint csak a %s accessén szereplõ userek használhatják

CHAN_HELP_KICK_PROTECTED
	
	Megjegyzés: a protected (+a) userek (védett felhasználók)
	nem rúghatóak ki ezzel a paranccsal.
	Kivéve, ha a founder adja ki a parancsot.

CHAN_HELP_TOPIC
	Syntax: TOPIC csatorna topic
	
	A %S átállítja a csatonra topic-ját.
	Ez a parancs akkor hasznos, amikor a TOPICLOCK be van kapcsolva.
	Alap beállítás szerint csak a %s accessén szereplõ userek használhatják

CHAN_HELP_CLEAR
	Syntax: CLEAR csatorna opció
	
	A %S eltávolítja a beálitásokat a csatornán.  opció
	lehet a következõ:
	
	    MODES       Törli az összes módot ami nincs az mlockban.
	    BANS        Törli az összes tiltást a csatornán.
CHAN_HELP_CLEAR_EXCEPTIONS
	    EXCEPTIONS  Törli az összes kivételt a csatornán.
CHAN_HELP_CLEAR_INVITES
	    INVITES     Törli az összes invite maszkot a csatornán.
CHAN_HELP_CLEAR_MID
	    OPS         Elvesz minden op-ot a csatornán
CHAN_HELP_CLEAR_HALFOPS
	    HALFOPS     Elvesz minden halfop-ot (mode +h) a csatornán.
CHAN_HELP_CLEAR_END
	    VOICES      Elvesz minden "voice"-t (mode +v) a csatornán.
	    USERS       Eltávolít minden usert a csatornáról (kirúgás).
	
	Alap beállítás szerint csak a %s accessén szereplõ userek használhatják

CHAN_HELP_STATUS
	Syntax: STATUS csatorna nicknév
	
	Megmutatja a megadott nicknév jelenlegi access szintjét a megadott csatornán.
	A válasz formája:
	
	    STATUS csatorna nicknév access-level
	
	Ha hiba lépne fel a válasz a következõ:
	
	    STATUS csatorna nicknév ERROR hiba-üzenet
	
	Alap beállítás szerint csak a %s accessén szereplõ userek használhatják

CHAN_OPER_HELP_COMMANDS
	
	A következõ parancsok csak Services adminoknak elérhetõek:
	
	    DROPCHAN   Droppolja bármelyik csatornát
CHAN_OPER_HELP_COMMANDS_GETPASS
	    GETPASS    Megnézni egy csatorna founder jelszavát
	                   (csak, ha az encryption nincs használatban)
CHAN_OPER_HELP_COMMANDS_FORBID
	    FORBID     Letiltja egy csatorna regisztrálását/használatát
	    SUSPEND    Felfüggeszti egy regisztrált csatorna használatát
	    UNSUSPEND  Törli egy csatorna használatának felfüggesztését

CHAN_OPER_HELP_COMMANDS_END
	Services adminok megnézhetik a privilégiummal rendelkezõk listáját,
	az autokick listát és a levels szinteket vagy használhatják a
	STATUS parancsot, bármelyik csatornán.

CHAN_OPER_HELP_DROPCHAN
	Syntax: DROPCHAN channel
	
	Törli a megnevezett csatornát a %S adatbázisából.
	
	Korlátozva: Services adminoknak.

CHAN_OPER_HELP_SET
	Services adminok állíthatják a NOEXPIRE opciót is, ami
	az adott csatornát megóvja az elévüléstõl.
	Mellegleg, Services adminok állíthatják a set opciókat bármelyik
	csatornán anélkül, hogy jelszóval azonosítanák magukat.

CHAN_OPER_HELP_SET_NOEXPIRE
	Syntax: SET csatorna NOEXPIRE {ON | OFF}
	
	Ha nincs beállítva, akkor az adott csatorna el fog évülni.
	Ha az ON opció be van kapcsolva akkor a csatorna nem fog elévülni.
	
	Korlátozva: Services adminoknak.

CHAN_OPER_HELP_UNSET
	
	Ugyanaz, mint a SET parancsnál, Services adminok törölhetik
	bármelyik csatorna beállításait, jelszavas azonosítás nélkül.

CHAN_OPER_HELP_INFO
	
	Services adminok használhatják az ALL paramétert bármelyik csatornánál.

CHAN_OPER_HELP_LIST
	Syntax: LIST [+start] sablon [FORBIDDEN] [NOEXPIRE] [SUSPENDED]
	
	Listáz minden regisztrált csatornát a sablonnak megfelelõen.
	Csatornák a PRIVATE opció beállítással csak a Services adminoknak
	fognak megjelenni. Csatornák a NOEXPIRE opció beállítással
	! elõtaggal (prefix) a csatorna neve elõtt fognak megjelenni
	a Services adminoknak.
	
	Ha a start paraméter is meg van adva, akkor a megadott számú találatot
	kihagyja és csak az utána lévõket jeleniti meg
	(például +50 azt eredményezi, hogy az elsõ 50 találatot átugorja.)
	
	Ha a FORBIDDEN, SUSPENDED vagy NOEXPIRE opció be van állítva,
	csak azok amelyek, kölcsönösen FORBIDoltak, SUSPENDeltek vagy
	NOEXPIRE opcióval rendelkeznek fognak látszani. Az opciók használhatóak
	bármilyen sorrendben és kombinációban.
	Ezek az opciók korlátozva Services adminoknak.

CHAN_OPER_HELP_GETPASS
	Syntax: GETPASS csatorna
	
	Megadja a megnevezett csatorna jelszavát.
	Figyelem ha ezt a parancsot használják, egy üzenet tartalmazni
	fogja annak a nevét aki kiadta a parancsot és a csatorna nevét
	amin használta. Bekerül a naplófájlba és egy WALLOPS/GLOBOPS üzenetbe.
	
	Ha az encryption használatban van, ez a parancs nem elérhetõ
	attól függõen, hogy az encryption melyik típusa van használatban.
	
	Korlátozva: Services adminoknak.

CHAN_OPER_HELP_FORBID
	Syntax: FORBID csatorna
	
	Megtiltja egy csatorna registrálását és használatát
	Törölni lehet a csatorna droppolásával.
	
	Korlátozva: Services adminoknak.

CHAN_OPER_HELP_SUSPEND
	Syntax: SUSPEND [+elévülés] szoba indok
	
	Megelõzi, hogy egy szobát használjanak vagy beazonosítsanak rá.
	A szoba unsuspendelhetõ az UNSUSPEND paranccsal, amely után
	minden marad a régiek szerint.
	Az elévülés paraméter ugyanazt jelenti, mint az %s AKILL parancs esetében.
	
	Korlátozva: Services adminoknak.

CHAN_OPER_HELP_UNSUSPEND
	Syntax: UNSUSPEND csatorna
	
	Törli egy csatorna használatának felfüggesztését és
	lehetõvé teszi a használatát.
	
	Korlátozva: Services adminoknak.


###########################################################################
#
# MemoServ help messages
#
###########################################################################

MEMO_HELP
	%S lehetõséget nyújt az IRC felhasználókank, hogy rövid
	szöveges üzeneteket küldjenek más IRC felhasználók részére,
	akkor is, ha a fogadó fél nem online, és csatornákra (*).
	Ahhoz, hogy üzenetet lehessen küldeni, mind a küldõ és a
	fogadó fél nicknevének regisztráltnak kell lennie.
	/msg %S HELP COMMANDS megjeleníti a(z) %S parancsok
	listáját; ha használni akarsz egy parancsot, írd be:
	/msg %S parancs, vagy a paranccsal kapcsolatos további
	információért írd be: /msg %S HELP parancs.
MEMO_HELP_EXPIRES
	
	Megjegyzés: Az üzenetek lejárnak (automatikusan törlõdnek)
	%s után kivéve, ha a SAVE paranccsal megjelölöd, hogy soha ne járjon le.

MEMO_HELP_END_LEVELS
	
	(*) Alapértelmezés szerint minden felhasználó (köztük a founder),
	    akinek a szintje legalább %s, olvashatja a csatornára
	    érkezõ üzeneteket. Ez a beállítás megváltoztatható
	    a %s LEVELS parancsával.
MEMO_HELP_END_XOP
	
	(*) A csatorna memokat a founder és minden az SOP
	    listán szereplõ felhasználó kapja meg.

MEMO_HELP_COMMANDS
	%S az alábbi parancsokat teszi elérhetõvé számodra:
	
	    SEND      Üzenetet küldhetsz egy nicknek vagy csatornára
	    LIST      Megjelenítheted az üzeneteid listáját
	    READ      Elolvashatod a megadott üzenete(ke)t
MEMO_HELP_COMMANDS_FORWARD
	    FORWARD   Továbbíthatod az üzeneteket az E-mail címedre
MEMO_HELP_COMMANDS_SAVE
	    SAVE      Megjelölheted az üzeneted, hogy soha ne járjon le
MEMO_HELP_COMMANDS_DEL
	    DEL       Törlhetsz üzeneteket
	    RENUMBER  Újraszámozhatod az üzeneteidet
	    SET       Üzeneteiddel kapcsolatosan beállításokat eszközölhetsz
	    INFO      Megjelenítheted az üzeneteiddel kapcsolatos információkat
MEMO_HELP_COMMANDS_IGNORE
	    IGNORE    A megadott felhasználótól nem fogadsz üzeneteket

MEMO_HELP_SEND
	Syntax: SEND {nicknév | csatorna} üzenet-szövege
	
	Üzenetet küld a megnevezett nicknek vagy csatornára
	a üzenet-szövegét tartalmazva. Amikor nicknévnek küldesz
	a címzett kap egy figyelmeztetést az új üzenet érkezésérõl.
	A címzett csatornának vagy nicknévnek regisztráltnak kell
	lennie.

MEMO_HELP_LIST
	Syntax: LIST [szám | lista | NEW]
	
	Megjelenít egy listát a jelenlegi üzenteteidrõl.  A NEW
	opcióval csak az új (olvasatlan) üzenteket listázza.
	listázza. Az olvasatlan üzeneteket megjelöli egy "*" jellel
	az üzenetszámtól balra, a csatorna üzenetek mellett "#" van.
	Megadhatsz számlistát is, mint, ahogy az alábbi példa mutatja:
	
	   LIST 2-5,7-9
	      Listázza az üzeneteket 2 és 5, illetve 7 és 9 között.

MEMO_HELP_LIST_EXPIRE
	Syntax: LIST [szám | lista | NEW]
	
	Megjelenít egy listát a jelenlegi üzenteteidrõl.  A NEW
	opcióval csak az új (olvasatlan) üzenteket listázza.
	listázza. Az olvasatlan üzeneteket megjelöli egy "*" jellel
	az üzenetszámtól balra; a csatorna üzenetek mellett "#" van,
	és egy "+" jel azon üzenetek mellett, amelyek nem évülnek le.
	Megadhatsz számlistát is, mint ahogy az alábbi példa mutatja:
	
	   LIST 2-5,7-9
	      Listázza az üzeneteket 2 és 5, illetve 7 és 9 között.

MEMO_HELP_READ
	Syntax: READ {szám | lista | LAST | NEW}
	
	Elküldi a megadott üzenetek tartalmát. A LAST opcióval
	megjeleníti a legutolsó üzenetet. A NEW opcióval
	minden új üzenetet. Egyébként a megadott számú üzenetet.
	Megadhatsz számlistát is, mint a LIST parancsnál.

MEMO_HELP_SAVE
	Syntax: SAVE {szám | lista}
	
	Megjelöli az megadott számú üzenetet hogy ne járjon le.
	Megadhatsz egyedül egy számot, vagy ahogyan a LIST
	parancsnál, számok listáját is.

MEMO_HELP_DEL
	Syntax: DEL {szám | lista | ALL}
	
	Törli a megjelölt üzenete(ke)t.
	Megadhatsz sok memo számot vagy intervallumot ami tartalmaz egy
	egyszerû számot, mint a LIST parancsnál; megadhatod az
	ALL szavacskát is, aminek következtében az összes memod törlödni fog.
	
	Figyelem: Ha egy memot már töröltél, az nem állítható vissza!

MEMO_HELP_RENUMBER
	Syntax: RENUMBER
	
	Újraszámozza az üzeneteid listáját, eltüntetheted a "lyukakat"
	a szám sorrendbõl amit, a üzenetek törlése okozott.


MEMO_HELP_SET
	Syntax: SET beállítás paraméterek
	
	Beállít különbözõ üzenetekkel kapcsolatos beállításokat.
	beállítás lehet az alábbiak egyike:
	
	    NOTIFY      Figyelmeztetés új üzenet érkezésekor
	    LIMIT       Beállíthatod az üzenetek maximális számát
MEMO_HELP_SET_OPTION_FORWARD
	    FORWARD     Továbbíthatod az üzeneteidet az E-mail címedre
MEMO_HELP_SET_END
	
	További információért írd be: /msg %S HELP SET opció

MEMO_HELP_SET_NOTIFY
	Syntax: SET NOTIFY {ON | LOGON | NEW | OFF}
	
	Itt lehetõséged van beállítani, ha értesítést szeretnél kapni
	az új üzenetekrõl:
	
	    ON      Értesítést kapsz az új üzenetedrõl belépéskor,
	            ha visszajössz /AWAY-bõl, vagy amikor érkezik.
	    LOGON   Értesítést kapsz az új üzenetekrõl belépéskor,
	            vagy ha visszajössz /AWAY-bõl.
	    NEW     Csak akkor kapsz értesítést, ha új üzenet érkezett.
	    OFF     Nem kapsz értesítést az üzeneteidrõl.
	
	Az ON megegyezik a LOGON és a NEW kombinálásával.
	
	Vedd figyelembe, hogy ha a nicknevedre be van állítva
	a "secure" opció, akkor csak új üzenetek érkezésekor kapsz
	értesítést (LOGON vagy ON) %s azonosítás után.

MEMO_HELP_SET_LIMIT
	Syntax: SET LIMIT [csatorna] korlát
	
	Beállítja, hogy maximálisan hány üzenetet fogadhatsz
	a nickedre. Ha 0-ra állítod, senki nem küldhet üzenetet a nickedre.
	Nem állíthatod ezt a számot nagyobb értékre, mint %d.

MEMO_HELP_INFO
	Syntax: INFO
	
	Információt jelenít meg arról, hogy mennyi üzeneted van, mennyi
	ebbõl mennyi olvasatlan, összesen mennyi üzenetet fogadhatsz,
	és azt is megmondja, hogy kapsz-e figyelmeztetést az új üzenetek
	érkezésekor.

MEMO_OPER_HELP_COMMANDS
	
	Services adminisztrátorok a SET LIMIT és INFO parancsokat
	a sajátjuknál különbözõ nickekre is használhatják.

MEMO_OPER_HELP_SET_LIMIT
	Syntax: SET LIMIT [nick] {korlát | NONE | DEFAULT} [HARD]
	
	Beállítja, hogy egy nicknek maximálisan mennyi üzenete lehet.
	Ha a korlát 0, a nick nem kaphat üzeneteket;
	NONE-ra állítva a nick kaphat és megtarthat annyi üzenetet,
	amennyit akar; DEFAULT-ra állítva a korlátot alapértelmezett
	értékre állítja (%d). Ha nem adsz meg nicknevet, akkor a saját
	korlátodat állítod be.
	
	A HARD opció hozzáadásával megakadályozza a felhasználót,
	hogy megváltoztassa a korlátot. A kihagyásával a HARD
	opció ellentétesen hat, engedélyezi a felhasználót a korlát
	megváltoztatására (még akkor is, ha az elõzõ korlát
	HARD-ra lett beállítva).
	
	A SET LIMIT parancs ezen használata Services
	adminisztrátorokra van korlátozva. Más felhasználók
	csak saját magukra vagy olyan szobára állíthatnak be
	korlátot, amelyen megvannak ezek a jogai, és nem törölhetik
	a asját korlátjaikat, illetve nem állíthatják át
	alapértelmezésre, nem állíthatnak be nagyobb értéket,
	mint %d, és nem használhatnak HARD korlátozást.

MEMO_OPER_HELP_INFO
	Syntax: INFO [nick]
	
	Paraméterek nélkül információt jelenít meg arról, hogy
	mennyi üzeneted van, mennyi ebbõl az olvasatlan, összesen
	mennyi üzenetet fogadhatsz, és kapsz-e értesítést,
	ha új üzeneted érkezik.
	
	Nicknév paraméterrel ugyanezeket az információkat
	megjeleníti a megadott nicknévre. Ez a lehetõség
	korlátozva van Services adminisztrátorokra.


MEMO_HELP_FORWARD
	Syntax: FORWARD {szám | lista | ALL}
	
	Továbbítja egy vagy több üzenetedet az E-mail címedre.
	A nicknevedhez beállított E-mail címnek valósnak kell
	lennie, ha szeretnéd használni ezt a parancsot.
	A csatornák üzenetei nem továbbíthatóak.

MEMO_HELP_SET_FORWARD
	Syntax: SET FORWARD {ON | COPY | OFF}
	
	Itt van lehetõséged beállítani, ha szeretnéd, hogy %S
	automatikusan továbbítsa az üzeneteidet az E-mail címedre.
	Ha ON-ra van állítva, az üzeneteket nem a nicknevedre,
	hanem arra az E-mail címedre küldi, amelyre a nickneved
	regisztrálva van. A COPY ehhez hasonló, de ilyenkor
	%S elküld egy másolatot is, hogy online olvasásra.
	Ha OFF-ra van állítva, az üzeneteket online tárolja,
	mint általában. Emellett ha beállítod az opciót ON-ra,
	még akkor is megkapod az üzeneteket online, ha a Services
	képtelen továbbítani õket az E-mail címedre.
	
	Vedd figyelembe, hogy ha ez az opció COPY-ra van állítva,
	és annyi üzenetet kaptál, mint amennyi üzenetet fogadhatsz
	maximálisan, addig nem kaphatsz új üzenetet (beleértve
	az E-mailt is), amíg le nem törölsz néhány régit a DEL
	paranccsal.

MEMO_HELP_IGNORE
	Syntax: IGNORE {ADD | DEL | LIST} [nicknév | maszk]
	
	Irányítja azoknak a felhasználóknak a listáját, amelyek
	által küldött üzeneteket nem veszel tudomásul; azon
	felhasználók, amelyeknek a nickneve vagy felhasználó@gazdanév
	maszkja ráillik ennek a listának az egyik bejegyzésére,
	nem küldhet üzenetet neked. ADD hozzáadja a megadott nicket
	vagy felhasználó@gazdanév maszkot az ignore listára, és
	a DEL parancs eltávolítja; LIST megjeleníti a jelenlegi
	ignore listát.
	
	Vedd figyelembe, hogty ha a nicknév vagy a maszk, amely az ADD
	parancshoz lett megadva, tartalmazhat "*" vagy "?" joker-
	karaktereket, és nem érzékeny a kis- és nagybetûkre; például
	"*Evil*" ráillik az "EvilNick"-re, "AN_EVIL_USER"-er és
	a "devil"-re is.


###########################################################################
#
# OperServ help üzenetek
#
###########################################################################

OPER_HELP
	%S lehetõséget nyújt az IRC operátoroknak, hogy különbözõ
	szempontok szerint irányíthassák az IRC hálózatot. A parancsok
	listájáért írd be: /msg %S HELP COMMANDS
	
	Megjegyzés: Minden parancs, amit az %S-nek küldesz, naplózva lesz!

OPER_HELP_COMMANDS
	%S parancsok:
	    GLOBAL      Üzenet küld minden felhasználónak
	    STATS       Megmutatja a Services és a hálozat állapotát
	    SERVERMAP   Megmutatja a hálózatban lévõ szervereket
	    OPER LIST   Megjeleníti a Services operátorok listáját
	    ADMIN LIST  Megjeleníti a Services adminok listáját
OPER_HELP_COMMANDS_SERVOPER
	
	Ezeket a parancsokat csak Services operátorok és adminok használhatják:
	    MODE        Megváltoztatja a megadott csatorna beállításait
	    KICK        Kirúgja a felhasználót a megadott szobából
	    CLEARMODES  Töröl egyes módokat a csatornáról
	    CLEARCHAN   Minden felhasználót kirúg a csatornáról (masskick)
	    KILLCLONES  Killel minden felhasználót a megadott hostról
OPER_HELP_COMMANDS_AKILL
	    AKILL       Módosítja az autokill listát
	    AKILLCHAN   Autokillel minden felhasználót a szobából
OPER_HELP_COMMANDS_EXCLUDE
	    EXCLUDE     Karbantartja az autokill kizárólistát
OPER_HELP_COMMANDS_SLINE
	    SGLINE      Karbantartja az SGLINE listát
	    SQLINE      Karbantartja az SQLINE listát
	    SZLINE      Karbantartja az SZLINE listát
OPER_HELP_COMMANDS_SESSION
	    SESSION     Megjeleníti a megadott host sessionok listáját
	    EXCEPTION   Módosítja a session-korlátozás kivétellistáját
OPER_HELP_COMMANDS_NEWS
	    LOGONNEWS   Módosítja a fellépéskor megjelenõ üzeneteket
	    OPERNEWS    Módosítja az operátoroknak szóló üzeneteket
OPER_HELP_COMMANDS_SERVADMIN
	
	Ezeket a parancsokat csak Services adminok használhatják:
	    OPER        Módosítja a Services operátorok listáját
	    JUPE        Létrehoz egy álszervert (jupiter)
	    SET         Beállít különbözõ globális Services opciókat
	    UPDATE      Arra készteti a Servicest, hogy az adatbázis-
	                    fájlokat azonnal frissítse
	    SHUTDOWN    Elmenti az adatbázist és leállítja a Servicest
	    QUIT        Leállítja a Servicest az adatbázis mentése nélkül
	    RESTART     Elmenti az adatbázist és újraindítja a Servicest
	    REHASH      Újratölti a konfigurációs fájlokat
	    SU          Services super-user jogokat biztosít
OPER_HELP_COMMANDS_SERVROOT
	
	Ezeket a parancsokat csak Services super-user használhatja:
	    ADMIN       Módosítja a Services adminok listáját
OPER_HELP_COMMANDS_RAW
	    RAW         Raw parancsot küld az IRC szervernek

OPER_HELP_GLOBAL
	Syntax: GLOBAL üzenet
	
	Lehetõséget nyújt az IRC operátoroknak a hálózaton lévõ összes
	felhasználónak szóló üzenet küldésére.
	Ezt az üzenetet errõl a nickrõl fogja küldeni: %s.

OPER_HELP_STATS
	Syntax: STATS [RESET | NETWORK | ALL]
	
	Minden más opció nélkül, megmutatja a jelenlegi felhasználók és
	online IRCopok számát (kivéve a Servicest), a legmagasabb
	felhasználó számot amióta a Services elindult, és a leghosszabb
	idõt amióta a Services fut.
	
	A RESET opció, lenullázza a maximális userszámot és az aktuális
	user számra változtatja.
	
	A NETWORK opció, megjeleníti azt adatmennyiséget amit a remote szervernek
	küldött és azt az adatmennyiséget amit a remoteszervertõl fogadott és azt a
	memoria mennyiségét, amit a hálozat küldõ/fogadó buffer használt.  Az
	jelenlegi buffer memoria kihasználtságának a buffer méret limitjének
	arányát, ami szerepelni fog, mint egy százalék, ha a limit be van állítva.
	
	Az ALL opció csak Services adminok számára érhetõ el, és megjeleníti
	az Services memoria felhasználására vonatkozó információját.  Ezen opció
	használatával lefagyhat a Services egy rövid idõre a nagyobb rendszereken,
	tehát ne használd túl sokszor!
	
	UPTIME a STATS szinonimájaként használható.

OPER_HELP_SERVERMAP
	Syntax: SERVERMAP
	
	Kirajzol egy térképet a hálózatra kapcsolódott szerverekrõl
	a Services nézõpontjából. Az álszervereknek -- magának a
	Servicesnek és a JUPE parancs által létrehozott (jupiterelt)
	szervereknek -- a neve után egy "(*") fog megjelenni.

OPER_HELP_OPER
	Syntax: OPER ADD nick
	        OPER DEL nick
	        OPER LIST
	
	Lehetõséget nyújt a Services adminoknak, hogy hozzáadjanak,
	töröljenek nickneveket a Services operátorok listáján.
	Az a felhasználó, akinek a nickneve szerepel a Services
	operátorok listáján, és azonosította a nicknevét
	%s-nél, hozzáférhet a Services operátorok
	parancsaihoz.
	
	Minden IRC operátor használhatja az OPER LIST parancsot.
	Minden más korlátozva a Services adminoknak.

OPER_HELP_ADMIN
	Syntax: ADMIN ADD nick
	        ADMIN DEL nick
	        ADMIN LIST
	
	Lehetõséget nyújt a Services super-usereknek, hogy
	hozzáadjanak, töröljenek nickneveket a Services adminok
	listáján. Az a felhasználó, akinek a nickneve szerepel
	a Services adminisztrátorok listáján, és azonosította
	a nicknevét %s-nél, hozzáférhet a Services
	admin parancsokhoz.
	
	Minden IRC operátor használhatja az ADMIN LIST parancsot.
	Minden más parancs korlátozva a Services super-usereknek.

OPER_HELP_GETKEY
	Syntax: GETKEY channel
	
	Megadja a csatorna kulcsot, amit beállítottak a csatornán, ha van ilyen.
	
	Korlátozva Services operátoroknak.

OPER_HELP_MODE
	Syntax: MODE csatorna módok
	
	Lehetõséget nyújt a Services operátoroknak, hogy bármelyik
	csatornán beállíthassanak csatornamódokat. A paraméterek
	megegyeznek a szabványos /MODE parancséval.
	
	Korlátozva Services operátoroknak.

OPER_HELP_CLEARMODES
	Syntax: CLEARMODES csatorna [ALL]
	
	Töröl minden csatorna módot (n,s,t,k,l,b, stb.) a csatornáról.
	Ha az ALL meg van adva, törli az összes felhasználói módot
	is (mint pl. +o vagy +v) a csatornáról. Habár, ha a csatorna
	regisztrált, akkor a modelock felülbírálhatja.
	
	Korlátozva Services operátoroknak.

OPER_HELP_CLEARCHAN
	Syntax: CLEARCHAN csatorna
	
	Elvtávolít minden felhasználót a csatornáról (masskick).
	
	Korlátozva Services operátoroknak.

OPER_HELP_KICK
	Syntax: KICK csatorna felhasználó indok
	
	Lehetõséget nyújt az IRCopoknak, hogy bármilyen szobából
	kirúgjanak egy felhasználót.
	A paraméterek ugyanazok, mint a szabványos /KICK parancsnál.
	A kirúgási üzenetben benne lesz a KIK parancsot küldõ
	IRCop nickneve;  például:
	
	*** SpamMan has been kicked off channel #my_channel by %S (Alcan (Flood))
	
	Korlátozva Services operátoroknak.

OPER_HELP_AKILL
	Syntax: AKILL ADD [+lejárat] maszk indok
	        AKILL DEL maszk
	        AKILL CLEAR ALL
	        AKILL LIST [+start] [sablon] [NOEXPIRE]
	        AKILL VIEW [+start] [sablon] [NOEXPIRE]
	        AKILL CHECK maszk
	        AKILL COUNT
	
	Lehetõséget nyújt a Services operátoroknak, hogy karbantartsák az
	autokill (AKILL) listát.  Ha egy felhasználó hozzáillik egy
	AKILL maszkhoz, és csatlakozni próbál, a Services kiad egy
	KILL parancsot erre a felhasználóra, és a támogatott
	szervertípusokon arra utasít minden szervereket, hogy adjon
	egy bant (K-line) a felhasználóra illõ maszkra.
	
	AKILL ADD hozzáadja a megadott felhasználó@gazdanév maszkot
	az AKILL listára a megadott indokkal (amit meg is kell adnod).
	Az indokban meg kell határoznod, hogy miért lett az Akill hozzáadva,
	a Services konfigurációjától függõen ez megjelenithetõ a kill üzenetben.
	Az elévülési idõt is meg kell adni, ahogy az alább le van írva.
	
	AKILL DEL elvátolítja a megadott maszkot az AKILL listáról,
	ha jelen van.
	
	AKILL CLEAR eltávolítja az összes maszkot az autokill
	listáról. Az ALL paraméter egy lehetõség, hogy elkerüld a
	véletlen használatot.
	
	AKILL LIST kijelzi a jelenlegi összes AKILL-t;
	és ha egy tetszõleges masz is meg van adva, a lista korlátozva lesz
	azokra az AKILL-ekre, amelyeknek ráillenek a megadott maszkra.
	Ha astart paraméter is meg van adva, akkor a megadott számú találatot
	kihagyja és csak az utána lévoket jeleniti meg (például +50
	azt eredményezi, hogy az elso 50 találatot átugorja.)
	Mellesleg, a NOEXPIRE opcióval korlátozni lehet az akillek
	listáját a nem elévülõkre.
	
	AKILL VIEW egy sokkal részletesebb verziója az AKILL LIST-nek,
	amely megmutatja, hogy ki vette fel az AKILL-t,
	mikor volt felvéve és mikor jár le, éppúgy, mint
	a felhasználó@gazdanév maszkot és indokot. Ha megadod
	a NOEXPIRE opciót, akkor csak azokat az AKILL-eket mutatja,
	amelyek nem járnak le, és ráillenek a felhasználó@gazdanév maszkra.
	
	AKILL CHECK megjeleníti azokat a bejegyzéseket az akill listán
	amelyekkel a megadott user@host maszk egyezik; ez arra használható,
	hogy eldönthesd melyik autokill a legmegfelelõbb a user killeléséhez.
	
	AKILL COUNT csak visszaküldi az AKILL-ek számát az AKILL listán.
	
	Opcionális, egy elévülési idõ megadható az AKILL ADD
	paranccsal. Az elévülési idõ megelõzi a user@host maszkot, és
	meghatározható mint egy egész szám amit egy követ ezek közül d (nap),
	h (óra), vagy m (perc).  Kombinációk (mint az 1h30m)
	most már megengedettek. Ha nincs idõegység, az alapértelmezés
	a nap (vagyis +30 önmagában azt jelenti, 30 nap).
	Soha le nem járó AKILL hozzáadásához használj +0-át. Ha
	a hozzáadandó usermaszk + jellel kezdõdik, a lejárati idõt meg
	kell adni, akkor is, ha ugyanaz, mint az alapértelmezés.

OPER_HELP_AKILL_OPERMAXEXPIRY
	
	Services adminisztrátorok korlátozás nélkül határozhatnak meg
	lejárati idõt, Services operátorok esetében a maximum %s.
OPER_HELP_AKILL_END
	
	Korlázotva Services operátoroknak.

OPER_HELP_AKILLCHAN
	Syntax: AKILLCHAN [KILL] [+elévülés] csatorna indok
	
	Hozzáad egy autokillt, mely minden felhasználóra vonatkozik,
	kivéve az IRC operátorokra a megadott csatornán,
	opcionálisan (ha a KILL meg van adva) eltávolítja oket a
	hálózatból.  Az indok tárolva lesz minden autokill mellett,
	és meg fog jelenni a killelési üzenetben a Services
	konfigurációjától függõen.  elévülést ugyanabban a
	formátumban kell megadni, mint az AKILL ADD parancsnál; ha
	nincs megadva, akkor az alapértéke: %s.
	
	Az Autokilleknél, melyeket ezzel a paranccsal adsz hozzá,
	*@hostname formában lesz minden hosztnév, melyet használ
	valaki a csatornán.  Ha már egy autokill szerepel az adott
	hosztnévre akkor az nem lesz módosítva; ennek okán lehet,
	hogy kevesebb autokill kerül hozzáadásra, mint ahány user a
	szobában érintett.
	
	Limitálva Services operátoroknak.

OPER_HELP_EXCLUDE
	Syntax: EXCLUDE ADD [+lejárat] maszk indok
	        EXCLUDE DEL maszk
	        EXCLUDE CLEAR ALL
	        EXCLUDE LIST [+start] [sablon] [NOEXPIRE]
	        EXCLUDE VIEW [+start] [sablon] [NOEXPIRE]
	        EXCLUDE CHECK maszk
	        EXCLUDE COUNT
	
	Lehetõséget nyújt a Services operátoroknak, hogy karbantartsák
	az autokill kizáró listát. Ha egy felhasználó ráillik
	egy autokill kizáró maszkra, és csatlakozik a hálózatra,
	a Services nem fog kiadni KILL parancsot arra a felhasználóra,
	még akkor is, ha a felhasználó máskülönben ráillene az
	autokill maszkra. Ez hasznos lehet, ha széleskörû autokill
	maszkjaid vannak, és még mindig akarsz gazdaneveket vagy
	felhasználókat, hogy egy ilyen maszkkal tudjanak csatlakozni.
	
	Az ADD, DEL, CLEAR, LIST, VIEW, CHECK, és COUNT
	alparancsok ugyanúgy mûködnek, mint az AKILL parancs
	alparancsai; nézd meg az AKILL helpjét további
	információért.
	
	Korlátozva Services operátoroknak.


OPER_HELP_SGLINE
	Syntax: SGLINE ADD [+expiry] mask reason
	        SGLINE DEL mask
	        SGLINE CLEAR ALL
	        SGLINE LIST [+start][mask] [NOEXPIRE]
	        SGLINE VIEW [+start][mask] [NOEXPIRE]
	        SGLINE CHECK maszk
	        SGLINE COUNT
	
	Engedélyezi a Services operátoroknak, hogy karbantartsák
	az SGLINE listát. Ha egy felhasználó "valós neve" ráillik
	egy SGLINe maszkra, és megpróbál csatlakozni, a Services
	kiad egy KILL parancsot arra a felhasználóra, és a támogatott
	szervertípusokon arra utasít minden szervert, hogy adjon
	egy ban-t (G-line) a felhasználóra illõ maszkra.
	
	SGLINE ADD hozzáadja a valósnévmaszkot az SGLINE listára
	a megadott indokkal (amit meg is kell adni).
	Az indokban meg kell határoznod, hogy miért lett az Sgline hozzáadva,
	a Services konfigurációjától függõen ez megjelenithetõ a kill üzenetben.
	Az elévülési idõt is meg kell adni, ahogy az alább le van írva.
	
	SGLINE DEL eltávolítja a megadott maszkot az SGLINE listáról,
	ha jelen van.
	
	SGLINE CLEAR törli az összes maszkot az SGLINE listáról
	Az ALL paraméter egy lehetoség, hogy elkerüld a
	véletlen használatot.
	
	SGLINE LIST kijelzi az összes SGLINE bejegyzést; ha
	egy tetszõleges maszk meg van adva, a lista korlátozva lesz
	azokra a bejegyzésekre, amelyekre ráillik a maszk.
	Ha astart paraméter is meg van adva, akkor a megadott számú találatot
	kihagyja és csak az utána lévoket jeleniti meg (például +50
	azt eredményezi, hogy az elso 50 találatot átugorja.)
	Mellesleg, a NOEXPIRE opcióval korlátozni lehet az sglineok
	listáját a nem elévülokre.
	
	SGLINE VIEW sokkal részletesebb verziója az SGLINE LIST-nek,
	és megmutatja, ki vette fel az SGLINE bejegyzéseket, a dátumot,
	amikor felvették, mikor használták utoljára, és mikor jár le,
	éppúgy, mint a maszkot és az indokot.
	
	SGLINE CHECK megjeleníti azokat a bejegyzéseket az akill listán
	amelyekkel a megadott user@host maszk egyezik; ez arra használható,
	hogy eldönthesd melyik SGLINE a legmegfelelobb a user killeléséhez.
	
	SGLINE COUNT csak visszaküldi a bejegyzések számát
	az SGLINE listán.
	
	A maszk paraméter idézõjelek közé tehetõ, és kötelezõ,
	ha a maszk szóközöket tartalmaz; például:
	    /msg %S SGLINE ADD +0 "rossz név" Ez egy rossz név.
	
	Nem kötelezõ, de megadható egy lejárati idõ is az SGLINE ADD
	parancshoz.  A lejárati idõ megelõzi a flehasználó@gazdanév maszkot,
	egész számként határozzuk meg, és követi egy d (nap),
	h (óra) vagy m (perc) betû. Kombinációk (mint pl. 1h30m)
	most már megengedettek.  Ha nincs idõegység, az alapértelmezés
	a nap (vagyis +30 önmagában azt jelenti, 30 nap). Ha szeretnél
	egy olyan bejegyzést, ami soha nem jár le, használj +0-át.
	Ha a maszk + jellel kezdõdik, és nincs megadva lejárati idõ,
	a maszkot idézõjelek közé kell tenni, ahogy fent le van írva.
	
	
	Korlátozva Services operátoroknak.

OPER_HELP_SQLINE
	Syntax: SQLINE ADD [+lejárat] maszk indok
	        SQLINE DEL maszk
	        SQLINE CLEAR ALL
	        SQLINE LIST [+start] [maszk] [NOEXPIRE]
	        SQLINE VIEW [+start] [maszk] [NOEXPIRE]
	        SQLINE CHECK maszk
	        SQLINE COUNT
	
OPER_HELP_SQLINE_KILL
	Megengedi a Services operátoroknak, hogy módosítsák az SQLINE listát.
	Ha egy felhasználó, akinek a neve szerepel egy SQLINE maszkban megpróbál
	csatlakozni, vagy a felhasználó megpróbálja átváltoztatni a
	nevét egy olyanra amely szerepel egy SQLINE maszkban, a Services ki fog
	osztani egy KILL-t ennek a felhasználónak, a támogatott szerver típusok
	utasítják az összes szervert, hogy adjanak egy tiltást (szerver Q-line)
	a maszknak amivel a felhasználó megegyezett.
OPER_HELP_SQLINE_NOKILL
	Megengedi a Services operátoroknak, hogy módosítsák az SQLINE listát.
	Ha egy felhasználó, akinek a neve szerepel egy SQLINE maszkban megpróbál
	csatlakozni, vagy a felhasználó megpróbálja átváltoztatni a
	nevét egy olyanra amely szerepel egy SQLINE maszkban, a Services
	megváltoztatja a nicknevét "guest" nicknévre és küld egy üzenetet a
	felhasználó IRC kliensének, hogy válasszon egy másik nicknevet.
	A támogatott szerver típusok utasítják az összes szervert,
	hogy adjanak egy tiltást (szerver Q-line) a maszknak amivel a
	felhasználó megegyezett.
OPER_HELP_SQLINE_IGNOREOPERS
	
	Megjegyzés az IRC operátorokra nem hatnak az SQLINE maszkok.

OPER_HELP_SQLINE_END
	
	Az ADD, DEL, CLEAR, LIST, VIEW, CHECK és COUNT alparancsok
	hozzáadásra, törlése, megtekintésre (rövid vagy hosszú verzió),
	vagy az SQLINE lista bejegyzéseinek megszámolására használhatóak.
	Lásd az SGLINE helpjét a részletekért.
	
	Korlátozva Services operátoroknak.

OPER_HELP_SZLINE
	Syntax: SZLINE ADD [+lejárat] maszk indok
	        SZLINE DEL maszk
	        SZLINE CLEAR ALL
	        SZLINE LIST [+start] [maszk] [NOEXPIRE]
	        SZLINE VIEW [+start] [maszk] [NOEXPIRE]
	        SZLINE CHECK maszk
	        SZLINE COUNT
	
	Lehetõséget nyújt a Services operátoroknak, hogy karbantartsák
	az SZLINE listát. Ha egy felhasználó, akinek az IP címe ráillik
	egy SZLINE maszkra, és megpróbál csatlakozni, a Services kiad
	egy KILL parancsot arra a felhasználóra, és a támogatott
	szervertípusokon arra utasít minden szervert, hogy adjon
	egy bant (server Z-line) a felhasználóra illõ maszkra.
	
	Az ADD, DEL, CLEAR, LIST, VIEW, CHECK, és COUNT
	alparancsok hozzáadásra, törlésre, megtekintésre (rövid vagy hosszú
	verzió), vagy az SZLINE lista bejegyzéseinek megszámolására
	használhatóak.
	Lásd az SGLINE helpjét (/msg %S HELP SGLINE) a részletekért.
	
	Korlátozva Services operátoroknak.

OPER_HELP_EXCEPTION
	Syntax: EXCEPTION ADD [+lejárat] maszk korlát indok
	        EXCEPTION DEL {maszk | lista}
	        EXCEPTION CLEAR ALL
	        EXCEPTION MOVE szám újszám
	        EXCEPTION LIST [[+start] maszk | lista]
	        EXCEPTION VIEW [[+start] maszk | lista]
	        EXCEPTION CHECK hosztnév
	        EXCEPTION COUNT
	
	Lehetõséget nyújt a Services operátoroknak, hogy
	karbantartsák azon hostok listáját, amelyeknek meghatározott
	session korlátozásuk van, engedélyezve bizonyos gépeket,
	úgymint a shell szervereket, hogy egyidõben felvigyenek
	klienseket az alapértelmezettnél nagyobb számban. Amennyiben
	a gép eléri a session korlátot, minden csatlakozással
	próbálkozó kliens killelve lesz. Mielõtt a kill megtörténne,
	figyelmeztetést kapnak egy %S /NOTICE üzeneten keresztül,
	ami, természetesen, egy segítségforrás a session korlátozásról.
	Az üzenet tartalma egy konfigurációs beállítás.
	
	EXCEPTION ADD hozzáadja a megadott hostmaszkot a kivétellistába.
	Vedd figyelembe, hogy a nick!user@host és user@host maszkok
	érvénytelenek! Csak valós hostmaszkok, mint pl. box.host.dom
	és *.host.dom, megengedettek, mert a session korlátozás
	maszkja nem vesz fel nicknevet vagy felhasználónevet.
	A korlát legyen egy nullával egyenlõ vagy nagyobb szám;
	ez határozza meg, hogy hány sessiont vihet magával a host
	egyidõben (a nulla azt jelenti, hogy a hostnak korlátlan
	számú sessionja lehet).
	
	Nem kötelezõ, de megadható egy lejárati idõ az EXCEPTION ADD
	parancshoz. A lejárati idõ megelõzi a hostmaszkot, egy
	számként határozzuk meg, és követi egy d (nap),
	h (óra) vagy m (perc) betû.  Kombinációk (mint pl. 1h30m)
	is megengedettek. Ha nincs megadva idõegység, az alapértelmezés
	a nap (vagyis +30 önmagában azt jelenti, 30 nap). Ha szeretnél
	olyan bejegyzést, ami soha nem jár le, használj +0-át. Ha
	a hozzáadandó maszk + jellel kezdõdik, egy lejárati idõt is meg
	kell adni, még akkor is, ha ugyanaz, mint ami az alapértelmezés.
	
	EXCEPTION DEL eltávolítja a megadott maszkot vagy a megjelölt sorszámú
	szereplõt a kivétel listáról (sorszám megtalálható: VIEW vagy LIST).
	
	EXCEPTION CLEAR Töröl minden maszkot a kivétel listáról.
	Az ALL paraméter egy lehetoség, hogy elkerüld a
	véletlen használatot.
	
	EXCEPTION MOVE megváltoztatja a kivételt szám ról új szám ra.
	Ha az újszám már használva van egy másik kivételnél, akkor a
	szám meg lesz növelve, hogy legyen helye.
	
	EXCEPTION LIST és EXCEPTION VIEW kijelez minden kivételt;
	ha megadsz egy tetszõleges maszkot, a listát korlátozza
	azokra a kivételekre, amelyekre ráillik a maszk. A különbség
	a két parancs között az, hogy az EXCEPTION VIEW sokkal
	részletesebb, megjeleníti a kivételt felvevõ személy nevét
	a session korlátot, indokot, hostmaszkot és a lejárati dátumot
	és idõt, úgymint a kivétel legutolsó "használati" idejét,
	pl. azon felhasználó legutolsó csatlakozási idejét, amelyre
	ráillik a maszk.
	
	EXCEPTION CHECK megjeleníti azokat a bejegyzéseket az akill listán
	amelyekkel a megadott user@host maszk egyezik; ez arra használható,
	hogy eldönthesd melyik kivétel, ha van, a legmegfelelõbb a usernek.
	
	EXCEPTION COUNT megjeleníti, hogy hány bejegyzés van a
	kivétel listán.
	
	Vedd figyelembe, hogy a csatlakozó kliens a ráillõ kivételek közül
	mindig az elsõt használja. Hatalmas kivétellisták és széleskörû
	kivételmaszkok lefokozhatják a Services teljesítményét.
	
	Korlátozva Services operátoroknak.

OPER_HELP_SESSION
	Syntax: SESSION LIST határérték
	        SESSION VIEW host
	
	Lehetõséget nyújt a Services operátoroknak, hogy megtekintsék
	a session listát.
	
	SESSION LIST kilistázza azokat a hostokat, amelyeknek legalább
	határérték sessionjuk van. A határérték legyen 1-nél nagyobb szám,
	hogy megelõzze a nagy számú, egy sessionú hostok kilistázását.
	
	SESSION VIEW megjeleníti a meghatározott host részletes információit,
	tartalmazva a jelenlegi session számot és session korlátot.
	A host értéke nem tartalmazhat maszkkaraktereket.
	
	Lásd az EXCEPTION helpjét további információért a session-
	korlátozással és azzal kapcsolatban, hogy hogyan állíts be
	különleges session korlátokat bizonyos hostokra és csoportokra.
	
	Korlátozva Services operátoroknak.

OPER_HELP_SU
	Syntax: SU jelszó
	
	Elérési lehetõséget nyújt olyan funkciókhoz, amelyek Services
	super-user jogokat igényelnek. A jelszó ugyanaz, mint ami
	a SET SUPASS paranccsal lett beállítva; vedd figyelembe, hogy
	ez a jelszó legyen beállítva, mielõtt használod ezt a parancsot.
	A super-user jogok addig tartanak, amíg ki nem lépsz
	az IRC-bõl.
	
	FIGYELEM: Ha helytelen jelszót írsz be, a Services figyelmeztetõ
	körüzenetet küld minden IRC operátornak.
	
	Korlátozva Services adminisztrátoroknak.

OPER_HELP_SET
	Syntax: SET opció beállítás
	
	Beállít különbözõ Services opciókat. A jelenleg definiált
	opciónevek:
	    READONLY   Csak olvasható vagy írható-olvasható módba vált
	    DEBUG      Bekapcsolja vagy kikapcsolja a hibakeresõ módot
	    SUPASS     Beállítja a jelszót a SU parancshoz (super-user)
	
	Korlátozva Services adminisztrátoroknak; a SUPASS opciót csak
	a Services super-user állíthatja be.

OPER_HELP_SET_READONLY
	Syntax: SET READONLY {ON | OFF}
	
	Bekapcsolja vagy kikapcsolja a csak olvasható módot. Ha
	csak olvasható módban van, a felhasználók nem módosíthatják
	a Services adatait, kivéve a szobák és nicknevek hozzáférési
	listáját, stb. IRC operátorok a megfelelõ Services jogokkal
	megváltoztathatják a Services AKILL listáját és droppolhatnak
	vagy letilthatnak szobákat, de a változások nem kerülnek mentésre,
	hacsak a csak olvasható módot kikapcsolják, mielõtt a Services
	leáll vagy újraindul.
	
	Ez az opció egyenértékû a -readonly parancssori opcióval.

OPER_HELP_SET_DEBUG
	Syntax: SET DEBUG {ON | OFF | szám}
	
	Bekapcsolja vagy kikapcsolja a hibakeresõ módot. Hibakeresõ
	módban minden adatot, ami a Servicest érinti, éppúgy, mint
	a többi hibakeresõ üzenet számát, beleírja a naplófájlba. Ha
	a szám meg van adva, a hibakeresõ mód akkor lesz kikapcsolva,
	ha a hibakeresõ szint száma a megadott szám-ra van beállítva.
	
	Ez az opció egyenértékû a -debug parancssori opcióval.

OPER_HELP_SET_SUPASS
	Syntax: SET SUPASS [jelszó]
	
	Beállítja a SU parancshoz használható jelszót. A jelszónak
	beállítva kell lennie, mielõtt a SU parancsot elsõ alkalommal
	használod.  Ha nincs jelszó megadva, a SU parancs nem lesz
	használható.
	
	Ezt az opciót csak a Services super-user állíthatja be.

OPER_HELP_JUPE
	Syntax: JUPE szerver [indok]
	
	Elmondja a Servicesnek, hogy jupitereljen egy szervert--vagyis,
	hogy készítsen egy "álszervert", ami a Serviceshez csatlakozik,
	és megvédi annak a névnek a valós szerverét a csatlakozástól.
	A jupe etávolítható egy szabványos /SQUIT paranccsal. Ha megadsz
	indokot, bekerül a szerver információs mezõjébe; egyébként
	a szerver információs mezõje a "Jupitered by <nick>" szöveget fogja
	tartalmazni, mutatva a szervert jupiterelõ felhasználó nicknevét.
	
	Korlátozva Services adminisztrátoroknak.

OPER_HELP_RAW
	Syntax: RAW szöveg
	
	Küld egy szövegsztringet közvetlenül annak a szervernek,
	ahová a Services kapcsolódott. A parancs elsõdlegesen
	a Services és az IRC Szerver teszteléséhez van; nagyon
	korlázotott körben használható egy élõ hálózaton, mert
	tönkreteheti a hálózatot, vagy kiakaszthatja a Servicest,
	ha helytelenül használják. NE HASZNÁLD EZT A PARANCSOT,
	amíg nem vagy abszolút biztos abban, hogy mit csinálsz!
	
	Korlátozva Services super-usereknek.

OPER_HELP_UPDATE
	Syntax: UPDATE [FORCE]
	
	Utasítja a Services, hogy mentsen el minden adatbázis fájlt
	olyan, hamar, ahogy lehet a parancs kiadásától számítva. Ha a
	FORCE opció is meg van adva a Services megpróbálja kinyitni
	az adatbázisokat mielõtt végrehajtaná a mentést; ezzel
	kiküszöbölhetõ a "databases are locked" hibaüzenet, az
	adatbázis mentésekor.
	
	Korlátozva Services adminoknak.

OPER_HELP_QUIT
	Syntax: QUIT
	
	Arra készteti a Servicest, hogy hajtsok végre egy azonnali
	leállítást; az adatbázist nem menti el. Ezt a parancsot
	nem érdemes használni, kivéve, ha attól tartasz, hogy
	az adatbázisok memóriabeli másolata megsérült, és nem akarod
	elmenteni. Normális leállításhoz használd a SHUTDOWN
	parancsot.
	
	Korlátozva Services adminisztrátoroknak.

OPER_HELP_SHUTDOWN
	Syntax: SHUTDOWN
	
	Arra készteti a Servicest, hogy mentsen el minden adatbázist,
	és álljon le.
	
	Korlátozva Services adminisztrátoroknak.

OPER_HELP_RESTART
	Syntax: RESTART
	
	Arra készteti a Servicest, hogy mentsen el minden adatbázist,
	és induljon újra (pl. kilépjen és azonnal újrafuttasa
	a futtatható fájlt).
	
	Korlátozva Services adminisztrátoroknak.

OPER_HELP_REHASH
	Syntax: REHASH
	
	Arra készteti a Servicest, hogy újraolvassa a konfigurációs
	és újrafrissítse az aktív beállításokat. Néhány beállítás,
	mint pl. a RemoteServer (ami beállítja az IRC szerver
	gazdanevét, ahová csatlakozik), nem változtatható meg
	REHASH-sal, és csak akkor lép életbe, ha a Services újraindul;
	ezekben az esetekben egy üzenetet ír a naplófájlba, és errõl
	küld egy figyelmeztetést is.
	
	Ha LoadModule sorok lettek hozzáadva vagy eltávolítva
	a fõ Services konfigurációs fájlból (%s),
	a modulokat betölti vagy kiiktatja, ha szükséges. Azokat
	a modulokat, amelyeknek már nincsenek LoadModule soraik,
	sorrendben visszafelé iktatja ki; aztán a maradék modul
	újratöltése után az új modulokat betölti abban a sorrendben,
	ahogy a konfigurációs fájlban szerepel. Ha hiba történik
	bármelyik lépésben, az újratöltés megszakad.
	
	Vedd figyelembe, hogy a modulfüggõségek következtépben néhány
	modul nem távolítható el vagy változhat meg REHASH-on keresztül;
	ilyen esetben újra kell indítani a Servicest, hogy a változások
	érvénybe lépjenek.
	
	Korlátozva Services adminisztrátoroknak.

OPER_HELP_KILLCLONES
	Syntax: KILLCLONES nick
	
	Killel minden olyan felhasználót, akinek ugyanaz a gazdaneve, mint
	ami a megadott nick-é. Felvesz egy ideiglenes AKILL-t, *@host
	formában, hogy megelõzze a támadó kliensek azonnali
	újracsatlakozását. Egy WALLOPS üzenetet is küldd, amelyben jelzi,
	ki használta a parancsot, milyen felhasználóra hatott, és hány
	felhasználót killelt. Ez a parancs hasznos lehet számos klónok
	hálózatról való eltávolítására.
	
	Korlátozva Services operátoroknak.


###########################################################################
#
# StatServ help üzenetek
#
###########################################################################

STAT_HELP
	%S statisztikai információkat nyújt az IRC hálózatról,
	a szervereirõl és felhasználóiról.
	/msg %S HELP COMMANDS megmutatja %S
	parancsainak listáját; a parancsokról további
	információért írd be: /msg %S HELP command.

STAT_HELP_COMMANDS
	%S az alábbi parancsokat nyújtja neked:
	
	    SERVERS     Statisztikák a szerverekrõl
	    USERS       Statisztikák a felhasználókról

STAT_HELP_SERVERS
	Syntax: SERVERS STATS
	        SERVERS LIST [maszk]
	        SERVERS VIEW [maszk] [ONLINE | OFFLINE]
	
	Statisztikát jelenít meg a hálózat szervereivel kapcsolatban.
	
	SERVERS STATS megjelenít egy rövid összegzést a Services
	által jelenleg látható (online) és nem látható (offline)
	szerverekrõl.
	SERVERS LIST megjelenít egy listát az online szerverekrõl,
	és a jelenlegi statisztikájukról.
	SERVERS VIEW megjelenít egy listát az online és offline
	szerverekrõl részletesebb statisztikával. Az ONLINE és
	OFFLINE opciók egyenként annyit tesznek, hogy csak az
	online vagy offline szervereket jeleníti meg.
	
	Használd a maszk opciót, hogy korlátozd, mely szerverek
	statisztikáját szeretnéd látni.

STAT_HELP_USERS
	Syntax: USERS STATS
	
	Részletes statisztikát nyújt a hálózat felhasználóival
	kapcsolatban.

STAT_OPER_HELP_SERVERS
	Syntax: SERVERS STATS
	        SERVERS LIST [maszk]
	        SERVERS VIEW [maszk] [ONLINE | OFFLINE]
	        SERVERS DELETE szerver
	        SERVERS COPY szerver újnév
	        SERVERS RENAME szerver újnév
	
	Részletes statisztikát nyújt a hálózat szervereivel
	kapcsolatban.
	
	SERVERS STATS megjelenít egy rövid összegzést a Services
	által jelenleg látható (online) és nem látható (offline)
	szerverekrõl.
	SERVERS LIST megjelenít egy listát az online szerverekrõl
	és a jelenlegi statisztikájukról.
	SERVERS VIEW megjelenít egy listát az online és offline
	szerverekrõl részletes statisztikával. Az ONLINE és OFFLINE
	opciók egyenként annyit tesznek, hogy csak az online vagy
	offline szervereket jeleníti meg.
	
	Használd a maszk opciót, hogy korlátozd, mely szerverek
	statisztikáját szeretnéd látni.
	
	DELETE törli a megadott szerver statisztikáját.
	COPY duplázza a megadott szerver statisztikáját.
	RENAME átnevezi a megadott szerver statisztikáját.
	Ez a három parancs korlátozva Services adminisztrátoroknak.

###########################################################################

# CVS: $Revision: 2.123 $
