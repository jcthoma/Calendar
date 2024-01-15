#if !defined(CALENDAR_H)
#define CALENDAR_H

#include "event.h"
#include <stdio.h>

#define SUCCESS 0
#define FAILURE -1

typedef struct calendar {
  char *name;
  Event **events;
  int days, total_events;
  int (*comp_func)(const void *, const void *);
  void (*free_info_func)(void *);
} Calendar;

int init_calendar(const char *, int, int (*)(const void *, const void *),
                  void (*)(void *), Calendar **);
int print_calendar(Calendar *, FILE *, int);
int add_event(Calendar *, const char *, int, int, void *, int);
int find_event(Calendar *, const char *, Event **);
int find_event_in_day(Calendar *, const char *, int, Event **);
int remove_event(Calendar *, const char *);
void *get_event_info(Calendar *, const char *);
int clear_day(Calendar *, int);
int clear_calendar(Calendar *);
int destroy_calendar(Calendar *);

#endif
