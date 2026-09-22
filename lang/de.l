# Emacs: -*- mode: fundamental; coding: binary; -*-

# IRC Services is copyright (c) 1996-2009 Andrew Church.
#     E-mail: <achurch@achurch.org>
# Parts written by Andrew Kempe and others.
# This program is free but copyrighted software; see the file GPL.txt for
# details.

LANG_NAME
	Deutsch (German)
#LANG_CHARSET
#	iso-8859-1

#########################################################
# Translated by TimeMr14C (uhc0@stud.uni-karlsruhe.de)  #
# Translated by freenet.de (www.freenet.de)             #
# Put together, corrected by jacek.margos@freenet-ag.de #
# Additional corrections by holger.baust@freenet-ag.de  #
#########################################################

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
	%d. %b %Y %H:%M:%S %Z
# This is used as the format string for strftime() for a date alone in long
# format (English: including weekday).
STRFTIME_LONG_DATE_FORMAT
	%a %d.%b.%Y
# This is used as the format string for strftime() for a date alone in
# short format (English: without weekday).
STRFTIME_SHORT_DATE_FORMAT
	%d.%b.%Y

# These tell our strftime() what the names of months and days are.  If you
# don't use %a, %A, %b, or %B in your strftime() strings above, you can
# leave these empty.  However, if you enter names, they MUST stay in order,
# one per line, and the list MUST be complete!

# %a
STRFTIME_DAYS_SHORT
	So
	Mo
	Di
	Mi
	Do
	Fr
	Sa
# %A
STRFTIME_DAYS_LONG
	Sonntag
	Montag
	Dienstag
	Mittwoch
	Donnerstag
	Freitag
	Samstag
# %b
STRFTIME_MONTHS_SHORT
	Jan
	Feb
	Mrz
	Apr
	Mai
	Jun
	Jul
	Aug
	Sep
	Okt
	Nov
	Dez
# %B
STRFTIME_MONTHS_LONG
	Januar
	Februar
	März
	April
	Mai
	Juni
	Juli
	August
	September
	Oktober
	November
	Dezember

# "day", "hour", "minute", and "second" in singular and plural.
STR_DAY
	 Tag
STR_DAYS
	 Tage
STR_HOUR
	 Stunde
STR_HOURS
	 Stunden
STR_MINUTE
	 Minute
STR_MINUTES
	 Minuten
STR_SECOND
	 Sekunde
STR_SECONDS
	 Sekunden
# Separator used between units of time; in English, a comma followed by a
# space (the ", " in "3 hours, 10 minutes").
STR_TIMESEP
	 und 


# This is used in ChanServ/NickServ INFO displays, and is a comma followed
# by a space (", " -- trailing space is significant), or the equivalent in
# the language's character set.
COMMA_SPACE
	, 

# Various error messages.
INTERNAL_ERROR
	Interner Fehler - Verarbeitung der Anfrage nicht möglich.
SERVICES_IS_BUSY
	Die Services sind zur Zeit überlastet. Bitte versuchen Sie es später nochmal.

UNKNOWN_COMMAND
	Unbekannter Befehl %s.
UNKNOWN_COMMAND_HELP
	Unbekannter Befehl %s. "/msg %s HELP" für Hilfe.
SYNTAX_ERROR
	Syntax: %s
MORE_INFO
	/msg %s HELP %s für weitere Informationen.
NO_HELP_AVAILABLE
	Keine Hilfe für %s verfügbar.
