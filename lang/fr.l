# Emacs: -*- mode: fundamental; coding: binary; -*-

# French language file.
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
#
# This translation courtesy of
#	elijah (admin@nevernet.net)
#	Maxime (maxime_imbeau@hotmail.com)

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
	Français (French)

# Character set used by this language.  This should be a "charset" value
# understood by mailreader or web browser software.

#LANG_CHARSET
#	iso-8859-1

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
	Dim
	Lun
	Mar
	Mer
	Jeu
	Ven
	Sam
# %A
STRFTIME_DAYS_LONG
	Dimanche
	Lundi
	Mardi
	Mercredi
	Jeudi
	Vendredi
	Samedi
# %b
STRFTIME_MONTHS_SHORT
	Jan
	Fev
	Mar
	Avr
	Mai
	Jun
	Jul
	Aou
	Sep
	Oct
	Nov
	Dec
# %B
STRFTIME_MONTHS_LONG
	Janvier
	Février
	Mars
	Avril
	Mai
	Juin
	Juillet
	Août
	Septembre
	Octobre
	Novembre
	Décembre


# "day", "hour", "minute", and "second" in singular and plural.
# These must be preceded by spaces if the language requires them
# (e.g. in English: "10 hours", not "10hours").
STR_DAY
	 jour
STR_DAYS
	 jours
STR_HOUR
	 heure
STR_HOURS
	 heures
STR_MINUTE
	 minute
STR_MINUTES
	 minutes
STR_SECOND
	 seconde
STR_SECONDS
	 secondes
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
	Erreur interne--impossible de donner suite à la requète.
SERVICES_IS_BUSY
	Les Services sont présentement occupés; s.v.p. essayer de nouveau plus tard.

UNKNOWN_COMMAND
	Commande inconnue %s.
UNKNOWN_COMMAND_HELP
	Commande inconnue %s.  Tapez /msg %s HELP pour de l'aide.
SYNTAX_ERROR
	Syntaxe: %s
MORE_INFO
	Tapez /msg %s HELP %s pour plus d'information.
NO_HELP_AVAILABLE
	Pas d'aide disponible pour %s.
