/*  =========================================================================
    wap_tutorial.c - Wallet access protocol tutorial

    Copyright (c) the Contributors as noted in the AUTHORS file.

    
    =========================================================================
*/

/*
@header
    This actor implements the Malamute service. The actor uses the CZMQ socket
    command interface, rather than a classic C API. You can start as many
    instances of the Malamute service as you like. Each will run in its own
    namespace, as virtual hosts. This class is wrapped as a main program via
    the malamute.c application, and can be wrapped in other languages in the
    same way as any C API.
@discuss
    This is a minimal, incomplete implementation of Malamute. It does however
    not have any known bugs.
@end
*/

//  This header file gives us the Wallet APIs plus CZMQ, and libzmq:
#include "../include/wallet.h"

int main (int argc, char *argv [])
{
    //  Let's start a new server instance; this runs as a background thread
    zactor_t *server = zactor_new (wap_server, NULL);

    //  Switch on verbose tracing... this gets a little overwhelming so you
    //  can comment or delete this when you're bored with it:
    zsock_send (server, "s", "VERBOSE");

    //  We control the server by sending it commands. It's a CZMQ actor, and
    //  we can talk to it using the zsock API (or zstr, or zframe, or zmsg).
    //  To get things started, let's tell the server to bind to an endpoint:
    zsock_send (server, "ss", "BIND", "ipc://@/monero");

    //  We can set server properties by sending it SET commands like this:
    //  (Or, configure the server via an external config file)
    zsock_send (server, "sss", "SET", "server/timeout", "5000");

    //  Switch on client tracing, we'll now see both the client and the
    //  server state machines animated:
    wap_client_verbose = true;

    //  The server is now running. Let's start a wallet client:
    wap_client_t *client = wap_client_new ();
    assert (client);
    int rc = wap_client_connect (client, "ipc://@/monero", 200, "wallet identity");
    assert (rc == 0);
    
    assert(wap_client_connected(client) == true);
    
    zchunk_t *tx_id = zchunk_new("12045", 5);
    rc = wap_client_output_indexes(client, &tx_id);
    
    assert(wap_client_status(client) == 0);
    zframe_t *frame = wap_client_o_indexes(client);
    assert(frame != 0);
    size_t size = zframe_size(frame) / sizeof(uint64_t);
    uint64_t *o_indexes_array = (uint64_t*)(zframe_data(frame));
    for (int i = 0; i < size; i++) {
    	printf("Output index: %d\n", (int)o_indexes_array[i]);
    }
    
    zchunk_t *address = zchunk_new("12045", 5);
    rc = wap_client_start(client, &address, 3);
    
    assert(wap_client_status(client) == 2);

    zlist_t *list = zlist_new();
    rc = wap_client_blocks(client, &list, 3);
    zmsg_t *msg = wap_client_block_data(client);
    assert(msg != 0);
    frame = zmsg_first(msg);
    char *data = (char*)zframe_data(frame);
    printf("%c %c\n", data[0], data[1]);
    
    frame = zframe_new("hello", 5);
    rc = wap_client_random_outs(client, 5, &frame);
    frame = wap_client_random_outputs(client);
    assert(frame != 0);
    data = (char*)zframe_data(frame);
    printf("%c %c\n", data[0], data[1]);

    zchunk_t *tx_as_hex = zchunk_new("12045", 5);
    wap_client_put (client, &tx_as_hex);
    
    rc = wap_client_get_height(client);
    assert(wap_client_height(client) == 12);
    
    rc = wap_client_save_bc(client);
    assert(wap_client_status(client) == 34);
    
    rc = wap_client_get_info(client);
    assert(wap_client_status(client) == 0);
    assert(wap_client_height(client) == 1);
    assert(wap_client_target_height(client) == 2);
    assert(wap_client_difficulty(client) == 3);
    assert(wap_client_tx_count(client) == 4);
    assert(wap_client_tx_pool_size(client) == 5);
    assert(wap_client_alt_blocks_count(client) == 6);
    assert(wap_client_outgoing_connections_count(client) == 7);
    assert(wap_client_incoming_connections_count(client) == 8);
    assert(wap_client_white_peerlist_size(client) == 9);
    assert(wap_client_grey_peerlist_size(client) == 10);
    
    rc = wap_client_get_peer_list(client);
    frame = wap_client_white_list(client);
    assert(frame != 0);
    data = (char*)zframe_data(frame);
    assert(data[0] == 'h');
    assert(data[1] == 'i');
    frame = wap_client_gray_list(client);
    assert(frame != 0);
    data = (char*)zframe_data(frame);
    assert(data[0] == 'h');
    assert(data[1] == 'i');
    
    rc = wap_client_get_mining_status(client);
    assert(wap_client_active(client) == 1);
    assert(wap_client_status(client) == 0);
    assert(wap_client_speed(client) == 100);
    assert(wap_client_thread_count(client) == 2);
    address = wap_client_address(client);
    assert(zchunk_size(address) == 5);
    char *x = "12045";
    assert(memcmp((char*)zchunk_data(address), x, 5) == 0);
    
    rc = wap_client_set_log_hash_rate(client, 1);
    assert(wap_client_status(client) == 2);
    
    rc = wap_client_set_log_level(client, -12);
    assert(wap_client_status(client) == 2);

    rc = wap_client_start_save_graph(client);
    assert(wap_client_status(client) == 2);
    
    rc = wap_client_stop_save_graph(client);
    assert(wap_client_status(client) == 2);
    
    rc = wap_client_get_block_hash(client, 3);
		zchunk_t *hash = wap_client_hash(client);
		assert(zchunk_size(hash) == 5);
 
 		address = zchunk_new("12045", 5);
    rc = wap_client_get_block_template(client, 5, &address);
    assert(memcmp((char*)zchunk_data(hash), x, 5) == 0);
    assert(wap_client_status(client) == 2);
    assert(wap_client_reserved_offset(client) == 3);
    assert(wap_client_height(client) == 4);
    assert(wap_client_difficulty(client) == 5);
    assert(memcmp((char*)zchunk_data(wap_client_address(client)), x, 5) == 0);
    assert(memcmp((char*)zchunk_data(wap_client_block_template_blob(client)), "12046", 5) == 0);
    
    //  Great, it all works. Now to shutdown, we use the destroy method,
    //  which does a proper deconnect handshake internally:
    wap_client_destroy (&client);

    //  Finally, shut down the server by destroying the actor; this does
    //  a proper shutdown so that all memory is freed as you'd expect.
    zactor_destroy (&server);
    return 0;
}
