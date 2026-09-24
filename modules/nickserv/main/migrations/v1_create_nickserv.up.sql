-- NickServ's data: nickname groups (the account: password, e-mail,
-- settings, access list, autojoin list, memos) and the nicknames that
-- belong to them.
--
-- Every column is a field of the C structures (NickGroupInfo, NickInfo,
-- Memo), under the field's name; Services read and write whole records
-- through the entity store (include/store.h), which keeps a copy of each
-- in Redis.  Change these tables only while Services are stopped, or
-- delete the record's key from Redis afterwards.

CREATE TABLE nickgroups (
  id              bigint   PRIMARY KEY,   -- random, never reused
  mainnick        text     NOT NULL,      -- the nick shown for the group
  pass            bytea,                  -- password, possibly encrypted
  pass_cipher     text,                   -- cipher it is encrypted with
  url             text,
  email           text,
  last_email      text,
  info            text,
  flags           integer  NOT NULL DEFAULT 0,
  os_priv         smallint NOT NULL DEFAULT 0,
  authcode        integer  NOT NULL DEFAULT 0,
  authset         bigint   NOT NULL DEFAULT 0,
  authreason      smallint NOT NULL DEFAULT 0,
  suspend_who     text,
  suspend_reason  text,
  suspend_time    bigint   NOT NULL DEFAULT 0,
  suspend_expires bigint   NOT NULL DEFAULT 0,
  language        smallint NOT NULL DEFAULT -1,
  timezone        smallint NOT NULL DEFAULT 0,
  channelmax      smallint NOT NULL DEFAULT 0,
  memos_memomax   smallint NOT NULL DEFAULT 0
);
CREATE INDEX nickgroups_email ON nickgroups (lower(email));
CREATE INDEX nickgroups_suspend_expires ON nickgroups (suspend_expires)
  WHERE suspend_expires > 0;
CREATE INDEX nickgroups_os_priv ON nickgroups (os_priv) WHERE os_priv > 0;
CREATE INDEX nickgroups_authset ON nickgroups (authset) WHERE authcode <> 0;

CREATE TABLE nicks (
  nick_key        text     PRIMARY KEY,   -- the nick, IRC-lowercased
  nick            text     NOT NULL,
  status          smallint NOT NULL DEFAULT 0,
  last_usermask   text,
  last_realmask   text,
  last_realname   text,
  last_quit       text,
  time_registered bigint   NOT NULL DEFAULT 0,
  last_seen       bigint   NOT NULL DEFAULT 0,
  nickgroup       bigint   NOT NULL DEFAULT 0,  -- 0 for a forbidden nick
  id_stamp        bigint   NOT NULL DEFAULT 0
);
CREATE INDEX nicks_nickgroup ON nicks (nickgroup);
CREATE INDEX nicks_last_seen ON nicks (last_seen);

CREATE TABLE nickgroup_access (
  nickgroup bigint  NOT NULL,
  idx       integer NOT NULL,
  mask      text    NOT NULL,
  PRIMARY KEY (nickgroup, idx)
);

CREATE TABLE nickgroup_ajoin (
  nickgroup bigint  NOT NULL,
  idx       integer NOT NULL,
  channel   text    NOT NULL,
  PRIMARY KEY (nickgroup, idx)
);

CREATE TABLE nickgroup_memos (
  nickgroup bigint   NOT NULL,
  idx       integer  NOT NULL,
  number    bigint   NOT NULL,
  flags     smallint NOT NULL DEFAULT 0,
  time      bigint   NOT NULL DEFAULT 0,
  firstread bigint   NOT NULL DEFAULT 0,
  sender    text,
  channel   text,
  text      text,
  PRIMARY KEY (nickgroup, idx)
);

CREATE TABLE nickgroup_memo_ignore (
  nickgroup bigint  NOT NULL,
  idx       integer NOT NULL,
  mask      text    NOT NULL,
  PRIMARY KEY (nickgroup, idx)
);
