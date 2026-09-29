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

/* EVENT_SERVER_EOB_ACK: the uplink's burst is over, so join the
 * serverinfo channel (see service.h). */
static int do_eob_ack(void)
{
    service_join_channel(&devnull_service);
    return 0;
}

static int devnull_init(Module *module)
{
    if (!event_attach(module, EVENT_SERVER_EOB_ACK, do_eob_ack)) {
        module_log("Unable to attach to " EVENT_SERVER_EOB_ACK);
        return 0;
    }
    return 1;
}

/*************************************************************************/

ModuleInfo module_info = {
    .abi = MODULE_ABI,
    .description = "DevNull: a pseudo-client that ignores what it is sent",
    .services = MODULE_SERVICES(&devnull_service),
    .init = devnull_init,
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
