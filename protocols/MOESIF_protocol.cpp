#include "MOESIF_protocol.h"
#include "../sim/mreq.h"
#include "../sim/sim.h"
#include "../sim/hash_table.h"

extern Simulator *Sim;

/*************************
 * Constructor/Destructor.
 *************************/
MOESIF_protocol::MOESIF_protocol (Hash_table *my_table, Hash_entry *my_entry)
    : Protocol (my_table, my_entry)
{
    this->state = MOESIF_CACHE_I;
}

MOESIF_protocol::~MOESIF_protocol ()
{    
}

void MOESIF_protocol::dump (void)
{
    const char *block_states[12] = {"X","I","S","E","O","M","F", "IM", "IS", "SM", "OM", "OF"};
    if (state > (sizeof(block_states) / sizeof(char*)))
    {
        fprintf(stderr, "INVALID PRINT STATE\n");
    }
    else
        fprintf (stdout, "MOESIF_protocol - state: %s\n", block_states[state]);
}

void MOESIF_protocol::process_cache_request (Mreq *request)
{
if (DEBUG) {
        switch (state) {
            case MOESIF_CACHE_I : printf("PI"); do_cache_I (request); break;
            case MOESIF_CACHE_S : printf("PS"); do_cache_S (request); break;
            case MOESIF_CACHE_E : printf("PE"); do_cache_E (request); break;
            case MOESIF_CACHE_O : printf("PO"); do_cache_O (request); break;
            case MOESIF_CACHE_M : printf("PM"); do_cache_M (request); break;
            case MOESIF_CACHE_F : printf("PF"); do_cache_F (request); break;
            case MOESIF_CACHE_IM : printf("PIM"); do_cache_IM (request); break;
            case MOESIF_CACHE_IS : printf("PIS"); do_cache_IS (request); break;
            case MOESIF_CACHE_SM : printf("PSM"); do_cache_SM (request); break;
            case MOESIF_CACHE_OM : printf("POM"); do_cache_OM (request); break;
            case MOESIF_CACHE_FM : printf("PFM"); do_cache_FM (request); break;
            default:
                fatal_error("Invalid Cache state for MOESIF Protocol\n");
        }
    }
    else {
        switch (state) {
            case MOESIF_CACHE_I : do_cache_I (request); break;
            case MOESIF_CACHE_S : do_cache_S (request); break;
            case MOESIF_CACHE_E : do_cache_E (request); break;
            case MOESIF_CACHE_O : do_cache_O (request); break;
            case MOESIF_CACHE_M : do_cache_M (request); break;
            case MOESIF_CACHE_F : do_cache_F (request); break;
            case MOESIF_CACHE_IM : do_cache_IM (request); break;
            case MOESIF_CACHE_IS : do_cache_IS (request); break;
            case MOESIF_CACHE_SM : do_cache_SM (request); break;
            case MOESIF_CACHE_OM : do_cache_OM (request); break;
            case MOESIF_CACHE_FM : do_cache_FM (request); break;
            default:
                fatal_error("Invalid Cache state for MOESIF Protocol\n");
        }
    }
}

