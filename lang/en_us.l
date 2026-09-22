# Emacs: -*- mode: fundamental; coding: binary -*-

# United States English language file.
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
	English

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
	Sun
	Mon
	Tue
	Wed
	Thu
	Fri
	Sat
# %A
STRFTIME_DAYS_LONG
	Sunday
	Monday
	Tuesday
	Wednesday
	Thursday
	Friday
	Saturday
# %b
STRFTIME_MONTHS_SHORT
	Jan
	Feb
	Mar
	Apr
	May
	Jun
	Jul
	Aug
	Sep
	Oct
	Nov
	Dec
# %B
STRFTIME_MONTHS_LONG
	January
	February
	March
	April
	May
	June
	July
	August
	September
	October
	November
	December


# "day", "hour", "minute", and "second" in singular and plural.
# These must be preceded by spaces if the language requires them
# (e.g. in English: "10 hours", not "10hours").
STR_DAY
	 day
STR_DAYS
	 days
STR_HOUR
	 hour
STR_HOURS
	 hours
STR_MINUTE
	 minute
STR_MINUTES
	 minutes
STR_SECOND
	 second
STR_SECONDS
	 seconds
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
	Internal error--unable to process request.
SERVICES_IS_BUSY
	Services is currently busy; please try again later.

UNKNOWN_COMMAND
	Unknown command %s.
UNKNOWN_COMMAND_HELP
	Unknown command %s.  Type /msg %s HELP for help.
SYNTAX_ERROR
	Syntax: %s
MORE_INFO
	Type /msg %s HELP %s for more information.
NO_HELP_AVAILABLE
	No help available for %s.
