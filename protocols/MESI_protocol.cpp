#include "MESI_protocol.h"
#include "../sim/mreq.h"
#include "../sim/sim.h"
#include "../sim/hash_table.h"

extern Simulator *Sim;

/*************************
 * Constructor/Destructor.
 *************************/
MESI_protocol::MESI_protocol (Hash_table *my_table, Hash_entry *my_entry)
    : Protocol (my_table, my_entry)
{
    this->state = MESI_CACHE_I;
}

MESI_protocol::~MESI_protocol ()
{    
}

void MESI_protocol::dump (void)
{
    const char *block_states[8] = {"X","I","S","E","M", "IM", "IS", "SM"};
    if (state > (sizeof(block_states) / sizeof(char*)))
    {
        fprintf(stderr, "INVALID PRINT STATE\n");
    }
    else
        fprintf (stdout, "MESI_protocol - state: %s\n", block_states[state]);
}

void MESI_protocol::process_cache_request (Mreq *request)
{
    if (DEBUG) {
        switch (state) {
            case MESI_CACHE_I: printf("PI"); do_cache_I (request); break;
            case MESI_CACHE_S: printf("PS"); do_cache_S (request); break;
            case MESI_CACHE_E: printf("PE"); do_cache_E (request); break;
            case MESI_CACHE_M: printf("PM"); do_cache_M (request); break;
            case MESI_CACHE_IM: printf("PIM"); do_cache_IM (request); break;
            case MESI_CACHE_IS: printf("PIS"); do_cache_IS (request); break;
            case MESI_CACHE_SM: printf("PSM"); do_cache_SM (request); break;
        default:
            fatal_error ("Invalid Cache State for MESI Protocol\n");
        }
    }
    else {
        switch (state) {
            case MESI_CACHE_I: do_cache_I (request); break;
            case MESI_CACHE_S: do_cache_S (request); break;
            case MESI_CACHE_E: do_cache_E (request); break;
            case MESI_CACHE_M: do_cache_M (request); break;
            case MESI_CACHE_IM: do_cache_IM (request); break;
            case MESI_CACHE_IS: do_cache_IS (request); break;
            case MESI_CACHE_SM: do_cache_SM (request); break;
        default:
            fatal_error ("Invalid Cache State for MESI Protocol\n");
        }
    }
}

void MESI_protocol::process_snoop_request (Mreq *request)
{
    if (DEBUG) {
        switch (state) {
            case MESI_CACHE_I: printf("SI"); do_snoop_I (request); break;
            case MESI_CACHE_S: printf("SS"); do_snoop_S (request); break;
            case MESI_CACHE_E: printf("SE"); do_snoop_E (request); break;
            case MESI_CACHE_M: printf("SM"); do_snoop_M (request); break;
            case MESI_CACHE_IM: printf("SIM"); do_snoop_IM (request); break;
            case MESI_CACHE_IS: printf("SIS"); do_snoop_IS (request); break;
            case MESI_CACHE_SM: printf("SSM"); do_snoop_SM (request); break;
        default:
            fatal_error ("Invalid Cache State for MESI Protocol\n");
        }
    }
    else {
        switch (state) {
            case MESI_CACHE_I: do_snoop_I (request); break;
            case MESI_CACHE_S: do_snoop_S (request); break;
            case MESI_CACHE_E: do_snoop_E (request); break;
            case MESI_CACHE_M: do_snoop_M (request); break;
            case MESI_CACHE_IM: do_snoop_IM (request); break;
            case MESI_CACHE_IS: do_snoop_IS (request); break;
            case MESI_CACHE_SM: do_snoop_SM (request); break;
        default:
            fatal_error ("Invalid Cache State for MESI Protocol\n");
        }
    }
}

inline void MESI_protocol::do_cache_I (Mreq *request)
{
    switch(request->msg) {
        case LOAD:
            // Go to the intermediate state, it's a miss
            send_GETS(request->addr);
            state = MESI_CACHE_IS;
            Sim->cache_misses++;
            break;
        case STORE:
            // Go to the intermediate state, it's a miss
            send_GETM(request->addr);
            state = MESI_CACHE_IM;
            Sim->cache_misses++;
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: CacheI state shouldn't see this message\n");
    }
}

inline void MESI_protocol::do_cache_S (Mreq *request)
{
    switch(request->msg) {
        case LOAD:
            // already have it, send it
            send_DATA_to_proc(request->addr);
            break;
        case STORE:
            // move to the intermediate state, another miss
            send_GETM(request->addr);
            state = MESI_CACHE_SM;
            Sim->cache_misses++;
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: CacheS state shouldn't see this message\n");
    }
}

