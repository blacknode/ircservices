-- ChanServ's data: registered channels, their access lists and their
-- autokick lists.
--
-- Every column is a field of the C structures (ChannelInfo, ChanAccess,
-- AutoKick), under the field's name: levels.* and mlock.* become levels_*
-- and mlock_*.  Services read and write whole records through the entity
-- store (include/store.h), which keeps a copy of each in Redis.  Change
-- these tables only while Services are stopped, or delete the record's key
-- from Redis afterwards.

CREATE TABLE channels (
  name_key          text     PRIMARY KEY,   -- the name, IRC-lowercased
  name              text     NOT NULL,
  founder           bigint   NOT NULL DEFAULT 0,  -- nickgroups.id
  successor         bigint   NOT NULL DEFAULT 0,
  founderpass       bytea,
  founderpass_cipher text,
  "desc"            text,
  url               text,
  email             text,
  entry_message     text,
  time_registered   bigint   NOT NULL DEFAULT 0,
  last_used         bigint   NOT NULL DEFAULT 0,
  last_topic        text,
  last_topic_setter text,
  last_topic_time   bigint   NOT NULL DEFAULT 0,
  flags             integer  NOT NULL DEFAULT 0,
  suspend_who       text,
  suspend_reason    text,
  suspend_time      bigint   NOT NULL DEFAULT 0,
  suspend_expires   bigint   NOT NULL DEFAULT 0,
  levels_invite         smallint NOT NULL DEFAULT -9999,
  levels_akick          smallint NOT NULL DEFAULT -9999,
  levels_set            smallint NOT NULL DEFAULT -9999,
  levels_unban          smallint NOT NULL DEFAULT -9999,
  levels_autoop         smallint NOT NULL DEFAULT -9999,
  levels_autodeop       smallint NOT NULL DEFAULT -9999,
  levels_autovoice      smallint NOT NULL DEFAULT -9999,
  levels_opdeop         smallint NOT NULL DEFAULT -9999,
  levels_access_list    smallint NOT NULL DEFAULT -9999,
  levels_clear          smallint NOT NULL DEFAULT -9999,
  levels_nojoin         smallint NOT NULL DEFAULT -9999,
  levels_access_change  smallint NOT NULL DEFAULT -9999,
  levels_memo           smallint NOT NULL DEFAULT -9999,
  levels_voice          smallint NOT NULL DEFAULT -9999,
  levels_autohalfop     smallint NOT NULL DEFAULT -9999,
  levels_halfop         smallint NOT NULL DEFAULT -9999,
  levels_autoprotect    smallint NOT NULL DEFAULT -9999,
  levels_protect        smallint NOT NULL DEFAULT -9999,
  levels_kick           smallint NOT NULL DEFAULT -9999,
  levels_status         smallint NOT NULL DEFAULT -9999,
  levels_topic          smallint NOT NULL DEFAULT -9999,
  mlock_on          integer  NOT NULL DEFAULT 0,
  mlock_off         integer  NOT NULL DEFAULT 0,
  mlock_limit       integer  NOT NULL DEFAULT 0,
  mlock_key         text,
  mlock_link        text,
  mlock_flood       text,
  mlock_joindelay   integer  NOT NULL DEFAULT 0,
  mlock_joinrate1   integer  NOT NULL DEFAULT 0,
  mlock_joinrate2   integer  NOT NULL DEFAULT 0
);
CREATE INDEX channels_founder ON channels (founder);
CREATE INDEX channels_successor ON channels (successor) WHERE successor <> 0;
CREATE INDEX channels_last_used ON channels (last_used);
CREATE INDEX channels_suspend_expires ON channels (suspend_expires)
  WHERE suspend_expires > 0;

CREATE TABLE channel_access (
  channel_key text     NOT NULL,
  idx         integer  NOT NULL,
  nickgroup   bigint   NOT NULL,
  level       smallint NOT NULL,
  PRIMARY KEY (channel_key, idx)
);
CREATE INDEX channel_access_nickgroup ON channel_access (nickgroup);

CREATE TABLE channel_akicks (
  channel_key text    NOT NULL,
  idx         integer NOT NULL,
  mask        text,
  reason      text,
  who         text,
  set         bigint  NOT NULL DEFAULT 0,
  lastused    bigint  NOT NULL DEFAULT 0,
  PRIMARY KEY (channel_key, idx)
);
