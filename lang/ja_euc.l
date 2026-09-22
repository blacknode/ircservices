# Emacs: -*- mode: fundamental; coding: euc-jp; -*-

# Japanese language file (EUC encoding) / 日本語メッセージファイル
#
# IRC Services is copyright (c) 1996-2009 Andrew Church.
#     E-mail: <achurch@achurch.org>
# Parts written by Andrew Kempe and others.
# This program is free but copyrighted software; see the file GPL.txt for
# details.

###########################################################################
#
# 言語の名前と文字コード
#
###########################################################################

LANG_NAME
	日本語（EUCコード） (Japanese / EUC encoding)
LANG_CHARSET
	euc-jp

###########################################################################
#
# 多用メッセージ
#
###########################################################################

# strftime()日時フォーマット
STRFTIME_DATE_TIME_FORMAT
	%Y.%m.%d %H:%M:%S %Z
# strftime()年月日フォーマット（１）
STRFTIME_LONG_DATE_FORMAT
	%Y年%m月%d日（%a）
# strftime()年月日フォーマット（２）
STRFTIME_SHORT_DATE_FORMAT
	%Y.%m.%d

# strftime()フォーマットで使用される文字列
STRFTIME_DAYS_SHORT
	日
	月
	火
	水
	木
	金
	土
STRFTIME_DAYS_LONG
	日曜日
	月曜日
	火曜日
	水曜日
	木曜日
	金曜日
	土曜日
STRFTIME_MONTHS_SHORT
	1月
	2月
	3月
	4月
	5月
	6月
	7月
	8月
	9月
	10月
	11月
	12月
STRFTIME_MONTHS_LONG
	1月
	2月
	3月
	4月
	5月
	6月
	7月
	8月
	9月
	10月
	11月
	12月


# "day", "hour", "minute", and "second" in singular and plural.
STR_DAY
	日
STR_DAYS
	日
STR_HOUR
	時間
STR_HOURS
	時間
STR_MINUTE
	分
STR_MINUTES
	分
STR_SECOND
	秒
STR_SECONDS
	秒
# Separator used between units of time; in English, a comma followed by a
# space (the ", " in "3 hours, 10 minutes").
# 日本語では空文字列
STR_TIMESEP
	


# ChanServ、NickServ INFOで使われるリスト区切り文字
COMMA_SPACE
	、


# エラーメッセージ
INTERNAL_ERROR
	エラーが発生しました。
SERVICES_IS_BUSY
	通信が混雑しております。しばらくお待ちください。

UNKNOWN_COMMAND
	「%s」というコマンドは分かりません。
UNKNOWN_COMMAND_HELP
	「%s」というコマンドは分かりません。ヘルプを見る場合は「/msg %s HELP」を入力して下さい。
SYNTAX_ERROR
	用法：%s
MORE_INFO
	ヘルプ情報を見る場合は「/msg %s HELP %s」を入力して下さい。
NO_HELP_AVAILABLE
	「%s」に対するヘルプ情報はありません。
MISSING_QUOTE
	カッコ " (ダブルクォート) が抜けています。

BAD_EMAIL
	メールアドレスは、「ユーザ名@ホスト名」形式で入力して下さい。色付き文字や太字、斜字、反転文字、及び , : ; | \ " ( ) < > [ ] の各文字は使用できません。
REJECTED_EMAIL
	指定されたメールアドレスは使用できません。
BAD_URL
	URLは「http://ホスト名[:ポート番号]/...」形式で入力して下さい。(ftp://等も可能)
BAD_USERHOST_MASK
	マスクを「ユーザ名@ホスト名」形式で入力して下さい。
BAD_NICKUSERHOST_MASK
	マスクを「ニック!ユーザ名@ホスト名」形式で入力して下さい。
BAD_EXPIRY_TIME
	有効期限設定が無効です。

SENDMAIL_NO_RESOURCES
	現在メールを送信することができません。時間を置いて再度お試し下さい。
READ_ONLY_MODE
	注意：Servicesは読み取り専用モードで動作しています。変更は保存されません。
PASSWORD_INCORRECT
	パスワードが間違っています。
PASSWORD_WARNING
	警告：間違ったパスワードをあと１回入力すると切断されます。
ACCESS_DENIED
	アクセス不能。
PERMISSION_DENIED
	アクセス不能。

MORE_OBSCURE_PASSWORD
	もっと複雑なパスワードにして下さい。パスワードは、お名前・ニック等のような分かりやすいものでなく、５文字以上のものにして下さい。

NICK_NOT_REGISTERED
	ニックは登録されていません。
NICK_NOT_REGISTERED_HELP
	ニックは登録されていません。登録方法については「/msg %s HELP」を入力して下さい。
NICK_TOO_LONG
	ニックは%d文字以下でなければなりません。
NICK_INVALID
	「%s」はニックとして無効です。
NICK_X_NOT_REGISTERED
	ニック「%s」は登録されていません。
NICK_X_ALREADY_REGISTERED
	ニック「%s」はすでに登録されています。
NICK_X_NOT_IN_USE
	ニック「%s」は現在使用されていません。
NICK_X_FORBIDDEN
	ニック「%s」は使用禁止です。
NICK_X_SUSPENDED
	ニック「%s」は使用停止のため、使用できません。
NICK_X_SUSPENDED_MEMOS
	ニック「%s」は使用停止のため、メモを受信できません。
NICK_IDENTIFY_REQUIRED
	そのコマンドを使うにはパスワード認証を行う必要があります。
	「/msg %s IDENTIFY パスワード」を入力して下さい。
NICK_PLEASE_AUTH
	あなたのニックは、メールアドレス確認が済んでいません。「%s」へ送信された確認用コードでアドレス確認を行ってください。
NICK_X_NOT_ON_CHAN_X
	ニック「%s」はチャンネル「%s」に入っていません。

CHAN_INVALID
	「%s」はチャンネル名として無効です。
CHAN_X_NOT_REGISTERED
	チャンネル「%s」は登録されていません。
CHAN_X_NOT_IN_USE
	チャンネル「%s」は現在使用されていません。
CHAN_X_FORBIDDEN
	チャンネル「%s」は使用禁止です。
CHAN_X_SUSPENDED
	チャンネル「%s」は使用停止のため、使用できません。
CHAN_X_SUSPENDED_MEMOS
	チャンネル「%s」は使用停止のため、メモを受信できません。
CHAN_IDENTIFY_REQUIRED
	そのコマンドを使うにはパスワード認証を行う必要があります。
	「/msg %s IDENTIFY %s パスワード」を入力して下さい。

SERV_X_NOT_FOUND
	サーバ「%s」は見つかりません。

EXPIRES_NONE
	有効期限無し
EXPIRES_NOW
	有効期限切れ
EXPIRES_IN
	%s後無効化

LIST_RESULTS
	検索終了 (%d/%d項目を表示)。

###########################################################################
#
# NickServ用メッセージ
#
###########################################################################

# 自動メッセージ
NICK_IS_REGISTERED
	このニックは登録されています。他のニックに変えて下さい。（所有者なら、「/msg %s IDENTIFY パスワード」を入力して下さい。）
NICK_IS_SECURE
	このニックは登録済で保護されています。所有者なら、「/msg %s IDENTIFY パスワード」を入力して下さい。所有者でなければ、他のニックに変えて下さい。
NICK_MAY_NOT_BE_USED
	このニックはお使いになれません。
DISCONNECT_IN_1_MINUTE
	1分以内にニックを変えなければ、切断されます。
DISCONNECT_IN_20_SECONDS
	20秒以内にニックを変えなければ、切断されます。
DISCONNECT_NOW
	このニックは登録されていますので、お使いになれません。再接続する前に他のニックに変更して下さい。
FORCENICKCHANGE_IN_1_MINUTE
	1分以内にニックを変えなければ、ニックが自動的に変更されます。
FORCENICKCHANGE_IN_20_SECONDS
	20秒以内にニックを変えなければ、ニックが自動的に変更されます。
FORCENICKCHANGE_NOW
	このニックは登録されていますので、お使いになれません。あなたのニックは「%s」に変更されました。
NICK_EXPIRES_SOON
	注意：このニックは%s後、登録が解除されます。引き続き利用する場合は、「/msg %s IDENTIFY パスワード」でパスワード認証を行って下さい。
NICK_EXPIRED
	注意：このニックの登録は解除されました。引き続き利用する場合は、再度登録して下さい。

# REGISTER応答
NICK_REGISTER_SYNTAX
	REGISTER パスワード [メールアドレス]
NICK_REGISTER_REQ_EMAIL_SYNTAX
	REGISTER パスワード メールアドレス
NICK_REGISTRATION_DISABLED
	ニックの登録は現在出来ません。
NICK_REGISTRATION_FAILED
	登録できませんでした。
NICK_REG_PLEASE_WAIT
	REGISTERコマンドを再度実行する前に、%sお待ち下さい。
NICK_REG_PLEASE_WAIT_FIRST
	REGISTERコマンドを使用する前に、%sお待ち下さい。
NICK_CANNOT_BE_REGISTERED
	ニック「%s」は使用禁止のため、登録できません。
NICK_REGISTER_EMAIL_UNAUTHED
	指定されたメールアドレスは、まだアドレス確認が行われていません。新しいニックを登録する前にアドレス確認を行ってください。
NICK_REGISTER_TOO_MANY_NICKS
	すでに%d個のニックを登録しています (上限%d)。現在登録しているニックのいくつかを解除しない限り、新たにニックを登録することは出来ません。
NICK_REGISTERED
	ニック「%s」を登録しました。
NICK_PASSWORD_IS
	ニックのパスワードは %s です。忘れないようにして下さい。

# IDENTIFY応答
NICK_IDENTIFY_SYNTAX
	IDENTIFY パスワード
NICK_IDENTIFY_FAILED
	IDENTIFYは失敗しました。
NICK_IDENTIFY_EMAIL_MISSING
	ニックを利用する前に、メールアドレスを設定する必要があります。「/msg %s SET EMAIL アドレス」で設定して下さい。
NICK_IDENTIFY_SUCCEEDED
	認証が成功しました。

# DROP応答
NICK_DROP_SYNTAX
	DROP パスワード
NICK_DROP_WARNING
	注意：DROPコマンドはリンクされたニックの登録を全て解除しますので、ご注意下さい。
NICK_DROP_DISABLED
	ニック登録の解除は現在出来ません。
NICK_DROP_FAILED
	ニック登録の解除が失敗しました。
NICK_DROPPED
	ニックの登録を解除しました。
NICK_DROPPED_LINKS
	現在のニック及び全てのリンクされたニックの登録を解除しました。

# DROPNICK responses
NICK_DROPNICK_SYNTAX
	DROPNICK ニック
NICK_X_DROPPED
	ニック「%s」の登録を解除しました。
NICK_X_DROPPED_LINKS
	ニック「%s」及び全てのリンクされたニックの登録を解除しました。

# DROPEMAIL、DROPEMAIL-CONFIRM応答
NICK_DROPEMAIL_SYNTAX
	DROPEMAIL パターン
NICK_DROPEMAIL_PATTERN_TOO_LONG
	パターンが長すぎます（最大%dバイト）。
NICK_DROPEMAIL_NONE
	指定されたパターンに一致するニックはありません。
NICK_DROPEMAIL_COUNT
	指定されたパターンに一致するニックが%d個あります。
	削除を実行するには、「/msg %s DROPEMAIL-CONFIRM %s」を入力してください。
NICK_DROPEMAIL_CONFIRM_SYNTAX
	DROPEMAIL-CONFIRM パターン
NICK_DROPEMAIL_CONFIRM_UNKNOWN
	指定されたパターンは記憶されていません。DROPEMAIL-CONFIRMの前に、再度、このパターンでDROPEMAILを実行してください。
NICK_DROPEMAIL_CONFIRM_DROPPING
	ニックの登録解除を開始します。
NICK_DROPEMAIL_CONFIRM_DROPPED
	ニックの登録解除が完了しました。

# SET応答
NICK_SET_SYNTAX
	SET 設定名 値
NICK_SET_OPER_SYNTAX
	SET [!ニック] 設定名 値
NICK_SET_DISABLED
	ニックの設定変更は現在出来ません。
NICK_SET_UNKNOWN_OPTION
	「%s」という設定はありません。

# SET PASSWORD応答
NICK_SET_PASSWORD_FAILED
	パスワードの変更は出来ませんでした。
NICK_SET_PASSWORD_CHANGED
	パスワードを変更しました。
NICK_SET_PASSWORD_CHANGED_TO
	パスワードを %s に変更しました。

# SET LANGUAGE応答
NICK_SET_LANGUAGE_SYNTAX
	SET LANGUAGE 番号
NICK_SET_LANGUAGE_UNKNOWN
	%d番の言語は存在しません。言語一覧を見る場合、「msg %s HELP SET LANGUAGE」を入力して下さい。
NICK_SET_LANGUAGE_CHANGED
	言語を%sに変更しました。

# SET URL応答
NICK_SET_URL_CHANGED
	ニック「%s」のホームページアドレスを「%s」に変更しました。

# SET EMAIL応答
NICK_SET_EMAIL_PLEASE_WAIT
	SET EMAILコマンドを再度実行する前に、%sお待ち下さい。
NICK_SET_EMAIL_UNAUTHED
	指定されたメールアドレスは、まだアドレス確認が行われていません。登録する前ににアドレス確認を行ってください。
NICK_SET_EMAIL_TOO_MANY_NICKS
	「%s」は、すでに%d個のニックを登録しています (上限%d)。メールアドレス変更が出来ませんでした。
NICK_SET_EMAIL_CHANGED
	ニック「%s」のメールアドレスを「%s」に変更しました。

# SET INFO応答
NICK_SET_INFO_CHANGED
	ニック「%s」の情報を変更しました。

# SET KILL応答
NICK_SET_KILL_SYNTAX
	SET KILL {ON | QUICK | OFF}
NICK_SET_KILL_IMMED_SYNTAX
	SET KILL {ON | QUICK | IMMED | OFF}
NICK_SET_KILL_ON
	ニック保護設定を有効に設定しました。
NICK_SET_KILL_QUICK
	ニック保護設定を有効（ニック変更期限は20秒）に設定しました。
NICK_SET_KILL_IMMED
	ニック保護設定を有効（即時切断）に設定しました。
NICK_SET_KILL_IMMED_DISABLED
	このネットワークでは、IMMEDを選択できません。
NICK_SET_KILL_OFF
	ニック保護設定を無効に設定しました。

# SET SECURE応答
NICK_SET_SECURE_SYNTAX
	SET SECURE {ON | OFF}
NICK_SET_SECURE_ON
	セキュア設定を有効に設定しました。
NICK_SET_SECURE_OFF
	セキュア設定を無効に設定しました。

# SET PRIVATE応答
NICK_SET_PRIVATE_SYNTAX
	SET PRIVATE {ON | OFF}
NICK_SET_PRIVATE_ON
	プライベート設定を有効に設定しました。
NICK_SET_PRIVATE_OFF
	プライベート設定を無効に設定しました。

# SET NOOP応答
NICK_SET_NOOP_SYNTAX
	SET NOOP {ON | OFF}
NICK_SET_NOOP_ON
	チャンネルアクセス拒否設定を有効にしました。
NICK_SET_NOOP_OFF
	チャンネルアクセス拒否設定を無効にしました。

# SET HIDE応答
NICK_SET_HIDE_SYNTAX
	SET HIDE {EMAIL | USERMASK | QUIT} {ON | OFF}
NICK_SET_HIDE_EMAIL_ON
	メールアドレスは%sのINFO応答で表示されません。
NICK_SET_HIDE_EMAIL_OFF
	メールアドレスは%sのINFO応答で表示されます。
NICK_SET_HIDE_MASK_ON
	前回のユーザアドレスは%sのINFO応答で表示されません。
NICK_SET_HIDE_MASK_OFF
	前回のユーザアドレスは%sのINFO応答で表示されます。
NICK_SET_HIDE_QUIT_ON
	前回の切断メッセージは%sのINFO応答で表示されません。
NICK_SET_HIDE_QUIT_OFF
	前回の切断メッセージは%sのINFO応答で表示されます。

# SET TIMEZONE応答
NICK_SET_TIMEZONE_SYNTAX
	SET TIMEZONE {UTCオフセット | 時間帯名 | DEFAULT}
NICK_SET_TIMEZONE_TO
	時間帯設定はUTC%c%d:%02dに設定しました。
	この時間帯での現在時刻は%sです。
NICK_SET_TIMEZONE_DEFAULT
	時間帯設定を既定値に戻しました。

# SET MAINNICK応答
NICK_SET_MAINNICK_NOT_FOUND
	ニック「%s」はあなたのニックにリンクされていません。
NICK_SET_MAINNICK_CHANGED
	メインニックを「%s」に設定しました。

# SET NOEXPIRE応答
NICK_SET_NOEXPIRE_SYNTAX
	SET [!ニック] NOEXPIRE {ON | OFF}
NICK_SET_NOEXPIRE_ON
	ニック「%s」の登録は自動解除しません。
NICK_SET_NOEXPIRE_OFF
	ニック「%s」の登録は自動解除します。

# UNSET応答
NICK_UNSET_SYNTAX
	UNSET {URL | EMAIL | INFO}
NICK_UNSET_SYNTAX_REQ_EMAIL
	UNSET {URL | INFO}
NICK_UNSET_OPER_SYNTAX
	UNSET [!ニック] {URL | EMAIL | INFO}
NICK_UNSET_OPER_SYNTAX_REQ_EMAIL
	UNSET [!ニック] {URL | INFO}
NICK_UNSET_URL
	ニック「%s」のホームページアドレスを消去しました。
NICK_UNSET_EMAIL
	ニック「%s」のメールアドレスを消去しました。
NICK_UNSET_INFO
	ニック「%s」の情報を消去しました。
NICK_UNSET_EMAIL_BAD
	メールアドレスを消去できません。
NICK_UNSET_EMAIL_OTHER_BAD
	ニックのメールアドレスを消去できません。

# ACCESS応答
NICK_ACCESS_SYNTAX
	ACCESS {ADD | DEL | LIST} [マスク]
NICK_ACCESS_DISABLED
	ニックの利用許可リストの編集は現在出来ません。
NICK_ACCESS_REACHED_LIMIT
	利用許可リストがいっぱいです。
NICK_ACCESS_ALREADY_PRESENT
	「%s」はすでに利用許可リストに登録されています。
NICK_ACCESS_NO_NICKS
	注意：利用許可マスクには、ニックを使用できません。マスクが「ユーザ@ホスト名」形式で正しく入力されているか、確認してください。
NICK_ACCESS_ADDED
	「%s」を利用許可リストに追加しました。
NICK_ACCESS_NOT_FOUND
	「%s」は利用許可リストに登録されていません。
NICK_ACCESS_DELETED
	「%s」を利用許可リストから削除しました。
NICK_ACCESS_LIST_EMPTY
	利用許可リストは空です。
NICK_ACCESS_LIST
	利用許可リスト：
NICK_ACCESS_LIST_X_EMPTY
	「%s」の利用許可リストは空です。
NICK_ACCESS_LIST_X
	「%s」の利用許可リスト：

# LINK応答
NICK_OLD_LINK_SYNTAX
	LINK ニック パスワード
NICK_LINK_SYNTAX
	LINK ニック
NICK_LINK_DISABLED
	ニックのリンクは現在出来ません。
NICK_LINK_FAILED
	リンクは出来ませんでした。
NICK_CANNOT_BE_LINKED
	ニック「%s」をリンクすることはできません。
NICK_OLD_LINK_SAME
	同じニックにリンクできません。
NICK_LINK_SAME
	同じニックにリンクできません。
NICK_LINK_ALREADY_LINKED
	ニック「%s」はすでにリンクされています。
NICK_LINK_IN_USE
	ニック「%s」は使用中です。
NICK_LINK_TOO_MANY
	同じニックに%d個のニックしかリンクできません。
NICK_LINK_TOO_MANY_NICKS
	すでに%d個のニックを登録しています (上限%d)。リンクできません。
NICK_OLD_LINK_TOO_MANY_CHANNELS
	ニックの合計登録チャンネル数は上限を越えるため、リンクできません。
NICK_OLD_LINKED
	現在のニックをニック「%s」にリンクしました。
NICK_LINKED
	ニック「%s」を現在のニックにリンクしました。

# UNLINK応答
NICK_OLD_UNLINK_SYNTAX
	UNLINK [ニック パスワード]
NICK_UNLINK_SYNTAX
	UNLINK ニック
NICK_UNLINK_OPER_SYNTAX
	UNLINK ニック [FORCE]
NICK_UNLINK_DISABLED
	ニックのリンク解除は現在出来ません。
NICK_UNLINK_FAILED
	リンク解除は出来ませんでした。
NICK_UNLINK_SAME
	現在のニックをアンリンクできません。
NICK_OLD_UNLINK_NOT_LINKED
	現在のニックはリンクされていません。
NICK_UNLINK_NOT_LINKED_YOURS
	ニック「%s」はあなたのニックにリンクされていません。
NICK_UNLINK_NOT_LINKED
	ニック「%s」はリンクされていません。
NICK_OLD_UNLINKED
	現在のニックを「%s」からアンリンクしました。
NICK_UNLINKED
	ニック「%s」をアンリンクしました。
NICK_X_UNLINKED
	ニック「%s」を「%s」からアンリンクしました。

# LISTLINKS応答
NICK_OLD_LISTLINKS_SYNTAX
	LISTLINKS ニック
NICK_LISTLINKS_SYNTAX
	LISTLINKS
NICK_LISTLINKS_HEADER
	「%s」にリンクされているニック：
NICK_LISTLINKS_FOOTER
	%d個見つかりました。

# INFO応答
NICK_INFO_SYNTAX
	INFO ニック [ALL]
NICK_INFO_REALNAME
	ニック「%s」所有者：%s
NICK_INFO_ADDRESS
	前回接続のアドレス：%s
NICK_INFO_ADDRESS_ONLINE
	　接続中のアドレス：%s
NICK_INFO_ADDRESS_ONLINE_NOHOST
	このユーザは現在オンラインです。
NICK_INFO_ADDRESS_OTHER_NICK
	このユーザは現在、ニック「%s」を使用しています。
NICK_INFO_TIME_REGGED
	　　　　　登録日時：%s
NICK_INFO_LAST_SEEN
	　　　前回使用日時：%s
NICK_INFO_LAST_QUIT
	前回切断メッセージ：%s
NICK_INFO_URL
	　　　ホームページ：%s
NICK_INFO_EMAIL
	　　メールアドレス：%s
NICK_INFO_EMAIL_UNAUTHED
	　　メールアドレス：%s（未確認）
NICK_INFO_INFO
	　　　　　　　情報：%s
NICK_INFO_OPTIONS
	　　オプション設定：%s
# These strings MUST NOT be empty
NICK_INFO_OPT_KILL
	ニック保護
NICK_INFO_OPT_SECURE
	セキュア
NICK_INFO_OPT_PRIVATE
	プライベート
NICK_INFO_OPT_NOOP
	チャンネルアクセス拒否
NICK_INFO_OPT_NONE
	無し
NICK_INFO_NO_EXPIRE
	このニックの登録は自動解除されません。
NICK_INFO_SUSPEND_DETAILS
	このニックは%sにより使用停止中です。
	停止日時：%s（%s）
NICK_INFO_SUSPEND_REASON
	停止理由：%s
NICK_INFO_SHOW_ALL
	詳細情報を見る場合は、「/msg %s INFO %s ALL」を入力して下さい。

# LISTCHANS応答
NICK_LISTCHANS_SYNTAX
	LISTCHANS
NICK_LISTCHANS_NONE
	ニック「%s」はチャンネルを登録していません。
NICK_LISTCHANS_HEADER
	ニック「%s」の登録チャンネル：
NICK_LISTCHANS_END
	%dチャンネル登録されています。

# LIST/LISTEMAIL応答
NICK_LIST_SYNTAX
	%s [+開始位置] パターン
NICK_LIST_OPER_SYNTAX
	%s [+開始位置] パターン [FORBIDDEN] [NOEXPIRE] [SUSPENDED]
NICK_LIST_OPER_SYNTAX_AUTH
	%s [+開始位置] パターン [FORBIDDEN] [NOEXPIRE] [SUSPENDED] [NOAUTH]
NICK_LIST_HEADER
	%s に一致するニック：
NICK_LIST_NO_MATCH
	一致するニックはありません。
NICK_LISTEMAIL_NONE
	（未設定）

# RECOVER応答
NICK_RECOVER_SYNTAX
	RECOVER ニック [パスワード]
NICK_NO_RECOVER_SELF
	現在のニックに対して使用できません。
NICK_RECOVERED
	ニックを使用していたユーザを切断しました。１分間の保留タイムアウトが経つ前にニックを使用したい場合は、「/msg %s RELEASE %s パスワード」を入力して下さい。

# RELEASE応答
NICK_RELEASE_SYNTAX
	RELEASE ニック [パスワード]
NICK_RELEASE_NOT_HELD
	ニック「%s」は保留されていません。
NICK_RELEASED
	ニック「%s」を解放しました。

# GHOST応答
NICK_GHOST_SYNTAX
	GHOST ニック [パスワード]
NICK_NO_GHOST_SELF
	現在のニックに対して使用できません。
NICK_GHOST_KILLED
	ニック「%s」を切断しました。

# GETPASS応答
NICK_GETPASS_SYNTAX
	GETPASS ニック