inline void MESI_protocol::do_cache_E (Mreq *request)
{
    switch(request->msg) {
        case LOAD:
            send_DATA_to_proc(request->addr);
            break;
        case STORE:
            send_DATA_to_proc(request->addr);
            state = MESI_CACHE_M; // silent upgrade
            Sim->silent_upgrades++;
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: CacheE state shouldn't see this message\n");
    }
}

inline void MESI_protocol::do_cache_M (Mreq *request)
{
    switch(request->msg) {
        case LOAD:
        case STORE:
            // already have it, send it to the processor
            send_DATA_to_proc(request->addr);
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: CacheM state shouldn't see this message\n");
    }
}

inline void MESI_protocol::do_cache_IM (Mreq *request) {
    switch(request->msg) {
        // Should only have one outstanding request
        case LOAD:
        case STORE:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error("CacheIM should only have one outstanding request per processor!\n");
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: CacheIM state shouldn't see this message\n");
    }
}

inline void MESI_protocol::do_cache_IS (Mreq *request) {
    switch(request->msg) {
        // Should only have one outstanding request
        case LOAD:
        case STORE:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error("CacheIS should only have one outstanding request per processor!\n");
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: CacheIS state shouldn't see this message\n");
    }
}

inline void MESI_protocol::do_cache_SM (Mreq *request) {
    switch(request->msg) {
        // Should only have one outstanding request
        case LOAD:
        case STORE:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error("CacheSM should only have one outstanding request per processor!\n");
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: CacheSM state shouldn't see this message\n");
    }
}


inline void MESI_protocol::do_snoop_I (Mreq *request)
{
    switch (request->msg) {
        // Do nothing
        case GETS:
        case GETM:
        case DATA:
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client SnoopI state shouldn't see this message\n");
    }
}

inline void MESI_protocol::do_snoop_S (Mreq *request)
{
    switch (request->msg) {
        case GETS:
            set_shared_line(true);
            break;
        case GETM:
            state = MESI_CACHE_I; // go to invalid, someone else wants to write
            break;
        case DATA:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("SnoopS should not see data for this line! I have the line!\n");
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client SnoopS state shouldn't see this message\n");
    }
}

inline void MESI_protocol::do_snoop_E (Mreq *request)
{
    switch (request->msg) {
        case GETS:
            send_DATA_on_bus(request->addr, request->src_mid);
            set_shared_line(true);
            state = MESI_CACHE_S;
            break;
        case GETM:
            send_DATA_on_bus(request->addr, request->src_mid);
            state = MESI_CACHE_I; // Go to the invalid state
            break;
        case DATA:
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client SnoopE state shouldn't see this message\n");
    }
}

inline void MESI_protocol::do_snoop_M (Mreq *request)
{
    switch (request->msg) {
        case GETS:
            // Send the data and go to S
            send_DATA_on_bus(request->addr, request->src_mid);
            set_shared_line(true);
            state = MESI_CACHE_S;
            break;
        case GETM:
            // Send the data and go to I
            send_DATA_on_bus(request->addr, request->src_mid);
            state = MESI_CACHE_I;
            break;
        case DATA:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error("SnoopM should not see data for this line! I have the line!\n");
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client SnoopM state shouldn't see this message\n");
    }
}

inline void MESI_protocol::do_snoop_IM (Mreq *request) {
    switch (request->msg) {
        case GETS:
        case GETM:
            break;
        case DATA:
            send_DATA_to_proc(request->addr);
            set_shared_line(false);
            state = MESI_CACHE_M; // Send the data and transition
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client SnoopIM state shouldn't see this message\n");
    }
}

inline void MESI_protocol::do_snoop_IS (Mreq *request) {
    switch (request->msg) {
        case GETS:
        case GETM:
            break;
        case DATA:
            send_DATA_to_proc(request->addr);
            // Check if there are other copies in other processors. If so
            // then we go to the shared state, otherwise go to exclusive
            if (get_shared_line())
                state = MESI_CACHE_S;
            else {
                state = MESI_CACHE_E;
                set_shared_line(false);
            }
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client SnoopIS state shouldn't see this message\n");
    }
}

inline void MESI_protocol::do_snoop_SM (Mreq *request) {
    switch (request->msg) {
        case GETS:
            set_shared_line(true); // Set line to shared
            break;
        case GETM:
            break;
        case DATA:
            send_DATA_to_proc(request->addr);
            state = MESI_CACHE_M;
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client SnoopSM state shouldn't see this message\n");
    }
}
