#define LANG_NAME                        0
#define LANG_CHARSET                     1
#define STRFTIME_DATE_TIME_FORMAT        2
#define STRFTIME_LONG_DATE_FORMAT        3
#define STRFTIME_SHORT_DATE_FORMAT       4
#define STRFTIME_DAYS_SHORT              5
#define STRFTIME_DAYS_LONG               6
#define STRFTIME_MONTHS_SHORT            7
#define STRFTIME_MONTHS_LONG             8
#define STR_DAY                          9
#define STR_DAYS                         10
#define STR_HOUR                         11
#define STR_HOURS                        12
#define STR_MINUTE                       13
#define STR_MINUTES                      14
#define STR_SECOND                       15
#define STR_SECONDS                      16
#define STR_TIMESEP                      17
#define COMMA_SPACE                      18
#define INTERNAL_ERROR                   19
#define SERVICES_IS_BUSY                 20
#define UNKNOWN_COMMAND                  21
#define UNKNOWN_COMMAND_HELP             22
#define SYNTAX_ERROR                     23
#define MORE_INFO                        24
#define NO_HELP_AVAILABLE                25
#define MISSING_QUOTE                    26
#define BAD_EMAIL                        27
#define REJECTED_EMAIL                   28
#define BAD_URL                          29
#define BAD_USERHOST_MASK                30
#define BAD_NICKUSERHOST_MASK            31
#define BAD_EXPIRY_TIME                  32
#define SENDMAIL_NO_RESOURCES            33
#define READ_ONLY_MODE                   34
#define PASSWORD_INCORRECT               35
#define PASSWORD_WARNING                 36
#define ACCESS_DENIED                    37
#define PERMISSION_DENIED                38
#define MORE_OBSCURE_PASSWORD            39
#define NICK_NOT_REGISTERED              40
#define NICK_NOT_REGISTERED_HELP         41
#define NICK_TOO_LONG                    42
#define NICK_INVALID                     43
#define NICK_X_NOT_REGISTERED            44
#define NICK_X_ALREADY_REGISTERED        45
#define NICK_X_NOT_IN_USE                46
#define NICK_X_FORBIDDEN                 47
#define NICK_X_SUSPENDED                 48
#define NICK_X_SUSPENDED_MEMOS           49
#define NICK_IDENTIFY_REQUIRED           50
#define NICK_PLEASE_AUTH                 51
#define NICK_X_NOT_ON_CHAN_X             52
#define CHAN_INVALID                     53
#define CHAN_X_NOT_REGISTERED            54
#define CHAN_X_NOT_IN_USE                55
#define CHAN_X_FORBIDDEN                 56
#define CHAN_X_SUSPENDED                 57
#define CHAN_X_SUSPENDED_MEMOS           58
#define CHAN_IDENTIFY_REQUIRED           59
#define SERV_X_NOT_FOUND                 60
#define EXPIRES_NONE                     61
#define EXPIRES_NOW                      62
#define EXPIRES_IN                       63
#define LIST_RESULTS                     64
#define NICK_IS_REGISTERED               65
#define NICK_IS_SECURE                   66
#define NICK_MAY_NOT_BE_USED             67
#define DISCONNECT_IN_1_MINUTE           68
#define DISCONNECT_IN_20_SECONDS         69
#define DISCONNECT_NOW                   70
#define FORCENICKCHANGE_IN_1_MINUTE      71
#define FORCENICKCHANGE_IN_20_SECONDS    72
#define FORCENICKCHANGE_NOW              73
#define NICK_EXPIRES_SOON                74
#define NICK_EXPIRED                     75
#define NICK_REGISTER_SYNTAX             76
#define NICK_REGISTER_REQ_EMAIL_SYNTAX   77
#define NICK_REGISTRATION_DISABLED       78
#define NICK_REGISTRATION_FAILED         79
#define NICK_REG_PLEASE_WAIT             80
#define NICK_REG_PLEASE_WAIT_FIRST       81
#define NICK_CANNOT_BE_REGISTERED        82
#define NICK_REGISTER_EMAIL_UNAUTHED     83
#define NICK_REGISTER_TOO_MANY_NICKS     84
#define NICK_REGISTERED                  85
#define NICK_PASSWORD_IS                 86
#define NICK_IDENTIFY_SYNTAX             87
#define NICK_IDENTIFY_FAILED             88
#define NICK_IDENTIFY_EMAIL_MISSING      89
#define NICK_IDENTIFY_SUCCEEDED          90
#define NICK_DROP_SYNTAX                 91
#define NICK_DROP_WARNING                92
#define NICK_DROP_DISABLED               93
#define NICK_DROP_FAILED                 94
#define NICK_DROPPED                     95
#define NICK_DROPPED_LINKS               96
#define NICK_DROPNICK_SYNTAX             97
#define NICK_X_DROPPED                   98
#define NICK_X_DROPPED_LINKS             99
#define NICK_DROPEMAIL_SYNTAX            100
#define NICK_DROPEMAIL_PATTERN_TOO_LONG  101
#define NICK_DROPEMAIL_NONE              102
#define NICK_DROPEMAIL_COUNT             103
#define NICK_DROPEMAIL_CONFIRM_SYNTAX    104
#define NICK_DROPEMAIL_CONFIRM_UNKNOWN   105
#define NICK_DROPEMAIL_CONFIRM_DROPPING  106
#define NICK_DROPEMAIL_CONFIRM_DROPPED   107
#define NICK_SET_SYNTAX                  108
#define NICK_SET_OPER_SYNTAX             109
#define NICK_SET_DISABLED                110
#define NICK_SET_UNKNOWN_OPTION          111
#define NICK_SET_PASSWORD_FAILED         112
#define NICK_SET_PASSWORD_CHANGED        113
#define NICK_SET_PASSWORD_CHANGED_TO     114
#define NICK_SET_LANGUAGE_SYNTAX         115
#define NICK_SET_LANGUAGE_UNKNOWN        116
#define NICK_SET_LANGUAGE_CHANGED        117
#define NICK_SET_URL_CHANGED             118
#define NICK_SET_EMAIL_PLEASE_WAIT       119
#define NICK_SET_EMAIL_UNAUTHED          120
#define NICK_SET_EMAIL_TOO_MANY_NICKS    121
#define NICK_SET_EMAIL_CHANGED           122
#define NICK_SET_INFO_CHANGED            123
#define NICK_SET_KILL_SYNTAX             124
#define NICK_SET_KILL_IMMED_SYNTAX       125
#define NICK_SET_KILL_ON                 126
#define NICK_SET_KILL_QUICK              127
#define NICK_SET_KILL_IMMED              128
#define NICK_SET_KILL_IMMED_DISABLED     129
#define NICK_SET_KILL_OFF                130
#define NICK_SET_SECURE_SYNTAX           131
#define NICK_SET_SECURE_ON               132
#define NICK_SET_SECURE_OFF              133
#define NICK_SET_PRIVATE_SYNTAX          134
#define NICK_SET_PRIVATE_ON              135
#define NICK_SET_PRIVATE_OFF             136
#define NICK_SET_NOOP_SYNTAX             137
#define NICK_SET_NOOP_ON                 138
#define NICK_SET_NOOP_OFF                139
#define NICK_SET_HIDE_SYNTAX             140
#define NICK_SET_HIDE_EMAIL_ON           141
#define NICK_SET_HIDE_EMAIL_OFF          142
#define NICK_SET_HIDE_MASK_ON            143
#define NICK_SET_HIDE_MASK_OFF           144
#define NICK_SET_HIDE_QUIT_ON            145
#define NICK_SET_HIDE_QUIT_OFF           146
#define NICK_SET_TIMEZONE_SYNTAX         147
#define NICK_SET_TIMEZONE_TO             148
#define NICK_SET_TIMEZONE_DEFAULT        149
#define NICK_SET_MAINNICK_NOT_FOUND      150
#define NICK_SET_MAINNICK_CHANGED        151
#define NICK_SET_NOEXPIRE_SYNTAX         152
#define NICK_SET_NOEXPIRE_ON             153
#define NICK_SET_NOEXPIRE_OFF            154
#define NICK_UNSET_SYNTAX                155
#define NICK_UNSET_SYNTAX_REQ_EMAIL      156
#define NICK_UNSET_OPER_SYNTAX           157
#define NICK_UNSET_OPER_SYNTAX_REQ_EMAIL 158
#define NICK_UNSET_URL                   159
#define NICK_UNSET_EMAIL                 160
#define NICK_UNSET_INFO                  161
#define NICK_UNSET_EMAIL_BAD             162
#define NICK_UNSET_EMAIL_OTHER_BAD       163
#define NICK_ACCESS_SYNTAX               164
#define NICK_ACCESS_DISABLED             165
#define NICK_ACCESS_ALREADY_PRESENT      166
#define NICK_ACCESS_REACHED_LIMIT        167
#define NICK_ACCESS_NO_NICKS             168
#define NICK_ACCESS_ADDED                169
#define NICK_ACCESS_NOT_FOUND            170
#define NICK_ACCESS_DELETED              171
#define NICK_ACCESS_LIST_EMPTY           172
#define NICK_ACCESS_LIST                 173
#define NICK_ACCESS_LIST_X_EMPTY         174
#define NICK_ACCESS_LIST_X               175
#define NICK_OLD_LINK_SYNTAX             176
#define NICK_LINK_SYNTAX                 177
#define NICK_LINK_DISABLED               178
#define NICK_LINK_FAILED                 179
#define NICK_CANNOT_BE_LINKED            180
#define NICK_OLD_LINK_SAME               181
#define NICK_LINK_SAME                   182
#define NICK_LINK_ALREADY_LINKED         183
#define NICK_LINK_IN_USE                 184
#define NICK_LINK_TOO_MANY               185
#define NICK_LINK_TOO_MANY_NICKS         186
#define NICK_OLD_LINK_TOO_MANY_CHANNELS  187
#define NICK_OLD_LINKED                  188
#define NICK_LINKED                      189
#define NICK_OLD_UNLINK_SYNTAX           190
#define NICK_UNLINK_SYNTAX               191
#define NICK_UNLINK_OPER_SYNTAX          192
#define NICK_UNLINK_DISABLED             193
#define NICK_UNLINK_FAILED               194
#define NICK_UNLINK_SAME                 195
#define NICK_OLD_UNLINK_NOT_LINKED       196
#define NICK_UNLINK_NOT_LINKED_YOURS     197
#define NICK_UNLINK_NOT_LINKED           198
#define NICK_OLD_UNLINKED                199
#define NICK_UNLINKED                    200
#define NICK_X_UNLINKED                  201
#define NICK_OLD_LISTLINKS_SYNTAX        202
#define NICK_LISTLINKS_SYNTAX            203
#define NICK_LISTLINKS_HEADER            204
#define NICK_LISTLINKS_FOOTER            205
#define NICK_INFO_SYNTAX                 206
#define NICK_INFO_REALNAME               207
#define NICK_INFO_ADDRESS                208
#define NICK_INFO_ADDRESS_ONLINE         209
#define NICK_INFO_ADDRESS_ONLINE_NOHOST  210
#define NICK_INFO_ADDRESS_OTHER_NICK     211
#define NICK_INFO_TIME_REGGED            212
#define NICK_INFO_LAST_SEEN              213
#define NICK_INFO_LAST_QUIT              214
#define NICK_INFO_URL                    215
#define NICK_INFO_EMAIL                  216
#define NICK_INFO_EMAIL_UNAUTHED         217
#define NICK_INFO_INFO                   218
#define NICK_INFO_OPTIONS                219
#define NICK_INFO_OPT_KILL               220
#define NICK_INFO_OPT_SECURE             221
#define NICK_INFO_OPT_PRIVATE            222
#define NICK_INFO_OPT_NOOP               223
#define NICK_INFO_OPT_NONE               224
#define NICK_INFO_NO_EXPIRE              225
#define NICK_INFO_SUSPEND_DETAILS        226
#define NICK_INFO_SUSPEND_REASON         227
#define NICK_INFO_SHOW_ALL               228
#define NICK_LISTCHANS_SYNTAX            229
#define NICK_LISTCHANS_NONE              230
#define NICK_LISTCHANS_HEADER            231
#define NICK_LISTCHANS_END               232
#define NICK_LIST_SYNTAX                 233
#define NICK_LIST_OPER_SYNTAX            234
#define NICK_LIST_OPER_SYNTAX_AUTH       235
#define NICK_LIST_HEADER                 236
#define NICK_LIST_NO_MATCH               237
#define NICK_LISTEMAIL_NONE              238
#define NICK_RECOVER_SYNTAX              239
#define NICK_NO_RECOVER_SELF             240
#define NICK_RECOVERED                   241
#define NICK_RELEASE_SYNTAX              242
#define NICK_RELEASE_NOT_HELD            243
#define NICK_RELEASED                    244
#define NICK_GHOST_SYNTAX                245
#define NICK_NO_GHOST_SELF               246
#define NICK_GHOST_KILLED                247
#define NICK_GETPASS_SYNTAX              248
#define NICK_GETPASS_UNAVAILABLE         249
#define NICK_GETPASS_PASSWORD_IS         250
#define NICK_FORBID_SYNTAX               251
#define NICK_FORBID_SUCCEEDED            252
#define NICK_FORBID_FAILED               253
#define NICK_SUSPEND_SYNTAX              254
#define NICK_SUSPEND_SUCCEEDED           255
#define NICK_SUSPEND_ALREADY_SUSPENDED   256
#define NICK_UNSUSPEND_SYNTAX            257
#define NICK_UNSUSPEND_SUCCEEDED         258
#define NICK_UNSUSPEND_NOT_SUSPENDED     259
#define NICK_AJOIN_SYNTAX                260
#define NICK_AJOIN_ADD_SYNTAX            261
#define NICK_AJOIN_DEL_SYNTAX            262
#define NICK_AJOIN_DISABLED              263
#define NICK_AJOIN_ALREADY_PRESENT       264
#define NICK_AJOIN_LIST_FULL             265
#define NICK_AJOIN_ADDED                 266
#define NICK_AJOIN_NOT_FOUND             267
#define NICK_AJOIN_DELETED               268
#define NICK_AJOIN_LIST_EMPTY            269
#define NICK_AJOIN_LIST                  270
#define NICK_AJOIN_LIST_X_EMPTY          271
#define NICK_AJOIN_LIST_X                272
#define NICK_AJOIN_AUTO_REMOVE           273
#define NICK_AUTH_SENDING                274
#define NICK_AUTH_FOR_REGISTER           275
#define NICK_AUTH_FOR_SET_EMAIL          276
#define NICK_AUTH_NOT_NEEDED             277
#define NICK_AUTH_NO_AUTHCODE            278
#define NICK_AUTH_HAS_AUTHCODE           279
#define PASSWORD_WARNING_FOR_AUTH        280
#define NICK_AUTH_MAIL_SUBJECT           281
#define NICK_AUTH_MAIL_BODY              282
#define NICK_AUTH_MAIL_TEXT_REG          283
#define NICK_AUTH_MAIL_TEXT_EMAIL        284
#define NICK_AUTH_MAIL_TEXT_SENDAUTH     285
#define NICK_AUTH_MAIL_TEXT_REAUTH       286
#define NICK_AUTH_MAIL_BODY_SETAUTH      287
#define NICK_AUTH_SYNTAX                 288
#define NICK_AUTH_DISABLED               289
#define NICK_AUTH_FAILED                 290
#define NICK_AUTH_SUCCEEDED_REGISTER     291
#define NICK_AUTH_SUCCEEDED_SET_EMAIL    292
#define NICK_AUTH_SUCCEEDED_REAUTH       293
#define NICK_AUTH_SUCCEEDED_SETAUTH      294
#define NICK_SENDAUTH_SYNTAX             295
#define NICK_SENDAUTH_TOO_SOON           296
#define NICK_SENDAUTH_NORESOURCES        297
#define NICK_SENDAUTH_ERROR              298
#define NICK_REAUTH_SYNTAX               299
#define NICK_REAUTH_HAVE_AUTHCODE        300
#define NICK_REAUTH_NO_EMAIL             301
#define NICK_REAUTH_AUTHCODE_SET         302
#define NICK_RESTOREMAIL_SYNTAX          303
#define NICK_RESTOREMAIL_NOT_NOW         304
#define NICK_RESTOREMAIL_DONE            305
#define NICK_SETAUTH_SYNTAX              306
#define NICK_SETAUTH_NO_EMAIL            307
#define NICK_SETAUTH_AUTHCODE_SET        308
#define NICK_SETAUTH_SEND_REFUSED        309
#define NICK_SETAUTH_SEND_TIMEOUT        310
#define NICK_SETAUTH_SEND_NORESOURCES    311
#define NICK_SETAUTH_SEND_ERROR          312
#define NICK_SETAUTH_USER_NOTICE         313
#define NICK_GETAUTH_SYNTAX              314
#define NICK_GETAUTH_AUTHCODE_IS         315
#define NICK_CLEARAUTH_SYNTAX            316
#define NICK_CLEARAUTH_CLEARED           317
#define CHAN_LEVEL_AUTOOP                318
#define CHAN_LEVEL_AUTOVOICE             319
#define CHAN_LEVEL_INVITE                320
#define CHAN_LEVEL_AKICK                 321
#define CHAN_LEVEL_SET                   322
#define CHAN_LEVEL_CLEAR                 323
#define CHAN_LEVEL_UNBAN                 324
#define CHAN_LEVEL_OPDEOP                325
#define CHAN_LEVEL_ACCESS_LIST           326
#define CHAN_LEVEL_ACCESS_CHANGE         327
#define CHAN_LEVEL_MEMO                  328
#define CHAN_LEVEL_VOICE                 329
#define CHAN_LEVEL_AUTOHALFOP            330
#define CHAN_LEVEL_HALFOP                331
#define CHAN_LEVEL_AUTOPROTECT           332
#define CHAN_LEVEL_PROTECT               333
#define CHAN_LEVEL_KICK                  334
#define CHAN_LEVEL_STATUS                335
#define CHAN_LEVEL_TOPIC                 336
#define CHAN_IS_REGISTERED               337
#define CHAN_MAY_NOT_BE_USED             338
#define CHAN_NOT_ALLOWED_TO_JOIN         339
#define CHAN_MUST_IDENTIFY_NICK          340
#define CHAN_BOUNCY_MODES                341
#define CHAN_REGISTER_SYNTAX             342
#define CHAN_REGISTER_DISABLED           343
#define CHAN_REGISTER_SHORT_CHANNEL      344
#define CHAN_REGISTER_NOT_LOCAL          345
#define CHAN_REGISTER_INVALID_NAME       346
#define CHAN_MUST_REGISTER_NICK          347
#define CHAN_MAY_NOT_BE_REGISTERED       348
#define CHAN_ALREADY_REGISTERED          349
#define CHAN_MUST_BE_CHANOP              350
#define CHAN_REACHED_CHANNEL_LIMIT       351
#define CHAN_EXCEEDED_CHANNEL_LIMIT      352
#define CHAN_REGISTRATION_FAILED         353
#define CHAN_REGISTERED                  354
#define CHAN_PASSWORD_IS                 355
#define CHAN_IDENTIFY_SYNTAX             356
#define CHAN_IDENTIFY_FAILED             357
#define CHAN_IDENTIFY_SUCCEEDED          358
#define CHAN_DROP_SYNTAX                 359
#define CHAN_DROP_DISABLED               360
#define CHAN_DROPPED                     361
#define CHAN_DROPCHAN_SYNTAX             362
#define CHAN_SET_SYNTAX                  363
#define CHAN_SET_DISABLED                364
#define CHAN_SET_UNKNOWN_OPTION          365
#define CHAN_SET_FOUNDER_TOO_MANY_CHANS  366
#define CHAN_FOUNDER_CHANGED             367
#define CHAN_SUCCESSOR_CHANGED           368
#define CHAN_SUCCESSOR_IS_FOUNDER        369
#define CHAN_SET_PASSWORD_FAILED         370
#define CHAN_PASSWORD_CHANGED            371
#define CHAN_PASSWORD_CHANGED_TO         372
#define CHAN_DESC_CHANGED                373
#define CHAN_URL_CHANGED                 374
#define CHAN_EMAIL_CHANGED               375
#define CHAN_ENTRY_MSG_CHANGED           376
#define CHAN_SET_MLOCK_NEED_PLUS_MINUS   377
#define CHAN_SET_MLOCK_NEED_PARAM        378
#define CHAN_SET_MLOCK_NEED_POSITIVE     379
#define CHAN_SET_MLOCK_MODE_REG_BAD      380
#define CHAN_SET_MLOCK_LINK_BAD          381
#define CHAN_SET_MLOCK_LINK_SAME         382
#define CHAN_SET_MLOCK_REQUIRES          383
#define CHAN_SET_MLOCK_BAD_PARAM         384
#define CHAN_SET_MLOCK_UNKNOWN_CHAR      385
#define CHAN_SET_MLOCK_CANNOT_LOCK       386
#define CHAN_MLOCK_CHANGED               387
#define CHAN_MLOCK_REMOVED               388
#define CHAN_SET_HIDE_SYNTAX             389
#define CHAN_SET_HIDE_EMAIL_ON           390
#define CHAN_SET_HIDE_EMAIL_OFF          391
#define CHAN_SET_HIDE_TOPIC_ON           392
#define CHAN_SET_HIDE_TOPIC_OFF          393
#define CHAN_SET_HIDE_MLOCK_ON           394
#define CHAN_SET_HIDE_MLOCK_OFF          395
#define CHAN_SET_KEEPTOPIC_SYNTAX        396
#define CHAN_SET_KEEPTOPIC_ON            397
#define CHAN_SET_KEEPTOPIC_OFF           398
#define CHAN_SET_TOPICLOCK_SYNTAX        399
#define CHAN_SET_TOPICLOCK_ON            400
#define CHAN_SET_TOPICLOCK_OFF           401
#define CHAN_SET_PRIVATE_SYNTAX          402
#define CHAN_SET_PRIVATE_ON              403
#define CHAN_SET_PRIVATE_OFF             404
#define CHAN_SET_SECUREOPS_SYNTAX        405
#define CHAN_SET_SECUREOPS_ON            406
#define CHAN_SET_SECUREOPS_OFF           407
#define CHAN_SET_LEAVEOPS_SYNTAX         408
#define CHAN_SET_LEAVEOPS_ON             409
#define CHAN_SET_LEAVEOPS_OFF            410
#define CHAN_SET_RESTRICTED_SYNTAX       411
#define CHAN_SET_RESTRICTED_ON           412
#define CHAN_SET_RESTRICTED_OFF          413
#define CHAN_SET_SECURE_SYNTAX           414
#define CHAN_SET_SECURE_ON               415
#define CHAN_SET_SECURE_OFF              416
#define CHAN_SET_OPNOTICE_SYNTAX         417
#define CHAN_SET_OPNOTICE_ON             418
#define CHAN_SET_OPNOTICE_OFF            419
#define CHAN_SET_ENFORCE_SYNTAX          420
#define CHAN_SET_ENFORCE_ON              421
#define CHAN_SET_ENFORCE_OFF             422
#define CHAN_SET_MEMO_RESTRICTED_SYNTAX  423
#define CHAN_SET_MEMO_RESTRICTED_ON      424
#define CHAN_SET_MEMO_RESTRICTED_OFF     425
#define CHAN_SET_NOEXPIRE_SYNTAX         426
#define CHAN_SET_NOEXPIRE_ON             427
#define CHAN_SET_NOEXPIRE_OFF            428
#define CHAN_UNSET_SYNTAX                429
#define CHAN_SUCCESSOR_UNSET             430
#define CHAN_URL_UNSET                   431
#define CHAN_EMAIL_UNSET                 432
#define CHAN_ENTRY_MSG_UNSET             433
#define CHAN_ACCESS_SYNTAX               434
#define CHAN_ACCESS_ADD_SYNTAX           435
#define CHAN_ACCESS_DEL_SYNTAX           436
#define CHAN_ACCESS_LIST_SYNTAX          437
#define CHAN_ACCESS_LISTLEVEL_SYNTAX     438
#define CHAN_ACCESS_COUNT_SYNTAX         439
#define CHAN_ACCESS_DISABLED             440
#define CHAN_ACCESS_LEVEL_NONZERO        441
#define CHAN_ACCESS_LEVEL_RANGE          442
#define CHAN_ACCESS_NICKS_ONLY           443
#define CHAN_ACCESS_NOOP                 444
#define CHAN_ACCESS_REACHED_LIMIT        445
#define CHAN_ACCESS_LEVEL_UNCHANGED      446
#define CHAN_ACCESS_LEVEL_CHANGED        447
#define CHAN_ACCESS_ADDED                448
#define CHAN_ACCESS_NOT_FOUND            449
#define CHAN_ACCESS_DELETED              450
#define CHAN_ACCESS_LIST_EMPTY           451
#define CHAN_ACCESS_NO_MATCH             452
#define CHAN_ACCESS_LIST_HEADER          453
#define CHAN_ACCESS_LIST_FORMAT          454
#define CHAN_ACCESS_COUNT                455
#define CHAN_SOP_SYNTAX                  456
#define CHAN_AOP_SYNTAX                  457
#define CHAN_HOP_SYNTAX                  458
#define CHAN_VOP_SYNTAX                  459
#define CHAN_NOP_SYNTAX                  460
#define CHAN_SOP_LIST_SYNTAX             461
#define CHAN_AOP_LIST_SYNTAX             462
#define CHAN_HOP_LIST_SYNTAX             463
#define CHAN_VOP_LIST_SYNTAX             464
#define CHAN_NOP_LIST_SYNTAX             465
#define CHAN_XOP_ADDED                   466
#define CHAN_XOP_LEVEL_CHANGED           467
#define CHAN_XOP_LEVEL_UNCHANGED         468
#define CHAN_XOP_NICKS_ONLY              469
#define CHAN_XOP_NOOP                    470
#define CHAN_XOP_NICKS_ONLY_HOP          471
#define CHAN_XOP_REACHED_LIMIT           472
#define CHAN_XOP_REACHED_LIMIT_HOP       473
#define CHAN_XOP_NOT_FOUND               474
#define CHAN_XOP_DELETED                 475
#define CHAN_XOP_LIST_EMPTY              476
#define CHAN_XOP_NO_MATCH                477
#define CHAN_XOP_LIST_HEADER             478
#define CHAN_XOP_COUNT                   479
#define CHAN_AKICK_SYNTAX                480
#define CHAN_AKICK_LIST_SYNTAX           481
#define CHAN_AKICK_VIEW_SYNTAX           482
#define CHAN_AKICK_DISABLED              483
#define CHAN_AKICK_ALREADY_EXISTS        484
#define CHAN_AKICK_REACHED_LIMIT         485
#define CHAN_AKICK_ADDED                 486
#define CHAN_AKICK_NOT_FOUND             487
#define CHAN_AKICK_DELETED               488
#define CHAN_AKICK_LIST_EMPTY            489
#define CHAN_AKICK_NO_MATCH              490
#define CHAN_AKICK_LIST_HEADER           491
#define CHAN_AKICK_VIEW_FORMAT           492
#define CHAN_AKICK_VIEW_UNUSED_FORMAT    493
#define CHAN_AKICK_COUNT                 494
#define CHAN_AKICK_ENFORCE_DONE          495
#define CHAN_LEVELS_SYNTAX               496
#define CHAN_LEVELS_READONLY             497
#define CHAN_LEVELS_RANGE                498
#define CHAN_LEVELS_CHANGED              499
#define CHAN_LEVELS_UNKNOWN              500
#define CHAN_LEVELS_DISABLED             501
#define CHAN_LEVELS_LIST_HEADER          502
#define CHAN_LEVELS_LIST_DISABLED        503
#define CHAN_LEVELS_LIST_FOUNDER         504
#define CHAN_LEVELS_LIST_NORMAL          505
#define CHAN_LEVELS_RESET                506
#define CHAN_INFO_SYNTAX                 507
#define CHAN_INFO_HEADER                 508
#define CHAN_INFO_FOUNDER                509
#define CHAN_INFO_SUCCESSOR              510
#define CHAN_INFO_DESCRIPTION            511
#define CHAN_INFO_ENTRYMSG               512
#define CHAN_INFO_TIME_REGGED            513
#define CHAN_INFO_LAST_USED              514
#define CHAN_INFO_LAST_TOPIC             515
#define CHAN_INFO_TOPIC_SET_BY           516
#define CHAN_INFO_URL                    517
#define CHAN_INFO_EMAIL                  518
#define CHAN_INFO_OPTIONS                519
#define CHAN_INFO_OPT_PRIVATE            520
#define CHAN_INFO_OPT_KEEPTOPIC          521
#define CHAN_INFO_OPT_TOPICLOCK          522
#define CHAN_INFO_OPT_SECUREOPS          523
#define CHAN_INFO_OPT_LEAVEOPS           524
#define CHAN_INFO_OPT_RESTRICTED         525
#define CHAN_INFO_OPT_SECURE             526
#define CHAN_INFO_OPT_OPNOTICE           527
#define CHAN_INFO_OPT_ENFORCE            528
#define CHAN_INFO_OPT_MEMO_RESTRICTED    529
#define CHAN_INFO_OPT_NONE               530
#define CHAN_INFO_MODE_LOCK              531
#define CHAN_INFO_NO_EXPIRE              532
#define CHAN_INFO_SUSPEND_DETAILS        533
#define CHAN_INFO_SUSPEND_REASON         534
#define CHAN_INFO_SHOW_ALL               535
#define CHAN_LIST_SYNTAX                 536
#define CHAN_LIST_OPER_SYNTAX            537
#define CHAN_LIST_HEADER                 538
#define CHAN_LIST_NO_MATCH               539
#define CHAN_INVITE_SYNTAX               540
#define CHAN_INVITE_OK                   541
#define CHAN_OPVOICE_SYNTAX              542
#define CHAN_OP_SUCCEEDED                543
#define CHAN_OP_ALREADY                  544
#define CHAN_OP_FAILED                   545
#define CHAN_DEOP_SUCCEEDED              546
#define CHAN_DEOP_ALREADY                547
#define CHAN_DEOP_FAILED                 548
#define CHAN_VOICE_SUCCEEDED             549
#define CHAN_VOICE_ALREADY               550
#define CHAN_VOICE_FAILED                551
#define CHAN_DEVOICE_SUCCEEDED           552
#define CHAN_DEVOICE_ALREADY             553
#define CHAN_DEVOICE_FAILED              554
#define CHAN_HALFOP_SUCCEEDED            555
#define CHAN_HALFOP_ALREADY              556
#define CHAN_HALFOP_FAILED               557
#define CHAN_DEHALFOP_SUCCEEDED          558
#define CHAN_DEHALFOP_ALREADY            559
#define CHAN_DEHALFOP_FAILED             560
#define CHAN_PROTECT_SUCCEEDED           561
#define CHAN_PROTECT_ALREADY             562
#define CHAN_PROTECT_FAILED              563
#define CHAN_DEPROTECT_SUCCEEDED         564
#define CHAN_DEPROTECT_ALREADY           565
#define CHAN_DEPROTECT_FAILED            566
#define CHAN_UNBAN_SYNTAX                567
#define CHAN_UNBANNED                    568
#define CHAN_KICK_SYNTAX                 569
#define CHAN_KICK_PROTECTED              570
#define CHAN_KICKED                      571
#define CHAN_TOPIC_SYNTAX                572
#define CHAN_CLEAR_SYNTAX                573
#define CHAN_CLEARED_BANS                574
#define CHAN_CLEARED_EXCEPTIONS          575
#define CHAN_CLEARED_INVITES             576
#define CHAN_CLEARED_MODES               577
#define CHAN_CLEARED_OPS                 578
#define CHAN_CLEARED_HALFOPS             579
#define CHAN_CLEARED_VOICES              580
#define CHAN_CLEARED_USERS               581
#define CHAN_GETPASS_SYNTAX              582
#define CHAN_GETPASS_UNAVAILABLE         583
#define CHAN_GETPASS_PASSWORD_IS         584
#define CHAN_FORBID_SYNTAX               585
#define CHAN_FORBID_SHORT_CHANNEL        586
#define CHAN_FORBID_SUCCEEDED            587
#define CHAN_FORBID_FAILED               588
#define CHAN_SUSPEND_SYNTAX              589
#define CHAN_SUSPEND_SUCCEEDED           590
#define CHAN_SUSPEND_ALREADY_SUSPENDED   591
#define CHAN_UNSUSPEND_SYNTAX            592
#define CHAN_UNSUSPEND_SUCCEEDED         593
#define CHAN_UNSUSPEND_NOT_SUSPENDED     594
#define MEMO_HAVE_NEW_MEMO               595
#define MEMO_HAVE_NEW_MEMOS              596
#define MEMO_TYPE_READ_LAST              597
#define MEMO_TYPE_READ_NUM               598
#define MEMO_TYPE_LIST_NEW               599
#define MEMO_AT_LIMIT                    600
#define MEMO_OVER_LIMIT                  601
#define MEMO_NEW_MEMO_ARRIVED            602
#define MEMO_NEW_CHAN_MEMO_ARRIVED       603
#define MEMO_HAVE_NO_MEMOS               604
#define MEMO_HAVE_NO_NEW_MEMOS           605
#define MEMO_DOES_NOT_EXIST              606
#define MEMO_LIST_NOT_FOUND              607
#define MEMO_SEND_SYNTAX                 608
#define MEMO_SEND_DISABLED               609
#define MEMO_SEND_PLEASE_WAIT            610
#define MEMO_SEND_CHAN_NOT_AVAIL         611
#define MEMO_X_GETS_NO_MEMOS             612
#define MEMO_X_HAS_TOO_MANY_MEMOS        613
#define MEMO_SENT                        614
#define MEMO_SEND_FAILED                 615
#define MEMO_LIST_SYNTAX                 616
#define MEMO_LIST_MEMOS                  617
#define MEMO_LIST_NEW_MEMOS              618
#define MEMO_LIST_HEADER                 619
#define MEMO_LIST_FORMAT                 620
#define MEMO_READ_SYNTAX                 621
#define MEMO_HEADER                      622
#define MEMO_CHAN_HEADER                 623
#define MEMO_SAVE_SYNTAX                 624
#define MEMO_SAVED_ONE                   625
#define MEMO_SAVED_SEVERAL               626
#define MEMO_DEL_SYNTAX                  627
#define MEMO_DELETED_NONE                628
#define MEMO_DELETED_ONE                 629
#define MEMO_DELETED_SEVERAL             630
#define MEMO_DELETED_ALL                 631
#define MEMO_RENUMBER_ONLY_YOU           632
#define MEMO_RENUMBER_DONE               633
#define MEMO_SET_SYNTAX                  634
#define MEMO_SET_DISABLED                635
#define MEMO_SET_UNKNOWN_OPTION          636
#define MEMO_SET_NOTIFY_SYNTAX           637
#define MEMO_SET_NOTIFY_ON               638
#define MEMO_SET_NOTIFY_LOGON            639
#define MEMO_SET_NOTIFY_NEW              640
#define MEMO_SET_NOTIFY_OFF              641
#define MEMO_SET_LIMIT_SYNTAX            642
#define MEMO_SET_LIMIT_OPER_SYNTAX       643
#define MEMO_SET_YOUR_LIMIT_FORBIDDEN    644
#define MEMO_SET_LIMIT_FORBIDDEN         645
#define MEMO_SET_YOUR_LIMIT_TOO_HIGH     646
#define MEMO_SET_LIMIT_TOO_HIGH          647
#define MEMO_SET_LIMIT_OVERFLOW          648
#define MEMO_SET_YOUR_LIMIT              649
#define MEMO_SET_YOUR_LIMIT_ZERO         650
#define MEMO_SET_YOUR_LIMIT_DEFAULT      651
#define MEMO_UNSET_YOUR_LIMIT            652
#define MEMO_SET_LIMIT                   653
#define MEMO_SET_LIMIT_ZERO              654
#define MEMO_SET_LIMIT_DEFAULT           655
#define MEMO_UNSET_LIMIT                 656
#define MEMO_INFO_NO_MEMOS               657
#define MEMO_INFO_MEMO                   658
#define MEMO_INFO_MEMO_UNREAD            659
#define MEMO_INFO_MEMOS                  660
#define MEMO_INFO_MEMOS_ONE_UNREAD       661
#define MEMO_INFO_MEMOS_SOME_UNREAD      662
#define MEMO_INFO_MEMOS_ALL_UNREAD       663
#define MEMO_INFO_LIMIT                  664
#define MEMO_INFO_HARD_LIMIT             665
#define MEMO_INFO_LIMIT_ZERO             666
#define MEMO_INFO_HARD_LIMIT_ZERO        667
#define MEMO_INFO_NO_LIMIT               668
#define MEMO_INFO_NOTIFY_OFF             669
#define MEMO_INFO_NOTIFY_ON              670
#define MEMO_INFO_NOTIFY_RECEIVE         671
#define MEMO_INFO_NOTIFY_SIGNON          672
#define MEMO_INFO_X_NO_MEMOS             673
#define MEMO_INFO_X_MEMO                 674
#define MEMO_INFO_X_MEMO_UNREAD          675
#define MEMO_INFO_X_MEMOS                676
#define MEMO_INFO_X_MEMOS_ONE_UNREAD     677
#define MEMO_INFO_X_MEMOS_SOME_UNREAD    678
#define MEMO_INFO_X_MEMOS_ALL_UNREAD     679
#define MEMO_INFO_X_LIMIT                680
#define MEMO_INFO_X_HARD_LIMIT           681
#define MEMO_INFO_X_NO_LIMIT             682
#define MEMO_INFO_X_NOTIFY_OFF           683
#define MEMO_INFO_X_NOTIFY_ON            684
#define MEMO_INFO_X_NOTIFY_RECEIVE       685
#define MEMO_INFO_X_NOTIFY_SIGNON        686
#define MEMO_FORWARD_MAIL_SUBJECT        687
#define MEMO_FORWARD_MULTIPLE_MAIL_SUBJECT 688
#define MEMO_FORWARD_MAIL_BODY           689
#define MEMO_FORWARD_CHANMEMO_MAIL_BODY  690
#define MEMO_FORWARD_SYNTAX              691
#define MEMO_FORWARD_NEED_EMAIL          692
#define MEMO_FORWARD_PLEASE_WAIT         693
#define MEMO_FORWARD_FAILED              694
#define MEMO_FORWARDED_NONE              695
#define MEMO_FORWARDED_ONE               696
#define MEMO_FORWARDED_SEVERAL           697
#define MEMO_FORWARDED_ALL               698
#define MEMO_SET_FORWARD_SYNTAX          699
#define MEMO_SET_FORWARD_ON              700
#define MEMO_SET_FORWARD_COPY            701
#define MEMO_SET_FORWARD_OFF             702
#define MEMO_IGNORE_SYNTAX               703
#define MEMO_IGNORE_ADD_SYNTAX           704
#define MEMO_IGNORE_DEL_SYNTAX           705
#define MEMO_IGNORE_LIST_FULL            706
#define MEMO_IGNORE_ALREADY_PRESENT      707
#define MEMO_IGNORE_ADDED                708
#define MEMO_IGNORE_NOT_FOUND            709
#define MEMO_IGNORE_DELETED              710
#define MEMO_IGNORE_LIST_EMPTY           711
#define MEMO_IGNORE_LIST                 712
#define MEMO_IGNORE_LIST_X_EMPTY         713
#define MEMO_IGNORE_LIST_X               714
#define OPER_BOUNCY_MODES                715
#define OPER_BOUNCY_MODES_U_LINE         716
#define OPER_GLOBAL_SYNTAX               717
#define OPER_STATS_UNKNOWN_OPTION        718
#define OPER_STATS_RESET_USER_COUNT      719
#define OPER_STATS_CURRENT_USERS         720
#define OPER_STATS_MAX_USERS             721
#define OPER_STATS_UPTIME_DHM            722
#define OPER_STATS_UPTIME_HM_MS          723
#define OPER_STATS_KBYTES_READ           724
#define OPER_STATS_KBYTES_WRITTEN        725
#define OPER_STATS_NETBUF_SOCK           726
#define OPER_STATS_NETBUF_SOCK_PERCENT   727
#define OPER_STATS_NETBUF_TOTAL          728
#define OPER_STATS_NETBUF_TOTAL_PERCENT  729
#define OPER_STATS_ALL_USER_MEM          730
#define OPER_STATS_ALL_CHANNEL_MEM       731
#define OPER_STATS_ALL_SERVER_MEM        732
#define OPER_STATS_ALL_NICKGROUPINFO_MEM 733
#define OPER_STATS_ALL_NICKINFO_MEM      734
#define OPER_STATS_ALL_CHANSERV_MEM      735
#define OPER_STATS_ALL_STATSERV_MEM      736
#define OPER_STATS_ALL_NEWS_MEM          737
#define OPER_STATS_ALL_AKILL_MEM         738
#define OPER_STATS_ALL_EXCEPTION_MEM     739
#define OPER_STATS_ALL_SGLINE_MEM        740
#define OPER_STATS_ALL_SQLINE_MEM        741
#define OPER_STATS_ALL_SZLINE_MEM        742
#define OPER_STATS_ALL_SESSION_MEM       743
#define OPER_GETKEY_SYNTAX               744
#define OPER_GETKEY_KEY_IS               745
#define OPER_GETKEY_NO_KEY               746
#define OPER_MODE_SYNTAX                 747
#define OPER_CLEARMODES_SYNTAX           748
#define OPER_CLEARMODES_DONE             749
#define OPER_CLEARMODES_ALL_DONE         750
#define OPER_CLEARCHAN_SYNTAX            751
#define OPER_CLEARCHAN_DONE              752
#define OPER_KICK_SYNTAX                 753
#define OPER_ADMIN_SYNTAX                754
#define OPER_ADMIN_ADD_SYNTAX            755
#define OPER_ADMIN_DEL_SYNTAX            756
#define OPER_ADMIN_NO_NICKSERV           757
#define OPER_ADMIN_EXISTS                758
#define OPER_ADMIN_ADDED                 759
#define OPER_ADMIN_TOO_MANY              760
#define OPER_ADMIN_REMOVED               761
#define OPER_ADMIN_NOT_FOUND             762
#define OPER_ADMIN_LIST_HEADER           763
#define OPER_OPER_SYNTAX                 764
#define OPER_OPER_ADD_SYNTAX             765
#define OPER_OPER_DEL_SYNTAX             766
#define OPER_OPER_NO_NICKSERV            767
#define OPER_OPER_EXISTS                 768
#define OPER_OPER_ADDED                  769
#define OPER_OPER_TOO_MANY               770
#define OPER_OPER_REMOVED                771
#define OPER_OPER_NOT_FOUND              772
#define OPER_OPER_LIST_HEADER            773
#define OPER_MASKDATA_SYNTAX             774
#define OPER_MASKDATA_ADD_SYNTAX         775
#define OPER_MASKDATA_DEL_SYNTAX         776
#define OPER_MASKDATA_CLEAR_SYNTAX       777
#define OPER_MASKDATA_LIST_SYNTAX        778
#define OPER_MASKDATA_CHECK_SYNTAX       779
#define OPER_MASKDATA_LIST_FORMAT        780
#define OPER_MASKDATA_VIEW_FORMAT        781
#define OPER_MASKDATA_VIEW_UNUSED_FORMAT 782
#define OPER_TOO_MANY_AKILLS             783
#define OPER_AKILL_EXISTS                784
#define OPER_AKILL_NO_NICK               785
#define OPER_AKILL_MASK_TOO_GENERAL      786
#define OPER_AKILL_EXPIRY_LIMITED        787
#define OPER_AKILL_ADDED                 788
#define OPER_AKILL_REMOVED               789
#define OPER_AKILL_CLEARED               790
#define OPER_AKILL_NOT_FOUND             791
#define OPER_AKILL_LIST_HEADER           792
#define OPER_AKILL_LIST_EMPTY            793
#define OPER_AKILL_LIST_NO_MATCH         794
#define OPER_AKILL_CHECK_NO_MATCH        795
#define OPER_AKILL_CHECK_HEADER          796
#define OPER_AKILL_CHECK_TRAILER         797
#define OPER_AKILL_COUNT                 798
#define OPER_AKILLCHAN_SYNTAX            799
#define OPER_AKILLCHAN_AKILLED           800
#define OPER_AKILLCHAN_KILLED            801
#define OPER_AKILLCHAN_AKILLED_ONE       802
#define OPER_AKILLCHAN_KILLED_ONE        803
#define OPER_TOO_MANY_EXCLUDES           804
#define OPER_EXCLUDE_EXISTS              805
#define OPER_EXCLUDE_ADDED               806
#define OPER_EXCLUDE_REMOVED             807
#define OPER_EXCLUDE_CLEARED             808
#define OPER_EXCLUDE_NOT_FOUND           809
#define OPER_EXCLUDE_LIST_HEADER         810
#define OPER_EXCLUDE_LIST_EMPTY          811
#define OPER_EXCLUDE_LIST_NO_MATCH       812
#define OPER_EXCLUDE_CHECK_NO_MATCH      813
#define OPER_EXCLUDE_CHECK_HEADER        814
#define OPER_EXCLUDE_CHECK_TRAILER       815
#define OPER_EXCLUDE_COUNT               816
#define OPER_TOO_MANY_SLINES             817
#define OPER_SLINE_EXISTS                818
#define OPER_SLINE_MASK_TOO_GENERAL      819
#define OPER_SLINE_ADDED                 820
#define OPER_SLINE_REMOVED               821
#define OPER_SLINE_CLEARED               822
#define OPER_SLINE_NOT_FOUND             823
#define OPER_SLINE_LIST_HEADER           824
#define OPER_SLINE_LIST_EMPTY            825
#define OPER_SLINE_LIST_NO_MATCH         826
#define OPER_SLINE_CHECK_NO_MATCH        827
#define OPER_SLINE_CHECK_HEADER          828
#define OPER_SLINE_CHECK_TRAILER         829
#define OPER_SLINE_COUNT                 830
#define OPER_SZLINE_NOT_AVAIL            831
#define OPER_SU_SYNTAX                   832
#define OPER_SU_NO_PASSWORD              833
#define OPER_SU_SUCCEEDED                834
#define OPER_SU_FAILED                   835
#define OPER_SET_SYNTAX                  836
#define OPER_SET_IGNORE_ON               837
#define OPER_SET_IGNORE_OFF              838
#define OPER_SET_IGNORE_ERROR            839
#define OPER_SET_READONLY_ON             840
#define OPER_SET_READONLY_OFF            841
#define OPER_SET_READONLY_ERROR          842
#define OPER_SET_DEBUG_ON                843
#define OPER_SET_DEBUG_OFF               844
#define OPER_SET_DEBUG_LEVEL             845
#define OPER_SET_DEBUG_ERROR             846
#define OPER_SET_SUPASS_FAILED           847
#define OPER_SET_SUPASS_OK               848
#define OPER_SET_SUPASS_NONE             849
#define OPER_SET_UNKNOWN_OPTION          850
#define OPER_JUPE_SYNTAX                 851
#define OPER_JUPE_INVALID_NAME           852
#define OPER_JUPE_ALREADY_JUPED          853
#define OPER_RAW_SYNTAX                  854
#define OPER_UPDATE_SYNTAX               855
#define OPER_UPDATE_FORCE_FAILED         856
#define OPER_UPDATING                    857
#define OPER_UPDATE_COMPLETE             858
#define OPER_UPDATE_FAILED               859
#define OPER_REHASHING                   860
#define OPER_REHASHED                    861
#define OPER_REHASH_ERROR                862
#define OPER_KILLCLONES_SYNTAX           863
#define OPER_KILLCLONES_UNKNOWN_NICK     864
#define OPER_KILLCLONES_KILLED           865
#define OPER_KILLCLONES_KILLED_AKILL     866
#define OPER_EXCEPTION_SYNTAX            867
#define OPER_EXCEPTION_ADD_SYNTAX        868
#define OPER_EXCEPTION_DEL_SYNTAX        869
#define OPER_EXCEPTION_CLEAR_SYNTAX      870
#define OPER_EXCEPTION_MOVE_SYNTAX       871
#define OPER_EXCEPTION_LIST_SYNTAX       872
#define OPER_EXCEPTION_ALREADY_PRESENT   873
#define OPER_EXCEPTION_TOO_MANY          874
#define OPER_EXCEPTION_ADDED             875
#define OPER_EXCEPTION_MOVED             876
#define OPER_EXCEPTION_NO_SUCH_ENTRY     877
#define OPER_EXCEPTION_NOT_FOUND         878
#define OPER_EXCEPTION_NO_MATCH          879
#define OPER_EXCEPTION_EMPTY             880
#define OPER_EXCEPTION_DELETED           881
#define OPER_EXCEPTION_DELETED_ONE       882
#define OPER_EXCEPTION_DELETED_SEVERAL   883
#define OPER_EXCEPTION_CLEARED           884
#define OPER_EXCEPTION_LIST_HEADER       885
#define OPER_EXCEPTION_LIST_COLHEAD      886
#define OPER_EXCEPTION_LIST_FORMAT       887
#define OPER_EXCEPTION_VIEW_FORMAT       888
#define OPER_EXCEPTION_CHECK_SYNTAX      889
#define OPER_EXCEPTION_CHECK_NO_MATCH    890
#define OPER_EXCEPTION_CHECK_HEADER      891
#define OPER_EXCEPTION_CHECK_TRAILER     892
#define OPER_EXCEPTION_COUNT             893
#define OPER_EXCEPTION_INVALID_LIMIT     894
#define OPER_EXCEPTION_INVALID_HOSTMASK  895
#define OPER_SESSION_SYNTAX              896
#define OPER_SESSION_LIST_SYNTAX         897
#define OPER_SESSION_VIEW_SYNTAX         898
#define OPER_SESSION_INVALID_THRESHOLD   899
#define OPER_SESSION_NOT_FOUND           900
#define OPER_SESSION_LIST_HEADER         901
#define OPER_SESSION_LIST_COLHEAD        902
#define OPER_SESSION_LIST_FORMAT         903
#define OPER_SESSION_VIEW_FORMAT         904
#define NEWS_LOGON_TEXT                  905
#define NEWS_OPER_TEXT                   906
#define NEWS_LOGON_SYNTAX                907
#define NEWS_LOGON_LIST_HEADER           908
#define NEWS_LOGON_LIST_ENTRY            909
#define NEWS_LOGON_LIST_NONE             910
#define NEWS_LOGON_ADD_SYNTAX            911
#define NEWS_LOGON_ADD_FULL              912
#define NEWS_LOGON_ADDED                 913
#define NEWS_LOGON_DEL_SYNTAX            914
#define NEWS_LOGON_DEL_NOT_FOUND         915
#define NEWS_LOGON_DELETED               916
#define NEWS_LOGON_DEL_NONE              917
#define NEWS_LOGON_DELETED_ALL           918
#define NEWS_OPER_SYNTAX                 919
#define NEWS_OPER_LIST_HEADER            920
#define NEWS_OPER_LIST_ENTRY             921
#define NEWS_OPER_LIST_NONE              922
#define NEWS_OPER_ADD_SYNTAX             923
#define NEWS_OPER_ADD_FULL               924
#define NEWS_OPER_ADDED                  925
#define NEWS_OPER_DEL_SYNTAX             926
#define NEWS_OPER_DEL_NOT_FOUND          927
#define NEWS_OPER_DELETED                928
#define NEWS_OPER_DEL_NONE               929
#define NEWS_OPER_DELETED_ALL            930
#define NEWS_HELP_LOGON                  931
#define NEWS_HELP_OPER                   932
#define STAT_SERVERS_REMOVE_SERV_FIRST   933
#define STAT_SERVERS_SERVER_EXISTS       934
#define STAT_SERVERS_SYNTAX              935
#define STAT_SERVERS_STATS_TOTAL         936
#define STAT_SERVERS_STATS_ON_OFFLINE    937
#define STAT_SERVERS_LASTQUIT_WAS        938
#define STAT_SERVERS_LIST_HEADER         939
#define STAT_SERVERS_LIST_FORMAT         940
#define STAT_SERVERS_LIST_RESULTS        941
#define STAT_SERVERS_VIEW_HEADER_ONLINE  942
#define STAT_SERVERS_VIEW_HEADER_OFFLINE 943
#define STAT_SERVERS_VIEW_LASTJOIN       944
#define STAT_SERVERS_VIEW_LASTQUIT       945
#define STAT_SERVERS_VIEW_QUITMSG        946
#define STAT_SERVERS_VIEW_USERS_OPERS    947
#define STAT_SERVERS_VIEW_RESULTS        948
#define STAT_SERVERS_DELETE_SYNTAX       949
#define STAT_SERVERS_DELETE_DONE         950
#define STAT_SERVERS_COPY_SYNTAX         951
#define STAT_SERVERS_COPY_DONE           952
#define STAT_SERVERS_RENAME_SYNTAX       953
#define STAT_SERVERS_RENAME_DONE         954
#define STAT_USERS_SYNTAX                955
#define STAT_USERS_TOTUSERS              956
#define STAT_USERS_TOTOPERS              957
#define STAT_USERS_SERVUSERS             958
#define STAT_USERS_SERVOPERS             959
#define NICK_HELP                        960
#define NICK_HELP_EXPIRES                961
#define NICK_HELP_WARNING                962
#define NICK_HELP_COMMANDS               963
#define NICK_HELP_COMMANDS_AUTH          964
#define NICK_HELP_COMMANDS_LINK          965
#define NICK_HELP_COMMANDS_ACCESS        966
#define NICK_HELP_COMMANDS_AJOIN         967
#define NICK_HELP_COMMANDS_SET           968
#define NICK_HELP_COMMANDS_LIST          969
#define NICK_HELP_COMMANDS_LISTCHANS     970
#define NICK_HELP_REGISTER               971
#define NICK_HELP_REGISTER_EMAIL         972
#define NICK_HELP_REGISTER_EMAIL_REQ     973
#define NICK_HELP_REGISTER_EMAIL_AUTH    974
#define NICK_HELP_REGISTER_END           975
#define NICK_HELP_IDENTIFY               976
#define NICK_HELP_DROP                   977
#define NICK_HELP_DROP_LINK              978
#define NICK_HELP_DROP_END               979
#define NICK_HELP_AUTH                   980
#define NICK_HELP_SENDAUTH               981
#define NICK_HELP_REAUTH                 982
#define NICK_HELP_RESTOREMAIL            983
#define NICK_HELP_LINK                   984
#define NICK_HELP_UNLINK                 985
#define NICK_HELP_LISTLINKS              986
#define NICK_HELP_ACCESS                 987
#define NICK_HELP_SET                    988
#define NICK_HELP_SET_OPTION_MAINNICK    989
#define NICK_HELP_SET_END                990
#define NICK_HELP_SET_PASSWORD           991
#define NICK_HELP_SET_LANGUAGE           992
#define NICK_HELP_SET_URL                993
#define NICK_HELP_SET_EMAIL              994
#define NICK_HELP_SET_INFO               995
#define NICK_HELP_SET_KILL               996
#define NICK_HELP_SET_SECURE             997
#define NICK_HELP_SET_PRIVATE            998
#define NICK_HELP_SET_NOOP               999
#define NICK_HELP_SET_HIDE               1000
#define NICK_HELP_SET_TIMEZONE           1001
#define NICK_HELP_SET_MAINNICK           1002
#define NICK_HELP_UNSET                  1003
#define NICK_HELP_UNSET_REQ_EMAIL        1004
#define NICK_HELP_RECOVER                1005
#define NICK_HELP_RELEASE                1006
#define NICK_HELP_GHOST                  1007
#define NICK_HELP_INFO                   1008
#define NICK_HELP_INFO_AUTH              1009
#define NICK_HELP_LISTCHANS              1010
#define NICK_HELP_LIST                   1011
#define NICK_HELP_LIST_OPERSONLY         1012
#define NICK_HELP_LISTEMAIL              1013
#define NICK_HELP_STATUS                 1014
#define NICK_HELP_AJOIN                  1015
#define NICK_HELP_AJOIN_END              1016
#define NICK_HELP_AJOIN_END_CHANSERV     1017
#define NICK_OPER_HELP_COMMANDS          1018
#define NICK_OPER_HELP_COMMANDS_DROPEMAIL 1019
#define NICK_OPER_HELP_COMMANDS_GETPASS  1020
#define NICK_OPER_HELP_COMMANDS_FORBID   1021
#define NICK_OPER_HELP_COMMANDS_SETAUTH  1022
#define NICK_OPER_HELP_COMMANDS_END      1023
#define NICK_OPER_HELP_DROPNICK          1024
#define NICK_OPER_HELP_DROPEMAIL         1025
#define NICK_OPER_HELP_SET               1026
#define NICK_OPER_HELP_SET_NOEXPIRE      1027
#define NICK_OPER_HELP_UNSET             1028
#define NICK_OPER_HELP_OLD_UNLINK        1029
#define NICK_OPER_HELP_UNLINK            1030
#define NICK_OPER_HELP_OLD_LISTLINKS     1031
#define NICK_OPER_HELP_LISTLINKS         1032
#define NICK_OPER_HELP_ACCESS            1033
#define NICK_OPER_HELP_INFO              1034
#define NICK_OPER_HELP_LISTCHANS         1035
#define NICK_OPER_HELP_LIST              1036
#define NICK_OPER_HELP_LIST_AUTH         1037
#define NICK_OPER_HELP_LIST_END          1038
#define NICK_OPER_HELP_GETPASS           1039
#define NICK_OPER_HELP_FORBID            1040
#define NICK_OPER_HELP_SUSPEND           1041
#define NICK_OPER_HELP_UNSUSPEND         1042
#define NICK_OPER_HELP_AJOIN             1043
#define NICK_OPER_HELP_SETAUTH           1044
#define NICK_OPER_HELP_GETAUTH           1045
#define NICK_OPER_HELP_CLEARAUTH         1046
#define CHAN_HELP_REQSOP_LEVXOP          1047
#define CHAN_HELP_REQSOP_LEV             1048
#define CHAN_HELP_REQSOP_XOP             1049
#define CHAN_HELP_REQAOP_LEVXOP          1050
#define CHAN_HELP_REQAOP_LEV             1051
#define CHAN_HELP_REQAOP_XOP             1052
#define CHAN_HELP_REQHOP_LEVXOP          1053
#define CHAN_HELP_REQHOP_LEV             1054
#define CHAN_HELP_REQHOP_XOP             1055
#define CHAN_HELP_REQVOP_LEVXOP          1056
#define CHAN_HELP_REQVOP_LEV             1057
#define CHAN_HELP_REQVOP_XOP             1058
#define CHAN_HELP                        1059
#define CHAN_HELP_EXPIRES                1060
#define CHAN_HELP_COMMANDS               1061
#define CHAN_HELP_COMMANDS_LIST          1062
#define CHAN_HELP_COMMANDS_AKICK         1063
#define CHAN_HELP_COMMANDS_LEVELS        1064
#define CHAN_HELP_COMMANDS_XOP           1065
#define CHAN_HELP_COMMANDS_HOP           1066
#define CHAN_HELP_COMMANDS_XOP_2         1067
#define CHAN_HELP_COMMANDS_OPVOICE       1068
#define CHAN_HELP_COMMANDS_HALFOP        1069
#define CHAN_HELP_COMMANDS_PROTECT       1070
#define CHAN_HELP_COMMANDS_INVITE        1071
#define CHAN_HELP_REGISTER               1072
#define CHAN_HELP_REGISTER_ADMINONLY     1073
#define CHAN_HELP_IDENTIFY               1074
#define CHAN_HELP_DROP                   1075
#define CHAN_HELP_SET                    1076
#define CHAN_HELP_SET_FOUNDER            1077
#define CHAN_HELP_SET_SUCCESSOR          1078
#define CHAN_HELP_SET_PASSWORD           1079
#define CHAN_HELP_SET_DESC               1080
#define CHAN_HELP_SET_URL                1081
#define CHAN_HELP_SET_EMAIL              1082
#define CHAN_HELP_SET_ENTRYMSG           1083
#define CHAN_HELP_SET_KEEPTOPIC          1084
#define CHAN_HELP_SET_TOPICLOCK          1085
#define CHAN_HELP_SET_MLOCK              1086
#define CHAN_HELP_SET_HIDE               1087
#define CHAN_HELP_SET_PRIVATE            1088
#define CHAN_HELP_SET_RESTRICTED         1089
#define CHAN_HELP_SET_SECURE             1090
#define CHAN_HELP_SET_SECUREOPS          1091
#define CHAN_HELP_SET_LEAVEOPS           1092
#define CHAN_HELP_SET_OPNOTICE           1093
#define CHAN_HELP_SET_ENFORCE            1094
#define CHAN_HELP_SET_MEMO_RESTRICTED    1095
#define CHAN_HELP_UNSET                  1096
#define CHAN_HELP_SOP                    1097
#define CHAN_HELP_SOP_MID1               1098
#define CHAN_HELP_SOP_MID1_CHANPROT      1099
#define CHAN_HELP_SOP_MID2               1100
#define CHAN_HELP_SOP_MID2_HALFOP        1101
#define CHAN_HELP_SOP_END                1102
#define CHAN_HELP_AOP                    1103
#define CHAN_HELP_AOP_MID                1104
#define CHAN_HELP_AOP_MID_HALFOP         1105
#define CHAN_HELP_AOP_END                1106
#define CHAN_HELP_HOP                    1107
#define CHAN_HELP_VOP                    1108
#define CHAN_HELP_NOP                    1109
#define CHAN_HELP_ACCESS                 1110
#define CHAN_HELP_ACCESS_XOP             1111
#define CHAN_HELP_ACCESS_XOP_HALFOP      1112
#define CHAN_HELP_ACCESS_LEVELS          1113
#define CHAN_HELP_ACCESS_LEVELS_HALFOP   1114
#define CHAN_HELP_ACCESS_LEVELS_END      1115
#define CHAN_HELP_LEVELS                 1116
#define CHAN_HELP_LEVELS_XOP             1117
#define CHAN_HELP_LEVELS_XOP_HOP         1118
#define CHAN_HELP_LEVELS_END             1119
#define CHAN_HELP_LEVELS_DESC            1120
#define CHAN_HELP_AKICK                  1121
#define CHAN_HELP_INFO                   1122
#define CHAN_HELP_LIST                   1123
#define CHAN_HELP_LIST_OPERSONLY         1124
#define CHAN_HELP_OP                     1125
#define CHAN_HELP_DEOP                   1126
#define CHAN_HELP_VOICE                  1127
#define CHAN_HELP_DEVOICE                1128
#define CHAN_HELP_HALFOP                 1129
#define CHAN_HELP_DEHALFOP               1130
#define CHAN_HELP_PROTECT                1131
#define CHAN_HELP_DEPROTECT              1132
#define CHAN_HELP_INVITE                 1133
#define CHAN_HELP_UNBAN                  1134
#define CHAN_HELP_KICK                   1135
#define CHAN_HELP_KICK_PROTECTED         1136
#define CHAN_HELP_TOPIC                  1137
#define CHAN_HELP_CLEAR                  1138
#define CHAN_HELP_CLEAR_EXCEPTIONS       1139
#define CHAN_HELP_CLEAR_INVITES          1140
#define CHAN_HELP_CLEAR_MID              1141
#define CHAN_HELP_CLEAR_HALFOPS          1142
#define CHAN_HELP_CLEAR_END              1143
#define CHAN_HELP_STATUS                 1144
#define CHAN_OPER_HELP_COMMANDS          1145
#define CHAN_OPER_HELP_COMMANDS_GETPASS  1146
#define CHAN_OPER_HELP_COMMANDS_FORBID   1147
#define CHAN_OPER_HELP_COMMANDS_END      1148
#define CHAN_OPER_HELP_DROPCHAN          1149
#define CHAN_OPER_HELP_SET               1150
#define CHAN_OPER_HELP_SET_NOEXPIRE      1151
#define CHAN_OPER_HELP_UNSET             1152
#define CHAN_OPER_HELP_INFO              1153
#define CHAN_OPER_HELP_LIST              1154
#define CHAN_OPER_HELP_GETPASS           1155
#define CHAN_OPER_HELP_FORBID            1156
#define CHAN_OPER_HELP_SUSPEND           1157
#define CHAN_OPER_HELP_UNSUSPEND         1158
#define MEMO_HELP                        1159
#define MEMO_HELP_EXPIRES                1160
#define MEMO_HELP_END_LEVELS             1161
#define MEMO_HELP_END_XOP                1162
#define MEMO_HELP_COMMANDS               1163
#define MEMO_HELP_COMMANDS_FORWARD       1164
#define MEMO_HELP_COMMANDS_SAVE          1165
#define MEMO_HELP_COMMANDS_DEL           1166
#define MEMO_HELP_COMMANDS_IGNORE        1167
#define MEMO_HELP_SEND                   1168
#define MEMO_HELP_LIST                   1169
#define MEMO_HELP_LIST_EXPIRE            1170
#define MEMO_HELP_READ                   1171
#define MEMO_HELP_SAVE                   1172
#define MEMO_HELP_DEL                    1173
#define MEMO_HELP_RENUMBER               1174
#define MEMO_HELP_SET                    1175
#define MEMO_HELP_SET_OPTION_FORWARD     1176
#define MEMO_HELP_SET_END                1177
#define MEMO_HELP_SET_NOTIFY             1178
#define MEMO_HELP_SET_LIMIT              1179
#define MEMO_HELP_INFO                   1180
#define MEMO_OPER_HELP_COMMANDS          1181
#define MEMO_OPER_HELP_SET_LIMIT         1182
#define MEMO_OPER_HELP_INFO              1183
#define MEMO_HELP_FORWARD                1184
#define MEMO_HELP_SET_FORWARD            1185
#define MEMO_HELP_IGNORE                 1186
#define OPER_HELP                        1187
#define OPER_HELP_COMMANDS               1188
#define OPER_HELP_COMMANDS_SERVOPER      1189
#define OPER_HELP_COMMANDS_AKILL         1190
#define OPER_HELP_COMMANDS_EXCLUDE       1191
#define OPER_HELP_COMMANDS_SLINE         1192
#define OPER_HELP_COMMANDS_SESSION       1193
#define OPER_HELP_COMMANDS_NEWS          1194
#define OPER_HELP_COMMANDS_SERVADMIN     1195
#define OPER_HELP_COMMANDS_SERVROOT      1196
#define OPER_HELP_COMMANDS_RAW           1197
#define OPER_HELP_GLOBAL                 1198
#define OPER_HELP_STATS                  1199
#define OPER_HELP_SERVERMAP              1200
#define OPER_HELP_OPER                   1201
#define OPER_HELP_ADMIN                  1202
#define OPER_HELP_GETKEY                 1203
#define OPER_HELP_MODE                   1204
#define OPER_HELP_CLEARMODES             1205
#define OPER_HELP_CLEARCHAN              1206
#define OPER_HELP_KICK                   1207
#define OPER_HELP_AKILL                  1208
#define OPER_HELP_AKILL_OPERMAXEXPIRY    1209
#define OPER_HELP_AKILL_END              1210
#define OPER_HELP_AKILLCHAN              1211
#define OPER_HELP_EXCLUDE                1212
#define OPER_HELP_SGLINE                 1213
#define OPER_HELP_SQLINE                 1214
#define OPER_HELP_SQLINE_KILL            1215
#define OPER_HELP_SQLINE_NOKILL          1216
#define OPER_HELP_SQLINE_IGNOREOPERS     1217
#define OPER_HELP_SQLINE_END             1218
#define OPER_HELP_SZLINE                 1219
#define OPER_HELP_EXCEPTION              1220
#define OPER_HELP_SESSION                1221
#define OPER_HELP_SU                     1222
#define OPER_HELP_SET                    1223
#define OPER_HELP_SET_READONLY           1224
#define OPER_HELP_SET_DEBUG              1225
#define OPER_HELP_SET_SUPASS             1226
#define OPER_HELP_JUPE                   1227
#define OPER_HELP_RAW                    1228
#define OPER_HELP_UPDATE                 1229
#define OPER_HELP_QUIT                   1230
#define OPER_HELP_SHUTDOWN               1231
#define OPER_HELP_RESTART                1232
#define OPER_HELP_REHASH                 1233
#define OPER_HELP_KILLCLONES             1234
#define STAT_HELP                        1235
#define STAT_HELP_COMMANDS               1236
#define STAT_HELP_SERVERS                1237
#define STAT_HELP_USERS                  1238
#define STAT_OPER_HELP_SERVERS           1239