void MOESIF_protocol::process_snoop_request (Mreq *request)
{
    if (DEBUG) {
        switch (state) {
            case MOESIF_CACHE_I : printf("SI"); do_snoop_I (request); break;
            case MOESIF_CACHE_S : printf("SS"); do_snoop_S (request); break;
            case MOESIF_CACHE_E : printf("SE"); do_snoop_E (request); break;
            case MOESIF_CACHE_O : printf("SO"); do_snoop_O (request); break;
            case MOESIF_CACHE_M : printf("SM"); do_snoop_M (request); break;
            case MOESIF_CACHE_F : printf("SF"); do_snoop_F (request); break;
            case MOESIF_CACHE_IM : printf("SIM"); do_snoop_IM (request); break;
            case MOESIF_CACHE_IS : printf("SIS"); do_snoop_IS (request); break;
            case MOESIF_CACHE_SM : printf("SSM"); do_snoop_SM (request); break;
            case MOESIF_CACHE_OM : printf("SOM"); do_snoop_OM (request); break;
            case MOESIF_CACHE_FM : printf("SFM"); do_snoop_FM (request); break;
            default:
                fatal_error("Invalid Cache State for MOESIF Protocol\n");
        }
    }
    else {
        switch (state) {
            case MOESIF_CACHE_I : do_snoop_I (request); break;
            case MOESIF_CACHE_S : do_snoop_S (request); break;
            case MOESIF_CACHE_E : do_snoop_E (request); break;
            case MOESIF_CACHE_O : do_snoop_O (request); break;
            case MOESIF_CACHE_M : do_snoop_M (request); break;
            case MOESIF_CACHE_F : do_snoop_F (request); break;
            case MOESIF_CACHE_IM : do_snoop_IM (request); break;
            case MOESIF_CACHE_IS : do_snoop_IS (request); break;
            case MOESIF_CACHE_SM : do_snoop_SM (request); break;
            case MOESIF_CACHE_OM : do_snoop_OM (request); break;
            case MOESIF_CACHE_FM : do_snoop_FM (request); break;
            default:
                fatal_error("Invalid Cache State for MOESIF Protocol\n");
        }
    }
}

inline void MOESIF_protocol::do_cache_F (Mreq *request)
{
    switch (request->msg) {
        case LOAD:
            send_DATA_to_proc(request->addr); // send it up
            break;
        case STORE:
            send_GETM(request->addr);
            state = MOESIF_CACHE_FM;
            Sim->cache_misses++; //todo - might not need thi
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error("Client: CacheF state shouldn't see this message\n");
    }
}

inline void MOESIF_protocol::do_cache_I (Mreq *request)
{
    switch (request->msg) {
        case LOAD:
            // send GETS, go to intermediate state
            send_GETS(request->addr);
            state = MOESIF_CACHE_IS;
            Sim->cache_misses++;
            break;
        case STORE:
            // send GETM, go to intermediate state
            send_GETM(request->addr);
            state = MOESIF_CACHE_IM;
            Sim->cache_misses++;
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error("Client: CacheI state shouldn't see this message\n");
    }
}

inline void MOESIF_protocol::do_cache_S (Mreq *request)
{
    switch (request->msg) {
        case LOAD:
            send_DATA_to_proc(request->addr); // send to proc
            break;
        case STORE:
            // Transition to M state
            send_GETM(request->addr);
            state = MOESIF_CACHE_SM;
            Sim->cache_misses++;
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error("Client: CacheS state shouldn't see this message\n");
    }
}

inline void MOESIF_protocol::do_cache_E (Mreq *request)
{
    switch (request->msg) {
        case LOAD:
            send_DATA_to_proc(request->addr);
            break;
        case STORE:
            send_DATA_to_proc(request->addr);
            state = MOESIF_CACHE_M; // silent upgrade
            Sim->silent_upgrades++;
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error("Client: CacheE state shouldn't see this message\n");
    }
}

inline void MOESIF_protocol::do_cache_O (Mreq *request)
{
    switch (request->msg) {
        case LOAD:
            send_DATA_to_proc(request->addr); // send it up
            break;
        case STORE:
            send_GETM(request->addr); // go to intermediate state, send GETM
            state = MOESIF_CACHE_OM;
            Sim->cache_misses++;
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error("Client: CacheO state shouldn't see this message\n");
    }
}

inline void MOESIF_protocol::do_cache_M (Mreq *request)
{
    switch (request->msg) {
        // send to processor
        case LOAD:
        case STORE:
            send_DATA_to_proc(request->addr);
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error("Client: CacheM state shouldn't see this message\n");
    }
}

inline void MOESIF_protocol::do_cache_IM (Mreq *request) {
    switch (request->msg) {
        // one request per processor
        case LOAD:
        case STORE:
            request->print_msg(my_table->moduleID, "ERROR");
            fatal_error("CacheIM should only have one oustanding request per processor!\n");
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error("Client: CacheIM state shouldn't see this message\n");
    }
}