MISSING_QUOTE
	Guillemets (") manquants.

BAD_EMAIL
	L'adresse courriel doit avoir la forme username@hostname. Vous ne pouvez utiliser de couleur, caractères gras, souligné, ou inversé ou les caractères suivants: , : ; | \ " ( ) < > [ ]
### CHANGED: REJECTED_EMAIL added
BAD_URL
	Les URLs doivent avoir la forme suivante http://hostname[:port]/... (or ftp://, etc.).
BAD_USERHOST_MASK
	Le masque doit prendre la forme user@host.
BAD_NICKUSERHOST_MASK
	Le masque doit prendre la forme surnom!user@host.
BAD_EXPIRY_TIME
	Temps d'expiration invalide.

SENDMAIL_NO_RESOURCES
	Le système est présentement trop surchargé pour expédier du courrier; s'il vous plait, essayez plus tard.
READ_ONLY_MODE
	Notice: Les Services sont présentement en mode lecture-seulement; les modifications ne seront pas sauvegardées!
PASSWORD_INCORRECT
	Mot de passe invalide.
PASSWORD_WARNING
	Attention: Après un autre mot de passe invalide vous serez déconnecté.

# What's the difference between "Access denied" and "Permission denied"?
# Very little; you can safely make them the same message with no real loss
# of meaning.  If you want to make a distinction, "Access denied" is
# usually used more generally; for example, a non-oper trying to access
# OperServ gets "Access denied", while a non-Services admin trying to use
# NickServ SET NOEXPIRE gets "Permission denied".
ACCESS_DENIED
	Accès refusé.
PERMISSION_DENIED
	Permission refusée.

MORE_OBSCURE_PASSWORD
	S.V.P. essayez plus tard avec un mot de passe plus compliqué.  Le mot de passe doit comporter au moins 5 caractères, et ne devrait pas être quelque chose facile à trouver (ex : votre vrai nom ou votre surnom).

NICK_NOT_REGISTERED
	Votre surnom n'est pas enregistré.
NICK_NOT_REGISTERED_HELP
	Votre surnom n'est pas enregistré.  Tapez /msg %s HELP pour de l'information sur l'enregistrement de votre surnom.
NICK_TOO_LONG
	Les surnoms ne doivent pas comporter plus de %d caractères.
NICK_INVALID
	%s n'est pas un surnom valide.
NICK_X_NOT_REGISTERED
	Le surnom %s n'est pas enregistré.
NICK_X_ALREADY_REGISTERED
	Le surnom %s est déjà enregistré!
NICK_X_NOT_IN_USE
	Le surnom %s n'est pas utilisé présentement.
NICK_X_FORBIDDEN
	Le surnom %s ne peut être enregistré ou utilisé.
NICK_X_SUSPENDED
	Le surnom %s est suspendu vous ne pouvez vous y être identifié ni l'utiliser.
NICK_X_SUSPENDED_MEMOS
	Le surnom %s est suspendu et ne peut pas recevoir de mémos.
NICK_IDENTIFY_REQUIRED
	Authentification par mot de passe requise pour l'utilisation de cette commande.
	Réessayez après avoit tapé /msg %s IDENTIFY mot de passe.
NICK_PLEASE_AUTH
	Votre surnom n'est pas encore authentifié.  S'il vous plait authentifiez-le en utilisant le code inclus dans le message expédié à: %s
NICK_X_NOT_ON_CHAN_X
	Le surnom %s n'est pas actuellement sur le canal %s.

CHAN_INVALID
	%s n'est pas valide pour un nom de canal.
CHAN_X_NOT_REGISTERED
	Le canal %s n'est pas enregistré.
CHAN_X_NOT_IN_USE
	Le canal %s n'est pas utilisé présentment.
CHAN_X_FORBIDDEN
	Le canal %s ne peut être enregistré ou utilisé.
CHAN_X_SUSPENDED
	Le canal %s est suspendu vous ne pouvez vous y être identifié ni l'utiliser.
CHAN_X_SUSPENDED_MEMOS
	Le canal %s est suspendu et ne peut pas recevoir de mémos.
CHAN_IDENTIFY_REQUIRED
	Authentification par mot de passe requise pour l'utilisation de cette commande.
	Réessayez après avoit tapé /msg %s IDENTIFY %s mot de passe.

SERV_X_NOT_FOUND
	Le server %s n'a pas été trouvé.

EXPIRES_NONE
	n'expirera pas
EXPIRES_IN
	expire dans %s

###########################################################################
#
# NickServ messages
#
###########################################################################

# Automatic messages
NICK_IS_REGISTERED
	Ce surnom appartient à quelqu'un d'autre. S.V.P. en choisir un autre. (S'il s'agit de votre surnom, tapez: /msg %s IDENTIFY mot de passe.)
NICK_IS_SECURE
	Ce surnom est enregistré et protégé.  S'il s'agit de votre surnom, tapez: /msg %s IDENTIFY mot de passe. Autrement, S.V.P. en choisir un autre.
NICK_MAY_NOT_BE_USED
	Ce surnom ne peut pas être utilisé. S.V.P. en choisir un autre.
DISCONNECT_IN_1_MINUTE
	Si vous ne changez pas à l'intérieur d'une minute, vous serez déconnecté.
DISCONNECT_IN_20_SECONDS
	Si vous ne changez pas à l'intérieur de 20 secondes, vous serez déconnecté.
DISCONNECT_NOW
	Ce surnom a déjà été enregistré; vous ne pouvez l'utiliser. S.V.P. en choisir un autre avant d'être déconnecté.
FORCENICKCHANGE_IN_1_MINUTE
	Si vous ne changez pas à l'intérieur d'une minute, Je changerai votre surnom.
FORCENICKCHANGE_IN_20_SECONDS
	Si vous ne changez pas à l'intérieur de 20 secondes, Je changerai votre surnom.
FORCENICKCHANGE_NOW
	Ce surnom a été enregistré; vous ne pouvez l'utiliser.  Votre surnom est maintenant changé pour %s.
NICK_EXPIRES_SOON
	Attention: Votre enregistrement de surnom va expirer dans %s.  Si vous désirez continuer de l'utiliser, S.V.P. identifiez-vous à %s en tapant: /msg %s IDENTIFY mot de passe
NICK_EXPIRED
	Attention: L'enregistrement de votre surnom a expiré.  S.V.P. ré-enregistrez votre surnom si vous désirez continuer à l'utiliser.

# REGISTER responses
NICK_REGISTER_SYNTAX
	REGISTER mot de passe [courriel]
NICK_REGISTER_REQ_EMAIL_SYNTAX
	REGISTER mot de passe courriel
NICK_REGISTRATION_DISABLED
	Désolé, l'enregistrement de surnom est temporairement hors service.
NICK_REGISTRATION_FAILED
	Désolé, l'enregistrement a échoué.
NICK_REG_PLEASE_WAIT
	S.V.P. attendez %s avant d'utiliser la commande REGISTER à nouveau.
NICK_CANNOT_BE_REGISTERED
	Le surnom %s ne peut pas être enregistré.
NICK_REGISTER_EMAIL_UNAUTHED
	L'adresse courriel que vous avez fourni n'a pas encore été autorisée.  S.V.P. utilisez la commande AUTH pour autoriser l'adresse avant d'enregistrer de nouveaux surnoms avec elle.
NICK_REGISTER_TOO_MANY_NICKS
	Vous avez trop de surnoms enregistrés ((%d, maximum de %d).  Vous ne pouvez en enregistrer d'autre à moins d'abandonner quelques uns de vos surnoms actuels.
NICK_REGISTERED
	Le surnom %s a été enregistré pour vous.
NICK_PASSWORD_IS
	Votre mot de passe est %s -- retenez-le pour usage ultérieur.

# IDENTIFY responses
NICK_IDENTIFY_SYNTAX
	IDENTIFY mot de passe
NICK_IDENTIFY_FAILED
	Désolé, l'identification a échouée.
NICK_IDENTIFY_EMAIL_MISSING
	Vous devez sélectionner une adresse courriel pour votre surnom avant de pouvoir continuer à l'utiliser. S.V.P. sélectionnez-en un maintenant avec la commande: /msg %s SET EMAIL addresse
NICK_IDENTIFY_SUCCEEDED
	Mot de passe accepté -- vous êtes maintenant reconnu.

# DROP responses
NICK_DROP_SYNTAX
	DROP mot de passe
NICK_DROP_WARNING
	Attention: La commande DROP dé-enregistrera tous vos surnoms liés!
NICK_DROP_DISABLED
	Désolé, le désenregistrement de surnom est temporairement hors-service.
NICK_DROP_FAILED
	Désolé, le désenregistrement de surnom a échoué.
NICK_DROPPED
	Votre surnom a été désenregistré.
NICK_DROPPED_LINKS
	Votre surnom ainsi que tous les surnoms qui y étaient liés ont été désenregistrés.

# DROPNICK responses
NICK_DROPNICK_SYNTAX
	DROPNICK surnom
NICK_X_DROPPED
	Le surnom %s a été désenregistré.
NICK_X_DROPPED_LINKS
	Le surnom %s ainsi que tous les surnoms qui y étaient liés ont été désenregistrés.

# SET responses
NICK_SET_SYNTAX
	SET option paramêtres
NICK_SET_OPER_SYNTAX
	SET [!surnom] option paramêtres
NICK_SET_DISABLED
	Désolé, la sélection d'options de surnom est temporairement hors-service.
NICK_SET_UNKNOWN_OPTION
	Option SET %s inconnue.

# SET PASSWORD responses
NICK_SET_PASSWORD_FAILED
	Désolé, le mot de passe ne peut changé.
NICK_SET_PASSWORD_CHANGED
	Mot de passe changé.
NICK_SET_PASSWORD_CHANGED_TO
	Mot de passe changé à %s.

# SET LANGUAGE responses
NICK_SET_LANGUAGE_SYNTAX
	SET LANGUAGE numéro
NICK_SET_LANGUAGE_UNKNOWN
	Numéro de langue inconnu %d.  Tapez : /msg %s HELP SET LANGUAGE pour la liste des langues.
NICK_SET_LANGUAGE_CHANGED
	Langue changée à %s.

# SET URL responses
NICK_SET_URL_CHANGED
	L'URL pour %s changée pour %s.

# SET EMAIL responses
### CHANGED: NICK_SET_EMAIL_PLEASE_WAIT added
NICK_SET_EMAIL_UNAUTHED
	L'adresse courriel que vous avez fourni n'a pas encore été autorisée.  S.V.P. utilisez la commande AUTH pour autoriser l'adresse d'abord.
NICK_SET_EMAIL_TOO_MANY_NICKS
	%s a présentement %d surnoms enregistrés (maximum de %d).  Le changement d'adresse courriel a échouée.
NICK_SET_EMAIL_CHANGED
	L'adresse courriel pour %s a été changée pour %s.

# SET INFO responses
NICK_SET_INFO_CHANGED
	 L'information pour %s a été changée.

# SET KILL responses
NICK_SET_KILL_SYNTAX
	SET KILL {ON | QUICK | OFF}
NICK_SET_KILL_IMMED_SYNTAX
	SET KILL {ON | QUICK | IMMED | OFF}
NICK_SET_KILL_ON
	La protection Kill est maintenant ON.
NICK_SET_KILL_QUICK
	La protection Kill est maintenant ON, avec un délais réduit.
NICK_SET_KILL_IMMED
	La protection Kill est maintenant ON, sans délais.
NICK_SET_KILL_IMMED_DISABLED
	L'option IMMED n'est pas disponible sur ce réseau.
NICK_SET_KILL_OFF
	La protection Kill est maintenant OFF.

# SET SECURE responses
NICK_SET_SECURE_SYNTAX
	SET SECURE {ON | OFF}
NICK_SET_SECURE_ON
	L'option Secure est maintenant ON.
NICK_SET_SECURE_OFF
	L'option Secure est maintenant OFF.

# SET PRIVATE responses
NICK_SET_PRIVATE_SYNTAX
	SET PRIVATE {ON | OFF}
NICK_SET_PRIVATE_ON
	L'option Private est maintenant ON.
NICK_SET_PRIVATE_OFF
	L'option Private est maintenant OFF.

# SET NOOP responses
NICK_SET_NOOP_SYNTAX
	SET NOOP {ON | OFF}

# SET HIDE responses
NICK_SET_HIDE_SYNTAX
	SET HIDE {EMAIL | USERMASK | QUIT} {ON | OFF}
NICK_SET_HIDE_EMAIL_ON
	Votre adresse courriel sera maintenant cachée de l'affichage %s INFO.
NICK_SET_HIDE_EMAIL_OFF
	Votre adresse courriel sera maintenant présentée lors de l'affichage %s INFO.
NICK_SET_HIDE_MASK_ON
	Votre dernier masque user@host appercu sera maintenant caché lors de l'affichage %s INFO.
NICK_SET_HIDE_MASK_OFF
	Votre dernier masque user@host appercu sera maintenant présentée lors de l'affichage %s INFO.
NICK_SET_HIDE_QUIT_ON
	Votre dernier message de sortie sera maintenant caché lors de l'affichage %s INFO.
NICK_SET_HIDE_QUIT_OFF
	Votre dernier message de sortie sera maintenant présenté lors de l'affichage %s INFO.

# SET TIMEZONE responses
NICK_SET_TIMEZONE_SYNTAX
	SET TIMEZONE {UTC-offset | fuseau horaire | DEFAULT}
NICK_SET_TIMEZONE_TO
	Votre fuseau horaire est sélectionné à UTC%c%d:%02d.
	L'heure actuelle dans ce fuseau horaire est %s.
NICK_SET_TIMEZONE_DEFAULT
	Votre fuseau horaire a été ramené au fuseau horaire des Services.

# SET MAINNICK responses
NICK_SET_MAINNICK_NOT_FOUND
	Le surnom %s n'est pas lié à votre surnom.
NICK_SET_MAINNICK_CHANGED
	Votre surnom principal a été changé pour %s.

# SET NOEXPIRE responses
NICK_SET_NOEXPIRE_SYNTAX
	SET [!surnom] NOEXPIRE {ON | OFF}
NICK_SET_NOEXPIRE_ON
	Le surnom %s n'expirera pas.
NICK_SET_NOEXPIRE_OFF
	Le surnom %s va expirer.

# UNSET responses
NICK_UNSET_SYNTAX
	UNSET {URL | EMAIL | INFO}
NICK_UNSET_SYNTAX_REQ_EMAIL
	UNSET {URL | INFO}
NICK_UNSET_OPER_SYNTAX
	UNSET [!surnom] {URL | EMAIL | INFO}
NICK_UNSET_OPER_SYNTAX_REQ_EMAIL
	UNSET [!surnom] {URL | INFO}
NICK_UNSET_URL
	L'URL pour %s unset.
NICK_UNSET_EMAIL
	L'adresse courriel %s unset.
NICK_UNSET_INFO
	L'information pour %s unset.
NICK_UNSET_EMAIL_BAD
	Vous ne pouvez pas effacer votre adresse courriel.
NICK_UNSET_EMAIL_OTHER_BAD
	L'adresse courriel du surnom ne peut être effacée.

# ACCESS responses
NICK_ACCESS_SYNTAX
	ACCESS {ADD | DEL | LIST} [masque]
NICK_ACCESS_ALREADY_PRESENT
	Le masque %s est déjà présent dans votre liste d'accès.
NICK_ACCESS_REACHED_LIMIT
	Désolé, vous pouvez avoir seulement %d entrées d'accès pour un surnom.
NICK_ACCESS_NO_NICKS
	Attention: Les masques d'accès des surnoms ne peuvent pas inclure de surnoms.  >S.V.P. vérifiez et assurez-vous que vous avez entré le masque usager@hôte correctement.
NICK_ACCESS_ADDED
	%s a été ajouté à votre liste d'accès.
NICK_ACCESS_NOT_FOUND
	%s n'apparait pas à la liste de vos accès.
NICK_ACCESS_DELETED
	%s effacé de votre liste d'accès.
NICK_ACCESS_LIST_EMPTY
	Votre liste d'accès est vide.
NICK_ACCESS_LIST
	Liste d'accès:
NICK_ACCESS_LIST_X_EMPTY
	Liste d'accès pour %s est vide.
NICK_ACCESS_LIST_X
	Liste d'accès pour %s:

# LINK responses
NICK_OLD_LINK_SYNTAX
	LINK surnom mot de passe
NICK_LINK_SYNTAX
	LINK surnom
NICK_LINK_DISABLED
	Désolé, l'option de liaison de surnom est temporairement hors-service.
NICK_LINK_FAILED
	Désolé, le lien n'a pu être établi.
NICK_CANNOT_BE_LINKED
	Le surnom %s ne peut pas être lié.
NICK_OLD_LINK_SAME
	Vous ne pouvez pas vous lier à votre surnom actuel!
NICK_LINK_SAME
	Vous ne pouvez lier votre surnom actuel!
NICK_LINK_ALREADY_LINKED
	Le surnom %s est présentement lié à votre surnom.
NICK_LINK_IN_USE
	Le surnom %s est présentement utilisé.
NICK_LINK_TOO_MANY
	Vous ne pouvez pas avoir plus de %d surnoms liés ensembles.
NICK_LINK_TOO_MANY_NICKS
	Vous avez présentement %d surnoms enregistrés (maximum de %d).  Le lien a échoué.
NICK_OLD_LINK_TOO_MANY_CHANNELS
	Ne peut être lié au surnom %s: trop de canaux enregistrés (combinaison maximale de %d).
NICK_OLD_LINKED
	Votre surnom a été lié à %s.
NICK_LINKED
	Le surnom %s a été lié à votre surnom.

# UNLINK responses
NICK_OLD_UNLINK_SYNTAX
	UNLINK [surnom mot de passe]
NICK_UNLINK_SYNTAX
	UNLINK surnom
NICK_UNLINK_OPER_SYNTAX
	UNLINK surnom [FORCE]
NICK_UNLINK_DISABLED
	Désolé, l'option de retrait de liaison de surnom est temporairement hors-service.
NICK_UNLINK_FAILED
	Désolé, le retrait du lien échoué.
NICK_UNLINK_SAME
	Vous ne pouvez enlever le lien de votre surnom actuel!
NICK_OLD_UNLINK_NOT_LINKED
	Votre surnom n'est pas lié.
NICK_UNLINK_NOT_LINKED_YOURS
	Le surnom %s n'est pas lié à votre surnom.
NICK_UNLINK_NOT_LINKED
	Le surnom %s n'est pas lié.
NICK_OLD_UNLINKED
	Votre surnom n'est plus lié à %s.
NICK_UNLINKED
	Le surnom %s n'est plus lié à votre surnom actuel.
NICK_X_UNLINKED
	Le surnom %s n'est plus lié à %s.

# LISTLINKS responses
NICK_OLD_LISTLINKS_SYNTAX
	LISTLINKS surnom [ALL]
NICK_LISTLINKS_SYNTAX
	LISTLINKS
NICK_LISTLINKS_HEADER
	Liste des surnoms liés à %s:
NICK_LISTLINKS_FOOTER
	%d surnoms liés.

# INFO responses
NICK_INFO_SYNTAX
	INFO surnom [ALL]
NICK_INFO_REALNAME
	   %s est %s
NICK_INFO_ADDRESS
	 Dernière adresse apperçue: %s
NICK_INFO_ADDRESS_ONLINE
	       Branché à partir de: %s
NICK_INFO_ADDRESS_ONLINE_NOHOST
	%s est présentement en ligne.
### CHANGED: NICK_INFO_ADDRESS_OTHER_NICK added
NICK_INFO_TIME_REGGED
	      Enregistré depuis le: %s
NICK_INFO_LAST_SEEN
	Apperçu la dernière fois à: %s
NICK_INFO_LAST_QUIT
	 Dernier message de sortie: %s
NICK_INFO_URL
	                       URL: %s
NICK_INFO_EMAIL
	          Adresse courriel: %s
NICK_INFO_EMAIL_UNAUTHED
	          Adresse courriel: %s (non vérifiée)
NICK_INFO_INFO
	               Information: %s
NICK_INFO_OPTIONS
	                   Options: %s
# These strings MUST NOT be empty
NICK_INFO_OPT_KILL
	Protection Kill
NICK_INFO_OPT_SECURE
	Securité active
NICK_INFO_OPT_PRIVATE
	Privé
NICK_INFO_OPT_NONE
	Aucune
NICK_INFO_NO_EXPIRE
	Ce surnom n'expirera pas.
NICK_INFO_SUSPEND_DETAILS
	Suspendu par %s sur %s (%s).
NICK_INFO_SUSPEND_REASON
	Raison: %s
NICK_INFO_SHOW_ALL
	 Pour plus d'information, tapez: /msg %s INFO %s ALL

# LISTCHANS responses
NICK_LISTCHANS_SYNTAX
	LISTCHANS
NICK_LISTCHANS_NONE
	%s n'a pas enregistré aucun canal.
NICK_LISTCHANS_HEADER
	Canaux enregistrés par %s:
NICK_LISTCHANS_END
	Fin de la liste (%d canaux enregistrés).

# LIST/LISTEMAIL responses
#NICK_LIST_SYNTAX
#	%s modèle
#NICK_LIST_OPER_SYNTAX
#	%s modèle [FORBIDDEN] [NOEXPIRE] [SUSPENDED]
#NICK_LIST_OPER_SYNTAX_AUTH
#	%s modèle [FORBIDDEN] [NOEXPIRE] [SUSPENDED] [NOAUTH]
NICK_LIST_HEADER
	Liste des entrées correspondantes à %s:
NICK_LISTEMAIL_NONE
	(non sélectionné)

# RECOVER responses
NICK_RECOVER_SYNTAX
	RECOVER surnom [mot de passe]
NICK_NO_RECOVER_SELF
	Vous ne pouvez vous reprendre vous-même!
NICK_RECOVERED
	L'usager qui avait pris votre surnom a été déconnecté.  Tapez /msg %s RELEASE %s mot de passe pour le reprendre avant l'expiration du délais d'une minute.

# RELEASE responses
NICK_RELEASE_SYNTAX
	RELEASE surnom [mot de passe]
NICK_RELEASE_NOT_HELD
	Le surnom %s n'est pas retenu.
NICK_RELEASED
	Les services ont libéré votre surnom.

# GHOST responses
NICK_GHOST_SYNTAX
	GHOST surnom [mot de passe]
NICK_NO_GHOST_SELF
	Vous ne pouvez utiliser ghost pour vous-même!
NICK_GHOST_KILLED
	Votre fantôme a été tué grâce à la commande Ghost.

# GETPASS responses
NICK_GETPASS_SYNTAX
	GETPASS surnom
### CHANGED: nickname parameter inserted
#NICK_GETPASS_UNAVAILABLE
#	La commande GETPASS n'es pas disponible parce que l'encryption est en cours.
NICK_GETPASS_PASSWORD_IS
	Le mot de passe pour %s est %s.

# FORBID responses
NICK_FORBID_SYNTAX
	FORBID surnom
NICK_FORBID_SUCCEEDED
	Le surnom %s est maintenant interdit.
NICK_FORBID_FAILED
	Ne peut interdire le nick %s!

# SUSPEND responses
NICK_SUSPEND_SYNTAX
	SUSPEND [+expiration] surnom raison
NICK_SUSPEND_SUCCEEDED
	Le surnom %s, et tous les surnoms qui y étaient liés, ont été suspendus.
NICK_SUSPEND_ALREADY_SUSPENDED
	Le surnom %s est présentement suspendu.

# UNSUSPEND responses
NICK_UNSUSPEND_SYNTAX
	UNSUSPEND surnom
NICK_UNSUSPEND_SUCCEEDED
	Le surnom %s a été dé-suspendu.
NICK_UNSUSPEND_NOT_SUSPENDED
	 Le surnom %s n'est pas suspendu.

################ autojoin module messages/responses

# AJOIN responses
NICK_AJOIN_SYNTAX
	AJOIN {ADD | DEL | LIST} [canal]
NICK_AJOIN_ADD_SYNTAX
	AJOIN ADD canal
NICK_AJOIN_DEL_SYNTAX
	AJOIN DEL canal
NICK_AJOIN_ALREADY_PRESENT
	Le canal %s est actuellement sur votre liste d'autojoin.
NICK_AJOIN_LIST_FULL
	Désolé, vous pouvez avoir seulement %d entrées dans la liste autojoin pour un surnom.
NICK_AJOIN_ADDED
	%s ajouté à votre liste autojoin.
NICK_AJOIN_NOT_FOUND
	%s abscent de votre liste autojoin.
NICK_AJOIN_DELETED
	%s effacé de votre liste autojoin.
NICK_AJOIN_LIST_EMPTY
	Votre liste d'autojoin est vide.
NICK_AJOIN_LIST
	Liste d'autojoin:
NICK_AJOIN_LIST_X_EMPTY
	Liste d'autojoin pour %s est vide.
NICK_AJOIN_LIST_X
	Liste d'autojoin pour %s:

################ mail-auth module messages/responses

# General-purpose messages
NICK_AUTH_FOR_REGISTER
	Lorsque vous recevrez ce message, tapez /msg %s AUTH code (remplacez code par le code d'autorisation inclus au message) pour compléter votre enregistrement de surnom.
NICK_AUTH_FOR_SET_EMAIL
	Lorsque vous recevrez ce message, tapez /msg %s AUTH code (remplacez code par le code d'autorisation inclus au message) pour compléter votre changement d'adresse.
	S.V.P. notez que vous ne pourrez plus continuer d'utiliser votre surnom tant que vous n'aurez pas complété le changement.
NICK_AUTH_NOT_NEEDED
	Il n'y a pour le moment rien qui requiert une autorisation.
NICK_AUTH_NO_AUTHCODE
	 Le surnom %s n'a pas de code d'autorisation.
NICK_AUTH_HAS_AUTHCODE
	Le surnom %s a déjà un code d'autorisation.
PASSWORD_WARNING_FOR_AUTH
	Attention: Après un autre code d'autorisation incorrect vous serez débranché.

# Mail text.  The last "%s" (before the user@host) in the body text is
# replaced by one of the NICK_AUTH_MAIL_TEXT_* messages.
NICK_AUTH_MAIL_SUBJECT
	Code d'autorisation pour %s
NICK_AUTH_MAIL_BODY
	Le code d'autorisation pour votre surnom (%s) est : %09d
	S.V.P. soumettez ce code à %s avec la commande:
	    /msg %s AUTH %09d
	
	Si vous avec quelque question que ce soit, s.v.p. joindre #aide pour de l'assistance.
	
	Ce message sera expédié par %s en réponse à %s
	%s@%s.
NICK_AUTH_MAIL_TEXT_REG
	enregistrement par
NICK_AUTH_MAIL_TEXT_EMAIL
	adresse courriel changée par
NICK_AUTH_MAIL_TEXT_SENDAUTH
	une commande SENDAUTH de
NICK_AUTH_MAIL_BODY_SETAUTH
	Le code d'autorisation pour votre surnom (%s) est: %09d
	S.V.P. soumettez ce code à %s avec la commande:
	    /msg %s AUTH %09d

# AUTH responses
NICK_AUTH_SYNTAX
	AUTH code
NICK_AUTH_DISABLED
	Désolé, l'autorisation de surnom est temporairement hors-service.
NICK_AUTH_FAILED
	Le code d'autorisation est incorrect.
NICK_AUTH_SUCCEEDED_REGISTER
	Autorisation réussie; votre enregistrement de surnom est maintenant complétée.
NICK_AUTH_SUCCEEDED_SET_EMAIL
	Autorisation réussie; votre changement d'adresse courriel est maintenant complétée.
NICK_AUTH_SUCCEEDED_SETAUTH
	Autorisation réussie; vous pouvez maintenant utiliser votre surnom.

# SENDAUTH responses
NICK_SENDAUTH_SYNTAX
	SENDAUTH
NICK_SENDAUTH_TOO_SOON
	S.V.P. attendez %s avant d'utiliser la commande SENDAUTH à nouveau.

# REAUTH responses
NICK_REAUTH_SYNTAX
	REAUTH
### CHANGED: REAUTH, RESTOREMAIL messages added

# SETAUTH responses
NICK_SETAUTH_SYNTAX
	SETAUTH surnom
NICK_SETAUTH_NO_EMAIL
	Le surnom %s n'a aucune adresse courriel de sélectionnée.
NICK_SETAUTH_AUTHCODE_SET
	Un nouveau code d'autorisation (%09d) a été sélectionné pour le surnom %s.
NICK_SETAUTH_SEND_NORESOURCES
	Attention: incapable d'expédier du courrier à %s (ressources insuffisantes).
#NICK_SETAUTH_USER_NOTICE
#	Vous devez autoriser votre surnom avant de continuer de l'utiliser.  Un code d'autorisation  vous a été expédié à l'adresse courriel: %s
#	Tapez /msg %s HELP AUTH pour plus d'information.

# GETAUTH responses
NICK_GETAUTH_SYNTAX
	GETAUTH surnom
NICK_GETAUTH_AUTHCODE_IS
	Le code d'autorisation pour le surnom %s est: %09d

# CLEARAUTH responses
NICK_CLEARAUTH_SYNTAX
	CLEARAUTH surnom
NICK_CLEARAUTH_CLEARED
	Le code d'autorisation pour le surnom %s a été effacé.

###########################################################################
#
# ChanServ messages
#
###########################################################################

# Access level descriptions
CHAN_LEVEL_AUTOOP
	Status d'opérateur de canal automatique
CHAN_LEVEL_AUTOVOICE
	Mode +v automatique
CHAN_LEVEL_INVITE
	Autorisé à utiliser la commande INVITE
CHAN_LEVEL_AKICK
	Autorisé à utiliser la commande AKICK
CHAN_LEVEL_SET
	Autorisé à utiliser la commande SET (exception faite de FOUNDER/PASSWORD)
CHAN_LEVEL_CLEAR
	Autorisé à utiliser la commande CLEAR
CHAN_LEVEL_UNBAN
	Autorisé à utiliser la commande UNBAN
CHAN_LEVEL_OPDEOP
	Autorisé à utiliser les commandes OP/DEOP
CHAN_LEVEL_ACCESS_LIST
	Autorisé à visualiser la liste des accès
CHAN_LEVEL_ACCESS_CHANGE
	Autorisé à modifier la liste d'accès
#CHAN_LEVEL_MEMO
#	Autorisé à lister et lire les messages du canal
CHAN_LEVEL_VOICE
	Autorisé à utiliser les commandes VOICE/DEVOICE
CHAN_LEVEL_AUTOHALFOP
	Mode +h automatique
CHAN_LEVEL_HALFOP
	Autorisé à utiliser les commandes HALFOP/DEHALFOP
CHAN_LEVEL_AUTOPROTECT
	Status automatique de protection de canal (+a)
CHAN_LEVEL_PROTECT
	Autorisé à utiliser les commandes PROTECT/DEPROTECT
CHAN_LEVEL_KICK
	Autorisé à utiliser la commande d'expulsion KICK
CHAN_LEVEL_STATUS
	Autorisé à utiliser la commande STATUS
CHAN_LEVEL_TOPIC
	Autorisé à utiliser la commande d'invitation INVITE

# Automatic responses
CHAN_IS_REGISTERED
	Ce canal a été enregistré avec %s.
CHAN_MAY_NOT_BE_USED
	Ce canal ne peut être utilisé.
CHAN_NOT_ALLOWED_TO_JOIN
	Vous n'êtes pas autorisé à vous trouver sur ce canal.

# Multi-purpose responses
CHAN_MUST_IDENTIFY_NICK
	S.V.P. identifiez vous d'abord avec %s, par la commande:
	/msg %s IDENTIFY mot de passe
CHAN_BOUNCY_MODES
	Désolé, la commande %s est temporairement hors d'usage.

# REGISTER responses
CHAN_REGISTER_SYNTAX
	REGISTER canal mot de passe description
CHAN_REGISTER_DISABLED
	Désolé, l'enregistrement de canal est temporairement désactivée.
CHAN_REGISTER_SHORT_CHANNEL
	Le canal # ne peut pas être enregistré.
CHAN_REGISTER_NOT_LOCAL
	Les canaux locaux ne peuvent être enregistrés.
CHAN_REGISTER_INVALID_NAME
	Vous pouvez enregistrer seulement les canaux dont les noms commençant par #.
CHAN_MUST_REGISTER_NICK
	Vous devez enregistrer votre surnom d'abord.  Tapez /msg %s HELP pour plus d'information sur l'enregistrement de surnoms.
CHAN_MAY_NOT_BE_REGISTERED
	Le canal %s ne peut pas être enregistré.
CHAN_ALREADY_REGISTERED
	Le canal %s est déjà enregistré!
CHAN_MUST_BE_CHANOP
	Vous devez être opérateur sur le canal afin de l'enregistrer.
CHAN_REACHED_CHANNEL_LIMIT
	Désolé, vous avez déjà atteint votre limite de %d canaux.
CHAN_EXCEEDED_CHANNEL_LIMIT
	Désolé, vous avez dépassé votre limite de %d canaux.
CHAN_REGISTRATION_FAILED
	Désolé, échec d'enregistrement.
CHAN_REGISTERED
	Le canal %s enregistré sous votre surnom: %s
CHAN_PASSWORD_IS
	Votre mot de passe de canal est %s -- retenez-le pour utilisation ultérieur.

# IDENTIFY responses
CHAN_IDENTIFY_SYNTAX
	IDENTIFY canal mot de passe
CHAN_IDENTIFY_FAILED
	Désolé, échec d'identification.
CHAN_IDENTIFY_SUCCEEDED
	Mot de passe accepté -- vous avez maintenant accès à %s au niveau de propriétaire.

# DROP responses
CHAN_DROP_SYNTAX
	DROP canal mot de passe
CHAN_DROP_DISABLED
	Désolé, le désenregistrement de canal est temporairement désactivé.
CHAN_DROPPED
	Le canal %s est désenregistré.

# DROPCHAN responses
CHAN_DROPCHAN_SYNTAX
	DROPCHAN canal

# SET responses
CHAN_SET_SYNTAX
	SET canal option paramêtres
CHAN_SET_DISABLED
	Désolé, la sélection d'option de canal est temporairement désactivée.
CHAN_SET_UNKNOWN_OPTION
	Option sélectionnée inconnue %s.

# SET FOUNDER responses
CHAN_SET_FOUNDER_TOO_MANY_CHANS
	%s a trop de canaux enregistrés.
CHAN_FOUNDER_CHANGED
	Le propriétaire de %s a été changé pour %s.

# SET SUCCESSOR responses
CHAN_SUCCESSOR_CHANGED
	Le successeur de %s a été changé pour %s.
CHAN_SUCCESSOR_IS_FOUNDER
	Vous ne pouvez pas assigner le propriétaire du canal à titre de successeur.

# SET PASSWORD responses
CHAN_SET_PASSWORD_FAILED
	Désolé, le mot de passe ne peut être sélectionné.
CHAN_PASSWORD_CHANGED
	Mot de passe de %s changé.
CHAN_PASSWORD_CHANGED_TO
	Le mot de passe de %s a été changé pour %s.

# SET DESC responses
CHAN_DESC_CHANGED
	Mot de passe de %s changé pour %s.

# SET URL responses
CHAN_URL_CHANGED
	L'URL pour %s changé pour %s.

# SET EMAIL responses
CHAN_EMAIL_CHANGED
	L'adresse courriel pour %s est changée pour %s.

# SET ENTRYMSG responses
CHAN_ENTRY_MSG_CHANGED
	Le message d'acceuil pour %s est changé.

# SET MLOCK responses
CHAN_SET_MLOCK_NEED_PLUS_MINUS
	La liste des modes doit commencer par un + (plus) ou un - (moins).
CHAN_SET_MLOCK_NEED_PARAM
	Paramêtre +%c requis pour le mode vérouillé (MLOCK).
CHAN_SET_MLOCK_NEED_POSITIVE
	Le paramêtre pour le mode de vérouillage MLOCK +%c doit être un nombre positif.
CHAN_SET_MLOCK_MODE_REG_BAD
	Le mode +%c est toujours présent sur un canal enregistré, et ne peut pas être utilisé avec MLOCK.
CHAN_SET_MLOCK_LINK_BAD
	Le paramêtre +%c pour MLOCK n'est pas un nom de canal valide.
CHAN_SET_MLOCK_LINK_SAME
	Le paramêtre +%c pour MLOCK doit être un canal différent.
CHAN_SET_MLOCK_REQUIRES
	Le mode MLOCK +%c requiert que MLOCK +%c soit sélectionné comme il faut.
CHAN_SET_MLOCK_BAD_PARAM
	Paramêtre invalide pour MLOCK +%c.
CHAN_SET_MLOCK_UNKNOWN_CHAR
	Caractère de mode inconnu %c ignoré.
CHAN_SET_MLOCK_CANNOT_LOCK
	Le mode %c ne peut pas être vérouillé.
CHAN_MLOCK_CHANGED
	Le mode vérouillé sur le canal %s est changé pour %s.
CHAN_MLOCK_REMOVED
	Mode vérouillé sur le canal %s retiré.

# SET HIDE responses
CHAN_SET_HIDE_SYNTAX
	SET canal HIDE {EMAIL | TOPIC | MLOCK} {ON | OFF}
CHAN_SET_HIDE_EMAIL_ON
	L'adresse courriel du canal %s sera maintenant masquée lors de l'affichage %s INFO.
CHAN_SET_HIDE_EMAIL_OFF
	L'adresse courriel du canal %s sera maintenant présentée lors de l'affichage %s INFO.
CHAN_SET_HIDE_TOPIC_ON
	Le sujet du canal %s sera maintenant masquée lors de l'affichage %s INFO.
CHAN_SET_HIDE_TOPIC_OFF
	Le sujet du canal %s sera maintenant présenté lors de l'affichage %s INFO.
CHAN_SET_HIDE_MLOCK_ON
	Le mode vérouillé du canal %s sera maintenant masqué lors de l'affichage %s INFO.
CHAN_SET_HIDE_MLOCK_OFF
	Le mode vérouillé du canal %s sera maintenant présenté lors de l'affichage %s INFO.

# SET KEEPTOPIC responses
CHAN_SET_KEEPTOPIC_SYNTAX
	SET canal KEEPTOPIC {ON | OFF}
CHAN_SET_KEEPTOPIC_ON
	L'option de retenue de sujet est maintenant ON.
CHAN_SET_KEEPTOPIC_OFF
	L'option de retenue de sujet est maintenant ON.

# SET TOPICLOCK responses
CHAN_SET_TOPICLOCK_SYNTAX
	SET canal TOPICLOCK {ON | OFF}
CHAN_SET_TOPICLOCK_ON
	L'option de rétention de sujet est maintenant ON.
CHAN_SET_TOPICLOCK_OFF
	Option de rétention de sujet maintenant à OFF.

# SET PRIVATE responses
CHAN_SET_PRIVATE_SYNTAX
	SET canal PRIVATE {ON | OFF}
CHAN_SET_PRIVATE_ON
	Option privée maintenant ON.
CHAN_SET_PRIVATE_OFF
	Option privée maintenant OFF.

# SET SECUREOPS responses
CHAN_SET_SECUREOPS_SYNTAX
	SET canal SECUREOPS {ON | OFF}
CHAN_SET_SECUREOPS_ON
	L'option d'opérateurs sécurisée est maintenant ON.
CHAN_SET_SECUREOPS_OFF
	L'option d'opérateurs sécurisée est maintenant OFF.

# SET LEAVEOPS responses
CHAN_SET_LEAVEOPS_SYNTAX
	SET canal LEAVEOPS {ON | OFF}
CHAN_SET_LEAVEOPS_ON
	L'option de départ des opérateurs est maintenant ON.
CHAN_SET_LEAVEOPS_OFF
	L'option de départ des opérateurs est maintenant OFF.

# SET RESTRICTED responses
CHAN_SET_RESTRICTED_SYNTAX
	SET canal RESTRICTED {ON | OFF}
CHAN_SET_RESTRICTED_ON
	L'option de restriction d'accès est maintenant ON.
CHAN_SET_RESTRICTED_OFF
	L'option de restriction d'accès est maintenant OFF.

# SET SECURE responses
CHAN_SET_SECURE_SYNTAX
	SET canal SECURE {ON | OFF}
CHAN_SET_SECURE_ON
	L'option de sécurité est maintenant ON.
CHAN_SET_SECURE_OFF
	L'option de sécurité est maintenant OFF.

# SET OPNOTICE responses
CHAN_SET_OPNOTICE_SYNTAX
	SET canal OPNOTICE {ON | OFF}
CHAN_SET_OPNOTICE_ON
	L'option Notice-Opérateur est maintenant ON.
CHAN_SET_OPNOTICE_OFF
	L'option Notice-Opérateur est maintenant OFF.

# SET ENFORCE responses
CHAN_SET_ENFORCE_SYNTAX
	SET canal ENFORCE {ON | OFF}
CHAN_SET_ENFORCE_ON
	L'option de régulation (ENFORCE) est maintenant ON.
CHAN_SET_ENFORCE_OFF
	L'option de régulation (ENFORCE) est maintenant OFF.

# SET MEMO-RESTRICTED responses
CHAN_SET_MEMO_RESTRICTED_SYNTAX
	SET canal MEMO-RESTRICTED {ON | OFF}

# SET NOEXPIRE responses
CHAN_SET_NOEXPIRE_SYNTAX
	SET canal NOEXPIRE {ON | OFF}
CHAN_SET_NOEXPIRE_ON
	Le canal %s n'expirera pas.
CHAN_SET_NOEXPIRE_OFF
	Le canal %s va expirer.

# UNSET responses
CHAN_UNSET_SYNTAX
	UNSET canal {SUCCESSOR | URL | EMAIL | ENTRYMSG}
CHAN_SUCCESSOR_UNSET
	Le successeur pour %s n'est pas déterminé.
CHAN_URL_UNSET
	L'URL pour %s n'est pas déterminé.
CHAN_EMAIL_UNSET
	L'adresse courriel pour %s n'est pas déterminée.
CHAN_ENTRY_MSG_UNSET
	Le message d'acceuil pour %s n'est pas déterminé.

# ACCESS messages
### CHANGED: syntax changed, CHAN_ACCESS_*_SYNTAX added
#CHAN_ACCESS_SYNTAX
#	ACCESS canal {ADD | DEL | LIST | COUNT} [surnom [niveau]]
CHAN_ACCESS_DISABLED
	Désolé, la modification de liste d'accès est temporairement désactivée.
CHAN_ACCESS_LEVEL_NONZERO
	Le niveau de l'accès doit être différent de zéro.
CHAN_ACCESS_LEVEL_RANGE
	Le niveau de l'accès doit être compris entre %d et %d inclusivement.
CHAN_ACCESS_NICKS_ONLY
	La liste d'accès du canal ne peut contenir exclisivement que des surnoms enregistrés.
CHAN_ACCESS_REACHED_LIMIT
	Désolé, vous pouvez avoir seulement %d entrées d'accès sur un canal.
CHAN_ACCESS_LEVEL_UNCHANGED
	Le niveau d'accès pour %s sur %s non-modifié de %d.
CHAN_ACCESS_LEVEL_CHANGED
	Le niveau d'accès pour %s sur %s changé à %d.
CHAN_ACCESS_ADDED
	%s ajouté à la liste d'accès de %s à un niveau de %d.
CHAN_ACCESS_NOT_FOUND
	%s n'a pas été trouvé dans la liste d'accès de %s.
CHAN_ACCESS_DELETED
	%s suprimé de la liste d'accès de %s.
CHAN_ACCESS_LIST_EMPTY
	La liste d'accès de %s est vide.
CHAN_ACCESS_NO_MATCH
	Aucune entrée correspondante dans la liste d'accès de %s.
### CHANGED: entry number field removed
#CHAN_ACCESS_LIST_HEADER
#	Liste d'accès pour %s:
#	  Num   Niv  Surnom
#CHAN_ACCESS_LIST_FORMAT
#	 %4d  %4d  %s
CHAN_ACCESS_COUNT
	La liste d'accès de %s a %d entrées.
	
# XOP (SOP/AOP/etc.) responses
### CHANGED: syntax changed, CHAN_{S,A,H,V,N}OP_LIST_SYNTAX added
#CHAN_SOP_SYNTAX
#	SOP canal {ADD | DEL | LIST | COUNT} [surnom]
#CHAN_AOP_SYNTAX
#	AOP canal {ADD | DEL | LIST | COUNT} [surnom]
#CHAN_HOP_SYNTAX
#	HOP canal {ADD | DEL | LIST | COUNT} [surnom]
#CHAN_VOP_SYNTAX
#	VOP canal {ADD | DEL | LIST | COUNT} [surnom]
#CHAN_NOP_SYNTAX
#	NOP canal {ADD | DEL | LIST | COUNT} [surnom]
CHAN_XOP_ADDED
	%s ajouté à la liste de %s %s.
CHAN_XOP_LEVEL_CHANGED
	%s déplacé vers la liste %s %s.
CHAN_XOP_LEVEL_UNCHANGED
	%s est déjà sur la liste %s %s.
CHAN_XOP_NICKS_ONLY
	Les listes SOP, AOP, VOP, et NOP doivent contenir exclusivement des surnoms enregistrés.
CHAN_XOP_NICKS_ONLY_HOP
	Les listes SOP, AOP, HOP, VOP, et NOP doivent contenir exclusivement des surnoms enregistrés.
CHAN_XOP_REACHED_LIMIT
	Désolé, le nombre d'entrées sur vos listes SOP, AOP, VOP, et NOP ne peuvent pas totaliser plus de %d entrées.  Limite dépassée.
CHAN_XOP_REACHED_LIMIT_HOP
	Désolé, le nombre d'entrées sur vos listes SOP, AOP, HOP, VOP, et NOP ne peuvent pas totaliser plus de %d entrées.  Limite dépassée.
CHAN_XOP_NOT_FOUND
	%s non découvert sur la liste %s %s.
CHAN_XOP_DELETED
	%s suprimée de la liste %s %s.
CHAN_XOP_LIST_EMPTY
	La liste %s %s est vide.
CHAN_XOP_NO_MATCH
	Aucune entrée correspondante sur la liste %s %s.
CHAN_XOP_LIST_HEADER
	Liste %s pour %s:
CHAN_XOP_COUNT
	La liste %s %s a %d entrées.
	
# AKICK responses
### CHANGED: syntax changed, CHAN_AKICK_{LIST,VIEW}_SYNTAX added
#CHAN_AKICK_SYNTAX
#	AKICK canal {ADD | DEL | LIST | VIEW | COUNT | ENFORCE} [masque]
CHAN_AKICK_DISABLED
	Désolé, la modification de liste d'autokick est temporairement impossible.
CHAN_AKICK_ALREADY_EXISTS
	%s existe déjà sur la liste d'autokick de %s.
CHAN_AKICK_REACHED_LIMIT
	Désolé, vous pouvez avoir seulement %d entrées sur la liste de masques autokick du canal.
CHAN_AKICK_ADDED
	%s ajouté à la liste d'autokick de %s.
CHAN_AKICK_NOT_FOUND
	%s introuvé dans la liste d'autokick de %s.
CHAN_AKICK_DELETED
	%s suprimé de la liste autokick de %s.
CHAN_AKICK_LIST_EMPTY
	La liste d'autokick de %s est vide.
CHAN_AKICK_NO_MATCH
	Aucune entrée correspondante dans la liste d'autokick de %s.
CHAN_AKICK_LIST_HEADER
	Liste d'autokick pour %s:
# mask, set-by, set-time, last-used, reason
CHAN_AKICK_VIEW_FORMAT
	    %s (par %s sur %s; dernière utilisation %s)
	        %s
# mask, set-by, set-time, reason
CHAN_AKICK_VIEW_UNUSED_FORMAT
	    %s (par %s sur %s; jamais utilisé)
	        %s
CHAN_AKICK_COUNT
	%s a %d entrées sur sa liste d'AKICK.
CHAN_AKICK_ENFORCE_DONE
	AKICK ENFORCE pour %s complété; %d usagers ont été affectés.

# LEVELS responses
CHAN_LEVELS_SYNTAX
	LEVELS canal {SET | DIS[ABLE] | LIST | RESET} [item [niveau]]
CHAN_LEVELS_RANGE
	Le niveau doit être entre %d et %d inclusivement.
CHAN_LEVELS_CHANGED
	Le niveau pour %s sur le canal %s changé à %d.
CHAN_LEVELS_UNKNOWN
	Ajustement %s inconnu.  Tapez /msg %s HELP LEVELS DESC pour une liste des ajustements valides.
CHAN_LEVELS_DISABLED
	%s désactivé sur le canal %s.
#CHAN_LEVELS_LIST_HEADER
#	Ajustement des niveaux d'accès pour le canal %s:
CHAN_LEVELS_LIST_DISABLED
	    %-*s  (disactivé)
CHAN_LEVELS_LIST_FOUNDER
	    %-*s  (propriétaire seulement)
CHAN_LEVELS_LIST_NORMAL
	    %-*s  %4d
#CHAN_LEVELS_RESET
#	Niveau d'accès pour %s rétablit à la valeur par défaut.

# INFO responses
CHAN_INFO_SYNTAX
	INFO canal [ALL]
CHAN_INFO_HEADER
	Information pour le canal %s:
CHAN_INFO_FOUNDER
	        Propriétaire: %s
CHAN_INFO_SUCCESSOR
	          Successeur: %s
CHAN_INFO_DESCRIPTION
	         Description: %s
CHAN_INFO_ENTRYMSG
	    Message d'entrée: %s
CHAN_INFO_TIME_REGGED
	          Enregistré: %s
CHAN_INFO_LAST_USED
	Dernière utilisation: %s
CHAN_INFO_LAST_TOPIC
	       Dernier sujet: %s
CHAN_INFO_TOPIC_SET_BY
	 Sujet déterminé par: %s
CHAN_INFO_URL
	                 URL: %s
CHAN_INFO_EMAIL
	    Adresse courriel: %s
CHAN_INFO_OPTIONS
	             Options: %s
CHAN_INFO_OPT_PRIVATE
	Privé
CHAN_INFO_OPT_KEEPTOPIC
	Rétention de sujet
CHAN_INFO_OPT_TOPICLOCK
	Sujet bloqué
CHAN_INFO_OPT_SECUREOPS
	Opérateurs avec accès seulement
CHAN_INFO_OPT_LEAVEOPS
	Départ des opérateurs
CHAN_INFO_OPT_RESTRICTED
	Accès restreint
CHAN_INFO_OPT_SECURE
	Sécurité
CHAN_INFO_OPT_OPNOTICE
	Notice d'opérateur
CHAN_INFO_OPT_ENFORCE
	Régulation (ENFORCE)
CHAN_INFO_OPT_NONE
	Aucun
CHAN_INFO_MODE_LOCK
	      Mode(s) barré(s): %s
CHAN_INFO_NO_EXPIRE
	Ce canal n'aura pas d'expiration.
CHAN_INFO_SUSPEND_DETAILS
	Suspendu par %s le %s (%s).
CHAN_INFO_SUSPEND_REASON
	Raison: %s
CHAN_INFO_SHOW_ALL
	Pour plus d'information, tapez: /msg %s INFO %s ALL

# LIST responses
#CHAN_LIST_SYNTAX
#	LIST modèle
#CHAN_LIST_OPER_SYNTAX
#	LIST modèle [FORBIDDEN] [NOEXPIRE] [SUSPENDED]
CHAN_LIST_HEADER
	Liste des entrées correspondantes au modèle %s:

# INVITE responses
CHAN_INVITE_SYNTAX
	INVITE canal
CHAN_INVITE_OK
	Invitation de %s sur le canal %s.

# OP/DEOP/VOICE/DEVOICE responses
CHAN_OPVOICE_SYNTAX
	%s canal [surnom]
CHAN_OP_SUCCEEDED
	%s a obtenu le status d'opérateur sur le canal %s.
CHAN_OP_ALREADY
	%s est actuellement opérateur sur le canal %s.
CHAN_OP_FAILED
	Incapable d'accorder le status d'opératuer à %s sur le canal %s.
CHAN_DEOP_SUCCEEDED
	Status d'opérateur de %s retiré sur le canal %s.
CHAN_DEOP_ALREADY
	%s n'a déjà plus de status d'opérateur sur le canal %s.
CHAN_DEOP_FAILED
	Incapable de retirer le status d'opérateur de %s sur le canal %s.
CHAN_VOICE_SUCCEEDED
	%s a obtenu les privilèges vocaux (mode +v) sur le canal %s.
CHAN_VOICE_ALREADY
	%s a déjà les privilèges vocaux (mode +v) sur le canal %s.
CHAN_VOICE_FAILED
	Incapable d'accorder le status vocal à %s sur le canal %s.
CHAN_DEVOICE_SUCCEEDED
	%s n'a plus les privilèges vocaux (mode +v) sur le canal %s.
CHAN_DEVOICE_ALREADY
	%s a déjà les privilèges vocaux (mode +v) sur le canal %s.
CHAN_DEVOICE_FAILED
	Incapable de retirer le status vocal de %s sur le canal %s.
CHAN_HALFOP_SUCCEEDED
	%s a obtenu les privilèges de demi-opérateur sur le canal %s.
CHAN_HALFOP_ALREADY
	%s détient déjà les privilèges de demi-opérateur sur le canal %s.
CHAN_HALFOP_FAILED
	Incapable d'accorder les privilèges de demi-opérateur à %s sur le canal %s.
CHAN_DEHALFOP_SUCCEEDED
	Status de demi-opérateur de %s retiré sur le canal %s.
CHAN_DEHALFOP_ALREADY
	%s n'a déjà plus de status de demi-opérateur sur le canal %s.
CHAN_DEHALFOP_FAILED
	Incapable de retirer les privilèges de demi-opérateur de %s sur le canal %s.
CHAN_PROTECT_SUCCEEDED
	%s protégé sur le canal %s.
CHAN_PROTECT_ALREADY
	%s est actuellement protégé sur le canal %s.
CHAN_PROTECT_FAILED
	Incapable de protéger %s sur le canal %s.
CHAN_DEPROTECT_SUCCEEDED
	Retrait de protection de %s sur le canal %s.
CHAN_DEPROTECT_ALREADY
	%s est actuellement sans protection sur le canal %s.
CHAN_DEPROTECT_FAILED
	Incapable de retirer la protection de %s sur le canal %s.

# UNBAN responses
CHAN_UNBAN_SYNTAX
	UNBAN canal
CHAN_UNBANNED
	Vous avez été débanni de %s.

# KICK responses
CHAN_KICK_SYNTAX
	KICK canal surnom [raison]
CHAN_KICK_PROTECTED
	%s ne peut pas être expulsé du canal %s.
CHAN_KICKED
	%s expulsé du canal %s.

# TOPIC responses
CHAN_TOPIC_SYNTAX
	TOPIC canal sujet

# CLEAR responses
CHAN_CLEAR_SYNTAX
	CLEAR canal quoi
CHAN_CLEARED_BANS
	Toutes les proscriptions (bans) inscrites pour le canal %s ont été retirées.
CHAN_CLEARED_EXCEPTIONS
	Toutes les exceptions sur le canal %s ont été retirées.
CHAN_CLEARED_MODES
	Tous les modes sur le canal %s ont été remis à leur valeur par défaut.
CHAN_CLEARED_OPS
	Le mode +o a été effacé pour le canal %s.
CHAN_CLEARED_HALFOPS
	Le mode +h a été effacé pour le canal %s.
CHAN_CLEARED_VOICES
	Le mode +v a été effacé pour le canal %s.
CHAN_CLEARED_USERS
	Tous les usagers ont été expulsé du canal %s.

# GETPASS responses
CHAN_GETPASS_SYNTAX
	GETPASS canal
### CHANGED: channel name parameter inserted
#CHAN_GETPASS_UNAVAILABLE
#	La commande GETPASS n'est pas disponible car l'encryption est en utilisation.
CHAN_GETPASS_PASSWORD_IS
	Le mot de passe pour le canal %s est %s.

# FORBID responses
CHAN_FORBID_SYNTAX
	FORBID canal
CHAN_FORBID_SHORT_CHANNEL
	Le canal # ne peut pas être interdit.
CHAN_FORBID_SUCCEEDED
	Le canal %s est maintenant interdit.
CHAN_FORBID_FAILED
	Ne peut pas interdire le canal %s!

# SUSPEND responses
CHAN_SUSPEND_SYNTAX
	SUSPEND [+expiration] canal raison
CHAN_SUSPEND_SUCCEEDED
	Le canal %s a été suspendu.
CHAN_SUSPEND_ALREADY_SUSPENDED
	Le canal %s a été suspendu.

# UNSUSPEND responses
CHAN_UNSUSPEND_SYNTAX
	UNSUSPEND canal
CHAN_UNSUSPEND_SUCCEEDED
	Le canal %s n'est plus suspendu.
CHAN_UNSUSPEND_NOT_SUSPENDED
	Le canal %s n'est pas suspendu.

###########################################################################
#
# MemoServ messages
#
###########################################################################

# Automatic messages
MEMO_HAVE_NEW_MEMO
	Vous avez 1 nouveau message.
MEMO_HAVE_NEW_MEMOS
	Vous avez %d nouveaux messages.
MEMO_TYPE_READ_LAST
	Tapez /msg %s READ LAST pour le lire.
MEMO_TYPE_READ_NUM
	Tapez /msg %s READ %d pour les lire.
MEMO_TYPE_LIST_NEW
	Tapez /msg %s LIST NEW pour les lister.
MEMO_AT_LIMIT
	Attention: Vous avez atteint votre nombre maximum de messages (%d).  Vous ne pourrez pas en recevoir de nouveau tant que vous n'en aurez pas effacé $
MEMO_OVER_LIMIT
	Attention: Vous excédez votre nombre maximum de messages (%d).  Vous ne pourrez pas en recevoir de nouveau tant que vous n'en aurez pas effacé $
MEMO_NEW_MEMO_ARRIVED
	Vous avez un nouveau message de %s.
	Tapez /msg %s READ %d pour le lire.

# Multi-use responses
MEMO_HAVE_NO_MEMOS
	Vous n'avez aucun messages.
MEMO_HAVE_NO_NEW_MEMOS
	Vous n'avez aucun nouveaux messages.
MEMO_DOES_NOT_EXIST
	Le message %d n'existe pas!
MEMO_LIST_NOT_FOUND
	Aucun message correspondant trouvé.

# SEND responses
MEMO_SEND_SYNTAX
	SEND {surnom | canal} texte-du-message
MEMO_SEND_DISABLED
	Désolé, l'envois de message est temporairement impossible.
MEMO_SEND_PLEASE_WAIT
	S.V.P. attendez %s avant d'utiliser la commande SEND de nouveau.
MEMO_X_GETS_NO_MEMOS
	%s ne peut pas recevoir de messages.
MEMO_X_HAS_TOO_MANY_MEMOS
	%s a présentement trop de messages et ne peut en recevoir plus.
MEMO_SENT
	Message expédié à %s.

# LIST responses
MEMO_LIST_SYNTAX
	LIST [num | liste | NEW]
MEMO_LIST_MEMOS
	Messages pour %s.  Pour en faire la lecture, tapez: /msg %s READ num
MEMO_LIST_NEW_MEMOS
	Nouveau message pour %s.  Pour en faire la lecture, tapez: /msg %s READ num
MEMO_LIST_HEADER
	   Num  Expéditeur        Date/Heure
MEMO_LIST_FORMAT
	%c%c%c%3d  %-16s  %s

# READ responses
MEMO_READ_SYNTAX
	READ {num | liste | LAST | NEW}
MEMO_HEADER
	Message %d de %s (%s).  Pour le suprimer, tapez: /msg %s DEL %d
#MEMO_CHAN_HEADER
#	Message %d de %s (%s).  Pour le suprimer, tapez: /msg %s DEL %s %d

# SAVE responses
MEMO_SAVE_SYNTAX
	SAVE {num | liste}
MEMO_SAVED_ONE
	Message %d marqué "sans expiration".
MEMO_SAVED_SEVERAL
	%d messages marqués "sans expiration".

# DEL responses
MEMO_DEL_SYNTAX
	DEL {num | liste | ALL}
MEMO_DELETED_NONE
	Aucun message n'a été effacé.
MEMO_DELETED_ONE
	%d messages ont été effacés.
MEMO_DELETED_SEVERAL
	%d messages ont été effacés.
MEMO_DELETED_ALL
	Tous vos messages ont été effacés.

# SET responses
MEMO_SET_SYNTAX
	SET option paramêtres
MEMO_SET_DISABLED
	Désolé, la sélection d'options de messagerie est temporairement impossible.
MEMO_SET_UNKNOWN_OPTION
	Option SET inconnue %s.

# SET NOTIFY responses
MEMO_SET_NOTIFY_SYNTAX
	SET NOTIFY {ON | LOGON | NEW | OFF}
MEMO_SET_NOTIFY_ON
	%s vous avertira de vos nouveaux messages lors de votre connection ainsi que lorsqu'ils vous seront expédiés.
MEMO_SET_NOTIFY_LOGON
	%s vous avertira maintenant de vos messages lors de votre connection sur ou retrait de /AWAY.
MEMO_SET_NOTIFY_NEW
	%s vous avisera de vos nouveaux messages losrqu'ils vous seront expédiés.
MEMO_SET_NOTIFY_OFF
	%s ne vous avisera pas à propos de l'état de votre messagerie.

# SET LIMIT responses
MEMO_SET_LIMIT_SYNTAX
	SET LIMIT limite
MEMO_SET_LIMIT_OPER_SYNTAX
	SET LIMIT [surnom] {limite | NONE | DEFAULT} [HARD]
MEMO_SET_YOUR_LIMIT_FORBIDDEN
	Vous n'êtes pas autorisé à modifier votre limite de message.
MEMO_SET_LIMIT_FORBIDDEN
	La limite de message pour %s ne peut pas être changée.
MEMO_SET_YOUR_LIMIT_TOO_HIGH
	Vous ne pouvez pas sélectionner une limite de message suppérieure à %d.
MEMO_SET_LIMIT_TOO_HIGH
	Vous ne pouvez pas sélectionner de limite de message pour %s supérieure à %d.
MEMO_SET_LIMIT_OVERFLOW
	Limite de message trop élevée; limité à %d à la place.
MEMO_SET_YOUR_LIMIT
	Votre limite de message a été déterminée à %d.
MEMO_SET_YOUR_LIMIT_ZERO
	Vous ne pourrez plus recevoir de messages.
MEMO_SET_YOUR_LIMIT_DEFAULT
	Votre limite de message a été déterminée à sa valeur par défaut (%d).
MEMO_UNSET_YOUR_LIMIT
	Votre limite de message a été désactivée.
MEMO_SET_LIMIT
	Limite de message pour %s déterminée à %d.
MEMO_SET_LIMIT_ZERO
	Limite de message pour %s déterminée à 0.
MEMO_SET_LIMIT_DEFAULT
	Limite de message pour %s déterminée à défaut (%d).
MEMO_UNSET_LIMIT
	Limite de message désactivée for %s.

# INFO responses
MEMO_INFO_NO_MEMOS
	Vous n'avez présentement aucun message.
MEMO_INFO_MEMO
	Vous avez présentement 1 message.
MEMO_INFO_MEMO_UNREAD
	Vous avez présentement 1 message(s), et il(s) n'est/ont toujours pas été lu(s).
MEMO_INFO_MEMOS
	Vous avez présentement %d messages.
MEMO_INFO_MEMOS_ONE_UNREAD
	Vous avez présentement %d messages, dont 1 non-lu.
MEMO_INFO_MEMOS_SOME_UNREAD
	Vous avez présentement %d messages, dont %d n'ont été lus.
MEMO_INFO_MEMOS_ALL_UNREAD
	Vous avez présentement %d messages; aucun d'entre eux n'ont été lus.
MEMO_INFO_LIMIT
	Votre limite de message est de %d.
MEMO_INFO_HARD_LIMIT
	Votre limite de message est de %d, et ne peut pas être changée.
MEMO_INFO_LIMIT_ZERO
	Votre limite de message est de 0; vous ne recevrez pas aucun nouveau message.
MEMO_INFO_HARD_LIMIT_ZERO
	Votre limite de message est de 0; vous ne recevrez pas aucun nouveau message.  Vous ne pouvez pas changer cette limite.
MEMO_INFO_NO_LIMIT
	Vous n'avez aucune limite relative au nombre de message que vous pouvez conserver.
MEMO_INFO_NOTIFY_OFF
	Vous ne serez plus avertis des nouveaux messages.
MEMO_INFO_NOTIFY_ON
	Vous serez informé de vos nouveaux messages lors de votre connection et lorsqu'ils entreront.
MEMO_INFO_NOTIFY_RECEIVE
	Vous serez informés de vos nouveaux messages lors de leur arrivée.
MEMO_INFO_NOTIFY_SIGNON
	Vous serez avisé de vos nouveaux messages lors de votre connection sur l'IRC.
MEMO_INFO_X_NO_MEMOS
	%s n'a actuellement aucun messages.
MEMO_INFO_X_MEMO
	%s a actuellement 1 message.
MEMO_INFO_X_MEMO_UNREAD
	%s actuellement a 1 message, et il n'est pas lu.
MEMO_INFO_X_MEMOS
	%s actuellement a %d messages.
MEMO_INFO_X_MEMOS_ONE_UNREAD
	%s actuellement a %d messages, dont 1 non-lu.
MEMO_INFO_X_MEMOS_SOME_UNREAD
	%s actuellement a %d messages, dont %d sont non-lus.
MEMO_INFO_X_MEMOS_ALL_UNREAD
	%s actuellement a %d messages; aucun d'entreux n'est lu.
MEMO_INFO_X_LIMIT
	La limite de message de %s est de %d.
MEMO_INFO_X_HARD_LIMIT
	La limite de message de %s est de %d, et ne peut pas être changée.
MEMO_INFO_X_NO_LIMIT
	%s n'a pas de limite de message.
MEMO_INFO_X_NOTIFY_OFF
	%s n'est pas avisé de ses nouveaux messages.
MEMO_INFO_X_NOTIFY_ON
	%s est avisé de ses messages à la connection et lors de leur arrivée.
MEMO_INFO_X_NOTIFY_RECEIVE
	%s est avisé lors de l'arrivée de nouveaux messages.
MEMO_INFO_X_NOTIFY_SIGNON
	%s est avisé lors de sa connection de ses nouveaux messages.

################ forward module messages/responses

# Mail format
MEMO_FORWARD_MAIL_SUBJECT
	Messages pour %s
MEMO_FORWARD_MAIL_BODY
	Message de %s (%s)
	%s

# FORWARD responses
MEMO_FORWARD_SYNTAX
	FORWARD {num | liste | ALL}
MEMO_FORWARD_NEED_EMAIL
	Vous devez avoir fourni préalablement une adresse de courriel avant que vos messages puissent vous y être re-transmis.
MEMO_FORWARD_PLEASE_WAIT
	S.V.P. attendez %s avant d'utiliser la commande FORWARD de nouveau.
MEMO_FORWARD_FAILED
	Désolé, la re-transmission de messagerie a échoué.
MEMO_FORWARDED_NONE
	Aucun message n'a été re-transmit.
MEMO_FORWARDED_ONE
	Le message %d a été re-transmit.
MEMO_FORWARDED_SEVERAL
	%d messages ont été re-transmis.
MEMO_FORWARDED_ALL
	Tous vos messages ont été retransmis.

# SET FORWARD responses
MEMO_SET_FORWARD_SYNTAX
	SET FORWARD {ON | COPY | OFF}
MEMO_SET_FORWARD_ON
	Vos messages seront re-transmis à votre adresse courriel : %s
MEMO_SET_FORWARD_COPY
	Vos messages seront maintenant re-transmis sur votre adresse courriel: %s
	Vos messages seront également sauvés par les Services.
MEMO_SET_FORWARD_OFF
	Vos messages ne seront pas re-transmis à votre adresse courriel.

################ ignore module messages/responses

# IGNORE responses
MEMO_IGNORE_SYNTAX
	IGNORE {ADD | DEL | LIST} [surnom | masque]
MEMO_IGNORE_ADD_SYNTAX
	IGNORE ADD surnom | masque
MEMO_IGNORE_DEL_SYNTAX
	IGNORE DEL surnom | masque
MEMO_IGNORE_LIST_FULL
	Votre liste d'indésirables est pleine.
MEMO_IGNORE_ALREADY_PRESENT
	%s déjà inscrit à votre liste d'indésirables.
MEMO_IGNORE_ADDED
	%s ajouté à votre liste d'indésirables.
MEMO_IGNORE_NOT_FOUND
	%s n'a pas été apperçu dans votre liste d'indésirables.
MEMO_IGNORE_DELETED
	%s effacé de votre liste d'indésirables.
MEMO_IGNORE_LIST_EMPTY
	Votre liste d'indésirables est vide.
MEMO_IGNORE_LIST
	Liste d'indésirables:
MEMO_IGNORE_LIST_X_EMPTY
	La liste d'indésirables de %s est vide.
MEMO_IGNORE_LIST_X
	Liste d'indésirables pour %s:

###########################################################################
#
# OperServ messages
#
###########################################################################

# General messages
OPER_BOUNCY_MODES
	Les services sont incapables de changer les modes.  Vos servers sont-ils correctement configurés?
OPER_BOUNCY_MODES_U_LINE
	Les services ne peuvent changer les modes.  Les U:lines de vos servers sont-elles correctement configurées?

# GLOBAL responses
OPER_GLOBAL_SYNTAX
	GLOBAL message

# STATS responses
OPER_STATS_UNKNOWN_OPTION
	Option STATS inconnue %s.
OPER_STATS_RESET_USER_COUNT
	Le maximum de comptes usagers a été mis à zéro.

OPER_STATS_CURRENT_USERS
	Nombre actuel d'usagers: %d (%d ops)
OPER_STATS_MAX_USERS
	Maximum d'usagers: %d (%s)
# Parameters: days, STR_DAYS or STR_DAY, hours, minutes
OPER_STATS_UPTIME_DHM
	Services en-ligne depuis %d%s, %02d:%02d
# If >= 1 hour, hours and minutes; if < 1 hour, minutes and seconds
OPER_STATS_UPTIME_HM_MS
	Services en-ligne depuis %s

OPER_STATS_KBYTES_READ
	Données reçues:      %10u kB
OPER_STATS_KBYTES_WRITTEN
	Données expédiées:   %10u kB
OPER_STATS_NETBUF_SOCK
	Base tampon de serveur:   %5u kB
OPER_STATS_NETBUF_SOCK_PERCENT
	Base tampon de serveur:   %5u kB (%d%%)
OPER_STATS_NETBUF_TOTAL
	Base tampon total:        %5u kB
OPER_STATS_NETBUF_TOTAL_PERCENT
	Base tampon total:        %5u kB (%d%%)

OPER_STATS_ALL_USER_MEM
	Usagers   : %6d registre, %5d kB
OPER_STATS_ALL_CHANNEL_MEM
	Canaux    : %6d registre, %5d kB
OPER_STATS_ALL_SERVER_MEM
	Servers   : %6d registre, %5d kB
OPER_STATS_ALL_NICKGROUPINFO_MEM
	Groupes de surnoms: %6d registre, %5d kB
OPER_STATS_ALL_NICKINFO_MEM
	Surnoms   : %6d registre, %5d kB
OPER_STATS_ALL_CHANSERV_MEM
	ChanServ  : %6d registre, %5d kB
OPER_STATS_ALL_STATSERV_MEM
	StatServ  : %6d registre, %5d kB
OPER_STATS_ALL_NEWS_MEM
	Nevelles  : %6d registre, %5d kB
OPER_STATS_ALL_AKILL_MEM
	Autokills : %6d registre, %5d kB
OPER_STATS_ALL_EXCEPTION_MEM
	Exceptions: %6d registre, %5d kB
OPER_STATS_ALL_SGLINE_MEM
	SGlines   : %6d registre, %5d kB
OPER_STATS_ALL_SQLINE_MEM
	SQlines   : %6d registre, %5d kB
OPER_STATS_ALL_SZLINE_MEM
	SZlines   : %6d registre, %5d kB
OPER_STATS_ALL_SESSION_MEM
	Sessions  : %6d registre, %5d kB

# GETKEY responses
### CHANGED: OPER_GETKEY_* added

# MODE responses
OPER_MODE_SYNTAX
	MODE canal modes

# CLEARMODES responses
OPER_CLEARMODES_SYNTAX
	CLEARMODES canal [ALL]
#OPER_CLEARMODES_DONE
#	Modes binaires et bans effacés du canal %s.
#OPER_CLEARMODES_ALL_DONE
#	Tous les modes effacés du canal %s.

# CLEARCHAN responses
OPER_CLEARCHAN_SYNTAX
	CLEARCHAN canal
OPER_CLEARCHAN_DONE
	Tous les usagers expulsés du canal %s.

# KICK responses
OPER_KICK_SYNTAX
	KICK canal usager raison

# ADMIN responses
OPER_ADMIN_SYNTAX
	ADMIN {ADD | DEL | LIST} [surnom]
OPER_ADMIN_ADD_SYNTAX
	ADMIN ADD surnom
OPER_ADMIN_DEL_SYNTAX
	ADMIN DEL surnom
OPER_ADMIN_NO_NICKSERV
	NickServ est présentement hors-service; la commande ADMIN n'est pas disponible.
OPER_ADMIN_EXISTS
	%s existe déjà sur la liste des Administrateurs des Services.
OPER_ADMIN_ADDED
	%s ajouté à la liste des Administrateurs des Services.
OPER_ADMIN_TOO_MANY
	Trop d'entrées (%d) sur la liste des Admin. des Services; ne peut en ajouter plus.
OPER_ADMIN_REMOVED
	%s retiré de la liste des Admin. des Services.
OPER_ADMIN_NOT_FOUND
	%s n'apparait pas dans la liste des Admin. des Services.
OPER_ADMIN_LIST_HEADER
	Liste des Administrateurs des Services:

# OPER responses
OPER_OPER_SYNTAX
	OPER {ADD | DEL | LIST} [surnom]
OPER_OPER_ADD_SYNTAX
	OPER ADD surnom
OPER_OPER_DEL_SYNTAX
	OPER DEL surnom
OPER_OPER_NO_NICKSERV
	NickServ est présentement hors-ligne; la commande OPER n'est pas disponible.
OPER_OPER_EXISTS
	%s existe déjà dans la liste des Opérateurs des Services.
OPER_OPER_ADDED
	%s ajouté à la liste des Opérateurs des Services.
OPER_OPER_TOO_MANY
	Trop d'entrées (%d) sur la liste des Opérateurs des Services; ne peut en ajouter d'autre.
OPER_OPER_REMOVED
	%s retiré de la liste des Opérateurs des Services.
OPER_OPER_NOT_FOUND
	%s n'apparait pas dans la liste des Opérateurs des Services.
OPER_OPER_LIST_HEADER
	Liste des Opérateurs des Services:

# AKILL/EXCLUDE/SxLINE common responses
### CHANGED: OPER_MASKDATA_* added

# AKILL responses
OPER_TOO_MANY_AKILLS
	La liste des AKILL est pleine!
OPER_AKILL_EXISTS
	%s existe déjà dans la liste des autokills.
OPER_AKILL_NO_NICK
	Avis: Les masques d'autokill ne peuvent pas contenir de surnoms; assurez-vous que vous n'avez pas inclus une portion surnom dans le masque.
OPER_AKILL_MASK_TOO_GENERAL
	Le masque d'autokill est trop général.
OPER_AKILL_EXPIRY_LIMITED
	Vous ne pouvez sélectionner un moment d'expiration jusqu'à %s.
OPER_AKILL_ADDED
	%s ajouté à la liste d'autokill.
OPER_AKILL_REMOVED
	%s retiré de la liste d'autokill.
OPER_AKILL_NOT_FOUND
	%s n'apparait pas à la liste d'autokill.
OPER_AKILL_LIST_HEADER
	Liste actuelle d'autokill:
### CHANGED: OPER_AKILL_{LIST_EMPTY,LIST_NO_MATCH,CHECK_*} added
OPER_AKILL_COUNT
	Il y a %d masques d'usager sur la liste d'autokill.

# EXCLUDE responses
OPER_TOO_MANY_EXCLUDES
	La liste EXCLUDE est pleine!
OPER_EXCLUDE_EXISTS
	%s existe déjà sur la liste d'exclusion d'autokill.
OPER_EXCLUDE_ADDED
	%s ajouté à la liste d'exclusion d'autokill.
OPER_EXCLUDE_REMOVED
	%s enlevé de la liste d'exclusion d'autokill.
OPER_EXCLUDE_NOT_FOUND
	%s n'apparait pas à la liste d'exclusion d'autokill.
OPER_EXCLUDE_LIST_HEADER
	Liste actuelle d'exclusion d'autokill:
### CHANGED: OPER_EXCLUDE_{LIST_EMPTY,LIST_NO_MATCH,CHECK_*} added
OPER_EXCLUDE_COUNT
	Il y a %d masques d'hôte sur la liste d'exclusion d'autokill.

# SxLINE responses
OPER_TOO_MANY_SLINES
	La liste %s est pleine!
OPER_SLINE_EXISTS
	%s déjà sur la liste %s.
OPER_SLINE_ADDED
	%s ajouté à la liste %s.
OPER_SLINE_REMOVED
	%s retiré de la liste %s.
OPER_SLINE_NOT_FOUND
	%s n'apparait pas sur la liste %s.
OPER_SLINE_LIST_HEADER
	Liste %s actuelle:
### CHANGED: OPER_SLINE_{LIST_EMPTY,LIST_NO_MATCH,CHECK_*} added
OPER_SLINE_COUNT
	Il y a %d entrées sur la liste %s.
OPER_SZLINE_NOT_AVAIL
	SZLINE n'est pas disponible avec ce server IRC.

# SU responses
OPER_SU_SYNTAX
	SU mot-de-passe
OPER_SU_NO_PASSWORD
	La commande SU n'est pas disponible car aucun mot-de-passe super-usager n'a été déterminé.
OPER_SU_SUCCEEDED
	Vous avez maintenant les privilèges de Super-Usager.
OPER_SU_FAILED
	Incapable de vérifier le mot-de-passe.

# SET responses
OPER_SET_SYNTAX
	SET option setting
OPER_SET_IGNORE_ON
	Le code ignore sera utilisé.
OPER_SET_IGNORE_OFF
	Le code ignore ne sera pas utilisé.
OPER_SET_IGNORE_ERROR
	L'ajustement de IGNORE doit être ON ou OFF.
OPER_SET_READONLY_ON
	Les services sont maintenant en mode lecture-seulement.
OPER_SET_READONLY_OFF
	Les services sont maintenant en mode lecture-écriture.
OPER_SET_READONLY_ERROR
	L'ajustement pour READONLY doit être ON ou OFF.
OPER_SET_DEBUG_ON
	Les services sont maintenant en mode DEBUG.
OPER_SET_DEBUG_OFF
	Les services sont maintenant en mode NON-DEBUG.
OPER_SET_DEBUG_LEVEL
	Les services sont maintenant en mode DEBUG (niveau %d).
OPER_SET_DEBUG_ERROR
	L'ajustement de DEBUG doit être ON, OFF, ou un nombre positif.
OPER_SET_SUPASS_FAILED
	Incapable de déterminer un mot-de-passe de Super-Usager des Services.
OPER_SET_SUPASS_OK
	Le mot de passe du Super-Usager des Services a été changé.
OPER_SET_SUPASS_NONE
	Le mot de passe du Super-Usager des Services a été désactivé.
OPER_SET_UNKNOWN_OPTION
	Option %s inconnue.

# JUPE responses
OPER_JUPE_SYNTAX
	JUPE nom-du-server [raison]
OPER_JUPE_INVALID_NAME
	Nom de server non-valide (les noms de server doivent contenir une période).
OPER_JUPE_ALREADY_JUPED
	Le server %s est déjà simulé.

# RAW responses
OPER_RAW_SYNTAX
	RAW texte

# UPDATE responses
OPER_UPDATE_SYNTAX
	UPDATE [FORCE]
OPER_UPDATING
	Mise à jour de la banque de données.

# REHASH responses
OPER_REHASHING
	Re-lecture des ajustements de configuration.
OPER_REHASHED
	Tous les ajustements de configuration ont été mis-à-jour avec succès.
OPER_REHASH_ERROR
	Une erreur est survenue lors de la re-lecture des ajustements de configuration.  S.V.P. vérifier le fichier de sauvegarde des Services pour les détails.

# KILLCLONES responses
OPER_KILLCLONES_SYNTAX
	KILLCLONES surnom
OPER_KILLCLONES_UNKNOWN_NICK
	Ne peut pas trouver l'usager %s.
OPER_KILLCLONES_KILLED
	%s a utilisé KILLCLONES pour %s tuant ainsi %d clones.
OPER_KILLCLONES_KILLED_AKILL
	%s a utilisé KILLCLONES pour %s tuant ainsi %d clones.  Un AKILL temporaire a été ajouté pout %s.


###########################################################################
#
# Session limiting and exception messages
#
###########################################################################

# EXCEPTION responses
#OPER_EXCEPTION_SYNTAX
#	EXCEPTION {ADD | DEL | MOVE | LIST | VIEW} [params]
OPER_EXCEPTION_ADD_SYNTAX
	EXCEPTION ADD [+expiration] masque limite raison
OPER_EXCEPTION_DEL_SYNTAX
	EXCEPTION DEL {masque | liste}
OPER_EXCEPTION_MOVE_SYNTAX
	EXCEPTION MOVE num nouveau-num
OPER_EXCEPTION_ALREADY_PRESENT
	Le masque %s est déjà inscrit à la liste d'exception.
OPER_EXCEPTION_TOO_MANY
	La liste d'exception se Limitation de Session est pleint!
OPER_EXCEPTION_ADDED
	La Limitations de Session pour %s est fixée à %d.
OPER_EXCEPTION_MOVED
	L'exception pour %s (#%d) a été déplacée au nombre %d.
OPER_EXCEPTION_NO_SUCH_ENTRY
	Il n'y a plus d'entrée (#%d) dans la liste d'exception de limitation de session.
OPER_EXCEPTION_NOT_FOUND
	%s n'apparait pas à la liste d'exception de limitation de session.
OPER_EXCEPTION_NO_MATCH
	Aucune entrée correspondante dans la liste d'exception de limitation de session.
OPER_EXCEPTION_EMPTY
	La liste d'exception de limitation de session est vide.
OPER_EXCEPTION_DELETED
	%s effacé de la liste d'exception de limitation de session.
OPER_EXCEPTION_DELETED_ONE
	Une entrée effacée de la liste d'exception de limitation de session.
OPER_EXCEPTION_DELETED_SEVERAL
	%d entrées effacées de la liste d'exception de limitation de session.
OPER_EXCEPTION_LIST_HEADER
	Liste actuelle d'exception de limitation de session:
# number, limit, host
OPER_EXCEPTION_LIST_COLHEAD
	Num  Limite  Hôte
OPER_EXCEPTION_LIST_FORMAT
	%3d  %4d   %s
# number, mask, set-by, set-time, expires, limit, reason
OPER_EXCEPTION_VIEW_FORMAT
	%3d.  %s  (par %s sur %s; %s)
	    Limite: %-4d  - %s
OPER_EXCEPTION_INVALID_LIMIT
	Limitation de session invalide; la limite doit être d'au moins zéro et jamais suppérieure à %d.
OPER_EXCEPTION_INVALID_HOSTMASK
	Masque d'hôte invalide.  Seuls les véritables masques d'hôte sont valides comme à titre d'exception et correspondent pas contre les surnoms et noms d'usagers.

# SESSION responses
OPER_SESSION_SYNTAX
	SESSION {LIST limite | VIEW hôte}
OPER_SESSION_LIST_SYNTAX
	SESSION LIST limite
OPER_SESSION_VIEW_SYNTAX
	SESSION VIEW hôte
OPER_SESSION_INVALID_THRESHOLD
	Valeur plancher invalide; la valeur plancher doit être un entier plus grand que 1.
OPER_SESSION_NOT_FOUND
	%s n'apparait pas dans la liste de la session.
OPER_SESSION_LIST_HEADER
	Hôtes avec au moins %d sessions:
OPER_SESSION_LIST_COLHEAD
	Sessions  Hôte
# limit, host
OPER_SESSION_LIST_FORMAT
	%6d    %s
# host, sessions, limit
OPER_SESSION_VIEW_FORMAT
	L'hôte %s a présentement %d sessions avec une limite de %d.


###########################################################################
#
# News system messages
#
###########################################################################

NEWS_LOGON_TEXT
	[Nouvelles au branchement - %s] %s
NEWS_OPER_TEXT
	[Nouvelles des Opérateurs - %s] %s

NEWS_LOGON_SYNTAX
	LOGONNEWS {ADD | DEL | LIST} [texte | num]
NEWS_LOGON_LIST_HEADER
	Articles de nouvelles au branchement:
NEWS_LOGON_LIST_ENTRY
	%5d (%s par %s)
	    %s
NEWS_LOGON_LIST_NONE
	Il n'y a pas de nouvelles au branchement.
NEWS_LOGON_ADD_SYNTAX
	LOGONNEWS ADD texte
NEWS_LOGON_ADD_FULL
	La liste des nouvelles est pleine!
NEWS_LOGON_ADDED
	Addition d'un nouvel item aux nouvelles (#%d).
NEWS_LOGON_DEL_SYNTAX
	LOGONNEWS DEL {num | ALL}
NEWS_LOGON_DEL_NOT_FOUND
	L'item des nouvelles au branchement #%d n'apparait pas!
NEWS_LOGON_DELETED
	L'item des nouvelles au branchement #%d est effacé.
NEWS_LOGON_DEL_NONE
	Aucun item de nouvelles au branchement à effacer!
NEWS_LOGON_DELETED_ALL
	Tous les itemps des nouvelles au branchement ont été effacés.

NEWS_OPER_SYNTAX
	OPERNEWS {ADD | DEL | LIST} [texte | num]
NEWS_OPER_LIST_HEADER
	Item des nouvelles des Opérateurs:
NEWS_OPER_LIST_ENTRY
	%5d (%s par %s)
	    %s
NEWS_OPER_LIST_NONE
	Il n'y a pas de nouvelles pour les opérateurs.
NEWS_OPER_ADD_SYNTAX
	OPERNEWS ADD texte
NEWS_OPER_ADD_FULL
	La liste des nouvelles est pleine!
NEWS_OPER_ADDED
	Ajout de l'item (#%d) aux nouvelles au branchement.
NEWS_OPER_DEL_SYNTAX
	OPERNEWS DEL {num | ALL}
NEWS_OPER_DEL_NOT_FOUND
	L'item des nouvelles des opérateurs #%d n'apparait pas!
NEWS_OPER_DELETED
	L'item des nouvelles des opérateurs #%d est effacé.
NEWS_OPER_DEL_NONE
	Aucune item de nouvelle d'opérateur à effacer!
NEWS_OPER_DELETED_ALL
	Tous les items de nouvelles d'opérateur sont effacés.

NEWS_HELP_LOGON
	Syntaxe: LOGONNEWS ADD texte
	         LOGONNEWS DEL {num | ALL}
	         LOGONNEWS LIST
	
	Permet l'édition ou la présentation de la liste des messages
	de nouvelles au branchement.  Lorsqu'un usager se connecte
	au réseau, ces messages lui seront expédiés.  (Toutefois,
	pas plus de trois messages seront expédiés afin d'éviter
	d'innonder l'usager.  Si il y a plus de trois messages de
	nouvelles, seuls les trois plus récents seront expédiés.)
	
	La commande LOGONNEWS LIST peut être utilisée par n'importe
	quel Opérateur d'IRC afin d'obtenir le sommaire actuel des
	messages de nouvelles.  ADD et DEL peuvent égalelent être
	utilisés par les Opérateurs des Services.

NEWS_HELP_OPER
	Syntaxe: OPERNEWS ADD texte
	         OPERNEWS DEL {num | ALL}
	         OPERNEWS LIST
	
	Permet l'édition ou la présentation de la liste des nouvelles
	des Opérateurs. Lorsqu'un usager obtient le status
	d'Opérateur IRC (avec la commande /OPER), Ces messages leurs
	seront expédiés.  (Toutefois, pas plus de trois messages
	seront expédiés afin d'éviter d'innonder l'usager.  S'il y a
	plus de trois messages de nouvelles, seuls les trois plus
	récents seront expédiés.)
	
	La commande OPERNEWS LIST peut être utilisée par n'importe
	quel Opérateur d'IRC pour avoir la liste actuelle des
	messages de nouvelle des Opérateurs.  ADD et DEL peuvent
	seulement être utilisé par les Opérateurs des Services.


###########################################################################
#
# StatServ messages
#
############################################################################

# General SERVERS responses
STAT_SERVERS_REMOVE_SERV_FIRST
	Le serveur %s est en-ligne. Retirez le du réseau et essayez de nouveau.
STAT_SERVERS_SERVER_EXISTS
	Le serveur %s existe déjà.

# SERVERS command syntax
STAT_SERVERS_SYNTAX
	SERVERS option paramêtres

# STATS responses
STAT_SERVERS_STATS_TOTAL
	Serveurs au total: %2d
STAT_SERVERS_STATS_ON_OFFLINE
	       En-ligne: %2d (%d%%)  Hors-ligne: %2d (%d%%)
STAT_SERVERS_LASTQUIT_WAS
	    Dernière Sortie: %-30s  %s

# LIST responses
STAT_SERVERS_LIST_HEADER
	Nom du serveur                 Usagers    Opérateurs
STAT_SERVERS_LIST_FORMAT
	%-30s %3d (%2d%%)  %3d (%2d%%)
STAT_SERVERS_LIST_RESULTS
	Fin de la liste -- %d/%d servers en-ligne présentés.

# VIEW responses
STAT_SERVERS_VIEW_HEADER_ONLINE
	%-30s  (en-ligne)
STAT_SERVERS_VIEW_HEADER_OFFLINE
	%-30s  (hors-ligne)
STAT_SERVERS_VIEW_LASTJOIN
	    Dernière entrée: %s
STAT_SERVERS_VIEW_LASTQUIT
	    Dernière sortie: %s
STAT_SERVERS_VIEW_QUITMSG
	  Message de sortie: %s
STAT_SERVERS_VIEW_USERS_OPERS
	        Usagers: %3d (%2d%%)  Opérateurs: %3d (%2d%%)
STAT_SERVERS_VIEW_RESULTS
	Fin de la liste -- %d/%d serveurs présentés.

# DELETE responses
STAT_SERVERS_DELETE_SYNTAX
	SERVERS DELETE serveur
STAT_SERVERS_DELETE_DONE
	Statistiques pour le serveur %s a été suprimé.

# COPY responses
STAT_SERVERS_COPY_SYNTAX
	SERVERS COPY serveur nouveau-nom
STAT_SERVERS_COPY_DONE
	Statistiques pour le serveur %s copiées pour %s.

# RENAME responses
STAT_SERVERS_RENAME_SYNTAX
	SERVERS RENAME serveur nouveau-nom
STAT_SERVERS_RENAME_DONE
	Statistiques pour le serveur %s renomées pour %s.

# USERS responses
STAT_USERS_SYNTAX
	STATISTIQUES D'USAGERS
STAT_USERS_TOTUSERS
	                 Total d'usagers: %d
STAT_USERS_TOTOPERS
	              Total d'opérateurs: %d
STAT_USERS_SERVUSERS
	   Moyenne d'usagers par serveur: %d
STAT_USERS_SERVOPERS
	Moyenne d'opérateurs par serveur: %d.%d


###########################################################################
#
# NickServ help messages
#
###########################################################################

NICK_HELP
	%S vous permet d'"enregistrer" un surnom et de
	prévenir son utilisation par quelqu'un d'autre.  %S est
	contrôlé par différentes commandes lesquelles permettent
	l'enregistrement et le maintien des surnoms.  Pour une liste
	des commandes, tapez /msg %S HELP COMMANDS; pour
	utiliser une commande, tapez /msg %S commande, et pour
	plus d'information sur une commande spécifique, tapez
	/msg %S HELP commande.
NICK_HELP_EXPIRES
	
	Si un surnom enregistré n'est pas utilisé pendant %s,
	l'enregistrement va expirer et le surnom sera disponible
	pour l'usage par n'importe qui.
NICK_HELP_WARNING
	
	REMARQUE: Ce service se veut un moyen d'assurer aux usagers
	IRC que leur identité n'est pas compromise. Ce n'est PAS
	pour faciliter le "vol" de surnom ou d'autres actions
	malicieuses.  En cas d'abus de %S il en résultera, au
	minimum, la perte du surnom usurpé.

NICK_HELP_COMMANDS
	Les commandes suivantes peuvent être utilisées avec
	%S:
	
	    REGISTER     Enregistrement d'un surnom
	    IDENTIFY     Vous identifie par votre mot de passe
	    DROP         Annule l'enregistrement d'un surnom
### CHANGED: REAUTH/RESTOREMAIL added
#NICK_HELP_COMMANDS_AUTH
#	    AUTH         Confirmation de l'enregistrement du surnom
#	    SENDAUTH     Vous expédie le code de confirmation du surnom
NICK_HELP_COMMANDS_LINK
	    LINK         Lie un autre surnom à votre propre surnom
	    UNLINK       Retire le lien avec un surnom lié
	    LISTLINKS    Présente la liste de tous les surnoms que vous avez liés
NICK_HELP_COMMANDS_ACCESS
	    ACCESS       Modifie la liste des adresses autorisées
NICK_HELP_COMMANDS_AJOIN
	    AJOIN        Modifie votre liste de canal auto-rejoint
NICK_HELP_COMMANDS_SET
	    SET          Détermine les options, incluant la protection
	                     Kill
	    UNSET        Efface l'information d'un surnom
	    RECOVER      Débranche un autre usager qui utilise votre
	                     surnom
	    RELEASE      Libère votre surnom après la commande RECOVER
	    GHOST        Débranche une session "fantôme"
	    INFO         Présente l'information sur un surnom
NICK_HELP_COMMANDS_LIST
	    LIST         Présente tous les surnoms correspondants à un
	                     masque donné
	    LISTEMAIL    Présente les surnoms par adresses courriel
NICK_HELP_COMMANDS_LISTCHANS
	    LISTCHANS    Présente tous les canaux qui vous
	                     appartiennent
	    STATUS       Vérifis qu'un surnom donné est identifié

NICK_HELP_REGISTER
	Syntaxe: %s
	
	Enregistre votre surnom dans la banque de données de
	%S.  Après cet enregistrement, vous pouvez utiliser
	les commandes SET et ACCESS pour configurer les ajustements
	de votre surnom selon ce que vous désirez.  Assurez-vous de
	vous souvenir de votre mot de passe déterminé à
	l'enregistrement; vous en aurez besoin pour apporter des
	changements à votre surnom après.  (Notez que les formes
	suivantes!  FIDO, Fido, et fido sont tous des
	mots-de-passe différents!)
NICK_HELP_REGISTER_EMAIL
	
	Vous devez inclure une adresse courriel lorsque vous
	enregistrez votre surnom; vous pourez la changer en
	utilisant la commande SET EMAIL.
NICK_HELP_REGISTER_EMAIL_REQ
	
	Vous devez inclure une adresse courriel lorsque
	vous enregistrez votre surnom.  Vous pouvez éviter
	l'affichage de cette adresse auprès des autres usagers en
	utilisant la commande SET HIDE (tapez /msg %S HELP SET
	HIDE pour plus d'information).
NICK_HELP_REGISTER_EMAIL_AUTH
	
	Vous devez inclure une adresse courriel valide lorsque vous
	enregistrez votre surnom;  %S expédira un message avec
	un code d'autorisation à cette adresse, lequel devra être
	utilisé avec la commande AUTH pour compléter votre
	enregistrement.  Vous pouvez éviter que cette adresse
	courriel soit présentée aux autres usagers en utilisant la
	commande SET HIDE (tapez /msg %S HELP SET HIDE pour
	plus d'information).
NICK_HELP_REGISTER_END
	
	Ligne directrice relative au choix du mot de passe:
	
	Un mot de passe ne doit pas être facile à découvrir.  Par exemple,
	utiliser votre véritable nom comme mot de passe est une mauvaise idée.
	Utiliser votre surnom comme mot de passe n'est pas plus une bonne idée,
	et en fait, %S ne le permettra pas.  Aussi, les courts mots-de-passe
	sont plus vulnérables en recherche essais-erreur, donc vous devez
	choisir un mot de passe d'au moins 5 caractères de long.

NICK_HELP_IDENTIFY
	Syntaxe: IDENTIFY mot-de-passe
	
	Dit à %S que vous êtes réellement le propriétaire de
	ce surnom.  Plusieurs commandes requierrent que vous soyez
	authentifiés avant de pouvoir les utiliser.  Le mot de passe
	doit être le même que celui donné avec la commande REGISTER.

NICK_HELP_DROP
	Syntaxe: DROP mot-de-passe
	
	Désenregistre votre surnom de la banque de donnée de
	%S. Un surnom désenregistré est disponible pour
	quiconque pour le ré-enregistrer.
NICK_HELP_DROP_LINK
	
	Remarquez que lorsque vous désenregistrez un surnom, tous
	les surnoms liés à ce dernier seront également
	désenregistrés.  Pour retirer un seul surnom lié, utilisez
	d'abord la commande UNLINK.
NICK_HELP_DROP_END
	
	Pour prévenir l'utilisation accidentelle de cette commande,
	vous devez entrer votre mot-de-passe afin de désenregistrer
	le surnom.

### CHANGED: authorization -> authentication
#NICK_HELP_AUTH
#	Syntaxe: AUTH code
#	
#	Autorise l'enregistrement ou le changement de courriel pour
#	un surnom.  Pour s'assurer que votre adresse courriel est
#	correcte, %S expédira un message comprenant un code
#	d'autorisation à l'adresse courriel donnée lors de
#	l'enregistrement de votre surnom ou lors du changement de
#	votre adresse courriel avec la commande SET EMAIL.  Cette
#	commande permet de confirmer que vous avez reçu le message
#	et vous permet de continuer d'utiliser votre surnom
#	normalement.  Remplacez code dans la commande par le code
#	d'autorisation de neuf chiffres donné dans le message
#	courriel.

### CHANGED: authorization -> authentication
#NICK_HELP_SENDAUTH
#	Syntaxe: SENDAUTH
#	
#	Vous expédie une copie du code d'autorisation de votre
#	surnom après l'enregistrement (REGISTER) ou un changement de
#	courriel (SET EMAIL).  Si vous effacez accidentellement, ou
#	perdez le message incluant le code d'autorisation d'une
#	autre manière, vous pouvez utiliser cette commande afin
#	qu'une nouvelle copie de ce message vous soit expédiée.
#	
#	Afin de prévenir les abus, cette commande ne peut être
#	utilisée qu'une fois toutes les 24 heures.

### CHANGED: last paragraph changed
#NICK_HELP_LINK
#	Syntaxe: LINK surnom
#	
#	Lie un nouveau surnom au surnom que vous utilisez
#	actuellement, crée un nouvel "alias" pour votre surnom.  Le
#	nouveau surnom sera l'équivalent de votre surnom actuel en
#	terme de privilèges d'accès et de messagerie; par exemple,
#	vous pouvez lire les messages expédiés à un de vos surnoms
#	liés à partir de n'importe quel autre, et si un de vos
#	surnoms dispose de privilèges auto-op sur un canal, vous
#	serez auto-opé, peu importe le surnom que vous utilisez.
#	
#	Pour annuler un lien, utilisez la commande UNLINK.
#	
#	Afin d'utiliser cette commande, vous devez vous identifier
#	pour votre surnom actuel (utiliser la commande IDENTIFY), et
#	le surnom à être lié ne doit pas être encore enregistré.

NICK_HELP_UNLINK
	Syntaxe: UNLINK surnom
	
	Enlève tous les liens (alias) sur votre surnom. créés avec
	la commande LINK.  Vous ne pouvez pas dé-lier le surnom que
	vous utilisez actuellement; changez pourun autre surnom lié
	avant d'utiliser cette commande.
	
	Si vous dé-liez le "surnom principal" vous devez utiliser la
	commande SET MAINNICK, votre surnom actuel sera
	automatiquement sélectionné comme surnom principal.

NICK_HELP_LISTLINKS
	Syntaxe: LISTLINKS
	
	Présente tous les surnoms qui sont liés avec le vôtre.  Le
	surnom principal du groupe des surnoms liés sera indiqué par
	un "*" à gauche du surnom.

NICK_HELP_ACCESS
	Syntaxe: ACCESS ADD masque
	         ACCESS DEL masque
	         ACCESS LIST
	
	Modifie ou présente la liste d'accès pour votre surnom.  Il
	s'agit de la liste des adresses usager@hôte qui seront
	automatiquement reconnues par %S comme étant
	autorisées à utiliser le surnom.  Si vous vous connectez à
	l'IRC avec une adresse apparaissant à cette liste, vous ne
	serez pas affecté par l'ajustement SET KILL, et si l'option
	SECURE est désactivée, vous serez en mesure de recevoir les
	privilèges auto-op ainsi que les autres privilèges des
	canaux sans avoir à utiliser la commande IDENTIFY.  Si vous
	voulez utiliser le surnom à partir d'une adresse différente,
	vous devez expédier une commande IDENTIFY pour que
	%S vous reconnaisse.
	
	Le masque donné avec les commandes ADD et DEL peut
	contenir des frimes (* ou ?).
	
	Exemples:
	
	    ACCESS ADD achurch@*.dragonfire.net
	        Permet à l'usager achurch d'accéder de n'importe
	        quelle machine du domaine dragonfire.net.
	
	    ACCESS DEL achurch@*.dragonfire.net
	        Renverse la commande précédante.
	
	    ACCESS LIST
	        Présente la liste d'accès actuelle.

NICK_HELP_SET
	Syntaxe: SET option paramêtres
	
	Détermine une variété d'options de surnom.  L'option peut
	être une des suivantes:
	
	    PASSWORD   Détermine votre mot de passe
	    LANGUAGE   Détermine la langue qu'utiliseront les
	               Services lorsqu'ils vous expédiront un
	               message
	    URL        Associe un URL à votre surnom
	    EMAIL      Associe une adresse courriel à votre surnom
	    INFO       Détermine le texte à être présenté avec la
	               commande INFO
	    KILL       Mets la protection "kill" à on ou off
	    SECURE     Mets la sécurité de surnom à on ou off
	    PRIVATE    Évite que votre surnom apparaisse dans un
	                   /msg %S LIST
	    HIDE       Voile certain détails de l'information
	               relative à votre surnom
	    TIMEZONE   Détermine le fuseau horaire pour vous donner
	               l'heure juste
NICK_HELP_SET_OPTION_MAINNICK
	    MAINNICK   Détermine le surnom présenté dans la liste
	               d'accès de canal
NICK_HELP_SET_END
	
	Afin d'utiliser cette commande, vous devez d'abord vous
	identifier avec votre mot de passe (/msg %S HELP
	IDENTIFY pour plus d'information).
	
	Tapez /msg %S HELP SET option pour plus d'information
	sur une option spécifique.

NICK_HELP_SET_PASSWORD
	Syntaxe: SET PASSWORD nouveau-mot-de-passe
	
	Change le mot-de-passe utilisé pour vous identifier comme
	propriétaire du surnom.

NICK_HELP_SET_LANGUAGE
	Syntaxe: SET LANGUAGE numéro
	
	Change la langue que les Services utilisent lorsqu'ils vous
	expédient un message (par exemple, en réponse à une commande
	que vous avez donné).  Le numéro doit être choisi dans la
	liste des langues suportées suivante:

NICK_HELP_SET_URL
	Syntaxe: SET URL url
	
	Associe l'URL donné avec votre surnom. Cet URL sera présenté
	chaque fois que quelqu'un fera une demande d'information sur
	votre surnom avec la commande INFO.

NICK_HELP_SET_EMAIL
	Syntaxe: SET EMAIL adresse
	
	Associe l'adresse courriel donnée avec votre surnom.  Cette
	adresse sera présentée chaque fois que quelqu'un fera une
	demande d'information sur votre surnom avec la commance INFO.

NICK_HELP_SET_INFO
	Syntaxe: SET INFO texte
	
	Associe le texte donné avec votre surnom; ça sera présenté chaque
	fois qu'un usager demandera de l'information sur votre surnom avec
	la commande INFO.

NICK_HELP_SET_KILL
	Syntaxe: SET KILL {ON | QUICK | IMMED | OFF}
	
	Mets à on ou off la protection "kill" pour votre surnom.
	Avec la protection à "ON", si un autre usager essais de
	prendre votre surnom, les services lui donneront une minute
	pour changer pour un autre surnom, après quoi, il sera forcé
	par %S de quitter l'IRC.
	
	Si vous sélectionnez QUICK, l'usager disposera seulement de
	20 secondes pour changer son surnom au lieu des 60
	habituelles.  Si vous sélectionnez IMMED, l'usager sera
	immédiatement débranché sans être préalablement averti et
	sans aucune chance de changer son surnom; s.v.p. ne pas
	utiliser cette option si ce n'est pas nécessaire.  Aussi,
	votre administrateur de réseau peut avoir désactivé cette
	option.

NICK_HELP_SET_SECURE
	Syntaxe: SET SECURE {ON | OFF}
	
	Mets les dispositifs de protection de %S à on ou off
	pour votre surnom.  Avec SECURE sélectionné, vous devez
	entrer votre mot de passe avant d'être reconnu comme le
	propriétaire du surnom, sans égard si l'adresse apparait
	dans la liste d'accès.  Toutefois, si vous êtes sur la liste
	d'accès, %S ne vous déconnectera pas, et ce peu
	importe l'ajustement de l'option KILL.

NICK_HELP_SET_PRIVATE
	Syntaxe: SET PRIVATE {ON | OFF}
	
	Mets l'option privée de %S à ON ou OFF pour votre
	surnom. Avec PRIVATE sélectionné, votre surnom m'apparaîtra
	pas dans la liste de surnoms générée avec la commande LIST
	de %S. (Toutefois, quiconque connaît votre surnom peut
	continuer d'obtenir de l'information sur vous avec la
	commande INFO.)

NICK_HELP_SET_HIDE
	Syntaxe: SET HIDE {EMAIL | USERMASK | QUIT} {ON | OFF}
	
	Vous permet de prévenir que certaines pièces d'information
	soient exposées lorsque quelqu'un fait une requête %S
	INFO sur votre surnom.  Vous pouvez masquer votre adresse
	courriel (EMAIL), le dernier masque user@host appreçu
	(USERMASK), et le dernier message de sortie (QUIT).  Le
	second paramêtre spécifie quelle information doit être
	présentée (OFF) ou masquée (ON).

NICK_HELP_SET_TIMEZONE
	Syntaxe: SET TIMEZONE {différence de TUC | fuseau-horaire | DEFAULT}
	
	Détermine le fuseau horaire pour l'affichage de temps (ainsi
	que les derniers moments utilisé pour un canal ou un surnom),
	au lieu d'utiliser le fuseau horaire sur lequel les Services
	opèrent; la sélection de DEFAULT permet de retourner au
	fuseau horaire des Services.  Le fuseau horaire peut être
	spécifié d'une ou 2 manières:
	
	   - Tel une différence de TUC, la différence de temps entre
	     votre heure locale et le TUC (Temps Universel
	     Coordonné, également connu sous l'appellation heure de
	     Greenwich).  La différence est donnée par un "+" ou un
	     "-" pour indiquer après (Est) ou avant (Ouest) le TUC,
	     suivi par un nombre entier d'heures ou fractionnaire
	     dans la forme "hh:mm".
	
	   - Tel un nom de zone horaire, comme "GMT" ou "EDT".  Ce
	     ne sont pas tous les noms de fuseaux horaires qui sont
	     supportés par les Services, et certains fuseaux
	     horaires de pays ont le même nom, donc si vous obtenez
	     un message d'erreur ou que le temps affiché par les
	     Services est erronné, utilisez la différence de TUC à
	     la place.
	
	Notez que les Services ne s'ajusteront pas automatiquement
	pour l'heure avancée; vous aurez à l'ajuster manuellement.
	
	Exemples:
	
	    SET TIMEZONE -5
	        Détermine le fuseau horaire à 5 heures avant le TUC.
	
	    SET TIMEZONE +3:30
	        Détermine le fuseau horaire à 3 heures, 30 minutes
	        après le TUC.
	
	    SET TIMEZONE JST
	        Détermine le fuseau horaire à l'heure normale du
	        Japon (TUC+9:00).

NICK_HELP_SET_MAINNICK
	Syntaxe: SET MAINNICK surnom
	
	Lorsque vous avez plusieurs surnoms liés ensembles, vous
	permet de sélectionner lequel apparaîtra dans la liste
	d'accès du canal.  Si plus tard, vous dé-liez ce surnom,
	après un autre sera sélectionné automatiquement.

NICK_HELP_UNSET
	Syntaxe: UNSET {URL | EMAIL | INFO}
	
	Vous permet d'effacer l'URL (URL), l'adresse courriel
	(EMAIL), ou le texte d'information (INFO) associé à votre
	surnom.

NICK_HELP_UNSET_REQ_EMAIL
	Syntaxe: UNSET {URL | INFO}
	
	Vous permet d'effacer l'URL (URL) ou le texte d'information
	(INFO) associé à votre surnom.

#NICK_HELP_RECOVER
#	Syntaxe: RECOVER surnom [mot-de-passe]
#	
#	Vous permet de reprendre votre surnom si quelqu'un d'autre
#	l'a prit; ceci a le même effet que la protection "kill"
#	automatique de %S sur un surnom protégé.
#	
#	Lorsque vous donnez cette commande, %S emmênera un
#	faux usager en-ligne avec le même surnom que l'usager duquel
#	vous désirez reprendre le surnom, forçant ainsi le server
#	IRC à débrancher l'autre usager.  Ce faux usager restera en
#	ligne pendant une minute pour s'assurer que l'autre usager
#	ne se reconnectera pas automatiquement; après une minute,
#	vous pourrez reprendre votre surnom.  Vous pouvez aussi
#	utiliser la commande RELEASE (/msg %S HELP RELEASE)
#	pour reprendre le surnom plus rapidement.
#	
#	Dans le but d'utiliser la commande RECOVER pour un surnom,
#	vous devez fournir le bon mot de passe pour le surnom, ou
#	vous devez déjà être identifié sur un surnom lié au surnom
#	donné.

#NICK_HELP_RELEASE
#	Syntaxe: RELEASE surnom [mot-de-passe]
#	
#	Ordonne à %S de retirer toute suspension sur votre
#	surnom provoquée par une protection "kill" automatique ou
#	par l'utilisation de la commande RECOVER.  Par défaut,
#	toutes ces suspensions durent une minute; cette commande
#	permet d'écourcir ce délais.
#	
#	Afin d'utiliser la commande RELEASE pour un surnom, vous
#	devez fournir le bon mot de passe pour ce surnom, ou vous
#	devez être déjà identifié à un surnom qui est lié au surnom
#	donné.

NICK_HELP_GHOST
	Syntaxe: GHOST surnom [mot-de-passe]
	
	Mets fin à une session "fantôme" d'IRC utilisant votre
	surnom.  Une session "fantôme" en est une qui n'est pas
	actuellement connectée, mais pour laquelle le server IRC
	croit qu'elle est toujours en-ligne pour une raison ou une
	autre.  Tipiquement, ceci survient si votre ordinateur
	s'arrête ou si votre connection modem à Internet tombe
	pendant que vous êtes sur IRC.
	
	Afin d'utiliser la commande GHOST pour un surnom, vous devez
	fournir le bon mot-de-passe pour le surnom, ou vous devez
	déjà être identifié à un surnom qui est lié au surnom donné.

NICK_HELP_INFO
	Syntaxe: INFO surnom [ALL]
	
	Présente l'information relative à un surnom donné, telle que
	le propriétaire du surnom, sa dernière adresse et son
	dernier moment de branchement ainsi que les options de son
	surnom.  Si vous êtes identifié pour le surnom sur lequel
	vous demandez de l'information, vous recevrez TOUTE
	l'information sans égard relativement à ce que vous avez
	demandé d'être masqué ou pas.
NICK_HELP_INFO_AUTH
	
	Si l'adresse courriel du surnom n'a pas encore été confirmée
	avec la commande AUTH, cette adresse ne sera pas présenté à
	personne excepté le propriétaire du surnom et les
	Administrateurs des services, sans égard à l'ajustement SET
	HIDE EMAIL du surnom.

NICK_HELP_LISTCHANS
	Syntaxe: LISTCHANS
	
	Présente le nom et le nombre total de tous les canaux que
	vous avez enregistré sous votre surnom.

#NICK_HELP_LIST
#	Syntaxe: LIST modèle
#	
#	Présente tous les surnoms enregistré correspondant avec le
#	modèle donné eux-mêmes à la dernière adresse apperçue (dans
#	le format usager@hôte). Les surnoms dont l'option PRIVATE
#	est sélectionnée ne seront pas affichés.
#	
#	Exemples:
#	
#	    LIST joeuser@foo.com
#	        Présente tous les surnoms enregistrés qui ont été
#	        utilisés pour la dernière fois pas joeuser@foo.com.
#	
#	    LIST *Bot*
#	        Présente tous les surnoms enregistrés contenant Bot
#	        dans leurs noms (cas le plus fréquent).
#	
#	    LIST *@*.bar.org
#	        Présente tous les surnoms enregistrés utilisés dans
#	        le domaine bar.org.

NICK_HELP_LIST_OPERSONLY
	
	Seuls les Opérateurs d'IRC peuvent utiliser cette fonction.

#NICK_HELP_LISTEMAIL
#	Syntaxe: LISTEMAIL modèle
#	
#	Présente les surnoms enregistrés comme la commande LIST,
#	mais correspondant à l'adresse courriel au lieu de l'adresse
#	usager@hôte. Voir la commande d'aide de LIST (/msg %S
#	HELP LIST) pour plus d'information.

NICK_HELP_STATUS
	Syntaxe: STATUS surnom...
	
	Réponds si l'usager utilisant le surnom donné est reconnu
	comme propriétaire du surnom.  La réponse a la forme
	suivante:
	
	    surnom status-code
	
	où surnom est le surnom donné avec la commande, et
	status-code is one of the following:
	
	    0 - aucun usager en ligne ou surnom pas enregistré
	    1 - usager non-reconnu comme propriétaire du surnom
	    2 - usager reconnu comme propriétaire via la liste
	        d'accès seulement
	    3 - usager reconnu comme propriétaire du surnom, via
	        identification par mot-de-passe
	
	Jusqu'à seize surnoms peuvent être envoyés avec chaque
	commande; les autres seront ignorées.  Aucun message
	d'erreur n'est généré si aucun surnom est donné.

#NICK_HELP_AJOIN
#	Syntaxe: AJOIN ADD canal
#	         AJOIN DEL canal
#	         AJOIN LIST
#	
#	Modifie ou présente la liste de canaux auto-joints pour
#	votre surnom.  Il s'agit de la liste des canaux que vous
#	désirez joindre automatiquement, dès que vous vous
#	identifiez à %S.  Remarquez que vous pouvez seulement
#	mettre des noms de canaux commençant par "#" dans la liste
#	auto-joindre (les canaux locaux ne peuvent pas être
#	ajoutés).

NICK_OPER_HELP_COMMANDS
	
	Les commandes suivantes sont réservées aux Administrateurs
	des services:
	
	    DROPNICK     Effacer un surnom
NICK_OPER_HELP_COMMANDS_GETPASS
	    GETPASS      Donne le mot de passe pour un surnom
	                     (seulement si l'encryption n'est pas
	                      active)
NICK_OPER_HELP_COMMANDS_FORBID
	    FORBID       Empêche un surnom d'être utilisé/enregistré
	    SUSPEND      Empêche de s'identifier pour un surnom
	    UNSUSPEND    Annule une suspension de surnom
### CHANGED: authorization -> authentication
#NICK_OPER_HELP_COMMANDS_SETAUTH
#	    SETAUTH      Détermine un nouveau code d'authorisation
#	                 pour un surnom
#	    GETAUTH      Présente le code d'autorisation pour un
#	                 surnom
#	    CLEARAUTH    Efface le code d'autorisation pour un surnom
NICK_OPER_HELP_COMMANDS_END
	
	Les administrateurs des services peuvent également utiliser
	les commandes suivantes sur n'importe quel surnom, sans
	s'identifier pour lui:
	
	    SET, ACCESS LIST, INFO ALL
	
	Vois "help" pour chacune de ces commandes afin d'avoir des
	détails spécifiques.

NICK_OPER_HELP_DROPNICK
	Syntaxe: DROPNICK surnom
	
	Efface le surnom donné de la banque de donnée de %S,
	ainsi que tous les surnoms qui y sont liés.
	
	Limité à l'usage des Administrateurs des Services.

NICK_OPER_HELP_SET
	
	Les Administrateurs des Services peuvent également
	déterminer l'option NOEXPIRE, pour permettre de prévenir
	qu'un surnom n'expire.  En plus, les Administrateurs des
	Services peuvent des options pour tous les surnoms sans
	entrer de mot-de-passe, en utilisant le format SET !surnom
	option paramêtres.

NICK_OPER_HELP_SET_NOEXPIRE
	Syntaxe: SET [!surnom] NOEXPIRE {ON | OFF}
	
	Détermine si oui ou non le surnom donné va expirer.  Utilisé
	avec ON prévient l'expiration du surnom.  Si aucun surnom
	n'est donné, fixe l'option de non-expiration pour votre
	surnom.
	
	Limité à l'usage des Administrateurs des Services.

### CHANGED: new !nick syntax
#NICK_OPER_HELP_UNSET
#	
#	Les Administrateurs des Services peuvent utiliser la
#	commande UNSET sur n'importe quel surnom en incluant le
#	surnom avant le nom de l'option, tel qu'avec la commande
#	SET.

NICK_OPER_HELP_OLD_UNLINK
	
	Les Administrateurs des Services peuvent dé-lier n'importe
	quel surnom sans entrer de mot-de-passe.

NICK_OPER_HELP_UNLINK
	Syntaxe: UNLINK surnom [FORCE]
	
	Enlève un lien (alias) de votre surnom créé par la commande
	LINK. Vous ne pouvez pas enlever le lien sur le surnom que
	vous utilisez actuellement; changez pour un autre surnom lié
	avant d'utiliser cette commande.
	
	Si vous dé-liez le "surnom principal" vous devez utiliser la
	commande SET MAINNICK, votre surnom actuel sera
	automatiquement sélectionné comme surnom principal.
	
	Les Administrateurs des Services peuvent ajouter l'option
	FORCE, laquelle permet de retirer un lien sur un surnom
	autre que le-leur.

NICK_OPER_HELP_OLD_LISTLINKS
	Syntaxe: LISTLINKS surnom [ALL]
	
	Donne le sommaire des surnoms liés au surnom donné.
	
	Limité à l'usage des Administrateurs des Services.

NICK_OPER_HELP_LISTLINKS
	Syntaxe: LISTLINKS [surnom]
	
	Présente tous les surnoms qui sont liés avec le vôtre.  Le
	surnom principal du groupe des surnoms liés sera indiqué par
	un "*" à gauche du surnom.  Les Opérateurs des Services
	peuvent obtenir cette liste pour n'importe quel surnom en
	spécifiant le surnom dans la commande.

NICK_OPER_HELP_ACCESS
	
	Les Administrateurs des Services peuvent donner un paramêtre
	de surnom avec la commande LIST pour obtenir l'affichage de
	la liste d'accès de n'importe quel surnom.

NICK_OPER_HELP_INFO
	
	Les Administrateurs des Services peuvent utiliser le
	paramêtre ALL avec tous les surnoms.

NICK_OPER_HELP_LISTCHANS
	
	Les Administrateurs des Services peuvent avoir la liste des
	canaux pour n'importe quel surnom en spécifiant le surnom
	dans la commande (par exemple, /msg %S LISTCHANS
	AutreSurnom).

#NICK_OPER_HELP_LIST
#	Syntaxe: LIST modèle [FORBIDDEN] [NOEXPIRE] [SUSPENDED]
#	
#	Donne la liste de tous les surnoms enregistrés pour
#	n'importe quel surnom ou la dernière adresse sur lequel il a
#	été apperçu (dans le format usager@hôte) correspondant au
#	modèle donné.  Les surnoms ayant l'option PRIVATE seront
#	seulement présentés aux Administrateurs des Services.  Pour
#	les Administrateurs des Services, les surnoms ayant l'option
#	NOEXPIRE déterminée auront un ! à côté du surnom, et les
#	surnoms suspendus seront accompagnés d'un *.
#	
#	Si les options FORBIDDEN, SUSPENDED ou NOEXPIRE sont
#	spécifiées, seuls les surnoms lesquels, respectivement, sont
#	interdits, suspendus, ou qui ont l'option NOEXPIRE en force
#	seront présentés.  Les options peuvent être utilisées dans
#	n'importe quel ordre et n'importe quelle combinaison. Ces
#	options sont limitées à l'usage des Administrateurs des
#	Services.
#	
#	Exemples:
#	
#	    LIST usagerjoe@foo.com
#	        Présente tous les surnoms enregistrés qui ont été
#	        utilisés pour la dernière fois par usagerjoe@fu.com.
#	
#	    LIST *Bot*
#	        Présente tous les surnoms enregistrés avec Bot dans
#	        leur noms (cas fréquent).
#	
#	    LIST * NOEXPIRE
#	        Présente tous les surnoms qui ont été déterminés
#	        comme n'expirant pas.

#NICK_OPER_HELP_LIST_AUTH
#	Syntaxe: LIST modèle [FORBIDDEN] [NOEXPIRE] [SUSPENDED] [NOAUTH]
#	
#	Donne la liste de tous les surnoms enregistrés qui
#	correspondent au modèle donné, autant avec le surnom
#	lui-même ou avec la dernière adresse apperçue (dans le
#	format usager@hôte).  Les surnoms ayant l'option PRIVATE
#	sélectionnée seront présentés exclusivement aux
#	Administrateurs des Services.  Pour ces derniers, les
#	surnoms ayant l'option NOEXPIRE déterminée auront à côté un
#	!, ceux qui sont suspendus serons précédés d'un *, et ceux
#	qui n'ont pas encore été autorisés (avec la commande AUTH)
#	seront précédés d'un ?.
#	
#	Si les options FORBIDDEN, SUSPENDED, NOEXPIRE, ou NOAUTH
#	sont données, seuls les surnoms qui, respectivement, sont
#	interdits, suspendus, ont l'option NOEXPIRE déterminée, ou
#	qui n'ont pas encore été autorisés seront affichés.  Les
#	options peuvent être utilisées dans n'importe quel ordre ou
#	n'importe quelle combinaison.  Ces options sont limitées à
#	l'usage des Administrateurs des Services.
#	
#	Exemples:
#	
#	    LIST usagerjoe@foo.com
#	        Présente tous les surnoms enregistrés qui ont été
#	        utilisés pour la dernière fois par usagerjoe@foo.com.
#	
#	    LIST *Bot*
#	        Présente tous les surnoms enregistrés avec Bot dans
#	        leur noms (cas fréquent).
#	
#	    LIST * NOEXPIRE
#	        Présente tous les surnoms qui ont été déterminés
#	        comme n'expirant pas.

NICK_OPER_HELP_GETPASS
	Syntaxe: GETPASS surnom
	
	Donne le mot-de-passe pour le surnom donné.  Remarquez que
	chaque fois que cette commande est utilisée, un message
	incluant le nom de la personne qui a donné la commande et le
	surnom pour lequel elle a été utilisé est enregistré et
	expédié sous forme de WALLOPS/GLOBOPS.
	
	Si l'encryption est en fonction, cette commande pourrait ne
	pas être disponible, tout dépendant du type d'encryption
	utilisée.
	
	Limité à l'usage des Administrateurs des Services.

NICK_OPER_HELP_FORBID
	Syntaxe: FORBID surnom
	
	Interdit qu'un surnom soit enregistré ou utilisé par
	quiconque. Peut être annulé en utilisant la commande DROP
	sur le surnom.
	
	Limité à l'usage des Administrateurs des Services.

#NICK_OPER_HELP_SUSPEND
#	Syntaxe: SUSPEND [+expiration] surnom raison
#	
#	Empêche un surnom, ainsi que tous les surnoms comprenant
#	ledit surnom à l'intérieur, d'être utilisés ou qu'on ne s'y
#	identifie.  Un surnom peut être dé-suspendu avec la commande
#	UNSUSPEND.
#	
#	Contrairement à un surnom interdit, un surnom suspendu ne
#	perd pas son information et finira par expirer!
#	
#	Limité à l'usage des Administrateurs des Services.

NICK_OPER_HELP_UNSUSPEND
	Syntaxe: UNSUSPEND surnom
	
	Permet aux gens d'utiliser et de s'identifier pour un surnom
	qui avant était suspendu.
	
	Limité à l'usage des Administrateurs des Services.

NICK_OPER_HELP_AJOIN
	
	Les Administrateurs des Services peuvent obtenir la liste
	d'autojoin pour tous les surnoms avec le format LIST surnom.

NICK_OPER_HELP_SETAUTH
	Syntaxe: SETAUTH surnom
	
	Détermine un nouveau code d'autorisation pour le surnom
	donné.  Le propriétaire du surnom aura à utiliser la
	commande AUTH pour le surnom avant d'être autorisé à
	s'identifier pour celui-ci de nouveau.  Si le surnom est
	présentement utilisé, une notice sera expédiée à l'usager du
	surnom et il n'aura plus les privilèges que lui procurent ce
	surnom lorsqu'il est convenablement identifié.
	
	Limité à l'usage des Administrateurs des Services.

### CHANGED: authorization -> authentication
#NICK_OPER_HELP_GETAUTH
#	Syntaxe: GETAUTH surnom
#	
#	Donne le code d'autorisation associé avec le surnom, s'il en
#	existe un.
#	
#	Limité à l'usage des Administrateurs des Services.

### CHANGED: authorization -> authentication
#NICK_OPER_HELP_CLEARAUTH
#	Syntaxe: CLEARAUTH surnom
#	
#	Efface tout code d'autorisation associé à un surnom,
#	permettant ainsi au surnom d'être normalement utilisé.
#	
#	Limité à l'usage des Administrateurs des Services.


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
	%S vous permet d'enregistrer et de contrôler divers aspects
	d'un canal.  %S peut autant prévenir les usagers malicieux de
	"prendre possession" du canal en limitant QUI est autorisé à
	bénéficier des privilèges d'opérateur.  Tapez /msg %S
	HELP COMMANDS pour une liste des commandes de %S; pour
	utiliser une commande, tapez /msg %S commande, ou
	pour plus d'information sur une commande, tapez /msg
	%S HELP commande.

CHAN_HELP_EXPIRES
	
	Prière de noter que tout canal qui n'est pas utilisé pendant %s
	(i.e. sur lequel aucun usager inscrit à la liste d'accès n'entre
	pour cette période de temps) sera automatiquement fermé.

CHAN_HELP_COMMANDS
	Les commandes suivantes peuvent être utilisées avec %S:
	
	    REGISTER   Enregistrer un canal
	    IDENTIFY   Vous identifier avec votre mot-de-passe
	    DROP       Désenregistrement d'un canal
	    SET        Sélectionne les options et informations d'un
	               canal
	    UNSET      Efface les informations d'un canal
	    INFO       Présente les options et informations d'un
	               canal
CHAN_HELP_COMMANDS_LIST
	    LIST       Sommaire des canaux enregistrés
CHAN_HELP_COMMANDS_AKICK
	    AKICK      Maintien de la liste d'Auto-Expulsion
#CHAN_HELP_COMMANDS_LEVELS
#	    ACCESS     Maintien de la liste de priorité d'un canal
#	    LEVELS     Ajustement précis des niveaux d'accès sur le
#	               canal
CHAN_HELP_COMMANDS_XOP
	    SOP        Maintien de la liste des SuperOps
	    AOP        Maintien de la liste des AutoOps
CHAN_HELP_COMMANDS_HOP
	    HOP        Maintien de la liste des AutoDemiOps
#CHAN_HELP_COMMANDS_XOP_2
#	    VOP        Maintien de la liste des AutoVoices
CHAN_HELP_COMMANDS_OPVOICE
	    OP         Accorder ponctuellement le status d'opérateur
	               à un usager (+o)
	    DEOP       Retirer le status d'opérateur à un usager
	               (-o)
	    VOICE      Accorder un privilège vocal à un usager (+v)
	    DEVOICE    Retirer le privilège vocal à un usager (-v)
CHAN_HELP_COMMANDS_HALFOP
	    HALFOP     Accorder un status demi-Opérateur à un usager
	               (+h)
	    DEHALFOP   Retirer le status demi-Opérateur à un usager
	               (-h)
CHAN_HELP_COMMANDS_PROTECT
	    PROTECT    Accorder à un usager un status de protégé
	               (+a)
	    DEPROTECT  Retirer à un usager un status de protégé (-a)
CHAN_HELP_COMMANDS_INVITE
	    INVITE     Inviter un usager sur un canal
	    UNBAN      Dé-bannir un usager d'un canal
	    KICK       Expulser un usager d'un canal
	    TOPIC      Changer le sujet d'un canal
	    CLEAR      Effacer les modes d'un canal ou expulsion de
	               masse de tous les usagers
	    STATUS     Retourne à un usager le niveau d'accès sur un
	               canal

CHAN_HELP_REGISTER
	Syntaxe: REGISTER canal mot-de-passe description
	
	Enregistre un canal dans la base de données de %S.
	Afin d'utiliser cette commande, vous devez d'abord être
	opérateur sur le canal que vous souhaitez enregistrer.  Le
	mot de passe est utilisé avec la commande IDENTIFY pour
	vous permettre d'apporter des changements aux ajustements du
	canal lors des usages futurs.  Le dernier paramêtre, lequel
	doit être inclus, est une description générale de la raison
	d'être du canal.
	
	Lorsque vous enregistrez un canal, vous êtes inscrit à titre
	de "propriétaire" (founder) du canal.  Le propriétaire du
	canal est habilité à changer tous les ajustements du canal;
	%S accordera automatiquement au propriétaire
	du canal les privilèges d'Opérateur lorsque ce dernier
	entrera sur le canal.
	
	AVIS: Afin d'enregistrer un canal, vous devez d'abord
	enregistrer et vous identifier pour votre surnom.  Si vous
	ne l'avez fait, /msg %s HELP pour de l'information sur
	comment procéder.

CHAN_HELP_REGISTER_ADMINONLY
	
	Limité aux Administrateurs des Services.

CHAN_HELP_IDENTIFY
	Syntaxe: IDENTIFY canal mot-de-passe
	
	Vous authentifie auprès de %S à titre de propriétaire du
	canal donné.  Plusieurs commandes requierrent que vous
	utilisiez cette commande avant de les utiliser.  Le mot de
	passe doit être le même que vous avez donné avec la commande
	REGISTER.

#CHAN_HELP_DROP
#	Syntaxe: DROP canal
#	
#	Désenregistre le canal nommé.  Ne peut être utilisée que par
#	le propriétaire du canal, lequel doit utiliser d'abord la
#	commande IDENTIFY.

CHAN_HELP_SET
	Syntaxe: SET canal option paramêtres
	
	Permet au propriétaire du canal de déterminer différentes
	options de canal et d'autres informations.  Le propriétaire
	doit utiliser la commande IDENTIFY avant d'utiliser la
	commande SET.
	
	Options disponibles:
	
	    FOUNDER     Déterminer le propriétaire du canal
	    SUCCESSOR   Déterminer le successeur du canal
	    PASSWORD    Déterminuer le mot-de-passe du propriétaire
	    DESC        Déterminer la description du canal
	    URL         Associer un URL au canal
	    EMAIL       Associer une adresse courriel au canal
	    ENTRYMSG    Déterminer un message à être expédié aux
	                usagers lorsqu'ils entrent sur le canal
	    KEEPTOPIC   Retenir le sujet du canal lorsque ce dernier
	                n'est pas utilisé
	    TOPICLOCK   Le sujet ne peut être changé qu'avec TOPIC
	    MLOCK       Barrer les modes du canal à ON ou OFF
	    PRIVATE     Voiler le canal de la commande LIST
	    RESTRICTED  Restreindre l'accès au canal
	    SECURE      Activer les dispositifs de protection de %S
	    SECUREOPS   Assurer un contrôle stricte des Opérateurs
	                de canal
	    LEAVEOPS    Ne pas retirer les privilèges d'opérateur à
	                l'entrée sur le canal
	    OPNOTICE    Expédier une notification lorsque les
	                commandes OP/VOICE sont utilisées
	    ENFORCE     Régulation des status auto-op et auto-voice
	
	Tapez /msg %S HELP SET option pour plus d'information
	sur une option particulière.

CHAN_HELP_SET_FOUNDER
	Syntaxe: SET canal FOUNDER surnom
	
	Change le propriétaire d'un canal.  Le nouveau surnom doit
	en être un enregistré.

CHAN_HELP_SET_SUCCESSOR
	Syntaxe: SET canal SUCCESSOR surnom
	
	Change le successeur d'un canal.  Si le surnom du
	propriétaire expire ou s'il est désenregistré alors que le
	canal continue d'être enregistré, le successeur deviendra le
	nouveau propriétaire du canal.  Toutefois, si le successeur
	a trop de canaux enregistrés (%d), le canal sera
	désenregistré à la place, tout comme si aucun successeur
	n'avait été déterminé.  Le surnom doit être enregistré.

CHAN_HELP_SET_PASSWORD
	Syntaxe: SET canal PASSWORD mot-de-passe
	
	Détermine le mot-de-passe utilisé pour identifier le
	propriétaire du canal.

CHAN_HELP_SET_DESC
	Syntaxe: SET canal DESC description
	
	Détermine la description du canal, laquelle est présenté
	avec les commandes LIST et INFO.

CHAN_HELP_SET_URL
	Syntaxe: SET canal URL url
	
	Associe l'URL donné avec le canal.  Cet URL sera présenté
	chaque fois que quelqu'un requiert de l'information sur le
	canal avec la commande INFO.

CHAN_HELP_SET_EMAIL
	Syntaxe: SET canal EMAIL adresse
	
	Associe l'adresse courriel donnée avec le canal.  Cette
	adresse sera présentée chaque fois que quelqu'un requiert de
	l'information sur le canal avec la commande INFO.

CHAN_HELP_SET_ENTRYMSG
	Syntaxe: SET canal ENTRYMSG message
	
	Détermine le message qui sera envoyé via /NOTICE aux usagers
	lorsqu'ils entreront sur le canal.

CHAN_HELP_SET_KEEPTOPIC
	Syntaxe: SET canal KEEPTOPIC {ON | OFF}
	
	Active ou désactive l'option de rétention de sujet pour un
	canal. Lorsque la rétention de sujet est sélectionné, le
	sujet du canal sera retenue par %S même après que le
	dernier usager ait quitté le canal, et sera remis la
	prochaine fois que quelqu'un embarqueras sur le canal.

CHAN_HELP_SET_TOPICLOCK
	Syntaxe: SET canal TOPICLOCK {ON | OFF}
	
	Active et désactive l'option de barrure de sujet pour un
	canal. Lorsque la barrure de sujet est sélectionné,
	%S n'autorisera pas le changement de sujet, excepté
	par la commande TOPIC.

CHAN_HELP_SET_MLOCK
	Syntaxe: SET canal MLOCK modes
	
	Détermine les paramêtres mode-barrés pour un canal. %S
	permet de définir certains modes de canal à être toujours ON
	(ou OFF).  Tous les modes binaires (n, s, t, k, l, ainsi
	qu'ON) peuvent être barrés; chaque mode peut être barré ON,
	barré OFF, ou ne pas être barrés.  Le paramêtre modes est
	construit exactement de la même manière que la commande
	/MODE; pour ce, les modes précédés d'un "+" sont barrés
	"ON", et les modes précédés d'un "-" sont barrés OFF.
	
	Attention:  Si vous déterminez une clef de mode-barré, tel
	que dans le second exemple ci-dessous, vous devrez également
	sélectionner l'option RESTRICTED pour le canal (voir HELP
	SET RESTRICTED), sans quoi quiconque entrera sur le canal
	alors qu'il est vide pourra voir la clef!
	
	Exemples:
	
	    SET #channel MLOCK +nt-ikl
	        Force les modes n et t à ON, et les modes i, k, et l
	        à OFF. Tous les autres modes sont libres d'être mis
	        à ON ou OFF.
	
	    SET #channel MLOCK +knst-ilmp ma-clef
	        Force les modes k, n, s, et t à ON, et les modes i,
	        l, m, et p à OFF.  Force également la clef du canal
	        à être "ma-clef".
	
	    SET #channel MLOCK +
	        Enlève le mode de barrage; tous les modes de canal
	        sont libres d'être à ON ou à OFF.

CHAN_HELP_SET_HIDE
	Syntaxe: SET canal HIDE {EMAIL | TOPIC | MLOCK} {ON | OFF}
	
	Vous permet de prévenir que certaines pièces d'information
	soient présentées lorsque quelqu'un fait un %S INFO
	sur le canal donné. Vous pouvez voiler l'adresse courriel du
	canal (EMAIL), le dernier sujet (TOPIC), et le mode de
	barrage (MLOCK).  Le second paramêtre spécifie quelle
	information peut être présentée (OFF) ou masquée (ON).
	
	Remarquez que le sujet sera toujours caché si le canal est
	en mode-barré +p ou +s.

CHAN_HELP_SET_PRIVATE
	Syntaxe: SET canal PRIVATE {ON | OFF}
	
	Active ou désactive l'option private pour un canal.
	Lorsque private est sélectionnée, un /msg %S LIST
	n'inclura pas le canal dans toutes les listes.

CHAN_HELP_SET_RESTRICTED
	Syntaxe: SET canal RESTRICTED {ON | OFF}
	
	Active ou désactive l'option d'accès restreint pour un
	canal.  Lorsque restricted access est sélectionné, les
	usagers qui n'apparaissent pas sur la liste d'accès ou qui
	normalement ne disposeraient pas des privilèges d'opérateur
	seront expulsés et bannis du canal.

#CHAN_HELP_SET_SECURE
#	Syntaxe: SET canal SECURE {ON | OFF}
#	
#	Active ou désactive les dispositifs de sécurité de %S
#	pour un canal.  Lorsque SECURE est sélectionné, seuls les
#	usagers qui ont enregistrés leurs surnoms avec %s et qui se
#	sont identifiés avec leur mot-de-passe pourront recevoir les
#	privilèges sur le canal tel que contrôlé par la liste
#	d'accès.

#CHAN_HELP_SET_SECUREOPS
#	Syntaxe: SET canal SECUREOPS {ON | OFF}
#	
#	Active ou désactive l'option secure ops pour un canal.
#	Lorsque secure ops est sélectionné, les usagers qui
#	n'apparaissent pas sur la liste d'opérateurs ne pourront pas
#	recevoir ce privilège.

#CHAN_HELP_SET_LEAVEOPS
#	Syntaxe: SET canal LEAVEOPS {ON | OFF}
#	
#	Active ou désactive l'option leave ops pour un canal.
#	Lorsque leave ops est sélectionné, %S laissera le
#	status d'opérateur (+o) à la première personne qui entrera
#	sur le canal, même si cet usager ne serait normalement pas
#	auto-opé.  L'option SECUREOPS et le niveau d'accès AUTODEOP
#	seront renforcées. Remarque:  Ceci permet à un usager de
#	"voler" les privilèges d'opérateur pendant une rupture de
#	réseau (netsplit).

CHAN_HELP_SET_OPNOTICE
	Syntaxe: SET canal OPNOTICE {ON | OFF}
	
	Active ou désactive l'option de notice d'opérateur pour un
	canal.  Lorsque op-notice est sélectionné, %S expédit
	une notification au canal lors de chaque commande OP ou DEOP
	utilisée pour un usager dans le canal.

CHAN_HELP_SET_ENFORCE
	Syntaxe: SET canal ENFORCE {ON | OFF}
	
	Active ou désactive l'option de régulation enforce pour un
	canal.  Lorsque enforce est sélectionné, %S renforcera
	les niveaux AutoOps et AutoVoice pour tous les usagers.  Par
	exemple, si un usager avec les privilèges d'AutoOp est
	déoppé, %S le ré-opera automatiquement, et %S
	n'autorisera pas non-plus que la commande DEOP soit utilisée
	sur cet usager.

CHAN_HELP_UNSET
	Syntaxe: UNSET canal {SUCCESSOR | URL | EMAIL | ENTRYMSG}
	
	Efface la sélection donnée pour un canal.

### CHANGED: new LIST syntax
#CHAN_HELP_SOP
#	Syntaxe: SOP canal ADD surnom
#	         SOP canal DEL surnom
#	         SOP canal LIST [masque]
#	         SOP canal COUNT
#	
CHAN_HELP_SOP_MID1
	Maintiens la liste des SuperOps (SOP) pour un canal.  Les
	usagers avec un status SOP obtiennent automatiquement les
	privilèges d'opérateur de canal (mode +o) lorsqu'ils entrent
	sur le canal.
CHAN_HELP_SOP_MID1_CHANPROT
	
	Maintiens la liste des SuperOps (SOP) pour un canal.  Les
	usagers avec un status SOP sont automatiquement Opés (mode
	+o) et protégés (mode +a) lorsqu'ils joignent le canal.
CHAN_HELP_SOP_MID2
	
	Un SOP peut modifier les listes des AOP, VOP, et NOP.  Seul
	un propriétaire de canal peut modifier la liste des SOP.
CHAN_HELP_SOP_MID2_HALFOP
	
	Un SOP peut modifier les listes des AOP, HOP, VOP, et NOP.
	Seul un propriétaire de canal peut modifier la liste des SOP.
### CHANGED: completely rewritten
#CHAN_HELP_SOP_END
#	
#	La commande SOP LIST présente la liste des SOP.  Si un
#	masque frimé est donné, seuls les entrées correspondants
#	audit masque seront présentées. Si une liste d'entrées
#	numériques est donnée, seules ces entrées seront exposées;
#	par exemple:
#	
#	   SOP #canal LIST 2-5,7-9
#	      Présentations des entrées à la Liste SOP numérotées 2
#	      à 5 et 7 à 9.
#	
#	La commande SOP COUNT retourne le nombre d'entrées présentes
#	dans la liste des SOP.

### CHANGED: new LIST syntax
#CHAN_HELP_AOP
#	Syntaxe: AOP canal ADD surnom
#	         AOP canal DEL surnom
#	         AOP canal LIST [masque]
#	         AOP canal COUNT
#	
#	Maintiens la liste des AutoOps (AOP) pour un canal.  Les
#	usagers avec un status AOP sont automatiquement opés (mode
#	+o) lorsqu'ils joignent le canal.
CHAN_HELP_AOP_MID
	
	Un AOP peut modifier les listes des VOP et NOP; les SOPs
	peuvent modifier la liste des AOP.
CHAN_HELP_AOP_MID_HALFOP
	
	Un AOP peut modifier les listes des HOP, VOP, et NOP; les
	SOPs peuvent modifier la liste des AOP.
### CHANGED: completely rewritten
#CHAN_HELP_AOP_END
#	
#	La commande AOP LIST présente la liste des AOP.  Si un
#	masque frimé est donné, seuls les entrées correspondantes
#	audit masque seront présentées. Si une liste d'entrées
#	numériques est donnée, seules les entrées correspondantes
#	seront affichées; par exemple:
#	
#	   AOP #canal LIST 2-5,7-9
#	      Présente les entrées de la liste des AOP numérotées 2
#	      à 5 et 7 à 9.
#	
#	La commande AOP COUNT retourne le nombre d'entrées présentes
#	dans la liste des AOP.

### CHANGED: completely rewritten
#CHAN_HELP_HOP
#	Syntaxe: HOP canal ADD surnom
#	         HOP canal DEL surnom
#	         HOP canal LIST [masque]
#	         HOP canal COUNT
#	
#	Maintiens la liste des HalfOp (HOP) pour un canal.  Les
#	usagers disposant d'un status HOP sont automatiquement
#	demi-opés (mode +h) lorsqu'ils joignent le canal.
#	
#	Un HOP peut modifier les listes des VOP et NOP; les SOPs et
#	AOPs peuvent modifier la liste des HOP.
#	
#	La commande HOP LIST présente la liste des HOP.  Si un
#	masque frimé est donné, seules les entrées correspondantes
#	audit masque seront présentées. Si une liste d'entrées
#	numériques est donnée, seules ces entrées seront présentées;
#	par exemple:
#	
#	   HOP #canal LIST 2-5,7-9
#	      Présente les entrées de la liste des HOP numérotées de
#	      2 à 5 et de 7 à 9.
#	
#	La commande HOP COUNT donne le nombre d'entrées présentes
#	sur la liste des HOP.

### CHANGED: completely rewritten
#CHAN_HELP_VOP
#	Syntaxe: VOP canal ADD surnom
#	         VOP canal DEL surnom
#	         VOP canal LIST [masque]
#	         VOP canal COUNT
#	
#	Maintiens la liste des VoiceOp (VOP) pour un canal.  Les
#	usagers ayant un status VOP obtiendront automatiquement les
#	privilèges vocaux (mode +v) lorsqu'ils joindront le canal.
#	
#	La commande VOP LIST affiche la liste des VOP.  Si un masque
#	frimé est donné, seules les entrées correspondantes audit
#	masque seront présentées.  Si une liste d'entrées numériques
#	est donnée, seules ces entrées seront présentées; par
#	exemple:
#	
#	   VOP #canal LIST 2-5,7-9
#	      Présente les entrées de la liste des VOP numérotées de
#	      2 à 5 et de 7 à 9.
#	
#	La commande VOP COUNT donne le nombre d'entrées présentes
#	sur la liste des HOP.

### CHANGED
#CHAN_HELP_ACCESS
#	Syntaxe: ACCESS canal ADD surnom niveau
#	         ACCESS canal DEL {surnom | entrée-num | liste}
#	         ACCESS canal LIST [masque | liste]
#	         ACCESS canal COUNT
#	
#	Maintiens la liste d'accès pour un canal.  La liste d'accès
#	spécifie quels usagers sont autorisés ont le status
#	d'opérateur de canal ou ont accès aux commandes de %S
#	sur le canal.  Différents niveaux d'accès permettent d'avoir
#	des sous groupes de privilèges; /msg %S HELP ACCESS
#	LEVELS pour plus d'information spécifique. Un surnom
#	n'apparaissant pas à la liste a un niveau d'usager de 0.
#	
#	La commande ACCESS ADD ajoute le surnom donné à la liste
#	d'accès avec le niveau d'usager spécifié; si le surnom est
#	déjà présent dans la liste, son niveau d'accès sera changé
#	au niveau spécifié dans la commande.  Le niveau spécifié
#	doit être inférieur à celui de l'usager qui donne la
#	commande, et si le surnom apparait déjà sur la liste, le
#	niveau actuel de l'accès de ce surnom doit être inférieur à
#	celui de l'usager qui entre la commande.
#	
#	La commande ACCESS DEL retire le surnom donné de la liste
#	d'accès.  Si une liste d'entrées numériques est donnée,
#	celles-ce seront effacées. (voir l'exemple pour LIST
#	ci-après.)
#	
#	La commande ACCESS LIST présente la liste des accès. Si un
#	masque frimé est donné, seules les entrées y correspondant
#	seront présentées.  Si une liste d'entrées numériques est
#	donnée, seules ces entrées seront présentées; par exemple:
#	
#	   ACCESS #canal LIST 2-5,7-9
#	      Présente les entrées de la liste numérotées de 2 à 5
#	      et de 7 à 9.
#	
#	
#	La commande ACCESS COUNT donne le nombre d'entrées présentes
#	sur la liste d'accès.
CHAN_HELP_ACCESS_XOP
	
	Notez que la liste d'accès peut également être manipulée
	avec les commandes SOP, AOP, VOP, et NOP, lesquels
	correspondent aux niveaux de %d, %d, %d, et %d
	respectivement.
CHAN_HELP_ACCESS_XOP_HALFOP
	
	Notez que la liste d'accès peut aussi être manipulée avec
	les commandes SOP, AOP, HOP, VOP, et NOP, lesquelles
	correspondent aux niveaux de %d, %d, %d, %d, et %d
	respectivement.

#CHAN_HELP_ACCESS_LEVELS
#	Niveaux d'accès des usagers
#	
#	Par défaut, les niveaux d'accès suivants sont définis:
#	
#	   Propriétaire   Plein accès aux fonctions de %S;
#	                    automatique
#	                      status d'opérateur dès l'entrée sur le canal.  Notez que
#	                      seulement une personne peut avoir le status de propriétaire
#	                      (ça ne peut pas être donné en utilisant la commande
#	                      ACCESS).
#	            %4d   Accès à la commande AKICK; opérateur automatique.
#	            %4d   Opérateur automatique.
CHAN_HELP_ACCESS_LEVELS_HALFOP
	            %4d   Demi-opérateur automatique.
CHAN_HELP_ACCESS_LEVELS_END
	            %4d   Status vocal automatique.
	              0   Sans privilèges spécifiques; peut être opé par les autres
	                      opérateurs (sauf si SECUREOPS est sélectionné).
	             -1   Ne peut pas être opérateur.
	           -100   Ne peut pas joindre le canal.
	
	Ces niveaux (à l'exception des deux derniers) peuvent être
	changés, ou de nouveaux ajoutésm en utilisant la commande
	LEVELS; tapez /msg %S HELP LEVELS pour les détails.

CHAN_HELP_LEVELS
	Syntaxe: LEVELS canal SET type niveau
	         LEVELS canal {DIS | DISABLE} type
	         LEVELS canal LIST
	         LEVELS canal RESET
	
	La commande LEVELS permet un contrôle plus rafiné que la
	signification des niveaux numériques d'accès utilisés dans
	la commande ACCESS.  Avec cette commande, vous pouvez
	définir le niveau d'accès requis pour la plupart des
	fonctions de %S.  (Les commandes SET FOUNDER et SET
	PASSWORD, ainsi que cette commande, sont toujours
	restreintes à l'usage du propriétaire du canal.)
CHAN_HELP_LEVELS_XOP
	
	Attention:  Cette commande est pour les usagers avancés
	seulement!  Changer le niveau d'ajustement avec cette
	commande peut empêcher les commandes SOP, AOP, VOP, et NOP
	de fonctionner correctement.
CHAN_HELP_LEVELS_XOP_HOP
	
	Attention:  Cette commande est pour les usagers avancés
	seulement!  Changer le niveau d'ajustement avec cette
	commande peut empêcher les commandes SOP, AOP, HOP, VOP, et
	NOP de fonctionner correctement.
CHAN_HELP_LEVELS_END
	
	LEVELS SET permet de changer le niveau d'accès pour une
	fonction ou un groupe de fonctions.  LEVELS DISABLE (ou le
	racourci DIS) désactive une composante ou rend impossible
	d'accéder à une fonction par quiconque autre que le
	propriétaire du canal.  LEVELS LIST présente le niveau
	actuel requis pour chaque fonction ou groupe de fonctions.
	LEVELS RESET remet à leur position d'origine les niveaux
	d'un canal nouvellement-créé (voir HELP ACCESS LEVELS).
	
	Pour une liste des accessoires et fonctions pour lesquels
	des niveaux peuvent être déterminés, voir HELP LEVELS DESC.

CHAN_HELP_LEVELS_DESC
	Les noms d'accessoires/fonctions suivants sont compris:

### CHANGED: various
#CHAN_HELP_AKICK
#	Syntaxe: AKICK canal ADD masque [raison]
#	         AKICK canal DEL masque
#	         AKICK canal LIST [masque]
#	         AKICK canal VIEW [masque]
#	         AKICK canal ENFORCE
#	         AKICK canal COUNT
#	
#	Maintien la liste d'auto-expulsion pour un canal.  Si un
#	usager inscrit à la liste d'auto-expulsion (autokick) tente
#	de joindre le canal, %S bannira cet usager du canal,
#	ensuite expulsera l'usager.
#	
#	La commande AKICK ADD ajoute l'usager@hôte ou
#	surnom!usager@hôte masque à la liste d'autokick.  Si une
#	raison est donnée avec la commande, cette raison sera
#	utilisée lorsque l'usager sera expulsé afin de justifier
#	cette action; si aucune raison n'est donnée, la raison par
#	défaut suivante "Vous avez été banni de ce canal" sera
#	utilisée.
#	
#	La commande AKICK DEL retire le masque donné de la liste
#	d'autoexpulsion (autokick).  Ça ne retire toutefois pas,
#	tous les bans placés par un auto-expulsion; ces derniers
#	doivent être retirés manuellement.
#	
#	La commande AKICK LIST présente la liste d'auto-expulsion,
#	ou optionnellement seulement les entrées "autokick"
#	correspondant au masque donné avec la commande. AKICK VIEW
#	est similaire, mais présente plus de détails.
#	
#	La commande AKICK COUNT retourne le nombre total d'entrées
#	inscrites actuellement sur la liste d'auto-expulsion du
#	canal.
#	
#	La commande AKICK ENFORCE force %S à régulariser
#	l'entrée en vigueur de l'actuelle liste d'auto-expulsion en
#	retirant les usagers qui correspondent avec un masque
#	d'auto-expulsion donné.
#	
#	Par défaut, limité aux usagers ayant accès avec %s qui
#	sont présent actuellement sur le canal.

CHAN_HELP_INFO
	Syntaxe: INFO canal [ALL]
	
	Présente l'information disponible à propos du nom de canal
	enregistré donné, incluant qui est le propriétaire, le
	moment de l'enregistrement, sa dernière utilisation, la
	description, les modes barrés, s'il y en a.  Si vous êtes
	identifiés comme étant le propriétaire dudit canal, vous
	recevrez TOUTE l'information, si ALL est spécifié, le
	message d'entrée et le successeur seront égalelement
	présentés.

#CHAN_HELP_LIST
#	Syntaxe: LIST modèle
#	
#	Présente tous canaux enregistrés correspondant au modèle
#	donné. (Les canaux avec l'option PRIVATE sélectionnée ne
#	seront pas présentés.)
CHAN_HELP_LIST_OPERSONLY
	
	Seuls les Opérateurs d'IRC peuvent utiliser cette commande.

CHAN_HELP_OP
	Syntaxe: OP canal [surnom]
	
	Accorde le status d'opérateur au surnom sélectionné sur un
	canal.  Si aucun surnom n'est donné, vous accordera le
	status d'opérateur.  Par défaut, limité aux usagers ayant
	accès auprès de %s qui sont présent sur le canal.

CHAN_HELP_DEOP
	Syntaxe: DEOP canal [surnom]
	
	Retire le status d'opérateur au surnom sélectionné sur le
	canal doné.  Si aucun surnom n'est donné, vous retire ce
	status.  Par défaut, limité aux usagers ayant accès auprès
	de %s présent sur le canal.

CHAN_HELP_VOICE
	Syntaxe: VOICE canal [surnom]
	
	Accorde les privilèges vocaux au surnom sélectionné sur le
	canal.  Si aucun surnom n'est donné, vous accorde ce status
	(+v).  Par défaut, limité aux usagers ayant accès auprès de
	%s présent sur le canal.

CHAN_HELP_DEVOICE
	Syntaxe: DEVOICE canal [surnom]
	
	Retire le privilège vocal au surnom donné sur le canal.  Si
	aucun surnom n'est donné, vous retire le privilège.  Par
	défaut, limité aux usagers ayant accès auprès de %s
	présent sur le canal.

CHAN_HELP_HALFOP
	Syntaxe: HALFOP canal [surnom]
	
	Accorde un accès demi-opérateur au surnom donné sur le
	canal.  Si aucun surnom n'est donné, vous accorde cet accès
	demi-opérateur.  Par défaut, limité aux usagers ayant accès
	auprès de %s présent sur le canal.

CHAN_HELP_DEHALFOP
	Syntaxe: DEHALFOP canal [surnom]
	
	Retire l'accès demi-opérateur du surnom sélectionné sur le
	canal.  Si aucun surnom n'est donné, vous retire l'accès
	demi-opérateur.  Par défaut, limité aux usagers ayant accès
	auprès de %s présent sur le canal.
	
CHAN_HELP_PROTECT
	Syntaxe: PROTECT canal [surnom]
	
	Accorde une protection de canal (mode+a) au surnom
	sélectionné sur le canal. Si aucun surnom n'est donné, vous
	accorde cette protection de canal.  Par défaut, limité aux
	usagers ayant accès auprès de %s présent sur le canal.

CHAN_HELP_DEPROTECT
	Syntaxe: DEPROTECT canal [surnom]
	
	Retire une protection de canal (mode+a) au surnom
	sélectionné sur le canal.  Si aucun surnom n'est donné, vous
	retire cette protection de canal.  Par défaut, limité aux
	usagers ayant accès auprès de %s présent sur le canal.

CHAN_HELP_INVITE
	Syntaxe: INVITE canal
	
	Dit à %S de vous inviter sur le canal donné.  Par
	défaut, limité aux usagers ayant accès auprès de %s présent
	sur le canal.
	
	Remarque: Si vous recevez un message vous avisant que le
	canal n'est pas utilisé, cela signifie que le canal est
	vide, et vous pouvez le joindre librement, sans utiliser
	cette commande.

CHAN_HELP_UNBAN
	Syntaxe: UNBAN canal
	
	Demande à %S de retirer tous les bans vous empêchant
	d'entrer sur le canal donné.  Par défaut, limité aux usagers
	ayant accès auprès de %s inscrits sur le canal.

CHAN_HELP_KICK
	Syntaxe: KICK canal surnom [raison]
	
	Demande à %S d'explulser le surnom donné du canal
	donné.  Le message compris avec l'expulsion présentera qui a
	donné la commande KICK, ainsi que la raison donnée avec la
	commande, s'il y en a une.  Par défaut, limité aux usagers
	ayant accès avec %s inscrits sur le canal.

CHAN_HELP_KICK_PROTECTED
	
	Remarquez qu'un usager protégé (+a) ne peut pas être expulsé
	du canal avec cette commande, même par le propriétaire du
	canal.

CHAN_HELP_TOPIC
	Syntaxe: TOPIC canal le sujet désiré
	
	Demande à %S de remplacer le sujet du canal pour celui
	spécifié.  Cette commande est plus utilisée conjointement
	avec SET TOPICLOCK. Par défaut, limité aux usagers ayant
	accès à %s inscrits pour le canal.

CHAN_HELP_CLEAR
	Syntaxe: CLEAR canal quoi
	
	Demande à %S d'effacer certains ajustements sur un
	canal.  Quoi peut être un des éléments suivants:
	
	    MODES       Efface tous les modes sur le canal à
	                    l'exception de ceux sélectionné dans le
	                    mode-barré du canal.
	    BANS        Efface tous les bans du canal.
CHAN_HELP_CLEAR_EXCEPTIONS
	    EXCEPTIONS  Efface toutes les exceptions du canal.
CHAN_HELP_CLEAR_MID
	    OPS         Retire le status d'opérateur-canal
	                    (mode +o) de tous les usagers.
CHAN_HELP_CLEAR_HALFOPS
	    HALFOPS     Retire le status demi-opérateur (mode +h) à
	                    tous les usagers.
CHAN_HELP_CLEAR_END
	    VOICES      Retire le status "vocal" (mode +v) à tous
	                    les usagers.
	    USERS       Retire (expulse) tous les usagers du canal.
	
	Par défaut, limité aux usagers ayant accès auprès de
	%s et qui sont présent sur le canal.

CHAN_HELP_STATUS
	Syntaxe: STATUS canal surnom
	
	Retourne l'actuel niveau d'accès pour un surnom donné sur le
	canal donné.  La réponse prend la forme suivante:
	
	    STATUS canal surnom niveau-d'accès
	
	Si une erreur survient, la réponse prendra la forme
	suivante:
	
	    STATUS canal surnom ERROR message-d'erreur
	
	Par défaut, limité aux usagers ayant accès auprès de
	%s et qui sont présent sur le canal.

#CHAN_OPER_HELP_COMMANDS
#	
#	Les commandes suivantes sont réservées aux Administrateurs
#	des services:
#	
CHAN_OPER_HELP_COMMANDS_GETPASS
	    GETPASS    Obtenir le mot-de-passe du propriétaire d'un
	                   canal (seulement si l'encryption n'est
	                   pas en cours)
CHAN_OPER_HELP_COMMANDS_FORBID
	    FORBID     Empêche un canal d'être enregistré/utilisé
	    SUSPEND    Empêche un canal enregistré d'être utilisé
	    UNSUSPEND  Annule une suspension de canal
#CHAN_OPER_HELP_COMMANDS_END
#	
#	Les Administrateurs des Services peuvent également annuler
#	l'enregistrement de n'importe quel canal sans avoir à
#	s'identifier par mot-de-passe, et peuvent voir les listes
#	d'accès, d'AKICK, ainsi que la liste des ajustements ou
#	utiliser la commande STATUS pour n'importe quel canal.

CHAN_OPER_HELP_SET
	
	Les Administrateurs des Services peuvent également
	sélectionner l'option NOEXPIRE, laquelle permet de prévenir
	l'expiration d'un canal.  En plus, les Administrateurs des
	Services peuvent sélectionner les options pour n'importe
	quel canal sans avoir à s'identifier par mot-de-passe pour
	le canal.

CHAN_OPER_HELP_SET_NOEXPIRE
	Syntaxe: SET canal NOEXPIRE {ON | OFF}
	
	Détermine si le canal donné doit expirer.  Déterminer cet
	ajustement à ON prévient l'expiration du canal.
	
	Limité à l'usage des Administrateurs des Services.

CHAN_OPER_HELP_UNSET
	
	Comme avec la commande SET, les Administrateurs des Services
	peuvent effacer les ajustements pour n'importe quel canal
	sans s'identifier pour le canal.

CHAN_OPER_HELP_INFO
	
	Les Administrateurs des Services peuvent utiliser le
	paramêtre ALL avec tous les canaux.

#CHAN_OPER_HELP_LIST
#	Syntaxe: LIST modèle [FORBIDDEN] [NOEXPIRE] [SUSPENDED]
#	
#	Donne le sommaire de tous les canaux correspondant au modèle
#	donné.  Les canaux ayant l'option PRIVATE déterminée
#	apparaitront seulement aux Administrateurs des Services.
#	Les canaux ayant l'option NOEXPIRE sélectionnée seront
#	précédés d'un ! avant leur nom pour les Administrateurs
#	des Services.
#	
#	Si les options FORBIDDEN, SUSPENDED ou NOEXPIRE sont
#	données, seuls les canaux qui, respectivement, sont
#	interdits, suspendus ou qui ont l'option NOEXPIRE en vigueur
#	seront présentés.  Les options peuvent être utilisées dans
#	n'importe quel ordre et dans n'importe quelle combinaison.
#	Ces options sont limitées à l'usage des Administrateurs des
#	Services.

CHAN_OPER_HELP_GETPASS
	Syntaxe: GETPASS canal
	
	Donne le mot-de-passe pour un canal donné.  Remarquez que
	chaque fois que cette commande est utiliseé, un message
	incluant le nom de la personne qui a fait la commande et le
	canal pour lequel elle a été utilisée sera sauvé et expédié
	sous forme de WALLOPS/GLOBOPS.
	
	Si l'encryption est en utilisation, cette commande risque de
	ne pas être disponible, dépendamment du type d'encryption
	utilisé.
	
	Limité à l'usage des Administrateurs des Services.

CHAN_OPER_HELP_FORBID
	Syntaxe: FORBID canal
	
	Interdit à quiconque d'enregistrer ou utiliser le canal
	donné.  Peut être annulé en désenregistrant (commande DROP)
	le canal.
	
	Limité à l'usage des Administrateurs des Services.

#CHAN_OPER_HELP_SUSPEND
#	Syntaxe: SUSPEND [+expiration] canal raison
#	
#	Interdit l'utilisation et l'identification à un canal par
#	quiconque.  Un canal suspendu peut être dé-suspendu avec la
#	commande UNSUSPEND.
#	
#	Contrairement à un canal interdit, un canal suspendu ne perd
#	pas ses information et fini par expirer!
#	
#	Limité à l'usage des Administrateurs des Services.

CHAN_OPER_HELP_UNSUSPEND
	Syntaxe: UNSUSPEND canal
	
	Permet aux usager d'utiliser et de s'identifier pour un
	canal qui avant était suspendu.
	
	Limité à l'usage des Administrateurs des Services.


###########################################################################
#
# MemoServ help messages
#
###########################################################################

MEMO_HELP
	%S est un utilitaire permettant aux usagers IRC
	d'expédier de courts messages aux autres usagers IRC ou au
	canaux (*), même s'ils ne sont pas en-ligne au moment de
	l'expédition.  Les surnoms tant de l'expéditeur que de la
	cible ou le canal doivent être enregistrés afin que le
	message puisse être expédié et délivré.  Tapez /msg %S
	HELP COMMANDS pour la liste des commandes de %S; pour
	utiliser une commande, tapez /msg %S commande, ou pour
	plus d'information sur une commande, tapez /msg %S
	HELP commande.
MEMO_HELP_EXPIRES
	
	Remarque:  Les messages expireront (seront automatiquement
	effacés) %s après avoir été expédiés, s'ils ne sont
	pas spécifiés n'expirant-pas avec la commande SAVE.
#MEMO_HELP_END_LEVELS
#	
#	(*) Par défaut, chaque usager avec un accès minimum de
#	    %s sur un canal peut lire les messages adressés au
#	    canal.  Ceci peut être changé avec la commande %s
#	    LEVELS.

MEMO_HELP_COMMANDS
	Les commandes suivantes peuvent être utilisées avec %S:
	
	    SEND      Expédier un message à un usager ou un canal
	    LIST      Sommaire de vos messages
	    READ      Lire un ou plusieurs messages
MEMO_HELP_COMMANDS_FORWARD
	    FORWARD   Faire suivre vos messages à votre adresse
	                  courriel
MEMO_HELP_COMMANDS_SAVE
	    SAVE      Spécifier un message comme n'expirant-pas
#MEMO_HELP_COMMANDS_DEL
#	    DEL       Effacer un ou plusieurs message(s)
#	    SET       Déterminer une option relative aux messages
#	    INFO      Présenter l'information relative aux messages
MEMO_HELP_COMMANDS_IGNORE
	    IGNORE    Ignorer les messages provenant de certains
	                  usagers

#MEMO_HELP_SEND
#	Syntaxe: SEND {surnom | canal} texte du message
#	
#	Expédie au surnom ou canal nommé un message contenant le
#	texte du message.  Lorsque vous expédiez à un surnom,
#	l'usager recevra une notification qu'il/elle a reçu un
#	nouveau message.  La cible (surnom/canal) doit être
#	enregistrée.

#MEMO_HELP_LIST
#	Syntaxe: LIST [num | liste | NEW]
#	
#	Sommaire de tous les messages que vous avez actuellement.
#	Avec NEW, présente seulement vos messages nouveaux
#	(non-lus).  Les messages non-lus sont marqués avec un "*" à
#	gauche du numéro de message.  Vous pouvez également
#	spécifier une liste de numéro, tel que dans l'exemple
#	suivant:
#	
#	   LIST 2-5,7-9
#	      Présente les messages numérotés 2 à 5 et 7 à 9.

#MEMO_HELP_LIST_EXPIRE
#	Syntaxe: LIST [num | liste | NEW]
#	
#	Sommaire de tous les messages que vous avez actuellement.
#	Avec NEW, présente seulement les nouveaux messages
#	(non-lus).  Les messages non-lus sont marqués d'un "*" à
#	gauche de leur numéro; les messages n'expirant-pas sont
#	marqués avec un "+".  Vous pouvez également spécifier une
#	liste de numéros comme dans l'exemple suivant:
#	
#	   LIST 2-5,7-9
#	      Présente les messages numérotés 2 à 5 et 7 à 9.

MEMO_HELP_READ
	Syntaxe: READ {num | liste | LAST | NEW}
	
	Vous envois le texte du ou des message(s) spécifié(s).  Si
	vous spécifiez LAST, vous envois le texte du message le plus
	récent.  Si vous spécifiez NEW, vous envois tous vos
	nouveaux messages.  Autrement, vous envois le message
	correspondant au numéro num.  Vous pouvez également donner
	une liste de numéros, tel que dans la commande LIST.

MEMO_HELP_SAVE
	Syntaxe: SAVE {num | liste}
	
	Marque le(s) numéro(s) de message comme n'expirant pas.
	Vous pouvez donner autant un simple numéro de message, ou
	comme dans la commande LIST, une liste de numéros de
	messages.

#MEMO_HELP_DEL
#	Syntaxe: DEL {num | liste | ALL}
#	
#	Efface le(s) message(s) spécifié(S).  Vous pouvez également
#	fournir plusieurs numéros de messages ou un gamme de numéros
#	au lieu d'un simple numéro, tel que dans le second exemple
#	ci-bas.
#	
#	Si vous donnez ALL, efface tous vos messages.
#	
#	Exemples:
#	
#	   DEL 1
#	      Efface votre premier message.
#	
#	   DEL 2-5,7-9
#	      Efface les messages numérotés 2 à 5 et 7 à 9.

MEMO_HELP_SET
	Syntaxe: SET option paramêtres
	
	Détermine divers options de message.  L'option peut être
	une des suivantes:
	
	    NOTIFY      Change le moment où vous serez notifiés de
	                    la réception de vos nouveaux messages
	                    (seulement pour les surnoms)
	    LIMIT       Détermine le nombre maximum de message que
	                    vous pouvez recevoir
MEMO_HELP_SET_OPTION_FORWARD
	    FORWARD     Détermine que tous vos messages vous seront
	                    automatiquement expédiés (sur votre
	                    adresse courriel)
MEMO_HELP_SET_END
	
	Tapez /msg %S HELP SET option pour plus d'information
	sur une option spécifique.

MEMO_HELP_SET_NOTIFY
	Syntaxe: SET NOTIFY {ON | LOGON | NEW | OFF}
	
	Modifie le moment où vous serez notifié de la réception de
	nouveaux messages:
	
	    ON      Vous serez notifiés de vos messages lorsque vous
	               vous connecterez, quand vous enlèverez
	               l'option /AWAY, de même que lorsqu'ils vous
	               seront expédiés.
	    LOGON   Vous serez exclusivement notifiés de vos
	               messages lorsque vous vous connecterez ou
	               lorsque vous enlèverez l'option /AWAY.
	    NEW     Vous serez exclusivement notifiés de vos
	               messages lorsqu'il vous seront expédiés.
	    OFF     Vous ne recevrez aucune notification de vos
	               messages.
	
	ON est équivalent à LOGON et NEW combinés.
	
	Notez que si votre surnom a l'option "secure" en fonction,
	vous serez notifiés de vos messages (pour LOGON ou ON)
	seulement après vous être identifié auprès de %s.

#MEMO_HELP_SET_LIMIT
#	Syntaxe: SET LIMIT [canal] limite
#	
#	Détermine le nombre maximum de messages que vous (ou le
#	canal donné) êtes autorisé à recevoir.  Si vous déterminez
#	cette limite à 0, personne ne sera en mesure de vous
#	expédier un message.  Toutefois, vous ne pouvez pas
#	déterminer une limite supérieure à %d.

#MEMO_HELP_INFO
#	Syntaxe: INFO [canal]
#	
#	Présente l'information relative au nombre de message que
#	vous avez, combien d'entre-eux sont non-lus, combien de
#	message vous pouvez recevoir au total et le moment où vous
#	êtes avertis de vos nouveaux messages.  Avec un paramêtre,
#	présente la même information pour un canal donné.

MEMO_OPER_HELP_COMMANDS
	
	Les Administrateurs des Services peuvent utiliser les
	commandes SET LIMIT et INFO avec des surnoms autres que
	les-leurs.

#MEMO_OPER_HELP_SET_LIMIT
#	Syntaxe: SET LIMIT [surnom | canal] {limite | NONE | DEFAULT} [HARD]
#	
#	Détermine le nombre maximum de messages qu'un surnom (ou
#	canal) est autorisé à recevoir.  Déterminer une limite de 0
#	fait en sorte que le surnom ne peut pas en recevoir aucun;
#	sélectionner NONE permet au surnom d'en recevoir et d'en
#	conserver autant qu'il le désire; et déterminer DEFAULT
#	remet à la valeur par défaut (%d) la limite de message.  Si
#	vous ne spécifiez aucun surnom ou canal, votre propre limite
#	sera déterminée.
#	
#	Ajouter HARD empêche un usager de changer la limite.  Ne pas
#	ajouter HARD a l'effet opposé, permettant ainsi à l'usager
#	de changer la limite (même si une limite avait
#	antérieurement été déterminée avec HARD).
#	
#	Cette utilisation de la commande SET LIMIT est limitée aux
#	Administrateurs des Services.  Les autres usagers peuvent
#	seulement entrer une limite pour eux-même ou pour un canal
#	sur lequel ils ont les privilèges nécessaires, ils ne
#	peuvent pas enlever leur limite ou remettre leur limite à sa
#	valeur par défaut ou encore déterminer une limite au dessus
#	de %d, et ils ne peuvent pas fixer de limite ferme.

#MEMO_OPER_HELP_INFO
#	Syntaxe: INFO [surnom | canal]
#	
#	Sans paramêtre, présente l'information relative au nombre de
#	messages que vous avez, combien d'entre-eux sont non-lus,
#	combien de message vous pouvez recevoir au total et quand
#	vous êtes notifiés de la réception de la réception de vos
#	nouveaux messages.
#	
#	Avec un paramêtre "canal", présente la même information pour
#	le canal donné.
#	
#	Avec un paramêtre "surnom", présente la même information
#	relativement au surnom donné.  Cette application est limitée
#	aux Administrateurs des Services.


MEMO_HELP_FORWARD
	Syntaxe: FORWARD {num | liste | ALL}
	
	Fait suivre un ou plusieurs de vos messages à votre adresse
	courriel.  Vous devez avoir donné une adresse courriel
	valide pour votre surnom afin de pouvoir utiliser cette
	commande.  Les messages adressés à un canal ne peuvent être
	expédiés par cette commande.

MEMO_HELP_SET_FORWARD
	Syntaxe: SET FORWARD {ON | COPY | OFF}
	
	Commande à %S de faire automatiquement suivre sur
	votre adresse courriel les messages qui vous sont expédiés.
	Lorque vous spécifiez ON, les messages expédiés à votre
	surnom seront plutôt envoyés à l'adresse courriel
	enregistrée avec votre surnom.  COPY est similaire, mais en
	plus demande à %S de sauvegarder une copie du message
	pour lecture en-ligne.  Lorsque vous spécifiez OFF, vos
	messages ne seront plus sauvés en-ligne comme à l'habitude.
	Même si vous déterminez ON, vous pouvez continuer de
	recevoir des messages en-ligne si les Services ne sont pas
	capables de vous les transmettre sur votre adresse courriel.
	
	Remarquez que lorsque l'option est spécifiée à COPY et que
	vous avez atteint le nombre maximum de message qu'il vous
	est permi de recevoir, vous ne pourrez pas recevoir de
	nouveaux messages (également par courriel) tant que vous
	n'aurez pas effacé quelques vieux messages en utilisant la
	commande DEL.

MEMO_HELP_IGNORE
	Syntaxe: IGNORE {ADD | DEL | LIST} [surnom | masque]
	
	Contrôle la liste des usagers "ignorés"; les usagers dont le
	surnom ou le masque usager@hôte correspond à une entrée sur
	cette liste ne seront pas autorisés à vous expédier de
	messages.  ADD ajoute le surnom ou le masque usager@hôte à
	votre liste d'usagers ignorés, et DEL retire le surnom ou le
	masque usager@hôte donné de la liste; LIST vous présente
	votre liste actuelle d'usagers ignorés.
	
	Remarquez que le surnom ou le masque donné avec la commande
	ADD doit contenir "*" ou "?" comme frime, et ce n'est pas un
	cas particulier; par exemple, "*Evil*" correspond à
	"EvilNick", "AN_EVIL_USER" et "devil".


###########################################################################
#
# OperServ help messages
#
###########################################################################

OPER_HELP
	%S permet aux Opérateurs d'IRC de contrôler plusieurs
	aspects du réseau.  Tapez /msg %S HELP COMMANDS pour
	une liste de ces commandes.
	
	Remarque: Toutes les commandes expédiées à %S sont
	sauvegardées!

OPER_HELP_COMMANDS
	Commandes de %S:
	    GLOBAL      Expédier un message à tous les usagers
	    STATS       Présente le status des Services et du réseau
	    SERVERMAP   Présente la carte des servers sur le réseau
	    OPER LIST   Présente la liste de tous les Opérateurs des
	                    Services
	    ADMIN LIST  Présente la liste de tous les
	                    Administrateurs des Services
OPER_HELP_COMMANDS_SERVOPER
	
	Commandes disponibles pour les Opérateurs et Administrateurs
	des Services seulement:
	    MODE        Changer un mode de canal
	    KICK        Expulser un usager d'un canal
	    CLEARMODES  Retirer certains modes d'un canal
	    CLEARCHAN   Retirer tous les usagers d'un canal
	                    (expulsion de masse)
	    KILLCLONES  Débrancher tous les usagers utilisant un
	                    certain hôte
OPER_HELP_COMMANDS_AKILL
	    AKILL       Manipuler la liste AKILL
OPER_HELP_COMMANDS_EXCLUDE
	    EXCLUDE     Manipuler la liste d'exclusion d'autokill
OPER_HELP_COMMANDS_SLINE
	    SGLINE      Manipuler la liste SGLINE
	    SQLINE      Manipuler la liste SQLINE
	    SZLINE      Manipuler la liste SZLINE
OPER_HELP_COMMANDS_SESSION
	    SESSION     Voir la liste des session hôtesses
	    EXCEPTION   Modifier la liste d'exception de limitation
	                    de session
OPER_HELP_COMMANDS_NEWS
	    LOGONNEWS   Definir un messages à être présenté aux
	                    usagers au branchement
	    OPERNEWS    Definir un messages à être présenté aux
	                    usagers qui deviennent Opérateur IRC
OPER_HELP_COMMANDS_SERVADMIN
	
	Commandes disponibles pour les Administrateurs des Services
	seulement:
	    OPER        Modifer la liste des Opérateurs des Services
	    JUPE        Ajoute un serveur fictif à la liste des
	                    serveurs
	    SET         Déterminer diverses options globales des
	                    Services
	    UPDATE      Force les banques de données des Services à
	                    être mis-à-jour sur le disque
	                    immédiatement
	    SHUTDOWN    Sauvegarde des banques de données et arrêt
	                    des Services
	    QUIT        Mettre fin aux services sans sauvegarder les
	                    banques de données
	    RESTART     Sauvegarde des banques de données et
	                    redémarrage des Services
	    REHASH      Re-lecture de fichiers de configuration
	    SU          Obtenir les privilèges de Super-usager des
	                    Services
OPER_HELP_COMMANDS_SERVROOT
	
	Commandes disponibles exclusivement pour les Super-usagers
	des Services:
	    ADMIN       Modifier la liste des Administrateurs des
	                    Services
OPER_HELP_COMMANDS_RAW
	    RAW         Donner un point sensible au serveur IRC

OPER_HELP_GLOBAL
	Syntaxe: GLOBAL message
	
	Permet aux Opérateurs d'IRC d'expédier un message à tous les
	usagers du réseau. Le message sera expédié sous le surnom
	%s.

OPER_HELP_STATS
	Syntaxe: STATS [RESET | NETWORK | ALL]
	
	Sans aucune option, présente le nombre actuel d'usagers et
	d'IRCops en-ligne (excluant les Services), le nombre le
	plus élevé d'usagers en-ligne depuis que les Services ont
	été démarrés, et le temps écoulé depuis le démarrage des
	Services.
	
	Avec l'option RESET, rétablit le compteur de maximum
	d'usager au nombre d'usagers actuellement en-ligne.
	
	Avec l'option NETWORK, présente le nombre de données
	expédiées et reçues à partir du serveur relais, ainsi que la
	quantité de mémoire utilisée par les tampons
	réception/expédition du réseau.  Le ratio d'utilisation
	actuelle des tampons de mémoire est également présenté sous
	forme de pourcentage, si une limite est configurée.
	
	L'option ALL est réservée aux Administrateurs des Services,
	et présente l'information sur l'utilisation de la mémoire
	des Services.  Utiliser cette option peut geler les Services
	pour une courte période de temps sur un vaste réseau, donc
	ne l'utilisez pas trop!
	
	UPTIME peut être utilisé comme synonyme pour STATS.

OPER_HELP_SERVERMAP
	Syntaxe: SERVERMAP
	
	Présente un plan des serveurs connectés au réseau, avec le
	point de vue des Services.  Les "Faux" serveurs--Les
	Services eux-même et tous les serveurs simulés avec la
	commande JUPE--ont un "(*)" affiché après leur nom de
	serveur.

OPER_HELP_OPER
	Syntaxe: OPER ADD surnom
	         OPER DEL surnom
	         OPER LIST
	
	Permet aux Administrateurs des Services d'ajouter ou retirer
	des surnoms dans ou de la liste des Opérateurs des Services.
	Un usager dont le surnom est dans la liste des Opérateurs
	des Services et qui est identifié auprès de %s pourra
	avoir accès aux commandes des Opérateurs des Services.
	
	Tous les Opérateurs d'IRC peuvent utiliser la forme de
	commande OPER LIST.  Toutes les autres formes sont limitées
	à l'usage des Administrateurs des Services.

OPER_HELP_ADMIN
	Syntaxe: ADMIN ADD surnom
	         ADMIN DEL surnom
	         ADMIN LIST
	
	Permet aux Super-usagers des Services d'ajouter ou retirer
	des surnoms de ou dans la liste des Administrateurs des
	Services.  Un usager dont le surnom apparait à la liste des
	Administrateurs des Services et qui est identifié auprès de
	%s sera capable d'accéder aux commandes des
	Administrateurs des Services.
	
	Tous les Opérateurs d'IRC peuvent utiliser la forme
	ADMIN LIST de la commande.  Toutes les autres applications
	sont limitées aux Super-usagers des Services.

### CHANGED: OPER_HELP_GETKEY added

OPER_HELP_MODE
	Syntaxe: MODE canal modes
	
	Permet aux Opérateurs des Services de déterminer un mode de
	canal pour n'importe quel canal.  Les paramêtres sont les
	mêmes que pour la commande normale /MODE.
	
	Limité aux Opérateurs des Services.

#OPER_HELP_CLEARMODES
#	Syntaxe: CLEARMODES canal [ALL]
#	
#	Efface tous les modes binaires (n,s,t,k,l, ainsi qu'ON) de
#	même que les bans d'un canal.  Si ALL est spécifié, efface
#	également tous les modes des usagers (comme +o et +v) sur le
#	canal.
#	
#	Limité aux Opérateurs des Services.

OPER_HELP_CLEARCHAN
	Syntaxe: CLEARCHAN canal
	
	Expulse tous les usagers d'un canal (masskick).
	
	Limité aux Opérateurs des Services.

OPER_HELP_KICK
	Syntaxe: KICK canal user raison
	
	Permet aux Opérateurs d'IRC d'expulser un usager de
	n'importe quel canal.  Les paramêtres sont les mêmes que
	pour la commande /KICK normale.  Le message d'expulsion
	donnera le nom de l'Opérateur d'IRC qui a donné la commande;
	par exemple:
	
	*** SpamMan has been kicked off channel #mon_canal by
	    %S (Alcan (Flood))
	
	Limité aux Opérateurs des Services.

#OPER_HELP_AKILL
#	Syntaxe: AKILL ADD [+expiration] masque raison
#	         AKILL DEL masque
#	         AKILL LIST [masque] [NOEXPIRE]
#	         AKILL VIEW [masque] [NOEXPIRE]
#	         AKILL COUNT
#	
#	Permet aux Opérateurs des Services de manipuler la liste
#	d'autokill (AKILL).  Si un usager correspondant à un masque
#	d'AKILL tente de se connecter, les Services feront un KILL
#	pour cet usager et, sur les types de serveurs le supportant,
#	donneront l'instruction à tous les serveurs d'ahouter un ban
#	(K-line) pour le masque auquel l'usager correspond.
#	
#	AKILL ADD ajoute le masque usager@hôte donné à la liste
#	d'AKILL pour la raison donnée (laquelle doit être donnée.)
#	AKILL DEL retire le masque donné de la liste d'AJILL si il y
#	est présent.  AKILL LIST présente tous les AKILLs actuels;
#	si le masque optionnel est donné, la liste sera limitée aux
#	AKILLs qui correspondent au masque donné. AKILL VIEW en dit
#	plus long que la version AKILL LIST, de présentera qui a
#	ajouté le AKILL, la date de l'ajout, le moment de
#	l'expiration, ainsi que le masque usager@hôte et la raison.
#	Si l'option NOEXPIRE est spécifiée, seuls les AKILLs qui
#	n'expireront pas, et qui correspondent au masque usager@hôte
#	donné, seront affichés.  AKILL COUNT retourne seulement le
#	nombre de AKILLs présents dans la liste d'AKILL.
#	
#	Optionnellement, un moment d'expiration peut être donné avec
#	la commande AKILL ADD.  Le moment d'expiration précède le
#	masque usager@hôte, et est spécifié comme un entier suivi
#	par un d (jours), h (heures), ou m (minutes).  Les
#	combinaisons (telles que 1h30m) sont également autorisées.
#	Si une l'unité n'est pas spécifiée, par défaut il s'agira de
#	jours (donc +30 en soit signifie 30 jours).  Pour ajouter un
#	AKILL qui n'expirera pas, utilisez +0.  Si le masque
#	d'usager à être ajouté commence par un +, un moment
#	d'expiration de être donné, même s'il s'agit du même que
#	celui par défaut.
#	
#	La raison peut décrire pourquoi l'entrée a été ajoutée;
#	dépendamment de comment les Services sont configurés, ça
#	peut aussi être affiché à l'usager dans le message de KILL.
OPER_HELP_AKILL_OPERMAXEXPIRY
	
	Les Opérateurs des Services peuvent seulement déterminer un
	moment d'expiration jusqu'à %s; les Administrateurs
	des Services peuvent déterminer tous les moments
	d'expiration.
OPER_HELP_AKILL_END
	
	Limité aux Opérateurs des Services.

#OPER_HELP_EXCLUDE
#	Syntaxe: EXCLUDE ADD [+expiration] masque raison
#	         EXCLUDE DEL masque
#	         EXCLUDE LIST [masque] [NOEXPIRE]
#	         EXCLUDE VIEW [masque] [NOEXPIRE]
#	         EXCLUDE CHECK masque
#	         EXCLUDE COUNT
#	
#	Permet aux Opérateurs des Services de manipuler la liste
#	d'exclusion d'autokill.  Lorsqu'un usager donc le masque
#	correspond à une entrée de liste d'exclusion d'autokill se
#	connecte au réseau, les Services n'émettront pas de KILL
#	même si l'usager correspond à un masque inscrit à la liste
#	d'autokill.  Ceci est utile si vous avez un masque
#	d'autokill vaste, et que vous désirez continuez à vouloir
#	permettre l'accès à un hôte ou un usager lequel correspond
#	au masque inscrit à la liste d'autokill.
#	
#	Les sous-commandes ADD, DEL, LIST, VIEW, CHECK, et COUNT
#	fonctionnent de la même manière que les sous-commandes de la
#	commande AKILL; voir l'assistance (HELP) pour la commande
#	AKILL afin d'avoir plus d'information.
#	
#	Limité aux Opérateurs des Services.

#OPER_HELP_SGLINE
#	Syntaxe: SGLINE ADD [+expiration] masque raison
#	         SGLINE DEL masque
#	         SGLINE LIST [masque] [NOEXPIRE]
#	         SGLINE VIEW [masque] [NOEXPIRE]
#	         SGLINE COUNT
#	
#	Permet aux Opérateurs des Services de manipuler la liste
#	SGLINE.  Si un usager dont le "vrai nom" correspond à un
#	masque SGLINE tente de se connecter, les Services émettront
#	un KILL pour cet usager et, sur les types de serveurs le
#	supportant, donneront l'instruction à tous les autres
#	serveurs d'ajouter un ban (G-line du serveur) pour le masque
#	auquel correspond l'usager.
#	
#	SGLINE ADD ajoute le masque de nom véritable donné à la
#	liste SGLINE pour la raison donnée (laquelle doit être
#	donnée).  SGLINE DEL retire le masque de nom véritable donné
#	de la liste SGLINE s'il y est inscrit.
#	SGLINE LITS présente toutes les entrées de la liste SGLINE;
#	si le masque optionnel est donné, la présentation se limite
#	aux entrées correspondantes audit masque.
#	SGLINE VIEW est une version plus complète de SGLINE LIST, et
#	présente qui a ajouté chacunes des entrées SGLINE, la date
#	de l'ajout, la dernière utilisation, quand est prévue
#	l'expiration, ainsi que le masque et la raison.  Si l'option
#	NOEXPIRE est donnée, seules les entrées qui n'expireront pas
#	et qui correspondent au masque donné seront présentées.
#	SGLINE COUNT retourne simplement le nombre d'entrées
#	inscrites à la liste SGLINE.
#	
#	Le paramêtre masque peut être compris dans un
#	double-extrait, et doit l'être lorsque le masque contient
#	des espaces; par exemple:
#	    /msg %S SGLINE ADD +0 "mauvais nom" C'est un mauvais nom
#	
#	Optionnellement, un moment d'expiration peut être donné avec
#	la commande SGLINE ADD.  Le moment d'expiration précède le
#	masque usager@hôte, et est spécifié par un entier suivi d'un
#	d (jours), h (heures), ou m (minutes).  Les combinaisons
#	(telles que 1h30m) sont également autorisées.  Si aucune
#	unité n'est spécifiée, celle par défaut est en jours (donc
#	+30 en soit signifie 30 jours).  Pour ajouter une entrée qui
#	n'expirera pas, utilisez +0.  Si le masque à être ajouté
#	commence par un + et qu'aucun moment d'expiration n'est
#	donné, le masque doit être inclus dans un double-extrait,
#	tel que décris ci-bas.
#	
#	La raison peut décrire pourquoi l'entrée a été ajoutée;
#	dépendamment de comment les Services sont configurés, cela
#	peut également être affiché aux usagers dans le message de
#	KILL.
#	
#	Limité aux Opérateurs des Services.

#OPER_HELP_SQLINE
#	Syntaxe: SQLINE ADD [+expiration] masque raison
#	         SQLINE DEL masque
#	         SQLINE LIST [masque] [NOEXPIRE]
#	         SQLINE VIEW [masque] [NOEXPIRE]
#	         SQLINE CHECK masque
#	         SQLINE COUNT
#	
#OPER_HELP_SQLINE_KILL
#	Permet aux Opérateurs des Services de manipuler la liste
#	SQLINE.  Si un usager dont le surnom correspond à un masque
#	SQLINE tente de se connecter, les Services émettront un KILL
#	pour cet usager, et sur les types de serveurs le supportant,
#	donneront l'instruction à tous les serveurs d'ajouter un ban
#	(Q-line du serveur) pour le masque auquel l'usager correspond.
#OPER_HELP_SQLINE_END
#	
#	Les sous-commandes ADD, DEL, LIST, VIEW, et COUNT peuvent
#	être utilisées pour ajouter, suprimer, visualiser (version
#	courte ou longue), ou compter le nombre d'entrées dans la
#	liste SQLINE.  Voir l'assistance (HELP) pour SQLINE (/msg
#	%S HELP SGLINE) pour plus de détails.
#	
#	Limité aux Opérateurs des Services.

#OPER_HELP_SZLINE
#	Syntaxe: SZLINE ADD [+expiration] masque raison
#	         SZLINE DEL masque
#	         SZLINE LIST [masque] [NOEXPIRE]
#	         SZLINE VIEW [masque] [NOEXPIRE]
#	         SZLINE COUNT
#	
#	Permet aux Opérateurs des Services de manipuler la liste
#	SZLINE.  Si un usager dont l'adresse IP correspond à un
#	masque SZLINE tente de se connecter, les Services émettront
#	un KILL pour cet usager et, sur les types de serveurs le
#	supportant, donneront l'instruction à tous les serveurs
#	d'ajouter un ban (Z-line du serveur) pour le masque auquel
#	l'usager correspond.
#	
#	Les sous-commandes ADD, DEL, LIST, VIEW, et COUNT peuvent
#	être utilisées pour ajouter, suprimer, visualiser (version
#	courte ou longue), ou compter le nombre d'entrées dans la
#	liste SQLINE.  Voir l'assistance (HELP) pour SQLINE (/msg
#	%S HELP SGLINE) pour plus de détails.
#	
#	Limité aux Opérateurs des Services.

#OPER_HELP_EXCEPTION
#	Syntaxe: EXCEPTION ADD [+expiration] masque limite raison
#	         EXCEPTION DEL {masque | liste}
#	         EXCEPTION MOVE num nouveau-num
#	         EXCEPTION LIST [masque | liste]
#	         EXCEPTION VIEW [masque | liste]
#	
#	Permet aux Administrateurs des Services de manipuler la
#	liste des hôtes qui ont une limitation de session
#	spécifique, permettant certains appareils, tels que les
#	serveurs coquilles (shell), de transporter plus de clients
#	que le nombre par défaut à la fois.  Une fois qu'un hôte à
#	atteint sa limitation de sessions, tous les clients qui
#	tenteront de se connecter à partir de cet hôte seront
#	débranchés.  Avant que l'usager soit débranché, il sera
#	notifié, via /NOTICE de %S, d'une source d'aide
#	concernant la limitation de session.  Le contenu de cette
#	notice est un ajustement de configuration.
#	
#	EXCEPTION ADD ajoute le masque d'hôte donné à la liste
#	d'exception.  Remarquez que les masques surnom!usager@hôte
#	et usager@hôte sont invalides!  Seuls les véritables masques
#	d'hôte, tels que boîte.hôte.dom et *.hôte.dom, sont
#	autorisés parce que la limitation de session ne tient pas
#	compte des surnoms ou noms d'usager dans ses compter.
#	limite doit être un nombre suppérieur ou égal à zéro; ceci
#	détermine combien de session cet hôte peut transporter à la
#	fois (une valeur de zéro signifie que l'hôte n'a aucune
#	limitation de session).
#	
#	Optionnellement, un moment d'expiration peut être donné avec
#	la commande EXCEPTION ADD.  Le moment d'expiration précède
#	le masque d'hôte, et est spécifié comme un entier suivi par
#	un d (jours), h (heures), ou m (minutes).  Les combinaisons
#	(telles que 1h30m) sont également autorisées.  Si aucune
#	unité n'est spécifiée, celle par défaut est en jours (donc
#	+30 en soit signifie 30 jours).  Pour ajouter une entrée qui
#	n'expirera pas, utilisez +0.  Si le masque à être ajouté
#	commence par un +, un moment d'expiration doit être donné,
#	même s'il s'agit de la même valeur que celle par défaut.
#	
#	EXCEPTION DEL retire le masque donné de la liste
#	d'exception.
#	
#	EXCEPTION MOVE déplace l'exception num à nouveau-num.  Si
#	nouveau-num est déjà utilisé par une autre exception, son
#	numéro sera augmenté afin de créer de l'espace.
#	
#	EXCEPTION LIST et EXCEPTION VIEW présente toutes les
#	exceptions actuelles; si le masque optionnel est donné,
#	cette liste est limitée aux exceptions correspondant audit
#	masque.  La différence entre les deux commandes est que
#	EXCEPTION VIEW en dit plus long, présentant le nom de la
#	personne qui a ajoutée l'exception, sa limitation de
#	session, la raison, le masque d'hôte et le jout et moment
#	d'expiration, de même que le moment de la dernière
#	utilisation de l'exception, i.c. la dernière fois qu'un
#	usager correspondant au masque d'exception s'est connecté.
#	
#	Remarquez qu'un client se connectant "utilisera" la première
#	exception correspondant à son hôte.  Les exceptions trop
#	larges ou correspondant à trop de masques dégradent quelque
#	peu les performances des Services.
#	
#	Limité aux Opérateurs des Services.

#OPER_HELP_SESSION
#	Syntaxe: SESSION LIST seuil (threshold)
#	         SESSION VIEW hôte
#	
#	Permet aux Administrateurs des Services de voir la liste des
#	sessions.
#	
#	SESSION LIST présente les hôtes qui ont au minimum le seuil
#	des sessions.  Le seuil doit être un nombre plus grand que
#	1, pour prévenir l'affichage accidentel d'un nombre trop
#	large de hôtes de session simples.
#	
#	SESSION VIEW présente l'information détaillée relative à un
#	hôte spécifique, incluant le nombre actuel de
#	comptes-session et la limitation de sessions.  La valeur
#	hôte ne doit pas inclure de frimes.
#	
#	Voir l'assistance (HELP) d'EXCEPTION pour plus d'information
#	à propos de la limitation de session et comment déterminer
#	une limitation de session spécifique pour certains hôtes et
#	groupes d'hôtes.
#	
#	Limité à l'usage des Administrateurs des Services.

OPER_HELP_SU
	Syntaxe: SU mot-de-passe
	
	Permet d'accéder aux fonctions qui requierrent les
	privilèges de Super-Usager des Services.  Le mot-de-passe
	est le même que celui déterminé avec la commande SET SUPASS;
	remarquez que le mot-de-passe doit être déterminé avant
	d'utiliser cette commande.  Les privilèges de Super-usager
	durent tant que vous ne quittez pas l'IRC.
	
	ATTENTION: Si vous entrez un mot-de-passe incorrect, les
	Services émettront un message d'avertissement à tous les
	Opérateurs d'IRC.
	
	Limité à l'usage des Administrateurs des Services.

OPER_HELP_SET
	Syntaxe: SET option ajustement
	
	Détermine une variété d'option globales des Services.  Les
	noms d'options actuellement définis sont:
	    READONLY   Déterminer les modes lecture-seulement ou
	                     lecture-écrituer
	    DEBUG      Activer ou désactiver le mode debug
	    SUPASS     Déterminer un mot-de-passe pour la commande
	                     SU (super-usager)
	
	Limité aux Administrateurs des Services; l'option SUPASS
	peut seulement être déterminé par le Super-Usager des
	Services.

OPER_HELP_SET_READONLY
	Syntaxe : SET READONLY (ON |OFF)
	
	Mets le mode lecture-seulement à ON ou à OFF.  En mode
	lecture-seulement, les usagers normaux ne seront pas
	autorisés à modifier aucune donnée des Services, incluant
	les listes d'accès des canaux et surnoms, etc.  Les
	Opérateurs d'IRC disposant des privilèges suffisants auprès
	des Services seront capables de modifier la liste d'AKILL,
	de désenregistrer ou interdire de surnoms ou canaux, mais
	aucun de ces changements ne seront sauvegardés tant que le
	mode lecture-seulement ne sera pas désactivé et que les
	Services ne termineront pas leur session ou ne seront pas
	redémarrés.
	
	Cette option est équivalente à l'option de ligne de commande
	-readonly.
	
	Limité aux Administrateurs des Services.

OPER_HELP_SET_DEBUG
	Syntaxe : SET DEBUG (ON | OFF | num)
	
	Mets le mode débogue à ON ou à OFF.  En mode débogue, toutes
	les données expédiées par ou à destination des Services
	ainsi qu'un certain nombre d'autres messages de déboguage
	sont inscris aux fichiers d'action.  Si un num est donné, le
	mode débogue est activé, avec le niveau de déboguage
	déterminé par le num.
	
	Cette option est équivalente à l'option de ligne de commande
	-debug.

OPER_HELP_SET_SUPASS
	Syntaxe : SET SUPASS [mot-de-passe]
	
	Détermine le mot-de-passe à être utilisé pour la commande
	SU.  Ce mot-de-passe doit être déterminé avant d'utiliser la
	commande SU pour la première fois.  Si aucun mot-de-passe
	n'est donné, la commande SU ne sera pas disponible.
	
	Cette option ne peut être déterminée que pas le
	Super-usager des Services.

OPER_HELP_JUPE
	Syntaxe : JUPE serveur [raison]
	
	Demande aux Services de simuler un serveur--c'est à dire,
	de créer un faux "serveur" connecté aux Services afin de
	prévenir que le serveur réel du même nom ne se connecte.  Ce
	"faux serveur" peut être retiré en utilisant la commande
	/SQUIT ordinaire.  Si une raison est donnée, elle sera
	inscrite dans le champ d'information du serveur; autrement,
	le champ d'information du serveur contiendra le texte
	"Jupitered by <surnom>", indiquant ainsi le surnom de la
	personne qui a simulé le serveur.
	
	Limité aux Administrateurs des Services.

#OPER_HELP_RAW
#	Syntaxe: RAW texte
#	
#	Donne une ligne de texte directement au serveur avec lequel
#	les Services sont connectés.  Cette commande a un champ
#	d'action vraiment limité, et peut faire des ravages sur un
#	réseau ou provoquer l'arrêt des services si elle est mal
#	utilisée.  N'UTILISEZ PAS CETTE COMMANDE si vous n'êtes pas
#	absolument certain de savoir comment le faire!
#	
#	Limité à l'usage des Super-usagers des Services.

#OPER_HELP_UPDATE
#	Syntaxe : UPDATE
#	
#	Force les Services à mettre à jour toutes les banques de
#	données dès que vou entrez la commande.
#	
#	Limité aux Administrateurs des Services.

OPER_HELP_QUIT
	Syntaxe : QUIT
	
	Force l'arrêt immédiat des Services; les banques de données
	ne sont pas sauvegardées.  Cette commande ne peut pas être
	utilisée sans endommager la copie en mémoire interne des
	banques de données qui est perdue et ne peut pas être
	sauvegardée.  Pour une mise à l'arrêt normale, utilisez la
	commande SHUTDOWN.
	
	Limité aux Administrateurs des Services.

OPER_HELP_SHUTDOWN
	Syntaxe : SHUTDOWN
	
	Force la sauvegarde de toutes les banques de données suivi
	de l'arrêt des Services.
	
	Limité aux Administrateurs des Services.

OPER_HELP_RESTART
	Syntaxe : RESTART
	
	Force la sauvegarde de toutes les banques de données suivi
	du redémarrage des Services.  (C'est à dire sortie et
	redémarrage immédiat des fichiers exécutables).
	
	Limité aux Administrateurs des Services.

OPER_HELP_REHASH
	Syntaxe : REHASH
	
	Force les Services à re-lire ses fichiers de configuration
	et de mettre à jour ses ajustements actifs.  Certains
	ajustements, tels que RemoteServer (lequel détermine le nom
	d'hôte du serveur IRC auquel il est connecté), ne peuvent
	pas être changés avec REHASH, et ne prendront effet que
	lorsque les Services seront redémarrés; auquel cas, un
	message sera inscrit au fichier d'action, et une notice sera
	expédiée, vous en avisant.
	
	Si des lignes LoadModule ont été ajoutées ou suprimées du
	fichier principal de configuration des Services les modules
	(%s), ne seront pas chargés ou déchargés tel que
	désiré.  Les modules n'ayant plus de ligne LoadModule seront
	déchargés dans l'ordre inverse de leur chargement; ensuite,
	après que les modules restant aient été remaniés, les
	nouveaux modules seront chargés dans l'ordre qu'ils
	apparaissent dans le fichier de configuration.  Si une
	erreur survient à n'importe quelle étape, le remaniement
	avortera.
	
	Remarquez qu'en raison de la dépendance des modules, certain
	modules ne peuvent pas être retirés ou chargés avec la
	commande REHASH; dans ces cas, vous aurez besoin de
	redémarrer les Services pour que les modifications prennent
	effet.
	
	Limité aux Administrateurs des Services.

OPER_HELP_KILLCLONES
	Syntaxe: KILLCLONES surnom
	
	Déconnecte tous les usagers qui ont le même nom d'hôte comme
	surnom.  Un AKILL temporaire, de forme *@hôte, est ajouté
	afin d'empècher le client fautif de se reconnecter
	immédiatement.  Un message WALLOPS est également expédié
	indiquant qui a utilisé la commande, quel hôte a été
	affecté, et combien d'usagers ont été déconnectés.  C,est
	utile pour retirer les clônes nombreux du réseau.
	
	Limité aux Opérateurs des Services.


###########################################################################
#
# StatServ help messages
#
###########################################################################

STAT_HELP
	%S donnes des informations statistiques relatives au
	réseau IRC, ses serveurs et ses usagers.  Tapez /msg
	%S HELP COMMANDS pour la liste des commandes de
	%S; pour de l'information sur une commande, tapez /msg
	%S HELP commande.

STAT_HELP_COMMANDS
	Les commandes suivantes peuvent être utilisées avec
	%S:
	
	    SERVERS     Statistiques relatives aux serveurs
	    USERS       Statistics relatives aux usagers

STAT_HELP_SERVERS
	Syntaxe: SERVERS STATS
	         SERVERS LIST [masque]
	         SERVERS VIEW [masque] [ONLINE | OFFLINE]
	
	Présente les statistiques relatives aux serveurs du réseau.
	
	SERVERS STATS présente un bref sommaire des serveurs
	actuellement visibles (en-ligne) et invisibles (hors-ligne)
	pour les Services.
	SERVERS LIST présente une liste des serveurs en-ligne et
	les statistiques actuelles relatives à chacun.
	SERVERS VIEW présente une liste des serveurs en-ligne et
	hors-ligne avec des statistiques détaillées relatives à chacuns.
	Les options ONLINE et OFFLINE font en sorte,
	respectivement, que seulement les serveurs en-ligne ou
	hors-ligne soient présentés.
	
	Utilisez l'option masque pour limiter les statistiques de
	quel serveur seront présentées.

STAT_HELP_USERS
	Syntaxe: USERS STATS
	
	Présente les statistiques relatives aux usagers du réseau.

STAT_OPER_HELP_SERVERS
	Syntaxe: SERVERS STATS
	         SERVERS LIST [masque]
	         SERVERS VIEW [masque] [ONLINE | OFFLINE]
	         SERVERS DELETE serveur
	         SERVERS COPY serveur nouveau-nom
	         SERVERS RENAME serveur nouveau-nom
	
	Présente les statistiques relatives aux serveurs du réseau.
	
	SERVERS STATS présente un bref sommaire des serveurs
	actuellement visibles (en-ligne) et invisibles (hors-ligne)
	aux Services.
	SERVERS LIST présente une liste des serveurs en-ligne et
	les statistiques actuelles relatives à chacun.
	SERVERS VIEW présente une liste des serveurs en-ligne et
	hors-ligne avec des statistiques détaillées relatives à
	chacun. Les options ONLINE et OFFLINE donnent,
	respectivement, seulement les serveurs en-ligne et
	hors-ligne.
	
	Utilisez l'option masque pour limiter quelles statistiques
	de serveur seront présentées.
	
	DELETE efface la statistique de serveur spécifiée.
	COPY copie un ensemble de statistiques de serveur.
	RENAME renomme un ensemble de statistiques de serveur.
	Ces trois commandes sont limitées aux Administrateurs des
	Services.

###########################################################################

# CVS: $Revision: 2.97 $
