// Terminal color code
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define NUM_RECIPES 5
#define MAX_ORDERS 2000
#define MAX_STAGES 6

#define NUM_STATIONS 4
#define NUM_CHEFS 3

// All the stages
#define STATION_STOVE 0
#define STATION_OVEN 1
#define STATION_PREP 2
#define STATION_SINK 3

#define STATION_NAME_STOVE "stove"
#define STATION_NAME_OVEN "oven"
#define STATION_NAME_PREP "prep"
#define STATION_NAME_SINK "sink"

#define RECIPE_1_LEN 6
#define RECIPE_2_LEN 3
#define RECIPE_3_LEN 3
#define RECIPE_4_LEN 3
#define RECIPE_5_LEN 6

struct stage {
  int type;
  int time;
};

struct recipe {
  int num_stages;
  struct stage stages[MAX_STAGES];
};

void init_kitchen();
void get_off_work();
void init_recipes();
void *chef(void *ptr);
void *customer(void *ptr);
void get_to_work();
void get_off_work();
