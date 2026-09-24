-- Deliberately does nothing.
--
-- Every up needs a down -- the loader enforces it, and the core set is
-- checked by the same validator as any module's, on purpose.  But this
-- particular down would have to drop the migrations table, and dropping it
-- would erase the record of every migration every module has applied.
--
-- So it is inert, and OperServ refuses MIGRATION REVERT core besides.  The
-- table is created, extended, and never removed.

SELECT 1;
