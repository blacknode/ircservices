# Emacs: -*- mode: fundamental; coding: binary -*-

# Spanish language file.
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
	Español (Spanish)

# Character set used by this language.  This should be a "charset" value
# understood by mailreader or web browser software.

LANG_CHARSET
	iso-8859-1

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
	Dom
	Lun
	Mar
	Mie
	Jue
	Vie
	Sab
# %A
STRFTIME_DAYS_LONG
	Domingo
	Lunes
	Martes
	Miercoles
	Jueves
	Viernes
	Sabado
# %b
STRFTIME_MONTHS_SHORT
	Ene
	Feb
	Mar
	Abr
	May
	Jun
	Jul
	Ago
	Sep
	Oct
	Nov
	Dic
# %B
STRFTIME_MONTHS_LONG
	Enero
	Febrero
	Marzo
	Abril
	Mayo
	Junio
	Julio
	Agosto
	Septiembre
	Octubre
	Noviembre
	Diciembre


# "day", "hour", "minute", and "second" in singular and plural.
# These must be preceded by spaces if the language requires them
# (e.g. in English: "10 hours", not "10hours").
STR_DAY
	 día
STR_DAYS
	 días
STR_HOUR
	 hora
STR_HOURS
	 horas
STR_MINUTE
	 minuto
STR_MINUTES
	 minutos
STR_SECOND
	 segundo
STR_SECONDS
	 segundos
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
	Error Interno - imposible procesar comando.
SERVICES_IS_BUSY
	Los servicios están muy ocupados en este momento; favor de intentar mas tarde nuevamente.

UNKNOWN_COMMAND
	Comando desconocido %s.
UNKNOWN_COMMAND_HELP
	Comando desconocido %s.  Escriba /msg %s HELP para ayuda.
SYNTAX_ERROR
	Sintaxis: %s
MORE_INFO
	Escriba /msg %s HELP %s para más información.
NO_HELP_AVAILABLE
	No existe ayuda disponible para %s.
