/*  =========================================================================
    fty_asset_inventory - Inventory server: process inventory ASSET messages and update extended attributes

    Copyright (C) 2014 - 2015 Eaton

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
    =========================================================================
*/

/*
@header
    fty_asset_inventory - Inventory server: process inventory ASSET messages and update extended attributes
@discuss
@end
*/

#include "fty_asset_classes.h"

//  Structure of our class

void
fty_asset_inventory_server (zsock_t *pipe, void *args)
{
    char *name = strdup ((const char*)args);
    mlm_client_t *client = mlm_client_new ();
    zpoller_t *poller = zpoller_new (pipe, mlm_client_msgpipe (client), NULL);
    bool verbose = false;
    bool test = false;
    std::map<std::string,std::string> ext_map_cache;

    zsock_signal (pipe, 0);
    zsys_info ("%s:\tStarted", name);

    while (!zsys_interrupted)
    {
        void *which = zpoller_wait (poller, -1);
        if (!which)
            continue;
        else
        if (which == pipe) {
            zmsg_t *msg = zmsg_recv (pipe);
            char *cmd = zmsg_popstr (msg);
            if ( verbose ) {
                zsys_debug ("%s:\tActor command=%s", name, cmd);
            }

            if (streq (cmd, "$TERM")) {
                if ( !verbose ) // ! is here intentionally, to get rid of duplication information
                    zsys_info ("%s:\tGot $TERM", name);
                zstr_free (&cmd);
                zmsg_destroy (&msg);
                break;
            }
            else
            if (streq (cmd, "VERBOSE")) {
                verbose = true;
            }
            else
            if (streq (cmd, "CONNECT")) {
                char* endpoint = zmsg_popstr (msg);
                int rv = mlm_client_connect (client, endpoint, 1000, name);
                if (rv == -1) {
                    zsys_error ("%s:\tCan't connect to malamute endpoint '%s'", name, endpoint);
                }
                zstr_free (&endpoint);
                zsock_signal (pipe, 0);
            }
            else
            if (streq (cmd, "PRODUCER")) {
                char* stream = zmsg_popstr (msg);
                int rv = mlm_client_set_producer (client, stream);
                if (rv == -1) {
                    zsys_error ("%s:\tCan't set producer on stream '%s'", name, stream);
                }
                zstr_free (&stream);
                zsock_signal (pipe, 0);
            }
            else
            if (streq (cmd, "CONSUMER")) {
                char* stream = zmsg_popstr (msg);
                char* pattern = zmsg_popstr (msg);
                test = streq (stream, "ASSETS-TEST");
                int rv = mlm_client_set_consumer (client, stream, pattern);
                if (rv == -1) {
                    zsys_error ("%s:\tCan't set consumer on stream '%s', '%s'", name, stream, pattern);
                }
                zstr_free (&pattern);
                zstr_free (&stream);
                zsock_signal (pipe, 0);
            }
            else
            {
                zsys_info ("%s:\tUnhandled command %s", name, cmd);
            }
            zstr_free (&cmd);
            zmsg_destroy (&msg);
        }
        else
        if (which == mlm_client_msgpipe (client)) {
            zmsg_t *msg = mlm_client_recv (client);
            fty_proto_t *proto = fty_proto_decode (&msg);
            if (!proto) {
                zsys_warning ("%s:'tfty_proto_decode () failed", name);
                continue;
            }

            const char *device_name = fty_proto_name (proto);
            zhash_t *ext = fty_proto_ext (proto);
            const char *operation = fty_proto_operation(proto);

            if (streq (operation, "inventory")) {
                int rv = process_insert_inventory (device_name, ext, true, test,ext_map_cache);
                if (rv != 0)
                    zsys_error ("Could not insert inventory data into DB");
            }
            fty_proto_destroy (&proto);
        }
    }

    mlm_client_destroy (&client);
    zpoller_destroy (&poller);
    zstr_free (&name);
}

//  --------------------------------------------------------------------------
//  Self test of this class

void
fty_asset_inventory_test (bool verbose)
{
    printf (" * fty_asset_inventory: ");

    //  @selftest
    //  Test #1: Simple create/destroy test
    {
        zsys_debug ("fty-asset-server-test:Test #1");
        zactor_t *self = zactor_new (fty_asset_inventory_server, (void*) "asset-inventory-test");
        zclock_sleep (200);
        zactor_destroy (&self);
        zsys_info ("fty-asset-server-test:Test #1: OK");
    }
    static const char* endpoint = "inproc://fty_asset_inventory_test";

    zactor_t *server = zactor_new (mlm_server, (void*) "Malamute");
    assert ( server != NULL );
    zstr_sendx (server, "BIND", endpoint, NULL);
    if (verbose)
                zstr_send (server, "VERBOSE");

    mlm_client_t *ui = mlm_client_new ();
    mlm_client_connect (ui, endpoint, 5000, "fty-asset-inventory-ui");
    mlm_client_set_producer (ui, "ASSETS-TEST");

    zactor_t *inventory_server = zactor_new (fty_asset_inventory_server, (void*)"asset-inventory-test");
    if (verbose) {
        zstr_send (inventory_server, "VERBOSE");
    }
    zstr_sendx (inventory_server, "CONNECT", endpoint, NULL);
    zsock_wait (inventory_server);
    zstr_sendx (inventory_server, "CONSUMER", "ASSETS-TEST", "inventory@.*", NULL);
    zsock_wait (inventory_server);

    // Test #2: create inventory message and process it
    {
        zsys_debug ("fty-asset-server-test:Test #2");
        zmsg_t *msg = fty_proto_encode_asset (
                NULL,
                "MyDC",
                "inventory",
                NULL);
        int rv = mlm_client_send (ui, "inventory@dc-1", &msg);
        assert (rv == 0);
        zclock_sleep (200);
        zsys_info ("fty-asset-server-test:Test #2: OK");
    }

    zactor_destroy (&inventory_server);
    mlm_client_destroy (&ui);
    zactor_destroy (&server);
    //  @end
    printf ("OK\n");
}
