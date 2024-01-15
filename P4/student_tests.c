#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "event.h"
#include "calendar.h"
#include "calendar.h"
#include "my_memory_checker_216.h"

/*****************************************************/
/* In this file you will provide tests for your      */
/* calendar application.  Each test should be named  */
/* test1(), test2(), etc. Each test must have a      */
/* brief description of what it is testing (this     */
/* description is important).                        */
/*                                                   */
/* You can tell whether any test failed if after     */
/* executing the students tests, you executed        */
/* "echo $?" on the command line and you get a value */
/* other than 0.  "echo $?" prints the status of     */
/* the last command executed by the shell.           */ 
/*                                                   */
/* Notice that main just calls test1(), test2(), etc.*/
/* and once one fails, the program eventually        */
/* return EXIT_FAILURE; otherwise EXIT_SUCCESS will  */
/* be returned.                                      */
/*****************************************************/

static int comp_minutes(const void *ptr1, const void *ptr2) {
   return ((Event *)ptr1)->duration_minutes - ((Event *)ptr2)->duration_minutes;
}

/* Description here: This test checks ...  */
static int test0() {
   int days = 7;
   Calendar *calendar;

   if (init_calendar("Spr", days, comp_minutes, NULL, &calendar) == SUCCESS) {
      if (print_calendar(calendar, stdout, 1) == SUCCESS) {
         return destroy_calendar(calendar);
      }
   }
    
   return FAILURE;
}

/* Description: Test initializing a calendar with valid parameters */
static int test1() {
   int days = 7;
   Calendar *calendar;

   if (init_calendar("Week", days, comp_minutes, NULL, &calendar) == SUCCESS) {
      if (calendar != NULL && strcmp(calendar->name, "Week") == 0 && calendar->days == days) {
         return destroy_calendar(calendar);
      }
   }

   return FAILURE;
}

/* Description: Test printing an empty calendar */
static int test2() {
   int days = 7;
   Calendar *calendar;

   if (init_calendar("Week", days, comp_minutes, NULL, &calendar) == SUCCESS) {
      if (print_calendar(calendar, stdout, 1) == SUCCESS) {
         return destroy_calendar(calendar);
      }
   }

   return FAILURE;
}

/* Description: Test adding and printing events in the calendar */
static int test3() {
   int days = 7;
   Calendar *calendar;

   if (init_calendar("Week", days, comp_minutes, NULL, &calendar) == SUCCESS) {
      if (add_event(calendar, "Meeting", 900, 60, NULL, 3) == SUCCESS) {
         if (add_event(calendar, "Lunch", 1200, 45, NULL, 5) == SUCCESS) {
            if (print_calendar(calendar, stdout, 1) == SUCCESS) {
               return destroy_calendar(calendar);
            }
         }
      }
   }

   return FAILURE;
}

/* Description: Test finding an event in the calendar */
static int test4() {
   int days = 7;
   Calendar *calendar;
   Event *found_event;

   if (init_calendar("Week", days, comp_minutes, NULL, &calendar) == SUCCESS) {
      if (add_event(calendar, "Meeting", 900, 60, NULL, 3) == SUCCESS) {
         if (find_event(calendar, "Meeting", &found_event) == SUCCESS) {
            if (found_event != NULL && strcmp(found_event->name, "Meeting") == 0) {
               return destroy_calendar(calendar);
            }
         }
      }
   }

   return FAILURE;
}

/* Description: Test removing an event from the calendar */
static int test5() {
   int days = 7;
   Calendar *calendar;

   if (init_calendar("Week", days, comp_minutes, NULL, &calendar) == SUCCESS) {
      if (add_event(calendar, "Meeting", 900, 60, NULL, 3) == SUCCESS) {
         if (remove_event(calendar, "Meeting") == SUCCESS) {
            if (find_event(calendar, "Meeting", NULL) == FAILURE) {
               return destroy_calendar(calendar);
            }
         }
      }
   }

   return FAILURE;
}

/* Description: Test finding an event in a specific day of the calendar */
static int test6() {
   int days = 7;
   Calendar *calendar;
   Event *found_event;

   if (init_calendar("Week", days, comp_minutes, NULL, &calendar) == SUCCESS) {
      if (add_event(calendar, "Meeting", 900, 60, NULL, 3) == SUCCESS) {
         if (find_event_in_day(calendar, "Meeting", 3, &found_event) == SUCCESS) {
            if (found_event != NULL && strcmp(found_event->name, "Meeting") == 0) {
               return destroy_calendar(calendar);
            }
         }
      }
   }

   return FAILURE;
}