NICK_GETPASS_UNAVAILABLE
	ニック「%s」のパスワードは暗号化されているため、GETPASSを使用できません。
NICK_GETPASS_PASSWORD_IS
	ニック「%s」のパスワードは %s です。

# FORBID応答
NICK_FORBID_SYNTAX
	FORBID ニック
NICK_FORBID_SUCCEEDED
	ニック「%s」を使用禁止にしました。
NICK_FORBID_FAILED
	ニック「%s」を使用禁止にすることができませんでした。

# SUSPEND responses
NICK_SUSPEND_SYNTAX
	SUSPEND [+期限] ニック 理由
NICK_SUSPEND_SUCCEEDED
	ニック「%s」及びそれにリンクされている全てのニックを使用停止にしました。
NICK_SUSPEND_ALREADY_SUSPENDED
	ニック「%s」はすでに使用停止になっています。

# UNSUSPEND responses
NICK_UNSUSPEND_SYNTAX
	UNSUSPEND ニック
NICK_UNSUSPEND_SUCCEEDED
	ニック「%s」の使用停止を解除しました。
NICK_UNSUSPEND_NOT_SUSPENDED
	ニック「%s」は使用停止になっていません。

################ autojoinモジュール用メッセージ

# AJOIN応答
NICK_AJOIN_SYNTAX
	AJOIN {ADD | DEL | LIST} [チャンネル]
NICK_AJOIN_ADD_SYNTAX
	AJOIN ADD チャンネル
NICK_AJOIN_DEL_SYNTAX
	AJOIN DEL チャンネル
NICK_AJOIN_DISABLED
	ニックの自動参加リストの編集は現在出来ません。
NICK_AJOIN_ALREADY_PRESENT
	チャンネル「%s」はすでに自動参加リストに登録されています。
NICK_AJOIN_LIST_FULL
	自動参加リストがいっぱいです。
NICK_AJOIN_ADDED
	チャンネル「%s」を自動参加リストに追加しました。
NICK_AJOIN_NOT_FOUND
	チャンネル「%s」は自動参加リストに登録されていません。
NICK_AJOIN_DELETED
	チャンネル「%s」を自動参加リストから削除しました。
NICK_AJOIN_LIST_EMPTY
	自動参加リストは空です。
NICK_AJOIN_LIST
	自動参加リスト：
NICK_AJOIN_LIST_X_EMPTY
	ニック「%s」の自動参加リストは空です。
NICK_AJOIN_LIST_X
	ニック「%s」の自動参加リスト：
NICK_AJOIN_AUTO_REMOVE
	「%s」はチャンネル名として無効なため、自動参加リストから削除されました。

################ mail-authモジュール用メッセージ

# 全般メッセージ
NICK_AUTH_SENDING
	メールアドレス確認コードを%sに送信します。
NICK_AUTH_FOR_REGISTER
	メッセージを受信したら、「/msg %s AUTH コード」(コードのところで実際のコードを入力します) を入力して登録を完了させて下さい。
NICK_AUTH_FOR_SET_EMAIL
	メッセージを受信したら、「/msg %s AUTH コード」(コードのところで実際のコードを入力します) を入力してアドレス変更を完了させて下さい。
	注意：変更が完了するまで、ニックを利用することができません。
NICK_AUTH_NOT_NEEDED
	アドレス確認の必要はありません。
NICK_AUTH_NO_AUTHCODE
	ニック「%s」は確認コードが設定されていません。
NICK_AUTH_HAS_AUTHCODE
	ニック「%s」は確認コードがすでに設定されています。
PASSWORD_WARNING_FOR_AUTH
	警告：間違った確認コードをあと１回入力すると切断されます

# メール用テキスト。user@host前の最後の「%s」はNICK_AUTH_MAIL_TEXT_*
# メッセージのいずれかが入る。
NICK_AUTH_MAIL_SUBJECT
	「%s」のメールアドレス確認コード
# awkward wording, rewrite when we can rearrange printf params
NICK_AUTH_MAIL_BODY
	このメールはIRC Servicesから自動送信されたメールです。
	
	ニック「%s」のメールアドレス確認コードは %09d です。
	このコードを下記コマンドで%sに送信して下さい。
	    /msg %s AUTH %09d
	
	このメールは、%sからの%s（%s@%sによる）
	のために送信されました。
NICK_AUTH_MAIL_TEXT_REG
	ニック登録
NICK_AUTH_MAIL_TEXT_EMAIL
	メールアドレス変更
NICK_AUTH_MAIL_TEXT_SENDAUTH
	SENDAUTHコマンド使用
NICK_AUTH_MAIL_TEXT_REAUTH
	REAUTHコマンド使用
NICK_AUTH_MAIL_BODY_SETAUTH
	このメールはIRC Servicesから自動送信されたメールです。
	
	ニック「%s」のメールアドレス確認コードは %09d です。
	このコードを下記コマンドで%sに送信して下さい。
	    /msg %s AUTH %09d

# AUTH応答
NICK_AUTH_SYNTAX
	AUTH コード
NICK_AUTH_DISABLED
	メールアドレス確認は現在出来ません。
NICK_AUTH_FAILED
	確認コードが間違っています。
NICK_AUTH_SUCCEEDED_REGISTER
	メールアドレス確認が成功しました。ニック登録はこれで完了です。
NICK_AUTH_SUCCEEDED_SET_EMAIL
	メールアドレス確認が成功しました。メールアドレス変更はこれで完了です。
NICK_AUTH_SUCCEEDED_REAUTH
	メールアドレス確認によるニック認証が成功しました。
NICK_AUTH_SUCCEEDED_SETAUTH
	メールアドレス確認が成功しました。引き続きニックをご利用できます。

# SENDAUTH応答
NICK_SENDAUTH_SYNTAX
	SENDAUTH
NICK_SENDAUTH_TOO_SOON
	SENDAUTHコマンドを再度実行する前に、%sお待ち下さい。
NICK_SENDAUTH_NORESOURCES
	システムが混雑しているため、メールアドレス確認コードを送信できませんでした。しばらく時間を置いてもう一度試してみるか、ネットワーク管理者にお問い合わせください。
NICK_SENDAUTH_ERROR
	システムの不具合のため、メールアドレス確認コードを送信できませんでした。しばらく時間を置いてもう一度試してみるか、ネットワーク管理者にお問い合わせください。

# REAUTH応答
NICK_REAUTH_SYNTAX
	REAUTH
NICK_REAUTH_HAVE_AUTHCODE
	確認コードがすでに設定されています。
NICK_REAUTH_NO_EMAIL
	メールアドレスが設定されていません。
NICK_REAUTH_AUTHCODE_SET
	新しい確認コードが設定され、メール送信されました。そのコードをAUTHコマンドで使って、認証を行ってください。

# RESTOREMAIL応答
NICK_RESTOREMAIL_SYNTAX
	RESTOREMAIL パスワード
NICK_RESTOREMAIL_NOT_NOW
	RESTOREMAILは、メールアドレスを変更した後、確認コードを入力するまでしか使えません。
NICK_RESTOREMAIL_DONE
	前のメールアドレス「%s」を復元しました。

# SETAUTH応答
NICK_SETAUTH_SYNTAX
	SETAUTH ニック
NICK_SETAUTH_NO_EMAIL
	ニック「%s」は、メールアドレスが設定されていません。
NICK_SETAUTH_AUTHCODE_SET
	新しい確認コード (%09d) をニック「%s」に設定しました。
NICK_SETAUTH_SEND_REFUSED
	警告：メールサーバがメールを拒否したため、「%s」にメールを送信できませんでした。
NICK_SETAUTH_SEND_TIMEOUT
	警告：メールサーバ接続中または送受信中にタイムアウトが発生したため、「%s」にメールを送信できませんでした。
NICK_SETAUTH_SEND_NORESOURCES
	警告：リソース不足のため、「%s」にメールを送信できませんでした。
NICK_SETAUTH_SEND_ERROR
	警告：不明なエラーのため「%s」にメールを送信できませんでした。
NICK_SETAUTH_USER_NOTICE
	ニックを引き続き利用するために、メールアドレスを確認する必要があります。確認コードを登録されているメールアドレス (%s) に送信します。
	詳しくは、「/msg %s HELP AUTH」を入力して下さい。

# GETAUTH応答
NICK_GETAUTH_SYNTAX
	GETAUTH ニック
NICK_GETAUTH_AUTHCODE_IS
	ニック「%s」のメールアドレス確認コードは %09d です。

# CLEARAUTH応答
NICK_CLEARAUTH_SYNTAX
	CLEARAUTH ニック
NICK_CLEARAUTH_CLEARED
	ニック「%s」のメールアドレス確認コードをクリアしました。

###########################################################################
#
# ChanServ messages
#
###########################################################################

# アクセスレベルの説明文
CHAN_LEVEL_AUTOOP
	チャンネル参加時にオペレータ権（+o）付与
CHAN_LEVEL_AUTOVOICE
	チャンネル参加時に発言権（+v）付与
CHAN_LEVEL_INVITE
	INVITEコマンド使用許可
CHAN_LEVEL_AKICK
	AKICKコマンド使用許可
CHAN_LEVEL_SET
	SET（FOUNDER、PASSWORD以外）使用許可
CHAN_LEVEL_CLEAR
	CLEARコマンド使用許可
CHAN_LEVEL_UNBAN
	UNBANコマンド使用許可
CHAN_LEVEL_OPDEOP
	OP、DEOPコマンド使用許可
CHAN_LEVEL_ACCESS_LIST
	アクセス一覧を表示許可
CHAN_LEVEL_ACCESS_CHANGE
	アクセス一覧を変更許可
CHAN_LEVEL_MEMO
	チャンネルメモ受信
CHAN_LEVEL_VOICE
	VOICE、DEVOICEコマンド使用許可
CHAN_LEVEL_AUTOHALFOP
	チャンネル参加時に半オペレータ権（+h）付与
CHAN_LEVEL_HALFOP
	HALFOP、DEHALFOPコマンド使用許可
CHAN_LEVEL_AUTOPROTECT
	チャンネル参加時にプロテクト状態（+a）
CHAN_LEVEL_PROTECT
	PROTECT、DEPROTECTコマンド使用許可
CHAN_LEVEL_KICK
	KICKコマンド使用許可
CHAN_LEVEL_STATUS
	STATUSコマンド使用許可
CHAN_LEVEL_TOPIC
	TOPICコマンド使用許可

# 自動メッセージ
CHAN_IS_REGISTERED
	このチャンネルは登録されています。
CHAN_MAY_NOT_BE_USED
	このチャンネルは使用禁止です。
CHAN_NOT_ALLOWED_TO_JOIN
	このチャンネルには入れません。

# 多用応答
CHAN_BOUNCY_MODES
	そのコマンドは現在使えません。

# REGISTER応答
CHAN_REGISTER_SYNTAX
	REGISTER チャンネル パスワード 説明
CHAN_REGISTER_DISABLED
	チャンネルの登録は現在出来ません。
CHAN_REGISTER_SHORT_CHANNEL
	チャンネル「#」は登録できません。
CHAN_REGISTER_NOT_LOCAL
	ローカルチャンネルの登録は出来ません。
CHAN_REGISTER_INVALID_NAME
	登録できるチャンネルは、名前が「#」で始まるチャンネルのみです。
CHAN_MUST_REGISTER_NICK
	チャンネルを登録するには、まずニックを登録する必要があります。ニック登録情報を見るには「/msg %s HELP」を入力して下さい。
CHAN_MUST_IDENTIFY_NICK
	チャンネルを登録するには、ニックのパスワード認証を行う必要があります。「/msg %s IDENTIFY パスワード」を入力して下さい。
CHAN_MAY_NOT_BE_REGISTERED
	チャンネル「%s」は使用禁止のため、登録できません。
CHAN_ALREADY_REGISTERED
	チャンネル「%s」はすでに登録されています。
CHAN_MUST_BE_CHANOP
	チャンネルを登録するには、そのチャンネルのオペレータ権（+oモード）を持っていなければなりません。
CHAN_REACHED_CHANNEL_LIMIT
	チャンネル登録数の上限（%d）に達していますので登録できません。
CHAN_EXCEEDED_CHANNEL_LIMIT
	チャンネル登録数の上限（%d）を超えていますので登録できません。
CHAN_REGISTRATION_FAILED
	登録は失敗しました。
CHAN_REGISTERED
	チャンネル「%s」を登録しました。
CHAN_PASSWORD_IS
	チャンネルのパスワードは %s です。忘れないようにして下さい。

# IDENTIFY応答
CHAN_IDENTIFY_SYNTAX
	IDENTIFY チャンネル パスワード
CHAN_IDENTIFY_FAILED
	IDENTIFYは失敗しました。
CHAN_IDENTIFY_SUCCEEDED
	チャンネル認証が成功しました。

# DROP応答
CHAN_DROP_SYNTAX
	DROP チャンネル パスワード
CHAN_DROP_DISABLED
	チャンネル登録の解除は現在出来ません。
CHAN_DROPPED
	チャンネル「%s」の登録を解除しました。

# DROPCHAN応答
CHAN_DROPCHAN_SYNTAX
	DROPCHAN チャンネル

# SET応答
CHAN_SET_SYNTAX
	SET チャンネル 設定名 値
CHAN_SET_DISABLED
	チャンネルの設定変更は現在出来ません。
CHAN_SET_UNKNOWN_OPTION
	「%s」という設定はありません。

# SET FOUNDER応答
CHAN_SET_FOUNDER_TOO_MANY_CHANS
	ニック「%s」はチャンネル登録数の上限を超えているため、創設者に設定できません。
CHAN_FOUNDER_CHANGED
	チャンネル「%s」の創設者を%sに変更しました。

# SET SUCCESSOR応答
CHAN_SUCCESSOR_CHANGED
	チャンネル「%s」の後継者を%sに変更しました。
CHAN_SUCCESSOR_IS_FOUNDER
	ファウンダと後継者両方に同じニックを設定することはできません。

# SET PASSWORD応答
CHAN_SET_PASSWORD_FAILED
	パスワードの変更は失敗しました。
CHAN_PASSWORD_CHANGED
	チャンネル「%s」のパスワードを変更しました。
CHAN_PASSWORD_CHANGED_TO
	チャンネル「%s」のパスワードを %s に変更しました。

# SET DESC応答
CHAN_DESC_CHANGED
	チャンネル「%s」の説明を「%s」に変更しました。

# SET URL応答
CHAN_URL_CHANGED
	チャンネル「%s」のホームページアドレスを「%s」に設定しました。

# SET EMAIL応答
CHAN_EMAIL_CHANGED
	チャンネル「%s」のメールアドレスを「%s」に設定しました。

# SET ENTRYMSG応答
CHAN_ENTRY_MSG_CHANGED
	チャンネル「%s」の参加時メッセージを設定しました。

# SET MLOCK応答
CHAN_SET_MLOCK_NEED_PLUS_MINUS
	モードリストの先頭には「+」か「-」が必要です。
CHAN_SET_MLOCK_NEED_PARAM
	モード「+%c」にはパラメータが必要です。
CHAN_SET_MLOCK_NEED_POSITIVE
	モード「+%c」には正数パラメータが必要です。
CHAN_SET_MLOCK_MODE_REG_BAD
	モード「+%c」は、登録されたチャンネルには常に設定され、SET MLOCKでは使えません。
CHAN_SET_MLOCK_LINK_BAD
	モード「+%c」のパラメータは有効なチャンネル名ではありません。
CHAN_SET_MLOCK_LINK_SAME
	モード「+%c」のパラメータに、同じチャンネルを指定することはできません。
CHAN_SET_MLOCK_REQUIRES
	モード「+%c」をロックするときは、モード「+%c」を同時にロックしなければなりません。
CHAN_SET_MLOCK_BAD_PARAM
	モード「+%c」のパラメータは無効です。
CHAN_SET_MLOCK_UNKNOWN_CHAR
	「%c」：モード文字が無効です。
CHAN_SET_MLOCK_CANNOT_LOCK
	モード「+%c」はロックできません。
CHAN_MLOCK_CHANGED
	チャンネル「%s」のモードロックを「%s」に設定しました。
CHAN_MLOCK_REMOVED
	チャンネル「%s」のモードロックを解除しました。

# SET HIDE応答
CHAN_SET_HIDE_SYNTAX
	SET チャンネル HIDE {EMAIL | TOPIC | MLOCK} {ON | OFF}
CHAN_SET_HIDE_EMAIL_ON
	チャンネル「%s」の電子メールアドレスは%sのINFO応答で表示されません。
CHAN_SET_HIDE_EMAIL_OFF
	チャンネル「%s」の電子メールアドレスは%sのINFO応答で表示されます。
CHAN_SET_HIDE_TOPIC_ON
	チャンネル「%s」のトピックは%sのINFO応答で表示されません。
CHAN_SET_HIDE_TOPIC_OFF
	チャンネル「%s」のトピックは%sのINFO応答で表示されます。
CHAN_SET_HIDE_MLOCK_ON
	チャンネル「%s」のモードロックは%sのINFO応答で表示されません。
CHAN_SET_HIDE_MLOCK_OFF
	チャンネル「%s」のモードロックは%sのINFO応答で表示されます。

# SET KEEPTOPIC応答
CHAN_SET_KEEPTOPIC_SYNTAX
	SET チャンネル KEEPTOPIC {ON | OFF}
CHAN_SET_KEEPTOPIC_ON
	トピック記録設定を有効に設定しました。
CHAN_SET_KEEPTOPIC_OFF
	トピック記録設定を無効に設定しました。

# SET TOPICLOCK応答
CHAN_SET_TOPICLOCK_SYNTAX
	SET チャンネル TOPICLOCK {ON | OFF}
CHAN_SET_TOPICLOCK_ON
	トピック変更禁止設定を有効に設定しました。
CHAN_SET_TOPICLOCK_OFF
	トピック変更禁止設定を無効に設定しました。

# SET PRIVATE応答
CHAN_SET_PRIVATE_SYNTAX
	SET チャンネル PRIVATE {ON | OFF}
CHAN_SET_PRIVATE_ON
	プライベート設定を有効に設定しました。
CHAN_SET_PRIVATE_OFF
	プライベート設定を無効に設定しました。

# SET SECUREOPS応答
CHAN_SET_SECUREOPS_SYNTAX
	SET チャンネル SECUREOPS {ON | OFF}
CHAN_SET_SECUREOPS_ON
	オペレータ権制限設定を有効に設定しました。
CHAN_SET_SECUREOPS_OFF
	オペレータ権制限設定を無効に設定しました。

# SET LEAVEOPS応答
CHAN_SET_LEAVEOPS_SYNTAX
	SET チャンネル LEAVEOPS {ON | OFF}
CHAN_SET_LEAVEOPS_ON
	オペレータ権放置設定を有効に設定しました。
CHAN_SET_LEAVEOPS_OFF
	オペレータ権放置設定を無効に設定しました。

# SET RESTRICTED応答
CHAN_SET_RESTRICTED_SYNTAX
	SET チャンネル RESTRICTED {ON | OFF}
CHAN_SET_RESTRICTED_ON
	参加制限設定を有効に設定しました。
CHAN_SET_RESTRICTED_OFF
	参加制限設定を無効に設定しました。

# SET SECURE応答
CHAN_SET_SECURE_SYNTAX
	SET チャンネル SECURE {ON | OFF}
CHAN_SET_SECURE_ON
	セキュア設定を有効に設定しました。
CHAN_SET_SECURE_OFF
	セキュア設定を無効に設定しました。

# SET OPNOTICE応答
CHAN_SET_OPNOTICE_SYNTAX
	SET チャンネル OPNOTICE {ON | OFF}
CHAN_SET_OPNOTICE_ON
	OP周知設定を有効に設定しました。
CHAN_SET_OPNOTICE_OFF
	OP周知設定を無効に設定しました。

# SET ENFORCE応答
CHAN_SET_ENFORCE_SYNTAX
	SET チャンネル ENFORCE {ON | OFF}
CHAN_SET_ENFORCE_ON
	チャンネル権限管理設定を有効にしました。
CHAN_SET_ENFORCE_OFF
	チャンネル権限管理設定を無効にしました。

# SET MEMO-RESTRICTED応答
CHAN_SET_MEMO_RESTRICTED_SYNTAX
	SET チャンネル MEMO-RESTRICTED {ON | OFF}
CHAN_SET_MEMO_RESTRICTED_ON
	メモ送信制限設定を有効に設定しました。
CHAN_SET_MEMO_RESTRICTED_OFF
	メモ送信制限設定を無効に設定しました。

# SET NOEXPIRE応答
CHAN_SET_NOEXPIRE_SYNTAX
	SET チャンネル NOEXPIRE {ON | OFF}
CHAN_SET_NOEXPIRE_ON
	チャンネル「%s」の登録は自動解除しません。
CHAN_SET_NOEXPIRE_OFF
	チャンネル「%s」の登録は自動解除します。

# UNSET responses
CHAN_UNSET_SYNTAX
	UNSET チャンネル {SUCCESSOR | URL | EMAIL | ENTRYMSG}
CHAN_SUCCESSOR_UNSET
	チャンネル「%s」の後継者設定を解除しました。
CHAN_URL_UNSET
	チャンネル「%s」のホームページアドレスを消去しました。
CHAN_EMAIL_UNSET
	チャンネル「%s」のメールアドレスを消去しました。
CHAN_ENTRY_MSG_UNSET
	チャンネル「%s」の参加時メッセージを消去しました。

# ACCESS messages
CHAN_ACCESS_SYNTAX
	ACCESS チャンネル {ADD | DEL | LIST | LISTLEVEL | COUNT} [パラメータ]
CHAN_ACCESS_ADD_SYNTAX
	ACCESS チャンネル ADD ニック レベル
CHAN_ACCESS_DEL_SYNTAX
	ACCESS チャンネル DEL ニック
CHAN_ACCESS_LIST_SYNTAX
	ACCESS チャンネル LIST [+開始位置] [パターン]
CHAN_ACCESS_LISTLEVEL_SYNTAX
	ACCESS チャンネル LISTLEVEL [+開始位置] [レベル]
CHAN_ACCESS_COUNT_SYNTAX
	ACCESS チャンネル COUNT
CHAN_ACCESS_DISABLED
	アクセスリストの編集は現在出来ません。
CHAN_ACCESS_LEVEL_NONZERO
	アクセスレベルはゼロには設定できません。
CHAN_ACCESS_LEVEL_RANGE
	アクセスレベル%d〜%dの範囲内に設定して下さい。
CHAN_ACCESS_NICKS_ONLY
	アクセスリストに追加できるのは登録済みニックのみです。
CHAN_ACCESS_NOOP
	%sはチャンネルアクセスリストに登録できません。
CHAN_ACCESS_REACHED_LIMIT
	アクセスリストがいっぱいです。
CHAN_ACCESS_LEVEL_UNCHANGED
	%sの%sでのアクセスレベルはすでに%dとなっています。
CHAN_ACCESS_LEVEL_CHANGED
	%sの%sでのアクセスレベルを%dに変更しました。
CHAN_ACCESS_ADDED
	%sを%sのアクセスリストに、レベル%dで追加しました。
CHAN_ACCESS_NOT_FOUND
	%sは%sのアクセスリストに入っていません。
CHAN_ACCESS_DELETED
	%sを%sのアクセスリストから消去しました。
CHAN_ACCESS_LIST_EMPTY
	%sのアクセスリストは空です。
CHAN_ACCESS_NO_MATCH
	%sのアクセスリストには一致するエントリーはありません。
CHAN_ACCESS_LIST_HEADER
	%sのアクセスリスト：
	   レベル   ニック
CHAN_ACCESS_LIST_FORMAT
	     %4d   %s
CHAN_ACCESS_COUNT
	%sのアクセスリストには%dエントリーがあります。

# XOP (SOP/AOP等) 応答
CHAN_SOP_SYNTAX
	SOP チャンネル {ADD | DEL | LIST | COUNT} [パラメータ]
CHAN_AOP_SYNTAX
	AOP チャンネル {ADD | DEL | LIST | COUNT} [パラメータ]
CHAN_HOP_SYNTAX
	HOP チャンネル {ADD | DEL | LIST | COUNT} [パラメータ]
CHAN_VOP_SYNTAX
	VOP チャンネル {ADD | DEL | LIST | COUNT} [パラメータ]
CHAN_NOP_SYNTAX
	NOP チャンネル {ADD | DEL | LIST | COUNT} [パラメータ]
CHAN_SOP_LIST_SYNTAX
	SOP チャンネル LIST [+開始位置] [パターン]
CHAN_AOP_LIST_SYNTAX
	AOP チャンネル LIST [+開始位置] [パターン]
CHAN_HOP_LIST_SYNTAX
	HOP チャンネル LIST [+開始位置] [パターン]
CHAN_VOP_LIST_SYNTAX
	VOP チャンネル LIST [+開始位置] [パターン]
CHAN_NOP_LIST_SYNTAX
	NOP チャンネル LIST [+開始位置] [パターン]
CHAN_XOP_ADDED
	%sを%sの%sリストに追加しました。
CHAN_XOP_LEVEL_CHANGED
	%sを%sの%sリストに移動しました。
CHAN_XOP_LEVEL_UNCHANGED
	%sはすでに%sの%sリストに入っています。
CHAN_XOP_NICKS_ONLY
	SOP、AOP、VOP及びNOPリストに追加できるのは登録済みニックのみです。
