#include "MSI_protocol.h"
#include "../sim/mreq.h"
#include "../sim/sim.h"
#include "../sim/hash_table.h"

extern Simulator *Sim;

/*************************
 * Constructor/Destructor.
 *************************/
MSI_protocol::MSI_protocol (Hash_table *my_table, Hash_entry *my_entry)
    : Protocol (my_table, my_entry)
{
    // Initialize lines to not have the data yet!
    this->state = MSI_CACHE_I;
}

MSI_protocol::~MSI_protocol ()
{    
}

void MSI_protocol::dump (void)
{
    const char *block_states[7] = {"X","I","S","M", "IS", "IM", "SM"};
    if (state > (sizeof(block_states) / sizeof(char*)))
    {
        fprintf(stderr, "INVALID PRINT STATE\n");
    }
    else
        fprintf (stdout, "MSI_protocol - state: %s\n", block_states[state]);
}

void MSI_protocol::process_cache_request (Mreq *request)
{
	switch (state) {
    case MSI_CACHE_I: do_cache_I (request); break;
    case MSI_CACHE_M: do_cache_M (request); break;
    case MSI_CACHE_S: do_cache_S (request); break;
    case MSI_CACHE_IS: do_cache_IS (request); break;
    case MSI_CACHE_IM: do_cache_IM (request); break;
    case MSI_CACHE_SM: do_cache_SM (request); break;
    default:
        fatal_error ("Invalid Cache State for MSI Protocol\n");
    }
}

void MSI_protocol::process_snoop_request (Mreq *request)
{
	switch (state) {
    case MSI_CACHE_I: do_snoop_I (request); break;
    case MSI_CACHE_M: do_snoop_M (request); break;
    case MSI_CACHE_S: do_snoop_S (request); break;
    case MSI_CACHE_IS: do_snoop_IS (request); break;
    case MSI_CACHE_IM: do_snoop_IM (request); break;
    case MSI_CACHE_SM: do_snoop_SM (request); break;
    default:
    	fatal_error ("Invalid Cache State for MSI Protocol\n");
    }
}

inline void MSI_protocol::do_cache_I (Mreq *request)
{
    switch (request->msg) {
        case LOAD:
            // Only want to read the data
            send_GETS(request->addr);
            state = MSI_CACHE_IS; // Invalid-Shared state
            Sim->cache_misses++; // cache miss
            break;
        case STORE:
            // Want to modify the data
            send_GETM(request->addr);
            state = MSI_CACHE_IM; // Invalid-Modified state
            Sim->cache_misses++; // cache miss
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: CacheI state shouldn't see this message\n");
    }
}

inline void MSI_protocol::do_cache_S (Mreq *request)
{
    switch (request->msg) {
        case LOAD:
            // Already have it so send to the processor
            send_DATA_to_proc(request->addr);
            break;
        case STORE:
            // Transition to M state, all other invalidate
            send_GETM(request->addr);
            state = MSI_CACHE_SM; // Shared-Modified State
            Sim->cache_misses++;
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: CacheS state shouldn't see this message\n");
    }
}

inline void MSI_protocol::do_cache_M (Mreq *request)
{
    switch (request->msg) {
        // Load or Store: already have it so send it to processor. 
        case LOAD:
        case STORE:
            send_DATA_to_proc(request->addr);
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: CacheM state shouldn't see this message\n");
    }
}

inline void MSI_protocol::do_cache_IS (Mreq *request) {
    switch (request->msg) {
        // Shouldn't be getting any requests because we have a pending request
        case LOAD:
        case STORE:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error("CacheIS should only have one outstanding request per processor!\n");
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: CacheIS state shouldn't see this message\n");
    }
}

inline void MSI_protocol::do_cache_IM (Mreq *request) {
    switch (request->msg) {
        // Shouldn't be getting any request because we have a pending request
        case LOAD:
        case STORE:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error("CacheIM should only have one outstanding request per processor!\n");
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: CacheIM state shouldn't see this message\n");
    }
}

inline void MSI_protocol::do_cache_SM (Mreq *request) {
    switch (request->msg) {
        // Shouldn't be getting any requests because we have a pending request
        case LOAD:
        case STORE:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error("CacheSM should only have one outstanding request per processor!");
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: CacheSM state shouldn't see this message\n");
    }
}

inline void MSI_protocol::do_snoop_I (Mreq *request)
{
    switch (request->msg) {
        // Invalid state so we don't do anything on bus requests
        case GETS:
        case GETM:
        case DATA:
            break;
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: SnoopI state shouldn't see this message\n");
    }
}

inline void MSI_protocol::do_snoop_S (Mreq *request)
{
    switch (request->msg) {
        // If we see a getS then don't do anything
        case GETS:
            break;
        case GETM:
            state = MSI_CACHE_I; // Invalidate if we see a GetM
            break;
        case DATA:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("SnoopS should not see data for this line!  I have the line!\n");
        default:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("Client: SnoopS state shouldn't see this message\n");
    }
}

inline void MSI_protocol::do_snoop_M (Mreq *request)
{
    switch (request->msg) {
        case GETS:
            send_DATA_on_bus(request->addr, request->src_mid); // Write back
            state = MSI_CACHE_S; // Go to shared
            break;
        case GETM:
            send_DATA_on_bus(request->addr, request->src_mid); // Write back
            state = MSI_CACHE_I; // Invalidate
            break;
        case DATA:
            request->print_msg (my_table->moduleID, "ERROR");
            fatal_error ("SnoopM should not see data for this line!  I have the line!\n");
            break;
        default:
            request->print_msg(my_table->moduleID, "ERROR");
            fatal_error ("Client: SnoopM state shouldn't see this message\n");
    }
}

inline void MSI_protocol::do_snoop_IS (Mreq *request) {
    switch (request->msg) {
        // Don't need to do anything, we just want data.
        case GETS:
        case GETM:
            break;
        case DATA:
            send_DATA_to_proc(request->addr); // Send the data to the processor
            state = MSI_CACHE_S; // Change to the shared state
            break;
        default:
            request->print_msg(my_table->moduleID, "ERROR");
            fatal_error ("Client: SnoopIS state shouldn't see this message\n");
    }
}
inline void MSI_protocol::do_snoop_IM (Mreq *request) {
    switch (request->msg) {
        // Don't need to do anything, we just want data
        case GETS:
        case GETM:
            break;
        case DATA:
            send_DATA_to_proc(request->addr); // Send the data to the processor
            state = MSI_CACHE_M; // Change to the modified state
            break;
        default:
            request->print_msg(my_table->moduleID, "ERROR");
            fatal_error ("Client: SnoopIM state shouldn't see this message\n");
    }
}

inline void MSI_protocol::do_snoop_SM (Mreq *request) {
    switch (request->msg) {
        // Don't need to do anything, just want data
        case GETS:
            break;
        case GETM:
            break;
        case DATA:
            send_DATA_to_proc(request->addr); // Send the data to the processor
            state = MSI_CACHE_M; // change to the modified state
            break;
        default:
            request->print_msg(my_table->moduleID, "ERROR");
            fatal_error ("Client: SnoopSM state shouldn't see this message\n");
    }
}