/* Description: Test destroying a valid calendar */
static int test7() {
   int days = 7;
   Calendar *calendar;

   if (init_calendar("Week", days, comp_minutes, NULL, &calendar) == SUCCESS) {
      return destroy_calendar(calendar);
   }

   return FAILURE;
}

/* Description: Test getting event info from a valid calendar */
static int test8() {
   int days = 7;
   Calendar *calendar;
   const char *event_info = "Info";  

   if (init_calendar("Week", days, comp_minutes, NULL, &calendar) == SUCCESS) {
      if (add_event(calendar, "Meeting", 900, 60, (void *)event_info, 3) == SUCCESS) {
         const void *retrieved_info = get_event_info(calendar, "Meeting");

         if (retrieved_info != NULL && strcmp((const char *)retrieved_info, event_info) == 0) {
            return destroy_calendar(calendar);
         }
      }
   }

   return FAILURE;
}

/* Description: Test clearing a valid calendar */
static int test9() {
   int days = 7;
   Calendar *calendar;

   if (init_calendar("Week", days, comp_minutes, NULL, &calendar) == SUCCESS) {
      if (add_event(calendar, "Meeting", 900, 60, NULL, 3) == SUCCESS) {
         if (clear_calendar(calendar) == SUCCESS && calendar->total_events == 0) {
            return destroy_calendar(calendar);
         }
      }
   }

   return FAILURE;
}

/* Description: Test adding the same event twice */
static int test10() {
   int days = 7;
   Calendar *calendar;

   if (init_calendar("Week", days, comp_minutes, NULL, &calendar) == SUCCESS) {
      const char *event_name = "Meeting";
      int start_time = 900;
      int duration_minutes = 60;
      void *event_info = NULL;

      if (add_event(calendar, event_name, start_time, duration_minutes, event_info, 3) == SUCCESS) {
         int result = add_event(calendar, event_name, start_time, duration_minutes, event_info, 3);

         if (result == FAILURE && calendar->total_events == 1) {
            printf("Adding the same event twice failed as expected.\n");

            return destroy_calendar(calendar);
         }
      }
   }

   return FAILURE;
}

/* Test removing an event from the middle of a list */
static int test11() {
   int days = 5;
   Calendar *calendar;

   if (init_calendar("Week", days, comp_minutes, NULL, &calendar) == SUCCESS) {
      if (add_event(calendar, "Meeting", 800, 60, NULL, 1) == SUCCESS) {
         if (add_event(calendar, "Dinner", 800, 80, NULL, 1) == SUCCESS) {
            if (add_event(calendar, "Conference", 800, 120, NULL, 1) == SUCCESS) {
               if (remove_event(calendar, "Dinner") == SUCCESS) {
                  if (find_event(calendar, "Dinner", NULL) == FAILURE) {
                     return destroy_calendar(calendar);
                  }
               }
            }
         }
      }
   }

   return FAILURE;
}



int main() {
   int result = SUCCESS;

   /***** Starting memory checking *****/
   start_memory_check();
   /***** Starting memory checking *****/

   if (test0() == FAILURE) result = FAILURE;
   if (test1() == FAILURE) result = FAILURE;
   if (test2() == FAILURE) result = FAILURE;
   if (test3() == FAILURE) result = FAILURE;
   if (test4() == FAILURE) result = FAILURE;
   if (test5() == FAILURE) result = FAILURE;
   if (test6() == FAILURE) result = FAILURE;
   if (test6() == FAILURE) result = FAILURE;
   if (test7() == FAILURE) result = FAILURE;
   if (test8() == FAILURE) result = FAILURE;
   if (test9() == FAILURE) result = FAILURE;
   if (test10() == FAILURE) result = FAILURE;
   if (test11() == FAILURE) result = FAILURE;

   /****** Gathering memory checking info *****/
   stop_memory_check();
   /****** Gathering memory checking info *****/
   
   if (result == FAILURE) {
      exit(EXIT_FAILURE);
   }

   return EXIT_SUCCESS;
}
