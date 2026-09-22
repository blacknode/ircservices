# Emacs: -*- mode: fundamental; coding: binary; -*-

# Turkish language file.
#
# IRC Services is copyright (c) 1996-2009 Andrew Church.
#     E-mail: <achurch@achurch.org>
# Parts written by Andrew Kempe and others.
# This program is free but copyrighted software; see the file GPL.txt for
# details.

LANG_NAME
	Turkce (Turkish)

# Karakter seti belirlemis oldugunuz dil icin kullanilacaktir.Bu, 
# email okuyucusu ve web tarayici yazilimlari tarafindan anlanabilmesi
# icin bir "charset" degeri olmalidir.
LANG_CHARSET
	iso-8859-9

########################################################
# Translated by TimeMr14C (uhc0@stud.uni-karlsruhe.de) #
# Former translations by mikado (mikado@holyfire.com)  #
# Typos fixed by |SaMaN| (saman@ttnet.net.tr)          #
########################################################

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
	Paz
	Pzt
	Sal
	Car
	Per
	Cum
	Cmt
# %A
STRFTIME_DAYS_LONG
	Pazar
	Pazartesi
	Sali
	Carsamba
	Persembe
	Cuma
	Cumartesi
# %b
STRFTIME_MONTHS_SHORT
	Oca
	Sub
	Mar
	Nis
	May
	Haz
	Tem
	Agu
	Eyl
	Ekm
	Ksm
	Ara
# %B
STRFTIME_MONTHS_LONG
	Ocak
	Subat
	Mart
	Nisan
	Mayis
	Haziran
	Temmuz
	Agustos
	Eylul
	Ekim
	Kasim
	Aralik

# "day", "hour", "minute", and "second" in singular and plural.
STR_DAY
	 gun
STR_DAYS
	 gun
STR_HOUR
	 saat
STR_HOURS
	 saat
STR_MINUTE
	 dakika
STR_MINUTES
	 dakika
STR_SECOND
	 saniye
STR_SECONDS
	 saniye
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
	Dahili hata - Kullanici kaydi bulunamadi.
SERVICES_IS_BUSY
	Services su an mesgul, lütfen daha sonra yeniden deneyiniz.

UNKNOWN_COMMAND
	%s diye bir komut yok.
UNKNOWN_COMMAND_HELP
	%s diye bir komut yok. "/msg %s HELP" yaziniz.
SYNTAX_ERROR
	Kullanimi: %s
MORE_INFO
	Daha fazla bilgi icin /msg %s HELP %s yaziniz.
NO_HELP_AVAILABLE
	%s hakkinda yardim bulunamadi.