CHAN_XOP_NOOP
	%sはチャンネルアクセスリストに登録できません。
CHAN_XOP_NICKS_ONLY_HOP
	SOP、AOP、HOP、VOP及びNOPリストに追加できるのは登録済みニックのみです。
CHAN_XOP_REACHED_LIMIT
	SOP、AOP、VOP及びNOPリストのエントリー数は合計%d個を超えるため、追加できません。
CHAN_XOP_REACHED_LIMIT_HOP
	SOP、AOP、HOP、VOP及びNOPリストのエントリー数は合計%d個を超えるため、追加できません。
CHAN_XOP_NOT_FOUND
	%sは、%sの%sリストに入っていません。
CHAN_XOP_DELETED
	%sを、%sの%sリストから消去しました。
CHAN_XOP_LIST_EMPTY
	%sの%sリストは空です。
CHAN_XOP_NO_MATCH
	%sの%sリストには一致するエントリーはありません。
CHAN_XOP_LIST_HEADER
	%sリスト (%s)：
CHAN_XOP_COUNT
	%sの%sリストには%d個のエントリーがあります。

# AKICK応答
CHAN_AKICK_SYNTAX
	AKICK チャンネル {ADD | DEL | LIST | VIEW | COUNT | ENFORCE} [パラメータ]
CHAN_AKICK_LIST_SYNTAX
	AKICK チャンネル LIST [+開始位置] [パターン]
CHAN_AKICK_VIEW_SYNTAX
	AKICK チャンネル LIST [+開始位置] [パターン]
CHAN_AKICK_DISABLED
	自動キックリストの編集は現在できません。
CHAN_AKICK_ALREADY_EXISTS
	「%s」はすでに%sの自動キックリストに存在しています。
CHAN_AKICK_REACHED_LIMIT
	自動キックリストがいっぱいです。
CHAN_AKICK_ADDED
	「%s」を%sの自動キックリストに追加しました。
CHAN_AKICK_NOT_FOUND
	「%s」は%sの自動キックリストで存在しません。
CHAN_AKICK_DELETED
	「%s」を%sの自動キックリストから消去しました。
CHAN_AKICK_LIST_EMPTY
	%sの自動キックリストは空です。
CHAN_AKICK_NO_MATCH
	%sの自動キックリストでは一致するエントリーはありません。
CHAN_AKICK_LIST_HEADER
	%sの自動キックリスト：
# mask, set-by, set-time, last-used, reason
CHAN_AKICK_VIEW_FORMAT
	    %s (%sにより追加)
	        設定時刻：%s・最近使用時刻：%s
	        理由：%s
# mask, set-by, set-time, reason
CHAN_AKICK_VIEW_UNUSED_FORMAT
	    %s (%sにより追加)
	        設定時刻：%s・未使用
	        理由：%s
CHAN_AKICK_COUNT
	%sの自動キックリストに%d個のエントリーがあります。
CHAN_AKICK_ENFORCE_DONE
	%sにおけるAKICK ENFORCEが完了しました。%d人のユーザがキックされました。

# LEVELS応答
CHAN_LEVELS_SYNTAX
	LEVELS チャンネル {SET | DIS[ABLE] | LIST | RESET} [項目 [レベル]]
CHAN_LEVELS_READONLY
	チャンネルの権限レベル設定の変更は現在出来ません。
CHAN_LEVELS_RANGE
	レベルは、%d〜%dの範囲内に設定して下さい。
CHAN_LEVELS_CHANGED
	%sのレベルを%sで%dに変更しました。
CHAN_LEVELS_UNKNOWN
	%sという項目はありません。項目一覧を見る場合は、「/msg %s HELP LEVELS DESC」を入力して下さい。
CHAN_LEVELS_DISABLED
	%sを%sで無効にしました。
CHAN_LEVELS_LIST_HEADER
	%sの権限レベル一覧：
CHAN_LEVELS_LIST_DISABLED
	    %-*s  無効
CHAN_LEVELS_LIST_FOUNDER
	    %-*s  創設者のみ
CHAN_LEVELS_LIST_NORMAL
	    %-*s  %4d
CHAN_LEVELS_RESET
	%sの権限レベルを既定値に戻しました。

# INFO応答
CHAN_INFO_SYNTAX
	INFO チャンネル [ALL]
CHAN_INFO_HEADER
	%sの情報：
CHAN_INFO_FOUNDER
	　　　　　創設者：%s
CHAN_INFO_SUCCESSOR
	　　　　　後継者：%s
CHAN_INFO_DESCRIPTION
	　　　　　　説明：%s
CHAN_INFO_ENTRYMSG
	参加時メッセージ：
CHAN_INFO_TIME_REGGED
	　　　　登録日時：%s
CHAN_INFO_LAST_USED
	　　最近使用日時：%s
CHAN_INFO_LAST_TOPIC
	　最近のトピック：%s
CHAN_INFO_TOPIC_SET_BY
	　トピック設定者：%s
CHAN_INFO_URL
	　　ホームページ：%s
CHAN_INFO_EMAIL
	　メールアドレス：%s
CHAN_INFO_OPTIONS
	　　　オプション：%s
CHAN_INFO_OPT_PRIVATE
	プライベート
CHAN_INFO_OPT_KEEPTOPIC
	トピック記録
CHAN_INFO_OPT_TOPICLOCK
	トピック変更禁止
CHAN_INFO_OPT_SECUREOPS
	オペレータ権制限
CHAN_INFO_OPT_LEAVEOPS
	オペレータ権放置
CHAN_INFO_OPT_RESTRICTED
	アクセス制限
CHAN_INFO_OPT_SECURE
	セキュア
CHAN_INFO_OPT_OPNOTICE
	OP周知
CHAN_INFO_OPT_ENFORCE
	チャンネル権限管理
CHAN_INFO_OPT_MEMO_RESTRICTED
	メモ送信制限
CHAN_INFO_OPT_NONE
	無し
CHAN_INFO_MODE_LOCK
	　　モードロック：%s
CHAN_INFO_NO_EXPIRE
	このチャンネルの登録は自動解除されません。
CHAN_INFO_SUSPEND_DETAILS
	%sにより、%sに使用停止にされています（%s）。
CHAN_INFO_SUSPEND_REASON
	理由：%s
CHAN_INFO_SHOW_ALL
	詳細情報を表示する場合、「/msg %s INFO %s ALL」を入力して下さい。

# LIST応答
CHAN_LIST_SYNTAX
	LIST [+開始位置] パターン
CHAN_LIST_OPER_SYNTAX
	LIST [+開始位置] パターン [FORBIDDEN] [NOEXPIRE] [SUSPENDED]
CHAN_LIST_HEADER
	「%s」に一致するチャンネル：
CHAN_LIST_NO_MATCH
	一致するチャンネルはありません。

# INVITE応答
CHAN_INVITE_SYNTAX
	INVITE チャンネル
CHAN_INVITE_OK
	%sをチャンネル「%s」に誘いました。

# OP/DEOP/VOICE/DEVOICE応答
CHAN_OPVOICE_SYNTAX
	%s チャンネル [ニック]
CHAN_OP_SUCCEEDED
	「%s」にチャンネル「%s」のオペレータ権を付与しました。
CHAN_OP_ALREADY
	「%s」はすでにチャンネル「%s」のオペレータ権を持っています。
CHAN_OP_FAILED
	「%s」にチャンネル「%s」のオペレータ権を付与できませんでした。
CHAN_DEOP_SUCCEEDED
	「%s」からチャンネル「%s」のオペレータ権を解除しました。
CHAN_DEOP_ALREADY
	「%s」はチャンネル「%s」のオペレータ権を持っていません。
CHAN_DEOP_FAILED
	「%s」からチャンネル「%s」のオペレータ権を解除できませんでした。
CHAN_VOICE_SUCCEEDED
	「%s」にチャンネル「%s」の発言権を付与しました。
CHAN_VOICE_ALREADY
	「%s」はすでにチャンネル「%s」の発言権を持っています。
CHAN_VOICE_FAILED
	「%s」にチャンネル「%s」の発言権を付与できませんでした。
CHAN_DEVOICE_SUCCEEDED
	「%s」からチャンネル「%s」の発言権を解除しました。
CHAN_DEVOICE_ALREADY
	「%s」はチャンネル「%s」の発言権を持っていません。
CHAN_DEVOICE_FAILED
	「%s」からチャンネル「%s」の発言権を解除できませんでした。
CHAN_HALFOP_SUCCEEDED
	「%s」にチャンネル「%s」の半オペレータ権を付与しました。
CHAN_HALFOP_ALREADY
	「%s」はすでにチャンネル「%s」の半オペレータ権を持っています。
CHAN_HALFOP_FAILED
	「%s」にチャンネル「%s」の半オペレータ権を付与できませんでした。
CHAN_DEHALFOP_SUCCEEDED
	「%s」からチャンネル「%s」の半オペレータ権を解除しました。
CHAN_DEHALFOP_ALREADY
	「%s」はチャンネル「%s」の半オペレータ権を持っていません。
CHAN_DEHALFOP_FAILED
	「%s」からチャンネル「%s」の半オペレータ権を解除できませんでした。
CHAN_PROTECT_SUCCEEDED
	「%s」をチャンネル「%s」でプロテクト状態にしました。
CHAN_PROTECT_ALREADY
	「%s」はすでにチャンネル「%s」でプロテクト状態になっています。
CHAN_PROTECT_FAILED
	「%s」をチャンネル「%s」でプロテクト状態にすることができませんでした。
CHAN_DEPROTECT_SUCCEEDED
	「%s」をチャンネル「%s」で非プロテクト状態にしました。
CHAN_DEPROTECT_ALREADY
	「%s」はすでにチャンネル「%s」で非プロテクト状態になっています。
CHAN_DEPROTECT_FAILED
	「%s」をチャンネル「%s」で非プロテクト状態にすることができませんでした。

# UNBAN応答
CHAN_UNBAN_SYNTAX
	UNBAN チャンネル
CHAN_UNBANNED
	あなたに対する%sの追放マスクを消去しました。

# KICK応答
CHAN_KICK_SYNTAX
	KICK チャンネル ニック [理由]
CHAN_KICK_PROTECTED
	「%s」をチャンネル「%s」からキックできません。
CHAN_KICKED
	「%s」をチャンネル「%s」からキックしました。

# TOPIC応答
CHAN_TOPIC_SYNTAX
	TOPIC チャンネル トピック

# CLEAR応答
CHAN_CLEAR_SYNTAX
	CLEAR チャンネル 項目
CHAN_CLEARED_BANS
	チャンネル「%s」の追放マスクを全て消去しました。
CHAN_CLEARED_EXCEPTIONS
	チャンネル「%s」の追放例外マスクを消去しました。
CHAN_CLEARED_INVITES
	チャンネル「%s」の誘い（invite）マスクを消去しました。
CHAN_CLEARED_MODES
	チャンネル「%s」のモードを既定値に戻しました。
CHAN_CLEARED_OPS
	オペレータ権をチャンネル「%s」から消去しました。
CHAN_CLEARED_HALFOPS
	半オペレータ権をチャンネル「%s」から消去しました。
CHAN_CLEARED_VOICES
	発言権をチャンネル「%s」から消去しました。
CHAN_CLEARED_USERS
	チャンネル「%s」のユーザを全てキックしました。

# GETPASS応答
CHAN_GETPASS_SYNTAX
	GETPASS チャンネル
CHAN_GETPASS_UNAVAILABLE
	チャンネル「%s」のパスワードは暗号化されているため、GETPASSを使用できません。
CHAN_GETPASS_PASSWORD_IS
	チャンネル「%s」のパスワードは %s です。

# FORBID応答
CHAN_FORBID_SYNTAX
	FORBID チャンネル
CHAN_FORBID_SHORT_CHANNEL
	チャンネル「#」を使用禁止にすることはできません。
CHAN_FORBID_SUCCEEDED
	チャンネル「%s」を使用禁止にしました。
CHAN_FORBID_FAILED
	チャンネル「%s」を使用禁止にすることができませんでした。

CHAN_SUSPEND_SYNTAX
	SUSPEND [+有効期限] チャンネル 理由
CHAN_SUSPEND_SUCCEEDED
	チャンネル「%s」を使用停止にしました。
CHAN_SUSPEND_ALREADY_SUSPENDED
	チャンネル「%s」はすでに使用停止になっています。

CHAN_UNSUSPEND_SYNTAX
	UNSUSPEND チャンネル
CHAN_UNSUSPEND_SUCCEEDED
	チャンネル「%s」の使用停止を解除しました。
CHAN_UNSUSPEND_NOT_SUSPENDED
	チャンネル「%s」は使用停止になっていません。

###########################################################################
#
# MemoServのメッセージ
#
###########################################################################

# 自動メッセージ
MEMO_HAVE_NEW_MEMO
	新着メモが1件あります。
MEMO_HAVE_NEW_MEMOS
	新着メモが%d件あります。
MEMO_TYPE_READ_LAST
	読むには「/msg %s READ LAST」を入力して下さい。
MEMO_TYPE_READ_NUM
	読むには「/msg %s READ %d」を入力して下さい。
MEMO_TYPE_LIST_NEW
	表示するには「/msg %s LIST NEW」を入力して下さい。
MEMO_AT_LIMIT
	注意：保存メモ件数が上限（%d件）に達しています。メモを消去するまで新たなメモを受信できません。
MEMO_OVER_LIMIT
	注意：保存メモ件数が上限（%d件）を超えています。メモを消去するまで新たなメモを受信できません。
MEMO_NEW_MEMO_ARRIVED
	%sからメモが届きました。
	読むには「/msg %s READ %d」を入力して下さい。
MEMO_NEW_CHAN_MEMO_ARRIVED
	%sから%s宛てのメモが届きました。
	読むには「/msg %s READ %d」を入力して下さい。

# 多用応答
MEMO_HAVE_NO_MEMOS
	保存メモはありません。
MEMO_HAVE_NO_NEW_MEMOS
	新着メモはありません。
MEMO_DOES_NOT_EXIST
	メモ番号%dは存在しません。
MEMO_LIST_NOT_FOUND
	%sに一致する保存メモはありません。

# SEND応答
MEMO_SEND_SYNTAX
	SEND {ニック | チャンネル} テキスト
MEMO_SEND_DISABLED
	メモの送信は現在出来ません。
MEMO_SEND_PLEASE_WAIT
	SENDコマンドを再度実行する前に%sお待ち下さい。
MEMO_SEND_CHAN_NOT_AVAIL
	チャンネルメモはご利用できません。
MEMO_X_HAS_TOO_MANY_MEMOS
	%sは現在、保存メモが多すぎるため、新たなメモを受信できません。
MEMO_X_GETS_NO_MEMOS
	%sはメモを受信できません。
MEMO_SENT
	%sにメモを送信しました。
MEMO_SEND_FAILED
	%sにメモを送信できませんでした。

# LIST応答
MEMO_LIST_SYNTAX
	LIST [番号 | リスト | NEW]
MEMO_LIST_MEMOS
	%sのメモ一覧。読むには「/msg %s READ 番号」
MEMO_LIST_NEW_MEMOS
	%sの新着メモ一覧。読むには「/msg %s READ 番号」
MEMO_LIST_HEADER
	   番号  差出人            日時
MEMO_LIST_FORMAT
	%c%c%c%4d  %-16s  %s

# READ応答
MEMO_READ_SYNTAX
	READ {番号 | リスト | LAST | NEW}
MEMO_HEADER
	%d番のメモ（%s、%s）。消去するには「/msg %s DEL %d」
MEMO_CHAN_HEADER
	%d番のメモ（%s、%s宛て、%s）。消去するには「/msg %s DEL %d」

# SAVE応答
MEMO_SAVE_SYNTAX
	SAVE {番号 | リスト}
MEMO_SAVED_ONE
	%d番のメモを保存しました。
MEMO_SAVED_SEVERAL
	メモを%d件保存しました。

# DEL応答
MEMO_DEL_SYNTAX
	DEL {番号 | リスト | ALL}
MEMO_DELETED_NONE
	入力した番号に一致するメモはありません。
MEMO_DELETED_ONE
	%d番のメモを消去しました。
MEMO_DELETED_SEVERAL
	メモを%d件消去しました。
MEMO_DELETED_ALL
	メモを全て消去しました。

# RENUMBER応答
MEMO_RENUMBER_ONLY_YOU
	RENUMBERは自分のメモに対してしか使えません。
MEMO_RENUMBER_DONE
	メモ番号を振り直しました。

# SET応答
MEMO_SET_SYNTAX
	SET 設定名 値
MEMO_SET_DISABLED
	メモのオプション設定は現在出来ません。
MEMO_SET_UNKNOWN_OPTION
	%sという設定名はありません。

# SET NOTIFY応答
MEMO_SET_NOTIFY_SYNTAX
	SET NOTIFY {ON | LOGON | NEW | OFF}
MEMO_SET_NOTIFY_ON
	メモ通知は、ログイン時・受信時に設定しました。
MEMO_SET_NOTIFY_LOGON
	メモ通知は、ログイン時のみに設定しました。
MEMO_SET_NOTIFY_NEW
	メモ通知は、受信時のみに設定しました。
MEMO_SET_NOTIFY_OFF
	メモ通知を無効にしました。

# SET LIMIT応答
MEMO_SET_LIMIT_SYNTAX
	SET LIMIT 上限
MEMO_SET_LIMIT_OPER_SYNTAX
	SET LIMIT [ニック] {上限 | NONE | DEFAULT} [HARD]
MEMO_SET_YOUR_LIMIT_FORBIDDEN
	メモ件数上限を変更できません。
MEMO_SET_LIMIT_FORBIDDEN
	%sのメモ件数上限を変更できません。
MEMO_SET_YOUR_LIMIT_TOO_HIGH
	メモ件数上限は%d件以内に設定して下さい。
MEMO_SET_LIMIT_TOO_HIGH
	%sのメモ件数上限は%d件以内に設定して下さい。
MEMO_SET_LIMIT_OVERFLOW
	上限を%dに制限しました。
MEMO_SET_YOUR_LIMIT
	メモ件数上限を%d件に設定しました。
MEMO_SET_YOUR_LIMIT_ZERO
	メモを受信拒否に設定しました。
MEMO_SET_YOUR_LIMIT_DEFAULT
	メモ件数上限を既定値（%d）に戻しました。
MEMO_UNSET_YOUR_LIMIT
	メモ件数の上限を無限に設定しました。
MEMO_SET_LIMIT
	%sのメモ数の上限を%d件に設定しました。
MEMO_SET_LIMIT_ZERO
	%sのメモを受信拒否に設定しました。
MEMO_SET_LIMIT_DEFAULT
	%sのメモ件数上限を既定値（%d）に戻しました。
MEMO_UNSET_LIMIT
	%sのメモ件数の上限は無限に設定しました。

# INFO応答
MEMO_INFO_NO_MEMOS
	保存されているメモはありません。
MEMO_INFO_MEMO
	メモが1件あります。
MEMO_INFO_MEMO_UNREAD
	メモが1件あり、未読です。
MEMO_INFO_MEMOS
	メモが%d件あります。
MEMO_INFO_MEMOS_ONE_UNREAD
	メモが%d件あり、1件は未読です。
MEMO_INFO_MEMOS_SOME_UNREAD
	メモが%d件あり、%d件は未読です。
MEMO_INFO_MEMOS_ALL_UNREAD
	メモが%d件あり、全て未読です。
MEMO_INFO_LIMIT
	メモ件数上限は%d件です。
MEMO_INFO_HARD_LIMIT
	メモ件数上限は%d件で、変更できません。
MEMO_INFO_LIMIT_ZERO
	メモは受信拒否に設定されています。
MEMO_INFO_HARD_LIMIT_ZERO
	メモは受信拒否に設定されており、変更できません。
MEMO_INFO_NO_LIMIT
	メモ件数上限はありません。
MEMO_INFO_NOTIFY_OFF
	新着メモ通知は行いません。
MEMO_INFO_NOTIFY_ON
	新着メモ通知は接続時、/AWAY解除時、受信時に行います。
MEMO_INFO_NOTIFY_RECEIVE
	新着メモ通知は受信時にのみ行います。
MEMO_INFO_NOTIFY_SIGNON
	新着メモ通知は接続時、/AWAY解除時にのみ行います。
MEMO_INFO_X_NO_MEMOS
	%sは保存されているメモがありません。
MEMO_INFO_X_MEMO
	%sはメモが1件あります。
MEMO_INFO_X_MEMO_UNREAD
	%sはメモが1件あり、未読です。
MEMO_INFO_X_MEMOS
	%sはメモが%d件あります。
MEMO_INFO_X_MEMOS_ONE_UNREAD
	%sはメモが%d件あり、1件は未読です。
MEMO_INFO_X_MEMOS_SOME_UNREAD
	%sはメモが%d件あり、%d件は未読です。
MEMO_INFO_X_MEMOS_ALL_UNREAD
	%sはメモが%d件あり、全て未読です。
MEMO_INFO_X_LIMIT
	%sのメモ件数上限は%d件です。
MEMO_INFO_X_HARD_LIMIT
	%sのメモ件数上限は%d件で、変更できません。
MEMO_INFO_X_NO_LIMIT
	%sのメモ件数上限はありません。
MEMO_INFO_X_NOTIFY_OFF
	%sへの新着メモ通知は行いません。
MEMO_INFO_X_NOTIFY_ON
	%sへの新着メモ通知は接続時、/AWAY解除時、受信時に行います。
MEMO_INFO_X_NOTIFY_RECEIVE
	%sへの新着メモ通知は受信時にのみ行います。
MEMO_INFO_X_NOTIFY_SIGNON
	%sへの新着メモ通知は接続時、/AWAY解除時にのみ行います。

################ forwardモジュール用メッセージ

# メール内容
MEMO_FORWARD_MAIL_SUBJECT
	%sさんへのメモ
MEMO_FORWARD_MULTIPLE_MAIL_SUBJECT
	%sさんへのメモ
MEMO_FORWARD_MAIL_BODY
	%sさんからのメモ（送信日時：%s）
	%s
MEMO_FORWARD_CHANMEMO_MAIL_BODY
	%sさんからのメモ（%s宛て、送信日時：%s）
	%s

# FORWARD応答
MEMO_FORWARD_SYNTAX
	FORWARD {番号 | リスト | ALL}
MEMO_FORWARD_NEED_EMAIL
	メモを転送するには、ニックにメールアドレスを登録しなければなりません。
MEMO_FORWARD_PLEASE_WAIT
	FORWARDコマンドを再度実行する前に、%sお待ち下さい。
MEMO_FORWARD_FAILED
	メモ転送が失敗しました。
MEMO_FORWARDED_NONE
	メモ転送を行いませんでした。
MEMO_FORWARDED_ONE
	%d番のメモを転送しました。
MEMO_FORWARDED_SEVERAL
	メモを%d件転送しました。
MEMO_FORWARDED_ALL
	すべてのメモを転送しました。

# SET FORWARD responses
MEMO_SET_FORWARD_SYNTAX
	SET FORWARD {ON | COPY | OFF}
MEMO_SET_FORWARD_ON
	新着メモは、あなたのメールアドレス「%s」に転送されます。
MEMO_SET_FORWARD_COPY
	新着メモは、あなたのメールアドレス「%s」に転送されます。
	Servicesのデータベースにも保存されます。
MEMO_SET_FORWARD_OFF
	新着メモのメール転送を行いません。

################ ignoreモジュール用メッセージ

# IGNORE responses
MEMO_IGNORE_SYNTAX
	IGNORE {ADD | DEL | LIST} [ニック | マスク]
MEMO_IGNORE_ADD_SYNTAX
	IGNORE ADD ニック | マスク
MEMO_IGNORE_DEL_SYNTAX
	IGNORE DEL ニック | マスク
MEMO_IGNORE_LIST_FULL
	受信拒否リストがいっぱいです。
MEMO_IGNORE_ALREADY_PRESENT
	「%s」はすでに受信拒否リストに登録されています。
MEMO_IGNORE_ADDED
	「%s」を受信拒否リストに追加しました。
MEMO_IGNORE_NOT_FOUND
	「%s」は受信拒否リストに登録されていません。
MEMO_IGNORE_DELETED
	「%s」を受信拒否リストから削除しました。
MEMO_IGNORE_LIST_EMPTY
	受信拒否リストは空です。
MEMO_IGNORE_LIST
	受信拒否リスト：
MEMO_IGNORE_LIST_X_EMPTY
	「%s」の受信拒否リストは空です。
MEMO_IGNORE_LIST_X
	「%s」の受信拒否リスト：

###########################################################################
#
# OperServのメッセージ
#
###########################################################################

# 多用メッセージ
OPER_BOUNCY_MODES
	Servicesはモードを変更できません。サーバの設定を確認して下さい。
OPER_BOUNCY_MODES_U_LINE
	Servicesはモードを変更できません。サーバのU:line設定を確認して下さい。

# GLOBAL応答
OPER_GLOBAL_SYNTAX
	GLOBAL メッセージ

# STATS応答
OPER_STATS_UNKNOWN_OPTION
	「%s」というSTATSパラメータはありません。
OPER_STATS_RESET_USER_COUNT
	最大ユーザ数をリセットしました。

OPER_STATS_CURRENT_USERS
	現在ユーザ数：%d
	オペレータ数：%d
OPER_STATS_MAX_USERS
	最大ユーザ数：%d（%s）
OPER_STATS_UPTIME_DHM
	起動から：%d%s%02d時間%02d分
