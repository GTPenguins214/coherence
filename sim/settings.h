#ifndef SETTINGS_H_
#define SETTINGS_H_

#include "enums.h"
#include "types.h"

typedef struct setts {
	char name[50];
	void *pointer;
} setts;

/**
 * Settings from the configuration file
 */
class Sim_settings {
public:
	// Dynamic Parameters
    int                  num_nodes;
	int	                 network_x_dimension;
	int                  network_y_dimension;

    int                  num_nhoods;
    int                  nhood_x_blocking_factor;
    int                  nhood_y_blocking_factor;

    int                  num_mem_ctrls;
    int*                 mem_ctrl_array;

    unsigned int         heartrate;

	bool 				 net_infinite_bw;
	bool 				 sharer_forwarding;
	bool  				 wait_on_inv_acks;
	bool 				 livelock_check;
    bool                 processor_affinity;
    bool                 mem_model_enabled;
    bool                 regression_test;

	// Cache Settings
	unsigned int		 cache_line_size_log2;
	unsigned int		 cache_line_size;

	// Processor
	bool                 LSQ_dependence;
    int                  mshrs_per_processor;
    int                  threads_per_processor;
    thread_map_t         thread_map_policy;

    /** Out-of-order processor.  */

    // L1
    cache_type_t         l1_cache_type;
	int	                 l1_cache_size;
	int	   	             l1_cache_assoc;
	int                  l1_hit_time;
    int                  l1_mshrs;
	replacement_policy_t l1_replacement_policy;
	coherence_policy_t	 l1_coherence_policy;
	cache_policy_t		 l1_cache_policy;
	int                  l1_lookup_time;
    bool                 l1_infinite;

    // Cache index and home node swizzle
    paddr_t              cache_index_swizzle;
    paddr_t              dir_home_swizzle;

    // Set Dueling
    bool                 qsets_enabled;
    int                  qsets_interval;

    int                  remap_table_size;

    // Selective Replica Predictor
    predictor_t          sel_rep_pred;
    predictor_scope_t    sel_rep_pred_scope;
    bool                 train_on_loads;
    bool                 train_on_stores;
    int                  sel_rep_pred_threshold;

    // Sim analysis tools
    bool                 sim_analysis_enabled;
    unsigned int         ro_tracker_gran;
    unsigned int         ro_tracker_entries;
	bool				 data_graph;

	// Network
    network_topology_t   network_topology;
    int					 express_link_len;
	bool				 express_link_active;
	// These are set by net_infinite_bandwidth and not the config file
	int					 num_virtual_channels;
	int					 buffer_entries_per_vc;

	long long int		 sampling_interval;

	sim_output_mode_t    report_output;

	paddr_t              debug_addr;
    paddr_t              test_addr;

    char                 *trace_dir;

    protocol_t protocol;
    bool debug;

    Sim_settings (void);
    ~Sim_settings (void);

    void set_defaults (void);  
  	void get_settings (void);
    void get_topology (void);
    void print_settings (void);
};

// Debug
#define GENERAL_DEBUG         false
#define TICK_TOCK_DEBUG       false
#define ADDR_DEBUG            (request->addr == Sim.settings.debug_addr)
#define LOGIC_DEBUG           false
#define DIR_HISTORY           false

#define O_ENABLED true

// Core configuration
#define ISSUE_WIDTH           2
                          
// Directory
#define DIR_LATENCY             3

#define MEM_HIT_TIME		    200	//assuming a 1.6GHz clock for simple cores
                                    //  ~150 ns to main mem

// Router configuration
#define LINK_FLIT_WIDTH         128

#define PACKET_OVERHEAD       64
//#define MIN_LINK_WIDTH        8
#define MAX_FLITS_PER_PACKET (((Sim.settings.cache_line_size << 3)+ PACKET_OVERHEAD) / LINK_FLIT_WIDTH)

#define NAME_ID_CHAR_BUFF        10

// Useful Macros
#define ABS(a)		    ((a) < 0 ? (0 - (a)) : (a))
#define ISPOW2(a)       (((a) & ((a) - 1)) ? 0 : 1)

#endif

