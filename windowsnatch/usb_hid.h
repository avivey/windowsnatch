
int rawhid_open(int max, int vid, int pid, int usage_page, int usage);
int rawhid_recv(int num, void *buf, int len, int timeout);
int rawhid_send(int num, void *buf, int len, int timeout);
void rawhid_close(int num);

void rawhid_async_recv(int num, LPOVERLAPPED_COMPLETION_ROUTINE on_complete);
int rawhid_async_recv_complete(int num, void *buf, int len);
int rawhid_async_recv_cancel(int num);
