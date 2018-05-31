// replace this with your netxtbus.com stop info
// you may have to remove the leading '1' from the stop number
// for example, the below info comes from
// https://www.nextbus.com/#!/sf-muni/KT/KT___O_P00/7355/5237
const char *host = "webservices.nextbus.com";
const char *nb_agency = "sf-muni";
const char *nb_route = "KT";
const char *nb_stop = "7355";

// minimum time to walk to stop
const int MIN_WALKING_TIME = 3;

// get predictions every
const int POLL_SECONDS = 60;