inline void MOESIF_protocol::do_cache_IS (Mreq *request) {
    switch (request->msg) {
        // one request per processor
        case LOAD:
        case STORE:
            request->print_msg(my_table->moduleID, "ERROR");
            fatal_error("CacheIM should only have one oustanding request per processor!\n");
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error("Client: CacheIS state shouldn't see this message\n");
    }
}

inline void MOESIF_protocol::do_cache_SM (Mreq *request) {
    switch (request->msg) {
        // one request per processor
        case LOAD:
        case STORE:
            request->print_msg(my_table->moduleID, "ERROR");
            fatal_error("CacheIM should only have one oustanding request per processor!\n");
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error("Client: CacheSM state shouldn't see this message\n");
    }
}

inline void MOESIF_protocol::do_cache_OM (Mreq *request) {
    switch (request->msg) {
        // one request per processor
        case LOAD:
        case STORE:
            request->print_msg(my_table->moduleID, "ERROR");
            fatal_error("CacheIM should only have one oustanding request per processor!\n");
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error("Client: CacheOM state shouldn't see this message\n");
    }
}

inline void MOESIF_protocol::do_cache_FM (Mreq *request) {
    switch (request->msg) {
        case LOAD:
        case STORE:
            request->print_msg(my_table->moduleID, "ERROR");
            fatal_error("Client: CacheFM state shouldn't see this message\n");
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error("Client: CacheFM state shouldn't see this message\n");
    }
}


inline void MOESIF_protocol::do_snoop_F (Mreq *request)
{
    switch (request->msg) {
        case GETS:
            send_DATA_on_bus(request->addr, request->src_mid);
            set_shared_line(true);
            break;
        case GETM:
            send_DATA_on_bus(request->addr, request->src_mid);
            state = MOESIF_CACHE_I;
            break;
        case DATA:
            request->print_msg(my_table->moduleID, "ERROR");
            fatal_error("SnoopF should not see data for this line! I have the line!\n");
            break;
        default:
            request->print_msg(my_table->moduleID, "ERROR");
            fatal_error("Client: SnoopF state shouldn't see this message\n");
    }
}

inline void MOESIF_protocol::do_snoop_I (Mreq *request)
{
    switch (request->msg) {
        // do nothing
        case GETS:
        case GETM:
        case DATA:
            break;
        default:
            request->print_msg(my_table->moduleID, "ERROR");
            fatal_error("Client: SnoopI state shouldn't see this message\n");
    }
}

inline void MOESIF_protocol::do_snoop_S (Mreq *request)
{
    switch (request->msg) {
        case GETS:
            set_shared_line(true); // set shared line
            break;
        case GETM:
            state = MOESIF_CACHE_I; // go to invalid
            break;
        case DATA:
            request->print_msg(my_table->moduleID, "ERROR");
            fatal_error("SnoopS should not see data for this line! I have the line!\n");
            break;
        default:
            request->print_msg(my_table->moduleID, "ERROR");
            fatal_error("Client: SnoopS state shouldn't see this message\n");
    }
}

inline void MOESIF_protocol::do_snoop_E (Mreq *request)
{
    switch (request->msg) {
        case GETS:
            // set shared line
            send_DATA_on_bus(request->addr, request->src_mid);
            set_shared_line(true);
            state = MOESIF_CACHE_F;
            break;
        case GETM:
            // go to invalid
            send_DATA_on_bus(request->addr, request->src_mid);
            state = MOESIF_CACHE_I;
            break;
        case DATA:
            request->print_msg(my_table->moduleID, "ERROR");
            fatal_error("SnoopE should not see data for this line! I have the line!\n");
            break;
        default:
            request->print_msg(my_table->moduleID, "ERROR");
            fatal_error("Client: SnoopE state shouldn't see this message\n");
    }
}

inline void MOESIF_protocol::do_snoop_O (Mreq *request)
{
    switch (request->msg) {
        case GETS:
            // send data to bus, set shared line
            send_DATA_on_bus(request->addr, request->src_mid);
            set_shared_line(true);
            break;
        case GETM:
            // send data to bus
            send_DATA_on_bus(request->addr, request->src_mid);
            state = MOESIF_CACHE_I;
            break;
        case DATA:
            request->print_msg(my_table->moduleID, "ERROR");
            fatal_error("SnoopO should not see data for this line! I have the line!\n");
            break;
        default:
            request->print_msg(my_table->moduleID, "ERROR");
            fatal_error("Client: SnoopO state shouldn't see this message\n");
    }
}

