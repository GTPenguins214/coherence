#include "MOSI_protocol.h"
#include "../sim/mreq.h"
#include "../sim/sim.h"
#include "../sim/hash_table.h"

extern Simulator *Sim;

/*************************
 * Constructor/Destructor.
 *************************/
MOSI_protocol::MOSI_protocol (Hash_table *my_table, Hash_entry *my_entry)
    : Protocol (my_table, my_entry)
{
    this->state = MOSI_CACHE_I;
}

MOSI_protocol::~MOSI_protocol ()
{    
}

void MOSI_protocol::dump (void)
{
    const char *block_states[9] = {"X","I","S","O","M", "IM", "IS", "SM", "OM"};
    if (state > (sizeof(block_states) / sizeof(char*)))
    {
        fprintf(stderr, "INVALID PRINT STATE\n");
    }
    else
        fprintf (stdout, "MOSI_protocol - state: %s\n", block_states[state]);
}

void MOSI_protocol::process_cache_request (Mreq *request)
{
    if (DEBUG) {
        switch(state) {
            case MOSI_CACHE_I: printf("PI"); do_cache_I (request); break;
            case MOSI_CACHE_S: printf("PS"); do_cache_S (request); break;
            case MOSI_CACHE_O: printf("PO"); do_cache_O (request); break;
            case MOSI_CACHE_M: printf("PM"); do_cache_M (request); break;
            case MOSI_CACHE_IS: printf("PIS"); do_cache_IS (request); break;
            case MOSI_CACHE_IM: printf("PIM"); do_cache_IM (request); break;
            case MOSI_CACHE_SM: printf("PSM"); do_cache_SM (request); break;
            case MOSI_CACHE_OM: printf("POM"); do_cache_OM (request); break;
            default:
                fatal_error ("Invalid Cache State for MOSI Protocol\n");
        }
    }
    else {
        switch (state) {
            case MOSI_CACHE_I: do_cache_I (request); break;
            case MOSI_CACHE_S: do_cache_S (request); break;
            case MOSI_CACHE_O: do_cache_O (request); break;
            case MOSI_CACHE_M: do_cache_M (request); break;
            case MOSI_CACHE_IS: do_cache_IS (request); break;
            case MOSI_CACHE_IM: do_cache_IM (request); break;
            case MOSI_CACHE_SM: do_cache_SM (request); break;
            case MOSI_CACHE_OM: do_cache_OM (request); break;
            default:
                fatal_error("Invalid Cache State for MOSI Protocol\n");
        }
    }
}

void MOSI_protocol::process_snoop_request (Mreq *request)
{
    if (DEBUG) {
        switch (state) {
            case MOSI_CACHE_I: printf("SI"); do_snoop_I (request); break;
            case MOSI_CACHE_S: printf("SS"); do_snoop_S (request); break;
            case MOSI_CACHE_O: printf("SO"); do_snoop_O (request); break;
            case MOSI_CACHE_M: printf("SM"); do_snoop_M (request); break;
            case MOSI_CACHE_IS: printf("SIS"); do_snoop_IS (request); break;
            case MOSI_CACHE_IM: printf("SIM"); do_snoop_IM (request); break;
            case MOSI_CACHE_SM: printf("SSM"); do_snoop_SM (request); break;
            case MOSI_CACHE_OM: printf("SOM"); do_snoop_OM (request); break;
            default:
                fatal_error ("Invalid Cache State for MOSI Protocol\n");
        }
    }
    else {
        switch (state) {
            case MOSI_CACHE_I: do_snoop_I (request); break;
            case MOSI_CACHE_S: do_snoop_S (request); break;
            case MOSI_CACHE_O: do_snoop_O (request); break;
            case MOSI_CACHE_M: do_snoop_M (request); break;
            case MOSI_CACHE_IS: do_snoop_IS (request); break;
            case MOSI_CACHE_IM: do_snoop_IM (request); break;
            case MOSI_CACHE_SM: do_snoop_SM (request); break;
            case MOSI_CACHE_OM: do_snoop_OM (request); break;
            default:
                fatal_error ("Invalid Cache State for MOSI Protocol\n");
        }
    }
}

inline void MOSI_protocol::do_cache_I (Mreq *request)
{
    switch (request->msg) {
        case LOAD:
            send_GETS(request->addr);
            state = MOSI_CACHE_IS;
            Sim->cache_misses++;
            break;
        case STORE:
            send_GETM(request->addr);
            state = MOSI_CACHE_IM;
            Sim->cache_misses++;
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: CacheI state shouldn't see this message\n");
    }
}

inline void MOSI_protocol::do_cache_S (Mreq *request)
{
    switch (request->msg) {
        case LOAD:
            send_DATA_to_proc(request->addr); // already have it, send it to the processor
            break;
        case STORE:
            // Transition to M state, all other invalidate
            send_GETM(request->addr);
            state = MOSI_CACHE_SM; // Shared-Modified State
            Sim->cache_misses++;
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: CacheS state shouldn't see this message\n");
    }
}

