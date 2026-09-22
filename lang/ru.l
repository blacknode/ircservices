# Emacs: -*- mode: fundamental; coding: binary -*-

# Russian language file.
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
# Translated by Alexander Zverev, tty(at)inbox(dot)ru
#

###########################################################################
#
# Name and character set of this language
#
###########################################################################

# For languages other than English, this string should have the following
# format:
#     language-name-in-language (language-name-in-English)
# For example, "Espaсol (Spanish)" or "Franзais (French)".

LANG_NAME
	Русский (Russian)

### CHANGED: LANG_CHARSET added

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
	%d %b %H:%M:%S %Y %Z
# This is used as the format string for strftime() for a date alone in long
# format (English: including weekday).
STRFTIME_LONG_DATE_FORMAT
	%a %d %b %Y
# This is used as the format string for strftime() for a date alone in
# short format (English: without weekday).
STRFTIME_SHORT_DATE_FORMAT
	%d %b %Y

# These tell our strftime() what the names of months and days are.  If you
# don't use %a, %A, %b, or %B in your strftime() strings above, you can
# leave these empty.  However, if you enter names, they MUST stay in order,
# one per line, and the list MUST be complete!

# %a
STRFTIME_DAYS_SHORT
	Вс
	Пн
	Вт
	Ср
	Чт
	Пт
	Сб
# %A
STRFTIME_DAYS_LONG
	Воскресенье
	Понедельник
	Вторник
	Среда
	Четверг
	Пятница
	Суббота
# %b
STRFTIME_MONTHS_SHORT
	Янв
	Фев
	Мар
	Апр
	Май
	Июн
	Июл
	Авг
	Сен
	Окт
	Ноя
	Дек
# %B
STRFTIME_MONTHS_LONG
	Январь
	Февраль
	Март
	Апрель
	Май
	Июнь
	Июль
	Август
	Сентябрь
	Октябрь
	Ноябрь
	Декабрь


# "day", "hour", "minute", and "second" in singular and plural.
# These must be preceded by spaces if the language requires them
# (e.g. in English: "10 hours", not "10hours").
STR_DAY
	 день
STR_DAYS
	 дней
STR_HOUR
	 час
STR_HOURS
	 часов
STR_MINUTE
	 минута
STR_MINUTES
	 минут
STR_SECOND
	 секунда
STR_SECONDS
	 секунд
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
	Внутренняя ошибка -- невозможно выполнить запрос.
SERVICES_IS_BUSY
	Сервисы в настоящее время заняты; пожалуйста, попробуйте еще раз попозже.

UNKNOWN_COMMAND
	Неизвестная команда %s.
UNKNOWN_COMMAND_HELP
	Неизвестная команда %s.  Наберите /msg %s HELP для получения помощи.
SYNTAX_ERROR
	Синтаксис: %s
MORE_INFO
	Наберите /msg %s HELP %s для дополнительной информации.
NO_HELP_AVAILABLE
	Справка по %s недоступна.