inline void MOESIF_protocol::do_snoop_M (Mreq *request)
{
    switch (request->msg) {
        case GETS:
            // send the data and go to O
            send_DATA_on_bus(request->addr, request->src_mid);
            set_shared_line(true);
            state = MOESIF_CACHE_O;
            break;
        case GETM:
            // send the data and go to I
            send_DATA_on_bus(request->addr, request->src_mid);
            state = MOESIF_CACHE_I;
            break;
        case DATA:
            request->print_msg(my_table->moduleID, "ERROR");
            fatal_error("SnoopM should not see data for this line! I have the line!\n");
            break;
        default:
            request->print_msg(my_table->moduleID, "ERROR");
            fatal_error("Client: SnoopM state shouldn't see this message\n");
    }
}

inline void MOESIF_protocol::do_snoop_IM (Mreq *request) {
    switch (request->msg) {
        // only care about data
        case GETS:
        case GETM:
            break;
        case DATA:
            // send the data up, set shared line
            send_DATA_to_proc(request->addr);
            set_shared_line(false);
            state = MOESIF_CACHE_M;
            break;
        default:
            request->print_msg(my_table->moduleID, "ERROR");
            fatal_error("Client: SnoopIM state shouldn't see this message\n");
    }
}

inline void MOESIF_protocol::do_snoop_IS (Mreq *request) {
    switch (request->msg) {
        case GETS:
        case GETM:
            break;
        case DATA:
            // send data up
            send_DATA_to_proc(request->addr);
            // check if there are other copies in other processors. If so
            // then we go to shared state, otherwise go to exclusive
            if (get_shared_line())
                state = MOESIF_CACHE_S;
            else {
                state = MOESIF_CACHE_E;
                set_shared_line(false);
            }
            break;
        default:
            request->print_msg(my_table->moduleID, "ERROR");
            fatal_error("Client: SnoopIS state shouldn't see this message\n");
    }
}

inline void MOESIF_protocol::do_snoop_SM (Mreq *request) {
    switch (request->msg) {
        case GETS:
            set_shared_line(true); // set line to shared
            break;
        case GETM:
            break;
        case DATA:
            // send data up, set shared line
            send_DATA_to_proc(request->addr);
            state = MOESIF_CACHE_M;
            set_shared_line(false);
            break;
        default:
            request->print_msg(my_table->moduleID, "ERROR");
            fatal_error("Client: SnoopSM state shouldn't see this message\n");
    }
}

inline void MOESIF_protocol::do_snoop_OM (Mreq *request) {
    switch (request->msg) {
        case GETS:
            // set shared line, send data to bus
            send_DATA_on_bus(request->addr, request->src_mid);
            set_shared_line(true);
            break;
        case GETM:
            // send the data to ourselves
            send_DATA_on_bus(request->addr, request->src_mid);
            break;
        case DATA:
            // send to processor, set shared line, go to M state
            send_DATA_to_proc(request->addr);
            state = MOESIF_CACHE_M;
            set_shared_line(false);
            break;
        default:
            request->print_msg(my_table->moduleID, "ERROR");
            fatal_error("Client: SnoopOM state shouldn't see this message\n");
    }
}

inline void MOESIF_protocol::do_snoop_FM (Mreq *request) {
    switch (request->msg) {
        case GETS:
            send_DATA_on_bus(request->addr, request->src_mid);
            set_shared_line(true);
            break;
        case GETM:
            send_DATA_on_bus(request->addr, request->src_mid);
            break;
        case DATA:
            send_DATA_to_proc(request->addr);
            state = MOESIF_CACHE_M;
            set_shared_line(false);
            break;
        default:
            request->print_msg(my_table->moduleID, "ERROR");
            fatal_error("Client: SnoopFM state shouldn't see this message\n");
    }
}