MISSING_QUOTE
	Double quote (") missing.

BAD_EMAIL
	E-mail addresses must be in the form username@hostname.  You may not use colors, bold, underline, or reverse, or any of these characters: , : ; | \ " ( ) < > [ ]
REJECTED_EMAIL
	Sorry, that E-mail address cannot be used.
BAD_URL
	URLs must be in the form http://hostname[:port]/... (or ftp://, etc.).
BAD_USERHOST_MASK
	Mask must be in the form user@host.
BAD_NICKUSERHOST_MASK
	Mask must be in the form nick!user@host.
BAD_EXPIRY_TIME
	Invalid expiry time.

SENDMAIL_NO_RESOURCES
	The system is currently too busy to send mail; please try again later.
READ_ONLY_MODE
	Notice: Services is in read-only mode; changes will not be saved!
PASSWORD_INCORRECT
	Password incorrect.
PASSWORD_WARNING
	Warning: After one more incorrect password you will be disconnected.

# What's the difference between "Access denied" and "Permission denied"?
# Very little; you can safely make them the same message with no real loss
# of meaning.  If you want to make a distinction, "Access denied" is
# usually used more generally; for example, a non-oper trying to access
# OperServ gets "Access denied", while a non-Services admin trying to use
# NickServ SET NOEXPIRE gets "Permission denied".
ACCESS_DENIED
	Access denied.
PERMISSION_DENIED
	Permission denied.

MORE_OBSCURE_PASSWORD
	Please try again with a more obscure password.  Passwords should be at least five characters long, and should not be something easily guessed (e.g. your real name or your nickname).

NICK_NOT_REGISTERED
	Your nickname isn't registered.
NICK_NOT_REGISTERED_HELP
	Your nickname isn't registered.  Type /msg %s HELP for information on registering your nickname.
NICK_TOO_LONG
	Nicknames must be no more than %d characters long.
NICK_INVALID
	%s is not a valid nickname.
NICK_X_NOT_REGISTERED
	Nickname %s isn't registered.
NICK_X_ALREADY_REGISTERED
	Nickname %s is already registered!
NICK_X_NOT_IN_USE
	Nickname %s isn't currently in use.
NICK_X_FORBIDDEN
	Nickname %s may not be registered or used.
NICK_X_SUSPENDED
	Nickname %s is suspended and may not be used or identified for.
NICK_X_SUSPENDED_MEMOS
	Nickname %s is suspended and may not receive memos.
NICK_IDENTIFY_REQUIRED
	Password authentication required for that command.
	Retry after typing /msg %s IDENTIFY password.
NICK_PLEASE_AUTH
	Your nickname is not yet authenticated.  Please authenticate your nickname using the authentication code in the mail message sent to: %s
NICK_X_NOT_ON_CHAN_X
	Nickname %s isn't currently on channel %s.

CHAN_INVALID
	%s is not a valid channel name.
CHAN_X_NOT_REGISTERED
	Channel %s isn't registered.
CHAN_X_NOT_IN_USE
	Channel %s isn't currently in use.
CHAN_X_FORBIDDEN
	Channel %s may not be registered or used.
CHAN_X_SUSPENDED
	Channel %s is suspended and may not be used or identified for.
CHAN_X_SUSPENDED_MEMOS
	Channel %s is suspended and may not receive memos.
CHAN_IDENTIFY_REQUIRED
	Password authentication required for that command.
	Retry after typing /msg %s IDENTIFY %s password.

SERV_X_NOT_FOUND
	Server %s not found.

EXPIRES_NONE
	does not expire
EXPIRES_NOW
	expired
EXPIRES_IN
	expires in %s

LIST_RESULTS
	End of list; %d/%d matches shown.

###########################################################################
#
# NickServ messages
#
###########################################################################

# Automatic messages
NICK_IS_REGISTERED
	This nickname is owned by someone else.  Please choose another.  (If this is your nickname, type /msg %s IDENTIFY password.)
NICK_IS_SECURE
	This nickname is registered and protected.  If it is your nickname, type /msg %s IDENTIFY password.  Otherwise, please choose a different nickname.
NICK_MAY_NOT_BE_USED
	This nickname may not be used. Please choose another one.
DISCONNECT_IN_1_MINUTE
	If you do not change your nickname within one minute, you will be disconnected.
DISCONNECT_IN_20_SECONDS
	If you do not change your nickname within 20 seconds, you will be disconnected.
DISCONNECT_NOW
	This nickname has been registered; you may not use it.  Please choose another nickname before reconnecting.
FORCENICKCHANGE_IN_1_MINUTE
	If you do not change your nickname within one minute, it will be changed automatically.
FORCENICKCHANGE_IN_20_SECONDS
	If you do not change your nickname within 20 seconds, it will be changed automatically.
FORCENICKCHANGE_NOW
	This nickname has been registered; you may not use it.  Your nickname is now being changed to %s.
NICK_EXPIRES_SOON
	Notice: Your nickname's registration will expire in %s.  If you want to continue using your nickname, please identify to %s by typing: /msg %s IDENTIFY password
NICK_EXPIRED
	Notice: Your nickname's registration has expired.  Please re-register your nickname if you want to continue using it.

# REGISTER responses
NICK_REGISTER_SYNTAX
	REGISTER password [email]
NICK_REGISTER_REQ_EMAIL_SYNTAX
	REGISTER password email
NICK_REGISTRATION_DISABLED
	Sorry, nickname registration is temporarily disabled.
NICK_REGISTRATION_FAILED
	Sorry, registration failed.
NICK_REG_PLEASE_WAIT
	Please wait %s before using the REGISTER command again.
NICK_REG_PLEASE_WAIT_FIRST
	Please wait %s before using the REGISTER command.
NICK_CANNOT_BE_REGISTERED
	Nickname %s may not be registered.
NICK_REGISTER_EMAIL_UNAUTHED
	The E-mail address you provided has not yet been authenticated.  Please use the AUTH command to authenticate the address before registering new nicknames with it.
NICK_REGISTER_TOO_MANY_NICKS
	You have too many nicknames registered (%d, maximum of %d).  You cannot register any more until you drop some of your current nicknames.
NICK_REGISTERED
	Nickname %s has been registered to you.
NICK_PASSWORD_IS
	Your password is %s -- remember this for later use.

# IDENTIFY responses
NICK_IDENTIFY_SYNTAX
	IDENTIFY password
NICK_IDENTIFY_FAILED
	Sorry, identification failed.
NICK_IDENTIFY_EMAIL_MISSING
	You must set an E-mail address for your nickname before you continue using it.  Please set one now with the command: /msg %s SET EMAIL address
NICK_IDENTIFY_SUCCEEDED
	Password accepted -- you are now recognized.

# DROP responses
NICK_DROP_SYNTAX
	DROP password
NICK_DROP_WARNING
	Notice: The DROP command will de-register all of your linked nicknames!
NICK_DROP_DISABLED
	Sorry, nickname de-registration is temporarily disabled.
NICK_DROP_FAILED
	Sorry, nickname de-registration failed.
NICK_DROPPED
	Your nickname has been dropped.
NICK_DROPPED_LINKS
	Your nickname and all linked nicknames have been dropped.

# DROPNICK responses
NICK_DROPNICK_SYNTAX
	DROPNICK nickname
NICK_X_DROPPED
	Nickname %s has been dropped.
NICK_X_DROPPED_LINKS
	Nickname %s and all linked nicknames have been dropped.

# DROPEMAIL and DROPEMAIL-CONFIRM responses
NICK_DROPEMAIL_SYNTAX
	DROPEMAIL pattern
NICK_DROPEMAIL_PATTERN_TOO_LONG
	The given pattern is too long (maximum %d characters).
NICK_DROPEMAIL_NONE
	No nicknames have E-mail addresses matching the given pattern.
NICK_DROPEMAIL_COUNT
	%d nicknames have E-mail addresses matching the given pattern.
	To drop these nicknames, type: /msg %s DROPEMAIL-CONFIRM %s
NICK_DROPEMAIL_CONFIRM_SYNTAX
	DROPEMAIL-CONFIRM address
NICK_DROPEMAIL_CONFIRM_UNKNOWN
	The given pattern is not recognized.  Please use the DROPEMAIL command again with this pattern before using the DROPEMAIL-CONFIRM command.
NICK_DROPEMAIL_CONFIRM_DROPPING
	Dropping %d nicknames...
NICK_DROPEMAIL_CONFIRM_DROPPED
	Nickname drop complete.

# SET responses
NICK_SET_SYNTAX
	SET option parameters
NICK_SET_OPER_SYNTAX
	SET [!nickname] option parameters
NICK_SET_DISABLED
	Sorry, nickname option setting is temporarily disabled.
NICK_SET_UNKNOWN_OPTION
	Unknown SET option %s.

# SET PASSWORD responses
NICK_SET_PASSWORD_FAILED
	Sorry, couldn't change password.
NICK_SET_PASSWORD_CHANGED
	Password changed.
NICK_SET_PASSWORD_CHANGED_TO
	Password changed to %s.

# SET LANGUAGE responses
NICK_SET_LANGUAGE_SYNTAX
	SET LANGUAGE number
NICK_SET_LANGUAGE_UNKNOWN
	Unknown language number %d.  Type /msg %s HELP SET LANGUAGE for a list of languages.
NICK_SET_LANGUAGE_CHANGED
	Language changed to %s.

# SET URL responses
NICK_SET_URL_CHANGED
	URL for %s changed to %s.

# SET EMAIL responses
NICK_SET_EMAIL_PLEASE_WAIT
	Please wait %s before using the SET EMAIL command again.
NICK_SET_EMAIL_UNAUTHED
	The E-mail address you provided has not yet been authenticated.  Please use the AUTH command to authenticate the address first.
NICK_SET_EMAIL_TOO_MANY_NICKS
	%s already has %d nicknames registered (maximum of %d).  E-mail address change failed.
NICK_SET_EMAIL_CHANGED
	E-mail address for %s changed to %s.

# SET INFO responses
NICK_SET_INFO_CHANGED
	Information for %s changed.

# SET KILL responses
NICK_SET_KILL_SYNTAX
	SET KILL {ON | QUICK | OFF}
NICK_SET_KILL_IMMED_SYNTAX
	SET KILL {ON | QUICK | IMMED | OFF}
NICK_SET_KILL_ON
	Kill protection is now ON.
NICK_SET_KILL_QUICK
	Kill protection is now ON, with a reduced kill delay.
NICK_SET_KILL_IMMED
	Kill protection is now ON, with no kill delay.
NICK_SET_KILL_IMMED_DISABLED
	The IMMED option is not available on this network.
NICK_SET_KILL_OFF
	Kill protection is now OFF.

# SET SECURE responses
NICK_SET_SECURE_SYNTAX
	SET SECURE {ON | OFF}
NICK_SET_SECURE_ON
	Secure option is now ON.
NICK_SET_SECURE_OFF
	Secure option is now OFF.

# SET PRIVATE responses
NICK_SET_PRIVATE_SYNTAX
	SET PRIVATE {ON | OFF}
NICK_SET_PRIVATE_ON
	Private option is now ON.
NICK_SET_PRIVATE_OFF
	Private option is now OFF.

# SET NOOP responses
NICK_SET_NOOP_SYNTAX
	SET NOOP {ON | OFF}
NICK_SET_NOOP_ON
	No-op option is now ON.
NICK_SET_NOOP_OFF
	No-op option is now OFF.

# SET HIDE responses
NICK_SET_HIDE_SYNTAX
	SET HIDE {EMAIL | USERMASK | QUIT} {ON | OFF}
NICK_SET_HIDE_EMAIL_ON
	Your E-mail address will now be hidden from %s INFO displays.
NICK_SET_HIDE_EMAIL_OFF
	Your E-mail address will now be shown in %s INFO displays.
NICK_SET_HIDE_MASK_ON
	Your last seen user@host mask will now be hidden from %s INFO displays.
NICK_SET_HIDE_MASK_OFF
	Your last seen user@host mask will now be shown in %s INFO displays.
NICK_SET_HIDE_QUIT_ON
	Your last quit message will now be hidden from %s INFO displays.
NICK_SET_HIDE_QUIT_OFF
	Your last quit message will now be shown in %s INFO displays.

# SET TIMEZONE responses
NICK_SET_TIMEZONE_SYNTAX
	SET TIMEZONE {UTC-offset | time-zone | DEFAULT}
NICK_SET_TIMEZONE_TO
	Your time zone has been set to UTC%c%d:%02d.
	The current time in this time zone is %s.
NICK_SET_TIMEZONE_DEFAULT
	Your time zone has been reset to Services' time zone.

# SET MAINNICK responses
NICK_SET_MAINNICK_NOT_FOUND
	The nickname %s is not linked to your current nickname.
NICK_SET_MAINNICK_CHANGED
	Your main nickname has been changed to %s.

# SET NOEXPIRE responses
NICK_SET_NOEXPIRE_SYNTAX
	SET [!nickname] NOEXPIRE {ON | OFF}
NICK_SET_NOEXPIRE_ON
	Nickname %s will not expire.
NICK_SET_NOEXPIRE_OFF
	Nickname %s will expire.

# UNSET responses
NICK_UNSET_SYNTAX
	UNSET {URL | EMAIL | INFO}
NICK_UNSET_SYNTAX_REQ_EMAIL
	UNSET {URL | INFO}
NICK_UNSET_OPER_SYNTAX
	UNSET [!nick] {URL | EMAIL | INFO}
NICK_UNSET_OPER_SYNTAX_REQ_EMAIL
	UNSET [!nick] {URL | INFO}
NICK_UNSET_URL
	URL for %s unset.
NICK_UNSET_EMAIL
	E-mail address for %s unset.
NICK_UNSET_INFO
	Information for %s unset.
NICK_UNSET_EMAIL_BAD
	You may not clear your E-mail address.
NICK_UNSET_EMAIL_OTHER_BAD
	Nickname E-mail addresses may not be cleared.

# ACCESS responses
NICK_ACCESS_SYNTAX
	ACCESS {ADD | DEL | LIST} [mask]
NICK_ACCESS_DISABLED
	Sorry, nickname access list modification is temporarily disabled.
NICK_ACCESS_ALREADY_PRESENT
	Mask %s already present on your access list.
NICK_ACCESS_REACHED_LIMIT
	Sorry, you can only have %d access entries for a nickname.
NICK_ACCESS_NO_NICKS
	Warning: Nickname access masks should not include nicknames.  Please check and make sure that you have entered the user@host mask correctly.
NICK_ACCESS_ADDED
	%s added to your access list.
NICK_ACCESS_NOT_FOUND
	%s not found on your access list.
NICK_ACCESS_DELETED
	%s deleted from your access list.
NICK_ACCESS_LIST_EMPTY
	Your access list is empty.
NICK_ACCESS_LIST
	Access list:
NICK_ACCESS_LIST_X_EMPTY
	Access list for %s is empty.
NICK_ACCESS_LIST_X
	Access list for %s:

# LINK responses
NICK_OLD_LINK_SYNTAX
	LINK nickname password
NICK_LINK_SYNTAX
	LINK nickname
NICK_LINK_DISABLED
	Sorry, nickname linking is temporarily disabled.
NICK_LINK_FAILED
	Sorry, linking failed.
NICK_CANNOT_BE_LINKED
	Nickname %s may not be linked.
NICK_OLD_LINK_SAME
	You can't link to your current nickname!
NICK_LINK_SAME
	You can't link your current nickname!
NICK_LINK_ALREADY_LINKED
	Nickname %s is already linked to your nick.
NICK_LINK_IN_USE
	Nickname %s is currently in use.
NICK_LINK_TOO_MANY
	You cannot have more than %d nicknames linked together.
NICK_LINK_TOO_MANY_NICKS
	You already have %d nicknames registered (maximum of %d).  Linking failed.
NICK_OLD_LINK_TOO_MANY_CHANNELS
	Can't link to nickname %s: too many channels registered (combined maximum %d).
NICK_OLD_LINKED
	Your nickname has been linked to %s.
NICK_LINKED
	Nickname %s has been linked to your nickname.

# UNLINK responses
NICK_OLD_UNLINK_SYNTAX
	UNLINK [nickname password]
NICK_UNLINK_SYNTAX
	UNLINK nickname
NICK_UNLINK_OPER_SYNTAX
	UNLINK nickname [FORCE]
NICK_UNLINK_DISABLED
	Sorry, nickname unlinking is temporarily disabled.
NICK_UNLINK_FAILED
	Sorry, unlinking failed.
NICK_UNLINK_SAME
	You can't unlink your current nickname!
NICK_OLD_UNLINK_NOT_LINKED
	Your nickname isn't linked.
NICK_UNLINK_NOT_LINKED_YOURS
	Nickname %s isn't linked to your nickname.
NICK_UNLINK_NOT_LINKED
	Nickname %s isn't linked.
NICK_OLD_UNLINKED
	Your nickname has been unlinked from %s.
NICK_UNLINKED
	Nickname %s has been unlinked from your nickname.
NICK_X_UNLINKED
	Nickname %s has been unlinked from %s.

# LISTLINKS responses
NICK_OLD_LISTLINKS_SYNTAX
	LISTLINKS nickname
NICK_LISTLINKS_SYNTAX
	LISTLINKS
NICK_LISTLINKS_HEADER
	List of nicknames linked to %s:
NICK_LISTLINKS_FOOTER
	%d linked nicknames.

# INFO responses
NICK_INFO_SYNTAX
	INFO nickname [ALL]
NICK_INFO_REALNAME
	%s is %s
NICK_INFO_ADDRESS
	Last seen address: %s
NICK_INFO_ADDRESS_ONLINE
	   Is online from: %s
NICK_INFO_ADDRESS_ONLINE_NOHOST
	%s is currently online.
NICK_INFO_ADDRESS_OTHER_NICK
	Is using nickname: %s
NICK_INFO_TIME_REGGED
	  Time registered: %s
NICK_INFO_LAST_SEEN
	   Last seen time: %s
NICK_INFO_LAST_QUIT
	Last quit message: %s
NICK_INFO_URL
	              URL: %s
NICK_INFO_EMAIL
	   E-mail address: %s
NICK_INFO_EMAIL_UNAUTHED
	   E-mail address: %s (unverified)
NICK_INFO_INFO
	      Information: %s
NICK_INFO_OPTIONS
	          Options: %s
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
	None
NICK_INFO_NO_EXPIRE
	This nickname will not expire.
NICK_INFO_SUSPEND_DETAILS
	Suspended by %s on %s (%s).
NICK_INFO_SUSPEND_REASON
	Reason: %s
NICK_INFO_SHOW_ALL
	For more information, type: /msg %s INFO %s ALL

# LISTCHANS responses
NICK_LISTCHANS_SYNTAX
	LISTCHANS
NICK_LISTCHANS_NONE
	%s has not registered any channels.
NICK_LISTCHANS_HEADER
	Channels registered by %s:
NICK_LISTCHANS_END
	End of list (%d channels registered).

# LIST/LISTEMAIL responses
NICK_LIST_SYNTAX
	%s [+start] pattern
NICK_LIST_OPER_SYNTAX
	%s [+start] pattern [FORBIDDEN] [NOEXPIRE] [SUSPENDED]
NICK_LIST_OPER_SYNTAX_AUTH
	%s [+start] pattern [FORBIDDEN] [NOEXPIRE] [SUSPENDED] [NOAUTH]
NICK_LIST_HEADER
	List of entries matching %s:
NICK_LIST_NO_MATCH
	No matching nicknames found.
NICK_LISTEMAIL_NONE
	(none set)

# RECOVER responses
NICK_RECOVER_SYNTAX
	RECOVER nickname [password]
NICK_NO_RECOVER_SELF
	You can't recover yourself!
NICK_RECOVERED
	The user claiming your nickname has been killed.  Type /msg %s RELEASE %s password to get it back before the one-minute timeout.

# RELEASE responses
NICK_RELEASE_SYNTAX
	RELEASE nickname [password]
NICK_RELEASE_NOT_HELD
	Nickname %s isn't being held.
NICK_RELEASED
	Services' hold on your nickname has been released.

# GHOST responses
NICK_GHOST_SYNTAX
	GHOST nickname [password]
NICK_NO_GHOST_SELF
	You can't ghost yourself!
NICK_GHOST_KILLED
	Ghost with your nickname has been killed.

# GETPASS responses
NICK_GETPASS_SYNTAX
	GETPASS nickname
NICK_GETPASS_UNAVAILABLE
	The GETPASS command cannot be used for nickname %s because encryption is in use.
NICK_GETPASS_PASSWORD_IS
	Password for %s is %s.

# FORBID responses
NICK_FORBID_SYNTAX
	FORBID nickname
NICK_FORBID_SUCCEEDED
	Nickname %s is now forbidden.
NICK_FORBID_FAILED
	Couldn't forbid nickname %s!

# SUSPEND responses
NICK_SUSPEND_SYNTAX
	SUSPEND [+expiry] nickname reason
NICK_SUSPEND_SUCCEEDED
	Nickname %s, and all nicknames linked to it, have been suspended.
NICK_SUSPEND_ALREADY_SUSPENDED
	Nickname %s is already suspended.

# UNSUSPEND responses
NICK_UNSUSPEND_SYNTAX
	UNSUSPEND nickname
NICK_UNSUSPEND_SUCCEEDED
	Nickname %s has been unsuspended.
NICK_UNSUSPEND_NOT_SUSPENDED
	Nickname %s is not suspended.

################ autojoin module messages/responses

# AJOIN responses
NICK_AJOIN_SYNTAX
	AJOIN {ADD | DEL | LIST} [channel]
NICK_AJOIN_ADD_SYNTAX
	AJOIN ADD channel
NICK_AJOIN_DEL_SYNTAX
	AJOIN DEL channel
NICK_AJOIN_DISABLED
	Sorry, nickname autojoin list modification is temporarily disabled.
NICK_AJOIN_ALREADY_PRESENT
	Channel %s already present on your autojoin list.
NICK_AJOIN_LIST_FULL
	Sorry, you can only have %d autojoin entries for a nickname.
NICK_AJOIN_ADDED
	%s added to your autojoin list.
NICK_AJOIN_NOT_FOUND
	%s not found on your autojoin list.
NICK_AJOIN_DELETED
	%s deleted from your autojoin list.
NICK_AJOIN_LIST_EMPTY
	Your autojoin list is empty.
NICK_AJOIN_LIST
	Autojoin list:
NICK_AJOIN_LIST_X_EMPTY
	Autojoin list for %s is empty.
NICK_AJOIN_LIST_X
	Autojoin list for %s:
NICK_AJOIN_AUTO_REMOVE
	Channel %s has been removed from your autojoin list because it is not a valid channel name.

################ mail-auth module messages/responses

# General-purpose messages
NICK_AUTH_SENDING
	An authentication code for your nickname is being sent to %s.
NICK_AUTH_FOR_REGISTER
	When you receive this message, type /msg %s AUTH code (replace code with the authentication code in the message) to complete your nickname registration.
NICK_AUTH_FOR_SET_EMAIL
	When you receive this message, type /msg %s AUTH code (replace code with the authentication code in the message) to complete your E-mail address change.
	Please note that you will not be able to continue using your nickname unless you complete the change.
NICK_AUTH_NOT_NEEDED
	There is currently nothing that needs authentication.
NICK_AUTH_NO_AUTHCODE
	Nickname %s has no authentication code.
NICK_AUTH_HAS_AUTHCODE
	Nickname %s already has an authentication code.
PASSWORD_WARNING_FOR_AUTH
	Warning: After one more incorrect authentication code you will be disconnected.

# Mail text.  The last "%s" (before the user@host) in the body text is
# replaced by one of the NICK_AUTH_MAIL_TEXT_* messages.
NICK_AUTH_MAIL_SUBJECT
	Authentication code for %s
NICK_AUTH_MAIL_BODY
	The authentication code for your nickname (%s) is: %09d
	Please submit this code to %s with the command:
	    /msg %s AUTH %09d
	
	This message was sent by %s in response to %s
	%s@%s.
NICK_AUTH_MAIL_TEXT_REG
	registration by
NICK_AUTH_MAIL_TEXT_EMAIL
	an E-mail address change by
NICK_AUTH_MAIL_TEXT_SENDAUTH
	a SENDAUTH command from
NICK_AUTH_MAIL_TEXT_REAUTH
	a REAUTH command from
NICK_AUTH_MAIL_BODY_SETAUTH
	The authentication code for your nickname (%s) is: %09d
	Please submit this code to %s with the command:
	    /msg %s AUTH %09d

# AUTH responses
NICK_AUTH_SYNTAX
	AUTH code
NICK_AUTH_DISABLED
	Sorry, nickname authentication is temporarily disabled.
NICK_AUTH_FAILED
	That authentication code is incorrect.
NICK_AUTH_SUCCEEDED_REGISTER
	Authentication succeeded; your nickname registration is now complete.
NICK_AUTH_SUCCEEDED_SET_EMAIL
	Authentication succeeded; your E-mail address change is now complete.
NICK_AUTH_SUCCEEDED_REAUTH
	Authentication succeeded; you are now identified for your nickname.
NICK_AUTH_SUCCEEDED_SETAUTH
	Authentication succeeded; you may now continue using your nickname.

# SENDAUTH responses
NICK_SENDAUTH_SYNTAX
	SENDAUTH
NICK_SENDAUTH_TOO_SOON
	Please wait %s before using the SENDAUTH command again.
NICK_SENDAUTH_NORESOURCES
	The message containing your authentication code could not be sent because the system is too busy.  Please try again later or contact a network administrator.
NICK_SENDAUTH_ERROR
	The message containing your authentication code could not be sent due to a system problem.  Please try again later or contact a network administrator.

# REAUTH responses
NICK_REAUTH_SYNTAX
	REAUTH
NICK_REAUTH_HAVE_AUTHCODE
	An authentication code is already set for your nickname.
NICK_REAUTH_NO_EMAIL
	Your nickname does not have an E-mail address set.
NICK_REAUTH_AUTHCODE_SET
	A new authentication code has been set for your nickname, and will be mailed to you shortly.  Use that code with the AUTH command to identify for your nickname.

# RESTOREMAIL responses
NICK_RESTOREMAIL_SYNTAX
	RESTOREMAIL password
NICK_RESTOREMAIL_NOT_NOW
	RESTOREMAIL can only be used after changing your E-mail address, and before authenticating the new address.
NICK_RESTOREMAIL_DONE
	Your previous E-mail address %s has been restored.

# SETAUTH responses
NICK_SETAUTH_SYNTAX
	SETAUTH nickname
NICK_SETAUTH_NO_EMAIL
	Nickname %s has no E-mail address set.
NICK_SETAUTH_AUTHCODE_SET
	A new authentication code (%09d) has been set for nickname %s.
NICK_SETAUTH_SEND_REFUSED
	Warning: unable to send mail to %s (message refused by mail server).
NICK_SETAUTH_SEND_TIMEOUT
	Warning: unable to send mail to %s (timeout connecting or sending to mail server).
NICK_SETAUTH_SEND_NORESOURCES
	Warning: unable to send mail to %s (insufficient resources).
NICK_SETAUTH_SEND_ERROR
	Warning: unable to send mail to %s (unknown error).
NICK_SETAUTH_USER_NOTICE
	You must authenticate your nickname before continuing to use it.  An authentication code is being sent to your E-mail address: %s
	Type /msg %s HELP AUTH for more information.

# GETAUTH responses
NICK_GETAUTH_SYNTAX
	GETAUTH nickname
NICK_GETAUTH_AUTHCODE_IS
	The authentication code for nickname %s is: %09d

# CLEARAUTH responses
NICK_CLEARAUTH_SYNTAX
	CLEARAUTH nickname
NICK_CLEARAUTH_CLEARED
	The authentication code for nickname %s has been cleared.

###########################################################################
#
# ChanServ messages
#
###########################################################################

# Access level descriptions
CHAN_LEVEL_AUTOOP
	Automatic channel operator status
CHAN_LEVEL_AUTOVOICE
	Automatic mode +v
CHAN_LEVEL_INVITE
	Allowed to use INVITE command
CHAN_LEVEL_AKICK
	Allowed to use AKICK command
CHAN_LEVEL_SET
	Allowed to use SET command (not FOUNDER/PASSWORD)
CHAN_LEVEL_CLEAR
	Allowed to use CLEAR command
CHAN_LEVEL_UNBAN
	Allowed to use UNBAN command
CHAN_LEVEL_OPDEOP
	Allowed to use OP/DEOP commands
CHAN_LEVEL_ACCESS_LIST
	Allowed to view the access list
CHAN_LEVEL_ACCESS_CHANGE
	Allowed to modify the access list
CHAN_LEVEL_MEMO
	Receives channel memos
CHAN_LEVEL_VOICE
	Allowed to use VOICE/DEVOICE commands
CHAN_LEVEL_AUTOHALFOP
	Automatic halfop (+h) status
CHAN_LEVEL_HALFOP
	Allowed to use HALFOP/DEHALFOP commands
CHAN_LEVEL_AUTOPROTECT
	Automatic channel protect (+a) status
CHAN_LEVEL_PROTECT
	Allowed to use PROTECT/DEPROTECT commands
CHAN_LEVEL_KICK
	Allowed to use KICK command
CHAN_LEVEL_STATUS
	Allowed to use STATUS command
CHAN_LEVEL_TOPIC
	Allowed to use TOPIC command

# Automatic responses
CHAN_IS_REGISTERED
	This channel has been registered with %s.
CHAN_MAY_NOT_BE_USED
	This channel may not be used.
CHAN_NOT_ALLOWED_TO_JOIN
	You are not permitted to be on this channel.

# Multi-purpose responses
CHAN_MUST_IDENTIFY_NICK
	Please identify with %s first, using the command:
	/msg %s IDENTIFY password
CHAN_BOUNCY_MODES
	Sorry, the %s command is temporarily unavailable.

# REGISTER responses
CHAN_REGISTER_SYNTAX
	REGISTER channel password description
CHAN_REGISTER_DISABLED
	Sorry, channel registration is temporarily disabled.
CHAN_REGISTER_SHORT_CHANNEL
	The channel # may not be registered.
CHAN_REGISTER_NOT_LOCAL
	Local channels cannot be registered.
CHAN_REGISTER_INVALID_NAME
	You may only register channels whose names begin with #.
CHAN_MUST_REGISTER_NICK
	You must register your nickname first.  Type /msg %s HELP for information on registering nicknames.
CHAN_MAY_NOT_BE_REGISTERED
	Channel %s may not be registered.
CHAN_ALREADY_REGISTERED
	Channel %s is already registered!
CHAN_MUST_BE_CHANOP
	You must be a channel operator to register the channel.
CHAN_REACHED_CHANNEL_LIMIT
	Sorry, you have already reached your limit of %d channels.
CHAN_EXCEEDED_CHANNEL_LIMIT
	Sorry, you have already exceeded your limit of %d channels.
CHAN_REGISTRATION_FAILED
	Sorry, registration failed.
CHAN_REGISTERED
	Channel %s registered under your nickname: %s
CHAN_PASSWORD_IS
	Your channel password is %s -- remember it for later use.

# IDENTIFY responses
CHAN_IDENTIFY_SYNTAX
	IDENTIFY channel password
CHAN_IDENTIFY_FAILED
	Sorry, identification failed.
CHAN_IDENTIFY_SUCCEEDED
	Password accepted -- you now have founder-level access to %s.

# DROP responses
CHAN_DROP_SYNTAX
	DROP channel password
CHAN_DROP_DISABLED
	Sorry, channel de-registration is temporarily disabled.
CHAN_DROPPED
	Channel %s has been dropped.

# DROPCHAN responses
CHAN_DROPCHAN_SYNTAX
	DROPCHAN channel

# SET responses
CHAN_SET_SYNTAX
	SET channel option parameters
CHAN_SET_DISABLED
	Sorry, channel option setting is temporarily disabled.
CHAN_SET_UNKNOWN_OPTION
	Unknown SET option %s.

# SET FOUNDER responses
CHAN_SET_FOUNDER_TOO_MANY_CHANS
	%s has too many channels registered.
CHAN_FOUNDER_CHANGED
	Founder of %s changed to %s.

# SET SUCCESSOR responses
CHAN_SUCCESSOR_CHANGED
	Successor for %s changed to %s.
CHAN_SUCCESSOR_IS_FOUNDER
	You can't make the founder of a channel the successor too.

# SET PASSWORD responses
CHAN_SET_PASSWORD_FAILED
	Sorry, couldn't set password.
CHAN_PASSWORD_CHANGED
	%s password changed.
CHAN_PASSWORD_CHANGED_TO
	%s password changed to %s.

# SET DESC responses
CHAN_DESC_CHANGED
	Description of %s changed to %s.

# SET URL responses
CHAN_URL_CHANGED
	URL for %s changed to %s.

# SET EMAIL responses
CHAN_EMAIL_CHANGED
	E-mail address for %s changed to %s.

# SET ENTRYMSG responses
CHAN_ENTRY_MSG_CHANGED
	Entry message for %s changed.

# SET MLOCK responses
CHAN_SET_MLOCK_NEED_PLUS_MINUS
	Mode list must begin with a + (plus) or - (minus).
CHAN_SET_MLOCK_NEED_PARAM
	Parameter required for MLOCK +%c.
CHAN_SET_MLOCK_NEED_POSITIVE
	Parameter for MLOCK +%c must be a positive number.
CHAN_SET_MLOCK_MODE_REG_BAD
	Mode +%c is always set on registered channels, and cannot be used with MLOCK.
CHAN_SET_MLOCK_LINK_BAD
	Parameter for MLOCK +%c is not a valid channel name.
CHAN_SET_MLOCK_LINK_SAME
	Parameter for MLOCK +%c must be a different channel.
CHAN_SET_MLOCK_REQUIRES
	MLOCK +%c requires MLOCK +%c to be set as well.
CHAN_SET_MLOCK_BAD_PARAM
	Invalid parameter for MLOCK +%c.
CHAN_SET_MLOCK_UNKNOWN_CHAR
	Unknown mode character %c ignored.
CHAN_SET_MLOCK_CANNOT_LOCK
	Mode %c cannot be locked.
CHAN_MLOCK_CHANGED
	Mode lock on channel %s changed to %s.
CHAN_MLOCK_REMOVED
	Mode lock on channel %s removed.

# SET HIDE responses
CHAN_SET_HIDE_SYNTAX
	SET channel HIDE {EMAIL | TOPIC | MLOCK} {ON | OFF}
CHAN_SET_HIDE_EMAIL_ON
	Channel %s E-mail address will now be hidden from %s INFO displays.
CHAN_SET_HIDE_EMAIL_OFF
	Channel %s E-mail address will now be shown in %s INFO displays.
CHAN_SET_HIDE_TOPIC_ON
	Channel %s topic will now be hidden from %s INFO displays.
CHAN_SET_HIDE_TOPIC_OFF
	Channel %s topic will now be shown in %s INFO displays.
CHAN_SET_HIDE_MLOCK_ON
	Channel %s mode lock will now be hidden from %s INFO displays.
CHAN_SET_HIDE_MLOCK_OFF
	Channel %s mode lock will now be shown in %s INFO displays.

# SET KEEPTOPIC responses
CHAN_SET_KEEPTOPIC_SYNTAX
	SET channel KEEPTOPIC {ON | OFF}
CHAN_SET_KEEPTOPIC_ON
	Topic retention option is now ON.
CHAN_SET_KEEPTOPIC_OFF
	Topic retention option is now OFF.

# SET TOPICLOCK responses
CHAN_SET_TOPICLOCK_SYNTAX
	SET channel TOPICLOCK {ON | OFF}
CHAN_SET_TOPICLOCK_ON
	Topic lock option is now ON.
CHAN_SET_TOPICLOCK_OFF
	Topic lock option is now OFF.

# SET PRIVATE responses
CHAN_SET_PRIVATE_SYNTAX
	SET channel PRIVATE {ON | OFF}
CHAN_SET_PRIVATE_ON
	Private option is now ON.
CHAN_SET_PRIVATE_OFF
	Private option is now OFF.

# SET SECUREOPS responses
CHAN_SET_SECUREOPS_SYNTAX
	SET channel SECUREOPS {ON | OFF}
CHAN_SET_SECUREOPS_ON
	Secure ops option is now ON.
CHAN_SET_SECUREOPS_OFF
	Secure ops option is now OFF.

# SET LEAVEOPS responses
CHAN_SET_LEAVEOPS_SYNTAX
	SET channel LEAVEOPS {ON | OFF}
CHAN_SET_LEAVEOPS_ON
	Leave ops option is now ON.
CHAN_SET_LEAVEOPS_OFF
	Leave ops option is now OFF.

# SET RESTRICTED responses
CHAN_SET_RESTRICTED_SYNTAX
	SET channel RESTRICTED {ON | OFF}
CHAN_SET_RESTRICTED_ON
	Restricted access option is now ON.
CHAN_SET_RESTRICTED_OFF
	Restricted access option is now OFF.

# SET SECURE responses
CHAN_SET_SECURE_SYNTAX
	SET channel SECURE {ON | OFF}
CHAN_SET_SECURE_ON
	Secure option is now ON.
CHAN_SET_SECURE_OFF
	Secure option is now OFF.

# SET OPNOTICE responses
CHAN_SET_OPNOTICE_SYNTAX
	SET channel OPNOTICE {ON | OFF}
CHAN_SET_OPNOTICE_ON
	Op-notice option is now ON.
CHAN_SET_OPNOTICE_OFF
	Op-notice option is now OFF.

# SET ENFORCE responses
CHAN_SET_ENFORCE_SYNTAX
	SET channel ENFORCE {ON | OFF}
CHAN_SET_ENFORCE_ON
	Enforce option is now ON.
CHAN_SET_ENFORCE_OFF
	Enforce option is now OFF.

# SET MEMO-RESTRICTED responses
CHAN_SET_MEMO_RESTRICTED_SYNTAX
	SET channel MEMO-RESTRICTED {ON | OFF}
CHAN_SET_MEMO_RESTRICTED_ON
	Restricted memo option is now ON.
CHAN_SET_MEMO_RESTRICTED_OFF
	Restricted memo option is now OFF.

# SET NOEXPIRE responses
CHAN_SET_NOEXPIRE_SYNTAX
	SET channel NOEXPIRE {ON | OFF}
CHAN_SET_NOEXPIRE_ON
	Channel %s will not expire.
CHAN_SET_NOEXPIRE_OFF
	Channel %s will expire.

# UNSET responses
CHAN_UNSET_SYNTAX
	UNSET channel {SUCCESSOR | URL | EMAIL | ENTRYMSG}
CHAN_SUCCESSOR_UNSET
	Successor for %s unset.
CHAN_URL_UNSET
	URL for %s unset.
CHAN_EMAIL_UNSET
	E-mail address for %s unset.
CHAN_ENTRY_MSG_UNSET
	Entry message for %s unset.

# ACCESS messages
CHAN_ACCESS_SYNTAX
	ACCESS channel {ADD | DEL | LIST | LISTLEVEL | COUNT} [parameters]
CHAN_ACCESS_ADD_SYNTAX
	ACCESS channel ADD nickname level
CHAN_ACCESS_DEL_SYNTAX
	ACCESS channel DEL nickname
CHAN_ACCESS_LIST_SYNTAX
	ACCESS channel LIST [+start] [mask]
CHAN_ACCESS_LISTLEVEL_SYNTAX
	ACCESS channel LISTLEVEL [+start] level
CHAN_ACCESS_COUNT_SYNTAX
	ACCESS channel COUNT
CHAN_ACCESS_DISABLED
	Sorry, channel access list modification is temporarily disabled.
CHAN_ACCESS_LEVEL_NONZERO
	Access level must be non-zero.
CHAN_ACCESS_LEVEL_RANGE
	Access level must be between %d and %d inclusive.
CHAN_ACCESS_NICKS_ONLY
	Channel access lists may only contain registered nicknames.
CHAN_ACCESS_NOOP
	%s cannot be added to channel access lists.
CHAN_ACCESS_REACHED_LIMIT
	Sorry, you can only have %d access entries on a channel.
CHAN_ACCESS_LEVEL_UNCHANGED
	Access level for %s on %s unchanged from %d.
CHAN_ACCESS_LEVEL_CHANGED
	Access level for %s on %s changed to %d.
CHAN_ACCESS_ADDED
	%s added to %s access list at level %d.
CHAN_ACCESS_NOT_FOUND
	%s not found on %s access list.
CHAN_ACCESS_DELETED
	%s deleted from %s access list.
CHAN_ACCESS_LIST_EMPTY
	%s access list is empty.
CHAN_ACCESS_NO_MATCH
	No matching entries on %s access list.
CHAN_ACCESS_LIST_HEADER
	Access list for %s:
	    Level  Nickname
CHAN_ACCESS_LIST_FORMAT
	     %4d  %s
CHAN_ACCESS_COUNT
	The %s access list has %d entries.

# XOP (SOP/AOP/etc.) responses
CHAN_SOP_SYNTAX
	SOP channel {ADD | DEL | LIST | COUNT} [parameters]
CHAN_AOP_SYNTAX
	AOP channel {ADD | DEL | LIST | COUNT} [parameters]
CHAN_HOP_SYNTAX
	HOP channel {ADD | DEL | LIST | COUNT} [parameters]
CHAN_VOP_SYNTAX
	VOP channel {ADD | DEL | LIST | COUNT} [parameters]
CHAN_NOP_SYNTAX
	NOP channel {ADD | DEL | LIST | COUNT} [parameters]
CHAN_SOP_LIST_SYNTAX
	SOP channel {ADD | DEL | LIST | COUNT} [+start] [mask]
CHAN_AOP_LIST_SYNTAX
	AOP channel {ADD | DEL | LIST | COUNT} [+start] [mask]
CHAN_HOP_LIST_SYNTAX
	HOP channel {ADD | DEL | LIST | COUNT} [+start] [mask]
CHAN_VOP_LIST_SYNTAX
	VOP channel {ADD | DEL | LIST | COUNT} [+start] [mask]
CHAN_NOP_LIST_SYNTAX
	NOP channel {ADD | DEL | LIST | COUNT} [+start] [mask]
CHAN_XOP_ADDED
	%s added to %s %s list.
CHAN_XOP_LEVEL_CHANGED
	%s moved to %s %s list.
CHAN_XOP_LEVEL_UNCHANGED
	%s is already on the %s %s list.
CHAN_XOP_NICKS_ONLY
	SOP, AOP, VOP, and NOP lists may only contain registered nicknames.
CHAN_XOP_NOOP
	%s cannot be added to channel access lists.
CHAN_XOP_NICKS_ONLY_HOP
	SOP, AOP, HOP, VOP, and NOP lists may only contain registered nicknames.
CHAN_XOP_REACHED_LIMIT
	Sorry, the number of entries on your SOP, AOP, VOP, and NOP lists may not total more than %d entries.  Limit exceeded.
CHAN_XOP_REACHED_LIMIT_HOP
	Sorry, the number of entries on your SOP, AOP, HOP, VOP, and NOP lists may not total more than %d entries.  Limit exceeded.
CHAN_XOP_NOT_FOUND
	%s not found on %s %s list.
CHAN_XOP_DELETED
	%s deleted from %s %s list.
CHAN_XOP_LIST_EMPTY
	%s %s list is empty.
CHAN_XOP_NO_MATCH
	No matching entries on %s %s list.
CHAN_XOP_LIST_HEADER
	%s list for %s:
CHAN_XOP_COUNT
	The %s %s list has %d entries.

# AKICK responses
CHAN_AKICK_SYNTAX
	AKICK channel {ADD | DEL | LIST | VIEW | COUNT | ENFORCE} [parameters]
CHAN_AKICK_LIST_SYNTAX
	AKICK channel LIST [+start] [mask]
CHAN_AKICK_VIEW_SYNTAX
	AKICK channel VIEW [+start] [mask]
CHAN_AKICK_DISABLED
	Sorry, channel autokick list modification is temporarily disabled.
CHAN_AKICK_ALREADY_EXISTS
	%s already exists on %s autokick list.
CHAN_AKICK_REACHED_LIMIT
	Sorry, you can only have %d autokick masks on a channel.
CHAN_AKICK_ADDED
	%s added to %s autokick list.
CHAN_AKICK_NOT_FOUND
	%s not found on %s autokick list.
CHAN_AKICK_DELETED
	%s deleted from %s autokick list.
CHAN_AKICK_LIST_EMPTY
	%s autokick list is empty.
CHAN_AKICK_NO_MATCH
	No matching entries on %s autokick list.
CHAN_AKICK_LIST_HEADER
	Autokick list for %s:
# mask, set-by, set-time, last-used, reason
CHAN_AKICK_VIEW_FORMAT
	    %s (by %s on %s; last used %s)
	        %s
# mask, set-by, set-time, reason
CHAN_AKICK_VIEW_UNUSED_FORMAT
	    %s (by %s on %s; never used)
	        %s
CHAN_AKICK_COUNT
	%s has %d entries on its AKICK list.
CHAN_AKICK_ENFORCE_DONE
	AKICK ENFORCE for %s complete; %d users were affected.

# LEVELS responses
CHAN_LEVELS_SYNTAX
	LEVELS channel {SET | DIS[ABLE] | LIST | RESET} [item [level]]
CHAN_LEVELS_READONLY
	Sorry, channel privilege level modification is currently disabled.
CHAN_LEVELS_RANGE
	Level must be between %d and %d inclusive.
CHAN_LEVELS_CHANGED
	Level for %s on channel %s changed to %d.
CHAN_LEVELS_UNKNOWN
	Setting %s not known.  Type /msg %s HELP LEVELS DESC for a list of valid settings.
CHAN_LEVELS_DISABLED
	%s disabled on channel %s.
CHAN_LEVELS_LIST_HEADER
	Privilege level settings for channel %s:
CHAN_LEVELS_LIST_DISABLED
	    %-*s  (disabled)
CHAN_LEVELS_LIST_FOUNDER
	    %-*s  (founder only)
CHAN_LEVELS_LIST_NORMAL
	    %-*s  %4d
CHAN_LEVELS_RESET
	Privilege level settings for channel %s reset to defaults.

# INFO responses
CHAN_INFO_SYNTAX
	INFO channel [ALL]
CHAN_INFO_HEADER
	Information for channel %s:
CHAN_INFO_FOUNDER
	        Founder: %s
CHAN_INFO_SUCCESSOR
	      Successor: %s
CHAN_INFO_DESCRIPTION
	    Description: %s
CHAN_INFO_ENTRYMSG
	  Entry message: %s
CHAN_INFO_TIME_REGGED
	     Registered: %s
CHAN_INFO_LAST_USED
	      Last used: %s
CHAN_INFO_LAST_TOPIC
	     Last topic: %s
CHAN_INFO_TOPIC_SET_BY
	   Topic set by: %s
CHAN_INFO_URL
	            URL: %s
CHAN_INFO_EMAIL
	 E-mail address: %s
CHAN_INFO_OPTIONS
	        Options: %s
CHAN_INFO_OPT_PRIVATE
	Private
CHAN_INFO_OPT_KEEPTOPIC
	Topic Retention
CHAN_INFO_OPT_TOPICLOCK
	Topic Lock
CHAN_INFO_OPT_SECUREOPS
	Secure Ops
CHAN_INFO_OPT_LEAVEOPS
	Leave Ops
CHAN_INFO_OPT_RESTRICTED
	Restricted Access
CHAN_INFO_OPT_SECURE
	Secure
CHAN_INFO_OPT_OPNOTICE
	Op-Notice
CHAN_INFO_OPT_ENFORCE
	Enforce
CHAN_INFO_OPT_MEMO_RESTRICTED
	Restricted Memos
CHAN_INFO_OPT_NONE
	None
CHAN_INFO_MODE_LOCK
	      Mode lock: %s
CHAN_INFO_NO_EXPIRE
	This channel will not expire.
CHAN_INFO_SUSPEND_DETAILS
	Suspended by %s on %s (%s).
CHAN_INFO_SUSPEND_REASON
	Reason: %s
CHAN_INFO_SHOW_ALL
	For more information, type: /msg %s INFO %s ALL

# LIST responses
CHAN_LIST_SYNTAX
	LIST [+start] pattern
CHAN_LIST_OPER_SYNTAX
	LIST [+start] pattern [FORBIDDEN] [NOEXPIRE] [SUSPENDED]
CHAN_LIST_HEADER
	List of entries matching %s:
CHAN_LIST_NO_MATCH
	No matching channels found.

# INVITE responses
CHAN_INVITE_SYNTAX
	INVITE channel
CHAN_INVITE_OK
	Inviting %s to channel %s.

# OP/DEOP/VOICE/DEVOICE responses
CHAN_OPVOICE_SYNTAX
	%s channel [nickname]
CHAN_OP_SUCCEEDED
	Opped %s on channel %s.
CHAN_OP_ALREADY
	%s is already opped on channel %s.
CHAN_OP_FAILED
	Unable to op %s on channel %s.
CHAN_DEOP_SUCCEEDED
	Deopped %s on channel %s.
CHAN_DEOP_ALREADY
	%s is already deopped on channel %s.
CHAN_DEOP_FAILED
	Unable to deop %s on channel %s.
CHAN_VOICE_SUCCEEDED
	Voiced %s on channel %s.
CHAN_VOICE_ALREADY
	%s is already voiced on channel %s.
CHAN_VOICE_FAILED
	Unable to voice %s on channel %s.
CHAN_DEVOICE_SUCCEEDED
	Devoiced %s on channel %s.
CHAN_DEVOICE_ALREADY
	%s is already devoiced on channel %s.
CHAN_DEVOICE_FAILED
	Unable to devoice %s on channel %s.
CHAN_HALFOP_SUCCEEDED
	Halfopped %s on channel %s.
CHAN_HALFOP_ALREADY
	%s is already halfopped on channel %s.
CHAN_HALFOP_FAILED
	Unable to halfop %s on channel %s.
CHAN_DEHALFOP_SUCCEEDED
	Dehalfopped %s on channel %s.
CHAN_DEHALFOP_ALREADY
	%s is already dehalfopped on channel %s.
CHAN_DEHALFOP_FAILED
	Unable to dehalfop %s on channel %s.
CHAN_PROTECT_SUCCEEDED
	Protected %s on channel %s.
CHAN_PROTECT_ALREADY
	%s is already protected on channel %s.
CHAN_PROTECT_FAILED
	Unable to protect %s on channel %s.
CHAN_DEPROTECT_SUCCEEDED
	Deprotected %s on channel %s.
CHAN_DEPROTECT_ALREADY
	%s is already deprotected on channel %s.
CHAN_DEPROTECT_FAILED
	Unable to deprotect %s on channel %s.

# UNBAN responses
CHAN_UNBAN_SYNTAX
	UNBAN channel
CHAN_UNBANNED
	You have been unbanned from %s.

# KICK responses
CHAN_KICK_SYNTAX
	KICK channel nickname [reason]
CHAN_KICK_PROTECTED
	%s may not be kicked from channel %s.
CHAN_KICKED
	%s kicked from channel %s.

# TOPIC responses
CHAN_TOPIC_SYNTAX
	TOPIC channel topic

# CLEAR responses
CHAN_CLEAR_SYNTAX
	CLEAR channel what
CHAN_CLEARED_BANS
	All bans on channel %s have been removed.
CHAN_CLEARED_EXCEPTIONS
	All exceptions on channel %s have been removed.
CHAN_CLEARED_INVITES
	All invite masks on channel %s have been removed.
CHAN_CLEARED_MODES
	All modes on channel %s have been reset.
CHAN_CLEARED_OPS
	Mode +o has been cleared from channel %s.
CHAN_CLEARED_HALFOPS
	Mode +h has been cleared from channel %s.
CHAN_CLEARED_VOICES
	Mode +v has been cleared from channel %s.
CHAN_CLEARED_USERS
	All users have been kicked from channel %s.

# GETPASS responses
CHAN_GETPASS_SYNTAX
	GETPASS channel
CHAN_GETPASS_UNAVAILABLE
	The GETPASS command cannot be used for channel %s because encryption is in use.
CHAN_GETPASS_PASSWORD_IS
	Password for channel %s is %s.

# FORBID responses
CHAN_FORBID_SYNTAX
	FORBID channel
CHAN_FORBID_SHORT_CHANNEL
	The channel # may not be forbidden.
CHAN_FORBID_SUCCEEDED
	Channel %s is now forbidden.
CHAN_FORBID_FAILED
	Couldn't forbid channel %s!

# SUSPEND responses
CHAN_SUSPEND_SYNTAX
	SUSPEND [+expiry] channel reason
CHAN_SUSPEND_SUCCEEDED
	Channel %s has been suspended.
CHAN_SUSPEND_ALREADY_SUSPENDED
	Channel %s is already suspended.

# UNSUSPEND responses
CHAN_UNSUSPEND_SYNTAX
	UNSUSPEND channel
CHAN_UNSUSPEND_SUCCEEDED
	Channel %s has been unsuspended.
CHAN_UNSUSPEND_NOT_SUSPENDED
	Channel %s is not suspended.

###########################################################################
#
# MemoServ messages
#
###########################################################################

# Automatic messages
MEMO_HAVE_NEW_MEMO
	You have 1 new memo.
MEMO_HAVE_NEW_MEMOS
	You have %d new memos.
MEMO_TYPE_READ_LAST
	Type /msg %s READ LAST to read it.
MEMO_TYPE_READ_NUM
	Type /msg %s READ %d to read it.
MEMO_TYPE_LIST_NEW
	Type /msg %s LIST NEW to list them.
MEMO_AT_LIMIT
	Warning: You have reached your maximum number of memos (%d).  You will be unable to receive any new memos until you delete some of your current ones.
MEMO_OVER_LIMIT
	Warning: You are over your maximum number of memos (%d).  You will be unable to receive any new memos until you delete some of your current ones.
MEMO_NEW_MEMO_ARRIVED
	You have a new memo from %s.
	Type /msg %s READ %d to read it.
MEMO_NEW_CHAN_MEMO_ARRIVED
	You have a new memo from %s for %s.
	Type /msg %s READ %d to read it.

# Multi-use responses
MEMO_HAVE_NO_MEMOS
	You have no memos.
MEMO_HAVE_NO_NEW_MEMOS
	You have no new memos.
MEMO_DOES_NOT_EXIST
	Memo %d does not exist!
MEMO_LIST_NOT_FOUND
	No matching memos found.

# SEND responses
MEMO_SEND_SYNTAX
	SEND {nickname | channel} memo-text
MEMO_SEND_DISABLED
	Sorry, memo sending is temporarily disabled.
MEMO_SEND_PLEASE_WAIT
	Please wait %s before using the SEND command again.
MEMO_SEND_CHAN_NOT_AVAIL
	Sorry, channel memos are not available.
MEMO_X_GETS_NO_MEMOS
	%s cannot receive memos.
MEMO_X_HAS_TOO_MANY_MEMOS
	%s currently has too many memos and cannot receive more.
MEMO_SENT
	Memo sent to %s.
MEMO_SEND_FAILED
	Memo could not be sent to %s.

# LIST responses
MEMO_LIST_SYNTAX
	LIST [num | list | NEW]
MEMO_LIST_MEMOS
	Memos for %s.  To read, type: /msg %s READ num
MEMO_LIST_NEW_MEMOS
	New memos for %s.  To read, type: /msg %s READ num
MEMO_LIST_HEADER
	   Num  Sender            Date/Time
MEMO_LIST_FORMAT
	%c%c%c%3d  %-16s  %s

# READ responses
MEMO_READ_SYNTAX
	READ {num | list | LAST | NEW}
MEMO_HEADER
	Memo %d from %s (%s).  To delete, type: /msg %s DEL %d
MEMO_CHAN_HEADER
	Memo %d from %s for %s (%s).  To delete, type: /msg %s DEL %d

# SAVE responses
MEMO_SAVE_SYNTAX
	SAVE {num | list}
MEMO_SAVED_ONE
	Memo %d marked non-expiring.
MEMO_SAVED_SEVERAL
	%d memos marked non-expiring.

# DEL responses
MEMO_DEL_SYNTAX
	DEL {num | list | ALL}
MEMO_DELETED_NONE
	No memos were deleted.
MEMO_DELETED_ONE
	Memo %d has been deleted.
MEMO_DELETED_SEVERAL
	%d memos have been deleted.
MEMO_DELETED_ALL
	All of your memos have been deleted.

# RENUMBER responses
MEMO_RENUMBER_ONLY_YOU
	You can only use the RENUMBER command on yourself.
MEMO_RENUMBER_DONE
	Your memos have been renumbered.

# SET responses
MEMO_SET_SYNTAX
	SET option parameters
MEMO_SET_DISABLED
	Sorry, memo option setting is temporarily disabled.
MEMO_SET_UNKNOWN_OPTION
	Unknown SET option %s.

# SET NOTIFY responses
MEMO_SET_NOTIFY_SYNTAX
	SET NOTIFY {ON | LOGON | NEW | OFF}
MEMO_SET_NOTIFY_ON
	%s will now notify you of memos when you log on and when they are sent to you.
MEMO_SET_NOTIFY_LOGON
	%s will now notify you of memos when you log on or unset /AWAY.
MEMO_SET_NOTIFY_NEW
	%s will now notify you of memos when they are sent to you.
MEMO_SET_NOTIFY_OFF
	%s will not send you any notification of memos.

# SET LIMIT responses
MEMO_SET_LIMIT_SYNTAX
	SET LIMIT limit
MEMO_SET_LIMIT_OPER_SYNTAX
	SET LIMIT [nickname] {limit | NONE | DEFAULT} [HARD]
MEMO_SET_YOUR_LIMIT_FORBIDDEN
	You are not permitted to change your memo limit.
MEMO_SET_LIMIT_FORBIDDEN
	The memo limit for %s may not be changed.
MEMO_SET_YOUR_LIMIT_TOO_HIGH
	You cannot set your memo limit higher than %d.
MEMO_SET_LIMIT_TOO_HIGH
	You cannot set the memo limit for %s higher than %d.
MEMO_SET_LIMIT_OVERFLOW
	Memo limit too large; limiting to %d instead.
MEMO_SET_YOUR_LIMIT
	Your memo limit has been set to %d.
MEMO_SET_YOUR_LIMIT_ZERO
	You will no longer be able to receive memos.
MEMO_SET_YOUR_LIMIT_DEFAULT
	Your memo limit has been set to default (%d).
MEMO_UNSET_YOUR_LIMIT
	Your memo limit has been disabled.
MEMO_SET_LIMIT
	Memo limit for %s set to %d.
MEMO_SET_LIMIT_ZERO
	Memo limit for %s set to 0.
MEMO_SET_LIMIT_DEFAULT
	Memo limit for %s set to default (%d).
MEMO_UNSET_LIMIT
	Memo limit disabled for %s.

# INFO responses
MEMO_INFO_NO_MEMOS
	You currently have no memos.
MEMO_INFO_MEMO
	You currently have 1 memo.
MEMO_INFO_MEMO_UNREAD
	You currently have 1 memo, and it has not yet been read.
MEMO_INFO_MEMOS
	You currently have %d memos.
MEMO_INFO_MEMOS_ONE_UNREAD
	You currently have %d memos, of which 1 is unread.
MEMO_INFO_MEMOS_SOME_UNREAD
	You currently have %d memos, of which %d are unread.
MEMO_INFO_MEMOS_ALL_UNREAD
	You currently have %d memos; all of them are unread.
MEMO_INFO_LIMIT
	Your memo limit is %d.
MEMO_INFO_HARD_LIMIT
	Your memo limit is %d, and may not be changed.
MEMO_INFO_LIMIT_ZERO
	Your memo limit is 0; you will not receive any new memos.
MEMO_INFO_HARD_LIMIT_ZERO
	Your memo limit is 0; you will not receive any new memos.  You cannot change this limit.
MEMO_INFO_NO_LIMIT
	You have no limit on the number of memos you may keep.
MEMO_INFO_NOTIFY_OFF
	You will not be notified of new memos.
MEMO_INFO_NOTIFY_ON
	You will be notified of new memos at logon and when they arrive.
MEMO_INFO_NOTIFY_RECEIVE
	You will be notified when new memos arrive.
MEMO_INFO_NOTIFY_SIGNON
	You will be notified of new memos when you log on to IRC.
MEMO_INFO_X_NO_MEMOS
	%s currently has no memos.
MEMO_INFO_X_MEMO
	%s currently has 1 memo.
MEMO_INFO_X_MEMO_UNREAD
	%s currently has 1 memo, and it has not yet been read.
MEMO_INFO_X_MEMOS
	%s currently has %d memos.
MEMO_INFO_X_MEMOS_ONE_UNREAD
	%s currently has %d memos, of which 1 is unread.
MEMO_INFO_X_MEMOS_SOME_UNREAD
	%s currently has %d memos, of which %d are unread.
MEMO_INFO_X_MEMOS_ALL_UNREAD
	%s currently has %d memos; all of them are unread.
MEMO_INFO_X_LIMIT
	%s's memo limit is %d.
MEMO_INFO_X_HARD_LIMIT
	%s's memo limit is %d, and may not be changed.
MEMO_INFO_X_NO_LIMIT
	%s has no memo limit.
MEMO_INFO_X_NOTIFY_OFF
	%s is not notified of new memos.
MEMO_INFO_X_NOTIFY_ON
	%s is notified of new memos at logon and when they arrive.
MEMO_INFO_X_NOTIFY_RECEIVE
	%s is notified when new memos arrive.
MEMO_INFO_X_NOTIFY_SIGNON
	%s is notified of new memos at logon.

################ forward module messages/responses

# Mail format
MEMO_FORWARD_MAIL_SUBJECT
	Memo for %s
MEMO_FORWARD_MULTIPLE_MAIL_SUBJECT
	Memos for %s
MEMO_FORWARD_MAIL_BODY
	Memo from %s (%s)
	%s
MEMO_FORWARD_CHANMEMO_MAIL_BODY
	Memo from %s for %s (%s)
	%s

# FORWARD responses
MEMO_FORWARD_SYNTAX
	FORWARD {num | list | ALL}
MEMO_FORWARD_NEED_EMAIL
	You must set an E-mail address before memos can be forwarded to you.
MEMO_FORWARD_PLEASE_WAIT
	Please wait %s before using the FORWARD command again.
MEMO_FORWARD_FAILED
	Sorry, forwarding failed.
MEMO_FORWARDED_NONE
	No memos were forwarded.
MEMO_FORWARDED_ONE
	Memo %d has been forwarded.
MEMO_FORWARDED_SEVERAL
	%d memos have been forwarded.
MEMO_FORWARDED_ALL
	All of your memos have been forwarded.

# SET FORWARD responses
MEMO_SET_FORWARD_SYNTAX
	SET FORWARD {ON | COPY | OFF}
MEMO_SET_FORWARD_ON
	Your memos will now be forwarded to your E-mail address: %s
MEMO_SET_FORWARD_COPY
	Your memos will now be forwarded to your E-mail address: %s
	Your memos will also be saved by Services.
MEMO_SET_FORWARD_OFF
	Your memos will not be forwarded to your E-mail address.

################ ignore module messages/responses

# IGNORE responses
MEMO_IGNORE_SYNTAX
	IGNORE {ADD | DEL | LIST} [nickname | mask]
MEMO_IGNORE_ADD_SYNTAX
	IGNORE ADD nickname | mask
MEMO_IGNORE_DEL_SYNTAX
	IGNORE DEL nickname | mask
MEMO_IGNORE_LIST_FULL
	Your ignore list is full.
MEMO_IGNORE_ALREADY_PRESENT
	%s already present on your ignore list.
MEMO_IGNORE_ADDED
	%s added to your ignore list.
MEMO_IGNORE_NOT_FOUND
	%s not found on your ignore list.
MEMO_IGNORE_DELETED
	%s deleted from your ignore list.
MEMO_IGNORE_LIST_EMPTY
	Your ignore list is empty.
MEMO_IGNORE_LIST
	Ignore list:
MEMO_IGNORE_LIST_X_EMPTY
	Ignore list for %s is empty.
MEMO_IGNORE_LIST_X
	Ignore list for %s:

###########################################################################
#
# OperServ messages
#
###########################################################################

# General messages
OPER_BOUNCY_MODES
	Services is unable to change modes.  Are your servers configured correctly?
OPER_BOUNCY_MODES_U_LINE
	Services is unable to change modes.  Are your servers' U:lines configured correctly?

# GLOBAL responses
OPER_GLOBAL_SYNTAX
	GLOBAL message

# STATS responses
OPER_STATS_UNKNOWN_OPTION
	Unknown STATS option %s.
OPER_STATS_RESET_USER_COUNT
	The maximum user count has been reset.

OPER_STATS_CURRENT_USERS
	Current users: %d (%d ops)
OPER_STATS_MAX_USERS
	Maximum users: %d (%s)
# Parameters: days, STR_DAYS or STR_DAY, hours, minutes
OPER_STATS_UPTIME_DHM
	Services up %d%s, %02d:%02d
# If >= 1 hour, hours and minutes; if < 1 hour, minutes and seconds
OPER_STATS_UPTIME_HM_MS
	Services up %s

OPER_STATS_KBYTES_READ
	Data received:    %10u kB
OPER_STATS_KBYTES_WRITTEN
	Data sent:        %10u kB
OPER_STATS_NETBUF_SOCK
	Server socket buffers: %5u kB
OPER_STATS_NETBUF_SOCK_PERCENT
	Server socket buffers: %5u kB (%d%%)
OPER_STATS_NETBUF_TOTAL
	Total socket buffers:  %5u kB
OPER_STATS_NETBUF_TOTAL_PERCENT
	Total socket buffers:  %5u kB (%d%%)

OPER_STATS_ALL_USER_MEM
	Users     : %6d records, %5d kB
OPER_STATS_ALL_CHANNEL_MEM
	Channels  : %6d records, %5d kB
OPER_STATS_ALL_SERVER_MEM
	Servers   : %6d records, %5d kB
OPER_STATS_ALL_NICKGROUPINFO_MEM
	Nickgroups: %6d records, %5d kB
OPER_STATS_ALL_NICKINFO_MEM
	Nicknames : %6d records, %5d kB
OPER_STATS_ALL_CHANSERV_MEM
	ChanServ  : %6d records, %5d kB
OPER_STATS_ALL_STATSERV_MEM
	StatServ  : %6d records, %5d kB
OPER_STATS_ALL_NEWS_MEM
	News      : %6d records, %5d kB
OPER_STATS_ALL_AKILL_MEM
	Autokills : %6d records, %5d kB
OPER_STATS_ALL_EXCEPTION_MEM
	Exceptions: %6d records, %5d kB
OPER_STATS_ALL_SGLINE_MEM
	SGlines   : %6d records, %5d kB
OPER_STATS_ALL_SQLINE_MEM
	SQlines   : %6d records, %5d kB
OPER_STATS_ALL_SZLINE_MEM
	SZlines   : %6d records, %5d kB
OPER_STATS_ALL_SESSION_MEM
	Sessions  : %6d records, %5d kB

# GETKEY responses
OPER_GETKEY_SYNTAX
	GETKEY channel
OPER_GETKEY_KEY_IS
	The key for channel %s is: %s
OPER_GETKEY_NO_KEY
	Channel %s has no key set.

# MODE responses
OPER_MODE_SYNTAX
	MODE channel modes

# CLEARMODES responses
OPER_CLEARMODES_SYNTAX
	CLEARMODES channel [ALL]
OPER_CLEARMODES_DONE
	All channel modes cleared from channel %s.
OPER_CLEARMODES_ALL_DONE
	All channel and user modes cleared from channel %s.

# CLEARCHAN responses
OPER_CLEARCHAN_SYNTAX
	CLEARCHAN channel
OPER_CLEARCHAN_DONE
	All users removed from channel %s.

# KICK responses
OPER_KICK_SYNTAX
	KICK channel user reason

# ADMIN responses
OPER_ADMIN_SYNTAX
	ADMIN {ADD | DEL | LIST} [nickname]
OPER_ADMIN_ADD_SYNTAX
	ADMIN ADD nickname
OPER_ADMIN_DEL_SYNTAX
	ADMIN DEL nickname
OPER_ADMIN_NO_NICKSERV
	NickServ is currently disabled; the ADMIN command is unavailable.
OPER_ADMIN_EXISTS
	%s already exists on the Services admin list.
OPER_ADMIN_ADDED
	%s added to the Services admin list.
OPER_ADMIN_TOO_MANY
	Too many entries (%d) on the Services admin list; cannot add more.
OPER_ADMIN_REMOVED
	%s removed from the Services admin list.
OPER_ADMIN_NOT_FOUND
	%s not found on the Services admin list.
OPER_ADMIN_LIST_HEADER
	Services admin list:

# OPER responses
OPER_OPER_SYNTAX
	OPER {ADD | DEL | LIST} [nickname]
OPER_OPER_ADD_SYNTAX
	OPER ADD nickname
OPER_OPER_DEL_SYNTAX
	OPER DEL nickname
OPER_OPER_NO_NICKSERV
	NickServ is currently disabled; the OPER command is unavailable.
OPER_OPER_EXISTS
	%s already exists on the Services operator list.
OPER_OPER_ADDED
	%s added to the Services operator list.
OPER_OPER_TOO_MANY
	Too many entries (%d) on the Services operator list; cannot add more.
OPER_OPER_REMOVED
	%s removed from the Services operator list.
OPER_OPER_NOT_FOUND
	%s not found on the Services operator list.
OPER_OPER_LIST_HEADER
	Services operator list:

# AKILL/EXCLUDE/SxLINE common responses
OPER_MASKDATA_SYNTAX
	%s {ADD | DEL | CLEAR | LIST | VIEW | CHECK | COUNT} [parameters]
OPER_MASKDATA_ADD_SYNTAX
	%s ADD [+expiry] mask reason
OPER_MASKDATA_DEL_SYNTAX
	%s DEL mask
OPER_MASKDATA_CLEAR_SYNTAX
	%s CLEAR ALL
OPER_MASKDATA_LIST_SYNTAX
	%s {LIST | VIEW} [+start] [pattern] [NOEXPIRE]
OPER_MASKDATA_CHECK_SYNTAX
	%s CHECK mask
# mask, reason
OPER_MASKDATA_LIST_FORMAT
	%-32s  %s
# mask, set-by, set-time, last-used, expires, reason
OPER_MASKDATA_VIEW_FORMAT
	%s (by %s on %s, last used %s; %s)
	    %s
# mask, set-by, set-time, expires, reason
OPER_MASKDATA_VIEW_UNUSED_FORMAT
	%s (by %s on %s, never used; %s)
	    %s

# AKILL responses
OPER_TOO_MANY_AKILLS
	Autokill list is full!
OPER_AKILL_EXISTS
	%s already exists on autokill list.
OPER_AKILL_NO_NICK
	Notice: Autokill masks cannot contain nicknames; make sure you have not included a nickname portion in your mask.
OPER_AKILL_MASK_TOO_GENERAL
	The autokill mask is too general.
OPER_AKILL_EXPIRY_LIMITED
	You may only set an expiration time up to %s.
OPER_AKILL_ADDED
	%s added to autokill list.
OPER_AKILL_REMOVED
	%s removed from autokill list.
OPER_AKILL_CLEARED
	The autokill list has been cleared.
OPER_AKILL_NOT_FOUND
	%s not found on autokill list.
OPER_AKILL_LIST_HEADER
	Current autokill list:
OPER_AKILL_LIST_EMPTY
	The autokill list is empty.
OPER_AKILL_LIST_NO_MATCH
	No matching masks found on the autokill list.
OPER_AKILL_CHECK_NO_MATCH
	No masks matching %s found on the autokill list.
OPER_AKILL_CHECK_HEADER
	Masks matching %s on the autokill list:
OPER_AKILL_CHECK_TRAILER
	%d matching masks found.
OPER_AKILL_COUNT
	There are %d host masks on the autokill list.

# AKILLCHAN responses
OPER_AKILLCHAN_SYNTAX
	AKILLCHAN [KILL] [+expiry] channel reason
OPER_AKILLCHAN_AKILLED
	%d users autokilled.
OPER_AKILLCHAN_KILLED
	%d users killed.
OPER_AKILLCHAN_AKILLED_ONE
	1 user autokilled.
OPER_AKILLCHAN_KILLED_ONE
	1 user killed.

# EXCLUDE responses
OPER_TOO_MANY_EXCLUDES
	Autokill exclusion list is full!
OPER_EXCLUDE_EXISTS
	%s already exists on autokill exclusion list.
OPER_EXCLUDE_ADDED
	%s added to autokill exclusion list.
OPER_EXCLUDE_REMOVED
	%s removed from autokill exclusion list.
OPER_EXCLUDE_CLEARED
	The autokill exclusion list has been cleared.
OPER_EXCLUDE_NOT_FOUND
	%s not found on autokill exclusion list.
OPER_EXCLUDE_LIST_HEADER
	Current autokill exclusion list:
OPER_EXCLUDE_LIST_EMPTY
	The autokill exclusion list is empty.
OPER_EXCLUDE_LIST_NO_MATCH
	No matching masks found on the autokill exclusion list.
OPER_EXCLUDE_CHECK_NO_MATCH
	No masks matching %s found on the autokill exclusion list.
OPER_EXCLUDE_CHECK_HEADER
	Masks matching %s on the autokill exclusion list:
OPER_EXCLUDE_CHECK_TRAILER
	%d matching masks found.
OPER_EXCLUDE_COUNT
	There are %d host masks on the autokill exclusion list.

# SxLINE responses
OPER_TOO_MANY_SLINES
	%s list is full!
OPER_SLINE_EXISTS
	%s already exists on %s list.
OPER_SLINE_MASK_TOO_GENERAL
	The %s mask is too general.
OPER_SLINE_ADDED
	%s added to %s list.
OPER_SLINE_REMOVED
	%s removed from %s list.
OPER_SLINE_CLEARED
	The %s list has been cleared.
OPER_SLINE_NOT_FOUND
	%s not found on %s list.
OPER_SLINE_LIST_HEADER
	Current %s list:
OPER_SLINE_LIST_EMPTY
	The %s list is empty.
OPER_SLINE_LIST_NO_MATCH
	No matching masks found on the %s list.
OPER_SLINE_CHECK_NO_MATCH
	No masks matching %s found on the %s list.
OPER_SLINE_CHECK_HEADER
	Masks matching %s on the %s list:
OPER_SLINE_CHECK_TRAILER
	%d matching masks found.
OPER_SLINE_COUNT
	There are %d entries on the %s list.
OPER_SZLINE_NOT_AVAIL
	SZLINE is not available with this IRC server.

# SU responses
OPER_SU_SYNTAX
	SU password
OPER_SU_NO_PASSWORD
	The SU command is unavailable because no super-user password has been set.
OPER_SU_SUCCEEDED
	You now have Services super-user privileges.
OPER_SU_FAILED
	Unable to verify password.

# SET responses
OPER_SET_SYNTAX
	SET option setting
OPER_SET_IGNORE_ON
	Ignore code will be used.
OPER_SET_IGNORE_OFF
	Ignore code will not be used.
OPER_SET_IGNORE_ERROR
	Setting for IGNORE must be ON or OFF.
OPER_SET_READONLY_ON
	Services is now in read-only mode.
OPER_SET_READONLY_OFF
	Services is now in read-write mode.
OPER_SET_READONLY_ERROR
	Setting for READONLY must be ON or OFF.
OPER_SET_DEBUG_ON
	Services is now in debug mode.
OPER_SET_DEBUG_OFF
	Services is now in non-debug mode.
OPER_SET_DEBUG_LEVEL
	Services is now in debug mode (level %d).
OPER_SET_DEBUG_ERROR
	Setting for DEBUG must be ON, OFF, or a positive number.
OPER_SET_SUPASS_FAILED
	Unable to set Services super-user password.
OPER_SET_SUPASS_OK
	Services super-user password has been changed.
OPER_SET_SUPASS_NONE
	Services super-user password has been disabled.
OPER_SET_UNKNOWN_OPTION
	Unknown option %s.

# JUPE responses
OPER_JUPE_SYNTAX
	JUPE servername [reason]
OPER_JUPE_INVALID_NAME
	Invalid server name (server names must contain a period).
OPER_JUPE_ALREADY_JUPED
	Server %s is already juped.

# RAW responses
OPER_RAW_SYNTAX
	RAW text

# UPDATE responses
OPER_UPDATE_SYNTAX
	UPDATE [FORCE]
OPER_UPDATE_FORCE_FAILED
	Unable to unlock databases.
OPER_UPDATING
	Updating databases...
OPER_UPDATE_COMPLETE
	Database update complete.
OPER_UPDATE_FAILED
	Database update failed.

# REHASH responses
OPER_REHASHING
	Re-reading configuration settings.
OPER_REHASHED
	All configuration settings successfully updated.
OPER_REHASH_ERROR
	An error occurred while re-reading configuration settings.  Please check the Services log file for details.

# KILLCLONES responses
OPER_KILLCLONES_SYNTAX
	KILLCLONES nickname
OPER_KILLCLONES_UNKNOWN_NICK
	Could not find user %s.
OPER_KILLCLONES_KILLED
	%s used KILLCLONES for %s killing %d clones.
OPER_KILLCLONES_KILLED_AKILL
	%s used KILLCLONES for %s killing %d clones.  A temporary AKILL has been added for %s.


###########################################################################
#
# Session limiting and exception messages
#
###########################################################################

# EXCEPTION responses
OPER_EXCEPTION_SYNTAX
	EXCEPTION {ADD | DEL | CLEAR | MOVE | LIST | VIEW | CHECK | COUNT} [parameters]
OPER_EXCEPTION_ADD_SYNTAX
	EXCEPTION ADD [+expiry] mask limit reason
OPER_EXCEPTION_DEL_SYNTAX
	EXCEPTION DEL {mask | list}
OPER_EXCEPTION_CLEAR_SYNTAX
	EXCEPTION CLEAR ALL
OPER_EXCEPTION_MOVE_SYNTAX
	EXCEPTION MOVE num newnum
OPER_EXCEPTION_LIST_SYNTAX
	EXCEPTION {LIST | VIEW} [[+start] pattern | list]
OPER_EXCEPTION_ALREADY_PRESENT
	Mask %s already present on exception list.
OPER_EXCEPTION_TOO_MANY
	Session-limit exception list is full!
OPER_EXCEPTION_ADDED
	Session limit for %s set to %d.
OPER_EXCEPTION_MOVED
	Exception for %s (#%d) moved to number %d.
OPER_EXCEPTION_NO_SUCH_ENTRY
	No such entry (#%d) session-limit exception list.
OPER_EXCEPTION_NOT_FOUND
	%s not found on session-limit exception list.
OPER_EXCEPTION_NO_MATCH
	No matching entries on session-limit exception list.
OPER_EXCEPTION_EMPTY
	Session-limit exception list is empty.
OPER_EXCEPTION_DELETED
	%s deleted from session-limit exception list.
OPER_EXCEPTION_DELETED_ONE
	Deleted 1 entry from session-limit exception list.
OPER_EXCEPTION_DELETED_SEVERAL
	Deleted %d entries from session-limit exception list.
OPER_EXCEPTION_CLEARED
	Session-limit exception list has been cleared.
OPER_EXCEPTION_LIST_HEADER
	Current session-limit exception list:
# number, limit, host
OPER_EXCEPTION_LIST_COLHEAD
	Num  Limit  Host
OPER_EXCEPTION_LIST_FORMAT
	%3d  %4d   %s
# number, mask, set-by, set-time, expires, limit, reason
OPER_EXCEPTION_VIEW_FORMAT
	%3d.  %s  (by %s on %s; %s)
	    Limit: %-4d  - %s
OPER_EXCEPTION_CHECK_SYNTAX
	EXCEPTION CHECK mask
OPER_EXCEPTION_CHECK_NO_MATCH
	No masks matching %s found on the session-limit exception list.
OPER_EXCEPTION_CHECK_HEADER
	Masks matching %s on the session-limit exception list:
OPER_EXCEPTION_CHECK_TRAILER
	%d matching masks found.
OPER_EXCEPTION_COUNT
	There are %d entries on the session-limit exception list.
OPER_EXCEPTION_INVALID_LIMIT
	Invalid session limit; limit must be at least zero and no greater than %d.
OPER_EXCEPTION_INVALID_HOSTMASK
	Invalid hostmask.  Only real hostmasks are valid as exceptions are not matched against nicknames or usernames.

# SESSION responses
OPER_SESSION_SYNTAX
	SESSION {LIST limit | VIEW host}
OPER_SESSION_LIST_SYNTAX
	SESSION LIST limit
OPER_SESSION_VIEW_SYNTAX
	SESSION VIEW host
OPER_SESSION_INVALID_THRESHOLD
	Invalid threshold value; threshold must be an integer greater than 1.
OPER_SESSION_NOT_FOUND
	%s not found on session list.
OPER_SESSION_LIST_HEADER
	Hosts with at least %d sessions:
OPER_SESSION_LIST_COLHEAD
	Sessions  Host
# limit, host
OPER_SESSION_LIST_FORMAT
	%6d    %s
# host, sessions, limit
OPER_SESSION_VIEW_FORMAT
	The host %s currently has %d sessions with a limit of %d.


###########################################################################
#
# News system messages
#
###########################################################################

NEWS_LOGON_TEXT
	[Logon News - %s] %s
NEWS_OPER_TEXT
	[Oper News - %s] %s

NEWS_LOGON_SYNTAX
	LOGONNEWS {ADD | DEL | LIST} [text | num]
NEWS_LOGON_LIST_HEADER
	Logon news items:
NEWS_LOGON_LIST_ENTRY
	%5d (%s by %s)
	    %s
NEWS_LOGON_LIST_NONE
	There is no logon news.
NEWS_LOGON_ADD_SYNTAX
	LOGONNEWS ADD text
NEWS_LOGON_ADD_FULL
	News list is full!
NEWS_LOGON_ADDED
	Added new logon news item (#%d).
NEWS_LOGON_DEL_SYNTAX
	LOGONNEWS DEL {num | ALL}
NEWS_LOGON_DEL_NOT_FOUND
	Logon news item #%d not found!
NEWS_LOGON_DELETED
	Logon news item #%d deleted.
NEWS_LOGON_DEL_NONE
	No logon news items to delete!
NEWS_LOGON_DELETED_ALL
	All logon news items deleted.

NEWS_OPER_SYNTAX
	OPERNEWS {ADD | DEL | LIST} [text | num]
NEWS_OPER_LIST_HEADER
	Oper news items:
NEWS_OPER_LIST_ENTRY
	%5d (%s by %s)
	    %s
NEWS_OPER_LIST_NONE
	There is no oper news.
NEWS_OPER_ADD_SYNTAX
	OPERNEWS ADD text
NEWS_OPER_ADD_FULL
	News list is full!
NEWS_OPER_ADDED
	Added new oper news item (#%d).
NEWS_OPER_DEL_SYNTAX
	OPERNEWS DEL {num | ALL}
NEWS_OPER_DEL_NOT_FOUND
	Oper news item #%d not found!
NEWS_OPER_DELETED
	Oper news item #%d deleted.
NEWS_OPER_DEL_NONE
	No oper news items to delete!
NEWS_OPER_DELETED_ALL
	All oper news items deleted.

NEWS_HELP_LOGON
	Syntax: LOGONNEWS ADD text
	        LOGONNEWS DEL {num | ALL}
	        LOGONNEWS LIST
	
	Edits or displays the list of logon news messages.  When a
	user connects to the network, these messages will be sent
	to them.  (However, no more than three messages will be
	sent in order to avoid flooding the user.  If there are
	more than three news messages, only the three most recent
	will be sent.)
	
	LOGONNEWS LIST may be used by any IRC operator to list the
	current news messages.  ADD and DEL may only be used by
	Services operators.

NEWS_HELP_OPER
	Syntax: OPERNEWS ADD text
	        OPERNEWS DEL {num | ALL}
	        OPERNEWS LIST
	
	Edits or displays the list of oper news messages.  When a
	user opers up (with the /OPER command), these messages will
	be sent to them.  (However, no more than three messages will
	be sent in order to avoid flooding the user.  If there are
	more than three news messages, only the three most recent
	will be sent.)
	
	OPERNEWS LIST may be used by any IRC operator to list the
	current oper news messages.  ADD and DEL may only be used by
	Services operators.


###########################################################################
#
# StatServ messages
#
############################################################################

# General SERVERS responses
STAT_SERVERS_REMOVE_SERV_FIRST
	Server %s is online.  Remove it from the the network and try again.
STAT_SERVERS_SERVER_EXISTS
	Server %s already exists.

# SERVERS command syntax
STAT_SERVERS_SYNTAX
	SERVERS option parameters

# STATS responses
STAT_SERVERS_STATS_TOTAL
	Total Servers: %2d
STAT_SERVERS_STATS_ON_OFFLINE
	       Online: %2d (%d%%)  Offline: %2d (%d%%)
STAT_SERVERS_LASTQUIT_WAS
	    Last Quit: %-30s  %s

# LIST responses
STAT_SERVERS_LIST_HEADER
	Server Name                    Users      Opers
STAT_SERVERS_LIST_FORMAT
	%-30s %3d (%2d%%)  %3d (%2d%%)
STAT_SERVERS_LIST_RESULTS
	End of list -- %d/%d online servers shown.

# VIEW responses
STAT_SERVERS_VIEW_HEADER_ONLINE
	%-30s  (online)
STAT_SERVERS_VIEW_HEADER_OFFLINE
	%-30s  (offline)
STAT_SERVERS_VIEW_LASTJOIN
	    Last join: %s
STAT_SERVERS_VIEW_LASTQUIT
	    Last quit: %s
STAT_SERVERS_VIEW_QUITMSG
	 Quit message: %s
STAT_SERVERS_VIEW_USERS_OPERS
	        Users: %3d (%2d%%)  Opers: %3d (%2d%%)
STAT_SERVERS_VIEW_RESULTS
	End of list -- %d/%d servers shown.

# DELETE responses
STAT_SERVERS_DELETE_SYNTAX
	SERVERS DELETE server
STAT_SERVERS_DELETE_DONE
	Statistics for server %s have been deleted.

# COPY responses
STAT_SERVERS_COPY_SYNTAX
	SERVERS COPY server newname
STAT_SERVERS_COPY_DONE
	Statistics for server %s copied to %s.

# RENAME responses
STAT_SERVERS_RENAME_SYNTAX
	SERVERS RENAME server newname
STAT_SERVERS_RENAME_DONE
	Statistics for server %s renamed to %s.

# USERS responses
STAT_USERS_SYNTAX
	USERS STATS
STAT_USERS_TOTUSERS
	             Total users: %d
STAT_USERS_TOTOPERS
	             Total opers: %d
STAT_USERS_SERVUSERS
	Average users per server: %d
STAT_USERS_SERVOPERS
	Average opers per server: %d.%d


###########################################################################
#
# NickServ help messages
#
###########################################################################

NICK_HELP
	%S allows you to "register" a nickname and prevent
	others from using it.  %S is controlled through
	various commands which allow for registration and
	maintenance of nicknames.  For a list of commands, type
	/msg %S HELP COMMANDS; to use a command, type
	/msg %S command, and for more information on a
	specific command, type /msg %S HELP command.
NICK_HELP_EXPIRES
	
	If a registered nickname is not used for %s, the
	registration will expire and the nickname will be available
	for anyone to use.
NICK_HELP_WARNING
	
	NOTICE: This service is intended to provide a way for
	IRC users to ensure their identity is not compromised.
	It is NOT intended to facilitate "stealing" of
	nicknames or other malicious actions.  Abuse of %S
	will result in, at minimum, loss of the abused
	nickname(s).

NICK_HELP_COMMANDS
	The following commands can be used with %S:
	
	    REGISTER     Register a nickname
	    IDENTIFY     Identify yourself with your password
	    DROP         Cancel the registration of a nickname
NICK_HELP_COMMANDS_AUTH
	    AUTH         Authenticate a nickname registration
	    SENDAUTH     Send yourself the nickname authentication code
	    REAUTH       Set a new code if you forget your password
	    RESTOREMAIL  Restore the last authenticated E-mail address
NICK_HELP_COMMANDS_LINK
	    LINK         Link another nickname to your own
	    UNLINK       Cancel a linked nickname
	    LISTLINKS    List all nicknames you have linked
NICK_HELP_COMMANDS_ACCESS
	    ACCESS       Modify the list of authorized addresses
NICK_HELP_COMMANDS_AJOIN
	    AJOIN        Modify your autojoin list
NICK_HELP_COMMANDS_SET
	    SET          Set options, including kill protection
	    UNSET        Clear nickname information
	    RECOVER      Kill another user who has taken your nickname
	    RELEASE      Regain custody of your nickname after RECOVER
	    GHOST        Disconnect a "ghost" session
	    INFO         Display information on a nickname
NICK_HELP_COMMANDS_LIST
	    LIST         List all nicknames matching a given mask
	    LISTEMAIL    List nicknames by E-mail address
NICK_HELP_COMMANDS_LISTCHANS
	    LISTCHANS    List all channels you own
	    STATUS       Check whether a given nickname is identified

NICK_HELP_REGISTER
	Syntax: %s
	
	Registers your nickname in the %S database.  Once
	your nickname is registered, you can use the SET and ACCESS
	commands to configure your nickname's settings as you like
	them.  Make sure you remember the password you use when
	registering; you'll need it to make changes to your nickname
	later.  (Note that case matters!  FIDO, Fido, and fido
	are all different passwords!)
NICK_HELP_REGISTER_EMAIL
	
	You may include an E-mail address when registering your
	nickname; you may also set one later using the SET EMAIL
	command.
NICK_HELP_REGISTER_EMAIL_REQ
	
	You must include an E-mail address when registering your
	nickname.  You can prevent your E-mail address from being
	shown to other users with the SET HIDE command (type
	/msg %S HELP SET HIDE for more information).
NICK_HELP_REGISTER_EMAIL_AUTH
	
	You must include a valid E-mail address when registering
	your nickname; %S will send a message with an
	authentication code to this address, which you must use with
	the AUTH command to complete your registration.  You can
	prevent your E-mail address from being shown to other users
	with the SET HIDE command (type /msg %S HELP SET HIDE for
	more information).
NICK_HELP_REGISTER_END
	
	Guidelines on choosing passwords:
	
	Passwords should not be easily guessable.  For example,
	using your real name as a password is a bad idea.  Using
	your nickname as a password is a much worse idea and,
	in fact, %S will not allow it.  Also, short
	passwords are vulnerable to trial-and-error searches, so
	you should choose a password at least 5 characters long.

NICK_HELP_IDENTIFY
	Syntax: IDENTIFY password
	
	Tells %S that you are really the owner of this
	nickname.  Many commands require you to authenticate
	yourself with this command before you use them.  The
	password should be the same one you sent with the REGISTER
	command.

NICK_HELP_DROP
	Syntax: DROP password
	
	De-registers (drops) your nickname from the %S
	database.  A nickname that has been dropped is free for
	anyone to re-register.
NICK_HELP_DROP_LINK
	
	Note that when you drop a nickname, all nicknames linked to
	it will be dropped as well.  To remove a single linked
	nickname, use the UNLINK command.
NICK_HELP_DROP_END
	
	To prevent accidental use of this command, you must enter
	your password in order to drop your nickname.

NICK_HELP_AUTH
	Syntax: AUTH code
	
	Authenticates registration or change of E-mail for a
	nickname.  To ensure that your E-mail address is correct,
	%S will send an message containing an authentication
	code to the E-mail address given when you register your
	nickname or change your E-mail address with the SET EMAIL
	command.  This command allows you to confirm that you have
	received the message and continue using your nickname
	normally.  Replace code in the command with the nine-digit
	authentication code given in the E-mail message.

NICK_HELP_SENDAUTH
	Syntax: SENDAUTH
	
	Sends you a copy of the authentication code for your
	nickname after a REGISTER or SET EMAIL.  If you accidentally
	delete or otherwise lose the message with the authentication
	code, you can use this command to have an extra copy sent to
	you.
	
	To prevent abuse, this command may only be used once every
	24 hours.

NICK_HELP_REAUTH
	Syntax: REAUTH
	
	Sets an authentication code which you can use to identify
	for your nickname.  If you forget your nickname password,
	use this command to generate a new authentication code, then
	give that code (which will be mailed to you) with the AUTH
	command to identify for your nickname; you can then change
	your password with the SET PASSWORD command as usual.

NICK_HELP_RESTOREMAIL
	Syntax: RESTOREMAIL password
	
	Restores the last authenticated E-mail address for your
	nickname.  If you accidentally use the wrong E-mail address
	in a SET EMAIL command, you can use this command to restore
	the previous E-mail address and clear the authentication
	code for your nickname, allowing you to identify for the
	nickname again.  You must give your nickname's password with
	this command.
	
	RESTOREMAIL can only be used after SET EMAIL and before
	entering the new authentication code with the AUTH command.

NICK_HELP_LINK
	Syntax: LINK nickname
	
	Links a new nickname to the nickname you are currently
	using, creating a new "alias" for your nickname.  The new
	nickname will be equivalent to your current nickname in
	terms of access privileges and memos; for example, you can
	read memos sent to one of your linked nicks from any other
	one, and if one of your nicks has auto-op privileges in a
	channel, you will get auto-opped no matter which linked
	nick you use.
	
	To cancel a link, use the UNLINK command.
	
	In order to use this command, you must identify for your
	current nickname (using the IDENTIFY command), and the
	nickname to be linked must be unregistered and not currently
	be in use.

NICK_HELP_UNLINK
	Syntax: UNLINK nickname
	
	Removes a link (alias) to your nickname created with the
	LINK command.  You cannot unlink the nickname you are
	currently using; change to another linked nick first before
	using this command.
	
	If you unlink the "main nickname" you have selected using
	the SET MAINNICK command, your current nickname will
	automatically be selected as the main nickname.

NICK_HELP_LISTLINKS
	Syntax: LISTLINKS
	
	Lists all nicknames which are linked to your nick.  The main
	nickname for the group of linked nicks will be indicated by
	a "*" to the left of the nick.

NICK_HELP_ACCESS
	Syntax: ACCESS ADD mask
	        ACCESS DEL mask
	        ACCESS LIST
	
	Modifies or displays the access list for your nickname.
	This is the list of user@host addresses which will be
	automatically recognized by %S as being allowed to use
	the nickname.  If you connect to IRC with an address on this
	list, you will not be affected by the nick's SET KILL
	setting, and if the SECURE option is disabled, you will be
	able to receive auto-op and other privileges in channels
	without using the IDENTIFY command.
	
	The mask given to the ADD and DEL commands may contain
	wildcards (* or ?).
	
	Examples:
	
	    ACCESS ADD achurch@*.dragonfire.net
	        Allows access to user achurch from any machine in
	        the dragonfire.net domain.
	
	    ACCESS DEL achurch@*.dragonfire.net
	        Reverses the previous command.
	
	    ACCESS LIST
	        Displays the current access list.

NICK_HELP_SET
	Syntax: SET option parameters
	
	Sets various nickname options.  option can be one of:
	
	    PASSWORD   Set your nickname password
	    LANGUAGE   Set the language Services will use when
	                   sending messages to you
	    URL        Associate a URL with your nickname
	    EMAIL      Associate an E-mail address with your nickname
	    INFO       Set text to be shown with the INFO command
	    KILL       Turn kill protection on or off
	    SECURE     Turn nickname security on or off
	    PRIVATE    Prevent your nickname from appearing in a
	                   /msg %S LIST
	    NOOP       Prevent your nickname from being added to
	                   channel access lists
	    HIDE       Hide certain pieces of nickname information
	    TIMEZONE   Set the time zone to use for displaying times
NICK_HELP_SET_OPTION_MAINNICK
	    MAINNICK   Set the nickname shown in channel access lists
NICK_HELP_SET_END
	
	In order to use this command, you must first identify
	with your password (/msg %S HELP IDENTIFY for more
	information).
	
	Type /msg %S HELP SET option for more information
	on a specific option.

NICK_HELP_SET_PASSWORD
	Syntax: SET PASSWORD new-password
	
	Changes the password used to identify you as the nickname's
	owner.

NICK_HELP_SET_LANGUAGE
	Syntax: SET LANGUAGE number
	
	Changes the language Services uses when sending messages to
	you (for example, when responding to a command you send).
	number should be chosen from the following list of
	supported languages:
	

NICK_HELP_SET_URL
	Syntax: SET URL url
	
	Associates the given URL with your nickname.  This URL
	will be displayed whenever someone requests information
	on your nickname with the INFO command.

NICK_HELP_SET_EMAIL
	Syntax: SET EMAIL address
	
	Associates the given E-mail address with your nickname.
	This address will be displayed whenever someone requests
	information on your nickname with the INFO command.

NICK_HELP_SET_INFO
	Syntax: SET INFO text
	
	Associates the given text with your nickname; it will be
	displayed whenever someone requests information on your
	nickname with the INFO command.

NICK_HELP_SET_KILL
	Syntax: SET KILL {ON | QUICK | IMMED | OFF}
	
	Turns the automatic kill protection option for your nickname
	on or off.  With kill protection on, if another user
	tries to take your nick, they will be given one minute to
	change to another nick, after which they will be forcibly
	removed from IRC by %S.
	
	If you select QUICK, the user will be given only 20 seconds
	to change their nickname instead of the usual 60.  If you
	select IMMED, the user will be killed immediately without
	being warned first or given a chance to change their nick;
	please do not use this option unless necessary.  Also, your
	network's administrators may have disabled this option.

NICK_HELP_SET_SECURE
	Syntax: SET SECURE {ON | OFF}
	
	Turns %S's security features on or off for your
	nickname.  With SECURE set, you must enter your password
	before you will be recognized as the owner of the nick,
	regardless of whether your address is on the access
	list.  However, if you are on the access list, %S
	will not auto-kill you regardless of the setting of the
	KILL option.

NICK_HELP_SET_PRIVATE
	Syntax: SET PRIVATE {ON | OFF}
	
	Turns %S's privacy option on or off for your nickname.
	With PRIVATE set, your nickname will not appear in
	nickname lists generated with %S's LIST command.
	(However, anyone who knows your nickname can still get
	information on it using the INFO command.)

NICK_HELP_SET_NOOP
	Syntax: SET NOOP {ON | OFF}
	
	Turns %S's no-op option on or off for your nickname.
	Setting the NOOP option prevents your nickname from being
	added to channel access lists.  However, if your nickname is
	already on a channel's access list, it will not be removed
	even if you turn this option on.

NICK_HELP_SET_HIDE
	Syntax: SET HIDE {EMAIL | USERMASK | QUIT} {ON | OFF}
	
	Allows you to prevent certain pieces of information from
	being displayed when someone does a %S INFO on your
	nickname.  You can hide your E-mail address (EMAIL), last
	seen user@host mask (USERMASK), and last quit message
	(QUIT).  The second parameter specifies whether the
	information should be displayed (OFF) or hidden (ON).

NICK_HELP_SET_TIMEZONE
	Syntax: SET TIMEZONE {UTC-offset | time-zone | DEFAULT}
	
	Sets the time zone to use for displaying times (such as a
	nickname or channel's last used time), instead of using the
	time zone Services is operating in; the DEFAULT setting can
	be used to return to Services' time zone.  The time zone can
	be specified in one of two ways:
	
	   - As a UTC offset, the time difference between your local
	     time and UTC (Universal Coordinated Time, also known as
	     GMT, Greenwich Mean Time).  The offset is given as a
	     "+" or "-" to indicate ahead of (east) or behind (west)
	     UTC, followed by either a whole number of hours or a
	     string in the form "hh:mm".
	
	   - As a time zone name, such as "GMT" or "EDT".  Not all
	     time zone names are supported by Services, and some
	     countries' time zones have the same names, so if you
	     get an error or the time displayed by Services is
	     wrong, use a UTC offset instead.
	
	Note that Services will not automatically adjust for
	daylight savings time (summer time); you will have to adjust
	the time zone manually.
	
	Examples:
	
	    SET TIMEZONE -5
	        Set the time zone to 5 hours behind UTC.
	
	    SET TIMEZONE +3:30
	        Set the time zone to 3 hours, 30 minutes ahead of UTC.
	
	    SET TIMEZONE JST
	        Set the time zone to Japan Standard Time (UTC+9:00).

NICK_HELP_SET_MAINNICK
	Syntax: SET MAINNICK nickname
	
	When you have several nicknames linked together, allows you
	to select which one is displayed in channel access lists.
	If you later unlink this nick, then a different one will be
	selected automatically.

NICK_HELP_UNSET
	Syntax: UNSET {URL | EMAIL | INFO}
	
	Allows you to clear the URL (URL), E-mail address (EMAIL),
	or information text (INFO) associated with your nickname.

NICK_HELP_UNSET_REQ_EMAIL
	Syntax: UNSET {URL | INFO}
	
	Allows you to clear the URL (URL) or information text (INFO)
	associated with your nickname.

NICK_HELP_RECOVER
	Syntax: RECOVER nickname [password]
	
	Allows you to recover your nickname if someone else has
	taken it; this does the same thing that %S does
	automatically if someone tries to use a kill-protected
	nickname.
	
	When you give this command, %S will bring a fake user
	online with the same nickname as the user you're trying to
	recover your nick from, causing the IRC servers to
	disconnect the other user.  This fake user will remain
	online for %s to ensure that the other user does not
	immediately reconnect; after that time, you can reclaim
	your nick.  Alternatively, use the RELEASE command
	(/msg %S HELP RELEASE) to get the nick back sooner.
	
	In order to use the RECOVER command for a nickname, you must
	supply the correct password for the nickname, or you must
	already have identified to a nick linked to the given nick.

NICK_HELP_RELEASE
	Syntax: RELEASE nickname [password]
	
	Instructs %S to remove any hold on your nickname
	caused by automatic kill protection or use of the RECOVER
	command.  By default, such holds last for %s; this
	command gets rid of them sooner.
	
	In order to use the RELEASE command for a nickname, you must
	supply the correct password for the nickname, or you must
	already have identified to a nick linked to the given nick.

NICK_HELP_GHOST
	Syntax: GHOST nickname [password]
	
	Terminates a "ghost" IRC session using your nickname.  A
	"ghost" session is one which is not actually connected, but
	which the IRC server believes is still online for one reason
	or another.  Typically, this happens if your computer
	crashes or your Internet or modem connection goes down while
	you're on IRC.
	
	In order to use the GHOST command for a nickname, you must
	supply the correct password for the nickname, or you must
	already have identified to a nick linked to the given nick.

NICK_HELP_INFO
	Syntax: INFO nickname [ALL]
	
	Displays information about the given nickname, such as
	the nickname's owner, last seen address and time, and
	options.  If you are identified for the nick you're
	getting information for and ALL is specified, you will
	be shown all the information, regardless of whether
	it's hidden or not.
NICK_HELP_INFO_AUTH
	
	If the nickname's E-mail address has not yet been confirmed
	with the AUTH command, it will not be shown to anyone except
	the nickname owner and Services administrators, regardless
	of the nickname's SET HIDE EMAIL setting.

NICK_HELP_LISTCHANS
	Syntax: LISTCHANS
	
	Lists the names and the total number of channels you have
	registered under your nickname.

NICK_HELP_LIST
	Syntax: LIST [+start] pattern
	
	Lists all registered nicknames for which either the nickname
	itself or the last seen address (in user@host format) match
	the given pattern.  Nicks with the PRIVATE option set will
	not be displayed.  If the start parameter is given, it
	specifies how many matches to skip before displaying results
	(for example, +50 would cause the first 50 matches to be
	skipped).
	
	Examples:
	
	    LIST joeuser@foo.com
	        Lists all registered nicknames which were last used
	        by joeuser@foo.com.
	
	    LIST *Bot*
	        Lists all registered nicknames with Bot in their
	        names (case insensitive).
	
	    LIST *@*.bar.org
	        Lists all registered nicknames last used by users in
	        the bar.org domain.
NICK_HELP_LIST_OPERSONLY
	
	Only IRC operators may use this command.

NICK_HELP_LISTEMAIL
	Syntax: %s
	
	Lists registered nicknames like the LIST command, but
	matches on E-mail addresses instead of user@host addresses.
	See the LIST command help (/msg %S HELP LIST) for
	more information.

NICK_HELP_STATUS
	Syntax: STATUS nickname...
	
	Returns whether the user using the given nickname is
	recognized as the owner of the nickname.  The response has
	this format:
	
	    nickname status-code
	
	where nickname is the nickname sent with the command, and
	status-code is one of the following:
	
	    0 - no such user online or nickname not registered
	    1 - user not recognized as nickname's owner
	    2 - user recognized as owner via access list only
	    3 - user recognized as owner via password identification
	
	Up to sixteen nicknames may be sent with each command; the
	rest will be ignored.  No error message is generated if no
	nickname is given.

NICK_HELP_AJOIN
	Syntax: AJOIN ADD channel
	        AJOIN DEL channel
	        AJOIN LIST
	
	Modifies or displays the autojoin list for your nickname.
	This is the list of channels that you would like to
	automatically join, and takes effect whenever you identify
	to %S.  You may only put channel names starting with
	"#" in the autojoin list (local channels cannot be added).
	
NICK_HELP_AJOIN_END
	Note that the IRC server may prohibit you from entering some
	channels on the autojoin list, such as channels that have a
	channel key set (mode +k) or are invite-only (mode +i).
NICK_HELP_AJOIN_END_CHANSERV
	Note that the IRC server may prohibit you from entering some
	channels on the autojoin list, such as channels that have a
	channel key set (mode +k) or are invite-only (mode +i).
	However, if you have access to the %s INVITE command
	on a registered channel, %S will automatically invite
	you into the channel.

NICK_OPER_HELP_COMMANDS
	
	The following commands are available to Services admins:
	
	    DROPNICK     Drop any nickname
NICK_OPER_HELP_COMMANDS_DROPEMAIL
	    DROPEMAIL    Drop all nicknames matching an E-mail address
NICK_OPER_HELP_COMMANDS_GETPASS
	    GETPASS      Show the password for a nickname
	                     (only if encryption is not active)
NICK_OPER_HELP_COMMANDS_FORBID
	    FORBID       Prevent a nickname from being used/registered
	    SUSPEND      Prevent a nickname from being identified for
	    UNSUSPEND    Cancel a nickname suspension
NICK_OPER_HELP_COMMANDS_SETAUTH
	    SETAUTH      Set a new authentication code for a nickname
	    GETAUTH      Show the authentication code for a nickname
	    CLEARAUTH    Clear the authentication code for a nickname
NICK_OPER_HELP_COMMANDS_END
	
	Services admins can also use the following commands on any
	nickname without needing to identify for it:
	
	    SET, ACCESS LIST, INFO ALL
	
	See the help for each command for specific details.

NICK_OPER_HELP_DROPNICK
	Syntax: DROPNICK nickname
	
	Drops the given nickname from the %S database, as well
	as any linked nicknames.
	
	Limited to Services admins.

NICK_OPER_HELP_DROPEMAIL
	Syntax: DROPEMAIL pattern
	        DROPEMAIL-CONFIRM pattern
	
	Drops all nicknames with an E-mail address matching the
	given pattern.  If pattern is the single character
	"-", drops all nicknames without an E-mail address.
	
	To prevent accidentally dropping nicknames via this command,
	it must be given twice: first using the DROPEMAIL command
	name, then using DROPEMAIL-CONFIRM with the same pattern.
	The DROPEMAIL-CONFIRM command must be given within %s
	of and by the same person who gave the DROPEMAIL command.
	If more than 4 DROPEMAIL commands are given in a row, the
	oldest will be discarded and no longer recognized for a
	DROPEMAIL-CONFIRM command.
	
	Limited to Services admins.

NICK_OPER_HELP_SET
	
	Services admins can also set the option NOEXPIRE, with
	which nicknames can be prevented from expiring.
	Additionally, Services admins can set options for any
	nickname without entering a password, using the format
	SET !nickname option parameters.

NICK_OPER_HELP_SET_NOEXPIRE
	Syntax: SET [!nickname] NOEXPIRE {ON | OFF}
	
	Sets whether the given nickname will expire.  Setting this
	to ON prevents the nickname from expiring.  If no
	nickname is given, sets the no-expire flag for your nick.
	
	Limited to Services admins.

NICK_OPER_HELP_UNSET
	
	Services admins may use the UNSET command on any nickname by
	adding !nickname before the option name, as with the SET
	command.

NICK_OPER_HELP_OLD_UNLINK
	
	Services admins may unlink any nickname without entering a
	password.

NICK_OPER_HELP_UNLINK
	Syntax: UNLINK nickname [FORCE]
	
	Removes a link (alias) to your nickname created with the
	LINK command.  You cannot unlink the nickname you are
	currently using; change to another linked nick first before
	using this command.
	
	If you unlink the "main nickname" you have selected using
	the SET MAINNICK command, your current nickname will
	automatically be selected as the main nickname.
	
	Services admins may add the FORCE option, which allows
	removal of a link to a nickname other than their own.

NICK_OPER_HELP_OLD_LISTLINKS
	Syntax: LISTLINKS nickname
	
	Lists nicknames which are linked to the given nickname.
	
	Limited to Services admins.

NICK_OPER_HELP_LISTLINKS
	Syntax: LISTLINKS [nickname]
	
	Lists all nicknames which are linked to your nick.  The main
	nickname for the group of linked nicks will be indicated by
	a "*" to the left of the nick.  Services operators may list
	links for any nick by specifying the nick in the command.

NICK_OPER_HELP_ACCESS
	
	Services admins may give a nickname parameter to the LIST
	command to display the access list for any nickname.

NICK_OPER_HELP_INFO
	
	Services admins may use the ALL parameter with any nickname.

NICK_OPER_HELP_LISTCHANS
	
	Services admins may list channels for any nickname by
	giving the nickname with the command (for example,
	/msg %S LISTCHANS OtherNick).

NICK_OPER_HELP_LIST
	Syntax: LIST [+start] pattern [FORBIDDEN] [NOEXPIRE] [SUSPENDED]
	
	Lists all registered nicknames for which either the nickname
	itself or the last seen address (in user@host format) match
	the given pattern.  Nicks with the PRIVATE option set will
	only be displayed to Services admins.  For Services admins,
	nicks with the NOEXPIRE option set will have a ! prepended
	to the nickname and those that are suspended will be
	prepended by a *.
	
	If the FORBIDDEN, SUSPENDED or NOEXPIRE options are given,
	only nicks which, respectively, are forbidden, suspended, or
	have the NOEXPIRE option set will be displayed.  The options
	can be used in any order and in any combination.  These
	options are limited to Services admins.
NICK_OPER_HELP_LIST_AUTH
	Syntax: LIST [+start] pattern [FORBIDDEN] [NOEXPIRE] [SUSPENDED] [NOAUTH]
	
	Lists all registered nicknames for which either the nickname
	itself or the last seen address (in user@host format) match
	the given pattern.  Nicks with the PRIVATE option set will
	only be displayed to Services admins.  For Services admins,
	nicks with the NOEXPIRE option set will have a ! prepended
	to the nickname, those that are suspended will be prepended
	by a *, and those that have not yet been authenticated (with
	the AUTH command) will be prepended by a ?.
	
	If the FORBIDDEN, SUSPENDED, NOEXPIRE, or NOAUTH options are
	given, only nicks which, respectively, are forbidden,
	suspended, have the NOEXPIRE option set, or have not yet
	been authenticated will be displayed.  The options can be
	used in any order and in any combination.  These options are
	limited to Services admins.
NICK_OPER_HELP_LIST_END
	
	If the start parameter is given, it specifies how many
	matches to skip before displaying results (for example, +50
	would cause the first 50 matches to be skipped).
	
	Examples:
	
	    LIST joeuser@foo.com
	        Lists all registered nicknames which were last used
	        by joeuser@foo.com.
	
	    LIST *Bot*
	        Lists all registered nicknames with Bot in their
	        names (case insensitive).
	
	    LIST * NOEXPIRE
	        Lists all registered nicknames which have been set
	        to not expire.

NICK_OPER_HELP_GETPASS
	Syntax: GETPASS nickname
	
	Returns the password for the given nickname.  Note that
	whenever this command is used, a message including the
	person who issued the command and the nickname it was used
	on will be logged and sent out as a WALLOPS/GLOBOPS.
	
	If encryption is in use, this command may not be available
	depending on the type of encryption used.
	
	Limited to Services admins.

NICK_OPER_HELP_FORBID
	Syntax: FORBID nickname
	
	Disallows a nickname from being registered or used by
	anyone.  May be cancelled by dropping the nickname.
	
	Limited to Services admins.

NICK_OPER_HELP_SUSPEND
	Syntax: SUSPEND [+expiry] nickname reason
	
	Prevents a nickname, and all nicknames linked to it, from
	being used or identified for by anyone.  The nickname can be
	unsuspended with the UNSUSPEND command, which will allow it
	to be used as before.  The expiry parameter is interpreted
	in the same manner as for the %s AKILL command.
	
	Limited to Services admins.

NICK_OPER_HELP_UNSUSPEND
	Syntax: UNSUSPEND nickname
	
	Allows people to use and identify for a previously suspended
	nickname.
	
	Limited to Services admins.

NICK_OPER_HELP_AJOIN
	
	Services admins can list the autojoin list for any nickname
	with the format LIST nickname.

NICK_OPER_HELP_SETAUTH
	Syntax: SETAUTH nickname
	
	Sets a new authentication code for the given nickname.  The
	nickname's owner will have to use the AUTH command for the
	nickname before being allowed to identify again.  If the
	nickname is currently in use, a notice will be sent to the
	user of the nickname and they will no longer be treated as
	identified for the nickname.
	
	Limited to Services admins.

NICK_OPER_HELP_GETAUTH
	Syntax: GETAUTH nickname
	
	Retrieves the authentication code associated with nickname,
	if one exists.
	
	Limited to Services admins.

NICK_OPER_HELP_CLEARAUTH
	Syntax: CLEARAUTH nickname
	
	Clears any authentication code associated with nickname,
	allowing the nickname to be used normally.
	
	Limited to Services admins.


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

# Real help messages start here
CHAN_HELP
	%S allows you to register and control various
	aspects of channels.  %S can often prevent
	malicious users from "taking over" channels by limiting
	who is allowed channel operator privileges.  Type
	/msg %S HELP COMMANDS for a list of %S commands;
	to use a command, type /msg %S command, or for more
	information on a command, type /msg %S HELP command.
CHAN_HELP_EXPIRES
	
	Note that any channel which is not used for %s
	(i.e. which no user on the channel's access list enters
	for that period of time) will be automatically dropped.

CHAN_HELP_COMMANDS
	The following commands can be used with %S:
	
	    REGISTER   Register a channel
	    IDENTIFY   Identify yourself with your password
	    DROP       Cancel the registration of a channel
	    SET        Set channel options and information
	    UNSET      Clear channel information
	    INFO       Show channel options and information
CHAN_HELP_COMMANDS_LIST
	    LIST       List registered channels
CHAN_HELP_COMMANDS_AKICK
	    AKICK      Maintain the AutoKick list
CHAN_HELP_COMMANDS_LEVELS
	    ACCESS     Maintain the overall channel access list
	    LEVELS     Fine-tune channel privilege levels
CHAN_HELP_COMMANDS_XOP
	    SOP        Maintain the SuperOp list
	    AOP        Maintain the AutoOp list
CHAN_HELP_COMMANDS_HOP
	    HOP        Maintain the AutoHalfop list
CHAN_HELP_COMMANDS_XOP_2
	    VOP        Maintain the AutoVoice list
	    NOP        Maintain the NeverOp list
CHAN_HELP_COMMANDS_OPVOICE
	    OP         Give a user chanop status (+o)
	    DEOP       Remove chanop status (+o)
	    VOICE      Give a user voice status (+v)
	    DEVOICE    Remove voice status (+v)
CHAN_HELP_COMMANDS_HALFOP
	    HALFOP     Give a user halfop status (+h)
	    DEHALFOP   Remove halfop status (+h)
CHAN_HELP_COMMANDS_PROTECT
	    PROTECT    Give a user protected status (+a)
	    DEPROTECT  Remove protected status (+a)
CHAN_HELP_COMMANDS_INVITE
	    INVITE     Invite yourself to a channel
	    UNBAN      Unban yourself from a channel
	    KICK       Kick a user from a channel
	    TOPIC      Change a channel's topic
	    CLEAR      Clear channel modes or mass-kick users
	    STATUS     Return a user's access level on a channel

CHAN_HELP_REGISTER
	Syntax: REGISTER channel password description
	
	Registers a channel in the %S database.  In order
	to use this command, you must first be a channel operator
	on the channel you're trying to register.  The password
	is used with the IDENTIFY command to allow you to
	make changes to the channel settings at a later time.
	The last parameter, which must be included, is a
	general description of the channel's purpose.
	
	When you register a channel, you are recorded as the
	"founder" of the channel.  The channel founder is allowed
	to change all of the channel settings for the channel;
	%S will also automatically give the founder
	channel-operator privileges when s/he enters the channel.
	
	NOTICE: In order to register a channel, you must have
	first registered and identified for your nickname.  If you
	haven't, /msg %s HELP for information on how to do so.
CHAN_HELP_REGISTER_ADMINONLY
	
	Limited to Services admins.

CHAN_HELP_IDENTIFY
	Syntax: IDENTIFY channel password
	
	Authenticates you to %S as the founder of the given
	channel.  Many commands require you to use this command
	before using them.  The password should be the same one
	you sent with the REGISTER command.

CHAN_HELP_DROP
	Syntax: DROP channel password
	
	Unregisters the named channel.  Can only be used by
	channel founder.  The password must be given to avoid
	accidentally dropping the channel.

CHAN_HELP_SET
	Syntax: SET channel option parameters
	
	Allows the channel founder to set various channel options
	and other information.  The founder must use the IDENTIFY
	command before using SET.
	
	Available options:
	
	    FOUNDER     Set the founder of a channel
	    SUCCESSOR   Set the successor for a channel
	    PASSWORD    Set the founder password
	    DESC        Set the channel description
	    URL         Associate a URL with the channel
	    EMAIL       Associate an E-mail address with the channel
	    ENTRYMSG    Set a message to be sent to users when they
	                   enter the channel
	    KEEPTOPIC   Retain topic when channel is not in use
	    TOPICLOCK   Topic can only be changed with TOPIC
	    MLOCK       Lock channel modes on or off
	    PRIVATE     Hide channel from LIST command
	    RESTRICTED  Restrict access to the channel
	    SECURE      Activate %S security features
	    SECUREOPS   Stricter control of chanop status
	    LEAVEOPS    Do not de-op users on channel entry
	    OPNOTICE    Send a notice when OP/VOICE commands are used
	    ENFORCE     Enforce auto-op, auto-voice status
	    MEMO-RESTRICTED
	                Limit who can send memos to the channel
	
	Type /msg %S HELP SET option for more information
	on a particular option.

CHAN_HELP_SET_FOUNDER
	Syntax: SET channel FOUNDER nickname
	
	Changes the founder of a channel.  The new nickname must
	be a registered one.

CHAN_HELP_SET_SUCCESSOR
	Syntax: SET channel SUCCESSOR nickname
	
	Changes the successor of a channel.  If the founder's
	nickname expires or is dropped while the channel is still
	registered, the successor will become the new founder of
	the channel.  However, if the successor already has too
	many channels registered (%d), the channel will be dropped
	instead, just as if no successor had been set.  The
	nickname must be a registered one.

CHAN_HELP_SET_PASSWORD
	Syntax: SET channel PASSWORD password
	
	Sets the password used to identify as the founder of the
	channel.

CHAN_HELP_SET_DESC
	Syntax: SET channel DESC description
	
	Sets the description for the channel, which shows up with
	the LIST and INFO commands.

CHAN_HELP_SET_URL
	Syntax: SET channel URL url
	
	Associates the given URL with the channel.  This URL will
	be displayed whenever someone requests information on the
	channel with the INFO command.

CHAN_HELP_SET_EMAIL
	Syntax: SET channel EMAIL address
	
	Associates the given E-mail address with the channel.
	This address will be displayed whenever someone requests
	information on the channel with the INFO command.

CHAN_HELP_SET_ENTRYMSG
	Syntax: SET channel ENTRYMSG message
	
	Sets the message which will be sent via /NOTICE to users
	when they enter the channel.

CHAN_HELP_SET_KEEPTOPIC
	Syntax: SET channel KEEPTOPIC {ON | OFF}
	
	Enables or disables the topic retention option for a
	channel.  When topic retention is set, the topic for the
	channel will be remembered by %S even after the
	last user leaves the channel, and will be restored the
	next time the channel is created.

CHAN_HELP_SET_TOPICLOCK
	Syntax: SET channel TOPICLOCK {ON | OFF}
	
	Enables or disables the topic lock option for a channel.
	When topic lock is set, %S will not allow the
	channel topic to be changed except via the TOPIC command.

CHAN_HELP_SET_MLOCK
	Syntax: SET channel MLOCK modes
	
	Sets the mode-lock parameter for the channel.  %S
	allows you to define certain channel modes to be always
	on (or off).  All binary modes (n, s, t, k, l, and so on)
	can be locked; each mode can be locked on, locked off, or
	not locked.  The modes parameter is constructed exactly
	the same way as a /MODE command; that is, modes followed by
	a + are locked on, and modes followed by a - are locked off.
	
	Warning:  If you set a mode-locked key, as in the second
	example below, you should also set the RESTRICTED option for
	the channel (see HELP SET RESTRICTED), or anyone entering
	the channel when it is empty will be able to see the key!
	MLOCK +i will likewise not prevent users from entering the
	channel when it is empty without being invited.
	
	Examples:
	
	    SET #channel MLOCK +nt-ikl
	        Forces modes n and t on, and modes i, k, and l off.
	        All other modes are left free to be either on or off.
	
	    SET #channel MLOCK +knst-ilmp my-key
	        Forces modes k, n, s, and t on, and modes i, l, m,
	        and p off.  Also forces the channel key to be
	        "my-key".
	
	    SET #channel MLOCK +
	        Removes the mode lock; all channel modes are free
	        to be either on or off.

CHAN_HELP_SET_HIDE
	Syntax: SET channel HIDE {EMAIL | TOPIC | MLOCK} {ON | OFF}
	
	Allows you to prevent certain pieces of information from
	being displayed when someone does a %S INFO on the
	given channel.  You can hide the channel's E-mail address
	(EMAIL), last topic (TOPIC), and mode lock (MLOCK).
	The second parameter specifies whether the information should
	be displayed (OFF) or hidden (ON).
	
	Note that the topic will always be hidden if the channel is
	mode-locked +p or +s.

CHAN_HELP_SET_PRIVATE
	Syntax: SET channel PRIVATE {ON | OFF}
	
	Enables or disables the private option for a channel.
	When private is set, a /msg %S LIST will not
	include the channel in any lists.

CHAN_HELP_SET_RESTRICTED
	Syntax: SET channel RESTRICTED {ON | OFF}
	
	Enables or disables the restricted access option for a
	channel.  When restricted access is set, users who are not
	on the access list or who would normally be disallowed from
	having channel operator privileges will be kicked and banned
	from the channel.

CHAN_HELP_SET_SECURE
	Syntax: SET channel SECURE {ON | OFF}
	
	Enables or disables %S's security features for a
	channel. When SECURE is set, only users who have registered
	their nicknames with %s and IDENTIFY'd with their
	password will be given privileges on the channel as
	controlled by the access list.  If the RESTRICTED option is
	also set, users will not be able to join the channel until
	they identify to %s.

CHAN_HELP_SET_SECUREOPS
	Syntax: SET channel SECUREOPS {ON | OFF}
	
	Enables or disables the secure ops option for a channel.
	When secure ops is set, users who are not on the access list
	will not be allowed chanop status.

CHAN_HELP_SET_LEAVEOPS
	Syntax: SET channel LEAVEOPS {ON | OFF}
	
	Enables or disables the leave ops option for a channel.
	When leave ops is set, %S will leave server ops (+o for
	the first person in the channel) in place even if the user
	would not normally get auto-opped.  The SECUREOPS option
	will still be enforced.
	Notice:  This can allow users to "steal" ops during a
	netsplit.

CHAN_HELP_SET_OPNOTICE
	Syntax: SET channel OPNOTICE {ON | OFF}
	
	Enables or disables the op-notice option for a channel.
	When op-notice is set, %S will send a notice to the
	channel whenever the OP, VOICE, or related commands are used
	for a user in the channel.

CHAN_HELP_SET_ENFORCE
	Syntax: SET channel ENFORCE {ON | OFF}
	
	Enables or disables the enforce option for a channel.
	When enforce is set, %S will enforce the auto-op and
	auto-voice levels for all users.  For example, if a user
	with auto-op privileges is deopped, %S will
	automatically op the user again, and %S will also
	not allow the DEOP command to be used on that user.

CHAN_HELP_SET_MEMO_RESTRICTED
	Syntax: SET channel MEMO-RESTRICTED {ON | OFF}
	
	Enables or disables the memo restriction option for a
	channel.  When memo restriction is set, only users with
	privileges to receive channel memos will be able to send
	memos to the channel.

CHAN_HELP_UNSET
	Syntax: UNSET channel {SUCCESSOR | URL | EMAIL | ENTRYMSG}
	
	Clears the given setting for a channel.

CHAN_HELP_SOP
	Syntax: SOP channel ADD nickname
	        SOP channel DEL nickname
	        SOP channel LIST [+start] [mask]
	        SOP channel COUNT
	
CHAN_HELP_SOP_MID1
	Maintains the SuperOp (SOP) list for a channel.  Users with
	SOP status are automatically opped (set mode +o) when they
	join the channel.
CHAN_HELP_SOP_MID1_CHANPROT
	Maintains the SuperOp (SOP) list for a channel.  Users with
	SOP status are automatically opped (set mode +o) and
	protected (mode +a) when they join the channel.
CHAN_HELP_SOP_MID2
	
	An SOP can modify the AOP, VOP, and NOP lists.  Only a
	channel's founder can modify the SOP list.
CHAN_HELP_SOP_MID2_HALFOP
	
	An SOP can modify the AOP, HOP, VOP, and NOP lists.  Only a
	channel's founder can modify the SOP list.
CHAN_HELP_SOP_END
	
	The ADD, DEL, LIST, and COUNT subcommands function the
	same way as for the AOP command.

CHAN_HELP_AOP
	Syntax: AOP channel ADD nickname
	        AOP channel DEL nickname
	        AOP channel LIST [+start] [mask]
	        AOP channel COUNT
	
	Maintains the AutoOp (AOP) list for a channel.  Users with
	AOP status are automatically opped (set mode +o) when they
	join the channel.
CHAN_HELP_AOP_MID
	
	An AOP can modify the VOP and NOP lists; SOPs can modify the
	AOP list.
CHAN_HELP_AOP_MID_HALFOP
	
	An AOP can modify the HOP, VOP, and NOP lists; SOPs can
	modify the AOP list.
CHAN_HELP_AOP_END
	
	The AOP ADD command adds a user to the AOP list, and the
	AOP DEL command removes a user from the AOP list.  Note that
	a user can only be on one channel access list at once; if
	the user given in an AOP ADD command is already on another
	channel access list, such as the VOP list, they will be
	removed from that list first.  (If you would not be allowed
	to modify that list--for example, if the user is already on
	the SOP list and you are not the channel founder--then the
	AOP ADD command will fail.)
	
	The AOP LIST command displays the AOP list.  If a wildcard
	mask is given, only those entries matching the mask are
	displayed.  If the start parameter is given, it specifies
	how many matches to skip before displaying results (for
	example, +50 would cause the first 50 matches to be
	skipped).
	
	The AOP COUNT command shows the number of entries there are
	are on the AOP list.

CHAN_HELP_HOP
	Syntax: HOP channel ADD nickname
	        HOP channel DEL nickname
	        HOP channel LIST [+start] [mask]
	        HOP channel COUNT
	
	Maintains the HalfOp (HOP) list for a channel.  Users with
	HOP status are automatically halfopped (set mode +h) when
	they join the channel.
	
	An HOP can modify the VOP and NOP lists; SOPs and AOPs can
	modify the HOP list.
	
	The ADD, DEL, LIST, and COUNT subcommands function the
	same way as for the AOP command.

CHAN_HELP_VOP
	Syntax: VOP channel ADD nickname
	        VOP channel DEL nickname
	        VOP channel LIST [+start] [mask]
	        VOP channel COUNT
	
	Maintains the VoiceOp (VOP) list for a channel.  Users with
	VOP status are automatically voiced (set mode +v) when they
	join the channel.
	
	The ADD, DEL, LIST, and COUNT subcommands function the
	same way as for the AOP command.

CHAN_HELP_NOP
	Syntax: NOP channel ADD nickname
	        NOP channel DEL nickname
	        NOP channel LIST [+start] [mask]
	        NOP channel COUNT
	
	Maintains the NeverOp (NOP) list for a channel.  Users with
	NOP status are not permitted to be given channel operator
	(mode +o) status, and ChanServ will remove mode +o from such
	users whenever it is given to them.
	
	The ADD, DEL, LIST, and COUNT subcommands function the
	same way as for the AOP command.

CHAN_HELP_ACCESS
	Syntax: ACCESS channel ADD nickname level
	        ACCESS channel DEL nickname
	        ACCESS channel LIST [+start] [mask]
	        ACCESS channel LISTLEVEL [+start] level
	        ACCESS channel COUNT
	
	Maintains the access list for a channel.  The access
	list specifies which users are allowed chanop status or
	access to %S commands on the channel.  Different
	user levels allow for access to different subsets of
	privileges; /msg %S HELP ACCESS LEVELS for more
	specific information.  Any nickname not on the access list
	has a user level of 0.
	
	The ACCESS ADD command adds the given nickname to the access
	list with the given user level; if the nick is already
	present on the list, its access level is changed to the
	level specified in the command.  The level specified must be
	less than that of the user giving the command, and if the
	nick is already on the access list, the current access level
	of that nick must be less than the access level of the user
	giving the command.
	
	The ACCESS DEL command removes the given nickname from the
	access list.  Users can only remove nicknames with an access
	level lower than their own.
	
	The ACCESS LIST command displays the access list.  If
	a wildcard mask is given, only those entries matching the
	mask are displayed.  If the start parameter is given, it
	specifies how many matches to skip before displaying results
	(for example, +50 would cause the first 50 matches to be
	skipped).
	
	The ACCESS LISTLEVEL command is similar to ACCESS LIST, but
	displays only entries with the given access level.  The
	level can be specified as a range: LISTLEVEL 50-100, for
	example, lists all users with access levels between 50 and 100
	inclusive.
	
	The ACCESS COUNT command returns the number of entries
	there are on the access list.
CHAN_HELP_ACCESS_XOP
	
	Note that the access list may also be manipulated with the
	SOP, AOP, VOP, and NOP commands, which correspond to levels
	of %d, %d, %d, and %d respectively.
CHAN_HELP_ACCESS_XOP_HALFOP
	
	Note that the access list may also be manipulated with the
	SOP, AOP, HOP, VOP, and NOP commands, which correspond to
	levels of %d, %d, %d, %d, and %d respectively.

CHAN_HELP_ACCESS_LEVELS
	Channel privilege levels
	
	By default, the following privilege levels are defined:
	
	   Founder   Full access to %S functions; automatic
	                 opping upon entering channel.  Note that
	                 only one person may have founder status (it
	                 cannot be given using the ACCESS command).
	      %4d   Access to AKICK command; automatic opping.
	      %4d   Automatic opping.
CHAN_HELP_ACCESS_LEVELS_HALFOP
	      %4d   Automatic halfopping.
CHAN_HELP_ACCESS_LEVELS_END
	      %4d   Automatic voicing.
	         0   No special privileges; can be opped by other
	                 ops (unless SECUREOPS is set).
	        -1   May not be opped.
	      -100   May not join the channel.
	
	These levels (except the last two) may be changed, or new
	ones added, using the LEVELS command; type
	/msg %S HELP LEVELS for details.

CHAN_HELP_LEVELS
	Syntax: LEVELS channel SET type level
	        LEVELS channel {DIS | DISABLE} type
	        LEVELS channel LIST
	        LEVELS channel RESET
	
	The LEVELS command allows fine control over the meaning of
	the numeric access levels used by the ACCESS command.  With
	this command, you can define the access level required for
	most of %S's functions.  (The SET FOUNDER and SET PASSWORD
	commands, as well as this command, are always restricted to
	the channel founder.)
CHAN_HELP_LEVELS_XOP
	
	Caution:  This command is for advanced users only!  Changing
	level settings with this command may cause the SOP, AOP,
	VOP, and NOP commands to stop working correctly.
CHAN_HELP_LEVELS_XOP_HOP
	
	Caution:  This command is for advanced users only!  Changing
	level settings with this command may cause the SOP, AOP,
	HOP, VOP, and NOP commands to stop working correctly.
CHAN_HELP_LEVELS_END
	
	LEVELS SET allows the access level for a function or group of
	functions to be changed.  LEVELS DISABLE (or DIS for short)
	disables an automatic feature or disallows access to a
	function by anyone other than the channel founder.
	LEVELS LIST shows the current levels for each function or
	group of functions.  LEVELS RESET resets the levels to the
	default levels of a newly-created channel (see
	HELP ACCESS LEVELS).
	
	For a list of the features and functions whose levels can be
	set, see HELP LEVELS DESC.

CHAN_HELP_LEVELS_DESC
	The following feature/function names are understood:

CHAN_HELP_AKICK
	Syntax: AKICK channel ADD mask [reason]
	        AKICK channel DEL mask
	        AKICK channel LIST [+start] [mask]
	        AKICK channel VIEW [+start] [mask]
	        AKICK channel COUNT
	        AKICK channel ENFORCE
	
	Maintains the autokick list for a channel.  If a user on the
	autokick list attempts to join the channel, %S will
	ban that user from the channel, then kick the user.
	
	The AKICK ADD command adds the given user@host or
	nick!user@host mask to the autokick list.  If a reason is
	given with the command, that reason will be used in the kick
	message when the user is kicked.
	
	The AKICK DEL command removes the given mask from the
	autokick list.  It does not, however, remove any bans placed
	by an autokick; those must be removed manually.
	
	The AKICK LIST command displays the autokick list, or
	optionally only those autokick entries which match the given
	mask.  AKICK VIEW is similar, but shows more details.
	If the start parameter is given to either command, it
	specifies how many matches to skip before displaying results
	(for example, +50 would cause the first 50 matches to be
	skipped).
	
	The AKICK COUNT command shows how many entries there are on
	the autokick list.
	
	The AKICK ENFORCE command causes %S to enforce the current
	autokick list by removing those users who match an autokick
	mask.
	
	By default, limited to users with %s access and above
	on the channel.

CHAN_HELP_INFO
	Syntax: INFO channel [ALL]
	
	Lists information about the named registered channel,
	including its founder, time of registration, last time
	used, description, and mode lock, if any.  If you are
	identified as the founder of the channel you're getting
	information for and ALL is specified, the entry message
	and successor will also be displayed.

CHAN_HELP_LIST
	Syntax: LIST [+start] pattern
	
	Lists all registered channels matching the given pattern.
	(Channels with the PRIVATE option set are not listed.)
	If the start parameter is given, it specifies how many
	matches to skip before displaying results (for example, +50
	would cause the first 50 matches to be skipped).
CHAN_HELP_LIST_OPERSONLY
	
	Only IRC operators may use this command.

CHAN_HELP_OP
	Syntax: OP channel [nickname]
	
	Ops a selected nickname on a channel.  If nickname is not
	given, ops you.  By default, limited to users with %s
	access and above on the channel.

CHAN_HELP_DEOP
	Syntax: DEOP channel [nickname]
	
	Deops a selected nickname on a channel.  If nickname is not
	given, deops you.  By default, limited to users with
	%s access and above on the channel.

CHAN_HELP_VOICE
	Syntax: VOICE channel [nickname]
	
	Voices a selected nickname on a channel.  If nickname is
	not given, voices you.  By default, limited to users with
	%s access and above on the channel.

CHAN_HELP_DEVOICE
	Syntax: DEVOICE channel [nickname]
	
	Devoices a selected nickname on a channel.  If nickname is
	not given, devoices you.  By default, limited to users with
	%s access and above on the channel.

CHAN_HELP_HALFOP
	Syntax: HALFOP channel [nickname]
	
	Gives halfop access to a selected nickname on a channel.  If
	nickname is not given, gives you halfop access.  By default,
	limited to users with %s access and above on the
	channel.

CHAN_HELP_DEHALFOP
	Syntax: DEHALFOP channel [nickname]
	
	Removes halfop access from a selected nickname on a channel.
	If nickname is not given, removes halfop access from you.
	By default, limited to users with %s access and above
	on the channel.

CHAN_HELP_PROTECT
	Syntax: PROTECT channel [nickname]
	
	Gives channel protection (mode +a) to a selected nickname on
	a channel.  If nickname is not given, gives channel
	protection to you.  By default, limited to users with
	%s access and above on the channel.

CHAN_HELP_DEPROTECT
	Syntax: DEPROTECT channel [nickname]
	
	Removes channel protection (mode +a) from a selected nickname
	on a channel.  If nickname is not given, removes channel
	protection from you.  By default, limited to users with
	%s access and above on the channel.

CHAN_HELP_INVITE
	Syntax: INVITE channel
	
	Tells %S to invite you into the given channel.  By
	default, limited to users with %s access and above on
	the channel.
	
	Note: If you get a message saying that the channel is not in
	use, it means that the channel is empty, and you can join it
	freely without needing to use this command.

CHAN_HELP_UNBAN
	Syntax: UNBAN channel
	
	Tells %S to remove all bans preventing you from
	entering the given channel.  By default, limited to users
	with %s access and above on the channel.

CHAN_HELP_KICK
	Syntax: KICK channel nickname [reason]
	
	Tells %S to kick the given nickname from the given
	channel.  The message included in the kick will show who
	used the KICK command, as well as the reason given with the
	command, if any.  By default, limited to users with
	%s access and above on the channel.
CHAN_HELP_KICK_PROTECTED
	
	Note that protected (+a) users cannot be kicked with this
	command, even by the channel founder.

CHAN_HELP_TOPIC
	Syntax: TOPIC channel topic
	
	Causes %S to set the channel topic to the one
	specified.  This command is most useful in conjunction
	with SET TOPICLOCK.  By default, limited to users with
	%s access and above on the channel.

CHAN_HELP_CLEAR
	Syntax: CLEAR channel what
	
	Tells %S to clear certain settings on a channel.  what
	can be any of the following:
	
	    MODES       Clears all modes on the channel except those
	                    set in the channel's mode lock.
	    BANS        Clears all bans on the channel.
CHAN_HELP_CLEAR_EXCEPTIONS
	    EXCEPTIONS  Clears all exceptions on the channel.
CHAN_HELP_CLEAR_INVITES
	    INVITES     Clears all invite masks on the channel.
CHAN_HELP_CLEAR_MID
	    OPS         Removes channel-operator status (mode +o) from
	                    all users.
CHAN_HELP_CLEAR_HALFOPS
	    HALFOPS     Removes halfop status (mode +h) from all users.
CHAN_HELP_CLEAR_END
	    VOICES      Removes "voice" status (mode +v) from all users.
	    USERS       Removes (kicks) all users from the channel.
	
	By default, limited to users with %s access and above
	on the channel.

CHAN_HELP_STATUS
	Syntax: STATUS channel nickname
	
	Returns the current access level of the given nickname on
	the given channel.  The reply is of the form:
	
	    STATUS channel nickname access-level
	
	If an error occurs, the reply will be in the form:
	
	    STATUS channel nickname ERROR error-message
	
	By default, limited to users with %s access and above
	on the channel.

CHAN_OPER_HELP_COMMANDS
	
	The following commands are available to Services admins:
	
	    DROPCHAN   Drop any channel
CHAN_OPER_HELP_COMMANDS_GETPASS
	    GETPASS    Retrieve the founder password for a channel
	                   (only if encryption is disabled)
CHAN_OPER_HELP_COMMANDS_FORBID
	    FORBID     Prevent a channel from being registered/used
	    SUSPEND    Prevent a registered channel from being used
	    UNSUSPEND  Cancel a channel suspension
CHAN_OPER_HELP_COMMANDS_END
	
	Services admins can also view the access, autokick, and
	level setting lists or use the STATUS command for any
	channel.

CHAN_OPER_HELP_DROPCHAN
	Syntax: DROPCHAN channel
	
	Drops the named channel from the %S database.
	
	Limited to Services admins.

CHAN_OPER_HELP_SET
	
	Services admins can also set the option NOEXPIRE, with
	which channels can be prevented from expiring.
	Additionally, Services admins can set options for any
	channel without identifying by password for the channel.

CHAN_OPER_HELP_SET_NOEXPIRE
	Syntax: SET channel NOEXPIRE {ON | OFF}
	
	Sets whether the given channel will expire.  Setting this
	to ON prevents the channel from expiring.
	
	Limited to Services admins.

CHAN_OPER_HELP_UNSET
	
	As with the SET command, Services admins can clear settings
	for any channel without identifying for the channel.

CHAN_OPER_HELP_INFO
	
	Services admins can use the ALL parameter with any channel.

CHAN_OPER_HELP_LIST
	Syntax: LIST [+start] pattern [FORBIDDEN] [NOEXPIRE] [SUSPENDED]
	
	Lists all registered channels matching the given pattern.
	Channels with the PRIVATE option set will only be displayed
	to Services admins.  Channels with the NOEXPIRE option set
	will have a ! prepended to the channel name for Services
	admins.
	
	If the start parameter is given, it specifies how many
	matches to skip before displaying results (for example, +50
	would cause the first 50 matches to be skipped).
	
	If the FORBIDDEN, SUSPENDED or NOEXPIRE options are given,
	only channels which, respectively, are FORBIDden, SUSPENDed or
	have the NOEXPIRE flag set will be displayed.  The options can
	be used in any order and in any combination. These options
	are limited to Services admins.

CHAN_OPER_HELP_GETPASS
	Syntax: GETPASS channel
	
	Returns the password for the given channel.  Note that
	whenever this command is used, a message including the
	person who issued the command and the channel it was used
	on will be logged and sent out as a WALLOPS/GLOBOPS.
	
	If encryption is in use, this command may not be available
	depending on the type of encryption used.
	
	Limited to Services admins.

CHAN_OPER_HELP_FORBID
	Syntax: FORBID channel
	
	Disallows anyone from registering or using the given
	channel.  May be cancelled by dropping the channel.
	
	Limited to Services admins.

CHAN_OPER_HELP_SUSPEND
	Syntax: SUSPEND [+expiry] channel reason
	
	Prevents a channel from being used or identified for
	by anyone.  The channel can be unsuspended with the
	UNSUSPEND command, which will allow it to be used as before.
	The expiry parameter is interpreted in the same manner as
	for the %s AKILL command.
	
	Limited to Services admins.

CHAN_OPER_HELP_UNSUSPEND
	Syntax: UNSUSPEND channel
	
	Allows users to use and identify for a previously suspended
	channel.
	
	Limited to Services admins.


###########################################################################
#
# MemoServ help messages
#
###########################################################################

MEMO_HELP
	%S is a utility allowing IRC users to send short
	messages to other IRC users, whether they are online at
	the time or not, or to channels(*).  Both the sender's
	nickname and the target nickname or channel must be
	registered in order to send a memo.  Type
	/msg %S HELP COMMANDS for a list of %S commands;
	to use a command, type /msg %S command, or for more
	information on a command, type /msg %S HELP command.
MEMO_HELP_EXPIRES
	
	Notice:  Memos will expire (automatically be deleted)
	%s after being sent, unless marked non-expiring
	with the SAVE command.
MEMO_HELP_END_LEVELS
	
	(*) By default, channel memos are sent to all users
	    (including the founder) with at least %s access
	    on the channel.  This can be changed with the %s
	    LEVELS command.
MEMO_HELP_END_XOP
	
	(*) Channel memos will be sent to the channel founder and
	    all users on the channel's SOP list.

MEMO_HELP_COMMANDS
	The following commands can be used with %S:
	
	    SEND      Send a memo to a nickname or channel
	    LIST      List your memos
	    READ      Read a memo or memos
MEMO_HELP_COMMANDS_FORWARD
	    FORWARD   Forwards memos to your E-mail address
MEMO_HELP_COMMANDS_SAVE
	    SAVE      Marks memos as non-expiring
MEMO_HELP_COMMANDS_DEL
	    DEL       Delete a memo or memos
	    RENUMBER  Renumbers your memos
	    SET       Set options related to memos
	    INFO      Display information related to memos
MEMO_HELP_COMMANDS_IGNORE
	    IGNORE    Ignore memos from certain users

MEMO_HELP_SEND
	Syntax: SEND {nickname | channel} memo-text
	
	Sends the named nickname or channel a memo containing
	memo-text.  The recipient(s) will receive a notice that
	they have a new memo.  The target nickname/channel must be
	registered.

MEMO_HELP_LIST
	Syntax: LIST [num | list | NEW]
	
	Lists any memos you currently have.  With NEW, lists only
	new (unread) memos.  Unread memos are marked with a "*" to
	the left of the memo number; channel memos are marked with a
	"#".  You can also specify a list of numbers, as in the
	example below:
	
	   LIST 2-5,7-9
	      Lists memos numbered 2 through 5 and 7 through 9.

MEMO_HELP_LIST_EXPIRE
	Syntax: LIST [num | list | NEW]
	
	Lists any memos you currently have.  With NEW, lists only
	new (unread) memos.  Unread memos are marked with a "*" to
	the left of the memo number; channel memos are marked with a
	"#", and memos that will not expire are marked with a "+".
	You can also specify a list of numbers, as in the example
	below:
	
	   LIST 2-5,7-9
	      Lists memos numbered 2 through 5 and 7 through 9.

MEMO_HELP_READ
	Syntax: READ {num | list | LAST | NEW}
	
	Sends you the text of the memos specified.  If LAST is
	given, sends you the memo you most recently received.  If
	NEW is given, sends you all of your new memos.  Otherwise,
	sends you memo number num.  You can also give a list of
	numbers, as with the LIST command.

MEMO_HELP_SAVE
	Syntax: SAVE {num | list}
	
	Marks the given memo or memos as non-expiring.  You can give
	either a single memo number or, as with the LIST command, a
	list of memos.

MEMO_HELP_DEL
	Syntax: DEL {num | list | ALL}
	
	Deletes the specified memo or memos.  You can supply
	multiple memo numbers or ranges of numbers instead of a
	single number, as with the LIST command; you can also give
	the word ALL, which causes all of your memos to be
	deleted.
	
	Warning: Once a memo is deleted, it cannot be recovered!

MEMO_HELP_RENUMBER
	Syntax: RENUMBER
	
	Renumbers your memo list, removing "holes" in the number
	sequence resulting from deleted memos.

MEMO_HELP_SET
	Syntax: SET option parameters
	
	Sets various memo options.  option can be one of:
	
	    NOTIFY      Changes when you will be notified about
	                    new memos (only for nicknames)
	    LIMIT       Sets the maximum number of memos you can
	                    receive
MEMO_HELP_SET_OPTION_FORWARD
	    FORWARD     Sets whether memos are automatically
	                    forwarded to you
MEMO_HELP_SET_END
	
	Type /msg %S HELP SET option for more information
	on a specific option.

MEMO_HELP_SET_NOTIFY
	Syntax: SET NOTIFY {ON | LOGON | NEW | OFF}
	
	Changes when you will be notified about new memos:
	
	    ON      You will be notified of memos when you log on,
	               when you unset /AWAY, and when they are sent
	               to you.
	    LOGON   You will only be notified of memos when you log
	               on or when you unset /AWAY.
	    NEW     You will only be notified of memos when they
	               are sent to you.
	    OFF     You will not receive any notification of memos.
	
	ON is equivalent to LOGON and NEW combined.
	
	Note that if your nickname has the "secure" setting on, you
	will only be notified of new memos (for LOGON or ON) after
	you identify to %s.

MEMO_HELP_SET_LIMIT
	Syntax: SET LIMIT limit
	
	Sets the maximum number of memos you are allowed to have.
	If you set this to 0, no one will be able to send any memos
	to you.  However, you cannot set this any higher than %d.

MEMO_HELP_INFO
	Syntax: INFO
	
	Displays information on the number of memos you have, how
	many of them are unread, how many total memos you can
	receive and when you are notified of new ones.

MEMO_OPER_HELP_COMMANDS
	
	Services admins may use the SET LIMIT and INFO commands with
	nicknames other than their own.

MEMO_OPER_HELP_SET_LIMIT
	Syntax: SET LIMIT [nickname] {limit | NONE | DEFAULT} [HARD]
	
	Sets the maximum number of memos a nickname is allowed to
	have.  Setting the limit to 0 prevents the nick from
	receiving any memos; setting it to NONE allows the nick to
	receive and keep as many memos as they want; and setting it
	to DEFAULT restores the limit to the default value (%d).
	If you do not give a nickname , your own limit is set.
	
	Adding HARD prevents the user from changing the limit.  Not
	adding HARD has the opposite effect, allowing the user to
	change the limit (even if a previous limit was set with
	HARD).
	
	This use of the SET LIMIT command is limited to Services
	admins.  Other users may only enter a limit for themselves,
	may not remove their limit or set their limit to the
	default, may not set a limit above %d, and may not set a
	hard limit.

MEMO_OPER_HELP_INFO
	Syntax: INFO [nickname]
	
	Without a parameter, displays information on the number of
	memos you have, how many of them are unread, how many
	total memos you can receive and when you are notified of
	new ones.
	
	With a nickname parameter, displays the same information
	for the given nickname.  This use limited to Services
	admins.


MEMO_HELP_FORWARD
	Syntax: FORWARD {num | list | ALL}
	
	Forwards one or more of your memos to your E-mail address.
	You must have a valid E-mail address set for your nickname
	in order to use this command.  Channel memos cannot be
	forwarded.

MEMO_HELP_SET_FORWARD
	Syntax: SET FORWARD {ON | COPY | OFF}
	
	Sets whether %S automatically forwards to your E-mail
	address memos sent to you.  When set to ON, memos sent to
	your nickname will instead be forwarded to the E-mail
	address registered with your nickname.  COPY is similar, but
	also causes %S to save a copy of the memo to read
	online.  When set to OFF, your memos will be stored online
	as usual.  Even if you set this option to ON, you may still
	receive memos online if Services is unable to forward them
	to you.
	
	Note that when this option is set to COPY and you have
	received the maximum number of memos you are allowed to, you
	will not be able to receive any new memos (including by
	E-mail) until you delete some old memos using the DEL
	command.

MEMO_HELP_IGNORE
	Syntax: IGNORE {ADD | DEL | LIST} [nickname | mask]
	
	Controls the list of "ignored" users; users whose nickname
	or user@host mask match an entry on this list are not
	allowed to send you memos.  ADD adds the given nickname or
	user@host mask to your ignore list, and DEL removes the
	given nickname or mask from the list; LIST displays your
	current ignore list.
	
	Note that the nickname or mask given to the ADD command may
	contain "*" or "?" wildcards, and is not case-sensitive; for
	example, "*Evil*" matches "EvilNick", "AN_EVIL_USER" and
	"devil".


###########################################################################
#
# OperServ help messages
#
###########################################################################

OPER_HELP
	%S allows IRC operators to control various aspects of
	the IRC network.  Type /msg %S HELP COMMANDS for a
	list of commands.
	
	Notice: All commands sent to %S are logged!

OPER_HELP_COMMANDS
	%S commands:
	    GLOBAL      Send a message to all users
	    STATS       Show status of Services and network
	    SERVERMAP   Show map of servers on network
	    OPER LIST   List all Services operators
	    ADMIN LIST  List all Services admins
OPER_HELP_COMMANDS_SERVOPER
	
	Commands available to Services operators and admins only:
	    MODE        Change a channel's modes
	    KICK        Kick a user from a channel
	    CLEARMODES  Removes certain modes from a channel
	    CLEARCHAN   Removes all users from a channel (masskick)
	    KILLCLONES  Kill all users that have a certain host
OPER_HELP_COMMANDS_AKILL
	    AKILL       Manipulate the autokill list
	    AKILLCHAN   Autokill all users in a channel
OPER_HELP_COMMANDS_EXCLUDE
	    EXCLUDE     Manipulate the autokill exclusion list
OPER_HELP_COMMANDS_SLINE
	    SGLINE      Manipulate the SGLINE list
	    SQLINE      Manipulate the SQLINE list
	    SZLINE      Manipulate the SZLINE list
OPER_HELP_COMMANDS_SESSION
	    SESSION     View the list of host sessions
	    EXCEPTION   Modify the session-limit exception list
OPER_HELP_COMMANDS_NEWS
	    LOGONNEWS   Define messages to be shown to users at logon
	    OPERNEWS    Define messages to be shown to users who oper
OPER_HELP_COMMANDS_SERVADMIN
	
	Commands available to Services admins only:
	    OPER        Modify the Services operator list
	    JUPE        "Jupiter" a server
	    SET         Set various global Services options
	    UPDATE      Force the Services databases to be
	                    updated on disk immediately
	    SHUTDOWN    Save databases and stop Services
	    QUIT        Terminate Services without saving databases
	    RESTART     Save databases and restart Services
	    REHASH      Re-read configuration files
	    SU          Obtain Services super-user privileges
OPER_HELP_COMMANDS_SERVROOT
	
	Commands available to the Services super-user only:
	    ADMIN       Modify the Services admin list
OPER_HELP_COMMANDS_RAW
	    RAW         Send a raw string to the IRC server

OPER_HELP_GLOBAL
	Syntax: GLOBAL message
	
	Allows IRCops to send messages to all users on the network.
	The message will be sent from the nickname %s.

OPER_HELP_STATS
	Syntax: STATS [RESET | NETWORK | ALL]
	
	Without any option, shows the current number of users and
	IRCops online (excluding Services), the highest number of
	users online since Services was started, and the length of
	time Services has been running.
	
	With the RESET option, resets the maximum user count to
	the number of users currently online.
	
	With the NETWORK option, shows the amount of data sent
	to and received from the remote server, and the amount of
	memory used by network send/receive buffers.  The ratio of
	the current buffer memory usage to the buffer size limit is
	also displayed as a percentage if a limit is configured.
	
	The ALL option is available only to Services admins, and
	displays information on Services' memory usage.  Using this
	option can freeze Services for a short period of time on
	large networks, so don't overuse it!
	
	UPTIME may be used as a synonym for STATS.

OPER_HELP_SERVERMAP
	Syntax: SERVERMAP
	
	Displays a map of servers connected to the network, from the
	viewpoint of Services.  "Fake" servers--Services itself and
	any servers jupitered with the JUPE command--have "(*)"
	displayed after the sever name.

OPER_HELP_OPER
	Syntax: OPER ADD nickname
	        OPER DEL nickname
	        OPER LIST
	
	Allows Services admins to add or remove nicknames to or from
	the Services operator list.  A user whose nickname is on the
	Services operator list and who has identified to %s
	will be able to access Services operator commands.
	
	Any IRC operator may use the OPER LIST form of the command.
	All other use limited to Services admins.

OPER_HELP_ADMIN
	Syntax: ADMIN ADD nickname
	        ADMIN DEL nickname
	        ADMIN LIST
	
	Allows the Services super-user to add or remove nicknames
	to or from the Services admin list.  A user whose nickname
	is on the Services admin list and who has identified to
	%s will be able to access Services admin commands.
	
	Any IRC operator may use the ADMIN LIST form of the command.
	All other use limited to Services super-user.

OPER_HELP_GETKEY
	Syntax: GETKEY channel
	
	Returns the channel key set on the given channel, if any.
	
	Limited to Services operators.

OPER_HELP_MODE
	Syntax: MODE channel modes
	
	Allows Services operators to set channel modes for any
	channel.  Parameters are the same as for the standard /MODE
	command.
	
	Limited to Services operators.

OPER_HELP_CLEARMODES
	Syntax: CLEARMODES channel [ALL]
	
	Clears all channel modes (n,s,t,k,l,b, and so on) from a
	channel.  If ALL is given, also clears all user modes (like
	+o and +v) from the channel.  However, if the channel is
	registered, its mode lock will still take effect.
	
	Limited to Services operators.

OPER_HELP_CLEARCHAN
	Syntax: CLEARCHAN channel
	
	Removes all users from a channel (masskick).
	
	Limited to Services operators.

OPER_HELP_KICK
	Syntax: KICK channel user reason
	
	Allows IRCops to kick a user from any channel.
	Parameters are the same as for the standard /KICK
	command.  The kick message will have the nickname of the
	IRCop sending the KICK command prepended; for example:
	
	*** SpamMan has been kicked off channel #my_channel by %S (Alcan (Flood))
	
	Limited to Services operators.

OPER_HELP_AKILL
	Syntax: AKILL ADD [+expiry] mask reason
	        AKILL DEL mask
	        AKILL CLEAR ALL
	        AKILL LIST [+start] [pattern] [NOEXPIRE]
	        AKILL VIEW [+start] [pattern] [NOEXPIRE]
	        AKILL CHECK mask
	        AKILL COUNT
	
	Allows Services operators to manipulate the autokill (AKILL)
	list.  If a user matching an autokill mask attempts to
	connect, Services will issue a KILL for that user and, on
	supported server types, will instruct all servers to add a
	ban (K-line) for the mask which the user matched.
	
	AKILL ADD adds the given user@host mask to the autokill
	list.  The reason should describe why the entry is being
	added; depending on how Services is configured, it may also
	be displayed to users in KILL messages.  An expiry time can
	also be specified, as described below.
	
	AKILL DEL removes the given mask from the autokill list if
	it is present.
	
	AKILL CLEAR clears (removes) all masks from the autokill
	list.  The ALL parameter is mandatory, to avoid accidental
	use of this command.
	
	AKILL LIST shows all current autokills; if the optional
	pattern is given, the list is limited to those AKILLs
	matching the pattern.  If the start parameter is given, it
	specifies how many matches to skip before displaying results
	(for example, +50 would cause the first 50 matches to be
	skipped).  Additionally, NOEXPIRE can be used to limit the
	list to autokills that do not expire.
	
	AKILL VIEW is a more verbose version of AKILL LIST, and will
	show who added an autokill, the date it was added, and when
	it expires, as well as the user@host mask and reason.
	
	AKILL CHECK will display any entries on the autokill list
	which the given user@host mask would match; this can be used
	to determine which autokill caused a particular user to be
	killed.
	
	AKILL COUNT simply returns the number of autokills in the
	autokill list.
	
	Optionally, an expiry time can be given with the AKILL ADD
	command.  The expiry time precedes the user@host mask, and
	is specified as an integer followed by one of d (days),
	h (hours), or m (minutes).  Combinations (such as 1h30m)
	are also permitted.  If a unit specifier is not included,
	the default is days (so +30 by itself means 30 days).  To
	add an autokill which does not expire, use +0.  If the
	usermask to be added starts with a +, an expiry time must
	be given, even if it is the same as the default.
OPER_HELP_AKILL_OPERMAXEXPIRY
	
	Services operators may only set an expiry time up to
	%s; Services administrators may set any
	expiry time.
OPER_HELP_AKILL_END
	
	Limited to Services operators.

OPER_HELP_AKILLCHAN
	Syntax: AKILLCHAN [KILL] [+expiry] channel reason
	
	Adds autokills for all users except IRC operators in a given
	channel, and optionally (if KILL is given) removes them from
	the network.  The reason will be stored with each autokill,
	and may be displayed in kill messages depending on how
	Services is configured.  expiry takes the same format as in
	the AKILL ADD command; if not given, it defaults to %s.
	
	Autokills added by this command will have the form
	*@hostname, for each hostname used by a client in the
	channel.  If such an autokill already exists for a hostname,
	it will not be modified; this may cause the number of
	autokills added to be less than the number of users
	affected.
	
	Limited to Services operators.

OPER_HELP_EXCLUDE
	Syntax: EXCLUDE ADD [+expiry] mask reason
	        EXCLUDE DEL mask
	        EXCLUDE CLEAR ALL
	        EXCLUDE LIST [+start] [pattern] [NOEXPIRE]
	        EXCLUDE VIEW [+start] [pattern] [NOEXPIRE]
	        EXCLUDE CHECK mask
	        EXCLUDE COUNT
	
	Allows Services operators to manipulate the autokill
	exclusion list.  When a user matching an autokill exclusion
	mask connects to the network, Services will not issue a KILL
	for that user even if the user would otherwise match an
	autokill mask.  This is useful if you have wide autokill
	masks, and still want specific hosts or users that match
	those masks to be able to connect.
	
	The ADD, DEL, CLEAR, LIST, VIEW, CHECK, and COUNT
	subcommands function the same way as the subcommands for the
	AKILL command; see the AKILL help for more information.
	
	Limited to Services operators.

OPER_HELP_SGLINE
	Syntax: SGLINE ADD [+expiry] mask reason
	        SGLINE DEL mask
	        SGLINE CLEAR ALL
	        SGLINE LIST [+start] [pattern] [NOEXPIRE]
	        SGLINE VIEW [+start] [pattern] [NOEXPIRE]
	        SGLINE CHECK mask
	        SGLINE COUNT
	
	Allows Services operators to manipulate the SGLINE list.  If
	a user whose "real name" matches an SGLINE mask attempts to
	connect, Services will issue a KILL for that user and, on
	supported server types, will instruct all servers to add a
	ban (server G-line) for the mask which the user matched.
	
	SGLINE ADD adds the given real name mask to the SGLINE list.
	The reason should describe why the entry is being added;
	depending on how Services is configured, it may also be
	displayed to users in KILL messages.
	
	SGLINE DEL removes the given mask from the SGLINE list if it
	is present.
	
	SGLINE CLEAR clears (removes) all masks from the SGLINE
	list.  The ALL parameter is mandatory, to avoid accidental
	use of this command.
	
	SGLINE LIST shows all SGLINE entries; if the optional
	pattern is given, the list is limited to those entries
	matching the pattern.  If the start parameter is given, it
	specifies how many matches to skip before displaying results
	(for example, +50 would cause the first 50 matches to be
	skipped).  Additionally, NOEXPIRE can be used to limit the
	list to entries that do not expire.
	
	SGLINE VIEW is a more verbose version of SGLINE LIST, and
	will show who added each SGLINE entry, the date it was
	added, when it was last used, and when it expires, as well
	as the mask and reason.
	
	SGLINE CHECK will display any entries on the SGLINE list
	which the given user@host mask would match; this can be used
	to determine which SGLINE caused a particular user to be
	killed.
	
	SGLINE COUNT simply returns the number of entries in the
	SGLINE list.
	
	The mask parameter may be enclosed in double quotes, and
	must be when the mask contains spaces; for example:
	    /msg %S SGLINE ADD +0 "bad name" This is a bad name.
	
	Optionally, an expiry time can be given with the SGLINE ADD
	command.  The expiry time precedes the user@host mask, and
	is specified as an integer followed by one of d (days),
	h (hours), or m (minutes).  Combinations (such as 1h30m)
	are also permitted.  If a unit specifier is not included,
	the default is days (so +30 by itself means 30 days).  To
	add an entry which does not expire, use +0.  If the
	mask to be added starts with a + and no expiry time is
	given, the mask must be enclosed in double quotes, as
	described above.
	
	Limited to Services operators.

OPER_HELP_SQLINE
	Syntax: SQLINE ADD [+expiry] mask reason
	        SQLINE DEL mask
	        SQLINE CLEAR ALL
	        SQLINE LIST [+start] [pattern] [NOEXPIRE]
	        SQLINE VIEW [+start] [pattern] [NOEXPIRE]
	        SQLINE CHECK mask
	        SQLINE COUNT
	
OPER_HELP_SQLINE_KILL
	Allows Services operators to manipulate the SQLINE list.  If
	a user whose nickname matches an SQLINE mask attempts to
	connect, or a user attempts to change to a nickname matching
	an SQLINE mask, Services will issue a KILL for that user
	and, on supported server types, will instruct all servers to
	add a ban (server Q-line) for the mask which the user
	matched.
OPER_HELP_SQLINE_NOKILL
	Allows Services operators to manipulate the SQLINE list.  If
	a user whose nickname matches an SQLINE mask attempts to
	connect, or a user attempts to change to a nickname matching
	an SQLINE mask, Services will change that user's nickname to
	a "guest" nickname, and will send the user a message that
	will normally cause the user's IRC client to request a new
	nickname.  On supported server types, Services will also
	instruct all servers to add a ban (server Q-line) for the
	mask which the user matched.
OPER_HELP_SQLINE_IGNOREOPERS
	
	Note that IRC operators will not be affected by SQLINE masks.
OPER_HELP_SQLINE_END
	
	The ADD, DEL, CLEAR, LIST, VIEW, CHECK, and COUNT
	subcommands function the same way as the subcommands for the
	SGLINE command; see the SGLINE help for more information.
	
	Limited to Services operators.

OPER_HELP_SZLINE
	Syntax: SZLINE ADD [+expiry] mask reason
	        SZLINE DEL mask
	        SZLINE CLEAR ALL
	        SZLINE LIST [+start] [pattern] [NOEXPIRE]
	        SZLINE VIEW [+start] [pattern] [NOEXPIRE]
	        SZLINE CHECK mask
	        SZLINE COUNT
	
	Allows Services operators to manipulate the SZLINE list.  If
	a user whose IP address matches an SZLINE mask attempts to
	connect, Services will issue a KILL for that user and, on
	supported server types, will instruct all servers to add a
	ban (server Z-line) for the mask which the user matched.
	
	The ADD, DEL, CLEAR, LIST, VIEW, CHECK, and COUNT
	subcommands function the same way as the subcommands for the
	SGLINE command; see the SGLINE help for more information.
	
	Limited to Services operators.

OPER_HELP_EXCEPTION
	Syntax: EXCEPTION ADD [+expiry] mask limit reason
	        EXCEPTION DEL {mask | list}
	        EXCEPTION CLEAR ALL
	        EXCEPTION MOVE num newnum
	        EXCEPTION LIST [[+start] pattern | list]
	        EXCEPTION VIEW [[+start] pattern | list]
	        EXCEPTION CHECK hostname
	        EXCEPTION COUNT
	
	Allows Services operators to manipulate the list of hosts
	that have specific session limits, allowing certain machines,
	such as shell servers, to carry more than the default number
	of clients at a time.  Once a host reaches its session limit,
	all clients attempting to connect from that host will be
	killed.  Before the user is killed, they are notified, via a
	/NOTICE from %S, of a source of help regarding session
	limiting.  The content of this notice is a configuration
	setting.
	
	EXCEPTION ADD adds the given host mask to the exception list.
	Note that nick!user@host and user@host masks are invalid!
	Only real host masks, such as box.host.dom and *.host.dom,
	are allowed, because sessions limiting does not take nick or
	user names into account.  limit must be a number greater than
	or equal to zero; this determines how many sessions this host
	may carry at a time (a value of zero means the host has an
	unlimited session limit).
	
	Optionally, an expiry time can be given with the EXCEPTION
	ADD command.  The expiry time precedes the host mask, and
	is specified as an integer followed by one of d (days),
	h (hours), or m (minutes).  Combinations (such as 1h30m)
	are also permitted.  If a unit specifier is not included,
	the default is days (so +30 by itself means 30 days).  To
	add an entry which does not expire, use +0.  If the
	mask to be added starts with a +, an expiry time must
	be given, even if it is the same as the default.
	
	EXCEPTION DEL removes the given mask or numbered entry or
	entries from the exception list (entry numbers can be found
	with VIEW or LIST).
	
	EXCEPTION CLEAR clears (removes) all masks from the
	exception list.  The ALL parameter is mandatory, to avoid
	accidental use of this command.
	
	EXCEPTION MOVE moves exception num to newnum.  If newnum
	is already used by another exception, its number will be
	incremented to make room.
	
	EXCEPTION LIST and EXCEPTION VIEW show all current
	exceptions.  If a pattern is given, the display is limited
	to entries matching that pattern (with the optional start
	parameter giving the number of entries to skip before
	displaying results); if a number list is given, only entries
	in the list will be displayed.  The difference between the
	two commands is that EXCEPTION VIEW is more verbose,
	displaying the nickname of the person who added the
	exception, its session limit, reason, host mask and the
	expiry date and time, as well as the last time the exception
	was "used", i.e., the last time a user connected who matched
	the exception's mask.
	
	EXCEPTION CHECK will display any entries on the exception
	list which the given hostname would match; this can be used
	to determine which exception, if any, is applied to a
	particular user.
	
	EXCEPTION COUNT displays the number of entries on the
	exception list.
	
	Note that when a client connects, the first matching
	exception found will be applied to that client.  Large
	exception lists and widely matching exception masks are
	likely to degrade Services' performance.
	
	Limited to Services operators.

OPER_HELP_SESSION
	Syntax: SESSION LIST threshold
	        SESSION VIEW host
	
	Allows Services operators to view the session list.
	
	SESSION LIST lists hosts with at least threshold sessions.
	The threshold must be a number greater than 1, to prevent
	accidental listing of the large number of single session
	hosts.
	
	SESSION VIEW displays detailed information about a specific
	host, including the current session count and session limit.
	The host value may not include wildcards.
	
	See the EXCEPTION help for more information about session
	limiting and how to set session limits specific to certain
	hosts and groups thereof.
	
	Limited to Services operators.

OPER_HELP_SU
	Syntax: SU password
	
	Allows access to functions which require Services super-user
	privileges.  The password is the same as that set with the
	SET SUPASS command; note that the password must be set once
	before using this command.  Super-user privileges last until
	you quit IRC.
	
	WARNING: If you enter an incorrect password, Services will
	broadcast a warning message to all IRC operators.
	
	Limited to Services admins.

OPER_HELP_SET
	Syntax: SET option setting
	
	Sets various global Services options.  Option names
	currently defined are:
	    READONLY   Set read-only or read-write mode
	    DEBUG      Activate or deactivate debug mode
	    SUPASS     Set password for SU command (super-user)
	
	Limited to Services admins; the SUPASS option may only be
	set by the Services super-user.

OPER_HELP_SET_READONLY
	Syntax: SET READONLY {ON | OFF}
	
	Sets read-only mode on or off.  In read-only mode, normal
	users will not be allowed to modify any Services data,
	including channel and nickname access lists, etc.  IRCops
	with sufficient Services privileges will be able to modify
	Services' AKILL list and drop or forbid nicknames and
	channels, but any such changes will not be saved unless
	read-only mode is deactivated before Services is terminated
	or restarted.
	
	This option is equivalent to the command-line option
	-readonly.

OPER_HELP_SET_DEBUG
	Syntax: SET DEBUG {ON | OFF | num}
	
	Sets debug mode on or off.  In debug mode, all data sent to
	and from Services as well as a number of other debugging
	messages are written to the log file.  If num is
	given, debug mode is activated, with the debugging level set
	to num.
	
	This option is equivalent to the command-line option
	-debug.

OPER_HELP_SET_SUPASS
	Syntax: SET SUPASS [password]
	
	Sets the password to be used for the SU command.  The
	password must be set once before using the SU command for
	the first time.  If no password is given, the SU command
	will be unavailable.
	
	This option may only be set by the Services super-user.

OPER_HELP_JUPE
	Syntax: JUPE server [reason]
	
	Tells Services to jupiter a server--that is, to create
	a fake "server" connected to Services which prevents
	the real server of that name from connecting.  The jupe
	may be removed using a standard /SQUIT.  If a reason is
	given, it is placed in the server information field;
	otherwise, the server information field will contain the
	text "Jupitered by <nick>", showing the nickname of the
	person who jupitered the server.
	
	If there is already a server of the same name connected to
	the network, it will be disconnected automatically.
	
	Limited to Services admins.

OPER_HELP_RAW
	Syntax: RAW text
	
	Sends a string of text directly to the server to which
	Services is connected, without performing any processing on
	it.  This command is intended primarily for testing of
	Services and IRC servers; it has a very limited range of
	uses on a live network, and can wreak havoc on a network or
	cause Services to crash if used improperly.  DO NOT USE THIS
	COMMAND unless you are absolutely certain you know what you
	are doing!
	
	Limited to the Services super-user.

OPER_HELP_UPDATE
	Syntax: UPDATE [FORCE]
	
	Causes Services to update all database files as soon as you
	send the command.  If the FORCE option is given, Services
	will attempt to unlock the databases before performing the
	update; this can be used to fix "databases are locked"
	errors when updating the databases.
	
	Limited to Services admins.

OPER_HELP_QUIT
	Syntax: QUIT
	
	Causes Services to do an immediate shutdown; databases are
	not saved.  This command should not be used unless
	damage to the in-memory copies of the databases is feared
	and they should not be saved.  For normal shutdowns, use the
	SHUTDOWN command.
	
	Limited to Services admins.

OPER_HELP_SHUTDOWN
	Syntax: SHUTDOWN
	
	Causes Services to save all databases and then shut down.
	
	Limited to Services admins.

OPER_HELP_RESTART
	Syntax: RESTART
	
	Causes Services to save all databases and then restart
	(i.e. exit and immediately re-run the executable).
	
	Limited to Services admins.

OPER_HELP_REHASH
	Syntax: REHASH
	
	Causes Services to re-read its configuration files and
	update active settings.  Some settings, such as RemoteServer
	(which sets the hostname of the IRC server to connect to),
	cannot be changed with REHASH, and will only take effect
	when Services is restarted; in these cases, a message will
	be written to the Services log file, and a notice will be
	sent notifying you of this.
	
	If LoadModule lines have been added to or removed from the
	main Services configuration file (%s), modules
	will be loaded or unloaded as needed.  Modules which no
	longer have a LoadModule line will be unloaded in reverse
	order of loading; then, after the remaining modules have
	been rehashed, new modules will be loaded in the order
	listed in the configuration file.  If an error occurs at any
	step, rehashing will be aborted.
	
	Note that due to module dependencies, some modules cannot be
	removed or changed via REHASH; in such cases, you will need
	to restart Services for the changes to take effect.
	
	Limited to Services admins.

OPER_HELP_KILLCLONES
	Syntax: KILLCLONES nickname
	
	Kills all users who have the same hostname as nickname.  A
	temporary AKILL, in the form *@host, is added to prevent the
	offending clients from immediately reconnecting.  A WALLOPS
	is also sent indicating who used the command, which host was
	affected, and how many users were killed.  It's useful for
	removing numerous clones from the network.
	
	Limited to Services operators.


###########################################################################
#
# StatServ help messages
#
###########################################################################

STAT_HELP
	%S provides statistical information about the
	IRC network, its servers and its users.  Type
	/msg %S HELP COMMANDS for a list of %S commands; for
	information on a command, type /msg %S HELP command.

STAT_HELP_COMMANDS
	The following commands can be used with %S:
	
	    SERVERS     Server statistics
	    USERS       User statistics

STAT_HELP_SERVERS
	Syntax: SERVERS STATS
	        SERVERS LIST [mask]
	        SERVERS VIEW [mask] [ONLINE | OFFLINE]
	
	Displays statistics relating to the network's servers.
	
	SERVERS STATS displays a brief summary of the servers
	currently visible (online) and invisible (offline) to
	Services.
	SERVERS LIST displays a list of online servers and
	current statistics about each.
	SERVERS VIEW displays a list of online and offline
	servers with detailed statistics about each.  The
	ONLINE and OFFLINE options cause, respectively,
	only online or offline servers to be shown.
	
	Use the mask option to limit which servers'
	statistics are shown.

STAT_HELP_USERS
	Syntax: USERS STATS
	
	Displays statistics relating to the network's users.

STAT_OPER_HELP_SERVERS
	Syntax: SERVERS STATS
	        SERVERS LIST [mask]
	        SERVERS VIEW [mask] [ONLINE | OFFLINE]
	        SERVERS DELETE server
	        SERVERS COPY server newname
	        SERVERS RENAME server newname
	
	Displays statistics relating to the network's servers.
	
	SERVERS STATS displays a brief summary of the servers
	currently visible (online) and invisible (offline) to
	Services.
	SERVERS LIST displays a list of online servers and
	current statistics about each.
	SERVERS VIEW displays a list of online and offline
	servers with detailed statistics about each.  The
	ONLINE and OFFLINE options cause, respectively,
	only online or offline servers to be shown.
	
	Use the mask option to limit which servers'
	statistics are shown.
	
	DELETE deletes the specified server's statistics.
	COPY duplicates a set of server statistics.
	RENAME rename's a set of server statistics.
	These three commands are limited to Services administrators.

###########################################################################

# Please do not change the following line; it is used to determine which
# version of the master (English) language file was used to create a
# translated file.

# CVS: $Revision: 2.293 $
