#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "event.h"
#include "calendar.h"


int init_calendar(const char *name, int days, int (*comp_func) (const void *ptr1, const void *ptr2), 
                  void (*free_info_func) (void *ptr), Calendar ** calendar){

   /* Check for NULL pointers and allocate memory for the calendar. */
   if((*calendar) == NULL || name == NULL || days < 1){
      return FAILURE;
   }

   (*calendar) = malloc(sizeof(Calendar));

   /* Check if memory allocation is successful. */
   if((*calendar) == NULL){
      return FAILURE;
   }

   /* Initialize calendar properties. */
   (*calendar)->days = days;
   (*calendar)->total_events = 0;
   (*calendar)->name = malloc(sizeof(char) * (strlen(name) + 1));
   strcpy((*calendar)->name, name);     

   (*calendar)->events = calloc(days, sizeof(Event));
   (*calendar)->comp_func = comp_func;
   (*calendar)->free_info_func = free_info_func;
   
   return SUCCESS;
}

int print_calendar(Calendar *calendar, FILE *output_stream, int print_all){
   int i;
   Event *curr_event;
   
   /* Check for NULL pointers. */
   if(calendar == NULL || output_stream == NULL){
      return FAILURE;
   }

   if(print_all){
      /* Print calendar details if requested. */
      fprintf(output_stream, "Calendar's Name: \"%s\"\nDays: %d\n", calendar->name, calendar->days);
      fprintf(output_stream, "Total Events: %d\n\n", calendar->total_events);
   }

   fprintf(output_stream, "**** Events ****\n");

   if(calendar->total_events > 0){
      for(i = 0; i < calendar->days; i++){
         /* Print events for each day. */
         fprintf(output_stream, "Day %d\n", i + 1);

         for(curr_event = calendar->events[i]; curr_event != NULL; curr_event = curr_event->next){
            fprintf(output_stream, "Event's Name: \"%s\", Start_time: %d, Duration: %d\n",
                    curr_event->name, curr_event->start_time, curr_event->duration_minutes);
         }
      }   
   }
   return SUCCESS;
}

/* Helper function to create a new event */
static Event* create_event(const char *name, int start_time, int duration_minutes, void *info) {
   Event *new_event = malloc(sizeof(Event));
   if (new_event == NULL) {
      return NULL;  /* Memory allocation failed */
   }

   new_event->name = malloc(sizeof(char) * (strlen(name) + 1));
   if (new_event->name == NULL) {
      free(new_event);
      return NULL;  /* Memory allocation failed */
   }

   strcpy(new_event->name, name);
   new_event->duration_minutes = duration_minutes;
   new_event->start_time = start_time;
   new_event->info = info;
   new_event->next = NULL;

   return new_event;
}

/* Helper function to insert an event into the calendar in a sorted order */
static int insert_event(Calendar *calendar, Event *new_event, int day) {
   Event *curr = calendar->events[day - 1];
   Event *prev = NULL;

   while (curr != NULL && calendar->comp_func(curr, new_event) < 0) {
      prev = curr;
      curr = curr->next;
   }

   if (prev == NULL) {
      new_event->next = calendar->events[day - 1];
      calendar->events[day - 1] = new_event;
   } else {
      new_event->next = curr;
      prev->next = new_event;
   }

   return SUCCESS;
}

/* Main function to add an event to the calendar */
int add_event(Calendar *calendar, const char *name, int start_time,
              int duration_minutes, void *info, int day) {
   Event *new_event, *event_check;

   /* Check for NULL pointers and input validity. */
   if (calendar == NULL || name == NULL || start_time < 0 || start_time > 2400) {
      return FAILURE;
   }

   if (duration_minutes <= 0 || day < 1 || day > calendar->days) {
      return FAILURE;
   }

   /* Prevent duplicate events. */
   if (find_event(calendar, name, &event_check) == SUCCESS) {
      return FAILURE;
   }

   /* Create a new event. */
   new_event = create_event(name, start_time, duration_minutes, info);
   if (new_event == NULL) {
      return FAILURE;  /* Memory allocation failed */
   }

   /* Insert the new event into the calendar. */
   if (insert_event(calendar, new_event, day) == FAILURE) {
      free(new_event->name);
      free(new_event);
      return FAILURE;
   }

   calendar->total_events++;
   return SUCCESS;
}