inline void MOSI_protocol::do_cache_O (Mreq *request)
{
    switch (request->msg) {
        case LOAD:
            send_DATA_to_proc(request->addr); // already have it, send it to the processor
            break;
        case STORE:
            send_GETM(request->addr);
            state = MOSI_CACHE_OM;
            Sim->cache_misses++;
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: CacheO state shouldn't see this message\n");
    }
}

inline void MOSI_protocol::do_cache_M (Mreq *request)
{
    switch (request->msg) {
        // Load or Store: send to cpu
        case LOAD:
        case STORE:
            send_DATA_to_proc(request->addr);
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: CacheM state shouldn't see this message\n");
    }
}

inline void MOSI_protocol::do_cache_IS (Mreq *request) {
    switch (request->msg) {
        // Shouldn't be getting any request because we have a pending request
        case LOAD:
        case STORE:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error("CacheIS Should only have one outstanding request per processor!\n");
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: CacheIS state shouldn't see this message\n");
    }
}

inline void MOSI_protocol::do_cache_IM (Mreq *request) {
    switch (request->msg) {
        // Shouldn't be getting any requests because we have a pending request
        case LOAD:
        case STORE:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error("CacheIM Should only have one outstanding request per processor!\n");
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: CacheIM state shouldn't see this message\n");
    }
}

inline void MOSI_protocol::do_cache_SM (Mreq *request) {
    switch (request->msg) {
        // Shouldn't be getting any requests
        case LOAD:
        case STORE:
            request->print_msg(my_table->moduleID, "ERROR");
            fatal_error("CacheSM should only have one outstanding request per processor!\n");
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: CacheSM state shouldn't see this message\n");
    }
}

inline void MOSI_protocol::do_cache_OM (Mreq *request) {
    switch (request->msg) {
        case LOAD:
        case STORE:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error("CacheOM should only have one outstanding request per processor!\n");
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error("Client: CacheOM state shouldn't see this message\n");
    }
}

inline void MOSI_protocol::do_snoop_I (Mreq *request)
{
    switch (request->msg) {
        // Don't need to do anything
        case GETS:
        case GETM:
            break;
        case DATA:
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: SnoopI state shouldn't see this message\n");
    }
}

inline void MOSI_protocol::do_snoop_S (Mreq *request)
{
    switch (request->msg) {
        case GETS: // Don't need to do anything for GetS
            break;
        case GETM:
            state = MOSI_CACHE_I;
            break;
        case DATA:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("SnoopS should not see data for this line!  I have the line!\n");
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: SnoopS state shouldn't see this message\n");
    }
}

inline void MOSI_protocol::do_snoop_O (Mreq *request)
{
    switch (request->msg) {
        case GETS: // Don't do anything for GetS
            send_DATA_on_bus(request->addr, request->src_mid);
            break;
        case GETM:
            send_DATA_on_bus(request->addr, request->src_mid);
            state = MOSI_CACHE_I;
            break;
        case DATA:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error("SnoopO should not see data for this line! I have the line!\n");
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: SnoopO state shouldn't see this message\n");
    }
}

inline void MOSI_protocol::do_snoop_M (Mreq *request)
{
    switch (request->msg) {
        case GETS:
            //printf("Got Here 1\n");
            send_DATA_on_bus(request->addr, request->src_mid);
            state = MOSI_CACHE_O;
            break;
        case GETM:
            send_DATA_on_bus(request->addr, request->src_mid);
            state = MOSI_CACHE_I;
            break;
        case DATA:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error("SnoopM should not see data for this line! I have the line!\n");
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: SnoopM state shouldn't see this message\n");
    }
}

inline void MOSI_protocol::do_snoop_IS (Mreq *request) {
    switch (request->msg) {
        case GETS:
        case GETM:
            break;
        case DATA:
            send_DATA_to_proc(request->addr);
            state = MOSI_CACHE_S;
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: SnoopIS state shouldn't see this message\n");
    }
}

inline void MOSI_protocol::do_snoop_IM (Mreq *request) {
    switch (request->msg) {
        case GETS:
        case GETM:
            break;
        case DATA:
            //printf("Got Here\n");
            send_DATA_to_proc(request->addr);
            state = MOSI_CACHE_M;
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: SnoopIM state shouldn't see this message\n");
    }
}

inline void MOSI_protocol::do_snoop_SM (Mreq *request) {
    switch (request->msg) {
        case GETS:
        case GETM:
            break;
        case DATA:
            //printf("Got Here 2\n");
            send_DATA_to_proc(request->addr);
            state = MOSI_CACHE_M;
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: SnoopSM state shouldn't see this message\n");
    }
}

inline void MOSI_protocol::do_snoop_OM (Mreq *request) {
    switch (request->msg) {
        case GETS:
            break;
        case GETM:
            send_DATA_on_bus(request->addr, request->src_mid);
            break;
        case DATA:
            //printf("Got Here 3\n");
            send_DATA_to_proc(request->addr);
            state = MOSI_CACHE_M;
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: SnoopOM state shouldn't see this message\n");
    }
}