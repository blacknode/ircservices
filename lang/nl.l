# Emacs: -*- mode: fundamental; coding: binary -*-

# Dutch language file.
#
# IRC Services is copyright (c) 1996-2009 Andrew Church.
#     E-mail: <achurch@achurch.org>
# Parts written by Andrew Kempe and others.
# This program is free but copyrighted software; see the file GPL.txt for
# details.

# When translating this file to another language, keep in mind that the
# order of parameters for sprintf() is fixed in the source code, so your
# messages need to take the same parameters in the same order as the
# English messages do.  (Obviously, this doesn't hold for the strftime()
# format lines immediately below.)  If you can't get a natural translation
# of a message without changing the order of the parameters, please inform
# the Services developers.
#
# In help messages, "%S" (capital S, not lowercase) refers to the name of
# the service sending the message; for example, in NickServ help messages,
# "%S" is replaced by "NickServ" (or whatever it is renamed to in
# config.h).  The %S's do not count as sprintf() parameters, so they can be
# rearranged, removed, or added as necessary.
#
# Also in help messages, please try to limit line lengths to 60 characters
# of text (not including the leading tab).  This length was chosen because
# it does not cause line wrap under default settings on most current IRC
# clients.  Remember that format characters (control-B, control-_) are not
# included in that 60-character limit (since they don't show on the user's
# screen).  Also remember that format specifiers (%S, etc.) will expand
# when displayed, so remember to take this into account; you can assume
# that the length of a pseudoclient name (%S replacement) will be eight
# characters, so reduce the maximum line length by 6 for every %S on a
# line.
#
# Finally, remember to put a tab at the beginning of every line of text
# (including empty lines).  This has to be a tab, not spaces.

###########################################################################
#
# Name and character set of this language
#
###########################################################################

# Language name.
# For languages other than English, this string should have the following
# format:
#     language-name-in-language (language-name-in-English)
# For example, "Español (Spanish)" or "Français (French)".

LANG_NAME
	Nederlands (Dutch)

# Character set used by this language.  This should be a "charset" value
# understood by mailreader or web browser software.

LANG_CHARSET
	iso-8859-1

####################################################
# Translation by Martin Pels (rodecker@mp3crew.nu) #
# Tested by Mp3CreW (www.mp3crew.nu)               #
####################################################

###########################################################################
#
# General messages
#
###########################################################################

# strftime() format strings.  man 3 strftime for information on the
# meanings of the format specifiers.  Short version:
#	%a = weekday name (short)	%H = hour
#	%b = month name (short)		%M = minute
#	%d = day of month		%S = second
#	%Y = year			%Z = time zone

# This is used as the format string for strftime() for a date and time
# together.
STRFTIME_DATE_TIME_FORMAT
	%b %d %H:%M:%S %Y %Z
# This is used as the format string for strftime() for a date alone in long
# format (English: including weekday).
STRFTIME_LONG_DATE_FORMAT
	%a %b %d %Y
# This is used as the format string for strftime() for a date alone in
# short format (English: without weekday).
STRFTIME_SHORT_DATE_FORMAT
	%b %d %Y

# These tell our strftime() what the names of months and days are.  If you
# don't use %a, %A, %b, or %B in your strftime() strings above, you can
# leave these empty.  However, if you enter names, they MUST stay in order,
# one per line, and the list MUST be complete!

# %a
STRFTIME_DAYS_SHORT
	Zo
	Ma
	Di
	Wo
	Do
	Vr
	Za
# %A
STRFTIME_DAYS_LONG
	zondag
	maandag
	dinsdag
	woensdag
	donderdag
	vrijdag
	zaterdag
# %b
STRFTIME_MONTHS_SHORT
	Jan
	Feb
	Maa
	Apr
	Mei
	Jun
	Jul
	Aug
	Sep
	Okt
	Nov
	Dec
# %B
STRFTIME_MONTHS_LONG
	Januari
	Februari
	Maart
	April
	Mei
	Juni
	Juli
	Augustus
	September
	Oktober
	November
	December


# "day", "hour", "minute", and "second" in singular and plural.
# These must be preceded by spaces if the language requires them
# (e.g. in English: "10 hours", not "10hours").
STR_DAY
	 dag
STR_DAYS
	 dagen
STR_HOUR
	 uur
STR_HOURS
	 uur
STR_MINUTE
	 minuut
STR_MINUTES
	 minuten
STR_SECOND
	 seconde
STR_SECONDS
	 seconden
# Separator used between units of time; in English, a comma followed by a
# space (the ", " in "3 hours, 10 minutes").
STR_TIMESEP
	, 

# This is used in ChanServ/NickServ INFO displays, and is a comma followed
# by a space (", " -- trailing space is significant), or the equivalent in
# the language's character set.
COMMA_SPACE
	, 


# Various error messages.
INTERNAL_ERROR
	Interne fout--kon verzoek niet uitvoeren.
SERVICES_IS_BUSY
	Services is momenteel druk; probeer het later nog eens.

UNKNOWN_COMMAND
	Onbekend commando %s.
UNKNOWN_COMMAND_HELP
	Onbekend commando %s.  Tik /msg %s HELP voor hulp.
SYNTAX_ERROR
	Syntax: %s
MORE_INFO
	Tik /msg %s HELP %s voor meer informatie.
NO_HELP_AVAILABLE
	Geen hulp beschikbaar voor %s.