OPER_STATS_UPTIME_HM_MS
	起動から：%s

OPER_STATS_KBYTES_READ
	受信データ量：        %10u kB
OPER_STATS_KBYTES_WRITTEN
	送信データ量：        %10u kB
OPER_STATS_NETBUF_SOCK
	ソケットバッファ (サーバ)：%5u kB
OPER_STATS_NETBUF_SOCK_PERCENT
	ソケットバッファ (サーバ)：%5u kB (%d%%)
OPER_STATS_NETBUF_TOTAL
	ソケットバッファ (全体)：  %5u kB
OPER_STATS_NETBUF_TOTAL_PERCENT
	ソケットバッファ (全体)：  %5u kB (%d%%)

OPER_STATS_ALL_USER_MEM
	ユーザ        : %6d レコード、%5d kB
OPER_STATS_ALL_CHANNEL_MEM
	チャンネル    : %6d レコード、%5d kB
OPER_STATS_ALL_SERVER_MEM
	サーバ        : %6d レコード、%5d kB
OPER_STATS_ALL_NICKGROUPINFO_MEM
	ニックグループ: %6d レコード、%5d kB
OPER_STATS_ALL_NICKINFO_MEM
	ニック        : %6d レコード、%5d kB
OPER_STATS_ALL_CHANSERV_MEM
	ChanServ      : %6d レコード、%5d kB
OPER_STATS_ALL_STATSERV_MEM
	StatServ      : %6d レコード、%5d kB
OPER_STATS_ALL_NEWS_MEM
	News          : %6d レコード、%5d kB
OPER_STATS_ALL_AKILL_MEM
	自動切断マスク: %6d レコード、%5d kB
OPER_STATS_ALL_EXCEPTION_MEM
	セッション例外: %6d レコード、%5d kB
OPER_STATS_ALL_SGLINE_MEM
	SGlines       : %6d レコード、%5d kB
OPER_STATS_ALL_SQLINE_MEM
	SQlines       : %6d レコード、%5d kB
OPER_STATS_ALL_SZLINE_MEM
	SZlines       : %6d レコード、%5d kB
OPER_STATS_ALL_SESSION_MEM
	セッション    : %6d レコード、%5d kB

# GETKEY応答
OPER_GETKEY_SYNTAX
	GETKEY チャンネル
OPER_GETKEY_KEY_IS
	チャンネル「%s」のキーワードは：%s
OPER_GETKEY_NO_KEY
	チャンネル「%s」にはキーワードが設定されていません。

# MODE応答
OPER_MODE_SYNTAX
	MODE チャンネル モード

# CLEARMODES応答
OPER_CLEARMODES_SYNTAX
	CLEARMODES チャンネル [ALL]
OPER_CLEARMODES_DONE
	チャンネル「%s」のチャンネルモードをクリアしました。
OPER_CLEARMODES_ALL_DONE
	チャンネル「%s」の全モードをクリアしました。

# CLEARCHAN応答
OPER_CLEARCHAN_SYNTAX
	CLEARCHAN チャンネル
OPER_CLEARCHAN_DONE
	チャンネル「%s」から全てのユーザをキックしました。

# KICK応答
OPER_KICK_SYNTAX
	KICK チャンネル ニック 理由

# ADMIN応答
OPER_ADMIN_SYNTAX
	ADMIN {ADD | DEL | LIST} [ニック]
OPER_ADMIN_ADD_SYNTAX
	ADMIN ADD ニック
OPER_ADMIN_DEL_SYNTAX
	ADMIN DEL ニック
OPER_ADMIN_NO_NICKSERV
	NickServが動作していないため、ADMINコマンドは使えません。
OPER_ADMIN_EXISTS
	ニック「%s」はすでにServicesアドミンリストに登録されています。
OPER_ADMIN_ADDED
	ニック「%s」をServicesアドミンリストに追加しました。
OPER_ADMIN_TOO_MANY
	Servicesアドミンリストはいっぱいですので追加できません。
OPER_ADMIN_REMOVED
	ニック「%s」をServicesアドミンリストから消去しました。
OPER_ADMIN_NOT_FOUND
	ニック「%s」はServicesアドミンリストに登録されていません。
OPER_ADMIN_LIST_HEADER
	Servicesアドミンリスト：

# OPER応答
OPER_OPER_SYNTAX
	OPER {ADD | DEL | LIST} [ニック]
OPER_OPER_ADD_SYNTAX
	OPER ADD ニック
OPER_OPER_DEL_SYNTAX
	OPER DEL ニック
OPER_OPER_NO_NICKSERV
	NickServが動作していないため、OPERコマンドは使えません。
OPER_OPER_EXISTS
	ニック「%s」はすでにServicesオペレータリストに登録されています。
OPER_OPER_ADDED
	ニック「%s」をServicesオペレータリストに追加しました。
OPER_OPER_TOO_MANY
	Servicesオペレータリストはいっぱいですので追加できません。
OPER_OPER_REMOVED
	ニック「%s」をServicesオペレータリストから消去しました。
OPER_OPER_NOT_FOUND
	ニック「%s」はServicesオペレータリストに登録されていません。
OPER_OPER_LIST_HEADER
	Servicesオペレータリスト：

# AKILL・EXCLUDE・SxLINE共通応答
OPER_MASKDATA_SYNTAX
	%s {ADD | DEL | LIST | VIEW | CHECK | COUNT} [[+有効期限] マスク [理由]]
OPER_MASKDATA_ADD_SYNTAX
	%s ADD [+有効期限] マスク 理由
OPER_MASKDATA_DEL_SYNTAX
	%s DEL マスク
OPER_MASKDATA_CLEAR_SYNTAX
	%s CLEAR ALL
OPER_MASKDATA_LIST_SYNTAX
	%s {LIST | VIEW} [+開始位置] [パターン] [NOEXPIRE]
OPER_MASKDATA_CHECK_SYNTAX
	%s CHECK マスク
# mask, reason
OPER_MASKDATA_LIST_FORMAT
	%-32s  %s
# mask, set-by, set-time, last-used, expires, reason
OPER_MASKDATA_VIEW_FORMAT
	%s
	　　　設定者：%s
	　　設定時刻：%s
	　　最近使用：%s
	　　有効期限：%s
	　　　　理由：%s
# mask, set-by, set-time, expires, reason
OPER_MASKDATA_VIEW_UNUSED_FORMAT
	%s
	　　　設定者：%s
	　　設定時刻：%s
	　　最近使用：未使用
	　　有効期限：%s
	　　　　理由：%s

# AKILL応答
OPER_TOO_MANY_AKILLS
	自動切断リストはいっぱいですので追加できません。
OPER_AKILL_EXISTS
	「%s」はすでに自動切断リストに存在します。
OPER_AKILL_NO_NICK
	自動切断マスクにはニックを使えません。マスクを確認して下さい。
OPER_AKILL_MASK_TOO_GENERAL
	マスクの適用範囲が広すぎます。
OPER_AKILL_EXPIRY_LIMITED
	有効期限は%sまでしか設定できません。
OPER_AKILL_ADDED
	「%s」を自動切断リストに追加しました。
OPER_AKILL_REMOVED
	「%s」を自動切断リストから消去しました。
OPER_AKILL_CLEARED
	自動切断リストをクリアしました。
OPER_AKILL_NOT_FOUND
	「%s」は自動切断リストに登録されていません。
OPER_AKILL_LIST_HEADER
	自動切断リスト：
OPER_AKILL_LIST_EMPTY
	自動切断リストは空です。
OPER_AKILL_LIST_NO_MATCH
	自動切断リストに一致するマスクはありません。
OPER_AKILL_CHECK_NO_MATCH
	%sに一致する自動切断マスクは見つかりませんでした。
OPER_AKILL_CHECK_HEADER
	%sに一致する自動切断マスク：
OPER_AKILL_CHECK_TRAILER
	%dマスク見つかりました。
OPER_AKILL_COUNT
	%d個のマスクが自動切断リストに登録されています。

# AKILLCHAN応答
OPER_AKILLCHAN_SYNTAX
	AKILLCHAN [KILL] [+有効期限] チャンネル 理由
OPER_AKILLCHAN_AKILLED
	%dユーザを自動切断リストに追加しました。
OPER_AKILLCHAN_KILLED
	%dユーザを切断しました。
OPER_AKILLCHAN_AKILLED_ONE
	1ユーザを自動切断リストに追加しました。
OPER_AKILLCHAN_KILLED_ONE
	1ユーザを切断しました。

# EXCLUDE応答
OPER_TOO_MANY_EXCLUDES
	自動切断除外リストはいっぱいですので追加できません。
OPER_EXCLUDE_EXISTS
	「%s」はすでに自動切断除外リストに存在します。
OPER_EXCLUDE_ADDED
	「%s」を自動切断除外リストに追加しました。
OPER_EXCLUDE_REMOVED
	「%s」を自動切断除外リストから消去しました。
OPER_EXCLUDE_CLEARED
	自動切断除外リストをクリアしました。
OPER_EXCLUDE_NOT_FOUND
	「%s」は自動切断除外リストに登録されていません。
OPER_EXCLUDE_LIST_HEADER
	自動切断除外リスト：
OPER_EXCLUDE_LIST_EMPTY
	自動切断除外リストは空です。
OPER_EXCLUDE_LIST_NO_MATCH
	自動切断除外リストに一致するマスクはありません。
OPER_EXCLUDE_CHECK_NO_MATCH
	%sに一致する自動切断除外マスクは見つかりませんでした。
OPER_EXCLUDE_CHECK_HEADER
	%sに一致する自動切断除外マスク：
OPER_EXCLUDE_CHECK_TRAILER
	%dマスク見つかりました。
OPER_EXCLUDE_COUNT
	%d個のマスクが自動切断除外リストに登録されています。

# SLINE応答
OPER_TOO_MANY_SLINES
	%sリストはいっぱいですので追加できません。
OPER_SLINE_EXISTS
	「%s」はすでに%sリストに存在します。
OPER_SLINE_MASK_TOO_GENERAL
	マスクの適用範囲が広すぎます。
OPER_SLINE_ADDED
	「%s」を%sリストに追加しました。
OPER_SLINE_REMOVED
	「%s」を%sリストから消去しました。
OPER_SLINE_CLEARED
	%sリストをクリアしました。
OPER_SLINE_NOT_FOUND
	「%s」は%sリストに登録されていません。
OPER_SLINE_LIST_HEADER
	%sリスト：
OPER_SLINE_LIST_EMPTY
	%sリストは空です。
OPER_SLINE_LIST_NO_MATCH
	%sリストに一致するマスクはありません。
OPER_SLINE_CHECK_NO_MATCH
	%sに一致する%sマスクは見つかりませんでした。
OPER_SLINE_CHECK_HEADER
	%sに一致する%sマスク：
OPER_SLINE_CHECK_TRAILER
	%dマスク見つかりました。
OPER_SLINE_COUNT
	%d個のマスクが%sリストに登録されています。
OPER_SZLINE_NOT_AVAIL
	このIRCサーバでは、SZLINEは使えません。

# SU応答
OPER_SU_SYNTAX
	SU パスワード
OPER_SU_NO_PASSWORD
	スーパーユーザパスワードが設定されていないため、SUコマンドを使えません。
OPER_SU_SUCCEEDED
	スーパーユーザ権の取得に成功しました。
OPER_SU_FAILED
	パスワードを確認することができませんでした。

# SET応答
OPER_SET_SYNTAX
	SET 設定名 値
OPER_SET_IGNORE_ON
	ユーザ無視処理を有効に設定しました。
OPER_SET_IGNORE_OFF
	ユーザ無視処理は無効に設定しました。
OPER_SET_IGNORE_ERROR
	IGNOREの値はONかOFFにして下さい。
OPER_SET_READONLY_ON
	読み取り専用モードに移行します。
OPER_SET_READONLY_OFF
	読み書き（通常）モードに移行します。
OPER_SET_READONLY_ERROR
	READONLYの値はONかOFFにして下さい。
OPER_SET_DEBUG_ON
	デバッグ処理を有効に設定しました。
OPER_SET_DEBUG_OFF
	デバッグ処理を無効に設定しました。
OPER_SET_DEBUG_LEVEL
	デバッグ処理を有効（レベル%d）に設定しました。
OPER_SET_DEBUG_ERROR
	DEBUGの値は、ON、OFFまたは正数にして下さい。
OPER_SET_SUPASS_FAILED
	スーパーユーザパスワードの設定が失敗しました。
OPER_SET_SUPASS_OK
	スーパーユーザパスワードを設定しました。
OPER_SET_SUPASS_NONE
	スーパーユーザパスワードを消去しました。
OPER_SET_UNKNOWN_OPTION
	「%s」という設定名はありません。

# JUPE応答
OPER_JUPE_SYNTAX
	JUPE サーバ名 [理由]
OPER_JUPE_INVALID_NAME
	サーバ名は無効です。（ピリオド「.」が必要です）
OPER_JUPE_ALREADY_JUPED
	サーバ「%s」はすでにジュピターされています。

# RAW応答
OPER_RAW_SYNTAX
	RAW テキスト

# UPDATE応答
OPER_UPDATE_SYNTAX
	UPDATE [FORCE]
OPER_UPDATE_FORCE_FAILED
	データベースのロックを解除できませんでした。
OPER_UPDATING
	データベースを保存しています。
OPER_UPDATE_COMPLETE
	データベースの保存が完了しました。
OPER_UPDATE_FAILED
	データベースを保存できませんでした。

# REHASH応答
OPER_REHASHING
	環境設定ファイルを再読み込みしています。
OPER_REHASHED
	環境設定が正常に更新されました。
OPER_REHASH_ERROR
	環境設定ファイルの再読み込み中にエラーが発生しました。ログファイルを確認してください。

# KILLCLONES応答
OPER_KILLCLONES_SYNTAX
	KILLCLONES ニック
OPER_KILLCLONES_UNKNOWN_NICK
	ユーザ「%s」は見つかりません。
OPER_KILLCLONES_KILLED
	%sはユーザ「%s」を対象にKILLCLONESコマンドを実行し、%dクライアントが切断されました。
OPER_KILLCLONES_KILLED_AKILL
	%sはユーザ「%s」を対象にKILLCLONESコマンドを実行し、%dクライアントが切断されました。一時的な自動切断マスク「%s」を自動切断リストに追加しました。

###########################################################################
#
# セッション制限用メッセージ
#
###########################################################################

# EXCEPTION応答
OPER_EXCEPTION_SYNTAX
	EXCEPTION {ADD | DEL | CLEAR | MOVE | LIST | VIEW} [パラメータ]
OPER_EXCEPTION_ADD_SYNTAX
	EXCEPTION ADD [+有効期限] マスク 上限 理由
OPER_EXCEPTION_DEL_SYNTAX
	EXCEPTION DEL {マスク | リスト}
OPER_EXCEPTION_CLEAR_SYNTAX
	EXCEPTION CLEAR ALL
OPER_EXCEPTION_MOVE_SYNTAX
	EXCEPTION MOVE 番号 新番号
OPER_EXCEPTION_LIST_SYNTAX
	EXCEPTION {LIST | VIEW} [[+開始位置] パターン | リスト]
OPER_EXCEPTION_ALREADY_PRESENT
	マスク「%s」はすでにセッション制限例外リストに存在します。
OPER_EXCEPTION_TOO_MANY
	セッション制限例外リストがいっぱいですので追加できません。
OPER_EXCEPTION_ADDED
	「%s」(上限：%d) をセッション制限例外リストに追加しました。
OPER_EXCEPTION_MOVED
	「%s」を%d番から%d番に移動しました。
OPER_EXCEPTION_NO_SUCH_ENTRY
	%d番のエントリはセッション制限例外リストに存在しません。
OPER_EXCEPTION_NOT_FOUND
	「%s」はセッション制限例外リストに登録されていません。
OPER_EXCEPTION_NO_MATCH
	セッション制限例外リストに一致するマスクはありません。
OPER_EXCEPTION_EMPTY
	セッション制限例外リストは空です。
OPER_EXCEPTION_DELETED
	「%s」をセッション制限例外リストから削除しました。
OPER_EXCEPTION_DELETED_ONE
	セッション制限例外リストから1個のマスクを削除しました。
OPER_EXCEPTION_DELETED_SEVERAL
	セッション制限例外リストから%d個のマスクを削除しました。
OPER_EXCEPTION_CLEARED
	セッション制限例外リストをクリアしました。
OPER_EXCEPTION_LIST_HEADER
	セッション制限例外リスト：
# number, limit, host
OPER_EXCEPTION_LIST_COLHEAD
	番号  上限  ホスト
OPER_EXCEPTION_LIST_FORMAT
	 %3d  %4d  %s
# number, mask, set-by, set-time, expires, limit, reason
OPER_EXCEPTION_VIEW_FORMAT
	%d. %s
	　　　設定者：%s
	　　設定時刻：%s
	　　有効期限：%s
	　ユーザ上限：%d
	　　　　理由：%s
OPER_EXCEPTION_CHECK_SYNTAX
	EXCEPTION CHECK マスク
OPER_EXCEPTION_CHECK_NO_MATCH
	%sに一致するセッション制限例外マスクは見つかりませんでした。
OPER_EXCEPTION_CHECK_HEADER
	%sに一致するセッション制限例外マスク：
OPER_EXCEPTION_CHECK_TRAILER
	%dマスク見つかりました。
OPER_EXCEPTION_COUNT
	セッション制限例外リストには%d個のエントリが登録されています。
OPER_EXCEPTION_INVALID_LIMIT
	ユーザ上限設定が無効です。0以上、%d以下の数値を指定してください。
OPER_EXCEPTION_INVALID_HOSTMASK
	セッション制限例外マスクはホスト名のみのマスクで、ニック・ユーザ名を指定できません。マスクを確認してください。

# SESSION応答
OPER_SESSION_SYNTAX
	SESSION {LIST 下限 | VIEW ホスト名}
OPER_SESSION_LIST_SYNTAX
	SESSION LIST 下限
OPER_SESSION_VIEW_SYNTAX
	SESSION VIEW ホスト名
OPER_SESSION_INVALID_THRESHOLD
	ユーザ数下限設定は無効です。2以上の数値を指定してください。
OPER_SESSION_NOT_FOUND
	ホスト「%s」は見つかりません。
OPER_SESSION_LIST_HEADER
	%dユーザ以上のホスト：
OPER_SESSION_LIST_COLHEAD
	ユーザ数  ホスト名
OPER_SESSION_LIST_FORMAT
	  %6d  %s
OPER_SESSION_VIEW_FORMAT
	ホスト「%s」は現在%dユーザが接続しており、ユーザ上限は%dです。


###########################################################################
#
# ニュースシステム用メッセージ
#
###########################################################################

NEWS_LOGON_TEXT
	（ログオン案内・%s） %s
NEWS_OPER_TEXT
	（オペレータ案内・%s） %s

NEWS_LOGON_SYNTAX
	LOGONNEWS {ADD | DEL | LIST} [内容 | 番号]
NEWS_LOGON_ADD_SYNTAX
	LOGONNEWS ADD 内容
NEWS_LOGON_DEL_SYNTAX
	LOGONNEWS DEL {番号 | ALL}
NEWS_LOGON_LIST_HEADER
	ログオン案内リスト：
NEWS_LOGON_LIST_ENTRY
	%5d (%s・%s)
	    %s
NEWS_LOGON_LIST_NONE
	ログオン案内は登録されていません。
NEWS_LOGON_ADD_FULL
	ログオン案内リストがいっぱいです。
NEWS_LOGON_ADDED
	ログオン案内（%d番）を追加しました。
NEWS_LOGON_DEL_NOT_FOUND
	%d番目のログオン案内は存在しません。
NEWS_LOGON_DELETED
	%d番目のログオン案内を消去しました。
NEWS_LOGON_DEL_NONE
	ログオン案内は登録されていません。
NEWS_LOGON_DELETED_ALL
	ログオン案内を全て消去しました。

NEWS_OPER_SYNTAX
	OPERNEWS {ADD | DEL | LIST} [内容 | 番号]
NEWS_OPER_ADD_SYNTAX
	OPERNEWS ADD 内容
NEWS_OPER_DEL_SYNTAX
	OPERNEWS DEL {番号 | ALL}
NEWS_OPER_LIST_HEADER
	オペレータ案内リスト：
NEWS_OPER_LIST_ENTRY
	%5d (%s・%s)
	    %s
NEWS_OPER_LIST_NONE
	オペレータ案内は登録されていません。
NEWS_OPER_ADD_FULL
	オペレータ案内リストがいっぱいです。
NEWS_OPER_ADDED
	オペレータ案内（%d番）を追加しました。
NEWS_OPER_DEL_NOT_FOUND
	%d番目のオペレータ案内は存在しません。
NEWS_OPER_DELETED
	%d番目のオペレータ案内を消去しました。
NEWS_OPER_DEL_NONE
	オペレータ案内は登録されていません。
NEWS_OPER_DELETED_ALL
	オペレータ案内を全て消去しました。

NEWS_HELP_LOGON
	シンタクス：LOGONNEWS ADD 内容
	　　　　　　LOGONNEWS DEL {番号 | ALL}
	　　　　　　LOGONNEWS LIST
	
	ログオン案内を追加・削除・表示します。ログオン案内は、ユーザ
	がネットワークに接続する時に自動的に送信されますが、４件以上
	登録しても、最新の３件しか送られません。
	
	LOGONNEWS LIST（表示）は全てのIRCオペレータが使えますが、編
	集コマンド（ADDとDEL）の使用はServicesオペレータに限られてい
	ます。

NEWS_HELP_OPER
	シンタクス：OPERNEWS ADD text
	　　　　　　OPERNEWS DEL {num | ALL}
	　　　　　　OPERNEWS LIST
	
	オペレータ案内を追加・削除・表示します。オペレータ案内は、ユ
	ーザが/operコマンドでIRCオペレータ権限を取得した時に送信され
	ますが、４件以上登録しても、最新の３件しか送られません。
	
	OPERNEWS LIST（表示）は全てのIRCオペレータが使えますが、編集
	コマンド（ADDとDEL）の使用はServicesオペレータに限られていま
	す。


###########################################################################
#
# StatServ用メッセージ
#
###########################################################################

# General SERVERS responses
STAT_SERVERS_REMOVE_SERV_FIRST
	サーバ「%s」はネットワークに接続しています。切断してからやり直してください。
STAT_SERVERS_SERVER_EXISTS
	サーバ「%s」はすでに存在します。

# SERVERS command syntax
STAT_SERVERS_SYNTAX
	SERVERS オプション パラメータ

# STATS responses
STAT_SERVERS_STATS_TOTAL
	総サーバ数：%2d
STAT_SERVERS_STATS_ON_OFFLINE
	オンライン：%2d (%d%%)  オフライン：%2d (%d%%)
STAT_SERVERS_LASTQUIT_WAS
	最近の切断：%-30s  %s

# LIST responses
STAT_SERVERS_LIST_HEADER
	サーバ名                       ユーザ数   オペレータ数
STAT_SERVERS_LIST_FORMAT
	%-30s %3d (%2d%%)  %3d (%2d%%)
STAT_SERVERS_LIST_RESULTS
	LIST終了（%d/%dサーバを表示）。

# VIEW responses
STAT_SERVERS_VIEW_HEADER_ONLINE
	%-30s  (オンライン)
STAT_SERVERS_VIEW_HEADER_OFFLINE
	%-30s  (オフライン)
STAT_SERVERS_VIEW_LASTJOIN
	　　最近接続時：%s
STAT_SERVERS_VIEW_LASTQUIT
	　　最近切断時：%s
STAT_SERVERS_VIEW_QUITMSG
	切断メッセージ：%s
STAT_SERVERS_VIEW_USERS_OPERS
	　　　ユーザ数：%3d (%2d%%)  オペレータ数：%3d (%2d%%)
STAT_SERVERS_VIEW_RESULTS
	LIST終了（%d/%dサーバを表示）。

# DELETE responses
STAT_SERVERS_DELETE_SYNTAX
	SERVERS DELETE サーバ
STAT_SERVERS_DELETE_DONE
	サーバ「%s」の統計情報を削除しました。

# COPY responses
STAT_SERVERS_COPY_SYNTAX
	SERVERS COPY サーバ 新サーバ名
STAT_SERVERS_COPY_DONE
	サーバ「%s」の統計情報を「%s」にコピーしました。

# RENAME responses
STAT_SERVERS_RENAME_SYNTAX
	SERVERS RENAME サーバ 新サーバ名
STAT_SERVERS_RENAME_DONE
	サーバ「%s」の統計情報を「%s」に名前を変更しました。

# USERS responses
STAT_USERS_SYNTAX
	USERS STATS
STAT_USERS_TOTUSERS
	　　　　　　　　　全ユーザ数：%d
STAT_USERS_TOTOPERS
	　　　　　　　全オペレータ数：%d
STAT_USERS_SERVUSERS
	　　サーバ当たり平均ユーザ数：%d
STAT_USERS_SERVOPERS
	サーバ当たり平均オペレータ数：%d.%d


###########################################################################
#
# NickServ用ヘルプメッセージ
#
###########################################################################

NICK_HELP
	%Sは、ニック登録サービスです。このサービスを使ってニッ
	クを登録すれば、他のユーザが使用しないように%Sが監視し
	ます。%Sを制御するには、「/msg %S コマンド名」のように
	コマンドを使います。使用できるコマンドの一覧は
	「/msg %S HELP COMMANDS」で表示できます。また、特定の
	コマンドについては「/msg %S HELP コマンド名」でヘルプ
	情報を表示できます。