MISSING_QUOTE
	Не хватает двойной кавычки (").

BAD_EMAIL
	E-mail адреса должны быть в формате username@hostname.  Вы не можете использовать цвета, жирные выделения, подчеркивания, инверсию или любые из этих символов: , : ; | \ " ( ) < > [ ]
### CHANGED: REJECTED_EMAIL added
BAD_URL
	URLs должны быть в формате http://hostname[:port]/... (or ftp://, и т.д.).
BAD_USERHOST_MASK
	Маска должна быть в формате user@host.
BAD_NICKUSERHOST_MASK
	Маска должна быть в формате nick!user@host.
BAD_EXPIRY_TIME
	Неверное время окончания.

SENDMAIL_NO_RESOURCES
	В настоящий момент система занята, чтобы отослать письмо; пожалуйста, попробуйте еще раз попозже.
READ_ONLY_MODE
	Уведомление: Сервисы в режиме только чтение; изменения НЕ будут сохранены!
PASSWORD_INCORRECT
	Неверный пароль.
PASSWORD_WARNING
	Предупреждение: После еще одного неверного пароля Вы будете отключены.

# What's the difference between "Access denied" and "Permission denied"?
# Very little; you can safely make them the same message with no real loss
# of meaning.  If you want to make a distinction, "Access denied" is
# usually used more generally; for example, a non-oper trying to access
# OperServ gets "Access denied", while a non-Services admin trying to use
# NickServ SET NOEXPIRE gets "Permission denied".
ACCESS_DENIED
	В доступе отказано.
PERMISSION_DENIED
	Нет прав.

MORE_OBSCURE_PASSWORD
	Пожалуйста, попробуйте еще раз с более сложным паролем. Пароли должны иметь длину не менее пяти символов, и не должны быть легкоугадываемыми (т.е. Ваше имя или ник).

NICK_NOT_REGISTERED
	Ваш ник незарегистрирован.
NICK_NOT_REGISTERED_HELP
	Ваш ник незарегистрирован.  Наберите /msg %s HELP для получения информации о том, как зарегистрировать ник.
NICK_TOO_LONG
	Ник должен быть не длиннее %d символов.
NICK_INVALID
	%s неправильный ник.
NICK_X_NOT_REGISTERED
	Ник %s незарегистрирован.
NICK_X_ALREADY_REGISTERED
	Ник %s уже зарегистрирован!
NICK_X_NOT_IN_USE
	Ник %s в данный момент не используется.
NICK_X_FORBIDDEN
	Ник %s не может быть зарегистрирован или использован.
NICK_X_SUSPENDED
	Ник %s приостановлен и не может использоваться и идентифицироваться.
NICK_X_SUSPENDED_MEMOS
	Ник %s приостановлен и не может принимать записки.
NICK_IDENTIFY_REQUIRED
	Для использования этой команды необходима аутентификация.
	Повторите еще раз после набора /msg %s IDENTIFY password.
NICK_PLEASE_AUTH
	Ваш ник еще не аутентифицирован. Пожалуйста, аутентифицируйтесь с кодом аутентификации, который послан на e-mail: %s
NICK_X_NOT_ON_CHAN_X
	Ник %s не находится на канале %s.

CHAN_INVALID
	%s неправильное название канала.
CHAN_X_NOT_REGISTERED
	Канал %s незарегистрирован.
CHAN_X_NOT_IN_USE
	Канал %s не используется.
CHAN_X_FORBIDDEN
	Канал %s не может быть зарегистрирован или использован.
CHAN_X_SUSPENDED
	Канал %s приостановлен и не может использоваться и идентифицироваться.
CHAN_X_SUSPENDED_MEMOS
	Канал %s приостановлен и не может принимать записки.
CHAN_IDENTIFY_REQUIRED
	Для использования этой команды необходима аутентификация.
	Повторите еще раз после набора /msg %s IDENTIFY %s password.

SERV_X_NOT_FOUND
	Сервер %s не найден.

EXPIRES_NONE
	не заканчивается
### CHANGED: EXPIRES_NOW added
EXPIRES_IN
	закончится %s

### CHANGED: LIST_RESULTS added

###########################################################################
#
# NickServ messages
#
###########################################################################

# Automatic messages
NICK_IS_REGISTERED
	Этот ник принадлежит другому. Пожалуйста, выберите другой ник. (Если это Ваш ник, наберите /msg %s IDENTIFY password.)
NICK_IS_SECURE
	Этот ник зарегистрирован и защищен. Если это Ваш ник, наберите /msg %s IDENTIFY password. Иначе выберите другой ник.
NICK_MAY_NOT_BE_USED
	Этот ник не может использоваться. Пожалуйста, выберите другой.
DISCONNECT_IN_1_MINUTE
	Если Вы не поменяете свой ник в течение одной минуты, Вы будете отключены.
DISCONNECT_IN_20_SECONDS
	Если Вы не поменяете свой ник в течение 20-ти секунд, Вы будете отключены.
DISCONNECT_NOW
	Этот ник зарегистрирован, Вы не можете его использовать. Пожалуйста, выберите другой ник прежде чем соединяться снова.
FORCENICKCHANGE_IN_1_MINUTE
	Если Вы не поменяете свой ник в течение одной минуты, он будет изменен автоматически.
FORCENICKCHANGE_IN_20_SECONDS
	Если Вы не поменяете свой ник в течение 20-ти секунд, он будет изменен автоматически.
FORCENICKCHANGE_NOW
	Этот ник зарегистрирован, Вы не можете его использовать. Ваш ник сейчас будет изменен на %s.
NICK_EXPIRES_SOON
	Уведомление: Регистрация Вашего ника закончится %s. Если Вы собираетесь и дальше использовать этот ник, пожалуйста, идентифицируйтесь на %s набрав: /msg %s IDENTIFY password
NICK_EXPIRED
	Уведомление: Регистрация Вашего ника закончилась. Пожалуйста, перерегистрируйте Ваш ник, если Вы хотите его использовать в дальнейшем.
# REGISTER responses
NICK_REGISTER_SYNTAX
	REGISTER пароль [email]
NICK_REGISTER_REQ_EMAIL_SYNTAX
	REGISTER пароль email
NICK_REGISTRATION_DISABLED
	Извините, регистрация ников временно отключена.
NICK_REGISTRATION_FAILED
	Извините, регистрация не удалась.
NICK_REG_PLEASE_WAIT
	Пожалуйста, подождите %s прежде чем использовать команду REGISTER еще раз.
NICK_REG_PLEASE_WAIT_FIRST
	Пожалуйста, подождите %s прежде чем использовать команду REGISTER.
NICK_CANNOT_BE_REGISTERED
	Ник %s не может быть зарегистрирован.
NICK_REGISTER_EMAIL_UNAUTHED
	Указанный Вами E-mail адрес еще не аутентифицирован. Пожалуйста, используйте команду AUTH для аутентификация этого адреса, прежде чем регистрировать новые ники с ним.
NICK_REGISTER_TOO_MANY_NICKS
	Вы зарегистрировали слишком много ников (%d, максимум %d). Вы не можете больше регистрировать, пока не разрегистрируете некоторые из имеющихся ников.
NICK_REGISTERED
	Ник %s зарегистрирован на Вас.
NICK_PASSWORD_IS
	Ваш пароль %s -- запомните его для дальнейшего использования.

# IDENTIFY responses
NICK_IDENTIFY_SYNTAX
	IDENTIFY пароль
NICK_IDENTIFY_FAILED
	Извините, идентификация не удалась.
NICK_IDENTIFY_EMAIL_MISSING
	Вы должны установить E-mail адрес для Вашего ника перед тем, как его использовать. Пожалуйста, установите e-mail командой: /msg %s SET EMAIL address
NICK_IDENTIFY_SUCCEEDED
	Пароль принят - Вы опознаны.

# DROP responses
NICK_DROP_SYNTAX
	DROP пароль
NICK_DROP_WARNING
	Уведомление: Команда DROP разрегистрирует все Ваши связанные ники!
NICK_DROP_DISABLED
	Извините, разрегистрация ников временно отключена.
NICK_DROP_FAILED
	Извините, разрегистрация не удалась.
NICK_DROPPED
	Ваш ник был разрегистрирован.
NICK_DROPPED_LINKS
	Ваш ник и все связанные ники были разрегистрированы.

# DROPNICK responses
NICK_DROPNICK_SYNTAX
	DROPNICK ник
NICK_X_DROPPED
	Ник %s был разрегистрирован.
NICK_X_DROPPED_LINKS
	Ник %s и все связанные ники были разрегистрированы.

# DROPEMAIL and DROPEMAIL-CONFIRM responses
NICK_DROPEMAIL_SYNTAX
	DROPEMAIL шаблон
NICK_DROPEMAIL_PATTERN_TOO_LONG
	Приведенный шаблон слишком длинный (максимум %d символов).
NICK_DROPEMAIL_NONE
	Не найдено ни одного ника с E-mail адресами, совпадающими с указанным шаблоном.
NICK_DROPEMAIL_COUNT
	Найдено %d ников с E-mail адресами, совпадающими с указанным шаблоном.
	Чтобы разрегистрировать эти ники, наберите /msg %s DROPEMAIL-CONFIRM %s
NICK_DROPEMAIL_CONFIRM_SYNTAX
	DROPEMAIL-CONFIRM адрес
NICK_DROPEMAIL_CONFIRM_UNKNOWN
	Указанный шаблон неопознан. Пожалуйста, используйте команду DROPEMAIL еще раз с этим шаблоном перед использованием команды DROPEMAIL-CONFIRM.
NICK_DROPEMAIL_CONFIRM_DROPPING
	Разрегистрация %d ников...
NICK_DROPEMAIL_CONFIRM_DROPPED
	Разрегистрация ника закончена.

# SET responses
NICK_SET_SYNTAX
	SET опция параметры
NICK_SET_OPER_SYNTAX
	SET [!ник] опция параметры
NICK_SET_DISABLED
	Извините, установка опций ника временно отключена.
NICK_SET_UNKNOWN_OPTION
	Неизвестная SET опция %s.

# SET PASSWORD responses
NICK_SET_PASSWORD_FAILED
	Извините, невозможно изменить пароль.
NICK_SET_PASSWORD_CHANGED
	Пароль изменен.
NICK_SET_PASSWORD_CHANGED_TO
	Пароль изменен на %s.

# SET LANGUAGE responses
NICK_SET_LANGUAGE_SYNTAX
	SET LANGUAGE номер
NICK_SET_LANGUAGE_UNKNOWN
	Неизвестный номер языка %d.  Наберите /msg %s HELP SET LANGUAGE для получения списка языков.
NICK_SET_LANGUAGE_CHANGED
	Язык изменен на %s.

# SET URL responses
NICK_SET_URL_CHANGED
	URL для %s изменен на %s.

# SET EMAIL responses
### CHANGED: NICK_SET_EMAIL_PLEASE_WAIT added
NICK_SET_EMAIL_UNAUTHED
	Указанный Вами E-mail адрес еще не аутентифицирован. Пожалуйста, сначала используйте команду AUTH для аутентификация этого адреса.
NICK_SET_EMAIL_TOO_MANY_NICKS
	%s уже имеет %d зарегистрированных ников (максимум %d).  Изменение E-mail адреса не удалась.
NICK_SET_EMAIL_CHANGED
	E-mail адрес для %s изменен на %s.

# SET INFO responses
NICK_SET_INFO_CHANGED
	Информация для %s изменена.

# SET KILL responses
NICK_SET_KILL_SYNTAX
	SET KILL {ON | QUICK | OFF}
NICK_SET_KILL_IMMED_SYNTAX
	SET KILL {ON | QUICK | IMMED | OFF}
NICK_SET_KILL_ON
	Автоматическая защита ВКЛЮЧЕНА.
NICK_SET_KILL_QUICK
	Автоматическая защита ВКЛЮЧЕНА с уменьшенной задержкой.
NICK_SET_KILL_IMMED
	Автоматическая защита ВКЛЮЧЕНА, без задержки.
NICK_SET_KILL_IMMED_DISABLED
	Параметр IMMED недоступен в этой сети.
NICK_SET_KILL_OFF
	Автоматическая защита ВЫКЛЮЧЕНА.

# SET SECURE responses
NICK_SET_SECURE_SYNTAX
	SET SECURE {ON | OFF}
NICK_SET_SECURE_ON
	Охрана ВКЛЮЧЕНА.
NICK_SET_SECURE_OFF
	Охрана ВЫКЛЮЧЕНА.

# SET PRIVATE responses
NICK_SET_PRIVATE_SYNTAX
	SET PRIVATE {ON | OFF}
NICK_SET_PRIVATE_ON
	Секретность ВКЛЮЧЕНА.
NICK_SET_PRIVATE_OFF
	Секретность ВЫКЛЮЧЕНА.

# SET NOOP responses
NICK_SET_NOOP_SYNTAX
	SET NOOP {ON | OFF}

# SET HIDE responses
NICK_SET_HIDE_SYNTAX
	SET HIDE {EMAIL | USERMASK | QUIT} {ON | OFF}
NICK_SET_HIDE_EMAIL_ON
	Ваш E-mail адрес теперь скрыт от показа в %s INFO.
NICK_SET_HIDE_EMAIL_OFF
	Ваш E-mail адрес теперь виден в %s INFO.
NICK_SET_HIDE_MASK_ON
	Ваша последняя маска user@host теперь скрыта от показа в %s INFO.
NICK_SET_HIDE_MASK_OFF
	Ваша последняя маска user@host теперь видна в %s INFO.
NICK_SET_HIDE_QUIT_ON
	Ваше последнее сообщение в /QUIT теперь скрыто от показа в %s INFO.
NICK_SET_HIDE_QUIT_OFF
	Ваше последнее сообщение в /QUIT теперь видно в %s INFO.

# SET TIMEZONE responses
NICK_SET_TIMEZONE_SYNTAX
	SET TIMEZONE {UTC-offset | time-zone | DEFAULT}
NICK_SET_TIMEZONE_TO
	Ваша временная зона установлена в UTC%c%d:%02d.
	Текущее время в это зоне %s.
NICK_SET_TIMEZONE_DEFAULT
	Ваша временная зона установлена такой-же, как и Сервисов.

# SET MAINNICK responses
NICK_SET_MAINNICK_NOT_FOUND
	Ник %s не связан в Вашим текущим ником.
NICK_SET_MAINNICK_CHANGED
	Ваш основной ник изменен на %s.

# SET NOEXPIRE responses
NICK_SET_NOEXPIRE_SYNTAX
	SET [!ник] NOEXPIRE {ON | OFF}
NICK_SET_NOEXPIRE_ON
	Регистрация ника %s не будет заканчиваться.
NICK_SET_NOEXPIRE_OFF
	Регистрация ника %s будет заканчиваться.

# UNSET responses
NICK_UNSET_SYNTAX
	UNSET {URL | EMAIL | INFO}
NICK_UNSET_SYNTAX_REQ_EMAIL
	UNSET {URL | INFO}
### CHANGED: NICK_UNSET_OPER_SYNTAX, NICK_UNSET_OPER_SYNTAX_REQ_EMAIL added
NICK_UNSET_URL
	URL для ника %s удален.
NICK_UNSET_EMAIL
	E-mail адрес для ника %s удален.
NICK_UNSET_INFO
	Информация для ника %s удалена.
NICK_UNSET_EMAIL_BAD
	Вы не можете удалить Ваш E-mail адрес.
NICK_UNSET_EMAIL_OTHER_BAD
	E-mail адреса ника не могут быть удалены.

# ACCESS responses
NICK_ACCESS_SYNTAX
	ACCESS {ADD | DEL | LIST} [маска]
NICK_ACCESS_DISABLED
	Извините, изменения списка доступов ника временно отключены.
NICK_ACCESS_ALREADY_PRESENT
	Маска %s уже имеется в Вашем списке доступа.
NICK_ACCESS_REACHED_LIMIT
	Извините, Вы можете иметь только %d записей доступа.
NICK_ACCESS_NO_NICKS
	Предупреждение: Маска списка доступа не должна включать ники.  Пожалуйста проверьте и убедитесь, что Вы ввели user@host маску правильно.
NICK_ACCESS_ADDED
	%s добавлен в Ваш список доступа.
NICK_ACCESS_NOT_FOUND
	%s не найдем в Вашем списке доступа.
NICK_ACCESS_DELETED
	%s удален из Вашего списка доступа.
NICK_ACCESS_LIST_EMPTY
	Ваш список доступа пуст.
NICK_ACCESS_LIST
	Список доступа:
NICK_ACCESS_LIST_X_EMPTY
	Список доступа %s пуст.
NICK_ACCESS_LIST_X
	Список доступа %s:

# LINK responses
NICK_OLD_LINK_SYNTAX
	LINK ник пароль
NICK_LINK_SYNTAX
	LINK ник
NICK_LINK_DISABLED
	Извините, связывание ников временно отключено.
NICK_LINK_FAILED
	Извините, связывание не удалось.
NICK_CANNOT_BE_LINKED
	Ник %s не может быть связан.
NICK_OLD_LINK_SAME
	Вы не можете связывать к Вашему текущему нику!
NICK_LINK_SAME
	Вы не можете связать Ваш текущий ник!
NICK_LINK_ALREADY_LINKED
	Ник %s уже связан с Вашим ником.
NICK_LINK_IN_USE
	Ник %s сейчас используется.
NICK_LINK_TOO_MANY
	Вы не можете связать более чем %d ников.
NICK_LINK_TOO_MANY_NICKS
	Вы умеете %d зарегистрированных ников (максимум %d).  Связывание не удалось.
NICK_OLD_LINK_TOO_MANY_CHANNELS
	Не могу связать с ником %s: слишком много зарегистрированных каналов (суммарный максимум %d).
NICK_OLD_LINKED
	Ваш ник был связан с ником %s.
NICK_LINKED
	Ник %s связан с Вашим ником.

# UNLINK responses
NICK_OLD_UNLINK_SYNTAX
	UNLINK [ник пароль]
NICK_UNLINK_SYNTAX
	UNLINK ник
NICK_UNLINK_OPER_SYNTAX
	UNLINK ник [FORCE]
NICK_UNLINK_DISABLED
	Извините, удаление связей временно отключено.
NICK_UNLINK_FAILED
	Извините, удаление связей не удались.
NICK_UNLINK_SAME
	Вы не можете удалить связь Вашего текущего ника!
NICK_OLD_UNLINK_NOT_LINKED
	Ваш ник не связан.
NICK_UNLINK_NOT_LINKED_YOURS
	Ник %s не связан с Вашим ником.
NICK_UNLINK_NOT_LINKED
	Ник %s не связан.
NICK_OLD_UNLINKED
	Удалена связь Вашего ника с ником %s.
NICK_UNLINKED
	Удалена связь ника %s с Вашим.
NICK_X_UNLINKED
	Удалена связь ника %s с ником %s.

# LISTLINKS responses
NICK_OLD_LISTLINKS_SYNTAX
	LISTLINKS ник
NICK_LISTLINKS_SYNTAX
	LISTLINKS
NICK_LISTLINKS_HEADER
	Список ников, связанных с %s:
NICK_LISTLINKS_FOOTER
	%d связанных ников.

# INFO responses
NICK_INFO_SYNTAX
	INFO ник [ALL]
NICK_INFO_REALNAME
	%s это %s
NICK_INFO_ADDRESS
	Последний адрес: %s
NICK_INFO_ADDRESS_ONLINE
	    Подключен с: %s
NICK_INFO_ADDRESS_ONLINE_NOHOST
	%s сейчас подключен.
### CHANGED: NICK_INFO_ADDRESS_OTHER_NICK added
NICK_INFO_TIME_REGGED
	Зарегистрирован: %s
NICK_INFO_LAST_SEEN
	Последний визит: %s
NICK_INFO_LAST_QUIT
	Последний /QUIT: %s
NICK_INFO_URL
	            URL: %s
NICK_INFO_EMAIL
	   E-mail адрес: %s
NICK_INFO_EMAIL_UNAUTHED
	   E-mail адрес: %s (непроверенный)
NICK_INFO_INFO
	     Информация: %s
NICK_INFO_OPTIONS
	          Опции: %s
# These strings MUST NOT be empty
NICK_INFO_OPT_KILL
	Защита
NICK_INFO_OPT_SECURE
	Охрана
NICK_INFO_OPT_PRIVATE
	Секретность
NICK_INFO_OPT_NONE
	Нет
NICK_INFO_NO_EXPIRE
	Регистрация ника не заканчивается.
NICK_INFO_SUSPEND_DETAILS
	Приостановлен %s в %s (%s).
NICK_INFO_SUSPEND_REASON
	        Причина: %s
NICK_INFO_SHOW_ALL
	Для дополнительной информации наберите: /msg %s INFO %s ALL

# LISTCHANS responses
NICK_LISTCHANS_SYNTAX
	LISTCHANS
NICK_LISTCHANS_NONE
	На ник %s не зарегистрировано ни одного канала.
NICK_LISTCHANS_HEADER
	Канал, зарегистрированный на %s:
NICK_LISTCHANS_END
	Конец списка (всего %d зарегистрированных каналов).

# LIST/LISTEMAIL responses
### CHANGED
#NICK_LIST_SYNTAX
#	%s шаблон
#NICK_LIST_OPER_SYNTAX
#	%s шаблон [FORBIDDEN] [NOEXPIRE] [SUSPENDED]
#NICK_LIST_OPER_SYNTAX_AUTH
#	%s шаблон [FORBIDDEN] [NOEXPIRE] [SUSPENDED] [NOAUTH]
NICK_LIST_HEADER
	Список совпадений с %s:
### CHANGED: NICK_LIST_NO_MATCH added
NICK_LISTEMAIL_NONE
	(не указано)

# RECOVER responses
NICK_RECOVER_SYNTAX
	RECOVER ник [пароль]
NICK_NO_RECOVER_SELF
	Вы не можете вернуть используемый Вами в данный момент ник!
NICK_RECOVERED
	Пользователь, притязающий на Ваш ник был отключен.  Наберите /msg %s RELEASE %s пароль для получения ника обратно в течении одной минуты.

# RELEASE responses
NICK_RELEASE_SYNTAX
	RELEASE ник [пароль]
NICK_RELEASE_NOT_HELD
	Ник %s не удерживается.
NICK_RELEASED
	Сервисы более не удерживают Ваш ник.

# GHOST responses
NICK_GHOST_SYNTAX
	GHOST ник [пароль]
NICK_NO_GHOST_SELF
	Вы не можете удалить сеанс для собственного ника!
NICK_GHOST_KILLED
	Сеанс Вашего ника был отключен.

# GETPASS responses
NICK_GETPASS_SYNTAX
	GETPASS ник
### CHANGED
#NICK_GETPASS_UNAVAILABLE
#	Команда GETPASS не может быть использована, т.к. используется шифрование.
NICK_GETPASS_PASSWORD_IS
	Пароль для %s: %s.

# FORBID responses
NICK_FORBID_SYNTAX
	FORBID ник
NICK_FORBID_SUCCEEDED
	Ник %s теперь запрещен к использованию.
NICK_FORBID_FAILED
	Не могу запретить ник %s!

# SUSPEND responses
NICK_SUSPEND_SYNTAX
	SUSPEND [+время_завершение] ник причина
NICK_SUSPEND_SUCCEEDED
	Использование ника %s и всех связанных с ним ников приостановлено.
NICK_SUSPEND_ALREADY_SUSPENDED
	Ник %s уже приостановлен.

# UNSUSPEND responses
NICK_UNSUSPEND_SYNTAX
	UNSUSPEND ник
NICK_UNSUSPEND_SUCCEEDED
	Использование ника %s возобновлено.
NICK_UNSUSPEND_NOT_SUSPENDED
	Ник %s не был приостановлен.

################ autojoin module messages/responses

# AJOIN responses
NICK_AJOIN_SYNTAX
	AJOIN {ADD | DEL | LIST} [канал]
NICK_AJOIN_ADD_SYNTAX
	AJOIN ADD канал
NICK_AJOIN_DEL_SYNTAX
	AJOIN DEL канал
NICK_AJOIN_DISABLED
	Извините, изменения списка автоматического захода временно отключено.
NICK_AJOIN_ALREADY_PRESENT
	Канал %s уже в Вашем списке автоматического захода.
NICK_AJOIN_LIST_FULL
	Извините, Вы можете иметь %d позиций в списке автоматического захода.
NICK_AJOIN_ADDED
	%s добавлен в Ваш список автоматического захода.
NICK_AJOIN_NOT_FOUND
	%s не найден в Вашем списке автоматического захода.
NICK_AJOIN_DELETED
	%s удален из Вашего списка автоматического захода.
NICK_AJOIN_LIST_EMPTY
	Ваш список автоматического захода пуст.
NICK_AJOIN_LIST
	Список автоматического захода:
NICK_AJOIN_LIST_X_EMPTY
	Список автоматического захода %s пуст.
NICK_AJOIN_LIST_X
	Список автоматического захода %s:

################ mail-auth module messages/responses

# General-purpose messages
### CHANGED: NICK_AUTH_SENDING added
NICK_AUTH_FOR_REGISTER
	Когда Вы получите это сообщение, наберите /msg %s AUTH code (замените code кодом аутентификации в сообщении) для завершения регистрации Вашего ника.
NICK_AUTH_FOR_SET_EMAIL
	Когда Вы получите это сообщение, наберите /msg %s AUTH code (замените code кодом аутентификации в сообщении) для завершения смены Вашего E-mail адреса.
	Учтите, что Вы не можете продолжать использовать Ваш ник до тех пор, пока Вы не закончите процедуру изменения.
NICK_AUTH_NOT_NEEDED
	Нет ничего, что требовало бы аутентификации.
NICK_AUTH_NO_AUTHCODE
	Ник %s не имеет кода аутентификации.
NICK_AUTH_HAS_AUTHCODE
	Ник %s уже имеет код аутентификации.
PASSWORD_WARNING_FOR_AUTH
	Предупреждение: После еще одного неправильного кода аутентификации Вы будете отключены.

# Mail text.  The last "%s" (before the user@host) in the body text is
# replaced by one of the NICK_AUTH_MAIL_TEXT_* messages.
NICK_AUTH_MAIL_SUBJECT
	Код аутентификации для %s
NICK_AUTH_MAIL_BODY
	Код аутентификации для ника (%s): %09d
	Пожалуйста, предоставьте код %s используя команду:
	    /msg %s AUTH %09d
	
	Это сообщение было послано %s в ответ на %s
	%s@%s.
NICK_AUTH_MAIL_TEXT_REG
	регистрацию от
NICK_AUTH_MAIL_TEXT_EMAIL
	смену E-mail адреса от
NICK_AUTH_MAIL_TEXT_SENDAUTH
	команду SENDAUTH от
### CHANGED: NICK_AUTH_MAIL_TEXT_REAUTH added
NICK_AUTH_MAIL_BODY_SETAUTH
	Код аутентификации для Вашего ника (%s): %09d
	Пожалуйста, предоставьте код %s используя команду:
	    /msg %s AUTH %09d

# AUTH responses
NICK_AUTH_SYNTAX
	AUTH код
NICK_AUTH_DISABLED
	Извините, аутентификация ников временно отключена.
NICK_AUTH_FAILED
	Неверный код аутентификации.
NICK_AUTH_SUCCEEDED_REGISTER
	Аутентификация прошла успешно; регистрация Вашего ника закончена.
NICK_AUTH_SUCCEEDED_SET_EMAIL
	Аутентификация прошла успешно; смена Вашего E-mail адреса закончена.
### CHANGED: NICK_AUTH_SUCCEEDED_REAUTH added
NICK_AUTH_SUCCEEDED_SETAUTH
	Аутентификация прошла успешно; Вы можете дальше пользоваться Вашим ником.

# SENDAUTH responses
NICK_SENDAUTH_SYNTAX
	SENDAUTH
NICK_SENDAUTH_TOO_SOON
	Пожалуйста, подождите %s прежде чем использовать команду SENDAUTH еще раз.
### CHANGED: NICK_SENDAUTH_NORESOURCES, NICK_SENDAUTH_ERROR added

# REAUTH responses
### CHANGED: REAUTH command added

# RESTOREMAIL responses
### CHANGED: RESTOREMAIL command added

# SETAUTH responses
NICK_SETAUTH_SYNTAX
	SETAUTH ник
NICK_SETAUTH_NO_EMAIL
	Не установлен E-mail адрес для ника %s.
NICK_SETAUTH_AUTHCODE_SET
	Новый код аутентификации (%09d) выслан для ника %s.
### CHANGED: several messages added, NICK_SETAUTH_USER_NOTICE changed
#NICK_SETAUTH_USER_NOTICE
#	Вы должны аутентифицировать Ваш ник прежде чем продолжать его использовать.  Код аутентификации был выслан Вам на E-mail адрес: %s
#	Наберите /msg %s HELP AUTH для дополнительной информации.

# GETAUTH responses
NICK_GETAUTH_SYNTAX
	GETAUTH ник
NICK_GETAUTH_AUTHCODE_IS
	Код аутентификации для ника %s: %09d

# CLEARAUTH responses
NICK_CLEARAUTH_SYNTAX
	CLEARAUTH ник
NICK_CLEARAUTH_CLEARED
	Код аутентификации для ника %s удален.

###########################################################################
#
# ChanServ messages
#
###########################################################################

# Access level descriptions
CHAN_LEVEL_AUTOOP
	Статус автоопа
CHAN_LEVEL_AUTOVOICE
	Статус автовойса (+v)
CHAN_LEVEL_INVITE
	Разрешено использовать команду INVITE
CHAN_LEVEL_AKICK
	Разрешено использовать команду AKICK
CHAN_LEVEL_SET
	Разрешено использовать команду SET (но не FOUNDER/PASSWORD)
CHAN_LEVEL_CLEAR
	Разрешено использовать команду CLEAR
CHAN_LEVEL_UNBAN
	Разрешено использовать команду UNBAN
CHAN_LEVEL_OPDEOP
	Разрешено использовать команды OP/DEOP
CHAN_LEVEL_ACCESS_LIST
	Разрешено просматривать список доступа
CHAN_LEVEL_ACCESS_CHANGE
	Разрешено изменять список доступа
### CHANGED
#CHAN_LEVEL_MEMO
#	Разрешено смотреть/читать записки канала
CHAN_LEVEL_VOICE
	Разрешено использовать команды VOICE/DEVOICE
CHAN_LEVEL_AUTOHALFOP
	Статус автополуопа (+h)
CHAN_LEVEL_HALFOP
	Разрешено использовать команды HALFOP/DEHALFOP
CHAN_LEVEL_AUTOPROTECT
	Статус автоматической защиты (+a)
CHAN_LEVEL_PROTECT
	Разрешено использовать команды PROTECT/DEPROTECT
CHAN_LEVEL_KICK
	Разрешено использовать команду KICK
CHAN_LEVEL_STATUS
	Разрешено использовать команду STATUS
CHAN_LEVEL_TOPIC
	Разрешено использовать команду TOPIC

# Automatic responses
CHAN_IS_REGISTERED
	Этот канал зарегистрирован %s
CHAN_MAY_NOT_BE_USED
	Этот канал не может использоваться.
CHAN_NOT_ALLOWED_TO_JOIN
	У Вас нет прав находиться на этом канале.

# Multi-purpose responses
CHAN_MUST_IDENTIFY_NICK
	Пожалуйста, сперва идентифицируйтесь на %s, используя команду:
	/msg %s IDENTIFY пароль
CHAN_BOUNCY_MODES
	Извините, но команда %s временно недоступна.

# REGISTER responses
CHAN_REGISTER_SYNTAX
	REGISTER канал пароль описание
CHAN_REGISTER_DISABLED
	Извините, регистрация каналов временно отключена.
CHAN_REGISTER_SHORT_CHANNEL
	Канал # не может быть зарегистрирован.
CHAN_REGISTER_NOT_LOCAL
	Локальные каналы не могут быть зарегистрированы.
CHAN_REGISTER_INVALID_NAME
	Вы можете зарегистрировать только те каналы, которые начинаются с #.
CHAN_MUST_REGISTER_NICK
	Вы должны сперва зарегистрировать Ваш ник. Наберите /msg %s HELP для получения информации по регистрации ников.
CHAN_MAY_NOT_BE_REGISTERED
	Канал %s не может быть зарегистрирован.
CHAN_ALREADY_REGISTERED
	Канал %s уже зарегистрирован!
CHAN_MUST_BE_CHANOP
	Вы должны быть оператором канала, чтобы зарегистрировать его.
CHAN_REACHED_CHANNEL_LIMIT
	Извините, но Вы достигли лимита в %d каналов.
CHAN_EXCEEDED_CHANNEL_LIMIT
	Извините, но Вы превысили лимит в %d каналов.
CHAN_REGISTRATION_FAILED
	Извините, регистрация не удалась.
CHAN_REGISTERED
	Канал %s зарегистрирован на Ваш ник: %s
CHAN_PASSWORD_IS
	Ваш пароль на канал %s -- запомните его для дальнейшего использования.

# IDENTIFY responses
CHAN_IDENTIFY_SYNTAX
	IDENTIFY канал пароль
CHAN_IDENTIFY_FAILED
	Извините, идентификация не удалась.
CHAN_IDENTIFY_SUCCEEDED
	Пароль принят - Вы имеете права владельца на канал %s.

# DROP responses
### CHANGED
#CHAN_DROP_SYNTAX
#	DROP канал
CHAN_DROP_DISABLED
	Извините, разрегистрация каналов временно отключена.
CHAN_DROPPED
	Канал %s разрегистрирован.

# DROPCHAN responses
### CHANGED: DROPCHAN command added

# SET responses
CHAN_SET_SYNTAX
	SET канал опция параметры
CHAN_SET_DISABLED
	Извините, установка опций канала временно отключена.
CHAN_SET_UNKNOWN_OPTION
	Неизвестная SET опция %s.

# SET FOUNDER responses
CHAN_SET_FOUNDER_TOO_MANY_CHANS
	%s имеет слишком много зарегистрированных каналов.
CHAN_FOUNDER_CHANGED
	Владелец канала %s изменен на %s.

# SET SUCCESSOR responses
CHAN_SUCCESSOR_CHANGED
	Преемник канала %s изменен на %s.
CHAN_SUCCESSOR_IS_FOUNDER
	Вы не можете сделать владельца канале его преемником.

# SET PASSWORD responses
CHAN_SET_PASSWORD_FAILED
	Извините, невозможно изменить пароль.
CHAN_PASSWORD_CHANGED
	%s пароль изменен.
CHAN_PASSWORD_CHANGED_TO
	%s пароль изменен на %s.

# SET DESC responses
CHAN_DESC_CHANGED
	Описание канала %s изменено на %s.

# SET URL responses
CHAN_URL_CHANGED
	URL канала %s изменен на %s.

# SET EMAIL responses
CHAN_EMAIL_CHANGED
	E-mail адрес канала %s изменен на %s.

# SET ENTRYMSG responses
CHAN_ENTRY_MSG_CHANGED
	Входное сообщение для канала %s изменено.

# SET MLOCK responses
CHAN_SET_MLOCK_NEED_PLUS_MINUS
	Список режимов должен начинаться со знака + (плюс) или - (минус).
CHAN_SET_MLOCK_NEED_PARAM
	Необходимы параметры для команды MLOCK +%c.
CHAN_SET_MLOCK_NEED_POSITIVE
	Параметр для команды MLOCK +%c должен быть положительным числом.
CHAN_SET_MLOCK_MODE_REG_BAD
	Режим +%c всегда устанавливается на зарегистрированные канала и не может быть установлен командой MLOCK.
CHAN_SET_MLOCK_LINK_BAD
	Параметр для команды MLOCK +%c не является правильным именем канала.
CHAN_SET_MLOCK_LINK_SAME
	Параметр для команды MLOCK +%c должен быть другим каналом.
CHAN_SET_MLOCK_REQUIRES
	Команда MLOCK +%c также требует, чтобы была установлена команда MLOCK +%c.
CHAN_SET_MLOCK_BAD_PARAM
	Неправильный параметр для команды MLOCK +%c.
CHAN_SET_MLOCK_UNKNOWN_CHAR
	Неизвестный символ режима %c, игнорировано.
CHAN_SET_MLOCK_CANNOT_LOCK
	Режим %c не может быть заблокирован.
CHAN_MLOCK_CHANGED
	Блокировка режимов на канале %s изменена на %s.
CHAN_MLOCK_REMOVED
	Блокировка режимов на канале %s удалена.

# SET HIDE responses
CHAN_SET_HIDE_SYNTAX
	SET канал HIDE {EMAIL | TOPIC | MLOCK} {ON | OFF}
CHAN_SET_HIDE_EMAIL_ON
	E-mail адрес канала %s теперь скрыт от показа в %s INFO.
CHAN_SET_HIDE_EMAIL_OFF
	E-mail адрес канала %s теперь виден в %s INFO.
CHAN_SET_HIDE_TOPIC_ON
	Топик канала %s теперь скрыт от показа в %s INFO.
CHAN_SET_HIDE_TOPIC_OFF
	Топик канала %s теперь виден в %s INFO.
CHAN_SET_HIDE_MLOCK_ON
	Закрепленные режимы канала %s теперь скрыты от показа в %s INFO.
CHAN_SET_HIDE_MLOCK_OFF
	Закрепленные режимы канала %s теперь видны в %s INFO.

# SET KEEPTOPIC responses
CHAN_SET_KEEPTOPIC_SYNTAX
	SET канал KEEPTOPIC {ON | OFF}
CHAN_SET_KEEPTOPIC_ON
	Восстановление топика ВКЛЮЧЕНО.
CHAN_SET_KEEPTOPIC_OFF
	Восстановление топика ВЫКЛЮЧЕНО.

# SET TOPICLOCK responses
CHAN_SET_TOPICLOCK_SYNTAX
	SET канал TOPICLOCK {ON | OFF}
CHAN_SET_TOPICLOCK_ON
	Блокировка топика ВКЛЮЧЕНА.
CHAN_SET_TOPICLOCK_OFF
	Блокировка топика ВЫКЛЮЧЕНА.

# SET PRIVATE responses
CHAN_SET_PRIVATE_SYNTAX
	SET канал PRIVATE {ON | OFF}
CHAN_SET_PRIVATE_ON
	Секретность ВКЛЮЧЕНА.
CHAN_SET_PRIVATE_OFF
	Секретность ВЫКЛЮЧЕНА.

# SET SECUREOPS responses
CHAN_SET_SECUREOPS_SYNTAX
	SET канал SECUREOPS {ON | OFF}
CHAN_SET_SECUREOPS_ON
	Охрана статуса оператора ВКЛЮЧЕНА.
CHAN_SET_SECUREOPS_OFF
	Охрана статуса оператора ВЫКЛЮЧЕНА.

# SET LEAVEOPS responses
CHAN_SET_LEAVEOPS_SYNTAX
	SET канал LEAVEOPS {ON | OFF}
CHAN_SET_LEAVEOPS_ON
	Разрешение статуса оператора непривилегированным лицам ВКЛЮЧЕНО.
CHAN_SET_LEAVEOPS_OFF
	Разрешение статуса оператора непривилегированным лицам ВЫКЛЮЧЕНО.

# SET RESTRICTED responses
CHAN_SET_RESTRICTED_SYNTAX
	SET канал RESTRICTED {ON | OFF}
CHAN_SET_RESTRICTED_ON
	Ограничение доступа ВКЛЮЧЕНО.
CHAN_SET_RESTRICTED_OFF
	Ограничение доступа ВЫКЛЮЧЕНО.

# SET SECURE responses
CHAN_SET_SECURE_SYNTAX
	SET канал SECURE {ON | OFF}
CHAN_SET_SECURE_ON
	Охрана ВКЛЮЧЕНА.
CHAN_SET_SECURE_OFF
	Охрана ВЫКЛЮЧЕНА.

# SET OPNOTICE responses
CHAN_SET_OPNOTICE_SYNTAX
	SET канал OPNOTICE {ON | OFF}
CHAN_SET_OPNOTICE_ON
	Уведомления о OP/DEOP ВКЛЮЧЕНЫ.
CHAN_SET_OPNOTICE_OFF
	Уведомления о OP/DEOP ВЫКЛЮЧЕНЫ.

# SET ENFORCE responses
CHAN_SET_ENFORCE_SYNTAX
	SET канал ENFORCE {ON | OFF}
CHAN_SET_ENFORCE_ON
	Приведение в силу ВКЛЮЧЕНО.
CHAN_SET_ENFORCE_OFF
	Приведение в силу ВЫКЛЮЧЕНО.

# SET MEMO-RESTRICTED responses
CHAN_SET_MEMO_RESTRICTED_SYNTAX
	SET канал MEMO-RESTRICTED {ON | OFF}

# SET NOEXPIRE responses
CHAN_SET_NOEXPIRE_SYNTAX
	SET канал NOEXPIRE {ON | OFF}
CHAN_SET_NOEXPIRE_ON
	Регистрация канала %s не будет заканчиваться.
CHAN_SET_NOEXPIRE_OFF
	Регистрация канала %s будет заканчиваться.

# UNSET responses
CHAN_UNSET_SYNTAX
	UNSET канал {SUCCESSOR | URL | EMAIL | ENTRYMSG}
CHAN_SUCCESSOR_UNSET
	Преемник для канала %s удален.
CHAN_URL_UNSET
	URL для канала %s удален.
CHAN_EMAIL_UNSET
	E-mail адрес для канала %s удален.
CHAN_ENTRY_MSG_UNSET
	Входное сообщение для канала %s удалено.

# ACCESS messages
### CHANGED: syntax changed, several syntax messages added
#CHAN_ACCESS_SYNTAX
#	ACCESS канал {ADD | DEL | LIST | COUNT} [ник [уровень] | элемент списка]
CHAN_ACCESS_DISABLED
	Извините, изменение списка доступов канала временно отключено.
CHAN_ACCESS_LEVEL_NONZERO
	Уровень доступа должен быть ненулевым значением.
CHAN_ACCESS_LEVEL_RANGE
	Уровень доступа должен быт между %d и %d включительно.
CHAN_ACCESS_NICKS_ONLY
	Список доступа канала может содержать только зарегистрированные ники.
CHAN_ACCESS_REACHED_LIMIT
	Извините, Вы можете иметь только %d записей доступа на канале.
CHAN_ACCESS_LEVEL_UNCHANGED
	Уровень доступа %s на %s остался неизменным от %d.
CHAN_ACCESS_LEVEL_CHANGED
	Уровень доступа %s на %s изменен на %d.
CHAN_ACCESS_ADDED
	%s добавлен(а) в список доступа %s с уровнем %d.
CHAN_ACCESS_NOT_FOUND
	%s не найден в списке доступа %s.
CHAN_ACCESS_DELETED
	%s удален(а) из списка доступа %s.
CHAN_ACCESS_LIST_EMPTY
	Список доступа %s пуст.
### CHANGED: CHAN_ACCESS_NO_MATCH added, list format changed
#CHAN_ACCESS_LIST_HEADER
#	Список доступа %s:
#	  Номер   Уровень  Ник
#CHAN_ACCESS_LIST_FORMAT
#	 %4d  %4d  %s
CHAN_ACCESS_COUNT
	Список доступа %s имеет %d записей.

# XOP (SOP/AOP/etc.) responses
### CHANGED: syntax changed, CHAN_NOP_SYNTAX and *_LIST_SYNTAX added
#CHAN_SOP_SYNTAX
#	SOP канал {ADD | DEL | LIST | COUNT} [ник | элемент списка]
#CHAN_AOP_SYNTAX
#	AOP канал {ADD | DEL | LIST | COUNT} [ник | элемент списка]
#CHAN_HOP_SYNTAX
#	HOP канал {ADD | DEL | LIST | COUNT} [ник | элемент списка]
#CHAN_VOP_SYNTAX
#	VOP канал {ADD | DEL | LIST | COUNT} [ник | элемент списка]
CHAN_XOP_ADDED
	%s добавлен в %s %s список.
CHAN_XOP_LEVEL_CHANGED
	%s перемещен в %s %s список.
CHAN_XOP_LEVEL_UNCHANGED
	%s уже в %s %s список.
CHAN_XOP_NICKS_ONLY
	SOP, AOP, VOP, и NOP списки могут содержать только зарегистрированные ники.
CHAN_XOP_NICKS_ONLY_HOP
	SOP, AOP, HOP, VOP, и NOP списки могут содержать только зарегистрированные ники.
### CHANGED
#CHAN_XOP_REACHED_LIMIT
#	Извините, суммарное количество записей в SOP, AOP и VOP списках
#	не может быть более чем %d записей.  Превышен лимит.
#CHAN_XOP_REACHED_LIMIT_HOP
#	Извините, суммарное количество записей в SOP, AOP, HOP и VOP списках
#	не может быть более чем %d записей.  Превышен лимит.
CHAN_XOP_NOT_FOUND
	%s не найден в %s %s списке.
CHAN_XOP_DELETED
	%s удален из %s %s списка.
CHAN_XOP_LIST_EMPTY
	%s %s  список пуст.
### CHANGED: CHAN_XOP_NO_MATCH added
CHAN_XOP_LIST_HEADER
	%s список %s:
CHAN_XOP_COUNT
	%s %s список имеет %d записей.

# AKICK responses
### CHANGED: syntax changed, CHAN_AKICK_LIST_SYNTAX/CHAN_AKICK_VIEW_SYNTAX added
#CHAN_AKICK_SYNTAX
#	AKICK канал {ADD | DEL | LIST | VIEW | ENFORCE | COUNT} [маска | список]
CHAN_AKICK_DISABLED
	Извините, изменение autokick списка временно отключено.
CHAN_AKICK_ALREADY_EXISTS
	%s уже находится в %s autokick списке.
CHAN_AKICK_REACHED_LIMIT
	Извините, Вы можете иметь только %d autokick масок на канале.
CHAN_AKICK_ADDED
	%s добавлен в %s autokick список.
CHAN_AKICK_NOT_FOUND
	%s не найден в %s autokick списке.
CHAN_AKICK_DELETED
	%s удален из %s autokick списке.
CHAN_AKICK_LIST_EMPTY
	%s autokick список пуст.
### CHANGED: CHAN_AKICK_NO_MATCH added
CHAN_AKICK_LIST_HEADER
	Autokick список для %s:
# mask, set-by, set-time, last-used, reason
CHAN_AKICK_VIEW_FORMAT
	    %s (установлен %s в %s; последнее попадание %s)
	        %s
# mask, set-by, set-time, reason
CHAN_AKICK_VIEW_UNUSED_FORMAT
	    %s (установлен %s в %s; никогда не активировался)
	        %s
CHAN_AKICK_COUNT
	%s имеет %d записей в AKICK списке.
CHAN_AKICK_ENFORCE_DONE
	AKICK ENFORCE для %s завершено; %d пользователей задето.

# LEVELS responses
CHAN_LEVELS_SYNTAX
	LEVELS канал {SET | DIS[ABLE] | LIST | RESET} [пункт [уровень]]
CHAN_LEVELS_READONLY
	Извините, изменение уровней привилегий канала временно отключено.
CHAN_LEVELS_RANGE
	Уровень должен быть между %d и %d включительно.
CHAN_LEVELS_CHANGED
	Уровень для %s на канале %s изменен на %d.
CHAN_LEVELS_UNKNOWN
	Значение %s неизвестно. Наберите /msg %s HELP LEVELS DESC для получения списка правильных значений.
CHAN_LEVELS_DISABLED
	%s отключено на канале %s.
### CHANGED: "access level" -> "privilege level"
#CHAN_LEVELS_LIST_HEADER
#	Установки уровней доступа %s:
CHAN_LEVELS_LIST_DISABLED
	    %-*s  (отключено)
CHAN_LEVELS_LIST_FOUNDER
	    %-*s  (только владелец)
CHAN_LEVELS_LIST_NORMAL
	    %-*s  %4d
### CHANGED: "access level" -> "privilege level"
#CHAN_LEVELS_RESET
#	Уровни доступа %s установлены по умолчанию.

# INFO responses
CHAN_INFO_SYNTAX
	INFO канал [ALL]
CHAN_INFO_HEADER
	Информация о канале %s:
CHAN_INFO_FOUNDER
	           Владелец: %s
CHAN_INFO_SUCCESSOR
	           Преемник: %s
CHAN_INFO_DESCRIPTION
	           Описание: %s
CHAN_INFO_ENTRYMSG
	Сообщение при входе: %s
CHAN_INFO_TIME_REGGED
	    Зарегистрирован: %s
CHAN_INFO_LAST_USED
	        Использован: %s
CHAN_INFO_LAST_TOPIC
	    Последний топик: %s
CHAN_INFO_TOPIC_SET_BY
	   Топик установлен: %s
CHAN_INFO_URL
	                URL: %s
CHAN_INFO_EMAIL
	       E-mail адрес: %s
CHAN_INFO_OPTIONS
	              Опции: %s
CHAN_INFO_OPT_PRIVATE
	Секретность
CHAN_INFO_OPT_KEEPTOPIC
	Восстановление топика
CHAN_INFO_OPT_TOPICLOCK
	Блокировка топика
CHAN_INFO_OPT_SECUREOPS
	Охрана статуса оператора
CHAN_INFO_OPT_LEAVEOPS
	Разрешение статуса оператора непривилегированным лицам
CHAN_INFO_OPT_RESTRICTED
	Ограничение доступа
CHAN_INFO_OPT_SECURE
	Охрана
CHAN_INFO_OPT_OPNOTICE
	Уведомления о OP/DEOP
CHAN_INFO_OPT_ENFORCE
	Приведение в силу
CHAN_INFO_OPT_NONE
	Нет
CHAN_INFO_MODE_LOCK
	Блокировка режимов: %s
CHAN_INFO_NO_EXPIRE
	Регистрация канала не завершится.
CHAN_INFO_SUSPEND_DETAILS
	Приостановлен %s в %s (%s).
CHAN_INFO_SUSPEND_REASON
	          Причина: %s
CHAN_INFO_SHOW_ALL
	Для дополнительной информации наберите: /msg %s INFO %s ALL

# LIST responses
### CHANGED
#CHAN_LIST_SYNTAX
#	LIST шаблон
#CHAN_LIST_OPER_SYNTAX
#	LIST шаблон [FORBIDDEN] [NOEXPIRE] [SUSPENDED]
CHAN_LIST_HEADER
	Список записей, соответствующих %s:
### CHANGED: CHAN_LIST_NO_MATCH added

# INVITE responses
CHAN_INVITE_SYNTAX
	INVITE канал
CHAN_INVITE_OK
	Приглашаем %s на канал %s.

# OP/DEOP/VOICE/DEVOICE responses
CHAN_OPVOICE_SYNTAX
	%s канал [ник]
CHAN_OP_SUCCEEDED
	%s получил(а) статус оператора на канале %s.
CHAN_OP_ALREADY
	%s уже оператор на канале %s.
CHAN_OP_FAILED
	Невозможно установить статус оператора для %s на канале %s.
CHAN_DEOP_SUCCEEDED
	С %s снят статус оператора на канале %s.
CHAN_DEOP_ALREADY
	%s не оператор на канале %s.
CHAN_DEOP_FAILED
	Невозможно снять статус оператора для %s на канале %s.
CHAN_VOICE_SUCCEEDED
	%s получил(а) статус голоса на канале %s.
CHAN_VOICE_ALREADY
	%s уже имеет статус голоса на канале %s.
CHAN_VOICE_FAILED
	Невозможно дать статус голоса для %s на канале %s.
CHAN_DEVOICE_SUCCEEDED
	С %s снят статус голоса на канале %s.
CHAN_DEVOICE_ALREADY
	%s не имеет статус голоса на канале %s.
CHAN_DEVOICE_FAILED
	Невозможно снять статус голоса с %s на канале %s.
CHAN_HALFOP_SUCCEEDED
	%s получил(а) статус полуоператора на канале %s.
CHAN_HALFOP_ALREADY
	%s уже имеет статус полуоператора на канале %s.
CHAN_HALFOP_FAILED
	Невозможно дать статус полуоператора для %s на канале %s.
CHAN_DEHALFOP_SUCCEEDED
	С %s снят статус полуоператора на канале %s.
CHAN_DEHALFOP_ALREADY
	%s не имеет статус полуоператора на канале %s.
CHAN_DEHALFOP_FAILED
	Невозможно снять статус полуоператора для %s на канале %s.
CHAN_PROTECT_SUCCEEDED
	Предоставлена защита для %s на канале %s.
CHAN_PROTECT_ALREADY
	%s уже защищен(а) на канале %s.
CHAN_PROTECT_FAILED
	Невозможно дать защиту для %s на канале %s.
CHAN_DEPROTECT_SUCCEEDED
	С %s снята защита на канале %s.
CHAN_DEPROTECT_ALREADY
	%s не имеет защиты на канале %s.
CHAN_DEPROTECT_FAILED
	Невозможно снять защиту с %s на канале %s.

# UNBAN responses
CHAN_UNBAN_SYNTAX
	UNBAN канал
CHAN_UNBANNED
	С Вас снят бан на канале %s.

# KICK responses
CHAN_KICK_SYNTAX
	KICK канал ник [причина]
CHAN_KICK_PROTECTED
	%s не может быть выброшен(а) с канала %s.
CHAN_KICKED
	%s выброшен(а) с канала %s.

# TOPIC responses
CHAN_TOPIC_SYNTAX
	TOPIC канал топик

# CLEAR responses
CHAN_CLEAR_SYNTAX
	CLEAR канал что
CHAN_CLEARED_BANS
	Все баны на канале %s сняты.
CHAN_CLEARED_EXCEPTIONS
	Все исключения из банов на канале %s сняты.
CHAN_CLEARED_INVITES
	Все маски приглашений на канале %s сняты.
CHAN_CLEARED_MODES
	Все режимы на канале %s сброшены.
CHAN_CLEARED_OPS
	Режим +o очищен на канале %s.
CHAN_CLEARED_HALFOPS
	Режим +h очищен на канале %s.
CHAN_CLEARED_VOICES
	Режим +v очищен на канале %s.
CHAN_CLEARED_USERS
	Все пользователи выброшены с канала %s.

# GETPASS responses
CHAN_GETPASS_SYNTAX
	GETPASS канал
### CHANGED
#CHAN_GETPASS_UNAVAILABLE
#	Команда GETPASS недоступна, т.к. используется шифрование.
CHAN_GETPASS_PASSWORD_IS
	Пароль для канала %s: %s.

# FORBID responses
CHAN_FORBID_SYNTAX
	FORBID канал
CHAN_FORBID_SHORT_CHANNEL
	Канал # не может быть запрещен.
CHAN_FORBID_SUCCEEDED
	Канал %s запрещен к использованию.
CHAN_FORBID_FAILED
	Не удалось запретить канал %s!

# SUSPEND responses
CHAN_SUSPEND_SYNTAX
	SUSPEND [+время_завершение] канал причина
CHAN_SUSPEND_SUCCEEDED
	Канал %s приостановлен.
CHAN_SUSPEND_ALREADY_SUSPENDED
	Канал %s уже приостановлен.

# UNSUSPEND responses
CHAN_UNSUSPEND_SYNTAX
	UNSUSPEND канал
CHAN_UNSUSPEND_SUCCEEDED
	Использование канала %s возобновлено.
CHAN_UNSUSPEND_NOT_SUSPENDED
	Канал %s не был приостановлен.

###########################################################################
#
# MemoServ messages
#
###########################################################################

# Automatic messages
MEMO_HAVE_NEW_MEMO
	Вы имеете одну новую записку.
MEMO_HAVE_NEW_MEMOS
	Вы имеете %d новых записок.
MEMO_TYPE_READ_LAST
	Наберите /msg %s READ LAST для прочтения.
MEMO_TYPE_READ_NUM
	Наберите /msg %s READ %d для прочтения.
MEMO_TYPE_LIST_NEW
	Наберите /msg %s LIST NEW для получения списка.
MEMO_AT_LIMIT
	Предупреждение: Вы достигли лимита записок (%d). Вы не можете получать новые записки, пока не удалите некоторые из имеющихся у Вас сейчас.
MEMO_OVER_LIMIT
	Предупреждение: Вы превысили лимит записок (%d). Вы не можете получать новые записки, пока не удалите некоторые из имеющихся у Вас сейчас.
MEMO_NEW_MEMO_ARRIVED
	Вы имеет новую записку от %s.
	Наберите /msg %s READ %d для ее прочтения.
### CHANGED: MEMO_NEW_CHAN_MEMO_ARRIVED added

# Multi-use responses
MEMO_HAVE_NO_MEMOS
	У Вас нет записок.
### CHANGED: MEMO_HAVE_NO_NEW_MEMOS added
MEMO_DOES_NOT_EXIST
	Записки %d не существует!
MEMO_LIST_NOT_FOUND
	Соответствующих записок не найдено.

# SEND responses
MEMO_SEND_SYNTAX
	SEND {ник | канал} текст записки
MEMO_SEND_DISABLED
	Извините, рассылка записок временно отключена.
MEMO_SEND_PLEASE_WAIT
	Пожалуйста, подождите %s прежде чем использовать команду SEND еще раз.
### CHANGED: MEMO_SEND_CHAN_NOT_AVAIL added
MEMO_X_GETS_NO_MEMOS
	%s не может принимать записки.
MEMO_X_HAS_TOO_MANY_MEMOS
	У %s слишком много записок и поэтому он(она) не может принимать еще.
MEMO_SENT
	Записка послана для %s.
### CHANGED: MEMO_SEND_FAILED added

# LIST responses
### CHANGED
#MEMO_LIST_SYNTAX
#	LIST [канал] [номер | список | NEW]
MEMO_LIST_MEMOS
	Записки для %s. Чтобы их прочитать, наберите: /msg %s READ номер
MEMO_LIST_NEW_MEMOS
	Новые записки для %s. Чтобы их прочитать, наберите: /msg %s READ номер
MEMO_LIST_HEADER
	   Номер  Отправитель            Дата/Время
MEMO_LIST_FORMAT
	%c%c%c%3d  %-16s  %s

# READ responses
### CHANGED
#MEMO_READ_SYNTAX
#	READ [канал] {номер | список | LAST | NEW}
MEMO_HEADER
	Записка %d от %s (%s). Чтобы удалить, наберите: /msg %s DEL %d
### CHANGED
#MEMO_CHAN_HEADER
#	Записка %d от %s (%s). Чтобы удалить, наберите: /msg %s DEL %s %d

# SAVE responses
### CHANGED
#MEMO_SAVE_SYNTAX
#	SAVE [канал] {номер |список}
MEMO_SAVED_ONE
	Записка %d теперь сохранена.
MEMO_SAVED_SEVERAL
	%d записок сохранено.

# DEL responses
### CHANGED
#MEMO_DEL_SYNTAX
#	DEL [канал] {номер | список | ALL}
MEMO_DELETED_NONE
	Ни одной записки не удалено.
MEMO_DELETED_ONE
	Записка %d удалена.
MEMO_DELETED_SEVERAL
	%d записок удалено.
MEMO_DELETED_ALL
	Все Ваши записки удалены.

# RENUMBER responses
### CHANGED: RENUMBER command added

# SET responses
MEMO_SET_SYNTAX
	SET опция параметры
MEMO_SET_DISABLED
	Извините, установка опций записок временно отключено.
MEMO_SET_UNKNOWN_OPTION
	Неизвестная SET опция %s.

# SET NOTIFY responses
MEMO_SET_NOTIFY_SYNTAX
	SET NOTIFY {ON | LOGON | NEW | OFF}
MEMO_SET_NOTIFY_ON
	%s теперь будет уведомлять Вас о записках при возвращении в онлайн, а также при их отправлении Вам.
MEMO_SET_NOTIFY_LOGON
	%s теперь будет уведомлять Вас о записках при возвращении в онлайн или снятии статуса /AWAY.
MEMO_SET_NOTIFY_NEW
	%s теперь будет уведомлять Вас о записках при их отправлении.
MEMO_SET_NOTIFY_OFF
	%s теперь не будет уведомлять Вас о записках.

# SET LIMIT responses
### CHANGED
#MEMO_SET_LIMIT_SYNTAX
#	SET LIMIT [канал] лимит
#MEMO_SET_LIMIT_OPER_SYNTAX
#	SET LIMIT [ник | канал] {лимит | NONE | DEFAULT} [HARD]
MEMO_SET_YOUR_LIMIT_FORBIDDEN
	У Вас нет прав изменять лимиты Ваших записок.
MEMO_SET_LIMIT_FORBIDDEN
	Лимит записок для %s не может быть изменен.
MEMO_SET_YOUR_LIMIT_TOO_HIGH
	Вы не можете установить Ваш лимит записок более чем %d.
MEMO_SET_LIMIT_TOO_HIGH
	Вы не можете установить лимит для %s более чем %d.
MEMO_SET_LIMIT_OVERFLOW
	Лимит записок слишком велик; установлено в %d.
MEMO_SET_YOUR_LIMIT
	Ваш лимит записок установлен в %d.
MEMO_SET_YOUR_LIMIT_ZERO
	Теперь Вы не будете принимать записки.
MEMO_SET_YOUR_LIMIT_DEFAULT
	Ваш лимит установлен по умолчанию (%d).
MEMO_UNSET_YOUR_LIMIT
	Теперь у Вас нет лимита записок.
MEMO_SET_LIMIT
	Лимит записок для %s установлен в %d.
MEMO_SET_LIMIT_ZERO
	Лимит записок для %s установлен в 0.
MEMO_SET_LIMIT_DEFAULT
	Лимит записок для %s установлен по умолчанию (%d).
MEMO_UNSET_LIMIT
	У %s теперь нет лимита записок.

# INFO responses
MEMO_INFO_NO_MEMOS
	У Вас нет записок.
MEMO_INFO_MEMO
	У Вас 1 записка.
MEMO_INFO_MEMO_UNREAD
	У Вас 1 записка и Вы ее еще не прочитали.
MEMO_INFO_MEMOS
	У Вас %d записок.
MEMO_INFO_MEMOS_ONE_UNREAD
	У Вас %d записок, из которых 1 непрочитана.
MEMO_INFO_MEMOS_SOME_UNREAD
	У Вас %d записок, из который Вы не прочитали %d.
MEMO_INFO_MEMOS_ALL_UNREAD
	У Вас %d записок; ни одна из них не прочитана.
MEMO_INFO_LIMIT
	Ваш лимит записок: %d.
MEMO_INFO_HARD_LIMIT
	Ваш лимит записок %d, Вы не можете его изменять.
MEMO_INFO_LIMIT_ZERO
	Ваш лимит записок 0; Вы не можете принимать ни одной новой записки.
MEMO_INFO_HARD_LIMIT_ZERO
	Ваш лимит записок 0; Вы не можете принимать ни одной новой записки. Вы не можете изменять лимит.
MEMO_INFO_NO_LIMIT
	У Вас нет лимита записок, которые Вы можете хранить.
MEMO_INFO_NOTIFY_OFF
	Вы не будете информированы о новых записках.
MEMO_INFO_NOTIFY_ON
	Вы будете информированы о новых записках при заходе и при их получении.
MEMO_INFO_NOTIFY_RECEIVE
	Вы будете информированы о новых записках при их получении.
MEMO_INFO_NOTIFY_SIGNON
	Вы будете информированы о новых записках при заходе на IRC.
MEMO_INFO_X_NO_MEMOS
	%s не имеет записок.
MEMO_INFO_X_MEMO
	%s имеет 1 записку.
MEMO_INFO_X_MEMO_UNREAD
	%s имеет 1 записку и она еще не прочитана.
MEMO_INFO_X_MEMOS
	%s имеет %d записок.
MEMO_INFO_X_MEMOS_ONE_UNREAD
	%s имеет %d записок, из которых 1 непрочитана.
MEMO_INFO_X_MEMOS_SOME_UNREAD
	%s имеет %d записок, из которых %d непрочитано.
MEMO_INFO_X_MEMOS_ALL_UNREAD
	%s имеет %d записок; ни одна не прочитана.
MEMO_INFO_X_LIMIT
	Лимит записок для %s: %d.
MEMO_INFO_X_HARD_LIMIT
	Лимит записок для %s %d и не может быть изменен.
MEMO_INFO_X_NO_LIMIT
	%s не имеет лимита записок.
MEMO_INFO_X_NOTIFY_OFF
	%s получает уведомления о новых записках.
MEMO_INFO_X_NOTIFY_ON
	%s получает уведомления о новых записках при заходе и при их получении.
MEMO_INFO_X_NOTIFY_RECEIVE
	%s получает уведомления о новых записках при их получении.
MEMO_INFO_X_NOTIFY_SIGNON
	%s получает уведомления о новых записках при заходе.

################ forward module messages/responses

# Mail format
MEMO_FORWARD_MAIL_SUBJECT
	Записка для %s
### CHANGED: MEMO_FORWARD_MULTIPLE_MAIL_SUBJECT added
MEMO_FORWARD_MAIL_BODY
	Записка от %s (%s)
	%s
### CHANGED: MEMO_FORWARD_CHANMEMO_MAIL_BODY added

# FORWARD responses
MEMO_FORWARD_SYNTAX
	FORWARD {номер | список | ALL}
MEMO_FORWARD_NEED_EMAIL
	Вы должны установить E-mail адрес до того, как записки будут переправляться Вам.
MEMO_FORWARD_PLEASE_WAIT
	Пожалуйста подождите %s прежде чем использовать команду FORWARD еще раз.
MEMO_FORWARD_FAILED
	Извините, переправить не удалось.
MEMO_FORWARDED_NONE
	Ни одна записка не была переправлена.
MEMO_FORWARDED_ONE
	Записка %d переправлена.
MEMO_FORWARDED_SEVERAL
	%d записок переправлено.
MEMO_FORWARDED_ALL
	Все Ваши записки переправлены.

# SET FORWARD responses
MEMO_SET_FORWARD_SYNTAX
	SET FORWARD {ON | COPY | OFF}
MEMO_SET_FORWARD_ON
	Ваши записки будут переправляться на Ваш E-mail адрес: %s
MEMO_SET_FORWARD_COPY
	Ваши записки будут переправляться на Ваш E-mail адрес: %s
	Ваши записки также будут сохранены Сервисами.
MEMO_SET_FORWARD_OFF
	Ваши записки не будут переправляться на Ваш E-mail адрес.

################ ignore module messages/responses

# IGNORE responses
MEMO_IGNORE_SYNTAX
	IGNORE {ADD | DEL | LIST} [ник | маска]
MEMO_IGNORE_ADD_SYNTAX
	IGNORE ADD ник | маска
MEMO_IGNORE_DEL_SYNTAX
	IGNORE DEL ник | маска
MEMO_IGNORE_LIST_FULL
	Ваш список игнорирования полон.
MEMO_IGNORE_ALREADY_PRESENT
	%s уже в списке игнорирования.
MEMO_IGNORE_ADDED
	%s добавлен в Ваш список игнорирования.
MEMO_IGNORE_NOT_FOUND
	%s не найден в Вашем списке игнорирования.
MEMO_IGNORE_DELETED
	%s удален из Вашего списка игнорирования.
MEMO_IGNORE_LIST_EMPTY
	Ваш список игнорирования пуст.
MEMO_IGNORE_LIST
	Список игнорирования:
MEMO_IGNORE_LIST_X_EMPTY
	Список игнорирования у %s пуст.
MEMO_IGNORE_LIST_X
	Список игнорирования у %s:

###########################################################################
#
# OperServ messages
#
###########################################################################

# General messages
OPER_BOUNCY_MODES
	Сервисы не могут изменять режимы. Ваши серверы правильно настроены?
OPER_BOUNCY_MODES_U_LINE
	Сервисы не могут изменять режимы. U:lines Ваших серверов правильно настроены?

# GLOBAL responses
OPER_GLOBAL_SYNTAX
	GLOBAL сообщение

# STATS responses
OPER_STATS_UNKNOWN_OPTION
	Неизвестная STATS опция %s.
OPER_STATS_RESET_USER_COUNT
	Счетчик максимального количества посетителей обнулен.

OPER_STATS_CURRENT_USERS
	Посетителей сейчас: %d (%d операторов)
OPER_STATS_MAX_USERS
	Максимум посетителей: %d (%s)
# Parameters: days, STR_DAYS or STR_DAY, hours, minutes
OPER_STATS_UPTIME_DHM
	Сервисы в работе %d%s, %02d:%02d
# If >= 1 hour, hours and minutes; if < 1 hour, minutes and seconds
OPER_STATS_UPTIME_HM_MS
	Сервисы в работе %s

OPER_STATS_KBYTES_READ
	          Данных принято: %10u kB
OPER_STATS_KBYTES_WRITTEN
	         Данных передано: %10u kB
OPER_STATS_NETBUF_SOCK
	Буферы серверных сокетов: %5u kB
OPER_STATS_NETBUF_SOCK_PERCENT
	Буферы серверных сокетов: %5u kB (%d%%)
OPER_STATS_NETBUF_TOTAL
	   Итого буферов сокетов: %5u kB
OPER_STATS_NETBUF_TOTAL_PERCENT
	   Итого буферов сокетов: %5u kB (%d%%)

OPER_STATS_ALL_USER_MEM
	             Посетителей: %6d записей, %5d kB
OPER_STATS_ALL_CHANNEL_MEM
	                 Каналов: %6d записей, %5d kB
OPER_STATS_ALL_SERVER_MEM
	                Серверов: %6d записей, %5d kB
OPER_STATS_ALL_NICKGROUPINFO_MEM
	             Групп ников: %6d записей, %5d kB
OPER_STATS_ALL_NICKINFO_MEM
	                   Ников: %6d записей, %5d kB
OPER_STATS_ALL_CHANSERV_MEM
	                ChanServ: %6d записей, %5d kB
OPER_STATS_ALL_STATSERV_MEM
	                StatServ: %6d записей, %5d kB
OPER_STATS_ALL_NEWS_MEM
	                    News: %6d записей, %5d kB
OPER_STATS_ALL_AKILL_MEM
	               Autokills: %6d записей, %5d kB
OPER_STATS_ALL_EXCEPTION_MEM
	              Исключений: %6d записей, %5d kB
OPER_STATS_ALL_SGLINE_MEM
	                 SGlines: %6d записей, %5d kB
OPER_STATS_ALL_SQLINE_MEM
	                 SQlines: %6d записей, %5d kB
OPER_STATS_ALL_SZLINE_MEM
	                 SZlines: %6d записей, %5d kB
OPER_STATS_ALL_SESSION_MEM
	                  Сессий: %6d записей, %5d kB

# GETKEY responses
### CHANGED: OPER_GETKEY_* added

# MODE responses
OPER_MODE_SYNTAX
	MODE канал режимы

# CLEARMODES responses
OPER_CLEARMODES_SYNTAX
	CLEARMODES канал [ALL]
### CHANGED
#OPER_CLEARMODES_DONE
#	Режимы и баны удалены на канале %s.
#OPER_CLEARMODES_ALL_DONE
#	Все режимы удалены на канале %s.

# CLEARCHAN responses
OPER_CLEARCHAN_SYNTAX
	CLEARCHAN канал
OPER_CLEARCHAN_DONE
	Все посетители удалены с канала %s.

# KICK responses
OPER_KICK_SYNTAX
	KICK канал пользователь причина

# ADMIN responses
OPER_ADMIN_SYNTAX
	ADMIN {ADD | DEL | LIST} [ник]
OPER_ADMIN_ADD_SYNTAX
	ADMIN ADD ник
OPER_ADMIN_DEL_SYNTAX
	ADMIN DEL ник
OPER_ADMIN_NO_NICKSERV
	NickServ отключен; команда ADMIN недоступна.
OPER_ADMIN_EXISTS
	%s уже в списке администраторов Сервисов.
OPER_ADMIN_ADDED
	%s добавлен в список администраторов Сервисов.
OPER_ADMIN_TOO_MANY
	Слишком много записей (%d) в списке администраторов Сервисов; невозможно добавить еще.
OPER_ADMIN_REMOVED
	%s удален из списка администраторов Сервисов.
OPER_ADMIN_NOT_FOUND
	%s не найден в списке администраторов Сервисов.
OPER_ADMIN_LIST_HEADER
	Список администраторов Сервисов:

# OPER responses
OPER_OPER_SYNTAX
	OPER {ADD | DEL | LIST} [ник]
OPER_OPER_ADD_SYNTAX
	OPER ADD ник
OPER_OPER_DEL_SYNTAX
	OPER DEL ник
OPER_OPER_NO_NICKSERV
	NickServ отключен; команда OPER недоступна.
OPER_OPER_EXISTS
	%s уже в списке операторов Сервисов.
OPER_OPER_ADDED
	%s добавлен в список операторов Сервисов.
OPER_OPER_TOO_MANY
	Слишком много записей (%d) в списке операторов Сервисов; невозможно добавить еще.
OPER_OPER_REMOVED
	%s удален из списка операторов Сервисов.
OPER_OPER_NOT_FOUND
	%s не найден в списке операторов Сервисов.
OPER_OPER_LIST_HEADER
	Список операторов Сервисов:

# AKILL/EXCLUDE/SxLINE common responses
### CHANGED: OPER_MASKDATA_* added
# mask, reason
OPER_MASKDATA_LIST_FORMAT
	%-32s  %s
# mask, set-by, set-time, last-used, expires, reason
OPER_MASKDATA_VIEW_FORMAT
	%s (установлен %s в %s, последнее попадание %s; %s)
	    %s
# mask, set-by, set-time, expires, reason
OPER_MASKDATA_VIEW_UNUSED_FORMAT
	%s (установлен %s в %s, никогда не активировался; %s)
	    %s

# AKILL responses
OPER_TOO_MANY_AKILLS
	Autokill список полон!
OPER_AKILL_EXISTS
	%s уже в autokill списке.
OPER_AKILL_NO_NICK
	Уведомление: Autokill маска не может содержать ники; убедитесь, что Вы не включили в маску ник.
OPER_AKILL_MASK_TOO_GENERAL
	Autokill маска слишком общая.
OPER_AKILL_EXPIRY_LIMITED
	Время завершения должно быть позднее, чем %s.
OPER_AKILL_ADDED
	%s уже в autokill списке.
OPER_AKILL_REMOVED
	%s удален из autokill списка.
OPER_AKILL_NOT_FOUND
	%s не найден в autokill списке.
OPER_AKILL_LIST_HEADER
	Autokill список:
### CHANGED: OPER_AKILL_{LIST_EMPTY,LIST_NO_MATCH,CHECK_*} added
OPER_AKILL_COUNT
	Autokill список содержит %d масок.

# AKILLCHAN responses
OPER_AKILLCHAN_SYNTAX
	AKILLCHAN [KILL] [+время_завершение] канал причина
OPER_AKILLCHAN_AKILLED
	%d пользователей отключено согласно списку autokill.
OPER_AKILLCHAN_KILLED
	%d пользователей отключено.
OPER_AKILLCHAN_AKILLED_ONE
	1 пользователь отключен согласно списку autokill.
OPER_AKILLCHAN_KILLED_ONE
	1 пользователь отключен.

# EXCLUDE responses
OPER_TOO_MANY_EXCLUDES
	EXCLUDE список полон!
OPER_EXCLUDE_EXISTS
	%s уже находится в списке autokill исключений.
OPER_EXCLUDE_ADDED
	%s добавлен в список autokill исключений.
OPER_EXCLUDE_REMOVED
	%s удален из списка autokill исключений.
OPER_EXCLUDE_NOT_FOUND
	%s не найден в списке autokill исключений.
OPER_EXCLUDE_LIST_HEADER
	Список autokill исключений:
### CHANGED: OPER_EXCLUDE_{LIST_EMPTY,LIST_NO_MATCH,CHECK_*} added
OPER_EXCLUDE_COUNT
	Список autokill исключений имеет %d записей.

# SxLINE responses
OPER_TOO_MANY_SLINES
	%s список полон!
OPER_SLINE_EXISTS
	%s уже в %s списке.
OPER_SLINE_MASK_TOO_GENERAL
	Маска %s слишком общая.
OPER_SLINE_ADDED
	%s добавлен в %s список.
OPER_SLINE_REMOVED
	%s удален из %s списка.
OPER_SLINE_NOT_FOUND
	%s не найден в %s списке.
OPER_SLINE_LIST_HEADER
	%s список:
### CHANGED: OPER_SLINE_{LIST_EMPTY,LIST_NO_MATCH,CHECK_*} added
OPER_SLINE_COUNT
	%d записей в %s списке.
OPER_SZLINE_NOT_AVAIL
	SZLINE недоступен на этом IRC сервере.

# SU responses
OPER_SU_SYNTAX
	SU пароль
OPER_SU_NO_PASSWORD
	Команда SU недоступна, т.к. не установлен пароль суперпользователя.
OPER_SU_SUCCEEDED
	Теперь Вы имеете привилегии суперпользователя на Сервисах.
OPER_SU_FAILED
	Невозможно проверить пароль.

# SET responses
OPER_SET_SYNTAX
	SET опция значение
OPER_SET_IGNORE_ON
	Код игнорирования будет использован.
OPER_SET_IGNORE_OFF
	Код игнорирования не будет использован.
OPER_SET_IGNORE_ERROR
	Опция IGNORE должна иметь значения только ON или OFF.
OPER_SET_READONLY_ON
	Сервисы сейчас в режиме только чтение.
OPER_SET_READONLY_OFF
	Сервисы сейчас в режиме чтения-записи.
OPER_SET_READONLY_ERROR
	Опция READONLY должна иметь значения только ON или OFF.
OPER_SET_DEBUG_ON
	Сервисы находятся в режиме отладки.
OPER_SET_DEBUG_OFF
	Сервисы больше не находятся в режиме отладки.
OPER_SET_DEBUG_LEVEL
	Сервисы находятся в режиме отладки (уровень %d).
OPER_SET_DEBUG_ERROR
	Опция DEBUG должна иметь значения только ON, OFF, или положительное число.
OPER_SET_SUPASS_FAILED
	Невозможно установить пароль суперпользователя для Сервисов.
OPER_SET_SUPASS_OK
	Пароль суперпользователя для Сервисов изменен.
OPER_SET_SUPASS_NONE
	Пароль суперпользователя для Сервисов отключен.
OPER_SET_UNKNOWN_OPTION
	Неизвестная опция %s.

# JUPE responses
OPER_JUPE_SYNTAX
	JUPE сервер [причина]
OPER_JUPE_INVALID_NAME
	Неправильное имя сервера (имя сервера должно содержать точки).
OPER_JUPE_ALREADY_JUPED
	Сервер %s уже в джупинге.

# RAW responses
OPER_RAW_SYNTAX
	RAW текст

# UPDATE responses
OPER_UPDATE_SYNTAX
	UPDATE [FORCE]
OPER_UPDATE_FORCE_FAILED
	Невозможно разблокировать базы.
OPER_UPDATING
	Обновление баз...
OPER_UPDATE_COMPLETE
	Обновление баз прошло успешно.
OPER_UPDATE_FAILED
	Не удалось обновить базы.

# REHASH responses
OPER_REHASHING
	Перечитывание конфигурационного файла.
OPER_REHASHED
	Все конфигурационные настройки успешно обновлены.
OPER_REHASH_ERROR
	Произошла ошибка во время перечитывания конфигурационных настроек. Пожалуйста, проверьте лог Сервисов для получения дополнительной информации.

# KILLCLONES responses
OPER_KILLCLONES_SYNTAX
	KILLCLONES ник
OPER_KILLCLONES_UNKNOWN_NICK
	Не могу найти посетителя %s.
OPER_KILLCLONES_KILLED
	%s использовал команду KILLCLONES для %s отключения %d клонов.
OPER_KILLCLONES_KILLED_AKILL
	%s использовал команду KILLCLONES для %s отключения %d клонов.  Добавлен временный AKILL для %s.


###########################################################################
#
# Session limiting and exception messages
#
###########################################################################

# EXCEPTION responses
### CHANGED
#OPER_EXCEPTION_SYNTAX
#	EXCEPTION {ADD | DEL | MOVE | LIST | VIEW} [параметры]
OPER_EXCEPTION_ADD_SYNTAX
	EXCEPTION ADD [+время_завершение] маска лимит причина
OPER_EXCEPTION_DEL_SYNTAX
	EXCEPTION DEL {маска | список}
OPER_EXCEPTION_MOVE_SYNTAX
	EXCEPTION MOVE номер новый_номер
### CHANGED: OPER_EXCEPTION_LIST_SYNTAX added
OPER_EXCEPTION_ALREADY_PRESENT
	Маска %s уже имеется в списке исключений.
OPER_EXCEPTION_TOO_MANY
	Список исключений лимита сессий полон!
OPER_EXCEPTION_ADDED
	Лимит сессий для %s установлен в %d.
OPER_EXCEPTION_MOVED
	Исключение для %s (#%d) перемещено в позицию %d.
OPER_EXCEPTION_NO_SUCH_ENTRY
	Нет такой записи (#%d) в списке исключений лимитов сессий.
OPER_EXCEPTION_NOT_FOUND
	%s не найден в списке исключений лимитов сессий.
OPER_EXCEPTION_NO_MATCH
	Нет соответствующих записей в списке исключений лимитов сессий.
OPER_EXCEPTION_EMPTY
	Список исключений лимитов сессий пуст.
OPER_EXCEPTION_DELETED
	%s удален из списка исключений лимитов сессий.
OPER_EXCEPTION_DELETED_ONE
	Удалена 1 запись из списка исключений лимитов сессий.
OPER_EXCEPTION_DELETED_SEVERAL
	Удалено %d записей из списка исключений лимитов сессий.
OPER_EXCEPTION_LIST_HEADER
	Текущий список исключений лимитов сессий:
# number, limit, host
OPER_EXCEPTION_LIST_COLHEAD
	Номер  Лимит  Хост
OPER_EXCEPTION_LIST_FORMAT
	%3d  %4d   %s
# number, mask, set-by, set-time, expires, limit, reason
OPER_EXCEPTION_VIEW_FORMAT
	%3d.  %s  (установлен %s в %s; %s)
	    Лимит: %-4d  - %s
### CHANGED: several messages added
OPER_EXCEPTION_INVALID_LIMIT
	Некорректный лимит сессий; он должен быть как минимум нулем и не больше чем %d.
OPER_EXCEPTION_INVALID_HOSTMASK
	Некорректная хостмаска. Только реальная хостмаска может быть добавлена в исключения, она не должна содержать ников или имен пользователей.

# SESSION responses
OPER_SESSION_SYNTAX
	SESSION {LIST лимит | VIEW хост}
OPER_SESSION_LIST_SYNTAX
	SESSION LIST лимит
OPER_SESSION_VIEW_SYNTAX
	SESSION VIEW хост
OPER_SESSION_INVALID_THRESHOLD
	Неверное пороговое значение. Оно должно быть целым числом больше 1.
OPER_SESSION_NOT_FOUND
	%s не найден в списке сессий.
OPER_SESSION_LIST_HEADER
	Хосты с меньшим чем %d количеством сессий:
OPER_SESSION_LIST_COLHEAD
	Сессии  Хост
# limit, host
OPER_SESSION_LIST_FORMAT
	%6d    %s
# host, sessions, limit
OPER_SESSION_VIEW_FORMAT
	Хост %s сейчас использует %d из %d максимально допустимых.


###########################################################################
#
# News system messages
#
###########################################################################

NEWS_LOGON_TEXT
	[Новости - %s] %s
NEWS_OPER_TEXT
	[Новости для Операторов - %s] %s

NEWS_LOGON_SYNTAX
	LOGONNEWS {ADD | DEL | LIST} [текст | номер]
NEWS_LOGON_LIST_HEADER
	Текущие новостей:
NEWS_LOGON_LIST_ENTRY
	%5d (%s от %s)
	    %s
NEWS_LOGON_LIST_NONE
	Новостей нет.
NEWS_LOGON_ADD_SYNTAX
	LOGONNEWS ADD текст
NEWS_LOGON_ADD_FULL
	Список новостей полон!
NEWS_LOGON_ADDED
	Добавлена очередная новость (#%d).
NEWS_LOGON_DEL_SYNTAX
	LOGONNEWS DEL {номер | ALL}
NEWS_LOGON_DEL_NOT_FOUND
	Новость под номером #%d не найдена!
NEWS_LOGON_DELETED
	Новость под номером #%d удалена.
NEWS_LOGON_DEL_NONE
	Нет новостей, подлежащих удалению!
NEWS_LOGON_DELETED_ALL
	Все новости удалены.

NEWS_OPER_SYNTAX
	OPERNEWS {ADD | DEL | LIST} [текст | номер]
NEWS_OPER_LIST_HEADER
	Список новостей для Операторов:
NEWS_OPER_LIST_ENTRY
	%5d (%s от %s)
	    %s
NEWS_OPER_LIST_NONE
	Нет новостей для Операторов.
NEWS_OPER_ADD_SYNTAX
	OPERNEWS ADD текст
NEWS_OPER_ADD_FULL
	Список новостей полон!
NEWS_OPER_ADDED
	Добавлена очередная новость для Операторов (#%d).
NEWS_OPER_DEL_SYNTAX
	OPERNEWS DEL {номер | ALL}
NEWS_OPER_DEL_NOT_FOUND
	Новость для Операторов под номером #%d не найдена!
NEWS_OPER_DELETED
	Новость для Операторов под номером #%d удалена.
NEWS_OPER_DEL_NONE
	Нет новостей для Операторов, подлежащих удалению!
NEWS_OPER_DELETED_ALL
	Все новости для Операторов удалены.

NEWS_HELP_LOGON
	Синтаксис: LOGONNEWS ADD текст
	        LOGONNEWS DEL {номер | ALL}
	        LOGONNEWS LIST
	
	Управление списком новостей. Когда пользователь
	заходит в IRC сеть, эти новости будут отправлены
	ему. (Тем не менее, не более трех сообщений будет
	отправлено, чтобы не допустить флуда. Если у Вас новостей
	больше трех, только три последние новости
	будут посланы.)
	
	Команда LOGONNEWS LIST может быть использована любым IRC Оператором для просмотра
	текущих новостей. Команды ADD и DEL могут использовать только
	Операторами Сервисов.

NEWS_HELP_OPER
	Синтаксис: OPERNEWS ADD текст
	        OPERNEWS DEL {номер | ALL}
	        OPERNEWS LIST
	
	Управление списком новостей для Операторов. Когда пользователь
	станет IRC Оператором (использовав команду /OPER), эти сообщения
	будут ему отправлены. (Тем не менее, не более трех сообщений будет
	отправлено, чтобы не допустить флуда. Если у Вас новостей
	больше трех, только три последние новости
	будут посланы.)
	
	Команда OPERNEWS LIST может быть использована любым IRC Оператором для просмотра
	текущих новостей. Команды ADD и DEL могут использовать только
	Операторами Сервисов.

###########################################################################
#
# StatServ messages
#
############################################################################

# General SERVERS responses
STAT_SERVERS_REMOVE_SERV_FIRST
	Сервер %s онлайн.  Удалите его из IRC сети и попробуйте еще раз.
STAT_SERVERS_SERVER_EXISTS
	Сервер %s уже существует.

# SERVERS command syntax
STAT_SERVERS_SYNTAX
	SERVERS опция параметры

# STATS responses
STAT_SERVERS_STATS_TOTAL
	Всего серверов: %2d
STAT_SERVERS_STATS_ON_OFFLINE
	       Онлайн: %2d (%d%%)  Офлайн: %2d (%d%%)
STAT_SERVERS_LASTQUIT_WAS
	    Последний выход: %-30s  %s

# LIST responses
STAT_SERVERS_LIST_HEADER
	Имя сервера                    Пользователей      Операторов
STAT_SERVERS_LIST_FORMAT
	%-30s %3d (%2d%%)  %3d (%2d%%)
STAT_SERVERS_LIST_RESULTS
	Конец списка -- %d/%d онлайн серверов показано.

# VIEW responses
STAT_SERVERS_VIEW_HEADER_ONLINE
	%-30s  (онлайн)
STAT_SERVERS_VIEW_HEADER_OFFLINE
	%-30s  (офлайн)
STAT_SERVERS_VIEW_LASTJOIN
	    Последний заход: %s
STAT_SERVERS_VIEW_LASTQUIT
	    Последний выход: %s
STAT_SERVERS_VIEW_QUITMSG
	 Сообщение при выходе: %s
STAT_SERVERS_VIEW_USERS_OPERS
	        Пользователей: %3d (%2d%%)  Операторов: %3d (%2d%%)
STAT_SERVERS_VIEW_RESULTS
	Конец списка -- %d/%d серверов показано.

# DELETE responses
STAT_SERVERS_DELETE_SYNTAX
	SERVERS DELETE сервер
STAT_SERVERS_DELETE_DONE
	Статистика для сервера %s удалена.

# COPY responses
STAT_SERVERS_COPY_SYNTAX
	SERVERS COPY сервер новое_имя
STAT_SERVERS_COPY_DONE
	Статистика сервера %s скопирована в %s.

# RENAME responses
STAT_SERVERS_RENAME_SYNTAX
	SERVERS RENAME сервер новое_имя
STAT_SERVERS_RENAME_DONE
	Статистика сервера %s переименована в %s.

# USERS responses
STAT_USERS_SYNTAX
	USERS STATS
STAT_USERS_TOTUSERS
	             Всего пользователей: %d
STAT_USERS_TOTOPERS
	                Всего Операторов: %d
STAT_USERS_SERVUSERS
	Среднее количество пользователей на сервер: %d
STAT_USERS_SERVOPERS
	   Среднее количество Операторов на сервер: %d.%d


###########################################################################
#
# NickServ help messages
#
###########################################################################

NICK_HELP
	%S позволяет Вам "регистрировать" ник защищает его от использования
	кем-либо, кроме Вас. %S управляется посредством команд для
	регистрации ника и различных настроек. Для получения списка команд
	наберите /msg %S HELP COMMANDS; для использования команды наберите
	/msg %S команда, для более подробной информации о нужной команде
	наберите: /msg %S HELP команда.
NICK_HELP_EXPIRES
	
	Если зарегистрированный ник не используется в течении %s - он будет удален
	из базы и доступен любому пользователю.
NICK_HELP_WARNING
	
	Уведомление: Данный сервис предназначен для того, чтобы пользователи IRC
	могли воспользоваться многими удобными возможностями.
	Он НЕ предназначен для облегчения "воровства" ников,
	"киберсквоттинга" (захвата большого количества ников) или других зловредных действий.
	Злоупотребление %S приведет, как минимум, к уничтожению Вашего ника(ников).
### CHANGED
#NICK_HELP_COMMANDS
#	Доступные команды %S:
#	
#	    REGISTER   Регистрация ника
#	    IDENTIFY   Идентификация Вас как владельца ника
#	    DROP       Отмена регистрации ника
#NICK_HELP_COMMANDS_AUTH
#	    AUTH       Аутентификация регистрации ника
#	    SENDAUTH   Послать себе код аутентификации
#NICK_HELP_COMMANDS_LINK
#	    LINK       Связать другой ник с Вашим
#	    UNLINK     Отменить связанный ник
#	    LISTLINKS  Список связанных ников
#NICK_HELP_COMMANDS_ACCESS
#	    ACCESS     Изменение списка авторизованных адресов
#NICK_HELP_COMMANDS_AJOIN
#	    AJOIN      Изменение Вашего списка автозахода
#NICK_HELP_COMMANDS_SET
#	    SET        Установка параметров, включая защиту kill'ом
#	    UNSET      Очистка информации ника
#	    RECOVER    Отключить того, кто пользуется Вашим ником
#	    RELEASE    Снять с охраны Ваш ник после команды RECOVER
#	    GHOST      Отключить "мертвую" сессию (соединение)
#	    INFO       Показать информацию о нике
#NICK_HELP_COMMANDS_LIST
#	    LIST       Показать список ников, соответствующих указанной маске
#	    LISTEMAIL  Показать список ников с указанным E-mail адресом
#NICK_HELP_COMMANDS_LISTCHANS
#	    LISTCHANS  Показать список каналов, которые принадлежат Вам
#	    STATUS     Информация о том, идентифицировался ли пользователь как владелец ника

NICK_HELP_REGISTER
	Синтаксис: %s
	
	Регистрирует Ваш ник в базе данных %S. После того, как Вы
	зарегистрировали ник, Вы можете использовать команды SET и ACCESS,
	чтобы сконфигурировать параметры Вашего ника по Вашему вкусу.
	Не забывайте пароль, который Вы укажете при регистрации; он Вам
	понадобится, когда будете менять настройки ника.
	(Примечание: пароли чувствительны к регистру!
	MYPASSWORD, MyPassword, и mypassword - это РАЗНЫЕ пароли!)
NICK_HELP_REGISTER_EMAIL
	
	Вы можете установить E-mail адрес при регистрации ника; его также
	можно будет установить позднее командой SET EMAIL.
NICK_HELP_REGISTER_EMAIL_REQ
	
	Вы должны указать E-mail адрес при регистрации ник.
	Вы можете скрыть E-mail от показа другим пользователям командой
	SET HIDE (наберите /msg %S HELP SET HIDE для дополнительной информации).
NICK_HELP_REGISTER_EMAIL_AUTH
	
	Вы должны указать правильный E-mail адрес при регистрации ника;
	%S пошлет сообщение с кодом аутентификации, который Вы используете
	с командой AUTH для завершения процедуры регистрации. Вы можете
	скрыть E-mail от показа другим пользователям командой
	SET HIDE (наберите /msg %S HELP SET HIDE для дополнительной информации).
NICK_HELP_REGISTER_END
	
	Принципы выбора пароля:
	
	Пароль не должен легко угадываться - например Ваше имя.
	Использование Вашего ника для пароля неприемлемо, и %S не допустит этого.
	Также, короткие пароли очень легко подбираются простым перебором,
	поэтому длина пароля должна быть больше 5 символов.

NICK_HELP_IDENTIFY
	Синтаксис: IDENTIFY пароль
	
	Сообщает %S, что Вы действительно являетесь владельцем
	ника. Многие команды требуют идентификации, без этого
	Вы не сможете их использовать. Пароль должен быть
	полностью идентичен тому, который Вы указывали, когда
	использовали команду REGISTER.

NICK_HELP_DROP
	Синтаксис: DROP пароль
	
	Удаляет Ваш ник из базы данных %S.  Удаленный таким образом ник
	становится свободным для регистрации любым человеком.

NICK_HELP_DROP_LINK
	
	Учтите, что если Вы удаляете ник, то все связанные с ним
	ники также будут удалены. Для удаления одного связанного
	ника используйте команду UNLINK.
NICK_HELP_DROP_END
	
	Для исключения случайного использования этой команды, Вы должны
	ввести пароль, чтобы удалить Ваш ник.

NICK_HELP_AUTH
	Синтаксис: AUTH код
	
	Аутентификация регистрации ника а также смены E-mail адреса.
	Чтобы удостовериться, что E-mail верный, %S пошлет сообщение
	с кодом аутентификации на E-mail адрес, который Вы указала при
	регистрации или в команде SET EMAIL. Это команда позволяет
	Вам подтвердить, что Вы получили сообщение и можете продолжать
	пользоваться ником. Замените код в команде девятизначным
	кодом аутентификации, указанным в сообщении.

### CHANGED: "authorization" -> "authentication"
#NICK_HELP_SENDAUTH
#	Синтаксис: SENDAUTH
#	
#	Отправляет Вам копию сообщения с кодом аутентификации, который ранее
#	был послан после команд REGISTER или SET EMAIL. Если Вы случайно или
#	еще по каким либо другим причинам потеряли сообщение с кодом,
#	Вы можете использовать эту команду для получения кода аутентификации еще раз.
#	
#	Для предотвращения злоупотребления этой командой,
#	она может быть использована только один раз в 24 часа.

### CHANGED: NICK_HELP_REAUTH, NICK_HELP_RESTOREMAIL added

### CHANGED: added "nick must be unregistered" at end
#NICK_HELP_LINK
#	Синтаксис: LINK ник
#	
#	Связать новый ник с ником, который Вы используете сейчас, создание псевдонима
#	для Вашего ника. Новый ник будет идентичен текущему в плане привилегий и записок;
#	например, Вы можете читать записки, которые посланы другому нику и если один
#	из ников имеет привилегию auto-op на канале, Вам тоже будет автоматически
#	поставлен оператор канала независимо от того, какой ник Вы сейчас используете.
#	
#	Используйте команду UNLINK для связывания ников.
#	
#	Чтобы пользоваться этой командой, Вы должны идентифицировать Ваш ник 
#	(команда IDENTIFY) и ник, который Вы хотите привязать к Вашему не должен использоваться.

NICK_HELP_UNLINK
	Синтаксис: UNLINK ник
	
	Удаляет связь (псевдоним) с ником, которая была создана командой LINK.
	Вы не можете удалить связь с Вашим текущим ником; измените его на другой связанный ник,
	прежде чем использовать эту команду.
	
	Если Вы разрываете связь с "основным ником", который Вы установили
	командой SET MAINNICK, Ваш текущий ник автоматически становится главным ником.

NICK_HELP_LISTLINKS
	Синтаксис: LISTLINKS
	
	Показать список всех ников, которые связаны с Вашим. Главный ник в группе
	связанных ников будет помечен символом "*" слева.

NICK_HELP_ACCESS
	Синтаксис: ACCESS ADD маска
	        ACCESS DEL маска
	        ACCESS LIST
	
	Изменяет или показывает список доступа Вашего ника. Имеется в виду
	список адресов, которые будут автоматически идентифицированы
	%S как адреса, с которых допустимо использовать Ваш ник. Если Вы
	соединяетесь с IRC с адресом из этого списка, Вы не попадете под действие
	опции Защита (SET KILL) и если опция Охрана (SECURE) выключена, Вы можете
	получать автоматически операторов каналов и другие привилегии на каналах
	без использования команды IDENTIFY.
	
	Маска, применяемая в командах ADD и DEL может содержать групповые символы (* или ?).
	
	Примеры:
	
	    ACCESS ADD achurch@*.dragonfire.net
	        Разрешает доступ пользователю achurch с любого компьютера
	        в домене dragonfire.net.
	
	    ACCESS DEL achurch@*.dragonfire.net
	        Антоним предыдущей команды.
	
	    ACCESS LIST
	        Покажет текущий список доступа.

NICK_HELP_SET
	Синтаксис: SET опция параметры
	
	Устанавливает различные опции ника.  Опция может быть:
	
	    PASSWORD   Установить пароль на ник
	    LANGUAGE   Установить язык, на котором сервисы будут
	                   посылать Вам сообщения
	    URL        Ассоциировать URL с Вашим ником
	    EMAIL      Ассоциировать E-mail с Вашим ником
	    INFO       Установить текст, который будет показываться при команде INFO
	    KILL       Включен или выключить тип защиты
	    SECURE     Включить или выключить режим охраны ника
	    PRIVATE    Предохранить Ваш ник от появления в
	                   /msg %S LIST
	    HIDE       Скрыть часть информации о Вас
	    TIMEZONE   Установить временную зону, которая будет использоваться при показе дат.
NICK_HELP_SET_OPTION_MAINNICK
	    MAINNICK   Установить ник, который будет показываться в списках доступов каналов
NICK_HELP_SET_END
	
	Прежде чем использовать эту команду, сперва Вы должны сначала идентифицироваться
	с Вашим паролем (/msg %S HELP IDENTIFY для более подробной
	информации).
	
	Напишите /msg %S HELP SET опция для более подробной информации
	по конкретной опции.

NICK_HELP_SET_PASSWORD
	Синтаксис: SET PASSWORD новый_пароль
	
	Изменяет пароль, который Вы используете для идентификации Вас как владельца ника.

NICK_HELP_SET_LANGUAGE
	Синтаксис: SET LANGUAGE номер
	
	Изменяет язык, на котором Сервисы будут посылать Вам
	сообщения (например, при ответе на посланную Вами команду).
	номер должен быть выбран из нижеследующего списка
	поддерживаемых языков:
	

NICK_HELP_SET_URL
	Синтаксис: SET URL адрес
	
	Ассоциирует URL с Вашим ником.  Этот URL
	будет показан, когда кто-либо запросит информацию
	о Вашем нике с помощью команды INFO.

NICK_HELP_SET_EMAIL
	Синтаксис: SET EMAIL адрес
	
	Ассоциирует адрес E-mail с Вашим ником. Этот адрес
	будет показан, когда кто-либо запросит информацию
	о Вашем нике с помощью команды INFO.

NICK_HELP_SET_INFO
	Синтаксис: SET INFO текст
	
	Ассоциирует введенный текст с Вашим ником. Этот адрес
	будет показан, когда кто-либо запросит информацию
	о Вашем нике с помощью команды INFO.

NICK_HELP_SET_KILL
	Синтаксис: SET KILL {ON | QUICK | IMMED | OFF}
	
	Включает или выключает автоматическую защиту для Вашего
	ника.  С включенной защитой, если другой пользователь
	пытается взять Ваш ник, ему будет дана одна минута для смены
	ника, в противном случае %S принудительно удалит его
	с IRC.
	
	Если Вы укажете QUICK, пользователю будет дано для смены
	ника только 20 секунд. Если укажете IMMED, пользователь будет
	удален немедленно, без предупреждения или шанса изменить свой ник;
	пожалуйста, не используйте эту опцию без необходимости.
	Также, администрация сети может отключить использование этой опции.

NICK_HELP_SET_SECURE
	Синтаксис: SET SECURE {ON | OFF}
	
	Включает или выключает охранные возможности %S для
	Вашего ника.  С включенной установкой SECURE Вы должны
	ввести свой пароль для того, чтобы быть идентифицированными
	как владелец ника, вне зависимости от того, есть Ваш адрес в
	списке доступа Вашего ника или нет.  Однако, если Ваш адрес
	есть в списке доступа Вашего ника, %S не будет
	применять к Вам функцию защиты, независимо от настроек опции
	KILL.
	
NICK_HELP_SET_PRIVATE
	Синтаксис: SET PRIVATE {ON | OFF}
	
	Включает или отключает опцию скрытия для Вашего ника на
	%S.  C включенной опцией PRIVATE Ваш ник не будет
	показываться в списке, который генерирует %S в ответ на
	команду LIST. (Однако, любой кто знает Ваш ник, сможет
	получить информацию о нем, используя команду INFO.)

NICK_HELP_SET_HIDE
	Синтаксис: SET HIDE {EMAIL | USERMASK | QUIT} {ON | OFF}
	
	Позволяет Вам запретить показ некоторой информации, когда
	кто-то запрашивает у %S команду INFO для Вашего ника.
	Вы можете скрыть свой E-mail адрес (EMAIL), последнюю маску
	user@host (USERMASK) и последнее сообщение при выходе (QUIT).
	Второй параметр определяет, будет информация показана (OFF) или скрыта (ON).

NICK_HELP_SET_TIMEZONE
	Синтаксис: SET TIMEZONE {UTC-смещение | временная-зона | DEFAULT}
	
	Устанавливает временную зону для отображения времени (таких как, например
	время последнего использования ника или канала), вместо того, чтобы использовать
	ту временную зону, с которой работают Сервисы; Настройка DEFAULT
	может быть использована, чтобы вернуться к временной зоне Сервисов.
	Возможны два пути назначения зоны:
	
	   - как UTC смещение, разница во времени между вашим временем и UTC
	     (Universal Coordinated Time, также известное как GMT, Greenwich Mean Time).
	     Смешение указывается как "+" или "-", чтобы указать смещение
	     относительно UTC (перед - восточнее, после - западнее), и следом идет
	     целое число часов или строка в форме "чч:мм".
	
	   - Как имя временной зоны, например как "GMT" или "EDT".
	     Не все имена временных зон поддерживаются Сервисами и некоторые государственные
	     временный зоны имеют такие же имена, поэтому, если вы получите ошибку или
	     время, которые показывают Сервисы неверное, используете вариант
	     со смещением относительно UTC.
	
	Учтите, что Сервисы не умеют автоматически переводить часы; Вы должны исправлять это сами.
	
	Примеры:
	
	    SET TIMEZONE -5
	        Установить временную зону на 5 часов после UTC.
	
	    SET TIMEZONE +3:30
	        Установить временную зону на 3 часа 30 минут перед UTC.
	
	    SET TIMEZONE JST
	        Установить временную зону в Japan Standard Time (UTC+9:00).

NICK_HELP_SET_MAINNICK
	Синтаксис: SET MAINNICK ник
	
	Когда у Вас есть несколько ников, связанных вместе, эта команда
	позволяет Вам выбрать один, который будет показываться в списках доступа каналов.
	Если позже Вы разъедините этот ник, тогда любой другой станет им автоматически.

NICK_HELP_UNSET
	Синтаксис: UNSET {URL | EMAIL | INFO}
	
	Позволяет Вам удалить URL (URL), E-mail адрес (EMAIL) или
	информационный текст (INFO) ассоциированные с Вашим ником.

NICK_HELP_UNSET_REQ_EMAIL
	Синтаксис: UNSET {URL | INFO}
	
	Позволяет Вам удалить URL (URL) или информационный текст (INFO)
	ассоциированные с Вашим ником.

NICK_HELP_RECOVER
	Синтаксис: RECOVER ник [пароль]
	
	Позволяет Вам вернуть Ваш ник, если его взял кто-то
	другой; т.е. сделать то, что %S делает автоматически,
	когда кто-то пытается использовать защищенный ник.
	
	При использовании данной команды, %S создает псевдопользователя
	с указанным ником. Это заставит IRC сервер отключить
	другого пользователя с данным ником. Этот псевдопользователь
	будет оставаться в онлайне в течении %s, чтобы гарантировать,
	что другой пользователь не сможет немедленно снова взять этот ник;
	через минуту Вы можете вернуть себе Ваш ник. Также, Вы можете использовать
	команду RELEASE (/msg %S HELP RELEASE) чтобы вернуть свой ник
	раньше.
	
	Для использования команды RECOVER Вы должны быть идентифицированы
	как владелец группы ника или Вам необходимо указать пароль для этого ника.

NICK_HELP_RELEASE
	Синтаксис: RELEASE ник [пароль]
	
	Приказывает %S удалить любое удержание Вашего ника в результате
	действий автоматической защиты или команды RECOVER.
	По умолчанию, удерживается в течении %s;
	эта команда освобождает ник довольно быстро.
	
	Для использования команды RELEASE Вы должны быть идентифицированы
	как владелец группы ника или Вам необходимо указать пароль для этого ника.

NICK_HELP_GHOST
	Синтаксис: GHOST ник [пароль]
	
	Принудительно завершает "зависший" сеанс в IRC, использующий
	Ваш ник.  "Зависший" сеанс -- это такой, который на самом
	деле не соединен с сервером, но сервер по каким-то причинам
	считает, что Вы онлайн. Обычно это случается при сбое
	компьютера или когда Ваше соединение с Интернет рвется
	в момент Вашего нахождения на IRC.
	
	Для использования команды GHOST Вы должны быть идентифицированы
	как владелец группы ника или Вам необходимо указать пароль для этого ника.

NICK_HELP_INFO
	Синтаксис: INFO ник [ALL]
	
	Показывает информацию об указанном нике, например такую как
	владельца ника, последний адрес в онлайне, опции ника. Если
	Вы идентифицировались как владелец этого ника и указан
	параметр ALL, Вам будет показана вся информация, независимо
	от того, скрыта она или нет.

NICK_HELP_INFO_AUTH
	
	Если E-mail адрес ника не был подтвержден командой AUTH, он не
	будет показан никому, включая владельца ника и администраторов Сервисов,
	независимо от состояния опции SET HIDE EMAIL.

NICK_HELP_LISTCHANS
	Синтаксис: LISTCHANS
	
	Показывает список зарегистрированных Вами каналов.

### CHANGED
#NICK_HELP_LIST
#	Синтаксис: LIST шаблон
#	
#	Показывает список всех зарегистрированных ников, у которых любой из связанных ников
#	или последний адрес (в формате user@host) совпадает с указанным шаблоном.
#	Ники, у которых включена опция PRIVATE, показаны не будут.
#	
#	Примеры:
#	
#	    LIST joeuser@foo.com
#	        Покажет все зарегистрированные ники, владельцы которых имеют или имели
#	        на момент выхода с IRC маску joeuser@foo.com.
#	
#	    LIST *Bot*
#	        Покажет все зарегистрированные ники со словом Bot в их нике (регистр неважен).
#	
#	    LIST *@*.bar.org
#	        Покажет все ники, принадлежащие пользователям из домена bar.org.

NICK_HELP_LIST_OPERSONLY
	
	Только IRC операторы могут использовать эту команду.

NICK_HELP_LISTEMAIL
	Синтаксис: %s
	
	Показывает список зарегистрированных ников также как и команда LIST, но
	поиск ведется по E-mail адресам.
	Для дополнительной информации смотрите помощь по команде LIST(/msg %S HELP LIST).

NICK_HELP_STATUS
	Синтаксис: STATUS ник...
	
	Возвращает информацию о данном нике; идентифицировался ли он как владелец ника.
	Ответ выводится в формате:
	
	    ник код-статуса
	
	где ник - это ник, который Вы указали при вводе команды,
	код-статуса может быть одним из:
	
	    0 - такого пользователя нет в онлайне или этот ник не зарегистрирован
	    1 - пользователь не идентифицирован как владелец ника
	    2 - пользователь идентифицирован как владелец ника только по списку доступа
	    3 - пользователь идентифицирован как владелец ника через идентификацию паролем
	
	До 16 ников сразу может быть запрошено этой командой;
	остальные будут проигнорированы. Сообщение об ошибке не будет генерироваться,
	если ник не отправлен в качестве параметра команды.

NICK_HELP_AJOIN
	Синтаксис: AJOIN ADD канал
	        AJOIN DEL канал
	        AJOIN LIST
	
	Изменяет или показывает список автозаходов для Вашего ника.
	Это список каналов, на которые Вы хотите заходить автоматически,
	приводится в действие всякий раз, когда Вы идентифицируетесь на %S.
	Вы можете добавлять только те имена каналов,
	которые начинаются с "#" (локальные каналы не могут быть добавлены).
	
NICK_HELP_AJOIN_END
	Обратите внимание, что IRC сервер может препятствовать Вам в посещении
	каналов в Вашем списке автозаходов, таких, как каналы с установленным
	паролем (режим +k) или каналы по приглашению (+i).
NICK_HELP_AJOIN_END_CHANSERV
	Обратите внимание, что IRC сервер может препятствовать Вам в посещении
	каналов в Вашем списке автозаходов, таких, как каналы с установленным
	паролем (режим +k) или каналы по приглашению (+i). Однако, если Вы
	имеете доступ к %s команде INVITE на зарегистрированном канале, %S
	автоматически пригласят Вас на канал.

### CHANGED
#NICK_OPER_HELP_COMMANDS
#	
#	Доступные команды для Администраторов Сервисов:
#	
#	    DROPNICK   удалить любой ник
#NICK_OPER_HELP_COMMANDS_DROPEMAIL
#	    DROPEMAIL  Удалить все ники, у которых совпадает E-mail адрес
#NICK_OPER_HELP_COMMANDS_GETPASS
#	    GETPASS    Возвращает пароль ника
#	                   (только если шифрование выключено)
#NICK_OPER_HELP_COMMANDS_FORBID
#	    FORBID     Запрещает использовать/регистрировать ник
#	    SUSPEND    Запрещает идентифицировать ник (приостановка)
#	    UNSUSPEND  Отмена приостановки
#NICK_OPER_HELP_COMMANDS_SETAUTH
#	    SETAUTH    Установить новый код аутентификации для ника
#	    GETAUTH    Показать код аутентификации для ника
#	    CLEARAUTH  Удалить код аутентификации для ника
NICK_OPER_HELP_COMMANDS_END
	
	Администраторы Сервисов также могут использовать следующие команды на любом нике
	без необходимости идентифицирования их:
	
	    SET, ACCESS LIST, INFO ALL
	
	Смотрите помощь по каждой из этих команд.

NICK_OPER_HELP_DROPNICK
	Синтаксис: DROPNICK ник
	
	Удаляет указанный и все связанные с ним ники из базы %S.
	
	Только для Администраторов Сервисов.

NICK_OPER_HELP_DROPEMAIL
	Синтаксис: DROPEMAIL шаблон | -
	        DROPEMAIL-CONFIRM шаблон | -
	
	Удаляет все ники, у которых E-mail совпадает с указанным шаблоном.
	Если шаблон равен символу "-", то удаляет все ники без E-mail адреса.
	
	Для предотвращения случайного удаления ников этой командой,
	она должна быть указана дважды: первый раз используя
	команду DROPEMAIL, потом командой DROPEMAIL-CONFIRM с тем же шаблоном.
	Команда DROPEMAIL-CONFIRM должна быть с шаблоном %s и выполнена тем же
	пользователем, который использовал команду DROPEMAIL.
	Если дано более 4 команд DROPEMAIL, то более старые команды будут
	отброшены и не будут определяться по команде DROPEMAIL-CONFIRM.
	
	Только для Администраторов Сервисов.

NICK_OPER_HELP_SET
	
	Администраторы Сервисов также могут установить опцию NOEXPIRE, с которой
	ники будут защищены от удаления при бездействии.
	Также, Администраторы Сервисов могут установить опции для любого
	ника без необходимости указывать пароль, используя формат:
	SET !ник опция параметры.

NICK_OPER_HELP_SET_NOEXPIRE
	Синтаксис: SET [!ник] NOEXPIRE {ON | OFF}
	
	Устанавливает особый режим для указанного ника - никогда не заканчиваться.
	В положении ON ник предохраняется от заканчивания. Если ник
	не указан, то не-заканчивание будет установлено на Ваш ник.
	
	Только для Администраторов Сервисов.

### CHANGED
#NICK_OPER_HELP_UNSET
#	
#	Администраторы Сервисов могут использовать команду UNSET на любом нике.
#	Добавляя ник после названия опции, также, как и в случае с командой SET.

NICK_OPER_HELP_OLD_UNLINK
	
	Администраторы Сервисов могут удалять связь любого ника без предоставления пароля.

NICK_OPER_HELP_UNLINK
	Синтаксис: UNLINK ник [FORCE]
	
	Удаляет связь (псевдоним) к Вашему нику, созданную командой LINK
	Вы не можете удалить связь с Вашим текущим ником; измените его на
	другой связанный ник, прежде чем использовать эту команду.
	
	Если Вы разрываете связь с "основным ником", который Вы установили
	командой SET MAINNICK, Ваш текущий ник автоматически становится главным ником.
	
	Администраторы Сервисов могут использовать команду FORCE, которая позволяет
	удалить связь ника, владельцами которых они не являются.

NICK_OPER_HELP_OLD_LISTLINKS
	Синтаксис: LISTLINKS ник
	
	Показать список всех ников, которые связаны с указанным ником.
	
	Только для Администраторов Сервисов.

# CHANGED: Services administrators -> Services operators
#NICK_OPER_HELP_LISTLINKS
#	Синтаксис: LISTLINKS [ник]
#	
#	Показать список всех ников, которые связаны с Вашим. Главный ник в группе
#	связанных ников будет помечен символом "*" слева.
#	Администраторы Сервисов могут получить список связей для любого ника, указав его в команде.

NICK_OPER_HELP_ACCESS
	
	Администраторы Сервисов могут указать ник в команде LIST, чтобы
	смотреть список доступа для любого ника.

NICK_OPER_HELP_INFO
	
	Администраторы Сервисов могут использовать параметр ALL с любым ником.

NICK_OPER_HELP_LISTCHANS
	
	Администраторы Сервисов могут смотреть список каналов для любого
	ника, указав этот ник с командой (например, /msg %S LISTCHANS другой_ник).

### CHANGED: NICK_OPER_HELP_LIST and NICK_OPER_HELP_LIST_AUTH changed, 
### NICK_OPER_HELP_LIST_END added
#NICK_OPER_HELP_LIST
#	Синтаксис: LIST шаблон [FORBIDDEN] [NOEXPIRE] [SUSPENDED]
#	
#	Показывает список всех зарегистрированных ников, у которых любой из связанных ников
#	или последний адрес (в формате user@host) совпадает с указанным шаблоном.
#	Ники, у которых включена опция PRIVATE, будут показаны только
#	для Администраторов Сервисов. Для Администраторов Сервисов у ников, для которых
#	установлена опция NOEXPIRE, будут иметь символ ! перед ником, а те, которые приостановлены - *.
#	
#	Если будут указаны опции FORBIDDEN, SUSPENDED или NOEXPIRE, то будут показаны
#	только те ники, которые запрещены, приостановлены или имеют опцию NOEXPIRE соответственно.
#	
#	Опции могут быть в любой последовательности и комбинации.
#	Эти опции только для Администраторов Сервисов.
#	
#	Примеры:
#	
#	    LIST joeuser@foo.com
#	        Покажет все зарегистрированные ники, владельцы которых имеют или имели
#	        на момент выхода с IRC маску joeuser@foo.com.
#	
#	    LIST *Bot*
#	        Покажет все зарегистрированные ники со словом Bot в их нике (регистр неважен).
#	
#	    LIST * NOEXPIRE
#	        Показать все зарегистрированные ники, у которых стоит опция NOEXPIRE.
#
#NICK_OPER_HELP_LIST_AUTH
#	Синтаксис: LIST шаблон [FORBIDDEN] [NOEXPIRE] [SUSPENDED] [NOAUTH]
#	
#	Показывает список всех зарегистрированных ников, у которых любой из связанных ников
#	или последний адрес (в формате user@host) совпадает с указанным шаблоном.
#	Ники, у которых включена опция PRIVATE, будут показаны только
#	для Администраторов Сервисов. Для Администраторов Сервисов у ников, для которых
#	установлена опция NOEXPIRE, будут иметь символ ! перед ником,
#	те, которые приостановлены - *, а те, которые еще не аутентифицированы - ?.
#	
#	Если будет предоставлены опции FORBIDDEN, SUSPENDED или NOEXPIRE, то будут показаны
#	только те ники, которые запрещены, приостановлены или имеют опцию NOEXPIRE соответственно.
#	
#	Опции могут быть в любой последовательности и комбинации.
#	Эти опции только для Администраторов Сервисов.
#	
#	Если будет предоставлены опции FORBIDDEN, SUSPENDED, NOEXPIRE или NOAUTH,
#	то будут показаны только те ники, которые запрещены, приостановлены,
#	имеют опцию NOEXPIRE или еще не аутентифицированы соответственно.
#	
#	Опции могут быть в любой последовательности и комбинации.
#	Эти опции только для Администраторов Сервисов.
#	
#	Примеры:
#	
#	    LIST joeuser@foo.com
#	        Покажет все зарегистрированные ники, владельцы которых имеют или имели
#	        на момент выхода с IRC маску joeuser@foo.com.
#	
#	    LIST *Bot*
#	        Покажет все зарегистрированные ники со словом Bot в их нике (регистр неважен).
#	
#	    LIST * NOEXPIRE
#	        Показать все зарегистрированные ники, у которых стоит опция NOEXPIRE.

NICK_OPER_HELP_GETPASS
	Синтаксис: GETPASS ник
	
	Возвращает пароль указанного ника. Учтите, что при
	использовании данной команды сообщение, включающее ник,
	вызвавший данную команду и ник, на который был запрошен пароль,
	будет записан в лог и отправлен в WALLOPS/GLOBOPS.
	
	В зависимости от метода шифрования паролей, команда может быть недоступна.
	
	Только для Администраторов Сервисов.

NICK_OPER_HELP_FORBID
	Синтаксис: FORBID ник
	
	Запрещает регистрировать или использовать ник любым пользователям.
	Может быть отменено путем удаления ника (DROP).
	
	Только для Администраторов Сервисов.

NICK_OPER_HELP_SUSPEND
	Синтаксис: SUSPEND [+время_завершение] ник причина
	
	Предотвращает использование/идентификацию ника и всех связанных
	с ним ников любому пользователю. Использование ника может быть возобновлено командой
	UNSUSPEND. Параметр время_завершениетакой же, как и в команде %s AKILL.
	
	Только для Администраторов Сервисов.

NICK_OPER_HELP_UNSUSPEND
	Синтаксис: UNSUSPEND ник
	
	Разрешает пользователям идентифицироваться и пользоваться ником, который
	ранее был приостановлен.
	
	Только для Администраторов Сервисов.

NICK_OPER_HELP_AJOIN
	
	Администраторы Сервисов могут просматривать списки автозаходов для любого
	пользователя используя формат LIST ник.

NICK_OPER_HELP_SETAUTH
	Синтаксис: SETAUTH ник
	
	Установить новый код аутентификации для указанного ника.
	Владелец ника должны использовать команду AUTH прежде чем идентифицироваться.
	Если ник используется, ему будет послано уведомление и
	пользователь перестанет иметь статус идентифицированного.
	
	Только для Администраторов Сервисов.

NICK_OPER_HELP_GETAUTH
	Синтаксис: GETAUTH ник
	
	Получить код аутентификации, ассоциированного с ником, если код существует.
	
	Только для Администраторов Сервисов.

NICK_OPER_HELP_CLEARAUTH
	Синтаксис: CLEARAUTH ник
	
	Очистить все коды аутентификации для ника, позволяя пользоваться ником как обычно.
	
	Только для Администраторов Сервисов.


###########################################################################
#
# ChanServ help messages
#
###########################################################################

# Used in OP/VOICE/etc for required access levels for commands
CHAN_HELP_REQSOP_LEVXOP
	уровень 100 (SOP)
CHAN_HELP_REQSOP_LEV
	уровень 100
CHAN_HELP_REQSOP_XOP
	SOP
CHAN_HELP_REQAOP_LEVXOP
	уровень 50 (AOP)
CHAN_HELP_REQAOP_LEV
	уровень 50
CHAN_HELP_REQAOP_XOP
	AOP
CHAN_HELP_REQHOP_LEVXOP
	уровень 40 (HOP)
CHAN_HELP_REQHOP_LEV
	уровень 40
CHAN_HELP_REQHOP_XOP
	HOP
CHAN_HELP_REQVOP_LEVXOP
	уровень 30 (VOP)
CHAN_HELP_REQVOP_LEV
	уровень 30
CHAN_HELP_REQVOP_XOP
	VOP

# Real help messages start here
CHAN_HELP
	%S позволяет Вам регистрировать и управлять различными
	аспектами каналов. %S может предотвратить "захват" канала
	злоумышленниками ограничением круга лиц, которым позволено
	иметь статус оператора на канале. Для получения списка команд
	наберите /msg %S HELP COMMANDS; для использования команды
	наберите /msg %S команда, для более подробной информации
	о нужной команде наберите: /msg %S HELP команда.
CHAN_HELP_EXPIRES
	
	Имейте ввиду, что регистрация любого канала, который не использовался
	в течение %s (т.е. пользователи из списка доступа канала не
	заходили на канал в течение этого периода времени) будет
	автоматически отменена.

CHAN_HELP_COMMANDS
	Доступные команды %S:
	
	    REGISTER   Зарегистрировать канал
	    IDENTIFY   Идентифицироваться с использованием пароля
	    DROP       Отменить регистрацию канала
	    SET        Установить опции канала и информацию о нем
	    UNSET      Очистка информации канала
	    INFO       Показать опции канала и информацию о нем
CHAN_HELP_COMMANDS_LIST
	    LIST       Показать список зарегистрированных каналов
CHAN_HELP_COMMANDS_AKICK
	    AKICK      Управление AKICK списком
### CHANGED: "access levels" -> "privilege levels"
#CHAN_HELP_COMMANDS_LEVELS
#	    ACCESS     Управление всем списком доступа канала
#	    LEVELS     Настройка уровней доступа канала
CHAN_HELP_COMMANDS_XOP
	    SOP        Управление списком SuperOp
	    AOP        Управление списком AutoOp
CHAN_HELP_COMMANDS_HOP
	    HOP        Управление списком AutoHalfop
### CHANGED: NOP command added
#CHAN_HELP_COMMANDS_XOP_2
#	    VOP        Управление списком AutoVoice
CHAN_HELP_COMMANDS_OPVOICE
	    OP         Дать пользователю статус оператора (+o)
	    DEOP       Удалить статус оператора (+o)
	    VOICE      Дать пользователю статус голоса (+v)
	    DEVOICE    Удалить статус голоса (+v)
CHAN_HELP_COMMANDS_HALFOP
	    HALFOP     Дать пользователю статус полуоператора (+h)
	    DEHALFOP   Удалить статус полуоператора (+h)
CHAN_HELP_COMMANDS_PROTECT
	    PROTECT    Дать пользователю статус защиты (+a)
	    DEPROTECT  Удалить статус защиты (+a)
CHAN_HELP_COMMANDS_INVITE
	    INVITE     Пригласить себя на канал
	    UNBAN      Снять бан с себя на канале
	    KICK       Удалить пользователя с канала
	    TOPIC      Изменить топик канала
	    CLEAR      очистить режимы канала или очистить всех пользователей
	    STATUS     Вернуть уровень доступа на канале для пользователя

### CHANGED: reference to ACCESS command removed
#CHAN_HELP_REGISTER
#	Синтаксис: REGISTER канал пароль описание
#	
#	Регистрирует канал в базе данных %S. Для того, чтобы
#	использовать эту команду, Вы должны иметь статус оператора
#	на канале, который хотите зарегистрировать.
#	Пароль указывается для использования в команде IDENTIFY, для
#	получения доступа к настройкам канала в будущем.
#	Последний параметр, который должен быть указан, является общим
#	описанием предназначения канала.
#	
#	Когда Вы регистрируете канал, Вы запоминаетесь как
#	"владелец" канала. Владелец канала может изменять все
#	настройки канала; %S также будет автоматически давать
#	статус оператора при его/ее заходе на канал.
#	Смотрите описание команды ACCESS (/%S HELP ACCESS)
#	для получения информации о том, как назначать
#	подмножества этих привилегий остальным пользователям канала.
#	
#	ВНИМАНИЕ: Для того, чтобы зарегистрировать канал, Вы должны
#	сначала зарегистрировать свой ник. Если Вы еще этого не
#	сделали, введите /msg %s HELP для получения
#	информации о регистрации ника.
CHAN_HELP_REGISTER_ADMINONLY
	
	Только для Администраторов Сервисов.

CHAN_HELP_IDENTIFY
	Синтаксис: IDENTIFY канал пароль
	
	Идентифицирует Вас на %S как владельца указанного канала.
	Многие команды для канала недоступны, пока Вы не
	идентифицируетесь. Пароль должен быть тем паролем, который
	Вы указывали, когда использовали команду REGISTER.

### CHANGED
#CHAN_HELP_DROP
#	Синтаксис: DROP канал
#	
#	Отменяет регистрацию указанного канала. Это может сделать
#	только владелец канала, который перед этим должен
#	идентифицироваться командой IDENTIFY.

CHAN_HELP_SET
	Синтаксис: SET канал опция параметры
	
	Позволяет владельцу канала устанавливать различные опции
	для канала, а также другую информацию. Владелец должен
	идентифицироваться командой IDENTIFY перед использованием
	SET.
	
	Доступные опции:
	
	    FOUNDER     Установить владельца канала
	    SUCCESSOR   Установить наследника канала
	    PASSWORD    Установить пароль владельца
	    DESC        Установить описание канала
	    URL         Ассоциировать с каналом URL
	    EMAIL       Ассоциировать с каналом E-mail адрес
	    ENTRYMSG    Установить сообщение, отправляемое
	                   пользователям при заходе на канал
	    KEEPTOPIC   Хранить топик, когда все покинули канал
	    TOPICLOCK   Блокировка топика на канале
	    MLOCK       Блокировка режимов канала
	    PRIVATE     Не показывать канал по команде LIST
	    RESTRICTED  Ограничить доступ на канал
	    SECURE      Включить охранные функции %S для канала
	    SECUREOPS   Строгий контроль статуса оператора
	    LEAVEOPS    Не забирать статус оператора
	    OPNOTICE    Уведомлять об использовании команд OP/DEOP
	    ENFORCE     Всегда устанавливать статус оператора и голоса
	
	Введите /msg %S HELP SET опция для получения дальнейшей
	информации о конкретной опции.

CHAN_HELP_SET_FOUNDER
	Синтаксис: SET канал FOUNDER ник
	
	Изменяет владельца канала. Ник нового владельца должен
	быть зарегистрированным.

CHAN_HELP_SET_SUCCESSOR
	Синтаксис: SET канал SUCCESSOR ник
	
	Установить наследника канала. Если ник владельца истечет
	или будет удален, пока канал существует, наследник
	станет новым владельцем канала. Тем не менее, если
	наследник уже зарегистрировал слишком много каналов (%d),
	канал будет удален, также, как если бы наследник не был указан.
	Ник наследника канала должен быть зарегистрированным.

CHAN_HELP_SET_PASSWORD
	Синтаксис: SET канал PASSWORD пароль
	
	Установить пароль, используемый для идентификации
	на канале как его владелец.

CHAN_HELP_SET_DESC
	Синтаксис: SET канал DESC описание
	
	Устанавливает описание канала, которое будет показано при использовании
	команд LIST и INFO.

CHAN_HELP_SET_URL
	Синтаксис: SET канал URL url
	
	Ассоциировать указанный URL с каналом. Этот URL
	будет показан, когда кто-либо запросит информацию о канале
	с помощью команды INFO.

CHAN_HELP_SET_EMAIL
	Синтаксис: SET канал EMAIL адрес
	
	Ассоциировать указанный E-mail адрес с каналом. Этот адрес
	будет показан, когда кто-либо запросит информацию о канале
	с помощью команды INFO.

CHAN_HELP_SET_ENTRYMSG
	Синтаксис: SET канал ENTRYMSG сообщение
	
	Установить сообщение, которое будет отправлено уведомлением (/NOTICE)
	каждому вошедшему на канал пользователю.

CHAN_HELP_SET_KEEPTOPIC
	Синтаксис: SET канал KEEPTOPIC {ON | OFF}
	
	Включает или выключает опцию хранения топика на
	канале. Когда хранение топика включено, топик канала
	будет запомнен %S даже после того, как последний
	пользователь покинет канал, и будет восстановлен
	как только кто-либо снова зайдет.

CHAN_HELP_SET_TOPICLOCK
	Синтаксис: SET канал TOPICLOCK {ON | OFF}
	
	Включает или выключает опцию блокировки топика на канале.
	Когда блокирование топика включено, %S не разрешает менять
	топик канала кроме как через команду TOPIC.

CHAN_HELP_SET_MLOCK
	Синтаксис: SET канал MLOCK режимы
	
	Устанавливает и блокирует определенные режимы канала. %S
	позволяет Вам установить режимы канала, которые должны быть
	всегда включенными (или выключенными). Все логические режимы
	(n, s, t, k, l и т.д.) могут быть заблокированы;
	каждый режим может быть заблокирован в состоянии включено,
	заблокирован в состоянии выключено или не заблокирован.
	Параметр режимы в принципе напоминает команду
	/MODE; режимы, начинающиеся с + блокируются в
	состоянии включено, а режимы, начинающиеся с - в состоянии выключено.
	
	Внимание: Если Вы установили и заблокировали ключ канала, как во втором
	примере ниже, Вы должны также установить опцию RESTRICTED
	на канал (см. HELP SET RESTRICTED), или любой, кто зайдет на канал
	когда тот будет пуст сможет увидеть ключ!
	Блокирование режима +i предотвращает заход на канал без приглашения когда он пуст.
	
	Примеры:
	
	    SET #канал MLOCK +nt-ikl
	        Блокирует включенными режимы n и t, и выключенными i, k и l.
	        Все остальные режимы могут устанавливаться/сниматься свободно.
	
	    SET #канал MLOCK +knst-ilmp мой-ключ
	        Блокирует включенными режимы k, n, s, и t, и выключенными i, l, m, и p.
	        Также устанавливает ключ для входа на канал "мой-ключ".
	
	    SET #канал MLOCK +
	        Удаляет блокировку режимов; Все режимы каналов свободно могут быть
	        изменены.

CHAN_HELP_SET_HIDE
	Синтаксис: SET канал HIDE {EMAIL | TOPIC | MLOCK} {ON | OFF}
	
	Позволяет Вам запретить показ некоторой информации, когда кто-то
	запрашивает у %S команду INFO для указанного канала.
	Вы можете скрыть E-mail адрес канала (EMAIL), последний топик (TOPIC)
	и блокированные режимы (MLOCK).
	Второй параметр определяет, будет информация показана (OFF) или скрыта (ON).
	
	Учтите, что топик всегда будет скрыт, если установлена
	блокировка режимов +p или +s.

CHAN_HELP_SET_PRIVATE
	Синтаксис: SET канал PRIVATE {ON | OFF}
	
	Включает или выключает опцию секретности на канале.
	Когда секретности включена, использование /msg %S LIST не покажет
	данный канал в списке.

CHAN_HELP_SET_RESTRICTED
	Синтаксис: SET канал RESTRICTED {ON | OFF}
	
	Включает или выключает ограниченный доступ на
	канал. Когда ограниченный доступ включен, пользователи,
	которые не в списке доступа или которые не имеют прав на получение
	статуса оператора, будут немедленно забанены и выкинуты с канала.

CHAN_HELP_SET_SECURE
	Синтаксис: SET канал SECURE {ON | OFF}
	
	Включает или выключает опции безопасности %S для
	канала. Когда SECURE включено, только пользователи
	с зарегистрированными и проидентифицировавшимися на %s никами
	могут получить права на канале в соответствии со списками доступа.
	Если также установлена опция RESTRICTED, то зайти на канал
	могут только проидентифицированные на %s пользователи.

CHAN_HELP_SET_SECUREOPS
	Синтаксис: SET канал SECUREOPS {ON | OFF}
	
	Включает или выключает безопасность статуса оператора канала.
	Когда безопасность статуса оператора включена, пользователи
	не из списка доступа не могут получить статус оператора на канале.

CHAN_HELP_SET_LEAVEOPS
	Синтаксис: SET канал LEAVEOPS {ON | OFF}
	
	Включает или выключает опцию оставлять оператора канала.
	Когда опция оставлять оператора включена, %S оставит статус
	серверного оператора (+o пользователю, который первый зашел на канал)
	даже если пользователь не может нормально получить оператора.
	Опция SECUREOPS будет по прежнему принудительно применяться.
	
	Уведомление:  Это может позволять пользователям "захватывать"
	статусы операторов при нетсплитах.

CHAN_HELP_SET_OPNOTICE
	Синтаксис: SET канал OPNOTICE {ON | OFF}
	
	Включает или выключает оп-уведомление на канале.
	Когда оп-уведомление включено, %S будет посылать уведомление
	на канал когда кто-либо использует команду OP, VOICE
	или подобные им на канале.

CHAN_HELP_SET_ENFORCE
	Синтаксис: SET канал ENFORCE {ON | OFF}
	
	Включает или выключает принуждение на канале.
	Когда принуждение включено, %S будет принудительно
	давать статусы авто-оператора и авто-голоса для всех пользователей.
	К примеру, если пользователю с правом авто-оператора
	сняли оператора, %S автоматически даст оператора опять. %S
	также не дает использовать команду DEOP на таком пользователе.

CHAN_HELP_UNSET
	Синтаксис: UNSET канал {SUCCESSOR | URL | EMAIL | ENTRYMSG}
	
	Удаляет указанную настройку для канале.

### CHANGED: LIST syntax changed
#CHAN_HELP_SOP
#	Синтаксис: SOP канал ADD ник
#	        SOP канал DEL ник
#	        SOP канал LIST [маска | список]
#	        SOP канал COUNT
#	
CHAN_HELP_SOP_MID1
	Поддерживает список СуперОператоров (SOP) канала. Пользователи
	со статусом SOP автоматически получают статус оператора (режим +o)
	когда они заходят на канал.
CHAN_HELP_SOP_MID1_CHANPROT
	Поддерживает список СуперОператоров (SOP) канала. Пользователи
	со статусом SOP автоматически получают статус оператора (режим +o)
	и защиту (режим +a) когда они заходят на канал.
### CHANGED: NOP list added
#CHAN_HELP_SOP_MID2
#	
#	SOP'ы могут изменять списки AOP'ов и VOP'ов. Только владелец канала может изменять список SOP'ов.
#CHAN_HELP_SOP_MID2_HALFOP
#	
#	SOP'ы могут изменять списки AOP'ов, HOP'ов и VOP'ов. Только владелец канала может изменять список SOP'ов.
### CHANGED
#CHAN_HELP_SOP_END
#	
#	Команда SOP LIST показывает список SOP'ов. Если дана маска,
#	то будут показаны только те записи, которые совпадают с ней.
#	Если указан список номеров записей, только эти записи будут показаны; например:
#	
#	   SOP #канал LIST 2-5,7-9
#	      Покажет SOP'ов, записи со 2 до 5 и с
#	      7 до 9.
#	
#	Команда SOP COUNT возвращает количество записей в SOP списке.

### CHANGED: LIST syntax changed
#CHAN_HELP_AOP
#	Синтаксис: AOP канал ADD ник
#	        AOP канал DEL ник
#	        AOP канал LIST [маска | список]
#	        AOP канал COUNT
#	
#	Поддерживает список АвтоОператоров (AOP) канала. Пользователи
#	со статусом AOP автоматически получают статус оператора (режим +o)
#	когда они заходят на канал.
### CHANGED: NOP added
#CHAN_HELP_AOP_MID
#	
#	AOP'ы могут изменять список VOP'ов. SOP'ы могут изменять список AOP'ов.
#CHAN_HELP_AOP_MID_HALFOP
#	
#	AOP'ы могут изменять списки HOP'ов и VOP'ов. SOP'ы могут изменять список AOP'ов.
### CHANGED
#CHAN_HELP_AOP_END
#	
#	Команда AOP LIST показывает список AOP'ов. Если дана маска,
#	то будут показаны только те записи, которые совпадают с ней.
#	Если указан список номеров записей, только эти записи будут показаны; например:
#	
#	   AOP #канал LIST 2-5,7-9
#	      Покажет AOP'ов, записи со 2 до 5 и с
#	      7 до 9.
#	
#	Команда AOP COUNT возвращает количество записей в AOP списке.

### CHANGED
#CHAN_HELP_HOP
#	Синтаксис: HOP канал ADD ник
#	        HOP канал DEL ник
#	        HOP канал LIST [маска | список]
#	        HOP канал COUNT
#	
#	Поддерживает список ПолуОператоров (HOP) канала. Пользователи
#	со статусом HOP автоматически получают статус полуоператора (режим +h)
#	когда они заходят на канал.
#	
#	HOP'ы могут изменять список VOP'ов. SOP'ы и AOP'ы могут изменять список HOP'ов.
#	
#	Команда HOP LIST показывает список HOP'ов. Если дана маска,
#	то будут показаны только те записи, которые совпадают в ней.
#	Если указан список номеров записей, только эти записи будут показаны; например:
#	
#	   HOP #канал LIST 2-5,7-9
#	      Покажет HOP'ов, записи со 2 до 5 и с
#	      7 до 9.
#	
#	Команда HOP COUNT возвращает количество записей в HOP списке.

### CHANGED
#CHAN_HELP_VOP
#	Синтаксис: VOP канал ADD ник
#	        VOP канал DEL ник
#	        VOP канал LIST [маска | список]
#	        VOP канал COUNT
#	
#	Поддерживает список Войсов (Голосов) (VOP) канала. Пользователи
#	со статусом VOP автоматически получают статус голоса (режим +v)
#	когда они заходят на канал.
#	
#	Команда VOP LIST показывает список VOP'ов. Если дана маска,
#	то будут показаны только те записи, которые совпадают с ней.
#	Если указан список номеров записей, только эти записи будут показаны; например:
#	
#	   VOP #канал LIST 2-5,7-9
#	      Покажет VOP'ов, записи со 2 до 5 и с
#	      7 до 9.
#	
#	Команда VOP COUNT возвращает количество записей в VOP списке.

### CHANGED: CHAN_HELP_NOP added

### CHANGED
#CHAN_HELP_ACCESS
#	Синтаксис: ACCESS канал ADD ник уровень
#	        ACCESS канал DEL {ник | номер записи | список}
#	        ACCESS канал LIST [маска | список]
#	        ACCESS канал COUNT
#	
#	Управляет списком доступа канала. Список доступа
#	определяет, какие пользователи могут быть операторами канала,
#	иметь доступ к командам %S на канал. Различные уровни доступа
#	разрешают доступ к различным настройкам канала и определяют пользовательские
#	привилегии; /msg %S HELP ACCESS LEVELS для получения более
#	подробной информации. Любой, не включенный в список доступа пользователь,
#	считается пользователем с уровнем доступа 0.
#	
#	Команда ACCESS ADD добавляет указанный ник в
#	список доступа с указанным уровнем; если данный ник уже
#	уже есть в списке, его уровень будет изменен на указанный
#	в этой команде. Указанный уровень должен быть меньше,
#	чем уровень пользователя, вызвавшего это команду, и
#	если ник уже есть в списке доступа, текущий
#	уровень доступа данного ника должен быть меньше, чем уровень доступа
#	пользователя, вызвавшего эту команду.
#	
#	Команда ACCESS DEL удаляет указанный ник из
#	списка доступа. Если указаны номера записей, эти записи
#	будут удалены. (см. пример использования команды LIST ниже.)
#	
#	Команда ACCESS LIST показывает список доступа. Если
#	указать маску, то только совпадающие с ней значения
#	будут показаны. Если указать номера записей, то только
#	эти записи будет показаны; например:
#	
#	   ACCESS #канал LIST 2-5,7-9
#	   Покажет записи в списке доступа со 2 по 5 и с
#	   7 по 9.
#	
#	Команда ACCESS COUNT возвращает количество записей в списке доступа.
### CHANGED: NOP added
#CHAN_HELP_ACCESS_XOP
#	
#	Помните, что список доступа можно изменять SOP, AOP и VOP командами,
#	которые отвечают за уровни %d, %d и %d соответственно.
#CHAN_HELP_ACCESS_XOP_HALFOP
#	
#	Помните, что список доступа можно изменять SOP, AOP, HOP и VOP командами,
#	которые отвечают за уровни %d, %d, %d и %d соответственно.

### CHANGED: "access levels" -> "privilege levels"
#CHAN_HELP_ACCESS_LEVELS
#	Уровни доступа пользователей
#	
#	По умолчанию, определены нижеследующие значения уровней доступа:
#	
#	   Founder   Полный доступ к функциям %S; автоматическое получение
#	                 статуса оператора при входе на канал. Помните,
#	                 что только один человек может иметь статус
#	                 владельца (и это никак нельзя изменить командой ACCESS).
#	      %4d   Доступ к команде AKICK; автооп.
#	      %4d   Автооп.
CHAN_HELP_ACCESS_LEVELS_HALFOP
	      %4d   Автополуоп.
CHAN_HELP_ACCESS_LEVELS_END
	      %4d   Автовойс.
	         0   Никаких особых привилегий; может получить оператора другим
	                 оператором (только если опция SECUREOPS выключена).
	        -1   Не может получить оператора на канале.
	      -100   Не может зайти на канал.
	
	Эти уровни (включая последние два) могут быть изменены или добавлены новые, используя
	команду LEVELS; напишите /msg %S HELP LEVELS для более полной информации.

CHAN_HELP_LEVELS
	Синтаксис: LEVELS канал SET тип уровень
	        LEVELS канал {DIS | DISABLE} тип
	        LEVELS канал LIST
	        LEVELS канал RESET
	
	Команда LEVELS позволяет удобно управлять уровнями
	доступа, используемыми командой ACCESS. Используя эту
	команду, Вы можете определить уровень доступа, требуемый для большинства
	функций %S'а. (Команды SET FOUNDER и SET PASSWORD,
	также как и эту команда, может использовать только
	владелец канала.)
### CHANGED: NOP added
#CHAN_HELP_LEVELS_XOP
#	
#	Внимание:  Эта команда только для опытных пользователей!
#	Изменение настроек уровней этой командой может привести к тому
#	что команды SOP, AOP и VOP перестанут правильно работать.
#CHAN_HELP_LEVELS_XOP_HOP
#	
#	Внимание:  Эта команда только для опытных пользователей!
#	Изменение настроек уровней этой командой может привести к тому
#	что команды SOP, AOP, HOP и VOP перестанут правильно работать.
CHAN_HELP_LEVELS_END
	
	LEVELS SET устанавливает уровень доступа для функции или группы функций.
	LEVELS DISABLE (или DIS) отключает функцию или запрещает доступ
	к функции всем, кроме владельца канала. LEVELS LIST покажет текущие
	уровни доступа для каждой функции или группы функций.
	LEVELS RESET сбросит все настройки уровней доступа на
	настройки по умолчанию (см. HELP ACCESS LEVELS).
	
	Чтобы посмотреть список функций, на которые можно определять уровни доступа,
	см. HELP LEVELS DESC.

CHAN_HELP_LEVELS_DESC
	Следующие возможности/функции принимаются:

### CHANGED
#CHAN_HELP_AKICK
#	Синтаксис: AKICK канал ADD маска [причина]
#	        AKICK канал DEL маска
#	        AKICK канал LIST [маска | список]
#	        AKICK канал VIEW [маска | список]
#	        AKICK канал ENFORCE
#	        AKICK канал COUNT
#	
#	Поддерживает список автокиков канала. Если пользователь,
#	включенный в список автокиков попытается зайти на канал,
#	%S забанит его на канале, а затем выкинет его.
#	
#	Команда AKICK ADD добавляет user@host или nick!user@host маску
#	в список автокиков. Если указана причина, то она будет использована,
#	когда пользователь будет кикнут; если причина не указана,
#	будет использована причина по умолчанию: "You have been banned from the channel".
#	
#	Команда AKICK DEL удаляет указанную маску из списка автокиков.
#	Тем не менее, это не удалит баны с канала; они должны быть удалены
#	вручную.
#	
#	Команда AKICK LIST показывает текущий список автокиков, или
#	опционально показывает только те записи, которые совпадают с
#	указанной маской. Команда AKICK VIEW аналогична, но дает
#	больше информации.
#	
#	Команда AKICK ENFORCE заставляет %S немедленно проверить AKICK
#	список и удалить с канала пользователей, включенных в него.
#	
#	Команда AKICK COUNT возвращает количество записей в AKICK списке.
#	
#	По умолчанию, команда доступна пользователям с доступом %s на канале.

CHAN_HELP_INFO
	Синтаксис: INFO канал [ALL]
	
	Показывает информацию об указанном зарегистрированном канале,
	включая его владельца, время регистрации, когда был последний
	раз использован, описание и заблокированные режимы, если они есть.
	Если Вы идентифицированы как владелец канала, то используя параметр
	ALL также будет показано сообщение при входа и преемник.

### CHANGED
#CHAN_HELP_LIST
#	Синтаксис: LIST шаблон
#	
#	Покажет все зарегистрированные каналы по указанному шаблону.
#	(Каналы с включенной опцией PRIVATE не будут показаны.)
CHAN_HELP_LIST_OPERSONLY
	
	Только IRC Операторы могут использовать эту команду.

CHAN_HELP_OP
	Синтаксис: OP канал [ник]
	
	Дает статус оператора канала указанному нику на указанном канале.
	Если ник не указан, то дает статус оператора канала Вам.
	По умолчанию, доступна пользователям с уровнем доступа на канале, равным %s или выше.
	
CHAN_HELP_DEOP
	Синтаксис: DEOP канал [ник]
	
	Снимает статус оператора канала с указанного ника на указанном канале.
	Если ник не указан, снимает статус оператора с Вас.
	По умолчанию, доступна пользователям с уровнем доступа на канале равным %s или выше.

CHAN_HELP_VOICE
	Синтаксис: VOICE канал [ник]
	
	Дает голос(войс) указанному нику на указанном канале. Если ник не указан, то дает войс Вам.
	По умолчанию, доступна пользователям с уровнем доступа на канале, равным %s или выше.

CHAN_HELP_DEVOICE
	Синтаксис: DEVOICE канал [ник]
	
	Снимает войс с указанного ника на указанном канале. Если ник не указан,
	снимет войса с Вас. По умолчанию, доступна пользователям
	с уровнем доступа на канале равным %s или выше.

CHAN_HELP_HALFOP
	Синтаксис: HALFOP канал [ник]
	
	Дает статус полуоператора канала указанному нику на указанном канале.
	Если ник не указан, то дает статус полуоператора канала Вам.
	По умолчанию, доступна пользователям с уровнем доступа на канале, равным %s или выше.
	
CHAN_HELP_DEHALFOP
	Синтаксис: DEHALFOP канал [ник]
	
	Снимает статус полуоператора с указанного ника на указанном канале. Если ник не указан,
	снимет статус полуоператора с Вас. По умолчанию, доступна пользователям
	с уровнем доступа на канале равным %s или выше.

CHAN_HELP_PROTECT
	Синтаксис: PROTECT канал [ник]
	
	Ставит защиту (режим +a) на указанный ник на указанном канале. Если ник не указан,
	ставит защиту на Вас. По умолчанию, доступна пользователям
	с уровнем доступа на канале равным %s или выше.

CHAN_HELP_DEPROTECT
	Синтаксис: DEPROTECT канал [канал]
	
	Снимает защиту (режим +a) с указанного ника на указанном канале.
	Если ник не указан, снимает защиту с Вас.
	По умолчанию, доступна пользователям с уровнем доступа на канале, равным %s или выше.

CHAN_HELP_INVITE
	Синтаксис: INVITE канал
	
	Заставляет %S пригласить Вас на указанный канал.
	По умолчанию, доступна пользователям с уровнем доступа на канале, равным %s или выше.
	
	Учтите, что если Вы получите сообщение о том, что канал не используется, это
	означает, что канал пуст и Вы можете зайти на него не используя эту команду.

CHAN_HELP_UNBAN
	Синтаксис: UNBAN канал
	
	Заставляет %S удалить с указанного канала все баны, не позволяющие
	Вам зайти на него. По умолчанию, доступна пользователям с уровнем
	доступа на канале равным %s или выше.

CHAN_HELP_KICK
	Синтаксис: KICK канал ник [причина]
	
	Заставляет %S выкинуть указанного пользователя с указанного канала.
	Если не указана причина, то в сообщение будет вставлен ник того,
	кто использовал команду, в противном случае использует указанную причину.
	По умолчанию доступна пользователям с уровнем доступа на канале равным %s или выше.

CHAN_HELP_KICK_PROTECTED
	
	Помните, что защищенные (+a) пользователи не могут быть выкинуты с помощью
	этой команды, даже если Вы владелец канала.

CHAN_HELP_TOPIC
	Синтаксис: TOPIC канал топик
	
	Заставляет %S установить топик канала. Эта команда наиболее
	полезна в паре с командой SET TOPICLOCK. По умолчанию, доступна
	пользователям с уровнем доступа на канале равным %s или выше.

CHAN_HELP_CLEAR
	Синтаксис: CLEAR канал что
	
	Заставляет %S очистить некоторые настройки на канале. Что может быть одним из приведенных:
	
	    MODES       Сбросить все режимы канала, включая те, которые заблокированы.
	    BANS        Удалить все баны на канале.
CHAN_HELP_CLEAR_EXCEPTIONS
	    EXCEPTIONS  Удалить все исключения из банов на канале.
CHAN_HELP_CLEAR_INVITES
	    INVITES     Удалить все маски приглашений на канале.
CHAN_HELP_CLEAR_MID
	    OPS         Снять статус оператора канала (режим +o) со всех пользователей.
CHAN_HELP_CLEAR_HALFOPS
	    HALFOPS     Снять статус полуоператора канала (режим +h) со всех пользователей.
CHAN_HELP_CLEAR_END
	    VOICES      Снять статус войса канала (режим +v) со всех пользователей.
	    USERS       Удалить (выкинуть) всех пользователей с канала.
	
	По умолчанию, доступна пользователям с уровнем доступа на канале равным %s или выше.

CHAN_HELP_STATUS
	Синтаксис: STATUS канал ник
	
	Возвращает текущий уровень доступа указанного ника
	на указанном канале. Ответ в форме:
	
	    STATUS канал ник доступ
	
	Если произошла ошибка, то форма ответа будет таким:
	
	    STATUS канал ник ERROR ошибка
	
	По умолчанию, доступна пользователям с уровнем доступа на канале равным %s или выше.

### CHANGED: DROPCHAN command added
#CHAN_OPER_HELP_COMMANDS
#	
#	Следующие команды доступны Администраторам Сервисов:
#	
CHAN_OPER_HELP_COMMANDS_GETPASS
	    GETPASS    Получить пароль владельца для канала
	                   (только если шифрование отключено)
CHAN_OPER_HELP_COMMANDS_FORBID
	    FORBID     Запрещает использовать/регистрировать канал
	    SUSPEND    Запрещает использовать зарегистрированный канал
	    UNSUSPEND  Снимает запрет на использование
### CHANGED
#CHAN_OPER_HELP_COMMANDS_END
#	
#	Администраторы Сервисов также могут удалять любой канал из базы без
#	необходимости идентифицироваться с паролем, а также могут просматривать
#	списки доступа, AKICK и уровней или использовать команду STATUS на
#	любой канал.

### CHANGED: CHAN_OPER_HELP_DROPCHAN added

CHAN_OPER_HELP_SET
	
	Администраторы Сервисов также могут установить опцию NOEXPIRE, с которой
	каналы будут защищены от удаления при бездействии.
	Также, Администраторы Сервисов могут установить опции для любого
	канала без необходимости указывать пароль канала.

CHAN_OPER_HELP_SET_NOEXPIRE
	Синтаксис: SET канал NOEXPIRE {ON | OFF}
	
	Устанавливает особый режим для указанного канала - никогда не заканчиваться.
	В положении ON канал предохраняется от заканчивания.
	
	Только для Администраторов Сервисов.

CHAN_OPER_HELP_UNSET
	
	Также как и с командой SET, Администраторы Сервисов могут удалять
	настройки на любом канале без необходимости идентификации как владелец этих каналов.

CHAN_OPER_HELP_INFO
	
	Администраторы Сервисов могут использовать параметр ALL для любого канала.

### CHANGED: "start" parameter added
#CHAN_OPER_HELP_LIST
#	Синтаксис: LIST канал [FORBIDDEN] [NOEXPIRE] [SUSPENDED]
#	
#	Показывает список имен всех зарегистрированных каналов, которые
#	совпали с шаблоном. Каналы с включенной опцией PRIVATE будут показаны
#	только Администраторам Сервисов. Каналы с установленной опцией NOEXPIRE
#	будут показаны для Администраторов Сервисов с символом ! перед именем.
#	
#	Если будут указаны опции FORBIDDEN, SUSPENDED или NOEXPIRE,
#	то будут показаны только те каналы, которые запрещены, приостановлены
#	или имеют опцию NOEXPIRE соответственно. Опции могут быть в любой
#	последовательности и комбинации.
#	Эти опции доступны только для Администраторов Сервисов.

CHAN_OPER_HELP_GETPASS
	Синтаксис: GETPASS канал
	
	Возвращает пароль для указанного канала. Учтите, что при
	использовании данной команды, сообщение, включающее ник,
	вызвавший данную команду и канал, на который был запрошен пароль,
	будет записан в лог и отправлен в WALLOPS/GLOBOPS.
	
	В зависимости от метода шифрования паролей, команда может быть недоступна.
	
	Только для Администраторов Сервисов.

CHAN_OPER_HELP_FORBID
	Синтаксис: FORBID канал
	
	Запрещает регистрировать или использовать канал любым пользователям.
	Может быть отменено путем удаления канала.
	
	Только для Администраторов Сервисов.

CHAN_OPER_HELP_SUSPEND
	Синтаксис: SUSPEND [+время_завершение] канал причина
	
	Предотвращает использование/идентификацию канала любому пользователю.
	Использование канала может быть возобновлено командой UNSUSPEND.
	Параметр время_завершениетакой же, как и в команде %s AKILL.
	
	Только для Администраторов Сервисов.

CHAN_OPER_HELP_UNSUSPEND
	Синтаксис: UNSUSPEND канал
	
	Разрешает пользователям идентифицироваться и пользоваться каналом, который
	ранее был приостановлен.
	
	Только для Администраторов Сервисов.


###########################################################################
#
# MemoServ help messages
#
###########################################################################

MEMO_HELP
	
	%S дает возможность пользователям IRC оставлять
	короткие сообщение (в дальнейшем - записки) другим
	пользователям, которые могут быть или не быть в данный
	момент в онлайне, или на каналы(*); Ник отправителя
	и ник или канал получателя должны быть зарегистрированы
	для возможности отправления записки. Для получения списка команд
	наберите /msg %S HELP COMMANDS; для использования команды наберите
	/msg %S команда, для более подробной информации о нужной команде
	наберите: /msg %S HELP команда.

MEMO_HELP_EXPIRES
	
	Уведомление:  Записки истекают (автоматически удаляются) по истечении
	%s после того, как были посланы, если не были помечены как
	неистекающие командой SAVE.
### CHANGED
#MEMO_HELP_END_LEVELS
#	
#	(*) По умолчанию, любой пользователь как минимум с уровнем доступа %s на
#	    канале может читать записки канала.  Это может быть
#	    изменено с использованием команды %s LEVELS.
#MEMO_HELP_END_XOP
#	
#	(*) Любой пользователь из списка SOP'ов может читать записка для канала.

MEMO_HELP_COMMANDS
	Доступные команды %S:
	
	    SEND      Отправить записку для ника или канала
	    LIST      Смотреть список своих записок
	    READ      Прочитать одну или несколько записок
MEMO_HELP_COMMANDS_FORWARD
	    FORWARD   Переправить записки на Ваш E-mail адрес
MEMO_HELP_COMMANDS_SAVE
	    SAVE      Пометить записки как неистекающие
### CHANGED: RENUMBER command added
#MEMO_HELP_COMMANDS_DEL
#	    DEL       Удалить одну или несколько записок
#	    SET       Установить опции записок
#	    INFO      Показать информацию о записках
MEMO_HELP_COMMANDS_IGNORE
	    IGNORE    Игнорировать записки от отдельных пользователей

### CHANGED
#MEMO_HELP_SEND
#	Синтаксис: SEND {ник | канал} текст-записки
#	
#	Отправляет записку для ника или для #канала, содержащее
#	текст-записки. Если получатель ник, то он будет уведомлен,
#	что Вы отправили ему/ей сообщение. Получатель, будь то ник или канал,
#	должен быть зарегистрирован.

### CHANGED
#MEMO_HELP_LIST
#	Синтаксис: LIST [канал] [номер | список | NEW]
#	
#	Показывает текущий список записок. С параметром NEW, покажет только
#	новые (непрочитанные) сообщения. Непрочитанные сообщения будут с пометкой в виде "*"
#	слева от номера сообщения. Вы можете также указать диапазон
#	номеров, например так:
#	
#	   LIST 2-5,7-9
#	      Покажет сообщения со 2 до 5 и с 7 до 9.

### CHANGED
#MEMO_HELP_LIST_EXPIRE
#	Синтаксис: LIST [канал] [номер | список | NEW]
#	
#	Показывает текущий список записок. С параметром NEW, покажет только
#	новые (непрочитанные) сообщения. Непрочитанные сообщения будут с пометкой в виде "*"
#	слева от номера сообщения, записки, помеченные как неистекающие, будут с пометкой "+".
#	Вы можете также указать диапазон номеров, например так:
#	
#	   LIST 2-5,7-9
#	      Покажет сообщения со 2 до 5 и с 7 до 9.

### CHANGED: syntax change
#MEMO_HELP_READ
#	Синтаксис: READ [канал] {номер | список | LAST | NEW}
#	
#	Покажет Вам текс записок, которые Вы указали. Если указан параметр LAST, то покажет
#	последнюю записку. Если указан параметр NEW, то покажет все непрочитанные ранее записки.
#	Иначе выведет Вам сообщение под номером  номер. Вы можете запросить сразу несколько
#	записок, также, как и в случае с командой LIST.

### CHANGED: syntax change
#MEMO_HELP_SAVE
#	Синтаксис: SAVE [канал] {номер | список}
#	
#	Пометит одну или несколько указанных Вами записок как неистекающие. Вы можете
#	указать номер одной записки, или, как в случае с командой LIST, несколько записок.

### CHANGED: syntax change
#MEMO_HELP_DEL
#	Синтаксис: DEL [канал] {номер | список | ALL}
#	
#	Удалит одну или несколько указанных Вами записок. Вы можете указать несколько
#	номеров записок или диапазон записок вместо одной, как в команде LIST; Вы
#	также можете указать параметр ALL, который укажет, что надо удалить все Ваши записки.
#	
#	Предупреждение: Если записка удалена, ее невозможно восстановить!

### CHANGED: MEMO_HELP_RENUMBER added

MEMO_HELP_SET
	Синтаксис: SET опция параметры
	
	Устанавливает различные параметры для службы записок. опция может быть одной из:
	
	    NOTIFY      Выбрать способ уведомления Вас
	                    о новых записках (только для ников)
	    LIMIT       Устанавливает лимит на количество записок, которое
	                    Вы можете принимать
MEMO_HELP_SET_OPTION_FORWARD
	    FORWARD     Устанавливает автоматическую пересылку записок Вам
MEMO_HELP_SET_END
	
	Наберите /msg %S HELP SET опция для получения более подробной информации
	о конкретной опции.

MEMO_HELP_SET_NOTIFY
	Синтаксис: SET NOTIFY {ON | LOGON | NEW | OFF}
	
	Изменить способ уведомления Вас о новых записках:
	
	    ON      Вы будете уведомлены о новом сообщении при входе в сеть,
	               когда Вы снимете с себя флаг /AWAY и как только кто-то отправит Вам записку.
	    LOGON   Вы будете уведомлены о новом сообщении только при входе в сеть
	               или когда Вы снимете с себя флаг /AWAY.
	    NEW     Вы будете уведомлены о новых записках только когда они
	               будут Вам отправлены.
	    OFF     Вы не будете получать уведомления о новых записках.
	
	ON эквивалентна комбинации LOGON и NEW.
	
	Учтите, что при включенной опции "охрана (secure)", Вы получите
	уведомления (при LOGON или ON) только после того, как идентифицируетесь на %s.

### CHANGED
#MEMO_HELP_SET_LIMIT
#	Синтаксис: SET LIMIT [канал] лимит
#	
#	Ограничивает максимальное количество записок, которое Вы (или канал)
#	можете принимать. Если Вы установите его равным 0, то никто
#	не сможет отправить Вам сообщение. Также, Вы не можете установить ограничение
#	больше, чем %d.

### CHANGED: channel parameter removed
#MEMO_HELP_INFO
#	Синтаксис: INFO [канал]
#	
#	Показывает информацию о количестве сообщений для Вас, сколько
#	из них непрочитано, сколько всего сообщений Вы можете принять и 
#	в каком случае Вы будете информированы о приходе новых.
#	С параметром покажет ту же информацию для указанного канала.

MEMO_OPER_HELP_COMMANDS
	
	Администраторы Сервисов могут использовать команды SET LIMIT и INFO на
	другие ники.

### CHANGED
#MEMO_OPER_HELP_SET_LIMIT
#	Синтаксис: SET LIMIT [ник | канал] {лимит | NONE | DEFAULT} [HARD]
#	
#	Ограничивает максимальное количество сообщений, которое пользователь (или канал)
#	может принимать. Установка лимита равным 0 не дает возможности
#	принимать сообщения вообще; установка в NONE разрешает
#	пользователю принимать и сохранять столько сообщений, сколько он пожелает и
#	установка в DEFAULT устанавливает значение по умолчанию (%d). Если
#	Вы не укажете пользователя или канал, то это будет установлено для
#	Вас.
#	
#	Опция HARD не позволит пользователю изменить лимит. Если
#	HARD не добавлять - будет противоположный эффект, т.е. пользователь сможет
#	изменять лимит (даже если предыдущий лимит был установлен с параметром
#	HARD).
#	
#	Команду SET LIMIT могут использовать только Администраторы
#	Сервисов. Другие пользователи могут менять лимит только для себя
#	или для канала, на котором у них есть соответствующие привилегии, не могут
#	удалить свой лимит или установить по умолчанию, не могут установить лимит выше %d
#	и не могут установить опцию HARD.

### CHANGED: channel parameter removed
#MEMO_OPER_HELP_INFO
#	Синтаксис: INFO [ник | канал]
#	
#	Без параметров покажет количество Ваших записок,
#	сколько из них непрочитано, сколько всего сообщений Вы можете принять и
#	в каком случае Вы будете информированы о новых записках.
#	
#	Если указан канал, то покажет тоже самое, но для канала.
#	Если указан ник, то покажет тоже самое, но для ника.
#	
#	Только для Администраторов Сервисов.

MEMO_HELP_FORWARD
	Синтаксис: FORWARD {номер | список | ALL}
	
	Пересылает одну или несколько записок на Ваш E-mail адрес.
	Вы должны иметь правильный E-mail адрес для Вашего ника, чтобы пользоваться этой командой.
	Записки для канала пересылать невозможно.

MEMO_HELP_SET_FORWARD
	Синтаксис: SET FORWARD {ON | COPY | OFF}
	
	Устанавливает, будет ли %S автоматически пересылать на Ваш E-mail
	адрес записки, посланные Вам. Если установлено в ON, то записки, посланные Вам, будут
	без сохранения локально пересылаться на E-mail адрес, зарегистрированный на Вашем нике.
	Параметр COPY аналогичен, только при этом %S сохранит копию записки как обычно.
	Если установлено в OFF, Ваши записки будут сохраняться как обычно. Даже если
	эта опция в состоянии ON, Вы все равно можете получать записки когда Вы онлайн если Сервисы
	не смогут их переправить.
	
	Учтите, что если опция установлена в COPY и Вы достигните максимально разрешенного
	лимита записок, Вы более не будете принимать новые записки, включая E-mail,
	пока Вы не удалите некоторые из старых командой DEL.

MEMO_HELP_IGNORE
	Синтаксис: IGNORE {ADD | DEL | LIST} [ник | маска]
	
	Управляет списком "игнорированных" пользователей; пользователей, чьи ники
	или user@host маска совпадает с записью из этого списка, которые не могут посылать
	Вам записки. Команда ADD добавляет указанный ник или user@host маску в Ваш список
	игнорирования, DEL удаляет указанный ник или маску из списка; LIST показывает
	Ваш текущий список игнорирования.
	
	Помните, что ник или маска могут содержать спецсимволы "*" или "?" и
	нечувствительны к регистру; к примеру, "*Evil*" совпадает с "EvilNick", "AN_EVIL_USER" и
	"devil".


###########################################################################
#
# OperServ help messages
#
###########################################################################

OPER_HELP
	%S дает возможность IRC Операторам управлять различными аспектами IRC сети.
	Наберите /msg %S HELP COMMANDS для получения списка команд.
	
	Уведомление: Все команды, посланные на %S записываются!

OPER_HELP_COMMANDS
	%S команды:
	    GLOBAL      Послать сообщение всем пользователям
	    STATS       Показать статистику сервисов и сети
	    SERVERMAP   Показать карту серверов сети
	    OPER LIST   Список Операторов Сервисов
	    ADMIN LIST  Список Администраторов Сервисов
OPER_HELP_COMMANDS_SERVOPER
	
	Команды, доступные только Операторам и Администраторам Сервисов:
	    MODE        Изменяет режим на канале
	    KICK        Выкинуть пользователя с канала
	    CLEARMODES  Очистка режимов на канале
	    CLEARCHAN   Убрать всех пользователей с канала (masskick)
	    KILLCLONES  Удалить всех пользователей с определенным хостом
OPER_HELP_COMMANDS_AKILL
	    AKILL       Управление списком AKILL'ов
	    AKILLCHAN   Autokill всех пользователей на канале
OPER_HELP_COMMANDS_EXCLUDE
	    EXCLUDE     Управление списком исключений из AKILL'ов
OPER_HELP_COMMANDS_SLINE
	    SGLINE      Управление списком SGLINE'ов
	    SQLINE      Управление списком SQLINE'ов
	    SZLINE      Управление списком SZLINE'ов
OPER_HELP_COMMANDS_SESSION
	    SESSION     Посмотреть список текущих соединений
	    EXCEPTION   Управление списком исключений из лимита сессий
OPER_HELP_COMMANDS_NEWS
	    LOGONNEWS   Установить сообщения при входе в сеть
	    OPERNEWS    Установить сообщения, показываемые IRC Операторам
OPER_HELP_COMMANDS_SERVADMIN
	
	Команды, доступные только Администраторам Сервисов:
	    OPER        Управление списком Операторов Сервисов
	    JUPE        "Джупнуть" сервер
	    SET         Установить различные опции Сервисов
	    UPDATE      Принудить Сервисы немедленно сохранить
	                    все базы данных на диск
	    SHUTDOWN    Завершить работу Сервисов с сохранением данных
	    QUIT        Завершить работу Сервисов без сохранения данных
	    RESTART     Сохранить базы данных и перезапустить Сервисы
	    REHASH      Перечитать конфигурационные файлы
	    SU          Получить привилегии суперпользователя Сервисов
OPER_HELP_COMMANDS_SERVROOT
	
	Команды, доступные только суперпользователю Сервисов:
	    ADMIN       Управление списком Администраторов Сервисов
OPER_HELP_COMMANDS_RAW
	    RAW         Послать raw строку на IRC сервер

OPER_HELP_GLOBAL
	Синтаксис: GLOBAL сообщение
	
	Позволяет IRC Операторам отправить сообщение всем пользователям IRC сети.
	Это сообщение будет отправлено от ника %s.

OPER_HELP_STATS
	Синтаксис: STATS [RESET | NETWORK | ALL]
	
	Без параметров, покажет текущее количество пользователей и
	IRC Операторов в онлайне (кроме Сервисов), максимальное число пользователей
	онлайн с момента старта Сервисов, и продолжительность работы Сервисов.
	
	Опция RESET сбрасывает счетчик максимально зарегистрированного Сервисами
	количества пользователей в онлайне.
	
	С опцией NETWORK покажет количество данных, переданных и принятых
	с сервера и количество памяти, используемое буферами приема/передачи.
	Отношение текущего использования буфера памяти к лимиту буфера памяти показывается
	в процентах, если этот лимит установлен.
	
	Опция ALL доступна только Администраторам Сервисов и показывает информацию
	об использовании памяти Сервисами. Использование этой опции может приостановить
	работу Сервисов на нкоторое время в больших сетях, поэтому не злоупотребляйте этой опцией!
	
	UPTIME является синонимом команды STATS.

OPER_HELP_SERVERMAP
	Синтаксис: SERVERMAP
	
	Показывает карту серверов, подключенных к сети с точки обзора Сервисов.
	"Псевдо" серверы--сами Сервисы и любые серверы, которые джупнуты
	командой JUPE -- имеют значок "(*)" после имени сервера.

OPER_HELP_OPER
	Синтаксис: OPER ADD ник
	        OPER DEL ник
	        OPER LIST
	
	Позволяет Администраторам Сервисов добавлять или удалять ники из списка
	Операторов Сервисов. Пользователь, чей ник присутствует в списке
	Операторов Сервисов, идентифицировавшись на %s получает доступ
	к командам Операторов Сервисов.
	
	Команда OPER ADD добавляет указанный ник в список Операторов Сервисов.
	Команда OPER DEL удаляет указанный ник из списка Операторов Сервисов.
	Команда OPER LIST показывает список Операторов Сервисов.
	
	Любой IRC Оператор может использовать команду OPER LIST.
	Все остальные команды могут использовать только Администраторы Сервисов.

OPER_HELP_ADMIN
	Синтаксис: ADMIN ADD ник
	        ADMIN DEL ник
	        ADMIN LIST
	
	Позволяет суперпользователю Сервисов добавлять или удалять ники
	из списка Администраторов Сервисов. Пользователь, чей ник
	находится в списке Администраторов Сервисов и который идентифицировался
	на %s получает доступ к командам Администраторов Сервисов.
	
	Команда ADMIN ADD добавляет указанный ник в список Администраторов Сервисов.
	Команда ADMIN DEL удаляет указанный ник из списка Администраторов Сервисов.
	Команда ADMIN LIST показывает список Администраторов Сервисов.
	
	Любой IRC Оператор может использовать команду ADMIN LIST.
	Все остальные команды могут использовать только суперпользователь Сервисов.

### CHANGED: OPER_HELP_GETKEY added

OPER_HELP_MODE
	Синтаксис: MODE канал режимы
	
	Позволяет Операторам Сервисов изменять режимы на любом канале.
	Параметры данной команды такие же, как и при использовании
	команды /MODE.
	
	Только для Операторов Сервисов.

### CHANGED
#OPER_HELP_CLEARMODES
#	Синтаксис: CLEARMODES channel [ALL]
#	
#	Очищает все двоичные режимы (n,s,t,k,l, и так далее) и баны на указанном
#	канале. Если указан параметр ALL, то также снимает со всех статус оператора канала,
#	войсы (режимы +o и +v) на канале.
#	
#	Только для Операторов Сервисов.

OPER_HELP_CLEARCHAN
	Синтаксис: CLEARCHAN канал
	
	Убирает всех пользователей с канала (masskick)
	
	Только для Операторов Сервисов.

OPER_HELP_KICK
	Синтаксис: KICK канал пользователь причина
	
	Позволяет IRC Операторам выкинуть пользователя с любого канала.
	Параметры данной команды такие же, как и при использовании команды /KICK.
	Сообщение кика будет включать имя IRC Оператора, использовавшего команду
	и указанную им причину, например:
	
	*** SpamMan has been kicked off channel #my_channel by %S (Alcan (флуд))
	
	Только для Операторов Сервисов.

### CHANGED
#OPER_HELP_AKILL
#	Синтаксис: AKILL ADD [+время_завершение] маска причина
#	        AKILL DEL маска
#	        AKILL LIST [маска] [NOEXPIRE]
#	        AKILL VIEW [маска] [NOEXPIRE]
#	        AKILL COUNT
#	
#	Позволяет Операторам Сервисов управлять списком AKILL'ов. Если
#	пользователь, подпадающий под маску AKILL'а попытается соединиться, сервисы
#	используют на него KILL, и, если Ваш IRC сервер это поддерживает,
#	укажет всем серверам IRC сети установить бан (K-line) на маску данного пользователя.
#	
#	AKILL ADD добавляет маску вида user@host в список AKILL'ов
#	с указанной причиной (которая обязательно должна быть указана).
#	AKILL DEL удаляет указанную маску из списка AKILL'ов.
#	AKILL LIST показывает текущий список AKILL'ов; если указать маску,
#	будут показаны только совпадающие с ней	значения.
#	AKILL VIEW покажет более подробную информацию, чем AKILL LIST, также покажет
#	того, кто добавил AKILL, время установления AKILL'а, когда
#	он истекает, ну и конечно же маску вида user@host и причину AKILL'а.
#	
#	Если указан параметр NOEXPIRE, то будут показаны только те записи, которые
#	не истекают и совпадают с указанной user@host маской.
#	AKILL COUNT вернет количество записей в списке.
#	
#	Опционально, время_окончания может быть дано с командой AKILL ADD.
#	Время_окончания ставится перед user@host маской и является целым числом перед символами
#	d(дней), h(часов) или m(минут). Допускается комбинировать эти значения (например 1h30m).
#	Если единица измерения не указана,	по умолчанию она будет установлена в "дни"
#	(таким образом +30 будет означать 30 дней). Чтобы добавить вечный AKILL,
#	используйте время истечения равным +0. Если добавляемая маска пользователя начинается с +,
#	время истечения должно быть указано, даже если оно равно времени по умолчанию.
#	
#	Причина должна описывать основание добавления записи; в зависимости от настроек Сервисов,
#	она может быть показана пользователь в KILL сообщениях.
OPER_HELP_AKILL_OPERMAXEXPIRY
	
	Операторы Сервисов могут указывать время_окончания вплоть до %s;
	Администраторы Сервисов могут указывать любое время.
OPER_HELP_AKILL_END
	
	Только для Операторов Сервисов.

OPER_HELP_AKILLCHAN
	Синтаксис: AKILLCHAN [KILL] [+время_окончания] канал причина
	
	Добавить autokills для всех пользователей кроме IRC Операторов на указанном канале,
	и, если указан параметр KILL, отключить их от сети.
	Причина сохраняется для каждой записи, и может быть показана в kill сообщении
	в зависимости от настроек Сервисов.
	Время_окончания задается в таком же формате, как и в команде AKILL ADD;
	если не указано, то значение по умолчанию %s.
	
	Autokills, добавленные этой командой имеют формат *@hostname, на каждый хост
	клиента на канале. Если такой autokill уже имеется для хоста, он не будет изменен;
	это приводит к тому, что количество добавленных autokills может быть меньше,
	чем количество посетителей на канале.
	
	Только для Операторов Сервисов.

### CHANGED: LIST/VIEW syntax changed, CLEAR and CHECK added
#OPER_HELP_EXCLUDE
#	Синтаксис: EXCLUDE ADD [+время_окончания] маска причина
#	        EXCLUDE DEL маска
#	        EXCLUDE LIST [маска] [NOEXPIRE]
#	        EXCLUDE VIEW [маска] [NOEXPIRE]
#	        EXCLUDE COUNT
#	
#	Позволяет Операторам Сервисов управлять списком AKILL исключений. Если пользователь
#	находится в списке исключений, то когда он заходит в сеть, Сервисы не будут применять
#	KILL для него даже если он находится в списке autokill. Это удобно, если Вы
#	имеете обширную autokill маску и желаете, чтобы определенные хосты или пользователи, попадающие
#	в эти маски, могли подключаться к сети.
#	
#	Подкоманды ADD, DEL, LIST, VIEW, и COUNT работают также, как и соответствующие
#	команды AKILL; смотрите помощь по команде AKILL для дополнительной информации.
#	
#	Только для Операторов Сервисов.

### CHANGED (similar to AKILL)
#OPER_HELP_SGLINE
#	Синтаксис: SGLINE ADD [+время_окончания] маска причина
#	        SGLINE DEL маска
#	        SGLINE LIST [маска] [NOEXPIRE]
#	        SGLINE VIEW [маска] [NOEXPIRE]
#	        SGLINE COUNT
#	
#	Позволяет Операторам Сервисов управлять списком SGLINE'ов. Если
#	пользователь, чье "реальное имя" совпадает с SGLINE маской, попытается
#	подключиться, Сервисы применят к нему KILL и, если поддерживается серверами, укажут
#	всем серверам установить бан (G-line) на совпавшую маску.
#	
#	
#	SGLINE ADD добавляет указанную маску реального имени в SGLINE список с
#	указанной причиной (которая обязательно должна быть указана).
#	SGLINE DEL удаляет указанную маску из списка SGLINE'ов.
#	SGLINE LIST показывает текущий список SGLINE'ов; если указать маску,
#	будут показаны только совпадающие с ней	значения.
#	SGLINE VIEW покажет более подробную информацию, чем SGLINE LIST, также покажет
#	того, кто добавил SGLINE, время установления SGLINE'а, когда последний раз активировалась,
#	ну и конечно же саму маску и причину. Если указан параметр NOEXPIRE, то будут показаны
#	только те записи, которые не истекают и совпадают с указанной маской. SGLINE COUNT
#	вернет количество записей в списке.
#	
#	Маска может быть в двойных кавычках и должна быть такой, если содержит пробелы, к примеру:
#	    /msg %S SGLINE ADD +0 "bad name" Мне твое имя не нравится.
#	
#	Опционально,  время_окончания  может быть дано с командой SGLINE ADD.
#	Время_окончания ставится перед user@host маской и является целым числом перед символами d(дней),
#	h(часов) или m(минут). Допускается комбинировать эти значения (например 1h30m).
#	Если единица измерения не указана,	по умолчанию она будет установлена в "дни"
#	(таким образом +30 будет означать 30 дней). Чтобы добавить вечный SGLINE,
#	используйте время окончания равным +0. Если добавляемая маска пользователя начинается с +,
#	и не указано время истечения время_окончания, маска должна быть в двойных кавычках,
#	как это описано выше.
#	
#	Причина должна описывать основание добавления записи; в зависимости от настроек Сервисов,
#	она может быть показана пользователь в KILL сообщениях.
#	
#	Только для Операторов Сервисов.

### CHANGED: LIST/VIEW syntax changed, CLEAR and CHECK added
#OPER_HELP_SQLINE
#	Синтаксис: SQLINE ADD [+время_окончания] маска причина
#	        SQLINE DEL маска
#	        SQLINE LIST [маска] [NOEXPIRE]
#	        SQLINE VIEW [маска] [NOEXPIRE]
#	        SQLINE COUNT
#	
OPER_HELP_SQLINE_KILL
	Позволяет Операторам Сервисов управлять списком SQLINE'ов. Если
	пользователь, чей ник совпадает с SQLINE маской попытается
	подключиться или поменять ник на совпадающий с маской, Сервисы применят
	к нему KILL, и, если поддерживается серверами, укажут
	всем серверам установить бан (Q-line) на маску данного пользователя.
OPER_HELP_SQLINE_NOKILL
	Позволяет Операторам Сервисов управлять списком SQLINE'ов. Если
	пользователь, чей ник совпадает с SQLINE маской попытается
	подключиться или поменять ник на совпадающий с маской, Сервисы поменяют
	его ник на "гостевой" и пошлют ему сообщение, которое обычно указывает клиенту
	запросить у пользователя новый ник. Если поддерживается серверами, Сервисы укажут
	всем серверам установить бан (Q-line) на соответствующую маску.
OPER_HELP_SQLINE_IGNOREOPERS
	
	IRC Операторы не попадают в поле действия SQLINE масок.
### CHANGED
#OPER_HELP_SQLINE_END
#	
#	Подкоманды ADD, DEL, LIST, VIEW, и COUNT могут быть использованы
#	для добавления, удаления просмотра (укороченная и полная версия) или подсчета
#	количества записей в SQLINE списке. Смотрите помощь для команды SGLINE
#	(/msg %S HELP SGLINE) для дополнительной информации.
#	
#	Только для Операторов Сервисов.

### CHANGED: LIST/VIEW syntax changed, CLEAR and CHECK added
#OPER_HELP_SZLINE
#	Синтаксис: SZLINE ADD [+время_окончания] маска причина
#	        SZLINE DEL маска
#	        SZLINE LIST [маска] [NOEXPIRE]
#	        SZLINE VIEW [маска] [NOEXPIRE]
#	        SZLINE COUNT
#	
#	Позволяет Операторам Сервисов управлять списком SZLINE'ов. Если 
#	пользователь, чей IP адрес совпадает с SZLINE маской, попытается 
#	подключиться, Сервисы применят к нему KILL и, если поддерживается 
#	серверами, укажут всем серверам установить бан (Z-line) на совпавшую маску.
#	
#	Подкоманды ADD, DEL, LIST, VIEW, и COUNT могут быть использованы
#	для добавления, удаления просмотра (укороченная и полная версия) или подсчета
#	количества записей в SZLINE списке. Смотрите помощь для команды SGLINE
#	(/msg %S HELP SGLINE) для дополнительной информации.
#	
#	Только для Операторов Сервисов.

### CHANGED
#OPER_HELP_EXCEPTION
#	Синтаксис: EXCEPTION ADD [+время_окончания] маска лимит причина
#	        EXCEPTION DEL {маска | список}
#	        EXCEPTION MOVE номер новый_номер
#	        EXCEPTION LIST [маска | список]
#	        EXCEPTION VIEW [маска | список]
#	
#	Позволяет Операторам Сервисов управлять списком хостов, которые имеют
#	особые лимиты сессий, позволяющие некоторым компьютерам иметь
#	больше соединений с IRC серверами одновременно. Как только хост
#	достигнет лимита сессий, все попытки соединиться с этого хоста будут отклонены.
#	Перед тем, как отключить пользователя, он будет уведомлен через /NOTICE
#	от %S относительно получения помощи по поводу лимитов сессий. Содержание
#	этого уведомления может быть изменено в конфигурации Сервисов.
#	
#	EXCEPTION ADD добавляет указанную маску хоста в список исключений. Учтите, что
#	хосты в формате nick ! user @ host и user@host неверны!
#	Только реальные маски хостов, такие как к примеру box.host.dom и *.host.dom,
#	допустимы, так как ограничение сессий не берет в расчет ники или имена пользователей.
#	лимит должен быть числом, большим или равным нулю; это определяет, столько сессий
#	этот хост может иметь одновременно (ноль означает, что этот хост имеет
#	неограниченное количество сессий).
#	
#	Опционально, время_окончания может быть дано с командой EXCEPTION ADD.
#	Время_окончания ставится перед маской хоста и является целым числом перед d(дней),
#	h(часов) или m(минут). Допускается комбинировать эти значения (например 1h30m).
#	Если единица измерения не указана,	по умолчанию она будет установлена в "дни"
#	(таким образом +30 будет означать 30 дней). Чтобы добавить вечный AKILL,
#	используйте время истечения равным +0. Если добавляемая маска хоста начинается с +,
#	время истечения должно быть указано, даже если оно равно времени по умолчанию.
#	
#	EXCEPTION DEL удаляет указанную маску или позицию с указанным номером из
#	списка исключений (номер может быть получен командами VIEW или LIST).
#	
#	EXCEPTION MOVE переносит исключение с номером  номер  в номер  новый_номер.
#	Если новый_номер уже существует, то он будет увеличен, чтобы сохраниться.
#	
#	EXCEPTION LIST и EXCEPTION VIEW показывают все имеющиеся исключения;
#	если указана маска, то список, соответствующий этой маске. Различие между
#	этими командами сводится к тому, что EXCEPTION VIEW дает более подробную
#	информацию, показывая имя того, кто добавил исключение, лимит сессии, маску хоста,
#	время окончания и время, когда последний раз это исключение "было задействовано", т.е.
#	тогда, когда кто-то соединился с соответствующим этому лимиту сессий хостом.
#	
#	Учтите, что подключаемые клиенты будут использовать первое соответствие
#	исключения с их хостом. Большие списки исключений и обширные маски могут привести
#	к снижению производительности Сервисов.
#	
#	Только для Операторов Сервисов.

OPER_HELP_SESSION
	Синтаксис: SESSION LIST порог
	        SESSION VIEW хост
	
	Позволяет Операторам Сервисов смотреть список сессий.
	
	SESSION LIST показывает список хостов, у которые количество соединений
	не меньше, чем указанный порог. Порог должен быть числом больше чем 1, для
	предотвращения получения списков с большим количеством одиночных соединений.
	
	SESSION VIEW показывает детальную информацию об указанном хосте, включая
	текущее количество сессий и лимит.
	Хост  не может иметь групповых символов.
	
	Смотрите помощь для EXCEPTION, чтобы получить больше информации о лимитировании сессий
	и как устанавливать лимиты для определенных хостов и групп.
	
	Только для Операторов Сервисов.

OPER_HELP_SU
	Синтаксис: SU пароль
	
	Предоставляет доступ к функциям, которые требуют привилегий суперпользователя Сервисов.
	Пароль должен быть тем же, который был указан командой SET SUPASS; учтите, что пароль
	должен быть установлен прежде чем использовать эту команду. Привилегии суперпользователя
	снимаются как только Вы покинете IRC.
	
	Предупреждение: Если Вы введете неверный пароль, Сервисы пошлют предупредительное сообщение
	всем IRC операторам.
	
	Только для Администраторов Сервисов.

OPER_HELP_SET
	Синтаксис: SET опция значение
	
	Устанавливает различные глобальные опции сервисов. Названия опций таковы:
	    READONLY   Установить режим только-чтение или чтение-запись
	    DEBUG      Активировать/деактивировать режим отладки
	    SUPASS     Установить пароль для команды SU (суперпользователь)
	
	Эту команду могут использовать только Администраторы сервисов; опция SUPASS
	может быть использована только суперпользователем Сервисов.

OPER_HELP_SET_READONLY
	Синтаксис: SET READONLY {ON | OFF}
	
	Включает или выключает режим только-чтения. В режиме только-чтения
	обычные пользователи не могу изменять любые данные,
	включая списки доступа для ников и каналов, и многое другое. IRC Операторы
	с достаточными привилегиями на Сервисах могут модифицировать список
	AKILL'ов и удалять или запрещать ники и каналы, но все эти изменения
	не будут сохранены, пока режим только-чтения не будет деактивирован
	либо сервисы не завершат работу или не перезапустятся.
	
	Эта опция является эквивалентом параметра командной строки
	-readonly.

OPER_HELP_SET_DEBUG
	Синтаксис: SET DEBUG {ON | OFF | номер}
	
	Включает/выключает режим отладки. В режиме отладки, все данные, которыми Сервисы
	обмениваются с IRC сервером, также как и некоторое количество других отладочных
	сообщений будут записаны в лог. Если указан номер, режим отладки
	активируется с уровнем отладки, установленным в  номер.
	
	Эта опция является эквивалентом параметра командной строки
	-debug.

OPER_HELP_SET_SUPASS
	Синтаксис: SET SUPASS [пароль]
	
	Устанавливает пароль, который потом используется в команде SU.
	Пароль должен быть установлен один раз перед первым использованием
	команды SU. Если пароль не установлен, то команда SU недоступна.
	
	Эту опцию может использовать только суперпользователь Сервисов.

OPER_HELP_JUPE
	Синтаксис: JUPE сервер [причина]
	
	Указывает Сервисам, чтобы они джупнули сервер - это создаст
	фальшивый "сервер" подключенный к сервисам, который
	не даст реальному серверу с этим именем подключиться к сети. Джуп
	может быть удален, используя стандартную команду SQUIT.
	Если указана причина, она будет показана в информации,
	выдаваемой подключаемому серверу; в ином случае, эта
	информационная строка будет содержать текст "Jupitered by <ник>",
	то есть покажет человека, который джупнул данный сервер.
	
	Если сервер с указанным именем уже существует в сети,
	он будет отключен автоматически.
	
	Только для Администраторов Сервисов.

### CHANGED
#OPER_HELP_RAW
#	Синтаксис: RAW текст
#	
#	Отправляет указанный текст напрямую на IRC сервер, с которым соединены Сервисы.
#	Эта команда имеет очень ограниченное использование и может вызвать
#	фатальную ошибку в сети при неверном использовании.
#	НЕ ИСПОЛЬЗУЙТЕ ЭТУ КОМАНДУ без абсолютной уверенности в том, что Вы делаете!
#	
#	Только для Администраторов Сервисов.

OPER_HELP_UPDATE
	Синтаксис: UPDATE [FORCE]
	
	Заставляет Сервисы обновить все файлы баз данных как только
	будет получена такая команда. Если указана опция FORCE, Сервисы
	попытаются разблокировать базы прежде чем выполнить обновление,
	это может исправить ошибку "databases are locked" при обновлении баз.
	
	Только для Администраторов Сервисов.

OPER_HELP_QUIT
	Синтаксис: QUIT
	
	Заставляет Сервисы немедленно завершить свою работу; базы данных
	не будут сохранены. Эта команда не должна использоваться кроме случаев
	повреждения базы данных в памяти, чтобы такая база данных не была сохранена.
	Для нормального завершения работы используйте команду SHUTDOWN.
	
	Только для Администраторов Сервисов.

OPER_HELP_SHUTDOWN
	Синтаксис: SHUTDOWN
	
	Заставляет Сервисы сохранить все базы данных и завершить работу.
	
	Только для Администраторов Сервисов.

OPER_HELP_RESTART
	Синтаксис: RESTART
	
	Заставляет сервисы сохранить все базы данных и перезапуститься
	(т.е. завершить работу и немедленно снова запустить Сервисы на исполнение).
	
	Только для Администраторов Сервисов.

OPER_HELP_REHASH
	Синтаксис: REHASH
	
	Заставляет сервисы перечитать конфигурационный файл и обновить активные настройки.
	Некоторые настройки, например RemoteServer (которая устанавливает имя IRC сервера,
	с которыми соединяются Сервисы), не могут быть изменены с REHASH и будут
	приведены в действие только когда Сервисы перезапустятся; поэтому будет записано
	соответствующее сообщение в логи Сервисов и послано уведомление Вам.
	
	Если была добавлена или удалена строчка LoadModule в основном
	конфигурационном файле (%s), соответствующие модули
	будут загружены или выгружены.
	Модули, которые более не загружаются строкой LoadModule будут выгружены в
	обратной последовательности; потом, когда оставшиеся модули будут перечитаны,
	новые модули будут загружены в той последовательности, которая указана
	в конфигурационном файле.
	При возникновении ошибки на любом этапе, вся операция будет прекращена.
	
	Учтите, что из-за зависимостей модулей друг от друга, некоторые модули не могут
	быть удалены или изменены через REHASH; в этом случае Вам необходимо перезапустить Сервисы,
	чтобы изменения вступили с силу.
	
	Только для Администраторов Сервисов.

OPER_HELP_KILLCLONES
	Синтаксис: KILLCLONES ник
	
	Отключает всех пользователей, хост которых такой же, как у указанного ника.
	Добавляется временный AKILL, в виде *@host, чтобы предотвратить
	немедленное пересоединение отключенных клиентов.
	Также при использовании этой команды будет отправлен WALLOPS,
	который сообщит о том, кто использовал команду, какой хост был использован и
	сколько пользователей было отключено. Эту команду очень удобно применять,
	чтобы убрать клонов из IRC сети.
	
	Только для Администраторов Сервисов.


###########################################################################
#
# StatServ help messages
#
###########################################################################

STAT_HELP
	%S предоставляет статистическую информацию об IRC сети,
	серверах и пользователях.
	Для получения списка команд наберите /msg %S HELP COMMANDS;
	для более подробной информации о нужной команде наберите: /msg %S HELP команда.

STAT_HELP_COMMANDS
	Следующие команды доступны c %S:
	
	    SERVERS     Статистика серверов
	    USERS       Статистика пользователей

STAT_HELP_SERVERS
	Синтаксис: SERVERS STATS
	        SERVERS LIST [маска]
	        SERVERS VIEW [маска] [ONLINE | OFFLINE]
	
	Показывает статистику, относящуюся к серверам сети.
	
	SERVERS STATS показываем краткую сводку о серверах, которые
	видны (онлайн), невидны (офлайн) Сервисам.
	SERVERS LIST показывает список онлайн серверов и текущую статистику о каждом из них.
	SERVERS VIEW показывает список онлайн и офлайн серверов с подробной информацией
	о каждом из них. Указывая опции ONLINE и OFFLINE соответственно, покажет
	только онлайн и офлайн сервера.
	
	Используйте маску чтобы показать только нужные сервера.

STAT_HELP_USERS
	Синтаксис: USERS STATS
	
	Показывает статистику, относящуюся к пользователям сети.

STAT_OPER_HELP_SERVERS
	Синтаксис: SERVERS STATS
	        SERVERS LIST [маска]
	        SERVERS VIEW [маска] [ONLINE | OFFLINE]
	        SERVERS DELETE сервер
	        SERVERS COPY сервер новое_имя
	        SERVERS RENAME сервер новое_имя
	
	Показывает статистику, относящуюся к серверам сети.
	
	SERVERS STATS показываем краткую сводку о серверах, которые
	видны (онлайн), невидны (офлайн) Сервисам.
	SERVERS LIST показывает список онлайн серверов и текущую статистику о каждом из них.
	SERVERS VIEW показывает список онлайн и офлайн серверов с подробной информацией
	о каждом из них. Указывая опции ONLINE и OFFLINE соответственно, покажет
	только онлайн и офлайн сервера.
	
	Используйте маску чтобы показать только нужные сервера.
	
	DELETE удалить статистику для указанного сервера.
	COPY скопировать статистику указанного сервера.
	RENAME переименовать набор статистик указанного сервера.
	
	Эти три команды доступны только Администраторам Сервисов.
