/* DevNull module: a pseudo-client that swallows whatever it is sent.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#include "services.h"
#include "modules.h"

/*************************************************************************/

/* DevNullName = <nick>, <description>; in the module block.  Without an
 * on_message handler, what it is sent goes nowhere. */
static struct Service devnull_service = {
    .directive = "DevNullName",
};

/*************************************************************************/

ModuleInfo module_info = {
    .abi = MODULE_ABI,
    .description = "DevNull: a pseudo-client that ignores what it is sent",
    .services = MODULE_SERVICES(&devnull_service),
};

/*************************************************************************/

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
