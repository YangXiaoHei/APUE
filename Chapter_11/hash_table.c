#define NHASH 29
#define HASH(_id_) (((unsigned long)id) % NHASH)

/* 维护 hash_table 结构一致性 */
pthread_mutex_t structure_lock = PTHREAD_MUTEX_INITIALIZER;

struct foo {
    int _ref_count;
    int _id;
    char *_msg;
    struct foo *_next;
    /* 维护 foo 数据一致性 */
    pthread_mutex_t _content_lock;
};

struct foo *hash_table[NHASH];

struct foo *foo_alloc(int id, const char *msg) {
    struct foo *fp;
    int slot;
    if ((fp = malloc(sizeof(struct foo))) != NULL) {
        fp->_ref_count = 1;
        fp->_id = id;
        if (pthread_mutex_init(&fp->_content_lock, NULL) != 0) {
            free(fp);
            return NULL;
        }
        slot = HASH(id);
        /* 一旦插入到 hash_table，
           就会被 find 线程找到，所以先加结构锁 */
        pthread_mutex_lock(&structure_lock);
        fp->_next = hash_table[slot];
        hash_table[slot] = fp;
        
        /* 一旦释放结构锁，会被 find 线程引用，更改 ref_count
           所以先加数据锁，在释放结构锁 */
        pthread_mutex_lock(&fp->_content_lock);
        pthread_mutex_unlock(&structure_lock);
        
        /* 其他数据的初始化 */
        fp->_msg = malloc(strlen(msg) + 1);
        strncpy(fp->_msg, msg, strlen(msg));
        fp->_msg[strlen(msg)] = 0;
        
        /* 释放数据锁 */
        pthread_mutex_unlock(&fp->_content_lock);
        return fp;
    }
}

struct foo *foo_find(int id) {
    struct foo *fp;
    
    /* 遍历前其他线程正在 foo_alloc
       所以先加结构锁 */
    pthread_mutex_lock(&structure_lock);
    for (fp = hash_table[HASH(id)]; fp != NULL; fp = fp->_next) {
        if (fp->_id == id) {
            
            /* 可能是因为 foo_alloc 释放了结构锁才走到这里
               但是要修改 ref_count，所以再加数据锁 */
            
            /* 问题 ： foo_find 线程可能先于 foo_alloc
               线程加锁吗？答：不可能，因为 foo_alloc 在
               释放结构锁前先加了数据锁 */
            pthread_mutex_lock(&fp->_content_lock);
            fp->_ref_count++;
            pthread_mutex_unlock(&fp->_content_lock);
            break;
        }
    }
    pthread_mutex_unlock(&structure_lock);
    return fp;
}

void foo_release(struct foo *fp) {
    
    struct foo *prev_fp;
    int slot;
    
    pthread_mutex_lock(&fp->_content_lock);
    if (fp->_ref_count == 1) {
        
        /*  举两个例子。
         
         1⃣️ 线程 A 调用 foo_alloc ，线程 B 调用 foo_release。
         A 先加结构锁并在哈希表中插入了 foo 但还没来得及对 foo 加数据锁，
         此时线程 B 已经可以通过哈希表引用到 foo，
         并先于 A 对 foo 加数据锁，然后 A 抢不到 foo 的数据锁挂起，
         B 抢不到结构锁而挂起。
         
         2⃣️ 线程 A 调用 foo_find，线程 B 调用 foo_release。
         A 先加结构锁开始遍历哈希表，在 A 遍历到 foo 这一项前，
         B 抢先对 foo 加数据锁，然后因为抢不到结构锁挂起，
         当 A 遍历到 foo 时，因为抢不到 foo 的数据锁挂起，
         
         以上两种情况都造成死锁 */
        
        /* 👇 */
        pthread_mutex_unlock(&fp->_content_lock);
        pthread_mutex_lock(&structure_lock);
        pthread_mutex_lock(&fp->_content_lock);
        /* 👆 */
        
        if (fp->_ref_count != 1) {
            fp->_ref_count--;
            pthread_mutex_unlock(&fp->_content_lock);
            pthread_mutex_unlock(&structure_lock);
            return;
        }
        
        slot = HASH(fp->_id);
        prev_fp = hash_table[slot];
        if (prev_fp == fp) {
            hash_table[slot] = fp->_next;
        } else {
            while (prev_fp->_next != fp)
                prev_fp = prev_fp->_next;
            prev_fp->_next = fp->_next;
        }
        
        pthread_mutex_unlock(&structure_lock);
        pthread_mutex_unlock(&fp->_content_lock);
        pthrea_mutex_destroy(&fp->_content_lock);
        free(fp);
    } else {
        fp->_ref_count--;
        pthread_mutex_unlock(&fp->_content_lock);
    }
}