int find_event(Calendar *calendar, const char *name, Event **event){
   int i;
   Event *curr;
 
   /* Check for NULL pointers. */
   if(calendar == NULL || name == NULL || event == NULL){
      return FAILURE;
   }

   for(i = 0; i < calendar->days; i++){
      curr = calendar->events[i];

      while(curr != NULL){
         /* Search for the event by name. */
         if(strcmp(curr->name, name) == 0){
            *event = curr;
            return SUCCESS;
         }
         curr = curr->next;
      }
   }
   
   return FAILURE;
}

int find_event_in_day(Calendar *calendar, const char *name, int day, Event **event){
   Event *curr;
   int i = 1;

   /* Check for NULL pointers and input validity. */
   if(calendar == NULL || name == NULL || event == NULL){
      return FAILURE;
   }

   if(day < 1 || day > calendar->days){
      return FAILURE;
   }

   while(i < calendar->days){
      if(i == day){
         curr = calendar->events[i - 1];

         while(curr != NULL && strcmp(curr->name, name) != 0){
            curr = curr->next;
         }

         if(curr != NULL){
            *event = curr;
            return SUCCESS;
         }
      }
   }

   return FAILURE;
}

int remove_event(Calendar *calendar, const char *name){
   Event *curr, *prev;
   int i;   

   /* Check for NULL pointers and input validity. */
   if(calendar == NULL || name == NULL){
      return FAILURE;
   }

   for(i = 0; i < calendar->days; i++){
      curr = calendar->events[i];
      prev = NULL;

      while(curr != NULL && strcmp(curr->name, name) != 0){
         prev = curr;
         curr = curr->next;
      }

      if(curr != NULL){
         if(prev != NULL){
            prev->next = curr->next;
         } else {
              calendar->events[i] = curr->next;
         }
  
         free(curr->name);
   
         if(curr->info != NULL && calendar->free_info_func != NULL){
            calendar->free_info_func(curr->info);
         }
         free(curr);
     
         calendar->total_events--;
         return SUCCESS;
      }
      
   }
  
   return FAILURE;  
}

int destroy_calendar(Calendar *calendar){

   /* Check for NULL pointer. */
   if(calendar == NULL){
      return FAILURE;
   }

   /* Free allocated memory and clear the calendar. */
   free(calendar->name);
   clear_calendar(calendar);
   free(calendar->events);
   free(calendar);
   return SUCCESS;
}

void *get_event_info(Calendar *calendar, const char *name){
   Event *event;

   /* Check for NULL pointers and find the event. */
   if(find_event(calendar, name, &event) == FAILURE){
      return NULL;
   }
   return event->info;
}

int clear_calendar(Calendar *calendar){
   int i;
 
   /* Check for NULL pointer. */
   if(calendar == NULL){
      return FAILURE;
   }

   /* Clear each day in the calendar. */
   for(i = 0; i < calendar->days; i++){
      clear_day(calendar, i + 1);
   }

   calendar->total_events = 0;
   return SUCCESS;
}


int clear_day(Calendar *calendar, int day){
   Event *curr, *prev;

   /* Check for NULL pointers and input validity. */
   if(calendar == NULL || day < 1 || day > calendar->days){
      return FAILURE;
   }

   curr = calendar->events[day - 1];
   while(curr != NULL){
      prev = curr;
      curr = curr->next;

      /* Free memory for the event's name and info. */
      free(prev->name);
      if(prev->info != NULL && calendar->free_info_func != NULL){
         calendar->free_info_func(prev->info);
      }
      
      free(prev);  

      calendar->total_events--;
   }
   calendar->events[day - 1] = NULL;

   return SUCCESS;
} 