MISSING_QUOTE
	Gänsefüßchen (") fehlt.

BAD_EMAIL
	E-Mail-Adressen müssen der Form Benutzer@Domain sein. Es dürfen keine Kontrollzeichen (Farbwechsel, etc) und keine der folgenden Zeichen benutzt werden: , : ; | \ " ( ) < > [ ]
### CHANGED: REJECTED_EMAIL added
BAD_URL
	Eine URL muss der Form http://Hostname[:Port]/... (oder ftp://, etc.) entsprechen.
BAD_USERHOST_MASK
	Maske muss folgendes Format haben: User@Host.
BAD_NICKUSERHOST_MASK
	Maske muss folgendes Format haben: Nick!User@Host.
BAD_EXPIRY_TIME
	Die Verfallsdauer ist ungültig.

SENDMAIL_NO_RESOURCES
	Das System ist zu belastet, um E-Mails zu versenden. Versuchen Sie es später nochmal.
READ_ONLY_MODE
	Hinweis: Die Services sind im schreibgeschützten Modus.
	Änderungen werden nicht gespeichert!
PASSWORD_INCORRECT
	Falsches Passwort.
PASSWORD_WARNING
	Warnung: Nach noch einem missglückten Versuch, sich zu identifizieren, werden sie vom Server getrennt.

# What's the difference between "Access denied" and "Permission denied"?
# Very little; you can safely make them the same message with no real loss
# of meaning.  If you want to make a distinction, "Access denied" is
# usually used more generally; for example, a non-oper trying to access
# OperServ gets "Access denied", while a non-Services admin trying to use
# NickServ SET NOEXPIRE gets "Permission denied".
ACCESS_DENIED
	Zugriff verweigert.
PERMISSION_DENIED
	Erlaubnis verweigert.

MORE_OBSCURE_PASSWORD
	Versuchen Sie es erneut mit einem ungewöhnlicheren Passwort. Passwörter sollten mindestens fünf Zeichen lang sein und nicht zu leicht zu erraten sein.  (z.B. nicht Ihr wirklicher Name oder Ihr Chatname).

NICK_NOT_REGISTERED
	Ihr Chatname ist nicht registriert.
NICK_NOT_REGISTERED_HELP
	Ihr Chatname ist nicht registriert. Für weitere Informationen zur Chatnamen-Registrierung bitte /msg %s HELP eingeben.
NICK_TOO_LONG
	Chatnamen dürfen bis %d Zeichen lang sein.
NICK_INVALID
	%s ist kein gültiger Chatname.
NICK_X_NOT_REGISTERED
	Der Chatname %s ist nicht registriert.
NICK_X_ALREADY_REGISTERED
	Der Chatname %s ist bereits registriert.
NICK_X_NOT_IN_USE
	Der Chatname %s ist momentan nicht in Benutzung.
NICK_X_FORBIDDEN
	Der Name %s darf weder genutzt noch registriert werden.
NICK_X_SUSPENDED
	Der Chatname %s ist suspendiert und kann nicht benutzt werden.
NICK_X_SUSPENDED_MEMOS
	Der Chatname %s ist suspendiert und kann keine Memos erhalten.
NICK_IDENTIFY_REQUIRED
	Dieser Befehl benötigt eine Passwort-Authentisierung.
	Bitte versuchen Sie es erneut nach der Eingabe von /msg %s IDENTIFY Passwort.
NICK_PLEASE_AUTH
	Ihr Chatname ist noch nicht authentifiziert. Bitte authentifizieren sie den Chatnamen mit dem Code, der an folgende E-Mail-Adresse geschickt wurde: %s.
NICK_X_NOT_ON_CHAN_X
	Der Chatname %s ist im Moment nicht im Raum %s.

CHAN_INVALID
	%s ist kein gültiger Raumname.
CHAN_X_NOT_REGISTERED
	Der Raum %s ist nicht registriert.
CHAN_X_NOT_IN_USE
	Der Raum %s wird momentan nicht benutzt.
CHAN_X_FORBIDDEN
	Der Raum %s darf weder genutzt noch registriert werden.
CHAN_X_SUSPENDED
	Der Raum %s ist suspendiert, und kann nicht benutzt werden.
CHAN_X_SUSPENDED_MEMOS
	Der Raum %s ist suspendiert, und kann keine Memos erhalten.
CHAN_IDENTIFY_REQUIRED
	Dieser Befehl benötigt eine Passwort-Authentisierung.
	Bitte versuchen Sie es erneut nach der Eingabe von /msg %s IDENTIFY %s Passwort.

SERV_X_NOT_FOUND
	Server %s wurde nicht gefunden.

EXPIRES_NONE
	Wird nicht verfallen
EXPIRES_IN
	Verfällt in %s

###########################################################################
#
# NickServ messages
#
###########################################################################

# Automatic messages
NICK_IS_REGISTERED
	Dieser Chatname gehört jemand anderes. Bitte wählen Sie einen anderen Namen. (Falls dies Ihr Chatname ist, geben Sie bitte /msg %s IDENTIFY Passwort ein.)
NICK_IS_SECURE
	Dieser Chatname ist registriert und geschützt. Falls dies Ihr Chatname ist, geben Sie bitte /msg %s IDENTIFY Passwort ein. Wählen Sie anderenfalls einen neuen Chatnamen.
NICK_MAY_NOT_BE_USED
	Dieser Chatname darf nicht genutzt werden.  Wählen Sie bitte einen anderen Namen.
DISCONNECT_IN_1_MINUTE
	Wenn Sie Ihren Chatnamen nicht innerhalb einer Minute ändern, wird die Verbindung getrennt.
DISCONNECT_IN_20_SECONDS
	Wenn Sie Ihren Chatnamen nicht innerhalb von 20 Sekunden ändern, wird die Verbindung getrennt.
DISCONNECT_NOW
	Dieser Chatname ist bereits registriert; Sie dürfen ihn nicht benutzen. Wählen Sie einen anderen Namen, bevor Sie sich neu verbinden.
FORCENICKCHANGE_IN_1_MINUTE
	Wenn Sie Ihren Chatnamen nicht innerhalb einer Minute ändern, wird Ihr Chatname geändert.
FORCENICKCHANGE_IN_20_SECONDS
	Wenn Sie Ihren Chatnamen nicht innerhalb von 20 Sekunden ändern, wird Ihr Chatname geändert.
FORCENICKCHANGE_NOW
	Dieser Chatname ist bereits registriert; Sie dürfen Ihn nicht benutzen. Ihr Chatname wird jetzt in %s geändert.
NICK_EXPIRES_SOON
	Warnung: Ihre Chatnamen-Registrierung wird in %s verfallen. Wenn Sie weiterhin Ihren Chatnamen verwenden wollen, müssen Sie sich dem %s gegenüber durch folgendes identifizieren: /msg %s IDENTIFY Passwort
NICK_EXPIRED
	Warnung: Ihre Chatnamen-Registrierung ist bereits verfallen. Bitte registrieren Sie den Chatnamen neu, wenn sie Ihn weiter verwenden wollen.


# REGISTER responses
NICK_REGISTER_SYNTAX
	REGISTER Password [E-Mail]
NICK_REGISTER_REQ_EMAIL_SYNTAX
	REGISTER Password E-Mail
NICK_REGISTRATION_DISABLED
	Die Chatnamen-Registrierung ist temporär nicht verfügbar.
NICK_REGISTRATION_FAILED
	Die Registrierung ist fehlgeschlagen.
NICK_REG_PLEASE_WAIT
	Bitte warten Sie %s, bevor Sie den REGISTER-Befehl erneut benutzen.
NICK_REG_PLEASE_WAIT_FIRST
	Bitte %s vor der Benutzung des REGISTER-Befehls warten.
NICK_CANNOT_BE_REGISTERED
	Chatname %s kann nicht registriert werden.
NICK_REGISTER_EMAIL_UNAUTHED
	Die E-Mail-Adresse, die Sie angegeben haben, wurde bis jetzt nicht authentifiziert. Benutzen Sie den AUTH Befehl um die E-Mail-Adresse zu authentifizieren, bevor Sie neue Chatnamen mit ihr registrieren.
NICK_REGISTER_TOO_MANY_NICKS
	Sie haben zu viele registrierten Chatnamen (%d, Maximum ist %d). Sie können keine neuen registrieren, bis Sie einige von Ihren Chatnamen löschen.
NICK_REGISTERED
	Der Chatname %s wurde für Sie registriert.
NICK_PASSWORD_IS
	Ihr Passwort lautet: %s. Bitte merken Sie es sich.

# IDENTIFY responses
NICK_IDENTIFY_SYNTAX
	IDENTIFY Password
NICK_IDENTIFY_FAILED
	Leider schlug Ihre Identifikation fehl.
NICK_IDENTIFY_EMAIL_MISSING
	Sie müssen eine E-Mail-Adresse angeben bevor Sie den Chatnamen weiter nutzen. Befehl: /msg %s SET EMAIL E-Mail-Adresse
NICK_IDENTIFY_SUCCEEDED
	Passwort akzeptiert - Sie sind identifiziert worden.

# DROP responses
NICK_DROP_SYNTAX
	DROP Passwort
NICK_DROP_WARNING
	Hinweis: Der DROP-Befehl wird auch alle mit diesem verbundene Chatnamen freigeben!
NICK_DROP_DISABLED
	Die Chatnamen-Freigabe ist temporär nicht verfügbar.
NICK_DROP_FAILED
	Freigabe ist fehlgeschlagen.
NICK_DROPPED
	Ihr Chatname ist wieder freigegeben worden.
NICK_DROPPED_LINKS
	Ihr Chatname und alle damit verbundenen Chatnamen sind freigegeben worden.

# DROPNICK responses
NICK_DROPNICK_SYNTAX
	DROPNICK Chatname
NICK_X_DROPPED
	Der Chatname %s ist wieder freigegeben worden.
NICK_X_DROPPED_LINKS
	Chatname %s und alle damit verbundenen Chatnamen sind freigegeben worden.

# DROPEMAIL and DROPEMAIL-CONFIRM responses
NICK_DROPEMAIL_SYNTAX
	DROPEMAIL E-Mail-Adressen-Suchmuster
NICK_DROPEMAIL_PATTERN_TOO_LONG
	Das Suchmuster ist zu lang. (maximal %d Zeichen)
NICK_DROPEMAIL_NONE
	Keine Chatnamen mit diesem Suchmuster gefunden.
NICK_DROPEMAIL_COUNT
	%d Chatnamen haben E-Mail-Adressen entsprechend dem Suchmuster.
	Um diese Chatnamen zu droppen, folgendes eingeben:
	/msg %s DROPEMAIL-CONFIRM %s
NICK_DROPEMAIL_CONFIRM_SYNTAX
	DROPEMAIL-CONFIRM E-Mail-Adresse
NICK_DROPEMAIL_CONFIRM_UNKNOWN
	Der mitgegebene Wert ist nicht bekannt. Bitte den DROPEMAIL Befehl nochmals mit diesem Suchmuster benutzen, bevor der DROPEMAIL-CONFIRM benutzt wird.
NICK_DROPEMAIL_CONFIRM_DROPPING
	Droppe %d Chatnamen...
NICK_DROPEMAIL_CONFIRM_DROPPED
	Chatnamen gedroppt.

# SET responses
NICK_SET_SYNTAX
	SET Option Parameter
NICK_SET_OPER_SYNTAX
	SET [!Chatname] Option Parameter
NICK_SET_DISABLED
	Die Chatname-Option ist temporär nicht verfügbar.
NICK_SET_UNKNOWN_OPTION
	Unbekannte SET-Option: %s.

# SET PASSWORD responses
NICK_SET_PASSWORD_FAILED
	Das Passwort konnte nicht geändert werden.
NICK_SET_PASSWORD_CHANGED
	Das Passwort wurde geändert.
NICK_SET_PASSWORD_CHANGED_TO
	Das Passwort wurde in %s geändert.

# SET LANGUAGE responses
NICK_SET_LANGUAGE_SYNTAX
	SET LANGUAGE Nummer
NICK_SET_LANGUAGE_UNKNOWN
	Unbekannte Sprachen-Nummer %d. Eine Liste aller Sprachen bekommt man durch folgenden Befehl: /msg %s HELP SET LANGUAGE ein
NICK_SET_LANGUAGE_CHANGED
	Die Sprache wurde in %s geändert.

# SET URL responses
NICK_SET_URL_CHANGED
	URL für %s wurde zu %s geändert.

# SET EMAIL responses
### CHANGED: NICK_SET_EMAIL_PLEASE_WAIT added
NICK_SET_EMAIL_UNAUTHED
	Die E-Mail-Adresse, die Sie angegeben haben, wurde bis jetzt nicht authentifiziert. Benutzen Sie den AUTH Befehl um die E-Mail-Adresse zu authentifizieren.
NICK_SET_EMAIL_TOO_MANY_NICKS
	%s hat bereits %d registrierte Chatnamen (Maximum %d). Die Änderung der E-Mail-Adresse fehlgeschlagen.
NICK_SET_EMAIL_CHANGED
	E-Mail-Adresse für %s wurde zu %s geändert.

# SET INFO responses
NICK_SET_INFO_CHANGED
	Information für %s wurde geändert.

# SET KILL responses
NICK_SET_KILL_SYNTAX
	SET KILL {ON | QUICK | OFF}
NICK_SET_KILL_IMMED_SYNTAX
	SET KILL {ON | QUICK | IMMED | OFF}
NICK_SET_KILL_ON
	Der Chatnamen-Übernahme-Schutz ist jetzt: ON (eingeschaltet).
NICK_SET_KILL_QUICK
	Der Chatnamen-Übernahme-Schutz ist jetzt: QUICK (eingeschaltet mit kurzer Warnzeit)
NICK_SET_KILL_IMMED
	Der Chatnamen-Übernahme-Schutz ist jetzt: IMMED (eingeschaltet ohne Warnzeit)
NICK_SET_KILL_IMMED_DISABLED
	Die IMMED-Option ist nicht verfügbar.
NICK_SET_KILL_OFF
	Der Chatnamen-Übernahme-Schutz ist jetzt: OFF (ausgeschaltet).

# SET SECURE responses
NICK_SET_SECURE_SYNTAX
	SET SECURE {ON | OFF}
NICK_SET_SECURE_ON
	Die Secure-Option ist jetzt: ON (eingeschaltet).
NICK_SET_SECURE_OFF
	Die Secure-Option ist jetzt: OFF (ausgeschaltet).

# SET PRIVATE responses
NICK_SET_PRIVATE_SYNTAX
	SET PRIVATE {ON | OFF}
NICK_SET_PRIVATE_ON
	Die Privat-Option ist jetzt: ON (eingeschaltet).
NICK_SET_PRIVATE_OFF
	Die Privat-Option ist jetzt: OFF (ausgeschaltet).

# SET NOOP responses
NICK_SET_NOOP_SYNTAX
	SET NOOP {ON | OFF}

# SET HIDE responses
NICK_SET_HIDE_SYNTAX
	SET HIDE {EMAIL | USERMASK | QUIT} {ON | OFF}
NICK_SET_HIDE_EMAIL_ON
	Ihre E-Mail-Adresse wird nun bei einer '%s INFO'-Anfrage nicht angezeigt.
NICK_SET_HIDE_EMAIL_OFF
	Ihre E-Mail-Adresse wird nun bei einer '%s INFO'-Anfrage angezeigt.
NICK_SET_HIDE_MASK_ON
	Ihre last seen user@host mask wird nun bei einer '%s INFO'-Anfrage nicht angezeigt.
NICK_SET_HIDE_MASK_OFF
	Ihre last seen user@host mask wird nun bei einer '%s INFO'-Anfrage angezeigt.
NICK_SET_HIDE_QUIT_ON
	Ihre Abschlussnachricht (QUIT) wird nun bei einer %s INFO'-Abfrage nicht angezeigt.
NICK_SET_HIDE_QUIT_OFF
	Ihre Abschlussnachricht (QUIT) wird nun bei einer '%s INFO'-Abfrage angezeigt.

# SET TIMEZONE responses
NICK_SET_TIMEZONE_SYNTAX
	SET TIMEZONE {UTC-Offset | Zeitzone | DEFAULT}
NICK_SET_TIMEZONE_TO
	Ihre Zeitzone wurde zu UTC%c%d:%02d geändert.
	Ihre jetzige Zeit in dieser Zone ist %s.
NICK_SET_TIMEZONE_DEFAULT
	Ihre Zeitzone wurde in die Default-Zone der Services geändert.

# SET MAINNICK responses
NICK_SET_MAINNICK_NOT_FOUND
	Der Chatname %s ist nicht mit Ihrem Chatnamen verbunden.
NICK_SET_MAINNICK_CHANGED
	Ihr Haupt-Chatname wurde zu %s geändert.

# SET NOEXPIRE responses
NICK_SET_NOEXPIRE_SYNTAX
	SET [!Chatname] NOEXPIRE {ON | OFF}
NICK_SET_NOEXPIRE_ON
	Der Chatname %s wird nicht verfallen.
NICK_SET_NOEXPIRE_OFF
	Der Chatname %s wird verfallen.

# UNSET responses
NICK_UNSET_SYNTAX
	UNSET {URL | EMAIL | INFO}
NICK_UNSET_SYNTAX_REQ_EMAIL
	UNSET {URL | INFO}
NICK_UNSET_OPER_SYNTAX
	UNSET [!Chatname] {URL | EMAIL | INFO}
NICK_UNSET_OPER_SYNTAX_REQ_EMAIL
	UNSET [!Chatname] {URL | INFO}
NICK_UNSET_URL
	URL für %s wurde zurückgesetzt.
NICK_UNSET_EMAIL
	E-Mail-Adresse für %s wurde zurückgesetzt.
NICK_UNSET_INFO
	Information für %s wurde zurückgesetzt.
NICK_UNSET_EMAIL_BAD
	Ihre E-Mail-Adresse kann nicht zurückgesetzt werden.
NICK_UNSET_EMAIL_OTHER_BAD
	E-Mail-Adressen können nicht zurückgesetzt werden.

# ACCESS responses
NICK_ACCESS_SYNTAX
	ACCESS {ADD | DEL | LIST} [Maske]
NICK_ACCESS_DISABLED
	Die Chatnamen Access Liste steht zur Zeit nicht zur Verfügung.
NICK_ACCESS_ALREADY_PRESENT
	%s steht bereits auf Ihrer Zugriffsliste.
NICK_ACCESS_REACHED_LIMIT
	Limit der %d Einträge der Zugriffsliste erschöpft.
NICK_ACCESS_NO_NICKS
	Warnung: Zugriffslisten sollten nicht Chatnamen beinhalten. Bitte Eingabe nochmal überprüfen, ob die Hostmaske (user@host) korrekt angegeben wurde.
NICK_ACCESS_ADDED
	%s wurde zu Ihrer Zugriffsliste hinzugefügt.
NICK_ACCESS_NOT_FOUND
	%s wurde in Ihrer Zugriffsliste nicht gefunden.
NICK_ACCESS_DELETED
	%s wurde von Ihrer Zugriffsliste gelöscht.
NICK_ACCESS_LIST_EMPTY
	Ihre Zugriffsliste ist leer.
NICK_ACCESS_LIST
	Zugriffsliste:
NICK_ACCESS_LIST_X_EMPTY
	Die Zugriffsliste von %s ist leer.
NICK_ACCESS_LIST_X
	Zugriffsliste für %s:

# LINK responses
NICK_OLD_LINK_SYNTAX
	LINK Chatname Passwort
NICK_LINK_SYNTAX
	LINK Chatname
NICK_LINK_DISABLED
	Zur Zeit ist das Linken von Chatnamen deaktiviert.
NICK_LINK_FAILED
	Der Link konnte nicht erzeugt werden.
NICK_CANNOT_BE_LINKED
	Chatname %s kann/darf nicht verlinkt werden.
NICK_OLD_LINK_SAME
	Ein Link zum selben Chatnamen nicht unmöglich!
NICK_LINK_SAME
	Ein Link vom aktuellen Chatnamen nicht möglich!
NICK_LINK_ALREADY_LINKED
	Der Chatname %s ist bereits mit Ihrem Chatnamen verbunden.
NICK_LINK_IN_USE
	Der Chatname %s wird momentan benutzt.
NICK_LINK_TOO_MANY
	Sie können bis zu %d Chatnamen miteinander verbinden.
NICK_LINK_TOO_MANY_NICKS
	Sie haben bereits %d registrierte Chatnamen (Maximum %d). Link fehlgeschlagen.
NICK_OLD_LINK_TOO_MANY_CHANNELS
	Kann keinen Link zu %s nicht erzeugen: Zu viele registrierte Räume (Maximum zusammen %d).
NICK_OLD_LINKED
	Der Link zu %s wurde erzeugt.
NICK_LINKED
	Der Chatname %s wurde mit Ihrem Chatnamen verbunden.

# UNLINK responses
NICK_OLD_UNLINK_SYNTAX
	UNLINK [Chatname Passwort]
NICK_UNLINK_SYNTAX
	UNLINK Chatname
NICK_UNLINK_OPER_SYNTAX
	UNLINK Chatname [FORCE]
NICK_UNLINK_DISABLED
	Zur Zeit ist das Unlinken von Chatnamen deaktiviert.
NICK_UNLINK_FAILED
	Der Link konnte nicht gelöscht werden.
NICK_UNLINK_SAME
	Sie können einen Link vom aktuellen Chatnamen nicht löschen.
NICK_OLD_UNLINK_NOT_LINKED
	Der Chatname hat keinen Link.
NICK_UNLINK_NOT_LINKED_YOURS
	Der Chatname %s ist nicht mit Ihrem Chatnamen verbunden.
NICK_UNLINK_NOT_LINKED
	Der Chatname %s hat keinen Link.
NICK_OLD_UNLINKED
	Der Link zu %s wurde gelöscht.
NICK_UNLINKED
	Der Link vom Chatnamen %s zu Ihrem Chatnamen wurde gelöscht.
NICK_X_UNLINKED
	Der Link vom Chatnamen %s zu %s wurde gelöscht.

# LISTLINKS responses
NICK_OLD_LISTLINKS_SYNTAX
	LISTLINKS Chatname
NICK_LISTLINKS_SYNTAX
	LISTLINKS
NICK_LISTLINKS_HEADER
	Links zum Chatnamen %s:
NICK_LISTLINKS_FOOTER
	%d verbundene Chatnamen.

# INFO responses
NICK_INFO_SYNTAX
	INFO Chatname [ALL]
NICK_INFO_REALNAME
	%s ist %s
NICK_INFO_ADDRESS
	   Last-seen Adresse: %s
NICK_INFO_ADDRESS_ONLINE
	      Ist online von: %s
NICK_INFO_ADDRESS_ONLINE_NOHOST
	%s ist momentan online.
### CHANGED: NICK_INFO_ADDRESS_OTHER_NICK added
NICK_INFO_TIME_REGGED
	  Registrationsdatum: %s
NICK_INFO_LAST_SEEN
	   Zuletzt gesehem am: %s
NICK_INFO_LAST_QUIT
	Letzte QUIT-Nachricht: %s
NICK_INFO_URL
	                  URL: %s
NICK_INFO_EMAIL
	        E-Mail-Adresse: %s
NICK_INFO_EMAIL_UNAUTHED
	        E-Mail-Adresse: %s (nicht verifiziert)
NICK_INFO_INFO
	          Information: %s
NICK_INFO_OPTIONS
	             Optionen: %s
# These strings MUST NOT be empty
NICK_INFO_OPT_KILL
	Chatnamen-Übernahme-Schutz
NICK_INFO_OPT_SECURE
	Secure Option
NICK_INFO_OPT_PRIVATE
	Privat
NICK_INFO_OPT_NONE
	Keine
NICK_INFO_NO_EXPIRE
	Dieser Chatname wird nicht verfallen.
NICK_INFO_SUSPEND_DETAILS
	Suspendiert von %s am %s (%s).
NICK_INFO_SUSPEND_REASON
	Grund: %s
NICK_INFO_SHOW_ALL
	/msg %s INFO %s ALL für weitere Informationen

# LISTCHANS responses
NICK_LISTCHANS_SYNTAX
	LISTCHANS
NICK_LISTCHANS_NONE
	%s hat keine registrierten Räume.
NICK_LISTCHANS_HEADER
	Registrierte Räume von %s:
NICK_LISTCHANS_END
	Ende der Liste (%d registrierte Räume)

# LIST responses
#NICK_LIST_SYNTAX
#	%s Muster
#NICK_LIST_OPER_SYNTAX
#	%s Muster [FORBIDDEN] [NOEXPIRE] [SUSPENDED]
#NICK_LIST_OPER_SYNTAX_AUTH
#	%s Muster [FORBIDDEN] [NOEXPIRE] [SUSPENDED] [NOAUTH]
NICK_LIST_HEADER
	Liste aller Einträge, auf die %s zutrifft:
NICK_LISTEMAIL_NONE
	(keine gesetzt)

# RECOVER responses
NICK_RECOVER_SYNTAX
	RECOVER Chatname [Passwort]
NICK_NO_RECOVER_SELF
	Sie können sich selbst nicht killen!
NICK_RECOVERED
	Der User mit Ihrem Chatnamen wurde aus dem Chat-Netzwerk gekillt. Mit /msg %s RELEASE %s Passwort können Sie Ihren Chatnamen vor der Zeit von 1 Minute zurückerhalten.

# RELEASE responses
NICK_RELEASE_SYNTAX
	RELEASE Chatname [Passwort]
NICK_RELEASE_NOT_HELD
	Der Chatname %s wird nicht beibehalten.
NICK_RELEASED
	Chatname wurde freigestellt.

# GHOST responses
NICK_GHOST_SYNTAX
	GHOST Chatname [Passwort]
NICK_NO_GHOST_SELF
	Sie können sich selbst nicht killen!
NICK_GHOST_KILLED
	Verbindung des hängengebliebenen Chatname wurde unterbrochen.

# GETPASS responses
NICK_GETPASS_SYNTAX
	GETPASS Chatname
### CHANGED: nickname parameter inserted
#NICK_GETPASS_UNAVAILABLE
#	Aufgrund der Nutzung von Verschlüsselung ist der GETPASS-Befehl ist nicht verfügbar.
NICK_GETPASS_PASSWORD_IS
	Das Passwort des für %s lautet %s.

# FORBID responses
NICK_FORBID_SYNTAX
	FORBID Chatname
NICK_FORBID_SUCCEEDED
	Der Chatname %s ist jetzt gesperrt.
NICK_FORBID_FAILED
	Der Chatname %s konnte nicht gesperrt werden!

# SUSPEND responses
NICK_SUSPEND_SYNTAX
	SUSPEND [+Dauer] Chatname Grund
NICK_SUSPEND_SUCCEEDED
	Der Chatname %s und alle damit verbundene Chatnamen sind suspendiert.
NICK_SUSPEND_ALREADY_SUSPENDED
	Der Chatname %s ist bereits suspendiert.

# UNSUSPEND responses
NICK_UNSUSPEND_SYNTAX
	UNSUSPEND Chatname
NICK_UNSUSPEND_SUCCEEDED
	Suspend von %s ist jetzt aufgehoben.
NICK_UNSUSPEND_NOT_SUSPENDED
	Der Chatname %s ist nicht suspendiert.

################ autojoin module messages/responses
# AJOIN responses
NICK_AJOIN_SYNTAX
	AJOIN {ADD | DEL | LIST} [Raum]
NICK_AJOIN_ADD_SYNTAX
	AJOIN ADD Raum
NICK_AJOIN_DEL_SYNTAX
	AJOIN DEL Raum
NICK_AJOIN_DISABLED
	Die Chatnamen AutoJoin Liste steht zur Zeit nicht zur Verfügung.
NICK_AJOIN_ALREADY_PRESENT
	Raum %s ist bereits auf der Autojoin-Liste.
NICK_AJOIN_LIST_FULL
	Die Autojoin-Liste kann bis zu %d Einträge haben. Liste voll.
NICK_AJOIN_ADDED
	%s wurde zu Ihrer Autojoin-Liste hinzugefügt.
NICK_AJOIN_NOT_FOUND
	%s wurde auf Ihrer Autojoin-Liste nicht gefunden.
NICK_AJOIN_DELETED
	%s wurde von Ihrer Autojoin-Liste entfernt.
NICK_AJOIN_LIST_EMPTY
	Ihre Autojoin-Liste ist leer.
NICK_AJOIN_LIST
	Autojoin-Liste:
NICK_AJOIN_LIST_X_EMPTY
	Die Autojoin-Liste von %s ist leer.
NICK_AJOIN_LIST_X
	Autojoin-Liste von %s:

################ mail-auth module messages/responses

# General-purpose messages
NICK_AUTH_FOR_REGISTER
	Wenn sie diese Nachricht bekommen, geben sie bitte, um die Registrierung des Chatnamens abzuschließen, folgenden Befehl ein: /msg %s AUTH Code. Den Code ersetzen sie mit dem, den sie in der Nachricht bekommen haben.
NICK_AUTH_FOR_SET_EMAIL
	Wenn sie diese Nachricht bekommen, geben sie bitte, um die Änderung der E-Mail-Adresse abzuschließen, folgenden Befehl ein: /msg %s AUTH Code. Den Code ersetzen sie mit dem, den sie in der Nachricht bekommen haben.
	Hinweis: Sie werden Ihren Chatnamen nicht benutzen können, bis Sie den Befehl angewandt haben.
NICK_AUTH_NOT_NEEDED
	Eine Authentifizierung ist momentan nicht notwendig.
NICK_AUTH_NO_AUTHCODE
	Der Chatname %s hat keinen Authentifizierungscode.
NICK_AUTH_HAS_AUTHCODE
	Der Chatname %s hat bereits einen Authentifizierungscode.
PASSWORD_WARNING_FOR_AUTH
	Warnung: Nach noch einem missglückten Versuch, den Authentifizierungscode einzugeben, werden sie vom Server getrennt.

# Mail text.  The last "%s" (before the user@host) in the body text is
# replaced by one of the NICK_AUTH_MAIL_TEXT_* messages.
NICK_AUTH_MAIL_SUBJECT
	Authentifizierungscode für %s
NICK_AUTH_MAIL_BODY
	Der Authentifizierungscode für Ihren Chatnamen (%s) ist: %09d
	Geben Sie es %s im Chat durch folgenden Befehl ein:
	    /msg %s AUTH %09d
	
	Diese Nachricht wurde von %s verschickt, als Antwort auf %s
	%s@%s.
NICK_AUTH_MAIL_TEXT_REG
	eine Registrierung durch
NICK_AUTH_MAIL_TEXT_EMAIL
	die Änderung der E-Mail-Adresse durch
NICK_AUTH_MAIL_TEXT_SENDAUTH
	einen SENDAUTH-Befehl abgesetzt vom
NICK_AUTH_MAIL_BODY_SETAUTH
	Der Authentifizierungscode für Ihren Chatnamen (%s) ist: %09d
	Geben Sie es %s im Chat durch folgenden Befehl ein:
	    /msg %s AUTH %09d

# AUTH responses
NICK_AUTH_SYNTAX
	AUTH Code
NICK_AUTH_DISABLED
	Die Chatnamen-Authentifizierung ist temporär nicht verfügbar.
NICK_AUTH_FAILED
	Der Authentifizierungscode ist nicht korrekt.
NICK_AUTH_SUCCEEDED_REGISTER
	Authentifizierung erfolgreich; die Registrierung des Chatnamens ist jetzt abgeschlossen.
NICK_AUTH_SUCCEEDED_SET_EMAIL
	Authentifizierung erfolgreich; die Änderung der E-Mail-Adresse ist jetzt abgeschlossen.
NICK_AUTH_SUCCEEDED_SETAUTH
	Authentifizierung erfolgreich; Sie können Ihren Chatnamen wieder benutzen.

# SENDAUTH responses
NICK_SENDAUTH_SYNTAX
	SENDAUTH
NICK_SENDAUTH_TOO_SOON
	Bitte, warten Sie %s bevor Sie den SENDAUTH-Befehl wieder nutzen.

# REAUTH responses
NICK_REAUTH_SYNTAX
	REAUTH
### CHANGED: REAUTH, RESTOREMAIL messages added

# SETAUTH responses
NICK_SETAUTH_SYNTAX
	SETAUTH Chatname
NICK_SETAUTH_NO_EMAIL
	Der Chatname %s hat keine E-Mail-Adresse angegeben.
NICK_SETAUTH_AUTHCODE_SET
	Der neue Authentifizierungscode (%09d) wurde für den Chatnamen %s gesetzt.
NICK_SETAUTH_SEND_NORESOURCES
	Warnung: Versenden der E-Mail zu %s fehlgeschlagen. Nicht genug Ressourcen.
#NICK_SETAUTH_USER_NOTICE
#	Sie müssen Ihren Chatnamen authentifizieren, bevor Sie ihn nutzen können. Der Authentifizierungscode wurde an folgende E-Mail-Adresse verschickt: %s.
#	/msg %s HELP AUTH für weitere Informationen.

# GETAUTH responses
NICK_GETAUTH_SYNTAX
	GETAUTH Chatname
NICK_GETAUTH_AUTHCODE_IS
	Der Authentifizierungscode für %s ist: %09d.

# CLEARAUTH responses
NICK_CLEARAUTH_SYNTAX
	CLEARAUTH Chatname
NICK_CLEARAUTH_CLEARED
	Der Authentifizierungscode für %s wurde zurückgesetzt.

###########################################################################
#
# ChanServ messages
#
###########################################################################

# Access level descriptions
CHAN_LEVEL_AUTOOP
	Automatischer Raumoperator-Status
CHAN_LEVEL_AUTOVOICE
	Automatischer Moderator-Status
CHAN_LEVEL_INVITE
	Der INVITE-Befehl darf benutzt werden
CHAN_LEVEL_AKICK
	Der AKICK-Befehl darf benutzt werden
CHAN_LEVEL_SET
	Der SET-Befehl darf benutzt werden (bis auf FOUNDER/PASSWORD)
CHAN_LEVEL_CLEAR
	Der CLEAR-Befehl darf benutzt werden
CHAN_LEVEL_UNBAN
	Der UNBAN-Befehl darf benutzt werden
CHAN_LEVEL_OPDEOP
	Die OP/DEOP-Befehle dürfen benutzt werden
CHAN_LEVEL_ACCESS_LIST
	Die Zugriffsliste darf angezeigt werden
CHAN_LEVEL_ACCESS_CHANGE
	Die Zugriffsliste darf modifiziert werden
#CHAN_LEVEL_MEMO
#	Raum-Memos dürfen aufgelistet/gelesen werden
CHAN_LEVEL_VOICE
	Die VOICE/DEVOICE-Befehle dürfen benutzt werden
CHAN_LEVEL_AUTOHALFOP
	Automatischer Raumoperator-Status (HalfOp +h)
CHAN_LEVEL_HALFOP
	Die HALFOP/DEHALFOP-Befehle dürfen benutzt werden
CHAN_LEVEL_AUTOPROTECT
	Automatischer Raum-Schutz-Status (+a)
CHAN_LEVEL_PROTECT
	Die PROTECT/DEPROTECT-Befehle dürfen benutzt werden
CHAN_LEVEL_KICK
	Der KICK-Befehl darf benutzt werden
CHAN_LEVEL_STATUS
	Der STATUS-Befehl darf benutzt werden
CHAN_LEVEL_TOPIC
	Der TOPIC-Befehl darf benutzt werden

# Automatic responses
CHAN_IS_REGISTERED
	Dieser Raum ist mit %s registriert worden.
CHAN_MAY_NOT_BE_USED
	Dieser Raum darf nicht benutzt werden.
CHAN_NOT_ALLOWED_TO_JOIN
	Ihnen ist der Aufenthalt in diesem Raum untersagt.

# Multi-purpose responses
CHAN_MUST_IDENTIFY_NICK
	Bitte, Identifizieren Sie sich zuerst mit %s. Befehl:
	/msg %s IDENTIFY Passwort
CHAN_BOUNCY_MODES
	Der %s-Befehl kann zur Zeit nicht benutzt werden.


# REGISTER responses
CHAN_REGISTER_SYNTAX
	REGISTER Raum Passwort Beschreibung
CHAN_REGISTER_DISABLED
	Die Raum-Registrierung ist temporär nicht verfügbar.
CHAN_REGISTER_SHORT_CHANNEL
	Der Raum # kann nicht registriert werden.
CHAN_REGISTER_INVALID_NAME
	Nur Räume, deren Namen mit # anfangen, können registriert werden.
CHAN_REGISTER_NOT_LOCAL
	Lokale Räume können nicht registriert werden.
CHAN_MUST_REGISTER_NICK
	Sie müssen zuerst Ihren Chatnamen registrieren. Für weitere Informationen zur Chatnamen-Registrierung bitte /msg %s HELP eingeben.
CHAN_MAY_NOT_BE_REGISTERED
	Der Raum %s darf nicht registriert werden.
CHAN_ALREADY_REGISTERED
	Der Raum %s ist bereits registriert!
CHAN_MUST_BE_CHANOP
	Sie müssen Raumoperator sein um diesen Raum zu registrieren.
CHAN_REACHED_CHANNEL_LIMIT
	Limit der %d registrierten Räume erreicht.
CHAN_EXCEEDED_CHANNEL_LIMIT
	Limit der %d registrierten Räume überschritten.
CHAN_REGISTRATION_FAILED
	Die Registrierung des Raumes ist fehlgeschlagen.
CHAN_REGISTERED
	Der Raum %s ist auf Sie registriert: %s
CHAN_PASSWORD_IS
	Ihr Raum-Passwort lautet %s. Bitte merken Sie es sich.

# IDENTIFY responses
CHAN_IDENTIFY_SYNTAX
	IDENTIFY Raum Passwort
CHAN_IDENTIFY_FAILED
	Identifizierung ist fehlgeschlagen.
CHAN_IDENTIFY_SUCCEEDED
	Passwort akzeptiert - Sie haben jetzt Zugriff auf Gründer-Stufe zu %s.

# DROP responses
CHAN_DROP_SYNTAX
	DROP Raum Passwort
CHAN_DROP_DISABLED
	Die Raum-Freigabe ist temporär nicht verfügbar.
CHAN_DROPPED
	Der Raum %s wurde freigegeben.

# DROPCHAN responses
CHAN_DROPCHAN_SYNTAX
	DROPCHAN Raum

# SET responses
CHAN_SET_SYNTAX
	SET Raum Option Parameter
CHAN_SET_DISABLED
	Die Raum-Option ist temporär nicht verfügbar.
CHAN_SET_UNKNOWN_OPTION
	Unbekannte SET-Option %s.

# SET FOUNDER responses
CHAN_SET_FOUNDER_TOO_MANY_CHANS
	%s hat zu viele Räume registriert.
CHAN_FOUNDER_CHANGED
	Der Gründer vom Raum %s wurde zu %s geändert.

# SET SUCCESSOR responses
CHAN_SUCCESSOR_CHANGED
	Der Nachfolger für %s wurde in %s geändert.
CHAN_SUCCESSOR_IS_FOUNDER
	Der Nachfolger kann nicht gleichzeitig Gründer sein.

# SET PASSWORD responses
CHAN_SET_PASSWORD_FAILED
	Das Passwort konnte nicht gesetzt werden.
CHAN_PASSWORD_CHANGED
	Das Passwort von %s wurde geändert.
CHAN_PASSWORD_CHANGED_TO
	Das Passwort von %s wurde in %s geändert.

# SET DESC responses
CHAN_DESC_CHANGED
	Die Beschreibung von %s wurde in %s geändert.

# SET URL responses
CHAN_URL_CHANGED
	URL für %s wurde zu %s geändert.

# SET EMAIL responses
CHAN_EMAIL_CHANGED
	Die E-Mail-Adresse für %s wurde in %s geändert.

# SET ENTRYMSG responses
CHAN_ENTRY_MSG_CHANGED
	Die Begrüssungsnachricht für %s wurde geändert.

# SET MLOCK responses
CHAN_SET_MLOCK_NEED_PLUS_MINUS
	Modusliste muss mit + (plus) oder - (minus) beginnen.
CHAN_SET_MLOCK_NEED_PARAM
	MLOCK +%c benötigt einen Parameter.
CHAN_SET_MLOCK_NEED_POSITIVE
	Der Parameter für MLOCK +%c muss eine positive Zahl sein.
CHAN_SET_MLOCK_MODE_REG_BAD
	Modus +%c kann mit MLOCK nicht benutzt werden (wird immer bei registrierten Räumen gesetzt).
CHAN_SET_MLOCK_LINK_BAD
	Der Parameter für MLOCK +%c ist kein gültiger Raumname.
CHAN_SET_MLOCK_LINK_SAME
	Der Parameter für MLOCK +%c muss ein anderer Raum sein.
CHAN_SET_MLOCK_REQUIRES
	Beim MLOCK +%c muss ebenfalls MLOCK +%c gesetzt sein.
CHAN_SET_MLOCK_BAD_PARAM
	Der Parameter für MLOCK +%c ist nicht gültig.
CHAN_SET_MLOCK_UNKNOWN_CHAR
	Unbekanntes Modus-Zeichen %c wird ignoriert.
CHAN_SET_MLOCK_CANNOT_LOCK
	Modus %c kann nicht erzwungen werden.
CHAN_MLOCK_CHANGED
	Der Sperr-Modus für Raum %s wurde in %s geändert.
CHAN_MLOCK_REMOVED
	Der Sperr-Modus für Raum %s wurde zurückgenommen.

# SET HIDE responses
CHAN_SET_HIDE_SYNTAX
	SET Raum HIDE {EMAIL | TOPIC | MLOCK} {ON | OFF}
CHAN_SET_HIDE_EMAIL_ON
	Die E-Mail-Adresse des Raumes %s wird bei einer '%s INFO'-Anfrage nicht angezeigt.
CHAN_SET_HIDE_EMAIL_OFF
	Die E-Mail-Adresse des Raumes %s wird bei einer '%s INFO'-Anfrage angezeigt.
CHAN_SET_HIDE_TOPIC_ON
	Das Thema des Raumes %s wird bei einer '%s INFO'-Anfrage nicht angezeigt.
CHAN_SET_HIDE_TOPIC_OFF
	Das Thema des Raumes %s wird bei einer '%s INFO'-Anfrage angezeigt.
CHAN_SET_HIDE_MLOCK_ON
	Das Sperr-Modus des Raumes %s wird bei einer '%s INFO'-Anfrage nicht angezeigt.
CHAN_SET_HIDE_MLOCK_OFF
	Das Sperr-Modus des Raumes %s wird bei einer '%s INFO'-Anfrage angezeigt.

# SET KEEPTOPIC responses
CHAN_SET_KEEPTOPIC_SYNTAX
	SET Raum KEEPTOPIC {ON | OFF}
CHAN_SET_KEEPTOPIC_ON
	Die Option zur Beibehaltung des Themas ist jetzt: ON (eingeschaltet).
CHAN_SET_KEEPTOPIC_OFF
	Die Option zur Beibehaltung des Themas ist jetzt: OFF (ausgeschaltet).

# SET TOPICLOCK responses
CHAN_SET_TOPICLOCK_SYNTAX
	SET Raum TOPICLOCK {ON | OFF}
CHAN_SET_TOPICLOCK_ON
	Die Option zur Sperrung des Themas ist jetzt: ON (eingeschaltet).
CHAN_SET_TOPICLOCK_OFF
	Die Option zur Sperrung des Themas ist jetzt: OFF (ausgeschaltet).

# SET PRIVATE responses
CHAN_SET_PRIVATE_SYNTAX
	SET Raum PRIVATE {ON | OFF}
CHAN_SET_PRIVATE_ON
	Die Privat-Option ist jetzt: ON (eingeschaltet).
CHAN_SET_PRIVATE_OFF
	Die Privat-Option ist jetzt: OFF (ausgeschaltet).

# SET SECUREOPS responses
CHAN_SET_SECUREOPS_SYNTAX
	SET Raum SECUREOPS {ON | OFF}
CHAN_SET_SECUREOPS_ON
	Die Secure-Ops-Option ist jetzt: ON (eingeschaltet).
CHAN_SET_SECUREOPS_OFF
	Die Secure-Ops-Option ist jetzt: OFF (ausgeschaltet).

# SET LEAVEOPS responses
CHAN_SET_LEAVEOPS_SYNTAX
	SET Raum LEAVEOPS {ON | OFF}
CHAN_SET_LEAVEOPS_ON
	Die Leave-Ops-Option ist jetzt: ON (eingeschaltet).
CHAN_SET_LEAVEOPS_OFF
	Die Leave-Ops-Option ist jetzt: OFF (ausgeschaltet).

# SET RESTRICTED responses
CHAN_SET_RESTRICTED_SYNTAX
	SET Raum RESTRICTED {ON | OFF}
CHAN_SET_RESTRICTED_ON
	Die Zugangsbeschränkungs-Option ist jetzt: ON (eingeschaltet).
CHAN_SET_RESTRICTED_OFF
	Die Zugangsbeschränkungs-Option ist jetzt: OFF (ausgeschaltet).

# SET SECURE responses
CHAN_SET_SECURE_SYNTAX
	SET Raum SECURE {ON | OFF}
CHAN_SET_SECURE_ON
	Secure-Option ist jetzt: ON (eingeschaltet).
CHAN_SET_SECURE_OFF
	Secure-Option ist jetzt: OFF (ausgeschaltet).

# SET OPNOTICE responses
CHAN_SET_OPNOTICE_SYNTAX
	SET Raum OPNOTICE {ON | OFF}
CHAN_SET_OPNOTICE_ON
	Op-Notice-Option ist jetzt: ON (eingeschaltet).
CHAN_SET_OPNOTICE_OFF
	Op-Notice-Option ist jetzt: OFF (ausgeschaltet).

# SET ENFORCE responses
CHAN_SET_ENFORCE_SYNTAX
	SET Raum ENFORCE {ON | OFF}
CHAN_SET_ENFORCE_ON
	Enforce-Option ist jetzt: ON (eingeschaltet).
CHAN_SET_ENFORCE_OFF
	Enforce-Option ist jetzt: OFF (ausgeschaltet).

# SET MEMO-RESTRICTED responses
CHAN_SET_MEMO_RESTRICTED_SYNTAX
	SET Raum MEMO-RESTRICTED {ON | OFF}

# SET NOEXPIRE responses
CHAN_SET_NOEXPIRE_SYNTAX
	SET Raum NOEXPIRE {ON | OFF}
CHAN_SET_NOEXPIRE_ON
	Der Raum %s wird nicht verfallen.
CHAN_SET_NOEXPIRE_OFF
	Der Raum %s wird verfallen.

# UNSET responses
CHAN_UNSET_SYNTAX
	UNSET Raum {SUCCESSOR | URL | EMAIL | ENTRYMSG}
CHAN_SUCCESSOR_UNSET
	Der Nachfolger für %s wurde zurückgesetzt.
CHAN_URL_UNSET
	URL für %s wurde zurückgesetzt.
CHAN_EMAIL_UNSET
	Die E-Mail-Adresse für %s wurde zurückgesetzt.
CHAN_ENTRY_MSG_UNSET
	Die Begrüssungsnachricht für %s wurde zurückgesetzt.



# ACCESS messages
CHAN_ACCESS_SYNTAX
	ACCESS Raum {ADD | DEL | LIST | LISTLEVEL | COUNT} [Parameter]
### CHANGED: CHAN_ACCESS_*_SYNTAX added
CHAN_ACCESS_DISABLED
	Die Zugriffslisten der Räume können momentan nicht modifiziert werden.
CHAN_ACCESS_LEVEL_NONZERO
	Zugriffsstufe muss eine Zahl außer 0 (Null) sein.
CHAN_ACCESS_LEVEL_RANGE
	Zugriffsstufe muss zwischen %d und %d sein (einschließlich).
CHAN_ACCESS_NICKS_ONLY
	Die Zugriffslisten für Räume können nur registrierte Chatnamen enthalten.
CHAN_ACCESS_REACHED_LIMIT
	Limit der %d Einträge in der Zugriffsliste erreicht.
CHAN_ACCESS_LEVEL_UNCHANGED
	Zugriffsstufe von %s in der Zugriffsliste von %s war bereits %d (blieb unverändert).
CHAN_ACCESS_LEVEL_CHANGED
	Zugriffsstufe von %s in der Zugriffsliste von %s wurde in %d geändert.
CHAN_ACCESS_ADDED
	%s wurde zur %s-Zugriffsliste mit Stufe %d hinzugefügt.
CHAN_ACCESS_NOT_FOUND
	Der Chatname %s steht nicht auf der Zugriffsliste von %s.
CHAN_ACCESS_DELETED
	Der Chatname %s wurde von der Zugriffsliste von %s entfernt.
CHAN_ACCESS_LIST_EMPTY
	Die Zugriffsliste von %s ist leer.
CHAN_ACCESS_NO_MATCH
	Keine passenden Einträge auf der Zugriffsliste von %s.
### CHANGED: entry number field removed
#CHAN_ACCESS_LIST_HEADER
#	Zugriffsliste von %s:
#	  Nr.  St.   Chatname
#CHAN_ACCESS_LIST_FORMAT
#	 %4d  %4d  %s
CHAN_ACCESS_COUNT
	Die Zugriffsliste von %s hat %d Einträge.

# XOP (SOP/AOP/etc.) responses
CHAN_SOP_SYNTAX
	SOP Raum {ADD | DEL | LIST | COUNT} [Parameter]
CHAN_AOP_SYNTAX
	AOP Raum {ADD | DEL | LIST | COUNT} [Parameter]
CHAN_HOP_SYNTAX
	HOP Raum {ADD | DEL | LIST | COUNT} [Parameter]
CHAN_VOP_SYNTAX
	VOP Raum {ADD | DEL | LIST | COUNT} [Parameter]
CHAN_NOP_SYNTAX
	NOP Raum {ADD | DEL | LIST | COUNT} [Parameter]
### CHANGED: CHAN_{S,A,H,V,N}OP_LIST_SYNTAX added
CHAN_XOP_ADDED
	%s wurde zu %s %s-Liste hinzugefügt.
CHAN_XOP_LEVEL_CHANGED
	%s wurde von %s %s-Liste entfernt.
CHAN_XOP_LEVEL_UNCHANGED
	%s ist bereits auf %s %s-Liste.
CHAN_XOP_NICKS_ONLY
	Die SOP-, AOP-, VOP- und NOP-Listen können nur registrierte Chatnamen enthalten.
CHAN_XOP_NICKS_ONLY_HOP
	Die SOP-, AOP-, HOP-, VOP- und VOP-Listen können nur registrierte Chatnamen enthalten.
CHAN_XOP_REACHED_LIMIT
	Die AOP-, SOP-, VOP- oder NOP-Listen können nicht mehr als %d Einträge haben. Limit überschritten.
CHAN_XOP_REACHED_LIMIT_HOP
	Die AOP-, SOP-, HOP-, VOP- oder NOP-Listen können nicht mehr als %d Einträge haben. Limit überschritten.
CHAN_XOP_NOT_FOUND
	%s wurde in der %s %s-Liste nicht gefunden.
CHAN_XOP_DELETED
	%s wurde von der %s %s-Liste entfernt.
CHAN_XOP_LIST_EMPTY
	Die %s %s-Liste ist leer.
CHAN_XOP_NO_MATCH
	Keine passenden Einträge in %s %s-Liste.
CHAN_XOP_LIST_HEADER
	%s-Liste für %s:
CHAN_XOP_COUNT
	%s %s-Liste hat %d Einträge.

# AKICK responses
CHAN_AKICK_SYNTAX
	AKICK Raum {ADD | DEL | LIST | VIEW | COUNT | ENFORCE} [Parameter]
### CHANGED: CHAN_AKICK_{LIST,VIEW}_SYNTAX added
CHAN_AKICK_DISABLED
	Autokick-Listen der Räume können zur Zeit nicht modifiziert werden.
CHAN_AKICK_ALREADY_EXISTS
	Der Eintrag %s ist in der Autokick-Liste von %s bereits enthalten.
CHAN_AKICK_REACHED_LIMIT
	In der Autokick-Liste können maximal %d Einträge existieren.
CHAN_AKICK_ADDED
	Der Eintrag %s wurde in die Autokick-Liste von %s eingetragen.
CHAN_AKICK_NOT_FOUND
	Der Eintrag %s steht nicht in der Autokick-Liste von %s.
CHAN_AKICK_DELETED
	Der Eintrag %s wurde von der Autokick-Liste von %s entfernt.
CHAN_AKICK_LIST_EMPTY
	Die Autokick-Liste von %s ist leer.
CHAN_AKICK_NO_MATCH
	Ein passender Eintrag ist in der Autokick-Liste von %s nicht zu finden.
CHAN_AKICK_LIST_HEADER
	Autokick-Liste von %s:
# mask, set-by, set-time, last-used, reason
CHAN_AKICK_VIEW_FORMAT
	    %s (durch %s am %s; Letzte Nutzung: %s)
	        %s
# mask, set-by, set-time, reason
CHAN_AKICK_VIEW_UNUSED_FORMAT
	    %s (durch %s am %s; Nie genutzt)
	        %s
CHAN_AKICK_COUNT
	%s hat %d Einträge in der Autokick-Liste.
CHAN_AKICK_ENFORCE_DONE
	AKICK ENFORCE für %s beendet; %d Benutzer waren betroffen.

# LEVELS responses
CHAN_LEVELS_SYNTAX
	LEVELS Raum {SET | DIS[ABLE] | LIST | RESET} [Einstellung [Stufe]]
CHAN_LEVELS_READONLY
	Die Raum Privilegien Liste steht zur Zeit nicht zur Verfügung.
CHAN_LEVELS_RANGE
	Die Stufe muss zwischen %d und %d liegen (einschließlich).
CHAN_LEVELS_CHANGED
	Die Stufe für %s im Raum %s wurde in %d geändert.
CHAN_LEVELS_UNKNOWN
	Die Einstellung %s ist unbekannt. Geben Sie /msg %s HELP LEVELS DESC für eine Liste gültiger Einstellungen ein.
CHAN_LEVELS_DISABLED
	%s wurde im Raum %s ausgeschaltet.
#CHAN_LEVELS_LIST_HEADER
#	Zugriffsstufen-Einstellungen für Raum %s:
CHAN_LEVELS_LIST_DISABLED
	    %-*s  (ausgeschaltet)
CHAN_LEVELS_LIST_FOUNDER
	    %-*s  (nur für Gründer)
CHAN_LEVELS_LIST_NORMAL
	    %-*s  %4d
#CHAN_LEVELS_RESET
#	Die Zugriffsstufen für %s wurden auf die Standardwerte zurückgesetzt.

# INFO responses
CHAN_INFO_SYNTAX
	INFO Raum [ALL]
CHAN_INFO_HEADER
	Information über Raum %s:
CHAN_INFO_FOUNDER
	             Gründer: %s
CHAN_INFO_SUCCESSOR
	          Nachfolger: %s
CHAN_INFO_DESCRIPTION
	        Beschreibung: %s
CHAN_INFO_ENTRYMSG
	Begrüssungsnachricht: %s
CHAN_INFO_TIME_REGGED
	 Registrierungsdatum: %s
CHAN_INFO_LAST_USED
	      Letzte Nutzung: %s
CHAN_INFO_LAST_TOPIC
	       Letztes Thema: %s
CHAN_INFO_TOPIC_SET_BY
	   Thema gesetzt von: %s
CHAN_INFO_URL
	                 URL: %s
CHAN_INFO_EMAIL
	      E-Mail-Adresse: %s
CHAN_INFO_OPTIONS
	            Optionen: %s
CHAN_INFO_OPT_PRIVATE
	Privat (Mit LIST Befehl unsichtbar)
CHAN_INFO_OPT_KEEPTOPIC
	Beibehaltung des Themas
CHAN_INFO_OPT_TOPICLOCK
	Sperrung des Themas
CHAN_INFO_OPT_SECUREOPS
	Secure Ops
CHAN_INFO_OPT_LEAVEOPS
	Leave Ops
CHAN_INFO_OPT_RESTRICTED
	beschränkter Zugang
CHAN_INFO_OPT_SECURE
	Secure
CHAN_INFO_OPT_OPNOTICE
	Op-Notice
CHAN_INFO_OPT_ENFORCE
	Enforce
CHAN_INFO_OPT_NONE
	Keine
CHAN_INFO_MODE_LOCK
	        Sperr-Modus: %s
CHAN_INFO_NO_EXPIRE
	Dieser Raum wird nicht verfallen.
CHAN_INFO_SUSPEND_DETAILS
	Suspendiert von %s am %s (%s).
CHAN_INFO_SUSPEND_REASON
	Begründung: %s
CHAN_INFO_SHOW_ALL
	/msg %s INFO %s ALL für weitere Informationen.

# LIST responses
#CHAN_LIST_SYNTAX
#	LIST Muster
#CHAN_LIST_OPER_SYNTAX
#	LIST Muster [FORBIDDEN] [NOEXPIRE] [SUSPENDED]
CHAN_LIST_HEADER
	Liste der zu %s passenden Einträge:

# INVITE responses
CHAN_INVITE_SYNTAX
	INVITE Raum
CHAN_INVITE_OK
	Lade %s zum Raum %s ein.

# OP/DEOP/VOICE/DEVOICE responses
CHAN_OPVOICE_SYNTAX
	%s Raum [Chatname]
CHAN_OP_SUCCEEDED
	%s wurde im Raum %s geopt.
CHAN_OP_ALREADY
	%s ist bereits Op im Raum %s.
CHAN_OP_FAILED
	Konnte %s im Raum %s nicht oppen.
CHAN_DEOP_SUCCEEDED
	%s wurde im Raum %s gedeopt.
CHAN_DEOP_ALREADY
	%s ist kein Op im Raum %s.
CHAN_DEOP_FAILED
	Konnte %s im Raum %s nicht deoppen.
CHAN_VOICE_SUCCEEDED
	%s erhielt im Raum %s ein Voice.
CHAN_VOICE_ALREADY
	%s hat bereist Voice im Raum %s.
CHAN_VOICE_FAILED
	Konnte %s im Raum %s kein Voice geben.
CHAN_DEVOICE_SUCCEEDED
	%s verlor im Raum %s das Voice.
CHAN_DEVOICE_ALREADY
	%s hat im Raum %s keinen Voice.
CHAN_DEVOICE_FAILED
	Konnte %s im Raum %s nicht devoicen.
CHAN_HALFOP_SUCCEEDED
	%s wurde im Raum %s gehalfopt.
CHAN_HALFOP_ALREADY
	%s ist bereits HalfOp im Raum %s.
CHAN_HALFOP_FAILED
	Konnte %s im Raum %s nicht halfoppen.
CHAN_DEHALFOP_SUCCEEDED
	%s verlor im Raum %s ein Halfop.
CHAN_DEHALFOP_ALREADY
	%s ist kein HalfOp im Raum %s.
CHAN_DEHALFOP_FAILED
	Konnte %s im Raum %s nicht dehalfoppen.
CHAN_PROTECT_SUCCEEDED
	%s wurde im Raum %s geschützt.
CHAN_PROTECT_ALREADY
	%s ist bereits im Raum %s geschützt.
CHAN_PROTECT_FAILED
	Konnte %s im Raum %s nicht schützen.
CHAN_DEPROTECT_ALREADY
	%s ist im Raum %s nicht geschützt.
CHAN_DEPROTECT_SUCCEEDED
	%s verlor im Raum %s den Schutz.
CHAN_DEPROTECT_FAILED
	Konnte %s im Raum %s nicht deprotecten.

# UNBAN responses
CHAN_UNBAN_SYNTAX
	UNBAN channel
CHAN_UNBANNED
	Bann auf Sie im Raum %s wurde behoben.

# KICK responses
CHAN_KICK_SYNTAX
	KICK Raum Chatname [Grund]
CHAN_KICK_PROTECTED
	%s kann vom Raum %s nicht gekickt werden.
CHAN_KICKED
	%s wurde vom Raum %s gekickt.

# TOPIC responses
CHAN_TOPIC_SYNTAX
	TOPIC Raum Thema

# CLEAR responses
CHAN_CLEAR_SYNTAX
	CLEAR Raum Eintragtyp
CHAN_CLEARED_BANS
	Alle bans (+b) vom Raum %s wurden aufgehoben.
CHAN_CLEARED_EXCEPTIONS
	Alle Exceptions vom Raum %s wurden zurückgesetzt.
CHAN_CLEARED_INVITES
	Alle Einladungsmasken vom Raum %s wurden entfernt.
CHAN_CLEARED_MODES
	Alle Modi vom Raum %s wurden zurückgesetzt.
CHAN_CLEARED_OPS
	Der Modus +o vom Raum %s wurde entfernt (massdeop).
CHAN_CLEARED_HALFOPS
	Der Modus +h vom Raum %s wurden entfernt (massdehalfop).
CHAN_CLEARED_VOICES
	Der Modus +v vom Raum %s wurden entfernt (massdevoice).
CHAN_CLEARED_USERS
	Alle Benutzer wurden aus dem Raum %s gekickt (masskick).

# GETPASS responses
CHAN_GETPASS_SYNTAX
	GETPASS Raum
### CHANGED: channel name parameter inserted
#CHAN_GETPASS_UNAVAILABLE
#	Aufgrund der Nutzung von Verschlüsselung ist der GETPASS-Befehl ist nicht verfügbar.
CHAN_GETPASS_PASSWORD_IS
	Das Passwort für Raum %s lautet %s.

# FORBID responses
CHAN_FORBID_SYNTAX
	FORBID Raum
CHAN_FORBID_SHORT_CHANNEL
	Der Raum # kann nicht verboten werden.
CHAN_FORBID_SUCCEEDED
	Der Zugang zu Raum %s ist ab sofort verboten.
CHAN_FORBID_FAILED
	Der Zugang zu Raum %s kann nicht verboten werden!

# SUSPEND responses
CHAN_SUSPEND_SYNTAX
	SUSPEND [+Dauer] Raum Grund
CHAN_SUSPEND_SUCCEEDED
	Der Raum %s wurde suspendiert.
CHAN_SUSPEND_ALREADY_SUSPENDED
	Der Raum %s ist bereits suspendiert.

# UNSUSPEND responses
CHAN_UNSUSPEND_SYNTAX
	UNSUSPEND Raum
CHAN_UNSUSPEND_SUCCEEDED
	Suspend von %s ist jetzt aufgehoben.
CHAN_UNSUSPEND_NOT_SUSPENDED
	Der Raum %s ist nicht suspendiert.

###########################################################################
#
# MemoServ messages
#
###########################################################################

# Automatic messages
MEMO_HAVE_NEW_MEMO
	Sie haben ein neues Memo.
MEMO_HAVE_NEW_MEMOS
	Sie haben %d neue Memos.
MEMO_TYPE_READ_LAST
	Zum Lesen /msg %s READ LAST eingeben.
MEMO_TYPE_READ_NUM
	Zum Lesen /msg %s READ %d eingeben.
MEMO_TYPE_LIST_NEW
	Zum auflisten /msg %s LIST NEW eingeben.
MEMO_AT_LIMIT
	Warnung: Sie haben Ihr Limit an Memos erreicht (%d). Sie werden keine weiteren Memos empfangen können, solange Sie nicht einige Ihrer alten löschen.
MEMO_OVER_LIMIT
	Warnung: Sie haben Ihr Limit an Memos überschritten (%d). Sie werden keine weiteren Memos empfangen können, solange Sie nicht einige Ihrer alten löschen.
MEMO_NEW_MEMO_ARRIVED
	Sie haben ein neues Memo von %s.
	Zum Lesen /msg %s READ %d eingeben.

# Multi-use responses
MEMO_HAVE_NO_MEMOS
	Sie haben keine Memos.
MEMO_HAVE_NO_NEW_MEMOS
	Sie haben keine neuen Memos.
MEMO_DOES_NOT_EXIST
	Memo %d existiert nicht!
MEMO_LIST_NOT_FOUND
	Es wurde kein zutreffendes Memo gefunden.

# SEND responses
MEMO_SEND_SYNTAX
	SEND {Chatname | Raum} Memo-Text
MEMO_SEND_DISABLED
	Das Versenden von Memos ist temporär nicht verfügbar.
MEMO_SEND_PLEASE_WAIT
	Bitte warten Sie %s, bevor Sie den SEND-Befehl erneut benutzen.
MEMO_X_GETS_NO_MEMOS
	%s kann keine Memos empfangen.
MEMO_X_HAS_TOO_MANY_MEMOS
	%s hat momentan zu viele Memos und kann daher keine weiteren empfangen.
MEMO_SENT
	Memo wurde an %s gesendet.

# LIST responses
MEMO_LIST_SYNTAX
	LIST [Nummer | Liste | NEW]
MEMO_LIST_MEMOS
	Memos für %s. Zum Lesen /msg %s READ Nummer eingeben.
MEMO_LIST_NEW_MEMOS
	Neue Memos für %s. Zum Lesen /msg %s READ Nummer eingeben.
MEMO_LIST_HEADER
	   Nummer  Sender            Datum/Uhrzeit
MEMO_LIST_FORMAT
	%c%c%c %3d    %-16s  %s

# READ responses
MEMO_READ_SYNTAX
	READ {Nummer | Liste | LAST | NEW}
MEMO_HEADER
	Memo %d von %s (%s). Zum Löschen /msg %s DEL %d eingeben.
#MEMO_CHAN_HEADER
#	Memo %d von %s (%s). Zum Löschen /msg %s DEL %s %d eingeben.

# SAVE responses
MEMO_SAVE_SYNTAX
	SAVE {Nummer | Liste}
MEMO_SAVED_ONE
	Memo %d wird nicht verfallen.
MEMO_SAVED_SEVERAL
	%d Memos werden nicht verfallen.

# DEL responses
MEMO_DEL_SYNTAX
	DEL {Nummer | Liste | ALL}
MEMO_DELETED_NONE
	Es wurden keine Memos gelöscht.
MEMO_DELETED_ONE
	Memo %d wurde gelöscht.
MEMO_DELETED_SEVERAL
	%d Memos wurden gelöscht.
MEMO_DELETED_ALL
	Alle Memos wurden gelöscht.

# SET responses
MEMO_SET_SYNTAX
	SET Option Parameter
MEMO_SET_DISABLED
	Der MemoServ SET-Befehl ist temporär nicht verfügbar.
MEMO_SET_UNKNOWN_OPTION
	Unbekannte SET-Option %s.

# SET NOTIFY responses
MEMO_SET_NOTIFY_SYNTAX
	SET NOTIFY {ON | LOGON | NEW | OFF}
MEMO_SET_NOTIFY_ON
	Sie werden von %s über neue Memos informiert, wenn Sie sich anmelden und wenn neue Memos empfangen werden.
MEMO_SET_NOTIFY_LOGON
	Sie werden von %s über neue Memos informiert, wenn Sie sich anmelden und wenn Sie den AWAY-Status entnehmen.
MEMO_SET_NOTIFY_NEW
	Sie werden von %s über neue Memos informiert, wenn neue Memos empfangen werden.
MEMO_SET_NOTIFY_OFF
	Sie werden von %s über neue Memos nicht informiert.

# SET LIMIT responses
MEMO_SET_LIMIT_SYNTAX
	SET LIMIT Limit
MEMO_SET_LIMIT_OPER_SYNTAX
	SET LIMIT [Chatname] {Limit | NONE | DEFAULT } [HARD]
MEMO_SET_YOUR_LIMIT_FORBIDDEN
	Sie dürfen Ihr Memo-Limit nicht ändern.
MEMO_SET_LIMIT_FORBIDDEN
	Das Memo-Limit für %s darf nicht geändert werden.
MEMO_SET_YOUR_LIMIT_TOO_HIGH
	Sie können Ihr Memo-Limit nicht größer als %d setzen.
MEMO_SET_LIMIT_TOO_HIGH
	Sie können das Memo-Limit für %s größer als %d setzen.
MEMO_SET_LIMIT_OVERFLOW
	Das Memo-Limit ist zu groß; es wird statt dessen auf %d begrenzt.
MEMO_SET_YOUR_LIMIT
	Ihr Memo-Limit wurde auf %d gesetzt.
MEMO_SET_YOUR_LIMIT_ZERO
	Sie können keine Memos mehr empfangen (Memo-Limit = 0).
MEMO_SET_YOUR_LIMIT_DEFAULT
	Ihr Memo-Limit wurde auf Standardwert (%d) gesetzt.
MEMO_UNSET_YOUR_LIMIT
	Ihr Memo-Limit wurde ausgeschaltet.
MEMO_SET_LIMIT
	Das Memo-Limit für %s wurde auf %d gesetzt.
MEMO_SET_LIMIT_ZERO
	Das Memo-Limit für %s wurde auf 0 gesetzt.
MEMO_SET_LIMIT_DEFAULT
	Das Memo-Limit für %s wurde auf Standardwert (%d) gesetzt.
MEMO_UNSET_LIMIT
	Das Memo-Limit für %s wurde ausgeschaltet.

# INFO responses
MEMO_INFO_NO_MEMOS
	Momentan haben Sie keine Memos.
MEMO_INFO_MEMO
	Momentan haben Sie 1 Memo.
MEMO_INFO_MEMO_UNREAD
	Momentan haben Sie 1 ungelesenes Memo.
MEMO_INFO_MEMOS
	Momentan haben Sie %d Memos.
MEMO_INFO_MEMOS_ONE_UNREAD
	Momentan haben Sie %d Memos (1 ungelesenes).
MEMO_INFO_MEMOS_SOME_UNREAD
	Momentan haben Sie %d Memos (%d ungelesene).
MEMO_INFO_MEMOS_ALL_UNREAD
	Momentan haben Sie %d Memos (alle ungelesen).
MEMO_INFO_LIMIT
	Ihr Memo-Limit ist %d.
MEMO_INFO_HARD_LIMIT
	Ihr Memo-Limit ist %d, es darf nicht geändert werden.
MEMO_INFO_LIMIT_ZERO
	Ihr Memo-Limit ist 0; Sie werden keine neuen Memos empfangen können.
MEMO_INFO_HARD_LIMIT_ZERO
	Ihr Memo-Limit ist 0; Sie werden keine neuen Memos empfangen können. Sie können den Limit nicht ändern.
MEMO_INFO_NO_LIMIT
	Sie haben kein Limit für die Anzahl Ihrer Memos.
MEMO_INFO_NOTIFY_OFF
	Sie werden über neue Memos nicht informiert.
MEMO_INFO_NOTIFY_ON
	Sie werden über neue Memos informiert, wenn Sie sich anmelden und wenn neue Memos empfangen werden.
MEMO_INFO_NOTIFY_RECEIVE
	Sie werden über neue Memos informiert, wenn neue Memos empfangen werden.
MEMO_INFO_NOTIFY_SIGNON
	Sie werden über neue Memos informiert, wenn Sie sich anmelden.
MEMO_INFO_X_NO_MEMOS
	%s hat keine Memos.
MEMO_INFO_X_MEMO
	%s hat 1 Memo.
MEMO_INFO_X_MEMO_UNREAD
	%s hat 1 nicht gelesenes Memo.
MEMO_INFO_X_MEMOS
	%s hat %d Memos.
MEMO_INFO_X_MEMOS_ONE_UNREAD
	%s hat %d Memos (1 ungelesenes).
MEMO_INFO_X_MEMOS_SOME_UNREAD
	%s hat %d Memos (%d ungelesene).
MEMO_INFO_X_MEMOS_ALL_UNREAD
	%s hat %d Memos (alle ungelesen).
MEMO_INFO_X_LIMIT
	Memo-Limit von %s: %d.
MEMO_INFO_X_HARD_LIMIT
	Memo-Limit von %s: %d. Das Limit kann nicht geändert werden.
MEMO_INFO_X_NO_LIMIT
	%s hat kein Memo-Limit.
MEMO_INFO_X_NOTIFY_OFF
	%s wird über neue Memos nicht informiert.
MEMO_INFO_X_NOTIFY_ON
	%s wird über neue Memos informiert, wenn er sich anmeldet und wenn neue Memos empfangen werden.
MEMO_INFO_X_NOTIFY_RECEIVE
	%s wird über neue Memos informiert, wenn neue Memos empfangen werden.
MEMO_INFO_X_NOTIFY_SIGNON
	%s wird über neue Memos informiert, wenn er sich anmeldet.

################ forward module messages/responses
# Mail format
MEMO_FORWARD_MAIL_SUBJECT
	Memo für %s
MEMO_FORWARD_MAIL_BODY
	Memo von %s (%s)
	%s

# FORWARD responses
MEMO_FORWARD_SYNTAX
	FORWARD {Nummer | Liste | ALL}
MEMO_FORWARD_NEED_EMAIL
	Sie müssen eine E-Mail-Adresse angeben bevor Sie sich Memos weiterleiten lassen können.
MEMO_FORWARD_PLEASE_WAIT
	Bitte warten Sie %s, bevor Sie den FORWARD-Befehl erneut benutzen.
MEMO_FORWARD_FAILED
	Weiterleiten fehlgeschlagen.
MEMO_FORWARDED_NONE
	Keine Memos wurden weitergeleitet.
MEMO_FORWARDED_ONE
	Memo %d wurde weitergeleitet.
MEMO_FORWARDED_SEVERAL
	%d Memos wurden weitergeleitet.
MEMO_FORWARDED_ALL
	Alle Ihre Memos wurden weitergeleitet.

# SET FORWARD responses
MEMO_SET_FORWARD_SYNTAX
	SET FORWARD {ON | COPY | OFF}
MEMO_SET_FORWARD_ON
	Ihre Memos werden an Ihre E-Mail-Adresse %s weitergeleitet.
MEMO_SET_FORWARD_COPY
	Ihre Memos werden an Ihre E-Mail-Adresse %s weitergeleitet und gespeichert durch die Services.
MEMO_SET_FORWARD_OFF
	Ihre Memos werden nicht weitergeleitet.

################ ignore module messages/responses

# IGNORE responses
MEMO_IGNORE_SYNTAX
	IGNORE {ADD | DEL | LIST} [Chatname | Maske]
MEMO_IGNORE_ADD_SYNTAX
	IGNORE ADD Chatname | Maske
MEMO_IGNORE_DEL_SYNTAX
	IGNORE DEL Chatname | Maske
MEMO_IGNORE_LIST_FULL
	Ihre Ignore-Liste ist voll.
MEMO_IGNORE_ALREADY_PRESENT
	%s steht bereits auf Ihrer Ignore-Liste.
MEMO_IGNORE_ADDED
	%s wurde zu Ihrer Ignore-Liste hinzugefügt.
MEMO_IGNORE_NOT_FOUND
	%s wurde in Ihrer Ignore-Liste nicht gefunden.
MEMO_IGNORE_DELETED
	%s wurde von Ihrer Ignore-Liste gelöscht.
MEMO_IGNORE_LIST_EMPTY
	Ihre Ignore-Liste ist leer.
MEMO_IGNORE_LIST
	Ignore-Liste:
MEMO_IGNORE_LIST_X_EMPTY
	Die Ignore-Liste von %s ist leer.
MEMO_IGNORE_LIST_X
	Ignore-Liste von %s:

###########################################################################
#
# OperServ messages
#
###########################################################################

# General messages
OPER_BOUNCY_MODES
	Services kann keine Modi setzen, sind Ihre Servers richtig konfiguriert?
OPER_BOUNCY_MODES_U_LINE
	Services kann keine Modi setzen, überprüfen Sie die U-Lines der Servers.

# GLOBAL responses
OPER_GLOBAL_SYNTAX
	GLOBAL Nachricht

# STATS responses
OPER_STATS_UNKNOWN_OPTION
	Unbekannte STATS-Option %s.
OPER_STATS_RESET_USER_COUNT
	Die maximale Anzahl der Benutzer ist zurückgesetzt.

OPER_STATS_CURRENT_USERS
	Momentane Benutzer: %d (%d Ops)
OPER_STATS_MAX_USERS
	Maximum an Benutzern: %d (%s)
# Parameters: days, STR_DAYS or STR_DAY, hours, minutes
OPER_STATS_UPTIME_DHM
	Services up %d%s, %02d:%02d
# If >= 1 hour, hours and minutes; if < 1 hour, minutes and seconds
OPER_STATS_UPTIME_HM_MS
	Services up %s

OPER_STATS_KBYTES_READ
	Daten empfangen: %10u kB
OPER_STATS_KBYTES_WRITTEN
	Daten gesendet:  %10u kB
OPER_STATS_NETBUF_SOCK
	Server socket puffer: %5u kB
OPER_STATS_NETBUF_SOCK_PERCENT
	Server socket puffer: %5u kB (%d%%)
OPER_STATS_NETBUF_TOTAL
	Gesamt socket puffer: %5u kB
OPER_STATS_NETBUF_TOTAL_PERCENT
	Gesamt socket puffer: %5u kB (%d%%)

OPER_STATS_ALL_USER_MEM
	Benutzer         : %6d Datensätze, %5d kB.
OPER_STATS_ALL_CHANNEL_MEM
	Räume            : %6d Datensätze, %5d kB.
OPER_STATS_ALL_SERVER_MEM
	Server           : %6d Datensätze, %5d kB.
OPER_STATS_ALL_NICKGROUPINFO_MEM
	Chatnamengruppen : %6d Datensätze, %5d kB.
OPER_STATS_ALL_NICKINFO_MEM
	Chatnamen        : %6d Datensätze, %5d kB.
OPER_STATS_ALL_CHANSERV_MEM
	ChanServ         : %6d Datensätze, %5d kB.
OPER_STATS_ALL_STATSERV_MEM
	StatServ         : %6d Datensätze, %5d kB.
OPER_STATS_ALL_NEWS_MEM
	News             : %6d Datensätze, %5d kB.
OPER_STATS_ALL_AKILL_MEM
	AutoKills        : %6d Datensätze, %5d kB.
OPER_STATS_ALL_EXCEPTION_MEM
	Exceptions       : %6d Datensätze, %5d kB.
OPER_STATS_ALL_SGLINE_MEM
	SGlines          : %6d Datensätze, %5d kB.
OPER_STATS_ALL_SQLINE_MEM
	SQlines          : %6d Datensätze, %5d kB.
OPER_STATS_ALL_SZLINE_MEM
	SZlines          : %6d Datensätze, %5d kB.
OPER_STATS_ALL_SESSION_MEM
	Sitzungen        : %6d Datensätze, %5d kB.

# GETKEY responses
### CHANGED: OPER_GETKEY_* added

# MODE responses
OPER_MODE_SYNTAX
	MODE Raum Modi

# CLEARMODES responses
OPER_CLEARMODES_SYNTAX
	CLEARMODES Raum [ALL]
#OPER_CLEARMODES_DONE
#	Alle Modi und Bans im Raum %s wurden entfernt.
#OPER_CLEARMODES_ALL_DONE
#	Alle Modi im Raum %s wurden entfernt.

# CLEARCHAN responses
OPER_CLEARCHAN_SYNTAX
	CLEARCHAN Raum
OPER_CLEARCHAN_DONE
	Alle Benutzer wurden aus dem Raum %s gekickt (masskick).

# KICK responses
OPER_KICK_SYNTAX
	KICK Raum Benutzer Grund

# ADMIN responses
OPER_ADMIN_SYNTAX
	ADMIN {ADD | DEL | LIST} [Chatname]
OPER_ADMIN_ADD_SYNTAX
	ADMIN ADD Chatname
OPER_ADMIN_DEL_SYNTAX
	ADMIN DEL Chatname
OPER_ADMIN_NO_NICKSERV
	NickServ ist momentan deaktiviert. Der ADMIN-Befehl steht nicht zur Verfügung.
OPER_ADMIN_EXISTS
	%s steht bereits in der Services-Administratorenliste.
OPER_ADMIN_ADDED
	%s wurde zur Services-Administratorenliste hinzugefügt.
OPER_ADMIN_TOO_MANY
	Zu viele Einträge (%d) in der Services-Administratorenliste; weitere können nicht hinzugefügt werden.
OPER_ADMIN_REMOVED
	%s wurde aus der Services-Administratorenliste herausgenommen.
OPER_ADMIN_NOT_FOUND
	%s wurde in der Services-Administratorenliste nicht gefunden.
OPER_ADMIN_LIST_HEADER
	Services-Administratorenliste:

# OPER responses
OPER_OPER_SYNTAX
	OPER {ADD | DEL | LIST} [Chatname]
OPER_OPER_ADD_SYNTAX
	OPER ADD Chatname
OPER_OPER_DEL_SYNTAX
	OPER DEL Chatname
OPER_OPER_NO_NICKSERV
	NickServ ist momentan deaktiviert. Der OPER-Befehl steht nicht zur Verfügung.
OPER_OPER_EXISTS
	%s steht bereits in der Services-Operatorenliste.
OPER_OPER_ADDED
	%s wurde zur Services-Operatorenliste hinzugefügt.
OPER_OPER_TOO_MANY
	Zu viele Einträge (%d) in der Services-Operatorenliste; weitere können nicht hinzugefügt werden.
OPER_OPER_REMOVED
	%s wurde aus der Services-Operatorenliste herausgenommen.
OPER_OPER_NOT_FOUND
	%s wurde in der Services-Operatorenliste nicht gefunden.
OPER_OPER_LIST_HEADER
	Services-Operatorenliste:

# AKILL/EXCLUDE/SxLINE common responses
OPER_MASKDATA_SYNTAX
	%s {ADD | DEL | CLEAR | LIST | VIEW | CHECK | COUNT} [Parameter]
OPER_MASKDATA_ADD_SYNTAX
	%s ADD [+Dauer] Maske Grund
OPER_MASKDATA_DEL_SYNTAX
	%s DEL Maske
OPER_MASKDATA_CLEAR_SYNTAX
	%s CLEAR ALL
### CHANGED: OPER_MASKDATA_LIST_SYNTAX added
OPER_MASKDATA_CHECK_SYNTAX
	%s CHECK Maske
# mask, reason
OPER_MASKDATA_LIST_FORMAT
	%-32s  %s
## mask, set-by, set-time, last-used, expires, reason
OPER_MASKDATA_VIEW_FORMAT
	%s (durch %s am %s, Letzte Nutzung %s; %s)
	    %s
## mask, set-by, set-time, expires, reason
OPER_MASKDATA_VIEW_UNUSED_FORMAT
	%s (durch %s am %s, nie benutzt; %s)
	    %s
	
# AKILL responses
OPER_TOO_MANY_AKILLS
	Die Autokill Liste ist voll!
OPER_AKILL_EXISTS
	%s steht bereits auf der Autokill-Liste.
OPER_AKILL_NO_NICK
	Erinnerung: Autokill-Masken können keine Chatnamen enthalten.
OPER_AKILL_MASK_TOO_GENERAL
	Die Autokill-Maske ist zu allgemein.
OPER_AKILL_EXPIRY_LIMITED
	Die Dauer kann höchstens %s betragen.
OPER_AKILL_ADDED
	%s wurde in die Autokill-Liste eingetragen.
OPER_AKILL_REMOVED
	%s wurde von der Autokill-Liste gelöscht.
OPER_AKILL_NOT_FOUND
	%s wurde in der Autokill-Liste nicht gefunden.
OPER_AKILL_LIST_HEADER
	Derzeitige Autokill-Liste:
### CHANGED: OPER_AKILL_{LIST_EMPTY,LIST_NO_MATCH,CHECK_*} added
OPER_AKILL_COUNT
	Es gibt %d Einträge in der Autokill-Liste.

# AKILLCHAN responses
OPER_AKILLCHAN_SYNTAX
	AKILLCHAN [KILL] [+Dauer] Raum Grund
OPER_AKILLCHAN_AKILLED
	%d neue Einträge in der AKILL-Liste.
OPER_AKILLCHAN_KILLED
	%d Chatter entfernt.
OPER_AKILLCHAN_AKILLED_ONE
	1 neuer Eintrag in der AKILL-Liste.
OPER_AKILLCHAN_KILLED_ONE
	1 Chatter entfernt.

# EXCLUDE responses
OPER_TOO_MANY_EXCLUDES
	Die Autokill-Ausnahme-Liste ist voll!
OPER_EXCLUDE_EXISTS
	%s steht bereits auf der Autokill-Ausnahme-Liste.
OPER_EXCLUDE_ADDED
	%s wurde zur Autokill-Ausnahme-Liste hinzugefügt.
OPER_EXCLUDE_REMOVED
	%s wurde von der Autokill-Ausnahme-Liste gelöscht.
OPER_EXCLUDE_NOT_FOUND
	%s wurde auf der Autokill-Ausnahme-Liste nicht gefunden.
OPER_EXCLUDE_LIST_HEADER
	Aktuelle Autokill-Ausnahme-Liste:
### CHANGED: OPER_EXCLUDE_{LIST_EMPTY,LIST_NO_MATCH,CHECK_*} added
OPER_EXCLUDE_COUNT
	Es gibt %d Einträge in der Autokill-Ausnahme-Liste.

# SxLINE responses
OPER_TOO_MANY_SLINES
	%s-Liste ist voll!
OPER_SLINE_EXISTS
	%s gibt es bereits auf der %s-Liste.
OPER_SLINE_MASK_TOO_GENERAL
	Die %s-Maske ist zu allgemein.
OPER_SLINE_ADDED
	%s wurde in die %s-Liste eingetragen.
OPER_SLINE_REMOVED
	%s wurde von der %s-Liste gelöscht.
OPER_SLINE_NOT_FOUND
	%s wurde in der %s-Liste nicht gefunden.
OPER_SLINE_LIST_HEADER
	Derzeitige %s-Liste:
### CHANGED: OPER_SLINE_{LIST_EMPTY,LIST_NO_MATCH,CHECK_*} added
OPER_SLINE_COUNT
	Es gibt %d Einträge in der %s-Liste.
OPER_SZLINE_NOT_AVAIL
	SZLINE wird von dem Server nicht unterstützt.

# SU responses
OPER_SU_SYNTAX
	SU Passwort
OPER_SU_NO_PASSWORD
	Der SU-Befehl kann nicht ausgeführt werden, weil kein SU-Passwort gesetzt wurde.
OPER_SU_SUCCEEDED
	Sie haben jetzt Services-Root Rechte.
OPER_SU_FAILED
	Kann das Passwort nicht verifizieren.

# SET responses
OPER_SET_SYNTAX
	SET Option Parameter
OPER_SET_IGNORE_ON
	Ignore AKTIV.
OPER_SET_IGNORE_OFF
	Ignore INAKTIV.
OPER_SET_IGNORE_ERROR
	IGNORE sollte entweder ON oder OFF sein.
OPER_SET_READONLY_ON
	Services momentan im read-only-Modus.
OPER_SET_READONLY_OFF
	Services momentan im read-write-Modus.
OPER_SET_READONLY_ERROR
	READONLY sollte entweder ON oder OFF sein.
OPER_SET_DEBUG_ON
	Services momentan im Debug-Modus.
OPER_SET_DEBUG_OFF
	Services momentan nicht im Debug-Modus.
OPER_SET_DEBUG_LEVEL
	Services momentan im Debug-Modus (Level %d).
OPER_SET_DEBUG_ERROR
	DEBUG sollte entweder ON, OFF oder eine positive Zahl sein.
OPER_SET_SUPASS_FAILED
	Kann das SU-Passwort der Services nicht setzen.
OPER_SET_SUPASS_OK
	Das SU-Passwort der Services wurde gesetzt.
OPER_SET_SUPASS_NONE
	Das SU-Passwort der Services wurde deaktiviert.
OPER_SET_UNKNOWN_OPTION
	Unbekannte Option: %s.

# JUPE responses
OPER_JUPE_SYNTAX
	JUPE Servername [Grund]
OPER_JUPE_INVALID_NAME
	Der Servername ist ungültig. Der Name muss einen Punkt enthalten.
OPER_JUPE_ALREADY_JUPED
	Der Server %s ist bereits ein "Jupiter"

# RAW responses
OPER_RAW_SYNTAX
	RAW Text

# UPDATE responses
OPER_UPDATE_SYNTAX
	UPDATE [FORCE]
OPER_UPDATE_FORCE_FAILED
	Unable to unlock databases.
OPER_UPDATING
	Aktualisiere Datenbanken...
OPER_UPDATE_COMPLETE
	Datenbankaktualisierung abgeschlossen.
OPER_UPDATE_FAILED
	Datenbankaktualisierung fehlgeschlagen.

# RESTART responses

# REHASH responses
OPER_REHASHING
	Die Konfiguration wird erneut gelesen.
OPER_REHASHED
	Die Konfiguration wurde erfolgreich eingelesen.
OPER_REHASH_ERROR
	Ein Fehler ist beim Lesen der Konfiguration eingetreten. Lesen Sie die Log-Datei für detaillierte Informationen.

# KILLCLONES responses
OPER_KILLCLONES_SYNTAX
	KILLCLONES Chatname
OPER_KILLCLONES_UNKNOWN_NICK
	Der User %s wurde nicht gefunden.
OPER_KILLCLONES_KILLED
	%s nutzte KILLCLONES auf %s. %d Clone wurden aus dem Chat entfernt.
OPER_KILLCLONES_KILLED_AKILL
	%s nutzte KILLCLONES auf %s. %d Clone wurden aus dem Chat entfernt. Eine temporäre AKILL auf %s wurde hinzugefügt.


###########################################################################
#
# Session limiting and exception messages
#
###########################################################################

# EXCEPTION responses
OPER_EXCEPTION_SYNTAX
	EXCEPTION {ADD | DEL | CLEAR | MOVE | LIST | VIEW | CHECK | COUNT} [Parameter]
OPER_EXCEPTION_ADD_SYNTAX
	EXCEPTION ADD [+Dauer] Maske Limit Grund
OPER_EXCEPTION_DEL_SYNTAX
	EXCEPTION DEL {Maske | Liste}
OPER_EXCEPTION_MOVE_SYNTAX
	EXCEPTION MOVE Nummer Neue-Nummer
OPER_EXCEPTION_ALREADY_PRESENT
	Mask %s ist schon in der Sitzungs-Limit-Liste eingetragen.
OPER_EXCEPTION_TOO_MANY
	Ausnahme-Liste ist voll.
OPER_EXCEPTION_ADDED
	Sitzungs Limit für %s auf %d gesetzt.
OPER_EXCEPTION_MOVED
	Ausnahme für %s (%d.) wurde auf Position %d geschoben.
OPER_EXCEPTION_NO_SUCH_ENTRY
	Der Eintrag (%d.) konnte in der Sitzung-Limit-Liste nicht gefunden werden.
OPER_EXCEPTION_NOT_FOUND
	%s konnte in der Sitzungs-Limit-Liste nicht gefunden werden.
OPER_EXCEPTION_NO_MATCH
	Keine passenden Einträge in der Ausnahme-Liste
OPER_EXCEPTION_EMPTY
	Sitzungs-Limit-Liste ist leer.
OPER_EXCEPTION_DELETED
	%s wurde aus der Sitzungs-Limit-Liste gelöscht.
OPER_EXCEPTION_DELETED_ONE
	Ein Eintrag wurde aus der Sitzungs-Limit-Liste gelöscht.
OPER_EXCEPTION_DELETED_SEVERAL
	%d Einträge wurden aus der Sitzungs-Limit-Liste gelöscht.
OPER_EXCEPTION_LIST_HEADER
	Derzeitige Sitzungslimit-Ausnahmen-Liste:
# mask, limit
OPER_EXCEPTION_LIST_FORMAT
	%3d  %4d   %s
OPER_EXCEPTION_LIST_COLHEAD
	Num  Limit  Host
# mask, set-by, set-time, expires, limit, reason
OPER_EXCEPTION_VIEW_FORMAT
	%3d.  %s  (durch %s am %s; %s)
	    Limit: %-4d  - %s
OPER_EXCEPTION_INVALID_LIMIT
	Falsche Limitangabe. Limit muss eine Zahl grössergleich Null und und nicht größer als %d sein.
OPER_EXCEPTION_INVALID_HOSTMASK
	Falsche Hostmaske. Nur Hostmasken werden als gültig interpretiert, keine UserID oder Chatnamen.

# SESSION responses
OPER_SESSION_SYNTAX
	SESSION {LIST Limit | VIEW Host}
OPER_SESSION_LIST_SYNTAX
	SESSION LIST Limit
OPER_SESSION_VIEW_SYNTAX
	SESSION VIEW Host
OPER_SESSION_INVALID_THRESHOLD
	Falscher Limit: der Wert muss größer 1 sein.
OPER_SESSION_NOT_FOUND
	%s wurde in der Sitzungsliste nicht gefunden.
OPER_SESSION_LIST_HEADER
	Hosts mit mindestens %d Sitzungen:
OPER_SESSION_LIST_COLHEAD
	Sitzungen  Host
# limit, host
OPER_SESSION_LIST_FORMAT
	%6d    %s
# host, sessions, limit
OPER_SESSION_VIEW_FORMAT
	Der Host %s hat momentan %d Sitzungen mit einem Limit von %d.


###########################################################################
#
# News system messages
#
###########################################################################

NEWS_LOGON_TEXT
	[Nachrichten - %s] %s
NEWS_OPER_TEXT
	[Oper-Nachrichten - %s] %s

NEWS_LOGON_SYNTAX
	LOGONNEWS {ADD | DEL | LIST} [Text | Nummer]
NEWS_LOGON_LIST_HEADER
	Logon-News-Einträge:
NEWS_LOGON_LIST_ENTRY
	%5d (%s von %s)
	    %s
NEWS_LOGON_LIST_NONE
	Es gibt keine Logon-Nachrichten.
NEWS_LOGON_ADD_SYNTAX
	LOGONNEWS ADD Text
NEWS_LOGON_ADD_FULL
	Logon-Nachrichtenliste ist voll!
NEWS_LOGON_ADDED
	Neue Logon-Nachricht eingetragen (%d.).
NEWS_LOGON_DEL_SYNTAX
	LOGONNEWS DEL {Nummer | ALL}
NEWS_LOGON_DEL_NOT_FOUND
	Logon-Nachricht (%d.) konnte nicht gefunden werden!
NEWS_LOGON_DELETED
	Logon-Nachricht (%d.) wurde gelöscht.
NEWS_LOGON_DEL_NONE
	Keine Logon-Nachrichten zum Löschen sind da!
NEWS_LOGON_DELETED_ALL
	Alle Nachrichten sind gelöscht.

NEWS_OPER_SYNTAX
	OPERNEWS {ADD | DEL | LIST} [Text | Nummer]
NEWS_OPER_LIST_HEADER
	Oper-News-Nachrichten:
NEWS_OPER_LIST_ENTRY
	%5d (%s von %s)
	    %s
NEWS_OPER_LIST_NONE
	Es gibt keine Oper-News-Nachrichten.
NEWS_OPER_ADD_SYNTAX
	OPERNEWS ADD Text
NEWS_OPER_ADD_FULL
	Oper-News-Nachrichtenliste ist voll!
NEWS_OPER_ADDED
	Neue Oper-News-Nachricht eingetragen (%d.).
NEWS_OPER_DEL_SYNTAX
	OPERNEWS DEL {Nummer | ALL}
NEWS_OPER_DEL_NOT_FOUND
	Oper Oper-News-Nachricht (%d.) konnte nicht gefunden werden!
NEWS_OPER_DELETED
	Oper Oper-News-Nachricht (%d.) wurde gelöscht.
NEWS_OPER_DEL_NONE
	Keine Oper-News-Nachrichten zum löschen!
NEWS_OPER_DELETED_ALL
	Alle Oper-News-Nachrichten sind gelöscht.

NEWS_HELP_LOGON
	Syntax: LOGONNEWS ADD Text
	        LOGONNEWS DEL {Nummer | ALL}
	        LOGONNEWS LIST
	
	Bearbeitet die Liste der Logon-Nachrichten. Wenn ein Benutzer
	sich mit dem Server verbindet, werden diese Nachrichten an ihm
	geschickt.  (Um ein Flood zu vermeiden werden nicht mehr als
	3 Nachrichten gleichzeitig geschickt: wenn es mehr als 3
	Nachrichten gibt, werden nur die neuesten 3 geschickt.)
	
	LOGONNEWS LIST kann jeder Oper benutzen, um die Nachrichten zu
	sehen, ADD und DEL können nur die Services-Operatoren benutzen.
	
NEWS_HELP_OPER
	Syntax: OPERNEWS ADD Text
	        OPERNEWS DEL {Nummer | ALL}
	        OPERNEWS LIST
	
	Bearbeitet die Liste der Oper-Nachrichten. Wenn ein Benutzer
	Oper wird (Befehl /oper), erhält er dann diese Nachrichten.
	(Um ein Flood zu vermeiden werden nicht mehr als 3 Nachrichten
	gleichzeitig geschickt:, wenn es mehr als 3 Nachrichten gibt,
	werden nur die neuesten 3 geschickt.)
	
	OPERNEWS LIST kann jeder Oper benutzen, um die Nachrichten zu
	sehen, ADD und DEL können nur die Services-Operatoren benutzen.


###########################################################################
#
# StatServ messages
#
############################################################################

# General SERVERS responses
STAT_SERVERS_REMOVE_SERV_FIRST
	Server %s ist online. Entfernen Sie ihn, und versuchen dann erneut.
STAT_SERVERS_SERVER_EXISTS
	Server %s gibt es bereits.

# SERVERS command syntax
STAT_SERVERS_SYNTAX
	SERVERS Option Parameter

# STATS responses
STAT_SERVERS_STATS_TOTAL
	 Server gesamt: %2d
STAT_SERVERS_STATS_ON_OFFLINE
	        Online: %2d (%d%%)  Offline: %2d (%d%%)
STAT_SERVERS_LASTQUIT_WAS
	  Letzter Quit: %-30s  %s

# LIST responses
STAT_SERVERS_LIST_HEADER
	Server Name                    Users      Opers
STAT_SERVERS_LIST_FORMAT
	%-30s %3d (%2d%%)  %3d (%2d%%)
STAT_SERVERS_LIST_RESULTS
	Ende der Liste -- %d von %d Server wurden angezeigt.

# VIEW responses
STAT_SERVERS_VIEW_HEADER_ONLINE
	%-30s  (online)
STAT_SERVERS_VIEW_HEADER_OFFLINE
	%-30s  (offline)
STAT_SERVERS_VIEW_LASTJOIN
	  Letzter Join: %s
STAT_SERVERS_VIEW_LASTQUIT
	  Letzter Quit: %s
STAT_SERVERS_VIEW_QUITMSG
	Quit-Nachricht: %s
STAT_SERVERS_VIEW_USERS_OPERS
	        Users: %3d (%2d%%)  Opers: %3d (%2d%%)
STAT_SERVERS_VIEW_RESULTS
	Ende der Liste -- %d von %d Server wurden angezeigt.

# DELETE responses
STAT_SERVERS_DELETE_SYNTAX
	SERVERS DELETE Server
STAT_SERVERS_DELETE_DONE
	Statistiken für %s wurden gelöscht.

# COPY responses
STAT_SERVERS_COPY_SYNTAX
	SERVERS COPY Server NeuerName
STAT_SERVERS_COPY_DONE
	Statistiken für %s sind auf %s kopiert.

# RENAME responses
STAT_SERVERS_RENAME_SYNTAX
	SERVERS RENAME Server NeuerName
STAT_SERVERS_RENAME_DONE
	Statistiken für Server %s sind zu %s umbenannt.

# USERS responses
STAT_USERS_SYNTAX
	USERS STATS
STAT_USERS_TOTUSERS
	                 Benutzer gesamt: %d
STAT_USERS_TOTOPERS
	               Operatoren gesamt: %d
STAT_USERS_SERVUSERS
	  Durchschn. Benutzer pro Server: %d
STAT_USERS_SERVOPERS
	Durchschn. Operatoren pro Server: %d.%d


###########################################################################
#
# NickServ help messages
#
###########################################################################

NICK_HELP
	%S ermöglicht es Ihnen einen Chatnamen zu registrieren,
	so dass es für andere unmöglich wird, diesen Chatnamen zu benutzen.
	%S versteht verschiedene Befehle für die Registrierung und Pflege
	der Chatnamen. Um eine Liste der verfügbaren Befehle zu bekommen,
	tippen Sie /msg %S HELP COMMANDS, um einen Befehl zu Nutzen,
	tippen Sie /msg %S Befehl, für Informationen über den Befehl
	tippen Sie /msg %S HELP Befehl
NICK_HELP_EXPIRES
	
	Wenn ein Chatname %s lang nicht benutzt wird, dann wird der
	Eintrag gelöscht. Die Registrierung verfällt somit und der Chatname
	wird für andere wieder frei.
NICK_HELP_WARNING
	
	WICHTIG: Dieser Service ist nur dazu da, im IRC einen gewissen
	Schutz für Identität der Personen zu gewährleisten. Es ist nicht
	dazu da, um anderen Usern die Chatnamen zu stehlen. %S darf
	nicht für andere Zwecke benutzt werden, und falls so etwas
	entdeckt werden sollte, kann es dazu führen, dass Chatnamen
	und Räume unter Ihrem Namen verboten werden.

NICK_HELP_COMMANDS
	%S versteht folgende Befehle:
	
	    REGISTER     Registrieren des aktuellen Chatnamens.
	    IDENTIFY     Identifizierung des Chatnamens.
	    DROP         Löschen (freigeben) des Chatnamens.
#NICK_HELP_COMMANDS_AUTH
#	    AUTH         Authentifizieren der Chatnamenregistrierung.
#	    SENDAUTH     Verschickt den Authentifizierungscode an Sie.
NICK_HELP_COMMANDS_LINK
	    LINK         Link zu einem anderen Chatnamen machen (verbinden).
	    UNLINK       Link aufheben.
	    LISTLINKS    Alle Links anzeigen.
NICK_HELP_COMMANDS_ACCESS
	    ACCESS       Modifizieren der Zugriffsliste (erlaubte Hostadressen).
NICK_HELP_COMMANDS_AJOIN
	    AJOIN        Modifizieren der AJOIN-Liste.
NICK_HELP_COMMANDS_SET
	    SET          Setzen verschiedener Optionen. Auch Kill-Schutz.
	    UNSET        Zurücksetzen verschiedener Optionen.
	    RECOVER      Entfernt unbefugte Personen die Ihren Chatnamen benutzen.
	    GHOST        Trennt die nicht mehr aktive Verbindung eines Chatnamens.
	    INFO         Zeigt Information über den gegebenen Chatnamen.
NICK_HELP_COMMANDS_LIST
	    LIST         Zeigt alle Chatnamen, die auf die Suchmaske passen.
	    LISTEMAIL    Zeigt alle Chatnamen zur gegebenen E-Mail-Adresse.
NICK_HELP_COMMANDS_LISTCHANS
	    LISTCHANS    Zeigt alle Ihnen gehörende Räume.
	    STATUS       Zeigt, ob der User sich für den Chatnamen identifiziert hat.

NICK_HELP_REGISTER
	Syntax: %s
	
	Der Chatname wird in die %S-Datenbank eingetragen. Danach können
	Sie mit den Befehlen SET und ACCESS Ihre eigenen Optionen
	bestimmen. Das Passwort, das Sie beim Registrieren benutzt haben,
	sollten Sie nicht vergessen. Zu beachten ist auch die Groß- und
	Kleinschreibung.
NICK_HELP_REGISTER_EMAIL
	
	Sie können eine E-Mail-Adresse angeben, wenn Sie den Chatnamen
	registrieren. Mit dem Befehl SET EMAIL können Sie die
	E-Mail-Adresse später ändern.
NICK_HELP_REGISTER_EMAIL_REQ
	
	Bei der Registrierung müssen Sie eine E-Mail-Adresse angeben.
	Mit dem Befehl SET HIDE können Sie verhindern, dass diese von
	anderen gelesen wird. /msg %S HELP SET HIDE für weitere
	Informationen
NICK_HELP_REGISTER_EMAIL_AUTH
	
	Bei der Registrierung müssen Sie ihre gültige E-Mail-Adresse angeben.
	%S wird eine E-Mail mit dem Authentifizierungscode an diese E-Mail-Adresse
	verschicken. Diesen Code müssen Sie mit dem Befehl AUTH angeben,
	damit die Registrierung abgeschlossen wird. Mit dem Befehl SET HIDE
	können Sie verhindern, dass diese E-Mail-Adresse von anderen gelesen wird.
	/msg %S HELP SET HIDE für weitere Informationen.
NICK_HELP_REGISTER_END
	
	Vorschläge für das Passwort:
	
	Passworte sollen schwer zu raten sein. Das benutzen Ihres
	Realnamens ist z.B. eine schlechte Idee. Eine noch schlechtere
	ist die Nutzung des Chatnamens als Passwort. %S wird es nicht
	erlauben. Zu kurze Passworte sind durch das einfache Ausprobieren
	zu leicht zu knacken. Deshalb sollte Ihr Passwort mindestens
	5 Zeichen lang sein. Die Nutzung von Groß- und Kleinbuchstaben
	und Ziffern ist zu empfehlen.

NICK_HELP_IDENTIFY
	Syntax: IDENTIFY Passwort
	
	Dieser Befehl bewirkt, dass Sie vom %S erkannt werden. Es
	ist unbedingt erforderlich, um weitere Befehle zu benutzen.
	Das Passwort ist das selbe Passwort, das Sie beim Registrieren
	mit REGISTER benutzt haben.

NICK_HELP_DROP
	Syntax: DROP Passwort
	
	Ihr Chatname wird aus der %S-Datenbank gelöscht. Der Chatname
	wird für andere frei - kann von jemand anders Registriert werden.
NICK_HELP_DROP_LINK
	
	Bedenken Sie, dass wenn die einen Chatnamen löschen (deregistrieren),
	alle damit verbundenen Chatnamen ebenfalls gelöscht werden.
	Um einen verbundenen Chatnamen freizugeben, sollen sie den Befehl
	UNLINK benutzen.
NICK_HELP_DROP_END
	
	Um das versehentliche Löschen eines Chatnamens zu verhindern,
	wird bei dem Befehl das Passwort verlangt.

NICK_HELP_AUTH
	Syntax: AUTH Code
	
	Authentifiziert die Registrierung und/oder die Änderung der
	E-Mail-Adresse. Um zu prüfen, ob die E-Mail-Adresse korrekt ist,
	versendet %S eine E-Mail mit dem Authentifizierungscode an diese
	E-Mail-Adresse wenn sie einen Chatnamen registrieren oder die
	E-Mail-Adresse (Befehl SET EMAIL) ändern. Durch diesen Befehl
	wird sichergestellt, dass sie die E-Mail erhalten haben. Der
	Chatname erst nach der Authentifizierung benutzt werden. Der
	Code soll durch den ersetzt werden, den sie über E-Mail
	bekommen haben.

NICK_HELP_SENDAUTH
	Syntax: SENDAUTH
	
	Verschickt eine Kopie des Authentifizierungscode für Ihren
	Chatnamen nach REGISTER oder SET EMAIL. Dies ist dann
	nützlich, wenn sie die E-Mail mit dem Authentifizierungscode
	versehentlich gelöscht haben.
	
	Um Missbrauch vorzubeugen, kann man den Befehl nur einmal
	in 24 Stunden benutzen.

### CHANGED: last paragraph changed
#NICK_HELP_LINK
#	Syntax: LINK Chatname
#	
#	Erzeugt einen neuen Chatnamen und verbindet ihn mit Ihrem
#	aktuellen Chatnamen: ein "Alias" zu Ihrem aktuellen Chatnamen.
#	Der neue Chatname wird die Zugriffsrechte und Memos mit dem
#	aktuellen Chatnamen teilen: Memos, die an einen Chatnamen
#	geschickt werden, können Sie mit dem anderen Chatnamen lesen.
#	Wenn ein Chatname auf der Auto-Op-Liste eines Raumes ist,
#	werden sie auch zum Operator gemacht, wenn Sie den anderen
#	Chatnamen benutzen.
#	
#	Benutzen Sie den Befehl UNLINK, um die Verbindung aufzuheben.
#	
#	Um den Befehl zu benutzen, müssen Sie sich für den aktuellen
#	Chatnamen identifizieren (Befehl IDENTIFY). Der neue
#	Chatname darf in Benutzung sein.

NICK_HELP_UNLINK
	Syntax: UNLINK Chatname
	
	Die Verbindung (Link) zum anderen Chatnamen wird aufgehoben.
	Sie können ihren aktuellen Chatnamen nicht als Parameter angeben.
	Wenn sie Ihren aktuellen Chatnamen freigeben wollen, müssen Sie
	zuerst zu einem anderen verbundenen Chatnamen wechseln.
	
	Wenn Sie Ihren Hauptchatnamen freigeben (gesetzt durch den Befehl
	SET MAINNICK), wird Ihr aktueller Chatname automatisch zum
	Hauptchatnamen.

NICK_HELP_LISTLINKS
	Syntax: LISTLINKS
	
	Zeigt alle Chatnamen, die mit Ihrem Aktuellen Chatnamen verbunden
	sind. Der Hauptchatname wird durch ein Stern (*) vor dem Chatnamen
	markiert.

NICK_HELP_ACCESS
	Syntax: ACCESS ADD Hostmaske
	        ACCESS DEL Hostmaske
	        ACCESS LIST
	
	Die Zugriffsliste Ihres Chatnamen wird mit diesen Befehlen
	bearbeitet. Diese sind die Hostadressen, welche vom %S automatisch
	erkannt und akzeptiert werden. Falls sie sich von einer anderen
	Hostmaske verbinden, wird von Ihnen verlangt, den IDENTIFY-Befehl
	zu benutzen, damit Sie erkannt werden.
	
	Die Hostmaske darf Platzhalter (* oder ?) enthalten.
	
	Beispiele:
	
	    ACCESS ADD achurch@*.dragonfire.net
	        Der User achurch wird von allen Rechnern aus der
	        Domäne dragonfire.net akzeptiert.
	
	    ACCESS DEL achurch@*.dragonfire.net
	        Der vorherige Befehl wird rückgängig gemacht.
	
	    ACCESS LIST
	        Die aktuelle Zugriffsliste wird gezeigt.

NICK_HELP_SET
	Syntax: SET Option Parameter
	
	Verschiedene Chatnamen-Optionen werden gesetzt. Diese sind:
	
	    PASSWORD   Ändern Ihres Chatnamenpasswortes.
	    LANGUAGE   Bestimmt die Sprache, in der die Mitteilungen der
	               Services erscheinen.
	    URL        Ihrem Chatnamen-Eintrag eine URL hinzufügen.
	    EMAIL      Ihrem Chatnamen-Eintrag eine E-Mail-Adresse hinzufügen.
	    INFO       Ihrem Chatnamen-Eintrag einen Informationstext
	               hinzufügen (wird beim INFO-Befehl angezeigt).
	    KILL       Die Kill-Sicherheitsfunktion ein/auszuschalten.
	    SECURE     Um die Sicherheitsoption ein/auszuschalten.
	    PRIVATE    Um den Chatname auf jener Liste mit
	               /msg %S LIST zu verstecken.
	    HIDE       Um verschiedene Attribute in INFO-Anzeigen
	               zu verstecken.
	    TIMEZONE   Die Zeitzone bestimmen, in der Zeitangaben erscheinen.
NICK_HELP_SET_OPTION_MAINNICK
	    MAINNICK   Setzt den Hauptchatnamen für die Gruppe der
	               verbundenen (verlinkten) Chatnamen.
NICK_HELP_SET_END
	
	Um diese Befehle nutzen zu können, müssen Sie sich identifiziert
	haben.  /msg %S HELP IDENTIFY für weitere Informationen.
	
	/msg %S HELP SET Option für weitere Information einer
	bestimmten Option.

NICK_HELP_SET_PASSWORD
	Syntax: SET PASSWORD Neues-Passwort
	
	Das Passwort des Chatnamens wird dadurch neu gesetzt.

NICK_HELP_SET_LANGUAGE
	Syntax: SET LANGUAGE Nummer
	
	Bestimmt die Sprache, in der die Mitteilungen der Services
	erscheinen. Z.B. eine Antwort auf einen Befehl. Die Nummer
	muss einer der unterstützten Sprachen entsprechen:

NICK_HELP_SET_URL
	Syntax: SET URL URL
	
	Die angegebene URL wird zu dem Chatnamen-Eintrag zugefügt.
	Diese URL wird in den INFO-Anzeigen zu Ihrem Chatnamen
	(Befehl INFO) angezeigt.

NICK_HELP_SET_EMAIL
	Syntax: SET EMAIL E-Mail-Adresse
	
	Die angegebene E-Mail-Adresse wird zu dem Chatnamen-Eintrag
	zugefügt. Dies wird dadurch in den INFO-Anzeigen zu Ihrem
	Chatnamen (Befehl INFO) angezeigt.

NICK_HELP_SET_INFO
	Syntax: SET INFO Text
	
	Der angegebene Text wird zu dem Chatnamen-Eintrag zugefügt.
	Dieser Text wird in den INFO-Anzeigen zu Ihrem Chatnamen
	(Befehl INFO) angezeigt.

NICK_HELP_SET_KILL
	Syntax: SET KILL {ON | QUICK | IMMED | OFF}
	
	Die Kill Sicherheitsfunktion wird ein oder ausgeschaltet.
	Wenn eingeschaltet, dann wird es von jedem gewünscht, der
	Ihren Chatname benutzt, sich per Passwort zu identifizieren.
	Wenn dieser in einer Minute sich nicht identifiziert, oder
	den Chatnamen verändert, wird dessen IRC-Verbindung durch %S
	getrennt.
	
	Die QUICK Option bewirkt, dass diese Person statt
	in einer Minute, in 20 Sekunden das Passwort tippen muss,
	bevor seine Verbindung getrennt wird. Die Option IMMED
	bewirkt, dass der Chatter sofort vom IRC getrennt wird. Es
	gibt dann auch keine Warnung. Es ist davon abzuraten, diese
	Option einzusetzen. Außerdem kann der Administrator sie
	auch deaktiviert haben.

NICK_HELP_SET_SECURE
	Syntax: SET SECURE {ON | OFF}
	
	Die Sicherheits Optionen von %S werden ein/ausgeschaltet.
	Wenn eingeschaltet, wird es von Ihnen erwartet, das
	Passwort zu tippen, obwohl Ihre Hostmaske in der Access-Liste
	eingetragen sein kann. Wenn aber Ihre Hostmaske in Ihrer
	Zugriffsliste eingetragen ist, wird Ihre Verbindung zum IRC
	durch %S nicht getrennt - egal, welche Einstellung sie
	bei SET KILL vorgenommen haben.

NICK_HELP_SET_PRIVATE
	Syntax: SET PRIVATE {ON | OFF}
	
	Schaltet die Privatoption von %S ein und aus. Wenn die Option
	PRIVATE aktiv ist, wird Ihr Chatname nicht in den Listen
	angezeigt, die man durch den Befehl /msg %S LIST erhält.
	Allerdings, wenn jemand Ihren Chatnamen kennt, kann er den
	Befehl INFO trotzdem benutzen.

NICK_HELP_SET_HIDE
	Syntax: SET HIDE {EMAIL | USERMASK | QUIT} {ON | OFF}
	
	Verschiedene Attribute Ihres Chatnamens werden aus der Anzeige
	der Informationen für Ihren Chatnamen (Befehl %S INFO)
	ausgeschlossen. Sie können so Ihre E-Mail-Adresse (EMAIL),
	Ihre letzte Hostmaske (USERMASK) und Ihre Letzte Quit-Message
	(QUIT) verstecken (ON) oder anzeigen lassen (OFF).

NICK_HELP_SET_TIMEZONE
	Syntax: SET TIMEZONE {UTC-Offset | TimeZone | DEFAULT}
	
	Damit wird die Zeitzone gewählt, in der alle Zeitangaben
	(wie z.B. die letzte Nutzung eines Chatnamens oder Raumes)
	erscheinen, wenn nicht die Standardzone des Servers benutzt
	wird. Die Option DEFAULT wechselt zurück zu der Zeitzone
	des Servers. Die Zeitzone kann in zwei Arten angegeben werden:
	
	   -  Als UTC-Offset: die Zeitverschiebung zwischen Ihrer
	      lokalen Zeit und UTC (Universal Coordinated Time, auch
	      bekannt unter dem Namen GMT - Greenwich Mean Time). Die
	      Zeitverschiebung wird mit "+" (östlich von Greenwich)
	      oder "-" (westlich von Greenwich) angegeben. Dahinter
	      wird eine Ganzzahl (Zeitverschiebung in Stunden) oder
	      eine Zeichenkette in Form "hh:mm" angegeben.
	
	   -  Als Zeitzonenname, wie "GMT" oder "MET". Nicht alle
	      Zeitzonennamen werden unterstützt. Des weiteren werden
	      die Selben Zeitzonennamen von unterschiedlichen Ländern
	      benutzt. Wenn die Zeitangaben in Ihrer gewählten Zeitzone
	      falsch sind, sollen sie stattdessen den UTC-Offset
	      benutzen.
	
	Hinweis: die Services wechseln nicht automatisch von Sommer-
	zu Winterzeit und umgekehrt. Sie müssen die Änderung selbst
	vornehmen.
	
	Beispiele:
	
	    SET TIMEZONE -5
	        Die Zeitzone auf 5 Stunden nach (westlich) UTC/GMT setzen.
	
	    SET TIMEZONE +3:30
	        Die Zeitzone auf 3 Stunden, 30 Minuten vor (östlich) UTC
	        setzen.
	
	    SET TIMEZONE JST
	        Die Zeitzone auf "Japan Standard Time" (UTC+9:00) setzen.
	
	    SET TIMEZONE CET
	        Die Zeitzone auf "Central European Time" (UTC+1:00) setzen.
	
	    SET TIMEZONE CEST
	        Die Zeitzone auf "Central European Daylight-Saving Time"
	        (UTC+2:00) setzen.

NICK_HELP_SET_MAINNICK
	Syntax: SET MAINNICK Chatname
	
	Erlaubt, wenn sie mehrere Chatnamen mit dem Befehl LINK verbunden
	haben, einen Chatnamen als den Hauptchatnamen zu deklarieren. Wenn
	der freigegeben wird, wird automatisch ein anderer zum Hauptchatnamen.

NICK_HELP_UNSET
	Syntax: UNSET {URL | EMAIL | INFO}
	
	Ermöglicht die URL (URL), die E-Mail-Adresse (EMAIL) oder
	die Zusatzinformation (INFO) aus Ihrem Chatnamen-Eintrag
	zu entfernen.

NICK_HELP_UNSET_REQ_EMAIL
	Syntax: UNSET {URL | INFO}
	
	Ermöglicht die URL (URL), oder die Zusatzinformation (INFO)
	aus Ihrem Chatnamen-Eintrag zu entfernen.

NICK_HELP_RECOVER
	Syntax: RECOVER Chatname [Passwort]
	
	Ermöglicht die Verbindung einer Person, die Ihren Chatnamen
	benutzt, zu trennen. Wenn die KILL Sicherheitsfunktion aktiv
	ist, macht %S das automatisch.
	
	Mit diesem Befehl, verbindet %S einen virtuellen User mit Ihrem
	Chatnamen zum Server. Das führt dazu, dass die Verbindung vom
	anderen Benutzer getrennt wird. Dieser virtuelle User bleibt
	%s online, um zu verhindern, dass ein anderer diesen Nick
	nach einem reconnect nutzen kann. Sie können nach einer Minute
	Ihren Chatnamen zurück haben. Mit dem Befehl RELEASE ist es
	möglich, den Chatnamen früher freizugeben.
	(/msg %S HELP RELEASE)
	
	Um den Befehl RECOVER benutzen zu können, müssen Sie
	das Passwort angeben oder sich für einen anderen Chatnamen
	identifizieren, der mit dem Zielchatnamen verbunden ist.

NICK_HELP_RELEASE
	Syntax: RELEASE Chatname [Passwort]
	
	Gibt den Chatnamen frei, der von %S, infolge des Befehls
	RECOVER gehalten ist. Standardmäßig wird der Chatname für
	%s gehalten. Dieser Befehl erlaubt diese Zeitspanne
	zu verkürzen.
	
	Um den Befehl RELEASE benutzen zu können, müssten Sie
	das Passwort angeben oder sich für einen anderen Chatnamen
	identifizieren, der mit dem Zielchatnamen verbunden ist.

NICK_HELP_GHOST
	Syntax: GHOST Chatname [Passwort]
	
	Trennt die nicht mehr aktive Verbindung eines Chatnamens, die
	vom Server noch als aktiv gehalten wird. Der GHOST-Befehl wird
	z.B. gelegentlich dann eingesetzt, wenn der Client-Computer
	abgestürzt ist oder Ihre Internet-Verbindung unerwartet getrennt
	wird. Diese verursachen, dass der Chatname immer noch im IRC ist,
	also ein Geist (Engl. GHOST) ist.
	
	Um den Befehl GHOST benutzen zu können, müssten Sie
	das Passwort angeben oder sich für einen anderen Chatnamen
	identifizieren, der mit dem Zielchatnamen verbunden ist.

NICK_HELP_INFO
	Syntax: INFO Chatname [ALL]
	
	Zeigt Information über den gegebenen Chatnamen. So wie wem
	der Nick gehört, letzte gesehene Hostmaske und Zeit und eingestellte
	Optionen.  Wenn der Nick Ihnen gehört und sie ALL auch mit
	spezifizieren, dann werden alle Informationen gezeigt, auch wenn
	sie versteckt sind.
NICK_HELP_INFO_AUTH
	
	Wenn die E-Mail-Adresse des Chatnamens noch nicht authentifiziert
	ist (Befehl AUTH), wird sie nicht angezeigt. Ausnahmen: der
	Besitzer des Chatnamens und Services-Administratoren - unabhängig von
	der Einstellung SET HIDE EMAIL.

NICK_HELP_LISTCHANS
	Syntax: LISTCHANS
	
	Zeigt alle Räume und deren Anzahl die auf Ihren Chatnamen
	registriert wurden.

#NICK_HELP_LIST
#	Syntax: LIST Maske
#	
#	Zeigt alle Chatnamen an, die zu den gegebenen Suchmaske passen.
#	Chatnamen mit PRIVATE Option werden nicht angezeigt.
#	
#	Beispiele:
#	
#	    LIST joeuser@foo.com
#	        Zeigt alle registrierten Chatnamen an, die als letzte
#	        von joeuser@foo.com benutzt wurden.
#	
#	    LIST *Bot*!*@*
#	        Zeigt alle registrierten Chatnamen an, die Bot im
#	        Namen haben (Groß-/Kleinschreibung wird ignoriert).
#	
#	    LIST @*.bar.org
#	        Zeigt alle registrierten Chatnamen an, die als letzte
#	        von Usern aus der Domäne bar.org benutzt wurden.
NICK_HELP_LIST_OPERSONLY
	
	Nur IRC-Operatoren können diesen Befehl benutzen.

#NICK_HELP_LISTEMAIL
#	Syntax: LISTEMAIL Maske
#	
#	Zeigt alle registrierten Chatnamen an (wie im Befehl LIST),
#	die auf eine Suchmaske passen. Dabei werden die E-Mail-Adressen
#	bei der Suche der Chatnamen benutzt. /msg %S HELP LIST für
#	weitere Informationen.

NICK_HELP_STATUS
	Syntax: STATUS [Chatname] ...
	
	Zeigt einen Code an, um herauszufinden, ob dieser Chatname
	dieser Person gehört. Antwort ist in dieser Form :
	
	    Chatname   Status-Code
	
	Chatname ist der, welchen sie gegeben haben, Status-Code
	ist einer von diesen:
	
	    0 - Chatname ist nicht registriert/user ist offline.
	    1 - Chatname wurde nicht identifiziert.
	    2 - Die Hostmaske dieses Benutzers ist auf der Access-Liste
	        des Chatnamens, Passwortabfrage fand nicht statt.
	    3 - Der Benutzer wurde durch Passwortangabe identifiziert.
	
	Sie können bis zu 16 Chatnamen angeben, die weiteren werden
	ignoriert. Keine Fehlermeldung wird angezeigt, wenn der
	Chatname nicht angegeben wird.

NICK_HELP_AJOIN
	Syntax: AJOIN ADD channel
	        AJOIN DEL channel
	        AJOIN LIST
	
	Modifiziert die Liste der Automatisch betretenden Räume oder
	zeigt sie an. Alle Räume von der Liste werden automatisch
	betreten, wenn Sie sich den %S gegenüber mit Ihrem Passwort
	identifizieren. Nur Räume, die mit "#" anfangen, können zur
	Liste hinzugefügt werden, lokale Räume nicht.
NICK_HELP_AJOIN_END
	Es ist zu beachten, dass der IRC-Server den Zutritt zu einigen
	Räumen der Autojoin-Liste verhindern kann. Vor allem die Räume,
	die ein Raum-Passwort haben (Raum-Modus +k) oder die nur nach
	einer Einladung betreten werden können (Modus +i).
NICK_HELP_AJOIN_END_CHANSERV
	Es ist zu beachten, dass der IRC-Server den Zutritt zu einigen
	Räumen der Autojoin-Liste verhindern kann. Vor allem die Räume,
	die ein Raum-Passwort haben (Raum-Modus +k) oder die nur nach
	einer Einladung betreten werden können (Modus +i).
	Hat allerdings der Chatname Zugriff auf den Befehl "%s INVITE"
	bei einem registrierten Raum, wird %S den Chatnamen automatisch
	in den Raum einladen.

NICK_OPER_HELP_COMMANDS
	
	Diese Befehle stehen nur den Services-Administratoren zur Verfügung:
	
	    DROPNICK     Jede Chatnamenregistrierung löschen.
NICK_OPER_HELP_COMMANDS_DROPEMAIL
	    DROPEMAIL    Löscht alle Chatnamen, die einem Suchmuster einer E-Mail-Adresse entsprechen.
NICK_OPER_HELP_COMMANDS_GETPASS
	    GETPASS      Zeigt das Passwort des gegebenen Chatnamens.
	                 (Nur, wenn die Verschlüsselung der Passworte
	                 nicht aktiviert wurde)
NICK_OPER_HELP_COMMANDS_FORBID
	    FORBID       Verbietet die Nutzung des gegebenen Chatnamens.
	    SUSPEND      Suspendiert die Benutzung des gegebenen Chatnamens.
	    UNSUSPEND    Hebt die Suspendierung auf.
NICK_OPER_HELP_COMMANDS_SETAUTH
	    SETAUTH      Setzt einen neuen Authentifizierungscode für den
	                 angegebenen Chatnamen.
	    GETAUTH      Zeigt den Authentifizierungscode für den angegebenen
	                 Chatnamen an.
	    CLEARAUTH    Löscht den Authentifizierungscode für den angegebenen
	               Chatnamen.
NICK_OPER_HELP_COMMANDS_END
	
	Services-Administratoren können andere Chatnamen mit folgenden Befehlen
	manipulieren, ohne sich für den Betroffenen Chatnamen zu
	identifizieren:
	    SET, ACCESS LIST, INFO ALL
	
	Weitere Informationen erhalten Sie bei der Hilfe für jeden
	Befehl (/msg HELP Befehl)

NICK_OPER_HELP_DROPNICK
	Syntax: DROPNICK Chatname
	
	Der angegebene Chatname wird zusammen mit allen verbundenen
	Chatnamen aus der %S-Datenbank gelöscht.
	Dies steht nur den Services-Administratoren zur Verfügung.

NICK_OPER_HELP_DROPEMAIL
	Syntax: DROPEMAIL Suchmuster
	        DROPEMAIL-CONFIRM Suchmuster
	
	Löscht alle Chatnamen, die eine E-Mail-Adresse haben, die dem
	Suchmuster entsprechen. Besteht das Suchmuster nur aus
	dem Zeichen "-", werden alle Chatnamen gelöscht, die keine
	E-Mail-Adresse haben.
	
	Um versehentliches Löschen von Chatnamen mittels diesem Befehl
	zu verhindern, muss dieser Befehl bestätigt werden.
	Zuerst nutzt man den Befehl DROPEMAIL, danach mit dem gleichen
	Parameter den Befehl BDROPEMAIL-CONFIRM. Der Befehl
	DROPEMAIL-CONFIRM muss %s von der gleichen Person übergeben werden
	wie der vorherige DROPEMAIL-Befehl. Werden mehr als 4 DROPEMAIL-
	Befehle hintereinander übergeben, wird der älteste verworfen, und
	steht für einen DROPEMAIL-CONFIRM-Befehl nicht mehr zur Verfügung.
	
	Diese Befehle können nur von Services Admins benutzt werden.

NICK_OPER_HELP_SET
	
	Services-Administratoren können die Option NOEXPIRE setzen, so dass
	eine Chatnamenregistrierung nicht verfällt. Services-Administratoren
	können desweiteren Optionen sämtlicher Chatnamen, ohne das
	Passwort anzugeben, mit dem folgendem Befehl setzen:
	SET !Chatname Option Parameter.

NICK_OPER_HELP_SET_NOEXPIRE
	Syntax: SET [!Chatname] NOEXPIRE {ON | OFF}
	
	Bestimmt, ob die Registrierung des Chatnamen verfällt.
	Wenn ON angegeben wird, wird der angegebene Chatname
	nicht verfallen und nicht aus der Datenbank gelöscht.
	
	Dieser Befehl steht nur den Services-Administratoren zur Verfügung.

### CHANGED: new !nick syntax
#NICK_OPER_HELP_UNSET
#	
#	Services-Administratoren können auch den UNSET-Befehl für jeden Chatnamen
#	benutzen, ohne das Passwort anzugeben. Analog zum Befehl SET.

NICK_OPER_HELP_OLD_UNLINK
	
	Services-Administratoren können von jedem Chatnamen einen Link aufheben,
	ohne Passwort anzugeben.

NICK_OPER_HELP_UNLINK
	Syntax: UNLINK Chatname [FORCE]
	
	Die Verbindung (Link) zum anderen Chatnamen wird aufgehoben.
	Sie können ihren aktuellen Chatnamen nicht als Parameter angeben.
	Wenn sie Ihren aktuellen Chatnamen freigeben wollen, müssen Sie
	zuerst zu einem anderen verbundenen Chatnamen wechseln.
	
	Wenn Sie Ihren Hauptchatnamen freigeben (gesetzt durch den Befehl
	SET MAINNICK), wird Ihr aktueller Chatname automatisch zum
	Hauptchatnamen.
	
	Services-Administratoren können zusätzlich die Option FORCE angeben, was
	es erlaubt, einen Link zu jemand anders' Chatnamen.

NICK_OPER_HELP_OLD_LISTLINKS
	Syntax: LISTLINKS Chatname
	
	Zeigt alle Links zu dem gegebenen Chatnamen an.
	
	Dieser Befehl steht nur den Services-Administratoren zur Verfügung.

NICK_OPER_HELP_LISTLINKS
	Syntax: LISTLINKS [Chatname]
	
	Zeigt alle Chatnamen, die mit Ihrem Aktuellen Chatnamen verbunden
	sind. Der Hauptchatname wird durch ein Stern (*) vor dem Chatnamen
	markiert. Services-Operatoren können einen Chatnamen angeben, um die
	Links von dem Chatnamen, statt dem aktuellen Chatnamen, anzeigen
	zu lassen.

NICK_OPER_HELP_ACCESS
	
	Services-Administratoren können mit einem Chatnamen-Parameter zu
	LIST die Zugriffsliste von jedem Chatnamen anzeigen.

NICK_OPER_HELP_INFO
	
	Services-Administratoren können den ALL Parameter für alle Chatnamen
	(nicht nur die eigenen) benutzen.

NICK_OPER_HELP_LISTCHANS
	
	Services-Administratoren können sich die Liste der Registrierten Räume
	für jeden Chatnamen (nicht nur die eigenen) anzeigen lassen.
	Beispiel: /msg %S LISTCHANS Jemand-Anders

#NICK_OPER_HELP_LIST
#	Syntax: LIST Maske [FORBIDDEN] [NOEXPIRE] [SUSPENDED]
#	
#	Zeigt alle Chatnamen an, die zu den angegebenen Suchmaske passen.
#	Dabei muss entweder der Chatname selbst oder die Letzte Adresse
#	(Format user@host) auf die Suchmaske passen. Chatnamen mit
#	der Option PRIVATE werden nur Services-Administratoren angezeigt.
#	Die NOEXPIRE-Chatnamen werden mit einem !, die suspendierten
#	mit einem * am Anfang markiert.
#	
#	Wenn  FORBIDDEN, SUSPENDED oder NOEXPIRE angegeben sind,
#	werden nur verbotene, suspendierte oder NOEXPIRE-Chatnamen angezeigt.
#	Diese Optionen können in beliebiger Reihenfolge oder Kombination
#	gegeben werden. Sie sind nur von den Services-Administratoren anwendbar.
#	
#	Beispiele:
#	
#	    LIST joeuser@foo.com
#	         Zeigt alle registrierten Chatnamen an, die als letzte
#	         von joeuser@foo.com benutzt wurden.
#	
#	    LIST *Bot*!*@*
#	         Zeigt alle registrierten Chatnamen an, die Bot
#	         Namen haben (Groß-/Kleinschreibung wird ignoriert).
#	
#	    LIST * NOEXPIRE
#	         Zeigt alle registrierten Chatnamen an, die nicht
#	         verfallen.

#NICK_OPER_HELP_LIST_AUTH
#	Syntax: LIST Maske [FORBIDDEN] [NOEXPIRE] [SUSPENDED] [AUTH]
#	
#	Zeigt alle Chatnamen an, die zu den angegebenen Suchmaske passen.
#	Dabei muss entweder der Chatname selbst oder die Letzte Adresse
#	(Format user@host) auf die Suchmaske passen. Chatnamen mit
#	der Option PRIVATE werden nur Services-Administratoren angezeigt.
#	Die NOEXPIRE-Chatnamen werden mit einem !, die suspendierten
#	mit einem * und noch nicht authentifizierten (Befehl AUTH) mit
#	einem ? am Anfang markiert.
#	
#	Wenn  FORBIDDEN, SUSPENDED, NOEXPIRE oder NOAUTH angegeben sind,
#	werden nur verbotene, suspendierte, NOEXPIRE- oder noch nicht
#	authentifizierte Chatnamen angezeigt. Diese Optionen können in beliebiger
#	Reihenfolge oder Kombination gegeben werden. Sie sind nur von den
#	Services-Administratoren anwendbar.
#	
#	Beispiele:
#	
#	    LIST joeuser@foo.com
#	         Zeigt alle registrierten Chatnamen an, die als letzte
#	         von joeuser@foo.com benutzt wurden.
#	
#	    LIST *Bot*!*@*
#	         Zeigt alle registrierten Chatnamen an, die Bot
#	         Namen haben (Groß-/Kleinschreibung wird ignoriert).
#	
#	    LIST * NOEXPIRE
#	         Zeigt alle registrierten Chatnamen an, die nicht
#	         verfallen.

NICK_OPER_HELP_GETPASS
	Syntax: GETPASS Chatname
	
	Zeigt das Passwort des angegebenen Chatnamens an. Die Nutzung des
	Befehls wird protokolliert (wer den Befehl benutzt hat, welcher
	Chatname betroffen war) und an IRC-Operatoren über WALLOPS/GLOBOPS
	mitgeteilt.
	
	Wenn die Verschlüsselung der Passworte aktiviert ist, steht
	der Befehl nicht zur Verfügung.
	
	Dieser Befehl steht nur den Services-Administratoren zur Verfügung.

NICK_OPER_HELP_FORBID
	Syntax: FORBID Chatname
	
	Verbietet die Benutzung des gegebenen Chatnamens. Der Verbot
	wird nur mit DROP aufgehoben.
	
	Dieser Befehl steht nur den Services-Administratoren zur Verfügung.

NICK_OPER_HELP_SUSPEND
	Syntax: SUSPEND [+Dauer] Chatname Grund
	
	Verhindert, dass der Chatname, und alle damit verbundenen, benutzt,
	oder identifiziert werden.  UNSUSPEND hebt die Suspendierung auf.
	Der Parameter Dauer wird genauso interpretiert wie beim Befehl
	%s AKILL.
	
	Im Gegensatz zu Verbot (FORBID), verliert der Chatname
	seine Information nicht. Die Registrierung kann ebenfalls
	verfallen.
	
	Dieser Befehl steht nur den Services-Administratoren zur Verfügung.

NICK_OPER_HELP_UNSUSPEND
	Syntax: UNSUSPEND Chatname
	
	Hebt die Suspendierung des Chatnamens auf.
	
	Dieser Befehl steht nur den Services-Administratoren zur Verfügung.

NICK_OPER_HELP_AJOIN
	
	Services-Administratoren können sich die Autojoin-Liste jedes
	Chatnamens anzeigen lassen: LIST Chatname.

NICK_OPER_HELP_SETAUTH
	Syntax: SETAUTH Chatname
	
	Setzt den Authentifizierungscode für den angegebenen Chatnamen
	neu. Der Inhaber der Chatnamens wird den Befehl AUTH erneut
	benutzen müssen, um den Chatnamen weiter nutzen zu können
	(z.B. Befehl IDENTIFY). Wenn der Chatname ist in Benutzung,
	wären dieser Befehl abgesetzt wird, wird der User benachrichtigt
	und wird von den Services nicht mehr als identifiziert angesehen.
	
	Dieser Befehl steht nur den Services-Administratoren zur Verfügung.

NICK_OPER_HELP_GETAUTH
	Syntax: GETAUTH Chatname
	
	Zeigt den Authentifizierungscode des angegebenen Chatnamens,
	falls existiert.
	
	Dieser Befehl steht nur den Services-Administratoren zur Verfügung.

NICK_OPER_HELP_CLEARAUTH
	Syntax: CLEARAUTH Chatname
	
	Löscht den Authentifizierungscode für den angegebenen Chatnamen.
	Dies erlaubt eine normale Nutzung des Chatnamens.
	
	Dieser Befehl steht nur den Services-Administratoren zur Verfügung.


###########################################################################
#
# ChanServ help messages
#
###########################################################################

# Used in OP/VOICE/etc for required access levels for commands
CHAN_HELP_REQSOP_LEVXOP
	Stufe 100 (SOP)
CHAN_HELP_REQSOP_LEV
	Stufe 100
CHAN_HELP_REQSOP_XOP
	SOP
CHAN_HELP_REQAOP_LEVXOP
	Stufe 50 (AOP)
CHAN_HELP_REQAOP_LEV
	Stufe 50
CHAN_HELP_REQAOP_XOP
	AOP
CHAN_HELP_REQHOP_LEVXOP
	Stufe 40 (HOP)
CHAN_HELP_REQHOP_LEV
	Stufe 40
CHAN_HELP_REQHOP_XOP
	HOP
CHAN_HELP_REQVOP_LEVXOP
	Stufe 30 (VOP)
CHAN_HELP_REQVOP_LEV
	Stufe 30
CHAN_HELP_REQVOP_XOP
	VOP

CHAN_HELP
	%S ermöglicht es Ihnen, einen Raum zu registrieren
	und selbst zu kontrollieren. %S kann verhindern, dass ihr
	Raum von unbefugten übernommen wird ("taking over").
	Um sich die Liste der verfügbaren Befehle von %S anzeigen
	zu lassen, tippen Sie /msg %S HELP COMMANDS. Um einen
	bestimmten Befehl zu nutzen, tippen Sie /msg %S Befehl.
	Um sich Hilfe für einen Bestimmten Befehl anzeigen zu lassen,
	tippen Sie /msg %S HELP Befehl.
CHAN_HELP_EXPIRES
	
	Vergessen Sie Bitte nicht, dass Räume, die %s nicht
	benutzt werden, automatisch verfallen und gelöscht werden.
	Zum Beispiel solche Räume, die niemand aus der Zugriffsliste
	besucht hat.

CHAN_HELP_COMMANDS
	%S versteht folgende Befehle:
	
	    REGISTER   Registrierung eines Raumes
	    IDENTIFY   Identifiziert Sie als Eigentümer des Raumes
	    DROP       Löschen des Raumes aus der Datenbank
	    SET        Setzten Optionen und Informationen für den Raum
	    UNSET      Zurücksetzen verschiedener Optionen
	    INFO       Zeigt Optionen und Informationen über den Raum
CHAN_HELP_COMMANDS_LIST
	    LIST       Zeigt registrierte Räume
CHAN_HELP_COMMANDS_AKICK
	    AKICK      Modifizieren/anzeigen der AutoKick-Liste
#CHAN_HELP_COMMANDS_LEVELS
#	    ACCESS     Modifizieren die Zugriffsliste
#	    LEVELS     Feintuning der Zugriffsstufen
CHAN_HELP_COMMANDS_XOP
	    SOP        Modifizieren/anzeigen der SuperOp-Liste
	    AOP        Modifizieren/anzeigen der AutoOp-Liste
CHAN_HELP_COMMANDS_HOP
	    HOP        Modifizieren/anzeigen der AutoHalfOp-Liste
#CHAN_HELP_COMMANDS_XOP_2
#	    VOP        Modifizieren/anzeigen der VoiceOp-Liste
CHAN_HELP_COMMANDS_OPVOICE
	    OP         Erteilen des Raum-Operator-Status (+o)
	    DEOP       Entziehen des Raum-Operator-Status (-o)
	    VOICE      Erteilen des Moderator-Status (+v)
	    DEVOICE    Entziehen des Moderator-Status (-v)
CHAN_HELP_COMMANDS_HALFOP
	    HALFOP     Erteilen des Raum-HalfOp-Status (+h)
	    DEHALFOP   Entziehen des Raum-HalfOp-Status (-h)
CHAN_HELP_COMMANDS_PROTECT
	    PROTECT    Erteilen des Kick-Schutzes (+a)
	    DEPROTECT  Entziehen des Kick-Schutzes (-a)
CHAN_HELP_COMMANDS_INVITE
	    INVITE     Sich in den Raum einladen lassen
	    UNBAN      Enfernen des Bans auf Sie
	    KICK       Kicken eines Users aus dem Raum
	    TOPIC      Setzen des Raumthemas
	    CLEAR      Zurücksetzen der Raum-Modis, Mass-Kick im Raum
	    STATUS     Anzeigen der Zugriffsrechte des Users im Raum
	
CHAN_HELP_REGISTER
	Syntax: REGISTER Raum Passwort Beschreibung
	
	Registriert einen Raum in der %S-Datenbank unter Ihrem
	Chatnamen. Um den Befehl nutzen zu können, müssen sie den
	Raum-Operator-Status im Raum besitzen, den sie registrieren
	wollen. Das Passwort, den Sie eingeben, wird später beim
	Befehl IDENTIFY benutzt, um Ihnen vollen Zugriff zum
	Raum zu geben und erlauben verschiedene Einstellungen zu
	modifizieren.
	Der letzte Parameter muss angegeben werden. Es ist eine
	allgemeine Beschreibung des Raumes.
	
	Wenn Sie einen Raum registrieren, sind sie der "Gründer" des
	Raumes. Der Gründer kann alle Einstellungen des Raumes ändern.
	%S gibt dem Gründer beim betreten des Raumes automatisch den
	Raum-Oper-Status.
	
	WICHTIG: Um ein Raum überhaupt registrieren zu können,
	müssen Sie einen registrierten Chatnamen haben.
	/msg %s HELP für weitere Informationen.
CHAN_HELP_REGISTER_ADMINONLY
	
	Dies steht nur den Services-Administratoren zur Verfügung.

CHAN_HELP_IDENTIFY
	Syntax: IDENTIFY Raum Passwort
	
	Sie identifizieren sich dem %S gegenüber als Gründer des Raumes.
	Viele Befehle setzen voraus, dass Sie die Identifikation ausführen,
	bevor sie diese nutzen können. Das Passwort ist das, das Sie beim
	Registrieren (Befehl REGISTER) benutzt haben.

#CHAN_HELP_DROP
#	Syntax: DROP Raum
#	
#	Der gegebene Raum wird aus der Datenbank gelöscht. Nur der
#	Gründer darf diesen Befehl nach dem IDENTIFY benutzen.

CHAN_HELP_SET
	Syntax: SET Raum Optionen Parameter
	
	Bestimmt sämtliche Raum-Optionen. Dafür muss der Gründer
	mit IDENTIFY sich identifiziert haben.
	
	Optionen:
	
	    FOUNDER     Bestimmt einen neuen Gründer
	    SUCCESSOR   Bestimmt einen neuen Nachfolger
	    PASSWORD    Bestimmt ein neues Raum-Passwort
	    DESC        Bestimmt eine neue Beschreibung
	    URL         Fügt eine URL dem Raum-Eintrag hinzu
	    EMAIL       Fügt eine E-Mail-Adresse dem Raum-Eintrag hinzu
	    ENTRYMSG    Ändert die Begrüssungsnachricht (die Nachricht,
	                die beim Betreten des Raumes angezeigt wird)
	    KEEPTOPIC   Ändert die Option zur Beibehaltung des Themas
	    TOPICLOCK   Ändert die Option zur Sperrung des Themas
	    MLOCK       Setzt neues Sperr-Modus
	    PRIVATE     Beim LIST-Befehl nicht anzeigen
	    RESTRICTED  Eingeschränkter Zugang zum Raum
	    SECURE      %S Sicherheitsfunktionen werden aktiviert
	    SECUREOPS   Strengere Kontrolle des Raum-Operator-Status
	    LEAVEOPS    Beim betreten den Raum-Operator-Status nicht
	                entziehen
	    OPNOTICE    Eine NOTICE schicken, wenn OP/VOICE Befehle
	                benutzt werden
	    ENFORCE     Zwingt die AutoOp und AutoVoice Status
	
	/msg %S HELP SET Option für weitere Informationen zu
	einer Option.

CHAN_HELP_SET_FOUNDER
	Syntax: SET Raum FOUNDER Chatname
	
	Setzt den Chatnamen als neuen Gründer des Raumes. Der Gründer
	muss ein registrierter Chatname sein.

CHAN_HELP_SET_SUCCESSOR
	Syntax: SET Raum SUCCESSOR Chatname
	
	Ändert den Nachfolger des Raumes. Wenn der Chatname des
	Gründers verfällt, oder gedropped wird, bekommt der Nachfolger
	den Gründerstatus. Dies erfolgt aber nicht, wenn des Nachfolger
	bereits zu viele registrierten Räume (%d) besitzt. Dann wird
	der Raum gelöscht, als hätte der Raum keinen Nachfolger.
	Der Nachfolger muss ein registrierter Chatname sein.

CHAN_HELP_SET_PASSWORD
	Syntax: SET Raum PASSWORD neues_Passwort
	
	Setzt ein neues Raum-Passwort für den Gründer-Zugriff.

CHAN_HELP_SET_DESC
	Syntax: SET Raum DESC Beschreibung
	
	Setzt eine neue Beschreibung für den Raum, die bei den
	Befehlen LIST und INFO gezeigt wird. Man muss den
	Gründer-Zugriff haben, um dies zu benutzen.

CHAN_HELP_SET_URL
	Syntax: SET Raum URL URL
	
	Fügt URL dem Raum-Eintrag hinzu. Diese URL wird angezeigt,
	wenn sich jemand die Rauminformation mit dem Befehl INFO
	anzeigen lässt.

CHAN_HELP_SET_EMAIL
	Syntax: SET Raum EMAIL Adresse
	
	Fügt dem Raum-Eintrag eine E-Mail-Adresse hinzu. Diese Adresse
	wird angezeigt, wenn sich jemand die Rauminformation mit dem
	Befehl INFO anzeigen lässt.

CHAN_HELP_SET_ENTRYMSG
	Syntax: SET Raum ENTRYMSG text
	
	Setzt eine neue Begrüssungsnachricht, die man als /NOTICE
	erhält, wenn man diesen Raum betritt.

CHAN_HELP_SET_KEEPTOPIC
	Syntax: SET Raum KEEPTOPIC {ON|OFF}
	
	Bewirkt, dass das Thema, nicht verloren geht, wenn niemand im
	Raum ist. Wenn topic retention aktiv ist, wird der Topic
	durch %S gespeichert, und, wenn jemand den Raum betritt,
	wird er wieder gesetzt.

CHAN_HELP_SET_TOPICLOCK
	Syntax: SET Raum TOPICLOCK {ON|OFF}
	
	Die Topic-Lock-Option wird aktivert/deaktiviert.
	Wenn aktiv, verhindert %S, dass jemand das Thema ändert,
	es sei denn der Befehl TOPIC wird benutzt.

CHAN_HELP_SET_MLOCK
	Syntax: SET Raum MLOCK Modi
	
	Aktiviert/deaktiviert die Mode-Lock Option für den Raum.
	%S ermöglicht, dass bestimmte Modi (Raumoptionen) immer
	eingeschaltet oder ausgeschaltet werden. Alle "binären" Modi
	(n, s, t, k, l usw.) können gesperrt werden. Jedes
	Modus kann eine Sperre auf Ein oder Aus haben, oder nicht
	gesperrt sein. Die Modi sind ähnlich dem /MODE-Befehl
	aufgebaut: die Modi, die einem + folgen werden aktiviert,
	die, die einem - folgen, werden deaktiviert.
	
	Warnung: Falls Sie, wie im zweiten Beispiel, eine Sperre
	für den Raumschlüssel haben (+k) setzen wollen, dann sollten
	Sie die Option RESTRICTED auch setzen, so dass niemand,
	der den leeren Raum betritt, den Schlüssel sieht. (siehe
	HELP SET RESTRICTED).
	
	Beispiele:
	
	    SET #Raum MLOCK +nt-ikl
	        Die Modi t und n werden gesetzt, die i, k und l werden
	        entfernt. Alle anderen Modi werden nicht verändert.
	
	    SET #Raum MLOCK +knst-ilmp schlüssel
	        Die Modi k, n, s und t werden gesetzt, Modi i, l, m
	        und p werden entfernt. Der Schlüssel wird auf
	        das Wort "schlüssel" gesetzt.
	
	    SET #Raum MLOCK +
	        Deaktiviert ModeLock, alle Modi können frei gesetzt werden.

CHAN_HELP_SET_HIDE
	Syntax: SET Raum HIDE {EMAIL | TOPIC | MLOCK} {ON | OFF}
	
	Verschiedene Attribute des Raumes werden aus der Anzeige
	der Informationen über den Raum (%S-Befehl INFO)
	ausgeschlossen. Sie können so die E-Mail-Adresse (EMAIL),
	das letzte Raumthema (TOPIC) oder die Sperr-Modi (MLOCK)
	verstecken (ON) oder anzeigen lassen (OFF).
	
	Hinweis: das Raumthema wird immer versteckt, wenn die Sperr-Modi
	die Modi +s oder +p enthalten.

CHAN_HELP_SET_PRIVATE
	Syntax: SET Raum PRIVATE {ON|OFF}
	
	Setzt die private-Option für den Raum. Wenn aktiv, wird der
	Raum auf der Liste mit /msg %S LIST nicht erscheinen.
	
CHAN_HELP_SET_RESTRICTED
	Syntax: SET Raum RESTRICTED {ON|OFF}
	
	Schaltet den eingeschränkten Zugang zum Raum ein oder
	aus. Wenn der eingeschränkte Zugang aktiviert ist, wird
	jeder, dem der Raum-Operator-Status nicht erlaubt ist (im
	Standard eine negative Stufe in der Zugriffsliste), aus dem
	Raum gekickt und gebannt.

CHAN_HELP_SET_SECURE
	Syntax: SET Raum SECURE {ON|OFF}
	
	Aktiviert/deaktiviert %S Sicherheitsfunktionen für den Raum.
	Wenn aktiv, können nur die, die ihren Chatnamen mit %s
	registriert haben und sich mit IDENTIFY identifiziert haben,
	besondere Rechte zum Raum erhalten - folgend der Zugriffsliste.
	Wenn die Option RESTRICTED auch gesetzt ist, können nur Chatter
	in den Raum, die sich bei %s identifiziert haben.

CHAN_HELP_SET_SECUREOPS
	Syntax: SET Raum SECUREOPS {ON|OFF}
	
	Aktiviert/deaktiviert die SecureOps-Option, so dass Benutzer,
	die nicht in der Zugriffsliste vermerkt sind, den Op-Status
	nicht erhalten können.

CHAN_HELP_SET_LEAVEOPS
	Syntax: SET Raum LEAVEOPS {ON | OFF}
	
	Aktiviert/deaktiviert die LeaveOps-Option. Wenn aktiv,
	wird %S der ersten Person, die den Raum betritt, nicht den
	Raum-Operator-Status entziehen. Die Option SECUREOPS wird
	trotzdem beachtet.
	Warnung: dies kann die Übernahme des Raumes bei einem
	Netsplit ermöglichen.

CHAN_HELP_SET_OPNOTICE
	Syntax: SET Raum OPNOTICE {ON | OFF}
	
	Schaltet die op-notice Funktion für einen Raum ein oder aus.
	Wenn op-notice eingeschaltet ist, schickt %S eine Notiz in
	den Raum sobald die Befehle OP, VOICE, oder ähnliche
	Befehle auf einen Chatnamen in dem Raum angewandt werden.

CHAN_HELP_SET_ENFORCE
	Syntax: SET Raum ENFORCE {ON | OFF}
	
	Aktiviert/deaktiviert die Zwangsoption für Auto-Op und
	Auto-Voice. Wenn aktiv, überprüft %S die Auto-Op und
	Auto-Voice Privilegien. Das bedeutet, wenn ein Benutzer mit
	Auto-Op Status gedeopped wird, wird %S ihn wieder oppen.
	%S wird auch verhindern, dass der DEOP Befehl für den
	Benutzer eingesetzt wird.

CHAN_HELP_UNSET
	Syntax: UNSET Raum {SUCCESSOR | URL | EMAIL | ENTRYMSG}
	
	Löscht die gegebene Information von dem Raum-Eintrag.

### CHANGED: new LIST syntax
#CHAN_HELP_SOP
#	Syntax: SOP Raum ADD Chatname
#	        SOP Raum DEL Chatname
#	        SOP Raum LIST [Maske]
#	        SOP Raum COUNT
#	
CHAN_HELP_SOP_MID1
	Modifiziert die SuperOp-Liste (SOP) des Raums.
	Benutzer mit SOP-Status bekommen automatisch den
	Raum-Operator-Status (+o), wenn sie den Raum betreten.
CHAN_HELP_SOP_MID1_CHANPROT
	Modifiziert die SuperOp-Liste (SOP) des Raums.
	Benutzer mit SOP-Status bekommen automatisch den
	Raum-Operator-Status (+o) und Kick-Schutz (+a), wenn
	sie das Raum betreten.
CHAN_HELP_SOP_MID2
	
	Ein SOP kann die AOP-, VOP- und NOP-Listen bearbeiten.  Nur
	der Gründer des Raumes kann die SOP-Liste bearbeiten.
CHAN_HELP_SOP_MID2_HALFOP
	
	Ein SOP kann die AOP-, HOP-, VOP- und NOP-Listen bearbeiten.
	Nur der Gründer des Raumes kann die SOP-Liste bearbeiten.
### CHANGED: completely rewritten
#CHAN_HELP_SOP_END
#	
#	SOP LIST zeigt die SOP-Liste. Wenn eine Maske angegeben ist,
#	werden nur die passenden Einträge angezeigt. Wenn eine Liste
#	angegeben ist, werden nur enthaltene Einträge angezeigt.
#	
#	   SOP #channel LIST 2-5,7-9
#	      Zeigt SOP-Einträge zwischen 2 und 5, und 7 und 9
#	
#	SOP COUNT gibt die Anzahl der SOP-Einträge an.

### CHANGED: new LIST syntax
#CHAN_HELP_AOP
#	Syntax: AOP Raum ADD Chatname
#	        AOP Raum DEL Chatname
#	        AOP Raum LIST [Maske]
#	        AOP Raum COUNT
#	
#	Modifiziert die AutoOp-Liste (AOP) des Raums.
#	Benutzer mit AOP-Status bekommen automatisch den
#	Raum-Operator-Status (+o), wenn sie den Raum betreten.
CHAN_HELP_AOP_MID
	
	Ein AOP kann die VOP- und NOP-Listen bearbeiten.  Ein SOP
	kann die AOP-Liste bearbeiten.
CHAN_HELP_AOP_MID_HALFOP
	
	Ein AOP kann die HOP-, VOP und NOP-Listen bearbeiten.  Ein
	SOP kann die AOP-Liste bearbeiten.
### CHANGED: completely rewritten
#CHAN_HELP_AOP_END
#	
#	AOP LIST zeigt die AOP-Liste.  Wenn eine Maske angegeben ist,
#	werden nur die passenden Einträge angezeigt. Wenn eine Liste
#	angegeben ist, werden nur enthaltene Einträge angezeigt.
#	
#	   AOP #channel LIST 2-5,7-9
#	      Zeigt AOP-Einträge zwischen 2 und 5, und 7 und 9
#	
#	Der Befehl AOP COUNT gibt die Anzahl der AOP-Einträge an.

### CHANGED: completely rewritten
#CHAN_HELP_HOP
#	Syntax: HOP Raum ADD Chatname
#	        HOP Raum DEL Chatname
#	        HOP Raum LIST [Maske]
#	        HOP Raum COUNT
#	
#	Modifiziert die HalfOp-Liste (HOP) des Raums.
#	Benutzer mit HOP-Status erhalten den Halfop-Status (+h),
#	wenn sie den Raum betreten.
#	
#	Ein HOP kann die VOP- und NOP-Listen bearbeiten. SOP und AOP
#	können die HOP-Liste bearbeiten.
#	
#	HOP LIST zeigt die HalfOp-Liste an. Wenn eine Maske angegeben
#	ist, werden nur die passende Einträge angezeigt. Wenn eine Liste
#	angegeben ist, werden nur enthaltene Einträge angezeigt.
#	
#	   HOP #channel LIST 2-5,7-9
#	      Zeigt HOP-Einträge zwischen 2 und 5, und 7 und 9
#	
#	Der Befehl HOP COUNT gibt die Anzahl der HOP-Einträge an.

### CHANGED: completely rewritten
#CHAN_HELP_VOP
#	Syntax: VOP Raum ADD Chatname
#	        VOP Raum DEL Chatname
#	        VOP Raum LIST [Maske]
#	        VOP Raum COUNT
#	
#	Modifiziert die VoiceOp-Liste (VOP) des Raums.
#	Benutzer mit VOP-Status erhalten den Moderator-Status (+v),
#	wenn sie den Raum betreten.
#	
#	VOP LIST zeigt die Auto-Voice-Liste an.  Wenn eine Maske angegeben
#	ist, werden nur die passende Einträge angezeigt. Wenn eine Liste
#	angegeben ist, werden nur enthaltene Einträge
#	angezeigt.
#	
#	   VOP #channel LIST 2-5,7-9
#	      Zeigt VOP-Einträge zwischen 2 und 5, und 7 und 9
#	
#	Der Befehl VOP COUNT gibt die Anzahl der VOP-Einträge an.

### CHANGED
#CHAN_HELP_ACCESS
#	Syntax: ACCESS Raum ADD [Chatname] Stufe}
#	        ACCESS Raum DEL Chatname
#	        ACCESS Raum LIST [Maske]
#	        ACCESS Raum COUNT
#	
#	Verwaltet die Zugriffsliste des Raumes. In dieser Liste sind
#	die Chatnamen eingetragen, die automatisch den Raum-Operator-Status
#	oder Moderator-Status bekommen und Zugriff auf andere %S Befehle
#	habe können. Die Stufe bestimmt eine Gruppe der verschiedenen
#	Zugriffsrechte. /msg %S HELP ACCESS LEVELS für weitere
#	Informationen. Jeder Chatname, der auf der Zugriffsliste
#	eingetragen ist, hat die Stufe 0.
#	
#	ACCESS ADD trägt den gegebenen Chatnamen in die Zugriffsliste
#	mit der gegebenen Stufe ein. Die Stufe muss kleiner als Stufe von
#	der Person sein, die diesen Befehl ausführen will. Wenn der
#	Chatname bereits auf der Zugriffsliste steht, muss dessen Stufe
#	niedriger sein, als der Person, die den Befehl absetzt.
#	
#	ACCESS DEL löscht den angegebenen Chatnamen von der Liste.
#	
#	ACCESS LIST zeigt die Zugriffsliste. Wenn eine Suchmaske
#	angegeben ist, werden nur die Chatnamen angezeigt, die der
#	Suchmaske entsprechen.
#	
#	Der Befehl ACCESS COUNT gibt die Anzahl der Einträge
#	#der Zugriffsliste an.
CHAN_HELP_ACCESS_XOP
	
	Hinweis: Zugriffslisten können auch von einem SOP, einem
	AOP, einem VOP, oder einem NOP modifiziert werden. Diese
	entsprechen den Stufen %d, %d, %d und %d.
CHAN_HELP_ACCESS_XOP_HALFOP
	
	Hinweis: Zugriffslisten können auch von einem SOP, einem
	AOP, einem HOP, einem VOP, oder einem NOP modifiziert
	werden. Diese entsprechen den Stufen %d, %d, %d, %d und %d.

#CHAN_HELP_ACCESS_LEVELS
#	Die Zugriffsstufen
#	
#	Die Standardeinstellungen:
#	
#	   Gründer  Als der Besitzer des Raums hat der Gründer
#	                vollständigen Zugriff auf alle %S-Befehle und
#	                Auto-Op, wenn er den Raum betritt. Zu beachten
#	                ist, dass es keinen zweiten Gründer geben kann.
#	                Der Gründer-Status kann mit dem Befehl ACCESS
#	                nicht weitergegeben werden.
#	      %4d  Kann AKICK-Befehl aufrufen, Auto-Op.
#	      %4d  Auto-Op (+o).
CHAN_HELP_ACCESS_LEVELS_HALFOP
	      %4d  Auto-HalfOp (+h).
CHAN_HELP_ACCESS_LEVELS_END
	      %4d  Auto-Voice (+v).
	         0  Normaler Benutzer. Wenn SecureOps nicht
	                aktiv ist kann, normal op werden.
	        -1  Benutzer kann kein Raum-Operator werden.
	      -100  Benutzer darf den Raum nicht betreten.
	
	Mit dem LEVELS-Befehl können diese Werte, bis auf die
	letzten zwei, geändert, und neue hinzugefügt werden.
	/msg %S HELP LEVELS für weitere Informationen.

CHAN_HELP_LEVELS
	Syntax: LEVELS Raum SET Option Level
	        LEVELS Raum {DIS | DISABLE} Option
	        LEVELS Raum LIST
	        LEVELS Raum RESET
	
	Der Befehl LEVELS definiert die Zugriffsstufen um, die mit
	dem Befehl ACCESS an Chatnamen vergeben werden. Mit dem Befehl
	können sie Zugriffsstufen für die meisten Befehle des %S
	definieren. Die Befehle SET FOUNDER, SET PASSWORD und
	dieser Befehl stehen ausschließlich dem Gründer zur Verfügung.
CHAN_HELP_LEVELS_XOP
	
	Warnung: Dieser Befehl ist nur für erfahrene Benutzer!
	Das Ändern der Stufen kann dazu führen, dass die Befehle SOP,
	AOP, VOP und NOP nicht mehr richtig funktionieren.
CHAN_HELP_LEVELS_XOP_HOP
	
	Warnung: Dieser Befehl ist nur für erfahrene Benutzer!
	Das Ändern der Stufen kann dazu führen, dass die Befehle SOP,
	AOP, HOP, VOP und NOP nicht mehr richtig funktionieren.
CHAN_HELP_LEVELS_END
	
	LEVELS SET setzt eine neue Stufe für die angegebene Option.
	LEVELS DISABLE (Kurzform DIS) deaktiviert diese Option
	für normale Benutzer (bis auf den Gründer).
	LEVELS LIST zeigt die aktuelle Liste der Zugriffsstufen.
	LEVELS RESET ersetzt die Stufen mit den Standardeinstellungen
	(HELP ACCESS LEVELS für Info).
	
	HELP LEVELS DESC für die Beschreibung der Optionen, deren
	Benutzung durch  die Zugriffsstufen eingegrenzt werden können.

CHAN_HELP_LEVELS_DESC
	Folgende Optionen können mit LEVELS SET gesetzt werden.

### CHANGED
#CHAN_HELP_AKICK
#	Syntax: AKICK Raum ADD Maske [Grund]
#	        AKICK Raum DEL Maske
#	        AKICK Raum LIST [Maske]
#	        AKICK Raum VIEW [Maske]
#	        AKICK Raum COUNT
#	        AKICK Raum ENFORCE
#	
#	Bearbeitet die AutoKick-Liste eines Raumes. Wenn jemand,
#	der in diese Liste eingetragen ist, den Raum betritt, wird
#	er vom %S sofort gebannt und gekickt.
#	
#	Der Befehl AKICK ADD trägt die Maske in der Form User@Host
#	oder Chatname!User@Host in die Autokick-Liste  ein.
#	Wenn ein Grund spezifiziert wurde, wird dieser als Kick-Grund
#	benutzt, sonst wird der Standardgrund ("You have been banned
#	from the Channel") benutzt.
#	
#	Der Befehl AKICK DEL löscht die Maske von der Liste. Falls ein
#	Autokick einen Bann im Raum erzwungen hat, wird dieser nicht
#	entfernt.  Der Raum-Operator muss den Bann manuell aufheben.
#	
#	Der Befehl AKICK LIST zeigt die AutoKick-Liste. Wenn eine
#	Suchmaske angegeben ist, werden nur die Einträge angezeigt, die
#	der Suchmaske entsprechen. AKICK VIEW funktioniert ähnlich,
#	zeigt aber detailiertere Informationen.
#	
#	Der Befehl ACKICK COUNT zeigt die Anzahl der Einträge
#	auf der AutoKick-Liste.
#	
#	Der Befehl AKICK ENFORCE veranlasst %S, alle User aus
#	dem Raum zu kicken, die einem Eintrag aus der AutoKick-Liste
#	entsprechen.
#	
#	Standardmäßig steht dieser Befehl den Usern zur Verfügung,
#	die den %s-Zugriff haben.

CHAN_HELP_INFO
	Syntax: INFO Raum [ALL]
	
	Zeigt Information über den angegebenen registrierten Raum:
	den Gründer, Registrierungsdatum, letzte Benutzung, Sperr-Modi.
	Wenn Sie auch der Gründer sind, können Sie mit ALL alle
	Informationen, wie Begrüssungstext und Nachfolger, erhalten.
	
#CHAN_HELP_LIST
#	Syntax: LIST Maske
#	
#	Zeigt alle Räume an, die der gegebenen Suchmaske entsprechen.
#	(Räume mit der Option PRIVATE werden nicht angezeigt.)
CHAN_HELP_LIST_OPERSONLY
	
	Dies steht nur den IRC-Operatoren zur Verfügung.

CHAN_HELP_OP
	Syntax: OP Raum [Chatname]
	
	Gibt dem angegebenen Chatnamen den Raum-Op-Status (+o).
	Wird kein Chatname angegeben, wird der den Befehl
	aufrufende User den Raum-Operator-Status bekommen.
	Standardmäßig eingeschränkt zu den Usern, die in der
	Zugriffsliste mit der Stufe %s oder höher stehen.

CHAN_HELP_DEOP
	Syntax: DEOP Raum [Chatname]
	
	Entzieht dem angegebenen Chatnamen den Raum-Op-Status (-o).
	Wird kein Chatname angegeben, wird dem den Befehl
	aufrufende User der Raum-Operator-Status entzogen.
	Standardmäßig eingeschränkt zu den Usern, die in der
	Zugriffsliste mit der Stufe %s oder höher stehen.

CHAN_HELP_VOICE
	Syntax: VOICE Raum [Chatname]
	
	Gibt dem angegebenen Chatnamen den Moderator-Status (+v).
	Wird kein Chatname angegeben, wird der den Befehl
	aufrufende User den Moderator-Status bekommen.
	Standardmäßig eingeschränkt zu den Usern, die in der
	Zugriffsliste mit der Stufe %s oder höher stehen.

CHAN_HELP_DEVOICE
	Syntax: DEVOICE Raum [Chatname]
	
	Entzieht dem angegebenen Chatnamen den Moderator-Status (-v).
	Wird kein Chatname angegeben, wird dem den Befehl
	aufrufende User der Moderator-Status entzogen.
	Standardmäßig eingeschränkt zu den Usern, die in der
	Zugriffsliste mit der Stufe %s oder höher stehen.

CHAN_HELP_HALFOP
	Syntax: HALFOP Raum [Chatname]
	
	Gibt dem angegebenen Chatnamen den Halfop-Status (+h).
	Wird kein Chatname angegeben, wird der den Befehl
	aufrufende User den Halfop-Status bekommen.
	Standardmäßig eingeschränkt zu den Usern, die in der
	Zugriffsliste mit der Stufe %s oder höher stehen.

CHAN_HELP_DEHALFOP
	Syntax: DEHALFOP Raum [Chatname]
	
	Entzieht dem angegebenen Chatnamen den Halfop-Status (-h).
	Wird kein Chatname angegeben, wird dem den Befehl
	aufrufende User der Halfop-Status entzogen.
	Standardmäßig eingeschränkt zu den Usern, die in der
	Zugriffsliste mit der Stufe %s oder höher stehen.

CHAN_HELP_PROTECT
	Syntax: PROTECT Raum [Chatname]
	
	Gibt dem angegebenen Chatnamen den Kick-Schutz (+a).
	Wird kein Chatname angegeben, wird der den Befehl
	aufrufende User den Kick-Schutz bekommen.
	Standardmäßig eingeschränkt zu den Usern, die in der
	Zugriffsliste mit der Stufe %s oder höher stehen.

CHAN_HELP_DEPROTECT
	Syntax: DEPROTECT Raum Chatname
	
	Entzieht dem Chatnamen den Kick-Schutz (-a).
	Wird kein Chatname angegeben, wird dem den Befehl
	aufrufende User der Kick-Schutz entzogen.
	Standardmäßig eingeschränkt zu den Usern, die in der
	Zugriffsliste mit der Stufe %s oder höher stehen.

CHAN_HELP_INVITE
	Syntax: INVITE Raum
	
	Lässt %S Sie zum angegebenen Raum einladen (invite).
	Standardmäßig eingeschränkt zu den Usern, die in der
	Zugriffsliste mit der Stufe %s oder höher stehen.
	
	Hinweis: Wenn Sie eine Meldung erhalten, dass der Raum
	nicht benutzt wird, bedeutet es, dass der Raum leer ist
	und Sie den Raum ohne Einladung betreten können.

CHAN_HELP_UNBAN
	Syntax: UNBAN Raum
	
	Lässt %S alle Bans aufheben, die Sie daran hindern den Raum
	zu betreten.
	Standardmäßig eingeschränkt zu den Usern, die in der
	Zugriffsliste mit der Stufe %s oder höher stehen.

CHAN_HELP_KICK
	Syntax: KICK Raum Chatname [Grund]
	
	Lässt %S den User mit dem angegebenen Chatnamen aus
	dem angegebenen Raum kicken. Die Kick-Nachricht wird
	die Information beinhalten, wer den Befehl benutzt hat
	und den Grund, wenn angegeben.
	Standardmäßig eingeschränkt zu den Usern, die in der
	Zugriffsliste mit der Stufe %s oder höher stehen.
CHAN_HELP_KICK_PROTECTED
	
	Hinweis: User mit Kick-Schutz (+a) können mit dem Befehl
	nicht gekickt werden. Auch nicht vom Gründer.

CHAN_HELP_TOPIC
	Syntax: TOPIC Raum Topic
	
	Bewirkt, dass %S das Thema für den Raum setzt. Dieser Befehl
	ist besonders nützlich, wenn TOPICLOCK aktiv ist.
	Standardmäßig eingeschränkt zu den Usern, die in der
	Zugriffsliste mit der Stufe %s oder höher stehen.

CHAN_HELP_CLEAR
	Syntax: CLEAR Raum Einstellung
	
	Lässt %S bestimmte Einstellungen vom Raum entfernen.
	Die Einstellung kann eine der folgenden sein:
	
	    MODES       Alle Modi bis auf MLOCK entfernen.
	    BANS        Alle Bans entfernen
CHAN_HELP_CLEAR_EXCEPTIONS
	    EXCEPTIONS  Alle Exceptions entfernen
CHAN_HELP_CLEAR_INVITES
	    INVITES     Alle Einladungsmasken entfernen
CHAN_HELP_CLEAR_MID
	    OPS         Entfernen des Raum-Operator-Status (-o)
	                    von allen Usern (Massdeop)
CHAN_HELP_CLEAR_HALFOPS
	    HALFOPS     Entfernen des Halfop-Status (-h) von allen
	                    Usern (Massdehalfop)
CHAN_HELP_CLEAR_END
	    VOICES      Entfernen des Moderator-Status (-v) von
	                    allen Usern (Massdevoice)
	    USERS       Entfernen (Kick) aller User (Masskick)
	
	Standardmäßig eingeschränkt zu den Usern, die in der
	Zugriffsliste mit der Stufe %s oder höher stehen.
	
CHAN_HELP_STATUS
	Syntax: STATUS Raum Chatname
	
	Zeigt die aktuelle Zugriffsstufe des angegebenen Chatnamens
	in dem angegebenem Raum. Die Antwort hat folgende Form:
	
	    STATUS Raum Chatname Stufe
	
	Im Falle eines Fehlers wird die Antwort folgende Form haben:
	
	    STATUS Raum Chatname ERROR Fehlermeldung
	
	Standardmäßig eingeschränkt zu den Usern, die in der
	Zugriffsliste mit der Stufe %s oder höher stehen.

#CHAN_OPER_HELP_COMMANDS
#	
#	Diese Befehle können nur Services-Administratoren benutzen:
#	
CHAN_OPER_HELP_COMMANDS_GETPASS
	    GETPASS    Zeigt das Passwort des gegebenen Raumes an
	               (nur, wenn Passwortverschlüsselung nicht aktiv ist)
CHAN_OPER_HELP_COMMANDS_FORBID
	    FORBID     Verbietet die Benutzung des gegebenen Raumes
	    SUSPEND    Suspendiert die Benutzung eines Raums
	    UNSUSPEND  Hebt die Suspendierung auf
#CHAN_OPER_HELP_COMMANDS_END
#	
#	Services-Administratoren können ohne IDENTIFY jeden Raum-Eintrag
#	löschen (DROP), die Zugriffs- und AutoKick-Listen einsehen
#	und den Befehl STATUS benutzen.

CHAN_OPER_HELP_SET
	
	Services-Administratoren können NOEXPIRE für einen Raum setzen,
	so dass die Registrierung des Raumes nicht verfällt. Services-Administratoren
	können desweiteren Optionen sämtlicher Räume setzen, ohne das
	Passwort anzugeben.

CHAN_OPER_HELP_SET_NOEXPIRE
	Syntax: SET Raum NOEXPIRE {ON | OFF}
	
	Bestimmt, ob die Registrierung des Raumes verfällt.
	Wenn On angegeben wird, wird der Raum nicht
	verfallen  und nicht aus der Datenbank gelöscht.
	
	Dieser Befehl steht nur den Services-Administratoren zur Verfügung.
	
CHAN_OPER_HELP_UNSET
	
	Services-Administratoren können Einstellungen aller Räume löschen.
	Ähnlich, wie beim Befehl SET.

CHAN_OPER_HELP_INFO
	
	Services-Administratoren können den ALL Parameter für alle Räume
	benutzen.

#CHAN_OPER_HELP_LIST
#	Syntax: LIST Maske [FORBIDDEN] [NOEXPIRE] [SUSPENDED]
#	
#	Zeigt alle Räume an, die der Suchmaske entsprechen. Räume
#	mit aktivierter Option PRIVATE werden nur den Services-Administratoren
#	angezeigt. Räume mit Option NOEXPIRE werden den Services-Administratoren
#	mit einem ! markiert gezeigt.
#	
#	Wenn FORBIDDEN, SUSPENDED oder NOEXPIRE angegeben sind,
#	werden nur verbotene, suspendierte oder NOEXPIRE-Räume angezeigt.
#	Diese Optionen können in beliebiger Reihenfolge oder Kombination
#	gegeben werden. Sie sind nur von den Services-Administratoren anwendbar.

CHAN_OPER_HELP_GETPASS
	Syntax: GETPASS Raum
	
	Zeigt das Passwort für den angegebenen Raum an. Die Nutzung des
	Befehls wird protokolliert (wer den Befehl benutzt hat, welcher
	Raum betroffen war) und an IRC-Operatoren über WALLOPS/GLOBOPS
	mitgeteilt.
	
	Wenn die Verschlüsselung der Passworte aktiviert ist, steht
	der Befehl nicht zur Verfügung.
	
	Dieser Befehl steht nur den Services-Administratoren zur Verfügung.

CHAN_OPER_HELP_FORBID
	Syntax: FORBID Raum
	
	Verbietet die Benutzung des angegebenen Raumes. Der Verbot
	wird nur mit DROP aufgehoben.
	
	Dieser Befehl steht nur den Services-Administratoren zur Verfügung.

CHAN_OPER_HELP_SUSPEND
	Syntax: SUSPEND [+Dauer] Raum Grund
	
	Verhindert, dass der Raum benutzt wird und dass sich jemand
	für den Raum identifiziert. UNSUSPEND hebt die Suspendierung
	auf. Der Parameter Dauer wird in der gleichen Syntax erwartet wie
	für den %s AKILL Befehl.
	
	Im Gegensatz zum Verbot (FORBID), verliert der Raum seine
	Information nicht. Die Registrierung kann ebenfalls verfallen.
	
	Dieser Befehl steht nur den Services-Administratoren zur Verfügung.

CHAN_OPER_HELP_UNSUSPEND
	Syntax: UNSUSPEND Raum
	
	Hebt die Suspendierung des Raums auf.
	
	Dieser Befehl steht nur den Services-Administratoren zur Verfügung.


###########################################################################
#
# MemoServ help messages
#
###########################################################################

MEMO_HELP
	Mit Hilfe von %S können Sie kleine Nachrichten zu anderen
	Personen schicken, unabhängig davon, ob die Person sich im
	Moment im IRC befindet oder nicht. Desweiteren können Memos
	an Räume(*) geschickt werden. Es ist erforderlich, dass der
	Sender und der Empfänger einen registrierten Chatnamen (bzw.
	registrierten Raum) haben. Um eine Liste der %S-Befehle
	zu bekommen, tippen Sie /msg %S HELP COMMANDS, um einen
	Befehl zu Nutzen, tippen Sie /msg %S Befehl, für
	Informationen über den Befehl tippen Sie /msg %S HELP Befehl.
MEMO_HELP_EXPIRES
	
	Hinweis: Memos können verfallen (werden automatisch gelöscht)
	%s nach dem Verschicken, es sei denn sie werden mit dem Befehl
	SAVE markiert.
#MEMO_HELP_END_LEVELS
#	
#	(*) Standardeinstellung ist, dass Chatnamen mit Stufe %s in
#	    der Zugriffsliste des Raumes die Raum-Memos lesen können.
#	    Diese Einstellung kann man mit dem %s-Befehl LEVELS ändern.
#MEMO_HELP_END_XOP
#	
#	(*) Jeder Chatter in der SOP-Liste eines Raumes kann Memos dieses
#	    Raumes lesen.

MEMO_HELP_COMMANDS
	%S versteht folgende Befehle:
	
	    SEND     Schickt das Memo zum gegebenen Chatnamen
	    LIST     Zeigt die eigenen Memos
	    READ     Zeigt ein Memo zum lesen
MEMO_HELP_COMMANDS_FORWARD
	    FORWARD  Leitet die Memos an Ihre E-Mail-Adresse weiter
MEMO_HELP_COMMANDS_SAVE
	    SAVE     Verhindert automatisches Löschen (Verfallen) des Memos
#MEMO_HELP_COMMANDS_DEL
#	    DEL      Löscht ein oder mehrere Memos
#	    SET      Setzt Optionen für Memos.
#	    INFO     Zeigt Informationen und Einstellungen für Memos.
MEMO_HELP_COMMANDS_IGNORE
	    IGNORE   Ignoriert Memos von einigen Usern

#MEMO_HELP_SEND
#	Syntax: SEND {Chatname | Raum} Text
#	
#	Schickt ein Memo (den Text) zum gegebenen Chatnamen oder Raum.
#	Beim schicken an einen Chatnamen wird der Empfänger über
#	das eintreffen des Memos informiert. Der Empfänger (Chatname
#	oder Raum) muss registriert sein.

#MEMO_HELP_LIST
#	Syntax: LIST [Liste | NEW]
#	
#	Zeigt die derzeitigen Memos. Wird der NEW Parameter
#	angeben, werden nur die neuen, nicht gelesenen Memos angezeigt.
#	Diese sind mit einem "*" markiert. Eine Liste kann die Auswahl
#	der anzuzeigenden Memos einschränken, wie in dem Beispiel:
#	
#	    LIST 2-5.7-9
#	       Übersicht über Memos 1 bis 5 und 7 bis 9 wird angezeigt.

#MEMO_HELP_LIST_EXPIRE
#	Syntax: LIST [Liste | NEW]
#	
#	Zeigt die derzeitigen Memos. Wird der NEW Parameter
#	angeben, werden nur die neuen, nicht gelesenen Memos angezeigt.
#	Diese sind mit einem "*" markiert. Memos, die nicht verfallen
#	sollen, werden mit einem "+" markiert. Eine Liste kann die Auswahl
#	der anzuzeigenden Memos einschränken, wie in dem Beispiel:
#	
#	    LIST 2-5.7-9
#	       Übersicht über Memos 1 bis 5 und 7 bis 9 wird angezeigt.

MEMO_HELP_READ
	Syntax: READ {Nummer | Liste | LAST | NEW}
	
	Zeigt Ihnen das spezifizierte Memo an. Wenn sie LAST angeben,
	wird das neueste Memo angezeigt. Wenn sie NEW benutzen,
	werden alle nicht gelesenen Memos angezeigt. Außerdem können
	Sie die Nummer des zu lesenden Memos angeben, oder eine Liste,
	wie beim Befehl LIST.

MEMO_HELP_SAVE
	Syntax: SAVE {Nummer | Liste}
	
	Markiert das angegebene Memo oder die angegebenen Memos, damit
	sie nicht verfallen und nicht gelöscht werden. Sie können eine
	Nummer, oder eine Liste, wie beim Befehl LIST, angeben.

MEMO_HELP_DEL
	Syntax: DEL {Nummer | Liste | ALL}
	
	Löscht das angegebene Memo oder die angegebenen Memos.
	Sie können über eine Liste mehrere Memos spezifizieren,
	die Sie löschen wollen, oder mit ALL alle Memos löschen.
	Beispiele:
	
	   DEL 1
	      Löscht das erste Memos.
	
	   DEL 2-5,7-9
	      Löscht Memos von 2 bis 5 und 7 bis 9.
	
	Achtung: Das Löschen eines Memos ist endgültig, und kann nicht
	           rückgängig gemacht werden.

MEMO_HELP_SET
	Syntax: SET Option Parameter
	
	Setzt verschiedene Optionen. Option ist ein Wert von:
	
	    NOTIFY      Bestimmt, wann man bei einem neuen
	                Memo benachrichtigt wird
	    LIMIT       Bestimmt die maximale Anzahl der Memos
	                die man bekommen kann
MEMO_HELP_SET_OPTION_FORWARD
	    FORWARD     Bestimmt, ob die Memos automatisch per
	                E-Mail weitergeleitet werden
MEMO_HELP_SET_END
	
	/msg %S HELP SET Option für weitere Informationen
	über bestimmte Option.

MEMO_HELP_SET_NOTIFY
	Syntax: SET NOTIFY {ON | LOGON | NEW | OFF}
	
	Bestimmt, wann Sie über neue Memos benachrichtigt werden.
	
	    ON      Beim Anmelden, beim Beenden von /AWAY und
	            wenn neue Memos eintreffen
	    LOGON   Beim Anmelden und beim Beenden von /AWAY
	    NEW     Beim eintreffen neuer Memos
	    OFF     Keine Benachrichtigung
	
	ON ist eine Kombination aus LOGON und NEW.
	
	Hinweis: wenn Ihr Chatname die Option SECURE hat, werden
	Sie erst dann benachrichtigt, wenn sie sich dem %s
	gegenüber identifiziert haben.

#MEMO_HELP_SET_LIMIT
#	Syntax: SET LIMIT [Raum] Limit
#	
#	Bestimmt die maximale Anzahl von Memos, die Sie oder
#	der Raum haben kann. Wenn Sie das auf 0 setzen, erhalten
#	Sie keine Memos. Diese Zahl kann nicht größer als %d sein.

#MEMO_HELP_INFO
#	Syntax: INFO [Raum]
#	
#	Zeigt die Anzahl der Memos (gelesen und ungelesen), wie viele
#	Memos Sie noch empfangen können und die Benachrichtigungsoption.
#	Wenn der Raum angegeben wird, werden die Informationen über den
#	angegebenen Raum angezeigt.

MEMO_OPER_HELP_COMMANDS
	
	Services-Administratoren können die Befehle SET LIMIT und
	INFO für alle Chatnamen benutzen. Nicht nur für ihre eigenen.

#MEMO_OPER_HELP_SET_LIMIT
#	Syntax: SET LIMIT [Chatname | Raum] {Limit | NONE | DEFAULT} [HARD]
#	
#	Setzt die maximale Anzahl der Memos für einen Chatnamen oder
#	Raum. Wenn auf 0 gesetzt, kann der Benutzer oder Raum keine
#	Memos erhalten. Wenn auf NONE gesetzt, dann gibt es keinen
#	Limit. Wenn auf DEFAULT gesetzt, wird der Standardwert (%d)
#	verwendet. Ohne Angabe des Chatnamens oder Raumes setzen Sie die
#	Optionen für Ihren eigenen Chatnamen.
#	
#	Der Parameter HARD führt dazu, dass der Benutzer diese nicht
#	eigenmächtig verändern kann. Das Weglassen des Parameters erlaubt
#	dem User die Änderung. Auch, wenn vorheriger Limit mit der
#	Option HARD gesetzt war.
#	
#	Diese Version des Befehls steht nur den Services-Administratoren
#	zur Verfügung. Andere Benutzer können nur Limits für eigene
#	Chatnamen oder Räume setzen, diese aber NICHT aufheben und keine
#	Limits größer als %d setzen, oder verändern, wenn diese mit HARD
#	gesetzt wurden.

#MEMO_OPER_HELP_INFO
#	Syntax: INFO [Chatname | Raum]
#	
#	Zeigt Information über die gelesenen und nicht gelesenen Memos
#	und über MemoServ-relevanten Einstellungen.
#	
#	Wenn Sie einen Raum angeben, werden Information über diesen Raum
#	angezeigt.
#	
#	Wenn Sie einen Chatnamen angeben, werden Information über
#	diesen Chatnamen gezeigt. Diese Version des Befehls steht nur
#	den Services-Administratoren zur Verfügung.

MEMO_HELP_FORWARD
	Syntax: FORWARD {Nummer | Liste | ALL}
	
	Leitet ein oder mehrere Memos an Ihre E-Mail-Adresse. Sie müssen
	eine gültige E-Mail-Adresse für ihren Chatnamen haben, um diesen
	Befehl zu nutzen. Raum-Memos können nicht weitergeleitet werden.

MEMO_HELP_SET_FORWARD
	Syntax: SET FORWARD {ON | COPY | OFF}
	
	Bestimme, ob %S die Memos automatisch an Ihre E-Mail-Adresse
	weitergeleitet soll. Wenn Sie ON angeben, werden alle
	ankommende Memos weitergeleitet und nicht gespeichert.
	Wenn Sie COPY angeben, werden die Memos weitergeleitet
	und gespeichert in der %S-Datenbank. Wenn Sie OFF angeben,
	werden die Memos, wie gewöhnlich, gespeichert. Wenn auf ON
	gesetzt, werden sie die Memos weiterhin erhalten, wenn die
	Weiterleitung fehlschlägt.
	
	Hinweis: wenn sie COPY angeben und Sie den Limit der Memos
	erreicht haben, werden Sie keine Memos mehr erhalten (auch
	nicht per E-Mail), bis Sie einige mit dem Befehl DEL gelöscht
	haben.

MEMO_HELP_IGNORE
	Syntax: IGNORE {ADD | DEL | LIST} [Chatname | Maske]
	
	Administriert die Liste der User, die ignoriert werden sollen.
	User, deren Chatname oder Maske (user@host) einem Eintrag der
	Liste entsprechen, können Ihnen keine Memos schicken.
	Die Option ADD fügt den angegebenen Chatnamen oder die
	angegebene Maske der Liste hinzu. Die Option DEL löscht
	den Chatnamen/Maske. Die Option LIST zeigt die aktuelle
	Ignore-Liste an.
	
	Hinweis: der Chatname oder Maske kann Platzhalter ("*" und "?")
	enthalten. Groß- Kleinschreibung wird ignoriert. Die Maske
	"*evil*" passt z.B. auf folgende Chatnamen: "EvilNick",
	"AN_EVIL_USER" und "devil".

###########################################################################
#
# OperServ help messages
#
###########################################################################

OPER_HELP
	%S erlaubt den IRC-Operatoren verschiedene Optionen
	des IRC-Netzes zu kontrollieren. Um die Liste der Befehle
	zu erhalten, tippen Sie /msg %S HELP COMMANDS
	
	Hinweis: Alle Befehle des %S werden protokolliert!

OPER_HELP_COMMANDS
	%S versteht folgende Befehle:
	    GLOBAL      Schickt ein Notice zu allen Benutzern
	    STATS       Statistiken über Services und Netzwerk
	    OPER LIST   Liste der Services-Operatoren
	    ADMIN LIST  Liste der Services-Administratoren
OPER_HELP_COMMANDS_SERVOPER
	
	Befehle nur für Services-Operatoren und Services-Administratoren:
	    MODE        Setzt Modi für dem Raum
	    KICK        Kickt Benutzer aus dem Raum
	    CLEARMODES  Setzt alle Modes von einem Raum neu
	    CLEARCHAN   Ein Masskick für den Raum
	    KILLCLONES  Killt alle Benutzer von einem Host
OPER_HELP_COMMANDS_AKILL
	    AKILL       Bearbeitet die Akill-Liste
	    AKILLCHAN   Macht Einträge für die User eines Raumes in der
	                   Akill-Liste hinzu
OPER_HELP_COMMANDS_EXCLUDE
	    EXCLUDE     Bearbeitet die Ausnahmen zu Autokill
OPER_HELP_COMMANDS_SLINE
	    SGLINE      Bearbeitet die SGLINE-Liste
	    SQLINE      Bearbeitet die SQLINE-Liste
	    SZLINE      Bearbeitet die SZLINE-Liste
OPER_HELP_COMMANDS_SESSION
	    SESSION     Zeigt die Liste der Host-Sitzungen
	    EXCEPTION   Bearbeitet die Sitzungslimit-Ausnahmen
OPER_HELP_COMMANDS_NEWS
	    LOGONNEWS   Bearbeitet die Liste der Logon-News
	    OPERNEWS    Bearbeitet die Liste der Oper-News
OPER_HELP_COMMANDS_SERVADMIN
	
	Befehle nur für Services-Administratoren:
	    OPER        Bearbeitet die Liste der Services-Operatoren
	    JUPE        Setzt ein Jupiter über ein Server
	    SET         Setzt Services-Optionen
	    UPDATE      Speichert die Services-Datenbank
	    SHUTDOWN    Speichert die Datenbank und beendet Services
	    QUIT        Beendet Services
	    RESTART     Speichert die Datenbank und startet Services neu
	    REHASH      Liest die Konfigurationsdatei neu
	    SU          Gibt einem Admin Services-Root Rechte
OPER_HELP_COMMANDS_SERVROOT
	
	Befehle nur für den Services-Root:
	    ADMIN       Bearbeitet die Liste der Services-Administratoren
OPER_HELP_COMMANDS_RAW
	    RAW         Schickt ein RAW Befehl zum IRC Server

OPER_HELP_GLOBAL
	Syntax: GLOBAL Text
	
	Ermöglicht IRC-Operatoren die Nachricht an jeden User im
	IRC-Netz zu schicken.  Der Text ist ein Notice vom Chatnamen
	%s.

OPER_HELP_STATS
	Syntax: STATS [RESET | NETWORK | ALL]
	
	Ohne Optionen zeigt dieser Befehl die momentane Anzahl
	der Chatter und IRCops, die online sind (ohne die Services),
	die maximale Anzahl der User seitdem die Services gestartet
	wurden und die Zeit seitdem die Services laufen.
	
	Mit der RESET-Option wird der Zähler für die maximale
	Useranzahl  auf den aktuellen Wert der verbundenen User
	gesetzt.
	
	Mit der NETWORK-Option wird die Menge der Daten, die
	von und zum Server gesendet wurden, angezeigt, sowie der
	Speicherverbrauch der Sende- und Empfangspuffer. Wenn ein
	Limit konfiguriert wurde, wird auch das Verhältnis des
	Speicherbedarfs zum limit in Prozent angezeigt.
	
	Die ALL-Option ist nur für Services-Administratoren verfügbar und
	zeigt Information über den Speicherbedarf der Services an.
	Diese Option kann Services für kurze Zeit auf großen Netzen
	einfrieren, deshalb bitte nicht zu oft gebrauchen.
	
	UPTIME kann als Synonym für STATS benutzt werden.

OPER_HELP_SERVERMAP
	Syntax: SERVERMAP
	
	Zeigt eine Karte der Server in Ihrem IRC-Netz - gesehen aus
	der Position der Services. Fake-Server, Services selbst und
	Jupiter-Server werden durch ein "(*)" markiert.

OPER_HELP_OPER
	Syntax: OPER ADD Chatname
	        OPER DEL Chatname
	        OPER LIST
	
	Ermöglicht die Listen der Services-Administratoren und Services-Operator
	zu bearbeiten. Alle Benutzer in dieser Liste können die Befehle
	der Services-Operatoren benutzen, sofern sie sich dem %s
	gegenüber identifiziert haben.
	
	OPER LIST kann jeder IRC-Oper benutzen. Die anderen
	stehen nur den Services-Administratoren zur Verfügung.

OPER_HELP_ADMIN
	Syntax: ADMIN ADD Chatname
	        ADMIN DEL Chatname
	        ADMIN LIST
	
	Erlaubt dem Services-Root, die Liste der Services-Administratoren zu
	bearbeiten. Alle Benutzer in dieser Liste können die Befehle
	der Services-Administratoren benutzen, sofern sie sich dem %s
	gegenüber identifiziert haben.
	
	ADMIN LIST kann jeder IRC-Oper benutzen. Die anderen
	stehen nur dem Services-Root zur Verfügung.

### CHANGED: OPER_HELP_GETKEY added

OPER_HELP_MODE
	Syntax: MODE Raum Mode
	
	Erlaubt Services-Operatoren, Modi für jeden Raum zu setzen.
	Die Parameter sind die gleichen wie bei den normalen /MODE.
	
	Dieser Befehl steht nur den Services-Operatoren zur Verfügung.

#OPER_HELP_CLEARMODES
#	Syntax: CLEARMODES Raum [ALL]
#	
#	Setzt alle "binären" Modi (n, s, t, k, l, usw.) und alle Bans
#	im Raum zurück. Die Option ALL führt dazu, dass zusätzlich
#	allen Usern der Raum-Operator- (+o) und Moderator-Status (-v)
#	entzogen wird.
#	
#	Dieser Befehl steht nur den Services-Operatoren zur Verfügung.

OPER_HELP_CLEARCHAN
	Syntax: CLEARCHAN Raum
	
	Entfernt (kickt) alle User aus dem Raum (Masskick).
	
	Dieser Befehl steht nur den Services-Operatoren zur Verfügung.

OPER_HELP_KICK
	Syntax: KICK Raum Chatname Grund
	
	Ermöglicht IRC-Operatoren einen Benutzer aus einem Raum
	zu kicken. Die Parameter sind die gleichen, wie bei dem
	normalen /KICK. Der Grund erscheint im Kick-Text mit dem
	Chatnamen des Opers, der diesen Befehl benutzt hat.
	Zum Beispiel :
	
	*** SpamMan has been kicked off channel #Raum by %S
	(Alcan (Floodder!))
	
	Dieser Befehl steht nur den Services-Operatoren zur Verfügung.

#OPER_HELP_AKILL
#	Syntax: AKILL ADD [+Dauer] Mask Grund
#	        AKILL DEL Mask
#	        AKILL LIST [Mask] [NOEXPIRE]
#	        AKILL VIEW [Mask] [NOEXPIRE]
#	        AKILL COUNT
#	
#	Erlaubt Services-Operatoren die Autokill-Liste zu bearbeiten.
#	Wenn ein Benutzer aus der AKILL Liste sich mit dem IRC-Server
#	verbindet, wird Services den Benutzer per KILL entfernen und,
#	wenn die Server es unterstützen, sie instruieren einen Bann
#	(K-Line) zu setzen setzen.
#	
#	AKILL ADD trägt die user@host Maske in die AKILL-Liste
#	mit dem Grund ein. Der Grund muss angegeben werden.
#	AKILL DEL löscht den AKILL-Eintrag aus der Liste.
#	AKILL LIST allein zeigt die komplette AKILL Liste an.
#	Wenn eine Suchmaske angegeben wurde, werden nur die passenden
#	Einträge angezeigt.
#	AKILL VIEW zeigt detailiertere Informationen als AKILL LIST:
#	wer diesen AKILL hinzugefügt hat, wann der Eintrag hinzugefügt
#	wurde, wann er ablaufen soll, die Maske und den Grund.
#	Wenn die Option NOEXPIRE angegeben ist, werden nur die Einträge
#	angezeigt, die nicht ablaufen.
#	AKILL COUNT zeigt einfach die Anzahl der Einträge.
#	
#	Mit AKILL ADD kann eine Dauer angegeben werden, die besagt,
#	wann der Eintrag ablaufen soll. Diese sollte mit einer
#	Zeiteinheit versehen werden: d (Tage), h (Stunden)
#	oder m (Minuten). Eine Kombination (z.B. 1h30m) ist
#	auch erlaubt. Der Standardeinheit ist ein Tag, +30
#	bedeutet somit 30 Tage. Ein Eintrag, der nicht ablaufen
#	soll, muss mit einer Zeitangabe von +0 versehen werden.
#	Die Dauer muss angegeben werden, wenn die Maske (user@host)
#	mit einem Plus (+) anfängt.
#	
#	Der Grund soll beschreiben, warum der Eintrag hinzugefügt
#	wurde. Er wird, wenn Services entsprechend konfiguriert sind,
#	bei der KILL-Nachricht angezeigt.
OPER_HELP_AKILL_OPERMAXEXPIRY
	
	Services-Operatoren können die Dauer bis %s setzen,
	Services-Administrators können beliebige Dauer setzen.
OPER_HELP_AKILL_END
	
	Dieser Befehl steht nur den Services-Operatoren zur Verfügung.

OPER_HELP_AKILLCHAN
	Syntax: AKILLCHAN [KILL] [+Dauer] Raum Grund
	
	Erstellt Einträge in der AKILL-Liste für alle Chatter in einem
	bestimmten Raum. IRC-Operatoren ausgeschlossen. Der optionale
	Parameter KILL entfernt die Chatter vom System. Der angegebene
	Grund wird in jedem AKILL-Eintrag gespeichert und u.U. als Grund
	des Kills angegeben. Die Dauer hat die gleiche Syntax wie beim
	AKILL ADD Befehl. Wird keine Dauer übergeben, wird der Default von
	%s genommen.
	
	AKILLs, die mit diesem Befehl hinzugefügt wurden, haben die Syntax
	*@hostname für jeden Hostnamen, der von einem Chatter im
	Raum benutzt wurde. Ein vorhandener AKILL auf diesen Hostnamen
	wird nicht modifiziert, daher kann die Anzahl der angegebenen AKILLs
	niedriger ausfallen als die Anzahl der im Raum anwesenden Chatter.
	
	Dieser Befehl steht nur Services-Operatoren zur Verfügung.

#OPER_HELP_EXCLUDE
#	Syntax: EXCLUDE ADD [+Dauer] Maske Grund
#	        EXCLUDE DEL Maske
#	        EXCLUDE LIST [Maske] [NOEXPIRE]
#	        EXCLUDE VIEW [Maske] [NOEXPIRE]
#	        EXCLUDE CHECK Maske
#	        EXCLUDE COUNT
#	
#	Erlaubt Services-Operatoren die Autokill-Ausnahmen-Liste zu
#	bearbeiten. Wenn ein Benutzer aus der Autokill-Ausnahmen-Liste
#	sich mit dem IRC-Server verbindet, wird er nicht durch Autokill
#	entfernt, auch, wenn der User einem Eintrag aus der Autokill-Liste
#	entspricht. Es ist nützlich, wenn Sie allgemeine Masken (user@host)
#	in Ihrer Autokill-Liste haben und manchen Usern die Verbindung
#	zum Server trotzdem erlauben möchten.
#	
#	Die Optionen ADD, DEL, LIST, VIEW, CHECK, und COUNT
#	funktionieren ähnlich, wie beim Befehl AKILL. Sehen Sie die
#	Hilfe für AKILL für weitere Informationen.
#	
#	Dieser Befehl steht nur den Services-Operatoren zur Verfügung.

#OPER_HELP_SGLINE
#	Syntax: SGLINE ADD [+Dauer] Maske Grund
#	        SGLINE DEL Maske
#	        SGLINE LIST [Maske] [NOEXPIRE]
#	        SGLINE VIEW [Maske] [NOEXPIRE]
#	        SGLINE COUNT
#	
#	Erlaubt Services-Operatoren die SGLINE-Liste zu bearbeiten.
#	Wenn ein User, dessen Realname einem Eintrag aus der SGLINE-Liste
#	entspricht, sich mit dem IRC-Server verbindet, wird Services
#	den Benutzer per KILL entfernen und, wenn die IRC-Server es
#	unterstützen, die Server instruieren, einen Bann (G-Line)
#	auf den Realnamen zu setzen.
#	
#	SGLINE ADD trägt die Realnamen-Maske in die SGLINE-Liste
#	mit dem Grund ein. Der Grund muss angegeben werden.
#	SGLINE DEL löscht den Eintrag aus der Liste.
#	SGLINE LIST zeigt alle SGLINE-Einträge, mit einer
#	Suchmaske nur die Einträge, die der Suchmaske entsprechen.
#	SGLINE VIEW zeigt detailiertere Informationen als SGLINE LIST:
#	wer diesen Eintrag hinzugefügt hat, wann der Eintrag hinzugefügt
#	wurde, wann er zuletzt benutzt wurde, wann er verfallen soll,
#	die Maske und den Grund.
#	Wenn die Option NOEXPIRE angegeben ist, werden nur die Einträge
#	angezeigt, die nicht ablaufen.
#	SGLINE COUNT zeigt einfach die Anzahl der Einträge.
#	
#	Die Maske kann in Gänsefüßchen stehen. Sie muss in Gänsefüßchen
#	stehen, wenn die Maske eine Leerzeichen enthält.  Beispiel:
#	    /msg %S SGLINE ADD +0 "bad name" Das ist ein unerwünschter Name
#	
#	Mit SGLINE ADD kann eine Dauer angegeben werden, die besagt,
#	wann der Eintrag ablaufen soll. Diese sollte mit einer
#	Zeiteinheit versehen werden: d (Tage), h (Stunden)
#	oder m (Minuten). Eine Kombination (z.B. 1h30m) ist
#	auch erlaubt. Der Standardeinheit ist ein Tag, +30
#	bedeutet somit 30 Tage. Ein Eintrag, der nicht ablaufen
#	soll, muss mit einer Zeitangabe von +0 versehen werden.
#	Die Maske muss in Gänsefüßchen angegeben werden, wenn die Maske
#	(user@host) mit einem Plus (+) anfängt und keine Dauer
#	angegeben wurde.
#	
#	Der Grund soll beschreiben, warum der Eintrag hinzugefügt
#	wurde. Er wird, wenn Services entsprechend konfiguriert sind,
#	bei der KILL-Nachricht angezeigt.
#	
#	Dieser Befehl steht nur den Services-Operatoren zur Verfügung.

#OPER_HELP_SQLINE
#	Syntax: SQLINE ADD [+Dauer] Maske Grund
#	        SQLINE DEL Maske
#	        SQLINE LIST [Maske] [NOEXPIRE]
#	        SQLINE VIEW [Maske] [NOEXPIRE]
#	        SQLINE CHECK Maske
#	        SQLINE COUNT
#	
OPER_HELP_SQLINE_KILL
	Erlaubt Services OPERs die SQLINE liste zu manipulieren.
	Sobald ein Chatter, dessen Chatname auf einen SQLINE Eintrag
	passt, eine Verbindung zum System aufbaut, oder seinen Chatnamen
	zu einem ändern möchte, der auf der Liste steht, wird er von
	den Services "gekillt". Wenn die Server das unterstützen, wird
	auf jedem Server eine Q-Line gesetzt.
OPER_HELP_SQLINE_NOKILL
	Erlaubt Services OPERs die SQLINE liste zu manipulieren.
	Sobald ein Chatter, dessen Chatname auf einen SQLINE Eintrag
	passt, eine Verbindung zum System aufbaut, oder seinen Chatnamen
	zu einem ändern möchte, der auf der Liste steht, wird er von
	den Services zu einem Gast umbenannt. Der Chatter bekommt eine
	Nachricht, die normalerweise dem Chatprogramm mitteilt, einen
	alternativen Chatnamen zu benutzen. Wenn die Server das
	unterstützen, wird auf jedem Server eine Q-Line gesetzt.
OPER_HELP_SQLINE_IGNOREOPERS
	
	Bitte beachten: IRC Ops sind von Einträgen in der SQLINE-liste
	nicht betroffen.
#OPER_HELP_SQLINE_END
#	
#	Die Optionen ADD, DEL, LIST, VIEW und COUNT bewirken
#	das Hinzufügen, Löschen, Anzeigen (Kurz- oder Langform) eines
#	Eintrags oder die Anzeige der Anzahl der Einträge der SQLINE-Liste.
#	Dieser Befehl funktioniert dem Befehl SGLINE ähnlich. Sehen Sie
#	die Hilfe für SGLINE (/msg %S HELP SGLINE) für weitere
#	Informationen.
#	
#	Dieser Befehl steht nur den Services-Operatoren zur Verfügung.

#OPER_HELP_SZLINE
#	Syntax: SZLINE ADD [+Dauer] Maske Grund
#	        SZLINE DEL Maske
#	        SZLINE LIST [Maske] [NOEXPIRE]
#	        SZLINE VIEW [Maske] [NOEXPIRE]
#	        SZLINE COUNT
#	
#	Erlaubt Services-Operatoren die SZLINE-Liste zu bearbeiten.
#	Wenn ein User, dessen IP-Adresse einem Eintrag aus der SZLINE-Liste
#	entspricht, sich mit dem IRC-Server verbindet, wird Services
#	den Benutzer per KILL entfernen und, wenn die IRC-Server es
#	unterstützen, die Server instruieren, einen Bann (Z-Line)
#	auf die IP-Adresse zu setzen.
#	
#	Die Optionen ADD, DEL, LIST, VIEW und COUNT bewirken
#	das Hinzufügen, Löschen, Anzeigen (Kurz- oder Langform) eines
#	Eintrags oder die Anzeige der Anzahl der Einträge der SZLINE-Liste.
#	Dieser Befehl funktioniert dem Befehl SGLINE ähnlich. Sehen Sie
#	die Hilfe für SGLINE (/msg %S HELP SGLINE) für weitere
#	Informationen.
#	
#	Dieser Befehl steht nur den Services-Operatoren zur Verfügung.

#OPER_HELP_EXCEPTION
#	Syntax: EXCEPTION ADD [+Dauer] Maske Limit Grund
#	        EXCEPTION DEL {Maske | Liste}
#	        EXCEPTION MOVE Nummer Position
#	        EXCEPTION LIST [Maske | Liste]
#	        EXCEPTION VIEW [Mask | Liste]
#	
#	Erlaubt Services-Operatoren die Liste der Hosts mit spezifischen
#	Sitzungslimit zu bearbeiten (z.B. Shell Servers) um einigen
#	Rechnern, wie z.B. Shell-Provider, eine größere Anzahl der
#	Client-Verbindungen zu erlauben, als die Standardeinstellung.
#	Wenn ein Rechner seinen Sitzungslimit erreicht hat, werden
#	alle weiteren Benutzer abgewiesen (gekillt). Bevor ein
#	Benutzer gekillt ist, wird er per /NOTICE vom %S über
#	Sitzungslimits informiert. Der Inhalt dieser Nachricht
#	ist eine Konfigurationseinstellung.
#	
#	EXCEPTION ADD trägt die Hostmaske in die Ausnahme-Liste ein.
#	Beachten Sie, dass nick!user@host und user@host nicht
#	gültig sind, weil Chatnamen und UserIDs dabei nicht relevant sind.
#	Nur Rechnernamen, wie host.domain.dom und Masken *.domain.dom
#	sind erlaubt. Die Limitierung der Sitzungen hängt nicht vom
#	Chatnamen oder UserID ab. Limit muss eine Zahl größer oder
#	gleich Null sein, und zeigt, wie viele Benutzer von diesem Host
#	erlaubt sind. Die Zahl 0 bedeutet keine Begrenzung.
#	EXCEPTION DEL löscht den Host von der Ausnahme-Liste.
#	EXCEPTION MOVE schiebt die Ausnahme Nummer nach Position.
#	Die Ausnahmen dazwischen werden hoch oder runter geschoben.
#	EXCEPTION LIST und EXCEPTION VIEW zeigen alle aktuellen
#	Ausnahmen. Bei der Angabe der Maske werden nur die angezeigt, die
#	dieser entsprechen.
#	Der Unterschied von EXCEPTION VIEW ist, dass VIEW mehr informativ
#	ist und enthält die Angaben darüber, wer die Ausnahme eingetragen hat
#	und wann die Ausnahme ablaufen wird.
#	
#	Mit EXCEPTION ADD kann eine Dauer angegeben werden, die besagt,
#	wann der Eintrag ablaufen soll. Diese sollte mit einer
#	Zeiteinheit versehen werden: d (Tage), h (Stunden)
#	oder m (Minuten). Eine Kombination (z.B. 1h30m) ist
#	auch erlaubt. Der Standardeinheit ist ein Tag, +30
#	bedeutet somit 30 Tage. Ein Eintrag, der nicht ablaufen
#	soll, muss mit einer Zeitangabe von +0 versehen werden.
#	Die Dauer muss angegeben werden, wenn die Host-Maske
#	mit einem Plus (+) anfängt.
#	
#	Zu beachten ist, dass der Server den ersten passenden Eintrag
#	aus der Liste benutzt. Lange Ausnahme-Listen beeinflussen die
#	Performance der Services.
#	
#	Dieser Befehl steht nur den Services-Operatoren zur Verfügung.

OPER_HELP_SESSION
	Syntax: SESSION LIST Grenzwert
	        SESSION VIEW Host
	
	Zeigt den Services-Operatoren die Sitzungs-Liste.
	
	SESSION LIST zeigt Hosts mit mindestens Grenzwert
	Sitzungen. Der Grenzwert ist eine Zahl größer als 1. Dies
	sollte verhindern, dass versehentlich alle Hosts mit einer
	Sitzung aufgelistet werden.
	
	SESSION VIEW zeigt detaillierte Information über eine
	Sitzung, sowie wie viele Sitzungen aktiv sind und Sitzungs-Limits.
	Host darf keine Platzhalter beinhalten.
	
	Konsultieren Sie Hilfe für EXCEPTION für weitere
	Informationen über Sitzungslimits und deren Bearbeitung.
	
	Dieser Befehl steht nur den Services-Operatoren zur Verfügung.

OPER_HELP_SU
	Syntax: SU Passwort
	
	Gibt einem Services-Admin, Services-Root Rechte. Das Passwort
	wird mit dem Befehl SET SUPASS gesetzt, und muss gesetzt sein,
	bevor man SU Benutzen kann. Die Services-Root Rechte bleiben
	erhalten, bis die IRC Sitzung beendet wird.
	
	WARNUNG: Wenn ein falsches Passwort angegeben wird, wird eine
	Warnmeldung an alle IRC-Operatoren geschickt.
	
	Dieser Befehl steht nur den Services-Administratoren zur Verfügung.

OPER_HELP_SET
	Syntax: SET Option Parameter
	
	Setzt verschiedene globale Optionen der Services. Aktuelle
	Optionen sind:
	    READONLY   Setzt den Nur-Lese-Modus ein oder aus
	    DEBUG      Aktiviert oder deaktiviert den Debug-Modus
	    SUPASS     Setzt das Passwort für den Befehl SU
	
	Dieser Befehl steht nur den Services-Administratoren zur Verfügung.
	Das SU-Passwort kann nur von Services-Root gesetzt werden.

OPER_HELP_SET_READONLY
	Syntax: SET READONLY {ON | OFF}
	
	Setzt den Nur-Lese-Modus ein oder aus. Wenn im Nur-Lese-Modus,
	können keine Modifikationen der Services-Daten vorgenommen
	werden, incl. Modifikationen der Zugriffslisten der Chatnamen
	oder Räume. IRC-Operatoren mit ausreichenden Rechten können
	die Autokill-Listen modifizieren und Chatnamen oder Räume
	verbieten (FORBID) oder löschen (DROP). Die Daten werden
	jedoch nicht gespeichert, wenn man vor dem Shutdown oder
	Restart der Services den Nur-Lese-Modus nicht deaktiviert
	hat.
	
	Dies entspricht dem Kommandozeilenparameter -readonly.

OPER_HELP_SET_DEBUG
	Syntax: SET DEBUG {ON | OFF | Nummer}
	
	Aktiviert oder deaktiviert den Debug-Modus. Im Debug-Modus
	werden alle Daten, die Services empfangen oder verschicken
	und andere Debug-Informationen in einer Log-Datei protokolliert.
	Wenn Sie die Nummer angeben, wird der Debug-Modus eingeschaltet
	und die Debug-Stufe auf die angegebene Nummer gesetzt.
	
	Dies entspricht dem Kommandozeilenparameter -debug.

OPER_HELP_SET_SUPASS
	Syntax: SET SUPASS [Passwort]
	
	Setzt das Passwort, das beim Befehl SU benutzt wird. Das
	Passwort muss gesetzt werden, bevor der Befehl SU benutzt
	werden kann. Wenn kein Passwort gesetzt ist, steht der
	Befehl SU nicht zur Verfügung.
	
	Dies steht nur den Services-Root zur Verfügung.

OPER_HELP_JUPE
	Syntax: JUPE Server [Grund]
	
	Setzt ein Jupiter auf den gegebenen Server, indem Services
	einen Fake-Server mit dem IRC-Netz verbindet, damit sich der
	tatsächliche Server mit dem selben Namen nicht mehr verbinden
	kann. Der Jupiter kann per /SQUIT entfernt werden. Wenn
	ein Grund angegeben ist, wird er bei der Serverinformation
	angezeigt. Andernfalls wird der Text "Jupitered by <Chatname>"
	angezeigt.
	
	Dieser Befehl steht nur den Services-Administratoren zur Verfügung.

#OPER_HELP_RAW
#	Syntax: RAW Text
#	
#	Schickt eine Zeichenkette direkt zum Server, zu dem Services
#	verbunden ist (RAW). Dieser Befehl hat nur wenige Bereiche,
#	wo er eingesetzt werden kann und kann Ihr IRC-Netz stören
#	oder einen Absturz der Services verursachen, wenn falsch
#	benutzt. Dieser Befehl sollte NICHT benutzt werden,
#	wenn Sie sich nicht sicher sind, was Sie tun.
#	
#	Dieser Befehl steht nur den Services-Root zur Verfügung.

OPER_HELP_UPDATE
	Syntax: UPDATE [FORCE]
	
	Veranlasst Services dazu, alle Datenbankdateien wegzuspeichern.
	Die Option FORCE sorgt dafür, dass versucht wird, eine sperre
	auf die Dateien aufzuheben. Damit kann die Fehlermeldung "databases
	are locked" umgangen werden.
	Diesen Befehl können nur Services Admins benutzen.

OPER_HELP_QUIT
	Syntax: QUIT
	
	Beendet Services ohne die Datenbank zu speichern.
	Dieser Befehl sollte nur dann benutzt werden, wenn die
	Daten im Hauptspeicher korrupt sind und man die korrupten
	Daten nicht speichern möchte.
	Für das normale beenden der Services sollen die den Befehl
	SHUTDOWN benutzen.
	
	Dieser Befehl steht nur den Services-Administratoren zur Verfügung.

OPER_HELP_SHUTDOWN
	Syntax: SHUTDOWN
	
	Speichert die Datenbank, und dann beendet Services.
	
	Dieser Befehl steht nur den Services-Administratoren zur Verfügung.

OPER_HELP_RESTART
	Syntax: RESTART
	
	Speichert die Datenbank und startet Services neu.
	
	Dieser Befehl steht nur den Services-Administratoren zur Verfügung.

OPER_HELP_REHASH
	Syntax: REHASH
	
	Veranlasst die Services dazu, die Konfiguration neu zu lesen
	und die Einstellungen zu übernehmen. Einige Einstellungen,
	wie z.B. RemoteServer (der Hostname des IRC-Servers, mit dem
	sich Services verbinden) können nicht durch den Befehl REHASH
	geändert werden. Solche Änderungen sind nur dann wirksam, wenn
	die Services neu gestartet werden. In solchen Fällen wird eine
	Meldung in der Log-Datei der Services und eine NOTICE Sie
	darauf hinweisen.
	
	Wenn die Optionen LoadModule in die Hauptkonfigurationsdatei
	(%s) neu aufgenommen oder entladen wurden, werden
	Module entsprechend geladen oder entladen. Module, die in
	der Konfiguration nicht mehr geladen sind, werden entladen.
	Danach werden die Module in der Reihenfolge geladen, wie sie
	in der Konfigurationsdatei stehen. Wenn ein Fehler beim Laden
	oder Entladen der Module auftritt, wird der REHASH abgebrochen.
	
	Hinweis: es kann vorkommen, dass aufgrund Abhängigkeiten
	der Module, einige Module nicht geladen oder entladen werden
	können. In einem solchen Fall müssen Sie die Services neu
	starten, um die Änderung der Konfiguration wirksam werden
	zu lassen.
	
	Dieser Befehl steht nur den Services-Administratoren zur Verfügung.

OPER_HELP_KILLCLONES
	Syntax: KILLCLONES Chatname
	
	Entfernt (Kill) alle Benutzer vom IRC-Netz, die von dem selben
	Host verbinden (Clones). Ein temporärer AKILL der Form *@host,
	wird aktiviert, um sofortige Versuche des Neu-Verbindens zu
	Unterbinden. Außerdem wird ein WALLOPS gesendet, mit den
	Informationen, wer den Befehl benutzt hat, welcher Host und
	wie viele Benutzer betroffen werden. Um Klone loszuwerden,
	ist es sehr hilfreich.
	
	Dieser Befehl steht nur den Services-Operatoren zur Verfügung.

###########################################################################
#
# StatServ help messages
#
############################################################################

STAT_HELP
	%S gibt statistische Information über das IRC-Netzwerk,
	die Server und die Benutzer. Um sich die Liste der verfügbaren
	%S-Befehle anzeigen zu lassen, tippen Sie /msg %S HELP COMMANDS,
	um sich Hilfe zu einem bestimmten Befehl anzeigen zu lassen,
	tippen Sie /msg %S HELP Befehl.

STAT_HELP_COMMANDS
	%S versteht folgende Befehle:
	    SERVERS     Server-Statistiken
	    USERS       Benutzer-Statistiken

STAT_HELP_SERVERS
	Syntax: SERVERS STATS
	        SERVERS LIST [Maske]
	        SERVERS VIEW [Maske] [ONLINE | OFFLINE]
	
	Zeigt statistische Information über die Server im Netzwerk an.
	
	SERVERS STATS zeigt eine kurze Zusammenfassung der verfügbaren
	(online) und der unsichtbaren (offline) Server an.
	SERVERS LIST zeigt eine Liste der verfügbaren Server und kurze
	Statistiken über diese Server an.
	SERVERS VIEW zeigt eine Liste der verfügbaren Server und eine
	detaillierte Statistik über diese Server an. Die ONLINE und
	OFFLINE Optionen bewirken, dass die verfügbaren, oder die offline
	gegangenen Server angezeigt werden.
	
	Wenn die Suchmaske angegeben ist, werden nur die Server angezeigt,
	die der Suchmaske entsprechen.

STAT_HELP_USERS
	Syntax: USERS STATS
	
	Zeigt Statistik über die Benutzer des Netzwerks.

STAT_OPER_HELP_SERVERS
	Syntax: SERVERS STATS
	        SERVERS LIST [Maske]
	        SERVERS VIEW [Maske] [ONLINE | OFFLINE]
	        SERVERS DELETE Server
	        SERVERS COPY Server NeuerName
	        SERVERS RENAME Server NeuerName
	
	Zeigt statistische Information über die Server im Chat-Netzwerk an.
	
	SERVERS STATS zeigt eine kurze Zusammenfassung der verfügbaren
	(online) und der unsichtbaren (offline) Server an.
	SERVERS LIST zeigt eine Liste der verfügbaren Server und kurze
	Statistik über diese Server an.
	SERVERS VIEW zeigt eine Liste der verfügbaren Server und eine
	detaillierte Statistik über diese Server an. Die ONLINE und
	OFFLINE Optionen bewirken, dass die verfügbaren, oder die offline
	gegangenen Server angezeigt werden.
	
	Wenn die Suchmaske angegeben ist, werden nur die Server angezeigt,
	die der Suchmaske entsprechen.
	
	DELETE löscht gespeicherte Statistiken über den gegebenen Server.
	COPY dupliziert die Statistiken über den gegebenen Server.
	RENAME benennt Statistiken über den gegebenen Server um.
	
	Diese drei Befehle steht nur den Services-Administratoren zur Verfügung.

###########################################################################

# CVS: $Revision: 2.183 $