MISSING_QUOTE
	Dubbele aanhalingstekens (") ontbreken.

BAD_EMAIL
	E-mailadressen dienen in de vorm gebruikersnaam@machinenaam opgegeven te worden.
	Niet toegestaan zijn kleuren, vetgedrukte, onderstreepte, of omgedraaide tekst, of één van de karakters: , : ; | \ " ( ) < > [ ]
REJECTED_EMAIL
	Sorry, dit e-mailadres kan niet worden gebruikt.
BAD_URL
	URLs dienen in de vorm http://machinenaam[:poort]/... (of ftp://, etc.) gegeven te worden.
BAD_USERHOST_MASK
	Masker dient in de vorm gebruiker@machine gegeven te worden.
BAD_NICKUSERHOST_MASK
	Masker dient in de vorm bijnaam!gebruiker@machine gegeven te worden.
BAD_EXPIRY_TIME
	Ongeldige vervaltijd.

SENDMAIL_NO_RESOURCES
	Het systeem is momenteel te druk om e-mail te verzenden; probeer het later nog eens.
READ_ONLY_MODE
	Opmerking: Services staat in alleen-lezen modus; wijzigingen worden niet opgeslagen!
PASSWORD_INCORRECT
	Wachtwoord onjuist.
PASSWORD_WARNING
	Waarschuwing: Na het volgende onjuiste wachtwoord wordt uw verbinding verbroken.

# What's the difference between "Access denied" and "Permission denied"?
# Very little; you can safely make them the same message with no real loss
# of meaning.  If you want to make a distinction, "Access denied" is
# usually used more generally; for example, a non-oper trying to access
# OperServ gets "Access denied", while a non-Services admin trying to use
# NickServ SET NOEXPIRE gets "Permission denied".
ACCESS_DENIED
	Toegang geweigerd.
PERMISSION_DENIED
	Permissie geweigerd.

MORE_OBSCURE_PASSWORD
	Probeer het nogmaals met een gecompliceerder wachtwoord.  Wachtwoorden dienen minimaal vijf karakters lang, en niet eenvoudig raadbaar te zijn (zoals uw echte naam of uw bijnaam).

NICK_NOT_REGISTERED
	Uw bijnaam is niet geregistreerd.
NICK_NOT_REGISTERED_HELP
	Uw bijnaam is niet geregistreerd.  Tik /msg %s HELP voor informatie over het registreren van uw bijnaam.
NICK_TOO_LONG
	Bijnamen dienen maximaal %d karakters lang zijn.
NICK_INVALID
	%s is geen geldige bijnaam.
NICK_X_NOT_REGISTERED
	Bijnaam %s is niet geregistreerd.
NICK_X_ALREADY_REGISTERED
	Bijnaam %s is reeds geregistreerd!
NICK_X_NOT_IN_USE
	Bijnaam %s is momenteel niet in gebruik.
NICK_X_FORBIDDEN
	Bijnaam %s mag niet worden geregistreerd of gebruikt.
NICK_X_SUSPENDED
	Bijnaam %s is buiten werking gesteld en mag niet gebruikt worden of voor worden geïdentificeerd.
NICK_X_SUSPENDED_MEMOS
	Bijnaam %s is buiten werking gesteld en mag geen memo's ontvangen.
NICK_IDENTIFY_REQUIRED
	Wachtwoordauthenticatie vereist voor dit commando.
	Probeer nogmaals na het tikken van /msg %s IDENTIFY wachtwoord.
NICK_PLEASE_AUTH
	Uw bijnaam is nog niet geauthenticeerd.  Authenticeer uw bijnaam met behulp van de authenticatiecode in het e-mailbericht dat verstuurd is naar: %s
NICK_X_NOT_ON_CHAN_X
	Bijnaam %s is momenteel niet in kamer %s.

CHAN_INVALID
	%s is geen geldige kamernaam.
CHAN_X_NOT_REGISTERED
	Kamer %s is niet geregistreerd.
CHAN_X_NOT_IN_USE
	Kamer %s is momenteel niet in gebruik.
CHAN_X_FORBIDDEN
	Kamer %s mag niet geregistreerd of gebruikt worden.
CHAN_X_SUSPENDED
	Kamer %s is buiten werking gesteld en mag niet gebruikt worden of voor worden geïdentificeerd.
CHAN_X_SUSPENDED_MEMOS
	Kamer %s is buiten werking gesteld en mag geen memo's ontvangen.
CHAN_IDENTIFY_REQUIRED
	Wachtwoordauthenticatie vereist voor dit commando.
	Probeer nogmaals na het tikken van /msg %s IDENTIFY %s wachtwoord.

SERV_X_NOT_FOUND
	Server %s niet gevonden.

EXPIRES_NONE
	vervalt niet
EXPIRES_NOW
	vervallen
EXPIRES_IN
	vervalt over %s

LIST_RESULTS
	Einde lijst; %d/%d resultaten getoond.

###########################################################################
#
# NickServ messages
#
###########################################################################

# Automatic messages
NICK_IS_REGISTERED
	Deze bijnaam is eigendom van iemand anders. Kies een andere. (Indien dit uw bijnaam is, tik /msg %s IDENTIFY wachtwoord.)
NICK_IS_SECURE
	Deze bijnaam is geregistreerd en beschermd.  Indien dit uw bijnaam is, tik /msg %s IDENTIFY wachtwoord.  Zo niet, kies een andere bijnaam.
NICK_MAY_NOT_BE_USED
	Deze bijnaam mag niet worden gebruikt. Kies een andere.
DISCONNECT_IN_1_MINUTE
	Wanneer u uw bijnaam niet binnen één minuut wijzigt wordt uw verbinding verbroken.
DISCONNECT_IN_20_SECONDS
	Wanneer u uw bijnaam niet binnen 20 seconden wijzigt wordt uw verbinding verbroken.
DISCONNECT_NOW
	Deze bijnaam is geregistreerd. U mag deze niet gebruiken. Kies een andere bijnaam voor u opnieuw verbinding maakt.
FORCENICKCHANGE_IN_1_MINUTE
	Wanneer u uw bijnaam niet binnen één minuut wijzigt wordt deze automatisch gewijzigd.
FORCENICKCHANGE_IN_20_SECONDS
	Wanneer u uw bijnaam niet binnen 20 seconden wijzigt wordt deze automatisch gewijzigd.
FORCENICKCHANGE_NOW
	Deze bijnaam is geregistreerd. U mag deze niet gebruiken. Uw bijnaam wordt nu gewijzigd naar %s.
NICK_EXPIRES_SOON
	Opmerking: De registratie van uw bijnaam vervalt over %s.  Identificeer bij %s door het tikken van: /msg %s IDENTIFY wachtwoord indien u uw bijnaam wilt blijven gebruiken.
NICK_EXPIRED
	Opmerking: De registratie van uw bijnaam is vervallen.  Registreer uw bijnaam opnieuw indien u deze wilt blijven gebruiken.

# REGISTER responses
NICK_REGISTER_SYNTAX
	REGISTER wachtwoord [e-mail]
NICK_REGISTER_REQ_EMAIL_SYNTAX
	REGISTER wachtwoord e-mail
NICK_REGISTRATION_DISABLED
	Sorry, bijnaamregistratie is tijdelijk uitgeschakeld.
NICK_REGISTRATION_FAILED
	Sorry, registratie mislukt.
NICK_REG_PLEASE_WAIT
	Wacht %s voordat u het commando REGISTER opnieuw gebruikt.
NICK_REG_PLEASE_WAIT_FIRST
	Wacht %s voordat u het commando REGISTER gebruikt.
NICK_CANNOT_BE_REGISTERED
	Bijnaam %s mag niet worden geregistreerd.
NICK_REGISTER_EMAIL_UNAUTHED
	Het opgegeven e-mailadres is nog niet geauthenticeerd.  Gebruik het commando AUTH om het adres te authenticeren voordat u er nieuwe bijnamen mee registreert.
NICK_REGISTER_TOO_MANY_NICKS
	U heeft teveel bijnamen geregistreerd (%d, maximum is %d).  U kunt er niet meer registreren voordat u één of meer van uw huidige bijnamen verwijdert.
NICK_REGISTERED
	Bijnaam %s is voor u geregistreerd.
NICK_PASSWORD_IS
	Uw wachtwoord is %s -- onthoudt dit voor later gebruik.

# IDENTIFY responses
NICK_IDENTIFY_SYNTAX
	IDENTIFY wachtwoord
NICK_IDENTIFY_FAILED
	Sorry, identificatie mislukt.
NICK_IDENTIFY_EMAIL_MISSING
	U dient een e-mailadres in te stellen voor uw bijnaam voor u deze verder gebruikt.  Stel dit in met het commando: /msg %s SET EMAIL adres
NICK_IDENTIFY_SUCCEEDED
	Wachtwoord geaccepteerd -- u bent nu herkend.

# DROP responses
NICK_DROP_SYNTAX
	DROP wachtwoord
NICK_DROP_WARNING
	Opmerking: Het commando DROP maakt de registratie van al uw gekoppelde bijnamen ongedaan!
NICK_DROP_DISABLED
	Sorry, ongedaan maken van bijnaamregistratie is tijdelijk uitgeschakeld.
NICK_DROP_FAILED
	Sorry, ongedaan maken van bijnaamregistratie is mislukt.
NICK_DROPPED
	Uw bijnaam is verwijderd.
NICK_DROPPED_LINKS
	Uw bijnaam en alle gekoppelde bijnamen zijn verwijderd.

# DROPNICK responses
NICK_DROPNICK_SYNTAX
	DROPNICK bijnaam
NICK_X_DROPPED
	Bijnaam %s is verwijderd.
NICK_X_DROPPED_LINKS
	Bijnaam %s en alle gekoppelde bijnamen zijn verwijderd.

# DROPEMAIL and DROPEMAIL-CONFIRM responses
NICK_DROPEMAIL_SYNTAX
	DROPEMAIL patroon
NICK_DROPEMAIL_PATTERN_TOO_LONG
	Het gegeven patroon is te lang (maximaal %d karakters).
NICK_DROPEMAIL_NONE
	Er zijn geen bijnamen met e-mailadressen die overeenkomen met het opgegeven patroon.
NICK_DROPEMAIL_COUNT
	%d bijnamen hebben e-mailadressen die overeenkomen met het opgegeven patroon.
	Om deze bijnamen te verwijderen, tik: /msg %s DROPEMAIL-CONFIRM %s
NICK_DROPEMAIL_CONFIRM_SYNTAX
	DROPEMAIL-CONFIRM adres
NICK_DROPEMAIL_CONFIRM_UNKNOWN
	Het gegeven patroon is niet herekend.  Gebruik het commando DROPEMAIL opnieuw met dit patroon voordat u het commando DROPEMAIL-CONFIRM gebruikt.
NICK_DROPEMAIL_CONFIRM_DROPPING
	Bezig met verwijderen van %d bijnamen...
NICK_DROPEMAIL_CONFIRM_DROPPED
	Bijnaamverwijdering voltooid.

# SET responses
NICK_SET_SYNTAX
	SET optie parameters
NICK_SET_OPER_SYNTAX
	SET [!bijnaam] optie parameters
NICK_SET_DISABLED
	Sorry, wijzigen van bijnaamopties is tijdelijk uitgeschakeld.
NICK_SET_UNKNOWN_OPTION
	Onbekende optie voor SET %s.

# SET PASSWORD responses
NICK_SET_PASSWORD_FAILED
	Sorry, het wachtwoord kon niet worden gewijzigd.
NICK_SET_PASSWORD_CHANGED
	Wachtwoord gewijzigd.
NICK_SET_PASSWORD_CHANGED_TO
	Wachtwoord gewijzigd naar %s.

# SET LANGUAGE responses
NICK_SET_LANGUAGE_SYNTAX
	SET LANGUAGE nummer
NICK_SET_LANGUAGE_UNKNOWN
	Onbekend taalnummer %d.  Tik /msg %s HELP SET LANGUAGE voor een lijst met talen.
NICK_SET_LANGUAGE_CHANGED
	Taal gewijzigd naar %s.

# SET URL responses
NICK_SET_URL_CHANGED
	URL voor %s gewijzigd naar %s.

# SET EMAIL responses
NICK_SET_EMAIL_PLEASE_WAIT
	Wacht %s voordat u het commando SET EMAIL opnieuw gebruikt.
NICK_SET_EMAIL_UNAUTHED
	Het opgegeven e-mailadres is niet geauthenticeerd.  Gebruik eerst het commando AUTH om het adres te authenticeren.
NICK_SET_EMAIL_TOO_MANY_NICKS
	%s heeft al %d geregistreerde bijnamen (maximum is %d).  E-mailadreswijziging is mislukt.
NICK_SET_EMAIL_CHANGED
	E-mailadres voor %s gewijzigd naar %s.

# SET INFO responses
NICK_SET_INFO_CHANGED
	Informatie voor %s gewijzigd.

# SET KILL responses
NICK_SET_KILL_SYNTAX
	SET KILL {ON | QUICK | OFF}
NICK_SET_KILL_IMMED_SYNTAX
	SET KILL {ON | QUICK | IMMED | OFF}
NICK_SET_KILL_ON
	Killbescherming staat nu op ON (aan).
NICK_SET_KILL_QUICK
	Killbescherming staat nu op ON (aan), met een gereduceerde vertraging.
NICK_SET_KILL_IMMED
	Killbescherming staat nu op ON (aan), zonder vetraging.
NICK_SET_KILL_IMMED_DISABLED
	De optie IMMED is niet beschikbaar op dit netwerk.
NICK_SET_KILL_OFF
	Killbescherming staat nu op OFF (uit).

# SET SECURE responses
NICK_SET_SECURE_SYNTAX
	SET SECURE {ON | OFF}
NICK_SET_SECURE_ON
	De optie SECURE (beveiligd) staat nu op ON (aan).
NICK_SET_SECURE_OFF
	De optie SECURE (beveiligd) staat nu op OFF (uit).

# SET PRIVATE responses
NICK_SET_PRIVATE_SYNTAX
	SET PRIVATE {ON | OFF}
NICK_SET_PRIVATE_ON
	De optie PRIVATE (privé) staat nu op ON (aan).
NICK_SET_PRIVATE_OFF
	De optie PRIVATE (privé) staat nu op OFF (off).

# SET NOOP responses
NICK_SET_NOOP_SYNTAX
	SET NOOP {ON | OFF}

# SET HIDE responses
NICK_SET_HIDE_SYNTAX
	SET HIDE {EMAIL | USERMASK | QUIT} {ON | OFF}
NICK_SET_HIDE_EMAIL_ON
	Uw e-mailadres wordt nu verborgen in %s INFO-overzichten.
NICK_SET_HIDE_EMAIL_OFF
	Uw e-mailadres wordt nu getoond in %s INFO-overzichten.
NICK_SET_HIDE_MASK_ON
	Uw laatst gezien gebruiker@machine-masker wordt nu verborgen in %s INFO-overzichten.
NICK_SET_HIDE_MASK_OFF
	Uw laatst gezien gebruiker@machine-masker wordt nu getoond in %s INFO-overzichten.
NICK_SET_HIDE_QUIT_ON
	Uw laatste vetrekbericht wordt nu verborgen in %s INFO-overzichten.
NICK_SET_HIDE_QUIT_OFF
	Uw laatste vetrekbericht wordt nu getoond in %s INFO-overzichten.

# SET TIMEZONE responses
NICK_SET_TIMEZONE_SYNTAX
	SET TIMEZONE {UTC-offset | tijdzone | DEFAULT}
NICK_SET_TIMEZONE_TO
	Uw tijdzone is ingesteld op UTC%c%d:%02d.
	De huidige tijd in deze tijdzone is %s.
NICK_SET_TIMEZONE_DEFAULT
	Uw tijdzone is ingesteld op die van Services.

# SET MAINNICK responses
NICK_SET_MAINNICK_NOT_FOUND
	De bijnaam %s is niet gekoppeld aan uw huidige bijnaam.
NICK_SET_MAINNICK_CHANGED
	Uw hoofdbijnaam is gewijzigd naar %s.

# SET NOEXPIRE responses
NICK_SET_NOEXPIRE_SYNTAX
	SET [!bijnaam] NOEXPIRE {ON | OFF}
NICK_SET_NOEXPIRE_ON
	Bijnaam %s zal niet vervallen.
NICK_SET_NOEXPIRE_OFF
	Bijnaam %s zal vervallen.

# UNSET responses
NICK_UNSET_SYNTAX
	UNSET {URL | EMAIL | INFO}
NICK_UNSET_SYNTAX_REQ_EMAIL
	UNSET {URL | INFO}
NICK_UNSET_OPER_SYNTAX
	UNSET [!bijnaam] {URL | EMAIL | INFO}
NICK_UNSET_OPER_SYNTAX_REQ_EMAIL
	UNSET [!bijnaam] {URL | INFO}
NICK_UNSET_URL
	URL voor %s uitgezet.
NICK_UNSET_EMAIL
	E-mailadres voor %s uitgezet.
NICK_UNSET_INFO
	Informatie voor %s uitgezet.
NICK_UNSET_EMAIL_BAD
	U mag uw e-mailadres niet verwijderen.
NICK_UNSET_EMAIL_OTHER_BAD
	E-mailadressen van bijnamen mogen niet worden verwijderd.

# ACCESS responses
NICK_ACCESS_SYNTAX
	ACCESS {ADD | DEL | LIST} [masker]
NICK_ACCESS_DISABLED
	Sorry, wijziging van bijnaam-toegangslijsten is tijdelijk uitgeschakeld.
NICK_ACCESS_ALREADY_PRESENT
	Masker %s staat al op uw toegangslijst.
NICK_ACCESS_REACHED_LIMIT
	Sorry, u kunt slechts %d toegangsinvoeren hebben voor een bijnaam.
NICK_ACCESS_NO_NICKS
	Waarschuwing: Toegangsmaskers voor bijnamen dienen geen bijnamen te bevatten.  Controleer of u het gebruiker@machine-masker juist heeft ingevoerd.
NICK_ACCESS_ADDED
	%s toegevoegd aan uw toegangslijst.
NICK_ACCESS_NOT_FOUND
	%s niet gevonden in uw toegangslijst.
NICK_ACCESS_DELETED
	%s verwijderd uit uw toegangslijst.
NICK_ACCESS_LIST_EMPTY
	Uw toegangslijst is leeg.
NICK_ACCESS_LIST
	Toegangslijst:
NICK_ACCESS_LIST_X_EMPTY
	Toegangslijst voor %s is leeg.
NICK_ACCESS_LIST_X
	Toegangslijst voor %s:

# LINK responses
NICK_OLD_LINK_SYNTAX
	LINK bijnaam wachtwoord
NICK_LINK_SYNTAX
	LINK bijnaam
NICK_LINK_DISABLED
	Sorry, koppelen van bijnamen is tijdelijk uitgeschakeld.
NICK_LINK_FAILED
	Sorry, koppelen mislukt.
NICK_CANNOT_BE_LINKED
	Bijnaam %s mag niet worden gekoppeld.
NICK_OLD_LINK_SAME
	U kunt niet koppelen aan uw huidige bijnaam!
NICK_LINK_SAME
	U kunt uw huidige bijnaam niet koppelen!
NICK_LINK_ALREADY_LINKED
	Bijnaam %s is al gekoppeld aan uw bijnaam.
NICK_LINK_IN_USE
	Bijnaam %s is momenteel in gebruik.
NICK_LINK_TOO_MANY
	U kunt niet meer dan %d bijnamen aan elkaar koppelen.
NICK_LINK_TOO_MANY_NICKS
	U heeft al %d bijnamen geregistreerd (maximum is %d).  Koppelen mislukt.
NICK_OLD_LINK_TOO_MANY_CHANNELS
	Kan niet koppelen aan %s: te veel geregistreerde kamers (gecombineerd maximum %d).
NICK_OLD_LINKED
	Uw bijnaam is gekoppeld aan %s.
NICK_LINKED
	Bijnaam %s is gekoppeld aan uw bijnaam.

# UNLINK responses
NICK_OLD_UNLINK_SYNTAX
	UNLINK [bijnaam wachtwoord]
NICK_UNLINK_SYNTAX
	UNLINK bijnaam
NICK_UNLINK_OPER_SYNTAX
	UNLINK bijnaam [FORCE]
NICK_UNLINK_DISABLED
	Sorry, verwijderen van bijnaamkoppelingen is tijdelijk uitgeschakeld.
NICK_UNLINK_FAILED
	Sorry, verwijderen van koppeling mislukt.
NICK_UNLINK_SAME
	U kunt UNLINK niet uitvoeren op uw huidige bijnaam!
NICK_OLD_UNLINK_NOT_LINKED
	Uw bijnaam is niet gekoppeld.
NICK_UNLINK_NOT_LINKED_YOURS
	Bijnaam %s is niet gekoppeld aan uw bijnaam.
NICK_UNLINK_NOT_LINKED
	Bijnaam %s is niet gekoppeld.
NICK_OLD_UNLINKED
	De koppeling van uw bijnaam met %s is verwijderd.
NICK_UNLINKED
	De koppeling van bijnaam %s aan uw bijnaam is verwijderd.
NICK_X_UNLINKED
	De koppeling van bijnaam %s aan bijnaam %s is verwijderd.

# LISTLINKS responses
NICK_OLD_LISTLINKS_SYNTAX
	LISTLINKS bijnaam
NICK_LISTLINKS_SYNTAX
	LISTLINKS
NICK_LISTLINKS_HEADER
	Lijst van bijnamen gekoppeld aan %s:
NICK_LISTLINKS_FOOTER
	%d gekoppelde bijnamen.

# INFO responses
NICK_INFO_SYNTAX
	INFO bijnaam [ALL]
NICK_INFO_REALNAME
	%s is %s
NICK_INFO_ADDRESS
	Laatst geziene adres: %s
NICK_INFO_ADDRESS_ONLINE
	   Is aanwezig vanaf: %s
NICK_INFO_ADDRESS_ONLINE_NOHOST
	%s is momenteel aanwezig.
NICK_INFO_ADDRESS_OTHER_NICK
	Gebruikt bijnaam: %s
NICK_INFO_TIME_REGGED
	  Geregistreerd: %s
NICK_INFO_LAST_SEEN
	   Laatst gezien: %s
NICK_INFO_LAST_QUIT
	Laatste vertrekboodschap: %s
NICK_INFO_URL
	              URL: %s
NICK_INFO_EMAIL
	   E-mailadres: %s
NICK_INFO_EMAIL_UNAUTHED
	   E-mailadres: %s (ongeverifieerd)
NICK_INFO_INFO
	      Informatie: %s
NICK_INFO_OPTIONS
	          Opties: %s
# These strings MUST NOT be empty
NICK_INFO_OPT_KILL
	Killbescherming
NICK_INFO_OPT_SECURE
	Beveiliging
NICK_INFO_OPT_PRIVATE
	Privé
NICK_INFO_OPT_NONE
	Geen
NICK_INFO_NO_EXPIRE
	Deze bijnaam vervalt niet.
NICK_INFO_SUSPEND_DETAILS
	Buiten werking gesteld door %s op %s (%s).
NICK_INFO_SUSPEND_REASON
	Reden: %s
NICK_INFO_SHOW_ALL
	Voor meer informatie, tik: /msg %s INFO %s ALL

# LISTCHANS responses
NICK_LISTCHANS_SYNTAX
	LISTCHANS
NICK_LISTCHANS_NONE
	%s heeft geen kamers geregistreerd.
NICK_LISTCHANS_HEADER
	Kamers geregistreerd door %s:
NICK_LISTCHANS_END
	Einde lijst (%d kamers geregistreerd).

# LIST/LISTEMAIL responses
NICK_LIST_SYNTAX
	%s [+start] patroon
NICK_LIST_OPER_SYNTAX
	%s [+start] patroon [FORBIDDEN] [NOEXPIRE] [SUSPENDED]
NICK_LIST_OPER_SYNTAX_AUTH
	%s [+start] patroon [FORBIDDEN] [NOEXPIRE] [SUSPENDED] [NOAUTH]
NICK_LIST_HEADER
	Lijst van invoeren overeenkomend met %s:
NICK_LIST_NO_MATCH
	Geen overeenkomende bijnamen gevonden.
NICK_LISTEMAIL_NONE
	(niet ingesteld)

# RECOVER responses
NICK_RECOVER_SYNTAX
	RECOVER bijnaam [wachtwoord]
NICK_NO_RECOVER_SELF
	U kunt RECOVER niet uitvoeren op uzelf!
NICK_RECOVERED
	De gebruiker die uw bijnaam opeisde is verwijderd van het netwerk.  Tik /msg %s RELEASE %s wachtwoord om de bijnaam terug te krijgen voor de pauzeperiode van één minuut.

# RELEASE responses
NICK_RELEASE_SYNTAX
	RELEASE bijnaam [wachtwoord]
NICK_RELEASE_NOT_HELD
	Bijnaam %s wordt niet vastgehouden.
NICK_RELEASED
	Services houdt uw bijnaam niet langer vast.

# GHOST responses
NICK_GHOST_SYNTAX
	GHOST bijnaam [wachtwoord]
NICK_NO_GHOST_SELF
	U kunt GHOST niet gebruiken op uzelf!
NICK_GHOST_KILLED
	De "ghost" met uw bijnaam is verwijderd.

# GETPASS responses
NICK_GETPASS_SYNTAX
	GETPASS bijnaam
NICK_GETPASS_UNAVAILABLE
	Het commando GETPASS kan niet gebruikt worden voor bijnaam %s omdat encryptie in gebruik is.
NICK_GETPASS_PASSWORD_IS
	Wachtwoord voor %s is %s.

# FORBID responses
NICK_FORBID_SYNTAX
	FORBID bijnaam
NICK_FORBID_SUCCEEDED
	Bijnaam %s is nu verboden.
NICK_FORBID_FAILED
	Kon geen verbod zetten op bijnaam %s!

# SUSPEND responses
NICK_SUSPEND_SYNTAX
	SUSPEND [+expiry] bijnaam reden
NICK_SUSPEND_SUCCEEDED
	Bijnaam %s en alle hieraan gekoppelde bijnamen zijn buiten werking gesteld.
NICK_SUSPEND_ALREADY_SUSPENDED
	Bijnaam %s is reeds buiten werking gesteld.

# UNSUSPEND responses
NICK_UNSUSPEND_SYNTAX
	UNSUSPEND bijnaam
NICK_UNSUSPEND_SUCCEEDED
	Bijnaam %s is niet langer buiten werking gesteld.
NICK_UNSUSPEND_NOT_SUSPENDED
	Bijnaam %s is niet buiten werking gesteld.

################ autojoin module messages/responses

# AJOIN responses
NICK_AJOIN_SYNTAX
	AJOIN {ADD | DEL | LIST} [kamer]
NICK_AJOIN_ADD_SYNTAX
	AJOIN ADD kamer
NICK_AJOIN_DEL_SYNTAX
	AJOIN DEL kamer
NICK_AJOIN_DISABLED
	Sorry, wijziging van de autojoinlijst voor bijnamen is tijdelijk uitgeschakeld.
NICK_AJOIN_ALREADY_PRESENT
	Kamer %s staat al op uw autojoinlijst.
NICK_AJOIN_LIST_FULL
	Sorry, u kunt niet meer dan %d items invoeren in de autojoinlijst voor een bijnaam.
NICK_AJOIN_ADDED
	%s toegevoegd aan uw autojoinlijst.
NICK_AJOIN_NOT_FOUND
	%s niet gevonden in uw autojoinlijst.
NICK_AJOIN_DELETED
	%s verwijderd uit uw autojoinlijst
NICK_AJOIN_LIST_EMPTY
	Uw autojoinlijst is leeg.
NICK_AJOIN_LIST
	Autojoinlijst:
NICK_AJOIN_LIST_X_EMPTY
	Autojoinlijst voor %s is leeg.
NICK_AJOIN_LIST_X
	Autojoinlijst voor %s:
NICK_AJOIN_AUTO_REMOVE
	Kamer %s is verwijderd uit uw autojoinlijst, omdat dit geen geldige kamernaam is.

################ mail-auth module messages/responses

# General-purpose messages
NICK_AUTH_SENDING
	Een authenticatiecode voor uw bijnaam wordt verstuurd naar %s.
NICK_AUTH_FOR_REGISTER
	Wanneer u dit bericht ontvangt, tik /msg %s AUTH code (vervang code door de authenticatiecode in het bericht) om uw bijnaamregistratie te voltooien.
NICK_AUTH_FOR_SET_EMAIL
	Wanneer u dit bericht ontvangt, tik /msg %s AUTH code (vervang code door de authenticatiecode in het bericht) om de wijziging van uw e-mailadres te voltooien.
	Let op dat u uw bijnaam niet kunt blijven gebruiken tenzij u de wijziging voltooit.
NICK_AUTH_NOT_NEEDED
	Er is momenteel niets dat authenticatie behoeft.
NICK_AUTH_NO_AUTHCODE
	Bijnaam %s heeft geen authenticatiecode.
NICK_AUTH_HAS_AUTHCODE
	Bijnaam %s heeft al een authenticatiecode.
PASSWORD_WARNING_FOR_AUTH
	Waarschuwing: Bij de volgende incorrecte authenticatiecode wordt uw verbinding verbroken.

# Mail text.  The last "%s" (before the user@host) in the body text is
# replaced by one of the NICK_AUTH_MAIL_TEXT_* messages.
NICK_AUTH_MAIL_SUBJECT
	Authenticatiecode voor %s
NICK_AUTH_MAIL_BODY
	De authenticatiecode voor uw bijnaam (%s) is: %09d
	Stuur deze code naar %s met het commando:
	    /msg %s AUTH %09d
	
	Dit bericht is verstuurd door %s als reactie op %s
	%s@%s.
NICK_AUTH_MAIL_TEXT_REG
	registratie door
NICK_AUTH_MAIL_TEXT_EMAIL
	een wijziging van e-mailadres door
NICK_AUTH_MAIL_TEXT_SENDAUTH
	gebruik van het commando SENDAUTH door
NICK_AUTH_MAIL_TEXT_REAUTH
	gebruik van het commando REAUTH door
NICK_AUTH_MAIL_BODY_SETAUTH
	De authenticatiecode voor uw bijnaam (%s) is: %09d
	Stuur deze code naar %s met het commando:
	    /msg %s AUTH %09d

# AUTH responses
NICK_AUTH_SYNTAX
	AUTH code
NICK_AUTH_DISABLED
	Sorry, bijnaamauthenticatie is tijdelijk uitgeschakeld.
NICK_AUTH_FAILED
	Deze authenticatiecode is onjuist.
NICK_AUTH_SUCCEEDED_REGISTER
	Authenticatie geslaagd. De registratie van uw bijnaam is nu voltooid.
NICK_AUTH_SUCCEEDED_SET_EMAIL
	Authenticatie geslaagd. De wijziging van uw e-mailadres is nu voltooid.
NICK_AUTH_SUCCEEDED_REAUTH
	Authenticatie geslaagd. U bent nu geïdentificeerd voor uw bijnaam.
NICK_AUTH_SUCCEEDED_SETAUTH
	Authenticatie geslaagd. U mag uw bijnaam nu blijven gebruiken.

# SENDAUTH responses
NICK_SENDAUTH_SYNTAX
	SENDAUTH
NICK_SENDAUTH_TOO_SOON
	Wacht %s voor u het commando SENDAUTH opnieuw gebruikt.
NICK_SENDAUTH_NORESOURCES
	Het bericht met uw authenticatiecode kon niet verstuurd worden, omdat het systeem het te druk heeft.  Probeert u het later nogmaals of neem contact op met een netwerkbeheerder.
NICK_SENDAUTH_ERROR
	Het bericht met uw authenticatiecode kon niet verstuurd worden vanwege een systeemprobleem.  Probeert u het later nogmaals of neem contact op met een netwerkbeheerder.

# REAUTH responses
NICK_REAUTH_SYNTAX
	REAUTH
NICK_REAUTH_HAVE_AUTHCODE
	Er is al een authenticatiecode ingesteld voor uw bijnaam.
NICK_REAUTH_NO_EMAIL
	Er is geen e-mailadres ingesteld voor uw bijnaam.
NICK_REAUTH_AUTHCODE_SET
	Een nieuwe authenticatiecode is ingesteld voor uw bijnaam en wordt aanstonds naar u verstuurd.  Gebruik deze code met het commando AUTH om u te identificeren voor uw bijnaam.

# RESTOREMAIL responses
NICK_RESTOREMAIL_SYNTAX
	RESTOREMAIL wachtwoord
NICK_RESTOREMAIL_NOT_NOW
	RESTOREMAIL kan alleen gebruikt worden na wijziging van uw e-mailadres en voor authenticatie van het nieuwe adres.
NICK_RESTOREMAIL_DONE
	Uw vorige e-mailadres %s is teruggezet.

# SETAUTH responses
NICK_SETAUTH_SYNTAX
	SETAUTH bijnaam
NICK_SETAUTH_NO_EMAIL
	Er is geen e-mailadres ingesteld voor bijnaam %s.
NICK_SETAUTH_AUTHCODE_SET
	Een nieuwe authenticatiecode (%09d) is ingesteld voor bijnaam %s.
NICK_SETAUTH_SEND_REFUSED
	Waarschuwing: Het was niet mogelijk e-mail te versturen naar %s (bericht geweigerd door de mailserver).
NICK_SETAUTH_SEND_TIMEOUT
	Waarschuwing: Het was niet mogelijk e-mail te versturen naar %s (verbinding maken met of versturen naar de mailserver duurde te lang).
NICK_SETAUTH_SEND_NORESOURCES
	Waarschuwing: Het was niet mogelijk e-mail te versturen naar %s (ontoereikende systeembronnen).
NICK_SETAUTH_SEND_ERROR
	Waarschuwing: Het was niet mogelijk e-mail te versturen naar %s (onbekende fout).
NICK_SETAUTH_USER_NOTICE
	U moet zich authenticeren voor uw bijnaam voordat u deze verder gebruikt.  Er wordt een authenticatiecode verstuurd naar uw e-mailadres: %s
	Tik /msg %s HELP AUTH voor meer informatie.

# GETAUTH responses
NICK_GETAUTH_SYNTAX
	GETAUTH bijnaam
NICK_GETAUTH_AUTHCODE_IS
	De authenticatiecode voor bijnaam %s is: %09d

# CLEARAUTH responses
NICK_CLEARAUTH_SYNTAX
	CLEARAUTH bijnaam
NICK_CLEARAUTH_CLEARED
	De authenticatiecode voor bijnaam %s is verwijderd.

###########################################################################
#
# ChanServ messages
#
###########################################################################

# Access level descriptions
CHAN_LEVEL_AUTOOP
	Automatische kameroperator-status
CHAN_LEVEL_AUTOVOICE
	Automatische modus +v
CHAN_LEVEL_INVITE
	Mag het commando INVITE gebruiken
CHAN_LEVEL_AKICK
	Mag het commando AKICK gebruiken
CHAN_LEVEL_SET
	Mag het commando SET gebruiken (niet FOUNDER/PASSWORD)
CHAN_LEVEL_CLEAR
	Mag het commando CLEAR gebruiken
CHAN_LEVEL_UNBAN
	Mag het commando UNBAN gebruiken
CHAN_LEVEL_OPDEOP
	Mag de commando's OP/DEOP gebruiken
CHAN_LEVEL_ACCESS_LIST
	Mag de toegangslijst bekijken
CHAN_LEVEL_ACCESS_CHANGE
	Mag de toegangslijst aanpassen
CHAN_LEVEL_MEMO
	Ontvangt kamermemo's
CHAN_LEVEL_VOICE
	Mag de commando's VOICE/DEVOICE gebruiken
CHAN_LEVEL_AUTOHALFOP
	Automatische halfop (+h) status
CHAN_LEVEL_HALFOP
	Mag de commando's HALFOP/DEHALFOP gebruiken
CHAN_LEVEL_AUTOPROTECT
	Automatische kamerbeschermingsstatus (+a)
CHAN_LEVEL_PROTECT
	Mag de commando's PROTECT/DEPROTECT gebruiken
CHAN_LEVEL_KICK
	Mag het commando KICK gebruiken
CHAN_LEVEL_STATUS
	Mag het commando STATUS gebruiken
CHAN_LEVEL_TOPIC
	Mag het commando TOPIC gebruiken

# Automatic responses
CHAN_IS_REGISTERED
	Deze kamer is geregistreerd met %s.
CHAN_MAY_NOT_BE_USED
	Deze kamer mag niet worden gebruikt.
CHAN_NOT_ALLOWED_TO_JOIN
	U heeft geen permissie om aanwezig te zijn in deze kamer.

# Multi-purpose responses
CHAN_MUST_IDENTIFY_NICK
	Identificeer u eerst met %s, door gebruik van het commando:
	/msg %s IDENTIFY wachtwoord
CHAN_BOUNCY_MODES
	Sorry, het commando %s tijdelijk niet beschikbaar.

# REGISTER responses
CHAN_REGISTER_SYNTAX
	REGISTER kamer wachtwoord beschrijving
CHAN_REGISTER_DISABLED
	Sorry, kamerregistratie is tijdelijk uitgeschakeld.
CHAN_REGISTER_SHORT_CHANNEL
	De kamer # mag niet worden geregistreerd.
CHAN_REGISTER_NOT_LOCAL
	Lokale kamers kunnen niet worden geregistreerd.
CHAN_REGISTER_INVALID_NAME
	U mag alleen kamers registreren waarvan de naam begint met #.
CHAN_MUST_REGISTER_NICK
	U dient eerst uw bijnaam te registreren.  Tik /msg %s HELP voor informatie over het registreren van bijnamen.
CHAN_MAY_NOT_BE_REGISTERED
	Kamer %s mag niet worden geregistreerd.
CHAN_ALREADY_REGISTERED
	Kamer %s is reeds geregistreerd!
CHAN_MUST_BE_CHANOP
	U dient kameroperator te zijn om de kamer te registreren.
CHAN_REACHED_CHANNEL_LIMIT
	Sorry, u heeft uw limiet van %d kamers reeds bereikt.
CHAN_EXCEEDED_CHANNEL_LIMIT
	Sorry, u heeft uw limiet van %d kamers reeds overschreden.
CHAN_REGISTRATION_FAILED
	Sorry, registratie mislukt.
CHAN_REGISTERED
	Kamer %s is geregistreerd op uw bijnaam: %s
CHAN_PASSWORD_IS
	Uw kamerwachtwoord is %s -- Onthoudt dit voor later gebruik.

# IDENTIFY responses
CHAN_IDENTIFY_SYNTAX
	IDENTIFY kamer wachtwoord
CHAN_IDENTIFY_FAILED
	Sorry, identificatie mislukt.
CHAN_IDENTIFY_SUCCEEDED
	Wachtwoord geaccepteerd -- u heeft nu toegang op oprichterniveau in %s.

# DROP responses
CHAN_DROP_SYNTAX
	DROP kamer wachtwoord
CHAN_DROP_DISABLED
	Sorry, ongedaanmaken van kamerregistratie is tijdelijk uitgeschakeld.
CHAN_DROPPED
	Kamer %s is verwijderd.

# DROPCHAN responses
CHAN_DROPCHAN_SYNTAX
	DROPCHAN kamer

# SET responses
CHAN_SET_SYNTAX
	SET kamer optie parameters
CHAN_SET_DISABLED
	Sorry, instellen van kameropties is tijdelijk uitgeschakeld.
CHAN_SET_UNKNOWN_OPTION
	Onbekende optie voor SET: %s.

# SET FOUNDER responses
CHAN_SET_FOUNDER_TOO_MANY_CHANS
	%s heeft teveel geregistreerde kamers.
CHAN_FOUNDER_CHANGED
	Oprichter voor %s gewijzigd naar %s.

# SET SUCCESSOR responses
CHAN_SUCCESSOR_CHANGED
	Opvolger voor %s gewijzigd naar %s.
CHAN_SUCCESSOR_IS_FOUNDER
	U kunt de oprichter van een kamer niet ook opvolger maken.

# SET PASSWORD responses
CHAN_SET_PASSWORD_FAILED
	Sorry, kon het wachtwoord niet instellen.
CHAN_PASSWORD_CHANGED
	%s wachtwoord gewijzigd.
CHAN_PASSWORD_CHANGED_TO
	%s wachtwoord gewijzigd naar %s.

# SET DESC responses
CHAN_DESC_CHANGED
	Beschrijving voor %s gewijzigd naar %s.

# SET URL responses
CHAN_URL_CHANGED
	URL voor %s gewijzigd naar %s.

# SET EMAIL responses
CHAN_EMAIL_CHANGED
	E-mailadres voor %s gewijzigd naar %s.

# SET ENTRYMSG responses
CHAN_ENTRY_MSG_CHANGED
	Binnenkomstbericht voor %s gewijzigd.

# SET MLOCK responses
CHAN_SET_MLOCK_NEED_PLUS_MINUS
	Modilijst dient te beginnen met een + (plus) of - (min).
CHAN_SET_MLOCK_NEED_PARAM
	Parameter vereist voor MLOCK +%c.
CHAN_SET_MLOCK_NEED_POSITIVE
	Parameter voor MLOCK +%c dient een positief nummer te zijn.
CHAN_SET_MLOCK_MODE_REG_BAD
	Modus +%c is altijd ingesteld op geregistreerde kamers en kan niet gebruikt worden met MLOCK.
CHAN_SET_MLOCK_LINK_BAD
	Parameter voor MLOCK +%c is geen geldige kamernaam.
CHAN_SET_MLOCK_LINK_SAME
	Parameter voor MLOCK +%c dient een andere kamer te zijn.
CHAN_SET_MLOCK_REQUIRES
	MLOCK +%c vereist dat MLOCK +%c ook ingesteld is.
CHAN_SET_MLOCK_BAD_PARAM
	Ongeldige parameter voor MLOCK +%c.
CHAN_SET_MLOCK_UNKNOWN_CHAR
	Onbekend moduskarakter %c genegeerd.
CHAN_SET_MLOCK_CANNOT_LOCK
	Modus %c kan niet worden vastgezet.
CHAN_MLOCK_CHANGED
	Modusslot op kamer %s gewijzigd naar %s.
CHAN_MLOCK_REMOVED
	Modusslot van kamer %s verwijderd.

# SET HIDE responses
CHAN_SET_HIDE_SYNTAX
	SET kamer HIDE {EMAIL | TOPIC | MLOCK} {ON | OFF}
CHAN_SET_HIDE_EMAIL_ON
	E-mailadres voor kamer %s wordt nu verborgen in %s INFO overzichten.
CHAN_SET_HIDE_EMAIL_OFF
	E-mailadres voor kamer %s wordt nu getoond in %s INFO overzichten.
CHAN_SET_HIDE_TOPIC_ON
	Onderwerp voor kamer %s wordt nu verborgen in %s INFO overzichten.
CHAN_SET_HIDE_TOPIC_OFF
	Onderwerp voor kamer %s wordt nu getoond in %s INFO overzichten.
CHAN_SET_HIDE_MLOCK_ON
	Modusslot voor kamer %s wordt nu verborgen in %s INFO overzichten.
CHAN_SET_HIDE_MLOCK_OFF
	Modusslot voor kamer %s wordt nu getoond in %s INFO overzichten.

# SET KEEPTOPIC responses
CHAN_SET_KEEPTOPIC_SYNTAX
	SET kamer KEEPTOPIC {ON | OFF}
CHAN_SET_KEEPTOPIC_ON
	Onderwerpbehoud staat nu op ON (aan).
CHAN_SET_KEEPTOPIC_OFF
	Onderwerpbehoud staat nu op OFF (uit).

# SET TOPICLOCK responses
CHAN_SET_TOPICLOCK_SYNTAX
	SET kamer TOPICLOCK {ON | OFF}
CHAN_SET_TOPICLOCK_ON
	Onderwerpslot staat nu op ON (aan).
CHAN_SET_TOPICLOCK_OFF
	Onderwerpslot staat nu op OFF (uit).

# SET PRIVATE responses
CHAN_SET_PRIVATE_SYNTAX
	SET kamer PRIVATE {ON | OFF}
CHAN_SET_PRIVATE_ON
	Privé-optie staat nu op ON (aan).
CHAN_SET_PRIVATE_OFF
	Privé-optie staat nu op OFF (uit).

# SET SECUREOPS responses
CHAN_SET_SECUREOPS_SYNTAX
	SET kamer SECUREOPS {ON | OFF}
CHAN_SET_SECUREOPS_ON
	Operatorbeveiliging staat nu op ON (aan).
CHAN_SET_SECUREOPS_OFF
	Operatorbeveiliging staat nu op OFF (uit).

# SET LEAVEOPS responses
CHAN_SET_LEAVEOPS_SYNTAX
	SET kamer LEAVEOPS {ON | OFF}
CHAN_SET_LEAVEOPS_ON
	Operatorbehoud staat nu op ON (aan).
CHAN_SET_LEAVEOPS_OFF
	Operatorbehoud staat nu op OFF (uit).

# SET RESTRICTED responses
CHAN_SET_RESTRICTED_SYNTAX
	SET kamer RESTRICTED {ON | OFF}
CHAN_SET_RESTRICTED_ON
	Beperkte toegang staat nu op ON (aan).
CHAN_SET_RESTRICTED_OFF
	Beperkte toegang staat nu op OFF (uit).

# SET SECURE responses
CHAN_SET_SECURE_SYNTAX
	SET kamer SECURE {ON | OFF}
CHAN_SET_SECURE_ON
	Beveiliging staat nu op ON (aan).
CHAN_SET_SECURE_OFF
	Beveiliging staat nu op OFF (uit).

# SET OPNOTICE responses
CHAN_SET_OPNOTICE_SYNTAX
	SET kamer OPNOTICE {ON | OFF}
CHAN_SET_OPNOTICE_ON
	Operatoralarm staat nu op ON (aan).
CHAN_SET_OPNOTICE_OFF
	Operatoralarm staat nu op OFF (uit).

# SET ENFORCE responses
CHAN_SET_ENFORCE_SYNTAX
	SET kamer ENFORCE {ON | OFF}
CHAN_SET_ENFORCE_ON
	Afdwinging staat nu op ON (aan).
CHAN_SET_ENFORCE_OFF
	Afdwinging staat nu op OFF.

# SET MEMO-RESTRICTED responses
CHAN_SET_MEMO_RESTRICTED_SYNTAX
	SET kamer MEMO-RESTRICTED {ON | OFF}

# SET NOEXPIRE responses
CHAN_SET_NOEXPIRE_SYNTAX
	SET kamer NOEXPIRE {ON | OFF}
CHAN_SET_NOEXPIRE_ON
	Kamer %s zal niet vervallen.
CHAN_SET_NOEXPIRE_OFF
	Kamer %s zal wel vervallen.

# UNSET responses
CHAN_UNSET_SYNTAX
	UNSET kamer {SUCCESSOR | URL | EMAIL | ENTRYMSG}
CHAN_SUCCESSOR_UNSET
	Opvolger voor %s uitgezet.
CHAN_URL_UNSET
	URL voor %s uitgezet.
CHAN_EMAIL_UNSET
	E-mailadres voor %s uitgezet.
CHAN_ENTRY_MSG_UNSET
	Binnenkomstbericht voor %s uitgezet.

# ACCESS messages
CHAN_ACCESS_SYNTAX
	ACCESS kamer {ADD | DEL | LIST | LISTLEVEL | COUNT} [parameters]
CHAN_ACCESS_ADD_SYNTAX
	ACCESS kamer ADD bijnaam niveau
CHAN_ACCESS_DEL_SYNTAX
	ACCESS kamer DEL bijnaam
CHAN_ACCESS_LIST_SYNTAX
	ACCESS kamer LIST [+begin] [masker]
CHAN_ACCESS_LISTLEVEL_SYNTAX
	ACCESS kamer LISTLEVEL [+begin] niveau
CHAN_ACCESS_COUNT_SYNTAX
	ACCESS kamer COUNT
CHAN_ACCESS_DISABLED
	Sorry, aanpassing van kamertoegangslijsten is tijdelijk uitgeschakeld.
CHAN_ACCESS_LEVEL_NONZERO
	Toegangsniveau mag geen nul zijn.
CHAN_ACCESS_LEVEL_RANGE
	Toegangsniveau dient tussen %d en %d te liggen.
CHAN_ACCESS_NICKS_ONLY
	Kamertoegangslijsten mogen alleen geregistreerde bijnamen bevatten.
CHAN_ACCESS_REACHED_LIMIT
	Sorry, u kunt slechts %d invoeren toevoegen aan de toegangslijst van een kamer.
CHAN_ACCESS_LEVEL_UNCHANGED
	Toegangsniveau voor %s in %s is ongewijzigd %d.
CHAN_ACCESS_LEVEL_CHANGED
	Toegangsniveau voor %s in %s gewijzigd naar %d.
CHAN_ACCESS_ADDED
	%s toegevoegd aan de toegangslijst voor %s op niveau %d.
CHAN_ACCESS_NOT_FOUND
	%s niet gevonden in de toegangslijst voor %s.
CHAN_ACCESS_DELETED
	%s verwijderd uit de toegangslijst voor %s.
CHAN_ACCESS_LIST_EMPTY
	Toegangslijst voor %s is leeg.
CHAN_ACCESS_NO_MATCH
	Geen overeenkomende invoeren gevonden in de toegangslijst van %s.
CHAN_ACCESS_LIST_HEADER
	Toegangslijst voor %s:
	    Niveau  Bijnaam
CHAN_ACCESS_LIST_FORMAT
	     %4d  %s
CHAN_ACCESS_COUNT
	De toegangslijst voor %s heeft %d invoeren.

# XOP (SOP/AOP/etc.) responses
CHAN_SOP_SYNTAX
	SOP kamer {ADD | DEL | LIST | COUNT} [parameters]
CHAN_AOP_SYNTAX
	AOP kamer {ADD | DEL | LIST | COUNT} [parameters]
CHAN_HOP_SYNTAX
	HOP kamer {ADD | DEL | LIST | COUNT} [parameters]
CHAN_VOP_SYNTAX
	VOP kamer {ADD | DEL | LIST | COUNT} [parameters]
CHAN_NOP_SYNTAX
	NOP kamer {ADD | DEL | LIST | COUNT} [parameters]
CHAN_SOP_LIST_SYNTAX
	SOP kamer {ADD | DEL | LIST | COUNT} [+begin] [masker]
CHAN_AOP_LIST_SYNTAX
	AOP kamer {ADD | DEL | LIST | COUNT} [+begin] [masker]
CHAN_HOP_LIST_SYNTAX
	HOP kamer {ADD | DEL | LIST | COUNT} [+begin] [masker]
CHAN_VOP_LIST_SYNTAX
	VOP kamer {ADD | DEL | LIST | COUNT} [+begin] [masker]
CHAN_NOP_LIST_SYNTAX
	NOP kamer {ADD | DEL | LIST | COUNT} [+begin] [masker]
CHAN_XOP_ADDED
	%s toegevoegd aan %s %s-lijst.
CHAN_XOP_LEVEL_CHANGED
	%s verplaatst naar %s %s-lijst.
CHAN_XOP_LEVEL_UNCHANGED
	%s staat al op %s %s-lijst.
CHAN_XOP_NICKS_ONLY
	SOP, AOP, VOP, en NOP lijsten mogen alleen geregistreerde bijnamen bevatten.
CHAN_XOP_NICKS_ONLY_HOP
	SOP, AOP, HOP, VOP, en NOP lijsten mogen alleen geregistreerde bijnamen bevatten.
CHAN_XOP_REACHED_LIMIT
	Sorry, het aantal invoeren op uw SOP, AOP, VOP, en NOP lijsten mag niet meer zijn dan %d.  Limiet overschreden.
CHAN_XOP_REACHED_LIMIT_HOP
	Sorry, het aantal invoeren op uw SOP, AOP, HOP, VOP, en NOP lijsten mag niet meer zijn dan %d.  Limiet overschreden.
CHAN_XOP_NOT_FOUND
	%s niet gevonden op %s %s-lijst.
CHAN_XOP_DELETED
	%s verwijderd van %s %s-lijst.
CHAN_XOP_LIST_EMPTY
	%s %s-lijst is leeg.
CHAN_XOP_NO_MATCH
	Geen overeenkomende invoeren op %s %s-lijst.
CHAN_XOP_LIST_HEADER
	%s-lijst voor %s:
CHAN_XOP_COUNT
	De %s %s-lijst heeft %d invoeren.

# AKICK responses
CHAN_AKICK_SYNTAX
	AKICK kamer {ADD | DEL | LIST | VIEW | COUNT | ENFORCE} [parameters]
CHAN_AKICK_LIST_SYNTAX
	AKICK kamer LIST [+begin] [masker]
CHAN_AKICK_VIEW_SYNTAX
	AKICK kamer VIEW [+begin] [masker]
CHAN_AKICK_DISABLED
	Sorry, aanpassing van de autokicklijst van kamers is tijdelijk uitgeschakeld.
CHAN_AKICK_ALREADY_EXISTS
	%s bestaat al op de autokicklijst van %s.
CHAN_AKICK_REACHED_LIMIT
	Sorry, er kunnen slechts %d maskers op de autokicklijst van een kamer staan.
CHAN_AKICK_ADDED
	%s toegevoegd aan de autokicklijst van %s.
CHAN_AKICK_NOT_FOUND
	%s niet gevonden op de autokicklijst van %s.
CHAN_AKICK_DELETED
	%s verwijderd van de autokicklijst van %s.
CHAN_AKICK_LIST_EMPTY
	Autokicklijst van %s is leeg.
CHAN_AKICK_NO_MATCH
	Geen overeenkomende invoeren gevonden op de autokicklijst van %s.
CHAN_AKICK_LIST_HEADER
	Autokicklijst voor %s:
# mask, set-by, set-time, last-used, reason
CHAN_AKICK_VIEW_FORMAT
	    %s (door %s op %s; laatst gebruikt %s)
	        %s
# mask, set-by, set-time, reason
CHAN_AKICK_VIEW_UNUSED_FORMAT
	    %s (door %s op %s; nooit gebruikt)
	        %s
CHAN_AKICK_COUNT
	%s heeft %d invoeren op de autokicklijst.
CHAN_AKICK_ENFORCE_DONE
	AKICK ENFORCE voor %s voltooid; betrof %d gebruikers.

# LEVELS responses
CHAN_LEVELS_SYNTAX
	LEVELS kamer {SET | DIS[ABLE] | LIST | RESET} [item [niveau]]
CHAN_LEVELS_READONLY
	Sorry, aanpassing van privilegeniveaus voor kamers is momenteel uitgeschakeld.
CHAN_LEVELS_RANGE
	Niveau dient tussen %d en %d te liggen.
CHAN_LEVELS_CHANGED
	Niveau voor %s in kamer %s gewijzigd naar %d.
CHAN_LEVELS_UNKNOWN
	Instelling %s onbekend.  Tik /msg %s HELP LEVELS DESC voor een lijst van geldige instellingen.
CHAN_LEVELS_DISABLED
	%s uitgeschakeld op kamer %s.
CHAN_LEVELS_LIST_HEADER
	Privilegeniveau-instellingen op kamer %s:
CHAN_LEVELS_LIST_DISABLED
	    %-*s  (uitgeschakeld)
CHAN_LEVELS_LIST_FOUNDER
	    %-*s  (alleen oprichter)
CHAN_LEVELS_LIST_NORMAL
	    %-*s  %4d
CHAN_LEVELS_RESET
	Privilegeniveau-instellingen voor kamer %s teruggezet naar de standaardinstelling.

# INFO responses
CHAN_INFO_SYNTAX
	INFO kamer [ALL]
CHAN_INFO_HEADER
	Informatie voor kamer %s:
CHAN_INFO_FOUNDER
	                  Oprichter: %s
CHAN_INFO_SUCCESSOR
	                   Opvolger: %s
CHAN_INFO_DESCRIPTION
	               Beschrijving: %s
CHAN_INFO_ENTRYMSG
	         Binnenkomstbericht: %s
CHAN_INFO_TIME_REGGED
	              Geregistreerd: %s
CHAN_INFO_LAST_USED
	            Laatst gebruikt: %s
CHAN_INFO_LAST_TOPIC
	          Laatste onderwerp: %s
CHAN_INFO_TOPIC_SET_BY
	   Onderwerp ingesteld door: %s
CHAN_INFO_URL
	                        URL: %s
CHAN_INFO_EMAIL
	                E-mailadres: %s
CHAN_INFO_OPTIONS
	                     Opties: %s
CHAN_INFO_OPT_PRIVATE
	Privé
CHAN_INFO_OPT_KEEPTOPIC
	Onderwerpbehoud
CHAN_INFO_OPT_TOPICLOCK
	Onderwerpslot
CHAN_INFO_OPT_SECUREOPS
	Operatorbeveiliging
CHAN_INFO_OPT_LEAVEOPS
	Operatorbehoud
CHAN_INFO_OPT_RESTRICTED
	Beperkte toegang
CHAN_INFO_OPT_SECURE
	Beveiliging
CHAN_INFO_OPT_OPNOTICE
	Operatoralarm
CHAN_INFO_OPT_ENFORCE
	Afdwinging
CHAN_INFO_OPT_NONE
	Geen
CHAN_INFO_MODE_LOCK
	                  Modusslot: %s
CHAN_INFO_NO_EXPIRE
	Deze kamer vervalt niet.
CHAN_INFO_SUSPEND_DETAILS
	Buiten werking gesteld door %s op %s (%s).
CHAN_INFO_SUSPEND_REASON
	Reden: %s
CHAN_INFO_SHOW_ALL
	Voor meer informatie, tik: /msg %s INFO %s ALL

# LIST responses
CHAN_LIST_SYNTAX
	LIST [+begin] patroon
CHAN_LIST_OPER_SYNTAX
	LIST [+begin] patroon [FORBIDDEN] [NOEXPIRE] [SUSPENDED]
CHAN_LIST_HEADER
	Lijst van invoeren overeenkomend met %s:
CHAN_LIST_NO_MATCH
	Geen overeenkomende kamers gevonden.

# INVITE responses
CHAN_INVITE_SYNTAX
	INVITE kamer
CHAN_INVITE_OK
	Bezig met uitnodigen van %s in kamer %s.

# OP/DEOP/VOICE/DEVOICE responses
CHAN_OPVOICE_SYNTAX
	%s kamer [bijnaam]
CHAN_OP_SUCCEEDED
	Operatorstatus gegeven aan%s in kamer %s.
CHAN_OP_ALREADY
	%s heeft al operatorstatus in kamer %s.
CHAN_OP_FAILED
	Kon geen operatorstatus geven aan %s in kamer %s.
CHAN_DEOP_SUCCEEDED
	Operatorstatus verwijderd van %s in kamer %s.
CHAN_DEOP_ALREADY
	%s heeft al geen operatorstatus meer in kamer %s.
CHAN_DEOP_FAILED
	Kon operatorstatus van %s in kamer %s niet verwijderen.
CHAN_VOICE_SUCCEEDED
	Voice-status gegeven aan %s in kamer %s.
CHAN_VOICE_ALREADY
	%s heeft al voice-status in kamer %s.
CHAN_VOICE_FAILED
	Kon geen voice-status geven aan %s in kamer %s.
CHAN_DEVOICE_SUCCEEDED
	Voice-status verwijderd van %s in kamer %s.
CHAN_DEVOICE_ALREADY
	%s heeft al geen voice-status meer in kamer %s.
CHAN_DEVOICE_FAILED
	Kon voice-status van %s in kamer %s niet verwijderen.
CHAN_HALFOP_SUCCEEDED
	Halfopstatus gegeven aan %s in kamer %s.
CHAN_HALFOP_ALREADY
	%s heeft al halfopstatus in kamer %s.
CHAN_HALFOP_FAILED
	Kon geen halfopstatus geven aan %s in kamer %s.
CHAN_DEHALFOP_SUCCEEDED
	Halfopstatus verwijderd van %s in kamer %s.
CHAN_DEHALFOP_ALREADY
	%s heeft al geen halfopstatus meer in kamer %s.
CHAN_DEHALFOP_FAILED
	Kon halfopstatus van %s in kamer %s niet verwijderen.
CHAN_PROTECT_SUCCEEDED
	Beschermingsstatus gegeven aan %s in kamer %s.
CHAN_PROTECT_ALREADY
	%s heeft al beschermingsstatus in kamer %s.
CHAN_PROTECT_FAILED
	Kon geen beschermingsstatus geven aan %s in kamer %s.
CHAN_DEPROTECT_SUCCEEDED
	Beschermingsstatus verwijderd van %s in kamer %s.
CHAN_DEPROTECT_ALREADY
	%s heeft al geen beschermingsstatus meer in kamer %s.
CHAN_DEPROTECT_FAILED
	Kon beschermingsstatus van %s in kamer %s niet verwijderen.

# UNBAN responses
CHAN_UNBAN_SYNTAX
	UNBAN kamer
CHAN_UNBANNED
	U bent niet langer verbannen van kamer %s.

# KICK responses
CHAN_KICK_SYNTAX
	KICK kamer bijnaam [reden]
CHAN_KICK_PROTECTED
	%s mag niet uit kamer %s worden verwijderd.
CHAN_KICKED
	%s uit kamer %s verwijderd.

# TOPIC responses
CHAN_TOPIC_SYNTAX
	TOPIC kamer onderwerp

# CLEAR responses
CHAN_CLEAR_SYNTAX
	CLEAR kamer wat
CHAN_CLEARED_BANS
	Alle verbanningen van kamer %s zijn verwijderd.
CHAN_CLEARED_EXCEPTIONS
	Alle uitzonderingen van kamer %s zijn verwijderd.
CHAN_CLEARED_INVITES
	Alle uitnodigingsmaskers van kamer %s zijn verwijderd.
CHAN_CLEARED_MODES
	Alle modi van kamer %s zijn teruggezet naar de standaardwaarde.
CHAN_CLEARED_OPS
	Modus +o is verwijderd van kamer %s.
CHAN_CLEARED_HALFOPS
	Modus +h is verwijderd van kamer %s.
CHAN_CLEARED_VOICES
	Modus +v is verwijderd van kamer %s.
CHAN_CLEARED_USERS
	Alle gebruikers zijn uit kamer %s verwijderd.

# GETPASS responses
CHAN_GETPASS_SYNTAX
	GETPASS kamer
CHAN_GETPASS_UNAVAILABLE
	Het commando GETPASS kan niet gebruikt worden voor kamer %s omdat encryptie in gebruik is.
CHAN_GETPASS_PASSWORD_IS
	Het wachtwoord voor kamer %s is %s.

# FORBID responses
CHAN_FORBID_SYNTAX
	FORBID kamer
CHAN_FORBID_SHORT_CHANNEL
	De kamer # mag niet worden verboden.
CHAN_FORBID_SUCCEEDED
	Kamer %s is nu verboden.
CHAN_FORBID_FAILED
	Kon kamer %s niet verbieden!

# SUSPEND responses
CHAN_SUSPEND_SYNTAX
	SUSPEND [+vervaltijd] kamer reden
CHAN_SUSPEND_SUCCEEDED
	Kamer %s is buiten werking gesteld.
CHAN_SUSPEND_ALREADY_SUSPENDED
	Kamer %s is al buiten werking.

# UNSUSPEND responses
CHAN_UNSUSPEND_SYNTAX
	UNSUSPEND kamer
CHAN_UNSUSPEND_SUCCEEDED
	Kamer %s is niet langer buiten werking.
CHAN_UNSUSPEND_NOT_SUSPENDED
	Kamer %s is niet buiten werking gesteld.

###########################################################################
#
# MemoServ messages
#
###########################################################################

# Automatic messages
MEMO_HAVE_NEW_MEMO
	U heeft 1 nieuwe memo.
MEMO_HAVE_NEW_MEMOS
	U heeft %d nieuwe memo's.
MEMO_TYPE_READ_LAST
	Tik /msg %s READ LAST om deze te lezen.
MEMO_TYPE_READ_NUM
	Tik /msg %s READ %d om deze te lezen.
MEMO_TYPE_LIST_NEW
	Tik /msg %s LIST NEW om ze te tonen.
MEMO_AT_LIMIT
	Waarschuwing: U heeft uw maximum aantal memo's (%d) bereikt.  U kunt geen nieuwe memo's ontvangen totdat u één of meer oude memo's verwijdert.
MEMO_OVER_LIMIT
	Waarschuwing: U heeft uw maximum aantal memo's (%d) overschreden.  U kunt geen nieuwe memo's ontvangen totdat u één of meer oude memo's verwijdert.
MEMO_NEW_MEMO_ARRIVED
	U heeft een nieuwe memo van %s.
	Tik /msg %s READ %d om deze te lezen.
MEMO_NEW_CHAN_MEMO_ARRIVED
	Heeft heeft een nieuwe memo van %s voor %s.
	Tik /msg %s READ %d om deze te lezen.

# Multi-use responses
MEMO_HAVE_NO_MEMOS
	U heeft geen memo's.
MEMO_HAVE_NO_NEW_MEMOS
	U heeft geen nieuwe memo's.
MEMO_DOES_NOT_EXIST
	Memo %d bestaat niet!
MEMO_LIST_NOT_FOUND
	Geen overeenkomende memo's gevonden.

# SEND responses
MEMO_SEND_SYNTAX
	SEND {bijnaam | kamer} memo-tekst
MEMO_SEND_DISABLED
	Sorry, versturen van memo's is tijdelijk uitgeschakeld.
MEMO_SEND_PLEASE_WAIT
	Wacht %s voordat u het commando SEND weer gebruikt.
MEMO_SEND_CHAN_NOT_AVAIL
	Sorry, kamermemo's zijn niet beschikbaar.
MEMO_X_GETS_NO_MEMOS
	%s kan geen memo's ontvangen.
MEMO_X_HAS_TOO_MANY_MEMOS
	%s heeft momenteel teveel memo's en kan er niet meer ontvangen.
MEMO_SENT
	Memo verzonden naar %s.
MEMO_SEND_FAILED
	Memo kon niet verzonden worden naar %s.

# LIST responses
MEMO_LIST_SYNTAX
	LIST [num | list | NEW]
MEMO_LIST_MEMOS
	Memo's voor %s.  Om te lezen, tik: /msg %s READ num
MEMO_LIST_NEW_MEMOS
	Nieuwe memo's voor %s.  Om te lezen, tik: /msg %s READ num
MEMO_LIST_HEADER
	   Num  Afzender          Datum/Tijd
MEMO_LIST_FORMAT
	%c%c%c%3d  %-16s  %s

# READ responses
MEMO_READ_SYNTAX
	READ {num | lijst | LAST | NEW}
MEMO_HEADER
	Memo %d van %s (%s).  Om te verwijderen, tik: /msg %s DEL %d
MEMO_CHAN_HEADER
	Memo %d van %s voor %s (%s).  Om te verwijderen, tik: /msg %s DEL %d

# SAVE responses
MEMO_SAVE_SYNTAX
	SAVE {num | lijst}
MEMO_SAVED_ONE
	Memo %d gemarkeerd om niet te vervallen.
MEMO_SAVED_SEVERAL
	%d memo's gemarkeerd om niet te vervallen.

# DEL responses
MEMO_DEL_SYNTAX
	DEL {num | lijst | ALL}
MEMO_DELETED_NONE
	Geen memo's verwijderd.
MEMO_DELETED_ONE
	Memo %d is verwijderd.
MEMO_DELETED_SEVERAL
	%d memo's zijn verwijderd.
MEMO_DELETED_ALL
	Al uw memo's zijn verwijderd.

# RENUMBER responses
MEMO_RENUMBER_ONLY_YOU
	U kunt het commando RENUMBER commando alleen op uzelf gebruiken.
MEMO_RENUMBER_DONE
	Uw memo's zijn hernummerd.

# SET responses
MEMO_SET_SYNTAX
	SET optie parameters
MEMO_SET_DISABLED
	Sorry, instellen van memo-opties is tijdelijk uitgeschakeld.
MEMO_SET_UNKNOWN_OPTION
	Onbekende optie voor SET: %s.

# SET NOTIFY responses
MEMO_SET_NOTIFY_SYNTAX
	SET NOTIFY {ON | LOGON | NEW | OFF}
MEMO_SET_NOTIFY_ON
	%s zal u vanaf nu op de hoogte brengen van memo's wanneer u verbinding maakt en wanneer ze naar u verzonden worden.
MEMO_SET_NOTIFY_LOGON
	%s zal u vanaf nu op de hoogte brengen van memo's wanneer u verbinding maakt of wanneer u /AWAY uitzet.
MEMO_SET_NOTIFY_NEW
	%s zal u vanaf nu op de hoogte brengen van memo's wanneer ze naar u verzonden worden.
MEMO_SET_NOTIFY_OFF
	%s zal u vanaf nu niet meer op de hoogte brengen van memo's.

# SET LIMIT responses
MEMO_SET_LIMIT_SYNTAX
	SET LIMIT limiet
MEMO_SET_LIMIT_OPER_SYNTAX
	SET LIMIT [bijnaam] {limiet | NONE | DEFAULT} [HARD]
MEMO_SET_YOUR_LIMIT_FORBIDDEN
	U heeft geen permissie om uw memolijst aan te passen.
MEMO_SET_LIMIT_FORBIDDEN
	De memolimiet voor %s mag niet worden gewijzigd.
MEMO_SET_YOUR_LIMIT_TOO_HIGH
	U kunt uw memolimiet niet hoger dan %d zetten.
MEMO_SET_LIMIT_TOO_HIGH
	U kunt de memolimiet van %s niet hoger dan %d zetten.
MEMO_SET_LIMIT_OVERFLOW
	Memolimiet te hoog; limiet in plaats daarvan ingesteld op %d.
MEMO_SET_YOUR_LIMIT
	Uw memolimiet is ingesteld op %d.
MEMO_SET_YOUR_LIMIT_ZERO
	U kunt niet langer memo's ontvangen.
MEMO_SET_YOUR_LIMIT_DEFAULT
	Uw memolimiet is ingesteld op DEFAULT (%d).
MEMO_UNSET_YOUR_LIMIT
	Uw memolimiet is uitgeschakeld.
MEMO_SET_LIMIT
	Memolimiet voor %s is ingesteld op %d.
MEMO_SET_LIMIT_ZERO
	Memolimiet voor %s is ingesteld op 0.
MEMO_SET_LIMIT_DEFAULT
	Memolimiet voor %s is ingesteld op DEFAULT (%d).
MEMO_UNSET_LIMIT
	Memolimiet voor %s is uitgeschakeld.

# INFO responses
MEMO_INFO_NO_MEMOS
	U heeft momenteel geen memo's.
MEMO_INFO_MEMO
	U heeft momenteel 1 memo.
MEMO_INFO_MEMO_UNREAD
	U heeft momenteel 1 memo, en deze is nog ongelezen.
MEMO_INFO_MEMOS
	U heeft momenteel %d memo's.
MEMO_INFO_MEMOS_ONE_UNREAD
	U heeft momenteel %d memo's, waarvan 1 ongelezen.
MEMO_INFO_MEMOS_SOME_UNREAD
	U heeft momenteel %d memo's, waarvan %d ongelezen.
MEMO_INFO_MEMOS_ALL_UNREAD
	U heeft momenteel %d memo's; die allen ongelezen zijn.
MEMO_INFO_LIMIT
	Uw memolimiet is %d.
MEMO_INFO_HARD_LIMIT
	Uw memolimiet is %d en mag niet worden gewijzigd.
MEMO_INFO_LIMIT_ZERO
	Uw memolimiet is 0; u zal geen nieuwe memo's ontvangen.
MEMO_INFO_HARD_LIMIT_ZERO
	Uw memolimiet is 0; u zal geen nieuwe memo's ontvangen.  U kunt deze limiet niet wijzigen.
MEMO_INFO_NO_LIMIT
	U heeft geen limiet op het aantal memo's dat u mag bewaren.
MEMO_INFO_NOTIFY_OFF
	U wordt niet van nieuwe memo's op de hoogte gebracht.
MEMO_INFO_NOTIFY_ON
	U wordt op de hoogte gebracht van nieuwe memo's wanneer u verbinding maakt en wanneer ze aankomen.
MEMO_INFO_NOTIFY_RECEIVE
	U wordt op de hoogte gebracht wanneer nieuwe memo's aankomen.
MEMO_INFO_NOTIFY_SIGNON
	U wordt op de hoogte gebracht van nieuwe memo's wanneer u verbinding maakt met IRC.
MEMO_INFO_X_NO_MEMOS
	%s heeft momenteel geen memo's.
MEMO_INFO_X_MEMO
	%s heeft momenteel 1 memo.
MEMO_INFO_X_MEMO_UNREAD
	%s heeft momenteel 1 memo en deze is nog ongelezen.
MEMO_INFO_X_MEMOS
	%s heeft momenteel %d memo's.
MEMO_INFO_X_MEMOS_ONE_UNREAD
	%s heeft momenteel %d memo's, waarvan 1 ongelezen.
MEMO_INFO_X_MEMOS_SOME_UNREAD
	%s heeft momenteel %d memo's, waarvan %d ongelezen.
MEMO_INFO_X_MEMOS_ALL_UNREAD
	%s heeft momenteel %d memo's; die allen ongelezen zijn.
MEMO_INFO_X_LIMIT
	De memolimiet van %s is %d.
MEMO_INFO_X_HARD_LIMIT
	De memolimiet van %s is %d en mag niet worden gewijzigd.
MEMO_INFO_X_NO_LIMIT
	%s heeft geen memolimiet.
MEMO_INFO_X_NOTIFY_OFF
	%s wordt niet op de hoogte gebracht van nieuwe memo's.
MEMO_INFO_X_NOTIFY_ON
	%s wordt op de hoogte gebracht van nieuwe memo's wanneer hij/zij verbinding maakt en wanneer ze aankomen.
MEMO_INFO_X_NOTIFY_RECEIVE
	%s wordt op de hoogte gebracht wanneer memo's aankomen.
MEMO_INFO_X_NOTIFY_SIGNON
	%s wordt op de hoogte gebracht van nieuwe memo's wanneer hij/zij verbinding maakt.

################ forward module messages/responses

# Mail format
MEMO_FORWARD_MAIL_SUBJECT
	Memo voor %s
MEMO_FORWARD_MULTIPLE_MAIL_SUBJECT
	Memo's voor %s
MEMO_FORWARD_MAIL_BODY
	Memo van %s (%s)
	%s
MEMO_FORWARD_CHANMEMO_MAIL_BODY
	Memo van %s voor %s (%s)
	%s

# FORWARD responses
MEMO_FORWARD_SYNTAX
	FORWARD {num | lijst | ALL}
MEMO_FORWARD_NEED_EMAIL
	U dient een e-mailadres in te stellen voordat memo's naar u kunnen worden doorgestuurd.
MEMO_FORWARD_PLEASE_WAIT
	Wacht %s voor u het commando FORWARD weer gebruikt.
MEMO_FORWARD_FAILED
	Sorry, doorsturen mislukt.
MEMO_FORWARDED_NONE
	Er zijn geen memo's doorgestuurd.
MEMO_FORWARDED_ONE
	Memo %d is doorgestuurd.
MEMO_FORWARDED_SEVERAL
	Er zijn %d memo's doorgestuurd.
MEMO_FORWARDED_ALL
	Al uw memo's zijn doorgestuurd.

# SET FORWARD responses
MEMO_SET_FORWARD_SYNTAX
	SET FORWARD {ON | COPY | OFF}
MEMO_SET_FORWARD_ON
	Uw memo's worden nu doorgestuurd naar uw e-mailadres: %s
MEMO_SET_FORWARD_COPY
	Uw memo's worden nu doorgestuurd naar uw e-mailadres: %s
	Uw memo's worden tevens opgeslagen door Services.
MEMO_SET_FORWARD_OFF
	Uw memo's worden niet doorgestuurd naar uw e-mailadres.

################ ignore module messages/responses

# IGNORE responses
MEMO_IGNORE_SYNTAX
	IGNORE {ADD | DEL | LIST} [bijnaam | masker]
MEMO_IGNORE_ADD_SYNTAX
	IGNORE ADD bijnaam | masker
MEMO_IGNORE_DEL_SYNTAX
	IGNORE DEL bijnaam | masker
MEMO_IGNORE_LIST_FULL
	Uw negeerlijst is vol.
MEMO_IGNORE_ALREADY_PRESENT
	%s staat al op uw negeerlijst.
MEMO_IGNORE_ADDED
	%s toegevoegd aan uw negeerlijst.
MEMO_IGNORE_NOT_FOUND
	%s niet gevonden op uw negeerlijst.
MEMO_IGNORE_DELETED
	%s verwijderd van uw negeerlijst.
MEMO_IGNORE_LIST_EMPTY
	Uw negeerlijst is leeg.
MEMO_IGNORE_LIST
	Negeerlijst:
MEMO_IGNORE_LIST_X_EMPTY
	Negeerlijst voor %s is leeg.
MEMO_IGNORE_LIST_X
	Negeerlijst voor %s:

###########################################################################
#
# OperServ messages
#
###########################################################################

# General messages
OPER_BOUNCY_MODES
	Services kan geen modi wijzigen.  Zijn uw servers correct geconfigureerd?
OPER_BOUNCY_MODES_U_LINE
	Services kan geen modi wijzigen.  Zijn de U:Lines van uw servers correct geconfigureerd?

# GLOBAL responses
OPER_GLOBAL_SYNTAX
	GLOBAL bericht

# STATS responses
OPER_STATS_UNKNOWN_OPTION
	Onbekende optie voor STATS: %s.
OPER_STATS_RESET_USER_COUNT
	De telling van het maximum aantal gebruikers is teruggezet naar de standaardwaarde.

OPER_STATS_CURRENT_USERS
	Huidige gebruikers: %d (%d ops)
OPER_STATS_MAX_USERS
	Maximum aantal gebruikers: %d (%s)
# Parameters: days, STR_DAYS or STR_DAY, hours, minutes
OPER_STATS_UPTIME_DHM
	Services aanwezig %d%s, %02d:%02d
# If >= 1 hour, hours and minutes; if < 1 hour, minutes and seconds
OPER_STATS_UPTIME_HM_MS
	Services aanwezig %s

OPER_STATS_KBYTES_READ
	Ontvangen data:    %10u kB
OPER_STATS_KBYTES_WRITTEN
	Verstuurde data:        %10u kB
OPER_STATS_NETBUF_SOCK
	Server socket buffers: %5u kB
OPER_STATS_NETBUF_SOCK_PERCENT
	Server socket buffers: %5u kB (%d%%)
OPER_STATS_NETBUF_TOTAL
	Totaal van de socket buffers:  %5u kB
OPER_STATS_NETBUF_TOTAL_PERCENT
	Totaal van de socket buffers:  %5u kB (%d%%)

OPER_STATS_ALL_USER_MEM
	Gebruikers    : %6d invoeren, %5d kB
OPER_STATS_ALL_CHANNEL_MEM
	Kamers        : %6d invoeren, %5d kB
OPER_STATS_ALL_SERVER_MEM
	Servers       : %6d invoeren, %5d kB
OPER_STATS_ALL_NICKGROUPINFO_MEM
	Bijnaamgroepen: %6d invoeren, %5d kB
OPER_STATS_ALL_NICKINFO_MEM
	Bijnamen      : %6d invoeren, %5d kB
OPER_STATS_ALL_CHANSERV_MEM
	ChanServ      : %6d invoeren, %5d kB
OPER_STATS_ALL_STATSERV_MEM
	StatServ      : %6d invoeren, %5d kB
OPER_STATS_ALL_NEWS_MEM
	Nieuws        : %6d invoeren, %5d kB
OPER_STATS_ALL_AKILL_MEM
	Autokills     : %6d invoeren, %5d kB
OPER_STATS_ALL_EXCEPTION_MEM
	Uitzonderingen: %6d invoeren, %5d kB
OPER_STATS_ALL_SGLINE_MEM
	SGlines       : %6d invoeren, %5d kB
OPER_STATS_ALL_SQLINE_MEM
	SQlines       : %6d invoeren, %5d kB
OPER_STATS_ALL_SZLINE_MEM
	SZlines       : %6d invoeren, %5d kB
OPER_STATS_ALL_SESSION_MEM
	Sessies       : %6d invoeren, %5d kB

# GETKEY responses
OPER_GETKEY_SYNTAX
	GETKEY kamer
OPER_GETKEY_KEY_IS
	De sleutel voor kamer %s is: %s
OPER_GETKEY_NO_KEY
	Er is geen sleutel ingesteld voor kamer %s.

# MODE responses
OPER_MODE_SYNTAX
	MODE kamer modi

# CLEARMODES responses
OPER_CLEARMODES_SYNTAX
	CLEARMODES kamer [ALL]
OPER_CLEARMODES_DONE
	Alle kamermodi zijn verwijderd van kamer %s.
OPER_CLEARMODES_ALL_DONE
	Alle kamer- en gebruikersmodi zijn verwijderd van kamer %s.

# CLEARCHAN responses
OPER_CLEARCHAN_SYNTAX
	CLEARCHAN kamer
OPER_CLEARCHAN_DONE
	Alle gebruikers zijn verwijderd van kamer %s.

# KICK responses
OPER_KICK_SYNTAX
	KICK kamer gebruiker reden

# ADMIN responses
OPER_ADMIN_SYNTAX
	ADMIN {ADD | DEL | LIST} [bijnaam]
OPER_ADMIN_ADD_SYNTAX
	ADMIN ADD bijnaam
OPER_ADMIN_DEL_SYNTAX
	ADMIN DEL bijnaam
OPER_ADMIN_NO_NICKSERV
	NickServ is momenteel uitgeschakeld; het commando ADMIN is niet beschikbaar.
OPER_ADMIN_EXISTS
	%s bestaat al op de Services adminlijst.
OPER_ADMIN_ADDED
	%s toegevoegd aan de Services adminlijst.
OPER_ADMIN_TOO_MANY
	Teveel invoeren (%d) op de Services adminlijst; kan er niet meer toevoegen.
OPER_ADMIN_REMOVED
	%s verwijderd van de Services adminlijst.
OPER_ADMIN_NOT_FOUND
	%s niet gevonden op de Services adminlijst.
OPER_ADMIN_LIST_HEADER
	Services adminlijst:

# OPER responses
OPER_OPER_SYNTAX
	OPER {ADD | DEL | LIST} [bijnaam]
OPER_OPER_ADD_SYNTAX
	OPER ADD bijnaam
OPER_OPER_DEL_SYNTAX
	OPER DEL bijnaam
OPER_OPER_NO_NICKSERV
	NickServ is momenteel uitgeschakeld; het commando OPER is niet beschikbaar.
OPER_OPER_EXISTS
	%s bestaat al op de Services operatorlijst.
OPER_OPER_ADDED
	%s toegevoegd aan de Services operatorlijst.
OPER_OPER_TOO_MANY
	Teveel invoeren (%d) op de Services operatorlijst; kan er niet meer toevoegen.
OPER_OPER_REMOVED
	%s verwijderd van de Services operatorlijst.
OPER_OPER_NOT_FOUND
	%s niet gevonden op de Services operatorlijst.
OPER_OPER_LIST_HEADER
	Services operatorlijst:

# AKILL/EXCLUDE/SxLINE common responses
OPER_MASKDATA_SYNTAX
	%s {ADD | DEL | CLEAR | LIST | VIEW | CHECK | COUNT} [parameters]
OPER_MASKDATA_ADD_SYNTAX
	%s ADD [+vervaltijd] masker reden
OPER_MASKDATA_DEL_SYNTAX
	%s DEL masker
OPER_MASKDATA_CLEAR_SYNTAX
	%s CLEAR masker
OPER_MASKDATA_LIST_SYNTAX
	%s {LIST | VIEW} [+begin] [patroon] [NOEXPIRE]
OPER_MASKDATA_CHECK_SYNTAX
	%s CHECK masker
# mask, reason
OPER_MASKDATA_LIST_FORMAT
	%-32s  %s
# mask, set-by, set-time, last-used, expires, reason
OPER_MASKDATA_VIEW_FORMAT
	%s (door %s op %s, laatst gebruikt %s; %s)
	    %s
# mask, set-by, set-time, expires, reason
OPER_MASKDATA_VIEW_UNUSED_FORMAT
	%s (door %s op %s, nooit gebruikt; %s)
	    %s

# AKILL responses
OPER_TOO_MANY_AKILLS
	Autokill-lijst is vol!
OPER_AKILL_EXISTS
	%s staat reeds op de autokill-lijst.
OPER_AKILL_NO_NICK
	Opmerking: Autokillmaskers kunnen geen bijnamen bevatten; zorg dat er geen bijnaamcomponent in uw masker staat.
OPER_AKILL_MASK_TOO_GENERAL
	Het autokillmasker is te generiek.
OPER_AKILL_EXPIRY_LIMITED
	De vervaltijd mag maximaal %s zijn.
OPER_AKILL_ADDED
	%s toegevoegd aan de autokill-lijst.
OPER_AKILL_REMOVED
	%s verwijderd van de autokill-lijst.
OPER_AKILL_CLEARED
	De autokill-lijst is leeggemaakt.
OPER_AKILL_NOT_FOUND
	%s niet gevonden op de autokill-lijst.
OPER_AKILL_LIST_HEADER
	Huidige autokill-lijst:
OPER_AKILL_LIST_EMPTY
	De autokill-lijst is leeg.
OPER_AKILL_LIST_NO_MATCH
	Geen overeenkomende maskers gevonden op de autokill-lijst.
OPER_AKILL_CHECK_NO_MATCH
	Geen maskers overeenkomend met %s gevonden op de autokill-lijst.
OPER_AKILL_CHECK_HEADER
	Maskers overeenkomend met %s op de autokill-lijst:
OPER_AKILL_CHECK_TRAILER
	%d overeenkomende maskers gevonden.
OPER_AKILL_COUNT
	Er staan %d machinenaam-maskers op de autokill-lijst.

# AKILLCHAN responses
OPER_AKILLCHAN_SYNTAX
	AKILLCHAN [KILL] [+vervaltijd] kamer reden
OPER_AKILLCHAN_AKILLED
	Autokill uitgoeverd op %d gebruikers.
OPER_AKILLCHAN_KILLED
	%d gebruikers verwijderd van het netwerk.
OPER_AKILLCHAN_AKILLED_ONE
	Autokill uitgevoerd op 1 gebruiker.
OPER_AKILLCHAN_KILLED_ONE
	1 gebruiker verwijderd van het netwerk.

# EXCLUDE responses
OPER_TOO_MANY_EXCLUDES
	Autokill-uitzonderingslijst
OPER_EXCLUDE_EXISTS
	%s bestaat al op de autokill-uitzonderingslijst.
OPER_EXCLUDE_ADDED
	%s toegevoegd aan de autokill-uitzonderingslijst.
OPER_EXCLUDE_REMOVED
	%s verwijderd van de autokill-uitzonderingslijst.
OPER_EXCLUDE_CLEARED
	De autokill-uitzonderingslijst is leeggemaakt.
OPER_EXCLUDE_NOT_FOUND
	%s niet gevonden op de autokill-uitzonderingslijst.
OPER_EXCLUDE_LIST_HEADER
	Huidige autokill-uitzonderingslijst:
OPER_EXCLUDE_LIST_EMPTY
	De autokill-uitzonderingslijst is leeg.
OPER_EXCLUDE_LIST_NO_MATCH
	Geen overeenkomende maskers gevonden op de autokill-uitzonderingslijst.
OPER_EXCLUDE_CHECK_NO_MATCH
	Geen maskers overeenkomend met %s gevonden op de autokill-uitzonderingslijst.
OPER_EXCLUDE_CHECK_HEADER
	Maskers overeenkomend met %s op de autokill-uitzonderingslijst:
OPER_EXCLUDE_CHECK_TRAILER
	%d overeenkomende maskers gevonden.
OPER_EXCLUDE_COUNT
	Er staan %d machinenaam-maskers op de autokill-uitzonderingslijst.

# SxLINE responses
OPER_TOO_MANY_SLINES
	%s-lijst is vol!
OPER_SLINE_EXISTS
	%s bestaat al op de %s-lijst.
OPER_SLINE_MASK_TOO_GENERAL
	Het %s-masker is te generiek.
OPER_SLINE_ADDED
	%s toegevoegd aan de %s-lijst.
OPER_SLINE_REMOVED
	%s verwijderd van de %s-lijst.
OPER_SLINE_CLEARED
	De %s-lijst is leeggemaakt.
OPER_SLINE_NOT_FOUND
	%s niet gevonden op de %s-lijst.
OPER_SLINE_LIST_HEADER
	Huidige %s-lijst:
OPER_SLINE_LIST_EMPTY
	De %s-lijst is leeg.
OPER_SLINE_LIST_NO_MATCH
	Geen overeenkomende maskers gevonden op de %s-lijst.
OPER_SLINE_CHECK_NO_MATCH
	Geen maskers overeenkomend met %s gevonden op de %s-lijst.
OPER_SLINE_CHECK_HEADER
	Maskers overeenkomend met %s op de %s-lijst:
OPER_SLINE_CHECK_TRAILER
	%d overeenkomende maskers gevonden.
OPER_SLINE_COUNT
	Er staan %d invoeren op de %s-lijst.
OPER_SZLINE_NOT_AVAIL
	SZLINE is niet beschikbaar op deze IRC-server.

# SU responses
OPER_SU_SYNTAX
	SU wachtwoord
OPER_SU_NO_PASSWORD
	Het commando SU is niet beschikbaar omdat er geen supergebruiker-wachtwoord is ingesteld.
OPER_SU_SUCCEEDED
	U heeft nu Services supergebruiker-privileges.
OPER_SU_FAILED
	Kon het wachtwoord niet verifiëren.

# SET responses
OPER_SET_SYNTAX
	SET optie instelling
OPER_SET_IGNORE_ON
	Negeercode wordt wel gebruikt.
OPER_SET_IGNORE_OFF
	Negeercode wordt niet gebruikt.
OPER_SET_IGNORE_ERROR
	Instelling voor IGNORE dient ON (aan) of OFF (uit) te zijn.
OPER_SET_READONLY_ON
	Services staat nu in alleen-lezen modus.
OPER_SET_READONLY_OFF
	Services staat nu in lezen-schrijven modus.
OPER_SET_READONLY_ERROR
	Instelling voor READONLY dient ON (aan) of OFF (uit) te zijn.
OPER_SET_DEBUG_ON
	Services staat nu in foutopsporingsmodus.
OPER_SET_DEBUG_OFF
	Services staat niet langer in foutopsporingsmodus.
OPER_SET_DEBUG_LEVEL
	Services staat nu in foutopsporingsmodus (niveau %d).
OPER_SET_DEBUG_ERROR
	Instelling voor DEBUG dient ON (aan), OFF (uit), of een positief nummer te zijn.
OPER_SET_SUPASS_FAILED
	Kon het supergebruiker-wachtwoord niet instellen.
OPER_SET_SUPASS_OK
	Het supergebruiker-wachtwoord van Services is gewijzigd.
OPER_SET_SUPASS_NONE
	Het supergebruiker-wachtwoord van Services is uitgeschakeld.
OPER_SET_UNKNOWN_OPTION
	Onbekende optie %s.

# JUPE responses
OPER_JUPE_SYNTAX
	JUPE servernaam [reden]
OPER_JUPE_INVALID_NAME
	Ongeldige servernaam (servernamen dienen een punt te bevatten).
OPER_JUPE_ALREADY_JUPED
	Het commando JUPE is al uitgevoerd voor server %s.

# RAW responses
OPER_RAW_SYNTAX
	RAW tekst

# UPDATE responses
OPER_UPDATE_SYNTAX
	UPDATE [FORCE]
OPER_UPDATE_FORCE_FAILED
	Kon het slot niet verwijderen van de gegevensbestanden.
OPER_UPDATING
	Bezig met updaten van de gegevensbestanden...
OPER_UPDATE_COMPLETE
	Update van de gegevensbestanden voltooid.
OPER_UPDATE_FAILED
	Update van de gegevensbestanden mislukt.

# REHASH responses
OPER_REHASHING
	Bezig met opnieuw inlezen van de configuratie-instellingen.
OPER_REHASHED
	Update van de configuratie-instellingen geslaagd.
OPER_REHASH_ERROR
	Er is een fout opgetreden bij het opnieuw inlezen van de configuratie-instellingen.  Bekijk het logbestand van Services voor details.

# KILLCLONES responses
OPER_KILLCLONES_SYNTAX
	KILLCLONES bijnaam
OPER_KILLCLONES_UNKNOWN_NICK
	Kon gebruiker %s niet vinden.
OPER_KILLCLONES_KILLED
	%s heeft het commando KILLCLONES gebruikt voor %s en verwijderde %d dubbele gebruikers.
OPER_KILLCLONES_KILLED_AKILL
	%s heeft het commando KILLCLONES gebruikt voor %s en verwijderde %d dubbele gebruikers.  Een tijdelijke AKILL is toegevoegd voor %s.


###########################################################################
#
# Session limiting and exception messages
#
###########################################################################

# EXCEPTION responses
OPER_EXCEPTION_SYNTAX
	EXCEPTION {ADD | DEL | CLEAR | MOVE | LIST | VIEW | CHECK | COUNT} [parameters]
OPER_EXCEPTION_ADD_SYNTAX
	EXCEPTION ADD [+vervaltijd] masker limiet reden
OPER_EXCEPTION_DEL_SYNTAX
	EXCEPTION DEL {masker | lijst}
OPER_EXCEPTION_CLEAR_SYNTAX
	EXCEPTION CLEAR ALL
OPER_EXCEPTION_MOVE_SYNTAX
	EXCEPTION MOVE num nieuwnum
OPER_EXCEPTION_LIST_SYNTAX
	EXCEPTION {LIST | VIEW} [[+begin] patroon | lijst]
OPER_EXCEPTION_ALREADY_PRESENT
	Masker %s staat al op de uitzonderingslijst.
OPER_EXCEPTION_TOO_MANY
	Sessielimiet-uitzonderingslijst is vol!
OPER_EXCEPTION_ADDED
	Sessielimiet voor %s ingesteld op %d.
OPER_EXCEPTION_MOVED
	Uitzondering voor %s (#%d) verplaatst naar nummer %d.
OPER_EXCEPTION_NO_SUCH_ENTRY
	Invoer (#%d) bestaat niet op de sessielimiet-uitzonderingslijst.
OPER_EXCEPTION_NOT_FOUND
	%s niet gevonden op de sessielimiet-uitzonderingslijst.
OPER_EXCEPTION_NO_MATCH
	Geen overeenkomende invoeren op de sessielimiet-uitzonderingslijst.
OPER_EXCEPTION_EMPTY
	Sessielimiet-uitzonderingslijst is leeg.
OPER_EXCEPTION_DELETED
	%s verwijderd van de sessielimiet-uitzonderingslijst.
OPER_EXCEPTION_DELETED_ONE
	1 invoer verwijderd van de sessielimiet-uitzonderingslijst.
OPER_EXCEPTION_DELETED_SEVERAL
	%d invoeren verwijderd van de sessielimiet-uitzonderingslijst.
OPER_EXCEPTION_CLEARED
	Sessielimiet-uitzonderingslijst is leeggemaakt.
OPER_EXCEPTION_LIST_HEADER
	Huidige sessielimiet-uitzonderingslijst:
# number, limit, host
OPER_EXCEPTION_LIST_COLHEAD
	Num  Limiet  Machinenaam
OPER_EXCEPTION_LIST_FORMAT
	%3d  %4d   %s
# number, mask, set-by, set-time, expires, limit, reason
OPER_EXCEPTION_VIEW_FORMAT
	%3d.  %s  (door %s op %s; %s)
	    Limiet: %-4d  - %s
OPER_EXCEPTION_CHECK_SYNTAX
	EXCEPTION CHECK masker
OPER_EXCEPTION_CHECK_NO_MATCH
	Geen maskers overeenkomend met %s gevonden op de sessielimiet-uitzonderingslijst.
OPER_EXCEPTION_CHECK_HEADER
	Maskers overeenkomend met %s op de sessielimiet-uitzonderingslijst:
OPER_EXCEPTION_CHECK_TRAILER
	%d overeenkomende maskers gevonden.
OPER_EXCEPTION_COUNT
	Er staan %d invoeren op de sessielimiet-uitzonderingslijst.
OPER_EXCEPTION_INVALID_LIMIT
	Ongeldige sessielimiet; limiet dient minimaal nul en niet groter dan %d te zijn.
OPER_EXCEPTION_INVALID_HOSTMASK
	Ongeldig machinenaam-masker.  Alleen bestaande machinenaam-maskers zijn geldig als uitzonderingen.

# SESSION responses
OPER_SESSION_SYNTAX
	SESSION {LIST limiet | VIEW machine}
OPER_SESSION_LIST_SYNTAX
	SESSION LIST limiet
OPER_SESSION_VIEW_SYNTAX
	SESSION VIEW machine
OPER_SESSION_INVALID_THRESHOLD
	Ongeldige drempelwaarde; drempel dient een getal groter dan 1 te zijn.
OPER_SESSION_NOT_FOUND
	%s niet gevonden op de sessielijst.
OPER_SESSION_LIST_HEADER
	Machinenamen met tenminste %d sessies:
OPER_SESSION_LIST_COLHEAD
	Sessies  Machine
# limit, host
OPER_SESSION_LIST_FORMAT
	%6d    %s
# host, sessions, limit
OPER_SESSION_VIEW_FORMAT
	De machine %s heeft momenteel %d sessies met een limiet van %d.


###########################################################################
#
# News system messages
#
###########################################################################

NEWS_LOGON_TEXT
	[Binnenkomstnieuws - %s] %s
NEWS_OPER_TEXT
	[Opernieuws - %s] %s

NEWS_LOGON_SYNTAX
	LOGONNEWS {ADD | DEL | LIST} [tekst | num]
NEWS_LOGON_LIST_HEADER
	Binnenkomstnieuws items:
NEWS_LOGON_LIST_ENTRY
	%5d (%s door %s)
	    %s
NEWS_LOGON_LIST_NONE
	Er is geen binnenkomstnieuws.
NEWS_LOGON_ADD_SYNTAX
	LOGONNEWS ADD tekst
NEWS_LOGON_ADD_FULL
	Nieuwslijst is vol!
NEWS_LOGON_ADDED
	Nieuw binnenkomstnieuws-item toegevoegd (#%d).
NEWS_LOGON_DEL_SYNTAX
	LOGONNEWS DEL {num | ALL}
NEWS_LOGON_DEL_NOT_FOUND
	Binnenkomstnieuws-item #%d niet gevonden!
NEWS_LOGON_DELETED
	Binnenkomstnieuws-item #%d verwijderd.
NEWS_LOGON_DEL_NONE
	Geen binnenkomstnieuws-items om te verwijderen!
NEWS_LOGON_DELETED_ALL
	Alle binnenkomstnieuws-items zijn verwijderd.

NEWS_OPER_SYNTAX
	OPERNEWS {ADD | DEL | LIST} [tekst | num]
NEWS_OPER_LIST_HEADER
	Opernieuws items:
NEWS_OPER_LIST_ENTRY
	%5d (%s door %s)
	    %s
NEWS_OPER_LIST_NONE
	Er is geen opernieuws.
NEWS_OPER_ADD_SYNTAX
	OPERNEWS ADD tekst
NEWS_OPER_ADD_FULL
	Nieuwslijst is vol!
NEWS_OPER_ADDED
	Nieuw opernieuws-item toegevoegd (#%d).
NEWS_OPER_DEL_SYNTAX
	OPERNEWS DEL {num | ALL}
NEWS_OPER_DEL_NOT_FOUND
	Opernieuws-item #%d niet gevonden!
NEWS_OPER_DELETED
	Opernieuws-tem #%d verwijderd.
NEWS_OPER_DEL_NONE
	Geen opernieuws-items om te verwijderen!
NEWS_OPER_DELETED_ALL
	Alle opernieuws-items zijn verwijderd.

NEWS_HELP_LOGON
	Syntax: LOGONNEWS ADD tekst
	        LOGONNEWS DEL {num | ALL}
	        LOGONNEWS LIST
	
	Wijzigt of toont de lijst met binnenkomstnieuws-berichten.
	Wanneer een gebruiker verbinding maakt met het netwerk
	worden deze berichten naar hem/haar verstuurd.  (Er worden
	echter niet meer dan drie berichten verstuurd om te
	voorkomen dat de gebruiker wordt overspoeld.  Wanneer er
	meer dan drie berichten zijn worden alleen de drie
	meest recente berichten verstuurd.)
	
	Het commando LOGONNEWS LIST mag door elke IRC operator
	gebruikt worden om een lijst van huidige nieuwsberichten
	te tonen.  De commando's ADD en DEL mogen alleen door
	Services-operators worden gebruikt.

NEWS_HELP_OPER
	Syntax: OPERNEWS ADD tekst
	        OPERNEWS DEL {num | ALL}
	        OPERNEWS LIST
	
	Wijzigt of toont de lijst met opernieuws-berichten.
	Wanneer een gebruiker operatorstatus verkrijgt (met het
	commando /OPER), worden deze berichten naar hem/haar
	verstuurd.  (Er worden echter niet meer dan drie
	berichten verstuurd om te voorkomen dat de gebruiker
	wordt overspoeld.  Wanneer er meer dan drie berichten zijn
	worden alleen de drie meest recente berichten verstuurd.)
	
	Het commando LOGONNEWS LIST mag door elke IRC operator
	gebruikt worden om een lijst van huidige opernieuws-
	berichten te tonen.  De commando's ADD en DEL mogen alleen
	door Services-operators worden gebruikt.


###########################################################################
#
# StatServ messages
#
############################################################################

# General SERVERS responses
STAT_SERVERS_REMOVE_SERV_FIRST
	Server %s is aanwezig.  Verwijder deze van het netwerk en probeer het opnieuw.
STAT_SERVERS_SERVER_EXISTS
	Server %s bestaat al.

# SERVERS command syntax
STAT_SERVERS_SYNTAX
	SERVERS optie parameters

# STATS responses
STAT_SERVERS_STATS_TOTAL
	Totaal aantal Servers: %2d
STAT_SERVERS_STATS_ON_OFFLINE
	       Aanwezig: %2d (%d%%)  Niet aanwezig: %2d (%d%%)
STAT_SERVERS_LASTQUIT_WAS
	    Laatste vertrek: %-30s  %s

# LIST responses
STAT_SERVERS_LIST_HEADER
	Servernaam                     Gebruikers Opers
STAT_SERVERS_LIST_FORMAT
	%-30s %3d (%2d%%)  %3d (%2d%%)
STAT_SERVERS_LIST_RESULTS
	Einde van de lijst -- %d/%d aanwezige servers getoond.

# VIEW responses
STAT_SERVERS_VIEW_HEADER_ONLINE
	%-30s  (aanwezig)
STAT_SERVERS_VIEW_HEADER_OFFLINE
	%-30s  (niet aanwezig)
STAT_SERVERS_VIEW_LASTJOIN
	    Laatste aankoppeling: %s
STAT_SERVERS_VIEW_LASTQUIT
	         Laatste vertrek: %s
STAT_SERVERS_VIEW_QUITMSG
	          Vertrekbericht: %s
STAT_SERVERS_VIEW_USERS_OPERS
	              Gebruikers: %3d (%2d%%)  Opers: %3d (%2d%%)
STAT_SERVERS_VIEW_RESULTS
	Einde van de lijst -- %d/%d servers getoond.

# DELETE responses
STAT_SERVERS_DELETE_SYNTAX
	SERVERS DELETE server
STAT_SERVERS_DELETE_DONE
	Statistieken voor server %s zijn verwijderd.

# COPY responses
STAT_SERVERS_COPY_SYNTAX
	SERVERS COPY server nieuwe naam
STAT_SERVERS_COPY_DONE
	Statistieken voor server %s gekopieerd naar %s.

# RENAME responses
STAT_SERVERS_RENAME_SYNTAX
	SERVERS RENAME server nieuwe naam
STAT_SERVERS_RENAME_DONE
	Statistieken voor server %s hernoemd naar %s.

# USERS responses
STAT_USERS_SYNTAX
	USERS STATS
STAT_USERS_TOTUSERS
	              Totaal aantal gebruikers: %d
STAT_USERS_TOTOPERS
	                   Totaal aantal opers: %d
STAT_USERS_SERVUSERS
	Gemiddeld aantal gebruikers per server: %d
STAT_USERS_SERVOPERS
	     Gemiddeld aantal opers per server: %d.%d


###########################################################################
#
# NickServ help messages
#
###########################################################################

NICK_HELP
	%S geeft u de mogelijkheid een bijnaam te "registreren"
	en te voorkomen dat anderen deze gebruiken.  %S wordt
	aangestuurd door middel van verscheidene commando's die
	registratie en beheer van bijnamen mogelijk maken. Voor een
	lijst van commando's, tik /msg %S commando en voor
	meer informatie over een specifiek commando, tik
	/msg %S HELP command.
NICK_HELP_EXPIRES
	
	Wanneer een registreerde bijnaam niet gebruikt wordt voor
	%s vervalt de registratie en wordt de bijnaam
	beschikbaar voor algemeen gebruik.
NICK_HELP_WARNING
	
	OPMERKING: Deze dienst is bedoeld als manier voor IRC-
	gebruikers om te zorgen dat hun identiteit niet
	gecompromitteerd wordt.  Ze is NIET bedoeld voor het
	"stelen" van bijnamen, of andere kwaadaardige activiteiten.
	Misbruik van %S resulteert, op zijn minst, in verlies van
	de misbruikte bijnaam of bijnamen.

NICK_HELP_COMMANDS
	De volgende commando's kunnen gebruikt worden met %S:
	
	    REGISTER     Registreer een bijnaam
	    IDENTIFY     Identificeer uzelf met uw wachtwoord
	    DROP         Maak de registratie van een bijnaam ongedaan
NICK_HELP_COMMANDS_AUTH
	    AUTH         Authenticeer een bijnaamregistratie
	    SENDAUTH     Stuur uzelf de authenticatiecode voor een bijnaam
	    REAUTH       Stel een nieuwe code in bij verlies van uw wachtwoord
	    RESTOREMAIL  Herstel het laatstgeauthenticeerde e-mailadres
NICK_HELP_COMMANDS_LINK
	    LINK         Koppel een bijnaam aan die van uzelf
	    UNLINK       Maak een bijnaamkoppeling ongedaan
	    LISTLINKS    Toon een lijst van alle gekoppelde bijnamen
NICK_HELP_COMMANDS_ACCESS
	    ACCESS       Wijzig de lijst van geauthorizeerde bijnamen
NICK_HELP_COMMANDS_AJOIN
	    AJOIN        Wijzig uw autojoinlijst
NICK_HELP_COMMANDS_SET
	    SET          Stel opties in, zoals killbescherming
	    UNSET        Verwijder bijnaaminformatie
	    RECOVER      Verwijder een gebruiker die uw bijnaam in bezit heeft
	    RELEASE      Herwin zeggenschap over uw bijnaam na RECOVER
	    GHOST        Verwijder een "ghost"-sessie van het netwerk
	    INFO         Toon informatie over een bijnaam
NICK_HELP_COMMANDS_LIST
	    LIST         Toon alle bijnamen die overeenkomen met een gegeven masker
	    LISTEMAIL    Toon bijnamen voor een e-mailadres
NICK_HELP_COMMANDS_LISTCHANS
	    LISTCHANS    Toon al uw kamers
	    STATUS       Controleer of er voor een bijnaam is geïdentificeerd

NICK_HELP_REGISTER
	Syntax: %s
	
	Registreer uw bijnaam in het %S-gegevensbestand.  Wanneer uw bijnaam is
	geregistreerd kunt u de commando's SET en ACCESS gebruiken
	om instellingen voor uw bijnaam te configureren zoals u dat wilt.
	Let op dat u het wachtwoord onthoudt dat u gebruikt voor registratie;
	dit heeft u nodig om later wijzigingen te kunnen maken.  (Let op dat
	de lettergrootte uitmaakt! FIDO, Fido en fido
	zijn allen verschillende wachtwoorden!)
NICK_HELP_REGISTER_EMAIL
	
	U mag een e-mailadres toevoegen bij het registreren van uw bijnaam;
	deze kunt u ook later instellen door gebruik van het commando
	SET EMAIL.
NICK_HELP_REGISTER_EMAIL_REQ
	
	U dient een e-mailadres toe te voegen bij het registreren van uw
	bijnaam.  U kunt voorkomen dat uw e-mailadres aan andere gebruikers
	wordt getoond met het commando SET HIDE (tik
	/msg %S HELP SET HIDE voor meer informatie).
NICK_HELP_REGISTER_EMAIL_AUTH
	
	U dient een geldig e-mailadres toe te voegen bij het registreren
	van uw bijnaam; %S stuurt een bericht met een
	authenticatiecode naar dit adres, welke u moet gebruiken met het
	commando AUTH om uw registratie te voltooien.  U kunt voorkomen
	dat uw e-mailadres aan andere gebruikers wordt getoond met het
	commando SET HIDE (tik /msg %S HELP SET HIDE
	voor meer informatie).
NICK_HELP_REGISTER_END
	
	Richtlijnen voor het kiezen van wachtwoorden:
	
	Wachtwoorden dienen niet makkelijk te raden te zijn,  Gebruik van
	uw echte naam als wachtwoord is bijvoorbeeld een slecht idee.
	Gebruik van uw bijnaam als wachtwoord is een nog slechter idee
	en wordt door %S niet toegestaan.  Verder
	zijn korte wachtwoorden gevaarlijk als iemand zomaar wat probeert,
	daarom dient uw wachtwoord minimaal 5 karakters lang te zijn.

NICK_HELP_IDENTIFY
	Syntax: IDENTIFY wachtwoord
	
	Vertelt %S dat uw daadwerkelijk de eigenaar
	bent van deze bijnaam.  Veel commando's vereisen dat u zich
	authenticeert met dit commando voordat u ze kunt gebruiken.
	Het wachtwoord dient hetzelfde te zijn als welke u opgaf met
	het commando REGISTER.

NICK_HELP_DROP
	Syntax: DROP wachtwoord
	
	De-registreert (verwijdert) uw bijnaam uit het
	%S-gegevensbestand.  Een bijnaam die verwijderd
	is kan door iedereen opnieuw geregistreerd worden.
NICK_HELP_DROP_LINK
	
	Let op dat wanneer u een bijnaam verwijdert, alle
	hieraan gekoppelde bijnamen ook verwijderd worden.  Gebruik
	het commando UNLINK om slechts één gekoppelde bijnaam te
	verwijderen.
NICK_HELP_DROP_END
	
	Om te voorkomen dat dit commando per ongeluk gebruikt wordt
	dient u uw wachtwoord op te geven om uw bijnaam te kunnen
	verwijderen.

NICK_HELP_AUTH
	Syntax: AUTH code
	
	Authenticeert registratie of wijziging van e-mailadres voor
	een bijnaam.  Om te zorgen dat uw e-mailadres correct is
	stuurt %S een bericht met een authenticatiecode naar
	het e-mailadres dat u opgeeft bij het registreren van uw
	bijnaam of wijziging van het adres met het commando
	SET EMAIL.  Dit commando geeft u de mogelijkheid om te
	bevestigen dat u het bericht ontvangen heeft en om uw
	bijnaam verder te gebruiken.  Vervang code in het
	commando door de negen-cijferige authenticatiecode uit
	het e-mailbericht.

NICK_HELP_SENDAUTH
	Syntax: SENDAUTH
	
	Stuurt u een kopie van de authenticatiecode voor uw bijnaam
	na gebruik van REGISTER of SET EMAIL.  Wanneer u per ongeluk
	het bericht met de authenticatiecode verwijdert of op een
	andere manier verliest kunt u dit commando gebruiken om een
	extra kopie naar uzelf te versturen.
	
	Om misbruik te voorkomen mag dit commando slechts eens per
	24 uur gebruikt worden.

NICK_HELP_REAUTH
	Syntax: REAUTH
	
	Stelt een authenticatiecode in die u kunt gebruiken om uzelf
	te identificeren voor uw bijnaam.  Wanneer u het wachtwoord
	voor uw bijnaam vergeet, geef dan deze code (die per e-mail
	naar u wordt verstuurd) op met het commando AUTH om
	uzelf te identificeren voor uw bijnaam.  U kunt vervolgens
	uw wachtwoord wijzigen als gewoonlijk, met het commando
	SET PASSWORD.

NICK_HELP_RESTOREMAIL
	Syntax: RESTOREMAIL wachtwoord
	
	Herstelt het laatste geauthenticeerde e-mailadres voor uw
	bijnaam.  Wanneer u per ongeluk het verkeerde e-mailadres
	opgeeft met het commando SET EMAIL kunt u dit commando
	gebruiken om het voorgaande e-mailadres te herstellen en
	de authenticatiecode voor uw bijnaam te verwijderen, zodat
	u zich weer kunt identificeren voor de bijnaam.  U dient
	het wachtwoord van uw bijnaam op te geven met dit commando.
	
	RESTOREMAIL kan alleen gebruikt worden na SET EMAIL en
	voordat de nieuwe authenticatiecode is ingevoerd met het
	commando AUTH.

NICK_HELP_LINK
	Syntax: LINK bijnaam
	
	Koppelt een nieuwe bijnaam aan de bijnaam die u momenteel
	gebruikt, waardoor een nieuwe "alias" voor uw bijnaam
	aangemaakt wordt.  De nieuwe bijnaam is gelijk aan uw
	huidige bijnaam op het gebied van toegangsrechten en
	memo's.  U kunt bijvoorbeeld memo's die verstuurd zijn naar
	één van uw gekoppelde bijnamen lezen met elke andere, en
	wanneer één van uw bijnamen auto-op privileges heeft in
	een kamer krijgt u automatisch operatorstatus, ongeacht
	welke gekoppelde bijnaam u gebruikt.
	
	Gebruik het UNLINK commando om een koppeling ongedaan
	te maken.
	
	Om dit commando te gebruiken dient u zich te identificeren
	voor uw bijnaam (door gebruik van het commando
	IDENTIFY), en dient de te koppelen bijnaam niet
	geregistreerd en niet in gebruik te zijn.

NICK_HELP_UNLINK
	Syntax: UNLINK bijnaam
	
	Verwijdert een koppeling (alias) van uw bijnaam, gemaakt
	met het commando LINK.  U kunt de bijnaam-alias die u
	momenteel gebruikt niet verwijderen.  Wijzig uw bijnaam
	naar een andere gekoppelde bijnaam voordat u dit commando
	gebruikt.
	
	Wanneer u de koppeling van de "hoofdbijnaam" die u ingesteld
	heeft met het commando SET MAINNICK verwijdert wordt uw
	huidige bijnaam automatisch geselecteert als de nieuwe
	hoofdbijnaam.

NICK_HELP_LISTLINKS
	Syntax: LISTLINKS
	
	Toont alle bijnamen die aan uw bijnaam zijn gekoppeld.  De
	hoofdbijnaam voor de groep van gekoppelde bijnamen wordt
	gemarkeerd met een "*" aan de linkerkant van de bijnaam.

NICK_HELP_ACCESS
	Syntax: ACCESS ADD masker
	        ACCESS DEL masker
	        ACCESS LIST
	
	Wijzigt of toont de toegangslijst voor uw bijnaam.
	Dit is de lijst met gebruiker@machine-adressen die
	automatisch door %S herkend worden alszijnde
	toegestaan om uw bijnaam te gebruiken.  Wanneer u
	verbinding maakt met IRC vanaf een adres op deze lijst
	heeft de instelling voor SET KILL van de bijnaam geen
	invloed, en wanneer de optie SECURE uitgeschakeld is
	ontvangt u auto-op en andere privileges in kamers zonder
	gebruik van het commando IDENTIFY.
	
	Het masker dat gegeven wordt met de commando's ADD en
	DEL mag zogenaamde wildcard-karakters (* of ?)
	bevatten.
	
	Voorbeelden:
	
	    ACCESS ADD achurch@*.dragonfire.net
	        Geeft toegang aan gebruiker achurch vanaf elke
	        machine in het domein dragonfire.net.
	
	    ACCESS DEL achurch@*.dragonfire.net
	        Maakt het vorige commando ongedaan.
	
	    ACCESS LIST
	        Toont de huidige toegangslijst.

NICK_HELP_SET
	Syntax: SET optie parameters
	
	Stelt verscheidene bijnaamopties in.  optie kan zijn:
	
	    PASSWORD   Stel uw bijnaamwachtwoord in
	    LANGUAGE   Stel de taal in waarin Services berichten
	                   naar u stuurt
	    URL        Associeer een URL met uw bijnaam
	    EMAIL      Associeer een e-mailadres met uw bijnaam
	    INFO       Stel tekst in die getoond wordt met het commando INFO
	    KILL       Zet killbescherming aan of uit
	    SECURE     Zet bijnaambeveiliging aan of uit
	    PRIVATE    Voorkom dat uw bijnaam getoond wordt in een
	                   /msg %S LIST
	    HIDE       Verberg bepaalde onderdelen van uw bijnaaminformatie
	    TIMEZONE   Stel de tijdzone in die gebruikt wordt voor het tonen van
	                   tijdsaanduidingen
NICK_HELP_SET_OPTION_MAINNICK
	    MAINNICK   Stel de bijnaam in die getoond wordt in kamer-toegangslijsten
NICK_HELP_SET_END
	
	Om dit commando te gebruiken dient u zich eerst te identificeren
	met uw wachtwoord (tik /msg %S HELP IDENTIFY voor meer
	informatie).
	
	Tik /msg %S HELP SET optie voor meer informatie
	over een specifieke optie.

NICK_HELP_SET_PASSWORD
	Syntax: SET PASSWORD nieuw-wachtwoord
	
	Wijzigt het wachtwoord dat gebruikt wordt om u te
	identificeren alszijnde de eigenaar van de bijnaam.

NICK_HELP_SET_LANGUAGE
	Syntax: SET LANGUAGE nummer
	
	Wijzigt de taal die Services gebruikt om berichten naar u
	te versturen (bijvoorbeeld als reactie op een commando dat
	u stuurt).
	nummer dient gekozen te worden uit de volgende lijst van
	ondersteunde talen:
	

NICK_HELP_SET_URL
	Syntax: SET URL url
	
	Associeert de gegeven URL met uw bijnaam.  Deze URL
	wordt getoond wanneer iemand informatie over uw bijnaam
	opvraagt met het commando INFO.

NICK_HELP_SET_EMAIL
	Syntax: SET EMAIL adres
	
	Associeert het gegeven e-mailadres met uw bijnaam.
	Dit adres wordt getoond wanneer iemand informatie over uw
	bijnaam opvraagt met het commando INFO.

NICK_HELP_SET_INFO
	Syntax: SET INFO tekst
	
	Associeert de gegeven tekst met uw bijnaam.  Deze tekst
	wordt getoond wanneer iemand informatie over uw bijnaam
	opvraagt met het commando INFO.

NICK_HELP_SET_KILL
	Syntax: SET KILL {ON | QUICK | IMMED | OFF}
	
	Zet de automatische killbeschermingsoptie voor uw bijnaam
	aan of uit.  Wanneer killbescherming is geactiveerd en
	een andere gebruiker uw bijnaam probeert te gebruiken
	krijgt deze één minuut om zijn/haar bijnaam te wijzigen,
	waarna %S de gebruiker hardhandig van IRC verwijdert.
	
	Wanneer u QUICK selecteert krijgt de gebruiker slechts
	20 seconden om zijn/haar bijnaam te wijzigen, in plaats van
	de gewoonlijke 60.  Wanneer u IMMED selecteert wordt de
	gebruiker onmiddelijk verwijderd, zonder eerst een
	waarschuwing te hebben gekregen, of de kans om zijn/haar
	bijnaam te wijzigen.  Gebruik deze optie niet tenzij dit
	strict noodzakelijk is.  Verder kunnen uw netwerkbeheerders
	deze optie hebben uitgeschakeld.

NICK_HELP_SET_SECURE
	Syntax: SET SECURE {ON | OFF}
	
	Zet de beveiligingsopties van %S aan of uit voor
	uw bijnaam.  Wanneer SECURE is ingesteld dient u uw
	wachtwoord in te voeren voordat u herkend wordt als de
	eigenaar van uw bijnaam, ongeacht of uw adres op de
	toegangslijst staat.  Wanneer u echter op de toegangslijst
	staat zal %S geen autokill op u uitvoeren, ongeacht
	de instelling van de optie KILL.

NICK_HELP_SET_PRIVATE
	Syntax: SET PRIVATE {ON | OFF}
	
	Zet de optie PRIVATE (privé) van %S aan of uit
	voor uw bijnaam.  Wanneer PRIVATE aan staat wordt uw
	bijnaam niet getoond in met het commando LIST
	gegenereerde overzichten.
	(Iedereen die uw bijnaam kent kan echter nog steeds
	informatie over deze bijnaam verkrijgen door gebruik van
	het commando INFO.)

NICK_HELP_SET_HIDE
	Syntax: SET HIDE {EMAIL | USERMASK | QUIT} {ON | OFF}
	
	Geeft u de mogelijkheid om te voorkomen dat bepaalde informatie
	getoond wordt wanneer iemand het commando %S INFO uitvoert
	op uw bijnaam.  U kunt uw e-mailadres (EMAIL), het
	laatstgeziene gebruiker@machine-masker (USERMASK) en
	het laatste vertrekbericht (QUIT) verbergen.  De tweede
	parameter specificeert of de informatie getoond moet worden
	(OFF), of moet worden verborgen (ON).

NICK_HELP_SET_TIMEZONE
	Syntax: SET TIMEZONE {UTC-verschil | tijdzone | DEFAULT}
	
	Stelt de tijdzone in voor het tonen van tijdstippen (zoals de
	laatst gebruikte tijd van een bijnaam of kamer), in plaats van
	gebruik te maken van de tijdzone waarin Services werkt.  De
	instelling DEFAULT kan gebruikt worden om terug te keren naar
	de tijdzone van Services.  De tijdzone kan gespecificeerd worden
	op twee manieren:
	
	   - Als een UTC-verschil, het tijdsverschil tussen uw lokale
	     tijd en UTC (Universal Coordinated Time, ookwel bekend als
	     GMT, Greenwich Mean Time).  Het verschil wordt opgegeven als
	     een "+" of "-" om aan te geven dat de klok voor (oost)
	     of achter (west) loopt op UTC, gevolgd door een geheel aantal
	     uren of een tekstregel in de vorm "hh:mm".
	
	   - Als een tijdzone-naam, zoals "GMT" of "EDT".  Niet alle
	     tijdzone-namen worden ondersteund door Services en de zones
	     van sommige landen hebben dezelfde naam, dus wanneer u een
	     foutmelding krijgt of wanneer de door Services getoonde tijd
	     incorrect is dient u een UTC-verschil te gebruiken.
	
	Let op dat Services de tijd niet automatisch aanpast naar zomertijd;
	u dient handmatig de tijdzone aan te passen.
	
	Voorbeelden:
	
	    SET TIMEZONE -5
	        Zet de tijdzone 5 uur achter op UTC.
	
	    SET TIMEZONE +3:30
	        Zet de tijdzone 3 uur en 30 minuten voor op UTC.
	
	    SET TIMEZONE JST
	        Zet de tijdzone op Japan Standard Time (UTC+9:00).

NICK_HELP_SET_MAINNICK
	Syntax: SET MAINNICK bijnaam
	
	Wanneer u meerdere bijnamen aan elkaar gekoppeld heeft geeft
	dit commando u de mogelijkheid te selecteren welke bijnaam
	getoond wordt in kamer-toegangslijsten.  Wanneer u de koppeling
	met deze bijnaam verwijdert wordt automatisch een andere bijnaam
	geselecteerd.

NICK_HELP_UNSET
	Syntax: UNSET {URL | EMAIL | INFO}
	
	Geeft u de mogelijkheid de URL (URL), het e-mailadres
	(EMAIL), of de informatietekst (INFO) behorend bij uw
	bijnaam te verwijderen.

NICK_HELP_UNSET_REQ_EMAIL
	Syntax: UNSET {URL | INFO}
	
	Geeft u de mogelijkheid de URL (URL) of informatietekst
	(INFO) behorend bij uw bijnaam te verwijderen.

NICK_HELP_RECOVER
	Syntax: RECOVER bijnaam [wachtwoord]
	
	Geeft u de mogelijkheid om uw bijnaam terug te winnen
	wanneer iemand anders deze heeft ingenomen.  Dit doet
	hetzelfde als %S automatisch doet wanneer iemand een
	bijnaam met killbescherming probeert te gebruiken.
	
	Wanneer dit commando gegeven wordt maakt %S een
	nepgebruiker aan met dezelfde bijnaam als de gebruiker
	waarvan u de bijnaam probeert terug te winnen.  Dit zorgt
	ervoor dat IRC-servers de andere gebruiker van het netwerk
	verwijderen.  De nepgebruiker blijft aanwezig voor %s
	om te zorgen dat de andere gebruiker niet meteen weer
	verbinding maakt.  Na deze tijdsperiode kunt u uw bijnaam
	weer terugvorderen.  U kunt ook het commando RELEASE
	(/msg %S HELP RELEASE) gebruiken om uw bijnaam sneller
	terug te krijgen.
	
	Om het commando RECOVER te gebruiken voor een bijnaam
	dient u het juiste wachtwoord voor de bijnaam op te geven,
	of reeds geïdentificeerd te zijn voor een bijnaam die
	gekoppeld is aan de gegeven bijnaam.

NICK_HELP_RELEASE
	Syntax: RELEASE bijnaam [wachtwoord]
	
	Geeft %S opdracht om een bijnaam die vastgehouden
	wordt als gevolg van automatische killbescherming of gebruik
	van het commando RECOVER vrij te geven.  Standaard wordt
	een bijnaam %s vastgehouden; dit commando geeft een bijnaam
	sneller vrij.
	
	Om het commando RELEASE te gebruiken voor een bijnaam
	dient u het juiste wachtwoord voor de bijnaam op te geven,
	of reeds geïdentificeerd te zijn voor een bijnaam die
	gekoppeld is aan de gegeven bijnaam.

NICK_HELP_GHOST
	Syntax: GHOST bijnaam [wachtwoord]
	
	Verwijdert een "ghost" IRC-sessie met uw bijnaam.  Een
	"ghost"-sessie is een gebruiker die niet daadwerkelijk
	aanwezig is, maar waarvan de IRC-server denkt dat dit wel
	het geval is, om de één of andere reden.  Dit kan
	gebeuren wanneer uw computer vastloopt of uw Internet-
	of modemverbinding verbroken wordt terwijl u zich op IRC
	bevindt.
	
	Om het commando GHOST te gebruiken voor een bijnaam
	dient u het juiste wachtwoord voor de bijnaam op te geven,
	of reeds geïdentificeerd te zijn voor een bijnaam die
	gekoppeld is aan de gegeven bijnaam.

NICK_HELP_INFO
	Syntax: INFO bijnaam [ALL]
	
	Toont informatie over de gegeven bijnaam, zoals de eigenaar
	van de bijnaam, het laatst gezien adres en de laatst
	geziene tijd, en ingestelde opties.  Wanneer u zich voor
	de betreffende bijnaam heeft geïdentificeerd en de optie
	ALL opgeeft wordt alle informatie aan u getoond,
	ongeacht of deze verborgen is of niet.
NICK_HELP_INFO_AUTH
	
	Wanneer het e-mailadres van de bijnaam nog niet is
	bevestigd met het commando AUTH wordt het aan niemand
	getoond, behalve aan de eigenaar van de bijnaam en aan
	Servicesbeheerders, ongeacht de instelling voor SET HIDE
	EMAIL van de bijnaam.

NICK_HELP_LISTCHANS
	Syntax: LISTCHANS
	
	Toont de namen en het totale aantal van de kamers die
	u geregistreerd heeft met uw bijnaam.

NICK_HELP_LIST
	Syntax: LIST [+begin] patroon
	
	Toont alle geregistreerde bijnaam waarvan de bijnaam zelf
	of het laatst geziene adres (in gebruiker@machine formaat)
	overeenkomt met het gegeven patroon.  Bijnamen waarvoor de
	optie PRIVATE is ingesteld worden niet getoond. Wanneer de
	parameter begin is opgegeven specificeert deze hoeveel
	overeenkomende invoeren moet worden overgeslagen voordat
	resultaten getoond worden (+50 zou er bijvoorbeeld voor zorgen
	dat de eerste 50 overeenkomende invoeren worden overgeslagen).
	
	Voorbeelden:
	
	    LIST joeuser@foo.com
	        Toon alle geregistreerde bijnamen die het laatst
	        gebruikt zijn door joeuser@foo.com.
	
	    LIST *Bot*
	        Toon alle geregistreerde bijnamen met Bot in de
	        naam (lettergrootte maakt uit).
	
	    LIST *@*.bar.org
	        Toont alle geregistreerde bijnamen die voor het
	        laatst gebruikt zijn door gebruikers in het domein
	        bar.org.
NICK_HELP_LIST_OPERSONLY
	
	Alleen IRC-Operators mogen dit commando gebruiken.

NICK_HELP_LISTEMAIL
	Syntax: %s
	
	Toont evenals het commando LIST geregistreerde bijnamen,
	maar zoekt overeenkomsten gebaseerd op e-mailadressen in
	plaats van gebruiker@machine-adressen.
	Zie het hulpbericht voor het commando LIST (/msg %S HELP
	LIST) voor meer informatie.

NICK_HELP_STATUS
	Syntax: STATUS bijnaam...
	
	Toont of de gebruiker van de gegeven bijnaam herkend wordt
	alszijnde de eigenaar van de bijnaam.  Het antwoord heeft
	het volgende formaat:
	
	    bijnaam statuscode
	
	waarbij bijnaam de bijnaam is de opgegeven werd met het
	commando, en statuscode één van de volgende is:
	
	    0 - gebruiker niet aanwezig of bijnaam niet geregistreerd
	    1 - gebruiker niet herkend als eigenaar
	    2 - gebruiker herkend als eigenaar, maar alleen via de toegangslijst
	    3 - gebruiker herkend als eigenaar via wachtwoordauthenticatie
	
	Maximaal zestien bijnamen mogen gegeven worden met elk commando; de
	rest wordt genegeerd.  Wanneer geen bijnaam wordt opgegeven wordt geen
	foutbericht gegenereerd.

NICK_HELP_AJOIN
	Syntax: AJOIN ADD kamer
	        AJOIN DEL kamer
	        AJOIN LIST
	
	Wijzigt of toont de autojoinlijst voor uw bijnaam.
	Dit is de lijst van kamers die u automatisch wilt binnengaan
	wanneer u zich identificeert bij %S.  U mag alleen
	kamernamen die beginnen met "#" plaatsen in de autojoinlijst
	(lokale kamers kunnen niet worden toegevoegd).
	
NICK_HELP_AJOIN_END
	Let op dat de IRC-server u kan verbieden om sommige kamers
	op uw autojoinlijst binnen te gaan, zoals kamers die een
	sleutel hebben (modus +k) of waar u alleen met een uitnodiging
	naar binnen mag (modus +i).
NICK_HELP_AJOIN_END_CHANSERV
	Let op dat de IRC-server u kan verbieden om sommige kamers
	op uw autojoinlijst binnen te gaan, zoals kamers die een
	sleutel hebben (modus +k) of waar u alleen met een uitnodiging
	naar binnen mag (modus +i).
	Wanneer u echter toegang heeft tot het commando %s INVITE
	voor een geregistreerde kamer, dan zal %S u automatisch
	uitnodigen in de kamer.

NICK_OPER_HELP_COMMANDS
	
	De volgende commando's zijn beschikbaar voor Servicesbeheerders:
	
	    DROPNICK     Verwijder een bijnaam
NICK_OPER_HELP_COMMANDS_DROPEMAIL
	    DROPEMAIL    Verwijder alle bijnamen behorend bij een e-mailadres
NICK_OPER_HELP_COMMANDS_GETPASS
	    GETPASS      Toon het wachtwoord voor een bijnaam
	                     (alleen wanneer encryptie niet in gebruik is)
NICK_OPER_HELP_COMMANDS_FORBID
	    FORBID       Voorkom dat een bijnaam wordt gebruikt/geregistreerd
	    SUSPEND      Voorkom dat voor een bijnaam wordt geïdentificeerd
	    UNSUSPEND    Maak buitenwerkingstelling van een bijnaam ongedaan
NICK_OPER_HELP_COMMANDS_SETAUTH
	    SETAUTH      Stel een nieuwe authenticatiecode in voor een bijnaam
	    GETAUTH      Toon de authenticatiecode voor een bijnaam
	    CLEARAUTH    Verwijder de authenticatiecode voor een bijnaam
NICK_OPER_HELP_COMMANDS_END
	
	Servicesbeheerders kunnen tevens de volgende commando's gebruiken
	voor elke bijnaam, zonder zich te identificeren:
	
	    SET, ACCESS LIST, INFO ALL
	
	Zie het hulpbericht voor elk van deze commando's voor specifieke details.

NICK_OPER_HELP_DROPNICK
	Syntax: DROPNICK bijnaam
	
	Verwijdert de gegeven bijnaam uit het gegevensbestand van
	%S, samen met alle hieraan gekoppelde bijnamen.
	
	Beperkt tot Servicesbeheerders.

NICK_OPER_HELP_DROPEMAIL
	Syntax: DROPEMAIL patroon
	        DROPEMAIL-CONFIRM patroon
	
	Verwijdert alle bijnamen met een e-mailadres dat overeenkomt
	met het gegeven patroon.  Wanneer patroon slechts
	bestaat uit het karakter "-" worden alle bijnamen zonder
	e-mailadres verwijderd.
	
	Om te voorkomen dat bijnamen per ongeluk verwijderd worden
	met dit commando dient het tweemaal gegeven te worden: eerst
	door gebruik van het commando DROPEMAIL, vervolgens door
	gebruik van het commando DROPEMAIL-CONFIRM met hetzelfde
	patroon.
	Het commando DROPEMAIL-CONFIRM dient binnen %s van het
	commando DROPEMAIL gegeven te worden, en door dezelfde
	persoon.
	Wanneer het commando DROPEMAIL meer dan vier keer achter
	elkaar wordt gegeven, wordt het oudste commando afgedankt
	en niet langer herkend voor een DROPEMAIL-CONFIRM commando.
	
	Beperkt tot Servicesbeheerders.

NICK_OPER_HELP_SET
	
	Servicesbeheerders kunnen ook de optie NOEXPIRE
	instellen, welke kan voorkomen dat bijnamen vervallen.
	Daarnaast kunnen Servicesbeheerders opties instellen voor
	elke bijnaam zonder een wachtwoord in te voeren, door
	gebruik van SET !bijnaam optie parameters.

NICK_OPER_HELP_SET_NOEXPIRE
	Syntax: SET [!bijnaam] NOEXPIRE {ON | OFF}
	
	Stelt in of de gegeven bijnaam zal vervallen.  Wanneer deze
	optie op ON gezet wordt vervalt de bijnaam niet.
	Wanneer geen bijnaam wordt opgegeven wijzigt dit commando
	de vervalinstelling voor uw eigen bijnaam.
	
	Beperkt tot Servicesbeheerders.

NICK_OPER_HELP_UNSET
	
	Servicesbeheerders mogen het commando UNSET gebruiken voor elke
	bijnaam door toevoeging van !bijnaam voor de optienaam,
	net zoals bij het commando SET.

NICK_OPER_HELP_OLD_UNLINK
	
	Servicesbheerders mogen de koppeling van elke bijnaam
	verwijderen zonder een wachtwoord in te voeren.

NICK_OPER_HELP_UNLINK
	Syntax: UNLINK bijnaam [FORCE]
	
	Verwijdert een koppeling (alias) naar uw bijnaam die
	gecreëerd is met het commando LINK.  U kunt de
	koppeling van de bijnaam die u momenteel gebruikt niet
	verwijderen; wijzig uw bijnaam naar een andere gekoppelde
	bijnaam voordat u dit commando gebruikt.
	
	Wanneer u de "hoofdbijnaam" verwijdert die u geselecteerd
	heeft met het commando SET MAINNICK wordt uw huidige
	bijnaam automatisch geselecteerd als de hoofdbijnaam.
	
	Servicesbeheerders mogen de optie FORCE toevoegen, welke
	het mogelijk maakt koppelingen naar bijnamen anders dan
	hun eigen bijnaam te verwijderen.

NICK_OPER_HELP_OLD_LISTLINKS
	Syntax: LISTLINKS bijnaam
	
	Toont bijnamen welke gekoppeld zijn aan de gegeven bijnaam.
	
	Beperkt tot Servicesbeheerders.

NICK_OPER_HELP_LISTLINKS
	Syntax: LISTLINKS [bijnaam]
	
	Toont alle bijnamen die gekoppeld zijn aan uw bijnaam.
	De hoofdbijnaam voor de groep van gekoppelde bijnamen wordt
	aangeduid met een "*" aan de linkerkant van de bijnaam.
	Services-operators mogen gekoppelde bijnamen tonen voor elke
	bijnaam door de betreffende bijnaam te specificeren in het
	commando.

NICK_OPER_HELP_ACCESS
	
	Servicesbeheerders mogen een bijnaam als parameter opgeven met
	het command LIST om de toegangslijst voor elke willekeurige
	bijnaam te tonen.

NICK_OPER_HELP_INFO
	
	Servicesbeheerders mogen de parameter ALL gebruiken met elke bijnaam.

NICK_OPER_HELP_LISTCHANS
	
	Servicesbeheerders mogen de kamers voor elke bijnaam
	tonen door de bijnaam mee te geven met het commando
	(bijvoorbeeld /msg %S LISTCHANS OtherNick).

NICK_OPER_HELP_LIST
	Syntax: LIST [+begin] patroon [FORBIDDEN] [NOEXPIRE] [SUSPENDED]
	
	Toont alle geregistreerde bijnamen waarvan ofwel de bijnaam zelf,
	of het laatsgeziene adres (in het formaat gebruiker@machine)
	overeenkomt met het gegeven patroon.  Bijnamen waarvoor de optie
	PRIVATE is ingesteld worden alleen getoond aan Servicesbeheerders.
	Voor Servicesbeheerders worden bijnamen waarvoor de optie NOEXPIRE
	is ingesteld aangeduid met een ! voor de bijnaam, en bijnamen
	die buiten werking gesteld zijn worden aangeduid met een *.
	
	Wanneer de opties FORBIDDEN, SUSPENDED of NOEXPIRE mee
	worden gegeven worden alleen bijnamen getoond die respectievelijk
	zijn verboden, buiten werking gesteld zijn, of waarvoor de optie NOEXPIRE
	is ingesteld.  De opties kunnen gebruikt worden in elke volgorde en
	combinatie.  Deze opties zijn beperkt tot Servicesbeheerders.
NICK_OPER_HELP_LIST_AUTH
	Syntax: LIST [+begin] patroon [FORBIDDEN] [NOEXPIRE] [SUSPENDED] [NOAUTH]
	
	Toont alle geregistreerde bijnamen waarvan ofwel de bijnaam zelf,
	of het laatsgeziene adres (in het formaat gebruiker@machine)
	overeenkomen met het gegeven patroon.  Bijnamen waarvoor de optie
	PRIVATE is ingesteld worden alleen getoond aan Servicesbeheerders.
	Voor Servicesbeheerders worden bijnamen waarvoor de optie NOEXPIRE
	is ingesteld aangeduid met een ! voor de bijnaam, bijnamen
	die buiten werking gesteld zijn worden aangeduid met een *,
	en bijnamen die nog niet zijn geauthenticeerd (met behulp van het
	commando AUTH) worden aangeduid met een ?.
	
	Wanneer de opties FORBIDDEN, SUSPENDED, NOEXPIRE of
	NOAUTH mee worden gegeven worden alleen bijnamen getoond die
	respectievelijk zijn verboden, buiten werking gesteld zijn, waarvoor
	de opties NOEXPIRE is ingesteld, of die nog niet zijn
	geauthenticeerd.  De opties kunnen gebruikt worden in elke volgorde
	en combinatie.  Deze opties zijn beperkt tot Servicesbeheerders.
NICK_OPER_HELP_LIST_END
	
	Wanneer de parameter begin wordt opgegeven specificeert deze
	hoeveel resultaten overgeslagen worden voordat resultaten getoond
	worden (+50 zorgt er bijvoorbeeld voor dat de eerste 50 resultaten
	worden overgeslagen).
	
	Voorbeelden:
	
	    LIST joeuser@foo.com
	        Toont alle geregistreerde bijnamen die het laatst
	        zijn gebruikt door joeuser@foo.com.
	
	    LIST *Bot*
	        Toont alle geregistreerde bijnamen met Bot in
	        de naam (lettergrootte maakt niet uit).
	
	    LIST * NOEXPIRE
	        Toont alle geregistreerde bijnamen die ingesteld
	        zijn om niet te vervallen.

NICK_OPER_HELP_GETPASS
	Syntax: GETPASS bijnaam
	
	Geeft het wachtwoord voor de gegeven bijnaam.  Let op
	dat wanneer dit commando gebruikt wordt er een bericht
	wordt opgeslagen en verstuurd wordt als WALLOPS/GLOBOPS,
	met daarin de persoon die het commando uitvoerde en de
	bijnaam waarvoor het commando werd uitgevoerd.
	
	Dit commando is mogelijk niet beschikbaar wanneer encryptie
	geactiveerd is, afhankelijk van het type encryptie dat gebruikt
	wordt.
	
	Beperkt tot Servicesbeheerders.

NICK_OPER_HELP_FORBID
	Syntax: FORBID bijnaam
	
	Voorkomt dat een bijnaam geregistreerd of gebruikt mag
	worden door wie dan ook.  Kan ongedaan worden gemaakt door
	de bijnaam te verwijderen.
	
	Beperkt tot Servicesbeheerders.

NICK_OPER_HELP_SUSPEND
	Syntax: SUSPEND [+vervaltijd] bijnaam reden
	
	Voorkomt dat een bijnaam en alle hieraan gekoppelde bijnamen
	worden gebruikt en dat ervoor wordt geïdentificeerd.  De
	bijnaam kan weer in gebruik gesteld worden met het commando
	UNSUSPEND, waardoor deze weer gebruikt kan worden als
	voorheen.  De parameter vervaltijd wordt op dezelfde
	manier geïnterpreteerd als bij het %s commando AKILL.
	
	Beperkt tot Servicesbeheerders.

NICK_OPER_HELP_UNSUSPEND
	Syntax: UNSUSPEND bijnaam
	
	Geeft mensen de mogelijkheid om een bijnaam die voorheen
	buiten werking gesteld was te gebruiken, en ervoor te
	identificeren.
	
	Beperkt tot Servicesbeheerders.

NICK_OPER_HELP_AJOIN
	
	Servicesbeheerders kunnen de autojoinlijst voor elke
	bijnaam tonen met LIST bijnaam.

NICK_OPER_HELP_SETAUTH
	Syntax: SETAUTH bijnaam
	
	Stelt een nieuwe authenticatiecode in voor de gegeven
	bijnaam.  De eigenaar van de bijnaam dient het AUTH
	commando te gebruiken om zich weer voor de betreffende
	bijnaam te mogen identificeren.  Wanneer de bijnaam
	momenteel in gebruik is wordt een bericht gestuurd naar
	de gebruiker van de bijnaam, en wordt deze gebruiker niet
	langer behandeld als geïdentificeerd voor de bijnaam.
	
	Beperkt tot Servicesbeheerders.

NICK_OPER_HELP_GETAUTH
	Syntax: GETAUTH bijnaam
	
	Vraagt de authenticatiecode voor  bijnaam op, mits deze
	code bestaat.
	
	Beperkt  tot Servicesbeheerders.

NICK_OPER_HELP_CLEARAUTH
	Syntax: CLEARAUTH bijnaam
	
	Verwijdert elke authenticatiecode die geassocieerd is met
	bijnaam, zodat de bijnaam weer normaal gebruikt kan worden.
	
	Beperkt tot Servicesbeheerders.


###########################################################################
#
# ChanServ help messages
#
###########################################################################

# Used in OP/VOICE/etc for required access levels for commands
CHAN_HELP_REQSOP_LEVXOP
	niveau 100 (SOP)
CHAN_HELP_REQSOP_LEV
	niveau 100
CHAN_HELP_REQSOP_XOP
	SOP
CHAN_HELP_REQAOP_LEVXOP
	niveau 50 (AOP)
CHAN_HELP_REQAOP_LEV
	niveau 50
CHAN_HELP_REQAOP_XOP
	AOP
CHAN_HELP_REQHOP_LEVXOP
	niveau 40 (HOP)
CHAN_HELP_REQHOP_LEV
	niveau 40
CHAN_HELP_REQHOP_XOP
	HOP
CHAN_HELP_REQVOP_LEVXOP
	niveau 30 (VOP)
CHAN_HELP_REQVOP_LEV
	niveau 30
CHAN_HELP_REQVOP_XOP
	VOP

# Real help messages start here
CHAN_HELP
	%S geeft u de mogelijkheid tot registratie en controle
	van verschillende aspecten van kamers.  %S kan vaak
	voorkomen dat kwaadaardige gebruikers kamers "overnemen"
	door te beperken wie kameroperator-rechten krijgt.  Tik
	/msg %S HELP COMMANDS voor een lijst van commando's
	voor %S, tik /msg %S commando om een commando te
	gebruiken, of tik /msg %S HELP commando voor meer
	informatie over een commando.

CHAN_HELP_EXPIRES
	
	Let op dat een kamer die niet gebruikt wordt voor %s
	(oftewel: waarvoor geen gebruiker op de toegangslijst van
	de kamer binnenkomt gedurende deze periode) automatisch wordt
	verwijderd.

CHAN_HELP_COMMANDS
	De volgende commando's kunnen gebruikt worden met %S:
	
	    REGISTER   Registreer een kamer
	    IDENTIFY   Identificeer uzelf met uw wachtwoord
	    DROP       Maak de registratie van een kamer ongedaan
	    SET        Stel kameropties en informatie in
	    UNSET      Verwijder kamerinformatie
	    INFO       Toon kameropties en informatie
CHAN_HELP_COMMANDS_LIST
	    LIST       Toon geregistreerde kamers
CHAN_HELP_COMMANDS_AKICK
	    AKICK      Beheer de autokicklijst
CHAN_HELP_COMMANDS_LEVELS
	    ACCESS     Beheer de algemene kamertoegangslijst
	    LEVELS     Verander de afstelling van kamerprivilege-niveaus
CHAN_HELP_COMMANDS_XOP
	    SOP        Beheer de SuperOp-lijst
	    AOP        Beheer de AutoOp-lijst
CHAN_HELP_COMMANDS_HOP
	    HOP        Beheer de AutoHalfop-lijst
CHAN_HELP_COMMANDS_XOP_2
	    VOP        Beheer de AutoVoice-lijst
	    NOP        Beheer de NeverOp-lijst
CHAN_HELP_COMMANDS_OPVOICE
	    OP         Geef een gebruiker kameroperator-status (+o)
	    DEOP       Verwijder kameroperator-status (+o)
	    VOICE      Geef een gebruiker voice-status (+v)
	    DEVOICE    Verwijder voice-status (+v)
CHAN_HELP_COMMANDS_HALFOP
	    HALFOP     Geef een gebruiker halfop-status (+h)
	    DEHALFOP   Verwijder halfop-status (+h)
CHAN_HELP_COMMANDS_PROTECT
	    PROTECT    Geef een gebruiker beschermde status (+a)
	    DEPROTECT  Verwijder beschermde status (+a)
CHAN_HELP_COMMANDS_INVITE
	    INVITE     Nodig uzelf uit tot een kamer
	    UNBAN      Verwijder verbanningen van uzelf in een kamer
	    KICK       Verwijder een gebruiker uit een kamer
	    TOPIC      Verander het onderwerp van een kamer
	    CLEAR      Verwijder kamer-modi of groepen gebruikers
	    STATUS     Toon het toegangsniveau van een gebruiker in een kamer

CHAN_HELP_REGISTER
	Syntax: REGISTER kamer wachtwoord beschrijving
	
	Registreert een kamer in het %S-gegevensbestand.  Om dit commando te
	gebruiken dient u operator te zijn in de kamer die u probeert
	te registreren.  Het wachtwoord wordt gebruikt met het commando
	 IDENTIFY om u de mogelijkheid te geven op een later
	tijdstip aanpassingen te maken in de kamerinstellingen.
	De laatste parameter, die opgegeven moet worden, is
	een globale beschrijving van het doel van de kamer.
	
	Wanneer u een kamer registreert wordt u opgeslagen als de
	"oprichter" van de kamer.  De kameroprichter mag alle
	instellingen voor de kamer wijzigen; %S geeft
	de oprichter ook automatisch kameroperator-privileges
	wanneer hij/zij de kamer binnenkomt.
	
	OPMERKING: Om een kamer te registreren dient u eerst een
	bijnaam geregistreerd te hebben, en hiervoor geïdentificeerd
	te zijn.  Wanneer u dit niet gedaan heeft, tik
	/msg %s HELP voor informatie over hoe dit te doen.
CHAN_HELP_REGISTER_ADMINONLY
	
	Beperkt tot Servicesbeheerders.

CHAN_HELP_IDENTIFY
	Syntax: IDENTIFY kamer wachtwoord
	
	Authenticeert u bij %S als de oprichter van de
	gegeven kamer.  Veel commando's vereisen dat u dit
	doet voordat u ze kunt gebruiken.  Het wachtwoord
	dient hetzelfde te zijn als welke u verstuurd heeft
	met het commando REGISTER.

CHAN_HELP_DROP
	Syntax: DROP kamer wachtwoord
	
	Maakt de registratie van de gegeven kamer ongedaan.  Kan
	alleen gebruikt worden door de kameroprichter.
	Het wachtwoord dient gegeven te worden om te voorkomen
	dat een kamer per ongeluk verwijderd wordt.

CHAN_HELP_SET
	Syntax: SET kamer optie parameters
	
	Geeft de kameroprichter de mogelijkheid verscheidene
	kameropties en andere informatie in te stellen.  De
	oprichter dient het commando IDENTIFY te gebruiken
	voor het gebruik van SET.
	
	Beschikbare opties:
	
	    FOUNDER     Stel de oprichter van een kamer in
	    SUCCESSOR   Stel de opvolger van een kamer in
	    PASSWORD    Stel het oprichterwachtwoord in
	    DESC        Verander de omschrijving van de kamer
	    URL         Associeer een URL met een kamer
	    EMAIL       Associeer een e-mailadres met een kamer
	    ENTRYMSG    Stel een bericht in dat verstuurd wordt naar
	                   gebruikers wanneer ze de kamer binnenkomen
	    KEEPTOPIC   Behoudt het onderwerp wanneer een kamer niet
	                   in gebruik is
	    TOPICLOCK   Onderwerp kan alleen gewijzigd worden met TOPIC
	    MLOCK       Zet kamermodi vast aan of uit
	    PRIVATE     Verberg een kamer voor het commando LIST
	    RESTRICTED  Beperk toegang tot de kamer
	    SECURE      Activeer %S beveiligingsopties
	    SECUREOPS   Strictere controle op kameroperator-status
	    LEAVEOPS    Verwijder geen operatorstatus wanneer gebruikers
	                  de kamer binnengaan
	    OPNOTICE    Verstuur een melding wanneer OP/VOICE wordt gebruikt
	    ENFORCE     Dwing auto-op en auto-voice status af
	
	Tik /msg %S HELP SET optie voor meer informatie over een
	bepaald onderwerp.

CHAN_HELP_SET_FOUNDER
	Syntax: SET kamer FOUNDER bijnaam
	
	Wijzigt de oprichter van een kamer.  De nieuwe bijnaam
	dient geregistreerd te zijn.

CHAN_HELP_SET_SUCCESSOR
	Syntax: SET kamer SUCCESSOR bijnaam
	
	Wijzigt de opvolger van een kamer.  Wanneer de bijnaam
	van de oprichter vervalt of wordt verwijderd terwijl de
	kamer nog geregistreerd is, wordt de opvolger de nieuwe
	oprichter van de kamer.  Wanneer de opvolger echter al
	teveel kamers geregistreerd heeft (%d) wordt de kamer
	verwijderd, net als wanneer er geen opvolger is ingesteld.
	De bijnaam dient geregistreerd te zijn.

CHAN_HELP_SET_PASSWORD
	Syntax: SET kamer PASSWORD wachtwoord
	
	Stelt het wachtwoord in dat gebruikt wordt voor
	identificatie als oprichter van de kamer.

CHAN_HELP_SET_DESC
	Syntax: SET kamer DESC beschrijving
	
	Stelt de beschrijving van de kamer in, welke wordt
	getoont door de commando's LIST en INFO.

CHAN_HELP_SET_URL
	Syntax: SET kamer URL url
	
	Associeert de gegeven URL met de kamer.  Deze URL wordt
	getoond wanneer iemand informatie over de kamer opvraagt
	met het commando INFO.

CHAN_HELP_SET_EMAIL
	Syntax: SET kamer EMAIL adres
	
	Associeert het gegeven e-mailadres met de kamer.
	Dit adres wordt getoond wanneer iemand informatie over
	de kamer opvraagt met het commando INFO.

CHAN_HELP_SET_ENTRYMSG
	Syntax: SET kamer ENTRYMSG bericht
	
	Stelt het bericht in dat verstuurd wordt met behulp
	van /NOTICE aan gebruikers wanneer zij de kamer
	binnengaan.

CHAN_HELP_SET_KEEPTOPIC
	Syntax: SET kamer KEEPTOPIC {ON | OFF}
	
	Zet de optie onderwerpbehoud aan of uit voor een
	kamer. Wanneer onderwerpbehoud is ingesteld wordt het
	onderwerp van de kamer onthouden door %S, zelfs nadat de
	laatste gebruiker de kamer verlaat, en wordt deze weer
	ingesteld bij de volgende keer dat de kamer wordt
	gecreëerd.

CHAN_HELP_SET_TOPICLOCK
	Syntax: SET kamer TOPICLOCK {ON | OFF}
	
	Zet de optie onderwerpslot aan of uit voor een kamer.
	Wanneer onderwerpslot is ingesteld staat %S
	niet toe dat het onderwerp van de kamer wordt gewijzigd,
	behalve met behulp van het commando TOPIC.

CHAN_HELP_SET_MLOCK
	Syntax: SET kamer MLOCK modi
	
	Stelt het modusslot in voor de kamer.  %S geeft u
	de mogelijkheid om bepaalde kamermodi altijd aan (of uit)
	te zetten.  Alle binaire modi (n, s, t, k,
	l, enzovoorts) kunnen op slot gezet worden; elke modus
	kan ingesteld worden op altijd aan, altijd uit, of geen slot.
	De parameter modi is op dezelfde manier opgebouwd als een
	commando /MODE; dat wil zeggen, modi die volgen op een +
	worden vastgezet op aan, en modi volgend op een - worden
	vastgezet op uit.
	
	Waarschuwing:  Wanneer u een modusslot insteld met een
	sleutel, zoals in het tweede onderstaande voorbeeld, dient u
	eveneens de optie RESTRICTED in te schakelen voor de kamer
	(zie HELP SET RESTRICTED), anders kan iedereen die de kamer
	binnenkomt wanneer deze leeg is de sleutel zien!
	MLOCK +i zal om dezelfde reden niet voorkomen dat gebruikers
	zonder uitnodiging de kamer binnengaan wanneer deze leeg is.
	
	Voorbeelden:
	
	    SET #kamer MLOCK +nt-ikl
	        Zet modi n en t vast op aan, en modi i, k en l op uit.
	        Alle andere modi blijven vrij om aan- of uitgezet te
	        worden.
	
	    SET #kamer MLOCK +knst-ilmp mijn-sleutel
	        Zet modi k, n, s en t vast op aan, en modi i, l, m en
	        p op uit.  Daarnaast wordt de kamersleutel vastgezet
	        op "mijn-sleutel".
	
	    SET #kamer MLOCK +
	        Verwijdert het modusslot; alle kamermodi zijn vrij om
	        aan- of uitgezet te worden.

CHAN_HELP_SET_HIDE
	Syntax: SET kamer HIDE {EMAIL | TOPIC | MLOCK} {ON | OFF}
	
	Geeft u de mogelijkheid om te voorkomen dat bepaalde informatie
	getoond wordt wanneer iemand het commando %S INFO gebruikt
	voor de gegeven kamer.  U kunt het e-mailadres (EMAIL),
	het laatste onderwerp (TOPIC), en het modusslot (MLOCK)
	van de kamer verbergen.
	De tweede parameter specificeert of de informatie getoond moet
	worden (OFF), of niet (ON).
	
	Let op dat het onderwerp altijd verborgen is wanneer de kamer
	een modusslot met +p of +s heeft.

CHAN_HELP_SET_PRIVATE
	Syntax: SET kamer PRIVATE {ON | OFF}
	
	Zet de optie PRIVATE (privé) voor een kamer aan of uit.
	Wanneer PRIVATE is ingesteld zal /msg %S LIST
	de kamer niet tonen in lijsten.

CHAN_HELP_SET_RESTRICTED
	Syntax: SET kamer RESTRICTED {ON | OFF}
	
	Stelt de optie RESTRICTED (beperkte toegang) in voor een kamer.
	Wanneer RESTRICTED is ingesteld worden gebruikers
	die niet op de toegangslijst van de kamer staan, of die
	normaalgesproken geen kameroperator-privileges mogen krijgen,
	verwijderd en verbannen van de kamer.

CHAN_HELP_SET_SECURE
	Syntax: SET kamer SECURE {ON | OFF}
	
	Zet %S's beveiligingsopties aan of uit voor een
	kamer.  Wanneer SECURE is ingesteld krijgen alleen
	gebruikers die hun bijnaam geregistreerd hebben met %s, en die
	zich geïdentificeerd hebben met hun wachtwoord privileges
	in de kamer, volgens de toegangslijst.  Wanneer de optie
	RESTRICTED eveneens is ingeschakeld is het voor gebruikers
	niet mogelijk de kamer binnen te gaan voordat ze zich
	geïdentificeerd hebben bij %s.

CHAN_HELP_SET_SECUREOPS
	Syntax: SET kamer SECUREOPS {ON | OFF}
	
	Zet de optie SECUREOPS (operatorbeveiliging) aan of uit voor
	een kamer. Wanneer SECUREOPS is ingesteld mogen
	gebruikers die niet op de toegangslijst staan geen
	kameroperator-status krijgen.

CHAN_HELP_SET_LEAVEOPS
	Syntax: SET kamer LEAVEOPS {ON | OFF}
	
	Zet de optie LEAVEOPS (operatorbehoud) aan of uit voor een kamer.
	Wanneer LEAVEOPS is ingesteld zal %S server-ops
	(+o voor de eerste persoon die de kamer binnenkomt) laten
	staan, zelfs wanneer de gebruiker normaalgesproken geen
	operatorstatus zou krijgen.  De optie SECUREOPS wordt nog
	steeds afgedwongen.
	Opmerking:  Dit kan het voor gebruikers mogelijk maken
	om operatorstatus te "stelen" tijdens een netsplit.

CHAN_HELP_SET_OPNOTICE
	Syntax: SET channel OPNOTICE {ON | OFF}
	
	Zet de optie OPNOTICE (operatoralarm) aan of uit voor een kamer.
	Wanneer OPNOTICE is ingesteld verstuurd %S een
	bericht naar de kamer wanneer OP, VOICE, of
	gerelateerde commando's worden gebruikt.

CHAN_HELP_SET_ENFORCE
	Syntax: SET kamer ENFORCE {ON | OFF}
	
	Zet de optie ENFORCE (afdwinging) aan of uit voor een kamer.
	Wanneer ENFORCE is ingesteld dwingt %S
	de auto-op en auto-voice niveaus af voor alle gebruikers.
	Wanneer een gebruiker met auto-op privileges bijvoorbeeld
	zijn operatorstatus verliest zal %S de gebruiker automatisch
	weer operatorstatus geven en zal %S eveneens niet
	toestaan dat het commando DEOP gebruikt wordt op de
	betreffende gebruiker.

CHAN_HELP_UNSET
	Syntax: UNSET kamer {SUCCESSOR | URL | EMAIL | ENTRYMSG}
	
	Verwijdert de waarde van de gegeven instelling voor een kamer.

CHAN_HELP_SOP
	Syntax: SOP kamer ADD bijnaam
	        SOP kamer DEL bijnaam
	        SOP kamer LIST [+begin] [masker]
	        SOP kamer COUNT
	
CHAN_HELP_SOP_MID1
	Beheert de SuperOp (SOP) lijst voor een kamer.  Gebruikers
	met SOP-status krijgen automatisch operatorstatus
	(modus +o) wanneer ze de kamer binnenkomen.
CHAN_HELP_SOP_MID1_CHANPROT
	Beheert de SuperOp (SOP) lijst voor een kamer.  Gebruikers
	met SOP-status krijgen automatisch operatorstatus
	(modus +o) en bescherming (modus +a) wanneer ze de kamer
	binnenkomen.
CHAN_HELP_SOP_MID2
	
	Een SOP kan de AOP, VOP en NOP-lijsten aanpassen.  Alleen de
	oprichter van de kamer kan de SOP-lijst aanpassen.
CHAN_HELP_SOP_MID2_HALFOP
	
	
	Een SOP kan de AOP, HOP, VOP en NOP-lijsten aanpassen.  Alleen de
	oprichter van de kamer kan de SOP-lijst aanpassen.
CHAN_HELP_SOP_END
	
	De ADD, DEL, LIST en COUNT subcommando's
	functioneren op dezelfde manier als bij het commando AOP.

CHAN_HELP_AOP
	Syntax: AOP kamer ADD bijnaam
	        AOP kamer DEL bijnaam
	        AOP kamer LIST [+begin] [masker]
	        AOP kamer COUNT
	
	Beheer de AutoOp (AOP) lijst voor een kamer.  Gebruikers
	met AOP-status krijgen automatisch operatorstatus (modus +o)
	wanneer ze de kamer binnengaan.
CHAN_HELP_AOP_MID
	
	Een AOP kan de VOP en NOP-lijsten aanpassen; SOPs kunnen de
	AOP-lijst aanpassen.
CHAN_HELP_AOP_MID_HALFOP
	
	Een AOP kan de VOP, HOP en NOP-lijsten aanpassen; SOPs kunnen de
	AOP-lijst aanpassen.
CHAN_HELP_AOP_END
	
	Het commando AOP ADD voegt een gebruiker toe aan de AOP-lijst,
	en het commando AOP DEL verwijdert een gebruiker van deze
	lijst.  Let op dat een gebruiker slechts op één
	kamer-toegangslijst tegelijk kan staan; wanneer de gebruiker die
	opgegeven wordt in een AOP ADD al op een andere
	kamer-toegangslijst staat (zoals de VOP-lijst) wordt de gebruiker
	eerst uit deze lijst verwijderd. (Wanneer u deze lijst niet mag
	bewerken, bijvoorbeeld omdat de gebruiker al op de SOP-lijst
	staat en u niet de kameroprichter bent, zal het commando AOP ADD
	mislukken.)
	
	Het commando AOP LIST toont de AOP-lijst.  Wanneer een
	masker met wildcard-karakters opgegeven wordt, worden alleen de
	invoeren getoond die overeenkomen met dit masker.  Wanneer de
	parameter begin is opgegeven specificeert deze hoeveel
	overeenkomende invoeren worden overgeslagen voordat de resultaten
	getoond worden (+50 zorgt er bijvoorbeeld voor dat de eerste
	50 overeenkomende invoeren worden overgeslagen).
	
	Het commando AOP COUNT toont het aantal invoeren op de
	AOP-lijst.

CHAN_HELP_HOP
	Syntax: HOP kamer ADD bijnaam
	        HOP kamer DEL bijnaam
	        HOP kamer LIST [+begin] [masker]
	        HOP kamer COUNT
	
	Beheert de HalfOp (HOP) lijst voor een kamer.  Gebruikers
	met HOP-status krijgen automatisch halfop-status (modus +h)
	wanneer ze de kamer binnengaan.
	
	Een HOP kan de VOP- en NOP-lijsten bewerken; SOPs en AOPs kunnen
	de HOP-lijst bewerken.
	
	De subcommando's ADD, DEL, LIST en COUNT functioneren
	hetzelfde als voor het commando AOP.

CHAN_HELP_VOP
	Syntax: VOP kamer ADD bijnaam
	        VOP kamer DEL bijnaam
	        VOP kamer LIST [+begin] [masker]
	        VOP kamer COUNT
	
	Beheert de VoiceOp (VOP) lijst voor een kamer.  Gebruikers met
	VOP-status krijgen automatisch voice-status (modus +v) wanneer ze
	de kamer binnengaan.
	
	De subcommando's ADD, DEL, LIST en COUNT functioneren
	hetzelfde als voor het commando AOP.

CHAN_HELP_NOP
	Syntax: NOP kamer ADD bijnaam
	        NOP kamer DEL bijnaam
	        NOP kamer LIST [+begin] [masker]
	        NOP kamer COUNT
	
	Beheert de NeverOp (NOP) lijst voor een kamer.  Gebruikers met
	NOP-status mogen geen kameroperator-status (modus +o) ontvangen.
	ChanServ zal de modus +o verwijderen wanneer deze aan dit type gebruiker
	wordt gegeven.
	
	De subcommando's ADD, DEL, LIST en COUNT functioneren
	hetzelfde als voor het commando AOP.

CHAN_HELP_ACCESS
	Syntax: ACCESS kamer ADD bijnaam niveau
	        ACCESS kamer DEL bijnaam
	        ACCESS kamer LIST [+begin] [masker]
	        ACCESS kamer LISTLEVEL [+begin] niveau
	        ACCESS kamer COUNT
	
	Beheert de toegangslijst voor een kamer.  De toegangslijst
	specificeert welke gebruikers kameroperator-status of toegang
	tot %S krijgen in de kamer.  Verscheidene
	gebruikersniveaus geven toegang tot verschillende subprivileges;
	Tik /msg %S HELP ACCESS LEVELS voor meer specifieke informatie.
	Een bijnaam die niet op de toegangslijst staat heeft
	gebruikersniveau 0.
	
	Het commando ACCESS ADD voegt de gegeven bijnaam toe aan de
	toegangslijst met het gegeven gebruikersniveau.  Wanneer de bijnaam
	reeds op de lijst staat wordt het toegangsniveau gewijzigd naar
	het niveau dat opgegeven werd in het commando.  Het gespecificeerde
	niveau dient lager te zijn dan dat van de gebruiker die het
	commando uitvoert, en wanneer de bijnaam al op de toegangslijst
	staat dient het toegangsniveau hiervan lager te zijn dan het
	toegangsniveau van de gebruiker die het commando uitvoert.
	
	Het commando ACCESS DEL verwijdert de gegeven bijnaam van de
	toegangslijst.  Gebruikers kunnen alleen bijnamen verwijderen met
	een lager toegangsniveau dan hun eigen niveau.
	
	Het commando ACCESS LIST toont de toegangslijst.  Wanneer een
	masker met wildcard-karakters gegeven wordt, worden alleen de
	invoeren overeenkomend met dit masker getoond. Wanneer de
	parameter begin opgegeven is specificeert dit hoeveel
	overeenkomende invoeren worden overgeslagen voordat de resultaten
	getoond worden (+50 zorgt er bijvoorbeeld voor dat de eerste
	50 overeenkomende invoeren worden overgeslagen).
	
	Het commando ACCESS LISTLEVEL is vergelijkbaar met ACCESS LIST,
	maar toont alleen invoeren met het gegeven toegangsniveau.  Het niveau
	kan gespecificeerd worden als een reeks: LISTLEVEL 50-100, toont
	bijvoorbeeld alle gebruikers met toegangsniveaus tussen 50 en 100.
	
	Het commando ACCESS COUNT toont het aantal invoeren op de
	toegangslijst.
CHAN_HELP_ACCESS_XOP
	
	Let op dat de toegangslijst ook bewerkt mag worden met de commando's
	SOP, AOP, VOP en NOP, welke respectievelijk overeenkomen
	met de niveaus %d, %d, %d en %d.
CHAN_HELP_ACCESS_XOP_HALFOP
	
	Let op dat de toegangslijst ook bewerkt mag worden met de commando's
	SOP, AOP, HOP, VOP en NOP, welke respectievelijk
	overeenkomen met de niveaus %d, %d, %d, %d en %d.

CHAN_HELP_ACCESS_LEVELS
	Niveaus voor kamerprivileges
	
	Standaard zijn de volgende privilege-niveaus gedefinieerd:
	
	   Oprichter Volledige toegang tot %S functies; automatische
	                  kameroperator-status.  Let op dat slechts één
	                  persoon oprichterstatus mag hebben (het kan niet
	                  gegeven worden met het commando ACCESS).
	        %4d  Toegang tot het commando AKICK command; automatische
	                  kameroperator-status
	        %4d  Automatische kameroperator-status
CHAN_HELP_ACCESS_LEVELS_HALFOP
	        %4d  Automatische halfop-status.
CHAN_HELP_ACCESS_LEVELS_END
	        %4d  Automatische voice-status.
	          0  Geen speciale privileges; kan operatorstatus ontvangen
	                  van andere ops (behalve wanneer SECUREOPS
	                  aan staat).
	         -1  Mag geen operatorstatus krijgen.
	       -100  Mag de kamer niet binnengaan.
	
	Deze niveaus (behalve de laatste twee) mogen gewijzigd worden, en
	nieuwe niveau's mogen toegevoegd worden met behulp van het commando
	LEVELS.  Tik /msg %S HELP LEVELS voor details.

CHAN_HELP_LEVELS
	Syntax: LEVELS kamer SET type niveau
	        LEVELS kamer {DIS | DISABLE} type
	        LEVELS kamer LIST
	        LEVELS kamer RESET
	
	Het commando LEVELS geeft gedetailleerde controle over de
	betekenis van de numerieke toegangsniveaus die gebruikt worden door
	het commando ACCESS.  Met behulp van dit commando kunt u de
	benodigde toegangsniveaus definiëren voor de meeste functies van
	%S.  (De commando's SET FOUNDER en SET PASSWORD zijn, evenals
	dit commando, beperkt tot de kameroprichter.)
CHAN_HELP_LEVELS_XOP
	
	Pas op:  Dit commando is alleen bedoeld voor gevorderde gebruikers!
	Wijziging van niveau-instellingen met behulp van dit commando kan
	ervoor zorgen dat de commando's SOP, AOP, VOP en NOP niet meer correct
	functioneren.
CHAN_HELP_LEVELS_XOP_HOP
	
	Pas op:  Dit commando is alleen bedoeld voor gevorderde gebruikers!
	Wijziging van niveau-instellingen met behulp van dit commando kan
	ervoor zorgen dat de commando's SOP, AOP, HOP, VOP en NOP niet meer
	correct functioneren.
CHAN_HELP_LEVELS_END
	
	LEVELS SET biedt de mogelijkheid om het toegangsniveau voor een
	functie of een groep van functies te wijzigen.  LEVELS DISABLE
	(of DIS afgekort) schakelt een automatische functie uit, of
	schakelt toegang tot de functie uit voor iedereen behalve de
	kameroprichter.
	LEVELS LIST toont de huidige niveaus voor elke functie of
	functiegroep.  LEVELS RESET herstelt de niveaus naar de
	standaardwaarden voor een nieuwgecreëerde kamer (zie HELP
	ACCESS LEVELS).
	
	Voor een lijst van functies waarvoor niveau's kunnen worden
	ingesteld, zie HELP LEVELS DESC.

CHAN_HELP_LEVELS_DESC
	De volgende functienamen worden herkend:

CHAN_HELP_AKICK
	Syntax: AKICK kamer ADD masker [reden]
	        AKICK kamer DEL masker
	        AKICK kamer LIST [+begin] [masker]
	        AKICK kamer VIEW [+begin] [masker]
	        AKICK kamer COUNT
	        AKICK kamer ENFORCE
	
	Beheert de autokick-lijst voor een kamer.  Wanneer een gebruiker
	die op deze lijst staat de kamer probeert binnen te gaan, zal
	%S deze gebruiker automatisch verbannen en verwijderen
	uit de kamer.
	
	Het commando AKICK ADD voegt het gegeven gebruiker@machine of
	bijnaam!gebruiker@machine masker toe aan de autokick-lijst.
	Wanneer een reden opgegeven is met het commando wordt deze gebruikt
	in het verwijderbericht wanneer de gebruiker verwijderd wordt.
	
	Het commando AKICK DEL verwijdert het gegeven masker van de
	autokick-lijst.  Het verwijdert echter geen verbanningen die geplaatst
	zijn door een autokick; deze dienen handmatig verwijderd te worden.
	
	Het commando AKICK LIST toont de autokick-lijst, of optioneel alleen
	de invoeren die overeenkomen met het gegeven masker.  AKICK VIEW is
	vergelijkbaar, maar toont meer details.
	Wanneer de parameter  begin opgegeven wordt met één van deze
	commando's specificeert dit hoeveel invoeren worden overgeslagen voordat
	de resultaten getoond worden (+50 zorgt er bijvoorbeeld voor dat de
	eerste 50 overeenkomende invoeren worden overgeslagen).
	
	Het commando AKICK COUNT toont het aantal invoeren op de
	autokick-lijst.
	
	Het commando AKICK ENFORCE zorgt ervoor dat %S de
	huidige akick-lijst afdwingt, door alle gebruikers te verwijderen die
	overeenkomen met een autokick-masker.
	
	Standaard is dit commando beperkt tot gebruikers met %s toegang en
	hoger in de kamer.

CHAN_HELP_INFO
	Syntax: INFO kamer [ALL]
	
	Toont informatie over de gegeven geregistreerde kamer,
	inclusief de oprichter, de tijd van registratie, de
	laatst gebruikte tijd, de beschrijving en het modusslot, mits
	dit is ingesteld.  Wanneer u geïdentificeerd bent als de
	oprichter van de kamer waarover u informatie opvraagt en
	ALL is opgegeven, worden het binnenkomstbericht en de
	opvolger eveneens getoond.

CHAN_HELP_LIST
	Syntax: LIST [+begin] patroon
	
	Toont alle geregistreerde kamers die overeenkomen met het
	gegeven patroon. (Kamers waarvoor de optie PRIVATE is
	ingesteld worden niet getoond.)
	Wanneer de parameter begin is opgegeven specificeert
	dit hoeveel invoeren worden overgeslagen voordat de
	resultaten getoond worden (+50 zorgt er bijvoorbeeld
	voor dat de eerste 50 overeenkomende invoeren worden
	overgeslagen).
CHAN_HELP_LIST_OPERSONLY
	
	Alleen IRC-operators mogen dit commando gebruiken.

CHAN_HELP_OP
	Syntax: OP kamer [bijnaam]
	
	Geeft een bijnaam operatorstatus in een kamer.  Wanneer
	bijnaam niet is opgegeven wordt operatorstatus aan uzelf
	gegeven.  Standaard is dit commando beperkt tot gebruikers
	met %s toegang en hoger in de kamer.

CHAN_HELP_DEOP
	Syntax: DEOP kamer [bijnaam]
	
	Verwijdert de operatorstatus van een bijnaam in een kamer.
	Wanneer bijnaam niet is opgegeven wordt de operatorstatus
	van uzelf verwijderd.  Standaard is dit commando beperkt tot
	gebruikers met %s toegang en hoger in de kamer.

CHAN_HELP_VOICE
	Syntax: VOICE kamer [bijnaam]
	
	Geeft een bijnaam voice-status in een kamer.  Wanneer
	bijnaam niet is opgegeven wordt voice-status aan uzelf
	gegeven.  Standaard is dit commando beperkt tot gebruikers
	met %s toegang en hoger in de kamer.

CHAN_HELP_DEVOICE
	Syntax: DEVOICE kamer [bijnaam]
	
	Verwijdert de voice-status van een bijnaam in een kamer.
	Wanneer bijnaam niet is opgegeven wordt de voice-status
	van uzelf verwijderd.  Standaard is dit commando beperkt tot
	gebruikers met %s toegang en hoger in de kamer.

CHAN_HELP_HALFOP
	Syntax: HALFOP kamer [bijnaam]
	
	Geeft een bijnaam halfop-status in een kamer.  Wanneer
	bijnaam niet is opgegeven wordt halfop-status aan uzelf
	gegeven.  Standaard is dit commando beperkt tot gebruikers
	met %s toegang en hoger in de kamer.

CHAN_HELP_DEHALFOP
	Syntax: DEHALFOP kamer [bijnaam]
	
	Verwijdert de halfop-status van een bijnaam in een kamer.
	Wanneer bijnaam niet is opgegeven wordt de halfop-status
	van uzelf verwijderd.  Standaard is dit commando beperkt tot
	gebruikers met %s toegang en hoger in de kamer.

CHAN_HELP_PROTECT
	Syntax: PROTECT kamer [bijnaam]
	
	Geeft kamerbescherming (modus +a) aan een bijnaam in een kamer.
	Wanneer bijnaam niet is opgegeven wordt kamerbescherming
	aan uzelf gegeven. Standaard is dit commando beperkt tot
	gebruikers met %s toegang en hoger in de kamer.

CHAN_HELP_DEPROTECT
	Syntax: DEPROTECT channel [nickname]
	
	Verwijdert kamerbescherming (modus +a) van een bijnaam in een
	kamer.  Wanneer bijnaam niet is opgegeven wordt de
	kamerbescherming van uzelf verwijderd.  Standaard is dit
	commando beperkt tot gebruikers met %s toegang en hoger in de
	kamer.

CHAN_HELP_INVITE
	Syntax: INVITE kamer
	
	Geeft %S opdracht om u uit te nodigen in de gegeven kamer.
	Standaard is dit commando beperkt tot gebruikers
	met %s toegang en hoger in de kamer.
	
	Let op: Wanneer u een bericht krijgt dat de kamer niet in
	gebruik is betekent dit dat de kamer leeg is, en dat u deze
	kunt binnengaan zonder gebruik van dit commando.

CHAN_HELP_UNBAN
	Syntax: UNBAN kamer
	
	Geeft %S opdracht alle verbanningen te verwijderen die
	voorkomen dat u de kamer kunt binnengaan. Standaard is dit
	commando beperkt tot gebruikers met %s toegang en hoger in
	de kamer.

CHAN_HELP_KICK
	Syntax: KICK kamer bijnaam [reden]
	
	Geeft %S opdracht de gegeven bijnaam van de kamer te
	verwijderen.  Het bericht dat opgegeven wordt bij het
	verwijderen toont de gebruiker die het commando
	KICK uitvoert, en, indien gegeven, de reden voor
	gebruik van het commando. Standaard is dit commando
	beperkt tot gebruikers met %s toegang en hoger in de kamer.

CHAN_HELP_KICK_PROTECTED
	
	Let op dat beschermde (+a) gebruikers niet verwijderd
	kunnen worden met dit commando, zelfs niet door de
	kameroprichter.

CHAN_HELP_TOPIC
	Syntax: TOPIC kamer onderwerp
	
	Geeft %S opdracht om het kameronderwerp te wijzigen naar
	het gegeven onderwerp.  Dit commando is vooral nuttig in
	combinatie met SET TOPICLOCK. Standaard is dit
	commando beperkt tot gebruikers met %s toegang en hoger
	in de kamer.

CHAN_HELP_CLEAR
	Syntax: CLEAR kamer instelling
	
	Geeft %S opdracht om bepaalde instellingen van een kamer
	te verwijderen. instelling kan één van de volgende
	zaken zijn:
	
	    MODES       Verwijdert alle modi van de kamer, behalve
	                    die in het modusslot van de kamer.
	    BANS        Verwijdert alle verbanningen van de kamer.
CHAN_HELP_CLEAR_EXCEPTIONS
	    EXCEPTIONS  Verwijdert alle uitzonderingen van de kamer.
CHAN_HELP_CLEAR_INVITES
	    INVITES     Verwijdert alle uitnodigingsmaskers van de kamer.
CHAN_HELP_CLEAR_MID
	    OPS         Verwijdert kameroperator-status (modus +o) van
	                    alle gebruikers.
CHAN_HELP_CLEAR_HALFOPS
	    HALFOPS     Verwijdert halfop-status (modus +h) van alle gebruikers.
CHAN_HELP_CLEAR_END
	    VOICES      Verwijdert voice-status (modus +v) van alle gebruikers.
	    USERS       Verwijdert (schopt) alle gebruikers van de kamer.
	
	Standaard is dit commando beperkt tot gebruikers met %s toegang
	en hoger in de kamer.

CHAN_HELP_STATUS
	Syntax: STATUS kamer bijnaam
	
	Toont het huidige toegangsniveau voor de gegeven bijnaam
	in de gegeven kamer.  Het antwoord wordt gegeven in de
	vorm:
	
	    STATUS kamer bijnaam toegangsniveau
	
	Wanneer een fout optreed wordt het antwoord gegeven in
	de vorm:
	
	    STATUS kamer bijnaam ERROR foutbericht
	
	Standaard is dit commando beperkt tot gebruikers
	met %s toegang en hoger in de kamer.

CHAN_OPER_HELP_COMMANDS
	
	De volgende commando's zijn beschikbaar voor Servicesbeheerders:
	
	    DROPCHAN   Verwijder een kamer
CHAN_OPER_HELP_COMMANDS_GETPASS
	    GETPASS    Vraag het oprichterwachtwoord voor een kamer op
	                   (alleen wanneer encryptie niet in gebruik is)
CHAN_OPER_HELP_COMMANDS_FORBID
	    FORBID     Voorkom dat een kamer geregistreerd/gebruikt wordt
	    SUSPEND    Voorkom dat een geregistreerde kamer gebruikt wordt
	    UNSUSPEND  Maak buitenwerkingstelling van een kamer ongedaan
CHAN_OPER_HELP_COMMANDS_END
	
	Servicesbeheerders kunnen tevens voor elke kamer de toegangs-,
	autokick- en niveaulijsten bekijken, en het commando STATUS
	gebruiken.

CHAN_OPER_HELP_DROPCHAN
	Syntax: DROPCHAN kamer
	
	Verwijdert de gegeven kamer uit het %S gegevensbestand.
	
	Beperkt tot Servicesbeheerders.

CHAN_OPER_HELP_SET
	
	Servicesbeheerders kunnen tevens de optie NOEXPIRE
	instellen, waarmee voorkomen kan worden dat kamers
	vervallen.
	Daarnaast kunnen Servicesbeheerders opties instellen voor
	elke kamer, zonder hiervoor te identificeren met het
	kamerwachtwoord.

CHAN_OPER_HELP_SET_NOEXPIRE
	Syntax: SET kamer NOEXPIRE {ON | OFF}
	
	Stelt in of de gegeven kamer zal vervallen.  Wanner deze
	instelling op ON gezet wordt voorkomt dit dat de kamer
	vervalt.
	
	Beperkt tot Servicesbeheerders.

CHAN_OPER_HELP_UNSET
	
	Evenals bij het commando SET kunnen Servicesbeheerders
	instellingen verwijderen voor iedere kamer, zonder zich
	te identificeren voor de betreffende kamer.

CHAN_OPER_HELP_INFO
	
	Servicesbeheerders kunnen de parameter ALL gebruiken voor
	iedere kamer.

CHAN_OPER_HELP_LIST
	Syntax: LIST [+begin] patroon [FORBIDDEN] [NOEXPIRE] [SUSPENDED]
	
	Toont alle geregistreerde kamers die overeenkomen met het gegeven
	patroon.  Kamers waarvoor de optie PRIVATE is ingesteld worden
	alleen getoond aan Servicesbeheerders.  Bij kamers waarvoor de optie
	NOEXPIRE is ingesteld wordt voor Servicesbeheerders de kamernaam
	voorafgegaan door het teken !.
	
	Wanneer de parameter begin is opgegeven specificeert deze
	hoeveel overeenkomende invoeren moeten worden overgeslagen voordat
	resultaten getoond worden (+50 zou er bijvoorbeeld voor zorgen
	dat de eerste 50 overeenkomende invoeren worden overgeslagen).
	
	Wanneer de opties FORBIDDEN, SUSPENDED of NOEXPIRE zijn
	opgegeven worden alleen kamers getoond die respectievelijk zijn
	verboden, buiten werking gesteld, of waarvoor NOEXPIRE is ingesteld.
	Deze opties kunnen in iedere volgorde en combinatie gebruikt worden.
	De opties zijn beperkt tot Servicesbeheerders.

CHAN_OPER_HELP_GETPASS
	Syntax: GETPASS kamer
	
	Toont het wachtwoord voor de gegeven kamer.  Let op dat
	wanneer dit commando gebruikt wordt er een bericht, waarin
	onder andere de persoon die het commando gebruikte staat,
	opgeslagen wordt en wordt verstuurd als WALLOPS/GLOBOPS.
	
	Wanneer encryptie is geactiveerd is dit commando mogelijk
	niet beschikbaar, afhankelijk van het type encryptie dat
	gebruikt wordt.
	Beperkt tot Servicesbeheerders.

CHAN_OPER_HELP_FORBID
	Syntax: FORBID kamer
	
	Voorkomt dat de gegeven kamer wordt geregistreerd of
	gebruikt.  Dit kan ongedaan worden gemaakt door de kamer
	te verwijderen.
	
	Beperkt tot Servicesbeheerders.

CHAN_OPER_HELP_SUSPEND
	Syntax: SUSPEND [+vervaltijd] kamer reden
	
	Voorkomt dat een kamer wordt gebruikt, of dat ervoor wordt
	geïdentificeerd.  De kamer kan weer in werking gesteld worden
	met het commando UNSUSPEND, waardoor de kamer weer
	gewoon gebruikt kan worden.  De parameter vervaltijd wordt
	op dezelfde manier geïnterpreteerd als voor het %s
	AKILL commando.
	
	Beperkt tot Servicesbeheerders.

CHAN_OPER_HELP_UNSUSPEND
	Syntax: UNSUSPEND kamer
	
	Geeft gebruikers de mogelijkheid een voorheen buiten werking
	gestelde kamer weer te gebruiken, en ervoor te identificeren.
	
	Beperkt tot Servicesbeheerders.


###########################################################################
#
# MemoServ help messages
#
###########################################################################

MEMO_HELP
	%S geeft IRC-gebruikers de mogelijkheid om korte berichten
	naar elkaar te sturen, of ze nu aanwezig zijn op het netwerk
	of niet, en naar kamers(*).  Zowel de bijnaam van de verstuurder
	en de naam van de ontvangende gebruiker of kamer dienen
	geregistreerd te zijn om een memo te kunnen versturen.  Tik
	/msg %S HELP COMMANDS voor een lijst van %S
	commando's, tik /msg %S commando om een commando
	te gebruiken, of tik /msg %S HELP commando
	voor meer informatie over een commando.
MEMO_HELP_EXPIRES
	
	Opmerking: Memo's vervallen (worden automatisch verwijderd)
	%s nadat ze verstuurd zijn, behalve wanneer ze gemarkeerd zijn
	als niet vervallend door middel van het commando SAVE.
MEMO_HELP_END_LEVELS
	
	(*) Standaard worden kamermemo's versturd naar alle
	    gebruikers (inclusief de oprichter) met tenminste
	    %s toegang in de kamer.  Dit kan gewijzigd worden met
	    het commando %s LEVELS.
MEMO_HELP_END_XOP
	
	(*) Kamermemo's worden gestuurd naar de kameroprichter en
	    alle gebruikers op de SOP-lijst van de kamer.

MEMO_HELP_COMMANDS
	De volgende commando's kunnen gebruikt worden met %S:
	
	    SEND      Verstuur een memo naar een bijnaam of kamer
	    LIST      Toon een lijst van uw memo's
	    READ      Lees één of meer memo's
MEMO_HELP_COMMANDS_FORWARD
	    FORWARD   Stuur memo's door naar uw e-mailadres
MEMO_HELP_COMMANDS_SAVE
	    SAVE      Markeer memo's als niet vervallend
MEMO_HELP_COMMANDS_DEL
	    DEL       Verwijder één of meer memo's
	    RENUMBER  Hernummer uw memo's
	    SET       Stel opties in met betrekking tot memo's
	    INFO      Toon informatie met betrekking tot memo's
MEMO_HELP_COMMANDS_IGNORE
	    IGNORE    Negeer memo's van bepaalde gebruikers

MEMO_HELP_SEND
	Syntax: SEND {bijnaam | kamer} memotekst
	
	Stuurt een memo naar de gegeven bijnaam of kamer met
	als inhoud de gegeven memotekst.  De ontvanger(s) krijgen
	een melding dat zij een nieuwe memo hebben.  De ontvangende
	bijnaam of kamer dient geregistreerd te zijn.

MEMO_HELP_LIST
	Syntax: LIST [num | lijst | NEW]
	
	Toont alle memo's die u momenteel heeft.  Wanneer NEW
	opgegeven wordt, worden alleen nieuwe (ongelezen) memo's
	getoond.  Ongelezen memo's zijn gemarkeerd met een "*" links
	van het memonummer; kamermemo's zijn gemarkeerd met een "#".
	U kunt ook een lijst van nummers specificeren, zoals in
	onderstaand voorbeeld:
	
	   LIST 2-5,7-9
	      Toont memo's met de nummers 2 t/m 5 en 7 t/m 9.

MEMO_HELP_LIST_EXPIRE
	Syntax: LIST [num | lijst | NEW]
	
	Toont alle memo's die u momenteel heeft.  Wanneer NEW
	opgegeven wordt, worden alleen nieuwe (ongelezen) memo's
	getoond.  Ongelezen memo's zijn gemarkeerd met een "*" links
	van het memonummer, kamermemo's zijn gemarkeerd met een "#" en
	memo's die niet vervallen zijn gemarkeerd met een "+".
	U kunt ook een lijst van nummers specificeren, zoals in
	onderstaand voorbeeld:
	
	   LIST 2-5,7-9
	      Toont memo's met de nummers 2 t/m 5 en 7 t/m 9.

MEMO_HELP_READ
	Syntax: READ {num | lijst | LAST | NEW}
	
	Stuurt u de tekst van de gespecificeerde memo's.  Wanneer
	LAST wordt opgegeven stuurt dit commando u de laatst
	ontvangen memo.  Wanneer NEW wordt opgegeven worden alle
	nieuwe memo's naar u verstuurd.  In elk ander geval wordt
	memo nummer num gestuurd.  U kunt ook een lijst van
	nummers opgeven, zoals met het commando LIST.

MEMO_HELP_SAVE
	Syntax: SAVE {num | lijst}
	
	Markeert de opgegeven memo of memo's als niet vervallend.
	U kunt ofwel een enkel nummer, of, zoals met het commando
	LIST,  een lijst van memo's opgeven.

MEMO_HELP_DEL
	Syntax: DEL {num | lijst | ALL}
	
	Verwijdert de opgegeven memo of memo's.  U kunt meerdere
	memonummers of reeksen van nummers opgeven in plaats
	van een enkel nummer, zoals met het commando LIST.
	U kunt tevens het woord ALL meegeven, wat ervoor zorgt
	dat al uw memo's verwijderd worden.
	
	Waarschuwing: Wanneer een memo verwijderd is kan deze
	niet meer teruggehaald worden.

MEMO_HELP_RENUMBER
	Syntax: RENUMBER
	
	Hernummer uw memolijst, waarbij "gaten" in de nummerreeks,
	onstaan door het verwijderen van memo's, verwijderd worden.

MEMO_HELP_SET
	Syntax: SET optie parameters
	
	Stelt verscheidene memo-opties in.  optie kan zijn:
	
	    NOTIFY      Bepaalt wanneer u op de hoogte gebracht
	                    wordt van nieuwe memo's (alleen voor
	                    bijnamen)
	    LIMIT       Stelt het maximum aantal memo's in dat u
	                    kunt ontvangen
MEMO_HELP_SET_OPTION_FORWARD
	    FORWARD     Stelt in of memo's automatisch naar u
	                    worden doorgestuurd
MEMO_HELP_SET_END
	
	Tik /msg %S HELP SET optie voor meer informatie
	over een specifieke optie.

MEMO_HELP_SET_NOTIFY
	Syntax: SET NOTIFY {ON | LOGON | NEW | OFF}
	
	Stelt in wanneer u op de hoogte gebracht wordt van
	nieuwe memo's:
	
	    ON      U wordt op e hoogte gebracht van nieuwe memo's
	               wanneer u verbinding maakt, wanneer u
	               terugkomt van /AWAY en wanneer memo's naar u
	               worden verzonden.
	    LOGON   u wordt alleen op de hoogte gebracht van memo's
	               wanneer u verbinding maakt of terugkomt van
	               /AWAY.
	    NEW     U wordt alleen op de hoogte gebracht van memo's
	               wanneer deze naar u worden verzonden.
	    OFF     U wordt niet op de hoogte gebracht van memo's.
	
	ON is gelijk aan de combinatie van LOGON en NEW.
	
	Let op dat wanneer "beveiliging" is ingesteld voor uw bijnaam u
	alleen op de hoogte gebracht wordt van nieuwe memo's (voor
	LOGON of ON) nadat u geïdentificeerd bent bij %s.

MEMO_HELP_SET_LIMIT
	Syntax: SET LIMIT limiet
	
	Stelt het maximum aantal memo's in dat u mag hebben.
	Wanneer u dit op 0 zet kan niemand memo's naar u sturen.
	U kunt deze optie niet hoger dan %d zetten.

MEMO_HELP_INFO
	Syntax: INFO
	
	Toont informatie over het aantal memo's dat u heeft,
	hoeveel hiervan er ongelezen zijn, hoeveel memo's u in
	totaal kan ontvangen en wanneer u op de hoogte gebracht
	wordt van nieuwe memo's.

MEMO_OPER_HELP_COMMANDS
	
	Servicesbeheerders mogen de commando's SET LIMIT en INFO
	gebruiken voor elke bijnaam.

MEMO_OPER_HELP_SET_LIMIT
	Syntax: SET LIMIT [bijnaam] {limiet | NONE | DEFAULT} [HARD]
	
	Stelt het maximum aantal memo's in dat een bijnaam mag
	ontvangen.  Instelling van deze limiet op 0 voorkomt dat de
	bijnaam memo's ontvangt, een instelling op NONE geeft
	de bijnaam de mogelijkheid een onbeperkte hoeveelheid memo's
	te ontvangen en op te slaan, en een instelling op DEFAULT
	zet de limiet terug op de standaardwaarde (%d).  Wanneer u
	geen bijnaam meegeeft wordt uw eigen limiet ingesteld.
	
	Toevoeging van HARD voorkomt dat de gebruiker de limiet
	aanpast.  Het niet toevoegen van HARD heeft het
	tegenovergestelde effect, waarbij de gebruiker de mogelijkheid
	heeft om de limiet aan te passen (zelfs wanneer een vorige limiet
	was ingesteld met HARD).
	
	Dit gebruik van het commando SET LIMIT is beperkt tot
	Servicesbeheerders.  Andere gebruikers mogen slechts een
	limiet instellen voor zichzelf, maar mogen de limiet niet
	verwijderen, instellen naar de standaardwaarde, hoger dan %d
	instellen of een harde limiet instellen.
	
MEMO_OPER_HELP_INFO
	Syntax: INFO [bijnaam]
	
	Zonder parameter toont dit commando informatie over het
	aantal memo's dat u heeft, hoeveel hiervan ongelezen zijn,
	hoeveel memo's u in totaal mag ontvangen en wanneer u op
	de hoogte gebracht wordt van nieuwe memo's.
	
	Wanneer de bijnaamparameter opgegeven wordt toont dit
	commando dezelfde informatie voor de gegeven bijnaam.  Dit
	gebruik is beperkt tot Servicesbeheerders.


MEMO_HELP_FORWARD
	Syntax: FORWARD {num | list | ALL}
	
	Stuurt één of meer van uw memo's door naar uw e-mailadres.
	U dient een geldig e-mailadres ingesteld te hebben voor uw
	bijnaam om dit commando te kunnen gebruiken.  Kamermemo's
	kunnen niet worden doorgestuurd.

MEMO_HELP_SET_FORWARD
	Syntax: SET FORWARD {ON | COPY | OFF}
	
	Stelt in of %S memo's die naar u verstuurd zijn automatisch
	doorstuurd naar uw e-mailadres.  Wanneer deze instelling op
	ON staat worden aan u geaddresseerde memo's direct
	doorgestuurd naar het e-mailadres dat geregistreerd is voor
	uw bijnaam.  COPY is vergelijkbaar, maar zorgt er tevens
	voor dat %S een kopie van de memo bewaart om online
	te kunnen lezen.  Wanneer de instelling op OFF staat worden
	memo's alleen online opgeslagen, zoals normaal.  Zelfs wanneer
	deze instelling op ON staat is het mogelijk dat u memo's
	online ontvangt, wanneer Services niet in staat is deze door
	te sturen naar uw e-mailadres.
	
	Let op dat wanneer deze optie ingesteld is op COPY en u het
	maximum aantal memo's dat u mag hebben heeft bereikt, u niet
	langer memo's kunt ontvangen (ook niet via e-mail) totdat u
	oude memo's verwijdert met behulp van het commando DEL.

MEMO_HELP_IGNORE
	Syntax: IGNORE {ADD | DEL | LIST} [bijnaam | masker]
	
	Beheert de lijst van "genegeerde" gebruikers.  Gebruikers wiens
	bijnaam of gebruiker@host-masker overeenkomt met een invoer op
	deze lijst mogen geen memo's naar u versturen.  ADD voegt
	de gegeven bijnaam of het gegeven gebruiker@host-masker toe aan
	uw negeerlijst, en DEL verwijdert een bijnaam of masker van
	de lijst.  LIST toont uw huidige negeerlijst.
	
	Let op dat een bijnaam of masker dat wordt opgegeven aan het
	commando ADD de wildcard-karakters "*" en "?" mag bevatten, en dat
	de lettergrootte niet uitmaakt.  Bijvoorbeeld, "*Evil*" komt
	overeen met "EvilNick", "AN_EVIL_USER" en "devil".

###########################################################################
#
# OperServ help messages
#
###########################################################################

OPER_HELP
	%S geeft IRC-operators de mogelijkheid verscheidene
	aspecten van het IRC-netwerk te beheren.  Tik
	/msg %S HELP COMMANDS voor een lijst van commando's.
	
	Opmerking: Alle naar %S verstuurde commando's worden
	opgeslagen.

OPER_HELP_COMMANDS
	%S commands:
	    GLOBAL      Stuur een bericht naar alle gebruikers
	    STATS       Toon de status van Services en het netwerk
	    SERVERMAP   Toon een overzicht van servers op het netwerk
	    OPER LIST   Toon een lijst van alle Services-operators
	    ADMIN LIST  Toon een lijst van alle Servicesbeheerders
OPER_HELP_COMMANDS_SERVOPER
	
	Commando's die alleen beschikbaar zijn voor Services-operators
	en beheerders:
	    MODE        Verander de modi van een kamer
	    KICK        Verwijder een gebruiker uit een kamer
	    CLEARMODES  Verwijder bepaalde modi van een kamer
	    CLEARCHAN   Verwijder alle gebruikers van een kamer (masskick)
	    KILLCLONES  Verwijder alle gebruikers met een bepaalde
	                    machinenaam van het netwerk
OPER_HELP_COMMANDS_AKILL
	    AKILL       Bewerk de autokill-lijst
	    AKILLCHAN   Zet een autokill voor alle gebruikers in een kamer
OPER_HELP_COMMANDS_EXCLUDE
	    EXCLUDE     Bewerk de autokill-uitzonderingslijst
OPER_HELP_COMMANDS_SLINE
	    SGLINE      Bewerk de SGLINE-lijst
	    SQLINE      Bewerk de SQLINE-lijst
	    SZLINE      Bewerk de SZLINE-lijst
OPER_HELP_COMMANDS_SESSION
	    SESSION     Bekijk de lijst van lopende sessies
	    EXCEPTION   Bewerk de sessielimiet-uitzonderingslijst
OPER_HELP_COMMANDS_NEWS
	    LOGONNEWS   Beheer berichten die aan gebruikers worden getoond
	                    bij binnenkomst
	    OPERNEWS    Beheer berichten die aan gebruikers worden getoond
	                    na gebruik van /OPER
OPER_HELP_COMMANDS_SERVADMIN
	
	Commando's die alleen beschikbaar zijn voor Servicesbeheerders
	    OPER        Bewerk de Services-operatorlijst
	    JUPE        "Jupiter" een server
	    SET         Verander de instellingen voor verscheidene globale
	                    Services-opties
	    UPDATE      Zorg ervoor dat de de gegevensbestanden van Services
	                    onmiddelijk naar de harde schijf geschreven worden
	    SHUTDOWN    Sla de gegevensbestanden op en stop Services
	    QUIT        Stop Services zonder het opslaan van gegevensbestanden
	    RESTART     Sla de gegevensbestanden op en herstart Services
	    REHASH      Lees de configuratiebestanden opnieuw in
	    SU          Verkrijg supergebruiker-privileges
OPER_HELP_COMMANDS_SERVROOT
	
	Commando's die alleen beschikbaar zijn voor de Services-supergebruiker:
	    ADMIN       Bewerk de Servicesbeheerderlijst
OPER_HELP_COMMANDS_RAW
	    RAW         Stuur een onbewerkte regel tekst naar de IRC-server

OPER_HELP_GLOBAL
	Syntax: GLOBAL bericht
	
	Geeft IRC-operators de mogelijkheid om berichten te sturen naar
	alle gebruikers op het netwerk.
	Dit bericht wordt verstuurd door de bijnaam %s.

OPER_HELP_STATS
	Syntax: STATS [RESET | NETWORK | ALL]
	
	Wanneer geen optie opgegeven wordt toont dit commando het
	aantal aanwezige gebruikers en IRC-operators (met uitzondering
	van Services), het hoogste aantal aanwezige gebruikers sinds
	Services gestart is, en de verstreken tijd sinds de laatste
	herstart van Services.
	
	De optie RESET zet het hoogste aantal aanwezige gebruikers
	terug naar het huidige aantal aanwezige gebruikers.
	
	De optie NETWORK toont de hoeveelheid data die uitgewisseld
	is met de IRC-server waarmee Services verbonden is, en de
	hoeveelheid geheugen die gebruikt wordt door de buffers voor
	verzonden en ontvangen data. Wanneer een limiet is ingesteld wordt
	de verhouding tussen het huidige netwerkgeheugen-gebruik en deze
	limiet eveneens weergegeven, in de vorm van een percentage.
	
	De optie ALL is alleen beschikbaar voor Servicesbeheerders en
	toont informatie over het geheugengebruik van Services.  Gebruik
	van deze optie kan er op grote netwerken voor zorgen dat Services
	tijdelijk bevriest, dus gebruik deze optie met mate!
	
	UPTIME kan gebruikt worden als synoniem voor STATS.

OPER_HELP_SERVERMAP
	Syntax: SERVERMAP
	
	Toont een overzicht van services die verbonden zijn met het
	netwerk, vanuit het oogpunt van Services.  "Nep"-servers
	(Services zelf en servers die aangemaakt zijn met het commando
	JUPE) worden getoond met "(*)" achter de servernaam.

OPER_HELP_OPER
	Syntax: OPER ADD bijnaam
	        OPER DEL bijnaam
	        OPER LIST
	
	Geeft Servicesbeheerders de mogelijkheid de Services-
	operatorlijst te bewerken.  Een gebruiker wiens bijnaam op
	deze lijst staat, en die geïdentificeerd is bij %s
	heeft de mogelijkheid om Services-operatorcommando's te
	gebruiken.
	
	Elke IRC-operator mag het subcommando OPER LIST gebruiken.
	Elk ander gebruik is beperkt tot Servicesbeheerders.

OPER_HELP_ADMIN
	Syntax: ADMIN ADD bijnaam
	        ADMIN DEL bijnaam
	        ADMIN LIST
	
	Geeft de Services-supergebruiker de mogelijkheid de Services-
	beheerderlijst te bewerken.  Een gebruiker wiens bijnaam op
	deze lijst staat, en die geïdentificeerd is bij %s
	heeft de mogelijkheid om Servicesbeheerdercommando's te
	gebruiken.
	
	Elke IRC-operator mag het subcommando ADMIN LIST gebruiken.
	Elk ander gebruik is beperkt tot de Services-supergebruiker.

OPER_HELP_GETKEY
	Syntax: GETKEY kamer
	
	Toont de sleutel voor de gegeven kamer, indien deze ingesteld is.
	
	Beperkt tot Services-operators.

OPER_HELP_MODE
	Syntax: MODE kamer modi
	
	Geeft Services-operators de mogelijkheid om kamermodi te
	wijzigen voor elke kamer.  De parameters zijn identiek aan
	die voor het standaardcommando /MODE.
	
	Beperkt tot Services-operators.

OPER_HELP_CLEARMODES
	Syntax: CLEARMODES kamer [ALL]
	
	Verwijdert alle kamermodi (n,s,t,k,l,b, etc,) van een kamer.
	Wanneer ALL meegegeven is worden eveneens alle gebruikermodi
	(zoals +o en +v) verwijderd van de kamer.  Wanneer de kamer
	geregistreerd is treedt echter wel het modusslot van de kamer
	in werking.
	
	Beperkt tot Services-operators.

OPER_HELP_CLEARCHAN
	Syntax: CLEARCHAN kamer
	
	Verwijdert alle gebruikers van een kamer (masskick).
	
	Beperkt tot Services-operators.

OPER_HELP_KICK
	Syntax: KICK kamer gebruiker reden
	
	Geeft IRC-operators de mogelijkheid om een gebruiker
	van een kamer te verwijderen.  De parameters zijn
	identiek aan die voor het standaardcommando /KICK.
	Het verwijderbericht begint met de bijnaam van de
	IRC-operator die het commando gestuurd heeft;
	bijvoorbeeld:
	
	*** SpamMan has been kicked off channel #my_channel by %S (Alcan (Flood))
	
	Beperkt tot Services-operators.

OPER_HELP_AKILL
	Syntax: AKILL ADD [+vervaltijd] masker reden
	        AKILL DEL masker
	        AKILL CLEAR ALL
	        AKILL LIST [+begin] [patroon] [NOEXPIRE]
	        AKILL VIEW [+begin] [patroon] [NOEXPIRE]
	        AKILL CHECK masker
	        AKILL COUNT
	
	Geeft Services-operators de mogelijkheid de autokill (KILL)
	lijst te bewerken.  Wanneer een gebruiker waarvoor een
	autokill-masker is gedefinieerd verbinding probeert te maken
	zal Services een KILL sturen voor deze gebruiker, en, op
	ondersteunde servertypen, alle servers de opdracht geven
	een verbanning (K-line) toe te voegen voor het masker
	dat overeenkomt met de gebruiker.
	
	AKILL ADD voegt het gegeven gebruiker@machine-masker toe
	aan de autokill-lijst.  De reden dient te beschrijven
	waarom de invoer wordt toegevoegd.  Afhankelijk van hoe
	Services is geconfigureerd wordt deze getoond aan
	gebruikers in KILL-berichten.  Een vervaltijd kan eveneens
	worden gespecificeerd, zoals hieronder wordt beschreven.
	
	AKILL DEL verwijdert het opgegeven masker van de
	autokill-lijst, indien aanwezig.
	
	AKILL CLEAR verwijdert alle maskers van de
	autokill-lijst.  De parameter ALL is verplicht, om te
	voorkomen dat dit commando per ongeluk gebruikt wordt.
	
	AKILL LIST toont alle huidige autokills.  Wanneer het
	optionele patroon is opgegeven, wordt de lijst gelimiteerd
	tot de autokills die overeenkomen met het gegeven patroon.
	Wanneer de parameter begin is opgegeven specificeert
	deze hoeveel overeenkomende invoeren moeten worden overgeslagen
	voordat resultaten getoond worden (+50 zou er bijvoorbeeld voor
	zorgen dat de eerste 50 overeenkomende invoeren worden
	overgeslagen).  Verder kan NOEXPIRE gebruikt worden om
	de lijst te limiteren tot de autokills die niet vervallen.
	
	AKILL VIEW is een uitgebreide versie van AKILL LIST,
	en toont wie een autokill heeft toegevoegd, de datum van
	toevoeging, wanneer de autokill vervalt, het gebruiker@machine-
	masker en de reden.
	
	AKILL CHECK toont alle invoeren op de autokill-lijst die
	overeenkomen met het gegeven gebruker@machine-masker; dit kan
	gebruikt worden om uit te vinden welke autokill er de oorzaak
	van is dat een bepaalde gebruiker van het netwerk is verwijderd.
	
	AKILL COUNT toont het aantal autokills in de autokill-lijst.
	
	Optioneel kan een vervaltijd opgegeven worden met het commando
	AKILL ADD.  De vervaltijd wordt voor het gebruiker@machine-
	masker geplaatst, en wordt gespecificeerd als een geheel getal,
	gevolgd door de karakters d (dagen), h (uren) of m
	(minuten).  Combinaties (zoals 1h30m) zijn eveneens toegestaan.
	Wanneer geen karakter is gespecificeerd wordt dagen als standaard
	genomen (oftewel, +30 betekent 30 dagen).  Om een autokill
	toe te voegen die niet vervalt kunt u +0 gebruiken.
	Wanneer het toe te voegen masker begint met een + is het
	opgeven van een vervaltijd verplicht, zelfs wanneer deze gelijk
	is aan de standaardwaarde.
OPER_HELP_AKILL_OPERMAXEXPIRY
	
	Services-operators mogen een vervaltijd instellen van maximaal
	%s; Servicesbeheerders mogen elke vervaltijd instellen.
OPER_HELP_AKILL_END
	
	Beperkt tot Services-operators.

OPER_HELP_AKILLCHAN
	Syntax: AKILLCHAN [KILL] [+vervaltijd] kamer reden
	
	Voegt autokils toe voor alle gebruikers in een gegeven kamer,
	behalve IRC-operators.  Wanneer KILL is opgegeven worden de
	gebruikers tevens van het netwerk verwijderd.  De reden wordt
	opgeslagen bij elke autokill en wordt getoond in KILL-berichten,
	afhankelijk van hoe Services is geconfigureerd.  De parameter
	vervaltijd is van dezelfde vorm als met het commando AKILL ADD;
	wanneer deze niet opgegeven wordt is %s de standaardwaarde.
	
	Autokills die toegevoegd worden met dit commando hebben de vorm
	*@machinenaam voor elke machinenaam die gebruikt wordt door
	een gebruiker in de kamer.  Wanneer een dergelijke autokill reeds
	bestaad wordt deze niet aangepast.  Dit kan ervoor zorgen dat het
	aantal toegevoegde autokills lager is dan het aantal gebruikers
	waarop de autokill effect heeft.
	
	Beperkt tot Services-operators.

OPER_HELP_EXCLUDE
	Syntax: EXCLUDE ADD [+vervaltijd] masker reden
	        EXCLUDE DEL masker
	        EXCLUDE CLEAR ALL
	        EXCLUDE LIST [+begin] [patroon] [NOEXPIRE]
	        EXCLUDE VIEW [+start] [patroon] [NOEXPIRE]
	        EXCLUDE CHECK masker
	        EXCLUDE COUNT
	
	Geeft Services-beheerders de mogelijkheid om de autokill-
	uitzonderingslijst te bewerken.  Wanneer een gebruiker
	overeenkomend met een autokill-uitzonderingsmasker verbinding
	maakt met het netwerk zal Services geen KILL sturen voor deze
	gebruiker, zelfs wanneer er een autokill-masker gedefinieerd is
	voor deze gebruiker.  Dit is nuttig wanneer u gebruik maakt van
	brede autokill-maskers en specifieke machines of gebruikers
	die binnen deze maskers vallen nog wel de mogelijkheid wilt
	geven om verbinding te maken.
	
	De subcommando's ADD, DEL, CLEAR, LIST, VIEW,
	CHECK en COUNT functioneren op dezelfde manier als de
	subcommando's voor het commando AKILL; zie de hulpcommando's
	voor AKILL voor meer informatie.
	
	Beperkt tot Servicesbeheerders.

OPER_HELP_SGLINE
	Syntax: SGLINE ADD [+vervaltijd] masker reden
	        SGLINE DEL masker
	        SGLINE CLEAR ALL
	        SGLINE LIST [+begin] [patroon] [NOEXPIRE]
	        SGLINE VIEW [+begin] [patroon] [NOEXPIRE]
	        SGLINE CHECK masker
	        SGLINE COUNT
	
	Geeft Services-operators de mogelijkheid de SGLINE-lijst te
	bewerken.  Wanneer een gebruiker wiens "echte naam"
	overeenkomt met een SGLINE-masker verbinding probeert te maken
	verstuurt Services een KILL voor deze gebruiker, en geeft, op
	ondersteunde servertypen, alle servers de opdracht om een
	verbanning (server G-line) toe te voegen voor het masker dat
	overeenkwam met de gebruiker.
	
	SGLINE ADD voegt het gegeven naammasker toe aan de
	SGLINE-lijst.  De reden dient te beschrijven waarom de
	invoer wordt toegevoegd.  Afhankelijk van hoe Services is
	geconfigureerd wordt deze ook in KILL-berichten getoond aan
	gebruikers.
	
	SGLINE DEL verwijdert het gegeven masker van de
	SGLINE-lijst, indien aanwezig.
	
	SGLINE CLEAN verwijdert alle maskers van de
	SGLINE-lijst.  De parameter ALL is verplicht, om te
	voorkomen dat dit commando per ongeluk gebruikt wordt.
	
	SGLINE LIST toont alle SGLINE-invoeren.  Wanneer de
	optionele parameter patroon is opgegeven worden alleen
	de invoeren getoond die overeenkomen met het patroon.
	Wanneer de parameter begin is opgegeven specificeert
	deze hoeveel overeenkomende invoeren moet worden overgeslagen
	voordat resultaten getoond worden (+50 zou er bijvoorbeeld
	voor zorgen dat de eerste 50 overeenkomende invoeren worden
	overgeslagen). Verder kan de parameter NOEXPIRE gebruikt
	worden om alleen de invoeren te tonen die niet vervallen.
	
	SGLINE VIEW is een uitgebreide versie van SGLINE LIST, en
	toont voor elke invoer wie de invoer heeft toegevoegd, op
	welke datum dit gebeurde, wanneer de SGLINE voor het laatst
	is geactiveerd, wanneer deze vervalt, en verder het masker
	en de reden.
	
	SGLINE CHECK toont alle invoeren op de SGLINE-lijst die
	overeenkomen met het gegeven gebruiker@machine-masker.  Dit
	kan gebruikt worden om te bepalen welke SGLINE ervoor heeft
	gezorgd dat een bepaalde gebruiker van het netwerk is
	verwijderd.
	
	SGLINE COUNT toont het aantal invoeren in de SGLINE-lijst.
	
	De parameter masker mag omringd worden door dubbele
	aanhalingstekens.  Dit is verplicht wanneer het masker spaties
	bevat; zoals bijvoorbeeld:
	    /msg %S SGLINE ADD +0 "slechte naam" Dit is een slechte naam.
	
	Een optionele vervaltijd kan opgegeven worden met het commando
	SGLINE ADD.  De vervaltijd wordt voor het gebruiker@machine-
	masker geplaatst en wordt gespecificeerd als een geheel getal,
	gevolgd door de karakters d (dagen), h (uren) of m
	(minuten).  Combinaties (zoals 1h30m) zijn eveneens toegestaan.
	Wanneer geen karakter is gespecificeerd wordt dagen als standaard
	genomen (oftewel, +30 betekent 30 dagen).  Om een invoer
	toe te voegen die niet vervalt kunt u +0 gebruiken.  Indien het
	toe te voegen masker begint met het karakter + en geen
	vervaltijd wordt opgegeven dient het masker omringt te worden door
	dubbele aanhalingstekens, zoals hierboven beschreven.
	
	Beperkt tot Services-operators.

OPER_HELP_SQLINE
	Syntax: SQLINE ADD [+expiry] masker reden
	        SQLINE DEL masker
	        SQLINE CLEAR ALL
	        SQLINE LIST [+begin] [patroon] [NOEXPIRE]
	        SQLINE VIEW [+begin] [patroon] [NOEXPIRE]
	        SQLINE CHECK masker
	        SQLINE COUNT
	
OPER_HELP_SQLINE_KILL
	Geeft Services-operators de mogelijkheid om de SQLINE-lijst
	te bewerken.  Wanneer een gebruiker wiens bijnaam overeenkomt
	met een SQLINE-masker verbinding probeert te maken, of wanneer
	een gebruiker probeert zijn bijnaam te wijzigen naar een naam
	die overeenkomt met een SQLINE-masker stuurt Services een KILL
	voor deze gebruiker, en geeft, op ondersteunde
	servertypen, alle servers de opdracht om een verbanning
	(server Q-line) toe te voegen voor het masker dat overeenkomt
	met de gebruiker.
OPER_HELP_SQLINE_NOKILL
	Geeft Services-operators de mogelijkheid om de SQLINE-lijst
	te bewerken.  Wanneer een gebruiker wiens bijnaam overeenkomt
	met een SQLINE-masker verbinding probeert te maken, of wanneer
	een gebruiker probeert zijn bijnaam te wijzigen naar een naam
	die overeenkomt met een SQLINE-masker wijzigt Services de
	bijnaam van een gebruiker naar een "gast"-bijnaam, en stuurt
	een bericht naar de gebruik dat er normaalgesproken voor zorgt
	dat de IRC-applicatie van de gebruiker een nieuwe bijnaam
	vraagt.  Op ondersteunde servertypen geeft Services daarnaast
	opdracht aan alle servers om een verbanning (server Q-line)
	toe te voegen voor het masker dat overeenkwam met de
	gebruiker.
OPER_HELP_SQLINE_IGNOREOPERS
	
	Let op de dat SQLINE-maskers geen effect hebben op IRC-operators.
OPER_HELP_SQLINE_END
	De subcommando's ADD, DEL, CLEAR, LIST, VIEW,
	CHECK en COUNT functioneren op dezelfde manier als de
	subcommando's voor het commando SGLINE; zie de hulpcommando's
	voor SGLINE voor meer informatie.
	
	Beperkt tot Services-operators.

OPER_HELP_SZLINE
	Syntax: SZLINE ADD [+vervaltijd] masker reden
	        SZLINE DEL masker
	        SZLINE CLEAR ALL
	        SZLINE LIST [+begin] [patroon] [NOEXPIRE]
	        SZLINE VIEW [+start] [patroon] [NOEXPIRE]
	        SZLINE CHECK masker
	        SZLINE COUNT
	
	Geeft Services-operators de mogelijkheid om de SZLINE-lijst
	te bewerken.  Wanneer een gebruiker wiens IP-adres overeenkomt
	met een SZLINE-masker verbinding probeert te maken stuurt
	Services een KILL voor deze gebruiker, en geeft, op ondersteunde
	servertypen, alle servers de opdracht om een verbanning
	(server Z-line) toe te voegen voor het masker dat overeenkomt
	met de gebruiker.
	
	De subcommando's ADD, DEL, CLEAR, LIST, VIEW,
	CHECK en COUNT functioneren op dezelfde manier als de
	subcommando's voor het commando SGLINE; zie de hulpcommando's
	voor SGLINE voor meer informatie.
	
	Beperkt tot Services-operators.

OPER_HELP_EXCEPTION
	Syntax: EXCEPTION ADD [+vervaltijd] masker limiet reden
	        EXCEPTION DEL {masker | lijst}
	        EXCEPTION CLEAR ALL
	        EXCEPTION MOVE num nieuwnum
	        EXCEPTION LIST [[+begin] patroon | lijst]
	        EXCEPTION VIEW [[+begin] patroon | lijst]
	        EXCEPTION CHECK machinenaam
	        EXCEPTION COUNT
	
	Geeft Services-operators de mogelijkheid om de lijst met
	machinenamen met specifieke sessielimieten te bewerken,
	waardoor bepaalde machines, zoals shell-servers, de mogelijkheid
	krijgen om meer dan het standaard aantal IRC-sessies te
	starten op het netwerk.  Wanneer een machine zijn sessielimiet
	bereikt worden alle gebruikers die vanaf deze machine
	verbinding proberen te maken verwijderd.  Voordat een
	gebruiker wordt verwijderd, wordt deze via een /NOTICE van %S
	op de hoogte gesteld over waar informatie te vinden over
	sessielimieten.  De inhoud van dit bericht is een configuratie-
	instelling.
	
	EXCEPTION ADD voegt het gegeven machinenaam-masker toe aan de
	uitzonderingslijst.  Let op dat  bijnaam!gebruiker@machine-
	en gebruiker@machine-maskers ongeldig zijn!
	Alleen echte machinenaam-maskers, zoals box.host.com en *.host.com
	zijn toegestan, omdat sessielimitering niet let op bijnamen of
	gebruikersnamen.  De parameter limiet dient een getal groter
	of gelijk aan nul te zijn; dit bepaald hoeveel sessies de
	betreffende machine tegelijkertijd mag hebben (de waarde nul
	betekent dat de machine een ongelimiteerd aantal sessies mag hebben).
	
	Optioneel kan een vervaltijd worden opgegeven met het commando
	EXCEPTION ADD. De vervaltijd wordt voor het gebruiker@machine-
	masker geplaatst en wordt gespecificeerd als een geheel getal,
	gevolgd door de karakters d (dagen), h (uren) of m
	(minuten).  Combinaties (zoals 1h30m) zijn eveneens toegestaan.
	Wanneer geen karakter is gespecificeerd wordt dagen als standaard
	genomen (oftewel, +30 betekent 30 dagen).  Om een invoer
	toe te voegen die niet vervalt kunt u +0 gebruiken.  Indien het
	toe te voegen masker begint met het karakter + moet een
	vervaltijd worden opgegeven, zelfs wanneer deze hetzelfde is als
	de standaardwaarde.
	
	EXCEPTION DEL verwijdert het gegeven masker of de invoer met
	het gegeven nummer of nummers van de uitzonderingslijst (invoer-
	nummers kunnen opgezocht worden met VIEW of LIST).
	
	EXCEPTION CLEAR verwijdert alle maskers van de
	uitzonderingslijst.  De parameter ALL is verplicht, om te
	voorkomen dat dit commando per ongeluk gebruikt wordt.
	
	EXCEPTION MOVE verplaatst uitzondering num naar
	nieuwnum.  Wanneer nieuwnum reeds gebruikt wordt door
	een andere uitzondering wordt dit nummer verhoogd om ruimte
	te maken.
	
	EXCEPTION LIST en EXCEPTION VIEW tonen alle huidige
	uitzonderingen.  Wanneer een patroon is opgegeven worden alleen de
	invoeren die overeenkomen met het patroon getoond (met de
	optionele parameter start om aan te geven hoeveel invoeren
	moeten worden overgeslagen voordat resultaten getoond worden).
	Wanneer een genummerde lijst wordt opgegeven, worden alleen
	invoeren in deze lijst getoond.  Het verschil tussen de twee
	commando's is dat EXCEPTION VIEW uitgebreider is.  Hierbij
	worden ook de bijnaam van de persoon die de uitzondering
	toevoegte, de sessielimiet, de reden, het machinenaam-masker
	en de vervaldatum en -tijd getoond, evenals het laatste
	tijdstip waarop de uitzondering is "gebruikt", ofwel de
	laatste keer dat een gebruiker overeenkomend met het
	uitzonderingsmasker verbinding maakte.
	
	EXCEPTION CHECK toont alle invoeren in de uitzonderingslijst
	die overeenkomen met de gegeven machinenaam.  Dit kan gebruikt
	worden om uit te vinden welke uitzondering, indien aanwezig,
	gebruikt wordt voor een bepaalde gebruiker.
	
	EXCEPTION COUNT toont het aantal invoeren in de
	uitzonderingslijst.
	
	Let op dat wanneer een gebruiker verbinding maakt de
	eerstgevonden uitzondering wordt gebruikt voor de gebruiker.
	Lange uitzonderingslijsten en uitzonderingen die met veel
	machinenamen overeenkomen kunnen ervoor zorgen dat
	Services trager presteert.
	
	Beperkt tot Services-operators.

OPER_HELP_SESSION
	Syntax: SESSION LIST drempelwaarde
	        SESSION VIEW machine
	
	Geeft Services-operators de mogelijkheid de sessielijst te
	bekijken.
	
	SESSION LIST toont machines met ten minste drempelwaarde
	sessies. De drempelwaarde dient een getal groter dan 1 te zijn,
	om te voorkomen dat er per ongeluk een groot aantal machinenamen
	met slechts één sessie getoond worden.
	
	SESSION VIEW toont gedetailleerde informatie over een
	specifieke machinenaam, inclusief het huidige aantal sessies en
	de sessielimiet.  De waarde van machine mag geen wildcard-
	karakters bevatten.
	
	Zie de hulpcommando's voor EXCEPTION voor meer informatie
	over sessielimitering en hoe specifieke sessielimieten in te
	stellen voor enkele machines en groepen van machines.
	
	Beperkt tot Services-operators.

OPER_HELP_SU
	Syntax: SU wachtwoord
	
	Geeft toegang tot functies die Services-supergebruiker
	privileges vereisen.  Het wachtwoord is hetzelfde dat ingesteld
	is met behulp van het SET SUPASS commando.  Let op dat het
	wachtwoord eenmalig ingesteld dient te worden voordat dit
	commando gebruikt kan worden.  Supergebruiker-privileges blijven
	behouden tot u IRC verlaat.
	
	WAARSCHUWING: Wanneer u een incorrect wachtwoord invoert zal
	Services een waarschuwingsbericht versturen naar alle IRC-operators.
	
	Beperkt tot Servicesbeheerders.

OPER_HELP_SET
	Syntax: SET optie instelling
	
	Beheert verscheidene globale Services-opties.
	Huidige beschikbare optienamen zijn:
	    READONLY   Stel de modus voor alleen-lezen of lezen-schrijven in
	    DEBUG      Activeer of deactiveer foutopsporingsmodus
	    SUPASS     Stel het wachtwoord voor het commando SU in (supergebruiker)
	
	Beperkt tot Servicesbeheerders; de optie SUPASS mag alleen ingesteld
	worden door de Services supergebruiker.

OPER_HELP_SET_READONLY
	Syntax: SET READONLY {ON | OFF}
	
	Zet de modus voor alleen-lezen aan of uit.  In deze modus
	is het voor normale gebruikers niet toegestaan om Services-
	data te wijzigen, inclusief kamer- en bijnaamtoegangslijsten,
	etc.  IRCops met voldoende Services-privileges mogen de
	AKILL-lijst van Services nog wel bewerken en mogen nog bijnamen
	en kamers verwijderen of verbieden, maar deze wijzigingen
	worden niet opgeslagen, tenzij de modus voor alleen-lezen
	gedeactiveerd wordt voordat Services uitgeschakeld of herstart
	wordt.
	
	Deze optie is gelijk aan de opstart-optie -read-only.

OPER_HELP_SET_DEBUG
	Syntax: SET DEBUG {ON | OFF | num}
	
	Zet de foutopsporingsmodus aan of uit.  In foutopsporingsmodus
	wordt alle data die van en naar Services gestuurd wordt en
	overige foutopsporingsberichten naar het logbestand geschreven.
	Wanneer de parameter num is opgegeven wordt de
	foutopsporingsmodus geactiveerd met het foutopsporingsniveau
	ingesteld op num.
	
	Deze optie is gelijk aan de opstart-optie -debug.

OPER_HELP_SET_SUPASS
	Syntax: SET SUPASS [wachtwoord]
	
	Stelt het wachtwoord in voor gebruik met het commando SU.
	Het wachtwoord dient eenmalig ingesteld te worden voordat
	het commando SU voor de eerste keer gebruikt wordt.  Wanneer
	geen wachtwoord wordt opgegeven is het commando SU
	onbeschikbaar.
	
	Deze optie mag alleen ingesteld worden door de Services-
	supergebruiker.

OPER_HELP_JUPE
	Syntax: JUPE server [reden]
	
	Geeft Services opdracht om "jupiter" uit te voeren op een
	server, wat inhoudt dat een nepserver verbonden wordt met
	Services, wat voorkomt dat de echte server met de
	betreffende naam verbinding maakt.  De "jupe" kan
	verwijderd worden met het standaardcommando /SQUIT.
	Wanneer een reden wordt opgegeven, wordt deze in het
	informatieveld van de server geplaatst.  In elk ander
	geval bevat dit veld de tekst "Jupitered by <bijnaam>",
	waarbij de bijnaam wordt getoond van de persoon die
	de jupiter uitvoerde.
	
	Wanneer er reeds een server met dezelfde naam verbonden
	is met het netwerk wordt deze automatisch verwijderd.
	
	Beperkt tot Servicesbeheerders.

OPER_HELP_RAW
	Syntax: RAW tekst
	
	Stuurt een tekstregel direct naar de server waar Services mee
	verbonden is, zonder dat deze op enige wijze bewerkt wordt.
	Dit commando is primair bedoeld voor het testen van Services
	en IRC-servers; het nut van dit commando op een produktie-
	netwerk is beperkt en het kan bij verkeerd gebruik voor grote
	problemen zorgen op een netwerk, of veroorzaken dat Services
	vastloopt.  GEBRUIK DIT COMMANDO NIET tenzij u absoluut
	zeker weet waar u mee bezig bent!
	
	Beperkt tot de Services-supergebruiker.

OPER_HELP_UPDATE
	Syntax: UPDATE [FORCE]
	
	Zorgt ervoor dat Services alle gegevensbestanden opslaat
	zodra u het commando verstuurd.  Wanneer de optie FORCE
	is opgegeven probeert Services het slot van de
	gegevensbestanden te verwijderen voor er gegevens heen te
	schrijven.  Dit kan gebruikt worden om de foutmelding dat de
	gegevensbestanden op slot zitten op te lossen.
	
	Beperkt tot Servicesbeheerders.

OPER_HELP_QUIT
	Syntax: QUIT
	
	Zorgt ervoor dat Services onmiddelijk uitgeschakeld wordt;
	gegevensbestanden worden niet opgeslagen.  Dit commando
	dient niet gebruikt te worden tenzij er vrees is voor
	beschadiging van de kopie van de gegevensbestanden in het
	geheugen en deze dus niet opgeslagen moeten worden.  Voor
	normale uitschakeling kunt u het commando SHUTDOWN
	gebruiken.
	
	Beperkt tot Servicesbeheerders.

OPER_HELP_SHUTDOWN
	Syntax: SHUTDOWN
	
	Zorgt ervoor dat Services alle gegevensbestanden opslaat
	en afsluit.
	
	Beperkt tot Servicesbeheerders.

OPER_HELP_RESTART
	Syntax: RESTART
	
	Zorgt ervoor dat Services alle gegevensbestanden opslaat
	en vervolgens zichzelf opnieuw opstart (ofwel, zichzelf
	afsluit en onmiddelijk weer uitvoert).
	
	Beperkt tot Servicesbeheerders.

OPER_HELP_REHASH
	Syntax: REHASH
	
	Zorgt ervoor dat Services de configuratiebestanden opnieuw
	inleest en de actieve instellingen aanpast.  Sommige
	instellingen, zoals RemoteServer (wat de machinenaam van
	de server waarmee verbinding gemaakt moet worden instelt)
	kunnen niet gewijzigd worden met REHASH, en treden pas in
	effect wanneer Services opnieuw opgestart wordt.  In deze
	gevallen wordt een bericht geschreven naar het logbestand
	van Services, en wordt op IRC een bericht verstuurd waarmee
	u hiervan op de hoogte wordt gebracht.
	
	Wanneer LoadModule-regels zijn toegevoegd of verwijderd van
	het hoofd-configuratiebestand van Services (%s) worden
	modules geladen of verwijderd waar nodig.  Modules die niet
	langer een LoadModule-regel hebben worden in omgekeerde
	volgorde verwijderd; vervolgens, nadat de overgebleven
	modules worden ververst, worden nieuwe modules geladen in
	de volgorde zoals ze in het configuratiebestand staan.
	Wanneer een fout optreed wordt de uitvoer van het commando
	afgebroken.
	
	Let op dat door afhankelijkheden in modules sommige modules
	niet verwijderd of gewijzigd kunnen worden.  In deze gevallen
	dient u Services opnieuw op te starten om de wijzigingen door
	te voeren.
	
	Beperkt tot Servicesbeheerders.

OPER_HELP_KILLCLONES
	Syntax: KILLCLONES bijnaam
	
	Verwijdert alle gebruikers die dezelfde machinenaam hebben als
	bijnaam.  Een tijdelijke AKILL, in de vorm *@machine,
	wordt toegevoegd om te voorkomen dat de verwijderde gebruikers
	meteen weer opnieuw verbinding maken.  Een WALLOPS wordt eveneens
	verstuurd, met hierin de naam van de persoon die het commando
	uitvoerde, voor welke machinenaam dit gebeurde en hoeveel
	gebruikers hierdoor zijn verwijderd.  Dit commando is nuttig om
	grote hoeveelheden dubbele gebruikers te verwijderen van het
	netwerk.
	
	Beperkt tot Services-operators.


###########################################################################
#
# StatServ help messages
#
###########################################################################

STAT_HELP
	%S geeft statistische informatie over het IRC-netwerk en
	de gebruikers hierop.  Tik /msg %S HELP COMMANDS voor
	een lijst van %S commando's; voor informatie over een
	commando, tik /msg %S HELP commando.

STAT_HELP_COMMANDS
	De volgende commando's kunnen gebruikt worden met %S:
	
	    SERVERS     Serverstatistieken
	    USERS       Gebruikerstatistieken

STAT_HELP_SERVERS
	Syntax: SERVERS STATS
	        SERVERS LIST [masker]
	        SERVERS VIEW [masker] [ONLINE | OFFLINE]
	
	Toont statistieken met betrekking tot de servers op het
	netwerk.
	
	SERVER STATS toont een korte samenvatting van de
	op dit moment voor Services zichtbare (online) en
	onzichtbare (offline) servers.
	SERVER LIST toont een lijst van aanwezige servers
	en recente, bijbehorende statistieken.
	SERVER VIEW toont een lijst van aanwezige en niet
	aanwezige servers met gedetailleerde bijbehorende
	statistieken.  De opties ONLINE en OFFLINE
	zorgen ervoor dat respectievelijk alleen aanwezige of
	alleen niet aanwezige servers getoond worden.
	
	Gebruik de optie masker om te beperken voor welke
	servers de statistieken getoond worden.

STAT_HELP_USERS
	Syntax: USERS STATS
	
	Toont statistieken met betrekking tot gebruikers op
	het netwerk.

STAT_OPER_HELP_SERVERS
	Syntax: SERVERS STATS
	        SERVERS LIST [masker]
	        SERVERS VIEW [masker] [ONLINE | OFFLINE]
	        SERVERS DELETE server
	        SERVERS COPY server nieuwenaam
	        SERVERS RENAME server nieuwenaam
	
	Toont statistieken met betrekking tot de servers in het
	netwerk.
	
	SERVERS STATS toont een korte samenvatting van de
	servers die momenteel voor Services zichtbaar (online) en
	onzichtbaar (offline) zijn.
	SERVER LIST toont een lijst van aanwezige servers
	en recente, bijbehorende statistieken.
	SERVERS VIEW toont een lijst van aanwezige en niet
	aanwezige servers, met recente, bijbehorende statistieken.
	De opties ONLINE en OFFLINE zorgen ervoor dat
	respectievelijk alleen aanwezige of niet aanwezige servers
	getoond worden.
	
	Gebruik de optie masker om te beperken voor welke
	servers de statistieken getoond worden.
	
	DELETE verwijdert de statistieken voor de opgegeven server.
	COPY dupliceert een verzameling serverstatistieken.
	RENAME hernoemt een verzameling serverstatistieken.
	Deze drie commando's zijn beperkt tot Servicesbeheerders.

###########################################################################

# Please do not change the following line; it is used to determine which
# version of the master (English) language file was used to create a
# translated file.

# CVS: $Revision: 2.194 $
