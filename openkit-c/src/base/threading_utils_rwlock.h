#ifndef _THREADING_UTILS_RWLOCK
#define _THREADING_UTILS_RWLOCK

typedef struct _threading_rw_lock
{
	void* platform_rw_lock;
} threading_rw_lock;

threading_rw_lock* init_rw_lock();
void destroy_rw_lock(threading_rw_lock* rw_lock);
void threading_rw_lock_lock_read(threading_rw_lock* rw_lock);
void threading_rw_lock_lock_write(threading_rw_lock* rw_lock);
void threading_rw_lock_unlock_read(threading_rw_lock* rw_lock);
void threading_rw_lock_unlock_write(threading_rw_lock* rw_lock);

#endif