NICK_HELP_EXPIRES
	
	登録されたニックは、%sの間使用されないと、登録が解除されます。
NICK_HELP_WARNING
	
	注意：このサービスは、ユーザの身元を守るためのサービスです。
	ニックの盗用など、サービスの悪用は許されませんのでご注意くだ
	さい。

NICK_HELP_COMMANDS
	%Sでは、下記コマンドが使えます。
	
	    REGISTER     ニックを登録する
	    IDENTIFY     パスワード認証を行う
	    DROP         ニックの登録を解除する
NICK_HELP_COMMANDS_AUTH
	    AUTH         ニック登録時のメールアドレス確認を行う
	    SENDAUTH     アドレス確認のコードを再送する
	    REAUTH       パスワードを忘れた場合、確認コードを設定する
	    RESTOREMAIL  最後に有効確認されたメールアドレスを復元する
NICK_HELP_COMMANDS_LINK
	    LINK         他のニックをリンクする
	    UNLINK       ニックのリンクを解除する
	    LISTLINKS    リンクされているニックを表示する
NICK_HELP_COMMANDS_ACCESS
	    ACCESS       利用許可アドレスリストを編集する
NICK_HELP_COMMANDS_AJOIN
	    AJOIN        自動チャンネル参加リストを編集する
NICK_HELP_COMMANDS_SET
	    SET          ニック保護などの設定を変更する
	    UNSET        ニック関連情報を消去する
	    RECOVER      ニックを不正に使用しているユーザを切断する
	    RELEASE      RECOVERを使用後、ニックを再び利用可能にする
	    GHOST        「ゴースト」接続を切断する
	    INFO         ニック情報を表示する
NICK_HELP_COMMANDS_LIST
	    LIST         パターンに一致するニックの一覧を表示する
	    LISTEMAIL    メールアドレスでニックを検索する
NICK_HELP_COMMANDS_LISTCHANS
	    LISTCHANS    登録したチャンネルの一覧を表示する
	    STATUS       ニックの認証状況を調べる

NICK_HELP_REGISTER
	用法：%s
	
	現在使用しているニックを%Sのデータベースに登録します。
	ニックを登録後、SETやACCESSなどのコマンドでニックの設定を変
	更したり、ホームページアドレス（URL）などの関連情報を登録す
	ることができます。なお、登録および設定変更にはパスワードが必
	要ですので、登録で使ったパスワードをよく覚えておいてください。
	（パスワードでは大文字と小文字は区別されます）
NICK_HELP_REGISTER_EMAIL
	
	登録時に電子メールアドレスも同時に登録できます。登録しなかっ
	た場合でも、SET EMAILコマンドを使って後で登録できます。
NICK_HELP_REGISTER_EMAIL_REQ
	
	登録時に、電子メールアドレスも登録しなければなりません。ア
	ドレスを他人に知られたくない場合は、SET HIDEコマンドで隠す
	ことができます（詳細については「/msg %S HELP SET HIDE」
	を入力して下さい）。
NICK_HELP_REGISTER_EMAIL_AUTH
	
	登録時に、電子メールアドレスも登録しなければなりません。
	%Sは、このアドレスに確認コードを送信しますので、登録後、
	このコードをAUTHコマンドに使って登録を完了させてください。ア
	ドレスを他人に知られたくない場合は、SET HIDEコマンドで隠すこ
	とができます（詳細については「/msg %S HELP SET HIDE」
	を入力して下さい）。
NICK_HELP_REGISTER_END
	
	パスワードは、他人に分かりにくいものを選んで下さい。特に、
	お名前・ニック等のような当てやすいパスワードはおやめ下さい
	（ニックはパスワードとしては使えません）。４文字以下のパス
	ワードも攻撃に弱いため、５文字以上のパスワードにして下さい。
	日本語（仮名と漢字）をパスワードで使用することはできますが、
	端末によっては正常に登録されない場合もありますのでご注意下
	さい。

NICK_HELP_IDENTIFY
	用法：IDENTIFY パスワード
	
	%Sに対してパスワード認証を行います。登録時または
	SET PASSWORDコマンドで設定したパスワードを入力することで、ニ
	ックの所有者であることを%Sに証明でき、ニックの設定変更
	などが出来るようになります。

NICK_HELP_DROP
	用法：DROP パスワード
	
	ニックの登録を解除します。登録を解除したニックは、誰でも再度
	登録することが出来ます。
NICK_HELP_DROP_LINK
	
	また、ニックの登録を解除すると、リンクされているニックも全て
	登録が解除されます。リンクを一つだけ解除する場合はUNLINKコマ
	ンドをご利用ください。
NICK_HELP_DROP_END
	
	誤った登録解除を防ぐため、ニックのパスワードも入力しなければ
	なりません。

NICK_HELP_AUTH
	用法：AUTH コード
	
	ニックの登録やメールアドレスの変更後、メールアドレス確認を行
	います。メールアドレスが有効であることを確認するため、
	%Sは確認の数字コードを含めたメールをそのアドレス宛てに
	送信します。このコマンドで、メールに書いてあるコードを
	Servicesに通知することで、メールアドレスが有効であることが確
	認されます。

NICK_HELP_SENDAUTH
	用法：SENDAUTH
	
	REGISTERやSET EMAILで送信されたメールアドレス確認コードを再
	送します。確認用コードを記載したメールを無くしてしまった場合、
	このコマンドでもう一度送信することができます。
	
	悪用を防ぐため、このコマンドを24時間に一回しか使えません。

NICK_HELP_REAUTH
	用法：REAUTH
	
	パスワード代わりの確認コードを設定します。パスワードを忘れた
	場合、このコマンドで確認コードを設定し、メール送信された確認
	コードをAUTHコマンドで使うと、SET PASSWORDでパスワードを変更
	できるようになります。

NICK_HELP_RESTOREMAIL
	用法：RESTOREMAIL パスワード
	
	最後に有効確認されたメールアドレスを復元します。SET EMAILコ
	マンドで誤ったメールアドレスを指定してしまったとき、このコマ
	ンドを使えば、前のアドレスを復元して確認コードをクリアするこ
	とができ、再びパスワード認証できるようになります。このコマン
	ドを使うには、パスワードを入力しなければなりません。
	
	RESTOREMAILは、SET EMAILを使用した後、AUTHコマンドで確認コー
	ドを入力するまでのみ使えます。

NICK_HELP_LINK
	用法：LINK ニック
	
	現在使っているニックに新しいニック（対象ニック）をリンクしま
	す。リンクした後は、対象ニックで現在のニックと同じチャンネル
	権限が与えられ、メモも共有されます。
	
	リンクを解除する時は、UNLINKコマンドを使います。
	
	このコマンドを使うには、IDENTIFYコマンドで認証を行う必要があ
	ります。また、対象ニックが登録されている、または現在使用中の
	場合はリンクできません。

NICK_HELP_UNLINK
	用法：UNLINK ニック
	
	現在のニックへのリンクを解除します。リンクを解除したニックは、
	登録解除と同様に誰でも使うことが出来ます。現在のニックを削除
	する場合は、まず別のニックに変えなければなりません。
	
	SET MAINNICKコマンドで設定した「メインニック」のリンクを解除
	すると、メインニックは現在のニックに設定されます。

NICK_HELP_LISTLINKS
	用法：LISTLINKS
	
	現在のニックにリンクされている全てのニックを表示します。メイ
	ンニックには「*」が先頭に付きます。

NICK_HELP_ACCESS
	用法：ACCESS ADD マスク
	　　　ACCESS DEL マスク
	　　　ACCESS LIST
	
	ニックの利用許可アドレスリストを編集または表示します。IRCに
	接続する時、ユーザ名・ホスト名がこのリストに載っていれば、暫
	定的にニックの所有者とみなされ、自動切断（SET KILL）設定に関
	わらず切断されません。また、SECURE設定がOFFになっている場合
	はパスワード認証を行わずにチャンネル権限を得ることもできます。
	
	ADDとDELで指定するマスクに、ワイルドカード文字（* と ?）を使
	うこともできます。
	
	例：
	    ACCESS ADD achurch@*.dragonfire.net
	        dragonfire.netドメイン内のマシンから、ユーザachurch
	        に利用許可を与えます。
	
	    ACCESS DEL achurch@*.dragonfire.net
	        上記の例で追加したマスクを削除します。
	
	    ACCESS LIST
	        利用許可リストを表示します。

NICK_HELP_SET
	用法：SET 設定名 値
	
	ニックの設定を変更します。「設定名」には以下のものがあります。
	
	    PASSWORD   ニックの認証用パスワードを変更する
	    LANGUAGE   Servicesが応答する言語を変更する
	    URL        ニックにホームページアドレスを登録する
	    EMAIL      ニックに電子メールアドレスを登録する
	    INFO       INFOコマンドで表示されるテキストを登録する
	    KILL       自動切断設定を変更する
	    SECURE     セキュア設定を変更する
	    PRIVATE    ニックをLISTコマンドから隠す
	    NOOP       チャンネルアクセスリストへの登録を拒否する
	    HIDE       ニックの情報をINFOコマンドから隠す
	    TIMEZONE   時刻を表示する場合の時間帯を変更する
NICK_HELP_SET_OPTION_MAINNICK
	    MAINNICK   チャンネルアクセスリストに表示するニックを変更
NICK_HELP_SET_END
	
	このコマンドを使うには、IDENTIFYコマンドで認証を行う必要があ
	ります。
	
	それぞれの設定の詳しい情報を見る場合は
	「/msg %S HELP SET 設定名」を入力してください。

NICK_HELP_SET_PASSWORD
	用法：SET PASSWORD 新パスワード
	
	ニックの認証用パスワードを変更します。

NICK_HELP_SET_LANGUAGE
	用法：SET LANGUAGE 番号
	
	Servicesが応答する言語を設定します。numberの選択肢は以下
	の通りです。
	

NICK_HELP_SET_URL
	用法：SET URL アドレス
	
	ホームページアドレスをニック情報に追加します。このアドレスは
	INFOコマンドで表示されます。

NICK_HELP_SET_EMAIL
	用法：SET EMAIL アドレス
	
	電子メールアドレスをニック情報に追加します。このアドレスは
	INFOコマンドで表示されます。

NICK_HELP_SET_INFO
	用法：SET INFO テキスト
	
	指定されたテキストをニック情報に追加します。このテキストは
	INFOコマンドで表示されます。

NICK_HELP_SET_KILL
	用法：SET KILL {ON | QUICK | IMMED | OFF}
	
	ニックの自動切断設定を変更します。ONにすると、他のユーザがニ
	ックを使おうとした時、%Sが警告し、１分以内にニックを変えない
	と切断されます。
	
	QUICKにすると、ニックを変える時間は20秒に短縮されます。
	
	IMMEDにすると、ユーザは接続してすぐに切断されます。（但し、
	IRCネットワークの管理者の設定により、IMMEDを使えない場合が
	あります）

NICK_HELP_SET_SECURE
	用法：SET SECURE {ON | OFF}
	
	セキュア設定を変更します。ONにすると、ユーザアドレスが利用許
	可アドレスリストに入っていても、IDENTIFYでパスワード認証を行
	わない限りチャンネル権限が与えられません。

NICK_HELP_SET_PRIVATE
	用法：SET PRIVATE {ON | OFF}
	
	プライベート設定を変更します。ONにすると、ニックはLISTコマン
	ドの結果には表示されません。（ただ、INFOコマンドは通常通り情
	報を表示します）

NICK_HELP_SET_NOOP
	用法：SET NOOP {ON | OFF}
	
	チャンネルアクセス拒否設定を変更します。ONにすると、ニックは
	チャンネルのアクセスリストに登録されなくなり、登録しようとし
	た場合はエラーになります。（ただ、既に登録されているチャンネ
	ルからは消去されません）

NICK_HELP_SET_HIDE
	用法：SET HIDE {EMAIL | USERMASK | QUIT} {ON | OFF}
	
	ニックの情報をINFOコマンドから隠します。隠せるのは、電子メー
	ルアドレス（EMAIL）、前回のユーザアドレス（USERMASK）、前回
	の切断メッセージ（QUIT）の３項目です。２番目のパラメータは、
	ONにすると情報を隠し、OFFにすると情報を表示します。

NICK_HELP_SET_TIMEZONE
	用法：SET TIMEZONE {UTCオフセット | 時間帯名 | DEFAULT}
	
	INFOコマンド等で表示される時刻を、どの時間帯で表示するか設定
	します。未設定の場合は、Servicesが動作しているサーバの時間帯
	で表示します。時間帯の設定方法は
	
	　・UTCオフセット（世界標準時刻との差）
	　・時間帯名（「JST」など、但し認識できないものもある）
	　・「DEFAULT」（未設定状態に戻す）
	
	の３通りです。UTCオフセットで指定する場合、「+」または「-」
	に続き、時間差を時間数または「hh:mm」形式で指定しま
	す（下記参照）。
	
	注意：サマータイムのある時間帯では、Servicesは自動調整を行い
	　　　ません。サマータイムが開始・終了した時に手動で設定して
	　　　ください。
	
	例：
	
	    SET TIMEZONE -5
	        世界標準時刻より５時間遅い（西）時間帯に設定します。
	
	    SET TIMEZONE +3:30
	        世界標準時刻より３時間半早い（東）時間帯に設定します。
	
	    SET TIMEZONE JST
	        時間帯を日本時間（JST）に設定します。

NICK_HELP_SET_MAINNICK
	用法：SET MAINNICK ニック
	
	複数のニックをリンクしている場合、チャンネルのアクセスリスト
	で表示されるニックを設定します。このニックのリンクが解除され
	た場合、別のニックが自動的に選択されます。

NICK_HELP_UNSET
	用法：UNSET {URL | EMAIL | INFO}
	
	ニックのホームページアドレス（URL）、メールアドレス（EMAIL）、
	またはテキスト情報（INFO）を消去します。

NICK_HELP_UNSET_REQ_EMAIL
	用法：UNSET {URL | INFO}
	
	ニックのホームページアドレス（URL）またはテキスト情報（INFO）
	を消去します。

NICK_HELP_RECOVER
	用法：RECOVER ニック [パスワード]
	
	他のユーザがあなたのニックを使っている時、そのニックを強制的
	に取り戻します。自動切断設定が有効なニックを使った場合と同様
	に、ユーザがIRCから切断されます。なおユーザが切断された後は、
	同じニックでの再接続を防ぐために、Servicesは「疑似ユーザ」で
	%s間、そのニックを保留します。保留時間が過ぎる前にニックを
	利用したい場合はRELEASEコマンドで保留を解除できます。
	
	このコマンドを使うには、ニックの認証用パスワードを入力するか、
	ニックにリンクされている別のニックに対して認証を行う必要があ
	ります。

NICK_HELP_RELEASE
	用法：RELEASE ニック [パスワード]
	
	RECOVERコマンド（または自動切断）によってニックがServicesに
	保留されている場合、保留を解除します。RELEASEを使わない場合、
	保留は%s後に自動解除されます。
	
	このコマンドを使うには、ニックの認証用パスワードを入力するか、
	ニックにリンクされている別のニックに対して認証を行う必要があ
	ります。

NICK_HELP_GHOST
	用法：GHOST ニック [パスワード]
	
	「ゴースト」ユーザを切断します。「ゴースト (幽霊)」ユーザは、
	実際には切断しているのに、IRCに残っているユーザのことで、イ
	ンターネット回線が切れたりパソコンが固まったりした時に発生し
	ます。
	
	このコマンドを使うには、ニックの認証用パスワードを入力するか、
	ニックにリンクされている別のニックに対して認証を行う必要があ
	ります。

NICK_HELP_INFO
	用法：INFO ニック [ALL]
	
	ニックの情報（所有者の名前・ユーザアドレス、前回使用した日時、
	オプションの設定など）を表示します。パスワード認証を行ったニ
	ックの場合は、ALLを追加することで、SET HIDE設定に関わらず全
	ての情報が表示されます。
NICK_HELP_INFO_AUTH
	
	メールアドレスの確認ができていないニックについては、メールア
	ドレスニックの所有者とServicesアドミニストレータにしか表示さ
	れません。

NICK_HELP_LISTCHANS
	用法：LISTCHANS
	
	あなたのニックで登録されているチャンネルを表示します。

NICK_HELP_LIST
	用法：LIST [+開始位置] パターン
	
	ニックと前回ユーザアドレスのどちらかが指定されたパターンに一
	致するニックの一覧を表示します。PRIVATE設定が有効になってい
	るニックは表示されません。開始位置で、最初に表示するレコード
	を指定できます（例えば、「+50」を指定すると最初の50個の該当
	レコードを飛ばし、51個目から表示します）。
	
	例：
	    LIST joeuser@foo.com
	        前回のユーザアドレスがjoeuser@foo.comであるニックを
	        表示します。
	    LIST *Bot*!*@*
	        「Bot」を含むニックを表示します。（大文字・小文字は
	        区別せず）
	    LIST *@*.bar.org
	        前回のユーザアドレスがbar.orgドメイン内のニックを表
	        示します。
NICK_HELP_LIST_OPERSONLY
	
	このコマンドの使用はIRCオペレータに制限されています。

NICK_HELP_LISTEMAIL
	用法：%s
	
	電子メールアドレスが指定されたパターンに一致するニックを表示
	します。詳しくはLISTコマンドの情報をご覧ください。
	（/msg %S HELP LIST）

NICK_HELP_STATUS
	用法：STATUS ニック...
	
	指定されたニックの認証状況を表示します。応答は１行に１ニック
	で、ニックの後に空白と状況を示す数字が付加されます。数字の意
	味は以下の通りです。
	    ・0: ニックが使用されていないか、未登録
	    ・1: 認証されていない
	    ・2: 利用許可リストに入っているが、パスワード認証されて
	         いない
	    ・3: パスワード認証されている
	１回で指定できるニックは16個までで、17個目以降は無視されます。

NICK_HELP_AJOIN
	用法：AJOIN ADD チャンネル
	　　　AJOIN DEL チャンネル
	　　　AJOIN LIST
	
	ニックの自動参加リストを編集または表示します。このリストに追
	加されたチャンネルは、ニック認証を行う時に自動的に参加させら
	れます。ただし追加できるのは、名前が「#」で始まるチャンネル
	のみです（ローカルチャンネルを追加できません）。
	
NICK_HELP_AJOIN_END
	また、IRCサーバによってチャンネル参加を拒否されることがあり
	ます（キーを設定されたチャンネルや、インバイト専用チャンネル
	など）。
NICK_HELP_AJOIN_END_CHANSERV
	また、IRCサーバによってチャンネル参加を拒否されることがあり
	ます（キーを設定されたチャンネルや、インバイト専用チャンネル
	など）。ただ、インバイト専用チャンネルで、%sに登録され、
	そのチャンネルでINVITEコマンドを使う権利がある場合は、
	%Sが自動的にチャンネルに誘います。


NICK_OPER_HELP_COMMANDS
	
	Servicesアドミニストレータは次のコマンドも使えます。
	
	    DROPNICK     任意のニックの登録を解除する
NICK_OPER_HELP_COMMANDS_DROPEMAIL
	    DROPEMAIL    指定されたメールアドレスを持つニックをすべて
	                 　　登録解除する
NICK_OPER_HELP_COMMANDS_GETPASS
	    GETPASS      ニックのパスワードを表示する
	                 　　（暗号化が使われていない場合のみ）
NICK_OPER_HELP_COMMANDS_FORBID
	    FORBID       ニックを使用禁止にする
	    SUSPEND      ニックを使用停止にする
	    UNSUSPEND    ニックの使用停止を解除する
NICK_OPER_HELP_COMMANDS_SETAUTH
	    SETAUTH      ニックのメール確認コードを設定する
	    GETAUTH      ニックのメール確認コードを表示する
	    CLEARAUTH    ニックのメール確認コードを消去する
NICK_OPER_HELP_COMMANDS_END
	
	なお、ServicesアドミニストレータはSET、ACCESS LIST、INFO ALL
	の各コマンドをどのニックに対しても使用できます。詳しくは当該
	コマンドのヘルプ情報をご覧ください。

NICK_OPER_HELP_DROPNICK
	用法：DROPNICK ニック
	
	指定されたニックの登録を解除します。指定ニックに他のニックが
	リンクされている場合、リンクも解除します。
	
	このコマンドの使用はServicesアドミンに制限されています。

NICK_OPER_HELP_DROPEMAIL
	用法：DROPEMAIL パターン
	　　　DROPEMAIL-CONFIRM パターン
	
	指定されたパターンに電子メールアドレスが一致する全てのニック
	の登録を解除します。パターンが「-」（半角のハイフン）である
	場合、メールアドレスが設定されていないニックの登録を解除しま
	す。
	
	誤った登録解除を防止するため、このコマンドは２段階で実行しま
	す。まずはDROPEMAILコマンドでパターンを指定し、該当するニッ
	クの数を確認してから、DROPEMAIL-CONFIRMコマンドで実際に登録
	解除を行います。DROPEMAIL-CONFIRMは、DROPEMAILの実行から%s
	以内、かつ同じユーザによって実行されなければなりません。なお、
	DROPEMAILコマンドが５つ以上連続で受け付けた場合は一番古いも
	のから送信者・該当ニック情報が消え、時間内でも
	DROPEMAIL-CONFIRMコマンドが使えなくなります。
	
	このコマンドの使用はServicesアドミンに制限されています。

NICK_OPER_HELP_SET
	
	Servicesアドミンは、「NOEXPIRE」（登録の自動解除の有無）設定
	も変更できます。また、他のニックの設定変更もできます。
	（用法：SET !ニック option parameters）

NICK_OPER_HELP_SET_NOEXPIRE
	用法：SET NOEXPIRE [!ニック] {ON | OFF}
	
	指定されたニック（指定のない場合は現在使っているニック）の登
	録が自動的に解除されるかどうかを設定します。
	
	このコマンドの使用はServicesアドミンに制限されています。

NICK_OPER_HELP_UNSET
	
	Servicesアドミンは、SETコマンド同様、設定名の前に「!」とニッ
	クを入れることで他のニックに対してもUNSETコマンドを使えます。
	（UNSET !ニック 設定名）

NICK_OPER_HELP_OLD_UNLINK
	
	Servicesアドミンは、パスワードを入力せずに他のニックのリンク
	も解除できます。

NICK_OPER_HELP_UNLINK
	用法：UNLINK ニック [FORCE]
	
	現在のニックへのリンクを解除します。リンクを解除したニックは、
	登録解除と同様に誰でも使うことが出来ます。現在のニックを削除
	する場合は、まず別のニックに変えなければなりません。
	
	SET MAINNICKコマンドで設定した「メインニック」のリンクを解除
	すると、メインニックは現在のニックに設定されます。
	
	Servicesアドミンは、FORCEオプションを追加することで自分以外
	のニックへのリンクを解除できます。

NICK_OPER_HELP_OLD_LISTLINKS
	用法：LISTLINKS ニック
	
	指定されたニックにリンクされているニックを表示します。
	
	このコマンドの使用はServicesアドミンに制限されています。

NICK_OPER_HELP_LISTLINKS
	用法：LISTLINKS [ニック]
	
	現在のニックにリンクされている全てのニックを表示します。メイ
	ンニックには「*」が先頭に付きます。Servicesオペレータは、
	ニックを指定することで自分以外のニックのリンクも表示できます。

NICK_OPER_HELP_ACCESS
	
	Servicesアドミンは、ニックを指定することで自分以外のニックの
	利用許可リストを表示できます。

NICK_OPER_HELP_INFO
	
	Servicesアドミンは、どのニックに対してもALLを指定できます。

NICK_OPER_HELP_LISTCHANS
	
	Servicesアドミンは、ニックを追加することで自分以外のニックの
	登録チャンネルを表示できます。
	（例：/msg %S LISTCHANS OtherNick）

NICK_OPER_HELP_LIST
	用法：LIST [+開始位置] パターン [FORBIDDEN] [NOEXPIRE] [SUSPENDED]
	
	ニックと前回ユーザアドレスのどちらかが指定されたパターンに一
	致するニックの一覧を表示します。PRIVATE設定が有効になってい
	るニックはServicesアドミンにしか表示されません。
	
	Servicesアドミンが使う場合は、ニックの先頭に、NOEXPIRE設定が
	有効なニックは「!」が、使用停止になっているニックは「*」が付
	きます。
	
	「FORBIDDEN」「SUSPENDED」「NOEXPIRE」の各オプションを指定し
	た場合、表示されるニックはそれぞれ使用禁止、使用停止、
	NOEXPIRE設定有効なものに限られます。複数併せて使うことも出来
	ます。これらのオプションを使えるのはServicesアドミンのみです。
NICK_OPER_HELP_LIST_AUTH
	用法：LIST [+開始位置] パターン [FORBIDDEN] [NOEXPIRE] [SUSPENDED] [NOAUTH]
	
	ニックと前回ユーザアドレスのどちらかが指定されたパターンに一
	致するニックの一覧を表示します。PRIVATE設定が有効になってい
	るニックはServicesアドミンにしか表示されません。
	
	Servicesアドミンが使う場合は、ニックの先頭に、NOEXPIRE設定が
	有効なニックは「!」が、使用停止になっているニックは「*」が、
	メールアドレス確認（AUTHコマンド）が済んでいないニックは「?」
	が付きます。
	
	「FORBIDDEN」「SUSPENDED」「NOEXPIRE」「NOAUTH」の各オプショ
	ンを指定した場合、表示されるニックはそれぞれ使用禁止、使用停
	止、NOEXPIRE設定有効、メールアドレス未確認のものに限られます。
	複数併せて使うことも出来ます。これらのオプションを使えるのは
	Servicesアドミンのみです。
