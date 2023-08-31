#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct Conch Conch;

Conch* conch_init(int64_t initial_conch_value);
void conch_checkin(Conch* conch, int64_t new_conch_value);
int64_t conch_checkout(Conch* conch);
bool conch_is_available(Conch* conch);
void conch_free(Conch* conch);
