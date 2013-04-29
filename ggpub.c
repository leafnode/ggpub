/*
 * przyk³ad prostego programu ³±cz±cego siê z serwerem i wysy³aj±cego
 * jedn± wiadomo¶æ.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <libgadu.h>
#include "xmalloc.h"
#include "dynstuff.h"

int match_arg(char *arg, char chr, char * option, int chgw) {
   if (strlen(arg) == 2) {
      return chr == arg[1];
   } else {
      return !strcmp((arg+2), option);
   }
}

uin_t get_uin(char * str) {
   return atoi(str);
}

void cp_to_iso(unsigned char *buf)
{
   if (!buf)
      return;

   while (*buf) {
      if (*buf == (unsigned char)'¥') *buf = '¡';
      if (*buf == (unsigned char)'¹') *buf = '±';
      if (*buf == 140) *buf = '¦';
      if (*buf == 156) *buf = '¶';
      if (*buf == 143) *buf = '¬';
      if (*buf == 159) *buf = '¼';

      if (*buf != 13 && *buf != 10 && (*buf < 32 || (*buf > 127 && *buf < 160)))
	 *buf = '?';

      buf++;
   }
}

void iso_to_cp(unsigned char *buf)
{
   if (!buf)
      return;

   while (*buf) {
      if (*buf == (unsigned char)'¡') *buf = '¥';
      if (*buf == (unsigned char)'±') *buf = '¹';
      if (*buf == (unsigned char)'¦') *buf = 'Œ';
      if (*buf == (unsigned char)'¶') *buf = 'œ';
      if (*buf == (unsigned char)'¬') *buf = '';
      if (*buf == (unsigned char)'¼') *buf = 'Ÿ';
      buf++;
   }
}

void iso_to_ascii(unsigned char *buf)
{
   if (!buf)
      return;

   while (*buf) {
      if (*buf == (unsigned char)'±') *buf = 'a';
      if (*buf == (unsigned char)'ê') *buf = 'e';
      if (*buf == (unsigned char)'æ') *buf = 'c';
      if (*buf == (unsigned char)'³') *buf = 'l';
      if (*buf == (unsigned char)'ñ') *buf = 'n';
      if (*buf == (unsigned char)'ó') *buf = 'o';
      if (*buf == (unsigned char)'¶') *buf = 's';
      if (*buf == (unsigned char)'¿') *buf = 'z';
      if (*buf == (unsigned char)'¼') *buf = 'z';

      if (*buf == (unsigned char)'¡') *buf = 'A';
      if (*buf == (unsigned char)'Ê') *buf = 'E';
      if (*buf == (unsigned char)'Æ') *buf = 'C';
      if (*buf == (unsigned char)'£') *buf = 'L';
      if (*buf == (unsigned char)'Ñ') *buf = 'N';
      if (*buf == (unsigned char)'Ó') *buf = 'O';
      if (*buf == (unsigned char)'¦') *buf = 'S';
      if (*buf == (unsigned char)'¯') *buf = 'Z';
      if (*buf == (unsigned char)'¬') *buf = 'Z';

      buf++;
   }
}

int main(int argc, char **argv)
{

   // Do po³±czenia
   struct gg_session *sess;
   struct gg_event *e;
   struct gg_login_params p;
   list_t searches = NULL;

   char *last_search_first_name = NULL;
   char *last_search_last_name = NULL;
   char *last_search_nickname = NULL;
   uin_t last_search_uin = 0;

   gg_pubdir50_t res;
   // Do wyszukiwania
   char *user;
   gg_pubdir50_t req;
   int i, all = 0;

   if (argc < 4) {
      fprintf(stderr, "Usage: %s <my_uid> <my_password> <params>\n\n", argv[0]);
      fprintf(stderr, "Parameters:\n");
      fprintf(stderr, " -f <val> (--first)     - first name\n");
      fprintf(stderr, " -l <val> (--last)      - last name\n");
      fprintf(stderr, " -n <val> (--nickname)  - nickname\n");
      fprintf(stderr, " -c <val> (--city)      - city\n");
      fprintf(stderr, " -u <val> (--uin)       - user ID\n");
      fprintf(stderr, " -F       (--female)    - search females only\n");
      fprintf(stderr, " -M       (--male)      - search males only\n");
      fprintf(stderr, " -a       (--active)    - search on-line users only\n");
      fprintf(stderr, " -b <val> (--born)      - year of birth\n");
      fprintf(stderr, " -s <val> (--start)     - search offset\n");
      fprintf(stderr, " -A       (--all)       - show all\n");
      return 1;
   }

   // Poziom debugowania
   gg_debug_level = 5;

   memset(&p, 0, sizeof(p));
   p.uin = atoi(argv[1]);
   p.password = argv[2];


   // Po³±czenie

   if (!(sess = gg_login(&p))) {
      printf("Connection failed: %s\n", strerror(errno));
      gg_free_session(sess);
      return 1;
   }

   printf("Po³±czono.\n");

   // Wyszukiwanie

   if (!sess || sess->state != GG_STATE_CONNECTED) {
      printf("not_connected\n");
      return 1;
   }

   argv = argv + 3;

   // próba odpalenia trybu szukania konkretnego usera
   user = strdup(argv[0]);

   // Konwersja do CP
   for (i = 0; argv[i]; i++) {
      iso_to_cp(argv[i]);
   }

   // Zapytanie
   if (!(req = gg_pubdir50_new(GG_PUBDIR50_SEARCH))) {
      printf("\n");
      return 1;
   }

   // wyszukiwanie UIN
   if (argv[0] && argv[0][0] != '-') {
      uin_t uin = get_uin(user);

      if (!uin) {
	 printf("User not found (%s)\n", user);
	 return 1;
      }

      gg_pubdir50_add(req, GG_PUBDIR50_UIN, itoa(uin));
      i = 1;

   } else {
      i = 0;
   }

   free(user);

   // Parsowanie argumentów
   for (; argv[i]; i++) {
      char *arg = argv[i];

      if (match_arg(arg, 'f', "first", 2) && argv[i + 1]) {
	 gg_pubdir50_add(req, GG_PUBDIR50_FIRSTNAME, argv[++i]);
	 continue;
      }

      if (match_arg(arg, 'l', "last", 2) && argv[i + 1]) {
	 gg_pubdir50_add(req, GG_PUBDIR50_LASTNAME, argv[++i]);
	 continue;
      }

      if (match_arg(arg, 'n', "nickname", 2) && argv[i + 1]) {
	 gg_pubdir50_add(req, GG_PUBDIR50_NICKNAME, argv[++i]);
	 continue;
      }

      if (match_arg(arg, 'c', "city", 2) && argv[i + 1]) {
	 gg_pubdir50_add(req, GG_PUBDIR50_CITY, argv[++i]);
	 continue;
      }

      if (match_arg(arg, 'u', "uin", 2) && argv[i + 1]) {
	 gg_pubdir50_add(req, GG_PUBDIR50_UIN, itoa(get_uin(argv[++i])));
	 continue;
      }

      if (match_arg(arg, 's', "start", 3) && argv[i + 1]) {
	 gg_pubdir50_add(req, GG_PUBDIR50_START, argv[++i]);
	 continue;
      }

      if (match_arg(arg, 'F', "female", 2)) {
	 gg_pubdir50_add(req, GG_PUBDIR50_GENDER, GG_PUBDIR50_GENDER_FEMALE);
	 continue;
      }

      if (match_arg(arg, 'M', "male", 2)) {
	 gg_pubdir50_add(req, GG_PUBDIR50_GENDER, GG_PUBDIR50_GENDER_MALE);
	 continue;
      }

      if (match_arg(arg, 'a', "active", 2)) {
	 gg_pubdir50_add(req, GG_PUBDIR50_ACTIVE, GG_PUBDIR50_ACTIVE_TRUE);
	 continue;
      }

      if (match_arg(arg, 'b', "born", 2) && argv[i + 1]) {
	 char *foo = strchr(argv[++i], ':');

	 if (foo)
	    *foo = ' ';

	 gg_pubdir50_add(req, GG_PUBDIR50_BIRTHYEAR, argv[i]);
	 continue;
      }

      if (match_arg(arg, 'A', "all", 3)) {
	 if (!gg_pubdir50_get(req, 0, GG_PUBDIR50_START))
	    gg_pubdir50_add(req, GG_PUBDIR50_START, "0");
	 all = 1;
	 continue;
      }

      printf("invalid_params\n");

      gg_pubdir50_free(req);

      return 1;
   }

   // Wywo³anie wyszukiwania

   if (!gg_pubdir50(sess, req)) {
      printf("Search failed\n");
      return 1;
   }

   // Dodanie wyniku wyszykiwania do listy
   if (all) {
      list_add(&searches, req, 0);
   } else {
      gg_pubdir50_free(req);
   }

   // Pêtla czekaj±ca na wynik wyszukiwania
   while (1) {
      if (!(e = gg_watch_fd(sess))) {
	 printf("Connection interrupted: %s\n", strerror(errno));
	 gg_logoff(sess);
	 gg_free_session(sess);
	 return 1;
      }

      if (e->type == GG_EVENT_PUBDIR50_SEARCH_REPLY) {
         printf("Received results!\n");
         res = e->event.pubdir50;
         all = i = 0;
         int count, all = 0;
         list_t l;
         uin_t last_uin = 0;

         if ((count = gg_pubdir50_count(res)) < 1) {
            printf("search_not_found\n");
            return 1;
         }

         gg_debug(GG_DEBUG_MISC, "handle_search50, count = %d\n", gg_pubdir50_count(res));

         for (l = searches; l; l = l->next) {
            gg_pubdir50_t req = l->data;

            if (gg_pubdir50_seq(req) == gg_pubdir50_seq(res)) {
               all = 1;
               break;
            }
         }

         for (i = 0; i < count; i++) {
            const char *__fmnumber = gg_pubdir50_get(res, i, "fmnumber");
            const char *uin = (__fmnumber) ? __fmnumber : "?";

            const char *__firstname = gg_pubdir50_get(res, i, "firstname");
            char *firstname = strdup((__firstname) ? __firstname : "");

            const char *__lastname = gg_pubdir50_get(res, i, "lastname");
            char *lastname = strdup((__lastname) ? __lastname : "");

            const char *__nickname = gg_pubdir50_get(res, i, "nickname");
            char *nickname = strdup((__nickname) ? __nickname : "");

            const char *__fmstatus = gg_pubdir50_get(res, i, "fmstatus");
            int status = (__fmstatus) ? atoi(__fmstatus) : GG_STATUS_NOT_AVAIL;

            const char *__birthyear = gg_pubdir50_get(res, i, "birthyear");
            const char *birthyear = (__birthyear && strcmp(__birthyear, "0")) ? __birthyear : "-";

            const char *__city = gg_pubdir50_get(res, i, "city");
            char *city = strdup((__city) ? __city : "");

            char *name, *active;

            const char *target = NULL;

            cp_to_iso(firstname);
            cp_to_iso(lastname);
            cp_to_iso(nickname);
            cp_to_iso(city);

            if (count == 1 && !all) {
               free(last_search_first_name);
               free(last_search_last_name);
               free(last_search_nickname);
               last_search_first_name = strdup(firstname);
               last_search_last_name = strdup(lastname);
               last_search_nickname = strdup(nickname);
               last_search_uin = atoi(uin);
            }

            name = saprintf("%s %s", firstname, lastname);

            switch (status & 0x7f) {
               case GG_STATUS_AVAIL:
               case GG_STATUS_AVAIL_DESCR:
                  active = strdup("Avail");
                  break;
               case GG_STATUS_BUSY:
               case GG_STATUS_BUSY_DESCR:
                  active = strdup("Busy");
                  break;
               case GG_STATUS_INVISIBLE:
               case GG_STATUS_INVISIBLE_DESCR:
                  active = strdup("Invis");
                  break;
               default:
                  active = strdup("Inact");
            }

            printf("UIN\t: %s\n", uin);
            printf("Name\t: %s\n", name);
            printf("Nick\t: %s\n", nickname);
            printf("City\t: %s\n", city);
            printf("Birth\t: %s\n", birthyear);
            printf("Active\t: %s\n\n", active);

            free(name);
            free(active);

            free(firstname);
            free(lastname);
            free(nickname);
            free(city);

            last_uin = atoi(uin);
         }

         /* je¶li mieli¶my ,,/find --all'', szukamy dalej */
         for (l = searches; l; l = l->next) {
            gg_pubdir50_t req = l->data;
            uin_t next;

            if (gg_pubdir50_seq(req) != gg_pubdir50_seq(res))
               continue;

            /* nie ma dalszych? to dziêkujemy */
            if (!(next = gg_pubdir50_next(res)) || !sess || next < last_uin) {
               list_remove(&searches, req, 0);
               gg_pubdir50_free(req);
               break;
            }

            gg_pubdir50_add(req, GG_PUBDIR50_START, itoa(next));
            gg_pubdir50(sess, req);

            break;
         }
	 gg_free_event(e);
	 break;
      }

      gg_free_event(e);
   }

   gg_logoff(sess);
   gg_free_session(sess);

   return 0;
}