NICK_OPER_HELP_LIST_END
	
	開始位置で、最初に表示するレコードを指定できます（例えば、
	「+50」を指定すると最初の50個の該当レコードを飛ばし、51個目
	から表示します）。
	
	例：
	    LIST joeuser@foo.com
	        前回のユーザアドレスがjoeuser@foo.comであるニックを
	        表示します。
	    LIST *Bot*!*@*
	        「Bot」を含むニックを表示します。（大文字・小文字は
	        区別せず）
	    LIST * NOEXPIRE
	        NOEXPIRE設定が有効になっているニックを全て表示します。

NICK_OPER_HELP_GETPASS
	用法：GETPASS ニック
	
	指定されたニックのパスワードを表示します。ただし、暗号化が利
	用されている場合は、このコマンドを使えません。
	
	このコマンドが使われた時、使用したユーザのニックとパスワード
	を表示されたニックがWALLOPS/GLOBOPSで全オペレータに周知され、
	ログされます。
	
	このコマンドの使用はServicesアドミンに制限されています。

NICK_OPER_HELP_FORBID
	用法：FORBID ニック
	
	ニックを使用禁止にします。解除にはDROPNICKコマンドを使います。
	
	このコマンドの使用はServicesアドミンに制限されています。

NICK_OPER_HELP_SUSPEND
	用法：SUSPEND [+期限] ニック 理由
	
	指定されたニックとそれにリンクされているニックを使用停止にし
	ます。使用停止中は、ニックの設定は保存されますが、使用するこ
	とはできません。使用停止はUNSUSPENDコマンドで解除できます。
	期限は%sのAKILLコマンドと同様に解釈されます。
	
	このコマンドの使用はServicesアドミンに制限されています。

NICK_OPER_HELP_UNSUSPEND
	用法：UNSUSPEND ニック
	
	SUSPENDコマンドで使用停止にしたニックを通常の状態に戻し、使
	用を許可します。
	
	このコマンドの使用はServicesアドミンに制限されています。

NICK_OPER_HELP_AJOIN
	
	Servicesアドミンは、ニックを追加することで自分以外のニックの
	自動参加リストを表示できます。
	（例：/msg %S AJOIN LIST OtherNick）

NICK_OPER_HELP_SETAUTH
	用法：SETAUTH ニック
	
	指定されたニックにメールアドレス確認コードを新たに設定します。
	ニックの所有者がAUTHコマンドでメールアドレス確認を行うまでニ
	ック認証ができなくなります。ニックが使用中の場合、利用者にコ
	ードが設定されたことを通知し、認証状況を取り消します。
	
	このコマンドの使用はServicesアドミンに制限されています。

NICK_OPER_HELP_GETAUTH
	用法：GETAUTH ニック
	
	指定されたニックのメールアドレス確認コードを表示します。
	
	このコマンドの使用はServicesアドミンに制限されています。

NICK_OPER_HELP_CLEARAUTH
	用法：CLEARAUTH ニック
	
	指定されたニックのメールアドレス確認コードをクリアし、ニック
	認証を許可します。
	
	このコマンドの使用はServicesアドミンに制限されています。

###########################################################################
#
# ChanServのヘルプメッセージ
#
###########################################################################

# OP/VOICE等で使われる文字列
CHAN_HELP_REQSOP_LEVXOP
	レベル100（SOP）
CHAN_HELP_REQSOP_LEV
	レベル100
CHAN_HELP_REQSOP_XOP
	SOP
CHAN_HELP_REQAOP_LEVXOP
	レベル50（AOP）
CHAN_HELP_REQAOP_LEV
	レベル50
CHAN_HELP_REQAOP_XOP
	AOP
CHAN_HELP_REQHOP_LEVXOP
	レベル40（HOP）
CHAN_HELP_REQHOP_LEV
	レベル40
CHAN_HELP_REQHOP_XOP
	HOP
CHAN_HELP_REQVOP_LEVXOP
	レベル30（VOP）
CHAN_HELP_REQVOP_LEV
	レベル30
CHAN_HELP_REQVOP_XOP
	VOP

CHAN_HELP
	%Sは、チャンネル登録サービスです。このサービスを使って
	チャンネルを登録すれば、チャンネルの状態を制御したり、自動的
	にオペレータ権を与えたりできるようになります。%Sを制御
	するには、「/msg %S コマンド名」のようにコマンドを使い
	ます。使用できるコマンドの一覧は「/msg %S HELP COMMANDS」
	で表示できます。また、特定のコマンドについては
	「/msg %S HELP コマンド名」でヘルプ情報を表示できます。
CHAN_HELP_EXPIRES
	
	注意：登録されたチャンネルは、%s間使用されないと、登録が
	　　　解除されます。

CHAN_HELP_COMMANDS
	%Sでは、下記コマンドが使えます。
	
	    REGISTER   チャンネルを登録する
	    IDENTIFY   パスワード認証を行う
	    DROP       チャンネルの登録を解除する
	    SET        チャンネル設定や情報を変更する
	    UNSET      チャンネル関連情報を消去する
	    INFO       チャンネル情報を表示する
CHAN_HELP_COMMANDS_LIST
	    LIST       パターンに一致するチャンネル一覧を表示する
CHAN_HELP_COMMANDS_AKICK
	    AKICK      自動キックリストを編集する
CHAN_HELP_COMMANDS_LEVELS
	    ACCESS     アクセスリストを編集する
	    LEVELS     チャンネル権限の設定を行う
CHAN_HELP_COMMANDS_XOP
	    SOP        SOPリストを編集する
	    AOP        AOPリストを編集する
CHAN_HELP_COMMANDS_HOP
	    HOP        HOPリストを編集する
CHAN_HELP_COMMANDS_XOP_2
	    VOP        VOPリストを編集する
	    NOP        NOPリストを編集する
CHAN_HELP_COMMANDS_OPVOICE
	    OP         オペレータ権を与える
	    DEOP       オペレータ権を解除する
	    VOICE      発言権を与える
	    DEVOICE    発言権を解除する
CHAN_HELP_COMMANDS_HALFOP
	    HALFOP     半オペレータ権を与える
	    DEHALFOP   半オペレータ権を解除する
CHAN_HELP_COMMANDS_PROTECT
	    PROTECT    ユーザをプロテクト状態にする
	    DEPROTECT  ユーザを非プロテクト状態にする
CHAN_HELP_COMMANDS_INVITE
	    INVITE     招待専用チャンネルに入る
	    UNBAN      自分に対する追放マスクを解除する
	    KICK       ユーザをチャンネルからキックする
	    TOPIC      チャンネルのトピックを変更する
	    CLEAR      チャンネルモードをクリア、全ユーザのキックを行う
	    STATUS     ユーザのアクセスレベルを表示する

CHAN_HELP_REGISTER
	用法：REGISTER チャンネル パスワード 説明
	
	チャンネルを%Sのデータベースに登録します。登録するには、
	チャンネルのオペレータ権を持っていなければなりません。パスワ
	ードは設定変更などのために必要です。「説明」は、チャンネルが
	何のために使われるか、どういう会話をするかなどの説明で、必須
	です。
	
	チャンネルを登録した人は、そのチャンネルの「創設者」となりま
	す。チャンネルの創設者はチャンネルの全ての設定を変更できるほ
	か、チャンネルに参加する時は自動的にオペレータ権を与えられま
	す。
	
	注意：チャンネルを登録するには、まずニックを登録する必要があ
	　　　ります。登録手続きについては「/msg %s HELP」を入
	　　　力してください。
CHAN_HELP_REGISTER_ADMINONLY
	
	このコマンドの使用はServicesアドミンに制限されています。

CHAN_HELP_IDENTIFY
	用法：IDENTIFY チャンネル パスワード
	
	%Sに対してパスワード認証を行います。登録時または
	SET PASSWORDコマンドで設定したパスワードを入力することで、チ
	の創設者であることを%Sに証明でき、SETやDROPコマンドを使える
	ようになります。なお、創設者のニックを使用していない時は、こ
	のコマンドを使えば創設者の権利が得られます。

CHAN_HELP_DROP
	用法：DROP チャンネル パスワード
	
	チャンネルの登録を解除します。このコマンドの使用はチャンネル
	の創設者に限られており、誤用を防ぐため、パスワードも入力しな
	ければなりません。

CHAN_HELP_SET
	用法：SET チャンネル 設定名 値
	
	チャンネルの設定を変更します。「設定名」には以下のものがあり
	ます。
	
	    FOUNDER     チャンネルの創設者を変更する
	    SUCCESSOR   チャンネルの後継者を登録・変更する
	    PASSWORD    チャンネルの認証用パスワードを変更する
	    DESC        チャンネルの説明を変更する
	    URL         チャンネルにホームページアドレスを登録する
	    EMAIL       チャンネルにメールアドレスを登録する
	    ENTRYMSG    チャンネルの参加時メッセージを設定する
	    KEEPTOPIC   トピック記録設定を変更する
	    TOPICLOCK   トピック変更禁止設定を変更する
	    MLOCK       チャンネルモードをON/OFFにロックする
	    PRIVATE     チャンネルをLISTコマンドから隠す
	    RESTRICTED  チャンネルへのアクセスを制限する
	    SECURE      セキュア設定を変更する
	    SECUREOPS   オペレータ権の管理レベルを設定する
	    LEAVEOPS    最初参加ユーザのオペレータ権を解除しない
	    OPNOTICE    OP/VOICE等のコマンド使用をチャンネルに周知する
	    ENFORCE     自動オペレータ権・自動発言権管理設定を変更する
	    MEMO-RESTRICTED
	                チャンネルメモ送信を制限する
	
	各オプションの詳細については、「/msg %S HELP SET
	コマンド名」でヘルプ情報を表示できます。
	
	このコマンドの使用はデフォルトでチャンネルの創設者に限られて
	おり、使用の前に%SのIDENTIFYコマンドでチャンネル認証を
	行わなければなりません。

CHAN_HELP_SET_FOUNDER
	用法：SET チャンネル FOUNDER ニック
	
	チャンネルの創設者を変更します。創設者として指定するニックは
	登録されていなければなりません。

CHAN_HELP_SET_SUCCESSOR
	用法：SET チャンネル SUCCESSOR ニック
	
	チャンネルの後継者を設定または変更します。後継者は、創設者のニ
	ックの登録が解除された場合にチャンネルを受け継いで創設者になり
	ます（後継者が設定されていない状態で創設者のニックの登録が解除
	されると、チャンネルの登録も同時に解除されてしまいます）。但し、
	受け継ぐべき時点で後継者のチャンネル登録数が上限（%d）を超えて
	いる場合は、チャンネルを受け継ぐことができず、チャンネルの登録
	が解除されます。
	
	後継者に設定するニックは登録されていなければなりません。

CHAN_HELP_SET_PASSWORD
	用法：SET チャンネル PASSWORD パスワード
	
	チャンネル認証用パスワードを変更します。

CHAN_HELP_SET_DESC
	用法：SET チャンネル DESC 説明
	
	チャンネルの説明を変更します。説明はLIST・INFOコマンドで表示
	されます。

CHAN_HELP_SET_URL
	用法：SET チャンネル URL アドレス
	
	ホームページアドレスをチャンネル情報に追加します。このアドレ
	スはINFOコマンドで表示されます。

CHAN_HELP_SET_EMAIL
	用法：SET チャンネル EMAIL アドレス
	
	電子メールアドレスをチャンネル情報に追加します。このアドレス
	はINFOコマンドで表示されます。

CHAN_HELP_SET_ENTRYMSG
	用法：SET チャンネル ENTRYMSG メッセージ
	
	チャンネルの参加時メッセージを設定します。このメッセージは、
	ユーザがチャンネルに参加した時に、%Sから自動送信されま
	す。

CHAN_HELP_SET_KEEPTOPIC
	用法：SET チャンネル KEEPTOPIC {ON | OFF}
	
	トピック記録設定を変更します。ONにすると、チャンネルが使用
	されていない時でもトピックが記録され、再びユーザが参加した
	時に復元されます。

CHAN_HELP_SET_TOPICLOCK
	用法：SET チャンネル TOPICLOCK {ON | OFF}
	
	トピック変更禁止設定を変更します。ONにすると、TOPICコマンド
	以外でのトピックの変更が禁止され、変更された場合は元のトピッ
	クに戻します。

CHAN_HELP_SET_MLOCK
	用法：SET チャンネル MLOCK モード
	
	チャンネルのモードロックを設定します。モードロックは、チャン
	ネルのバイナリモード（m,n,s,t等）を有効（+）または無効（-）
	にロックすることができます。「モード」はIRCの「/MODE」コマン
	ドと同じ形式で指定します（「+」に続くモードは有効に、「-」に
	続くモードは無効にロックされます）。
	
	注意：キー（+k）をロックする場合、RESTRICTED設定を有効にする
	　　　ことをお勧めします。RESTRICTEDを有効にしないと、チャン
	　　　ネル未使用時にユーザが入ると、キーが見えてしまいます。
	　　　インバイト専用（+i）も、最初に参加するユーザに対しては
	　　　効果を発揮しませんので、同様にRESTRICTEDの使用を推奨し
	　　　ます。
	
	例：
	    SET #channel MLOCK +nt-iklps
	       nとtの両モードを有効に、i,k,l,p,sの各モードを無効にロ
	       ックします。mモードはロックされません。
	
	    SET #channel MLOCK +knst my-key
	       k,n,s,tの各モードを有効にロックします。キーは「my-key」
	       にロックします。
	
	    SET #channel MLOCK +
	       モードロックを解除します。

CHAN_HELP_SET_HIDE
	用法：SET チャンネル HIDE {EMAIL | TOPIC | MLOCK} {ON | OFF}
	
	チャンネルの情報をINFOコマンドから隠します。隠せるのは、電子
	メールアドレス（EMAIL）、トピック（TOPIC）、モードロック
	（MLOCK）の３項目です。２番目のパラメータは、ONにすると情報
	を隠し、OFFにすると情報を表示します。
	
	チャンネルのモードロックで+pまたは+sがロックされている場合、
	この設定に関わらずトピックは表示されません。

CHAN_HELP_SET_PRIVATE
	用法：SET チャンネル PRIVATE {ON | OFF}
	
	プライベート設定を変更します。ONにすると、チャンネルはLISTコ
	マンドの結果には表示されません。（ただ、INFOコマンドは通常通
	り情報を表示します）

CHAN_HELP_SET_RESTRICTED
	用法：SET チャンネル RESTRICTED {ON | OFF}
	
	アクセス制限設定を変更します。ONにすると、アクセスリストに登
	録されていないユーザ及びオペレータ権を禁止されているユーザは
	チャンネルに参加できなくなります。（参加しようとした時は、
	%Sによりキック・追放されます）

CHAN_HELP_SET_SECURE
	用法：SET チャンネル SECURE {ON | OFF}
	
	セキュア設定を変更します。ONにすると、%sに対してニック認証を
	行っていないユーザは自動オペレータ権などの権限を与えられず、
	アクセス制限（RESTRICTED）設定が有効になっている場合は、チャ
	ンネルに参加することも出来なくなります。

CHAN_HELP_SET_SECUREOPS
	用法：SET チャンネル SECUREOPS {ON | OFF}
	
	オペレータ権制限設定を変更します。ONにすると、アクセスリスト
	に登録されていないユーザはオペレータ権を得られなくなり、他の
	オペレータからオペレータ権を与えられても、%Sにより解除
	されます。

CHAN_HELP_SET_LEAVEOPS
	用法：SET チャンネル LEAVEOPS {ON | OFF}
	
	オペレータ権放置設定を変更します。IRCでは、最初に参加したユ
	ーザが自動的にオペレータ権を与えられますが、Servicesのチャン
	ネル管理を正しく行うため、このオペレータ権を通常、%Sが
	解除します。この設定をONにすると、オペレータ権をそのまま放置
	します（ただしSECUREOPS設定は優先されます）。

CHAN_HELP_SET_OPNOTICE
	用法：SET チャンネル OPNOTICE {ON | OFF}
	
	OPやDEOPのチャンネル権限与奪コマンドの使用周知設定を変更しま
	す。ONにすると、これらのコマンドが使用された時、%Sが
	/NOTICE形式でチャンネルに周知します。

CHAN_HELP_SET_ENFORCE
	用法：SET チャンネル ENFORCE {ON | OFF}
	
	自動オペレータ権・自動発言権などの管理設定を変更します。ONに
	すると、自動オペレータ権・自動発言権を持つユーザに対してDEOP
	やDEVOICEコマンドを使用できなくなり、/MODEコマンドで解除され
	た場合でも%Sが再度権限を与えます。

CHAN_HELP_SET_MEMO_RESTRICTED
	用法：SET チャンネル MEMO-RESTRICTED {ON | OFF}
	
	メモ送信制限設定を変更します。ONにすると、チャンネルメモ受信
	権限を持たないユーザはチャンネルにメモを送れなくなります。

CHAN_HELP_UNSET
	用法：UNSET チャンネル {SUCCESSOR | URL | EMAIL | ENTRYMSG}
	
	指定されたチャンネル設定を消去します。パラメータの意味はSET
	同様で、以下の通りです。
	
	    SUCCESSOR   チャンネル後継者
	    URL         ホームページアドレス
	    EMAIL       電子メールアドレス
	    ENTRYMSG    参加時メッセージ

CHAN_HELP_SOP
	用法：SOP チャンネル ADD ニック
	　　　SOP チャンネル DEL ニック
	　　　SOP チャンネル LIST [+開始位置] [パターン]
	　　　SOP チャンネル COUNT
	
CHAN_HELP_SOP_MID1
	SOP（スーパーオペレータ）リストを編集または表示します。SOPリ
	ストに登録されたユーザはチャンネル参加時に自動的にオペレータ
	権を与えられます。
CHAN_HELP_SOP_MID1_CHANPROT
	SOP（スーパーオペレータ）リストを編集または表示します。SOPリ
	ストに登録されたユーザはチャンネル参加時に自動的にオペレータ
	権とプロテクトステータスを与えられます。
CHAN_HELP_SOP_MID2
	
	SOPユーザはAOP、VOP、NOPの各リストを編集できます。SOPリスト
	を編集できるのはチャンネル創設者のみです。
CHAN_HELP_SOP_MID2_HALFOP
	
	SOPユーザはAOP、HOP、VOP、NOPの各リストを編集できます。SOPリ
	ストを編集できるのはチャンネル創設者のみです。
CHAN_HELP_SOP_END
	
	ADD、DEL、LIST、COUNTの各サブコマンドはAOPコマンドと同様に動
	作します。

CHAN_HELP_AOP
	用法：AOP チャンネル ADD ニック
	　　　AOP チャンネル DEL ニック
	　　　AOP チャンネル LIST [+開始位置] [パターン]
	　　　AOP チャンネル COUNT
	
	AOP（オートオペレータ）リストを編集または表示します。AOPリス
	トに登録されたユーザはチャンネル参加時に自動的にオペレータ権
	を与えられます。
CHAN_HELP_AOP_MID
	
	AOPユーザはVOPとNOPリストを編集できます。AOPリストを編集でき
	るのは、SOPユーザとチャンネル創設者です。
CHAN_HELP_AOP_MID_HALFOP
	
	AOPユーザはHOP、VOP、NOPの各リストを編集できます。AOPリスト
	を編集できるのは、SOPユーザとチャンネル創設者です。
CHAN_HELP_AOP_END
	
	ADDとDELコマンドはそれぞれユーザを登録・消去します。ただし、
	特定のユーザを複数のリストに登録することはできません。ADDコ
	マンドで指定されたユーザがすでに他のリスト（VOPリストなど）
	に登録されている場合、そのリストから消去した上で、AOPリスト
	に登録します。なお、当該リストの編集権を持っていない場合は、
	AOP ADDコマンドもそのユーザに対して使えません。
	
	LISTコマンドはAOPリストを表示します。パターンを指定すると、
	そのパターンに一致するエントリのみが表示されます。また、開始
	位置で最初に表示するレコードを指定できる（例：「+50」で51個
	目の該当レコードから表示）
	
	COUNTコマンドはAOPリストのエントリ数を表示します。

CHAN_HELP_HOP
	用法：HOP チャンネル ADD ニック
	　　　HOP チャンネル DEL ニック
	　　　HOP チャンネル LIST [+開始位置] [パターン]
	　　　HOP チャンネル COUNT
	
	HOP（ハーフオペレータ）リストを編集または表示します。HOPリス
	トに登録されたユーザはチャンネル参加時に自動的に半オペレータ
	権を与えられます。
	
	HOPユーザはVOPとNOPリストを編集できます。HOPリストを編集でき
	るのは、AOP・SOPユーザとチャンネル創設者です。
	
	ADD、DEL、LIST、COUNTの各サブコマンドはAOPコマンドと同様に動
	作します。

CHAN_HELP_VOP
	用法：VOP チャンネル ADD ニック
	　　　VOP チャンネル DEL ニック
	　　　VOP チャンネル LIST [+開始位置] [パターン]
	　　　VOP チャンネル COUNT
	
	VOP（ボイスオペレータ）リストを編集または表示します。VOPリス
	トに登録されたユーザはチャンネル参加時に自動的に発言権を与え
	られます。
	
	ADD、DEL、LIST、COUNTの各サブコマンドはAOPコマンドと同様に動
	作します。

CHAN_HELP_NOP
	用法：NOP チャンネル ADD ニック
	　　　NOP チャンネル DEL ニック
	　　　NOP チャンネル LIST [+開始位置] [パターン]
	　　　NOP チャンネル COUNT
	
	NOP（NeverOp：オペレータ不許可）リストを編集または表示します。
	NOPリストに登録されたユーザはオペレータ権を取得できず、他の
	ユーザから与えられた場合も%Sにより即時剥奪されます。
	
	ADD、DEL、LIST、COUNTの各サブコマンドはAOPコマンドと同様に動
	作します。

CHAN_HELP_ACCESS
	用法：ACCESS チャンネル ADD ニック レベル
	　　　ACCESS チャンネル DEL ニック
	　　　ACCESS チャンネル LIST [+開始位置] [パターン]
	　　　ACCESS チャンネル LISTLEVEL [+開始位置] レベル
	      ACCESS チャンネル COUNT
	
	チャンネルのアクセスリストを編集または表示します。アクセスリ
	ストは、どのユーザがどのチャンネル権限を持っているかを管理す
	るリストで、登録したアクセスレベルによってユーザの権限が決ま
	ります（登録されていないユーザはアクセスレベルが「０」となり
	ます）。アクセスレベルと与えられる権限との関係については、
	「/msg %S HELP ACCESS LEVELS」を入力してください。
	
	ADDコマンドでユーザをアクセスリストに登録できます。ユーザの
	ニックと、ユーザに与えたいアクセスレベルを指定します。自分の
	アクセスレベル以上のレベルを指定することは出来ません（例えば
	レベル50のユーザはレベル49以下しか指定できません）。また、指
	定したニックがすでにアクセスリストに登録されている場合はアク
	セスレベルを変更しますが、もとのレベルが自分のレベル以上のユ
	ーザのアクセスレベルを変更できません。
	
	DELコマンドでユーザをアクセスリストから消去できます。消去で
	きるのは、自分のアクセスレベルより下のユーザのみです。
	
	LISTコマンドはアクセスリストを表示します。パターンを指定する
	と、そのパターンに一致するニックのみが表示されます。また、開
	始位置で最初に表示するレコードを指定できる（例：「+50」で51
	個目の該当レコードから表示）
	
	LISTLEVELコマンドもアクセスリストを表示しますが、ニックでは
	なく、アクセスレベルで検索します。例えば、レベルを「50」と指
	定したときはアクセスレベル50のユーザを、「50-100」の場合は、
	レベル50以上、100以下のユーザを表示します。
	
	COUNTコマンドはアクセスリストのエントリ数を表示します。
CHAN_HELP_ACCESS_XOP
	
	なお、%SのSOP/AOP/VOP/NOPコマンドでもアクセスリストを
	編集できます。この場合、SOPはレベル%d、AOPはレベル%d、VOPは
	レベル%d、NOPはレベル%dに相当します。
CHAN_HELP_ACCESS_XOP_HALFOP
	
	なお、%SのSOP/AOP/VOP/HOP/NOPコマンドでもアクセスリス
	トを編集できます。この場合、SOPはレベル%d、AOPはレベル%d、
	HOPはレベル%d、VOPはレベル%d、NOPはレベル%dに相当します。

CHAN_HELP_ACCESS_LEVELS
	アクセスレベルとチャンネル権限
	
	デフォルトでは、アクセスレベルとチャンネル権限の関係は以下の
	通りです。
	
	創設者   全ての%Sコマンドを使える。自動オペレータ権
	  %4d   AKICKコマンドが使える。自動オペレータ権
	  %4d   自動オペレータ権
CHAN_HELP_ACCESS_LEVELS_HALFOP
	  %4d   自動半オペレータ権
