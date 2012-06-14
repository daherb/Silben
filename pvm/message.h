#define MSG_SLAVE_READY 1   // Parameter Slave-TID
#define MSG_MASTER_EOD 2    // No Parameter
#define MSG_SLAVE_RESULT 3  // Parameter Slave-TID, Result-Count, Array of Results
#define MSG_MASTER_DATA 4   // Parameter Data-Count, Array of Data
#define MSG_MASTER_EXIT 5   // No Parameter
#define MSG_SLAVE_ERROR 666 // Parameter Slave-TID, Error Numer or at least -1
#define PACK_SIZE 4096        // Number of Elements in a Data Packet