#define NUM_BASE_STRINGS 1240

#ifdef LANGSTR_ARRAY
static const char * const base_langstrs[] = {
    "LANG_NAME",
    "LANG_CHARSET",
    "STRFTIME_DATE_TIME_FORMAT",
    "STRFTIME_LONG_DATE_FORMAT",
    "STRFTIME_SHORT_DATE_FORMAT",
    "STRFTIME_DAYS_SHORT",
    "STRFTIME_DAYS_LONG",
    "STRFTIME_MONTHS_SHORT",
    "STRFTIME_MONTHS_LONG",
    "STR_DAY",
    "STR_DAYS",
    "STR_HOUR",
    "STR_HOURS",
    "STR_MINUTE",
    "STR_MINUTES",
    "STR_SECOND",
    "STR_SECONDS",
    "STR_TIMESEP",
    "COMMA_SPACE",
    "INTERNAL_ERROR",
    "SERVICES_IS_BUSY",
    "UNKNOWN_COMMAND",
    "UNKNOWN_COMMAND_HELP",
    "SYNTAX_ERROR",
    "MORE_INFO",
    "NO_HELP_AVAILABLE",
    "MISSING_QUOTE",
    "BAD_EMAIL",
    "REJECTED_EMAIL",
    "BAD_URL",
    "BAD_USERHOST_MASK",
    "BAD_NICKUSERHOST_MASK",
    "BAD_EXPIRY_TIME",
    "SENDMAIL_NO_RESOURCES",
    "READ_ONLY_MODE",
    "PASSWORD_INCORRECT",
    "PASSWORD_WARNING",
    "ACCESS_DENIED",
    "PERMISSION_DENIED",
    "MORE_OBSCURE_PASSWORD",
    "NICK_NOT_REGISTERED",
    "NICK_NOT_REGISTERED_HELP",
    "NICK_TOO_LONG",
    "NICK_INVALID",
    "NICK_X_NOT_REGISTERED",
    "NICK_X_ALREADY_REGISTERED",
    "NICK_X_NOT_IN_USE",
    "NICK_X_FORBIDDEN",
    "NICK_X_SUSPENDED",
    "NICK_X_SUSPENDED_MEMOS",
    "NICK_IDENTIFY_REQUIRED",
    "NICK_PLEASE_AUTH",
    "NICK_X_NOT_ON_CHAN_X",
    "CHAN_INVALID",
    "CHAN_X_NOT_REGISTERED",
    "CHAN_X_NOT_IN_USE",
    "CHAN_X_FORBIDDEN",
    "CHAN_X_SUSPENDED",
    "CHAN_X_SUSPENDED_MEMOS",
    "CHAN_IDENTIFY_REQUIRED",
    "SERV_X_NOT_FOUND",
    "EXPIRES_NONE",
    "EXPIRES_NOW",
    "EXPIRES_IN",
    "LIST_RESULTS",
    "NICK_IS_REGISTERED",
    "NICK_IS_SECURE",
    "NICK_MAY_NOT_BE_USED",
    "DISCONNECT_IN_1_MINUTE",
    "DISCONNECT_IN_20_SECONDS",
    "DISCONNECT_NOW",
    "FORCENICKCHANGE_IN_1_MINUTE",
    "FORCENICKCHANGE_IN_20_SECONDS",
    "FORCENICKCHANGE_NOW",
    "NICK_EXPIRES_SOON",
    "NICK_EXPIRED",
    "NICK_REGISTER_SYNTAX",
    "NICK_REGISTER_REQ_EMAIL_SYNTAX",
    "NICK_REGISTRATION_DISABLED",
    "NICK_REGISTRATION_FAILED",
    "NICK_REG_PLEASE_WAIT",
    "NICK_REG_PLEASE_WAIT_FIRST",
    "NICK_CANNOT_BE_REGISTERED",
    "NICK_REGISTER_EMAIL_UNAUTHED",
    "NICK_REGISTER_TOO_MANY_NICKS",
    "NICK_REGISTERED",
    "NICK_PASSWORD_IS",
    "NICK_IDENTIFY_SYNTAX",
    "NICK_IDENTIFY_FAILED",
    "NICK_IDENTIFY_EMAIL_MISSING",
    "NICK_IDENTIFY_SUCCEEDED",
    "NICK_DROP_SYNTAX",
    "NICK_DROP_WARNING",
    "NICK_DROP_DISABLED",
    "NICK_DROP_FAILED",
    "NICK_DROPPED",
    "NICK_DROPPED_LINKS",
    "NICK_DROPNICK_SYNTAX",
    "NICK_X_DROPPED",
    "NICK_X_DROPPED_LINKS",
    "NICK_DROPEMAIL_SYNTAX",
    "NICK_DROPEMAIL_PATTERN_TOO_LONG",
    "NICK_DROPEMAIL_NONE",
    "NICK_DROPEMAIL_COUNT",
    "NICK_DROPEMAIL_CONFIRM_SYNTAX",
    "NICK_DROPEMAIL_CONFIRM_UNKNOWN",
    "NICK_DROPEMAIL_CONFIRM_DROPPING",
    "NICK_DROPEMAIL_CONFIRM_DROPPED",
    "NICK_SET_SYNTAX",
    "NICK_SET_OPER_SYNTAX",
    "NICK_SET_DISABLED",
    "NICK_SET_UNKNOWN_OPTION",
    "NICK_SET_PASSWORD_FAILED",
    "NICK_SET_PASSWORD_CHANGED",
    "NICK_SET_PASSWORD_CHANGED_TO",
    "NICK_SET_LANGUAGE_SYNTAX",
    "NICK_SET_LANGUAGE_UNKNOWN",
    "NICK_SET_LANGUAGE_CHANGED",
    "NICK_SET_URL_CHANGED",
    "NICK_SET_EMAIL_PLEASE_WAIT",
    "NICK_SET_EMAIL_UNAUTHED",
    "NICK_SET_EMAIL_TOO_MANY_NICKS",
    "NICK_SET_EMAIL_CHANGED",
    "NICK_SET_INFO_CHANGED",
    "NICK_SET_KILL_SYNTAX",
    "NICK_SET_KILL_IMMED_SYNTAX",
    "NICK_SET_KILL_ON",
    "NICK_SET_KILL_QUICK",
    "NICK_SET_KILL_IMMED",
    "NICK_SET_KILL_IMMED_DISABLED",
    "NICK_SET_KILL_OFF",
    "NICK_SET_SECURE_SYNTAX",
    "NICK_SET_SECURE_ON",
    "NICK_SET_SECURE_OFF",
    "NICK_SET_PRIVATE_SYNTAX",
    "NICK_SET_PRIVATE_ON",
    "NICK_SET_PRIVATE_OFF",
    "NICK_SET_NOOP_SYNTAX",
    "NICK_SET_NOOP_ON",
    "NICK_SET_NOOP_OFF",
    "NICK_SET_HIDE_SYNTAX",
    "NICK_SET_HIDE_EMAIL_ON",
    "NICK_SET_HIDE_EMAIL_OFF",
    "NICK_SET_HIDE_MASK_ON",
    "NICK_SET_HIDE_MASK_OFF",
    "NICK_SET_HIDE_QUIT_ON",
    "NICK_SET_HIDE_QUIT_OFF",
    "NICK_SET_TIMEZONE_SYNTAX",
    "NICK_SET_TIMEZONE_TO",
    "NICK_SET_TIMEZONE_DEFAULT",
    "NICK_SET_MAINNICK_NOT_FOUND",
    "NICK_SET_MAINNICK_CHANGED",
    "NICK_SET_NOEXPIRE_SYNTAX",
    "NICK_SET_NOEXPIRE_ON",
    "NICK_SET_NOEXPIRE_OFF",
    "NICK_UNSET_SYNTAX",
    "NICK_UNSET_SYNTAX_REQ_EMAIL",
    "NICK_UNSET_OPER_SYNTAX",
    "NICK_UNSET_OPER_SYNTAX_REQ_EMAIL",
    "NICK_UNSET_URL",
    "NICK_UNSET_EMAIL",
    "NICK_UNSET_INFO",
    "NICK_UNSET_EMAIL_BAD",
    "NICK_UNSET_EMAIL_OTHER_BAD",
    "NICK_ACCESS_SYNTAX",
    "NICK_ACCESS_DISABLED",
    "NICK_ACCESS_ALREADY_PRESENT",
    "NICK_ACCESS_REACHED_LIMIT",
    "NICK_ACCESS_NO_NICKS",
    "NICK_ACCESS_ADDED",
    "NICK_ACCESS_NOT_FOUND",
    "NICK_ACCESS_DELETED",
    "NICK_ACCESS_LIST_EMPTY",
    "NICK_ACCESS_LIST",
    "NICK_ACCESS_LIST_X_EMPTY",
    "NICK_ACCESS_LIST_X",
    "NICK_OLD_LINK_SYNTAX",
    "NICK_LINK_SYNTAX",
    "NICK_LINK_DISABLED",
    "NICK_LINK_FAILED",
    "NICK_CANNOT_BE_LINKED",
    "NICK_OLD_LINK_SAME",
    "NICK_LINK_SAME",
    "NICK_LINK_ALREADY_LINKED",
    "NICK_LINK_IN_USE",
    "NICK_LINK_TOO_MANY",
    "NICK_LINK_TOO_MANY_NICKS",
    "NICK_OLD_LINK_TOO_MANY_CHANNELS",
    "NICK_OLD_LINKED",
    "NICK_LINKED",
    "NICK_OLD_UNLINK_SYNTAX",
    "NICK_UNLINK_SYNTAX",
    "NICK_UNLINK_OPER_SYNTAX",
    "NICK_UNLINK_DISABLED",
    "NICK_UNLINK_FAILED",
    "NICK_UNLINK_SAME",
    "NICK_OLD_UNLINK_NOT_LINKED",
    "NICK_UNLINK_NOT_LINKED_YOURS",
    "NICK_UNLINK_NOT_LINKED",
    "NICK_OLD_UNLINKED",
    "NICK_UNLINKED",
    "NICK_X_UNLINKED",
    "NICK_OLD_LISTLINKS_SYNTAX",
    "NICK_LISTLINKS_SYNTAX",
    "NICK_LISTLINKS_HEADER",
    "NICK_LISTLINKS_FOOTER",
    "NICK_INFO_SYNTAX",
    "NICK_INFO_REALNAME",
    "NICK_INFO_ADDRESS",
    "NICK_INFO_ADDRESS_ONLINE",
    "NICK_INFO_ADDRESS_ONLINE_NOHOST",
    "NICK_INFO_ADDRESS_OTHER_NICK",
    "NICK_INFO_TIME_REGGED",
    "NICK_INFO_LAST_SEEN",
    "NICK_INFO_LAST_QUIT",
    "NICK_INFO_URL",
    "NICK_INFO_EMAIL",
    "NICK_INFO_EMAIL_UNAUTHED",
    "NICK_INFO_INFO",
    "NICK_INFO_OPTIONS",
    "NICK_INFO_OPT_KILL",
    "NICK_INFO_OPT_SECURE",
    "NICK_INFO_OPT_PRIVATE",
    "NICK_INFO_OPT_NOOP",
    "NICK_INFO_OPT_NONE",
    "NICK_INFO_NO_EXPIRE",
    "NICK_INFO_SUSPEND_DETAILS",
    "NICK_INFO_SUSPEND_REASON",
    "NICK_INFO_SHOW_ALL",
    "NICK_LISTCHANS_SYNTAX",
    "NICK_LISTCHANS_NONE",
    "NICK_LISTCHANS_HEADER",
    "NICK_LISTCHANS_END",
    "NICK_LIST_SYNTAX",
    "NICK_LIST_OPER_SYNTAX",
    "NICK_LIST_OPER_SYNTAX_AUTH",
    "NICK_LIST_HEADER",
    "NICK_LIST_NO_MATCH",
    "NICK_LISTEMAIL_NONE",
    "NICK_RECOVER_SYNTAX",
    "NICK_NO_RECOVER_SELF",
    "NICK_RECOVERED",
    "NICK_RELEASE_SYNTAX",
    "NICK_RELEASE_NOT_HELD",
    "NICK_RELEASED",
    "NICK_GHOST_SYNTAX",
    "NICK_NO_GHOST_SELF",
    "NICK_GHOST_KILLED",
    "NICK_GETPASS_SYNTAX",
    "NICK_GETPASS_UNAVAILABLE",
    "NICK_GETPASS_PASSWORD_IS",
    "NICK_FORBID_SYNTAX",
    "NICK_FORBID_SUCCEEDED",
    "NICK_FORBID_FAILED",
    "NICK_SUSPEND_SYNTAX",
    "NICK_SUSPEND_SUCCEEDED",
    "NICK_SUSPEND_ALREADY_SUSPENDED",
    "NICK_UNSUSPEND_SYNTAX",
    "NICK_UNSUSPEND_SUCCEEDED",
    "NICK_UNSUSPEND_NOT_SUSPENDED",
    "NICK_AJOIN_SYNTAX",
    "NICK_AJOIN_ADD_SYNTAX",
    "NICK_AJOIN_DEL_SYNTAX",
    "NICK_AJOIN_DISABLED",
    "NICK_AJOIN_ALREADY_PRESENT",
    "NICK_AJOIN_LIST_FULL",
    "NICK_AJOIN_ADDED",
    "NICK_AJOIN_NOT_FOUND",
    "NICK_AJOIN_DELETED",
    "NICK_AJOIN_LIST_EMPTY",
    "NICK_AJOIN_LIST",
    "NICK_AJOIN_LIST_X_EMPTY",
    "NICK_AJOIN_LIST_X",
    "NICK_AJOIN_AUTO_REMOVE",
    "NICK_AUTH_SENDING",
    "NICK_AUTH_FOR_REGISTER",
    "NICK_AUTH_FOR_SET_EMAIL",
    "NICK_AUTH_NOT_NEEDED",
    "NICK_AUTH_NO_AUTHCODE",
    "NICK_AUTH_HAS_AUTHCODE",
    "PASSWORD_WARNING_FOR_AUTH",
    "NICK_AUTH_MAIL_SUBJECT",
    "NICK_AUTH_MAIL_BODY",
    "NICK_AUTH_MAIL_TEXT_REG",
    "NICK_AUTH_MAIL_TEXT_EMAIL",
    "NICK_AUTH_MAIL_TEXT_SENDAUTH",
    "NICK_AUTH_MAIL_TEXT_REAUTH",
    "NICK_AUTH_MAIL_BODY_SETAUTH",
    "NICK_AUTH_SYNTAX",
    "NICK_AUTH_DISABLED",
    "NICK_AUTH_FAILED",
    "NICK_AUTH_SUCCEEDED_REGISTER",
    "NICK_AUTH_SUCCEEDED_SET_EMAIL",
    "NICK_AUTH_SUCCEEDED_REAUTH",
    "NICK_AUTH_SUCCEEDED_SETAUTH",
    "NICK_SENDAUTH_SYNTAX",
    "NICK_SENDAUTH_TOO_SOON",
    "NICK_SENDAUTH_NORESOURCES",
    "NICK_SENDAUTH_ERROR",
    "NICK_REAUTH_SYNTAX",
    "NICK_REAUTH_HAVE_AUTHCODE",
    "NICK_REAUTH_NO_EMAIL",
    "NICK_REAUTH_AUTHCODE_SET",
    "NICK_RESTOREMAIL_SYNTAX",
    "NICK_RESTOREMAIL_NOT_NOW",
    "NICK_RESTOREMAIL_DONE",
    "NICK_SETAUTH_SYNTAX",
    "NICK_SETAUTH_NO_EMAIL",
    "NICK_SETAUTH_AUTHCODE_SET",
    "NICK_SETAUTH_SEND_REFUSED",
    "NICK_SETAUTH_SEND_TIMEOUT",
    "NICK_SETAUTH_SEND_NORESOURCES",
    "NICK_SETAUTH_SEND_ERROR",
    "NICK_SETAUTH_USER_NOTICE",
    "NICK_GETAUTH_SYNTAX",
    "NICK_GETAUTH_AUTHCODE_IS",
    "NICK_CLEARAUTH_SYNTAX",
    "NICK_CLEARAUTH_CLEARED",
    "CHAN_LEVEL_AUTOOP",
    "CHAN_LEVEL_AUTOVOICE",
    "CHAN_LEVEL_INVITE",
    "CHAN_LEVEL_AKICK",
    "CHAN_LEVEL_SET",
    "CHAN_LEVEL_CLEAR",
    "CHAN_LEVEL_UNBAN",
    "CHAN_LEVEL_OPDEOP",
    "CHAN_LEVEL_ACCESS_LIST",
    "CHAN_LEVEL_ACCESS_CHANGE",
    "CHAN_LEVEL_MEMO",
    "CHAN_LEVEL_VOICE",
    "CHAN_LEVEL_AUTOHALFOP",
    "CHAN_LEVEL_HALFOP",
    "CHAN_LEVEL_AUTOPROTECT",
    "CHAN_LEVEL_PROTECT",
    "CHAN_LEVEL_KICK",
    "CHAN_LEVEL_STATUS",
    "CHAN_LEVEL_TOPIC",
    "CHAN_IS_REGISTERED",
    "CHAN_MAY_NOT_BE_USED",
    "CHAN_NOT_ALLOWED_TO_JOIN",
    "CHAN_MUST_IDENTIFY_NICK",
    "CHAN_BOUNCY_MODES",
    "CHAN_REGISTER_SYNTAX",
    "CHAN_REGISTER_DISABLED",
    "CHAN_REGISTER_SHORT_CHANNEL",
    "CHAN_REGISTER_NOT_LOCAL",
    "CHAN_REGISTER_INVALID_NAME",
    "CHAN_MUST_REGISTER_NICK",
    "CHAN_MAY_NOT_BE_REGISTERED",
    "CHAN_ALREADY_REGISTERED",
    "CHAN_MUST_BE_CHANOP",
    "CHAN_REACHED_CHANNEL_LIMIT",
    "CHAN_EXCEEDED_CHANNEL_LIMIT",
    "CHAN_REGISTRATION_FAILED",
    "CHAN_REGISTERED",
    "CHAN_PASSWORD_IS",
    "CHAN_IDENTIFY_SYNTAX",
    "CHAN_IDENTIFY_FAILED",
    "CHAN_IDENTIFY_SUCCEEDED",
    "CHAN_DROP_SYNTAX",
    "CHAN_DROP_DISABLED",
    "CHAN_DROPPED",
    "CHAN_DROPCHAN_SYNTAX",
    "CHAN_SET_SYNTAX",
    "CHAN_SET_DISABLED",
    "CHAN_SET_UNKNOWN_OPTION",
    "CHAN_SET_FOUNDER_TOO_MANY_CHANS",
    "CHAN_FOUNDER_CHANGED",
    "CHAN_SUCCESSOR_CHANGED",
    "CHAN_SUCCESSOR_IS_FOUNDER",
    "CHAN_SET_PASSWORD_FAILED",
    "CHAN_PASSWORD_CHANGED",
    "CHAN_PASSWORD_CHANGED_TO",
    "CHAN_DESC_CHANGED",
    "CHAN_URL_CHANGED",
    "CHAN_EMAIL_CHANGED",
    "CHAN_ENTRY_MSG_CHANGED",
    "CHAN_SET_MLOCK_NEED_PLUS_MINUS",
    "CHAN_SET_MLOCK_NEED_PARAM",
    "CHAN_SET_MLOCK_NEED_POSITIVE",
    "CHAN_SET_MLOCK_MODE_REG_BAD",
    "CHAN_SET_MLOCK_LINK_BAD",
    "CHAN_SET_MLOCK_LINK_SAME",
    "CHAN_SET_MLOCK_REQUIRES",
    "CHAN_SET_MLOCK_BAD_PARAM",
    "CHAN_SET_MLOCK_UNKNOWN_CHAR",
    "CHAN_SET_MLOCK_CANNOT_LOCK",
    "CHAN_MLOCK_CHANGED",
    "CHAN_MLOCK_REMOVED",
    "CHAN_SET_HIDE_SYNTAX",
    "CHAN_SET_HIDE_EMAIL_ON",
    "CHAN_SET_HIDE_EMAIL_OFF",
    "CHAN_SET_HIDE_TOPIC_ON",
    "CHAN_SET_HIDE_TOPIC_OFF",
    "CHAN_SET_HIDE_MLOCK_ON",
    "CHAN_SET_HIDE_MLOCK_OFF",
    "CHAN_SET_KEEPTOPIC_SYNTAX",
    "CHAN_SET_KEEPTOPIC_ON",
    "CHAN_SET_KEEPTOPIC_OFF",
    "CHAN_SET_TOPICLOCK_SYNTAX",
    "CHAN_SET_TOPICLOCK_ON",
    "CHAN_SET_TOPICLOCK_OFF",
    "CHAN_SET_PRIVATE_SYNTAX",
    "CHAN_SET_PRIVATE_ON",
    "CHAN_SET_PRIVATE_OFF",
    "CHAN_SET_SECUREOPS_SYNTAX",
    "CHAN_SET_SECUREOPS_ON",
    "CHAN_SET_SECUREOPS_OFF",
    "CHAN_SET_LEAVEOPS_SYNTAX",
    "CHAN_SET_LEAVEOPS_ON",
    "CHAN_SET_LEAVEOPS_OFF",
    "CHAN_SET_RESTRICTED_SYNTAX",
    "CHAN_SET_RESTRICTED_ON",
    "CHAN_SET_RESTRICTED_OFF",
    "CHAN_SET_SECURE_SYNTAX",
    "CHAN_SET_SECURE_ON",
    "CHAN_SET_SECURE_OFF",
    "CHAN_SET_OPNOTICE_SYNTAX",
    "CHAN_SET_OPNOTICE_ON",
    "CHAN_SET_OPNOTICE_OFF",
    "CHAN_SET_ENFORCE_SYNTAX",
    "CHAN_SET_ENFORCE_ON",
    "CHAN_SET_ENFORCE_OFF",
    "CHAN_SET_MEMO_RESTRICTED_SYNTAX",
    "CHAN_SET_MEMO_RESTRICTED_ON",
    "CHAN_SET_MEMO_RESTRICTED_OFF",
    "CHAN_SET_NOEXPIRE_SYNTAX",
    "CHAN_SET_NOEXPIRE_ON",
    "CHAN_SET_NOEXPIRE_OFF",
    "CHAN_UNSET_SYNTAX",
    "CHAN_SUCCESSOR_UNSET",
    "CHAN_URL_UNSET",
    "CHAN_EMAIL_UNSET",
    "CHAN_ENTRY_MSG_UNSET",
    "CHAN_ACCESS_SYNTAX",
    "CHAN_ACCESS_ADD_SYNTAX",
    "CHAN_ACCESS_DEL_SYNTAX",
    "CHAN_ACCESS_LIST_SYNTAX",
    "CHAN_ACCESS_LISTLEVEL_SYNTAX",
    "CHAN_ACCESS_COUNT_SYNTAX",
    "CHAN_ACCESS_DISABLED",
    "CHAN_ACCESS_LEVEL_NONZERO",
    "CHAN_ACCESS_LEVEL_RANGE",
    "CHAN_ACCESS_NICKS_ONLY",
    "CHAN_ACCESS_NOOP",
    "CHAN_ACCESS_REACHED_LIMIT",
    "CHAN_ACCESS_LEVEL_UNCHANGED",
    "CHAN_ACCESS_LEVEL_CHANGED",
    "CHAN_ACCESS_ADDED",
    "CHAN_ACCESS_NOT_FOUND",
    "CHAN_ACCESS_DELETED",
    "CHAN_ACCESS_LIST_EMPTY",
    "CHAN_ACCESS_NO_MATCH",
    "CHAN_ACCESS_LIST_HEADER",
    "CHAN_ACCESS_LIST_FORMAT",
    "CHAN_ACCESS_COUNT",
    "CHAN_SOP_SYNTAX",
    "CHAN_AOP_SYNTAX",
    "CHAN_HOP_SYNTAX",
    "CHAN_VOP_SYNTAX",
    "CHAN_NOP_SYNTAX",
    "CHAN_SOP_LIST_SYNTAX",
    "CHAN_AOP_LIST_SYNTAX",
    "CHAN_HOP_LIST_SYNTAX",
    "CHAN_VOP_LIST_SYNTAX",
    "CHAN_NOP_LIST_SYNTAX",
    "CHAN_XOP_ADDED",
    "CHAN_XOP_LEVEL_CHANGED",
    "CHAN_XOP_LEVEL_UNCHANGED",
    "CHAN_XOP_NICKS_ONLY",
    "CHAN_XOP_NOOP",
    "CHAN_XOP_NICKS_ONLY_HOP",
    "CHAN_XOP_REACHED_LIMIT",
    "CHAN_XOP_REACHED_LIMIT_HOP",
    "CHAN_XOP_NOT_FOUND",
    "CHAN_XOP_DELETED",
    "CHAN_XOP_LIST_EMPTY",
    "CHAN_XOP_NO_MATCH",
    "CHAN_XOP_LIST_HEADER",
    "CHAN_XOP_COUNT",
    "CHAN_AKICK_SYNTAX",
    "CHAN_AKICK_LIST_SYNTAX",
    "CHAN_AKICK_VIEW_SYNTAX",
    "CHAN_AKICK_DISABLED",
    "CHAN_AKICK_ALREADY_EXISTS",
    "CHAN_AKICK_REACHED_LIMIT",
    "CHAN_AKICK_ADDED",
    "CHAN_AKICK_NOT_FOUND",
    "CHAN_AKICK_DELETED",
    "CHAN_AKICK_LIST_EMPTY",
    "CHAN_AKICK_NO_MATCH",
    "CHAN_AKICK_LIST_HEADER",
    "CHAN_AKICK_VIEW_FORMAT",
    "CHAN_AKICK_VIEW_UNUSED_FORMAT",
    "CHAN_AKICK_COUNT",
    "CHAN_AKICK_ENFORCE_DONE",
    "CHAN_LEVELS_SYNTAX",
    "CHAN_LEVELS_READONLY",
    "CHAN_LEVELS_RANGE",
    "CHAN_LEVELS_CHANGED",
    "CHAN_LEVELS_UNKNOWN",
    "CHAN_LEVELS_DISABLED",
    "CHAN_LEVELS_LIST_HEADER",
    "CHAN_LEVELS_LIST_DISABLED",
    "CHAN_LEVELS_LIST_FOUNDER",
    "CHAN_LEVELS_LIST_NORMAL",
    "CHAN_LEVELS_RESET",
    "CHAN_INFO_SYNTAX",
    "CHAN_INFO_HEADER",
    "CHAN_INFO_FOUNDER",
    "CHAN_INFO_SUCCESSOR",
    "CHAN_INFO_DESCRIPTION",
    "CHAN_INFO_ENTRYMSG",
    "CHAN_INFO_TIME_REGGED",
    "CHAN_INFO_LAST_USED",
    "CHAN_INFO_LAST_TOPIC",
    "CHAN_INFO_TOPIC_SET_BY",
    "CHAN_INFO_URL",
    "CHAN_INFO_EMAIL",
    "CHAN_INFO_OPTIONS",
    "CHAN_INFO_OPT_PRIVATE",
    "CHAN_INFO_OPT_KEEPTOPIC",
    "CHAN_INFO_OPT_TOPICLOCK",
    "CHAN_INFO_OPT_SECUREOPS",
    "CHAN_INFO_OPT_LEAVEOPS",
    "CHAN_INFO_OPT_RESTRICTED",
    "CHAN_INFO_OPT_SECURE",
    "CHAN_INFO_OPT_OPNOTICE",
    "CHAN_INFO_OPT_ENFORCE",
    "CHAN_INFO_OPT_MEMO_RESTRICTED",
    "CHAN_INFO_OPT_NONE",
    "CHAN_INFO_MODE_LOCK",
    "CHAN_INFO_NO_EXPIRE",
    "CHAN_INFO_SUSPEND_DETAILS",
    "CHAN_INFO_SUSPEND_REASON",
    "CHAN_INFO_SHOW_ALL",
    "CHAN_LIST_SYNTAX",
    "CHAN_LIST_OPER_SYNTAX",
    "CHAN_LIST_HEADER",
    "CHAN_LIST_NO_MATCH",
    "CHAN_INVITE_SYNTAX",
    "CHAN_INVITE_OK",
    "CHAN_OPVOICE_SYNTAX",
    "CHAN_OP_SUCCEEDED",
    "CHAN_OP_ALREADY",
    "CHAN_OP_FAILED",
    "CHAN_DEOP_SUCCEEDED",
    "CHAN_DEOP_ALREADY",
    "CHAN_DEOP_FAILED",
    "CHAN_VOICE_SUCCEEDED",
    "CHAN_VOICE_ALREADY",
    "CHAN_VOICE_FAILED",
    "CHAN_DEVOICE_SUCCEEDED",
    "CHAN_DEVOICE_ALREADY",
    "CHAN_DEVOICE_FAILED",
    "CHAN_HALFOP_SUCCEEDED",
    "CHAN_HALFOP_ALREADY",
    "CHAN_HALFOP_FAILED",
    "CHAN_DEHALFOP_SUCCEEDED",
    "CHAN_DEHALFOP_ALREADY",
    "CHAN_DEHALFOP_FAILED",
    "CHAN_PROTECT_SUCCEEDED",
    "CHAN_PROTECT_ALREADY",
    "CHAN_PROTECT_FAILED",
    "CHAN_DEPROTECT_SUCCEEDED",
    "CHAN_DEPROTECT_ALREADY",
    "CHAN_DEPROTECT_FAILED",
    "CHAN_UNBAN_SYNTAX",
    "CHAN_UNBANNED",
    "CHAN_KICK_SYNTAX",
    "CHAN_KICK_PROTECTED",
    "CHAN_KICKED",
    "CHAN_TOPIC_SYNTAX",
    "CHAN_CLEAR_SYNTAX",
    "CHAN_CLEARED_BANS",
    "CHAN_CLEARED_EXCEPTIONS",
    "CHAN_CLEARED_INVITES",
    "CHAN_CLEARED_MODES",
    "CHAN_CLEARED_OPS",
    "CHAN_CLEARED_HALFOPS",
    "CHAN_CLEARED_VOICES",
    "CHAN_CLEARED_USERS",
    "CHAN_GETPASS_SYNTAX",
    "CHAN_GETPASS_UNAVAILABLE",
    "CHAN_GETPASS_PASSWORD_IS",
    "CHAN_FORBID_SYNTAX",
    "CHAN_FORBID_SHORT_CHANNEL",
    "CHAN_FORBID_SUCCEEDED",
    "CHAN_FORBID_FAILED",
    "CHAN_SUSPEND_SYNTAX",
    "CHAN_SUSPEND_SUCCEEDED",
    "CHAN_SUSPEND_ALREADY_SUSPENDED",
    "CHAN_UNSUSPEND_SYNTAX",
    "CHAN_UNSUSPEND_SUCCEEDED",
    "CHAN_UNSUSPEND_NOT_SUSPENDED",
    "MEMO_HAVE_NEW_MEMO",
    "MEMO_HAVE_NEW_MEMOS",
    "MEMO_TYPE_READ_LAST",
    "MEMO_TYPE_READ_NUM",
    "MEMO_TYPE_LIST_NEW",
    "MEMO_AT_LIMIT",
    "MEMO_OVER_LIMIT",
    "MEMO_NEW_MEMO_ARRIVED",
    "MEMO_NEW_CHAN_MEMO_ARRIVED",
    "MEMO_HAVE_NO_MEMOS",
    "MEMO_HAVE_NO_NEW_MEMOS",
    "MEMO_DOES_NOT_EXIST",
    "MEMO_LIST_NOT_FOUND",
    "MEMO_SEND_SYNTAX",
    "MEMO_SEND_DISABLED",
    "MEMO_SEND_PLEASE_WAIT",
    "MEMO_SEND_CHAN_NOT_AVAIL",
    "MEMO_X_GETS_NO_MEMOS",
    "MEMO_X_HAS_TOO_MANY_MEMOS",
    "MEMO_SENT",
    "MEMO_SEND_FAILED",
    "MEMO_LIST_SYNTAX",
    "MEMO_LIST_MEMOS",
    "MEMO_LIST_NEW_MEMOS",
    "MEMO_LIST_HEADER",
    "MEMO_LIST_FORMAT",
    "MEMO_READ_SYNTAX",
    "MEMO_HEADER",
    "MEMO_CHAN_HEADER",
    "MEMO_SAVE_SYNTAX",
    "MEMO_SAVED_ONE",
    "MEMO_SAVED_SEVERAL",
    "MEMO_DEL_SYNTAX",
    "MEMO_DELETED_NONE",
    "MEMO_DELETED_ONE",
    "MEMO_DELETED_SEVERAL",
    "MEMO_DELETED_ALL",
    "MEMO_RENUMBER_ONLY_YOU",
    "MEMO_RENUMBER_DONE",
    "MEMO_SET_SYNTAX",
    "MEMO_SET_DISABLED",
    "MEMO_SET_UNKNOWN_OPTION",
    "MEMO_SET_NOTIFY_SYNTAX",
    "MEMO_SET_NOTIFY_ON",
    "MEMO_SET_NOTIFY_LOGON",
    "MEMO_SET_NOTIFY_NEW",
    "MEMO_SET_NOTIFY_OFF",
    "MEMO_SET_LIMIT_SYNTAX",
    "MEMO_SET_LIMIT_OPER_SYNTAX",
    "MEMO_SET_YOUR_LIMIT_FORBIDDEN",
    "MEMO_SET_LIMIT_FORBIDDEN",
    "MEMO_SET_YOUR_LIMIT_TOO_HIGH",
    "MEMO_SET_LIMIT_TOO_HIGH",
    "MEMO_SET_LIMIT_OVERFLOW",
    "MEMO_SET_YOUR_LIMIT",
    "MEMO_SET_YOUR_LIMIT_ZERO",
    "MEMO_SET_YOUR_LIMIT_DEFAULT",
    "MEMO_UNSET_YOUR_LIMIT",
    "MEMO_SET_LIMIT",
    "MEMO_SET_LIMIT_ZERO",
    "MEMO_SET_LIMIT_DEFAULT",
    "MEMO_UNSET_LIMIT",
    "MEMO_INFO_NO_MEMOS",
    "MEMO_INFO_MEMO",
    "MEMO_INFO_MEMO_UNREAD",
    "MEMO_INFO_MEMOS",
    "MEMO_INFO_MEMOS_ONE_UNREAD",
    "MEMO_INFO_MEMOS_SOME_UNREAD",
    "MEMO_INFO_MEMOS_ALL_UNREAD",
    "MEMO_INFO_LIMIT",
    "MEMO_INFO_HARD_LIMIT",
    "MEMO_INFO_LIMIT_ZERO",
    "MEMO_INFO_HARD_LIMIT_ZERO",
    "MEMO_INFO_NO_LIMIT",
    "MEMO_INFO_NOTIFY_OFF",
    "MEMO_INFO_NOTIFY_ON",
    "MEMO_INFO_NOTIFY_RECEIVE",
    "MEMO_INFO_NOTIFY_SIGNON",
    "MEMO_INFO_X_NO_MEMOS",
    "MEMO_INFO_X_MEMO",
    "MEMO_INFO_X_MEMO_UNREAD",
    "MEMO_INFO_X_MEMOS",
    "MEMO_INFO_X_MEMOS_ONE_UNREAD",
    "MEMO_INFO_X_MEMOS_SOME_UNREAD",
    "MEMO_INFO_X_MEMOS_ALL_UNREAD",
    "MEMO_INFO_X_LIMIT",
    "MEMO_INFO_X_HARD_LIMIT",
    "MEMO_INFO_X_NO_LIMIT",
    "MEMO_INFO_X_NOTIFY_OFF",
    "MEMO_INFO_X_NOTIFY_ON",
    "MEMO_INFO_X_NOTIFY_RECEIVE",
    "MEMO_INFO_X_NOTIFY_SIGNON",
    "MEMO_FORWARD_MAIL_SUBJECT",
    "MEMO_FORWARD_MULTIPLE_MAIL_SUBJECT",
    "MEMO_FORWARD_MAIL_BODY",
    "MEMO_FORWARD_CHANMEMO_MAIL_BODY",
    "MEMO_FORWARD_SYNTAX",
    "MEMO_FORWARD_NEED_EMAIL",
    "MEMO_FORWARD_PLEASE_WAIT",
    "MEMO_FORWARD_FAILED",
    "MEMO_FORWARDED_NONE",
    "MEMO_FORWARDED_ONE",
    "MEMO_FORWARDED_SEVERAL",
    "MEMO_FORWARDED_ALL",
    "MEMO_SET_FORWARD_SYNTAX",
    "MEMO_SET_FORWARD_ON",
    "MEMO_SET_FORWARD_COPY",
    "MEMO_SET_FORWARD_OFF",
    "MEMO_IGNORE_SYNTAX",
    "MEMO_IGNORE_ADD_SYNTAX",
    "MEMO_IGNORE_DEL_SYNTAX",
    "MEMO_IGNORE_LIST_FULL",
    "MEMO_IGNORE_ALREADY_PRESENT",
    "MEMO_IGNORE_ADDED",
    "MEMO_IGNORE_NOT_FOUND",
    "MEMO_IGNORE_DELETED",
    "MEMO_IGNORE_LIST_EMPTY",
    "MEMO_IGNORE_LIST",
    "MEMO_IGNORE_LIST_X_EMPTY",
    "MEMO_IGNORE_LIST_X",
    "OPER_BOUNCY_MODES",
    "OPER_BOUNCY_MODES_U_LINE",
    "OPER_GLOBAL_SYNTAX",
    "OPER_STATS_UNKNOWN_OPTION",
    "OPER_STATS_RESET_USER_COUNT",
    "OPER_STATS_CURRENT_USERS",
    "OPER_STATS_MAX_USERS",
    "OPER_STATS_UPTIME_DHM",
    "OPER_STATS_UPTIME_HM_MS",
    "OPER_STATS_KBYTES_READ",
    "OPER_STATS_KBYTES_WRITTEN",
    "OPER_STATS_NETBUF_SOCK",
    "OPER_STATS_NETBUF_SOCK_PERCENT",
    "OPER_STATS_NETBUF_TOTAL",
    "OPER_STATS_NETBUF_TOTAL_PERCENT",
    "OPER_STATS_ALL_USER_MEM",
    "OPER_STATS_ALL_CHANNEL_MEM",
    "OPER_STATS_ALL_SERVER_MEM",
    "OPER_STATS_ALL_NICKGROUPINFO_MEM",
    "OPER_STATS_ALL_NICKINFO_MEM",
    "OPER_STATS_ALL_CHANSERV_MEM",
    "OPER_STATS_ALL_STATSERV_MEM",
    "OPER_STATS_ALL_NEWS_MEM",
    "OPER_STATS_ALL_AKILL_MEM",
    "OPER_STATS_ALL_EXCEPTION_MEM",
    "OPER_STATS_ALL_SGLINE_MEM",
    "OPER_STATS_ALL_SQLINE_MEM",
    "OPER_STATS_ALL_SZLINE_MEM",
    "OPER_STATS_ALL_SESSION_MEM",
    "OPER_GETKEY_SYNTAX",
    "OPER_GETKEY_KEY_IS",
    "OPER_GETKEY_NO_KEY",
    "OPER_MODE_SYNTAX",
    "OPER_CLEARMODES_SYNTAX",
    "OPER_CLEARMODES_DONE",
    "OPER_CLEARMODES_ALL_DONE",
    "OPER_CLEARCHAN_SYNTAX",
    "OPER_CLEARCHAN_DONE",
    "OPER_KICK_SYNTAX",
    "OPER_ADMIN_SYNTAX",
    "OPER_ADMIN_ADD_SYNTAX",
    "OPER_ADMIN_DEL_SYNTAX",
    "OPER_ADMIN_NO_NICKSERV",
    "OPER_ADMIN_EXISTS",
    "OPER_ADMIN_ADDED",
    "OPER_ADMIN_TOO_MANY",
    "OPER_ADMIN_REMOVED",
    "OPER_ADMIN_NOT_FOUND",
    "OPER_ADMIN_LIST_HEADER",
    "OPER_OPER_SYNTAX",
    "OPER_OPER_ADD_SYNTAX",
    "OPER_OPER_DEL_SYNTAX",
    "OPER_OPER_NO_NICKSERV",
    "OPER_OPER_EXISTS",
    "OPER_OPER_ADDED",
    "OPER_OPER_TOO_MANY",
    "OPER_OPER_REMOVED",
    "OPER_OPER_NOT_FOUND",
    "OPER_OPER_LIST_HEADER",
    "OPER_MASKDATA_SYNTAX",
    "OPER_MASKDATA_ADD_SYNTAX",
    "OPER_MASKDATA_DEL_SYNTAX",
    "OPER_MASKDATA_CLEAR_SYNTAX",
    "OPER_MASKDATA_LIST_SYNTAX",
    "OPER_MASKDATA_CHECK_SYNTAX",
    "OPER_MASKDATA_LIST_FORMAT",
    "OPER_MASKDATA_VIEW_FORMAT",
    "OPER_MASKDATA_VIEW_UNUSED_FORMAT",
    "OPER_TOO_MANY_AKILLS",
    "OPER_AKILL_EXISTS",
    "OPER_AKILL_NO_NICK",
    "OPER_AKILL_MASK_TOO_GENERAL",
    "OPER_AKILL_EXPIRY_LIMITED",
    "OPER_AKILL_ADDED",
    "OPER_AKILL_REMOVED",
    "OPER_AKILL_CLEARED",
    "OPER_AKILL_NOT_FOUND",
    "OPER_AKILL_LIST_HEADER",
    "OPER_AKILL_LIST_EMPTY",
    "OPER_AKILL_LIST_NO_MATCH",
    "OPER_AKILL_CHECK_NO_MATCH",
    "OPER_AKILL_CHECK_HEADER",
    "OPER_AKILL_CHECK_TRAILER",
    "OPER_AKILL_COUNT",
    "OPER_AKILLCHAN_SYNTAX",
    "OPER_AKILLCHAN_AKILLED",
    "OPER_AKILLCHAN_KILLED",
    "OPER_AKILLCHAN_AKILLED_ONE",
    "OPER_AKILLCHAN_KILLED_ONE",
    "OPER_TOO_MANY_EXCLUDES",
    "OPER_EXCLUDE_EXISTS",
    "OPER_EXCLUDE_ADDED",
    "OPER_EXCLUDE_REMOVED",
    "OPER_EXCLUDE_CLEARED",
    "OPER_EXCLUDE_NOT_FOUND",
    "OPER_EXCLUDE_LIST_HEADER",
    "OPER_EXCLUDE_LIST_EMPTY",
    "OPER_EXCLUDE_LIST_NO_MATCH",
    "OPER_EXCLUDE_CHECK_NO_MATCH",
    "OPER_EXCLUDE_CHECK_HEADER",
    "OPER_EXCLUDE_CHECK_TRAILER",
    "OPER_EXCLUDE_COUNT",
    "OPER_TOO_MANY_SLINES",
    "OPER_SLINE_EXISTS",
    "OPER_SLINE_MASK_TOO_GENERAL",
    "OPER_SLINE_ADDED",
    "OPER_SLINE_REMOVED",
    "OPER_SLINE_CLEARED",
    "OPER_SLINE_NOT_FOUND",
    "OPER_SLINE_LIST_HEADER",
    "OPER_SLINE_LIST_EMPTY",
    "OPER_SLINE_LIST_NO_MATCH",
    "OPER_SLINE_CHECK_NO_MATCH",
    "OPER_SLINE_CHECK_HEADER",
    "OPER_SLINE_CHECK_TRAILER",
    "OPER_SLINE_COUNT",
    "OPER_SZLINE_NOT_AVAIL",
    "OPER_SU_SYNTAX",
    "OPER_SU_NO_PASSWORD",
    "OPER_SU_SUCCEEDED",
    "OPER_SU_FAILED",
    "OPER_SET_SYNTAX",
    "OPER_SET_IGNORE_ON",
    "OPER_SET_IGNORE_OFF",
    "OPER_SET_IGNORE_ERROR",
    "OPER_SET_READONLY_ON",
    "OPER_SET_READONLY_OFF",
    "OPER_SET_READONLY_ERROR",
    "OPER_SET_DEBUG_ON",
    "OPER_SET_DEBUG_OFF",
    "OPER_SET_DEBUG_LEVEL",
    "OPER_SET_DEBUG_ERROR",
    "OPER_SET_SUPASS_FAILED",
    "OPER_SET_SUPASS_OK",
    "OPER_SET_SUPASS_NONE",
    "OPER_SET_UNKNOWN_OPTION",
    "OPER_JUPE_SYNTAX",
    "OPER_JUPE_INVALID_NAME",
    "OPER_JUPE_ALREADY_JUPED",
    "OPER_RAW_SYNTAX",
    "OPER_UPDATE_SYNTAX",
    "OPER_UPDATE_FORCE_FAILED",
    "OPER_UPDATING",
    "OPER_UPDATE_COMPLETE",
    "OPER_UPDATE_FAILED",
    "OPER_REHASHING",
    "OPER_REHASHED",
    "OPER_REHASH_ERROR",
    "OPER_KILLCLONES_SYNTAX",
    "OPER_KILLCLONES_UNKNOWN_NICK",
    "OPER_KILLCLONES_KILLED",
    "OPER_KILLCLONES_KILLED_AKILL",
    "OPER_EXCEPTION_SYNTAX",
    "OPER_EXCEPTION_ADD_SYNTAX",
    "OPER_EXCEPTION_DEL_SYNTAX",
    "OPER_EXCEPTION_CLEAR_SYNTAX",
    "OPER_EXCEPTION_MOVE_SYNTAX",
    "OPER_EXCEPTION_LIST_SYNTAX",
    "OPER_EXCEPTION_ALREADY_PRESENT",
    "OPER_EXCEPTION_TOO_MANY",
    "OPER_EXCEPTION_ADDED",
    "OPER_EXCEPTION_MOVED",
    "OPER_EXCEPTION_NO_SUCH_ENTRY",
    "OPER_EXCEPTION_NOT_FOUND",
    "OPER_EXCEPTION_NO_MATCH",
    "OPER_EXCEPTION_EMPTY",
    "OPER_EXCEPTION_DELETED",
    "OPER_EXCEPTION_DELETED_ONE",
    "OPER_EXCEPTION_DELETED_SEVERAL",
    "OPER_EXCEPTION_CLEARED",
    "OPER_EXCEPTION_LIST_HEADER",
    "OPER_EXCEPTION_LIST_COLHEAD",
    "OPER_EXCEPTION_LIST_FORMAT",
    "OPER_EXCEPTION_VIEW_FORMAT",
    "OPER_EXCEPTION_CHECK_SYNTAX",
    "OPER_EXCEPTION_CHECK_NO_MATCH",
    "OPER_EXCEPTION_CHECK_HEADER",
    "OPER_EXCEPTION_CHECK_TRAILER",
    "OPER_EXCEPTION_COUNT",
    "OPER_EXCEPTION_INVALID_LIMIT",
    "OPER_EXCEPTION_INVALID_HOSTMASK",
    "OPER_SESSION_SYNTAX",
    "OPER_SESSION_LIST_SYNTAX",
    "OPER_SESSION_VIEW_SYNTAX",
    "OPER_SESSION_INVALID_THRESHOLD",
    "OPER_SESSION_NOT_FOUND",
    "OPER_SESSION_LIST_HEADER",
    "OPER_SESSION_LIST_COLHEAD",
    "OPER_SESSION_LIST_FORMAT",
    "OPER_SESSION_VIEW_FORMAT",
    "NEWS_LOGON_TEXT",
    "NEWS_OPER_TEXT",
    "NEWS_LOGON_SYNTAX",
    "NEWS_LOGON_LIST_HEADER",
    "NEWS_LOGON_LIST_ENTRY",
    "NEWS_LOGON_LIST_NONE",
    "NEWS_LOGON_ADD_SYNTAX",
    "NEWS_LOGON_ADD_FULL",
    "NEWS_LOGON_ADDED",
    "NEWS_LOGON_DEL_SYNTAX",
    "NEWS_LOGON_DEL_NOT_FOUND",
    "NEWS_LOGON_DELETED",
    "NEWS_LOGON_DEL_NONE",
    "NEWS_LOGON_DELETED_ALL",
    "NEWS_OPER_SYNTAX",
    "NEWS_OPER_LIST_HEADER",
    "NEWS_OPER_LIST_ENTRY",
    "NEWS_OPER_LIST_NONE",
    "NEWS_OPER_ADD_SYNTAX",
    "NEWS_OPER_ADD_FULL",
    "NEWS_OPER_ADDED",
    "NEWS_OPER_DEL_SYNTAX",
    "NEWS_OPER_DEL_NOT_FOUND",
    "NEWS_OPER_DELETED",
    "NEWS_OPER_DEL_NONE",
    "NEWS_OPER_DELETED_ALL",
    "NEWS_HELP_LOGON",
    "NEWS_HELP_OPER",
    "STAT_SERVERS_REMOVE_SERV_FIRST",
    "STAT_SERVERS_SERVER_EXISTS",
    "STAT_SERVERS_SYNTAX",
    "STAT_SERVERS_STATS_TOTAL",
    "STAT_SERVERS_STATS_ON_OFFLINE",
    "STAT_SERVERS_LASTQUIT_WAS",
    "STAT_SERVERS_LIST_HEADER",
    "STAT_SERVERS_LIST_FORMAT",
    "STAT_SERVERS_LIST_RESULTS",
    "STAT_SERVERS_VIEW_HEADER_ONLINE",
    "STAT_SERVERS_VIEW_HEADER_OFFLINE",
    "STAT_SERVERS_VIEW_LASTJOIN",
    "STAT_SERVERS_VIEW_LASTQUIT",
    "STAT_SERVERS_VIEW_QUITMSG",
    "STAT_SERVERS_VIEW_USERS_OPERS",
    "STAT_SERVERS_VIEW_RESULTS",
    "STAT_SERVERS_DELETE_SYNTAX",
    "STAT_SERVERS_DELETE_DONE",
    "STAT_SERVERS_COPY_SYNTAX",
    "STAT_SERVERS_COPY_DONE",
    "STAT_SERVERS_RENAME_SYNTAX",
    "STAT_SERVERS_RENAME_DONE",
    "STAT_USERS_SYNTAX",
    "STAT_USERS_TOTUSERS",
    "STAT_USERS_TOTOPERS",
    "STAT_USERS_SERVUSERS",
    "STAT_USERS_SERVOPERS",
    "NICK_HELP",
    "NICK_HELP_EXPIRES",
    "NICK_HELP_WARNING",
    "NICK_HELP_COMMANDS",
    "NICK_HELP_COMMANDS_AUTH",
    "NICK_HELP_COMMANDS_LINK",
    "NICK_HELP_COMMANDS_ACCESS",
    "NICK_HELP_COMMANDS_AJOIN",
    "NICK_HELP_COMMANDS_SET",
    "NICK_HELP_COMMANDS_LIST",
    "NICK_HELP_COMMANDS_LISTCHANS",
    "NICK_HELP_REGISTER",
    "NICK_HELP_REGISTER_EMAIL",
    "NICK_HELP_REGISTER_EMAIL_REQ",
    "NICK_HELP_REGISTER_EMAIL_AUTH",
    "NICK_HELP_REGISTER_END",
    "NICK_HELP_IDENTIFY",
    "NICK_HELP_DROP",
    "NICK_HELP_DROP_LINK",
    "NICK_HELP_DROP_END",
    "NICK_HELP_AUTH",
    "NICK_HELP_SENDAUTH",
    "NICK_HELP_REAUTH",
    "NICK_HELP_RESTOREMAIL",
    "NICK_HELP_LINK",
    "NICK_HELP_UNLINK",
    "NICK_HELP_LISTLINKS",
    "NICK_HELP_ACCESS",
    "NICK_HELP_SET",
    "NICK_HELP_SET_OPTION_MAINNICK",
    "NICK_HELP_SET_END",
    "NICK_HELP_SET_PASSWORD",
    "NICK_HELP_SET_LANGUAGE",
    "NICK_HELP_SET_URL",
    "NICK_HELP_SET_EMAIL",
    "NICK_HELP_SET_INFO",
    "NICK_HELP_SET_KILL",
    "NICK_HELP_SET_SECURE",
    "NICK_HELP_SET_PRIVATE",
    "NICK_HELP_SET_NOOP",
    "NICK_HELP_SET_HIDE",
    "NICK_HELP_SET_TIMEZONE",
    "NICK_HELP_SET_MAINNICK",
    "NICK_HELP_UNSET",
    "NICK_HELP_UNSET_REQ_EMAIL",
    "NICK_HELP_RECOVER",
    "NICK_HELP_RELEASE",
    "NICK_HELP_GHOST",
    "NICK_HELP_INFO",
    "NICK_HELP_INFO_AUTH",
    "NICK_HELP_LISTCHANS",
    "NICK_HELP_LIST",
    "NICK_HELP_LIST_OPERSONLY",
    "NICK_HELP_LISTEMAIL",
    "NICK_HELP_STATUS",
    "NICK_HELP_AJOIN",
    "NICK_HELP_AJOIN_END",
    "NICK_HELP_AJOIN_END_CHANSERV",
    "NICK_OPER_HELP_COMMANDS",
    "NICK_OPER_HELP_COMMANDS_DROPEMAIL",
    "NICK_OPER_HELP_COMMANDS_GETPASS",
    "NICK_OPER_HELP_COMMANDS_FORBID",
    "NICK_OPER_HELP_COMMANDS_SETAUTH",
    "NICK_OPER_HELP_COMMANDS_END",
    "NICK_OPER_HELP_DROPNICK",
    "NICK_OPER_HELP_DROPEMAIL",
    "NICK_OPER_HELP_SET",
    "NICK_OPER_HELP_SET_NOEXPIRE",
    "NICK_OPER_HELP_UNSET",
    "NICK_OPER_HELP_OLD_UNLINK",
    "NICK_OPER_HELP_UNLINK",
    "NICK_OPER_HELP_OLD_LISTLINKS",
    "NICK_OPER_HELP_LISTLINKS",
    "NICK_OPER_HELP_ACCESS",
    "NICK_OPER_HELP_INFO",
    "NICK_OPER_HELP_LISTCHANS",
    "NICK_OPER_HELP_LIST",
    "NICK_OPER_HELP_LIST_AUTH",
    "NICK_OPER_HELP_LIST_END",
    "NICK_OPER_HELP_GETPASS",
    "NICK_OPER_HELP_FORBID",
    "NICK_OPER_HELP_SUSPEND",
    "NICK_OPER_HELP_UNSUSPEND",
    "NICK_OPER_HELP_AJOIN",
    "NICK_OPER_HELP_SETAUTH",
    "NICK_OPER_HELP_GETAUTH",
    "NICK_OPER_HELP_CLEARAUTH",
    "CHAN_HELP_REQSOP_LEVXOP",
    "CHAN_HELP_REQSOP_LEV",
    "CHAN_HELP_REQSOP_XOP",
    "CHAN_HELP_REQAOP_LEVXOP",
    "CHAN_HELP_REQAOP_LEV",
    "CHAN_HELP_REQAOP_XOP",
    "CHAN_HELP_REQHOP_LEVXOP",
    "CHAN_HELP_REQHOP_LEV",
    "CHAN_HELP_REQHOP_XOP",
    "CHAN_HELP_REQVOP_LEVXOP",
    "CHAN_HELP_REQVOP_LEV",
    "CHAN_HELP_REQVOP_XOP",
    "CHAN_HELP",
    "CHAN_HELP_EXPIRES",
    "CHAN_HELP_COMMANDS",
    "CHAN_HELP_COMMANDS_LIST",
    "CHAN_HELP_COMMANDS_AKICK",
    "CHAN_HELP_COMMANDS_LEVELS",
    "CHAN_HELP_COMMANDS_XOP",
    "CHAN_HELP_COMMANDS_HOP",
    "CHAN_HELP_COMMANDS_XOP_2",
    "CHAN_HELP_COMMANDS_OPVOICE",
    "CHAN_HELP_COMMANDS_HALFOP",
    "CHAN_HELP_COMMANDS_PROTECT",
    "CHAN_HELP_COMMANDS_INVITE",
    "CHAN_HELP_REGISTER",
    "CHAN_HELP_REGISTER_ADMINONLY",
    "CHAN_HELP_IDENTIFY",
    "CHAN_HELP_DROP",
    "CHAN_HELP_SET",
    "CHAN_HELP_SET_FOUNDER",
    "CHAN_HELP_SET_SUCCESSOR",
    "CHAN_HELP_SET_PASSWORD",
    "CHAN_HELP_SET_DESC",
    "CHAN_HELP_SET_URL",
    "CHAN_HELP_SET_EMAIL",
    "CHAN_HELP_SET_ENTRYMSG",
    "CHAN_HELP_SET_KEEPTOPIC",
    "CHAN_HELP_SET_TOPICLOCK",
    "CHAN_HELP_SET_MLOCK",
    "CHAN_HELP_SET_HIDE",
    "CHAN_HELP_SET_PRIVATE",
    "CHAN_HELP_SET_RESTRICTED",
    "CHAN_HELP_SET_SECURE",
    "CHAN_HELP_SET_SECUREOPS",
    "CHAN_HELP_SET_LEAVEOPS",
    "CHAN_HELP_SET_OPNOTICE",
    "CHAN_HELP_SET_ENFORCE",
    "CHAN_HELP_SET_MEMO_RESTRICTED",
    "CHAN_HELP_UNSET",
    "CHAN_HELP_SOP",
    "CHAN_HELP_SOP_MID1",
    "CHAN_HELP_SOP_MID1_CHANPROT",
    "CHAN_HELP_SOP_MID2",
    "CHAN_HELP_SOP_MID2_HALFOP",
    "CHAN_HELP_SOP_END",
    "CHAN_HELP_AOP",
    "CHAN_HELP_AOP_MID",
    "CHAN_HELP_AOP_MID_HALFOP",
    "CHAN_HELP_AOP_END",
    "CHAN_HELP_HOP",
    "CHAN_HELP_VOP",
    "CHAN_HELP_NOP",
    "CHAN_HELP_ACCESS",
    "CHAN_HELP_ACCESS_XOP",
    "CHAN_HELP_ACCESS_XOP_HALFOP",
    "CHAN_HELP_ACCESS_LEVELS",
    "CHAN_HELP_ACCESS_LEVELS_HALFOP",
    "CHAN_HELP_ACCESS_LEVELS_END",
    "CHAN_HELP_LEVELS",
    "CHAN_HELP_LEVELS_XOP",
    "CHAN_HELP_LEVELS_XOP_HOP",
    "CHAN_HELP_LEVELS_END",
    "CHAN_HELP_LEVELS_DESC",
    "CHAN_HELP_AKICK",
    "CHAN_HELP_INFO",
    "CHAN_HELP_LIST",
    "CHAN_HELP_LIST_OPERSONLY",
    "CHAN_HELP_OP",
    "CHAN_HELP_DEOP",
    "CHAN_HELP_VOICE",
    "CHAN_HELP_DEVOICE",
    "CHAN_HELP_HALFOP",
    "CHAN_HELP_DEHALFOP",
    "CHAN_HELP_PROTECT",
    "CHAN_HELP_DEPROTECT",
    "CHAN_HELP_INVITE",
    "CHAN_HELP_UNBAN",
    "CHAN_HELP_KICK",
    "CHAN_HELP_KICK_PROTECTED",
    "CHAN_HELP_TOPIC",
    "CHAN_HELP_CLEAR",
    "CHAN_HELP_CLEAR_EXCEPTIONS",
    "CHAN_HELP_CLEAR_INVITES",
    "CHAN_HELP_CLEAR_MID",
    "CHAN_HELP_CLEAR_HALFOPS",
    "CHAN_HELP_CLEAR_END",
    "CHAN_HELP_STATUS",
    "CHAN_OPER_HELP_COMMANDS",
    "CHAN_OPER_HELP_COMMANDS_GETPASS",
    "CHAN_OPER_HELP_COMMANDS_FORBID",
    "CHAN_OPER_HELP_COMMANDS_END",
    "CHAN_OPER_HELP_DROPCHAN",
    "CHAN_OPER_HELP_SET",
    "CHAN_OPER_HELP_SET_NOEXPIRE",
    "CHAN_OPER_HELP_UNSET",
    "CHAN_OPER_HELP_INFO",
    "CHAN_OPER_HELP_LIST",
    "CHAN_OPER_HELP_GETPASS",
    "CHAN_OPER_HELP_FORBID",
    "CHAN_OPER_HELP_SUSPEND",
    "CHAN_OPER_HELP_UNSUSPEND",
    "MEMO_HELP",
    "MEMO_HELP_EXPIRES",
    "MEMO_HELP_END_LEVELS",
    "MEMO_HELP_END_XOP",
    "MEMO_HELP_COMMANDS",
    "MEMO_HELP_COMMANDS_FORWARD",
    "MEMO_HELP_COMMANDS_SAVE",
    "MEMO_HELP_COMMANDS_DEL",
    "MEMO_HELP_COMMANDS_IGNORE",
    "MEMO_HELP_SEND",
    "MEMO_HELP_LIST",
    "MEMO_HELP_LIST_EXPIRE",
    "MEMO_HELP_READ",
    "MEMO_HELP_SAVE",
    "MEMO_HELP_DEL",
    "MEMO_HELP_RENUMBER",
    "MEMO_HELP_SET",
    "MEMO_HELP_SET_OPTION_FORWARD",
    "MEMO_HELP_SET_END",
    "MEMO_HELP_SET_NOTIFY",
    "MEMO_HELP_SET_LIMIT",
    "MEMO_HELP_INFO",
    "MEMO_OPER_HELP_COMMANDS",
    "MEMO_OPER_HELP_SET_LIMIT",
    "MEMO_OPER_HELP_INFO",
    "MEMO_HELP_FORWARD",
    "MEMO_HELP_SET_FORWARD",
    "MEMO_HELP_IGNORE",
    "OPER_HELP",
    "OPER_HELP_COMMANDS",
    "OPER_HELP_COMMANDS_SERVOPER",
    "OPER_HELP_COMMANDS_AKILL",
    "OPER_HELP_COMMANDS_EXCLUDE",
    "OPER_HELP_COMMANDS_SLINE",
    "OPER_HELP_COMMANDS_SESSION",
    "OPER_HELP_COMMANDS_NEWS",
    "OPER_HELP_COMMANDS_SERVADMIN",
    "OPER_HELP_COMMANDS_SERVROOT",
    "OPER_HELP_COMMANDS_RAW",
    "OPER_HELP_GLOBAL",
    "OPER_HELP_STATS",
    "OPER_HELP_SERVERMAP",
    "OPER_HELP_OPER",
    "OPER_HELP_ADMIN",
    "OPER_HELP_GETKEY",
    "OPER_HELP_MODE",
    "OPER_HELP_CLEARMODES",
    "OPER_HELP_CLEARCHAN",
    "OPER_HELP_KICK",
    "OPER_HELP_AKILL",
    "OPER_HELP_AKILL_OPERMAXEXPIRY",
    "OPER_HELP_AKILL_END",
    "OPER_HELP_AKILLCHAN",
    "OPER_HELP_EXCLUDE",
    "OPER_HELP_SGLINE",
    "OPER_HELP_SQLINE",
    "OPER_HELP_SQLINE_KILL",
    "OPER_HELP_SQLINE_NOKILL",
    "OPER_HELP_SQLINE_IGNOREOPERS",
    "OPER_HELP_SQLINE_END",
    "OPER_HELP_SZLINE",
    "OPER_HELP_EXCEPTION",
    "OPER_HELP_SESSION",
    "OPER_HELP_SU",
    "OPER_HELP_SET",
    "OPER_HELP_SET_READONLY",
    "OPER_HELP_SET_DEBUG",
    "OPER_HELP_SET_SUPASS",
    "OPER_HELP_JUPE",
    "OPER_HELP_RAW",
    "OPER_HELP_UPDATE",
    "OPER_HELP_QUIT",
    "OPER_HELP_SHUTDOWN",
    "OPER_HELP_RESTART",
    "OPER_HELP_REHASH",
    "OPER_HELP_KILLCLONES",
    "STAT_HELP",
    "STAT_HELP_COMMANDS",
    "STAT_HELP_SERVERS",
    "STAT_HELP_USERS",
    "STAT_OPER_HELP_SERVERS",
};
#endif
