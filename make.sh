gcc -std=gnu99 sources/serv_account*.c -o releases/serv_account -lpthread
gcc -std=gnu99 sources/clnt_auto_account_req.c -o releases/clnt_auto_account_req -lpthread
gcc -std=gnu99 sources/clnt_manuel_account_req.c -o releases/clnt_manuel_account_req -lpthread
gcc -std=gnu99 sources/serv_onethread.c sources/serv_account_manage_*.c -o releases/serv_onthread -lpthread
