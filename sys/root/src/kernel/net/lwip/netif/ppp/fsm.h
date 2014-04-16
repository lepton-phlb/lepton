
#ifndef FSM_H
#define FSM_H

/*****************************************************************************
************************* PUBLIC DEFINITIONS *********************************
*****************************************************************************/
/*
 * LCP Packet header = Code, id, length.
 */
#define HEADERLEN (sizeof (u_char) + sizeof (u_char) + sizeof (u_short))


/*
 *  CP (LCP, IPCP, etc.) codes.
 */
#define CONFREQ     1 /* Configuration Request */
#define CONFACK     2 /* Configuration Ack */
#define CONFNAK     3 /* Configuration Nak */
#define CONFREJ     4 /* Configuration Reject */
#define TERMREQ     5 /* Termination Request */
#define TERMACK     6 /* Termination Ack */
#define CODEREJ     7 /* Code Reject */

/*
 * Link states.
 */
#define LS_INITIAL  0 /* Down, hasn't been opened */
#define LS_STARTING 1 /* Down, been opened */
#define LS_CLOSED   2 /* Up, hasn't been opened */
#define LS_STOPPED  3 /* Open, waiting for down event */
#define LS_CLOSING  4 /* Terminating the connection, not open */
#define LS_STOPPING 5 /* Terminating, but open */
#define LS_REQSENT  6 /* We've sent a Config Request */
#define LS_ACKRCVD  7 /* We've received a Config Ack */
#define LS_ACKSENT  8 /* We've sent a Config Ack */
#define LS_OPENED   9 /* Connection available */

/*
 * Flags - indicate options controlling FSM operation
 */
#define OPT_PASSIVE 1 /* Don't die if we don't get a response */
#define OPT_RESTART 2 /* Treat 2nd OPEN as DOWN, UP */
#define OPT_SILENT  4 /* Wait for peer to speak first */


/*****************************************************************************
************************* PUBLIC DATA TYPES **********************************
*****************************************************************************/
/*
 * Each FSM is described by an fsm structure and fsm callbacks.
 */
typedef struct fsm {
  int unit;                        /* Interface unit number */
  u_short protocol;                /* Data Link Layer Protocol field value */
  int state;                       /* State */
  int flags;                       /* Contains option bits */
  u_char id;                       /* Current id */
  u_char reqid;                    /* Current request id */
  u_char seen_ack;                 /* Have received valid Ack/Nak/Rej to Req */
  int timeouttime;                 /* Timeout time in milliseconds */
  int maxconfreqtransmits;         /* Maximum Configure-Request transmissions */
  int retransmits;                 /* Number of retransmissions left */
  int maxtermtransmits;            /* Maximum Terminate-Request transmissions */
  int nakloops;                    /* Number of nak loops since last ack */
  int maxnakloops;                 /* Maximum number of nak loops tolerated */
  struct fsm_callbacks* callbacks; /* Callback routines */
  char* term_reason;               /* Reason for closing protocol */
  int term_reason_len;             /* Length of term_reason */
} fsm;


typedef struct fsm_callbacks {
  void (*resetci)(fsm*);                            /* Reset our Configuration Information */
  int  (*cilen)(fsm*);                              /* Length of our Configuration Information */
  void (*addci)(fsm*, u_char*, int*);               /* Add our Configuration Information */
  int  (*ackci)(fsm*, u_char*, int);                /* ACK our Configuration Information */
  int  (*nakci)(fsm*, u_char*, int);                /* NAK our Configuration Information */
  int  (*rejci)(fsm*, u_char*, int);                /* Reject our Configuration Information */
  int  (*reqci)(fsm*, u_char*, int*, int);          /* Request peer's Configuration Information */
  void (*up)(fsm*);                                 /* Called when fsm reaches LS_OPENED state */
  void (*down)(fsm*);                               /* Called when fsm leaves LS_OPENED state */
  void (*starting)(fsm*);                           /* Called when we want the lower layer */
  void (*finished)(fsm*);                           /* Called when we don't want the lower layer */
  void (*protreject)(int);                          /* Called when Protocol-Reject received */
  void (*retransmit)(fsm*);                         /* Retransmission is necessary */
  int  (*extcode)(fsm*, int, u_char, u_char*, int); /* Called when unknown code received */
  char *proto_name;                                 /* String name for protocol (for messages) */
} fsm_callbacks;


/*****************************************************************************
*********************** PUBLIC DATA STRUCTURES *******************************
*****************************************************************************/
/*
 * Variables
 */
extern int peer_mru[]; /* currently negotiated peer MRU (per unit) */


/*****************************************************************************
************************** PUBLIC FUNCTIONS **********************************
*****************************************************************************/

/*
 * Prototypes
 */
void fsm_init (fsm*);
void fsm_lowerup (fsm*);
void fsm_lowerdown (fsm*);
void fsm_open (fsm*);
void fsm_close (fsm*, char*);
void fsm_input (fsm*, u_char*, int);
void fsm_protreject (fsm*);
void fsm_sdata (fsm*, u_char, u_char, u_char*, int);

#endif /* FSM_H */