MISSING_QUOTE
	Faltan las doble comillas (").

BAD_EMAIL
	La dirección de correo electrónico debe ser en el formato Usuario@ProveedorWeb.  No debes usar colores, negritas, subrayado, ni reverso, o alguno de estos caracteres: , : ; | \ " ( ) < > [ ]
REJECTED_EMAIL
	Lo sentimos, esa dirección de correo electrónico no puede ser usada.
BAD_URL
	URLs deben ser en el formato http://ProveedorWeb[:puerto]/... (o ftp://, etc.).
BAD_USERHOST_MASK
	Máscara debe ser en el formato Usuario@ProveedorWeb.
BAD_NICKUSERHOST_MASK
	Máscara debe ser en el formato Apodo!Usuario@ProveedorWeb.
BAD_EXPIRY_TIME
	Fecha de expiración inválida.

SENDMAIL_NO_RESOURCES
	El sistema está actualmente demasiado ocupado para enviar correo; Favor trate más tarde nuevamente.
READ_ONLY_MODE
	Notícia: ¡Los Servicios están en modo de leer solamente; cambios no serán grabados!
PASSWORD_INCORRECT
	Contraseña incorrecta.

PASSWORD_WARNING
	Advertensia: Despues de una contraseña incorrecta más usted será desconectado.

# What's the difference between "Access denied" and "Permission denied"?
# Very little; you can safely make them the same message with no real loss
# of meaning.  If you want to make a distinction, "Access denied" is
# usually used more generally; for example, a non-oper trying to access
# OperServ gets "Access denied", while a non-Services admin trying to use
# NickServ SET NOEXPIRE gets "Permission denied".
ACCESS_DENIED
	Acceso negado.
PERMISSION_DENIED
	Permiso negado.

MORE_OBSCURE_PASSWORD
	Favor trate de nuevo con una contraseña más difícil. Contraseñas deben ser de 5 caracteres mínimo,no debe ser algo fácil de descifrar (ejemplo. su verdadero nombre o su apodo).

NICK_NOT_REGISTERED
	Su apodo no está registrado.
NICK_NOT_REGISTERED_HELP
	Su apodo no está registrado.  Escriba /msg %s HELP para más información de como registrar su apodo.
NICK_TOO_LONG
	Apodos no deben tener más de %d caracteres de largo.
NICK_INVALID
	%s no es un apodo válido.
NICK_X_NOT_REGISTERED
	El apodo %s no está registrado.
NICK_X_ALREADY_REGISTERED
	¡El apodo %s ya está registrado!
NICK_X_NOT_IN_USE
	El apodo %s no está en uso en este momento.
NICK_X_FORBIDDEN
	El apodo %s no puede ser registrado o utilizado.
NICK_X_SUSPENDED
	El apodo %s está suspendido y no se puede utilizar o identificarse por él.
NICK_X_SUSPENDED_MEMOS
	El apodo %s está suspendido y no puede recibir mensajes.
NICK_IDENTIFY_REQUIRED
	Identificación con su contraseña se requiere para usar ese comando.
	Vuelva a intentar después de escribir /msg %s IDENTIFY Contraseña.
NICK_PLEASE_AUTH
	Su apodo no está aún autenticado.  Favor de autenticar su apodo usando el código de autenticación incluido en el correo electrónico enviado a: %s
NICK_X_NOT_ON_CHAN_X
	El apodo %s no está actualmente en el canal %s.

CHAN_INVALID
	%s no es un nombre válido de canal.
CHAN_X_NOT_REGISTERED
	El canal %s no está registrado.
CHAN_X_NOT_IN_USE
	El canal %s no está en uso actualmente.
CHAN_X_FORBIDDEN
	El canal %s no puede ser registrado o utilizado.
CHAN_X_SUSPENDED
	El canal %s está suspendido y no se puede utilizar o identificarse por él.
CHAN_X_SUSPENDED_MEMOS
	El canal %s está suspendido y no puede recibir mensajes.
CHAN_IDENTIFY_REQUIRED
	Identificación con su contraseña se requiere para usar ese comando.
	Vuelva a intentar después de escribir /msg %s IDENTIFY %s Contraseña.

SERV_X_NOT_FOUND
	El servidor %s no fue encontrado.

EXPIRES_NONE
	no expira
EXPIRES_NOW
	expiró
EXPIRES_IN
	expira en %s
LIST_RESULTS
	Fin del listado; %d/%d asociaciones visibles.

###########################################################################
#
# NickServ messages
#
###########################################################################

# Automatic messages
NICK_IS_REGISTERED
	Este apodo pertence a otra persona.  Favor de escoger otro.  (Si este es su apodo, escriba /msg %s IDENTIFY Contraseña.)
NICK_IS_SECURE
	Este apodo está registrado y protegido.  Si es su apodo, escriba /msg %s IDENTIFY Contraseña.  De lo contrario, favor de escoger un apodo diferente.
NICK_MAY_NOT_BE_USED
	Este apodo no puede utilizarse. Favor de escoger otro.
DISCONNECT_IN_1_MINUTE
	Si no cambia su apodo dentro de un minuto, será desconectado.
DISCONNECT_IN_20_SECONDS
	Si no cambia su apodo dentro de 20 segundos, será desconectado.
DISCONNECT_NOW
	Este apodo ha sido registrado; no lo puedes utilizar.  Favor de escoger otro apodo antes de reconectar.
FORCENICKCHANGE_IN_1_MINUTE
	Si no cambia su apodo dentro de un minuto, le será cambiado automáticamente.
FORCENICKCHANGE_IN_20_SECONDS
	Si no cambia su apodo dentro de 20 segundos, le será cambiado automáticamente.
FORCENICKCHANGE_NOW
	Este apodo ha sido registrado; no lo puedes utilizar.  Su apodo está ahóra siendo cambiado a %s.
NICK_EXPIRES_SOON
	Nota: La registración de su apodo expirará en %s.  Si usted desea continuar utilizando su apodo, favor de identificarse a %s Escribiendo: /msg %s IDENTIFY Contraseña
NICK_EXPIRED
	Nota: La registración de su apodo ha expirado.  Favor de re-registrar su apodo si usted desea continuar utilizandolo.

# REGISTER responses
NICK_REGISTER_SYNTAX
	REGISTER Contraseña [Correo-Electrónico]
NICK_REGISTER_REQ_EMAIL_SYNTAX
	REGISTER Contraseña Correo-Electrónico
NICK_REGISTRATION_DISABLED
	Lo sentimos, el regístro de apodos está temporaralmente deshabilitado.
NICK_REGISTRATION_FAILED
	Lo sentimos, la registración falló.
NICK_REG_PLEASE_WAIT
	Favor de esperar %s antes de utilizar nuevamente el comando REGISTER.
NICK_REG_PLEASE_WAIT_FIRST
	Favor de esperar %s antes de utilizar el comando REGISTER.
NICK_CANNOT_BE_REGISTERED
	El apodo %s no puede ser registrado.
NICK_REGISTER_EMAIL_UNAUTHED
	La dirección de correo electrónico (E-mail) que usted proevió no ha sido aún autenticada. Por favor use el comando AUTH para autenticar la dirección antes de registrar un nuevo apodo con ella.
NICK_REGISTER_TOO_MANY_NICKS
	Usted tiene demasiados apodos registrados (%d, máximo de %d).  No puede registrarse más a menos que cancele algunos de sus apodos actuales.
NICK_REGISTERED
	El apodo %s ha sido registrado a usted.
NICK_PASSWORD_IS
	Su contraseña es %s -- recuerde esto para uso posterior.

# IDENTIFY responses
NICK_IDENTIFY_SYNTAX
	IDENTIFY Contraseña
NICK_IDENTIFY_FAILED
	Lo sentimos, la identificación falló.
NICK_IDENTIFY_EMAIL_MISSING
	Usted debe fijar una dirección de correo electrónico para su apodo antes de continuar usandolo.  Favor de fijar una ahóra con el comando: /msg %s SET EMAIL Dirección
NICK_IDENTIFY_SUCCEEDED
	Contraseña aceptada - ahóra se le ha reconocido.

# DROP responses
NICK_DROP_SYNTAX
	DROP Contraseña
NICK_DROP_WARNING
	Nota: ¡El comando DROP cancelará todos sus apodos ligados!
NICK_DROP_DISABLED
	Lo sentimos, la cancelación de apodos ha sido temporaralmente dishabilatada.
NICK_DROP_FAILED
	Lo sentimos, la cancelación del apodo falló.
NICK_DROPPED
	Su apodo ha sido cancelado.
NICK_DROPPED_LINKS
	Su apodo y todos los apodos ligados han sido cancelados.

# DROPNICK responses
NICK_DROPNICK_SYNTAX
	DROPNICK Apodo
NICK_X_DROPPED
	El apodo %s ha sido cancelado.
NICK_X_DROPPED_LINKS
	El apodo %s y todos los apodos ligados han sido cancelados.

# DROPEMAIL and DROPEMAIL-CONFIRM responses
NICK_DROPEMAIL_SYNTAX
	DROPEMAIL Patrón
NICK_DROPEMAIL_PATTERN_TOO_LONG
	El patrón dado es demaciado largo (%d caractéres máximo).
NICK_DROPEMAIL_NONE
	Ningún apodo tiene una dirección de correo electrónico que concuerde con el patrón especificado.
NICK_DROPEMAIL_COUNT
	%d apodos tienen direcciónes de correo electrónico que concuerdan con el patrón especificado.
	Para cancelar estos apodos, escriba: /msg %s DROPEMAIL-CONFIRM %s
NICK_DROPEMAIL_CONFIRM_SYNTAX
	DROPEMAIL-CONFIRM Patrón
NICK_DROPEMAIL_CONFIRM_UNKNOWN
	El patrón especificado no es reconocido.  Por favor utilice el comando DROPEMAIL nuevamente con este patrón antes de usar el comando DROPEMAIL-CONFIRM.
NICK_DROPEMAIL_CONFIRM_DROPPING
	Cancelando %d apodos...
NICK_DROPEMAIL_CONFIRM_DROPPED
	Cancelación de apodos completado.

# SET responses
NICK_SET_SYNTAX
	SET Opción Parámetros
NICK_SET_OPER_SYNTAX
	SET [!Apodo] Opción Parámetros
NICK_SET_DISABLED
	Lo sentimos, la configuración de opciones del apodo está temporalmente deshabilitada.
NICK_SET_UNKNOWN_OPTION
	Opción SET desconocida %s.

# SET PASSWORD responses
NICK_SET_PASSWORD_FAILED
	Lo sentimos, la contraseña no se pudo cambiar.
NICK_SET_PASSWORD_CHANGED
	Contraseña cambiada.
NICK_SET_PASSWORD_CHANGED_TO
	Contraseña cambiada a %s.

# SET LANGUAGE responses
NICK_SET_LANGUAGE_SYNTAX
	SET LANGUAGE Número
NICK_SET_LANGUAGE_UNKNOWN
	Número %d de lenguaje desconocido.  Escriba /msg %s HELP SET LANGUAGE para ver la lista de lenguajes disponibles.
NICK_SET_LANGUAGE_CHANGED
	Lenguaje cambiado a %s.

# SET URL responses
NICK_SET_URL_CHANGED
	URL para %s cambiado a %s.

# SET EMAIL responses
NICK_SET_EMAIL_PLEASE_WAIT
	Favor de esperar %s antes de usar el comando SET EMAIL nuevamente.
NICK_SET_EMAIL_UNAUTHED
	La dirección de correo electrónico que usted proporcionó no ha sido aún autenticada. Por favor use el comando AUTH para autenticar la dirección primeramente.
NICK_SET_EMAIL_TOO_MANY_NICKS
	%s ya tiene %d apodos registrados (máximo de %d).  El cambio de correo-electrónico falló
NICK_SET_EMAIL_CHANGED
	Correo electrónico (EMAIL) para %s cambiada a %s.

# SET INFO responses
NICK_SET_INFO_CHANGED
	Información para %s cambiada.

# SET KILL responses
NICK_SET_KILL_SYNTAX
	SET KILL {ON | QUICK | OFF}
NICK_SET_KILL_IMMED_SYNTAX
	SET KILL {ON | QUICK | IMMED | OFF}
NICK_SET_KILL_ON
	Protección de KILL ahóra Activada (ON).
NICK_SET_KILL_QUICK
	Protección de KILL ahóra Activada (ON), con una espera reducida.
NICK_SET_KILL_IMMED
	Protección de KILL Activada (ON), sin espera reducida.
NICK_SET_KILL_IMMED_DISABLED
	La opción IMMED no está disponible en esta red.
NICK_SET_KILL_OFF
	Protección de KILL ahóra Desactivada (OFF).

# SET SECURE responses
NICK_SET_SECURE_SYNTAX
	SET SECURE {ON | OFF}
NICK_SET_SECURE_ON
	Opción SECURE ahóra Activada (ON).
NICK_SET_SECURE_OFF
	Opción SECURE ahóra Desactivada (OFF).

# SET PRIVATE responses
NICK_SET_PRIVATE_SYNTAX
	SET PRIVATE {ON | OFF}
NICK_SET_PRIVATE_ON
	Opción PRIVATE ahóra Activada (ON).
NICK_SET_PRIVATE_OFF
	Opción PRIVATE ahóra Desactivada (OFF).

# SET NOOP responses
NICK_SET_NOOP_SYNTAX
	SET NOOP {ON | OFF}

# SET HIDE responses
NICK_SET_HIDE_SYNTAX
	SET HIDE {EMAIL | USERMASK | QUIT} {ON | OFF}
NICK_SET_HIDE_EMAIL_ON
	Su correo electrónico será ahóra ocultada en la opción de %s INFO.
NICK_SET_HIDE_EMAIL_OFF
	Su correo electrónico será ahóra visible en la opción de %s INFO.
NICK_SET_HIDE_MASK_ON
	Su última máscara de Usuario@ProveedorWeb será ahóra ocultada en la opción de %s INFO.
NICK_SET_HIDE_MASK_OFF
	Su última máscara de Usuario@ProveedorWeb será ahóra visible en la opción de %s INFO.
NICK_SET_HIDE_QUIT_ON
	Su último mensaje de salida será ahóra ocultado en la opción de %s INFO.
NICK_SET_HIDE_QUIT_OFF
	Su último mensaje de salida será ahóra visible en la opción de %s INFO.

# SET TIMEZONE responses
NICK_SET_TIMEZONE_SYNTAX
	SET TIMEZONE {UTC-Más/Menos | Uso-Horario | DEFAULT}
NICK_SET_TIMEZONE_TO
	Su uso horario ha sido ajustado a UTC%c%d:%02d.
	La hora actual en este uso horario es %s.
NICK_SET_TIMEZONE_DEFAULT
	Su uso horario ha sido re-ajustado al uso horario de los Servicios.

# SET MAINNICK responses
NICK_SET_MAINNICK_NOT_FOUND
	El apodo %s no está ligado al apodo de usted.
NICK_SET_MAINNICK_CHANGED
	Su apodo principal ha sido cambiado a %s.

# SET NOEXPIRE responses
NICK_SET_NOEXPIRE_SYNTAX
	SET [!Apodo] NOEXPIRE {ON | OFF}
NICK_SET_NOEXPIRE_ON
	El apodo %s no expirará.
NICK_SET_NOEXPIRE_OFF
	El apodo %s si expirará.

# UNSET responses
NICK_UNSET_SYNTAX
	UNSET {URL | EMAIL | INFO}
NICK_UNSET_SYNTAX_REQ_EMAIL
	UNSET {URL | INFO}
NICK_UNSET_OPER_SYNTAX
	UNSET [!Apodo] {URL | EMAIL | INFO}
NICK_UNSET_OPER_SYNTAX_REQ_EMAIL
	UNSET [!Apodo] {URL | INFO}
NICK_UNSET_URL
	URL para %s está ahóra Indefinido (Unset).
NICK_UNSET_EMAIL
	Correo electrónico (EMAIL) para %s está ahóra Indefinido (Unset).
NICK_UNSET_INFO
	Información para %s está ahóra Indefinida (Unset).
NICK_UNSET_EMAIL_BAD
	Usted no puede remover su correo electrónico (EMAIL).
NICK_UNSET_EMAIL_OTHER_BAD
	El correo electrónico de los apodos no puede ser removida.

# ACCESS responses
NICK_ACCESS_SYNTAX
	ACCESS {ADD | DEL | LIST} [Máscara]
NICK_ACCESS_DISABLED
	Lo sentimos, modificación a la lista de acceso del apodo está temporalmente deshabilitada.
NICK_ACCESS_ALREADY_PRESENT
	Máscara %s ya existe en su lista de acceso.
NICK_ACCESS_REACHED_LIMIT
	Lo sentimos, solamente puede tener %d entradas de acceso para su apodo.
NICK_ACCESS_NO_NICKS
	Advertencia: Las máscaras de acceso de apodos no deben incluír apodos.  Por favor verifique y asegurarse de que ha ingresado la máscara Usuario@Proveedor correctamente.
NICK_ACCESS_ADDED
	%s añadido a su lista de acceso.
NICK_ACCESS_NOT_FOUND
	%s no fue encontrado en su lista de acceso.
NICK_ACCESS_DELETED
	%s removido de su lista de acceso.
NICK_ACCESS_LIST_EMPTY
	Su lista de acceso está vacía.
NICK_ACCESS_LIST
	Lista de acceso:
NICK_ACCESS_LIST_X_EMPTY
	Lista de acceso para %s está vacía.
NICK_ACCESS_LIST_X
	Lista de acceso para %s:

# LINK responses
NICK_OLD_LINK_SYNTAX
	LINK Apodo Contraseña
NICK_LINK_SYNTAX
	LINK Apodo
NICK_LINK_DISABLED
	Lo sentimos, el ligado de apodos esta temporalmente deshabilitado.
NICK_LINK_FAILED
	Lo sentimos, liga de apodo falló.
NICK_CANNOT_BE_LINKED
	El apodo %s no puede ser ligado.
NICK_OLD_LINK_SAME
	¡No puede ligar su apodo actual!
NICK_LINK_SAME
	¡No puede ligar su apodo actual!
NICK_LINK_ALREADY_LINKED
	El apodo %s ya está ligado al apodo de usted.
NICK_LINK_IN_USE
	El apodo %s está siendo usado en este momento.
NICK_LINK_TOO_MANY
	Usted no puede tener más de %d apodos ligados a una misma liga.
NICK_LINK_TOO_MANY_NICKS
	Usted ya tiene %d apodos registrados (el máximo es %d).  la liga falló.
NICK_OLD_LINK_TOO_MANY_CHANNELS
	No fue posible ligar el apodo %s: demasiados canales registrados (combinación máxima %d).
NICK_OLD_LINKED
	Su apodo ha sido ligado a %s.
NICK_LINKED
	El apodo %s ha sido ligado a su apodo actual.

# UNLINK responses
NICK_OLD_UNLINK_SYNTAX
	UNLINK [Apodo Contraseña]
NICK_UNLINK_SYNTAX
	UNLINK Apodo
NICK_UNLINK_OPER_SYNTAX
	UNLINK Apodo [FORCE]
NICK_UNLINK_DISABLED
	Lo sentimos, el desligado de apodos esta temporalmente deshabilitado.
NICK_UNLINK_FAILED
	Lo sentimos, la liga no pudo ser removida.
NICK_UNLINK_SAME
	¡Usted no puede remover una liga de un apodo mientras lo está usando!
NICK_OLD_UNLINK_NOT_LINKED
	Su apodo no está ligado.
NICK_UNLINK_NOT_LINKED_YOURS
	El apodo %s no está ligado a su apodo.
NICK_UNLINK_NOT_LINKED
	El apodo %s no está ligado.
NICK_OLD_UNLINKED
	Su apodo ha sido desligado de %s.
NICK_UNLINKED
	El apodo %s ha sido desligado del apodo de usted.
NICK_X_UNLINKED
	El apodo %s ha sido desligado de %s.

# LISTLINKS responses
NICK_OLD_LISTLINKS_SYNTAX
	LISTLINKS Apodo
NICK_LISTLINKS_SYNTAX
	LISTLINKS
NICK_LISTLINKS_HEADER
	Lista de apodos ligados a %s:
NICK_LISTLINKS_FOOTER
	%d apodos ligados.

# INFO responses
NICK_INFO_SYNTAX
	INFO Apodos [ALL]
NICK_INFO_REALNAME
	%s es %s
NICK_INFO_ADDRESS
	  Última dirección vista: %s
NICK_INFO_ADDRESS_ONLINE
	    Está en línea usando: %s
NICK_INFO_ADDRESS_ONLINE_NOHOST
	%s está actualmente en línea.
NICK_INFO_ADDRESS_OTHER_NICK
	Está usando el apodo: %s
NICK_INFO_TIME_REGGED
	       Fecha de regístro: %s
NICK_INFO_LAST_SEEN
	Se le vio por última vez en: %s
NICK_INFO_LAST_QUIT
	Último mensaje de salida: %s
NICK_INFO_URL
	                     URL: %s
NICK_INFO_EMAIL
	      Correo electrónico: %s
NICK_INFO_EMAIL_UNAUTHED
	   Correo electrónico: %s (No-Verificado)
NICK_INFO_INFO
	      Información: %s
NICK_INFO_OPTIONS
	                Opciones: %s
# These strings MUST NOT be empty
NICK_INFO_OPT_KILL
	Protección de KILL
NICK_INFO_OPT_SECURE
	Seguro
NICK_INFO_OPT_PRIVATE
	Privado
NICK_INFO_OPT_NONE
	Ninguna
NICK_INFO_NO_EXPIRE
	Este apodo no expirará.
NICK_INFO_SUSPEND_DETAILS
	Suspendido por %s en %s (%s).
NICK_INFO_SUSPEND_REASON
	Razón: %s
NICK_INFO_SHOW_ALL
	Para más información, escriba: /msg %s INFO %s ALL

# LISTCHANS responses
NICK_LISTCHANS_SYNTAX
	LISTCHANS
NICK_LISTCHANS_NONE
	%s no ha registrado canal alguno.
NICK_LISTCHANS_HEADER
	Canales registrados por %s:
NICK_LISTCHANS_END
	Fin del listado (%d canales registrados).

# LIST responses
NICK_LIST_SYNTAX
	%s [+Inicio] Patrón
NICK_LIST_OPER_SYNTAX
	%s [+Inicio] Patrón [FORBIDDEN] [NOEXPIRE] [SUSPENDED]
NICK_LIST_OPER_SYNTAX_AUTH
	%s [+Inicio] Patrón [FORBIDDEN] [NOEXPIRE] [SUSPENDED] [NOAUTH]
NICK_LIST_HEADER
	Lista de entradas similares a %s:
NICK_LIST_NO_MATCH
	No se encontraron apodos similares
NICK_LISTEMAIL_NONE
	(Ninguno fijado)

# RECOVER responses
NICK_RECOVER_SYNTAX
	RECOVER Apodo [Contraseña]
NICK_NO_RECOVER_SELF
	¡Usted no puede utilizar el comando RECOVER en usted mismo!
NICK_RECOVERED
	El usuario que reclamaba su apodo ha sido desconectado.
	Escriba /msg %s RELEASE %s Contraseña para recuperarlo ahóra y no esperar a un minuto.

# RELEASE responses
NICK_RELEASE_SYNTAX
	RELEASE Apodo [Contraseña]
NICK_RELEASE_NOT_HELD
	El apodo %s no está siendo retenido.
NICK_RELEASED
	La retención de su apodo por los Servicios has sido liberada.

# GHOST responses
NICK_GHOST_SYNTAX
	GHOST Apodo [Contraseña]
NICK_NO_GHOST_SELF
	¡No puede usar el comando GHOST en usted mismo!
NICK_GHOST_KILLED
	El fantasma con su apodo ha sido desconectado.

# GETPASS responses
NICK_GETPASS_SYNTAX
	GETPASS Apodo
NICK_GETPASS_UNAVAILABLE
	El comando GETPASS no está disponible para el apodo %s porque se está usando encripción.
NICK_GETPASS_PASSWORD_IS
	Contraseña para %s es %s.

# FORBID responses
NICK_FORBID_SYNTAX
	FORBID Apodo
NICK_FORBID_SUCCEEDED
	El apodo %s está ahóra prohibido.
NICK_FORBID_FAILED
	¡No se pudo prohibir el apodo %s!

# SUSPEND responses
NICK_SUSPEND_SYNTAX
	SUSPEND [+Expiración] Apodo Razón
NICK_SUSPEND_SUCCEEDED
	El apodo %s, y todos los apodos ligados a este, han sido suspendidos.
NICK_SUSPEND_ALREADY_SUSPENDED
	El apodo %s ya está suspendido.

# UNSUSPEND responses
NICK_UNSUSPEND_SYNTAX
	UNSUSPEND Apodo
NICK_UNSUSPEND_SUCCEEDED
	El apodo %s ya no está suspendido.
NICK_UNSUSPEND_NOT_SUSPENDED
	El apodo %s no está suspendido.

################ autojoin module messages/responses

# AJOIN responses
NICK_AJOIN_SYNTAX
	AJOIN {ADD | DEL | LIST} [canal]
NICK_AJOIN_ADD_SYNTAX
	AJOIN ADD canal
NICK_AJOIN_DEL_SYNTAX
	AJOIN DEL canal
NICK_AJOIN_DISABLED
	Lo sentimos, modificación a la lista de Auto-Ingreso está temporalmente deshabilitada.
NICK_AJOIN_ALREADY_PRESENT
	El canal %s ya está presente en su lista autojoin (Auto-Ingreso).
NICK_AJOIN_LIST_FULL
	Lo sentimos, solo puedes tener %d entradas de autojoin (Auto-Ingreso) por cada apodo.
NICK_AJOIN_ADDED
	%s agregado a su lista autojoin (Auto-Ingreso).
NICK_AJOIN_NOT_FOUND
	%s no fue encontrado en su lista autojoin (Auto-Ingreso).
NICK_AJOIN_DELETED
	%s removido de su lista autojoin (Auto-Ingreso).
NICK_AJOIN_LIST_EMPTY
	Su lista autojoin (Auto-Ingreso) está vacía.
NICK_AJOIN_LIST
	Lista Autojoin (Auto-Ingreso):
NICK_AJOIN_LIST_X_EMPTY
	Lista Autojoin (Auto-Ingreso) para %s está vacía.
NICK_AJOIN_LIST_X
	Lista Autojoin (Auto-Ingreso) para %s:
NICK_AJOIN_AUTO_REMOVE
	El canal %s ha sido removido de su lista de auto-ingreso porque no es un nombre de canal válido..

################ mail-auth module messages/responses

# General-purpose messages
NICK_AUTH_SENDING
	Un código de autenticación para su apodo está siendo enviando a %s.
NICK_AUTH_FOR_REGISTER
	Cuando reciba este mensaje, escriba /msg %s AUTH Código (reemplace Código por el código de autenticación incluido en el mensaje) para completar la registración de su apodo.
NICK_AUTH_FOR_SET_EMAIL
	Cuando reciba este mensaje, escriba /msg %s AUTH Código (reemplace Código por el código de autenticación incluido en el mensaje) para completar el cambio a su dirección E-mail (Correo-electrónico)
	Favor note que usted no podrá continuar usando su apodo a menos que complete el cambio.
NICK_AUTH_NOT_NEEDED
	No existe nada que necesite autenticación en este momento.
NICK_AUTH_NO_AUTHCODE
	El apodo %s no tiene código de autenticación.
NICK_AUTH_HAS_AUTHCODE
	El apodo %s ya tiene un código de autenticación.
PASSWORD_WARNING_FOR_AUTH
	Advertensia: Despues de otro código de autenticación incorrecto usted será desconectado.

# Mail text.  The last "%s" (before the user@host) in the body text is
# replaced by one of the NICK_AUTH_MAIL_TEXT_* messages.
NICK_AUTH_MAIL_SUBJECT
	Código de autenticación para %s
NICK_AUTH_MAIL_BODY
	El código de autenticación para su apodo (%s) es: %09d
	Favor de ingresar este código a %s con el comando:
	    /msg %s AUTH %09d
	
	Este mensaje fue enviado por %s en respuesta a %s
	%s@%s.
NICK_AUTH_MAIL_TEXT_REG
	registración por
NICK_AUTH_MAIL_TEXT_EMAIL
	un cambio de E-mail (Correo-Electrónico) por
NICK_AUTH_MAIL_TEXT_SENDAUTH
	un comando SENDAUTH de
NICK_AUTH_MAIL_TEXT_REAUTH
	un comando REAUTH de
NICK_AUTH_MAIL_BODY_SETAUTH
	El código de autenticación para su apodo (%s) es: %09d
	Favor de ingresar este código a %s con el comando:
	    /msg %s AUTH %09d

# AUTH responses
NICK_AUTH_SYNTAX
	AUTH Código
NICK_AUTH_DISABLED
	Lo sentimos, La autenticación de apodos está temporaralmente deshabilatada.
NICK_AUTH_FAILED
	Ese código de autenticación es incorrecto.
NICK_AUTH_SUCCEEDED_REGISTER
	Autenticación exitosa; la registración de su apodo ha sido completada.
NICK_AUTH_SUCCEEDED_SET_EMAIL
	Autenticación exitosa; su cambio de E-mail (Correo-Electrónico) ha sido completado.
NICK_AUTH_SUCCEEDED_REAUTH
	Autenticación exitosa; te has identificado por tu apodo ahora.
NICK_AUTH_SUCCEEDED_SETAUTH
	Autenticación exitosa; puedes continuar usando tu apodo ahóra.

# SENDAUTH responses
NICK_SENDAUTH_SYNTAX
	SENDAUTH
NICK_SENDAUTH_TOO_SOON
	Favor de esperar %s antes de usar el comando SENDAUTH nuevamente.
NICK_SENDAUTH_NORESOURCES
	El mensaje que contenía su código de autenticación no pudo ser enviado porque el sistema está muy ocupado.  Favor de intentar mas tarde o contactar un administrador de la red.
NICK_SENDAUTH_ERROR
	El mensaje que contenía su código de autenticación no pudo ser enviado debido a un problema en el sistema.  Favor de intentar mas tarde o contactar un administrador de la red.

# REAUTH responses
NICK_REAUTH_SYNTAX
	REAUTH
NICK_REAUTH_HAVE_AUTHCODE
	Un código de autenticación está ya fijado para su apodo.
NICK_REAUTH_NO_EMAIL
	Su apodo no tiene fijada una dirección de correo electrónico.
NICK_REAUTH_AUTHCODE_SET
	Un nuevo código de autenticación ha sido fijado para su apodo, y le será enviado en breve.  Use ese código con el comando AUTH para indentificarse por su apodo.

# RESTOREMAIL responses
NICK_RESTOREMAIL_SYNTAX
	RESTOREMAIL Contraseña
NICK_RESTOREMAIL_NOT_NOW
	RESTOREMAIL solo puede usarse despues de cambiar su dirección de correo electrónico , y antes de autenticar la dirección nueva.
NICK_RESTOREMAIL_DONE
	Su dirección previa de correo electrónico %s ha sido restaurada.

# SETAUTH responses
NICK_SETAUTH_SYNTAX
	SETAUTH Apodo
NICK_SETAUTH_NO_EMAIL
	El apodo %s no tiene un E-mail (Correo-Electrónico) establecido.
NICK_SETAUTH_AUTHCODE_SET
	Un nuevo código de autenticación (%09d) ha sido fijado para el apodo %s.
NICK_SETAUTH_SEND_REFUSED
	Advertencia: no fue posible enviar correo a %s (el mensaje fue bloqueado por el servidor de correo).
NICK_SETAUTH_SEND_TIMEOUT
	Advertencia: no fue posible enviar correo a  %s (fue imposible conectar o enviar al servidor de correo).
NICK_SETAUTH_SEND_NORESOURCES
	Advertencia: no fue posible enviar correo a %s (insuficientes recursos).
NICK_SETAUTH_SEND_ERROR
	Advertencia: no fue posible enviar correo a %s (error desconocido).
NICK_SETAUTH_USER_NOTICE
	Usted debe autenticar su apodo antes de continuar usandolo.  Un código de autenticación ha sido enviado a su E-mail (Correo-Electrónico): %s
	Escriba: /msg %s HELP AUTH para más información.

# GETAUTH responses
NICK_GETAUTH_SYNTAX
	GETAUTH Apodo
NICK_GETAUTH_AUTHCODE_IS
	El código de autenticación para el apodo %s es: %09d

# CLEARAUTH responses
NICK_CLEARAUTH_SYNTAX
	CLEARAUTH Apodo
NICK_CLEARAUTH_CLEARED
	El código de autenticación para el apodo %s ha sido removido.

###########################################################################
#
# ChanServ messages
#
###########################################################################

# Access level descriptions
CHAN_LEVEL_AUTOOP
	Estatus de operador de canal automático
CHAN_LEVEL_AUTOVOICE
	Modo +v automático
CHAN_LEVEL_INVITE
	Permitido usar el comando INVITE
CHAN_LEVEL_AKICK
	Permitido usar el comando AKICK
CHAN_LEVEL_SET
	Permitido usar el comando SET (excluidos FOUNDER/PASSWORD)
CHAN_LEVEL_CLEAR
	Permitido usar el comando CLEAR
CHAN_LEVEL_UNBAN
	Permitido usar el comando UNBAN
CHAN_LEVEL_OPDEOP
	Permitido usar los comandos OP/DEOP
CHAN_LEVEL_ACCESS_LIST
	Permitido ver la lista de acceso
CHAN_LEVEL_ACCESS_CHANGE
	Permitido modificar la lista de acceso
CHAN_LEVEL_MEMO
	Recive mensajes del canal
CHAN_LEVEL_VOICE
	Permitido usar los comandos VOICE/DEVOICE
CHAN_LEVEL_AUTOHALFOP
	Estatus MedioOP (+h) automático
CHAN_LEVEL_HALFOP
	Permitido usar los comandos HALFOP/DEHALFOP (MitadOP/No-MitadOP)
CHAN_LEVEL_AUTOPROTECT
	Estatus de Protección de canal (+a) automático
CHAN_LEVEL_PROTECT
	Permitido usar los comandos PROTECT/DEPROTECT (Proteger/Desproteger)
CHAN_LEVEL_KICK
	Permitido usar el comando KICK
CHAN_LEVEL_STATUS
	Permitido usar el comando STATUS
CHAN_LEVEL_TOPIC
	Permitido usar el comando TOPIC

# Automatic responses
CHAN_IS_REGISTERED
	Este canal ha sido registrado con %s.
CHAN_MAY_NOT_BE_USED
	Este canal no puede ser utilizado.
CHAN_NOT_ALLOWED_TO_JOIN
	No le es permitido ingresar a este canal.

# Multi-purpose responses
CHAN_MUST_IDENTIFY_NICK
	Favor de identificarse con %s primero, usando el comando:
	/msg %s IDENTIFY Contraseña
CHAN_BOUNCY_MODES
	Lo sentimos, el comando %s está temporaralmente deshabilitado.

# REGISTER responses
CHAN_REGISTER_SYNTAX
	REGISTER Canal Contraseña Descripcción
CHAN_REGISTER_DISABLED
	Lo sentimos, la registración de canales está temporalmente deshabilitada.
CHAN_REGISTER_SHORT_CHANNEL
	El canal # no puede ser registrado.
CHAN_REGISTER_NOT_LOCAL
	Canales locales no pueden ser registrados.
CHAN_REGISTER_INVALID_NAME
	Solo puedes registrar nombres de canales que comiencen con el signo #.
CHAN_MUST_REGISTER_NICK
	Usted debe registrar su apodo primeramente.  Escriba /msg %s HELP para información sobre como registrar apodos.
CHAN_MAY_NOT_BE_REGISTERED
	El canal %s no puede ser registrado.
CHAN_ALREADY_REGISTERED
	¡El canal %s ya está registrado!
CHAN_MUST_BE_CHANOP
	Usted debe ser operador del canal para poder registrarlo.
CHAN_REACHED_CHANNEL_LIMIT
	Lo sentimos, usted ya ha alcazado su límite de %d canales.
CHAN_EXCEEDED_CHANNEL_LIMIT
	Lo sentimos, usted ya ha excedido su límite de %d canales.
CHAN_REGISTRATION_FAILED
	Lo sentimos, la registración falló.
CHAN_REGISTERED
	Canal %s registrado bajo su apodo: %s
CHAN_PASSWORD_IS
	La contraseña de su canal es %s - recuerdela para uso posterior.

# IDENTIFY responses
CHAN_IDENTIFY_SYNTAX
	IDENTIFY Canal Contraseña
CHAN_IDENTIFY_FAILED
	Lo sentimos, la identificación falló.
CHAN_IDENTIFY_SUCCEEDED
	Contraseña aceptada - usted ahóra tiene acceso a nivel de fundador con %s.

# DROP responses
CHAN_DROP_SYNTAX
	DROP Canal Contraseña
CHAN_DROP_DISABLED
	Lo sentimos, la cancelación de regístro de canal está temporalmente deshabilitada.
CHAN_DROPPED
	El canal %s ha sido cancelado.

# DROPCHAN responses
CHAN_DROPCHAN_SYNTAX
	DROPCHAN Canal

# SET responses
CHAN_SET_SYNTAX
	SET Canal Opción Parámetros
CHAN_SET_DISABLED
	Lo sentimos, el ajuste de opciones de canal está temporalmente deshabilitada.
CHAN_SET_UNKNOWN_OPTION
	Opción SET %s desconocida.

# SET FOUNDER responses
CHAN_SET_FOUNDER_TOO_MANY_CHANS
	%s tiene demasiados canales registrados.
CHAN_FOUNDER_CHANGED
	Fundador de %s cambiado a %s.

# SET SUCCESSOR responses
CHAN_SUCCESSOR_CHANGED
	Sucesor para %s cambiado a %s.
CHAN_SUCCESSOR_IS_FOUNDER
	Usted no puede convertir al fundador de un canal también en sucesor.

# SET PASSWORD responses
CHAN_SET_PASSWORD_FAILED
	Lo sentimos, fue imposible establecer la contraseña.
CHAN_PASSWORD_CHANGED
	Contraseña para %s cambiada.
CHAN_PASSWORD_CHANGED_TO
	Contraseña para %s cambiada a %s.

# SET DESC responses
CHAN_DESC_CHANGED
	Descripción de %s cambiada a %s.

# SET URL responses
CHAN_URL_CHANGED
	URL para %s cambiado a %s.

# SET EMAIL responses
CHAN_EMAIL_CHANGED
	Correo electrónico para %s cambiado a %s.

# SET ENTRYMSG responses
CHAN_ENTRY_MSG_CHANGED
	Mensaje de entrada para %s cambió.

# SET MLOCK responses
CHAN_SET_MLOCK_NEED_PLUS_MINUS
	Lista de Modos debe comenzar con un + (más) o - (menos).
CHAN_SET_MLOCK_NEED_PARAM
	Parámetro requerido para MLOCK +%c.
CHAN_SET_MLOCK_NEED_POSITIVE
	Parámetro para MLOCK +%c debe ser un número positivo.
CHAN_SET_MLOCK_MODE_REG_BAD
	Modo +%c es siempre establecido en canales registrados, y no puede ser usado con MLOCK (Opción Candado).
CHAN_SET_MLOCK_LINK_BAD
	Parámetro para MLOCK +%c no es un nombre válido de canal.
CHAN_SET_MLOCK_LINK_SAME
	Parámetro para MLOCK +%c debe ser canal diferente.
CHAN_SET_MLOCK_REQUIRES
	MLOCK +%c requiere que se fije MLOCK +%c también.
CHAN_SET_MLOCK_BAD_PARAM
	Parámetro inválido para MLOCK +%c.
CHAN_SET_MLOCK_UNKNOWN_CHAR
	Modo con caractér %c es desconocido e ignorado.
CHAN_SET_MLOCK_CANNOT_LOCK
	Modo %c no puede ser fijado con la opción candado (MLOCK).
CHAN_MLOCK_CHANGED
	Modo de candado en canal %s cambiado a %s.
CHAN_MLOCK_REMOVED
	Modo de candado en canal %s removido.

# SET HIDE responses
CHAN_SET_HIDE_SYNTAX
	SET Canal HIDE {EMAIL | TOPIC | MLOCK} {ON | OFF}
CHAN_SET_HIDE_EMAIL_ON
	La dirección electrónica (E-mail) del canal %s será ahóra ocultada de el comando %s INFO.
CHAN_SET_HIDE_EMAIL_OFF
	La dirección electrónica (E-mail) del canal %s será ahóra mostrada en el comando %s INFO.
CHAN_SET_HIDE_TOPIC_ON
	El tópico del canal %s será ahóra ocultado de el comando %s INFO.
CHAN_SET_HIDE_TOPIC_OFF
	El tópico del canal %s será ahóra mostrado en el comando %s INFO.
CHAN_SET_HIDE_MLOCK_ON
	El modo de candado del canal %s será ahóra ocultado de el comando %s INFO.
CHAN_SET_HIDE_MLOCK_OFF
	El modo de candado del canal %s será ahóra mostrado en el comando %s INFO.

# SET KEEPTOPIC responses
CHAN_SET_KEEPTOPIC_SYNTAX
	SET Canal KEEPTOPIC {ON | OFF}
CHAN_SET_KEEPTOPIC_ON
	Opción de retención de tópico ahóra ACTIVADA (ON).
CHAN_SET_KEEPTOPIC_OFF
	Opción de retención de tópico ahóra DESACTIVADA (OFF).

# SET TOPICLOCK responses
CHAN_SET_TOPICLOCK_SYNTAX
	SET Canal TOPICLOCK {ON | OFF}
CHAN_SET_TOPICLOCK_ON
	Opción de candado en tópico ACTIVADA (ON).
CHAN_SET_TOPICLOCK_OFF
	Opción de candado en tópico DESACTIVADA (OFF).

# SET PRIVATE responses
CHAN_SET_PRIVATE_SYNTAX
	SET Canal PRIVATE {ON | OFF}
CHAN_SET_PRIVATE_ON
	Opción de privacidad ACTIVADA (ON).
CHAN_SET_PRIVATE_OFF
	Opción de privacidad DESACTIVADA (OFF).

# SET SECUREOPS responses
CHAN_SET_SECUREOPS_SYNTAX
	SET Canal SECUREOPS {ON | OFF}
CHAN_SET_SECUREOPS_ON
	Opción de SecureOPs (OPs Seguros) ACTIVADA (ON).
CHAN_SET_SECUREOPS_OFF
	Opción de SecureOPs (OPs Seguros) DESACTIVADA (OFF).

# SET LEAVEOPS responses
CHAN_SET_LEAVEOPS_SYNTAX
	SET Canal LEAVEOPS {ON | OFF}
CHAN_SET_LEAVEOPS_ON
	Opción de LeaveOPs (OPs Dejados) ACTIVADA (ON).
CHAN_SET_LEAVEOPS_OFF
	Opción de LeaveOPs (OPs Dejados) DESACTIVADA (OFF).

# SET RESTRICTED responses
CHAN_SET_RESTRICTED_SYNTAX
	SET Canal RESTRICTED {ON | OFF}
CHAN_SET_RESTRICTED_ON
	Opción de acceso restringido ACTIVADA (ON).
CHAN_SET_RESTRICTED_OFF
	Opción de acceso restringido DESACTIVADA (OFF).

# SET SECURE responses
CHAN_SET_SECURE_SYNTAX
	SET Canal SECURE {ON | OFF}
CHAN_SET_SECURE_ON
	Opción Secure (Seguro) ahóra ACTIVADA (ON).
CHAN_SET_SECURE_OFF
	Opción Secure (Seguro) ahóra DESACTIVADA (OFF).

# SET OPNOTICE responses
CHAN_SET_OPNOTICE_SYNTAX
	SET Canal OPNOTICE {ON | OFF}
CHAN_SET_OPNOTICE_ON
	Opción de Op-notice (Nota a OPs) ahóra ACTIVADA (ON).
CHAN_SET_OPNOTICE_OFF
	Opción de Op-notice (Nota a OPs) ahóra DESACTIVADA (OFF).

# SET ENFORCE responses
CHAN_SET_ENFORCE_SYNTAX
	SET Canal ENFORCE {ON | OFF}
CHAN_SET_ENFORCE_ON
	Opción de Enforce (Implementar) está ahóra ACTIVADA (ON).
CHAN_SET_ENFORCE_OFF
	Opción de Enforce (Implementar) está ahóra DESACTIVADA (OFF).

# SET MEMO-RESTRICTED responses
CHAN_SET_MEMO_RESTRICTED_SYNTAX
	SET Canal MEMO-RESTRICTED {ON | OFF}

# SET NOEXPIRE responses
CHAN_SET_NOEXPIRE_SYNTAX
	SET Canal NOEXPIRE {ON | OFF}
CHAN_SET_NOEXPIRE_ON
	El canal %s NO expirará.
CHAN_SET_NOEXPIRE_OFF
	El canal %s SI expirará.

# UNSET responses
CHAN_UNSET_SYNTAX
	UNSET Canal {SUCCESSOR | URL | EMAIL | ENTRYMSG}
CHAN_SUCCESSOR_UNSET
	Sucesor para %s ahóra indefinido.
CHAN_URL_UNSET
	URL para %s ahóra indefinido.
CHAN_EMAIL_UNSET
	Correo electrónico (EMAIL) para %s ahóra indefinida.
CHAN_ENTRY_MSG_UNSET
	Mensaje de entrada para %s ahóra indefinido.

# ACCESS messages
CHAN_ACCESS_SYNTAX
	ACCESS Canal {ADD | DEL | LIST | LISTLEVEL | COUNT} [Parámetros]
CHAN_ACCESS_ADD_SYNTAX
	ACCESS canal ADD apodo nivel
CHAN_ACCESS_DEL_SYNTAX
	ACCESS canal DEL apodo
CHAN_ACCESS_LIST_SYNTAX
	ACCESS canal LIST [+Inicio] [máscara]
CHAN_ACCESS_LISTLEVEL_SYNTAX
	ACCESS canal LISTLEVEL [+Inicio] nivel
CHAN_ACCESS_COUNT_SYNTAX
	ACCESS canal COUNT
CHAN_ACCESS_DISABLED
	Lo sentimos, modificación a la lista de acceso de canal está temporalmente deshabilitada.
CHAN_ACCESS_LEVEL_NONZERO
	Nivel de acceso debe ser diferente a cero.
CHAN_ACCESS_LEVEL_RANGE
	Nivel de acceso debe estar entre %d y %d.
CHAN_ACCESS_NICKS_ONLY
	La lista de acceso del canal deberá contener solamente apodos registrados.
CHAN_ACCESS_REACHED_LIMIT
	Lo sentimos, usted solamente puede tener %d entradas de acceso en un canal.
CHAN_ACCESS_LEVEL_UNCHANGED
	Nivel de acceso para %s en %s no cambio de %d.
CHAN_ACCESS_LEVEL_CHANGED
	Nivel de acceso para %s en %s fue cambiado a %d.
CHAN_ACCESS_ADDED
	%s agregado a la lista de acceso de %s con nivel %d.
CHAN_ACCESS_NOT_FOUND
	%s no fue encontrado en la lista de acceso de %s.
CHAN_ACCESS_DELETED
	%s borrado de la lista de acceso de %s.
CHAN_ACCESS_LIST_EMPTY
	La lista de acceso de %s está vacía.
CHAN_ACCESS_NO_MATCH
	No existe entrada alguna en la lista de acceso de %s.
CHAN_ACCESS_LIST_HEADER
	Lista de acceso para %s:
	  Niv  Apodo
CHAN_ACCESS_LIST_FORMAT
	 %4d  %s
CHAN_ACCESS_COUNT
	La lista de acceso de %s tiene %d entradas.

# XOP (SOP/AOP/etc.) responses
CHAN_SOP_SYNTAX
	SOP Canal {ADD | DEL | LIST | COUNT} [Parámetros]
CHAN_AOP_SYNTAX
	AOP Canal {ADD | DEL | LIST | COUNT} [Parámetros]
CHAN_HOP_SYNTAX
	HOP Canal {ADD | DEL | LIST | COUNT} [Parámetros]
CHAN_VOP_SYNTAX
	VOP Canal {ADD | DEL | LIST | COUNT} [Parámetros]
CHAN_NOP_SYNTAX
	NOP Canal {ADD | DEL | LIST | COUNT} [Parámetros]
CHAN_SOP_LIST_SYNTAX
	SOP Canal {ADD | DEL | LIST | COUNT} [+Inicio] [Máscara]
CHAN_AOP_LIST_SYNTAX
	AOP Canal {ADD | DEL | LIST | COUNT} [+Inicio] [Máscara]
CHAN_HOP_LIST_SYNTAX
	HOP Canal {ADD | DEL | LIST | COUNT} [+Inicio] [Máscara]
CHAN_VOP_LIST_SYNTAX
	VOP Canal {ADD | DEL | LIST | COUNT} [+Inicio] [Máscara]
CHAN_NOP_LIST_SYNTAX
	NOP Canal {ADD | DEL | LIST | COUNT} [+Inicio] [Máscara]
CHAN_XOP_ADDED
	%s agregado a la lista de %s %s.
CHAN_XOP_LEVEL_CHANGED
	%s movido a la lista de %s %s.
CHAN_XOP_LEVEL_UNCHANGED
	%s ya está en la lista de %s %s.
CHAN_XOP_NICKS_ONLY
	Las listas de SOP, AOP, VOP, y NOP solo pueden contener apodos registrados.
CHAN_XOP_NICKS_ONLY_HOP
	Las listas de SOP, AOP, HOP, VOP, y NOP solo pueden contener apodos registrados.
CHAN_XOP_REACHED_LIMIT
	Lo sentimos, el número de entradas en sus listas de SOP, AOP, VOP y NOP no puede sobrepasar más de %d entradas.  Límite excedido.
CHAN_XOP_REACHED_LIMIT_HOP
	Lo sentimos, el número de entradas en sus listas de SOP, AOP, HOP, VOP y NOP no puede sobrepasar más de %d entradas.  Límite excedido.
CHAN_XOP_NOT_FOUND
	%s no fue encontrado en la lista de %s %s.
CHAN_XOP_DELETED
	%s borrado de la lista de %s %s.
CHAN_XOP_LIST_EMPTY
	La lista de %s %s está vacía.
CHAN_XOP_NO_MATCH
	No existe entrada alguna en la lista de %s %s.
CHAN_XOP_LIST_HEADER
	Lista de %s para %s:
CHAN_XOP_COUNT
	La lista de %s %s tiene %d entradas.

# AKICK responses
CHAN_AKICK_SYNTAX
	AKICK Canal {ADD | DEL | LIST | VIEW | COUNT | ENFORCE} [Parámetros]
CHAN_AKICK_LIST_SYNTAX
	AKICK Canal LIST [+Inicio] [Máscara]
CHAN_AKICK_VIEW_SYNTAX
	AKICK channel VIEW [+Inicio] [mask]
CHAN_AKICK_DISABLED
	Lo sentimos, modificación a la lista de pateo automático de canal está temporalmente deshabilitada.
CHAN_AKICK_ALREADY_EXISTS
	%s ya existe en la lista de pateo automático de %s.
CHAN_AKICK_REACHED_LIMIT
	Lo sentimos, usted solo puede tener %d máscaras de pateo automático en un canal.
CHAN_AKICK_ADDED
	%s agregado a la lista de pateo automático de %s.
CHAN_AKICK_NOT_FOUND
	%s no se encuentra en la lista de pateo automático de %s.
CHAN_AKICK_DELETED
	%s borrado de la lista de pateo automático de %s.
CHAN_AKICK_LIST_EMPTY
	Lista de pateo automático de %s está vacía.
CHAN_AKICK_NO_MATCH
	No existe entrada alguna en la lista de pateo automático de %s.
CHAN_AKICK_LIST_HEADER
	Lista de pateo automático para %s:
# mask, set-by, set-time, last-used, reason
CHAN_AKICK_VIEW_FORMAT
	    %s (por %s en %s; última vez usado %s)
	        %s
# mask, set-by, set-time, reason
CHAN_AKICK_VIEW_UNUSED_FORMAT
	    %s (por %s en %s; nunca usado)
	        %s
CHAN_AKICK_COUNT
	%s tiene %d entradas en su lista de Auto Pateo.
CHAN_AKICK_ENFORCE_DONE
	Implementación de auto pateo para %s completado; %d usuarios fueron afectados.

# LEVELS responses
CHAN_LEVELS_SYNTAX
	LEVELS Canal {SET | DIS[ABLE] | LIST | RESET} [Articulo [Nivel]]
CHAN_LEVELS_READONLY
	Lo sentimos, modificación a la lista de nivel de privilegios del canal está actualmente deshabilitada.
CHAN_LEVELS_RANGE
	Nivel debe de estar entre %d y %d.
CHAN_LEVELS_CHANGED
	Nivel para %s en canal %s cambiado a %d.
CHAN_LEVELS_UNKNOWN
	Opción %s desconocida.  Escriba /msg %s HELP LEVELS DESC para ver la lista de opciones válidas.
CHAN_LEVELS_DISABLED
	%s deshabilitado en canal %s.
CHAN_LEVELS_LIST_HEADER
	Configuración de nivel de prilegios para el canal %s:
CHAN_LEVELS_LIST_DISABLED
	    %-*s  (deshabilitado)
CHAN_LEVELS_LIST_FOUNDER
	    %-*s  (fundador solamente)
CHAN_LEVELS_LIST_NORMAL
	    %-*s  %4d
CHAN_LEVELS_RESET
	Niveles de privilegios para el canal %s reconfigurados a los niveles originales.

# INFO responses
CHAN_INFO_SYNTAX
	INFO Canal [ALL]
CHAN_INFO_HEADER
	Información del canal %s:
CHAN_INFO_FOUNDER
	           Fundador: %s
CHAN_INFO_SUCCESSOR
	            Sucesor: %s
CHAN_INFO_DESCRIPTION
	        Descripción: %s
CHAN_INFO_ENTRYMSG
	 Mensaje de Entrada: %s
CHAN_INFO_TIME_REGGED
	         Registrado: %s
CHAN_INFO_LAST_USED
	         Último uso: %s
CHAN_INFO_LAST_TOPIC
	      Último tópico: %s
CHAN_INFO_TOPIC_SET_BY
	Tópico ajustado por: %s
CHAN_INFO_URL
	                URL: %s
CHAN_INFO_EMAIL
	 Correo electrónico: %s
CHAN_INFO_OPTIONS
	           Opciones: %s
CHAN_INFO_OPT_PRIVATE
	Privado
CHAN_INFO_OPT_KEEPTOPIC
	Retención de tópico
CHAN_INFO_OPT_TOPICLOCK
	Candado en tópico
CHAN_INFO_OPT_SECUREOPS
	OPs Seguros
CHAN_INFO_OPT_LEAVEOPS
	OPs Dejados
CHAN_INFO_OPT_RESTRICTED
	Acceso restringido
CHAN_INFO_OPT_SECURE
	Seguro
CHAN_INFO_OPT_OPNOTICE
	Nota a OPs
CHAN_INFO_OPT_ENFORCE
	Implementar
CHAN_INFO_OPT_NONE
	Ninguna
CHAN_INFO_MODE_LOCK
	    Modo de candado: %s
CHAN_INFO_NO_EXPIRE
	Este canal no expirará.
CHAN_INFO_SUSPEND_DETAILS
	Suspendido por %s en %s (%s).
CHAN_INFO_SUSPEND_REASON
	Razón: %s
CHAN_INFO_SHOW_ALL
	Para más información, Escriba: /msg %s INFO %s ALL

# LIST responses
CHAN_LIST_SYNTAX
	LIST [+Inicio] Patrón
CHAN_LIST_OPER_SYNTAX
	LIST [+Inicio] Patrón [FORBIDDEN] [NOEXPIRE] [SUSPENDED]
CHAN_LIST_HEADER
	Lista de entradas similares a %s:
CHAN_LIST_NO_MATCH
	No se encontraron canales similares.

# INVITE responses
CHAN_INVITE_SYNTAX
	INVITE Canal
CHAN_INVITE_OK
	Invitando %s al canal %s.

# OP/DEOP/VOICE/DEVOICE responses
CHAN_OPVOICE_SYNTAX
	%s Canal [Apodo]
CHAN_OP_SUCCEEDED
	OPs dadas a %s en el canal %s.
CHAN_OP_ALREADY
	%s ya tiene OPs en el canal %s.
CHAN_OP_FAILED
	Imposible dar OPs a %s en el canal %s.
CHAN_DEOP_SUCCEEDED
	OPs removidas de %s en el canal %s.
CHAN_DEOP_ALREADY
	Ya le fueron removidas los OPs a %s en el canal %s.
CHAN_DEOP_FAILED
	Imposible remover OPs de %s en el canal %s.
CHAN_VOICE_SUCCEEDED
	Voz dada a %s en el canal %s.
CHAN_VOICE_ALREADY
	%s ya tiene voz en el canal %s.
CHAN_VOICE_FAILED
	Imposible dar voz a %s en el canal %s.
CHAN_DEVOICE_SUCCEEDED
	Voz removida de %s en el canal %s.
CHAN_DEVOICE_ALREADY
	Ya se le ha removido la voz a %s en el canal %s.
CHAN_DEVOICE_FAILED
	Imposible remover voz de %s en el canal %s.
CHAN_HALFOP_SUCCEEDED
	Mitad-OPs dadas a %s en el canal %s.
CHAN_HALFOP_ALREADY
	%s ya tiene estatus de Mitad-OPs en el canal %s.
CHAN_HALFOP_FAILED
	Imposible dar mitad-OPs a %s en el canal %s.
CHAN_DEHALFOP_SUCCEEDED
	Mitad-OPs removidas de %s en el canal %s.
CHAN_DEHALFOP_ALREADY
	Ya le fue removido el estatus de Mitdad-OPs a %s en el canal %s.
CHAN_DEHALFOP_FAILED
	Imposible remover mitad-OPs de %s en el canal %s.
CHAN_PROTECT_SUCCEEDED
	Protegido %s en el canal %s.
CHAN_PROTECT_ALREADY
	%s ya ha sido protegido en el canal %s.
CHAN_PROTECT_FAILED
	Imposible proteger a %s en el canal %s.
CHAN_DEPROTECT_SUCCEEDED
	Desprovisto de protección %s en el canal %s.
CHAN_DEPROTECT_ALREADY
	%s ya está desprovisto de protección en el canal %s.
CHAN_DEPROTECT_FAILED
	Imposible desproteger a %s en el canal %s.

# UNBAN responses
CHAN_UNBAN_SYNTAX
	UNBAN Canal
CHAN_UNBANNED
	Usted ya no está bloqueado en %s.

# KICK responses
CHAN_KICK_SYNTAX
	KICK Canal Apodo [Razón]
CHAN_KICK_PROTECTED
	%s no puede ser pateado en el canal %s.
CHAN_KICKED
	%s pateado de el canal %s.

# TOPIC responses
CHAN_TOPIC_SYNTAX
	TOPIC Canal Tópico

# CLEAR responses
CHAN_CLEAR_SYNTAX
	CLEAR Canal Opción
CHAN_CLEARED_BANS
	Todos los bloqueos en el canal %s han sido removidos.
CHAN_CLEARED_EXCEPTIONS
	Todas las excepciones en el canal %s han sido removidas.
CHAN_CLEARED_INVITES
	Todas las máscaras de invitación en el canal %s han sido removidas.
CHAN_CLEARED_MODES
	Todos los modos en el canal %s han sido borrados.
CHAN_CLEARED_OPS
	Modo +o ha sido desactivado en el canal %s.
CHAN_CLEARED_HALFOPS
	Modo +h ha sido desactivado en el canal %s.
CHAN_CLEARED_VOICES
	Modo +v ha sido desactivado en el canal %s.
CHAN_CLEARED_USERS
	Todos los usuarios han sido pateados del canal %s.

# GETPASS responses
CHAN_GETPASS_SYNTAX
	GETPASS Canal
CHAN_GETPASS_UNAVAILABLE
	El comando GETPASS no está disponible para el canal %s porque se está usando encripción.
CHAN_GETPASS_PASSWORD_IS
	Contraseña para el canal %s es %s.

# FORBID responses
CHAN_FORBID_SYNTAX
	FORBID Channel
CHAN_FORBID_SHORT_CHANNEL
	El canal # no puede ser prohibido.
CHAN_FORBID_SUCCEEDED
	El canal %s está ahóra prohibido.
CHAN_FORBID_FAILED
	¡No fue posible prohibir el canal %s!

# SUSPEND responses
CHAN_SUSPEND_SYNTAX
	SUSPEND [+Expiración] Canal Razón
CHAN_SUSPEND_SUCCEEDED
	El canal %s ha sido suspendido.
CHAN_SUSPEND_ALREADY_SUSPENDED
	El canal %s ya está suspendido.

# UNSUSPEND responses
CHAN_UNSUSPEND_SYNTAX
	UNSUSPEND Canal
CHAN_UNSUSPEND_SUCCEEDED
	Suspensión del canal %s ha sido desactivada.
CHAN_UNSUSPEND_NOT_SUSPENDED
	El canal %s no está suspendido.

###########################################################################
#
# MemoServ messages
#
###########################################################################

# Automatic messages
MEMO_HAVE_NEW_MEMO
	Usted tiene 1 nuevo mensaje.
MEMO_HAVE_NEW_MEMOS
	Usted tiene %d nuevos mensajes.
MEMO_TYPE_READ_LAST
	Escriba /msg %s READ LAST para leerlo.
MEMO_TYPE_READ_NUM
	Escriba /msg %s READ %d para leerlo.
MEMO_TYPE_LIST_NEW
	Escriba /msg %s LIST NEW para alistarlos.
MEMO_AT_LIMIT
	¡AVISO!: Usted ha alcanzado su número máximo de mensajes (%d).  No podrá recibir mensajes nuevos hasta que borre algunos de los que ahóra tiene.
MEMO_OVER_LIMIT
	¡AVISO!: Usted ha pasado su número máximo de mensajes (%d).  No podrá recibir mensajes nuevos hasta que borre algunos de los que ahóra tiene.
MEMO_NEW_MEMO_ARRIVED
	Usted tiene un nuevo mensaje de %s.
	Escriba /msg %s READ %d para leerlo.
MEMO_NEW_CHAN_MEMO_ARRIVED
	Usted tiene un nuevo mensaje de %s para %s.
	Escriba /msg %s READ %d para leerlo.

# Multi-use responses
MEMO_HAVE_NO_MEMOS
	Usted no tiene mensajes.
MEMO_HAVE_NO_NEW_MEMOS
	Usted no tiene nuevos mensajes.
MEMO_DOES_NOT_EXIST
	¡El mensaje %d no existe!
MEMO_LIST_NOT_FOUND
	Ningún mensaje similar fue encontrado.

# SEND responses
MEMO_SEND_SYNTAX
	SEND {Apodo | Canal} Texto
MEMO_SEND_DISABLED
	Lo sentimos, el envío de mensajes está temporalmente deshabilitado.
MEMO_SEND_PLEASE_WAIT
	Favor de esperar %s antes de utilizar el comando SEND nuevamente.
MEMO_SEND_CHAN_NOT_AVAIL
	Lo sentimos, los mensajes del canal no están disponibles.
MEMO_X_GETS_NO_MEMOS
	%s no puede recibir mensajes.
MEMO_X_HAS_TOO_MANY_MEMOS
	%s tiene demasiados mensajes actualmente y no puede recibir más.
MEMO_SENT
	Mensaje enviado a %s.
MEMO_SEND_FAILED
	El mensaje no pudo ser enviado a %s.

# LIST responses
MEMO_LIST_SYNTAX
	LIST [Número | List | NEW]
MEMO_LIST_MEMOS
	Mensajes para %s.  Para leer, escriba: /msg %s READ Número
MEMO_LIST_NEW_MEMOS
	Nuevos mensajes para %s.  Para leer, escriba: /msg %s READ Número
MEMO_LIST_HEADER
	   Núm  Envia             Fecha/Hora
MEMO_LIST_FORMAT
	%c%c%c%3d  %-16s  %s

# READ responses
MEMO_READ_SYNTAX
	READ {Número | List | LAST | NEW}
MEMO_HEADER
	Mensaje %d de %s (%s).  Para borrar, escriba: /msg %s DEL %d
MEMO_CHAN_HEADER
	Mensaje %d de %s para %s (%s).  Para borrar, escriba: /msg %s DEL %d

# SAVE responses
MEMO_SAVE_SYNTAX
	SAVE {Número | List}
MEMO_SAVED_ONE
	Mensaje %d marcado para que no expire.
MEMO_SAVED_SEVERAL
	%d mensajes marcados para que no expiren.

# DEL responses
MEMO_DEL_SYNTAX
	DEL {Número | List | ALL}
MEMO_DELETED_NONE
	Ningún mensaje fue borrado.
MEMO_DELETED_ONE
	Mensaje %d ha sido borrado.
MEMO_DELETED_SEVERAL
	%d mensajes han sido borrados.
MEMO_DELETED_ALL
	Todos sus mensajes han sido borrados.

# RENUMBER responses
MEMO_RENUMBER_ONLY_YOU
	Solo puede usar el comando RENUMBER en usted mismo.
MEMO_RENUMBER_DONE
	Sus mensajes han sido re-enumerados.

# SET responses
MEMO_SET_SYNTAX
	SET Opción Parámetros
MEMO_SET_DISABLED
	Lo sentimos, la opción de ajuste de mensajes está temporalmente deshabilitada.
MEMO_SET_UNKNOWN_OPTION
	Opción SET %s desconocida.

# SET NOTIFY responses
MEMO_SET_NOTIFY_SYNTAX
	SET NOTIFY {ON | LOGON | NEW | OFF}
MEMO_SET_NOTIFY_ON
	%s le notificará ahóra de sus mensajes cuando se conecte o cuando le sean enviados.
MEMO_SET_NOTIFY_LOGON
	%s le notificará ahóra de sus mensajes cuando se conecte o cancele su modo /AWAY.
MEMO_SET_NOTIFY_NEW
	%s le notificará ahóra de sus mensajes cuando le sean enviados.
MEMO_SET_NOTIFY_OFF
	%s no le eviará notificación alguna de mensajes.

# SET LIMIT responses
MEMO_SET_LIMIT_SYNTAX
	SET LIMIT Límite
MEMO_SET_LIMIT_OPER_SYNTAX
	SET LIMIT [Apodo] {Límite | NONE} [HARD]
MEMO_SET_YOUR_LIMIT_FORBIDDEN
	No le es permitido cambiar su límite de mensajes.
MEMO_SET_LIMIT_FORBIDDEN
	El límite de mensajes para %s no puede ser cambiado.
MEMO_SET_YOUR_LIMIT_TOO_HIGH
	Usted no puede ajustar su límite de mensajes a más de %d.
MEMO_SET_LIMIT_TOO_HIGH
	Usted no puede ajustar el límite de mensajes para %s a más de %d.
MEMO_SET_LIMIT_OVERFLOW
	Límite de mensajes muy alto; lo hemos limitado a %d.
MEMO_SET_YOUR_LIMIT
	Su límite de mensajes ha sido ajustado a %d.
MEMO_SET_YOUR_LIMIT_ZERO
	No le será posible recibir más mensajes.
MEMO_SET_YOUR_LIMIT_DEFAULT
	Su límite de mensajes ha sido fijado a la Configuración Original (%d).
MEMO_UNSET_YOUR_LIMIT
	Su límite de mensajes ha sido dishabilitado.
MEMO_SET_LIMIT
	Límite de mensajes para %s ajustado a %d.
MEMO_SET_LIMIT_ZERO
	Límite de mensajes para %s ajustado a 0.
MEMO_SET_LIMIT_DEFAULT
	Límite de mensajes para %s fijado a la Configuración Original (%d).
MEMO_UNSET_LIMIT
	Límite de mensajes Dishabilitado para %s.

# INFO responses
MEMO_INFO_NO_MEMOS
	Actualmente usted no tiene mensajes.
MEMO_INFO_MEMO
	Actualmente usted tiene 1 mensaje.
MEMO_INFO_MEMO_UNREAD
	Actualmente usted tiene 1 mensaje, y no ha sido aún leído.
MEMO_INFO_MEMOS
	Actualmente usted tiene %d mensajes.
MEMO_INFO_MEMOS_ONE_UNREAD
	Actualmente usted tiene %d mensajes, de los cuales 1 no ha sido leído.
MEMO_INFO_MEMOS_SOME_UNREAD
	Actualmente usted tiene %d mensajes, de los cuales %d no han sido leídos.
MEMO_INFO_MEMOS_ALL_UNREAD
	Actualmente usted tiene %d mensajes; ninguno ha sido leído.
MEMO_INFO_LIMIT
	Su límite de mensajes es %d.
MEMO_INFO_HARD_LIMIT
	Su límite de mensajes es %d, y no puede ser cambiado.
MEMO_INFO_LIMIT_ZERO
	Su límite de mensajes es 0; usted no recibirá ningún nuevo mensaje.
MEMO_INFO_HARD_LIMIT_ZERO
	Su límite de mensajes es 0; usted no recibirá ningún nuevo mensaje.  Usted no puede cambiar este límite.
MEMO_INFO_NO_LIMIT
	Usted no tiene límite en la cantidad de mensajes que puede guardar.
MEMO_INFO_NOTIFY_OFF
	Usted no será notificado de nuevos mensajes.
MEMO_INFO_NOTIFY_ON
	Usted será notificado de nuevos mensajes al conectarse y cuando estos lleguen.
MEMO_INFO_NOTIFY_RECEIVE
	Usted será notificado cuando nuevos mensajes lleguen.
MEMO_INFO_NOTIFY_SIGNON
	Usted será notificado de nuevos mensajes al conectarse a la red de IRC.
MEMO_INFO_X_NO_MEMOS
	%s no tiene mensajes actualmente.
MEMO_INFO_X_MEMO
	%s actualmente tiene 1 mensaje.
MEMO_INFO_X_MEMO_UNREAD
	%s actualmente tiene 1 mensaje, y aún está por ser leído.
MEMO_INFO_X_MEMOS
	%s actualmente tiene %d mensajes.
MEMO_INFO_X_MEMOS_ONE_UNREAD
	%s actualmente tiene %d mensajes, de los cuales 1 está aún sin leer.
MEMO_INFO_X_MEMOS_SOME_UNREAD
	%s actualmente tiene %d mensajes, de los cuales %d están aún sin leer.
MEMO_INFO_X_MEMOS_ALL_UNREAD
	%s actualmente tiene %d mensajes; todos los cuales están sin leer.
MEMO_INFO_X_LIMIT
	El límite de mensages para %s es %d.
MEMO_INFO_X_HARD_LIMIT
	El límite de mensajes para %s es %d, y no puede ser cambiado.
MEMO_INFO_X_NO_LIMIT
	%s no tiene límite de mensajes.
MEMO_INFO_X_NOTIFY_OFF
	%s no es notificado de nuevos mensajes.
MEMO_INFO_X_NOTIFY_ON
	%s es notificado de nuevos mensajes al conectarse y cuando estos llegan.
MEMO_INFO_X_NOTIFY_RECEIVE
	%s es notificado cuando llegan nuevos mensajes.
MEMO_INFO_X_NOTIFY_SIGNON
	%s es notificado de nuevos mensajes al conectarse.

################ forward module messages/responses

# Mail format
MEMO_FORWARD_MAIL_SUBJECT
	Mensaje para %s
MEMO_FORWARD_MULTIPLE_MAIL_SUBJECT
	Mensajes para %s
MEMO_FORWARD_MAIL_BODY
	Mensaje para %s (%s)
	%s
MEMO_FORWARD_CHANMEMO_MAIL_BODY
	Mensaje de %s para %s (%s)
	%s

# FORWARD responses
MEMO_FORWARD_SYNTAX
	FORWARD {Número | List | ALL}
MEMO_FORWARD_NEED_EMAIL
	Usted debe fijar una dirección de correo electrónico (E-mail) antes de que los mensajes puedan ser reexpedidos a usted.
MEMO_FORWARD_PLEASE_WAIT
	Favor de esperar %s antes de usar el comando FORWARD nuevamente.
MEMO_FORWARD_FAILED
	Lo sentimos, el reexpedido de mensaje falló.
MEMO_FORWARDED_NONE
	Ningún mensaje fue reexpedido.
MEMO_FORWARDED_ONE
	Mensaje %d ha sido reexpedido.
MEMO_FORWARDED_SEVERAL
	%d mensajes han sido reexpedidos.
MEMO_FORWARDED_ALL
	Todos sus mensajes han sido reexpedidos.

# SET FORWARD responses
MEMO_SET_FORWARD_SYNTAX
	SET FORWARD {ON | COPY | OFF}
MEMO_SET_FORWARD_ON
	Sus mensajes serán ahóra reexpedidos a su dirección de correo electrónico: %s
MEMO_SET_FORWARD_COPY
	Sus mensajes serán ahóra reexpedidos a su dirección de correo electrónico: %s
	Sus mensajes ahóra también serán grabados por el sistema (Services).
MEMO_SET_FORWARD_OFF
	Sus mensajes no serán reexpedidos a su dirección de correo electrónico (E-mail).

################ ignore module messages/responses

# IGNORE responses
MEMO_IGNORE_SYNTAX
	IGNORE {ADD | DEL | LIST} [Apodo | Máscara]
MEMO_IGNORE_ADD_SYNTAX
	IGNORE ADD Apodo | Máscara
MEMO_IGNORE_DEL_SYNTAX
	IGNORE DEL Apodo | Máscara
MEMO_IGNORE_LIST_FULL
	Su lista de ignorar está llena.
MEMO_IGNORE_ALREADY_PRESENT
	%s ya está presente en su lista de ignorar.
MEMO_IGNORE_ADDED
	%s agregado a su lista de ignorar.
MEMO_IGNORE_NOT_FOUND
	%s no fue encontrado en su lista de ignorar.
MEMO_IGNORE_DELETED
	%s borrado de su lista de ignorar.
MEMO_IGNORE_LIST_EMPTY
	Su lista de ignorar está vacía.
MEMO_IGNORE_LIST
	Lista De Ignorar:
MEMO_IGNORE_LIST_X_EMPTY
	Lista de ignorar para %s está vacía.
MEMO_IGNORE_LIST_X
	Lista de ignorar para %s:

###########################################################################
#
# OperServ messages
#
###########################################################################

# General messages
OPER_BOUNCY_MODES
	Los servicios no pueden cambiar modos.  ¿Están sus servidores configurados correctamente?
OPER_BOUNCY_MODES_U_LINE
	Los servicios no pueden cambiar modos.  ¿Están las U:lines de sus servidores configurados correctamente?

# GLOBAL responses
OPER_GLOBAL_SYNTAX
	GLOBAL Notícia

# STATS responses
OPER_STATS_UNKNOWN_OPTION
	Opción STATS %s desconocida.
OPER_STATS_RESET_USER_COUNT
	La cantidad máxima de usuarios ha sido re-ajustada.

OPER_STATS_CURRENT_USERS
	Usuarios actuales: %d (%d ops)
OPER_STATS_MAX_USERS
	Máximo de usuarios: %d (%s)
# Parameters: days, STR_DAYS or STR_DAY, hours, minutes
OPER_STATS_UPTIME_DHM
	Servicios activos %d%s, %02d:%02d
# If >= 1 hour, hours and minutes; if < 1 hour, minutes and seconds
OPER_STATS_UPTIME_HM_MS
	Servicios activos %s

OPER_STATS_KBYTES_READ
	Data recibida: %10u kB
OPER_STATS_KBYTES_WRITTEN
	Data enviada:  %10u kB
OPER_STATS_NETBUF_SOCK
	Prelecturas de enchufe de servidor: %5u kB
OPER_STATS_NETBUF_SOCK_PERCENT
	Prelecturas de Enchufe de servidor: %5u kB (%d%%)
OPER_STATS_NETBUF_TOTAL
	Total de prelecturas de enchufe:    %5u kB
OPER_STATS_NETBUF_TOTAL_PERCENT
	Total de prelecturas de enchufe:    %5u kB (%d%%)

OPER_STATS_ALL_USER_MEM
	Usuarios           : %6d regístros, %5d kB
OPER_STATS_ALL_CHANNEL_MEM
	Canales            : %6d regístros, %5d kB
OPER_STATS_ALL_SERVER_MEM
	Servidores         : %6d regístros, %5d kB
OPER_STATS_ALL_NICKGROUPINFO_MEM
	Grupos De Apodos   : %6d regístros, %5d kB
OPER_STATS_ALL_NICKINFO_MEM
	Apodos             : %6d regístros, %5d kB
OPER_STATS_ALL_CHANSERV_MEM
	ChanServ           : %6d regístros, %5d kB
OPER_STATS_ALL_STATSERV_MEM
	StatServ           : %6d regístros, %5d kB
OPER_STATS_ALL_NEWS_MEM
	Notícias        : %6d regístros, %5d kB
OPER_STATS_ALL_AKILL_MEM
	Auto-desconecciones: %6d regístros, %5d kB
OPER_STATS_ALL_EXCEPTION_MEM
	Exceptions         : %6d regístros, %5d kB
OPER_STATS_ALL_SGLINE_MEM
	SGlines            : %6d regístros, %5d kB
OPER_STATS_ALL_SQLINE_MEM
	SQlines            : %6d regístros, %5d kB
OPER_STATS_ALL_SZLINE_MEM
	SZlines            : %6d regístros, %5d kB
OPER_STATS_ALL_SESSION_MEM
	Sesiones           : %6d regístros, %5d kB

# GETKEY responses
OPER_GETKEY_SYNTAX
	GETKEY Canal
OPER_GETKEY_KEY_IS
	La llave para el canal %s es: %s
OPER_GETKEY_NO_KEY
	El canal %s no tiene fijada una llave.

# MODE responses
OPER_MODE_SYNTAX
	MODE Canal Modos

# CLEARMODES responses
OPER_CLEARMODES_SYNTAX
	CLEARMODES Canal [ALL]
OPER_CLEARMODES_DONE
	Todos los modos de canal han sido removidos del canal %s.
OPER_CLEARMODES_ALL_DONE
	Todos los modos de canal y usuario han sido removidos del canal %s.

# CLEARCHAN responses
OPER_CLEARCHAN_SYNTAX
	CLEARCHAN Canal
OPER_CLEARCHAN_DONE
	Todos los usuarios removidos del canal %s.

# KICK responses
OPER_KICK_SYNTAX
	KICK Canal Usuario Razón

# ADMIN responses
OPER_ADMIN_SYNTAX
	ADMIN {ADD | DEL | LIST} [Apodos]
OPER_ADMIN_ADD_SYNTAX
	ADMIN ADD Apodo
OPER_ADMIN_DEL_SYNTAX
	ADMIN DEL Apodo
OPER_ADMIN_NO_NICKSERV
	NickServ está deshabilitado en este momento; el comando ADMIN no está disponible.
OPER_ADMIN_EXISTS
	%s ya existe en la lista de Administradores de Servicios.
OPER_ADMIN_ADDED
	%s agregado a la lista de Administradores de Servicios.
OPER_ADMIN_TOO_MANY
	Demasiadas entradas (%d) en la lista de Administradores de Servicios; imposible agregar más.
OPER_ADMIN_REMOVED
	%s removido de la lista de Administradores de Servicios.
OPER_ADMIN_NOT_FOUND
	%s no encontrado en la lista de administradores de Servicios.
OPER_ADMIN_LIST_HEADER
	Lista de Adminisradores de Servicios:

# OPER responses
OPER_OPER_SYNTAX
	OPER {ADD | DEL | LIST} [Apodo]
OPER_OPER_ADD_SYNTAX
	OPER ADD Apodo
OPER_OPER_DEL_SYNTAX
	OPER DEL Apodo
OPER_OPER_NO_NICKSERV
	NickServ está deshabilitado en este momento; el comando OPER no está disponible.
OPER_OPER_EXISTS
	%s ya existe en la lista de Operadores de Servicios.
OPER_OPER_ADDED
	%s agregado a la lista de Operadores de Servicios.
OPER_OPER_TOO_MANY
	Demasiadas entradas (%d) en la lista de Operadores de Servicios; imposible agregar más.
OPER_OPER_REMOVED
	%s removido de la lista de Operadores de Servicios.
OPER_OPER_NOT_FOUND
	%s no encontrado en la lista de Operadores de Servicios.
OPER_OPER_LIST_HEADER
	Lista de Operadores de Servicios:

# AKILL/EXCLUDE/SxLINE common responses
OPER_MASKDATA_SYNTAX
	%s {ADD | DEL | CLEAR | LIST | VIEW | CHECK | COUNT} [Parámetros]
OPER_MASKDATA_ADD_SYNTAX
	%s ADD [+Expiración] Máscara Razón
OPER_MASKDATA_DEL_SYNTAX
	%s DEL Máscara
OPER_MASKDATA_CLEAR_SYNTAX
	%s CLEAR Máscara
OPER_MASKDATA_LIST_SYNTAX
	%s {LIST | VIEW} [+Inicio] [Patrón] [NOEXPIRE]
OPER_MASKDATA_CHECK_SYNTAX
	%s CHECK Máscara
# mask, reason
OPER_MASKDATA_LIST_FORMAT
	%-32s  %s
# mask, set-by, set-time, last-used, expires, reason
OPER_MASKDATA_VIEW_FORMAT
	%s (por %s en %s, última vez usada %s; %s)
	    %s
# mask, set-by, set-time, expires, reason
OPER_MASKDATA_VIEW_UNUSED_FORMAT
	%s (por %s en %s, nunca usada; %s)
	    %s

# AKILL responses
OPER_TOO_MANY_AKILLS
	¡La lista AKILL está llena!
OPER_AKILL_EXISTS
	%s ya existe en la lista de auto-desconexión (AKILL).
OPER_AKILL_NO_NICK
	Nota: Las máscaras de auto-desconexión (AKILL) no pueden contener apodos; asegurese que usted no ha incluido una porción con apodo en su máscara.
OPER_AKILL_MASK_TOO_GENERAL
	La máscara de auto-desconexión (AKILL) es demasiado general.
OPER_AKILL_EXPIRY_LIMITED
	Solamente es posible fijar una fecha de expiración de %s.
OPER_AKILL_ADDED
	%s agregado a la lista de auto-desconexión (AKILL).
OPER_AKILL_REMOVED
	%s removido de la lista de auto-desconexión (AKILL).
OPER_AKILL_CLEARED
	La lista de auto-desconexión (AKILL) ha sido eliminada.
OPER_AKILL_NOT_FOUND
	%s no encontrado en la lista de auto-desconexión (AKILL).
OPER_AKILL_LIST_HEADER
	Lista actual de auto-desconexión (AKILL):
OPER_AKILL_LIST_EMPTY
	La lista de auto-desconexión (AKILL) está vacía.
OPER_AKILL_LIST_NO_MATCH
	No se encontraron máscaras similares en la lista de auto-desconexión (AKILL).
OPER_AKILL_CHECK_NO_MATCH
	No se encontraron máscaras similares a %s en la lista de auto-desconexión (AKILL).
OPER_AKILL_CHECK_HEADER
	Máscaras similares a %s en la lista de auto-desconexión (AKILL):
OPER_AKILL_CHECK_TRAILER
	%d máscaras similares fueron encontradas.
OPER_AKILL_COUNT
	Existen %d máscaras de proveedor web en la lista de auto-desconexión (AKILL).

# AKILLCHAN responses
OPER_AKILLCHAN_SYNTAX
	AKILLCHAN [KILL] [+Expiración] Canal Razón
OPER_AKILLCHAN_AKILLED
	%d usuarios fueron agregados a la lista de auto-desconexión (AKILL).
OPER_AKILLCHAN_KILLED
	%d usuarios fueron desconectados.
OPER_AKILLCHAN_AKILLED_ONE
	1 usuario fue agregado a la lista de auto-desconexión (AKILL).
OPER_AKILLCHAN_KILLED_ONE
	1 usuario fue auto-desconectado.

# EXCLUDE responses
OPER_TOO_MANY_EXCLUDES
	¡La lista EXCLUDE está llena!
OPER_EXCLUDE_EXISTS
	%s ya existe en la lista de exclusión de auto-desconexión (AKILL).
OPER_EXCLUDE_ADDED
	%s agregado a la lista de exclusión de auto-desconexión (AKILL).
OPER_EXCLUDE_REMOVED
	%s removido de la lista de exclusión de auto-desconexión (AKILL).
OPER_EXCLUDE_CLEARED
	La lista de exclusión de auto-desconexión (AKILL) ha sido limpiada.
OPER_EXCLUDE_NOT_FOUND
	%s no fue encontrado en la lista de exclusión de auto-desconexión (AKILL).
OPER_EXCLUDE_LIST_HEADER
	Lista actual de exclusión de auto-desconexión (AKILL):
OPER_EXCLUDE_LIST_EMPTY
	La lista de exclusión de auto-desconexión (AKILL) está vacía.
OPER_EXCLUDE_LIST_NO_MATCH
	No se encontraron máscaras similares en la lista de exclusión de auto-desconexión (AKILL).
OPER_EXCLUDE_CHECK_NO_MATCH
	No se encontraron máscaras similares a %s en la lista de exclusión de auto-desconexión (AKILL).
OPER_EXCLUDE_CHECK_HEADER
	Máscaras similares a %s en la lista de exclusión de auto-desconexión (AKILL):
OPER_EXCLUDE_CHECK_TRAILER
	Se encontraron %d máscaras similares.
OPER_EXCLUDE_COUNT
	Existen %d máscaras de proveedor web en la lista de exclusión de auto-desconexión (AKILL).

# SxLINE responses
OPER_TOO_MANY_SLINES
	¡La lista %s está llena!
OPER_SLINE_EXISTS
	%s ya exixte en la lista %s.
OPER_SLINE_MASK_TOO_GENERAL
	La máscara de %s es demasiado general.
OPER_SLINE_ADDED
	%s agregado a la lista %s.
OPER_SLINE_REMOVED
	%s removido de la lista %s.
OPER_SLINE_CLEARED
	La lista %s ha sido limpiada.
OPER_SLINE_NOT_FOUND
	%s no fue encontrado en la lista %s.
OPER_SLINE_LIST_HEADER
	Lista %s actual:
OPER_SLINE_LIST_EMPTY
	La lista %s está vacía.
OPER_SLINE_LIST_NO_MATCH
	No se encontraron máscaras %s en la lista de auto-desconexión (Autokill).
OPER_SLINE_CHECK_NO_MATCH
	No se encontraron máscaras similares a %s en la lista %s.
OPER_SLINE_CHECK_HEADER
	Máscaras similares a %s en la lista %s:
OPER_SLINE_CHECK_TRAILER
	Se encontraron %d máscaras similares.
OPER_SLINE_COUNT
	Existen %d entradas en la lista %s.
OPER_SZLINE_NOT_AVAIL
	SZLINE no está disponible con este servidor de IRC.

# SU responses
OPER_SU_SYNTAX
	SU Contraseña
OPER_SU_NO_PASSWORD
	El comando SU no está disponible porque ninguna contraseña de super-usuario ha sido fijada.
OPER_SU_SUCCEEDED
	Usted tiene en este momento privilegios de super-usuario de los servicios.
OPER_SU_FAILED
	Fue imposible verificar la contraseña.

# SET responses
OPER_SET_SYNTAX
	SET Opción Ajuste
OPER_SET_IGNORE_ON
	Código Ignore será usado.
OPER_SET_IGNORE_OFF
	Código Ignore no será usado.
OPER_SET_IGNORE_ERROR
	Parámetro para IGNORE debe ser ON o OFF.
OPER_SET_READONLY_ON
	Los Servicios están ahóra en modo de Leer-solamente (Read-Only).
OPER_SET_READONLY_OFF
	Los Servicios están ahóra en modo de Leer-Escribir (Read-Write).
OPER_SET_READONLY_ERROR
	Parámetro para READONLY debe ser ON u OFF.
OPER_SET_DEBUG_ON
	Los Servicios están ahóra en modo debug (Detección De Errores).
OPER_SET_DEBUG_OFF
	Los Servicios están ahóra en modo de non-debug (No Detectando Errores).
OPER_SET_DEBUG_LEVEL
	Los Servicios están ahóra en modo de debug (nivel %d).
OPER_SET_DEBUG_ERROR
	Parámetro para DEBUG debe ser ON, OFF, o un número positivo.
OPER_SET_SUPASS_FAILED
	Imposible fijar la contraseña para el Super-Usuario de los servicios.
OPER_SET_SUPASS_OK
	Contraseña para el Super-Usuario de los servicios ha sido cambiada.
OPER_SET_SUPASS_NONE
	Contraseña para el Super-Usuario de los servicios ha sido dishabilitada.
OPER_SET_UNKNOWN_OPTION
	Opción %s desconocida.

# JUPE responses
OPER_JUPE_SYNTAX
	JUPE Nombre-Del-Servidor [Razón]
OPER_JUPE_INVALID_NAME
	El nombre de servidor es inválido (los nombres de servidores deben contener un punto).
OPER_JUPE_ALREADY_JUPED
	Ya se ha ejecutado el comando JUPE en el servidor %s.

# RAW responses
OPER_RAW_SYNTAX
	RAW Texto

# UPDATE responses
OPER_UPDATE_SYNTAX
	UPDATE [FORCE]
OPER_UPDATE_FORCE_FAILED
	Fue imposible abrir las bases de datos.
OPER_UPDATING
	Actualizando las bases de datos...
OPER_UPDATE_COMPLETE
	La actualización de la base de datos ha completado.
OPER_UPDATE_FAILED
	La actualización de la base de datos falló.

# REHASH responses
OPER_REHASHING
	Re-leyendo los ajustes a la configuración.
OPER_REHASHED
	Todos los ajustes a la configuración fueron exitosamente actualizados.
OPER_REHASH_ERROR
	Un error occurrio mientras se re-leian los ajustes a la configuración.  Favor examine el fichero de archivo de los Servicios (log) para más detalles.

# KILLCLONES responses
OPER_KILLCLONES_SYNTAX
	KILLCLONES Apodo
OPER_KILLCLONES_UNKNOWN_NICK
	Imposible encontrar el usuario %s.
OPER_KILLCLONES_KILLED
	%s utilizo KILLCLONES para %s desconectando %d clones.
OPER_KILLCLONES_KILLED_AKILL
	%s utilizo KILLCLONES para %s desconectando %d clones.  Un AKILL temporario ha sido agregado para %s.


###########################################################################
#
# Session limiting and exception messages
#
###########################################################################

# EXCEPTION responses
OPER_EXCEPTION_SYNTAX
	EXCEPTION {ADD | DEL | CLEAR | MOVE | LIST | VIEW | CHECK | COUNT} [Parámetros]
OPER_EXCEPTION_ADD_SYNTAX
	EXCEPTION ADD [+Expiración] Máscara Límite Razón
OPER_EXCEPTION_DEL_SYNTAX
	EXCEPTION DEL {Máscara | List}
OPER_EXCEPTION_CLEAR_SYNTAX
	EXCEPTION CLEAR ALL
OPER_EXCEPTION_MOVE_SYNTAX
	EXCEPTION MOVE Núm Nuevo-Núm
OPER_EXCEPTION_LIST_SYNTAX
	EXCEPTION {LIST | VIEW} [[+Inicio] Patrón | lista]
OPER_EXCEPTION_ALREADY_PRESENT
	La máscara %s ya está presente en la lista de excepción.
OPER_EXCEPTION_TOO_MANY
	¡La lista de excepción para límite de sesiones está llena!
OPER_EXCEPTION_ADDED
	Límite de sesiones para %s fijado a %d.
OPER_EXCEPTION_MOVED
	Excepción para %s (#%d) movido al número %d.
OPER_EXCEPTION_NO_SUCH_ENTRY
	Ninguna entrada (#%d) existe en la lista de excepción para límite de sesiones.
OPER_EXCEPTION_NOT_FOUND
	%s no encontrado en la lista de excepción para límite de sesiones.
OPER_EXCEPTION_NO_MATCH
	Ninguna entrada existe que concuerde en la lista de excepción para límite de sesiones.
OPER_EXCEPTION_EMPTY
	La lista de excepción para límite de sesiones está vacía.
OPER_EXCEPTION_DELETED
	%s borrado de la lista de excepción para límite de sesiones.
OPER_EXCEPTION_DELETED_ONE
	Borrada 1 entrada de la lista de excepción para límite de sesiones.
OPER_EXCEPTION_DELETED_SEVERAL
	Borradas %d entradas de la lista de excepción para límite de sesiones.
OPER_EXCEPTION_CLEARED
	La lista de excepción para límite de sesiones ha sido limpiada.
OPER_EXCEPTION_LIST_HEADER
	Lista de excepción para límite de sesiones actual:
# number, limit, host
OPER_EXCEPTION_LIST_COLHEAD
	Núm  Límite  De
OPER_EXCEPTION_LIST_FORMAT
	%3d  %4d   %s
# number, mask, set-by, set-time, expires, limit, reason
OPER_EXCEPTION_VIEW_FORMAT
	%3d.  %s  (por %s on %s; %s)
	    Límite: %-4d  - %s
OPER_EXCEPTION_CHECK_SYNTAX
	EXCEPTION CHECK Máscara
OPER_EXCEPTION_CHECK_NO_MATCH
	No se encontraron máscaras similares a %s en la lista de excepción para límite de sesiones.
OPER_EXCEPTION_CHECK_HEADER
	Máscaras similares a %s en la lista de excepción para límite de sesiones:
OPER_EXCEPTION_CHECK_TRAILER
	Se encontraron %d máscaras similares.
OPER_EXCEPTION_COUNT
	Existen %d entradas en la lista de excepción para límite de sesiones.
OPER_EXCEPTION_INVALID_LIMIT
	Límite de sesión inválido; límite debe ser al menos cero y no mayor que %d.
OPER_EXCEPTION_INVALID_HOSTMASK
	Máscara proveedor-web inválida. Solo máscaras de proveedor-web reales son válidas ya que excepciones no son relacionadas a apodos o nombres de usuario.

# SESSION responses
OPER_SESSION_SYNTAX
	SESSION {LIST Límite | VIEW Máscara-web}
OPER_SESSION_LIST_SYNTAX
	SESSION LIST Límite
OPER_SESSION_VIEW_SYNTAX
	SESSION VIEW Máscara-Web
OPER_SESSION_INVALID_THRESHOLD
	Valor límite inválido; límite debe ser un entero mayor que 1.
OPER_SESSION_NOT_FOUND
	%s no encontrado en la lista de sesión.
OPER_SESSION_LIST_HEADER
	Proveedores con al menos %d sesiones:
OPER_SESSION_LIST_COLHEAD
	Sesiones  Proveedor
# Límite, Proveedor
OPER_SESSION_LIST_FORMAT
	%6d    %s
# provee, sesiones, límite
OPER_SESSION_VIEW_FORMAT
	El proveedor %s actualmente tiene %d sesiones con un límite de %d.


###########################################################################
#
# News system messages
#
###########################################################################

NEWS_LOGON_TEXT
	[Notícias - %s] %s
NEWS_OPER_TEXT
	[Notícias De Operador - %s] %s

NEWS_LOGON_SYNTAX
	LOGONNEWS {ADD | DEL | LIST} [Texto | Núm]
NEWS_LOGON_LIST_HEADER
	Listado De Notícias:
NEWS_LOGON_LIST_ENTRY
	%5d (%s por %s)
	    %s
NEWS_LOGON_LIST_NONE
	No existen artículos noticiosos.
NEWS_LOGON_ADD_SYNTAX
	LOGONNEWS ADD Texto
NEWS_LOGON_ADD_FULL
	¡La lista de notícias está llena!
NEWS_LOGON_ADDED
	Nuevo artículo notícioso agregado (#%d).
NEWS_LOGON_DEL_SYNTAX
	LOGONNEWS DEL {Núm | ALL}
NEWS_LOGON_DEL_NOT_FOUND
	¡El artículo notícioso #%d no fue encontrado!
NEWS_LOGON_DELETED
	Artículo notícioso  #%d borrado.
NEWS_LOGON_DEL_NONE
	¡No existe artículo notícioso alguno que borrar!
NEWS_LOGON_DELETED_ALL
	Todos los artículo notícioso borrados.

NEWS_OPER_SYNTAX
	OPERNEWS {ADD | DEL | LIST} [Texto | Núm]
NEWS_OPER_LIST_HEADER
	Artículos notíciosos para operadores:
NEWS_OPER_LIST_ENTRY
	%5d (%s por %s)
	    %s
NEWS_OPER_LIST_NONE
	No existen artículos noticiosos para los Operadores.
NEWS_OPER_ADD_SYNTAX
	OPERNEWS ADD Texto
NEWS_OPER_ADD_FULL
	¡La lista de notícias está llena!
NEWS_OPER_ADDED
	Nuevo artículo notícioso de Operadores agregado (#%d).
NEWS_OPER_DEL_SYNTAX
	OPERNEWS DEL {Num | ALL}
NEWS_OPER_DEL_NOT_FOUND
	¡El artículo notícioso de Operadores #%d no fue encontrado!
NEWS_OPER_DELETED
	Artículo notícioso de Operadores #%d borrado.
NEWS_OPER_DEL_NONE
	¡No existe artículo notícioso de Operadores alguno que borrar!
NEWS_OPER_DELETED_ALL
	Todos los artículos notíciosos de Operadores borrados.

NEWS_HELP_LOGON
	Sintaxis: LOGONNEWS ADD Texto
	        LOGONNEWS DEL {Núm | ALL}
	        LOGONNEWS LIST
	
	Edita o muestra la lista de artículos notíciosos.  Cuando los
	usuarios se conectan a la red, estos mensajes serán enviados
	a ellos.  (Sin embargo, no más de tres mensajes les serán
	enviados para asi evitar un ahógo (flood) del usuario.  Si hay
	más de tres mensajes notíciosos, solo los tres más recientes
	serán enviados.)
	
	LOGONNEWS LIST puede ser usado por cualquier operador de IRC para
	listar los mensajes notíciosos actuales.  ADD y DEL solo pueden ser
	usados por los Operadores de los Servicios.

NEWS_HELP_OPER
	Sintaxis: OPERNEWS ADD Texto
	        OPERNEWS DEL {Núm | ALL}
	        OPERNEWS LIST
	
	Edita o muestra la lista de mensajes notíciosos de operadores.
	Cuando un usuario se hace Operador (con el comando /OPER), estos
	mensajes le serán enviados.  (Sin embargo, no más de tres mensajes
	serán enviados asi evitando un ahógo (flood) del usuario.  Si hay
	más de tres mensajes notíciosos, solo los tres más recientes
	serán enviados.)
	
	OPERNEWS LIST puede ser usado por cualquier operador de IRC para
	listar los mensajes notíciosos al Operador actuales.  ADD y DEL solo
	pueden ser usandos por los Operadores de los Servicios.


###########################################################################
#
# StatServ messages
#
############################################################################

# General SERVERS responses
STAT_SERVERS_REMOVE_SERV_FIRST
	Servidor %s está en línea. Remuevalo de la red y trate nuevamente.
STAT_SERVERS_SERVER_EXISTS
	Servidor %s ya existe.

# SERVERS command syntax
STAT_SERVERS_SYNTAX
	SERVERS Opción Parámetros

# STATS responses
STAT_SERVERS_STATS_TOTAL
	Total de Servidores: %2d
STAT_SERVERS_STATS_ON_OFFLINE
	       En línea: %2d (%d%%)  No en línea: %2d (%d%%)
STAT_SERVERS_LASTQUIT_WAS
	    Última Desconexión: %-30s  %s

# LIST responses
STAT_SERVERS_LIST_HEADER
	Servidor                       Usuarios   Operadores
STAT_SERVERS_LIST_FORMAT
	%-30s %3d (%2d%%)  %3d (%2d%%)
STAT_SERVERS_LIST_RESULTS
	Fín del listado -- %d/%d servidores en línea mostrados.

# VIEW responses
STAT_SERVERS_VIEW_HEADER_ONLINE
	%-30s  (En línea)
STAT_SERVERS_VIEW_HEADER_OFFLINE
	%-30s  (Fuera de línea)
STAT_SERVERS_VIEW_LASTJOIN
	    Última conexión: %s
STAT_SERVERS_VIEW_LASTQUIT
	    Última desconexión: %s
STAT_SERVERS_VIEW_QUITMSG
	 Mensaje de salida: %s
STAT_SERVERS_VIEW_USERS_OPERS
	        Usuarios: %3d (%2d%%)  Operadores: %3d (%2d%%)
STAT_SERVERS_VIEW_RESULTS
	Fín del listado -- %d/%d servidores mostrados.

# DELETE responses
STAT_SERVERS_DELETE_SYNTAX
	SERVERS DELETE Servidor
STAT_SERVERS_DELETE_DONE
	Estadísticas para el servidor %s han sido borradas.

# COPY responses
STAT_SERVERS_COPY_SYNTAX
	SERVERS COPY Servidor Nuevo-Nombre
STAT_SERVERS_COPY_DONE
	Estadísticas para el servidor %s copiadas a %s.

# RENAME responses
STAT_SERVERS_RENAME_SYNTAX
	SERVERS RENAME Servidor Nuevo-Nombre
STAT_SERVERS_RENAME_DONE
	Estadísticas para el servidor %s renombradas a %s.

# USERS responses
STAT_USERS_SYNTAX
	USERS STATS
STAT_USERS_TOTUSERS
	             Total de usuarios: %d
STAT_USERS_TOTOPERS
	             Total de operadores: %d
STAT_USERS_SERVUSERS
	Average de usuarios por servidor: %d
STAT_USERS_SERVOPERS
	Average de operadores por servidor: %d.%d


###########################################################################
#
# NickServ help messages
#
###########################################################################

NICK_HELP
	%S le permite "registrar" un apodo y previene el que
	otros hagan uso de él.  %S es controlado por medio de
	varios comandos que hacen posible la registración y también el
	mantenimiento de apodos.  Para ver la lista de comandos, escriba
	/msg %S HELP COMMANDS; para utilizar un comando, escriba
	/msg %S Comando, y para más información sobre un comando
	específico, escriba /msg %S HELP Comando.
NICK_HELP_EXPIRES
	
	Si un apodo registrado no es utilizado por %s, el
	regístro expirará y el apodo estará  disponible
	para que cualquier persona lo vuelva a utilizar.
NICK_HELP_WARNING
	
	NOTA: Este servicio está diseñado para proveer una forma en la
	que los usuarios de IRC se aseguran de que su identidad no sea
	comprometida.  Su intención NO es para facilitar el "robo"
	de apodos u otras acciones mal intencionadas.  Abuso de
	%S resultará, por lo menos, en la pérdida de el/los
	apodo(s) abusado(s).

NICK_HELP_COMMANDS
	Los siguientes comandos pueden ser utilizados con %S:
	
	    REGISTER     Registre un apodo
	    IDENTIFY     Identifiquese con su contraseña
	    DROP         Cancele el regístro de un apodo
NICK_HELP_COMMANDS_AUTH
	    AUTH         Autentique el regístro de un apodo
	    SENDAUTH     Envie a sí mismo el código de autenticación de un apodo
	    REAUTH       Genere un nuevo código si usted olvido su contraseña
	    RESTOREMAIL  Restaure la última dirección autenticada de correo electrónico
NICK_HELP_COMMANDS_LINK
	    LINK         Ligue otro apodo al suyo
	    UNLINK       Cancele un apodo ligado previamente
	    LISTLINKS    Aliste todos los apodos que usted ha ligado
NICK_HELP_COMMANDS_ACCESS
	    ACCESS       Modifique la lista de direcciones autorizadas
NICK_HELP_COMMANDS_AJOIN
	    AJOIN        Modifique su lista de auto-ingreso (autojoin)
NICK_HELP_COMMANDS_SET
	    SET          Ajuste opciones, incluyendo protección de kill
	    UNSET        Re-configure la información del apodo
	    RECOVER      Desconecte otro usuario que este usando su apodo
	    RELEASE      Tome custodia de su apodo después de utilizar RECOVER
	    GHOST        Desconecte una sesión "fantasma"
	    INFO         Muestra información sobre un apodo
NICK_HELP_COMMANDS_LIST
	    LIST         Alista todos los apodos que concuerden con una máscara dada
	    LISTEMAIL    Alista apodos segun su dirección electrónica (E-Mail)
NICK_HELP_COMMANDS_LISTCHANS
	    LISTCHANS    Alista todos los canales que usted posee
	    STATUS       Asegurese si se han identificado correctamente por un apodo dado

NICK_HELP_REGISTER
	Sintaxis: %s
	
	Registra su apodo en la base de datos de %S.  Una vez registrado
	su apodo, usted puede utilizar los comandos SET y ACCESS
	para configurar los parámetros de su apodo como usted los desee.
	Asegúrese de recordar la contraseña que utilizó para
	registrarse--la necesitará para hacer cambios a su apodo
	posteriormente.  (¡Nótese que las mayúsculas importan
	FIDO, Fido, y fido son todas diferentes contraseñas!)
NICK_HELP_REGISTER_EMAIL
	
	Usted puede incluír una dirección de correo electrónico (e-mail)
	cuando registre su apodo; también puede fijar una más tarde usando
	el comando SET EMAIL.
NICK_HELP_REGISTER_EMAIL_REQ
	
	Usted debe incluír una dirección de correo Electrónico (e-mail)
	cuando registre su apodo.  Usted puede prevenir que su correo
	electrónico sea mostrado a otros usuarios con el comando SET HIDE
	(Escriba /msg %S HELP SET HIDE para más información).
NICK_HELP_REGISTER_EMAIL_AUTH
	
	Usted debe incluír una dirección válida de correo electrónico
	cuando registre su apodo; %S enviará un mensaje con un código
	de autenticación a esta dirección, el cual usted deberá usar con
	el comando AUTH para completar su registración.  Usted puede
	prevenir que su correo electrónico (E-mail) sea mostrado a otros
	usuarios con el comando SET HIDE (Escriba /msg %S HELP SET HIDE
	para más información).
NICK_HELP_REGISTER_END
	
	Sugerencias para escoger contraseñas:
	
	Contraseñas no deben ser fácil de adivinar.  Por ejemplo,
	usar su verdadero nombre como contraseña es una mala idea.
	Usar su apodo como contraseña es una idea peor ;) y, por
	supuesto que, %S no lo permitirá.  También, contraseñas
	cortas son vulnerables a búsquedas de prueba y error, por lo
	que usted debe escoger una contraseña de por lo menos 5
	caracteres.

NICK_HELP_IDENTIFY
	Sintaxis: IDENTIFY Contraseña
	
	Notifica a %S que usted es el verdadero dueño de este
	apodo.  Muchos de los comandos requieren que usted se
	identifique con este comando antes de poder utilizarlos.  La
	contraseña debe ser la misma que utilizó con el comando
	REGISTER.

NICK_HELP_DROP
	Sintaxis: DROP Contraseña
	
	Cancela (borra) su apodo de la base de datos de %S.
	Un apodo que ha sido borrado está disponible para que
	cualquier persona lo vuelva a registrar.
NICK_HELP_DROP_LINK
	
	Note que cuando usted cancele un apodo, todos los apodos ligados a este
	serán borrados también.  Para cancelar la liga de solamente un apodo,
	utilice el comando UNLINK.
NICK_HELP_DROP_END
	
	Para prevenir el uso accidental de este comando, usted debe entrar
	su contraseña para poder cancelar (borrar) su apodo.

NICK_HELP_AUTH
	Sintaxis: AUTH Código
	
	Autentica la registración o cambio de correo para un apodo.
	Para asegurarse que su correo electrónico es correcto, %S
	enviará un mensaje que contendrá un código de autenticación
	a la dirección electrónica dada cuando registre su apodo o
	cambie su dirección electrónica con el comando SET EMAIL.
	Este comando le permite a usted confirmar que ha recibido el
	mensaje y continuar usando su apodo normalmente.  Reemplace
	Código en el comando con el código de autenticación de nueve
	dígitos especificado en el mensaje electrónico (E-mail).
	
NICK_HELP_SENDAUTH
	Sintaxis: SENDAUTH
	
	Le envia a usted una copia del código de autenticación para
	su apodo despues de un REGISTER o SET EMAIL.  Si por accidente
	usted borra o pierde el mensaje con el código de autenticación,
	usted puede utilizar este comando para recibir una copia extra.
	
	Para prevenir abuso, este comando solo puede ser usado una vez
	cada 24 horas.

NICK_HELP_REAUTH
	Sintaxis: REAUTH
	
	Establece un código de autenticación el cual usted puede usar
	para identificarse por su apodo.  Si se olvida de la contraseña
	de su apodo, use este comando para generar un nuevo código de
	autenticación, entonces provea este código (el cual le será
	enviado) con el comando AUTH para identificarse por su apodo;
	despues le será posible cambiar la contraseña con el comando
	SET PASSWORD como se hace usualmente.

NICK_HELP_RESTOREMAIL
	Sintaxis: RESTOREMAIL Contraseña
	
	Recobra la última dirección de correo electrónico autenticada
	para su apodo.  Si usted accidentalmente indica una dirección
	erronea con el comando SET EMAIL, puede utilizar este comando
	para recobrar la dirección de correo electrónico usada
	previamente y eliminar el código de autenticación para su
	apodo, permitiendole a usted identificarse por el apodo
	nuevamente.  Usted debe proveer la contraseña para su apodo
	con este comando.
	
	RESTOREMAIL solo puede ser usado despues de SET EMAIL y antes
	de entrar el nuevo código de autenticación con el comando AUTH.

NICK_HELP_LINK
	Sintaxis: LINK Apodo
	
	Liga un nuevo apodo al apodo que usted está usando, creando
	asi un nuevo "alias" para su apodo.  El nuevo apodo será
	equivalente a su apodo actual en terminos de privilegios de
	acceso y mensajes; por ejemplo, usted puede leer mensajes
	enviados a uno de sus apodos ligados mientras usa cualquiera
	de sus otros apodos, y si uno de sus apodos tiene privilegios
	de auto-op en un canal, usted obtendrá estatus de operador sin
	importar cual de sus apodos ligados esté utilizando.
	
	Para cancelar una liga, use el comando UNLINK.
	
	Para poder utilizar este comando, usted debe identificarse por
	su apodo actual (usando el comando IDENTIFY), ademas el apodo
	que será ligado no puede estar registrado o encontrarse en uso
	en este momento.

NICK_HELP_UNLINK
	Sintaxis: UNLINK Apodo
	
	Remueve una liga (alias) de su apodo creada con el comando
	LINK.  Usted no puede desligar el apodo que está utilizando
	actualmente; cambiese a otro de sus apodos ligados primeramente
	antes de usar este comando.
	
	Si usted desliga el "main nick" (Apodo Principal) que seleccionó
	usando el comando SET MAINNICK, su apodo actual será automáticamente
	seleccionado como el apodo principal (main nick).

NICK_HELP_LISTLINKS
	Sintaxis: LISTLINKS
	
	Alista todos los apodos que esten ligados a su apodo.  El apodo
	principal para el grupo de apodos ligados será indicado por un
	signo "*" a la izquierda del apodo.

NICK_HELP_ACCESS
	Sintaxis: ACCESS ADD Máscara
	          ACCESS DEL Máscara
	          ACCESS LIST
	
	Modifica o muestra la lista de acceso para su apodo.  Esta es la
	lista de direcciones usuario@proveedor que serán automáticamente
	reconocidas por %S como autorizadas para usar el apodo.  Si usted
	se conecta a IRC con una dirección incluída en dicha lista, usted no
	será afectado por la configuración SET KILL fijada en su apodo, y si
	la opción SECURE está disahabilitada, le será posible recibir los
	privilegios de auto-op y otros privilegios en los canales sin utilizar
	el comando IDENTIFY.
	
	La Máscara dada con los comandos ADD y DEL pueden contener
	comodínes (* o el signo ?).
	
	Ejemplos:
	
	    ACCESS ADD achurch@*.dragonfire.net
	        Permite acceso al usuario achurch desde cualquier
	        computadora en el dominio dragonfire.net.
	
	    ACCESS DEL achurch@*.dragonfire.net
	        Hace lo contrario al comando anterior.
	
	    ACCESS LIST
	        Muestra la lista de acceso actual.

NICK_HELP_SET
	Sintaxis: SET Opción Parámetros
	
	Ajusta varias opciones del apodo. Donde Opción puede ser:
	
	    PASSWORD   Ajusta la contraseña de su apodo
	    LANGUAGE   Ajusta el lenguaje que Servicios utilizará cuando
	                   envié mensajes a usted
	    URL        Asocia un URL con su apodo
	    EMAIL      Asocia una dirección de E-mail con su apodo
	    INFO       Ajuste el texto que se mostrará con el comando INFO
	    KILL       Activa/desactiva la protección de kill
	    SECURE     Activa/desactiva el modo SECURE para su apodo
	    PRIVATE    Evita que su apodo aparezca en
	                   /msg %S LIST
	    HIDE       Esconde ciertos datos e información sobre su apodo
	    TIMEZONE   Ajusta el uso horario a utilizarse para mostrar la hora
NICK_HELP_SET_OPTION_MAINNICK
	    MAINNICK   Fija el apodo mostrado en las listas de acceso de canales
NICK_HELP_SET_END
	
	Para utilizar este comando es necesario identificarse antes
	con su contraseña. Escriba (/msg %S HELP IDENTIFY para
	mayor información).
	
	Escriba /msg %S HELP SET Opción para mayor información
	sobre una opción especifica.

NICK_HELP_SET_PASSWORD
	Sintaxis: SET PASSWORD Nueva-Contraseña
	
	Cambia la contraseña que utiliza para identificarse como el
	dueño del apodo.

NICK_HELP_SET_LANGUAGE
	Sintaxis: SET LANGUAGE Número
	
	Cambia el lenguaje que los Servicios utilizan cuando envían
	mensajes a usted (por ejemplo cuando responden a un comando
	que usted envió).  Número debe ser escogido de la siguiente
	lista de lenguajes disponibles:
	

NICK_HELP_SET_URL
	Sintaxis: SET URL url
	
	Asocia el URL dado con su apodo.  Este URL será  mostrado
	cuando alguien solicite información sobre su apodo con el
	comando INFO.

NICK_HELP_SET_EMAIL
	Sintaxis: SET EMAIL Dirección
	
	Asocia la dirección de E-mail (Correo-Electrónico) dada con su apodo.
	Esta dirección será mostrada cuando alguien solicite información
	sobre su apodo con el comando INFO.

NICK_HELP_SET_INFO
	Sintaxis: SET INFO Texto
	
	Asocia el texto dado con su apodo; el cual será mostrado
	cuando un usuario pida información sobre su apodo con el
	comando INFO.

NICK_HELP_SET_KILL
	Sintaxis: SET KILL {ON | QUICK | IMMED | OFF}
	
	Activa/desactiva la protección de kill para su apodo.  Con
	protección de kill activada, si algún otro usuario intenta
	utilizar su apodo, se le dará un minuto para que cambie a
	otro, o será removido forzosamente de IRC por %S.
	
	Si selecciona QUICK, el usuario solo tendrá 20 segundos para
	cambiar de apodo en lugar de lo usual (60 segundos).  Si
	seleccionas IMMED, el usuario será desconectado (killed)
	inmediatamente sin ser avisado primero o darle una
	oportunidad de cambiar de apodo, favor de no utilizar esta
	opción a menos que sea verdaderamente necesario.  Los
	Administradores de la red pueden haber deshabilitado esta opción.

NICK_HELP_SET_SECURE
	Sintaxis: SET SECURE {ON | OFF}
	
	Activa/desactiva las funciones de seguridad de %S para
	su apodo.  Con SECURE activado, usted deberá enviar su
	contraseña antes de que se le reconozca como el dueño de
	el apodo, sin importar si su dirección este o no en la lista
	de acceso.  Sin embargo, si aparece en la lista de acceso,
	%S no lo desconectará automáticamente, independientemente
	de como este la opción de KILL.

NICK_HELP_SET_PRIVATE
	Sintaxis: SET PRIVATE {ON | OFF}
	
	Activa/desactiva la opción privacidad de %S sobre su
	apodo.  Con PRIVATE activado, su apodo no aparecerá en las
	listas de apodos generadas con el comando %S LIST.
	(Sin embargo, cualquiera que conozca su apodo puede obtener
	información usando el comando INFO.)

NICK_HELP_SET_HIDE
	Sintaxis: SET HIDE {EMAIL | USERMASK | QUIT} {ON | OFF}
	
	Permite que usted evite que ciertos datos e información
	sean mostradas cuando alguien usa el comando %S INFO en su
	apodo.  Usted puede esconder su correo electrónico (EMAIL),
	última máscara usuario@host (USERMASK), o su último mensaje
	de salida (QUIT).  El segundo parámetro especifica cual
	información debe ser mostrada (OFF) o escondida (ON).

NICK_HELP_SET_TIMEZONE
	Sintaxis: SET TIMEZONE {UTC-Más/Menos | Uso-Horario | DEFAULT}
	
	Ajusta el uso horario a utilizarse para mostrar la hora (Como por
	ejemplo la última vez que un canal fue usado), en lugar de utilizar
	el uso horario en el cual los servicios operan; la opcción DEFAULT
	puede usarse para retornar al uso horario original de los Servicios.
	El uso horario puede ser especificado en una de estas dos formas:
	
	   - Como en UTC Más/Menos, la diferencia de hora entre su hora
	     local y la UTC (Tiempo Universal Coordinado, también conocido
	     como GMT, Greenwich Mean Time).  La diferencia "más" o "menos"
	     es dada con los signos "+" o "-" indicando antes (este) o detras
	     (oeste) de UTC, seguido por o bien un número completo de horas o
	     por una linea en el formato "hh:mm".
	
	   - Como en Nombre de uso horario, tal como "GMT" o "EDT".  No todos
	     los nombres de usos horarios están disponibles en los Servicios, y
	     los usos horarios de algunos paises tienen el mismo nombre, asi es
	     que si usted recibe un error o si el uso horario mostrado por los
	     servicios es incorrecto, use un UTC más/menos en lugar de un nombre.
	
	Note que los Servicios no se ajustarán a si mismos automáticamente para
	el horario de verano; usted necesitará ajustar el uso horario de su
	preferencia manualmente.
	
	Ejemplos:
	
	    SET TIMEZONE -5
	        Ajuste el uso horario a 5 horas detras de UTC.
	
	    SET TIMEZONE +3:30
	        Ajuste el uso horario a 3 horas, 30 minutos antes de UTC.
	
	    SET TIMEZONE JST
	        Ajuste el uso horario al Horario Estandar Japones (UTC+9:00).

NICK_HELP_SET_MAINNICK
	Sintaxis: SET MAINNICK Apodo
	
	Cuando usted tiene multiples apodos ligados, este comando le permite
	seleccionar cual se muestra en las listas de acceso de canales.  Si
	usted desliga este apodo posteriormente, entonces uno diferente
	será seleccionado automáticamente.

NICK_HELP_UNSET
	Sintaxis: UNSET {URL | EMAIL | INFO}
	
	Le permite borrar el URL (URL), correo electrónico (EMAIL),
	o el texto de la información (INFO) asociada a su apodo.

NICK_HELP_UNSET_REQ_EMAIL
	Sintaxis: UNSET {URL | INFO}
	
	Le permite borrar el URL (URL) o texto de la información (INFO)
	asociada a su apodo.

NICK_HELP_RECOVER
	Sintaxis: RECOVER Apodo [Contraseña]
	
	Permite que usted recupere su apodo si alguien más lo ha
	tomado; esto es lo mismo que %S hace automáticamente si
	alguien intenta utilizar un apodo protegido con kill.
	
	Cuando usted envía este comando, %S conectará en línea
	a un usuario falso con el mismo apodo del usuario del cual
	usted desea recuperar su apodo.  Esto causará que los
	servidores de IRC desconecten al otro usuario.  Este usuario
	falso permanecerá en línea por %s para asegurar que
	el otro usuario no se reconecte inmediatamente; después de
	ese tiempo, usted puede reclamar su apodo.  Alternativamente,
	use el comando RELEASE (/msg %S HELP RELEASE) para obtener
	su apodo más rápidamente.
	
	Para poder usar el comando RECOVER para un apodo, usted debe
	suplir la contraseña correcta para ese apodo, o usted debe haberse
	ya identificado por un apodo ligado a el apodo dado.

NICK_HELP_RELEASE
	Sintaxis: RELEASE Apodo [Contraseña]
	
	Instruye a %S que elimine cualquier retención en su
	apodo causada por un kill automático de protección o por el
	uso de el comando RECOVER Normalmente, la espera dura
	%s; este comando lo libera más rápidamente.
	
	Para poder usar el comando RELEASE para un apodo, usted debe
	suplir la contraseña correcta para ese apodo, o usted debe haberse
	ya identificado por un apodo ligado a el apodo dado.

NICK_HELP_GHOST
	Sintaxis: GHOST Apodo [Contraseña]
	
	Termina una sesión "fantasma" de IRC que este utilizando su
	apodo.  Una sesión "fantasma" es aquella que no está
	realmente conectada, pero que el servidor de IRC acredita
	como activa por una razón u otra.  Típicamente esto sucede
	si su computadora se atora o su conexión a Internet se
	desconecta muy brevemente mientras usted está en IRC.
	
	Para poder usar el comando GHOST para un apodo, usted debe
	suplir la contraseña correcta para ese apodo, o usted debe haberse
	ya identificado por un apodo ligado a el apodo dado.

NICK_HELP_INFO
	Sintaxis: INFO Apodo [ALL]
	
	Muestra información sobre el apodo dado, como quien es
	el dueño, última dirección utilizada y hora, y las
	diferentes opciones del apodo. Si se identificó por el apodo
	del cual está reclamando información y ALL es especificado,
	se le mostrará toda la información; independientemente de si
	esta esté escondida o no.
NICK_HELP_INFO_AUTH
	
	Si el correo electrónico del apodo no ha sido aún confirmado con
	el comando AUTH, no será mostrado a nadie excepto a el dueño de el
	apodo y a los Administradores de los Servicios, independientemente
	de la configuración SET HIDE EMAIL estableciada para el apodo.

NICK_HELP_LISTCHANS
	Sintaxis: LISTCHANS
	
	Lista los nombres y el número total de canales que usted
	ha registrado bajo su apodo.

NICK_HELP_LIST
	Sintaxis: LIST [+Inicio] Patrón
	
	Alista todos los apodos registrados para los cuales o bien el apodo
	mismo o la  última dirección vista (en el formato Usuario@Proveedor-web)
	concuerdan con el patrón dado.  Apodos con la opción PRIVATE activada no
	serán mostrados. Si el parámetro "Inicio" es dado, se está especificando
	cuantas entradas similares han de ser ignoradas antes de mostrar los resultados
	(por ejemplo, +50 causaria que las primeras 50 entradas similares fueran
	ignoradas).
	
	Ejemplos:
	
	    LIST joeuser@foo.com
	        Lista todos los apodos registrados que fueron usados últimamente
	        por joeuser@foo.com.
	
	    LIST *Bot*!*@*
	        Lista todos los apodos registrados que contengan Bot en sus
	        nombres (no importan las mayúsculas).
	
	    LIST *@*.bar.org
	        Lista todos los apodos registrados que fueron  usados últimamente por
	        usuarios del dominio bar.org.
NICK_HELP_LIST_OPERSONLY
	
	Solamente los operadores de IRC pueden utilizar este comando.

NICK_HELP_LISTEMAIL
	Sintaxis: %s
	
	Alista apodos registrados al igual que el comando LIST, pero lo hace
	basado en la dirección de correo electrónico en lugar de Usuario@proveedor-web.
	Vea el comando de ayuda para LIST (/msg %S HELP LIST) para obtener
	más información.

NICK_HELP_STATUS
	Sintaxis: STATUS Apodo...
	
	Informa si el usuario de un apodo es reconocido como el dueño
	del mismo.  La respuesta tiene el siguiente formato:
	
	    Apodo código-De-Estatus
	
	donde Apodo es el apodo enviado con el comando,
	Código-De-Estatus es uno de los siguientes:
	
	    0 - usuario no está conectado o apodo no está registrado
	    1 - usuario no reconocido como dueño del apodo
	    2 - usuario reconocido por la lista de acceso solamente
	    3 - usuario reconocido por identificación con contraseña
	
	Hasta 16 apodos pueden ser enviados con cada comando; el
	resto serán ignorados. No se genera mensaje de error alguno si
	no se envía un apodo.

NICK_HELP_AJOIN
	Sintaxis: AJOIN ADD Canal
	          AJOIN DEL Canal
	          AJOIN LIST
	
	Modifica o muestra la lista de auto ingreso (autojoin) para
	su apodo.  Esta es la lista de canales a los cuales le
	gustaria ser llevado automáticamente, y va a surtir efecto
	cuando usted se identifica a %S.  Note que usted
	solamente puede incluír nombres de canales que comiencen con
	"#" en la lista de auto ingreso (canales locales no pueden
	ser incluidos).
	
NICK_HELP_AJOIN_END
	Note que el servidor de IRC pudiera prohibirle el ingreso a
	algunos de los canales en la lista de auto ingreso, tales
	canales incluyen aquellos a los cuales le ha sido fijado una
	llave (modo +k) o que se han fijado como invitación-solamente
	(modo +i).
NICK_HELP_AJOIN_END_CHANSERV
	Note que el servidor de IRC pudiera prohibirle el ingreso a
	algunos de los canales en la lista de auto ingreso, tales
	canales incluyen aquellos a los cuales le ha sido fijado una
	llave (modo +k) o que se han fijado como invitación-solamente
	(modo +i).  Sin embargo, si usted tiene acceso al comando
	INVITE de %s en un canal registrado, %S le
	invitará automáticamente a usted para que pueda ingresar al
	canal.

NICK_OPER_HELP_COMMANDS
	
	Los siguientes comandos están disponibles a los Administradores de Servicios:
	
	    DROPNICK     Cancela cualquier apodo
NICK_OPER_HELP_COMMANDS_DROPEMAIL
	    DROPEMAIL    Cancela todos los apodos que concuerden con una
	                    dirección de correo electrónico especificada
NICK_OPER_HELP_COMMANDS_GETPASS
	    GETPASS      Muestra la contraseña para un apodo
	                    (solo si encripción está desactivada)
NICK_OPER_HELP_COMMANDS_FORBID
	    FORBID       Previene que un apodo sea utilizado/registrado
	    SUSPEND      Previene que alguien se identifique por un apodo dado
	    UNSUSPEND    Cancela la suspensión de un apodo
NICK_OPER_HELP_COMMANDS_SETAUTH
	    SETAUTH      Fija un nuevo código de autenticación para un apodo
	    GETAUTH      Muestra el código de autenticación para un apodo
	    CLEARAUTH    Cancela el código de autenticación para un apodo
NICK_OPER_HELP_COMMANDS_END
	
	Administradores de Servicios también pueden usar los siguientes
	comandos en cualquier apodo sin necesidad de identificarse:
	
	    SET, ACCESS LIST, INFO ALL
	
	Vea la ayuda para cada comando para detalles específicos.

NICK_OPER_HELP_DROPNICK
	Sintaxis: DROPNICK Apodo
	
	Remueve el apodo dado de la base de datos de %S, asi como
	cualquier apodo ligado a este.
	
	Límitado a los Administradores de servicios.

NICK_OPER_HELP_DROPEMAIL
	Syntax: DROPEMAIL Patrón
	        DROPEMAIL-CONFIRM Patrón
	
	Cancela todos los apodos con una dirección de correo
	electrónico que concuerde con el Patrón dado.  Si Patrón es el
	simple caractér "-", entonces cancelará todos los apodos sin
	una dirección de correo electrónico.
	
	Para prevenir el que accidentalmente se cancelen apodos via
	este comando, se deberá entrar dos veces: primero usando el
	comando llamado DROPEMAIL, y despues usando DROPEMAIL-CONFIRM
	con el mismo patrón.  El comando DROPEMAIL-CONFIRM debe ser
	ejecutado dentro de %s de y por la misma persona que
	ejecutó el comando DROPEMAIL.  Si se dan mas de 4 ejecuciones
	del comando DROPEMAIL continuamente, la mas antigua será
	ignorada y no se le reconoserá mas para una ejecución del
	comando DROPEMAIL-CONFIRM.
	
	Limitado a los Administradores De Servicios.

NICK_OPER_HELP_SET
	
	Administradores de Servicios pueden seleccionar la opción
	NOEXPIRE, con la cual se puede prevenir que algunos apodos
	expiren.  Adicionalmente, los Administradores de Servicios
	pueden establecer opciones para cualquier apodo sin necesidad
	de proveer una contraseña, utilizando el formato
	SET !Apodo Opción Parámetros.

NICK_OPER_HELP_SET_NOEXPIRE
	Sintaxis: SET [!Apodo] NOEXPIRE {ON | OFF}
	
	Define si el apodo dado expirará.  Si se define ON previene
	que el apodo expire.  Si no se incluye un apodo, entonces se
	aplica a tu propio apodo.
	
	Limitado a los Administradores De Servicios.

NICK_OPER_HELP_UNSET
	
	Administradores de Servicios pueden usar el comando UNSET en
	cualquier apodo por medio de añadir !Apodo antes del nombre
	de la opción, como en el comando SET.

NICK_OPER_HELP_OLD_UNLINK
	
	Administradores de Servicios pueden desligar cualquier apodo sin
	necesidad de enviar una contraseña.

NICK_OPER_HELP_UNLINK
	Sintaxis: UNLINK Apodo [FORCE]
	
	Remueve una liga (alias) de su apodo creada con el
	comando LINK.  Usted no puede desligar el apodo que está
	usando actualmente; cambie a otro apodo ligado primeramente
	antes de utilizar este comando.
	
	Si usted desliga el "main nick" (Apodo Principal) que seleccionó
	usando el comando SET MAINNICK, su apodo actual será automáticamente
	seleccionado como el apodo principal (main nick).
	
	Los Administradores de servicios pueden agregar la opcción FORCE,
	la cual permite remover una liga de un apodo que no les pertenese.

NICK_OPER_HELP_OLD_LISTLINKS
	Sintaxis: LISTLINKS Apodo
	
	Alista los apodos que estén ligados al apodo dado.
	
	Limitado a los Administradores De Servicios.

NICK_OPER_HELP_LISTLINKS
	Sintaxis: LISTLINKS [Apodo]
	
	Alista todos los apodos que esten ligados a su apodo.  El apodo
	principal para el grupo de apodos ligados será indicado por un
	signo "*" a la izquierda del apodo.  Operadores de Servicios
	pueden alistar ligas para cualquier apodo cuando especifican el
	apodo en el comando.

NICK_OPER_HELP_ACCESS
	
	Los Administradores de Servicios pueden dar un apodo como parámetro en
	el comando LIST para mostrar la lista de acceso de cualquier apodo.

NICK_OPER_HELP_INFO
	
	Administradores de Servicios pueden usar parámetro ALL en cualquier apodo.

NICK_OPER_HELP_LISTCHANS
	
	Administradores de Servicios pueden alistar canales para cualquier apodo
	usando el apodo con el comando (por ejemplo,
	/msg %S LISTCHANS Cualquier-Apodo).

NICK_OPER_HELP_LIST
	Sintaxis: LIST [+Inicio] Patrón [FORBIDDEN] [NOEXPIRE] [SUSPENDED]
	
	Alista todos los apodos registrados para los cuales o bien el apodo
	mismo o la  última dirección vista (en el formato Usuario@Proveedor-web)
	concuerdan con el patrón dado.  Apodos con la opción PRIVATE activada
	serán solamente mostrados a los Administradores de los servicios.  Ellos,
	verán que los apodos con la opción NOEXPIRE activada tendrán un signo !
	antes del apodo y aquellos que están suspendidos estarán prefijados con el
	signo * antes del apodo.
	
	Si las opciones FORBIDDEN, SUSPENDED o NOEXPIRE son dadas, entonces
	solamente los apodos que, respectivamente, están prohibidos, suspendidos,
	o que tienen la opción NOEXPIRE activada será mostrados.  Las opciones
	pueden ser utilizadas en cualquier orden y en cualquier combinación.
	Estas opciones están limitadas a los Administradores de Servicios.
NICK_OPER_HELP_LIST_AUTH
	Sintaxis: LIST [+Inicio] Patrón [FORBIDDEN] [NOEXPIRE] [SUSPENDED] [NOAUTH]
	
	Alista todos los apodos registrados para los cuales o bien el apodo
	mismo o la  última dirección vista (en el formato Usuario@Proveedor-web)
	concuerdan con el patrón dado.  Apodos con la opción PRIVATE activada
	serán solamente mostrados a los Administradores de los servicios.  Ellos,
	verán que los apodos con la opción NOEXPIRE activada tendrán un signo !
	antes del apodo, los que están suspendidos estarán prefijados con el signo *
	antes del apodo, y aquellos que aún no han sido autenticados (con el comando
	AUTH) tendrán un signo ? antes del apodo.
	
	Si las opciones FORBIDDEN, SUSPENDED, NOEXPIRE o NOAUTH son dadas,
	solamente los apodos que, respectivamente, están prohibidos, suspendidos, que
	tienen la opción NOEXPIRE activada, o que que no han sido aún autenticados
	será mostrados.  Las opciones pueden ser utilizadas en cualquier orden y en
	cualquier combinación.  Estas opciones están limitadas a los
	Administradores De Servicios.
NICK_OPER_HELP_LIST_END
	
	Si el parámetro "Inicio" es dado, este especifica cuantas entradas
	similares serán ignoradas antes de mostrar los resultados (por ejemplo, +50
	causaria que las primeras 50 entradas similares fueran ignoradas).
	
	Ejemplos:
	
	    LIST joeuser@foo.com
	        Alista todos los apodos registrados que fueron usados últimamente
	        por joeuser@foo.com.
	
	    LIST *Bot*!*@*
	        Alista todos los apodos registrados que contengan Bot
	        en su nombre (no importa mayúsculas).
	
	    LIST * NOEXPIRE
	        Alista todos los apodos registrados que han sido fijados
	        como apodos que no expirarán.

NICK_OPER_HELP_GETPASS
	Sintaxis: GETPASS Apodo
	
	Muestra la contraseña para el apodo dado.  Note que cuando
	este comando es utilizado, un mensaje incluyendo el apodo de
	la persona que lo utiliza, y el apodo sobre el cual se interroga
	es grabado en el archivo de services y enviado como un mensaje global
	(WALLOPS/GLOBOPS).
	
	Este comando pudiera no estar disponible cuando la red está usando encripción,
	dependiendo de el tipo de encripción utilizada.
	
	Limitado a los Administradores De Servicios.

NICK_OPER_HELP_FORBID
	Sintaxis: FORBID Apodo
	
	Prohibe a todos el regístro y el uso de un apodo. Se puede
	revertir utilizando el comando DROP.
	
	Limitado a los Administradores De Servicios.

NICK_OPER_HELP_SUSPEND
	Sintaxis: SUSPEND [+Expiración] Apodo Razón
	
	Previene que un apodo, y todos aquellos apodos con el mismo
	apodo raíz, sean utilizados o que alguien se identifique por
	ellos.  Puede lograrse que un apodo deje de estar suspendido
	por medio del comando UNSUSPEND, lo cual permitirá que sea
	usado como antes.  El parámetro Expiración es interpretado
	en la misma manera que para el comando %s AKILL.
	
	Limitado a los Administradores De Servicios.

NICK_OPER_HELP_UNSUSPEND
	Sintaxis: UNSUSPEND Apodo
	
	Permite a los usuarios el uso y habilidad de identificarse por
	un apodo previamente SUSPENDido.
	
	Limitado a los Administradores De Servicios.

NICK_OPER_HELP_AJOIN
	
	Administradores de servicios pueden ver la lista de auto ingreso (autojoin)
	para cualquier apodo con el formato LIST Apodo.

NICK_OPER_HELP_SETAUTH
	Sintaxis: SETAUTH Apodo
	
	Establece un nuevo código de autenticación para el apodo dado. El dueño
	del apodo tendrá que utilizar el comando AUTH para el apodo antes de
	que le sea permitido identificarse nuevamente.   Si el apodo está en uso
	actualmente, una notificación (notice) le será enviada al usuario del
	apodo y esta persona no será ya más tratada como si se hubiese
	identificado previamente por el apodo.
	
	Limitado a los Administradores De Servicios.

NICK_OPER_HELP_GETAUTH
	Sintaxis: GETAUTH Apodo
	
	Recobra el código de autenticación asociado con el  nickname,
	si es que realmente existe un código para dicho apodo.
	
	Limitado a los Administradores De Servicios.

NICK_OPER_HELP_CLEARAUTH
	Sintaxis: CLEARAUTH Apodo
	
	Remueve cualquier código de autenticación asociado con el  Apodo,
	permitiendo que el apodo pueda ser usado normalmente.
	
	Limitado a los Administradores De Servicios.


###########################################################################
#
# ChanServ help messages
#
###########################################################################

# Used in OP/VOICE/etc for required access levels for commands
CHAN_HELP_REQSOP_LEVXOP
	nivel 100 (SOP)
CHAN_HELP_REQSOP_LEV
	level 100
CHAN_HELP_REQSOP_XOP
	SOP
CHAN_HELP_REQAOP_LEVXOP
	nivel 50 (AOP)
CHAN_HELP_REQAOP_LEV
	nivel 50
CHAN_HELP_REQAOP_XOP
	AOP
CHAN_HELP_REQHOP_LEVXOP
	nivel 40 (HOP)
CHAN_HELP_REQHOP_LEV
	nivel 40
CHAN_HELP_REQHOP_XOP
	HOP
CHAN_HELP_REQVOP_LEVXOP
	nivel 30 (VOP)
CHAN_HELP_REQVOP_LEV
	nivel 30
CHAN_HELP_REQVOP_XOP
	VOP

# Real help messages start here
CHAN_HELP
	%S le permite a usted registrar y controlar varios aspectos
	de los canales.  %S frecuentemente previene el que usuarios
	mal intecionados puedan "tomar" el control limitando a quién
	se permite obtener privilegios de operador del canal. Escriba
	/msg %S HELP COMMANDS para ver la lista de comandos de %S;
	para usar un comando, escriba /msg %S Comando, o para más
	información sobre un comando, escriba /msg %S HELP Comando.
CHAN_HELP_EXPIRES
	
	Note que si un canal no es utilizado por %s (es decir
	qué ningún usuario en la lista de acceso del canal entra en
	ese periodo de tiempo) será automáticamente cancelado.

CHAN_HELP_COMMANDS
	Los siguientes comandos pueden ser utilizados con %S:
	
	    REGISTER   Registre un canal
	    IDENTIFY   Identifiquese con su contraseña
	    DROP       Cancele la registración de un canal
	    SET        Fija opciones e información del canal
	    UNSET      Remueve información del canal
	    INFO       Muestra opciones e información del canal
CHAN_HELP_COMMANDS_LIST
	    LIST       Alista canales registrados
CHAN_HELP_COMMANDS_AKICK
	    AKICK      Mantiene la lista de AutoKick (AutoPateo)
CHAN_HELP_COMMANDS_LEVELS
	    ACCESS     Mantiene la lista general de acceso al canal
	    LEVELS     Fije los niveles de privilegio de acceso al canal en una manera más detallada
CHAN_HELP_COMMANDS_XOP
	    SOP        Mantiene la lista de SuperOp
	    AOP        Mantiene la lista de AutoOp
CHAN_HELP_COMMANDS_HOP
	    HOP        Mantiene la lista AutoHalfop (Auto Mitad OP)
CHAN_HELP_COMMANDS_XOP_2
	    VOP        Mantiene la lista de AutoVoice (Auto Voz)
	    NOP        Mantiene la lista de NeverOp (Nunca OP)
CHAN_HELP_COMMANDS_OPVOICE
	    OP         Le da a un usuario estatus de operador de canal (+o)
	    DEOP       Remueve el estatus de operador de canal (+o)
	    VOICE      Le da a un usuario el estatus de voz (voice) (+v)
	    DEVOICE    Remueve el estatus de voz (voice) (+v)
CHAN_HELP_COMMANDS_HALFOP
	    HALFOP     Le da a un usuario estatus de MitadOP(halfop) (+h)
	    DEHALFOP   Remueve el estatus de MitadOP (halfop) (+h)
CHAN_HELP_COMMANDS_PROTECT
	    PROTECT    Le da a un usuario estatus de protegido (+a)
	    DEPROTECT  Remueve el estatus de protegido (+a)
CHAN_HELP_COMMANDS_INVITE
	    INVITE     Invitese usted mismo a un canal
	    UNBAN      Remueva un bloqueo a usted mismo de un canal
	    KICK       Patee (kick) a un usuario de un canal
	    TOPIC      Cambie el tópico de un canal
	    CLEAR      Remueva los modos de un canal o haga un pateo masivo de sus usuarios
	    STATUS     Muestre el nivel de acceso de un usuario en un canal

CHAN_HELP_REGISTER
	Sintaxis: REGISTER Canal Contraseña Descripción
	
	Registra un nuevo canal en la base de datos de %S.
	Para poder utilizar este comando, usted primero debe ser
	operador del canal que está tratando de registrar.  La
	contraseña deberá ser utilizada con el comando IDENTIFY
	cuando necesite hacer modificaciones posteriores a las
	opciones del canal.  El último parámetro, que debe ser
	incluido, es una descripción de los propósitos del canal.
	
	Cuando usted registra un canal, usted queda grabado como
	el "fundador" del canal. El fundador de un canal puede
	modificar cualquiera de las opciones para dicho canal;
	%S le dará automáticamente privilegios de operador
	al fundador del canal cuando él/ella ingresen al canal.
	
	AVISO: para poder registrar un canal, usted deber haber registrado
	su apodo e identificado por él.  Si aún no lo registró, escriba
	/msg %s HELP para obtener información de cómo hacerlo.
CHAN_HELP_REGISTER_ADMINONLY
	
	Limitado a los Administradores De Servicios.

CHAN_HELP_IDENTIFY
	Sintaxis: IDENTIFY Canal Contraseña
	
	Lo identifica a usted ante %S como el fundador
	del canal.  Muchos comandos requieren que usted
	ejecute este comando antes de poder utilizarlos.  La
	contraseña debe ser la misma que se ingresó al momento
	de registrar el canal (comando REGISTER).

CHAN_HELP_DROP
	Sintaxis: DROP Canal Contraseña
	
	Elimina la registración del canal nombrado.  Esto puede ser utilizado
	únicamente por el fundador del canal, La contraseña debe ser suplida
	para asi evitar la eliminación accidental del canal.

CHAN_HELP_SET
	Sintaxis: SET Canal Opción Parámetros
	
	Habilita al fundador del canal a definir varias opciones
	e información del canal.  El fundador, debe hacer uso del
	comando IDENTIFY antes de utilizar SET.
	
	Opciones disponibles:
	
	    FOUNDER     Cambia el fundador de un canal
	    SUCCESSOR   Establece el sucesor para un canal
	    PASSWORD    Cambia la contraseña del fundador
	    DESC        Cambia la descripción del canal
	    URL         Asocia un URL con el canal
	    EMAIL       Asocia una dirección E-mail con el canal
	    ENTRYMSG    Fija un mensaje que será enviado a los usuarios
	                   cuando ellos entran al canal
	    KEEPTOPIC   Retenga el tópico cuando el canal no es usado
	    TOPICLOCK   Tópico solo podrá ser cambiado via TOPIC
	    MLOCK       Fija los modos del canal en 'ON' u 'OFF'
	    PRIVATE     Oculta al canal del comando LIST
	    RESTRICTED  Restringe el acceso al canal
	    SECURE      Activa las opciones de seguridad de %S
	    SECUREOPS   Control más estricto del estatus de operador
	    LEAVEOPS    No remover OPs a los usuarios al entrar al canal
	    OPNOTICE    Notifíca cuándo los comandos OP/VOICE son usados
	    ENFORCE     Impone estatus auto-op y auto-voice de immediato
	
	Escriba /msg %S HELP SET Opción para más información
	sobre una opción en particular.

CHAN_HELP_SET_FOUNDER
	Sintaxis: SET Canal FOUNDER Apodo
	
	Cambia el fundador de un canal.  El nuevo apodo debe
	ser uno registrado.

CHAN_HELP_SET_SUCCESSOR
	Sintaxis: SET Canal SUCCESSOR Apodo
	
	Cambia el sucesor de un canal.  Si el apodo del fundador
	expira o si es cancelado mientras el canal está todavia
	registrado, el sucesor se convertirá en el nuevo fundador
	del canal.  Sin embargo, si el sucesor ya tiene demaciados
	canales registrados (%d), el canal será cancelado como
	resultado, justo como si ningun sucesor hubiese sido fijado.
	El nuevo apodo debe ser uno registrado.

CHAN_HELP_SET_PASSWORD
	Sintaxis: SET Canal PASSWORD Contraseña
	
	Cambia la contraseña usada para identificarse como el fundador
	del canal.

CHAN_HELP_SET_DESC
	Sintaxis: SET Canal DESC Descripción
	
	Cambia la descripción para el canal, que se muestra con
	los comandos LIST e INFO.

CHAN_HELP_SET_URL
	Sintaxis: SET Canal URL url
	
	Asocia con el canal el URL dado.  Este URL será
	mostrado siempre que alguien pida información sobre
	el canal con el comando INFO.

CHAN_HELP_SET_EMAIL
	Sintaxis: SET Canal EMAIL Dirección
	
	Asocia con el canal la dirección E-mail (correo electrónico) dada.
	Esta dirección será mostrada siempre que alguien pida
	información sobre el canal con el comando INFO.

CHAN_HELP_SET_ENTRYMSG
	Sintaxis: SET Canal ENTRYMSG Mensaje
	
	Fija el mensaje que será enviado via /NOTICE a los
	usuarios cuando estos entren al canal.

CHAN_HELP_SET_KEEPTOPIC
	Sintaxis: SET Canal KEEPTOPIC {ON | OFF}
	
	Habilita o deshabilita la opción de retención del tópico
	para el canal.  Cuando retención del tópico está en ON
	(activada), el tópico del canal será recordado por %S
	aún después que el último usuario se haya retirado, y se
	restaurará la próxima vez que alguien ingrese al canal.

CHAN_HELP_SET_TOPICLOCK
	Sintaxis: SET Canal TOPICLOCK {ON | OFF}
	
	Habilita o deshabilita la opción para candado en tópico del
	canal.  Cuando TOPICLOCK (candado en tópico) está en ON, %S
	no habilitará el cambio de tópico excepto vía el comando TOPIC.

CHAN_HELP_SET_MLOCK
	Sintaxis: SET Canal MLOCK Modos
	
	Fija el parámetro modo-candado para el canal.  %S le permite a
	usted definir ciertos modos en el canal para que esten siempre
	activados (o desactivados).  Todos los modos binarios (n, s, t,
	k, l, ect) pueden ser fijados permanentemente; cada modo puede
	ser activado, desactivado, o no fijado.  El parámetro Modos es
	construido exactamente en la misma forma que se ejecuta un
	comando /MODE; lo cual significa que, los modos seguidos por
	un signo + son activados, y los modos seguidos por - son
	desactivados.
	
	Advertencia:  ¡Si usted fija una llave con el modo-candado, como
	en el segundo ejemplo abajo, entonces deberá también fijar la
	opción RESTRICTED para el canal (vea HELP SET RESTRICTED), de
	lo contrario a cualquier usuario que ingrese al canal cuando
	este esté vacío le sera posible ver la llave del canal!
	Igualmente MLOCK +i no preveerá el que los usuarios ingresen
	al canal cuando está vacío sin haber sido invitados.
	
	Ejemplos:
	
	    SET #Canal MLOCK +nt-ikl
	        Impone los modos n y t como activos, y los modos i, k,
	        y l como innactivos.  Todos los otros modos son
	        liberados para estár bien activos o innactivos.
	
	    SET #Canal MLOCK +knst-ilmp My-Contraseña
	        Impone los modos k, n, s, y t como activos, y los modos
	        i, l, m, y p como innactivos.  También establece la
	        contraseña del canal como "My-Contraseña".
	
	    SET #Canal MLOCK +
	        Remueve el modo-candado; todos los modos en el canal
	        son liberados para estár o bien activados o
	        desactivados (innactivos).

CHAN_HELP_SET_HIDE
	Sintaxis: SET Canal HIDE {EMAIL | TOPIC | MLOCK} {ON | OFF}
	
	Le permite a usted prevenir que ciertos datos e información
	sean mostrados cuando alguien ejecuta un %S INFO en el
	canal especificado.  Usted puede esconder el correo electrónico
	(EMAIL) para un canal, el último tópico (TOPIC), y el modo-candado
	(MLOCK).  El segundo parámetro especifica si la información deberia
	ser mostrada (OFF) o escondida (ON).
	
	Note que el tópico será siempre ocultada si el canal tiene activados
	los modos-candado +p o +s.

CHAN_HELP_SET_PRIVATE
	Sintaxis: SET Canal PRIVATE {ON | OFF}
	
	Habilita o deshabilita la opción de Privado para el canal.
	Cuando esta opción está activada, el comando /msg %S LIST no
	incluirá el canal en ninguna lista.

CHAN_HELP_SET_RESTRICTED
	Sintaxis: SET Canal RESTRICTED {ON | OFF}
	
	Habilita o deshabilita la opción de Acceso Restringido para
	el canal.  Cuando el acceso restringido está activo (ON),
	usuarios que NO están en la lista de acceso o usuarios que
	normalmente no tienen privilegio de operadores cuando entran
	al canal serán expulsados y se les prohibirá el acceso  al
	canal.

CHAN_HELP_SET_SECURE
	Sintaxis: SET Canal SECURE {ON | OFF}
	
	Habilita o deshabilita la las opciones de seguridad de
	%S en un canal.  Cuando SECURE está activo (SET ON),
	SOLO los usuarios que tienen registrados sus apodos con
	%s y que se han IDENTIFICADO con sus contraseñas
	tendrán privilegios en el canal segun lo establecido por la
	lita de acceso.  Si la opción RESTRICTED está también fijada
	(SET ON), será negado a los usuarios el ingreso al canal
	hasta que se hayan IDENTIFICADO a %s.

CHAN_HELP_SET_SECUREOPS
	Sintaxis: SET Canal SECUREOPS {ON | OFF}
	
	Habilita o deshabilita la opción de OPs Seguros para un
	canal.  Cuando OPs Seguros está activada (ON), usuarios que
	NO estén en la lista de acceso del canal NO podrán obtener
	estatus de operadores.

CHAN_HELP_SET_LEAVEOPS
	Sintaxis: SET Canal LEAVEOPS {ON | OFF}
	
	Habilita o deshabilita la opción leave ops para un canal.
	Cuando leave ops está fijado, %S dejará OPs del
	servidor (+o para la primera persona en el canal) en efecto
	incluso si el usuario normalmente no optendria OPs
	automáticamente.  La opción SECUREOPS será aún enforzada.
	¡Cuidado!:  Esto puede permitir que personas no autorizadas
	"roben" el estatus de Operador durante una ruptura de la red.

CHAN_HELP_SET_OPNOTICE
	Sintaxis: SET Canal OPNOTICE {ON | OFF}
	
	Habilita o deshabilita la opción Nota de OPs para un canal.
	Cuando OPNOTICE es activada (ON), %S enviará una nota
	al canal siempre que los comandos OP, VOICE, o cualquier
	otro comando relacionado a estos sea utilizado para un usuario
	en el canal.

CHAN_HELP_SET_ENFORCE
	Sintaxis: SET Canal ENFORCE {ON | OFF}
	
	Habilita o deshabilita la opción de IMPONER a un canal. Cuando
	ENFORCE es activado (ON), %S impondrá los niveles de
	auto-op y auto-voice para todos los usuarios.  Por ejemplo, si a
	un usuario con privilegios de auto-op se le remueven los OPs, %S
	automáticamente le dará OPs al usuario de nuevo, y %S tampoco
	permitirá que el comando DEOP sea usado en aquel usuario.

CHAN_HELP_UNSET
	Sintaxis: UNSET Canal {SUCCESSOR | URL | EMAIL | ENTRYMSG}
	
	Borra los ajustes especificados de un canal.

CHAN_HELP_SOP
	Sintaxis: SOP Canal ADD Apodo
	          SOP Canal DEL Apodo
	          SOP Canal LIST [+Inicio] [Máscara]
	          SOP Canal COUNT
	
CHAN_HELP_SOP_MID1
	Mantiene la lista de SuperOp (SOP) para un canal.
	Usuarios con el estatus SOP reciben OPs automáticamente
	(reciben modo +o) cuando estos ingresan al canal.
CHAN_HELP_SOP_MID1_CHANPROT
	Mantiene la lista de SuperOp (SOP) para un canal.
	Usuarios con el estatus SOP reciben OPs automáticamente
	(reciben modo +o) y son protegidos (modo +a) cuando estos
	ingresan al canal.
CHAN_HELP_SOP_MID2
	
	Un SOP puede modificar las listas AOP, VOP, y NOP.
	Solamente el fundador del canal puede modificar la lista de
	SOP.
CHAN_HELP_SOP_MID2_HALFOP
	
	Un SOP puede modificar las listas AOP, HOP, VOP, y NOP.
	Solamente el fundador del canal puede modificar la lista SOP.
CHAN_HELP_SOP_END
	
	Los subcomandos ADD, DEL, LIST, y COUNT funcionan de la
	misma manera que el comando AOP.

CHAN_HELP_AOP
	Sintaxis: AOP Canal ADD Apodo
	          AOP Canal DEL Apodo
	          AOP Canal LIST [+Inicio] [Máscara]
	          AOP Canal COUNT
	
	Mantiene la lista AutoOp (AOP) para un canal.
	Usuarios con estatus AOP reciben OPs automáticamente
	(reciben modo +o) cuando ingresan al canal.
CHAN_HELP_AOP_MID
	
	Un AOP puede modificar las listas VOP y NOP; los SOPs pueden
	modificar la lista AOP.
CHAN_HELP_AOP_MID_HALFOP
	
	Un AOP puede modificar las listas HOP, VOP, y NOP; los SOPs
	pueden modificar la lista AOP.
CHAN_HELP_AOP_END
	
	El comando AOP ADD añade un usuario a la lista AOP, y el comando
	AOP DEL remueve un usuario de la lista AOP.  Notese que un usuario
	solo puede estar en la lista de acceso de un canal una vez; si el
	usuario especificado en un comando AOP ADD se encuentra ya en otra
	lista de acceso al canal, por ejemplo la lista VOP, este será removido
	de aquella lista primeramente.  (Si no le fuese permitido a usted el
	modificar aquella lista--por ejemplo, si el usuario ya está en la
	lista SOP y usted no es el fundador del canal--entonces el comando
	AOP ADD fallará.)
	
	El comando AOP LIST muestra la lista AOP.  Si una máscara de comodín
	(signo *) es insertada, solamente aquellas entradas que concuerden con
	esa máscara son mostradas. Si el parámetro "Inicio" es dado este
	especifica cuántas entradas similares serán ignoradas antes de mostrar
	los resultados (por ejemplo, +50 causaria que las primeras 50
	entradas sean ignoradas).
	
	El comando AOP COUNT retorna el número de entradas que
	existen en la lista AOP.

CHAN_HELP_HOP
	Sintaxis: HOP Canal ADD Apodo
	          HOP Canal DEL Apodo
	          HOP Canal LIST [+Inicio] [Máscara]
	          HOP Canal COUNT
	
	Mantiene la lista HalfOp (HOP) para un canal.  Usuarios
	con estatus HOP reciben privilegios de Mitad-OP (reciben
	modo +h) automáticamente cuando estos ingresan al canal.
	
	Un HOP puede modificar las listas VOP y NOP; SOPs y AOPs
	pueden modificar la lista HOP.
	
	Los subcomandos ADD, DEL, LIST, y COUNT funcionan de la
	misma manera que con el comando AOP.

CHAN_HELP_VOP
	Sintaxis: VOP Canal ADD Apodo
	          VOP Canal DEL Apodo
	          VOP Canal LIST [+Inicio] [Máscara]
	          VOP Canal COUNT
	
	Mantiene la lista VoiceOp (VOP) para un canal.
	Usuarios con estatus VOP reciben vóz automáticamente
	(reciben modo +v) cuando estos ingresan al canal.
	
	Los subcomandos ADD, DEL, LIST, y COUNT funcionan de la
	misma manera que con el comando AOP.

CHAN_HELP_NOP
	Syntax: NOP canal ADD Apodo
	        NOP canal DEL Apodo
	        NOP canal LIST [+Inicio] [Máscara]
	        NOP canal COUNT
	
	Mantiene la lista NeverOp (NOP) (NuncaOP) para un canal. A los usuarios
	con el estatus NOP no les es permitido recibir estatus de operador de canal
	(modo +o), y ChanServ removerá el modo +o de tales usuarios
	cuando sea que este le sea dado a ellos.
	
	Los subcomandos ADD, DEL, LIST, y COUNT funcionan de la
	misma manera que con el comando AOP.

CHAN_HELP_ACCESS
	Sintaxis: ACCESS Canal ADD Apodo Nivel
	          ACCESS Canal DEL Apodo
	          ACCESS Canal LIST [+Inicio] [Máscara]
	          ACCESS Canal COUNT
	
	Mantiene la Lista De Acceso de un canal.  La lista de acceso
	establece qué usuarios están habilitados a ser operadores en
	el canal o acceder a comandos de %S en el canal.  Se pueden
	establecer diferentes niveles de usuarios para acceder a
	diferentes privilegios; utilice /msg %S HELP ACCESS LEVELS
	para obtener información más específica.  Cualquier apodo que
	no esté en la lista de acceso tiene nivel de usuario 0 (cero).
	
	El comando ACCESS ADD agrega el apodo con el nivel dado a
	la lista de acceso; si el usuario ya existe en la lista, se
	reemplazará su nivel con el dado.  El nivel especificado debe
	ser menor al que posee el usuario que está ingresando el
	comando, y si el apodo está actualmente en la lista, el nivel
	actual de dicho apodo debe ser menor que el nivel del usuario que
	está ingresando el comando.
	
	El comando ACCESS DEL remueve el apodo dado de la
	lista de acceso. Los usuarios solamente pueden remover apodos
	con niveles de acceso inferiores al que poseen ellos mismos.
	
	El comando ACCESS LIST muestra la lista de acceso.  Si una máscara
	de comodín (signo *) es insertada, solamente aquellas entradas que
	concuerden con esa máscara son mostradas. Si el parámetro Inicio es dado,
	este especifica cuántas entradas similares ignorar antes de mostrar los
	resultados (por ejemplo, +50 causará que las primeras 50 entradas sean
	ignoradas).
	
	El comando ACCESS LISTLEVEL es similar a ACCESS LIST, pero
	solo muestra las entradas con el nivel de acceso especificado.  El
	nivel puede ser especificado en serie: LISTLEVEL 50-100, por ejemplo,
	alista todos los usuarios con niveles de acceso inclusivos entre
	50 y 100.
	
	El comando ACCESS COUNT retorna el número de entradas que
	existen en la lista de acceso.
CHAN_HELP_ACCESS_XOP
	
	Note que la lista de acceso puede también ser manipulada con los
	comandos SOP, AOP, VOP, y NOP, los cuales corresponden a los
	niveles de %d, %d, %d, y %d respectivamente.
CHAN_HELP_ACCESS_XOP_HALFOP
	
	Note que la lista de acceso puede ser también manipulada con los
	comandos SOP, AOP, HOP, VOP, y NOP, los cuales corresponden
	a los niveles de %d, %d, %d, %d, y %d respectivamente.

CHAN_HELP_ACCESS_LEVELS
	Niveles De Privilegio De Canal
	
	Originalmente, los siguientes niveles de privilegio son definidos:
	
	   Founder   Acceso completo a las funciones de %S; obtención
	                 automática de OPs al entrar al canal.  Note
	                 que solo una persona puede tener estatus de
	                 fundador (el cual no puede ser dado usando el
	                 comando ACCESS).
	      %4d   Acceso al comando AKICK; obtención automática de OPs.
	      %4d   Obtención automática de OPs.
CHAN_HELP_ACCESS_LEVELS_HALFOP
	      %4d   Obtención automática de MitadOP.
CHAN_HELP_ACCESS_LEVELS_END
	      %4d   Obtención automática de vóz (voice +v).
	         0   Ningún privilegio especial; otros operadores pueden
	                 darle OPs (a menos que SECUREOPS este fijado).
	        -1   No puede obtener status de Operador del canal (OPs).
	      -100   No puede ingresar al canal.
	
	Estos niveles (Excepto los dos últimos) pueden ser cambiados, o nuevos agregados,
	usando el comando LEVELS; escriba /msg %S HELP LEVELS para más
	información.

CHAN_HELP_LEVELS
	Sintaxis: LEVELS Canal SET Tipo Nivel
	          LEVELS Canal {DIS | DISABLE} Tipo
	          LEVELS Canal LIST
	          LEVELS Canal RESET
	
	El comando LEVELS permite un control fino sobre los accesos
	numéricos utilizados por los canales.  Con este comando,
	usted puede definir los niveles requeridos para muchas de
	las funciones de %S.  Los comandos SET FOUNDER,
	SET PASSWORD, así como éste comando están siempre
	restringidos al fundador.
CHAN_HELP_LEVELS_XOP
	
	¡Cuidado!:  ¡Este comando es solo para usuarios avanzados!  Cambiar
	las opciones de nivel con este comando puede causar que los comandos
	SOP, AOP, VOP, y NOP dejen de funcionar correctamente.
CHAN_HELP_LEVELS_XOP_HOP
	
	¡Cuidado!:  ¡Este comando es solo para usuarios avanzados!  Cambiar
	las opciones de nivel con este comando puede causar que los comandos
	SOP, AOP, HOP, VOP, y NOP dejen de funcionar correctamente.
CHAN_HELP_LEVELS_END
	
	LEVELS SET permite el nivel de acceso para una función o grupo
	de funciones a ser cambiadas.  LEVELS DISABLE (o DIS abreviado)
	deshabilita una opción automática o prohibe el acceso a una
	función por cualquiera que no sea el fundador del canal.
	LEVELS LIST muestra el nivel actual para cada función o
	grupo de funciones.  LEVELS RESET reajusta los niveles a los
	niveles originales de un canal recien creado (vea
	HELP ACCESS LEVELS para más información).
	
	Para una lista de las opciones y funciones a los que se le puede
	ajustar los niveles, vea HELP LEVELS DESC.

CHAN_HELP_LEVELS_DESC
	Los siguientes nombres de valores/funciones son entendidos:

CHAN_HELP_AKICK
	Sintaxis: AKICK Canal ADD Máscara [Razón]
	          AKICK Canal DEL Máscara
	          AKICK Canal LIST [+Inicio] [Máscara]
	          AKICK Canal VIEW [+Inicio] [Máscara]
	          AKICK Canal COUNT
	          AKICK Canal ENFORCE
	
	Mantiene la lista de Auto-Pateo (AKICK) para un canal.  Si un
	usuario en la lista de Auto-Pateo intenta ingresar al canal,
	%S lo bloqueará (ban) del canal, despues lo pateará (kick).
	
	El comando AKICK ADD agrega la máscara usuario@dominio dada o
	Apodo!Usuario@dominio a la lista de auto-pateo.  Si una
	razón es dada con el comando, esa razón será usada cuando el
	usuario sea pateado.
	
	El comando AKICK DEL elimina la máscara dada de la lista AKICK.
	Mas sin embargo no remueve otros bloqueos (bans) causados por un AKICK
	ya existentes en el canal, estos deberán ser removidos manualmente.
	
	El comando AKICK LIST muestra la lista de auto-pateo (AKICK), u
	opcionalmente solo aquellas entradas de auto-pateo que concuerden con
	la máscara dada.  AKICK VIEW es similar, pero muestra más detalles.
	Si el parámetro Inicio es especificado en cualquiera de los dos comandos,
	entonces especifica cuantas entradas ignorar antes de mostrar los resultados
	(por ejemplo, +50 causará que las primeras 50 entradas sean
	ignoradas).
	
	El comando AKICK COUNT retorna cuantas entradas existen
	en la lista de Auto-Pateo (AKICK).
	
	El comando AKICK ENFORCE causa que %S imponga la lista AKICK
	actual por medio de remover aquellos usuarios que concuerdan con
	una máscara AKICK .
	
	Originalmente, esto está limitado a usuarios con acceso %s y superior
	en el canal.

CHAN_HELP_INFO
	Sintaxis: INFO Canal [ALL]
	
	Lista información sobre el canal registrado especificado,
	incluyendo su fundador, fecha de regístro, última vez usado,
	descripción, y modo lock (candado), si existe.  Si usted se
	ha identificado como el fundador del canal del cual pide
	información y ALL es especificado, el mensaje de entrada
	y el sucesor también serán mostrado.

CHAN_HELP_LIST
	Sintaxis: LIST [+Inicio] Patrón
	
	Lista todos los canales registrados que coincidan con el patrón dado.
	(Canales con la opción PRIVATE fijada no son alistados.)
	Si el parámetro Inicio es dado, entonces especifica cuantas
	entradas ignorar antes de mostrar los resultados (por ejemplo, +50
	causará que las primeras 50 entradas sean ignoradas).
CHAN_HELP_LIST_OPERSONLY
	
	Solamente los operadores de IRC pueden utilizar este comando.

CHAN_HELP_OP
	Sintaxis: OP Canal [Apodo]
	
	Da estatus de OP (modo +o) al apodo seleccionado en un canal.  Si Apodo no es especificado
	entonces le da a usted los OPs.  Originalmente, limitado a usuarios con acceso %s
	o superior en un canal.

CHAN_HELP_DEOP
	Sintaxis: DEOP Canal Apodo
	
	Remueve el estatus de OP (modo +o) de un apodo seleccionado en un canal.  Si Apodo
	no es especificado, Remueve los OPs de usted.  Originalmente, limitado a usuarios
	con acceso %s o superior en el canal.

CHAN_HELP_VOICE
	Sintaxis: VOICE Canal [Apodo]
	
	Da voz (modo +v) a un apodo seleccionado nick en un canal.  Si Apodo
	no es especificado, Le da voz a usted.  Originalmente, limitado
	a usuarios con acceso %s o superior en el canal.

CHAN_HELP_DEVOICE
	Sintaxis: DEVOICE Canal [Apodo]
	
	Remueve la voz (modo +v) de el apodo seleccionado en un canal.  Si Apodo
	no es especificado, remueve la voz de usted.  Originalmente, limitado
	a usuarios con acceso %s o superior en el canal.

CHAN_HELP_HALFOP
	Sintaxis: HALFOP Canal [Apodo]
	
	Da acceso MitadOP (modo +h) a un apodo seleccionado en un canal.  Si Apodo
	no es especificado, le da acceso de MitadOP a usted.  Originalmente,
	limitado a usuarios con acceso %s o superior en el canal.

CHAN_HELP_DEHALFOP
	Sintaxis: DEHALFOP Canal [Apodo]
	
	Remueve el acceso de MitadOP (modo +h) de un apodo seleccionado en un canal.  Si
	Apodo no es especificado, entonces le remueve el acceso MitadOP a usted mismo.
	Originalmente, limitado a usuarios con acceso %s o superior en el canal.

CHAN_HELP_PROTECT
	Sintaxis: PROTECT Canal [Apodo]
	
	Da estatus de protegido en el canal (modo +a) a un apodo seleccionado en un
	canal.  Si Apodo no es especificado, le da estatus de protegido en el canal
	a usted mismo.  Originalmente, limitado a usuarios con acceso %s o superior
	en el canal.

CHAN_HELP_DEPROTECT
	Sintaxis: DEPROTECT Canal [Apodo]
	
	Remueve el estatus de protegido en el canal (modo +a) de un apodo seleccionado
	en un canal.  Si Apodo no es especificado, entonces le remueve el estatus de
	protegido en el canal a usted mismo.  Originalmente, limitado a usuarios con
	acceso %s o superior en el canal.

CHAN_HELP_INVITE
	Sintaxis: INVITE Canal
	
	Instruye a %S que lo invite a usted a el canal dado.  Originalmente,
	limitado a usuarios con acceso %s o superior en
	el canal.
	
	Note: Si usted recibe un mensaje diciendo que el canal no está en uso,
	eso significa que el canal está vacío, y usted puede ingresar a este
	libremente sin necesidad de utilizar este comando.

CHAN_HELP_UNBAN
	Sintaxis: UNBAN Canal
	
	Instruye a  %S que remueva todos los bloqueos (bans) que le impiden a
	usted el ingreso al canal dado.  Originalmente, limitado a usuarios
	con acceso %s o superior en el canal.

CHAN_HELP_KICK
	Sintaxis: KICK Canal Apodo [Razón]
	
	Instruye a %S que patee (kick) al apodo dado de el canal
	dado.  El mensaje incluido en el pateo mostrará tanto quien
	utilizó el comando KICK, como también la razón dada con el
	comando, si una es especificada.  Originalmente, limitado a
	usuarios con acceso %s o superior en el canal.

CHAN_HELP_KICK_PROTECTED
	
	Note que los usuarios protegidos (+a) no pueden ser pateados
	con este comando, ni siquiera por el fundador del canal.

CHAN_HELP_TOPIC
	Sintaxis: TOPIC Canal Tópico
	
	Causa que %S cambie el tópico del canal a aquel que se
	ha especificado.  Este comando es más útil en conjunción
	con SET TOPICLOCK.  Originalmente, limitado a usuarios
	con acceso %s o superior en el canal.

CHAN_HELP_CLEAR
	Sintaxis: CLEAR Canal Algo
	
	Le dice a %S que borre ciertos ajustes en un canal.  Algo
	puede ser cualquiera de las siguientes opciones:
	
	    MODES       Borra todos los modos en el canal excepto aquellos
	                    modos fijados con la opción MLOCK (candado).
	    BANS        Borra todos los bans (bloqueos) en el canal.
CHAN_HELP_CLEAR_EXCEPTIONS
	    EXCEPTIONS  Borra todas las excepciones en el canal.
CHAN_HELP_CLEAR_INVITES
	    INVITES     Remueve todas las máscaras de invitación en el canal.
CHAN_HELP_CLEAR_MID
	    OPS         Remueve el estatus de operador de canal (modo +o)
	                    de todos los usuarios.
CHAN_HELP_CLEAR_HALFOPS
	    HALFOPS     Remueve estatus MitadOP (modo +h) de todos los usuarios.
CHAN_HELP_CLEAR_END
	    VOICES      Remueve el estatus "voice" (modo +v) de todo usuario.
	    USERS       Remueve o Patea (kicks) todos los usuarios del canal.
	
	Originalmente, limitado a usuarios con acceso %s o superior
	en el canal.

CHAN_HELP_STATUS
	Sintaxis: STATUS Canal Apodo
	
	Muestra el nivel de acceso curriente del apodo dado en el
	canal dado.  La repuesta es en el formato:
	
	    STATUS Canal Apodo Nivel-De-Acceso
	
	Si ocurre un error, la respuesta será en el formato:
	
	    STATUS Canal Apodo ERROR Mensaje-De-Error
	
	Originalmente, limitado a usuarios con acceso %s o superior
	en el canal.

CHAN_OPER_HELP_COMMANDS
	
	Comandos disponibles para los administradores de Servicios:
	
	    DROPCHAN   Permite el dar de baja a cualquier canal
CHAN_OPER_HELP_COMMANDS_GETPASS
	    GETPASS   Recobre la contraseña de fundador para un canal
	                  (solo si encripción está deshabilitada)
CHAN_OPER_HELP_COMMANDS_FORBID
	    FORBID    Prevenga el que un canal sea registrado/usado
	    SUSPEND   Prevenga el que un canal registrado sea usado
	    UNSUSPEND  Cancele la suspensión de un canal
CHAN_OPER_HELP_COMMANDS_END
	
	Administradores de Servicios también pueden pueden ver las listas
	de acceso, AKICK, y de ajuste de niveles o utilizar el comando
	STATUS para cualquier canal.

CHAN_OPER_HELP_DROPCHAN
	Syntax: DROPCHAN Canal
	
	Da de baja al canal nombrado en la base de datos de %S.
	
	Limitado a los Administradores De Los Servicios.


CHAN_OPER_HELP_SET
	
	Administradores de Servicios también pueden fijar la opción NOEXPIRE,
	con la cual se puede prevenir el que los canales expiren. Adicionalmente,
	los Administradores de Servicios pueden fijar opciones para cualquier
	canal sin identificarse via contraseña por el canal.

CHAN_OPER_HELP_SET_NOEXPIRE
	Sintaxis: SET Canal NOEXPIRE {ON | OFF}
	
	Ajusta si el canal especificado expirará o no.  Ajustar esta opción
	a ON previene que el canal expire.
	
	Limitado a los Administradores De Servicios.

CHAN_OPER_HELP_UNSET
	
	Igual que con el comando SET, los Administradores de Servicios pueden
	borrar ajustes para cualquier canal sin identificarse por el canal.

CHAN_OPER_HELP_INFO
	
	Administradores de Servicios pueden usar el parámetro ALL con cualquier canal.

CHAN_OPER_HELP_LIST
	Sintaxis: LIST [+Inicio]Patrón [FORBIDDEN] [NOEXPIRE] [SUSPENDED]
	
	Lista todos los canales registrados que concuerden con el patrón dado.
	Canales con la opción PRIVATE activada solamente serán mostrados a
	los administradores de Servicios. Canales con la opción NOEXPIRE activada
	tendrán un signo ! antecediendo al nombre del canal, Esto en el caso de
	los administradores de servicios.
	
	Si el parámetro "Inicio" es dado, este especifica cuantas entradas
	similares serán ignoradas antes de mostrar los resultados (por ejemplo, +50
	causaria que las primeras 50 entradas similares fueran ignoradas).
	
	Si las opciones FORBIDDEN, SUSPENDED o NOEXPIRE son dadas, solo
	aquellos canales que, respectivamente, tienen las opciones FORBID, SUSPEND
	o que tienen el modo NOEXPIRE activado serán mostrados. Las opciones pueden
	ser utilizadas en cualquier orden y en cualquier combinación. Estas opciones
	están limitadas a los Administradores de Servicios.

CHAN_OPER_HELP_GETPASS
	Sintaxis: GETPASS Canal
	
	Retorna la contraseña para el canal indicado.  Note que
	siempre que este comando es usado, un mensaje incluyendo la
	persona que ejuctó el comando y el canal para el cual fue
	usado será archivado y enviado en forma de WALLOPS/GLOBOPS.
	
	Si encripción está en uso, este comando puediera no estar disponible
	dependiendo del tipo de encripción que se este usando en la red.
	
	Limitado a los Administradores De Servicios.

CHAN_OPER_HELP_FORBID
	Sintaxis: FORBID Canal
	
	Deshabilita el que alguien registre o utilice el canal indicado.
	Esto puede ser revertido por medio de eliminar el canal (drop).
	
	Limitado a los Administradores De Servicios.

CHAN_OPER_HELP_SUSPEND
	Sintaxis: SUSPEND [+Expiración] Canal Razón
	
	Previene el que un canal sea utilizado o que alguien se
	identifique por él.  Se puede lograr que el canal se utilice
	nuevamente por medio del comando UNSUSPEND, lo cual
	permitirá su uso como antes.  El parámetro Expiración es
	interpretado en la misma manera que el comando %s AKILL.
	
	Limitado a los Administradores De Servicios.

CHAN_OPER_HELP_UNSUSPEND
	Sintaxis: UNSUSPEND Canal
	
	Permite a los usuarios el uso y el que se identifiquen por un
	canal que habia sido previamente suspendido (suspend).
	
	Limitado a los Administradores De Servicios.


###########################################################################
#
# MemoServ help messages
#
###########################################################################

MEMO_HELP
	%S es una utilería que le permite a los usuarios de IRC
	el envio de mensajes cortos a otros usuarios de la misma red,
	independientemente de si estos están en línea o no, igualmente
	se puede enviar mensajes a canales(*).  Ambos apodos remitente
	y destinatario, asi como el canal al que se envia deben estar
	registrados para poder enviar un mensaje. Escriba
	/msg %S HELP COMMANDS para ver la lista de comandos de %S;
	para usar un comando, escriba /msg %S Comando, o para más
	información sobre un comando, escriba /msg %S HELP Comando.
MEMO_HELP_EXPIRES
	
	Nota:  Los mensajes expirarán ( automáticamente borrados)
	%s despues de haber sido enviados, a menos que se hayan marcado
	como para no expirar con el comando SAVE.
MEMO_HELP_END_LEVELS
	
	(*) Originalmente, los mensajes del canal son enviados a todos los usuarios
	    (incluyendo el fundador) siempre que estos tengan al menos acceso %s en el canal.
	    Esto puede ser cambiado con el comando %s LEVELS.
MEMO_HELP_END_XOP
	
	(*) Los mensajes del canal serán enviados al fundador del canal y
	    a todos los usuarios en la lista SOP del canal.

MEMO_HELP_COMMANDS
	Los siguientes comandos pueden ser utilizados con %S:
	
	    SEND   Envie un mensaje a un apodo o canal
	    LIST   Liste sus mensajes
	    READ   Lea un mensaje o mensajes
MEMO_HELP_COMMANDS_FORWARD
	    FORWARD   Reexpida mensajes a su dirección e-mail
MEMO_HELP_COMMANDS_SAVE
	    SAVE      Define el que ciertos mensajes no expiren
MEMO_HELP_COMMANDS_DEL
	    DEL    Borre un mensaje o mensajes
	    RENUMBER  Renumere sus mensajes
	    SET    Ajuste opciones relacionadas a los mensajes
	    INFO   Muestra información relacionada a los mensajes
MEMO_HELP_COMMANDS_IGNORE
	    IGNORE    Ignore los mensajes de ciertos usuarios

MEMO_HELP_SEND
	Sintaxis: SEND {Apodo | Canal} Texto-Del-Mensaje
	
	Le envia al apodo o canal indicado un mensaje conteniendo el
	texto-del-mensaje.  El recipiente recibirá una nota informandole
	que él/ella tiene un nuevo mensaje. El apodo/canal al que se envie
	debe estar registrado.

MEMO_HELP_LIST
	Sintaxis: LIST [Núm | List | NEW]
	
	Lista cualquier mensaje que usted pueda tener actualmente.  Con NEW,
	lista solamente nuevos mensajes (sin leer).  Mensajes no leidos se
	marcan con el signo * a la izquierda del número del mensaje; los
	mensajes del canal son marcados con el signo "#". Usted también
	puede especificar una lista de números, como en el ejemplo abajo:
	
	   LIST 2-5,7-9
	      Lista mensajes enumerados de 2 hasta el 5 y 7 hasta el 9.

MEMO_HELP_LIST_EXPIRE
	Sintaxis: LIST [Núm | List | NEW]
	
	Alista todos sus mensajes actuales.  Con NEW, alista solo los mensajes
	nuevos (sin leer).  Mensajes aún sin leer son marcados con un signo "*"
	a la inzquierda de el número del mensaje; los mensajes del canal son
	marcados con el signo "#", asi también los mensajes que no expirarán son
	marcados con un signo "+".  Usted también puede especificar una lista de
	númmeros, como en el siguiente ejemplo:
	
	   LIST 2-5,7-9
	      Alista mensajes enumberados de 2 hasta el 5 y 7 hasta el 9.

MEMO_HELP_READ
	Sintaxis: READ {Núm | List | LAST | NEW}
	
	Le envia el texto de los mensajes especificados.  Si se especifica
	LAST, Le envia el mensaje más reciente recibido por usted.  Si se
	especifica NEW, le envia todos sus nuevos mensajes.  De lo contrario,
	le envia el número Núm del mensaje.  Usted también puede especificar
	una lista de números, como con el comando LIST.

MEMO_HELP_SAVE
	Sintaxis: SAVE {Núm | List}
	
	Marca el mensaje o mensajes dados como unos que no expirarán.  Usted puede
	simplemente dar o bien el número de un mensaje o, como con el comando LIST,
	una lista de mensajes.

MEMO_HELP_DEL
	Sintaxis: DEL {Núm | List | ALL}
	
	Borra los mensajes o mensaje especificados.  Usted puede
	suplir multiples números o un rango de números en lugar de
	un simple número, tal como con el comando LIST; también se
	pude especificar el parámetro ALL, lo cual causará que
	todos sus mensajes sean borrados.
	
	Advertensia: ¡Una vez que un mensaje ha sido borrado, no se
	puede recuperar!

MEMO_HELP_RENUMBER
	Sintaxis: RENUMBER
	
	Renumera su lista de mensajes, removiendo "hoyos" en la sequencia de
	números generada al borrar mensajes.

MEMO_HELP_SET
	Sintaxis: SET Opción Parámetros
	
	Ajusta varias opciones del mensaje.  opción puede ser:
	
	    NOTIFY      Cambia cuándo será usted notificado de
	                    nuevos mensajes (Solamente apodos)
	   LIMIT       Fija el número máximo de mensajes que
	                    usted puede recibir
MEMO_HELP_SET_OPTION_FORWARD
	    FORWARD     Establece el que si los mensajes serán
	                automáticamente reexpedidos a usted
MEMO_HELP_SET_END
	
	Escriba /msg %S HELP SET opción para más información
	sobre una opción específica.

MEMO_HELP_SET_NOTIFY
	Sintaxis: SET NOTIFY {ON | LOGON | NEW | OFF}
	
	Cambia cuándo será notificado usted de nuevos mensajes:
	
	    ON      Usted será notificado de mensajes cuando se
	               conecte, cuando desactive /AWAY, y cuando
	               estos le sean enviados a usted.
	    LOGON   Usted será notificado de mensajes solamente al
	               conectarse o cuando desactive /AWAY.
	    NEW     Usted será notificado de mensajes solamente
	               cuando estos le sean enviados.
	    OFF     No recibirás notificación alguna de mensajes.
	
	ON es equivalente a LOGON y a NEW combinados.
	
	Note que si su apodo tiene la opción "secure" activada,
	usted será solamente notificado de mensajes nuevos (para
	LOGON u ON) despues de indentificarse a %s.

MEMO_HELP_SET_LIMIT
	Sintaxis: SET LIMIT Límite
	
	Fija el número máximo permitido de mensajes que usted puede tener.
	Si se fija esto a 0, nadie podrá enviar mensaje alguno a usted.
	Sin embargo, usted no puede fijar esto a un número mayor que %d.
	
MEMO_HELP_INFO
	Sintaxis: INFO
	
	Muestra información sobre el número de mensajes que usted
	tiene, cuántos de estos están sin leer, cuántos mensajes
	en total usted puede recibir y cuándo será usted notificado
	de los nuevos mensajes.

MEMO_OPER_HELP_COMMANDS
	
	Administradores de servicios pueden usar los comandos SET
	LIMIT y INFO con apodos que no sean los suyos propios.

MEMO_OPER_HELP_SET_LIMIT
	Sintaxis: SET LIMIT [Apodo] {Límite | NONE | DEFAULT} [HARD]
	
	Fija el número máximo permitido de mensajes que un apodo puede
	tener.  Si se fija el límite a 0 previene que el apodo reciba
	mensaje alguno; fijandolo a NONE le permitirá al apodo recibir y
	mantener cuantos mensajes desee; y fijandolo a DEFAULT restaurá
	el límite a el valor original (%d).  Si usted no indica
	un apodo, su propio límite es ajustado
	
	Agregar HARD previene que el usuario cambie el límite.  No
	agregar HARD tiene el efecto opuesto, permitiendo entonces
	que el usuario cambie el límite (inclusive si un límite previo
	fue fijado con HARD).
	
	Este uso del comando SET LIMIT es limitado a los Administradores
	De Servicios.  Otros usuarios solo pueden establecer su propio límite, no
	pueden remover su límite o fijarlo a lo original, no pueden fijar un límite
	superior a %d, y no pueden fijar un límite duro (HARD).

MEMO_OPER_HELP_INFO
	Sintaxis: INFO [Apodo]
	
	Sin un parámetro, muestra información sobre el número de
	mensajes que usted tiene, cuántos están sin leer, cuántos
	mensajes en total usted puede recibir y cuándo es que se
	le notifica a usted de nuevos mensajes.
	
	Con un parámetro de apodo, muestra la misma información para
	el apodo indicado.  Este comando está limitado a los
	Administradores De Servicios.

MEMO_HELP_FORWARD
	Sintaxis: FORWARD {Núm | List | ALL}
	
	Reexpide uno o más de sus mensajes a su correo electrónico.  Usted debe
	haber fijado una dirección de correo electrónico válida para su apodo
	para que le sea posible utilizar este comando.  Mensajes a canales no
	pueden ser reexpedidos.

MEMO_HELP_SET_FORWARD
	Sintaxis: SET FORWARD {ON | COPY | OFF}
	
	Fija si %S automáticamente reexpide los mensajes enviados a usted
	a su dirección de correo electrónico.  Fijado a ON,  reexpide los
	mensajes enviados a usted a el correo electrónico acreditado a su
	apodo en lugar de enviarlos a usted en IRC.  COPY es similar, pero
	también instruye a %S que mantenga una copia del mensaje para
	leerlo en linea.  Fijado a OFF, sus mensajes serán almancenados en
	linea como es usual.  Incluso si usted fija esta opción a ON, usted
	aún podrá recibir mensajes en linea si los servicios son incapaces de
	reexpedirlos a usted.
	
	Note que cuando esta opción es fijada a COPY y usted ha recibido
	el número  máximo de mensajes que le son permitidos, no le será
	posible a usted recibir mensaje nuevo alguno (incluyendo por correo
	electrónico) hasta que haya borrado algunos mensajes viejos usando
	el comando DEL.

MEMO_HELP_IGNORE
	Sintaxis: IGNORE {ADD | DEL | LIST} [Apodo | Máscara]
	
	Controla la lista de usuarios "ignorados"; los usuarios de los cuales el apodo
	o su máscara usuario@proveedor-web  concuerden con una entrada en esta lista
	no les será permitido enviarle mensajes a usted.  ADD agrega el apodo dado o
	la máscara usuario@proveedor-web a su lita de ignorar, y DEL remueve el apodo
	dado o la máscara de dicha lista; LIST muestra su lista actual de ignorar.
	
	Note que el apodo o máscara dada en el comando ADD puede contener los
	signos "*" o "?", y esto no es sensitivo a mayúsculas or minúsculas; por
	ejemplo, "*Evil*" concuerda con "EvilNick", "AN_EVIL_USER" y también con
	"devil".


###########################################################################
#
# OperServ help messages
#
###########################################################################

OPER_HELP
	%S permite a los operadores de IRC controlar varios aspectos
	de la red de IRC.  Escriba /msg %S HELP COMMANDS para ver la
	lista de comandos.
	
	Nota: ¡Todos los comandos enviados a %S son archivados!

OPER_HELP_COMMANDS
	Comandos de %S:
	    GLOBAL      Envie un mensaje a todos los usuarios
	    STATS       Muestra el estatus de los Servicios y red
	    SERVERMAP   Muestra un mapa de los servidores en la red
	    OPER LIST   Lista todos los Operadores de Servicios
	    ADMIN LIST  Lista todos los Administradores de Servicios
OPER_HELP_COMMANDS_SERVOPER
	
	Comandos solo para Administradores y Operadores de Servicios:
	    MODE        Cambia los modos de un canal
	    KICK        Patea (kicks) a un usuario de un canal
	    CLEARMODES  Remueve ciertos modos de un canal
	    CLEARCHAN   Remueve todos los usuarios de un canal (pateo masivo)
	    KILLCLONES  Desconecta (kills) todos los usuarios con cierta máscara
OPER_HELP_COMMANDS_AKILL
	    AKILL       Manipula la lista de auto-desconeccíon (AKILL)
	    AKILLCHAN   Auto-desconecta todos los usuarios en un canal
OPER_HELP_COMMANDS_EXCLUDE
	    EXCLUDE     Manipula la lista de exclusión de auto-desconexión (AKILL)
OPER_HELP_COMMANDS_SLINE
	    SGLINE      Manipula la lista SGLINE
	    SQLINE      Manipula la lista SQLINE
	    SZLINE      Manipula la lista SZLINE
OPER_HELP_COMMANDS_SESSION
	    SESSION     Vea la lista de sesiones para una máscara
	    EXCEPTION   Modifica la lista de excepción de límite de sesiones
OPER_HELP_COMMANDS_NEWS
	    LOGONNEWS   Define mensajes que ven los usuarios al conectarse
	    OPERNEWS    Define mensajes que ven los usuarios que usan /OPER
OPER_HELP_COMMANDS_SERVADMIN
	
	Comandos disponibles solo a los Administradores de Servicios:
	    OPER        Modifica la lista de Operadores de Servicios
	    JUPE        Impone el comando "Jupiter" para un servidor
	    SET         Ajusta varias opciones globales de los Servicios
	    UPDATE      Fuerza una actualización immediata en el disco de las
	                    bases de datos de los servicios
	    SHUTDOWN    Graba la base de datos al disco y detiene los servicios
	    QUIT        Detiene los Servicios sin grabar la base de datos al disco
	    RESTART     Graba la base de datos al disco y recomienza los servicios
	    REHASH      Re-lee los ficheros de configuración
	    SU          Obtenga privilegios de Super-Usuario de los Servicios
OPER_HELP_COMMANDS_SERVROOT
	
	Comandos disponibles solo al Super-Usuario de los Servicios:
	    ADMIN       Modifica la lista de Administradores De Los Servicios
OPER_HELP_COMMANDS_RAW
	    RAW         Envia un comando "raw" al servidor de IRC

OPER_HELP_GLOBAL
	Sintaxis: GLOBAL Mensaje
	
	Permite a los IRCops enviar mensajes a todos los usuarios en la red.
	El mensaje será enviado desde el apodo %s.

OPER_HELP_STATS
	Sintaxis: STATS [RESET | NETWORK | ALL]
	
	Sin ninguna opción, muestra el número corriente de usuarios y IRCops
	en línea (excluyendo los Servicios), el número mas alto de usuarios en
	línea desde que los Servicios comenzaron, asi como la cantidad de
	tiempo que ha pasado desde que los Servicios han estado corriendo.
	
	Con la opción RESET, reajusta la cuenta máxima de usuarios a la
	del número de usuarios corrientes en línea.
	
	Con la opción NETWORK, muestra la cantidad de data enviada a
	y recibida desde el servidor remoto, asi como la cantidad de memoria
	usada por las prelecturas de enchufe de la red. El rátio de uso de la
	memoria de prelectura corriente contra la dimensión del límite de la
	prelectura es también mostrado en porcentage si se configuró un límite.
	
	La opción ALL está disponible solamente a los administradores de Servicios,
	y muestra información sobre la cantidad de memoria usada por los Servicios.
	utilizar esta opción puede conjelar los Servicios por un corto periodo de tiempo
	en redes muy grandes, por lo tanto ¡no la sobreuse!
	
	UPTIME puede ser utilizado como un sinónimo de STATS.

OPER_HELP_SERVERMAP
	Sintaxis: SERVERMAP
	
	Muestra un mapa de los servidores conectados a la red, desde el punto
	de vista de los servicios.  Servidores "Falsos" los servicios mismos o
	cualquier otro  servidor que ha sido sacado de la red con el comando JUPE
	tienen un signo "(*)" que se muestra detras del nombre de el servidor.

OPER_HELP_OPER
	Sintaxis: OPER ADD Apodo
	          OPER DEL Apodo
	          OPER LIST
	
	Habilita a los Administradores de los Servicios para agregar
	(ADD) o a borrar (DEL) usuarios de la lista de operadores.
	Un usuario que se encuentre en la lista de operadores y
	que se identifique ante %s podrá acceder a los comandos
	de Operador de Servicios.
	
	Cualquiera de los operadores de IRC (IRCops) puede utilizar
	el comando OPER LIST. Todos los otros comandos solo pueden
	ser utilizados por los Administradores de Servicios.

OPER_HELP_ADMIN
	Sintaxis: ADMIN ADD Apodo
	          ADMIN DEL Apodo
	          ADMIN LIST
	
	Habilita a los súper-usuarios de los Servicios a agregar (ADD)
	o a eliminar (DEL) usuarios de la lista de los administradores.
	Un usuario que se encuentre en la lista de operadores y que se
	identifique ante %s podrá acceder a los comandos de los
	Administrador de Servicios.
	
	Cualquiera de los operadores de IRC (IRCops) puede utilizar
	el comando ADMIN LIST.  Todos los otros comandos solo pueden
	ser utilizados por el súper-usuarios de los Servicios.

OPER_HELP_GETKEY
	Sintaxis: GETKEY Canal
	
	Muestra la contraseña de canal fijada en el canal dado, si es que esta existe.
	
	Limitado a los Operadores De Los Servicios.

OPER_HELP_MODE
	Sintaxis: MODE Canal Modos
	
	Habilita a los operadores de Servicios a establecer los
	modos para cualquier canal.  Los parámetros son los mismos
	que se utilizan en la forma estándar de /MODE.
	
	Solo disponible para los Operadores de Servicios.

OPER_HELP_CLEARMODES
	Sintaxis: CLEARMODES Canal [ALL]
	
	Elimina los modos de canal (n,s,t,k,l,b ect) de un canal. Si el
	parámetro ALL es dado, también remueve todos los modos de usuario
	(tales como +o y +v) del canal. Mas sin embargo, si el canal está
	registrado, sus modos de candado (MLOCK) aún estarán en efecto.
	
	Solo disponible a los Operadores De Servicios.

OPER_HELP_CLEARCHAN
	Sintaxis: CLEARCHAN Canal
	
	Remueve todos los usuarios de un canal (pateo masivo).
	
	Limitado a los Operadores De Servicios.

OPER_HELP_KICK
	Sintaxis: KICK Canal Usuario Razón
	
	Habilita a un IRCop a sacar a un usuario de cualquier canal.
	Los parámetros son los mismos que para un /KICK común.  El
	mensaje de kick mostrará al usuario el nombre del IRCop que
	está enviando el comando. Por ejemplo:
	
	*** SpamMan has been kicked off channel #my_channel by %S (Alcan (Flood))
	
	Solo disponible para los Operadores de Servicios.

OPER_HELP_AKILL
	Sintaxis: AKILL ADD [+Expiración] Máscara Razón
	          AKILL DEL Máscara
	        AKILL CLEAR ALL
	        AKILL LIST [+Inicio] [Patrón] [NOEXPIRE]
	        AKILL VIEW [+Inicio] [Patrón] [NOEXPIRE]
	        AKILL CHECK Máscara
	          AKILL COUNT
	
	Permite a los Operadores de servicios manipular la lista AKILL.
	Si un usuario que figura en dicha lista, intenta conectarse, los
	Servicios impondrán un KILL para el usuario y darán instrucciones
	a todos los servidores donde esta opción sea disponible, que agreguen
	un ban (k-line) para la máscara que coincida con la del usuario.
	
	AKILL ADD agrega el usuario dado con la máscara usuario@hospedaje a la
	lista de akill. La razón debe describir el porqué es agregada la entrada;
	dependiendo de cómo esten configurados los servicios, esta pudiera también
	ser mostrada a los usuarios en los mensajes AKILL. Una fecha de
	expiración también puede ser especificada, como se describe abajo.
	
	AKILL DEL remueve la máscara dada de la lista AKILL si la misma
	está presente.
	
	AKILL CLEAR elimina (remueve) todas las máscaras de la lista
	autokill (AKILL).  El parámetro ALL es mandatorio, para evitar
	el uso accidental de este comando.
	
	AKILL LIST muestra las entradas actuales en la lista autokill;
	si el patrón opcional es dado, entonces listado estará limitado a
	aquellas entradas similares al patrón dado.  Si el parámetro "Inicio"
	es dado entonces especifica cuantas entradas similares ignorar antes de
	mostrar los resultados (por ejemplo, +50 causaria que las primeras 50
	entradas similares fueran ignoradas). Adicionalmente, NOEXPIRE se puede
	usar para limitar el listado a aquellas entradas que no expiran.
	
	AKILL VIEW es más explícito que AKILL LIST, dado que muestra quién
	agregó un AKILL, la fecha en que fue añadido, cuándo expira, asi como
	también la máscara usuario@hospedaje utilizada y la razón.
	
	AKILL CHECK mostrará todas las entradas en la lista autokill en
	las que la máscara usuario@hospedaje dada sean similares; esto se
	puede utilizar para determinar cuál autokill causó que un usuario
	en particular fuese desconectado.
	
	AKILL COUNT simplemente muestra el número de autokills en
	la lista AKILL.
	
	Opcionalmente, una fecha de expiración también puede ser
	suministrada con el comando AKILL ADD.  La fecha de
	expiración PRECEDE a la máscara usuario@hospedaje y deberá
	ser especificado como un entero seguido o bien por d (días),
	h (horas), o por m (minutos). Combinaciones (como 1h30m) son
	también permitidas.  Si un especificador de unidades no es
	incluido, originalmente se asumen DIAS (entonces, poner +30
	sin nada, significa 30 días).  Para agregar un AKILL que NO
	expira, utilice +0.  Si la máscara de usuario comienza con
	+, un período de expiración deberá ser indicado, aún si es
	el mismo período que el asignado originalmente.
OPER_HELP_AKILL_OPERMAXEXPIRY
	
	Los Operadores De Servicios pueden fijar una fecha de expiración de solamente
	%s; Mientras que los Administradores De Servicios pueden fijar cualquier
	fecha de expiración.
OPER_HELP_AKILL_END
	
	Limitado a los Operadores De Servicios.

OPER_HELP_AKILLCHAN
	Syntax: AKILLCHAN [KILL] [+Expiración] Canal Razón
	
	Añade auto-desconecciones para todos los usuarios excepto para los operadores de
	IRC en el canal especificado, y opcionalmente (si KILL es especificado)
	los remueve de la red.  La Razón será recordada en cada auto-desconexión,
	y puede ser mostrada en los mensajes de desconexión dependiendo de como esten
	configurados los Servicios.  Expiración toma el mismo formato como en el
	comando AKILL ADD; Si no es especificado, lo fija por %s.
	
	Auto-desconecciones añadidas con este comando tendrán el formato
	*@Máscara, para cada máscara usada por un cliente en el
	canal.  Si dicha auto-desconexión ya existe para una máscara,
	esta no será modificada; esto puede causar que el número de
	auto-desconecciones añadidas sea menor que el número de usuarios
	afectados.
	
	Limitado a los Operadores De Servicios.

OPER_HELP_EXCLUDE
	Sintaxis: EXCLUDE ADD [+Expiración] Máscara Razón
	          EXCLUDE DEL Máscara
	          EXCLUDE CLEAR ALL
	          EXCLUDE LIST [+Inicio] [Patrón] [NOEXPIRE]
	          EXCLUDE VIEW [+Inicio] [Patrón] [NOEXPIRE]
	          EXCLUDE CHECK Máscara
	          EXCLUDE COUNT
	
	Permite a los Operadores de servicios manipular la lista de exclusión de
	auto-desconexión (AKILL).  Cuando un usuario que concuerda con una máscara
	de exclusión de auto-desconexión se connecta a la red, Los servicios no
	impondrán un KILL para dicho usuario inclusive si el usuario de otra forma
	concordara con una máscara de auto-desconexión (AKILL) .  Esto es útil si
	usted tiene máscaras de auto-desconexión muy generales, y aún desea que
	ciertos dominios especificos o usuarios que conduerdan con esas máscaras
	le sea posible conectarse a la red.
	
	Los subcomandos ADD, DEL, LIST, VIEW, CHECK, y COUNT funcionan en la misma
	manera en que lo hacen los subcomandos con respecto al comando AKILL; vea
	la ayuda para AKILL para obtener más información.
	
	Limitado a los Operadores De Servicios.

OPER_HELP_SGLINE
	Sintaxis: SGLINE ADD [+Expiración] Máscara Razón
	          SGLINE DEL Máscara
	          SGLINE CLEAR ALL
	          SGLINE LIST [+Inicio] [Patrón] [NOEXPIRE]
	          SGLINE VIEW [+Inicio] [Patrón] [NOEXPIRE]
	          SGLINE CHECK Máscara
	          SGLINE COUNT
	
	Permite a los Operadores de servicios manipular la lista SGLINE.  Si un usuario
	de quien el "real name" concuerda con una máscara SGLINE trata de conectarse,
	Los servicios impondrán un KILL para dicho usuario y, en servidores en los que
	esto esté disponible, instruirán a todos los servidores que agregen un bloqueo
	(G-line en el servidor) para la máscara con la que el usuario concuerda.
	
	SGLINE ADD agrega la máscara de "real name" dada a la lista SGLINE.
	La razón deberia describir el porqué se agrego la entrada; dependiendo
	de cómo los servicios han sido configurados, esta pudiera también ser
	mostrada a los usuarios en los mensajes de KILL.
	
	SGLINE DEL remueve la máscara especificada de la lista SGLINE si esta
	está presente.
	
	SGLINE CLEAR elimina (remueve) todas las máscaras de la lista SGLINE.
	El parámetro ALL es mandatorio, para evitar el uso accidental de este
	comando.
	
	SGLINE LIST muestra todas las entradas SGLINE; si el patrón opcional es
	especificado, el listado se limitará a aquellas entradas que concuerden con
	dicho patrón.  Si el parámetro "Inicio" es dado, entonces este especifica
	cuantas entradas similares serán ignoradas antes de mostrar los resultados
	(por ejemplo, +50 causará que las primeras 50 entradas similares sean
	ignoradas). Adicionalmente, NOEXPIRE puede ser utilizado para limitar
	el listado a las entradas similares que no expiran.
	
	SGLINE VIEW es una versión más detallada de el comando SGLINE LIST, y
	mostrará quién añadió cada entrada SGLINE, la fecha en que fue añadida,
	cuándo fue usada por última vez, y cuándo expira, asi como también la
	máscara y la razón.
	
	SGLINE CHECK mostrará todas las entradas en la lista SGLINE en
	las que la máscara usuario@hospedaje dada sean similares; esto se
	puede utilizar para determinar cuál SGLINE causó que un usuario
	en particular fuese desconectado.
	
	SGLINE COUNT simplemente muestra el número de entradas en la
	lista SGLINE.
	
	El parámetro Máscara puede ser encerrado en comodínes dobles, y
	debe estar cuando la máscara contiene espacios; por ejemplo:
	    /msg %S SGLINE ADD +0 "nombre malo" Este es un nombre prohibido.
	
	Opcionalmente, una fecha de expiración puede ser dada con el comando
	SGLINE ADD.  La fecha de expiración precede a la máscara usuario@hospedaje,
	y es especificada con un entero seguido por o bien d (días),
	h (horas), o m (minutos).  Combinaciones (tales como 1h30m)
	son también permitidas.  Si un especificador de unidad no es incluido,
	lo fijado serán días (por lo tanto +30 por si mismo significa 30 días).
	Para agregar una entrada que no expire, utilice +0.  Si la máscara que
	se añadirá comienza con un + y no se especifica una fecha de expiración,
	la máscara debe ser encerrada entre comodínes dobles, justo como lo hemos
	descrito arriba.
	
	Limitado a los Operadores De Servicios.

OPER_HELP_SQLINE
	Sintaxis: SQLINE ADD [+Expiración] Máscara Razón
	          SQLINE DEL Máscara
	          SQLINE CLEAR ALL
	          SQLINE LIST [+Inicio] [Patrón] [NOEXPIRE]
	          SQLINE VIEW [+Inicio] [Patrón] [NOEXPIRE]
	          SQLINE CHECK Máscara
	          SQLINE COUNT
	
OPER_HELP_SQLINE_KILL
	Permite a los Operadores de Servicios manipular la lista
	SQLINE.  Si un usuario cuyo apodo concuerda con una máscara
	SQLINE trata de conectarse, o si un usuario trata de cambiar
	su apodo a uno que concuerda con una máscara SQLINE, Los
	Servicios impondrán un KILL para dicho usuario y, en los
	tipos de servidores donde esto este disponible, instruirán a
	todos los servidores que agregen un bloqueo (G-line en el
	servidor) para la máscara con la que el usuario concuerda.
OPER_HELP_SQLINE_NOKILL
	Permite a los Operadores de Servicios manipular la lista
	SQLINE.  Si un usuario cuyo apodo concuerda con una máscara
	SQLINE trata de conectarse, o si un usuario trata de cambiar
	su apodo a uno que concuerda con una máscara SQLINE, Los
	Servicios cambiarán el apodo de dicho usuario a un apodo
	"guest" (Huésped), y enviará un mensaje al usuario que
	normalmente causará que el programa cliente de IRC del
	usuario pida un nuevo apodo.  En tipos de servidores donde
	esté disponible, Los Servicios también instruirán a todos
	los servidores que agregen un bloqueo (Q-line en el
	servidor) para la máscara con la que el usuario concuerda.
OPER_HELP_SQLINE_IGNOREOPERS
	
	Note que los Operadores de IRC no serán afectados por
	máscaras SQLINE.
OPER_HELP_SQLINE_END
	
	Los subcomandos ADD, DEL, CLEAR, LIST, y VIEW, CHECK, y COUNT
	funcionan de la misma manera que los subcomandos para el comando
	SGLINE;  Vea la ayuda para SGLINE para más información.
	
	Limitado a los Operadores De Servicios.

OPER_HELP_SZLINE
	Sintaxis: SZLINE ADD [+Expiración] Máscara Razón
	          SZLINE DEL Máscara
	          SZLINE CLEAR ALL
	          SZLINE LIST [+Inicio] [Patrón] [NOEXPIRE]
	          SZLINE VIEW [+Inicio] [Patrón] [NOEXPIRE]
	          SZLINE CHECK Máscara
	          SZLINE COUNT
	
	Permite a los Operadores de Servicios el manipular la lista SZLINE.  Si un usuario
	cuya dirección IP concuerda con una máscara SZLINE trata de conectarse, Los servicios
	impondrán un KILL para dicho usuario y,en servidores en los que esto este disponible,
	instruirán a todos los servidores que agregen un bloqueo (Z-line en el servidor) para
	la máscara con la que el usuario concuerda.
	
	Los subcomandos ADD, DEL, CLEAR, LIST, VIEW, CHECK, y COUNT
	funcionan de la misma forma que los subcomandos para el comando
	SGLINE; vea la ayuda para SGLINE para más información.
	
	Limitado a los Operadores De Servicios.

OPER_HELP_EXCEPTION
	Sintaxis: EXCEPTION ADD [+Expiración] Máscara Límite Razón
	          EXCEPTION DEL {Máscara | List}
	          EXCEPTION CLEAR ALL
	          EXCEPTION MOVE Número NuevoNúmero
	          EXCEPTION LIST [[+Inicio] Patrón | list]
	          EXCEPTION VIEW [[+Inicio] Patrón | list]
	          EXCEPTION CHECK hostname
	          EXCEPTION COUNT
	
	Permite a los Operadores de Servicios manipular la lista de las
	máquinas (hosts) que tienen límite de sesiones específicas - permitiendo
	a ciertas máquinas, como los servidores de shell, a que carguen más
	del número permitido de clientes al mismo tiempo. Cuando una máquina
	alcanza su límite de sesiones, culquier cliente que trate de conectarse
	desde aquella máquina será desconectado (killed). Antes de esto, ellos
	serán, notificados via /NOTICE por %S, sobre dónde buscar ayuda acerca de
	límite de sesiones. El contenido de la nota es una opción de configuración.
	
	EXCEPTION ADD agrega la máscara de una máquina a la lista de excepción.
	Note que máscaras tales como  apodo ! usuario @ host  y  usuario @ host  son inválidas!
	Solamente máscaras reales, tales como máquina.host.dom y *.host.dom,
	son permitidas porque el limitado de sesiones no toma en cuenta apodos o
	nombres de usuario. el límite debe ser un número mayor o igual que cero;
	esto determina cuantas sesiones le es permitido cargar a esta máquina (host)
	simultaneamente. (Un valor de cero significa que dicha máquina disfruta
	de sesiónes ilimitadas).
	
	Opcionalmente, una fecha de expiración también puede ser dada con el comando
	EXCEPTION ADD.  La fecha de expiración precederá la máscara de la máquina,
	y es especificada con un entero seguido por o bien d (días), h (horas),
	o m (minutos).  Combinaciones (tales como 1h30m) son también permitidas.
	Si un especificador de unidad no es incluido, entonces lo normal es días (por
	lo tanto +30 por si mismo significa 30 días).  Para agregar una entrada que
	expire, utilice +0.  Si la máscara que se agregará comienza con un +,
	entonces una fecha de expiración debe ser especificada, incluso de ser lo mismo
	a lo fijado como normal en la configuración.
	
	EXCEPTION DEL remueve la máscara dada o entrada enumerada o entradas
	del la lista de excepción (números de entrada pueden ser localizados con
	los parámetros VIEW o LIST).
	
	EXCEPTION CLEAR elimina (remueve) todas las máscaras de la lista
	de excepciónes.  El parámetro ALL es mandatorio, para evitar el uso
	accidental de este comando.
	
	EXCEPTION MOVE mueve el Número de excepción al NuevoNúmero. Si
	NuevoNúmero está ya siendo usado por otra excepción, entonces su
	número será aumentado para hacer espacio.
	
	EXCEPTION LIST y EXCEPTION VIEW muestran cada excepción actual.
	Si se especifica un patrón opcional, la lista está limitada a aquellas
	excepciones que sean similares a dicho patrón (con el parámetro opcional
	"Inicio" especificando el número de entradas a ignorar antes de mosta los
	resultados);  si se da una lista de número, entonces solamente entradas
	en la lista serán mostradas. La diferencia entre los dos comandos es que
	EXCEPTION VIEW es más explícito, mostrando el nombre de la persona
	que agregó la excepción, su límite de sesión, razón, máscara de la
	máquina, y la fecha de expiración con la hora, asi como también la última
	vez que fue "utilizada", i.e., la última vez que se conecto un usuario que
	concordaba con la máscara de sesión.
	
	EXCEPTION CHECK mostrará todas las entradas en la lista de excepción
	que sean similares al hostname (maquina) dado; esto se puede usar
	para determinar cuál excepción, de existir, está siendo aplicada a un
	usuario en particular.
	
	EXCEPTION COUNT muestra el número de entradas en la
	lista de excepción.
	
	Note que al conectarse un cliente, la primera excepción encontrada que
	concuerde será aplicada a aquel cliente.  Listas de excepción
	grandes junto a máscaras de excepción que concuerden apliamente
	afectarán negativamente la eficiencia de los servicios.
	
	Limitado a los Operadores De Servicios.

OPER_HELP_SESSION
	Sintaxis: SESSION LIST Núm-Límite
	          SESSION VIEW Máscara
	
	Permite a Operadores de Servicios ver lista de sesión.
	
	SESSION LIST lista  máscaras con al menos sesiones límites.
	El límite debe ser un número mayor que 1. Esto es para
	prevenir el listado accidental de el gran número de máquinas o
	máscaras con una sola sesión.
	
	SESSION VIEW muestra información detallada sobre una máscara o
	máquina específica - incluyendo la cuenta de sesión curriente y
	límite de sesión. El valor máscara no puede incluír comodínes.
	
	Vea la ayuda de EXCEPTION para más información sobre limitado
	de sesiones y sobre como fijar límites de sesiones específicas a
	ciertas máquinas y grupos de esas máquinas.
	
	Limitado a los Operadores De Servicios.

OPER_HELP_SU
	Sintaxis: SU Contraseña
	
	Permite acceso a funciones que requieren privilegios de
	super-usuario de los servicios.  La contraseña es la misma
	fijada con el comando SET SUPASS; note que la contraseña
	debe ser fijada antes de usar este comando.  Privilegios de
	Super-usuario solamente duran hasta que usted se desconecte
	de IRC.
	
	¡AVISO!: Si usted ingresa una contraseña incorrecta, Entonces
	los servicios enviarán un mensaje de aviso a todos los IRCops.
	
	Limitado a los Administradores De Servicios.

OPER_HELP_SET
	Sintaxis: SET Opción Ajuste
	
	Ajusta varias opciones globales de los Servicios.  Algunas
	de las opciones actualmente definidas son:
	    READONLY   Fija servicios en modo de leer-solamente
	    DEBUG      Activa/desactiva el modo de detección de errores
	    SUPASS     Fija contraseña para el comando SU (super-usuario)
	
	Limitado a los Administradores De Servicios; la opción SUPASS
	solo puede ser fijada por el Super-Usuario De Servicios.

OPER_HELP_SET_READONLY
	Sintaxis: SET READONLY {ON | OFF}
	
	Activa o desactiva el modo de leer-solamente.  En modo de
	leer-solamente, los usuarios regulares no podrán modificar
	las bases de datos de los servicios, incluyendo las listas
	de acceso de canales y apodos, ect. IRCops con suficientes
	privilegios si podrán modificar la lista AKILL asi como
	cancelar o prohibir apodos o canales, pero ninguno de estos
	cambios será grabado al disco a menos que se desactive el modo
	de leer-solamente antes que los servicios sean desactivados o
	re-activados.
	
	Esta opción es equivalente a la opción -readonly en la línea
	de comandos.

OPER_HELP_SET_DEBUG
	Sintaxis: SET DEBUG {ON | OFF | Núm}
	
	Activa/Desactiva el modo de detección de errores.  En modo de
	detección de errores, toda la informacion enviada a o desde los
	servicios asi como un número considerable de mensajes de chequeo
	son escritos en un archivo en el disco, Si se especifica Núm,
	el modo de detección de errores es activado con el nivel de
	detección fijado a Núm.
	
	Esta opción es equivalente a la opción -debug en la línea
	de comandos.

OPER_HELP_SET_SUPASS
	Sintaxis: SET SUPASS [Contraseña]
	
	Fija la contraseña que se usará para el comando SU.  La
	contraseña debe ser fijada una vez antes de usar el comando SU
	por primera vez.  Si no se establece una contraseña, el comando
	SU no estará disponible.
	
	Esta opción solo puede ser fijada por el Super-Usuario de Servicios.

OPER_HELP_JUPE
	Sintaxis: JUPE Servidor [Razón]
	
	Ordena a Servicios que ejecuten el comando jupiter en un servidor
	--eso es, crear un "servidor" falso conectado a los servicios
	que evita que el servidor real con ese nombre se conecte.  Esta
	opción puede ser removida usando un /SQUIT estandar.  Si se da
	una razón, esta es colocada en el area de información del servidor;
	de otra manera, el area de información sobre el servidor contendrá
	el texto "Jupitered by <apodo>", mostrando el apodo de la persona
	que desenganchó (jupitered) al servidor.
	
	Limitado a los Administradores De Servicios.

OPER_HELP_RAW
	Sintaxis: RAW Texto
	
	Envía una secuencia del texto directamente al servidor al cual
	los Servicios están conectados, sin ejecutar prosesamiento alguno
	de dicho texto. Este comando ha sido creado primordialmente con
	el proposito de hacer pruebas entre los Servicios y ciertos servidores
	de IRC; tiene un muy limitado rango de usos en una red viva, y puede
	causar estragos en la red o causar que los servicios caigan de no ser
	utilizado apropiadamente. ¡NO USE ESTE COMANDO a menos que usted
	esté completamente seguro de lo que está haciendo!
	
	Limitado al Super-Usuario De Servicios.

OPER_HELP_UPDATE
	Sintaxis: UPDATE [FORCE]
	
	Causa que los Servicios actualicen las bases de datos tan
	pronto como usted envía el comando.  Si la opción FORCE es
	dada, los servicios procurarán abrir (liberar) las bases de
	datos antes de realizar la actualización; esto puede usarse
	para corregir los errores "databases are locked" cuando se
	actualizan las bases de datos.
	
	Limitado a los Administradores De Servicios.

OPER_HELP_QUIT
	Sintaxis: QUIT
	
	Causa la desactivación de los Servicios; las bases de datos
	no se graban.  Este comando no debe utilizarse a menos que
	se sospeche de un daño en las bases de datos en-memoria y no
	deseen grabarse.  Para desactivación normal de los
	Servicios, utilice el comando SHUTDOWN.
	
	Limitado a los Administradores De Servicios.

OPER_HELP_SHUTDOWN
	Sintaxis: SHUTDOWN
	
	Causa que los Servicios graben todas las bases de datos y despues se desactiven.
	
	Limitado a los Administradores De Servicios.

OPER_HELP_RESTART
	Sintaxis: RESTART
	
	Causa que los servicios graben todas las bases de datos y
	luego se desactiven. (ejemplo: salir e immediatamente
	re-hacer el ejecutable).
	
	Limitado a los Administradores De Servicios.

OPER_HELP_REHASH
	Sintaxis: REHASH
	
	Causa que los Servicios re-lean sus ficheros de configuración y que actualicen
	la configuración activa corriente.  Algunos ajustes, tales como RemoteServer
	(la cual establece la dirección de el servidor de IRC al que servicios se
	conectará), no puede ser cambiado con REHASH, y solo entrará en efecto cuando
	los Servicios sean re-empezados; en estos casos, un mensaje será escrito en
	en el fichero de archivo de los Servicios , y una notificación será enviada
	poniendolo a usted al corriente de esto.
	
	Si lineas de LoadModule han sido agregadas o removidas de el fichero principal
	de configuración de los servicios (%s), los modulos serán bien cargados o
	descargados según sea necesario.  Los modulos que ya no tengan una linea de
	LoadModule serán descargados (desmontados) en orden reversa a aquellos que se
	cargarán; entonces, despues que los modulos que faltan hayan sido actualizados,
	los modulos nuevos serán cargados (montados) en el orden alistado en el fichero
	configuración.  Si ocurriera un error en cualquiera de los pasos, entonces el
	proceso de actualización será abortado (cancelado).
	
	Note que debido a dependencias de los modules, algunos modulos no pueden ser
	removidos o cambiandos via REHASH; en estos cases, usted deberá re-empenzar
	los servicios para que los cambios entren en efecto.
	
	Limitado a los Administradores De Servicios.

OPER_HELP_KILLCLONES
	Sintaxis: KILLCLONES Apodo
	
	Desconecta (Kills) todos los usuarios que tengan la misma
	máscara que Apodo. un KILL temporario, en la forma *@máscara,
	es agregado para prevenir que los clientes ofensivos se reconecten
	immediatamente. un wallops es también enviado indicando quien usó
	el comando, que máscara fue afectada y cuántos usuarios fueron
	desconectados. Esto es muy útil cuando es necesario remover
	numerosos clones de la red.
	
	Limitado a los Operadores De Servicios.


###########################################################################
#
# StatServ help messages
#
###########################################################################

STAT_HELP
	%S provee información estadística sobre la red de IRC,
	sobre sus servidores y también sus usuarios.  Escriba
	/msg %S HELP COMMANDS para ver la lista de comandos de %S;  Para
	información sobre un comando, escriba /msg %S HELP comando.

STAT_HELP_COMMANDS
	Los siguientes comandoss pueden ser utilizados con %S:
	
	    SERVERS     Estadísticas del servidor
	    USERS       Estadísticas de los usuarios

STAT_HELP_SERVERS
	Sintaxis: SERVERS STATS
	          SERVERS LIST [Máscara]
	          SERVERS VIEW [Máscara] [ONLINE | OFFLINE]
	
	Muestra estadísticas relacionadas a los servidores de la red.
	
	SERVERS STATS muestra un sumário breve de los servidores
	actualmente visibles (en línea) e invisibles (no en línea) a
	los Servicios.
	SERVERS LIST muestra una lista de servidores en línea y
	estadísticas corrientes sobre cada uno de ellos.
	SERVERS VIEW muestra una lista de servidores en o fuera de
	línea con estadísticas detalladas sobre cada uno de ellos. las
	opciones ONLINE y OFFLINE causan, respectivamente, que
	solo los servidores en línea o fuera de línea sean mostrados.
	
	Utilice la opción Máscara para limitar el tipo de estadísticas
	que serán mostradas sobre el servidor.

STAT_HELP_USERS
	Sintaxis: USERS STATS
	
	Muestra estadísticas relacionadas a los usuarios de la red.

STAT_OPER_HELP_SERVERS
	Sintaxis: SERVERS STATS
	          SERVERS LIST [Máscara]
	          SERVERS VIEW [Máscara] [ONLINE | OFFLINE]
	          SERVERS DELETE Servidor
	          SERVERS COPY Servidor NuevoNombre
	          SERVERS RENAME Servidor NuevoNombre
	
	Muestra estadísticas relacionadas a los servidores de la red.
	
	SERVERS STATS muestra un sumário breve de los servidores
	actualmente visibles (en línea) e invisibles (no en línea) a
	los Servicios.
	SERVERS LIST muestra una lista de servidores en línea y
	estadísticas corrientes sobre cada uno de ellos.
	SERVERS VIEW muestra una lista de servidores en o fuera de
	línea con estadísticas detalladas sobre cada uno de ellos. las
	opciones ONLINE y OFFLINE causan, respectivamente, que
	solo los servidores en línea o fuera de línea sean mostrados.
	
	Utilice la opción Máscara para limitar las estadísticas de
	qué servidor serán mostradas.
	
	DELETE borra las estadísticas del servidor especificado.
	COPY duplica un grupo de estadísticas del servidor.
	RENAME renombra un grupo de estadísticas del servidor.
	Estos tres comandos están limitados al Super-Usuario De
	Servicios.

###########################################################################

# CVS: $Revision: 2.207 $