CHAN_HELP_ACCESS_LEVELS_END
	  %4d   自動発言権
	     0   権限なし。オペレータ権は許可される（SECUREOPS無効時）
	    -1   オペレータ権は許可されない
	  -100   チャンネル参加禁止
	
	LEVELSコマンドを使えば、上記のレベル（最後の２つを除く）を変
	更したり、新しい権限レベルを設定したりできます。詳しくは
	「/msg %S HELP LEVELS」を入力してください。

CHAN_HELP_LEVELS
	用法：LEVELS チャンネル SET 権限 レベル
	　　　LEVELS チャンネル {DIS | DISABLE} 権限
	　　　LEVELS チャンネル LIST
	　　　LEVELS チャンネル RESET
	
	チャンネルのアクセスレベルと権限の関係を変更します。このコマ
	ンドで、%Sの殆どの機能に必要なレベルを設定することができます。
	（但し、SET FOUNDER・SET PASSWORD・LEVELSの各コマンドは創設
	者しか使えません。）「権限」に使える権限名はHELP LEVELS DESC
	でご覧になれます。
CHAN_HELP_LEVELS_XOP
	
	注意：権限のアクセスレベルを変更すると、SOP/AOP/VOP/NOPコマ
	　　　ンドが正しく動作しなくなる場合があります。
CHAN_HELP_LEVELS_XOP_HOP
	
	注意：権限のアクセスレベルを変更すると、SOP/AOP/HOP/VOP/NOP
	　　　コマンドが正しく動作しなくなる場合があります。
CHAN_HELP_LEVELS_END
	
	SETコマンドで、特定のチャンネル権限を得たり、特定のコマンド
	を使用するのに必要なアクセスレベルを設定できます。
	
	DISABLEコマンド（省略のDISも可）で権限を無効にすることができ
	ます。自動オペレータ権などの権限を無効にすると、チャンネル創
	設者を含めどのユーザにも適用されませんが、コマンド権限を無効
	にした場合は、創設者に限って引き続きコマンドを使うことができ
	ます。
	
	LISTコマンドで、現在の権限レベルを確認できます。
	
	RESETコマンドで、全ての権限レベルを初期値（デフォルト）に戻
	すことができます。

CHAN_HELP_LEVELS_DESC
	LEVELSコマンドで使う権限名は以下の通りです。

CHAN_HELP_AKICK
	用法：AKICK チャンネル ADD マスク [理由]
	　　　AKICK チャンネル DEL マスク
	　　　AKICK チャンネル LIST [開始位置] [パターン]
	　　　AKICK チャンネル VIEW [開始位置] [パターン]
	　　　AKICK チャンネル COUNT
	　　　AKICK チャンネル ENFORCE
	
	チャンネルの自動キックリストを編集または表示します。このリス
	トに登録されたユーザがチャンネルに入ると、%Sはそのユー
	ザに対して追放（+b）マスクをチャンネルに追加し、ユーザを強制
	的にチャンネルから排除（キック）します。
	
	ADDコマンドは指定されたマスク（「ユーザ名@ホスト名」または
	「ニック!ユーザ名@ホスト名」形式）を自動キックリストに追加しま
	す。理由を指定した場合は、ユーザがキックされる時にキックメッ
	セージの中で表示されます。
	
	DELコマンドは指定されたマスクを自動キックリストから消去しま
	す。ただ、自動キックにより設定されたチャンネル追放マスクは解
	除されませんので手動で解除してください。
	
	LISTとVIEWコマンドは自動キックリストを表示します。ACCESSコマ
	ンド同様、検索パターンや開始位置を指定して結果を絞り込むこと
	もできます。LISTとVIEWの違いは、VIEWの方がより詳細な情報を表
	示することです。
	
	COUNTコマンドは、自動キックリストのエントリ数を表示します。
	
	ENFORCEコマンドは、現在チャンネルに参加しているユーザを自動
	キックリストに照らし合わせ、キックすべきユーザをキックします。
	すでに参加しているユーザを自動キックリストに追加した時に便利
	です。
	
	このコマンドの使用は、デフォルトでは%s以上のアク
	セスを持つユーザに制限されています。

CHAN_HELP_INFO
	用法：INFO チャンネル [ALL]
	
	チャンネルの情報（創設者、説明、登録された日時、設定など）を
	表示します。創設者であり、またはパスワード認証を行ったチャン
	ネルについては、「ALL」を追加すれば参加時メッセージと後継者
	も表示されます。

CHAN_HELP_LIST
	用法：LIST [開始位置] パターン
	
	パターンに一致するチャンネルの一覧を表示します。但し、プライ
	ベート設定が有効になっているチャンネルは表示されません。開始
	位置で、最初に表示するレコードを指定できます（例えば、「+50」
	を指定すると最初の50個の該当レコードを飛ばし、51個目から表示
	します）。
CHAN_HELP_LIST_OPERSONLY
	
	このコマンドの使用はIRCオペレータに制限されています。

CHAN_HELP_OP
	用法：OP チャンネル [ニック]
	
	チャンネルのユーザにオペレータ権を与えます。ニックを指定しな
	い場合は、コマンド使用者が対象となります。使用はデフォルトで
	%s以上の権限を持つユーザに制限されています。

CHAN_HELP_DEOP
	用法：DEOP チャンネル [ニック]
	
	チャンネルのユーザのオペレータ権を解除します。ニックを指定し
	ない場合は、コマンド使用者が対象となります。使用はデフォルト
	で%s以上の権限を持つユーザに制限されています。

CHAN_HELP_VOICE
	用法：VOICE チャンネル [ニック]
	
	チャンネルのユーザに発言権を与えます。ニックを指定しない場合
	は、コマンド使用者が対象となります。使用はデフォルトで
	%s以上の権限を持つユーザに制限されています。

CHAN_HELP_DEVOICE
	用法：DEVOICE チャンネル [ニック]
	
	チャンネルのユーザの発言権を解除します。ニックを指定しない場
	合は、コマンド使用者が対象となります。使用はデフォルトで
	%s以上の権限を持つユーザに制限されています。

CHAN_HELP_HALFOP
	用法：HALFOP チャンネル [ニック]
	
	チャンネルのユーザに半オペレータ権を与えます。ニックを指定し
	ない場合は、コマンド使用者が対象となります。使用はデフォルト
	で%s以上の権限を持つユーザに制限されています。

CHAN_HELP_DEHALFOP
	用法：DEHALFOP チャンネル [ニック]
	
	チャンネルのユーザの半オペレータ権を解除します。ニックを指定
	しない場合は、コマンド使用者が対象となります。使用はデフォル
	トで%s以上の権限を持つユーザに制限されています。

CHAN_HELP_PROTECT
	用法：PROTECT チャンネル [ニック]
	
	チャンネルのユーザをプロテクト状態にします。ニックを指定しな
	い場合は、コマンド使用者が対象となります。使用はデフォルトで
	%s以上の権限を持つユーザに制限されています。

CHAN_HELP_DEPROTECT
	用法：DEPROTECT チャンネル [ニック]
	
	チャンネルのユーザを非プロテクト状態にします。ニックを指定し
	ない場合は、コマンド使用者が対象となります。使用はデフォルト
	で%s以上の権限を持つユーザに制限されています。

CHAN_HELP_INVITE
	用法：INVITE チャンネル
	
	指定されたチャンネルに、%Sに自分を誘わせます。使用はデフォル
	トで%s以上の権限を持つユーザに制限されています。
	
	注意：「チャンネルが使用されていない」との応答が返ってくる場
	　　　合は、このコマンドを使わず、そのままチャンネルに参加で
	　　　きます。

CHAN_HELP_UNBAN
	用法：UNBAN チャンネル
	
	あなたの現在のユーザアドレスが一致するチャンネルの追放マスク
	を解除し、チャンネルに参加できるようにします。使用はデフォル
	トで%s以上の権限を持つユーザに制限されています。

CHAN_HELP_KICK
	用法：KICK チャンネル ニック [理由]
	
	%Sに、指定されたニックを指定されたチャンネルからキック
	させます。キックメッセージには、KICKコマンドを利用した人と、
	指定された場合は理由も表示されます。使用はデフォルトで
	%s以上の権限を持つユーザに制限されています。
CHAN_HELP_KICK_PROTECTED
	
	プロテクト状態のユーザは、仮に使用者がチャンネル創立者でも、
	このコマンドではキックできません。

CHAN_HELP_TOPIC
	用法：TOPIC チャンネル topic
	
	%Sにチャンネルのトピックを変更させます。TOPICLOCK（トピック
	変更禁止）設定と一緒に使うと特に便利です。使用はデフォルトで
	%s以上の権限を持つユーザに制限されています。

CHAN_HELP_CLEAR
	用法：CLEAR チャンネル 項目
	
	チャンネルのモードをクリアしたり、ユーザを全てキックしたりし
	ます。「項目」には以下のものが使えます。
	
	    MODES       チャンネルのバイナリモード（m,n,p,s,t等）を
	            　　    モードロック状態に戻す
	    BANS        追放マスクを全て解除する
CHAN_HELP_CLEAR_EXCEPTIONS
	    EXCEPTIONS  追放例外マスクを全て解除する
CHAN_HELP_CLEAR_INVITES
	    INVITES     誘い（invite）マスクを全て解除する
CHAN_HELP_CLEAR_MID
	    OPS         全てのユーザのオペレータ権を解除する
CHAN_HELP_CLEAR_HALFOPS
	    HALFOPS     全てのユーザの半オペレータ権を解除する
CHAN_HELP_CLEAR_END
	    VOICES      全てのユーザの発言権を解除する
	    USERS       全てのユーザをチャンネルからキックする
	
	使用はデフォルトで%s以上の権限を持つユーザに制限されています。

CHAN_HELP_STATUS
	用法：STATUS チャンネル ニック
	
	指定されたニックの、指定されたチャンネルでの現在のアクセスレ
	ベルを表示します。結果の形式は下記の通りです。
	
	    STATUS チャンネル ニック レベル
	
	エラーが生じた場合は、「レベル」の変わりに「ERROR メッセージ」
	が表示されます。
	
	使用はデフォルトで%s以上の権限を持つユーザに制限されています。

CHAN_OPER_HELP_COMMANDS
	
	Servicesアドミンは次のコマンドも使えます。
	
	    DROPCHAN   任意のチャンネルの登録を解除する
CHAN_OPER_HELP_COMMANDS_GETPASS
	    GETPASS    チャンネルのパスワードを表示する
	               　　（暗号化が使われていない場合のみ）
CHAN_OPER_HELP_COMMANDS_FORBID
	    FORBID     チャンネルを使用禁止にする
	    SUSPEND    チャンネルを使用停止にする
	    UNSUSPEND  チャンネルの使用停止を解除する
CHAN_OPER_HELP_COMMANDS_END
	
	なお、Servicesアドミンは任意のチャンネルに対して、パスワード
	認証を行わずに、アクセスリストや自動キックリストの表示と
	STATUSコマンドの使用ができます。

CHAN_OPER_HELP_DROPCHAN
	用法：DROPCHAN チャンネル
	
	指定されたチャンネルの登録を解除します。
	
	このコマンドの使用はServicesアドミンに制限されています。

CHAN_OPER_HELP_SET
	
	Servicesアドミンは、NOEXPIRE（自動登録解除の有無）設定を変更
	したり、そのたの設定をパスワード認証を行わずに変更できます。

CHAN_OPER_HELP_SET_NOEXPIRE
	用法：SET チャンネル NOEXPIRE {ON | OFF}
	
	指定されたチャンネルの登録が自動的に解除されるかどうかを設定
	します。
	
	このコマンドの使用はServicesアドミンに制限されています。

CHAN_OPER_HELP_UNSET
	
	SETコマンド同様、Servicesアドミンはパスワード認証を行わずに
	チャンネルの設定変更ができます。

CHAN_OPER_HELP_INFO
	
	Servicesアドミンは、どのチャンネルに対してもALLを指定できま
	す。

CHAN_OPER_HELP_LIST
	用法：LIST パターン [FORBIDDEN] [NOEXPIRE] [SUSPENDED]
	
	パターンに一致するチャンネルを表示します。但し、プライベート
	設定が有効になっているチャンネルは、Servicesアドミンにしか表
	示されません。また、Servicesアドミンが使った場合は、NOEXPIRE
	設定が有効なちゃんねるの先頭に「!」印が付加されます。
	
	開始位置で、最初に表示するレコードを指定できます（例えば、
	「+50」を指定すると最初の50個の該当レコードを飛ばし、51個目
	から表示します）。
	
	「FORBIDDEN」「SUSPENDED」「NOEXPIRE」の各オプションを指定す
	ると、結果をそれぞれ使用禁止チャンネル、使用停止チャンネル、
	NOEXPIRE設定有効なチャンネルに限定します。但し、これらのオプ
	ションの使用はServicesアドミンに限られています。

CHAN_OPER_HELP_GETPASS
	用法：GETPASS チャンネル
	
	指定されたチャンネルのパスワードを表示します。ただし、暗号化
	が利用されている場合は、このコマンドを使えません。
	
	このコマンドが使われた時、使用したユーザのニックとパスワード
	を表示されたチャンネルがWALLOPS/GLOBOPSで全オペレータに周知
	され、ログされます。
	
	このコマンドの使用はServicesアドミンに制限されています。

CHAN_OPER_HELP_FORBID
	用法：FORBID チャンネル
	
	チャンネルを使用禁止にします。解除にはDROPコマンドを使います。
	
	このコマンドの使用はServicesアドミンに制限されています。

CHAN_OPER_HELP_SUSPEND
	用法：SUSPEND [+期限] チャンネル 理由
	
	チャンネルを使用停止にします。ユーザが利用できなくなるほか、
	パスワード認証も停止されます。解除にはUNSUSPENDを使います。
	期限は%sのAKILLコマンドと同様に解釈されます。
	
	このコマンドの使用はServicesアドミンに制限されています。

CHAN_OPER_HELP_UNSUSPEND
	用法：UNSUSPEND チャンネル
	
	チャンネルの使用停止を解除します、ユーザが普段通りチャンネル
	を使用できるようになります。
	
	このコマンドの使用はServicesアドミンに制限されています。

###########################################################################
#
# MemoServのヘルプメッセージ
#
###########################################################################

MEMO_HELP
	%Sは、メッセージ送受信サービスです。現在IRCに接続して
	いないユーザでも、そのユーザへのメッセージをこのサービスに預
	けると、そのユーザが次回接続した時にメッセージを受け取ること
	になります。このメッセージのことを「メモ」と呼びます。チャン
	ネルに対してもメモを送信できます(※)。
	
	メモを利用するには、宛先ニックやチャンネルだけでなく、送信す
	るユーザのニックも登録されていなければなりません。
	
	%Sで使えるコマンドは「/msg %S HELP COMMANDS」で
	表示できます。また、特定のコマンドの情報を見る場合は
	「/msg %S HELP コマンド」で表示できます。
MEMO_HELP_EXPIRES
	
	注意：メモは、SAVEコマンドで保存したものを除き、送信後%sが
	　　　経つと自動的に削除されます。
MEMO_HELP_END_LEVELS
	
	※チャンネルへのメモは、チャンネル創設者及び%s以上
	　のアクセスを持つユーザに送られます。メモを受信するアクセス
	　レベルは%sのLEVELSコマンドで変更できます。
MEMO_HELP_END_XOP
	
	※チャンネルへのメモは、チャンネル創設者及びSOPリストに登録
	　されているユーザに送られます。

MEMO_HELP_COMMANDS
	%Sでは、下記コマンドが使えます。
	
	    SEND      ニックまたはチャンネルへメモを送信する
	    LIST      受信メモ一覧を表示する
	    READ      受信メモの内容を表示する
MEMO_HELP_COMMANDS_FORWARD
	    FORWARD   受信メモをニックのメールアドレスに転送する
MEMO_HELP_COMMANDS_SAVE
	    SAVE      受信メモを保存する
MEMO_HELP_COMMANDS_DEL
	    DEL       受信メモを削除する
	    RENUMBER  メモ番号を振り直す
	    SET       メモ関連の設定を変更する
	    INFO      メモ関連情報を表示する
MEMO_HELP_COMMANDS_IGNORE
	    IGNORE    メモ受信拒否リストを編集する

MEMO_HELP_SEND
	用法：SEND {ニック | チャンネル} テキスト
	
	指定されたニックまたはチャンネルへメモを送信します。宛先（チ
	ャンネルの場合はチャンネルの管理者）がIRCに接続している場合
	は、ユーザに新着通知を送ります。

MEMO_HELP_LIST
	用法：LIST [番号 | リスト | NEW]
	
	受信メモ一覧を表示します。番号のリストを指定すると該当するメ
	モだけが、NEWを指定すると未読メモが表示されます。どの形式で
	も、未読メモは番号の左側に「*」印が、チャンネル宛てのメモに
	「#」印が付きます。
	
	番号リスト使用例：
	    LIST 2-5,7-9
	       ２〜５、７〜９番目のメモをリストします。

MEMO_HELP_LIST_EXPIRE
	用法：LIST [番号 | リスト | NEW]
	
	受信メモ一覧を表示します。番号のリストを指定すると該当するメ
	モだけが、NEWを指定すると未読メモが表示されます。どの形式で
	も、未読メモは番号の左側に「*」印が、チャンネル宛てのメモに
	「#」印が、SAVEコマンドで保存されたメモに「+」印が付きます。
	
	番号リスト使用例：
	    LIST 2-5,7-9
	       ２〜５、７〜９番目のメモをリストします。

MEMO_HELP_READ
	用法：READ {番号 | リスト | LAST | NEW}
	
	メモを読みます。番号または番号のリストを指定すると該当するメ
	モが、LASTを指定すると最も最近届いたメモが、NEWを指定すると
	未読メモ全てが表示されます。

MEMO_HELP_SAVE
	用法：SAVE {番号 | リスト}
	
	メモを保存します。指定された番号または番号リストに該当するメ
	モは自動削除されなくなります。

MEMO_HELP_DEL
	用法：DEL {番号 | リスト | ALL}
	
	メモを削除します。番号または番号リストを指定すると該当するメ
	モが、ALLを指定すると全てのメモが削除されます。メモを削除す
	ると、元に戻すことはできませんのでご注意ください。

MEMO_HELP_RENUMBER
	用法：RENUMBER
	
	メモ番号を振り直し、メモの削除による欠番を解消します。

MEMO_HELP_SET
	用法：SET 設定名 値
	
	メモ関連設定を変更します。「設定名」には以下のものが使えます。
	
	    NOTIFY      新着通知設定を変更する
	    LIMIT       受信メモ件数上限を変更する
MEMO_HELP_SET_OPTION_FORWARD
	    FORWARD     メモの自動転送設定を変更する
MEMO_HELP_SET_END
	
	それぞれの設定の詳しい情報を見る場合は
	「/msg %S HELP SET 設定名」を入力してください。

MEMO_HELP_SET_NOTIFY
	用法：SET NOTIFY {ON | LOGON | NEW | OFF}
	
	新着メモの通知設定を変更します。各値の意味は次の通りです。
	
	    ON      接続時、/AWAY解除時、受信時
	    LOGON   接続時、/AWAY解除時のみ
	    NEW     受信時のみ
	    OFF     通知なし
	
	ニックのセキュア設定（%sのSET SECUREコマンド）が有効に
	なっている場合は、新着通知設定をONまたはLOGONにしても、ニッ
	クのパスワード認証を行うまでは新着メモの通知はされません。

MEMO_HELP_SET_LIMIT
	用法：SET LIMIT 上限
	
	受信できるメモの最大件数を設定します。０に設定すると、メモを
	受信できなくなります。但し、%dより大きい数値は指定できません。

MEMO_HELP_INFO
	用法：INFO
	
	受信メモ件数とその上限、未読件数、そして新着通知設定を表示し
	ます。

MEMO_OPER_HELP_COMMANDS
	
	Servicesアドミンは自分以外のニックに対してもSET LIMIT及び
	INFOコマンドを使えます。

MEMO_OPER_HELP_SET_LIMIT
	用法：SET LIMIT [ニック] {上限 | NONE | DEFAULT} [HARD]
	
	受信できるメモの最大件数を設定します。０に設定すると、メモを
	受信できなくなります。但し、Servicesアドミン以外のユーザは%d
	より大きい数値は指定できません。
	
	Servicesアドミンに限って、他のニックのメモ件数上限を変更でき
	ます。数値のほか、DEFAULT（デフォルト値）とNONE（無制限）も
	指定できます。なお、上限の後にHARDを追加すると、そのユーザは
	件数上限の変更ができなくなります。HARDを指定しない場合は逆に
	変更可能になります。

MEMO_OPER_HELP_INFO
	用法：INFO [ニック]
	
	受信メモ件数とその上限、未読件数、そして新着通知設定を表示し
	ます。ニックを指定した場合はそのニックのメモ情報を表示します。
	（ニック指定はServicesアドミンに制限されています。）

MEMO_HELP_FORWARD
	用法：FORWARD {番号 | リスト | ALL}
	
	受信メモを、ニックに登録されているメールアドレスに転送します。
	番号や番号リストを指定した時は該当するメモが、ALLを指定した
	時は全てのメモが転送されます。チャンネルのメモを転送すること
	はできません。

MEMO_HELP_SET_FORWARD
	用法：SET FORWARD {ON | COPY | OFF}
	
	受信メモの自動転送設定を変更します。ONにすると、メモは保存さ
	れず、ニックに登録されたメールアドレスに自動的に転送されます。
	COPYにすると、同様に転送すると同時に%Sのデータベースにもメモ
	を保存します。OFFにすると、通常通りデータベースにメモを保存
	します。但し、ONに設定した場合でも、何らかの理由でメール送信
	が出来ない場合はOFF同様にメモを保存します。
	
	COPYに設定した場合で、受信メモ件数が上限に達した場合は、メー
	ルでもメモを受信できなくなりますのでご注意ください。

MEMO_HELP_IGNORE
	Syntax: IGNORE {ADD | DEL | LIST} [ニック | マスク]
	
	メモ受信拒否リストを編集または表示します。このリストに登録し
	たニックまたは「ユーザ@ホスト」形式のマスクを持つユ
	ーザからメモを受け取らなくなります。ADDコマンドでニックまた
	はマスクを追加、DELコマンドで消去します。LISTコマンドで受信
	拒否リストを表示します。
	
	受信拒否リストに追加するニックやマスクにワイルドカード文字
	「*」と「?」を使用できます。また、大文字と小文字の区別はされ
	ませんので、「*Waru*」のようなマスクには「warumono」も
	「IJIWARU」も当てはまります。


###########################################################################
#
# OperServのヘルプメッセージ
#
###########################################################################

OPER_HELP
	%Sは、IRCオペレータ専用のネットワーク管理・制御サービ
	スです。「/msg %S HELP COMMANDS」でコマンド一覧を表示できま
	す。
	
	注意：%Sに送信された全てのコマンドはログされます。

OPER_HELP_COMMANDS
	%Sでは、下記コマンドが使えます。
	
	全てのIRCオペレータが使えるコマンド：
	    GLOBAL      全ユーザにメッセージを送信する
	    STATS       Servicesとネットワークの状況等を表示する
	    SERVERMAP   ネットワーク構成図を表示する
	    OPER LIST   Servicesオペレータリストを表示する
	    ADMIN LIST  Servicesアドミンリストを表示する
OPER_HELP_COMMANDS_SERVOPER
	
	Servicesオペレータ専用コマンド：
	    MODE        チャンネルのモードを変更する
	    KICK        チャンネルからユーザをキックする
	    CLEARMODES  チャンネルのモードをクリアする
	    CLEARCHAN   チャンネルの全ユーザをキックする
	    KILLCLONES  同じホストから接続しているユーザを全て切断する
OPER_HELP_COMMANDS_AKILL
	    AKILL       自動切断リストを編集する
	    AKILLCHAN   チャンネルの全ユーザを自動切断リストに追加する
OPER_HELP_COMMANDS_EXCLUDE
	    EXCLUDE     自動切断除外リストを編集する
OPER_HELP_COMMANDS_SLINE
	    SGLINE      SGLINEリストを編集する
	    SQLINE      SQLINEリストを編集する
	    SZLINE      SZLINEリストを編集する
OPER_HELP_COMMANDS_SESSION
	    SESSION     セッションリストを表示する
	    EXCEPTION   セッション接続数制限の例外リストを編集する
OPER_HELP_COMMANDS_NEWS
	    LOGONNEWS   接続時に表示されるニュース項目を登録する
	    OPERNEWS    IRCオペレータ権取得時ニュース項目を登録する
OPER_HELP_COMMANDS_SERVADMIN
	
	Servicesアドミン専用コマンド：
	    OPER        Servicesオペレータリストを編集する
	    JUPE        サーバを「ジュピター」（接続拒否）する
	    SET         Services全体の設定を変更する
	    UPDATE      ディスク上のデータベースをすぐ更新させる
	    SHUTDOWN    データベースを保存し、Servicesを終了させる
	    QUIT        データベースを保存せずにServicesを終了させる
	    RESTART     データベースを保存し、Servicesを再起動する
	    REHASH      環境設定ファイルを再読み込みする
	    SU          Servicesスーパーユーザ権を取得する
OPER_HELP_COMMANDS_SERVROOT
	
	Servicesスーパーユーザ専用コマンド：
	    ADMIN       Servicesアドミンリストを編集する
OPER_HELP_COMMANDS_RAW
	    RAW         IRCネットワークに文字列を直接送信する