MISSING_QUOTE
	Tirnak isareti (") bulunamadi.
BAD_EMAIL
	Email adresleri account@domain seklinde olmak zorundadir. Adreslerde kontrol karakterleri (renk, bold, alt cizgi, ters renk), ya da su karakterler kullanilamaz: , : ; | \ " ( ) < > [ ]
REJECTED_EMAIL
	Bu email adresi kullanilamaz.
BAD_URL
	URL bilgileri http://hostname[:port]/... ya da ftp://hostname[:port]/... seklinde olmak zorundadir.
BAD_USERHOST_MASK
	"mask"  kullanici@IP seklinde tanimlanmali.
BAD_NICKUSERHOST_MASK
	"mask" nick!kullanici@IP seklinde tanimlanmali.
SENDMAIL_NO_RESOURCES
	Sistem email gönderebilmek icin cok yüklü durumda. Lütfen daha sonra yeniden deneyiniz.
READ_ONLY_MODE
	Notice: Services read-only calisiyor; degisiklikler kaydedilmeyecek !
PASSWORD_INCORRECT
	Gecersiz sifre.
PASSWORD_WARNING
	DIKKAT: Gecersiz bir sifre daha girerseniz, baglantiniz kesilecektir.
ACCESS_DENIED
	Bu komuta erisiminiz yok.
PERMISSION_DENIED
	Bu komutu kullanma izniniz yok.
MORE_OBSCURE_PASSWORD
	Lütfen daha az kolay bir sifre ile deneyin. Sifreler en azindan bes karakterden olusmali ve kolay tahmin edilir olmamalidir (örnegin isminiz veya nickiniz olmamasina özen gösterin)
BAD_EXPIRY_TIME
	Gecersiz zaman tanimlamasi.
NICK_NOT_REGISTERED
	Nick'iniz kayitli degil.
NICK_NOT_REGISTERED_HELP
	Nick'iniz kayitli degil./msg %s HELP yazarak nasil
	kaydedebileceginiz konusunda ayrintili bilgi alabilirsiniz.
NICK_TOO_LONG
	Nickler %d karakterden daha uzun olamazlar.
NICK_INVALID
	%s gecersiz bir nick.
NICK_X_NOT_REGISTERED
	%s kayitli bir nick degil.
NICK_X_ALREADY_REGISTERED
	%s daha onceden kaydedilmis.
NICK_X_NOT_IN_USE
	%s nick'i su anda kullanimda degil.
NICK_X_FORBIDDEN
	%s kaydedilemez ve/veya kullanilamaz.
NICK_X_SUSPENDED
	%s nicki su an dondurulmus. (suspended)
NICK_X_SUSPENDED_MEMOS
	%s nicki su an dondurulmus durumda oldugu icin memo alamaz.
NICK_IDENTIFY_REQUIRED
	Bu komutu kullanabilmeniz icin sifrenizi girmelisiniz.
	/msg %s IDENTIFY sifreniz yazarak yeniden deneyiniz.
NICK_PLEASE_AUTH
	Nickiniz henüz sisteme tanitilmadi. Lütfen %s adresine gönderilmis olan
	emailden faydanalanarak nickinizi sisteme tanitiniz. (authorize)
NICK_X_NOT_ON_CHAN_X
	%s nicki su an %s kanalinda degil.

CHAN_INVALID
	%s gecersiz bir kanal.
CHAN_X_NOT_REGISTERED
	%s kanali kayitli degil.
CHAN_X_NOT_IN_USE
	%s kanali su an kullanimda degil.
CHAN_X_FORBIDDEN
	%s kanali kaydedilemez ve/veya kullanilamaz.
CHAN_X_SUSPENDED
	%s kanali su an dondurulmus. (suspended)
CHAN_X_SUSPENDED_MEMOS
	%s kanali su an dondurulmus durumda oldugu icin memo alamaz.
CHAN_IDENTIFY_REQUIRED
	Bu komutu kullanabilmeniz icin sifrenizi girmelisiniz.
	(/msg %s IDENTIFY %s sifreniz) yazarak yeniden deneyiniz.

SERV_X_NOT_FOUND
	%s serveri bulunamadi.

EXPIRES_NONE
	Zaman asimi yok.
EXPIRES_NOW
	silindi.
EXPIRES_IN
	%s sonra silinecek.

LIST_RESULTS
	Liste sonu; %d/%d eslesme gosteriliyor.

###########################################################################
#
# NickServ messages
#
###########################################################################

# Automatic messages
NICK_IS_REGISTERED
	Bu kayitli bir nick'tir. Lutfen degistiriniz. (Eger sizinse,/msg %s IDENTIFY sifreniz yaziniz).
NICK_IS_SECURE
	Bu kayitli ve sifre korumali bir nick'tir. Eger sizinse /msg %s IDENTIFY sifreniz yaziniz. Aksi takdirde lutfen baska bir nick seciniz.
NICK_MAY_NOT_BE_USED
	Bu nick'in kullanimi yasaklanmistir.  Lutfen degistiriniz.
DISCONNECT_IN_1_MINUTE
	Eger 1 dakika icinde degistirmezseniz, baglantiniz kesilecektir.
DISCONNECT_IN_20_SECONDS
	Eger 20 saniye icinde degistirmezseniz, baglantiniz kesilecektir.
DISCONNECT_NOW
	Bu nick bir baskasinin adina kayitli oldugundan kullanamazsiniz. Lutfen baska bir nick alarak yeniden baglaniniz.
FORCENICKCHANGE_IN_1_MINUTE
	Eger 1 dakika icinde degistirmezseniz, nickiniz degistirilecektir.
FORCENICKCHANGE_IN_20_SECONDS
	Eger 20 saniye icinde degistirmezseniz, nickiniz degistirilecektir.
FORCENICKCHANGE_NOW
	Bu nick kayitli bir nicktir, siz bu nicki kullanamazsiniz. Nickiniz %s nickine cevrilmistir.
NICK_EXPIRES_SOON
	UYARI: Nickinizin kaydi %s icinde silinecektir. Eger bu nicki kullanmaya devam etmek istiyorsaniz, lütfen /msg %s IDENTIFY komutu ile identify ediniz.
NICK_EXPIRED
	UYARI: Nickinizin kaydi silinmistir. Eger bu nicki kullanmak istiyorsaniz, yeniden kaydediniz.

# REGISTER responses
NICK_REGISTER_SYNTAX
	REGISTER sifre [email]
NICK_REGISTER_REQ_EMAIL_SYNTAX
	REGISTER sifre email
NICK_REGISTRATION_DISABLED
	Nick kaydi gecici olarak devre disidir.
NICK_REGISTRATION_FAILED
	Nick kaydi basarisiz.
NICK_REG_PLEASE_WAIT
	Lütfen %s bekleyerek yeniden deneyiniz.
NICK_REG_PLEASE_WAIT_FIRST
	Lütfen %s bekledikten sonra REGISTER komutunu tekrar
	kullaniniz.
NICK_CANNOT_BE_REGISTERED
	%s nick'i kaydedilemez.
NICK_REGISTER_EMAIL_UNAUTHED
	Belirttiginiz email adresi sisteme henüz tanitilmadi (auth). Lütfen baska nickleri kaydetmeden önce AUTH komutunu kullanarak adresi sisteme tanitiniz.
NICK_REGISTER_TOO_MANY_NICKS
	Cok fazla kayitli nicke sahipsiniz. (%d adet. Max: %d) Daha fazla nick kaydedebilmek icin bazi kayitli nicklerinizi droplamaniz gerekmektedir.
NICK_REGISTERED
	%s nick'i basari ile kaydedilmistir.
NICK_PASSWORD_IS
	Nick sifreniz: %s 'dir. Lutfen sifrenizi unutmayiniz ve kimseye soylemeyiniz.

# IDENTIFY responses
NICK_IDENTIFY_SYNTAX
	IDENTIFY sifre
NICK_IDENTIFY_FAILED
	Tanimlama basarisiz - Sifre gecersiz.
NICK_IDENTIFY_SUCCEEDED
	Sifre kabul edildi.
NICK_IDENTIFY_EMAIL_MISSING
	Nickinizi kullanmaya devam edebilmek icin bir email belirtmeniz gerekmektedir.
	Lütfen simdi su komutu kullanarak bir email belirleyiniz.
	/msg %s SET EMAIL address

# DROP responses
NICK_DROP_SYNTAX
	DROP sifre
NICK_DROP_DISABLED
	Nick kaydi silinmesi gecici olarak devre disidir.
NICK_DROP_WARNING
	Dikkat: DROP komutu ile bütün linkli nickleriniz de silinecektir.
NICK_DROP_FAILED
	Nick'inizin kaydi silinemedi.
NICK_DROPPED_LINKS
	Nick'iniz ve de kayitli bütün linkli nickleriniz silinmistir. (dropped)
NICK_DROPPED
	Nick'inizin kaydi silinmistir (dropped).

# DROPNICK responses
NICK_DROPNICK_SYNTAX
	DROPNICK nick
NICK_X_DROPPED
	%s nick'inin kaydi silinmistir (dropped).
NICK_X_DROPPED_LINKS
	%s ve de linkli tüm nicklerinin kaydi silinmistir. (dropped)

# DROPEMAIL and DROPEMAIL-CONFIRM responses
NICK_DROPEMAIL_SYNTAX
	DROPEMAIL maske
NICK_DROPEMAIL_PATTERN_TOO_LONG
	Belirtilen maske cok uzun (En fazla %d karakter kullanilabilir.)
NICK_DROPEMAIL_NONE
	Verilen maskeye uygun herhangi bir nick bulunamadi.
NICK_DROPEMAIL_COUNT
	Belirtilen maskeye uygun %d adet nick bulundu.
	Silmek icin lütfen /msg %s DROPEMAIL-CONFIRM %s komutunu kullaniniz.
NICK_DROPEMAIL_CONFIRM_SYNTAX
	DROPEMAIL-CONFIRM maske
NICK_DROPEMAIL_CONFIRM_UNKNOWN
	Belirtilen maske anlasilamadi. Lütfen DROPEMAIL-CONFIRM komutunu tekrar kullanmadan önce DROPEMAIL komutu ile maskeyi yeniden ayarlayiniz.
NICK_DROPEMAIL_CONFIRM_DROPPING
	%d adet nickin kaydi siliniyor...
NICK_DROPEMAIL_CONFIRM_DROPPED
	Kayit silme islemi tamamlandi.

# SET responses
NICK_SET_SYNTAX
	SET secenek parametreler
NICK_SET_OPER_SYNTAX
	SET [!nick] secenek parametreler
NICK_SET_DISABLED
	NickServ'un SET komutu gecici olarak devre disidir.
NICK_SET_UNKNOWN_OPTION
	Varolmayan SET secenegi: %s.

# SET PASSWORD responses
NICK_SET_PASSWORD_FAILED
	Sifre degistirilemedi.
NICK_SET_PASSWORD_CHANGED
	Sifreniz degistirildi.
NICK_SET_PASSWORD_CHANGED_TO
	Nick sifresi %s olarak degistirildi.

# SET URL responses
NICK_SET_URL_CHANGED
	%s icin URL %s olarak degistirildi.

# SET EMAIL responses
NICK_SET_EMAIL_PLEASE_WAIT
	Lütfen SET EMAIL komutunu tekrar kullanmadan önce %s bekleyiniz.
NICK_SET_EMAIL_UNAUTHED
	Belirttiginiz email adresi sisteme henüz tanitilmadi (auth). Lütfen AUTH komutunu kullanarak adresi sisteme tanitiniz.
NICK_SET_EMAIL_TOO_MANY_NICKS
	%s adresinden zaten %d adet kayitli nick var (Max: %d). Email adresi degistirilemedi.
NICK_SET_EMAIL_CHANGED
	%s icin email adresi %s olarak degistirildi.

# SET INFO responses
NICK_SET_INFO_CHANGED
	%s nicki icin bilgi satiri degistirildi.

# SET LANGUAGE responses
NICK_SET_LANGUAGE_SYNTAX
	SET LANGUAGE numara
NICK_SET_LANGUAGE_UNKNOWN
	Varolmayan dil numarasi: %d . /msg %s HELP SET LANGUAGE yazarak mevcut dillerin bir listesini alabilirsiniz.
NICK_SET_LANGUAGE_CHANGED
	Dil %s 'ye cevrilmistir.

# SET KILL responses
NICK_SET_KILL_SYNTAX
	SET KILL {ON | QUICK | OFF}
NICK_SET_KILL_IMMED_SYNTAX
	SET KILL {ON | QUICK | IMMED | OFF}
NICK_SET_KILL_ON
	Kill korumasi: AKTIF.
NICK_SET_KILL_QUICK
	Kill korumasi: zaman limitli olarak AKTIF,
NICK_SET_KILL_IMMED
	Aninda Kill korumasi: AKTIF
NICK_SET_KILL_IMMED_DISABLED
	IMMED secenegi bu network'te aktif degil.
NICK_SET_KILL_OFF
	Kill korumasi: IPTAL.

# SET SECURE responses
NICK_SET_SECURE_SYNTAX
	SET SECURE {ON | OFF}
NICK_SET_SECURE_ON
	Secure secenegi: AKTIF.
NICK_SET_SECURE_OFF
	Secure secenegi: IPTAL.

# SET PRIVATE responses
NICK_SET_PRIVATE_SYNTAX
	SET PRIVATE {ON | OFF}
NICK_SET_PRIVATE_ON
	Private secenegi: AKTIF.
NICK_SET_PRIVATE_OFF
	Private secenegi: IPTAL.

# SET NOOP responses
NICK_SET_NOOP_SYNTAX
	SET NOOP {ON | OFF}
NICK_SET_NOOP_ON
	No-op secenegi: AKTIF.
NICK_SET_NOOP_OFF
	No-op secenegi: IPTAL.

# SET HIDE responses
NICK_SET_HIDE_SYNTAX
	SET HIDE {EMAIL | USERMASK | QUIT} {ON | OFF}
NICK_SET_HIDE_EMAIL_ON
	E-mail adresiniz %s INFO listelerinden gizlendi.
NICK_SET_HIDE_EMAIL_OFF
	E-mail adresiniz %s INFO listelerinde gorulebilecek.
NICK_SET_HIDE_MASK_ON
	"last seen" 'iniz %s INFO listelerinden gizlendi.
NICK_SET_HIDE_MASK_OFF
	"last seen" 'iniz %s INFO listelerinde gorulebilecek.
NICK_SET_HIDE_QUIT_ON
	Son "/quit" mesajiniz %s INFO listelerinden gizlendi.
NICK_SET_HIDE_QUIT_OFF
	Son "/quit" mesajiniz %s INFO listelerinde gorulebilecek.

# SET TIMEZONE responses
NICK_SET_TIMEZONE_SYNTAX
	SET TIMEZONE {UTC-offset | zaman-dilimi | DEFAULT}
NICK_SET_TIMEZONE_TO
	Zaman diliminiz UTC%c%d:%02d olarak belirlendi.
	Bu zaman dilimindeki aktüel saat: %s
NICK_SET_TIMEZONE_DEFAULT
	Zaman dilimi bilginiz services standard ayari ile degistirildi.

# SET MAINNICK responses
NICK_SET_MAINNICK_NOT_FOUND
	%s nicki sizin nickinize linkli degil.
NICK_SET_MAINNICK_CHANGED
	Temel nickiniz %s olarak degistirildi.

# SET NOEXPIRE responses
NICK_SET_NOEXPIRE_SYNTAX
	SET [!nick] NOEXPIRE {ON | OFF}
NICK_SET_NOEXPIRE_ON
	%s nick'i zaman asimindan cikarildi
NICK_SET_NOEXPIRE_OFF
	%s nick'i zaman asimina eklendi .

# UNSET resposnes
NICK_UNSET_SYNTAX
	UNSET {URL | EMAIL | INFO}
NICK_UNSET_SYNTAX_REQ_EMAIL
	UNSET {URL | INFO}
NICK_UNSET_OPER_SYNTAX
	UNSET [!nick] {URL | EMAIL | INFO}
NICK_UNSET_OPER_SYNTAX_REQ_EMAIL
	UNSET [!nick] {URL | INFO}
NICK_UNSET_URL
	%s icin URL kaydi silindi.
NICK_UNSET_EMAIL
	%s icin email kaydi silindi.
NICK_UNSET_EMAIL_BAD
	Email kaydi silinemez.
NICK_UNSET_INFO
	%s nicki icin bilgi satiri silindi.
NICK_UNSET_EMAIL_OTHER_BAD
	Baska nicklerin email kayitlari silinemez.


# ACCESS responses
NICK_ACCESS_SYNTAX
	ACCESS {ADD | DEL | LIST} [mask]
NICK_ACCESS_DISABLED
	Nick erisim listesi su an degistirilemez.
NICK_ACCESS_ALREADY_PRESENT
	%s erisim listenizde zaten var.
NICK_ACCESS_REACHED_LIMIT
	Sadece %d aded farkli erisim kaydi ekleyebilirsiniz.
NICK_ACCESS_NO_NICKS
	UYARI: Nick erisim listelerinde ayrica nick belirtilemez. Lütfen maskenin sadece user@host seklinde oldugundan emin olunuz.
NICK_ACCESS_ADDED
	%s erisim listenize eklendi.
NICK_ACCESS_NOT_FOUND
	%s erisim listenizde bulunamadi.
NICK_ACCESS_DELETED
	%s erisim listenizden silindi.
NICK_ACCESS_LIST
	Erisim Listesi:
NICK_ACCESS_LIST_EMPTY
	Erisim listeniz bos.
NICK_ACCESS_LIST_X_EMPTY
	%s nick'inin erisim listesi bos.
NICK_ACCESS_LIST_X
	%s nick'inin erisim listesi:

# LINK responses
NICK_OLD_LINK_SYNTAX
	LINK nick sifre
NICK_LINK_SYNTAX
	LINK nick
NICK_LINK_DISABLED
	Nick baglama (link) gecici olarak devre disi.
NICK_LINK_FAILED
	Baglama (linking) basarisiz.
NICK_CANNOT_BE_LINKED
	%s nicki linklenemez (baglanamaz).
NICK_LINK_SAME
	Aktüel nickinize bir link kuramazsiniz.
NICK_LINK_ALREADY_LINKED
	%s zaten sizin nickinize linkli.
NICK_LINK_IN_USE
	%s nicki su an kullanimda.
NICK_LINK_TOO_MANY
	%d adetten daha fazla nick linkleyemezsiniz.
NICK_LINK_TOO_MANY_NICKS
	Zaten %d adet nick kaydetmissiniz (Max: %d). Link basarisiz oldu.
NICK_LINKED
	%s nicki sizin nickinize linklendi.
NICK_OLD_LINK_SAME
	Bir nick kendine linklenemez!
NICK_OLD_LINKED
	Nick'iniz %s 'e linklendi.
NICK_OLD_LINK_TOO_MANY_CHANNELS
	%s ile nick basarisiz oldu: Kayitli kanal sayisi maksimum olan %d adedi asiyor.

# UNLINK responses
NICK_OLD_UNLINK_SYNTAX
	UNLINK [nick sifre]
NICK_UNLINK_SYNTAX
	UNLINK nick
NICK_UNLINK_OPER_SYNTAX
	UNLINK nick [FORCE]
NICK_UNLINK_DISABLED
	Nick baglantisini kesme (unlink) gecici olarak devre disi.
NICK_UNLINK_FAILED
	Link silme islemi basarisiz oldu.
NICK_OLD_UNLINK_NOT_LINKED
	Nick'iniz bagli (linkli) degil.
NICK_UNLINK_NOT_LINKED
	%s nicki zaten linkli degil.
NICK_OLD_UNLINKED
	Nick'inizin %s ile olan baglantisi (link) silindi.
NICK_X_UNLINKED
	%s nickinin baglantisi (link) silindi.
NICK_UNLINK_SAME
	Aktüel nickinizin baglantisini silemezsiniz (unlink).
NICK_UNLINK_NOT_LINKED_YOURS
	%s sizin nicklerinize linkli degil.
NICK_UNLINKED
	%s nickinin baglantisi kesildi. (unlinked)

# LISTLINKS responses
NICK_OLD_LISTLINKS_SYNTAX
	LISTLINKS nick
NICK_LISTLINKS_SYNTAX
	LISTLINKS
NICK_LISTLINKS_HEADER
	%s nickine linkli nickler:
NICK_LISTLINKS_FOOTER
	%d linkli nick.

# INFO responses
NICK_INFO_SYNTAX
	INFO nick [ALL]
NICK_INFO_REALNAME
	      %s     isim: %s
NICK_INFO_ADDRESS
	Son gorulen adres: %s
NICK_INFO_ADDRESS_ONLINE
	Online oldugu adres: %s
NICK_INFO_ADDRESS_ONLINE_NOHOST
	%s nicki su an online.
NICK_INFO_ADDRESS_OTHER_NICK
	%s nickini kullaniyor.
NICK_INFO_TIME_REGGED
	     Kayit zamani: %s
NICK_INFO_LAST_SEEN
	Son gorulen zaman: %s
NICK_INFO_LAST_QUIT
	  Son quit mesaji: %s
NICK_INFO_INFO
	            Bilgi: %s
NICK_INFO_URL
	              URL: %s
NICK_INFO_EMAIL
	    E-mail adresi: %s
NICK_INFO_EMAIL_UNAUTHED
	    E-mail adresi: %s (onaylanmamis)
NICK_INFO_OPTIONS
	            Ekler: %s
# These strings MUST NOT be empty
NICK_INFO_OPT_KILL
	Kill korumasi
NICK_INFO_OPT_SECURE
	Ek Guvenlik
NICK_INFO_OPT_PRIVATE
	Ozel
NICK_INFO_OPT_NOOP
	No-op
NICK_INFO_OPT_NONE
	YOK
NICK_INFO_NO_EXPIRE
	Bu nick zamanasimina ugramayacak.
NICK_INFO_SUSPEND_DETAILS
	%s tarafindan %s (%s) tarihinde suspend.
NICK_INFO_SUSPEND_REASON
	Gerekce: %s
NICK_INFO_SHOW_ALL
	Daha fazla bilgi icin: /msg %s INFO %s ALL

# LISTCHANS responses
NICK_LISTCHANS_SYNTAX
	LISTCHANS
NICK_LISTCHANS_NONE
	%s hicbir kanal kaydetmemis.
NICK_LISTCHANS_HEADER
	%s nickinin kayitli kanal listesi:
NICK_LISTCHANS_END
	Liste sonu. (%d kanal kaydedilmis).

# LIST/LISTEMAIL responses
NICK_LIST_SYNTAX
	%s pattern
NICK_LIST_OPER_SYNTAX
	%s pattern [FORBIDDEN] [NOEXPIRE] [SUSPENDED]
NICK_LIST_OPER_SYNTAX_AUTH
	%s pattern [FORBIDDEN] [NOEXPIRE] [SUSPENDED] [NOAUTH]
NICK_LIST_HEADER
	%s 'a uyan veriler:
NICK_LIST_NO_MATCH
	Uyan veri bulunamadi.
NICK_LISTEMAIL_NONE
	(bulunamadi)

# RECOVER responses
NICK_RECOVER_SYNTAX
	RECOVER nickiniz [sifreniz]
NICK_NO_RECOVER_SELF
	NickServ'e kendinizi killetemezsiniz!
NICK_RECOVERED
	Nick'inizi kullanan kisinin IRC baglantisi kesilmistir.
	/msg %s RELEASE %s sifreniz yazarak bir dakikalik sureden once nick'inizi alabilirsiniz.

# RELEASE responses
NICK_RELEASE_SYNTAX
	RELEASE nickiniz [sifreniz]
NICK_RELEASE_NOT_HELD
	%s nick'i zaten serbest.
NICK_RELEASED
	Nickiniz serbest birakildi.

# GHOST responses
NICK_GHOST_SYNTAX
	GHOST nickiniz [sifreniz]
NICK_NO_GHOST_SELF
	NickServ'e kendinizi killetemezsiniz!
NICK_GHOST_KILLED
	Asili kalan nick'inizin baglantisi kesilmistir.

# GETPASS responses
NICK_GETPASS_SYNTAX
	GETPASS nick
NICK_GETPASS_UNAVAILABLE
	%s nick'i icin GETPASS komutu sifreleme (encryption) devrede oldugundan kullanilanamaz.
NICK_GETPASS_PASSWORD_IS
	%s nick'inin sifresi : %s.

# FORBID responses
NICK_FORBID_SYNTAX
	FORBID nick
NICK_FORBID_SUCCEEDED
	%s nick'inin kullanimi yasaklandi.
NICK_FORBID_FAILED
	%s nick'inin kullanimi yasaklanamadi!
	
# SUSPEND responses
NICK_SUSPEND_SYNTAX
	SUSPEND [+expiry] nick gerekce
NICK_SUSPEND_SUCCEEDED
	%s nicki ve de ona linkli tüm nicklerin services ile
	iliskisi kesildi (suspend).
NICK_SUSPEND_ALREADY_SUSPENDED
	%s nicki zaten suspend altinda.

# UNSUSPEND responses
NICK_UNSUSPEND_SYNTAX
	UNSUSPEND nick
NICK_UNSUSPEND_SUCCEEDED
	%s nicki tekrar identify edilebilir hale getirildi.
NICK_UNSUSPEND_NOT_SUSPENDED
	%s nicki suspend edilmedi.


# AJOIN responses
NICK_AJOIN_SYNTAX
	AJOIN {ADD | DEL | LIST} [channel]
NICK_AJOIN_ADD_SYNTAX
	AJOIN ADD channel
NICK_AJOIN_DEL_SYNTAX
	AJOIN DEL channel
NICK_AJOIN_DISABLED
	Nick otojoin listesi su an degistirilemez.
NICK_AJOIN_ALREADY_PRESENT
	%s kanali zaten otojoin listenizde var.
NICK_AJOIN_LIST_FULL
	Sadece %d adet kanal kaydedebilirsiniz.
NICK_AJOIN_ADDED
	%s kanali otojoin listenize eklendi.
NICK_AJOIN_NOT_FOUND
	%s kanali otojoin listenizde bulunamadi.
NICK_AJOIN_DELETED
	%s kanali otojoin listenizden silindi.
NICK_AJOIN_LIST
	Otojoin listesi:
NICK_AJOIN_LIST_EMPTY
	Otojoin listesi bos.
NICK_AJOIN_LIST_X
	%s nicki otojoin listesi:
NICK_AJOIN_LIST_X_EMPTY
	%s nickinin otojoin listesi bos.
NICK_AJOIN_AUTO_REMOVE
	%s kanali gecerli bir kanal olmadigindan otojoin listenizden silindi.

################ mail-auth module messages/responses

# General-purpose messages
NICK_AUTH_SENDING
	Nickiniz icin auth (tanitim) kodu %s adresine gonderilmistir.
NICK_AUTH_FOR_REGISTER
	Bu mesaji aliyorsaniz, /msg %s AUTH code (code yerine emailde gelen
	authorization kodunu girmeniz gerekmektedir.) komutunu girerek
	nick kayit isleminizi tamamlayabilirsiniz.
NICK_AUTH_FOR_SET_EMAIL
	Bu mesaji aliyorsaniz, /msg %s AUTH code (code yerine emailde gelen
	authorization kodunu girmeniz gerekmektedir.) komutunu girerek
	email degisim isleminizi tamamlayabilirsiniz.
NICK_AUTH_NOT_NEEDED
	Auth etmek (tanitmak) gereken herhangi bir islem yok.
NICK_AUTH_NO_AUTHCODE
	%s nickinin auth (tanitim) kodu yok.
NICK_AUTH_HAS_AUTHCODE
	%s nickinin zaten bir auth (tanitim) kodu var.
PASSWORD_WARNING_FOR_AUTH
	Dikkat: Bir hatali auth (tanitim) denemesi daha yaparsiniz, baglantiniz kesilecektir.

# Mail text.  The last "%s" (before the user@host) in the body text is
# replaced by one of the NICK_AUTH_MAIL_TEXT_* messages.

NICK_AUTH_SYNTAX
	AUTH kod
NICK_AUTH_MAIL_SUBJECT
	%s nicki icin tanitim kodu.
NICK_AUTH_MAIL_BODY
	%s nickinin tanitim kodu: %09d
	Lütfen bu kodu, %s 'e asagidaki komutu kullanarak belirtiniz.
	    /msg %s AUTH %09d
	
	Bu email %s tarafindan %s islemi icin gönderilmistir.
	(Gönderen %s@%s).

NICK_AUTH_MAIL_TEXT_REG
	kayit
NICK_AUTH_MAIL_TEXT_EMAIL
	email degisikligi
NICK_AUTH_MAIL_TEXT_SENDAUTH
	SENDAUTH
NICK_AUTH_MAIL_TEXT_REAUTH
	REAUTH
NICK_AUTH_MAIL_BODY_SETAUTH
	%s nickinin auth (tanitim) kodu: %09d
	Lütfen bu kodu %s 'e asagidaki komutu kullanarak gönderiniz.
	    /msg %s AUTH %09d

# AUTH responses
NICK_AUTH_DISABLED
	Nick tanitim islemleri (auth) gecici olarak kullanim disidir.
NICK_AUTH_FAILED
	Auth (tanitim) kodu gecersiz.
NICK_AUTH_SUCCEEDED_REGISTER
	Auth (tanitim) basarili oldu: nickinizin kaydi tamamlandi.
NICK_AUTH_SUCCEEDED_SET_EMAIL
	Auth (tanitim) basarili oldu: email adresiniz degistirildi.
NICK_AUTH_SUCCEEDED_REAUTH
	Auth (tanitim) basarili oldu: simdi nickinizi identify edebilirsiniz.
NICK_AUTH_SUCCEEDED_SETAUTH
	Auth (tanitim) basarili oldu: nickinizi kullanmaya devam edebilisiniz.

# SENDAUTH responses
NICK_SENDAUTH_SYNTAX
	SENDAUTH
NICK_SENDAUTH_TOO_SOON
	Lütfen SENDAUTH komutunu tekrar kullanmadan önce %s bekleyiniz.
NICK_SENDAUTH_NORESOURCES
	Auth (tanitim) kodunuzu iceren mesaj sistemin cok mesgul olmasindan dolayi gönderilemedi. Lutfen daha sonra tekrar deneyiniz veya bir network admin ile irtibata geciniz.
NICK_SENDAUTH_ERROR
	Auth (tanitim) kodunuzu iceren mesaj bir sistem problemi ile karsilasildigindan gönderilemedi. Lutfen daha sonra tekrar deneyiniz veya bir network admin ile irtibata geciniz.

# REAUTH responses
NICK_REAUTH_SYNTAX
	REAUTH
NICK_REAUTH_HAVE_AUTHCODE
	Nickiniz icin auth (tanitim) kodu zaten gonderilmistir.
NICK_REAUTH_NO_EMAIL
	Nickiniz icin belirlenmis bir email adresi yok.
NICK_REAUTH_AUTHCODE_SET
	Nickiniz icin yeni bir auth (tanitim) kodu belirlendi ve en kisa surede email adresinize gonderilecektir. Nickinizi identify etmek icin bu kodu AUTH komutu ile birlikte kullaniniz.

# RESTOREMAIL responses
NICK_RESTOREMAIL_SYNTAX
	RESTOREMAIL sifre
NICK_RESTOREMAIL_NOT_NOW
	RESTOREMAIL sadece yeni bir email adresi tanitma öncesi ve email adresi degistirme sonrasi kullanilabilir.
NICK_RESTOREMAIL_DONE
	Sisteme tanittiginiz son email adresinize %s dönüldü.

# SETAUTH responses
NICK_SETAUTH_SYNTAX
	SETAUTH nick
NICK_SETAUTH_NO_EMAIL
	%s nickinin email adresi yok.
NICK_SETAUTH_AUTHCODE_SET
	Yeni bir auth (tanitim) kodu (%09d) %s nicki icin belirlendi.
NICK_SETAUTH_SEND_REFUSED
	Dikkat: %s icin mail gönderilmedi. (mesaj mail server'i tarafindan reddedildi).
NICK_SETAUTH_SEND_TIMEOUT
	Dikkat: %s icin mail gönderilemedi. (baglantida veya server'a gönderimde zaman asimi).
NICK_SETAUTH_SEND_NORESOURCES
	Dikkat: %s icin mail gönderilemedi (asiri yük).
NICK_SETAUTH_SEND_ERROR
	Dikkat: %s icin mail gönderilemedi (bilinmeyen hata).
NICK_SETAUTH_USER_NOTICE
	Nickinizi kullanmaya devam edebilmek icin auth (tanitma) islemini
	yapmaniz gerekmektedir. Bu islem icin gerekli kod %s adresine
	gönderilmistir. Daha fazla bilgi icin /msg %s HELP AUTH
	yazabilirsiniz.

# GETAUTH responses
NICK_GETAUTH_SYNTAX
	GETAUTH nick
NICK_GETAUTH_AUTHCODE_IS
	%s nicki icin auth (tanitim) kodu: %09d

# CLEARAUTH responses
NICK_CLEARAUTH_SYNTAX
	CLEARAUTH nick
NICK_CLEARAUTH_CLEARED
	%s nickinin auth (tanitim) kodu silindi.

###########################################################################
#
# ChanServ messages
#
###########################################################################

# Access level descriptions
CHAN_LEVEL_AUTOOP
	Kanala giriste verilen otomatik op. (+o)
CHAN_LEVEL_AUTOVOICE
	Kanala giriste verilen otomatik voice. (+v)
CHAN_LEVEL_INVITE
	INVITE komutunu kullanmaya yetkili.
CHAN_LEVEL_AKICK
	AKICK komutunu kullanmaya yetkili.
CHAN_LEVEL_SET
	SET komutunu kullanmaya yetkili (FOUNDER/PASSWORD haric).
CHAN_LEVEL_CLEAR
	CLEAR komutunu kullanmaya yetkili.
CHAN_LEVEL_UNBAN
	UNBAN komutunu kullanmaya yetkili.
CHAN_LEVEL_OPDEOP
	OP/DEOP komutlarini kullanmaya yetkili.
CHAN_LEVEL_ACCESS_LIST
	Kanalin erisim listesini (access list) gormeye yetkili.
CHAN_LEVEL_ACCESS_CHANGE
	Kanalin erisim listesini (access list) degistirmeye yetkili.
CHAN_LEVEL_MEMO
	Kanala gelen notlari okumaya yetkili.
CHAN_LEVEL_VOICE
	VOICE/DEVOICE komutlarini kullanmaya yetkili.
CHAN_LEVEL_AUTOHALFOP
	Kanala giriste verilen otomatik halfop. (+h)
CHAN_LEVEL_HALFOP
	HALFOP/DEHALFOP komutlarini kullanmaya yetkili.
CHAN_LEVEL_AUTOPROTECT
	Otomatik kanal koruma (+a) modu.
CHAN_LEVEL_PROTECT
	PROTECT/DEPROTECT komutlarini kullanma hakki.
CHAN_LEVEL_KICK
	KICK komutunu kullanabilmeye yetkili.
CHAN_LEVEL_STATUS
	STATUS komutunu kullanabilmeye yetkili.
CHAN_LEVEL_TOPIC
	TOPIC komutunu kullanabilmeye yetkili.


# Automatic responses
CHAN_IS_REGISTERED
	Bu kanal %s ile kaydedilmistir.
CHAN_MAY_NOT_BE_USED
	Bu kanal kullanilamaz.
CHAN_NOT_ALLOWED_TO_JOIN
	Bu kanalda bulunmaya yetkili degilsiniz.
CHAN_BOUNCY_MODES
	%s komutu su an kullanim disi.

# REGISTER responses
CHAN_REGISTER_SYNTAX
	REGISTER kanal sifre aciklama
CHAN_REGISTER_DISABLED
	Kanal kaydi (REGISTER) gecici olarak devre disidir.
CHAN_REGISTER_SHORT_CHANNEL
	# kanali kaydedilemez.
CHAN_REGISTER_NOT_LOCAL
	Local kanallar kaydedilemezler.
CHAN_REGISTER_INVALID_NAME
	Sadece isimleri # ile baslayan kanallari kaydedebilirsiniz.
CHAN_MUST_REGISTER_NICK
	Oncelikle nick'inizi kaydetmelisiniz. /msg %s HELP yazarak daha fazla bilgi alabilirsiniz.
CHAN_MUST_IDENTIFY_NICK
	Lutfen once /msg %s IDENTIFY sifre
	komutu ile kendinizi tanitiniz.
CHAN_MAY_NOT_BE_REGISTERED
	%s kanali kaydedilemez.
CHAN_ALREADY_REGISTERED
	%s kanali daha onceden kaydedilmis.
CHAN_MUST_BE_CHANOP
	Kanali kaydedebilmek icin op olmalisiniz.
CHAN_REACHED_CHANNEL_LIMIT
	%d kanallik limitinizi doldurdunuz.
CHAN_EXCEEDED_CHANNEL_LIMIT
	%d kanallik limitinizi astiniz.
CHAN_REGISTRATION_FAILED
	Kanal kaydi (registration) basarisiz.
CHAN_REGISTERED
	%s kanali %s nick'inize kaydedilmistir.
CHAN_PASSWORD_IS
	Kanal sifrenizi ( %s ) kimseye soylemeyiniz ve unutmayiniz.

# IDENTIFY responses
CHAN_IDENTIFY_SYNTAX
	IDENTIFY kanal sifre
CHAN_IDENTIFY_FAILED
	Gecersiz sifre.
CHAN_IDENTIFY_SUCCEEDED
	Sifre kabul edildi : %s kanalina en yuksek duzeyde erisim kazandiniz.

# DROP responses
CHAN_DROP_SYNTAX
	DROP kanal sifre
CHAN_DROP_DISABLED
	Kanal kaydinin silinmesi (DROP) gecici olarak devre disidir.
CHAN_DROPPED
	%s isimli kanalinizin kaydi silinmistir (dropped).

# DROPCHAN responses
CHAN_DROPCHAN_SYNTAX
	DROPCHAN kanal

# SET responses
CHAN_SET_SYNTAX
	SET kanal secenek parametre
CHAN_SET_DISABLED
	Set komutu gecici olarak devre disidir.
CHAN_SET_UNKNOWN_OPTION
	Gecersiz SET ozelligi : %s.

# SET FOUNDER responses
CHAN_SET_FOUNDER_TOO_MANY_CHANS
	%s cok fazla kanala sahip.
CHAN_FOUNDER_CHANGED
	%s kanalinin sahibi (founder) %s olarak degisti.

# SET SUCCESSOR responses
CHAN_SUCCESSOR_CHANGED
	%s kanali successor olarak %s belirlendi.
CHAN_SUCCESSOR_IS_FOUNDER
	Bir kanal kurucusu ayni zamanda successor olamz.

# SET PASSWORD responses
CHAN_SET_PASSWORD_FAILED
	Sifre degisimi basarisiz.
CHAN_PASSWORD_CHANGED
	%s kanalinin sifresi degisti.
CHAN_PASSWORD_CHANGED_TO
	%s kanalinin sifresi %s olarak degisti.

# SET DESC responses
CHAN_DESC_CHANGED
	%s kanalinin aciklamasi (Description) %s olarak degisti.

# SET URL responses
CHAN_URL_CHANGED
	%s kanalinin web adresi (URL) %s olarak degisti.

# SET EMAIL responses
CHAN_EMAIL_CHANGED
	%s kanalinin E-Mail adresi %s olarak degisti.

# SET ENTRYMSG responses
CHAN_ENTRY_MSG_CHANGED
	%s kanali icin giris mesaji degisti.

# SET MLOCK responses
CHAN_SET_MLOCK_NEED_PLUS_MINUS
	Mode belirtirken + ve ya - isaretleri kullanmak gerekmektedir.
CHAN_SET_MLOCK_NEED_PARAM
	+%c modu icin parametre gerekiyor.
CHAN_SET_MLOCK_NEED_POSITIVE
	MLOCK +%c olarak pozitif bir sayi kullanmalisiniz.
CHAN_SET_MLOCK_MODE_REG_BAD
	Mode +%c daima kayitli kanallar icin kullanilir, MLOCK ile tanimlanamaz.
CHAN_SET_MLOCK_LINK_BAD
	MLOCK icin +%c parametersi gecerli bir kanal ismi degil.
CHAN_SET_MLOCK_LINK_SAME
	MLOCK icin +%c farkli bir kanal olmak zorunda.
CHAN_SET_MLOCK_REQUIRES
	+%c icin belirtilen mod kilidinde ayrica +%c de bulunmalidir.
CHAN_SET_MLOCK_BAD_PARAM
	+%c modu icin hatali parametre verildi.
CHAN_SET_MLOCK_UNKNOWN_CHAR
	Varolmayan mode %c dikkate alinmadi.
CHAN_SET_MLOCK_CANNOT_LOCK
	%c modu kitlenemez.
CHAN_MLOCK_CHANGED
	%s kanalindaki "mode lock" %s olarak degisti.
CHAN_MLOCK_REMOVED
	%s kanalindaki "mode lock" silindi.

# SET HIDE responses
CHAN_SET_HIDE_SYNTAX
	SET kanal HIDE {EMAIL | TOPIC | MLOCK} {ON | OFF}
CHAN_SET_HIDE_EMAIL_ON
	%s kanal email bilgisi %s infosunda gizlenecek.
CHAN_SET_HIDE_EMAIL_OFF
	%s kanal email bilgisi %s infosunda gösterilecek.
CHAN_SET_HIDE_TOPIC_ON
	%s kanal topic bilgisi %s infosunda gizlenecek.
CHAN_SET_HIDE_TOPIC_OFF
	%s kanal topic bilgisi %s infosunda gösterilecek.
CHAN_SET_HIDE_MLOCK_ON
	%s kanal modlari bilgisi %s infosunda gizlenecek.
CHAN_SET_HIDE_MLOCK_OFF
	%s kanal modlari bilgisi %s infosunda gösterilecek.

# SET KEEPTOPIC responses
CHAN_SET_KEEPTOPIC_SYNTAX
	SET kanal KEEPTOPIC {ON | OFF}
CHAN_SET_KEEPTOPIC_ON
	Topic hatirlama (retention) AKTIF.
CHAN_SET_KEEPTOPIC_OFF
	Topic hatirlama (retention) KAPALI.

# SET TOPICLOCK responses
CHAN_SET_TOPICLOCK_SYNTAX
	SET kanal TOPICLOCK {ON | OFF}
CHAN_SET_TOPICLOCK_ON
	Topic kilidi (topiclock) AKTIF.
CHAN_SET_TOPICLOCK_OFF
	Topic kilidi (topiclock) KAPALI.

# SET PRIVATE responses
CHAN_SET_PRIVATE_SYNTAX
	SET kanal PRIVATE {ON | OFF}
CHAN_SET_PRIVATE_ON
	Ozel (PRIVATE) AKTIF.
CHAN_SET_PRIVATE_OFF
	Ozel (PRIVATE) KAPALI.

# SET SECUREOPS responses
CHAN_SET_SECUREOPS_SYNTAX
	SET kanal SECUREOPS {ON | OFF}
CHAN_SET_SECUREOPS_ON
	Kisitli Op (SECUREOPS) AKTIF.
CHAN_SET_SECUREOPS_OFF
	Kisitli Op (SECUREOPS) KAPALI.

# SET LEAVEOPS responses
CHAN_SET_LEAVEOPS_SYNTAX
	SET kanal LEAVEOPS {ON | OFF}
CHAN_SET_LEAVEOPS_ON
	Leave ops opsiyonu AKTIF.
CHAN_SET_LEAVEOPS_OFF
	Leave ops opsiyonu KAPALI.

# SET RESTRICTED responses
CHAN_SET_RESTRICTED_SYNTAX
	SET kanal RESTRICTED {ON | OFF}
CHAN_SET_RESTRICTED_ON
	Kisitli erisim AKTIF.
CHAN_SET_RESTRICTED_OFF
	Kisitli Erisim KAPALI.

# SET SECURE responses
CHAN_SET_SECURE_SYNTAX
	SET kanal SECURE {ON | OFF}
CHAN_SET_SECURE_ON
	Guvenlik AKTIF.
CHAN_SET_SECURE_OFF
	Guvenlik KAPALI.

# SET OPNOTICE responses
CHAN_SET_OPNOTICE_SYNTAX
	SET kanal OPNOTICE {ON | OFF}
CHAN_SET_OPNOTICE_ON
	Op-notice opsiyonu AKTIF.
CHAN_SET_OPNOTICE_OFF
	Op-notice opsiyonu KAPALI.
	
# SET ENFORCE responses
CHAN_SET_ENFORCE_SYNTAX
	SET channel ENFORCE {ON | OFF}
CHAN_SET_ENFORCE_ON
	Mod-Koruma (enforce) opsiyonu ACIK.
CHAN_SET_ENFORCE_OFF
	Mod-Koruma (enforce) opsiyonu KAPALI.

# SET MEMO-RESTRICTED responses
CHAN_SET_MEMO_RESTRICTED_SYNTAX
	SET kanal MEMO-RESTRICTED {ON | OFF}
CHAN_SET_MEMO_RESTRICTED_ON
	Kisitli not secenegi AKTIF.
CHAN_SET_MEMO_RESTRICTED_OFF
	Kisitli not secenegi IPTAL.

# SET NOEXPIRE responses
CHAN_SET_NOEXPIRE_SYNTAX
	SET kanal NOEXPIRE {ON | OFF}
CHAN_SET_NOEXPIRE_ON
	%s kanali zaman asimindan cikarildi .
CHAN_SET_NOEXPIRE_OFF
	%s kanali zaman asimina eklendi.

# UNSET responses
CHAN_UNSET_SYNTAX
	UNSET kanal {SUCCESSOR | URL | EMAIL | ENTRYMSG}
CHAN_SUCCESSOR_UNSET
	%s icin successor kaldirildi.
CHAN_URL_UNSET
	%s kanalinin web adresi (URL) silindi.
CHAN_EMAIL_UNSET
	%s kanalinin E-mail adresi silindi.
CHAN_ENTRY_MSG_UNSET
	%s kanali icin giris mesaji silindi.

# ACCESS messages
CHAN_ACCESS_SYNTAX
	ACCESS kanal {ADD | DEL | LIST | LISTLEVEL | COUNT} [parametre]
CHAN_ACCESS_ADD_SYNTAX
	ACCESS kanal ADD nick seviye
CHAN_ACCESS_DEL_SYNTAX
	ACCESS kanal DEL nick
CHAN_ACCESS_LIST_SYNTAX
	ACCESS kanal LIST [maske]
CHAN_ACCESS_LISTLEVEL_SYNTAX
	ACCESS kanal LISTLEVEL seviye
CHAN_ACCESS_COUNT_SYNTAX
	ACCESS kanal COUNT
CHAN_ACCESS_DISABLED
	Kanal erisim listesinin degistirilmesi gecici olarak devre disidir.
CHAN_ACCESS_LEVEL_NONZERO
	Erisim seviyesi sifirdan farkli bir sayi olmalidir.
CHAN_ACCESS_LEVEL_RANGE
	Erisim seviyesi %d ve %d arasinda olmalidir.
CHAN_ACCESS_REACHED_LIMIT
	Erisim listesinde en fazla %d adet nick bulunabilir.
CHAN_ACCESS_NICKS_ONLY
	Kanalin erisim listesi sadece kayitli (registered) nick'lerden olusabilir.
CHAN_ACCESS_NOOP
	%s kanal erisim listenize eklenemez.
CHAN_ACCESS_LEVEL_UNCHANGED
	%s nick'inin %s kanalindaki erisim seviyesi zaten %d.
CHAN_ACCESS_LEVEL_CHANGED
	%s nick'inin %s kanalindaki erisim seviyesi %d olarak degisti.
CHAN_ACCESS_ADDED
	%s nick'i %s kanalinin erisim listesine %d seviyeden eklendi.
CHAN_ACCESS_NOT_FOUND
	%s  %s kanalinin erisim listesinde bulunamadi.
CHAN_ACCESS_DELETED
	%s  %s kanalinin erisim listesinden silindi.
CHAN_ACCESS_LIST_EMPTY
	%s kanalinin erisim listesi bos.
CHAN_ACCESS_NO_MATCH
	%s erisim listesinde belirtilen tanima uyan kayit bulunamadi.
CHAN_ACCESS_LIST_HEADER
	%s Kanali Erisim Listesi:
	  Seviye  Nick
CHAN_ACCESS_LIST_FORMAT
	   %4d   %s
CHAN_ACCESS_COUNT
	%s kanali erisim listesinde %d adet kayit var.

# XOP (SOP/AOP/etc.) responses

CHAN_SOP_SYNTAX
	SOP kanal {ADD | DEL | LIST | COUNT} [parametre]
CHAN_AOP_SYNTAX
	AOP kanal {ADD | DEL | LIST | COUNT} [parametre]
CHAN_HOP_SYNTAX
	HOP kanal {ADD | DEL | LIST | COUNT} [parametre]
CHAN_VOP_SYNTAX
	VOP kanal {ADD | DEL | LIST | COUNT} [parametre]
CHAN_NOP_SYNTAX
	NOP kanal {ADD | DEL | LIST | COUNT} [parametre]
CHAN_SOP_LIST_SYNTAX
	SOP kanal {ADD | DEL | LIST | COUNT} [maske]
CHAN_AOP_LIST_SYNTAX
	AOP kanal {ADD | DEL | LIST | COUNT} [maske]
CHAN_HOP_LIST_SYNTAX
	HOP kanal {ADD | DEL | LIST | COUNT} [maske]
CHAN_VOP_LIST_SYNTAX
	VOP kanal {ADD | DEL | LIST | COUNT} [maske]
CHAN_NOP_LIST_SYNTAX
	NOP kanal {ADD | DEL | LIST | COUNT} [maske]
CHAN_XOP_ADDED
	%s %s %s listesine eklendi.
CHAN_XOP_LEVEL_CHANGED
	%s %s %s listesine eklendi.
CHAN_XOP_LEVEL_UNCHANGED
	%s zaten %s %s listesinde.
CHAN_XOP_NICKS_ONLY
	AOP, SOP, VOP ve NOP listelerine sadece kayitli nickler eklenebilir.
CHAN_XOP_NICKS_ONLY_HOP
	AOP, SOP, HOP, VOP ve NOP listelerine sadece kayitli nickler eklenebilir.
CHAN_XOP_NOOP
	%s kanal erisim listenize eklenemez.
CHAN_XOP_REACHED_LIMIT
	AOP, SOP, VOP ve NOP listelerindeki kayit sayisi %d yi asmamalidir.
CHAN_XOP_REACHED_LIMIT_HOP
	AOP, SOP, HOP, VOP ve NOP listelerindeki kayit sayisi %d yi asmamalidir.
CHAN_XOP_NOT_FOUND
	%s %s %s listesinde bulunamadi.
CHAN_XOP_DELETED
	%s %s %s listesinden silindi.
CHAN_XOP_LIST_EMPTY
	%s %s listesi bos.
CHAN_XOP_NO_MATCH
	%s %s listesinde belirtilen tanima uyan kayit bulunamadi.
CHAN_XOP_LIST_HEADER
	%s %s listesi:
CHAN_XOP_COUNT
	%s kanali %s listesinde %d adet kayit var.

# AKICK responses
CHAN_AKICK_SYNTAX
	AKICK kanal {ADD | DEL | LIST | VIEW | COUNT | ENFORCE} [parametre]
CHAN_AKICK_LIST_SYNTAX
	AKICK kanal LIST [maske]
CHAN_AKICK_VIEW_SYNTAX
	AKICK kanal VIEW [maske]
CHAN_AKICK_DISABLED
	Kanal otomatik kick (akick) listesinin degistirilmesi gecici olarak devre disidir.
CHAN_AKICK_ALREADY_EXISTS
	%s nick'i %s kanalinin akick listesinde zaten var.
CHAN_AKICK_REACHED_LIMIT
	Bir kanalin akick listesinde en fazla %d kisi olabilir.
CHAN_AKICK_ADDED
	%s maskesi %s kanalinin akick listesine eklendi.
CHAN_AKICK_NOT_FOUND
	%s  %s kanalinin akick listesinde bulunamadi.
CHAN_AKICK_DELETED
	%s  %s kanalinin akick listesinden silindi.
CHAN_AKICK_LIST_EMPTY
	%s akick listesi bos.
CHAN_AKICK_NO_MATCH
	%s akick listesinde belirtilen tanima uyan bir akick bulunamadi.
CHAN_AKICK_LIST_HEADER
	%s kanalinin akick listesi:
# mask, set-by, set-time, last-used, reason
CHAN_AKICK_VIEW_FORMAT
	    %s (%s tarafindan %s tarihinde; son kullanim %s)
	        %s
# mask, set-by, set-time, reason
CHAN_AKICK_VIEW_UNUSED_FORMAT
	    %s (%s tarafindan %s tarihinde; hic kullanilmadi)
	        %s
CHAN_AKICK_COUNT
	%s AKICK listesinde %d adet akick bulunmakta.
CHAN_AKICK_ENFORCE_DONE
	%s icin AKICK ENFORCE tamamlandi; %d kullanici bundan etkilendi.

# LEVELS responses
CHAN_LEVELS_SYNTAX
	LEVELS kanal {SET | DIS[ABLE] | LIST | RESET} [ozellik [seviye]]
CHAN_LEVELS_READONLY
	Kanal erisim derecesi listesi su an degistirilemez.
CHAN_LEVELS_RANGE
	Seviye %d ve %d arasinda olmalidir.
CHAN_LEVELS_CHANGED
	%s icin %s kanalinda gereken seviye %d olarak degisti.
CHAN_LEVELS_UNKNOWN
	Varolmayan secenek %s .  /msg %s HELP LEVELS DESC komutu ile gecerli listeyi aliniz.
CHAN_LEVELS_DISABLED
	%s %s kanalinda iptal edildi.
CHAN_LEVELS_LIST_HEADER
	%s kanalindaki erisim haklari:
CHAN_LEVELS_LIST_DISABLED
	    %-*s  (devre disi)
CHAN_LEVELS_LIST_FOUNDER
	    %-*s  (sadece kurucu)
CHAN_LEVELS_LIST_NORMAL
	    %-*s  %4d
CHAN_LEVELS_RESET
	%s icin gereken erisim haklari sifirlanmistir.

# INFO responses
CHAN_INFO_SYNTAX
	INFO kanal [ALL]
CHAN_INFO_HEADER
	  %s Kanali Bilgisi:
CHAN_INFO_FOUNDER
	   Kanal Sahibi: %s
CHAN_INFO_SUCCESSOR
	      Successor: %s
CHAN_INFO_DESCRIPTION
	       Aciklama: %s
CHAN_INFO_ENTRYMSG
	   Giris mesaji: %s
CHAN_INFO_TIME_REGGED
	   Kayit zamani: %s
CHAN_INFO_LAST_USED
	   Son Kullanim: %s
CHAN_INFO_LAST_TOPIC
	      Son Topic: %s
CHAN_INFO_TOPIC_SET_BY
	   Topici yazan: %s
CHAN_INFO_URL
	            URL: %s
CHAN_INFO_EMAIL
	  E-mail adresi: %s
CHAN_INFO_OPTIONS
	          Ekler: %s
CHAN_INFO_OPT_PRIVATE
	  Gizli (LIST komutunda cikmaz)
CHAN_INFO_OPT_KEEPTOPIC
	  Topic Hatirlama (retention)
CHAN_INFO_OPT_TOPICLOCK
	  Topic Kilidi (topiclock)
CHAN_INFO_OPT_SECUREOPS
	  Kisitli Op
CHAN_INFO_OPT_LEAVEOPS
	  Leave Ops
CHAN_INFO_OPT_RESTRICTED
	  Kisitli Giris
CHAN_INFO_OPT_SECURE
	  Ek guvenlik
CHAN_INFO_OPT_OPNOTICE
	  Op-Notice
CHAN_INFO_OPT_ENFORCE
	  Mod-Koruma (enforce)
CHAN_INFO_OPT_MEMO_RESTRICTED
	  Kisitli Notlar
CHAN_INFO_OPT_NONE
	  YOK
CHAN_INFO_MODE_LOCK
	  Mode Kilidi: %s
CHAN_INFO_NO_EXPIRE
	Bu kanal zamanasimina ugramayacak.
CHAN_INFO_SUSPEND_DETAILS
	Bu kanal %s tarafindan %s tarihinde donduruldu. (%s)
CHAN_INFO_SUSPEND_REASON
	Gerekce: %s
CHAN_INFO_SHOW_ALL
	Daha fazla bilgi icin: /msg %s INFO %s ALL

# LIST responses
CHAN_LIST_SYNTAX
	LIST degisken
CHAN_LIST_OPER_SYNTAX
	LIST degisken [NOEXPIRE] [FORBIDDEN] [SUSPENDED]
CHAN_LIST_HEADER
	%s 'a uyan verilerin listesi:
CHAN_LIST_NO_MATCH
	Uyan kanal bulunamadi.

# INVITE responses
CHAN_INVITE_SYNTAX
	INVITE kanal
CHAN_INVITE_OK
	%s %s kanalina davet ediliyor.

# OP/DEOP/VOICE/DEVOICE responses
CHAN_OPVOICE_SYNTAX
	%s kanal nick
CHAN_OP_SUCCEEDED
	%s %s kanalinda oplandi.
CHAN_OP_FAILED
	%s %s kanalinda oplanamadi.
CHAN_OP_ALREADY
	%s %s kanalinda zaten op.
CHAN_DEOP_SUCCEEDED
	%s %s kanalinda deoplandi.
CHAN_DEOP_FAILED
	%s %s kanalinda deoplanamadi.
CHAN_DEOP_ALREADY
	%s %s kanalinda zaten op degil.
CHAN_VOICE_SUCCEEDED
	%s %s kanalinda voice aldi.
CHAN_VOICE_FAILED
	%s %s kanalinda voice alamadi.
CHAN_VOICE_ALREADY
	%s %s kanalinda zaten voice sahibi.
CHAN_DEVOICE_SUCCEEDED
	%s %s kanalinda devoice edildi.
CHAN_DEVOICE_FAILED
	%s %s kanalinda devoice edilemedi.
CHAN_DEVOICE_ALREADY
	%s %s kanalinda zaten voice sahibi degil.
CHAN_HALFOP_SUCCEEDED
	%s %s kanalinda halfop aldi.
CHAN_HALFOP_FAILED
	%s %s kanalinda halfop alamadi.
CHAN_HALFOP_ALREADY
	%s %s kanalinda zaten halfop.
CHAN_DEHALFOP_SUCCEEDED
	%s %s kanalinda dehalfop edildi.
CHAN_DEHALFOP_FAILED
	%s %s kanalinda dehalfop edilemedi.
CHAN_DEHALFOP_ALREADY
	%s %s kanalinda zaten halfop degil.
CHAN_PROTECT_SUCCEEDED
	%s nickine %s kanalinda koruma verildi.
CHAN_PROTECT_FAILED
	%s nickine %s kanalinda koruma verilemedi.
CHAN_PROTECT_ALREADY
	%s %s kanalinda zaten koruma altinda.
CHAN_DEPROTECT_SUCCEEDED
	%s nickindeki koruma %s kanalinda kaldirildi.
CHAN_DEPROTECT_FAILED
	%s nickindeki koruma %s kanalinda kaldirilamadi.
CHAN_DEPROTECT_ALREADY
	%s %s kanalinda zaten koruma modunda degil.


# UNBAN responses
CHAN_UNBAN_SYNTAX
	UNBAN kanal
CHAN_UNBANNED
	%s kanalindaki ban'iniz kaldirildi.

# KICK responses
CHAN_KICK_SYNTAX
	KICK kanal nick [sebep]
CHAN_KICK_PROTECTED
	%s %s kanalindan atilamaz.
CHAN_KICKED
	%s %s kanalindan atildi. (kicked)

# TOPIC responses
CHAN_TOPIC_SYNTAX
	TOPIC kanal topic

# CLEAR responses
CHAN_CLEAR_SYNTAX
	CLEAR kanal parametre
CHAN_CLEARED_BANS
	%s kanalindaki butun ban'lar (+b) kaldirildi.
CHAN_CLEARED_EXCEPTIONS
	%s kanalindaki tüm ban asimlari (+e) kaldirildi.
CHAN_CLEARED_INVITES
	%s kanalindaki tüm davet (invite) maskeleri silindi.
CHAN_CLEARED_MODES
	%s kanalindaki butun mode'lar (+ovbntispklm) kaldirildi.
CHAN_CLEARED_OPS
	%s kanalindaki +o mode'u kaldirildi.(mass deop)
CHAN_CLEARED_HALFOPS
	%s kanalindaki tüm halfoplar (+h) kaldirildi.
CHAN_CLEARED_VOICES
	%s kanalindaki +v mode'u kaldirildi.(mass devoice)
CHAN_CLEARED_USERS
	%s kanalindaki tum kullanicilar atildi.

# GETPASS responses
CHAN_GETPASS_SYNTAX
	GETPASS kanal
CHAN_GETPASS_UNAVAILABLE
	%s kanali icin GETPASS komutu sifreleme (encryption) devrede oldugundan kullanilanamaz.
CHAN_GETPASS_PASSWORD_IS
	%s kanalinin sifresi : %s.

# FORBID responses
CHAN_FORBID_SYNTAX
	FORBID kanal
CHAN_FORBID_SUCCEEDED
	%s kanalinin kullanimi yasaklandi.(forbidden)
CHAN_FORBID_FAILED
	%s kanalinin kullanimi yasaklanamadi!
CHAN_FORBID_SHORT_CHANNEL
	# kanalinin kullanimi yasaklanamaz.

# SUSPEND responses
CHAN_SUSPEND_SYNTAX
	SUSPEND [+süre] kanal gerekce
CHAN_SUSPEND_SUCCEEDED
	%s kanali donduruldu. (suspended)
CHAN_SUSPEND_ALREADY_SUSPENDED
	%s kanali zaten dondurulmus. (suspended)

# UNSUSPEND responses
CHAN_UNSUSPEND_SYNTAX
	UNSUSPEND kanal
CHAN_UNSUSPEND_SUCCEEDED
	%s kanalindaki suspend kaldirildi.
CHAN_UNSUSPEND_NOT_SUSPENDED
	%s kanali suspend altinda degil.

###########################################################################
#
# MemoServ messages
#
###########################################################################

# Automatic messages
MEMO_HAVE_NEW_MEMO
	1 yeni notunuz var.
MEMO_HAVE_NEW_MEMOS
	%d yeni notunuz var.
MEMO_TYPE_READ_LAST
	/msg %s READ LAST yazarak okuyabilirsiniz.
MEMO_TYPE_READ_NUM
	/msg %s READ %d yazarak okuyabilirsiniz.
MEMO_TYPE_LIST_NEW
	/msg %s LIST NEW yazarak bir liste alabilirsiniz.
MEMO_AT_LIMIT
	UYARI: Maximum not sayiniza ulastiniz (%d).  Mevcutlardan bir kismini silmediginiz takdirde yeni notlar alamayacaksiniz.
MEMO_OVER_LIMIT
	UYARI: Maximum not sayinizi astiniz (%d). Mevcutlardan bir kismini silmediginiz takdirde yeni notlar alamayacaksiniz.
MEMO_NEW_MEMO_ARRIVED
	%s nick'inden yeni bir notunuz var.
	/msg %s READ %d yazarak okuyabilirsiniz.
MEMO_NEW_CHAN_MEMO_ARRIVED
	%s nickinden %s kanali icin yeni bir notunuz var.
	/msg %s READ %d yazarak okuyabilirsiniz.

# Multi-use responses
MEMO_HAVE_NO_MEMOS
	Size herhangi bir not yok.
MEMO_HAVE_NO_NEW_MEMOS
	Herhangi bir notunuz yok.
MEMO_DOES_NOT_EXIST
	%d numarali not bulunamadi!
MEMO_LIST_NOT_FOUND
	Uyan herhangi bir not bulunamadi.

# SEND responses
MEMO_SEND_SYNTAX
	SEND {nick | kanal} not-metni
MEMO_SEND_DISABLED
	Not birakma (SEND) gecici olarak devre disidir.
MEMO_SEND_PLEASE_WAIT
	Lutfen SEND komutunu yeniden kullanmadan once %s bekleyiniz.
MEMO_SEND_CHAN_NOT_AVAIL
	Kanal notlari kullanim disidir.
MEMO_X_GETS_NO_MEMOS
	%s nick'ine not birakilamaz.
MEMO_X_HAS_TOO_MANY_MEMOS
	%s nick'i su anda limitini doldurdugundan daha fazla not alamaz.
MEMO_SENT
	Notunuz  %s nick'ine gonderilmistir..
MEMO_SEND_FAILED
	%s icin not gonderilemedi.

# LIST responses
MEMO_LIST_SYNTAX
	LIST [num | liste | NEW]
MEMO_LIST_MEMOS
	%s nick'ine ait notlar.  Okumak icin, /msg %s READ num yaziniz.
MEMO_LIST_NEW_MEMOS
	%s nick'i icin yeni notlar var. Okumak icin, /msg %s READ num yaziniz.
MEMO_LIST_HEADER
	  Sira  Gonderen          Tarih/Saat
MEMO_LIST_FORMAT
	%c%c%c%4d  %-16s  %s

# READ responses
MEMO_READ_SYNTAX
	READ {num | liste | LAST | NEW}
MEMO_HEADER
	%d numarali not %s nick'inden (%s) .  Silmek icin, /msg %s DEL %d yaziniz.
MEMO_CHAN_HEADER
	%d numarali not %s nick'inden (%s).  Silmek icin, /msg %s DEL %s %d yaziniz.

# SAVE responses
MEMO_SAVE_SYNTAX
	SAVE {num | liste}
MEMO_SAVED_ONE
	%d numarali notunuz silinmeyecek.
MEMO_SAVED_SEVERAL
	%d adet not zamanasimindan cikarildi.

# DEL responses
MEMO_DEL_SYNTAX
	DEL {num | liste | ALL}
MEMO_DELETED_NONE
	Herhangi bir not silinmedi.
MEMO_DELETED_ONE
	%d numarali not silindi.
MEMO_DELETED_SEVERAL
	%d adet not silindi.
MEMO_DELETED_ALL
	Butun notlariniz silindi.

# RENUMBER responses
MEMO_RENUMBER_ONLY_YOU
	RENUMBER komutunu sadece kendiniz icin kullanabilirsiniz.
MEMO_RENUMBER_DONE
	Notlariniz yeniden numaralandi.

# SET responses
MEMO_SET_SYNTAX
	SET secenek parametre
MEMO_SET_DISABLED
	MemoServ'un SET komutu gecici olarak devre disidir.
MEMO_SET_UNKNOWN_OPTION
	Varolmayan secenek: %s.

# SET NOTIFY responses
MEMO_SET_NOTIFY_SYNTAX
	SET NOTIFY {ON | LOGON | NEW | OFF}
MEMO_SET_NOTIFY_ON
	%s artik size gonderilen notlarda online oldugunuzda ve/veya aninda uyari verecek.
MEMO_SET_NOTIFY_LOGON
	%s artik size gonderilen notlarda online oldugunuzda ve/veya /AWAY'den dondugunuzde uyari verecek.
MEMO_SET_NOTIFY_NEW
	%s artik size gonderilen notlarda aninda uyari verecek.
MEMO_SET_NOTIFY_OFF
	%s artik size herhangi bir uyari vermeyecek.

# SET LIMIT responses
MEMO_SET_LIMIT_SYNTAX
	SET LIMIT limit
MEMO_SET_LIMIT_OPER_SYNTAX
	SET LIMIT [kullanici] {limit | NONE | DEFAULT} [HARD]
MEMO_SET_YOUR_LIMIT_DEFAULT
	Not limitiniz standard ayar ile degistirildi (%d).
MEMO_SET_LIMIT_DEFAULT
	%s icin not limiti standard ayar ile degistirildi (%d).
MEMO_SET_YOUR_LIMIT_FORBIDDEN
	Not limitinizi belirleme yetkiniz yok.
MEMO_SET_LIMIT_FORBIDDEN
	%s nick'inin not limiti degistirilemez.
MEMO_SET_YOUR_LIMIT_TOO_HIGH
	Not limitiniz en fazla %d olabilir.
MEMO_SET_LIMIT_TOO_HIGH
	%s nick'inin limiti en fazla %d olabilir.
MEMO_SET_LIMIT_OVERFLOW
	Not limitiniz fazla.Limit %d olarak dusuruldu.
MEMO_SET_YOUR_LIMIT
	Not limitiniz %d olarak ayarlandi.
MEMO_SET_YOUR_LIMIT_ZERO
	Limitiniz sifir oldugundan bir daha not alamayacaksiniz.
MEMO_UNSET_YOUR_LIMIT
	Not limitiniz iptal edildi.
MEMO_SET_LIMIT
	%s nick'inin not limiti: %d.
MEMO_SET_LIMIT_ZERO
	%s nick'inin not limiti: 0.
MEMO_UNSET_LIMIT
	%s nick'inin not limiti: devre disi .

# INFO responses
MEMO_INFO_NO_MEMOS
	Su anda notunuz yok.
MEMO_INFO_MEMO
	Su anda 1 yeni notunuz var.
MEMO_INFO_MEMO_UNREAD
	Su anda 1 notunuz var, ve henuz okumadiniz.
MEMO_INFO_MEMOS
	Su anda %d notunuz var.
MEMO_INFO_MEMOS_ONE_UNREAD
	Su anda 1 'i okunmamis olmak uzere %d notunuz var.
MEMO_INFO_MEMOS_SOME_UNREAD
	Su anda %d adedi okunmamis olmak uzere %d notunuz var.
MEMO_INFO_MEMOS_ALL_UNREAD
	Su anda hicbirini okumadiginiz %d notunuz var.
MEMO_INFO_LIMIT
	Not limiti: %d.
MEMO_INFO_HARD_LIMIT
	Not limiti: %d, ve degistirme yetkiniz yok.
MEMO_INFO_LIMIT_ZERO
	Not limiti: 0; herhangi bir not alamayacaksiniz.
MEMO_INFO_HARD_LIMIT_ZERO
	Not limitiniz: 0; ve degistirme yetkiniz yok. Herhangi bir not alamayacaksiniz!
MEMO_INFO_NO_LIMIT
	Not limiti yok.
MEMO_INFO_NOTIFY_OFF
	Yeni not geldigi bildirilmeyecek.
MEMO_INFO_NOTIFY_ON
	Yeni not geldigi, yeni baglandiginizda ve ya yeni not gelir gelmez bildirilecek.
MEMO_INFO_NOTIFY_RECEIVE
	Yeni not geldigi an bildirilecek.
MEMO_INFO_NOTIFY_SIGNON
	Irc'ye baglandiginizda yeni memolar size bildirilecek.
MEMO_INFO_X_NO_MEMOS
	%s nick'inin su anda notu yok.
MEMO_INFO_X_MEMO
	%s nick'inin su anda 1 notu var.
MEMO_INFO_X_MEMO_UNREAD
	%s nick'inin su anda 1 notu var, ve henuz okunmamis.
MEMO_INFO_X_MEMOS
	%s nick'inin %d notu var.
MEMO_INFO_X_MEMOS_ONE_UNREAD
	%s nick'inin 1 okunmamis olmak uzere %d notu var.
MEMO_INFO_X_MEMOS_SOME_UNREAD
	%s nick'inin %d notu var, %d tanesi okunmamis.
MEMO_INFO_X_MEMOS_ALL_UNREAD
	%s nick'inin su anda hepsi okunmus %d notu var.
MEMO_INFO_X_LIMIT
	%s nick'inin memo limiti: %d.
MEMO_INFO_X_HARD_LIMIT
	%s nick'inin memo limiti: %d.
MEMO_INFO_X_NO_LIMIT
	%s nick'inin memo limiti yok.
MEMO_INFO_X_NOTIFY_OFF
	%s yeni not geldigini ögrenmiyor.
MEMO_INFO_X_NOTIFY_ON
	%s yeni notlari baglandigi vakit ve de yeni not geldiginde ögreniyor.
MEMO_INFO_X_NOTIFY_RECEIVE
	%s yeni notlari geldikleri vakit ögreniyor.
MEMO_INFO_X_NOTIFY_SIGNON
	%s yeni notlari baglandigi vakit ögreniyor.

################ forward module messages/responses

# Mail format
MEMO_FORWARD_MAIL_SUBJECT
	%s icin not
MEMO_FORWARD_MULTIPLE_MAIL_SUBJECT
	%s icin notlar
MEMO_FORWARD_MAIL_BODY
	%s (%s) size bir not gönderdi.
	%s
MEMO_FORWARD_CHANMEMO_MAIL_BODY
	%s size %s (%s) hakkinda bir not gonderdi.
	%s
	
# FORWARD responses
MEMO_FORWARD_SYNTAX
	FORWARD {num | liste | ALL}
MEMO_FORWARD_NEED_EMAIL
	Notlarinizi email ile almadan önce bir email adresi belirtmelisiniz.
MEMO_FORWARD_PLEASE_WAIT
	Lütfen FORWARD komutunu kullanmadan önce %s bekleyiniz.
MEMO_FORWARD_FAILED
	Not iletilemedi.
MEMO_FORWARDED_NONE
	Hicbir not iletilmedi.
MEMO_FORWARDED_ONE
	%d numarali not iletildi.
MEMO_FORWARDED_SEVERAL
	%d adet not iletildi.
MEMO_FORWARDED_ALL
	Tüm notlariniz iletildi.

# SET FORWARD responses
MEMO_SET_FORWARD_SYNTAX
	SET FORWARD {ON | COPY | OFF}
MEMO_SET_FORWARD_ON
	Notlariniz %s email adresinize gönderilecek.
MEMO_SET_FORWARD_OFF
	Notlariniz email adresinize gönderilmeyecek.
MEMO_SET_FORWARD_COPY
	Notlariniz %s email adresinize gönderilecek ve de
	Services tarafindan ayrica saklanacak.

################ ignore module messages/responses

# IGNORE responses
MEMO_IGNORE_SYNTAX
	IGNORE {ADD | DEL | LIST} [nick | mask]
MEMO_IGNORE_ADD_SYNTAX
	IGNORE ADD nick | mask
MEMO_IGNORE_DEL_SYNTAX
	IGNORE DEL nick | mask
MEMO_IGNORE_LIST_FULL
	Ignore listeniz dolu.
MEMO_IGNORE_LIST_X
	%s nicki ignore listesi:
MEMO_IGNORE_LIST
	Ignore listesi:
MEMO_IGNORE_ALREADY_PRESENT
	%s zaten ignore listesinde var.
MEMO_IGNORE_ADDED
	%s ignore listenize eklendi.
MEMO_IGNORE_DELETED
	%s ignore listenizden silindi.
MEMO_IGNORE_NOT_FOUND
	%s ignore listenizde bulunamadi.
MEMO_IGNORE_LIST_EMPTY
	Ignore listeniz bos.
MEMO_IGNORE_LIST_X_EMPTY
	%s nicki ignore listesi bos.

###########################################################################
#
# OperServ messages
#
###########################################################################

# General messages
OPER_BOUNCY_MODES
	Services mode degistiremiyor. Server'larinizin dogru ayarlandigina emin misiniz?
OPER_BOUNCY_MODES_U_LINE
	Services mode degistiremiyor. Server'larinizin U:line 'larini kontrol edin.

# GLOBAL responses
OPER_GLOBAL_SYNTAX
	GLOBAL metin

# STATS responses
OPER_STATS_UNKNOWN_OPTION
	Varolmayan STATS secenegi: %s.
OPER_STATS_RESET_USER_COUNT
	Maximum kullanici sayisi resetlendi.

OPER_STATS_CURRENT_USERS
	Mevcut Kullanici : %d (%d ops)
OPER_STATS_MAX_USERS
	Maximum Kullanici: %d (%s)
OPER_STATS_UPTIME_DHM
	Services %d%sdur calisiyor.
OPER_STATS_UPTIME_HM_MS
	Services %sdir calisiyor.

OPER_STATS_KBYTES_READ
	Okunan veri:      %10u kB
OPER_STATS_KBYTES_WRITTEN
	Yazilan veri:     %10u kB
OPER_STATS_NETBUF_SOCK
	Sunucu soket buffer:   %5u kB
OPER_STATS_NETBUF_SOCK_PERCENT
	Sunucu soket buffer:   %5u kb (%%%d)
OPER_STATS_NETBUF_TOTAL
	Toplam soket buffer:   %5u kB
OPER_STATS_NETBUF_TOTAL_PERCENT
	Toplam soket buffer:   %5u kB (%%%d)

OPER_STATS_ALL_USER_MEM
	Kullanici : %6d adet kayit, %5d kB
OPER_STATS_ALL_CHANNEL_MEM
	Kanallar  : %6d adet kayit, %5d kB
OPER_STATS_ALL_SERVER_MEM
	Sunucular : %6d adet kayit, %5d kB
OPER_STATS_ALL_NICKGROUPINFO_MEM
	Nickgroups: %6d adet kayit, %5d kB
OPER_STATS_ALL_NICKINFO_MEM
	Nickler   : %6d adet kayit, %5d kB
OPER_STATS_ALL_CHANSERV_MEM
	ChanServ  : %6d adet kayit, %5d kB
OPER_STATS_ALL_STATSERV_MEM
	StatServ  : %6d adet kayit, %5d kB
OPER_STATS_ALL_NEWS_MEM
	News      : %6d adet kayit, %5d kB
OPER_STATS_ALL_AKILL_MEM
	Autokills : %6d adet kayit, %5d kB
OPER_STATS_ALL_EXCEPTION_MEM
	Exceptions: %6d adet kayit, %5d kB
OPER_STATS_ALL_SGLINE_MEM
	SGlines   : %6d adet kayit, %5d kB
OPER_STATS_ALL_SQLINE_MEM
	SQlines   : %6d adet kayit, %5d kB
OPER_STATS_ALL_SZLINE_MEM
	SZlines   : %6d adet kayit, %5d kB
OPER_STATS_ALL_SESSION_MEM
	Oturumlar : %6d adet kayit, %5d kB

# GETKEY responses
OPER_GETKEY_SYNTAX
	GETKEY kanal
OPER_GETKEY_KEY_IS
	%s kanali icin anahtar: %s
OPER_GETKEY_NO_KEY
	%s kanali icin belirlenmis bir anahtar yok.

# MODE responses
OPER_MODE_SYNTAX
	MODE kanal mode(lar)

# CLEARMODES responses
OPER_CLEARMODES_SYNTAX
	CLEARMODES kanal [ALL]
OPER_CLEARMODES_DONE
	Kanal mode'lari ve banlar %s kanalinda temizlendi.
OPER_CLEARMODES_ALL_DONE
	%s kanalindaki butun mode'lar temizlendi.

# CLEARCHAN responses
OPER_CLEARCHAN_SYNTAX
	CLEARCHAN kanal
OPER_CLEARCHAN_DONE
	%s kanali tamamen temizlendi.

# KICK responses
OPER_KICK_SYNTAX
	KICK kanal kullanici sebep

# ADMIN responses
OPER_ADMIN_SYNTAX
	ADMIN {ADD | DEL | LIST} [nick]
OPER_ADMIN_ADD_SYNTAX
	ADMIN ADD nick
OPER_ADMIN_DEL_SYNTAX
	ADMIN DEL nick
OPER_ADMIN_EXISTS
	%s zaten Services admin listesinde.
OPER_ADMIN_ADDED
	%s Services admin listesine eklendi.
OPER_ADMIN_TOO_MANY
	Services admin listesi dolu (%d); ekleme iptal edildi.
OPER_ADMIN_REMOVED
	%s Services admin listesinden silindi.
OPER_ADMIN_NOT_FOUND
	%s Services admin listesinde bulunamadi.
OPER_ADMIN_LIST_HEADER
	Services admin listesi:
OPER_ADMIN_NO_NICKSERV
	NickServ su an kapali, ADMIN komutu kullanilamaz.

# OPER responses
OPER_OPER_SYNTAX
	OPER {ADD | DEL | LIST} [nick]
OPER_OPER_ADD_SYNTAX
	OPER ADD nick
OPER_OPER_DEL_SYNTAX
	OPER DEL nick
OPER_OPER_EXISTS
	%s zaten Services operator listesinde.
OPER_OPER_ADDED
	%s Services operator listesine eklendi.
OPER_OPER_TOO_MANY
	Services operator listesi dolu (%d); ekleme iptal edildi.
OPER_OPER_REMOVED
	%s Services operator listesinden silindi.
OPER_OPER_NOT_FOUND
	%s Services operator listesinde bulunamadi.
OPER_OPER_LIST_HEADER
	Services operator listesi:
OPER_OPER_NO_NICKSERV
	NickServ su an kapali, ADMIN komutu kullanilamaz.

# AKILL/EXCLUDE/SxLINE common responses
OPER_MASKDATA_SYNTAX
	%s {ADD | DEL | CLEAR | LIST | VIEW | CHECK | COUNT} [parametre]
OPER_MASKDATA_ADD_SYNTAX
	%s ADD [+süre] maske sebep
OPER_MASKDATA_DEL_SYNTAX
	%s DEL maske
OPER_MASKDATA_CLEAR_SYNTAX
	%s CLEAR ALL
OPER_MASKDATA_LIST_SYNTAX
	%s {LIST | VIEW} [maske] [NOEXPIRE]
OPER_MASKDATA_CHECK_SYNTAX
	%s CHECK maske
# mask, reason
OPER_MASKDATA_LIST_FORMAT
	%-32s  %s
# mask, set-by, set-time, last-used, expires, reason
OPER_MASKDATA_VIEW_FORMAT
	%s (%s tarafindan %s tarihinde, son kullanim %s; %s)
	    %s
# mask, set-by, set-time, expires, reason
OPER_MASKDATA_VIEW_UNUSED_FORMAT
	%s (%s tarafindan %s tarihinde, hic kullanilmamis; %s)
	    %s

# AKILL responses
OPER_TOO_MANY_AKILLS
	AKILL listesi dolu!
OPER_AKILL_NO_NICK
	Hatirlatma: AKILL'lerde nick bulunamaz.AKILL'inize nick yazmadiginizdan emin olun.
OPER_AKILL_MASK_TOO_GENERAL
	AKILL tanimi cok genis kapsamli.
OPER_AKILL_ADDED
	%s AKILL listesine eklendi.
OPER_AKILL_REMOVED
	%s AKILL listesinden silindi.
OPER_AKILL_CLEARED
	AKILL listesi temizlendi.
OPER_AKILL_NOT_FOUND
	%s AKILL listesinde bulunamadi.
OPER_AKILL_EXISTS
	%s AKILL listesinde zaten var.
OPER_AKILL_EXPIRY_LIMITED
	En fazla %s kadar bir süre belirtebilirsiniz.
OPER_AKILL_LIST_HEADER
	Su Anki AKILL Listesi:
OPER_AKILL_LIST_EMPTY
	AKILL listesi bos.
OPER_AKILL_LIST_NO_MATCH
	AKILL listesinde uyan kayit bulunamadi.
OPER_AKILL_CHECK_NO_MATCH
	%s AKILL listesinde bulunamadi.
OPER_AKILL_CHECK_HEADER
	AKILL listesinde %s icin uyan kayitlar:
OPER_AKILL_CHECK_TRAILER
	%d adet kayit bulundu.
OPER_AKILL_COUNT
	AKILL listesinde su an %d aded mask kaydi bulunuyor.

# AKILLCHAN responses
OPER_AKILLCHAN_SYNTAX
	AKILLCHAN [KILL] [+süre] kanal aciklama
OPER_AKILLCHAN_AKILLED
	%d adet kullaniciya autokill atildi.
OPER_AKILLCHAN_KILLED
	%d adet kullanicinin baglantisi kesildi.
OPER_AKILLCHAN_AKILLED_ONE
	1 kullaniciya autokill atildi.
OPER_AKILLCHAN_KILLED_ONE
	1 kullanicinin baglantisi kesildi.

# EXCLUDE responses
OPER_TOO_MANY_EXCLUDES
	EXCLUDE listesi dolu!
OPER_EXCLUDE_ADDED
	%s EXCLUDE listesine eklendi.
OPER_EXCLUDE_EXISTS
	%s EXCLUDE listesinde zaten var.
OPER_EXCLUDE_REMOVED
	%s EXCLUDE listesinden silindi.
OPER_EXCLUDE_CLEARED
	EXCLUDE listesi temizlendi.
OPER_EXCLUDE_NOT_FOUND
	%s EXCLUDE listesinde bulunamadi.
OPER_EXCLUDE_LIST_HEADER
	Su Anki EXCLUDE Listesi:
OPER_EXCLUDE_LIST_EMPTY
	EXCLUDE listesi bos.
OPER_EXCLUDE_LIST_NO_MATCH
	EXCLUDE listesinde uyan kayit bulunamadi.
OPER_EXCLUDE_CHECK_NO_MATCH
	EXCLUDE listesinde %s icin uyan kayit bulunamadi.
OPER_EXCLUDE_CHECK_HEADER
	EXCLUDE listesinde %s icin bulunan kayitlar:
OPER_EXCLUDE_CHECK_TRAILER
	%d adet kayit bulundu.
OPER_EXCLUDE_COUNT
	EXCLUDE listesinde su an %d adet mask kaydi bulunuyor.

# SxLINE responses
OPER_SLINE_ADDED
	%s %s listesine eklendi.
OPER_SLINE_MASK_TOO_GENERAL
	%s maskesi cok genis kapsamli.
OPER_SLINE_EXISTS
	%s %s listesinde zaten var.
OPER_SLINE_REMOVED
	%s %s listesinden silindi.
OPER_SLINE_CLEARED
	%s listesi temizlendi.
OPER_SLINE_NOT_FOUND
	%s %s listesinde bulunamadi.
OPER_TOO_MANY_SLINES
	%s listesi dolu.
OPER_SLINE_LIST_HEADER
	Aktüel liste:
OPER_SLINE_LIST_EMPTY
	%s listesi bos.
OPER_SLINE_LIST_NO_MATCH
	%s listesinde uyan kayit bulumamadi.
OPER_SLINE_CHECK_NO_MATCH
	%s %s listesinde bulunamadi.
OPER_SLINE_CHECK_HEADER
	%s icin %s listesinde uyan kayitlar:
OPER_SLINE_CHECK_TRAILER
	%d adet kayit bulundu.
OPER_SLINE_COUNT
	%d adet nesne %s listesinde bulunuyor.
OPER_SZLINE_NOT_AVAIL
	SZLINE bu server ile kullanilamaz.

# SU responses
OPER_SU_SYNTAX
	SU password
OPER_SU_NO_PASSWORD
	Services root sifresi girilmedigi icin SU komutu kullanim disi.
OPER_SU_SUCCEEDED
	Services root yetkilerine sahipsiniz.
OPER_SU_FAILED
	Sifre hatali.

# SET responses
OPER_SET_SYNTAX
	SET secenek ayar
OPER_SET_IGNORE_ON
	Ignore kodu AKTIF .
OPER_SET_IGNORE_OFF
	Ignore kodu KAPALI .
OPER_SET_IGNORE_ERROR
	IGNORE ayari ON veya OFF seklinde olmali.
OPER_SET_READONLY_ON
	Services su anda read-only calisiyor.
OPER_SET_READONLY_OFF
	Services su anda read-write calisiyor.
OPER_SET_READONLY_ERROR
	READONLY ayari ON veya OFF seklinde olmali.
OPER_SET_DEBUG_ON
	Services su anda debug mode'unda calisiyor.
OPER_SET_DEBUG_OFF
	Services su anda non-debug mode'unda calisiyor.
OPER_SET_DEBUG_LEVEL
	Services su anda debug mode'unda (level %d).
OPER_SET_DEBUG_ERROR
	DEBUG ayari ON, OFF formatinda veya pozitif bir sayi olmali.
OPER_SET_UNKNOWN_OPTION
	Varolmayan secenek: %s.
OPER_SET_SUPASS_FAILED
	Services root sifresi belirlenemedi.
OPER_SET_SUPASS_OK
	Services root sifresi degistirildi.
OPER_SET_SUPASS_NONE
	Services root sifresi kaldirildi.

# JUPE responses
OPER_JUPE_SYNTAX
	JUPE sunucu (server) [aciklama]
OPER_JUPE_INVALID_NAME
	Gecersiz server ismi. Server isimlerinde nokta karakteri bulunmalidir.
OPER_JUPE_ALREADY_JUPED
	%s serveri zaten jupe edilmis.

# RAW responses
OPER_RAW_SYNTAX
	RAW metin

# REHASH responses
OPER_REHASHING
	Ayarlar yeniden okunuyor.
OPER_REHASHED
	Tüm ayarlar basariyla yeniden okundu.
OPER_REHASH_ERROR
	Ayarlari okurken bir hata olustu, lütfen services log dosyasini inceleyiniz.

# UPDATE responses
OPER_UPDATE_SYNTAX
	UPDATE [FORCE]
OPER_UPDATE_FORCE_FAILED
	Vertabanlarinin kilidi kaldirilamadi.
OPER_UPDATING
	Veritabani güncelleniyor...
OPER_UPDATE_COMPLETE
	Güncelleme tamamlandi.
OPER_UPDATE_FAILED
	Güncelleme basarisiz oldu.

# KILLCLONES responses
OPER_KILLCLONES_SYNTAX
	KILLCLONES nick
OPER_KILLCLONES_UNKNOWN_NICK
	%s nickli kullanici bulunamadi.
OPER_KILLCLONES_KILLED
	%s KILLCLONES komutunu %s nicki icin kullandi, %d adet baglanti kesildi.
OPER_KILLCLONES_KILLED_AKILL
	%s KILLCLONES komutunu %s nicki icin kullandi, %d adet baglanti kesildi.
	%s icin gecici bir autokill eklendi.

###########################################################################
#
# Session limiting and exception messages
#
###########################################################################

# EXCEPTION responses

OPER_EXCEPTION_SYNTAX
	EXCEPTION {ADD | DEL | CLEAR | MOVE | LIST | VIEW | CHECK | COUNT} [parametre]
OPER_EXCEPTION_ADD_SYNTAX
	EXCEPTION ADD [+süre] mask limit gerekce
OPER_EXCEPTION_DEL_SYNTAX
	EXCEPTION DEL {mask | liste}
OPER_EXCEPTION_CLEAR_SYNTAX
	EXCEPTION CLEAR ALL
OPER_EXCEPTION_MOVE_SYNTAX
	EXCEPTION MOVE num posizyon
OPER_EXCEPTION_LIST_SYNTAX
	EXCEPTION {LIST | VIEW} maske | list]
OPER_EXCEPTION_ALREADY_PRESENT
	%s maski zaten listede mevcut.
OPER_EXCEPTION_TOO_MANY
	Oturum limiti ayricalik listesi (exception) dolu!
OPER_EXCEPTION_ADDED
	%s icin oturum limiti %d'e cevrildi.
OPER_EXCEPTION_MOVED
	%s (#%d) icin ayricalik %d pozisyonuna ilerletildi.
OPER_EXCEPTION_NO_SUCH_ENTRY
	(#%d) numarali exception listede bulunamadi.
OPER_EXCEPTION_EMPTY
	Ayricalik listesi bos.
OPER_EXCEPTION_NOT_FOUND
	%s ayricalik listesinde bulunamadi.
OPER_EXCEPTION_NO_MATCH
	Ayricalik listesinde uyan kayit bulunamadi.
OPER_EXCEPTION_DELETED
	%s kaydi ayricalik listesinden silindi.
OPER_EXCEPTION_DELETED_ONE
	Ayricalik listesinden 1 kayit silindi.
OPER_EXCEPTION_DELETED_SEVERAL
	Ayricalik listesinden %d kayit silindi.
OPER_EXCEPTION_CLEARED
	Ayricalik listesi temizlendi.
OPER_EXCEPTION_LIST_HEADER
	Su anki oturum limiti ayricalik listesi:
# mask, set-by, set-time, expires, limit, reason
OPER_EXCEPTION_LIST_COLHEAD
	Sira  Limit  Host
OPER_EXCEPTION_LIST_FORMAT
	%4d  %4d   %s
OPER_EXCEPTION_VIEW_FORMAT
	%3d.  %s  (%s tarafindan, %s tarihinde; %s)
	    Limit: %-4d  - %s
OPER_EXCEPTION_CHECK_SYNTAX
	EXCEPTION CHECK maske
OPER_EXCEPTION_CHECK_NO_MATCH
	Ayricalik listesinde %s icin uyan kayit bulunamadi.
OPER_EXCEPTION_CHECK_HEADER
	Ayricalik listesinde %s icin uyan kayitlar:
OPER_EXCEPTION_CHECK_TRAILER
	%d adet kayit bulundu.
OPER_EXCEPTION_COUNT
	Ayricalik listesinde %d adet kayit bulunmakta.
OPER_EXCEPTION_INVALID_LIMIT
	Gecersiz oturum limiti. Bu rakam sifirdan büyük ve ya sifira esit bir tamsayi
	ve de %d'den kücük olmalidir.
OPER_EXCEPTION_INVALID_HOSTMASK
	Gecersiz host. Sadece gercek hostmaskeleri ayricalik olarak kaydedilebilir.
	Nickler ve ya userid ler hostmask icinde kullanilamazlar.

# SESSION responses

OPER_SESSION_SYNTAX
	SESSION {LIST threshold | VIEW host}
OPER_SESSION_LIST_SYNTAX
	SESSION LIST threshold
OPER_SESSION_VIEW_SYNTAX
	SESSION VIEW host
OPER_SESSION_INVALID_THRESHOLD
	Gecersiz sinir tanimi. Bu sayi 1 den büyük bir tamsayi olmalidir.
OPER_SESSION_NOT_FOUND
	%s oturum listesinde bulunamadi.
OPER_SESSION_LIST_HEADER
	En az %d baglantili oturumlar:
OPER_SESSION_LIST_COLHEAD
	Oturum  Host
OPER_SESSION_LIST_FORMAT
	%5d   %s
OPER_SESSION_VIEW_FORMAT
	%s hostu su an %d baglanti yapmakta ve de limiti %d.


###########################################################################
#
# News system messages
#
###########################################################################

NEWS_LOGON_TEXT
	[Logon Haberler - %s] %s
NEWS_OPER_TEXT
	[Oper Haberler - %s] %s

NEWS_LOGON_SYNTAX
	LOGONNEWS {ADD | DEL | LIST} [text | num]
NEWS_LOGON_LIST_HEADER
	Logon news metinleri:
NEWS_LOGON_LIST_NONE
	Hicbir LOGONNEWS metni bulunamadi.
NEWS_LOGON_ADD_SYNTAX
	LOGONNEWS ADD text
NEWS_LOGON_ADD_FULL
	News listesi tamamen dolu!
NEWS_LOGON_ADDED
	Yeni LOGONNEWS metni eklendi (#%d).
NEWS_LOGON_DEL_SYNTAX
	LOGONNEWS DEL {num | ALL}
NEWS_LOGON_DEL_NOT_FOUND
	#%d numarali LOGONNEWS metni bulunamadi!
NEWS_LOGON_DELETED
	#%d numarali LOGONNEWS metni silindi.
NEWS_LOGON_DEL_NONE
	Silinecek LOGONNEWS bulunamadi!
NEWS_LOGON_DELETED_ALL
	Tüm LOGONNEWS metinleri silindi.
NEWS_LOGON_LIST_ENTRY
	%5d (%s %s tarafindan)
	    %s

NEWS_OPER_SYNTAX
	OPERNEWS {ADD | DEL | LIST} [text | num]
NEWS_OPER_LIST_HEADER
	Oper news metinleri:
NEWS_OPER_LIST_NONE
	Hicbir OPERNEWS metni bulunamadi.
NEWS_OPER_ADD_SYNTAX
	OPERNEWS ADD text
NEWS_OPER_ADD_FULL
	News listesi tamamen dolu!
NEWS_OPER_ADDED
	Yeni OPERNEWS eklendi (#%d).
NEWS_OPER_DEL_SYNTAX
	OPERNEWS DEL {num | ALL}
NEWS_OPER_DEL_NOT_FOUND
	#%d numarali OPERNEWS metni bulunamadi!
NEWS_OPER_DELETED
	#%d numarali OPERNEWS silindi.
NEWS_OPER_DEL_NONE
	Silecek OPERNEWS metni bulunamadi!
NEWS_OPER_DELETED_ALL
	Tüm OPERNEWS metinleri silindi.
NEWS_OPER_LIST_ENTRY
	%5d (%s %s tarafindan)
	    %s

NEWS_HELP_LOGON
	Kullanimi: LOGONNEWS ADD text
	           LOGONNEWS DEL {num | ALL}
	           LOGONNEWS LIST
	
	Logon news metinlerini gösterir ve ya düzenler. Logonnews
	bir kullanici networke baglandigi vakit ona gönderilen metindir.
	Flood etkisi yaratmamak icin ücten daha fazla mesaj
	gönderilmeyecektir. Dolayisiyla sadece en yeni üc mesaj gönderilir.
	
	LOGONNEWS LIST ile aktüel mesajlarin listesini görmek mümkündür.
	ADD ve de DEL komutlarini sadece Services operatorler kullanabilir.
	
NEWS_HELP_OPER
	Kullanimi: OPERNEWS ADD text
	           OPERNEWS DEL {num | ALL}
	           OPERNEWS LIST
	
	Oper news metinlerini gösterir ve ya düzenler. Bir /oper
	komutu kullanildiginda bu listedeki en yeni 3 metin o kullaniciya
	gönderilir. Flood olmamasi icin ücten daha fazla metin gönderilmeyecektir.
	
	OPERNEWS LIST ile aktüel metinleri görmek mümkündür.
	ADD ve ya DEL komutlari ise Services operatorlere aittir.


###########################################################################
#
# StatServ messages
#
############################################################################

# General SERVERS responses
STAT_SERVERS_REMOVE_SERV_FIRST
	Server %s online. Bu serveri networkden cikartip yeniden deneyin.
STAT_SERVERS_SERVER_EXISTS
	Server %s zaten mevcut.

# SERVERS command syntax
STAT_SERVERS_SYNTAX
	SERVERS secenek parametre

# STATS responses
STAT_SERVERS_STATS_TOTAL
	Toplam server: %2d
STAT_SERVERS_STATS_ON_OFFLINE
	       Online: %2d (%d%%)  Offline: %2d (%d%%)
STAT_SERVERS_LASTQUIT_WAS
	    Son quit: %-30s  %s

# LIST responses
STAT_SERVERS_LIST_HEADER
	Server Ismi                    User       Oper
STAT_SERVERS_LIST_FORMAT
	%-30s %3d (%2d%%)  %3d (%2d%%)
STAT_SERVERS_LIST_RESULTS
	Liste sonu -- %d/%d online server gösteriliyor.

# VIEW responses
STAT_SERVERS_VIEW_HEADER_ONLINE
	%-30s  (online)
STAT_SERVERS_VIEW_HEADER_OFFLINE
	%-30s  (offline)
STAT_SERVERS_VIEW_LASTJOIN
	    Son baglanti: %s
STAT_SERVERS_VIEW_LASTQUIT
	    Son cikis: %s
STAT_SERVERS_VIEW_QUITMSG
	 Quit mesaji: %s
STAT_SERVERS_VIEW_USERS_OPERS
	        Kullanici: %3d (%2d%%)  Oper: %3d (%2d%%)
STAT_SERVERS_VIEW_RESULTS
	Liste sonu -- %d/%d server gösteriliyor.

# DELETE responses
STAT_SERVERS_DELETE_SYNTAX
	SERVERS DELETE server
STAT_SERVERS_DELETE_DONE
	%s server istatistikleri silindi.

# COPY responses
STAT_SERVERS_COPY_SYNTAX
	SERVERS COPY server yeniisim
STAT_SERVERS_COPY_DONE
	%s server istatistikleri %s'ne kopyalandi.

# RENAME responses
STAT_SERVERS_RENAME_SYNTAX
	SERVERS RENAME server yeniisim
STAT_SERVERS_RENAME_DONE
	%s server istatistikleri ismi %s ismine cevrildi.

# USERS responses
STAT_USERS_SYNTAX
	USERS STATS
STAT_USERS_TOTUSERS
	     Toplam kullanici: %d
STAT_USERS_TOTOPERS
	      Toplam operator: %d
STAT_USERS_SERVUSERS
	Server basina ortalama kullanici: %d
STAT_USERS_SERVOPERS
	 Server basina ortalama operator: %d.%d


###########################################################################
#
# NickServ help messages
#
###########################################################################

NICK_HELP
	%S size bir nick'i kaydetme ve baskalarinin
	kullanmasini engelleme imkani saglar. %S kullanmak icin bircok
	komut vardir. Komutlarin listesini görebilmek icin,
	/msg %S HELP COMMANDS komutunu kullanabilirsiniz.
	/msg %S komut ile bir komut calistirilir. Bilgi almak icin
	/msg %S HELP komut deneyebilirsiniz.
NICK_HELP_EXPIRES
	
	Eger nick %s kullanilmassa otomatik olarak drop edilecektir.

NICK_HELP_WARNING
	
	ONEMLI: %S kullanicilarin IRC'deki kimliklerini
	guvence altina almak amaci ile duzenlenmistir.Kesinlikle bir
	baskasinin nick'ini calmayi kolaylastirma amacli degildir.
	%S'un amac disi kullanimi kesinlikle yasaktir ve
	tesbit edilmesi durumunda adiniza kayitli nick ve kanallarin
	yasaklanmasi ile sonuclanacaktir.

NICK_HELP_COMMANDS
	Asagidaki komutlar kullanilabilmektedir.
	
	    REGISTER     O anki nick'i sizin adiniza kaydeder.
	    IDENTIFY     Sizi nick'in sahibi olarak tanitir.
	    DROP         Nick'inizin kaydini siler.
NICK_HELP_COMMANDS_AUTH
	    AUTH         Bir nickin kaydini tamamlamak icin kullanilir.
	    SENDAUTH     Kayip tanitma kodunu email ile gönderir.
NICK_HELP_COMMANDS_LINK
	    LINK         Nick'inizi baska bir nick ile birlestirir.
	    UNLINK       Baska bir nickle olan bagi kaldirir.
	    LISTLINKS    Nick'inize linkli (bagli) tüm diger nickleri
	                   gösterir.
NICK_HELP_COMMANDS_ACCESS
	    ACCESS       Nick'inizin erisim listesini duzenler.
NICK_HELP_COMMANDS_AJOIN
	    AJOIN        Otojoin listenizi ayarlar.
NICK_HELP_COMMANDS_SET
	    SET          Cesitli %S ozelliklerini duzenler.
	    UNSET        Cesitli %S özelliklerini kaldirir.
	    RECOVER      Nickinizi kullanan bir baskasini KILL'ler.
	    RELEASE      RECOVER komutundan sonra tutulan nick'i birakir.
	    GHOST        Hayalet bir baglantiyi keser.
	    INFO         Bir nick hakkinda bilgi gösterir.
NICK_HELP_COMMANDS_LIST
	    LIST         Bir maskeye uyan tüm nickleri listeler.
	    LISTEMAIL    Bir emaile sahip tüm nickleri listeler.
NICK_HELP_COMMANDS_LISTCHANS
	    LISTCHANS    Kayitli tüm kanallarinizi listeler.
	    STATUS       Bir nickin identify seviyesini gösterir.

NICK_HELP_REGISTER
	Kullanimi: %s
	
	Nick'inizi %S veritabanina kaydeder.Nick'iniz kaydedildikten
	sonra (register) SET ve ACCESS komutlarini kullanarak
	ayarlarinizi istediginiz sekilde degistirebilirsiniz. Kayit
	sirasinda kullandiginiz sifreyi unutmamaniz onemlidir.
	Sifrenizde kucuk ve buyuk harfler farkli karakterlerdir!
NICK_HELP_REGISTER_EMAIL
	
	Ayrica istege bagli olarak, nick kaydederken bir email adresi
	de belirtebilirsiniz.
NICK_HELP_REGISTER_EMAIL_REQ
	
	Sifrenizi kaydederken, gecerli bir email adresi de belirtmek
	durumundasiniz. Bu adres size sifreniz kayboldugunda sorulacaktir.
	Adresin görünmesini engellemek icin /msg %S SET HIDE EMAIL ON
	komutunu kullanabilirsiniz.
NICK_HELP_REGISTER_EMAIL_AUTH
	
	Nickinizi kaydederken gecerli bir email adresi belirtmeniz gerekmektedir,
	zira %S bu adrese bir email gönderecek, bir tanitim (AUTH) kodu
	bildirecektir. Bundan sonra AUTH komutunu kullanarak kayit
	isleminizi tamamlayabilirsiniz. Email adresinizin baska kullanicilar
	tarafindan görülmesini engelleyebilirsiniz. Bunun icin
	/msg %S HELP SET HIDE komutunu kullanarak bilgi alabilirsiniz.

NICK_HELP_REGISTER_END
	
	Sifreniz hakkinda oneriler:
	
	Sifrenizi secerken gercek adiniz gibi kolay tahmin
	edilebilecek seyler kullanmayin.Kisa sifreler de guvenilir
	degildir. Zaten %S de buna izin vermeyecektir. Bu nedenle
	en azindan 5 karakter uzunlugunda, kolay tahmin edilemeyen ve de
	icinde space karakteri bulunmayan,hatta icinde rakam olan
	karakter serileri secmeniz yarariniza olacaktir.

NICK_HELP_IDENTIFY
	Kullanimi: IDENTIFY sifre
	
	Kendinizi %S 'e nick'in sahibi olarak tanitmanizi saglar.
	Pekcok komutu kullanabilmeniz icin bu gereklidir.Sifreniz
	REGISTER komutu sirasinda kullandiginiz ile ayni olmalidir.

NICK_HELP_DROP
	Kullanimi: DROP sifre
	
	Nick'inizin %S veritabanindaki kaydini siler.Drop
	edilen bir nick'i bir baskasi kaydedebilir.Bu komutu
	kullanabilmeniz icin once sifrenizi kullanarak kendinizi
	tanitmalisiniz.(/msg %S HELP IDENTIFY yazarak daha
	genis bilgi alabilirsiniz).
NICK_HELP_DROP_LINK
	
	Dikkat edilmelidir ki, DROP komutunu kullandiginiz vakit,
	linkli tüm nickleri de silmis olursunuz. Bu vakit bu nickler
	tekrar register edilebilirler.
NICK_HELP_DROP_END
	
	Kazayla kullanimi engellemek icin nick sifrenizi girmeniz
	gerekmektedir.

NICK_HELP_AUTH
	Kullanimi: AUTH kod
	
	Nick'inizin size ait oldugunu ya da email degisiminizi kesinlestirir.
	
	Sözkonusu kod siz nickinizi kaydettiginiz vakit ya da SET EMAIL
	kullanarak email adresi degistirdiginiz vakit belirttiginiz
	email adresine gönderilecektir. Lütfen bu kodu güvenli bir yerde
	saklayiniz. Nickinizin calindigi iddiasi gibi bir durumda, nickin
	hakikaten size ait oldugunu kanitlamanin tek yolu budur.

NICK_HELP_SENDAUTH
	Kullanimi: SENDAUTH
	
	Nickinizin tanitma kodunu, REGISTER ya da SET EMAIl gibi islemlerden
	sonra kayitli email adresinize gönderir. Kazayla bu kodu
	kaybettiginizde ise yarayacak bir komuttur.
	
	Bu komut sadece 24 saatte bir kullanilabilir.

NICK_HELP_REAUTH
	Kullanimi: REAUTH
	
	Nickinizi identify etmeniz icin gereken bir auth (tanitim)
	kodu olusturur.Eger nick sifrenizi unuttuysaniz, bu komutu kullanarak
	yeni bir auth (tanitim) kodu olusturabilir; ardindan kodu (size mail
	olarak gonderilmis olan) AUTH komutu ile birlikte kullanarak
	nickinizi identify edip, SET PASSWORD komutu ile nick sifrenizi
	degistirebilirsiniz.

NICK_HELP_RESTOREMAIL
	Kullanimi: RESTOREMAIL sifre
	
	Nickiniz icin sisteme son tanitilmis Email adresinize geri dönülür.
	Eger yanlislikla SET EMAIL komutunda yanlis bir Email adresi
	belirttiyseniz, bu komutu kullanarak eski Email adresinize geri
	dönebilir ve auth (tanitim) kodunuzun sistemden silinmesini saglayarak
	nickinizi tekrardan identify edebilirsiniz.Bu komutu nick sifreniz ile
	birlikte kullanmalisiniz.
	
	RESTOREMAIL komutu sadece SET EMAIL'den sonra ve AUTH komutu ile
	yeni auth (tanitim) kodunuzu girmeden once kullanilabilir.
	
NICK_HELP_LINK
	Kullanimi: LINK nick
	
	Sizin nickinizle tamamen ayni özelliklere sahip
	bir alternatif nick yaratir. Kanal accessleri, memolar,
	her iki nick icin de erisilebilir durumda olurlar.
	Bir nickinize bir kanalda verilen autoop, linkli
	nickiniz icin de gecerli olur.
	
	Linki kaldirmak icin UNLINK kullanabilirsiniz.
	
	Bu komutu kullanabilmeniz icin once sifrenizi kullanarak
	kendinizi tanitmali (IDENTIFY kullanarak) ve linkleyeceginiz
	nickin kayitsiz ve kullanimda olmamasi gerekmektedir.

NICK_HELP_UNLINK
	Kullanimi: UNLINK nick
	
	Nickinizin alternatiflerinden birini siler. Aktüel nickinizin
	linkini kaldiramazsiniz, bunun icin baska bir linkli nicke
	gecmeniz gerekmektedir.
	
	Temel nickinizi unlink ederseniz, aktüel nickiniz temel
	nick olarak belirlenir. Degistirmek icin SET MAINNICK
	komutunu kullanabilirsiniz.

NICK_HELP_LISTLINKS
	Kullanimi: LISTLINKS
	
	Sizin nickinize linkli tüm nickler gösterir. Temel nick
	bir "*" ile belirtilecektir.

NICK_HELP_ACCESS
	Kullanimi: ACCESS ADD maske
	           ACCESS DEL maske
	           ACCESS LIST
	
	Nick'inizin erisim listesini duzenler ve/veya goruntuler.
	Bu liste %S tarafindan otomatik olarak taninacak user@host
	seklinde tanimlanmis adresleri belirler. Bu listedeki bir
	adresi kullanarak baglandiginiz takdirde, SET KILL ayarindan
	etkilenmeyecek, ve de SET SECURE opsiyonu belirtilmemis ise,
	otomatik op da dahil diger bütün yetkilerinize IDENTIFY
	komutunu kullanmadan sahip olursunuz.
	
	Belirtilen maske ADD ve DEL komutlarinda asterisk (*)
	ve de soru isareti (?) sembollerini kullanmaniza olanak
	saglar.
	
	Örnekler:
	
	ACCESS ADD mikado@*.holyfire.com
	      mikado isimli userin holyfire.com domain'indeki
	       herhangi bir makineden taninmasi.
	
	ACCESS DEL mikado@*.holyfire.com
	       Onceki komutu geri alir.
	
	ACCESS LIST
	       Aktüel access list'i goruntuler.

NICK_HELP_SET
	Kullanimi: SET secenek parametreler
	
	Cesitli %S seceneklerini ayarlar.
	Gecerli secenek'ler:
	
	    PASSWORD   Nick'inizin sifresini degistirir.
	    LANGUAGE   Services'in size hitap edecegi dili belirler.
	    URL        Belirtilen URL'yi nick'inizle iliskilendirir.
	    EMAIL      Belirtilen E-Mail'i nick'inizle iliskilendirir.
	    KILL       Kill secenegini acar/kapar.
	    INFO       INFO komutunda bir bilgi satiri gösterir.
	    SECURE     Security secenegini acar/kapar.
	    PRIVATE    /msg %S LIST komutu ile cikacak listede
	                  nick'inizin gorunmesini engeller.
	    NOOP       Nickinizin kanal erisim listelerine eklenmesini
	                  engeller.
	    HIDE       Nick bilgilerinizin bir kismini "INFO"
	                  kismindan saklar.
	    TIMEZONE   %S bilgi metinlerinde gösterilen zaman dilimini
	                  degistirmeye yarar.
NICK_HELP_SET_OPTION_MAINNICK
	    MAINNICK   Linkli nickler arasindaki temel nicki gösterir.
NICK_HELP_SET_END
	Bu komutu kullanabilmeniz icin once sifrenizi kullanarak
	kendinizi tanitmalisiniz.(/msg %S HELP IDENTIFY
	yazarak daha genis bilgi alabilirsiniz).
	
	Belirli bir komut hakkinda daha fazla bilgi almak icin
	/msg %S HELP SET secenek yazabilirsiniz.

NICK_HELP_SET_PASSWORD
	Kullanimi: SET PASSWORD yeni-sifre
	
	Sizi %S'e nick'in sahibi olarak tanitacak olan sifreyi
	degistirir.

NICK_HELP_SET_LANGUAGE
	Kullanimi: SET LANGUAGE numara
	
	Services'in size hitap edecegi dili degistirir.numara
	desteklenen dillerden birine ait bir numara olmalidir:

NICK_HELP_SET_URL
	Kullanimi: SET URL url
	
	Belirtilen URL'yi nick'inizle iliskilendirir.Bu URL INFO
	komutu ile nick'iniz hakkinda alinabilecek bilgiye
	eklenecektir.

NICK_HELP_SET_EMAIL
	Kullanimi: SET EMAIL adres
	
	Belirtilen E-Mail adresini nick'inizle iliskilendirir.Bu
	E-Mail INFO komutu ile nick'iniz hakkinda alinabilecek
	bilgiye eklenecektir.

NICK_HELP_SET_KILL
	Kullanimi: SET KILL {ON | QUICK | IMMED | OFF}
	
	Otomatik Kill secenegini acar/kapar.Kill acik oldugu zaman
	nick'inizi almaya calisan baska bir kullaniciya degistirmesi
	icin bir dakika taninacak ve eger degistirmezse %S
	tarafindan IRC baglantisi kesilerek "KILL" edilecektir.
	
	Eger QUICK secenegini secerseniz,kullaniciya taninan sure
	1 dakika yerine 20 saniye olur.Eger IMMED secenegi aktifse
	kullanici uyarilmadan "KILL" edilir.Lutfen bu ozelligi
	gereksiz yere kullanmayin.Ayrica network yoneticiniz bu
	secenegi devreden cikarmis olabilir.

NICK_HELP_SET_SECURE
	Kullanimi: SET SECURE {ON | OFF}
	
	%S's guvenlik fonksiyonlarini acar/kapar. SECURE
	secenegi aktif iken nick'inizin sahibi olarak taninmaniz
	icin erisim listenize bakilmaksizin sifrenizi kullanarak
	kendinizi %S'e tanitmaniz gerekir.Ancak %S 'un
	erisim listesinde iseniz eger,sizi tanimamasina ragmen
	"KILL" 'lemeyecektir.

NICK_HELP_SET_PRIVATE
	Kullanimi: SET PRIVATE {ON | OFF}
	
	/msg %S LIST komutu ile cikacak listede nick'inizin
	gorunmesini engeller.Ancak yine de INFO komutu ile
	nick'iniz hakkinda bilgi alabilirsiniz.

NICK_HELP_SET_NOOP
	Kullanimi: SET NOOP {ON | OFF}
	
	%S'ün no-op secenegini acar/kapar.  NOOP secenegini
	aktif etmek nickinizin kanal erisim listelerine
	eklenmesini engeller.  Ancak, kullanici zaten kanalin
	erisim listesindeyse, bu secenegi aktif etseniz bile
	kullanici erisim listesinden silinmeyecektir.

NICK_HELP_SET_HIDE
	Kullanimi: SET HIDE {EMAIL | USERMASK | QUIT} {ON | OFF}
	
	Nick bilgilerinizdeki bazi kisimlari saklamanizi saglar.Her
	kullanicinin "/msg %S INFO" komutu ile gorebilecegi
	bilgilerden E-mail adresinizi (EMAIL), user@host'u (USERMASK) ve
	son quit mesajinizdan (QUIT) istediklerinizi
	saklayabilirsiniz.Hangi kismin saklanacagindan hemen sonraki
	parametre (OFF) veya (ON) seklinde olmalidir.

NICK_HELP_SET_TIMEZONE
	Kullanimi: SET TIMEZONE {UTC-offset | zaman-dilimi | DEFAULT}
	
	Süreyle ilgili bilgileri gösterirken, (mesela bir kanal ve ya
	nickin en son kullanildigi tarih), standarttan farkli bir
	zaman dilimini temel almaya yarar. DEFAULT ayari ile,
	services'in standard ayarina geri dönülür. Zaman dilimi iki
	farkli sekilde belirtilebilir.
	
	   - UTC-offset olarak belirtmek istediginizde, kendi
	     yerel saat ile UTC (Universal Coordinated Time, ya da
	     GMT - Greenwich Mean Time -) arasindaki farki "+" ya da "-"
	     isaretleri kullanarak verebilirsiniz. Bu isaretlerden
	     sonraki rakamlar hh:mm seklinde olup, saat ve de
	     dakikayi belirtmektedirler.
	
	   - zaman-dilimi belirtirken, "GMT" ya da "EEST" gibi
	     zaman dilimi isimleri kullanmaniz gerekmektedir. Ancak
	     bazi zaman dilimi isimleri services e yabanci olabilir.
	     Böyle bir durumda, UTC offset yöntemini kullanmaniz
	     tavsiye edilir.
	
	Services yaz saati uygulamasina kendi kendine gecemez, bu
	ayari kendiniz yapmak durumundasiniz.
	
	Örnekler:
	
	    SET TIMEZONE -5
	        Zaman dilimini UTC den 5 saat daha geriye, yani
	        5 saat daha doguya dogru ayarlar.
	
	    SET TIMEZONE +3:30
	        Zaman dlimini UTC den 3 saat 30 dakika daha ileriye,
	        yani 3 saat 30 dakika daha batiya dogru ayarlar.
	
	    SET TIMEZONE EEST
	        Zaman dilimini Dogu Avrupa standard zaman dilimi ile
	        degistirir.

NICK_HELP_SET_INFO
	Kullanimi: SET INFO metin
	
	Nickiniz icin bir bilgi metni belirler, bu metin INFO
	komutu kullanildiginda gösterilir.

NICK_HELP_SET_MAINNICK
	Kullanimi: SET MAINNICK nick
	
	Birden fazla linkli nickiniz varsa, kanal access listesi
	gibi listelerde, hangi nickin gösterilecegini belirlemenize
	yarar. Bu nicki daha sonra unlink ederseniz, otomatik olarak
	baska bir nick secilecektir.

NICK_HELP_UNSET
	Kullanimi: UNSET {URL | EMAIL | INFO}
	
	Nickinize bagli URL, INFO ve ya EMAIL ayarlarini silmeye yarar.
	Ayni anda sadece bir ayar silebilirsiniz.

NICK_HELP_UNSET_REQ_EMAIL
	Kullanimi: UNSET {URL | INFO}
	
	Nickinize bagli URL ve ya INFO ayarini silmeye yarar.

NICK_HELP_RECOVER
	Kullanimi: RECOVER nickiniz [sifreniz]
	
	Nick'inizi kullanan bir kullaniciyi server'dan atmanizi
	saglar. Bu , "KILL" seceneginin aktif olmasi halinde
	%S 'un otomatik olarak yapacagi islemdir.
	
	Bu komutu verdiginizde , %S server'a belirtilen nick'i
	tasiyan bir clone sokar.Bu durum da IRC server'larinin oteki
	kullaniciyi hattan atmalarina yol acar.Bu "clone" karsi
	tarafin hemen yeniden baglanip nick'i yeniden almasina imkan
	vermemek icin %s sureyle hatta kalir.Nick'inizi bu
	surenin sonunda geri alabilir veya RELEASE
	komutunu kullanarak daha erken serbest birakilmasini
	saglayabilirsiniz.(/msg %S HELP RELEASE)
	
	Bu komutu kullanabilmeniz icin o anki hostunuz erisim
	listenizde olmali veya sifrenizi kullanarak kendinizi
	%S'e tanitmalisiniz.

NICK_HELP_RELEASE
	Kullanimi: RELEASE nickiniz [sifreniz]
	
	%S tarafindan tutulan nick'inizin %s kadar sure
	dolmadan serbest birakilmasini saglar.
	
	Bu komutu kullanabilmeniz icin o anki hostunuz erisim
	listenizde olmali veya sifrenizi kullanarak kendinizi
	%S'e tanitmalisiniz.

NICK_HELP_GHOST
	Kullanimi: GHOST nickiniz [sifreniz]
	
	IRC'de asili kalmis olan nick'inizi temizler."GHOST" genelde
	bilgisayarinizin takilmasi gibi bir sebeple siz IRC'den
	cikmadan internet baglantinizin kesilmesi sonucunda
	server'da asili kalan nick'inize verilen addir.
	
	Bu komutu kullanabilmeniz icin o anki /WHOIS 'iniz erisim
	listenizde olmali veya sifrenizi kullanarak kendinizi
	%S'e tanitmalisiniz.

NICK_HELP_INFO
	Kullanimi: INFO nick
	
	Belirtilen nick'in bilgilerini (nick sahibi,secenekler,URL
	vs.) listeler.
NICK_HELP_INFO_AUTH
	
	Eger nickin mail adresi henüz AUTH komutu ile
	onaylanmamissa SET HIDE EMAIL özelliginden bagimsiz
	olarak, nickin sahibi ve services adminleri disinda
	kimse tarafindan görüntülenemez.

NICK_HELP_LISTCHANS
	Kullanimi: LISTCHANS
	
	Kaydetmis oldugunuz bütün kanallarin isimlerini ve de
	toplam kanal adedini gösterir.

NICK_HELP_LIST
	Kullanimi: LIST [+baslangic degeri] parametre
	
	Belirtilen parametreye uyan butun kayitli nick'leri ya nick
	ya da user@host formatinda listeler. PRIVATE özelligi aktif
	olan nickler bu listede görünmezler.Eðer baslangic degeri
	kullanirsaniz geriye dönük degerler alirsiniz.Soyleki;
	Eger '+50' parametresini kullanirsaniz ilk 50 sonuctan sonrasini
	almis olursunuz.
	
	Ornekler:
	
	    LIST Joseph_14@rz.uni-karlsruhe.de
	         En son olarak Joseph_14@rz.uni-karlsruhe.de adresinden
	         kullanilmis olan nicklerin tümünü listeler.
	    LIST *Bot*
	         Icinde Bot sözcügü gecen tüm nickleri listeler,
	         bunun icin büyük kücük harf ayrimi yapmaz.
	    LIST *@*.dot.com
	         En son dot.com domaininden baglanmis kullanicilarin
	         tümünün listesini verir.
NICK_HELP_LIST_OPERSONLY
	
	Sadece Irc Operatörleri bu komutu kullanabilirler.

NICK_HELP_LISTEMAIL
	Kullanimi: LISTEMAIL pattern
	
	Tipki LIST komutunda oldugu gibi nickleri listeler,
	ancak, bu sefer user@host yerine email adreslerini gösterir.
	Daha fazla bilgiicin, LIST komutu hakkinda bilgi alabilirsiniz.
	(/msg %S HELP LIST)

NICK_HELP_STATUS
	Kullanimi: STATUS [nick]
	
	Belirtilen kullanicinin nick'in sahibi olrak taninip
	taninmadigini goruntuler.Cevap su sekildedir:
	
	    nick statu-kodu
	
	Burada nick sizin komutta belirttiginiz nick iken statu-kodu
	asagidakilerden biri olabilir:
	
	    0 - Bu kullanici hatta deil (offline) veya bu kayitli
	            bir nick degil.
	    1 - Bu kullanici %S tarafindan nick'in sahibi
	            olarak taninmamis.
	    2 - Bu kullanici erisim listesi ile taninmis.
	    3 - Bu kullanici nick sifresi ile  taninmis.
	
	Her komutta 16 nick'e kadar belirtilebilir.Kalanlar dikkate
	alinmayacaktir.Eger nick belirtilmez ise herhangi bir hata
	mesaji verilmez.

NICK_HELP_AJOIN
	Kullanimi: AJOIN ADD channel
	           AJOIN DEL channel
	           AJOIN LIST
	
	Nick'inizin otojoin kanal listesini düzenler ve/veya görüntüler.
	Bu listeye eklediginiz kanallara, /identify komutu kullandiktan
	hemen sonra %S tarafindan gönderilirsiniz. Dikkat
	etmelisiniz ki sadece "#" isareti ile baslayan kanal isimleri
	kabul edilebilir.
	
NICK_HELP_AJOIN_END
	Bazi IRC server, otojoin listenizdeki kanallara girmenizi
	engelleyebilir. Mesela anahtar (+k) ve ya invite-only (+i)
	modlarinin bulundugu kanallara girmek mümkün olmayabilir.
NICK_HELP_AJOIN_END_CHANSERV
	Bazi IRC server, otojoin listenizdeki kanallara girmenizi
	engelleyebilir. Mesela anahtar (+k) ve ya invite-only (+i)
	modlarinin bulundugu kanallara girmek mümkün olmayabilir.
	Ancak, %s INVITE komutuna erisiminiz varsa, %S sizi
	bu kanallara direk olarak davet edecektir.

NICK_OPER_HELP_COMMANDS
	
	Bu komutlar Services Adminler tarafindan kullanilabilir:
	
	    DROPNICK     Bir nickin kaydini siler.
NICK_OPER_HELP_COMMANDS_DROPEMAIL
	    DROPEMAIL    Bir email adresi maskesine uyan her nicki siler
NICK_OPER_HELP_COMMANDS_GETPASS
	    GETPASS      Sifreleme gecerli degilse, bir nickin
	                     sifresini gösterir.
NICK_OPER_HELP_COMMANDS_FORBID
	    FORBID       Bir nickin kaydini engeller.
	    SUSPEND      Bir nickin identify edilmesini engeller.
	    UNSUSPEND    Bir suspend olayini kaldirir.
NICK_OPER_HELP_COMMANDS_SETAUTH
	    SETAUTH      Bir nick icin yeni bir AUTH kodu belirler
	    GETAUTH      Bir nick icin gecerli AUTH kodunu gösterir
	    CLEARAUTH    Bir nickin AUTH kodunu siler.
NICK_OPER_HELP_COMMANDS_END
	
	Services Adminler ayrica asagidaki komutlari identify
	gerekmeden kullanabilirler.
	
	    SET, ACCESS LIST, INFO ALL
	
	Her birisi icin yardim almak mümkündür.

NICK_OPER_HELP_DROPNICK
	Kullanimi: DROPNICK nick
	
	Belirtilen nick ve de linklerinin kaydini siler.
	
	Bu komutu sadece services adminler kullanabilirler.

NICK_OPER_HELP_DROPEMAIL
	Kullanimi: DROPEMAIL maske
	           DROPEMAIL-CONFIRM maske
	
	Belirtilen maske ye uyan tüm emaillere ait olan nicklerin
	kaydini siler. Eger maske tek bir sadece "-" karakterinden
	olusuyorsa, email adresi olmayan nicklerin kaydi silinir.
	
	Nicklerin yanlislikla silinmesini engellemek icin, bu komut iki
	asamadan olusmaktadir. Öncelikle DROPEMAIL komutu ile bir
	maske belirlenir. Daha sonra DROPEMAIL-CONFIRM komutu ile
	ayni maske onaylanir. DROPEMAIL-CONFIRM komutu %s süre icinde
	DROPEMAIL komutunu kullanan kisi tarafindan verilmek zorundadir
	Eger ayni dan 4 DROPEMAIL komutundan daha fazlasi verilirse,
	sonuncusu gecersiz sayilacaktir.
	
	Bu komutu sadece Services Adminler kullanabilir.

NICK_OPER_HELP_UNLINK
	Kullanimi: UNLINK nick [FORCE]
	
	Link komutu ile kurulmus bir bagi kaldirir. Aktüel nickinizin
	linkini kaldirmak icin, baska bir linkli nicke gecmeniz
	gerekmektedir.
	
	Temel nickinizi unlink ederseniz, aktüel nickiniz temel
	nick olarak belirlenir. Degistirmek icin SET MAINNICK
	komutunu kullanabilirsiniz.
	
	Services adminler ayrica FORCE opsiyonu ile, baska bir nickin
	de linkini kaldirabilirler.

NICK_OPER_HELP_SET
	
	Services admin'ler ayrica NOEXPIRE komutunu da kullanmaya
	ve bu sekilde belirtilen nick'lerin zaman asimi yuzunden
	silinmelerini (EXPIRE) engellemeye de yetkilidirler.Services
	admin'ler sifreyi girmelerine gerek olmaksizin
	belirtecekleri her nick'in ayarlarini
	SET !nick secenek parametre komutu ile degistirme yetkisine
	sahiptirler.

NICK_OPER_HELP_SET_NOEXPIRE
	Kullanimi: SET [!nick] NOEXPIRE {ON | OFF}
	
	ON parametresinin kullanilmasi halinde,belirtilen nick zaman
	asimi nedeni ile %S veritabanindan silinmeyecektir.
	
	Bu komutu sadece Services admin'ler kullanabilir.


NICK_OPER_HELP_UNSET
	
	Services adminler UNSET komutunu herhangi bir nick üzerinde; SET
	komutunda oldugu oldugu gibi; komuttan once nick'i belirterek
	kullanabilirler.

NICK_OPER_HELP_OLD_UNLINK
	
	Services adminler sifre kullanmaksizin nick linklerini
	kesebilirler.

NICK_OPER_HELP_OLD_LISTLINKS
	Kullanimi: LISTLINKS nick
	
	Belirtilen nicke linkli nickleri gösterir.
	
	Bu komutu sadece Services admin'ler kullanabilir.

NICK_OPER_HELP_LISTLINKS
	Kullanimi: LISTLINKS [nick]
	
	Belirtilen nicke linkli tüm nickleri gösterir, eger nick
	belirtilmemis ise, kendi nickinize linkli nickler
	gösterilecektir. Temel nick bir "*" ile belirtilecektir.

NICK_OPER_HELP_ACCESS
	
	Services adminler herhangi bir nickin access listesini LIST
	komutuna parametre vererek görebilirler.

NICK_OPER_HELP_INFO
	
	Services admin'ler ALL parametresini her nickle
	kullanabilirler.

NICK_OPER_HELP_LISTCHANS
	
	Services adminler, bir nickin kaydettigi tüm kanallari,
	nick belirtmek suretiyle görebilirler. (Örnek:
	/msg %S LISTCHANS TimeMr14C).

NICK_OPER_HELP_LIST
	Kullanimi: LIST maske [FORBIDDEN] [NOEXPIRE] [SUSPENDED]
	
	Belirtilen parametreye uyan butun kayitli nick'leri ya nick
	ya da user@host formatinda listeler. PRIVATE özelligi aktif
	olan nickler bu listede sadece services adminler tarafindan
	görülebilirler.
	
	FORBIDDEN, yasaklanmis nick'leri, NOEXPIRE, düsmeyecek olan
	nick'leri, SUSPENDED, kullanimi durdurulmus nick'leri gösterir.
	NOEXPIRE nick'ler birer ! ile, SUSPENDED nick'ler birer * ile
	gösterilirler.
	
	Services admin'ler bu komutu her türlü kombinasyonla
	kullanabilirler.

NICK_OPER_HELP_LIST_AUTH
	Kullanimi: LIST maske [FORBIDDEN] [NOEXPIRE] [SUSPENDED] [NOAUTH]
	
	Belirtilen parametreye uyan butun kayitli nick'leri ya nick
	ya da user@host formatinda listeler. PRIVATE özelligi aktif
	olan nickler bu listede sadece services adminler tarafindan
	görülebilirler.
	
	FORBIDDEN, yasaklanmis nick'leri, NOEXPIRE, düsmeyecek olan nick'leri,
	SUSPENDED, kullanimi durdurulmus nick'leri gösterir. NOEXPIRE nick'ler
	birer ! ile, SUSPENDED nick'ler birer * ile, henüz sisteme tanitilmamis
	olan nick'ler birer ? ile gösterilirler.NOAUTH opsiyonu ile daha
	henüz AUTH komutu kullanilarak sisteme tanitilmamis nickleri görebilirsiniz.
	
	Services admin'ler bu komutu her türlü kombinasyonla kullanabilirler.

NICK_OPER_HELP_LIST_END
	
	Eðer baslangic degeri kullanirsaniz geriye dönük degerler alirsiniz.
	Soyleki; Eger '+50' parametresini kullanirsaniz ilk 50 sonuctan sonrasini
	almis olursunuz.
	
	Ornekler:
	
	    LIST Joseph_14@rz.uni-karlsruhe.de
	         En son olarak Joseph_14@rz.uni-karlsruhe.de adresinden
	         kullanilmis olan nicklerin tümünü listeler.
	
	    LIST *Bot*
	         Icinde Bot sözcügü gecen tüm nickleri listeler,
	         bunun icin büyük kücük harf ayrimi yapmaz.
	
	    LIST * NOEXPIRE
	         Tüm NOEXPIRE nick'leri gösterir.

NICK_OPER_HELP_AJOIN
	
	Services adminleri herhangi bir nickin otojoin listesini
	nick parametresini AJOIN LIST komutuna vererek gösterebilirler.

NICK_OPER_HELP_GETPASS
	Kullanimi: GETPASS nick
	
	Belirtilen nick'in sifresini verir.Bu komut kullanildiginda
	komutu kullanan Services admin'in nick'ini de iceren bir
	server mesaji (WALLOPS veya GLOBOPS seklinde) da ayni anda
	atilir.
	
	Bu komutu sadece Services admin'ler kullanabilir ve
	sifreleme (encryption) aktif iken devre disidir.

NICK_OPER_HELP_FORBID
	Kullanimi: FORBID nick
	
	Belirtilen nick'in kullanimini yasaklar.Yasagin kaldirilmasi icin
	sozkonusu nick'in %S veritabanindan silinmesi gerekir.(DROP)
	
	Bu komutu sadece Services admin'ler kullanabilir.

NICK_OPER_HELP_SUSPEND
	Kullanimi: SUSPEND [+expiry] nick gerekce
	
	Belirtilen nick'i ve de ona linkli tüm nickleri, kullanilabilir
	olmaktan cikartir, UNSUSPEND komutu ile de bu özellik eski
	haline geri getirilebilir.
	
	Suspendli nickler, forbidin aksine, düsünceye kadar bilgilerini
	kaybetmezler. expiry parametresi %s AKILL komutunda oldugu
	gibidir.
	
	Bu komutu sadece Services admin'ler kullanabilir.

NICK_OPER_HELP_UNSUSPEND
	Kullanimi: UNSUSPEND nick
	
	SUSPEND altindaki bir nicki eski haline geri getirir.
	
	Bu komutu sadece Services admin'ler kullanabilir.

NICK_OPER_HELP_SETAUTH
	Kullanimi: SETAUTH nick
	
	Belirtilen nick icin yeni bir AUTH kodu belirler. Bu komuttan
	sonra nickin sahibi tekrar AUTH komutunu kullanarak nickini
	sisteme tanitmak durumundadir. Eger nick kullanimda ise,
	nick sahibi bir NOTICE ile durumdan haberdar edilecek,
	ve de o andan itibaren identify edilmis olarak sayilmayacaktir.
	
	Bu komutu sadece Services admin'ler kullanabilir.

NICK_OPER_HELP_GETAUTH
	Kullanimi: GETAUTH nick
	
	Bir nickin AUTH (tanitim) kodunu gösterir.
	
	Bu komutu sadece Services admin'ler kullanabilir.

NICK_OPER_HELP_CLEARAUTH
	Kullanimi: CLEARAUTH nick
	
	Bir nick ile iliskilendirilmis bir AUTH (tanitim) kodunu siler.
	Nick normal bir sekilde kullanilabilir.
	
	Bu komutu sadece Services admin'ler kullanabilir.

###########################################################################
#
# ChanServ help messages
#
###########################################################################

# Used in OP/VOICE/etc for required access levels for commands
CHAN_HELP_REQSOP_LEVXOP
	seviye 100 (SOP)
CHAN_HELP_REQSOP_LEV
	seviye 100
CHAN_HELP_REQSOP_XOP
	SOP
CHAN_HELP_REQAOP_LEVXOP
	seviye 50 (AOP)
CHAN_HELP_REQAOP_LEV
	seviye 50
CHAN_HELP_REQAOP_XOP
	AOP
CHAN_HELP_REQHOP_LEVXOP
	seviye 40 (HOP)
CHAN_HELP_REQHOP_LEV
	seviye 40
CHAN_HELP_REQHOP_XOP
	HOP
CHAN_HELP_REQVOP_LEVXOP
	seviye 30 (VOP)
CHAN_HELP_REQVOP_LEV
	seviye 30
CHAN_HELP_REQVOP_XOP
	VOP

CHAN_HELP
	%S size bir kanali kendi adiniza kaydetme ve onu
	yonetme imkani saglar. %S cogu zaman kotu
	niyetli kullanicilarin kanalinizi elegecirmelerini
	(takeover) , ornegin kanalda sadece belirli bir listedeki
	kisilerin op olmalarina izin vererek engeller.Mevcut
	komutlari ögrenmek icin /msg %S HELP COMMANDS komutunu
	kullanabilirsiniz. Komutlar /msg %S komut yazarak calistirilirlar.
	Belirli bir komut hakkinda daha ayrintili bilgi almak icinse
	/msg %S HELP komut yazmaniz yeterlidir.
CHAN_HELP_EXPIRES
	
	Unutulmamalidir ki %s suresince kullanilmayan kanallar otomatik
	olarak silinirler.(ornegin access list'indeki kimsenin bu sure icerisinde
	kanala girmemesi durumunda)

CHAN_HELP_COMMANDS
	
	Asagidaki komutlar %S ile kullanilabilir.
	
	    REGISTER   Kanali sizin adiniza %S veritabanina kaydeder.
	    IDENTIFY   Sifreniz ile birlikte sizi kanal sahibi olarak tanitir.
	    DROP       Bir kanalin kaydini siler.
	    SET        Cesitli kanal ozelliklerini ve kanal bilgisinini ayarlar.
	    UNSET      Belirtilmis kanal özelliklerini siler.
	    INFO       Kanal hakkinda bilgi gösterir.
CHAN_HELP_COMMANDS_LIST
	    LIST       Kayitli kanallari listeler.
CHAN_HELP_COMMANDS_AKICK
	    AKICK      Otokick listesini düzenler.
CHAN_HELP_COMMANDS_LEVELS
	    ACCESS     Ayricalikli kullanicilarin listesini duzenler.
	    LEVELS     Kullanicilarin erisim seviyelerini yeniden duzenler.
CHAN_HELP_COMMANDS_XOP
	    AOP        Autoop listesini düzenler.
	    SOP        Süperop listesini düzenler.
CHAN_HELP_COMMANDS_HOP
	    HOP        Halfop listesini düzenler.
CHAN_HELP_COMMANDS_XOP_2
	    VOP        Autovoice listesini düzenler.
CHAN_HELP_COMMANDS_OPVOICE
	    OP         Bir kullaniciyi oplar. (+o)
	    DEOP       Bir kullaniciyi deoplar. (-o)
	    VOICE      Bir kullaniciya voice verir. (+v)
	    DEVOICE    Bir kullanicidan voice alir. (-v)
CHAN_HELP_COMMANDS_HALFOP
	    HALFOP     Bir kullaniciya halfop verir. (+h)
	    DEHALFOP   Bir kullanicidan halfop alir. (-h)
CHAN_HELP_COMMANDS_PROTECT
	    PROTECT    Bir kullaniciya koruma verir. (+a)
	    DEPROTECT  Bir kullanicidan koruma alir. (-a)
CHAN_HELP_COMMANDS_INVITE
	    INVITE     Bir kanala davet edilmenizi saglar.
	    UNBAN      Bir kanaldaki baninizi kaldirir.
	    KICK       Bir kanaldaki bir kullaniciyi atar.
	    TOPIC      Bir kanal topicini degistirir.
	    CLEAR      Bir kanalda toplu MODE/KICK komutu kullanir.
	    STATUS     Bir kullanicinin kanaldaki erisim seviyesini gösterir.

CHAN_HELP_REGISTER
	Kullanimi: REGISTER kanal sifre aciklama
	
	Bir kanali sizin adiniza kaydeder.Bu komutu kullanabilmek
	icin sozkonusu kanalda operator olmaniz gerekir.Sifreniz
	daha sonra IDENTIFY komutunu kullanarak kendinizi kanalin
	sahibi olarak %S'e tanitacaginiz sifre oldugundan
	kolay tahmin edilemeyecek ve sizin de unutmayacaginiz bir
	sifre secmeniz onemlidir.Aciklama kismi ise,kanal hakkinda
	genel bir aciklama niteligindedir.Bir kanali kaydettiginizde
	(register) o kanalin sahibi olarak "founder" taninirsiniz.
	Kanal founder'i kanalda bazi ayar ve degisiklikler yapma
	hakkina sahiptir.Ayrica %S founder nick'ine kanala her
	girisinde otomatik olarak op verecektir.ACCESS ve/veya
	LEVELS komutlarini kullanarak (/msg %S HELP ACCESS) founder
	yetkilerinden bazilarini diger kullanicilara da vermek
	mumkundur.
	
	ONEMLI NOT: Bir kanali adiniza register edebilmek icin
	oncelikle nick'inizi register etmeniz gerekir.Eger
	nick'inizi register etmediyseniz /msg %s HELP yazarak
	yardim alabilirsiniz.

CHAN_HELP_REGISTER_ADMINONLY
	
	Bu komutu sadece Services Adminler kullanabilir.

CHAN_HELP_IDENTIFY
	Kullanimi: IDENTIFY kanal sifre
	
	Sizi %S'e belirtilen kanalin founder'i olarak tanitir.
	Pek cok komutu kullanabilmek icin once kendinizi founder
	olarak tanitmaniz gerekir.Sifre kismi kanali register
	ederken kullanilan sifre ile ayni olmalidir.

CHAN_HELP_DROP
	Kullanimi: DROP kanal
	Belirtilen kanalin kaydini siler.Sadece founder tarafindan
	IDENTIFY komutundan sonra kullanilabilir.

CHAN_HELP_SET
	Kullanimi: SET kanal secenek parametre
	
	Kanal founder'inin cesitli kanal opsiyonlarini ve
	bilgilerini degistirmesi icindir.Founder once IDENTIFY
	komutunu kullanarak kendini %S'e tanitmalidir.
	
	Kanal Secenekleri:
	
	    FOUNDER     Kanal founder'ini belirtir.
	    PASSWORD    Founder sifresini degistirir.
	    DESC        Kanal hakkindaki aciklamayi degistirir.
	    URL         Bir internet adresini kanal ile iliskilendirir.
	    EMAIL       Bir E-posta adresini kanal ile iliskilendirir.
	    ENTRYMSG    Kanala girince gönderilecek bir mesaj belirler.
	    MLOCK       Kanal modlarini kilitler/acar.
	    KEEPTOPIC   Kanal kullanilmadigi zaman topici hafizada
	                   tutar.
	    TOPICLOCK   Kullanilmasi durumunda topic ancak TOPIC ile
	                   degisir.
	    PRIVATE     Kanali %S listesinden saklar.
	    SECUREOPS   Kanal access list'indeki oplar disinda kimse
	                   op alamaz.
	    RESTRICTED  Kanala access list'te olanlar disindakiler
	                   giremez.
	    SECURE      %S guvenlik fonksiyonlarini aktif hale
	                   getirir.
	    LEAVEOPS    Kanala ilk giren kisideki opu kaldirmaz.
	    OPNOTICE    OP/VOICE kullanildiginda bir notice gönderir.
	    ENFORCE     Autoop/Autovoice seviyelerini koruma (enforce)
	    MEMO-RESTRICTED
	                Kanala not gonderebilecek olan kullanicilari
	                   belirler.
	
	/msg %S HELP SET secenek yazarak belirli bir secenek
	hakkinda daha ayrintili bilgi alabilirsiniz.

CHAN_HELP_SET_FOUNDER
	Kullanimi: SET kanal FOUNDER nick
	
	Belirtilen kanalin founder'ini (sahibini) degistirir.
	Yeni nick de %S'e kaydedilmis olmalidir.

CHAN_HELP_SET_SUCCESSOR
	Kullanimi: SET kanal SUCCESSOR nick
	
	Kanalin SUCCESSOR unu belirler. SUCCESSOR, founder nicki düsünce
	founder a cevrilen nick'tir, böylece kanal düsmemis olur.
	SUCCESSOR un (%d) kayitli kanali varsa, yeni kanal düser.
	Successor, kayitli bir nick olmak zorundadir.
	
CHAN_HELP_SET_PASSWORD
	Kullanimi: SET kanal PASSWORD yenisifre
	
	Kullanicinin kanalin sahibi olarak taninacagi sifreyi
	belirler.

CHAN_HELP_SET_DESC
	Kullanimi: SET kanal DESC aciklama
	
	Belirtilen kanal hakkindaki,LIST ve INFO komutlari ile
	gorunecek olan aciklamayi belirler.Kullanabilmek icin
	founder level access gerekir.

CHAN_HELP_SET_URL
	Kullanimi: SET kanal URL webadresi
	
	Belirtilmis URL'yi kanal ile iliskilendirir.Bu URL "INFO"
	komutu ile kanal hakkinda bilgi istendiginde gorunecektir.

CHAN_HELP_SET_EMAIL
	Kullanimi: SET kanal EMAIL e-mail adresi
	
	Belirtilen e-mail adresini yine belirtilen kanal ile iliskilendirir.
	Bir kullanici kanal hakkinda INFO komutu ile bilgi istediginde bu e-mail
	adresi de gozukur.

CHAN_HELP_SET_ENTRYMSG
	Kullanimi: SET kanal ENTRYMSG mesaj
	
	Kullanicilarin kanala giriste /NOTICE seklinde alacaklari
	mesaji belirler.

CHAN_HELP_SET_KEEPTOPIC
	Kullanimi: SET kanal KEEPTOPIC {ON | OFF}
	
	Kanalin topic hatirlama (retention) opsiyonunu acar/kapar.
	topic retention aktifken, kanalin topici %S tarafindan
	son kullanici da kanaldan ayrildiktan sonra dahi
	hatirlanacak ve kanala bir kullanici geldiginde yeniden
	yerine yazilacaktir.

CHAN_HELP_SET_TOPICLOCK
	Kullanimi: SET kanal TOPICLOCK {ON | OFF}
	
	Kanalin topic kilidi (topiclock) opsiyonunu acar/kapar.
	topic lock aktifken, %S kanal topicinin TOPIC
	disindaki herhangi bir komutla degistirilmesine izin
	vermeyecektir.

CHAN_HELP_SET_MLOCK
	Kullanimi: SET kanal MLOCK modlar
	
	Kanalin mod-kilidi (mode-lock) parametresini acar/kapar.
	%S size bazi kanal modlarinin daimi olarak aktif veya
	kapali tutma imkani saglar.Sozkonusu modlar i, k, l, m, n,
	p, s ve t arasindan bir veya birkaci olabilir.modlar
	parametresinin sekli /MODE komutundaki kullanimi ile
	aynidir.(Ornegin +nt , -isp gibi.)
	MLOCK ile kilitlenen modlar, ayni MODE komutu kullanildigindaki
	gibi, "+" ile baslayanlar setlenen, "-" ile baslayanlar
	kaldirilan modlari belirtecek sekildedirler.
	
	DIKKAT: Eger kanal icin bir anahtar modu kilitlenirse,
	bu kanali ayrica RESTRICTED opsiyonu ile güvenli bir hale
	getirmeniz gerekir. (Daha fazla bilgi icin HELP SET RESTRICTED)
	Aksi takdirde, bos kanala giren herkes kanal anahtarini
	ögrenebilir, ve de +i modunun kilitlenmis olmasi bunu
	degistirmez.
	
	Ornekler:
	
	    SET #kanal MLOCK +nt-iklps
	        Kanalda +nt modlari surekli aktif (+) iken, i, k, l,
	        p ve s surekli devre disi (-) kalacaktir.Kanal
	        kodu m ise serbest birakilmistir.
	
	    SET #kanal MLOCK +knst-ilmp anahtarim
	        k, n, s ve t modlari surekli aktifken i, l, m, ve
	        p modlari surekli devre disidir.Ayrica kanalin
	        anahtari da (KEY) "anahtarim" seklinde sabit
	        tutulur.
	
	    SET #kanal MLOCK +
	        Mode-lock opsiyonunu kaldirir.Boylece butun kanal
	        modlari serbest kalir.

CHAN_HELP_SET_PRIVATE
	Kullanimi: SET kanal PRIVATE {ON | OFF}
	
	Kanalin private ozelligini acar/kapar.private ozelligi aktif
	olan kanallar /msg %S LIST komutu ile alinabilecek listede
	gorunmezler.
	
CHAN_HELP_SET_RESTRICTED
	Kullanimi: SET kanal RESTRICTED {ON | OFF}
	
	Kanalin RESTRICTED ozelligini acar/kapar.Bu ozelligin aktif
	oldugu bir kanalda, normalde kanalda op alamayan
	kullanicilar ya da kanalin erisim listesinde bulunmayan kullanicilar
	kanala giremeyecekler, kicklenip banlanacaklardir.

CHAN_HELP_SET_SECURE
	Kullanimi: SET kanal SECURE {ON | OFF}
	
	%S'un guvenlik ozelliklerini acar/kapar.SECURE
	aktifken, kanala sadece kayitli nick'li kullanicilar
	kendilerini "IDENTIFY" komutu ile %s'e tanittiktan
	sonra girebilirler.

CHAN_HELP_SET_SECUREOPS
	Kullanimi: SET kanal SECUREOPS {ON | OFF}
	
	Kanalin secure ops ozelligini acar/kapar.Bu ozellik aktifken
	kanalin erisim listesinde olmayan kullancilar op alamazlar.
	
CHAN_HELP_SET_LEAVEOPS
	Kullanimi: SET kanal LEAVEOPS {ON | OFF}
	
	Kanalin leave ops özelligini acar/kapar. Acik iken, %S
	kanala ilk girmek sureti ile op olmus kisileri deoplamayacaktir.
	Öte yandan, SECUREOPS normal calismaya devam edecek, erisimi
	olmadan op olan kisiler deoplanacaktir.
	DIKKAT: Bu özellik, netsplit esnasinda op olmayi kolaylastirabilir.

CHAN_HELP_SET_OPNOTICE
	Kullanimi: SET kanal OPNOTICE {ON | OFF}
	
	Kanal icin op-notice opsiyonunu acar ya da kapar. Bu opsiyon
	acikken, %S kanala bir notice gönderecektir. Bu metinde OP
	ya da VOICE ve benzeri komutlarin bir kullanici icin
	kullanildiklari belirtilir.

CHAN_HELP_SET_ENFORCE
	Kullanimi: SET channel ENFORCE {ON | OFF}
	
	Bir kanal icin enforce opsiyonunu acar ve ya kapar.
	Acikken, %S autoop ve ya autovoice modlarini koruyacaktir.
	Bu durumda, autoop haklarina sahip bir kisi deoplanirsa,
	%S o kisiyi tekrar oplayacaktir, ve de DEOP komutunun da
	bu kisi üstünde kullanilmasina izin vermeyecektir.

CHAN_HELP_SET_MEMO_RESTRICTED
	Syntax: SET kanal MEMO-RESTRICTED {ON | OFF}
	
	Kanalin kisitli not secenegini acar/kapar.  kisitli not
	secenegi aktifken, sadece kanal notlarina erisim yetkisi
	bulunan kullanicilar kanala not gonderebileceklerdir.

CHAN_HELP_SET_HIDE
	Kullanimi: SET kanal HIDE {EMAIL | TOPIC | MLOCK} {ON | OFF}
	
	%S kanal infosundan belirtilen bilgi türünü gizler
	ve ya gösterir. EMAIL email bilgisi icin, TOPIC
	topic bilgisi icin, MLOCK kanal mod kilidi
	icin kullanilmaktadir.
	
	Dikkat edilmelidir ki, kanal +p ya da +s modunda ise, TOPIC
	gösterilmeyecektir.

CHAN_HELP_UNSET
	Kullanimi: UNSET kanal {SUCCESSOR | URL | EMAIL | ENTRYMSG}
	
	Belirtilen ayari siler.

### CHANGED: new LIST syntax
CHAN_HELP_SOP
	Kullanimi: SOP kanal ADD nick
	           SOP kanal DEL nick
	           SOP kanal LIST [maske]
	           SOP kanal COUNT
	
CHAN_HELP_SOP_MID1
	Bir kanalin SOP (süperop) listesini düzenler. SOP seviyesindeki
	her bir kullanici kanala girdiklerinde otomatik olarak
	op (+o) alacaklardir.
CHAN_HELP_SOP_MID1_CHANPROT
	Bir kanalin SuperOp (SOP) listesini duzenleri. SOP
	seviyesindeki kullanicilar kanala girdiklerinde otomatik
	olarak op (+o) ve de protect (+a) alirlar.
CHAN_HELP_SOP_MID2
	
	Bir SOP iseniz, kanalin AOP, VOP, ve de NOP listelerini de
	düzenleyebilirsiniz.
CHAN_HELP_SOP_MID2_HALFOP
	
	Bir SOP iseniz, kanalin AOP, VOP, HOP, ve de NOP listelerini
	de düzenleyebilirsiniz.

CHAN_HELP_SOP_END
	
	ADD, DEL, LIST, ve COUNT alt komutlari, AOP komutunun alt
	komutlariyla ayni bicimdedir.


CHAN_HELP_AOP
	Kullanimi: AOP kanal ADD nick
	           AOP kanal DEL nick
	           AOP kanal LIST [maske]
	           AOP kanal COUNT
	
	Bir kanalin AOP listesini düzenler. AOP seviyesindeki
	her bir kullanici kanala girdiklerinde otomatik olarak
	op (+o) alacaklardir.

CHAN_HELP_AOP_MID
	
	Bir AOP iseniz, kanalin VOP ve NOP listelerini de
	düzenleyebilirsiniz. SOPlar AOP listesini düzenleyebilirler.
CHAN_HELP_AOP_MID_HALFOP
	
	Bir AOP iseniz, kanalin HOP, VOP ve de NOP listelerini de
	düzenleyebilirsiniz. SOPlar da bu listeleri düzenleyebilirler.
CHAN_HELP_AOP_END
	
	AOP ADD komutu ile bir kullaniciyi AOP listesine ekler,
	AOP DEL komutu ile bir kullaniciyi AOP listesinden silersiniz.
	Unutmayin ki bir kullanici bir kanalin erisim listesinde yanlizca
	bir defa bulunabilir. Eger kullaniciya AOP komutuyla erisim ekleniyorsa
	ve kullanici VOP listesi gibi zaten kanalin erisim listesindeyse;
	kullanici ilk olarak bu listeden silinir (Eger bu listeyi düzenlemeye
	erisiminiz yoksa, yani kullanici zaten SOP listesindeyse AOP ADD komutu
	ile listeyi yeniden duzenleyemezsiniz).
	
	AOP LIST komutu kanalin AOP listesini gösterir. Eger bir joker karakter
	ile maskeyi belirtirseniz, yanlizca maske ile uyusan kayitlari görüntülersiniz.
	Eger bir baslangic degeri belirttiyseniz geriye dönük degerler alirsiniz.
	Soyleki; Eger '+50' parametresini kullanirsaniz ilk 50 sonuctan sonrasini
	almis olursunuz.
	
	AOP COUNT komutu, kanalin autoop listesinde kac nick oldugunu
	gösterir.

CHAN_HELP_HOP
	Kullanimi: HOP channel ADD nick
	           HOP channel DEL nick
	           HOP channel LIST [maske]
	           HOP channel COUNT
	
	Bir kanalin HOP listesini (HalfOp) düzenler. HOP seviyesindeki
	her bir kullanici, kanala girdiklerinde otomatik olarak op (+o)
	alacaklardir.
	
	Bir HOP iseniz, kanalin VOP ve NOP listelerini de
	düzenleyebilirsiniz. AOP ve ya SOP'lar ise, HOP listesini
	düzenleyebilirler.
	
	ADD, DEL, LIST, ve COUNT alt komutlari, AOP komutunun alt
	komutlariyla ayni bicimdedir.

CHAN_HELP_VOP
	Kullanimi: VOP kanal ADD nick
	           VOP kanal DEL nick
	           VOP kanal LIST [+baslangic degeri | liste]
	           VOP kanal COUNT
	
	Bir kanalin VOP listesini (autovoice) düzenler. VOP seviyesindeki
	her bir kullanici kanala girdiklerinde otomatik olarak
	voice (+v) alacaklardir.
	
	ADD, DEL, LIST, ve COUNT alt komutlari, AOP komutunun alt
	komutlariyla ayni bicimdedir.

CHAN_HELP_NOP
	Syntax: NOP channel ADD nick
	        NOP channel DEL nick
	        NOP channel LIST [+baslangic degeri] [maske]
	        NOP channel COUNT
	
	Bir kanalin NOP listesini (neverop) düzenler. NOP seviyesindeki
	bir kullanici kanalda opearatör mode'unu (+o) alamaz ve bu mode kullanicilar
	tarafindan verilirse, ChanServ kullanicidan +o mode'unu geri alir.
	
	ADD, DEL, LIST, ve COUNT alt komutlari, AOP komutunun alt
	komutlariyla ayni bicimdedir.


CHAN_HELP_ACCESS
	Kullanimi: ACCESS kanal ADD nick seviye
	        ACCESS kanal DEL nick
	        ACCESS kanal LIST [+baslangic degeri] [maske]
	        ACCESS kanal LISTLEVEL [+baslangic degeri] seviye
	        ACCESS kanal COUNT
	
	Kanalin erisim listesi'ni (access list) duzenler.Bu liste
	kanalda kimlerin oto-op olacagi ve/veya %S komutlarina
	kimlerin erisebilecegi gibi bilgileri saklar.Degisik erisim
	haklari icin farkli erisim seviyeleri (access level)
	belirlenmistir.Standart olan bu seviyelerin buyuk bir kismi
	kanal sahibi tarafindan degistirilebilir(bkz.
	/msg %S HELP ACCESS LEVELS). Bir kanalin kullanici
	listesinde (access list) bulunmayan her nick'in seviyesi
	varsayilan deger olarak 0'dir.
	
	ACCESS ADD komutu belirtilen nick'i, yine belirtilen seviye
	ile kanalin erisim listesine eklemek icin kullanilir.Burada
	sozedilen seviye komutu kullanan kullanicininkinden sayica
	daha dusuk olmak zorundadir.Eger eklenmek istenen kullanici
	zaten listedeyse de komutu veren kullanicinin seviyesi
	(access level) daha yuksek olmalidir.Baska bir deyisle
	sizinkinden yuksek seviyeli bir kullanici icin bu komutu
	kullanamazsiniz.
	
	ACCESS DEL komutu belirtilen nick'i kanalin erisim
	listesinden siler. Kullanicilar yanlizca erisim seviyeleri
	kendi seviyelerinden düþük olan nick'leri silebilirler.
	
	ACCESS LIST komutu kanalin erisim listesini (access list)
	goruntuler.Eger bir joker karakter ile maskeyi belirtirseniz,
	yanlizca maske ile uyusan kayitlari görüntülersiniz.
	Eger baslangic degeri belirttiyseniz
	geriye dönük degerler alirsiniz.Soyleki; Eger '+50' parametresini
	kullanirsaniz ilk 50 sonuctan sonrasini almis olursunuz.
	
	ACCESS LISTLEVEL komutu ACCESS LIST komutu ile benzerlik gösterir,
	fakat yanlizca belirtilen erisim seviyelerindeki kayitlari gösterir. Seviye
	bir dizi olarak belirtilebilir. Örnegin: LISTLEVEL 50-100, erisim seviyesi
	50 ile 100 arasini kapsayan bütün kullanicilari listeler.
	
	ACCESS COUNT komutu, kanalin access listesinde kac nick oldugunu
	gösterir.

CHAN_HELP_ACCESS_XOP
	
	Access listeleri, ayrica SOP, AOP, VOP ve de NOP
	komutlariyla da degistirilebilir. Bu komutlar, %d, %d, %d
	ve de %d erisim derecelerine denk düsmektedirler.
CHAN_HELP_ACCESS_XOP_HALFOP
	
	Access listeleri, ayrica SOP, AOP, HOP, VOP ve de VOP
	komutlariyla da degistirilebilir. Bu komutlar, %d, %d, %d,
	%d ve de %d erisim derecelerine denk düsmektedirler.

CHAN_HELP_ACCESS_LEVELS
	Kullanici Seviyeleri
	
	Varsayilan degerler olarak asagidaki seviyeler belirlenmistir:
	
	     Founder  Kanal sahibi olarak %S komutlarina tam
	                  yetki, kanala giriste otomatik op.Dikkat
	                  edilirse sadece bir kisi founder
	                  ozelliklerine sahip olabilir.(ACCESS komutu
	                  kullanilarak paylasilamaz).
	        %4d  AKICK komutunu kullanma yetkisi,otomatik op.
	        %4d  Kanala giriste otomatik op.
CHAN_HELP_ACCESS_LEVELS_HALFOP
	        %4d  Kanala giriste otomatik halfop (+h).
CHAN_HELP_ACCESS_LEVELS_END
	        %4d  Kanala giriste otomatik voice (+v).
	           0  Siradan kullanici.secure-ops ozelliginin aktif
	                  olmadigi durumlarda op veya voice verilebilir.
	          -1  Kullanici kanalda op olamaz.
	        -100  Kullanici kanala giremez.
	
	Bu seviyeler LEVELS komutu kullanilarak degisebilir ve/veya
	yenileri eklenebilir. (Son iki deger haric) Daha ayrintili bilgi icin
	/msg %S HELP LEVELS yaziniz.


CHAN_HELP_LEVELS
	Kullanimi: LEVELS kanal SET ozellik seviye
	           LEVELS kanal {DIS | DISABLE} ozellik
	           LEVELS kanal RESET
	           LEVELS channel LIST
	
	LEVELS komutu kanaldaki erisim seviyelerinin varsayilan
	degerlerini duzenlemek icin kullanilir.Surekli kullanilan
	%S ozelliklerine degisik numerik seviyeler vererek
	onlari daha cok sayida kullaniciya acar veya kaparsiniz.
	Ancak SET FOUNDER ve SET PASSWORD komutlari hala kanal
	sahibine (founder) ozel birakilmistir.
CHAN_HELP_LEVELS_XOP
	
	Dikkat: Bu komut sadece tecrübeli kullanicilar icindir.
	Erisim seviyelerini bu komutla degistirdiginizde, AOP, SOP ve de
	VOP komutlari dogru calismayacaklardir.
CHAN_HELP_LEVELS_XOP_HOP
	
	Dikkat: Bu komut sadece tecrübeli kullanicilar icindir.
	Erisim seviyelerini bu komutla degistirdiginizde, AOP, SOP, HOP ve de
	VOP komutlari dogru calismayacaklardir.

CHAN_HELP_LEVELS_END
	
	LEVELS SET komutu ile bir veya birkac ozelligin varsayilan
	numerik degerleri degistirilebilir.LEVELS DISABLE veya DIS
	komutu ileyse bu ozellik(ler) iptal edilebilir.
	
	LEVELS LIST komutuysa o anki erisim seviyelerinin numerik
	degerlerini listeler.LEVELS RESET komutu butun numerik
	degerleri baslangictaki konumlarina getirir.

CHAN_HELP_LEVELS_DESC
	Gecerli ozelliklerin listesi asagidadir.

CHAN_HELP_AKICK
	Syntax: AKICK channel ADD maske [aciklama]
	        AKICK channel DEL maske
	        AKICK channel LIST [+baslangic degeri] [maske]
	        AKICK channel VIEW [+baslangic degeri] [maske]
	        AKICK channel COUNT
	        AKICK channel ENFORCE
	
	Bir kanal icin autokick listesini düzenler. Eger autokick
	listesinde bulunan bir kullanici kanala girmeye calisirsa, %S
	tarafindan kanaldan banlanir ve atilir.
	
	AKICK ADD komutu verilen user@host veya
	nick!user@hostu autokick listesine ekler. Eger
	bir aciklama verilmis ise bu kullanilir aksi takdirde
	kullanici atilirken "You have been banned from the channel"
	mesaji kullanilir.
	
	AKICK DEL komutu verilen mask i autokick listesindan cikarir.
	Ancak bu veriye uyan mevcut banlari kaldirmaz, bunlar elle
	kaldirilmalidir.
	
	AKICK LIST komutu autokick listesini veya verilen maska uygun
	olan verileri görüntüler.
	AKICK VIEW benzerdir ancak daha ayrintili bilgi verir.
	Eger bir baslangic degeri belirttiyseniz
	geriye dönük degerler alirsiniz.Soyleki; Eger '+50' parametresini
	kullanirsaniz ilk 50 sonuctan sonrasini almis olursunuz.
	
	AKICK COUNT komutu ile listede kac adet verinin bulundugu
	görüntülenebilir.
	
	AKICK ENFORCE komutu ile %S e o anda kanalda bulunan ve
	autokick listesindeki verilere uyan kisileri uzaklastirir.
	
	Standart ayar olarak kanalda %s ve üzeri access bulunan
	kullanicilara acik bir komuttur.

CHAN_HELP_INFO
	Kullanimi: INFO kanal [ALL]
	
	Belirtilen kanalin bilgilerini goruntuler.Kanal bilgileri
	icinde bazi kisimlar saklanmis veya yazilmamis olabilir.
	Bu bilgiler, sahibin nicki, kayit tarihi, son kullanilma
	tarihi ve de mod kilidi gibi bilgilerdir. Kanal sahibi
	ayrica ALL parametresini kullanarak, giris mesaji ve ya
	successor bilgilerini de görebilir.

CHAN_HELP_LIST
	Kullanimi: LIST [+baslangic degeri] paramatre
	
	Belirtilen parametreye uyan butun kayitli kanallari listeler.
	(PRIVATE secenegi aktif olan kanallar listelenmez).
	Eger bir baslangic degeri belirttiyseniz
	geriye dönük degerler alirsiniz.Soyleki; Eger '+50' parametresini
	kullanirsaniz ilk 50 sonuctan sonrasini almis olursunuz.
CHAN_HELP_LIST_OPERSONLY
	
	Bu komutu sadece Irc Operatörleri kullanabilir.

CHAN_HELP_OP
	Kullanimi: OP kanal nick
	
	Belirtilen nick'e sozkonusu kanalda op verir.Varsayilan
	deger olarak sadece kanaldaki erisim seviyesi %s ve uzeri
	kisiler bu komutu kullanabilir.

CHAN_HELP_DEOP
	Kullanimi: DEOP kanal nick
	
	Belirtilen nick'in sozkonusu kanaldaki op'unu alir.
	Varsayilan deger olarak sadece kanaldaki erisim seviyesi %s
	ve uzeri olan kisiler bu komutu kullanabilir.

CHAN_HELP_VOICE
	Kullanimi: VOICE kanal nick
	
	Belirtilen nicke söz konusu kanalda voice verir. Varsayilan deger
	olarak erisim seviyesi %s ve de üstünde olan kisiler bu
	komutu kullanabilir.

CHAN_HELP_DEVOICE
	Kullanimi: DEVOICE kanal nick
	
	Belirtilen nickin voice'ini alir. Varsayilan deger olarak
	erisim seviyesi %s ve de üstünde olan kisiler bu komutu
	kullanabilir.

CHAN_HELP_HALFOP
	Kullanimi: HALFOP kanal nick
	
	Belirtilen nicke halfop yetkisi verir. Varsayilan deger
	olarak, erisim seviyesi %s ve de üstünde olan kisiler bu
	komutu kullanabilirler.

CHAN_HELP_DEHALFOP
	Kullanimi: DEHALFOP kanal nick
	
	Belirtilen kisinin halfop'unu alir. Varsayilan deger olarak
	erisim seviyesi %s ve de üstünde olan kisiler bu komutu
	kullanabilirler.

CHAN_HELP_PROTECT
	Kullanimi: PROTECT kanal nick
	
	Belirtilen nicke koruma modu olan +a yi belirtilen kanalda
	verir. Varsayilan deger olarak, erisim seviyesi %s ve de
	ustu olan kisiler bu komutu kullanabilir.

CHAN_HELP_DEPROTECT
	Kullanimi: DEPROTECT kanal nick
	
	Belirtilen nickten koruma modu olan +a yi belirtilen kanalda
	kaldirir. Varsayilan deger olarak, erisim seviyesi %s ve de
	ustu olan kisiler bu komutu kullanabilir.

CHAN_HELP_INVITE
	Kullanimi: INVITE kanal
	
	%S 'un sizi belirtilen kanala cagirmasini (INVITE)
	saglar.Varsayilan deger olarak sadece kanaldaki erisim
	seviyesi en az %s olan kisiler bu komutu kullanabilir.

CHAN_HELP_UNBAN
	Kullanimi: UNBAN kanal
	
	%S 'un sizin kanala girisinizi engelleyen tum ban'lari
	kaldirmasini saglar.Varsayilan deger olarak sadece kanaldaki
	erisim seviyesi en az %s olan kisiler bu komutu kullanabilir.

CHAN_HELP_TOPIC
	Kullanimi: TOPIC kanal topic
	
	%S 'un kanala topic atmasini saglar.Ozellikle
	TOPICLOCK ozelliginin aktif oldugu zamanlarda
	kullanislidir. Standart olarak, erisim seviyesi %s ve de üzeri
	olan kullanicilar bu komutu kullanabilir.

CHAN_HELP_KICK
	Kullanimi: KICK kanal nick [sebep]
	
	%S, belirtilen kullaniciyi, belirtilen kanaldan atacaktir.
	Sebep verilmis ise bu kullanilacak, ya da standart bir sebep
	gösterilecektir. Varsayilan deger olarak, erisim seviyesi
	en az %s olan kullanicilar bu komutu kullanabilirler.

CHAN_HELP_KICK_PROTECTED
	
	Koruma (+a) altinda bulunan kullanicilar ve de kanal kurucusu
	(founder) bu komut ile kanaldan atilamaz.

CHAN_HELP_CLEAR
	Kullanimi: CLEAR kanal parametre
	
	%S'un kanaldaki belirtilen parametre'yi sifirlamasini
	saglar.Gecerli parametre'ler sunlardir:
	
	    MODES       Kanaldaki butun mode'lari sifirlar.
	                    (i,k,l,m,n,p,s,t).
	    BANS        Kanaldaki butun ban'lari sifirlar.
CHAN_HELP_CLEAR_EXCEPTIONS
	    EXCEPTIONS  Kanaldaki tüm ban asimlarini (+e) sifirlar.
CHAN_HELP_CLEAR_INVITES
	    INVITES     Kanaldaki tüm davet (invite) maskelerini siler.
CHAN_HELP_CLEAR_MID
	    OPS         Kanaldaki +o mode'unu (tum kanal oplarini)
	                    sifirlar.
CHAN_HELP_CLEAR_HALFOPS
	    HALFOPS     Kanaldaki tüm halfoplari (+h) temizler.
CHAN_HELP_CLEAR_END
	    VOICES      Kanaldaki +v mode'unu (tum +'lari) sifirlar.
	    USERS       Kanaldaki tum kullanicilari atar.
	
	Varsayilan deger olarak erisim seviyesi en az %s olan kullanicilar
	bu komutu kullanabilirler.

CHAN_HELP_STATUS
	Kullanimi: STATUS kanal nick
	
	Belirtilen kullanicinin kanaldaki erisim seviyesini gösterir.
	Bu cevap asagidaki formda olur.
	    STATUS kanal nick erisim-seviyesi
	Eger bir hata meydana gelmisse:
	    STATUS kanal nick ERROR mesaj
	Seklinde bir cevap verilecektir.
	
	Varsayilan deger olarak, erisim seviyesi en az %s olan kullanicilar
	bu komutu kullanabilirler.

CHAN_OPER_HELP_COMMANDS
	
	Asagidaki komutlari sadece Services admin'ler kullanabilir.:

CHAN_OPER_HELP_COMMANDS_GETPASS
	    GETPASS   Belirtilen kanalin sifresini verir.
	                 (eger sifreleme (encryption) devre disiyken)
CHAN_OPER_HELP_COMMANDS_FORBID
	    FORBID    Belirtilen kanalin kullanimini yasaklar.
	    SUSPEND   Belirtilen kanalin kaydini silmez ama
	                 kullanimini yasaklar.
	    UNSUSPEND Belirtilen kanalin üstündeki suspendi kaldirir.
CHAN_OPER_HELP_COMMANDS_END
	
	Services admin'ler ayrica IDENTIFY komutunu kullanmaksizin
	bir kanalin kaydini silebilir (DROP), sifresini
	degistirebilir ve/veya akick ve erisim listesine
	(/msg %S ACCESS kanal {ADD | DEL | LIST}) komutu ile
	mudahale edebilirler, STATUS komutu ile bir kullanicinin
	erisim seviyesini ögrenebilirler.

CHAN_OPER_HELP_DROPCHAN
	Syntax: DROPCHAN kanal
	
	%S veritabanindan belirtilen kanali siler.
	
	Bu komutu sadece Services admin'ler kullanabilir.

CHAN_OPER_HELP_SET
	
	Services admin'ler NOEXPIRE komutu ile herhangi bir kanalin
	zaman asimi yuzunden silinmesi ozelligini kapatabilirler.
	Buna ek olarak Services admin'ler herhangi bir kanalin her
	ozelligini de kanal sifresini girmelerine gerek olmaksizin
	degistirebilirler.

CHAN_OPER_HELP_SET_NOEXPIRE
	Kullanimi: SET kanal NOEXPIRE {ON | OFF}
	
	Belirtilen kanalin zaman asimi nedeniyle silinecegi veya
	silinmeyecegini belirler.Bu ozelligin ON seklinde
	ayarlanmasi kanalin zaman asimi ile silinmesini
	engelleyecektir.
	
	Bu komutu sadece Services admin'ler kullanabilir.

CHAN_OPER_HELP_UNSET
	
	SET komutunda oldugu gibi UNSET komutu ile de services adminler
	bir kanalin herhangi bir özelligini silebilirler.

CHAN_OPER_HELP_INFO
	
	Services admin'ler ALL parametresini her kanala kullanabilir.

CHAN_OPER_HELP_LIST
	Kullanimi: LIST [+baslangic degeri] parametre [NOEXPIRE] [FORBIDDEN] [SUSPENDED]
	
	Belirtilen parametreye uyan tüm kanallari gösterir. PRIVATE özelligi
	bulunan kanallar sadece services adminlere gösterilecektir.
	NOEXPIRE özelligi bulunan kanallarin basinda bir ! isareti
	bulunacaktir.
	
	Eger bir baslangic degeri belirttiyseniz geriye dönük degerler alirsiniz.
	Soyleki; Eger '+50' parametresini kullanirsaniz ilk 50 sonuctan sonrasini
	almis olursunuz.
	
	Bu üc özellik; NOEXPIRE, FORBIDDEN, SUSPENDED; kullanildiklarinda,
	sadece bu tip kanallar gösterilir. Bunlar, herhangi bir kombinasyon
	ile kullanilabilir.Bu üc özellik sadece services adminler
	tarafindan kullanilabilir.

CHAN_OPER_HELP_GETPASS
	Kullanimi: GETPASS kanal
	
	Belirtilen kanalin sifresini verir.Bu komut kullanildiginda
	butun IRCop'larin gorebilecegi sekilde ve komutu kullananin
	nick'ini de iceren bir notice atilir.
	
	Sifreleme gecerli iken, bu komut sifreleme derecesine bagli
	olarak gecerli olmayabilir.
	
	Bu komutu sadece Services admin'ler kullanabilir.

CHAN_OPER_HELP_FORBID
	Kullanimi: FORBID kanal
	
	Belirtilen kanalin kullanimini yasaklar.Yasagin kalkmasi icin
	sozkonucu nick'in %S veritabanindan silinmesi gerekir
	(DROP)
	
	Bu komutu sadece Services admin'ler kullanabilir.

CHAN_OPER_HELP_SUSPEND
	Kullanimi: SUSPEND [+süre] kanal gerekce
	
	Belirtilen kanali dondurur. Dondurulmus kanallar bilgilerini
	kaybetmezler, ve de süresi gelince de düserler. Bu kanallar
	UNSUSPEND komutu ile tekrar normal hale getirilebilir.
	süre parametresi tipki %s AKILL komutunda oldugu gibi
	kullanilmaktadir. Dondurulmis bir kanala, tipki FORBID
	komutunda oldugu gibi, girilemez.
	
	Bu komutu sadece Services admin'ler kullanabilir.

CHAN_OPER_HELP_UNSUSPEND
	Kullanimi: UNSUSPEND kanal
	
	Belirtilen kanaldaki suspendi kaldirip, kanali yeniden
	kullanilabilir hale getirir.
	
	Bu komutu sadece Services admin'ler kullanabilir.


###########################################################################
#
# MemoServ help messages
#
###########################################################################

MEMO_HELP
	%S o anda IRC'de olan veya olmayan kullanicilara not
	birakmanizi saglar.Notu hem gonderen,hem de alacak kisinin
	nickleri kayitli olmalidir.
	
	Mevcut komutlari ögrenmek icin /msg %S HELP COMMANDS komutunu
	kullanabilirsiniz. Komutlar /msg %S komut yazarak calistirilirlar.
	Belirli bir komut hakkinda daha ayrintili bilgi almak icinse
	/msg %S HELP komut yazmaniz yeterlidir.
MEMO_HELP_EXPIRES
	
	Dikkat: Notlariniz, %s tarafindan bir süre sonra zamanasimina
	ugrayabilir. Bunu engellemek icin, silinmesini istemediginiz notlari
	SAVE komutu ile korumaya almalisiniz.
MEMO_HELP_END_LEVELS
	
	(*) Varsayilan deger olarak, erisim seviyesi en az %s olan kullanicilar
	    kanal notlarini okuyabilirler, bunu degistirmek icin %s LEVELS
	    komutu kullanilabilir.
MEMO_HELP_END_XOP
	
	(*) SOP listesindeki her kullanici kanal notlarini okuyabilir.

MEMO_HELP_COMMANDS
	%S komutlari:
	
	    SEND     Belirtilen nick'e not birakir.
	    LIST     Notlarinizi listeler.
	    READ     Bir notu okumanizi saglar.
MEMO_HELP_COMMANDS_FORWARD
	    FORWARD  Gelen notlarinizi email adresinize gönderir.
MEMO_HELP_COMMANDS_SAVE
	    SAVE     Notlarinizi zaman asimindan cikartir.
MEMO_HELP_COMMANDS_DEL
	    DEL      Bir notu (veya hepsini) siler.
	    SET      Notlarinizla ilgili secenekleri duzenler.
	    INFO     Notlarinizin durumu hakkinda bilgi verir.
MEMO_HELP_COMMANDS_IGNORE
	    IGNORE   Bazi kullanicilarin size not atmasini engeller.

MEMO_HELP_SEND
	Kullanimi: SEND {nick | kanal} not-metni
	
	Belirtilen nick veya kanal'a not-metni'ni gonderir.
	Not gonderildiginde karsi taraf notice seklinde bir uyari
	alacaktir.Karsi taraftaki kanal veya nick kayitli olmalidir.

MEMO_HELP_LIST
	Kullanimi: LIST [numara | liste | NEW]
	
	O anki notlarinizi listeler.NEW parametresi kullanildiginda
	sadece yeni (yani okunmamis) notlarinizin bir listesini
	alirsiniz.Okunmamis notlarin basinda "*" isareti olacaktir.
	Ayrica bir liste belirtmeniz de mümkündür:
	
	    LIST 2-5,7-9
	        2 ile 5 arasindaki ve 7 ile 9 numara arasindaki notlarinizi
	        gösterir.

MEMO_HELP_LIST_EXPIRE
	Kullanimi: LIST [NEW]
	
	O anki notlarinizi listeler.NEW parametresi kullanildiginda
	sadece yeni (yani okunmamis) notlarinizin bir listesini
	alirsiniz.Okunmamis notlarin basinda "*" isareti olacaktir.
	Zamanasimina ugramayacak notlar ise "+" isaretiyle belirtilirler.

MEMO_HELP_READ
	Kullanimi: READ {num | liste | LAST | NEW}
	
	num numarali notu, veya LAST parametresi ile en son notunuzu
	goruntuler. NEW parametresi ile tüm yeni notlarinizi okuyabilirsiniz.
	Tipki LIST komutunda oldugu gibi, bir liste de belirtmek mümkündür.

MEMO_HELP_SAVE
	Kullanimi: SAVE {num | liste}
	
	Belirtilen not ya da notlarin zaman asimina ugramalarini engeller.
	Birden fazla not belirtmek icin LIST komutundaki gibi bir parametre
	vermek mümkündür.

MEMO_HELP_DEL
	Kullanimi: DEL {num[-num][,num...] | ALL}
	
	Eger num kisminda bir sayi verilmisse o notu siler.
	Birden fazla numara veya bir aralik belirtebilirsiniz; ayni
	LIST komutunda oldugu gibi.
	
	ALL yazarak size ait tum notlari silebilirsiniz.
	
	Uyari: Bir not silindikten sonra geri getirilemez.

MEMO_HELP_RENUMBER
	Syntax: RENUMBER
	
	Not listenizi yeniden numaralandirip, silinmis notlar yüzünden
	olusmus siralama bosluklarini düzenli hale getirir.

MEMO_HELP_SET
	Kullanimi: SET secenek parametre
	
	Cesitli not seceneklerini duzenler.Secenek olarak:
	
	    NOTIFY   Yeni notlarda ne zaman uyarilacaginizi
	                     belirler.(sadece nick'ler icin)
	    LIMIT    Alabileceginiz en fazla not sayisini
	                     belirler.
MEMO_HELP_SET_OPTION_FORWARD
	    FORWARD  Not iletimi ile ilgili ayarlari yapmanizi
	                     saglar.
MEMO_HELP_SET_END
	
	/msg %S HELP SET secenek yazarak belirli bir komut
	hakkinda ayrintili bilgi alabilirsiniz.

MEMO_HELP_SET_NOTIFY
	Kullanimi: SET NOTIFY {ON | LOGON | NEW | OFF}
	
	Yeni notlar geldiginde ne zaman uyari alacaginizi belirler.
	
	    ON      IRC'ye girer girmez /AWAY 'den doner donmez ve
	                notlar size gonderildikleri anda
	                uyarilirsiniz.
	    LOGON   IRC'ye girer girmez ve /AWAY 'den doner donmez
	                uyarilirsiniz.
	    NEW     Sadece notlar size gonderildikleri anda
	                uyarilirsiniz.
	    OFF     Bu secenekle herhangi bir uyari almazsiniz.
	
	ON secenegi aslinda LOGON ve NEW seklindedir.
	
	Dikkat edilmelidir ki, nick'inizin "secure" opsiyonu aciksa,
	sadece %s ye IDENTIFY komutu ile kendinizi tanittiktan sonra
	yeni memolarin geldiklerini ögrenebilirsiniz.

MEMO_HELP_SET_LIMIT
	Kullanimi: SET LIMIT [kanal] limit
	
	Sizin veya belirtilen kanalin alabilecegi en fazla not
	sayisini belirler.Eger bu limiti 0 olarak ayarlarsaniz
	size not birakilamaz.Bu limit en fazla %d olabilir.

MEMO_HELP_INFO
	Kullanimi: INFO [kanal]
	
	Notlarinizi ve ilgili secenekleri listeler.Notlariniz
	okunmus ve okunmamislar olarak ayrilirken,alabileceginiz
	en fazla not sayisini da bu listede gorebilirsiniz.Ek bir
	parametre ile belirtilen bir kanal hakkinda da benzer bir
	liste alinabilir.

MEMO_OPER_HELP_COMMANDS
	
	Services adminler SET LIMIT ve de INFO komutlarini her nick
	icin kullanabilirler.

MEMO_OPER_HELP_SET_LIMIT
	Kullanimi: SET LIMIT [kullanici | kanal] {limit | NONE | DEFAULT} [HARD]
	
	Bir kullanici veya kanalin alabilecegi en fazla not sayisini
	belirler.Bu limiti 0 yapmak kullanicinin not alamamasini ve
	NONE yapmak ta kullanicinin not limitinin kalkmasini saglar.
	Eger bir nick veya kanal belirtmezseniz kendi limitinizi
	belirlersiniz.
	
	DEFAULT parametresi, not limitini services standard degeri olan
	%d ile degistirecektir.
	
	HARD parametresini eklemek kullanicinin limitini
	degistirmesini engeller.
	
	SET LIMIT komutunu baska bir nick belirterek sadece
	Services admin'ler kullanabilir.Diger kullanicilar sadece
	kendi veya erisimleri olan kanallarin limitlerini
	belirleyebilirler,kendi limitlerini kaldiramazlar ve/veya
	kendilerine %d 'den yuksek bir limit veremezler.

MEMO_OPER_HELP_INFO
	Kullanimi: INFO [nick | kanal]
	
	Bu komut tek basina kullanildiginda not sayinizi,okunmus ve
	okunmamis olarak ayirarak,not limitinizle beraber listeler.
	
	Eger bir kanal belirtilmissea ayni bilgiyi o kanal icin
	listeler.
	
	Eger bir nick belirtilmisse ayni bilgiyi o kanal icin
	listeler. Bu haliyle sadece Services admin'ler kullanabilir.

MEMO_HELP_FORWARD
	Kullanimi: FORWARD {num | liste | ALL}
	
	Bir ya da daha fazla notu, gecerli bir email adresi belirtti
	iseniz, bu adrese gönderecektir. Kanal notlari bu komut
	ile bir email adresine gönderilemez.

MEMO_HELP_SET_FORWARD
	Kullanimi: SET FORWARD {ON | COPY | OFF}
	
	Notlarinizin, %S tarafindan otomatik olarak email adresinize
	gönderilmesi ayarini yapar. Eger ON belirtilmis ise,
	notlar direk olarak email ile gönderilecek, services tarafindan
	saklanmayacaklardir. Saklanmalari icin COPY opsiyonunu
	vermeniz gerekmektedir. Eger OFF opsiyonunu secerseniz,
	notlariniz, sadece services tarafindan saklanacaklardir.
	
	Dikkat edilmelidir ki, ON durumunda herhangi bir hata yüzünden
	notlarin size gönderilmesi mümkün olmayabilir, bu vakit bu notlar
	services tarafindan saklanacaklardir.
	
	Ayrica, COPY opsiyonu acikken, not limitinizi astiginizda da
	yeni not alirsaniz, bu yeni notlar email ile de gönderilemeyeceklerdir.
	Bunun icin DEL komutunu kullanarak bazi notlarinizi silmeniz
	gerekmektedir.

MEMO_HELP_IGNORE
	Kullanimi: IGNORE {ADD | DEL | LIST} [nick | mask]
	
	Ignore listenizi düzenler. Ignore listenizdeki nicklerden ya da
	kullanici bilgileri verdiginiz maskeye uyan kullanicilardan
	size not gönderilemeyecektir.
	
	ADD komutu ile bir nick ya da maske bu listeye eklenir, DEL
	ile cikartilir. LIST ile aktüel liste görüntülenebilir.
	
	Maske icinde ? ya da * isaretleri bulunabilir, ayrica büyük kücük
	harf ayrimi yapilmaz. *Evil* seklinde bir maske, EvilNick nickini
	de icerecektir.

###########################################################################
#
# OperServ help messages #mikado#
#
###########################################################################

OPER_HELP
	%S irc operatörlerinin degisik network özelliklerini düzenlemelerine
	yardimci olur.  Komut listesini almak icin /msg %S HELP COMMANDS
	komutunu kullanabilirsiniz.
	
	%S e gönderilen tüm komutlar loglanacaktir.

OPER_HELP_COMMANDS
	%S komutlar:
	    GLOBAL      Butun kullanicilara notice atar.
	    STATS       Services'in ve network'un durumu.
	    SERVERMAP   Network haritasini gösterir.
	    OPER LIST   Services operator'leri listeler.
	    ADMIN LIST  Services admin'leri listeler.
OPER_HELP_COMMANDS_SERVOPER
	
	Services operator ve admin'lere ozel komutlar:
	    MODE        Bir kanalin mode'unu degistirir.
	    KICK        Kanaldaki bir kullaniciyi atar.
	    CLEARMODES  Belirtilen modlari kanalda kapatir
	    CLEARCHAN   Belirtilen kanaldaki herkesi kickler (masskick)
	    KILLCLONES  Bir nickin tüm klonlarina KILL atar.
OPER_HELP_COMMANDS_AKILL
	    AKILL       Autokill listesini düzenler.
	    AKILLCHAN   Bir kanaldaki kullanicilara autokill atar.
OPER_HELP_COMMANDS_EXCLUDE
	    EXCLUDE     Autokill asim listesini (EXCLUDE) düzenler.
OPER_HELP_COMMANDS_SLINE
	    SQLINE      Sqline listesini düzenler.
	    SGLINE      Sgline listesini düzenler.
	    SZLINE      Szline listesini düzenler.
OPER_HELP_COMMANDS_SESSION
	    SESSION     Oturum listesini gösterir.
	    EXCEPTION   Oturum listesi özel durumlari düzenler.
OPER_HELP_COMMANDS_NEWS
	    LOGONNEWS   LOGON mesajini belirler
	    OPERNEWS    OPER mesajini belirler
OPER_HELP_COMMANDS_SERVADMIN
	
	Services admin'lere ozel komutlar:
	    OPER        Services operator listesini duzenler
	    JUPE        Bir server'i "Jupiter" eder
	    SU          Services Root yetkilerine erismeye yarar.
	    SET         Cesitli Services seceneklerini duzenler
	    UPDATE      Services veritabanini gunceller.
	    SHUTDOWN    Veritabanini kaydedip, services'i kapar
	    QUIT        Services'i kapatir.
	    REHASH      Ayar dosyasini yeniden okur.
	    RESTART     Veritabanini kaydederek Services'i
	                  yeniden baslatir.
OPER_HELP_COMMANDS_SERVROOT
	
	Services root'a ozel komutlar:
	    ADMIN       Services admin listesini duzenler
OPER_HELP_COMMANDS_RAW
	    RAW         IRC sunucusuna bir RAW komutu yollar

OPER_HELP_GLOBAL
	Kullanimi: GLOBAL metin
	
	IRCop'larin network'teki tum kullanicilara mesaj
	gondermelerini saglar.
	
	Mesaj %s nick'inden gelen bir notice seklinde
	gorunecektir.

OPER_HELP_STATS
	Kullanimi: STATS [RESET | NETWORK | ALL]
	
	Bu komut tek basina kullanildiginda mevcut kullanici ve
	IRCop sayisini (services haric) , services acildigindan
	beri en yüksek kullanici sayisini ve de services in ne
	kadar zamandir calistigini görüntüler.
	
	RESET opsiyonu, en yüksek kullanici sayisini o anki
	cevrimici (online) kullanici sayisina esitler.
	
	NETWORK opsiyonu, uzaktaki sunucudan alinan ve de
	uzaktaki sunucuya yazilan veri miktarini ve de buffer
	durumunu gösterir, ayrica harcanilan bellek miktarini
	da belirtir. Eger yüzdesel limit ayari yapilmis ise,
	bu veri de belirtilir.
	
	ALL secenegini sadece services Adminler kullanabilir.
	Bu komut servicesin kullandigi bellegi görüntüler. Bu komutu
	kullanmak büyük aglarda (network) Services i kisa süreyle
	dondurabileceginden ötürü gereksiz yere kullanmayiniz.
	
	STATS yerine UPTIME komutu da kullanilabilir.

OPER_HELP_SERVERMAP
	Kullanimi: SERVERMAP
	
	Bagli serverlarin bir haritasini gösterir. Services in kendisi
	ve de jupe edilmis serverlar ve de hayali serverlar (*) isareti
	ile gösterilecektir.

OPER_HELP_OPER
	Kullanimi: OPER ADD nick
	           OPER DEL nick
	           OPER LIST
	
	Services admin'lerin Services operator listesini
	duzenlemelerini saglar.Bu listedeki her kullanici %s'e
	sifresini girdikten sonra Services operator komutlarini
	kullanabilecektir.
	
	Komutun OPER LIST seklini her IRCop kullanabilir.Butun diger
	kullanimlar Services admin'lere ozeldir.

OPER_HELP_ADMIN
	Kullanimi: ADMIN ADD nick
	           ADMIN DEL nick
	           ADMIN LIST
	
	Services root'un Services admin listesini duzenlemesini
	saglar.Bu listedeki her kullanici %s'e sifresini
	girdikten sonra Services admin komutlarini
	kullanabilecektir.
	
	Komutun ADMIN LIST seklini her IRCop kullanabilir.Butun
	diger kullanimlar Services root'a ozeldir.

OPER_HELP_GETKEY
	Syntax: GETKEY kanal
	
	Eger belirtilen kanal anahtar mode'una (+k) sahipse bu anahtari gösterir.
	
	Bu komutu sadece Services admin'ler kullanabilir.

OPER_HELP_MODE
	Kullanimi: MODE kanal mode(lar)
	
	Services operator'larin herhangi bir kanalin mode'larini
	duzenlemelerini saglar.Kullanilicak parametreler /MODE
	komutununkilerle aynidir.
	
	Bu komutu sadece Services operator'lar kullanabilir.

OPER_HELP_CLEARMODES
	Kullanimi: CLEARMODES kanal [ALL]
	
	Kanaldaki butun "binary mode"'lari (i,k,l,m,n vb.) ve
	banlari sifirlar.ALL parametresi ile kullanildiginda op ve
	voice gibi kullanici modlari (+o, +v vb.) sifirlanir.
	
	Bu komutu sadece Services operator'lar kullanabilir.

OPER_HELP_CLEARCHAN
	Kullanimi: CLEARCHAN kanal
	
	IRCop'larin bir kanali temizlemelerini saglar. Services bu islemi
	ChanServ i kanala sokup, global bir ban ve de +l 1 ile kanali
	güvenceye alip, herkesi deoplayip kicklemekle yerine getirir.
	
	Lütfen bu komutu dikkatsizce kullanmayiniz.
	
	Bu komutu sadece Services operator'ler kullanabilir.

OPER_HELP_KICK
	Kullanimi: KICK kanal kullanici sebep
	
	IRCop'larin herhangi bir kanaldaki kullaniciyi kanaldan
	atmalarini saglar.Kullanilicak parametreler /KICK
	komutununkilerle aynidir.Kick mesajinin yaninda komutu
	kullanan IRCop'un nick'i de gorunecektir.Ornegin :
	
	*** SpamMan has been kicked off channel #kanal by %S
	(mikado (Flood))
	
	Bu komutu sadece Services operator'lar kullanabilir.

OPER_HELP_AKILL
	Kullanimi: AKILL ADD [+sure] mask sebep
	           AKILL DEL mask
	           AKILL LIST [mask] [NOEXPIRE]
	           AKILL VIEW [mask] [NOEXPIRE]
	           AKILL COUNT
	
	Services operators'larin AKILL listesini duzenlemelerini
	saglar.AKILL listesindeki bir kullanici baglanmaya
	calistiginda Services o kullaniciyi KILL edecek ve
	destekleyen sunucu turlerinde network'teki tum sunuculardan
	da kullaniciyi banlayacaktir (K-Line).
	
	AKILL ADD komutu belirtilen user@host formatindaki nick'i
	AKILL listesine belirtilmek zorunda olan sebep ile ekler.
	
	AKILL DEL komutu belirtilen veriyi AKILL listesinden siler.
	
	AKILL LIST tek basina AKILL listesinin tamamini listeler.
	Eger bir secenek verilmisse sadece ona uyan veriler
	listelenir.
	
	AKILL VIEW komutu ise akill'i ekleyen kisinin nick'i,
	otomatik olarak silinecegi suregibi ayrintilari da
	goruntulemesi acisindan AKILL LIST'e gore daha
	kullanislidir.
	AKILL COUNT ile AKILL listesindeki toplam akill sayisini
	ögrenebilirsiniz.
	
	AKILL ADD komutu ile beraber bir zaman asimi suresi de
	belirtilebilir.Bu sure d (gun), h (saat) veya m (dakika)
	seklinde olabilir.(1h30m) gibi kombinasyonlar gecerlidir.
	Varsayilan secenek gundur.Yani +30 30 gun anlamina gelir.Bir
	AKILL'in otomatik olarak silinmemesi icin sure kisminda +0
	yazilmasi gerekir.
	
	sebep belirttiginizde, bu services in ayarina bagli olarak
	diger kullanicilar tarafindan da görülebilir.
OPER_HELP_AKILL_OPERMAXEXPIRY
	
	Services operatörleri en fazla %s kadar bir süre belirtebilirler.
	Services adminler bu limitten etkilenmezler.
OPER_HELP_AKILL_END
	
	Bu komutu sadece Services operator'lar kullanabilir.

OPER_HELP_AKILLCHAN
	Kullanimi: AKILLCHAN [KILL] [+süre] kanal aciklama
	
	Belirtilen kanaldaki kullanicilara, ircoplar haric, tek
	tek autokill atar. Eger KILL belirtilmis ise, ayrica tek tek
	KILL göndererek bunlarin baglantisini keser. Aciklama mesajlari,
	servicesin ayarlanmasina bagli olarak KILL mesajlarinda görülür.
	süre AKILL ADD komutunda oldugu gibi kullanilir, belirtil-
	mezse, %s olarak saptanir.
	
	Bu komutla atilan autokiller *@host formatindadir,
	kanaldaki her bir kullanicinin hostu tek tek alinir. Eger var
	olan bir autokill varsa, bu degistirilmez. Bu yüzden eklenen
	autokill sayisi, kullanici sayisindan daha az olabilir.
	
	Bu komutu sadece services operatorler kullanabilir.

OPER_HELP_EXCLUDE
	Kullanimi: EXCLUDE ADD [+sure] mask sebep
	           EXCLUDE DEL mask
	           EXCLUDE LIST [mask] [NOEXPIRE]
	           EXCLUDE VIEW [mask] [NOEXPIRE]
	           EXCLUDE COUNT
	
	Services operators'larin EXCLUDE listesini duzenlemelerini
	saglar.EXCLUDE listesindeki bir kullanici baglanmaya
	calistiginda Services o kullaniciyi AKILL etmeyecek ve
	destekleyen sunucu turlerinde network'teki tum sunuculardan
	da kullaniciyi banlamayacaktir.
	
	EXCLUDE ADD komutu belirtilen user@host formatindaki nick'i
	EXCLUDE listesine belirtilmek zorunda olan sebep ile ekler.
	
	EXCLUDE DEL komutu belirtilen veriyi EXCLUDE listesinden siler.
	
	EXCLUDE LIST tek basina EXCLUDE listesinin tamamini listeler.
	Eger bir secenek verilmisse sadece ona uyan veriler
	listelenir.
	
	EXCLUDE VIEW komutu ise exclude ekleyen kisinin nick'i,
	otomatik olarak silinecegi suregibi ayrintilari da
	goruntulemesi acisindan EXCLUDE LIST'e gore daha
	kullanislidir.
	
	EXCLUDE COUNT ile EXCLUDE listesindeki toplam akill asimi sayisini
	ögrenebilirsiniz.
	
	EXCLUDE ADD komutu ile beraber bir zaman asimi suresi de
	belirtilebilir.Bu sure d (gun), h (saat) veya m (dakika)
	seklinde olabilir.(1h30m) gibi kombinasyonlar gecerlidir.
	Varsayilan secenek gundur.Yani +30 30 gun anlamina gelir.Bir
	EXCLUDE'in otomatik olarak silinmemesi icin sure kisminda +0
	yazilmasi gerekir.
	
	sebep sadece diger IRCop'larin gormesi icindir.Kullanicilar
	KILL'lenirken bu mesaj kullanilmaz.
	
	Bu komutu sadece Services operator'lar kullanabilir.

OPER_HELP_SGLINE
	Kullanimi: SGLINE ADD [+süre] maske sebep
	           SGLINE DEL maske
	           SGLINE LIST [maske] [NOEXPIRE]
	           SGLINE VIEW [maske] [NOEXPIRE]
	           SGLINE COUNT
	
	Bu komut ile SGLINE listesi düzenlenir. Bu listedeki bir kaleme
	uyan bir kullanici baglandiginda, bu baglanti KILL ile kesilecek,
	ayrica bazi server tiplerinde bu karantina tüm serverlara
	bildirilecektir.
	
	SGLINE ADD komutu ile bir isim ban listesine alinir. Bu durumda
	bir sebep belirtmek sarttir.
	
	SGLINE DEL komutu ile listeden bir nesne silinebilir.
	
	SGLINE LIST komutu ile aktüel SGLINE listesi görüntülenebilir.
	Bir maske belirtildiginde, bu maskeye uyan kalemler görüntülenebilir.
	
	SGLINE VIEW komutu ile LIST komutunda olmayan detaylar görüntülenebilir.
	Ekleyen, eklenme tarihi, silinme tarihi gibi. NOEXPIRE parametresi
	verilmis ise, zamanasimina ugramayacak kalemler gösterilir.
	
	SGLINE COUNT ile kac SQLINE oldugunu görmek mümkündür.
	
	maske parametresi tirnak isaretleri icine alinabilir. Ve de
	eger birden fazla sözcük iceren bir maske varsa, o vakit tirnak isareti
	kullanimi zorunludur. Örnek olarak:
	    /msg %S SGLINE ADD +0 "This is a proxy connection" Proxy kullanmak yasaktir!
	
	SGLINE ADD komutu ile beraber bir zaman asimi suresi de
	belirtilebilir.Bu sure d (gun), h (saat) veya m (dakika)
	seklinde olabilir.(1h30m) gibi kombinasyonlar gecerlidir.
	Varsayilan secenek gundur.Yani +30 30 gun anlamina gelir.Bir
	SGLINE'in otomatik olarak silinmemesi icin sure kisminda +0
	yazilmasi gerekir.
	
	Sebep belirtilmesi gereklidir, ve de bu sebep, bazi services konfigürasyonlarinda
	KILL sebebi olarak da kullanilabilir.
	
	Bu komutu sadece Services operator'lar kullanabilir.

OPER_HELP_SQLINE
	Kullanimi: SQLINE ADD [+süre] maske sebep
	           SQLINE DEL maske
	           SQLINE LIST [maske] [NOEXPIRE]
	           SQLINE VIEW [maske] [NOEXPIRE]
	           SQLINE CHECK maske
	           SQLINE COUNT
	
OPER_HELP_SQLINE_KILL
	Bu komut ile SQLINE listesi düzenlenir. Bu listedeki bir kaleme
	uyan bir kullanici baglandiginda, bu baglanti KILL ile
	kesilecek,
	ayrica bazi server tiplerinde bu karantina tüm serverlara
	bildirilecektir.
OPER_HELP_SQLINE_NOKILL
	Bu komut ile SQLINE listesi düzenlenir. Bu listedeki bir kaleme
	uyan bir kullanici baglandiginda, bu kullanicinin nickinin
	degistirilmesi istenecektir. Ayrica bazi server tiplerinde bu
	karantina tüm serverlara bildirilecektir.
OPER_HELP_SQLINE_IGNOREOPERS
	
	Dikkat ediniz ki, irc operatörleri SQLINE maskelerinden
	etkilenmezler.
OPER_HELP_SQLINE_END
	
	ADD ile bir SQLINE eklenir, DEL ile silinir. LIST ile
	aktüel SQLINE listesi görüntülenir, VIEW ile ek birtakim bilgi
	görmek mümkündür. COUNT ile kac SQLINE oldugunu görmek mümkündür.
	
	Detayli bilgi icin SGLINE komutunu inceleyebilirsiniz.
	/msg %S HELP SGLINE.
	
	Bu komutu sadece Services operator'lar kullanabilir.

OPER_HELP_SZLINE
	Kullanimi: SZLINE ADD [+süre] maske sebep
	           SZLINE DEL maske
	           SZLINE LIST [maske] [NOEXPIRE]
	           SZLINE VIEW [maske] [NOEXPIRE]
	           SZLINE COUNT
	
	Bu komut ile SZLINE listesi düzenlenir. Bu listedeki bir kaleme
	uyan bir kullanici baglandiginda, bu baglanti KILL ile kesilecek,
	ayrica bazi server tiplerinde bu ban tüm serverlara
	bildirilecektir.
	
	ADD ile bir SZLINE eklenir, DEL ile silinir. LIST ile
	aktüel SZLINE listesi görüntülenir, VIEW ile ek birtakim bilgi
	görmek mümkündür. COUNT ile kac SZLINE oldugunu görmek mümkündür.
	
	Detayli bilgi icin SGLINE komutunu inceleyebilirsiniz.
	/msg %S HELP SGLINE.
	
	Bu komutu sadece Services operator'lar kullanabilir.

OPER_HELP_EXCEPTION
	Kullanimi: EXCEPTION ADD [+süre] mask limit gerekce
	           EXCEPTION DEL {mask | liste}
	           EXCEPTION MOVE num posizyon
	           EXCEPTION LIST [mask | liste]
	           EXCEPTION VIEW [mask | liste]
	
	Services operatorlerin kimi hostlar icin özel baglanti limitleri tanimlayabilmeleri
	icindir. Mesela shell server larin, normalden daha fazla baglantiyi
	kaldirabilmeleri gibi. Bir host limitine erismis ise, limiti asan her bir
	baglantiya kill komutu gönderilir ve de %S den bir NOTICE ile o hosttan
	baglanti limiti asimi oldugu belirtilir. Bu notice in icerigi config
	esnasinda belirtilir.
	
	EXCEPTION ADD ile SADECE hostmask lar bu listeye alinirlar.
	nick!user@host ve ya user@host masklari gecersizdir.
	Sadece box.host.dom ve ya *.host.dom kabul edilir.
	limit sifirdan büyük bir sayi olup, o hosttan izin verilen baglanti
	sayisini temsil eder. sifir demek, sinirsiz baglanti demektir.
	AKILL help kullanilarak süre tanimlamasi hakkinda bilgi alinabilir.
	
	EXCEPTION DEL belirtilen maske ya da numarayi listeden
	siler. Numaralar VIEW ya da LIST komutu ile
	görülebilirler.
	
	EXCEPTION MOVE ile belirtilen hostun listdeki pozisyonu
	num posizyonu ile degistirilir. Aradaki hostlar uygun sekilde
	kaydirilirlar.
	
	EXCEPTION LIST ve ya EXCEPTION VIEW ile aktüel
	baglanti limiti listesi görüntülenebilir, VIEW daha aciklamali bir bilgi vermek
	icindir.
	
	EXCEPTION ADD komutu ile beraber bir zaman asimi suresi de
	belirtilebilir.Bu sure d (gun), h (saat) veya m (dakika)
	seklinde olabilir.(1h30m) gibi kombinasyonlar gecerlidir.
	Varsayilan secenek gundur.Yani +30 30 gun anlamina gelir.Bir
	EXCEPTION'in otomatik olarak silinmemesi icin sure kisminda +0
	yazilmasi gerekir.
	
	Baglanan bir kullaniciya denk düsen ilk exception ayari onun icin aktif duruma
	gecirilir. Genis kapsamli exception listeleri performansa kötü etkide
	bulunabilirler.
	
	Bu komutu sadece Services operatorler kullanabilir.

OPER_HELP_SESSION
	Kullanimi: SESSION LIST threshold
	           SESSION VIEW host
	
	Services operatorlerin oturum listesini görmelerini saglar.
	
	SESSION LIST ile en az threshold baglantili hostlar
	görüntülenir. Flood sorunu olmamasi icin bu rakam en az 1 olmalidir.
	
	SESSION VIEW ile bir baglanti hakkinda detayli bilgi alinabilir.
	Aktif baglanti sayisi ve de limit gibi.
	
	host belirtirken asterisk kullanilamaz.
	
	EXCEPTION helpini kullanarak daha fazla bilgi alabilirsiniz.
	
	Bu komutu sadece Services operatorler kullanabilir.

OPER_HELP_SU
	Kullanimi: SU password
	
	Services Root komutlarina erisim hakki verir. SET SUPASS
	komutu ile belirtilen sifreyi ullanmak suretiyle bir
	services admin, services root yetkilerini alabilir.
	Services root yetkileri, nick degisene ve ya QUIT yapilana
	kadar devam edecektir.
	
	UYARI: Hatali bir sifre girildiginde, diger IRCOPlar
	uyarilacaktir.
	
	Bu komutu sadece Services admin'ler kullanabilir.

OPER_HELP_SET
	Kullanimi: SET secenek ayar
	
	Cesitli Services seceneklerini ayarlar.Mevcut secenekler:
	
	    READONLY   read-only ve read-write arasinda gecis
	    DEBUG      debug mode'u acar/kapar.
	    SUPASS     services adminlere root yetkisi vermek
	                     icin gereken sifreyi belirler.
	
	Bu komutu Services admin'ler kullanabilir. SUPASS opsiyonu ise
	sadece services root'a aittir.

OPER_HELP_SET_READONLY
	Kullanimi: SET READONLY {ON | OFF}
	
	read-only secenegini acar/kapar. Bu secenek aktifken normal
	kullanicilar Services veritabani uzerinde herhangi bir
	degisiklik yaratacak komutlari (nick ve kanal erisim
	listeleri gibi).Gerekli yetkilere sahip IRCop'lar AKILL
	listesini duzenleyebildikleri gibi nick ve kanallari
	yasaklayabilir (FORBID) ve/veya kayitlarini silebilirler
	(DROP).Ancak bu degisikliklerin hicbiri Services
	kapatilmadan veya yeniden baslatilmadan once read-only
	ozelliginin kapatilmamasi halinde kaydedilmeyecektir.
	
	Bu ozellik komut satirindaki -readonly ile aynidir.

OPER_HELP_SET_DEBUG
	Kullanimi: SET DEBUG {ON | OFF | num}
	
	Debug ozelligini acar/kapar.Bu ozellik aktifken Services'a
	gelen ve Services'den gonderilen her veri kaydedilir.Eger
	num belirtilmisse debug ozelligi bu seviyede baslatilir.
	
	Bu ozellik komut satirindaki -debug ile aynidir.

OPER_HELP_SET_SUPASS
	Kullanimi: SET SUPASS [sifre]
	
	SU komutu icin gereken sifreyi belirler. Bu sifre belirtilmeden
	SU komutu kullanilamaz. Eger sifre belirtilmemis ise
	SU komutu kullanilamayacaktir.
	
	Bu özellik sadece services root'a aittir.

OPER_HELP_JUPE
	Kullanimi: JUPE sunucu (server) [aciklama]
	
	Services bu komut kullanildiginda, belirtilen isimde bir
	sanal sunucuyu networke baglar, ve de bu isimdeki herhangi
	bir baska sunucunun baglanmasini engellemis olur. JUPE,
	normal bir /SQUIT komutu ile kaldirilabilir. Eger bir aciklama
	belirtilmis ise, bu bilgi kullanilir, yoksa, "Jupitered by <nick>"
	metini kullanilacaktir; JUPE komutunu kullanan operatörün nicki ile.
	
	Bu komutu sadece services adminler kullanabilir.

OPER_HELP_RAW
	Kullanimi: RAW metin
	
	Direk olarak Services'in bagli oldugu sunucuya bir metin
	gonderir. Bu komutun oldukca kisitli bir kullanim alani
	vardir ve ne yapildigindan emin olmadikca kesinlikle
	BU KOMUTU KULLANMAYIN
	
	Bu komutu sadece Services root kullanabilir.

OPER_HELP_UPDATE
	Kullanimi: UPDATE [FORCE]
	
	Services veritabanini belirtilen sureden once,yani siz
	komutu yazar yazmaz guncellenmesini saglar. FORCE parametresi
	verildigi takdirde, "kilitli veritabani" hatasina ragmen
	güncelleme islemi yapilabilir.
	
	Bu komutu sadece Services admin'ler kullanabilir.

OPER_HELP_QUIT
	Kullanimi: QUIT
	
	Services' kapatan bu komut kullanildiginda veritabani
	GUNCELLENMEZ. Bu yuzden veritabaninda veya Services'in
	calistigi makinede bir sorun ciktigi durumlar disinda
	kullanilmamalidir.
	
	Bu komutu sadece Services admin'ler kullanabilir.

OPER_HELP_SHUTDOWN
	Kullanimi: SHUTDOWN
	
	QUIT'in aksine bu komut kullanildiginda Services butun
	veritabanini guncelledikten sonra kapanir.
	
	Bu komutu sadece Services admin'ler kullanabilir.

OPER_HELP_RESTART
	Kullanimi: RESTART
	
	Services'i, butun veritabanini guncelledikten sonra yeniden
	baslatir.
	
	Bu komutu sadece Services admin'ler kullanabilir.
	
OPER_HELP_REHASH
	Kullanimi: REHASH
	
	Services in konfigürasyon dosyalarini yeniden okumasini saglar.
	Bunun sonucunda yeni ayarlar gecerli olur. Bazi ayarlari, mesela
	RemoteServer, REHASH kullanarak degistirmek mümkün degildir,
	bunlari degistirmek icin RESTART gereklidir. Bu durumlarda
	log dosyasina bu konuyla ilgili bir kayit yapilacak, ve de
	size de bir notice gönderilecektir.
	
	LoadModule satirlarinin ana services konfigürasyon dosyasi
	(%s)na eklenmesi durumunda, bu modüller eklenecekler,
	cikartilmasi durumunda da cikartilacaklardir. Cikarma islemi
	ekleme islemi ile ters sirada yapilacaktir. Herhangi bir
	hata durumunda, rehash islemi kesilecektir.
	
	Modüllerin birbirlerine bagli olmalari durumu yüzünden,
	bazi modülleri cikartmak mümkün olmayabilir. Böyle bir durumda
	restart komutunun kullanilmasi gerekebilir.
	
	Bu komutu sadece services adminler kullanabilir.

OPER_HELP_KILLCLONES
	Kullanimi: KILLCLONES nick
	
	Belirtilen nick ile ayni hostname'e sahip tüm kullanicilara
	KILL gönderir ve de *@host seklindeki bir gecici
	akill ile bu kullanicilarin hemen geri baglanmalarini da
	engeller. Bir WALLOPS/GLOBOPS ile komutu kimin kullandigi,
	kac kullaniciyi etkiledigi belirtilir. Klon temizlemek icin
	düsünülmüs bir komuttur.
	
	Bu komutu sadece Services operator'ler kullanabilir.


###########################################################################
#
# StatServ help messages
#
############################################################################

STAT_HELP
	%S Network hakkinda istatistik veren bir servistir.
	Bir komut hakkinda detayli bilgi icin, /msg %S HELP COMMANDS
	komutunu kullanabilirsiniz.
	Bir komut hakkinda daha fazla bilgi icin, /msg %S HELP command
	komutunu kullaniniz.
	
STAT_HELP_COMMANDS
	Asagidaki komutlar kullanilabilir.
	
	    SERVERS     Server istatistikleri
	    USERS       Kullanici istatistikleri

STAT_HELP_SERVERS
	Syntax: SERVERS STATS
	        SERVERS LIST [mask]
	        SERVERS VIEW [mask] [ONLINE | OFFLINE]
	
	Network ile ilgili istatistik bilgisi verir.
	
	SERVERS STATS su an görünen ve de görünmeyen (online ve de
	offline) serverlar hakkinda bilgi verir.
	
	SERVERS LIST online serverlari listeler ve de onlar hakkinda
	bir istatistik verir.
	
	SERVERS VIEW online serverlari listeler ve de onlar hakkinda
	detayli bir istatistik verir. ONLINE ve de OFFLINE
	parametreleri ile online ve ya offline serverlar hakkinda bilgi alinabilir.
	
	mask opsiyonu ile gösterilecek serverlar sinirlanabilir.

STAT_HELP_USERS
	Kullanimi: USERS STATS
	
	Networkdeki kullanicilar hakkinda bir istatistik verir.
	
STAT_OPER_HELP_SERVERS
	Kullanimi: SERVERS STATS
	           SERVERS LIST [mask]
	           SERVERS VIEW [mask] [ONLINE | OFFLINE]
	           SERVERS DELETE server
	           SERVERS COPY server yeniisim
	           SERVERS RENAME server yeniisim
	
	Network ile ilgili istatistik bilgisi verir.
	
	SERVERS STATS su an görünen ve de görünmeyen (online ve de
	offline) serverlar hakkinda bilgi verir.
	
	SERVERS LIST online serverlari listeler ve de onlar hakkinda
	bir istatistik verir.
	
	SERVERS VIEW online serverlari listeler ve de onlar hakkinda
	detayli bir istatistik verir. ONLINE ve de OFFLINE
	parametreleri ile online ve ya offline serverlar hakkinda bilgi alinabilir.
	
	mask opsiyonu ile gösterilecek serverlar sinirlanabilir.
	
	DELETE ile belirtilen server hakkindaki istatistik silinir.
	COPY belirtilen serverin istatistiklerini cogaltir.
	RENAME ile belirtilen server ismi degistirilir.
	Bu üc komut sadece Services admin'lere özeldir.

###########################################################################

# CVS: $Revision: 2.200 $
