gcc -std=gnu99 server/serv_manage*.c -o releases/serv_account -lpthread
gcc -std=gnu99 server/serv_onethread.c server/serv_manage_user*.c -o releases/serv_onthread -lpthread
gcc -std=gnu99 client/clnt_auto_req.c -o releases/clnt_auto_req -lpthread
gcc -std=gnu99 client/clnt_manuel_req.c -o releases/clnt_manuel_account_req -lpthread