OPER_HELP_GLOBAL
	用法：GLOBAL メッセージ
	
	IRCネットワークの全ユーザにメッセージを送信します。メッセー
	ジはニック「%s」から送信されます。

OPER_HELP_STATS
	用法：STATS [RESET | NETWORK | ALL]
	
	パラメータがない時、ネットワークに接続しているユーザ数とIRC
	オペレータ数（Servicesの疑似ユーザは含まず）、現在までの最大
	ユーザ数、そして起動から経っている時間を表示します。
	
	RESETを指定した時、最大ユーザ数を現在のユーザ数にリセットし
	ます。
	
	NETWORKを指定した時、Servicesがネットワークに送信したデータ
	量とネットワークから受信したデータ量と、送受信バッファの合計
	メモリ使用量を表示します。バッファサイズ制限が設定されている
	場合は、その制限に対する使用率もパーセントで表示されます。
	
	ALLはServicesアドミンに制限されているオプションで、Services
	の各機能のメモリ使用量を表示します。この使用量を計算している
	間は他のコマンドに応答できず、大規模なネットワークでは時間が
	かかることもありますので、使用に注意してください。
	
	「STATS」の代わりに「UPTIME」を使うことも出来ます。

OPER_HELP_SERVERMAP
	用法：SERVERMAP
	
	Servicesの観点からネットワークの構成図を表示します。疑似サー
	バ（Services自体と、JUPEコマンドで接続拒否されたサーバ）は
	「(*)」で示されます。

OPER_HELP_OPER
	用法：OPER ADD ニック
	　　　OPER DEL ニック
	　　　OPER LIST
	
	Servicesオペレータリストを編集または表示します。このリストに
	登録されているユーザは、%sに対してパスワード認証を行えば、
	Servicesオペレータ専用コマンドを使用できるようになります。
	
	LIST（表示）コマンドは、全てのIRCオペレータが使えますが、ADD
	（追加）とDEL（削除）はServicesアドミンに制限されています。

OPER_HELP_ADMIN
	用法：ADMIN ADD ニック
	　　　ADMIN DEL ニック
	　　　ADMIN LIST
	
	Servicesオペレータリストを編集または表示します。このリストに
	登録されているユーザは、%sに対してパスワード認証を行えば、
	Servicesオペレータ・Servicesアドミン専用コマンドを使用できる
	ようになります。
	
	LIST（表示）コマンドは、全てのIRCオペレータが使えますが、ADD
	（追加）とDEL（削除）はServicesスーパーユーザに制限されてい
	ます。

OPER_HELP_GETKEY
	用法：GETKEY チャンネル
	
	チャンネルに設定されたキーワードを表示します。
	
	このコマンドの使用はServicesオペレータに制限されています。

OPER_HELP_MODE
	用法：MODE チャンネル モード
	
	チャンネルのモードを変更します。パラメータはIRCの/MODEコマン
	ドと同様です。
	
	このコマンドの使用はServicesオペレータに制限されています。

OPER_HELP_CLEARMODES
	用法：CLEARMODES チャンネル [ALL]
	
	チャンネルモード（n,s,t,k,l,b等）を全てクリアします。ALLを指
	定すると、オペレータ権（+o）・発言権（+v）等のユーザモードも
	クリアされます。ただし、チャンネルが登録され、モードロックが
	設定されている場合は、ロックされたモードはそのまま残ります。
	
	このコマンドの使用はServicesオペレータに制限されています。

OPER_HELP_CLEARCHAN
	用法：CLEARCHAN チャンネル
	
	チャンネルの全ユーザをそのチャンネルからキックします。

OPER_HELP_KICK
	用法：KICK チャンネル ニック 理由
	
	チャンネルからユーザをキックします。パラメータはIRCの/KICKコ
	マンドと同様ですが、理由の先頭にこのコマンドを使ったユーザの
	ニックが追加されます。
	
	このコマンドの使用はServicesオペレータに制限されています。

OPER_HELP_AKILL
	用法：AKILL ADD [+有効期限] マスク 理由
	　　　AKILL DEL マスク
	　　　AKILL CLEAR ALL
	　　　AKILL LIST [+開始位置] [パターン] [NOEXPIRE]
	　　　AKILL VIEW [+開始位置] [パターン] [NOEXPIRE]
	　　　AKILL CHECK マスク
	      AKILL COUNT
	
	自動切断（オートキル）リストを編集または表示します。このリス
	トに登録された「ユーザ@ホスト」形式のマスクに一致す
	るユーザがIRCネットワークに接続した場合、Servicesはそのユー
	ザを切断（/KILL）し、ネットワークの各サーバに切断マスク
	（Kline）を追加します。
	
	サブコマンドは以下の通りです。
	　・ADD：マスクを登録する（理由も必須）
	　・DEL：マスクを消去する
	　・CLEAR：全マスクを消去する（「ALL」は、誤用防止のため必須）
	　・LIST：登録されたマスクを表示する（パターン指定で絞り込み
	　　可能）。NOEXPIREを指定すると、表示対象を有効期限のないも
	　　のに限定する。また、開始位置で最初に表示するレコードを指
	　　定できる（例：「+50」で51個目の該当レコードから表示）
	　・VIEW：LIST同様の検索をし、各エントリの詳細情報を表示する
	　・CHECK：指定されたマスクが当てはまるエントリを表示する
	　・COUNT：登録エントリ数を表示する
	
	ADDコマンドでは、エントリの有効期限を指定することもできます。
	マスクの前に、数値と単位の組合わせで現在からの有効期間を指定
	します。単位は「d」(日)、「h」(時間)、「m」(分) のいずれかで、
	例えば「+90m」または「+1h30m」を指定すると、エントリが１時間
	30分後に自動的に消去されます。単位を指定しない場合は日数と解
	釈されます。なお、「+」で始まるマスクを追加する時は、有効期
	限と解釈されてしまうため、必ず有効期限を指定しなければなりま
	せん。
	
	ADDコマンドで使う「理由」は、Servicesの設定によっては、/KILL
	メッセージで表示される場合があります。
OPER_HELP_AKILL_OPERMAXEXPIRY
	
	Servicesオペレータが有効期限として指定できる期間は%s以下
	です。Servicesアドミンが指定できる期間に制限はありません。
OPER_HELP_AKILL_END
	
	このコマンドの使用はServicesオペレータに制限されています。

OPER_HELP_AKILLCHAN
	用法：AKILLCHAN [KILL] [+有効期限] チャンネル 理由
	
	指定されたチャンネルに参加している、IRCオペレータを除く全ユ
	ーザを、自動切断リストに追加し、KILLを指定した場合は強制切断
	します。理由は各自動切断マスクと一緒に保存され、Servicesの設
	定によっては切断メッセージに表示されます。有効期限はAKILL
	ADDコマンドと同様に指定することができ、指定しない場合は%s
	となります。
	
	このコマンドで追加される自動切断マスクは「*@ホスト名」形式で
	生成されます。すでに同一のマスクが存在する場合は、その登録内
	容を変更しませんので、実際に追加されるマスクの数は、チャンネ
	ルのユーザ数より少ないこともあります。
	
	このコマンドの使用はServicesオペレータに制限されています。

OPER_HELP_EXCLUDE
	用法：EXCLUDE ADD [+有効期限] マスク 理由
	　　　EXCLUDE DEL マスク
	　　　EXCLUDE CLEAR ALL
	　　　EXCLUDE LIST [+開始位置] [パターン] [NOEXPIRE]
	　　　EXCLUDE VIEW [+開始位置] [パターン] [NOEXPIRE]
	　　　EXCLUDE CHECK マスク
	      EXCLUDE COUNT
	
	自動切断除外リストを編集または表示します。このリストに登録さ
	れたマスクに一致するユーザがIRCに接続した場合、自動切断マス
	クに一致している場合でも切断されません。プロバイダ全体を遮断
	した自動切断マスクがあって、そのプロバイダから特定のユーザに
	接続を許可したい場合などに便利です。
	
	各サブコマンドはAKILLコマンドと同様に動作します。詳しくは
	AKILLコマンドのヘルプをご覧ください。
	
	このコマンドの使用はServicesオペレータに制限されています。

OPER_HELP_SGLINE
	用法：SGLINE ADD [+有効期限] マスク 理由
	　　　SGLINE DEL マスク
	　　　SGLINE CLEAR ALL
	　　　SGLINE LIST [+開始位置] [パターン] [NOEXPIRE]
	　　　SGLINE VIEW [+開始位置] [パターン] [NOEXPIRE]
	　　　SGLINE CHECK マスク
	      SGLINE COUNT
	
	SGLINEリストを編集または表示します。このリストに登録されたマ
	スクに「リアルネーム」（いわゆるgecos）が一致するユーザがIRC
	に接続した場合は、そのユーザを切断し、IRCサーバの機能によっ
	てはSGLINE追放マスクを各サーバに追加させます。
	
	サブコマンドは以下の通りです。
	　・ADD：マスクを登録する（理由も必須）
	　・DEL：マスクを消去する
	　・CLEAR：全マスクを消去する（「ALL」は、誤用防止のため必須）
	　・LIST：登録されたマスクを表示する（パターン指定で絞り込み
	　　可能）。NOEXPIREを指定すると、表示対象を有効期限のないも
	　　のに限定する。また、開始位置で最初に表示するレコードを指
	　　定できる（例：「+50」で51個目の該当レコードから表示）
	　・VIEW：LIST同様の検索をし、各エントリの詳細情報を表示する
	　・CHECK：指定されたマスクが当てはまるエントリを表示する
	　・COUNT：登録エントリ数を表示する
	
	マスクにスペース（空白文字）が含まれている場合、クォート文字
	「"」で囲んでください。例：
	    /msg %S SGLINE ADD +0 "bad name" いけない名前です。
	
	ADDコマンドでは、エントリの有効期限を指定することもできます。
	マスクの前に、数値と単位の組合わせで現在からの有効期間を指定
	します。単位は「d」(日)、「h」(時間)、「m」(分) のいずれかで、
	例えば「+90m」または「+1h30m」を指定すると、エントリが１時間
	30分後に自動的に消去されます。単位を指定しない場合は日数と解
	釈されます。なお、「+」で始まるマスクを追加する時は、有効期
	限と解釈されてしまうため、必ず有効期限を指定しなければなりま
	せん。
	
	ADDコマンドで使う「理由」は、Servicesの設定によっては、/KILL
	メッセージで表示される場合があります。
	
	このコマンドの使用はServicesオペレータに制限されています。

OPER_HELP_SQLINE
	用法：SQLINE ADD [+有効期限] マスク 理由
	　　　SQLINE DEL マスク
	　　　SQLINE CLEAR ALL
	　　　SQLINE LIST [+開始位置] [パターン] [NOEXPIRE]
	　　　SQLINE VIEW [+開始位置] [パターン] [NOEXPIRE]
	　　　SQLINE CHECK マスク
	      SQLINE COUNT
	
OPER_HELP_SQLINE_KILL
	SQLINEリストを編集または表示します。このリストに登録されたマ
	スクにニックが一致するユーザがIRCに接続した場合、または接続
	しているユーザがそのようなニックに変更しようとした場合は、そ
	のユーザを切断し、IRCサーバの機能によってはSQLINE追放マスク
	を各サーバに追加させます。
OPER_HELP_SQLINE_NOKILL
	SQLINEリストを編集または表示します。このリストに登録されたマ
	スクにニックが一致するユーザがIRCに接続した場合、または接続
	しているユーザがそのようなニックに変更しようとした場合は、そ
	のユーザのニックを自動生成された「一時ニック」に変更し、ニッ
	クの変更を促すメッセージをユーザに送ります。また、IRCサーバ
	の機能によってはSQLINE追放マスクを各サーバに追加させます。
OPER_HELP_SQLINE_IGNOREOPERS
	
	IRCオペレータは、SQLINEの制限を受けません。
OPER_HELP_SQLINE_END
	
	各サブコマンドはSGLINEコマンドと同様に動作します。詳しくは
	SGLINEコマンドのヘルプをご覧ください。
	
	このコマンドの使用はServicesオペレータに制限されています。

OPER_HELP_SZLINE
	用法：SZLINE ADD [+有効期限] マスク 理由
	　　　SZLINE DEL マスク
	　　　SZLINE CLEAR ALL
	　　　SZLINE LIST [+開始位置] [パターン] [NOEXPIRE]
	　　　SZLINE VIEW [+開始位置] [パターン] [NOEXPIRE]
	　　　SZLINE CHECK マスク
	      SZLINE COUNT
	
	SZLINEリストを編集または表示します。このリストに登録されたマ
	スクにIPアドレスが一致するユーザがIRCに接続した場合は、その
	ユーザを切断し、IRCサーバの機能によってはSZLINE追放マスクを
	各サーバに追加させます。
	
	各サブコマンドはSGLINEコマンドと同様に動作します。詳しくは
	SGLINEコマンドのヘルプをご覧ください。
	
	このコマンドの使用はServicesオペレータに制限されています。

OPER_HELP_EXCEPTION
	用法：EXCEPTION ADD [+有効期限] マスク 上限 理由
	　　　EXCEPTION DEL {マスク | リスト}
	　　　EXCEPTION CLEAR ALL
	　　　EXCEPTION MOVE 番号 新番号
	　　　EXCEPTION LIST [[+開始位置] パターン | リスト]
	　　　EXCEPTION VIEW [[+開始位置] パターン | リスト]
	　　　EXCEPTION CHECK ホスト名
	　　　EXCEPTION COUNT
	
	セッション制限例外リストを編集または表示します。「セッション」
	とは、一つのクライアント（ユーザ）のIRCへの接続のことで、
	Servicesは「セッション制限機能」で一つのホストからのセッショ
	ン（接続）数を制限することができます。同じホストから、予め設
	定されたセッション数上限を超えた接続があった場合は、Services
	がそのクライアントをすぐに切断します。また、設定によっては切
	断の前に理由を通知するメッセージを送ることもできます。そして
	「セッション制限例外リスト」は、予め設定されたのと違うセッシ
	ョン数上限を特定のホストに適用させるためのものです。
	
	ADDコマンドで、指定されたホスト名またはホストマスク（ユーザ
	名、ニックは使用不可）をセッション制限例外リストに追加できま
	す。マスクに一致する全てのホストに、この制限が適用されます。
	指定された上限がゼロの場合、セッション数が制限されません（そ
	のホストからいくつのクライアントでも接続できるようになります）。
	
	ADDコマンドでは、エントリの有効期限を指定することもできます。
	マスクの前に、数値と単位の組合わせで現在からの有効期間を指定
	します。単位は「d」(日)、「h」(時間)、「m」(分) のいずれかで、
	例えば「+90m」または「+1h30m」を指定すると、エントリが１時間
	30分後に自動的に消去されます。単位を指定しない場合は日数と解
	釈されます。なお、「+」で始まるマスクを追加する時は、有効期
	限と解釈されてしまうため、必ず有効期限を指定しなければなりま
	せん。
	
	DELコマンドで、指定されたマスクをリストから消去できます。番
	号リストを指定することもできます（エントリ番号はLISTやVIEWコ
	マンドで確認できます）。
	
	CLEARコマンドで、リストに登録された全てのマスクを一括して消
	去できます。誤用を防ぐため、「ALL」パラメータは必須です。
	
	MOVEコマンドで、指定されたエントリを別の位置に移動できます。
	新しい番号がすでに他のエントリに割り当てられている場合、その
	エントリの前に挿入し、番号が重複しないように以降のエントリの
	番号を自動的に調整します。
	
	LISTとVIEWコマンドで、リストを表示できます。パターンを指定し
	た場合は、一致しないマスクは表示されません（なお、開始位置で
	表示の開始位置も指定できる。例えば+50で51個目の該当マスクか
	ら表示します）。LISTがマスクのみを表示するのに対して、VIEWは
	より詳細な情報（マスク追加したユーザのニック、有効期限、使用
	状況など）を併せて表示します。
	
	CHECKコマンドで、特定のホスト名が当てはまるエントリを確認で
	きます。あるユーザがどの例外マスクに該当するかを調べることが
	できます。
	
	COUNTコマンドで、リストに登録されているエントリ数を表示でき
	ます。
	
	なお、同じホストに適用できるエントリが複数ある場合、リストの
	一番上（番号が一番若い）ものが適用されます。
	
	このコマンドの使用はServicesオペレータに制限されています。

OPER_HELP_SESSION
	用法：SESSION LIST 下限
	　　　SESSION VIEW ホスト名
	
	セッションリスト（どのホストから、何人のユーザが接続している
	かのリスト）を表示します。
	
	LISTコマンドで、指定された「下限」以上のユーザが接続している
	ホストを表示します。「1」を指定すると、全てのホストが該当し
	て大量の情報が表示されてしまうため、2以上の数値を指定しなけ
	ればなりません。
	
	VIEWコマンドで、特定のホストの現在ユーザ数と上限を表示します。
	「ホスト名」ではワイルドカードを使用できません。
	
	セッション制限機能の詳しい説明については、EXCEPTIONコマンド
	のヘルプ情報をご覧ください。
	
	このコマンドの使用はServicesオペレータに制限されています。

OPER_HELP_SU
	用法：SU パスワード
	
	Servicesスーパーユーザ権を取得します。パスワードはSET SUPASS
	コマンドで設定したものです。Servicesスーパーユーザ権は、IRC
	から切断するまで有効です。
	
	警告：パスワードが正しくない場合、全オペレータに警告メッセー
	　　　ジが送信されます。
	
	このコマンドの使用はServicesアドミンに制限されています。

OPER_HELP_SET
	用法：SET 設定名 値
	
	Services全体の設定を変更します。「設定名」には以下のものが使
	えます。
	
	    READONLY   読み取り専用モード設定を行う
	    DEBUG      デバッグレベルを変更する
	    SUPASS     SUコマンド用のパスワードを設定する
	
	このコマンドの使用はServicesアドミンに制限されています
	（SUPASSはServicesスーパーユーザのみです）。

OPER_HELP_SET_READONLY
	用法：SET READONLY {ON | OFF}
	
	読み取り専用モードに移行したり、通常モードに戻ったりします。
	読み取り専用モードでは、Servicesアドミン以外のユーザはニック
	やチャンネルのアクセスリストなどのデータを変更できず（自動切
	断リストはServicesオペレータでも変更可）、変更した場合でも、
	SET READONLY OFFで通常モードに戻さないと変更は保存されません。
	
	このコマンドはコマンドラインオプション「-readonly」と同様に
	動作します。

OPER_HELP_SET_DEBUG
	用法：SET DEBUG {ON | OFF | レベル}
	
	デバッグレベルを変更します。ONまた１にすると、ServicesとIRC
	ネットワークとのデータ送受信や他のデバッグ情報がログファイル
	に出力されます。レベルを２以上に設定すると、更に詳細なデバッ
	グ情報が出力されます（このためログファイルのサイズが急増する
	こともありますので要注意）。
	
	このコマンドはコマンドラインオプション「-debug」と同様に動作
	します。

OPER_HELP_SET_SUPASS
	用法：SET SUPASS [パスワード]
	
	SUコマンド用のパスワードを設定します。SUコマンドを使う前に、
	このコマンドでパスワードを設定しなければなりません。パスワー
	ドを指定しない場合はSUコマンドが使用できなくなります。
	
	このコマンドの使用はServicesスーパーユーザに制限されています。

OPER_HELP_JUPE
	用法：JUPE サーバ名 [理由]
	
	サーバを「ジュピター」します。ジュピター（jupiter）とは、サ
	ーバをネットに接続させないように疑似サーバーをネットに接続す
	ることで、ハッキングされたり、設定が正しくないサーバがネット
	ワーク全体に影響を及ぼさないために使われることが多いです。疑
	似サーバはIRCの/SQUITコマンドで切断（解除）することが出来ま
	す。疑似サーバの情報フィールドには、「理由」を指定すればその
	文字列が、指定しなければサーバ名そのままが入ります。
	
	同じサーバ名のサーバがIRCネットワーク上に存在する場合は、自
	動的に切断されます。
	
	このコマンドの使用はServicesアドミンに制限されています。

OPER_HELP_RAW
	用法：RAW テキスト
	
	Servicesが接続しているサーバに文字列を送信します。但し、
	Services自体はこの文字列を処理しません。
	
	警告：このコマンドは、IRCサーバやServices自体の試験のために
	　　　用意されたものです。誤って使うと、Servicesをクラッシュ
	　　　させたり、ネットワーク全体を混乱をさせたりする可能性が
	　　　ありますので、使い方には充分ご注意ください。
	
	このコマンドの使用はServicesスーパーユーザに制限されています。

OPER_HELP_UPDATE
	用法：UPDATE [FORCE]
	
	データベースファイルの更新間隔にかかわらず、データベースをす
	ぐにディスクに書き込みます。「Databases are locked (データベ
	ースがロックされています)」エラーが発生した場合、FORCEオプシ
	ョンを指定することでロックを解除することができます。
	
	このコマンドの使用はServicesアドミンに制限されています。

OPER_HELP_QUIT
	用法：QUIT
	
	Servicesをすぐに終了させます。データベースは保存されません。
	データベースを保存して終了させる場合はSHUTDOWNコマンドを使っ
	てください。
	
	このコマンドの使用はServicesアドミンに制限されています。

OPER_HELP_SHUTDOWN
	用法：SHUTDOWN
	
	データベースを保存してServicesを終了させます。
	
	このコマンドの使用はServicesアドミンに制限されています。

OPER_HELP_RESTART
	用法：RESTART
	
	データベースを保存してServicesを再起動します。
	
	このコマンドの使用はServicesアドミンに制限されています。

OPER_HELP_REHASH
	用法：REHASH
	
	環境設定ファイルを再読み込みし、Servicesの設定を更新します。
	但し、再読み込みで更新できない設定もあり（RemoteServer等）、
	これらの設定が設定ファイルで変更されている場合、警告メッセー
	ジが送信されます。
	
	メイン設定ファイル（%s）内で「LoadModule」行が
	追加または削除されている場合、モジュールのロード・アンロード
	も行います。アンロードの順序はロードと逆順で、アンロードが完
	了し、残るモジュールの設定が更新された後、追加されたモジュー
	ルが設定ファイルに記述された順にロードされます。途中でエラー
	が発生した場合は設定更新を中止します。
	
	なお、モジュール依存のため、REHASHコマンドでアンロードできな
	いモジュールもあります。このようなモジュールをアンロードする
	には、Servicesを再起動する必要があります。
	
	このコマンドの使用はServicesアドミンに制限されています。

OPER_HELP_KILLCLONES
	用法：KILLCLONES ニック
	
	指定されたニックと同じホストから接続しているユーザを全て切断
	します。すぐに再接続することを防ぐため、一時的な自動切断マス
	クが「*@ホスト名」の形で追加されます。また、コマンドを使
	用したユーザのニック、対象となったニック、そして切断されたユ
	ーザ数をWALLOPSでIRCオペレータに周知します。
	
	このコマンドの使用はServicesオペレータに制限されています。


###########################################################################
#
# StatServ help messages
#
###########################################################################

STAT_HELP
	%Sは、ネットワークの統計情報を提供するサービスです。
	「/msg %S HELP COMMANDS」でコマンド一覧を表示できます。

STAT_HELP_COMMANDS
	%Sでは、下記コマンドが使えます。
	
	    SERVERS     サーバ統計情報
	    USERS       ユーザ統計情報

STAT_HELP_SERVERS
	用法：SERVERS STATS
	　　　SERVERS LIST [パターン]
	　　　SERVERS VIEW [パターン] [ONLINE | OFFLINE]
	
	サーバについての統計情報を表示します。
	
	STATSコマンドで、ネットワークに接続している（オンライン）サ
	ーバと接続していない（オフライン）サーバの数と、最近切断され
	たサーバの名前を表示します。
	
	LISTコマンドで、オンラインサーバの一覧とユーザ、オペレータ数
	を表示します。
	
	VIEWコマンドで、全てのサーバの詳細情報を表示します。ONLINEま
	たはOFFLINEを指定することで、それぞれオンラインやオフライン
	サーバに表示を限定させることができます。
	
	パターンを指定すると、そのパターンに一致しないサーバは表示さ
	れません。

STAT_HELP_USERS
	用法：USERS STATS
	
	ユーザについての統計情報を表示します。

STAT_OPER_HELP_SERVERS
	用法：SERVERS STATS
	　　　SERVERS LIST [パターン]
	　　　SERVERS VIEW [パターン] [ONLINE | OFFLINE]
	　　　SERVERS DELETE サーバ
	　　　SERVERS COPY サーバ 新サーバ名
	　　　SERVERS RENAME サーバ 新サーバ名
	
	サーバについての統計情報を表示します。
	
	STATSコマンドで、ネットワークに接続している（オンライン）サ
	ーバと接続していない（オフライン）サーバの数と、最近切断され
	たサーバの名前を表示します。
	
	LISTコマンドで、オンラインサーバの一覧とユーザ、オペレータ数
	を表示します。
	
	VIEWコマンドで、全てのサーバの詳細情報を表示します。ONLINEま
	たはOFFLINEを指定することで、それぞれオンラインやオフライン
	サーバに表示を限定させることができます。
	
	パターンを指定すると、そのパターンに一致しないサーバは表示さ
	れません。
	
	なお、DELETE（サーバ情報を削除する）、COPY（サーバ情報を複製
	する）、RENAME（サーバ情報の名前を変更する）の３つのコマンド
	の使用は、Servicesアドミンに制限されています。

###########################################################################

# CVS: $Revision: 2.180 $
